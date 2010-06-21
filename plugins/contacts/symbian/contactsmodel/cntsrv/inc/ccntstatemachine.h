/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __CCNTSTATEMACHINE_H__
#define __CCNTSTATEMACHINE_H__

#include "cactiveloop.h" // for mactiveloopcallback.
#include <cntdef.h>  	 // for tcontactitemid.
#include "ccntrequest.h"

namespace nsState 
	{			  
	const TInt KLastStep    = 0;
	const TInt KNoSessionId = 0;
	
	enum TStates
		{
		EStateClosed 	    = 0,
		EStateTablesClosed,
		EStateWritable,
		EStateOpening,
		EStateTransaction,
		EStateBackupRestore
		};
	}


// Forward class references.	
class CCntStateMachine;
class CTransactionTimer;
class CCntRequest;
class CPersistenceLayer;
class CCntDbManager;
class MLplContactsFile;


/**
This class is the base class for each State object held in the State Machine.
*/
class CState : public CBase, public MContactDbObserverV2
	{
public:
	~CState();

	virtual TAccept AcceptRequestL(CReqAsyncOpen*    	 aRequest); //visitor pattern
	virtual TAccept AcceptRequestL(CReqUpdateCnt*    	 aRequest); //visitor pattern
	virtual TAccept AcceptRequestL(CReqCommitCnt*    	 aRequest); //visitor pattern
	virtual TAccept AcceptRequestL(CReqReadCnt*			 aRequest); //visitor pattern
	virtual TAccept AcceptRequestL(CReqOpenCnt*			 aRequest); //visitor pattern
	virtual TAccept AcceptRequestL(CReqDeleteCnt*		 aRequest); //visitor pattern
	virtual TAccept AcceptRequestL(CReqCloseCnt* 		 aRequest); //visitor pattern
	virtual TAccept AcceptRequestL(CReqCreateCnt*		 aRequest); //visitor pattern
	virtual TAccept AcceptRequestL(CReqCancelAsyncOpen*  aRequest); //visitor pattern
  	virtual TAccept AcceptRequestL(CReqCloseTables*  	 aRequest); //visitor pattern	
	virtual TAccept AcceptRequestL(CReqReOpen*  	  	 aRequest); //visitor pattern
	virtual TAccept AcceptRequestL(CReqDbBeginTrans*     aRequest); //visitor pattern
	virtual TAccept AcceptRequestL(CReqDbCommitTrans*    aRequest); //visitor pattern
	virtual TAccept AcceptRequestL(CReqDbRollbackTrans*  aRequest); //visitor pattern
	virtual TAccept AcceptRequestL(CReqBackupRestoreBegin* aRequest); //visitor pattern
	virtual TAccept AcceptRequestL(CReqBackupRestoreEnd* aRequest); //visitor pattern
	virtual TAccept AcceptRequestL(CReqDiskSpaceLow*     aRequest); //visitor pattern
	virtual TAccept AcceptRequestL(CReqDiskSpaceNormal*  aRequest); //visitor pattern
	virtual TAccept AcceptRequestL(CReqAsyncActivity*	 aRequest); //visitor pattern
	virtual TAccept AcceptRequestL(CReqNoAsyncActivity*	 aRequest); //visitor pattern
	virtual TAccept AcceptRequestL(CReqSetSpeedDial* 	 aRequest); //visitor pattern
	virtual TAccept AcceptRequestL(CReqSetOwnCard* 	 	 aRequest); //visitor pattern 
	virtual TAccept AcceptRequestL(CReqInternalSessionUnlock* aRequest); //visitor pattern
	
	// Implementation of the MContactDbObserver interface
	void HandleDatabaseEventV2L(TContactDbObserverEventV2 aEvent);

protected:
	CState(CCntStateMachine& aStateMachine, CPersistenceLayer&	iPersistenceLayer);

	void TransactionStartLC(TUint aSessionId);
	void TransactionCommitLP();
	virtual void RollbackTransAndRecoverL(const TBool aNotification);

	TAccept DeferRequest(CCntRequest* aRequest);
	TAccept DeferWithTimeOutError(CCntRequest* aRequest);

	// derived state classes use different error codes and implement a derived TimeOutErrorCode()
	virtual TInt TimeOutErrorCode();
	
private:
	static void CleanupTransactionRollback(TAny* aState);
	
protected:
	CCntStateMachine&	iStateMachine;
	CPersistenceLayer&	iPersistenceLayer;
	TUint iCurrentTransactionSessionId;
	};


