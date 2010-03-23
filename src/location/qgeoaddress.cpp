
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

#include "qgeoaddress.h"
#include "qgeoaddress_p.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoAddress
    \brief The QGeoAddress class represents an address
    \ingroup location

    This class represents an address of a location.

    \sa QGeocodingReply
*/

/*!
    Default constructor.
*/
QGeoAddress::QGeoAddress()
{
}

/*!
    Returns the country name according to ISO 3166-1 alpha-3
*/
QString QGeoAddress::country() const
{
    Q_D(const QGeoAddress);
    return d->sCountry;
}

/*!
    Sets the \a country name according to ISO 3166-1 alpha-3
*/
void QGeoAddress::setCountry(const QString &country)
{
    Q_D(QGeoAddress);
    d->sCountry = country;
}

/*!
    Returns the country code according to ISO 3166-1 alpha-3
*/
QString QGeoAddress::countryCode() const
{
    Q_D(const QGeoAddress);
    return d->sCountryCode;
}

/*!
    Sets the \a countryCode according to ISO 3166-1 alpha-3
*/
void QGeoAddress::setCountryCode(const QString &countryCode)
{
    Q_D(QGeoAddress);
    d->sCountryCode = countryCode;
}

/*!
    Returns the state.
*/
QString QGeoAddress::state() const
{
    Q_D(const QGeoAddress);
    return d->sState;
}

/*!
    Sets the \a state.
*/
void QGeoAddress::setState(const QString &state)
{
    Q_D(QGeoAddress);
    d->sState = state;
}

/*!
    Returns the county.
*/
QString QGeoAddress::county() const
{
    Q_D(const QGeoAddress);
    return d->sCounty;
}

/*!
    Sets the \a county.
*/
void QGeoAddress::setCounty(const QString &county)
{
    Q_D(QGeoAddress);
    d->sCounty = county;
}

/*!
    Returns the city.
*/
QString QGeoAddress::city() const
{
    Q_D(const QGeoAddress);
    return d->sCity;
}

/*!
    Sets the \a city.
*/
void QGeoAddress::setCity(const QString &city)
{
    Q_D(QGeoAddress);
    d->sCity = city;
}

/*!
    Returns the district.
*/
QString QGeoAddress::district() const
{
    Q_D(const QGeoAddress);
    return d->sDistrict;
}

/*!
    Sets the \a district.
*/
void QGeoAddress::setDistrict(const QString &district)
{
    Q_D(QGeoAddress);
    d->sDistrict = district;
}

/*!
    Returns the thoroughfare name.
*/
QString QGeoAddress::thoroughfareName() const
{
    Q_D(const QGeoAddress);
    return d->sThoroughfareName;
}

/*!
    Sets the \a thoroughfareName.
*/
void QGeoAddress::setThoroughfareName(const QString &thoroughfareName)
{
    Q_D(QGeoAddress);
    d->sThoroughfareName = thoroughfareName;
}

/*!
    Returns the thoroughfare number.
*/
QString QGeoAddress::thoroughfareNumber() const
{
    Q_D(const QGeoAddress);
    return d->sThoroughfareNumber;
}

/*!
    Sets the \a thoroughfareNumber.
*/
void QGeoAddress::setThoroughfareNumber(const QString &thoroughfareNumber)
{
    Q_D(QGeoAddress);
    d->sThoroughfareNumber = thoroughfareNumber;
}

/*!
    Returns the post code.
*/
QString QGeoAddress::postCode() const
{
    Q_D(const QGeoAddress);
    return d->sPostCode;
}

/*!
    Sets the \a postCode.
*/
void QGeoAddress::setPostCode(const QString &postCode)
{
    Q_D(QGeoAddress);
    d->sPostCode = postCode;
}

/*!
   Returns the post office box.
*/
QString QGeoAddress::postOfficeBox() const
{
    Q_D(const QGeoAddress);
    return d->sPostOfficeBox;
}

/*!
   Sets the \a postOfficeBox.
*/
void QGeoAddress::setPostOfficeBox(const QString &postOfficeBox)
{
    Q_D(QGeoAddress);
    d->sPostOfficeBox = postOfficeBox;
}

/*!
    Returns whether this address is empty. An address is considered empty
    if \i all of its fields are empty.
*/
bool QGeoAddress::isEmpty() const
{
    Q_D(const QGeoAddress);
    return d->sCountry.isEmpty() &&
           d->sCountryCode.isEmpty() &&
           d->sState.isEmpty() &&
           d->sCounty.isEmpty() &&
           d->sCity.isEmpty() &&
           d->sDistrict.isEmpty() &&
           d->sThoroughfareName.isEmpty() &&
           d->sThoroughfareNumber.isEmpty() &&
           d->sPostCode.isEmpty();
}

QTM_END_NAMESPACE

