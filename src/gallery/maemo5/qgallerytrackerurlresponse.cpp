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


#include "qgallerytrackerurlresponse_p.h"

#include "qgallerytrackermetadataedit_p.h"
#include "qgallerytrackerschema_p.h"
#include "qgalleryurlrequest.h"

#include <QtCore/qcoreapplication.h>
#include <QtCore/qcoreevent.h>
#include <QtCore/qdatetime.h>
#include <QtDBus/qdbuspendingreply.h>

#include "qgalleryabstractresponse_p.h"

Q_DECLARE_METATYPE(QVector<QStringList>)

QTM_BEGIN_NAMESPACE

class QGalleryTrackerUrlResponsePrivate : public QGalleryAbstractResponsePrivate
{
    Q_DECLARE_PUBLIC(QGalleryTrackerUrlResponse)
public:
    QGalleryTrackerUrlResponsePrivate(
            const QGalleryDBusInterfacePointer &fileInterface,
            const QUrl &itemUrl,
            bool create)
        : create(create)
        , cancelled(false)
        , timeout(10000)
        , getServiceTypeWatcher(0)
        , existWatcher(0)
        , fileInterface(fileInterface)
        , itemUrl(itemUrl)
    {
    }

    void getServiceType();
    void _q_getServiceTypeFinished(QDBusPendingCallWatcher *watcher);
    void getServiceTypeFinished(const QDBusPendingCall &call);

    void exist();
    void _q_existFinished(QDBusPendingCallWatcher *call);
    void existFinished(const QDBusPendingCall &call);

    const bool create;
    bool cancelled;
    int timeout;
    QDBusPendingCallWatcher *getServiceTypeWatcher;
    QDBusPendingCallWatcher *existWatcher;
    QGalleryDBusInterfacePointer fileInterface;
    QBasicTimer createTimer;
    QTime createTime;
    const QUrl itemUrl;
    QVariant itemId;
    QString itemType;
};

void QGalleryTrackerUrlResponsePrivate::getServiceType()
{
    QDBusPendingCall getServiceTypeCall = fileInterface->asyncCall(
            QLatin1String("GetServiceType"), itemUrl.path());

    if (getServiceTypeCall.isFinished()) {
        getServiceTypeFinished(getServiceTypeCall);
    } else {
        getServiceTypeWatcher = new QDBusPendingCallWatcher(getServiceTypeCall, q_func());

        QObject::connect(
                getServiceTypeWatcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                q_func(), SLOT(_q_getServiceTypeFinished(QDBusPendingCallWatcher*)));
    }
}

void QGalleryTrackerUrlResponsePrivate::_q_getServiceTypeFinished(QDBusPendingCallWatcher *watcher)
{
    if (watcher == getServiceTypeWatcher) {
        getServiceTypeWatcher->deleteLater();
        getServiceTypeWatcher = 0;

        getServiceTypeFinished(*watcher);
    } else {
        qWarning("%s Finished signal emitted from within waitForFinished", Q_FUNC_INFO);
    }
}

void QGalleryTrackerUrlResponsePrivate::getServiceTypeFinished(const QDBusPendingCall &call)
{
    if (call.isError()) {
        QDBusError error = call.error();

        if (error.type() == QDBusError::Other) {
            // Url may not be in database.
            if (!cancelled)
                exist();
            else
                q_func()->finish(QGalleryAbstractRequest::Cancelled);
        } else {
            qWarning("QGalleryUrlRequest DBUS error: %s", qPrintable(error.message()));

            q_func()->finish(QGalleryAbstractRequest::ConnectionError);
        }
    } else {
        QGalleryTrackerSchema schema = QGalleryTrackerSchema::fromService(
                QDBusPendingReply<QString>(call).value());

        if (schema.isItemType()) {
            itemId = schema.itemIdFromUri(itemUrl.path());
            itemType = schema.itemType();

            emit q_func()->inserted(0, 1);

            q_func()->finish(QGalleryAbstractRequest::Succeeded);
        } else {
            q_func()->finish(QGalleryAbstractRequest::ItemTypeError);
        }
    }
}

void QGalleryTrackerUrlResponsePrivate::exist()
{
    QDBusPendingCall existCall = fileInterface->asyncCall(
            QLatin1String("Exist"), itemUrl.path(), false);

    if (existCall.isFinished()) {
        existFinished(existCall);
    } else {
        existWatcher = new QDBusPendingCallWatcher(existCall, q_func());

        QObject::connect(
                existWatcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                q_func(), SLOT(_q_existFinished(QDBusPendingCallWatcher*)));
    }
}

void QGalleryTrackerUrlResponsePrivate::_q_existFinished(QDBusPendingCallWatcher *watcher)
{
    if (watcher == existWatcher) {
        existWatcher->deleteLater();
        existWatcher = 0;

        existFinished(*watcher);
    } else {
        qWarning("%s Finished signal emitted from within waitForFinished", Q_FUNC_INFO);
    }
}

