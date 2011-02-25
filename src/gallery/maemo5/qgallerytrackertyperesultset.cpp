/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in 
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgallerytrackertyperesultset_p.h"

#include "qgallerytrackerschema_p.h"

#include <QtCore/qdatetime.h>
#include <QtDBus/qdbuspendingreply.h>

#include "qdocumentgallery.h"

#include "qgalleryresultset_p.h"

Q_DECLARE_METATYPE(QVector<QStringList>)

QTM_BEGIN_NAMESPACE

class QGalleryTrackerTypeResultSetPrivate : public QGalleryResultSetPrivate
{
    Q_DECLARE_PUBLIC(QGalleryTrackerTypeResultSet)
public:
    QGalleryTrackerTypeResultSetPrivate(const QGalleryTrackerTypeResultSetArguments &arguments)
        : accumulative(arguments.accumulative)
        , canceled(false)
        , refresh(false)
        , updateMask(arguments.updateMask)
        , currentIndex(-1)
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

    const bool accumulative;
    bool canceled;
    bool refresh;
    const int updateMask;
    int currentIndex;
    int count;
    int workingCount;
    int currentOffset;
    QDBusPendingCallWatcher *queryWatcher;
    const QGalleryDBusInterfacePointer queryInterface;
    const QString queryMethod;
    const QVariantList queryArguments;
};

void QGalleryTrackerTypeResultSetPrivate::_q_queryFinished(QDBusPendingCallWatcher *watcher)
{
    if (queryWatcher == watcher) {
        queryWatcher->deleteLater();
        queryWatcher = 0;

        watcher->deleteLater();

        queryFinished(*watcher);
    }
}

void QGalleryTrackerTypeResultSetPrivate::queryFinished(const QDBusPendingCall &call)
{
    const int oldCount = count;

    if (call.isError()) {
        q_func()->finish(QDocumentGallery::ConnectionError);

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

            if (counts.count() != 0) {
                if (count > workingCount)
                    count = workingCount;

                if (canceled)
                    q_func()->QGalleryAbstractResponse::cancel();
                else
                    queryCount();
            } else {
                count = workingCount;
            }
        }
    }

    if (count != oldCount)
        emit q_func()->metaDataChanged(0, 1, QList<int>() << 0);

    if (!queryWatcher)
        q_func()->finish();
}

void QGalleryTrackerTypeResultSetPrivate::queryCount()
{
    QVariantList arguments = queryArguments;

    if (accumulative)
        arguments << currentOffset << int(0);

    QDBusPendingCall call = queryInterface->asyncCallWithArgumentList(queryMethod, arguments);

    if (call.isFinished()) {
        queryFinished(call);
    } else {
        queryWatcher = new QDBusPendingCallWatcher(call, q_func());

        QObject::connect(queryWatcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                q_func(), SLOT(_q_queryFinished(QDBusPendingCallWatcher*)));
    }
}

QGalleryTrackerTypeResultSet::QGalleryTrackerTypeResultSet(
        const QGalleryTrackerTypeResultSetArguments &arguments,
        QObject *parent)
    : QGalleryResultSet(*new QGalleryTrackerTypeResultSetPrivate(arguments), parent)

{
    Q_D(QGalleryTrackerTypeResultSet);

    d->queryCount();
}

QGalleryTrackerTypeResultSet::~QGalleryTrackerTypeResultSet()
{
}

int QGalleryTrackerTypeResultSet::propertyKey(const QString &property) const
{
    return property == QLatin1String("count") ? 0 : -1;
}

QGalleryProperty::Attributes QGalleryTrackerTypeResultSet::propertyAttributes(int key) const
{
    return key == 0 ? QGalleryProperty::CanRead : QGalleryProperty::Attributes();
}

QVariant::Type QGalleryTrackerTypeResultSet::propertyType(int key) const
{
    return key == 0 ? QVariant::Int : QVariant::Invalid;
}

int QGalleryTrackerTypeResultSet::itemCount() const
{
    return 1;
}

QVariant QGalleryTrackerTypeResultSet::itemId() const
{
    return QVariant();
}

QUrl QGalleryTrackerTypeResultSet::itemUrl() const
{
    return QUrl();
}

QString QGalleryTrackerTypeResultSet::itemType() const
{
    return QString();
}

QVariant QGalleryTrackerTypeResultSet::metaData(int key) const
{
    return d_func()->currentIndex == 0 && key == 0
            ? d_func()->count
            : 0;
}

bool QGalleryTrackerTypeResultSet::setMetaData(int, const QVariant &)
{
    return false;
}

int QGalleryTrackerTypeResultSet::currentIndex() const
{
    return d_func()->currentIndex;
}

bool QGalleryTrackerTypeResultSet::fetch(int index)
{
    if (index != d_func()->currentIndex) {
        bool itemChanged = index == 0 || d_func()->currentIndex == 0;

        d_func()->currentIndex = index;

        emit currentIndexChanged(index);

        if (itemChanged)
            emit currentItemChanged();
    }
    return d_func()->currentIndex == 0;
}

void QGalleryTrackerTypeResultSet::cancel()
{
    d_func()->canceled = true;
    d_func()->refresh = false;

    if (!d_func()->queryWatcher)
        QGalleryAbstractResponse::cancel();
}

bool QGalleryTrackerTypeResultSet::waitForFinished(int msecs)
{
    Q_D(QGalleryTrackerTypeResultSet);

    QTime timer;
    timer.start();
    do {
        if (QDBusPendingCallWatcher *watcher = d->queryWatcher) {
            d->queryWatcher = 0;

            watcher->waitForFinished();

            d->queryFinished(*watcher);

            delete watcher;

            if (d->state != QGalleryAbstractRequest::Active)
                return true;
        } else {
            return true;
        }
    } while ((msecs -= timer.restart()) > 0);

    return false;
}

void QGalleryTrackerTypeResultSet::refresh(int serviceId)
{
    Q_D(QGalleryTrackerTypeResultSet);

    if (!d->canceled && (d->updateMask & serviceId)) {
        d->refresh = true;

        if (!d->queryWatcher)
            d->queryCount();

    }
}

#include "moc_qgallerytrackertyperesultset_p.cpp"

QTM_END_NAMESPACE
