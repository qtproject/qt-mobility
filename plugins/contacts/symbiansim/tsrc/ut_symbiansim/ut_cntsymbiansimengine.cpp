/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "ut_cntsymbiansimengine.h"
#include "cntsymbiansimengine.h"

#include <qcontactdetailfilter.h>
#include <qcontactname.h>
#include <qcontactphonenumber.h>
#include <mmtsy_names.h>
#include <QtTest/QtTest>

void TestSymbianSimEngine::initTestCase()
{
    //called once before all test cases are executed
    
    //make sure that ADN contacts are cached on TSY side
    TRAPD(err, initializeAdnCacheL());
    QCOMPARE(err, KErrNone);
    
    //create sim engine
    QContactManager::Error error =  QContactManager::NoError;
    QMap<QString, QString> parameters;
    parameters.insert(KParameterKeySimStoreName, "ADN");

    m_engine = new CntSymbianSimEngine(parameters, &error);
    if (error == QContactManager::NoError) {
        removeAllContacts();
    }
    else {
        QSKIP("Error creating manager", SkipAll);
    }
}

void TestSymbianSimEngine::cleanupTestCase()
{
    removeAllContacts();
    
    m_etelStore.Close();
    m_etelPhone.Close();  
    m_etelServer.Close();
 
    delete m_engine;
    m_engine = NULL;
}

void TestSymbianSimEngine::init()
{
}

void TestSymbianSimEngine::cleanup()
{
    //called after every test case
    removeAllContacts();
}

void TestSymbianSimEngine::testSyncOperations()
{
    //create a new ADN contact with basic details
    QContact adnContact;
    QContactName name;
    QString nm("Vasya Pupkin");
    name.setCustomLabel(nm);
    adnContact.saveDetail(&name);
    QContactPhoneNumber number;
    QString nmbr("1234567");
    number.setNumber(nmbr);
    adnContact.saveDetail(&number);
    QContactManager::Error err = QContactManager::NoError;
    QVERIFY(m_engine->saveContact(&adnContact, &err));
    QCOMPARE(err, QContactManager::NoError);
    
    //fetch the contact
    QList<QContactLocalId> ids = m_engine->contactIds(QContactFilter(), QList<QContactSortOrder>(), &err);
    QCOMPARE(err, QContactManager::NoError);
    QCOMPARE(ids.count(), 1);
    QList<QContact> contacts = m_engine->contacts(QContactFilter(), QList<QContactSortOrder>(), QContactFetchHint(), &err);
    QCOMPARE(err, QContactManager::NoError);
    QCOMPARE(contacts.count(), 1);
    QContact savedContact = m_engine->contact(adnContact.localId(), QContactFetchHint(), &err);
    QCOMPARE(err, QContactManager::NoError);
    QList<QContactName> names = savedContact.details<QContactName>();
    QCOMPARE(names.count(), 1);
    QCOMPARE(names.at(0).customLabel(), nm);
    QList<QContactPhoneNumber> numbers = savedContact.details<QContactPhoneNumber>();
    QCOMPARE(numbers.count(), 1);
    QCOMPARE(numbers.at(0).number(), nmbr);
    
    //edit the contact
    QContactPhoneNumber newNumber = numbers.at(0);
    newNumber.setNumber("7654321");
    savedContact.saveDetail(&newNumber);
    QVERIFY(m_engine->saveContact(&savedContact, &err));
    QCOMPARE(err, QContactManager::NoError);
    ids = m_engine->contactIds(QContactFilter(), QList<QContactSortOrder>(), &err);
    QCOMPARE(err, QContactManager::NoError);
    QCOMPARE(ids.count(), 1);
    
    //delete the contact
    m_engine->removeContact(savedContact.localId(), &err);
    QCOMPARE(err, QContactManager::NoError);
    ids = m_engine->contactIds(QContactFilter(), QList<QContactSortOrder>(), &err);
    QCOMPARE(err, QContactManager::DoesNotExistError);
    QCOMPARE(ids.count(), 0);
}

