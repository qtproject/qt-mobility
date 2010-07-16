/*
* Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef CNTSYNCCHECKER_H
#define CNTSYNCCHECKER_H

#include <e32base.h>
#include <cntsync.h>
#include <cntdb.h>

/*
const TUid KUidIccGlobalAdnPhonebook	= { 0x10001111 };
const TUid KUidIccGlobalSdnPhonebook	= { 0x10002222 };
const TUid KUidIccGlobalLdnPhonebook	= { 0x10003333 };
const TUid KUidUsimAppAdnPhonebook		= { 0x10004444 };
*/

const TUid KPropertyCategory = { 0x1020384e };

const TUid KUidIccPhonebookNotFound		= {-0x00000001 };

class TLookupPhonebookEntry
	{
	public:
		TLookupPhonebookEntry(TUid aPhonebookUid,TInt aPhonebookTemplateId,TInt aPhonebookGroupId);
	public:
		TUid iPhonebookUid;
		TInt iPhonebookTemplateId;
		TInt iPhonebookGroupId;
	};

namespace SyncNotification
    {
    const TUid KPropertyCategory = { 0x1020384E };

    enum TPropertyKeys
        {
        ESyncChangeNotification
        };    
    }

/** 
 Simulated Phonebook Synchronisation - to test the MContactSynchroniser interface is called correctly.

 Steps:
 1. Choose which ICC phonebooks should be present
	TInt SetGsmPhonebookL();
	TInt SetUsimPhonebookL();
	TInt SetPhonebookListL(const RArray<TUid> aNewPhonebookList);
 
 2. Define zero or more contacts in the phonebook(s)
	TInt AddContactToPhonebookL(TUid aPhonebookUid, const TDesC& aName, const TDesC& aNumber);
 
 3. Perform the synchronisation, optionally one phonebook at a time, optionally with an error code
	TInt SynchroniseIccPhonebookL(CContactDatabase& aDb, TUid aPhonebookUid, TInt aCompletionError, TInt aSyncError=KErrNone);
	TInt SynchroniseIccAllPhonebooksL(CContactDatabase& aDb, TInt aCompletionError, TInt aSyncError=KErrNone);

	These methods simulate a real phonebook synchronisation taking place,
	(though without ICC read delays):
	+ completing all queued notifications for the phonebook.
	+ creating a template & group for each phonebook, if none exists,
	  or finds the current template & group id so that the ICCtemplateIdL() &
	  GroupIdL() methods work
 
 4. Reset for next test
	TInt ResetSyncIccPhonebookL(TUid aPhonebookUid);
	TInt ResetSyncIccAllPhonebooksL();

 5. At any time set or change the response to ICC Validate requests
	TInt ValidateResponseL(MContactSynchroniser::TValidateOperation aOp);
	TInt ValidateWriteResponseL();
	TInt DeleteContactResponseL();

 6. At any time track the number of calls to MContactSynchroniser interface
	void ResetMethodCallCount();
	TInt MethodCallCount();
  
 */
class CContactSyncChecker : public CBase
	{
public:
	IMPORT_C static CContactSyncChecker* NewL();
	IMPORT_C ~CContactSyncChecker();

	IMPORT_C static TInt ValidateResponseL(MContactSynchroniser::TValidateOperation aOp);
	IMPORT_C static TInt ValidateWriteResponseL();
	IMPORT_C static TInt DeleteContactResponseL();
	IMPORT_C static TContactItemId ICCTemplateIdL(TUid aPhonebookUid);
	IMPORT_C static TContactItemId GroupIdL(TUid aPhonebookUid);
	IMPORT_C static TUid PhonebookUidL(TContactItemId aTemplateId);

	IMPORT_C static void SetValidateResponseL(MContactSynchroniser::TValidateOperation aOp, TInt aError);
	IMPORT_C static void SetValidateWriteResponseL(TInt aError);
	IMPORT_C static void SetDeleteContactResponseL(TInt aError);
	
	
	IMPORT_C static TInt UpdatePhonebookTemplateIdL(TUid aPhonebookUid, TInt aPhonebookTemplateId);
	IMPORT_C static TInt UpdatePhonebookGroupIdL(TUid aPhonebookUid, TInt aPhonebookGroupId);
	
	//SetPhonebookEntryL does not exist
	IMPORT_C static void SetPhonebookEntryL(TUid aPhonebookUid,TInt aPhonebookTemplateId,TInt aPhonebookGroupId);
	IMPORT_C static TInt UpdatePhonebookEntryL(TUid aPhonebookUid,TInt aPhonebookTemplateId,TInt aPhonebookGroupId);

	IMPORT_C static TInt SynchroniseIccPhonebookL(CContactDatabase& aDb, TUid aPhonebookUid, TInt aCompletionError, TInt aSyncError);
	IMPORT_C static TInt SynchroniseIccAllPhonebooksL(CContactDatabase& aDb, TInt aCompletionError, TInt aSyncError);
	IMPORT_C static TInt AddContactToPhonebookL(TUid aPhonebookUid, const TDesC& aName, const TDesC& aNumber);


	IMPORT_C static void EnableVerboseLoggingL();
	IMPORT_C static TBool VerboseLoggingEnabledL();


	IMPORT_C TInt ResetSyncIccPhonebookL(TUid aPhonebookUid, TBool aResetTemplateAndGroupIds);
	IMPORT_C TInt ResetSyncIccAllPhonebooksL(TBool aResetTemplateAndGroupIds);


	/*
	Reset Synchronisation state for all phonebooks, reset plug-in call count and set all validate 
	responses to KErrNotFound.
	*/

	// count the phbksync plug-in method calls
	IMPORT_C static void ValidateMethodCalledL();
	IMPORT_C static void SyncMethodCalledL();
	IMPORT_C static void PluginLoadedL();
	IMPORT_C static void PluginUnloadedL();
	IMPORT_C static void ResetMethodCallCountsL();
	IMPORT_C static TInt ValidateMethodCallCountL();
	IMPORT_C static TInt SyncMethodCallCountL();
	IMPORT_C static TInt PluginLoadedCountL();
	
	IMPORT_C static void ResetL();
	IMPORT_C static void ResetEverythingL();
	IMPORT_C static TInt TotalNotificationPendingCountL();
	IMPORT_C static TInt FetchNotificationErrorL(TUid aPhoneBookUid);
	IMPORT_C static void SetSynchronisedL(TBool aIsSynchronised, TUid aPhonebookUid); 
	IMPORT_C static void StoreNotificationRequestL(TRequestStatus& aStatus, TUid aPhonebookUid);
	IMPORT_C TBool IsSynchronisedL(TUid aPhonebookUid);
	IMPORT_C static void CompleteNotificationRequestL(TInt aError, TUid aPhonebookUid); 
	IMPORT_C static void SetSynchronisedLeaveCodeL(TInt aLeave, TUid aPhonebookUid); 
 	IMPORT_C static TInt NotificationPendingCountL(TUid aPhonebookUid); 

	/*
	Configure simulation for GSM Phonebook.
	*/
	IMPORT_C static TInt SetGsmPhonebookL();

	/*
	Configure simulation for USIM (UMTS/WCDMA) Phonebook.
	*/
	IMPORT_C static TInt SetUsimPhonebookL();

	/*
	Configure custom Phonebook simulation.
	*/
	IMPORT_C static TInt SetPhonebookListL(const RArray<TUid> aNewPhonebookList);
	/*
	Get phonebook list
	*/
	IMPORT_C static TInt PhonebookListL(RArray<TUid>& aPhonebookList);

 	
	};

#endif
