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

#include "qgeoroutesegment.h"
#include "qgeoroutesegment_p.h"

#include "qgeocoordinate.h"
#include <QDateTime>

QTM_BEGIN_NAMESPACE

/*!
  \class QGeoRouteSegment
  \brief The QGeoRouteSegment class
  \ingroup maps

*/

/*!
  \enum QGeoRouteSegment::SegmentType

  description

  \value NormalSegment description
  \value PrivateTransportSegment description
  \value PublicTransportSegment description
  \value TruckSegment description
};
*/

/*!
*/
QGeoRouteSegment::QGeoRouteSegment()
    : d_ptr(new QGeoRouteSegmentPrivate()) {}

/*!
*/
QGeoRouteSegment::QGeoRouteSegment(QGeoRouteSegmentPrivate *d_ptr)
    : d_ptr(d_ptr) {}

/*!
*/
QGeoRouteSegment::~QGeoRouteSegment()
{
    Q_D(QGeoRouteSegment);
    delete d;
}

/*!
*/
QGeoRouteSegment::SegmentType QGeoRouteSegment::type() const
{
    Q_D(const QGeoRouteSegment);
    return d->type;
}

/*!
*/
void QGeoRouteSegment::setDuration(const QDateTime &duration)
{
    Q_D(QGeoRouteSegment);
    d->duration = duration;
}

/*!
*/
QDateTime QGeoRouteSegment::duration() const
{
    Q_D(const QGeoRouteSegment);
    return d->duration;
}

/*!
*/
void QGeoRouteSegment::setDistance(const QGeoDistance &distance)
{
    Q_D(QGeoRouteSegment);
    d->distance = distance;
}

/*!
*/
QGeoDistance QGeoRouteSegment::distance() const
{
    Q_D(const QGeoRouteSegment);
    return d->distance;
}

// bounds per segment?  or is bounds per route enough?

/*!
*/
void QGeoRouteSegment::setGeometry(const QList<QGeoCoordinate> &geometry)
{
    Q_D(QGeoRouteSegment);
    d->geometry = geometry;
}

/*!
*/
QList<QGeoCoordinate> QGeoRouteSegment::geometry() const
{
    Q_D(const QGeoRouteSegment);
    return d->geometry;
}

/*!
*/
void QGeoRouteSegment::setInstructions(const QList<const QGeoNavigationInstruction *> &instructions)
{
    Q_D(QGeoRouteSegment);
    d->instructions = instructions;
}

/*!
*/
QList<const QGeoNavigationInstruction*> QGeoRouteSegment::instructions() const
{
    Q_D(const QGeoRouteSegment);
    return d->instructions;
}

/*******************************************************************************
*******************************************************************************/

QGeoRouteSegmentPrivate::QGeoRouteSegmentPrivate()
{
    type = QGeoRouteSegment::NormalSegment;
}

QGeoRouteSegmentPrivate::QGeoRouteSegmentPrivate(const QGeoRouteSegmentPrivate &other)
    : type(other.type),
    duration(other.duration),
    distance(other.distance),
    geometry(other.geometry),
    instructions(other.instructions){}

QGeoRouteSegmentPrivate::~QGeoRouteSegmentPrivate() {}

QGeoRouteSegmentPrivate& QGeoRouteSegmentPrivate::operator= (const QGeoRouteSegmentPrivate &other)
{
    type = other.type;
    duration = other.duration;
    distance = other.distance;
    geometry = other.geometry;
    instructions = other.instructions;
    return *this;
}

QTM_END_NAMESPACE

