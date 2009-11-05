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

#include "ut_qversitcontactconverter.h"
#include "qversitcontactconverter.h"
#include "qversitcontactconverter_p.h"
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


void UT_QVersitContactConverter::scale(
    const QString& imageFileName,
    QByteArray& imageData)
{
    QCOMPARE(imageFileName, mTestPhotoFile);
    imageData = mSimulatedImageData;
    mScaleSignalEmitted = true;
}

void UT_QVersitContactConverter::init()
{
    mConverter = new QVersitContactConverter();
    QObject::connect(
        mConverter, SIGNAL(scale(const QString&,QByteArray&)),
        this, SLOT(scale(const QString&,QByteArray&)));
    mConverterPrivate = new QVersitContactConverterPrivate();
    QVERIFY(!mConverterPrivate->mParameterMappings.empty());
    QVERIFY(!mConverterPrivate->mPropertyMappings.empty());
    mScaleSignalEmitted = false;
}

void UT_QVersitContactConverter::cleanup()
{
    mSimulatedImageData = QByteArray();
    mScaleSignalEmitted = false;
    delete mConverterPrivate;
    delete mConverter;
}

void UT_QVersitContactConverter::initTestCase()
{
    //create dummy file.
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

void UT_QVersitContactConverter::cleanupTestCase()
{
    QDir dir = QDir::current();
    dir.remove(mTestPhotoFile);
    dir.remove(mTestAudioFile);
}


void UT_QVersitContactConverter::testConvertContact()
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
    QVersitDocument document = mConverter->convertContact(contact);
    QCOMPARE(document.properties().count(), 2);
}
void UT_QVersitContactConverter::testUnconvertedContactDetails()
{
    // Test1: Un-supported Avatar Test
    QContact contact;
    QVersitDocument versitDocument;
    QContactAvatar contactAvatar;
    const QString url = QString::fromAscii("http://www.myhome.com/test.jpg");
    contactAvatar.setSubType(QContactAvatar::SubTypeTexturedMesh);
    contact.saveDetail(&contactAvatar);
    versitDocument = mConverter->convertContact(contact);
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
    versitDocument = mConverter->convertContact(contact);
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
    versitDocument = mConverter->convertContact(contact);
    unconvertedDetails = mConverter->unconvertedContactDetails();
    detail = searchDetail(unconvertedDetails,defintionName);
    QCOMPARE(defintionName, detail.definitionName());
}
void UT_QVersitContactConverter::testEncodeName()
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
    QVersitDocument document = mConverter->convertContact(contact);
    QCOMPARE(document.properties().count(), 1);
    QVersitProperty property = document.properties().at(0);
    // Check parameters, contexts not allowed for N property
    QCOMPARE(property.parameters().count(), 0);
    // Check name
    QCOMPARE(property.name(), QString::fromAscii(versitNameId));
    // Check value
    QCOMPARE(QString::fromAscii(property.value()), QString::fromAscii("HH;Heido;A;Mr.;"));

    // vCard 3.0, special characters in the name parts are backslash escaped
    contact.removeDetail(&name);
    name.setFirst(QString::fromAscii("Hom,er"));
    name.setLast(QString::fromAscii("Simp;son"));
    name.setMiddle(QString::fromAscii("J;"));
    name.setPrefix(QString::fromAscii(";Mr."));
    name.setSuffix(QString::fromAscii("Sir,"));
    contact.saveDetail(&name);
    document = mConverter->convertContact(contact,QVersitDocument::VCard30);
    QCOMPARE(document.versitType(),QVersitDocument::VCard30);
    QCOMPARE(document.properties().count(), 1);
    property = document.properties().at(0);
    // Check parameters
    QCOMPARE(property.parameters().count(), 0);
    // Check name
    QCOMPARE(property.name(), QString::fromAscii(versitNameId));
    // Check value
    QCOMPARE(QString::fromAscii(property.value()),
             QString::fromAscii("Simp\\;son;Hom\\,er;J\\;;\\;Mr.;Sir\\,"));
}

void UT_QVersitContactConverter::testEncodePhoneNumber()
{
    QContact contact;
    QContactPhoneNumber phoneNumber;
    phoneNumber.setNumber(QString::fromAscii("12345678"));
    phoneNumber.setContexts(QContactDetail::ContextHome);
    phoneNumber.setSubTypes(QContactPhoneNumber::SubTypeMobile);
    contact.saveDetail(&phoneNumber);
    QVersitDocument document = mConverter->convertContact(contact);
    QCOMPARE(document.properties().count(), 1);
    QVersitProperty property = document.properties().at(0);
    // Check parameters
    QCOMPARE(property.parameters().count(), 2);
    QVERIFY(property.parameters().contains(
        QString::fromAscii(versitType),QString::fromAscii(versitContextHomeId)));
    QVERIFY(property.parameters().contains(
        QString::fromAscii(versitType),QString::fromAscii(versitCellId)));
    // Check name
    QCOMPARE(property.name(), QString::fromAscii(versitPhoneId));
    // Check value
    QCOMPARE(QString::fromAscii(property.value()), phoneNumber.number());
}

