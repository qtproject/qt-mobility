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

#include "ut_qversitcontactexporter.h"
#include "qversitcontactexporter.h"
#include "qversitcontactexporter_p.h"
#include "qversitproperty.h"
#include "qversitdefs.h"
#include <QString>
#include <QList>
#include <QtTest/QtTest>
#include <qcontact.h>
#include <qcontactaddress.h>
#include <qcontactemailaddress.h>
#include <qcontactname.h>
#include <qcontactphonenumber.h>
#include <qcontacturl.h>
#include <qcontactguid.h>
#include <qcontacttimestamp.h>
#include <qcontactbirthday.h>
#include <qcontactnote.h>
#include <qcontactgeolocation.h>
#include <qcontactorganization.h>
#include <qcontactavatar.h>
#include <qcontactgender.h>
#include <qcontactnickname.h>
#include <qcontactanniversary.h>
#include <qcontactonlineaccount.h>
#include <qcontactfamily.h>
#include <qcontactpresence.h>
#include <qcontactdisplaylabel.h>


void UT_QVersitContactExporter::scale(
    const QString& imageFileName,
    QByteArray& imageData)
{
    QCOMPARE(imageFileName, mTestPhotoFile);
    imageData = mSimulatedImageData;
    mScaleSignalEmitted = true;
}

void UT_QVersitContactExporter::init()
{
    mConverter = new QVersitContactExporter();
    QObject::connect(
        mConverter, SIGNAL(scale(const QString&,QByteArray&)),
        this, SLOT(scale(const QString&,QByteArray&)));
    mConverterPrivate = new QVersitContactExporterPrivate();
    QVERIFY(!mConverterPrivate->mParameterMappings.empty());
    QVERIFY(!mConverterPrivate->mPropertyMappings.empty());
    mScaleSignalEmitted = false;
}

void UT_QVersitContactExporter::cleanup()
{
    mSimulatedImageData = QByteArray();
    mScaleSignalEmitted = false;
    delete mConverterPrivate;
    delete mConverter;
}

void UT_QVersitContactExporter::initTestCase()
{
    // Create a dummy file
    QDir dir = QDir::current();
    mTestPhotoFile = dir.filePath(QString::fromAscii("versitTest001.jpg"));
    mTestAudioFile = dir.filePath(QString::fromAscii("versitTest001.wav"));
    QFile testPhotoFile(dir.filePath(mTestPhotoFile));
    QFile testAudioFile(dir.filePath(mTestAudioFile));

    if ( testPhotoFile.open( QIODevice::ReadWrite ) ) {
        QTextStream stream( &testPhotoFile );
        stream << "HHH KKK UUU NNN OOO PPP GGG NNN KKK OOO UUU PPP III" << endl;
    }
    testPhotoFile.close();

    if ( testAudioFile.open( QIODevice::ReadWrite ) ) {
        QTextStream stream( &testPhotoFile );
        stream << "HHH KKK UUU NNN OOO PPP GGG NNN KKK OOO UUU PPP III" << endl;
    }
    testAudioFile.close();
}

void UT_QVersitContactExporter::cleanupTestCase()
{
    QDir dir = QDir::current();
    dir.remove(mTestPhotoFile);
    dir.remove(mTestAudioFile);
}

void UT_QVersitContactExporter::testConvertContact()
{
    QContact contact;

    // Adding name to the contact
    QContactName name;
    name.setFirst(QString::fromAscii("Moido"));
    contact.saveDetail(&name);

    // Adding phone number to the Contact.
    QContactPhoneNumber phoneNumber;
    phoneNumber.setNumber(QString::fromAscii("12345678"));
    contact.saveDetail(&phoneNumber);

    // Convert contact into versit properties
    QVersitDocument document = mConverter->exportContact(contact);

    // Each Contact has display label detail by default. Display label is enocded
    // if some value exisit for the Label or if value for Name exisit.
    QCOMPARE(document.properties().count(), 3);
}

void UT_QVersitContactExporter::testUnconvertedContactDetails()
{
    // Test1: Un-supported Avatar Test
    QContact contact;
    QVersitDocument versitDocument;
    QContactAvatar contactAvatar;
    const QString url = QString::fromAscii("http://www.myhome.com/test.jpg");
    contactAvatar.setSubType(QContactAvatar::SubTypeTexturedMesh);
    contact.saveDetail(&contactAvatar);
    versitDocument = mConverter->exportContact(contact);
    QCOMPARE(versitDocument.properties().count(), 0);
    QList<QContactDetail> unconvertedDetails = mConverter->unconvertedContactDetails();
    QString defintionName = contactAvatar.definitionName();
    QContactDetail detail = searchDetail(unconvertedDetails,defintionName);
    QCOMPARE(defintionName, detail.definitionName());

    // Test2: Un-supported Online Account
    QContactOnlineAccount onlineAccount;
    QString testUri = QString::fromAscii("sip:abc@temp.com");
    onlineAccount.setAccountUri(testUri);
    onlineAccount.setSubTypes(QContactOnlineAccount::SubTypeH323);
    contact.saveDetail(&onlineAccount);
    versitDocument = mConverter->exportContact(contact);
    QCOMPARE(versitDocument.properties().count(), 0);
    unconvertedDetails = mConverter->unconvertedContactDetails();
    defintionName = onlineAccount.definitionName();
    detail = QContactDetail();
    detail = searchDetail(unconvertedDetails,defintionName);
    QCOMPARE(defintionName, detail.definitionName());

    // Test3: UnConvered Field Name
    QContactPresence presence;
    detail = QContactDetail();
    presence.setAccountUri(QString::fromAscii("a@abc.com"));
    contact.saveDetail(&presence);
    defintionName = presence.definitionName();
    versitDocument = mConverter->exportContact(contact);
    unconvertedDetails = mConverter->unconvertedContactDetails();
    detail = searchDetail(unconvertedDetails,defintionName);
    QCOMPARE(defintionName, detail.definitionName());
}

