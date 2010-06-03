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

QGalleryFilter GalleryFilter::filter() const
{
    QGalleryMetaDataFilter filter;
    filter.setPropertyName(m_propertyName);
    filter.setValue(m_value);
    filter.setMatchFlags(Qt::MatchFlag(int(m_matchFlags)));
    return filter;
}

QGalleryFilter GalleryFilterLessThan::filter() const
{
    QGalleryMetaDataRangeFilter filter;
    filter.setPropertyName(m_propertyName);
    filter.setLessThan(m_value);
    return filter;
}

QGalleryFilter GalleryFilterLessThanEquals::filter() const
{
    QGalleryMetaDataRangeFilter filter;
    filter.setPropertyName(m_propertyName);
    filter.setLessThanEquals(m_value);
    return filter;
}

QGalleryFilter GalleryFilterGreaterThan::filter() const
{
    QGalleryMetaDataRangeFilter filter;
    filter.setPropertyName(m_propertyName);
    filter.setGreaterThan(m_value);
    return filter;
}

QGalleryFilter GalleryFilterGreaterThanEquals::filter() const
{
    QGalleryMetaDataRangeFilter filter;
    filter.setPropertyName(m_propertyName);
    filter.setGreaterThanEquals(m_value);
    return filter;
}

QGalleryFilter GalleryFilterExclusiveRange::filter() const
{
    QGalleryMetaDataRangeFilter filter;
    filter.setPropertyName(m_propertyName);
    filter.setExclusiveRange(m_minimumValue, m_maximumValue);
    return filter;
}

QGalleryFilter GalleryFilterInclusiveRange::filter() const
{
    QGalleryMetaDataRangeFilter filter;
    filter.setPropertyName(m_propertyName);
    filter.setInclusiveRange(m_minimumValue, m_maximumValue);
    return filter;
}

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
