/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/


#ifndef QCONTACTADDRESS_H
#define QCONTACTADDRESS_H

#include <QString>

#include "qtcontactsglobal.h"
#include "qcontactdetail.h"
#include "qcontact.h"

QTM_BEGIN_NAMESPACE

/* Leaf class */
class Q_CONTACTS_EXPORT QContactAddress : public QContactDetail
{
public:
#ifdef Q_QDOC
    static const QLatin1Constant DefinitionName;
    static const QLatin1Constant FieldStreet;
    static const QLatin1Constant FieldLocality;
    static const QLatin1Constant FieldRegion;
    static const QLatin1Constant FieldPostcode;
    static const QLatin1Constant FieldCountry;
    static const QLatin1Constant FieldSubTypes;
    static const QLatin1Constant FieldPostOfficeBox;
    static const QLatin1Constant SubTypeParcel;
    static const QLatin1Constant SubTypePostal;
    static const QLatin1Constant SubTypeDomestic;
    static const QLatin1Constant SubTypeInternational;
#else
    Q_DECLARE_CUSTOM_CONTACT_DETAIL(QContactAddress, "Address")
    Q_DECLARE_LATIN1_CONSTANT(FieldStreet, "Street");
    Q_DECLARE_LATIN1_CONSTANT(FieldLocality, "Locality");
    Q_DECLARE_LATIN1_CONSTANT(FieldRegion, "Region");
    Q_DECLARE_LATIN1_CONSTANT(FieldPostcode, "Postcode");
    Q_DECLARE_LATIN1_CONSTANT(FieldCountry, "Country");
    Q_DECLARE_LATIN1_CONSTANT(FieldSubTypes, "SubTypes");
    Q_DECLARE_LATIN1_CONSTANT(FieldPostOfficeBox, "PostOfficeBox");
    Q_DECLARE_LATIN1_CONSTANT(SubTypeParcel, "Parcel");
    Q_DECLARE_LATIN1_CONSTANT(SubTypePostal, "Postal");
    Q_DECLARE_LATIN1_CONSTANT(SubTypeDomestic, "Domestic");
    Q_DECLARE_LATIN1_CONSTANT(SubTypeInternational, "International");
#endif

    void setStreet(const QString& street) {setValue(FieldStreet, street);}
    QString street() const {return value(FieldStreet);}
    void setLocality(const QString& locality) {setValue(FieldLocality, locality);}
    QString locality() const {return value(FieldLocality);}
    void setRegion(const QString& region) {setValue(FieldRegion, region);}
    QString region() const {return value(FieldRegion);}
    void setPostcode(const QString& postcode) {setValue(FieldPostcode, postcode);}
    QString postcode() const {return value(FieldPostcode);}
    void setCountry(const QString& country) {setValue(FieldCountry, country);}
    QString country() const {return value(FieldCountry);}
    void setPostOfficeBox(const QString& postOfficeBox) {setValue(FieldPostOfficeBox, postOfficeBox);}
    QString postOfficeBox() const {return value(FieldPostOfficeBox);}

    void setSubTypes(const QStringList& subTypes) {setValue(FieldSubTypes, subTypes);}
    void setSubTypes(const QString& subType) {setValue(FieldSubTypes, QStringList(subType));}
    QStringList subTypes() const {return value<QStringList>(FieldSubTypes);}
};

QTM_END_NAMESPACE

#endif

