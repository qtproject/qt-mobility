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

#ifndef QLOCATION_ADDRESS_H
#define QLOCATION_ADDRESS_H

#include "qmobilityglobal.h"

#include <QString>

QTM_BEGIN_NAMESPACE

/*!
* This class represents an address as returned by a
* geocoding reply and as contained in \ref QGeoLocation.
* @see QGeocodingReply
*/
class Q_LOCATION_EXPORT QAddress
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

QTM_END_NAMESPACE

#endif
