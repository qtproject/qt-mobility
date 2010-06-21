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

#include "pplcontactitemmanager.h"
#include "cntsqlprovider.h"
#include "dbsqlconstants.h"
#include "cntpersistenceutility.h"
#include "c12keypredictivesearchtable.h"
#include "cqwertypredictivesearchtable.h"
#include "cpredictivesearchsettingstable.h"
#include "cpredictivesearchsynchronizer.h"
#include "predictivesearchlog.h"
//#include "cntmetadataoperation.h"
#include <cntdef.h>
#include <sqldb.h>
#include <cntdb.h>

// TODO: The code related to predictive search table is placed inside this
// macro. Uncomment the below line to get the table into use.
#define USE_PRED_SEARCH_TABLE

// If this macro is defined, then pred.search qwerty tables are used
#define USE_QWERTY_PRED_SEARCH_TABLE


/**
Creates a concrete CPplContactItemManager object 

@param 	aDatabase reference to RSqlDatabase
		aTransactionManager reference to MLplTransactionManager (provided by CPlContactsFile)
@return concrete CPplContactItemManager object
@leave KErrNoMemory, an out of memory occured
*/		
CPplContactItemManager* CPplContactItemManager::NewL(RSqlDatabase& aDatabase, MLplTransactionManager& aTransactionManager, CLplContactProperties&  aContactProperties, RPplIccContactStore& aIccContactStore)
	{
	CPplContactItemManager* self = new (ELeave) CPplContactItemManager(aDatabase, aTransactionManager, aContactProperties, aIccContactStore);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;	
	}

/**
Class destructor. Deletes holded CCntSqlStatement

*/		
CPplContactItemManager::~CPplContactItemManager()
	{
	delete iSelectStatement;	
	delete iContactTable;
	delete iCommAddrTable;
#if defined(USE_PRED_SEARCH_TABLE)
	PRINT(_L("delete pred search table objects"));
	delete iPredictiveSearchSynchronizer;
	delete iPredSearch12keyTable;
#if defined(USE_QWERTY_PRED_SEARCH_TABLE)
	delete iPredSearchQwertyTable;
	delete iPredSearchSettingsTable;
#endif
	PRINT(_L("delete pred search table objects - done"));
#endif
    delete iPresenceTable; 
	delete iGroupTable;
	delete iPreferencePersistor;
	//iColSession.Close();
	}
	
/**
Class constructor

@param aDatabase reference to RSqlDatabase
@param aTransactionManager reference to transaction manager
@param aContactProperties reference to contact properties
*/	
CPplContactItemManager::CPplContactItemManager(RSqlDatabase& aDatabase, MLplTransactionManager& aTransactionManager, CLplContactProperties&  aContactProperties, RPplIccContactStore& aIccContactStore) :
	iDatabase(aDatabase),
	iTransactionManager(aTransactionManager),
	iContactProperties(aContactProperties),
	iIccContactStore(aIccContactStore)
	{
	}
	
