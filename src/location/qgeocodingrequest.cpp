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

QTM_BEGIN_NAMESPACE

/*!
    \class QGeocodingRequest
    \brief The QGeocodingRequest class reprents a geocoding request.
    \ingroup location

    This class represents a geocoding request.
*/

/*!
    Default constructor.
*/
QGeocodingRequest::QGeocodingRequest()
{
    vers = "1.0";
    languageMARC = "eng";
}

/*!
    Returns the service version.

    Currently the only supported version is 1.0.
*/
QString QGeocodingRequest::version() const
{
    return vers;
}

/*!
    Returns the MARC language code.
*/
QString QGeocodingRequest::language() const
{
    return languageMARC;
}

/*!
    Sets the language code to \a language.
    
    This function assumes that \a language is a MARC language code.
*/
void QGeocodingRequest::setLanguage(const QString& language)
{
    if (language.length() != 3)
        return; //ignore

    languageMARC = language;
}

/*!
    Returns the OneBox location description.
*/
QString QGeocodingRequest::oneBoxLocation() const
{
    return obloc;
}

/*!
    Sets the OneBox location description to \a obloc, 

    This function assumes that \a obloc is a OneBox location description.
    
    This contains all the location information like country, state, city,
    street in one string without any specific order,
    that will be processed by the OneBox-Search to extract
    the arguments that are required for a geocoding request.

    If OneBox-Search is not desired, you can enter the specific
    search arguments in the specific parameters.
    It is not possible to combine OneBox-Search and
    the specific parameters.
*/
void QGeocodingRequest::setOneBoxLocation(const QString& obloc)
{
    this->obloc = obloc;
}

/*!
    Returns the name of the country.
*/
QString QGeocodingRequest::country() const
{
    return cntry;
}

/*!
    Sets the name of the country to \a country. 
    
    It is required that either this function or setOneBoxLocation() are used.
*/
void QGeocodingRequest::setCountry(const QString& country)
{
    this->cntry = country;
}

/*!
    Returns the name of the state.
*/
QString QGeocodingRequest::state() const
{
    return st;
}

/*!
    Sets the name of the state to \a state.
*/
void QGeocodingRequest::setState(const QString& state)
{
    this->st = state;
}

/*!
    Returns the name of the city.
*/
QString QGeocodingRequest::city() const
{
    return cty;
}

/*!
    Sets the name of the city to \a city.
*/
void QGeocodingRequest::setCity(const QString& city)
{
    this->cty = city;
}

/*!
    Returns the post code.
*/
QString QGeocodingRequest::postCode() const
{
    return pCode;
}

/*!
    Sets the post code to \a postCode.
*/
void QGeocodingRequest::setPostCode(const QString& postCode)
{
    this->pCode = postCode;
}

/*!
    Returns the street name.
*/
QString QGeocodingRequest::street() const
{
    return strt;
}

/*!
    Sets the street name to \a street.
*/
void QGeocodingRequest::setStreet(const QString& street)
{
    this->strt = street;
}

/*!
    Returns the street number.
*/
QString QGeocodingRequest::number() const
{
    return num;
}

/*!
    Sets the street number to \a number.
*/
void QGeocodingRequest::setNumber(const QString& number)
{
    this->num = number;
}

QString QGeocodingRequest::requestString(const QString &host) const
{
    QString request = "http://";
    request += host;
    request += "/geocoder/gc/";
    request += vers;
    request += "?referer=localhost";

    if (languageMARC != "") {
        request += "&lg=";
        request += languageMARC;
    }

    if (obloc.isEmpty()) {
        request += "&country=";
        request += cntry;

        if (!st.isEmpty()) {
            request += "&state=";
            request += st;
        }
        if (!cty.isEmpty()) {
            request += "&city=";
            request += cty;
        }
        if (!pCode.isEmpty()) {
            request += "&zip=";
            request += pCode;
        }
        if (!strt.isEmpty()) {
            request += "&street=";
            request += strt;
        }
        if (!num.isEmpty()) {
            request += "&number=";
            request += num;
        }
    } else {
        request += "&obloc=";
        request += obloc;
    }

    return request;
}

QTM_END_NAMESPACE

