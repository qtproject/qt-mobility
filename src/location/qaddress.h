/*
* ==============================================================================
*  Name        : QAddress.h
*  Version     : 0.1
*  Author      : Thomas Zahn
*
*  Copyright (c) 2010 Nokia Corporation.
*  This material, including documentation and any related
*  computer programs, is protected by copyright controlled by
*  Nokia Corporation.
* ==============================================================================
*/

#ifndef QLOCATION_ADDRESS_H
#define QLOCATION_ADDRESS_H

#include <QString>

namespace QLocation
{

/*!
* This class represents an address as returned by a
* geocoding reply and as contained in \ref QGeoLocation.
* @see QGeocodingReply
*/
class QAddress
{
    friend class QGeocodingXmlHandler;

public:
    /*!
    * Default constructor.
    */
    QAddress() {}

    /*!
    * @return The country name according to ISO 3166-1 alpha-3
    */
    QString country() const {
        return sCountry;
    }

    /*!
    * @return The country code according to ISO 3166-1 alpha-3
    */
    QString countryCode() const {
        return sCountryCode;
    }

    /*!
    * @return The state.
    */
    QString state() const {
        return sState;
    }

    /*!
    * @return The county.
    */
    QString county() const {
        return sCounty;
    }

    /*!
    * @return The city.
    */
    QString city() const {
        return sCity;
    }

    /*!
    * @return The district.
    */
    QString district() const {
        return sDistrict;
    }

    /*!
    * @return The thoroughfare name.
    */
    QString thoroughfareName() const {
        return sThoroughfareName;
    }

    /*!
    * @return The thoroughfare number.
    */
    QString thoroughfareNumber() const {
        return sThoroughfareNumber;
    }

    /*!
    * @return The post code.
    */
    QString postCode() const {
        return sPostCode;
    }

    /*!
    * Returns whether this address is empty. An address is considered empty
    * if <i>all</i> of its fields are empty.
    * @returns true (empty) / false (not empty).
    */
    bool isEmpty() const;

private:
    QString sCountry; //!< country field
    QString sCountryCode; //!< country code field
    QString sState; //!< state field
    QString sCounty; //!< county field
    QString sCity; //!< city field
    QString sDistrict; //!< district field
    QString sThoroughfareName; //!< thoroughfare name field
    QString sThoroughfareNumber; //!< thoroughfare number field
    QString sPostCode; //!< post code field
};

}

#endif
