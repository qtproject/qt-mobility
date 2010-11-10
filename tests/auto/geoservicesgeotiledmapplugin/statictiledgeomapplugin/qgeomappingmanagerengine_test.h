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

#ifndef QGEOMAPPINGMANAGERENGINE_TEST_H
#define QGEOMAPPINGMANAGERENGINE_TEST_H

#include <qgeoserviceprovider.h>
#include <qgeotiledmappingmanagerengine.h>
#include <qgeomapdata.h>
#include <QTimer>
#include <QBuffer>
#include <QFile>
#include <QResource>

QTM_USE_NAMESPACE

class QGeoMapReplyTest: public QGeoTiledMapReply
{
Q_OBJECT

public:
    QGeoMapReplyTest(const QGeoTiledMapRequest &request, QObject *parent = 0) :
        QGeoTiledMapReply(request, parent)
    {
        m_finishedTimer.setSingleShot(true);
        m_finishedTimer.setInterval(1);

        if (request == QGeoTiledMapRequest()) {
            connect(&m_finishedTimer, SIGNAL(timeout()), this, SLOT(replyerror()));
            m_finishedTimer.start();
            return;
        }

        Q_INIT_RESOURCE( geoservicestestmaptiles);
        m_tileSize = QSize(256, 256);
        setCached(true);
        connect(&m_finishedTimer, SIGNAL(timeout()), this, SLOT(replyfinished()));
        m_finishedTimer.start();
    }

    ~QGeoMapReplyTest()
    {
        m_finishedTimer.stop();
    }
    ;

    void abort()
    {
        m_finishedTimer.stop();
        QGeoTiledMapReply::abort();
    }

private slots:
    void replyfinished()
    {
        QFile tile(":/tiles/png8.png");
        tile.open(QFile::ReadOnly);
        setMapImageData(tile.readAll());
        tile.close();
        setMapImageFormat("PNG");
        setFinished(true);
    }

    void replyerror()
    {
        setError(QGeoTiledMapReply::UnknownError, "Error");
    }
private:
    QSize m_tileSize;
    QTimer m_finishedTimer;
};

class QGeoTiledMappingManagerEngineTest: public QGeoTiledMappingManagerEngine
{
Q_OBJECT
public:
    QGeoTiledMappingManagerEngineTest(const QMap<QString, QVariant> &parameters,
        QGeoServiceProvider::Error *error, QString *errorString) :
        QGeoTiledMappingManagerEngine(parameters)
    {
        Q_UNUSED(error)
        Q_UNUSED(errorString)
        setMinimumZoomLevel(0.0);
        setMaximumZoomLevel(18.0);
        setTileSize(QSize(256, 256));
        QList<QGraphicsGeoMap::ConnectivityMode> modes;
        modes << QGraphicsGeoMap::OfflineMode;
        modes << QGraphicsGeoMap::OnlineMode;
        modes << QGraphicsGeoMap::HybridMode;
        setSupportedConnectivityModes(modes);

        QList<QGraphicsGeoMap::MapType> types;
        types << QGraphicsGeoMap::StreetMap;
        types << QGraphicsGeoMap::SatelliteMapDay;
        types << QGraphicsGeoMap::TerrainMap;
        setSupportedMapTypes(types);
    }

    QGeoMapData* createMapData()
    {
        QGeoMapData *data = QGeoTiledMappingManagerEngine::createMapData();
        data->setConnectivityMode(QGraphicsGeoMap::OfflineMode);
        data->setMapType(QGraphicsGeoMap::StreetMap);
        return data;
    }
    QGeoTiledMapReply* getTileImage(const QGeoTiledMapRequest &request)
    {
        return new QGeoMapReplyTest(request);
    }

};

#endif