class CStateClosed : public CState
	{
public:
	static CStateClosed* NewL(CCntStateMachine& aStateMachine, CPersistenceLayer& aPersistenceLayer);
	~CStateClosed();

	virtual TAccept AcceptRequestL(CReqAsyncOpen* 		aRequest);
	virtual TAccept AcceptRequestL(CReqReOpen*    		aRequest);

	// Overridden read-only operations from base class: can't read from the
	// database while in the Closed state so defer these requests.
	virtual TAccept AcceptRequestL(CReqReadCnt*			aRequest); //visitor pattern
	virtual TAccept AcceptRequestL(CReqOpenCnt*			aRequest); //visitor pattern
	virtual TAccept AcceptRequestL(CReqDeleteCnt*		aRequest); //visitor pattern
	virtual TAccept AcceptRequestL(CReqUpdateCnt*    	aRequest); //visitor pattern
	virtual TAccept AcceptRequestL(CReqCommitCnt*    	aRequest); //visitor pattern
	virtual TAccept AcceptRequestL(CReqCreateCnt*		aRequest); //visitor pattern
	virtual TAccept AcceptRequestL(CReqCloseTables*  	aRequest); //visitor pattern	
	virtual TAccept AcceptRequestL(CReqDbBeginTrans*     aRequest); //visitor pattern
	virtual TAccept AcceptRequestL(CReqDbCommitTrans*    aRequest); //visitor pattern
	virtual TAccept AcceptRequestL(CReqDbRollbackTrans*  aRequest); //visitor pattern
	virtual TAccept AcceptRequestL(CReqSetSpeedDial* 	 aRequest); //visitor pattern
	virtual TAccept AcceptRequestL(CReqSetOwnCard* 	 	 aRequest); //visitor pattern 
		
	// Overridden Backup/Restore operations from base class.
	virtual TAccept AcceptRequestL(CReqBackupRestoreBegin* aRequest); //visitor pattern
	
protected:
	CStateClosed(CCntStateMachine& aStateMachine, CPersistenceLayer& aPersistenceLayer);	
	}; 
		


/**
CStateTablesClosed is a special case of the CStateClosed.  It allows requests
that don't use the tables, such as compression.
*/

class CStateTablesClosed : public CStateClosed	
  	{
public:
  	static CStateTablesClosed* NewL(CCntStateMachine& aStateMachine, CPersistenceLayer& aPersistenceLayer);
  	~CStateTablesClosed();
  	
	// Do not allow AsyncOpen requests - return deferred. 
  	// The CStateClosed parent class processes CReqAsyncOpen requests.
  	virtual TAccept AcceptRequestL(CReqAsyncOpen* 		 aRequest); //visitor pattern  	
  	virtual TAccept AcceptRequestL(CReqReOpen*    	 	 aRequest); //visitor pattern
  	
private:
 	CStateTablesClosed(CCntStateMachine& aStateMachine, CPersistenceLayer& aPersistenceLayer);	
  	};	


class CStateBackupRestore : public CStateClosed
	{
public:
	static CStateBackupRestore* NewL(CCntStateMachine& aStateMachine, CPersistenceLayer& aPersistenceLayer);
	~CStateBackupRestore();

	// Do not allow AsyncOpen requests
	virtual TAccept AcceptRequestL(CReqAsyncOpen* 		 aRequest); //visitor pattern

	virtual TAccept AcceptRequestL(CReqBackupRestoreBegin* aRequest); //visitor pattern
	virtual TAccept AcceptRequestL(CReqBackupRestoreEnd* aRequest); //visitor pattern
	virtual TAccept AcceptRequestL(CReqNoAsyncActivity*	 aRequest); //visitor pattern
		
	// Does not use the CStateClosed Timeout error code
	virtual TInt TimeOutErrorCode();

private:
	CStateBackupRestore(CCntStateMachine& aStateMachine, CPersistenceLayer& aPersistenceLayer);
	};


