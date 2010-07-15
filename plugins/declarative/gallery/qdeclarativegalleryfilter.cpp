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

#include "qdeclarativegalleryfilter.h"

#include <qgalleryfilter.h>

QTM_BEGIN_NAMESPACE

/*!
    \qmlclass GalleryFilter QDeclarativeGalleryFilter

    \brief The GalleryFilter element provides filtering criteria for a gallery
    query.

    This element is part of the \bold {QtMobility.gallery 1.0} module.

    \qml
    GalleryFilter {
        property: "keywords"
        value: "holiday"
    }
    \endqml
*/

/*!
    \qmlproperty string GalleryFilter::property

    This property holds the name of the property to filter against.
*/

/*!
    \qmlproperty variant GalleryFilter::value

    This property holds the value to filter using.
*/

/*!
    \qmlproperty enum GalleryFilter::comparator

    This property holds the type of comparison made by a GalleryFilter.  It can
    be one of:

    \list
    \o Equals The filter tests if a meta-data property is equal to a value.
    \o LessThan The filter tests if a meta-data property is less than a
    value.
    \o GreaterThan The filter tests if a meta-data property is greater
    than a value.
    \o LessThanEquals The filter tests if a meta-data property is less than
    or equal to a value.
    \o GreaterThanEquals The filter tests if a meta-data property is
    greater than or equal to a value.
    \o Contains The filter tests if a meta-data property contains a
    sub-string.
    \o StartsWith The filter tests if a meta-data property starts with a
    string.
    \o EndsWith The filter tests if a meta-data property ends with a
    string.
    \o Wildcard The filter tests if a meta-data property matches a wildcard
    string.
    \o RegExp The filter tests if a meta-data property matches a regular
    expression.
    \endlist
*/

/*!
    \qmlproperty enum GalleryFilter::caseSensitivity

    This property holds whether the comparison made by a GalleryFilter should
    be case sensitive.  It can be one of:

    \list
    \o Qt.CaseSensitive The comparison is case sensitive.
    \o Qt.CaseInsensitive The comparison is not case case sensitive.
    \endlist
*/

/*!
    \qmlproperty bool GalleryFilter::inverted

    This property holds whether the result of a GalleryFilter should be
    inverted.
*/

QGalleryFilter QDeclarativeGalleryFilter::filter() const
{
    return m_filter;
}

/*!
    \qmlclass GalleryFilterUnion GalleryFilterUnion

    \brief The GalleryFilterUnion elements provides a union of gallery filters.

    This element is part of the \bold {QtMobility.gallery 1.0} module.
*/

/*!
    \qmlproperty filterlist GalleryFilterUnion::filters

    This property holds the children of a union filter.
*/

QGalleryFilter QDeclarativeGalleryFilterUnion::filter() const
{
    QGalleryUnionFilter unionFilter;

    typedef QList<QDeclarativeGalleryFilterBase *>::const_iterator iterator;
    for (iterator it = m_filters.begin(), end = m_filters.end(); it != end; ++it) {
        QGalleryFilter filter = (*it)->filter();
        switch (filter.type()) {
        case QGalleryFilter::MetaData:
            unionFilter.append(filter.toMetaDataFilter());
            break;
        case QGalleryFilter::Union:
            unionFilter.append(filter.toUnionFilter());
            break;
        case QGalleryFilter::Intersection:
            unionFilter.append(filter.toIntersectionFilter());
            break;
        default:
            break;
        }
    }
    return unionFilter;
}

/*!
    \qmlclass GalleryFilterIntersection GalleryFilterIntersection

    \brief The GalleryFilterIntersection elements provides a intersection of
    gallery filters.

    This element is part of the \bold {QtMobility.gallery 1.0} module.
*/

/*!
    \qmlproperty filterlist GalleryFilterIntersection::filters

    This property holds the children of a intersection filter.
*/

QGalleryFilter QDeclarativeGalleryFilterIntersection::filter() const
{
    QGalleryIntersectionFilter intersectionFilter;

    typedef QList<QDeclarativeGalleryFilterBase *>::const_iterator iterator;
    for (iterator it = m_filters.begin(), end = m_filters.end(); it != end; ++it) {
        QGalleryFilter filter = (*it)->filter();
        switch (filter.type()) {
        case QGalleryFilter::MetaData:
            intersectionFilter.append(filter.toMetaDataFilter());
            break;
        case QGalleryFilter::Union:
            intersectionFilter.append(filter.toUnionFilter());
            break;
        case QGalleryFilter::Intersection:
            intersectionFilter.append(filter.toIntersectionFilter());
            break;
        default:
            break;
        }
    }
    return intersectionFilter;
}

#include "moc_qdeclarativegalleryfilter.cpp"

QTM_END_NAMESPACE
