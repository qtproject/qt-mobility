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

QTM_BEGIN_NAMESPACE

QGeoBoundingBox::QGeoBoundingBox()
{
}

QGeoBoundingBox::QGeoBoundingBox(const QGeoCoordinate topLeft, const QGeoCoordinate bottomRight)
{
    coordTL = topLeft;
    coordBR = bottomRight;
}

QGeoBoundingBox::~QGeoBoundingBox()
{
}

QGeoCoordinate QGeoBoundingBox::topLeft() const
{
    return coordTL;
}

QGeoCoordinate QGeoBoundingBox::bottomRight() const
{
    return coordBR;    
}
void QGeoBoundingBox::setTopLeft(const QGeoCoordinate topLeft)
{
    coordTL = topLeft;
}

void QGeoBoundingBox::setBottomRight(const QGeoCoordinate bottomRight)
{
    coordBR = bottomRight;    
}

QGeoCoordinate QGeoBoundingBox::getCenter() const
{
    if(!coordTL.isValid() || !coordBR.isValid())
        return QGeoCoordinate(0, 0, 0);

    double cLat = (coordTL.latitude()+coordBR.latitude())/2;

    double left = coordTL.longitude();
    double cLong = (getWidth()/2)+left;
    if(cLong>180) //going over date line
        cLong -= 360.0;

    double cAlt = (coordTL.altitude()+coordBR.altitude())/2;

    return QGeoCoordinate(cLat, cLong, cAlt);
}

double QGeoBoundingBox::getHeight() const
{
    return coordBR.latitude()-coordTL.latitude();
}

double QGeoBoundingBox::getWidth() const
{
    double left = coordTL.longitude();
    double right = coordBR.longitude();
    if(left>right) //going over date line
        right += 360;

    return right-left;
}

void QGeoBoundingBox::resizeToCenter(const QGeoCoordinate /*center*/)
{
    // TODO: implement QGeoBoundingBox::resizeToCenter
}

bool QGeoBoundingBox::contains(const QGeoCoordinate coord)
{
    if(isEmpty() || !coord.isValid())
        return false;

    if (coord.latitude() > coordTL.latitude() || coord.latitude() < coordBR.latitude())
        return false;
    
    double left = coordTL.longitude();
    double right = coordBR.longitude();
    
    double cLong = coord.longitude();
    if (left>right && cLong<left && cLong > right) { //going over date line
        return false;
    }
    else if (cLong < left || cLong > right) {
        return false;
    }

    return true;
}

bool QGeoBoundingBox::contains(const QGeoBoundingBox bbox)
{
    return contains(bbox.topLeft()) && contains(bbox.bottomRight());
}

bool QGeoBoundingBox::intersects(const QGeoBoundingBox bbox) const
{
    double top = coordTL.latitude();
    double bot = coordBR.latitude();
    double bbTop = bbox.topLeft().latitude();
    double bbBot = bbox.bottomRight().latitude();
    if (top < bbBot || bbTop < bot)
        return false;
    
    double left = coordTL.longitude();
    double right = coordBR.longitude();
    if (left>right) //going over date line
        right += 360;

    double bbLeft = bbox.topLeft().longitude();
    double bbRight = bbox.bottomRight().longitude();
    if (bbLeft>bbRight) //going over date line
        bbRight += 360;

    if (left > bbRight || bbLeft > right)
        return false;

    return true;
}

bool QGeoBoundingBox::isEmpty() const
{
    if(!coordTL.isValid() || !coordBR.isValid())
        return true;

    if(getWidth()==0 || getHeight()==0)
        return true;

    return false;
}

QTM_END_NAMESPACE
