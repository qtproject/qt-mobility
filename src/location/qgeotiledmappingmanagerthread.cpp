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

#include "qgeotiledmapviewport.h"

#include <QTimer>

#include <QDebug>

QTM_BEGIN_NAMESPACE

Q_DECLARE_METATYPE(QList<QGeoTiledMapRequest>);
Q_DECLARE_METATYPE(QGeoTiledMapReply::Error);

QGeoTiledMappingManagerThread::QGeoTiledMappingManagerThread(QGeoTiledMappingManagerEngine* engine, QObject *parent)
        : QThread(),
        d_ptr(new QGeoTiledMappingManagerThreadPrivate())
{
    moveToThread(this);

    qRegisterMetaType<QList<QGeoTiledMapRequest> >();
    qRegisterMetaType<QGeoTiledMapReply::Error>();

    d_ptr->engine = engine;
}

QGeoTiledMappingManagerThread::~QGeoTiledMappingManagerThread()
{
    delete d_ptr;
}

//virtual QGeoTiledMapReply* getTileImage(const QGeoTiledMapRequest &request) const = 0;

void QGeoTiledMappingManagerThread::removeViewport(QGeoTiledMapViewport* viewport)
{
    QGeoTiledMapRequestHandler *handler = d_ptr->handlers.value(viewport, 0);
    if (handler) {
        d_ptr->handlers.remove(viewport);
        delete handler;
    }
}

void QGeoTiledMappingManagerThread::initialize() {}

void QGeoTiledMappingManagerThread::run()
{
    initialize();

    connect(d_ptr->engine,
            SIGNAL(tileRequestsPrepared(QGeoTiledMapViewport*, QList<QGeoTiledMapRequest>)),
            this,
            SLOT(setRequests(QGeoTiledMapViewport*, QList<QGeoTiledMapRequest>)),
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

void QGeoTiledMappingManagerThread::setRequests(QGeoTiledMapViewport* viewport, const QList<QGeoTiledMapRequest> &requests)
{
    QGeoTiledMapRequestHandler *handler = d_ptr->handlers.value(viewport, 0);

    if (!handler) {
        handler = new QGeoTiledMapRequestHandler(this, viewport);

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

        d_ptr->handlers.insert(viewport, handler);
    }

    handler->setRequests(requests);
}

/*******************************************************************************
*******************************************************************************/


QGeoTiledMapRequestHandler::QGeoTiledMapRequestHandler(QGeoTiledMappingManagerThread *thread, QGeoTiledMapViewport *viewport)
        : QObject(thread),
        thread(thread),
        viewport(viewport),
        lastZoomLevel(-1.0) {}

QGeoTiledMapRequestHandler::~QGeoTiledMapRequestHandler()
{
    // TODO abort and delete everything in replies
}

void QGeoTiledMapRequestHandler::setRequests(const QList<QGeoTiledMapRequest> &requests)
{
    bool wasEmpty = (queue.size() == 0);

    queue.clear();

//    if (lastZoomLevel != -1.0) {
//        // TODO make the viewport access thread-safe
//        if ((lastZoomLevel != viewport->zoomLevel()) || (lastMapType != viewport->mapType())) {

    // clear all replies

//            QList<QGeoTiledMapReply*> replyList = replies.toList();

//            for (int i = 0; i < replyList.size(); ++i) {
//                replyList.at(i)->abort();
//                replyList.at(i)->deleteLater();
//            }
//            replies.clear();

    queue.append(requests);

//        } else {

//            QRectF screenRect = QRectF(
//                    viewport->topLeftMapPixelX() /  viewport->zoomFactor(),
//                    viewport->topLeftMapPixelY() / viewport->zoomFactor(),
//                    viewport->viewportSize().width(),
//                    viewport->viewportSize().height());

//            // abort all replies that are offscreen

//            QList<QGeoTiledMapReply*> replyList = replies.toList();

//            // TODO replace with a set - too busy to stop and write a qHash overload for rects
//            QList<QRectF> replyRects;

//            for (int i = 0; i < replyList.size(); ++i) {
//                replyRects.append(replyList.at(i)->request().zoomedWorldRect());
//                if (!replyList.at(i)->isFinished()) {
//                    if (screenRect.intersected(replyList.at(i)->request().zoomedWorldRect()).isEmpty()) {
//                        replyList.at(i)->abort();
//                        replies.remove(replyList.at(i));
//                    }
//                }
//            }

//            // don't add the requests which have pending replies
//            for (int i = 0; i < requests.size(); ++i) {
//                if (!replyRects.contains(requests.at(i).zoomedWorldRect()))
//                    queue.append(requests.at(i));
//            }
//        }
//    }

    lastZoomLevel = viewport->zoomLevel();
    lastMapType = viewport->mapType();

    if (wasEmpty && queue.size() != 0)
        QTimer::singleShot(0, this, SLOT(sendNextRequest()));
}

void QGeoTiledMapRequestHandler::sendNextRequest()
{
    if (queue.size() == 0)
        return;

    QGeoTiledMapReply *reply = thread->getTileImage(queue.takeFirst());

    if (!reply) {
        QTimer::singleShot(0, this, SLOT(sendNextRequest()));
        return;
    }

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

    QTimer::singleShot(0, this, SLOT(sendNextRequest()));
}

void QGeoTiledMapRequestHandler::tileFinished()
{
    QGeoTiledMapReply *reply = qobject_cast<QGeoTiledMapReply*>(sender());

    if (!reply)
        return;

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

