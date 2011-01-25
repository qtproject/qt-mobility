/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/


/**
 @file
 @internalComponent
 @released
*/

#include "cntsqldbstructure.h"
#include "dbsqlconstants.h"
#include <sqldb.h>

const TInt KSqlStatementSize = 1024;
// Destroy the RPointerArray
void DestroyRPointerArray(TAny* aPtr)
    {
    RPointerArray<CCntSqlDbTableColumn>* self = static_cast<RPointerArray<CCntSqlDbTableColumn>*> (aPtr);
    self->ResetAndDestroy();
    self->Close();
    }

/*
 * Factory method to get instance of CCntSqlDbTableColumn
 */
CCntSqlDbTableColumn* CCntSqlDbTableColumn::NewLC( const TDesC& aColumnName, 
			const TDesC& aColType, 
			TBool aNotNullValue /*= EFalse*/,
			const TDesC& aAdditionalInfo /*= KNullDesC()*/ )
	{
	CCntSqlDbTableColumn* self = new (ELeave) CCntSqlDbTableColumn( aNotNullValue );
	CleanupStack::PushL( self );
	self->ConstructL( aColumnName, aColType, aAdditionalInfo );	    
	return self;	
	}

void CCntSqlDbTableColumn::ConstructL( const TDesC& aColumnName, 
			const TDesC& aColType,
			const TDesC& aAdditionalInfo )
	{
	iColumnName = aColumnName.AllocL();
	iColumnType = aColType.AllocL();    
	iAdditionalInfo = aAdditionalInfo.AllocL();
	if ( iNotNullValue )
	    {
	    iNotNullStr = KNotNull().AllocL();
	    }
	else
	    {
	    iNotNullStr = KNullDesC().AllocL();
	    }
	}

/*
 * constructor
 */
CCntSqlDbTableColumn::CCntSqlDbTableColumn( TBool aNotNullValue )
	: iNotNullValue( aNotNullValue )
	{
	}

/*
 * returns the column name
 */
const TDesC& CCntSqlDbTableColumn::ColumnName() const
	{
	return *iColumnName;
	}
/*
 * Returns the column type
 */
const TDesC& CCntSqlDbTableColumn::ColumnType() const
	{
	return *iColumnType;
	}

/*
 * Returns the non null value 
 */
TBool CCntSqlDbTableColumn::NotNullValue() const 
    {
    return iNotNullValue;
    }

/*
 * Returns the non null value 
 */
const TDesC& CCntSqlDbTableColumn::NotNullString() const 
    {
    return *iNotNullStr;
    }

/*
 * Additional info such as default value, constrain etc
 */
const TDesC& CCntSqlDbTableColumn::AdditionalInfo() const
    {
    return *iAdditionalInfo;
    }
/*
 * Destructor
 */
CCntSqlDbTableColumn::~CCntSqlDbTableColumn()
	{
	delete iColumnName;
	delete iColumnType;
	delete iAdditionalInfo;
	delete iNotNullStr;
	}
       
/*
Factory method to create an instance of the class
*/

CCntSqlDbTable* CCntSqlDbTable::NewLC( const TDesC&  aTableName, RSqlDatabase& aDbInstance )
	{
	CCntSqlDbTable* self = new (ELeave) CCntSqlDbTable( aDbInstance );
	CleanupStack::PushL( self );
	self->ConstructL( aTableName );	
	return self;	
	}

/*
Private Constructor
*/

CCntSqlDbTable::CCntSqlDbTable( RSqlDatabase& aDbInstance )
: iDatabase( aDbInstance )
	{	
	}

/*
Returns the table name
*/

const TDesC& CCntSqlDbTable::TableName() const
	{
	return *iTableName;
	}

/*
Reads the database and gets the existing columns information for this table
*/

