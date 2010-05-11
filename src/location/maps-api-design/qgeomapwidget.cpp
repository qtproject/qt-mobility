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

#include "qgeomapwidget.h"
#include "qgeomapwidget_p.h"

#include "qgeocoordinate.h"
#include "qgeodistance.h"
#include "qgeoboundingbox.h"

QTM_BEGIN_NAMESPACE

QGeoMapWidget::QGeoMapWidget(QGraphicsItem *parent) : QGraphicsWidget(parent, Qt::Widget)
{
}

QGeoMapWidget::~QGeoMapWidget()
{
}

void QGeoMapWidget::setMapService(const QGeoMappingService* mapService)
{
    Q_UNUSED(mapService);
}

QGeoMappingService* QGeoMapWidget::mapService() const
{
    return 0;
}

void QGeoMapWidget::setMapType(QGeoMapWidget::MapType mapType)
{
    Q_UNUSED(mapType);
}

QGeoMapWidget::MapType QGeoMapWidget::mapType() const
{
    return StreetMap;
}

void QGeoMapWidget::setZoomLevel(int zoomLevel)
{
    Q_UNUSED(zoomLevel);
}

int QGeoMapWidget::zoomLevel() const
{
    return 0;
}

void QGeoMapWidget::setCenter(const QGeoCoordinate &center)
{
    Q_UNUSED(center);
}

QGeoCoordinate QGeoMapWidget::center() const
{
    return QGeoCoordinate();
}

QGeoBoundingBox QGeoMapWidget::viewBounds() const
{
    return QGeoBoundingBox();
}

QPointF QGeoMapWidget::coordinateToScreenPosition(const QGeoCoordinate &coordinate) const
{
    Q_UNUSED(coordinate);
    return QPointF();
}

QGeoCoordinate QGeoMapWidget::screenPositionToCoordinate(const QPointF &screenPosition) const
{
    Q_UNUSED(screenPosition);
    return QGeoCoordinate();
}

void QGeoMapWidget::addMapObject(const QGeoMapObject *object)
{
    Q_UNUSED(object);
}

void QGeoMapWidget::removeMapObject(const QGeoMapObject *object)
{
    Q_UNUSED(object);
}

QList<QGeoMapObject*> QGeoMapWidget::mapObjects() const
{
    return QList<QGeoMapObject*>();
}

QList<QGeoMapObject*> QGeoMapWidget::mapObjectsAt(const QPointF &center, const QGeoDistance &radius) const
{
    Q_UNUSED(center);
    Q_UNUSED(radius);
    return QList<QGeoMapObject*>();
}

QList<QGeoMapObject*> QGeoMapWidget::mapObjectsAt(const QRectF &rectangle) const
{
    Q_UNUSED(rectangle);
    return QList<QGeoMapObject*>();
}

#include "moc_qgeomapwidget.cpp"

QTM_END_NAMESPACE
