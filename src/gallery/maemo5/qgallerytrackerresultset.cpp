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

#include "qgallerytrackerresultset_p_p.h"

#include "qgallerytrackermetadataedit_p.h"

#include <QtCore/qdatetime.h>
#include <QtDBus/qdbusreply.h>

#include <qdocumentgallery.h>
#include <qgalleryresource.h>


Q_DECLARE_METATYPE(QVector<QStringList>)

QTM_BEGIN_NAMESPACE

class QGalleryTrackerResultSetParser
{
public:
    QGalleryTrackerResultSetParser(
            QVector<QVariant> &values,
            const QVector<QGalleryTrackerValueColumn *> &valueColumns,
            int valueOffset,
            int tableWidth)
        : values(values)
        , valueColumns(valueColumns)
        , valueOffset(valueOffset)
        , tableWidth(tableWidth)
    {
    }

    QVector<QVariant> &values;
    const QVector<QGalleryTrackerValueColumn *> &valueColumns;
    const int valueOffset;
    const int tableWidth;
};

const QDBusArgument &operator >>(
        const QDBusArgument &argument, QGalleryTrackerResultSetParser &parser)
{
    QString string;
    const QVariant variant;

    argument.beginArray();
    while (!argument.atEnd()) {
        argument.beginArray();

        int i = 0;
        for (; !argument.atEnd() && i < parser.valueOffset; ++i) {
            argument >> string;
            parser.values.append(QVariant(string));
        }
        for (; !argument.atEnd() && i < parser.tableWidth; ++i) {
            argument >> string;
            parser.values.append(parser.valueColumns.at(i - parser.valueOffset)->toVariant(string));
        }
        for (; i < parser.tableWidth; ++i)
            parser.values.append(variant);
        argument.endArray();
    }
    argument.endArray();

    return argument;
}

void QGalleryTrackerResultSetPrivate::update()
{
    flags &= ~UpdateRequested;

    updateTimer.stop();

    typedef QList<QGalleryTrackerMetaDataEdit *>::iterator iterator;
    for (iterator it = edits.begin(), end = edits.end(); it != end; ++it)
        (*it)->commit();
    edits.clear();

    if (!(flags & (Active | Canceled))) {
        query();

        flags &= ~Refresh;
    }
}

void QGalleryTrackerResultSetPrivate::query()
{
    flags &= ~(Refresh | SyncFinished);
    flags |= Active;
    flags |= Reset;

    updateTimer.stop();

    rCache.count = iCache.count;
    rCache.offset = 0;

    iCache.count = 0;
    iCache.cutoff = 0;

    qSwap(rCache.values, iCache.values);

    const int limit = queryLimit < 1 || queryLimit > 1024 ? 1024 : queryLimit;

    QDBusPendingCall call = queryInterface->asyncCallWithArgumentList(
            queryMethod, QVariantList(queryArguments) << queryOffset << limit);

    if (call.isFinished()) {
        queryFinished(call);
    } else {
        queryWatcher.reset(new QDBusPendingCallWatcher(call));

        QObject::connect(
                queryWatcher.data(), SIGNAL(finished(QDBusPendingCallWatcher*)),
                q_func(), SLOT(_q_queryFinished(QDBusPendingCallWatcher*)));

        progressMaximum = 2;

        emit q_func()->progressChanged(0, progressMaximum);
    }
}

void QGalleryTrackerResultSetPrivate::_q_queryFinished(QDBusPendingCallWatcher *watcher)
{
    if (queryWatcher.data() == watcher) {
        queryWatcher.take()->deleteLater();

        queryFinished(*watcher);
    }
}

