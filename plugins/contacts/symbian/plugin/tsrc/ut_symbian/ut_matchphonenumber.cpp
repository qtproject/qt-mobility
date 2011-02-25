/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in 
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
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
const QString DB1_LASTNAME1 = "Jo";
const QString DB1_FIRSTNAME2 = "Di";
const QString DB1_LASTNAME2 = "Jo";


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
    addContact( DB1_FIRSTNAME1, DB1_LASTNAME1, QString( "34567" ) );
    
    QContactManager::Error err;
    QList<QContactLocalId> cnt_ids = m_engine->contactIds(QContactFilter(),QList<QContactSortOrder>(), &err);
    int j = cnt_ids.count();
    QVERIFY(1 == j);
}

void TestMatchPhoneNumber::setupBestMatchingContactsDb2()
{
    cleanupContactsDb();
    addContact( DB1_FIRSTNAME1, DB1_LASTNAME1, QString( "358401234567" ) );
    
    QContactManager::Error err;
    QList<QContactLocalId> cnt_ids = m_engine->contactIds(QContactFilter(),QList<QContactSortOrder>(), &err);
    int j = cnt_ids.count();
    QVERIFY(1 == j);
}

void TestMatchPhoneNumber::setupBestMatchingContactsDb3()
{
    cleanupContactsDb();
    addContact( DB1_FIRSTNAME1, DB1_LASTNAME1, QString( "3560 0123456" ) );
    
    QContactManager::Error err;
    QList<QContactLocalId> cnt_ids = m_engine->contactIds(QContactFilter(),QList<QContactSortOrder>(), &err);
    int j = cnt_ids.count();
    QVERIFY(1 == j);
}

void TestMatchPhoneNumber::setupBestMatchingContactsDb4()
{
    cleanupContactsDb();
    addContact( DB1_FIRSTNAME1, DB1_LASTNAME1, QString( "1234567" ) );
    
    QContactManager::Error err;
    QList<QContactLocalId> cnt_ids = m_engine->contactIds(QContactFilter(),QList<QContactSortOrder>(), &err);
    int j = cnt_ids.count();
    QVERIFY(1 == j);
}

void TestMatchPhoneNumber::setupBestMatchingContactsDb5()
{
    cleanupContactsDb();
    addContact( DB1_FIRSTNAME1, DB1_LASTNAME1, QString( "0000 0123456" ) );
    
    QContactManager::Error err;
    QList<QContactLocalId> cnt_ids = m_engine->contactIds(QContactFilter(),QList<QContactSortOrder>(), &err);
    int j = cnt_ids.count();
    QVERIFY(1 == j);
}

void TestMatchPhoneNumber::setupBestMatchingContactsDb6()
{
    cleanupContactsDb();
    addContact( DB1_FIRSTNAME1, DB1_LASTNAME1, QString( "020 7700 9001" ) );
    addContact( DB1_FIRSTNAME1, DB1_LASTNAME1, QString( "20 7700 90012" ) );
    addContact( DB1_FIRSTNAME1, DB1_LASTNAME1, QString( "020 7700 9081" ) );
    addContact( DB1_FIRSTNAME1, DB1_LASTNAME1, QString( "120 7700 9081" ) );
    addContact( DB1_FIRSTNAME1, DB1_LASTNAME1, QString( "9120 7700 9081" ) );
    
    QContactManager::Error err;
    QList<QContactLocalId> cnt_ids = m_engine->contactIds(QContactFilter(),QList<QContactSortOrder>(), &err);
    int j = cnt_ids.count();
    QVERIFY(5 == j);
}

void TestMatchPhoneNumber::setupBestMatchingContactsDb7()
{
    cleanupContactsDb();
    addContact( DB1_FIRSTNAME1, DB1_LASTNAME1, QString( "9999 9990 0999 999" ) );
    addContact( DB1_FIRSTNAME1, DB1_LASTNAME1, QString( "9000 0000 0000 000" ) );
    addContact( DB1_FIRSTNAME1, DB1_LASTNAME1, QString( "0000 0000 0000 000" ) );
    addContact( DB1_FIRSTNAME1, DB1_LASTNAME1, QString( "0000 0000 0000 009" ) );
    addContact( DB1_FIRSTNAME1, DB1_LASTNAME1, QString( "9 9000 000" ) );
    addContact( DB1_FIRSTNAME1, DB1_LASTNAME1, QString( "9000 000" ) );
    
    QContactManager::Error err;
    QList<QContactLocalId> cnt_ids = m_engine->contactIds(QContactFilter(),QList<QContactSortOrder>(), &err);
    int j = cnt_ids.count();
    QVERIFY(6 == j);
}

