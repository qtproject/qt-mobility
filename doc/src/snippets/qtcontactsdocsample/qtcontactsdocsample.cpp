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

#include "qmobilityglobal.h"
#include "qtcontacts.h"
#include "requestexample.h"

#include <QDebug>
#include <QCoreApplication>
#include <QObject>
#include <QTimer>

QTM_USE_NAMESPACE

static void loadDefault();
static void queryManagerCapabilities();
static void contactDetailManipulation();
static void contactManipulation();
static void addContact(QContactManager*);
static void matchCall(QContactManager*, const QString&);
static void viewSpecificDetail(QContactManager*);
static void viewDetails(QContactManager*);
static void detailSharing(QContactManager*);
static void addPlugin(QContactManager*);
static void editView(QContactManager*);
static void loadManager();
static void loadManagerWithParameters();

int stopCompilerWarnings()
{
    // manager configuration examples
    loadManager();
    loadManagerWithParameters();

    // synchronous API examples
    QContactManager* cm = new QContactManager();
    addContact(cm);
    matchCall(cm, "111-222-333"); // unknown number.
    matchCall(cm, "12345678");    // alice's number.
    viewSpecificDetail(cm);
    viewDetails(cm);
    detailSharing(cm);
    addPlugin(cm);
    editView(cm);

    // asynchronous API example
    RequestExample re;
    re.setManager(cm);
    QTimer::singleShot(10, &re, SLOT(performRequest()));
    delete cm;

    // more doc snippet examples
    loadDefault();
    queryManagerCapabilities();
    contactDetailManipulation();
    contactManipulation();

    // async doc snippet examples
    AsyncRequestExample example;
    QTimer::singleShot(10, &example, SLOT(performRequests()));

    return 0;
}


void loadDefault()
{
//! [Loading the default manager for the platform]
    QContactManager stackDefaultContactManager;
//! [Loading the default manager for the platform]

//! [Loading the default manager for the platform on heap]
    QContactManager *heapDefaultContactManager = new QContactManager;
    // ... perform contact manipulation
    delete heapDefaultContactManager;
//! [Loading the default manager for the platform on heap]
}

void queryManagerCapabilities()
{
//! [Querying a manager for capabilities]
    QContactManager cm;
    qDebug() << "The default manager for the platform is:" << cm.managerName();
    qDebug() << "It" << (cm.isRelationshipTypeSupported(QContactRelationship::HasAssistant) ? "supports" : "does not support") << "assistant relationships.";
    qDebug() << "It" << (cm.supportedContactTypes().contains(QContactType::TypeGroup) ? "supports" : "does not support") << "groups.";
    qDebug() << "It" << (cm.hasFeature(QContactManager::MutableDefinitions) ? "supports" : "does not support") << "mutable detail definitions.";
//! [Querying a manager for capabilities]
}

void contactDetailManipulation()
{
//! [Adding a detail to a contact]
    QContact exampleContact;

    QContactName nameDetail;
    nameDetail.setFirstName("Adam");
    nameDetail.setLastName("Unlikely");

    QContactPhoneNumber phoneNumberDetail;
    phoneNumberDetail.setNumber("+123 4567");

    exampleContact.saveDetail(&nameDetail);
    exampleContact.saveDetail(&phoneNumberDetail);
//! [Adding a detail to a contact]

//! [Updating a detail in a contact]
    phoneNumberDetail.setNumber("+123 9876");
    exampleContact.saveDetail(&phoneNumberDetail); // overwrites old value on save
//! [Updating a detail in a contact]

//! [Removing a detail from a contact]
    exampleContact.removeDetail(&phoneNumberDetail);
//! [Removing a detail from a contact]
}

