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

    if (!values.isEmpty()) {
        sortRows(
                row_iterator(values.begin(), tableWidth),
                row_iterator(values.end(), tableWidth),
                sortCriteria.constBegin(),
                sortCriteria.constEnd());
    }

    return values;
}

void QGalleryTrackerItemListPrivate::sortRows(
        row_iterator begin,
        row_iterator end,
        sort_iterator sortCriteria,
        sort_iterator sortEnd,
        bool reversed) const
{
    int column = sortCriteria->column;

    const int sortFlags = sortCriteria->flags;
    const int vColumn = column - valueOffset;

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
    } else if (sortFlags & QGalleryTrackerSortCriteria::Ascending) {
        qStableSort(begin, end, QGalleryTrackerItemListLessThan(
                valueColumns.at(vColumn), column));

        if (++sortCriteria == sortEnd)
            return;
    } else if (sortFlags & QGalleryTrackerSortCriteria::Descending) {
        qStableSort(begin, end, QGalleryTrackerItemListGreaterThan(
                valueColumns.at(vColumn), column));

        if (++sortCriteria == sortEnd)
            return;
    }

    for (row_iterator upper, lower = begin; lower != end; lower = upper) {
        int count = 1;

        for (upper = lower + 1; upper != end && lower[column] == upper[column]; ++upper, ++count) {}

        if (count > 1)
            sortRows(lower, upper, sortCriteria, sortEnd, reversed);
    }
}

void QGalleryTrackerItemListPrivate::synchronize()
{
    const row_iterator aEnd(aCache.values.end(), tableWidth);
    const row_iterator rEnd(rCache.values.end(), tableWidth);

    row_iterator aBegin(aCache.values.begin(), tableWidth);
    row_iterator rBegin(rCache.values.begin(), tableWidth);

    row_iterator aIt = aEnd;
    row_iterator rIt = rEnd;

    bool equal = false;

    for (row_iterator aOuter = aBegin, rOuter = rBegin;     // Brute force search for common items.
            aOuter != aEnd && rOuter != rEnd && !equal;
            ++aOuter, ++rOuter) {
        for (row_iterator aInner = aOuter, rInner = rOuter;
                aInner != aEnd && rInner != rEnd;
                ++aInner, ++rInner) {
            if ((equal = aInner->isEqual(*rOuter, identityWidth))) {
                aIt = aInner;
                rIt = rOuter;

                break;
            } else if ((equal = rInner->isEqual(*aOuter, identityWidth))) {
                aIt = aOuter;
                rIt = rInner;

                break;
            }
        }
    }

    if (equal) {
        int aIndex = aIt - aBegin + aCache.index;
        int rIndex = rIt - rBegin + rCache.index;

        if (aIndex < rIndex) {
            QCoreApplication::postEvent(
                    q_func(), new RowEvent(RowsRemoved, aIndex, rIndex, rIndex - aIndex));
        } else if (rIndex > aIndex) {
            QCoreApplication::postEvent(
                    q_func(), new RowEvent(RowsInserted, aIndex, rIndex, aIndex - rIndex));
        }

        synchronizeRows(aIt, rIt, aEnd, rEnd);
    }
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
            if ((equal = aBegin->isEqual(*rBegin, identityWidth))
                    && !(changed = !aBegin->isEqual(*rBegin, identityWidth, tableWidth))) {
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
                if ((equal = aIt->isEqual(*rIt, identityWidth))
                        && aIt->isEqual(*rIt, identityWidth, tableWidth)) {
                    ++aIt;
                    ++rIt;
                } else {
                    break;
                }
            } while (aIt != aEnd && rIt != rEnd);

            QCoreApplication::postEvent(q, new RowEvent(
                    RowsChanged,
                    (aBegin->begin - aCache.values.begin()) / tableWidth,
                    (rBegin->begin - rCache.values.begin()) / tableWidth,
                    (rIt->begin - rBegin->begin) / tableWidth));

            continue;
        } else if (equal) {
            return;
        }

        for (row_iterator aOuter = aBegin, rOuter = rBegin;
                aOuter != aEnd && rOuter != rEnd;
                ++aOuter, ++rOuter) {
            for (row_iterator aInner = aOuter + 1, rInner = rOuter + 1;
                    aInner != aEnd && rInner != rEnd;
                    ++aInner, ++rInner) {
                if (aInner->isEqual(*rOuter, identityWidth)) {
                    if (rOuter != rBegin) {
                        QCoreApplication::postEvent(q, new RowEvent(
                                RowsInserted,
                                (aBegin->begin - aCache.values.begin()) / tableWidth + aCache.index,
                                (rBegin->begin - rCache.values.begin()) / tableWidth + rCache.index,
                                (rOuter->begin - rBegin->begin) / tableWidth));
                    }

                    QCoreApplication::postEvent(q, new RowEvent(
                            RowsRemoved,
                            (aOuter->begin - aCache.values.begin()) / tableWidth + aCache.index,
                            (aOuter->begin - rCache.values.begin()) / tableWidth + rCache.index,
                            (aInner->begin - aOuter->begin) / tableWidth));

                    aBegin = aInner;
                    rBegin = rOuter;

                    break;
                } else if (rInner->isEqual(*aOuter, identityWidth)) {
                    if (aOuter != aBegin) {
                        QCoreApplication::postEvent(q, new RowEvent(
                                RowsRemoved,
                                (aBegin->begin - aCache.values.begin()) / tableWidth + aCache.index,
                                (rBegin->begin - rCache.values.begin()) / tableWidth + rCache.index,
                                (aOuter->begin - aBegin->begin) / tableWidth));
                    }

                    QCoreApplication::postEvent(q, new RowEvent(
                            RowsInserted,
                            (aOuter->begin - aCache.values.begin()) / tableWidth + aCache.index,
                            (rOuter->begin - rCache.values.begin()) / tableWidth + rCache.index,
                            (rInner->begin - rOuter->begin) / tableWidth));

                    aBegin = aOuter;
                    rBegin = rInner;

                    break;
                }
            }
        }
    }
}

