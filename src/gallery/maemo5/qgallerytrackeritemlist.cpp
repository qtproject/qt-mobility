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

#include "qgallerytrackeritemlist_p_p.h"

#include "qgallerytrackermetadataedit_p.h"
#include <QtCore/qtconcurrentrun.h>
#include <QtCore/qdatetime.h>
#include <QtDBus/qdbusreply.h>

Q_DECLARE_METATYPE(QVector<QStringList>)

QTM_BEGIN_NAMESPACE

void QGalleryTrackerItemListPrivate::update()
{
    flags &= ~UpdateRequested;

    updateTimer.stop();

    typedef QList<QGalleryTrackerMetaDataEdit *>::iterator iterator;
    for (iterator it = edits.begin(), end = edits.end(); it != end; ++it)
        (*it)->commit();
    edits.clear();

    if (!(flags & (Active | Cancelled))) {
        if (cursorPosition > iCache.index + queryLimit - minimumPagedItems) {
            query(qMax(0, cursorPosition - minimumPagedItems) & ~63);
        } else if (cursorPosition < iCache.index) {
            query(qMax(0, cursorPosition - queryLimit + 2 * minimumPagedItems) & ~63);
        } else if (flags & Refresh) {
            query(iCache.index);
        }

        flags &= ~(Refresh | PositionUpdated);
    }

    if (imageColumns.count() > 0) {
        const int minimumIndex = qMax(iCache.index, qMax(0, cursorPosition) & ~7) ;
        const int maximumIndex = qMin(iCache.cutoff, minimumIndex + minimumPagedItems + 8);

        if (imageCacheLimit > imageCacheIndex) {
            if (minimumIndex > imageCacheIndex) {
                removeHeadImages(minimumIndex < imageCacheLimit
                        ? minimumIndex - imageCacheIndex
                        : imageCacheLimit - imageCacheIndex);
            } else if (maximumIndex < imageCacheLimit) {
                removeTailImages(maximumIndex > imageCacheIndex
                        ? imageCacheLimit - maximumIndex
                        : imageCacheLimit - imageCacheIndex);
            }
        }

        if (iCache.cutoff > iCache.index) {
            if (minimumIndex < imageCacheIndex) {
                insertHeadImages(minimumIndex, qMin(imageCacheIndex - minimumIndex, minimumPagedItems));
            } else if (maximumIndex > imageCacheLimit) {
                const int count = qMin(maximumIndex - imageCacheLimit, minimumPagedItems);

                insertTailImages(maximumIndex - count, count);
            }
        }
    }
}

void QGalleryTrackerItemListPrivate::removeHeadImages(int count)
{
    imageCacheIndex += count;

    typedef QVector<QGalleryTrackerImageColumn *>::const_iterator iterator;
    for (iterator it = imageColumns.constBegin(), end = imageColumns.constEnd(); it != end; ++it) {
        (*it)->removeImages(0, count);
        (*it)->moveOffset(imageCacheIndex);
    }
}

void QGalleryTrackerItemListPrivate::removeTailImages(int count)
{
    imageCacheLimit -= count;

    const int index = imageCacheLimit - imageCacheIndex;

    typedef QVector<QGalleryTrackerImageColumn *>::const_iterator iterator;
    for (iterator it = imageColumns.constBegin(), end = imageColumns.constEnd(); it != end; ++it)
        (*it)->removeImages(index, index + count);
}

void QGalleryTrackerItemListPrivate::insertHeadImages(int index, int count)
{
    imageCacheLimit += index - imageCacheIndex + count;
    imageCacheIndex = index;

    QVector<QVariant>::const_iterator row
            = iCache.values.constBegin() + ((index - iCache.index) * tableWidth);

    typedef QVector<QGalleryTrackerImageColumn *>::const_iterator iterator;
    for (iterator it = imageColumns.constBegin(), end = imageColumns.constEnd(); it != end; ++it) {
        (*it)->insertImages(0, count, row, tableWidth);
        (*it)->moveOffset(imageCacheIndex);
    }
}

