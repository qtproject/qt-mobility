
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

#include "qaddress.h"

#include <QString>

QTM_BEGIN_NAMESPACE

/*!
    \class QAddress
    \brief The QAddress class represents an address
    \ingroup location

    This class represents an address as returned by a geocoding
    reply and as contained in QGeoLocation.

    \sa QGeocodingReply
*/

/*!
    Default constructor.
*/
QAddress::QAddress() {}

/*!
    Returns the country name according to ISO 3166-1 alpha-3
*/
QString QAddress::country() const
{
    return sCountry;
}

/*!
    Returns the country code according to ISO 3166-1 alpha-3
*/
QString QAddress::countryCode() const
{
    return sCountryCode;
}

/*!
    Returns the state.
*/
QString QAddress::state() const
{
    return sState;
}

/*!
    Returns the county.
*/
QString QAddress::county() const
{
    return sCounty;
}

/*!
    Returns the city.
*/
QString QAddress::city() const
{
    return sCity;
}

/*!
    Returns the district.
*/
QString QAddress::district() const
{
    return sDistrict;
}

/*!
    Returns the thoroughfare name.
*/
QString QAddress::thoroughfareName() const
{
    return sThoroughfareName;
}

/*!
    Returns the thoroughfare number.
*/
QString QAddress::thoroughfareNumber() const
{
    return sThoroughfareNumber;
}

/*!
    Returns the post code.
*/
QString QAddress::postCode() const
{
    return sPostCode;
}

/*!
    Returns whether this address is empty. An address is considered empty
    if \i all of its fields are empty.
*/

bool QAddress::isEmpty() const
{
    return sCountry.isEmpty() &&
           sCountryCode.isEmpty() &&
           sState.isEmpty() &&
           sCounty.isEmpty() &&
           sCity.isEmpty() &&
           sDistrict.isEmpty() &&
           sThoroughfareName.isEmpty() &&
           sThoroughfareNumber.isEmpty() &&
           sPostCode.isEmpty();
}

QTM_END_NAMESPACE