void UT_QVersitContactExporter::testEncodeName()
{
    QContact contact;
    QContactName name;

    // vCard 2.1
    name.setFirst(QString::fromAscii("Heido"));
    name.setLast(QString::fromAscii("HH"));
    name.setMiddle(QString::fromAscii("A"));
    name.setPrefix(QString::fromAscii("Mr."));
    name.setContexts(QContactDetail::ContextHome);
    contact.saveDetail(&name);
    QVersitDocument document = mConverter->exportContact(contact);

    // Each Contact has display label detail by default. Display label is enocded
    // if some value exisit for the Label or if value for Name exisit.
    QCOMPARE(document.properties().count(), 2);

    QVersitProperty displayProperty = document.properties().at(0);
    // Check name
    QCOMPARE(displayProperty.name(), QString::fromAscii("FN"));
    // Check value
    QCOMPARE(QString::fromAscii(displayProperty.value()), QString::fromAscii("Heido HH"));

    QVersitProperty nameProperty = document.properties().at(1);
    // Check parameters, contexts not allowed for N property
    QCOMPARE(nameProperty.parameters().count(), 0);
    // Check name
    QCOMPARE(nameProperty.name(), QString::fromAscii("N"));
    // Check value
    QCOMPARE(QString::fromAscii(nameProperty.value()), QString::fromAscii("HH;Heido;A;Mr.;"));

    // vCard 3.0, special characters in the name parts are backslash escaped
    contact.removeDetail(&name);
    name.setFirst(QString::fromAscii("Hom,er"));
    name.setLast(QString::fromAscii("Simp;son"));
    name.setMiddle(QString::fromAscii("J;"));
    name.setPrefix(QString::fromAscii(";Mr."));
    name.setSuffix(QString::fromAscii("Sir,"));
    contact.saveDetail(&name);
    document = mConverter->exportContact(contact,QVersitDocument::VCard30);
    QCOMPARE(document.versitType(),QVersitDocument::VCard30);

    // Each Contact has display label detail by default. Display label is enocded
    // if some value exisit for the Label or if value for Name exisit.
    QCOMPARE(document.properties().count(), 2);
    displayProperty = document.properties().at(0);
    nameProperty = document.properties().at(1);
    // Check parameters
    QCOMPARE(displayProperty.parameters().count(), 0);
    QCOMPARE(nameProperty.parameters().count(), 0);
    // Check name
    QCOMPARE(displayProperty.name(), QString::fromAscii("FN"));
    QCOMPARE(nameProperty.name(), QString::fromAscii("N"));
    // Check value

    QCOMPARE(QString::fromAscii(displayProperty.value()),
             QString::fromAscii("Hom\\,er Simp\\;son"));

    QCOMPARE(QString::fromAscii(nameProperty.value()),
             QString::fromAscii("Simp\\;son;Hom\\,er;J\\;;\\;Mr.;Sir\\,"));
}

void UT_QVersitContactExporter::testEncodePhoneNumber()
{
    QContact contact;
    QContactPhoneNumber phoneNumber;
    phoneNumber.setNumber(QString::fromAscii("12345678"));
    phoneNumber.setContexts(QContactDetail::ContextHome);
    phoneNumber.setSubTypes(QContactPhoneNumber::SubTypeMobile);
    contact.saveDetail(&phoneNumber);
    QVersitDocument document = mConverter->exportContact(contact);
    QCOMPARE(document.properties().count(), 1);
    QVersitProperty property = document.properties().at(0);
    // Check parameters
    QCOMPARE(property.parameters().count(), 2);
    QVERIFY(property.parameters().contains(
        QString::fromAscii("TYPE"),QString::fromAscii("HOME")));
    QVERIFY(property.parameters().contains(
        QString::fromAscii("TYPE"),QString::fromAscii("CELL")));
    // Check name
    QCOMPARE(property.name(), QString::fromAscii("TEL"));
    // Check value
    QCOMPARE(QString::fromAscii(property.value()), phoneNumber.number());
}

void UT_QVersitContactExporter::testEncodeEmailAddress()
{
    QContact contact;
    QContactEmailAddress email;
    email.setEmailAddress(QString::fromAscii("test@test"));
    email.setContexts(QContactDetail::ContextHome);
    contact.saveDetail(&email);
    QVersitDocument document = mConverter->exportContact(contact);
    QCOMPARE(document.properties().count(), 1);
    QVersitProperty property = document.properties().at(0);
    // Check parameters
    QCOMPARE(property.parameters().count(), 1);
    QVERIFY(property.parameters().contains(
        QString::fromAscii("TYPE"),QString::fromAscii("HOME")));
    // Check name
    QCOMPARE(property.name(), QString::fromAscii("EMAIL"));
    // Check value
    QCOMPARE(QString::fromAscii(property.value()), email.emailAddress());
}

