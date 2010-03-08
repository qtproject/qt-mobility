/*
* ==============================================================================
*  Name        : QAlternativeAddress.h
*  Version     : 0.1
*  Author      : Thomas Zahn
*
*  Copyright (c) 2010 Nokia Corporation.
*  This material, including documentation and any related
*  computer programs, is protected by copyright controlled by
*  Nokia Corporation.
* ==============================================================================
*/

#ifndef QLOCATION_ALTERNATIVEADDRESS_H
#define QLOCATION_ALTERNATIVEADDRESS_H

#include "qmobilityglobal.h"

#include <QString>
#include <QList>

QTM_BEGIN_NAMESPACE

/*!
* This class holds alternatives to an address as returned by a
* geocoding reply and as contained in \ref QGeoLocation.
* @see QGeocodingReply
*/
class QAlternativeAddress
{
    friend class QGeocodingXmlHandler;

public:
    /*!
    * This struct holds an alternative value for a \ref QAddress field.
    */
    struct AltValue {
        QString value; //!< The string value of a given alternative.
        QString language; //!< The MARC language code
    };

public:
    /*!
    * Default constructor.
    */
    QAlternativeAddress() {}

    /*!
    * @return A list of alternative country names.
    */
    QList<AltValue> country() const {
        return lCountry;
    }
    /*!
    * @return A list of alternative country codes.
    */
    QList<AltValue> countryCode() const {
        return lCountryCode;
    }
    /*!
    * @return A list of alternative state names.
    */
    QList<AltValue> state() const {
        return lState;
    }
    /*!
    * @return A list of alternative county names.
    */
    QList<AltValue> county() const {
        return lCounty;
    }
    /*!
    * @return A list of alternative city names.
    */
    QList<AltValue> city() const {
        return lCity;
    }
    /*!
    * @return A list of alternative district names.
    */
    QList<AltValue> district() const {
        return lDistrict;
    }
    /*!
    * @return A list of alternative thoroughfare names.
    */
    QList<AltValue> thoroughfareName() const {
        return lThoroughfareName;
    }
    /*!
    * @return A list of alternative thoroughfare numbers.
    */
    QList<AltValue> thoroughfareNumber() const {
        return lThoroughfareNumber;
    }
    /*!
    * @return A list of alternative post codes.
    */
    QList<AltValue> postCode() const {
        return lPostCode;
    }

private:
    QList<AltValue> lCountry;
    QList<AltValue> lCountryCode;
    QList<AltValue> lState;
    QList<AltValue> lCounty;
    QList<AltValue> lCity;
    QList<AltValue> lDistrict;
    QList<AltValue> lThoroughfareName;
    QList<AltValue> lThoroughfareNumber;
    QList<AltValue> lPostCode;
};

QTM_END_NAMESPACE

#endif
