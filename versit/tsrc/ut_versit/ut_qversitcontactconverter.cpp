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


void UT_QVersitContactConvertert::init()
{
    mConverter = new QVersitContactConverter();
    mConverterPrivate = new QVersitContactConverterPrivate();
}

void UT_QVersitContactConvertert::cleanup()
{
    delete mConverterPrivate;
    delete mConverter;
}

void UT_QVersitContactConvertert::testConvertContact()
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
    QVersitDocument myVersitDocument = mConverter->convertContact(contact);
    
    // Ensure versit document is created with properties.
    QCOMPARE(2, myVersitDocument.properties().count());
}

void UT_QVersitContactConvertert::testEncodeName()
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

    QVersitDocument myVersitDocument;

    //Ensure there are no properties in the versit document.
    QVERIFY(!myVersitDocument.properties().count());

    //Convert Contat Into Versit Document
    myVersitDocument = mConverter->convertContact(contact);

    //Ensure Context parameter is not encoded.
    QCOMPARE(0, myVersitDocument.properties().at(0).parameters().count());
    
    //Ensure versit document is created with properties.
    QCOMPARE(1, myVersitDocument.properties().count());
    
    //Check for the property Name
    QString propertyName = myVersitDocument.properties().at(0).name();
    QString expectedPropertyName = 
        mConverterPrivate->mMappings.value(QContactName::DefinitionName);
    QCOMPARE(propertyName, expectedPropertyName );
    
    //Ensure value of properties contains all the infomation encoded
    QString value (myVersitDocument.properties().at(0).value() );
    QString expectedValue = "HH;Heiddo;A;Mr.;";
    QCOMPARE(expectedValue, value );
}

void UT_QVersitContactConvertert::testEncodePhoneNumber()
{
    QContact contact;

    // Adding Phone Numer for the Contact.
    QContactPhoneNumber p;
    p.setNumber("12345678");
    p.setContexts(QContactDetail::ContextHome);
    p.setSubTypes(QContactPhoneNumber::SubTypeMobile);

    contact.saveDetail(&p);

    //Convert Contat Into Versit Document
    QVersitDocument myVersitDocument = mConverter->convertContact(contact);

    //Ensure versit document is created with properties
    QCOMPARE(1, myVersitDocument.properties().count());

    //Ensure Only Valid params are encoded
    QCOMPARE(2, myVersitDocument.properties().at(0).parameters().count());

    //Ensure Valid parameters exisit
    QVERIFY(myVersitDocument.properties().at(0).parameters().contains(versitType, versitContextHomeId));
    QVERIFY(myVersitDocument.properties().at(0).parameters().contains(versitType, versitCellId));

    //Check property name
    QString propertyName = myVersitDocument.properties().at(0).name();
    QString expectedPropertyName = 
        mConverterPrivate->mMappings.value(QContactPhoneNumber::DefinitionName);
    QCOMPARE(propertyName, expectedPropertyName );
    
    //Check property value
    QString value (myVersitDocument.properties().at(0).value() );
    QString expectedValue = "12345678";
    QCOMPARE(expectedValue, value );
}

void UT_QVersitContactConvertert::testEncodeEmailAddress()
{
    QContact contact;

    QContactEmailAddress email;
    email.setEmailAddress("test@test");
    email.setContexts(QContactDetail::ContextHome);
    contact.saveDetail(&email);
    
    QVersitDocument myVersitDocument;
    
    //Ensure there are no properties in the versit document.
    QVERIFY(!myVersitDocument.properties().count());

    //Convert Contat Into Versit Document
    myVersitDocument = mConverter->convertContact(contact);

    //Ensure versit document is created with properties
    QCOMPARE(1, myVersitDocument.properties().count());

    //Ensure Only Valid params are encoded
    QCOMPARE(1, myVersitDocument.properties().at(0).parameters().count());

    //Ensure Valid parameters exisit
    QVERIFY(myVersitDocument.properties().at(0).parameters().contains(versitType, versitContextHomeId));
    
    //Check property name
    QString propertyName = myVersitDocument.properties().at(0).name();
    QString expectedPropertyName = 
        mConverterPrivate->mMappings.value(QContactEmailAddress::DefinitionName);
    QCOMPARE(propertyName, expectedPropertyName );
    
    //Check value 
    QString value (myVersitDocument.properties().at(0).value() );
    QString expectedValue = "test@test";
    QCOMPARE(expectedValue, value );

}

