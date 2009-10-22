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

#include "ut_relationship.h"
#include "cntrelationship.h"
#include <qtcontacts.h>

#include <QtTest/QtTest>

#define QTRY_COMPARE(__expr, __expected) \
    do { \
        const int __step = 50; \
        const int __timeout = 10000; \
        QTest::qWait(10); \
        for (int __i = 0; __i < __timeout && ((__expr) != (__expected)); __i+=__step) { \
            QTest::qWait(__step); \
        } \
        QCOMPARE(__expr, __expected); \
    } while(0)


void TestRelationship::initTestCase()
{
    
}

void TestRelationship::cleanupTestCase()
{
}

void TestRelationship::createGroupContact()
{
    QContactManager manager("symbian");
    
    //Create group contact
    QContact groupContact;
    groupContact.setType(QContactType::TypeGroup);
        
    QContactPhoneNumber number;
    number.setNumber("1312313");
    groupContact.saveDetail(&number);
    
    manager.saveContact(&groupContact);
    QVERIFY(groupContact.localId() != 0);
    
    //Create contact 1
    QContact contact;
    contact.setType(QContactType::TypeContact);
        
    number.setNumber("1312313");
    contact.saveDetail(&number);
    
    manager.saveContact(&contact);
    QVERIFY(contact.localId() != 0);
    
    //Create contact 2
    QContact contact2;
    contact.setType(QContactType::TypeContact);
    manager.saveContact(&contact2);
    QVERIFY(contact2.localId() != 0);
    
    //Add contact 1 to group
    QContactRelationship relationship; 
    relationship.setRelationshipType(QContactRelationship::HasMember);
    relationship.setFirst(groupContact.id());
    relationship.setSecond(contact.id());
    
    QSignalSpy spySaveRelationship(&manager, SIGNAL(relationshipsAdded(const QList<QContactLocalId>& affectedContactIds)));
    //QSignalSpy spyRemoveRelationship(&manager, SIGNAL(relationshipsRemoved(const QList<QContactLocalId>& affectedContactIds)));
    manager.saveRelationship(&relationship);
    QTRY_COMPARE(spySaveRelationship.count(), 1); 
    
    //Add contact 2 to group
    relationship.setSecond(contact2.id());
    manager.saveRelationship(&relationship);
    
    
    //retrieve the relationships
    QList<QContactRelationship> relationshipList;
    
    //group
    relationshipList = manager.relationships(groupContact.id(), QContactRelationshipFilter::First);
    QVERIFY2(relationshipList.count() == 2, "group - First");
    
    relationshipList = manager.relationships(groupContact.id(), QContactRelationshipFilter::Second);
    QVERIFY2(relationshipList.count() == 0, "group - Second");
    
    relationshipList = manager.relationships(groupContact.id(), QContactRelationshipFilter::Either);
    QVERIFY2(relationshipList.count() == 2, "group - Either");
    
    
    //contact 1
    relationshipList = manager.relationships(contact.id(), QContactRelationshipFilter::First);
    QVERIFY2(relationshipList.count() == 0, "contact - First");
    
    relationshipList = manager.relationships(contact.id(), QContactRelationshipFilter::Second);
    QVERIFY2(relationshipList.count() == 1, "contact - Second");
    
    relationshipList = manager.relationships(contact.id(), QContactRelationshipFilter::Either);
    QVERIFY2(relationshipList.count() == 1, "contact - Either");
    
    
    //contact 2
    relationshipList = manager.relationships(contact2.id(), QContactRelationshipFilter::First);
    QVERIFY2(relationshipList.count() == 0, "contact - First");
    
    relationshipList = manager.relationships(contact2.id(), QContactRelationshipFilter::Second);
    QVERIFY2(relationshipList.count() == 1, "contact - First");
    
    relationshipList = manager.relationships(contact2.id(), QContactRelationshipFilter::Either);
    QVERIFY2(relationshipList.count() == 1, "contact - First");
    
    
    
}
#if 0
void TestRelationship::executeRelationships()
{
    CntRelationship *rel = new CntRelationship;
    
    QString relationshipType; 
    QContactId participantId;
    QContactRelationshipFilter::Role role;
    QContactManager::Error error;
        
    QList<QContactRelationship> returnValue = rel->relationships(relationshipType, participantId, role, error);
    
    delete rel;
}

void TestRelationship::executeSaveRelationship()
{
    CntRelationship *rel = new CntRelationship;
    
    QContactRelationship relationship; 
    QContactManager::Error error;
        
    bool returnValue = rel->saveRelationship(&relationship, error);
    
    delete rel;
}

void TestRelationship::executeSaveRelationships()
{
    CntRelationship *rel = new CntRelationship;
    
    QList<QContactRelationship> relationships;
    QContactManager::Error error;
        
    QList<QContactManager::Error> returnValue = rel->saveRelationships(&relationships, error);
    
    delete rel;
}

void TestRelationship::executeRemoveRelationship()
{
    CntRelationship *rel = new CntRelationship;
    
    QContactRelationship relationship; 
    QContactManager::Error error;
        
    bool returnValue = rel->removeRelationship(relationship, error);
    
    delete rel;
}

void TestRelationship::executeRemoveRelationships()
{
    CntRelationship *rel = new CntRelationship;
    
    QList<QContactRelationship> relationships;
    QContactManager::Error error;
    QList<QContactManager::Error> returnValue = rel->removeRelationships(relationships, error);
    
    delete rel;
}
#endif

QTEST_MAIN(TestRelationship);
