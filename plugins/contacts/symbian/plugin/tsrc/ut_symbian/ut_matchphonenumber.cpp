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

#include "ut_matchphonenumber.h"

#include "cntdbinfo.h"
#include "cntsymbiansrvconnection.h"
#include "cntfilterdetail.h"
#include "cntfilterunion.h"
#include "cntfilterintersection.h"
#include <qtcontacts.h>
#include <QContactDetailFilter.h>
#include <QtTest/QtTest>


#include "cntfilterdetail.h"

const TUid KCRUidTelConfiguration = {0x102828B8};
// Amount of digits to be used in contact matching.
// This allows a customer to variate the amount of digits to be matched.
const TUint32 KTelMatchDigits                               = 0x00000001;

const QString DB1_FIRSTNAME1 = "A";
const QString DB1_LASTNAME1 = "9";
const QString DB1_FIRSTNAME2 = "D";
const QString DB1_LASTNAME2 = "6";


void TestMatchPhoneNumber::initTestCase()
{
    QContactManager::Error err;
    
    //create manager
    m_engine = new CntSymbianEngine(QMap<QString, QString>(), &err);
        
    //open symbian database
    m_database = 0;
        TRAPD(error, m_database = CContactDatabase::OpenL());
        QVERIFY(error == KErrNone);

    // Remove all contacts from the database
    QList<QContactLocalId> contacts = m_engine->contactIds(QContactFilter(),QList<QContactSortOrder>(), &err);
    QMap<int, QContactManager::Error> errorMap;
    m_engine->removeContacts(contacts, &errorMap, &err);
        
    QList<QContactLocalId> cnt_ids = m_engine->contactIds(QContactFilter(), QList<QContactSortOrder>(), &err);
    QVERIFY(0 == cnt_ids.count());
}

void TestMatchPhoneNumber::cleanupTestCase()
{
    delete m_engine;
}

void TestMatchPhoneNumber::cleanupContactsDb()
{
    QContactManager::Error err;
    
    // Remove all contacts from the database
    QList<QContactLocalId> contacts = m_engine->contactIds(QContactFilter(),QList<QContactSortOrder>(), &err);
    QMap<int, QContactManager::Error> errorMap;
    m_engine->removeContacts(contacts, &errorMap, &err);
        
    QList<QContactLocalId> cnt_ids = m_engine->contactIds(QContactFilter(), QList<QContactSortOrder>(), &err);
    QVERIFY(0 == cnt_ids.count());
}

void TestMatchPhoneNumber::setupBestMatchingContactsDb1()
{
    cleanupContactsDb();
    
    addContact( DB1_FIRSTNAME1, DB1_LASTNAME1, QString( "00358401234561" ) );
    addContact( DB1_FIRSTNAME1, DB1_LASTNAME1, QString( "99533358401234562" ) );
    addContact( DB1_FIRSTNAME1, DB1_LASTNAME1, QString( "99533358401234563" ) );
    addContact( DB1_FIRSTNAME2, DB1_LASTNAME2, QString( "+358401234564" ) );
    addContact( DB1_FIRSTNAME2, DB1_LASTNAME2, QString( "00358401234565" ) );
    addContact( DB1_FIRSTNAME2, DB1_LASTNAME2, QString( "99533358401234566" ) );
    addContact( DB1_FIRSTNAME2, DB1_LASTNAME2, QString( "0561234567" ) );
    addContact( DB1_FIRSTNAME2, DB1_LASTNAME2, QString( "0561234568" ) );
    addContact( DB1_FIRSTNAME2, DB1_LASTNAME2, QString( "061234569" ) );
    
    QContactManager::Error err;
    QList<QContactLocalId> cnt_ids = m_engine->contactIds(QContactFilter(),QList<QContactSortOrder>(), &err);
    int j = cnt_ids.count();
    QVERIFY(9 == j);
}

