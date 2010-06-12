/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "ut_cntsymbianengine.h"
#include "cntsymbianengine.h"
#include "qcontactrelationship.h"
#include "qcontactrelationshipfilter.h"

#include <qcontactmanager.h>
#include <qcontactdetailfilter.h>
#include <qcontactorganization.h>
#include <qcontactemailaddress.h>
#include <qcontactguid.h>

#include <QtTest/QtTest>

void TestSymbianEngine::initTestCase()
{
    QContactManager::Error error;
    QMap<QString, QString> emptyParameters;

    m_engine = new CntSymbianEngine(emptyParameters, &error);
    if (error == QContactManager::NoError)
        removeAllContacts();
    else
        QSKIP("Error creating manager", SkipAll);
}

void TestSymbianEngine::cleanupTestCase()
{
    removeAllContacts();
    delete m_engine;
}

void TestSymbianEngine::init()
{
}

void TestSymbianEngine::cleanup()
{
    removeAllContacts();
}

void TestSymbianEngine::removeAllContacts()
{
    if(m_engine) {
        // Empty cnt database
        QContactManager::Error err(QContactManager::NoError);
        QList<QContactSortOrder> sortOrders;
        QContactFilter defaultFilter = QContactFilter();
        QList<QContactLocalId> cnts_ids = m_engine->contactIds(defaultFilter, sortOrders, &err);
        QVERIFY(err == QContactManager::NoError);

        for(int i=0; i<cnts_ids.count(); i++) {
            QVERIFY(m_engine->removeContact(cnts_ids[i], &err));
        }
    }
}

void TestSymbianEngine::ctors()
{
    QContactManager::Error error;
    QMap<QString, QString> params;

    // Ctor
    CntSymbianEngine *ce;
    ce = new CntSymbianEngine(params, &error);
    QVERIFY(ce != NULL);
    QVERIFY(error == QContactManager::NoError);
    if (error == QContactManager::NoError) {
        QVERIFY(ce->managerName() == CNT_SYMBIAN_MANAGER_NAME);
        QVERIFY(ce->m_contactFilter != 0);
        QVERIFY(ce->m_dataBase != 0);
        QVERIFY(ce->m_relationship != 0);
        QVERIFY(ce->m_transformContact != 0);
    } else {
        QSKIP("Error creating CntSymbianEngine in ctor", SkipSingle);
    }

    delete ce;
}

void TestSymbianEngine::saveContact()
{
    QContactManager::Error err;
    QList<QContactSortOrder> sortOrders;
    QContactId empty;
    QContactFilter defaultFilter = QContactFilter();

    int init_count = m_engine->contactIds(defaultFilter, sortOrders, &err).count();
    QVERIFY(err == QContactManager::NoError);

    // Save a "NULL" contact
    QVERIFY(!m_engine->saveContact(NULL, &err));
    QVERIFY(err == QContactManager::BadArgumentError);
    int current_count = m_engine->contactIds(defaultFilter, sortOrders, &err).count();
    QVERIFY(err == QContactManager::NoError);
    QVERIFY(init_count == current_count);

    // Save a contact that is not in database
    QContact invaId;
    QVERIFY(m_engine->saveContact(&invaId, &err));   // Add to db
    QVERIFY(err == QContactManager::NoError);
    QContactId cId = invaId.id();
    m_engine->removeContact(invaId.localId(), &err);   // Ensure not in db
    QVERIFY(err == QContactManager::NoError);
    invaId.setId(cId);
    QVERIFY(!m_engine->saveContact(&invaId, &err));   // Update non existent contact
    QVERIFY(err == QContactManager::DoesNotExistError);
    current_count = m_engine->contactIds(defaultFilter, sortOrders, &err).count();
    QVERIFY(err == QContactManager::NoError);
    QVERIFY(init_count == current_count);

    QContact alice;
    alice.setType("Jargon");

    // Save a "non contact(Jargon) type" contact
    QVERIFY(!m_engine->saveContact(&alice, &err));
    QVERIFY(err == QContactManager::InvalidDetailError);
    QVERIFY(alice.id() == empty);
    QVERIFY(alice.localId() == 0);
    current_count = m_engine->contactIds(defaultFilter, sortOrders, &err).count();
    QVERIFY(err == QContactManager::NoError);
    QVERIFY(init_count == current_count);

    // Save a valid contact
    alice.setType(QContactType::TypeContact);
    QVERIFY(m_engine->saveContact(&alice, &err));
    QVERIFY(err == QContactManager::NoError);
    QVERIFY(alice.id() != empty);
    QVERIFY(alice.localId() != 0);
    QString uri = QString(QLatin1String(CNT_SYMBIAN_MANAGER_NAME));
    QVERIFY(alice.id().managerUri().contains(uri, Qt::CaseInsensitive));
    current_count = m_engine->contactIds(defaultFilter, sortOrders, &err).count();
    QVERIFY(err == QContactManager::NoError);
    QVERIFY(init_count + 1 == current_count);
    
    // Save a valid contact
    QContact g;
    g.setType(QContactType::TypeGroup);
    QContactName en;
    en.setCustomLabel("ccc");
    QVERIFY(g.saveDetail(&en));
    QVERIFY(m_engine->saveContact(&g, &err));
    QVERIFY(err == QContactManager::NoError);
    QVERIFY(g.id() != empty);
    QVERIFY(g.localId() != 0);
    QVERIFY(g.id().managerUri().contains(uri, Qt::CaseInsensitive));
}

