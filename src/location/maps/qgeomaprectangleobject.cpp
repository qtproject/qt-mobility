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

#include "qgeomaprectangleobject.h"
#include "qgeomaprectangleobject_p.h"

#include "qgeocoordinate.h"
#include "qgeoboundingbox.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoMapRectangleObject
    \brief The QGeoMapRectangleObject class is a QGeoMapObject used to draw
    a rectangular region on a map.

    \inmodule QtLocation

    \ingroup maps-mapping-objects

    The rectangle is specified by either a valid QGeoBoundingBox instance or
    a pair of valid QGeoCoordinate instances which represent the top left and
    bottom right coordinates of the rectangle respectively.
*/

/*!
    Constructs a new rectangle object with the parent \a parent.
*/
QGeoMapRectangleObject::QGeoMapRectangleObject(QGeoMapObject *parent)
        : QGeoMapObject(new QGeoMapRectangleObjectPrivate(this, parent)) {}

/*!
    Constructs a new rectangle object based on the bounding box \a boundingBox,
    with parent \a parent.
*/
QGeoMapRectangleObject::QGeoMapRectangleObject(const QGeoBoundingBox &boundingBox, QGeoMapObject *parent)
        : QGeoMapObject(new QGeoMapRectangleObjectPrivate(this, parent))
{
    Q_D(QGeoMapRectangleObject);
    d->bounds = boundingBox;
}

/*!
    Constructs a new rectangle object with the top left coordinate at \a
    topLeft and the bottom right coordinate at \a bottomRight, with the parent
    \a parent.
*/
QGeoMapRectangleObject::QGeoMapRectangleObject(const QGeoCoordinate &topLeft, const QGeoCoordinate &bottomRight, QGeoMapObject *parent)
        : QGeoMapObject(new QGeoMapRectangleObjectPrivate(this, parent))
{
    Q_D(QGeoMapRectangleObject);
    d->bounds = QGeoBoundingBox(topLeft, bottomRight);
}

/*!
    Destroys this rectangle object.
*/
QGeoMapRectangleObject::~QGeoMapRectangleObject()
{
}

/*!
    Returns a QGeoBoundingBox instance which corresponds to the rectangle that
    will be drawn by this object.

    This is equivalent to
    \code
        QGeoMapRectangleObject *object;
        // setup object
        QGeoBoundingBox(object->topLeft(), object->bottomRight());
    \endcode
*/
QGeoBoundingBox QGeoMapRectangleObject::bounds() const
{
    Q_D(const QGeoMapRectangleObject);
    return d->bounds;
}

/*!
    Sets the rectangle that will be drawn by this object to \a bounds.

    This is equivalent to
    \code
        QGeoMapRectangleObject *object;
        // setup object
        object->setTopLeft(bounds.topLeft());
        object->setBottomRight(bounds.bottomRight());
    \endcode
*/
void QGeoMapRectangleObject::setBounds(const QGeoBoundingBox &bounds)
{
    Q_D(QGeoMapRectangleObject);

    QGeoBoundingBox oldBounds = d->bounds;

    if (oldBounds == bounds)
        return;

    d->bounds = bounds;

    if (d->bounds.topLeft() != oldBounds.topLeft())
        emit topLeftChanged(d->bounds.topLeft());

    if (d->bounds.bottomRight() != oldBounds.bottomRight())
        emit bottomRightChanged(d->bounds.bottomRight());
}

/*!
    \property QGeoMapRectangleObject::topLeft
    \brief This property holds the top left coordinate of the rectangle to be
    drawn by this rectangle object.

    The default value of this property is an invalid coordinate.  While
    the value of this property is invalid the rectangle object will not be
    displayed.
*/
QGeoCoordinate QGeoMapRectangleObject::topLeft() const
{
    Q_D(const QGeoMapRectangleObject);
    return d->bounds.topLeft();
}

void QGeoMapRectangleObject::setTopLeft(const QGeoCoordinate &topLeft)
{
    Q_D(QGeoMapRectangleObject);
    if (d->bounds.topLeft() != topLeft) {
        d->bounds.setTopLeft(topLeft);
        objectUpdated();
        emit topLeftChanged(d->bounds.topLeft());
    }
}

/*!
    \property QGeoMapRectangleObject::bottomRight
    \brief This property holds the bottom right coordinate of the rectangle to
    be drawn by this rectangle object.

    The default value of this property is an invalid coordinate.  While
    the value of this property is invalid the rectangle object will not be
    displayed.
*/
QGeoCoordinate QGeoMapRectangleObject::bottomRight() const
{
    Q_D(const QGeoMapRectangleObject);
    return d->bounds.bottomRight();
}

void QGeoMapRectangleObject::setBottomRight(const QGeoCoordinate &bottomRight)
{
    Q_D(QGeoMapRectangleObject);
    if (d->bounds.bottomRight() != bottomRight) {
        d->bounds.setBottomRight(bottomRight);
        objectUpdated();
        emit bottomRightChanged(d->bounds.bottomRight());
    }
}

/*!
    \property QGeoMapRectangleObject::pen
    \brief This property holds the pen that will be used to draw this object.

    The pen is used to draw an outline around the rectangle. The rectangle is
    filled using the QGeoMapRectangleObject::brush property.

    The pen will be treated as a cosmetic pen, which means that the width
    of the pen will be independent of the zoom level of the map.
*/
QPen QGeoMapRectangleObject::pen() const
{
    Q_D(const QGeoMapRectangleObject);
    return d->pen;
}

void QGeoMapRectangleObject::setPen(const QPen &pen)
{
    Q_D(QGeoMapRectangleObject);

    QPen newPen = pen;
    newPen.setCosmetic(true);

    if (d->pen == newPen)
        return;

    d->pen = newPen;
    objectUpdated();
    emit penChanged(d->pen);
}

/*!
    \property QGeoMapRectangleObject::brush
    \brief This property holds the brush that will be used to draw this object.

    The brush is used to fill in rectangle.

    The outline around the perimeter of the rectangle is drawn using the
    QGeoMapRectangleObject::pen property.
*/
QBrush QGeoMapRectangleObject::brush() const
{
    Q_D(const QGeoMapRectangleObject);
    return d->brush;
}

void QGeoMapRectangleObject::setBrush(const QBrush &brush)
{
    Q_D(QGeoMapRectangleObject);
    if (d->brush != brush) {
        d->brush = brush;
        objectUpdated();
        emit brushChanged(d->brush);
    }
}

/*******************************************************************************
*******************************************************************************/

QGeoMapRectangleObjectPrivate::QGeoMapRectangleObjectPrivate(QGeoMapObject *impl, QGeoMapObject *parent)
        : QGeoMapObjectPrivate(impl, parent, QGeoMapObject::RectangleType)
{
    pen.setCosmetic(true);
}

QGeoMapRectangleObjectPrivate::~QGeoMapRectangleObjectPrivate() {}

#include "moc_qgeomaprectangleobject.cpp"

QTM_END_NAMESPACE