void TestMatchPhoneNumber::setupBestMatchingContactsDb8()
{
    cleanupContactsDb();
    addContact( DB1_FIRSTNAME1, DB1_LASTNAME1, QString( "443049607" ) );
    
    QContactManager::Error err;
    QList<QContactLocalId> cnt_ids = m_engine->contactIds(QContactFilter(),QList<QContactSortOrder>(), &err);
    int j = cnt_ids.count();
    QVERIFY(1 == j);
}

void TestMatchPhoneNumber::setupBestMatchingContactsDb9()
{
    cleanupContactsDb();
    addContact( DB1_FIRSTNAME1, DB1_LASTNAME1, QString( "0443049607" ) );
    
    QContactManager::Error err;
    QList<QContactLocalId> cnt_ids = m_engine->contactIds(QContactFilter(),QList<QContactSortOrder>(), &err);
    int j = cnt_ids.count();
    QVERIFY(1 == j);
}

void TestMatchPhoneNumber::setupBestMatchingContactsDb10()
{
    cleanupContactsDb();
    addContact( DB1_FIRSTNAME1, DB1_LASTNAME1, QString( "358443049607" ) );
    
    QContactManager::Error err;
    QList<QContactLocalId> cnt_ids = m_engine->contactIds(QContactFilter(),QList<QContactSortOrder>(), &err);
    int j = cnt_ids.count();
    QVERIFY(1 == j);
}

void TestMatchPhoneNumber::setupBestMatchingContactsDb11()
{
    cleanupContactsDb();
    addContact( DB1_FIRSTNAME1, DB1_LASTNAME1, QString( "443049607" ) );
    
    QContactManager::Error err;
    QList<QContactLocalId> cnt_ids = m_engine->contactIds(QContactFilter(),QList<QContactSortOrder>(), &err);
    int j = cnt_ids.count();
    QVERIFY(1 == j);
}

void TestMatchPhoneNumber::setupBestMatchingContactsDb12()
{
    cleanupContactsDb();
    addContact( DB1_FIRSTNAME1, DB1_LASTNAME1, QString( "0443049607" ) );
    
    QContactManager::Error err;
    QList<QContactLocalId> cnt_ids = m_engine->contactIds(QContactFilter(),QList<QContactSortOrder>(), &err);
    int j = cnt_ids.count();
    QVERIFY(1 == j);
}

void TestMatchPhoneNumber::setupBestMatchingContactsDb13()
{
    cleanupContactsDb();
    addContact( DB1_FIRSTNAME1, DB1_LASTNAME1, QString( "3049607" ) );
    
    QContactManager::Error err;
    QList<QContactLocalId> cnt_ids = m_engine->contactIds(QContactFilter(),QList<QContactSortOrder>(), &err);
    int j = cnt_ids.count();
    QVERIFY(1 == j);
}

void TestMatchPhoneNumber::setupBestMatchingContactsDb14()
{
    cleanupContactsDb();
    addContact( DB1_FIRSTNAME1, DB1_LASTNAME1, QString( "03049607" ) );
    
    QContactManager::Error err;
    QList<QContactLocalId> cnt_ids = m_engine->contactIds(QContactFilter(),QList<QContactSortOrder>(), &err);
    int j = cnt_ids.count();
    QVERIFY(1 == j);
}

void TestMatchPhoneNumber::setupBestMatchingContactsDb15()
{
    cleanupContactsDb();
    addContact( DB1_FIRSTNAME1, DB1_LASTNAME1, QString( "4443049607" ) );
    
    QContactManager::Error err;
    QList<QContactLocalId> cnt_ids = m_engine->contactIds(QContactFilter(),QList<QContactSortOrder>(), &err);
    int j = cnt_ids.count();
    QVERIFY(1 == j);
}

