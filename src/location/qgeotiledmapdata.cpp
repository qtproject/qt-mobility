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

#include "qgeotiledmapdata.h"
#include "qgeotiledmapdata_p.h"

#include "qgeotiledmappingmanagerengine.h"
#include "qgeotiledmappingmanagerengine_p.h"
#include "qgeocoordinate.h"
#include "qgeoboundingbox.h"
#include "qgeomaprectangleobject.h"

#include <QDebug>

#define DEFAULT_ZOOMLEVEL 8
#define PI 3.14159265
#include <math.h>

QTM_BEGIN_NAMESPACE

QGeoTiledMapData::QGeoTiledMapData(QGeoMappingManagerEngine *engine, QGeoMapWidget *widget)
        : QGeoMapData(engine, widget),
        d_ptr(new QGeoTiledMapDataPrivate(this))
{
    QGeoTiledMappingManagerEngine *tileEngine = static_cast<QGeoTiledMappingManagerEngine *>(QGeoMapData::engine());
    QGeoMapData::setZoomLevel(DEFAULT_ZOOMLEVEL);
    d_ptr->width = (1 << DEFAULT_ZOOMLEVEL) * tileEngine->tileSize().width();
    d_ptr->height = (1 << DEFAULT_ZOOMLEVEL) * tileEngine->tileSize().height();
}

QGeoTiledMapData::~QGeoTiledMapData()
{
}

QPointF QGeoTiledMapData::coordinateToScreenPosition(const QGeoCoordinate &coordinate) const
{
    qulonglong worldX;
    qulonglong worldY;

    coordinateToWorldPixel(coordinate, &worldX, &worldY);
    QPointF pos(worldX, worldY);
    pos -= d_ptr->screenRect.topLeft();
    return pos;
}

QGeoCoordinate QGeoTiledMapData::screenPositionToCoordinate(const QPointF &screenPosition) const
{
    qulonglong worldX = static_cast<qulonglong>(d_ptr->screenRect.left() + screenPosition.x()) % d_ptr->width;
    qulonglong worldY = static_cast<qulonglong>(d_ptr->screenRect.top() + screenPosition.y()) % d_ptr->height;

    return worldPixelToCoordinate(worldX, worldY);
}

/*!
    Returns the (row,col) index of the tile that \a screenPosition lies on.
    The screen position (0,0) represents the top-left corner of the current map view.
*/
QPoint QGeoTiledMapData::screenPositionToTileIndices(const QPointF &screenPosition) const
{
    //TODO: add some type checking
    QGeoTiledMappingManagerEngine *tileEngine = static_cast<QGeoTiledMappingManagerEngine *>(QGeoMapData::engine());
    QPointF pos = d_ptr->screenRect.topLeft() + screenPosition;

    int numCols = 1 << static_cast<int>(zoomLevel());
    int tileX = static_cast<int>(pos.x() / tileEngine->tileSize().width()) % numCols;
    int tileY = static_cast<int>(pos.y() / tileEngine->tileSize().height()) % numCols;

    return QPoint(tileX, tileY);
}

void QGeoTiledMapData::coordinateToWorldPixel(const QGeoCoordinate &coordinate, qulonglong *x, qulonglong *y) const
{
    double lng = coordinate.longitude(); //x
    double lat = coordinate.latitude(); //y

    lng = lng / 360.0 + 0.5;

    lat = 0.5 - (log(tan((PI / 4.0) + (PI / 2.0) * lat / 180.0)) / PI) / 2.0;
    lat = qMax(0.0, lat);
    lat = qMin(1.0, lat);

    *x = qRound64(lng * qreal(d_ptr->width));
    *y = qRound64(lat * qreal(d_ptr->height));
}


qreal rmod(const qreal a, const qreal b)
{
    quint64 div = static_cast<quint64>(a / b);
    return a - static_cast<qreal>(div) * b;
}