void contactManipulation()
{
    QContactManager m_manager("memory");
//! [Synchronously creating a new contact in a manager]
    QContact exampleContact;

    QContactName nameDetail;
    nameDetail.setFirstName("Adam");
    nameDetail.setLastName("Unlikely");

    QContactPhoneNumber phoneNumberDetail;
    phoneNumberDetail.setNumber("+123 4567");

    exampleContact.saveDetail(&nameDetail);
    exampleContact.saveDetail(&phoneNumberDetail);

    // save the newly created contact in the manager
    if (!m_manager.saveContact(&exampleContact))
        qDebug() << "Error" << m_manager.error() << "occurred whilst saving contact!";
//! [Synchronously creating a new contact in a manager]

//! [Synchronously filtering contacts from a manager]
    QList<QContact> results = m_manager.contacts(QContactPhoneNumber::match("+123 4567"));
//! [Synchronously filtering contacts from a manager]

//! [Synchronously retrieving an existing contact from a manager]
    QContact existing = m_manager.contact(exampleContact.localId());
//! [Synchronously retrieving an existing contact from a manager]

//! [Synchronously updating an existing contact in a manager]
    phoneNumberDetail.setNumber("+123 9876");
    exampleContact.saveDetail(&phoneNumberDetail);
    m_manager.saveContact(&exampleContact);
//! [Synchronously updating an existing contact in a manager]

//! [Synchronously removing a contact from a manager]
    m_manager.removeContact(exampleContact.localId());
//! [Synchronously removing a contact from a manager]

//! [Synchronously creating a new relationship between two contacts]
    // first, create the group and the group member
    QContact exampleGroup;
    exampleGroup.setType(QContactType::TypeGroup);
    QContactNickname groupName;
    groupName.setNickname("Example Group");
    exampleGroup.saveDetail(&groupName);

    QContact exampleGroupMember;
    QContactName groupMemberName;
    groupMemberName.setFirstName("Member");
    exampleGroupMember.saveDetail(&groupMemberName);

    // second, save those contacts in the manager
    QMap<int, QContactManager::Error> errorMap;
    QList<QContact> saveList;
    saveList << exampleGroup << exampleGroupMember;
    m_manager.saveContacts(&saveList, &errorMap);

    // third, create the relationship between those contacts
    QContactRelationship groupRelationship;
    groupRelationship.setFirst(exampleGroup.id());
    groupRelationship.setRelationshipType(QContactRelationship::HasMember);
    groupRelationship.setSecond(exampleGroupMember.id());

    // finally, save the relationship in the manager
    m_manager.saveRelationship(&groupRelationship);
//! [Synchronously creating a new relationship between two contacts]

//! [Synchronously retrieving relationships between contacts]
    QList<QContactRelationship> groupRelationships = m_manager.relationships(QContactRelationship::HasMember, exampleGroup.id(), QContactRelationship::First);
    QList<QContactRelationship> result;
    for (int i = 0; i < groupRelationships.size(); i++) {
        if (groupRelationships.at(i).second() == exampleGroupMember.id()) {
            result.append(groupRelationships.at(i));
        }
    }
//! [Synchronously retrieving relationships between contacts]

//! [Retrieving relationships from cache]
    exampleGroup = m_manager.contact(exampleGroup.localId()); // refresh the group contact
    groupRelationships = exampleGroup.relationships(QContactRelationship::HasMember);
    for (int i = 0; i < groupRelationships.size(); i++) {
        if (groupRelationships.at(i).second() == exampleGroupMember.id()) {
            result.append(groupRelationships.at(i));
        }
    }
//! [Retrieving relationships from cache]

//! [Synchronously providing a fetch hint]
    QContactFetchHint hasMemberRelationshipsOnly;
    hasMemberRelationshipsOnly.setRelationshipTypesHint(QStringList(QContactRelationship::HasMember));

    // retrieve all contacts, with no specified sort order, requesting that
    // HasMember relationships be included in the cache of result contacts
    QList<QContact> allContacts = m_manager.contacts(QContactFilter(), QList<QContactSortOrder>(), hasMemberRelationshipsOnly);
//! [Synchronously providing a fetch hint]

//! [Synchronously removing a relationship]
    m_manager.removeRelationship(groupRelationship);
//! [Synchronously removing a relationship]

//! [Synchronously querying the schema supported by a manager]
    QMap<QString, QContactDetailDefinition> definitions = m_manager.detailDefinitions();
    qDebug() << "This manager"
             << (definitions.value(QContactName::DefinitionName).fields().contains(QContactName::FieldCustomLabel) ? "supports" : "does not support")
             << "the custom label field of QContactName";
//! [Synchronously querying the schema supported by a manager]

//! [Synchronously modifying the schema supported by a manager]
    // modify the name definition, adding a patronym field
    QContactDetailDefinition nameDefinition = definitions.value(QContactName::DefinitionName);
    QContactDetailFieldDefinition fieldPatronym;
    fieldPatronym.setDataType(QVariant::String);
    nameDefinition.insertField("Patronym", fieldPatronym);

    // save the updated definition in the manager if supported...
    if (m_manager.hasFeature(QContactManager::MutableDefinitions)) {
        m_manager.saveDetailDefinition(nameDefinition, QContactType::TypeContact);
    }
//! [Synchronously modifying the schema supported by a manager]
}

