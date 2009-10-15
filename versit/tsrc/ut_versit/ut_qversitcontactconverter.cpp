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

#include "ut_qversitcontactconverter.h"
#include "qversitcontactconverter.h"
#include "qversitproperty.h"
#include "qversitdefs.h"



void UT_QVersitContactConvertert::init()
{
    mVersitContactConverter = new QVersitContactConverter();
    QVERIFY(mVersitContactConverter);
}

void UT_QVersitContactConvertert::cleanup()
{
    delete mVersitContactConverter;
}

void UT_QVersitContactConvertert::error()
{
    QCOMPARE(QVersitContactConverter::NoError, mVersitContactConverter->error());
}

void UT_QVersitContactConvertert::convertContact()
{
    QContact contact;

    // Adding Name for the Contact
    QContactName name;
    name.setFirst("Moido");
    contact.saveDetail(&name);
    
    // Adding Phone Numer for the Contact.
    QContactPhoneNumber p;
    p.setNumber("12345678");
    contact.saveDetail(&p);

    //Convert contact into versit properties
    QVersitDocument myVersitDocument = mVersitContactConverter->convertContact(contact);
    
    //Ensure versit document is created with properties.
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
    contact.saveDetail(&name);

    QVersitDocument myVersitDocument;

    //Ensure there are no properties in the versit document.
    QVERIFY(!myVersitDocument.properties().count());

    //Convert Contat Into Versit Document
    myVersitDocument = mVersitContactConverter->convertContact(contact);
    
    //Ensure versit document is created with properties.
    QCOMPARE(1, myVersitDocument.properties().count());
    
    //Check for the property Name
    QString propertyName = myVersitDocument.properties().at(0).name();
    QString expectedPropertyName = 
        mVersitContactConverter->getMappingTable().value(QContactName::DefinitionName);
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
    contact.saveDetail(&p);

    //Convert Contat Into Versit Document
    QVersitDocument myVersitDocument = mVersitContactConverter->convertContact(contact);

    //Ensure versit document is created with properties
    QCOMPARE(1, myVersitDocument.properties().count());

    //Check property name
    QString propertyName = myVersitDocument.properties().at(0).name();
    QString expectedPropertyName = 
            mVersitContactConverter->getMappingTable().value(QContactPhoneNumber::DefinitionName);
    QCOMPARE(propertyName, expectedPropertyName );
    
    //Check property value
    QString value (myVersitDocument.properties().at(0).value() );
    QString expectedValue = "12345678";
    QCOMPARE(expectedValue, value );
}

void UT_QVersitContactConvertert::testEncodeEmailAddress()
{
    QContact contact;

    QContactEmailAddress pref;
    pref.setEmailAddress("test@test");
    contact.saveDetail(&pref);
    
    QVersitDocument myVersitDocument;
    
    //Ensure there are no properties in the versit document.
    QVERIFY(!myVersitDocument.properties().count());

    //Convert Contat Into Versit Document
    myVersitDocument = mVersitContactConverter->convertContact(contact);

    //Ensure versit document is created with properties
    QCOMPARE(1, myVersitDocument.properties().count());
    
    //Check property name
    QString propertyName = myVersitDocument.properties().at(0).name();
    QString expectedPropertyName = 
        mVersitContactConverter->getMappingTable().value(QContactEmailAddress::DefinitionName);
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
    contact.saveDetail(&address);
    
    QVersitDocument myVersitDocument;

    //Ensure there are no properties in the versit document.
    QVERIFY(!myVersitDocument.properties().count());

    //Conver Contact
    myVersitDocument= mVersitContactConverter->convertContact(contact);
    
    //Ensure versit document is created with properties
    QCOMPARE(1, myVersitDocument.properties().count());
    
    //Check property name
    QString propertyName = myVersitDocument.properties().at(0).name();
    QString expectedPropertyName = 
                mVersitContactConverter->getMappingTable().value(QContactAddress::DefinitionName);
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
    p.setUrl("http://wwww.myhome.com");
    p.setSubType(QContactUrl::SubTypeHomePage);

    contact.saveDetail(&p);

    //Convert Contat Into Versit Document
    QVersitDocument myVersitDocument = mVersitContactConverter->convertContact(contact);

    //Ensure property Exisit
    QCOMPARE(1, myVersitDocument.properties().count());

    //Ensure Only Valid params are encoded
    QCOMPARE(1, myVersitDocument.properties().at(0).parameters().count());

    //Ensure Valid parameters exisit
    QVERIFY(myVersitDocument.properties().at(0).parameters().contains("TYPE", "HOMEPAGE"));

    //Ensure valud value exist for the parameters.

    QString propertyName = myVersitDocument.properties().at(0).name();
    QString expectedPropertyName =
                mVersitContactConverter->getMappingTable().value(QContactUrl::DefinitionName);
    QCOMPARE(propertyName, expectedPropertyName );

    //Check property value
    QString value (myVersitDocument.properties().at(0).value() );
    QString expectedValue = "http://wwww.myhome.com";
    QCOMPARE(expectedValue, value );
}


void UT_QVersitContactConvertert::testEncodeUid()
{
    QContact contact;

    // Adding uid for the Contact.
    QContactGuid p;
    p.setGuid("0101222");
    contact.saveDetail(&p);

    //Convert Contat Into Versit Document
    QVersitDocument myVersitDocument = mVersitContactConverter->convertContact(contact);

    //Ensure property Exisit
    QCOMPARE(1, myVersitDocument.properties().count());

    //Ensure valud value exist for the parameters.

    QString propertyName = myVersitDocument.properties().at(0).name();
    QString expectedPropertyName =
                mVersitContactConverter->getMappingTable().value(QContactGuid::DefinitionName);
    QCOMPARE(propertyName, expectedPropertyName );

    //Check property value
    QString value (myVersitDocument.properties().at(0).value() );
    QString expectedValue = "0101222";
    QCOMPARE(expectedValue, value );
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

    p.setSubTypes(mysubtypes);

    contact.saveDetail(&p);

    //Convert Contat Into Versit Document
    QVersitDocument myVersitDocument = mVersitContactConverter->convertContact(contact);

    //Ensure property Exisit
    QCOMPARE(1, myVersitDocument.properties().count());

    //Ensure Only Valid params are encoded
    QCOMPARE(2, myVersitDocument.properties().at(0).parameters().count());

    //Ensure Valid parameters exisit
    QVERIFY(myVersitDocument.properties().at(0).parameters().contains("TYPE", "CELL"));
    QVERIFY(myVersitDocument.properties().at(0).parameters().contains("TYPE", "VIDEO"));
}

