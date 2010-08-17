/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef CNTRELATIONSHIPGROUP_H
#define CNTRELATIONSHIPGROUP_H

#include "cntabstractrelationship.h"
#include <qtcontacts.h>
QTM_USE_NAMESPACE
class CntRelationshipGroup: public CntAbstractRelationship
{
public: 
    CntRelationshipGroup(CContactDatabase *contactDatabase, const QString &managerUri);
    virtual ~CntRelationshipGroup();

public:
    QList<QContactRelationship> relationshipsL(const QContactId &participantId, QContactRelationship::Role role, QContactManager::Error *error);
    bool saveRelationshipL(QSet<QContactLocalId> *affectedContactIds, QContactRelationship *relationship, QContactManager::Error *error);
    bool removeRelationshipL(QSet<QContactLocalId> *affectedContactIds, const QContactRelationship &relationship, QContactManager::Error *error);
    bool validateRelationship(const QContactRelationship &relationship, QContactManager::Error *error);
    
private:
    void isGroupMemberL(const CContactItem *contactItem, const TContactItemId groupId) const;
    void fetchGroupMembersL(const QContactLocalId &participantId, QList<QContactRelationship> *relationships);
    void fetchMemberOfGroupsL(const QContactLocalId &participantId, QList<QContactRelationship> *relationships);
    void fetchMembersOfAllGroupsL(QList<QContactRelationship> *relationships);
};

#endif
