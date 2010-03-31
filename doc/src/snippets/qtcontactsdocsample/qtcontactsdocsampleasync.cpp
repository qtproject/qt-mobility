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

#include "qmobilityglobal.h"
#include "qtcontacts.h"
#include "requestexample.h"

#include <QDebug>
#include <QCoreApplication>
#include <QObject>
#include <QTimer>

QTM_USE_NAMESPACE

AsyncRequestExample::AsyncRequestExample()
    : QObject()
{
    m_manager = new QContactManager("memory");
}

AsyncRequestExample::~AsyncRequestExample()
{
    delete m_manager;
}

//! [Example of an asynchronous request slot]
void AsyncRequestExample::contactFetchRequestStateChanged(QContactAbstractRequest::State newState)
{
    if (newState == QContactAbstractRequest::FinishedState) {
        QContactFetchRequest *request = qobject_cast<QContactFetchRequest*>(QObject::sender());
        if (request->error() != QContactManager::NoError) {
            qDebug() << "Error" << request->error() << "occurred during fetch request!";
            return;
        }

        QList<QContact> results = request->contacts();
        for (int i = 0; i < results.size(); i++) {
            qDebug() << "Retrieved contact:" << results.at(i).displayLabel();
        }
    } else if (newState == QContactAbstractRequest::CanceledState) {
        qDebug() << "Fetch operation canceled!";
    }
}
//! [Example of an asynchronous request slot]

void AsyncRequestExample::contactSaveRequestStateChanged(QContactAbstractRequest::State newState)
{
    if (newState == QContactAbstractRequest::FinishedState)
        qDebug() << "Finished saving the contacts!";
    else if (newState == QContactAbstractRequest::CanceledState)
        qDebug() << "Save operation canceled!";
}

void AsyncRequestExample::contactRemoveRequestStateChanged(QContactAbstractRequest::State newState)
{
    if (newState == QContactAbstractRequest::FinishedState)
        qDebug() << "Finished removing the contacts!";
    else if (newState == QContactAbstractRequest::CanceledState)
        qDebug() << "Remove operation canceled!";
}

void AsyncRequestExample::relationshipFetchRequestStateChanged(QContactAbstractRequest::State newState)
{
    if (newState == QContactAbstractRequest::FinishedState)
        qDebug() << "Finished fetching the contacts!";
    else if (newState == QContactAbstractRequest::CanceledState)
        qDebug() << "Fetch operation canceled!";
}

void AsyncRequestExample::relationshipSaveRequestStateChanged(QContactAbstractRequest::State newState)
{
    if (newState == QContactAbstractRequest::FinishedState)
        qDebug() << "Finished saving the contacts!";
    else if (newState == QContactAbstractRequest::CanceledState)
        qDebug() << "Save operation canceled!";
}

void AsyncRequestExample::relationshipRemoveRequestStateChanged(QContactAbstractRequest::State newState)
{
    if (newState == QContactAbstractRequest::FinishedState)
        qDebug() << "Finished removing the contacts!";
    else if (newState == QContactAbstractRequest::CanceledState)
        qDebug() << "Remove operation canceled!";
}

void AsyncRequestExample::definitionFetchRequestStateChanged(QContactAbstractRequest::State newState)
{
    if (newState == QContactAbstractRequest::FinishedState)
        qDebug() << "Finished fetching the contacts!";
    else if (newState == QContactAbstractRequest::CanceledState)
        qDebug() << "Fetch operation canceled!";
}

void AsyncRequestExample::definitionSaveRequestStateChanged(QContactAbstractRequest::State newState)
{
    if (newState == QContactAbstractRequest::FinishedState)
        qDebug() << "Finished saving the contacts!";
    else if (newState == QContactAbstractRequest::CanceledState)
        qDebug() << "Save operation canceled!";
}

void AsyncRequestExample::definitionRemoveRequestStateChanged(QContactAbstractRequest::State newState)
{
    if (newState == QContactAbstractRequest::FinishedState)
        qDebug() << "Finished removing the contacts!";
    else if (newState == QContactAbstractRequest::CanceledState)
        qDebug() << "Remove operation canceled!";
}

