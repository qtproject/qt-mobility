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

#include "qgeotiledmapdata.h"
#include "qgeotiledmaprequest.h"

#include <QPainter>
#include <QTimer>
#include <QNetworkAccessManager>
#include <QNetworkDiskCache>
#include <QMutableListIterator>
#include <QDebug>

uint qHash(const QRectF& key)
{
    uint result = qHash(qRound(key.x()));
    result += qHash(qRound(key.y()));
    result += qHash(qRound(key.width()));
    result += qHash(qRound(key.height()));
    return result;
}

QTM_BEGIN_NAMESPACE

/*!
    Constructs a QGeoTiledMappingManagerEngine object.
*/
QGeoTiledMappingManagerEngine::QGeoTiledMappingManagerEngine(const QMap<QString, QString> &parameters, QObject *parent)
        : QGeoMappingManagerEngine(new QGeoTiledMappingManagerEnginePrivate(parameters), parent)
{
    qRegisterMetaType<QGeoTiledMapReply::Error>("QGeoTiledMapReply::Error");

    setTileSize(QSize(128, 128));
    QTimer::singleShot(0, this, SLOT(init()));
}

void QGeoTiledMappingManagerEngine::init()
{
    initialize();
}

void QGeoTiledMappingManagerEngine::initialize() {}

/*!
    Destroys this QGeoTiledMappingManagerEngine object.
*/
QGeoTiledMappingManagerEngine::~QGeoTiledMappingManagerEngine()
{
    // deleted in superclass destructor
}

QGeoMapData* QGeoTiledMappingManagerEngine::createMapData(QGeoMapWidget *widget)
{
    return new QGeoTiledMapData(this, widget);
}

void QGeoTiledMappingManagerEngine::removeMapData(QGeoMapData* mapData)
{
    Q_D(QGeoTiledMappingManagerEngine);

    QGeoTiledMapData *tiledMapData = static_cast<QGeoTiledMapData*>(mapData);

    QGeoTiledMapRequestHandler *handler = d->handlers.value(tiledMapData, 0);
    if (handler) {
        d->handlers.remove(tiledMapData);
        handler->deleteLater();;
    }
}

void QGeoTiledMappingManagerEngine::updateMapImage(QGeoMapData *mapData)
{
    Q_D(QGeoTiledMappingManagerEngine);

    if (!mapData || mapData->zoomLevel() == -1.0)
        return;

    // deal with pole viewing later (needs more than two coords)
    // determine tile indices to fetch

    //TODO: need to have some form of type checking in here
    QGeoTiledMapData *tiledMapData = static_cast<QGeoTiledMapData*>(mapData);

    QGeoTileIterator it(tiledMapData->screenRect(), tileSize(), qRound(mapData->zoomLevel()));

    QList<QGeoTiledMapRequest> requests;

    QRectF protectedRegion = tiledMapData->protectedRegion();

    while (it.hasNext()) {
        it.next();
        int col = it.col();
        int row = it.row();
        QRectF tileRect = it.tileRect();

        // Protected region is the area that was on the screen before the
        // start of a resize or pan.
        // We shouldn't request tiles that are entirely contained in this
        // region.
        if (protectedRegion.isNull() || !protectedRegion.contains(tileRect.intersected(tiledMapData->screenRect()))) {
            //qWarning() << tiledMapData->zoomLevel() << row << col;
            requests.append(QGeoTiledMapRequest(tiledMapData, row, col, tileRect));
        }
    }

    if (requests.count() > 0) {
        QGeoTiledMapRequestHandler *handler = d->handlers.value(tiledMapData, 0);
        if (!handler) {
            handler = new QGeoTiledMapRequestHandler(this, tiledMapData);

            connect(handler,
                    SIGNAL(finished(QGeoTiledMapReply*)),
                    this,
                    SLOT(tileFinished(QGeoTiledMapReply*)));

            connect(handler,
                    SIGNAL(error(QGeoTiledMapReply*, QGeoTiledMapReply::Error, QString)),
                    this,
                    SLOT(tileError(QGeoTiledMapReply*, QGeoTiledMapReply::Error, QString)));

            connect(handler,
                    SIGNAL(triggerUpdate(QGeoTiledMapData*)),
                    this,
                    SLOT(triggerUpdate(QGeoTiledMapData*)));

            d->handlers.insert(tiledMapData, handler);
        }

        handler->setRequests(requests);
    }

    tiledMapData->clearProtectedRegion();
}

