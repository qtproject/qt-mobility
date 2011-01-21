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

#include "mapswidget.h"
#include "marker.h"

#include <QGeoCoordinate>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneWheelEvent>

GeoMap::GeoMap(QGeoMappingManager *manager, MapsWidget *mapsWidget) :
    QGraphicsGeoMap(manager), m_mapsWidget(mapsWidget)
{
}

GeoMap::~GeoMap()
{
}

void GeoMap::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    panActive = true;

    markerPressed = false;
    QList<QGeoMapObject*> objects = mapObjectsAtScreenPosition(event->pos());
    if (objects.size() > 0) {
        pressed = objects.first();
        markerPressed = true;
    }

    event->accept();
}

void GeoMap::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    panActive = false;

    if (markerPressed) {
        // check if we're still over the object
        QList<QGeoMapObject*> objects = mapObjectsAtScreenPosition(event->pos());
        if (objects.contains(pressed)) {
            Marker *m = dynamic_cast<Marker*>(pressed);
            if (m)
                emit clicked(m);
        }

        markerPressed = false;
    }

    event->accept();
}

void GeoMap::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (panActive) {
        QPointF delta = event->lastPos() - event->pos();
        pan(delta.x(), delta.y());
    }
    event->accept();
}

void GeoMap::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    qreal panx = event->pos().x() - size().width() / 2.0;
    qreal pany = event->pos().y() - size().height() / 2.0;
    pan(panx, pany);
    if (event->delta() > 0) {   // zoom in
        if (zoomLevel() < maximumZoomLevel()) {
            setZoomLevel(zoomLevel() + 1);
        }
    } else {                    // zoom out
        if (zoomLevel() > minimumZoomLevel()) {
            setZoomLevel(zoomLevel() - 1);
        }
    }
    pan(-panx, -pany);
    event->accept();
}

MapsWidget::MapsWidget(QWidget *parent) :
    QWidget(parent),
    geoMap(0),
    graphicsView(0),
    m_markerManager(0)
{
}

MapsWidget::~MapsWidget()
{
}

void MapsWidget::initialize(QGeoMappingManager *manager)
{
    geoMap = new GeoMap(manager, this);
    if (m_markerManager)
        m_markerManager->setMap(geoMap);

    connect(geoMap, SIGNAL(clicked(Marker*)),
            this, SIGNAL(markerClicked(Marker*)));

    QGraphicsScene *sc = new QGraphicsScene;
    sc->addItem(geoMap);

    geoMap->setPos(0, 0);
    geoMap->resize(this->size());

    graphicsView = new QGraphicsView(sc, this);
    graphicsView->setVisible(true);
    graphicsView->setInteractive(true);
    graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    graphicsView->resize(this->size());
    graphicsView->centerOn(geoMap);

    geoMap->setCenter(QGeoCoordinate(-27.5796, 153.1));
    geoMap->setZoomLevel(15);
}

void MapsWidget::setMyLocation(QGeoCoordinate location, bool center)
{
    if (m_markerManager)
        m_markerManager->setMyLocation(location);
    if (geoMap && center)
        geoMap->setCenter(location);
}

QGraphicsGeoMap *MapsWidget::map() const
{
    return geoMap;
}

void MapsWidget::resizeEvent(QResizeEvent *event)
{
    if (graphicsView && geoMap) {
        graphicsView->resize(event->size());
        geoMap->resize(event->size());
        graphicsView->centerOn(geoMap);
    }
}

void MapsWidget::showEvent(QShowEvent *event)
{
    if (graphicsView && geoMap) {
        graphicsView->resize(size());
        geoMap->resize(size());
        graphicsView->centerOn(geoMap);
    }
}

MarkerManager *MapsWidget::markerManager() const
{
    return m_markerManager;
}

void MapsWidget::setMarkerManager(MarkerManager *markerManager)
{
    m_markerManager = markerManager;
    if (geoMap)
        m_markerManager->setMap(geoMap);
}
