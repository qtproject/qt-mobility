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

#include "qlandmarkdistancesort.h"
#include "qgeocoordinate.h"
#include "qlandmarkid.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QLandmarkDistanceSort
    \brief The QLandmarkDistanceSort class is used to sort landmarks by distance
    from a given coordinate.
    \ingroup location
*/

/*!
    Creates a sort order that sorts landmarks by distance from a given \a coordinate.
    If the \a direction is Qt::Ascending, it means that landmarks are listed in
    increasing order of distance.
*/
QLandmarkDistanceSort::QLandmarkDistanceSort(QGeoCoordinate coordinate, Qt::SortOrder direction)
{
    //TODO: implement
}

/*!
    Destroys the sort order.
*/
QLandmarkDistanceSort::~QLandmarkDistanceSort()
{
    //TODO: implement
}

/*!
    Returns the coordinate to sort distances from.
*/
QGeoCoordinate QLandmarkDistanceSort::coordinate() const
{
    return QGeoCoordinate();
}

/*!
    Sets the \a coordinate to sort distances from.
*/
void QLandmarkDistanceSort::setCoordinate(const QGeoCoordinate &coordinate)
{
}

QTM_END_NAMESPACE
