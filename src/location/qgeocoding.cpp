/*
* ==============================================================================
*  Name        : QGeocoding.cpp
*  Version     : 0.1
*  Author      : Thomas Zahn
*
*  Copyright (c) 2010 Nokia Corporation.
*  This material, including documentation and any related
*  computer programs, is protected by copyright controlled by
*  Nokia Corporation.
* ==============================================================================
*/
#include "qgeocodingrequest.h"
#include "qreversegeocodingrequest.h"
#include "qgeocodingreply.h"

namespace QLocation
{

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

}
