/*
* ==============================================================================
*  Name        : QReverseGeocodingRequest.h
*  Version     : 0.1
*  Author      : Thomas Zahn
*
*  Copyright (c) 2010 Nokia Corporation.
*  This material, including documentation and any related
*  computer programs, is protected by copyright controlled by
*  Nokia Corporation.
* ==============================================================================
*/

#ifndef QLOCATION_REVERSEGEOCODINGREQUEST_H
#define QLOCATION_REVERSEGEOCODINGREQUEST_H

#include <QString>

#include "qgeocoordinatemaps.h"

namespace QLocation
{

class QReverseGeocodingRequest
{
    friend class QGeoNetworkManager;

public:
    /*!
    * Default constructor.
    */
    QReverseGeocodingRequest();
    /*!
    * Constructs a request for the given geo coordinate.
    * @param coordinate The geocoordinate for which geo information
    * is requested.
    */
    QReverseGeocodingRequest(const QGeoCoordinateMaps& coordinate);

    /*!
    * @return The service version; currently the only supported version is 1.0.
    */
    QString version() const;

    /*!
    * @return The MARC language code.
    */
    QString language() const;
    /*!
    * Sets the language code.
    * @param language The MARC language code.
    */
    void setLanguage(const QString& language);

    /*!
    * @return The geo coordinate for which geo information
    * is requested.
    */
    QGeoCoordinateMaps coordinate() const;
    /*!
    * Sets the geo coordinate for which geo information
    * is requested.
    * @param coordinate The geo coordinate.
    */
    void setCoordinate(const QGeoCoordinateMaps& coordinate);

private:
    Q_DISABLE_COPY(QReverseGeocodingRequest)

    QString        vers;
    QString        languageMARC;
    QGeoCoordinateMaps coord;
};

}

#endif
