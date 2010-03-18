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
#include <cntdef.h>
#include "cntdbinfo.h"
#include "cntmodelextuids.h"


CntDbInfo::CntDbInfo()       
{

    contactsTableIdColumNameMapping.insert(KUidContactFieldGivenName.iUid,"first_name" );
    contactsTableIdColumNameMapping.insert(KUidContactFieldGivenNamePronunciation.iUid,"firstname_prn" );
    contactsTableIdColumNameMapping.insert(KUidContactFieldFamilyName.iUid,"last_name" );
    contactsTableIdColumNameMapping.insert(KUidContactFieldFamilyNamePronunciation.iUid,"lastname_prn" );
    contactsTableIdColumNameMapping.insert(KUidContactFieldCompanyName.iUid,"company_name" );
    contactsTableIdColumNameMapping.insert(KUidContactFieldCompanyNamePronunciation.iUid,"companyname_prn" );
    
    commAddrTableIdColumNameMapping.insert(KUidContactFieldSIPID.iUid,ESipAddress );
    commAddrTableIdColumNameMapping.insert(KUidContactFieldVCardMapVOIP.iUid,ESipAddress );
    commAddrTableIdColumNameMapping.insert(KUidContactFieldVCardMapSWIS.iUid,ESipAddress );
    commAddrTableIdColumNameMapping.insert(KUidContactFieldIMPP.iUid,ESipAddress );
    commAddrTableIdColumNameMapping.insert(KUidContactFieldEMail.iUid,EEmailAddress );
    commAddrTableIdColumNameMapping.insert(KUidContactFieldPhoneNumber.iUid,EPhoneNumber );
}

CntDbInfo::~CntDbInfo()
{

    contactsTableIdColumNameMapping.clear();
    commAddrTableIdColumNameMapping.clear();
}

bool CntDbInfo::SupportsUid(int uid)
    {
    if(contactsTableIdColumNameMapping.contains(uid) )
        return true;
    else if (commAddrTableIdColumNameMapping.contains(uid))
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
void CntDbInfo::getDbTableAndColumnName( const quint32 fieldId ,
                                                  QString& tableName,
                                                  QString& columnName ) const
{
    //Uncomment later
    

    //check contacts table
    columnName = "";
    tableName = "";

    if (contactsTableIdColumNameMapping.contains(fieldId)){
         columnName = contactsTableIdColumNameMapping.value(fieldId);
         tableName = "contact";
     }

    if( (columnName.isEmpty())  || (tableName.isEmpty())){
        //Search comm Addr table
        if (commAddrTableIdColumNameMapping.contains(fieldId)){
                // communication address table has slightly differnt format, so we make the column name as
                //  "type = <type> and value "
                int typeval = commAddrTableIdColumNameMapping.value(fieldId) ;
                columnName =  QString(" TYPE = %1").arg(typeval);
                columnName += " and value " ;
                tableName = "comm_addr";
             }

    }

}

