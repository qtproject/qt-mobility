/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qversitdefs.h"
#include "ut_qversitcontactimporter.h"
#include "qversitcontactimporter.h"
#include "qversitcontactimporter_p.h"
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
#include <qcontactanniversary.h>
#include <qcontactbirthday.h>
#include <qcontactgender.h>
#include <qcontactnickname.h>
#include <qcontactavatar.h>
#include <qcontactgeolocation.h>
#include <qcontactnote.h>
#include <qcontactonlineaccount.h>
#include <qcontactfamily.h>
#include <QDir>




QString imageAndAudioClipPath(QString::fromAscii("random98354_dir76583_ut_versit_photo"));

void UT_QVersitContactImporter::initTestCase()
{
    // Create the directory to store the image
    QDir dir;
    if (!dir.exists(imageAndAudioClipPath)) {
        dir.mkdir(imageAndAudioClipPath);
    }
}

void UT_QVersitContactImporter::cleanupTestCase()
{
    QDir dir;

    if (dir.exists(imageAndAudioClipPath)) {
        dir.cd(imageAndAudioClipPath);
        // remove all the files first
        QStringList allFiles;
        allFiles << QString::fromAscii("*");
        QStringList fileList = dir.entryList(allFiles, QDir::Files);
        foreach (QString file, fileList) {
            dir.remove(file);
        }
        dir.cdUp();
        dir.rmdir(imageAndAudioClipPath);
    }
}

void UT_QVersitContactImporter::init()
{    
    mImporter = new QVersitContactImporter();
    mImporterPrivate = new QVersitContactImporterPrivate();
}

void UT_QVersitContactImporter::cleanup()
{
    delete mImporterPrivate;
    delete mImporter;
}

void UT_QVersitContactImporter::testName()
{
    QVersitDocument document;
    QVersitProperty nameProperty;
    QStringList value;
    value.append(QString::fromAscii("Homer"));//FirstName
    value.append(QString::fromAscii("Simpson"));//LastName
    value.append(QString::fromAscii("BellyBoy"));//GivenName
    value.append(QString::fromAscii("Dr"));//PreFix
    value.append(QString::fromAscii("MSc"));//Suffix
    nameProperty.setName(QString::fromAscii("N"));
    nameProperty.setValue(value.join(QString::fromAscii(";")).toAscii());
    document.addProperty(nameProperty);        
    QContact contact = mImporter->importContact(document);
    QContactName name = (QContactName)contact.detail(QContactName::DefinitionName);
    QCOMPARE(name.last(),value[0]);
    QCOMPARE(name.first(),value[1]);
    QCOMPARE(name.middle(),value[2]);
    QCOMPARE(name.prefix(),value[3]);
    QCOMPARE(name.suffix(),value[4]);

    // Multiple names, first one will be picked and rest will be discarded
    nameProperty = QVersitProperty();
    QStringList anotherValue;
    anotherValue.append(QString::fromAscii("FakeHomer"));//FirstName
    anotherValue.append(QString::fromAscii("FakeSimpson"));//LastName
    anotherValue.append(QString::fromAscii("FakeBellyBoy"));//GivenName
    anotherValue.append(QString::fromAscii("FakeDr"));//PreFix
    anotherValue.append(QString::fromAscii("FakeMSc"));//Suffix
    nameProperty.setName(QString::fromAscii("N"));
    nameProperty.setValue(anotherValue.join(QString::fromAscii(";")).toAscii());
    document.addProperty(nameProperty);
    contact = mImporter->importContact(document);
    QList<QContactDetail> names = contact.details(QContactName::DefinitionName);
    QCOMPARE(names.count(),1);
    // anotherValue should be discarded, so check for value
    name = (QContactName)names[0];
    QCOMPARE(name.last(),value[0]);
    QCOMPARE(name.first(),value[1]);
    QCOMPARE(name.middle(),value[2]);
    QCOMPARE(name.prefix(),value[3]);
    QCOMPARE(name.suffix(),value[4]);
}