void UT_QVersitContactConverter::testEncodeEmailAddress()
{
    QContact contact;
    QContactEmailAddress email;
    email.setEmailAddress(QString::fromAscii("test@test"));
    email.setContexts(QContactDetail::ContextHome);
    contact.saveDetail(&email);
    QVersitDocument document = mConverter->convertContact(contact);
    QCOMPARE(document.properties().count(), 1);
    QVersitProperty property = document.properties().at(0);
    // Check parameters
    QCOMPARE(property.parameters().count(), 1);
    QVERIFY(property.parameters().contains(
        QString::fromAscii(versitType),QString::fromAscii(versitContextHomeId)));
    // Check name
    QCOMPARE(property.name(), QString::fromAscii(versitEmailId));
    // Check value
    QCOMPARE(QString::fromAscii(property.value()), email.emailAddress());
}

void UT_QVersitContactConverter::testEncodeStreetAddress()
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
    QVersitDocument document = mConverter->convertContact(contact);
    QCOMPARE(document.properties().count(), 1);
    QVersitProperty property = document.properties().at(0);
    // Check parameters
    QCOMPARE(property.parameters().count(), 2);
    QVERIFY(property.parameters().contains(
        QString::fromAscii(versitType),QString::fromAscii(versitContextHomeId)));
    QVERIFY(property.parameters().contains(
        QString::fromAscii(versitType),QString::fromAscii(versitPostalId)));
    // Check name
    QCOMPARE(property.name(), QString::fromAscii(versitAddressId));
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
    document = mConverter->convertContact(contact,QVersitDocument::VCard30);
    QCOMPARE(document.versitType(),QVersitDocument::VCard30);
    QCOMPARE(document.properties().count(), 1);
    property = document.properties().at(0);
    // Check name
    QCOMPARE(property.name(), QString::fromAscii(versitAddressId));
    // Check value
    QCOMPARE(QString::fromAscii(property.value()),
             QString::fromAscii("PO\\;Box;;My\\;Street;My\\;Town;My\\;State;12345\\;;My\\;Country"));
}

void UT_QVersitContactConverter::testEncodeUrl()
{
    QContact contact;
    QContactUrl url;
    url.setUrl(QString::fromAscii("http://www.myhome.com"));
    url.setContexts(QContactDetail::ContextHome);
    url.setSubType(QContactUrl::SubTypeHomePage);
    contact.saveDetail(&url);
    QVersitDocument document = mConverter->convertContact(contact);
    QCOMPARE(document.properties().count(), 1);
    QVersitProperty property = document.properties().at(0);
    // Check parameters
    QCOMPARE(property.parameters().count(), 1);
    QVERIFY(property.parameters().contains(
        QString::fromAscii(versitType),QString::fromAscii(versitContextHomeId)));
    // Check name
    QCOMPARE(property.name(), QString::fromAscii(versitUrlId));
    // Check value
    QCOMPARE(QString::fromAscii(property.value()), url.url());
}

void UT_QVersitContactConverter::testEncodeUid()
{
    QContact contact;
    QContactGuid guid;

    // vCard 2.1
    guid.setContexts(QContactDetail::ContextHome);
    guid.setGuid(QString::fromAscii("0101222"));
    contact.saveDetail(&guid);
    QVersitDocument document = mConverter->convertContact(contact);
    QCOMPARE(document.properties().count(), 1);
    QVersitProperty property = document.properties().at(0);
    // Check parameters
    // Contexts are not allowed for UID
    QCOMPARE(property.parameters().count(), 0);
    // Check name
    QCOMPARE(property.name(), QString::fromAscii(versitUidId));
    // Check value
    QCOMPARE(QString::fromAscii(property.value()), guid.guid());

    // vCard 3.0, special characters in the value are backslash escaped
    contact.removeDetail(&guid);
    guid.setGuid(QString::fromAscii("1;2,3\r\n4\\5"));
    contact.saveDetail(&guid);
    document = mConverter->convertContact(contact,QVersitDocument::VCard30);
    QCOMPARE(document.versitType(),QVersitDocument::VCard30);
    QCOMPARE(document.properties().count(), 1);
    property = document.properties().at(0);
    // Check name
    QCOMPARE(property.name(), QString::fromAscii(versitUidId));
    // Check value
    QCOMPARE(QString::fromAscii(property.value()),
             QString::fromAscii("1\\;2\\,3\\n4\\\\5"));
}