void UT_QVersitContactExporter::testEncodeStreetAddress()
{
    QContact contact;
    QContactAddress address;

    // vCard 2.1
    address.setCountry(QString::fromAscii("Finland"));
    address.setPostcode(QString::fromAscii("00440"));
    address.setStreet(QString::fromAscii("HKKI 1X 90"));
    address.setLocality(QString::fromAscii("Helsinki"));
    address.setContexts(QContactDetail::ContextHome);
    address.setSubTypes(QContactAddress::SubTypePostal);
    contact.saveDetail(&address);
    QVersitDocument document = mConverter->exportContact(contact);
    QCOMPARE(document.properties().count(), 1);
    QVersitProperty property = document.properties().at(0);
    // Check parameters
    QCOMPARE(property.parameters().count(), 2);
    QVERIFY(property.parameters().contains(
        QString::fromAscii("TYPE"),QString::fromAscii("HOME")));
    QVERIFY(property.parameters().contains(
        QString::fromAscii("TYPE"),QString::fromAscii("POSTAL")));
    // Check name
    QCOMPARE(property.name(), QString::fromAscii("ADR"));
    // Check value
    QCOMPARE(QString::fromAscii(property.value()),
             QString::fromAscii(";;HKKI 1X 90;Helsinki;;00440;Finland"));

    // vCard 3.0, special characters in the address parts are backslash escaped
    contact.removeDetail(&address);
    address.setPostOfficeBox(QString::fromAscii("PO;Box"));
    address.setStreet(QString::fromAscii("My;Street"));
    address.setLocality(QString::fromAscii("My;Town"));
    address.setRegion(QString::fromAscii("My;State"));
    address.setPostcode(QString::fromAscii("12345;"));
    address.setCountry(QString::fromAscii("My;Country"));
    contact.saveDetail(&address);
    document = mConverter->exportContact(contact,QVersitDocument::VCard30);
    QCOMPARE(document.versitType(),QVersitDocument::VCard30);
    QCOMPARE(document.properties().count(), 1);
    property = document.properties().at(0);
    // Check name
    QCOMPARE(property.name(), QString::fromAscii("ADR"));
    // Check value
    QCOMPARE(QString::fromAscii(property.value()),
             QString::fromAscii("PO\\;Box;;My\\;Street;My\\;Town;My\\;State;12345\\;;My\\;Country"));
}

void UT_QVersitContactExporter::testEncodeUrl()
{
    QContact contact;
    QContactUrl url;
    url.setUrl(QString::fromAscii("http://www.myhome.com"));
    url.setContexts(QContactDetail::ContextHome);
    url.setSubType(QContactUrl::SubTypeHomePage);
    contact.saveDetail(&url);
    QVersitDocument document = mConverter->exportContact(contact);
    QCOMPARE(document.properties().count(), 1);
    QVersitProperty property = document.properties().at(0);
    // Check parameters
    QCOMPARE(property.parameters().count(), 1);
    QVERIFY(property.parameters().contains(
        QString::fromAscii("TYPE"),QString::fromAscii("HOME")));
    // Check name
    QCOMPARE(property.name(), QString::fromAscii("URL"));
    // Check value
    QCOMPARE(QString::fromAscii(property.value()), url.url());
}

void UT_QVersitContactExporter::testEncodeUid()
{
    QContact contact;
    QContactGuid guid;

    // vCard 2.1
    guid.setContexts(QContactDetail::ContextHome);
    guid.setGuid(QString::fromAscii("0101222"));
    contact.saveDetail(&guid);
    QVersitDocument document = mConverter->exportContact(contact);
    QCOMPARE(document.properties().count(), 1);
    QVersitProperty property = document.properties().at(0);
    // Check parameters
    // Contexts are not allowed for UID
    QCOMPARE(property.parameters().count(), 0);
    // Check name
    QCOMPARE(property.name(), QString::fromAscii("UID"));
    // Check value
    QCOMPARE(QString::fromAscii(property.value()), guid.guid());

    // vCard 3.0, special characters in the value are backslash escaped
    contact.removeDetail(&guid);
    guid.setGuid(QString::fromAscii("1;2,3\r\n4\\5"));
    contact.saveDetail(&guid);
    document = mConverter->exportContact(contact,QVersitDocument::VCard30);
    QCOMPARE(document.versitType(),QVersitDocument::VCard30);
    QCOMPARE(document.properties().count(), 1);
    property = document.properties().at(0);
    // Check name
    QCOMPARE(property.name(), QString::fromAscii("UID"));
    // Check value
    QCOMPARE(QString::fromAscii(property.value()),
             QString::fromAscii("1\\;2\\,3\\n4\\\\5"));
}

void UT_QVersitContactExporter::testEncodeRev()
{
    QContact contact;
    QContactTimestamp timeStamp;

    // Last modified time found
    QDateTime revisionTime =
        QDateTime::fromString(
            QString::fromAscii("M1d1y200906:01:02"),
            QString::fromAscii("'M'M'd'd'y'yyyyhh:mm:ss"));
    revisionTime.setTimeSpec(Qt::UTC);
    timeStamp.setLastModified(revisionTime);
    // Contexts not allowed in REV property, check that they are not added
    timeStamp.setContexts(QContactDetail::ContextHome);
    contact.saveDetail(&timeStamp);
    QVersitDocument document = mConverter->exportContact(contact);
    QCOMPARE(document.properties().count(), 1);
    QVersitProperty property = document.properties().at(0);
    QCOMPARE(property.parameters().count(), 0);
    QCOMPARE(property.name(), QString::fromAscii("REV"));
    QString expectedValueUTCEncoded = QString::fromAscii("2009-01-01T06:01:02Z");
    QCOMPARE(QString::fromAscii(property.value()), expectedValueUTCEncoded);

    // Last modified time not found, use the creation time
    QDateTime emptyTime;
    timeStamp.setLastModified(emptyTime);
    timeStamp.setCreated(revisionTime);
    contact.saveDetail(&timeStamp);
    document = mConverter->exportContact(contact);
    QCOMPARE(document.properties().count(), 1);
    property = document.properties().at(0);
    QCOMPARE(QString::fromAscii(property.value()), expectedValueUTCEncoded);

    // Last modified time found, Local Time spec not UTC
    QDateTime localTime;
    timeStamp.setLastModified(revisionTime);
    timeStamp.setCreated(localTime);
    revisionTime.setTimeSpec(Qt::LocalTime);
    contact.saveDetail(&timeStamp);
    document = mConverter->exportContact(contact);
    QCOMPARE(document.properties().count(), 1);
    property = document.properties().at(0);
    QString expectedValueEncoded = QString::fromAscii("2009-01-01T06:01:02");
    QCOMPARE(QString::fromAscii(property.value()), expectedValueUTCEncoded);

    // Last modified time not found, creation time not found
    timeStamp.setLastModified(emptyTime);
    timeStamp.setCreated(emptyTime);
    contact.saveDetail(&timeStamp);
    document = mConverter->exportContact(contact);
    QCOMPARE(document.properties().count(), 0);
}

