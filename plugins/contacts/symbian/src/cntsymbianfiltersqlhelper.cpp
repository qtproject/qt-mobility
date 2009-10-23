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
//system includes
#include <e32base.h>
#include <s32mem.h>

//user includes
#include "cntsymbianfiltersqlhelper.h"
#include "qcontactdetailfilter.h"
#include "cnttransformcontact.h"

// Constants
// To be removed. Should be defined in a header file
#define KCntSearchResultIdLists 99
#define KCntOpenDataBase 100 // = KCapabilityReadUserData

_LIT(KCntServerExe,"CNTSRV.EXE");   // Name of the exe for the Contacts server.
_LIT(KCntServerName,"CNTSRV");  // Name used to connect a session 
                                // to the Contacts server.

/** Maximum number of asynchronous IPC calls. */
const TInt KAsyncMessageSlots=6;

/* Contacts server version number. */ 
const TInt KCntServerMajorVersionNumber=1;
const TInt KCntServerMinorVersionNumber=1;
const TInt KCntServerBuildVersionNumber=1;

const TInt KGranularityRank = 8; //2^8 = 256 bytes
const TInt KDefaultPackagerSize = 3514; //Observed Techview Golden template size.

//Class documentation go here:
/*!
    \class CntSymbianFilterSqlHelper
    \brief Helper class for converting filter to sql queries
*/

/*!
 * The constructor
 */
CntSymbianFilterSqlHelper::CntSymbianFilterSqlHelper()
{
   m_srvConnection = new CntSrvConnection();
}

/*!
 * Destructor 
 */
CntSymbianFilterSqlHelper::~CntSymbianFilterSqlHelper()
{
    delete m_srvConnection;
}

/*!
 * Fetch search results from the database.
 * 
 * \a filter The simple/complex QContactFilter passed .
 * \a error On return, contains the possible error.
 * \return the list of matched contact ids
 */
QList<QContactLocalId> CntSymbianFilterSqlHelper::searchContacts(const QContactFilter& filter, 
                                                                  QContactManager::Error& error)
{
    // Create swl query from the filters
    QString sqlQuery;
    createSqlQuery(filter, sqlQuery, error);

    if( error != QContactManager::NoError) {
        return QList<QContactLocalId>();
    }
    // Query the database
    return m_srvConnection->searchContacts(sqlQuery, error);
}

/*!
 * converts complex filter into simple filters 
 *
 * \a filter The simple/complex QContactFilter passed .
 * \a sqlQuery The sql query that would be formed
 * \a error On return, contains the possible error.
 */
void CntSymbianFilterSqlHelper::createSqlQuery(const QContactFilter& filter,
                                                 QString& sqlQuery,
                                                 QContactManager::Error& error)
{
    //Check if it is a single filter
    bool IsOneLevelFilter  = isSingleFilter(filter,error);
    if( error != QContactManager::NoError) {  
        return;
    }

    if(IsOneLevelFilter) {
        //Single Filter, get the sql query here
        updateSqlQueryForSingleFilter(filter,sqlQuery,error);
    } else {
        // We have multiple filters. Combine these to form correct query
        // Not supported yet
        error = QContactManager::NotSupportedError;
    }
}

/*!
 * Checks if the given filter is a single filter or combination of filters
 *
 * \a filter The QContactFilter to be used.
 * \a error On return, contains the possible error in filtering/sorting.
 * \return True if the filters is single filter
 */

bool CntSymbianFilterSqlHelper::isSingleFilter(const QContactFilter& singlefilter, 
                                               QContactManager::Error& error) const 
{
    error = QContactManager::NoError;
    bool returnValue = false;
    switch (singlefilter.type()) {
        case QContactFilter::InvalidFilter :
        case QContactFilter::ContactDetailFilter:
        case QContactFilter::ContactDetailRangeFilter:
        case QContactFilter::ChangeLogFilter:
        case QContactFilter::DefaultFilter:
                        //All are single filters, return True
                        returnValue = true;
                        break;
        case QContactFilter::ActionFilter:
        case QContactFilter::IntersectionFilter:
        case QContactFilter::UnionFilter:
                        //All these are multiple filters
                        error = QContactManager::NotSupportedError;
                        returnValue = false;
                        break;
        default:
                        error = QContactManager::NotSupportedError;
                        returnValue = false;
                        break;
    };
    return returnValue;
}

/*!
 * Updates the input sql query for single filter
 *
 * \a filter The QContactFilter to be used.
 * \a sqlQuery The sql query that would be updated
 * \a error On return, contains the possible error 
 */    