void TestMatchPhoneNumber::setupBestMatchingContactsDb16()
{
    cleanupContactsDb();
    addContact( DB1_FIRSTNAME1, DB1_LASTNAME1, QString( "584443049607" ) );
    
    QContactManager::Error err;
    QList<QContactLocalId> cnt_ids = m_engine->contactIds(QContactFilter(),QList<QContactSortOrder>(), &err);
    int j = cnt_ids.count();
    QVERIFY(1 == j);
}

void TestMatchPhoneNumber::setupBestMatchingContactsDb17()
{
    cleanupContactsDb();
    addContact( DB1_FIRSTNAME1, DB1_LASTNAME1, QString( "401234567" ) );
    
    QContactManager::Error err;
    QList<QContactLocalId> cnt_ids = m_engine->contactIds(QContactFilter(),QList<QContactSortOrder>(), &err);
    int j = cnt_ids.count();
    QVERIFY(1 == j);
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
    testBestMatching1();
    testBestMatching2();
    testBestMatching3();
    testBestMatching4();
    testBestMatching5();
    testBestMatching6();
    testBestMatching7();
    testBestMatching8();
    testBestMatching9();
    testBestMatching10();
    testBestMatching11();
    testBestMatching12();
    testBestMatching13();
    testBestMatching14();
    testBestMatching15();
    testBestMatching16();
    testBestMatching17();
    
    testBestMatchingWithUnionFilter();
    testBestMatchingWithIntersectionFilter();
}

void TestMatchPhoneNumber::testBestMatching1()
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
    expectedCount =0;  
    QVERIFY(expectedCount == seachedcontactcount);
    
    detailFilter.setValue("34567");
    error = QContactManager::NoError;
    cnt_ids = filter.contacts(detailFilter, QContactSortOrder(), filterSupported, &error);
    // check counts 
    seachedcontactcount = cnt_ids.count();
    expectedCount =1;  
    QVERIFY(expectedCount == seachedcontactcount);
}

void TestMatchPhoneNumber::testBestMatching2()
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
    
    detailFilter.setValue("358401234567");
    error = QContactManager::NoError;
    cnt_ids = filter.contacts(detailFilter, QContactSortOrder(), filterSupported, &error);
    // check counts 
    seachedcontactcount = cnt_ids.count();
    expectedCount =1;  
    QVERIFY(expectedCount == seachedcontactcount);
    
    detailFilter.setValue("34567");
    error = QContactManager::NoError;
    cnt_ids = filter.contacts(detailFilter, QContactSortOrder(), filterSupported, &error);
    // check counts 
    seachedcontactcount = cnt_ids.count();
    expectedCount =0;  
    QVERIFY(expectedCount == seachedcontactcount);
}

void TestMatchPhoneNumber::testBestMatching3()
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
    
    detailFilter.setValue("0000 0123456");
    error = QContactManager::NoError;
    cnt_ids = filter.contacts(detailFilter, QContactSortOrder(), filterSupported, &error);
    // check counts 
    seachedcontactcount = cnt_ids.count();
    expectedCount =1;  
    QVERIFY(expectedCount == seachedcontactcount);
    
    detailFilter.setValue("123456");
    error = QContactManager::NoError;
    cnt_ids = filter.contacts(detailFilter, QContactSortOrder(), filterSupported, &error);
    // check counts 
    seachedcontactcount = cnt_ids.count();
    expectedCount =1;  
    QVERIFY(expectedCount == seachedcontactcount);
}

void TestMatchPhoneNumber::testBestMatching4()
{
    setupBestMatchingContactsDb4();
    
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
    
    detailFilter.setValue("358401234567");
    error = QContactManager::NoError;
    cnt_ids = filter.contacts(detailFilter, QContactSortOrder(), filterSupported, &error);
    // check counts 
    seachedcontactcount = cnt_ids.count();
    expectedCount =1;  
    QVERIFY(expectedCount == seachedcontactcount);
    
    detailFilter.setValue("34567");
    error = QContactManager::NoError;
    cnt_ids = filter.contacts(detailFilter, QContactSortOrder(), filterSupported, &error);
    // check counts 
    seachedcontactcount = cnt_ids.count();
    expectedCount =0;  
    QVERIFY(expectedCount == seachedcontactcount);
}
void TestMatchPhoneNumber::testBestMatching5()
{
    setupBestMatchingContactsDb5();
    
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
    
    detailFilter.setValue("0123456");
    error = QContactManager::NoError;
    cnt_ids = filter.contacts(detailFilter, QContactSortOrder(), filterSupported, &error);
    // check counts 
    seachedcontactcount = cnt_ids.count();
    expectedCount =1;  
    QVERIFY(expectedCount == seachedcontactcount);
    
    detailFilter.setValue("123456");
    error = QContactManager::NoError;
    cnt_ids = filter.contacts(detailFilter, QContactSortOrder(), filterSupported, &error);
    // check counts 
    seachedcontactcount = cnt_ids.count();
    expectedCount =1;  
    QVERIFY(expectedCount == seachedcontactcount);
    
    detailFilter.setValue("3456");
    error = QContactManager::NoError;
    cnt_ids = filter.contacts(detailFilter, QContactSortOrder(), filterSupported, &error);
    // check counts 
    seachedcontactcount = cnt_ids.count();
    expectedCount =0;  
    QVERIFY(expectedCount == seachedcontactcount);
}