void CCntSqlDbTable::GetExistingColumnsFromDatabaseL(
		RPointerArray<CCntSqlDbTableColumn>& aExistingColumnsInDb )
    {    
    //Get the table information using the PRAGMA Statement
    RSqlStatement columnInfoStatement;
    CleanupClosePushL( columnInfoStatement );    
   
    HBufC* sqlstmt = HBufC::NewLC( KPragmaStmnt().Length() + 
                            iTableName->Length() );
    sqlstmt->Des().Format( KPragmaStmnt, &*iTableName );    
        
    //Get the name and type
    User::LeaveIfError( columnInfoStatement.Prepare( iDatabase, *sqlstmt ) );
    TInt KNameIndex = columnInfoStatement.ColumnIndex( KPragmaName );
    TInt KTypeIndex = columnInfoStatement.ColumnIndex( KPragmaType );
    
    TInt err = KErrNotFound;
    
    //Go through each one and add the column details
    while( ( err = columnInfoStatement.Next() ) == KSqlAtRow )
        {
        TPtrC nameCol = ( columnInfoStatement.ColumnTextL( KNameIndex ) );
        TPtrC typeCol = ( columnInfoStatement.ColumnTextL( KTypeIndex ) );
        CCntSqlDbTableColumn* columnInOldTable = 
				CCntSqlDbTableColumn::NewLC( nameCol, typeCol );
        aExistingColumnsInDb.AppendL(columnInOldTable);
        CleanupStack::Pop(); //columnInOldTable
        }

    if ( KSqlAtEnd != err )
        {
        User::Leave( err );
        }
    
	//clean up
    CleanupStack::PopAndDestroy() ; //sqlstmt
    CleanupStack::PopAndDestroy( &columnInfoStatement );            
    }

/* 
 * Gets the columns that are missing from the database file for this table
 * */
void CCntSqlDbTable::GetMissingColumns( RPointerArray<CCntSqlDbTableColumn>& aColumnsToAdd )
    {
    RPointerArray<CCntSqlDbTableColumn> existingColumnsFromDb;
    CleanupStack::PushL( TCleanupItem( DestroyRPointerArray, &existingColumnsFromDb ) );
    GetExistingColumnsFromDatabaseL( existingColumnsFromDb );
    
    TInt existingColumnCount = existingColumnsFromDb.Count();
    if( existingColumnCount )
        {
        //Check if any new columns exist
        for( TInt i = 0; i < iColumnInfo.Count(); i++ )
            {
            CCntSqlDbTableColumn* col = iColumnInfo[i];
            TInt j = 0;
            for( ; j < existingColumnCount; j++ )
                {
                
                if( col->ColumnName().CompareC( existingColumnsFromDb[j]->ColumnName() ) == 0 )
                    {                    
                    break;                    
                    }
                }
            if( existingColumnCount == j )
                {
                aColumnsToAdd.Append( iColumnInfo[i] );
                }
                
            }
        }
    CleanupStack::PopAndDestroy( &existingColumnsFromDb ); //existingColumnsFromDb
    }
/*
 * Checks the table structure and adds new columns if needed for this table
 * 
 */
void CCntSqlDbTable::MakeTableStructureCompatibleL()

    {
    RPointerArray<CCntSqlDbTableColumn> columnsToAdd;
    //Contents are not owned by ColumnsToAdd. So its enough if we call close at the end
    CleanupClosePushL( columnsToAdd );
    
    GetMissingColumns(columnsToAdd);
    
	// Create the sql string for altering the table
    // We cannot add all columns in one statement, so need to process 
    // one by one    
    TInt cnt = columnsToAdd.Count();
    
    for( TInt i = 0; i< cnt ; i++ )
        {
        AlterTableToAddColumnL(columnsToAdd[i]);
        }    

    CleanupStack::PopAndDestroy();  //ColumnsToAdd.Close()   
    }
/*
 * Alters this  table and adds the column specified by aColInfo.
 */