QGeoCoordinate QGeoTiledMapData::worldPixelToCoordinate(qulonglong x, qulonglong y) const
{
    qreal fx = qreal(x) / qreal(d_ptr->width);
    qreal fy = qreal(y) / qreal(d_ptr->height);

    if (fy < 0.0f)
        fy = 0.0f;
    else if (fy > 1.0f)
        fy = 1.0f;

    qreal lat;

    if (fy == 0.0f)
        lat = 90.0f;
    else if (fy == 1.0f)
        lat = -90.0f;
    else
        lat = (180.0f / PI) * (2.0f * atan(exp(PI * (1.0f - 2.0f * fy))) - (PI / 2.0f));

    qreal lng;
    if (fx >= 0) {
        lng = rmod(fx, 1.0f);
    } else {
        lng = rmod(1.0f - rmod(-1.0f * fx, 1.0f), 1.0f);
    }

    lng = lng * 360.0f - 180.0f;

    return QGeoCoordinate(lat, lng);
}

void QGeoTiledMapData::setCenter(const QGeoCoordinate &center)
{
    d_ptr->protectRegion = QRectF();
    qulonglong x;
    qulonglong y;
    coordinateToWorldPixel(center, &x, &y);
    d_ptr->screenRect.moveCenter(QPointF(x, y));
    d_ptr->screenRect.moveLeft(qRound64(d_ptr->screenRect.left()));
    d_ptr->screenRect.moveTop(qRound64(d_ptr->screenRect.top()));
}

QGeoCoordinate QGeoTiledMapData::center() const
{
    QPointF center = d_ptr->screenRect.center();
    return worldPixelToCoordinate(center.x(), center.y());
}

void QGeoTiledMapData::setZoomLevel(qreal zoomLevel)
{
    qreal oldZoomLevel = QGeoMapData::zoomLevel();

    QGeoMapData::setZoomLevel(zoomLevel);

    // QGeoMapData::setZoomLevel clips the zoom level to be
    // in between the minimum and maximum zoom levels
    zoomLevel = QGeoMapData::zoomLevel();

    int zoomDiff = qRound(zoomLevel - oldZoomLevel);

    if (zoomDiff == 0)
        return;

    d_ptr->protectRegion = QRectF();

    QGeoCoordinate currCenter = center();
    //TODO: add some type checking
    QGeoTiledMappingManagerEngine *tileEngine = static_cast<QGeoTiledMappingManagerEngine *>(QGeoMapData::engine());
    d_ptr->width = (1 << static_cast<int>(zoomLevel)) * tileEngine->tileSize().width();
    d_ptr->height = (1 << static_cast<int>(zoomLevel)) * tileEngine->tileSize().height();

    setCenter(currCenter);

    //scale old image
    QRectF target = QGeoMapData::mapImage().rect();
    qreal width = target.width() / (1 << qAbs(zoomDiff));
    qreal height = target.width() / (1 << qAbs(zoomDiff));
    qreal x = target.x() + ((target.width() - width) / 2.0);
    qreal y = target.y() + ((target.height() - height) / 2.0);
    QRectF source = QRectF(x, y, width, height);

    QPixmap pm(QGeoMapData::mapImage().size());
    QPainter painter(&pm);

    if (zoomDiff < 0) {
        painter.drawPixmap(source, QGeoMapData::mapImage(), target);
    } else {
        painter.drawPixmap(target, QGeoMapData::mapImage(), source);
    }

    setMapImage(pm);
}

void QGeoTiledMapData::setViewportSize(const QSizeF &size)
{
    //d_ptr->protectRegion = d_ptr->screenRect;
    d_ptr->screenRect.setSize(size);
    QGeoMapData::setViewportSize(size);

    QPixmap pm(size.toSize());
    QPainter p(&pm);
    if (!QGeoMapData::mapImage().isNull())
        p.drawPixmap(QGeoMapData::mapImage().rect(), QGeoMapData::mapImage(), QGeoMapData::mapImage().rect());
    setMapImage(pm);
}