/**
The CStateOpening state is where the opening and possibly recovery and upgrade
of the Contacts database takes place.
*/
class CStateOpening : public CState, public MActiveLoopCallBack 
	{
public:
	static CStateOpening* NewL(CCntStateMachine& aStateMachine, CPersistenceLayer& aPersistenceLayer);
	~CStateOpening();

	virtual TAccept AcceptRequestL(CReqAsyncOpen*   	 aRequest); //visitor pattern
	virtual TAccept AcceptRequestL(CReqCancelAsyncOpen*  aRequest); //visitor pattern
	virtual TAccept AcceptRequestL(CReqReOpen*      	 aRequest); //visitor pattern
	virtual TAccept AcceptRequestL(CReqBackupRestoreEnd* aRequest); //visitor pattern

	// Overridden read-only operations from base class: can't read from the
	// database while in the Opening state so defer these requests.
	virtual TAccept AcceptRequestL(CReqReadCnt*			 aRequest); //visitor pattern
	virtual TAccept AcceptRequestL(CReqOpenCnt*			 aRequest); //visitor pattern
	
	TBool DoStepL();
	void  DoError(TInt aError);

private:
	CStateOpening(CCntStateMachine& aStateMachine, CPersistenceLayer& aPersistenceLayer);
	void 	SetFileNameL (const TDes& aFileName);
	void 	ConstructL  ();
	void 	InitialStep();
	TAccept OpenDatabaseFileL(CCntRequest* aRequest, TBool aNoftify = EFalse);
	void 	DoCompletion(TInt aError);
private:	
	CActiveLoop* 				iActive;
	MLplContactsFile*	 		iCntFile;
	HBufC* 						iFileName;
	RPointerArray<CCntRequest>  iOpenReqsStore;
	// When opening the database a notification event is propogated for some requests
	// however, the Persistence layer knows nothing of requests, so this flag is set for
	// those requests that require notification to be propagated to the client.
	TBool 						iNotify;  
	}; 


/**
The CStateWritable state is the "normal" state for a Contacts database.  In this
state CRUD operations can be carried out.
*/
class CStateWritable : public CState
	{
public:
	static CStateWritable* NewL(CCntStateMachine& aStateMachine, CPersistenceLayer& aPersistenceLayer);
	~CStateWritable();

	// Locking methods.
	virtual TAccept AcceptRequestL(CReqUpdateCnt*    	 aRequest); //visitor pattern
	virtual TAccept AcceptRequestL(CReqCommitCnt*     	 aRequest); //visitor pattern
	virtual TAccept AcceptRequestL(CReqDeleteCnt*    	 aRequest); //visitor pattern
	virtual TAccept AcceptRequestL(CReqCreateCnt*    	 aRequest); //visitor pattern
	virtual TAccept AcceptRequestL(CReqCancelAsyncOpen*  aRequest); //visitor pattern
	virtual TAccept AcceptRequestL(CReqDbBeginTrans* 	 aRequest); //visitor pattern
	virtual TAccept AcceptRequestL(CReqBackupRestoreBegin* aRequest); //visitor pattern
	virtual TAccept AcceptRequestL(CReqSetSpeedDial* 	 aRequest); //visitor pattern
	virtual TAccept AcceptRequestL(CReqSetOwnCard* 	 	 aRequest); //visitor pattern
    virtual TAccept AcceptRequestL(CReqCloseTables* aRequest); //visitor pattern	
	
	virtual TInt TimeOutErrorCode();
	
private:
	CStateWritable(CCntStateMachine& aStateMachine, CPersistenceLayer& aPersistenceLayer);
	}; 


