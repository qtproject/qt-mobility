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

#include <QGraphicsLineItem>
#include <QPointF>
#include <QPainterPath>
#include <QPolygonF>

#include "qmapobject.h"
#include "qmaproute.h"
#include "qmapview.h"
#include "qmapline.h"
#include "qmapmarker.h"
#include "qmaprect.h"
#include "qmappolygon.h"
#include "qgeocoordinatemaps.h"

QTM_BEGIN_NAMESPACE

/******************************************************************************
* QMapObject
*******************************************************************************/
void QMapObject::compIntersectingTiles(const QRectF& box)
{
    QMapView::TileIterator it(mapView, box);

    while (it.hasNext()) {
        it.next();

        if (it.isValid() && intersects(it.tileRect()))
            intersectingTiles.append(mapView.getTileIndex(it.col(), it.row()));
    }
}

QRectF QMapObject::boundingRect(const QLineF& line)
{
    qreal top;
    qreal left;
    qreal right;
    qreal bottom;

    if (line.x1() < line.x2()) {
        left = line.x1();
        right = line.x2();
    } else {
        left = line.x2();
        right = line.x1();
    }

    if (line.y1() < line.y2()) {
        top = line.y1();
        bottom = line.y2();
    } else {
        top = line.y2();
        bottom = line.y1();
    }

    return QRectF(QPoint(left, top), QPoint(right, bottom));
}

bool QMapObject::intersect(const QRectF& rect, const QLineF& line)
{
    //check all for edges of the tile
    QPointF tmp;
    QLineF leftEdge(rect.topLeft(), rect.bottomLeft());
    QLineF topEdge(rect.topLeft(), rect.topRight());
    QLineF rightEdge(rect.topRight(), rect.bottomRight());
    QLineF bottomEdge(rect.bottomLeft(), rect.bottomRight());

    if (line.intersect(leftEdge, &tmp) == QLineF::BoundedIntersection) {
        return true;
    } else if (line.intersect(topEdge, &tmp) == QLineF::BoundedIntersection) {
        return true;
    } else if (line.intersect(rightEdge, &tmp) == QLineF::BoundedIntersection) {
        return true;
    } else if (line.intersect(bottomEdge, &tmp) == QLineF::BoundedIntersection) {
        return true;
    }

    //no intersection so far, maybe line lies completely within rect?
    QRectF box = boundingRect(line);

    if (rect.contains(box))
        return true;

    return false;
}

/******************************************************************************
* QMapRoute
*******************************************************************************/
QMapRoute::QMapRoute(const QMapView& mapView, const QRoute& route, const QPen& pen, const QPixmap& /*endpointMarker*/, quint16 layerIndex)
        : QMapObject(mapView, QMapObject::Route, layerIndex), rt(route), rPen(pen)
{
    //TODO: endpoint marker for routes
}

void QMapRoute::compMapCoords()
{
    segments.clear();
    quint32 minDist = mapView.routeDetailLevel();
    QList<QManeuver> maneuvers = rt.maneuvers();
    QGeoCoordinateMaps last;
    QGeoCoordinateMaps here;

    for (int i = 0; i < maneuvers.size(); i++) {
        QList<QGeoCoordinateMaps> wayPoints = maneuvers[i].wayPoints();

        for (int n = 0; n < wayPoints.size(); n++) {
            //make sure first waypoint is always shown
            if (last.isNull()) {
                last = wayPoints[n];
                continue;
            }

            here = wayPoints[n];
            QLineF line = mapView.connectShortest(here, last);

            if ((line.p1() - line.p2()).manhattanLength() >= minDist ||
                    mapView.zoomLevel() == mapView.maxZoomLevel()) {
                addSegment(line);
                last = here;
                here = QGeoCoordinateMaps();
            }
        }
    }

    //make sure last waypoint is always shown
    if (!here.isNull()) {
        QLineF line = mapView.connectShortest(here, last);
        addSegment(line);
    }

    compIntersectingTiles(QRectF());
}

