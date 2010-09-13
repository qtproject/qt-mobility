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
    within a given distance of a coordinate.

    \inmodule QtLocation

    \ingroup maps-mapping-objects

    The circle is specified by either a valid QGeoBoundingCircle instance or a
    valid QGeoCoordinate instance and a qreal with value greater than 0.0,
    which represent the center of the circle and the radius of the circle in
    metres respectively.

    The circle may appear as an ellipse on maps which use the Mercator
    projection. This is done so that the circle accurately covers all points at
    a distance of the radius or less from the center.
*/

/*!
    Constructs a new circle object with the parent \a parent.
*/
QGeoMapCircleObject::QGeoMapCircleObject(QGeoMapObject *parent)
        : QGeoMapObject(new QGeoMapCircleObjectPrivate(this, parent)) {}

/*!
    Constructs a new circle object based on the circle \a circle, with
    the parent \a parent.
*/
QGeoMapCircleObject::QGeoMapCircleObject(const QGeoBoundingCircle &circle, QGeoMapObject *parent)
        : QGeoMapObject(new QGeoMapCircleObjectPrivate(this, parent))
{
    Q_D(QGeoMapCircleObject);
    d->circle = circle;
}

/*!
    Constructs a new circle object with a center at coordinate \a center
    and a radius in meters of \a radius, with the parent \a parent.
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

/*!
    \property QGeoMapCircleObject::pen
    \brief This property holds the pen that will be used to draw this object.

    The pen is used to draw an outline around the circle. The circle is
    filled using the QGeoMapCircleObject::brush property.

    The pen will be treated as a cosmetic pen, which means that the width
    of the pen will be independent of the zoom level of the map.
*/
void QGeoMapCircleObject::setPen(const QPen &pen)
{
    Q_D(QGeoMapCircleObject);

    QPen newPen = pen;
    newPen.setCosmetic(true);

    if (d->pen == newPen)
        return;

    d->pen = newPen;
    objectUpdated();
    emit penChanged(d->pen);
}

QPen QGeoMapCircleObject::pen() const
{
    Q_D(const QGeoMapCircleObject);
    return d->pen;
}

/*!
    \property QGeoMapCircleObject::brush
    \brief This property holds the brush that will be used to draw this object.

    The brush is used to fill in circle.

    The outline around the perimeter of the circle is drawn using the
    QGeoMapCircleObject::pen property.
*/
void QGeoMapCircleObject::setBrush(const QBrush &brush)
{
    Q_D(QGeoMapCircleObject);
    if (d->brush != brush) {
        d->brush = brush;
        objectUpdated();
        emit brushChanged(d->brush);
    }
}

QBrush QGeoMapCircleObject::brush() const
{
    Q_D(const QGeoMapCircleObject);
    return d->brush;
}

/*!
    Returns a QGeoBoundingCircle instance which corresponds to the circle that
    will be drawn by this object.

    This is equivalent to
    \code
        QGeoMapCircleObject *object;
        // setup object
        QGeoBoundingCircle(object->center(), object->radius());
    \endcode
*/
QGeoBoundingCircle QGeoMapCircleObject::circle() const
{
    Q_D(const QGeoMapCircleObject);
    return d->circle;
}

/*!
    Sets the circle that will be drawn by this object to \a circle.

    This is equivalent to
    \code
        QGeoMapCircleObject *object;
        // setup object
        object->setCenter(circle.center());
        object->setRadius(circle.radius());
    \endcode
*/
void QGeoMapCircleObject::setCircle(const QGeoBoundingCircle &circle)
{
    Q_D(QGeoMapCircleObject);

    QGeoBoundingCircle oldCircle = d->circle;

    if (oldCircle == circle)
        return;

    d->circle = circle;

    objectUpdated();

    if (oldCircle.center() != d->circle.center())
        emit centerChanged(d->circle.center());

    if (oldCircle.radius() != d->circle.radius())
        emit radiusChanged(d->circle.radius());
}

/*!
    \property QGeoMapCircleObject::center

    \brief This property holds the coordinate of the center of the circle to be
    drawn by this circle object.

    The default value of this property is an invalid coordinate.  While the
    value of this property is invalid the circle object will not be displayed.
*/
void QGeoMapCircleObject::setCenter(const QGeoCoordinate &center)
{
    Q_D(QGeoMapCircleObject);
    if (d->circle.center() != center) {
        d->circle.setCenter(center);
        objectUpdated();
        emit centerChanged(center);
    }
}

QGeoCoordinate QGeoMapCircleObject::center() const
{
    Q_D(const QGeoMapCircleObject);
    return d->circle.center();
}

/*!
    \property QGeoMapCircleObject::radius
    \brief This property holds the radius in metres of the circle that will be
    drawn by this circle object.

    The default value of this property is -1.0. While the value of this
    property is not greater than 0 the circle object will not be displayed.
*/
void QGeoMapCircleObject::setRadius(qreal radius)
{
    Q_D(QGeoMapCircleObject);
    if (d->circle.radius() != radius) {
        d->circle.setRadius(radius);
        objectUpdated();
        emit radiusChanged(radius);
    }
}

qreal QGeoMapCircleObject::radius() const
{
    Q_D(const QGeoMapCircleObject);
    return d->circle.radius();
}

/*******************************************************************************
*******************************************************************************/

QGeoMapCircleObjectPrivate::QGeoMapCircleObjectPrivate(QGeoMapObject *impl, QGeoMapObject *parent)
        : QGeoMapObjectPrivate(impl, parent, QGeoMapObject::CircleType)
{
    pen.setCosmetic(true);
}

QGeoMapCircleObjectPrivate::~QGeoMapCircleObjectPrivate() {}

#include "moc_qgeomapcircleobject.cpp"

QTM_END_NAMESPACE