//! [Creating a new contact]
void addContact(QContactManager* cm)
{
    QContact alice;

    /* Set the contact's name */
    QContactName aliceName;
    aliceName.setFirstName("Alice");
    aliceName.setLastName("Jones");
    aliceName.setCustomLabel("Ally Jones");
    alice.saveDetail(&aliceName);

    /* Add a phone number */
    QContactPhoneNumber number;
    number.setContexts(QContactDetail::ContextHome);
    number.setSubTypes(QContactPhoneNumber::SubTypeMobile);
    number.setNumber("12345678");
    alice.saveDetail(&number);

    /* Add a second phone number */
    QContactPhoneNumber number2;
    number2.setContexts(QContactDetail::ContextWork);
    number2.setSubTypes(QContactPhoneNumber::SubTypeLandline);
    number2.setNumber("555-4444");
    alice.saveDetail(&number2);

    /* Save the contact */
    cm->saveContact(&alice) ? qDebug() << "Successfully saved" << aliceName.customLabel()
                            : qDebug() << "Failed to save" << aliceName.customLabel();
    qDebug() << "The backend has synthesized a display label for the contact:" << alice.displayLabel();
}
//! [Creating a new contact]


//! [Filtering by definition and value]
void matchCall(QContactManager* cm, const QString& incomingCallNbr)
{
    QContactDetailFilter phoneFilter;
    phoneFilter.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldNumber);
    phoneFilter.setValue(incomingCallNbr);
    phoneFilter.setMatchFlags(QContactFilter::MatchExactly);

    QList<QContactLocalId> matchingContacts = cm->contactIds(phoneFilter);
    if (matchingContacts.size() == 0) {
        qDebug() << "Incoming call from unknown contact (" << incomingCallNbr << ")";
    } else {
        QContact match = cm->contact(matchingContacts.at(0));
        qDebug() << "Incoming call from"
                 << match.displayLabel()
                 << "(" << incomingCallNbr << ")";
    }
}
//! [Filtering by definition and value]

//! [Viewing a specific detail of a contact]
void viewSpecificDetail(QContactManager* cm)
{
    QList<QContactLocalId> contactIds = cm->contactIds();
    QContact a = cm->contact(contactIds.first());
    qDebug() << "The first phone number of" << a.displayLabel()
             << "is" << a.detail(QContactPhoneNumber::DefinitionName).value(QContactPhoneNumber::FieldNumber);
}
//! [Viewing a specific detail of a contact]

//! [Viewing the details of a contact]
void viewDetails(QContactManager* cm)
{
    QList<QContactLocalId> contactIds = cm->contactIds();
    QContact a = cm->contact(contactIds.first());
    qDebug() << "Viewing the details of" << a.displayLabel();

    QList<QContactDetail> allDetails = a.details();
    for (int i = 0; i < allDetails.size(); i++) {
        QContactDetail detail = allDetails.at(i);
        QContactDetailDefinition currentDefinition = cm->detailDefinition(detail.definitionName());
        QMap<QString, QContactDetailFieldDefinition> fields = currentDefinition.fields();

        qDebug("\tDetail #%d (%s):", i, detail.definitionName().toAscii().constData());
        foreach (const QString& fieldKey, fields.keys()) {
            qDebug() << "\t\t" << fieldKey << "(" << fields.value(fieldKey).dataType() << ") =" << detail.value(fieldKey);
        }
        qDebug();
    }
}
//! [Viewing the details of a contact]