void TestSymbianEngine::saveContactWithPreferredDetails()
{
    QContactManager::Error err;

    //save a contact with preferred details
    QContact c;
    c.setType(QContactType::TypeContact);

    QContactPhoneNumber number1;
    number1.setNumber("123");
    number1.setSubTypes(QContactPhoneNumber::SubTypeMobile);
    c.saveDetail(&number1);
    c.setPreferredDetail("call", number1);

    QContactPhoneNumber number2;
    number2.setNumber("456");
    number2.setSubTypes(QContactPhoneNumber::SubTypeMobile);
    c.saveDetail(&number2);
    c.setPreferredDetail("videocall", number2);

    QContactPhoneNumber number3;
    number3.setNumber("789");
    number3.setSubTypes(QContactPhoneNumber::SubTypeMobile);
    c.saveDetail(&number3);
    c.setPreferredDetail("message", number3);

    QContactEmailAddress email;
    email.setEmailAddress("dummyemail");
    c.saveDetail(&email);
    c.setPreferredDetail("email", email);

    QVERIFY(m_engine->saveContact(&c, &err));
    QVERIFY(err == QContactManager::NoError);

    //fetch the saved contact and check preferred details
    QContactFetchHint hint = QContactFetchHint();
    QContact fetched = m_engine->contact(c.localId(), hint, &err);
    QVERIFY(err == QContactManager::NoError);

    QContactDetail callDetail1 = fetched.preferredDetail("call");
    QVERIFY(callDetail1.definitionName() == QContactPhoneNumber::DefinitionName);
    QContactPhoneNumber fetchedNumber1 = static_cast<QContactPhoneNumber>(callDetail1);
    QVERIFY(fetchedNumber1.number() == "123");

    QContactDetail callDetail2 = fetched.preferredDetail("videocall");
    QVERIFY(callDetail2.definitionName() == QContactPhoneNumber::DefinitionName);
    QContactPhoneNumber fetchedNumber2 = static_cast<QContactPhoneNumber>(callDetail2);
    QVERIFY(fetchedNumber2.number() == "456");

    QContactDetail callDetail3 = fetched.preferredDetail("message");
    QVERIFY(callDetail3.definitionName() == QContactPhoneNumber::DefinitionName);
    QContactPhoneNumber fetchedNumber3 = static_cast<QContactPhoneNumber>(callDetail3);
    QVERIFY(fetchedNumber3.number() == "789");

    QContactDetail emailDetail = fetched.preferredDetail("email");
    QVERIFY(emailDetail.definitionName() == QContactEmailAddress::DefinitionName);
    QContactEmailAddress fetchedEmail = static_cast<QContactEmailAddress>(emailDetail);
    QVERIFY(fetchedEmail.emailAddress() == "dummyemail");

    //save a contact with one preferred details for several actions
    QContact c2;
    c2.setType(QContactType::TypeContact);
    c2.saveDetail(&number1);
    c2.setPreferredDetail("call", number1);
    c2.setPreferredDetail("videocall", number1);
    c2.setPreferredDetail("message", number1);

    QVERIFY(m_engine->saveContact(&c2, &err));
    QVERIFY(err == QContactManager::NoError);

    //fetch the saved contact and check preferred details
    QContact fetched2 = m_engine->contact(c2.localId(), hint, &err);
    QVERIFY(err == QContactManager::NoError);

    QContactDetail callDetail4 = fetched2.preferredDetail("call");
    QVERIFY(callDetail4.definitionName() == QContactPhoneNumber::DefinitionName);
    QContactPhoneNumber fetchedNumber4 = static_cast<QContactPhoneNumber>(callDetail4);
    QVERIFY(fetchedNumber4.number() == "123");

    QContactDetail callDetail5 = fetched2.preferredDetail("videocall");
    QVERIFY(callDetail5.definitionName() == QContactPhoneNumber::DefinitionName);
    QContactPhoneNumber fetchedNumber5 = static_cast<QContactPhoneNumber>(callDetail5);
    QVERIFY(fetchedNumber5.number() == "123");

    QContactDetail callDetail6 = fetched2.preferredDetail("message");
    QVERIFY(callDetail6.definitionName() == QContactPhoneNumber::DefinitionName);
    QContactPhoneNumber fetchedNumber6 = static_cast<QContactPhoneNumber>(callDetail6);
    QVERIFY(fetchedNumber6.number() == "123");
}

