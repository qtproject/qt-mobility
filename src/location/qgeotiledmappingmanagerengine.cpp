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

#include "qgeotiledmappingmanagerengine.h"
#include "qgeotiledmappingmanagerengine_p.h"

#include "qgeotiledmapviewport.h"
#include "qgeotiledmaprequest.h"

#include "qgeotiledmappingmanagerthread.h"

#include <QPainter>
#include <QTimer>
#include <QNetworkAccessManager>
#include <QNetworkDiskCache>

#include <QDebug>

QTM_BEGIN_NAMESPACE

//Q_DECLARE_METATYPE(QList<QGeoTiledMapRequest>);

/*!
    Constructs a QGeoTiledMappingManagerEngine object.
*/
QGeoTiledMappingManagerEngine::QGeoTiledMappingManagerEngine(QObject *parent)
        : QGeoMappingManagerEngine(new QGeoTiledMappingManagerEnginePrivate(), parent)
{
    //qRegisterMetaType<QList<QGeoTiledMapRequest> >();

    setTileSize(QSize(128, 128));
    QTimer::singleShot(0, this, SLOT(init()));
}

void QGeoTiledMappingManagerEngine::init()
{
    Q_D(QGeoTiledMappingManagerEngine);

    d->thread = createTileManagerThread();

    /*
    connect(this,
            SIGNAL(tileRequestsPrepared(QGeoTiledMapViewport*,QList<QGeoTiledMapRequest>)),
            d->thread,
            SLOT(setRequests(QGeoTiledMapViewport*,QList<QGeoTiledMapRequest>)),
            Qt::QueuedConnection);

    connect(d->thread,
            SIGNAL(tileFinished(QGeoTiledMapReply*)),
            this,
            SLOT(tileFinished(QGeoTiledMapReply*)),
            Qt::QueuedConnection);

    connect(d->thread,
            SIGNAL(tileError(QGeoTiledMapReply*,QGeoTiledMapReply::Error,QString)),
            this,
            SLOT(tileError(QGeoTiledMapReply*,QGeoTiledMapReply::Error,QString)),
            Qt::QueuedConnection);
*/

    d->thread->start();
}

/*!
    Destroys this QGeoTiledMappingManagerEngine object.
*/
QGeoTiledMappingManagerEngine::~QGeoTiledMappingManagerEngine()
{
    Q_D(QGeoTiledMappingManagerEngine);
    d->thread->quit();
    d->thread->wait();
}

QGeoMapViewport* QGeoTiledMappingManagerEngine::createViewport(QGeoMapWidget *widget)
{
    return new QGeoTiledMapViewport(this, widget);
}

void QGeoTiledMappingManagerEngine::updateMapImage(QGeoMapViewport *viewport)
{
    Q_D(QGeoTiledMappingManagerEngine);

    if (!viewport)
        return;

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

    QList<QGeoTiledMapRequest> requests;

    QList<QPair<int, int> > tiles;

    QRectF protectedRegion = tiledViewport->protectedRegion().translated(QPointF(tiledViewport->topLeftMapPixelX(), tiledViewport->topLeftMapPixelY()));

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

        QRectF tileRect = QRectF((col + colOffset) * tileWidth, row * tileHeight, tileWidth, tileHeight);

//        if (!protectedRegion.isNull() && protectedRegion.contains(tileRect))
//            continue;
        if (i + protectedTiles < tiles.size()) {
            if (!protectedRegion.isNull() && protectedRegion.contains(tileRect)) {
                tiles.move(i, tiles.size() - 1);
                ++protectedTiles;
                --i;
                continue;
            }
        }

        requests.append(QGeoTiledMapRequest(tiledViewport, row, col, tileRect));
    }

    emit tileRequestsPrepared(tiledViewport, requests);

    tiledViewport->clearProtectedRegion();

//    QRectF screenRect = QRectF(
//            tiledViewport->topLeftMapPixelX() /  tiledViewport->zoomFactor(),
//            tiledViewport->topLeftMapPixelY() / tiledViewport->zoomFactor(),
//            viewport->viewportSize().width(),
//            viewport->viewportSize().height());

