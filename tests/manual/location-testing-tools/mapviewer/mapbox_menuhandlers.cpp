/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "mapbox.h"
#include "geomap.h"


#include <qgeomaprectangleobject.h>
#include <qgeomapcircleobject.h>
#include <qgeomappolylineobject.h>
#include <qgeomappolygonobject.h>
#include <qgeomappixmapobject.h>
#include <qgeomaptextobject.h>
#include <qgeomaprouteobject.h>
#include <qgeomapcustomobject.h>

#include <QMessageBox>
#include <QLineEdit>

#include <QPixmap>
#include <QPainter>
#include <QBrush>
#include <QGraphicsEllipseItem>

void MapBox::selectObjects()
{
    QList<QGeoMapObject *> allObjects = m_mapWidget->mapObjects();
    for (int i = 0; i < allObjects.size(); ++i)
        allObjects[i]->setSelected(false);

    if (m_markerObjects.count() < 2)
        return;

    QGeoMapPixmapObject * bottomRight = m_markerObjects.takeLast();
    QGeoMapPixmapObject * topLeft = m_markerObjects.takeLast();
    m_mapWidget->removeMapObject(topLeft);
    m_mapWidget->removeMapObject(bottomRight);

    QList<QGeoMapObject *> selectedObjects = m_mapWidget->mapObjectsInScreenRect(
                                                QRectF(m_mapWidget->coordinateToScreenPosition(topLeft->coordinate()),
                                                       m_mapWidget->coordinateToScreenPosition(bottomRight->coordinate()))
                                            );
    for (int i = 0; i < selectedObjects.size(); ++i)
        selectedObjects[i]->setSelected(true);
}

#define MVTEST_MARK(pos) do { QGeoMapPixmapObject * marker = new QGeoMapPixmapObject(pos, m_markerOffset, m_markerIcon); m_mapWidget->addMapObject(marker); m_markerObjects.append(marker); } while (0)
#define MVTEST_MARK2(lat,lng) MVTEST_MARK(QGeoCoordinate(lat,lng))
#define MVTEST_RECT(topleft,bottomright) removePixmaps(); MVTEST_MARK(topleft); MVTEST_MARK(bottomright); drawRect();
#define MVTEST_RECT2(topleftlat,topleftlng,bottomrightlat,bottomrightlng) MVTEST_RECT(QGeoCoordinate(topleftlat,topleftlng),QGeoCoordinate(bottomrightlat,bottomrightlng))
void MapBox::demo1()
{
    MVTEST_MARK2(-30, 175);
    MVTEST_MARK2(-20, 170);
    MVTEST_MARK2(-25, -175);
    drawRect();
    drawPolygon();
    drawCircle();
}
void MapBox::demo2()
{
    const qreal density = 3; // 1 object each n degrees lat/lng

    int i = 0;
    for (qreal lat = -90 + density; lat < 90 - density; lat += density * 3) {
        for (qreal lng = -180 + density; lng < 180 - density; lng += density * 3) {
            MVTEST_RECT2(lat - density, lng - density, lat + density, lng + density);
            i++;
        }
    }

    qDebug("%i items added, %i items total.", i, m_mapWidget->mapObjects().count());

    QMessageBox * mb = new QMessageBox(QMessageBox::NoIcon, "MapViewer", QString::number(i) + " items");
    mb->open();

}
void MapBox::demo3()
{
    const qreal density = 10; // 1 cluster each n degrees lat/lng
    const qreal clusterSize = 2; // clusters extend for +/- n degrees lat/lng
    const qreal clusterDensity = 0.1 * clusterSize; // 1 object each n degrees lat/lng (as part of a cluster)

    int i = 0;
    for (qreal latm = -90 + density; latm < 90 - density; latm += density * 3) {
        for (qreal lngm = -180 + density; lngm < 180 - density; lngm += density * 3) {
            for (qreal lat = latm - clusterSize + clusterDensity; lat < latm + clusterSize - clusterDensity; lat += clusterDensity * 3) {
                for (qreal lng = lngm - clusterSize + clusterDensity; lng < lngm + clusterSize - clusterDensity; lng += clusterDensity * 3) {
                    MVTEST_RECT2(lat - clusterDensity, lng - clusterDensity, lat + clusterDensity, lng + clusterDensity);
                    i++;
                }
            }
        }
    }

    qDebug("%i items added, %i items total.", i, m_mapWidget->mapObjects().count());

    QMessageBox * mb = new QMessageBox(QMessageBox::NoIcon, "MapViewer", QString::number(i) + " items");
    mb->open();
}

void MapBox::drawRect()
{
    if (m_markerObjects.count() < 2)  return;
    QGeoMapPixmapObject * p1 = m_markerObjects.at(0);
    QGeoMapPixmapObject * p2 = m_markerObjects.at(1);

    addRectangle(p1->coordinate(), p2->coordinate());
}

void MapBox::drawPolyline()
{
    QList<QGeoCoordinate> path;

    for (int i = 0; i < m_markerObjects.size(); i++) {
        QGeoMapPixmapObject * p = m_markerObjects.at(i);
        path.append(p->coordinate());
    }

    addPolyline(path);
}

void MapBox::drawPolygon()
{
    QList<QGeoCoordinate> path;

    for (int i = 0; i < m_markerObjects.size(); i++) {
        QGeoMapPixmapObject * p = m_markerObjects.at(i);
        path.append(p->coordinate());
    }

    addPolygon(path);
}