void UT_QVersitContactConverter::testEncodeRev()
{
    QContact contact;
    QContactTimestamp p;
    QDateTime changeRev = QDateTime::fromString(QString::fromAscii("M1d1y200906:01:02"),
                                                QString::fromAscii("'M'M'd'd'y'yyyyhh:mm:ss"));
    changeRev.setTimeSpec(Qt::UTC);

    p.setLastModified(changeRev);

    //API Permits setting up context but it should not be in Versit Doc
    p.setContexts(QContactDetail::ContextHome);

    contact.saveDetail(&p);

    //Convert Contat Into Versit Document
    QVersitDocument versitDocument = mConverter->convertContact(contact);

    //Ensure parameters does not exisit
    QCOMPARE(versitDocument.properties().at(0).parameters().count(), 0);

    //Ensure property Exisit
    QCOMPARE(versitDocument.properties().count(), 1);

    //Ensure property parameer exisit and matches.
    QString propertyName = versitDocument.properties().at(0).name();
    QString expectedPropertyName =
        mConverterPrivate->mPropertyMappings.value(QContactTimestamp::DefinitionName);
    QCOMPARE(propertyName, expectedPropertyName);

    //Check property value
    QString value = QString::fromAscii(versitDocument.properties().at(0).value().data());
    QString expValueUTCEncoded = QString::fromAscii("2009-01-01T06:01:02Z");
    QCOMPARE(value, expValueUTCEncoded);


    // Test 2: If Modified Data does not Exist than date of contact created will be used.
    QDateTime emptyTime;
    p.setLastModified(emptyTime);
    p.setCreated(changeRev);
    contact.saveDetail(&p);

    versitDocument = mConverter->convertContact(contact);
    value = QString::fromAscii(versitDocument.properties().at(0).value());

    // Ensure Value exisit and matches.
    QCOMPARE(value, expValueUTCEncoded);


    // Test 3: Modified Date but Local Time spec not UTC
    QDateTime localTime;
    p.setLastModified(changeRev);
    p.setCreated(localTime);
    changeRev.setTimeSpec(Qt::LocalTime);
    contact.saveDetail(&p);

    versitDocument = mConverter->convertContact(contact);
    value = QString::fromAscii(versitDocument.properties().at(0).value());

    // Ensure Value exisit and matches.
    QString expValueEncoded = QString::fromAscii("2009-01-01T06:01:02");
    QCOMPARE(value, expValueUTCEncoded);


    // Test 4: Empty Date Entry No Created / Modified Date.
    p.setLastModified(emptyTime);
    p.setCreated(emptyTime);
    changeRev.setTimeSpec(Qt::LocalTime);
    contact.saveDetail(&p);

    versitDocument = mConverter->convertContact(contact);
    QCOMPARE(versitDocument.properties().count(), 0);
}

void UT_QVersitContactConverter::testEncodeBirthDay()
{
    QContact contact;
    QDate date(2009,1,1);
    QContactBirthday birthDay;

    birthDay.setDate(date);

    //API Permits setting up context but it should not be in Versit Doc
    birthDay.setContexts(QContactDetail::ContextHome);

    contact.saveDetail(&birthDay);

    //Convert Contat Into Versit Document
    QVersitDocument versitDocument = mConverter->convertContact(contact);

    //Ensure parameters does not exisit
    QCOMPARE(versitDocument.properties().at(0).parameters().count(), 0);

    //Ensure property Exisit
    QCOMPARE(versitDocument.properties().count(), 1);

    //Ensure property parameer exisit and matches.
    QString propertyName = versitDocument.properties().at(0).name();
    QString expectedPropertyName =
        mConverterPrivate->mPropertyMappings.value(QContactBirthday::DefinitionName);
    QCOMPARE(propertyName, expectedPropertyName);

    //Check property value
    QString value = QString::fromAscii(versitDocument.properties().at(0).value().data());
    QString expectedValue = QString::fromAscii("2009-01-01");
    QCOMPARE(value, expectedValue);
}

void UT_QVersitContactConverter::testEncodeNote()
{
    QContact contact;
    QString myNote = QString::fromAscii("My Note");
    QContactNote note;
    note.setNote(myNote);

    //API Permits setting up context but it should not be in Versit Doc
    note.setContexts(QContactDetail::ContextHome);

    contact.saveDetail(&note);

    //Convert Contat Into Versit Document
    QVersitDocument versitDocument = mConverter->convertContact(contact);

    //Ensure parameters does not exisit
    QCOMPARE(versitDocument.properties().at(0).parameters().count(), 0);

    //Ensure property Exisit
    QCOMPARE(versitDocument.properties().count(), 1);

    //Ensure property parameer exisit and matches.
    QString propertyName = versitDocument.properties().at(0).name();
    QString expectedPropertyName =
        mConverterPrivate->mPropertyMappings.value(QContactNote::DefinitionName);
    QCOMPARE(propertyName, expectedPropertyName);

    //Check property value
    QString value = QString::fromAscii(versitDocument.properties().at(0).value().data());
    QCOMPARE(value, myNote);
}

