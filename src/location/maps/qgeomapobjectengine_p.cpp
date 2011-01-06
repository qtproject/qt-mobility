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

#include "qgeomapobjectengine_p.h"
#include "projwrapper_p.h"

#include <QTransform>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsPolygonItem>
#include <QGraphicsPathItem>

QTM_BEGIN_NAMESPACE

QGeoMapObjectEngine::QGeoMapObjectEngine(QGeoMapData *mapData, QGeoMapDataPrivate *mapDataP) :
    md(mapData),
    mdp(mapDataP)
{
}

QGeoMapObjectEngine::~QGeoMapObjectEngine()
{}

static QGraphicsPolygonItem *polyCopy(const QGraphicsPolygonItem *polyItem)
{
    QGraphicsPolygonItem *pi = new QGraphicsPolygonItem;
    pi->setBrush(polyItem->brush());
    pi->setPen(polyItem->pen());
    pi->setVisible(polyItem->isVisible());
    pi->setFillRule(polyItem->fillRule());
    pi->setOpacity(polyItem->opacity());
    pi->setPolygon(polyItem->polygon());
    pi->setGraphicsEffect(polyItem->graphicsEffect());
    return pi;
}

static QGraphicsPathItem *pathCopy(const QGraphicsPathItem *pathItem)
{
    QGraphicsPathItem *pi = new QGraphicsPathItem;
    pi->setBrush(pathItem->brush());
    pi->setPen(pathItem->pen());
    pi->setVisible(pathItem->isVisible());
    pi->setOpacity(pathItem->opacity());
    pi->setGraphicsEffect(pathItem->graphicsEffect());
    pi->setPath(pathItem->path());
    return pi;
}

bool QGeoMapObjectEngine::exactMetersToSeconds(const QGeoCoordinate &origin,
                                              QGeoMapObject *object,
                                              QGraphicsItem *item,
                                              QList<QPolygonF> &polys)
{
    QString projStr = "+proj=tmerc +lat_0=%1 +lon_0=%2 +k=1.0 +x_0=0 +y_0=0 +ellps=WGS84";
    projStr = projStr.arg(origin.latitude(), 0, 'f', 12)
                     .arg(origin.longitude(), 0, 'f', 12);

    ProjCoordinateSystem localSys(projStr, false);
    ProjCoordinateSystem wgs84("+proj=latlon +ellps=WGS84");

    QTransform west;
    west.translate(360.0 * 3600.0, 0.0);

    QTransform east;
    east.translate(-360.0 * 3600.0, 0.0);

    QGraphicsPolygonItem *polyItem = dynamic_cast<QGraphicsPolygonItem*>(item);
    if (polyItem) {
        QPolygonF poly = polyItem->polygon() * polyItem->transform();

        ProjPolygon p(poly, localSys);
        p.convert(wgs84);
        QPolygonF wgs = p.toPolygonF(3600.0);

        latLonExact.remove(object);
        QGraphicsPolygonItem *pi = polyCopy(polyItem);
        pi->setPolygon(wgs);
        latLonExact.insertMulti(object, pi);
        polys << pi->boundingRect();

        QPolygonF westPoly = wgs * west;
        pi = polyCopy(polyItem);
        pi->setPolygon(westPoly);
        latLonExact.insertMulti(object, pi);
        polys << pi->boundingRect();

        QPolygonF eastPoly = wgs * east;
        pi = polyCopy(polyItem);
        pi->setPolygon(eastPoly);
        latLonExact.insertMulti(object, pi);
        polys << pi->boundingRect();

        return true;
    }

    QGraphicsPathItem *pathItem = dynamic_cast<QGraphicsPathItem*>(item);
    if (pathItem) {
        QPainterPath path = pathItem->path() * pathItem->transform();

        for (int i = 0; i < path.elementCount(); ++i) {
            QPainterPath::Element e = path.elementAt(i);

            ProjCoordinate c(e.x, e.y, 0.0, localSys);
            Q_ASSERT(c.convert(wgs84));

            path.setElementPositionAt(i, c.x() * 3600.0, c.y() * 3600.0);
        }

        latLonExact.remove(object);


        QGraphicsPathItem *pi = pathCopy(pathItem);
        pi->setPath(path);
        latLonExact.insertMulti(object, pi);
        polys << pi->boundingRect();

        QPainterPath westPath = path * west;
        pi = pathCopy(pathItem);
        pi->setPath(westPath);
        latLonExact.insertMulti(object, pi);
        polys << pi->boundingRect();

        QPainterPath eastPath = path * east;
        pi = pathCopy(pathItem);
        pi->setPath(eastPath);
        latLonExact.insertMulti(object, pi);
        polys << pi->boundingRect();

        return true;
    }

    qWarning("QGeoMapData: did not recognize type of exact-transformed"
             "object: type #%d (object not supported for exact transform)",
             item->type());
    return false;
}

