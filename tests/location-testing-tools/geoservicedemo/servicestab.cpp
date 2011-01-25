/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
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
#include <qgeoserviceprovider.h>

ServicesTab::ServicesTab(QWidget *parent) :
        QWidget(parent)
{
    m_requestBtn = new QPushButton(tr("Set Service Provider"));
    m_requestBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    QObject::connect(m_requestBtn, SIGNAL(clicked(bool)), this, SLOT(on_btnRequest_clicked()));

    m_resultTree = new QTreeWidget();
    QStringList labels;
    labels << tr("Services") << tr("Available");
    m_resultTree->setHeaderLabels(labels);
    m_resultTree->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_resultTree->setColumnWidth(0, 275);

    QHBoxLayout *firstrow = new QHBoxLayout;
    firstrow->setSpacing(2);
    firstrow->setContentsMargins(2, 1, 2, 1);
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

        QTreeWidgetItem* searchItem = new QTreeWidgetItem(top);
        searchItem->setText(0, "Search");
        QGeoSearchManager* searchManager = serviceProvider->searchManager();
        if (searchManager)
            searchItem->setText(1, tr("true"));
        else
            searchItem->setText(1, tr("false"));

        QTreeWidgetItem* mappingItem = new QTreeWidgetItem(top);
        mappingItem->setText(0, "Mapping");
        QGeoMappingManager* mappingManager = serviceProvider->mappingManager();
        if (mappingManager)
            mappingItem->setText(1, tr("true"));
        else
            mappingItem->setText(1, tr("false"));

        QTreeWidgetItem* routingItem = new QTreeWidgetItem(top);
        routingItem->setText(0, "Routing");
        QGeoRoutingManager* routingManager = serviceProvider->routingManager();
        if (routingManager)
            routingItem->setText(1, tr("true"));
        else
            routingItem->setText(1, tr("false"));

        delete serviceProvider;
    }
}
