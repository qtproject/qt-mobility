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
#ifndef QGEOMAPWIDGET_H
#define QGEOMAPWIDGET_H

#include "qmobilityglobal.h"

#include <QGraphicsWidget>
#include <QList>

class QGraphicsItem;
class QPointF;
class QRectF;

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

class QGeoCoordinate;
class QGeoBoundingBox;
class QGeoMapObject;
class QGeoMappingService;

class QGeoMapWidgetPrivate;

class Q_LOCATION_EXPORT QGeoMapWidget : public QGraphicsWidget
{
    Q_OBJECT

public:
    // TODO - is there a better place to put this?
    enum DistanceUnits {
        Metres,
        Kilometres,
        Miles
    };

    static double convertDistance(double value, DistanceUnits from, DistanceUnits to);

    QGeoMapWidget(QGraphicsItem *parent = 0);
    ~QGeoMapWidget();

    void setMapService(const QGeoMappingService* mapService);
    QGeoMappingService* mapService() const;

    void setZoomLevel(int zoomLevel);
    int zoomLevel() const;

    void setCenter(const QGeoCoordinate &center);
    QGeoCoordinate center() const;

    QGeoBoundingBox viewBounds() const;

    QPointF coordinateToScreenPosition(const QGeoCoordinate &coordinate) const;
    QGeoCoordinate screenPositionToCoordinate(const QPointF &screenPosition) const;

    // animated zooming options?
    // animated panning options?
    // rotation and tilting?  
    //   use graphicsview to fake it? or only support that if the map service has support for them?

    // TODO
    // - need a better name for map objects - map decorations? map graphics?
    // - might be able to just use qgraphicsitems

    void addMapObject(const QGeoMapObject *object);
    void removeMapObject(const QGeoMapObject *object);

    QList<QGeoMapObject*> mapObjects() const;
    QList<QGeoMapObject*> mapObjectsAt(const QPointF &center, int radius = 0, DistanceUnits units = Metres) const;
    QList<QGeoMapObject*> mapObjectsAt(const QRectF &rectangle) const;

private:
    QGeoMapWidgetPrivate* d_ptr;
    Q_DECLARE_PRIVATE(QGeoMapWidget);
};

QTM_END_NAMESPACE

QT_END_HEADER

#endif
