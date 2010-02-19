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

#include "qgalleryfilter.h"

#include <QtCore/qstringlist.h>
#include <QtCore/qurl.h>

class QGalleryFilterPrivate : public QSharedData
{
public:
    QGalleryFilterPrivate(QGalleryFilter::Type type)
        : type(type)
    {
    }

    QGalleryFilterPrivate(const QGalleryFilterPrivate &other)
        : QSharedData(other)
        , type(other.type)
    {
    }

    virtual ~QGalleryFilterPrivate() {}

    virtual QGalleryFilterPrivate *clone() const = 0;
    virtual bool isEqual(const QGalleryFilterPrivate &other) const = 0;

    const QGalleryFilter::Type type;
};

template <> QGalleryFilterPrivate *QSharedDataPointer<QGalleryFilterPrivate>::clone()
{
    return d->clone();
}

class QGalleryInvalidFilterPrivate : public QGalleryFilterPrivate
{
public:
    QGalleryInvalidFilterPrivate() : QGalleryFilterPrivate(QGalleryFilter::Invalid) {}

    QGalleryFilterPrivate *clone() const { return new QGalleryInvalidFilterPrivate; }
    bool isEqual(const QGalleryFilterPrivate &other) const { return type == other.type; }
};

class QGalleryDocumentFilterPrivate : public QGalleryFilterPrivate
{
public:
    QGalleryDocumentFilterPrivate()
        : QGalleryFilterPrivate(QGalleryFilter::Document)
    {
    }

    QGalleryDocumentFilterPrivate(const QGalleryDocumentFilterPrivate &other)
        : QGalleryFilterPrivate(other)
        , documentIds(other.documentIds)
    {
    }

    QGalleryFilterPrivate *clone() const
    {
        return new QGalleryDocumentFilterPrivate(*this);
    }

    bool isEqual(const QGalleryFilterPrivate &other) const
    {
        return other.type == type && static_cast<const QGalleryDocumentFilterPrivate &>
                (other).documentIds == documentIds;
    }

    QStringList documentIds;
};

class QGalleryDocumentUrlFilterPrivate : public QGalleryFilterPrivate
{
public:
    QGalleryDocumentUrlFilterPrivate()
        : QGalleryFilterPrivate(QGalleryFilter::DocumentUrl)
    {
    }

    QGalleryDocumentUrlFilterPrivate(const QGalleryDocumentUrlFilterPrivate &other)
        : QGalleryFilterPrivate(other)
        , documentUrls(other.documentUrls)
    {
    }

    QGalleryFilterPrivate *clone() const
    {
        return new QGalleryDocumentUrlFilterPrivate(*this);
    }

    bool isEqual(const QGalleryFilterPrivate &other) const
    {
        return other.type == type && static_cast<const QGalleryDocumentUrlFilterPrivate &>
                (other).documentUrls == documentUrls;
    }

    QList<QUrl> documentUrls;
};

class QGalleryContainerFilterPrivate : public QGalleryFilterPrivate
{
public:
    QGalleryContainerFilterPrivate()
        : QGalleryFilterPrivate(QGalleryFilter::Container)
    {
    }

    QGalleryContainerFilterPrivate(const QGalleryContainerFilterPrivate &other)
        : QGalleryFilterPrivate(other)
        , containerId(other.containerId)
    {
    }

    QGalleryFilterPrivate *clone() const
    {
        return new QGalleryContainerFilterPrivate(*this);
    }

    bool isEqual(const QGalleryFilterPrivate &other) const
    {
        return other.type == type && static_cast<const QGalleryContainerFilterPrivate &>
                (other).containerId == containerId;
    }

    QString containerId;
};

class QGalleryContainerUrlFilterPrivate : public QGalleryFilterPrivate
{
public:
    QGalleryContainerUrlFilterPrivate()
        : QGalleryFilterPrivate(QGalleryFilter::ContainerUrl)
    {
    }

    QGalleryContainerUrlFilterPrivate(const QGalleryContainerUrlFilterPrivate &other)
        : QGalleryFilterPrivate(other)
        , containerUrl(other.containerUrl)
    {
    }

    QGalleryFilterPrivate *clone() const
    {
        return new QGalleryContainerUrlFilterPrivate(*this);
    }

    bool isEqual(const QGalleryFilterPrivate &other) const
    {
        return other.type == type && static_cast<const QGalleryContainerUrlFilterPrivate &>
                (other).containerUrl == containerUrl;
    }

    QUrl containerUrl;
};

class QGalleryIntersectionFilterPrivate : public QGalleryFilterPrivate
{
public:
    QGalleryIntersectionFilterPrivate()
        : QGalleryFilterPrivate(QGalleryFilter::Intersection)
    {
    }

    QGalleryIntersectionFilterPrivate(const QGalleryIntersectionFilterPrivate &other)
        : QGalleryFilterPrivate(other)
        , filters(other.filters)
    {
    }

    QGalleryFilterPrivate *clone() const
    {
        return new QGalleryIntersectionFilterPrivate(*this);
    }

    bool isEqual(const QGalleryFilterPrivate &other) const
    {
        return other.type == type && static_cast<const QGalleryIntersectionFilterPrivate &>
                (other).filters == filters;
    }