void UT_QVersitContactImporter::testAddress()
{
    QContact contact;
    QVersitDocument document;
    QVersitProperty property;
    property.setName(QString::fromAscii("ADR"));
    
    // Empty value for the address
    document = createDocumentWithProperty(property);
    contact = mImporter->importContact(document);
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
    contact = mImporter->importContact(document);
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
    contact = mImporter->importContact(document);
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
    contact = mImporter->importContact(document);
    address = static_cast<QContactAddress>(contact.detail(QContactAddress::DefinitionName));
    QCOMPARE(address.postOfficeBox(),QString::fromAscii("PO Box"));
    QCOMPARE(address.street(),QString::fromAscii("My Street"));
    QCOMPARE(address.locality(),QString::fromAscii("My Town"));
    QCOMPARE(address.region(),QString::fromAscii("My State"));
    QCOMPARE(address.postcode(),QString::fromAscii("12345"));
    QCOMPARE(address.country(),QString::fromAscii("My Country"));
    
    // Address with TYPE parameters converted to contexts and subtypes
    property.addParameter(QString::fromAscii("TYPE"),QString::fromAscii("HOME"));
    property.addParameter(QString::fromAscii("TYPE"),QString::fromAscii("WORK"));
    property.addParameter(QString::fromAscii("TYPE"),QString::fromAscii("DOM"));
    property.addParameter(QString::fromAscii("TYPE"),QString::fromAscii("INTL"));
    property.addParameter(QString::fromAscii("TYPE"),QString::fromAscii("POSTAL"));
    property.addParameter(QString::fromAscii("TYPE"),QString::fromAscii("PARCEL"));
    property.addParameter(QString::fromAscii("TYPE"),QString::fromAscii("X-EXTENSION"));
    document = createDocumentWithProperty(property);
    contact = mImporter->importContact(document);
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

void UT_QVersitContactImporter::testOrganizationName()
{
    QContact contact;
    QVersitDocument document;
    QVersitProperty property;

    // Empty value for the organization
    property.setName(QString::fromAscii("ORG"));
    document = createDocumentWithProperty(property);
    contact = mImporter->importContact(document);
    QContactOrganization organization =
        static_cast<QContactOrganization>(
            contact.detail(QContactOrganization::DefinitionName));
    QCOMPARE(organization.name(),QString());
    QCOMPARE(organization.department().count(),0);

    // Organization without separators
    property.setValue(QByteArray("Nokia"));
    document = createDocumentWithProperty(property);
    contact = mImporter->importContact(document);
    organization =
        static_cast<QContactOrganization>(
            contact.detail(QContactOrganization::DefinitionName));
    QCOMPARE(organization.name(),QString::fromAscii("Nokia"));
    QCOMPARE(organization.department().count(),0);

    // Organization with one separator
    property.setValue(QByteArray(";"));
    document = createDocumentWithProperty(property);
    contact = mImporter->importContact(document);
    organization =
        static_cast<QContactOrganization>(
            contact.detail(QContactOrganization::DefinitionName));
    QCOMPARE(organization.name(),QString::fromAscii(""));
    QCOMPARE(organization.department().count(),0);

    // Organization with just separators
    property.setValue(QByteArray(";;;"));
    document = createDocumentWithProperty(property);
    contact = mImporter->importContact(document);
    organization =
        static_cast<QContactOrganization>(
            contact.detail(QContactOrganization::DefinitionName));
    QCOMPARE(organization.name(),QString::fromAscii(""));
    QCOMPARE(organization.department().count(),0);

    // Organization with one Organizational Unit
    property.setValue(QByteArray("Nokia;R&D"));
    document = createDocumentWithProperty(property);
    contact = mImporter->importContact(document);
    organization =
        static_cast<QContactOrganization>(
            contact.detail(QContactOrganization::DefinitionName));
    QCOMPARE(organization.name(),QString::fromAscii("Nokia"));
    QCOMPARE(organization.department().count(),1);
    QCOMPARE(organization.department().at(0),QString::fromAscii("R&D"));

    // Organization with organization name and semicolon
    property.setValue(QByteArray("Nokia;"));
    document = createDocumentWithProperty(property);
    contact = mImporter->importContact(document);
    organization =
        static_cast<QContactOrganization>(
            contact.detail(QContactOrganization::DefinitionName));
    QCOMPARE(organization.name(),QString::fromAscii("Nokia"));
    QCOMPARE(organization.department().count(),0);

    // Organization with semicolon and department
    property.setValue(QByteArray(";R&D"));
    document = createDocumentWithProperty(property);
    contact = mImporter->importContact(document);
    organization =
        static_cast<QContactOrganization>(
            contact.detail(QContactOrganization::DefinitionName));
    QCOMPARE(organization.name(),QString());
    QCOMPARE(organization.department().count(),1);
    QCOMPARE(organization.department().at(0),QString::fromAscii("R&D"));

    // Organization with more Organizational Units
    property.setValue(QByteArray("Nokia;R&D;Devices;Qt"));
    document = createDocumentWithProperty(property);
    contact = mImporter->importContact(document);
    organization =
        static_cast<QContactOrganization>(
            contact.detail(QContactOrganization::DefinitionName));
    QCOMPARE(organization.name(),QString::fromAscii("Nokia"));
    QCOMPARE(organization.department().count(),3);
    QCOMPARE(organization.department().at(0),QString::fromAscii("R&D"));
    QCOMPARE(organization.department().at(1),QString::fromAscii("Devices"));
    QCOMPARE(organization.department().at(2),QString::fromAscii("Qt"));
}

void UT_QVersitContactImporter::testOrganizationTitle()
{
    QContact contact;
    QVersitDocument document;
    QVersitProperty property;

    // One title
    property.setName(QString::fromAscii("TITLE"));
    QByteArray titleValue("Developer");
    property.setValue(titleValue);
    document = createDocumentWithProperty(property);
    contact = mImporter->importContact(document);
    QList<QContactDetail> organizationDetails =
        contact.details(QContactOrganization::DefinitionName);
    QCOMPARE(organizationDetails.count(), 1);
    QContactOrganization organization =
        static_cast<QContactOrganization>(organizationDetails[0]);
    QCOMPARE(organization.title(),QString::fromAscii(titleValue));

    // Two titles -> two QContactOrganizations created
    property.setName(QString::fromAscii("TITLE"));
    QByteArray secondTitleValue("Hacker");
    property.setValue(secondTitleValue);
    document.addProperty(property);
    contact = mImporter->importContact(document);
    organizationDetails = contact.details(QContactOrganization::DefinitionName);
    QCOMPARE(organizationDetails.count(), 2);
    QContactOrganization firstOrganization =
        static_cast<QContactOrganization>(organizationDetails[0]);
    QCOMPARE(firstOrganization.title(),QString::fromAscii(titleValue));
    QContactOrganization secondOrganization =
        static_cast<QContactOrganization>(organizationDetails[1]);
    QCOMPARE(secondOrganization.title(),QString::fromAscii(secondTitleValue));

    // Two titles and one organization name -> two QContactOrganizations created
    property.setName(QString::fromAscii("ORG"));
    QByteArray organizationName("Nokia");
    property.setValue(organizationName);
    document.addProperty(property);
    contact = mImporter->importContact(document);
    organizationDetails = contact.details(QContactOrganization::DefinitionName);
    QCOMPARE(organizationDetails.count(), 2);
    firstOrganization = static_cast<QContactOrganization>(organizationDetails[0]);
    QCOMPARE(firstOrganization.title(),QString::fromAscii(titleValue));
    QCOMPARE(firstOrganization.name(),QString::fromAscii(organizationName));
    secondOrganization = static_cast<QContactOrganization>(organizationDetails[1]);
    QCOMPARE(secondOrganization.title(),QString::fromAscii(secondTitleValue));
    QCOMPARE(secondOrganization.name(),QString());
}

void UT_QVersitContactImporter::testOrganizationLogo()
{
    QContact contact;
    QVersitDocument document;
    QVersitProperty property;
    property.setName(QString::fromAscii("X-ASSISTANT"));
    QByteArray assistantValue("Marge");
    property.setValue(assistantValue);
    document = createDocumentWithProperty(property);
    contact = mImporter->importContact(document);
    QContactOrganization organization =
        static_cast<QContactOrganization>(
            contact.detail(QContactOrganization::DefinitionName));
    QCOMPARE(organization.assistantName(), QString::fromAscii(assistantValue));
}

void UT_QVersitContactImporter::testOrganizationAssistant()
{
    QContact contact;
    QVersitDocument document;
    QVersitProperty property;

    // Embedded LOGO
    property.setName(QString::fromAscii("LOGO"));
    QByteArray logo = "R0lGODlhEgASAIAAAAAAAP///yH5BAEAAAEALAAAAAASABIAAAIdjI+py+0G";
    property.setValue(logo.toBase64());
    property.addParameter(QString::fromAscii("TYPE"),
                          QString::fromAscii("GIF"));
    property.addParameter(QString::fromAscii("ENCODING"),
                          QString::fromAscii("BASE64"));
    document = createDocumentWithProperty(property);
    mImporter->setImagePath(imageAndAudioClipPath);
    contact = mImporter->importContact(document);
    QContactOrganization organization =
        static_cast<QContactOrganization>(
            contact.detail(QContactOrganization::DefinitionName));
    QString logoFileName = organization.logo();
    QVERIFY(logoFileName.endsWith(QString::fromAscii(".gif")));
    QVERIFY(logoFileName.contains(imageAndAudioClipPath + QString::fromAscii("/")));
    QFile logoFile(logoFileName);
    QVERIFY(logoFile.open(QIODevice::ReadOnly));
    QByteArray content = logoFile.readAll();
    QCOMPARE(content,logo);
    logoFile.close();

    // LOGO as a URL
    property.setName(QString::fromAscii("LOGO"));
    QByteArray logoUrl = "http://www.organization.org/logo.gif";
    property.setValue(logoUrl);
    property.addParameter(QString::fromAscii("VALUE"),QString::fromAscii("URL"));
    document = createDocumentWithProperty(property);
    contact = mImporter->importContact(document);
    organization =
        static_cast<QContactOrganization>(
            contact.detail(QContactOrganization::DefinitionName));
    QCOMPARE(organization.logo(),QString::fromAscii(logoUrl));
}

void UT_QVersitContactImporter::testOrganizationRole()
{
    QContact contact;
    QVersitDocument document;
    QVersitProperty property;

    // Setting the role is not yet supported by QContactOrganization
    property.setName(QString::fromAscii("ROLE"));
    QByteArray roleValue("Very important manager and proud of it");
    property.setValue(roleValue);
    document = createDocumentWithProperty(property);
    contact = mImporter->importContact(document);
    QContactOrganization organization =
        static_cast<QContactOrganization>(
            contact.detail(QContactOrganization::DefinitionName));
    QVERIFY(organization.isEmpty());
}

void UT_QVersitContactImporter::testTel()
{
    QVersitDocument document;
    QVersitProperty property;
    property.setName(QString::fromAscii("TEL"));
    QByteArray value("+35850987654321");
    property.setValue(value);   

    property.addParameter(QString::fromAscii("TYPE"),QString::fromAscii("VOICE"));
    property.addParameter(QString::fromAscii("TYPE"),QString::fromAscii("CELL"));
    property.addParameter(QString::fromAscii("TYPE"),QString::fromAscii("MODEM"));
    property.addParameter(QString::fromAscii("TYPE"),QString::fromAscii("CAR"));
    property.addParameter(QString::fromAscii("TYPE"),QString::fromAscii("VIDEO"));
    property.addParameter(QString::fromAscii("TYPE"),QString::fromAscii("FAX"));
    property.addParameter(QString::fromAscii("TYPE"),QString::fromAscii("BBS"));
    property.addParameter(QString::fromAscii("TYPE"),QString::fromAscii("PAGER"));
    property.addParameter(QString::fromAscii("TYPE"),QString::fromAscii("HOME"));
    property.addParameter(QString::fromAscii("TYPE"),QString::fromAscii("WORK"));

    document.addProperty(property);
    QContact contact = mImporter->importContact(document);
    const QContactPhoneNumber& phone = 
        static_cast<QContactPhoneNumber>(
            contact.detail(QContactPhoneNumber::DefinitionName));
    QCOMPARE(phone.number(),QString(QString::fromAscii(value)));

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

void UT_QVersitContactImporter::testEmail()
{
    QVersitProperty property;
    property.setName(QString::fromAscii("EMAIL"));
    QByteArray value("homer.simpson@burns-corporation.com");
    property.setValue(value);
    property.addParameter(QString::fromAscii("TYPE"),QString::fromAscii("WORK"));
    QVersitDocument document = createDocumentWithProperty(property);
    QContact contact = mImporter->importContact(document);
    QContactEmailAddress email =
        static_cast<QContactEmailAddress>(
            contact.detail(QContactEmailAddress::DefinitionName));
    QCOMPARE(email.emailAddress(),QString::fromAscii(value));
    const QStringList contexts = email.contexts();
    QCOMPARE(contexts.count(),1);
    QVERIFY(contexts.contains(QContactDetail::ContextWork)); 
}

void UT_QVersitContactImporter::testUrl()
{
    QVersitProperty property;
    property.setName(QString::fromAscii("URL"));
    QByteArray value("http://www.simpsonsmovie.com/homer.html");
    property.setValue(value);
    property.addParameter(QString::fromAscii("TYPE"),QString::fromAscii("WORK"));
    QVersitDocument document = createDocumentWithProperty(property);    
    QContact contact = mImporter->importContact(document);
    QContactUrl url =
        static_cast<QContactUrl>(
            contact.detail(QContactUrl::DefinitionName));
    QCOMPARE(url.url(),QString::fromAscii(value));
    const QStringList contexts = url.contexts();
    QCOMPARE(contexts.count(),1);
    QVERIFY(contexts.contains(QContactDetail::ContextWork));    
}

void UT_QVersitContactImporter::testUid()
{
    QVersitProperty property;
    property.setName(QString::fromAscii("UID"));
    QByteArray value("unique identifier");
    property.setValue(value);
    QVersitDocument document = createDocumentWithProperty(property);
    QContact contact = mImporter->importContact(document);
    QContactGuid uid =
        static_cast<QContactGuid>(
            contact.detail(QContactGuid::DefinitionName));
    QCOMPARE(uid.guid(),QString::fromAscii(value));    
}

void UT_QVersitContactImporter::testTimeStamp()
{
    // Simple date : ISO 8601 extended format
    QVersitProperty property;
    property.setName(QString::fromAscii("REV"));
    QByteArray dateValue("1981-05-20");
    property.setValue(dateValue);
    QVersitDocument document = createDocumentWithProperty(property);
    QContact contact = mImporter->importContact(document);
    QContactTimestamp timeStamp =
        static_cast<QContactTimestamp>(
            contact.detail(QContactTimestamp::DefinitionName));    
    QCOMPARE(timeStamp.lastModified().date().toString(Qt::ISODate),QString::fromAscii(dateValue));

    // Date and Time : ISO 8601 extended format without utc offset
    QByteArray dateAndTimeValue("1981-05-20T23:55:55");
    property.setValue(dateAndTimeValue);
    document = createDocumentWithProperty(property);
    contact = mImporter->importContact(document);
    timeStamp =
        static_cast<QContactTimestamp>(
            contact.detail(QContactTimestamp::DefinitionName));    
    QCOMPARE(timeStamp.lastModified().toString(Qt::ISODate),QString::fromAscii(dateAndTimeValue));    

    // Date and Time : ISO 8601 extented format with utc offset
    QByteArray utcOffset = "Z";
    QByteArray dateAndTimeWithUtcValue = dateAndTimeValue+utcOffset;
    property.setValue(dateAndTimeWithUtcValue);
    document = createDocumentWithProperty(property);
    contact = mImporter->importContact(document);
    timeStamp =
        static_cast<QContactTimestamp>(
            contact.detail(QContactTimestamp::DefinitionName));
    QCOMPARE(timeStamp.lastModified().toString(Qt::ISODate),QString::fromAscii(dateAndTimeValue));
    QCOMPARE(timeStamp.lastModified().timeSpec(),Qt::UTC);

    // Date and Time : ISO 8601 in basic format without utc offset
    dateAndTimeValue = "19810520T235555";
    property.setValue(dateAndTimeValue);
    document = createDocumentWithProperty(property);
    contact = mImporter->importContact(document);
    timeStamp =
        static_cast<QContactTimestamp>(
            contact.detail(QContactTimestamp::DefinitionName));

    QCOMPARE(timeStamp.lastModified().toString(QString::fromAscii("yyyyMMddThhmmss")),
             QString::fromAscii(dateAndTimeValue));

    // Date and Time : ISO 8601 in basic format with utc offset
    dateAndTimeValue = "19810520T235555";
    dateAndTimeWithUtcValue = dateAndTimeValue+utcOffset;
    property.setValue(dateAndTimeWithUtcValue);
    document = createDocumentWithProperty(property);
    contact = mImporter->importContact(document);
    timeStamp =
        static_cast<QContactTimestamp>(
            contact.detail(QContactTimestamp::DefinitionName));
    QCOMPARE(timeStamp.lastModified().toString(QString::fromAscii("yyyyMMddThhmmss")),
             QString::fromAscii(dateAndTimeValue));
    QCOMPARE(timeStamp.lastModified().timeSpec(),Qt::UTC);
}

void UT_QVersitContactImporter::testAnniversary()
{
    // Date : ISO 8601 extended format
    QVersitProperty property;
    property.setName(QString::fromAscii("X-ANNIVERSARY"));
    QByteArray dateValue("1981-05-20");
    property.setValue(dateValue);
    QVersitDocument document = createDocumentWithProperty(property);
    QContact contact = mImporter->importContact(document);
    QContactAnniversary anniversary =
        static_cast<QContactAnniversary>(
            contact.detail(QContactAnniversary::DefinitionName));
    QCOMPARE(anniversary.originalDate().toString(Qt::ISODate),QString::fromAscii(dateValue));

    // Date : ISO 8601 in basic format
    dateValue = "19810520";
    property.setValue(dateValue);
    document = createDocumentWithProperty(property);
    contact = mImporter->importContact(document);
    anniversary =
        static_cast<QContactAnniversary>(
            contact.detail(QContactAnniversary::DefinitionName));
    QCOMPARE(anniversary.originalDate().toString(QString::fromAscii("yyyyMMdd")),
             QString::fromAscii(dateValue));

}

void UT_QVersitContactImporter::testBirthday()
{
    // Date : ISO 8601 extended format
    QVersitProperty property;
    property.setName(QString::fromAscii("BDAY"));
    QByteArray dateValue("1981-05-20");
    property.setValue(dateValue);
    QVersitDocument document = createDocumentWithProperty(property);
    QContact contact = mImporter->importContact(document);
    QContactBirthday bday =
        static_cast<QContactBirthday>(
            contact.detail(QContactBirthday::DefinitionName));
    QCOMPARE(bday.date().toString(Qt::ISODate),
             QString::fromAscii(dateValue));

    // Date : ISO 8601 in basic format
    dateValue = "19810520";
    property.setValue(dateValue);
    document = createDocumentWithProperty(property);
    contact = mImporter->importContact(document);
    bday =
        static_cast<QContactBirthday>(
            contact.detail(QContactBirthday::DefinitionName));
    QCOMPARE(bday.date().toString(QString::fromAscii("yyyyMMdd")),
             QString::fromAscii(dateValue));

}

void UT_QVersitContactImporter::testGender()
{
    // Date : ISO 8601 extended format
    QVersitProperty property;
    property.setName(QString::fromAscii("X-GENDER"));
    QByteArray val("Male");
    property.setValue(val);
    QVersitDocument document = createDocumentWithProperty(property);
    QContact contact = mImporter->importContact(document);
    QContactGender  gender =
        static_cast<QContactGender >(
            contact.detail(QContactGender ::DefinitionName));
    QCOMPARE(gender.gender(),QString::fromAscii(val));
}

void UT_QVersitContactImporter::testNickname()
{
    // one value
    QVersitDocument document;
    QVersitProperty nameProperty;
    QString singleVal(QString::fromAscii("Homie"));
    nameProperty.setName(QString::fromAscii("NICKNAME"));
    nameProperty.setValue(singleVal.toAscii());
    document.addProperty(nameProperty);
    QContact contact = mImporter->importContact(document);
    QContactNickname nickName = (QContactNickname)contact.detail(QContactNickname::DefinitionName);
    QCOMPARE(nickName.nickname(),singleVal);

    // comma separated values should generate multiple nickname fields
    contact.clearDetails();
    document = QVersitDocument();
    QStringList multiVal;
    multiVal.append(QString::fromAscii("Homie"));
    multiVal.append(QString::fromAscii("SuperHero"));
    multiVal.append(QString::fromAscii("NukeSpecialist"));
    nameProperty.setName(QString::fromAscii("NICKNAME"));
    nameProperty.setValue(multiVal.join(QString::fromAscii(",")).toAscii());
    document.addProperty(nameProperty);
    contact = mImporter->importContact(document);
    QList<QContactDetail> nickNames = contact.details(QContactNickname::DefinitionName);
    QCOMPARE(nickNames.count(),3);
    nickName = static_cast<QContactNickname>(nickNames[0]);
    QCOMPARE(nickName.nickname(),QString::fromAscii("Homie"));
    nickName = static_cast<QContactNickname>(nickNames[1]);
    QCOMPARE(nickName.nickname(),QString::fromAscii("SuperHero"));
    nickName = static_cast<QContactNickname>(nickNames[2]);
    QCOMPARE(nickName.nickname(),QString::fromAscii("NukeSpecialist"));

    // X-NICKNAME
    document = QVersitDocument();
    nameProperty = QVersitProperty();
    nameProperty.setName(QString::fromAscii("X-NICKNAME"));
    nameProperty.setValue(singleVal.toAscii());
    document.addProperty(nameProperty);
    contact = mImporter->importContact(document);
    nickName =
        static_cast<QContactNickname>(
            contact.detail(QContactNickname::DefinitionName));
    QCOMPARE(nickName.nickname(),singleVal);
}

void UT_QVersitContactImporter::testAvatarJpegStored()
{
    // JPEG image. Name property present. The image directory exist.
    // Test that the avatar detail is created and the image
    // is stored on the disk.

    QByteArray img =
"/9j/4AAQSkZJRgABAQAAAQABAAD/2wCEABALDA4MChAODQ4SERATGCgaGBYWGDEj"
"JR0oOjM9PDkzODdASFxOQERXRTc4UG1RV19iZ2hnPk1xeXBkeFxlZ2MBERISGBUY"
"LxoaL2NCOEJjY2NjY2NjY2NjY2NjY2NjY2NjY2NjY2NjY2NjY2NjY2NjY2NjY2Nj"
"Y2NjY2NjY2NjY//AABEIAGAAPAMBIgACEQEDEQH/xAGiAAABBQEBAQEBAQAAAAAA"
"AAAAAQIDBAUGBwgJCgsQAAIBAwMCBAMFBQQEAAABfQECAwAEEQUSITFBBhNRYQci"
"cRQygZGhCCNCscEVUtHwJDNicoIJChYXGBkaJSYnKCkqNDU2Nzg5OkNERUZHSElK"
"U1RVVldYWVpjZGVmZ2hpanN0dXZ3eHl6g4SFhoeIiYqSk5SVlpeYmZqio6Slpqeo"
"qaqys7S1tre4ubrCw8TFxsfIycrS09TV1tfY2drh4uPk5ebn6Onq8fLz9PX29/j5"
"+gEAAwEBAQEBAQEBAQAAAAAAAAECAwQFBgcICQoLEQACAQIEBAMEBwUEBAABAncA"
"AQIDEQQFITEGEkFRB2FxEyIygQgUQpGhscEJIzNS8BVictEKFiQ04SXxFxgZGiYn"
"KCkqNTY3ODk6Q0RFRkdISUpTVFVWV1hZWmNkZWZnaGlqc3R1dnd4eXqCg4SFhoeI"
"iYqSk5SVlpeYmZqio6Slpqeoqaqys7S1tre4ubrCw8TFxsfIycrS09TV1tfY2dri"
"4+Tl5ufo6ery8/T19vf4+fr/2gAMAwEAAhEDEQA/AO/qC8uUs7SW4k+7GucDqT2A"
"9zU9ZWp/vbuONuUjXeVPcngH8MH86yrVFSg5voOKu7GfK19ePlneJNvdyMk+gUjg"
"e5zT4UvLfYYp4yy4ySrDcPf5sfpViivEeNrN3udKpxL9percMY2UxygZ2E5yPUHu"
"Kt1gzt5Ki4H3oTv47gdR+IyK3QQQCOhr1cLXdaF3ujCceVi0lLSV1kC1lX3y35J7"
"xL+hb/GtWuX8Sy3C3wRcbGjESkdcueh/75Nc+Jhz0nEqDtK5ehiublPMhjQRn7rS"
"MRu9wADxUby+UzJMhSRcfL1zngY9cmtaN/Ot4XtnVUODyufl7j2NUdSkA1S1ZF8x"
"oAzOo9CMAfXv+FctTBUYwve3mWqkmyN7a7aAs1sCpHKB/nx9On61f0xmfToS5y23"
"HPXj19/X3qE6pbbwZZJLfYC21wMOP1z9BzVixybbfjAkZnA9ATkV00KVOGtPqRKT"
"e5ZpKWkrpJFrF1jzHvoUTaNgVwD/ABckH8h/6FW1VLVkVrPcygsjqVPcfMKxrxcq"
"bSY4uzMSe5WKV0tw3mfx7ZGRQffHeoUmuEGFeNc8n5Ccn3JPNQRuFthI+eRvbAzy"
"eT/Ol844yIZMeuB/LrXmKHu8r1OtJbjp2uZCH81CyA7V2YByMc11WnuklhA0eduw"
"AZ6jHGK5L7SnA2y5JwB5TD+ldJottcW1qy3HyhnLJHkHYO/T3yfxrswseW6SMqtj"
"RpKWkrtMBao6rNGlsI2b53Zdqjkn5hV6sW7V7uWWOE/vzLkc8BUAOD7EnH4+1RNX"
"i0gW5mS20lux2IXhJyNoyV9sdx9Ki8wZxskJ9PLbP8q0/OXyt5BBzjZjnd/dx65q"
"e3sWN1svSW3x71RWICEHBHHXqP1ryMPGrUurbHVKaiZVmrDUIZpo28qLLMnXaMff"
"YdsHGB9a6wEEZHIquy2tlbtlY4oscgDGf8ar2F9ELGDf5iqEA3NGwGPrivVpQ5I2"
"ZzylzO5o0lIrK6hlIZT0IPBpa1JILqV1CxQ486Thc9FHdj9P54qGwt4orm5aNQMb"
"Yy2OWIGSSe5+apItp1Gcsf3gRQo/2eefzz+QpNPO5Z27NO/P0OP6UAS/ZoPP8/yU"
"83+/tGaJ7fzWRhI8bJnDJjoeo5H0qaikBnT2sIlhhwXeVjueQ7m2gZI56A8DHvWg"
"OBgVVuPkvrWU/d+aMn0LYx/LH41aJAGScAd6YFSWE2xM9svHV4R0YdyB2P8AOrSO"
"siK6EFWGQR3FUJ9TUgpZgTP/AH/4B+Pf8Kn05w+nwY/hQKfYjgj8xURqRlJxT1Q7"
"NK5kSLNd3SSAtFcyttQg8xp1I/LP4/hW9GixRrGgwqgAD2FZ+lx77macn7mIlHp0"
"JP6j8q06xwql7Pmk9XqVO17IKKKK6SBkkaSxskihlYYINYbQq880UpeXyZCoEjFg"
"BgEcH2IrerEBD3FzMpyJJSR+AC/+y1wY+VqWj6mlJe8Oq1o/+omXsszY/HB/rVWr"
"ejqfsruRgSSMw9x0B/SuTLk/aP0NKux//9k=";

    QStringList nameValues(QString::fromAscii("Homer")); // First name
    nameValues.append(QString::fromAscii("Simpson")); // Last name
    QByteArray name = nameValues.join(QString::fromAscii(";")).toAscii();
    QVersitDocument document =
        createDocumentWithNameAndPhoto(
            name,img,QString::fromAscii("JPEG"),
            QString::fromAscii("BASE64"));
    mImporter->setImagePath(imageAndAudioClipPath);
    QContact contact = mImporter->importContact(document);
    QContactAvatar avatar =
        static_cast<QContactAvatar>(contact.detail(QContactAvatar::DefinitionName));
    QVERIFY(avatar.subType() == QContactAvatar::SubTypeImage);
    QString fileName = avatar.avatar();
    QVERIFY(fileName.contains(nameValues.at(0) + nameValues.at(1)));
    QVERIFY(fileName.endsWith(QString::fromAscii(".jpg")));
    QFile file(fileName);
    QVERIFY(file.open(QIODevice::ReadOnly));
    QByteArray content = file.readAll();
    file.close();
    QCOMPARE(content,img);
}

void UT_QVersitContactImporter::testAvatarGifStored()
{
    QByteArray img =
"R0lGODlhEgASAIAAAAAAAP///yH5BAEAAAEALAAAAAASABIAAAIdjI+py+0G"
"wEtxUmlPzRDnzYGfN3KBaKGT6rDmGxQAOw==";
    QStringList nameValues(QString::fromAscii("Homer")); // First name
    nameValues.append(QString::fromAscii("Simpson")); // Last name
    QByteArray name = nameValues.join(QString::fromAscii(";")).toAscii();
    QVersitDocument document =
        createDocumentWithNameAndPhoto(
            name,img,QString::fromAscii("GIF"),
            QString::fromAscii("B"));
    mImporter->setImagePath(imageAndAudioClipPath);
    QContact contact = mImporter->importContact(document);
    QContactDetail detail = contact.detail(QContactAvatar::DefinitionName);
    QVERIFY(!detail.isEmpty());
    // Take out the random part from the original file name
    QContactAvatar avatar = static_cast<QContactAvatar>(detail);
    QString fileName = avatar.avatar();
    QVERIFY(avatar.subType() == QContactAvatar::SubTypeImage);
    QDir dir;
    QVERIFY(dir.exists(avatar.avatar()));
    QVERIFY(fileName.contains(nameValues.at(0) + nameValues.at(1)));
    QVERIFY(fileName.endsWith(QString::fromAscii(".gif")));
    QFile file(fileName);
    QVERIFY(file.open(QIODevice::ReadOnly));
    QByteArray content = file.readAll();
    QCOMPARE(content,img);
}

void UT_QVersitContactImporter::testAvatarJpegTwoContactsWithSameName()
{
    // Create 2 contacts with the same name.
    // Test that the generated photo files have different names
    QByteArray img =
"/9j/4AAQSkZJRgABAgAAZABkAAD/7AARRHVja3kAAQAEAAAAHgAA/+4ADkFkb2JlAGTAAAAAAf\
/bAIQAEAsLCwwLEAwMEBcPDQ8XGxQQEBQbHxcXFxcXHx4XGhoaGhceHiMlJyUjHi8vMzMvL0BAQ\
EBAQEBAQEBAQEBAQAERDw8RExEVEhIVFBEUERQaFBYWFBomGhocGhomMCMeHh4eIzArLicnJy4r\
NTUwMDU1QEA/QEBAQEBAQEBAQEBA/8AAEQgBhAGGAwEiAAIRAQMRAf/EAT8AAAEFAQEBAQEBAAA\
AAAAAAAMAAQIEBQYHCAkKCwEAAQUBAQEBAQEAAAAAAAAAAQACAwQFBgcICQoLEAABBAEDAgQCBQ\
cGCAUDDDMBAAIRAwQhEjEFQVFhEyJxgTIGFJGhsUIjJBVSwWIzNHKC0UMHJZJT8OHxY3M1FqKyg\
yZEk1RkRcKjdDYX0lXiZfKzhMPTdePzRieUpIW0lcTU5PSltcXV5fVWZnaGlqa2xtbm9jdHV2d3\
h5ent8fX5/cRAAICAQIEBAMEBQYHBwYFNQEAAhEDITESBEFRYXEiEwUygZEUobFCI8FS0fAzJGL\
hcoKSQ1MVY3M08SUGFqKygwcmNcLSRJNUoxdkRVU2dGXi8rOEw9N14/NGlKSFtJXE1OT0pbXF1e\
X1VmZ2hpamtsbW5vYnN0dXZ3eHl6e3x//aAAwDAQACEQMRAD8A9ASSSSUpJJJJSkkkklKSSSSUp\
JJJJSkkkklKSSSSUpJJJJSkySdJSkkkklKSTEgCToFXfmN4qG8/vfmpk8kMY4pyER4pESdg2Uyo\
uvudy6PIaIck8mVSn8TxDSMZT/BkGE9S6UhJZwPgpCx7fouITY/FIdcch5G0+z2LoJKozKePpCR\
+KsMtZYPadfBW8PNYcukZa/unQscoSjuzSSSU61ZOmSSUpOkkkpSSSSSlJJJJKUkkkkpSSSSSlJ\
JJJKUmTpJKUkkkkpSSSSSlJJJJKUmMwY5TpJKW+KdJJJSkkkklKSSSSUpJJJJSkkkklKSSSSUpJ\
JJJSlC21lTC95gD7yfAJrbWUsL3nTsO5PgFQc99r/Ut0I+gzs0f3qvzPMxwRs6yPyxXwgZHwZWW\
WXGX6N7Vj+PiopJLCy5Z5JcUzZ/ls2QABQVKSSUqNSkkpS7pBSk4JBkaFMkiNCpt05AdDX6HsUd\
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\
XCXdMn7JKUklOqSClJJd0vFJCuUkgkUkqSSS7/BJCkuySSKlJ2kgyOUx/FJCyCpvU3CwQfpIqzW\
uIII0IV2m4WCDo7wWzyXOe4BjyH1jY/vf2tfJjrUbJUkkloMakkkklKTOc1rS5xho1JKdZ+Tab7\
XVj+aq0P8p/n5N/Kos+aOLGZy6bDuV0Y8Rpa652QeCKR9Fp5d5lR7J0lz+bNPLMzmfLsPJsgACg\
skkko0rJJFOklYpdk6ZLqpdJJJJCkkkoSSsknTJKUnSTJKXSSKSSlkinSRUpMkn7pKW7pcJJaoK\
UkknSClkkuyXZFSk6ZOgpZJJJKlKTpJSipSSSSClJJeCSKlJJd0klKKSSbugpdSaSDIKikjqDaG\
9TcLBB0cirOa4tIIOqvVP9Rgd37ra5HmzlHtz+eI0P7w/i18kK1GzNJJJXmNha/wBOp7/3Wl33C\
VmY4IpZOpIlx8zqfxK0cljn41rG/Scxwb8SIWdQ4OqY7xaD+CzPihPDjHS5fsZsP6X0SJk6SyWZ\
bukkkkpSXCSRSSpJJJJSkkvgnSQsklKSVqUl3SS5KSVJJJIoUlokkkVKShJJJSku6SSCVQkkkih\
bVOkkUkqTd06SRUrskkkgpSZOkUVKShJJJCkuyUpd0lKS8UkklK0SSSSUpKEkgEFK7JJBOkpQCs\
YrjuLfESq6Pij3k+SscoSOYx1+8syfKW2kkkuhaylmPq9C11fDHEur+B1I+RWmh21MuZtePMHuD\
4hQczgGbGYbEaxPivhPhPh1aISU30W1nUbm/vD+IUPyrCyYcmM1OJH5fa2AQdQbUmTpKIhKuySS\
SNFSkycpIKWhOkmSUrRIhJJJSkkkiipSSSSSlBIpJJKUEkku6SlJJJeSSlu6dJJJKkkkkkKSSS7\
pFSkkkikpXdJMOU8oJUlKSSSFJJJIqUnTBOgpSSSUI0eylJJJFKipQ4S1nyS7Jwx7tAJlIRkdAC\
T4KtZXMevayTyVGrGiHP8AuR1q8jycoH3cgo/ox6+ZYMkwdAukkktNiUkkkkpSaAeQnSSUx2t8B\
9yW1vgFJJCh2Va21vgE21vgPuUkkqHZVsHV1uEFoKoZFZxrWQZqsMNnkO5j5rSVPqYnHaf3baz9\
7w3+Kg5rBDJilcRxRiTE9bC/HIiQF6HRCmTplzzZUknTd0lKS7wkl3SSpJJOihbukkkgpSSSXdF\
SkgkkEkq7BJJJJCk0J0u6RUpJJNqkpdIpJFBSgkkEiklSXdLskkhSXdOm7JFS7ROg7q7XQxo1Eu\
VWnWxvxV5afw3DCXFkkBIxNC2HLIigFbW+ASgeCdJa1BhWgeCW1vgE6SVBTHa3wH3J4jhOklQUp\
JJJJSkkkklKSSSSUpJJJJSydJJJSkkkklKVLq0/YXkdn1H7rGK6qnU9cJ48XMH/AE2psxcJDvEp\
juPNARqU2qdILmG2oJk5TBJSkkkkVK7p0ydJSySSSCVSkkkUUKTBOkklR4KYmEh/vS8+UlLlIpJ\
kFLpJJBFCkjykkkpZOkkgLSpJIpJIUNEkoTpKZ0/zjfiryo0fzrfir62Phf8ANT/v/sYM3zDyUk\
kktFiUkkkkpSSSSSlJJJJKUkkkkpSSSSSlJJJJKUkkkkpSSSSSlKl1b+gP/r1f+fGK6qfVRODYP\
Nn/AFbUzJ/Nz/ulMfmHmEPdMnTLmW2umSShFSkkkigpSdN2SKKlBJLhJJKkiYH8EgmPdDopfukm\
SRUqE4SSSUpJMkgpQTpJh+VFS6RSSSQpJJJJSkkkklKSSThBTOj+dar6oUfzjfir62Phf83P+/8\
AsYM3zDyUkkktFiUkkkkpSSSSSlJJJJKUkkkkpSSSSSlJJJJKUkkkkpSSSSSlKp1T+hP+LP8Aq2\
q2qfVhOBaPNn/VtTcnyS/ulMdx5oO6SR5SXMNtSSRSRUqNZSKSXdJSkkkklKSCSSXVSkoS7peKS\
Vk6bukgpdJLtCXwSUt8Ekk6Slkk5CZJS/ZJJI90UK7pJJIFKkgkkkhXZOm8kuCBHxKKklP8434q\
+qFP842PFX1r/C/5uf8AeH5MGbceSkkklosSkkkklKSSSSUpJJJJSkkkklKSSSSUpJJJJSkkkkl\
KSSSSUpU+qmMGz4sH3vaFcVLq8/YXR+/V93qsTcmkJH+qV0fmHmEXdMl/FLuuYbSkkkkVKTJykA\
gpXKSSSKleSSSSCVJJJJKUUydJJSkkkklLFIJ4lMkpdJIJIqUkkkUkLJ0ydBK6ZJI+aKFDxTlLs\
mSUkp/nW/FX1Qp/nW/FX1r/AAv+bn/f/YwZtx5KSSSWixKSSSSUsnSSSUpJJJJSkkkklKSSSSUp\
JJJJSkkkklKSSSSUpU+qQcJ08bq5/wA9quKn1Uxgv/rV/wDnxqZkNY5n+qfyTH5h5hCkmSXMttS\
SXkkipR4SS5EJIJUkkkihXdJJIIJUkl2SRUpJJJBSpSSSSUpLzTJ0lKhMnTJKXSSKSKlk6ZOAgp\
SRSSKKF0yRPZJAqSU/zrfir6z6f5xvxWgtf4X/ADc/7/7GDNuPJSSSS0mJSSSSSlJJJJKUkkkkp\
SSSSSlJJJJKUkmTpKUkkkkpSSSSSlKn1QTg2DzZ/wBW1XFT6q7bhPP8qv8AGxqbk+SX90pj8w80\
J8UkjymXMNtSSSSKlJFJJBSkkkkVKSSlJJKkkkklKSSSSUryShN3ToKV2TJJ4SUpNynTcJKX7Jk\
6SKllIJgnSCCtKZOkglSSZOOUrUzp/nW/FaCz6f51vxWgtf4X/Nz/ALwa+bceSkkklpMSkkkklK\
SSSSUsnSSSUpJJJJSkkkklKSSSSUpJJJJSkkkklKVLqwnAs+LD9z2lXVU6n/QrPiz/AKtqZk+SX\
90pj8w8wgOpKZOkuZbaySSSKlJJJJKV8EkkklKSSSSStwl/BOkkpSQSKSSld0ikkUuilJk4S7oK\
UEycpklLpk6ZEqXCSQSSQpJJJBKx4SCSceKSmVX863wkLRWfT/ON+IWgtf4X8mT+8Gvm3HkpJJJ\
aTEpJJJJSkkkklKSSSSUpJJJJSkkkklKSSSSUpJJJJSkkkklKVPqv9Bf/AFq//PjVcVTqn9Dd/W\
Z/1bUzJ/Nz/un8l0fmHmEB0lJLul4LmW0pMUkkVK7Jk6SSVcpJJJBCu6SSSCVBJJLsihSSSRSUp\
JJJJS6ZJJJSkku6UIKUkkkipSRSSKSlJBJJLqpSQSSGiCWdP8434rQVCn+cb8VfWv8AC/kyf3g1\
8248lJJJLSYlJJJJKUkkkkpSSSSSlJJJJKUkkmSUukmTpKUkkkkpSSSSSlKp1QxhPPg6ufh6jZV\
tCyavXx7Kv32kD49k2YuMh3BCYmiD2LSSUa3b2Bx0PDh4HupQuZIINHcNtZJOl2QUpMnASRUsEk\
kkgpRS7pJ0lLJJ4TJKV2SSSQKlFJJLskpSSXknRUskkE6Sld1FOlCSlJJd0/dJSySRlJJSil2S7\
p+2iCmdOtrfir6p4rSX7uzVcW18MiRilL96Wn0a+U+ryCkkklfY1JJJJKUkkkkpZOkkkpSSSSSl\
JJJJKUkkkkpSSSSSlJJJJKUkkkkppZOM5rjbUJDtXsHj+8EEEH5crSUXVVv+k0E+Ko8zyEcsjOB\
4JHfsWWOWhR1aCSufZqvA/el9mq8/vVT/AEZm7w+0/wAF/ux8Wmkrn2arwP3pfZqvA/el/ozN+9\
D7T/BXux8Wmkrn2arwP3pfZavA/eh/ozN+9D7T/BXux8WmkrYxax4p/stXn96X+jM/eH2n+Cvdj\
4tNLurn2Wrz+9L7LV5/el/o3P3h9p/gr3Y+LTShXPstXn96b7LV5/el/ozP3h9v9ivdj4tSEoVv\
7JV5/el9lr8/vS/0bn7w+3+xXux8Wokrf2WrzS+y1+aX+jc/eH2/2K92Pi1Elc+y1+aX2Wvz+9L\
/AEbn7w+3+xXux8Wmkrf2Wrz+9P8AZa/P70v9G5+8Pt/sV7sfFppamfBXPstXn96b7LVM6/el/o\
3P3h9pV7sfFqJK39lr8Sl9lq8/vS/0bn7w+3+xXux8WopMY5xAaJVsY1Q7T8UQNDRAEBSY/hk7/\
WSiB/V1KDmHQMa6xW2B8yppJLUhGMIiMRQiKDATepUkkknKUkkkkpSSSSSlJJJJKUkkkkpSSSSS\
lJJJJKUkkkkpSSSSSlk6SSSlk6SSSlJJJJKUkkkkpSSSSSlJJJJKUkkkkpSSSSSlJJJJKUkkkkp\
SSSSSlJJJJKUkkkkpSSSSSlJJJJKUkkkkpSSSSSlJJJJKUkkkkpSSSSSlJJJJKf/Z";

QStringList nameValues(QString::fromAscii("Homer")); // First name
    nameValues.append(QString::fromAscii("Simpson")); // Last name
    QByteArray name = nameValues.join(QString::fromAscii(";")).toAscii();
    QVersitDocument document1 =
        createDocumentWithNameAndPhoto(
            name,img,QString::fromAscii("JPEG"),
            QString::fromAscii("BASE64"));
    QVersitDocument document2 =
        createDocumentWithNameAndPhoto(
            name,img,QString::fromAscii("JPEG"),
            QString::fromAscii("B"));

    mImporter->setImagePath(imageAndAudioClipPath);
    QContact contact1 = mImporter->importContact(document1);
    QContact contact2 = mImporter->importContact(document2);
    QContactAvatar avatar1 =
        static_cast<QContactAvatar>(contact1.detail(QContactAvatar::DefinitionName));
    QContactAvatar avatar2 =
        static_cast<QContactAvatar>(contact2.detail(QContactAvatar::DefinitionName));
    QVERIFY(avatar1.subType() == QContactAvatar::SubTypeImage);
    QVERIFY(avatar2.subType() == QContactAvatar::SubTypeImage);
    QVERIFY(avatar1.avatar() != avatar2.avatar());
}

void UT_QVersitContactImporter::testAvatarJpegNonexistentPath()
{
    // JPEG image. Name property present. The image path doesn't exist.
    // Test that the avatar detail is not created and there is no image
    // stored on the disk.
    QByteArray img = "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";
    QStringList nameValues(QString::fromAscii("Homer")); // First name
    nameValues.append(QString::fromAscii("Simpson")); // Last name
    QByteArray name = nameValues.join(QString::fromAscii(";")).toAscii();
    QVersitDocument document =
        createDocumentWithNameAndPhoto(
            name,img,QString::fromAscii("JPEG"),
            QString::fromAscii("BASE64"));

    mImporter->setImagePath(QString::fromAscii("some_nonexistent/path/anywhere543"));
    QContact contact = mImporter->importContact(document);

    QContactDetail detail = contact.detail(QContactAvatar::DefinitionName);
    QVERIFY(detail.isEmpty());

    QDir dir(mImporter->imagePath());
    QVERIFY(!dir.exists());
}

void UT_QVersitContactImporter::testAvatarUrl()
{
    QVersitProperty property;
    property.setName(QString::fromAscii("PHOTO"));
    QByteArray value("file:///jgpublic.");
    property.setValue(value);
    property.addParameter(
        QString::fromAscii("VALUE"),QString::fromAscii("URL"));

    QVersitDocument document;
    document.addProperty(property);

    QContact contact = mImporter->importContact(document);
    QContactAvatar avatar =
        static_cast<QContactAvatar>(contact.detail(QContactAvatar::DefinitionName));
    QCOMPARE(avatar.avatar(), QString::fromAscii("file:///jgpublic."));
    QVERIFY(avatar.subType() == QContactAvatar::SubTypeImage);
}

void UT_QVersitContactImporter::testAvatarEncoding()
{
    // Tests only quoted-printable encoding.
    // Assumes that this is the only other encoding supported in PHOTO.
    // In versit property the image is already decoded by the reader.
    QByteArray img =
"R0lGODlhEgASAIAAAAA"
"AAP///yH5BAEAAAEALAAAAAASABIAAAIdjI+py+0G"
"wEtxUmlPzRDnzYGfN3KBaKGT6rDmGxQAOw==";
    QStringList nameValues(QString::fromAscii("Homer")); // First name
    nameValues.append(QString::fromAscii("Simpson")); // Last name
    QByteArray name = nameValues.join(QString::fromAscii(";")).toAscii();
    QVersitDocument document =
        createDocumentWithNameAndPhoto(
            name,img,QString::fromAscii("GIF"),
            QString::fromAscii("8BIT"));

    mImporter->setImagePath(imageAndAudioClipPath);
    QContact contact = mImporter->importContact(document);

    QContactDetail detail = contact.detail(QContactAvatar::DefinitionName);
    QVERIFY(!detail.isEmpty());
    QContactAvatar avatar = static_cast<QContactAvatar>(detail);
    QVERIFY(avatar.subType() == QContactAvatar::SubTypeImage);
    QDir dir;
    QVERIFY(dir.exists(avatar.avatar()));
}

void UT_QVersitContactImporter::testGeo()
{
    // some positive values
    QVersitDocument document;
    QVersitProperty nameProperty;
    QStringList val;    
    val.append(QString::fromAscii("18.53"));// Longtitude
    val.append(QString::fromAscii("45.32")); // Latitude
    nameProperty.setName(QString::fromAscii("GEO"));
    nameProperty.setValue(val.join(QString::fromAscii(",")).toAscii());
    document.addProperty(nameProperty);
    QContact contact = mImporter->importContact(document);
    QContactGeolocation geo = (QContactGeolocation)contact.detail(QContactGeolocation::DefinitionName);
    QString str;
    str.setNum(geo.longitude(),'.',2);
    QCOMPARE(str,val[0]);
    str.setNum(geo.latitude(),'.',2);
    QCOMPARE(str,val[1]);

    // some negative values
    document = QVersitDocument();
    nameProperty = QVersitProperty();
    val.append(QString::fromAscii("18.53"));// Longtitude
    val.append(QString::fromAscii("-45.32")); // Latitude
    nameProperty.setName(QString::fromAscii("GEO"));
    nameProperty.setValue(val.join(QString::fromAscii(",")).toAscii());
    document.addProperty(nameProperty);
    contact = mImporter->importContact(document);
    geo = (QContactGeolocation)contact.detail(QContactGeolocation::DefinitionName);
    str.setNum(geo.longitude(),'.',2);
    QCOMPARE(str,val[0]);
    str.setNum(geo.latitude(),'.',2);
    QCOMPARE(str,val[1]);
}

void UT_QVersitContactImporter::testNote()
{
    // single line value
    QVersitDocument document;
    QVersitProperty nameProperty;
    QByteArray val("I will not sleep at my work -Bart");
    nameProperty.setName(QString::fromAscii("NOTE"));
    nameProperty.setValue(val);
    document.addProperty(nameProperty);
    QContact contact = mImporter->importContact(document);
    QContactNote note = (QContactNote)contact.detail(QContactNote::DefinitionName);
    QCOMPARE(note.note(),QString::fromAscii(val));

    // Multiline value and quoted printable encoding
    document = QVersitDocument();
    nameProperty = QVersitProperty();
    val = QByteArray("My Dad acts like he belongs,=0D=0AHe belongs in the zoo.=0D=0A");
    nameProperty.setName(QString::fromAscii("NOTE"));
    nameProperty.setValue(val);
    QMultiHash<QString,QString> params;
    params.insert(QString::fromAscii("QUOTED-PRINTABLE"),QString::fromAscii(val));
    nameProperty.setParameters(params);
    document.addProperty(nameProperty);
    contact = mImporter->importContact(document);
    note = (QContactNote)contact.detail(QContactNote::DefinitionName);
    QCOMPARE(note.note(),QString::fromAscii(val));
}

void UT_QVersitContactImporter::testOnlineAccount()
{
    QByteArray accountUri("sip:homer@simpsons.com");

    // Plain X-SIP, no TYPE ->
    QVersitDocument document;
    QVersitProperty property;
    property.setName(QString::fromAscii("X-SIP"));
    property.setValue(accountUri);
    document.addProperty(property);
    QContact contact = mImporter->importContact(document);
    QContactOnlineAccount onlineAccount =
         static_cast<QContactOnlineAccount>(
             contact.detail(QContactOnlineAccount::DefinitionName));
    QCOMPARE(onlineAccount.accountUri(),QString::fromAscii(accountUri));
    QStringList subTypes = onlineAccount.subTypes();
    QCOMPARE(subTypes.count(),1);
    QVERIFY(subTypes.first() == QContactOnlineAccount::SubTypeSip);

    // VideoSharing subtype
    document = QVersitDocument();
    property = QVersitProperty();
    property.setName(QString::fromAscii("X-SIP"));
    property.setValue(accountUri);
    QMultiHash<QString,QString> params;
    params.insert(QString::fromAscii("TYPE"),QString::fromAscii("SWIS"));
    property.setParameters(params);
    document.addProperty(property);
    contact = mImporter->importContact(document);
    onlineAccount =
         static_cast<QContactOnlineAccount>(
             contact.detail(QContactOnlineAccount::DefinitionName));
    QCOMPARE(onlineAccount.accountUri(),QString::fromAscii(accountUri));
    subTypes = onlineAccount.subTypes();
    QCOMPARE(subTypes.count(),1);
    QVERIFY(subTypes.first() == QContactOnlineAccount::SubTypeVideoShare);

    // Internet subtype
    document = QVersitDocument();
    property = QVersitProperty();
    property.setName(QString::fromAscii("X-SIP"));
    property.setValue(accountUri);
    params.clear();
    params.insert(QString::fromAscii("TYPE"),QString::fromAscii("VOIP"));
    property.setParameters(params);
    document.addProperty(property);
    contact = mImporter->importContact(document);
    onlineAccount =
         static_cast<QContactOnlineAccount>(
             contact.detail(QContactOnlineAccount::DefinitionName));
    QCOMPARE(onlineAccount.accountUri(),QString::fromAscii(accountUri));
    subTypes = onlineAccount.subTypes();
    QCOMPARE(subTypes.count(),1);
    QVERIFY(subTypes.first() == QContactOnlineAccount::SubTypeSipVoip);
}

void UT_QVersitContactImporter::testFamily()
{
    // Interesting : kid but no wife :)
    QVersitDocument document;
    QVersitProperty nameProperty;
    QByteArray val("Maggie"); // one is enough
    nameProperty.setName(QString::fromAscii("X-CHILDREN"));
    nameProperty.setValue(val);
    document.addProperty(nameProperty);
    QContact contact = mImporter->importContact(document);
    QContactFamily family = (QContactFamily)contact.detail(QContactFamily::DefinitionName);
    QStringList children = family.children();
    QCOMPARE(children.count(),1); // ensure no other kids in list
    QCOMPARE(family.spouse(),QString()); // make sure no wife
    QCOMPARE(children[0],QString::fromAscii(val)); // ensure it is your kid

    // Critical : wife but no kids , happy hours
    document = QVersitDocument();
    nameProperty = QVersitProperty();
    nameProperty.setName(QString::fromAscii("X-SPOUSE"));
    val = "March";
    nameProperty.setValue(val);
    document.addProperty(nameProperty);
    contact = mImporter->importContact(document);
    family = (QContactFamily)contact.detail(QContactFamily::DefinitionName);
    children = family.children();
    QCOMPARE(children.count(),0); // list should be empty as you know
    QCOMPARE(family.spouse(),QString::fromAscii(val)); // make sure thats your wife:(

    // Hopeless : couple of kids and wife
    document = QVersitDocument();
    // Add kids first
    nameProperty = QVersitProperty();
    nameProperty.setName(QString::fromAscii("X-CHILDREN"));
    QStringList kidsVal;
    kidsVal.append(QString::fromAscii("Bart"));
    kidsVal.append(QString::fromAscii("Liisa"));
    kidsVal.append(QString::fromAscii("Maggie"));
    nameProperty.setValue(kidsVal.join(QString::fromAscii(",")).toAscii());
    document.addProperty(nameProperty);
    // Add wife next
    val = "March";
    nameProperty = QVersitProperty();
    nameProperty.setName(QString::fromAscii("X-SPOUSE"));
    nameProperty.setValue(val);
    document.addProperty(nameProperty);
    contact = mImporter->importContact(document);
    family = (QContactFamily)contact.detail(QContactFamily::DefinitionName);
    children = family.children();
    QCOMPARE(children.count(),3); // too late , count them now.
    // painfull but ensure they are your kids
    QCOMPARE(children.join(QString::fromAscii(",")),kidsVal.join(QString::fromAscii(",")));
    QCOMPARE(family.spouse(),QString::fromAscii(val)); // make sure thats your wife:(
}

void UT_QVersitContactImporter::testSound()
{
    QString path(imageAndAudioClipPath);
    mImporter->setAudioClipPath(path);
    QVersitDocument document;
    QVersitProperty nameProperty;
    nameProperty.setName(QString::fromAscii("N"));
    nameProperty.setValue("Simpson;Homer;;;");
    document.addProperty(nameProperty);
    nameProperty = QVersitProperty();
    QMultiHash<QString,QString> param;
    param.insert(QString::fromAscii("TYPE"),QString::fromAscii("WAVE"));
    param.insert(QString::fromAscii("ENCODING"),QString::fromAscii("BASE64"));
    nameProperty.setName(QString::fromAscii("SOUND"));
    QByteArray val("111110000011111");
    nameProperty.setValue(val.toBase64());
    nameProperty.setParameters(param);
    document.addProperty(nameProperty);
    QContact contact = mImporter->importContact(document);
    QContactAvatar avatar = (QContactAvatar)contact.detail(QContactAvatar::DefinitionName);
    QCOMPARE(avatar.value(QContactAvatar::FieldSubType),QContactAvatar::SubTypeAudioRingtone.operator QString());
    QDir dir(path);    
    QStringList type(QString::fromAscii("*.wav"));
    QStringList files = dir.entryList(type);
    QCOMPARE(files.count(),1);
    QString fileName = avatar.avatar();    
    QVERIFY(fileName.endsWith(QString::fromAscii(".wav")));
    QFile file(fileName);
    QVERIFY(file.open( QIODevice::ReadOnly));
    QByteArray content = file.readAll();
    QCOMPARE(content,val);
}

void UT_QVersitContactImporter::testUnknownVersitProperties()
{
    QVersitDocument document;
    QVersitProperty property;
    QCOMPARE(mImporter->unknownVersitProperties().count(), 0);

    // No unconverted properties, no converted properties either
    mImporter->importContact(document);
    QCOMPARE(mImporter->unknownVersitProperties().count(), 0);

    // No unconverted properties, one converted property
    property.setName(QString::fromAscii("N"));
    property.setValue("Simpson;Homer;J;;");
    document.addProperty(property);
    mImporter->importContact(document);
    QCOMPARE(mImporter->unknownVersitProperties().count(), 0);

    // One unknown property
    property.setName(QString::fromAscii("X-EXTENSION-1"));
    property.setValue("extension value 1");
    document.addProperty(property);
    mImporter->importContact(document);
    QList<QVersitProperty> unknownProperties =
        mImporter->unknownVersitProperties();
    QCOMPARE(unknownProperties.count(), 1);
    QCOMPARE(unknownProperties[0].name(), QString::fromAscii("X-EXTENSION-1"));
    QCOMPARE(QString::fromAscii(unknownProperties[0].value()),
             QString::fromAscii("extension value 1"));

    // Two unknown properties
    property.setName(QString::fromAscii("X-EXTENSION-2"));
    property.setValue("extension value 2");
    document.addProperty(property);
    mImporter->importContact(document);
    unknownProperties = mImporter->unknownVersitProperties();
    QCOMPARE(unknownProperties.count(), 2);
    QCOMPARE(unknownProperties[0].name(), QString::fromAscii("X-EXTENSION-1"));
    QCOMPARE(QString::fromAscii(unknownProperties[0].value()),
             QString::fromAscii("extension value 1"));
    QCOMPARE(unknownProperties[1].name(), QString::fromAscii("X-EXTENSION-2"));
    QCOMPARE(QString::fromAscii(unknownProperties[1].value()),
             QString::fromAscii("extension value 2"));

    // Test that the previous unknown properties are cleaned
    // when importContact is called again
    document = QVersitDocument();
    mImporter->importContact(document);
    unknownProperties = mImporter->unknownVersitProperties();
    QCOMPARE(unknownProperties.count(), 0);
}

QVersitDocument UT_QVersitContactImporter::createDocumentWithProperty(
    const QVersitProperty& property)
{
    QVersitDocument document;
    document.addProperty(property);
    return document;
}

QVersitDocument UT_QVersitContactImporter::createDocumentWithNameAndPhoto(
    const QByteArray& name,
    QByteArray image,
    const QString& imageType,
    const QString& encoding)
{
    QVersitDocument document;

    QVersitProperty nameProperty;
    nameProperty.setName(QString::fromAscii("N"));
    nameProperty.setValue(name);
    document.addProperty(nameProperty);

    QVersitProperty property;
    property.setName(QString::fromAscii("PHOTO"));
    if (encoding == QString::fromAscii("BASE64") ||
        encoding == QString::fromAscii("B")){
        property.setValue(image.toBase64());
    } else {
        property.setValue(image);
    }
    if (imageType != QString()) {
        property.addParameter(QString::fromAscii("TYPE"), imageType);
    }
    if (encoding != QString()) {
        property.addParameter(QString::fromAscii("ENCODING"), encoding);
    }
    document.addProperty(property);

    return document;
}
