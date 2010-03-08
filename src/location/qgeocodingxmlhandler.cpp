/*
* ==============================================================================
*  Name        : QGeocodingXmlHandler.cpp
*  Version     : 0.1
*  Author      : Thomas Zahn
*
*  Copyright (c) 2010 Nokia Corporation.
*  This material, including documentation and any related
*  computer programs, is protected by copyright controlled by
*  Nokia Corporation.
* ==============================================================================
*/

#include "qgeocodingxmlhandler.h"

#define GEOCODING_CODE_OK     "OK"
#define GEOCODING_CODE_FAILED "FAILED"

namespace QLocation
{

QGeocodingXmlHandler::QGeocodingXmlHandler(QGeocodingReply* codingReply) :
        reply(codingReply)
{
}

bool QGeocodingXmlHandler::startBoundingBox()
{
    if (parseStates.count() == 0)
        return false;
    if (currPlace == NULL)
        return false;
    if (!currPlace->box.isNull())   //box already opened
        return false;
    if (parseStates[0] != Location)
        return false;

    currBox = &currPlace->box;
    return true;
}

bool QGeocodingXmlHandler::startElement(const QString& namespaceURI,
                                        const QString& localName,
                                        const QString& qName,
                                        const QXmlAttributes& atts)
{
    if (QBaseXmlHandler::startElement(namespaceURI, localName, qName, atts))
        return true;

    if (qName == "places") {
        if (parseStates.count() > 0)
            return false;

        parseCodingReply(atts);
        parseStates.push_front(Root);
    } else if (qName == "place") {
        if (parseStates.count() == 0 || parseStates[0] != Root)
            return false;

        parsePlace(atts);
        parseStates.push_front(Place);
    } else if (qName == "location") {
        if (parseStates.count() == 0 || parseStates[0] != Place)
            return false;

        parseStates.push_front(Location);
    } else if (qName == "position") {
        if (parseStates.count() == 0 || parseStates[0] != Location)
            return false;

        parseStates.push_front(Position);
    } else if (qName == "latitude") {
        if (parseStates.count() == 0 || parseStates[0] != Position)
            return false;

        parseStates.push_front(Latitude);
    } else if (qName == "longitude") {
        if (parseStates.count() == 0 || parseStates[0] != Position)
            return false;

        parseStates.push_front(Longitude);
    } else if (qName == "address") {
        if (parseStates.count() == 0 || parseStates[0] != Place)
            return false;
        if (!currPlace->addr.isEmpty())
            return false; //already parsed an address for this place

        parseStates.push_front(Address);
    } else if (qName == "alternatives") {
        if (parseStates.count() == 0 || parseStates[0] != Place)
            return false;

        parseStates.push_front(Alternative);
    }
    //--------------------------------------------------------
    //(alternative) address elements
    else if (qName == "country") {
        if (parseStates.count() == 0)
            return false;

        if (parseStates[0] == Address)
            parseStates.push_front(Address_Country);
        else if (parseStates[0] == Alternative) {
            parseStates.push_front(Alternative_Country);
            QAlternativeAddress::AltValue altValue;
            QString lang = atts.value("language");

            if (!lang.isEmpty())
                altValue.language = lang;

            currPlace->altern.lCountry.append(altValue);
        } else
            return false;
    } else if (qName == "countryCode") {
        if (parseStates.count() == 0)
            return false;

        if (parseStates[0] == Address)
            parseStates.push_front(Address_CountryCode);
        else if (parseStates[0] == Alternative) {
            parseStates.push_front(Alternative_CountryCode);
            QAlternativeAddress::AltValue altValue;
            QString lang = atts.value("language");

            if (!lang.isEmpty())
                altValue.language = lang;

            currPlace->altern.lCountryCode.append(altValue);
        } else
            return false;
    } else if (qName == "state") {
        if (parseStates.count() == 0)
            return false;

        if (parseStates[0] == Address)
            parseStates.push_front(Address_State);
        else if (parseStates[0] == Alternative) {
            parseStates.push_front(Alternative_State);
            QAlternativeAddress::AltValue altValue;
            QString lang = atts.value("language");

            if (!lang.isEmpty())
                altValue.language = lang;

            currPlace->altern.lState.append(altValue);
        } else
            return false;
    } else if (qName == "county") {
        if (parseStates.count() == 0)
            return false;

        if (parseStates[0] == Address)
            parseStates.push_front(Address_County);
        else if (parseStates[0] == Alternative) {
            parseStates.push_front(Alternative_County);
            QAlternativeAddress::AltValue altValue;
            QString lang = atts.value("language");

            if (!lang.isEmpty())
                altValue.language = lang;

            currPlace->altern.lCounty.append(altValue);
        } else
            return false;
    } else if (qName == "city") {
        if (parseStates.count() == 0)
            return false;

        if (parseStates[0] == Address)
            parseStates.push_front(Address_City);
        else if (parseStates[0] == Alternative) {
            parseStates.push_front(Alternative_City);
            QAlternativeAddress::AltValue altValue;
            QString lang = atts.value("language");

            if (!lang.isEmpty())
                altValue.language = lang;

            currPlace->altern.lCity.append(altValue);
        } else
            return false;
    } else if (qName == "district") {
        if (parseStates.count() == 0)
            return false;

        if (parseStates[0] == Address)
            parseStates.push_front(Address_District);
        else if (parseStates[0] == Alternative) {
            parseStates.push_front(Alternative_District);
            QAlternativeAddress::AltValue altValue;
            QString lang = atts.value("language");

            if (!lang.isEmpty())
                altValue.language = lang;

            currPlace->altern.lDistrict.append(altValue);
        } else
            return false;
    } else if (qName == "postCode") {
        if (parseStates.count() == 0)
            return false;

        if (parseStates[0] == Address)
            parseStates.push_front(Address_PostCode);
        else if (parseStates[0] == Alternative) {
            parseStates.push_front(Alternative_PostCode);
            QAlternativeAddress::AltValue altValue;
            QString lang = atts.value("language");

            if (!lang.isEmpty())
                altValue.language = lang;

            currPlace->altern.lPostCode.append(altValue);
        } else
            return false;
    } else if (qName == "thoroughfare") {
        if (parseStates.count() == 0)
            return false;

        if (parseStates[0] == Address)
            parseStates.push_front(Address_Thoroughfare);
        else if (parseStates[0] == Alternative) {
            parseStates.push_front(Alternative_Thoroughfare);
            QAlternativeAddress::AltValue altNameValue;
            QAlternativeAddress::AltValue altNumberValue;

            QString lang = atts.value("language");

            if (!lang.isEmpty()) {
                altNameValue.language = lang;
                altNumberValue.language = lang;
            }
        } else
            return false;
    } else if (qName == "number") {
        if (parseStates.count() == 0)
            return false;

        if (parseStates[0] == Address_Thoroughfare)
            parseStates.push_front(Address_ThoroughfareNumber);
        else if (parseStates[0] == Alternative_Thoroughfare)
            parseStates.push_front(Alternative_ThoroughfareNumber);
        else
            return false;
    } else if (qName == "name") {
        if (parseStates.count() == 0)
            return false;

        if (parseStates[0] == Address_Thoroughfare)
            parseStates.push_front(Address_ThoroughfareName);
        else if (parseStates[0] == Alternative_Thoroughfare)
            parseStates.push_front(Alternative_ThoroughfareName);
        else
            return false;
    }

    return true;
}

void QGeocodingXmlHandler::parseCodingReply(const QXmlAttributes& atts)
{
    QString code = atts.value("resultCode");

    if (!code.isEmpty()) {
        if (code == GEOCODING_CODE_OK)
            reply->code = QGeocodingReply::OK;
        else
            reply->code = QGeocodingReply::Failed;
    }

    QString descr = atts.value("resultDescription");

    if (!descr.isEmpty()) {
        reply->descr = descr;
    }
}

void QGeocodingXmlHandler::parsePlace(const QXmlAttributes& atts)
{
    currPlace = new QGeoLocation();
    QString s = atts.value("title");

    if (!s.isEmpty()) {
        currPlace->ttl = s;
    }

    s = atts.value("language");

    if (!s.isEmpty()) {
        currPlace->lang = s;
    }
}

bool QGeocodingXmlHandler::characters(const QString& ch)
{
    if (parseStates.count() == 0)
        return true;
    if (QBaseXmlHandler::characters(ch))
        return true;

    if (parseStates.count() > 1 &&
            parseStates[0] == Latitude &&
            parseStates[1] == Position) {
        currPlace->pos.setY(ch.toDouble());
    } else if (parseStates.count() > 1 &&
               parseStates[0] == Longitude &&
               parseStates[1] == Position) {
        currPlace->pos.setX(ch.toDouble());
    } else if (parseStates[0] == Address_Country) {
        currPlace->addr.sCountry = ch;
    } else if (parseStates[0] == Address_CountryCode) {
        currPlace->addr.sCountryCode = ch;
    } else if (parseStates[0] == Address_State) {
        currPlace->addr.sState = ch;
    } else if (parseStates[0] == Address_County) {
        currPlace->addr.sCounty = ch;
    } else if (parseStates[0] == Address_City) {
        currPlace->addr.sCity = ch;
    } else if (parseStates[0] == Address_District) {
        currPlace->addr.sDistrict = ch;
    } else if (parseStates[0] == Address_ThoroughfareName) {
        currPlace->addr.sThoroughfareName = ch;
    } else if (parseStates[0] == Address_ThoroughfareNumber) {
        currPlace->addr.sThoroughfareNumber = ch;
    } else if (parseStates[0] == Address_PostCode) {
        currPlace->addr.sPostCode = ch;
    }
    //alternative values
    else if (parseStates[0] == Alternative_Country) {
        if (currPlace->altern.lCountry.count() > 0)
            currPlace->altern.lCountry.last().value = ch;
    } else if (parseStates[0] == Alternative_CountryCode) {
        if (currPlace->altern.lCountryCode.count() > 0)
            currPlace->altern.lCountryCode.last().value = ch;
    } else if (parseStates[0] == Alternative_State) {
        if (currPlace->altern.lState.count() > 0)
            currPlace->altern.lState.last().value = ch;
    } else if (parseStates[0] == Alternative_County) {
        if (currPlace->altern.lCounty.count() > 0)
            currPlace->altern.lCounty.last().value = ch;
    } else if (parseStates[0] == Alternative_City) {
        if (currPlace->altern.lCity.count() > 0)
            currPlace->altern.lCity.last().value = ch;
    } else if (parseStates[0] == Alternative_District) {
        if (currPlace->altern.lDistrict.count() > 0)
            currPlace->altern.lDistrict.last().value = ch;
    } else if (parseStates[0] == Alternative_ThoroughfareName) {
        if (currPlace->altern.lThoroughfareName.count() > 0)
            currPlace->altern.lThoroughfareName.last().value = ch;
    } else if (parseStates[0] == Alternative_ThoroughfareNumber) {
        if (currPlace->altern.lThoroughfareNumber.count() > 0)
            currPlace->altern.lThoroughfareNumber.last().value = ch;
    } else if (parseStates[0] == Alternative_PostCode) {
        if (currPlace->altern.lPostCode.count() > 0)
            currPlace->altern.lPostCode.last().value = ch;
    }

    return true;
}

bool QGeocodingXmlHandler::endElement(const QString& namespaceURI, const QString& localName, const QString& qName)
{
    if (parseStates.count() == 0)
        return false;
    if (QBaseXmlHandler::endElement(namespaceURI, localName, qName))
        return true;

    if (qName == "places") {
        if (parseStates[0] != Root)
            return false;
    } else if (qName == "latitude") {
        if (parseStates[0] != Latitude)
            return false;
    } else if (qName == "longitude") {
        if (parseStates[0] != Longitude)
            return false;
    } else if (qName == "position") {
        if (parseStates[0] != Position)
            return false;
    } else if (qName == "location") {
        if (parseStates[0] != Location)
            return false;
    } else if (qName == "place") {
        if (parseStates[0] != Place)
            return false;

        reply->plcs.append(*currPlace);
        currPlace = NULL;
    } else if (qName == "address") {
        if (parseStates[0] != Address)
            return false;
    } else if (qName == "alternatives") {
        if (parseStates[0] != Alternative)
            return false;
    }
    //--------------------------------------------------------
    //address elements
    else if (qName == "country") {
        if (parseStates[0] != Address_Country &&
                parseStates[0] != Alternative_Country)
            return false;
    } else if (qName == "countryCode") {
        if (parseStates[0] != Address_CountryCode &&
                parseStates[0] != Alternative_CountryCode)
            return false;
    } else if (qName == "state") {
        if (parseStates[0] != Address_State &&
                parseStates[0] != Alternative_State)
            return false;
    } else if (qName == "county") {
        if (parseStates[0] != Address_County &&
                parseStates[0] != Alternative_County)
            return false;
    } else if (qName == "city") {
        if (parseStates[0] != Address_City &&
                parseStates[0] != Alternative_City)
            return false;
    } else if (qName == "district") {
        if (parseStates[0] != Address_District &&
                parseStates[0] != Alternative_District)
            return false;
    } else if (qName == "postCode") {
        if (parseStates[0] != Address_PostCode &&
                parseStates[0] != Alternative_PostCode)
            return false;
    } else if (qName == "thoroughfare") {
        if (parseStates[0] != Address_Thoroughfare &&
                parseStates[0] != Alternative_Thoroughfare)
            return false;
    } else if (qName == "number") {
        if (parseStates[0] != Address_ThoroughfareNumber &&
                parseStates[0] != Alternative_ThoroughfareNumber)
            return false;
    } else if (qName == "name") {
        if (parseStates[0] != Address_ThoroughfareName &&
                parseStates[0] != Alternative_ThoroughfareName)
            return false;
    } else
        return false;

    parseStates.removeFirst();
    return true;
}

}
