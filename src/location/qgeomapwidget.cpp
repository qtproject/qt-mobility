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

QTM_BEGIN_NAMESPACE

QGeoMapWidget::QGeoMapWidget(QGeoMappingManager *manager)
    : d_ptr(new QGeoMapWidgetPrivate(manager))
{
    d_ptr->viewport = d_ptr->manager->createViewport(this);
}

QGeoMapWidget::~QGeoMapWidget()
{
    delete d_ptr;
}

qreal QGeoMapWidget::minimumZoomLevel() const
{
    if (d_ptr->manager)
        return d_ptr->manager->minimumZoomLevel();

    return -1;
}

qreal QGeoMapWidget::maximumZoomLevel() const
{
    if (d_ptr->manager)
        return d_ptr->manager->maximumZoomLevel();

    return -1;
}

void QGeoMapWidget::setZoomLevel(qreal zoomLevel)
{
    if (d_ptr->viewport)
        d_ptr->viewport->setZoomLevel(zoomLevel);
}

qreal QGeoMapWidget::zoomLevel() const
{
    if (d_ptr->viewport)
        return d_ptr->viewport->zoomLevel();

    return -1;
}

void QGeoMapWidget::setCenter(const QGeoCoordinate &center)
{
    if (d_ptr->viewport)
        d_ptr->viewport->setCenter(center);
}

QGeoCoordinate QGeoMapWidget::center() const
{
    if (d_ptr->viewport)
        return d_ptr->viewport->center();

    return QGeoCoordinate();
}

void QGeoMapWidget::setMapType(QGeoMapWidget::MapType mapType)
{
    if (d_ptr->viewport)
        d_ptr->viewport->setMapType(mapType);
}

QGeoMapWidget::MapType QGeoMapWidget::mapType() const
{
    return d_ptr->viewport->mapType();
}

QPointF QGeoMapWidget::coordinateToScreenPosition(const QGeoCoordinate &coordinate) const
{
    if (d_ptr->manager)
        d_ptr->manager->coordinateToScreenPosition(d_ptr->viewport, coordinate);

    return QPointF();
}

QGeoCoordinate QGeoMapWidget::screenPositionToCoordinate(QPointF screenPosition) const
{
    if (d_ptr->manager)
        d_ptr->manager->screenPositionToCoordinate(d_ptr->viewport, screenPosition);

    return QGeoCoordinate();
}

void QGeoMapWidget::mapImageUpdated()
{
}

/*******************************************************************************
*******************************************************************************/

QGeoMapWidgetPrivate::QGeoMapWidgetPrivate(QGeoMappingManager *manager)
    : manager(manager),
    viewport(0) {}

QGeoMapWidgetPrivate::QGeoMapWidgetPrivate(const QGeoMapWidgetPrivate &other)
    : manager(other.manager),
    viewport(other.viewport) {}

QGeoMapWidgetPrivate::~QGeoMapWidgetPrivate() {}

QGeoMapWidgetPrivate& QGeoMapWidgetPrivate::operator= (const QGeoMapWidgetPrivate &other)
{
    manager = other.manager;
    viewport = other.viewport;

    return *this;
}

#include "moc_qgeomapwidget.cpp"

QTM_END_NAMESPACE
