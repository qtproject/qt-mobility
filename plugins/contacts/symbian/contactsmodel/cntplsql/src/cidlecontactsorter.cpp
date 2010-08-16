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

#include "cviewcontactmanager.h"
#include "cntviewprivate.h"
#include "cntstd.h"

/*
 * Factory constructor.
 */
CIdleContactSorter* CIdleContactSorter::NewL(MContactViewSortObserver& aObserver, MLplPersistenceLayerFactory& aFactory, TContactViewPreferences aRequestedViewPrefs)
	{
	CIdleContactSorter* self = new (ELeave) CIdleContactSorter(aObserver, aRequestedViewPrefs);
	CleanupStack::PushL(self);
	self->ConstructL(aFactory);
	CleanupStack::Pop(self);
	return self;
	}

	
/* Constructor */
CIdleContactSorter::CIdleContactSorter(MContactViewSortObserver& aObserver, TContactViewPreferences aRequestedViewPrefs) 
: CAsyncOneShot(CActive::EPriorityLow), 
  iSortObserver(aObserver), 
  iRequestedView(aRequestedViewPrefs)
    {
    }

	
/* 
 * Second phase construction.
 * Copy the View's requested preferences, get link to ICC phonebook watcher
 */
void CIdleContactSorter::ConstructL(MLplPersistenceLayerFactory& aFactory)
	{
	// is ICC phonebook sync expected?
	if (iRequestedView & (EICCEntriesOnly | EICCEntriesAndContacts))
		{
		iPhbkSyncWatcher = CContactPhbkSyncWatcher::NewL(aFactory.GetContactSynchroniserL(KMaxTUint32));

		// observe ICC sync events
		iPhbkSyncWatcher->AddPhbkObserverL(*this);
		}
	}
	
	
/* Destructor */
CIdleContactSorter::~CIdleContactSorter()
	{
	Cancel();

	if (iPhbkSyncWatcher)
		{
		iPhbkSyncWatcher->RemovePhbkObserver(*this);
		if (iPhbkSyncWatcher->ObserverCount() == 0)
			{
			delete iPhbkSyncWatcher;
			}
		} //if (iPhbkSyncWatcher)
	}

	
/* Cancel any active requests to the phonebook synchroniser */
void CIdleContactSorter::DoCancel()
	{
	// Nothing to do.
	}

	
/** 
 * Uses a simple state machine, initial iSortState is set by Start() to either
 * EInsertSortFinal or EWaitingForInitialICCReady
 *
 * Either Insert Sort all or part of the requested view.
 * (CIdle::RunL calls back to the Insert Sort code.)
 *    State
 *    EInsertContactsOnlyIccLocked		  insert Contacts only (in a mixed view)
 *        goes to EContactsReadyWaitICCUnlock
 *    EInsertSortFinal                    insert all Contacts & ICC entries, or
 *        goes to ESortDone               add ICC entries to mixed view
 *                                        (iSortView specifies which)
 * Or wait for Phonebook Synchroniser to either finish or fail
 *    (failure other than SIM Locked causes a Sort Error) 
 *    State
 *    EWaitingForInitialICCReady          the view has nothing in: Phonebook
 *        goes to EInsertSortFinal        Synchronised allows full view to be available;
 *        or EInsertContactsOnlyIccLocked SIM Locked allows a view without ICC entries to 
 *                                        accessible
 *    EContactsReadyWaitICCUnlock         SIM was previously found to be locked, if/when
 *        goes to EInsertSortFinal        Phonebook Synchroniser completes we can merge in
 *                                        requested ICC entries
 *
 * The check whether the phonebook synchroniser is in a cache-valid state:-
 *
 * This check is done by making a async request to be completed when the 
 * phbksync cache state has changed, checking the current cache state and 
 * if the cache is valid already cancelling the request.
 * (The cancelled request will complete, causing RunL to run again.)
 * If there was a phbksync error check the error code, if it is not due to the
 * SIM card being locked then Leave.
 */
