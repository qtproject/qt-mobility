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
#include <QTimer>

#include <QDebug>

QTM_BEGIN_NAMESPACE

/*!
  \class QGeoMapWidget
  \brief The QGeoMapWidget class is.
  \ingroup maps-mapping
*/

/*!
\enum QGeoMapWidget::MapType

\value StreetMap

\value SatelliteMapDay

\value SatelliteMapNight

\value TerrainMap

*/

/*!
*/
QGeoMapWidget::QGeoMapWidget(QGeoMappingManager *manager)
        : d_ptr(new QGeoMapWidgetPrivate(manager))
{
    d_ptr->mapData = d_ptr->manager->createMapData(this);

    setFlag(QGraphicsItem::ItemIsFocusable);
    setFocus();

    setMinimumSize(QSizeF(0, 0));
    setPreferredSize(QSizeF(500, 500));
    d_ptr->mapData->setViewportSize(QSizeF(300, 300));
}

/*!
*/
QGeoMapWidget::~QGeoMapWidget()
{
    delete d_ptr;
}

/*!
*/
void QGeoMapWidget::resizeEvent(QGraphicsSceneResizeEvent *event)
{
    QGraphicsWidget::resizeEvent(event);
    if (d_ptr->mapData && d_ptr->manager) {
        d_ptr->mapData->setViewportSize(event->newSize());
        d_ptr->manager->updateMapImage(d_ptr->mapData);
    }

    event->accept();
}

/*!
*/
void QGeoMapWidget::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    setFocus();
    if (event->button() == Qt::LeftButton)
        d_ptr->panActive = true;

    event->accept();
}

/*!
*/
void QGeoMapWidget::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
        d_ptr->panActive = false;

    event->accept();
}

/*!
*/
void QGeoMapWidget::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (d_ptr->panActive) {
        int deltaLeft = event->lastPos().x() - event->pos().x();
        int deltaTop  = event->lastPos().y() - event->pos().y();
        pan(deltaLeft, deltaTop);
    }

    event->accept();
}

/*!
*/
void QGeoMapWidget::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    setFocus();
    if (!d_ptr->manager || !d_ptr->mapData)
        return;

    pan(event->lastPos().x() -  size().width() / 2.0, event->lastPos().y() - size().height() / 2.0);
    if (d_ptr->mapData->zoomLevel() < d_ptr->manager->maximumZoomLevel())
        setZoomLevel(d_ptr->mapData->zoomLevel() + 1);

    event->accept();
}

/*!
*/
void QGeoMapWidget::keyPressEvent(QKeyEvent *event)
{
    if (!d_ptr->manager || !d_ptr->mapData)
        return;

    if (event->key() == Qt::Key_Minus) {
        if (d_ptr->mapData->zoomLevel() > d_ptr->manager->minimumZoomLevel()) {
            setZoomLevel(d_ptr->mapData->zoomLevel() - 1);
        }
    } else if (event->key() == Qt::Key_Plus) {
        if (d_ptr->mapData->zoomLevel() < d_ptr->manager->maximumZoomLevel()) {
            setZoomLevel(d_ptr->mapData->zoomLevel() + 1);
        }
    }

    event->accept();
}

/*!
*/
void QGeoMapWidget::wheelEvent(QGraphicsSceneWheelEvent* event)
{
    if (!d_ptr->manager || !d_ptr->mapData)
        return;

    if (event->delta() > 0) { //zoom in
        if (d_ptr->mapData->zoomLevel() > d_ptr->manager->minimumZoomLevel()) {
            setZoomLevel(d_ptr->mapData->zoomLevel() + 1);
        }
    } else { //zoom out
        if (d_ptr->mapData->zoomLevel() < d_ptr->manager->maximumZoomLevel()) {
            setZoomLevel(d_ptr->mapData->zoomLevel() - 1);
        }
    }
    event->accept();
}

/*!
*/
QPainterPath QGeoMapWidget::shape() const
{
    QPainterPath path;
    path.addRect(boundingRect());
    return path;
}