void QGeoTiledMappingManagerEngine::triggerUpdate(QGeoTiledMapData *data)
{
    data->widget()->update();
}

void QGeoTiledMappingManagerEngine::tileFinished(QGeoTiledMapReply *reply)
{
    if (!reply)
        return;

    if (reply->error() != QGeoTiledMapReply::NoError) {
        QTimer::singleShot(0, reply, SLOT(deleteLater()));
        return;
    }

    QGeoTiledMapData *mapData = reply->request().mapData();

    if ((mapData->zoomLevel() != reply->request().zoomLevel())
            || (mapData->mapType() != reply->request().mapType())) {
        QTimer::singleShot(0, reply, SLOT(deleteLater()));
        return;
    }

    QPixmap tile;

    if (!tile.loadFromData(reply->mapImageData(), reply->mapImageFormat().toAscii())) {
        QTimer::singleShot(0, reply, SLOT(deleteLater()));
        return;
        //setError(QGeoTiledMapReply::ParseError, "The response from the service was not in a recognisable format.");
    }

    if (tile.isNull() || tile.size().isEmpty()) {
        QTimer::singleShot(0, reply, SLOT(deleteLater()));
        return;
        //setError(QGeoTiledMapReply::ParseError, "The map image is empty.");
    }

    int tileHeight = tileSize().height();
    int tileWidth = tileSize().width();
    int numCols = 1 << static_cast<int>(mapData->zoomLevel());
    qreal totalPixelWidth = static_cast<qreal>(numCols) * tileWidth;
    QRectF screenRect = mapData->screenRect();
    QRectF tileRect(tileWidth * reply->request().column(),
                    tileHeight * reply->request().row(),
                    tileWidth, tileHeight);
    QPixmap pm = mapData->mapImage();
    QPainter *painter = new QPainter(&pm);

    while (tileRect.left() <= screenRect.right())
    {
        QRectF overlap = tileRect.intersected(screenRect);

        if (!overlap.isEmpty())
        {
            QRectF source = overlap.translated(-1.0 * tileRect.x(), -1.0 * tileRect.y());
            QRectF dest = overlap.translated(-1.0 * screenRect.x(), -1.0 * screenRect.y());
            painter->drawPixmap(dest, tile, source);
        }

        tileRect.translate(totalPixelWidth, 0);
    }

    mapData->setMapImage(pm);
    delete painter;

    QTimer::singleShot(0, reply, SLOT(deleteLater()));
}

void QGeoTiledMappingManagerEngine::tileError(QGeoTiledMapReply *reply, QGeoTiledMapReply::Error error, QString errorString)
{
    qWarning() << errorString;
}

