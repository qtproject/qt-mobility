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

#ifndef QGALLERYFILTER_H
#define QGALLERYFILTER_H

#include <qmobilityglobal.h>

#include <QtCore/qset.h>
#include <QtCore/qshareddata.h>
#include <QtCore/qvariant.h>
#include <QtCore/qvector.h>

QT_BEGIN_NAMESPACE
class QDebug;
QT_END_NAMESPACE

QTM_BEGIN_NAMESPACE
class QGalleryFilter;
class QGalleryIntersectionFilter;
class QGalleryMetaDataFilter;
class QGalleryUnionFilter;
QTM_END_NAMESPACE

#ifndef Q_QDOC
bool Q_GALLERY_EXPORT operator ==(
        const QTM_PREPEND_NAMESPACE(QGalleryFilter) &filter1,
        const QTM_PREPEND_NAMESPACE(QGalleryFilter) &filter2);

bool Q_GALLERY_EXPORT operator !=(
        const QTM_PREPEND_NAMESPACE(QGalleryFilter) &filter1,
        const QTM_PREPEND_NAMESPACE(QGalleryFilter) &filter2);
#else
bool Q_GALLERY_EXPORT operator ==(const QGalleryFilter &filter1, const QGalleryFilter &filter2);
bool Q_GALLERY_EXPORT operator !=(const QGalleryFilter &filter2, const QGalleryFilter &filter2);
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
        MetaData
    };

    enum Comparator
    {
        Equals,
        LessThan,
        GreaterThan,
        LessThanEquals,
        GreaterThanEquals,
        Contains,
        StartsWith,
        EndsWith,
        Wildcard,
        RegExp
    };

    QGalleryFilter();
    QGalleryFilter(const QGalleryFilter &filter);
    QGalleryFilter(const QGalleryIntersectionFilter &filter);
    QGalleryFilter(const QGalleryUnionFilter &filter);
    QGalleryFilter(const QGalleryMetaDataFilter &filter);
    ~QGalleryFilter();

    QGalleryFilter &operator =(const QGalleryFilter &filter);

    Type type() const;
    bool isValid() const;

    QGalleryIntersectionFilter toIntersectionFilter() const;
    QGalleryUnionFilter toUnionFilter() const;
    QGalleryMetaDataFilter toMetaDataFilter() const;

private:
    QSharedDataPointer<QGalleryFilterPrivate> d;

    friend Q_GALLERY_EXPORT bool ::operator ==(
            const QGalleryFilter &filter1, const QGalleryFilter &filter2);
    friend Q_GALLERY_EXPORT bool ::operator !=(
            const QGalleryFilter &filter1, const QGalleryFilter &filter2);

#ifndef QT_NO_DEBUG_STREAM
    friend Q_GALLERY_EXPORT QDebug operator <<(QDebug debug, const QGalleryFilter &filter);
#endif
};

#ifndef QT_NO_DEBUG_STREAM
Q_GALLERY_EXPORT QDebug operator <<(QDebug debug, const QGalleryFilter &filter);
#endif

class QGalleryIntersectionFilterPrivate;

class Q_GALLERY_EXPORT QGalleryIntersectionFilter
{
public:
    QGalleryIntersectionFilter();
    QGalleryIntersectionFilter(const QGalleryMetaDataFilter &filter);
    QGalleryIntersectionFilter(const QGalleryUnionFilter &filter);
    QGalleryIntersectionFilter(const QGalleryIntersectionFilter &filter);
    ~QGalleryIntersectionFilter();

    QGalleryIntersectionFilter &operator =(const QGalleryIntersectionFilter &filter);

    bool isValid() const;

    int filterCount() const;
    bool isEmpty() const;

    QList<QGalleryFilter> filters() const;

    void append(const QGalleryMetaDataFilter &filter);
    void append(const QGalleryUnionFilter &filter);
    void append(const QGalleryIntersectionFilter &filter);

    void insert(int index, const QGalleryMetaDataFilter &filter);
    void insert(int index, const QGalleryUnionFilter &filter);
    void insert(int index, const QGalleryIntersectionFilter &filter);

    void replace(int index, const QGalleryMetaDataFilter &filter);
    void replace(int index, const QGalleryUnionFilter &filter);

    void removeAt(int index);
    void clear();

private:
    explicit inline QGalleryIntersectionFilter(QGalleryFilterPrivate *d);
    explicit inline QGalleryIntersectionFilter(QGalleryFilter::Type type);

