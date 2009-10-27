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
    mTestPhotoFile = dir.filePath("versitTest001.jpg");
    mTestAudioFile = dir.filePath("versitTest001.wav");
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
    name.setFirst("Moido");
    contact.saveDetail(&name);
    
    // Adding phone number to the Contact.
    QContactPhoneNumber p;
    p.setNumber("12345678");
    contact.saveDetail(&p);

    // Convert contact into versit properties
    QVersitDocument versitDocument = mConverter->convertContact(contact);
    
    // Ensure versit document is created with properties.
    QCOMPARE(2, versitDocument.properties().count());
}

void UT_QVersitContactConverter::testEncodeName()
{
    QContact contact;

    // Adding name detail for the Contact
    QContactName name;
    name.setFirst("Heiddo");
    name.setLast("HH");
    name.setMiddle("A");
    name.setPrefix("Mr.");

    //Try to set context: for name context does not exisit ensure
    //Its not encoded into versit doc
    name.setContexts(QContactDetail::ContextHome);

    contact.saveDetail(&name);

    QVersitDocument versitDocument;

    //Ensure there are no properties in the versit document.
    QVERIFY(!versitDocument.properties().count());

    //Convert Contat Into Versit Document
    versitDocument = mConverter->convertContact(contact);

    //Ensure Context parameter is not encoded.
    QCOMPARE(0, versitDocument.properties().at(0).parameters().count());
    
    //Ensure versit document is created with properties.
    QCOMPARE(1, versitDocument.properties().count());
    
    //Check for the property Name
    QString propertyName = versitDocument.properties().at(0).name();
    QString expectedPropertyName = 
        mConverterPrivate->mMappings.value(QContactName::DefinitionName);
    QCOMPARE(propertyName, expectedPropertyName );
    
    //Ensure value of properties contains all the infomation encoded
    QString value (versitDocument.properties().at(0).value() );
    QString expectedValue = "HH;Heiddo;A;Mr.;";
    QCOMPARE(expectedValue, value );
}

void UT_QVersitContactConverter::testEncodePhoneNumber()
{
    QContact contact;

    // Adding Phone Numer for the Contact.
    QContactPhoneNumber p;
    p.setNumber("12345678");
    p.setContexts(QContactDetail::ContextHome);
    p.setSubTypes(QContactPhoneNumber::SubTypeMobile);

    contact.saveDetail(&p);

    //Convert Contat Into Versit Document
    QVersitDocument versitDocument = mConverter->convertContact(contact);

    //Ensure versit document is created with properties
    QCOMPARE(1, versitDocument.properties().count());

    //Ensure Only Valid params are encoded
    QCOMPARE(2, versitDocument.properties().at(0).parameters().count());

    //Ensure Valid parameters exisit
    QVERIFY(versitDocument.properties().at(0).parameters().contains(versitType, versitContextHomeId));
    QVERIFY(versitDocument.properties().at(0).parameters().contains(versitType, versitCellId));

    //Check property name
    QString propertyName = versitDocument.properties().at(0).name();
    QString expectedPropertyName = 
        mConverterPrivate->mMappings.value(QContactPhoneNumber::DefinitionName);
    QCOMPARE(propertyName, expectedPropertyName );
    
    //Check property value
    QString value (versitDocument.properties().at(0).value() );
    QString expectedValue = "12345678";
    QCOMPARE(expectedValue, value );
}

void UT_QVersitContactConverter::testEncodeEmailAddress()
{
    QContact contact;

    QContactEmailAddress email;
    email.setEmailAddress("test@test");
    email.setContexts(QContactDetail::ContextHome);
    contact.saveDetail(&email);
    
    QVersitDocument versitDocument;
    
    //Ensure there are no properties in the versit document.
    QVERIFY(!versitDocument.properties().count());

    //Convert Contat Into Versit Document
    versitDocument = mConverter->convertContact(contact);

    //Ensure versit document is created with properties
    QCOMPARE(1, versitDocument.properties().count());

    //Ensure Only Valid params are encoded
    QCOMPARE(1, versitDocument.properties().at(0).parameters().count());

    //Ensure Valid parameters exisit
    QVERIFY(versitDocument.properties().at(0).parameters().contains(versitType, versitContextHomeId));
    
    //Check property name
    QString propertyName = versitDocument.properties().at(0).name();
    QString expectedPropertyName = 
        mConverterPrivate->mMappings.value(QContactEmailAddress::DefinitionName);
    QCOMPARE(propertyName, expectedPropertyName );
    
    //Check value 
    QString value (versitDocument.properties().at(0).value() );
    QString expectedValue = "test@test";
    QCOMPARE(expectedValue, value );

}