    QList<QGalleryFilter> filters;
};

class QGalleryUnionFilterPrivate : public QGalleryFilterPrivate
{
public:
    QGalleryUnionFilterPrivate() : QGalleryFilterPrivate(QGalleryFilter::Union) {}

    QGalleryUnionFilterPrivate(const QGalleryUnionFilterPrivate &other)
        : QGalleryFilterPrivate(other)
        , filters(other.filters)
    {
    }

    QGalleryFilterPrivate *clone() const
    {
        return new QGalleryUnionFilterPrivate(*this);
    }

    bool isEqual(const QGalleryFilterPrivate &other) const
    {
        return other.type == type && static_cast<const QGalleryUnionFilterPrivate &>
                (other).filters == filters;
    }

    QList<QGalleryFilter> filters;
};

class QGalleryMetaDataFilterPrivate : public QGalleryFilterPrivate
{
public:
    QGalleryMetaDataFilterPrivate()
        : QGalleryFilterPrivate(QGalleryFilter::MetaData)
        , flags(0)
    {
    }

    QGalleryMetaDataFilterPrivate(const QGalleryMetaDataFilterPrivate &other)
        : QGalleryFilterPrivate(other)
        , flags(other.flags)
        , field(other.field)
        , value(other.value)
    {
    }

    QGalleryFilterPrivate *clone() const
    {
        return new QGalleryMetaDataFilterPrivate(*this);
    }

    bool isEqual(const QGalleryFilterPrivate &other) const
    {
        if (other.type == type) {
            const QGalleryMetaDataFilterPrivate &o
                    = static_cast<const QGalleryMetaDataFilterPrivate &>(other);

            return o.flags == flags
                    && o.field == field
                    && o.value == value;
        } else {
            return false;
        }
    }

    Qt::MatchFlags flags;
    QString field;
    QVariant value;
};

class QGalleryMetaDataRangeFilterPrivate : public QGalleryFilterPrivate
{
public:
    QGalleryMetaDataRangeFilterPrivate()
        : QGalleryFilterPrivate(QGalleryFilter::MetaDataRange)
        , flags(0)
    {
    }

    QGalleryMetaDataRangeFilterPrivate(const QGalleryMetaDataRangeFilterPrivate &other)
        : QGalleryFilterPrivate(other)
        , flags(other.flags)
        , field(other.field)
        , minimum(other.minimum)
        , maximum(other.maximum)
    {
    }

    QGalleryFilterPrivate *clone() const
    {
        return new QGalleryMetaDataRangeFilterPrivate(*this);
    }

    bool isEqual(const QGalleryFilterPrivate &other) const
    {
        if (other.type == type) {
            const QGalleryMetaDataRangeFilterPrivate &o
                    = static_cast<const QGalleryMetaDataRangeFilterPrivate &>(other);

            return o.flags == flags
                    && o.field == field
                    && o.minimum == minimum
                    && o.maximum == maximum;
        } else {
            return false;
        }
    }

    void setRange(const QVariant &min, const QVariant &max, QGalleryFilter::RangeFlags f)
    {
        minimum = min;
        maximum = max;
        flags = f;
    }

    QGalleryFilter::RangeFlags flags;
    QString field;
    QVariant minimum;
    QVariant maximum;
};

/*!
    \class QGalleryFilter

    \ingroup gallery
    \ingroup gallery-filters

    \brief The QGalleryFilter class provides filtering criteria for gallery
    requests.
*/

/*!
    \enum QGalleryFilter::Type

    Identifies the type of a filter.

    \value Invalid
    \value Document
    \value DocumentUrl
    \value Container
    \value ContainerUrl
    \value Intersection
    \value Union
    \value MetaData
    \value MetaDataRange
*/

/*!
    \enum QGalleryFilter::RangeFlag

    Enumerates the comparisons made by QGalleryMetaDataRangeFilter.

    \value EqualsMinimum
    \value GreaterThanMinimum
    \value GreaterThanEqualsMinimum
    \value LessThanMaximum
    \value EqualsMaximum
    \value LessThanEqualsMaximum
    \value InclusiveRange
    \value ExclusiveRange
*/

/*!
    Constructs a gallery filter of type \l Invalid.
*/

QGalleryFilter::QGalleryFilter()
    : d(new QGalleryInvalidFilterPrivate)
{
}

/*!
    Constructs a copy of a gallery \a filter.
*/

QGalleryFilter::QGalleryFilter(const QGalleryFilter &filter)
    : d(filter.d)
{
}

/*!
    Constructs a copy of a gallery document \a filter.
*/


QGalleryFilter::QGalleryFilter(const QGalleryDocumentFilter &filter)
    : d(const_cast<QGalleryDocumentFilterPrivate *>(filter.d.constData()))
{
}

/*!
    Constructs a copy of a gallery document URL \a filter.
*/

QGalleryFilter::QGalleryFilter(const QGalleryDocumentUrlFilter &filter)
    : d(const_cast<QGalleryDocumentUrlFilterPrivate *>(filter.d.constData()))
{
}

/*!
    Constructs a copy of a gallery container \a filter.
*/