/*!
*/
void QGeoMapWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    d_ptr->mapData->mapImage().rect();
    painter->drawPixmap(d_ptr->mapData->mapImage().rect(), d_ptr->mapData->mapImage(), d_ptr->mapData->mapImage().rect());
}

/*!
*/
qreal QGeoMapWidget::minimumZoomLevel() const
{
    if (d_ptr->manager)
        return d_ptr->manager->minimumZoomLevel();

    return -1;
}

/*!
*/
qreal QGeoMapWidget::maximumZoomLevel() const
{
    if (d_ptr->manager)
        return d_ptr->manager->maximumZoomLevel();

    return -1;
}

/*!
*/
void QGeoMapWidget::setZoomLevel(qreal zoomLevel)
{
    if (d_ptr->mapData && d_ptr->manager) {
        d_ptr->mapData->setZoomLevel(zoomLevel);
        d_ptr->manager->updateMapImage(d_ptr->mapData);
    }
}

/*!
*/
qreal QGeoMapWidget::zoomLevel() const
{
    if (d_ptr->mapData)
        return d_ptr->mapData->zoomLevel();

    return -1;
}

/*!
*/
void QGeoMapWidget::pan(int dx, int dy)
{
    if (d_ptr->mapData && d_ptr->manager) {
        d_ptr->mapData->pan(dx, dy);
        d_ptr->manager->updateMapImage(d_ptr->mapData);
    }
}

/*!
*/
void QGeoMapWidget::setCenter(const QGeoCoordinate &center)
{
    if (d_ptr->mapData && d_ptr->manager) {
        d_ptr->mapData->setCenter(center);
        d_ptr->manager->updateMapImage(d_ptr->mapData);
    }
}

/*!
*/
QGeoCoordinate QGeoMapWidget::center() const
{
    if (d_ptr->mapData)
        return d_ptr->mapData->center();

    return QGeoCoordinate();
}

/*!
*/
void QGeoMapWidget::setMapType(QGeoMapWidget::MapType mapType)
{
    if (d_ptr->mapData && d_ptr->manager) {
        d_ptr->mapData->setMapType(mapType);
        d_ptr->manager->updateMapImage(d_ptr->mapData);
    }
}

/*!
*/
QGeoMapWidget::MapType QGeoMapWidget::mapType() const
{
    return d_ptr->mapData->mapType();
}

/*!
*/
QPointF QGeoMapWidget::coordinateToScreenPosition(const QGeoCoordinate &coordinate) const
{
    if (d_ptr->mapData)
        return d_ptr->mapData->coordinateToScreenPosition(coordinate);

    return QPointF();
}

/*!
*/
QGeoCoordinate QGeoMapWidget::screenPositionToCoordinate(QPointF screenPosition) const
{
    if (d_ptr->mapData)
        return d_ptr->mapData->screenPositionToCoordinate(screenPosition);

    return QGeoCoordinate();
}

// TODO remove this - just call update from the mapData
void QGeoMapWidget::mapImageUpdated()
{
    this->update();
}

/*******************************************************************************
*******************************************************************************/

QGeoMapWidgetPrivate::QGeoMapWidgetPrivate(QGeoMappingManager *manager)
        : manager(manager),
        mapData(0),
        panActive(false) {}

QGeoMapWidgetPrivate::QGeoMapWidgetPrivate(const QGeoMapWidgetPrivate &other)
        : manager(other.manager),
        mapData(other.mapData),
        panActive(other.panActive) {}

QGeoMapWidgetPrivate::~QGeoMapWidgetPrivate() {}

QGeoMapWidgetPrivate& QGeoMapWidgetPrivate::operator= (const QGeoMapWidgetPrivate & other)
{
    manager = other.manager;
    mapData = other.mapData;
    panActive = other.panActive;

    return *this;
}

#include "moc_qgeomapwidget.cpp"

QTM_END_NAMESPACE
