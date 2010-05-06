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

#include "qgallerytrackeritemlist_p.h"

#include "qgalleryabstractresponse_p.h"

#include <QtCore/qcoreapplication.h>
#include <QtCore/qcoreevent.h>
#include <QtCore/qfuturewatcher.h>
#include <QtCore/qtconcurrentrun.h>

QTM_BEGIN_NAMESPACE

class QGalleryTrackerItemListPrivate : public QGalleryAbstractResponsePrivate
{
    Q_DECLARE_PUBLIC(QGalleryTrackerItemList)
public:
    struct Row
    {
        Row() {}
        Row(QVector<QVariant>::iterator begin, QVector<QVariant>::iterator end)
            : begin(begin), end(end) {}

        int width() const { return end - begin; }

        Row &operator +=(int span) { begin += span; end += span; return *this; }
        Row &operator -=(int span) { begin -= span; end -= span; return *this; }

        bool isEqual(const Row &row, int count) const {
            return qEqual(begin, row.begin, begin + count); }
        bool isEqual(const Row &row, int index, int count) {
            return qEqual(begin + index, row.begin + index, begin + count); }

        QVector<QVariant>::iterator begin;
        QVector<QVariant>::iterator end;
    };

    struct row_iterator
    {
        row_iterator() {}
        row_iterator(QVector<QVariant>::iterator begin, QVector<QVariant>::iterator end)
            : row(begin, end) {}
        row_iterator(QVector<QVariant>::iterator begin, int width)
            : row(begin, begin + width) {}
        row_iterator(const Row &row) : row(row) {}

        bool operator != (const row_iterator &other) const { return row.begin != other.row.begin; }
        bool operator <(const row_iterator &other) const { return row.begin < other.row.begin; }

        row_iterator &operator ++() { row += row.width(); return *this; }
        row_iterator &operator --() { row -= row.width(); return *this; }

        row_iterator operator ++(int) { row_iterator n(row); row += row.width(); return n; }
        row_iterator operator --(int) { row_iterator n(row); row -= row.width(); return n; }

        int operator -(const row_iterator &other) const {
            return (row.begin - other.row.begin) / row.width(); }

        row_iterator operator +(int span) const {
            span *= row.width(); return row_iterator(row.begin + span, row.end + span); }

        Row &operator *() {  return row; }
        const Row &operator *() const {  return row; }

        Row *operator ->() { return &row; }
        const Row *operator ->() const { return &row; }

        QVariant operator[] (int column) const { return *(row.begin + column); }

        Row row;
    };

    enum RowEventType
    {
        RowsChanged = QEvent::User,
        RowsInserted,
        RowsRemoved
    };

    class RowEvent : public QEvent
    {
    public:
        RowEvent(RowEventType type, const Row &aRow, const Row &rRow, const Row &endRow)
            : QEvent(QEvent::Type(type))
            , aRow(aRow)
            , rRow(rRow)
            , endRow(endRow)
        {
        }

        const Row aRow;
        const Row rRow;
        const Row endRow;
    };

    struct Cache
    {
        Cache() : index(0), count(0), cutoff(0) {}

        int index;
        int count;
        union
        {
            int offset;
            int cutoff;
        };
        QVector<QVariant> values;
    };

    QGalleryTrackerCompositeColumn *idColumn;
    QGalleryTrackerCompositeColumn *urlColumn;
    QGalleryTrackerCompositeColumn *typeColumn;

    int valueOffset;
    union
    {
        int compositeOffset;
        int tableWidth;
    };
    int aliasOffset;
    int imageOffset;
    int columnCount;
    int rowCount;
    QStringList propertyNames;
    QVector<QGalleryProperty::Attributes> propertyAttributes;
    QVector<QGalleryTrackerValueColumn *> valueColumns;
    QVector<QGalleryTrackerCompositeColumn *> compositeColumns;
    QVector<QGalleryTrackerAliasColumn *> aliasColumns;
    QVector<QGalleryTrackerImageColumn *> imageColumns;
    Cache aCache;   // Access cache.
    Cache rCache;   // Replacement cache.

    QFutureWatcher<QVector<QVariant> > parseWatcher;
    QFutureWatcher<void> synchronizeWatcher;

    QVector<QVariant> parseResultSet(const QVector<QStringList> &resultSet) const;
    void sortRows(row_iterator begin, row_iterator end, int column, bool reversed = false) const;

    void synchronize();
    void synchronizeRows(
            row_iterator &aBegin,
            row_iterator &rBegin,
            const row_iterator &aEnd,
            const row_iterator &rEnd);
    void rowsChanged(int aIndex, int rIndex, int count);
    void rowsInserted(int aIndex, int rIndex, int count);
    void rowsRemoved(int aIndex, int rIndex, int count);

    void _q_parseFinished();
    void _q_synchronizeFinished();
};

class QGalleryTrackerItemListLessThan
{
public:
    QGalleryTrackerItemListLessThan(const QGalleryTrackerValueColumn *column, int index)
        : m_column(column), m_index(index) {}

    bool operator ()(
            const QGalleryTrackerItemListPrivate::Row &row1,
            const QGalleryTrackerItemListPrivate::Row &row2)
    {
        return m_column->compare(*(row1.begin + m_index), *(row2.begin + m_index)) < 0;
    }

private:
    const QGalleryTrackerValueColumn *m_column;
    int m_index;
};

class QGalleryTrackerItemListGreaterThan
{
public:
    QGalleryTrackerItemListGreaterThan(const QGalleryTrackerValueColumn *column, int index)
        : m_column(column), m_index(index) {}