void UT_QVersitContactConverter::testEncodeStreetAddress()
{
    QContact contact;

    QContactAddress address;
    address.setCountry("Finland");
    address.setPostcode("00440");
    address.setStreet("HKKI 1X 90");
    address.setLocality("Helsinki");

    address.setContexts(QContactDetail::ContextHome);
    address.setSubTypes(QContactAddress::SubTypePostal);
    contact.saveDetail(&address);
    
    QVersitDocument versitDocument;

    //Ensure there are no properties in the versit document.
    QVERIFY(!versitDocument.properties().count());

    //Conver Contact
    versitDocument= mConverter->convertContact(contact);
    
    //Ensure versit document is created with properties
    QCOMPARE(1, versitDocument.properties().count());

    //Ensure Only Valid params are encoded
    QCOMPARE(2, versitDocument.properties().at(0).parameters().count());

    //Ensure Valid parameters exisit
    QVERIFY(versitDocument.properties().at(0).parameters().contains(versitType, versitContextHomeId));
    QVERIFY(versitDocument.properties().at(0).parameters().contains(versitType, versitPostalId));
    
    //Check property name
    QString propertyName = versitDocument.properties().at(0).name();
    QString expectedPropertyName = 
        mConverterPrivate->mMappings.value(QContactAddress::DefinitionName);
    QCOMPARE(propertyName, expectedPropertyName );

    //Check property value 
    QString value (versitDocument.properties().at(0).value() );

    //Format: Post Office Address  + ";" +  Extended Address  + ";" + Street  + ";" +
    //Locality  + ";" + Region  + ";" + Postal Code  + ";" + Country

    QString expectedValue = ";;HKKI 1X 90;Helsinki;;00440;Finland";
    QCOMPARE(expectedValue, value );
}

void UT_QVersitContactConverter::testEncodeUrl()
{
    QContact contact;

    // Adding Phone Numer for the Contact.
    QContactUrl p;
    p.setUrl("http://www.myhome.com");
    p.setContexts(QContactDetail::ContextHome);
    p.setSubType(QContactUrl::SubTypeHomePage);

    contact.saveDetail(&p);

    //Convert Contat Into Versit Document
    QVersitDocument versitDocument = mConverter->convertContact(contact);

    //Ensure property Exisit
    QCOMPARE(1, versitDocument.properties().count());

    //Ensure Only Valid params are encoded
    QCOMPARE(1, versitDocument.properties().at(0).parameters().count());

    //Ensure Valid parameters exisit
    QVERIFY(versitDocument.properties().at(0).parameters().contains(versitType, versitContextHomeId));

    //Ensure valud value exist for the parameters.

    QString propertyName = versitDocument.properties().at(0).name();
    QString expectedPropertyName =
        mConverterPrivate->mMappings.value(QContactUrl::DefinitionName);
    QCOMPARE(propertyName, expectedPropertyName);

    //Check property value
    QString value (versitDocument.properties().at(0).value());
    QString expectedValue = "http://www.myhome.com";
    QCOMPARE(expectedValue, value );
}

