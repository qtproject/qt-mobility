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

#include "cntfilterunion.h"
#include "cntfilterdetail.h"
#include "cnttransformcontact.h"
#include "qcontactunionfilter.h"

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
        QContactManager::Error &error) 
{
    QList<QContactLocalId> idList;
    //Check if any invalid filter is passed 
    if(!filterSupported(filter))
        {
        error =  QContactManager::NotSupportedError;
        return QList<QContactLocalId>();
        }
    const QContactUnionFilter& unionfilter = static_cast<const QContactUnionFilter&>(filter);
    QString tableName;
    QString sqlQuery; 
    this->getSqlQuery( unionfilter,tableName,sqlQuery,error) ;
    //fetch the contacts
        idList =  m_srvConnection.searchContacts(sqlQuery, error);
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

void CntFilterUnion::createSelectQuery(const QContactFilter& detailFilter,
                              QString& sqlQuery,
                              QContactManager::Error& error)

{
    
    
}

void CntFilterUnion::getSqlQuery( const QContactUnionFilter& unionfilter,
                                               QString& tableName,
                                               QString& sqlquery ,
                                               QContactManager::Error& error) const
{
    // get the individual filters of the union filter
    
    QList<QContactFilter> individualFilters = unionfilter.filters();
    QList<QString> commAddrTableColList;
    QList<QString> contactTableColList;
    //currently only simple detail filter unions will be supported
    for(int i=0; i< individualFilters.count();i++)
        {
        if(individualFilters[i].type() ==  QContactFilter::ContactDetailFilter)
            {
            QString tablename;
            QString whereclause;
            CntFilterDetail filterdetail(m_contactdatabase,m_srvConnection,m_dbInfo);
            filterdetail.getTableNameWhereClause(individualFilters[i],tablename,whereclause,error);
            if(tablename == "contact")
                {
                contactTableColList.append(whereclause);
                }
            else if(tablename == "comm_addr")
                {
                commAddrTableColList.append(whereclause);
                }
            else
                {
                error =  QContactManager::NotSupportedError;
                return ;
                }
            }
        else
            {
            error =  QContactManager::NotSupportedError;
            return ;
            }
    
        }
    createUnionQuery(commAddrTableColList, contactTableColList,sqlquery);

    
        
}


void CntFilterUnion::createUnionQuery(QList<QString>& commAddrTableColList, QList<QString>& contactTableColList,QString& sqlquery) const
    {
    // Create the where clause for contactTableColList
    QString sqlWhereClause;
    //------------------------------------------------------------------------------------
    int j=0;
    if( contactTableColList.count() )
        sqlWhereClause += contactTableColList[j];
    for( j=1; j< contactTableColList.count() ;j++ )
        {
        sqlWhereClause += " OR ";    
        sqlWhereClause += contactTableColList[j];
        }

    //Create the complete query for contact table
    if(  contactTableColList.count())
        {
        //The where clause exists in both tables so create query involving both tables
        sqlquery = "SELECT contact_id FROM contact WHERE  " + sqlWhereClause ;
        }
    
    if(contactTableColList.count() && commAddrTableColList.count())
        sqlquery.append(" UNION ");
    //------------------------------------------------------------------------------------
    //Create complete query for comm_addr table
    // Create the where clause for commAddrTableColList
    sqlWhereClause = "";
    int k=0;
    if(commAddrTableColList.count())
        sqlWhereClause += commAddrTableColList[k];
    for( k=1; k< commAddrTableColList.count() ;k++ )
        {
        sqlWhereClause += " OR ";        
        sqlWhereClause += commAddrTableColList[k];
        }
    if( commAddrTableColList.count())
            {
            //The where clause exists in both tables so create query involving both tables
            sqlquery += "SELECT contact_id FROM comm_addr WHERE  " + sqlWhereClause ; 
            }
    //------------------------------------------------------------------------------------
    
    }
