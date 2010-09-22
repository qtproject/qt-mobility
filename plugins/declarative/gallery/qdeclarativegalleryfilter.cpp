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

void QDeclarativeGalleryValueFilter::setValue(const QVariant &value)
{
    if (value != m_filter.value()) {
        m_filter.setValue(value);

        emit valueChanged();
        emit filterChanged();
    }
}
void QDeclarativeGalleryValueFilter::setPropertyName(const QString &name)
{
    if (name != m_filter.propertyName()) {
        m_filter.setPropertyName(name);

        emit propertyNameChanged();
        emit filterChanged();
    }
}

void QDeclarativeGalleryValueFilter::setNegated(bool negated)
{
    if (negated != m_filter.isNegated()) {
        m_filter.setNegated(negated);

        emit negatedChanged();
        emit filterChanged();
    }
}

QGalleryFilter QDeclarativeGalleryValueFilter::filter() const
{
    return m_filter;
}

void QDeclarativeGalleryStringFilter::setPropertyName(const QString &name)
{
    if (name != m_filter.propertyName()) {
        m_filter.setPropertyName(name);

        emit propertyNameChanged();
        emit filterChanged();
    }
}

void QDeclarativeGalleryStringFilter::setValue(const QString &value)
{
    if (value != m_filter.value()) {
        m_filter.setValue(value);

        emit valueChanged();
        emit filterChanged();
    }
}

void QDeclarativeGalleryStringFilter::setNegated(bool negated)
{
    if (negated != m_filter.isNegated()) {
        m_filter.setNegated(negated);

        emit negatedChanged();
        emit filterChanged();
    }
}

QGalleryFilter QDeclarativeGalleryStringFilter::filter() const
{
    return m_filter;
}

/*!
    \qmlclass GalleryEqualsFilter QDeclarativeGalleryEqualsFilter

    \brief The GalleryEqualsFilter element provides a filter which tests
    if a meta-data property is equal to a value.

    \ingroup qml-gallery
    \ingroup qml-gallery-filters
    
    This element is part of the \bold {QtMobility.gallery 1.0} module.

    \qml
    GalleryEqualsFilter {
        property: "keywords"
        value: "holiday"
    }
    \endqml
*/

/*!
    \qmlproperty string GalleryEqualsFilter::property

    This property holds the name of the property to filter against.
*/

/*!
    \qmlproperty variant GalleryEqualsFilter::value

    This property holds the value to filter using.
*/

/*!
    \qmlproperty bool GalleryEqualsFilter::negated

    This property holds whether the result of a filter should be negated.
*/

QGalleryFilter QDeclarativeGalleryEqualsFilter::filter() const
{
    if (m_filter.value().type() == QVariant::RegExp) {
        QGalleryMetaDataFilter filter(m_filter);

        filter.setComparator(QGalleryFilter::RegExp);

        return filter;
    } else {
        return m_filter;
    }
}

/*!
    \qmlclass GalleryLessThanFilter QDeclarativeGalleryLessThanFilter

    \brief The GalleryLessThanEqualsFilter element provides a filter which tests
    if a meta-data property is less than a value.

    \ingroup qml-gallery
    \ingroup qml-gallery-filters

    This element is part of the \bold {QtMobility.gallery 1.0} module.

    \qml
    GalleryLessThanFilter {
        property: "lastAccessed"
        value: 2008-01-01T00:00:00
    }
    \endqml
*/

/*!
    \qmlproperty string GalleryLessThanFilter::property

    This property holds the name of the property to filter against.
*/

/*!
    \qmlproperty variant GalleryLessThanFilter::value

    This property holds the value to filter using.
*/

/*!
    \qmlproperty bool GalleryLessThanFilter::negated

    This property holds whether the result of a filter should be negated.
*/


/*!
    \qmlclass GalleryLessThanEqualsFilter QDeclarativeGalleryLessThanEqualsFilter

    \brief The GalleryLessThanEqualsFilter element provides a filter which tests
    if a meta-data property is less than or equal to a value.

    \ingroup qml-gallery
    \ingroup qml-gallery-filters

    This element is part of the \bold {QtMobility.gallery 1.0} module.

    \qml
    GalleryLessThanEqualsFilter {
        property: "rating"
        value: 3.5
    }
    \endqml
*/

