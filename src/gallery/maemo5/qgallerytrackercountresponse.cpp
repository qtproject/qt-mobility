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

#include "qgallerytrackercountresponse_p.h"

#include "qgallerytrackerschema_p.h"

#include <QtCore/qdatetime.h>
#include <QtDBus/qdbuspendingreply.h>

#include "qgalleryabstractresponse_p.h"

Q_DECLARE_METATYPE(QVector<QStringList>)

QTM_BEGIN_NAMESPACE

class QGalleryTrackerCountResponsePrivate : public QGalleryAbstractResponsePrivate
{
    Q_DECLARE_PUBLIC(QGalleryTrackerCountResponse)
public:
    QGalleryTrackerCountResponsePrivate(const QGalleryTrackerCountResponseArguments &arguments)
        : accumulative(arguments.accumulative)
        , cancelled(false)
        , refresh(false)
        , updateMask(arguments.updateMask)
        , count(0)
        , workingCount(0)
        , currentOffset(0)
        , queryWatcher(0)
        , queryInterface(arguments.queryInterface)
        , queryMethod(arguments.queryMethod)
        , queryArguments(arguments.queryArguments)
    {
    }

    void _q_queryFinished(QDBusPendingCallWatcher *watcher);
    void queryFinished(const QDBusPendingCall &call);

    void queryCount();

    enum
    {
        MaximumFetchSize = 512
    };

    const bool accumulative;
    bool cancelled;
    bool refresh;
    const int updateMask;
    int count;
    int workingCount;
    int currentOffset;
    QDBusPendingCallWatcher *queryWatcher;
    const QGalleryDBusInterfacePointer queryInterface;
    const QString queryMethod;
    const QVariantList queryArguments;
};

void QGalleryTrackerCountResponsePrivate::_q_queryFinished(QDBusPendingCallWatcher *watcher)
{
    if (queryWatcher == watcher) {
        queryWatcher->deleteLater();
        queryWatcher = 0;

        watcher->deleteLater();

        queryFinished(*watcher);
    }
}

void QGalleryTrackerCountResponsePrivate::queryFinished(const QDBusPendingCall &call)
{
    const int oldCount = count;

    if (call.isError()) {
        q_func()->finish(QGalleryAbstractRequest::ConnectionError);

        return;
    } else if (!accumulative) {
        QDBusPendingReply<int> reply(call);

        count = reply.value();

        if (refresh) {
            refresh = false;

            queryCount();
        }

    } else {
        QDBusPendingReply<QVector<QStringList> > reply(call);

        const QVector<QStringList> counts = reply.value();

        typedef QVector<QStringList>::const_iterator iterator;
        for (iterator it = counts.begin(), end = counts.end(); it != end; ++it)
            workingCount += it->value(1).toInt();

        if (refresh) {
            refresh = false;

            currentOffset = 0;
            workingCount = 0;

            queryCount();
        } else {
            currentOffset += counts.count();

            if (counts.count() == MaximumFetchSize) {
                if (count > workingCount)
                    count = workingCount;

                if (cancelled)
                    q_func()->QGalleryAbstractResponse::cancel();
                else
                    queryCount();
            } else {
                count = workingCount;
            }
        }
    }

    if (count > oldCount)
        emit q_func()->inserted(oldCount, count - oldCount);
    else if (count < oldCount)
        emit q_func()->removed(count, oldCount - count);

    if (!queryWatcher)
        q_func()->finish(QGalleryAbstractRequest::Succeeded);
}

void QGalleryTrackerCountResponsePrivate::queryCount()
{
    QVariantList arguments = queryArguments;

    if (accumulative)
        arguments << currentOffset << int(MaximumFetchSize);

    QDBusPendingCall call = queryInterface->asyncCallWithArgumentList(queryMethod, arguments);

    if (call.isFinished()) {
        queryFinished(call);
    } else {
        queryWatcher = new QDBusPendingCallWatcher(call, q_func());

        QObject::connect(queryWatcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                q_func(), SLOT(_q_queryFinished(QDBusPendingCallWatcher*)));
    }
}

QGalleryTrackerCountResponse::QGalleryTrackerCountResponse(
        const QGalleryTrackerCountResponseArguments &arguments,
        QObject *parent)
    : QGalleryBaseResponse(*new QGalleryTrackerCountResponsePrivate(arguments), parent)

{
    Q_D(QGalleryTrackerCountResponse);

    d->queryCount();
}

QGalleryTrackerCountResponse::~QGalleryTrackerCountResponse()
{
}

int QGalleryTrackerCountResponse::count() const
{
    return d_func()->count;
}

void QGalleryTrackerCountResponse::cancel()
{
    d_func()->cancelled = true;
    d_func()->refresh = false;

    if (!d_func()->queryWatcher)
        QGalleryAbstractResponse::cancel();
}

bool QGalleryTrackerCountResponse::waitForFinished(int msecs)
{
    Q_D(QGalleryTrackerCountResponse);

    QTime timer;
    timer.start();
    do {
        if (QDBusPendingCallWatcher *watcher = d->queryWatcher) {
            d->queryWatcher = 0;

            watcher->waitForFinished();

            d->queryFinished(*watcher);

            delete watcher;

            if (d->result != QGalleryAbstractRequest::NoResult)
                return true;
        } else {
            return true;
        }
    } while ((msecs -= timer.restart()) > 0);

    return false;
}

void QGalleryTrackerCountResponse::refresh(int serviceId)
{
    Q_D(QGalleryTrackerCountResponse);

    if (!d->cancelled && (d->updateMask & serviceId)) {
        d->refresh = true;

        if (!d->queryWatcher)
            d->queryCount();

    }
}

#include "moc_qgallerytrackercountresponse_p.cpp"

QTM_END_NAMESPACE