bool QGeoMapObjectEngine::exactSecondsToSeconds(const QGeoCoordinate &origin,
                                               QGeoMapObject *object,
                                               QGraphicsItem *item,
                                               QList<QPolygonF> &polys)
{
    QTransform west;
    west.translate(360.0 * 3600.0, 0.0);

    QTransform east;
    east.translate(-360.0 * 3600.0, 0.0);

    QTransform toAbs;
    if (object->units() == QGeoMapObject::RelativeArcSecondUnit) {
        double ox = origin.longitude() * 3600.0;
        double oy = origin.latitude() * 3600.0;
        toAbs.translate(ox, oy);
    }

    QGraphicsPolygonItem *polyItem = dynamic_cast<QGraphicsPolygonItem*>(item);
    if (polyItem) {
        if (polyItem->polygon().isEmpty() || polyItem->polygon().size() < 3)
            return false;

        QPolygonF poly = polyItem->polygon() * polyItem->transform();
        poly = poly * toAbs;

        latLonExact.remove(object);
        QGraphicsPolygonItem *pi = polyCopy(polyItem);
        pi->setPolygon(poly);
        latLonExact.insertMulti(object, pi);
        polys << pi->boundingRect();

        QPolygonF westPoly = poly * west;
        pi = polyCopy(polyItem);
        pi->setPolygon(westPoly);
        latLonExact.insertMulti(object, pi);
        polys << pi->boundingRect();

        QPolygonF eastPoly = poly * east;
        pi = polyCopy(polyItem);
        pi->setPolygon(eastPoly);
        latLonExact.insertMulti(object, pi);
        polys << pi->boundingRect();

        return true;
    }

    QGraphicsPathItem *pathItem = dynamic_cast<QGraphicsPathItem*>(item);
    if (pathItem) {
        if (pathItem->path().isEmpty() || pathItem->path().elementCount() < 2)
            return false;

        QPainterPath path = pathItem->path() * pathItem->transform();
        path = path * toAbs;

        latLonExact.remove(object);
        QGraphicsPathItem *pi = pathCopy(pathItem);
        pi->setPath(path);
        latLonExact.insertMulti(object, pi);
        polys << pi->boundingRect();

        QPainterPath westPath = path * west;
        pi = pathCopy(pathItem);
        pi->setPath(westPath);
        latLonExact.insertMulti(object, pi);
        polys << pi->boundingRect();

        QPainterPath eastPath = path * east;
        pi = pathCopy(pathItem);
        pi->setPath(eastPath);
        latLonExact.insertMulti(object, pi);
        polys << pi->boundingRect();

        return true;
    }

    qWarning("QGeoMapData: did not recognize type of exact-transformed "
             "object: type #%d (object not supported for exact transform)",
             item->type());
    return false;
}

void QGeoMapObjectEngine::bilinearMetersToSeconds(const QGeoCoordinate &origin,
                                                 QGraphicsItem *item,
                                                 QPolygonF &local,
                                                 QTransform &latLon)
{
    QString projStr = "+proj=tmerc +lat_0=%1 +lon_0=%2 +k=1.0 +x_0=0 +y_0=0 +ellps=WGS84";
    projStr = projStr.arg(origin.latitude(), 0, 'f', 12)
                     .arg(origin.longitude(), 0, 'f', 12);

    ProjCoordinateSystem localSys(projStr, false);
    ProjCoordinateSystem wgs84("+proj=latlon +ellps=WGS84");

    ProjPolygon p(local, localSys);
    if (!p.convert(wgs84)) {
        qWarning("QGeoMapData: bilinear transform from meters to arc-seconds "
                 "failed: projection is singular");
        return;
    }
    QPolygonF wgs = p.toPolygonF(3600.0);

    // QTransform expects the last vertex (closing vertex) to be dropped
    local.remove(4);
    wgs.remove(4);

    // perform wrapping
    if (wgs.at(2).x() < wgs.at(3).x()) {
        QPointF topRight = wgs.at(1);
        topRight.setX(topRight.x() + 360.0 * 3600.0);
        wgs.replace(1, topRight);

        QPointF bottomRight = wgs.at(2);
        bottomRight.setX(bottomRight.x() + 360.0 * 3600.0);
        wgs.replace(2, bottomRight);
    }

    bool ok = QTransform::quadToQuad(local, wgs, latLon);
    if (!ok) {
        qWarning("QGeoMapData: bilinear transform from meters to arc-seconds "
                 "failed: could not obtain a transformation matrix");
        return;
    }

    latLon = item->transform() * latLon;
}

