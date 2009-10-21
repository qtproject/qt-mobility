/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "cntrelationship.h"


CntRelationship::CntRelationship(CContactDatabase* contactDatabase) 
{
    CntAbstractRelationship *relationshipGroup = new CntRelationshipGroup(contactDatabase);
    m_relationshipMap.insert(relationshipGroup->relationshipType(), relationshipGroup);
}

CntRelationship::~CntRelationship()
{
}


/* Relationships between contacts */
QList<QContactRelationship> CntRelationship::relationshipsL(const QString& relationshipType, const QContactId& participantId, QContactRelationshipFilter::Role role, QContactManager::Error& error) const
{
    QList<QContactRelationship> returnValue;
    
    if(m_relationshipMap.contains(relationshipType)){
        returnValue = m_relationshipMap.value(relationshipType)->relationshipsL(participantId, role, error);
    }
    else{
        error = QContactManager::NotSupportedError;
    }
    
    return returnValue;
}

bool CntRelationship::saveRelationshipL(QContactRelationship* relationship, QContactManager::Error& error)
{
    bool returnValue(false);
            
    if(m_relationshipMap.contains(relationship->relationshipType())){
        returnValue = m_relationshipMap.value(relationship->relationshipType())->saveRelationshipL(relationship, error);
    }
    else{
        error = QContactManager::NotSupportedError;
    }
    
    return returnValue;
}


QList<QContactManager::Error> CntRelationship::saveRelationshipsL(QList<QContactRelationship>* relationships, QContactManager::Error& error)
{
    QList<QContactManager::Error> returnValue;
    
    for(int i = 0; i < relationships->count(); i++){
        QContactManager::Error error;
        saveRelationshipL(&relationships->value(i), error);
        returnValue.append(error);
    }
   
    return returnValue;
}

bool CntRelationship::removeRelationshipL(const QContactRelationship& relationship, QContactManager::Error& error)
{
    bool returnValue;
    
    if(m_relationshipMap.contains(relationship.relationshipType())){
        returnValue = m_relationshipMap.value(relationship.relationshipType())->removeRelationshipL(relationship, error);
    }
    else{
        error = QContactManager::NotSupportedError;
    }
}

QList<QContactManager::Error> CntRelationship::removeRelationshipsL(const QList<QContactRelationship>& relationships, QContactManager::Error& error)
{
    QList<QContactManager::Error> returnValue;
    
    for(int i = 0; i < relationships.count(); i++){
        QContactManager::Error error;
        removeRelationshipL(relationships.at(i), error);
        returnValue.append(error);
    }
   
    return returnValue;
}