/*!
    \qmlproperty string GalleryLessThanEqualsFilter::property

    This property holds the name of the property to filter against.
*/

/*!
    \qmlproperty variant GalleryLessThanEqualsFilter::value

    This property holds the value to filter using.
*/

/*!
    \qmlproperty bool GalleryLessThanEqualsFilter::negated

    This property holds whether the result of a filter should be negated.
*/


/*!
    \qmlclass GalleryGreaterThanFilter QDeclarativeGalleryGreaterThanFilter

    \brief The GalleryGreaterThanFilter element provides a filter which tests
    if a meta-data property is greater than a value.

    \ingroup qml-gallery
    \ingroup qml-gallery-filters

    This element is part of the \bold {QtMobility.gallery 1.0} module.

    \qml
    GalleryGreaterThanFilter {
        property: "rating"
        value: 3.5
    }
    \endqml
*/

/*!
    \qmlproperty string GalleryGreaterThanFilter::property

    This property holds the name of the property to filter against.
*/

/*!
    \qmlproperty variant GalleryGreaterThanFilter::value

    This property holds the value to filter using.
*/

/*!
    \qmlproperty bool GalleryGreaterThanFilter::negated

    This property holds whether the result of a filter should be negated.
*/

/*!
    \qmlclass GalleryGreaterThanEqualsFilter QDeclarativeGalleryGreaterThanEqualsFilter

    \brief The GalleryGreaterThanEqualsFilter element provides a filter which
    tests if a meta-data property is greater than or equal to a value.

    \ingroup qml-gallery
    \ingroup qml-gallery-filters

    This element is part of the \bold {QtMobility.gallery 1.0} module.

    \qml
    GalleryGreaterThanEqualsFilter {
        property: "lastAccessed"
        value: 2008-01-01T00:00:00
    }
    \endqml
*/

/*!
    \qmlproperty string GalleryGreaterThanEqualsFilter::property

    This property holds the name of the property to filter against.
*/

/*!
    \qmlproperty variant GalleryGreaterThanEqualsFilter::value

    This property holds the value to filter using.
*/

/*!
    \qmlproperty bool GalleryGreaterThanEqualsFilter::negated

    This property holds whether the result of a filter should be negated.
*/

/*!
    \qmlclass GalleryContainsFilter QDeclarativeGalleryContainsFilter

    \brief The GalleryContainsFilter element provides a filter which tests if
    a meta-data property contains a string.

    \ingroup qml-gallery
    \ingroup qml-gallery-filters

    This element is part of the \bold {QtMobility.gallery 1.0} module.

    \qml
    GalleryContainsFilter {
        property: "title"
        value: "excellent"
    }
    \endqml
*/

/*!
    \qmlproperty string GalleryContainsFilter::property

    This property holds the name of the property to filter against.
*/

/*!
    \qmlproperty variant GalleryContainsFilter::value

    This property holds the value to filter using.
*/

/*!
    \qmlproperty bool GalleryContainsFilter::negated

    This property holds whether the result of a filter should be negated.
*/

/*!
    \qmlclass GalleryStartsWithFilter QDeclarativeGalleryStartsWithFilter

    \brief The GalleryStartsWithFilter element provides a filter which tests if
    a meta-data property starts with a string.

    \ingroup qml-gallery
    \ingroup qml-gallery-filters

    This element is part of the \bold {QtMobility.gallery 1.0} module.

    \qml
    GalleryStartsWithFilter {
        property: "title"
        value: "the"
    }
    \endqml
*/

/*!
    \qmlproperty string GalleryStartsWithFilter::property

    This property holds the name of the property to filter against.
*/

/*!
    \qmlproperty variant GalleryStartsWithFilter::value

    This property holds the value to filter using.
*/

/*!
    \qmlproperty bool GalleryStartsWithFilter::negated

    This property holds whether the result of a filter should be negated.
*/

/*!
    \qmlclass GalleryEndsWithFilter QDeclarativeGalleryEndsWithFilter

    \brief The GalleryEndsWithFilter element provides a filter which tests if a
    meta-data property ends with a string.

    \ingroup qml-gallery
    \ingroup qml-gallery-filters

    This element is part of the \bold {QtMobility.gallery 1.0} module.

    \qml
    GalleryEndsWithFilter {
        property: "title"
        value: ", the"
    }
    \endqml
*/