void TestMatchPhoneNumber::setupBestMatchingContactsDb2()
{
    cleanupContactsDb();
    
    addContact( DB1_FIRSTNAME1, DB1_LASTNAME1, QString( "+358401234561" ) );
    addContact( DB1_FIRSTNAME1, DB1_LASTNAME1, QString( "+358401234562" ) );
    addContact( DB1_FIRSTNAME1, DB1_LASTNAME1, QString( "00358401234563" ) );
    addContact( DB1_FIRSTNAME2, DB1_LASTNAME2, QString( "0401234564" ) );
    addContact( DB1_FIRSTNAME2, DB1_LASTNAME2, QString( "0401234565" ) );
    addContact( DB1_FIRSTNAME2, DB1_LASTNAME2, QString( "0401234566" ) );
    addContact( DB1_FIRSTNAME2, DB1_LASTNAME2, QString( "061234567" ) );
    addContact( DB1_FIRSTNAME2, DB1_LASTNAME2, QString( "+8261234568" ) );
    addContact( DB1_FIRSTNAME2, DB1_LASTNAME2, QString( "+82561234569" ) );
    
    QContactManager::Error err;
    QList<QContactLocalId> cnt_ids = m_engine->contactIds(QContactFilter(),QList<QContactSortOrder>(), &err);
    int j = cnt_ids.count();
    QVERIFY(9 == j);
}

void TestMatchPhoneNumber::setupBestMatchingContactsDb3()
{
    cleanupContactsDb();
    
    addContact( DB1_FIRSTNAME1, DB1_LASTNAME1, QString( "1234561" ) );
    addContact( DB1_FIRSTNAME1, DB1_LASTNAME1, QString( "401234561" ) );
    addContact( DB1_FIRSTNAME1, DB1_LASTNAME1, QString( "00358401234563" ) );
    addContact( DB1_FIRSTNAME2, DB1_LASTNAME2, QString( "0401234564" ) );
    addContact( DB1_FIRSTNAME2, DB1_LASTNAME2, QString( "+358401234565" ) );
    addContact( DB1_FIRSTNAME2, DB1_LASTNAME2, QString( "16101234566" ) );
    addContact( DB1_FIRSTNAME2, DB1_LASTNAME2, QString( "4567" ) );
    addContact( DB1_FIRSTNAME2, DB1_LASTNAME2, QString( "1234567" ) );
    addContact( DB1_FIRSTNAME2, DB1_LASTNAME2, QString( "221234567" ) );
    
    addContact( DB1_FIRSTNAME2, DB1_LASTNAME2, QString( "4566" ) );
    addContact( DB1_FIRSTNAME2, DB1_LASTNAME2, QString( "*4566" ) );
    addContact( DB1_FIRSTNAME2, DB1_LASTNAME2, QString( "*55*0705#" ) );
    
    QContactManager::Error err;
    QList<QContactLocalId> cnt_ids = m_engine->contactIds(QContactFilter(),QList<QContactSortOrder>(), &err);
    int j = cnt_ids.count();
    QVERIFY(12 == j);
}

void TestMatchPhoneNumber::addContact(QString firstname,QString lastname,QString phonenumber)
{
    //Currenlty we can only fetch firstname,lastname,companyname and sip/email/phone from the databse
    // so create contact with only these details
    QContact phonecontact;
    // Stefann Fedrernn +02644424423 ste.Fed@nokia.com
    // Contact details
    QContactName contactName;
    contactName.setFirstName(firstname);
    contactName.setLastName(lastname);
    phonecontact.saveDetail(&contactName);
    
    QContactPhoneNumber number;
    number.setContexts("Home");
    number.setSubTypes("Mobile");
    number.setNumber(phonenumber);
    phonecontact.saveDetail(&number);
    
    QContactManager::Error err;
    QVERIFY(m_engine->saveContact(&phonecontact, &err));
}

void TestMatchPhoneNumber::testMatchPhoneNumber()
{
    testBestMatchingStrategy1();
    testBestMatchingStrategy2();
    testBestMatchingStrategy3();
    testBestMatchingWithUnionFilter();
    testBestMatchingWithIntersectionFilter();
}

