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

#include "qgeomapcircleobject.h"
#include "qgeomapcircleobject_p.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoMapCircleObject
    \brief The QGeoMapCircleObject class is a QGeoMapObject used to draw the region
    within a certain distance of a coordinate.

    \inmodule QtLocation
    
    \ingroup maps-mapping-objects

    TODO add pen and brush attributes as per alignment doc
*/

/*!
    Constructs a new circle object with the specified \a parent.
*/
QGeoMapCircleObject::QGeoMapCircleObject(QGeoMapObject *parent)
    : QGeoMapObject(new QGeoMapCircleObjectPrivate(this, parent)) {}

/*!
    Destroys this circle object.
*/
QGeoMapCircleObject::~QGeoMapCircleObject()
{
}

/*!
    Sets the center of the circle object to \a center.
*/
void QGeoMapCircleObject::setCenter(const QGeoCoordinate &center)
{
    Q_D(QGeoMapCircleObject);
    d->center = center;
}

/*!
    Returns the center of the circle object.
*/
QGeoCoordinate QGeoMapCircleObject::center() const
{
    Q_D(const QGeoMapCircleObject);
    return d->center;
}

/*!
    Sets the radius of the circle object to \a radius.
*/
void QGeoMapCircleObject::setRadius(const QGeoDistance &radius)
{
    Q_D(QGeoMapCircleObject);
    d->radius = radius;
}

/*!
    Returns the radius of the circle object.
*/
QGeoDistance QGeoMapCircleObject::radius() const
{
    Q_D(const QGeoMapCircleObject);
    return d->radius;
}

/*******************************************************************************
*******************************************************************************/

QGeoMapCircleObjectPrivate::QGeoMapCircleObjectPrivate(QGeoMapObject *impl, QGeoMapObject *parent)
    : QGeoMapObjectPrivate(impl, parent)
{
    type = QGeoMapObject::CircleType;
}

QGeoMapCircleObjectPrivate::QGeoMapCircleObjectPrivate(const QGeoMapCircleObjectPrivate &other)
    : QGeoMapObjectPrivate(other),
    center(other.center),
    radius(other.radius) {}

QGeoMapCircleObjectPrivate::~QGeoMapCircleObjectPrivate() {}

QGeoMapCircleObjectPrivate& QGeoMapCircleObjectPrivate::operator= (const QGeoMapCircleObjectPrivate &other)
{
    QGeoMapObjectPrivate::operator=(other);

    center = other.center;
    radius = other.radius;

    return *this;
}

QTM_END_NAMESPACE