QGalleryFilter::QGalleryFilter(const QGalleryContainerFilter &filter)
    : d(const_cast<QGalleryContainerFilterPrivate *>(filter.d.constData()))
{
}

/*!
    Constructs a copy of a gallery container URL \a filter.
*/

QGalleryFilter::QGalleryFilter(const QGalleryContainerUrlFilter &filter)
    : d(const_cast<QGalleryContainerUrlFilterPrivate *>(filter.d.constData()))
{
}

/*!
    Constructs a copy of a gallery intersection \a filter.
*/

QGalleryFilter::QGalleryFilter(const QGalleryIntersectionFilter &filter)
    : d(const_cast<QGalleryIntersectionFilterPrivate *>(filter.d.constData()))
{
}

/*!
    Constructs a copy of a gallery union \a filter.
*/

QGalleryFilter::QGalleryFilter(const QGalleryUnionFilter &filter)
    : d(const_cast<QGalleryUnionFilterPrivate *>(filter.d.constData()))
{
}

/*!
    Constructs a copy of a gallery meta-data \a filter.
*/

QGalleryFilter::QGalleryFilter(const QGalleryMetaDataFilter &filter)
    : d(const_cast<QGalleryMetaDataFilterPrivate *>(filter.d.constData()))
{
}

/*!
    Constructs a copy of a gallery meta-data range \a filter.
*/

QGalleryFilter::QGalleryFilter(const QGalleryMetaDataRangeFilter &filter)
    : d(const_cast<QGalleryMetaDataRangeFilterPrivate *>(filter.d.constData()))
{
}

/*!
    Destroys a gallery filter.
*/

QGalleryFilter::~QGalleryFilter()
{
}

/*!
    Assigns the value of \a filter to another filter.
*/

QGalleryFilter &QGalleryFilter::operator =(const QGalleryFilter &filter)
{
    d = filter.d;

    return *this;
}

/*!
    Returns the type of a filter.
*/

QGalleryFilter::Type QGalleryFilter::type() const
{
    return d->type;
}

/*!
    Casts a filter to a document filter.  The filter must be of type Document
    or this will return an Invalid filter.

    Returns a QGalleryDocumentFilter.
*/

QGalleryDocumentFilter QGalleryFilter::toDocumentFilter() const
{
    return d->type == Document
            ? QGalleryDocumentFilter(const_cast<QGalleryFilterPrivate *>(d.constData()))
            : QGalleryDocumentFilter();
}

/*!
    Casts a filter to a document URL filter.  The filter must be of type
    DocumentUrl or this will return an Invalid filter.

    Returns a QGalleryDocumentUrlFilter.
*/

QGalleryDocumentUrlFilter QGalleryFilter::toDocumentUrlFilter() const
{
    return d->type == DocumentUrl
            ? QGalleryDocumentUrlFilter(const_cast<QGalleryFilterPrivate *>(d.constData()))
            : QGalleryDocumentUrlFilter();
}

/*!
    Casts a filter to a container filter.  The filter must be of type Container
    or this will return an Invalid filter.

    Returns a QGalleryContainerFilter.
*/

QGalleryContainerFilter QGalleryFilter::toContainerFilter() const
{
    return d->type == Container
            ? QGalleryContainerFilter(const_cast<QGalleryFilterPrivate *>(d.constData()))
            : QGalleryContainerFilter();
}

/*!
    Casts a filter to a container URL filter.  The filter must be of type
    ContainerUrl or this will return an Invalid filter.

    Returns a QGalleryContainerUrlFilter.
*/

QGalleryContainerUrlFilter QGalleryFilter::toContainerUrlFilter() const
{
    return d->type == ContainerUrl
            ? QGalleryContainerUrlFilter(const_cast<QGalleryFilterPrivate *>(d.constData()))
            : QGalleryContainerUrlFilter();
}

/*!
    Casts a filter to an intersection filter.  The filter must be of type
    Intersection or this will return an Invalid filter.

    Returns a QGalleryIntersectionFilter.
*/

QGalleryIntersectionFilter QGalleryFilter::toIntersectionFilter() const
{
    return d->type == Intersection
            ? QGalleryIntersectionFilter(const_cast<QGalleryFilterPrivate *>(d.constData()))
            : QGalleryIntersectionFilter();
}

/*!
    Casts a filter to a union filter.  The filter must be of type Union or this
    will return an Invalid filter.

    Returns a QGalleryUnionFilter.
*/

QGalleryUnionFilter QGalleryFilter::toUnionFilter() const
{
    return d->type == Union
            ? QGalleryUnionFilter(const_cast<QGalleryFilterPrivate *>(d.constData()))
            : QGalleryUnionFilter();
}

/*!
    Casts a filter to a meta-data filter.  The filter must be of type MetaData
    or this will return an Invalid filter.

    Returns a QGalleryMetaDataFilter.
*/

QGalleryMetaDataFilter QGalleryFilter::toMetaDataFilter() const
{
    return d && d->type == MetaData
            ? QGalleryMetaDataFilter(const_cast<QGalleryFilterPrivate *>(d.constData()))
            : QGalleryMetaDataFilter();
}

