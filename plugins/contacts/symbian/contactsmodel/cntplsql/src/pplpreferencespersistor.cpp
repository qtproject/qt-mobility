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

#include "pltables.h"
#include "dbsqlconstants.h"
#include <cntdb.h>

_LIT(KFirstSQLiteSchemaVersion, "1.0");
const TInt KArrayGrowthSize = 3;
			
/**		
Construct a concrete CPplPreferencesPersistor object

@param aDatabase Reference to RSqlDatabase.
*/		
CPplPreferencesPersistor* CPplPreferencesPersistor::NewL(RSqlDatabase& aDatabase)
	{
	CPplPreferencesPersistor* self = new (ELeave) CPplPreferencesPersistor(aDatabase);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;	
	}
	
/**		
Utility method used to write a to the preferences table the first time
*/		
void CPplPreferencesPersistor::FirstWriteToTableL()        
	{		
	// insert preferences record
	// 	INSERT INTO preferences 
	//		(preference_id , data_schema_version, machine_id, creation_date, preferred_sortorder, 
	//        preferred template id) 
	//		VALUES (:NULL, VersionNum, :[created machine id], 
	//			:[time now], :NULL, :[-1] );
	
	TCntSqlStatementType cntStatement(EInsert, KSqlContactPrefTableName);
	CCntSqlStatement* insertStatement = TSqlProvider::GetSqlStatementL(cntStatement);
	CleanupStack::PushL(insertStatement);

	// Set Id Arg
	insertStatement->SetParamL(KPrefPrefId, KPrefIdParam);

	// Set Schema Version Number  	
	insertStatement->SetParamL(KPrefSchemaVersion, KFirstSQLiteSchemaVersion());
	
	// Set Machine Id
    insertStatement->SetParamL(KPrefMachineId, KPrefMachineIdParam);
		
	// Set Creation Date Param
	insertStatement->SetParamL(KPrefPrefCreationDate, KPrefCreationDateParam);
	
    // No sort order defined at this point
    insertStatement->SetParamL(KPrefPreferredOrder, KPrefPreferredOrderParam);
    
    // Write -1 for the pref template
    insertStatement->SetParamL(KPrefferredTemplateId, KPrefferredTemplateIdParam);
	
	RSqlStatement insertStmt;
	CleanupClosePushL(insertStmt);
	User::LeaveIfError(insertStmt.Prepare(iDatabase, insertStatement->SqlStringL()));
			
	// NULL id
	User::LeaveIfError(insertStmt.BindNull(KFirstParam));	
		
    // Machine id
    GenerateMachineUniqueID();
	User::LeaveIfError(insertStmt.BindInt64(KFirstParam + 1, iMachineId ));
		
	// Creation Date
	TTime timeNow;
	timeNow.UniversalTime();
	User::LeaveIfError(insertStmt.BindInt64(KFirstParam + 2, timeNow.Int64()));	
		
	// 0 sort order		
	RSqlParamWriteStream sortOrderStream;
	CleanupClosePushL(sortOrderStream);
	sortOrderStream.BindBinaryL(insertStmt, KFirstParam + 3);
		
	sortOrderStream.WriteInt32L(0);

	CleanupStack::PopAndDestroy(&sortOrderStream);

	// Preffered Template
	User::LeaveIfError(insertStmt.BindInt(KFirstParam + 4, KErrNotFound));
		
	User::LeaveIfError(insertStmt.Exec());	
	CleanupStack::PopAndDestroy(2, insertStatement); 	
				
	iCreationDate = timeNow;  // in case the Exec leaves, don't update this state until here
	}

/**
Utility method used to create a preference table in the contact database & write default content
*/
void CPplPreferencesPersistor::CreateTableL()
	{
	User::LeaveIfError(iDatabase.Exec(KPreferencesCreateStmnt));	
	FirstWriteToTableL();	
	}
	
/**
CPplPreferencesPersistor destructor
*/
CPplPreferencesPersistor::~CPplPreferencesPersistor()
	{
    delete iSortOrderPrefs;
    delete iUpdateStmnt;
    
	}
	
/**
CPplPreferencesPersistor constructor

@param aDatabase reference to contact database
*/	
CPplPreferencesPersistor::CPplPreferencesPersistor(RSqlDatabase& aDatabase):
	iDatabase(aDatabase),
	iCreationDate(Time::NullTTime())
	{	
	}	
	
/**
Second phase constructor for CPplPreferencesPersistor object
*/	
void CPplPreferencesPersistor::ConstructL()
	{  
	TCntSqlStatementType updateType(EUpdate, KSqlContactPrefTableName() );
	iUpdateStmnt = TSqlProvider::GetSqlStatementL(updateType);
	iSortOrderPrefs = new (ELeave) CArrayFixFlat<CContactDatabase::TSortPref>(KArrayGrowthSize);
	}
		