void UT_QVersitContactConvertert::testEncodeStreetAddress()
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
    
    QVersitDocument myVersitDocument;

    //Ensure there are no properties in the versit document.
    QVERIFY(!myVersitDocument.properties().count());

    //Conver Contact
    myVersitDocument= mConverter->convertContact(contact);
    
    //Ensure versit document is created with properties
    QCOMPARE(1, myVersitDocument.properties().count());

    //Ensure Only Valid params are encoded
    QCOMPARE(2, myVersitDocument.properties().at(0).parameters().count());

    //Ensure Valid parameters exisit
    QVERIFY(myVersitDocument.properties().at(0).parameters().contains(versitType, versitContextHomeId));
    QVERIFY(myVersitDocument.properties().at(0).parameters().contains(versitType, versitPostalId));
    
    //Check property name
    QString propertyName = myVersitDocument.properties().at(0).name();
    QString expectedPropertyName = 
        mConverterPrivate->mMappings.value(QContactAddress::DefinitionName);
    QCOMPARE(propertyName, expectedPropertyName );

    //Check property value 
    QString value (myVersitDocument.properties().at(0).value() );

    //Format: Post Office Address  + ";" +  Extended Address  + ";" + Street  + ";" +
    //Locality  + ";" + Region  + ";" + Postal Code  + ";" + Country

    QString expectedValue = ";;HKKI 1X 90;Helsinki;;00440;Finland";
    QCOMPARE(expectedValue, value );
}

void UT_QVersitContactConvertert::testEncodeUrl()
{
    QContact contact;

    // Adding Phone Numer for the Contact.
    QContactUrl p;
    p.setUrl("http://www.myhome.com");
    p.setContexts(QContactDetail::ContextHome);
    p.setSubType(QContactUrl::SubTypeHomePage);

    contact.saveDetail(&p);

    //Convert Contat Into Versit Document
    QVersitDocument myVersitDocument = mConverter->convertContact(contact);

    //Ensure property Exisit
    QCOMPARE(1, myVersitDocument.properties().count());

    //Ensure Only Valid params are encoded
    QCOMPARE(1, myVersitDocument.properties().at(0).parameters().count());

    //Ensure Valid parameters exisit
    QVERIFY(myVersitDocument.properties().at(0).parameters().contains(versitType, versitContextHomeId));

    //Ensure valud value exist for the parameters.

    QString propertyName = myVersitDocument.properties().at(0).name();
    QString expectedPropertyName =
        mConverterPrivate->mMappings.value(QContactUrl::DefinitionName);
    QCOMPARE(propertyName, expectedPropertyName);

    //Check property value
    QString value (myVersitDocument.properties().at(0).value());
    QString expectedValue = "http://www.myhome.com";
    QCOMPARE(expectedValue, value );
}

void UT_QVersitContactConvertert::testEncodeUid()
{
    QContact contact;

    // Adding uid for the Contact.
    QContactGuid p;

    //API Permits setting up context but it should not be in Versit Doc
    p.setContexts(QContactDetail::ContextHome);

    p.setGuid("0101222");
    contact.saveDetail(&p);

    //Convert Contat Into Versit Document
    QVersitDocument myVersitDocument = mConverter->convertContact(contact);

    //Ensure parameters does not exisit
    QCOMPARE(0, myVersitDocument.properties().at(0).parameters().count());

    //Ensure property Exisit
    QCOMPARE(1, myVersitDocument.properties().count());

    //Ensure valud value exist for the parameters.

    QString propertyName = myVersitDocument.properties().at(0).name();
    QString expectedPropertyName =
        mConverterPrivate->mMappings.value(QContactGuid::DefinitionName);
    QCOMPARE(propertyName, expectedPropertyName);

    //Check property value
    QString value (myVersitDocument.properties().at(0).value());
    QString expectedValue = "0101222";
    QCOMPARE(expectedValue, value);
}

void UT_QVersitContactConvertert::testEncodeRev()
{
    QContact contact;
    QContactTimestamp p;
    QDateTime changeRev = QDateTime::fromString("M1d1y200906:01:02", "'M'M'd'd'y'yyyyhh:mm:ss");

    p.setLastModified(changeRev);

    //API Permits setting up context but it should not be in Versit Doc
    p.setContexts(QContactDetail::ContextHome);

    contact.saveDetail(&p);

    //Convert Contat Into Versit Document
    QVersitDocument mVersitDocument = mConverter->convertContact(contact);

    //Ensure parameters does not exisit
    QCOMPARE(0, mVersitDocument.properties().at(0).parameters().count());

    //Ensure property Exisit
    QCOMPARE(1, mVersitDocument.properties().count());

    //Ensure property parameer exisit and matches.
    QString propertyName = mVersitDocument.properties().at(0).name();
    QString expectedPropertyName =
        mConverterPrivate->mMappings.value(QContactTimestamp::DefinitionName);
    QCOMPARE(propertyName, expectedPropertyName);

    //Check property value
    QString value = (mVersitDocument.properties().at(0).value());
    QString expectedValue = QString::fromAscii("2009-01-01T06:01:02");
    QCOMPARE(expectedValue, value);


    // Test 2: If Modified Data does not Exist than Date of Contact Created will be used.
    QDateTime emptyTime;
    p.setLastModified(emptyTime);
    p.setCreated(changeRev);
    contact.saveDetail(&p);

    mVersitDocument = mConverter->convertContact(contact);
    value = (mVersitDocument.properties().at(0).value() );

    // Ensure Value exisit and matches.
    QCOMPARE(expectedValue, value );
}

