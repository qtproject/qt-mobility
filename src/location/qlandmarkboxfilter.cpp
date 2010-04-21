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

#include "qlandmarkboxfilter.h"
#include "qlandmarkboxfilter_p.h"
#include "qgeocoordinate.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QLandmarkBoxFilter
    \brief The QLandmarkBoxFilter class is used to search for landmarks within a given bounding box.
    \ingroup landmarks-filter
*/

/*!
    Creates a filter that will search for landmarks within a bounding
    box defined by the \a topLeft and \a bottomRight coordinates.
*/
QLandmarkBoxFilter::QLandmarkBoxFilter(const QGeoCoordinate &topLeft,
                                       const QGeoCoordinate &bottomRight)
        : QLandmarkFilter(new QLandmarkBoxFilterPrivate(topLeft, bottomRight))
{
}

/*!
    Destroys the filter.
*/
QLandmarkBoxFilter::~QLandmarkBoxFilter()
{
    // pointer deleted in superclass destructor
}

/*!
    Returns the top-left corner of the bounding box this filter uses
    to select landmarks.
*/
QGeoCoordinate QLandmarkBoxFilter::topLeftCoordinate() const
{
    Q_D(const QLandmarkBoxFilter);
    return d->topLeftCoord;
}

/*!
    Sets the \a topLeft corner of the bounding box this filter uses to
    select landmarks.
*/
void QLandmarkBoxFilter::setTopLeftCoordinate(const QGeoCoordinate &topLeft)
{
    Q_D(QLandmarkBoxFilter);
    d->topLeftCoord = topLeft;
}

/*!
    Returns the bottom-right coordinate of the bounding box this filter uses
    to select landmarks.
*/
QGeoCoordinate QLandmarkBoxFilter::bottomRightCoordinate() const
{
    Q_D(const QLandmarkBoxFilter);
    return d->bottomRightCoord;
}

/*!
    Sets the \a bottomRight coordinate of the bounding box this filter uses to
    select landmarks.
*/
void QLandmarkBoxFilter::setBottomRightCoordinate(const QGeoCoordinate &bottomRight)
{
    Q_D(QLandmarkBoxFilter);
    d->bottomRightCoord = bottomRight;
}

/*******************************************************************************
*******************************************************************************/

QLandmarkBoxFilterPrivate::QLandmarkBoxFilterPrivate(const QGeoCoordinate &topLeft,
        const QGeoCoordinate &bottomRight)
        : topLeftCoord(topLeft),
        bottomRightCoord(bottomRight)
{
    type = QLandmarkFilter::BoxFilter;
}

QLandmarkBoxFilterPrivate::QLandmarkBoxFilterPrivate(const QLandmarkBoxFilterPrivate &other)
        : QLandmarkFilterPrivate(other),
        topLeftCoord(other.topLeftCoord),
        bottomRightCoord(other.bottomRightCoord) {}

QLandmarkBoxFilterPrivate::~QLandmarkBoxFilterPrivate() {}

QTM_END_NAMESPACE