/**
Utility method used to persiste preferences in preference table
*/	
void CPplPreferencesPersistor::PersistStateL(const TDesC& aParam, TInt aValue)
	{
	TBuf<64> temp;  
	temp.Format(KIntValue, aValue);
	
	iUpdateStmnt->Reset();
	iUpdateStmnt->SetParamL(aParam, temp);

    HBufC* wherePrefIdBuf = HBufC::NewLC(KPrefPrefId().Size() + KConditionClause().Size() + 1);
    	
	// for WHERE pref_id = 1
	wherePrefIdBuf->Des().AppendFormat(KConditionClause, &KPrefPrefId, 1);	
	
	iUpdateStmnt->SetConditionL(*wherePrefIdBuf);	
	CleanupStack::PopAndDestroy(wherePrefIdBuf);
		
	RSqlStatement updateStmt;
	CleanupClosePushL(updateStmt);
	User::LeaveIfError(updateStmt.Prepare(iDatabase, iUpdateStmnt->SqlStringL()));

	User::LeaveIfError(updateStmt.Exec() );
	CleanupStack::PopAndDestroy(&updateStmt);
	}
	
/**
Check if the Creation Date has been set.  If it has, there is nothing to do (as file is newly created and
state set OR state has been read in already).  Otherwise read in the state.
*/	
void CPplPreferencesPersistor::ReadInStateL()
	{
	if (iCreationDate == Time::NullTTime())  // if this is not set, the state has never been read - read it all in
		{
		TCntSqlStatementType selectType(ESelect, KSqlContactPrefTableName);
		CCntSqlStatement* sqlSelectStmnt = TSqlProvider::GetSqlStatementL(selectType);  
		CleanupStack::PushL(sqlSelectStmnt);		
				
		sqlSelectStmnt->SetParamL(KPrefPrefCreationDate(), KNullDesC() );		
		sqlSelectStmnt->SetParamL(KPrefMachineId(), KNullDesC() );	
		sqlSelectStmnt->SetParamL(KPrefPreferredOrder(), KNullDesC() );		
		sqlSelectStmnt->SetParamL(KPrefferredTemplateId(), KNullDesC() );	
	
		RSqlStatement selectStmt;
		CleanupClosePushL(selectStmt);
		User::LeaveIfError(selectStmt.Prepare(iDatabase, sqlSelectStmnt->SqlStringL()));
	
		TInt err;
		if((err = selectStmt.Next()) == KSqlAtRow)
			{
			
			iCreationDate = selectStmt.ColumnInt64(sqlSelectStmnt->ParameterIndex(KPrefPrefCreationDate));
			iMachineId = selectStmt.ColumnInt64(sqlSelectStmnt->ParameterIndex(KPrefMachineId));
			iPreferCardTemplateId = selectStmt.ColumnInt(sqlSelectStmnt->ParameterIndex(KPrefferredTemplateId));
		
		    // Read In the Pref Sort Order
			TPtrC8 sortOrderDescriptor;
			selectStmt.ColumnBinary(User::LeaveIfError(selectStmt.ColumnIndex(KPrefPreferredOrder)), sortOrderDescriptor);
			RDesReadStream sortOrderStream(sortOrderDescriptor);
			CleanupClosePushL(sortOrderStream);	

			// read in number of prefs
			CArrayFix<CContactDatabase::TSortPref>* sortOrder = new (ELeave) CArrayFixFlat<CContactDatabase::TSortPref>(KArrayGrowthSize);
		    CleanupStack::PushL(sortOrder);
			const TInt KNumSortPrefs = sortOrderStream.ReadInt32L();
			for (TInt i=0 ; i < KNumSortPrefs ; ++i)
				{
				CContactDatabase::TSortPref thisPref;
				sortOrderStream >> thisPref;
				sortOrder->AppendL(thisPref);
				}
			CleanupStack::Pop(sortOrder);	
		    
		    CleanupStack::PopAndDestroy(3); 
			delete iSortOrderPrefs;
			iSortOrderPrefs = sortOrder;
			
			}
		else
			{
			if(err == KSqlAtEnd)
				{
				User::Leave(KErrNotFound);		
				}
			else
				{
				User::Leave(err);	
				}	
			}
		}
	}
	
/**
Getter method used to retrieve database schema version

@return database schema version
*/	
TDesC CPplPreferencesPersistor::DataSchemaVersion() const
	{
	return KFirstSQLiteSchemaVersion();
	}
/**
Getter method used to retrieve database creation time

@return database creation time
*/	
TTime CPplPreferencesPersistor::CreationDateL() 
	{
	ReadInStateL();
	return iCreationDate;	
	}

/**
Getter method used to retrieve prefered card template id

@return prefered card template id
*/	
TContactItemId CPplPreferencesPersistor::PreferredCardTemplateIdL() 
	{
	ReadInStateL();
	return iPreferCardTemplateId;
	}

/**
Getter method used to retrieve preferred sort order

@return preferred sort order
*/	
const CArrayFix<CContactDatabase::TSortPref>& CPplPreferencesPersistor::PreferredSortOrderL() 
	{
	ReadInStateL();
	return *iSortOrderPrefs;
	}