void TestSymbianEngine::saveContacts()
{
    QContactManager::Error err;
    QContactFilter defaultFilter = QContactFilter();
    QList<QContactSortOrder> sortOrders;
    QList<QContact> contacts;
    QContactId empty;
    int count = 5;

    int init_count = m_engine->contactIds(defaultFilter, sortOrders, &err).count();
    QVERIFY(err == QContactManager::NoError);

    // NULL
    QMap<int, QContactManager::Error> errorMap;
    QVERIFY(!m_engine->saveContacts(NULL, &errorMap, &err));
    QVERIFY(errorMap.count() == 0);
    QVERIFY(err == QContactManager::BadArgumentError);
    int current_count = m_engine->contactIds(defaultFilter, sortOrders, &err).count();
    QVERIFY(err == QContactManager::NoError);
    QVERIFY(init_count == current_count);

    // Save a "non contact(Jargon) type" contacts
    for(int i=0; i<count; i++) {
        QContact alice;
        alice.setType("Jargon");
        contacts.append(alice);
    }
    QVERIFY(!m_engine->saveContacts(&contacts, &errorMap, &err));
    QVERIFY(err == QContactManager::InvalidDetailError);
    foreach(QContactManager::Error err, errorMap) {
        QVERIFY(err == QContactManager::InvalidDetailError);
    }
    foreach(const QContact& c, contacts) {
        QVERIFY(c.id() == empty);
        QVERIFY(c.localId() == 0);
    }
    current_count = m_engine->contactIds(defaultFilter, sortOrders, &err).count();
    QVERIFY(err == QContactManager::NoError);
    QVERIFY(init_count == current_count);
    contacts.clear();

    // Save valid contacts
    for(int i=0; i<count; i++) {
        QContact alice;
        alice.setType(QContactType::TypeContact);
        contacts.append(alice);
    }
    QVERIFY(m_engine->saveContacts(&contacts, &errorMap, &err));
    QVERIFY(err == QContactManager::NoError);
    foreach(QContactManager::Error err, errorMap) {
        QVERIFY(err == QContactManager::NoError);
    }
    QString uri = QString(QLatin1String(CNT_SYMBIAN_MANAGER_NAME));
    foreach(const QContact& c, contacts) {
        QVERIFY(c.id() != empty);
        QVERIFY(c.localId() != 0);
        QVERIFY(c.id().managerUri().contains(uri, Qt::CaseInsensitive));
    }
    current_count = m_engine->contactIds(defaultFilter, sortOrders, &err).count();
    QVERIFY(err == QContactManager::NoError);
    QVERIFY(init_count + count == current_count);
    contacts.clear();

    // Save with one invalid contact in list
    init_count = m_engine->contactIds(defaultFilter, sortOrders, &err).count();
    for(int i=0; i<count; i++) {
        QContact alice;
        alice.setType(QContactType::TypeContact);
        contacts.append(alice);
    }
    QContact invalid;
    invalid.setType("Jasdfasd");
    contacts.insert(3, invalid);

    QVERIFY(!m_engine->saveContacts(&contacts, &errorMap, &err));
    QVERIFY(err == QContactManager::InvalidDetailError);
    foreach(QContactManager::Error err, errorMap) {
        QVERIFY(err == QContactManager::InvalidDetailError);
    }

    for(int i=0; i<contacts.count(); i++) {
        QContact c = contacts[i];
        if (i == 3) {
            QVERIFY(c.id() == empty);
            QVERIFY(c.localId() == 0);
        } else {
            QVERIFY(c.id() != empty);
            QVERIFY(c.localId() != 0);
            QVERIFY(c.id().managerUri().contains(uri, Qt::CaseInsensitive));
        }
    }
    current_count = m_engine->contactIds(defaultFilter, sortOrders, &err).count();
    QVERIFY(err == QContactManager::NoError);
    QVERIFY(init_count + count == current_count);
    contacts.clear();
}

void TestSymbianEngine::retrieveContact()
{
    QContactManager::Error err;
    QContactFetchHint hint = QContactFetchHint();

    QContact alice;
    alice.setType(QContactType::TypeContact);
    QVERIFY(m_engine->saveContact(&alice, &err));
    QVERIFY(err == QContactManager::NoError);

    // Retrieve "non contact"
    QContact c = m_engine->contact(0, hint, &err);
    QVERIFY(&c != NULL);
    QVERIFY(c.localId() == 0);
    QVERIFY(err == QContactManager::DoesNotExistError);

    // Retrieve valid existing contact
    QContactLocalId aid = alice.localId();
    c = m_engine->contact(aid, hint, &err);
    QVERIFY(&c != NULL);
    QVERIFY(c.localId() == aid);
    QVERIFY(err == QContactManager::NoError);
}

void TestSymbianEngine::retrieveContacts()
{
    QContactManager::Error err;
    QContactFilter f;
    QContactFilter defaultFilter = QContactFilter();
    QContactFetchHint hint = QContactFetchHint();
    QList<QContactSortOrder> s;
    QList<QContactLocalId> cnt_ids;

    QContact c;
    c.setType(QContactType::TypeContact);
    QContactName cn;
    cn.setFirstName("aaa");
    QVERIFY(c.saveDetail(&cn));

    QContactPhoneNumber number;
    number.setContexts("Home");
    number.setSubTypes("Mobile");
    number.setNumber("12345678");
    QVERIFY(c.saveDetail(&number));
    QVERIFY(m_engine->saveContact(&c, &err));
    QVERIFY(err == QContactManager::NoError);

    QContact d;
    d.setType(QContactType::TypeContact);
    QContactName dn;
    dn.setFirstName("bbb");
    QVERIFY(d.saveDetail(&dn));
    QVERIFY(m_engine->saveContact(&d, &err));
    QVERIFY(err == QContactManager::NoError);

    QContact e;
    e.setType(QContactType::TypeGroup);
    QContactName en;
    en.setCustomLabel("ccc");
    QVERIFY(e.saveDetail(&en));
    QVERIFY(m_engine->saveContact(&e, &err));
    QVERIFY(err == QContactManager::NoError);

    // Retrieve all contacts
    cnt_ids = m_engine->contactIds(f, s, &err);
    QVERIFY(err == QContactManager::NoError);
    QVERIFY(cnt_ids.count() > 0);

    QContactDetailFilter mobileFilter;
    mobileFilter.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldSubTypes);
    mobileFilter.setValue(QLatin1String(QContactPhoneNumber::SubTypeMobile));

    // Retrieve contacts with mobile number
    cnt_ids = m_engine->contactIds(mobileFilter, s, &err);
    QVERIFY(err == QContactManager::NoError);
    QVERIFY(cnt_ids.count() > 0);
    
    // Slow filter
    QList<QContactLocalId> fast_ids = m_engine->contactIds(mobileFilter, s, &err);
    QList<QContactLocalId> all_ids = m_engine->contactIds(f, s, &err);
    QList<QContactLocalId> slow_ids = m_engine->slowFilter(mobileFilter, all_ids, &err);
    QVERIFY(err == QContactManager::NoError);
    QVERIFY(slow_ids.count() == fast_ids.count());

    QContactDetailFilter invalidFilter;
    mobileFilter.setDetailDefinitionName("asfdasdf", "asdfasdf");

    // Retrieve contacts with invalid filter
    cnt_ids = m_engine->contactIds(invalidFilter, s, &err);
    QVERIFY(err == QContactManager::NotSupportedError);

    // Retrieve sorted contacts
    QContactSortOrder sortOrder;
    QList<QContactSortOrder> s1;
    sortOrder.setDetailDefinitionName(QContactName::DefinitionName,  QContactName::FieldFirstName);
    sortOrder.setBlankPolicy(QContactSortOrder::BlanksLast);
    sortOrder.setDirection(Qt::AscendingOrder);
    sortOrder.setCaseSensitivity(Qt::CaseInsensitive);
    s1.append(sortOrder);

    cnt_ids = m_engine->contactIds(defaultFilter, s1, &err);
    QVERIFY(err == QContactManager::NoError);

    // Retrieve with invalid sort order
    QContactSortOrder sortOrder1;
    QList<QContactSortOrder> s2;
    sortOrder1.setDetailDefinitionName("asdfasdf", "asdfasd");

    cnt_ids = m_engine->contactIds(defaultFilter, s2, &err);
    QVERIFY(err == QContactManager::NoError);
    
    // Retrieve full contacts (with all details)
    QList<QContact> contactList;
    QList<QContactSortOrder> sortOrders;
    QStringList definitionRestrictions;
    contactList = m_engine->contacts(defaultFilter, sortOrders, hint, &err);
    QVERIFY(err == QContactManager::NoError);
    QContactFilter filter;
    contactList = m_engine->contacts(filter, sortOrders, hint, &err);
    QVERIFY(err == QContactManager::NoError);
}