void CCntSqlDbTable::AlterTableToAddColumnL( CCntSqlDbTableColumn* aColInfo)
    {    
    HBufC* sqlstmt = HBufC::NewLC( KSqlStatementSize );
    if(aColInfo)
        {        
        sqlstmt->Des().Format(  KContactTableAddColumn, 
								&*iTableName,
								&( aColInfo->ColumnName() ),
								&( aColInfo->ColumnType() ),
								&( aColInfo->NotNullString() ),
								&( aColInfo->AdditionalInfo() )
								);    
        
        RSqlStatement addColStmt;
        CleanupClosePushL(addColStmt);
        addColStmt.Prepare( iDatabase, *sqlstmt );
        TInt err = addColStmt.Exec();
        CleanupStack::PopAndDestroy( &addColStmt );
        }
    CleanupStack::PopAndDestroy(); //sqlstmt
    }
/*
 * Adds a column to the table info.
 */
void CCntSqlDbTable::AddColumnToColumnInfoL( const TDesC& aColName, 
					const TDesC& aColType, 
					TBool aNotNullValue /*= EFalse*/, 
					const TDesC& aAdditionalInfo /*= KNullDesC()*/ )
	{
	
	CCntSqlDbTableColumn* col = CCntSqlDbTableColumn::NewLC( aColName, aColType,
					aNotNullValue, aAdditionalInfo );
	iColumnInfo.AppendL(col);
	CleanupStack::Pop(); //col
	}

/*
 * Destructor
 */
CCntSqlDbTable::~CCntSqlDbTable()
	{
	delete iTableName;
	iColumnInfo.ResetAndDestroy();
	iColumnInfo.Close();
	}

void CCntSqlDbTable::ConstructL(const TDesC& aTableName)
	{
	iTableName = aTableName.Alloc();
	}


/*
 * Factory method to return instance of CCntSqlDbStructure. 
 */
CCntSqlDbStructure* CCntSqlDbStructure::NewL( RSqlDatabase& aDbInstance)
    {   
    return new ( ELeave ) CCntSqlDbStructure( aDbInstance );
    }
/*
 * Initialises the table with table and columns
 */
void CCntSqlDbStructure::InitializeL()
	{
	// Contacts table
	InitializeContactsTableInfoL();
	
	//Groups table
	InitializeGroupsTableInfoL();
	
	// Comm addr table
	InitializeCommAddrTableInfoL();
	}


/*
 * Initialises the contacts table column information.
 */
void CCntSqlDbStructure::InitializeContactsTableInfoL()
	{
	CCntSqlDbTable * contactTable = CCntSqlDbTable::NewLC( KSqlContactTableName(), iDatabase );
	
	contactTable->AddColumnToColumnInfoL(KContactId(), KInteger(),ETrue, KAutoIncrement);
	contactTable->AddColumnToColumnInfoL(KContactTemplateId(), KInteger(),EFalse, KNullDesC());
	contactTable->AddColumnToColumnInfoL(KContactTypeFlags(), KInteger(),EFalse, KNullDesC());
	contactTable->AddColumnToColumnInfoL(KContactAccessCount(), KInteger(),EFalse, KNullDesC());
	contactTable->AddColumnToColumnInfoL(KContactCreationDate(), KInteger(),EFalse, KNullDesC());
	contactTable->AddColumnToColumnInfoL(KContactLastModified(), KInteger(),EFalse, KNullDesC());
	contactTable->AddColumnToColumnInfoL(KContactGuidString(), KVarchar255(),EFalse, KNullDesC());
	contactTable->AddColumnToColumnInfoL(KContactFirstName(), KVarchar255(),ETrue, KDefault());
	contactTable->AddColumnToColumnInfoL(KContactLastName(), KVarchar255(),ETrue, KDefault());
	contactTable->AddColumnToColumnInfoL(KContactCompanyName(), KVarchar255(),ETrue, KDefault());
	
	contactTable->AddColumnToColumnInfoL(KContactFirstNamePrn(), KVarchar255(),EFalse, KNullDesC());
	contactTable->AddColumnToColumnInfoL(KContactLastNamePrn(), KVarchar255(),EFalse, KNullDesC());
	contactTable->AddColumnToColumnInfoL(KContactCompanyNamePrn(), KVarchar255(),EFalse, KNullDesC());
	contactTable->AddColumnToColumnInfoL(KContactTextFieldHeader(), KBlob(),EFalse, KNullDesC());
	contactTable->AddColumnToColumnInfoL(KContactBinaryFieldHeader(), KBlob(),EFalse, KNullDesC());
	
	contactTable->AddColumnToColumnInfoL(KContactTextFields(), KText(),EFalse, KNullDesC());
	contactTable->AddColumnToColumnInfoL(KContactBinaryFields(), KBlob(),EFalse, KNullDesC());
    contactTable->AddColumnToColumnInfoL(KContactFavoriteIndex(), KInteger(),EFalse, KNullDesC());

	iTables.AppendL( contactTable );
	CleanupStack::Pop(); //contactTable
	}