void CIdleContactSorter::RunL()
	{
	User::LeaveIfError(iStatus.Int());

	// either sort or wait for ICC ready / phonebook synch state change
	switch (iSortState)
		{
		// states that are sorting all or part of view
	case EInsertSortFinal:				// full insert sort or Phonebook Synched so add ICC entries
	case EInsertContactsOnlyIccLocked:	// insert Contacts for now, then wait for SIM to be unlocked
		// do slice of full / Contacts only /ICC only insert sort
		if (iSortObserver.IdleSorterNotifyL(KErrNone))
			{ // CAsyncOneShot::Call()
			Call();
			}
		else if(iSortState == EInsertContactsOnlyIccLocked)
			{
			// we are now waiting for phbksync, so that we can add ICC entries
			ChangeSortState(EContactsReadyWaitICCUnlock);
			}
		else
			{
			// CIdleContactSorter all done
			iSortState = ESortAllDone;
			}
		
		break;

		// states that are waiting for a phonebook sync event
	case EWaitingForInitialICCReady:	// ICC entries in view, waiting for Phonebook Synch state change
		// ICC entries are included in view:
		if (iPhbkSyncWatcher->PhonebooksReady() > 0)
			{
			// ICC sync complete - can immediately sort everything
			ChangeSortState(EInsertSortFinal);
			}
		else if (iPhbkSyncWatcher->PhonebooksWaiting() > 0)
			{
			// SIM card is locked and, this is the first time we've seen this

			// Insert Contacts (if wanted) into View now
			// Afterwards we will wait again for SIM to unlock & Phonebook Synch to complete
			if(iRequestedView & EICCEntriesAndContacts)
				{
				// insert/sort view, but without the requested ICC entries
				iSortView = static_cast<TContactViewPreferences>(iSortView & ~EICCEntriesAndContacts);
				ChangeSortState(EInsertContactsOnlyIccLocked);
				}
			else 
				{
				iSortObserver.IccViewNotifyL(MContactViewSortObserver::TIccViewNotify_IccOnlyLocked);
				
				// now wait for SIM to unlock & Phonebook Synch to complete
				ChangeSortState(EContactsReadyWaitICCUnlock);
				} 
			}
		else
			{
			// synchronisation finished with an error?
			User::LeaveIfError(iPhbkSyncWatcher->PhonebookSyncError());
			}

		// otherwise wait for a Phonebook Synch event
		break;

	case EContactsReadyWaitICCUnlock:	// when SIM is unlocked add ICC Entries to this view
		// ICC entries are included in view:
		if (iPhbkSyncWatcher->PhonebooksReady() > 0)
			{
			// ICC sync complete - can sort everything
			ChangeSortState(EInsertSortFinal);

			// add requested ICC entries into the sorted view
			iSortView = STATIC_CAST(TContactViewPreferences, (iSortView & ~EContactsOnly) | EICCEntriesOnly);
			
			iSortObserver.IccViewNotifyL(MContactViewSortObserver::TIccViewNotify_IccUnlocked);
			}	

		// otherwise wait for a Phonebook Synch event
		break;

	case ESortAllDone: // shouldn't have come back here
	default:
		Panic(ECntPanicViewSorterStateMachine);
		break;
		}

	}


/* 
 * Change the current sort state.
 * 
 * @param aNewSortState the new sort state to be set.
 */
void CIdleContactSorter::ChangeSortState(TSorterState aNewSortState)
	{
	// new state
	iSortState = aNewSortState;
	// make the active object to run, CAsyncOneShot::Call()
	Call();
	}


/* 
 * Handle any leave during CIdleContactSorter::RunL. 
 * The local view is informed of that the view construction failed.
 * It will broadcast indirectly an ESortError view event to all clients of this view.
 * 
 * @param aError Leave code from RunL
 */
TInt CIdleContactSorter::RunError(TInt aError)
	{
	if ((aError != KErrCancel) && (iSortState != ESortAllDone))
		{
		TInt err = KErrNone;
		TRAP(err, iSortObserver.IdleSorterNotifyL(aError));
		iSortState = ESortAllDone;
		}
	return KErrNone;
	}


/*
 Initialise Idle Contact Sorter for a new sort

 Re-init iSortView - the view filter for the Insert Sort
 Decide the initial iSortState for RunL:
   Contacts only view -> EInsertSortFinal
   ICC entries included -> EWaitingForInitialICCReady
 */