void QGalleryTrackerItemListPrivate::insertTailImages(int index, int count)
{
    const int relativeIndex = imageCacheLimit - imageCacheIndex;

    imageCacheIndex = index - relativeIndex;
    imageCacheLimit = index + count;

    QVector<QVariant>::const_iterator row
            = iCache.values.constBegin() + ((index - iCache.index) * tableWidth);

    typedef QVector<QGalleryTrackerImageColumn *>::const_iterator iterator;
    for (iterator it = imageColumns.constBegin(), end = imageColumns.constEnd(); it != end; ++it) {
        (*it)->insertImages(relativeIndex, relativeIndex + count, row, tableWidth);
        (*it)->moveOffset(imageCacheIndex);
    }
}

void QGalleryTrackerItemListPrivate::query(int index)
{
    flags &= ~(Refresh | SyncFinished);
    flags |= Active;

    updateTimer.stop();

    rCache.index = iCache.index;
    rCache.limit = iCache.limit;
    rCache.offset = iCache.index;

    iCache.index = index;
    iCache.limit = index + queryLimit;
    iCache.cutoff = index;

    qSwap(rCache.values, iCache.values);

    QDBusPendingCall call = queryInterface->asyncCallWithArgumentList(
            queryMethod, QVariantList(queryArguments) << index << queryLimit);

    if (call.isFinished()) {
        queryFinished(call);
    } else {
        queryWatcher.reset(new QDBusPendingCallWatcher(call));

        QObject::connect(
                queryWatcher.data(), SIGNAL(finished(QDBusPendingCallWatcher*)),
                q_func(), SLOT(_q_queryFinished(QDBusPendingCallWatcher*)));

        emit q_func()->progressChanged(0, 2);
    }

    const int statusLimit = qMin(rowCount, iCache.limit);

    if (statusLimit > iCache.index)
        emit q_func()->statusChanged(iCache.index, statusLimit - iCache.index);
}

void QGalleryTrackerItemListPrivate::_q_queryFinished(QDBusPendingCallWatcher *watcher)
{
    if (queryWatcher.data() == watcher) {
        queryWatcher.take()->deleteLater();

        queryFinished(*watcher);
    }
}

void QGalleryTrackerItemListPrivate::queryFinished(const QDBusPendingCall &call)
{
    if (call.isError()) {
        emit q_func()->progressChanged(2, 2);

        qWarning("DBUS error %s", qPrintable(call.error().message()));

        flags &= ~Active;

        q_func()->finish(QGalleryAbstractRequest::ConnectionError);
    } else if (flags & Cancelled) {
        iCache.limit = 0;

        flags &= ~Active;

        q_func()->QGalleryAbstractResponse::cancel();
    } else {
        parseWatcher.setFuture(QtConcurrent::run(
                this, &QGalleryTrackerItemListPrivate::parseRows, call));

        emit q_func()->progressChanged(1, 2);
    }
}

void QGalleryTrackerItemListPrivate::parseRows(const QDBusPendingCall &call)
{
    QDBusReply<QVector<QStringList> > reply(call);

    typedef QVector<QStringList>::const_iterator iterator;

    const QVector<QStringList> resultSet = reply.value();

    iCache.limit = iCache.index + resultSet.count();

    QVector<QVariant> &values = iCache.values;
    values.clear();
    values.reserve(resultSet.count() * tableWidth);

    for (iterator it = resultSet.begin(), end = resultSet.end(); it != end; ++it) {
        for (int i = 0, count = qMin(valueOffset, it->count()); i < count; ++i)
            values.append(it->at(i));

        for (int i = valueOffset, count = qMin(tableWidth, it->count()); i < count; ++i)
            values.append(valueColumns.at(i - valueOffset)->toVariant(it->at(i)));

        // The rows should all have a count equal to tableWidth, but check just in case.
        for (int i = qMin(tableWidth, it->count()); i < tableWidth; ++i)
            values.append(QVariant());
    }

    if (!values.isEmpty()) {
        if (!sortCriteria.isEmpty()) {
            correctRows(
                    row_iterator(values.begin(), tableWidth),
                    row_iterator(values.end(), tableWidth),
                    sortCriteria.constBegin(),
                    sortCriteria.constEnd());
        }
    }

    synchronize();
}

