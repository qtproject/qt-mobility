/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include "interfacestabwidget.h"
#include "interfacewidget.h"
#include "errorcollector.h"

#include <qserviceinterfacedescriptor.h>

#include <QVBoxLayout>
#include <QPushButton>
#include <QTabBar>
#include <QDebug>

InterfacesTabWidget::InterfacesTabWidget(QWidget *parent)
    : QTabWidget(parent)
{
    connect(tabBar(), SIGNAL(tabCloseRequested(int)), SLOT(tabCloseRequested(int)));
}

void InterfacesTabWidget::load(const QList<QTM_PREPEND_NAMESPACE(QServiceInterfaceDescriptor)> &descriptors)
{
    QWidget *widg;
    while (count() > 0) {
        widg = widget(count()-1);
        if (widg) {
            removeTab(count()-1);
            delete widg;
        }
    }

    InterfaceWidget *iface;
    for (int i=0; i<descriptors.count(); i++)
        iface = addInterface(descriptors[i]);

    if (count() == 0)
        addInterface(); // blank placeholder interface
    setCurrentIndex(0);
}

void InterfacesTabWidget::validate(ErrorCollector *errors)
{
    int firstInvalid = -1;
    QHash<QString, int> titleCount;
    for (int i=0; i<count(); i++) {
        InterfaceWidget *ifaceWidget = qobject_cast<InterfaceWidget*>(widget(i));
        int errCountBefore = errors->errorCount();
        ifaceWidget->validate(errors);
        if (errors->errorCount() > errCountBefore) {
            if (firstInvalid < 0)
                firstInvalid = i;
            tabBar()->setTabTextColor(i, Qt::red);
        } else {
            QString title = ifaceWidget->title();
            if (titleCount.contains(title)) {
                titleCount[title]++;
            } else {
                titleCount[title] = 1;
            }
        }
    }

    QHashIterator<QString, int> i(titleCount);
    while (i.hasNext()) {
        if (i.next().value() > 1) {
            errors->addError(tr("Found multiple implementations for interface \"%1\". A service cannot have multiple implementations of the same interface with the same version.", "interface name and version").arg(i.key()));
        }
    }

    if (firstInvalid >= 0)
        setCurrentIndex(firstInvalid);
}

void InterfacesTabWidget::writeXml(QXmlStreamWriter *writer) const
{
    for (int i=0; i<count(); i++) {
        InterfaceWidget *ifaceWidget = qobject_cast<InterfaceWidget*>(widget(i));
        if (ifaceWidget) {
            writer->writeStartElement(QLatin1String("interface"));
            ifaceWidget->writeXml(writer);
            writer->writeEndElement();
        }
    }
}

InterfaceWidget *InterfacesTabWidget::addInterface(const QTM_PREPEND_NAMESPACE(QServiceInterfaceDescriptor) &descriptor)
{
    InterfaceWidget *w = new InterfaceWidget;
    connect(w, SIGNAL(titleChanged(QString)), SLOT(interfaceTitleChanged(QString)));
    connect(w, SIGNAL(dataChanged()), SIGNAL(dataChanged()));
    if (descriptor.isValid())
        w->load(descriptor);
    addTab(w, w->title());
    setCurrentWidget(w);
    w->init();
    return w;
}

void InterfacesTabWidget::tabInserted(int)
{
    if (count() > 1)
        tabBar()->setTabsClosable(true);
    emit dataChanged();
}

void InterfacesTabWidget::tabRemoved(int)
{
    if (count() == 1)
        tabBar()->setTabsClosable(false);
    emit dataChanged();
}

void InterfacesTabWidget::tabCloseRequested(int index)
{
    QWidget *w = widget(index);
    if (w) {
        removeTab(index);
        delete w;
    }
}

void InterfacesTabWidget::interfaceTitleChanged(const QString &text)
{
    InterfaceWidget *tab = qobject_cast<InterfaceWidget*>(sender());
    if (tab) {
        int index = indexOf(tab);
        if (index >= 0) {
            setTabText(index, text);
            tabBar()->setTabTextColor(index, QPalette().text().color());
        }
    }
}
