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

#ifndef QGALLERYTRACKERITEMLIST_P_P_H
#define QGALLERYTRACKERITEMLIST_P_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API. It exists purely as an
// implementation detail. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//


#include "qgallerytrackeritemlist_p.h"

#include "qgalleryabstractresponse_p.h"
#include "qgallerytrackerlistcolumn_p.h"
#include "qgallerytrackerschema_p.h"

#include <QtCore/qcoreevent.h>
#include <QtCore/qfuturewatcher.h>

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

        QVector<QVariant>::iterator begin;
        QVector<QVariant>::iterator end;
    };

    struct row_iterator
    {
        row_iterator() {}
        row_iterator(QVector<QVariant>::iterator begin, int width) : begin(begin), width(width) {}

        bool operator != (const row_iterator &other) const { return begin != other.begin; }
        bool operator <(const row_iterator &other) const { return begin < other.begin; }

        row_iterator &operator ++() { begin += width; return *this; }
        row_iterator &operator --() { begin -= width; return *this; }

        row_iterator operator ++(int) { row_iterator n(*this); begin += width; return n; }
        row_iterator operator --(int) { row_iterator n(*this); begin -= width; return n; }

        int operator -(const row_iterator &other) const { return (begin - other.begin) / width; }

        row_iterator operator +(int span) const {
            return row_iterator(begin + (span * width), width); }

        row_iterator &operator +=(int span) { begin += span * width; return *this; }

        Row &operator *() {  return row = Row(begin, begin + width); }
        const Row &operator *() const {  return row = Row(begin, begin + width); }

        bool isEqual(const row_iterator &other, int count) const {
            return qEqual(begin, begin + count, other.begin); }
        bool isEqual(const row_iterator &other, int index, int count) {
            return qEqual(begin + index, begin + count, other.begin + index); }

        const QVariant &operator[] (int column) const { return *(begin + column); }

        QVector<QVariant>::iterator begin;
        int width;
        mutable Row row;
    };

    enum RowEventType
    {
        RowsChanged = QEvent::User,
        RowsInserted,
        RowsRemoved,
        SyncFinalized
    };

    class RowEvent : public QEvent
    {
    public:
        RowEvent(RowEventType type, int aIndex, int rIndex, int count)
            : QEvent(QEvent::Type(type))
            , aIndex(aIndex)
            , rIndex(rIndex)
            , count(count)
        {
        }

        const int aIndex;
        const int rIndex;
        const int count;
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

    typedef QVector<QGalleryTrackerSortCriteria>::const_iterator sort_iterator;

    ~QGalleryTrackerItemListPrivate()
    {
        delete idColumn;
        delete urlColumn;
        delete typeColumn;
        qDeleteAll(valueColumns);
        qDeleteAll(compositeColumns);
        qDeleteAll(imageColumns);
    }

    QGalleryTrackerCompositeColumn *idColumn;
    QGalleryTrackerCompositeColumn *urlColumn;
    QGalleryTrackerCompositeColumn *typeColumn;

    int identityWidth;
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
    int queryLimit;
    int imageCacheIndex;
    int imageCacheCount;
    QGalleryDBusInterfacePointer queryInterface;
    QString queryMethod;
    QVariantList queryArguments;
    QStringList propertyNames;
    QVector<QGalleryProperty::Attributes> propertyAttributes;
    QVector<QGalleryTrackerValueColumn *> valueColumns;
    QVector<QGalleryTrackerCompositeColumn *> compositeColumns;
    QVector<int> aliasColumns;
    QVector<QGalleryTrackerImageColumn *> imageColumns;
    QVector<QGalleryTrackerSortCriteria> sortCriteria;
    Cache aCache;   // Access cache.
    Cache rCache;   // Replacement cache.

    QFutureWatcher<int> queryWatcher;

    void update(int index);

    int queryRows(int offset);
    void correctRows(
            row_iterator begin,
            row_iterator end,
            const sort_iterator sortBegin,
            const sort_iterator sortEnd,
            bool reversed = false) const;

    void synchronize();
    void synchronizeRows(
            row_iterator &aBegin,
            row_iterator &rBegin,
            const row_iterator &aEnd,
            const row_iterator &rEnd);

    void _q_queryFinished();
    void _q_imagesLoaded(int index, const QList<uint> &ids);
};

QTM_END_NAMESPACE

template <> inline void qSwap<QTM_PREPEND_NAMESPACE(QGalleryTrackerItemListPrivate::Row)>(
        QTM_PREPEND_NAMESPACE(QGalleryTrackerItemListPrivate::Row) &row1,
        QTM_PREPEND_NAMESPACE(QGalleryTrackerItemListPrivate::Row) &row2)
{
    typedef QVector<QVariant>::iterator iterator;

    for (iterator it1 = row1.begin, it2 = row2.begin; it1 != row1.end; ++it1, ++it2)
        qSwap(*it1, *it2);
}

#endif