void UT_QVersitContactConverter::testEncodeGeoLocation()
{
    QContact contact;
    QContactGeolocation geoLocation;

    QString longitude = QString::fromAscii("99.9");
    QString latitude = QString::fromAscii("98.9");
    QString accuracy = QString::fromAscii("90.7");

    geoLocation.setLongitude(longitude.toDouble());

    geoLocation.setLatitude(latitude.toDouble());

    // API Permit setting accuracy and other parameters which should be encoded.
    geoLocation.setAccuracy(accuracy.toDouble());

    //API Permits setting up context but it should not be in Versit Doc
    geoLocation.setContexts(QContactDetail::ContextHome);

    contact.saveDetail(&geoLocation);

    //Convert Contat Into Versit Document
    QVersitDocument versitDocument = mConverter->convertContact(contact);

    //Ensure parameters does not exisit
    QCOMPARE(versitDocument.properties().at(0).parameters().count(), 0);

    //Ensure property Exisit
    QCOMPARE(versitDocument.properties().count(), 1);

    //Ensure property parameer exisit and matches.
    QString propertyName = versitDocument.properties().at(0).name();
    QString expectedPropertyName =
        mConverterPrivate->mPropertyMappings.value(QContactGeolocation::DefinitionName);
    QCOMPARE(propertyName, expectedPropertyName);

    //Check property value
    QString value = QString::fromAscii(versitDocument.properties().at(0).value().data() );
    QString expectedValue = longitude + QString::fromAscii(",") + latitude;
    QCOMPARE(value, expectedValue);
}

void UT_QVersitContactConverter::testEncodeOrganization()
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
    versitDocument = mConverter->convertContact(contact);
    QCOMPARE(versitDocument.properties().count(), 1);
    property = versitDocument.properties().at(0);
    QCOMPARE(property.name(), QString::fromAscii(versitTitleId));
    QCOMPARE(QString::fromAscii(property.value()), title);

    // ORG with name
    organization.setTitle(QString());
    organization.setName(QString::fromAscii("Nokia"));
    contact.saveDetail(&organization);
    versitDocument = mConverter->convertContact(contact);
    QCOMPARE(versitDocument.properties().count(), 1);
    property = versitDocument.properties().at(0);
    QCOMPARE(property.name(), QString::fromAscii(versitOrganizationId));
    QCOMPARE(QString::fromAscii(property.value()), QString::fromAscii("Nokia;"));

    // ORG with department/unit
    organization.setName(QString());
    organization.setDepartment(QString::fromAscii("R&D"));
    contact.saveDetail(&organization);
    versitDocument = mConverter->convertContact(contact);
    QCOMPARE(versitDocument.properties().count(), 1);
    property = versitDocument.properties().at(0);
    QCOMPARE(property.name(), QString::fromAscii(versitOrganizationId));
    QCOMPARE(QString::fromAscii(property.value()), QString::fromAscii(";R&D"));

    // ORG with name and department/unit
    organization.setName(QString::fromAscii("Nokia"));
    contact.saveDetail(&organization);
    versitDocument = mConverter->convertContact(contact);
    QCOMPARE(versitDocument.properties().count(), 1);
    property = versitDocument.properties().at(0);
    QCOMPARE(property.name(), QString::fromAscii(versitOrganizationId));
    QCOMPARE(QString::fromAscii(property.value()), QString::fromAscii("Nokia;R&D"));

    // TITLE and ORG
    organization.setTitle(QString::fromAscii("Developer"));
    contact.saveDetail(&organization);
    versitDocument = mConverter->convertContact(contact);
    QCOMPARE(versitDocument.properties().count(), 2);
    property = versitDocument.properties().at(0);
    QCOMPARE(property.name(), QString::fromAscii(versitTitleId));
    QCOMPARE(QString::fromAscii(property.value()), title);
    property = versitDocument.properties().at(1);
    QCOMPARE(property.name(), QString::fromAscii(versitOrganizationId));
    QCOMPARE(QString::fromAscii(property.value()), QString::fromAscii("Nokia;R&D"));

    // ORG LOGO Test1: LOGO as remote Resouce
    mScaleSignalEmitted = false;

    const QString url = QString::fromAscii("http://www.myhome.com/test.jpg");
    contact = QContact();
    organization = QContactOrganization();
    organization.setLogo(url);
    contact.saveDetail(&organization);
    versitDocument = mConverter->convertContact(contact);
    QVERIFY(!mScaleSignalEmitted);

    //Media type, and source type are encoded.
    QCOMPARE(versitDocument.properties().at(0).parameters().count(), 2);

    QVERIFY(versitDocument.properties().at(0).parameters().contains(
            QString::fromAscii(versitType), QString::fromAscii(versitFormatJpeg)));
    QVERIFY(versitDocument.properties().at(0).parameters().contains(
            QString::fromAscii(versitValue), QString::fromAscii(versitUrlId)));

    //Check property Name
    QString propertyName = versitDocument.properties().at(0).name();
    QCOMPARE(propertyName, QString::fromAscii(versitLogoId));

    //Check property value
    QString value = QString::fromAscii(versitDocument.properties().at(0).value().data() );
    QCOMPARE(value, url);


    // ORG LOGO Test2: LOGO File.
    contact = QContact();
    organization = QContactOrganization();
    organization.setLogo(mTestPhotoFile);
    contact.saveDetail(&organization);
    versitDocument = mConverter->convertContact(contact);
    QVERIFY(mScaleSignalEmitted);

    //Media type, source encoding is encoded i.e. base64
    QCOMPARE(versitDocument.properties().at(0).parameters().count(), 2);

    QVERIFY(versitDocument.properties().at(0).parameters().contains(
            QString::fromAscii(versitType), QString::fromAscii(versitFormatJpeg)));

    QVERIFY(versitDocument.properties().at(0).parameters().contains(
            QString::fromAscii(versitEncoding), QString::fromAscii(versitEncodingBase64)));

    //Ensure value1 is not URL
    QString value1 = QString::fromAscii(versitDocument.properties().at(0).value().data());
    QEXPECT_FAIL(value1.toAscii(), url.toAscii(), Continue);


    // Assistant Name Test.
    contact = QContact();
    organization = QContactOrganization();
    organization.setAssistantName(QString::fromAscii("myAssistant"));
    contact.saveDetail(&organization);
    versitDocument = mConverter->convertContact(contact);
    QCOMPARE(versitDocument.properties().count(), 1);
    property = versitDocument.properties().at(0);
    QCOMPARE(property.name(), QString::fromAscii(versitAssistantId));
    QCOMPARE(QString::fromAscii(property.value()), QString::fromAscii("myAssistant"));

}

