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

#ifndef QLOCATIONFILTER_H_
#define QLOCATIONFILTER_H_

#include <QString>
#include "qmobilityglobal.h"
#include "qgeocoordinate.h"

QTM_BEGIN_NAMESPACE

class QLocationFilterPrivate;
class Q_LOCATION_EXPORT QLocationFilter
{
public:
    QLocationFilter();
    virtual ~QLocationFilter();

    /*!
        Returns the LRO Id.
    */
    QString LROId() const;
    
    /*!
        Sets the LRO Id.
    */
    void setLROId(const QString& country);
    
    /*!
        Returns the name of the country.
    */
    QString country() const;
    
    /*!
        Sets the name of the country.
    */
    void setCountry(const QString& country);
    
    /*!
        Returns the name of the county.
    */
    QString county() const;
    
    /*!
        Sets the name of the county.
    */
    void setCounty(const QString& country);

    /*!
        Returns the name of the state.
    */
    QString state() const;
    
    /*!
        Sets the name of the state.
    */
    void setState(const QString& state);

    /*!
        Returns the name of the city.
    */
    QString city() const;
    
    /*!
        Sets the name of the city.
    */
    void setCity(const QString& city);

    /*!
        Returns the post code.
    */
    QString postalCode() const;
    
    /*!
        Sets the post code.
    */
    void setPostalCode(const QString& postalCode);

    /*!
        Returns the district.
    */
    QString district() const;
    /*!
        Sets the name of the district.
    */
    void setDistrict(const QString& country);
    
    /*!
        Returns the street name.
    */
    QString street() const;
    
    /*!
        Sets the street name.
    */
    void setStreet(const QString& street);

    /*!
        Returns the house number.
    */
    QString houseNumber() const;
    
    /*!
        Sets the house number
    */
    void setHouseNumber(const QString& number);
    
    /*!
        Returns the geo coodinate.
    */
    QGeoCoordinate coordinate() const;
    
    /*!
        Sets the geo coodinate
    */
    void setCoordinate(const QGeoCoordinate& coord);
    
private:
    QLocationFilterPrivate *d_ptr;
    Q_DECLARE_PRIVATE(QLocationFilter);
};

QTM_END_NAMESPACE

#endif /* QLOCATIONFILTER_H_ */
