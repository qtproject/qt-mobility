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

#include "qgeocodingrequest.h"
#include "qreversegeocodingrequest.h"
#include "qgeocodingreply.h"

QTM_BEGIN_NAMESPACE

//------------------------------------------------------------------------------
//QGeocodingRequest

QGeocodingRequest::QGeocodingRequest()
{
    vers = "1.0";
    languageMARC = "eng";
}

QString QGeocodingRequest::version() const
{
    return vers;
}

QString QGeocodingRequest::language() const
{
    return languageMARC;
}

void QGeocodingRequest::setLanguage(const QString& language)
{
    if (language.length() != 3)
        return; //ignore

    languageMARC = language;
}

QString QGeocodingRequest::oneBoxLocation() const
{
    return obloc;
}

void QGeocodingRequest::setOneBoxLocation(const QString& obloc)
{
    this->obloc = obloc;
}

QString QGeocodingRequest::country() const
{
    return cntry;
}

void QGeocodingRequest::setCountry(const QString& country)
{
    this->cntry = country;
}

QString QGeocodingRequest::state() const
{
    return st;
}

void QGeocodingRequest::setState(const QString& state)
{
    this->st = state;
}

QString QGeocodingRequest::city() const
{
    return cty;
}

void QGeocodingRequest::setCity(const QString& city)
{
    this->cty = city;
}

QString QGeocodingRequest::postCode() const
{
    return pCode;
}

void QGeocodingRequest::setPostCode(const QString& postCode)
{
    this->pCode = postCode;
}

QString QGeocodingRequest::street() const
{
    return strt;
}

void QGeocodingRequest::setStreet(const QString& street)
{
    this->strt = street;
}

QString QGeocodingRequest::number() const
{
    return num;
}

void QGeocodingRequest::setNumber(const QString& number)
{
    this->num = number;
}

//------------------------------------------------------------------------------
//QReverseGeocodingRequest

QReverseGeocodingRequest::QReverseGeocodingRequest()
{
    languageMARC = "eng";
    vers = "1.0";
}

QReverseGeocodingRequest::QReverseGeocodingRequest(const QGeoCoordinateMaps& coordinate)
        : coord(coordinate)
{
    languageMARC = "eng";
    vers = "1.0";
}

QString QReverseGeocodingRequest::version() const
{
    return vers;
}

void QReverseGeocodingRequest::setLanguage(const QString& language)
{
    languageMARC = language;
}

QString QReverseGeocodingRequest::language() const
{
    return languageMARC;
}

void QReverseGeocodingRequest::setCoordinate(const QGeoCoordinateMaps& coordinate)
{
    coord = coordinate;
}

QGeoCoordinateMaps QReverseGeocodingRequest::coordinate() const
{
    return coord;
}

//------------------------------------------------------------------------------
//QAddress
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

