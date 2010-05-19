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
#include <QtDBus/qdbusreply.h>

Q_DECLARE_METATYPE(QVector<QStringList>)

QTM_BEGIN_NAMESPACE

int QGalleryTrackerItemListPrivate::queryRows(int offset)
{
    QDBusReply<QVector<QStringList> > reply = queryInterface->callWithArgumentList(
            QDBus::Block, queryMethod, QVariantList(queryArguments) << offset << queryLimit);

    if (reply.isValid()) {
        typedef QVector<QStringList>::const_iterator iterator;

        const QVector<QStringList> resultSet = reply.value();

        rCache.count = rCache.index + resultSet.count();

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
            correctRows(
                    row_iterator(values.begin(), tableWidth),
                    row_iterator(values.end(), tableWidth),
                    sortCriteria.constBegin(),
                    sortCriteria.constEnd());

            synchronize();
        }

        return QGalleryAbstractRequest::Succeeded;
    } else {
        qWarning("DBUS error %s", qPrintable(reply.error().message()));

        return QGalleryAbstractRequest::ConnectionError;
    }
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
        int aIndex = aIt - aBegin + aCache.index;
        int rIndex = rIt - rBegin + rCache.index;

        if (aIndex < rIndex) {
            QCoreApplication::postEvent(
                    q_func(), new RowEvent(RowsRemoved, aIndex, rIndex, rIndex - aIndex));
        } else if (rIndex > aIndex) {
            QCoreApplication::postEvent(
                    q_func(), new RowEvent(RowsInserted, aIndex, rIndex, aIndex - rIndex));
        }

        if (rIndex > rCache.index) {
            QCoreApplication::postEvent(q_func(), new RowEvent(
                    RowsChanged,
                    aCache.index,
                    rCache.index,
                    rIndex - rCache.index));
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
                    break;
                }
            } while (aIt != aEnd && rIt != rEnd);

            QCoreApplication::postEvent(q, new RowEvent(
                    RowsChanged,
                    (aBegin.begin - aCache.values.begin()) / tableWidth,
                    (rBegin.begin - rCache.values.begin()) / tableWidth,
                    (rIt.begin - rBegin.begin) / tableWidth));

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
                    if (rOuter != rBegin) {
                        QCoreApplication::postEvent(q, new RowEvent(
                                RowsInserted,
                                (aBegin.begin - aCache.values.begin()) / tableWidth + aCache.index,
                                (rBegin.begin - rCache.values.begin()) / tableWidth + rCache.index,
                                (rOuter.begin - rBegin.begin) / tableWidth));
                    }

                    QCoreApplication::postEvent(q, new RowEvent(
                            RowsRemoved,
                            (aOuter.begin - aCache.values.begin()) / tableWidth + aCache.index,
                            (aOuter.begin - rCache.values.begin()) / tableWidth + rCache.index,
                            (aInner.begin - aOuter.begin) / tableWidth));

                    aBegin = aInner;
                    rBegin = rOuter;

                    break;
                } else if ((equal = rInner.isEqual(aOuter, identityWidth))) {
                    if (aOuter != aBegin) {
                        QCoreApplication::postEvent(q, new RowEvent(
                                RowsRemoved,
                                (aBegin.begin - aCache.values.begin()) / tableWidth + aCache.index,
                                (rBegin.begin - rCache.values.begin()) / tableWidth + rCache.index,
                                (aOuter.begin - aBegin.begin) / tableWidth));
                    }

                    QCoreApplication::postEvent(q, new RowEvent(
                            RowsInserted,
                            (aOuter.begin - aCache.values.begin()) / tableWidth + aCache.index,
                            (rOuter.begin - rCache.values.begin()) / tableWidth + rCache.index,
                            (rInner.begin - rOuter.begin) / tableWidth));

                    aBegin = aOuter;
                    rBegin = rInner;

                    break;
                }
            }
        }
    }
}

void QGalleryTrackerItemListPrivate::_q_queryFinished()
{
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


    const int index = qMax(0, cursorPosition & ~63);
    const int count = index + minimumPagedItems;

    if (index > lowerThreshold || count < upperThreshold) {
        upperThreshold = count;
        lowerThreshold = index + queryLimit - minimumPagedItems;

        aCache.index = rCache.index;
        aCache.count = rCache.count;
        aCache.offset = rCache.index;

        rCache.index = index;
        rCache.count = 0;
        rCache.cutoff = 0;

        qSwap(aCache.values, rCache.values);

        queryWatcher.setFuture(QtConcurrent::run(
                this, &QGalleryTrackerItemListPrivate::queryRows, index));
    }
}