void QGalleryTrackerItemListPrivate::correctRows(
        row_iterator begin,
        row_iterator end,
        sort_iterator sortCriteria,
        sort_iterator sortEnd,
        bool reversed) const
{
    int column = sortCriteria->column;

    const int sortFlags = sortCriteria->flags;

    if (sortFlags & QGalleryTrackerSortCriteria::Sorted) {
        if (reversed) {
            QAlgorithmsPrivate::qReverse(begin, end);

            reversed = false;

            if (++sortCriteria == sortEnd)
                return;
        } else do {
            column = sortCriteria->column;

            if (++sortCriteria == sortEnd)
                return;
        } while(sortCriteria->flags & QGalleryTrackerSortCriteria::Sorted);
    } else if (sortFlags & QGalleryTrackerSortCriteria::ReverseSorted) {
        if (!reversed) {
            QAlgorithmsPrivate::qReverse(begin, end);

            reversed = true;

            if (++sortCriteria == sortEnd)
                return;
        } else do {
            column = sortCriteria->column;

            if (++sortCriteria == sortEnd)
                return;
        } while(sortCriteria->flags & QGalleryTrackerSortCriteria::ReverseSorted);
    }

    for (row_iterator upper, lower = begin; lower != end; lower = upper) {
        int count = 1;

        for (upper = lower + 1; upper != end && lower[column] == upper[column]; ++upper, ++count) {}

        if (count > 1)
            correctRows(lower, upper, sortCriteria, sortEnd, reversed);
    }
}

void QGalleryTrackerItemListPrivate::synchronize()
{
    const row_iterator rEnd(rCache.values.end(), tableWidth);
    const row_iterator iEnd(iCache.values.end(), tableWidth);

    row_iterator rBegin(rCache.values.begin(), tableWidth);
    row_iterator iBegin(iCache.values.begin(), tableWidth);

    row_iterator rIt = rBegin;
    row_iterator iIt = iBegin;

    bool equal = false;

    if (iCache.index > rCache.index && iCache.index < rCache.limit) {
        const int offset = iCache.index - rCache.index;

        if ((equal = iBegin.isEqual(rBegin + offset, identityWidth))) {
            rIt = rBegin + offset;

            postSyncEvent(SyncEvent::startEvent(rCacheIndex(rIt), 0, iCache.index, 0));
        } else {
            rIt = rBegin + qMax(0, offset - 8);
        }
    } else if (rCache.index > iCache.index && rCache.index < iCache.limit) {
        const int offset = rCache.index - iCache.index;

        if ((equal = rBegin.isEqual(iBegin + offset, identityWidth))) {
            iIt = iBegin + offset;

            postSyncEvent(SyncEvent::startEvent(rCache.index, 0, iCacheIndex(iIt), 0));
        } else {
            iIt = iBegin + qMax(0, offset - 8);
        }
    }

    if (!equal) {
        const int rStep = qMax(64, rEnd - rBegin) / 16;
        const int iStep = qMax(64, iEnd - iBegin) / 16;

        row_iterator rOuterEnd = rBegin + ((((rEnd - rBegin) + iStep - 1) / rStep) * rStep);
        row_iterator iOuterEnd = iBegin + ((((iEnd - iBegin) + iStep - 1) / iStep) * iStep);

        row_iterator rInnerEnd = qMin(rBegin + rStep, rEnd);
        row_iterator iInnerEnd = qMin(iBegin + iStep, iEnd);

        for (row_iterator rOuter = rBegin, iOuter = iBegin;
                !equal && rOuter != rOuterEnd && iOuter != iOuterEnd;
                rOuter += rStep / 2, iOuter += iStep / 2) {
            for (row_iterator rInner = rIt, iInner = iIt;
                    rInner != rInnerEnd && iInner != iInnerEnd;
                    ++rInner, ++iInner) {
                if ((equal = rInner.isEqual(iOuter, identityWidth))) {
                    do {
                        rIt = rInner;
                        iIt = iOuter;
                    } while (rInner-- != rBegin && iOuter-- != iBegin
                             && rInner.isEqual(iOuter, identityWidth));

                    const int rIndex = rCacheIndex(rOuter);
                    const int iIndex = iCacheIndex(iBegin);
                    const int rCount = rIt - rBegin;
                    const int iCount = iIt - iBegin;

                    postSyncEvent(SyncEvent::startEvent(rIndex, rCount, iIndex, iCount));

                    break;
                } else if ((equal = iInner.isEqual(rOuter, identityWidth))) {
                    do {
                        rIt = rOuter;
                        iIt = iInner;
                    } while (iInner-- != iBegin && rOuter-- != rBegin
                           && iInner.isEqual(rOuter, identityWidth));

                    const int rIndex = rCacheIndex(rBegin);
                    const int iIndex = iCacheIndex(iOuter);
                    const int rCount = rIt - rBegin;
                    const int iCount = iIt - iBegin;

                    postSyncEvent(SyncEvent::startEvent(rIndex, rCount, iIndex, iCount));

                    break;
                }
            }
        }
    }

    if (equal) {
        synchronizeRows(rIt, iIt, rEnd, iEnd);

        postSyncEvent(SyncEvent::finishEvent(rCacheIndex(rIt), iCacheIndex(iIt)));
    } else {
        postSyncEvent(SyncEvent::startEvent(0, 0, 0, 0));
        postSyncEvent(SyncEvent::finishEvent(rCache.index, iCache.index));
    }
}

