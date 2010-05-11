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
           for the different sort order types.
    \ingroup landmarks-sort

    The sort orders define how a list of landmarks should be ordered
    when they searched using a QLandmarkManager or one of the
    landmark request classes.
*/

/*!
    \enum QLandmarkSortOrder::SortType
    Defines the type of sort order.
    \value DefaultSort The default sort order that does nothing
    \value NameSort   Sorts landmarks by name.
    \value DistanceSort   Sorts landmarks by distance from a particular
           coordinate.
*/

/*!
    Constructs a landmark sort order.

    The type of the sort order is InvalidSort.
*/
QLandmarkSortOrder::QLandmarkSortOrder()
        : d_ptr(new QLandmarkSortOrderPrivate())
{
}

/*!
    \fn QLandmarkSortOrder::QLandmarkSortOrder(const QLandmarkSortOrder &other)
    Constructs a copy of \a other.
*/

/*!
  \internal
*/
QLandmarkSortOrder::QLandmarkSortOrder(QLandmarkSortOrderPrivate *d_ptr)
        : d_ptr(d_ptr)
{
}

QLandmarkSortOrder::QLandmarkSortOrder(const QLandmarkSortOrder &other)
        : d_ptr(other.d_ptr)
{
}

QLandmarkSortOrder &QLandmarkSortOrder::operator=(const QLandmarkSortOrder & other)
{
    d_ptr = other.d_ptr;
    return *this;
}

QLandmarkSortOrder::~QLandmarkSortOrder()
{
}

/*!
    Returns the type of sort.
*/
QLandmarkSortOrder::SortType QLandmarkSortOrder::type() const
{
    return d_ptr->type;
}

/*!
    Returns the direction of the sort order.
*/
Qt::SortOrder QLandmarkSortOrder::direction() const
{
    return d_ptr->order;
}

/*!
    Sets the \a direction of the sort order.
*/
void QLandmarkSortOrder::setDirection(Qt::SortOrder direction)
{
    d_ptr->order = direction;
}

/*!
    \fn QLandmarkSortOrder::operator==(const QLandmarkSortOrder& other) const
    Returns true if this sort order is identical to \a other.
    \sa operator!=()
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

    \sa operator==()
*/

/*!
    \fn QLandmarkSortOrder::operator=(const QLandmarkSortOrder &other)
    Assigns \a other to this sort order and returns a reference to this
    sort order.
*/

/*******************************************************************************
*******************************************************************************/

QLandmarkSortOrderPrivate::QLandmarkSortOrderPrivate()
        : QSharedData(),
          type(QLandmarkSortOrder::DefaultSort),
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
