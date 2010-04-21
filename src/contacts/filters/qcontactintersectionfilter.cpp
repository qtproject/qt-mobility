/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qcontactintersectionfilter.h"
#include "qcontactintersectionfilter_p.h"
#include "qcontactfilter_p.h"
#include "qcontactmanager.h"

QTM_BEGIN_NAMESPACE

/*!
  \class QContactIntersectionFilter
  \brief The QContactIntersectionFilter class provides a filter which intersects the results of other filters.
   \ingroup contacts-filters
 
  It may be used to select contacts which match all of the filters in the intersection
 */

Q_IMPLEMENT_CONTACTFILTER_PRIVATE(QContactIntersectionFilter);

/*!
 * \fn QContactIntersectionFilter::QContactIntersectionFilter(const QContactFilter& other)
 * Constructs a copy of \a other if possible, otherwise constructs a new intersection filter
 */

/*!
 * Constructs a new intersection filter
 */
QContactIntersectionFilter::QContactIntersectionFilter()
    : QContactFilter(new QContactIntersectionFilterPrivate)
{
}

/*!
 * Sets the filters whose criteria will be intersected to \a filters
 * \sa filters()
 */
void QContactIntersectionFilter::setFilters(const QList<QContactFilter>& filters)
{
    Q_D(QContactIntersectionFilter);
    d->m_filters = filters;
}

/*!
 * Prepends the given \a filter to the list of intersected filters
 * \sa append(), filters()
 */
void QContactIntersectionFilter::prepend(const QContactFilter& filter)
{
    Q_D(QContactIntersectionFilter);
    d->m_filters.prepend(filter);
}

/*!
 * Appends the given \a filter to the list of intersected filters
 * \sa operator<<(), prepend(), filters()
 */
void QContactIntersectionFilter::append(const QContactFilter& filter)
{
    Q_D(QContactIntersectionFilter);
    d->m_filters.append(filter);
}

/*!
 * Removes the given \a filter from the intersection list
 * \sa filters(), append(), prepend()
 */
void QContactIntersectionFilter::remove(const QContactFilter& filter)
{
    Q_D(QContactIntersectionFilter);
    d->m_filters.removeAll(filter);
}

/*!
 * Appends the given \a filter to the list of intersected filters
 * \sa append()
 */
QContactIntersectionFilter& QContactIntersectionFilter::operator<<(const QContactFilter& filter)
{
    Q_D(QContactIntersectionFilter);
    d->m_filters << filter;
    return *this;
}

/*!
 * Returns the list of filters which form the intersection filter
 * \sa setFilters(), prepend(), append(), remove()
 */
QList<QContactFilter> QContactIntersectionFilter::filters() const
{
    Q_D(const QContactIntersectionFilter);
    return d->m_filters;
}

QTM_END_NAMESPACE