void UT_QVersitContactConverter::testEncodeAvatar()
{
    QContact contact;
    QContactAvatar contactAvatar;

    mScaleSignalEmitted = false;

    // Test1: Web URL
    const QString url = QString::fromAscii("http://www.myhome.com/test.jpg");
    contactAvatar.setAvatar(url);
    contactAvatar.setSubType(QContactAvatar::SubTypeImage);
    contact.saveDetail(&contactAvatar);
    QVersitDocument versitDocument = mConverter->convertContact(contact);
    QCOMPARE(versitDocument.properties().at(0).parameters().count(), 2);
    QVERIFY(!mScaleSignalEmitted);

    // Test 2: Local Media PHOTO
    contactAvatar.setAvatar(mTestPhotoFile);
    contactAvatar.setSubType(QContactAvatar::SubTypeImage);
    contact.saveDetail(&contactAvatar);
    versitDocument = mConverter->convertContact(contact);
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
    versitDocument = mConverter->convertContact(contact);
    QCOMPARE(versitDocument.properties().count(), 0);
    QVERIFY(!mScaleSignalEmitted);
}


void UT_QVersitContactConverter::testEncodeEmbeddedContent()
{
    QContact contact;
    QContactAvatar contactAvatar;

    // Test 1: URL
    const QString url = QString::fromAscii("http://www.myhome.com/test.jpg");
    contactAvatar.setAvatar(url);
    contactAvatar.setSubType(QContactAvatar::SubTypeImage);
    contact.saveDetail(&contactAvatar);
    QVersitDocument versitDocument = mConverter->convertContact(contact);
    QVERIFY(!mScaleSignalEmitted);
    QVersitProperty photoProperty = versitDocument.properties().at(0);
    QCOMPARE(photoProperty.parameters().count(), 2);
    QVERIFY(photoProperty.parameters().contains(
            QString::fromAscii(versitType),QString::fromAscii(versitFormatJpeg)));
    QVERIFY(photoProperty.parameters().contains(
            QString::fromAscii(versitValue),QString::fromAscii(versitUrlId)));
    QString expectedPropertyName =
        mConverterPrivate->mPropertyMappings.value(QContactAvatar::SubTypeImage);
    QCOMPARE(photoProperty.name(), expectedPropertyName);
    QCOMPARE(QString::fromAscii(photoProperty.value()), url);

    // Test 2: Local Media PHOTO, image not scaled
    contactAvatar.setAvatar(mTestPhotoFile);
    contactAvatar.setSubType(QContactAvatar::SubTypeImage);
    contact.saveDetail(&contactAvatar);
    versitDocument = mConverter->convertContact(contact);
    QVERIFY(mScaleSignalEmitted);
    photoProperty = versitDocument.properties().at(0);
    QCOMPARE(photoProperty.parameters().count(), 2);
    QVERIFY(photoProperty.parameters().contains(
        QString::fromAscii(versitType),
        QString::fromAscii(versitFormatJpeg)));
    QVERIFY(photoProperty.parameters().contains(
        QString::fromAscii(versitEncoding),
        QString::fromAscii(versitEncodingBase64)));
    QFile photoFile(mTestPhotoFile);
    photoFile.open(QIODevice::ReadOnly);
    QByteArray photoFileContent = photoFile.readAll();
    QCOMPARE(photoProperty.value(), photoFileContent.toBase64());

    // Test 3: Local Media PHOTO, image scaled by the "client"
    mScaleSignalEmitted = false;
    mSimulatedImageData = "simulated image data";
    versitDocument = mConverter->convertContact(contact);
    QVERIFY(mScaleSignalEmitted);
    photoProperty = versitDocument.properties().at(0);
    QCOMPARE(photoProperty.parameters().count(), 2);
    QVERIFY(photoProperty.parameters().contains(
        QString::fromAscii(versitType),
        QString::fromAscii(versitFormatJpeg)));
    QVERIFY(photoProperty.parameters().contains(
        QString::fromAscii(versitEncoding),
        QString::fromAscii(versitEncodingBase64)));
    QCOMPARE(photoProperty.value(), mSimulatedImageData.toBase64());

    // Test 5: Local Media SOUND
    mScaleSignalEmitted = false;
    contactAvatar.setAvatar(mTestAudioFile);
    contactAvatar.setSubType(QContactAvatar::SubTypeAudioRingtone);
    contact.saveDetail(&contactAvatar);
    versitDocument = mConverter->convertContact(contact);
    QVERIFY(!mScaleSignalEmitted);
    QVersitProperty soundProperty = versitDocument.properties().at(0);
    QCOMPARE(soundProperty.parameters().count(), 2);
    QVERIFY(soundProperty.parameters().contains(
        QString::fromAscii(versitType),
        QString::fromAscii(versitFormatWave)));
    QVERIFY(soundProperty.parameters().contains(
        QString::fromAscii(versitEncoding),
        QString::fromAscii(versitEncodingBase64)));
    QCOMPARE(soundProperty.value(), QByteArray());

    // Test 6: New Media Format will be encoded also
    const QString testUrl = QString::fromAscii("http://www.myhome.com/test.ggg");
    contactAvatar.setAvatar(testUrl);
    contactAvatar.setSubType(QContactAvatar::SubTypeImage);
    contact.saveDetail(&contactAvatar);
    versitDocument = mConverter->convertContact(contact);
    QVERIFY(!mScaleSignalEmitted);
    QCOMPARE(versitDocument.properties().count(), 1);
    QCOMPARE(versitDocument.properties().at(0).parameters().count(), 2);
    QVERIFY(versitDocument.properties().at(0).parameters().contains(
            QString::fromAscii(versitType), QString::fromAscii("GGG")));
    QVERIFY(versitDocument.properties().at(0).parameters().contains(
            QString::fromAscii(versitValue),QString::fromAscii(versitUrlId)));

    // Test 7: UnSupported Media Type, properties and parameters are not encoded
    const QString testUrl2 = QString::fromAscii("http://www.myhome.com/test.jpg");
    contactAvatar.setAvatar(testUrl2);
    // un-supported media type is encoded
    contactAvatar.setSubType(QContactAvatar::SubTypeTexturedMesh);
    contact.saveDetail(&contactAvatar);
    versitDocument = mConverter->convertContact(contact);
    QCOMPARE(versitDocument.properties().count(), 0);
}