void TestMatchPhoneNumber::testBestMatchingStrategy1()
{
    setupBestMatchingContactsDb1();
    
    CntSymbianSrvConnection srvConnection(m_engine);
    CntDbInfo dbInfo(m_engine);
    bool filterSupported(true);
    
    QContactDetailFilter detailFilter;
    detailFilter.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldNumber);
    QList<QContactLocalId> cnt_ids;
    QContactManager::Error error;
    int seachedcontactcount;
    int expectedCount;
    
    CntFilterDetail filter(*m_database,srvConnection,dbInfo);
    filter.emulateBestMatching();
    
    detailFilter.setValue("+358401234561");
    error = QContactManager::NoError;
    cnt_ids = filter.contacts(detailFilter, QContactSortOrder(), filterSupported, &error);
    // check counts 
    seachedcontactcount = cnt_ids.count();
    expectedCount =1;  
    QVERIFY(expectedCount == seachedcontactcount);
    
    detailFilter.setValue("+358401234562");
    error = QContactManager::NoError;
    cnt_ids = filter.contacts(detailFilter, QContactSortOrder(), filterSupported, &error);
    // check counts 
    seachedcontactcount = cnt_ids.count();
    expectedCount =1;  
    QVERIFY(expectedCount == seachedcontactcount);
    
    detailFilter.setValue("00358401234563");
    error = QContactManager::NoError;
    cnt_ids = filter.contacts(detailFilter, QContactSortOrder(), filterSupported, &error);
    // check counts 
    seachedcontactcount = cnt_ids.count();
    expectedCount =1;  
    QVERIFY(expectedCount == seachedcontactcount);
    
    detailFilter.setValue("0401234564");
    error = QContactManager::NoError;
    cnt_ids = filter.contacts(detailFilter, QContactSortOrder(), filterSupported, &error);
    // check counts 
    seachedcontactcount = cnt_ids.count();
    expectedCount =1;  
    QVERIFY(expectedCount == seachedcontactcount);
    
    detailFilter.setValue("0401234565");
    error = QContactManager::NoError;
    cnt_ids = filter.contacts(detailFilter, QContactSortOrder(), filterSupported, &error);
    // check counts 
    seachedcontactcount = cnt_ids.count();
    expectedCount =1;  
    QVERIFY(expectedCount == seachedcontactcount);
    
    detailFilter.setValue("0401234566");
    error = QContactManager::NoError;
    cnt_ids = filter.contacts(detailFilter, QContactSortOrder(), filterSupported, &error);
    // check counts 
    seachedcontactcount = cnt_ids.count();
    expectedCount =1;  
    QVERIFY(expectedCount == seachedcontactcount);
    
    detailFilter.setValue("061234567");
    error = QContactManager::NoError;
    cnt_ids = filter.contacts(detailFilter, QContactSortOrder(), filterSupported, &error);
    // check counts 
    seachedcontactcount = cnt_ids.count();
    expectedCount =0;  
    QVERIFY(expectedCount == seachedcontactcount);
    
    detailFilter.setValue("+8261234568");
    error = QContactManager::NoError;
    cnt_ids = filter.contacts(detailFilter, QContactSortOrder(), filterSupported, &error);
    // check counts 
    seachedcontactcount = cnt_ids.count();
    expectedCount =0;  
    QVERIFY(expectedCount == seachedcontactcount);
    
    detailFilter.setValue("+82561234569");
    error = QContactManager::NoError;
    cnt_ids = filter.contacts(detailFilter, QContactSortOrder(), filterSupported, &error);
    // check counts 
    seachedcontactcount = cnt_ids.count();
    expectedCount =1;  
    QVERIFY(expectedCount == seachedcontactcount);
}
void TestMatchPhoneNumber::testBestMatchingStrategy2()
{
    setupBestMatchingContactsDb2();
    
    CntSymbianSrvConnection srvConnection(m_engine);
    CntDbInfo dbInfo(m_engine);
    bool filterSupported(true);
    
    QContactDetailFilter detailFilter;
    detailFilter.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldNumber);
    QList<QContactLocalId> cnt_ids;
    QContactManager::Error error;
    int seachedcontactcount;
    int expectedCount;
    
    CntFilterDetail filter(*m_database,srvConnection,dbInfo);
    filter.emulateBestMatching();
    
    detailFilter.setValue("00358401234561");
    error = QContactManager::NoError;
    cnt_ids = filter.contacts(detailFilter, QContactSortOrder(), filterSupported, &error);
    // check counts 
    seachedcontactcount = cnt_ids.count();
    expectedCount =1;  
    QVERIFY(expectedCount == seachedcontactcount);
    
    detailFilter.setValue("99533358401234562");
    error = QContactManager::NoError;
    cnt_ids = filter.contacts(detailFilter, QContactSortOrder(), filterSupported, &error);
    // check counts 
    seachedcontactcount = cnt_ids.count();
    expectedCount =1;  
    QVERIFY(expectedCount == seachedcontactcount);
    
    detailFilter.setValue("99533358401234563");
    error = QContactManager::NoError;
    cnt_ids = filter.contacts(detailFilter, QContactSortOrder(), filterSupported, &error);
    // check counts 
    seachedcontactcount = cnt_ids.count();
    expectedCount =1;  
    QVERIFY(expectedCount == seachedcontactcount);
    
    detailFilter.setValue("+358401234564");
    error = QContactManager::NoError;
    cnt_ids = filter.contacts(detailFilter, QContactSortOrder(), filterSupported, &error);
    // check counts 
    seachedcontactcount = cnt_ids.count();
    expectedCount =1;  
    QVERIFY(expectedCount == seachedcontactcount);
    
    detailFilter.setValue("00358401234565");
    error = QContactManager::NoError;
    cnt_ids = filter.contacts(detailFilter, QContactSortOrder(), filterSupported, &error);
    // check counts 
    seachedcontactcount = cnt_ids.count();
    expectedCount =1;  
    QVERIFY(expectedCount == seachedcontactcount);
    
    detailFilter.setValue("99533358401234566");
    error = QContactManager::NoError;
    cnt_ids = filter.contacts(detailFilter, QContactSortOrder(), filterSupported, &error);
    // check counts 
    seachedcontactcount = cnt_ids.count();
    expectedCount =1;  
    QVERIFY(expectedCount == seachedcontactcount);
    
    detailFilter.setValue("0561234567");
    error = QContactManager::NoError;
    cnt_ids = filter.contacts(detailFilter, QContactSortOrder(), filterSupported, &error);
    // check counts 
    seachedcontactcount = cnt_ids.count();
    expectedCount =0;  
    QVERIFY(expectedCount == seachedcontactcount);
    
    detailFilter.setValue("0561234568");
    error = QContactManager::NoError;
    cnt_ids = filter.contacts(detailFilter, QContactSortOrder(), filterSupported, &error);
    // check counts 
    seachedcontactcount = cnt_ids.count();
    expectedCount =0;  
    QVERIFY(expectedCount == seachedcontactcount);
    
    detailFilter.setValue("061234569");
    error = QContactManager::NoError;
    cnt_ids = filter.contacts(detailFilter, QContactSortOrder(), filterSupported, &error);
    // check counts 
    seachedcontactcount = cnt_ids.count();
    expectedCount =1;  
    QVERIFY(expectedCount == seachedcontactcount);
}