void QGeoMapObjectEngine::bilinearPixelsToSeconds(const QGeoCoordinate &origin,
                                                 QGraphicsItem *item,
                                                 QPolygonF &local,
                                                 QTransform &latLon)
{
    QPointF pixelOrigin = this->coordinateToScreenPosition(origin.longitude(),
                                                           origin.latitude());

    QPolygonF wgs;
    foreach (const QPointF &pt, local) {
        const QGeoCoordinate coord =
                md->screenPositionToCoordinate(pt + pixelOrigin);
        const QPointF lpt(coord.longitude() * 3600.0, coord.latitude() * 3600.0);
        wgs.append(lpt);
    }

    // QTransform expects the last vertex (closing vertex) to be dropped
    local.remove(4);
    wgs.remove(4);

    // perform wrapping
    if (wgs.at(2).x() < wgs.at(3).x()) {
        QPointF topRight = wgs.at(1);
        topRight.setX(topRight.x() + 360.0 * 3600.0);
        wgs.replace(1, topRight);

        QPointF bottomRight = wgs.at(2);
        bottomRight.setX(bottomRight.x() + 360.0 * 3600.0);
        wgs.replace(2, bottomRight);
    }

    bool ok = QTransform::quadToQuad(local, wgs, latLon);
    if (!ok) {
        qWarning("QGeoMapData: bilinear transform from meters to arc-seconds "
                 "failed: could not obtain a transformation matrix");
        return;
    }

    latLon = item->transform() * latLon;
}

void QGeoMapObjectEngine::bilinearSecondsToScreen(const QGeoCoordinate &origin,
                                                 QGeoMapObject *object,
                                                 QList<QPolygonF> &polys)
{
    QList<QTransform> latLons = latLonTrans.values(object);

    // compute the transform by linearising from the lat/lon space
    foreach (QTransform latLon, latLons) {
        QTransform pixel;

        QGraphicsItem *item = object->graphicsItem();
        QPolygonF local = item->boundingRect();
        QPolygonF latLonPoly = latLon.map(local);

        QPolygonF pixelPoly = mdp->polyToScreen(latLonPoly);

        // QTransform expects the last vertex (closing vertex) to be dropped
        local.remove(4);
        pixelPoly.remove(4);

        bool ok = QTransform::quadToQuad(local, pixelPoly, pixel);
        if (!ok) {
            qWarning("QGeoMapData: bilinear transform to screen from arc-seconds "
                     "failed: could not compute transformation matrix");
            return;
        }

        pixelTrans.insertMulti(object, pixel);

        polys << pixelPoly;
    }
}

void QGeoMapObjectEngine::exactPixelMap(const QGeoCoordinate &origin,
                                       QGeoMapObject *object,
                                       QList<QPolygonF> &polys)
{
    QList<QGraphicsItem*> latLonItems = latLonExact.values(object);

    pixelExact.remove(object);

    foreach (QGraphicsItem *latLonItem, latLonItems) {
        QGraphicsPolygonItem *polyItem = dynamic_cast<QGraphicsPolygonItem*>(latLonItem);
        if (polyItem) {
            QPolygonF poly = polyItem->polygon();
            QPolygonF pixelPoly = mdp->polyToScreen(poly);

            QGraphicsPolygonItem *pi = polyCopy(polyItem);
            pi->setPolygon(pixelPoly);
            pixelExact.insertMulti(object, pi);
            polys << pi->boundingRect();
        }

        QGraphicsPathItem *pathItem = dynamic_cast<QGraphicsPathItem*>(latLonItem);
        if (pathItem) {
            QPainterPath path = pathItem->path();

            for (int i = 0; i < path.elementCount(); ++i) {
                QPainterPath::Element e = path.elementAt(i);

                double x = e.x;
                x /= 3600.0;
                double y = e.y;
                y /= 3600.0;

                QPointF pixel = mdp->coordinateToScreenPosition(x, y);

                path.setElementPositionAt(i, pixel.x(), pixel.y());
            }

            QGraphicsPathItem *pi = pathCopy(pathItem);
            pi->setPath(path);
            pixelExact.insertMulti(object, pi);
            polys << QPolygonF(pi->boundingRect());
        }
    }
}