void QMapRoute::addSegment(const QLineF& line)
{
    //TODO: refine: check whether tiles are actually being intersected
    QMapView::TileIterator it(mapView, QMapObject::boundingRect(line));

    while (it.hasNext()) {
        it.next();

        if (it.isValid()) {
            quint64 tileIndex = mapView.getTileIndex(it.col(), it.row());

            if (!segments.contains(tileIndex))
                segments.insert(tileIndex, QList<QLineF>());

            segments[tileIndex].append(line);
        }
    }
}

void QMapRoute::compIntersectingTiles(const QRectF& /*box*/)
{
    intersectingTiles.clear();
    QMapIterator<quint64, QList<QLineF> > it(segments);

    while (it.hasNext()) {
        it.next();
        intersectingTiles.append(it.key());
    }
}

bool QMapRoute::intersects(const QRectF& rect) const
{
    QMapView::TileIterator it(mapView, rect);

    while (it.hasNext()) {
        it.next();

        if (!it.isValid())
            continue;

        quint64 tileIndex = mapView.getTileIndex(it.col(), it.row());

        if (segments.contains(tileIndex)) {
            QRectF tile = it.tileRect();
            QListIterator<QLineF> lit(segments[tileIndex]);

            while (lit.hasNext()) {
                const QLineF& line = lit.next();

                if (QMapObject::intersect(tile, line))
                    return true;
            }
        }
    }

    return false;
}

void QMapRoute::paint(QPainter* painter, const QRectF& viewPort)
{
    QPen oldPen = painter->pen();
    painter->setPen(rPen);
    QMapView::TileIterator it(mapView, viewPort);
    int count = 0;
    QPainterPath path;

    while (it.hasNext()) {
        it.next();

        if (!it.isValid())
            continue;

        quint64 tileIndex = mapView.getTileIndex(it.col(), it.row());

        if (segments.contains(tileIndex)) {
            QListIterator<QLineF> lit(segments[tileIndex]);

            while (lit.hasNext()) {
                QLineF line = lit.next().translated(-viewPort.topLeft());
                //painter->drawLine(line);
                path.moveTo(line.p1());
                path.lineTo(line.p2());
                count++;
            }
        }
    }

    painter->drawPath(path);
    qint64 mapWidth = static_cast<qint64>(mapView.mapWidth());

    //Is view port wrapping around date line?
    if (viewPort.right() >= mapWidth) {
        path.translate(mapWidth, 0);
        painter->drawPath(path);
        path.translate(-mapWidth, 0);
    }
    //Is path spanning date line?
    if (path.boundingRect().right() >= mapWidth) {
        path.translate(-mapWidth, 0);
        painter->drawPath(path);
    }

    painter->setPen(oldPen);
}

/******************************************************************************
* QMapLine
*******************************************************************************/
QMapLine::QMapLine(const QMapView& mapView, const QGeoCoordinateMaps& point1,
                   const QGeoCoordinateMaps& point2, const QPen& pen, quint16 layerIndex)
        : QMapObject(mapView, QMapObject::Line, layerIndex), pt1(point1), pt2(point2), p(pen)
{
}

void QMapLine::compMapCoords()
{
    QPointF p1 = mapView.geoToMap(pt1);
    QPointF p2 = mapView.geoToMap(pt2);
    line = QLineF(p1, p2);

    //lines are drawn from west to east
    if (pt2.longitude() < pt1.longitude()) {
        p2.rx() += mapView.mapWidth();
        line.setP2(p2);
    }

    //compute intersecting map tiles now
    intersectingTiles.clear();
    QRectF box = QMapObject::boundingRect(line);
    compIntersectingTiles(box);
}