void QGalleryTrackerItemListPrivate::synchronizeRows(
        row_iterator &rBegin,
        row_iterator &iBegin,
        const row_iterator &rEnd,
        const row_iterator &iEnd)
{
    const int rStep = qMax(64, rEnd - rBegin) / 16;
    const int iStep = qMax(64, iEnd - iBegin) / 16;

    for (bool equal = true; equal && rBegin != rEnd && iBegin != iEnd; ) {
        bool changed = false;

        do {    // Skip over identical rows.
            if ((equal = rBegin.isEqual(iBegin, identityWidth))
                    && !(changed = !rBegin.isEqual(iBegin, identityWidth, tableWidth))) {
                ++rBegin;
                ++iBegin;
            } else {
                break;
            }
        } while (iBegin != rEnd && iBegin != iEnd);

        if (changed) {
            row_iterator rIt = rBegin;
            row_iterator iIt = iBegin;

            do {    // Skip over rows with equal IDs but different values.
                if ((equal = rIt.isEqual(iIt, identityWidth))
                        && rIt.isEqual(iIt, identityWidth, tableWidth)) {
                    ++rIt;
                    ++iIt;
                } else {
                    ++rIt;
                    ++iIt;

                    break;
                }
            } while (rIt != rEnd && iIt != iEnd);

            const int rIndex = rCacheIndex(rBegin);
            const int iIndex = iCacheIndex(iBegin);
            const int count = iIt - iBegin;

            postSyncEvent(SyncEvent::updateEvent(rIndex, iIndex, count));

            rBegin = rIt;
            iBegin = iIt;

            continue;
        } else if (equal) {
            return;
        }

        row_iterator rOuterEnd = rBegin + ((((rEnd - rBegin) + iStep - 1) / rStep) * rStep);
        row_iterator iOuterEnd = iBegin + ((((iEnd - iBegin) + iStep - 1) / iStep) * iStep);

        row_iterator rInnerEnd = qMin(rBegin + rStep, rEnd);
        row_iterator iInnerEnd = qMin(iBegin + iStep, iEnd);

        for (row_iterator rOuter = rBegin, iOuter = iBegin;
                !equal && rOuter != rOuterEnd && iOuter != iOuterEnd;
                rOuter += rStep / 2, iOuter += iStep / 2) {
            for (row_iterator rInner = rBegin, iInner = iBegin;
                    rInner != rInnerEnd && iInner != iInnerEnd;
                    ++rInner, ++iInner) {
                if ((equal = rInner.isEqual(iOuter, identityWidth))) {
                    row_iterator rIt;
                    row_iterator iIt;

                    do {
                        rIt = rInner;
                        iIt = iOuter;
                    } while (rInner-- != rBegin && iOuter-- != iBegin
                             && rInner.isEqual(iOuter, identityWidth));

                    const int rIndex = rCacheIndex(rOuter);
                    const int iIndex = iCacheIndex(iBegin);
                    const int rCount = rIt - rBegin;
                    const int iCount = iIt - iBegin;

                    postSyncEvent(SyncEvent::replaceEvent(rIndex, rCount, iIndex, iCount));

                    rBegin = rIt;
                    iBegin = iIt;

                    break;
                } else if ((equal = iInner.isEqual(rOuter, identityWidth))) {
                    row_iterator rIt;
                    row_iterator iIt;

                    do {
                        rIt = rOuter;
                        iIt = iInner;
                    } while (iInner-- != iBegin && rOuter-- != rBegin
                           && iInner.isEqual(rOuter, identityWidth));

                    const int rIndex = rCacheIndex(rBegin);
                    const int iIndex = iCacheIndex(iOuter);
                    const int rCount = rIt - rBegin;
                    const int iCount = iIt - iBegin;

                    postSyncEvent(SyncEvent::replaceEvent(rIndex, rCount, iIndex, iCount));

                    rBegin = rIt;
                    iBegin = iIt;

                    break;
                }
            }
        }
    }
}