void TestSymbianSimEngine::testHasFeature()
{
    QVERIFY(!m_engine->hasFeature(QContactManager::Groups, QContactType::TypeGroup));
}

void TestSymbianSimEngine::testIsFilterSupported()
{
    QContactDetailFilter filter;
    filter.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldNumber);
    filter.setMatchFlags(QContactFilter::MatchPhoneNumber);
    QVERIFY(m_engine->isFilterSupported(filter));
    QVERIFY(!m_engine->isFilterSupported(QContactDetailFilter()));
}

void TestSymbianSimEngine::testIsBestMatch()
{
    QVERIFY(CntSymbianSimPhoneNumberMatching::isBestMatchL(("1234567"), ("1234567")));
    QVERIFY(CntSymbianSimPhoneNumberMatching::isBestMatchL(("+358501234567"), ("0501234567")));
}

void TestSymbianSimEngine::testCntSymbianSimFactory()
{
    QMap<QString, QString> parameters;
    parameters.insert(KParameterKeySimStoreName, "ADN");
    QContactManager::Error error = QContactManager::NoError;
    CntSymbianSimFactory *factory = new CntSymbianSimFactory();
    QVERIFY(factory != NULL);
    QVERIFY(factory->managerName() == "symbiansim");
    QContactManagerEngine *engine = factory->engine(parameters, &error);
    QVERIFY(engine != NULL);
    QVERIFY(error == QContactManager::NoError);
    QVERIFY(static_cast<CntSymbianSimEngine*>(engine)->managerName() == "symbiansim");
    
    //use also QContactManager to check that plug-in is loaded
    QContactManager* manager = new QContactManager("symbiansim", parameters);
    QVERIFY(engine != NULL);
    QVERIFY(manager->managerName() == "symbiansim");
    
    delete manager;
    delete engine;
    delete factory; 
}

void TestSymbianSimEngine::removeAllContacts()
{
    if(m_engine) {
        QContactManager::Error err(QContactManager::NoError);
        QList<QContactLocalId> cnts_ids = m_engine->contactIds(QContactFilter(),
                QList<QContactSortOrder>(), &err);
        QVERIFY(err == QContactManager::NoError || err == QContactManager::DoesNotExistError);

        for(int i = 0; i < cnts_ids.count(); i++) {
            QVERIFY(m_engine->removeContact(cnts_ids[i], &err));
        }
    }
}

void TestSymbianSimEngine::initializeAdnCacheL()
{
    User::LeaveIfError(m_etelServer.Connect());
    User::LeaveIfError(m_etelServer.LoadPhoneModule(KMmTsyModuleName));
      
    RTelServer::TPhoneInfo info;
    User::LeaveIfError(m_etelServer.GetPhoneInfo(0, info));
    User::LeaveIfError(m_etelPhone.Open(m_etelServer, info.iName));

    RMobilePhoneBookStore::TMobilePhoneBookInfoV5 m_etelStoreInfo;
    RMobilePhoneBookStore::TMobilePhoneBookInfoV5Pckg m_etelStoreInfoPckg(m_etelStoreInfo);
    User::LeaveIfError(m_etelStore.Open(m_etelPhone, KETelIccAdnPhoneBook));
    TRequestStatus requestStatus;
    m_etelStore.GetInfo(requestStatus, m_etelStoreInfoPckg);
    User::WaitForRequest(requestStatus);
    qDebug() << "sim info err = " << requestStatus.Int();
    if (requestStatus.Int() == KErrNotReady) {
        qDebug() << "ADN cache is not ready -> wait";
        //wait a bit until contact cache is ready on TSY side
        QTest::qWait(10000); //10sec
        m_etelStore.GetInfo(requestStatus, m_etelStoreInfoPckg);
        User::WaitForRequest(requestStatus);
        qDebug() << "sim info err = " << requestStatus.Int();
    }
    
    User::LeaveIfError(requestStatus.Int());
}