/*!
    Casts a filter to a meta-data range filter.  The filter must be of type
    MetaDataRange or this will return an Invalid filter.

    Returns a QGalleryMetaDataRangeFilter.
*/

QGalleryMetaDataRangeFilter QGalleryFilter::toMetaDataRangeFilter() const
{
    return d && d->type == MetaDataRange
            ? QGalleryMetaDataRangeFilter(const_cast<QGalleryFilterPrivate *>(d.constData()))
            : QGalleryMetaDataRangeFilter();
}

/*!
    Compares \a filter1 to filter2.

    Returns true if the filters are identical, and false otherwise.
*/

bool operator ==(const QGalleryFilter &filter1, const QGalleryFilter &filter2)
{
    return filter1.d == filter2.d || filter1.d->isEqual(*filter2.d);
}

/*!
    Compares \a filter1 to filter2.

    Returns true if the filters are not identical, and false otherwise.
*/

bool operator !=(const QGalleryFilter &filter1, const QGalleryFilter &filter2)
{
    return filter1.d != filter2.d && !filter1.d->isEqual(*filter2.d);
}

/*!
    \class QGalleryDocumentFilter

    \ingroup gallery
    \ingroup gallery-filters

    \brief The QGalleryDocumentFilter class provides a gallery filter which
    matches documents with specific IDs.
*/

/*!
    Constructs a null document filter.
*/

QGalleryDocumentFilter::QGalleryDocumentFilter()
    : d(new QGalleryDocumentFilterPrivate)
{
}

/*!
    Constructs a document filter which matches a single \a id.
*/

QGalleryDocumentFilter::QGalleryDocumentFilter(const QString &id)
    : d(new QGalleryDocumentFilterPrivate)
{
    d->documentIds.append(id);
}

/*!
    Constructs a document filter which matches a list of \a ids.
*/

QGalleryDocumentFilter::QGalleryDocumentFilter(const QStringList &ids)
    : d(new QGalleryDocumentFilterPrivate)
{
    d->documentIds = ids;
}

/*!
    Constructs a copy of a document \a filter.
*/

QGalleryDocumentFilter::QGalleryDocumentFilter(const QGalleryDocumentFilter &filter)
    : d(filter.d)
{
}

/*!
    \internal
*/

QGalleryDocumentFilter::QGalleryDocumentFilter(QGalleryFilterPrivate *d)
    : d(static_cast<QGalleryDocumentFilterPrivate *>(d))
{
}

/*!
    Destroys a document filter.
*/

QGalleryDocumentFilter::~QGalleryDocumentFilter()
{
}

/*!
    Assigns the value of \a filter to another document filter.
*/

QGalleryDocumentFilter &QGalleryDocumentFilter::operator =(const QGalleryDocumentFilter &filter)
{
    d = filter.d;

    return *this;
}

/*!
    Returns the ID of single document matched by a document filter.

    If documentIds() contains multiple IDs this will be null.
*/

QString QGalleryDocumentFilter::documentId() const
{
    return d->documentIds.count() == 1
            ? d->documentIds.first()
            : QString();
}

/*!
    Sets the \a id of a single document matched by a document filter.

    Setting this will overwrite the value of documentIds().
*/

void QGalleryDocumentFilter::setDocumentId(const QString &id)
{
    d->documentIds.clear();

    if (!id.isNull())
        d->documentIds.append(id);
}

/*!
    Returns the list of IDs matched by a document filter.
*/

QStringList QGalleryDocumentFilter::documentIds() const
{
    return d->documentIds;
}

/*!
    Sets the list of document \a ids matched by a document filter.

    Setting this will overwrite the value of documentId().
*/

void QGalleryDocumentFilter::setDocumentIds(const QStringList &ids)
{
    d->documentIds = ids;
}

/*!
    \class QGalleryDocumentUrlFilter

    \ingroup gallery
    \ingroup gallery-filters

    \brief The QGalleryDocumentUrlFilter class provides a gallery filter which
    matches documents with specific URLs.
*/

/*!
    Constructs an empty document URL filter.
*/
QGalleryDocumentUrlFilter::QGalleryDocumentUrlFilter()
    : d(new QGalleryDocumentUrlFilterPrivate)
{
}

/*!
    Constructs a document URL filter which matches a single \a url.
*/

QGalleryDocumentUrlFilter::QGalleryDocumentUrlFilter(const QUrl &url)
    : d(new QGalleryDocumentUrlFilterPrivate)
{
    d->documentUrls.append(url);
}

/*!
    Constructs a document URL filter which matches a list of \a urls.
*/

QGalleryDocumentUrlFilter::QGalleryDocumentUrlFilter(const QList<QUrl> &urls)
    : d(new QGalleryDocumentUrlFilterPrivate)
{
    d->documentUrls = urls;
}

/*!
    Constructs a copy of a document URL \a filter.
*/

QGalleryDocumentUrlFilter::QGalleryDocumentUrlFilter(const QGalleryDocumentUrlFilter &filter)
    : d(filter.d)
{
}

/*!
    \internal
*/