void UT_QVersitContactConverter::testEncodeParameters()
{
    QContact contact;

    // Adding Phone Numer for the Contact.
    QContactPhoneNumber p;
    p.setNumber(QString::fromAscii("12345678"));

    // Set Supported Types
    QStringList mysubtypes;

    mysubtypes.append(QContactPhoneNumber::SubTypeMobile);
    mysubtypes.append(QContactPhoneNumber::SubTypeVideo);

    // Not supported sub type in versit specs
    // ensure its not encoded.
    mysubtypes.append(QContactPhoneNumber::SubTypeDtmfMenu);

    p.setSubTypes(mysubtypes);

    contact.saveDetail(&p);

    //Convert Contat Into Versit Document
    QVersitDocument versitDocument = mConverter->convertContact(contact);

    //Ensure property Exisit
    QCOMPARE(versitDocument.properties().count(), 1);

    //Ensure Only Valid params are encoded
    QCOMPARE(versitDocument.properties().at(0).parameters().count(), 2);

    //Ensure Valid parameters exisit
    QVERIFY(versitDocument.properties().at(0).parameters().contains(
            QString::fromAscii(versitType), QString::fromAscii(versitCellId)));
    QVERIFY(versitDocument.properties().at(0).parameters().contains(
            QString::fromAscii(versitType),QString::fromAscii(versitVideoId)));
}