/*
 * Initialises the Groups table column information.
 */
void CCntSqlDbStructure::InitializeGroupsTableInfoL()
	{
	CCntSqlDbTable * groupTable = CCntSqlDbTable::NewLC( KSqlContactGroupTableName(), iDatabase );

	groupTable->AddColumnToColumnInfoL(KGroupId(), KInteger(),ETrue, KAutoIncrement());
	groupTable->AddColumnToColumnInfoL(KGroupContactGroupId(), KInteger(),ETrue, KNullDesC());
	groupTable->AddColumnToColumnInfoL(KGroupContactGroupMemberId(), KInteger(),ETrue, KNullDesC());
	
	iTables.AppendL( groupTable );
	CleanupStack::Pop(); //groupTable	  
	}

/*
 * Initialises the comm_addr table column information.
 */
void CCntSqlDbStructure::InitializeCommAddrTableInfoL()
	{
	CCntSqlDbTable * commAddrTable = CCntSqlDbTable::NewLC( KSqlContactCommAddrTableName(), iDatabase );
	
    commAddrTable->AddColumnToColumnInfoL(KCommAddrId(), KInteger(),ETrue, KAutoIncrement());
    commAddrTable->AddColumnToColumnInfoL(KCommAddrType(), KInteger(),ETrue, KNullDesC());
    commAddrTable->AddColumnToColumnInfoL(KCommAddrValue(), KChar255(),ETrue, KNullDesC());
    commAddrTable->AddColumnToColumnInfoL(KCommAddrExtraValue(), KChar255(),EFalse, KNullDesC());
    commAddrTable->AddColumnToColumnInfoL(KCommAddrContactId(), KInteger(),ETrue, KNullDesC());
    commAddrTable->AddColumnToColumnInfoL(KCommAddrExtraTypeInfo(), KInteger(),EFalse, KNullDesC());
	
	iTables.AppendL( commAddrTable );
	CleanupStack::Pop(); //commAddrTable
	}

/*
 * Destructor
 */
CCntSqlDbStructure::~CCntSqlDbStructure()
	{
	iTables.ResetAndDestroy();
	iTables.Close();
	}
	
/*
 * Makes the database compatible with respect to the structure defined
 * For each of the tables, it calls the corresponding function to make 
 * it compatible
 */
void CCntSqlDbStructure::MakeDatabaseCompatibleL()
	{
	// Initialise the table and column info
	InitializeL();
	TInt count = iTables.Count();
	for( TInt i = 0; i < count; i++ )
	    {
	    iTables[i]->MakeTableStructureCompatibleL();	    
	    }
	}

/*
 * Returns the list of tables
 */
const RPointerArray<CCntSqlDbTable>& CCntSqlDbStructure::Tables()
	{
	return iTables;	
	}

/*
 * Constructor
 */
CCntSqlDbStructure::CCntSqlDbStructure( RSqlDatabase& aDbInstance)
	: iDatabase( aDbInstance )
	{
	}
