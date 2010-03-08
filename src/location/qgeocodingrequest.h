/*
* ==============================================================================
*  Name        : QGeocodingRequest.h
*  Version     : 0.1
*  Author      : Thomas Zahn
*
*  Copyright (c) 2010 Nokia Corporation.
*  This material, including documentation and any related
*  computer programs, is protected by copyright controlled by
*  Nokia Corporation.
* ==============================================================================
*/

#ifndef QLOCATION_GEOCODINGREQUEST_H
#define QLOCATION_GEOCODINGREQUEST_H

#include <QString>

namespace QLocation
{

/*!
* This class represents a geocoding request.
*/
class QGeocodingRequest
{
    friend class QGeoNetworkManager;

public:
    /*!
    * Default constructor.
    */
    QGeocodingRequest();

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
    * @return The OneBox location description.
    */
    QString oneBoxLocation() const;
    /*!
    * Sets the OneBox location description. This contains
    * all the location information like country, state, city,
    * street in one string without any specific order,
    * that will be processed by the OneBox-Search to extract
    * the arguments that are required for a geocoding request.
    * If OneBox-Search is not desired, you can enter the specific
    * search arguments in the specific parameters.
    * It is not possible to combine OneBox-Search and
    * the specific parameters.
    * @param obloc The OneBox location description.
    */
    void setOneBoxLocation(const QString& obloc);

    /*!
    * @return The country.
    */
    QString country() const;
    /*!
    * Sets the country. If no \oneBoxLocation was assigned,
    * this field is required.
    * @param county The country name.
    */
    void setCountry(const QString& country);

    /*!
    * @return The state name.
    */
    QString state() const;
    void setState(const QString& state);

    /*!
    * @return The city name.
    */
    QString city() const;
    /*!
    * Sets the city name.
    * @param city The city name.
    */
    void setCity(const QString& city);

    /*!
    * @return The post code.
    */
    QString postCode() const;
    /*!
    * Sets the post code.
    * @param postCode The post code.
    */
    void setPostCode(const QString& postCode);

    /*!
    * @return Street name.
    */
    QString street() const;
    /*!
    * Sets the street name.
    * @param street The street name.
    */
    void setStreet(const QString& street);

    /*!
    * @return Street number.
    */
    QString number() const;
    /*!
    * Sets the street number.
    * @param number The street number.
    */
    void setNumber(const QString& number);

private:
    Q_DISABLE_COPY(QGeocodingRequest)

private:
    QString vers;
    QString languageMARC;

    QString obloc;
    QString cntry;
    QString st;
    QString cty;
    QString pCode;
    QString strt;
    QString num;
};

}

#endif
