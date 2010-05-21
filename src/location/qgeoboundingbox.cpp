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
#include "qnumeric.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoBoundingBox
    \brief The QGeoBoundingBox class defines a rectangular geographic area.
    \ingroup maps
*/

/*!
*/
QGeoBoundingBox::QGeoBoundingBox()
    : d_ptr(new QGeoBoundingBoxPrivate()) {}

/*!
*/
QGeoBoundingBox::QGeoBoundingBox(const QGeoCoordinate &center, double degreesWidth, double degreesHeight)
    : d_ptr(new QGeoBoundingBoxPrivate(center, degreesWidth, degreesHeight)) {}

/*!
*/
QGeoBoundingBox::QGeoBoundingBox(const QGeoCoordinate &topLeft, const QGeoCoordinate &bottomRight)
    : d_ptr(new QGeoBoundingBoxPrivate(topLeft, bottomRight)) {}

/*!
*/
QGeoBoundingBox::QGeoBoundingBox(const QGeoBoundingBox &other)
    : d_ptr(other.d_ptr) {}

/*!
*/
QGeoBoundingBox::~QGeoBoundingBox() {}

/*!
*/
QGeoBoundingBox& QGeoBoundingBox::operator = (const QGeoBoundingBox &other)
{
    d_ptr = other.d_ptr;
    return *this;
}

/*!
*/
bool QGeoBoundingBox::operator == (const QGeoBoundingBox &other) const
{
    return (d_ptr.constData() == other.d_ptr.constData());
}

/*!
*/
bool QGeoBoundingBox::operator != (const QGeoBoundingBox &other) const
{
    return !(this->operator==(other));
}

/*!
*/
bool QGeoBoundingBox::isValid() const
{
    return ((d_ptr->center.isValid())
                && (qIsFinite(d_ptr->width))
                && (qIsFinite(d_ptr->height)));
}

/*!
*/
bool QGeoBoundingBox::isEmpty() const
{
    return ((d_ptr->width == 0.0) || (d_ptr->height == 0.0));
}

/*!
*/
QGeoCoordinate QGeoBoundingBox::topLeft() const
{
    return QGeoCoordinate(d_ptr->center.latitude() + (d_ptr->height / 2.0),
                            d_ptr->center.longitude() - (d_ptr->width / 2.0));
}

/*!
*/
QGeoCoordinate QGeoBoundingBox::topRight() const
{
    return QGeoCoordinate(d_ptr->center.latitude() + (d_ptr->height / 2.0),
                            d_ptr->center.longitude() + (d_ptr->width / 2.0));
}

/*!
*/
QGeoCoordinate QGeoBoundingBox::bottomLeft() const
{
    return QGeoCoordinate(d_ptr->center.latitude() - (d_ptr->height / 2.0),
                            d_ptr->center.longitude() - (d_ptr->width / 2.0));
}

/*!
*/
QGeoCoordinate QGeoBoundingBox::bottomRight() const
{
    return QGeoCoordinate(d_ptr->center.latitude() - (d_ptr->height / 2.0),
                            d_ptr->center.longitude() + (d_ptr->width / 2.0));
}

/*!
*/
void QGeoBoundingBox::setCenter(const QGeoCoordinate &center)
{
    d_ptr->center = center;
}

/*!
*/
QGeoCoordinate QGeoBoundingBox::center() const
{
    return d_ptr->center;
}

/*!
*/
void QGeoBoundingBox::setWidth(double degreesWidth)
{
    if (degreesWidth > 360.0)
        d_ptr->width = 360.0;
    else
        d_ptr->width = degreesWidth;
}

/*!
*/
double QGeoBoundingBox::width() const
{
    return d_ptr->width;
}

/*!
*/
void QGeoBoundingBox::setHeight(double degreesHeight)
{
    if (degreesHeight > 360.0)
        d_ptr->height = 360.0;
    else
        d_ptr->height = degreesHeight;
}