void TestSymbianEngine::retrieveName()
{
    QContactManager::Error err;
    QContactFetchHint hint = QContactFetchHint();
    QStringList hints;
    hints << QContactDisplayLabel::DefinitionName;
    hint.setDetailDefinitionsHint(hints);

    QContact alice;
    alice.setType(QContactType::TypeContact);
    QContactName name;
    name.setFirstName("Alice");
    name.setLastName("Wonders");
    alice.saveDetail(&name);
    QVERIFY(m_engine->saveContact(&alice, &err));
    QVERIFY(err == QContactManager::NoError);

    // Retrieve name of "non contact"
    QContact c = m_engine->contact(0, hint, &err);
    QVERIFY(&c != NULL);
    QVERIFY(c.localId() == 0);
    QVERIFY(err == QContactManager::DoesNotExistError);

    // Retrieve name of valid existing contact
    QContactLocalId aid = alice.localId();
    c = m_engine->contact(aid, hint, &err);
    QVERIFY(&c != NULL);
    QVERIFY(c.localId() == aid);
    QVERIFY(c.displayLabel() == alice.displayLabel());
    QVERIFY(err == QContactManager::NoError);
}

void TestSymbianEngine::retrieveNames()
{
    QContactManager::Error err;
    QContactDetailFilter filter;
    filter.setDetailDefinitionName(QContactType::DefinitionName);
    filter.setValue(QContactType::TypeContact);

    QList<QContactSortOrder> sortOrders;

    QContactFetchHint hint = QContactFetchHint();
    QStringList hints;
    hints << QContactDisplayLabel::DefinitionName;
    hint.setDetailDefinitionsHint(hints);

    QContact alice;
    alice.setType(QContactType::TypeContact);
    QContactName name;
    name.setFirstName("Alice");
    name.setLastName("Wonders");
    alice.saveDetail(&name);
    QVERIFY(m_engine->saveContact(&alice, &err));
    QVERIFY(err == QContactManager::NoError);

    QContact charlie;
    charlie.setType(QContactType::TypeContact);
    name.setFirstName("Charlie");
    name.setLastName("Choco");
    charlie.saveDetail(&name);
    QVERIFY(m_engine->saveContact(&charlie, &err));
    QVERIFY(err == QContactManager::NoError);

    int numContacts = m_engine->contactIds(filter, sortOrders, &err).count();

    // Retrieve names
    QList<QContact> contacts = m_engine->contacts(filter, sortOrders, hint, &err);
    QVERIFY(contacts.count() == numContacts);

    int verifiedContacts = 0;
    foreach (QContact contact, contacts) {
        if (contact.localId() == alice.localId()) {
            TInt contactId = contact.localId();
            TInt aliceId = alice.localId();
            QString contactName = contact.displayLabel();
            QString aliceName = alice.displayLabel();
            QVERIFY(contact.displayLabel() == alice.displayLabel());  
            ++verifiedContacts;
        }
        if (contact.localId() == charlie.localId()) {
            QVERIFY(contact.displayLabel() == charlie.displayLabel());
            ++verifiedContacts;
        }
    }

    QVERIFY(verifiedContacts == 2);
}

void TestSymbianEngine::updateContact()
{
    QContactManager::Error err;
    QContactFetchHint hint = QContactFetchHint();
    QContact c;

    QVERIFY(m_engine->saveContact(&c, &err));
    QVERIFY(err == QContactManager::NoError);

    int details_before = c.details().count();

    QContactName aliceName;
    aliceName.setFirstName("Alice");
    c.saveDetail(&aliceName);

    QContactPhoneNumber number;
    number.setContexts("Home");
    number.setSubTypes("Mobile");
    number.setNumber("12345678");
    c.saveDetail(&number);

    // update the contact
    QContactLocalId id = c.localId();
    QVERIFY(m_engine->saveContact(&c, &err));
    QVERIFY(err == QContactManager::NoError);

    // Verify that contact has been updated
    QContact d = m_engine->contact(id, hint, &err);
    QVERIFY(err == QContactManager::NoError);
    QVERIFY(d.localId() == id);
    QVERIFY(d.details().count() > details_before);
    QString str = d.detail(QContactPhoneNumber::DefinitionName).definitionName();
    QVERIFY(str == QContactPhoneNumber::DefinitionName);
}

