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

/*
 * This file contains example code for common use cases as implemented with
 * the current proposal for relationships as a separate, managed entity.
 */

// Assume we have a valid manager initialised
QContactManager contactManager;

// Use Case 1: Retrieve A List Of Groups Synchronously
QList<QContactLocalId> allGroups = contactManager.contacts(QContactType::TypeGroup);

// Use Case 2: Retrieve A List Of Groups Asynchronously
QContactDetailFilter df;
df.setDetailDefinition(QContactType::DefinitionName, QContactType::FieldType);
df.setValue(QContactType::TypeGroup);
QContactLocalIdFetchRequest cifr;
cifr.setManager(&contactManager);
cifr.setFilter(df);
cifr.start();
cifr.waitForFinished();
QList<QContactLocalId> groupIds = cifr.ids();

// Use Case 3: Retrieve Details Of A Particular Group
QContactLocalId groupId = groupIds.at(0);
QContact theGroup = contactManager.contact(groupId);
QList<QContactDetail> groupDetails = theGroup.details();

// Use Case 4: Retrieve The Groups Which A Particular Contact Is A Member Of
QContactRelationshipFilter crf;
crf.setRole(QContactRelationshipFilter::Source);                 // we want "source" contact ids
crf.setType(QContactRelationship::HasMember);                    // which have as a member
crf.setOtherParticipant(someContact.id());                       // the particular contact.
QList<QContactLocalId> matchingGroups = contactManager.contacts(crf);

// Use Case 5: Return Groups In-Line With Other Contacts
QList<QContactLocalId> allContactsAndGroups = contactManager.contacts();

// Use Case 6: Return Only Contacts Which Are Not Groups
QList<QContactLocalId> allContacts = contactManager.contacts(QContactType::TypeContact);

// Use Case 7: Matching An Incoming Call To A Group
QContactDetailFilter cdf;
cdf.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldNumber);
cdf.setValue(theIncomingPhoneNumber);
QContactDetailFilter cdf2;
cdf2.setDetailDefinition(QContactType::DefinitionName, QContactType::FieldType);
cdf2.setValue(QContactType::TypeGroup);
QContactIntersectionFilter cif;
cif << cdf << cdf2;
QList<QContactLocalId> matchingGroups = contactManager.contacts(cif);

// Use Case 8: Top Level Groups (ie, non-nested groups)
// This use case is still under discussion.
// - if we disallow nested groups (ie, during validation of saving a relationship) then this cannot happen
// - otherwise, we'll need to either add a new Contact Type (TopLevelGroup) or do complex filtering.

// Use Case 9: Retrieving The Members Of A Group (Option 1)
// Advantage: know that the information is up to date
// Disadvantage: only get local id's (of contacts which are stored in the manager)
QContactRelationshipFilter crf;
crf.setRole(QContactRelationshipFilter::Destination);           // we want all destination particpants
crf.setType(QContactRelationship::HasMember);                   // ie, group members
crf.setOtherParticipant(someGroupContact.id());                 // where I am the source (group)
QList<QContactLocalId> groupMembers = contactManager.contacts(crf);

// Use Case 10: Retrieving The Members Of A Group (Option 2)
// Advantage: get _all_ groups members no matter which manager they are part of
// Disadvantage: cached information, possibly stale.
QContact someGroup;
QList<QContactId> groupMembers = someGroup.relatedContacts(QContactRelationship::HasMember, QContactRelationshipFilter::Destination);

// Use Case 11: Modify Ordering Of Group Members
QContactRelationship someGroupRelationship;
QList<QContactId> groupMembers = someGroupRelationship.destinationContacts();
QContactId someEntry = groupMembers.takeAt(3);                  // move from position 3
groupMembers.insert(6, someEntry);                              // to position 6
someGroupRelationship.setDestinationContacts(groupMembers);
contactManager.saveRelationship(someGroupRelationship);

// Use Case 12: Adding A Contact To A Group
QContactRelationship someGroupRelationship;
someGroupRelationship.appendDestinationContact(someContact.id());
contactManager.saveRelationship(someGroupRelationship);