void UT_QVersitContactConverter::testEncodeUid()
{
    QContact contact;

    // Adding uid for the Contact.
    QContactGuid p;

    //API Permits setting up context but it should not be in Versit Doc
    p.setContexts(QContactDetail::ContextHome);

    p.setGuid("0101222");
    contact.saveDetail(&p);

    //Convert Contat Into Versit Document
    QVersitDocument versitDocument = mConverter->convertContact(contact);

    //Ensure parameters does not exisit
    QCOMPARE(0, versitDocument.properties().at(0).parameters().count());

    //Ensure property Exisit
    QCOMPARE(1, versitDocument.properties().count());

    //Ensure valud value exist for the parameters.

    QString propertyName = versitDocument.properties().at(0).name();
    QString expectedPropertyName =
        mConverterPrivate->mMappings.value(QContactGuid::DefinitionName);
    QCOMPARE(propertyName, expectedPropertyName);

    //Check property value
    QString value (versitDocument.properties().at(0).value());
    QString expectedValue = "0101222";
    QCOMPARE(expectedValue, value);
}

void UT_QVersitContactConverter::testEncodeRev()
{
    QContact contact;
    QContactTimestamp p;
    QDateTime changeRev = QDateTime::fromString("M1d1y200906:01:02", "'M'M'd'd'y'yyyyhh:mm:ss");
    changeRev.setTimeSpec(Qt::UTC);

    p.setLastModified(changeRev);

    //API Permits setting up context but it should not be in Versit Doc
    p.setContexts(QContactDetail::ContextHome);

    contact.saveDetail(&p);

    //Convert Contat Into Versit Document
    QVersitDocument versitDocument = mConverter->convertContact(contact);

    //Ensure parameters does not exisit
    QCOMPARE(0, versitDocument.properties().at(0).parameters().count());

    //Ensure property Exisit
    QCOMPARE(1, versitDocument.properties().count());

    //Ensure property parameer exisit and matches.
    QString propertyName = versitDocument.properties().at(0).name();
    QString expectedPropertyName =
        mConverterPrivate->mMappings.value(QContactTimestamp::DefinitionName);
    QCOMPARE(propertyName, expectedPropertyName);

    //Check property value
    QString value = (versitDocument.properties().at(0).value());
    QString expValueUTCEncoded = QString::fromAscii("2009-01-01T06:01:02Z");
    QCOMPARE(expValueUTCEncoded, value);


    // Test 2: If Modified Data does not Exist than date of contact created will be used.
    QDateTime emptyTime;
    p.setLastModified(emptyTime);
    p.setCreated(changeRev);
    contact.saveDetail(&p);

    versitDocument = mConverter->convertContact(contact);
    value = QString::fromAscii(versitDocument.properties().at(0).value());

    // Ensure Value exisit and matches.
    QCOMPARE(expValueUTCEncoded, value);


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
    QCOMPARE(expValueUTCEncoded, value);


    // Test 4: Empty Date Entry No Created / Modified Date.
    p.setLastModified(emptyTime);
    p.setCreated(emptyTime);
    changeRev.setTimeSpec(Qt::LocalTime);
    contact.saveDetail(&p);

    versitDocument = mConverter->convertContact(contact);
    QCOMPARE(0, versitDocument.properties().count());
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
    QCOMPARE(0, versitDocument.properties().at(0).parameters().count());

    //Ensure property Exisit
    QCOMPARE(1, versitDocument.properties().count());

    //Ensure property parameer exisit and matches.
    QString propertyName = versitDocument.properties().at(0).name();
    QString expectedPropertyName =
        mConverterPrivate->mMappings.value(QContactBirthday::DefinitionName);
    QCOMPARE(propertyName, expectedPropertyName);

    //Check property value
    QString value = (versitDocument.properties().at(0).value());
    QString expectedValue = QString::fromAscii("2009-01-01");
    QCOMPARE(expectedValue, value);
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
    QCOMPARE(0, versitDocument.properties().at(0).parameters().count());

    //Ensure property Exisit
    QCOMPARE(1, versitDocument.properties().count());

    //Ensure property parameer exisit and matches.
    QString propertyName = versitDocument.properties().at(0).name();
    QString expectedPropertyName =
        mConverterPrivate->mMappings.value(QContactNote::DefinitionName);
    QCOMPARE(propertyName, expectedPropertyName);

    //Check property value
    QString value = versitDocument.properties().at(0).value();
    QCOMPARE(myNote, value);
}

