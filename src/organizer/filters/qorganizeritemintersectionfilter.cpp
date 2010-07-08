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

#include "qorganizeritemintersectionfilter.h"
#include "qorganizeritemintersectionfilter_p.h"
#include "qorganizeritemfilter_p.h"
#include "qorganizeritemmanager.h"

QTM_BEGIN_NAMESPACE

/*!
  \class QOrganizerItemIntersectionFilter
  \brief The QOrganizerItemIntersectionFilter class provides a filter which intersects the results of other filters.
  \inmodule QtOrganizer
  \ingroup organizer-filters
 
  It may be used to select organizeritems which match all of the filters in the intersection
 */

Q_IMPLEMENT_ORGANIZERITEMFILTER_PRIVATE(QOrganizerItemIntersectionFilter);

/*!
 * \fn QOrganizerItemIntersectionFilter::QOrganizerItemIntersectionFilter(const QOrganizerItemFilter& other)
 * Constructs a copy of \a other if possible, otherwise constructs a new intersection filter
 */

/*!
 * Constructs a new intersection filter
 */
QOrganizerItemIntersectionFilter::QOrganizerItemIntersectionFilter()
    : QOrganizerItemFilter(new QOrganizerItemIntersectionFilterPrivate)
{
}

/*!
 * Sets the filters whose criteria will be intersected to \a filters
 * \sa filters()
 */
void QOrganizerItemIntersectionFilter::setFilters(const QList<QOrganizerItemFilter>& filters)
{
    Q_D(QOrganizerItemIntersectionFilter);
    d->m_filters = filters;
}

/*!
 * Prepends the given \a filter to the list of intersected filters
 * \sa append(), filters()
 */
void QOrganizerItemIntersectionFilter::prepend(const QOrganizerItemFilter& filter)
{
    Q_D(QOrganizerItemIntersectionFilter);
    d->m_filters.prepend(filter);
}

/*!
 * Appends the given \a filter to the list of intersected filters
 * \sa operator<<(), prepend(), filters()
 */
void QOrganizerItemIntersectionFilter::append(const QOrganizerItemFilter& filter)
{
    Q_D(QOrganizerItemIntersectionFilter);
    d->m_filters.append(filter);
}

/*!
 * Removes the given \a filter from the intersection list
 * \sa filters(), append(), prepend()
 */
void QOrganizerItemIntersectionFilter::remove(const QOrganizerItemFilter& filter)
{
    Q_D(QOrganizerItemIntersectionFilter);
    d->m_filters.removeAll(filter);
}

/*!
 * Appends the given \a filter to the list of intersected filters
 * \sa append()
 */
QOrganizerItemIntersectionFilter& QOrganizerItemIntersectionFilter::operator<<(const QOrganizerItemFilter& filter)
{
    Q_D(QOrganizerItemIntersectionFilter);
    d->m_filters << filter;
    return *this;
}

/*!
 * Returns the list of filters which form the intersection filter
 * \sa setFilters(), prepend(), append(), remove()
 */
QList<QOrganizerItemFilter> QOrganizerItemIntersectionFilter::filters() const
{
    Q_D(const QOrganizerItemIntersectionFilter);
    return d->m_filters;
}

QTM_END_NAMESPACE
