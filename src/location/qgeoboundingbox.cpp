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

#include "qgeoboundingbox.h"
#include "qgeoboundingbox_p.h"

#include "qgeocoordinate.h"
#include "qnumeric.h"

#include <QDebug>

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoBoundingBox
    \brief The QGeoBoundingBox class defines a rectangular geographic area.

    \inmodule QtLocation
    
    \ingroup maps
*/

/*!
*/
QGeoBoundingBox::QGeoBoundingBox()
        : d_ptr(new QGeoBoundingBoxPrivate()) {}

/*!
*/
QGeoBoundingBox::QGeoBoundingBox(const QGeoCoordinate &center, double degreesWidth, double degreesHeight)
        : d_ptr(new QGeoBoundingBoxPrivate(center, center))
{
        setWidth(degreesWidth);
        setHeight(degreesHeight);
}

/*!
*/
QGeoBoundingBox::QGeoBoundingBox(const QGeoCoordinate &topLeft, const QGeoCoordinate &bottomRight)
        : d_ptr(new QGeoBoundingBoxPrivate(topLeft, bottomRight)) {}

/*!
*/
QGeoBoundingBox::QGeoBoundingBox(const QGeoBoundingBox &other)
        : QGeoBoundingArea(other),
          d_ptr(other.d_ptr) {}

/*!
*/
QGeoBoundingBox::~QGeoBoundingBox() {}

/*!
*/
QGeoBoundingBox& QGeoBoundingBox::operator = (const QGeoBoundingBox & other)
{
    QGeoBoundingArea::operator=(other);
    d_ptr = other.d_ptr;
    return *this;
}

/*!
*/
bool QGeoBoundingBox::operator == (const QGeoBoundingBox &other) const
{
    return ((*(d_ptr.constData()) == *(other.d_ptr.constData())));
}

/*!
*/
bool QGeoBoundingBox::operator != (const QGeoBoundingBox &other) const
{
    return !(this->operator==(other));
}

/*!
*/
QGeoBoundingArea::AreaType QGeoBoundingBox::type() const
{
    return QGeoBoundingArea::BoxType;
}

/*!
*/
bool QGeoBoundingBox::isValid() const
{
    return (d_ptr->topLeft.isValid()
            && d_ptr->bottomRight.isValid()
            && d_ptr->topLeft.latitude() >= d_ptr->bottomRight.latitude());
}

/*!
*/
bool QGeoBoundingBox::isEmpty() const
{
    return (!isValid()
            || (d_ptr->topLeft.latitude() == d_ptr->bottomRight.latitude())
            || (d_ptr->topLeft.longitude() == d_ptr->bottomRight.longitude()));
}

/*!
*/
void QGeoBoundingBox::setTopLeft(const QGeoCoordinate &topLeft)
{
    d_ptr->topLeft = topLeft;
}

/*!
*/
QGeoCoordinate QGeoBoundingBox::topLeft() const
{
    return d_ptr->topLeft;
}

/*!
*/
void QGeoBoundingBox::setTopRight(const QGeoCoordinate &topRight)
{
    d_ptr->topLeft.setLatitude(topRight.latitude());
    d_ptr->bottomRight.setLongitude(topRight.longitude());
}

/*!
*/
QGeoCoordinate QGeoBoundingBox::topRight() const
{
    if (!isValid())
        return QGeoCoordinate();

    return QGeoCoordinate(d_ptr->topLeft.latitude(), d_ptr->bottomRight.longitude());
}

/*!
*/
void QGeoBoundingBox::setBottomLeft(const QGeoCoordinate &bottomLeft)
{
    d_ptr->bottomRight.setLatitude(bottomLeft.latitude());
    d_ptr->topLeft.setLongitude(bottomLeft.longitude());
}

/*!
*/
QGeoCoordinate QGeoBoundingBox::bottomLeft() const
{
    if (!isValid())
        return QGeoCoordinate();

    return QGeoCoordinate(d_ptr->bottomRight.latitude(), d_ptr->topLeft.longitude());
}

/*!
*/
void QGeoBoundingBox::setBottomRight(const QGeoCoordinate &bottomRight)
{
    d_ptr->bottomRight = bottomRight;
}