//    QRectF protectedRegion = tiledViewport->protectedRegion().translated(QPointF(screenRect.x(), screenRect.y()));


//    int protectedTiles = 0;
//    for (int i = 0; i < tiles.size(); ++i) {
//        int col = tiles.at(i).first;
//        int row = tiles.at(i).second;

//        int colOffset = 0;
//        if ((tileMinX > tileMaxX) && col <= tileMaxX)
//            colOffset = numTiles;

//        // intersected with relative translation
//        QRectF tileRect = QRectF((col + colOffset) * tileWidth, row * tileHeight, tileWidth, tileHeight);

//        if (i + protectedTiles < tiles.size()) {
//            if (!protectedRegion.isNull() && protectedRegion.contains(tileRect)) {
//                tiles.move(i, tiles.size() - 1);
//                ++protectedTiles;
//                --i;
//                continue;
//            }
//        }

//        QRectF overlap = tileRect.intersected(screenRect);

//        QRectF source = overlap.translated(-1.0 * tileRect.x(), -1.0 * tileRect.y());
//        QRectF dest = overlap.translated(-1.0 * screenRect.x(), -1.0 *screenRect.y());

//        // create replies
//        QGeoMapReply *mapReply = getTileImage(viewport->zoomLevel(), row, col, viewport->mapType(), "png");
//        // create wrappers and add to container
//        QGeoMapTileReply *reply = new QGeoMapTileReply(mapReply, source, dest);
//        container->addReply(reply);
//    }
}

void QGeoTiledMappingManagerEngine::tileFinished(QGeoTiledMapReply *reply)
{
    qWarning() << QString("QGeoTiledMappingManagerEngine::tileFinished") << (int) QThread::currentThreadId();

    if (!reply)
        return;

    if (reply->error() != QGeoTiledMapReply::NoError) {
        reply->deleteLater();
        return;
    }

    QGeoTiledMapViewport *viewport = reply->request().viewport();

    if ((viewport->zoomLevel() != reply->request().zoomLevel())
            || (viewport->mapType() != reply->request().mapType())) {
        reply->deleteLater();
        return;
    }

    QRectF screenRect = QRectF(
            viewport->topLeftMapPixelX() /  viewport->zoomFactor(),
            viewport->topLeftMapPixelY() / viewport->zoomFactor(),
            viewport->viewportSize().width(),
            viewport->viewportSize().height());

    QRectF tileRect = reply->request().zoomedWorldRect();

    QRectF overlap = tileRect.intersected(screenRect);

    if (overlap.isEmpty()) {
        reply->deleteLater();
        return;
    }

    QRectF source = overlap.translated(-1.0 * tileRect.x(), -1.0 * tileRect.y());
    QRectF dest = overlap.translated(-1.0 * screenRect.x(), -1.0 *screenRect.y());

    QPixmap pm = viewport->mapImage();
    QPainter *painter = new QPainter(&pm);
    painter->drawPixmap(dest, reply->mapImage(), source);
    viewport->setMapImage(pm);
    delete painter;

    reply->deleteLater();
}

void QGeoTiledMappingManagerEngine::tileError(QGeoTiledMapReply *reply, QGeoTiledMapReply::Error error, QString errorString)
{
    qWarning() << QString("error");
}

QPoint QGeoTiledMappingManagerEngine::screenPositionToTilePosition(const QGeoMapViewport *viewport, const QPointF &screenPosition) const
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
    Returns a list of the image formats supported by this QGeoTiledMappingManagerEngine
    instance.

    \sa QGeoTiledMappingManagerEngine::setSupportedImageFormats()
*/
QList<QString> QGeoTiledMappingManagerEngine::supportedImageFormats() const
{
    Q_D(const QGeoTiledMappingManagerEngine);
    return d->supportedImageFormats;
}

QSize QGeoTiledMappingManagerEngine::tileSize() const
{
    Q_D(const QGeoTiledMappingManagerEngine);
    return d->tileSize;
}

