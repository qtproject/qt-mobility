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
** contact Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qversitdefs.h"
#include "ut_qversitcontactgenerator.h"
#include <qversitcontactgenerator.h>
#include <qversitproperty.h>
#include <qversitdocument.h>
#include <QtTest/QtTest>
#include <qcontact.h>
#include <qcontactdetail.h>
#include <qcontactname.h>
#include <qcontactaddress.h>
#include <qcontactphonenumber.h>
#include <qcontactemailaddress.h>
#include <qcontacturl.h>


void UT_QVersitContactGenerator::init()
{    
   mGenerator = new QVersitContactGenerator();
   QVERIFY(mGenerator);
}

void UT_QVersitContactGenerator::cleanup()
{
   delete mGenerator;
}

void UT_QVersitContactGenerator::testCreateName()
{
    QVersitDocument document;
    QVersitProperty nameProperty;
    QStringList val;
    val.append("Simpson");//FirstName
    val.append("Homer");//LastName
    val.append("BellyBoy");//GivenName
    val.append("Dr");//PreFix
    val.append("MSc");//Suffix
    nameProperty.setName(QString::fromAscii(versitNameId));
    nameProperty.setValue(val.join(QString::fromAscii(",")).toAscii());
    document.addProperty(nameProperty);        
    QContact contact = mGenerator->generateContact(document);    
    QCOMPARE(contact.details().count(),2);
    const QContactName& name = (QContactName)contact.detail(QContactName::DefinitionName);    
    QCOMPARE(name.last(),val[0]);
    QCOMPARE(name.first(),val[1]);
    QCOMPARE(name.middle(),val[2]);
    QCOMPARE(name.prefix(),val[3]);
    QCOMPARE(name.suffix(),val[4]);
}

void UT_QVersitContactGenerator::testCreateAddress()
{
    QVersitProperty property;
    property.setName(QString::fromAscii(versitNameId));   
    
    // Empty value for the address
    QContactAddress* address = 
        static_cast<QContactAddress*>(mGenerator->createAddress(property));
    QVERIFY(address != 0);
    QCOMPARE(address->postOfficeBox(),QString());
    QCOMPARE(address->street(),QString());
    QCOMPARE(address->locality(),QString());
    QCOMPARE(address->region(),QString());
    QCOMPARE(address->postcode(),QString());
    QCOMPARE(address->country(),QString());
    delete address;
    address = 0;
    
    // Address with just seprators
    property.setValue(QByteArray(";;;;;;"));
    address = static_cast<QContactAddress*>(mGenerator->createAddress(property));
    QVERIFY(address != 0);
    QCOMPARE(address->postOfficeBox(),QString());
    QCOMPARE(address->street(),QString());
    QCOMPARE(address->locality(),QString());
    QCOMPARE(address->region(),QString());
    QCOMPARE(address->postcode(),QString());
    QCOMPARE(address->country(),QString());
    delete address;
    address = 0;     
    
    // Address with some fields missing
    property.setValue(QByteArray(";;My Street;My Town;;12345;"));
    address = static_cast<QContactAddress*>(mGenerator->createAddress(property));
    QVERIFY(address != 0);
    QCOMPARE(address->postOfficeBox(),QString());
    QCOMPARE(address->street(),QString::fromAscii("My Street"));
    QCOMPARE(address->locality(),QString::fromAscii("My Town"));
    QCOMPARE(address->region(),QString());
    QCOMPARE(address->postcode(),QString::fromAscii("12345"));
    QCOMPARE(address->country(),QString());
    delete address;
    address = 0;    
    
    // Address with all the fields filled
    property.setValue(QByteArray("PO Box;E;My Street;My Town;My State;12345;My Country"));
    address = static_cast<QContactAddress*>(mGenerator->createAddress(property));
    QVERIFY(address != 0);
    QCOMPARE(address->postOfficeBox(),QString::fromAscii("PO Box"));
    QCOMPARE(address->street(),QString::fromAscii("My Street"));
    QCOMPARE(address->locality(),QString::fromAscii("My Town"));
    QCOMPARE(address->region(),QString::fromAscii("My State"));
    QCOMPARE(address->postcode(),QString::fromAscii("12345"));
    QCOMPARE(address->country(),QString::fromAscii("My Country"));
    delete address;
    address = 0;    
    
    // Address with TYPE parameters coverted to contexts and subtypes
    property.addParameter(QString::fromAscii(versitType),QString::fromAscii("HOME"));
    property.addParameter(QString::fromAscii(versitType),QString::fromAscii("WORK"));    
    property.addParameter(QString::fromAscii(versitType),QString::fromAscii("DOM"));
    property.addParameter(QString::fromAscii(versitType),QString::fromAscii("INTL"));
    property.addParameter(QString::fromAscii(versitType),QString::fromAscii("POSTAL"));
    property.addParameter(QString::fromAscii(versitType),QString::fromAscii("PARCEL"));
    property.addParameter(QString::fromAscii(versitType),QString::fromAscii("X-EXTENSION")); 
    address = static_cast<QContactAddress*>(mGenerator->createAddress(property));
    QVERIFY(address != 0);
    QStringList contexts = address->contexts();
    QVERIFY(contexts.contains(QContactDetail::ContextHome));   
    QVERIFY(contexts.contains(QContactDetail::ContextWork));
    QStringList subTypes = address->subTypes();
    QVERIFY(subTypes.contains(QContactAddress::SubTypeDomestic));
    QVERIFY(subTypes.contains(QContactAddress::SubTypeInternational));
    QVERIFY(subTypes.contains(QContactAddress::SubTypePostal));
    QVERIFY(subTypes.contains(QContactAddress::SubTypeParcel));
    delete address;
    address = 0;    
}

