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

#include "qgeoboundingbox.h"
#include "qgeoboundingbox_p.h"

#include "qgeocoordinate.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoBoundingBox
    \brief The QGeoBoundingBox class defines a rectangular geographic area.
    \ingroup maps

    The rectangular region is specified by the upper left and lower right
    coordinates.

    If either of these coordinates are invalid then the QGeoBoundingBox is
    considered to be invalid as well.

    The QGeoBoundingBox class is intended for use with the maps and navigation
    services.  These services assume that the maps will be displayed using the
    Mercator projection, under which the poles of the map can be asymptotically
    approached but never reached.

    A consequence of this is that is not possible to specify a QGeoBoundingBox
    which is centred one one of the poles.

    Note that this would be possible if the rectangular region was defined by
    three coordiantes to eliminate potential ambiguity, however being able to
    specify such a region is not useful in this context.
*/

/*!
    Constructs a QGeoBoundingBox object.

    The bounding box will be invalid until setUpperLeft() and setLowerRight()
    have been called.
*/
QGeoBoundingBox::QGeoBoundingBox()
    : d_ptr(new QGeoBoundingBoxPrivate()) {}

/*!
    Constucts a QGeoBoundingBox object from the contents of \a other.
*/
QGeoBoundingBox::QGeoBoundingBox(const QGeoBoundingBox &other)
    : d_ptr(new QGeoBoundingBoxPrivate(*(other.d_ptr))) {}

/*!
    Constructs a QGeoBoundingBox object with the given \a upperLeft and \a lowerRight
    coordinates.
*/
QGeoBoundingBox::QGeoBoundingBox(const QGeoCoordinate &upperLeft,
                                 const QGeoCoordinate &lowerRight)
                                     : d_ptr(new QGeoBoundingBoxPrivate(upperLeft, lowerRight)) {}

/*!
  Destroys this QGeoBoundingBox object.
*/
QGeoBoundingBox::~QGeoBoundingBox()
{
    Q_D(QGeoBoundingBox);
    delete d;
}

/*!
    Assigns \a other to this QGeoBoundingBox and returns a reference
    to this QGeoBoundingBox.
*/
QGeoBoundingBox& QGeoBoundingBox::operator= (const QGeoBoundingBox &other)
{
    *d_ptr = *(other.d_ptr);
    return *this;
}

/*!
  Returns whether or not this QGeoBoundingBox is valid.

  To be considered valid a QGeoBoundingBox must have valid coordinates
  specified for the upper left and lower right coordinate.
*/
bool QGeoBoundingBox::isValid() const
{
    Q_D(const QGeoBoundingBox);
    return (d->upperLeft.isValid() && d->lowerRight.isValid());
}

/*!
  Sets the upper left coordinate to \a upperLeft.
*/
void QGeoBoundingBox::setUpperLeft(const QGeoCoordinate &upperLeft)
{
    Q_D(QGeoBoundingBox);
    d->upperLeft = upperLeft;
}

/*!
  Returns the upper left coordinate.

  The returned value will be an invalid coordinate until the upper left
  coordinate has been set.
*/
QGeoCoordinate QGeoBoundingBox::upperLeft() const
{
    Q_D(const QGeoBoundingBox);
    return d->upperLeft;
}

/*!
  Sets the lower right coordinate to \a lowerRight.
*/
void QGeoBoundingBox::setLowerRight(const QGeoCoordinate &lowerRight)
{
    Q_D(QGeoBoundingBox);
    d->lowerRight = lowerRight;
}

/*!
  Returns the lower right coordinate.

  The returned value will be an invalid coordinate until the lower right
  coordinate has been set.
*/
QGeoCoordinate QGeoBoundingBox::lowerRight() const
{
    Q_D(const QGeoBoundingBox);
    return d->lowerRight;
}

/*******************************************************************************
*******************************************************************************/

QGeoBoundingBoxPrivate::QGeoBoundingBoxPrivate() {}

QGeoBoundingBoxPrivate::QGeoBoundingBoxPrivate(const QGeoCoordinate &upperLeft,
                                               const QGeoCoordinate &lowerRight)
                                                   : upperLeft(upperLeft),
                                                   lowerRight(lowerRight) {}

QGeoBoundingBoxPrivate::QGeoBoundingBoxPrivate(const QGeoBoundingBoxPrivate &other)
    : upperLeft(other.upperLeft),
    lowerRight(other.lowerRight) {}

QGeoBoundingBoxPrivate::~QGeoBoundingBoxPrivate() {}

QGeoBoundingBoxPrivate& QGeoBoundingBoxPrivate::operator= (const QGeoBoundingBoxPrivate &other)
{
    upperLeft = other.upperLeft;
    lowerRight = other.lowerRight;
    return *this;
}

QTM_END_NAMESPACE

