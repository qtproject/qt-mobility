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

#include "galleryfilter.h"

#include <qgalleryfilter.h>

QTM_BEGIN_NAMESPACE

/*!
    \qmlclass GalleryFilter GalleryFilter

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
    \qmlproperty enum GalleryFilter::matchFlags

    This property holds how a value is filtered against a property.
*/

QGalleryFilter GalleryFilter::filter() const
{
    QGalleryMetaDataFilter filter;
    filter.setPropertyName(m_propertyName);
    filter.setValue(m_value);
    filter.setMatchFlags(Qt::MatchFlag(int(m_matchFlags)));
    return filter;
}

/*!
    \qmlclass GalleryPropertyLessThan GalleryFilterLessThan

    \brief The GalleryPropertyLessThan element provides a gallery filter which
    tests if a property value is less than a value.

    This element is part of the \bold {QtMobility.gallery 1.0} module.
*/

/*!
    \qmlproperty string GalleryPropertyLessThan::property

    This property holds the name of the property the filter tests.
*/

/*!
    \qmlproperty variant GalleryPropertyLessThan::value

    This property holds the value the filter tests against.
*/

QGalleryFilter GalleryFilterLessThan::filter() const
{
    QGalleryMetaDataRangeFilter filter;
    filter.setPropertyName(m_propertyName);
    filter.setLessThan(m_value);
    return filter;
}

/*!
    \qmlclass GalleryPropertyLessThan GalleryFilterLessEqualsThan

    \brief The GalleryPropertyLessEqualThan element provides a gallery filter
    which tests if a property value is less than or equals a value.

    This element is part of the \bold {QtMobility.gallery 1.0} module.
*/

/*!
    \qmlproperty string GalleryPropertyLessThan::property

    This property holds the name of the property the filter tests.
*/

/*!
    \qmlproperty variant GalleryPropertyLessThan::value

    This property holds the value the filter tests against.
*/

QGalleryFilter GalleryFilterLessThanEquals::filter() const
{
    QGalleryMetaDataRangeFilter filter;
    filter.setPropertyName(m_propertyName);
    filter.setLessThanEquals(m_value);
    return filter;
}

/*!
    \qmlclass GalleryPropertyGreaterThan GalleryFilterGreaterThan

    \brief The GalleryPropertyGreaterThan element provides a gallery filter
    which tests if a property value is greater than a value.

    This element is part of the \bold {QtMobility.gallery 1.0} module.
*/

/*!
    \qmlproperty string GalleryPropertyGreaterThan::property

    This property holds the name of the property the filter tests.
*/

/*!
    \qmlproperty variant GalleryPropertyGreaterThan::value

    This property holds the value the filter tests against.
*/

QGalleryFilter GalleryFilterGreaterThan::filter() const
{
    QGalleryMetaDataRangeFilter filter;
    filter.setPropertyName(m_propertyName);
    filter.setGreaterThan(m_value);
    return filter;
}

/*!
    \qmlclass GalleryPropertyGreaterThanEquals GalleryFilterGreaterThanEquals

    \brief The GalleryPropertyGreaterThanEquals element provides a gallery filter which
    tests if a property value is greater than or equals a value.

    This element is part of the \bold {QtMobility.gallery 1.0} module.
*/

/*!
    \qmlproperty string GalleryPropertyGreaterThanEquals::property

    This property holds the name of the property the filter tests.
*/

/*!
    \qmlproperty variant GalleryPropertyGreaterThanEquals::value

    This property holds the value the filter tests against.
*/

QGalleryFilter GalleryFilterGreaterThanEquals::filter() const
{
    QGalleryMetaDataRangeFilter filter;
    filter.setPropertyName(m_propertyName);
    filter.setGreaterThanEquals(m_value);
    return filter;
}

/*!
    \qmlclass GalleryPropertyExclusiveRange GalleryFilterExclusiveRange

    \brief The GalleryPropertyExclusiveRange element provides a gallery filter
    which tests if a property value is between but not equal to two values.

    This element is part of the \bold {QtMobility.gallery 1.0} module.
*/

/*!
    \qmlproperty string GalleryPropertyExclusiveRange::property

    This property holds the name of the property the filter tests.
*/

/*!
    \qmlproperty variant GalleryPropertyExclusiveRange::minimum

    This property holds the minimum value the filter tests against.
*/

/*!
    \qmlproperty variant GalleryPropertyExclusiveRange::maximum
    
    This property holds the maximum value the filter tests against.
*/

QGalleryFilter GalleryFilterExclusiveRange::filter() const
{
    QGalleryMetaDataRangeFilter filter;
    filter.setPropertyName(m_propertyName);
    filter.setExclusiveRange(m_minimumValue, m_maximumValue);
    return filter;
}

/*!
    \qmlclass GalleryInclusivePropertyRange GalleryFilterInclusiveRange

    \brief The GalleryInclusivePropertyRange element provides a gallery filter
    which tests if a property value is between or equal to two values.

    This element is part of the \bold {QtMobility.gallery 1.0} module.
*/

/*!
    \qmlproperty string GalleryInclusivePropertyRange::property

    This property holds the name of the property the filter tests.
*/

/*!
    \qmlproperty variant GalleryInclusivePropertyRange::minimum

    This property holds the minimum value the filter tests against.
*/

/*!
    \qmlproperty variant GalleryInclusivePropertyRange::maximum
    
    This property holds the maximum value the filter tests against.
*/

QGalleryFilter GalleryFilterInclusiveRange::filter() const
{
    QGalleryMetaDataRangeFilter filter;
    filter.setPropertyName(m_propertyName);
    filter.setInclusiveRange(m_minimumValue, m_maximumValue);
    return filter;
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

QGalleryFilter GalleryFilterUnion::filter() const
{
    QGalleryUnionFilter unionFilter;

    typedef QList<GalleryFilterUnionItem *>::const_iterator iterator;
    for (iterator it = m_filters.begin(), end = m_filters.end(); it != end; ++it) {
        QGalleryFilter filter = (*it)->filter();
        switch (filter.type()) {
        case QGalleryFilter::MetaData:
            unionFilter.append(filter.toMetaDataFilter());
            break;
        case QGalleryFilter::MetaDataRange:
            unionFilter.append(filter.toMetaDataRangeFilter());
            break;
        case QGalleryFilter::Union:
            unionFilter.append(filter.toUnionFilter());
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

QGalleryFilter GalleryFilterIntersection::filter() const
{
    QGalleryIntersectionFilter intersectionFilter;

    typedef QList<GalleryFilterIntersectionItem *>::const_iterator iterator;
    for (iterator it = m_filters.begin(), end = m_filters.end(); it != end; ++it) {
        QGalleryFilter filter = (*it)->filter();
        switch (filter.type()) {
        case QGalleryFilter::MetaData:
            intersectionFilter.append(filter.toMetaDataFilter());
            break;
        case QGalleryFilter::MetaDataRange:
            intersectionFilter.append(filter.toMetaDataRangeFilter());
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

#include "moc_galleryfilter.cpp"

QTM_END_NAMESPACE
