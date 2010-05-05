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
        bool isEqual(const Row &row, int offset, int count) {
            return qEqual(begin + offset, row.begin + offset, begin + count); }

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

        Row *operator ->() { return &row; }
        const Row *operator ->() const { return &row; }

        QVariant operator[] (int column) const { return *(row.begin + column); }

        Row row;
    };

    struct Cache
    {
        int offset;
        int count;
        QVector<QVariant> values;
    };

    QGalleryTrackerCompositeColumn *idColumn;
    QGalleryTrackerCompositeColumn *urlColumn;
    QGalleryTrackerCompositeColumn *typeColumn;
    int valueOffset;
    int compositeOffset;
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

    void insert(int index, int count);
    void replace(int index, int count);
    void update(int index, int count);
    void remove(int index, int count);
    void clear(int index, int count);

    QVector<QVariant> parseTable(const QVector<QStringList> &table) const;
    void sortRows(row_iterator begin, row_iterator end, int column, bool reversed = false) const;

    void synchronizeRows();
    void rowsChanged(
            const QVector<QVariant>::iterator aBegin,
            const QVector<QVariant>::iterator rBegin,
            const QVector<QVariant>::iterator aEnd);
    void rowsInserted(
            const QVector<QVariant>::iterator aBegin,
            const QVector<QVariant>::iterator rBegin,
            const QVector<QVariant>::iterator rEnd);
    void rowsRemoved(
            const QVector<QVariant>::iterator aBegin,
            const QVector<QVariant>::iterator rBegin,
            const QVector<QVariant>::iterator aEnd);
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

void QGalleryTrackerItemListPrivate::insert(int index, int count)
{

}

void QGalleryTrackerItemListPrivate::replace(int index, int count)
{

}

void QGalleryTrackerItemListPrivate::update(int index, int count)
{

}

void QGalleryTrackerItemListPrivate::remove(int index, int count)
{

}

void QGalleryTrackerItemListPrivate::clear(int index, int count)
{

}

QVector<QVariant> QGalleryTrackerItemListPrivate::parseTable(
        const QVector<QStringList> &table) const
{
    typedef QVector<QStringList>::const_iterator iterator;

    QVector<QVariant> values;
    values.reserve(table.count() * compositeOffset);

    for (iterator it = table.begin(), end = table.end(); it != end; ++it) {
        for (int i = 0, count = qMin(valueOffset, it->count()); i < count; ++i)
            values.append(it->at(i));

        for (int i = valueOffset, count = qMin(compositeOffset, it->count()); i < count; ++i)
            values.append(valueColumns.at(i - valueOffset)->toVariant(it->at(i)));

        // The rows should all have a count equal to compositeOffset, but check just in case.
        for (int i = qMin(compositeOffset, it->count()); i < compositeOffset; ++i)
            values.append(QVariant());
    }

    return values;
}

