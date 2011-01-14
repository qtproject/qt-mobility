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

#include "ut_cntrelationship.h"
#include "cntrelationship.h"



//Symbian database
#include <cntdb.h>
#include <cntitem.h>
#include <cntdb.h>
#include <cntdef.h>

#include <QtTest/QtTest>

#define QTRY_COMPARE(__expr, __expected) \
    do { \
        const int __step = 50; \
        const int __timeout = 1000; \
        QTest::qWait(10); \
        for (int __i = 0; __i < __timeout && ((__expr) != (__expected)); __i+=__step) { \
            QTest::qWait(__step); \
        } \
        QCOMPARE(__expr, __expected); \
    } while(0)


/*
 * Done once before all tests
 */
void TestCntRelationship::initTestCase()
{
    qRegisterMetaType<QContactLocalId>("QContactLocalId");
    qRegisterMetaType<QList<QContactLocalId> >("QList<QContactLocalId>");

    //create manager
    QContactManager::Error err;
    m_manager = new CntSymbianEngine(QMap<QString, QString>(), &err);

    //open symbian database
    TRAPD(error, m_database = CContactDatabase::OpenL());
    QVERIFY(error == KErrNone);

    // create relationship
    m_relationship = new CntRelationship(m_database, m_manager->m_managerUri);
}


/*
 * Cleanup after all test
 */
void TestCntRelationship::cleanupTestCase()
{
    delete m_manager;
    delete m_database;
    delete m_relationship;
}

/*
 * Initialization done before each test
 */
void TestCntRelationship::init()
{
    //delete all contacts from the database
    QContactManager::Error err;
    QList<QContactLocalId> contacts = m_manager->contactIds(QContactFilter(), QList<QContactSortOrder>(), &err);
    
    QMap<int, QContactManager::Error> errorMap;
    m_manager->removeContacts(contacts, &errorMap, &err);
}

void TestCntRelationship::cleanup()
{}

/*
 * Test relatioship that doesn't exist
 */
void TestCntRelationship::invalidRelationship()
{
    QContactRelationship relationship;
    relationship.setRelationshipType(QLatin1String("invalid relationship") );

    //save & remove relationship
    QContactManager::Error error;
    QSet<QContactLocalId> affectedContactIds;
    bool returnValue(false);

    //save & remove relationships
    QList<QContactRelationship> relationships;
    relationships.append(relationship);

    //save relationship
    returnValue = m_relationship->saveRelationship(&affectedContactIds, &relationship, &error);
    QVERIFY(returnValue == false);
    QVERIFY(affectedContactIds.count() == 0);
    QVERIFY(error == QContactManager::NotSupportedError);

    //remove relationship
    returnValue = m_relationship->removeRelationship(&affectedContactIds, relationship, &error);
    QVERIFY(returnValue == false);
    QVERIFY(affectedContactIds.count() == 0);
    QVERIFY(error == QContactManager::NotSupportedError);

    //save relationships
    QMap<int, QContactManager::Error> errorMap;
    QVERIFY(!m_relationship->saveRelationships(&affectedContactIds, &relationships, &errorMap, &error));
    QVERIFY(affectedContactIds.count() == 0);
    foreach (QContactManager::Error err, errorMap) {
        QVERIFY(err == QContactManager::NotSupportedError);
    }

    //remove relationships
    QVERIFY(!m_relationship->removeRelationships(&affectedContactIds, relationships, &errorMap, &error));
    QVERIFY(affectedContactIds.count() == 0);
    foreach (QContactManager::Error err, errorMap) {
        QVERIFY(err == QContactManager::NotSupportedError);
    }

    //relationships
    QList<QContactRelationship> relationshipList;
    QContactId id;

    relationshipList = m_relationship->relationships(QLatin1String("invalid relationship"), id, QContactRelationship::Either, &error);
    QVERIFY(relationshipList.count() == 0);
    QVERIFY(error == QContactManager::NotSupportedError);
}

/*
 * Test valid group relationship functionality
 */
