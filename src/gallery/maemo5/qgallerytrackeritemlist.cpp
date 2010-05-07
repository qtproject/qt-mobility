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

#include "qgallerytrackeritemlist_p_p.h"

#include <QtCore/qcoreapplication.h>
#include <QtCore/qtconcurrentrun.h>

QTM_BEGIN_NAMESPACE

QVector<QVariant> QGalleryTrackerItemListPrivate::parseResultSet(
        const QVector<QStringList> &resultSet) const
{
    typedef QVector<QStringList>::const_iterator iterator;

    QVector<QVariant> values;
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

    sortRows(
            row_iterator(values.begin(), tableWidth),
            row_iterator(values.end(), tableWidth),
            sortCriteria.constBegin(),
            sortCriteria.constEnd());

    return values;
}

void QGalleryTrackerItemListPrivate::sortRows(
        row_iterator begin,
        row_iterator end,
        sort_iterator sortCriteria,
        sort_iterator sortEnd,
        bool reversed) const
{
    int column;

    for (;;) {
        if (sortCriteria == sortEnd)
            return;

        column = sortCriteria->column;

        const int sortFlags = sortCriteria->flags;
        const int vColumn = column - valueOffset;

        ++sortCriteria;

        if (sortFlags & QGalleryTrackerSortCriteria::Sorted) {
            if (reversed) {
                QAlgorithmsPrivate::qReverse(begin, end);

                reversed = false;

                break;
            }
        } else if (sortFlags & QGalleryTrackerSortCriteria::ReverseSorted) {
            if (!reversed) {
                QAlgorithmsPrivate::qReverse(begin, end);

                reversed = true;

                break;
            }
        } else if (sortFlags & QGalleryTrackerSortCriteria::Ascending) {
            qStableSort(begin, end, QGalleryTrackerItemListLessThan(
                    valueColumns.at(vColumn), column));
            break;
        } else if (sortFlags & QGalleryTrackerSortCriteria::Descending) {
            qStableSort(begin, end, QGalleryTrackerItemListGreaterThan(
                    valueColumns.at(vColumn), column));
            break;
        }
    }

    for (row_iterator upper, lower = begin; lower != end; lower = upper) {
        int count = 1;

        for (upper = ++lower; upper != end && lower[column] != upper[column]; ++upper, ++count) {}

        if (count > 1)
            sortRows(lower, upper, sortCriteria, sortEnd, reversed);
    }
}

void QGalleryTrackerItemListPrivate::synchronize()
{
    const row_iterator aEnd(aCache.values.end(), tableWidth);
    const row_iterator rEnd(rCache.values.end(), tableWidth);

    row_iterator aIt(aCache.values.begin(), tableWidth);
    row_iterator rIt(rCache.values.begin(), tableWidth);

    synchronizeRows(aIt, rIt, aEnd, rEnd);

    if (aIt != aEnd)
        QCoreApplication::postEvent(q_func(), new RowEvent(RowsChanged, *aIt, *rIt, *aEnd));

    if (rIt != rEnd)
        QCoreApplication::postEvent(q_func(), new RowEvent(RowsChanged, *aIt, *rIt, *rEnd));
}

void QGalleryTrackerItemListPrivate::synchronizeRows(
        row_iterator &aBegin,
        row_iterator &rBegin,
        const row_iterator &aEnd,
        const row_iterator &rEnd)
{
    Q_Q(QGalleryTrackerItemList);

    for (bool equal = true; equal && aBegin != aEnd && rBegin != rEnd; ) {
        bool changed = false;

        do {    // Skip over identical rows.
            if ((equal = aBegin->isEqual(*rBegin, valueOffset))
                    && !(changed = !aBegin->isEqual(*rBegin, valueOffset, tableWidth))) {
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
                if ((equal = aIt->isEqual(*rIt, valueOffset))
                        && aIt->isEqual(*rIt, valueOffset, tableWidth)) {
                    ++aIt;
                    ++rIt;
                } else {
                    break;
                }
            } while (aIt != aEnd && rIt != rEnd);

            QCoreApplication::postEvent(q, new RowEvent(RowsChanged, *aBegin, *rBegin, *rIt));

            continue;
        } else if (equal) {
            return;
        }

        for (row_iterator aIt = aBegin + 1, rIt = rBegin + 1;   // Scan for rows with common IDs.
                aIt != aEnd && rIt != rEnd;
                ++aIt, ++rIt) {
            if ((equal = aIt->isEqual(*rBegin, valueOffset))) {
                QCoreApplication::postEvent(q, new RowEvent(RowsRemoved, *aBegin, *rBegin, *aIt));

                aBegin = aIt;
                break;
            } else if ((equal = rIt->isEqual(*aBegin, valueOffset))) {
                QCoreApplication::postEvent(q, new RowEvent(RowsInserted, *aBegin, *rBegin, *rIt));

                rBegin = rIt;
                break;
            }
        }
    }
}

void QGalleryTrackerItemListPrivate::rowsChanged(int, int rIndex, int count)
{
    emit q_func()->metaDataChanged(rCache.index + rIndex, count, QList<int>());
}