void UT_QVersitContactExporter::testEncodeBirthDay()
{
    QContact contact;
    QDate date(2009,1,1);
    QContactBirthday birthDay;
    birthDay.setDate(date);
    // Contexts not allowed in BDAY property, check that they are not added
    birthDay.setContexts(QContactDetail::ContextHome);
    contact.saveDetail(&birthDay);
    QVersitDocument document = mConverter->exportContact(contact);
    QCOMPARE(document.properties().count(), 1);
    QVersitProperty property = document.properties().at(0);
    QCOMPARE(property.parameters().count(), 0);
    QCOMPARE(property.name(), QString::fromAscii("BDAY"));
    QCOMPARE(QString::fromAscii(property.value()), QString::fromAscii("2009-01-01"));
}

void UT_QVersitContactExporter::testEncodeNote()
{
    QContact contact;
    QContactNote note;
    note.setNote(QString::fromAscii("My Note"));
    // Contexts not allowed in NOTE property, check that they are not added
    note.setContexts(QContactDetail::ContextHome);
    contact.saveDetail(&note);
    QVersitDocument document = mConverter->exportContact(contact);
    QCOMPARE(document.properties().count(), 1);
    QVersitProperty property = document.properties().at(0);
    QCOMPARE(property.parameters().count(), 0);
    QCOMPARE(property.name(), QString::fromAscii("NOTE"));
    QCOMPARE(QString::fromAscii(property.value()), note.note());
}

void UT_QVersitContactExporter::testEncodeGeoLocation()
{
    QContact contact;
    QContactGeolocation geoLocation;
    QString longitude = QString::fromAscii("99.9");
    geoLocation.setLongitude(longitude.toDouble());
    QString latitude = QString::fromAscii("98.9");
    geoLocation.setLatitude(latitude.toDouble());
    // Contexts not allowed in GEO property, check that they are not added
    geoLocation.setContexts(QContactDetail::ContextHome);
    contact.saveDetail(&geoLocation);
    QVersitDocument document = mConverter->exportContact(contact);
    QCOMPARE(document.properties().count(), 1);
    QVersitProperty property = document.properties().at(0);
    QCOMPARE(property.parameters().count(), 0);
    QCOMPARE(property.name(), QString::fromAscii("GEO"));
    QString expectedValue = longitude + QString::fromAscii(",") + latitude;
    QCOMPARE(QString::fromAscii(property.value()), expectedValue);
}