void TestCntRelationship::validGroupRelationship()
{
    //create a group
    QContact groupContact;
    groupContact.setType(QContactType::TypeGroup);
    QContactManager::Error err;
    m_manager->saveContact(&groupContact, &err);

    //create a contact
    QContact contact;
    contact.setType(QContactType::TypeContact);
    m_manager->saveContact(&contact, &err);

    //create relationship
    QContactRelationship relationship;
    relationship.setRelationshipType(QContactRelationship::HasMember);
    relationship.setFirst(groupContact.id());
    relationship.setSecond(contact.id());

    //save relationship
    bool returnValue(false);
    QContactManager::Error error;
    QSet<QContactLocalId> affectedContactIds;
    returnValue = m_relationship->saveRelationship(&affectedContactIds, &relationship, &error);
    QVERIFY2(returnValue == true, "save");
    QVERIFY2(affectedContactIds.count() == 2, "save");
    QVERIFY2(affectedContactIds.toList().contains(groupContact.localId()), "save");
    QVERIFY2(affectedContactIds.toList().contains(contact.localId()), "save");
    QVERIFY2(error == QContactManager::NoError, "save");

    //retrieve the relationships
    QList<QContactRelationship> relationshipList;

    //Retrive group
    relationshipList = m_relationship->relationships(QLatin1String(QContactRelationship::HasMember), groupContact.id(), QContactRelationship::First, &error);

    QVERIFY2(relationshipList.count() == 1, "group - First");
    QVERIFY2(error == QContactManager::NoError, "group - First");

    relationshipList = m_relationship->relationships(QLatin1String(QContactRelationship::HasMember), groupContact.id(), QContactRelationship::Second, &error);
    QVERIFY2(relationshipList.count() == 0, "group - Second");
    QVERIFY2(error == QContactManager::DoesNotExistError, "group - Second");

    relationshipList = m_relationship->relationships(QLatin1String(QContactRelationship::HasMember), groupContact.id(), QContactRelationship::Either, &error);
    QVERIFY2(relationshipList.count() == 1, "group - Either");
    QVERIFY2(error == QContactManager::NoError, "group - Either");

    //Retrive contact
    relationshipList = m_relationship->relationships(QLatin1String(QContactRelationship::HasMember), contact.id(), QContactRelationship::First, &error);
    QVERIFY2(relationshipList.count() == 0, "contact - First");
    QVERIFY2(error == QContactManager::DoesNotExistError, "contact - First");

    relationshipList = m_relationship->relationships(QLatin1String(QContactRelationship::HasMember), contact.id(), QContactRelationship::Second, &error);
    QVERIFY2(relationshipList.count() == 1, "contact - Second");
    QVERIFY2(error == QContactManager::NoError, "contact - Second");

    relationshipList = m_relationship->relationships(QLatin1String(QContactRelationship::HasMember), contact.id(), QContactRelationship::Either, &error);
    QVERIFY2(relationshipList.count() == 1, "contact - Either");
    QVERIFY2(error == QContactManager::NoError, "contact - Either");   
    
    //Relationship for empty contact -> all relationships are returned 
    relationshipList = m_relationship->relationships(QLatin1String(QContactRelationship::HasMember), QContactId(),
                QContactRelationship::First, &error);
    QVERIFY2(relationshipList.count() == 1, "All relationships");
    QVERIFY2(error == QContactManager::NoError, "All relatiosnships");
    
    //Validate Filter
    QList<QContactLocalId> expectedContacts;
    expectedContacts += contact.localId();
    QVERIFY(true == validateRelationshipFilter(QContactRelationship::First, groupContact.id(), expectedContacts));
    
    expectedContacts.clear();
    expectedContacts += contact.localId();
    QVERIFY(true == validateRelationshipFilter(QContactRelationship::Either, groupContact.id(), expectedContacts));

    expectedContacts.clear();
    expectedContacts += groupContact.localId();
    QVERIFY(true == validateRelationshipFilter(QContactRelationship::Second, contact.id(), expectedContacts));
    
    //remove relationship
    returnValue = m_relationship->removeRelationship(&affectedContactIds, relationship, &error);
    QVERIFY2(returnValue == true, "remove");
    QVERIFY2(affectedContactIds.count() == 2, "remove");
    QVERIFY2(affectedContactIds.toList().contains(groupContact.localId()), "remove");
    QVERIFY2(affectedContactIds.toList().contains(contact.localId()), "remove");
    QVERIFY2(error == QContactManager::NoError, "remove");
}

/*
 * Test valid group relationships
 */
void TestCntRelationship::validGroupRelationships()
{
}

/*
 * Test invalid group relationships
 */
void TestCntRelationship::invalidGroupRelationship()
{
}

/*
 * Test invalid group passed to grouprelationship functions
 */
void TestCntRelationship::invalidFirstContactGroupRelationship()
{
    //create a contact
    QContact contact;
    contact.setType(QContactType::TypeContact);
    QContactManager::Error err;
    m_manager->saveContact(&contact, &err);

    //create relationship
    QContactId invalidId;
    QContactRelationship relationship;
    relationship.setRelationshipType(QContactRelationship::HasMember);
    relationship.setFirst(invalidId);
    relationship.setSecond(contact.id());

    //save relationship
    bool returnValue(false);
    QContactManager::Error error;
    QSet<QContactLocalId> affectedContactIds;
    returnValue = m_relationship->saveRelationship(&affectedContactIds, &relationship, &error);
    QVERIFY(returnValue == false);
    QVERIFY(affectedContactIds.count() == 0);
    QVERIFY(error != QContactManager::NoError);

    //remove relationship
    returnValue = m_relationship->removeRelationship(&affectedContactIds, relationship, &error);
    QVERIFY(returnValue == false);
    QVERIFY(affectedContactIds.count() == 0);
    QVERIFY(error != QContactManager::NoError);
}