QGalleryTrackerItemList::QGalleryTrackerItemList(
        QGalleryTrackerItemListPrivate &dd,
        const QGalleryTrackerSchema &schema,
        const QGalleryDBusInterfacePointer &queryInterface,
        const QString &query,
        int cursorPosition,
        int minimumPagedItems,
        QObject *parent)
    : QGalleryAbstractResponse(dd, parent)
{
    Q_D(QGalleryTrackerItemList);

    d->cursorPosition = cursorPosition;
    d->minimumPagedItems = (minimumPagedItems + 15) & ~15;
    d->identityWidth = schema.identityWidth();
    d->valueOffset = schema.valueOffset();
    d->queryInterface = queryInterface;
    d->queryMethod = schema.queryMethod();
    d->queryArguments = schema.queryArguments(query);
    d->idColumn = schema.createIdColumn();
    d->urlColumn = schema.createUrlColumn();
    d->typeColumn = schema.createTypeColumn();
    d->valueColumns = schema.createValueColumns();
    d->compositeColumns = schema.createCompositeColumns();
    d->aliasColumns = schema.aliasColumns();
    d->imageColumns = schema.createImageColumns();
    d->sortCriteria = schema.sortCriteria();
    d->propertyNames = schema.propertyNames();
    d->propertyAttributes = schema.propertyAttributes();

    d->imageCaches.resize(d->imageColumns.count());

    d->compositeOffset = d->valueOffset + d->valueColumns.count();
    d->aliasOffset = d->tableWidth + d->compositeColumns.count();
    d->imageOffset = d->aliasOffset + d->aliasColumns.count();
    d->columnCount = d->imageOffset + d->imageColumns.count();

    d->rowCount = 0;
    d->queryLimit = qMax(256, 4 * d->minimumPagedItems);

    connect(&d->queryWatcher, SIGNAL(finished()), this, SLOT(_q_queryFinished()));

    const int index = qMax(0, d->cursorPosition & ~63);
    const int count = index + d->minimumPagedItems;

    d->upperThreshold = count;
    d->lowerThreshold = index + d->queryLimit - minimumPagedItems;

    d->queryWatcher.setFuture(QtConcurrent::run(
            d, &QGalleryTrackerItemListPrivate::queryRows, index));
}

QGalleryTrackerItemList::~QGalleryTrackerItemList()
{
    Q_D(QGalleryTrackerItemList);

    d->queryWatcher.waitForFinished();
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

void QGalleryTrackerItemList::setCursorPosition(int position)
{
    Q_D(QGalleryTrackerItemList);

    d->cursorPosition = position;

    if (d->queryWatcher.isFinished()) {
        const int index = qMax(0, position & ~63);
        const int count = index + d->minimumPagedItems;

        if (index > d->lowerThreshold || count < d->upperThreshold) {
            d->upperThreshold = count;
            d->lowerThreshold = index + d->queryLimit - d->minimumPagedItems;

            d->aCache.index = d->rCache.index;
            d->aCache.count = d->rCache.count;
            d->aCache.offset = d->rCache.index;

            d->rCache.index = index;
            d->rCache.count = 0;
            d->rCache.cutoff = 0;

            qSwap(d->aCache.values, d->rCache.values);

            d->queryWatcher.setFuture(QtConcurrent::run(
                    d, &QGalleryTrackerItemListPrivate::queryRows, index));
        }
    }

    if (d->rCache.cutoff > 0) {
        const int imageIndex = qMax(0, position & ~7);
        const int imageCount = imageIndex + ((d->minimumPagedItems + 7) & ~7);

        if (imageIndex < d->imageCacheIndex) {


        } else if (imageCount > d->imageCacheCount) {
            for (int i = 0, count = d->imageColumns.count(); i < count; ++i) {

            }
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
        } else if (key < d->columnCount) {      // Image column.
            key -= d->imageOffset;

            if (index < d->imageCacheCount && (index -= d->imageCacheIndex) >= 0) {
                return d->imageCaches.at(key).at(index).image();
            }
        }
    }

    return QVariant();
}

void QGalleryTrackerItemList::setMetaData(int, int, const QVariant &)
{

}

void QGalleryTrackerItemList::cancel()
{

}

bool QGalleryTrackerItemList::waitForFinished(int)
{
    d_func()->queryWatcher.waitForFinished();

    return true;
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

#include "moc_qgallerytrackeritemlist_p.cpp"

QTM_END_NAMESPACE
