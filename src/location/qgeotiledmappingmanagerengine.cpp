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

/*!
    Constructs a QGeoTiledMappingManagerEngine object.
*/
QGeoTiledMappingManagerEngine::QGeoTiledMappingManagerEngine(const QMap<QString, QString> &parameters, QObject *parent)
        : QGeoMappingManagerEngine(new QGeoTiledMappingManagerEnginePrivate(parameters), parent)
{
    setTileSize(QSize(128, 128));
    QTimer::singleShot(0, this, SLOT(init()));
}

void QGeoTiledMappingManagerEngine::init()
{
    Q_D(QGeoTiledMappingManagerEngine);

    d->thread = createTileManagerThread();
    d->thread->start();
}

/*!
    Destroys this QGeoTiledMappingManagerEngine object.
*/
QGeoTiledMappingManagerEngine::~QGeoTiledMappingManagerEngine()
{
    Q_D(QGeoTiledMappingManagerEngine);
    if(d->thread) {
        d->thread->quit();
        d->thread->wait();
    }
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

    //TODO: replace this QList-based implementation with a more mem-lightweight solution like a la TileIterator
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

    //TODO: need to have some form of type checking in here
    QGeoTiledMapViewport *tiledViewport = static_cast<QGeoTiledMapViewport*>(viewport);

    QList<QGeoTiledMapRequest> requests;

    QList<QPair<int, int> > tiles;

    QRectF protectedRegion = tiledViewport->protectedRegion();

    // TODO order in direction of travel if panning
    // TODO request excess tiles around border
    for (int x = 0; x < cols.size(); ++x) {
        for (int y = 0; y < rows.size(); ++y) {
            tiles.append(qMakePair(cols.at(x), rows.at(y)));
        }
    }

    for (int i = 0; i < tiles.size(); ++i) {
        int col = tiles.at(i).first;
        int row = tiles.at(i).second;

        int colOffset = 0;
        if ((tileMinX > tileMaxX) && col <= tileMaxX)
            colOffset = numTiles;

        QRectF tileRect = QRectF((col + colOffset) * tileWidth, row * tileHeight, tileWidth, tileHeight);

        // Protected region is the area that was on the screen before the
        // start of a resize or pan.
        // We shouldn't request tiles that are entirely contained in this
        // region.
        if (protectedRegion.isNull() || !protectedRegion.contains(tileRect))
            requests.append(QGeoTiledMapRequest(tiledViewport, row, col, tileRect));
    }

    emit tileRequestsPrepared(tiledViewport, requests);

    tiledViewport->clearProtectedRegion();
}

void QGeoTiledMappingManagerEngine::tileFinished(QGeoTiledMapReply *reply)
{
    if (!reply)
        return;

    if (reply->error() != QGeoTiledMapReply::NoError) {
        QTimer::singleShot(0, reply, SLOT(deleteLater()));
        return;
    }

    QGeoTiledMapViewport *viewport = reply->request().viewport();

    if ((viewport->zoomLevel() != reply->request().zoomLevel())
            || (viewport->mapType() != reply->request().mapType())) {
        QTimer::singleShot(0, reply, SLOT(deleteLater()));
        return;
    }

    QRectF screenRect = viewport->screenRect();
    QRectF tileRect = reply->request().tileRect();
    QRectF overlap = tileRect.intersected(screenRect);

    if (overlap.isEmpty()) {
        QTimer::singleShot(0, reply, SLOT(deleteLater()));
        return;
    }

    QRectF source = overlap.translated(-1.0 * tileRect.x(), -1.0 * tileRect.y());
    QRectF dest = overlap.translated(-1.0 * screenRect.x(), -1.0 * screenRect.y());

    QPixmap pm = viewport->mapImage();
    QPainter *painter = new QPainter(&pm);
    painter->drawPixmap(dest, reply->mapImage(), source);
    viewport->setMapImage(pm);
    delete painter;

    QTimer::singleShot(0, reply, SLOT(deleteLater()));
}

void QGeoTiledMappingManagerEngine::tileError(QGeoTiledMapReply *reply, QGeoTiledMapReply::Error error, QString errorString)
{
    qWarning() << errorString;
}

QPoint QGeoTiledMappingManagerEngine::screenPositionToTilePosition(const QGeoMapViewport *viewport, const QPointF &screenPosition) const
{
    // TODO checking mechanism for viewport type?
    const QGeoTiledMapViewport *tiledViewport = static_cast<const QGeoTiledMapViewport*>(viewport);
    return tiledViewport->screenPositionToTileIndices(screenPosition);
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
        : QGeoMappingManagerEnginePrivate(parameters), thread(NULL) {}

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

#include "moc_qgeotiledmappingmanagerengine.cpp"

QTM_END_NAMESPACE
