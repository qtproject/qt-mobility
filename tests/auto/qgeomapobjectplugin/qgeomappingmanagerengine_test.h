/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QGEOMAPPINGMANAGERENGINE_TEST_H
#define QGEOMAPPINGMANAGERENGINE_TEST_H

#include <qgeoserviceprovider.h>
#include <qgeotiledmappingmanagerengine.h>
#include <qgeotiledmapreply.h>
#include <qgeotiledmaprequest.h>

QTM_USE_NAMESPACE

class QGeoMapReplyTest : public QGeoTiledMapReply
{
    Q_OBJECT
public:
    QGeoMapReplyTest(const QGeoTiledMapRequest &request, QObject *parent = 0)
        : QGeoTiledMapReply(request, parent)
    {
        setFinished(true);
    }

    ~QGeoMapReplyTest() {}
};

class QGeoMappingManagerEngineTest: public QGeoTiledMappingManagerEngine

{
    Q_OBJECT
public:
    QGeoMappingManagerEngineTest(const QMap<QString, QVariant> &parameters,
        QGeoServiceProvider::Error *error, QString *errorString) :
        QGeoTiledMappingManagerEngine(parameters)
    {
        Q_UNUSED(error)
        Q_UNUSED(errorString)
        setTileSize(QSize(256, 256));
        setMinimumZoomLevel(0.0);
        setMaximumZoomLevel(18.0);

        QList<QGraphicsGeoMap::MapType> types;
        types << QGraphicsGeoMap::StreetMap;
        types << QGraphicsGeoMap::SatelliteMapDay;
        types << QGraphicsGeoMap::TerrainMap;
        setSupportedMapTypes(types);

        QList<QGraphicsGeoMap::ConnectivityMode> modes;
        modes << QGraphicsGeoMap::OnlineMode;
        setSupportedConnectivityModes(modes);
    }

    QGeoTiledMapReply* getTileImage(const QGeoTiledMapRequest &request)
    {
        return new QGeoMapReplyTest(request, this);
    }
};

#endif
