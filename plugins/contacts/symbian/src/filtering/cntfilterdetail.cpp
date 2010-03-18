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

#include <centralrepository.h>

#include "cntfilterdetail.h"
#include "cnttransformcontact.h"
#include "cntfilterdetaildisplaylabel.h" //todo rename class to follow naming pattern CntFilterDetailDisplayLabel
#include "cntsqlsearch.h"

// Telephony Configuration API
// Keys under this category are used in defining telephony configuration.
const TUid KCRUidTelConfiguration = {0x102828B8};
// Amount of digits to be used in contact matching.
// This allows a customer to variate the amount of digits to be matched.
const TUint32 KTelMatchDigits                               = 0x00000001;
// Default match length
const TInt KDefaultMatchLength(7);


CntFilterDetail::CntFilterDetail(CContactDatabase& contactDatabase,CntSymbianSrvConnection &cntServer,CntDbInfo& dbInfo) 
                                        : m_contactdatabase(contactDatabase),
                                          m_srvConnection(cntServer),
                                          m_dbInfo(dbInfo)
{

}

CntFilterDetail::~CntFilterDetail()
{
    
}


QList<QContactLocalId> CntFilterDetail::contacts(
        const QContactFilter &filter,
        const QList<QContactSortOrder> &sortOrders,
        bool &filterSupportedflag,
        QContactManager::Error &error)  
{
    Q_UNUSED(sortOrders);
    Q_UNUSED(filterSupportedflag);
    //Check if any invalid filter is passed 
    if(!filterSupported(filter) )
        {
        error =  QContactManager::NotSupportedError;
        return QList<QContactLocalId>();
        }
    QList<QContactLocalId> idList;
    QContactDetailFilter detailFilter(filter);
    QString sqlQuery;
    //Check for phonenumber. Special handling needed
    if( (detailFilter.detailDefinitionName() == QContactPhoneNumber::DefinitionName ) &&
            (detailFilter.detailFieldName() != QContactPhoneNumber::FieldSubTypes))
    {
        //Handle phonenumber ...
        idList = HandlePhonenumberDetailFilter(filter);
        
    }
    else if (detailFilter.matchFlags() == QContactFilter::MatchKeypadCollation) 
    {
        //predictive search filter
        idList = HandlePredictiveSearchFilter(filter,error);
    }
            
    // handle other cases
    else 
    {
        createSelectQuery(filter,sqlQuery,error);
        QString sortQuery = m_dbInfo.getSortQuery(sortOrders, sqlQuery, error);
        
        if(error == QContactManager::NoError)
            {
            //fetch the contacts
            idList =  m_srvConnection.searchContacts(sortQuery, error);
            }
    }
    
    return idList;

    
    
}


bool CntFilterDetail::filterSupported(const QContactFilter& filter) 
{
    bool result = false;
    if(QContactFilter::ContactDetailFilter == filter.type())
        {
        result = true;
        }
 
    return result;
}

void CntFilterDetail::createSelectQuery(const QContactFilter& filter,
                              QString& sqlQuery,
                              QContactManager::Error& error)

{
      if(!filterSupported(filter) )
      {
          error = QContactManager::NotSupportedError;
          return;
      }
      QContactDetailFilter detailFilter(filter);
      //display label
      if (detailFilter.detailDefinitionName() == QContactDisplayLabel::DefinitionName)
       {
          CntFilterDetailDisplayLabel displayLabelFilter;
          displayLabelFilter.createSelectQuery(filter, sqlQuery, error);
       }
       
       //type
       else if(detailFilter.detailDefinitionName() == QContactType::DefinitionName)
       {
           if(detailFilter.value().toString() == QContactType::TypeContact)
               sqlQuery = "SELECT contact_id FROM contact WHERE (type_flags>>24)=0";
           else if(detailFilter.value().toString() == QContactType::TypeGroup)
               sqlQuery = "SELECT contact_id FROM contact WHERE (type_flags>>24)=3";
       }
       else if(detailFilter.detailDefinitionName() == QContactGuid::DefinitionName)
       {
           if(detailFilter.detailFieldName() == QContactGuid::FieldGuid)
           {
               QStringList fullGuidValue = detailFilter.value().toString().split('-');
               if (fullGuidValue.count() == 3) {
                   QString localGuidValue = fullGuidValue.at(1);
                   sqlQuery = "SELECT contact_id FROM contact WHERE guid_string = '" + localGuidValue + '\'';
                }
            }
       }
       //everything else
       else
       {   
           QString tableName;
           QString sqlWhereClause;
           getTableNameWhereClause(detailFilter,tableName,sqlWhereClause,error);
           //Create the sql query
           sqlQuery += "SELECT DISTINCT contact_id FROM  " + tableName + " WHERE " + sqlWhereClause;
       }
}


/*!
 * Updates match flage for columns.
 */