void TestMatchPhoneNumber::testBestMatching6()
{
    setupBestMatchingContactsDb6();
    
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
    
    detailFilter.setValue("020 7700 9001");
    error = QContactManager::NoError;
    cnt_ids = filter.contacts(detailFilter, QContactSortOrder(), filterSupported, &error);
    // check counts 
    seachedcontactcount = cnt_ids.count();
    expectedCount =1;  
    QVERIFY(expectedCount == seachedcontactcount);
    
    detailFilter.setValue("20 7700 90012");
    error = QContactManager::NoError;
    cnt_ids = filter.contacts(detailFilter, QContactSortOrder(), filterSupported, &error);
    // check counts 
    seachedcontactcount = cnt_ids.count();
    expectedCount =1;  
    QVERIFY(expectedCount == seachedcontactcount);
    
    detailFilter.setValue("020 7700 9081");
    error = QContactManager::NoError;
    cnt_ids = filter.contacts(detailFilter, QContactSortOrder(), filterSupported, &error);
    // check counts 
    seachedcontactcount = cnt_ids.count();
    expectedCount =3;  
    QVERIFY(expectedCount == seachedcontactcount);
    
    detailFilter.setValue("120 7700 9081");
    error = QContactManager::NoError;
    cnt_ids = filter.contacts(detailFilter, QContactSortOrder(), filterSupported, &error);
    // check counts 
    seachedcontactcount = cnt_ids.count();
    expectedCount =3;  
    QVERIFY(expectedCount == seachedcontactcount);
    
    detailFilter.setValue("9120 7700 9081");
    error = QContactManager::NoError;
    cnt_ids = filter.contacts(detailFilter, QContactSortOrder(), filterSupported, &error);
    // check counts 
    seachedcontactcount = cnt_ids.count();
    expectedCount =3;  
    QVERIFY(expectedCount == seachedcontactcount);
    
    detailFilter.setValue("20 7700 9081");
    error = QContactManager::NoError;
    cnt_ids = filter.contacts(detailFilter, QContactSortOrder(), filterSupported, &error);
    // check counts 
    seachedcontactcount = cnt_ids.count();
    expectedCount =3;  
    QVERIFY(expectedCount == seachedcontactcount);
}