void TestMatchPhoneNumber::testBestMatchingStrategy3()
{
    setupBestMatchingContactsDb3();
    
    CntSymbianSrvConnection srvConnection(m_engine);
    CntDbInfo dbInfo(m_engine);
    bool filterSupported(true);
    
    QContactDetailFilter detailFilter;
    detailFilter.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldNumber);
    QList<QContactLocalId> cnt_ids;
    QContactManager::Error error;
    int seachedcontactcount;
    int expectedCount;
    
    CntFilterDetail filter(*m_database,srvConnection,dbInfo);
    filter.emulateBestMatching();
    
    detailFilter.setValue("1234561");
    error = QContactManager::NoError;
    cnt_ids = filter.contacts(detailFilter, QContactSortOrder(), filterSupported, &error);
    // check counts 
    seachedcontactcount = cnt_ids.count();
    expectedCount =2;  
    QVERIFY(expectedCount == seachedcontactcount);
    
    detailFilter.setValue("401234561");
    error = QContactManager::NoError;
    cnt_ids = filter.contacts(detailFilter, QContactSortOrder(), filterSupported, &error);
    // check counts 
    seachedcontactcount = cnt_ids.count();
    expectedCount =2;  
    QVERIFY(expectedCount == seachedcontactcount);
    
    detailFilter.setValue("358401234563");
    error = QContactManager::NoError;
    cnt_ids = filter.contacts(detailFilter, QContactSortOrder(), filterSupported, &error);
    // check counts 
    seachedcontactcount = cnt_ids.count();
    expectedCount =0;  
    QVERIFY(expectedCount == seachedcontactcount);
    
    detailFilter.setValue("401234564");
    error = QContactManager::NoError;
    cnt_ids = filter.contacts(detailFilter, QContactSortOrder(), filterSupported, &error);
    // check counts 
    seachedcontactcount = cnt_ids.count();
    expectedCount =1;  
    QVERIFY(expectedCount == seachedcontactcount);
    
    detailFilter.setValue("+358401234565");
    error = QContactManager::NoError;
    cnt_ids = filter.contacts(detailFilter, QContactSortOrder(), filterSupported, &error);
    // check counts 
    seachedcontactcount = cnt_ids.count();
    expectedCount =1;  
    QVERIFY(expectedCount == seachedcontactcount);
    
    detailFilter.setValue("+16101234566");
    error = QContactManager::NoError;
    cnt_ids = filter.contacts(detailFilter, QContactSortOrder(), filterSupported, &error);
    // check counts 
    seachedcontactcount = cnt_ids.count();
    expectedCount =1;  
    QVERIFY(expectedCount == seachedcontactcount);
    
    detailFilter.setValue("4567");
    error = QContactManager::NoError;
    cnt_ids = filter.contacts(detailFilter, QContactSortOrder(), filterSupported, &error);
    // check counts 
    seachedcontactcount = cnt_ids.count();
    expectedCount =1;  
    QVERIFY(expectedCount == seachedcontactcount);
    
    detailFilter.setValue("*4566");
    error = QContactManager::NoError;
    cnt_ids = filter.contacts(detailFilter, QContactSortOrder(), filterSupported, &error);
    // check counts 
    seachedcontactcount = cnt_ids.count();
    expectedCount =1;  
    QVERIFY(expectedCount == seachedcontactcount);
    
    detailFilter.setValue("*55*0705#");
    error = QContactManager::NoError;
    cnt_ids = filter.contacts(detailFilter, QContactSortOrder(), filterSupported, &error);
    // check counts 
    seachedcontactcount = cnt_ids.count();
    expectedCount =0;  
    QVERIFY(expectedCount == seachedcontactcount);
}

