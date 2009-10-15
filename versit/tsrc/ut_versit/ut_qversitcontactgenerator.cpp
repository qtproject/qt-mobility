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
#include <qcontactguid.h>
#include <qcontactorganization.h>
#include <qcontacttimestamp.h>


void UT_QVersitContactGenerator::init()
{    
   mGenerator = new QVersitContactGenerator();
   QVERIFY(mGenerator);
}

void UT_QVersitContactGenerator::cleanup()
{
   delete mGenerator;
}

void UT_QVersitContactGenerator::testName()
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
    nameProperty.setValue(val.join(QString::fromAscii(";")).toAscii());
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

void UT_QVersitContactGenerator::testAddress()
{
    QContact contact;
    QVersitDocument document;
    QVersitProperty property;
    property.setName(QString::fromAscii(versitAddressId)); 
    
    // Empty value for the address
    document = createDocumentWithProperty(property);
    contact = mGenerator->generateContact(document);
    QContactAddress address = 
        static_cast<QContactAddress>(contact.detail(QContactAddress::DefinitionName));
    QCOMPARE(address.postOfficeBox(),QString());
    QCOMPARE(address.street(),QString());
    QCOMPARE(address.locality(),QString());
    QCOMPARE(address.region(),QString());
    QCOMPARE(address.postcode(),QString());
    QCOMPARE(address.country(),QString());
    
    // Address with just seprators
    property.setValue(QByteArray(";;;;;;"));
    document = createDocumentWithProperty(property);
    contact = mGenerator->generateContact(document);
    address = static_cast<QContactAddress>(contact.detail(QContactAddress::DefinitionName));
    QCOMPARE(address.postOfficeBox(),QString());
    QCOMPARE(address.street(),QString());
    QCOMPARE(address.locality(),QString());
    QCOMPARE(address.region(),QString());
    QCOMPARE(address.postcode(),QString());
    QCOMPARE(address.country(),QString());
    
    // Address with some fields missing
    property.setValue(QByteArray(";;My Street;My Town;;12345;"));
    document = createDocumentWithProperty(property);
    contact = mGenerator->generateContact(document);
    address = static_cast<QContactAddress>(contact.detail(QContactAddress::DefinitionName));
    QCOMPARE(address.postOfficeBox(),QString());
    QCOMPARE(address.street(),QString::fromAscii("My Street"));
    QCOMPARE(address.locality(),QString::fromAscii("My Town"));
    QCOMPARE(address.region(),QString());
    QCOMPARE(address.postcode(),QString::fromAscii("12345"));
    QCOMPARE(address.country(),QString());
    
    // Address with all the fields filled
    property.setValue(QByteArray("PO Box;E;My Street;My Town;My State;12345;My Country"));
    document = createDocumentWithProperty(property);
    contact = mGenerator->generateContact(document);
    address = static_cast<QContactAddress>(contact.detail(QContactAddress::DefinitionName));
    QCOMPARE(address.postOfficeBox(),QString::fromAscii("PO Box"));
    QCOMPARE(address.street(),QString::fromAscii("My Street"));
    QCOMPARE(address.locality(),QString::fromAscii("My Town"));
    QCOMPARE(address.region(),QString::fromAscii("My State"));
    QCOMPARE(address.postcode(),QString::fromAscii("12345"));
    QCOMPARE(address.country(),QString::fromAscii("My Country"));
    
    // Address with TYPE parameters coverted to contexts and subtypes
    property.addParameter(QString::fromAscii(versitType),QString::fromAscii("HOME"));
    property.addParameter(QString::fromAscii(versitType),QString::fromAscii("WORK"));    
    property.addParameter(QString::fromAscii(versitType),QString::fromAscii("DOM"));
    property.addParameter(QString::fromAscii(versitType),QString::fromAscii("INTL"));
    property.addParameter(QString::fromAscii(versitType),QString::fromAscii("POSTAL"));
    property.addParameter(QString::fromAscii(versitType),QString::fromAscii("PARCEL"));
    property.addParameter(QString::fromAscii(versitType),QString::fromAscii("X-EXTENSION"));
    document = createDocumentWithProperty(property);
    contact = mGenerator->generateContact(document);
    address = static_cast<QContactAddress>(contact.detail(QContactAddress::DefinitionName));
    QStringList contexts = address.contexts();
    QVERIFY(contexts.contains(QContactDetail::ContextHome));   
    QVERIFY(contexts.contains(QContactDetail::ContextWork));
    QStringList subTypes = address.subTypes();
    QVERIFY(subTypes.contains(QContactAddress::SubTypeDomestic));
    QVERIFY(subTypes.contains(QContactAddress::SubTypeInternational));
    QVERIFY(subTypes.contains(QContactAddress::SubTypePostal));
    QVERIFY(subTypes.contains(QContactAddress::SubTypeParcel));
}

