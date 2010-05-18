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
#include <cntdef.h>
#include "cntdbinfo.h"
#include "cntfilterdetail.h"


CntDbInfo::CntDbInfo()       
{
    QString id("%1.%2");
    contactsTableIdColumNameMapping.insert(id.arg(QContactName::DefinitionName, QContactName::FieldFirstName), "first_name");
    contactsTableIdColumNameMapping.insert(id.arg(QContactName::DefinitionName, QContactName::FieldLastName), "last_name");
    contactsTableIdColumNameMapping.insert(id.arg(QContactOrganization::DefinitionName, QContactOrganization::FieldName), "company_name");
    contactsTableIdColumNameMapping.insert(id.arg(QContactName::DefinitionName, QContactName::FieldCustomLabel), "text_fields");

    commAddrTableIdColumNameMapping.insert(id.arg(QContactOnlineAccount::DefinitionName, QContactOnlineAccount::FieldSubTypes), QPair<int,bool>(ESipAddress,true));
    commAddrTableIdColumNameMapping.insert(id.arg(QContactEmailAddress::DefinitionName, QContactEmailAddress::FieldEmailAddress), QPair<int,bool>(EEmailAddress,false));
    commAddrTableIdColumNameMapping.insert(id.arg(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldSubTypes), QPair<int,bool>(EPhoneNumber,true));
}

CntDbInfo::~CntDbInfo()
{

    contactsTableIdColumNameMapping.clear();
    commAddrTableIdColumNameMapping.clear();
}

bool CntDbInfo::SupportsDetail(QString definitionName, QString fieldName)
{
    QString fieldId = QString("%1.%2").arg(definitionName, fieldName);
    if (contactsTableIdColumNameMapping.contains(fieldId))
        return true;
    else if (commAddrTableIdColumNameMapping.contains(fieldId))
        return true;
    else
        return false;
}

/*!
 * Converts filed id to column name of the database table.
 * QContactManager::contacts function.
 *
 * \a fieldId field id representing the detail field name
 * \a sqlDbTableColumnName On return,contains the column name in the database
 */
void CntDbInfo::getDbTableAndColumnName( const QString definitionName,
                                         const QString fieldName,
                                         QString& tableName,
                                         QString& columnName,
                                         bool& isSubType) const
{
    //check contacts table
    QString fieldId = definitionName + "." + fieldName;

    columnName = "";
    tableName = "";
    isSubType = false;

    if (contactsTableIdColumNameMapping.contains(fieldId)){
         columnName = contactsTableIdColumNameMapping.value(fieldId);
         tableName = "contact";
     }

    if (columnName.isEmpty() || tableName.isEmpty()) {
        //Search comm Addr table
        if (commAddrTableIdColumNameMapping.contains(fieldId)) {
            // communication address table has slightly differnt format, so we make the column name as
            //  "type = <type> and value "
            QPair<int,bool> val = commAddrTableIdColumNameMapping.value(fieldId);
            int typeval = val.first;
            isSubType = val.second;
            columnName =  QString(" TYPE = %1").arg(typeval);
            columnName += " and value " ;
            tableName = "comm_addr";
        }
    }
}

QString CntDbInfo::getSortQuery( const QList<QContactSortOrder> &sortOrders,
                                 const QString& selectQuery,
                                 QContactManager::Error* error)
{
    // Set to initial select query
    QString sortQuery =  selectQuery;
    
    if(*error == QContactManager::NoError)
        {
        QList<QString> list;
        foreach(const QContactSortOrder& s, sortOrders )
            {
            QString tableName;
            QString columnName;
            bool isSubType;

            // Get column names for sort order
            getDbTableAndColumnName(s.detailDefinitionName(), s.detailFieldName(), tableName, columnName, isSubType);
            
            if (tableName.compare("contact") != 0 || columnName.isEmpty()) {
                // Skip invalid sort clause
                continue;
            }
            else {
                if (s.direction() == Qt::DescendingOrder) {
                    QString col;
                    if (s.caseSensitivity() == Qt::CaseInsensitive)
                        col = ' ' + columnName + ' ' + "COLLATE NOCASE DESC";
                    else
                        col = ' ' + columnName + ' ' + "DESC";
                    list.append(col);
                }
                else {
                    // Default sort order
                    QString col;
                    if(s.caseSensitivity() == Qt::CaseInsensitive)
                        col= ' ' + columnName + ' ' + "COLLATE CompareC3";
                    else
                        col= ' ' + columnName + ' ' + "ASC";
                    list.append(col);
                }
            }
        }
        
        if (list.count() > 0) {
            // Recreate query
            // SELECT DISTINCT contact_id FROM contact WHERE contact_id in (
            //      SELECT ..
            // )  ORDER BY  <field> <order>
            sortQuery = " SELECT DISTINCT contact_id FROM contact WHERE contact_id in (";
            QString clause = " ORDER BY " + list.at(0);
            for (int i = 1; i < list.size(); ++i) {
                clause += " ," + list.at(i);
            }
            sortQuery += selectQuery + ')';
            sortQuery += clause;
        }
    }
    
    return sortQuery;
}


