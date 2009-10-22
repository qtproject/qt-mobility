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

#include "cntabstractrelationship.h"

CntAbstractRelationship::CntAbstractRelationship(CContactDatabase* contactDatabase, const QString &relationshipType) : 
    m_contactDatabase(contactDatabase),
    m_relationshipType(relationshipType) 
{}
    
CntAbstractRelationship::~CntAbstractRelationship()
{}

CContactDatabase* CntAbstractRelationship::database()
{
    return m_contactDatabase;
}

QList<QContactRelationship> CntAbstractRelationship::relationshipsL(const QContactId& participantId, QContactRelationshipFilter::Role role, QContactManager::Error& error)
{
    Q_UNUSED(participantId);
    Q_UNUSED(role);
    
    error = QContactManager::NotSupportedError;

    QList<QContactRelationship> relationships;
    return relationships;
}


bool CntAbstractRelationship::saveRelationshipL(QSet<QContactLocalId> *affectedContactIds, QContactRelationship* relationship, QContactManager::Error& error)
{
    Q_UNUSED(affectedContactIds);
    Q_UNUSED(relationship);
    
    error = QContactManager::NotSupportedError;
    return false;
}


QList<QContactManager::Error> CntAbstractRelationship::saveRelationshipsL(QSet<QContactLocalId> *affectedContactIds, QList<QContactRelationship>* relationships, QContactManager::Error& error)
{
    Q_UNUSED(error);
    
    QContactManager::Error singleError(QContactManager::NoError);
    QList<QContactManager::Error> errorList;
    
    for(int i = 0; i < relationships->count(); i++ ){
        saveRelationshipL(affectedContactIds, &relationships->value(i), singleError);
        errorList.append(singleError);
    }    
    
    return errorList;
}

bool CntAbstractRelationship::removeRelationshipL(QSet<QContactLocalId> *affectedContactIds, const QContactRelationship& relationship, QContactManager::Error& error)
{
    Q_UNUSED(affectedContactIds);
    Q_UNUSED(relationship);
    
    error = QContactManager::NotSupportedError;
    return false;
}

QList<QContactManager::Error> CntAbstractRelationship::removeRelationshipsL(QSet<QContactLocalId> *affectedContactIds, const QList<QContactRelationship>& relationships, QContactManager::Error& error)
{
    Q_UNUSED(error);
    
    QContactManager::Error singleError(QContactManager::NoError);
    QList<QContactManager::Error> errorList;
    
    for(int i = 0; i < relationships.count(); i++ ){
        removeRelationshipL(affectedContactIds, relationships.at(i), singleError);
        errorList.append(singleError);
    }    
    
    return errorList;
}