/**
Add the given contact item to the database. Forward the call to CPplTableBase
based classes representing the tables in the contact database.

@param 	aItem The contact item to be added to the database. 
@param 	aSessionId The ID of the session that issued the request.  Used to
		prevent Phonebook Synchroniser deadlock.

@return Contact item ID of the contact added to the database.
*/
TContactItemId CPplContactItemManager::CreateL(CContactItem& aItem, TUint aSessionId)
	{
	TBool controlTransaction = !(iTransactionManager.IsTransactionActive());
	
	TBool compressedGuid=EFalse;

	// If needed generate a gui for the current contact item
	if (aItem.Guid() == TPtrC(KNullDesC))
		{
		iPreferencePersistor->SetGuidL(aItem, compressedGuid);
		}

	if (compressedGuid)
		{
		aItem.SetHasCompressedGuid(compressedGuid);
		}

	if (aItem.Type() == KUidContactICCEntry) 
		{
		const TInt ret = iContactProperties.ContactSynchroniserL(aSessionId).ValidateWriteContact(static_cast<CContactICCEntry&>(aItem));
		User::LeaveIfError(ret);	
		}
	
	if(controlTransaction)
		{
		StartTransactionL(aSessionId);
		}
		
	iContactTable->CreateInDbL(aItem);	
	iGroupTable->CreateInDbL(aItem);	
	iCommAddrTable->CreateInDbL(aItem);
#if defined(USE_PRED_SEARCH_TABLE)
	PRINT(_L("add new contact to pred search tables"));
	iPredSearch12keyTable->CreateInDbL(aItem);
#if defined(USE_QWERTY_PRED_SEARCH_TABLE)
	iPredSearchQwertyTable->CreateInDbL(aItem);
	iPredSearchSettingsTable->CreateInDbL(aItem);
#endif
	PRINT(_L("add new contact to pred search tables - done"));
#endif

   	TContactItemId groupId = iIccContactStore.CreateInDbL(aItem, aSessionId);
   	if(groupId != KNullContactId)
   	    {
   	    //Every ICC entry is added to a special group, created by the Phonebook
        //Synchroniser server during the initial synchronisation with the Contacts Model.
		CContactItemViewDef* itemDef = CContactItemViewDef::NewLC(CContactItemViewDef::EIncludeFields,CContactItemViewDef::EMaskHiddenFields);
		itemDef->AddL(KUidContactFieldMatchAll);

		// Add ICC entry to the group.
		CContactGroup* grp = static_cast<CContactGroup*>(ReadLC(groupId, *itemDef, EPlAllInfo, aSessionId));
		
		grp->AddContactL(aItem.Id());
		UpdateL(*grp, aSessionId);

		CleanupStack::PopAndDestroy(2, itemDef); // grp
   	    }

	if(controlTransaction)
		{
		CommitTransactionL();
		}
	
	// Assuming success if no leaves at this point, so update
	// the metadata search store for this entry
	//CCntMetadataOperation* op = CCntMetadataOperation::NewLC(iColSession);
	//TRAP_IGNORE(op->SaveContactLD(aItem));
	//CleanupStack::Pop(op); // Do not destroy - called LD function
	
	return aItem.Id();	
	}
	
