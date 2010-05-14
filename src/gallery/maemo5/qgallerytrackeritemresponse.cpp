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

#include "qgallerytrackeritemresponse_p.h"

#include "qgallerytrackeritemlist_p_p.h"
#include "qgallerytrackerschema_p.h"

#include <QtDBus/qdbuspendingreply.h>

Q_DECLARE_METATYPE(QVector<QStringList>)

QTM_BEGIN_NAMESPACE

class QGalleryTrackerItemResponsePrivate : public QGalleryTrackerItemListPrivate
{
    Q_DECLARE_PUBLIC(QGalleryTrackerItemResponse)
public:
    bool descendingSortOrder;
    int lowerThreshold;
    int upperThreshold;
    int maximumLimit;
    int limit;
    int offset;
    QGalleryDBusInterfacePointer searchInterface;
    QString query;
    QString service;
    QStringList fields;
    QStringList sortFields;
    QVector<QStringList> resultSet;
    QDBusPendingCallWatcher *getValuesCall;

    void _q_getValuesFinished(QDBusPendingCallWatcher *watcher);

    void getValues(int index);
    void getValuesFinished(const QDBusPendingReply<QVector<QStringList> > &reply);
};

void QGalleryTrackerItemResponsePrivate::_q_getValuesFinished(QDBusPendingCallWatcher *watcher)
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

void QGalleryTrackerItemResponsePrivate::getValues(int index)
{
    offset = index;

    lowerThreshold = qMax(0, offset - 32);
    upperThreshold = lowerThreshold + 96;

    QList<QVariant> arguments = QList<QVariant>()
            << 0 //live query id.
            << service
            << fields
            << QString() // Search text
            << QStringList()
            << query
            << false // Sort by service
            << sortFields
            << descendingSortOrder
            << offset
            << limit;

    QDBusPendingCall call = searchInterface->asyncCallWithArgumentList(
            QLatin1String("Query"), arguments);

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

void QGalleryTrackerItemResponsePrivate::getValuesFinished(
        const QDBusPendingReply<QVector<QStringList> > &reply)
{
    if (updateState == QGalleryTrackerItemList::UpToDate) {
        q_func()->updateResultSet(reply.value(), offset);

        const int index = (cursorPosition + 63) & ~63;

        if (index >= 0 && index < maximumLimit
                && (index < lowerThreshold || index >= upperThreshold)) {
            getValues(index);
        }
    } else {
        resultSet = reply.value();
    }
}

QGalleryTrackerItemResponse::QGalleryTrackerItemResponse(
        const QGalleryDBusInterfacePointer &searchInterface,
        const QGalleryTrackerSchema &schema,
        const QString &query,
        int cursorPosition,
        int minimumPagedItems,
        QObject *parent)
    : QGalleryTrackerItemList(
            *new QGalleryTrackerItemResponsePrivate,
            schema,
            cursorPosition,
            minimumPagedItems,
            parent)
{
    Q_D(QGalleryTrackerItemResponse);

    const QVector<QGalleryTrackerSortCriteria> sortCriteria = schema.sortCriteria();

    d->descendingSortOrder = !sortCriteria.isEmpty()
            && sortCriteria.first().flags & QGalleryTrackerSortCriteria::Descending;
    d->limit = qMax(256, 128 + ((minimumPagedItems + 63) & ~63));
    d->maximumLimit = INT_MAX;
    d->searchInterface = searchInterface;
    d->query = query;
    d->service = schema.service();
    d->fields = schema.fields();
    d->sortFields = schema.sortFields();
    d->getValuesCall = 0;

    d->getValues((cursorPosition + 63) & ~63);
}

QGalleryTrackerItemResponse::~QGalleryTrackerItemResponse()
{
}

void QGalleryTrackerItemResponse::setCursorPosition(int position)
{
    Q_D(QGalleryTrackerItemResponse);

    d->cursorPosition = position;

    const int index = (position + 63) & ~63;

    if (d->resultSet.isEmpty() && !d->getValuesCall
            && index >= 0 && index < d->maximumLimit
            && (index < d->lowerThreshold || index >= d->upperThreshold)) {
        d->getValues(index);
    }
}

void QGalleryTrackerItemResponse::cancel()
{

}

bool QGalleryTrackerItemResponse::waitForFinished(int msecs)
{

    return false;
}


void QGalleryTrackerItemResponse::updateStateChanged(UpdateState state)
{
    Q_D(QGalleryTrackerItemResponse);

    if (d->updateState == UpToDate && !d->resultSet.isEmpty()) {
        updateResultSet(d->resultSet, d->offset);

        d->resultSet.clear();

        const int index = (d->cursorPosition + 63) & ~63;

        if (index >= 0 && index < d->maximumLimit
                && (index < d->lowerThreshold || index >= d->upperThreshold)) {
            d->getValues(index);
        }
    }
}

#include "moc_qgallerytrackeritemresponse_p.cpp"

QTM_END_NAMESPACE
