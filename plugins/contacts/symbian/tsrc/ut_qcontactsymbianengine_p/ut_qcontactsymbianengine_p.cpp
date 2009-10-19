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

#include "ut_qcontactsymbianengine_p.h"
#include "qcontactsymbianengine_p.h"
#include "qcontactchangeset.h"

#include <QtTest/QtTest>

void TestSymbianEngine::initTestCase()
{
    QContactManager::Error error;
    m_engine = new QContactSymbianEngineData(error);
    removeAllContacts();
}

void TestSymbianEngine::cleanupTestCase()
{
    removeAllContacts();
    delete m_engine;
}

void TestSymbianEngine::removeAllContacts()
{
    if(m_engine) {
        // Empty cnt database
        QContactManager::Error err = QContactManager::NoError;
        QList<QContactSortOrder> sortOrders;
        QContactChangeSet cs;
        QList<QContactLocalId> cnts_ids = m_engine->contacts(sortOrders, err);
        
        for(int i=0; i<cnts_ids.count(); i++) {
            m_engine->removeContact(cnts_ids[i], cs, err);
        }
    }
}

void TestSymbianEngine::testContactOperations()
{
    QContactManager::Error err = QContactManager::NoError;
    QContactChangeSet cs;
    
    /* Add contact */
    QString first("Alice");        
    QContact alice;
    
    QContactName aliceName;
    aliceName.setFirst(first);
    alice.saveDetail(&aliceName);
    
    // Save and verify contact has been saved
    QVERIFY(m_engine->addContact(alice, cs, err));
    QVERIFY(err == QContactManager::NoError);
    QVERIFY(alice.localId() != 0);
    
    /* Retrieve a contact */
    QContactLocalId aid = alice.localId();
    QContact c = m_engine->contact(aid, err);
    QVERIFY(&c != NULL);
    QVERIFY(c.localId() == aid);
    QVERIFY(err == QContactManager::NoError);
    
    /* Update contact */
    // Create and save a contact
    int details_before = c.details().count();
    QContactPhoneNumber number;
    number.setContexts("Home");
    number.setSubTypes("Mobile");
    number.setNumber("12345678");
    c.setPreferredDetail("call", number);
    c.saveDetail(&number);
    
    // update the contact
    QContactLocalId id = c.localId();
    QVERIFY(m_engine->updateContact(c, cs, err));
    QVERIFY(err == QContactManager::NoError);
    
    // Verify that contact has been updated
    QContact c1 = m_engine->contact(id, err);
    QVERIFY(c1.localId() == id);
    QVERIFY(c1.details().count() > details_before);
    QString str = c1.detail(QContactPhoneNumber::DefinitionName).definitionName();
    QVERIFY(str == QContactPhoneNumber::DefinitionName);

    // Fetch a non existent contact
    QContact d;
    QVERIFY(!m_engine->updateContact(d, cs, err));
    QVERIFY(err == QContactManager::DoesNotExistError);
    
    /* Remove contact */
    QContact e = m_engine->contact(c.localId(), err);
    id = e.localId();
    QVERIFY(m_engine->removeContact(id, cs, err));
    QVERIFY(err == QContactManager::NoError);
    
    // Verify that contact has been removed
    QContact f = m_engine->contact(id, err);
    QVERIFY(f.localId() == 0);
    QVERIFY(err == QContactManager::DoesNotExistError);
    
    // Remove non existent contact
    QVERIFY(!m_engine->removeContact(0, cs, err));
    QVERIFY(err == QContactManager::DoesNotExistError);   
}

void TestSymbianEngine::testGroupOperations()
{
    // XXX TODO
#if 0
    QContactManager::Error err = QContactManager::NoError;
    QContactChangeSet cs;
    
    /* Add group */
    QContactGroup g1;
    g1.setName("Group1");
    
    // save and verify a group has been saved
    QVERIFY(m_engine->addGroup(g1, cs, err));
    QVERIFY(g1.id() != 0);
    QVERIFY(err == QContactManager::NoError);
    
    /* Fetch group */
    // Fetch a non existing group
    QContactGroup g = m_engine->group(0, err);
    QVERIFY(g.id() == 0);
    QVERIFY(err == QContactManager::DoesNotExistError);
    
    // Fetch an existing group
    g = m_engine->group(g1.id(), err);
    QVERIFY(g.id() == g1.id());
    QVERIFY(err == QContactManager::NoError);
    
    /* Update group */
    // Create group
    QContactLocalId id(111);
    g.addMember(id);
    
    // Update and verify group has been added
    QVERIFY(m_engine->updateGroup(g, cs, err));    
    QContactGroup g2 = m_engine->group(g.id(), err);
    QVERIFY(g2.hasMember(id));
    
    // Update a non existing group
    QContactGroup g3;
    QVERIFY(!m_engine->updateGroup(g3, cs, err));
    QVERIFY(err == QContactManager::DoesNotExistError);
        
    /* Remove group */
    // Remove an existent group and verify
    QContactLocalId id2 = g.id();
    QVERIFY(m_engine->removeGroup(id2, cs, err));
    QVERIFY(err == QContactManager::NoError);
    
    // Remove a non existent group and verify
    QContactGroup g4 = m_engine->group(id2, err);
    QVERIFY(g4.id() == 0);
    QVERIFY(err == QContactManager::DoesNotExistError);
#endif
}

void TestSymbianEngine::testSelfContactOperations()
{
    QContactManager::Error err = QContactManager::NoError;
    QContactChangeSet cs;
    
    /* self contact */
    // Create a new contact
    QContact own;
    QString first("Alice");
    QString last("Jones");
    
    QContactName aliceName;
    aliceName.setFirst(first);
    aliceName.setLast(last);
    own.saveDetail(&aliceName);
    
    QString label("Ally Jones");
    QContactDisplayLabel aliceDisplay;
    aliceDisplay.setLabel(label);
    own.saveDetail(&aliceDisplay);
    m_engine->addContact(own, cs, err);
    
    // Set a non existent contact as self contact and verify
    // ensure this contact does not exist in dbase
    QContactLocalId id(12);
    bool exist = m_engine->removeContact(id, cs, err);
    
    QVERIFY(!m_engine->setSelfContactId(id, err)); // does not exist
    QVERIFY(err == QContactManager::DoesNotExistError);
 
    // Test a "0" contact id
    QVERIFY(!m_engine->setSelfContactId(0, err)); // does not exist
    QVERIFY(err == QContactManager::BadArgumentError);    
   
    // Set an existent contact as self contact and verify
    QVERIFY(m_engine->setSelfContactId(own.localId(), err));
    QVERIFY(err == QContactManager::NoError);
    
    // Fetch existing self contact
    QContactLocalId own_id = m_engine->selfContactId(err);
    QVERIFY(own_id == own.localId());
    
    // Remove self contact and verify
    m_engine->removeContact(own.localId(), cs, err);
    QContactLocalId idr = m_engine->selfContactId(err);
    QVERIFY(err == QContactManager::DoesNotExistError);
}

QTEST_MAIN(TestSymbianEngine);
