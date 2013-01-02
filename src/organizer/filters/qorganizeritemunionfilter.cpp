/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qorganizeritemunionfilter.h"
#include "qorganizeritemunionfilter_p.h"
#include "qorganizeritemfilter_p.h"
#include "qorganizermanager.h"

QTM_BEGIN_NAMESPACE

/*!
   \class QOrganizerItemUnionFilter
   \brief The QOrganizerItemUnionFilter class provides a filter which unions the
    results of other filters.
   \inmodule QtOrganizer
   \ingroup organizer-filters
   \since 1.1

   It may be used to select organizeritems which match all of the filters in the union
 */

Q_IMPLEMENT_ORGANIZERITEMFILTER_PRIVATE(QOrganizerItemUnionFilter);

/*!
 * \fn QOrganizerItemUnionFilter::QOrganizerItemUnionFilter(const QOrganizerItemFilter& other)
 * Constructs a copy of \a other if possible, otherwise constructs a new union filter
   \since 1.1
 */

/*!
 * Constructs a new intersection filter
   \since 1.1
 */
QOrganizerItemUnionFilter::QOrganizerItemUnionFilter()
    : QOrganizerItemFilter(new QOrganizerItemUnionFilterPrivate)
{
}

/*!
 * Sets the filters whose criteria will be unioned to \a filters
 * \sa filters()
   \since 1.1
 */
void QOrganizerItemUnionFilter::setFilters(const QList<QOrganizerItemFilter>& filters)
{
    Q_D(QOrganizerItemUnionFilter);
    d->m_filters = filters;
}

/*!
 * Prepends the given \a filter to the list of unioned filters
 * \sa append(), filters()
   \since 1.1
 */
void QOrganizerItemUnionFilter::prepend(const QOrganizerItemFilter& filter)
{
    Q_D(QOrganizerItemUnionFilter);
    d->m_filters.prepend(filter);
}

/*!
 * Appends the given \a filter to the list of unioned filters
 * \sa operator<<(), prepend(), filters()
   \since 1.1
 */
void QOrganizerItemUnionFilter::append(const QOrganizerItemFilter& filter)
{
    Q_D(QOrganizerItemUnionFilter);
    d->m_filters.append(filter);
}

/*!
 * Removes the given \a filter from the union list
 * \sa filters(), append(), prepend(), clear()
   \since 1.1
 */
void QOrganizerItemUnionFilter::remove(const QOrganizerItemFilter& filter)
{
    Q_D(QOrganizerItemUnionFilter);
    d->m_filters.removeAll(filter);
}

/*!
 * Clears the list of filters.  A cleared union filter will match no items.
 * \sa filters(), remove()
   \since 1.1
 */
void QOrganizerItemUnionFilter::clear()
{
    Q_D(QOrganizerItemUnionFilter);
    d->m_filters.clear();
}

/*!
 * Appends the given \a filter to the list of unioned filters
 * \sa append()
   \since 1.1
 */
QOrganizerItemUnionFilter& QOrganizerItemUnionFilter::operator<<(const QOrganizerItemFilter& filter)
{
    Q_D(QOrganizerItemUnionFilter);
    d->m_filters << filter;
    return *this;
}

/*!
 * Returns the list of filters which form the union filter
 * \sa setFilters(), prepend(), append(), remove()
   \since 1.1
 */
QList<QOrganizerItemFilter> QOrganizerItemUnionFilter::filters() const
{
    Q_D(const QOrganizerItemUnionFilter);
    return d->m_filters;
}

QTM_END_NAMESPACE
