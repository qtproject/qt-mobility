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

#include "qgeotiledmappingmanager.h"
#include "qgeotiledmappingmanager_p.h"

#include "qgeotiledmapviewport.h"

#include <QPainter>

#include <QDebug>

QTM_BEGIN_NAMESPACE

/*!
    Constructs a QGeoTiledMappingManager object.
*/
QGeoTiledMappingManager::QGeoTiledMappingManager()
        : QGeoMappingManager(new QGeoTiledMappingManagerPrivate)
{
    setTileSize(QSize(128, 128));
}

/*!
    Destroys this QGeoTiledMappingManager object.
*/
QGeoTiledMappingManager::~QGeoTiledMappingManager()
{
}

QGeoMapViewport* QGeoTiledMappingManager::createViewport(QGeoMapWidget *widget)
{
    return new QGeoTiledMapViewport(this, widget);
}

void QGeoTiledMappingManager::updateMapImage(QGeoMapViewport *viewport)
{
    Q_D(QGeoTiledMappingManager);

    if (!viewport)
        return;

    QGeoMapTileReplyContainer *container = d->replies.value(viewport, 0);

    // TODO delete and remove in removeviewport
    // TODO delete and remove all in descructor

    if (!container) {
        container = new QGeoMapTileReplyContainer(viewport);
        d->replies.insert(viewport, container);
    } else {
        container->cancelAllReplies();
    }

    // deal with pole viewing later (needs more than two coords)

    // determine tile indices to fetch

    QPoint tileIndicesTopLeft = screenPositionToTilePosition(viewport, QPointF(0.0, 0.0));
    QPoint tileIndicesBottomRight = screenPositionToTilePosition(viewport, QPointF(viewport->viewportSize().width(), viewport->viewportSize().height()));

    int tileMinX = tileIndicesTopLeft.x();
    int tileMaxX = tileIndicesBottomRight.x();
    int tileMinY = tileIndicesTopLeft.y();
    int tileMaxY = tileIndicesBottomRight.y();

    int numTiles = 1 << qRound(viewport->zoomLevel());

    QList<int> cols;

    if (tileMinX <= tileMaxX) {
        for (int i = tileMinX; i <= tileMaxX; ++i)
            cols << i;
    } else {
        for (int i = tileMinX; i < numTiles; ++i)
            cols << i;
        for (int i = 0; i <= tileMaxX; ++i)
            cols << i;
    }

    QList<int> rows;

    if (tileMinY <= tileMaxY) {
        for (int i = tileMinY; i <= tileMaxY; ++i)
            rows << i;
    } else {
        for (int i = tileMinY; i < numTiles; ++i)
            rows << i;
        for (int i = 0; i <= tileMaxY; ++i)
            rows << i;
    }

    int tileHeight = tileSize().height();
    int tileWidth = tileSize().width();

    QGeoTiledMapViewport *tiledViewport = static_cast<QGeoTiledMapViewport*>(viewport);

    QRectF screenRect = QRectF(
            tiledViewport->topLeftMapPixelX() /  tiledViewport->zoomFactor(),
            tiledViewport->topLeftMapPixelY() / tiledViewport->zoomFactor(),
            viewport->viewportSize().width(),
            viewport->viewportSize().height());

    QRectF protectedRegion = tiledViewport->protectedRegion().translated(QPointF(screenRect.x(), screenRect.y()));

    QList<QPair<int, int> > tiles;

    // TODO from centre out
    // TODO request excess tiles around border
    for (int x = 0; x < cols.size(); ++x) {
        for (int y = 0; y < rows.size(); ++y) {
            tiles.append(qMakePair(cols.at(x), rows.at(y)));
        }
    }

    int protectedTiles = 0;
    for (int i = 0; i < tiles.size(); ++i) {
        int col = tiles.at(i).first;
        int row = tiles.at(i).second;

        int colOffset = 0;
        if ((tileMinX > tileMaxX) && col <= tileMaxX)
            colOffset = numTiles;

        // intersected with relative translation
        QRectF tileRect = QRectF((col + colOffset) * tileWidth, row * tileHeight, tileWidth, tileHeight);

        if (i + protectedTiles < tiles.size()) {
            if (!protectedRegion.isNull() && protectedRegion.contains(tileRect)) {
                tiles.move(i, tiles.size() - 1);
                ++protectedTiles;
                --i;
                continue;
            }
        }

        QRectF overlap = tileRect.intersected(screenRect);

        QRectF source = overlap.translated(-1.0 * tileRect.x(), -1.0 * tileRect.y());
        QRectF dest = overlap.translated(-1.0 * screenRect.x(), -1.0 *screenRect.y());

        // create replies
        QGeoMapReply *mapReply = getTileImage(viewport->zoomLevel(), row, col, viewport->mapType(), "png");
        // create wrappers and add to container
        QGeoMapTileReply *reply = new QGeoMapTileReply(mapReply, source, dest);
        container->addReply(reply);
    }

    tiledViewport->clearProtectedRegion();
}