//! [Demonstration of detail sharing semantics]
void detailSharing(QContactManager* cm)
{
    QList<QContactLocalId> contactIds = cm->contactIds();
    QContact a = cm->contact(contactIds.first());
    qDebug() << "Demonstrating detail sharing semantics with" << a.displayLabel();

    /* Create a new phone number detail. */
    QContactPhoneNumber newNumber;
    newNumber.setNumber("123123123");
    qDebug() << "\tThe new phone number is" << newNumber.number();

    /*
     * Create a copy of that detail.  These will be implicitly shared;
     * changes to nnCopy will not affect newNumber, and vice versa.
     * However, attempting to save them will cause overwrite to occur.
     * Removal is done purely via key() checking, also.
     */
    QContactPhoneNumber nnCopy(newNumber);
    nnCopy.setNumber("456456456");
    qDebug() << "\tThat number is still" << newNumber.number() << ", the copy is" << nnCopy.number();

    /* Save the detail in the contact, then remove via the copy, then resave. */
    a.saveDetail(&newNumber);
    a.removeDetail(&nnCopy);  // identical to a.removeDetail(&newNumber);
    a.saveDetail(&newNumber); // since newNumber.key() == nnCopy.key();

    /* Saving will cause overwrite */
    qDebug() << "\tPrior to saving nnCopy," << a.displayLabel() << "has" << a.details().count() << "details.";
    a.saveDetail(&nnCopy);
    qDebug() << "\tAfter saving nnCopy," << a.displayLabel() << "still has" << a.details().count() << "details.";

    /* In order to save nnCopy as a new detail, we must reset its key */
    nnCopy.resetKey();
    qDebug() << "\tThe copy key is now" << nnCopy.key() << ", whereas the original key is" << newNumber.key();
    qDebug() << "\tPrior to saving (key reset) nnCopy," << a.displayLabel() << "has" << a.details().count() << "details.";
    a.saveDetail(&nnCopy);
    qDebug() << "\tAfter saving (key reset) nnCopy," << a.displayLabel() << "still has" << a.details().count() << "details.";
    a.removeDetail(&nnCopy);

    /*
     * Note that changes made to details are not
     * propagated automatically to the contact.
     * To persist changes to a detail, you must call saveDetail().
     */
    QList<QContactPhoneNumber> allNumbers = a.details<QContactPhoneNumber>();
    foreach (const QContactPhoneNumber& savedPhn, allNumbers) {
        if (savedPhn.key() != newNumber.key()) {
            continue;
        }

        /*
         * This phone number is the saved copy of the newNumber detail.
         * It is detached from the newNumber detail, so changes to newNumber
         * shouldn't affect savedPhn until saveDetail() is called again.
         */
        qDebug() << "\tCurrently, the (stack) newNumber is" << newNumber.number()
                 << ", and the saved newNumber is" << savedPhn.number();
        newNumber.setNumber("678678678");
        qDebug() << "\tNow, the (stack) newNumber is" << newNumber.number()
                 << ", but the saved newNumber is" << savedPhn.number();
    }

    /*
     * Removal of the detail depends only on the key of the detail; the fact
     * that the values differ is not taken into account by the remove operation.
     */
    a.removeDetail(&newNumber) ? qDebug() << "\tSucceeded in removing the temporary detail."
                               : qDebug() << "\tFailed to remove the temporary detail.\n";
}
//! [Demonstration of detail sharing semantics]

//! [Installing a plugin which modifies a definition]
void addPlugin(QContactManager* cm)
{
    /* Find the definition that we are modifying */
    QMap<QString, QContactDetailDefinition> definitions = cm->detailDefinitions();
    QContactDetailDefinition modified = definitions.value(QContactEmailAddress::DefinitionName);

    /* Make our modifications: we add a "Label" field to email addresses */
    QContactDetailFieldDefinition newField;
    newField.setDataType(QVariant::String);
    QMap<QString, QContactDetailFieldDefinition> fields = modified.fields();
    fields.insert("Label", newField);

    /* Update the definition with the new field included */
    modified.setFields(fields);

    /* Save the definition back to the manager */
    if (cm->saveDetailDefinition(modified))
        qDebug() << "Successfully modified the detail definition!";
    else
        qDebug() << "This backend could not support our modifications!";
}
//! [Installing a plugin which modifies a definition]

//! [Modifying an existing contact]
void editView(QContactManager* cm)
{
    QList<QContactLocalId> contactIds = cm->contactIds();
    QContact a = cm->contact(contactIds.first());
    qDebug() << "Modifying the details of" << a.displayLabel();

    /* Change the first phone number */
    QList<QContactDetail> numbers = a.details(QContactPhoneNumber::DefinitionName);
    QContactPhoneNumber phone = numbers.value(0);
    phone.setNumber("123-4445");

    /* Add an email address */
    QContactEmailAddress email;
    email.setEmailAddress("alice.jones@example");
    email.setContexts(QContactDetail::ContextHome);
    email.setValue("Label", "Alice's Work Email Address");

    /* Save the updated details to the contact. */
    a.saveDetail(&phone);
    a.saveDetail(&email);

    /* Now we must save the updated contact back to the database. */
    cm->saveContact(&a);
    viewDetails(cm);
}
//! [Modifying an existing contact]

void displayLabel()
{
    QContactManager *manager = new QContactManager();
    QContactLocalId myId;
//! [Updating the display label of a contact]
    /* Retrieve a contact */
    QContact c = manager->contact(myId);
    qDebug() << "Current display label" << c.displayLabel();

    /* Update some fields that might influence the display label */
    QContactName name = c.detail<QContactName>();
    name.setFirstName("Abigail");
    name.setLastName("Arkansas");
    c.saveDetail(&name);

    /* Update the display label */
    manager->synthesizeContactDisplayLabel(&c);
    qDebug() << "Now the label is:" << c.displayLabel();
//! [Updating the display label of a contact]
}