void TestMatchPhoneNumber::testBestMatching7()
{
    setupBestMatchingContactsDb7();
    
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
    
    detailFilter.setValue("9999 9990 0999 999");
    error = QContactManager::NoError;
    cnt_ids = filter.contacts(detailFilter, QContactSortOrder(), filterSupported, &error);
    // check counts 
    seachedcontactcount = cnt_ids.count();
    expectedCount =1;  
    QVERIFY(expectedCount == seachedcontactcount);
    detailFilter.setValue("9999 9990 0999 999");
    error = QContactManager::NoError;
    cnt_ids = filter.contacts(detailFilter, QContactSortOrder(), filterSupported, &error);
    // check counts 
    seachedcontactcount = cnt_ids.count();
    expectedCount =1;  
    QVERIFY(expectedCount == seachedcontactcount);
    
    detailFilter.setValue("9000 0000 0000 000");
    error = QContactManager::NoError;
    cnt_ids = filter.contacts(detailFilter, QContactSortOrder(), filterSupported, &error);
    // check counts 
    seachedcontactcount = cnt_ids.count();
    expectedCount =2;  
    QVERIFY(expectedCount == seachedcontactcount);
    
    detailFilter.setValue("0000 0000 0000 000");
    error = QContactManager::NoError;
    cnt_ids = filter.contacts(detailFilter, QContactSortOrder(), filterSupported, &error);
    // check counts 
    seachedcontactcount = cnt_ids.count();
    expectedCount =2;  
    QVERIFY(expectedCount == seachedcontactcount);
    
    detailFilter.setValue("0000 0000 0000 009");
    error = QContactManager::NoError;
    cnt_ids = filter.contacts(detailFilter, QContactSortOrder(), filterSupported, &error);
    // check counts 
    seachedcontactcount = cnt_ids.count();
    expectedCount =1;  
    QVERIFY(expectedCount == seachedcontactcount);
    
    detailFilter.setValue("9 9000 000");
    error = QContactManager::NoError;
    cnt_ids = filter.contacts(detailFilter, QContactSortOrder(), filterSupported, &error);
    // check counts 
    seachedcontactcount = cnt_ids.count();
    expectedCount =2;  
    QVERIFY(expectedCount == seachedcontactcount);
    
    detailFilter.setValue("9000 000");
    error = QContactManager::NoError;
    cnt_ids = filter.contacts(detailFilter, QContactSortOrder(), filterSupported, &error);
    // check counts 
    seachedcontactcount = cnt_ids.count();
    expectedCount =2;  
    QVERIFY(expectedCount == seachedcontactcount);
    
    detailFilter.setValue("0000 000");
    error = QContactManager::NoError;
    cnt_ids = filter.contacts(detailFilter, QContactSortOrder(), filterSupported, &error);
    // check counts 
    seachedcontactcount = cnt_ids.count();
    expectedCount =2;  
    QVERIFY(expectedCount == seachedcontactcount);
}

void TestMatchPhoneNumber::testBestMatching8()
{
    setupBestMatchingContactsDb8();
    
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
    
    detailFilter.setValue("358443049607");
    error = QContactManager::NoError;
    cnt_ids = filter.contacts(detailFilter, QContactSortOrder(), filterSupported, &error);
    // check counts 
    seachedcontactcount = cnt_ids.count();
    expectedCount =1;  
    QVERIFY(expectedCount == seachedcontactcount);
}

void TestMatchPhoneNumber::testBestMatching9()
{
    setupBestMatchingContactsDb9();
    
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
    
    detailFilter.setValue("358443049607");
    error = QContactManager::NoError;
    cnt_ids = filter.contacts(detailFilter, QContactSortOrder(), filterSupported, &error);
    // check counts 
    seachedcontactcount = cnt_ids.count();
    expectedCount =1;  
    QVERIFY(expectedCount == seachedcontactcount);
}

void TestMatchPhoneNumber::testBestMatching10()
{
    setupBestMatchingContactsDb10();
    
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
    
    detailFilter.setValue("443049607");
    error = QContactManager::NoError;
    cnt_ids = filter.contacts(detailFilter, QContactSortOrder(), filterSupported, &error);
    // check counts 
    seachedcontactcount = cnt_ids.count();
    expectedCount =1;  
    QVERIFY(expectedCount == seachedcontactcount);
    
    detailFilter.setValue("0443049607");
    error = QContactManager::NoError;
    cnt_ids = filter.contacts(detailFilter, QContactSortOrder(), filterSupported, &error);
    // check counts 
    seachedcontactcount = cnt_ids.count();
    expectedCount =1;  
    QVERIFY(expectedCount == seachedcontactcount);
    
    detailFilter.setValue("3049607");
    error = QContactManager::NoError;
    cnt_ids = filter.contacts(detailFilter, QContactSortOrder(), filterSupported, &error);
    // check counts 
    seachedcontactcount = cnt_ids.count();
    expectedCount =1;  
    QVERIFY(expectedCount == seachedcontactcount);
    
    detailFilter.setValue("03049607");
    error = QContactManager::NoError;
    cnt_ids = filter.contacts(detailFilter, QContactSortOrder(), filterSupported, &error);
    // check counts 
    seachedcontactcount = cnt_ids.count();
    expectedCount =1;  
    QVERIFY(expectedCount == seachedcontactcount);
}