void UT_QVersitContactExporter::testEncodeOrganization()
{
    QContact contact;
    QContactOrganization organization;
    QVersitDocument versitDocument;
    QVersitProperty property;
    QString title(QString::fromAscii("Developer"));
    QString organizationName(QString::fromAscii("Nokia"));
    QString department(QString::fromAscii("R&D"));

    // TITLE
    organization.setTitle(title);
    contact.saveDetail(&organization);
    versitDocument = mConverter->exportContact(contact);
    QCOMPARE(versitDocument.properties().count(), 1);
    property = versitDocument.properties().at(0);
    QCOMPARE(property.name(), QString::fromAscii("TITLE"));
    QCOMPARE(QString::fromAscii(property.value()), title);

    // ORG with name
    organization.setTitle(QString());
    organization.setName(QString::fromAscii("Nokia"));
    contact.saveDetail(&organization);
    versitDocument = mConverter->exportContact(contact);
    QCOMPARE(versitDocument.properties().count(), 1);
    property = versitDocument.properties().at(0);
    QCOMPARE(property.name(), QString::fromAscii("ORG"));
    QCOMPARE(QString::fromAscii(property.value()), QString::fromAscii("Nokia;"));

    // ORG with department/unit
    organization.setName(QString());
    organization.setDepartment(QString::fromAscii("R&D"));
    contact.saveDetail(&organization);
    versitDocument = mConverter->exportContact(contact);
    QCOMPARE(versitDocument.properties().count(), 1);
    property = versitDocument.properties().at(0);
    QCOMPARE(property.name(), QString::fromAscii("ORG"));
    QCOMPARE(QString::fromAscii(property.value()), QString::fromAscii(";R&D"));

    // ORG with name and department/unit
    organization.setName(QString::fromAscii("Nokia"));
    contact.saveDetail(&organization);
    versitDocument = mConverter->exportContact(contact);
    QCOMPARE(versitDocument.properties().count(), 1);
    property = versitDocument.properties().at(0);
    QCOMPARE(property.name(), QString::fromAscii("ORG"));
    QCOMPARE(QString::fromAscii(property.value()), QString::fromAscii("Nokia;R&D"));

    // TITLE and ORG
    organization.setTitle(QString::fromAscii("Developer"));
    contact.saveDetail(&organization);
    versitDocument = mConverter->exportContact(contact);
    QCOMPARE(versitDocument.properties().count(), 2);
    property = versitDocument.properties().at(0);
    QCOMPARE(property.name(), QString::fromAscii("TITLE"));
    QCOMPARE(QString::fromAscii(property.value()), title);
    property = versitDocument.properties().at(1);
    QCOMPARE(property.name(), QString::fromAscii("ORG"));
    QCOMPARE(QString::fromAscii(property.value()), QString::fromAscii("Nokia;R&D"));

    // ORG LOGO Test1: LOGO as remote Resouce
    mScaleSignalEmitted = false;

    const QString url = QString::fromAscii("http://www.myhome.com/test.jpg");
    contact = QContact();
    organization = QContactOrganization();
    organization.setLogo(url);
    contact.saveDetail(&organization);
    versitDocument = mConverter->exportContact(contact);
    QVERIFY(!mScaleSignalEmitted);

    //Media type, and source type are encoded.
    QCOMPARE(versitDocument.properties().at(0).parameters().count(), 2);

    QVERIFY(versitDocument.properties().at(0).parameters().contains(
            QString::fromAscii("TYPE"), QString::fromAscii("JPEG")));
    QVERIFY(versitDocument.properties().at(0).parameters().contains(
            QString::fromAscii("VALUE"), QString::fromAscii("URL")));

    //Check property Name
    QString propertyName = versitDocument.properties().at(0).name();
    QCOMPARE(propertyName, QString::fromAscii("LOGO"));

    //Check property value
    QString value = QString::fromAscii(versitDocument.properties().at(0).value().data() );
    QCOMPARE(value, url);


    // ORG LOGO Test2: LOGO File.
    contact = QContact();
    organization = QContactOrganization();
    organization.setLogo(mTestPhotoFile);
    contact.saveDetail(&organization);
    versitDocument = mConverter->exportContact(contact);
    QVERIFY(mScaleSignalEmitted);

    //Media type, source encoding is encoded i.e. base64
    QCOMPARE(versitDocument.properties().at(0).parameters().count(), 2);

    QVERIFY(versitDocument.properties().at(0).parameters().contains(
            QString::fromAscii("TYPE"), QString::fromAscii("JPEG")));

    QVERIFY(versitDocument.properties().at(0).parameters().contains(
            QString::fromAscii("ENCODING"), QString::fromAscii("BASE64")));

    //Ensure value1 is not URL
    QString value1 = QString::fromAscii(versitDocument.properties().at(0).value().data());
    QEXPECT_FAIL(value1.toAscii(), url.toAscii(), Continue);


    // Assistant Name Test.
    contact = QContact();
    organization = QContactOrganization();
    organization.setAssistantName(QString::fromAscii("myAssistant"));
    contact.saveDetail(&organization);
    versitDocument = mConverter->exportContact(contact);
    QCOMPARE(versitDocument.properties().count(), 1);
    property = versitDocument.properties().at(0);
    QCOMPARE(property.name(), QString::fromAscii("X-ASSISTANT"));
    QCOMPARE(QString::fromAscii(property.value()), QString::fromAscii("myAssistant"));

}

void UT_QVersitContactExporter::testEncodeAvatar()
{
    QContact contact;
    QContactAvatar contactAvatar;

    mScaleSignalEmitted = false;

    // Test1: Web URL
    const QString url = QString::fromAscii("http://www.myhome.com/test.jpg");
    contactAvatar.setAvatar(url);
    contactAvatar.setSubType(QContactAvatar::SubTypeImage);
    contact.saveDetail(&contactAvatar);
    QVersitDocument versitDocument = mConverter->exportContact(contact);
    QCOMPARE(versitDocument.properties().at(0).parameters().count(), 2);
    QVERIFY(!mScaleSignalEmitted);

    // Test 2: Local Media PHOTO
    contactAvatar.setAvatar(mTestPhotoFile);
    contactAvatar.setSubType(QContactAvatar::SubTypeImage);
    contact.saveDetail(&contactAvatar);
    versitDocument = mConverter->exportContact(contact);
    QVERIFY(mScaleSignalEmitted);

    //Media type, source encoding is encoded i.e. base64
    QCOMPARE(versitDocument.properties().at(0).parameters().count(), 2);

    // Test3: UnSupported Media Type, properties and parameters are not encoded
    mScaleSignalEmitted = false;
    const QString testUrl2 = QString::fromAscii("http://www.myhome.com/test.jpg");
    contactAvatar.setAvatar(testUrl2);
    // un-supported media type is encoded
    contactAvatar.setSubType(QContactAvatar::SubTypeTexturedMesh);
    contact.saveDetail(&contactAvatar);
    versitDocument = mConverter->exportContact(contact);
    QCOMPARE(versitDocument.properties().count(), 0);
    QVERIFY(!mScaleSignalEmitted);
}