void TestSymbianEngine::updateContactByUid()
{
    QContactManager::Error err;
    QContact c;

    QVERIFY(m_engine->saveContact(&c, &err));
    QVERIFY(err == QContactManager::NoError);
    QContactLocalId initialId = c.localId();
    
    QContactGuid guidDetail = static_cast<QContactGuid>(c.details(QContactGuid::DefinitionName).at(0));
    QString uid = guidDetail.guid();
    
    //clear id
    QScopedPointer<QContactId> contactId(new QContactId());
    contactId->setLocalId(0);
    contactId->setManagerUri(QString());
    c.setId(*contactId);
    
    // update the contact   
    QContactName aliceName;
    aliceName.setFirstName("Alice");
    c.saveDetail(&aliceName);
    QContactPhoneNumber number;
    number.setContexts("Home");
    number.setSubTypes("Mobile");
    number.setNumber("12345678");
    c.saveDetail(&number);

    //verify that the same contact was updated
    QVERIFY(m_engine->saveContact(&c, &err));
    QContactLocalId id = c.localId();
    QVERIFY(err == QContactManager::NoError);
    QVERIFY(initialId == id);
}

void TestSymbianEngine::removeContact()
{
    QContactManager::Error err;
    QContactFetchHint hint = QContactFetchHint();

    QContact c;
    c.setType(QContactType::TypeContact);
    QVERIFY(m_engine->saveContact(&c, &err));
    QVERIFY(err == QContactManager::NoError);

    // Remove existing contact
    QContactLocalId id = c.localId();
    QVERIFY(m_engine->removeContact(id, &err));
    QVERIFY(err == QContactManager::NoError);

    // Verify that contact has been removed
    QContact f = m_engine->contact(id, hint, &err);
    QVERIFY(f.localId() == 0);
    QVERIFY(err == QContactManager::DoesNotExistError);

    // Remove non existent contact
    QVERIFY(!m_engine->removeContact(0, &err));
    QVERIFY(err == QContactManager::DoesNotExistError);
}

void TestSymbianEngine::removeContacts()
{
    QContactManager::Error err;
    QContactFetchHint hint = QContactFetchHint();
    QList<QContactLocalId> contacts;
    int count = 5;

    // Remove non existent contacts
    QMap<int, QContactManager::Error> errorMap;
    QVERIFY(!m_engine->removeContacts(contacts, &errorMap, &err));
    QVERIFY(err == QContactManager::BadArgumentError);
    QVERIFY(errorMap.count() == 0);

    // Remove existing contacts
    for(int i=0; i<count; i++) {
        QContact c;
        c.setType(QContactType::TypeContact);
        QVERIFY(m_engine->saveContact(&c, &err));
        QVERIFY(err == QContactManager::NoError);
        contacts.append(c.localId());
    }
    QVERIFY(m_engine->removeContacts(contacts, &errorMap, &err));
    QVERIFY(err == QContactManager::NoError);
    foreach(QContactManager::Error e, errorMap) {
        QVERIFY(e == QContactManager::NoError);
    }

    // Verify that contacts have been removed
    for(int i=0; i<contacts.count(); i++) {
        QContact f = m_engine->contact(contacts[i], hint, &err);
        QVERIFY(f.localId() == 0);
        QVERIFY(err == QContactManager::DoesNotExistError);
    }

    // Remove a list with one non existent contact
    contacts.clear();
    for(int i=0; i<count; i++) {
        QContact c;
        c.setType(QContactType::TypeContact);
        QVERIFY(m_engine->saveContact(&c, &err));
        QVERIFY(err == QContactManager::NoError);
        contacts.append(c.localId());
    }
    contacts.insert(3, 0);

    QVERIFY(!m_engine->removeContacts(contacts, &errorMap, &err));
    QVERIFY(err == QContactManager::DoesNotExistError);
    foreach(QContactManager::Error e, errorMap) {
        QVERIFY(e == QContactManager::DoesNotExistError);
    }

    for(int i=0; i<contacts.count(); i++) {
        QContact f = m_engine->contact(contacts[i], hint, &err);
        QVERIFY(f.localId() == 0);
        QVERIFY(err == QContactManager::DoesNotExistError);
    }
}

void TestSymbianEngine::addOwnCard()
{
    QContactManager::Error err;

    // Create a new contact own card
    QContact own;
    QContactName ownName;
    ownName.setFirstName("Own");
    own.saveDetail(&ownName);
    QVERIFY(m_engine->saveContact(&own, &err));
    QVERIFY(err == QContactManager::NoError);

    // Set a non existent contact as own card and verify
    // ensure this contact does not exist in dbase
    QContactLocalId id(12);
    m_engine->removeContact(id, &err);   // Don't test err. May or may not be in dbase
    QVERIFY(!m_engine->setSelfContactId(id, &err)); // does not exist
    QVERIFY(err == QContactManager::DoesNotExistError);

    // Test a "0" contact id
    QVERIFY(!m_engine->setSelfContactId(0, &err)); // Bad argument
    QVERIFY(err == QContactManager::BadArgumentError);

    // Set an existent contact as own card
    QVERIFY(m_engine->setSelfContactId(own.localId(), &err));
    QVERIFY(err == QContactManager::NoError);
}

void TestSymbianEngine::retrieveOwnCard()
{
    QContactManager::Error err;

    // Create a new contact own card
    QContact own;
    QContactName ownName;
    ownName.setFirstName("Own");
    own.saveDetail(&ownName);
    QVERIFY(m_engine->saveContact(&own, &err));
    QVERIFY(err == QContactManager::NoError);
    QVERIFY(m_engine->setSelfContactId(own.localId(), &err));
    QVERIFY(err == QContactManager::NoError);

    // Fetch existing self contact
    QContactLocalId own_id = m_engine->selfContactId(&err);
    QVERIFY(err == QContactManager::NoError);
    QVERIFY(own_id == own.localId());

    // Remove self contact and verify
    QVERIFY(m_engine->removeContact(own.localId(), &err));
    QContactLocalId idr = m_engine->selfContactId(&err);
    QVERIFY(err == QContactManager::DoesNotExistError);
    QVERIFY(idr == 0);
}

