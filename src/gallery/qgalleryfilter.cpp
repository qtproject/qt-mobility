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
{
}

/*!
    Constructs a copy of a gallery \a filter.
*/

QGalleryFilter::QGalleryFilter(const QGalleryFilter &filter)
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

}

/*!
    Compares \a filter to another filter.

    Returns true if the filters are identical, and false otherwise.
*/

bool QGalleryFilter::operator ==(const QGalleryFilter &filter) const
{

}

/*!
    Compares \a filter to another filter.

    Returns true if the filters are not identical, and false otherwise.
*/

bool QGalleryFilter::operator !=(const QGalleryFilter &filter) const
{

}

/*!
    Returns the type of a filter.
*/

QGalleryFilter::Type QGalleryFilter::type() const
{

}

/*!
    Identifies if a filter has any criteria set.

    Returns true if the filter is empty, and false otherwise.
*/

bool QGalleryFilter::isEmpty() const
{

}

/*!
    Casts a filter to a document filter.  The filter must be of type Document
    or this will return an Invalid filter.

    Returns a QGalleryDocumentFilter.
*/

QGalleryDocumentFilter QGalleryFilter::toDocumentFilter() const
{

}

/*!
    Casts a filter to a document URL filter.  The filter must be of type
    DocumentUrl or this will return an Invalid filter.

    Returns a QGalleryDocumentUrlFilter.
*/

QGalleryDocumentUrlFilter QGalleryFilter::toDocumentUrlFilter() const
{

}

/*!
    Casts a filter to a container filter.  The filter must be of type Container
    or this will return an Invalid filter.

    Returns a QGalleryContainerFilter.
*/

QGalleryContainerFilter QGalleryFilter::toContainerFilter() const
{

}

/*!
    Casts a filter to a container URL filter.  The filter must be of type
    ContainerUrl or this will return an Invalid filter.

    Returns a QGalleryContainerUrlFilter.
*/

QGalleryContainerUrlFilter QGalleryFilter::toContainerUrlFilter() const
{

}

/*!
    Casts a filter to an intersection filter.  The filter must be of type
    Intersection or this will return an Invalid filter.

    Returns a QGalleryIntersectionFilter.
*/

QGalleryIntersectionFilter QGalleryFilter::toIntersectionFilter() const
{

}

/*!
    Casts a filter to a union filter.  The filter must be of type Union or this
    will return an Invalid filter.

    Returns a QGalleryUnionFilter.
*/

QGalleryUnionFilter QGalleryFilter::toUnionFilter() const
{

}

/*!
    Casts a filter to a meta-data filter.  The filter must be of type MetaData
    or this will return an Invalid filter.

    Returns a QGalleryMetaDataFilter.
*/

QGalleryMetaDataFilter QGalleryFilter::toMetaDataFilter() const
{

}

/*!
    Casts a filter to a meta-data range filter.  The filter must be of type
    MetaDataRange or this will return an Invalid filter.

    Returns a QGalleryMetaDataRangeFilter.
*/

