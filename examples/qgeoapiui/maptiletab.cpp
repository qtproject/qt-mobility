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

#include "maptiletab.h"
#include "qmaptileservice_nokia_p.h"

MapTileTab::MapTileTab(QWidget *parent) 
    : QWidget(parent)
{
    QMapTileServiceNokia *service = new QMapTileServiceNokia();
    QNetworkProxy proxy(QNetworkProxy::HttpProxy, "172.16.42.40", 8080);
    service->setHost("origin.maptile.svc.tst.s2g.gate5.de");
    //service->setHost("maptile.maps.svc.ovi.com");

    mapService = service;
    QObject::connect(mapService, SIGNAL(finished(QMapTileReply*)), 
                     this, SLOT(replyFinished(QMapTileReply*)));
    
    QLabel *coordlbl = new QLabel(tr("Coordinates:"));
    tileLong = new QLineEdit("-74");
    tileLat = new QLineEdit("40.7");
    QLabel *zoomlbl = new QLabel(tr("Zoom:"));
    tileZoomLevel = new QLineEdit("8");

    QPushButton *requestBtn = new QPushButton(tr("Request Map Tile"));
    QObject::connect(requestBtn, SIGNAL(clicked(bool)), this, SLOT(on_btnRequest_clicked()));
    
    result = new QLabel(tr("MapTile"));

    QHBoxLayout *firstrow = new QHBoxLayout;
    firstrow->addWidget(coordlbl);
    firstrow->addWidget(tileLong);
    firstrow->addWidget(tileLat);
    firstrow->addStretch(1);
    
    QHBoxLayout *secondrow = new QHBoxLayout;
    secondrow->addWidget(zoomlbl);
    secondrow->addWidget(tileZoomLevel);
    secondrow->addWidget(requestBtn);
    secondrow->addStretch(1);
    
    QHBoxLayout *resultrow = new QHBoxLayout;
    resultrow->addWidget(result);
    resultrow->addStretch(1);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(firstrow);
    mainLayout->addLayout(secondrow);
    mainLayout->addLayout(resultrow);
    mainLayout->addStretch(1);
    setLayout(mainLayout);
}

MapTileTab::~MapTileTab()
{
    delete mapService;
}

void MapTileTab::on_btnRequest_clicked()
{
    QGeoCoordinate coord(tileLat->text().toDouble(),
                         tileLong->text().toDouble()
                        );
    quint16 zoomLevel = tileZoomLevel->text().toInt();

    quint32 col;
    quint32 row;
    mapService->getMercatorTileIndex(coord, zoomLevel, &row, &col);
    mapService->request(zoomLevel, row, col);
}

void MapTileTab::replyFinished(QMapTileReply* reply)
{
    QPixmap pixmap;
    pixmap.loadFromData(reply->data());
    result->setPixmap(pixmap);
    reply->deleteLater();
}