void TestSymbianEngine::filterSupport()
{
    // Filter feature support
    QContactDetailFilter df;
    df.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldSubTypes);
    QVERIFY(m_engine->isFilterSupported(df));
    QContactFilter f;
    QVERIFY(m_engine->isFilterSupported(f));
}

void TestSymbianEngine::featureSupport()
{
    //hasFeature(QContactManager::ManagerFeature feature, const QString& contactType)

    // test with illegal feature enum value
    const int illegalFeature(198);
    QContactManager::ManagerFeature f = static_cast<QContactManager::ManagerFeature>(illegalFeature);
    QVERIFY(!m_engine->hasFeature(f, QContactType::TypeContact));
    QVERIFY(!m_engine->hasFeature(f, QContactType::TypeGroup));

    // empty contact type param
    QVERIFY(!m_engine->hasFeature(QContactManager::Groups, ""));
    QVERIFY(!m_engine->hasFeature(QContactManager::ActionPreferences, ""));
    QVERIFY(!m_engine->hasFeature(QContactManager::MutableDefinitions, ""));
    QVERIFY(!m_engine->hasFeature(QContactManager::Relationships, ""));
    QVERIFY(!m_engine->hasFeature(QContactManager::ArbitraryRelationshipTypes, ""));
    QVERIFY(!m_engine->hasFeature(QContactManager::SelfContact, ""));
    QVERIFY(!m_engine->hasFeature(QContactManager::Anonymous, ""));
    QVERIFY(!m_engine->hasFeature(QContactManager::ChangeLogs, ""));

    // wrong contact type param
    QVERIFY(!m_engine->hasFeature(QContactManager::Groups, "aserasdf"));
    QVERIFY(!m_engine->hasFeature(QContactManager::ActionPreferences, "aserasdf"));
    QVERIFY(!m_engine->hasFeature(QContactManager::MutableDefinitions, "aserasdf"));
    QVERIFY(!m_engine->hasFeature(QContactManager::Relationships, "aserasdf"));
    QVERIFY(!m_engine->hasFeature(QContactManager::ArbitraryRelationshipTypes, "aserasdf"));
    QVERIFY(!m_engine->hasFeature(QContactManager::SelfContact, "aserasdf"));
    QVERIFY(!m_engine->hasFeature(QContactManager::Anonymous, "aserasdf"));
    QVERIFY(!m_engine->hasFeature(QContactManager::ChangeLogs, "aserasdf"));

    // TypeContact contact type param
    QVERIFY(m_engine->hasFeature(QContactManager::Groups, QContactType::TypeContact));
    QVERIFY(!m_engine->hasFeature(QContactManager::ActionPreferences, QContactType::TypeContact));
    QVERIFY(!m_engine->hasFeature(QContactManager::MutableDefinitions, QContactType::TypeContact));
    QVERIFY(m_engine->hasFeature(QContactManager::Relationships, QContactType::TypeContact));
    QVERIFY(!m_engine->hasFeature(QContactManager::ArbitraryRelationshipTypes, QContactType::TypeContact));
    QVERIFY(m_engine->hasFeature(QContactManager::SelfContact, QContactType::TypeContact));
    QVERIFY(!m_engine->hasFeature(QContactManager::Anonymous, QContactType::TypeContact));
    QVERIFY(!m_engine->hasFeature(QContactManager::ChangeLogs, QContactType::TypeContact));

    // TypeGroup contact type param
    QVERIFY(m_engine->hasFeature(QContactManager::Groups, QContactType::TypeGroup));
    QVERIFY(!m_engine->hasFeature(QContactManager::ActionPreferences, QContactType::TypeGroup));
    QVERIFY(!m_engine->hasFeature(QContactManager::MutableDefinitions, QContactType::TypeGroup));
    QVERIFY(m_engine->hasFeature(QContactManager::Relationships, QContactType::TypeGroup));
    QVERIFY(!m_engine->hasFeature(QContactManager::ArbitraryRelationshipTypes, QContactType::TypeGroup));
    QVERIFY(m_engine->hasFeature(QContactManager::SelfContact, QContactType::TypeGroup));
    QVERIFY(!m_engine->hasFeature(QContactManager::Anonymous, QContactType::TypeGroup));
    QVERIFY(!m_engine->hasFeature(QContactManager::ChangeLogs, QContactType::TypeContact));
}

void TestSymbianEngine::addGroup()
{
    QContactManager::Error err = QContactManager::NoError;
    QContactId empty;

    // Add valid group contact
    QContact g;
    g.setType(QContactType::TypeGroup);

    QVERIFY(m_engine->saveContact(&g, &err));
    QVERIFY(err == QContactManager::NoError);
    QVERIFY(g.id() != empty);
    QVERIFY(g.localId() != 0);
    QString uri = QString(QLatin1String(CNT_SYMBIAN_MANAGER_NAME));
    QVERIFY(g.id().managerUri().contains(uri, Qt::CaseInsensitive));

    // Add invalid group contact
    QContact g1;
    g1.setType("Jargon");

    QVERIFY(!m_engine->saveContact(&g1, &err));
    QVERIFY(err == QContactManager::InvalidDetailError);
    QVERIFY(g1.id() == empty);
    QVERIFY(g1.localId() == 0);
}

void TestSymbianEngine::retrieveGroup()
{
    QContactManager::Error err;
    QList<QContactSortOrder> s;

    // retrieve group contacts
    QContactDetailFilter filter;
    filter.setDetailDefinitionName(QContactType::DefinitionName, QContactType::FieldType);
    filter.setValue(QString(QLatin1String(QContactType::TypeGroup)));
    
    QList<QContactLocalId> grp_ids = m_engine->contactIds(filter, s, &err);
    QVERIFY(err == QContactManager::NoError);
    QVERIFY(err == QContactManager::NoError);

    QContact g;
    g.setType(QContactType::TypeGroup);
    QVERIFY(m_engine->saveContact(&g, &err));
    QVERIFY(err == QContactManager::NoError);

    QList<QContactLocalId> grp_ids1 = m_engine->contactIds(filter, s, &err);
    QVERIFY(err == QContactManager::NoError);
    QVERIFY(grp_ids.count() + 1 == grp_ids1.count());

}

