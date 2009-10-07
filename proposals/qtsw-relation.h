/*
 * This file contains example code for common use cases as implemented with
 * the current proposal for relationships as a separate, managed entity.
 */

// Assume we have a valid manager initialised
QContactManager contactManager;

// Use Case 1: Retrieve A List Of Groups Synchronously
QList<QUniqueId> allGroups = contactManager.contacts(QContactType::TypeGroup);

// Use Case 2: Retrieve A List Of Groups Asynchronously
QContactDetailFilter df;
df.setDetailDefinition(QContactType::DefinitionName, QContactType::FieldType);
df.setValue(QContactType::TypeGroup);
QContactIdFetchRequest cifr;
cifr.setManager(&contactManager);
cifr.setFilter(df);
cifr.start();
cifr.waitForFinished();
QList<QUniqueId> groupIds = cifr.ids();

// Use Case 3: Retrieve Details Of A Particular Group
QUniqueId groupId = groupIds.at(0);
QContact theGroup = contactManager.contact(groupId);
QList<QContactDetail> groupDetails = theGroup.details();

// Use Case 4: Retrieve The Groups Which A Particular Contact Is A Member Of
QUniqueId particularContactId;
QContactRelationshipFilter crf;
crf.setRole(QContactRelationshipFilter::Source);                 // we want "source" contact ids
crf.setType(QContactRelationship::RelationshipTypeHasMember);    // which have as a member
crf.setOtherParticipant(particularContactId);                    // the particular contact.
QList<QUniqueId> matchingGroups = contactManager.contacts(crf);

// Use Case 5: Return Groups In-Line With Other Contacts
QList<QUniqueId> allContactsAndGroups = contactManager.contacts();

// Use Case 6: Return Only Contacts Which Are Not Groups
QList<QUniqueId> allContacts = contactManager.contacts(QContactType::TypeContact);

// Use Case 7: Matching An Incoming Call To A Group
QString theIncomingPhoneNumber;
QContactDetailFilter cdf;
cdf.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldNumber);
cdf.setValue(theIncomingPhoneNumber);
QContactDetailFilter cdf2;
cdf2.setDetailDefinition(QContactType::DefinitionName, QContactType::FieldType);
cdf2.setValue(QContactType::TypeGroup);
QContactIntersectionFilter cif;
cif << cdf << cdf2;
QList<QUniqueId> matchingGroups = contactManager.contacts(cif);

// Use Case 8: Top Level Groups (ie, non-nested groups)
// This use case is still under discussion.
// - if we disallow nested groups (ie, during validation of saving a relationship) then this cannot happen
// - otherwise, we'll need to either add a new Contact Type (TopLevelGroup) or do complex filtering.

// Use Case 9: Retrieving The Members Of A Group
QUniqueId particularGroupId;
QContactRelationshipFilter crf;
crf.setRole(QContactRelationshipFilter::Destination);           // we want all destination particpants
crf.setType(QContactRelationship::RelationshipTypeHasMember);   // ie, group members
crf.setOtherParticipant(particularGroupId);                     // where I am the source (group)
QList<QUniqueId> groupMembers = contactManager.contacts(crf);

// Use Case 10: Modify Ordering Of Group Members
QContactRelationship someGroupRelationship;
QList<QPair<QString, QUniqueId> > groupMembers = someGroupRelationship.destinationContacts();
QPair<QString, QUniqueId> someEntry = groupMembers.takeAt(3);
groupMembers.insert(6, someEntry);
someGroupRelationship.setDestinationContacts(groupMembers);
contactManager.saveRelationship(someGroupRelationship);

// Use Case 11: Adding A Contact To A Group
QUniqueId particularContactId;
QContactRelationship someGroupRelationship;
QList<QPair<QString, QUniqueId> > groupMembers = someGroupRelationship.destinationContacts();
QPair<QString, QUniqueId> contactToAdd;
contactToAdd.second = particularContactId;
groupMembers.append(contactToAdd);
someGroupRelationship.setDestinationContacts(groupMembers);
contactManager.saveRelationship(someGroupRelationship);

