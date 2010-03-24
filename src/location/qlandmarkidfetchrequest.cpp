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

#include "qlandmarkidfetchrequest.h"
#include "qlandmarkfilter.h"
#include "qlandmarksortorder.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QLandmarkIdFetchRequest
    \brief The QLandmarkIdFetchRequest class allows a client to asynchronously
    request a list of landmark identifiers from a landmark manager.

    For a QLandmarkIdFetchRequest, the resultsAvailable() signal will be emitted when the resultant
    identifiers (which may be retrieved by calling ids()), are updated, as well as if
    the overall operation error (which may be retrieved by calling error()) is updated.

    \ingroup location
*/

/*!
    Creates a new landmark id fetch request object.
*/
QLandmarkIdFetchRequest::QLandmarkIdFetchRequest()
{
}

/*!
    Destroys the request object.
*/
QLandmarkIdFetchRequest::~QLandmarkIdFetchRequest()
{
}

/*!
    Returns the filter which will be used to select the landmarks whose identifiers
    will be returned.

    By default, the filter's type will be a LandmarkFilter::DefaultFilter
    and thus match all landmarks.
*/
QLandmarkFilter &QLandmarkIdFetchRequest::filter() const
{
    return *(new QLandmarkFilter());
}

/*!
    Sets the \a filter which will be used to select landmark identifiers.

*/
void QLandmarkIdFetchRequest::setFilter(const QLandmarkFilter &filter)
{
}

/*!
    Returns the sort ordering which is used to sort the result.
*/
QList<QLandmarkSortOrder> QLandmarkIdFetchRequest::sorting() const
{
    return QList<QLandmarkSortOrder>();
}

/*!
    Sets the sort ordering of the request to \a sorting.  This
    function will only have an effect on the results if invoked
    prior to calling \c start()
*/
void QLandmarkIdFetchRequest::setSorting(QList<QLandmarkSortOrder> &sorting)
{
}

/*!
    Returns the list of identifiers of landmarks which matched the
    filter.
*/
QList<QLandmarkId> QLandmarkIdFetchRequest::ids() const
{
    return QList<QLandmarkId>();
}

#include "moc_qlandmarkidfetchrequest.cpp"

QTM_END_NAMESPACE