void TestSymbianEngine::singleRelationship()
{
    // More relationships logic is tested in relationship unit tests.
    // We do simple tests here
    QContactManager::Error error;

    QContact a;
    QVERIFY(m_engine->saveContact(&a, &error));
    QContact b;
    QVERIFY(m_engine->saveContact(&b, &error));

    QContactRelationship rel;
    rel.setFirst(a.id());
    rel.setSecond(b.id());
    rel.setRelationshipType(QContactRelationship::HasSpouse);

    // Add relationship
    m_engine->saveRelationship(&rel, &error);
    bool isValid(false);
    if (error == QContactManager::NoError ||
        error == QContactManager::NotSupportedError)
        isValid = true;

    QVERIFY(isValid);

    // Remove relationship
    m_engine->removeRelationship(rel, &error);
    if (error == QContactManager::NoError ||
        error == QContactManager::NotSupportedError)
        isValid = true;
    else
        isValid = false;
    QVERIFY(isValid);

    // TODO: replacement?
    /*QStringList supportedRelationships =
        m_engine->supportedRelationshipTypes(QContactType::TypeGroup);
    QVERIFY(supportedRelationships.contains(QContactRelationship::HasMember));
    */
}

void TestSymbianEngine::batchRelationships()
{
    // More relationships logic is tested somewhere else.
    // We do simple tests here
    QContactManager::Error error;

    QContact a;
    QVERIFY(m_engine->saveContact(&a, &error));
    QContact b;
    QVERIFY(m_engine->saveContact(&b, &error));

    QContactRelationship rel;
    rel.setFirst(a.id());
    rel.setSecond(b.id());
    rel.setRelationshipType(QContactRelationship::HasSpouse);

    QList<QContactRelationship> list;
    list.append(rel);
    bool isValid(false);

    // Add relationships
    QMap<int, QContactManager::Error> errorMap;
    QVERIFY(!m_engine->saveRelationships(&list, &errorMap, &error));
    foreach(QContactManager::Error err, errorMap) {
        if (err == QContactManager::NoError ||
                err == QContactManager::NotSupportedError)
            isValid = true;
        else
            isValid = false;
        QVERIFY(isValid);
    }

    // fetch relationships
    QContactRelationship::Role role;
    role = QContactRelationship::First;
    list.clear();
    list = m_engine->relationships(QContactRelationship::HasSpouse, a.id(), role, &error);
    QVERIFY(&list != NULL);
    if (error == QContactManager::NoError ||
        error == QContactManager::NotSupportedError)
        isValid = true;
    else
        isValid = false;
    QVERIFY(isValid);

    // Remove relationships
    QVERIFY(m_engine->removeRelationships(list, &errorMap, &error));
    foreach(QContactManager::Error err, errorMap) {
        if (err == QContactManager::NoError ||
                err == QContactManager::NotSupportedError)
            isValid = true;
        else
            isValid = false;
        QVERIFY(isValid);
    }
}

void TestSymbianEngine::dataTypeSupport()
{
    QList<QVariant::Type> support = m_engine->supportedDataTypes();
    QVERIFY(support.contains(QVariant::String));
}

void TestSymbianEngine::synthesizeDisplaylable()
{
    QContactManager::Error err = QContactManager::NoError;

    QContact empty;
    QString label = m_engine->synthesizedDisplayLabel(empty, &err);
    QVERIFY(err == QContactManager::NoError);
    QVERIFY(label.isEmpty());

    QContact first;
    QContactName fn;
    fn.setFirstName("Alice");
    first.saveDetail(&fn);
    label = m_engine->synthesizedDisplayLabel(first, &err);
    QVERIFY(err == QContactManager::NoError);
    QVERIFY(label == QString("Alice"));

    QContact last;
    QContactName ln;
    ln.setLastName("Jones");
    last.saveDetail(&ln);
    label = m_engine->synthesizedDisplayLabel(last, &err);
    QVERIFY(err == QContactManager::NoError);
    QVERIFY(label == QString("Jones"));

    QContact orgContact;
    QContactOrganization org;
    org.setName("Nokia");
    org.setDepartment(QStringList("Services"));
    org.setTitle("Assistant Manager");
    org.setLocation("Nokia Cyber Park");
    orgContact.saveDetail(&org);
    label = m_engine->synthesizedDisplayLabel(orgContact, &err);
    QVERIFY(err == QContactManager::NoError);
    QVERIFY(label == QString("Nokia"));

    QContact jargon;
    jargon.setType("jargon");
    label = m_engine->synthesizedDisplayLabel(jargon, &err);
    QVERIFY(err == QContactManager::InvalidContactTypeError);
    QVERIFY(label.isEmpty());

    QContact group;
    group.setType(QContactType::TypeGroup);
    QContactName gn;
    gn.setCustomLabel("grouplable");
    group.saveDetail(&gn);
    label = m_engine->synthesizedDisplayLabel(group, &err);
    QVERIFY(err == QContactManager::NoError);
    QVERIFY(label == QString("grouplable"));
}

void TestSymbianEngine::definitionDetails()
{
    QMap<QString, QContactDetailDefinition> defs;
    QContactManager::Error err;

    // Wrong contact type
    defs = m_engine->detailDefinitions("aerafa", &err);
    QVERIFY(err = QContactManager::InvalidContactTypeError);
    QVERIFY(defs.count() == 0);

    // Valid defs
    defs = m_engine->detailDefinitions(QContactType::TypeContact, &err);
    QVERIFY(err == QContactManager::NoError);
    defs = m_engine->detailDefinitions(QContactType::TypeGroup, &err);
    QVERIFY(err == QContactManager::NoError);
}