QGalleryDocumentUrlFilter::QGalleryDocumentUrlFilter(QGalleryFilterPrivate *d)
    : d(static_cast<QGalleryDocumentUrlFilterPrivate *>(d))
{
}

/*!
    Destroys a document URL filter.
*/

QGalleryDocumentUrlFilter::~QGalleryDocumentUrlFilter()
{
}

/*!
    Assigns the value of \a filter to another document URL filter.
*/

QGalleryDocumentUrlFilter &QGalleryDocumentUrlFilter::operator =(
        const QGalleryDocumentUrlFilter &filter)
{
    d = filter.d;

    return *this;
}

/*!
    Returns the URL of single document matched by a document filter.

    If documentUrls() contains multiple URLs this will be null.
*/

QUrl QGalleryDocumentUrlFilter::documentUrl() const
{
    return d->documentUrls.count() == 1
            ? d->documentUrls.first()
            : QUrl();
}

/*!
    Sets the \a url of a single document matched by a document filter.

    Setting this will overwrite the value of documentUrls().
*/

void QGalleryDocumentUrlFilter::setDocumentUrl(const QUrl &url)
{
    d->documentUrls.clear();

    if (!url.isEmpty())
        d->documentUrls.append(url);
}

/*!
    Returns the list of URLs matched by a document filter.
*/

QList<QUrl> QGalleryDocumentUrlFilter::documentUrls() const
{
    return d->documentUrls;
}

/*!
    Sets the list of document \a urls matched by a document filter.

    Setting this will overwrite the value of documentUrl().
*/

void QGalleryDocumentUrlFilter::setDocumentUrls(const QList<QUrl> &urls)
{
    d->documentUrls = urls;
}

/*!
    \class QGalleryContainerFilter

    \ingroup gallery
    \ingroup gallery-filters

    \brief The QGalleryContainerFilter class provides a filter which matches
    content belonging to a container document.
*/

/*!
    Constructs an empty container filter.
*/
QGalleryContainerFilter::QGalleryContainerFilter()
    : d(new QGalleryContainerFilterPrivate)
{

}

/*!
    Constructs a container filter which matches the contents of a container
    document with the given \a id.
*/

QGalleryContainerFilter::QGalleryContainerFilter(const QString &id)
    : d(new QGalleryContainerFilterPrivate)
{
    d->containerId = id;
}

/*!
    Constructs a copy of a container \a filter.
*/

QGalleryContainerFilter::QGalleryContainerFilter(const QGalleryContainerFilter &filter)
    : d(filter.d)
{
}

/*!
    \internal
*/

QGalleryContainerFilter::QGalleryContainerFilter(QGalleryFilterPrivate *d)
    : d(static_cast<QGalleryContainerFilterPrivate *>(d))
{
}

/*!
    Destroys a container filter.
*/

QGalleryContainerFilter::~QGalleryContainerFilter()
{
}

/*!
    Assigns the value of \a filter to another container filter.
*/

QGalleryContainerFilter &QGalleryContainerFilter::operator =(const QGalleryContainerFilter &filter)
{
    d = filter.d;

    return *this;
}

/*!
    Returns the ID of the container document a filter matches the contents of.
*/

QString QGalleryContainerFilter::containerId() const
{
    return d->containerId;
}

/*!
    Sets the \a id of the container document a filter should match the contents
    of.
*/

void QGalleryContainerFilter::setContainerId(const QString &id)
{
    d->containerId = id;
}

/*!
    \class QGalleryContainerUrlFilter

    \ingroup gallery
    \ingroup gallery-filters

    \brief The QGalleryContainerUrlFilter class provides a filter which matches
    content belonging to a container document identified by a URL.
*/

/*!
    Constructs an empty container URL filter.
*/

QGalleryContainerUrlFilter::QGalleryContainerUrlFilter()
    : d(new QGalleryContainerUrlFilterPrivate)
{
}

/*!
    Constructs a container filter which matches the contents of a container
    document with the given \a url.
*/

QGalleryContainerUrlFilter::QGalleryContainerUrlFilter(const QUrl &url)
    : d(new QGalleryContainerUrlFilterPrivate)
{
    d->containerUrl = url;
}

/*!
    Constructs a copy of a container URL \a filter.
*/

QGalleryContainerUrlFilter::QGalleryContainerUrlFilter(const QGalleryContainerUrlFilter &filter)
    : d(filter.d)
{
}

/*!
    \internal
*/

QGalleryContainerUrlFilter::QGalleryContainerUrlFilter(QGalleryFilterPrivate *d)
    : d(static_cast<QGalleryContainerUrlFilterPrivate *>(d))
{
}

/*!
    Destroys a container URL filter.
*/

QGalleryContainerUrlFilter::~QGalleryContainerUrlFilter()
{
}

/*!
    Assigns the value of \a filter to another container URL filter.
*/

QGalleryContainerUrlFilter &QGalleryContainerUrlFilter::operator =(
        const QGalleryContainerUrlFilter &filter)
{
    d = filter.d;

    return *this;
}

/*!
    Returns the URL of the container document a filter matches the contents of.
*/

QUrl QGalleryContainerUrlFilter::containerUrl() const
{
    return d->containerUrl;
}