/*!
*/
double QGeoBoundingBox::height()
{
    return d_ptr->height;
}

/*!
*/
bool QGeoBoundingBox::contains(const QGeoCoordinate &coordinate) const
{
    double leftBound = QGeoBoundingBoxPrivate::degreesLeft(d_ptr->center.longitude(), d_ptr->width / 2.0);
    double rightBound = QGeoBoundingBoxPrivate::degreesRight(d_ptr->center.longitude(), d_ptr->width / 2.0);
    double topBound = QGeoBoundingBoxPrivate::degreesUp(d_ptr->center.latitude(), d_ptr->height / 2.0);
    double bottomBound = QGeoBoundingBoxPrivate::degreesDown(d_ptr->center.latitude(), d_ptr->height / 2.0);

    if ((d_ptr->width == 0.0) || (d_ptr->height == 0.0))
        return false;

    if (d_ptr->width != 360.0) {
        if (leftBound < rightBound) {
            if ((coordinate.longitude() < leftBound) || (rightBound < coordinate.longitude()))
                return false;
        } else {
            if ((leftBound < coordinate.longitude()) && (coordinate.longitude() < rightBound))
                return false;
        }
    }

    if (d_ptr->height != 360.0) {
        if (bottomBound < topBound) {
            if ((coordinate.latitude() < bottomBound) || (topBound < coordinate.latitude()))
                return false;
        } else {
            if ((bottomBound < coordinate.latitude()) && (coordinate.latitude() < topBound))
                return false;
        }
    }

    return true;
}

/*!
*/
bool QGeoBoundingBox::contains(const QGeoBoundingBox &boundingBox) const
{
    return (contains(boundingBox.topLeft())
            && contains(boundingBox.topRight())
            && contains(boundingBox.bottomLeft())
            && contains(boundingBox.bottomRight()));
}

/*!
*/
bool QGeoBoundingBox::intersects(const QGeoBoundingBox &boundingBox) const
{
    return (contains(boundingBox.topLeft())
            || contains(boundingBox.topRight())
            || contains(boundingBox.bottomLeft())
            || contains(boundingBox.bottomRight()));
}

/*!
*/
void QGeoBoundingBox::translate(double degreesLatitude, double degreesLongitude)
{
    // TODO - mod degrees latitude into -90 < dlat < 90
    // TODO - mod degrees longitude into -180 < dlon < 180
    QGeoCoordinate center = d_ptr->center;
    center.setLatitude(QGeoBoundingBoxPrivate::latitudeShift(center.latitude(), degreesLatitude));
    center.setLongitude(QGeoBoundingBoxPrivate::longitudeShift(center.longitude(), degreesLongitude));
    d_ptr->center = center;
}

/*!
*/
QGeoBoundingBox QGeoBoundingBox::translated(double degreesLatitude, double degreesLongitude) const
{
    QGeoBoundingBox result(*this);
    result.translate(degreesLatitude, degreesLongitude);
    return result;
}

/*!
*/
QGeoBoundingBox QGeoBoundingBox::united(const QGeoBoundingBox &boundingBox) const
{
    QGeoBoundingBox result(*this);
    result |= boundingBox;
    return result;
}

/*!
*/
inline
QGeoBoundingBox QGeoBoundingBox::operator | (const QGeoBoundingBox &boundingBox) const
{
    return united(boundingBox);
}

/*!
*/
QGeoBoundingBox& QGeoBoundingBox::operator |= (const QGeoBoundingBox &boundingBox)
{
    // TODO
    return *this;
}

/*!
*/
QGeoBoundingBox QGeoBoundingBox::intersected(const QGeoBoundingBox &boundingBox) const
{
    QGeoBoundingBox result(*this);
    result &= boundingBox;
    return result;
}

/*!
*/
inline
QGeoBoundingBox QGeoBoundingBox::operator & (const QGeoBoundingBox &boundingBox) const
{
    return intersected(boundingBox);
}