QPoint QGeoTiledMappingManager::screenPositionToTilePosition(const QGeoMapViewport *viewport, const QPointF &screenPosition) const
{
    // TODO checking mechanism for viewport type
    const QGeoTiledMapViewport *tiledViewport = static_cast<const QGeoTiledMapViewport*>(viewport);

    qulonglong x = tiledViewport->topLeftMapPixelX();
    x += qRound64(screenPosition.x() * tiledViewport->zoomFactor());
    x = x % tiledViewport->width();
    qreal rx = qreal(x) / tiledViewport->zoomFactor();

    int tileX = rx / tileSize().width();

//    qreal y = tiledViewport->topLeftMapPixelY() / qreal(tiledViewport->zoomFactor());
//    y += screenPosition.y();
//    int tileY = y / tileSize().height();

    qulonglong y = tiledViewport->topLeftMapPixelY();
    y += qRound64(screenPosition.y() * tiledViewport->zoomFactor());
    y = y % tiledViewport->height();
    qreal ry = qreal(y) / tiledViewport->zoomFactor();

    int tileY = ry / tileSize().height();

    return QPoint(tileX, tileY);
}

/*!
    Returns a list of the image formats supported by this QGeoTiledMappingManager
    instance.

    \sa QGeoTiledMappingManager::setSupportedImageFormats()
*/
QList<QString> QGeoTiledMappingManager::supportedImageFormats() const
{
    Q_D(const QGeoTiledMappingManager);
    return d->supportedImageFormats;
}

QSize QGeoTiledMappingManager::tileSize() const
{
    Q_D(const QGeoTiledMappingManager);
    return d->tileSize;
}

/*!
    Sets the list of image formats supported by this QGeoTiledMappingManager
    instance to \a imageFormats.

    Subclasses of QGeoCodingService should use this function to ensure that
    supportedImageFormats() provides accurate information.

    \sa QGeoTiledMappingManager::supportedImageFormats()
*/
void QGeoTiledMappingManager::setSupportedImageFormats(const QList<QString> &imageFormats)
{
    Q_D(QGeoTiledMappingManager);
    d->supportedImageFormats = imageFormats;
}

void QGeoTiledMappingManager::setTileSize(const QSize &tileSize)
{
    Q_D(QGeoTiledMappingManager);
    d->tileSize = tileSize;
}

/*!
    \fn void QGeoTiledMappingManager::replyFinished(QGeoMapReply *reply)

    Indicates that a request handled by this QGeoTiledMappingManager object has
    finished successfully.  The result of the request will be in \a reply.

    Note that \a reply will be the same object returned by this
    QGeoTiledMappingManager instance when the request was issued, and that the
    QGeoMapReply::finished() signal can be used instead of this signal if it
    is more convinient to do so.

    Do not delete the QGeoMapReply object in a slot connected to this signal
    - use deleteLater() if it is necessary to do so.

    \sa QGeoMapReply::finished()
*/

/*!
    \fn void QGeoTiledMappingManager::replyError(QGeoMapReply *reply,
                                            QGeoTiledMappingManager::ErrorCode errorCode,
                                            QString errorString);

    Indicates that a request handled by this QGeoTiledMappingManager object has
    failed.  The error is described by \a errorCode and \a errorString, and \a
    reply is the QGeoMapReply object which was managing the result of the
    corresponding service request.

    Note that \a reply will be the same object returned by this
    QGeoTiledMappingManager instance when the request was issued, and that the
    QGeoMapReply::error() signal can be used instead of this signal if it is
    more convinient to do so.

    Do not delete the QGeoMapReply object in a slot connected to this signal
    - use deleteLater() if it is necessary to do so.

    \sa QGeoMapReply::error()
*/