/*!
    Sets the \a url of the container document a filter should match the contents
    of.
*/

void QGalleryContainerUrlFilter::setContainerUrl(const QUrl &url)
{
    d->containerUrl = url;
}

/*!
    \class QGalleryIntersectionFilter

    \ingroup gallery
    \ingroup gallery-filters

    \brief The QGalleryIntersectionFilter class provides a filter which matches
    the intersection of two or more meta-data filters.
*/

/*!
    Constructs an empty intersection.
*/

QGalleryIntersectionFilter::QGalleryIntersectionFilter()
    : d(new QGalleryIntersectionFilterPrivate)
{
}

/*!
    Constructs a copy of an intersection \a filter.
*/

QGalleryIntersectionFilter::QGalleryIntersectionFilter(const QGalleryIntersectionFilter &filter)
    : d(filter.d)
{
}

/*!
    \internal
*/

QGalleryIntersectionFilter::QGalleryIntersectionFilter(QGalleryFilterPrivate *d)
    : d(static_cast<QGalleryIntersectionFilterPrivate *>(d))
{
}

/*!
    Destroys an intersection filter.
*/

QGalleryIntersectionFilter::~QGalleryIntersectionFilter()
{
}

/*!
    Assigns the value of \a filter to another intersection filter.
*/

QGalleryIntersectionFilter &QGalleryIntersectionFilter::operator =(
        const QGalleryIntersectionFilter &filter)
{
    d = filter.d;

    return *this;
}

/*!
    Returns the number of filters in an intersection.
*/

int QGalleryIntersectionFilter::filterCount() const
{
    return d->filters.count();
}

/*!
    Returns true if an intersection does not contain any filters, and returns
    false otherwise.
*/

bool QGalleryIntersectionFilter::isEmpty() const
{
    return d->filters.isEmpty();
}

/*!
    Return the filters contained in an intersection.
*/

QList<QGalleryFilter> QGalleryIntersectionFilter::filters() const
{
    return d->filters;
}

/*!
    Appends a union \a filter to an intersection.
*/

void QGalleryIntersectionFilter::append(const QGalleryUnionFilter &filter)
{
    d->filters.append(filter);
}

/*!
    Appends a meta-data \a filter to an intersection.
*/

void QGalleryIntersectionFilter::append(const QGalleryMetaDataFilter &filter)
{
    d->filters.append(filter);
}

/*!
    Appends a meta-data range \a filter to an intersection.
*/

void QGalleryIntersectionFilter::append(const QGalleryMetaDataRangeFilter &filter)
{
    d->filters.append(filter);
}

/*!
    Appends the contents of an intersection \a filter to an intersection.
*/

void QGalleryIntersectionFilter::append(const QGalleryIntersectionFilter &filter)
{
    d->filters += filter.d->filters;
}

/*!
    Inserts a union \a filter into an intersection at \a index.
*/

void QGalleryIntersectionFilter::insert(int index, const QGalleryUnionFilter &filter)
{
    d->filters.insert(index, filter);
}

/*!
    Inserts a meta-data \a filter into an intersection \a index.
*/

void QGalleryIntersectionFilter::insert(int index, const QGalleryMetaDataFilter &filter)
{
    d->filters.insert(index, filter);
}

/*!
    Inserts a meta-data range \a filter into an intersection at \a index.
*/

void QGalleryIntersectionFilter::insert(int index, const QGalleryMetaDataRangeFilter &filter)
{
    d->filters.insert(index, filter);
}

/*!
    Inserts the contents of an intersection  \a filter into an intersection at
    \a index.
*/

void QGalleryIntersectionFilter::insert(int index, const QGalleryIntersectionFilter &filter)
{
    QList<QGalleryFilter> start = d->filters.mid(0, index);
    QList<QGalleryFilter> end = d->filters.mid(index);

    d->filters = start + filter.d->filters + end;
}

/*!
    Replaces the filter at \a index in an intersection with a union \a filter.
*/

void QGalleryIntersectionFilter::replace(int index, const QGalleryUnionFilter &filter)
{
    d->filters.replace(index, filter);
}

/*!
    Replaces the filter at \a index in an intersection with a meta-data
    \a filter.
*/

void QGalleryIntersectionFilter::replace(int index, const QGalleryMetaDataFilter &filter)
{
    d->filters.replace(index, filter);
}

/*!
    Replaces the filter at \a index in an intersection with a meta-data range
    \a filter.
*/

void QGalleryIntersectionFilter::replace(int index, const QGalleryMetaDataRangeFilter &filter)
{
    d->filters.replace(index, filter);
}

/*!
    Removes the filter at \a index from an intersection.
*/

void QGalleryIntersectionFilter::removeAt(int index)
{
    d->filters.removeAt(index);
}

/*!
    Clears all the filters from an intersection.
*/

void QGalleryIntersectionFilter::clear()
{
    d->filters.clear();
}

/*!
    \class QGalleryUnionFilter

    \ingroup gallery
    \ingroup gallery-filters

    \brief The QGalleryUnionFilter class provides a filter which matches the
    union of two or more meta-data filters.
*/

/*!
    Constructs an empty union.
*/