/*!
    \qmlproperty string GalleryEndsWithFilter::property

    This property holds the name of the property to filter against.
*/

/*!
    \qmlproperty variant GalleryEndsWithFilter::value

    This property holds the value to filter using.
*/

/*!
    \qmlproperty bool GalleryEndsWithFilter::negated

    This property holds whether the result of a filter should be negated.
*/

/*!
    \qmlclass GalleryWildcardFilter QDeclarativeGalleryWildcardFilter

    \brief The GalleryWildcardFilter element provides a filter which tests a
    meta-data property against a value using wildcard matching.

    \ingroup qml-gallery
    \ingroup qml-gallery-filters

    This element is part of the \bold {QtMobility.gallery 1.0} module.

    \qml
    GalleryWildcardFilter {
        property: "fileName"
        value: "*.png"
    }
    \endqml
*/

/*!
    \qmlproperty string GalleryWildcardFilter::property

    This property holds the name of the property to filter against.
*/

/*!
    \qmlproperty variant GalleryWildcardFilter::value

    This property holds the value to filter using.
*/

/*!
    \qmlproperty bool GalleryWildcardFilter::negated

    This property holds whether the result of a filter should be negated.
*/


void QDeclarativeGalleryFilterGroup::classBegin()
{
}

void QDeclarativeGalleryFilterGroup::componentComplete()
{
    m_complete = true;

    typedef QList<QDeclarativeGalleryFilterBase *>::const_iterator iterator;
    for (iterator it = m_filters.constBegin(), end = m_filters.constEnd(); it != end; ++it)
        connect(*it, SIGNAL(filterChanged()), this, SIGNAL(filterChanged()));
}

QDeclarativeListProperty<QDeclarativeGalleryFilterBase> QDeclarativeGalleryFilterGroup::filters()
{
    return QDeclarativeListProperty<QDeclarativeGalleryFilterBase>(
            this, &m_filters, append, count, at, clear);
}

void QDeclarativeGalleryFilterGroup::append(
        QDeclarativeListProperty<QDeclarativeGalleryFilterBase> *filters,
        QDeclarativeGalleryFilterBase *filter)
{
    QDeclarativeGalleryFilterGroup *filterGroup
            = static_cast<QDeclarativeGalleryFilterGroup *>(filters->object);

    static_cast<QList<QDeclarativeGalleryFilterBase *>*>(filters->data)->append(filter);

    if (static_cast<QDeclarativeGalleryFilterGroup *>(filters->object)->m_complete) {
        connect(filter, SIGNAL(filterChanged()), filterGroup, SIGNAL(filterChanged()));

        emit filterGroup->filterChanged();
    }
}

int QDeclarativeGalleryFilterGroup::count(
        QDeclarativeListProperty<QDeclarativeGalleryFilterBase> *filters)
{
    return static_cast<QList<QDeclarativeGalleryFilterBase *>*>(filters->data)->count();
}

QDeclarativeGalleryFilterBase *QDeclarativeGalleryFilterGroup::at(
        QDeclarativeListProperty<QDeclarativeGalleryFilterBase> *filters, int index)
{
    return static_cast<QList<QDeclarativeGalleryFilterBase *>*>(filters->data)->at(index);
}

void QDeclarativeGalleryFilterGroup::clear(
        QDeclarativeListProperty<QDeclarativeGalleryFilterBase> *filters)
{
    QDeclarativeGalleryFilterGroup *filterGroup
            = static_cast<QDeclarativeGalleryFilterGroup *>(filters->object);

    QList<QDeclarativeGalleryFilterBase *> *list
            = static_cast<QList<QDeclarativeGalleryFilterBase *>*>(filters->data);

    typedef QList<QDeclarativeGalleryFilterBase *>::const_iterator iterator;
    for (iterator it = list->constBegin(), end = list->constEnd(); it != end; ++it)
        disconnect(*it, SIGNAL(filterChanged()), filterGroup, SIGNAL(filterChanged()));

    list->clear();

    emit filterGroup->filterChanged();
}

/*!
    \qmlclass GalleryFilterUnion GalleryFilterUnion

    \brief The GalleryFilterUnion elements provides a union of gallery filters.

    \ingroup qml-gallery
    \ingroup qml-gallery-filters

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

    \ingroup qml-gallery
    \ingroup qml-gallery-filters

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
