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

#ifndef QLOCATION_GEOCODINGREQUEST_H
#define QLOCATION_GEOCODINGREQUEST_H

#include "qmobilityglobal.h"
#include <QString>

QTM_BEGIN_NAMESPACE

/*!
* This class represents a geocoding request.
*/
class Q_LOCATION_EXPORT QGeocodingRequest
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

QTM_END_NAMESPACE

#endif