/**
The CStateTransaction state prevents all other sessions performing changes to
the Contacts database.

This state contains a CTransactionTimer which is reset after all requests are
processed when the CStateTransaction is the current active state.  If the
CTransactionTimer timer object times out, the session is deemed to have died
since it has not been in touch, and the transaction is rolled back.
*/
class CStateTransaction : public CState
	{
public:
	static CStateTransaction* NewL(CCntStateMachine& aStateMachine, CPersistenceLayer& aPersistenceLayer);
	~CStateTransaction();

	// Overloaded Visitor Pattern methods
	virtual TAccept AcceptRequestL(CReqDbBeginTrans*     aRequest); //visitor pattern
	virtual TAccept AcceptRequestL(CReqDbCommitTrans*    aRequest); //visitor pattern
	virtual TAccept AcceptRequestL(CReqDbRollbackTrans*  aRequest); //visitor pattern
	virtual TAccept AcceptRequestL(CReqCreateCnt* 		 aRequest); //visitor pattern
	virtual TAccept AcceptRequestL(CReqReadCnt* 		 aRequest); //visitor pattern
	virtual TAccept AcceptRequestL(CReqUpdateCnt* 		 aRequest); //visitor pattern
	virtual TAccept AcceptRequestL(CReqDeleteCnt* 	  	 aRequest); //visitor pattern
	virtual TAccept AcceptRequestL(CReqCommitCnt*    	 aRequest); //visitor pattern
	virtual TAccept AcceptRequestL(CReqOpenCnt*		 	 aRequest); //visitor pattern
	virtual TAccept AcceptRequestL(CReqCloseCnt* 		 aRequest); //visitor pattern

	virtual void CancelTransactionL();

	void HandleDatabaseEventV2L(TContactDbObserverEventV2 aEvent);
private:
	CStateTransaction(CCntStateMachine& aStateMachine, CPersistenceLayer& aPersistenceLayer);
	void ConstructL();
	void StrayRequestL(CCntRequest* aRequest);
	void PropagateDatabaseEventsL();
	TBool CheckInternalCompressL();

protected:
	virtual TInt TimeOutErrorCode();	
	
private:
	// Only one session is allowed to perform a transaction
	TUint 				iSessionId; 
	CTransactionTimer*  iTimeOut;
	// Holds all events until the a commit transaction has been called
	RArray<TContactDbObserverEventV2> iEventQ;
	};


class CTransactionTimer : public CTimer
	{
public:
	static CTransactionTimer* NewL(CStateTransaction& aTransState);
	~CTransactionTimer();
	void Start();
	void Stop ();
	void Reset();

private:
	CTransactionTimer(CStateTransaction& aTransState);
	void RunL();
	void ConstructL();

private:
	CStateTransaction& iTransState;
	};


/**
The transaction lock is NOT directly related to the CStateTransaction state.
The CTransactionLock holds all the IDs of locked contact items - locked by a
session for updating.  No other session can perform a write operation on a
locked contact item.  There is one instance of the transaction lock owned by the
CStateMachine class.
*/ 
class CTransactionLock : public CBase
	{
public:
	static CTransactionLock* NewL(CCntStateMachine& aStateMachine);
	inline ~CTransactionLock(){ iLockedIds.Close();}
	
	TInt  LockLX (const TUint aSessionId, const TContactItemId aCntId);
	TInt  UnLockL (const TUint aSessionId, const TContactItemId aCntId);
	
	// Used with TCleanupItem when a method leaves & cleanup stack pop by the client
	void UnlockLastLockedContactL(TUint aSessionId = nsState::KNoSessionId);
	static void CleanupUnlockRecord(TAny* aTransLock);
		
	// Used for the session close - removes all contact items that remain locked by the session
	void  UnLockAllL(const TUint aSessionId);
	TBool IsLocked  (const TUint aSessionId, const TContactItemId aCntId) const;

	// Overridden IsLocked is used by CStateWritable::AcceptRequestL()
	// which ignore the session id when checking for locked contact ids - original implementation
	TBool IsLocked(const TContactItemId aCntId) const;

	TBool AnyLocked() const;

private:
	inline CTransactionLock(CCntStateMachine& aStateMachine) : iStateMachine(aStateMachine){};
	void ProcessLockedContactsL();

private:
	struct TLockData
		{
		TContactItemId	iCntItemId;
		const TUint 	iSessionId;
		TLockData (TContactItemId aCntId, const TUint aSessionId);
		};
		
	// The iLockIds array works like a stack. On the client side, the lock
	// can be popped via the CleanUpStack, this generates an IPC close request
	// which pops the last LockID 
	RArray<TLockData> iLockedIds;
	CCntStateMachine& iStateMachine;
	};



