/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/


#ifndef QCONTACTADDRESS_H
#define QCONTACTADDRESS_H

#include <QString>

#include "qtcontactsglobal.h"
#include "qcontactdetail.h"
#include "qcontact.h"

/* Leaf class */
class QTCONTACTS_EXPORT QContactAddress : public QContactDetail
{
public:
    Q_DECLARE_CUSTOM_CONTACT_DETAIL(QContactAddress, "StreetAddress")

    Q_DECLARE_CONSTANT_LATIN_STRING(FieldDisplayLabel, "DisplayLabel");
    Q_DECLARE_CONSTANT_LATIN_STRING(FieldStreet, "Street");
    Q_DECLARE_CONSTANT_LATIN_STRING(FieldLocality, "Locality");
    Q_DECLARE_CONSTANT_LATIN_STRING(FieldRegion, "Region");
    Q_DECLARE_CONSTANT_LATIN_STRING(FieldPostcode, "Postcode");
    Q_DECLARE_CONSTANT_LATIN_STRING(FieldCountry, "Country");

    Q_DECLARE_CONSTANT_LATIN_STRING(AttributeSubTypeParcel, "Parcel");
    Q_DECLARE_CONSTANT_LATIN_STRING(AttributeSubTypePostal, "Postal");
    Q_DECLARE_CONSTANT_LATIN_STRING(AttributeSubTypeDomestic, "Domestic");
    Q_DECLARE_CONSTANT_LATIN_STRING(AttributeSubTypeInternational, "International");

    void setDisplayLabel(const QString& displayLabel) {setValue(QLatin1String(FieldDisplayLabel), displayLabel);}
    QString displayLabel() const;
    void setStreet(const QString& street) {setValue(QLatin1String(FieldStreet), street);}
    QString street() const {return value(QLatin1String(FieldStreet));}
    void setLocality(const QString& locality) {setValue(QLatin1String(FieldLocality), locality);}
    QString locality() const {return value(QLatin1String(FieldLocality));}
    void setRegion(const QString& region) {setValue(QLatin1String(FieldRegion), region);}
    QString region() const {return value(QLatin1String(FieldRegion));}
    void setPostcode(const QString& postcode) {setValue(QLatin1String(FieldPostcode), postcode);}
    QString postcode() const {return value(QLatin1String(FieldPostcode));}
    void setCountry(const QString& country) {setValue(QLatin1String(FieldCountry), country);}
    QString country() const {return value(QLatin1String(FieldCountry));}
};

#endif