void QGalleryTrackerUrlResponsePrivate::existFinished(const QDBusPendingCall &call)
{
    if (call.isError()) {
        QDBusError error = call.error();

        qWarning("QGalleryUrlRequest DBUS error: %s", qPrintable(error.message()));

        q_func()->finish(QGalleryAbstractRequest::ConnectionError);
    } else {
        QDBusPendingReply<bool> reply(call);

        if (reply.value()) {
            if (!cancelled)
                getServiceType();
            else
                q_func()->finish(QGalleryAbstractRequest::Cancelled);
        } else if (create && timeout > 0) {
            createTimer.start(timeout, q_func());
            createTime.start();
        } else {
            q_func()->finish(QGalleryAbstractRequest::InvalidUrlError);
        }
    }
}

QGalleryTrackerUrlResponse::QGalleryTrackerUrlResponse(
        const QGalleryDBusInterfacePointer &fileInterface,
        const QUrl &itemUrl,
        bool create,
        QObject *parent)
    : QGalleryAbstractResponse(
            *new QGalleryTrackerUrlResponsePrivate(fileInterface, itemUrl, create), parent)
{
    d_func()->getServiceType();
}

QGalleryTrackerUrlResponse::~QGalleryTrackerUrlResponse()
{
}

int QGalleryTrackerUrlResponse::minimumPagedItems() const
{
    return 1;
}

QStringList QGalleryTrackerUrlResponse::propertyNames() const
{
    return QStringList();
}

int QGalleryTrackerUrlResponse::propertyKey(const QString &) const
{
    return -1;
}

QGalleryProperty::Attributes QGalleryTrackerUrlResponse::propertyAttributes(int) const
{
    return QGalleryProperty::CanRead;
}

int QGalleryTrackerUrlResponse::count() const
{
    return !d_func()->itemId.isNull() ? 1 : 0;
}

QVariant QGalleryTrackerUrlResponse::id(int) const
{
    return d_func()->itemId;
}

QUrl QGalleryTrackerUrlResponse::url(int) const
{
    return d_func()->itemUrl;
}

QString QGalleryTrackerUrlResponse::type(int) const
{
    return d_func()->itemType;
}

QList<QGalleryResource> QGalleryTrackerUrlResponse::resources(int) const
{
    return QList<QGalleryResource>();
}

QGalleryItemList::ItemStatus QGalleryTrackerUrlResponse::status(int) const
{
    return ItemStatus();
}

QVariant QGalleryTrackerUrlResponse::metaData(int, int) const
{
    return QVariant();
}

void QGalleryTrackerUrlResponse::setMetaData(int, int, const QVariant &)
{
}

void QGalleryTrackerUrlResponse::cancel()
{
    Q_D(QGalleryTrackerUrlResponse);

    if (d->createTimer.isActive()) {
        d->createTimer.stop();

        finish(QGalleryAbstractRequest::Cancelled);
    } else {
        d->cancelled = true;
    }
}

bool QGalleryTrackerUrlResponse::waitForFinished(int msecs)
{
    Q_D(QGalleryTrackerUrlResponse);

    QTime time;
    time.start();

    do {
        if (QDBusPendingCallWatcher *watcher = d->getServiceTypeWatcher) {
            d->getServiceTypeWatcher = 0;

            watcher->waitForFinished();

            d->getServiceTypeFinished(*watcher);

            delete watcher;

            if (d->result != QGalleryAbstractRequest::NoResult)
                return true;
        } else if (QDBusPendingCallWatcher *watcher = d->existWatcher) {
            d->existWatcher = 0;

            watcher->waitForFinished();

            d->existFinished(*watcher);

            delete watcher;

            if (d->result != QGalleryAbstractRequest::NoResult)
                return true;
        } else if (d->createTimer.isActive()) {
            // This will poll tracker aggressively.  A sleep may be advisable.
            d->createTimer.stop();
            d->timeout -= d->createTime.elapsed();

            d->exist();
        } else {
            return true;
        }
    } while ((msecs -= time.restart()) > 0);

    return false;
}

void QGalleryTrackerUrlResponse::indexingFinished()
{
    Q_D(QGalleryTrackerUrlResponse);

    if (d->createTimer.isActive()) {
        d->createTimer.stop();
        d->timeout = 0;

        d->exist();
    }
}

void QGalleryTrackerUrlResponse::timerEvent(QTimerEvent *event)
{
    Q_D(QGalleryTrackerUrlResponse);

    if (event->timerId() == d->createTimer.timerId()) {
        d->createTimer.stop();
        d->timeout = 0;

        d->exist();
    }
}

#include "moc_qgallerytrackerurlresponse_p.cpp"

QTM_END_NAMESPACE
