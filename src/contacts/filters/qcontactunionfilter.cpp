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

#include "qcontactunionfilter.h"
#include "qcontactunionfilter_p.h"
#include "qcontactfilter_p.h"
#include "qcontactmanager.h"

QTM_BEGIN_NAMESPACE

/*!
   \class QContactUnionFilter
   \brief The QContactUnionFilter class provides a filter which unions the
    results of other filters.


   \inmodule QtContacts
   \since 1.0

   \ingroup contacts-filters

   It may be used to select contacts which match all of the filters in the union
 */

Q_IMPLEMENT_CONTACTFILTER_PRIVATE(QContactUnionFilter);

/*!
 * \fn QContactUnionFilter::QContactUnionFilter(const QContactFilter& other)
 * Constructs a copy of \a other if possible, otherwise constructs a new union filter
 * \since 1.0
 */

/*!
 * Constructs a new intersection filter
 */
QContactUnionFilter::QContactUnionFilter()
    : QContactFilter(new QContactUnionFilterPrivate)
{
}

/*!
 * Sets the filters whose criteria will be unioned to \a filters
 * \sa filters()
 * \since 1.0
 */
void QContactUnionFilter::setFilters(const QList<QContactFilter>& filters)
{
    Q_D(QContactUnionFilter);
    d->m_filters = filters;
}

/*!
 * Clears the list of filters.  A cleared union filter will match no contacts.
 * \sa filters(), remove()
 */
void QContactUnionFilter::clear()
{
    Q_D(QContactUnionFilter);
    d->m_filters.clear();
}

/*!
 * Prepends the given \a filter to the list of unioned filters
 * \since 1.0
 * \sa append(), filters()
 */
void QContactUnionFilter::prepend(const QContactFilter& filter)
{
    Q_D(QContactUnionFilter);
    d->m_filters.prepend(filter);
}

/*!
 * Appends the given \a filter to the list of unioned filters
 * \since 1.0
 * \sa operator<<(), prepend(), filters()
 */
void QContactUnionFilter::append(const QContactFilter& filter)
{
    Q_D(QContactUnionFilter);
    d->m_filters.append(filter);
}

/*!
 * Removes the given \a filter from the union list
 * \since 1.0
 * \sa filters(), append(), prepend(), clear()
 */
void QContactUnionFilter::remove(const QContactFilter& filter)
{
    Q_D(QContactUnionFilter);
    d->m_filters.removeAll(filter);
}

/*!
 * Appends the given \a filter to the list of unioned filters
 * \sa append()
 */
QContactUnionFilter& QContactUnionFilter::operator<<(const QContactFilter& filter)
{
    Q_D(QContactUnionFilter);
    d->m_filters << filter;
    return *this;
}

/*!
 * Returns the list of filters which form the union filter
 * \sa setFilters(), prepend(), append(), remove()
 */
QList<QContactFilter> QContactUnionFilter::filters() const
{
    Q_D(const QContactUnionFilter);
    return d->m_filters;
}

QTM_END_NAMESPACE