/**
Reads the contact item from the database using the given contact item ID.

@param aItemId The Id number of the contact to be read.
@param aView Specifies the fields to be read.
@param aInfoToRead not used 
@param aSessionId The ID of the session that issued the request.  This is used
to prevent Phonebook Synchroniser deadlock.
@param aIccOpenCheck Specifies if validation with the Phonebook Synchroniser is
needed for this contact.

@leave 	KErrArgument 	if the itemID can't be set within select statement
@leave  KErrNotFound 	if a contact item with aItemId does not exist within contact database
@leave  KSqlErrBusy 	the database file is locked; thrown if RSqlStatement::Next() 
						returns this error
@leave  KErrNoMemory 	an out of memory condition has occurred - the statement
                        will be reset;thrown if RSqlStatement::Next() returns this error
@leave  KSqlErrGeneral	a run-time error has occured - this function must not
                        be called again;thrown if RSqlStatement::Next() returns this error
@leave	KSqlErrMisuse	this function has been called after a previous call
                        returned KSqlAtEnd or KSqlErrGeneral.thrown if RSqlStatement::Next() 
                        returns this error
@leave  KSqlErrStmtExpired	the SQL statement has expired (if new functions or
                            collating sequences have been registered or if an
                            authorizer function has been added or changed);
                            thrown if RSqlStatement::Next() returns this error

@return CContactItem created from reading the database tables.
*/	
CContactItem* CPplContactItemManager::ReadLC(TContactItemId aItemId, const CContactItemViewDef& aView, TInt aInfoToRead, TUint aSessionId, TBool aIccOpenCheck) const
	{	
	CContactTemplate* sysTemplate = NULL;
	if (aItemId != KGoldenTemplateId) 
		{
		sysTemplate = const_cast<CContactTemplate*>(&iContactProperties.SystemTemplateL());
		}
	
	RSqlStatement selectStmt;
	CleanupClosePushL(selectStmt);
	User::LeaveIfError(selectStmt.Prepare(iDatabase, iSelectStatement->SqlStringL()));
	
	TInt err = selectStmt.BindInt(KFirstParam, aItemId);
	
	if(err != KErrNone)
		{
		User::Leave(KErrArgument);	
		}
	
	CContactItem* item = NULL;
	TUid type(KNullUid);
	
	if((err = selectStmt.Next()) == KSqlAtRow)
		{
		TInt contactId = selectStmt.ColumnInt(iSelectStatement->ParameterIndex(KContactId));
		TInt templateId = selectStmt.ColumnInt(iSelectStatement->ParameterIndex(KContactTemplateId));
		TInt typeFlags = selectStmt.ColumnInt(iSelectStatement->ParameterIndex(KContactTypeFlags));
		type = TCntPersistenceUtility::TypeFlagsToContactTypeUid(typeFlags); 
		item = CContactItem::NewLC(type);
		item->SetId(contactId);
		TPtrC guid = selectStmt.ColumnTextL(iSelectStatement->ParameterIndex(KContactGuidString));
		item->SetUidStringL(guid);
		TInt attr = (typeFlags & EContactAttributes_Mask) >> EContactAttributes_Shift; 
		item->SetAttributes(attr);
		item->SetTemplateRefId(templateId);
		item->SetLastModified(TTime(selectStmt.ColumnInt64(iSelectStatement->ParameterIndex(KContactLastModified)))); 
		item->SetCreationDate(TTime(selectStmt.ColumnInt64(iSelectStatement->ParameterIndex(KContactCreationDate))));
		item->SetAccessCount(selectStmt.ColumnInt(iSelectStatement->ParameterIndex(KContactAccessCount)));
			
		RArray<TPtrC> fastAccessFields;
		CleanupClosePushL(fastAccessFields);
		fastAccessFields.AppendL(selectStmt.ColumnTextL(iSelectStatement->ParameterIndex(KContactFirstName)));
		fastAccessFields.AppendL(selectStmt.ColumnTextL(iSelectStatement->ParameterIndex(KContactLastName)));
		fastAccessFields.AppendL(selectStmt.ColumnTextL(iSelectStatement->ParameterIndex(KContactCompanyName)));
		fastAccessFields.AppendL(selectStmt.ColumnTextL(iSelectStatement->ParameterIndex(KContactFirstNamePrn)));
		fastAccessFields.AppendL(selectStmt.ColumnTextL(iSelectStatement->ParameterIndex(KContactLastNamePrn)));
		fastAccessFields.AppendL(selectStmt.ColumnTextL(iSelectStatement->ParameterIndex(KContactCompanyNamePrn)));
				
		//set first name, last name, company name, first name pronunciation, last name pronunciation, company name pronunciation
		for (TInt fieldNum = item->CardFields().Count() - 1; fieldNum>=0; --fieldNum) 
			{
			CContactItemField& textField = (item->CardFields())[fieldNum];
			const TInt nameFieldIndex = NameFieldIndex(textField);

			// Check if field is first name, last name, company name,
			// first name pronunciation, last name pronunciation, company name pronunciation.
			if (nameFieldIndex != KErrNotFound)
				{
				HBufC* text = HBufC::NewLC(fastAccessFields[nameFieldIndex].Size());
				text->Des() = fastAccessFields[nameFieldIndex];
				textField.TextStorage()->SetText(text);
				CleanupStack::PopAndDestroy(text);
				}
			}
			
		CleanupStack::PopAndDestroy(&fastAccessFields);		
		}
	else
		{
		if(err == KSqlAtEnd)
			{
			User::Leave(KErrNotFound);	// Select statement did not return any row		
			}
		else
			{
			User::Leave(err); 	// Otherwise selectStmt.Next() generated a sql error
			}	
		
		}

	if (!(item->IsDeleted()) || type == KUidContactCardTemplate)
		{
		TCntPersistenceUtility::ReadBlobL(*item, aView, sysTemplate, iDatabase);
		}
    
    //Fill in group content.
    static_cast<CPplGroupsTable*>(iGroupTable)->ReadL(*item);
    
    //Validate checking ICC entry.
    iIccContactStore.ReadL(*item, aInfoToRead, aSessionId, aIccOpenCheck);
    
	CleanupStack::Pop(item);
	CleanupStack::PopAndDestroy(&selectStmt);
	CleanupStack::PushL(item);
	
	return item;
	}
	