void TestMatchPhoneNumber::testBestMatchingWithUnionFilter()
{
    setupBestMatchingContactsDb2();
    
    CntSymbianSrvConnection srvConnection(m_engine);
    CntDbInfo dbInfo(m_engine);
    bool filterSupported(true);
    QContactManager::Error error;
    
    //Create first filter
    QContactDetailFilter f1;
    f1.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldNumber);
    f1.setValue("1234561");
    //Create second filter
    QContactDetailFilter f2;
    f2.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldNumber);
    f2.setValue("1234562");
    
    //Create an union filter with the above created filters 
    QList<QContactLocalId> cnt_ids;
    QContactUnionFilter qfilter;
    CntFilterUnion unionFilter(*m_database,srvConnection,dbInfo);
    unionFilter.emulateBestMatching();
    qfilter.append(f1);
    qfilter.append(f2);

    //Search for contacts 
    error = QContactManager::NoError;
    cnt_ids = unionFilter.contacts(qfilter, QContactSortOrder(), filterSupported, &error);
    // check counts 
    int seachedcontactcount = cnt_ids.count();
    int expectedCount =2;
    QVERIFY(expectedCount == seachedcontactcount);
}

void TestMatchPhoneNumber::testBestMatchingWithIntersectionFilter()
{
    setupBestMatchingContactsDb2();
    
    CntSymbianSrvConnection srvConnection(m_engine);
    CntDbInfo dbInfo(m_engine);
    bool filterSupported(true);
    QContactManager::Error error;
    
    //Create first filter
    QContactDetailFilter f1;
    f1.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldNumber);
    f1.setValue("00358401234561");
    //Create second filter
    QContactDetailFilter f2;
    f2.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldNumber);
    f2.setValue("99533358401234561");
    
    //Create an intersection filter with the above created filters 
    QList<QContactLocalId> cnt_ids;
    QContactIntersectionFilter qfilter;
    CntFilterIntersection intersectionFilter(*m_database,srvConnection,dbInfo);
    intersectionFilter.emulateBestMatching();
    qfilter.append(f1);
    qfilter.append(f2);

    //Search for contacts 
    error = QContactManager::NoError;
    cnt_ids = intersectionFilter.contacts(qfilter, QContactSortOrder(), filterSupported, &error);
    // check counts 
    int seachedcontactcount = cnt_ids.count();
    int expectedCount =1;
    QVERIFY(expectedCount == seachedcontactcount);
}