void UT_QVersitContactConverter::testEncodeGeoLocation()
{
    QContact contact;
    QContactGeolocation geoLocation;

    QString longitude = "99.9";
    QString latitude = "98.9";
    QString accuracy = "90.7";

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
    QCOMPARE(0, versitDocument.properties().at(0).parameters().count());

    //Ensure property Exisit
    QCOMPARE(1, versitDocument.properties().count());

    //Ensure property parameer exisit and matches.
    QString propertyName = versitDocument.properties().at(0).name();
    QString expectedPropertyName =
        mConverterPrivate->mMappings.value(QContactGeolocation::DefinitionName);
    QCOMPARE(propertyName, expectedPropertyName);

    //Check property value
    QString value = (versitDocument.properties().at(0).value() );
    QString expectedValue = longitude + QString::fromAscii(",") + latitude;
    QCOMPARE(expectedValue, value);
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
}



void UT_QVersitContactConverter::testEncodeEmbeddedContent()
{
    QContact contact;
    QContactAvatar contactAvatar;

    // Test 1: URL
    const QString url = "http://www.myhome.com/test.jpg";
    contactAvatar.setAvatar(url);
    contactAvatar.setSubType(QContactAvatar::SubTypeImage);
    contact.saveDetail(&contactAvatar);
    QVersitDocument versitDocument = mConverter->convertContact(contact);
    QVERIFY(!mScaleSignalEmitted);
    QVersitProperty photoProperty = versitDocument.properties().at(0);
    QCOMPARE(photoProperty.parameters().count(), 2);
    QVERIFY(photoProperty.parameters().contains(versitType,versitPhotoJpeg));
    QVERIFY(photoProperty.parameters().contains(versitValue,versitUrlId));
    QString expectedPropertyName =
        mConverterPrivate->mMappings.value(QContactAvatar::SubTypeImage);
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
        QString::fromAscii(versitPhotoJpeg)));
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
        QString::fromAscii(versitPhotoJpeg)));
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
        QString::fromAscii(versitAudioWave)));
    QVERIFY(soundProperty.parameters().contains(
        QString::fromAscii(versitEncoding),
        QString::fromAscii(versitEncodingBase64)));
    QCOMPARE(soundProperty.value(), QByteArray());

    // Test 6: New Media Format will be encoded also
    const QString testUrl = "http://www.myhome.com/test.ggg";
    contactAvatar.setAvatar(testUrl);
    contactAvatar.setSubType(QContactAvatar::SubTypeImage);
    contact.saveDetail(&contactAvatar);
    versitDocument = mConverter->convertContact(contact);
    QVERIFY(!mScaleSignalEmitted);
    QCOMPARE(1, versitDocument.properties().count());
    QCOMPARE(2, versitDocument.properties().at(0).parameters().count());
    QVERIFY(versitDocument.properties().at(0).parameters().contains(versitType, "GGG"));
    QVERIFY(versitDocument.properties().at(0).parameters().contains(versitValue, versitUrlId));

    // Test 7: UnSupported Media Type, properties and parameters are not encoded
    const QString testUrl2 = "http://www.myhome.com/test.jpg";
    contactAvatar.setAvatar(testUrl2);
    // un-supported media type is encoded
    contactAvatar.setSubType(QContactAvatar::SubTypeTexturedMesh);
    contact.saveDetail(&contactAvatar);
    versitDocument = mConverter->convertContact(contact);
    QCOMPARE(0, versitDocument.properties().count());
}

void UT_QVersitContactConverter::testEncodeParameters()
{
    QContact contact;

    // Adding Phone Numer for the Contact.
    QContactPhoneNumber p;
    p.setNumber("12345678");

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
    QCOMPARE(1, versitDocument.properties().count());

    //Ensure Only Valid params are encoded
    QCOMPARE(2, versitDocument.properties().at(0).parameters().count());

    //Ensure Valid parameters exisit
    QVERIFY(versitDocument.properties().at(0).parameters().contains(versitType, versitCellId));
    QVERIFY(versitDocument.properties().at(0).parameters().contains(versitType, versitVideoId));
}