void QGeoTiledMapData::pan(int dx, int dy)
{
    QRectF oldScreenRect(d_ptr->screenRect);
    d_ptr->screenRect.translate(dx, dy);

    //Have we crossed the dateline from east-->west (i.e. "left edge")
    while (d_ptr->screenRect.left() < 0) {
        d_ptr->screenRect.translate(d_ptr->width, 0);
    }

    //Have we crossed the dateline from west-->east (i.e. "right edge")
    if (d_ptr->screenRect.left()>= d_ptr->width) {
        d_ptr->screenRect.moveLeft(static_cast<qulonglong>(d_ptr->screenRect.left()) % d_ptr->width);
    }

    qreal deltaX = oldScreenRect.left() - d_ptr->screenRect.left();
    qreal deltaY = oldScreenRect.top() - d_ptr->screenRect.top();
    qreal sx;
    qreal sy;
    qreal tx;
    qreal ty;

    //TODO: make this work in case we wrapped around the date line
    if (deltaX >= 0) {
        sx = 0.0;
        tx = deltaX;
    } else {
        sx = qAbs(deltaX);
        tx = 0.0;
    }

    if (deltaY >= 0) {
        sy = 0.0;
        ty = deltaY;
    } else {
        sy = qAbs(deltaY);
        ty = 0.0;
    }

    QRectF source = QRectF(sx, sy, QGeoMapData::mapImage().width(), QGeoMapData::mapImage().height());
    QRectF target = QRectF(tx, ty, QGeoMapData::mapImage().width(), QGeoMapData::mapImage().height());

    QPixmap pm(QGeoMapData::mapImage().size());
    QPainter p(&pm);
    if (!QGeoMapData::mapImage().isNull()) {
        p.drawPixmap(target, QGeoMapData::mapImage(), source);
        //TODO: is this correct?
        //d_ptr->protectRegion = target.translated(d_ptr->screenRect.topLeft());
        setMapImage(pm);
    }
}

QRectF QGeoTiledMapData::screenRect() const
{
    return d_ptr->screenRect;
}

QRectF QGeoTiledMapData::protectedRegion() const
{
    return d_ptr->protectRegion;
}

void QGeoTiledMapData::clearProtectedRegion()
{
    d_ptr->protectRegion = QRectF();
}

QList<QGeoMapObject*> QGeoTiledMapData::visibleMapObjects()
{
    return QList<QGeoMapObject*>();
}

QList<QGeoMapObject*> QGeoTiledMapData::mapObjectsAtScreenPosition(const QPointF &screenPosition, int radius)
{
    return QList<QGeoMapObject*>();
}

QList<QGeoMapObject*> QGeoTiledMapData::mapObjectsInScreenRect(const QRectF &screenRect)
{
    return QList<QGeoMapObject*>();
}

QPixmap QGeoTiledMapData::mapObjectsOverlay() const
{
    bool needsPainting = false;
    QPixmap overlay(d_ptr->screenRect.width(), d_ptr->screenRect.height());
    overlay.fill(Qt::transparent);
    QPainter painter(&overlay);

    QMapIterator<QGeoCompositeZValue, QGeoMapObject*> it(d_ptr->zOrderedObj);

    while (it.hasNext()) {
        it.next();
        QGeoMapObject *obj = it.value();

        if (d_ptr->intersects(obj, d_ptr->screenRect)) {
            needsPainting = true;
            d_ptr->paintMapObject(painter, obj);
        }
    }

    if (needsPainting)
        return overlay;

    return QPixmap();
}

void QGeoTiledMapData::addMapObject(QGeoMapObject *mapObject)
{
    //construct composite zValue first
    QGeoCompositeZValue zValue;
    QGeoMapObject* parent = mapObject->parentObject();

    if (parent && d_ptr->objToCompZValue.contains(parent))
        zValue = d_ptr->objToCompZValue[parent];

    zValue.compZValue.append(mapObject->zValue());
    //add to internal tables
    d_ptr->objToCompZValue.insert(mapObject, zValue);
    d_ptr->zOrderedObj.insert(zValue, mapObject);

    d_ptr->calculateInfo(mapObject);

    QGeoMapData::addMapObject(mapObject);
}

/*******************************************************************************
*******************************************************************************/

QGeoCompositeZValue::QGeoCompositeZValue() {}

QGeoCompositeZValue::QGeoCompositeZValue(const QGeoCompositeZValue &other)
    : compZValue(other.compZValue) 
{}

QGeoCompositeZValue& QGeoCompositeZValue::operator=(const QGeoCompositeZValue &other)
{
    compZValue = other.compZValue;
    return *this;
}

bool QGeoCompositeZValue::operator==(const QGeoCompositeZValue &other) const
{
    if (compZValue.size() != other.compZValue.size())
        return false;

    int sz = compZValue.size();

    for (int i = 0; i < sz; i++)

        if (compZValue.at(i) != other.compZValue.at(i))
            return false;

    return true;
}

