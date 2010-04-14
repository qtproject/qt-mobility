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

#ifndef QGALLERYFILTER_H
#define QGALLERYFILTER_H

#include <qmobilityglobal.h>

#include <QtCore/qset.h>
#include <QtCore/qshareddata.h>
#include <QtCore/qvariant.h>
#include <QtCore/qvector.h>

QTM_BEGIN_NAMESPACE
class QGalleryFilter;
class QGalleryIntersectionFilter;
class QGalleryMetaDataFilter;
class QGalleryMetaDataRangeFilter;
class QGalleryUnionFilter;
QTM_END_NAMESPACE

#ifndef Q_QDOC
bool operator ==(
        const QTM_PREPEND_NAMESPACE(QGalleryFilter) &filter1,
        const QTM_PREPEND_NAMESPACE(QGalleryFilter) &filter2);

bool operator !=(
        const QTM_PREPEND_NAMESPACE(QGalleryFilter) &filter1,
        const QTM_PREPEND_NAMESPACE(QGalleryFilter) &filter2);
#else
bool operator ==(const QGalleryFilter &filter1, const QGalleryFilter &filter2);
bool operator !=(const QGalleryFilter &filter2, const QGalleryFilter &filter2);
#endif

QTM_BEGIN_NAMESPACE

class QGalleryFilterPrivate;

class Q_GALLERY_EXPORT QGalleryFilter
{
public:
    enum Type
    {
        Invalid,
        Intersection,
        Union,
        MetaData,
        MetaDataRange
    };

    enum RangeFlag
    {
        EqualsMinimum            = 0x01,
        GreaterThanMinimum       = 0x02,
        GreaterThanEqualsMinimum = EqualsMinimum | GreaterThanMinimum,
        LessThanMaximum          = 0x04,
        EqualsMaximum            = 0x08,
        LessThanEqualsMaximum    = EqualsMaximum | LessThanMaximum,
        InclusiveRange           = GreaterThanEqualsMinimum | LessThanEqualsMaximum,
        ExclusiveRange           = GreaterThanMinimum | LessThanMaximum
    };

    Q_DECLARE_FLAGS(RangeFlags, RangeFlag);

    QGalleryFilter();
    QGalleryFilter(const QGalleryFilter &filter);
    QGalleryFilter(const QGalleryIntersectionFilter &filter);
    QGalleryFilter(const QGalleryUnionFilter &filter);
    QGalleryFilter(const QGalleryMetaDataFilter &filter);
    QGalleryFilter(const QGalleryMetaDataRangeFilter &filter);
    ~QGalleryFilter();

    QGalleryFilter &operator =(const QGalleryFilter &filter);

    Type type() const;

    QGalleryIntersectionFilter toIntersectionFilter() const;
    QGalleryUnionFilter toUnionFilter() const;
    QGalleryMetaDataFilter toMetaDataFilter() const;
    QGalleryMetaDataRangeFilter toMetaDataRangeFilter() const;

private:
    QSharedDataPointer<QGalleryFilterPrivate> d;

    friend bool ::operator ==(const QGalleryFilter &filter1, const QGalleryFilter &filter2);
    friend bool ::operator !=(const QGalleryFilter &filter1, const QGalleryFilter &filter2);
};

class QGalleryIntersectionFilterPrivate;

class Q_GALLERY_EXPORT QGalleryIntersectionFilter
{
public:
    QGalleryIntersectionFilter();
    QGalleryIntersectionFilter(const QGalleryUnionFilter &filter);
    QGalleryIntersectionFilter(const QGalleryMetaDataFilter &filter);
    QGalleryIntersectionFilter(const QGalleryMetaDataRangeFilter &filter);
    QGalleryIntersectionFilter(const QGalleryIntersectionFilter &filter);
    ~QGalleryIntersectionFilter();

    QGalleryIntersectionFilter &operator =(const QGalleryIntersectionFilter &filter);

    int filterCount() const;
    bool isEmpty() const;

    QList<QGalleryFilter> filters() const;

    void append(const QGalleryUnionFilter &filter);
    void append(const QGalleryMetaDataFilter &filter);
    void append(const QGalleryMetaDataRangeFilter &filter);
    void append(const QGalleryIntersectionFilter &filter);

    void insert(int index, const QGalleryUnionFilter &filter);
    void insert(int index, const QGalleryMetaDataFilter &filter);
    void insert(int index, const QGalleryMetaDataRangeFilter &filter);
    void insert(int index, const QGalleryIntersectionFilter &filter);

    void replace(int index, const QGalleryUnionFilter &filter);
    void replace(int index, const QGalleryMetaDataFilter &filter);
    void replace(int index, const QGalleryMetaDataRangeFilter &filter);

    void removeAt(int index);
    void clear();

private:
    explicit QGalleryIntersectionFilter(QGalleryFilterPrivate *d);

    QSharedDataPointer<QGalleryIntersectionFilterPrivate> d;