void QGalleryTrackerResultSetPrivate::queryFinished(const QDBusPendingCall &call)
{
    if (call.isError()) {
        emit q_func()->progressChanged(progressMaximum, progressMaximum);

        qWarning("DBUS error %s", qPrintable(call.error().message()));

        flags &= ~Active;

        q_func()->finish(QDocumentGallery::ConnectionError);
    } else if (flags & Canceled) {
        iCache.count = 0;

        flags &= ~Active;

        q_func()->QGalleryAbstractResponse::cancel();
    } else {
        parserLimit = queryLimit < 1 ? 1023 : queryLimit - iCache.count;
        parserReset = flags & Reset;
        queryReply = call.reply().arguments().at(0).value<QDBusArgument>();

        flags &= ~Reset;

        parserThread.start(QThread::LowPriority);

        emit q_func()->progressChanged(progressMaximum - 1, progressMaximum);
    }
}

void QGalleryTrackerResultSetPrivate::run()
{
    if (parserReset) {
        iCache.values.clear();
        iCache.count = 0;
    }

    const int previousCount = iCache.count;

    QGalleryTrackerResultSetParser parser(iCache.values, valueColumns, valueOffset, tableWidth);

    queryReply >> parser;

    iCache.count += iCache.values.count() / tableWidth;

    if (previousCount - iCache.count <= parserLimit) {
        if (!iCache.values.isEmpty() && !sortCriteria.isEmpty()) {
            correctRows(
                    row_iterator(iCache.values.begin(), tableWidth),
                    row_iterator(iCache.values.end(), tableWidth),
                    sortCriteria.constBegin(),
                    sortCriteria.constEnd());
        }

        synchronize();

        parserReset = true;
    } else {
        parserReset = false;
    }
}