void TestMatchPhoneNumber::testBestMatching11()
{
    setupBestMatchingContactsDb11();
    
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
    
    detailFilter.setValue("0443049607");
    error = QContactManager::NoError;
    cnt_ids = filter.contacts(detailFilter, QContactSortOrder(), filterSupported, &error);
    // check counts 
    seachedcontactcount = cnt_ids.count();
    expectedCount =1;  
    QVERIFY(expectedCount == seachedcontactcount);
}

void TestMatchPhoneNumber::testBestMatching12()
{
    setupBestMatchingContactsDb12();
    
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
    
    detailFilter.setValue("443049607");
    error = QContactManager::NoError;
    cnt_ids = filter.contacts(detailFilter, QContactSortOrder(), filterSupported, &error);
    // check counts 
    seachedcontactcount = cnt_ids.count();
    expectedCount =1;  
    QVERIFY(expectedCount == seachedcontactcount);
}

void TestMatchPhoneNumber::testBestMatching13()
{
    setupBestMatchingContactsDb13();
    
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
    
    detailFilter.setValue("358443049607");
    error = QContactManager::NoError;
    cnt_ids = filter.contacts(detailFilter, QContactSortOrder(), filterSupported, &error);
    // check counts 
    seachedcontactcount = cnt_ids.count();
    expectedCount =1;  
    QVERIFY(expectedCount == seachedcontactcount);
}

void TestMatchPhoneNumber::testBestMatching14()
{
    setupBestMatchingContactsDb14();
    
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
    
    detailFilter.setValue("358443049607");
    error = QContactManager::NoError;
    cnt_ids = filter.contacts(detailFilter, QContactSortOrder(), filterSupported, &error);
    // check counts 
    seachedcontactcount = cnt_ids.count();
    expectedCount =1;  
    QVERIFY(expectedCount == seachedcontactcount);
    
    detailFilter.setValue("0358443049607");
    error = QContactManager::NoError;
    cnt_ids = filter.contacts(detailFilter, QContactSortOrder(), filterSupported, &error);
    // check counts 
    seachedcontactcount = cnt_ids.count();
    expectedCount =1;  
    QVERIFY(expectedCount == seachedcontactcount);
}

void TestMatchPhoneNumber::testBestMatching15()
{
    setupBestMatchingContactsDb15();
    
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
    
    detailFilter.setValue("3584443049607");
    error = QContactManager::NoError;
    cnt_ids = filter.contacts(detailFilter, QContactSortOrder(), filterSupported, &error);
    // check counts 
    seachedcontactcount = cnt_ids.count();
    expectedCount =1;  
    QVERIFY(expectedCount == seachedcontactcount);
}

void TestMatchPhoneNumber::testBestMatching16()
{
    setupBestMatchingContactsDb16();
    
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
    
    detailFilter.setValue("4443049607");
    error = QContactManager::NoError;
    cnt_ids = filter.contacts(detailFilter, QContactSortOrder(), filterSupported, &error);
    // check counts 
    seachedcontactcount = cnt_ids.count();
    expectedCount =1;  
    QVERIFY(expectedCount == seachedcontactcount);
}
void TestMatchPhoneNumber::testBestMatching17()
{
    setupBestMatchingContactsDb17();
    
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
    
    detailFilter.setValue("2041234567");
    error = QContactManager::NoError;
    cnt_ids = filter.contacts(detailFilter, QContactSortOrder(), filterSupported, &error);
    // check counts 
    seachedcontactcount = cnt_ids.count();
    expectedCount =0;  
    QVERIFY(expectedCount == seachedcontactcount);
}


void TestMatchPhoneNumber::testBestMatchingWithUnionFilter()
{
    setupBestMatchingContactsDb6();
    
    CntSymbianSrvConnection srvConnection(m_engine);
    CntDbInfo dbInfo(m_engine);
    bool filterSupported(true);
    QContactManager::Error error;
    
    //Create first filter
    QContactDetailFilter f1;
    f1.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldNumber);
    f1.setValue("02077009001");
    //Create second filter
    QContactDetailFilter f2;
    f2.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldNumber);
    f2.setValue("02077009081");
    
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
    int expectedCount =4;
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
    f1.setValue("358401234567");
    //Create second filter
    QContactDetailFilter f2;
    f2.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldNumber);
    f2.setValue("1234567");
    
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
