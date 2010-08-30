/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgeomapcircleobject.h"
#include "qgeomapcircleobject_p.h"

#include "qgeocoordinate.h"

#include <QDebug>

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoMapCircleObject
    \brief The QGeoMapCircleObject class is a QGeoMapObject used to draw the region
    within a certain distance of a coordinate.

    \inmodule QtLocation

    \ingroup maps-mapping-objects
*/

QGeoMapCircleObject::QGeoMapCircleObject(QGeoMapObject *parent)
        : QGeoMapObject(new QGeoMapCircleObjectPrivate(this, parent)) {}

/*!
    Constructs a new circle object with the specified \a parent.
*/
QGeoMapCircleObject::QGeoMapCircleObject(const QGeoBoundingCircle &circle, QGeoMapObject *parent)
        : QGeoMapObject(new QGeoMapCircleObjectPrivate(this, parent))
{
    Q_D(QGeoMapCircleObject);
    d->circle = circle;
}

/*!
    Constructs a new circle object with the specified \a parent.
*/
QGeoMapCircleObject::QGeoMapCircleObject(const QGeoCoordinate &center, qreal radius, QGeoMapObject *parent)
        : QGeoMapObject(new QGeoMapCircleObjectPrivate(this, parent))
{
    Q_D(QGeoMapCircleObject);
    d->circle = QGeoBoundingCircle(center, radius);
}

/*!
    Destroys this circle object.
*/
QGeoMapCircleObject::~QGeoMapCircleObject()
{
}

void QGeoMapCircleObject::setPen(const QPen &pen)
{
    Q_D(QGeoMapCircleObject);
    if (d->pen != pen) {
        d->pen = pen;
        objectUpdate();
        emit penChanged(d->pen);
    }
}

QPen QGeoMapCircleObject::pen() const
{
    Q_D(const QGeoMapCircleObject);
    return d->pen;
}

void QGeoMapCircleObject::setBrush(const QBrush &brush)
{
    Q_D(QGeoMapCircleObject);
    if (d->brush != brush) {
        d->brush = brush;
        objectUpdate();
        emit brushChanged(d->brush);
    }
}

QBrush QGeoMapCircleObject::brush() const
{
    Q_D(const QGeoMapCircleObject);
    return d->brush;
}

QGeoBoundingCircle QGeoMapCircleObject::circle() const
{
    Q_D(const QGeoMapCircleObject);
    return d->circle;
}

void QGeoMapCircleObject::setCircle(const QGeoBoundingCircle &circle)
{
    Q_D(QGeoMapCircleObject);

    QGeoBoundingCircle oldCircle = d->circle;

    if (oldCircle == circle)
        return;

    d->circle = circle;

    objectUpdate();

    if (oldCircle.center() != d->circle.center())
        emit centerChanged(d->circle.center());

    if (oldCircle.radius() != d->circle.radius())
        emit radiusChanged(d->circle.radius());
}

/*!
    Sets the center of the circle object to \a center.
*/
void QGeoMapCircleObject::setCenter(const QGeoCoordinate &center)
{
    Q_D(QGeoMapCircleObject);
    if (d->circle.center() != center) {
        d->circle.setCenter(center);
        objectUpdate();
        emit centerChanged(center);
    }
}

/*!
    Returns the center of the circle object.
*/
QGeoCoordinate QGeoMapCircleObject::center() const
{
    Q_D(const QGeoMapCircleObject);
    return d->circle.center();
}

/*!
    Sets the radius of the circle object to \a radius metres.
*/
void QGeoMapCircleObject::setRadius(qreal radius)
{
    Q_D(QGeoMapCircleObject);
    if (d->circle.radius() != radius) {
        d->circle.setRadius(radius);
        objectUpdate();
        emit radiusChanged(radius);
    }
}

/*!
    Returns the radius of the circle object in metres.
*/
qreal QGeoMapCircleObject::radius() const
{
    Q_D(const QGeoMapCircleObject);
    return d->circle.radius();
}

/*******************************************************************************
*******************************************************************************/

QGeoMapCircleObjectPrivate::QGeoMapCircleObjectPrivate(QGeoMapObject *impl, QGeoMapObject *parent)
        : QGeoMapObjectPrivate(impl, parent, QGeoMapObject::CircleType) {}

QGeoMapCircleObjectPrivate::~QGeoMapCircleObjectPrivate() {}

#include "moc_qgeomapcircleobject.cpp"

QTM_END_NAMESPACE