/** 
Updates the given contact item in the database.Forward the call to CPplTableBase
based classes representing the tables in the contact database.

@param aItem The contact to be updated with all the changes (received from the
user).
@param aSessionId The ID of the session that issued the request.  Used to
prevent Phonebook Synchroniser deadlock.
*/	
void CPplContactItemManager::UpdateL(CContactItem& aItem, TUint aSessionId, TBool /*aSpeeDailUpdate*/)
	{
	TBool controlTransaction = !(iTransactionManager.IsTransactionActive());
	
	if (aItem.Type() == KUidContactICCEntry) 
		{
        //Fakely validating read right to ICC entry to keep BC.
		const TInt ret = iContactProperties.ContactSynchroniserL(aSessionId).ValidateContact(MContactSynchroniser::ERead, aItem.Id());
		User::LeaveIfError(ret);	
		}
		
	if(controlTransaction)
		{
		StartTransactionL(aSessionId);
		}

	iIccContactStore.UpdateL(aItem, aSessionId);
	iContactTable->UpdateL(aItem);	
	iGroupTable->UpdateL(aItem);	
	iCommAddrTable->UpdateL(aItem);
#if defined(USE_PRED_SEARCH_TABLE)
	PRINT(_L("update contact in pred search tables"));
	iPredSearch12keyTable->UpdateL(aItem);
#if defined(USE_QWERTY_PRED_SEARCH_TABLE)
	iPredSearchQwertyTable->UpdateL(aItem);
	iPredSearchSettingsTable->UpdateL(aItem);
#endif
	PRINT(_L("update contact in pred search tables - done"));
#endif

	if(controlTransaction)
		{
		CommitTransactionL();
		}
		
	// If the item is the System template then delete it from the template
	// manager.  The template can later be recreated from the table.
	if (aItem.Id() == KGoldenTemplateId)
		{
		iContactProperties.SystemTemplateManager().DeleteTemplate();
		}
	
    // Assuming success if no leaves at this point, so update
    // the metadata search store for this entry
    //CCntMetadataOperation* op = CCntMetadataOperation::NewLC(iColSession);
    //TRAP_IGNORE(op->SaveContactLD(aItem));
    //CleanupStack::Pop(op); // Do not destroy - called LD function
	}