void QGalleryTrackerItemListPrivate::rowsInserted(int aIndex, int rIndex, int count)
{
    aCache.offset = aCache.index + aIndex;
    rCache.cutoff = rCache.index + rIndex + count;

    rowCount += count;

    emit q_func()->inserted(rCache.index + rIndex, count);
}

void QGalleryTrackerItemListPrivate::rowsRemoved(int aIndex, int rIndex, int count)
{
    aCache.offset = aCache.index + aIndex + count;
    rCache.cutoff = rCache.index + rIndex;

    rowCount -= count;

    emit q_func()->removed(rCache.index + rIndex, count);
}

void QGalleryTrackerItemListPrivate::_q_parseFinished()
{
    rCache.values = parseWatcher.result();
    rCache.count = rCache.values.count();

    if (aCache.count == 0) {
        rowCount = rCache.index + rCache.count;

        emit q_func()->inserted(0, rowCount);

        q_func()->updateStateChanged(updateState = QGalleryTrackerItemList::UpToDate);
    } else {
        synchronizeWatcher.setFuture(
                QtConcurrent::run(this, &QGalleryTrackerItemListPrivate::synchronize));
    }
}

void QGalleryTrackerItemListPrivate::_q_synchronizeFinished()
{
    aCache.values.clear();
    aCache.count = 0;

    q_func()->updateStateChanged(updateState = QGalleryTrackerItemList::UpToDate);
}

QGalleryTrackerItemList::QGalleryTrackerItemList(
        QGalleryTrackerItemListPrivate &dd,
        const QGalleryTrackerSchema &schema,
        int cursorPosition,
        int minimumPagedItems,
        int valueOffset,
        QObject *parent)
    : QGalleryAbstractResponse(dd, parent)
{
    Q_D(QGalleryTrackerItemList);

    d->cursorPosition = cursorPosition;
    d->minimumPagedItems = minimumPagedItems;
    d->valueOffset = valueOffset;
    d->idColumn = schema.createIdColumn();
    d->urlColumn = schema.createUrlColumn();
    d->typeColumn = schema.createTypeColumn();
    d->valueColumns = schema.createValueColumns();
    d->compositeColumns = schema.createCompositeColumns();
    d->aliasColumns = schema.createAliasColumns();
    d->imageColumns = schema.createImageColumns();
    d->sortCriteria = schema.sortCriteria();
    d->propertyNames = schema.propertyNames();
    d->propertyAttributes = schema.propertyAttributes();

    d->compositeOffset = d->valueOffset + d->valueColumns.count();
    d->aliasOffset = d->tableWidth + d->compositeColumns.count();
    d->imageOffset = d->aliasOffset + d->aliasColumns.count();
    d->columnCount = d->imageOffset + d->imageColumns.count();

    d->rowCount = 0;
    d->updateState = UpToDate;
}

QGalleryTrackerItemList::~QGalleryTrackerItemList()
{
}

QGalleryTrackerItemList::UpdateState QGalleryTrackerItemList::updateState() const
{
    return d_func()->updateState;
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
    return d_func()->propertyAttributes.value(key);
}

int QGalleryTrackerItemList::count() const
{
    return d_func()->rowCount;
}

QVariant QGalleryTrackerItemList::id(int index) const
{
    Q_D(const QGalleryTrackerItemList);

    if (index < d->rCache.cutoff) {
        index -= d->rCache.index;

        return index >= 0 && index <= d->rCache.count
                ? d->idColumn->value(d->rCache.values.constBegin() + (index * d->tableWidth))
                : QVariant();
    } else {
        index -= d->aCache.cutoff;

        return index >= 0 && index <= d->aCache.count
                ? d->idColumn->value(d->aCache.values.constBegin() + (index * d->tableWidth))
                : QVariant();
    }
}

QUrl QGalleryTrackerItemList::url(int index) const
{
    Q_D(const QGalleryTrackerItemList);

    if (index < d->rCache.cutoff) {
        index -= d->rCache.index;

        return index >= 0
                ? d->urlColumn->value(
                        d->rCache.values.constBegin() + (index * d->tableWidth)).toUrl()
                : QUrl();
    } else {
        index -= d->aCache.cutoff;

        return index >= 0 && index <= d->aCache.count
                ? d->urlColumn->value(
                        d->aCache.values.constBegin() + (index * d->tableWidth)).toUrl()
                : QUrl();
    }
}

QString QGalleryTrackerItemList::type(int index) const
{
    Q_D(const QGalleryTrackerItemList);

    if (index < d->rCache.cutoff) {
        index -= d->rCache.index;

        return index >= 0
                ? d->typeColumn->value(
                        d->rCache.values.constBegin() + (index * d->tableWidth)).toString()
                : QString();
    } else {
        index -= d->aCache.cutoff;

        return index >= 0 && index <= d->aCache.count
                ? d->typeColumn->value(
                        d->aCache.values.constBegin() + (index * d->tableWidth)).toString()
                : QString();
    }
}

QList<QGalleryResource> QGalleryTrackerItemList::resources(int index) const
{
    Q_UNUSED(index);

    return QList<QGalleryResource>();
}

