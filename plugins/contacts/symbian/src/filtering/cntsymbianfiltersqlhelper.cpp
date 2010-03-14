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
#include <centralrepository.h>

#include <QDebug>

//user includes
#include "cntsymbianfiltersqlhelper.h"
#include "qcontactdetailfilter.h"
#include "cnttransformcontact.h"
#include "cntdisplaylabel.h"
#include "cntdisplaylabelsqlfilter.h"
#include "cntsqlsearch.h"

// Telephony Configuration API
// Keys under this category are used in defining telephony configuration.
const TUid KCRUidTelConfiguration = {0x102828B8};
// Amount of digits to be used in contact matching.
// This allows a customer to variate the amount of digits to be matched.
const TUint32 KTelMatchDigits                               = 0x00000001;
// Default match length
const TInt KDefaultMatchLength(7);
//Class documentation go here:
/*!
    \class CntSymbianFilterSqlHelper
    \brief Helper class for converting filter to sql queries
*/

 Q_DEFINE_LATIN1_LITERAL(CntSymbianFilterSqlHelper::SingleQuote,"'")  ;
 Q_DEFINE_LATIN1_LITERAL(CntSymbianFilterSqlHelper::PercentSign,"%") ;
 Q_DEFINE_LATIN1_LITERAL(CntSymbianFilterSqlHelper::Space," ") ;
 Q_DEFINE_LATIN1_LITERAL(CntSymbianFilterSqlHelper::EqualTo,"=") ;
 Q_DEFINE_LATIN1_LITERAL(CntSymbianFilterSqlHelper::SqlLike," LIKE ") ;
 Q_DEFINE_LATIN1_LITERAL(CntSymbianFilterSqlHelper::SqlNotNull," NOT NULL ") ;
 Q_DEFINE_LATIN1_LITERAL(CntSymbianFilterSqlHelper::SqlWhere," WHERE ") ;
 Q_DEFINE_LATIN1_LITERAL(CntSymbianFilterSqlHelper::SqlOr," OR ") ;
 Q_DEFINE_LATIN1_LITERAL(CntSymbianFilterSqlHelper::contactsTable," contact ") ;
 Q_DEFINE_LATIN1_LITERAL(CntSymbianFilterSqlHelper::commAddrTable," comm_addr ") ;
 

/*!
 * The constructor
 */
CntSymbianFilterSqlHelper::CntSymbianFilterSqlHelper(CContactDatabase& contactDatabase)
                                               : m_contactDatabase(contactDatabase),
                                                 isPhoneNumberSearchforDetailFilter(false)
{
   m_srvConnection = new CntSymbianSrvConnection();
   m_sqlSearch = new CntSqlSearch();

   contactsTableIdColumNameMapping.insert(KUidContactFieldGivenName.iUid,"first_name" );
   contactsTableIdColumNameMapping.insert(KUidContactFieldGivenNamePronunciation.iUid,"firstname_prn" );
   contactsTableIdColumNameMapping.insert(KUidContactFieldFamilyName.iUid,"last_name" );
   contactsTableIdColumNameMapping.insert(KUidContactFieldFamilyNamePronunciation.iUid,"lastname_prn" );
   contactsTableIdColumNameMapping.insert(KUidContactFieldCompanyName.iUid,"company_name" );
   contactsTableIdColumNameMapping.insert(KUidContactFieldCompanyNamePronunciation.iUid,"companyname_prn" );

   //commAddrTableIdColumNameMapping.insert(KUidContactFieldIMPP.iUid,ESipAddress );
   commAddrTableIdColumNameMapping.insert(KUidContactFieldSIPID.iUid,ESipAddress );
   commAddrTableIdColumNameMapping.insert(KUidContactFieldEMail.iUid,EEmailAddress );

}

/*!
 * Destructor
 */
CntSymbianFilterSqlHelper::~CntSymbianFilterSqlHelper()

{
    delete m_srvConnection;
    delete m_sqlSearch;
    contactsTableIdColumNameMapping.clear();
    commAddrTableIdColumNameMapping.clear();
}

