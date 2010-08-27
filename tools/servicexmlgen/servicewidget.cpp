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
#include "servicewidget.h"
#include "interfacestabwidget.h"
#include "mandatorylineedit.h"
#include "errorcollector.h"

#include <servicemetadata_p.h>

#include <QLabel>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QDebug>

//QTM_USE_NAMESPACE

ServiceWidget::ServiceWidget(QWidget *parent)
    : QWidget(parent),
      m_ifacesTabs(new InterfacesTabWidget),
      m_title(new QLabel),
      m_name(new MandatoryLineEdit(tr("(Name required)"))),
      m_path(new MandatoryLineEdit(tr("(Path required)"))),
      m_desc(new QLineEdit)
{
    connect(m_name, SIGNAL(textChanged(QString)), SLOT(setTitle(QString)));

    connect(m_name, SIGNAL(textEdited(QString)), SIGNAL(dataChanged()));
    connect(m_path, SIGNAL(textEdited(QString)), SIGNAL(dataChanged()));
    connect(m_desc, SIGNAL(textEdited(QString)), SIGNAL(dataChanged()));
    connect(m_ifacesTabs, SIGNAL(dataChanged()), SIGNAL(dataChanged()));

    QFont f = m_title->font();
    f.setBold(true);
    m_title->setFont(f);
    setTitle(QString());

    QPushButton *buttonAdd = new QPushButton(tr("Add interface"));
    buttonAdd->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    connect(buttonAdd, SIGNAL(clicked()), m_ifacesTabs, SLOT(addInterface()));

    QGroupBox *serviceAttrsGroup = new QGroupBox(tr("Main attributes"));
    QFormLayout *form = new QFormLayout;
    form->addRow(tr("* Name"), m_name);
    form->addRow(tr("* Resource path"), m_path);
    form->addRow(tr("Description"), m_desc);
    serviceAttrsGroup->setLayout(form);

    QGroupBox *interfaceTabsGroup = new QGroupBox(tr("Interfaces (must have at least one)"));
    QVBoxLayout *groupLayout = new QVBoxLayout;
    groupLayout->addWidget(m_ifacesTabs);
    groupLayout->addWidget(buttonAdd);
    interfaceTabsGroup->setLayout(groupLayout);

    QVBoxLayout *main = new QVBoxLayout;
    main->addWidget(m_title, 0, Qt::AlignHCenter);
    main->addWidget(serviceAttrsGroup);
    main->addWidget(interfaceTabsGroup);
    main->addWidget(new QLabel(tr("* Denotes mandatory attributes.")), 0, Qt::AlignHCenter);
    setLayout(main);
}

void ServiceWidget::load(const QTM_PREPEND_NAMESPACE(ServiceMetaDataResults) &data)
{
    m_name->setText(data.name);
    m_path->setText(data.location);
    m_desc->setText(data.description);
    m_ifacesTabs->load(data.interfaces);

    m_name->setFocus();
}

void ServiceWidget::validate(ErrorCollector *errors)
{
    m_name->validate(errors);
    m_path->validate(errors);
    m_ifacesTabs->validate(errors);
}

void ServiceWidget::writeXml(QXmlStreamWriter *writer) const
{
    writer->writeTextElement(QLatin1String("name"), !m_name->hasText() ? QString() : m_name->text());
    writer->writeTextElement(QLatin1String("filepath"), !m_path->hasText() ? QString() : m_path->text());
    writer->writeTextElement(QLatin1String("description"), m_desc->text());
    m_ifacesTabs->writeXml(writer);
}

void ServiceWidget::setTitle(const QString &text)
{
    if (text.isEmpty())
        m_title->setText(tr("[New Service]"));
    else
        m_title->setText(text);
}
