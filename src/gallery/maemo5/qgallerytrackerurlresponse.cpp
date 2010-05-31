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
        , watcher(0)
        , fileInterface(fileInterface)
        , itemUrl(itemUrl)
    {
    }

    void _q_callFinished(QDBusPendingCallWatcher *call);

    const bool create;
    QDBusPendingCallWatcher *watcher;
    QGalleryDBusInterfacePointer fileInterface;
    const QUrl itemUrl;
    QVariant itemId;
    QString itemType;
};

void QGalleryTrackerUrlResponsePrivate::_q_callFinished(QDBusPendingCallWatcher *call)
{
    Q_ASSERT(call == watcher);

    call->deleteLater();
    watcher = 0;

    if (call->isError()) {
        qWarning("DBUS error %s", qPrintable(call->error().message()));

        q_func()->finish(QGalleryAbstractRequest::ConnectionError);
    } else {
        QGalleryTrackerSchema schema;
        schema.resolveTypeFromService(QDBusPendingReply<QString>(*call).value());

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

QGalleryTrackerUrlResponse::QGalleryTrackerUrlResponse(
        const QGalleryDBusInterfacePointer &fileInterface,
        const QUrl &itemUrl,
        bool create,
        QObject *parent)
    : QGalleryAbstractResponse(
            *new QGalleryTrackerUrlResponsePrivate(fileInterface, itemUrl, create), parent)
{
    Q_D(QGalleryTrackerUrlResponse);

    d->watcher = new QDBusPendingCallWatcher(
            d->fileInterface->asyncCall(QLatin1String("GetServiceType"), d->itemUrl.path()), this);

    if (d->watcher->isFinished()) {
        d->_q_callFinished(d->watcher);
    } else {
        connect(d->watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                this, SLOT(_q_callFinished(QDBusPendingCallWatcher*)));
    }
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
}

bool QGalleryTrackerUrlResponse::waitForFinished(int)
{
    Q_D(QGalleryTrackerUrlResponse);

    if (d->watcher) {
        d->watcher->waitForFinished();

        d->_q_callFinished(d->watcher);
    }
    return true;
}

#include "moc_qgallerytrackerurlresponse_p.cpp"

QTM_END_NAMESPACE