void UT_QVersitContactConverter::testIsValidRemoteUrl()
{
    QContact contact;
    QContactAvatar contactAvatar;
    QVersitDocument versitDocument;

    // Test1: http URL
    QString url = QString::fromAscii("http://www.nonoh.com/test.jpg");
    contactAvatar.setAvatar(url);
    contactAvatar.setSubType(QContactAvatar::SubTypeImage);
    contact.saveDetail(&contactAvatar);
    versitDocument = mConverter->convertContact(contact);
    QCOMPARE(versitDocument.properties().count(), 1);


    // Test2: FTP URL
    url = QString::fromAscii("ftp://nonoh.com/test.jpg");
    contactAvatar.setAvatar(url);
    contactAvatar.setSubType(QContactAvatar::SubTypeImage);
    contact.saveDetail(&contactAvatar);
    versitDocument = mConverter->convertContact(contact);
    QCOMPARE(versitDocument.properties().count(), 1);


    // Test3: NEW Protocol URL
    url = QString::fromAscii("myProtocol://nonoh.com/test.jpg");
    contactAvatar.setAvatar(url);
    contactAvatar.setSubType(QContactAvatar::SubTypeImage);
    contact.saveDetail(&contactAvatar);
    versitDocument = mConverter->convertContact(contact);
    QCOMPARE(versitDocument.properties().count(), 1);


    // Test4: URL without schem
    url = QString::fromAscii("www.nonoh.com/test.jpg");
    contactAvatar.setAvatar(url);
    contactAvatar.setSubType(QContactAvatar::SubTypeImage);
    contact.saveDetail(&contactAvatar);
    versitDocument = mConverter->convertContact(contact);
    QCOMPARE(versitDocument.properties().count(), 1);

    // Test5: File Name but File does not Exisit
    url = QString::fromAscii("c:/filedoesnotexisit.jok");
    contactAvatar.setAvatar(url);
    contactAvatar.setSubType(QContactAvatar::SubTypeImage);
    contact.saveDetail(&contactAvatar);
    versitDocument = mConverter->convertContact(contact);
    QCOMPARE(versitDocument.properties().count(), 0);
}


void UT_QVersitContactConverter::testEncodeGender()
{
    QContact contact;
    QContactGender gender;
    gender.setGender(QContactGender::GenderMale);

    //API Permits setting up context but it should not be in Versit Doc
    gender.setContexts(QContactGender::ContextHome);

    contact.saveDetail(&gender);

    //Convert Contat Into Versit Document
    QVersitDocument versitDocument = mConverter->convertContact(contact);

    //Ensure parameters does not exisit
    QCOMPARE(versitDocument.properties().at(0).parameters().count(), 0);

    //Ensure property Exisit
    QCOMPARE(versitDocument.properties().count(), 1);

    //Ensure property parameer exisit and matches.
    QString propertyName = versitDocument.properties().at(0).name();
    QString expectedPropertyName =
        mConverterPrivate->mPropertyMappings.value(QContactGender::DefinitionName);
    QCOMPARE(propertyName, expectedPropertyName);

    //Check property value
    QString value = QString::fromAscii(versitDocument.properties().at(0).value().data());
    QCOMPARE(value, QString(QLatin1String(QContactGender::GenderMale)));
}

void UT_QVersitContactConverter::testEncodeNickName()
{
    QVersitDocument document;

    // Nickname not yet in the document
    QContactNickname nicknameDetail;
    QString firstNickname(QString::fromAscii("Homie"));
    nicknameDetail.setNickname(firstNickname);
    mConverterPrivate->encodeFieldInfo(document,nicknameDetail);
    QCOMPARE(document.properties().count(), 1);
    QVersitProperty property = document.properties().at(0);
    QCOMPARE(property.name(), QString::fromAscii(versitNicknameXId));
    QCOMPARE(QString::fromAscii(property.value()), firstNickname);

    // Nickname already in the document, append to the existing property
    QString secondNickname(QString::fromAscii("Jay"));
    nicknameDetail.setNickname(secondNickname);
    mConverterPrivate->encodeFieldInfo(document,nicknameDetail);
    QCOMPARE(document.properties().count(), 1);
    property = document.properties().at(0);
    QCOMPARE(property.name(), QString::fromAscii(versitNicknameXId));
    QCOMPARE(QString::fromAscii(property.value()), QString::fromAscii("Homie,Jay"));
}

void UT_QVersitContactConverter::testEncodeAnniversary()
{
    QContact contact;
    QContactAnniversary anniversary;
    QDate date(2009,1,1);
    anniversary.setOriginalDate(date);

    //API Permits setting up context but it should not be in Versit Doc
    anniversary.setContexts(QContactDetail::ContextHome);

    // Set Sub Types for the Aniversary
    anniversary.setSubType(QContactAnniversary::SubTypeWedding);

    // Save Contact Detail
    contact.saveDetail(&anniversary);

    //Convert Contat Into Versit Document
    QVersitDocument versitDocument = mConverter->convertContact(contact);

    //Ensure parameters only one parameter exisit
    QCOMPARE(versitDocument.properties().at(0).parameters().count(), 1);

    //Ensure property Exisit
    QCOMPARE(versitDocument.properties().count(), 1);

    //Ensure property parameer exisit and matches.
    QString propertyName = versitDocument.properties().at(0).name();
    QString expectedPropertyName =
        mConverterPrivate->mPropertyMappings.value(QContactAnniversary::DefinitionName);
    QCOMPARE(propertyName, expectedPropertyName);

    //Check property value
    QString expectedValue = QString::fromAscii("2009-01-01");
    QString value = QString::fromAscii(versitDocument.properties().at(0).value().data());
    QCOMPARE(value, expectedValue);

    // Ensure Sub types matches.
    QVERIFY(versitDocument.properties().at(0).parameters().contains(QString::fromAscii(versitType),
            QString(QLatin1String(QContactAnniversary::SubTypeWedding)).toUpper()));
}