void QGalleryTrackerItemListPrivate::processSyncEvents()
{
    while (SyncEvent *event = syncEvents.dequeue()) {
        switch (event->type) {
        case SyncEvent::Start:
            syncStart(event->rIndex, event->rCount, event->iIndex, event->iCount);
            break;
        case SyncEvent::Update:
            syncUpdate(event->rIndex, event->rCount, event->iIndex, event->iCount);
            break;
        case SyncEvent::Replace:
            syncReplace(event->rIndex, event->rCount, event->iIndex, event->iCount);
            break;
        case SyncEvent::Finish:
            syncFinish(event->rIndex, event->iIndex);
            break;
        default:
            break;
        }

        delete event;
    }
}

void QGalleryTrackerItemListPrivate::syncStart(
        const int rIndex, const int rCount, const int iIndex, const int iCount)
{
    if (iIndex > rCache.index) {
        const int statusCount = qMin(iIndex, rCache.limit) - rCache.index;

        iCache.cutoff = iIndex;

        emit q_func()->statusChanged(rCache.index, statusCount);
    }

    if (rCount > 0) {
        rCache.offset = rIndex + rCount;
        iCache.cutoff = iIndex;

        rowCount -= rCount;

        emit q_func()->removed(iIndex, rCount);
    }

    if (iCount > 0) {
        rCache.offset = rIndex + rCount;
        iCache.cutoff = iIndex + iCount;

        rowCount += iCount;

        emit q_func()->inserted(iIndex, iCount);
    } else if (iIndex > iCache.cutoff) {
        const int statusCount = iIndex - iCache.cutoff;

        iCache.cutoff = iIndex;

        emit q_func()->metaDataChanged(iCache.index, statusCount);
        emit q_func()->statusChanged(iCache.index, statusCount);
    }
}

void QGalleryTrackerItemListPrivate::syncUpdate(
        const int rIndex, const int rCount, const int iIndex, const int iCount)
{
    const int statusIndex = iCache.cutoff;

    rCache.offset = rIndex + rCount;
    iCache.cutoff = iIndex + iCount;

    emit q_func()->metaDataChanged(iIndex, iCount);
    emit q_func()->statusChanged(statusIndex, iCache.cutoff - statusIndex);
}

void QGalleryTrackerItemListPrivate::syncReplace(
        const int rIndex, const int rCount, const int iIndex, const int iCount)
{
    if (iIndex > iCache.cutoff) {
        const int statusIndex = iCache.cutoff;

        iCache.cutoff = iIndex;

        emit q_func()->statusChanged(statusIndex, iCache.cutoff - statusIndex);
    }

    if (rCount > 0) {
        rCache.offset = rIndex + rCount;
        iCache.cutoff = iIndex;

        rowCount -= rCount;

        emit q_func()->removed(iIndex, rCount);
    }

    if (iCount > 0) {
        rCache.offset = rIndex + rCount;
        iCache.cutoff = iIndex + iCount;

        rowCount += iCount;

        emit q_func()->inserted(iIndex, iCount);
    }
}

