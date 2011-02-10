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
#include <qgeomappingmanagerengine.h>
#include <qgeomapdata.h>
#include <qgeocoordinate.h>
#include <qgeoboundingbox.h>
#include <qgeomapobjectinfo.h>
#include <qgeomapobject.h>
#include <qgeomaprectangleobject.h>

QTM_USE_NAMESPACE
class QGeoMapObjectInfoTest: public QGeoMapObjectInfo
{
public:
    QGeoMapObjectInfoTest(QGeoMapData *mapData, QGeoMapObject *mapObject) :
        QGeoMapObjectInfo(mapData, mapObject)
    {
        if (mapObject->type() == QGeoMapObject::RectangleType)
            m_bbox = qobject_cast<QGeoMapRectangleObject*> (mapObject)->bounds();
    }

    QGeoBoundingBox boundingBox() const
    {
        return m_bbox;
    }

    bool contains(const QGeoCoordinate &coordinate) const
    {
        return boundingBox().contains(coordinate);
    }
private:
    QGeoBoundingBox m_bbox;
};
class QGeoMapDataTest: public QGeoMapData
{
Q_OBJECT
public:
    QGeoMapDataTest(QGeoMappingManagerEngine *engine) :
        QGeoMapData(engine)
    {
    }

    virtual QGeoBoundingBox viewport() const
    {
        return QGeoBoundingBox(QGeoCoordinate(50.0,0.0),
                               QGeoCoordinate(0.0,50.0));
    }

    virtual void fitInViewport(const QGeoBoundingBox &bounds, bool preserveViewportCenter = false)
    {
        if (!preserveViewportCenter)
            setCenter(bounds.center());
    }

    QGeoMapObjectInfo* createMapObjectInfo(QGeoMapObject *object)
    {
        return new QGeoMapObjectInfoTest(this, object);
    }

    virtual QPointF coordinateToScreenPosition(const QGeoCoordinate &coordinate) const
    {
        return QPointF(coordinate.longitude(), coordinate.latitude());
    }
    virtual QGeoCoordinate screenPositionToCoordinate(const QPointF &screenPosition) const
    {
        return QGeoCoordinate(screenPosition.y(), screenPosition.x());
    }
};

class QGeoMappingManagerEngineTest: public QGeoMappingManagerEngine
{
Q_OBJECT
public:
    QGeoMappingManagerEngineTest(const QMap<QString, QVariant> &parameters,
        QGeoServiceProvider::Error *error, QString *errorString) :
        QGeoMappingManagerEngine(parameters)
    {
        Q_UNUSED(error)
        Q_UNUSED(errorString)
        setMinimumZoomLevel(10.0);
        setMaximumZoomLevel(20.0);
        setMaximumTilt(90.0);
        setMinimumTilt(0.0);
        setSupportsBearing(true);
        setSupportsTilting(true);
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
        QGeoMapData *data = new QGeoMapDataTest(this);
        data->setConnectivityMode(QGraphicsGeoMap::OfflineMode);
        data->setMapType(QGraphicsGeoMap::StreetMap);
        return data;
    }
};

#endif
