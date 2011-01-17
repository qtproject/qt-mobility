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
    Constructs a new rectangle object.
*/
QGeoMapRectangleObject::QGeoMapRectangleObject()
    : d_ptr(new QGeoMapRectangleObjectPrivate())
{
    setGraphicsItem(d_ptr->item);
    setUnits(QGeoMapObject::RelativeArcSecondUnit);
}

/*!
    Constructs a new rectangle object based on the bounding box \a boundingBox.
*/
QGeoMapRectangleObject::QGeoMapRectangleObject(const QGeoBoundingBox &boundingBox)
    : d_ptr(new QGeoMapRectangleObjectPrivate())
{
    d_ptr->bounds = boundingBox;
    setGraphicsItem(d_ptr->item);
    setUnits(QGeoMapObject::RelativeArcSecondUnit);
    setOrigin(boundingBox.center());
    d_ptr->item->setRect(-0.5*boundingBox.width()*3600.0, -0.5*boundingBox.height()*3600.0,
                         0.5*boundingBox.width()*3600.0, 0.5*boundingBox.height()*3600.0);
}

/*!
    Constructs a new rectangle object with the top left coordinate at \a
    topLeft and the bottom right coordinate at \a bottomRight.
*/
QGeoMapRectangleObject::QGeoMapRectangleObject(const QGeoCoordinate &topLeft, const QGeoCoordinate &bottomRight)
    : d_ptr(new QGeoMapRectangleObjectPrivate())
{
    d_ptr->bounds = QGeoBoundingBox(topLeft, bottomRight);
    setGraphicsItem(d_ptr->item);
    setUnits(QGeoMapObject::RelativeArcSecondUnit);
    setOrigin(d_ptr->bounds.center());
    d_ptr->item->setRect(-0.5*d_ptr->bounds.width()*3600.0, -0.5*d_ptr->bounds.height()*3600.0,
                         0.5*d_ptr->bounds.width()*3600.0, 0.5*d_ptr->bounds.height()*3600.0);
}

/*!
    Destroys this rectangle object.
*/
QGeoMapRectangleObject::~QGeoMapRectangleObject()
{
    delete d_ptr;
}

/*!
    \reimp
*/
QGeoMapObject::Type QGeoMapRectangleObject::type() const
{
    return QGeoMapObject::RectangleType;
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
    return d_ptr->bounds;
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
    QGeoBoundingBox oldBounds = d_ptr->bounds;

    if (oldBounds == bounds)
        return;

    d_ptr->bounds = bounds;
    setOrigin(d_ptr->bounds.center());
    d_ptr->item->setRect(-0.5*d_ptr->bounds.width()*3600.0, -0.5*d_ptr->bounds.height()*3600.0,
                         0.5*d_ptr->bounds.width()*3600.0, 0.5*d_ptr->bounds.height()*3600.0);

    if (d_ptr->bounds.topLeft() != oldBounds.topLeft())
        emit topLeftChanged(d_ptr->bounds.topLeft());

    if (d_ptr->bounds.bottomRight() != oldBounds.bottomRight())
        emit bottomRightChanged(d_ptr->bounds.bottomRight());

    emit mapAppearanceChanged();
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
    return d_ptr->bounds.topLeft();
}

void QGeoMapRectangleObject::setTopLeft(const QGeoCoordinate &topLeft)
{
    if (d_ptr->bounds.topLeft() != topLeft) {
        d_ptr->bounds.setTopLeft(topLeft);
        emit topLeftChanged(d_ptr->bounds.topLeft());
        setOrigin(d_ptr->bounds.center());
        d_ptr->item->setRect(-0.5*d_ptr->bounds.width()*3600.0, -0.5*d_ptr->bounds.height()*3600.0,
                             0.5*d_ptr->bounds.width()*3600.0, 0.5*d_ptr->bounds.height()*3600.0);
        emit mapAppearanceChanged();
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
    return d_ptr->bounds.bottomRight();
}

void QGeoMapRectangleObject::setBottomRight(const QGeoCoordinate &bottomRight)
{
    if (d_ptr->bounds.bottomRight() != bottomRight) {
        d_ptr->bounds.setBottomRight(bottomRight);
        emit bottomRightChanged(d_ptr->bounds.bottomRight());
        setOrigin(d_ptr->bounds.center());
        d_ptr->item->setRect(-0.5*d_ptr->bounds.width()*3600.0, -0.5*d_ptr->bounds.height()*3600.0,
                             0.5*d_ptr->bounds.width()*3600.0, 0.5*d_ptr->bounds.height()*3600.0);
        emit mapAppearanceChanged();
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
    return d_ptr->item->pen();
}

void QGeoMapRectangleObject::setPen(const QPen &pen)
{
    QPen newPen = pen;
    newPen.setCosmetic(true);

    if (d_ptr->item->pen() == newPen)
        return;

    d_ptr->item->setPen(newPen);
    emit penChanged(newPen);
    emit mapAppearanceChanged();
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
    return d_ptr->item->brush();
}

void QGeoMapRectangleObject::setBrush(const QBrush &brush)
{
    if (d_ptr->item->brush() != brush) {
        d_ptr->item->setBrush(brush);
        emit brushChanged(d_ptr->item->brush());
        emit mapAppearanceChanged();
    }
}

/*!
\fn void QGeoMapRectangleObject::topLeftChanged(const QGeoCoordinate &topLeft)

    This signal is emitted when the top left coordinate of this rectangle 
    object has changed.

    The new value is \a topLeft.
*/

/*!
\fn void QGeoMapRectangleObject::bottomRightChanged(const QGeoCoordinate &bottomRight)

    This signal is emitted when the bottom right coordinate of this rectangle 
    object has changed.

    The new value is \a bottomRight.
*/

/*!
\fn void QGeoMapRectangleObject::penChanged(const QPen &pen)

    This signal is emitted when the pen used to draw the perimeter of this 
    rectangle object has changed.

    The new value is \a pen.
*/

/*!
\fn void QGeoMapRectangleObject::brushChanged(const QBrush &brush)

    This signal is emitted when the brush used to fill in the interior of 
    this rectangle object has changed.

    The new value is \a brush.
*/

/*******************************************************************************
*******************************************************************************/

QGeoMapRectangleObjectPrivate::QGeoMapRectangleObjectPrivate() :
    item(new QGraphicsRectItem)
{
    QPen pen = item->pen();
    pen.setCosmetic(true);
    item->setPen(pen);
}

QGeoMapRectangleObjectPrivate::~QGeoMapRectangleObjectPrivate()
{
}

#include "moc_qgeomaprectangleobject.cpp"

QTM_END_NAMESPACE

