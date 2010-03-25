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

#include "qreversegeocodingrequest.h"
#include "qreversegeocodingrequest_p.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QReverseGeocodingRequest
    \brief The QReverseGeocodingRequest class represents a reverse geocoding request.
    \ingroup location
*/

/*!
    Default constructor.
*/
QReverseGeocodingRequest::QReverseGeocodingRequest()
    : d_ptr(new QReverseGeocodingRequestPrivate()) {}

/*!
    Constructs a request for the geocoordinate \a coordinate.
*/
QReverseGeocodingRequest::QReverseGeocodingRequest(const QGeoCoordinate& coordinate)
    : d_ptr(new QReverseGeocodingRequestPrivate())
{
    setCoordinate(coordinate);
}

/*!
  Destructor.
*/
QReverseGeocodingRequest::~QReverseGeocodingRequest()
{
    Q_D(QReverseGeocodingRequest);
    delete d;
}

/*!
    Returns the service version.
    
    Currently the only supported version is 1.0.
*/
QString QReverseGeocodingRequest::version() const
{
    Q_D(const QReverseGeocodingRequest);
    return d->vers;
}

/*!
    Sets the language code to \a language.

    This function assumes that \a language is a MARC language code.
*/
void QReverseGeocodingRequest::setLanguage(const QString& language)
{
    Q_D(QReverseGeocodingRequest);
    d->languageMARC = language;
}

/*!
    Returns the MARC language code.
*/
QString QReverseGeocodingRequest::language() const
{
    Q_D(const QReverseGeocodingRequest);
    return d->languageMARC;
}

/*!
    Sets the geocoordinate for this request to \a coordinate.
*/
void QReverseGeocodingRequest::setCoordinate(const QGeoCoordinate& coordinate)
{
    Q_D(QReverseGeocodingRequest);
    d->coord = coordinate;
}

/*!
    Return the geocoordinate for this request.
*/
QGeoCoordinate QReverseGeocodingRequest::coordinate() const
{
    Q_D(const QReverseGeocodingRequest);
    return d->coord;
}

/*!
  Returns the request string based on this request and \a host.
*/
QString QReverseGeocodingRequest::requestString(const QString &host) const
{
    Q_D(const QReverseGeocodingRequest);
    return d->requestString(host);
}

/******************************************************************************
  ****************************************************************************/

QReverseGeocodingRequestPrivate::QReverseGeocodingRequestPrivate()
{
    languageMARC = "eng";
    vers = "1.0";
}

QString QReverseGeocodingRequestPrivate::requestString(const QString &host) const
{
    QString request = "http://";
    request += host;
    request += "/geocoder/rgc/";
    request += vers;
    request += "?referer=localhost";
    request += "&long=";
    request += trimDouble(coord.longitude());
    request += "&lat=";
    request += trimDouble(coord.latitude());

    if (languageMARC != "") {
        request += "&lg=";
        request += languageMARC;
    }

    return request;
}

QString QReverseGeocodingRequestPrivate::trimDouble(qreal degree, int decimalDigits) const
{
    QString sDegree = QString::number(degree, 'g', decimalDigits);

    int index = sDegree.indexOf('.');

    if (index == -1)
        return sDegree;
    else
        return QString::number(degree, 'g', decimalDigits + index);
}

QTM_END_NAMESPACE