void UT_QVersitContactConvertert::testEncodeBirthDay()
{
    QContact contact;
    QDate date(2009,1,1);
    QContactBirthday birthDay;

    birthDay.setDate(date);

    //API Permits setting up context but it should not be in Versit Doc
    birthDay.setContexts(QContactDetail::ContextHome);

    contact.saveDetail(&birthDay);

    //Convert Contat Into Versit Document
    QVersitDocument mVersitDocument = mConverter->convertContact(contact);

    //Ensure parameters does not exisit
    QCOMPARE(0, mVersitDocument.properties().at(0).parameters().count());

    //Ensure property Exisit
    QCOMPARE(1, mVersitDocument.properties().count());

    //Ensure property parameer exisit and matches.
    QString propertyName = mVersitDocument.properties().at(0).name();
    QString expectedPropertyName =
        mConverterPrivate->mMappings.value(QContactBirthday::DefinitionName);
    QCOMPARE(propertyName, expectedPropertyName);

    //Check property value
    QString value = (mVersitDocument.properties().at(0).value());
    QString expectedValue = QString::fromAscii("2009-01-01");
    QCOMPARE(expectedValue, value);
}

void UT_QVersitContactConvertert::testEncodeNote()
{
    QContact contact;
    QString myNote = QString::fromAscii("My Note");
    QContactNote note;
    note.setNote(myNote);

    //API Permits setting up context but it should not be in Versit Doc
    note.setContexts(QContactDetail::ContextHome);

    contact.saveDetail(&note);

    //Convert Contat Into Versit Document
    QVersitDocument mVersitDocument = mConverter->convertContact(contact);

    //Ensure parameters does not exisit
    QCOMPARE(0, mVersitDocument.properties().at(0).parameters().count());

    //Ensure property Exisit
    QCOMPARE(1, mVersitDocument.properties().count());

    //Ensure property parameer exisit and matches.
    QString propertyName = mVersitDocument.properties().at(0).name();
    QString expectedPropertyName =
        mConverterPrivate->mMappings.value(QContactNote::DefinitionName);
    QCOMPARE(propertyName, expectedPropertyName);

    //Check property value
    QString value = mVersitDocument.properties().at(0).value();
    QCOMPARE(myNote, value);
}

void UT_QVersitContactConvertert::testEncodeGeoLocation()
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
    QVersitDocument mVersitDocument = mConverter->convertContact(contact);

    //Ensure parameters does not exisit
    QCOMPARE(0, mVersitDocument.properties().at(0).parameters().count());

    //Ensure property Exisit
    QCOMPARE(1, mVersitDocument.properties().count());

    //Ensure property parameer exisit and matches.
    QString propertyName = mVersitDocument.properties().at(0).name();
    QString expectedPropertyName =
        mConverterPrivate->mMappings.value(QContactGeolocation::DefinitionName);
    QCOMPARE(propertyName, expectedPropertyName);

    //Check property value
    QString value = (mVersitDocument.properties().at(0).value() );
    QString expectedValue;
    expectedValue =  longitude + QString::fromAscii(",") + latitude;
    QCOMPARE(expectedValue, value);
}

void UT_QVersitContactConvertert::testEncodeParameters()
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
    QVersitDocument myVersitDocument = mConverter->convertContact(contact);

    //Ensure property Exisit
    QCOMPARE(1, myVersitDocument.properties().count());

    //Ensure Only Valid params are encoded
    QCOMPARE(2, myVersitDocument.properties().at(0).parameters().count());

    //Ensure Valid parameters exisit
    QVERIFY(myVersitDocument.properties().at(0).parameters().contains(versitType, versitCellId));
    QVERIFY(myVersitDocument.properties().at(0).parameters().contains(versitType, versitVideoId));
}
