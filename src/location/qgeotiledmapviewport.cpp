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

#include "qgeotiledmapviewport.h"
#include "qgeotiledmapviewport_p.h"

#include "qgeotiledmappingmanagerengine.h"

#include <QDebug>

#define DEFAULT_ZOOMLEVEL 8
#define PI 3.14159265
#include <math.h>

QTM_BEGIN_NAMESPACE

QGeoTiledMapViewport::QGeoTiledMapViewport(QGeoMappingManagerEngine *engine, QGeoMapWidget *widget)
        : QGeoMapViewport(new QGeoTiledMapViewportPrivate(), engine, widget)
{
    Q_D(QGeoTiledMapViewport);
    QGeoTiledMappingManagerEngine *tileEngine = static_cast<QGeoTiledMappingManagerEngine *>(d->engine);
    QGeoMapViewport::setZoomLevel(DEFAULT_ZOOMLEVEL);
    d->width = (1 << DEFAULT_ZOOMLEVEL) * tileEngine->tileSize().width();
    d->height = (1 << DEFAULT_ZOOMLEVEL) * tileEngine->tileSize().height();
}

QGeoTiledMapViewport::~QGeoTiledMapViewport()
{
}

QPointF QGeoTiledMapViewport::coordinateToScreenPosition(const QGeoCoordinate &coordinate) const
{
    Q_D(const QGeoTiledMapViewport);

    qulonglong worldX;
    qulonglong worldY;

    coordinateToWorldPixel(coordinate, &worldX, &worldY);
    QPointF pos(worldX, worldY);
    pos -= d->screenRect.topLeft();
    return pos;
}

QGeoCoordinate QGeoTiledMapViewport::screenPositionToCoordinate(const QPointF &screenPosition) const
{
    Q_D(const QGeoTiledMapViewport);

    qulonglong worldX = static_cast<qulonglong>(d->screenRect.left() + screenPosition.x()) % d->width;
    qulonglong worldY = static_cast<qulonglong>(d->screenRect.top() + screenPosition.y()) % d->height;

    return worldPixelToCoordinate(worldX, worldY);
}

/*!
    Returns the (row,col) index of the tile that \a screenPosition lies on.
    The screen position (0,0) represents the top-left corner of the current map view.
*/
QPoint QGeoTiledMapViewport::screenPositionToTileIndices(const QPointF &screenPosition) const
{
    Q_D(const QGeoTiledMapViewport);

    //TODO: add some type checking
    QGeoTiledMappingManagerEngine *tileEngine = static_cast<QGeoTiledMappingManagerEngine *>(d->engine);
    QPointF pos = d->screenRect.topLeft() + screenPosition;

    int tileX = pos.x() / tileEngine->tileSize().width();
    int tileY = pos.y() / tileEngine->tileSize().height();

    return QPoint(tileX, tileY);
}

void QGeoTiledMapViewport::coordinateToWorldPixel(const QGeoCoordinate &coordinate, qulonglong *x, qulonglong *y) const
{
    Q_D(const QGeoTiledMapViewport);

    double lng = coordinate.longitude(); //x
    double lat = coordinate.latitude(); //y

    lng = lng / 360.0 + 0.5;

    lat = 0.5 - (log(tan((PI / 4.0) + (PI / 2.0) * lat / 180.0)) / PI) / 2.0;
    lat = qMax(0.0, lat);
    lat = qMin(1.0, lat);

    *x = qRound64(lng * qreal(d->width));
    *y = qRound64(lat * qreal(d->height));
}


qreal rmod(const qreal a, const qreal b)
{
    quint64 div = static_cast<quint64>(a / b);
    return a - static_cast<qreal>(div) * b;
}

