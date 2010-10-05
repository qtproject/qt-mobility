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


#ifndef QDECLARATIVECONTACTADDRESS_H
#define QDECLARATIVECONTACTADDRESS_H

#include "qdeclarativecontactdetail_p.h"
#include "qcontactaddress.h"

class QDeclarativeContactAddress : public QDeclarativeContactDetail
{
    Q_OBJECT

    Q_PROPERTY(QString street READ street WRITE setStreet NOTIFY fieldsChanged)
    Q_PROPERTY(QString locality READ locality WRITE setLocality NOTIFY fieldsChanged)
    Q_PROPERTY(QString region READ region WRITE setRegion  NOTIFY fieldsChanged)
    Q_PROPERTY(QString postcode READ postcode WRITE setPostcode  NOTIFY fieldsChanged)
    Q_PROPERTY(QString country READ country WRITE setCountry  NOTIFY fieldsChanged)
    Q_PROPERTY(QStringList subTypes READ subTypes WRITE setSubTypes NOTIFY fieldsChanged)
    Q_PROPERTY(QString postOfficeBox READ postOfficeBox WRITE setPostOfficeBox  NOTIFY fieldsChanged)
    Q_ENUMS(FieldType);
public:
    enum FieldType {
        Street = 0,
        Locality,
        Region,
        PostCode,
        Country,
        SubTypes,
        PostOfficeBox
    };

    QDeclarativeContactAddress(QObject* parent = 0)
        :QDeclarativeContactDetail(parent)
    {
        setDetail(QContactAddress());
    }

    ContactDetailType detailType() const
    {
        return QDeclarativeContactDetail::Address;
    }

    void setStreet(const QString& street) {detail().setValue(QContactAddress::FieldStreet, street);}
    QString street() const {return detail().value(QContactAddress::FieldStreet);}
    void setLocality(const QString& locality) {detail().setValue(QContactAddress::FieldLocality, locality);}
    QString locality() const {return detail().value(QContactAddress::FieldLocality);}
    void setRegion(const QString& region) {detail().setValue(QContactAddress::FieldRegion, region);}
    QString region() const {return detail().value(QContactAddress::FieldRegion);}
    void setPostcode(const QString& postcode) {detail().setValue(QContactAddress::FieldPostcode, postcode);}
    QString postcode() const {return detail().value(QContactAddress::FieldPostcode);}
    void setCountry(const QString& country) {detail().setValue(QContactAddress::FieldCountry, country);}
    QString country() const {return detail().value(QContactAddress::FieldCountry);}
    void setPostOfficeBox(const QString& postOfficeBox) {detail().setValue(QContactAddress::FieldPostOfficeBox, postOfficeBox);}
    QString postOfficeBox() const {return detail().value(QContactAddress::FieldPostOfficeBox);}

    void setSubTypes(const QStringList& subTypes) {detail().setValue(QContactAddress::FieldSubTypes, subTypes);}
    QStringList subTypes() const {return detail().value<QStringList>(QContactAddress::FieldSubTypes);}
signals:
    void fieldsChanged();
};

QML_DECLARE_TYPE(QDeclarativeContactAddress)

#endif

