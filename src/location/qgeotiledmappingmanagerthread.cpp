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

#include "qgeotiledmappingmanagerthread.h"
#include "qgeotiledmappingmanagerthread_p.h"

#include "qgeotiledmaprequest.h"
#include "qgeotiledmappingmanagerengine.h"

#include "qgeotiledmapdata.h"

#include <QSet>
#include <QMutableSetIterator>

#include <QTimer>

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

QGeoTiledMappingManagerThread::QGeoTiledMappingManagerThread(QGeoTiledMappingManagerEngine* engine, QObject *parent)
        : QThread(),
        d_ptr(new QGeoTiledMappingManagerThreadPrivate())
{
    qRegisterMetaType<QGeoTiledMapRequest>("QGeoTiledMapRequest");
    qRegisterMetaType<QList<QGeoTiledMapRequest> >("QList<QGeoTiledMapRequest>");
    qRegisterMetaType<QGeoTiledMapReply::Error>("QGeoTiledMapReply::Error");

    moveToThread(this);

    d_ptr->engine = engine;
}

QGeoTiledMappingManagerThread::~QGeoTiledMappingManagerThread()
{
    delete d_ptr;
}

/*!
\fn QGeoTiledMapReply* QGeoTiledMappingManagerThread::getTileImage(const QGeoTiledMapRequest &request) const
*/

void QGeoTiledMappingManagerThread::removeMapData(QGeoTiledMapData* mapData)
{
    QGeoTiledMapRequestHandler *handler = d_ptr->handlers.value(mapData, 0);
    if (handler) {
        d_ptr->handlers.remove(mapData);
        delete handler;
    }
}

void QGeoTiledMappingManagerThread::initialize() {}

void QGeoTiledMappingManagerThread::run()
{
    initialize();

    qRegisterMetaType<QGeoTiledMapRequest>();
    qRegisterMetaType<QList<QGeoTiledMapRequest> >();
    qRegisterMetaType<QGeoTiledMapReply::Error>();
    connect(d_ptr->engine,
            SIGNAL(tileRequestsPrepared(QGeoTiledMapData*, QList<QGeoTiledMapRequest>)),
            this,
            SLOT(setRequests(QGeoTiledMapData*, QList<QGeoTiledMapRequest>)),
            Qt::QueuedConnection);

    connect(this,
            SIGNAL(tileFinished(QGeoTiledMapReply*)),
            d_ptr->engine,
            SLOT(tileFinished(QGeoTiledMapReply*)));

    connect(this,
            SIGNAL(tileError(QGeoTiledMapReply*, QGeoTiledMapReply::Error, QString)),
            d_ptr->engine,
            SLOT(tileError(QGeoTiledMapReply*, QGeoTiledMapReply::Error, QString)));

    exec();
}

void QGeoTiledMappingManagerThread::setRequests(QGeoTiledMapData* mapData, const QList<QGeoTiledMapRequest> &requests)
{
    QGeoTiledMapRequestHandler *handler = d_ptr->handlers.value(mapData, 0);

    if (!handler) {
        handler = new QGeoTiledMapRequestHandler(this, mapData);

        // TODO remove replies from cache on error?
        // Would need to do this in handler

        connect(handler,
                SIGNAL(finished(QGeoTiledMapReply*)),
                this,
                SIGNAL(tileFinished(QGeoTiledMapReply*)));

        connect(handler,
                SIGNAL(error(QGeoTiledMapReply*, QGeoTiledMapReply::Error, QString)),
                this,
                SIGNAL(tileError(QGeoTiledMapReply*, QGeoTiledMapReply::Error, QString)));

        d_ptr->handlers.insert(mapData, handler);
    }

    handler->setRequests(requests);
}

/*******************************************************************************
*******************************************************************************/