//! [Asynchronous contact request]
void RequestExample::performRequest()
{
    // retrieve any contact whose first name is "Alice"
    QContactDetailFilter dfil;
    dfil.setDetailDefinitionName(QContactName::DefinitionName, QContactName::FieldFirstName);
    dfil.setValue("Alice");
    dfil.setMatchFlags(QContactFilter::MatchExactly);

    // m_fetchRequest was created with m_fetchRequest = new QContactFetchRequest() in the ctor.
    m_fetchRequest->setManager(this->m_manager); // m_manager is a QContactManager*.
    m_fetchRequest->setFilter(dfil);
    connect(m_fetchRequest, SIGNAL(resultsAvailable()), this, SLOT(printContacts()));
    connect(m_fetchRequest, SIGNAL(stateChanged(QContactAbstractRequest::State)),
            this, SLOT(stateChanged(QContactAbstractRequest::State)));
    if (!m_fetchRequest->start()) {
        qDebug() << "Unable to request contacts!";
        QCoreApplication::exit(0);
    } else {
        qDebug() << "Requested contacts; awaiting results...";
    }
}

void RequestExample::printContacts()
{
    QList<QContact> results = m_fetchRequest->contacts();
    for (m_previousLastIndex = 0; m_previousLastIndex < results.size(); ++m_previousLastIndex) {
        qDebug() << "Found an Alice:" << results.at(m_previousLastIndex).displayLabel();
    }
}

void RequestExample::stateChanged(QContactAbstractRequest::State state)
{
    // once we've finished retrieving results, stop processing events.
    if (state == QContactAbstractRequest::FinishedState
        || state == QContactAbstractRequest::CanceledState) {
        qDebug() << "Finished displaying asynchronously retrieved contacts!";
        QCoreApplication::exit(0);
    }
}
//! [Asynchronous contact request]


void shortsnippets()
{
    QContact contact;
    QContact groupContact;
    {
        //! [0]
        QContactDetail detail = contact.detail(QContactName::DefinitionName);
        //! [0]
        //! [1]
        QContactName name = contact.detail<QContactName>();
        //! [1]
        //! [2]
        QList<QContactDetail> details = contact.details(QContactPhoneNumber::DefinitionName);
        //! [2]
        //! [3]
        QList<QContactPhoneNumber> phoneNumbers = contact.details<QContactPhoneNumber>();
        //! [3]
        //! [4]
        QList<QContactPhoneNumber> homePhones = contact.details<QContactPhoneNumber>("Context", "Home");
        //! [4]
        //! [5]
        QList<QContactRelationship> spouseRelationships = contact.relationships(QContactRelationship::HasSpouse);
        // For each relationship in spouseRelationships, contact.id() will either be first() or second()
        //! [5]
        //! [6]
        // Who are the members of a group contact?
        QList<QContactId> groupMembers = groupContact.relatedContacts(QContactRelationship::HasMember, QContactRelationship::Second);
        // What groups is this contact in?
        QList<QContactId> contactGroups = contact.relatedContacts(QContactRelationship::HasMember, QContactRelationship::First);
        // An alternative to QContact::relationships()
        QList<QContactId> spouses = contact.relatedContacts(QContactRelationship::HasSpouse, QContactRelationship::Either);
        if (spouses.count() > 1) {
            // Custom relationship type
            QList<QContactId> therapists = contact.relatedContacts("HasTherapist", QContactRelationship::Second);
        }
        //! [6]
        //! [Getting all tags]
        QSet<QString> tags;
        foreach(const QContactTag& tag, contact.details<QContactTag>()) {
             tags.insert(tag.tag());
        }
        //! [Getting all tags]
        //! [Checking for a specific tag]
        if (contact.details<QContactTag>(QContactTag::FieldTag, "MyTag").count() > 0) {
            // Do something with it
        }
        //! [Checking for a specific tag]
    }
}

void loadManager()
{
//! [Loading a specific manager backend]
    QContactManager contactManager("KABC");
//! [Loading a specific manager backend]
}

void loadManagerWithParameters()
{
//! [Loading a specific manager backend with parameters]
    QMap<QString, QString> parameters;
    parameters.insert("Settings", "~/.qcontactmanager-kabc-settings.ini");
    QContactManager contactManager("KABC", parameters);
//! [Loading a specific manager backend with parameters]
}
