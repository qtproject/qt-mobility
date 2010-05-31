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

#include <QtDBus/qdbuspendingreply.h>

#include "qgalleryabstractresponse_p.h"

Q_DECLARE_METATYPE(QVector<QStringList>)

QTM_BEGIN_NAMESPACE

class QGalleryTrackerCountResponsePrivate : public QGalleryAbstractResponsePrivate
{
    Q_DECLARE_PUBLIC(QGalleryTrackerCountResponse)
public:
    QGalleryTrackerCountResponsePrivate()
        : count(0)
        , workingCount(0)
        , currentOffset(0)
        , call(0)
    {
    }

    void _q_callFinished(QDBusPendingCallWatcher *watcher);

    void queryCount();

    enum
    {
        MaximumFetchSize = 512
    };

    int count;
    int workingCount;
    int currentOffset;
    QDBusPendingCallWatcher *call;
    QGalleryDBusInterfacePointer metaDataInterface;
    QString query;
    QString service;
    QString countField;
    QStringList identityFields;
};


void QGalleryTrackerCountResponsePrivate::_q_callFinished(QDBusPendingCallWatcher *watcher)
{
    if (watcher != call) {
        // This shouldn't ever happen.
        Q_ASSERT(false);

        return;
    }
    call = 0;

    watcher->deleteLater();

    int oldCount = count;

    if (watcher->isError()) {
        q_func()->finish(QGalleryAbstractRequest::ConnectionError);

        return;
    } else if (identityFields.isEmpty()) {
        QDBusPendingReply<int> reply(*watcher);

        count = reply.value();
    } else {
        QDBusPendingReply<QVector<QStringList> > reply(*watcher);

        const QVector<QStringList> counts = reply.value();

        for (QVector<QStringList>::const_iterator it = counts.begin(), end = counts.end();
                it != end;
                ++it) {
            workingCount += it->value(1).toInt();
        }

        currentOffset += counts.count();

        if (counts.count() == MaximumFetchSize) {
            if (count > workingCount)
                count = workingCount;

            queryCount();
        } else {
            count = workingCount;
        }
    }

    if (count > oldCount)
        emit q_func()->inserted(oldCount, count - oldCount);
    else if (count < oldCount)
        emit q_func()->removed(count, oldCount - count);

    if (!call)
        q_func()->finish(QGalleryAbstractRequest::Succeeded);
}

void QGalleryTrackerCountResponsePrivate::queryCount()
{
    if (countField.isEmpty()) {
        call = new QDBusPendingCallWatcher(metaDataInterface->asyncCall(
                QLatin1String("GetCount"), service, QLatin1String("*"), query), q_func());
    } else if (identityFields.isEmpty()) {
        call = new QDBusPendingCallWatcher(metaDataInterface->asyncCall(
                QLatin1String("GetCount"), service, countField, query), q_func());
    } else {
        call = new QDBusPendingCallWatcher(metaDataInterface->asyncCall(
                QLatin1String("GetUniqueValuesWithCount"),
                service,
                identityFields,
                query,
                countField,
                false,
                currentOffset,
                MaximumFetchSize), q_func());
    }

    if (call->isFinished()) {
        _q_callFinished(call);
    } else {
        QObject::connect(call, SIGNAL(finished(QDBusPendingCallWatcher*)),
                q_func(), SLOT(_q_callFinished(QDBusPendingCallWatcher*)));
    }
}


QGalleryTrackerCountResponse::QGalleryTrackerCountResponse(
        const QGalleryDBusInterfacePointer &metaDataInterface,
        const QGalleryTrackerSchema &schema,
        const QString &query,
        QObject *parent)
    : QGalleryAbstractResponse(*new QGalleryTrackerCountResponsePrivate, parent)

{
    Q_D(QGalleryTrackerCountResponse);

    d->metaDataInterface = metaDataInterface;
    d->query = query;
    d->service = schema.service();
    d->identityFields  = schema.identityFields();
    if (!d->identityFields.isEmpty())
        d->countField = d->identityFields.takeLast();

    d->queryCount();
}

QGalleryTrackerCountResponse::~QGalleryTrackerCountResponse()
{
}


QStringList QGalleryTrackerCountResponse::propertyNames() const
{
    return QStringList();
}

int QGalleryTrackerCountResponse::propertyKey(const QString &) const
{
    return -1;
}

QGalleryProperty::Attributes QGalleryTrackerCountResponse::propertyAttributes(int) const
{
    return QGalleryProperty::Attributes();
}

int QGalleryTrackerCountResponse::count() const
{
    return d_func()->count;
}

QVariant QGalleryTrackerCountResponse::id(int) const
{
    return QVariant();
}

QUrl QGalleryTrackerCountResponse::url(int) const
{
    return QUrl();
}

QString QGalleryTrackerCountResponse::type(int) const
{
    return QString();
}

QList<QGalleryResource> QGalleryTrackerCountResponse::resources(int) const
{
    return QList<QGalleryResource>();
}

QGalleryItemList::ItemStatus QGalleryTrackerCountResponse::status(int) const
{
    return ItemStatus();
}

QVariant QGalleryTrackerCountResponse::metaData(int, int) const
{
    return QVariant();
}

void QGalleryTrackerCountResponse::setMetaData(int, int, const QVariant &)
{
}

void QGalleryTrackerCountResponse::cancel()
{

}

bool QGalleryTrackerCountResponse::waitForFinished(int)
{
    Q_D(QGalleryTrackerCountResponse);

    if (d->call) {
        d->call->waitForFinished();

        d->_q_callFinished(d->call);

        return true;
    } else {
        return false;
    }
}

#include "moc_qgallerytrackercountresponse_p.cpp"

QTM_END_NAMESPACE
