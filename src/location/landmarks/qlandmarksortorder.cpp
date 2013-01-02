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

#include "qlandmarksortorder.h"
#include "qlandmarksortorder_p.h"
#include "qlandmarkid.h"

#if !defined(Q_CC_MWERKS)
template<> QTM_PREPEND_NAMESPACE(QLandmarkSortOrderPrivate) *QSharedDataPointer<QTM_PREPEND_NAMESPACE(QLandmarkSortOrderPrivate)>::clone()
{
    return d->clone();
}
#endif

QTM_BEGIN_NAMESPACE

/*!
    \class QLandmarkSortOrder
    \brief The QLandmarkSortOrder class serves as a base class
           for the different sort order types and by itself represents
           no sorting.

    \inmodule QtLocation
    \since 1.1

    \ingroup landmarks-sort

    The sort orders define how a list of landmarks should be ordered
    when they searched using a QLandmarkManager or one of the
    landmark request classes.  (For sorting by distance
    see the QLandmarkProximityFilter).
*/

/*!
    \enum QLandmarkSortOrder::SortType
    Defines the type of sort order.
    \value NoSort No sorting is applied
    \value NameSort   Sorts landmarks by name.
*/

/*!
    Constructs a landmark sort order.

    The type of the sort order is NoSort.
*/
QLandmarkSortOrder::QLandmarkSortOrder()
        : d_ptr(new QLandmarkSortOrderPrivate())
{
}

/*!
  \internal
*/
QLandmarkSortOrder::QLandmarkSortOrder(QLandmarkSortOrderPrivate *d_ptr)
        : d_ptr(d_ptr)
{
}

/*!
    Constructs a copy of \a other.
*/
QLandmarkSortOrder::QLandmarkSortOrder(const QLandmarkSortOrder &other)
        : d_ptr(other.d_ptr)
{
}

/*!
    Assigns \a other to this sort order and returns a reference to this
    sort order.
    \since 1.1
*/
QLandmarkSortOrder &QLandmarkSortOrder::operator=(const QLandmarkSortOrder & other)
{
    d_ptr = other.d_ptr;
    return *this;
}

/*!
    Destroys the sort order
*/
QLandmarkSortOrder::~QLandmarkSortOrder()
{
}

/*!
    Returns the type of sort.
    \since 1.1
*/
QLandmarkSortOrder::SortType QLandmarkSortOrder::type() const
{
    return d_ptr->type;
}

/*!
    Returns the direction of the sort order.
    \since 1.1
*/
Qt::SortOrder QLandmarkSortOrder::direction() const
{
    return d_ptr->order;
}

/*!
    Sets the \a direction of the sort order.
    \since 1.1
*/
void QLandmarkSortOrder::setDirection(Qt::SortOrder direction)
{
    d_ptr->order = direction;
}

/*!
    \fn QLandmarkSortOrder::operator==(const QLandmarkSortOrder& other) const
    Returns true if this sort order is identical to \a other.
    \sa operator!=()
    \since 1.1
*/
bool QLandmarkSortOrder::operator==(const QLandmarkSortOrder& other) const
{

    if(!d_ptr->QLandmarkSortOrderPrivate::compare(other.d_ptr))
        return false;

    /* Different types can't be equal */
    if (other.type() != type())
        return false;

    /* Otherwise, use the virtual op == */
    return d_ptr->compare(other.d_ptr);
}

/*!
    \fn QLandmarkSortOrder::operator!=(const QLandmarkSortOrder &other) const
    Returns true if this sort order is not identical to \a other.

    \since 1.1
    \sa operator==()
*/

/*******************************************************************************
*******************************************************************************/

QLandmarkSortOrderPrivate::QLandmarkSortOrderPrivate()
        : QSharedData(),
          type(QLandmarkSortOrder::NoSort),
          order(Qt::AscendingOrder)
{
}

QLandmarkSortOrderPrivate::QLandmarkSortOrderPrivate(const QLandmarkSortOrderPrivate &other)
        : QSharedData(),
          type(other.type),
          order(other.order)
{
}

QLandmarkSortOrderPrivate::~QLandmarkSortOrderPrivate()
{
}

QTM_END_NAMESPACE