void CIdleContactSorter::Start()
	{
	// initially we will try to insert sort everything requested
	iSortView = iRequestedView;

	if (iPhbkSyncWatcher)
		{
		// ICC entries included in view, must wait for Phonebook Synch
		iSortState = EWaitingForInitialICCReady;
		}
	else
		{
		// Only Contacts wanted in view, we can Sort straight away
		iSortState = EInsertSortFinal;
		}

	// set Active for the first time, CAsyncOneShot::Call()
	Call();
	}


/*
 Stop any sort that is already in progress
 */
void CIdleContactSorter::Stop()
	{
	if (iSortState != ESortAllDone)
 		{
		// stop sorting
		iSortState = ESortAllDone;
		Cancel();
 		}
	}


/* 
 * Determines whether view events should be queued.
 * 
 * @return ETrue, if view events should be queued. EFalse, otherwise
 */
TBool CIdleContactSorter::IsICCSynchronised() const
	{
	// Initial wait for phonebook synch (i.e. waiting for ICC ready or locked) ?
	if(iSortState == EWaitingForInitialICCReady)
		{
		return EFalse;
		}
	return ETrue;
	}

	
/**
 * Current View Preferences for insert sort in to View
 *
 * May be a subset of the requested View.
 * If the SIM card is locked this will initially be a View without ICC entries.
 * If the SIM becomes unlocked a second pass then picks out ICC entries only.
 *
 */
TContactViewPreferences CIdleContactSorter::SortViewPreferences() const
	{
	return iSortView;
	}


/**
 * Modifies View Preferences for inserting into View
 *
 * May be a subset of the requested View:
 * If a Mixed (Contacts & ICC view) is requested and the Phonebook Synch has NOT
 * completed then only Contacts entries are added to the view. When the PhoneBook Synch 
 * completes all ICC entries will at the same time.
 *
 */
TBool CIdleContactSorter::InsertViewPreferences(TContactViewPreferences &aInsertView) const
	{
	TBool okayToInsert = ETrue;

	switch (iSortState)
		{
	case EInsertSortFinal:				// full insert sort or Phonebook Synched so add ICC entries
	case ESortAllDone:
		// view is finished or finishing, can Insert any contact
		break;

	case EInsertContactsOnlyIccLocked:	// insert Contacts for now, then wait for SIM to be unlocked
		// only Contacts can be inserted now, no ICC entries
		aInsertView = iSortView;
		break;

	case EWaitingForInitialICCReady:	// ICC entries in view, waiting for Phonebook Synch state change
		// Waiting for initial ICC Synch result, insert nothing
		okayToInsert = EFalse;
		break;

	case EContactsReadyWaitICCUnlock:	// when SIM is unlocked add ICC Entries to this view
		if (aInsertView & EICCEntriesOnly)
			{
			okayToInsert = EFalse;		// can't insert ICC entries yet
			}
		else
			{
			aInsertView = iSortView;	// only Insert Contacts
			}
		break;
		}

	return okayToInsert;
	}

	
/**
 * PhoneBook Synch event handler
 *
 * @param aPhbkState the new phone book synch state.
 */
void CIdleContactSorter::ContactPhbkSyncEventHandler(TPhonebookState aPhbkState)
	{
	switch (aPhbkState)
		{
		case EIccPhbkNotSynchronised:
			/* Initial state, or ICC card has 'gone away' (e.g. ICC is resetting). */
			// no action - may want to act on this in future
			break;

		case EIccPhbkSynchronised:	// ICC Phonebook has completed synchronisation.

		case EIccWaitForPhbkToBeReady:	// Sync failed due to ICC being locked or not ready.

		case EIccPhbkSyncError:		//	Sync with Phbk Server failed.

			// in a sorting state where we care?
			if ((iSortState == EWaitingForInitialICCReady) || (iSortState == EContactsReadyWaitICCUnlock))
				{
				// let state machine in RunL() deal with the event
				if (!IsActive())
					{ // CAsyncOneShot::Call()
					Call();
					}
				}
			break;
		}
	}