/*!
*/
QGeoCoordinate QGeoBoundingBox::bottomRight() const
{
    return d_ptr->bottomRight;
}

/*!
*/
void QGeoBoundingBox::setCenter(const QGeoCoordinate &center)
{
    double width = this->width();
    double height = this->height();

    double tlLat = center.latitude() + height / 2.0;
    double tlLon = center.longitude() - width / 2.0;
    double brLat = center.latitude() - height / 2.0;
    double brLon = center.longitude() + width / 2.0;

    if (tlLon < -180.0)
        tlLon += 360.0;
    if (tlLon > 180.0)
        tlLon -= 360.0;

    if (brLon < -180.0)
        brLon += 360.0;
    if (brLon > 180.0)
        brLon -= 360.0;

    if (tlLat > 90.0) {
        brLat = 2* center.latitude() - 90.0;
        tlLat = 90.0;
    }

    if (tlLat < -90.0) {
        brLat = -90.0;
        tlLat = -90.0;
    }

    if (brLat > 90.0) {
        tlLat = 90.0;
        brLat = 90.0;
    }

    if (brLat < -90.0) {
        tlLat = 2 * center.latitude() + 90.0;
        brLat = -90.0;
    }

    if (width == 360.0) {
        tlLon = -180.0;
        brLon = 180.0;
    }

    d_ptr->topLeft = QGeoCoordinate(tlLat, tlLon);
    d_ptr->bottomRight = QGeoCoordinate(brLat, brLon);
}

/*!
*/
QGeoCoordinate QGeoBoundingBox::center() const
{
    if (!isValid())
        return QGeoCoordinate();

    double cLat = (d_ptr->topLeft.latitude() + d_ptr->bottomRight.latitude()) / 2.0;

    double cLon = (d_ptr->bottomRight.longitude() + d_ptr->topLeft.longitude()) / 2.0;
    if (d_ptr->topLeft.longitude() > d_ptr->bottomRight.longitude()) {
        cLon = cLon - 180.0;
    }

    if (cLon < -180.0)
        cLon += 360.0;
    if (cLon > 180.0)
        cLon -= 360.0;

    return QGeoCoordinate(cLat, cLon);
}

/*!
*/
void QGeoBoundingBox::setWidth(double degreesWidth)
{
    if (!isValid())
        return;

    if (degreesWidth < 0.0)
        return;

    if (degreesWidth >= 360.0) {
        d_ptr->topLeft.setLongitude(-180.0);
        d_ptr->bottomRight.setLongitude(180.0);
        return;
    }

    double tlLat = d_ptr->topLeft.latitude();
    double brLat = d_ptr->bottomRight.latitude();

    QGeoCoordinate c = center();

    double tlLon = c.longitude() - degreesWidth / 2.0;

    if (tlLon < -180.0)
        tlLon += 360.0;
    if (tlLon > 180.0)
        tlLon -= 360.0;

    double brLon = c.longitude() + degreesWidth / 2.0;

    if (brLon < -180.0)
        brLon += 360.0;
    if (brLon > 180.0)
        brLon -= 360.0;

    d_ptr->topLeft = QGeoCoordinate(tlLat, tlLon);
    d_ptr->bottomRight = QGeoCoordinate(brLat, brLon);
}

/*!
*/
double QGeoBoundingBox::width() const
{
    if (!isValid())
        return qQNaN();

    double result = d_ptr->bottomRight.longitude() - d_ptr->topLeft.longitude();
    if (result < 0.0)
        result += 360.0;
    if (result > 360.0)
        result -= 360.0;

    return result;
}

/*!
*/
void QGeoBoundingBox::setHeight(double degreesHeight)
{
    if (!isValid())
        return;

    if (degreesHeight < 0.0)
        return;

    if (degreesHeight >= 180.0) {
        degreesHeight = 180.0;
    }

    double tlLon = d_ptr->topLeft.longitude();
    double brLon = d_ptr->bottomRight.longitude();

    QGeoCoordinate c = center();

    double tlLat = c.latitude() + degreesHeight / 2.0;
    double brLat = c.latitude() - degreesHeight / 2.0;

    if (tlLat > 90.0) {
        brLat = 2* c.latitude() - 90.0;
        tlLat = 90.0;
    }

    if (tlLat < -90.0) {
        brLat = -90.0;
        tlLat = -90.0;
    }

    if (brLat > 90.0) {
        tlLat = 90.0;
        brLat = 90.0;
    }

    if (brLat < -90.0) {
        tlLat = 2 * c.latitude() + 90.0;
        brLat = -90.0;
    }

    d_ptr->topLeft = QGeoCoordinate(tlLat, tlLon);
    d_ptr->bottomRight = QGeoCoordinate(brLat, brLon);
}