QGalleryMetaDataRangeFilter QGalleryFilter::toMetaDataRangeFilter() const
{

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
{

}

/*!
    Constructs a document filter which matches a single \a id.
*/

QGalleryDocumentFilter::QGalleryDocumentFilter(const QString &id)
{

}

/*!
    Constructs a document filter which matches a list of \a ids.
*/

QGalleryDocumentFilter::QGalleryDocumentFilter(const QStringList &ids)
{

}

/*!
    Constructs a copy of a document \a filter.
*/

QGalleryDocumentFilter::QGalleryDocumentFilter(const QGalleryDocumentIdFilter &filter)
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

QGalleryDocumentFilter &QGalleryDocumentFilter::operator =(const QGalleryDocumentIdFilter &filter)
{

}

/*!
    Returns the ID of single document matched by a document filter.

    If documentIds() contains multiple IDs this will be null.
*/

QString QGalleryDocumentFilter::documentId() const
{

}

/*!
    Sets the \a id of a single document matched by a document filter.

    Setting this will overwrite the value of documentIds().
*/

void QGalleryDocumentFilter::setDocumentId(const QString &id)
{

}

/*!
    Returns the list of IDs matched by a document filter.
*/

QStringList QGalleryDocumentFilter::documentIds() const
{

}

/*!
    Sets the list of document \a ids matched by a document filter.

    Setting this will overwrite the value of documentId().
*/

void QGalleryDocumentFilter::setDocumentIds(const QStringList &ids)
{

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
{

}

/*!
    Constructs a document URL filter which matches a single \a url.
*/

QGalleryDocumentUrlFilter::QGalleryDocumentUrlFilter(const QUrl &url)
{

}

/*!
    Constructs a document URL filter which matches a list of \a urls.
*/

QGalleryDocumentUrlFilter::QGalleryDocumentUrlFilter(const QList<QUrl> &urls)
{

}

/*!
    Constructs a copy of a document URL \a filter.
*/

QGalleryDocumentUrlFilter::QGalleryDocumentUrlFilter(const QGalleryDocumentUrlFilter &filter)
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

}

/*!
    Returns the URL of single document matched by a document filter.

    If documentUrls() contains multiple URLs this will be null.
*/

QUrl QGalleryDocumentUrlFilter::documentUrl() const
{

}

/*!
    Sets the \a url of a single document matched by a document filter.

    Setting this will overwrite the value of documentUrls().
*/

void QGalleryDocumentUrlFilter::setDocumentUrl(const QUrl &url)
{

}

/*!
    Returns the list of URLs matched by a document filter.
*/

QList<QUrl> QGalleryDocumentUrlFilter::documentUrls() const
{

}

/*!
    Sets the list of document \a urls matched by a document filter.

    Setting this will overwrite the value of documentUrl().
*/

void QGalleryDocumentUrlFilter::setDocumentUrls(const QList<QUrl> &urls)
{

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
{

}

/*!
    Constructs a container filter which matches the contents of a container
    document with the given \a id.
*/

QGalleryContainerFilter::QGalleryContainerFilter(const QString &id)
{

}

/*!
    Constructs a copy of a container \a filter.
*/

QGalleryContainerFilter::QGalleryContainerFilter(const QGalleryDocumentIdFilter &filter)
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

}

/*!
    Returns the ID of the container document a filter matches the contents of.
*/

QString QGalleryContainerFilter::containerId() const
{

}

/*!
    Sets the \a id of the container document a filter should match the contents
    of.
*/

void QGalleryContainerFilter::setContainerId(const QString &id)
{

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
{

}

/*!
    Constructs a container filter which matches the contents of a container
    document with the given \a url.
*/

QGalleryContainerUrlFilter::QGalleryContainerUrlFilter(const QUrl &url)
{

}

/*!
    Constructs a copy of a container URL \a filter.
*/

QGalleryContainerUrlFilter::QGalleryContainerUrlFilter(const QGalleryContainerUrlFilter &filter)
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

}

/*!
    Returns the URL of the container document a filter matches the contents of.
*/

QUrl QGalleryContainerUrlFilter::containerUrl() const
{

}

/*!
    Sets the \a url of the container document a filter should match the contents
    of.
*/

void QGalleryContainerUrlFilter::setContainerUrl(const QUrl &url)
{

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
{

}

/*!
    Constructs a copy of an intersection \a filter.
*/

QGalleryIntersectionFilter::QGalleryIntersectionFilter(const QGalleryIntersectionFilter &filter)
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

}

/*!
    Returns the number of filters in an intersection.
*/

int QGalleryIntersectionFilter::filterCount() const
{

}

/*!
    Return the filters contained in an intersection.
*/

QList<QGalleryFilter> QGalleryIntersectionFilter::filters() const
{

}

/*!
    Appends a union \a filter to an intersection.
*/

void QGalleryIntersectionFilter::append(const QGalleryUnionFilter &filter)
{

}

/*!
    Appends a meta-data \a filter to an intersection.
*/

void QGalleryIntersectionFilter::append(const QGalleryMetaDataFilter &filter)
{

}

/*!
    Appends a meta-data range \a filter to an intersection.
*/

void QGalleryIntersectionFilter::append(const QGalleryMetaDataRangeFilter &filter)
{

}

/*!
    Appends the contents of an intersection \a filter to an intersection.
*/

void QGalleryIntersectionFilter::append(const QGalleryIntersectionFilter &filter)
{

}

/*!
    Inserts a union \a filter into an intersection at \a index.
*/

void QGalleryIntersectionFilter::insert(int index, const QGalleryUnionFilter &filter)
{

}

/*!
    Inserts a meta-data \a filter into an intersection \a index.
*/

void QGalleryIntersectionFilter::insert(int index, const QGalleryMetaDataFilter &filter)
{

}

/*!
    Inserts a meta-data range \a filter into an intersection at \a index.
*/

void QGalleryIntersectionFilter::insert(int index, const QGalleryMetaDataRangeFilter &filter)
{

}

/*!
    Inserts the contents of an intersection  \a filter into an intersection at
    \a index.
*/

void QGalleryIntersectionFilter::insert(int index, const QGalleryIntersectionFilter &filter)
{

}

/*!
    Replaces the filter at \a index in an intersection with a union \a filter.
*/

void QGalleryIntersectionFilter::replace(int index, const QGalleryUnionFilter &filter)
{

}

/*!
    Replaces the filter at \a index in an intersection with a meta-data
    \a filter.
*/

void QGalleryIntersectionFilter::replace(int index, const QGalleryMetaDataFilter &filter)
{


}

/*!
    Replaces the filter at \a index in an intersection with a meta-data range
    \a filter.
*/

void QGalleryIntersectionFilter::replace(int index, const QGalleryMetaDataRangeFilter &filter)
{

}

/*!
    Removes the filter at \a index from an intersection.
*/

void QGalleryIntersectionFilter::removeAt(int index)
{

}

/*!
    Clears all the filters from an intersection.
*/

void QGalleryIntersectionFilter::clear()
{

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
{

}

/*!
    Constructs a copy of a union \a filter.
*/

QGalleryUnionFilter::QGalleryUnionFilter(const QGalleryUnionFilter &filter)
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

}

/*!
    Returns the number of filters in a union.
*/

int QGalleryUnionFilter::filterCount() const
{

}

/*!
    Returns the filters contained in a union.
*/

QList<QGalleryFilter> QGalleryUnionFilter::filters() const
{

}

/*!
    Appends a meta-data \a filter to a union.
*/

void QGalleryUnionFilter::append(const QGalleryMetaDataFilter &filter)
{

}

/*!
    Appends a meta-data range \a filter to a union.
*/

void QGalleryUnionFilter::append(const QGalleryMetaDataRangeFilter &filter)
{

}

/*!
    Appends the contents of a union \a filter to a union.
*/

void QGalleryUnionFilter::append(const QGalleryUnionFilter &filter)
{

}

/*!
    Inserts a meta-data \a filter into a union at \a index.
*/

void QGalleryUnionFilter::insert(int index, const QGalleryMetaDataFilter &filter)
{

}

/*!
    Inserts a meta-data range \a filter into a union at \a index.
*/

void QGalleryUnionFilter::insert(int index, const QGalleryMetaDataRangeFilter &filter)
{

}

/*!
    Inserts the contents of a union \a filter into a union at \a index.
*/

void QGalleryUnionFilter::insert(int index, const QGalleryUnionFilter &filter)
{

}

/*!
    Replaces the filter at \a index in a union with a meta-data \a filter.
*/

void QGalleryUnionFilter::replace(int index, const QGalleryMetaDataFilter &filter)
{

}

/*!
    Replaces the filter at \a index in a union with a meta-data range \a filter.
*/

void QGalleryUnionFilter::replace(int index, const QGalleryMetaDataRangeFilter &filter)
{

}

/*!
    Removes the filter at \a index from a union.
*/

void QGalleryUnionFilter::removeAt(int index)
{

}

/*!
    Clears all the filters from a union.
*/

void QGalleryUnionFilter::clear()
{

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
{

}

/*!
    Constructs a meta-data filter which matches documents in which the value of
    \a field matches \a value.

    The type of comparison made is determined by the match \a flags.
*/

QGalleryMetaDataFilter::QGalleryMetaDataFilter(
        const QString &field, const QVariant &value, Qt::MatchFlags flags)
{

}

/*!
    Constructs a copy of a meta-data \a filter.
*/

QGalleryMetaDataFilter::QGalleryMetaDataFilter(const QGalleryMetaDataFilter &filter)
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

}

/*!
    Returns the name of the field a meta-data filter is compared against.
*/

QString QGalleryMetaDataFilter::fieldName() const
{

}

/*!
    Sets the \a name of the field a meta-data filter is compared against.
*/

void QGalleryMetaDataFilter::setFieldName(const QString &name)
{

}

/*!
    Returns the value a field is expected to contain for a meta-data filter to
    pass.
*/

QVariant QGalleryMetaDataFilter::value() const
{

}

/*!
    Sets the \a value a field is expected to contain for a meta-data filter to
    pass.
*/

void QGalleryMetaDataFilter::setValue(const QVariant &value)
{

}

/*!
    Returns the type of comparison made by a meta-data filter.
*/

Qt::MatchFlags QGalleryMetaDataFilter::matchFlags() const
{

}

/*!
    Sets \a flags identifying the type of comparsion made by a meta-data filter.
*/

void QGalleryMetaDataFilter::setMatchFlags(Qt::MatchFlags flags)
{

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
{

}

/*!
    Constructs a copy of a meta-data range \a filter.
*/

QGalleryMetaDataRangeFilter::QGalleryMetaDataRangeFilter(const QGalleryMetaDataRangeFilter &filter)
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

}

/*!
    Returns the name of the field a meta-data filter is compared against.
*/

QString QGalleryMetaDataRangeFilter::fieldName() const
{

}

/*!
    Sets the \a name of the field a meta-data filter is compared against.
*/

void QGalleryMetaDataRangeFilter::setFieldName(const QString &name)
{

}

/*!
    Returns a range's minimum value.
*/

QVariant QGalleryMetaDataRangeFilter::minimumValue() const
{

}

/*!
    Returns a range's maximum value.
*/

QVariant QGalleryMetaDataRangeFilter::maximumValue() const
{

}

/*!
    Returns flags identifying how a value is compared to the limits of a range.
*/

QGalleryFilter::RangeFlags QGalleryMetaDataRangeFilter::rangeFlags() const
{

}

/*!
    Sets a range that matches values that are between or equal to the \a minimum
    and \a maximum values.
*/

void QGalleryMetaDataRangeFilter::setInclusiveRange(
        const QVariant &minimum, const QVariant &maximum)
{

}

/*!
    Sets a range that matches values that are between but not equal to the
    \a minimum and \a maximum values.
*/

void QGalleryMetaDataRangeFilter::setExclusiveRange(
        const QVariant &minimum, const QVariant &maximum)
{

}

/*!
    Sets a range that matches values that are less than a \a value.
*/

void QGalleryMetaDataRangeFilter::setLessThan(const QVariant &value)
{

}

/*!
    Sets a range that matches values that are less than or equal to a \a value.
*/

void QGalleryMetaDataRangeFilter::setLessThanEquals(const QVariant &value)
{

}

/*!
    Sets a range that matches values that are greater than a \a value.
*/

void QGalleryMetaDataRangeFilter::setGreaterThan(const QVariant &value)
{

}

/*!
    Sets a range that matches values that are greater than or equal to a
    \a value.
*/

void QGalleryMetaDataRangeFilter::setGreaterThanEquals(const QVariant &value)
{

}
