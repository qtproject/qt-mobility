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

#ifndef __PPLCONTACTITEMMANAGER_H__
#define __PPLCONTACTITEMMANAGER_H__

#include <e32base.h>
#include <sqldb.h>
//#include <columbo.h>
#include "cntsqlprovider.h"
#include "clplcontactproperties.h" 
#include "persistencelayer.h"
#include "rpplicccontactstore.h"

class CPplTableBase;
class CPplPreferencesPersistor;
class CPplPredictiveSearchTableBase;
class C12keyPredictiveSearchTable;
class CQwertyPredictiveSearchTable;
class CPredictiveSearchSettingsTable;
class CPredictiveSearchSynchronizer;


/**
The CPplContactItemManager implements the MLplPersistenceBroker.

MLplPersistenceBroker is a public interface responsible for carrying out the
basic crud operations (Create, Read, Update, Delete). It is exposed to the users 
of Persistence Layer making the CPplContactItemManager an entry point for these 
operations.

CPplContactItemManager is responsible to forward the Create, Delete and Update calls
to the CPplTableBase based object representing actual tables in the contact database.
CPplContactItemManager holds the knowledge of the database structure: how many tables
and what tables compose the contact database.

The Read operation is managed inside CPplContactItemManager and not passed to the table
classes.
*/
NONSHARABLE_CLASS(CPplContactItemManager): public CBase, public MLplPersistenceBroker, public MLplFieldMatcher
{
public:
	static CPplContactItemManager* NewL(RSqlDatabase& aDatabase, MLplTransactionManager& aTransactionManager, CLplContactProperties&  aContactProperties, RPplIccContactStore& aIccContactStore);
	~CPplContactItemManager();
	TContactItemId CreateL(CContactItem& aItem, TUint aSessionId);
	CContactItem* ReadLC(TContactItemId aItemId, const CContactItemViewDef& aView, TInt aInfoToRead, TUint aSessionId, TBool aIccOpenCheck = EFalse) const;
	void UpdateL(CContactItem& aItem, TUint aSessionId, TBool aSpeeDailUpdate = EFalse);
	CContactItem* DeleteLC(TContactItemId  aItemId, TUint aSessionId, TCntSendEventAction aEventType);	
	void ChangeTypeL(TContactItemId aItemId, TUid aNewType);
	void SetConnectionId(TInt aConnectionId);
	void CreateTablesL();
	CContactIdArray* MatchPhoneNumberL(const TDesC& aNumber, TInt aMatchLengthFromRight);
	TBool IsDatabaseEmptyL();
	CPplPreferencesPersistor& PreferencesPersitor();
	CContactIdArray* GroupIdListL();	
    CBufSeg* DetailsListL (const TDesC& aSearchQuery) const;
    CContactIdArray& CardTemplateIdsL();
    TContactItemId OwnCardIdL();
    void SetOwnCardIdL(TContactItemId aId, TBool aPersist = ETrue);
    TInt CardTemplatePrefIdL() const;
    void SetCardTemplatePrefIdL(TInt aCardTemplatePrefId);
	void SynchronizePredSearchTableL();
	void RecreatePredSearchTablesL();

private:
	CPplContactItemManager(RSqlDatabase& aDatabase,
						   MLplTransactionManager& aTransactionManager,
						   CLplContactProperties& aContactProperties,
						   RPplIccContactStore& aIccContactStore);
	void ConstructL();
	static void CleanupOperationRollbackL(TAny* aDatabase);
	
	void StartTransactionL(TUint aSessionId);
	void CommitTransactionL();
	
	TInt NameFieldIndex(const CContactItemField& aNameField) const;
	void DeleteInLowDiskConditionL(CPplTableBase* aTable, CContactItem* aContactItem);

private:
	RSqlDatabase&				  iDatabase; // CPplContactItemManager does not own the RSqlDatabase object
	MLplTransactionManager& 	  iTransactionManager;
	TUint						  iSessionId;
	CCntSqlStatement* 			  iSelectStatement;
	CLplContactProperties&  	  iContactProperties;
	CPplTableBase* 				  iContactTable;
	CPplTableBase* 				  iGroupTable;	
	CPplTableBase* 			      iCommAddrTable;
	C12keyPredictiveSearchTable*  iPredSearch12keyTable;
	CQwertyPredictiveSearchTable* iPredSearchQwertyTable;
	CPredictiveSearchSettingsTable* iPredSearchSettingsTable;
	CPplTableBase*                iPresenceTable;
	CPplPreferencesPersistor*     iPreferencePersistor;
	RPplIccContactStore&          iIccContactStore;
	//RColumboSession             iColSession;
	CPredictiveSearchSynchronizer* iPredictiveSearchSynchronizer; // Owned

	// For unit testing
	friend class UT_CPplPredictiveSearchTable;
};

#endif // __PPLCONTACTITEMMANAGER_H__