void QMapLine::paint(QPainter* painter, const QRectF& viewPort)
{
    qint64 mapWidth = static_cast<qint64>(mapView.mapWidth());
    QPen oldPen = painter->pen();
    painter->setPen(p);
    line.translate(-viewPort.left(), -viewPort.top());
    painter->drawLine(line);
    line.translate(viewPort.left(), viewPort.top());

    //Is view port wrapping around date line?
    if (viewPort.right() >= mapWidth) {
        line.translate(-viewPort.left(), -viewPort.top());
        line.translate(mapWidth, 0);
        painter->drawLine(line);
        line.translate(-mapWidth, 0);
        line.translate(viewPort.left(), viewPort.top());
    }

    //Is line crossing date line?
    if (line.x2() >= mapWidth) {
        line.translate(-viewPort.left(), -viewPort.top());
        line.translate(-mapWidth, 0);
        painter->drawLine(line);
        line.translate(mapWidth, 0);
        line.translate(viewPort.left(), viewPort.top());
    }

    painter->setPen(oldPen);
}

bool QMapLine::intersects(const QRectF& rect) const
{
    return QMapObject::intersect(rect, line);
}

/******************************************************************************
* QMapMarker
*******************************************************************************/

/*!
    \class QMapMarker
    \brief Internal representation of a map marker.
    \ingroup location

    This class is used by QMapView internally to represent a map marker.
*/

/*!
    Constructor.
*/

#define MARKER_HEIGHT 36
#define MARKER_WIDTH 25
#define MARKER_PIN_LEN 10

QMapMarker::QMapMarker(const QMapView& mapView, const QGeoCoordinateMaps& point,
                       const QString& text, const QFont& font, const QColor& fontColor,
                       const QPixmap& icon, const QRectF& textRect,
                       quint16 layerIndex)
        : QMapObject(mapView, QMapObject::Marker, layerIndex),
        pt(point), txt(text), txtFont(font), fColor(fontColor), icn(icon), txtRect(textRect)
{
    if (icon.isNull()) {
        box.setHeight(MARKER_HEIGHT);
        box.setWidth(MARKER_WIDTH);
    }
}

void QMapMarker::compMapCoords()
{
    if (icn.isNull()) {
        mapPt = mapView.geoToMap(pt);
        box.moveLeft(mapPt.x() - (MARKER_WIDTH / 2));
        box.moveTop(mapPt.y() - (MARKER_HEIGHT - 1));
        intersectingTiles.clear();
        compIntersectingTiles(box);
    }
}

void QMapMarker::constructMarker(QPainter* painter, const QPointF& point)
{
    QPointF p1 = point;
    QPointF p2(p1.x(), p1.y() - MARKER_PIN_LEN);
    QPen pen(Qt::black);
    pen.setWidth(2);
    painter->setPen(pen);
    painter->drawLine(p1, p2);
    QRectF ellipse(p2.x() - (MARKER_WIDTH / 2), p1.y() - (MARKER_HEIGHT - 1), MARKER_WIDTH, MARKER_WIDTH);
    pen.setWidth(1);
    painter->setPen(pen);
    QColor color(Qt::green);
    color.setAlpha(127);
    QBrush brush(color);
    painter->setBrush(brush);
    painter->drawEllipse(ellipse);

    if (!txt.isNull()) {
        painter->setFont(txtFont);
        pen.setColor(fColor);
        painter->setPen(pen);
        painter->drawText(ellipse, Qt::AlignCenter | Qt::TextWordWrap, txt);
    }
}

void QMapMarker::paint(QPainter* painter, const QRectF& viewPort)
{
    if (icn.isNull()) {
        quint64 mapWidth = mapView.mapWidth();
        QPen oldPen = painter->pen();
        QBrush oldBrush = painter->brush();
        constructMarker(painter, mapPt - viewPort.topLeft());

        //Is view port wrapping around date line?
        if (viewPort.right() >= mapWidth)
            constructMarker(painter, mapPt - viewPort.topLeft() + QPointF(mapWidth, 0));
        //Is marker spanning date line?
        if (box.right() >= mapWidth)
            constructMarker(painter, mapPt - viewPort.topLeft() - QPointF(mapWidth, 0));

        painter->setPen(oldPen);
        painter->setBrush(oldBrush);
    }
    //TODO: map marker with provided icon
}

bool QMapMarker::intersects(const QRectF& tileRect) const
{
    return box.intersects(tileRect);
}