/*
 * Test invalid group memeber, passed to group relationship functions
 */
void TestCntRelationship::invalidSecondContactGroupRelationship()
{
    //create a group
    QContact groupContact;
    groupContact.setType(QContactType::TypeGroup);
    QContactManager::Error err;
    m_manager->saveContact(&groupContact, &err);

    //create relationship
    QContactId invalidId;
    QContactRelationship relationship;
    relationship.setRelationshipType(QContactRelationship::HasMember);
    relationship.setFirst(groupContact.id());
    relationship.setSecond(invalidId);

    //save relationship
    bool returnValue(false);
    QContactManager::Error error;
    QSet<QContactLocalId> affectedContactIds;
    returnValue = m_relationship->saveRelationship(&affectedContactIds, &relationship, &error);
    QVERIFY2(returnValue == false, "save");
    QVERIFY2(affectedContactIds.count() == 0, "save");
    QVERIFY2(error != QContactManager::NoError, "save");

    //remove relationship
    returnValue = m_relationship->removeRelationship(&affectedContactIds, relationship, &error);
    QVERIFY2(returnValue == false, "remove");
    QVERIFY2(affectedContactIds.count() == 0, "remove");
    QVERIFY2(error != QContactManager::NoError, "remove");
    
    //remove relationship: cyclic relantionship
    relationship.setRelationshipType(QContactRelationship::HasMember);
    relationship.setFirst(groupContact.id());
    relationship.setSecond(groupContact.id());
    returnValue = m_relationship->removeRelationship(&affectedContactIds, relationship, &error);
    QVERIFY2(returnValue == false, "remove");
    QVERIFY2(affectedContactIds.count() == 0, "remove");
    QVERIFY2(error != QContactManager::NoError, "remove");
}

/*
 * Test invalid group and group member, passed to group relationship functions
 */
void TestCntRelationship::invalidFirstAndSecondContactGroupRelationship()
{
    //create relationship
    QContactId invalidId;
    QContactRelationship relationship;
    relationship.setRelationshipType(QContactRelationship::HasMember);
    relationship.setFirst(invalidId);
    relationship.setSecond(invalidId);

    //save relationship
    bool returnValue(false);
    QContactManager::Error error;
    QSet<QContactLocalId> affectedContactIds;
    returnValue = m_relationship->saveRelationship(&affectedContactIds, &relationship, &error);
    QVERIFY(returnValue == false);
    QVERIFY(affectedContactIds.count() == 0);
    QVERIFY(error != QContactManager::NoError);

    //remove relationship
    returnValue = m_relationship->removeRelationship(&affectedContactIds, relationship, &error);
    QVERIFY(returnValue == false);
    QVERIFY(affectedContactIds.count() == 0);
    QVERIFY(error != QContactManager::NoError);
    
    //remove relationship: first contact's manager is not the same as cntrelationship's 
    QContactRelationship relationshipDifManagers;
    QContactId id1;
    id1.setLocalId(10);
    id1.setManagerUri("manager1");
    QContactId id2;
    id2.setLocalId(15);
    id2.setManagerUri("manager2");
    relationshipDifManagers.setRelationshipType(QContactRelationship::HasMember);
    relationshipDifManagers.setFirst(id1);
    relationshipDifManagers.setSecond(id2);
    returnValue = m_relationship->removeRelationship(&affectedContactIds, relationshipDifManagers, &error);
    QVERIFY(returnValue == false);
    QVERIFY(affectedContactIds.count() == 0);
    QVERIFY(error != QContactManager::NoError);
    
    //remove relationship: second contact's manager is not the same as cntrelationship's
    QContact contact;
    contact.setType(QContactType::TypeContact);
    QContactManager::Error err;
    m_manager->saveContact(&contact, &err);
    QContactRelationship relationshipDifManagers2;
    id2.setLocalId(15);
    id2.setManagerUri("manager2");
    relationshipDifManagers2.setRelationshipType(QContactRelationship::HasMember);
    relationshipDifManagers2.setFirst(contact.id());
    relationshipDifManagers2.setSecond(id2);
    returnValue = m_relationship->removeRelationship(&affectedContactIds, relationshipDifManagers2, &error);
    QVERIFY(returnValue == false);
    QVERIFY(affectedContactIds.count() == 0);
    QVERIFY(error != QContactManager::NoError);
}

/*
 * Test batch saving/removing group relationship functionality
 */