void  CntSymbianFilterSqlHelper::updateSqlQueryForSingleFilter( const QContactFilter& filter,
                                                                QString& sqlQuery,
                                                                QContactManager::Error& error)
{ 

    //Currently we are using only contact table.
    sqlQuery += "SELECT contact_id FROM contact WHERE ";

    switch (filter.type()) {
           case QContactFilter::InvalidFilter :
                       // Not supported yet
                       error = QContactManager::NotSupportedError;
                       break;
           case QContactFilter::ContactDetailFilter:
                       updateSqlQueryForDetailFilter(filter,sqlQuery,error);
                       break;
           case QContactFilter::ContactDetailRangeFilter:
                       // Not supported yet
                       error = QContactManager::NotSupportedError;
                       break;
               
           case QContactFilter::ChangeLogFilter:
                       // Not supported yet
                       error = QContactManager::NotSupportedError;
                       break;
           case QContactFilter::DefaultFilter:

                       // Not supported yet
                       error = QContactManager::NotSupportedError;
                       break;
           case QContactFilter::ActionFilter:
           case QContactFilter::IntersectionFilter:
           case QContactFilter::UnionFilter:
                       //All these are multiple filters
                       // Not supported yet
                       error = QContactManager::NotSupportedError;
                       break;
           default:
                       error = QContactManager::NotSupportedError;
                       break;
    };
}

/*!
 * Updates the input sql query for detail filter
 *
 * \a filter The QContactFilter to be used.
 * \a sqlQuery The sql query that would be updated
 * \a error On return, contains the possible error 
 */ 
void CntSymbianFilterSqlHelper::updateSqlQueryForDetailFilter(const QContactFilter& filter,
                                                              QString& sqlQuery,
                                                              QContactManager::Error& error)    
{
    // cast the filter into detail filter
    const QContactDetailFilter cdf(filter);

    //Get the field id for the detail field name
    TransformContact transformContact;
    quint32 fieldId; // = transformContact.GetIdForDetailL(cdf.detailDefinitionName(),cdf.detailFieldName());
    QString sqlDbTableColumnName;
    //Get the corresponding column name 
    convertFieldIdToSqlDbColumnName(fieldId,sqlDbTableColumnName );

    //Update the query
    sqlQuery += sqlDbTableColumnName; 
    sqlQuery += " like ";

    //Get the value and update it to the query
    sqlQuery += "'";
    sqlQuery +=  cdf.value().toString();
    sqlQuery += "%";
    sqlQuery += "'";
    error = QContactManager::NoError;
}

/*!
 * Initilize the transform classes and use them for getting the 
 * supported field ids 
 */
void CntSymbianFilterSqlHelper::initializeTransformContactData()
{

/*
    //These can be added to normal list, if we loop through it.
    m_transformContactData.insert(Name, new TransformName);
    m_transformContactData.insert(Nickname, new TransformNickname);
    m_transformContactData.insert(PhoneNumber, new TransformPhoneNumber);
    m_transformContactData.insert(EmailAddress, new TransformEmail);
    m_transformContactData.insert(Address, new TransformAddress);
    m_transformContactData.insert(URL, new TransformUrl);
    m_transformContactData.insert(Birthday, new TransformBirthday);
    m_transformContactData.insert(OnlineAccount, new TransformOnlineAccount);
    m_transformContactData.insert(Organisation, new TransformOrganisation);
    m_transformContactData.insert(Avatar, new TransformAvatar);
    m_transformContactData.insert(SyncTarget, new TransformSyncTarget);
    m_transformContactData.insert(Gender, new TransformGender);
    m_transformContactData.insert(Anniversary, new TransformAnniversary);
    m_transformContactData.insert(Geolocation, new TransformGeolocation);
    */
}

/*!
 * Create mapping between database columns and field ids .
 */
void  CntSymbianFilterSqlHelper::createDatabaseColumnMap()
{

    initializeTransformContactData();

}

/*!
 * Converts filed id to column name of the database table.
 * QContactManager::contacts function.
 *
 * \a fieldId field id representing the detail field name 
 * \a sqlDbTableColumnName On return,contains the column name in the database 
 */
void CntSymbianFilterSqlHelper::convertFieldIdToSqlDbColumnName(const quint32 fieldId,
                                                                QString& sqlDbTableColumnName )
{
    if(fieldId == KUidContactFieldGivenName.iUid) {
        sqlDbTableColumnName += "first_name";
    } else {
        sqlDbTableColumnName += "last_name";
    }
}

