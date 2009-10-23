/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#include "cntrelationshipgroup.h"
#include <cntitem.h>

#include <QDebug>

CntRelationshipGroup::CntRelationshipGroup(CContactDatabase* contactDatabase) : 
    CntAbstractRelationship(contactDatabase, QContactRelationship::HasMember)
{
}
    
CntRelationshipGroup::~CntRelationshipGroup()
{
}

bool CntRelationshipGroup::saveRelationshipL(QSet<QContactLocalId> *affectedContactIds, QContactRelationship* relationship, QContactManager::Error& error)
{
    bool returnValue(false);
    
    if(relationship->relationshipType() == this->relationshipType())
    {
        QContactId groupId   = relationship->first();
        QContactId contactId = relationship->second();
        
        database()->AddContactToGroupL(TContactItemId(contactId.localId()), TContactItemId(groupId.localId()));
        
        //add the group member to the list of affected contacts
        affectedContactIds->insert(contactId.localId());
    }

    else
    {
        error = QContactManager::NotSupportedError;
        returnValue = false;
    }
    
    return returnValue;
}

bool CntRelationshipGroup::removeRelationshipL(QSet<QContactLocalId> *affectedContactIds, const QContactRelationship& relationship, QContactManager::Error& error)
{
    bool returnValue(false);
    
    if(relationship.relationshipType() == this->relationshipType())
    {
        //get the ids of the relationship
        QContactId groupId   = relationship.first();
        QContactId contactId = relationship.second();
        
        //read the contacts from the database
        CContactItem* groupContact = database()->ReadContactLC(groupId.localId());
        CContactItem* contact      = database()->ReadContactLC(contactId.localId());
        
        //if both found remove the contact from the group
        if(groupContact && contact)
        {
            database()->RemoveContactFromGroupL(TContactItemId(contactId.localId()), TContactItemId(groupId.localId()));
            
            //add the removed group member to the list of affected contacts
            affectedContactIds->insert(contactId.localId());
        }
        
        CleanupStack::PopAndDestroy(groupContact);
        CleanupStack::PopAndDestroy(contact);
    }

    else
    {
        error = QContactManager::NotSupportedError;
        returnValue = false;
    }
    
    return returnValue;
}

//retrieve all the groups that the contact is part of
QList<QContactRelationship> CntRelationshipGroup::relationshipsL(const QContactId& participantId, QContactRelationshipFilter::Role role, QContactManager::Error& error) 
{
    Q_UNUSED(error);
    
    QList<QContactRelationship> returnValue;
    
    //role is a group
    if(role == QContactRelationshipFilter::First || role == QContactRelationshipFilter::Either)
    {
        fetchGroupMembersL(participantId, &returnValue);
    }
    
    //role is member of a group
    if(role == QContactRelationshipFilter::Second || role == QContactRelationshipFilter::Either)
    {
        fetchMemberOfGroupsL(participantId, &returnValue);
    }
    
    return returnValue;
}

void CntRelationshipGroup::fetchGroupMembersL(const QContactId& participantId, QList<QContactRelationship> *relationships)
{
    //fetch the contact item from the database
    CContactItem *contactItem = database()->ReadContactLC(participantId.localId());
    
    //make sure it's a group
    if(contactItem && contactItem->Type() == KUidContactGroup)
    {
        //cast the contact to a group
        CContactGroup *groupContact = static_cast<CContactGroup*>( contactItem );
    
        //create the relationship
        QContactRelationship relationship;
        relationship.setRelationshipType(QContactRelationship::HasMember);
    
        //get the group contacts
        const CContactIdArray *idArray = groupContact->ItemsContained();
       
        QContactId groupId;
        
        //loop through all the contacts and add them to the list
        for(int i = 0; i < idArray->Count(); i++ )
        {
            //set participant id as first id
            relationship.setFirst(participantId);
            
            //set the manager uri
            groupId.setManagerUri(participantId.managerUri());
                
            groupId.setLocalId(idArray->operator[](i));
            relationship.setSecond(groupId);
            
            *relationships << relationship;
        }
    }

    CleanupStack::PopAndDestroy(contactItem);
}


void CntRelationshipGroup::fetchMemberOfGroupsL(const QContactId& participantId, QList<QContactRelationship> *relationships)
{
    CContactItem *contactItem = database()->ReadContactLC(TContactItemId(participantId.localId()));
  
    //verify that we havea a contact
    if(contactItem)
    {
        //cast the contact item to base class for groups and contacts
        CContactItemPlusGroup *contact = static_cast<CContactItemPlusGroup *>(contactItem);
        
        //create the relationship
        QContactRelationship relationship;
        relationship.setRelationshipType(QContactRelationship::HasMember);
            
        const CContactIdArray *idArray = contact->GroupsJoined();
        
        QContactId groupId;
        
        //loop through all the contacts and add them to the list
        for(int i = 0; i < idArray->Count(); i++ )
        {
           //set the manager uri
           groupId.setManagerUri(participantId.managerUri());
           groupId.setLocalId(idArray->operator[](i));
          
           //set the group as first
           relationship.setFirst(groupId);
           
           //set participant id as member of group
           relationship.setSecond(participantId);
                       
           *relationships << relationship;
        }
    }

    CleanupStack::PopAndDestroy(contactItem);
}