/**
Deletes the given contact from the database.Forward the call to CPplTableBase
based classes representing the tables in the contact database. In low disk condition
checks if it can gain access to reserver database space and perform the deletion. If 
this is not possible a KErrDiskFull will be thrown.

@param aItemId The contact ID of the contact item to be deleted.
@param aSessionId The ID of the session that issued the request.  Used to
prevent Phonebook Synchroniser deadlock.

@leave KErrDiskFull if a full disk error appears

@return The item which was read from the database before it was deleted.   This
object can be used by, for example, the CLplAnalyserProxy::DeleteLC() method to
create a notification message.
*/	
CContactItem* CPplContactItemManager::DeleteLC(TContactItemId  aItemId, TUint aSessionId, TCntSendEventAction /*aEventType*/)
	{
	TBool controlTransaction = !(iTransactionManager.IsTransactionActive());

	CContactItemViewDef* viewDef = CContactItemViewDef::NewL(CContactItemViewDef::EIncludeFields, CContactItemViewDef::EIncludeHiddenFields);
	CleanupStack::PushL(viewDef);

	if(controlTransaction)
		{
		StartTransactionL(aSessionId);
		}

	TBool lowDisk = EFalse;
	CContactItem* savedContactItem = static_cast<CPplContactTable*>(iContactTable)->DeleteLC(aItemId, lowDisk);
	if (lowDisk) 
		{
		CleanupStack::PopAndDestroy(savedContactItem); // this was returned from the first call, but not deleted in the db
		TInt err = iDatabase.GetReserveAccess();
		if(err != KErrNotFound)
			{
			User::Leave(KErrDiskFull);	
			}
		
		lowDisk = EFalse;
		
		savedContactItem = static_cast<CPplContactTable*>(iContactTable)->DeleteLC(aItemId, lowDisk);
		if(lowDisk)
			{
			iDatabase.FreeReservedSpace();
			User::Leave(KErrDiskFull);	
			}
		iDatabase.FreeReservedSpace();		
		}

	lowDisk = EFalse;	
	iGroupTable->DeleteL(*savedContactItem, lowDisk);	
	if (lowDisk) 
		{
		DeleteInLowDiskConditionL(iGroupTable, savedContactItem);
		}

	lowDisk = EFalse;
	iCommAddrTable->DeleteL(*savedContactItem, lowDisk);	
	if (lowDisk) 
		{
		DeleteInLowDiskConditionL(iCommAddrTable, savedContactItem);
		}

#if defined(USE_PRED_SEARCH_TABLE)
	PRINT(_L("delete contact from pred search tables"));
	lowDisk = EFalse;
    iPredSearch12keyTable->DeleteL(*savedContactItem, lowDisk);
    if (lowDisk) 
		{
		DeleteInLowDiskConditionL(iPredSearch12keyTable, savedContactItem);
		}
	lowDisk = EFalse;
#if defined(USE_QWERTY_PRED_SEARCH_TABLE)
	iPredSearchQwertyTable->DeleteL(*savedContactItem, lowDisk);
    if (lowDisk) 
		{
		DeleteInLowDiskConditionL(iPredSearchQwertyTable, savedContactItem);
		}
	iPredSearchSettingsTable->DeleteL(*savedContactItem, lowDisk);
    if (lowDisk) 
		{
		DeleteInLowDiskConditionL(iPredSearchSettingsTable, savedContactItem);
		}
#endif
	PRINT(_L("delete contact from pred search tables - done"));
#endif
  
    //Fake checking read access to ICCEntry store to keep BC.
    iIccContactStore.ReadL(*savedContactItem, EPlGroupMembershipInfo, aSessionId, EFalse);    
    
	//Don't need to check low disk state for ICC synchronizer.
	iIccContactStore.DeleteL(*savedContactItem, aSessionId);

	if(controlTransaction)
		{
		CommitTransactionL();  	// Pops item
		}
		
	CleanupStack::Pop();		// Pops item or transaction
	CleanupStack::PopAndDestroy(viewDef);	
	CleanupStack::PushL(savedContactItem);

	// Assume no leaves by this point indicates successful delete,
	// therefore update the metadata store
	//CCntMetadataOperation* op = CCntMetadataOperation::NewLC(iColSession);
	//TRAP_IGNORE(op->DeleteContactLD(aItemId));
	//CleanupStack::Pop(op); // Do not destroy - called LD function
	
	return savedContactItem;
	
	}
	
/**
Perform a deletion in low disk condition

@param aTable CPplTableBase on which the deletion will be done
@param aContactItem the contact item to be deleted

@leave KErrDiskFull if the deletion can't be done a KerrDiskFull will be throw
*/
void CPplContactItemManager::DeleteInLowDiskConditionL(CPplTableBase* aTable, CContactItem* aContactItem)
	{
	TInt err = iDatabase.GetReserveAccess();
	if(err != KErrNotFound)
		{
		User::Leave(KErrDiskFull);	
		}
		
	TBool lowDisk = EFalse;
	aTable->DeleteL(*aContactItem, lowDisk);
	if(lowDisk)
		{
		iDatabase.FreeReservedSpace();
		User::Leave(KErrDiskFull);	
		}
	iDatabase.FreeReservedSpace();		
	}
	
/**
Change the type of the given contact item to the given type.Forward the call to CPplTableBase
based classes representing the tables in the contact database.

@param aItemId Contact item ID whose type is to be changed.
@param aNewType New type for contact item.
*/	
void CPplContactItemManager::ChangeTypeL(TContactItemId aItemId, TUid aNewType)
	{
	static_cast<CPplContactTable*>(iContactTable)->ChangeTypeL(aItemId, aNewType);
	}

/** 
Does nothing: the CLplAnalyserProxy class sets the connetion ID.
*/
void CPplContactItemManager::SetConnectionId(TInt /*aConnectionId*/)
	{
	}
	