bool QGeoCompositeZValue::operator<(const QGeoCompositeZValue &other) const
{
    int sz = compZValue.size();
    int otherSz = other.compZValue.size();
    
    for (int i = 0; i < sz && i < otherSz; i++)

        if (compZValue.at(i) < other.compZValue.at(i))
            return true;
        else if (compZValue.at(i) > other.compZValue.at(i))
            return false;

    return false;
}

uint qHash(const QGeoCompositeZValue &zValue)
{
    QString hashStr;
    int len = zValue.compZValue.size();

    for (int i = 0; i < len; i++)
        hashStr += QString::number(zValue.compZValue.at(i)) + "/";

    return qHash(hashStr);
}

/*******************************************************************************
*******************************************************************************/

QGeoTiledMapDataPrivate::QGeoTiledMapDataPrivate(QGeoTiledMapData *q)
    : q_ptr(q)
{}

QGeoTiledMapDataPrivate::QGeoTiledMapDataPrivate(const QGeoTiledMapDataPrivate &other)
        : width(other.width),
        height(other.height),
        protectRegion(other.protectRegion),
        screenRect(other.screenRect),
        q_ptr(other.q_ptr)
{}

QGeoTiledMapDataPrivate::~QGeoTiledMapDataPrivate() {}

QGeoTiledMapDataPrivate& QGeoTiledMapDataPrivate::operator= (const QGeoTiledMapDataPrivate & other)
{
    width = other.width;
    height = other.height;
    protectRegion = other.protectRegion;
    screenRect = other.screenRect;
    q_ptr = other.q_ptr;

    return *this;
}

qulonglong QGeoTiledMapDataPrivate::tileKey(int row, int col, int zoomLevel)
{
    qulonglong result = 1 << (zoomLevel);
    result *= row;
    result += col;
    return result;
}

bool QGeoTiledMapDataPrivate::intersects(const QGeoMapObject *mapObject, const QRectF &rect) const
{
    if (!mapObject)
        return false;

    if (!objInfo.contains(mapObject))
        return false;

    //TODO: consider dateline wrapping
    return rect.intersects(objInfo[mapObject]->boundingBox);
}

void QGeoTiledMapDataPrivate::paintMapObject(QPainter &painter, const QGeoMapObject *mapObject) const
{
    if (mapObject->type() == QGeoMapObject::RectangleType)
        paintMapRectangleObject(painter, static_cast<const QGeoMapRectangleObject*>(mapObject));
}

void QGeoTiledMapDataPrivate::paintMapRectangleObject(QPainter &painter, const QGeoMapRectangleObject *rectangle) const
{
    if (!objInfo.contains(rectangle))
        return;

    QGeoTiledMapObjectInfo* info = objInfo.value(rectangle);
    QRectF rect = info->boundingBox.translated(-(screenRect.topLeft()));
    painter.drawRect(rect);
}

void QGeoTiledMapDataPrivate::calculateInfo(const QGeoMapObject *mapObject)
{
    if (mapObject->type() == QGeoMapObject::RectangleType)
        calculateMapRectangleInfo(static_cast<const QGeoMapRectangleObject*>(mapObject));
}

void QGeoTiledMapDataPrivate::calculateMapRectangleInfo(const QGeoMapRectangleObject *rectangle)
{
    qulonglong topLeftX;
    qulonglong topLeftY;
    qulonglong bottomRightX;
    qulonglong bottomRightY;
    
    q_ptr->coordinateToWorldPixel(rectangle->boundingBox().topLeft(), &topLeftX, &topLeftY);
    q_ptr->coordinateToWorldPixel(rectangle->boundingBox().bottomRight(), &bottomRightX, &bottomRightY);

    if (objInfo.contains(rectangle))
        delete objInfo.take(rectangle);

    QGeoTiledMapObjectInfo* info = new QGeoTiledMapObjectInfo;
    info->boundingBox = QRectF(QPointF(topLeftX, topLeftY), QPointF(bottomRightX, bottomRightY));
    objInfo[rectangle] = info;
}

QTM_END_NAMESPACE