QGalleryUnionFilter::QGalleryUnionFilter()
    : d(new QGalleryUnionFilterPrivate)
{
}

/*!
    Constructs a copy of a union \a filter.
*/

QGalleryUnionFilter::QGalleryUnionFilter(const QGalleryUnionFilter &filter)
    : d(filter.d)
{
}

/*!
    \internal
*/

QGalleryUnionFilter::QGalleryUnionFilter(QGalleryFilterPrivate *d)
    : d(static_cast<QGalleryUnionFilterPrivate *>(d))
{
}

/*!
    Destroys a union filter.
*/

QGalleryUnionFilter::~QGalleryUnionFilter()
{
}

/*!
    Assigns the value of \a filter to another union filter.
*/

QGalleryUnionFilter &QGalleryUnionFilter::operator =(const QGalleryUnionFilter &filter)
{
    d = filter.d;

    return *this;
}

/*!
    Returns the number of filters in a union.
*/

int QGalleryUnionFilter::filterCount() const
{
    return d->filters.count();
}

/*!
    Returns true if a union contains no filters, and false otherwise.
*/

bool QGalleryUnionFilter::isEmpty() const
{
    return d->filters.isEmpty();
}

/*!
    Returns the filters contained in a union.
*/

QList<QGalleryFilter> QGalleryUnionFilter::filters() const
{
    return d->filters;
}

/*!
    Appends a meta-data \a filter to a union.
*/

void QGalleryUnionFilter::append(const QGalleryMetaDataFilter &filter)
{
    d->filters.append(filter);
}

/*!
    Appends a meta-data range \a filter to a union.
*/

void QGalleryUnionFilter::append(const QGalleryMetaDataRangeFilter &filter)
{
    d->filters.append(filter);
}

/*!
    Appends the contents of a union \a filter to a union.
*/

void QGalleryUnionFilter::append(const QGalleryUnionFilter &filter)
{
    d->filters += filter.d->filters;
}

/*!
    Inserts a meta-data \a filter into a union at \a index.
*/

void QGalleryUnionFilter::insert(int index, const QGalleryMetaDataFilter &filter)
{
    d->filters.insert(index, filter);
}

/*!
    Inserts a meta-data range \a filter into a union at \a index.
*/

void QGalleryUnionFilter::insert(int index, const QGalleryMetaDataRangeFilter &filter)
{
    d->filters.insert(index, filter);
}

/*!
    Inserts the contents of a union \a filter into a union at \a index.
*/

void QGalleryUnionFilter::insert(int index, const QGalleryUnionFilter &filter)
{
    QList<QGalleryFilter> start = d->filters.mid(0, index);
    QList<QGalleryFilter> end = d->filters.mid(index);

    d->filters = start + filter.d->filters + end;
}

/*!
    Replaces the filter at \a index in a union with a meta-data \a filter.
*/

void QGalleryUnionFilter::replace(int index, const QGalleryMetaDataFilter &filter)
{
    d->filters.replace(index, filter);
}

/*!
    Replaces the filter at \a index in a union with a meta-data range \a filter.
*/

void QGalleryUnionFilter::replace(int index, const QGalleryMetaDataRangeFilter &filter)
{
    d->filters.replace(index, filter);
}

/*!
    Removes the filter at \a index from a union.
*/

void QGalleryUnionFilter::removeAt(int index)
{
    d->filters.removeAt(index);
}

/*!
    Clears all the filters from a union.
*/

void QGalleryUnionFilter::clear()
{
    d->filters.clear();
}

/*!
    \class QGalleryMetaDataFilter

    \ingroup gallery
    \ingroup gallery-filters

    \brief The QGalleryMetaDataFilter class provides a filter which matches
    documents with meta-data fields containing a specific value.
*/

/*!
    Constructs an empty meta-data filter.
*/

QGalleryMetaDataFilter::QGalleryMetaDataFilter()
    : d(new QGalleryMetaDataFilterPrivate)
{
}

/*!
    Constructs a meta-data filter which matches documents in which the value of
    \a field matches \a value.

    The type of comparison made is determined by the match \a flags.
*/

QGalleryMetaDataFilter::QGalleryMetaDataFilter(
        const QString &field, const QVariant &value, Qt::MatchFlags flags)
            : d(new QGalleryMetaDataFilterPrivate)
{
    d->field = field;
    d->value = value;
    d->flags = flags;
}

/*!
    Constructs a copy of a meta-data \a filter.
*/

QGalleryMetaDataFilter::QGalleryMetaDataFilter(const QGalleryMetaDataFilter &filter)
    : d(filter.d)
{
}

/*!
    \internal
*/

QGalleryMetaDataFilter::QGalleryMetaDataFilter(QGalleryFilterPrivate *d)
    : d(static_cast<QGalleryMetaDataFilterPrivate *>(d))
{
}

/*!
    Destroys a meta-data filter.
*/

QGalleryMetaDataFilter::~QGalleryMetaDataFilter()
{
}

/*!
    Assigns the value of \a filter to another meta-data filter.
*/

QGalleryMetaDataFilter &QGalleryMetaDataFilter::operator =(const QGalleryMetaDataFilter &filter)
{
    d = filter.d;

    return *this;
}

