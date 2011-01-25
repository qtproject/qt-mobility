/*
* Copyright (c) 2011 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: An  object representing a sql database table information. 
* All the tables and their column information is stored in the source code. 
* While checking compatibility, this information is compared with the tables and columns in the database.
* This class is used for checking if the database contains all the columns. If not, then
* it may be needed to add those columns (To maintain compatibility across releases)
* 
*/



/**
@internalComponent
@released
*/

#ifndef __CNTSQLDBSTRUCTURE_H__
#define __CNTSQLDBSTRUCTURE_H__

#include <e32base.h>
#include <sqldb.h>
 
/*
* Class provide information about columns in a table
*/
class CCntSqlDbTableColumn:public CBase
    {
    public:
    
    /*
    * Creates a new CCntSqlDbTableColumn obj
    * @param aColumnName - Mandatory
    * @param aColType - Mandatory
    * @param aNotNullValue 
    * @param aAdditionalInfo
    * @return - obj of CCntSqlDbTableColumn
    */
    static CCntSqlDbTableColumn* NewLC(
            const TDesC& aColumnName,
            const TDesC& aColType,
            TBool aNotNullValue = EFalse,
            const TDesC& aAdditionalInfo = KNullDesC() );
    
    /*
    * Returns the column Name
    */
    const TDesC& ColumnName() const;
    
    /*
    * Returns the column type i.e INTEGER, VARCHAR etc
    */
    const TDesC& ColumnType() const;
    
    /*
    * Returns true if NOTNULL constrain set
    */
    TBool NotNullValue() const ;
    
    /*
    * Any addition information such as default value etc
    */
    const TDesC& AdditionalInfo() const;
    
    /*
    * Returns NOTNULL string if NOTNULL constrain set
    * else returns KNullDesC()
    */
    const TDesC& NotNullString() const;
    
    /*
    * Destructor
    */
    ~CCntSqlDbTableColumn();
    
private:
    
    CCntSqlDbTableColumn(TBool aNotNullValue);
    void ConstructL(const TDesC& aColumnName, const TDesC& aColType,const TDesC& aAdditionalInfo);
    
private:
    
    HBufC* iColumnName; //own, column name
    HBufC* iColumnType; //own, column type
    TBool iNotNullValue; //own , notnull value
    HBufC* iAdditionalInfo; //own, additional information
    HBufC* iNotNullStr; //Own, Not NULL String
    };


/*
* Class provide information about the table
*/
NONSHARABLE_CLASS(CCntSqlDbTable) : public CBase
    {
public:
     static CCntSqlDbTable* NewLC (const TDesC& aTableName,  RSqlDatabase& aDbInstance);
public:
    /*
    * Returns the table name
    */
    const TDesC& TableName() const;
    /*
    * Checks the database table and verifies if all columns are present. If not, 
    * then the missing columns are added in the database table
    */
    void MakeTableStructureCompatibleL();
    /*
    * Add a column to this table
    * @param aColumnName - Mandatory
    * @param aColType - Mandatory
    * @param aNotNullValue 
    * @param aAdditionalInfo    
     */
    void AddColumnToColumnInfoL( const TDesC& aColumnName,
            const TDesC& aColType,
            TBool aNotNullValue = EFalse,
            const TDesC& aAdditionalInfo = KNullDesC() );

    /*
    * destructor
    */
    ~CCntSqlDbTable();
    
protected:
    void ConstructL(const TDesC& aTableName);

private:
    CCntSqlDbTable(RSqlDatabase& aDbInstance);
    void GetMissingColumns(RPointerArray<CCntSqlDbTableColumn>& aColumnsToAdd);
    void GetExistingColumnsFromDatabaseL(RPointerArray<CCntSqlDbTableColumn>& aExistingColumnsFromOldDb);
    void AlterTableToAddColumnL( CCntSqlDbTableColumn* aColInfo);

protected:
    HBufC* iTableName; // holds the table name, own
    RPointerArray<CCntSqlDbTableColumn> iColumnInfo;  // own,List of columns this table should have
    RSqlDatabase& iDatabase; // not own
    
    };

/*
 * This class holds the actual table structure that should be. If the databsse does not have 
 * this structure then it needs to be modified to make it compatible 
 */
NONSHARABLE_CLASS(CCntSqlDbStructure) : public CBase
    {
public:
    static CCntSqlDbStructure* NewL( RSqlDatabase& aDbInstance);
    ~CCntSqlDbStructure();    
    /*
    *  Checks the database and verifies if all tables and columns are present. If not,
    *  then the missing columns are added in the database 
    */
    void MakeDatabaseCompatibleL();
    
    /*
    * Returns the List of Tables present in this DB 
    */
    const RPointerArray<CCntSqlDbTable>& Tables();

private:
    CCntSqlDbStructure( RSqlDatabase& aDbInstance);
    void InitializeL();
    void InitializeContactsTableInfoL();
    void InitializeGroupsTableInfoL();
    void InitializeCommAddrTableInfoL();
    
private:
    RPointerArray<CCntSqlDbTable> iTables;  //own, list of tables
    RSqlDatabase& iDatabase; // not own, handle to the database
    
    };
#endif //__CNTSQLDBSTRUCTURE_H__