void QGalleryTrackerItemListPrivate::_q_parseFinished()
{
    rCache.values = parseWatcher.result();
    rCache.count = rCache.values.count() / tableWidth;

    if (aCache.count == 0) {
        rCache.cutoff = rCache.count;

        if (rCache.count > 0) {
            rowCount = rCache.index + rCache.count;

            emit q_func()->inserted(0, rowCount);
        }
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

    rCache.cutoff = rCache.index + rCache.count;

    if (rCache.cutoff > rowCount) {
        const int index = rowCount;
        const int count = rCache.cutoff - rowCount;

        rowCount = rCache.cutoff;

        emit q_func()->inserted(index, count);
    }

    q_func()->updateStateChanged(updateState = QGalleryTrackerItemList::UpToDate);
}

QGalleryTrackerItemList::QGalleryTrackerItemList(
        QGalleryTrackerItemListPrivate &dd,
        const QGalleryTrackerSchema &schema,
        int cursorPosition,
        int minimumPagedItems,
        QObject *parent)
    : QGalleryAbstractResponse(dd, parent)
{
    Q_D(QGalleryTrackerItemList);

    d->cursorPosition = cursorPosition;
    d->minimumPagedItems = minimumPagedItems;
    d->identityWidth = schema.identityWidth();
    d->valueOffset = schema.valueOffset();
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

    connect(&d->parseWatcher, SIGNAL(finished()), this, SLOT(_q_parseFinished()));
    connect(&d->synchronizeWatcher, SIGNAL(finished()), this, SLOT(_q_synchronizeFinished()));
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

            d->aCache.offset = rowEvent->aIndex + rowEvent->count;
            d->rCache.cutoff = rowEvent->rIndex + rowEvent->count;

            emit metaDataChanged(rowEvent->rIndex, rowEvent->count, QList<int>());

            return true;
        }
    case QGalleryTrackerItemListPrivate::RowsInserted: {
            Q_D(QGalleryTrackerItemList);

            QGalleryTrackerItemListPrivate::RowEvent *rowEvent
                    = static_cast<QGalleryTrackerItemListPrivate::RowEvent *>(event);

            d->aCache.offset = rowEvent->aIndex;
            d->rCache.cutoff = rowEvent->rIndex + rowEvent->count;

            d->rowCount += rowEvent->count;

            emit inserted(rowEvent->rIndex, rowEvent->count);

            return true;
        }
    case QGalleryTrackerItemListPrivate::RowsRemoved: {
            Q_D(QGalleryTrackerItemList);

            QGalleryTrackerItemListPrivate::RowEvent *rowEvent
                    = static_cast<QGalleryTrackerItemListPrivate::RowEvent *>(event);

            d->aCache.offset = rowEvent->aIndex + rowEvent->count;
            d->rCache.cutoff = rowEvent->rIndex;

            d->rowCount -= rowEvent->count;

            emit removed(rowEvent->rIndex, rowEvent->count);

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