/*!
    Sets the list of image formats supported by this QGeoTiledMappingManagerEngine
    instance to \a imageFormats.

    Subclasses of QGeoCodingService should use this function to ensure that
    supportedImageFormats() provides accurate information.

    \sa QGeoTiledMappingManagerEngine::supportedImageFormats()
*/
void QGeoTiledMappingManagerEngine::setSupportedImageFormats(const QList<QString> &imageFormats)
{
    Q_D(QGeoTiledMappingManagerEngine);
    d->supportedImageFormats = imageFormats;
}

void QGeoTiledMappingManagerEngine::setTileSize(const QSize &tileSize)
{
    Q_D(QGeoTiledMappingManagerEngine);
    d->tileSize = tileSize;
}

/*!
    \fn void QGeoTiledMappingManagerEngine::replyFinished(QGeoMapReply *reply)

    Indicates that a request handled by this QGeoTiledMappingManagerEngine object has
    finished successfully.  The result of the request will be in \a reply.

    Note that \a reply will be the same object returned by this
    QGeoTiledMappingManagerEngine instance when the request was issued, and that the
    QGeoMapReply::finished() signal can be used instead of this signal if it
    is more convinient to do so.

    Do not delete the QGeoMapReply object in a slot connected to this signal
    - use deleteLater() if it is necessary to do so.

    \sa QGeoMapReply::finished()
*/

/*!
    \fn void QGeoTiledMappingManagerEngine::replyError(QGeoMapReply *reply,
                                            QGeoTiledMappingManagerEngine::ErrorCode errorCode,
                                            QString errorString);

    Indicates that a request handled by this QGeoTiledMappingManagerEngine object has
    failed.  The error is described by \a errorCode and \a errorString, and \a
    reply is the QGeoMapReply object which was managing the result of the
    corresponding service request.

    Note that \a reply will be the same object returned by this
    QGeoTiledMappingManagerEngine instance when the request was issued, and that the
    QGeoMapReply::error() signal can be used instead of this signal if it is
    more convinient to do so.

    Do not delete the QGeoMapReply object in a slot connected to this signal
    - use deleteLater() if it is necessary to do so.

    \sa QGeoMapReply::error()
*/

/*******************************************************************************
*******************************************************************************/

QGeoTiledMappingManagerEnginePrivate::QGeoTiledMappingManagerEnginePrivate()
    : QGeoMappingManagerEnginePrivate() {}

QGeoTiledMappingManagerEnginePrivate::QGeoTiledMappingManagerEnginePrivate(const QGeoTiledMappingManagerEnginePrivate &other)
        : QGeoMappingManagerEnginePrivate(other),
        supportedImageFormats(other.supportedImageFormats),
        tileSize(other.tileSize),
        thread(other.thread) {}

QGeoTiledMappingManagerEnginePrivate::~QGeoTiledMappingManagerEnginePrivate() {}

QGeoTiledMappingManagerEnginePrivate& QGeoTiledMappingManagerEnginePrivate::operator= (const QGeoTiledMappingManagerEnginePrivate & other)
{
    QGeoMappingManagerEnginePrivate::operator =(other);

    supportedImageFormats = other.supportedImageFormats;
    tileSize = other.tileSize;
    thread = other.thread;

    return *this;
}

///*******************************************************************************
//*******************************************************************************/


//void QGeoTiledMapRequestHandler::setRequests(const QList<QGeoMapTileRequest*> &requests)
//{
//    qWarning() << QString("requests and replies size") << m_queue.size() << m_replies.size();

//    // clear replies

//    QList<QGeoMapReply*> keys = m_replies.keys();

//    for (int i = 0; i < keys.size(); ++i) {
//        QGeoMapTileResponse *reply = m_replies.value(keys.at(i));
//        // this triggers finished which deletes both reply and reply->m_reply
//        // it also remove the entry from the m_replies
//        if (!reply->m_reply->isFinished())
//            reply->m_reply->abort();
//    }

//    m_replies.clear();

//    bool wasEmpty = (m_queue.size() == 0);

//    // clear queue

//    qDeleteAll(m_queue);
//    m_queue.clear();
//    m_queue.append(requests);

//    if (wasEmpty)
//        QTimer::singleShot(0, this, SLOT(sendNextRequest()));
//}


#include "moc_qgeotiledmappingmanagerengine.cpp"

QTM_END_NAMESPACE