void TestSymbianEngine::asyncRequests()
{
    //create a contact
    QContactManager::Error err;
    QContact dummy;
    dummy.setType(QContactType::TypeContact);
    QContactName name;
    name.setFirstName("dummy");
    dummy.saveDetail(&name);
    QVERIFY(m_engine->saveContact(&dummy, &err));
    QVERIFY(err == QContactManager::NoError);
    
    //create a group with members 
    QContact groupContact;
    groupContact.setType(QContactType::TypeGroup);
    m_engine->saveContact(&groupContact, &err);
    QVERIFY(err == QContactManager::NoError);
    QContactRelationship relationship;
    relationship.setRelationshipType(QContactRelationship::HasMember);
    relationship.setFirst(groupContact.id());
    relationship.setSecond(dummy.id());
    bool returnValue(false);
    returnValue = m_engine->saveRelationship(&relationship, &err);
    QVERIFY(returnValue == true);
    QVERIFY(err == QContactManager::NoError);
    
    //fetch request
    QContactFetchRequest fetch;
    QVERIFY(m_engine->startRequest(&fetch));
    QTest::qWait(1000); //1sec
    QVERIFY(fetch.error() == QContactManager::NoError);
    QVERIFY(fetch.state() == QContactFetchRequest::FinishedState);
    QVERIFY(fetch.contacts().count() > 0);
    
    //fetch ids request
    QContactLocalIdFetchRequest fetchIds;
    QVERIFY(m_engine->startRequest(&fetchIds));
    QTest::qWait(1000); //1sec
    QVERIFY(fetchIds.error() == QContactManager::NoError);
    QVERIFY(fetchIds.state() == QContactFetchRequest::FinishedState);
    QVERIFY(fetchIds.ids().count() > 0);
    
    //save request
    QContactSaveRequest saveReq;
    QContact c;
    c.setType(QContactType::TypeContact);
    QList<QContact> contactList;
    contactList += c;
    saveReq.setContacts(contactList);
    QVERIFY(m_engine->startRequest(&saveReq));
    QTest::qWait(1000); //1sec
    QVERIFY(saveReq.error() == QContactManager::NoError);
    QVERIFY(saveReq.state() == QContactFetchRequest::FinishedState);
    QVERIFY(saveReq.contacts().count() > 0);
    
    //remove request
    QContactRemoveRequest removeReq;
    QList<QContactLocalId> idList;
    idList += saveReq.contacts().at(0).localId();
    removeReq.setContactIds(idList);
    QVERIFY(m_engine->startRequest(&removeReq));
    QTest::qWait(1000); //1sec
    int err_temp = removeReq.error();
    QVERIFY(removeReq.error() == QContactManager::NoError);
    QVERIFY(removeReq.state() == QContactFetchRequest::FinishedState);
    
    //detail definition request
    QContactDetailDefinitionFetchRequest detDefReq;
    detDefReq.setContactType(QContactType::TypeContact);
    QStringList defList;
    defList += QContactName::DefinitionName;
    detDefReq.setDefinitionNames(defList);
    QVERIFY(m_engine->startRequest(&detDefReq));
    QTest::qWait(1000); //1sec
    QVERIFY(detDefReq.error() == QContactManager::NoError);
    QVERIFY(detDefReq.state() == QContactFetchRequest::FinishedState);
    
    //relationship fetch request
    QContactRelationshipFetchRequest relFetchReq;
    relFetchReq.setFirst(groupContact.id());
    relFetchReq.setSecond(dummy.id());
    relFetchReq.setRelationshipType(QContactRelationship::HasMember);
    QVERIFY(m_engine->startRequest(&relFetchReq));
    QTest::qWait(1000); //1sec
    QVERIFY(relFetchReq.error() == QContactManager::NoError);
    QVERIFY(relFetchReq.state() == QContactFetchRequest::FinishedState);
    QVERIFY(relFetchReq.relationships().count() > 0);
    
    //relationship remove request
    QContactRelationshipRemoveRequest relRemoveReq;
    QList<QContactRelationship> relList;
    relList += relationship;
    relRemoveReq.setRelationships(relList);
    QVERIFY(m_engine->startRequest(&relRemoveReq));
    QTest::qWait(1000); //1sec
    QVERIFY(relRemoveReq.error() == QContactManager::NoError);
    QVERIFY(relRemoveReq.state() == QContactFetchRequest::FinishedState);

    //relationship save request
    QContactRelationshipSaveRequest relSaveReq;
    relSaveReq.setRelationships(relList);
    QVERIFY(m_engine->startRequest(&relSaveReq));
    QTest::qWait(1000); //1sec
    QVERIFY(relSaveReq.error() == QContactManager::NoError);
    QVERIFY(relSaveReq.state() == QContactFetchRequest::FinishedState);
    
    //cancel request
    QVERIFY(m_engine->startRequest(&relRemoveReq));
    m_engine->cancelRequest(&relRemoveReq);
    QTest::qWait(1000); //1sec
    QVERIFY(relRemoveReq.error() == QContactManager::NoError);
    QVERIFY(relRemoveReq.state() == QContactFetchRequest::CanceledState);
    
    //wait for a request finish
    QVERIFY(!m_engine->waitForRequestFinished(&relSaveReq, 1000));
    QVERIFY(m_engine->startRequest(&relRemoveReq));
    QVERIFY(m_engine->waitForRequestFinished(&relRemoveReq, 1000));
    QTest::qWait(1000); //1sec
    QVERIFY(relRemoveReq.error() == QContactManager::NoError);
    QVERIFY(relRemoveReq.state() == QContactFetchRequest::FinishedState);
    
    //destroy request
    QVERIFY(m_engine->startRequest(&relRemoveReq));
    m_engine->requestDestroyed(&relRemoveReq);
    QVERIFY(!m_engine->m_asynchronousOperations.contains(&relRemoveReq));
}