void QGalleryTrackerItemListPrivate::syncFinish(
        const int rIndex, const int iIndex)
{
    const int rCount = rCache.limit - rIndex;
    const int iCount = iCache.limit - iIndex;

    if (rCount > 0) {
        if (iCache.limit - iCache.index < queryLimit) {
            rCache.offset = rCache.limit;

            rowCount -= rCount;

            emit q_func()->removed(iIndex, rCount);
        } else {
            const int statusIndex = rCache.offset;

            rCache.offset = rCache.limit;
            iCache.cutoff = iIndex;

            emit q_func()->statusChanged(statusIndex, rCache.limit - statusIndex);
        }
    } else {
        rCache.offset = rCache.limit;
    }

    if (iCache.cutoff < iIndex) {
        const int statusIndex = iCache.cutoff;

        iCache.cutoff = iIndex;

        emit q_func()->statusChanged(statusIndex, iCache.cutoff - statusIndex);
    }

    if (iCount > 0) {
        if (iIndex < rowCount) {
            iCache.cutoff = qMin(rowCount, iCache.limit);

            emit q_func()->metaDataChanged(iIndex, iCache.cutoff - iIndex);
            emit q_func()->statusChanged(iIndex, iCache.cutoff - iIndex);
        } else {
            iCache.cutoff = iIndex;
        }

        if (iCache.cutoff < iCache.limit) {
            const int index = rowCount;

            iCache.cutoff = iCache.limit;
            rowCount = iCache.limit;

            q_func()->inserted(index, rowCount - index);
        }
    } else if (iCache.cutoff < iCache.limit) {
        const int statusIndex = iCache.cutoff;
        const int statusCount = iCache.limit - iCache.cutoff;

        iCache.cutoff = iCache.limit;

        emit q_func()->statusChanged(statusIndex, statusCount);
    }

    flags |= SyncFinished;
}

bool QGalleryTrackerItemListPrivate::waitForSyncFinish(int msecs)
{
    QTime timer;
    timer.start();

    do {
        processSyncEvents();

        if (flags & SyncFinished) {
            return true;
        }

        if (!syncEvents.waitForEvent(msecs))
            return false;
    } while ((msecs -= timer.restart()) > 0);

    return false;
}

void QGalleryTrackerItemListPrivate::_q_parseFinished()
{
    processSyncEvents();

    rCache.values.clear();
    rCache.limit = 0;

    flags &= ~Active;

    q_func()->setCursorPosition(cursorPosition);

    if (flags & (Refresh | PositionUpdated))
        update();
    else
        emit q_func()->progressChanged(2, 2);

    q_func()->finish(QGalleryAbstractRequest::Succeeded, flags & Live);
}

void QGalleryTrackerItemListPrivate::_q_editFinished(QGalleryTrackerMetaDataEdit *edit)
{
    edit->deleteLater();

    emit q_func()->itemEdited(edit->service());
}

QGalleryTrackerItemList::QGalleryTrackerItemList(
        const QGalleryTrackerItemListArguments &arguments,
        bool live,
        int cursorPosition,
        int minimumPagedItems,
        QObject *parent)
    : QGalleryAbstractResponse(
            *new QGalleryTrackerItemListPrivate(arguments, live, cursorPosition, minimumPagedItems),
            parent)
{
    Q_D(QGalleryTrackerItemList);

    for (int i = 0; i < d->imageColumns.count(); ++i) {
        connect(d->imageColumns.at(i), SIGNAL(imagesChanged(int,int,QList<int>)),
                this, SIGNAL(metaDataChanged(int,int,QList<int>)));
    }

    connect(&d->parseWatcher, SIGNAL(finished()), this, SLOT(_q_parseFinished()));

    d_func()->query(qMax(0, d->cursorPosition - d->minimumPagedItems) & ~63);
}

QGalleryTrackerItemList::QGalleryTrackerItemList(
        QGalleryTrackerItemListPrivate &dd,
        QObject *parent)
    : QGalleryAbstractResponse(dd, parent)
{
    Q_D(QGalleryTrackerItemList);

    for (int i = 0; i < d->imageColumns.count(); ++i) {
        connect(d->imageColumns.at(i), SIGNAL(imagesChanged(int,int,QList<int>)),
                this, SIGNAL(metaDataChanged(int,int,QList<int>)));
    }

    connect(&d->parseWatcher, SIGNAL(finished()), this, SLOT(_q_parseFinished()));

    d_func()->query(qMax(0, d->cursorPosition - d->minimumPagedItems) & ~63);
}

QGalleryTrackerItemList::~QGalleryTrackerItemList()
{
    Q_D(QGalleryTrackerItemList);

    typedef QList<QGalleryTrackerMetaDataEdit *>::iterator iterator;
    for (iterator it = d->edits.begin(), end = d->edits.end(); it != end; ++it)
        (*it)->commit();

    d->parseWatcher.waitForFinished();
}