void QGalleryTrackerItemListPrivate::sortRows(
        row_iterator begin, row_iterator end, int column, bool reversed) const
{
    for (int vColumn = column - valueOffset; column != compositeOffset; ++column, ++vColumn) {
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

void QGalleryTrackerItemListPrivate::synchronizeRows()
{
    const row_iterator aEnd(aCache.values.end(), compositeOffset);
    const row_iterator rEnd(rCache.values.end(), compositeOffset);

    row_iterator aBegin(aCache.values.begin(), compositeOffset);
    row_iterator rBegin(rCache.values.begin(), compositeOffset);

    while (aBegin != aEnd && rBegin != rEnd) {
        bool equal;
        bool changed = false;

        do {    // Skip over identical rows.
            if ((equal = aBegin->isEqual(*rBegin, valueOffset))
                    && !(changed = !aBegin->isEqual(*rBegin, valueOffset, compositeOffset))) {
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
                        && aIt->isEqual(*rIt, valueOffset, compositeOffset)) {
                    ++aIt;
                    ++rIt;
                } else {
                    break;
                }
            } while (aIt != aEnd && rIt != rEnd);

            rowsChanged(aBegin->begin, rBegin->begin, aIt->begin);

            continue;
        } else if (equal) {
            break;
        }

        row_iterator aIt = aBegin;
        row_iterator rIt = rBegin;

        // Scan for rows with common IDs.
        for (++aIt, ++rIt; aIt != aEnd && rIt != rEnd; ++aIt, ++rIt) {
            if ((equal = aIt->isEqual(*rBegin, valueOffset))) {
                // TODO: try and determine if this is the result of a row that has moved.
                // if (atIt->isEqual(*rBegin, valueOffset, compositeOffset))

                rowsRemoved(aBegin->begin, rBegin->begin, aIt->begin);

                aBegin = aIt;
                ++rBegin;

                break;
            } else if ((equal = rIt->isEqual(*aBegin, valueOffset))) {
                // TODO: try and determine if this is the result of a row that has moved.
                // if (rIt->isEqual(*aBegin, valueOffset, compositeOffset))

                rowsInserted(aBegin->begin, rBegin->begin, rIt->begin);

                ++aBegin;
                rBegin = rIt;

                break;
            }
        }

        if (!equal)
            break;
    }

    if (aBegin != aEnd)
        rowsRemoved(aBegin->begin, rBegin->begin, aEnd->begin);

    if (rBegin != rEnd)
        rowsInserted(aBegin->begin, rBegin->begin, rEnd->begin);
}

void QGalleryTrackerItemListPrivate::rowsChanged(
        const QVector<QVariant>::iterator aBegin,
        const QVector<QVariant>::iterator rBegin,
        const QVector<QVariant>::iterator aEnd)
{
}

void QGalleryTrackerItemListPrivate::rowsInserted(
        const QVector<QVariant>::iterator aBegin,
        const QVector<QVariant>::iterator rBegin,
        const QVector<QVariant>::iterator rEnd)
{
}

void QGalleryTrackerItemListPrivate::rowsRemoved(
        const QVector<QVariant>::iterator aBegin,
        const QVector<QVariant>::iterator rBegin,
        const QVector<QVariant>::iterator aEnd)
{
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

    d->aliasOffset = d->compositeOffset + d->compositeColumns.count();

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
    d->aCache.offset = 0;
    d->aCache.count = 0;
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

    index -= d->aCache.offset;

    return index >= 0 && index <= d->aCache.count
            ? d->idColumn->value(
                    d->aCache.values.constBegin() + (index * d->compositeOffset)).toString()
            : QString();
}

QUrl QGalleryTrackerItemList::url(int index) const
{
    Q_D(const QGalleryTrackerItemList);

    index -= d->aCache.offset;

    return index >= 0 && index <= d->aCache.count
            ? d->urlColumn->value(
                    d->aCache.values.constBegin() + (index * d->compositeOffset)).toUrl()
            : QUrl();
}

QString QGalleryTrackerItemList::type(int index) const
{
    Q_D(const QGalleryTrackerItemList);

    index -= d->aCache.offset;

    return index >= 0 && index <= d->aCache.count
            ? d->typeColumn->value(
                    d->aCache.values.constBegin() + (index * d->compositeOffset)).toString()
            : QString();
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

    index -= d->aCache.offset;

    if (index >= 0 && index <= d->aCache.count && key >= d->valueOffset) {
        if (key < d->compositeOffset) {         // Value column.
            key -= d->valueOffset;

            return d->aCache.values.at((index * d->compositeOffset) + key);
        } else if (key < d->aliasOffset) {      // Composite column.
            key -= d->compositeOffset;

            return d->compositeColumns.at(key)->value(
                    d->aCache.values.constBegin() + (index * d->compositeOffset));
        } else if (key < d->imageOffset) {      // Alias column.
            key = d->aliasColumns.at(key - d->aliasOffset)->aliasedKey();

            return d->aCache.values.at((index * d->compositeOffset) + key);
        } else if (key < d->columnCount) {      // Image column.
            key -= d->imageOffset;

            // TBD

            return QVariant();
        }
    }

    return QVariant();
}

void QGalleryTrackerItemList::setMetaData(int, int, const QVariant &)
{

}

#include "moc_qgallerytrackeritemlist_p.cpp"

QTM_END_NAMESPACE