    QSharedDataPointer<QGalleryIntersectionFilterPrivate> d;

    friend class QGalleryFilter;
};

class QGalleryUnionFilterPrivate;

class Q_GALLERY_EXPORT QGalleryUnionFilter
{
public:
    QGalleryUnionFilter();
    QGalleryUnionFilter(const QGalleryMetaDataFilter &filter);
    QGalleryUnionFilter(const QGalleryIntersectionFilter &filter);
    QGalleryUnionFilter(const QGalleryUnionFilter &other);
    ~QGalleryUnionFilter();

    QGalleryUnionFilter &operator =(const QGalleryUnionFilter &filter);

    bool isValid() const;

    int filterCount() const;
    bool isEmpty() const;

    QList<QGalleryFilter> filters() const;

    void append(const QGalleryMetaDataFilter &filter);
    void append(const QGalleryIntersectionFilter &filter);
    void append(const QGalleryUnionFilter &filter);

    void insert(int index, const QGalleryMetaDataFilter &filter);
    void insert(int index, const QGalleryIntersectionFilter &filter);
    void insert(int index, const QGalleryUnionFilter &filter);

    void replace(int index, const QGalleryMetaDataFilter &filter);
    void replace(int index, const QGalleryIntersectionFilter &filter);

    void removeAt(int index);
    void clear();

private:
    explicit inline QGalleryUnionFilter(QGalleryFilterPrivate *d);
    explicit inline QGalleryUnionFilter(QGalleryFilter::Type type);

    QSharedDataPointer<QGalleryUnionFilterPrivate> d;

    friend class QGalleryFilter;
};

class QGalleryMetaDataFilterPrivate;

class Q_GALLERY_EXPORT QGalleryMetaDataFilter
{
public:
    QGalleryMetaDataFilter();
    QGalleryMetaDataFilter(
            const QString &propertyName,
            const QVariant &value,
            QGalleryFilter::Comparator comparator = QGalleryFilter::Equals,
            Qt::CaseSensitivity caseSensitivity = Qt::CaseSensitive);
    QGalleryMetaDataFilter(const QGalleryMetaDataFilter &other);
    ~QGalleryMetaDataFilter();

    QGalleryMetaDataFilter &operator =(const QGalleryMetaDataFilter &filter);

    bool isValid() const;

    QString propertyName() const;
    void setPropertyName(const QString &name);

    QVariant value() const;
    void setValue(const QVariant &value);

    QGalleryFilter::Comparator comparator() const;
    void setComparator(QGalleryFilter::Comparator comparator);

    Qt::CaseSensitivity caseSensitivity() const;
    void setCaseSensitivity(Qt::CaseSensitivity sensitivity);

    bool isInverted() const;
    void setInverted(bool inverted);

    QGalleryMetaDataFilter operator !() const;

private:
    explicit inline QGalleryMetaDataFilter(QGalleryFilterPrivate *d);
    explicit inline QGalleryMetaDataFilter(QGalleryFilter::Type type);

    QSharedDataPointer<QGalleryMetaDataFilterPrivate> d;

    friend class QGalleryFilter;
};

QTM_END_NAMESPACE

#ifndef Q_QDOC
template <typename T>
QTM_PREPEND_NAMESPACE(QGalleryUnionFilter) operator ||(
        const QTM_PREPEND_NAMESPACE(QGalleryUnionFilter) &filter1, const T &filter2)
{
    QTM_PREPEND_NAMESPACE(QGalleryUnionFilter) filter = filter1;
    filter.append(filter2);
    return filter;
}

template <typename T>
QTM_PREPEND_NAMESPACE(QGalleryIntersectionFilter) operator &&(
        const QTM_PREPEND_NAMESPACE(QGalleryIntersectionFilter) &filter1, const T &filter2)
{
    QTM_PREPEND_NAMESPACE(QGalleryIntersectionFilter) filter = filter1;
    filter.append(filter2);
    return filter;
}
#else
QGalleryUnionFilter Q_GALLERY_EXPORT operator ||(const QGalleryUnionFilter &filter1, const T &filter2);
QGalleryIntersectionFilter Q_GALLERY_EXPORT operator &&(const QGalleryIntersectionFilter &filter1, const T &filter2);
#endif

#endif
