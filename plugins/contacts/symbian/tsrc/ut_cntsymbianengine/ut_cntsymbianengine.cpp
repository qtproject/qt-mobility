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

#include "ut_cntsymbianengine.h"
#include "cntsymbianengine.h"
#include "qcontactrelationship.h"
#include "qcontactrelationshipfilter.h"
#include <qcontactmanager.h>

#include <QtTest/QtTest>

void TestSymbianEngine::initTestCase()
{
    QContactManager::Error error;
    QMap<QString, QString> emptyParameters;

    m_engine = new CntSymbianEngine(emptyParameters, error);
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

void TestSymbianEngine::clean()
{
    removeAllContacts();
}

void TestSymbianEngine::removeAllContacts()
{
    if(m_engine) {
        // Empty cnt database
        QContactManager::Error err;
        QList<QContactSortOrder> sortOrders;
        QList<QContactLocalId> cnts_ids = m_engine->contacts(sortOrders, err);
        QVERIFY(err == QContactManager::NoError);

        for(int i=0; i<cnts_ids.count(); i++) {
            QVERIFY(m_engine->removeContact(cnts_ids[i], err));
        }
    }
}

void TestSymbianEngine::saveContact()
{
    QContactManager::Error err;
    QList<QContactSortOrder> sortOrders;
    QContactId empty;

    int init_count = m_engine->contacts(sortOrders, err).count();
    QVERIFY(err == QContactManager::NoError);

    // Save a "NULL" contact
    QVERIFY(!m_engine->saveContact(NULL, err));
    QVERIFY(err == QContactManager::BadArgumentError);
    int current_count = m_engine->contacts(sortOrders, err).count();
    QVERIFY(err == QContactManager::NoError);
    QVERIFY(init_count == current_count);

    QContact alice;
    alice.setType("Jargon");

    // Save a "non contact(Jargon) type" contact
    QVERIFY(!m_engine->saveContact(&alice, err));
    QVERIFY(err == QContactManager::InvalidDetailError);
    QVERIFY(alice.id() == empty);
    QVERIFY(alice.localId() == 0);
    current_count = m_engine->contacts(sortOrders, err).count();
    QVERIFY(err == QContactManager::NoError);
    QVERIFY(init_count == current_count);

    // Save a valid contact
    alice.setType(QContactType::TypeContact);
    QVERIFY(m_engine->saveContact(&alice, err));
    QVERIFY(err == QContactManager::NoError);
    QVERIFY(alice.id() != empty);
    QVERIFY(alice.localId() != 0);
    QString uri = QString(QLatin1String(CNT_SYMBIAN_MANAGER_NAME));
    QVERIFY(alice.id().managerUri().contains(uri, Qt::CaseInsensitive));
    current_count = m_engine->contacts(sortOrders, err).count();
    QVERIFY(err == QContactManager::NoError);
    QVERIFY(init_count + 1 == current_count);
}

void TestSymbianEngine::saveContacts()
{
    QContactManager::Error err;
    QList<QContactSortOrder> sortOrders;
    QList<QContact> contacts;
    QContactId empty;
    int count = 5;

    int init_count = m_engine->contacts(sortOrders, err).count();
    QVERIFY(err == QContactManager::NoError);

    // NULL
    QList<QContactManager::Error> errors = m_engine->saveContacts(NULL, err);
    QVERIFY(errors.count() == 0);
    QVERIFY(err == QContactManager::BadArgumentError);
    int current_count = m_engine->contacts(sortOrders, err).count();
    QVERIFY(err == QContactManager::NoError);
    QVERIFY(init_count == current_count);

    // Save a "non contact(Jargon) type" contacts
    for(int i=0; i<count; i++) {
        QContact alice;
        alice.setType("Jargon");
        contacts.append(alice);
    }
    errors.clear();
    errors = m_engine->saveContacts(&contacts, err);
    QVERIFY(err == QContactManager::InvalidDetailError);
    foreach(QContactManager::Error err, errors) {
        QVERIFY(err == QContactManager::InvalidDetailError);
    }
    foreach(QContact c, contacts) {
        QVERIFY(c.id() == empty);
        QVERIFY(c.localId() == 0);
    }
    current_count = m_engine->contacts(sortOrders, err).count();
    QVERIFY(err == QContactManager::NoError);
    QVERIFY(init_count == current_count);
    contacts.clear();
    errors.clear();

    // Save valid contacts
    for(int i=0; i<count; i++) {
        QContact alice;
        alice.setType(QContactType::TypeContact);
        contacts.append(alice);
    }
    errors = m_engine->saveContacts(&contacts, err);
    QVERIFY(err == QContactManager::NoError);
    foreach(QContactManager::Error err, errors) {
        QVERIFY(err == QContactManager::NoError);
    }
    QString uri = QString(QLatin1String(CNT_SYMBIAN_MANAGER_NAME));
    foreach(QContact c, contacts) {
        QVERIFY(c.id() != empty);
        QVERIFY(c.localId() != 0);
        QVERIFY(c.id().managerUri().contains(uri, Qt::CaseInsensitive));
    }
    current_count = m_engine->contacts(sortOrders, err).count();
    QVERIFY(err == QContactManager::NoError);
    QVERIFY(init_count + count == current_count);
    errors.clear();
    contacts.clear();

    // Save with one invalid contact in list
    init_count = m_engine->contacts(sortOrders, err).count();
    for(int i=0; i<count; i++) {
        QContact alice;
        alice.setType(QContactType::TypeContact);
        contacts.append(alice);
    }
    QContact invalid;
    invalid.setType("Jasdfasd");
    contacts.insert(3, invalid);

    errors = m_engine->saveContacts(&contacts, err);
    QVERIFY(err == QContactManager::InvalidDetailError);
    for(int i=0; i<errors.count(); i++) {
        if (i == 3)
            QVERIFY(errors[i] == QContactManager::InvalidDetailError);
        else
            QVERIFY(errors[i] == QContactManager::NoError);
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
    current_count = m_engine->contacts(sortOrders, err).count();
    QVERIFY(err == QContactManager::NoError);
    QVERIFY(init_count + count == current_count);
    errors.clear();
    contacts.clear();
}

void TestSymbianEngine::contact()
{
    QContactManager::Error err;

    QContact alice;
    alice.setType(QContactType::TypeContact);
    QVERIFY(m_engine->saveContact(&alice, err));
    QVERIFY(err == QContactManager::NoError);

    // Retrieve "non contact"
    QContact c = m_engine->contact(0, err);
    QVERIFY(&c != NULL);
    QVERIFY(c.localId() == 0);
    QVERIFY(err == QContactManager::DoesNotExistError);

    // Retrieve valid existing contact
    QContactLocalId aid = alice.localId();
    c = m_engine->contact(aid, err);
    QVERIFY(&c != NULL);
    QVERIFY(c.localId() == aid);
    QVERIFY(err == QContactManager::NoError);
}

void TestSymbianEngine::updateContact()
{
    QContactManager::Error err;
    QContact c;

    QVERIFY(m_engine->saveContact(&c, err));
    QVERIFY(err == QContactManager::NoError);

    int details_before = c.details().count();

    QContactName aliceName;
    aliceName.setFirst("Alice");
    c.saveDetail(&aliceName);

    QContactPhoneNumber number;
    number.setContexts("Home");
    number.setSubTypes("Mobile");
    number.setNumber("12345678");
    c.saveDetail(&number);

    // update the contact
    QContactLocalId id = c.localId();
    QVERIFY(m_engine->saveContact(&c, err));
    QVERIFY(err == QContactManager::NoError);

    // Verify that contact has been updated
    QContact d = m_engine->contact(id, err);
    QVERIFY(err == QContactManager::NoError);
    QVERIFY(d.localId() == id);
    QVERIFY(d.details().count() > details_before);
    QString str = d.detail(QContactPhoneNumber::DefinitionName).definitionName();
    QVERIFY(str == QContactPhoneNumber::DefinitionName);
}

void TestSymbianEngine::removeContact()
{
    QContactManager::Error err;

    QContact c;
    c.setType(QContactType::TypeContact);
    QVERIFY(m_engine->saveContact(&c, err));
    QVERIFY(err == QContactManager::NoError);

    // Remove existing contact
    QContactLocalId id = c.localId();
    QVERIFY(m_engine->removeContact(id, err));
    QVERIFY(err == QContactManager::NoError);

    // Verify that contact has been removed
    QContact f = m_engine->contact(id, err);
    QVERIFY(f.localId() == 0);
    QVERIFY(err == QContactManager::DoesNotExistError);

    // Remove non existent contact
    QVERIFY(!m_engine->removeContact(0, err));
    QVERIFY(err == QContactManager::DoesNotExistError);
}

void TestSymbianEngine::removeContacts()
{
    QContactManager::Error err;
    QList<QContactLocalId> contacts;
    int count = 5;

    // Remove non existent contacts
    QList<QContactManager::Error> errors = m_engine->removeContacts(&contacts, err);
    QVERIFY(err == QContactManager::NoError);
    QVERIFY(errors.count() == 0);

    // NULL argument
    errors.clear();
    errors = m_engine->removeContacts(NULL, err);
    QVERIFY(errors.count() == 0);
    QVERIFY(err == QContactManager::BadArgumentError);

    // Remove existing contacts
    for(int i=0; i<count; i++) {
        QContact c;
        c.setType(QContactType::TypeContact);
        QVERIFY(m_engine->saveContact(&c, err));
        QVERIFY(err == QContactManager::NoError);
        contacts.append(c.localId());
    }
    errors.clear();
    errors = m_engine->removeContacts(&contacts, err);
    QVERIFY(err == QContactManager::NoError);
    foreach(QContactManager::Error e, errors) {
        QVERIFY(e == QContactManager::NoError);
    }

    // Verify that contacts have been removed
    for(int i=0; i<contacts.count(); i++) {
        QContact f = m_engine->contact(contacts[i], err);
        QVERIFY(f.localId() == 0);
        QVERIFY(err == QContactManager::DoesNotExistError);
    }

    // Remove a list with one non existent contact
    contacts.clear();
    for(int i=0; i<count; i++) {
        QContact c;
        c.setType(QContactType::TypeContact);
        QVERIFY(m_engine->saveContact(&c, err));
        QVERIFY(err == QContactManager::NoError);
        contacts.append(c.localId());
    }
    contacts.insert(3, 0);

    errors.clear();
    errors = m_engine->removeContacts(&contacts, err);
    QVERIFY(err == QContactManager::DoesNotExistError);
    for(int i=0; i<errors.count(); i++) {
        if (i == 3)
            QVERIFY(errors[i] == QContactManager::DoesNotExistError);
        else
            QVERIFY(errors[i] == QContactManager::NoError);
    }

    for(int i=0; i<contacts.count(); i++) {
        QContact f = m_engine->contact(contacts[i], err);
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
    ownName.setFirst("Own");
    own.saveDetail(&ownName);
    QVERIFY(m_engine->saveContact(&own, err));
    QVERIFY(err == QContactManager::NoError);

    // Set a non existent contact as own card and verify
    // ensure this contact does not exist in dbase
    QContactLocalId id(12);
    m_engine->removeContact(id, err);   // Dont test err. May or may not be in dbase
    QVERIFY(!m_engine->setSelfContactId(id, err)); // does not exist
    QVERIFY(err == QContactManager::DoesNotExistError);

    // Test a "0" contact id
    QVERIFY(!m_engine->setSelfContactId(0, err)); // Bad argument
    QVERIFY(err == QContactManager::BadArgumentError);

    // Set an existent contact as own card
    QVERIFY(m_engine->setSelfContactId(own.localId(), err));
    QVERIFY(err == QContactManager::NoError);
}

void TestSymbianEngine::retrieveOwnCard()
{
    QContactManager::Error err;

    // Create a new contact own card
    QContact own;
    QContactName ownName;
    ownName.setFirst("Own");
    own.saveDetail(&ownName);
    QVERIFY(m_engine->saveContact(&own, err));
    QVERIFY(err == QContactManager::NoError);
    QVERIFY(m_engine->setSelfContactId(own.localId(), err));
    QVERIFY(err == QContactManager::NoError);

    // Fetch existing self contact
    QContactLocalId own_id = m_engine->selfContactId(err);
    QVERIFY(err == QContactManager::NoError);
    QVERIFY(own_id == own.localId());

    // Remove self contact and verify
    QVERIFY(m_engine->removeContact(own.localId(), err));
    QContactLocalId idr = m_engine->selfContactId(err);
    QVERIFY(err == QContactManager::DoesNotExistError);
    QVERIFY(idr == 0);
}

void TestSymbianEngine::simpleFilterTest()
{
    QContactManager::Error err = QContactManager::NoError;
    QContactFilter filter;
    QList<QContactSortOrder> sortOrders;

    // Filters logic is tested somewhere else.
    // Test that a valid list object is returned
    // We do not care if its empty or not
    QList<QContactLocalId> ids = m_engine->contacts(filter, sortOrders, err);
    QVERIFY(&ids != NULL);

    /*// We test that valid values are returned
    CntAbstractContactFilter::FilterSupport support = m_engine->filterSupported(filter);
    bool isValid(false);
    if (support == CntAbstractContactFilter::NotSupported ||
        support == CntAbstractContactFilter::Supported ||
        support == CntAbstractContactFilter::SupportedPreFilterOnly) {
        isValid = true;
    }
    QVERIFY(isValid);
    */

}

void TestSymbianEngine::featureSupport()
{
    //hasFeature(QContactManager::ManagerFeature feature, const QString& contactType)
    QContactManager::ManagerFeature f;

    // NULL parameters
    QVERIFY(!m_engine->hasFeature(f, ""));

    // empty feature param
    QVERIFY(!m_engine->hasFeature(f, QContactType::TypeContact));
    QVERIFY(!m_engine->hasFeature(f, QContactType::TypeGroup));

    // Non existent feature
    f = static_cast<QContactManager::ManagerFeature>(198);
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
    QVERIFY(m_engine->hasFeature(QContactManager::Groups, QContactType::TypeContact));
    QVERIFY(!m_engine->hasFeature(QContactManager::ActionPreferences, QContactType::TypeContact));
    QVERIFY(!m_engine->hasFeature(QContactManager::MutableDefinitions, QContactType::TypeContact));
    QVERIFY(m_engine->hasFeature(QContactManager::Relationships, QContactType::TypeContact));
    QVERIFY(!m_engine->hasFeature(QContactManager::ArbitraryRelationshipTypes, QContactType::TypeContact));
    QVERIFY(m_engine->hasFeature(QContactManager::SelfContact, QContactType::TypeContact));
    QVERIFY(!m_engine->hasFeature(QContactManager::Anonymous, QContactType::TypeContact));
    QVERIFY(!m_engine->hasFeature(QContactManager::ChangeLogs, QContactType::TypeContact));
}

/*
void TestSymbianEngine::addGroup()
{
    QContactManager::Error err = QContactManager::NoError;
    QContactChangeSet cs;
    QContactId empty;

    // Add valid group contact
    QContact g;
    g.setType(QContactType::TypeGroup);

    QVERIFY(m_engine->addContact(g, cs, err));
    QVERIFY(err == QContactManager::NoError);
    QVERIFY(g.id() != empty);
    QVERIFY(g.localId() != 0);
    QString uri = QString(QLatin1String(CNT_SYMBIAN_MANAGER_NAME));
    QVERIFY(g.id().managerUri().contains(uri, Qt::CaseInsensitive));

    // Add invalid group contact
    QContact g1;
    g1.setType("Jargon");

    QVERIFY(!m_engine->addContact(g1, cs, err));
    QVERIFY(err == QContactManager::InvalidDetailError);
    QVERIFY(g1.id() == empty);
    QVERIFY(g1.localId() == 0);
}

void TestSymbianEngine::retrieveGroup()
{
    QContactManager::Error err = QContactManager::NoError;
    QContactChangeSet cs;
    QContactId empty;

    // retrieve group contacts
    QContact g;
    g.setType(QContactType::TypeGroup);

    QList<QContactLocalId> grp_ids = m_engine->groups(err);
    QVERIFY(err == QContactManager::NoError);

    QVERIFY(m_engine->addContact(g, cs, err));
    QVERIFY(err == QContactManager::NoError);

    QVERIFY(grp_ids.count() + 1 == m_engine->groups(err).count());
    QVERIFY(err == QContactManager::NoError);
}
*/

void TestSymbianEngine::singleRelationship()
{
    // More relationships logic is tested in relationship unit tests.
    // We do simple tests here
    QContactManager::Error error;

    QContact a;
    QVERIFY(m_engine->saveContact(&a, error));
    QContact b;
    QVERIFY(m_engine->saveContact(&b, error));

    QContactRelationship rel;
    rel.setFirst(a.id());
    rel.setSecond(b.id());
    rel.setRelationshipType(QContactRelationship::IsSpouseOf);

    // Add relationship
    m_engine->saveRelationship(&rel, error);
    bool isValid(false);
    if (error == QContactManager::NoError ||
        error == QContactManager::NotSupportedError)
        isValid = true;

    QVERIFY(isValid);

    // Remove relationship
    m_engine->removeRelationship(rel, error);
    if (error == QContactManager::NoError ||
        error == QContactManager::NotSupportedError)
        isValid = true;
    else
        isValid = false;
    QVERIFY(isValid);
}

void TestSymbianEngine::batchRelationships()
{
    // More relationships logic is tested somewhere else.
    // We do simple tests here
    QContactManager::Error error;

    QContact a;
    QVERIFY(m_engine->saveContact(&a, error));
    QContact b;
    QVERIFY(m_engine->saveContact(&b, error));

    QContactRelationship rel;
    rel.setFirst(a.id());
    rel.setSecond(b.id());
    rel.setRelationshipType(QContactRelationship::IsSpouseOf);

    QList<QContactRelationship> list;
    list.append(rel);
    bool isValid(false);

    // Add relationships
    QList<QContactManager::Error> errors = m_engine->saveRelationships(&list, error);
    QVERIFY(&errors != NULL);
    foreach(QContactManager::Error err, errors) {
        if (err == QContactManager::NoError ||
                err == QContactManager::NotSupportedError)
            isValid = true;
        else
            isValid = false;
        QVERIFY(isValid);
    }

    // fetch relationships
    QContactRelationshipFilter::Role role;
    role = QContactRelationshipFilter::First;
    list.clear();
    list = m_engine->relationships(QContactRelationship::IsSpouseOf, a.id(), role, error);
    QVERIFY(&list != NULL);
    if (error == QContactManager::NoError ||
        error == QContactManager::NotSupportedError)
        isValid = true;
    else
        isValid = false;
    QVERIFY(isValid);

    // Remove relationships
    errors = m_engine->removeRelationships(list, error);
    QVERIFY(&errors != NULL);
    foreach(QContactManager::Error err, errors) {
        if (err == QContactManager::NoError ||
                err == QContactManager::NotSupportedError)
            isValid = true;
        else
            isValid = false;
        QVERIFY(isValid);
    }
}

QTEST_MAIN(TestSymbianEngine);
