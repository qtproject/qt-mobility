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

#include "geocodingtab.h"

#include "placepresenter.h"
#include "qsearchrequest_nokia_p.h"
#include "qgeocodingservice_nokia_p.h"

GeocodingTab::GeocodingTab(QWidget *parent) :
    QWidget(parent)
{
    QGeocodingServiceNokia *service = new QGeocodingServiceNokia();
    service->setProxy( QNetworkProxy(QNetworkProxy::HttpProxy, "172.16.42.137", 8080) );
    service->setHost("loc.desktop.maps.svc.ovi.com");

    geocodingService = service;
    searchController=new QSearchControllerNokia(geocodingService);
    
    QObject::connect(searchController, SIGNAL(searchRequestFinished(QSearchResponse*)),
                     this, SLOT(replyFinished(QSearchResponse*)));

    obloc = new QLineEdit("Deutschland, München");
    QLabel *countrylbl = new QLabel(tr("Country:"));
    country = new QLineEdit("");
    QLabel *statelbl = new QLabel(tr("State:"));
    state = new QLineEdit("");
    QLabel *citylbl = new QLabel(tr("City:"));
    city = new QLineEdit("");
    QLabel *ziplbl = new QLabel(tr("Zip:"));
    zip = new QLineEdit("");
    QLabel *streetlbl = new QLabel(tr("street:"));
    street = new QLineEdit("");
    QLabel *streetnumlbl = new QLabel(tr("#:"));
    streetNumber = new QLineEdit("");

    QPushButton *requestBtn = new QPushButton(tr("Geocoding"));
    QObject::connect(requestBtn, SIGNAL(clicked(bool)), this, SLOT(on_btnRequest_clicked()));

    resultTree = new QTreeWidget();
    QStringList labels;
    labels << "Elements" << "Value";
    resultTree->setHeaderLabels(labels);
    resultTree->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    resultTree->setColumnWidth(0, 150);

    QHBoxLayout *firstrow = new QHBoxLayout;
    firstrow->addWidget(obloc);
    firstrow->addWidget(requestBtn);
    firstrow->addStretch(1);

    QHBoxLayout *secondrow = new QHBoxLayout;
    secondrow->addWidget(streetlbl);
    secondrow->addWidget(street);
    secondrow->addWidget(streetnumlbl);
    secondrow->addWidget(streetNumber);
    secondrow->addWidget(ziplbl);
    secondrow->addWidget(zip);
    secondrow->addStretch(1);

    QHBoxLayout *thirdrow = new QHBoxLayout;
    thirdrow->addWidget(citylbl);
    thirdrow->addWidget(city);
    thirdrow->addWidget(statelbl);
    thirdrow->addWidget(state);
    thirdrow->addWidget(countrylbl);
    thirdrow->addWidget(country);
    thirdrow->addStretch(1);
    
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(firstrow);
    mainLayout->addLayout(secondrow);
    mainLayout->addLayout(thirdrow);
    mainLayout->addWidget(resultTree);
    mainLayout->addStretch(1);
    setLayout(mainLayout);
}

GeocodingTab::~GeocodingTab()
{
    delete searchController;
}

void GeocodingTab::on_btnRequest_clicked()
{
    QSearchRequestNokia request;
    QString s = obloc->text();

    if (!s.isEmpty()) {
        request.setOneBoxLocation(s);
    }
    else {
        request.setCountry(country->text());
        request.setState(state->text());
        request.setCity(city->text());
        request.setPostCode(zip->text());
        request.setStreet(street->text());
        request.setNumber(streetNumber->text());
    }
    resultTree->clear();
    searchController->geocode(request);
}

void GeocodingTab::testReplyFinishedSignal()
{
    int x = 1;
    x++;
}

void GeocodingTab::replyFinished(QSearchResponse* reply)
{
    PlacePresenter presenter(resultTree, reply);
    presenter.show();
}