/**
Second phase constructor for CPplContactItemManager object.
Sets the CCntSqlStatement to be used for reading contact item.

*/
void CPplContactItemManager::ConstructL()
	{
	RDebug::Print(_L("CPplContactItemManager::ConstructL"));
	TCntSqlStatementType statementType(ESelect, KSqlContactTableName);
	
	iSelectStatement = TSqlProvider::GetSqlStatementL(statementType);
	
	// also prepare everything in CCntSqlStatement
	// First add columns from contact table
	iSelectStatement->SetParamL(KContactId, KSpace);
	iSelectStatement->SetParamL(KContactTemplateId, KSpace);
	iSelectStatement->SetParamL(KContactTypeFlags, KSpace);
	iSelectStatement->SetParamL(KContactAccessCount, KSpace);
	iSelectStatement->SetParamL(KContactCreationDate, KSpace);	
	iSelectStatement->SetParamL(KContactLastModified, KSpace);
	iSelectStatement->SetParamL(KContactGuidString, KSpace);
	iSelectStatement->SetParamL(KContactFirstName, KSpace);
	iSelectStatement->SetParamL(KContactLastName, KSpace);
	iSelectStatement->SetParamL(KContactCompanyName, KSpace);
	iSelectStatement->SetParamL(KContactFirstNamePrn, KSpace);
	iSelectStatement->SetParamL(KContactLastNamePrn, KSpace);
	iSelectStatement->SetParamL(KContactCompanyNamePrn, KSpace);
	
	HBufC* condition = HBufC::NewLC(KWhereStringEqualsStringFormatText().Size() + KContactId().Size());
	TPtr ptrCondition = condition->Des();
	ptrCondition.Format(KWhereStringEqualsStringFormatText, &KContactId, &KVar);
	
	// Set condition
	iSelectStatement->SetConditionL(ptrCondition);
	
	CleanupStack::PopAndDestroy( condition ); 

	// construct tables
	iContactTable = CPplContactTable::NewL(iDatabase, iContactProperties);
	iCommAddrTable = CPplCommAddrTable::NewL(iDatabase, iContactProperties);
	iGroupTable = CPplGroupsTable::NewL(iDatabase);
	iPreferencePersistor = CPplPreferencesPersistor::NewL(iDatabase);

#if defined(USE_PRED_SEARCH_TABLE)
	PRINT(_L("create C12keyPredictiveSearchTable object"));
	iPredSearch12keyTable = C12keyPredictiveSearchTable::NewL(iDatabase);
#if defined(USE_QWERTY_PRED_SEARCH_TABLE)
	PRINT(_L("create CQwertyPredictiveSearchTable object"));
	iPredSearchQwertyTable = CQwertyPredictiveSearchTable::NewL(iDatabase);
	PRINT(_L("create CPredictiveSearchSettingsTable object"));
	iPredSearchSettingsTable = CPredictiveSearchSettingsTable::NewL(iDatabase);

	iPredictiveSearchSynchronizer =
		CPredictiveSearchSynchronizer::NewL(iDatabase,
											*iPredSearch12keyTable,
											*iPredSearchQwertyTable,
											*iPredSearchSettingsTable);
#endif // #if defined(USE_QWERTY_PRED_SEARCH_TABLE)
#endif // #if defined(USE_PRED_SEARCH_TABLE)

	iPresenceTable = CPplPresenceTable::NewL(iDatabase); 
	
	// Connect to metadata server
	//User::LeaveIfError(iColSession.Connect());

	RDebug::Print(_L("CPplContactItemManager::ConstructL ends"));
	}

/**
Cleanup operation. If one of the crud operation fails, this method will be called
to rollback the sql transaction

*/
void CPplContactItemManager::CleanupOperationRollbackL(TAny* aPplContactItemanager)
	{
	CPplContactItemManager* contactItemManager = static_cast<CPplContactItemManager*>(aPplContactItemanager);
	contactItemManager->iTransactionManager.RollbackCurrentTransactionL(contactItemManager->iSessionId);
	}
	
