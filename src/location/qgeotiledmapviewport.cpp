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

#define PI 3.14159265
#include <math.h>

QTM_BEGIN_NAMESPACE

QGeoTiledMapViewport::QGeoTiledMapViewport(QGeoMappingManagerEngine *engine, QGeoMapWidget *widget)
        : QGeoMapViewport(new QGeoTiledMapViewportPrivate(), engine, widget)
{
    Q_D(QGeoTiledMapViewport);
    QGeoTiledMappingManagerEngine *tileEngine = static_cast<QGeoTiledMappingManagerEngine *>(d->engine);
    d->width = (1 << qRound(engine->maximumZoomLevel())) * tileEngine->tileSize().width();
    d->height = (1 << qRound(engine->maximumZoomLevel())) * tileEngine->tileSize().height();
    setZoomLevel(0.0);
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

    qreal screenX = (qreal(worldX) - qreal(d->x)) / d->zoomFactor;
    qreal screenY = (qreal(worldY) - qreal(d->y)) / d->zoomFactor;

    return QPointF(screenX, screenY);
}

QGeoCoordinate QGeoTiledMapViewport::screenPositionToCoordinate(const QPointF &screenPosition) const
{
    Q_D(const QGeoTiledMapViewport);

    qulonglong worldX = (d->x + qRound64(screenPosition.x() * d->zoomFactor)) % d->width;
    qulonglong worldY = (d->y + qRound64(screenPosition.y() * d->zoomFactor)) % d->height;

    return worldPixelToCoordinate(worldX, worldY);
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
    coordinateToWorldPixel(center, &(d->x), &(d->y));
    d->x -= qRound(d->zoomFactor * (d->viewportSize.width() / 2.0));
    d->y -= qRound(d->zoomFactor * (d->viewportSize.height() / 2.0));
}

QGeoCoordinate QGeoTiledMapViewport::center() const
{
    Q_D(const QGeoTiledMapViewport);

    qulonglong cx = d->x + qRound(d->zoomFactor * (d->viewportSize.width() / 2.0));
    qulonglong cy = d->y + qRound(d->zoomFactor * (d->viewportSize.height() / 2.0));
    return worldPixelToCoordinate(cx, cy);
}

void QGeoTiledMapViewport::setZoomLevel(qreal zoomLevel)
{
    Q_D(QGeoTiledMapViewport);
    int zoomDiff = qRound(zoomLevel - QGeoMapViewport::zoomLevel());

    QGeoMapViewport::setZoomLevel(zoomLevel);

    qulonglong cx = d->x + qRound(d->zoomFactor * (d->viewportSize.width() / 2.0));
    qulonglong cy = d->y + qRound(d->zoomFactor * (d->viewportSize.height() / 2.0));

    d->zoomFactor = static_cast<qulonglong>(1) << qRound64(d->engine->maximumZoomLevel() - d->zoomLevel);

    d->x = cx - qRound(d->zoomFactor * (d->viewportSize.width() / 2.0));
    d->y = cy - qRound(d->zoomFactor * (d->viewportSize.height() / 2.0));

    if (zoomDiff == 0)
        return;

    QRectF target = d->mapImage.rect();
    qreal width = target.width() / (1 << qAbs(zoomDiff));
    qreal height = target.width() / (1 << qAbs(zoomDiff));
    qreal x = target.x() + ((target.width() - width) / 2.0);
    qreal y = target.y() + ((target.height() - height) / 2.0);
    QRectF source = QRectF(x, y, width, height);

    QPixmap pm(d->mapImage.size());
    QPainter *painter = new QPainter(&pm);

    if (zoomDiff < 0) {
        painter->drawPixmap(source, d->mapImage, target);
    } else {
        painter->drawPixmap(target, d->mapImage, source);
    }

    delete painter;

    setImageChangesTriggerUpdates(true);
    setMapImage(pm);
}

void QGeoTiledMapViewport::setViewportSize(const QSizeF &size)
{
    Q_D(QGeoTiledMapViewport);

    QGeoMapViewport::setViewportSize(size);

    d->protectRegion = QRectF(0.0, 0.0, size.width(), size.height());

    QPixmap pm(size.toSize());
    QPainter p(&pm);
    if (!d->mapImage.isNull())
        p.drawPixmap(d->mapImage.rect(), d->mapImage, d->mapImage.rect());
    d->mapImage = pm;
    d->engine->updateMapImage(this);
}

void QGeoTiledMapViewport::pan(int dx, int dy)
{
    Q_D(QGeoTiledMapViewport);

    if (dx < 0 && (d->x < -1 * dx * d->zoomFactor))
        d->x += d->width;
    d->x += dx * d->zoomFactor;
    d->x = d->x % d->width;

    if (dy < 0 && (d->y < -1 * dy * d->zoomFactor))
        d->y += d->height;
    d->y += dy * d->zoomFactor;
    d->y = d->y % d->height;

    qreal width = d->mapImage.width() - qAbs(dx);
    qreal height = d->mapImage.height() - qAbs(dy);

    qreal sx;
    qreal sy;
    qreal tx;
    qreal ty;

    if (dx < 0) {
        sx = 0.0;
        tx = qAbs(dx);
    } else {
        sx = dx;
        tx = 0.0;
    }

    if (dy < 0) {
        sy = 0.0;
        ty = qAbs(dy);
    } else {
        sy = dy;
        ty = 0.0;
    }

    QRectF source = QRectF(sx, sy, width, height);
    QRectF target = QRectF(tx, ty, width, height);

    QPixmap pm(d->mapImage.size());
    QPainter p(&pm);
    if (!d->mapImage.isNull()) {
        p.drawPixmap(target, d->mapImage, source);
        d->protectRegion = target;
        setMapImage(pm);
    }
}

void QGeoTiledMapViewport::setTopLeftMapPixelX(qulonglong x)
{
    Q_D(QGeoTiledMapViewport);
    d->x = x;
}

qulonglong QGeoTiledMapViewport::topLeftMapPixelX() const
{
    Q_D(const QGeoTiledMapViewport);
    return d->x;
}

void QGeoTiledMapViewport::setTopLeftMapPixelY(qulonglong y)
{
    Q_D(QGeoTiledMapViewport);
    d->y = y;
}

qulonglong QGeoTiledMapViewport::topLeftMapPixelY() const
{
    Q_D(const QGeoTiledMapViewport);
    return d->y;
}

qulonglong QGeoTiledMapViewport::width() const
{
    Q_D(const QGeoTiledMapViewport);
    return d->width;
}

qulonglong QGeoTiledMapViewport::height() const
{
    Q_D(const QGeoTiledMapViewport);
    return d->height;
}

qulonglong QGeoTiledMapViewport::zoomFactor() const
{
    Q_D(const QGeoTiledMapViewport);
    return d->zoomFactor;
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
        x(other.x),
        y(other.y),
        width(other.width),
        height(other.height),
        zoomFactor(other.zoomFactor),
        protectRegion(other.protectRegion) {}

QGeoTiledMapViewportPrivate::~QGeoTiledMapViewportPrivate() {}

QGeoTiledMapViewportPrivate& QGeoTiledMapViewportPrivate::operator= (const QGeoTiledMapViewportPrivate & other)
{
    QGeoMapViewportPrivate::operator =(other);

    x = other.x;
    y = other.y;
    width = other.width;
    height = other.height;
    zoomFactor = other.zoomFactor;
    protectRegion = other.protectRegion;

    return *this;
}

QTM_END_NAMESPACE
