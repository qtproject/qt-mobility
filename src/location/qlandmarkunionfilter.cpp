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

#include "qlandmarkunionfilter.h"
#include "qlandmarkfilter_p.h"

QTM_BEGIN_NAMESPACE

class QLandmarkUnionFilterPrivate : public QLandmarkFilterPrivate
{
public:
    QLandmarkUnionFilterPrivate();
    QLandmarkUnionFilterPrivate(const QLandmarkUnionFilterPrivate &other);
    ~QLandmarkUnionFilterPrivate();

    QList<QLandmarkFilter> filters;
};

QLandmarkUnionFilterPrivate::QLandmarkUnionFilterPrivate()
    : QLandmarkFilterPrivate()
{
}

QLandmarkUnionFilterPrivate::QLandmarkUnionFilterPrivate(const QLandmarkUnionFilterPrivate &other)
    : QLandmarkFilterPrivate(other),
      filters(other.filters)
{
}

QLandmarkUnionFilterPrivate::~QLandmarkUnionFilterPrivate()
{
}

/*!
    \class QLandmarkUnionFilter
    \brief The QLandmarkUnionFilter class provides a filter which unions the results
            of its constituent filters.
    \ingroup location

    During a search the union filter performs and OR operation with all
    its constituent filters.
*/

/*!
    Constructs a new union filter.
*/
QLandmarkUnionFilter::QLandmarkUnionFilter()
    : QLandmarkFilter(*new QLandmarkUnionFilterPrivate)
{
}

/*!
    Destroys the filter.
*/
QLandmarkUnionFilter::~QLandmarkUnionFilter()
{
}

/*!
    Sets the \a filters whose criteria will be unioned.
    \sa filters()
*/
void QLandmarkUnionFilter::setFilters(const QList<QLandmarkFilter>& filters)
{
}

/*!
    Prepends the given \a filter to the list of unioned filters
    \sa append(), filters()
*/
void QLandmarkUnionFilter::prepend(const QLandmarkFilter& filter)
{
}

/*!
    Appends the given \a filter to the list of unioned filters
    \sa operator<<(), prepend(), filters()
*/
void QLandmarkUnionFilter::append(const QLandmarkFilter& filter)
{
}

/*!
    Removes the given \a filter from the union list
    \sa filters(), append(), prepend()
*/
void QLandmarkUnionFilter::remove(const QLandmarkFilter& filter)
{
}

/*!
    Appends the given \a filter to the list of unioned filters
    \sa append()
 */
QLandmarkUnionFilter& QLandmarkUnionFilter::operator<<(const QLandmarkFilter& filter)
{
    return *this;
}

/*!
    Returns the list of filters which form the union filter
    \sa setFilters(), prepend(), append(), remove()
 */
QList<QLandmarkFilter> QLandmarkUnionFilter::filters() const
{
    return QList<QLandmarkFilter>();
}

QTM_END_NAMESPACE