void CntFilterDetail::updateForMatchFlag( const QContactDetailFilter& filter,
                                          QString& fieldToUpdate ,
                                          QContactManager::Error& error) const
{
    // Modify the filed depending on the query
    switch(filter.matchFlags())
        {
            case QContactFilter::MatchExactly:
                {
                // Pattern for MatchExactly:
                // " ='xyz'"
                fieldToUpdate = " ='"
                               + filter.value().toString() + '\'';
                error = QContactManager::NoError;
                break;
                }
            case QContactFilter::MatchContains:
                {
                // Pattern for MatchContains:
                // " LIKE '%xyz%'"
                fieldToUpdate = " LIKE '%" + filter.value().toString() + "%'" ;
                error = QContactManager::NoError;
                break;
                }
            case QContactFilter::MatchStartsWith:
                {
                // Pattern for MatchStartsWith:
                // " LIKE 'xyz%'"
                fieldToUpdate = " LIKE '" +  filter.value().toString() + "%'"  ;
                error = QContactManager::NoError;
                break;
                }
            case QContactFilter::MatchEndsWith:
                {
                // Pattern for MatchEndsWith:
                // " LIKE '%xyz'"
                fieldToUpdate = " LIKE '%" + filter.value().toString() + '\'' ;
                error = QContactManager::NoError;
                break;
                }
            case QContactFilter::MatchFixedString:
                {
                error = QContactManager::NotSupportedError;
                break;
                }
            case QContactFilter::MatchCaseSensitive:
                {
                error = QContactManager::NotSupportedError;
                break;
                }
            default:
                {
                error = QContactManager::NotSupportedError;
                break;
                }
        }
        
}

void CntFilterDetail::getTableNameWhereClause( const QContactDetailFilter& detailfilter,
                                               QString& tableName,
                                               QString& sqlWhereClause ,
                                               QContactManager::Error& error) const
{
    //Get the table name and the column name
    bool isSubType;
    QString columnName;

    //Get the field id for the detail field name
    CntTransformContact transformContact;
    quint32 fieldId  = transformContact.GetIdForDetailL(detailfilter, isSubType);
    m_dbInfo.getDbTableAndColumnName(fieldId,tableName,columnName);
    
    //return if tableName is empty
    if(tableName.isEmpty())
        {
        error = QContactManager::NotSupportedError;
        return;
        }

    //check columnName
    if(columnName.isEmpty())
        {
        error = QContactManager::NotSupportedError;
        return;
        }
    else if(isSubType) 
        {
        sqlWhereClause += columnName;
        sqlWhereClause += " NOT NULL ";
        }
    else 
        {
        sqlWhereClause += ' ' + columnName + ' ';
        QString fieldToUpdate;
        //Update the value depending on the match flag
        updateForMatchFlag(detailfilter,fieldToUpdate,error);
        sqlWhereClause +=  fieldToUpdate;
        }
}

QList<QContactLocalId>  CntFilterDetail::HandlePredictiveSearchFilter(const QContactFilter& filter,QContactManager::Error& error)
    {
    
    QString sqlQuery;
    
    if(filter.type() == QContactFilter::ContactDetailFilter){
       const QContactDetailFilter detailFilter(filter);
       if(  detailFilter.matchFlags() == QContactFilter::MatchKeypadCollation ){
       CntSqlSearch sqlSearch;
          //convert string to numeric format
            QString pattern = detailFilter.value().toString();
            sqlQuery = sqlSearch.CreatePredictiveSearch(pattern);
            return  m_srvConnection.searchContacts(sqlQuery, error);  
           }
       else
           {
           return QList<QContactLocalId>();
           }
        }
    else
        {
        return QList<QContactLocalId>();
        }
    }

QList<QContactLocalId> CntFilterDetail::HandlePhonenumberDetailFilter(const QContactFilter& filter)
    {
    QList<QContactLocalId> matches;
    QContactDetailFilter detailFilter(filter);
    // Phone numbers need separate handling

    QString number((detailFilter.value()).toString());
    TPtrC commPtr(reinterpret_cast<const TUint16*>(number.utf16()));

    TInt matchLength(KDefaultMatchLength);
    // no need to propagate error, we can use the default match length
    TRAP_IGNORE(getMatchLengthL(matchLength));
    int actualLength = number.length();

    //call the search
    CContactIdArray* idArray(0);
    TInt err = searchPhoneNumbers(idArray, commPtr, matchLength);
    if( idArray && (err == KErrNone)){
        // copy the matching contact ids
        for(int i(0); i < idArray->Count(); i++) {
            matches.append(QContactLocalId((*idArray)[i]));
        }
        delete idArray;
    }
    else{
        //CntSymbianTransformError::transformError(err, error);
        }


    
    return matches;

    }
/*
 * Get the match length setting. Digits to be used in matching (counted from
 * right).
 */
void CntFilterDetail::getMatchLengthL(TInt& matchLength)
{
    //Get number of digits used to match
    CRepository* repository = CRepository::NewL(KCRUidTelConfiguration);
    CleanupStack::PushL(repository);
    User::LeaveIfError(repository->Get(KTelMatchDigits, matchLength));
    CleanupStack::PopAndDestroy(repository);
}

/*
 * Find contacts based on a phone number.
 * \a idArray On return contains the ids of the contacts that match the phonenumber.
 * \a phoneNumber The phone number to match
 * \a matchLength Match length; digits from right.
 */
TInt CntFilterDetail::searchPhoneNumbers(
        CContactIdArray*& idArray,
        const TDesC& phoneNumber,
        const TInt matchLength)
{
    CContactIdArray* idArrayTmp(0);
    TRAPD( err, idArrayTmp = m_contactdatabase.MatchPhoneNumberL(phoneNumber, matchLength));
    if(err == KErrNone){
       idArray = idArrayTmp;
    }
    return 0;
}