    bool operator ()(
            const QGalleryTrackerItemListPrivate::Row &row1,
            const QGalleryTrackerItemListPrivate::Row &row2)
    {
        return m_column->compare(*(row1.begin + m_index), *(row2.begin + m_index)) > 0;
    }

private:
    const QGalleryTrackerValueColumn *m_column;
    int m_index;
};

QTM_END_NAMESPACE

template <> void qSwap<QTM_PREPEND_NAMESPACE(QGalleryTrackerItemListPrivate::Row)>(
        QTM_PREPEND_NAMESPACE(QGalleryTrackerItemListPrivate::Row) &row1,
        QTM_PREPEND_NAMESPACE(QGalleryTrackerItemListPrivate::Row) &row2)
{
    typedef QVector<QVariant>::iterator iterator;

    for (iterator it1 = row1.begin, it2 = row2.begin; it1 != row1.end; ++it1, ++it2)
        qSwap(*it1, *it2);
}

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
            valueOffset);

    return values;
}

void QGalleryTrackerItemListPrivate::sortRows(
        row_iterator begin, row_iterator end, int column, bool reversed) const
{
    for (int vColumn = column - valueOffset; column != tableWidth; ++column, ++vColumn) {
        int sortFlags = valueColumns.at(vColumn)->sortFlags();
        if (sortFlags & QGalleryTrackerValueColumn::Sorted) {
            if (reversed) {
                QAlgorithmsPrivate::qReverse(begin, end);

                reversed = false;

                break;
            }
        } else if (sortFlags & QGalleryTrackerValueColumn::ReverseSorted) {
            if (!reversed) {
                QAlgorithmsPrivate::qReverse(begin, end);

                reversed = true;

                break;
            }
        } else if (sortFlags & QGalleryTrackerValueColumn::Ascending) {
            qStableSort(begin, end, QGalleryTrackerItemListLessThan(
                    valueColumns.at(vColumn), column));
            break;
        } else if (sortFlags & QGalleryTrackerValueColumn::Descending) {
            qStableSort(begin, end, QGalleryTrackerItemListGreaterThan(
                    valueColumns.at(vColumn), column));
            break;
        } else {
            return;
        }
    }

    for (row_iterator upper, lower = begin; lower != end; lower = upper) {
        int count = 1;

        for (upper = ++lower; upper != end && lower[column] != upper[column]; ++upper, ++count) {}

        if (count > 1)
            sortRows(lower, upper, column, reversed);
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
    } else {
        synchronizeWatcher.setFuture(
                QtConcurrent::run(this, &QGalleryTrackerItemListPrivate::synchronize));
    }
}

void QGalleryTrackerItemListPrivate::_q_synchronizeFinished()
{
    aCache.values.clear();
    aCache.count = 0;
}

QGalleryTrackerItemList::QGalleryTrackerItemList(
        QGalleryTrackerCompositeColumn *idColumn,
        QGalleryTrackerCompositeColumn *urlColumn,
        QGalleryTrackerCompositeColumn *typeColumn,
        int valueOffset,
        const QVector<QGalleryTrackerValueColumn *> &valueColumns,
        const QVector<QGalleryTrackerCompositeColumn *> &compositeColumns,
        const QVector<QGalleryTrackerAliasColumn *> &aliasColumns,
        const QVector<QGalleryTrackerImageColumn *> &imageColumns,
        QObject *parent)
    : QGalleryAbstractResponse(*new QGalleryTrackerItemListPrivate, parent)
{
    Q_D(QGalleryTrackerItemList);

    d->idColumn = idColumn;
    d->urlColumn = urlColumn;
    d->typeColumn = typeColumn;
    d->valueOffset = valueOffset;
    d->valueColumns = valueColumns;
    d->compositeColumns = compositeColumns;
    d->aliasColumns = aliasColumns;
    d->imageColumns = imageColumns;

    for (int i = 0; i < d->valueColumns.count(); ++i) {
        d->propertyNames.append(d->valueColumns.at(i)->name());
        d->propertyAttributes.append(d->valueColumns.at(i)->attributes());
    }

    d->compositeOffset = d->valueOffset + d->valueColumns.count();

    for (int i = 0; i < d->compositeColumns.count(); ++i) {
        d->propertyNames.append(d->compositeColumns.at(i)->name());
        d->propertyAttributes.append(d->compositeColumns.at(i)->attributes());
    }

    d->aliasOffset = d->tableWidth + d->compositeColumns.count();

    for (int i = 0; i < d->aliasColumns.count(); ++i) {
        d->propertyNames.append(d->aliasColumns.at(i)->name());
        d->propertyAttributes.append(d->aliasColumns.at(i)->attributes());
    }

    d->imageOffset = d->aliasOffset + d->aliasColumns.count();

    for (int i = 0; i < d->aliasColumns.count(); ++i) {
        d->propertyNames.append(d->imageColumns.at(i)->name());
        d->propertyAttributes.append(d->imageColumns.at(i)->attributes());
    }

    d->columnCount = d->imageOffset + d->imageColumns.count();

    d->rowCount = 0;


}

QGalleryTrackerItemList::~QGalleryTrackerItemList()
{
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

QString QGalleryTrackerItemList::id(int index) const
{
    Q_D(const QGalleryTrackerItemList);

    if (index < d->rCache.cutoff) {
        index -= d->rCache.index;

        return index >= 0 && index <= d->rCache.count
                ? d->idColumn->value(
                        d->rCache.values.constBegin() + (index * d->tableWidth)).toString()
                : QString();
    } else {
        index -= d->aCache.cutoff;

        return index >= 0 && index <= d->aCache.count
                ? d->idColumn->value(
                        d->aCache.values.constBegin() + (index * d->tableWidth)).toString()
                : QString();
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
