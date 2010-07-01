/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#include <cntdb.h>
#include <phbksync.h>

#include "cntstd.h"
#include "cntphbksyncwatcher.h"


//uncomment for commonly required debug printing
//#define __VERBOSE_DEBUG__



/*
 class CContactPhbkSyncWatcher is a simple class that aggregates
 synchronisation events from the different ICC phonebooks (e.g. Global ADN or USIM App ADN)
 */
CContactPhbkSyncWatcher* CContactPhbkSyncWatcher::NewL(MContactSynchroniser& aSyncPlugin)
	{
	CContactPhbkSyncWatcher* self = new (ELeave) CContactPhbkSyncWatcher(aSyncPlugin);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	

/*
 Release all resources.
 */
CContactPhbkSyncWatcher::~CContactPhbkSyncWatcher()
	{
	__ASSERT_DEBUG(iObservers.Count() == 0, Panic(ECntPhonebookSyncObserverList));
	iObservers.Close();
	// delete each phbk watcher
	iWatchers.ResetAndDestroy();
	iWatchers.Close();
	}


CContactPhbkSyncWatcher::CContactPhbkSyncWatcher(MContactSynchroniser& aSyncPlugin) : iSyncPlugin(aSyncPlugin)
	{
	}


/*
 Create an array of active objects, one for each Phonebook on the ICC (SIM card),
 to observer Phonebook Synchroniser state change events.
 */
void CContactPhbkSyncWatcher::ConstructL()
	{
	RArray<TUid> phbkList;
	CleanupClosePushL(phbkList);

	// get list of phonebooks for type of ICC (SIM card) present
	TInt error = iSyncPlugin.PhonebookList(phbkList);
	User::LeaveIfError(error);
	const TInt count = phbkList.Count();

	for (TInt j = 0; j < count; ++j)
		{
		iWatchers.AppendL(CSinglePhbkWatcher::NewLC(*this, phbkList[j]));
		CleanupStack::Pop(); // CSinglePhbkWatcher
		}

	CleanupStack::PopAndDestroy();	// close phbkList
	}


/*
 Add Contact View to observer list.
 */
void CContactPhbkSyncWatcher::AddPhbkObserverL(MContactPhbkSyncObserver& aChangeNotifier)
	{
	iObservers.AppendL(&aChangeNotifier);
	}


/*
 Remove Contact View from observer list.
 */
void CContactPhbkSyncWatcher::RemovePhbkObserver(MContactPhbkSyncObserver& aChangeNotifier)
	{
	const TInt observerIndex = iObservers.Find(&aChangeNotifier);

	if (observerIndex != KErrNotFound)
		{
		iObservers.Remove(observerIndex);
		}
	}


/*
 Count of views Phonebooks that are Synchronised.
 */
TInt CContactPhbkSyncWatcher::PhonebooksReady()
	{
	// number of phonebooks that are ready
	return iPhonebooksReady;
	}

/*
 Count of views Phonebooks that are locked (PIN not yet entered).
 */
TInt CContactPhbkSyncWatcher::PhonebooksWaiting()
	{
	// number of phonebooks that are waiting
	return iPhonebooksWaiting;
	}

/*
 Last error, if any, from Phonebook Synchronisation.
 */
TInt CContactPhbkSyncWatcher::PhonebookSyncError()
	{
	// most recent phbksync error
	return iSyncError;
	}

/*
 Pointer to the Phonebook Synchroniser plug-in.
 */
MContactSynchroniser& CContactPhbkSyncWatcher::SyncPlugin()
	{
	return iSyncPlugin;
	}


/*
 Count of views observing Phonebook Synchroniser events.
 */
TInt CContactPhbkSyncWatcher::ObserverCount()
	{
	return iObservers.Count();
	}


/*
 Distributor of Phonebook Sync state changes.
 
 
 */
void CContactPhbkSyncWatcher::SendWatcherEvent(TPhonebookState aNewState, TPhonebookState aOldState, TInt aError)
	{
#if defined(__VERBOSE_DEBUG__)
	RDebug::Print(_L("[CNTMODEL] CContactPhbkSyncWatcher::SendWatcherEvent(NewState = %i, OldState = %i, error = %i)\n\r"), 
		aNewState, aOldState, aError);
#endif

	// Guard maybe the state has not really changed
	if (aNewState != aOldState)
		{
		// track numbers of synchronised & locked phonebooks:
		
		// decrement count associated with the old state
		switch (aOldState)
			{
		case EIccPhbkNotSynchronised:
			break;
		case EIccPhbkSynchronised:
			--iPhonebooksReady;
			break;
		case EIccWaitForPhbkToBeReady:
			--iPhonebooksWaiting;
			break;
		case EIccPhbkSyncError:
			--iPhonebookErrors;
			if (0 == iPhonebookErrors)
				{
				iSyncError = KErrNone;
				}
			break;
			}

		// increment count associated with the new state
		switch (aNewState)
			{
		case EIccPhbkNotSynchronised:
			break;
		case EIccPhbkSynchronised:
			++iPhonebooksReady;
			break;
		case EIccWaitForPhbkToBeReady:
			++iPhonebooksWaiting;
			break;
		case EIccPhbkSyncError:
			++iPhonebookErrors;
			iSyncError = aError;
			break;
			}

#if defined(__VERBOSE_DEBUG__)
		RDebug::Print(_L("[ PhonebooksReady = %i, PhonebooksWaiting = %i, PhonebookErrors = %i, SyncError = %i)\n\r"), 
			iPhonebooksReady, iPhonebooksWaiting, iPhonebookErrors, iSyncError);
#endif

		// distribute the state change to all observing contact views
		const TInt count = iObservers.Count();
		for(TInt loop = count; loop > 0; --loop)
			{
			iObservers[loop - 1]->ContactPhbkSyncEventHandler(aNewState);
			}
		}
	}


/*
 class CSinglePhbkWatcher tracks the status of an ICC phonebook

 This is a very simple CActive derived class.
 */
CSinglePhbkWatcher* CSinglePhbkWatcher::NewLC(CContactPhbkSyncWatcher& aSyncWatcher, TUid& aPhonebookUid)
	{
#if defined(__VERBOSE_DEBUG__)
	RDebug::Print(_L("[CNTMODEL] CSinglePhbkWatcher::CSinglePhbkWatcher(PhonebookUid = 0x%X)\n\r"), aPhonebookUid.iUid);
#endif

	CSinglePhbkWatcher* self = new(ELeave) CSinglePhbkWatcher(aSyncWatcher, aPhonebookUid);
	CleanupStack::PushL(self);
	return self;
	}
	
	
/*
 Standard CActive destructor.
 */
CSinglePhbkWatcher::~CSinglePhbkWatcher()
	{
	Cancel();
	}


/*
 Standard CActive constructor, adds itself to the active scheduler.
 */
CSinglePhbkWatcher::CSinglePhbkWatcher(CContactPhbkSyncWatcher& aSyncWatcher, TUid& aPhonebookUid) :
	CActive(CActive::EPriorityStandard), iSyncWatcher(aSyncWatcher), iPhonebookUid(aPhonebookUid), iPhbkState(EIccPhbkNotSynchronised)
	{
	CActiveScheduler::Add(this);
	// ensure active scheduler calls RunL for the first time
	TRequestStatus* ptrStatus = &iStatus;
	*ptrStatus = KRequestPending;
	User::RequestComplete(ptrStatus, KErrNone);
	SetActive();
	}


/*
 Register for Phonebook Synchroniser state change notification for this phonebook.
 */
void CSinglePhbkWatcher::Start()
	{
#if defined(__VERBOSE_DEBUG__)
	RDebug::Print(_L("[CNTMODEL] CSinglePhbkWatcher{PhonebookUid = 0x%X}::Start(), &iStatus = 0x%X\n\r"),
		iPhonebookUid.iUid, &iStatus);
#endif

	// Waiting for ICC to be ready and for this Phonebook to synchronise
	iSyncWatcher.SyncPlugin().NotifySyncStateChange(iStatus, iPhonebookUid);
	SetActive();
	}


/*
 Cancel any active request to the phonebook synchroniser
 */
void CSinglePhbkWatcher::DoCancel()
	{
#if defined(__VERBOSE_DEBUG__)
	RDebug::Print(_L("[CNTMODEL] CSinglePhbkWatcher{PhonebookUid = 0x%X}::DoCancel()\n\r"), iPhonebookUid.iUid);
#endif

	// cleanup for Delete, RunL may have left an outstanding NotifySyncStateChangeL request
	iSyncWatcher.SyncPlugin().CancelNotifyRequest(iPhonebookUid);
	}


/*
 Process Phonebook Synchroniser notifications for this Phonebook.
 
 Error conditions drop out to the RunError() handler.
 This can only end up at the Synchronised or Not Synchronised states,
 which are passed back to the parent CContactPhbkSyncWatcher.
 */
void CSinglePhbkWatcher::RunL()
	{
#if defined(__VERBOSE_DEBUG__)
	RDebug::Print(_L("[CNTMODEL] CSinglePhbkWatcher{PhonebookUid = 0x%X}::RunL(), iStatus = %i\n\r"), iPhonebookUid.iUid, iStatus.Int());
#endif
	
	const TInt status = iStatus.Int();

	// re-start, for next change
	Start();

	// ignore KErrCancel
	if (status != KErrCancel)
		{

		User::LeaveIfError(status);

		TBool isPhbkSync = iSyncWatcher.SyncPlugin().IsSynchronisedL(iPhonebookUid);

		TPhonebookState oldState = iPhbkState;
		if (isPhbkSync)
			{
			iPhbkState = EIccPhbkSynchronised;
			}
		else
			{
			iPhbkState = EIccPhbkNotSynchronised;
			}

		iSyncWatcher.SendWatcherEvent(iPhbkState, oldState);
		}
	}


/*
 Process Phonebook Synchroniser errors for this Phonebook.
 
 Error conditions either come from completion with an error code or 
 the Phonebook Synchroniser IsSynchronisedL() method leaving.
 This can only end up at the PhbkLocked or PhbkSyncError states,
 which are passed back to the parent CContactPhbkSyncWatcher.
 */
TInt CSinglePhbkWatcher::RunError(TInt aError)
	{
#if defined(__VERBOSE_DEBUG__)
	RDebug::Print(_L("[CNTMODEL] CSinglePhbkWatcher{PhonebookUid = 0x%X}::RunError(%i)\n\r"), iPhonebookUid.iUid, aError);
#endif

	// IsSynchronisedL() left
	if (IsAcceptableError(aError))
		{
		// notify that ICC phonebook is locked
		iSyncWatcher.SendWatcherEvent(EIccWaitForPhbkToBeReady, iPhbkState);
		iPhbkState = EIccWaitForPhbkToBeReady;
		}
	else
		{
		// notify that ICC phonebook sync has an error
		iSyncWatcher.SendWatcherEvent(EIccPhbkSyncError, iPhbkState, aError);
		iPhbkState = EIccPhbkSyncError;
		}
	return KErrNone;
	}
	
/*
Process Phonebook errors recieved in RunError and check if they should be treated
as acceptable errors or as sync errors.
*/
TBool CSinglePhbkWatcher::IsAcceptableError(TInt aSyncError)
	{
	switch(aSyncError)
		{
		case KErrAccessDenied:
		case KErrNotReady:
			{
			return ETrue;	
			}
		default:
			return EFalse;
		}	
	}


