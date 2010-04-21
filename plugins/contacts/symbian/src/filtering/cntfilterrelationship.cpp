/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "cntfilterrelationship.h"
#include "cnttransformcontact.h"

QTM_USE_NAMESPACE



CntFilterRelationship::CntFilterRelationship(CContactDatabase& contactDatabase,CntSymbianSrvConnection &cntServer,CntDbInfo& dbInfo) 
                                        : m_contactdatabase(contactDatabase),
                                          m_srvConnection(cntServer),
                                          m_dbInfo(dbInfo)
{

}

CntFilterRelationship::~CntFilterRelationship()
{
    
}

QList<QContactLocalId> CntFilterRelationship::contacts(
        const QContactFilter &filter,
        const QList<QContactSortOrder> &sortOrders,
        bool &filterSupportedflag,
        QContactManager::Error *error)
{
    Q_UNUSED(sortOrders);
    Q_UNUSED(filterSupportedflag);
    //Check if any invalid filter is passed 
    if(!filterSupported(filter) )
        {
        *error =  QContactManager::NotSupportedError;
        return QList<QContactLocalId>();
        }
    QList<QContactLocalId> idList;
    QString sqlQuery;
    createSelectQuery(filter,sqlQuery,error);
    //fetch the contacts
    if(*error == QContactManager::NoError )
        {
        idList =  m_srvConnection.searchContacts(sqlQuery, error);
        }
    return idList;
}


bool CntFilterRelationship::filterSupported(const QContactFilter& filter) 
{
    bool result = false;
    if(QContactFilter::RelationshipFilter == filter.type())
    {
        QContactRelationshipFilter relationfilter(filter);
        if(relationfilter.relationshipType() == QContactRelationship::HasMember)
        {
            result = true;
        }
    }
 
    return result;
}

void CntFilterRelationship::createSelectQuery(const QContactFilter& filter,
                              QString& sqlQuery,
                              QContactManager::Error* error)

{
    if(!filterSupported(filter))
    {
      *error = QContactManager::NotSupportedError;
      return;
    }
        
    QContactRelationshipFilter relationfilter(filter);
    QContactId  id_to_search = relationfilter.relatedContactId();

    if(relationfilter.relatedContactRole() == QContactRelationship::First )
    {
        sqlQuery = QString("SELECT DISTINCT contact_group_member_id FROM  groups WHERE contact_group_id = %1").arg(id_to_search.localId());
    }
    else if(relationfilter.relatedContactRole() == QContactRelationship::Second )
    {
        sqlQuery = QString("SELECT DISTINCT contact_group_id FROM  groups WHERE contact_group_member_id = %1").arg(id_to_search.localId());
    }
    else if(relationfilter.relatedContactRole() == QContactRelationship::Either )
    {
        sqlQuery = QString("SELECT contact_group_member_id FROM  groups WHERE contact_group_id = %1").arg(id_to_search.localId());
               + " union " + 
               QString("SELECT DISTINCT  contact_group_id FROM  groups WHERE contact_group_id = %1").arg(id_to_search.localId());
    }
}


void CntFilterRelationship::getSqlquery( const QContactRelationshipFilter& relationfilter,
                                               QString& sqlquery ,
                                               QContactManager::Error* error) const
{
    Q_UNUSED(relationfilter);
    Q_UNUSED(sqlquery);
    Q_UNUSED(error);
}