QGeoCoordinate QGeoTiledMapViewport::worldPixelToCoordinate(qulonglong x, qulonglong y) const
{
    Q_D(const QGeoTiledMapViewport);

    qreal fx = qreal(x) / qreal(d->width);
    qreal fy = qreal(y) / qreal(d->height);

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

void QGeoTiledMapViewport::setCenter(const QGeoCoordinate &center)
{
    Q_D(QGeoTiledMapViewport);

    d->protectRegion = QRectF();
    qulonglong x;
    qulonglong y;
    coordinateToWorldPixel(center, &x, &y);
    d->screenRect.moveCenter(QPointF(x, y));
}

QGeoCoordinate QGeoTiledMapViewport::center() const
{
    Q_D(const QGeoTiledMapViewport);

    QPointF center = d->screenRect.center();
    return worldPixelToCoordinate(center.x(), center.y());
}

void QGeoTiledMapViewport::setZoomLevel(qreal zoomLevel)
{
    Q_D(QGeoTiledMapViewport);

    int zoomDiff = qRound(zoomLevel - QGeoMapViewport::zoomLevel());

    if (zoomDiff == 0)
        return;

    QGeoMapViewport::setZoomLevel(zoomLevel);

    d->protectRegion = QRectF();

    //compute new screenRect
    qreal ratioX = d->screenRect.center().x() / d->width;
    qreal ratioY = d->screenRect.center().y() / d->height;
    //TODO: add some type checking
    QGeoTiledMappingManagerEngine *tileEngine = static_cast<QGeoTiledMappingManagerEngine *>(d->engine);
    d->width = (1 << static_cast<int>(zoomLevel)) * tileEngine->tileSize().width();
    d->height = (1 << static_cast<int>(zoomLevel)) * tileEngine->tileSize().height();
    QPointF newCenter(qRound(ratioX * d->width), qRound(ratioY * d->height));
    d->screenRect.moveCenter(newCenter);

    //scale old image
    QRectF target = d->mapImage.rect();
    qreal width = target.width() / (1 << qAbs(zoomDiff));
    qreal height = target.width() / (1 << qAbs(zoomDiff));
    qreal x = target.x() + ((target.width() - width) / 2.0);
    qreal y = target.y() + ((target.height() - height) / 2.0);
    QRectF source = QRectF(x, y, width, height);

    QPixmap pm(d->mapImage.size());
    QPainter painter(&pm);

    if (zoomDiff < 0) {
        painter.drawPixmap(source, d->mapImage, target);
    } else {
        painter.drawPixmap(target, d->mapImage, source);
    }

    setMapImage(pm);
}

void QGeoTiledMapViewport::setViewportSize(const QSizeF &size)
{
    Q_D(QGeoTiledMapViewport);

    d->protectRegion = d->screenRect;
    d->screenRect.setSize(size);
    QGeoMapViewport::setViewportSize(size);

    QPixmap pm(size.toSize());
    QPainter p(&pm);
    if (!d->mapImage.isNull())
        p.drawPixmap(d->mapImage.rect(), d->mapImage, d->mapImage.rect());
    setMapImage(pm);
}

void QGeoTiledMapViewport::pan(int dx, int dy)
{
    Q_D(QGeoTiledMapViewport);

    QRectF oldScreenRect(d->screenRect);
    d->screenRect.translate(dx, dy);

    //Have we crossed the dateline from east-->west (i.e. "left edge")
    while (d->screenRect.left() < 0) {
        d->screenRect.translate(d->width, 0);
    }

    //Have we crossed the dateline from west-->east (i.e. "right edge")
    if (d->screenRect.left()>= d->width) {
        d->screenRect.moveLeft(static_cast<qulonglong>(d->screenRect.left()) % d->width);
    }

    qreal deltaX = oldScreenRect.left() - d->screenRect.left();
    qreal deltaY = oldScreenRect.top() - d->screenRect.top();
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

    QRectF source = QRectF(sx, sy, d->mapImage.width(), d->mapImage.height());
    QRectF target = QRectF(tx, ty, d->mapImage.width(), d->mapImage.height());

    QPixmap pm(d->mapImage.size());
    QPainter p(&pm);
    if (!d->mapImage.isNull()) {
        p.drawPixmap(target, d->mapImage, source);
        //TODO: is this correct?
        d->protectRegion = target.translated(d->screenRect.topLeft());
        setMapImage(pm);
    }
}

QRectF QGeoTiledMapViewport::screenRect() const
{
    Q_D(const QGeoTiledMapViewport);
    return d->screenRect;
}

QRectF QGeoTiledMapViewport::protectedRegion() const
{
    Q_D(const QGeoTiledMapViewport);
    return d->protectRegion;
}

void QGeoTiledMapViewport::clearProtectedRegion()
{
    Q_D(QGeoTiledMapViewport);
    d->protectRegion = QRectF();
}

/*******************************************************************************
*******************************************************************************/

QGeoTiledMapViewportPrivate::QGeoTiledMapViewportPrivate()
        : QGeoMapViewportPrivate() {}

QGeoTiledMapViewportPrivate::QGeoTiledMapViewportPrivate(const QGeoTiledMapViewportPrivate &other)
        : QGeoMapViewportPrivate(other),
        width(other.width),
        height(other.height),
        protectRegion(other.protectRegion),
        screenRect(other.screenRect) {}

QGeoTiledMapViewportPrivate::~QGeoTiledMapViewportPrivate() {}

QGeoTiledMapViewportPrivate& QGeoTiledMapViewportPrivate::operator= (const QGeoTiledMapViewportPrivate & other)
{
    QGeoMapViewportPrivate::operator =(other);

    width = other.width;
    height = other.height;
    protectRegion = other.protectRegion;
    screenRect = other.screenRect;

    return *this;
}

QTM_END_NAMESPACE
