/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qgeoroute.h"
#include "qgeoroute_p.h"

#include "qgeocoordinate.h"
#include "qgeoboundingbox.h"

#include <QDateTime>

QTM_BEGIN_NAMESPACE

QGeoRoute::QGeoRoute()
{
}

QGeoRoute::QGeoRoute(const QGeoRoute &other)
{
    Q_UNUSED(other);
}

QGeoRoute::~QGeoRoute()
{
}

QGeoRoute& QGeoRoute::operator= (const QGeoRoute &other)
{
    Q_UNUSED(other);
    return *this;
}

void QGeoRoute::setRouteOverview(const QList<QGeoCoordinate> &routeOverview)
{
    Q_UNUSED(routeOverview);
}

QList<QGeoCoordinate> QGeoRoute::routeOverview() const
{
    return QList<QGeoCoordinate>();
}

void QGeoRoute::setBounds(const QGeoBoundingBox &bounds)
{
    Q_UNUSED(bounds);
}

QGeoBoundingBox QGeoRoute::bounds() const
{
    return QGeoBoundingBox();
}

void QGeoRoute::setRouteSegments(const QList<const QGeoRouteSegment *> &routeSegments)
{
    Q_UNUSED(routeSegments);
}

QList<const QGeoRouteSegment *> QGeoRoute::routeSegments() const
{
    return QList<const QGeoRouteSegment *>();
}

void QGeoRoute::setDuration(const QDateTime &duration)
{
    Q_UNUSED(duration);
}

QDateTime QGeoRoute::duration() const
{
    return QDateTime();
}

void QGeoRoute::setDistance(double value, QGeoMapWidget::DistanceUnits units)
{
    Q_UNUSED(value);
    Q_UNUSED(units);
}

double QGeoRoute::distance(QGeoMapWidget::DistanceUnits units) const
{
    Q_UNUSED(units);
    return 0.0;
}

QGeoCoordinate QGeoRoute::closestPointOnRoute(const QGeoCoordinate &position) const
{
    Q_UNUSED(position);
    return QGeoCoordinate();
}

QTM_END_NAMESPACE
