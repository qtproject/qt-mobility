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

#include <QTreeWidget>
#include <QLineEdit>
#include <QString>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

#include "revgeocodingtab.h"

#include "placepresenter.h"
#include "qgeocodingservice_nokia_p.h"
#include "qsearchrequest.h"

ReverseGeocodingTab::ReverseGeocodingTab(QWidget *parent) :
    QWidget(parent)
{
    QGeocodingServiceNokia *service = new QGeocodingServiceNokia();
    service->setProxy( QNetworkProxy(QNetworkProxy::HttpProxy, "172.16.42.137", 8080) );
    service->setHost("loc.desktop.maps.svc.ovi.com");

    geocodingService = service;
    searchController=new QSearchControllerNokia(geocodingService);
    QObject::connect(searchController, SIGNAL(searchRequestFinished(QSearchResponse*)),
                     this, SLOT(replyFinished(QSearchResponse*)));

    QLabel *locationlbl = new QLabel(tr("Location:"));
    locLong = new QLineEdit("13.377");
    locLat = new QLineEdit("52.51");

    QPushButton *requestBtn = new QPushButton(tr("Reverse Geocoding"));
    QObject::connect(requestBtn, SIGNAL(clicked(bool)), this, SLOT(on_btnRequest_clicked()));

    resultTree = new QTreeWidget();
    QStringList labels;
    labels << "Elements" << "Value";
    resultTree->setHeaderLabels(labels);
    resultTree->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    resultTree->setColumnWidth(0, 150);

    QHBoxLayout *firstrow = new QHBoxLayout;
    firstrow->addWidget(locationlbl);
    firstrow->addWidget(locLong);
    firstrow->addWidget(locLat);
    firstrow->addWidget(requestBtn);
    firstrow->addStretch(1);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(firstrow);
    mainLayout->addWidget(resultTree);
    mainLayout->addStretch(1);
    setLayout(mainLayout);
}

ReverseGeocodingTab::~ReverseGeocodingTab()
{
    delete searchController;
}

void ReverseGeocodingTab::on_btnRequest_clicked()
{
    QGeoCoordinate coord(locLat->text().toDouble(), locLong->text().toDouble());
    QSearchRequest request;
    request.locationFilter().setCoordinate(coord);
    resultTree->clear();

    searchController->reverseGeocode(request);
}

void ReverseGeocodingTab::replyFinished(QSearchResponse* reply)
{
    PlacePresenter presenter(resultTree, reply);
    presenter.show();
}