void UT_QVersitContactExporter::testEncodeEmbeddedContent()
{
    QContact contact;
    QContactAvatar contactAvatar;

    // Test 1: URL
    const QString url = QString::fromAscii("http://www.myhome.com/test.jpg");
    contactAvatar.setAvatar(url);
    contactAvatar.setSubType(QContactAvatar::SubTypeImage);
    contact.saveDetail(&contactAvatar);
    QVersitDocument versitDocument = mConverter->exportContact(contact);
    QVERIFY(!mScaleSignalEmitted);
    QVersitProperty photoProperty = versitDocument.properties().at(0);
    QCOMPARE(photoProperty.parameters().count(), 2);
    QVERIFY(photoProperty.parameters().contains(
            QString::fromAscii("TYPE"),QString::fromAscii("JPEG")));
    QVERIFY(photoProperty.parameters().contains(
            QString::fromAscii("VALUE"),QString::fromAscii("URL")));
    QCOMPARE(photoProperty.name(), QString::fromAscii("PHOTO"));
    QCOMPARE(QString::fromAscii(photoProperty.value()), url);

    // Test 2: Local PHOTO, image not scaled
    contactAvatar.setAvatar(mTestPhotoFile);
    contactAvatar.setSubType(QContactAvatar::SubTypeImage);
    contact.saveDetail(&contactAvatar);
    versitDocument = mConverter->exportContact(contact);
    QVERIFY(mScaleSignalEmitted);
    photoProperty = versitDocument.properties().at(0);
    QCOMPARE(photoProperty.parameters().count(), 2);
    QVERIFY(photoProperty.parameters().contains(
        QString::fromAscii("TYPE"),
        QString::fromAscii("JPEG")));
    QVERIFY(photoProperty.parameters().contains(
        QString::fromAscii("ENCODING"),
        QString::fromAscii("BASE64")));
    QFile photoFile(mTestPhotoFile);
    photoFile.open(QIODevice::ReadOnly);
    QByteArray photoFileContent = photoFile.readAll();
    QCOMPARE(photoProperty.value(), photoFileContent.toBase64());

    // Test 3: Local PHOTO, image scaled by the "client"
    mScaleSignalEmitted = false;
    mSimulatedImageData = "simulated image data";
    versitDocument = mConverter->exportContact(contact);
    QVERIFY(mScaleSignalEmitted);
    photoProperty = versitDocument.properties().at(0);
    QCOMPARE(photoProperty.parameters().count(), 2);
    QVERIFY(photoProperty.parameters().contains(
        QString::fromAscii("TYPE"),
        QString::fromAscii("JPEG")));
    QVERIFY(photoProperty.parameters().contains(
        QString::fromAscii("ENCODING"),
        QString::fromAscii("BASE64")));
    QCOMPARE(photoProperty.value(), mSimulatedImageData.toBase64());

    // Test 5: Local SOUND
    mScaleSignalEmitted = false;
    contactAvatar.setAvatar(mTestAudioFile);
    contactAvatar.setSubType(QContactAvatar::SubTypeAudioRingtone);
    contact.saveDetail(&contactAvatar);
    versitDocument = mConverter->exportContact(contact);
    QVERIFY(!mScaleSignalEmitted);
    QVersitProperty soundProperty = versitDocument.properties().at(0);
    QCOMPARE(soundProperty.parameters().count(), 2);
    QVERIFY(soundProperty.parameters().contains(
        QString::fromAscii("TYPE"),
        QString::fromAscii("WAVE")));
    QVERIFY(soundProperty.parameters().contains(
        QString::fromAscii("ENCODING"),
        QString::fromAscii("BASE64")));
    QCOMPARE(soundProperty.value(), QByteArray());

    // Test 6: New media format will be encoded also
    const QString testUrl = QString::fromAscii("http://www.myhome.com/test.ggg");
    contactAvatar.setAvatar(testUrl);
    contactAvatar.setSubType(QContactAvatar::SubTypeImage);
    contact.saveDetail(&contactAvatar);
    versitDocument = mConverter->exportContact(contact);
    QVERIFY(!mScaleSignalEmitted);
    QCOMPARE(versitDocument.properties().count(), 1);
    QCOMPARE(versitDocument.properties().at(0).parameters().count(), 2);
    QVERIFY(versitDocument.properties().at(0).parameters().contains(
            QString::fromAscii("TYPE"), QString::fromAscii("GGG")));
    QVERIFY(versitDocument.properties().at(0).parameters().contains(
            QString::fromAscii("VALUE"),QString::fromAscii("URL")));

    // Test 7: Unsupported media type, properties and parameters are not encoded
    const QString testUrl2 = QString::fromAscii("http://www.myhome.com/test.jpg");
    contactAvatar.setAvatar(testUrl2);
    // un-supported media type is encoded
    contactAvatar.setSubType(QContactAvatar::SubTypeTexturedMesh);
    contact.saveDetail(&contactAvatar);
    versitDocument = mConverter->exportContact(contact);
    QCOMPARE(versitDocument.properties().count(), 0);
}

void UT_QVersitContactExporter::testEncodeParameters()
{
    QContact contact;
    QContactPhoneNumber phoneNumber;
    phoneNumber.setNumber(QString::fromAscii("12345678"));
    QStringList subtypes;
    subtypes.append(QContactPhoneNumber::SubTypeMobile);
    subtypes.append(QContactPhoneNumber::SubTypeVideo);
    // Add a not supported subtype in vCard, to make sure its not encoded.
    subtypes.append(QContactPhoneNumber::SubTypeDtmfMenu);
    phoneNumber.setSubTypes(subtypes);
    contact.saveDetail(&phoneNumber);
    QVersitDocument document = mConverter->exportContact(contact);
    QCOMPARE(document.properties().count(), 1);
    QVersitProperty property = document.properties().at(0);
    QCOMPARE(property.parameters().count(), 2);
    QVERIFY(property.parameters().contains(
        QString::fromAscii("TYPE"), QString::fromAscii("CELL")));
    QVERIFY(property.parameters().contains(
        QString::fromAscii("TYPE"),QString::fromAscii("VIDEO")));
}