void UT_QVersitContactConverter::testIsValidRemoteUrl()
{
    QContact contact;
    QContactAvatar contactAvatar;
    QVersitDocument versitDocument;

    // Test1: http URL
    QString url = "http://www.nonoh.com/test.jpg";
    contactAvatar.setAvatar(url);
    contactAvatar.setSubType(QContactAvatar::SubTypeImage);
    contact.saveDetail(&contactAvatar);
    versitDocument = mConverter->convertContact(contact);
    QCOMPARE(1, versitDocument.properties().count());


    // Test2: FTP URL
    url = QString::fromAscii("ftp://nonoh.com/test.jpg");
    contactAvatar.setAvatar(url);
    contactAvatar.setSubType(QContactAvatar::SubTypeImage);
    contact.saveDetail(&contactAvatar);
    versitDocument = mConverter->convertContact(contact);
    QCOMPARE(1, versitDocument.properties().count());


    // Test3: NEW Protocol URL
    url = QString::fromAscii("myProtocol://nonoh.com/test.jpg");
    contactAvatar.setAvatar(url);
    contactAvatar.setSubType(QContactAvatar::SubTypeImage);
    contact.saveDetail(&contactAvatar);
    versitDocument = mConverter->convertContact(contact);
    QCOMPARE(1, versitDocument.properties().count());


    // Test4: URL without schem
    url = QString::fromAscii("www.nonoh.com/test.jpg");
    contactAvatar.setAvatar(url);
    contactAvatar.setSubType(QContactAvatar::SubTypeImage);
    contact.saveDetail(&contactAvatar);
    versitDocument = mConverter->convertContact(contact);
    QCOMPARE(1, versitDocument.properties().count());

    // Test5: File Name but File does not Exisit
    url = QString::fromAscii("c:/filedoesnotexisit.jok");
    contactAvatar.setAvatar(url);
    contactAvatar.setSubType(QContactAvatar::SubTypeImage);
    contact.saveDetail(&contactAvatar);
    versitDocument = mConverter->convertContact(contact);
    QCOMPARE(0, versitDocument.properties().count());
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
    QCOMPARE(0, versitDocument.properties().at(0).parameters().count());

    //Ensure property Exisit
    QCOMPARE(1, versitDocument.properties().count());

    //Ensure property parameer exisit and matches.
    QString propertyName = versitDocument.properties().at(0).name();
    QString expectedPropertyName =
        mConverterPrivate->mMappings.value(QContactGender::DefinitionName);
    QCOMPARE(propertyName, expectedPropertyName);

    //Check property value
    QString value = versitDocument.properties().at(0).value();
    QCOMPARE(QString(QLatin1String(QContactGender::GenderMale)), value);
}

void UT_QVersitContactConverter::testEncodeNickName()
{
    QContact contact;
    QContactNickname nickName;
    QString nick("Simpson");
    nickName.setNickname(nick);

    //API Permits setting up context but it should not be in Versit Doc
    nickName.setContexts(QContactDetail::ContextHome);

    contact.saveDetail(&nickName);

    //Convert Contat Into Versit Document
    QVersitDocument versitDocument = mConverter->convertContact(contact);

    //Ensure parameters does not exisit
    QCOMPARE(0, versitDocument.properties().at(0).parameters().count());

    //Ensure property Exisit
    QCOMPARE(1, versitDocument.properties().count());

    //Ensure property parameer exisit and matches.
    QString propertyName = versitDocument.properties().at(0).name();
    QCOMPARE(propertyName, QString::fromAscii(versitNicknameXId));
    QString expectedPropertyName =
        mConverterPrivate->mMappings.value(QContactNickname::DefinitionName);
    QCOMPARE(propertyName, expectedPropertyName);

    //Check property value
    QString value = versitDocument.properties().at(0).value();
    QCOMPARE(nick, value);

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
    QCOMPARE(1, versitDocument.properties().at(0).parameters().count());

    //Ensure property Exisit
    QCOMPARE(1, versitDocument.properties().count());

    //Ensure property parameer exisit and matches.
    QString propertyName = versitDocument.properties().at(0).name();
    QString expectedPropertyName =
        mConverterPrivate->mMappings.value(QContactAnniversary::DefinitionName);
    QCOMPARE(propertyName, expectedPropertyName);

    //Check property value
    QString expectedValue = QString::fromAscii("2009-01-01");
    QString value = versitDocument.properties().at(0).value();
    QCOMPARE(expectedValue, value);

    // Ensure Sub types matches.
    QVERIFY(versitDocument.properties().at(0).parameters().contains(versitType,
            QString(QLatin1String(QContactAnniversary::SubTypeWedding)).toUpper()));
}


