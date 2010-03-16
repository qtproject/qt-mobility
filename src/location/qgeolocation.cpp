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

#include "qgeolocation.h"


QTM_BEGIN_NAMESPACE

/*!
    \class QGeoLocation
    \brief The QGeoLocation class represents a geolocation.
    \ingroup location

    This class represent a geo location as returned by QGeocodingReply::places().
*/

/*!
    Default constructor.
*/
QGeoLocation::QGeoLocation() {}

/*!
    Returns the bounding box that completely encloses the location.

    The x coordinates of the corner points represent longitudes,
    the y coordinates represent latitudes.
*/
QRectF QGeoLocation::boundingBox() const
{
    return box;
}
/*!
    Returns the geocoordinate of this location.
*/
QGeoCoordinate QGeoLocation::position() const
{
    return pos;
}
/*!
    Returns a description of the location.
*/
QString QGeoLocation::title() const
{
    return ttl;
}
/*!
    Returns the MARC language used in the description of this location.
*/
QString QGeoLocation::language() const
{
    return lang;
}
/*!
    Returns the address found.
*/
QAddress QGeoLocation::address() const
{
    return addr;
}
/*!
    Returns alternatives to the address found.
*/
QAlternativeAddress QGeoLocation::alternatives() const
{
    return altern;
}

QTM_END_NAMESPACE