/*******************************************************************************
*******************************************************************************/

QGeoTiledMappingManagerPrivate::QGeoTiledMappingManagerPrivate()
    : QGeoMappingManagerPrivate() {}

QGeoTiledMappingManagerPrivate::QGeoTiledMappingManagerPrivate(const QGeoTiledMappingManagerPrivate &other)
        : QGeoMappingManagerPrivate(other),
        supportedImageFormats(other.supportedImageFormats),
        tileSize(other.tileSize) {}

QGeoTiledMappingManagerPrivate::~QGeoTiledMappingManagerPrivate() {}

QGeoTiledMappingManagerPrivate& QGeoTiledMappingManagerPrivate::operator= (const QGeoTiledMappingManagerPrivate & other)
{
    QGeoMappingManagerPrivate::operator =(other);

    supportedImageFormats = other.supportedImageFormats;
    tileSize = other.tileSize;

    return *this;
}

/*******************************************************************************
*******************************************************************************/

QGeoMapTileReply::QGeoMapTileReply(QGeoMapReply *reply, const QRectF &source, const QRectF &dest)
    : m_reply(reply),
    m_source(source),
    m_dest(dest) {}

QGeoMapTileReply::~QGeoMapTileReply() {}

/*******************************************************************************
*******************************************************************************/

QGeoMapTileReplyContainer::QGeoMapTileReplyContainer(QGeoMapViewport *viewport)
    : m_viewport(viewport) {}

QGeoMapTileReplyContainer::~QGeoMapTileReplyContainer()
{
    cancelAllReplies();
}

void QGeoMapTileReplyContainer::cancelAllReplies()
{
    QList<QGeoMapReply*> keys = m_replies.keys();

    for (int i = 0; i < keys.size(); ++i) {
        QGeoMapTileReply *reply = m_replies.value(keys.at(i));
        // this triggers finished which deletes both reply and reply->m_reply
        // it also remove the entry from the m_replies
        reply->m_reply->abort();
    }

    m_replies.clear();
}

void QGeoMapTileReplyContainer::addReply(QGeoMapTileReply *reply)
{
    m_replies.insert(reply->m_reply, reply);

    connect(reply->m_reply,
            SIGNAL(finished()),
            this,
            SLOT(replyFinished()));
    connect(reply->m_reply,
            SIGNAL(error(QGeoMapReply::Error,QString)),
            this,
            SLOT(replyError(QGeoMapReply::Error,QString)));
}

void QGeoMapTileReplyContainer::replyFinished()
{
    QGeoMapReply *mapReply = qobject_cast<QGeoMapReply*>(sender());

    if (!mapReply)
        return;

    QGeoMapTileReply *reply = m_replies.value(mapReply, 0);

    if (reply) {
        // this needs streamlining badly- inefficient as it stands
        QPixmap pm = m_viewport->mapImage();
        QPainter *painter = new QPainter(&pm);
        painter->drawPixmap(reply->m_dest, reply->m_reply->mapImage(), reply->m_source);
        // trigger updates - make less ugly
        m_viewport->setMapImage(pm);
        delete painter;
    }

    mapReply->deleteLater();
    m_replies.remove(mapReply);

    if (reply)
        delete reply;
}

void QGeoMapTileReplyContainer::replyError(QGeoMapReply::Error, const QString &errorString)
{
    QGeoMapReply *mapReply = qobject_cast<QGeoMapReply*>(sender());

    if (!mapReply)
        return;

    QGeoMapTileReply *reply = m_replies.value(mapReply, 0);

    //qWarning() << QString("Error getting reply: ") << errorString;

    m_replies.remove(mapReply);
    mapReply->deleteLater();

    if (reply)
        delete reply;
}

#include "moc_qgeotiledmappingmanager.cpp"
#include "moc_qgeotiledmappingmanager_p.cpp"

QTM_END_NAMESPACE