/**
The CCntStateMachine class contains all states, sets the current state and is
the point of entry for processing requests.  It does none of the request
processing, this is done within the state objects.

It also contains all generic helper objects used by more than one state such as
the request store where all deferred requests (requests that can not be 
processed) by the current active state are queued until the state changed.

Another helper object is the transaction lock which holds the IDds of locked
contact item.  No state can  modify a locked contact item.  The states call into
the state machine to ascertain if a contact item has been locked before
modifying it.

The CCntStateMachine class also implements the MContactDbObserver interface
which propagates events back to the client session.  All events originating in
the Persistence Layer are routed through the State Machine. The reason for this
is that within the Transaction State, the operations which trigger these events 
are only committed via an explicit commit request and therefore these events
should only be propagated to the sessions after the operations have been
committed to the Contacts database.

Finally, each CCntStateMachine instance is owned by an associated CCntDbManager
instance.  This means one State Machine exists for each open Contacts database
file which is shared among many sessions - hence the need for states.
*/
class CCntStateMachine : public CBase, public MContactDbObserverV2
	{
public:
	static	 CCntStateMachine* NewL(CPersistenceLayer& aPersistenceLayer, CCntDbManager& aDbManager);
	virtual ~CCntStateMachine();

	CState&  CurrentState(); 
	void 	 SetCurrentStateL  (CState& aState);
	
	void 	 ProcessRequestL  (CCntRequest* aRequest); 
	inline TBool DatabaseReady() { return iState == &StateWritable(); };
	
	// Implementation of the MContactDbObserverV2 interface
	void HandleDatabaseEventV2L(TContactDbObserverEventV2 aEvent);
	
	CTransactionLock& TransactionLockL();
		
	CCntDbManager& DbManager();
	CRequestStore& ReqStoreL();

	inline void SetLowDisk(TBool aLowDisk) { iLowDisk = aLowDisk; };
	inline TBool LowDisk() { return iLowDisk; };

	inline void SetAsyncActivity(TBool aAsyncActivity) { iAsyncActivity = aAsyncActivity; };
	inline TBool AsyncActivity() { return iAsyncActivity; };

	inline CStateClosed& 		StateClosed()		{ return static_cast<CStateClosed&> 	  (*iStateArray[nsState::EStateClosed]);};
    inline CStateTablesClosed&  StateTablesClosed() { return static_cast<CStateTablesClosed&> (*iStateArray[nsState::EStateTablesClosed]);};  
	inline CStateWritable&      StateWritable()  	{ return static_cast<CStateWritable&> 	  (*iStateArray[nsState::EStateWritable]);};
	inline CStateOpening& 		StateOpening()		{ return static_cast<CStateOpening&> 	  (*iStateArray[nsState::EStateOpening]);};
	inline CStateTransaction&	StateTransaction()	{ return static_cast<CStateTransaction&>  (*iStateArray[nsState::EStateTransaction]);};
	inline CStateBackupRestore&	StateBackupRestore(){ return static_cast<CStateBackupRestore&>(*iStateArray[nsState::EStateBackupRestore]);};

private:
	CCntStateMachine(CCntDbManager& aDbManager);
	void ConstructL(CPersistenceLayer& aPersistenceLayer);

#ifdef __STATE_MACHINE_DEBUG__
	const TDesC& StateName(CState& aState);
#endif
	
private:
	CState* 				iState;
	RPointerArray <CState>	iStateArray;
	CRequestStore* 			iReqStore;
	CTransactionLock* 		iTransLock;
	CCntDbManager&			iDbManager;
	TBool					iLowDisk;
	TBool					iAsyncActivity;
	};


#endif
