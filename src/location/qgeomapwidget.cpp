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

#include <QGraphicsSceneResizeEvent>

#include <QDebug>

QTM_BEGIN_NAMESPACE

QGeoMapWidget::QGeoMapWidget(QGeoMappingManager *manager)
        : d_ptr(new QGeoMapWidgetPrivate(manager))
{
    d_ptr->viewport = d_ptr->manager->createViewport(this);

    setFlag(QGraphicsItem::ItemIsFocusable);
    setFocus();

    setMinimumSize(QSizeF(0, 0));
    setPreferredSize(QSizeF(500, 500));
    //setGeometry(QRectF(-250, -250, 500, 500));
    d_ptr->viewport->setViewportSize(QSizeF(300, 300));
    if (d_ptr->manager)
        d_ptr->manager->updateMapImage(d_ptr->viewport);
}

QGeoMapWidget::~QGeoMapWidget()
{
    delete d_ptr;
}

void QGeoMapWidget::resizeEvent(QGraphicsSceneResizeEvent *event)
{
    QGraphicsWidget::resizeEvent(event);
    if (d_ptr->viewport) {
        d_ptr->viewport->setViewportSize(event->newSize());
    }
    if (d_ptr->manager)
        d_ptr->manager->updateMapImage(d_ptr->viewport);
    event->accept();
}

void QGeoMapWidget::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    setFocus();
    if (event->button() == Qt::LeftButton)
        d_ptr->panActive = true;
    event->accept();
}

void QGeoMapWidget::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
        d_ptr->panActive = false;
    event->accept();
}

void QGeoMapWidget::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (d_ptr->panActive) {
        int deltaLeft = event->lastPos().x() - event->pos().x();
        int deltaTop  = event->lastPos().y() - event->pos().y();
        pan(deltaLeft, deltaTop);
    }
    event->accept();
}

void QGeoMapWidget::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    setFocus();
    if (!d_ptr->manager || !d_ptr->viewport) {
        return;
    }

    pan(event->lastPos().x() -  size().width() / 2.0, event->lastPos().y() - size().height() / 2.0);
    if (d_ptr->viewport->zoomLevel() < d_ptr->manager->maximumZoomLevel())
        setZoomLevel(d_ptr->viewport->zoomLevel() + 1);
    d_ptr->manager->updateMapImage(d_ptr->viewport);
    event->accept();
}

void QGeoMapWidget::keyPressEvent(QKeyEvent *event)
{
    if (!d_ptr->manager || !d_ptr->viewport) {
        return;
    }

    if (event->key() == Qt::Key_Minus) {
        if (d_ptr->viewport->zoomLevel() > d_ptr->manager->minimumZoomLevel()) {
            setZoomLevel(d_ptr->viewport->zoomLevel() - 1);
            d_ptr->manager->updateMapImage(d_ptr->viewport);
        }
    } else if (event->key() == Qt::Key_Plus) {
        if (d_ptr->viewport->zoomLevel() < d_ptr->manager->maximumZoomLevel()) {
            setZoomLevel(d_ptr->viewport->zoomLevel() + 1);
            d_ptr->manager->updateMapImage(d_ptr->viewport);
        }
    }
    event->accept();
}

QPainterPath QGeoMapWidget::shape() const
{
    QPainterPath path;
    path.addRect(boundingRect());
    return path;
}

void QGeoMapWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    d_ptr->viewport->mapImage().rect();
    painter->drawPixmap(d_ptr->viewport->mapImage().rect(), d_ptr->viewport->mapImage(), d_ptr->viewport->mapImage().rect());
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
    if (d_ptr->manager)
        d_ptr->manager->updateMapImage(d_ptr->viewport);
}

qreal QGeoMapWidget::zoomLevel() const
{
    if (d_ptr->viewport)
        return d_ptr->viewport->zoomLevel();

    return -1;
}

void QGeoMapWidget::pan(int dx, int dy)
{
    if (d_ptr->viewport)
        d_ptr->viewport->pan(dx, dy);
    if (d_ptr->manager)
        d_ptr->manager->updateMapImage(d_ptr->viewport);
}

void QGeoMapWidget::setCenter(const QGeoCoordinate &center)
{
    if (d_ptr->viewport)
        d_ptr->viewport->setCenter(center);
    if (d_ptr->manager)
        d_ptr->manager->updateMapImage(d_ptr->viewport);
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
    if (d_ptr->viewport)
        return d_ptr->viewport->coordinateToScreenPosition(coordinate);

    return QPointF();
}

QGeoCoordinate QGeoMapWidget::screenPositionToCoordinate(QPointF screenPosition) const
{
    if (d_ptr->viewport)
        return d_ptr->viewport->screenPositionToCoordinate(screenPosition);

    return QGeoCoordinate();
}

void QGeoMapWidget::mapImageUpdated()
{
    this->update();
}

/*******************************************************************************
*******************************************************************************/

QGeoMapWidgetPrivate::QGeoMapWidgetPrivate(QGeoMappingManager *manager)
        : manager(manager),
        viewport(0), panActive(false)
{}

QGeoMapWidgetPrivate::QGeoMapWidgetPrivate(const QGeoMapWidgetPrivate &other)
        : manager(other.manager),
        viewport(other.viewport), panActive(other.panActive)
{}

QGeoMapWidgetPrivate::~QGeoMapWidgetPrivate() {}

QGeoMapWidgetPrivate& QGeoMapWidgetPrivate::operator= (const QGeoMapWidgetPrivate & other)
{
    manager = other.manager;
    viewport = other.viewport;
    panActive = other.panActive;

    return *this;
}

#include "moc_qgeomapwidget.cpp"

QTM_END_NAMESPACE
