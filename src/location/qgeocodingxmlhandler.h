/*
* ==============================================================================
*  Name        : QGeocodingXmlHandler.h
*  Version     : 0.1
*  Author      : Thomas Zahn
*
*  Copyright (c) 2010 Nokia Corporation.
*  This material, including documentation and any related
*  computer programs, is protected by copyright controlled by
*  Nokia Corporation.
* ==============================================================================
*/

#ifndef QLOCATION_GEOCODINGXMLHANDLER_H
#define QLOCATION_GEOCODINGXMLHANDLER_H

#include "qbasexmlhandler.h"
#include "qgeocodingreply.h"

namespace QLocation
{
/*!
* This class parses a returned raw Qt::QNetworkReply to populate a
* corresponding \ref QGeocodingReply.
*/

class QGeocodingXmlHandler : public QBaseXmlHandler
{
public:
    /*!
    * Cosntructor.
    * @param codingReply The QGeocodingReply to populate.
    */
    QGeocodingXmlHandler(QGeocodingReply* codingReply);

    virtual bool startElement(const QString& namespaceURI, const QString& localName, const QString& qName, const QXmlAttributes& atts);
    virtual bool endElement(const QString& namespaceURI, const QString& localName, const QString& qName);
    virtual bool characters(const QString& ch);

private:
    Q_DISABLE_COPY(QGeocodingXmlHandler)

    /*!
    * Parses a geo coding reply.
    * @param atts The XML attributes to parse.
    */
    void parseCodingReply(const QXmlAttributes& atts);
    /*!
    * Parses a geo location.
    * @param atts The XML attributes to parse.
    */
    void parsePlace(const QXmlAttributes& atts);
    virtual bool startBoundingBox();

private:
    //! Internal parsing states.
    enum GeocodingParseState {
        Place = QBaseXmlHandler::Last,
        Location,
        Position,
        Address,
        Address_Country,
        Address_CountryCode,
        Address_State,
        Address_County,
        Address_City,
        Address_District,
        Address_Thoroughfare,
        Address_ThoroughfareName,
        Address_ThoroughfareNumber,
        Address_PostCode,
        Alternative,
        Alternative_Country,
        Alternative_CountryCode,
        Alternative_State,
        Alternative_County,
        Alternative_City,
        Alternative_District,
        Alternative_Thoroughfare,
        Alternative_ThoroughfareName,
        Alternative_ThoroughfareNumber,
        Alternative_PostCode

    };

    QGeocodingReply* reply; //!< Pointer to the QRouteReply object being populated.
    QGeoLocation* currPlace; //!< The current geo location for which XML data is being parsed.
};

}

#endif