/*!
    Returns the name of the field a meta-data filter is compared against.
*/

QString QGalleryMetaDataFilter::fieldName() const
{
    return d->field;
}

/*!
    Sets the \a name of the field a meta-data filter is compared against.
*/

void QGalleryMetaDataFilter::setFieldName(const QString &name)
{
    d->field = name;
}

/*!
    Returns the value a field is expected to contain for a meta-data filter to
    pass.
*/

QVariant QGalleryMetaDataFilter::value() const
{
    return d->value;
}

/*!
    Sets the \a value a field is expected to contain for a meta-data filter to
    pass.
*/

void QGalleryMetaDataFilter::setValue(const QVariant &value)
{
    d->value = value;
}

/*!
    Returns the type of comparison made by a meta-data filter.
*/

Qt::MatchFlags QGalleryMetaDataFilter::matchFlags() const
{
    return d->flags;
}

/*!
    Sets \a flags identifying the type of comparsion made by a meta-data filter.
*/

void QGalleryMetaDataFilter::setMatchFlags(Qt::MatchFlags flags)
{
    d->flags = flags;
}

/*!
    \class QGalleryMetaDataRangeFilter

    \ingroup gallery
    \ingroup gallery-filters

    \brief The QGalleryMetaDataRangeFilter class provides a filter which matches
    documents with meta-data fields with values within a specified range.
*/

/*!
    Constructs an empty meta-data range filter.
*/

QGalleryMetaDataRangeFilter::QGalleryMetaDataRangeFilter()
    : d(new QGalleryMetaDataRangeFilterPrivate)
{

}

/*!
    Constructs a copy of a meta-data range \a filter.
*/

QGalleryMetaDataRangeFilter::QGalleryMetaDataRangeFilter(const QGalleryMetaDataRangeFilter &filter)
    : d(filter.d)
{
}

/*!
    \internal
*/

QGalleryMetaDataRangeFilter::QGalleryMetaDataRangeFilter(QGalleryFilterPrivate *d)
    : d(static_cast<QGalleryMetaDataRangeFilterPrivate *>(d))
{
}

/*!
    Destroys a meta-data range filter.
*/

QGalleryMetaDataRangeFilter::~QGalleryMetaDataRangeFilter()
{
}

/*!
    Assigns the value \a filter to another meta-data range filter.
*/

QGalleryMetaDataRangeFilter &QGalleryMetaDataRangeFilter::operator =(
        const QGalleryMetaDataRangeFilter &filter)
{
    d = filter.d;

    return *this;
}

/*!
    Returns the name of the field a meta-data filter is compared against.
*/

QString QGalleryMetaDataRangeFilter::fieldName() const
{
    return d->field;
}

/*!
    Sets the \a name of the field a meta-data filter is compared against.
*/

void QGalleryMetaDataRangeFilter::setFieldName(const QString &name)
{
    d->field = name;
}

/*!
    Returns a range's minimum value.
*/

QVariant QGalleryMetaDataRangeFilter::minimumValue() const
{
    return d->minimum;
}

/*!
    Returns a range's maximum value.
*/

QVariant QGalleryMetaDataRangeFilter::maximumValue() const
{
    return d->maximum;
}

/*!
    Returns flags identifying how a value is compared to the limits of a range.
*/

QGalleryFilter::RangeFlags QGalleryMetaDataRangeFilter::rangeFlags() const
{
    return d->flags;
}

/*!
    Sets a range that matches values that are between or equal to the \a minimum
    and \a maximum values.
*/

void QGalleryMetaDataRangeFilter::setInclusiveRange(
        const QVariant &minimum, const QVariant &maximum)
{
    d->setRange(minimum, maximum, QGalleryFilter::InclusiveRange);
}

/*!
    Sets a range that matches values that are between but not equal to the
    \a minimum and \a maximum values.
*/

void QGalleryMetaDataRangeFilter::setExclusiveRange(
        const QVariant &minimum, const QVariant &maximum)
{
    d->setRange(minimum, maximum, QGalleryFilter::ExclusiveRange);
}

/*!
    Sets a range that matches values that are less than a \a value.
*/

void QGalleryMetaDataRangeFilter::setLessThan(const QVariant &value)
{
    d->setRange(QVariant(), value, QGalleryFilter::LessThanMaximum);
}

/*!
    Sets a range that matches values that are less than or equal to a \a value.
*/

void QGalleryMetaDataRangeFilter::setLessThanEquals(const QVariant &value)
{
    d->setRange(QVariant(), value, QGalleryFilter::LessThanEqualsMaximum);
}

/*!
    Sets a range that matches values that are greater than a \a value.
*/

void QGalleryMetaDataRangeFilter::setGreaterThan(const QVariant &value)
{
    d->setRange(value, QVariant(), QGalleryFilter::GreaterThanMinimum);
}

/*!
    Sets a range that matches values that are greater than or equal to a
    \a value.
*/

void QGalleryMetaDataRangeFilter::setGreaterThanEquals(const QVariant &value)
{
    d->setRange(value, QVariant(), QGalleryFilter::GreaterThanEqualsMinimum);
}
