/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "cntfilterunion.h"
#include "cntfilterdetail.h"
#include "cntfilterdefault.h"
#include "cntfilterrelationship.h"
#include "cntfilterintersection.h"
#include "cnttransformcontact.h"

CntFilterUnion::CntFilterUnion(CContactDatabase& contactDatabase,CntSymbianSrvConnection &cntServer,CntDbInfo& dbInfo) : 
                    m_contactdatabase(contactDatabase),
                    m_srvConnection(cntServer),
                    m_dbInfo(dbInfo)
{

}

CntFilterUnion::~CntFilterUnion()
{
    
}


QList<QContactLocalId> CntFilterUnion::contacts(
        const QContactFilter &filter,
        const QList<QContactSortOrder> &sortOrders,
        bool &filterSupportedflag,
        QContactManager::Error* error)
{
    Q_UNUSED(sortOrders);
    Q_UNUSED(filterSupportedflag);
    //Check if any invalid filter is passed 
    if(!filterSupported(filter))
        {
        *error = QContactManager::NotSupportedError;
        return QList<QContactLocalId>();
        }
     QList<QContactLocalId> idList;
     QString sqlQuery;
     createSelectQuery(filter,sqlQuery,error);
     QString sortQuery = m_dbInfo.getSortQuery(sortOrders, sqlQuery, error);
     //fetch the contacts
     if(*error == QContactManager::NoError )
         {
         idList =  m_srvConnection.searchContacts(sortQuery, error);
         }
     return idList;
}


bool CntFilterUnion::filterSupported(const QContactFilter& filter) 
{
    bool result = false;
     if(QContactFilter::UnionFilter == filter.type())
         {
         result = true;
         }
  
     return result;
}



void CntFilterUnion::createSelectQuery(const QContactFilter& filter,
                              QString& selectquery,
                              QContactManager::Error* error)

{
    const QContactUnionFilter& unionfilter = static_cast<const QContactUnionFilter&>(filter);
    if(!filterSupported(filter))
        {
        *error = QContactManager::NotSupportedError;
        return;
        }
        QList<QContactFilter> individualFilters = unionfilter.filters();
        //QString selectquery;
        int fltrcnt =  individualFilters.count();          
        if(fltrcnt)
            getSelectQueryforFilter(individualFilters[0],selectquery,error);

        for(int i=1; i< fltrcnt ; i++)
            {
        
            QString query;
            getSelectQueryforFilter(individualFilters[i],query,error);
            if(*error == QContactManager::NoError )
                {
                selectquery.append(" UNION ");
                selectquery += query;
                }
            
            }
        
            
}

void CntFilterUnion::getSelectQueryforFilter(const QContactFilter& filter,QString& sqlSelectQuery,QContactManager::Error* error)
    {
    switch(filter.type())
            {
            case QContactFilter::DefaultFilter:
                {
                CntFilterDefault defaultfltr(m_contactdatabase,m_srvConnection,m_dbInfo);
                defaultfltr.createSelectQuery(filter,sqlSelectQuery,error);
                break;
                }
            case QContactFilter::ContactDetailFilter:
                {
                QContactDetailFilter detailfilter(filter);
                if(detailfilter.detailDefinitionName() == QContactPhoneNumber::DefinitionName ) 
                    {
                    *error=QContactManager::NotSupportedError;
                    }
                else
                    {
                    CntFilterDetail dtlfltr(m_contactdatabase,m_srvConnection,m_dbInfo);
                    dtlfltr.createSelectQuery(filter,sqlSelectQuery,error);
                    }
                break;
                }
            case QContactFilter::RelationshipFilter:
                {
                CntFilterRelationship relationfltr(m_contactdatabase,m_srvConnection,m_dbInfo);
                relationfltr.createSelectQuery(filter,sqlSelectQuery,error);
                break;
                }
            case QContactFilter::IntersectionFilter:
                {
                sqlSelectQuery += "SELECT DISTINCT contact_id FROM (";
                CntFilterIntersection intersectionfltr(m_contactdatabase,m_srvConnection,m_dbInfo);
                intersectionfltr.createSelectQuery(filter,sqlSelectQuery,error);
                sqlSelectQuery += ')';
                break;
                }
            case QContactFilter::UnionFilter:
                {
                sqlSelectQuery += "SELECT DISTINCT contact_id FROM (";
                CntFilterUnion unionfltr(m_contactdatabase,m_srvConnection,m_dbInfo);
                unionfltr.createSelectQuery(filter,sqlSelectQuery,error);
                sqlSelectQuery += ')';
                break;
                }
            default:
                {
                *error = QContactManager::NotSupportedError;
                break;
                }
            }
    }
    