QGalleryItemList::ItemStatus QGalleryTrackerItemList::status(int index) const
{
    Q_UNUSED(index);

    return QGalleryItemList::ItemStatus();
}

QVariant QGalleryTrackerItemList::metaData(int index, int key) const
{
    Q_D(const QGalleryTrackerItemList);

    if (index < d->rCache.cutoff) {
        index -= d->rCache.index;

        if (index >=0 && key >= d->valueOffset) {
            if (key < d->compositeOffset) {         // Value column.
                key -= d->valueOffset;

                return d->rCache.values.at((index * d->tableWidth) + key);
            } else if (key < d->aliasOffset) {      // Composite column.
                key -= d->compositeOffset;

                return d->compositeColumns.at(key)->value(
                        d->rCache.values.constBegin() + (index * d->tableWidth));
            } else if (key < d->imageOffset) {      // Alias column.
                key = d->aliasColumns.at(key - d->aliasOffset)->aliasedKey();

                return d->rCache.values.at((index * d->tableWidth) + key);
            } else if (key < d->columnCount) {      // Image column.
                key -= d->imageOffset;

                // TBD

                return QVariant();
            }
        }
    } else {
        index -= d->aCache.offset;

        if (index >= 0 && index <= d->aCache.count && key >= d->valueOffset) {
            if (key < d->compositeOffset) {         // Value column.
                key -= d->valueOffset;

                return d->aCache.values.at((index * d->tableWidth) + key);
            } else if (key < d->aliasOffset) {      // Composite column.
                key -= d->compositeOffset;

                return d->compositeColumns.at(key)->value(
                        d->aCache.values.constBegin() + (index * d->tableWidth));
            } else if (key < d->imageOffset) {      // Alias column.
                key = d->aliasColumns.at(key - d->aliasOffset)->aliasedKey();

                return d->aCache.values.at((index * d->tableWidth) + key);
            } else if (key < d->columnCount) {      // Image column.
                key -= d->imageOffset;

                // TBD

                return QVariant();
            }
        }
    }

    return QVariant();
}

void QGalleryTrackerItemList::setMetaData(int, int, const QVariant &)
{

}

bool QGalleryTrackerItemList::event(QEvent *event)
{
    switch (event->type()) {
    case QGalleryTrackerItemListPrivate::RowsChanged: {
            Q_D(QGalleryTrackerItemList);

            QGalleryTrackerItemListPrivate::RowEvent *rowEvent
                    = static_cast<QGalleryTrackerItemListPrivate::RowEvent *>(event);

            const int count = (rowEvent->endRow.begin - rowEvent->rRow.begin) / d->tableWidth;

            const int aIndex = (rowEvent->aRow.begin - d->aCache.values.begin()) / d->tableWidth;
            const int rIndex = (rowEvent->rRow.begin - d->rCache.values.begin()) / d->tableWidth;

            d->rowsChanged(aIndex, rIndex, count);

            return true;
        }
    case QGalleryTrackerItemListPrivate::RowsInserted: {
            Q_D(QGalleryTrackerItemList);

            QGalleryTrackerItemListPrivate::RowEvent *rowEvent
                    = static_cast<QGalleryTrackerItemListPrivate::RowEvent *>(event);

            const int count = (rowEvent->endRow.begin - rowEvent->rRow.begin) / d->tableWidth;

            const int aIndex = (rowEvent->aRow.begin - d->aCache.values.begin()) / d->tableWidth;
            const int rIndex = (rowEvent->rRow.begin - d->rCache.values.begin()) / d->tableWidth;

            d->rowsInserted(aIndex, rIndex, count);

            return true;
        }
    case QGalleryTrackerItemListPrivate ::RowsRemoved: {
            Q_D(QGalleryTrackerItemList);

            QGalleryTrackerItemListPrivate::RowEvent *rowEvent
                    = static_cast<QGalleryTrackerItemListPrivate::RowEvent *>(event);

            const int count = (rowEvent->endRow.begin - rowEvent->aRow.begin) / d->tableWidth;

            const int aIndex = (rowEvent->aRow.begin - d->aCache.values.begin()) / d->tableWidth;
            const int rIndex = (rowEvent->rRow.begin - d->rCache.values.begin()) / d->tableWidth;

            d->rowsRemoved(aIndex, rIndex, count);

            return true;
        }
    default:
        return QGalleryAbstractResponse::event(event);
    }
}

void QGalleryTrackerItemList::updateResultSet(const QVector<QStringList> &resultSet, int index)
{
    Q_D(QGalleryTrackerItemList);

    Q_ASSERT(d->updateState == UpToDate);

    updateStateChanged(d->updateState = Updating);

    d->rCache.cutoff = 0;

    d->aCache = d->rCache;

    d->rCache.index = index;
    d->rCache.count = 0;
    d->rCache.values.clear();

    d->parseWatcher.setFuture(
            QtConcurrent::run(d, &QGalleryTrackerItemListPrivate::parseResultSet, resultSet));
}

#include "moc_qgallerytrackeritemlist_p.cpp"

QTM_END_NAMESPACE
