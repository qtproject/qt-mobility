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

#include "servicestab.h"
#include "placepresenter.h"

#include <QTreeWidget>
#include <QLineEdit>
#include <QString>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QTimer>
#include <QGeoServiceProvider>

ServicesTab::ServicesTab(QWidget *parent) :
        QWidget(parent)
{
    m_requestBtn = new QPushButton(tr("Set Service Provider"));
    m_requestBtn->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
    QObject::connect(m_requestBtn, SIGNAL(clicked(bool)), this, SLOT(on_btnRequest_clicked()));

    m_resultTree = new QTreeWidget();
    QStringList labels;
    labels << tr("Services") << tr("Available");
    m_resultTree->setHeaderLabels(labels);
    m_resultTree->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_resultTree->setColumnWidth(0, 275);

    QHBoxLayout *firstrow = new QHBoxLayout;
    firstrow->setSpacing(0);
    firstrow->setContentsMargins(0, 0, 0, 0);
    firstrow->addWidget(m_requestBtn);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setSpacing(2);
    mainLayout->setContentsMargins(2, 1, 2, 1);
    mainLayout->addLayout(firstrow);
    mainLayout->addWidget(m_resultTree);
    setLayout(mainLayout);

    m_requestBtn->setDisabled(true);
}

void ServicesTab::initialize()
{
    QTreeWidgetItem* top = new QTreeWidgetItem(m_resultTree);
    top->setText(0, tr("Loading Service Providers..."));

    listServiceProviders();

    if (m_resultTree->topLevelItemCount() == 0) {
        m_requestBtn->setDisabled(true);
        QTreeWidgetItem* top = new QTreeWidgetItem(m_resultTree);
        top->setText(0, tr("No providers available"));
    } else {
        m_requestBtn->setDisabled(false);
        on_btnRequest_clicked();
    }
}

ServicesTab::~ServicesTab()
{
}

void ServicesTab::on_btnRequest_clicked()
{
    if (m_resultTree->currentItem() == NULL) {
        if (m_resultTree->topLevelItemCount() > 0)
            m_resultTree->setCurrentItem(m_resultTree->topLevelItem(0));
        else
            return;
    }

    QTreeWidgetItem *serviceItem = m_resultTree->currentItem();
    while (serviceItem->parent() != NULL)
        serviceItem = m_resultTree->currentItem()->parent();

    QString serviceId = serviceItem->text(0);
    emit serviceProviderChanged(serviceId);

    for (int i = 0; i < m_resultTree->topLevelItemCount(); ++i) {
        QTreeWidgetItem* top = m_resultTree->topLevelItem(i);
        if (top->text(0) == serviceId)
            top->setText(1, tr("CurrentProvider"));
        else
            top->setText(1, tr(""));
    }
}

void ServicesTab::listServiceProviders()
{
    m_resultTree->clear();
    QList<QString> providers = QGeoServiceProvider::availableServiceProviders();

    QString providerId;
    foreach(providerId, providers) {
        QTreeWidgetItem* top = new QTreeWidgetItem(m_resultTree);
        top->setText(0, providerId);
        QGeoServiceProvider* serviceProvider = new QGeoServiceProvider(providerId);

        QTreeWidgetItem* prop = new QTreeWidgetItem(top);
        prop->setText(0, "Places");
        QGeoPlacesManager* placesManager = serviceProvider->placesManager();
        if (placesManager)
            prop->setText(1, tr("true"));
        else
            prop->setText(1, tr("false"));

        prop = new QTreeWidgetItem(top);
        prop->setText(0, "Mapping");
        QGeoMappingManager* mappingManager = serviceProvider->mappingManager();
        if (mappingManager)
            prop->setText(1, tr("true"));
        else
            prop->setText(1, tr("false"));

        prop = new QTreeWidgetItem(top);
        prop->setText(0, "Routing");
        QGeoRoutingManager* routingManager = serviceProvider->routingManager();
        if (routingManager)
            prop->setText(1, tr("true"));
        else
            prop->setText(1, tr("false"));

        delete serviceProvider;
    }
}
