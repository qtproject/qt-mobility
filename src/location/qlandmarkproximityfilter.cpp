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

#include "qlandmarkproximityfilter.h"
#include "qgeocoordinate.h"
#include "qlandmarkfilter_p.h"

QTM_BEGIN_NAMESPACE

class QLandmarkProximityFilterPrivate : public QLandmarkFilterPrivate
{
public:
    QLandmarkProximityFilterPrivate();
    QLandmarkProximityFilterPrivate(const QLandmarkProximityFilterPrivate &other);
    ~QLandmarkProximityFilterPrivate();

    QGeoCoordinate coordinate;
    double radius;
};

QLandmarkProximityFilterPrivate::QLandmarkProximityFilterPrivate()
        : QLandmarkFilterPrivate(),
        coordinate(QGeoCoordinate()),
        radius(0.0)
{
}

QLandmarkProximityFilterPrivate::QLandmarkProximityFilterPrivate(const QLandmarkProximityFilterPrivate &other)
        : QLandmarkFilterPrivate(other),
        coordinate(other.coordinate),
        radius(other.radius)
{
}

QLandmarkProximityFilterPrivate::~QLandmarkProximityFilterPrivate()
{
}

/*!
    \class QLandmarkProximityFilter
    \brief The QLandmarkProximityFilter class is used to search for landmarks based on the radius
    around a given coordinate.
    \ingroup location
*/

/*!
    Creates a filter that will select landmarks within a given \a radius around
    a central \a coordinate.
*/
QLandmarkProximityFilter::QLandmarkProximityFilter(const QGeoCoordinate &coordinate, double radius)
        : QLandmarkFilter(*new QLandmarkProximityFilterPrivate)
{
    Q_UNUSED(coordinate);
    Q_UNUSED(radius);
    //TODO: implement
}

/*!
    Destroys the filter.
*/
QLandmarkProximityFilter::~QLandmarkProximityFilter()
{
    //TODO: implement
}

/*!
    Returns the central coordinate of the filter.
*/
QGeoCoordinate QLandmarkProximityFilter::coordinate() const
{
    return QGeoCoordinate();
}

/*!
    Sets the central \a coordinate of the filter.
*/
void QLandmarkProximityFilter::setCoordinate(const QGeoCoordinate &coordinate)
{
}

/*!
    Returns the radius of the filter.
*/
double QLandmarkProximityFilter::radius() const
{
    return 0.0;
}

/*!
    Sets the \a radius of the filter.
*/
void QLandmarkProximityFilter::setRadius(double radius)
{
}

QTM_END_NAMESPACE