void UT_QVersitContactGenerator::testCreateTel()
{
    QVersitDocument document;
    QVersitProperty nameProperty;
    nameProperty.setName(QString::fromAscii(versitPhoneId));
    QByteArray value("+35850486321");
    nameProperty.setValue(value);
    
    nameProperty.addParameter(QString::fromAscii(versitType),QString::fromAscii(versitVoiceId));
    nameProperty.addParameter(QString::fromAscii(versitType),QString::fromAscii(versitCellId));
    nameProperty.addParameter(QString::fromAscii(versitType),QString::fromAscii(versitModemId));
    nameProperty.addParameter(QString::fromAscii(versitType),QString::fromAscii(versitCarId));
    nameProperty.addParameter(QString::fromAscii(versitType),QString::fromAscii(versitVideoId));
    nameProperty.addParameter(QString::fromAscii(versitType),QString::fromAscii(versitContextHomeId));
    nameProperty.addParameter(QString::fromAscii(versitType),QString::fromAscii(versitContextWorkId));

    document.addProperty(nameProperty);
    QContact contact = mGenerator->generateContact(document);
    const QContactPhoneNumber& phone = 
        static_cast<QContactPhoneNumber>(
            contact.detail(QContactPhoneNumber::DefinitionName));
    QCOMPARE(phone.number(),QString(value));

    const QStringList subTypes = phone.subTypes();
    QCOMPARE(subTypes.count(),5);
    QCOMPARE(subTypes[4],QContactPhoneNumber::SubTypeVoice.operator QString());
    QCOMPARE(subTypes[3],QContactPhoneNumber::SubTypeMobile.operator QString());
    QCOMPARE(subTypes[2],QContactPhoneNumber::SubTypeModem.operator QString());
    QCOMPARE(subTypes[1],QContactPhoneNumber::SubTypeCar.operator QString());
    QCOMPARE(subTypes[0],QContactPhoneNumber::SubTypeVideo.operator QString());

    const QStringList contexts = phone.contexts();
    QCOMPARE(contexts.count(),2);
    QCOMPARE(contexts[0],QContactDetail::ContextWork.operator QString());
    QCOMPARE(contexts[1],QContactDetail::ContextHome.operator QString());
}

void UT_QVersitContactGenerator::testCreateEmail()
{
    QVersitDocument document;
    QVersitProperty nameProperty;
    nameProperty.setName(QString::fromAscii(versitEmailId));
    QByteArray value("homer.simpson@springfield.com");
    nameProperty.setValue(value);
    document.addProperty(nameProperty);
    QContact contact = mGenerator->generateContact(document);
    const QContactEmailAddress& email =
        static_cast<QContactEmailAddress>(
            contact.detail(QContactEmailAddress::DefinitionName));
    QCOMPARE(email.emailAddress(),QString(value));
}

void UT_QVersitContactGenerator::testCreateUrl()
{
    QVersitDocument document;
    QVersitProperty nameProperty;
    nameProperty.setName(QString::fromAscii(versitUrlId));
    QByteArray value("http://www.simpsonsmovie.com/homer.html");
    nameProperty.setValue(value);
    document.addProperty(nameProperty);
    QContact contact = mGenerator->generateContact(document);
    const QContactUrl& url =
        static_cast<QContactUrl>(
            contact.detail(QContactUrl::DefinitionName));
    QCOMPARE(url.url(),QString(value));
}

void UT_QVersitContactGenerator::testExtractContexts()
{
    // Empty list
    QStringList result = mGenerator->extractContexts(QStringList());
    QVERIFY(result.isEmpty());
    
    // HOME
    QStringList types(QString::fromAscii("HOME"));
    result = mGenerator->extractContexts(types);
    QCOMPARE(result.count(),1);
    QVERIFY(result.contains(QContactDetail::ContextHome));
    
    // WORK
    types.clear();
    types.append(QString::fromAscii("WORK"));
    result = mGenerator->extractContexts(types);
    QCOMPARE(result.count(),1);
    QVERIFY(result.contains(QContactDetail::ContextWork));    
    
    // WORK and HOME
    types.append(QString::fromAscii("HOME"));
    result = mGenerator->extractContexts(types);
    QCOMPARE(result.count(),2);
    QVERIFY(result.contains(QContactDetail::ContextWork)); 
    QVERIFY(result.contains(QContactDetail::ContextHome));
    
    // HOME and WORK and other items
    types.prepend(QString::fromAscii("OTHER TYPE 1"));
    types.insert(2,QString::fromAscii("OTHER TYPE 2"));
    types.append(QString::fromAscii("OTHER TYPE 3"));
    result = mGenerator->extractContexts(types);
    QCOMPARE(result.count(),2);
    QVERIFY(result.contains(QContactDetail::ContextWork)); 
    QVERIFY(result.contains(QContactDetail::ContextHome));
}