void UT_QVersitContactGenerator::testOrganization()
{
    QContact contact;
    QVersitDocument document;
    QVersitProperty property;
    property.setName(QString::fromAscii(versitOrganizationId));
    
    // Empty value for the organization
    document = createDocumentWithProperty(property);
    contact = mGenerator->generateContact(document);
    QContactOrganization org = 
        static_cast<QContactOrganization>(contact.detail(QContactOrganization::DefinitionName));
    QCOMPARE(org.name(),QString());
    QCOMPARE(org.department(),QString());

    // Organization with one seprator
    property.setValue(QByteArray(";"));
    document = createDocumentWithProperty(property);
    contact = mGenerator->generateContact(document);
    org = static_cast<QContactOrganization>(contact.detail(QContactOrganization::DefinitionName));
    QCOMPARE(org.name(),QString::fromAscii(""));
    QCOMPARE(org.department(),QString::fromAscii(""));
    
    // Organization with just seprators
    property.setValue(QByteArray(";;;"));
    document = createDocumentWithProperty(property);
    contact = mGenerator->generateContact(document);
    org = static_cast<QContactOrganization>(contact.detail(QContactOrganization::DefinitionName));
    QCOMPARE(org.name(),QString::fromAscii(""));
    QCOMPARE(org.department(),QString::fromAscii(";;"));
    
    // Organization with one Organizational Unit
    property.setValue(QByteArray("Nokia Oyj;R&D"));
    document = createDocumentWithProperty(property);
    contact = mGenerator->generateContact(document);
    org = static_cast<QContactOrganization>(contact.detail(QContactOrganization::DefinitionName));
    QCOMPARE(org.name(),QString::fromAscii("Nokia Oyj"));
    QCOMPARE(org.department(),QString::fromAscii("R&D"));
    
    // Organization with one Organizational Unit
    property.setValue(QByteArray("ABC, Inc.;North American Division;Devices;Marketing"));
    document = createDocumentWithProperty(property);
    contact = mGenerator->generateContact(document);
    org = static_cast<QContactOrganization>(contact.detail(QContactOrganization::DefinitionName));
    QCOMPARE(org.name(),QString::fromAscii("ABC, Inc."));
    QCOMPARE(org.department(),QString::fromAscii("North American Division;Devices;Marketing"));
}

void UT_QVersitContactGenerator::testTel()
{
    QVersitDocument document;
    QVersitProperty property;
    property.setName(QString::fromAscii(versitPhoneId));
    QByteArray value("+35850486321");
    property.setValue(value);
    
    property.addParameter(QString::fromAscii(versitType),QString::fromAscii(versitVoiceId));
    property.addParameter(QString::fromAscii(versitType),QString::fromAscii(versitCellId));
    property.addParameter(QString::fromAscii(versitType),QString::fromAscii(versitModemId));
    property.addParameter(QString::fromAscii(versitType),QString::fromAscii(versitCarId));
    property.addParameter(QString::fromAscii(versitType),QString::fromAscii(versitVideoId));
    property.addParameter(QString::fromAscii(versitType),QString::fromAscii(versitFaxId));
    property.addParameter(QString::fromAscii(versitType),QString::fromAscii(versitBbsId));
    property.addParameter(QString::fromAscii(versitType),QString::fromAscii(versitPagerId));
    property.addParameter(QString::fromAscii(versitType),QString::fromAscii(versitContextHomeId));
    property.addParameter(QString::fromAscii(versitType),QString::fromAscii(versitContextWorkId));

    document.addProperty(property);
    QContact contact = mGenerator->generateContact(document);
    const QContactPhoneNumber& phone = 
        static_cast<QContactPhoneNumber>(
            contact.detail(QContactPhoneNumber::DefinitionName));
    QCOMPARE(phone.number(),QString(value));

    const QStringList subTypes = phone.subTypes();
    QCOMPARE(subTypes.count(),8);
    QVERIFY(subTypes.contains(QContactPhoneNumber::SubTypeVoice));
    QVERIFY(subTypes.contains(QContactPhoneNumber::SubTypeMobile));
    QVERIFY(subTypes.contains(QContactPhoneNumber::SubTypeModem));
    QVERIFY(subTypes.contains(QContactPhoneNumber::SubTypeCar));
    QVERIFY(subTypes.contains(QContactPhoneNumber::SubTypeVideo));
    QVERIFY(subTypes.contains(QContactPhoneNumber::SubTypeFacsimile));
    QVERIFY(subTypes.contains(QContactPhoneNumber::SubTypeBulletinBoardSystem));
    QVERIFY(subTypes.contains(QContactPhoneNumber::SubTypePager));
    
    const QStringList contexts = phone.contexts();
    QCOMPARE(contexts.count(),2);
    QVERIFY(contexts.contains(QContactDetail::ContextWork));
    QVERIFY(contexts.contains(QContactDetail::ContextHome));
}

