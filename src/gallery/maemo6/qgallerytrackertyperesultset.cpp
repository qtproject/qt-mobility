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

#include "qgallerytrackertyperesultset_p.h"

#include "qgallerytrackerschema_p.h"

#include <QtCore/qdatetime.h>
#include <QtCore/QDebug>
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
        , cancelled(false)
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
        , service( arguments.service )
    {
    }

    void _q_queryFinished(QDBusPendingCallWatcher *watcher);
    void queryFinished(const QDBusPendingCall &call);

    void queryCount();

    const bool accumulative;
    bool cancelled;
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
    const QString service;
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

class QGalleryTrackerType : public QString
{
public:
    QGalleryTrackerType(const QString& type ) : QString(type) {}
};

static bool operator ==(const QStringList &statistic, const QGalleryTrackerType &type)
{
    return statistic.first() == type;
}

void QGalleryTrackerTypeResultSetPrivate::queryFinished(const QDBusPendingCall &call)
{
    const int oldCount = count;

    if (call.isError()) {
        q_func()->finish(QDocumentGallery::ConnectionError);

        return;
    } else if (!accumulative) {
        QDBusPendingReply<QVector<QStringList> > reply(call);

        if (queryMethod == QLatin1String("SparqlQuery")) {
            QVector<QStringList> v = reply.value();
            count = v[0].first().toInt();
        } else {
            /*
             * Process reply to org.freedesktop.Tracker.Statistics.Get -method. Value is a list of list of two strings:
             * type1 count1
             * type2 count2
             * ...
             * where typeX corresponds to the service name ( i.e. ontology class name, e.g. "nfo:FileDataObject" ).
             * Search through the list and find the requested service and extract the count from the second string.
             */
            QVector<QStringList> v = reply.value();
            QVector<QStringList>::const_iterator pos = qFind(
                    v, QGalleryTrackerType(service ));
            if (pos != v.constEnd())
                count = (*pos).last().toInt();

            // TODO Do we need this?
            if (refresh) {
                refresh = false;
                queryCount();
            }
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

                if (cancelled)
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
    d_func()->cancelled = true;
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

    if (!d->cancelled && (d->updateMask & serviceId)) {
        d->refresh = true;

        if (!d->queryWatcher)
            d->queryCount();

    }
}

#include "moc_qgallerytrackertyperesultset_p.cpp"

QTM_END_NAMESPACE
