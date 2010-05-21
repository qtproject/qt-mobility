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

#include "maptiletab.h"

#include <QTreeWidget>
#include <QLineEdit>
#include <QString>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMessageBox>
#include <qgeomappingmanager.h>
#include <qgeomaprequestoptions.h>

MapTileTab::MapTileTab(QWidget *parent) 
    : QWidget(parent),
    m_mapManager(NULL)
{
    QLabel *coordlbl = new QLabel(tr("Coordinates:"));
    m_tileLong = new QLineEdit("-74");
    m_tileLong->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    m_tileLat = new QLineEdit("40.7");
    m_tileLat->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    QLabel *zoomlbl = new QLabel(tr("Zoom:"));
    m_tileZoomLevel = new QLineEdit("8");
    m_tileZoomLevel->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);

    QPushButton *requestBtn = new QPushButton(tr("Request Map Tile"));
    requestBtn->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Fixed);
    QObject::connect(requestBtn, SIGNAL(clicked(bool)), this, SLOT(on_btnRequest_clicked()));
    
    m_result = new QLabel(tr("MapTile"));

    QHBoxLayout *firstrow = new QHBoxLayout;
    firstrow->setSpacing(0);
    firstrow->setContentsMargins(0,0,0,0);
    firstrow->addWidget(coordlbl);
    firstrow->addWidget(m_tileLong);
    firstrow->addWidget(m_tileLat);
    
    QHBoxLayout *secondrow = new QHBoxLayout;
    secondrow->setSpacing(0);
    secondrow->setContentsMargins(0,0,0,0);
    secondrow->addWidget(zoomlbl);
    secondrow->addWidget(m_tileZoomLevel);
    secondrow->addWidget(requestBtn);
    
    QHBoxLayout *resultrow = new QHBoxLayout;
    resultrow->setSpacing(0);
    resultrow->setContentsMargins(0,0,0,0);
    resultrow->addWidget(m_result);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setSpacing(2);
    mainLayout->setContentsMargins(2,1,2,1);
    mainLayout->addLayout(firstrow);
    mainLayout->addLayout(secondrow);
    mainLayout->addLayout(resultrow);
    setLayout(mainLayout);
}

MapTileTab::~MapTileTab()
{
}

void MapTileTab::initialize(QGeoMappingManager *mapManager)
{
    m_mapManager = mapManager;
    if (m_mapManager) {
        QObject::connect(m_mapManager, SIGNAL(finished(QGeoMapReply*)), this,
            SLOT(replyFinished(QGeoMapReply*)));
        QObject::connect(m_mapManager,
            SIGNAL(error(QGeoMapReply*,QGeoMapReply::ErrorCode,QString)), this,
            SLOT(resultsError(QGeoMapReply*,QGeoMapReply::ErrorCode,QString)));
    }
}

void MapTileTab::on_btnRequest_clicked()
{
    if (m_mapManager) {
        m_result->setText(tr("Loading map tile"));

        QGeoCoordinate coord(m_tileLat->text().toDouble(), m_tileLong->text().toDouble());
        quint16 zoomLevel = m_tileZoomLevel->text().toInt();
        if(!m_mapManager->requestTile(coord, zoomLevel, QSize(256,256), QGeoMapRequestOptions()))
            m_result->setText(tr("Error - requestMap returned NULL"));        
    }
    else {
        QMessageBox::warning(this, tr("MapTile"), tr("No mapping manager available."));
    }
}

void MapTileTab::replyFinished(QGeoMapReply* reply)
{
    m_result->setPixmap(reply->mapImage());
    reply->deleteLater();
}

void MapTileTab::resultsError(QGeoMapReply* reply, QGeoMapReply::Error error, QString errorString)
{
    m_result->setText(tr("Error ")+errorString);
    reply->deleteLater();
}
