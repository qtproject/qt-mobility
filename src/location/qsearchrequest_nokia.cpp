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

#include "qsearchrequest_nokia.h"
#include "qsearchrequest_nokia_p.h"

QTM_BEGIN_NAMESPACE

QSearchRequestNokia::QSearchRequestNokia()
    : QSearchRequest(), d_ptr(new QSearchRequestNokiaPrivate(this))
{
    additionalData.insert("version","1.0");
    additionalData.insert("language","eng");
}

QSearchRequestNokia::~QSearchRequestNokia()
{
    Q_D(QSearchRequestNokia);
    delete d;
}
/*!
    Returns the service version.

    Currently the only supported version is 1.0.
*/
QString QSearchRequestNokia::version() const
{
    return additionalData["version"];
}

/*!
    Returns the MARC language code.
*/
QString QSearchRequestNokia::language() const
{
    return additionalData["language"];
}

/*!
    Sets the language code to \a language.

    This function assumes that \a language is a MARC language code.
*/
void QSearchRequestNokia::setLanguage(const QString& language)
{
    if (language.length() != 3)
        return; //ignore

    additionalData.insert("language",language);
}

/*!
    Returns the OneBox location description.
*/
QString QSearchRequestNokia::oneBoxLocation() const
{
    return additionalData["obloc"];
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
void QSearchRequestNokia::setOneBoxLocation(const QString& obloc)
{
    additionalData.insert("obloc",obloc);
}

/*!
    Returns the name of the country.
*/
QString QSearchRequestNokia::country() const
{
    return additionalData["country"];
}

/*!
    Sets the name of the country to \a country.

    It is required that either this function or setOneBoxLocation() are used.
*/
void QSearchRequestNokia::setCountry(const QString& country)
{
    additionalData.insert("country",country);
}

/*!
    Returns the name of the state.
*/
QString QSearchRequestNokia::state() const
{
    return additionalData["state"];
}

/*!
    Sets the name of the state to \a state.
*/
void QSearchRequestNokia::setState(const QString& state)
{
    additionalData.insert("state",state);
}

/*!
    Returns the name of the city.
*/
QString QSearchRequestNokia::city() const
{
    return additionalData["city"];
}

/*!
    Sets the name of the city to \a city.
*/
void QSearchRequestNokia::setCity(const QString& city)
{
    additionalData.insert("city",city);
}

/*!
    Returns the post code.
*/
QString QSearchRequestNokia::postCode() const
{
    return additionalData["postCode"];
}

/*!
    Sets the post code to \a postCode.
*/
void QSearchRequestNokia::setPostCode(const QString& postCode)
{
    additionalData.insert("postCode",postCode);
}

/*!
    Returns the street name.
*/
QString QSearchRequestNokia::street() const
{
    return additionalData["street"];
}

/*!
    Sets the street name to \a street.
*/
void QSearchRequestNokia::setStreet(const QString& street)
{
    additionalData.insert("street",street);
}

/*!
    Returns the street number.
*/
QString QSearchRequestNokia::number() const
{
    return additionalData["number"];
}

/*!
    Sets the street number to \a number.
*/
void QSearchRequestNokia::setNumber(const QString& number)
{
    additionalData.insert("number",number);
}

/*!
    Sets the geocoordinate for this request to \a coordinate.
*/
void QSearchRequestNokia::setCoordinate(const QGeoCoordinate& coordinate)
{
    Q_D(QSearchRequestNokia);
    d->coord = coordinate;
}

/*!
    Return the geocoordinate for this request.
*/
QGeoCoordinate QSearchRequestNokia::coordinate() const
{
    Q_D(const QSearchRequestNokia);
    return d->coord;
}

QString QSearchRequestNokia::requestGeocodingString(const QString &host) const
{
    Q_D(const QSearchRequestNokia);
    return d->requestGeocodingString(host);
}

QString QSearchRequestNokia::requestReverseGeocodingString(const QString &host) const
{
    Q_D(const QSearchRequestNokia);
    return d->requestReverseGeocodingString(host);
}

QSearchRequestNokiaPrivate::QSearchRequestNokiaPrivate(QSearchRequestNokia* parent)
    : q_ptr(parent)
{
}

QSearchRequestNokiaPrivate::~QSearchRequestNokiaPrivate()
{
}

QString QSearchRequestNokiaPrivate::requestGeocodingString(const QString &host) const
{
    Q_Q(const QSearchRequestNokia);
    QString request = "http://";
    request += host;
    request += "/geocoder/gc/";
    request += q->version();
    request += "?referer=localhost";

    if (q->language() != "") {
        request += "&lg=";
        request += q->language();
    }

    if (q->oneBoxLocation().isEmpty()) {
        request += "&country=";
        request += q->country();

        if (!q->state().isEmpty()) {
            request += "&state=";
            request += q->state();
        }
        if (!q->city().isEmpty()) {
            request += "&city=";
            request += q->city();
        }
        if (!q->postCode().isEmpty()) {
            request += "&zip=";
            request += q->postCode();
        }
        if (!q->street().isEmpty()) {
            request += "&street=";
            request += q->street();
        }
        if (!q->number().isEmpty()) {
            request += "&number=";
            request += q->number();
        }
    } else {
        request += "&obloc=";
        request += q->oneBoxLocation();
    }

    return request;
}
QString QSearchRequestNokiaPrivate::requestReverseGeocodingString(const QString &host) const
{
    Q_Q(const QSearchRequestNokia);
    QString request = "http://";
    request += host;
    request += "/geocoder/rgc/";
    request += q->version();
    request += "?referer=localhost";
    request += "&long=";
    request += trimDouble(coord.longitude());
    request += "&lat=";
    request += trimDouble(coord.latitude());

    if (q->language() != "") {
        request += "&lg=";
        request += q->language();
    }

    return request;
}

QString QSearchRequestNokiaPrivate::trimDouble(qreal degree, int decimalDigits) const
{
    QString sDegree = QString::number(degree, 'g', decimalDigits);

    int index = sDegree.indexOf('.');

    if (index == -1)
        return sDegree;
    else
        return QString::number(degree, 'g', decimalDigits + index);
}

QTM_END_NAMESPACE
