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

#include "cntrelationship.h"
#include "qcontactsymbianengine_p.h"
#include "qcontactsymbiantransformerror.h"


CntRelationship::CntRelationship(CContactDatabase* contactDatabase) 
{
    CntAbstractRelationship *relationshipGroup = new CntRelationshipGroup(contactDatabase);
    m_relationshipMap.insert(relationshipGroup->relationshipType(), relationshipGroup);
}

CntRelationship::~CntRelationship()
{
    QMap<QString, CntAbstractRelationship *>::iterator itr; 
    
    for (itr = m_relationshipMap.begin(); itr != m_relationshipMap.end(); ++itr)
    {
        CntAbstractRelationship* value = itr.value();
        delete value;
        value = 0;
    }
}


/* Relationships between contacts */
QList<QContactRelationship> CntRelationship::relationships(const QString& relationshipType, const QContactId& participantId, QContactRelationshipFilter::Role role, QContactManager::Error& error) const
{
    QList<QContactRelationship> returnValue;
    
    //check if we support the relationship
    if(m_relationshipMap.contains(relationshipType))
    {
        //get the relationship
        CntAbstractRelationship *abstractRelationship = m_relationshipMap.value(relationshipType);
        
        //retrieve the relationships
        TRAPD(symbianError, returnValue = abstractRelationship->relationshipsL(participantId, role, error));
        
        //if error translate it into a qt error
        if (symbianError){
            qContactSymbianTransformError(symbianError, error);
        }
    }
    else{
        error = QContactManager::NotSupportedError;
    }
    
    return returnValue;
}

bool CntRelationship::saveRelationship(QSet<QContactLocalId> *affectedContactIds, QContactRelationship* relationship, QContactManager::Error& error)
{
    bool returnValue(false);
            
    if(m_relationshipMap.contains(relationship->relationshipType()))
    {
        //get the relationship
        CntAbstractRelationship *abstractRelationship = m_relationshipMap.value(relationship->relationshipType());
        
        //save the relationship
        TRAPD(symbianError, returnValue = abstractRelationship->saveRelationshipL(affectedContactIds, relationship, error));
        
        //if symbian error translate it into a qt error
        if (!symbianError){
            qContactSymbianTransformError(symbianError, error);
        }
    }
    else{
        error = QContactManager::NotSupportedError;
    }
    
    return returnValue;
}


QList<QContactManager::Error> CntRelationship::saveRelationships(QSet<QContactLocalId> *affectedContactIds, QList<QContactRelationship>* relationships, QContactManager::Error& error)
{
    Q_UNUSED(error);
    
    QList<QContactManager::Error> returnValue;
    QContactManager::Error singleError;
    
    //loop through the relationships
    for(int i = 0; i < relationships->count(); i++)
    {
        //save the relationship
        saveRelationship(affectedContactIds, &(relationships->operator[](i)), singleError);
        returnValue.append(error);
    }
   
    return returnValue;
}

bool CntRelationship::removeRelationship(QSet<QContactLocalId> *affectedContactIds, const QContactRelationship& relationship, QContactManager::Error& error)
{
    bool returnValue(false);
    
    if(m_relationshipMap.contains(relationship.relationshipType()))
    {
        //get the relationship
        CntAbstractRelationship *abstractRelationship = m_relationshipMap.value(relationship.relationshipType()); 
        
        TRAPD(symbianError, returnValue = abstractRelationship->removeRelationshipL(affectedContactIds, relationship, error));
        
        //if symbian error translate it into a qt error
        if (!symbianError){
            qContactSymbianTransformError(symbianError, error);
        }
    }
    else{
        error = QContactManager::NotSupportedError;
    }
    
    return returnValue;
}

QList<QContactManager::Error> CntRelationship::removeRelationships(QSet<QContactLocalId> *affectedContactIds, const QList<QContactRelationship>& relationships, QContactManager::Error& error)
{
    Q_UNUSED(error);
    
    QList<QContactManager::Error> returnValue;
    QContactManager::Error qtError(QContactManager::NoError);
    
    //loop through the relationships
    for(int i = 0; i < relationships.count(); i++)
    {
        //remove the relationships
        removeRelationship(affectedContactIds, relationships.at(i), qtError);

        //add the error value
        returnValue.append(qtError);
    }
   
    return returnValue;
}