void QGeoMapObjectEngine::pixelShiftToScreen(const QGeoCoordinate &origin,
                                            QGeoMapObject *object,
                                            QList<QPolygonF> &polys)
{
    // compute the transform as an origin shift
    QList<QPointF> origins;
    origins << QPointF(origin.longitude(), origin.latitude());
    origins << QPointF(origin.longitude() + 360.0, origin.latitude());
    origins << QPointF(origin.longitude() - 360.0, origin.latitude());

    foreach (QPointF o, origins) {
        QTransform pixel = object->graphicsItem()->transform();
        QPointF pixelOrigin = mdp->coordinateToScreenPosition(o.x(), o.y());
        pixel.translate(pixelOrigin.x(), pixelOrigin.y());
        pixelTrans.insertMulti(object, pixel);
        polys << pixel.map(object->graphicsItem()->boundingRect());
    }
}

static void _zoomDepsRecurse(QGeoMapObjectEngine *eng, QGeoMapGroupObject *group)
{
    foreach (QGeoMapObject *obj, group->childObjects()) {
        QGeoMapGroupObject *subgroup = qobject_cast<QGeoMapGroupObject*>(obj);
        if (subgroup) {
            _zoomDepsRecurse(eng, subgroup);
        } else {
            if (obj->units() == QGeoMapObject::PixelUnit) {
                eng->objectsForLatLonUpdate << obj;
                eng->objectsForPixelUpdate << obj;
            }
        }
    }
}

void QGeoMapObjectEngine::invalidateZoomDependents()
{
    _zoomDepsRecurse(this, mdp->containerObject);
}

// update the transform tables as necessary
void QGeoMapObjectEngine::updateTransforms()
{
    foreach (QGeoMapObject *obj, objectsForLatLonUpdate) {
        QGeoMapGroupObject *group = qobject_cast<QGeoMapGroupObject*>(obj);
        if (group)
            updateLatLonTransforms(group);
        else
            updateLatLonTransform(obj);
    }

    objectsForLatLonUpdate.clear();

    foreach (QGeoMapObject *obj, objectsForPixelUpdate) {
        QGeoMapGroupObject *group = qobject_cast<QGeoMapGroupObject*>(obj);
        if (group)
            updatePixelTransforms(group);
        else
            updatePixelTransform(obj);
    }

    objectsForPixelUpdate.clear();
}

void QGeoMapObjectEngine::invalidatePixelsForViewport()
{
    QPolygonF view = mdp->latLonViewport();

    QList<QGraphicsItem*> itemsInView;
    itemsInView = latLonScene->items(view, Qt::IntersectsItemShape,
                                     Qt::AscendingOrder);

    QSet<QGeoMapObject*> done = objectsForPixelUpdate.toSet();

    foreach (QGraphicsItem *latLonItem, itemsInView) {
        QGeoMapObject *object = latLonItems.value(latLonItem);
        Q_ASSERT(object);
        if (!done.contains(object)) {
            objectsForPixelUpdate << object;
            done.insert(object);
        }
    }
}

void QGeoMapObjectEngine::updatePixelsForGroup(QGeoMapGroupObject *group)
{
    foreach (QGeoMapObject *obj, group->childObjects()) {
        QGeoMapGroupObject *subgroup = qobject_cast<QGeoMapGroupObject*>(obj);
        if (subgroup) {
            updatePixelsForGroup(subgroup);
        } else {
            updatePixelTransform(obj);
        }
    }
}

void QGeoMapObjectEngine::updateLatLonsForGroup(QGeoMapGroupObject *group)
{
    foreach (QGeoMapObject *object, group->childObjects()) {
        QGeoMapGroupObject *subGroup = qobject_cast<QGeoMapGroupObject*>(object);
        if (subGroup)
            updateLatLonsForGroup(subGroup);
        else
            updateLatLonTransform(object);
    }
}

#include "moc_qgeomapobjectengine_p.cpp"

QTM_END_NAMESPACE
