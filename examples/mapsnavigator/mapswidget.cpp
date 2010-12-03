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

#include <QTimer>

//TODO: get rid of debug when finalized
#include <QDebug>

class GeoMap : public QGraphicsGeoMap
{
public:

    GeoMap(QGeoMappingManager *manager, MapsWidget *mapsWidget)
        : QGraphicsGeoMap(manager),
          m_mapsWidget(mapsWidget),
          panActive(false),
          objectPressed(false),
          objectHeld(false)
    {
        // connect timer for hold event
        timer = new QTimer();
        timer->setInterval(1000);
        connect(timer, SIGNAL(timeout()), timer, SLOT(stop()));
    }

    ~GeoMap()
    {
        delete timer;
    }

    void setObjectHeld(bool held) {
        objectHeld = held;
    }

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event)
    {
        panActive = true;
        timer->disconnect(m_mapsWidget);

        QList<QGeoMapObject*> markers = mapObjectsAtScreenPosition(event->pos());
        objectPressed = false;
        for (int i=0; i<markers.size(); i++) {
            if (markers.at(i)->isVisible()) {
                objectPressed = true;
                m_mapsWidget->setMouseMarker(markers.at(i));
                break;
            }
        }

        // sets timer for  map or map-object hold event
        if (!objectPressed)
            connect(timer, SIGNAL(timeout()), m_mapsWidget, SLOT(setMarker()));
        else
            connect(timer, SIGNAL(timeout()), m_mapsWidget, SLOT(moveMarker()));

        m_mapsWidget->setMarkerLocation(screenPositionToCoordinate(event->pos()));
        timer->start();

        event->accept();
    }

    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
    {
        panActive = false;
        timer->stop();
        if (objectPressed) {
            emit m_mapsWidget->markerReleased();
            objectPressed = false;
            objectHeld = false;
        }
        event->accept();
    }

    void mouseMoveEvent(QGraphicsSceneMouseEvent* event)
    {
        timer->stop();
        if (panActive) {
            if (objectPressed && objectHeld) {
                QGeoCoordinate last = screenPositionToCoordinate(event->lastPos());
                QGeoCoordinate curr = screenPositionToCoordinate(event->pos());
                emit m_mapsWidget->moveMarker(last, curr);
            } else {
                QPointF diff = -(event->pos() - event->lastPos());
                pan(diff.x(), diff.y());

                if (objectPressed) {
                    emit m_mapsWidget->mapMoved();
                    objectPressed = false;
                }
            }
            event->accept();
        }
    }

    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
    {
        setCenter(screenPositionToCoordinate(event->pos()));
        setZoomLevel(zoomLevel() + 1);
        event->accept();
    }

private:
    MapsWidget *m_mapsWidget;
    QTimer *timer;
    bool panActive;
    bool objectPressed;
    bool objectHeld;
};


MapsWidget::MapsWidget(QWidget *parent)
    : QWidget(parent), waypointSelect(false)
{
}

MapsWidget::~MapsWidget()
{
}

void MapsWidget::initialize(QGeoMappingManager *mappingManager)
{
    //TODO: GPS my location
    myLocation = QGeoCoordinate(-27.5796, 153.1);
    markerLocation = myLocation;

    m_mappingManager = mappingManager;

    if (m_mappingManager) {
        //TODO: map zooming

        // setup map and graphics view
        geomap = new GeoMap(m_mappingManager, this);
        QGraphicsScene* scene = new QGraphicsScene(this);
        scene->addItem(geomap);

        view = new QGraphicsView(scene, this);
        view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        view->setVisible(true);
        view->setInteractive(true);

        geomap->resize(300, 480);
        geomap->setCenter(myLocation);
        geomap->setZoomLevel(15);

        QHBoxLayout *widgetLayout = new QHBoxLayout;
        widgetLayout->addWidget(view);
        setLayout(widgetLayout);

        // add my location marker
        MarkerObject *myMarker = new MarkerObject(MarkerObject::MyLocation);
        myMarker->setCoordinate(myLocation);
        myMarker->setIconPath(":/icons/mylocation.png");
        createMarker(myMarker);
        myLocationMarker = myMarker;

    } else {
        // TODO: errors
    }
}

MarkerObject* MapsWidget::getMyLocation()
{
    return myLocationMarker;
}

bool MapsWidget::getWaypointSelect()
{
    return waypointSelect;
}

void MapsWidget::centerOnMyLocation()
{
    geomap->setCenter(myLocation);
}

void MapsWidget::centerOnMarker(MarkerObject *marker)
{
    geomap->setCenter(marker->coordinate());
}

void MapsWidget::createMarker(MarkerObject *marker)
{
    emit addMarker(marker);
    geomap->addMapObject(marker);
}

void MapsWidget::deleteMarker(MarkerObject *marker)
{
    emit removeMarker(marker);
    geomap->removeMapObject(marker);
    delete marker;
}

void MapsWidget::moveMarker()
{
    if (m_marker->markerType() == MarkerObject::Search) {
        m_marker->expandIcon(markerLocation);
        geomap->setObjectHeld(true);
    }
}

void MapsWidget::setMarker()
{
    if (waypointSelect) {
        waypointSelect = false;
        m_marker->setCoordinate(markerLocation);
        emit waypointSelected();
        return;
    }

    setMarkerCoordinate(markerLocation);
}

MarkerObject* MapsWidget::setMarkerCoordinate(const QGeoCoordinate& coord)
{
    MarkerObject *marker = new MarkerObject(MarkerObject::Search);
    marker->setIconPath(":/icons/searchmarker.png");
    marker->setCoordinate(coord);
    createMarker(marker);
    return marker;
}

MarkerObject* MapsWidget::setMarkerAddress(const QString& address)
{
    MarkerObject *marker = new MarkerObject(MarkerObject::Search);
    marker->setIconPath(":/icons/searchmarker.png");
    marker->setMarkerState(MarkerObject::Address);
    marker->setName(address);
    createMarker(marker);
    return marker;
}

void MapsWidget::setMarkerLocation(const QGeoCoordinate& coord)
{
    markerLocation = coord;
}

void MapsWidget::setMouseMarker(QGeoMapObject *marker)
{
    m_marker = qobject_cast<MarkerObject*>(marker);
    emit markerPressed(m_marker);
}

void MapsWidget::beginWaypointSelect(MarkerObject *marker)
{
    m_marker = marker;
    waypointSelect = true;
}