void UT_QVersitContactConverter::testEncodOnlineAccount()
{
    QContact contact;
    QContactOnlineAccount onlineAccount;

    //Test1:  Valid SWIS Account.
    QString testUri = "sip:abc@temp.com";
    onlineAccount.setAccountUri(testUri);
    onlineAccount.setSubTypes(QContactOnlineAccount::SubTypeShareVideo);
    onlineAccount.setContexts(QContactDetail::ContextHome);
    contact.saveDetail(&onlineAccount);
    
    //Convert Contat Into Versit Document
    QVersitDocument versitDocument = mConverter->convertContact(contact);

    //Ensure parameters exisit, i.e. home and swiss
    QCOMPARE(2, versitDocument.properties().at(0).parameters().count());
    QVERIFY(versitDocument.properties().at(0).parameters().contains(
            versitType, versitContextHomeId));
    QVERIFY(versitDocument.properties().at(0).parameters().contains(
            versitType, versitSwisId));

    //Ensure property Exisit
    QCOMPARE(1, versitDocument.properties().count());

    //Ensure property parameer exisit and matches.
    QString propertyName = versitDocument.properties().at(0).name();
    QCOMPARE(propertyName, QString::fromAscii(versitSipId));

    //Check property value
    QString value = versitDocument.properties().at(0).value();
    QCOMPARE(testUri, value);

    //Test2: InValid Sub Type Value is not encoded.
    onlineAccount.setAccountUri(testUri);
    onlineAccount.setSubTypes("INVALIDSUBTYPE");
    contact.saveDetail(&onlineAccount);
    versitDocument = mConverter->convertContact(contact);
    QCOMPARE(0, versitDocument.properties().count());
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
    QCOMPARE(0, versitDocument.properties().count());

    //Test2: Only Spouce.
    QString spouce = "ABC";
    family.setSpouse(spouce);
    contact.saveDetail(&family);
    //Convert Contat Into Versit Document
    versitDocument = mConverter->convertContact(contact);

    //Ensure property Exisit
    QCOMPARE(1, versitDocument.properties().count());
    //Ensure no parameters exisit,
    QCOMPARE(0, versitDocument.properties().at(0).parameters().count());
    QString propertyName = versitDocument.properties().at(0).name();
    QCOMPARE(propertyName, QString::fromAscii(versitSpouseId));

    QString value (versitDocument.properties().at(0).value() );
    QCOMPARE(spouce, value );


    //Test3: Spouce with few childerns
    QStringList childerns;
    childerns << "A" << "B" ;
    family.setChildren(childerns);
    family.setSpouse(spouce);
    contact.saveDetail(&family);
    versitDocument = mConverter->convertContact(contact);

    //Ensure property Exisit
    QCOMPARE(2, versitDocument.properties().count());
    //Ensure no parameters exisit,
    QCOMPARE(0, versitDocument.properties().at(0).parameters().count());

    //Ensure property parameer exisit and matches.
    QString propertyName1 = versitDocument.properties().at(0).name();
    QCOMPARE(propertyName1, QString::fromAscii(versitSpouseId));

    QString value1 (versitDocument.properties().at(0).value() );
    QCOMPARE(spouce, value1 );

    QString propertyName2 = versitDocument.properties().at(1).name();
    QCOMPARE(propertyName2, QString::fromAscii(versitChildrenId));

    QString value2 (versitDocument.properties().at(1).value() );
    QString expected = "A,B";
    QCOMPARE(expected, value2 );
}