/**
Getter method used to retrieve database UID

@return database UID
*/	
TPtrC CPplPreferencesPersistor::DatabaseUidL() 
	{
	ReadInStateL(); 
	TInt64 part2 	 = iCreationDate.Int64(); 
	TInt64 machineId = iMachineId;
	
	iUidString.Format(_L("%08X%08X-08X%08X"), I64HIGH(machineId), I64LOW(machineId), I64HIGH(part2), I64LOW(part2));
	
	return iUidString; 

	}

/**
Getter method used to retrieve machine ID

@return machine ID
*/	
TInt64 CPplPreferencesPersistor::MachineIdL()  
	{
	ReadInStateL();	
	return iMachineId;
	}	
	
/**
Setter method used to set machine id

@param aDbUid new machine id
*/
void CPplPreferencesPersistor::SetMachineIdL(TInt64 aDbUid)
	{
	PersistStateL(KPrefMachineId, aDbUid);
	iMachineId = aDbUid;  
	}

	
/**
Setter method used to set preferred sort order

@param aSortPrefs new preffered sort order
*/
void CPplPreferencesPersistor::SetPreferredSortOrderL(CArrayFix<CContactDatabase::TSortPref>* aSortPrefs)
	{
	iUpdateStmnt->Reset();
	iUpdateStmnt->SetParamL(KPrefPreferredOrder, KPrefPreferredOrderParam);
	
    HBufC* wherePrefIdBuf = HBufC::NewLC(KPrefPrefId().Size() + KConditionClause().Size() + 1);
    	
	// for WHERE pref_id = 1
	wherePrefIdBuf->Des().AppendFormat(KConditionClause, 
		&KPrefPrefId, 1);	
	
	iUpdateStmnt->SetConditionL(*wherePrefIdBuf);	
	CleanupStack::PopAndDestroy(wherePrefIdBuf);
		
	RSqlStatement updateStmt;
	CleanupClosePushL(updateStmt);
	User::LeaveIfError(updateStmt.Prepare(iDatabase, iUpdateStmnt->SqlStringL()));
	
	// 0 sort order		
	RSqlParamWriteStream sortOrderStream;
	CleanupClosePushL(sortOrderStream);
	sortOrderStream.BindBinaryL(updateStmt, KFirstParam);
	
	
	const TInt KNumSortPrefs = aSortPrefs->Count();
	sortOrderStream.WriteInt32L(KNumSortPrefs);
	
	for (TInt i=0; i < KNumSortPrefs; ++i)
		{
		(*aSortPrefs)[i].ExternalizeL(sortOrderStream);
		}
	sortOrderStream.CommitL();
	CleanupStack::PopAndDestroy(&sortOrderStream);
	
	User::LeaveIfError(updateStmt.Exec() );
	CleanupStack::PopAndDestroy(&updateStmt);

    delete iSortOrderPrefs;
	iSortOrderPrefs = aSortPrefs;	
	}
	
void CPplPreferencesPersistor::SetPreferredCardTemplateIdL(TContactItemId aTemplateId)
	{
	PersistStateL(KPrefferredTemplateId, aTemplateId);
	iPreferCardTemplateId = aTemplateId;
	}
	
	
/**
Generate a 64-bit "unique id" that is likely to be different each time and is
reasonably well distributed.
*/
void CPplPreferencesPersistor::GenerateMachineUniqueID()
	{
	// 2 different 32-bit inputs from environment: a counter and a memory
	// address.
 	TUint32 temp1 = User::FastCounter();
	// Add 1 because the address will be 32 bit aligned.
 	TUint32 temp2 = reinterpret_cast<TUint32>(this) + 1;

	// Get 32 fairly random bits.
	TUint32 part1 = temp1 * temp2;

	// Jumble things around to get different 32 bits.
	TUint32 part2 = (part1*0x4321) + (0x65*temp1) + (0x87*temp2) + 0xFEDCBA98;

	// Combine part1 and part2 to make 64 random bits.
	TInt64 uid = MAKE_TINT64(part1,part2);

	// Get calendar time to randomise further.
	TTime time;
	time.UniversalTime();

	iMachineId = uid + time.Int64();
	}

/**
Utility method used to generate a guid for a contact item
*/
void CPplPreferencesPersistor::SetGuidL(CContactItem& aContact, TBool& aCompressed)
	{
	const TInt64 KMachineUniqueId = MachineIdL();
	TPtrC guid=aContact.UidStringL(KMachineUniqueId);
	HBufC* guidBuf=NULL;
	aCompressed=EFalse;

	if (guid.Length()==0)
		{
		guidBuf=HBufC::NewLC(20);
		TPtr ptr=guidBuf->Des();
		TTime time;
		time.UniversalTime();
		TInt64 num=time.Int64();
		ptr.AppendFormat(_L("%08x"),(TInt32)I64HIGH(num));
		ptr.AppendFormat(_L("%08x"),(TInt32)I64LOW(num));
		aCompressed=ETrue;
		}
	else
		{
		guidBuf=guid.AllocLC();
		TPtr ptr=guidBuf->Des();
		aCompressed=ContactGuid::GetCreationDate(ptr,KMachineUniqueId);
		}
	
	aContact.SetUidStringL(*guidBuf);
	CleanupStack::PopAndDestroy(guidBuf);
	}