/*!
 * Fetch search results from the database.
 *
 * \a filter The simple/complex QContactFilter passed .
 * \a error On return, contains the possible error.
 * \return the list of matched contact ids
 */
QList<QContactLocalId> CntSymbianFilterSqlHelper::searchContacts(const QContactFilter& filter, const QList<QContactSortOrder>& sortOrders,
                                                                  QContactManager::Error& error)
{
    isPhoneNumberSearchforDetailFilter = false;
    QList<QContactLocalId> idList;
    bool isPredSearch;
    idList =  HandlePredictiveSearchFilter(filter,isPredSearch, error);
    if(isPredSearch)
            return idList;
   if(filterSupportLevel(filter)){
        
        // Create sql query from the filters
        QString sqlQuery;
        createSqlQuery(filter, sqlQuery, error);

        if( error != QContactManager::NoError) {
            return QList<QContactLocalId>();
        }
        // Query the database
        // If isPhoneNumberSearchforDetailFilter flag is set, we use the existing cntmodel
        // else call searchContacts
        if(isPhoneNumberSearchforDetailFilter)
        {
            // cast the filter into detail filte
            const QContactDetailFilter detailFilter(filter);
            idList = HandlePhonenumberDetailFilter(detailFilter);
        }
        else
        {
            //append the sort order to the query
            appendSortOrderQuery(sqlQuery, sortOrders);
            
            //fetch the contacts
            idList =  m_srvConnection->searchContacts(sqlQuery, error);
        }

    }
    else
        {
        error = QContactManager::NotSupportedError;
        }
    return idList;
    
        
}