void AsyncRequestExample::performRequests()
{
//! [Creating a new contact in a manager]
    QContact exampleContact;

    QContactName nameDetail;
    nameDetail.setFirstName("Adam");
    nameDetail.setLastName("Unlikely");

    QContactPhoneNumber phoneNumberDetail;
    phoneNumberDetail.setNumber("+123 4567");

    exampleContact.saveDetail(&nameDetail);
    exampleContact.saveDetail(&phoneNumberDetail);

    // save the newly created contact in the manager
    connect(&m_contactSaveRequest, SIGNAL(stateChanged(QContactAbstractRequest::State)), this, SLOT(contactSaveRequestStateChanged(QContactAbstractRequest::State)));
    m_contactSaveRequest.setManager(m_manager);
    m_contactSaveRequest.setContacts(QList<QContact>() << exampleContact);
    m_contactSaveRequest.start();
//! [Creating a new contact in a manager]

    m_contactSaveRequest.waitForFinished();

//! [Creating a new contact in a manager waiting until finished]
    m_contactSaveRequest.setManager(m_manager);
    m_contactSaveRequest.setContacts(QList<QContact>() << exampleContact);
    m_contactSaveRequest.start();
    m_contactSaveRequest.waitForFinished();
    QList<QContact> savedContacts = m_contactSaveRequest.contacts();
//! [Creating a new contact in a manager waiting until finished]

//! [Filtering contacts from a manager]
    connect(&m_contactFetchRequest, SIGNAL(stateChanged(QContactAbstractRequest::State)), this, SLOT(contactFetchRequestStateChanged(QContactAbstractRequest::State)));
    m_contactFetchRequest.setManager(m_manager);
    m_contactFetchRequest.setFilter(QContactPhoneNumber::match("+123 4567"));
    m_contactFetchRequest.start();
//! [Filtering contacts from a manager]

    m_contactFetchRequest.waitForFinished();

//! [Retrieving an existing contact from a manager]
    QContactLocalIdFilter idListFilter;
    idListFilter.setIds(QList<QContactLocalId>() << exampleContact.localId());
    m_contactFetchRequest.setManager(m_manager);
    m_contactFetchRequest.setFilter(idListFilter);
    m_contactFetchRequest.start();
//! [Retrieving an existing contact from a manager]

    m_contactFetchRequest.waitForFinished();

//! [Updating an existing contact in a manager]
    phoneNumberDetail.setNumber("+123 9876");
    exampleContact.saveDetail(&phoneNumberDetail);
    m_contactSaveRequest.setManager(m_manager);
    m_contactSaveRequest.setContacts(QList<QContact>() << exampleContact);
    m_contactSaveRequest.start();
//! [Updating an existing contact in a manager]

    m_contactFetchRequest.waitForFinished();

//! [Removing a contact from a manager]
    connect(&m_contactRemoveRequest, SIGNAL(stateChanged(QContactAbstractRequest::State)), this, SLOT(contactRemoveRequestStateChanged(QContactAbstractRequest::State)));
    m_contactRemoveRequest.setManager(m_manager);
    m_contactRemoveRequest.setContactIds(QList<QContactLocalId>() << exampleContact.localId());
    m_contactRemoveRequest.start();
//! [Removing a contact from a manager]

    m_contactFetchRequest.waitForFinished();

//! [Creating a new relationship between two contacts]
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
    QList<QContact> saveList;
    saveList << exampleGroup << exampleGroupMember;
    m_contactSaveRequest.setContacts(saveList);
    m_contactSaveRequest.start();
    m_contactSaveRequest.waitForFinished();

    // third, create the relationship between those contacts
    QContactRelationship groupRelationship;
    groupRelationship.setFirst(exampleGroup.id());
    groupRelationship.setRelationshipType(QContactRelationship::HasMember);
    groupRelationship.setSecond(exampleGroupMember.id());

    // finally, save the relationship in the manager
    connect(&m_relationshipSaveRequest, SIGNAL(stateChanged(QContactAbstractRequest::State)), this, SLOT(relationshipSaveRequestStateChanged(QContactAbstractRequest::State)));
    m_relationshipSaveRequest.setManager(m_manager);
    m_relationshipSaveRequest.setRelationships(QList<QContactRelationship>() << groupRelationship);
    m_relationshipSaveRequest.start();
//! [Creating a new relationship between two contacts]

    m_contactFetchRequest.waitForFinished();

//! [Retrieving relationships between contacts]
    connect(&m_relationshipFetchRequest, SIGNAL(stateChanged(QContactAbstractRequest::State)), this, SLOT(relationshipFetchRequestStateChanged(QContactAbstractRequest::State)));
    m_relationshipFetchRequest.setManager(m_manager);
    // retrieve the list of relationships between the example group contact and the example member contact
    // where the group contact is the first contact in the relationship, and the member contact is the
    // second contact in the relationship.  In order to fetch all relationships between them, another
    // relationship fetch must be performed with their roles reversed, and the results added together.
    m_relationshipFetchRequest.setFirst(exampleGroup.id());
    m_relationshipFetchRequest.setSecond(exampleGroupMember.id());
    m_relationshipFetchRequest.start();
//! [Retrieving relationships between contacts]

    m_contactFetchRequest.waitForFinished();

//! [Providing a fetch hint]
    QContactFetchHint hasMemberRelationshipsOnly;
    hasMemberRelationshipsOnly.setRelationshipTypesHint(QStringList(QContactRelationship::HasMember));

    m_contactFetchRequest.setManager(m_manager);
    m_contactFetchRequest.setFilter(QContactFilter()); // all contacts
    m_contactFetchRequest.setFetchHint(hasMemberRelationshipsOnly);
    m_contactFetchRequest.start();
//! [Providing a fetch hint]

//! [Removing a relationship]
    connect(&m_relationshipRemoveRequest, SIGNAL(stateChanged(QContactAbstractRequest::State)), this, SLOT(relationshipRemoveRequestStateChanged(QContactAbstractRequest::State)));
    m_relationshipRemoveRequest.setManager(m_manager);
    m_relationshipRemoveRequest.setRelationships(QList<QContactRelationship>() << groupRelationship);
    m_relationshipRemoveRequest.start();
//! [Removing a relationship]

    connect(&m_definitionFetchRequest, SIGNAL(stateChanged(QContactAbstractRequest::State)), this, SLOT(definitionFetchRequestStateChanged(QContactAbstractRequest::State)));
//! [Querying the schema supported by a manager]
    m_definitionFetchRequest.setManager(m_manager);
    m_definitionFetchRequest.setDefinitionNames(QStringList(QContactName::DefinitionName));
    m_definitionFetchRequest.start();
    m_definitionFetchRequest.waitForFinished();
    QMap<QString, QContactDetailDefinition> definitions = m_definitionFetchRequest.definitions();
    qDebug() << "This manager"
             << (definitions.value(QContactName::DefinitionName).fields().contains(QContactName::FieldCustomLabel) ? "supports" : "does not support")
             << "the custom label field of QContactName";
//! [Querying the schema supported by a manager]

    connect(&m_definitionSaveRequest, SIGNAL(stateChanged(QContactAbstractRequest::State)), this, SLOT(definitionSaveRequestStateChanged(QContactAbstractRequest::State)));
    connect(&m_definitionRemoveRequest, SIGNAL(stateChanged(QContactAbstractRequest::State)), this, SLOT(definitionRemoveRequestStateChanged(QContactAbstractRequest::State)));
//! [Modifying the schema supported by a manager]
    // modify the name definition, adding a patronym field
    QContactDetailDefinition nameDefinition = definitions.value(QContactName::DefinitionName);
    QContactDetailFieldDefinition fieldPatronym;
    fieldPatronym.setDataType(QVariant::String);
    nameDefinition.insertField("Patronym", fieldPatronym);

    // save the updated definition in the manager if supported...
    if (m_manager->hasFeature(QContactManager::MutableDefinitions)) {
        m_definitionSaveRequest.setManager(m_manager);
        m_definitionSaveRequest.setContactType(QContactType::TypeContact);
        m_definitionSaveRequest.setDefinitions(QList<QContactDetailDefinition>() << nameDefinition);
        m_definitionSaveRequest.start();
    }
//! [Modifying the schema supported by a manager]

    QCoreApplication::exit(0);
}