void UT_QVersitContactGenerator::testEmail()
{
    QVersitProperty property;
    property.setName(QString::fromAscii(versitEmailId));
    QByteArray value("homer.simpson@burns-corporation.com");
    property.setValue(value);
    property.addParameter(QString::fromAscii(versitType),QString::fromAscii(versitContextWorkId));    
    QVersitDocument document = createDocumentWithProperty(property);
    QContact contact = mGenerator->generateContact(document);
    QContactEmailAddress email =
        static_cast<QContactEmailAddress>(
            contact.detail(QContactEmailAddress::DefinitionName));
    QCOMPARE(email.emailAddress(),QString::fromAscii(value));
    const QStringList contexts = email.contexts();
    QCOMPARE(contexts.count(),1);
    QVERIFY(contexts.contains(QContactDetail::ContextWork)); 
}

void UT_QVersitContactGenerator::testUrl()
{
    QVersitProperty property;
    property.setName(QString::fromAscii(versitUrlId));
    QByteArray value("http://www.simpsonsmovie.com/homer.html");
    property.setValue(value);
    property.addParameter(QString::fromAscii(versitType),QString::fromAscii(versitContextWorkId));    
    QVersitDocument document = createDocumentWithProperty(property);    
    QContact contact = mGenerator->generateContact(document);
    QContactUrl url =
        static_cast<QContactUrl>(
            contact.detail(QContactUrl::DefinitionName));
    QCOMPARE(url.url(),QString::fromAscii(value));
    const QStringList contexts = url.contexts();
    QCOMPARE(contexts.count(),1);
    QVERIFY(contexts.contains(QContactDetail::ContextWork));    
}

void UT_QVersitContactGenerator::testUid()
{
    QVersitProperty property;
    property.setName(QString::fromAscii(versitUidId));
    QByteArray value("unique identifier");
    property.setValue(value);
    QVersitDocument document = createDocumentWithProperty(property);
    QContact contact = mGenerator->generateContact(document);
    QContactGuid uid =
        static_cast<QContactGuid>(
            contact.detail(QContactGuid::DefinitionName));
    QCOMPARE(uid.guid(),QString::fromAscii(value));    
}

void UT_QVersitContactGenerator::testTimeStamp()
{
    // Simple date : ISO 8601 extended format
    QVersitProperty property;
    property.setName(QString::fromAscii(versitRevisionId));    
    QByteArray dateValue("1981-05-20");
    property.setValue(dateValue);
    QVersitDocument document = createDocumentWithProperty(property);
    QContact contact = mGenerator->generateContact(document);
    QContactTimestamp timeStamp =
        static_cast<QContactTimestamp>(
            contact.detail(QContactTimestamp::DefinitionName));    
    QCOMPARE(timeStamp.lastModified().date().toString(Qt::ISODate),QString::fromAscii(dateValue));

    // Date and Time : ISO 8601 extended format without utc offset
    QByteArray dateAndTimeValue("1981-05-20T23:55:55");
    property.setValue(dateAndTimeValue);
    document = createDocumentWithProperty(property);
    contact = mGenerator->generateContact(document);
    timeStamp =
        static_cast<QContactTimestamp>(
            contact.detail(QContactTimestamp::DefinitionName));    
    QCOMPARE(timeStamp.lastModified().toString(Qt::ISODate),QString::fromAscii(dateAndTimeValue));    

    // Date and Time with utc offset    
    QByteArray utcOffset = "Z";
    QByteArray dateAndTimeWithUtcValue = dateAndTimeValue+utcOffset;
    property.setValue(dateAndTimeWithUtcValue);
    document = createDocumentWithProperty(property);
    contact = mGenerator->generateContact(document);
    timeStamp =
        static_cast<QContactTimestamp>(
            contact.detail(QContactTimestamp::DefinitionName));
    QCOMPARE(timeStamp.lastModified().toString(Qt::ISODate),QString::fromAscii(dateAndTimeValue));
    QCOMPARE(timeStamp.lastModified().timeSpec(),Qt::UTC);

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

QVersitDocument UT_QVersitContactGenerator::createDocumentWithProperty(
    const QVersitProperty& property)
{
    QVersitDocument document;
    document.addProperty(property);
    return document;
}
