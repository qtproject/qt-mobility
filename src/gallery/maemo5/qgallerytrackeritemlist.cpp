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

void QGalleryTrackerItemListPrivate::update(int index)
{
    flags &= ~(Refresh | SyncFinished);
    flags |= Active;

    updateTimer.stop();


    aCache.index = rCache.index;
    aCache.count = rCache.count;
    aCache.offset = rCache.index;

    rCache.index = index;
    rCache.count = index + queryLimit;
    rCache.cutoff = 0;

    qSwap(aCache.values, rCache.values);

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
        rCache.count = 0;

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

    postSyncEvent(SyncEvent::startEvent(resultSet.count()));

    QVector<QVariant> &values = rCache.values;
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

    postSyncEvent(SyncEvent::finishEvent());
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
    const row_iterator aEnd(aCache.values.end(), tableWidth);
    const row_iterator rEnd(rCache.values.end(), tableWidth);

    row_iterator aBegin(aCache.values.begin(), tableWidth);
    row_iterator rBegin(rCache.values.begin(), tableWidth);

    row_iterator aIt = aBegin;
    row_iterator rIt = rBegin;

    bool equal = false;

    if (rCache.index > aCache.index && rCache.index < aCache.count) {
        const int offset = rCache.index - aCache.index;

        if ((equal = rBegin.isEqual(aBegin + offset, identityWidth))) {
            aIt = aBegin + offset;
        } else {
            aIt = aBegin + qMax(0, offset - 8);
        }
    } else if (aCache.index > rCache.index && aCache.index < rCache.count) {
        const int offset = aCache.index - rCache.index;

        if ((equal = aBegin.isEqual(rBegin + offset, identityWidth))) {
            rIt = rBegin + offset;
        } else {
            rIt = rBegin + qMax(0, offset - 8);
        }
    }

    if (!equal) {
        row_iterator aOuterEnd = aBegin + (((aEnd - aBegin) + 15) & ~15);
        row_iterator rOuterEnd = rBegin + (((rEnd - rBegin) + 15) & ~15);

        row_iterator aInnerEnd = qMin(aIt + 16, aEnd);
        row_iterator rInnerEnd = qMin(rIt + 16, rEnd);

        for (row_iterator aOuter = aBegin, rOuter = rBegin;
                !equal && aOuter != aOuterEnd && rOuter != rOuterEnd;
                aOuter += 16, rOuter += 16) {
            for (row_iterator aInner = aIt, rInner = rIt;
                    aInner != aInnerEnd && rInner != rInnerEnd;
                    ++aInner, ++rInner) {
                if ((equal = aInner.isEqual(rOuter, identityWidth))) {
                    aIt = aInner;
                    rIt = rOuter;

                    break;
                } else if ((equal = rInner.isEqual(aOuter, identityWidth))) {
                    aIt = aOuter;
                    rIt = rInner;

                    break;
                }
            }
        }
    }

    if (equal) {
        const int aIndex = aIt - aBegin + aCache.index;
        const int rIndex = rIt - rBegin + rCache.index;

        if (aIndex < rIndex) {
            postSyncEvent(SyncEvent::replaceEvent(aIndex, rIndex - aIndex, rIndex, 0));
        } else if (aIndex > rIndex) {
            postSyncEvent(SyncEvent::replaceEvent(aIndex, 0, rIndex, rIndex - aIndex));
        }

        if (rIndex > rCache.index) {
            postSyncEvent(SyncEvent::updateEvent(
                    aCache.index, rCache.index, rIndex - rCache.index));
        }

        synchronizeRows(aIt, rIt, aEnd, rEnd);
    } else {
        const int aCount = aCache.count - aCache.index;
        const int rCount = rCache.values.count() / tableWidth;

        postSyncEvent(SyncEvent::replaceEvent(aCache.index, aCount, rCache.index, rCount));
    }
}