QPoint QGeoTiledMappingManagerEngine::screenPositionToTilePosition(const QGeoMapData *mapData, const QPointF &screenPosition) const
{
    // TODO checking mechanism for viewport type?
    const QGeoTiledMapData *tiledMapData = static_cast<const QGeoTiledMapData*>(mapData);
    return tiledMapData->screenPositionToTileIndices(screenPosition);
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

QGeoTiledMappingManagerEnginePrivate::QGeoTiledMappingManagerEnginePrivate(const QMap<QString, QString> &parameters)
        : QGeoMappingManagerEnginePrivate(parameters) {}

QGeoTiledMappingManagerEnginePrivate::QGeoTiledMappingManagerEnginePrivate(const QGeoTiledMappingManagerEnginePrivate &other)
        : QGeoMappingManagerEnginePrivate(other),
        supportedImageFormats(other.supportedImageFormats),
        tileSize(other.tileSize),
        handlers(other.handlers) {}

QGeoTiledMappingManagerEnginePrivate::~QGeoTiledMappingManagerEnginePrivate()
{
    QList<QGeoTiledMapData*> keys = handlers.keys();
    for (int i = 0; i < keys.size(); ++i)
        delete handlers[keys.at(i)];
}

QGeoTiledMappingManagerEnginePrivate& QGeoTiledMappingManagerEnginePrivate::operator= (const QGeoTiledMappingManagerEnginePrivate & other)
{
    QGeoMappingManagerEnginePrivate::operator =(other);

    supportedImageFormats = other.supportedImageFormats;
    tileSize = other.tileSize;
    handlers = other.handlers;

    return *this;
}

/*******************************************************************************
*******************************************************************************/

QGeoTiledMapRequestHandler::QGeoTiledMapRequestHandler(QGeoTiledMappingManagerEngine *engine, QGeoTiledMapData *mapData)
        : QObject(engine),
        engine(engine),
        mapData(mapData),
        lastZoomLevel(-1.0),
        cachedReplies(0) {}

QGeoTiledMapRequestHandler::~QGeoTiledMapRequestHandler()
{
    QList<QGeoTiledMapReply*> replyList = replies.toList();
    for (int i = 0; i < replyList.size(); ++i) {
        replyList.at(i)->abort();
        replyList.at(i)->deleteLater();
    }
}

void QGeoTiledMapRequestHandler::setRequests(const QList<QGeoTiledMapRequest> &requests)
{
    bool wasEmpty = (requestQueue.size() == 0);

    if (lastZoomLevel != -1.0) {
        // TODO make the viewport access thread-safe
        if ((lastZoomLevel != mapData->zoomLevel()) || (lastMapType != mapData->mapType())) {
            cachedReplies = 0;
            requestQueue = requests;
            requestRects.clear();
            for (int i = 0; i < requests.size(); ++i)
                requestRects.insert(requests.at(i).tileRect());
        } else {
            QMutableListIterator<QGeoTiledMapRequest> requestIter(requestQueue);
            while (requestIter.hasNext()) {
                QGeoTiledMapRequest req = requestIter.next();

                if (!mapData->screenRect().intersects(req.tileRect())) {
                    requestRects.remove(req.tileRect());
                    requestIter.remove();
                }
            }
            for (int i = 0; i < requests.size(); ++i)
                if (!requestRects.contains(requests.at(i).tileRect())
                    && !replyRects.contains(requests.at(i).tileRect())) {
                        requestQueue.append(requests.at(i));
                        requestRects.insert(requests.at(i).tileRect());
                }
        }
    }

    lastScreenRect = mapData->screenRect();
    lastZoomLevel = mapData->zoomLevel();
    lastMapType = mapData->mapType();

    if (wasEmpty)
        QTimer::singleShot(50, this, SLOT(processRequests()));
}

void QGeoTiledMapRequestHandler::processRequests()
{
    QMutableSetIterator<QGeoTiledMapReply*> replyIter(replies);

    // Kill off screen replies
    while (replyIter.hasNext()) {
        QGeoTiledMapReply *reply = replyIter.next();
        if (!lastScreenRect.intersects(reply->request().tileRect())
            || (lastZoomLevel != reply->request().zoomLevel())
            || (lastMapType != reply->request().mapType())) {
                reply->abort();
                replyRects.remove(reply->request().tileRect());
                replyIter.remove();
        }
    }

    QMutableListIterator<QGeoTiledMapRequest> requestIter(requestQueue);
    while (requestIter.hasNext()) {
        QGeoTiledMapRequest req = requestIter.next();

        requestRects.remove(req.tileRect());
        requestIter.remove();

        // Do not use the requests which have pending replies or are off screen
        if (replyRects.contains(req.tileRect()) || !lastScreenRect.intersects(req.tileRect())) {
            continue;
        }

        QGeoTiledMapReply *reply = engine->getTileImage(req);

        if (!reply) {
            continue;
        }

        if (reply->error() != QGeoTiledMapReply::NoError) {
            emit error(reply, reply->error(), reply->errorString());
            continue;
        }

        connect(reply,
                SIGNAL(finished()),
                this,
                SLOT(tileFinished()));

        connect(reply,
                SIGNAL(error(QGeoTiledMapReply::Error, QString)),
                this,
                SLOT(tileError(QGeoTiledMapReply::Error, QString)));

        replies.insert(reply);
        replyRects.insert(reply->request().tileRect());

        if (reply->isCached())
            ++cachedReplies;

        break;
    }
}

void QGeoTiledMapRequestHandler::tileFinished()
{
    QGeoTiledMapReply *reply = qobject_cast<QGeoTiledMapReply*>(sender());

    if (!reply)
        return;

    replyRects.remove(reply->request().tileRect());
    replies.remove(reply);

    if (reply->isCached())
        --cachedReplies;

    if (receivers(SIGNAL(finished(QGeoTiledMapReply*))) == 0) {
        reply->deleteLater();
        return;
    }

    emit finished(reply);

    if (cachedReplies == 0)
        emit triggerUpdate(reply->request().mapData());

    if (requestQueue.size() != 0)
        QTimer::singleShot(50, this, SLOT(processRequests()));
}

void QGeoTiledMapRequestHandler::tileError(QGeoTiledMapReply::Error error, const QString &errorString)
{
    QGeoTiledMapReply *reply = qobject_cast<QGeoTiledMapReply*>(sender());

    if (!reply)
        return;

    if ((receivers(SIGNAL(finished(QGeoTiledMapReply*))) == 0)
            && (receivers(SIGNAL(error(QGeoTiledMapReply*, QGeoTiledMapReply::Error, QString))) == 0)) {
        reply->deleteLater();
        return;
    }

    emit this->error(reply, error, errorString);
}

///*******************************************************************************
//*******************************************************************************/

QGeoTileIterator::QGeoTileIterator(const QRectF &screenRect, const QSize &tileSize, int zoomLevel)
    : aHasNext(true), aRow(-1), aCol(-1), aScreenRect(screenRect),
    aTileSize(tileSize), aZoomLevel(zoomLevel), aTileRect(QPointF(0,0), tileSize)
{
    qulonglong x = static_cast<qulonglong>(screenRect.topLeft().x() / tileSize.width());
    qulonglong y = static_cast<qulonglong>(screenRect.topLeft().y() / tileSize.height());

    currTopLeft.setX(x * tileSize.width());
    currTopLeft.setY(y * tileSize.height());
}

bool QGeoTileIterator::hasNext()
{
    return aHasNext;
}

QRectF QGeoTileIterator::tileRect() const
{
    return aTileRect;
}

void QGeoTileIterator::next()
{
    int numCols = 1 << aZoomLevel;
    aCol = static_cast<int>(currTopLeft.x() / aTileSize.width()) % numCols;
    aRow = static_cast<int>(currTopLeft.y() / aTileSize.height()) % numCols;
    aTileRect.moveTopLeft(currTopLeft);

    currTopLeft.rx() += aTileSize.width();

    if (currTopLeft.x() > aScreenRect.right()) { //next row
        qulonglong x = static_cast<qulonglong>(aScreenRect.topLeft().x() / aTileSize.width());
        currTopLeft.setX(x * aTileSize.width());
        currTopLeft.ry() += aTileSize.height();
    }

    if (currTopLeft.y() > aScreenRect.bottom()) //done
        aHasNext = false;
}

int QGeoTileIterator::row() const
{
    return aRow;
}

int QGeoTileIterator::col() const
{
    return aCol;
}

int QGeoTileIterator::zoomLevel() const
{
    return aZoomLevel;
}

///*******************************************************************************
//*******************************************************************************/

#include "moc_qgeotiledmappingmanagerengine.cpp"
#include "moc_qgeotiledmappingmanagerengine_p.cpp"

QTM_END_NAMESPACE