void UT_QVersitContactConverter::testEncodeOnlineAccount()
{
    QContact contact;
    QContactOnlineAccount onlineAccount;

    //Test1:  Valid SWIS Account.
    QString testUri = QString::fromAscii("sip:abc@temp.com");
    onlineAccount.setAccountUri(testUri);
    onlineAccount.setSubTypes(QContactOnlineAccount::SubTypeShareVideo);
    onlineAccount.setContexts(QContactDetail::ContextHome);
    contact.saveDetail(&onlineAccount);
    
    //Convert Contat Into Versit Document
    QVersitDocument versitDocument = mConverter->convertContact(contact);

    //Ensure parameters exisit, i.e. home and swiss
    QCOMPARE(versitDocument.properties().at(0).parameters().count(), 2);
    QVERIFY(versitDocument.properties().at(0).parameters().contains(
            QString::fromAscii(versitType), QString::fromAscii(versitContextHomeId)));
    QVERIFY(versitDocument.properties().at(0).parameters().contains(
            QString::fromAscii(versitType), QString::fromAscii(versitSwisId)));

    //Ensure property Exisit
    QCOMPARE(versitDocument.properties().count(), 1);

    //Ensure property parameer exisit and matches.
    QString propertyName = versitDocument.properties().at(0).name();
    QCOMPARE(propertyName, QString::fromAscii(versitSipId));

    //Check property value
    QString value = QString::fromAscii(versitDocument.properties().at(0).value().data());
    QCOMPARE(value, testUri);

    //Test2: InValid Sub Type Value is not encoded.
    onlineAccount.setAccountUri(testUri);
    onlineAccount.setSubTypes(QString::fromAscii("INVALIDSUBTYPE"));
    contact.saveDetail(&onlineAccount);
    versitDocument = mConverter->convertContact(contact);
    QCOMPARE(versitDocument.properties().count(), 0);
}



void UT_QVersitContactConverter::testEncodeFamily()
{
    QContact contact;
    QContactFamily family;

    //Test1: No Spouce No Family
    family.setContexts(QContactDetail::ContextHome);
    contact.saveDetail(&family);
    //Convert Contat Into Versit Document
    QVersitDocument versitDocument = mConverter->convertContact(contact);
    QCOMPARE(versitDocument.properties().count(), 0);

    //Test2: Only Spouce.
    QString spouce = QString::fromAscii("ABC");
    family.setSpouse(spouce);
    contact.saveDetail(&family);
    //Convert Contat Into Versit Document
    versitDocument = mConverter->convertContact(contact);

    //Ensure property Exisit
    QCOMPARE(versitDocument.properties().count(), 1);
    //Ensure no parameters exisit,
    QCOMPARE(0, versitDocument.properties().at(0).parameters().count());
    QString propertyName = versitDocument.properties().at(0).name();
    QCOMPARE(propertyName, QString::fromAscii(versitSpouseId));

    QString value = QString::fromAscii(versitDocument.properties().at(0).value().data() );
    QCOMPARE(value, spouce);


    //Test3: Spouce with few childerns
    QStringList childerns;
    childerns << QString::fromAscii("A") << QString::fromAscii("B") ;
    family.setChildren(childerns);
    family.setSpouse(spouce);
    contact.saveDetail(&family);
    versitDocument = mConverter->convertContact(contact);

    //Ensure property Exisit
    QCOMPARE(versitDocument.properties().count(), 2);
    //Ensure no parameters exisit,
    QCOMPARE(versitDocument.properties().at(0).parameters().count(), 0);

    //Ensure property parameer exisit and matches.
    QString propertyName1 = versitDocument.properties().at(0).name();
    QCOMPARE(propertyName1, QString::fromAscii(versitSpouseId));

    QString value1 = QString::fromAscii(versitDocument.properties().at(0).value().data() );
    QCOMPARE(value1, spouce);

    QString propertyName2 = versitDocument.properties().at(1).name();
    QCOMPARE(propertyName2, QString::fromAscii(versitChildrenId));

    QString value2 = QString::fromAscii(versitDocument.properties().at(1).value().data() );
    QString expected = QString::fromAscii("A,B");
    QCOMPARE(value2, expected);
}

// Test Utility Funcitons
QContactDetail UT_QVersitContactConverter::searchDetail(QList<QContactDetail> details,
                                                        QString search)
{
    QContactDetail detail;
    for (int i= 0; i < details.count(); i++) {
        if ( details.at(i).definitionName() == search )
        detail = details.at(i);
    }
    return detail;
}