QStringList QGalleryTrackerItemList::propertyNames() const
{
    return d_func()->propertyNames;
}

int QGalleryTrackerItemList::propertyKey(const QString &property) const
{
    Q_D(const QGalleryTrackerItemList);

    int index = d->propertyNames.indexOf(property);

    return index >= 0
            ? index + d->valueOffset
            : -1;
}

QGalleryProperty::Attributes QGalleryTrackerItemList::propertyAttributes(int key) const
{
    return d_func()->propertyAttributes.value(key - d_func()->valueOffset);
}

QVariant::Type QGalleryTrackerItemList::propertyType(int key) const
{
    return d_func()->propertyTypes.value(key - d_func()->valueOffset);
}

int QGalleryTrackerItemList::count() const
{
    return d_func()->rowCount;
}

void QGalleryTrackerItemList::setCursorPosition(int position)
{
    Q_D(QGalleryTrackerItemList);

    d->cursorPosition = position;

    d->flags |= QGalleryTrackerItemListPrivate::PositionUpdated;

    d->requestUpdate();
}

QVariant QGalleryTrackerItemList::id(int index) const
{
    Q_D(const QGalleryTrackerItemList);

    if (index < d->iCache.cutoff) {
        if ((index -= d->iCache.index) >= 0)
            return d->idColumn->value(d->iCache.values.begin() + (index * d->tableWidth));
    } else if (index < d->rCache.limit && (index -= d->rCache.offset) >= 0) {
        return d->idColumn->value(d->rCache.values.begin() + (index * d->tableWidth));
    }

    return QVariant();
}

QUrl QGalleryTrackerItemList::url(int index) const
{
    Q_D(const QGalleryTrackerItemList);

    if (index < d->iCache.cutoff) {
        if ((index -= d->iCache.index) >= 0)
            return d->urlColumn->value(d->iCache.values.begin() + (index * d->tableWidth)).toUrl();
    } else if (index < d->rCache.limit && (index -= d->rCache.offset) >= 0) {
        return d->urlColumn->value(d->rCache.values.begin() + (index * d->tableWidth)).toUrl();
    }

    return QUrl();
}

QString QGalleryTrackerItemList::type(int index) const
{
    Q_D(const QGalleryTrackerItemList);

    if (index < d->iCache.cutoff) {
        if ((index -= d->iCache.index) >= 0) {
            return d->typeColumn->value(
                    d->iCache.values.begin() + (index * d->tableWidth)).toString();
        }
    } else if (index < d->rCache.limit && (index -= d->rCache.offset) >= 0) {
        return d->typeColumn->value(d->rCache.values.begin() + (index * d->tableWidth)).toString();
    }

    return QString();
}

QList<QGalleryResource> QGalleryTrackerItemList::resources(int index) const
{
    Q_D(const QGalleryTrackerItemList);

    QList<QGalleryResource> resources;

    QUrl url = QGalleryTrackerItemList::url(index);

    if (!url.isEmpty()) {
        QMap<int, QVariant> attributes;

        typedef QVector<int>::const_iterator iterator;
        for (iterator it = d->resourceKeys.begin(), end = d->resourceKeys.end(); it != end; ++it) {
            QVariant value = metaData(index, *it);

            if (!value.isNull())
                attributes.insert(*it, value);
        }

        resources.append(QGalleryResource(url, attributes));
    }

    return resources;
}

QGalleryItemList::ItemStatus QGalleryTrackerItemList::status(int index) const
{
    Q_D(const QGalleryTrackerItemList);

    ItemStatus status;

    typedef QList<QGalleryTrackerMetaDataEdit *>::const_iterator iterator;
    for (iterator it = d->edits.begin(), end = d->edits.end(); it != end; ++it) {
        if ((*it)->index() == index) {
            status |= Writing;
            break;
        }
    }

    if (index < d->iCache.cutoff) {
        if (index >= d->iCache.index)
            return status;
    } else {
        if (index >= d->iCache.index && index < d->iCache.limit)
            status |= Reading;

        if (index < d->rCache.limit && index >= d->rCache.offset)
            return status;
    }

    status |= OutOfRange;

    return status;
}