/*!
*/
QGeoBoundingBox& QGeoBoundingBox::operator &= (const QGeoBoundingBox &boundingBox)
{
    // TODO
    return *this;
}

/*******************************************************************************
*******************************************************************************/

QGeoBoundingBoxPrivate::QGeoBoundingBoxPrivate() : QSharedData() {}

QGeoBoundingBoxPrivate::QGeoBoundingBoxPrivate(const QGeoCoordinate &center, double degreesWidth, double degreesHeight)
    : QSharedData(),
    center(center),
    width(degreesWidth),
    height(degreesHeight)
{
    if (degreesWidth > 360.0)
        width = 360.0;
    if (degreesHeight > 360.0)
        height = 360.0;
}

QGeoBoundingBoxPrivate::QGeoBoundingBoxPrivate(const QGeoCoordinate &topLeft, const QGeoCoordinate &bottomRight)
    : QSharedData()
{
    center = QGeoCoordinate((topLeft.latitude() + bottomRight.latitude()) / 2.0,
                             (topLeft.longitude() + bottomRight.longitude()) / 2.0);

    width = degreesLeft(center.longitude(), topLeft.longitude())
            + degreesRight(center.longitude(), bottomRight.longitude());
    height = degreesUp(center.latitude(), topLeft.latitude())
             + degreesDown(center.latitude(), bottomRight.latitude());
}

QGeoBoundingBoxPrivate::QGeoBoundingBoxPrivate(const QGeoBoundingBoxPrivate &other)
    : QSharedData(),
    center(other.center),
    width(other.width),
    height(other.height) {}

QGeoBoundingBoxPrivate::~QGeoBoundingBoxPrivate() {}

QGeoBoundingBoxPrivate& QGeoBoundingBoxPrivate::operator= (const QGeoBoundingBoxPrivate & other)
{
    center = other.center;
    width = other.width;
    height = other.height;

    return *this;
}

bool QGeoBoundingBoxPrivate::operator== (const QGeoBoundingBoxPrivate &other) const
{
    return ((center == other.center)
            && (width == other.width)
            && (height == other.height));
}

// assumes longitude is in the range  -180 <= l <= 180
double QGeoBoundingBoxPrivate::longitudeShift(double longitudeFrom, double delta)
{
    double result = longitudeFrom + delta;

    if (result < -180.0)
        result += 360.0;
    else if (result > 180.0)
        result -= 360.0;

    return result;
}

// assumes latitude is in the range  -90 <= l <= 90
double QGeoBoundingBoxPrivate::latitudeShift(double latitudeFrom, double delta)
{
    double result = latitudeFrom + delta;

    if (result < -90.0)
        result = -180.0 - result;
    else if (result > 90.0)
        result = 180.0 - result;

    return result;
}

// assumes longitude is in the range  -180 <= l <= 180
double QGeoBoundingBoxPrivate::degreesLeft(double longitudeFrom, double longitudeTo)
{
    if (longitudeFrom >= longitudeTo)
        return longitudeFrom - longitudeTo;
    else
        return 360.0 + longitudeFrom - longitudeTo;
}

// assumes longitude is in the range  -180 <= l <= 180
double QGeoBoundingBoxPrivate::degreesRight(double longitudeFrom, double longitudeTo)
{
    return degreesLeft(longitudeTo, longitudeFrom);
}

// assumes latitude is in the range  -90 <= l <= 90
double QGeoBoundingBoxPrivate::degreesUp(double latitudeFrom, double latitudeTo)
{
    if (latitudeTo >= latitudeFrom)
        return latitudeTo - latitudeFrom;
    else
        return 180.0 + latitudeTo - latitudeFrom;
}

// assumes latitude is in the range  -90 <= l <= 90
double QGeoBoundingBoxPrivate::degreesDown(double latitudeFrom, double latitudeTo)
{
    return degreesUp(latitudeTo, latitudeFrom);
}

QTM_END_NAMESPACE