/******************************************************************************
* QMapRect
*******************************************************************************/
QMapRect::QMapRect(const QMapView& mapView, const QGeoCoordinateMaps& topLeft, const QGeoCoordinateMaps& bottomRight,
                   const QPen& pen, const QBrush& brush, quint16 layerIndex)
        : QMapObject(mapView, QMapObject::Rect, layerIndex),
        geoTopLeft(topLeft), geoBottomRight(bottomRight), p(pen), b(brush)
{
}

void QMapRect::compMapCoords()
{
    QPointF p1 = mapView.geoToMap(geoTopLeft);
    QPointF p2 = mapView.geoToMap(geoBottomRight);

    if (p2.y() < p1.y()) {
        qreal y = p2.y();
        p2.setY(p1.y());
        p1.setY(y);
    }

    //Lines will always connect points from west to east.
    //If we cross the date line, we need to split.
    if (geoTopLeft.longitude() > geoBottomRight.longitude())
        p2.rx() += mapView.mapWidth();

    rect = QRectF(p1, p2);
    //compute intersecting map tiles now
    intersectingTiles.clear();
    compIntersectingTiles(rect);
}

void QMapRect::paint(QPainter* painter, const QRectF& viewPort)
{
    QPen oldPen = painter->pen();
    QBrush oldBrush = painter->brush();
    painter->setPen(p);
    painter->setBrush(b);
    rect.translate(-viewPort.left(), -viewPort.top());
    painter->drawRect(rect);
    rect.translate(viewPort.left(), viewPort.top());
    qint64 mapWidth = static_cast<qint64>(mapView.mapWidth());

    //Is view port wrapping around date line?
    if (viewPort.right() >= mapWidth) {
        rect.translate(-viewPort.left(), -viewPort.top());
        rect.translate(mapWidth, 0);
        painter->drawRect(rect);
        rect.translate(-mapWidth, 0);
        rect.translate(viewPort.left(), viewPort.top());
    }
    //Is rect crossing date line?
    if (rect.right() >= mapWidth) {
        rect.translate(-viewPort.left(), -viewPort.top());
        rect.translate(-mapWidth, 0);
        painter->drawRect(rect);
        rect.translate(mapWidth, 0);
        rect.translate(viewPort.left(), viewPort.top());
    }

    painter->setPen(oldPen);
    painter->setBrush(oldBrush);
}

bool QMapRect::intersects(const QRectF& rect) const
{
    return this->rect.intersects(rect);
}

/******************************************************************************
* QMapPolygon
*******************************************************************************/
QMapPolygon::QMapPolygon(const QMapView& mapView, const QList<QGeoCoordinateMaps>& polygon,
                         const QPen& pen, const QBrush& brush, quint16 layerIndex)
        : QMapObject(mapView, QMapObject::Polygon, layerIndex),
        poly(polygon), p(pen), br(brush)
{
}

void QMapPolygon::compMapCoords()
{
    if (poly.count() < 2)
        return;

    QPolygonF pgn;

    for (int i = 0; i < poly.count(); i++) {
        pgn << mapView.geoToMap(poly[i]);
    }

    pgn << mapView.geoToMap(poly[0]);
    path = QPainterPath();
    path.addPolygon(pgn);
    //compute intersecting map tiles now
    intersectingTiles.clear();
    compIntersectingTiles(path.boundingRect());
}

void QMapPolygon::paint(QPainter* painter, const QRectF& viewPort)
{
    QPen oldPen = painter->pen();
    QBrush oldBrush = painter->brush();
    painter->setPen(p);
    painter->setBrush(br);
    path.translate(-viewPort.topLeft());
    painter->drawPath(path);
    qint64 mapWidth = static_cast<qint64>(mapView.mapWidth());

    //Is view port wrapping around date line?
    if (viewPort.right() >= mapWidth) {
        path.translate(mapWidth, 0);
        painter->drawPath(path);
        path.translate(-mapWidth, 0);
    }

    path.translate(viewPort.topLeft());
    painter->setPen(oldPen);
    painter->setBrush(oldBrush);
}

bool QMapPolygon::intersects(const QRectF& rect) const
{
    return path.intersects(rect);
}

QTM_END_NAMESPACE