/*!
*/
double QGeoBoundingBox::height()
{
    if (!isValid())
        return qQNaN();

    double result = d_ptr->topLeft.latitude() - d_ptr->bottomRight.latitude();
    if (result < 0.0)
        result = qQNaN();
    return result;
}

/*!
*/
bool QGeoBoundingBox::contains(const QGeoCoordinate &coordinate) const
{

    if (!isValid() || !coordinate.isValid())
        return false;

    double left = d_ptr->topLeft.longitude();
    double right = d_ptr->bottomRight.longitude();
    double top = d_ptr->topLeft.latitude();
    double bottom = d_ptr->bottomRight.latitude();

    double lon = coordinate.longitude();
    double lat = coordinate.latitude();

    if (lat > top)
        return false;
    if (lat < bottom)
        return false;

    if ((lat == 90.0) && (top == 90.0))
        return true;

    if ((lat == -90.0) && (bottom == -90.0))
        return true;

    if (left <= right) {
        if ((lon < left) || (lon > right))
            return false;
    } else {
        if ((lon < left) && (lon > right))
            return false;
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
    double left1 = d_ptr->topLeft.longitude();
    double right1 = d_ptr->bottomRight.longitude();
    double top1 = d_ptr->topLeft.latitude();
    double bottom1 = d_ptr->bottomRight.latitude();

    double left2 = boundingBox.d_ptr->topLeft.longitude();
    double right2 = boundingBox.d_ptr->bottomRight.longitude();
    double top2 = boundingBox.d_ptr->topLeft.latitude();
    double bottom2 = boundingBox.d_ptr->bottomRight.latitude();

    if (top1 < bottom2)
        return false;

    if (bottom1 > top2)
        return false;

    if ((top1 == 90.0) && (top1 == top2))
        return true;

    if ((bottom1 == -90.0) && (bottom1 == bottom2))
        return true;

    if (left1 < right1) {
        if (left2 < right2) {
            if ((left1 > right2) || (right1 < left2))
                return false;
        } else {
            if ((left1 > right2) && (right1 < left2))
                return false;
        }
    } else {
        if (left2 < right2) {
            if ((left2 > right1) && (right2 < left1))
                return false;
        } else {
            // if both wrap then they have to intersect
        }
    }

    return true;
}

/*!
*/
void QGeoBoundingBox::translate(double degreesLatitude, double degreesLongitude)
{
    double tlLat = d_ptr->topLeft.latitude();
    double tlLon = d_ptr->topLeft.longitude();
    double brLat = d_ptr->bottomRight.latitude();
    double brLon = d_ptr->bottomRight.longitude();

    if ((tlLat != 90.0) || (brLat != -90.0)) {
        tlLat += degreesLatitude;
        brLat += degreesLatitude;
    }

    if ( (tlLon != -180.0) || (brLon != 180.0) ) {
        tlLon += degreesLongitude;
        brLon += degreesLongitude;
    }

    if (tlLon < -180.0)
        tlLon += 360.0;
    if (tlLon > 180.0)
        tlLon -= 360.0;

    if (brLon < -180.0)
        brLon += 360.0;
    if (brLon > 180.0)
        brLon -= 360.0;

    if (tlLat > 90.0)
        tlLat = 90.0;

    if (tlLat < -90.0)
        tlLat = -90.0;

    if (brLat > 90.0)
        brLat = 90.0;

    if (brLat < -90.0)
        brLat = -90.0;

    d_ptr->topLeft = QGeoCoordinate(tlLat, tlLon);
    d_ptr->bottomRight = QGeoCoordinate(brLat, brLon);
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
  \fn QGeoBoundingBox QGeoBoundingBox::operator | (const QGeoBoundingBox &boundingBox) const
*/

/*!
*/
QGeoBoundingBox& QGeoBoundingBox::operator |= (const QGeoBoundingBox & boundingBox)
{
    // If non-intersecting goes for least narrow box

    double left1 = d_ptr->topLeft.longitude();
    double right1 = d_ptr->bottomRight.longitude();
    double top1 = d_ptr->topLeft.latitude();
    double bottom1 = d_ptr->bottomRight.latitude();

    double left2 = boundingBox.d_ptr->topLeft.longitude();
    double right2 = boundingBox.d_ptr->bottomRight.longitude();
    double top2 = boundingBox.d_ptr->topLeft.latitude();
    double bottom2 = boundingBox.d_ptr->bottomRight.latitude();

    double top = qMax(top1, top2);
    double bottom = qMin(bottom1, bottom2);

    double left = 0.0;
    double right = 0.0;

    bool wrap1 = (left1 > right1);
    bool wrap2 = (left2 > right2);

    if ((wrap1 && wrap2) || (!wrap1 && !wrap2)) {
        left = qMin(left1, left2);
        right = qMax(right1, right2);
    } else {
        double wrapLeft = 0.0;
        double wrapRight = 0.0;
        double nonWrapLeft = 0.0;
        double nonWrapRight = 0.0;

        if (wrap1) {
            wrapLeft = left1;
            wrapRight = right1;
            nonWrapLeft = left2;
            nonWrapRight = right2;
        } else {
            wrapLeft = left2;
            wrapRight = right2;
            nonWrapLeft = left1;
            nonWrapRight = right1;
        }

        bool joinWrapLeft = (nonWrapRight >= wrapLeft);
        bool joinWrapRight = (nonWrapLeft <= wrapRight);

        if (joinWrapLeft) {
            if (joinWrapRight) {
                left = -180.0;
                right = 180.0;
            } else {
                left = nonWrapLeft;
                right = wrapRight;
            }
        } else {
            if (joinWrapRight) {
                left = wrapLeft;
                right = nonWrapRight;
            } else {
                double wrapRightDistance = nonWrapLeft - wrapRight;
                double wrapLeftDistance = wrapLeft - nonWrapRight;

                if (wrapLeftDistance == wrapRightDistance) {
                    left = -180.0;
                    right = 180.0;
                } else if (wrapLeftDistance < wrapRightDistance) {
                    left = nonWrapLeft;
                    right = wrapRight;
                } else {
                    left = wrapLeft;
                    right = nonWrapRight;
                }
            }
        }
    }

    if (((left1 == -180) && (right1 == 180.0))
            || ((left2 == -180) && (right2 == 180.0))) {
        left = -180;
        right = 180;
    }

    d_ptr->topLeft = QGeoCoordinate(top, left);
    d_ptr->bottomRight = QGeoCoordinate(bottom, right);

    return *this;
}

/*******************************************************************************
*******************************************************************************/

QGeoBoundingBoxPrivate::QGeoBoundingBoxPrivate()
    : QSharedData() {}

QGeoBoundingBoxPrivate::QGeoBoundingBoxPrivate(const QGeoCoordinate &topLeft, const QGeoCoordinate &bottomRight)
        : QSharedData(),
        topLeft(topLeft),
        bottomRight(bottomRight) {}

QGeoBoundingBoxPrivate::QGeoBoundingBoxPrivate(const QGeoBoundingBoxPrivate &other)
        : QSharedData(),
        topLeft(other.topLeft),
        bottomRight(other.bottomRight) {}

QGeoBoundingBoxPrivate::~QGeoBoundingBoxPrivate() {}

QGeoBoundingBoxPrivate& QGeoBoundingBoxPrivate::operator= (const QGeoBoundingBoxPrivate & other)
{
    topLeft = other.topLeft;
    bottomRight = other.bottomRight;

    return *this;
}

bool QGeoBoundingBoxPrivate::operator== (const QGeoBoundingBoxPrivate &other) const
{
    return ((topLeft == other.topLeft) && (bottomRight == other.bottomRight));
}

QTM_END_NAMESPACE

