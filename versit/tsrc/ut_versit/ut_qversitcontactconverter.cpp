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
#include <QContactAddress.h> 
#include <QContactDisplayLabel.h> 
#include <QContactEmailAddress.h> 
#include <QContactName.h> 
#include <QContactPhoneNumber.h> 
#include <QContactUrl.h> 


/* Temporary Added for testing only */
#include <qcontactmanager.h>


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

void UT_QVersitContactConvertert::convertContacts()
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
    
    QVersitDocument myVersitDocument = mVersitContactConverter->convertContacts(contact);
    
    //Ensure versit document is created with properties.
    QCOMPARE(2, myVersitDocument.properties().count());
}


void UT_QVersitContactConvertert::encodeName()
{
    QContact contact;

    // Adding Name for the Contact
    QContactName name;
    name.setFirst("Heiddo");
    name.setLast("HH");
    name.setMiddle("A");
    name.setPrefix("Mr.");
    contact.saveDetail(&name);
    
    QVersitDocument myVersitDocument;
    QContactDetailDefinition defintion;
    defintion.setName(versitContactName);
    
    //Ensure there are no properties in the versit document.
    QVERIFY(!myVersitDocument.properties().count());
    mVersitContactConverter->encodeFieldInfo(myVersitDocument, contact, defintion);
    
    //Ensure versit document is created with properties.
    QCOMPARE(1, myVersitDocument.properties().count());
    QCOMPARE(versitName, myVersitDocument.properties().at(0).name());
    
    //Ensure value of properties contains all the infomation encoded
    QString value (myVersitDocument.properties().at(0).value() );
    QString expectedValue = "HH;Heiddo;A;Mr.;";
    QCOMPARE(expectedValue, value );
}

void UT_QVersitContactConvertert::encodePhoneNumber()
{
    QContact contact;

    // Adding Name for the Contact
    QContactName name;
    name.setFirst("Wesley");
    contact.saveDetail(&name);
    
    // Adding Phone Numer for the Contact.
    QContactPhoneNumber p;
    p.setNumber("12345678");
    contact.saveDetail(&p);
    
    QVersitDocument myVersitDocument;
    QContactDetailDefinition defintion;
    
    defintion.setName(versitContactPhoneNumer);

    //Ensure there are no properties in the versit document.
    QVERIFY(!myVersitDocument.properties().count());

    mVersitContactConverter->encodeFieldInfo(myVersitDocument, contact, defintion);
    
    QCOMPARE(1, myVersitDocument.properties().count());
    QCOMPARE(versitPhoneNumer, myVersitDocument.properties().at(0).name());
    
    QString value (myVersitDocument.properties().at(0).value() );
    QString expectedValue = "12345678";
    QCOMPARE(expectedValue, value );
}

void UT_QVersitContactConvertert::encodeEmailAddress()
{
    QContact contact;

    // Adding Name for the Contact
    QContactName name;
    name.setFirst("Wesley");
    contact.saveDetail(&name);

    QContactEmailAddress pref;
    pref.setEmailAddress("test@test");
    contact.saveDetail(&pref);
    
    QVersitDocument myVersitDocument;
    QContactDetailDefinition defintion;
    defintion.setName(versitContactEmail);
    
    //Ensure there are no properties in the versit document.
    QVERIFY(!myVersitDocument.properties().count());
    mVersitContactConverter->encodeFieldInfo(myVersitDocument, contact, defintion);
    
    QCOMPARE(1, myVersitDocument.properties().count());
    QCOMPARE(versitEmail, myVersitDocument.properties().at(0).name());

    QString value (myVersitDocument.properties().at(0).value() );
    QString expectedValue = "test@test";
    QCOMPARE(expectedValue, value );

}


void UT_QVersitContactConvertert::encodeStreetAddress()
{
    QContact contact;

    // Adding Name for the Contact
    QContactName name;
    name.setFirst("Wesley");
    contact.saveDetail(&name);
    
    QContactAddress address;
    address.setCountry("Finland");
    address.setPostcode("00440");
    address.setStreet("HKKI 1X 90");
    address.setLocality("Helsinki");
    contact.saveDetail(&address);
    
    QVersitDocument myVersitDocument;
    QContactDetailDefinition defintion;
    defintion.setName(versitContactAddress);

    //Ensure there are no properties in the versit document.
    QVERIFY(!myVersitDocument.properties().count());

    mVersitContactConverter->encodeFieldInfo(myVersitDocument, contact, defintion);
    
    QCOMPARE(1, myVersitDocument.properties().count());
    QCOMPARE(versitAddress, myVersitDocument.properties().at(0).name());
    
    QString value (myVersitDocument.properties().at(0).value() );
    QString expectedValue = ";HKKI 1X 90;Helsinki;;00440;Finland";
    QCOMPARE(expectedValue, value );
}