/**
Utility method used to identify if the provided CContactItemField is first name, last name, 
company name, first name pronunciation, last name pronunciation or company name pronunciation 
Method used to fill information when a CContactItem is read from contact database

@param aNameField reference to a constant CContactItemField for which the index will be returned

*/	
TInt CPplContactItemManager::NameFieldIndex(const CContactItemField& aNameField) const
	{
	// For all the name fields...
	for (TInt nameFieldNum=0; nameFieldNum < sizeof(KFastAccessFieldUids)/sizeof(TInt); ++nameFieldNum)
		{
		if (aNameField.ContentType().ContainsFieldType(TUid::Uid(KFastAccessFieldUids[nameFieldNum])))
			{
			return nameFieldNum;
			}
		}
	return KErrNotFound;
	}
	
/**
Utility method used to start a transaction in database

@param aSessionId id of the session
*/
void CPplContactItemManager::StartTransactionL(TUint aSessionId)
	{
	iTransactionManager.StartTransactionL();
	
	iSessionId = aSessionId;
	CleanupStack::PushL(TCleanupItem(CleanupOperationRollbackL, this));		
	}
	
/**
Utility method used to commit a transaction
*/
void CPplContactItemManager::CommitTransactionL()
	{
	iTransactionManager.CommitCurrentTransactionL(iSessionId);
		
	CleanupStack::Pop(); //CleanupOperationRollbackL		
	}
	
/**
Utility method used to create tables in a newly create database
*/	
void CPplContactItemManager::CreateTablesL()
	{
	RDebug::Print(_L("CPplContactItemManager::CreateTablesL"));	

	TBool controlTransaction = !(iTransactionManager.IsTransactionActive());
	
	if (controlTransaction)
		{
		StartTransactionL(0);
		}
		
	iContactTable->CreateTableL();	
	iGroupTable->CreateTableL();	
	iCommAddrTable->CreateTableL();
	iPreferencePersistor->CreateTableL();
#if defined(USE_PRED_SEARCH_TABLE)
	PRINT(_L("create pred search tables to DB"));	
	iPredSearch12keyTable->CreateTableL();
#if defined(USE_QWERTY_PRED_SEARCH_TABLE)
	iPredSearchQwertyTable->CreateTableL();
	iPredSearchSettingsTable->CreateTableL();
#endif
	PRINT(_L("create pred search tables to DB - done"));	
#endif
	iPresenceTable->CreateTableL(); 
	
	if (controlTransaction)
		{
		CommitTransactionL();
		}			

	RDebug::Print(_L("CPplContactItemManager::CreateTablesL ends"));	
	}
	
/**
Utility method used to check if there is any record in the contact database

@return ETrue if the contact database is empty and EFalse otherwise
*/	
TBool CPplContactItemManager::IsDatabaseEmptyL()
	{
	return static_cast<CPplContactTable*>(iContactTable)->IsTableEmptyL();
	}

CContactIdArray* CPplContactItemManager::MatchPhoneNumberL(const TDesC& aNumber, TInt aMatchLengthFromRight)
	{
	// Call comm address table
	if (aMatchLengthFromRight == KBestMatchingPhoneNumbers)
        {
        return  static_cast<CPplCommAddrTable*>(iCommAddrTable)->BestMatchingPhoneNumberL(aNumber);
        }
    else
        {
        return  static_cast<CPplCommAddrTable*>(iCommAddrTable)->MatchPhoneNumberL(aNumber, aMatchLengthFromRight);
        }
	}

/**
Utility method used to retrieve an array of card template ids

@return reference to a CContactIdArray containing the card template ids
*/
CContactIdArray& CPplContactItemManager::CardTemplateIdsL()
	{
	// Call contact table
	return  static_cast<CPplContactTable*>(iContactTable)->CardTemplateIdsL();	
	}

/**
Utility method used to retrieve a reference to persistence layer preference persistor
*/
CPplPreferencesPersistor& CPplContactItemManager::PreferencesPersitor()  
	{
	return *iPreferencePersistor;
	}

/**
Utility method used to retrieve own card contact id
*/
TContactItemId CPplContactItemManager::OwnCardIdL()
	{
	// Call contact table
	return  static_cast<CPplContactTable*>(iContactTable)->OwnCardIdL();		
	}
	