void QGalleryTrackerItemListPrivate::synchronizeRows(
        row_iterator &aBegin,
        row_iterator &rBegin,
        const row_iterator &aEnd,
        const row_iterator &rEnd)
{
    for (bool equal = true; equal && aBegin != aEnd && rBegin != rEnd; ) {
        bool changed = false;

        do {    // Skip over identical rows.
            if ((equal = aBegin.isEqual(rBegin, identityWidth))
                    && !(changed = !aBegin.isEqual(rBegin, identityWidth, tableWidth))) {
                ++aBegin;
                ++rBegin;
            } else {
                break;
            }
        } while (rBegin != aEnd && rBegin != rEnd);

        if (changed) {
            row_iterator aIt = aBegin;
            row_iterator rIt = rBegin;

            do {    // Skip over rows with equal IDs but different values.
                if ((equal = aIt.isEqual(rIt, identityWidth))
                        && aIt.isEqual(rIt, identityWidth, tableWidth)) {
                    ++aIt;
                    ++rIt;
                } else {
                    ++aIt;
                    ++rIt;

                    break;
                }
            } while (aIt != aEnd && rIt != rEnd);

            const int aIndex = (aBegin.begin - aCache.values.begin()) / tableWidth;
            const int rIndex = (rBegin.begin - rCache.values.begin()) / tableWidth;
            const int count = (rIt.begin - rBegin.begin) / tableWidth;

            postSyncEvent(SyncEvent::updateEvent(aIndex, rIndex, count));

            aBegin = aIt;
            rBegin = rIt;

            continue;
        } else if (equal) {
            return;
        }

        row_iterator aOuterEnd = aBegin + (((aEnd - aBegin) + 15) & ~15);
        row_iterator rOuterEnd = rBegin + (((rEnd - rBegin) + 15) & ~15);

        row_iterator aInnerEnd = qMin(aBegin + 16, aEnd);
        row_iterator rInnerEnd = qMin(rBegin + 16, rEnd);

        for (row_iterator aOuter = aBegin, rOuter = rBegin;
                !equal && aOuter != aOuterEnd && rOuter != rOuterEnd;
                aOuter += 16, rOuter += 16) {
            for (row_iterator aInner = aBegin, rInner = rBegin;
                    aInner != aInnerEnd && rInner != rInnerEnd;
                    ++aInner, ++rInner) {
                if ((equal = aInner.isEqual(rOuter, identityWidth))) {
                    const int aIndex = (aOuter.begin - aCache.values.begin()) / tableWidth;
                    const int rIndex = (rOuter.begin - rCache.values.begin()) / tableWidth;
                    const int aCount = (aInner.begin - aOuter.begin) / tableWidth;
                    const int rCount = (rOuter.begin - rBegin.begin) / tableWidth;

                    postSyncEvent(SyncEvent::replaceEvent(aIndex, aCount, rIndex, rCount));

                    aBegin = aInner;
                    rBegin = rOuter;

                    break;
                } else if ((equal = rInner.isEqual(aOuter, identityWidth))) {
                    const int aIndex = (aOuter.begin - aCache.values.begin()) / tableWidth;
                    const int rIndex = (rOuter.begin - rCache.values.begin()) / tableWidth;
                    const int aCount = (aOuter.begin - aBegin.begin) / tableWidth;
                    const int rCount = (rInner.begin - rOuter.begin) / tableWidth;

                    postSyncEvent(SyncEvent::replaceEvent(aIndex, aCount, rIndex, rCount));

                    aBegin = aOuter;
                    rBegin = rInner;

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
            rCache.count = rCache.index + event->rCount;
            break;
        case SyncEvent::Update:
            aCache.offset = event->aIndex;
            rCache.cutoff = event->rIndex + event->rCount;

            emit q_func()->metaDataChanged(event->rIndex, event->rCount, QList<int>());

            break;
        case SyncEvent::Replace:
            if (event->aCount > 0) {
                aCache.offset = event->aIndex + event->aCount;
                rCache.cutoff = event->rIndex;

                rowCount -= event->aCount;

                emit q_func()->removed(event->rIndex, event->aCount);
            }
            if (event->rCount > 0) {
                aCache.offset = event->aIndex + event->aCount;
                rCache.cutoff = event->rIndex + event->rCount;

                rowCount += event->rCount;

                q_func()->inserted(event->rIndex, event->rCount);

                break;
            }
            break;
        case SyncEvent::Finish:
            flags |= SyncFinished;
            break;
        default:
            break;
        }

        delete event;
    }
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

    aCache.values.clear();
    aCache.count = 0;

    const int statusIndex = rCache.cutoff;

    rCache.cutoff = rCache.count;

    if (rCache.cutoff > rowCount) {
        const int statusCount = rowCount - statusIndex;

        const int index = rowCount;
        const int count = rCache.cutoff - rowCount;

        rowCount = rCache.cutoff;

        emit q_func()->inserted(index, count);

        if (statusCount > 0)
            emit q_func()->metaDataChanged(statusIndex, statusCount, QList<int>());
    } else {
        const int statusCount = rCache.index - statusIndex;

        if (statusCount > 0)
            emit q_func()->metaDataChanged(statusIndex, statusCount, QList<int>());
    }

    flags &= ~Active;

    q_func()->setCursorPosition(cursorPosition);

    if (flags & Refresh)
        update(rCache.index);
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

    d_func()->update(qMax(0, d->cursorPosition) & ~63);
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

    d_func()->update(qMax(0, d->cursorPosition) & ~63);
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

int QGalleryTrackerItemList::count() const
{
    return d_func()->rowCount;
}

void QGalleryTrackerItemList::setCursorPosition(int position)
{
    Q_D(QGalleryTrackerItemList);

    d->cursorPosition = position;

    if (!(d->flags & (QGalleryTrackerItemListPrivate::Cancelled
            | QGalleryTrackerItemListPrivate::Active))) {
        if (position > d->rCache.index + d->queryLimit - d->minimumPagedItems) {
            d->update(qMax(0, position - d->minimumPagedItems) & ~63);
        } else if (position < d->rCache.index) {
            d->update(qMax(0, position - d->queryLimit + 2 * d->minimumPagedItems) & ~63);
        }
    }

    if (d->rCache.cutoff > 0 && d->imageColumns.count() > 0) {
        typedef QVector<QGalleryTrackerImageColumn *>::const_iterator iterator;

        const int absoluteIndex = qMax(
                d->rCache.index, qMin(d->rCache.count - d->minimumPagedItems, position & ~7));
        const int maximumCount = qMin(d->minimumPagedItems, d->rCache.count - absoluteIndex);
        const int absoluteCount = absoluteIndex + maximumCount;

        if (absoluteIndex < d->imageCacheIndex) {
            QVector<QVariant>::const_iterator begin
                    = d->rCache.values.constBegin()
                    + ((absoluteIndex - d->rCache.index) * d->tableWidth);

            if (absoluteCount > d->imageCacheIndex) {
                const int insertCount = d->imageCacheIndex - absoluteIndex;
                const int removeCount = d->imageCacheCount - absoluteCount;

                for (iterator it = d->imageColumns.begin(); it != d->imageColumns.end(); ++it) {
                    (*it)->removeImages(insertCount, removeCount);
                    (*it)->insertImages(0, insertCount, begin, d->tableWidth);
                    (*it)->moveOffset(absoluteIndex);
                }
            } else {
                for (iterator it = d->imageColumns.begin(); it != d->imageColumns.end(); ++it) {
                    (*it)->removeImages(0, maximumCount);
                    (*it)->insertImages(0, maximumCount, begin, d->tableWidth);
                    (*it)->moveOffset(absoluteIndex);
                }
            }

            d->imageCacheIndex = absoluteIndex;
            d->imageCacheCount = absoluteCount;
        } else if (absoluteCount > d->imageCacheCount) {
            if (absoluteIndex < d->imageCacheCount) {
                const int removeCount = absoluteIndex - d->imageCacheIndex;
                const int insertCount = absoluteCount - d->imageCacheCount;

                QVector<QVariant>::const_iterator begin
                        = d->rCache.values.constBegin()
                        + ((absoluteIndex - d->rCache.index + removeCount) * d->tableWidth);

                for (iterator it = d->imageColumns.begin(); it != d->imageColumns.end(); ++it) {
                    (*it)->removeImages(insertCount, removeCount);
                    (*it)->insertImages(0, insertCount, begin, d->tableWidth);
                    (*it)->moveOffset(absoluteIndex);
                }
            } else {
                QVector<QVariant>::const_iterator begin
                        = d->rCache.values.constBegin()
                        + ((absoluteIndex - d->rCache.index) * d->tableWidth);

                for (iterator it = d->imageColumns.begin(); it != d->imageColumns.end(); ++it) {
                    (*it)->removeImages(0, d->imageCacheCount);
                    (*it)->insertImages(0, maximumCount, begin, d->tableWidth);
                    (*it)->moveOffset(absoluteIndex);
                }
            }

            d->imageCacheIndex = absoluteIndex;
            d->imageCacheCount = absoluteCount;
        }
    }
}

QVariant QGalleryTrackerItemList::id(int index) const
{
    Q_D(const QGalleryTrackerItemList);

    if (index < d->rCache.cutoff) {
        if ((index -= d->rCache.index) >= 0)
            return d->idColumn->value(d->rCache.values.begin() + (index * d->tableWidth));
    } else if (index < d->aCache.count && (index -= d->aCache.offset) >= 0) {
        return d->idColumn->value(d->aCache.values.begin() + (index * d->tableWidth));
    }

    return QVariant();
}

QUrl QGalleryTrackerItemList::url(int index) const
{
    Q_D(const QGalleryTrackerItemList);

    if (index < d->rCache.cutoff) {
        if ((index -= d->rCache.index) >= 0)
            return d->urlColumn->value(d->rCache.values.begin() + (index * d->tableWidth)).toUrl();
    } else if (index < d->aCache.count && (index -= d->aCache.offset) >= 0) {
        return d->urlColumn->value(d->aCache.values.begin() + (index * d->tableWidth)).toUrl();
    }

    return QUrl();
}

QString QGalleryTrackerItemList::type(int index) const
{
    Q_D(const QGalleryTrackerItemList);

    if (index < d->rCache.cutoff) {
        if ((index -= d->rCache.index) >= 0) {
            return d->typeColumn->value(
                    d->rCache.values.begin() + (index * d->tableWidth)).toString();
        }
    } else if (index < d->aCache.count && (index -= d->aCache.offset) >= 0) {
        return d->typeColumn->value(d->aCache.values.begin() + (index * d->tableWidth)).toString();
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

    if (index < d->rCache.cutoff) {
        if (index >= d->rCache.index)
            return status;
    } else {
        if (index >= d->rCache.index && index < d->rCache.count)
            status |= Reading;

        if (index < d->aCache.count && index >= d->aCache.offset)
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
            if (index < d->rCache.cutoff) {
                if ((index -= d->rCache.index) >= 0)
                    return d->rCache.values.at(index * d->tableWidth + key);
            } else if (index < d->aCache.count && (index -= d->aCache.offset) >= 0) {
                return d->aCache.values.at(index * d->tableWidth + key);
            }
        } else if (key < d->aliasOffset) {      // Composite column.
            key -= d->compositeOffset;

            if (index < d->rCache.cutoff) {
                if ((index -= d->rCache.index) >= 0) {
                    return d->compositeColumns.at(key)->value(
                            d->rCache.values.begin() + (index * d->tableWidth));
                }
            } else if (index < d->aCache.count && (index -= d->aCache.offset) >= 0) {
                return d->compositeColumns.at(key)->value(
                        d->aCache.values.begin() + (index * d->tableWidth));
            }
        } else if (key < d->imageOffset) {      // Alias column.
            key = d->aliasColumns.at(key - d->aliasOffset);

            if (index < d->rCache.cutoff) {
                if ((index -= d->rCache.index) >= 0)
                    return d->rCache.values.at(index * d->tableWidth + key);
            } else if (index < d->aCache.count && (index -= d->aCache.offset) >= 0) {
                return d->aCache.values.at(index * d->tableWidth + key);
            }
        } else if (key < d->columnCount
                && index < d->imageCacheCount
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
        if (d->updateTimer.isActive()) {
            d->updateTimer.stop();

            d->update(d->rCache.index);
        } else if (d->queryWatcher) {
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
        } else {
            return true;
        }
    } while ((msecs -= timer.restart()) > 0);

    return false;
}

bool QGalleryTrackerItemList::event(QEvent *event)
{
    switch (event->type()) {

    case QEvent::UpdateRequest: {
            Q_D(QGalleryTrackerItemList);

            d->processSyncEvents();

            typedef QList<QGalleryTrackerMetaDataEdit *>::iterator iterator;
            for (iterator it = d->edits.begin(), end = d->edits.end(); it != end; ++it)
                (*it)->commit();
            d->edits.clear();

            return true;
        }
    case QEvent::Timer: {
            Q_D(QGalleryTrackerItemList);

            QTimerEvent *timerEvent = static_cast<QTimerEvent *>(event);

            if (timerEvent->timerId() == d->updateTimer.timerId()) {
                d->updateTimer.stop();

                d->update(d->rCache.index);
            }
            return true;
        }
    default:
        return QGalleryAbstractResponse::event(event);
    }
}

void QGalleryTrackerItemList::refresh(int updateId)
{
    Q_D(QGalleryTrackerItemList);

    if ((d->updateMask & updateId)
            && !d->updateTimer.isActive()
            && (d->flags & QGalleryTrackerItemListPrivate::Live)) {


        d->flags |= QGalleryTrackerItemListPrivate::Refresh;

        if (!(d->flags &QGalleryTrackerItemListPrivate::Active)) {
            d->updateTimer.start(100, this);
        }
    }
}

#include "moc_qgallerytrackeritemlist_p.cpp"

QTM_END_NAMESPACE