void MapBox::drawCircle()
{
    if (m_markerObjects.count() < 1) return;

    QGeoMapPixmapObject * p1 = m_markerObjects.at(0);

    // center of the circle
    QGeoCoordinate center = p1->coordinate();

    // the circle's radius, in meters, defaults to 3000 km
    qreal radius = 3000000;

    // if a 2nd marker object is given, evaluate its distance to the first one to get the circle's radius.
    if (m_markerObjects.count() >= 2) {
        QGeoMapPixmapObject * p2 = m_markerObjects.at(1);

        radius = center.distanceTo(p2->coordinate());
    }

    QGeoMapCircleObject * circle = addCircle(center, radius);
    m_mapWidget->lastCircle = circle;
}

void MapBox::drawText()
{
    if (m_markerObjects.count() < 1) return;

    QGeoMapPixmapObject * p1 = m_markerObjects.at(0);

    // start of the text
    QGeoCoordinate start = p1->coordinate();

    QGeoMapTextObject * text = new QGeoMapTextObject(start, QString("text"));

    QColor fill(Qt::black);
    text->setBrush(QBrush(fill));
    m_mapWidget->addMapObject(text);
}

void MapBox::drawPixmap()
{
    QGeoMapPixmapObject * marker = new QGeoMapPixmapObject(m_mapWidget->screenPositionToCoordinate(m_lastClicked),
            m_markerOffset, m_markerIcon);
    m_mapWidget->addMapObject(marker);
    m_markerObjects.append(marker);
}

void MapBox::removePixmaps()
{
    while (m_markerObjects.size() > 0) {
        QGeoMapPixmapObject * marker = m_markerObjects.takeFirst();
        m_mapWidget->removeMapObject(marker);
        marker->deleteLater();
    }
}
void MapBox::removeMapObject()
{
    m_mapWidget->removeMapObject(m_lastClickedMapObject);
    if (m_lastClickedMapObject->type() == QGeoMapObject::PixmapType)
        m_markerObjects.removeAll(static_cast<QGeoMapPixmapObject *>(m_lastClickedMapObject));
    m_lastClickedMapObject->deleteLater();
}

void MapBox::calcRoute()
{
    if (m_markerObjects.count() < 2)
        return;

    QList<QGeoCoordinate> waypoints;

    for (int i = 0; i < m_markerObjects.count(); i++) {
        QGeoMapPixmapObject * p = m_markerObjects.at(i);
        waypoints.append(p->coordinate());
    }

    addRoute(waypoints);
}

void MapBox::captureCoordinates()
{
    QGeoCoordinate coords = m_mapWidget->screenPositionToCoordinate(m_lastClicked);

    if (!coords.isValid())
        return;

    m_latitudeEdit->setText(QString::number(coords.latitude()));
    m_longitudeEdit->setText(QString::number(coords.longitude()));
}

void MapBox::testDateline()
{
    m_mapWidget->setCenter(QGeoCoordinate(0.0, 180.0));
    m_mapWidget->setZoomLevel(5.0);

    QList<QGeoCoordinate> dateline;

    dateline << QGeoCoordinate(-15.0, 180.0);
    dateline << QGeoCoordinate(15.0, 180.0);

    QGeoMapPolylineObject *datelineObject = new QGeoMapPolylineObject();
    datelineObject->setPath(dateline);
    QPen p(Qt::DashLine);
    p.setColor(Qt::black);
    p.setWidth(2);
    //p.setCosmetic(true);
    datelineObject->setPen(p);

    m_mapWidget->addMapObject(datelineObject);

    addRectangle(QGeoCoordinate(-5.0, 179.0), QGeoCoordinate(-6.0, -179.0));
    addCircle(QGeoCoordinate(-7.0, 180.0), 50000.0);

    QList<QGeoCoordinate> polyline;

    polyline << QGeoCoordinate(-3.0, 180.0);
    polyline << QGeoCoordinate(-4.0, 179.0);
    polyline << QGeoCoordinate(-4.0, -179.0);

    addPolyline(polyline);

    QList<QGeoCoordinate> polygon;

    polygon << QGeoCoordinate(0.0, 180.0);
    polygon << QGeoCoordinate(-2.0, 179.0);
    polygon << QGeoCoordinate(-2.0, -179.0);

    addPolygon(polygon);

    QPixmap pixmap(48, 48);
    QPainter pixmapPainter(&pixmap);
    pixmapPainter.fillRect(pixmap.rect(), Qt::yellow);
    pixmapPainter.setPen(QPen(Qt::black));
    pixmapPainter.setBrush(QBrush(Qt::black));

    QFont font = pixmapPainter.font();
    font.setPointSize(8);
    pixmapPainter.setFont(font);

    pixmapPainter.drawText(pixmap.rect(), Qt::AlignCenter, "Pixmap");

    addPixmap(QGeoCoordinate(6.0, 180.0), QPoint(-1 * pixmap.width() / 2, -1 * pixmap.width() / 2), pixmap);

    addText(QGeoCoordinate(4.0, 180.0), QString("Text"));

    QList<QGeoCoordinate> route;

    route << QGeoCoordinate(3.0, 180.0);
    route << QGeoCoordinate(2.5, 179.0);
    route << QGeoCoordinate(1.5, 179.0);
    route << QGeoCoordinate(1.0, 180.0);
    route << QGeoCoordinate(1.5, -179.0);
    route << QGeoCoordinate(2.5, -179.0);

    addFakeRoute(route, 3);

    QGraphicsEllipseItem *ellipseItem = new QGraphicsEllipseItem(QRectF(0.0, 0.0, 96.0, 48.0));
    ellipseItem->setBrush(Qt::red);

    addCustom(QGeoCoordinate(10.0, 180.0), QPoint(-48, -24), ellipseItem);
}