void UT_QVersitContactExporter::testIsValidRemoteUrl()
{
    QContact contact;
    QContactAvatar contactAvatar;
    QVersitDocument versitDocument;

    // Test1: http URL
    QString url = QString::fromAscii("http://www.nonoh.com/test.jpg");
    contactAvatar.setAvatar(url);
    contactAvatar.setSubType(QContactAvatar::SubTypeImage);
    contact.saveDetail(&contactAvatar);
    versitDocument = mConverter->exportContact(contact);
    QCOMPARE(versitDocument.properties().count(), 1);

    // Test2: FTP URL
    url = QString::fromAscii("ftp://nonoh.com/test.jpg");
    contactAvatar.setAvatar(url);
    contactAvatar.setSubType(QContactAvatar::SubTypeImage);
    contact.saveDetail(&contactAvatar);
    versitDocument = mConverter->exportContact(contact);
    QCOMPARE(versitDocument.properties().count(), 1);

    // Test3: NEW Protocol URL
    url = QString::fromAscii("myProtocol://nonoh.com/test.jpg");
    contactAvatar.setAvatar(url);
    contactAvatar.setSubType(QContactAvatar::SubTypeImage);
    contact.saveDetail(&contactAvatar);
    versitDocument = mConverter->exportContact(contact);
    QCOMPARE(versitDocument.properties().count(), 1);

    // Test4: URL without scheme
    url = QString::fromAscii("www.nonoh.com/test.jpg");
    contactAvatar.setAvatar(url);
    contactAvatar.setSubType(QContactAvatar::SubTypeImage);
    contact.saveDetail(&contactAvatar);
    versitDocument = mConverter->exportContact(contact);
    QCOMPARE(versitDocument.properties().count(), 1);

    // Test5: File Name but File does not Exisit
    url = QString::fromAscii("c:/filedoesnotexisit.jok");
    contactAvatar.setAvatar(url);
    contactAvatar.setSubType(QContactAvatar::SubTypeImage);
    contact.saveDetail(&contactAvatar);
    versitDocument = mConverter->exportContact(contact);
    QCOMPARE(versitDocument.properties().count(), 0);
}

void UT_QVersitContactExporter::testEncodeGender()
{
    QContact contact;
    QContactGender gender;
    gender.setGender(QContactGender::GenderMale);
    gender.setContexts(QContactGender::ContextHome); // Should not be encoded
    contact.saveDetail(&gender);
    QVersitDocument document = mConverter->exportContact(contact);
    QCOMPARE(document.properties().count(), 1);
    QVersitProperty property = document.properties().at(0);
    QCOMPARE(property.parameters().count(), 0);
    QCOMPARE(property.name(), QString::fromAscii("X-GENDER"));
    QCOMPARE(QString::fromAscii(property.value()), gender.gender());
}

void UT_QVersitContactExporter::testEncodeNickName()
{
    QContact contact;
    QVersitDocument document;

    // Nickname not yet in the document
    QContactNickname nicknameDetail;
    QString firstNickname(QString::fromAscii("Homie"));
    nicknameDetail.setNickname(firstNickname);

    contact.saveDetail(&nicknameDetail);
    mConverterPrivate->exportContact(document,contact);

    QCOMPARE(document.properties().count(), 1);
    QVersitProperty property = document.properties().at(0);
    QCOMPARE(property.name(), QString::fromAscii("X-NICKNAME"));
    QCOMPARE(QString::fromAscii(property.value()), firstNickname);

    // Nickname already in the document, append to the existing property
    QString secondNickname(QString::fromAscii("Jay"));
    nicknameDetail.setNickname(secondNickname);

    contact.saveDetail(&nicknameDetail);
    mConverterPrivate->exportContact(document,contact);
    QCOMPARE(document.properties().count(), 1);
    property = document.properties().at(0);
    QCOMPARE(property.name(), QString::fromAscii("X-NICKNAME"));
    QCOMPARE(QString::fromAscii(property.value()), QString::fromAscii("Homie,Jay"));
}

void UT_QVersitContactExporter::testEncodeAnniversary()
{
    QContact contact;
    QContactAnniversary anniversary;
    QDate date(2009,1,1);
    anniversary.setOriginalDate(date);
    anniversary.setContexts(QContactDetail::ContextHome);
    anniversary.setSubType(QContactAnniversary::SubTypeWedding);
    contact.saveDetail(&anniversary);
    QVersitDocument document = mConverter->exportContact(contact);
    QCOMPARE(document.properties().count(), 1);
    QVersitProperty property = document.properties().at(0);
    // The contexts and subtypes are not defined for X-ANNIVERSARY property
    QCOMPARE(property.parameters().count(), 0);
    // Check name
    QCOMPARE(property.name(), QString::fromAscii("X-ANNIVERSARY"));
    // Check value
    QCOMPARE(QString::fromAscii(property.value()), date.toString(Qt::ISODate));
}