void TestCntRelationship::batchOperationGroupRelationship()
{
    //create two groups
    QContact groupContact1;
    groupContact1.setType(QContactType::TypeGroup);
    QContactManager::Error err;
    m_manager->saveContact(&groupContact1, &err);
    
    QContact groupContact2;
    groupContact2.setType(QContactType::TypeGroup);
    m_manager->saveContact(&groupContact2, &err);

    //create contacts
    QContact contact1;
    contact1.setType(QContactType::TypeContact);
    m_manager->saveContact(&contact1, &err);
    
    QContact contact2;
    contact2.setType(QContactType::TypeContact);
    m_manager->saveContact(&contact2, &err);

    QList <QContactRelationship> inputList;
    QList <QContactRelationship> relationshipList;
    //create relationships 
    QContactRelationship relationship;
    relationship.setRelationshipType(QContactRelationship::HasMember);
    relationship.setFirst(groupContact1.id());
    relationship.setSecond(contact1.id());
    inputList.append(relationship);
    relationship.setSecond(contact2.id());
    inputList.append(relationship);
    QSet<QContactLocalId> affectedContacts;
    QMap<int,QContactManager::Error> errorMap;
    
    QVERIFY(true==m_relationship->saveRelationships(&affectedContacts,&inputList,&errorMap, &err));
    QVERIFY(err == QContactManager::NoError);
    QVERIFY(affectedContacts.count() == 3); 
    relationshipList = m_relationship->relationships(QLatin1String(QContactRelationship::HasMember), groupContact1.id(), QContactRelationship::First, &err);
    QVERIFY(relationshipList.count()==2);
    QVERIFY(relationshipList[0].second()==contact1.id() || relationshipList[1].second()==contact1.id());
    QVERIFY(relationshipList[0].second()==contact2.id() || relationshipList[1].second()==contact2.id());
    
    //Remove the relationships
    QVERIFY(true==m_relationship->removeRelationships(&affectedContacts,relationshipList,&errorMap, &err));
    QVERIFY(err == QContactManager::NoError);
    QVERIFY(affectedContacts.count() == 3); 
    relationshipList = m_relationship->relationships(QLatin1String(QContactRelationship::HasMember), groupContact1.id(), QContactRelationship::First, &err);
    QVERIFY(relationshipList.count()==0);
    
    
    //Save relationships to two different group
    inputList.clear();
    relationship.setRelationshipType(QContactRelationship::HasMember);
    relationship.setFirst(groupContact1.id());
    relationship.setSecond(contact1.id());
    inputList.append(relationship);
    relationship.setFirst(groupContact2.id());
    relationship.setSecond(contact2.id());
    inputList.append(relationship);
    QVERIFY(true==m_relationship->saveRelationships(&affectedContacts,&inputList,&errorMap,&err));
    QVERIFY(err == QContactManager::NoError);
    //QVERIFY(affectedContacts.count() == 4); 
    relationshipList = m_relationship->relationships(QLatin1String(QContactRelationship::HasMember), groupContact1.id(), QContactRelationship::First, &err);
    QVERIFY(relationshipList.count()==1);
    QVERIFY(relationshipList[0].second()==contact1.id()); 
    relationshipList = m_relationship->relationships(QLatin1String(QContactRelationship::HasMember), groupContact2.id(), QContactRelationship::First, &err);
    QVERIFY(relationshipList.count()==1);
    QVERIFY(relationshipList[0].second()==contact2.id()); 
    
    //Remove the relationships
    QVERIFY(true==m_relationship->removeRelationships(&affectedContacts,inputList,&errorMap,&err));
    QVERIFY(err == QContactManager::NoError);
    QVERIFY(affectedContacts.count() == 4); 
    relationshipList = m_relationship->relationships(QLatin1String(QContactRelationship::HasMember), groupContact1.id(), QContactRelationship::First, &err);
    QVERIFY(relationshipList.count()==0);
    relationshipList = m_relationship->relationships(QLatin1String(QContactRelationship::HasMember), groupContact2.id(), QContactRelationship::First, &err);
    QVERIFY(relationshipList.count()==0);
}

bool TestCntRelationship::validateRelationshipFilter(const QContactRelationship::Role role, const QContactId contactId, const QList<QContactLocalId> expectedContacts)
    {
    QContactRelationshipFilter filter;
    filter.setRelationshipType(QContactRelationship::HasMember);
    filter.setRelatedContactRole(role);
    filter.setRelatedContactId(contactId);
    
    QContactManager::Error err;
    QList<QContactLocalId> result = m_manager->contactIds(filter, QList<QContactSortOrder>(), &err);
    
    for(int i = 0; i < result.count(); i++)
        qDebug() << "result: " << result.at(i);
    
    for(int j = 0; j < expectedContacts.count(); j++)
            qDebug() << "expectedContacts: " << expectedContacts.at(j);
    
    return (result == expectedContacts);
    }