QVariant QGalleryTrackerItemList::metaData(int index, int key) const
{
    Q_D(const QGalleryTrackerItemList);

    if (key >= d->valueOffset) {
        if (key < d->compositeOffset) {         // Value column.
            if (index < d->iCache.cutoff) {
                if ((index -= d->iCache.index) >= 0)
                    return d->iCache.values.at(index * d->tableWidth + key);
            } else if (index < d->rCache.limit && (index -= d->rCache.offset) >= 0) {
                return d->rCache.values.at(index * d->tableWidth + key);
            }
        } else if (key < d->aliasOffset) {      // Composite column.
            key -= d->compositeOffset;

            if (index < d->iCache.cutoff) {
                if ((index -= d->iCache.index) >= 0) {
                    return d->compositeColumns.at(key)->value(
                            d->iCache.values.begin() + (index * d->tableWidth));
                }
            } else if (index < d->rCache.limit && (index -= d->rCache.offset) >= 0) {
                return d->compositeColumns.at(key)->value(
                        d->rCache.values.begin() + (index * d->tableWidth));
            }
        } else if (key < d->imageOffset) {      // Alias column.
            key = d->aliasColumns.at(key - d->aliasOffset);

            if (index < d->iCache.cutoff) {
                if ((index -= d->iCache.index) >= 0)
                    return d->iCache.values.at(index * d->tableWidth + key);
            } else if (index < d->rCache.limit && (index -= d->rCache.offset) >= 0) {
                return d->rCache.values.at(index * d->tableWidth + key);
            }
        } else if (key < d->columnCount
                && index < d->imageCacheLimit
                && (index -= d->imageCacheIndex) >= 0) {
            return d->imageColumns.at(key - d->imageOffset)->image(index);
        }
    }

    return QVariant();
}

void QGalleryTrackerItemList::setMetaData(int, int, const QVariant &)
{

}

void QGalleryTrackerItemList::cancel()
{
    d_func()->flags |= QGalleryTrackerItemListPrivate::Cancelled;
    d_func()->flags &= ~QGalleryTrackerItemListPrivate::Live;

    if (!(d_func()->flags &QGalleryTrackerItemListPrivate::Active))
        QGalleryAbstractResponse::cancel();
}

bool QGalleryTrackerItemList::waitForFinished(int msecs)
{
    Q_D(QGalleryTrackerItemList);

    QTime timer;
    timer.start();

    do {
        if (d->queryWatcher) {
            QScopedPointer<QDBusPendingCallWatcher> watcher(d_func()->queryWatcher.take());

            watcher->waitForFinished();

            d->queryFinished(*watcher);

            if (!(d->flags &QGalleryTrackerItemListPrivate::Active))
                return true;
        } else if (d->flags & QGalleryTrackerItemListPrivate::Active) {
            if (d->waitForSyncFinish(msecs)) {
                d->parseWatcher.waitForFinished();

                d->_q_parseFinished();

                return true;
            } else {
                return false;
            }
        } else if (d->flags & (QGalleryTrackerItemListPrivate::Refresh
                | QGalleryTrackerItemListPrivate::PositionUpdated)) {
            d->update();
        } else {
            return true;
        }
    } while ((msecs -= timer.restart()) > 0);

    return false;
}

bool QGalleryTrackerItemList::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::UpdateRequest:
        d_func()->update();

        return true;
    case QEvent::UpdateLater:
        d_func()->processSyncEvents();

        return true;
    default:
        return QGalleryAbstractResponse::event(event);
    }
}

void QGalleryTrackerItemList::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == d_func()->updateTimer.timerId()) {
        d_func()->update();

        event->accept();
   }
}

void QGalleryTrackerItemList::refresh(int updateId)
{
    Q_D(QGalleryTrackerItemList);

    if ((d->updateMask & updateId)
            && !d->updateTimer.isActive()
            && (d->flags & QGalleryTrackerItemListPrivate::Live)) {


        d->flags |= QGalleryTrackerItemListPrivate::Refresh;

        if (!(d->flags & QGalleryTrackerItemListPrivate::Active)) {
            d->updateTimer.start(100, this);
        }
    }
}

#include "moc_qgallerytrackeritemlist_p.cpp"

QTM_END_NAMESPACE