void QGalleryTrackerResultSetPrivate::correctRows(
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

void QGalleryTrackerResultSetPrivate::synchronize()
{
    const const_row_iterator rEnd(rCache.values.constEnd(), tableWidth);
    const const_row_iterator iEnd(iCache.values.constEnd(), tableWidth);

    const_row_iterator rBegin(rCache.values.constBegin(), tableWidth);
    const_row_iterator iBegin(iCache.values.constBegin(), tableWidth);

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
            const_row_iterator rIt = rBegin;
            const_row_iterator iIt = iBegin;

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
            postSyncEvent(SyncEvent::finishEvent(rCacheIndex(rBegin), iCacheIndex(iBegin)));

            return;
        }

        const_row_iterator rOuterEnd = rBegin + ((((rEnd - rBegin) + iStep - 1) / rStep) * rStep);
        const_row_iterator iOuterEnd = iBegin + ((((iEnd - iBegin) + iStep - 1) / iStep) * iStep);

        const_row_iterator rInnerEnd = qMin(rBegin + rStep * 2, rEnd);
        const_row_iterator iInnerEnd = qMin(iBegin + iStep * 2, iEnd);

        for (const_row_iterator rOuter = rBegin, iOuter = iBegin;
                !equal && rOuter != rOuterEnd && iOuter != iOuterEnd;
                rOuter += rStep, iOuter += iStep) {
            for (const_row_iterator rInner = rBegin, iInner = iBegin;
                    rInner != rInnerEnd && iInner != iInnerEnd;
                    ++rInner, ++iInner) {
                if ((equal = rInner.isEqual(iOuter, identityWidth))) {
                    const_row_iterator rIt;
                    const_row_iterator iIt;

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
                    const_row_iterator rIt;
                    const_row_iterator iIt;

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

    postSyncEvent(SyncEvent::finishEvent(rCacheIndex(rBegin), iCacheIndex(iBegin)));
}

void QGalleryTrackerResultSetPrivate::processSyncEvents()
{
    while (SyncEvent *event = syncEvents.dequeue()) {
        switch (event->type) {
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

void QGalleryTrackerResultSetPrivate::removeItems(
        const int rIndex, const int iIndex, const int count)
{
    const int originalIndex = currentIndex;

    rCache.offset = rIndex + count;
    iCache.cutoff = iIndex;

    if (currentIndex >= iIndex && currentIndex < rCache.offset) {
        currentIndex = iIndex;

        if (currentIndex < rCache.count) {
            currentRow = rCache.values.constBegin()
                    + ((currentIndex + rCache.offset - iCache.cutoff) * tableWidth);
        } else {
            currentRow = 0;
        }
    }

    rowCount -= count;

    emit q_func()->itemsRemoved(iIndex, count);

    if (originalIndex != currentIndex) {
        emit q_func()->currentIndexChanged(currentIndex);
        emit q_func()->currentItemChanged();
    }
}

void QGalleryTrackerResultSetPrivate::insertItems(
        const int rIndex, const int iIndex, const int count)
{
    rCache.offset = rIndex;
    iCache.cutoff = iIndex + count;

    rowCount += count;

    emit q_func()->itemsInserted(iIndex, count);
}

void QGalleryTrackerResultSetPrivate::syncUpdate(
        const int rIndex, const int rCount, const int iIndex, const int iCount)
{
    bool itemChanged = false;

    if (currentIndex >= iCache.cutoff && currentIndex < iCache.cutoff + iCount) {
        currentRow = iCache.values.constBegin() + (currentIndex * tableWidth);

        itemChanged = true;
    }
    rCache.offset = rIndex + rCount;
    iCache.cutoff = iIndex + iCount;

    emit q_func()->metaDataChanged(iIndex, iCount, propertyKeys);

    if (itemChanged)
        emit q_func()->currentItemChanged();
}

void QGalleryTrackerResultSetPrivate::syncReplace(
        const int rIndex, const int rCount, const int iIndex, const int iCount)
{
    bool itemChanged = false;

    if (rCount > 0)
        removeItems(rIndex, iIndex, rCount);

    if (currentIndex >= iCache.cutoff && currentIndex < iCache.cutoff + iCount) {
        currentRow = iCache.values.constBegin() + (currentIndex * tableWidth);

        itemChanged = true;
    }

    if (iCount > 0)
        insertItems(rIndex + rCount, iIndex, iCount);

    if (itemChanged)
        emit q_func()->currentItemChanged();
}

void QGalleryTrackerResultSetPrivate::syncFinish(const int rIndex, const int iIndex)
{
    const int rCount = rCache.count - rIndex;
    const int iCount = iCache.count - iIndex;

    bool itemChanged = false;

    if (rCount > 0)
        removeItems(rIndex, iIndex, rCount);
    else
        rCache.offset = rCache.count;

    if (currentIndex >= iCache.cutoff && currentIndex < iCache.count) {
        currentRow = iCache.values.constBegin() + (currentIndex * tableWidth);

        itemChanged = true;
    }

    if (iCount > 0)
        insertItems(rIndex + rCount, iIndex, iCount);
    else
        iCache.cutoff = iCache.count;

    if (itemChanged)
        emit q_func()->currentItemChanged();

    flags |= SyncFinished;
}

bool QGalleryTrackerResultSetPrivate::waitForSyncFinish(int msecs)
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

void QGalleryTrackerResultSetPrivate::_q_parseFinished()
{
    processSyncEvents();

    queryReply = QDBusArgument();

    if (parserReset) {
        Q_ASSERT(rCache.offset == rCache.count);
        Q_ASSERT(iCache.cutoff == iCache.count);

        rCache.values.clear();
        rCache.count = 0;

        flags &= ~Active;

        if (flags & Refresh)
            update();
        else
            emit q_func()->progressChanged(progressMaximum, progressMaximum);

        q_func()->finish(flags & Live);
    } else if (flags & Canceled) {
        iCache.count = 0;

        flags &= ~Active;

        q_func()->QGalleryAbstractResponse::cancel();
    } else {
        const int offset = queryOffset + iCache.count;
        const int limit = queryLimit < 1 || queryLimit - iCache.count > 1024
                ? 1024
                : queryLimit - iCache.count;

        QDBusPendingCall call = queryInterface->asyncCallWithArgumentList(
                queryMethod, QVariantList(queryArguments) << offset << limit);

        if (call.isFinished()) {
            queryFinished(call);
        } else {
            queryWatcher.reset(new QDBusPendingCallWatcher(call));

            QObject::connect(
                    queryWatcher.data(), SIGNAL(finished(QDBusPendingCallWatcher*)),
                    q_func(), SLOT(_q_queryFinished(QDBusPendingCallWatcher*)));

            progressMaximum += 2;

            emit q_func()->progressChanged(progressMaximum - 2, progressMaximum);
        }
    }
}

void QGalleryTrackerResultSetPrivate::_q_editFinished(QGalleryTrackerMetaDataEdit *edit)
{
    edit->deleteLater();

    emit q_func()->itemEdited(edit->service());
}

QGalleryTrackerResultSet::QGalleryTrackerResultSet(
        QGalleryTrackerResultSetArguments *arguments,
        bool autoUpdate,
        int cursorPosition,
        int minimumPagedItems,
        QObject *parent)
    : QGalleryResultSet(
            *new QGalleryTrackerResultSetPrivate(
                    arguments, autoUpdate, cursorPosition, minimumPagedItems),
            parent)
{
    Q_D(QGalleryTrackerResultSet);

    connect(&d->parserThread, SIGNAL(finished()), this, SLOT(_q_parseFinished()));

    d_func()->query();
}

QGalleryTrackerResultSet::QGalleryTrackerResultSet(
        QGalleryTrackerResultSetPrivate &dd,
        QObject *parent)
    : QGalleryResultSet(dd, parent)
{
    Q_D(QGalleryTrackerResultSet);

    connect(&d->parserThread, SIGNAL(finished()), this, SLOT(_q_parseFinished()));

    d_func()->query();
}

QGalleryTrackerResultSet::~QGalleryTrackerResultSet()
{
    Q_D(QGalleryTrackerResultSet);

    typedef QList<QGalleryTrackerMetaDataEdit *>::iterator iterator;
    for (iterator it = d->edits.begin(), end = d->edits.end(); it != end; ++it)
        (*it)->commit();

    d->parserThread.wait();
}

QStringList QGalleryTrackerResultSet::propertyNames() const
{
    return d_func()->propertyNames;
}

int QGalleryTrackerResultSet::propertyKey(const QString &property) const
{
    Q_D(const QGalleryTrackerResultSet);

    int index = d->propertyNames.indexOf(property);

    return index >= 0
            ? index + d->valueOffset
            : -1;
}

QGalleryProperty::Attributes QGalleryTrackerResultSet::propertyAttributes(int key) const
{
    return d_func()->propertyAttributes.value(key - d_func()->valueOffset);
}

QVariant::Type QGalleryTrackerResultSet::propertyType(int key) const
{
    return d_func()->propertyTypes.value(key - d_func()->valueOffset);
}

int QGalleryTrackerResultSet::itemCount() const
{
    return d_func()->rowCount;
}

int QGalleryTrackerResultSet::currentIndex() const
{
    return d_func()->currentIndex;
}

bool QGalleryTrackerResultSet::fetch(int index)
{
    Q_D(QGalleryTrackerResultSet);

    d->currentIndex = index;

    if (d->currentIndex < 0 || d->currentIndex >= d->rowCount) {
        d->currentRow = 0;
    } else if (d->currentIndex < d->iCache.cutoff) {
        d->currentRow = d->iCache.values.constBegin() + (d->currentIndex * d->tableWidth);
    } else {
        d->currentRow
                = d->rCache.values.constBegin()
                + ((d->currentIndex + d->rCache.offset - d->iCache.cutoff) * d->tableWidth);
    }

    emit currentIndexChanged(d->currentIndex);
    emit currentItemChanged();

    return d->currentRow != 0;
}

QVariant QGalleryTrackerResultSet::itemId() const
{
    Q_D(const QGalleryTrackerResultSet);

    return d->currentRow
            ? d->idColumn->value(d->currentRow)
            : QVariant();
}

QUrl QGalleryTrackerResultSet::itemUrl() const
{
    Q_D(const QGalleryTrackerResultSet);

    return d->currentRow
            ? d->urlColumn->value(d->currentRow).toUrl()
            : QUrl();
}

QString QGalleryTrackerResultSet::itemType() const
{
    Q_D(const QGalleryTrackerResultSet);

    return d->currentRow
            ? d->typeColumn->value(d->currentRow).toString()
            : QString();
}

QList<QGalleryResource> QGalleryTrackerResultSet::resources() const
{
    Q_D(const QGalleryTrackerResultSet);

    QList<QGalleryResource> resources;

    if (d->currentRow) {
        const QUrl url = d->urlColumn->value(d->currentRow).toUrl();

        if (!url.isEmpty()) {
            QMap<int, QVariant> attributes;

            typedef QVector<int>::const_iterator iterator;
            for (iterator it = d->resourceKeys.begin(), end = d->resourceKeys.end();
                    it != end;
                    ++it) {
                QVariant value = metaData(*it);

                if (!value.isNull())
                    attributes.insert(*it, value);
            }

            resources.append(QGalleryResource(url, attributes));
        }
    }
    return resources;
}

QVariant QGalleryTrackerResultSet::metaData(int key) const
{
    Q_D(const QGalleryTrackerResultSet);

    if (!d->currentRow || key < d->valueOffset) {
        return QVariant();
    } else if (key < d->compositeOffset) {  // Value column.
        return *(d->currentRow + key);
    } else if (key < d->aliasOffset) {      // Composite column.
        return d->compositeColumns.at(key - d->compositeOffset)->value(d->currentRow);
    } else if (key < d->columnCount) {      // Alias column.
        return *(d->currentRow + d->aliasColumns.at(key - d->aliasOffset));
    } else {
        return QVariant();
    }
}

bool QGalleryTrackerResultSet::setMetaData(int, const QVariant &)
{
    return false;
}

void QGalleryTrackerResultSet::cancel()
{
    d_func()->flags |= QGalleryTrackerResultSetPrivate::Canceled;
    d_func()->flags &= ~QGalleryTrackerResultSetPrivate::Live;

    if (!(d_func()->flags &QGalleryTrackerResultSetPrivate::Active))
        QGalleryAbstractResponse::cancel();
}

bool QGalleryTrackerResultSet::waitForFinished(int msecs)
{
    Q_D(QGalleryTrackerResultSet);

    QTime timer;
    timer.start();

    do {
        if (d->queryWatcher) {
            QScopedPointer<QDBusPendingCallWatcher> watcher(d_func()->queryWatcher.take());

            watcher->waitForFinished();

            d->queryFinished(*watcher);

            if (!(d->flags & QGalleryTrackerResultSetPrivate::Active))
                return true;
        } else if (d->flags & QGalleryTrackerResultSetPrivate::Active) {
            if (d->waitForSyncFinish(msecs)) {
                d->parserThread.wait();

                d->_q_parseFinished();

                if (!(d->flags & QGalleryTrackerResultSetPrivate::Active))
                    return true;
            } else {
                return false;
            }
        } else if (d->flags & (QGalleryTrackerResultSetPrivate::Refresh)) {
            d->update();
        } else {
            return true;
        }
    } while ((msecs -= timer.restart()) > 0);

    return false;
}

bool QGalleryTrackerResultSet::event(QEvent *event)
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

void QGalleryTrackerResultSet::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == d_func()->updateTimer.timerId()) {
        d_func()->update();

        event->accept();
   }
}

void QGalleryTrackerResultSet::refresh(int updateId)
{
    Q_D(QGalleryTrackerResultSet);

    if ((d->updateMask & updateId)
            && !d->updateTimer.isActive()
            && (d->flags & QGalleryTrackerResultSetPrivate::Live)) {


        d->flags |= QGalleryTrackerResultSetPrivate::Refresh;

        if (!(d->flags & QGalleryTrackerResultSetPrivate::Active)) {
            d->updateTimer.start(100, this);
        }
    }
}

#include "moc_qgallerytrackerresultset_p.cpp"

QTM_END_NAMESPACE