    friend class QGalleryFilter;
};

class QGalleryUnionFilterPrivate;

class Q_GALLERY_EXPORT QGalleryUnionFilter
{
public:
    QGalleryUnionFilter();
    QGalleryUnionFilter(const QGalleryMetaDataFilter &filter);
    QGalleryUnionFilter(const QGalleryMetaDataRangeFilter &filter);
    QGalleryUnionFilter(const QGalleryUnionFilter &other);
    ~QGalleryUnionFilter();

    QGalleryUnionFilter &operator =(const QGalleryUnionFilter &filter);

    int filterCount() const;
    bool isEmpty() const;

    QList<QGalleryFilter> filters() const;

    void append(const QGalleryMetaDataFilter &filter);
    void append(const QGalleryMetaDataRangeFilter &filter);
    void append(const QGalleryUnionFilter &filter);

    void insert(int index, const QGalleryMetaDataFilter &filter);
    void insert(int index, const QGalleryMetaDataRangeFilter &filter);
    void insert(int index, const QGalleryUnionFilter &filter);

    void replace(int index, const QGalleryMetaDataFilter &filter);
    void replace(int index, const QGalleryMetaDataRangeFilter &filter);

    void removeAt(int index);
    void clear();

private:
    explicit QGalleryUnionFilter(QGalleryFilterPrivate *d);

    QSharedDataPointer<QGalleryUnionFilterPrivate> d;

    friend class QGalleryFilter;
};

class QGalleryMetaDataFilterPrivate;

class Q_GALLERY_EXPORT QGalleryMetaDataFilter
{
public:
    QGalleryMetaDataFilter();
    QGalleryMetaDataFilter(
            const QString &fieldName,
            const QVariant &value,
            Qt::MatchFlags flags = Qt::MatchExactly);
    QGalleryMetaDataFilter(const QGalleryMetaDataFilter &other);
    ~QGalleryMetaDataFilter();

    QGalleryMetaDataFilter &operator =(const QGalleryMetaDataFilter &filter);

    QString fieldName() const;
    void setFieldName(const QString &name);

    QVariant value() const;
    void setValue(const QVariant &value);

    Qt::MatchFlags matchFlags() const;
    void setMatchFlags(Qt::MatchFlags flags);

private:
    explicit QGalleryMetaDataFilter(QGalleryFilterPrivate *d);

    QSharedDataPointer<QGalleryMetaDataFilterPrivate> d;

    friend class QGalleryFilter;
};

class QGalleryMetaDataRangeFilterPrivate;

class Q_GALLERY_EXPORT QGalleryMetaDataRangeFilter
{
public:
    QGalleryMetaDataRangeFilter();
    QGalleryMetaDataRangeFilter(const QGalleryMetaDataRangeFilter &filter);
    ~QGalleryMetaDataRangeFilter();

    QGalleryMetaDataRangeFilter &operator =(const QGalleryMetaDataRangeFilter &filter);

    QString fieldName() const;
    void setFieldName(const QString &name);

    QVariant minimumValue() const;
    QVariant maximumValue() const;

    QGalleryFilter::RangeFlags rangeFlags() const;

    void setInclusiveRange(const QVariant &minimum, const QVariant &maximum);
    void setExclusiveRange(const QVariant &minimum, const QVariant &maximum);
    void setLessThan(const QVariant &value);
    void setLessThanEquals(const QVariant &value);
    void setGreaterThan(const QVariant &value);
    void setGreaterThanEquals(const QVariant &value);

private:
    explicit QGalleryMetaDataRangeFilter(QGalleryFilterPrivate *d);

    QSharedDataPointer<QGalleryMetaDataRangeFilterPrivate> d;

    friend class QGalleryFilter;
};

QTM_END_NAMESPACE

#ifndef Q_QDOC
template <typename T>
QTM_PREPEND_NAMESPACE(QGalleryUnionFilter) operator &&(
        const QTM_PREPEND_NAMESPACE(QGalleryUnionFilter) &filter1, const T &filter2)
{
    QTM_PREPEND_NAMESPACE(QGalleryUnionFilter) filter = filter1;
    filter.append(filter2);
    return filter;
}

template <typename T>
QTM_PREPEND_NAMESPACE(QGalleryIntersectionFilter) operator ||(
        const QTM_PREPEND_NAMESPACE(QGalleryIntersectionFilter) &filter1, const T &filter2)
{
    QTM_PREPEND_NAMESPACE(QGalleryIntersectionFilter) filter = filter1;
    filter.append(filter2);
    return filter;
}
#else
QGalleryUnionFilter operator &&(const QGalleryUnionFilter &filter1, const T &filter2);
QGalleryIntersectionFilter operator ||(const QGalleryIntersectionFilter &filter1, const T &filter2);
#endif


Q_DECLARE_OPERATORS_FOR_FLAGS(QTM_PREPEND_NAMESPACE(QGalleryFilter::RangeFlags))

#endif
