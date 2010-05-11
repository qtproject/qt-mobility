/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qgallerytrackeraggregateresponse_p.h"

#include "qgallerytrackeritemlist_p_p.h"
#include "qgallerytrackerschema_p.h"

#include <QtDBus/qdbuspendingreply.h>

Q_DECLARE_METATYPE(QVector<QStringList>)

QTM_BEGIN_NAMESPACE

class QGalleryTrackerAggregateResponsePrivate : public QGalleryTrackerItemListPrivate
{
    Q_DECLARE_PUBLIC(QGalleryTrackerAggregateResponse)
public:
    bool descendingSortOrder;
    QGalleryDBusInterfacePointer metaDataInterface;
    QString query;
    QString service;
    QStringList identityFields;
    QStringList aggregates;
    QStringList aggregateFields;
    QVector<QStringList> resultSet;
    QDBusPendingCallWatcher *getValuesCall;

    void _q_getValuesFinished(QDBusPendingCallWatcher *watcher);

    void getValues(int offset, int limit);
    void getValuesFinished(const QDBusPendingReply<QVector<QStringList> > &reply);
};

void QGalleryTrackerAggregateResponsePrivate::_q_getValuesFinished(QDBusPendingCallWatcher *watcher)
{
    if (watcher->isError()) {
        qWarning("DBUS error %s", qPrintable(watcher->error().message()));

        q_func()->finish(QGalleryAbstractRequest::ConnectionError);
    } else {
        getValuesCall->deleteLater();
        getValuesCall = 0;

        getValuesFinished(*watcher);
    }
}

void QGalleryTrackerAggregateResponsePrivate::getValues(int offset, int limit)
{
    QDBusPendingCall call = metaDataInterface->asyncCall(
            QLatin1String("GetUniqueValuesWithAggregates"),
            service,
            identityFields,
            query,
            aggregates,
            aggregateFields,
            descendingSortOrder,
            offset,
            limit);

    if (call.isError()) {
        qWarning("DBUS error %s", qPrintable(call.error().message()));

        q_func()->finish(QGalleryAbstractRequest::ConnectionError);
    } else if (call.isFinished()) {
        getValuesFinished(call);
    } else {
        getValuesCall = new QDBusPendingCallWatcher(call);

        QObject::connect(getValuesCall, SIGNAL(finished(QDBusPendingCallWatcher*)),
                q_func(), SLOT(_q_getValuesFinished(QDBusPendingCallWatcher*)));
    }
}

void QGalleryTrackerAggregateResponsePrivate::getValuesFinished(
        const QDBusPendingReply<QVector<QStringList> > &reply)
{
    if (updateState == QGalleryTrackerItemList::UpToDate) {
        q_func()->updateResultSet(reply.value());
    } else {
        resultSet = reply.value();
    }
}

QGalleryTrackerAggregateResponse::QGalleryTrackerAggregateResponse(
        const QGalleryDBusInterfacePointer &metaDataInterface,
        const QGalleryTrackerSchema &schema,
        const QString &query,
        int cursorPosition,
        int minimumPagedItems,
        QObject *parent)
    : QGalleryTrackerItemList(
            *new QGalleryTrackerAggregateResponsePrivate,
            schema,
            cursorPosition,
            minimumPagedItems,
            0,
            parent)
{
    Q_D(QGalleryTrackerAggregateResponse);

    const QVector<QGalleryTrackerSortCriteria> sortCriteria = schema.sortCriteria();

    d->descendingSortOrder = !sortCriteria.isEmpty()
            && sortCriteria.first().flags & QGalleryTrackerSortCriteria::Descending;
    d->metaDataInterface = metaDataInterface;
    d->query = query;
    d->service = schema.service();
    d->identityFields = schema.fields();
    d->aggregates = schema.aggregations();
    d->aggregateFields = schema.aggregateFields();
    d->getValuesCall = 0;

    d->getValues(0, 2048);
}

QGalleryTrackerAggregateResponse::~QGalleryTrackerAggregateResponse()
{
}

void QGalleryTrackerAggregateResponse::cancel()
{

}

bool QGalleryTrackerAggregateResponse::waitForFinished(int msecs)
{

    return false;
}


void QGalleryTrackerAggregateResponse::updateStateChanged(UpdateState state)
{

}

#include "moc_qgallerytrackeraggregateresponse_p.cpp"

QTM_END_NAMESPACE