QList<QContactLocalId>  CntSymbianFilterSqlHelper::HandlePredictiveSearchFilter(const QContactFilter& filter, bool &isPredSearch,
        QContactManager::Error& error)
    {
    isPredSearch = false;
    QString sqlQuery;
    if(filter.type() == QContactFilter::ContactDetailFilter){
       const QContactDetailFilter detailFilter(filter);
       if(  detailFilter.matchFlags() == QContactFilter::MatchKeypadCollation ){
          //convert string to numeric format
            QString pattern = detailFilter.value().toString();
            sqlQuery = m_sqlSearch->CreatePredictiveSearch(pattern);
            isPredSearch = true;
        
            return  m_srvConnection->searchContacts(sqlQuery, error);  
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

/*!
 * Append the sort order to the sql query
 *
 * \a sqlQuery to add the sort order to
 * \a sortOrders to be added
 */
void CntSymbianFilterSqlHelper::appendSortOrderQuery(QString& sqlQuery, const QList<QContactSortOrder>& sortOrders)
{
    QString column;
    CntDisplayLabel displayLabel;
    
    bool first(true);
    
    for(int i = 0; i < sortOrders.count(); i++)
    {
        columnName(column, sortOrders.at(i).detailDefinitionName(), sortOrders.at(i).detailFieldName());
      
        if(!column.isEmpty())
        {
            if(first)
            {
                sqlQuery += " ORDER BY";
                first = false;        
            }
          
            else
            {
                sqlQuery += ',';
            }
            
            //use the display label if the name is null, ignore case
            sqlQuery += " CASE WHEN " + column + " ISNULL THEN \'"+ displayLabel.unNamned().toLower() + "\' ELSE lower(" + column + ") END";
            
            if(sortOrders.at(i).direction() == Qt::AscendingOrder)
            {
                sqlQuery += " ASC";
            }
                
            else if(sortOrders.at(i).direction() == Qt::DescendingOrder)
            {
                sqlQuery += " DESC";
            }
        }
    }
}

/*!
 * Retrieve a column name
 *
 * \a columnName to be saved the column name if found
 * \a detailDefinitionName of the detail to fetch column name for
 * \a detailFieldName of the detail to fetch column name for
 */
void CntSymbianFilterSqlHelper::columnName( QString &columnName, const QString &detailDefinitionName, const QString & detailFieldName)
{
    columnName = "";

    //Name detail
    if(detailDefinitionName == QContactName::DefinitionName)
    {
        if(detailFieldName == QContactName::FieldFirst)
        {
            columnName = "first_name";
        }
        
        else if(detailFieldName == QContactName::FieldLast)
        {
            columnName = "last_name";
        }
    }
    
    //Organization
    else if(detailDefinitionName == QContactOrganization::DefinitionName)
    {
        if(detailFieldName == QContactOrganization::FieldName)
        {
            columnName = "company_name";
        }
    }
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
    bool IsOneLevelFilter  = isSingleFilter(filter);
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
 * \return True if the filters is single filter
 */

bool CntSymbianFilterSqlHelper::isSingleFilter(const QContactFilter& singlefilter) const
{

    bool returnValue = false;
    switch (singlefilter.type()) {
        case QContactFilter::ContactDetailFilter:
        case QContactFilter::InvalidFilter :
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
                        returnValue = false;
                        break;
        default:
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
    switch (filter.type()) {
           case QContactFilter::InvalidFilter :
           {
               // Not supported yet
               error = QContactManager::NotSupportedError;
               break;
           }
           case QContactFilter::ContactDetailFilter:
           {
               const QContactDetailFilter detailFilter(filter);
               
               //display label
               if (detailFilter.detailDefinitionName() == QContactDisplayLabel::DefinitionName)
               {
                   CntDisplayLabelSqlFilter displayLabelFilter;
                   displayLabelFilter.createSqlQuery(detailFilter,sqlQuery,error);
               }
               
               //type
               else if(detailFilter.detailDefinitionName() == QContactType::DefinitionName)
               {
                   if(detailFilter.value().toString() == QContactType::TypeContact)
                       sqlQuery = "SELECT contact_id FROM contact WHERE (type_flags>>24)=0";
                   else if(detailFilter.value().toString() == QContactType::TypeGroup)
                       sqlQuery = "SELECT contact_id FROM contact WHERE (type_flags>>24)=3";
               }
               
               //everything else
               else
               {
                   updateSqlQueryForDetailFilter(filter,sqlQuery,error);
               }
               
               break;
           }
           case QContactFilter::ContactDetailRangeFilter:
                       // Not supported yet
                       error = QContactManager::NotSupportedError;
                       break;

           case QContactFilter::ChangeLogFilter:
                       // Not supported yet
                       error = QContactManager::NotSupportedError;
                       break;
           case QContactFilter::DefaultFilter:
                       sqlQuery = "SELECT DISTINCT contact_id FROM contact WHERE (type_flags>>24)=0 OR (type_flags>>24)=3"; 
                       error = QContactManager::NoError;
                       break;
           case QContactFilter::ActionFilter:
           case QContactFilter::IntersectionFilter:
           case QContactFilter::UnionFilter:
                       //All these are multiple filters
                       // Not supported yet
                       error = QContactManager::NotSupportedError;
                       break;
           default:
                       //Some Unknow filter value
                       // Not supported
                       error = QContactManager::NotSupportedError;
                       break;
    };
    if( error != QContactManager::NoError)
        {
        sqlQuery = "";
        }
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
    const QContactDetailFilter detailFilter(filter);

    QString sqlWhereClause =  Space  + " WHERE ";

    //Get the table name and the column name
    bool isSubType;
    QString columnName;
    QString tableName;

    //Check for phonenumber. Special handling needed
    if(detailFilter.detailDefinitionName() == QContactPhoneNumber::DefinitionName){
        isPhoneNumberSearchforDetailFilter = true;
        error = QContactManager::NoError;
        return;
    }

    getSqlDbTableAndColumnNameforDetailFilter(detailFilter,isSubType,tableName,columnName);

    //return if tableName is empty
    if(tableName.isEmpty()){
        error = QContactManager::NotSupportedError;
        return;
    }

    //check columnName
    if(columnName.isEmpty()) {
        error = QContactManager::NotSupportedError;
        return;
    }
    else if(isSubType) {
        sqlWhereClause += columnName;
        sqlWhereClause += " NOT NULL ";
    }
    else {

        sqlWhereClause += Space + columnName + Space ;
        QString fieldToUpdate;
        //Update the value depending on the match flag
        updateFieldForDeatilFilterMatchFlag(detailFilter,fieldToUpdate,error);
        sqlWhereClause +=  fieldToUpdate;
    }


    //Create the sql query
    sqlQuery += "SELECT DISTINCT contact_id FROM " + Space + tableName + Space + sqlWhereClause;


}

/*!
 * Converts filed id to column name of the database table.
 * QContactManager::contacts function.
 *
 * \a fieldId field id representing the detail field name
 * \a sqlDbTableColumnName On return,contains the column name in the database
 */
void CntSymbianFilterSqlHelper::updateFieldForDeatilFilterMatchFlag(
                                                    const QContactDetailFilter& filter,
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
                fieldToUpdate = Space + EqualTo + SingleQuote
                               + filter.value().toString() + SingleQuote;
                error = QContactManager::NoError;
                break;
                }
            case QContactFilter::MatchContains:
                {
                // Pattern for MatchContains:
                // " LIKE '%xyz%'"
                fieldToUpdate = Space + SqlLike + Space + SingleQuote + PercentSign
                                + filter.value().toString() + PercentSign + SingleQuote ;
                error = QContactManager::NoError;
                break;
                }
            case QContactFilter::MatchStartsWith:
                {
                // Pattern for MatchStartsWith:
                // " LIKE 'xyz%'"
                fieldToUpdate = Space + SqlLike + Space + SingleQuote
                               +  filter.value().toString() + PercentSign + SingleQuote  ;
                error = QContactManager::NoError;
                break;
                }
            case QContactFilter::MatchEndsWith:
                {
                // Pattern for MatchEndsWith:
                // " LIKE '%xyz'"
                fieldToUpdate = Space + SqlLike + Space + SingleQuote + PercentSign
                              + filter.value().toString() + SingleQuote ;
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

/*!
 * Converts filed id to column name of the database table.
 * QContactManager::contacts function.
 *
 * \a fieldId field id representing the detail field name
 * \a sqlDbTableColumnName On return,contains the column name in the database
 */
void CntSymbianFilterSqlHelper::getSqlDbTableAndColumnNameforDetailFilter(
                                                    const QContactDetailFilter& detailFilter ,
                                                    bool& isSubType,
                                                    QString& tableName,
                                                    QString& columnName )
{

    //Get the field id for the detail field name
    CntTransformContact transformContact;
    quint32 fieldId  = transformContact.GetIdForDetailL(detailFilter, isSubType);

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
                columnName = Space + "TYPE = ";
                columnName.append('0'+ typeval)
                + typeval + Space;
                columnName += " and value " ;
                tableName = "comm_addr";
             }

    }
}

QList<QContactLocalId> CntSymbianFilterSqlHelper::HandlePhonenumberDetailFilter(const QContactDetailFilter detailFilter)
    {
    QList<QContactLocalId> matches;

    if(detailFilter.detailDefinitionName() == QContactPhoneNumber::DefinitionName){

    // Phone numbers need separate handling
            if ((filterSupportLevel(detailFilter) == CntAbstractContactFilter::Supported
                 || filterSupportLevel(detailFilter) == CntAbstractContactFilter::SupportedPreFilterOnly))
            {
                QString number((detailFilter.value()).toString());
                TPtrC commPtr(reinterpret_cast<const TUint16*>(number.utf16()));

                TInt matchLength(KDefaultMatchLength);
                // no need to propagate error, we can use the default match length
                TRAP_IGNORE(getMatchLengthL(matchLength));

                //cal the search
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
            }


    }
    return matches;

    }
/*!
 * The contact database version implementation for
 * QContactManager::filterSupported function. The possible return values are
 * Supported, NotSupported and SupportedPreFilterOnly. Supported means that
 * the filtering is implemented directly by the underlying database.
 * NotSupported means that CntSymbianFilterDbms::contacts will return an
 * error. And SupportedPreFilterOnly means that the filter is not supported,
 * but the CntSymbianFilterDbms::contacts will act like the filter was
 * supported. This means that the client must filter the pre-filtered set of
 * contacts to see if there are false positives included. Note that in some
 * cases the pre-filtering is not very effective.
 *
 * \a filter The QContactFilter to be checked.
 * \a return Supported in case the filter is supported. NotSupported in case
 * the filter is not supported. returns
 *
 */
CntAbstractContactFilter::FilterSupport CntSymbianFilterSqlHelper::filterSupportLevel(const QContactFilter& filter)
{
    CntAbstractContactFilter::FilterSupport filterSupported(CntAbstractContactFilter::NotSupported);
    switch (filter.type()) {
            case QContactFilter::DefaultFilter: //default filter == no filter
                {
                filterSupported = CntAbstractContactFilter::Supported;
                break;
                }
            case QContactFilter::ContactDetailFilter:
            	{
                const QContactDetailFilter &detailFilter = static_cast<const QContactDetailFilter &>(filter);
                filterSupported = checkIfDetailFilterSupported(detailFilter);
                break;
                }
            case QContactFilter::InvalidFilter :
            case QContactFilter::ContactDetailRangeFilter:
            case QContactFilter::ChangeLogFilter:
            case QContactFilter::ActionFilter:
            case QContactFilter::IntersectionFilter:
            case QContactFilter::UnionFilter:
            case QContactFilter::RelationshipFilter: 
            default:
                    filterSupported = CntAbstractContactFilter::NotSupported;
                    break;
        }
    return filterSupported;
}
CntAbstractContactFilter::FilterSupport CntSymbianFilterSqlHelper::checkIfDetailFilterSupported
                                                                    (const QContactDetailFilter& detailFilter) const
{

        CntAbstractContactFilter::FilterSupport filterSupported(CntAbstractContactFilter::NotSupported);
        //Get the match flags
        QContactFilter::MatchFlags matchFlags = detailFilter.matchFlags();
        // Phone numbers
        if (detailFilter.detailDefinitionName() == QContactPhoneNumber::DefinitionName) {
            if (matchFlags == QContactFilter::MatchEndsWith){
                filterSupported = CntAbstractContactFilter::Supported;
            }
            else if (matchFlags == QContactFilter::MatchExactly){
                filterSupported = CntAbstractContactFilter::SupportedPreFilterOnly;
            }
        }
        // Names , Email,Sip address
        else if ( detailFilter.detailDefinitionName() == QContactName::DefinitionName ||
                  detailFilter.detailDefinitionName() == QContactEmailAddress::DefinitionName ||
                  detailFilter.detailDefinitionName() == QContactOnlineAccount::DefinitionName ||
                  detailFilter.detailDefinitionName() == QContactDisplayLabel::DefinitionName ||
                  detailFilter.detailDefinitionName() == QContactType::DefinitionName){
               if (  (matchFlags == QContactFilter::MatchContains)|| (matchFlags == QContactFilter::MatchStartsWith)||
                     (matchFlags == QContactFilter::MatchEndsWith)|| (matchFlags == QContactFilter::MatchExactly)){
                filterSupported = CntAbstractContactFilter::Supported;
               }
               if(matchFlags == QContactFilter::MatchKeypadCollation)
                      filterSupported = CntAbstractContactFilter::Supported;
        }
        return filterSupported;

}
/*
 * Get the match length setting. Digits to be used in matching (counted from
 * right).
 */
void CntSymbianFilterSqlHelper::getMatchLengthL(TInt& matchLength)
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
TInt CntSymbianFilterSqlHelper::searchPhoneNumbers(
        CContactIdArray*& idArray,
        const TDesC& phoneNumber,
        const TInt matchLength)
{
    CContactIdArray* idArrayTmp(0);
    TRAPD( err, idArrayTmp = m_contactDatabase.MatchPhoneNumberL(phoneNumber, matchLength));
    if(err == KErrNone){
       idArray = idArrayTmp;
    }
    return 0;
}