QGeoTiledMapRequestHandler::QGeoTiledMapRequestHandler(QGeoTiledMappingManagerThread *thread, QGeoTiledMapData *mapData)
        : QObject(thread),
        thread(thread),
        mapData(mapData),
        lastZoomLevel(-1.0) {}

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
            requestQueue = requests;
            requestRects.clear();
            for (int i = 0; i < requests.size(); ++i)
                requestRects.insert(requests.at(i).tileRect());
        } else {
            for (int i = 0; i < requests.size(); ++i)
                if (!requestRects.contains(requests.at(i).tileRect())) {
                    requestQueue.append(requests.at(i));
                    requestRects.insert(requests.at(i).tileRect());
                }
        }
    }

    lastScreenRect = mapData->screenRect();
    lastZoomLevel = mapData->zoomLevel();
    lastMapType = mapData->mapType();

    if (wasEmpty)
        QTimer::singleShot(0, this, SLOT(processRequests()));

    /*
    QSet<QRectF> replyRects;

    if (lastZoomLevel != -1.0) {
        // TODO make the viewport access thread-safe
        if ((lastZoomLevel != mapData->zoomLevel()) || (lastMapType != mapData->mapType())) {

            // clear all replies

            QList<QGeoTiledMapReply*> replyList = replies.toList();

            for (int i = 0; i < replyList.size(); ++i) {
                replyList.at(i)->abort();
                //TODO: make sure that abort will always cause an error on the reply
                //replyList.at(i)->deleteLater();
            }
            replies.clear();

        } else {

            QRectF screenRect = mapData->screenRect();

            // abort all replies that are off screen

            QMutableSetIterator<QGeoTiledMapReply*> replyIter(replies);

            while (replyIter.hasNext()) {
                QGeoTiledMapReply *reply = replyIter.next();
                if (!screenRect.intersects(reply->request().tileRect())) {
                    reply->abort();
                    replyIter.remove();
                } else {
                    if (reply->error() == QGeoTiledMapReply::NoError)
                        replyRects.insert(reply->request().tileRect());
                }
            }
        }
    }

    lastZoomLevel = mapData->zoomLevel();
    lastMapType = mapData->mapType();

    for (int i = 0; i < requests.size(); ++i) {
        // Do not use the requests which have pending replies
        if (replyRects.contains(requests.at(i).tileRect()))
            continue;

        QGeoTiledMapReply *reply = thread->getTileImage(requests.at(i));
        if (!reply)
            continue;

        connect(reply,
                SIGNAL(finished()),
                this,
                SLOT(tileFinished()));

        connect(reply,
                SIGNAL(error(QGeoTiledMapReply::Error, QString)),
                this,
                SLOT(tileError(QGeoTiledMapReply::Error, QString)));

        if (reply->error() == QGeoTiledMapReply::NoError) {
            replies.insert(reply);
        } else {
            emit error(reply, reply->error(), reply->errorString());
        }
    }
*/
}

void QGeoTiledMapRequestHandler::processRequests()
{
    QMutableSetIterator<QGeoTiledMapReply*> replyIter(replies);

    // Kill off screen replies
    while (replyIter.hasNext()) {
        QGeoTiledMapReply *reply = replyIter.next();
        if (!lastScreenRect.intersects(reply->request().tileRect())) {
            reply->abort();
            replyRects.remove(reply->request().tileRect());
            replyIter.remove();
        }
    }

    for (int i = 0; i < requestQueue.size(); ++i) {

        // Do not use the requests which have pending replies
        if (replyRects.contains(requestQueue.at(i).tileRect()))
            continue;

        QGeoTiledMapReply *reply = thread->getTileImage(requestQueue.at(i));
        if (!reply)
            continue;

        connect(reply,
                SIGNAL(finished()),
                this,
                SLOT(tileFinished()));

        connect(reply,
                SIGNAL(error(QGeoTiledMapReply::Error, QString)),
                this,
                SLOT(tileError(QGeoTiledMapReply::Error, QString)));

        if (reply->error() == QGeoTiledMapReply::NoError) {
            replies.insert(reply);
            replyRects.insert(reply->request().tileRect());
        } else {
            emit error(reply, reply->error(), reply->errorString());
        }
    }

    requestRects.clear();
    requestQueue.clear();
}

void QGeoTiledMapRequestHandler::tileFinished()
{
    QGeoTiledMapReply *reply = qobject_cast<QGeoTiledMapReply*>(sender());

    if (!reply)
        return;

    replyRects.remove(reply->request().tileRect());
    replies.remove(reply);

    if (receivers(SIGNAL(finished(QGeoTiledMapReply*))) == 0) {
        reply->deleteLater();
        return;
    }

    emit finished(reply);
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

/*******************************************************************************
*******************************************************************************/

QGeoTiledMappingManagerThreadPrivate::QGeoTiledMappingManagerThreadPrivate()
        : engine(0) {}

QGeoTiledMappingManagerThreadPrivate::QGeoTiledMappingManagerThreadPrivate(const QGeoTiledMappingManagerThreadPrivate &other)
        : engine(other.engine),
        handlers(other.handlers) {}

QGeoTiledMappingManagerThreadPrivate::~QGeoTiledMappingManagerThreadPrivate() {}

QGeoTiledMappingManagerThreadPrivate& QGeoTiledMappingManagerThreadPrivate::operator= (const QGeoTiledMappingManagerThreadPrivate & other)
{
    engine = other.engine;
    handlers = other.handlers;

    return *this;
}

#include "moc_qgeotiledmappingmanagerthread.cpp"
#include "moc_qgeotiledmappingmanagerthread_p.cpp"

QTM_END_NAMESPACE

