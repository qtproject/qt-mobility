/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
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
    \since Mobility 1.1

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
    \since Mobility 1.1

    This property holds the name of the property to filter against.
*/

/*!
    \qmlproperty variant GalleryEqualsFilter::value
    \since Mobility 1.1

    This property holds the value to filter using.
*/

/*!
    \qmlproperty bool GalleryEqualsFilter::negated
    \since Mobility 1.1

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
    \since Mobility 1.1

    \brief The GalleryLessThanFilter element provides a filter which tests
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
    \since Mobility 1.1

    This property holds the name of the property to filter against.
*/

/*!
    \qmlproperty variant GalleryLessThanFilter::value
    \since Mobility 1.1

    This property holds the value to filter using.
*/

/*!
    \qmlproperty bool GalleryLessThanFilter::negated
    \since Mobility 1.1

    This property holds whether the result of a filter should be negated.
*/


/*!
    \qmlclass GalleryLessThanEqualsFilter QDeclarativeGalleryLessThanEqualsFilter
    \since Mobility 1.1

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
    \since Mobility 1.1

    This property holds the name of the property to filter against.
*/

/*!
    \qmlproperty variant GalleryLessThanEqualsFilter::value
    \since Mobility 1.1

    This property holds the value to filter using.
*/

/*!
    \qmlproperty bool GalleryLessThanEqualsFilter::negated
    \since Mobility 1.1

    This property holds whether the result of a filter should be negated.
*/


/*!
    \qmlclass GalleryGreaterThanFilter QDeclarativeGalleryGreaterThanFilter
    \since Mobility 1.1

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
    \since Mobility 1.1

    This property holds the name of the property to filter against.
*/

/*!
    \qmlproperty variant GalleryGreaterThanFilter::value
    \since Mobility 1.1

    This property holds the value to filter using.
*/

/*!
    \qmlproperty bool GalleryGreaterThanFilter::negated
    \since Mobility 1.1

    This property holds whether the result of a filter should be negated.
*/

/*!
    \qmlclass GalleryGreaterThanEqualsFilter QDeclarativeGalleryGreaterThanEqualsFilter
    \since Mobility 1.1

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
    \since Mobility 1.1

    This property holds the name of the property to filter against.
*/

/*!
    \qmlproperty variant GalleryGreaterThanEqualsFilter::value
    \since Mobility 1.1

    This property holds the value to filter using.
*/

/*!
    \qmlproperty bool GalleryGreaterThanEqualsFilter::negated
    \since Mobility 1.1

    This property holds whether the result of a filter should be negated.
*/

/*!
    \qmlclass GalleryContainsFilter QDeclarativeGalleryContainsFilter
    \since Mobility 1.1

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
    \since Mobility 1.1

    This property holds the name of the property to filter against.
*/

/*!
    \qmlproperty variant GalleryContainsFilter::value
    \since Mobility 1.1

    This property holds the value to filter using.
*/

/*!
    \qmlproperty bool GalleryContainsFilter::negated
    \since Mobility 1.1

    This property holds whether the result of a filter should be negated.
*/

/*!
    \qmlclass GalleryStartsWithFilter QDeclarativeGalleryStartsWithFilter
    \since Mobility 1.1

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
    \since Mobility 1.1

    This property holds the name of the property to filter against.
*/

/*!
    \qmlproperty variant GalleryStartsWithFilter::value
    \since Mobility 1.1

    This property holds the value to filter using.
*/

/*!
    \qmlproperty bool GalleryStartsWithFilter::negated
    \since Mobility 1.1

    This property holds whether the result of a filter should be negated.
*/

/*!
    \qmlclass GalleryEndsWithFilter QDeclarativeGalleryEndsWithFilter

    \brief The GalleryEndsWithFilter element provides a filter which tests if a
    meta-data property ends with a string.
    \since Mobility 1.1

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
    \since Mobility 1.1

    This property holds the name of the property to filter against.
*/

/*!
    \qmlproperty variant GalleryEndsWithFilter::value
    \since Mobility 1.1

    This property holds the value to filter using.
*/

/*!
    \qmlproperty bool GalleryEndsWithFilter::negated
    \since Mobility 1.1

    This property holds whether the result of a filter should be negated.
*/

/*!
    \qmlclass GalleryWildcardFilter QDeclarativeGalleryWildcardFilter
    \since Mobility 1.1

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
    \since Mobility 1.1

    This property holds the name of the property to filter against.
*/

/*!
    \qmlproperty variant GalleryWildcardFilter::value
    \since Mobility 1.1

    This property holds the value to filter using.
*/

/*!
    \qmlproperty bool GalleryWildcardFilter::negated
    \since Mobility 1.1

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
    \since Mobility 1.1

    \ingroup qml-gallery
    \ingroup qml-gallery-filters

    This element is part of the \bold {QtMobility.gallery 1.0} module.
*/

/*!
    \qmlproperty filterlist GalleryFilterUnion::filters
    \since Mobility 1.1

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
    \since Mobility 1.1

    \ingroup qml-gallery
    \ingroup qml-gallery-filters

    This element is part of the \bold {QtMobility.gallery 1.0} module.
*/

/*!
    \qmlproperty filterlist GalleryFilterIntersection::filters
    \since Mobility 1.1

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