void UT_QVersitContactExporter::testEncodeOnlineAccount()
{
    QContact contact;
    QContactOnlineAccount onlineAccount;
    QString accountUri(QString::fromAscii("sip:abc@temp.com"));
    onlineAccount.setAccountUri(accountUri);

    // Video sharing
    onlineAccount.setSubTypes(QContactOnlineAccount::SubTypeShareVideo);
    onlineAccount.setContexts(QContactDetail::ContextHome);
    contact.saveDetail(&onlineAccount);
    QVersitDocument document = mConverter->exportContact(contact);
    QCOMPARE(document.properties().count(), 1);
    QVersitProperty property = document.properties().at(0);
    // Check parameters
    QCOMPARE(property.parameters().count(), 2);
    QVERIFY(property.parameters().contains(
        QString::fromAscii("TYPE"),QString::fromAscii("HOME")));
    QVERIFY(property.parameters().contains(
        QString::fromAscii("TYPE"),QString::fromAscii("SWIS")));
    // Check name
    QCOMPARE(property.name(), QString::fromAscii("X-SIP"));
    // Check value
    QCOMPARE(QString::fromAscii(property.value()), accountUri);

    // VoIP
    onlineAccount.setSubTypes(QContactOnlineAccount::SubTypeInternet);
    onlineAccount.setContexts(QContactDetail::ContextWork);
    contact.saveDetail(&onlineAccount);
    document = mConverter->exportContact(contact);
    QCOMPARE(document.properties().count(), 1);
    property = document.properties().at(0);
    // Check parameters
    QCOMPARE(property.parameters().count(), 2);
    QVERIFY(property.parameters().contains(
        QString::fromAscii("TYPE"),QString::fromAscii("WORK")));
    QVERIFY(property.parameters().contains(
        QString::fromAscii("TYPE"),QString::fromAscii("VOIP")));
    // Check name
    QCOMPARE(property.name(), QString::fromAscii("X-SIP"));
    // Check value
    QCOMPARE(QString::fromAscii(property.value()), accountUri);

    // Plain SIP
    onlineAccount.setSubTypes(QContactOnlineAccount::SubTypeSip);
    onlineAccount.setContexts(QContactDetail::ContextHome);
    contact.saveDetail(&onlineAccount);
    document = mConverter->exportContact(contact);
    QCOMPARE(document.properties().count(), 1);
    property = document.properties().at(0);
    // Check parameters, SIP not added as a TYPE parameter
    QCOMPARE(property.parameters().count(), 1);
    QVERIFY(property.parameters().contains(
        QString::fromAscii("TYPE"),QString::fromAscii("HOME")));
    // Check name
    QCOMPARE(property.name(), QString::fromAscii("X-SIP"));
    // Check value
    QCOMPARE(QString::fromAscii(property.value()), accountUri);

    // Other subtypes not converted
    onlineAccount.setSubTypes(QString::fromAscii("INVALIDSUBTYPE"));
    contact.saveDetail(&onlineAccount);
    document = mConverter->exportContact(contact);
    QCOMPARE(document.properties().count(), 0);
}

void UT_QVersitContactExporter::testEncodeFamily()
{
    QContact contact;
    QContactFamily family;

    // No spouse, no family
    family.setContexts(QContactDetail::ContextHome);
    contact.saveDetail(&family);
    QVersitDocument document = mConverter->exportContact(contact);
    QCOMPARE(document.properties().count(), 0);

    // Only spouse present
    QString spouce = QString::fromAscii("ABC");
    family.setSpouse(spouce);
    contact.saveDetail(&family);
    document = mConverter->exportContact(contact);
    QCOMPARE(document.properties().count(), 1);
    QVersitProperty spouseProperty = document.properties().at(0);
    QCOMPARE(spouseProperty.parameters().count(), 0);
    QCOMPARE(spouseProperty.name(), QString::fromAscii("X-SPOUSE"));
    QCOMPARE(QString::fromAscii(spouseProperty.value()), spouce);

    // Spouse and children
    QStringList children;
    children << QString::fromAscii("A") << QString::fromAscii("B") ;
    family.setChildren(children);
    family.setSpouse(spouce);
    contact.saveDetail(&family);
    document = mConverter->exportContact(contact);
    QCOMPARE(document.properties().count(), 2);
    spouseProperty = document.properties().at(0);
    QCOMPARE(spouseProperty.parameters().count(), 0);
    QCOMPARE(spouseProperty.name(), QString::fromAscii("X-SPOUSE"));
    QCOMPARE(QString::fromAscii(spouseProperty.value()), spouce);
    QVersitProperty childrenProperty = document.properties().at(1);
    QCOMPARE(childrenProperty.parameters().count(), 0);
    QCOMPARE(childrenProperty.name(), QString::fromAscii("X-CHILDREN"));
    QCOMPARE(QString::fromAscii(childrenProperty.value()), QString::fromAscii("A,B"));
}


void UT_QVersitContactExporter::testEncodeDisplayLabel()
{
    QContact contact;
    QContactDisplayLabel displayLaebl;
    QContactName contactName;

    //Test1: Display Label and Name does not Exisit
    QVersitDocument document = mConverter->exportContact(contact);
    QCOMPARE(document.properties().count(), 0);

    //Test2: Display Label does not Exisit but Name Exisit
    contactName.setFirst(QString::fromAscii("First"));
    contactName.setLast(QString::fromAscii("Last"));
    contactName.setMiddle(QString::fromAscii("Middle"));

    contact.saveDetail(&contactName);

    document = mConverter->exportContact(contact);
    QCOMPARE(document.properties().count(), 2);

    // Display Label Verify
    QVersitProperty displayProperty = document.properties().at(0);
    QCOMPARE(displayProperty.name(), QString::fromAscii("FN"));
    QCOMPARE(QString::fromAscii(displayProperty.value()), QString::fromAscii("First Last"));

    // Name Verify
    QVersitProperty nameProperty = document.properties().at(1);
    QCOMPARE(nameProperty.name(), QString::fromAscii("N"));
    QCOMPARE(QString::fromAscii(nameProperty.value()),
        QString::fromAscii("Last;First;Middle;;"));

    //Test3: Valid Display Lable and Name Exisit & test escaping
    contact = QContact();
    displayLaebl.setLabel(QString::fromAscii("Display,Label"));
    contact.saveDetail(&displayLaebl);
    contact.saveDetail(&contactName);
    document = mConverter->exportContact(contact, QVersitDocument::VCard30);

    displayProperty = document.properties().at(0);
    QCOMPARE(displayProperty.name(), QString::fromAscii("FN"));
    QCOMPARE(QString::fromAscii(displayProperty.value()), QString::fromAscii("Display\\,Label"));
}

// Test utility functions
QContactDetail UT_QVersitContactExporter::searchDetail(
    QList<QContactDetail> details,
    QString search)
{
    QContactDetail detail;
    for (int i= 0; i < details.count(); i++) {
        if ( details.at(i).definitionName() == search )
        detail = details.at(i);
    }
    return detail;
}
