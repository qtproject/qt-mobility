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




/**
 @file
 @internalComponent
 @released
*/


#ifndef __CNTPHBKSYNCWATCHER_H__
#define __CNTPHBKSYNCWATCHER_H__


#include <cntsync.h>


class CSinglePhbkWatcher;


/**
Types of ICC Phonebook synchronisation tht Contact Views care about events.
*/
enum TPhonebookState 
	{
	/**
	Initial state, or ICC card has 'gone away' (e.g. ICC is resetting).
	*/
	EIccPhbkNotSynchronised,
	/**
	ICC Phonebook has completed synchronisation.
	*/
	EIccPhbkSynchronised,
	/**
	Sync failed due to ICC either being locked or not ready.
	*/
	EIccWaitForPhbkToBeReady,
	/**
	ICC Sync with Phonebook Server failed.
	*/
	EIccPhbkSyncError,
	};
	

/**	
An observer class for Contacts Views to track the status of ICC Phonebook
Synchronisation.
*/
class MContactPhbkSyncObserver
	{
public:
	virtual void ContactPhbkSyncEventHandler(TPhonebookState aPhbkState) = 0;
	};


/**
A watcher class for Contacts Views to track the status of ICC Phonebook
Synchronisation.
	
If created with a NULL pointer to the Phonebook Sync plugin implements default
behaviour of notifications (e.g. no Phonebook Synchroniser plugin present or
non-default database).
*/
NONSHARABLE_CLASS(CContactPhbkSyncWatcher) : public CBase
	{
public:
	static CContactPhbkSyncWatcher* NewL(MContactSynchroniser& aSyncPlugin);
	~CContactPhbkSyncWatcher();

	/**
	Add the object that sorts a contact view to the PhbkSync observer list.
	*/
	void AddPhbkObserverL(MContactPhbkSyncObserver& aPhbkObserver);

	/**
	Add the object that sorts a contact view to the PhbkSync observer list.
	*/
	void RemovePhbkObserver(MContactPhbkSyncObserver& aPhbkObserver);

	TInt PhonebooksReady();
	TInt PhonebooksWaiting();
	TInt PhonebookSyncError();
	
	TInt ObserverCount();

private:
	CContactPhbkSyncWatcher(MContactSynchroniser& aSyncPlugin);
	void ConstructL();

	friend class CSinglePhbkWatcher;
	MContactSynchroniser& SyncPlugin();
	void SendWatcherEvent(TPhonebookState aNewState, TPhonebookState aOldState, TInt aError=KErrNone);

private:
	MContactSynchroniser&				iSyncPlugin;
	RArray<MContactPhbkSyncObserver*>	iObservers;
	RPointerArray<CSinglePhbkWatcher>	iWatchers;
	TInt								iPhonebooksReady;
	TInt								iPhonebooksWaiting;
	TInt								iPhonebookErrors;
	TInt								iSyncError;
	};
	

NONSHARABLE_CLASS(CSinglePhbkWatcher) : public CActive
	{
public:
	static CSinglePhbkWatcher* NewLC(CContactPhbkSyncWatcher& aSyncWatcher, TUid& aPhonebookUid);
	~CSinglePhbkWatcher();
	TBool IsAcceptableError(TInt aSyncError);

private:
	CSinglePhbkWatcher(CContactPhbkSyncWatcher& aSyncWatcher, TUid& aPhonebookUid);

	void ConstructL();
	void Start();

	// From CActive.
	void DoCancel();
	void RunL();
	TInt RunError(TInt aError);

private:
	CContactPhbkSyncWatcher& 	iSyncWatcher;
	TUid						iPhonebookUid;
	TPhonebookState				iPhbkState;
	};


#endif // __CNTPHBKSYNCWATCHER_H__