/**
Utility method used to set own card id
*/	
void CPplContactItemManager::SetOwnCardIdL(TContactItemId aId)
	{	
	// Call contact table
	static_cast<CPplContactTable*>(iContactTable)->SetOwnCardIdL(aId);		
	}

/**
Utility class. Returns an array with ids of all groups in contact database
*/
CContactIdArray* CPplContactItemManager::GroupIdListL()
	{
	CContactIdArray* idArray = CContactIdArray::NewLC();

	HBufC* selectString = HBufC::NewLC(KOneTypeField().Length() + KSqlContactTableName().Length() + KContactTypeFlags().Length() + 3); 
	TPtr ptrSelectString = selectString->Des();
	ptrSelectString.Format(KOneTypeField, &KContactId, &KSqlContactTableName, &KContactTypeFlags, EContactType_Shift, EContactTypeFlags_Group);

	RSqlStatement selectStatement;
	CleanupClosePushL(selectStatement);
	
	User::LeaveIfError(selectStatement.Prepare(iDatabase, ptrSelectString));
	const TInt KIdx = iSelectStatement->ParameterIndex(KContactId);
	
	TInt err;
	while ((err = selectStatement.Next()) == KSqlAtRow)
		{
		idArray->AddL(selectStatement.ColumnInt(KIdx));	
		}

	if (err != KSqlAtEnd)
		{
		User::Leave(err);
		}

    CleanupStack::PopAndDestroy(&selectStatement);
	CleanupStack::PopAndDestroy(selectString);
	CleanupStack::Pop(idArray);
	
	return idArray;
	}

/**
Fast access method for retrieving a list of contact details as raw data.
This method can be used for example to fetch the names of all contacts
in a very efficient manner. It is assumed that the first column in
aSearchQuery is 'id' and that the other columns are text.

The returned buffer can be read with RBufReadStream - the first column
of each row is a 32-bit integer (the id), the rest of the columns are
16-bit descriptors. An id of 0 signifies the end of the list.
*/
CBufSeg* CPplContactItemManager::DetailsListL(const TDesC& aSearchQuery) const
    {
    CBufSeg* array = CBufSeg::NewL(4096);
    CleanupStack::PushL(array);

    //Prepare and execute the sql query
    RSqlStatement selectStatement;
    CleanupClosePushL(selectStatement);
    User::LeaveIfError(selectStatement.Prepare(iDatabase, aSearchQuery));

    // Iterate through the results and append the contactIds to idArray
    TInt err;
    RBufWriteStream stream;
    stream.Open(*array);
    TInt columnCount = selectStatement.ColumnCount();
    while ((err = selectStatement.Next()) == KSqlAtRow)
        {
        stream.WriteInt32L(selectStatement.ColumnInt(0));
		for (TInt i = 1; i < columnCount; ++i)
	        stream << selectStatement.ColumnTextL(i);
        }
    stream.WriteInt32L(0);
    stream.Close();

    if (err != KSqlAtEnd)
        {
        User::Leave(err);
        }

    //Cleanup 
    CleanupStack::PopAndDestroy(&selectStatement);
    CleanupStack::Pop(array);
    
    return array;
    }

/**
Utility method used to rthe prefered card template id
*/
TInt CPplContactItemManager::CardTemplatePrefIdL() const
	{
	return iPreferencePersistor->PreferredCardTemplateIdL();
	}

/**
Utility method used to set the prefered card template id
*/
void CPplContactItemManager::SetCardTemplatePrefIdL(TInt aCardTemplatePrefId)
	{
	iPreferencePersistor->SetPreferredCardTemplateIdL(aCardTemplatePrefId);
	}

void CPplContactItemManager::SynchronizePredSearchTableL()
	{
#if defined(USE_PRED_SEARCH_TABLE)
	iPredictiveSearchSynchronizer->SynchronizeTablesL();
#endif
	}

void CPplContactItemManager::RecreatePredSearchTablesL()
	{
#if defined(USE_PRED_SEARCH_TABLE)
	iPredictiveSearchSynchronizer->DeletePredSearchTablesL();
	iPredictiveSearchSynchronizer->CreatePredSearchTablesL();
#endif
	}
