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
 @internalTechnology
 @released 
*/

#include "ccntstatemachine.h"
#include "ccntrequest.h"
#include "ccntdbmanager.h"
#include "ccntbackuprestoreagent.h"
#include "persistencelayer.h"
#include "cntspeeddials.h"
#include <cntfldst.h> 		// for ccontacttextfield
#include <cntfield.h> 		// for ccontacttextfield
#include "cntstd.h"   		// for panic codes

// Event related headers
#include <cntdbobs.h> 	    // for ccontactdbobserver
#include "ccntserver.h"     // for kcntnullconnectionid.
#include "ccnteventqueue.h" // for kmaxnumberofeventsineventqueue, kcnteventgranularity
#include "ccntlogger.h"  

// Require SQL header for leave code checking
#include <sqldb.h>
	
/** 
CState - Base state destructor, default implementataion
*/
CState::~CState() 
	{}

/**
 CState constructor. 
 The main purpose of the CState class is to define the state transition table.
 The CState class implements the most common state behaviour for each request object, 
 in the overridden AcceptRequestL method.
 Note there is no NewL method on the CState class. It is not meant to be instantiated.
 
 @param aStateMachine The statemachine object that is used for state transitions
 @param aPersistenceLayer The persistence layer that allows provides database access
 
*/	
CState::CState(CCntStateMachine& aStateMachine, CPersistenceLayer&	aPersistenceLayer)
:iStateMachine(aStateMachine), iPersistenceLayer(aPersistenceLayer)
	{}

/**
 TransactionStartLC is called from many derived states. The session that started the
 transaction is remembered in order to only allow that session perform database operations
 during the transaction. 
 Transaction rollback is pushed onto the clean up stack in case of a leave should occur before the
 transaction is committed. If the transaction can not be committed in full, then none of
 the transaction should be committed.
 
 @param aSessionId The unique ID of session that is moving the state machine into a transaction state.
*/
void CState::TransactionStartLC(TUint aSessionId)
	{
	iCurrentTransactionSessionId = aSessionId;
	iPersistenceLayer.TransactionManager().StartTransactionL();
	CleanupStack::PushL(TCleanupItem(CState::CleanupTransactionRollback, this));
	}

/** 
 Rollback a transaction after an leave has occured.
 None of the transaction is committed to the database
 
 @param aState The state in which the leave occured
*/
void CState::CleanupTransactionRollback(TAny* aState)
  	{
  	TRAP_IGNORE(static_cast<CState*>(aState)->RollbackTransAndRecoverL(EFalse));
  	}

/**
 Commit the current transaction.
*/
void CState::TransactionCommitLP()
	{
	iPersistenceLayer.TransactionManager().CommitCurrentTransactionL(iCurrentTransactionSessionId);
	iCurrentTransactionSessionId = 0;
	CleanupStack::Pop(); // CleanupTransactionRollback
	}

/**
 Clean up the transaction after a leave occurs. 
*/
void CState::RollbackTransAndRecoverL(const TBool aNotification)
	{
  	// Some operation has left before a commit could be called.
  	iPersistenceLayer.TransactionManager().RollbackCurrentTransactionL(iCurrentTransactionSessionId);
	iCurrentTransactionSessionId = 0;
	
	iPersistenceLayer.ContactsFileL().CloseTablesL(!aNotification);
	iPersistenceLayer.ContactsFileL().OpenTablesL(!aNotification);
	iStateMachine.SetCurrentStateL(iStateMachine.StateWritable());			
	}

/* Note: The following methods implement default 
   AcceptRequestL behaviour for all states derived 
   from Parent Class CState
*/

/* 
 Default behaviour: The file has already been opened 
 so complete KErrNone.
 
 @param aRequest Open database request object
 @return TAccept EProcessed - finished processing request
*/ 
TAccept CState::AcceptRequestL(CReqAsyncOpen* aRequest)
	{
	aRequest->Complete();
	return EProcessed;
	}

/**
 Default behaviour is to defer the request

 @param aRequest Update contact item request object
 @return EDeferred if the request is not processed, EProcessed if the request is
         completed with timeout error.
 @see CState::DeferRequest 
*/
TAccept CState::AcceptRequestL(CReqUpdateCnt* aRequest)
	{
	return DeferRequest(aRequest);
	}

/**
 Default behaviour is to defer the request

 @param aRequest Commit contact item request object
 @return EDeferred if the request is not processed, EProcessed if the request is
         completed with timeout error.
 @see CState::DeferRequest 
*/
TAccept CState::AcceptRequestL(CReqCommitCnt* aRequest)
	{
	return DeferRequest(aRequest);
	}
	

/** 
 Default behaviour is to allow read access on the database	
 Open is the same as read but with locking - does not change the database	

 @param aRequest Open contact item request object
 @return EProcessed if the request is completed or one of the values returned by 
         CState::DeferRequest if the database is currently locked.
 @see CState::DeferRequest 
*/
TAccept CState::AcceptRequestL(CReqOpenCnt* aRequest)
	{
	if (iStateMachine.TransactionLockL().LockLX(aRequest->SessionId(), aRequest->CntItemId()) == KErrInUse)
		{
		// The contact item has been locked by another session
		aRequest->SetTimeOutError(KErrInUse);
		return DeferRequest(aRequest);
		}

	CContactItem* cntItem = iPersistenceLayer.PersistenceBroker().ReadLC(aRequest->CntItemId(), aRequest->ItemViewDef(), EPlAllInfo, aRequest->SessionId(), ETrue);
	aRequest->CompleteL(*cntItem);
	CleanupStack::PopAndDestroy(cntItem); 
	CleanupStack::Pop(); // we do not destroy it since that would trigger the leave mechanism and unlock the record
	return EProcessed;
	}

/**
 Default behaviour is to allow read access on the database.
 Read Contact does not change the database
 
 @param aRequest Read contact item request object
 @return TAccept EProcessed - finished processing request
*/ 
TAccept CState::AcceptRequestL(CReqReadCnt* aRequest)
	{
	CContactItem* cntItem = iPersistenceLayer.PersistenceBroker().ReadLC(aRequest->CntItemId(), aRequest->ItemViewDef(), EPlAllInfo, aRequest->SessionId());
	aRequest->CompleteL(*cntItem);
	CleanupStack::PopAndDestroy(cntItem);
	return EProcessed;		
	}		

/**
 Default behaviour is to defer the request

 @param aRequest Delete contact item request object
 @return EDeferred if the request is not processed, EProcessed if the request is
         completed with timeout error.
 @see CState::DeferRequest 
*/
TAccept CState::AcceptRequestL(CReqDeleteCnt* aRequest)
	{
	return DeferRequest(aRequest);	
	}
	
/**
 Default behaviour is to close the contact item by removing the locking 
 The lock was added during from an Open Contact Request
 If the contact id is supplied, explicity unlock that contact item, otherwise
 unlock the last contact item locked by the session

 @param aRequest Close contact item request object
 @return TAccept EProcessed - finished processing request
*/  
TAccept CState::AcceptRequestL(CReqCloseCnt* aRequest)
	{
	if (aRequest->CntItemId() > KNullContactId)
		{
		aRequest->Complete(iStateMachine.TransactionLockL().UnLockL(aRequest->SessionId(), aRequest->CntItemId()));	
		}
	else
		{
		iStateMachine.TransactionLockL().UnlockLastLockedContactL(aRequest->SessionId());
		aRequest->Complete(KErrNone);
		}
	return EProcessed;		
	}

/**
 Default behaviour is to always allow a session to unlock any contact 
 items that remain locked for that session before the session is closed

 @param aRequest Unlock all contact items request object  
 @return TAccept EProcessed - finished processing request
*/  
TAccept CState::AcceptRequestL(CReqInternalSessionUnlock* aRequest)
	{
	iStateMachine.TransactionLockL().UnLockAllL(aRequest->SessionId());
	aRequest->Complete(KErrNone);
	return EProcessed;		
	}

/**
 Default behaviour is to defer the request

 @param Create contact item request object
 @return EDeferred if the request is not processed, EProcessed if the request is
         completed with timeout error.
 @see CState::DeferRequest 
*/
TAccept CState::AcceptRequestL(CReqCreateCnt* aRequest)
	{
	return DeferRequest(aRequest);	
	}

/**
 The default behaviour for Cancelling an Asyncronous Open database command
 is to accept the file has been opened.
 
 @param aRequest Cancel database open request object 
 @return TAccept EProcessed - finished processing request
*/ 
TAccept CState::AcceptRequestL(CReqCancelAsyncOpen* aRequest)
	{
	aRequest->Complete();
	return EProcessed;	
	}


/**
 Default behaviour is to defer the request
   
 @param aRequest Close contact database tables request object
 @return EDeferred if the request is not processed, EProcessed if the request is
         completed with timeout error.
 @see CState::DeferRequest 
*/
TAccept CState::AcceptRequestL(CReqCloseTables* aRequest)
  	{
 	return DeferRequest(aRequest);	
  	}

/**
 Default behaviour is to defer the request

 @param aRequest ReOpen contact database tables request object
 @return EDeferred if the request is not processed, EProcessed if the request is
         completed with timeout error.
 @see CState::DeferRequest 
*/
TAccept CState::AcceptRequestL(CReqReOpen* aRequest)
	{
	return DeferRequest(aRequest);	
	}

/**
 Default behaviour is to defer the request

 @param aRequest Begin transaction request object
 @return EDeferred if the request is not processed, EProcessed if the request is
         completed with timeout error.
 @see CState::DeferRequest 
*/
TAccept CState::AcceptRequestL(CReqDbBeginTrans* aRequest)
	{
	return DeferRequest(aRequest);	
	}

/**
 Default behaviour is to defer the request

 @param aRequest Commit transaction request object
 @return EDeferred if the request is not processed, EProcessed if the request is
         completed with timeout error.
 @see CState::DeferRequest 
*/
TAccept CState::AcceptRequestL(CReqDbCommitTrans* aRequest)
	{
	return DeferRequest(aRequest);	
	}

/**
 Default behaviour is to defer the request

 @param aRequest Rollback transaction request object
 @return EDeferred if the request is not processed, EProcessed if the request is
         completed with timeout error.
 @see CState::DeferRequest 
 */
TAccept CState::AcceptRequestL(CReqDbRollbackTrans* aRequest)
	{
	return DeferRequest(aRequest);	
	}

/**
 Default behaviour is to defer the request

 @param aRequest Notification of database backup/restore request object
 @return EDeferred if the request is not processed, EProcessed if the request is
         completed with timeout error.
 @see CState::DeferRequest 
*/
TAccept CState::AcceptRequestL(CReqBackupRestoreBegin* aRequest)
	{
	return DeferRequest(aRequest);	
	}

/**
 Default behaviour is to complete the request

 @param aRequest Notification that a backup/restore has finished request object
 @return TAccept EProcessed - finished processing request
*/
TAccept CState::AcceptRequestL(CReqBackupRestoreEnd* aRequest)
   	{
  	// In most cases no backup/restore will be in progress so by default
  	// complete this request.
  	aRequest->Complete();
  	return EProcessed;
  	}
  
/**
 Default behaviour is set an internal low disk flag to true
 
 @param aRequest Notification of low disk space request object
 @return TAccept EProcessed - finished processing request
*/  
TAccept CState::AcceptRequestL(CReqDiskSpaceLow* aRequest)
  	{
  	iStateMachine.SetLowDisk(ETrue);
  	aRequest->Complete();
  	return EProcessed;
  	}

/**
 Default behaviour is to set an internal low disk flag to false

 @param aRequest Notification disk space is normal request object
 @return TAccept EProcessed - finished processing request
*/
TAccept CState::AcceptRequestL(CReqDiskSpaceNormal* aRequest)
  	{
  	iStateMachine.SetLowDisk(EFalse);
  	aRequest->Complete();
  	return EProcessed;
  	}

/**
 Default behaviour is set an internal async activity flag to true
 
 @param aRequest Notification of async activity request object
 @return TAccept EProcessed - finished processing request
*/  
TAccept CState::AcceptRequestL(CReqAsyncActivity* aRequest)
  	{
  	iStateMachine.SetAsyncActivity(ETrue);
  	aRequest->Complete();
  	return EProcessed;
  	}

/**
 Default behaviour is to set an internal async activity flag to false

 @param aRequest Notification of no async activity request object
 @return TAccept EProcessed - finished processing request
*/
TAccept CState::AcceptRequestL(CReqNoAsyncActivity* aRequest)
  	{
  	iStateMachine.SetAsyncActivity(EFalse);
  	aRequest->Complete();
  	return EProcessed;
  	}

/**
 Default behaviour is to defer the request

 @param aRequest Speed dial request object
 @return EDeferred if the request is not processed, EProcessed if the request is
         completed with timeout error.
 @see CState::DeferRequest 
*/
TAccept CState::AcceptRequestL(CReqSetSpeedDial* aRequest)
	{
	return DeferRequest(aRequest);	
	}

/**
 Default behaviour is to defer the request

 @param aRequest Own card request object
 @return EDeferred if the request is not processed, EProcessed if the request is
         completed with timeout error.
 @see CState::DeferRequest 
*/
TAccept CState::AcceptRequestL(CReqSetOwnCard* aRequest)
	{
	return DeferRequest(aRequest);	
	}


/**
 Don't do anything with the event here. Simply propagate the event to the dbmanager
 Any state that wishes to handle the event implements it's own overwritten 
 HandleDatabaseEventV2L method. The only state
 that actually implements this is the Transaction State.
 
 @param aEvent Database event generated in the Persistence Layer
*/ 
void CState::HandleDatabaseEventV2L(TContactDbObserverEventV2 aEvent)
	{
	iStateMachine.DbManager().HandleDatabaseEventV2L(aEvent);
	}

/**
 Sets a timeout error code on the request. This error code is retrieved from the 
 derived state class via a call to TimeOutErrorCode() 
 if the derived state class does not implement a overridden TimeOutErrorCode(), 
 the CState::TimeOutErrorCode() is used.
 
 Calls CState::DeferRequest to determine if the request should be deferred or not
 depending on the request's timer status.

 @param aRequest The request on which the timeout error is set.
 @return EDeferred if the request is not processed, EProcessed if the request is
         completed with timeout error.
 @see CState::DeferRequest 
*/
TAccept CState::DeferWithTimeOutError(CCntRequest* aRequest)
	{
	// use the time out error code specified with the current state
	aRequest->SetTimeOutError(TimeOutErrorCode());

    return DeferRequest(aRequest);
	}

/**
 Determines if an un-processed request should be completed with timeout error or if it 
 should be re-tried again later, depending on the current status of the request's timer.  

 @param aRequest The request failed to be processed by the current state
 @return EDeferred - if the request's timer has not expired, the request can be processed again at the 
                     next opportunity.
         EProcessed - if the request's timer has expired and is completed with timeout 
                      error.
*/	
TAccept CState::DeferRequest(CCntRequest* aRequest)
    {
	// request still cannot be processed, check if the timer on the request
	// has expired yet
	if (aRequest->TimeOut() <= 0)
        {
        // timer expired, request should be completed with timeout error
    	aRequest->Complete(aRequest->TimeOutError());
    	return EProcessed;
        }

    // timer still valid, as request cannot be processed now, signal to re-try again later
    return EDeferred;	
    }

/** 
 Returns the default timeout - KErrNotReady
*/
TInt CState::TimeOutErrorCode()  	
	{
	return KErrNotReady;
	}


/** 
 CStateClosed Class NewL factory constructor
 @see CState constructor
*/
CStateClosed* CStateClosed::NewL(CCntStateMachine& aStateMachine, CPersistenceLayer& aPersistenceLayer)
	{
	CStateClosed* stateClosed = new (ELeave) CStateClosed(aStateMachine, aPersistenceLayer);
	return stateClosed;
	}

/** 
 CStateClosed Class constructor
 @see CState constructor
*/
CStateClosed::CStateClosed(CCntStateMachine& aStateMachine, CPersistenceLayer& aPersistenceLayer)
:CState(aStateMachine, aPersistenceLayer)
	{}

/** 
 CStateClosed Class destructor
 @see CState destructor
*/
CStateClosed::~CStateClosed() {}


/**
 Process an open request in the closed state.
 Note on the State design pattern:
 Unlike the common state design pattern, most of the work is done in
 the opening state and not in the closed state before the state transition.
 The reason for this is re-use of code. 
 
 @param aRequest Open database request object
 @return TAccept EProcessed if finished processing request
 		 	     EDeferred if the request was not processed 
*/		
TAccept CStateClosed::AcceptRequestL(CReqAsyncOpen* aRequest)
	{
	iStateMachine.SetCurrentStateL(iStateMachine.StateOpening());
	return iStateMachine.CurrentState().AcceptRequestL(aRequest);
	}

/**
 We can only process re-open tables requests in the CStateTablesClosed
 which means that this request must be preceeded by both a CReqAsyncOpen &
 a CReqCloseTables. The correct behaviour is to defer the request with
 the default CStateClosed timeout error.
 
 @param aRequest ReOpen database tables request object 
 @return EDeferred if the request is not processed, EProcessed if the request is
         completed with timeout error.
 @see CState::DeferWithTimeOutError
*/

TAccept CStateClosed::AcceptRequestL(CReqReOpen* aRequest)
	{
	return DeferWithTimeOutError(aRequest);
	}


/** 
 Overridden read-only operations from base class: can't read from the database
 while in the Closed state so defer these requests.
 
 @param aRequest Read contact item request object  
 @return EDeferred if the request is not processed, EProcessed if the request is
         completed with timeout error.
 @see CState::DeferWithTimeOutError
*/
TAccept CStateClosed::AcceptRequestL(CReqReadCnt* aRequest)
	{
	return DeferWithTimeOutError(aRequest);
	}

/**
 Defer Update requests with the default error for CStateClosed.
 Note: This default error code is taken from the original
 contacts model.
 
 @param aRequest Update contact item request object  
 @return EDeferred if the request is not processed, EProcessed if the request is
         completed with timeout error.
 @see CState::DeferWithTimeOutError
*/ 
TAccept CStateClosed::AcceptRequestL(CReqUpdateCnt* aRequest)
	{
	return DeferWithTimeOutError(aRequest);
	}

/** 
 Defer Commit requests
 The default error for CStateClosed has been taken from the original 
 contacts model

 @param aRequest Commit contact item request object  
 @return EDeferred if the request is not processed, EProcessed if the request is
         completed with timeout error.
 @see CState::DeferWithTimeOutError
*/
TAccept CStateClosed::AcceptRequestL(CReqCommitCnt* aRequest)
	{
	return DeferWithTimeOutError(aRequest);
	}

/** 
 Defer Delete requests
 The default error for CStateClosed has been taken from the original 
 contacts model

 @param aRequest Delete contact item request object  
 @return EDeferred if the request is not processed, EProcessed if the request is
         completed with timeout error.
 @see CState::DeferWithTimeOutError
*/
TAccept CStateClosed::AcceptRequestL(CReqDeleteCnt* aRequest)
	{
	return DeferWithTimeOutError(aRequest);
	}

/** 
 Defer Create requests
 The default error for CStateClosed has been taken from the original 
 contacts model

 @param aRequest Create contact item request object  
 @return EDeferred if the request is not processed, EProcessed if the request is
         completed with timeout error.
 @see CState::DeferWithTimeOutError
*/
TAccept CStateClosed::AcceptRequestL(CReqCreateCnt* aRequest)
	{
	return DeferWithTimeOutError(aRequest);
	}

/** 
 Defer Open requests
 The default error for CStateClosed has been taken from the original 
 contacts model
 The default behaviour of the parent class CStateis to process an open request, the Closed State	
 defers this request

 @param aRequest Commit contact item request object  
 @return EDeferred if the request is not processed, EProcessed if the request is
         completed with timeout error.
 @see CState::DeferWithTimeOutError
*/
TAccept CStateClosed::AcceptRequestL(CReqOpenCnt* aRequest)
	{
	return DeferWithTimeOutError(aRequest);
	}

/**
 Defer the Close tables request
 The tables are closed but so is the file. 
 To close the tables the file must be open
 	
 @param aRequest Close database tables request object  
 @return EDeferred if the request is not processed, EProcessed if the request is
         completed with timeout error.
 @see CState::DeferWithTimeOutError
*/
TAccept CStateClosed::AcceptRequestL(CReqCloseTables* aRequest)
  	{
  	return DeferWithTimeOutError(aRequest);
  	}
  
/**
 Defer begin transaction requests  
 The default behaviour of the parent class CState is to process a begin transaction request, 
 the Closed State defers this request

 @param aRequest Begin transaction request object  
 @return EDeferred if the request is not processed, EProcessed if the request is
         completed with timeout error.
 @see CState::DeferWithTimeOutError
*/
TAccept CStateClosed::AcceptRequestL(CReqDbBeginTrans* aRequest)
	{
	return DeferWithTimeOutError(aRequest);
	}

/**
 Defer commit transaction requests  
 The default behaviour of the parent class CState is to process a commit transaction request, 
 the Closed State defers this request

 @param aRequest Commit transaction request object  
 @return EDeferred if the request is not processed, EProcessed if the request is
         completed with timeout error.
 @see CState::DeferWithTimeOutError
*/
TAccept CStateClosed::AcceptRequestL(CReqDbCommitTrans* aRequest)
	{
	return DeferWithTimeOutError(aRequest);
	}

/**
 Defer rollback transaction requests  
 The default behaviour of the parent class CState is to process a rollback transaction request, 
 the Closed State defers this request

 @param aRequest Rollback transaction request object  
 @return EDeferred if the request is not processed, EProcessed if the request is
         completed with timeout error.
 @see CState::DeferWithTimeOutError
*/
TAccept CStateClosed::AcceptRequestL(CReqDbRollbackTrans*  aRequest)
	{
	return DeferWithTimeOutError(aRequest);
	}

/**
 Defer set speed dial requests  
 The default behaviour of the parent class CState is to process a set speed dial request, 
 the Closed State defers this request

 @param aRequest Set speed dial request object  
 @return EDeferred if the request is not processed, EProcessed if the request is
         completed with timeout error.
 @see CState::DeferWithTimeOutError
*/
TAccept CStateClosed::AcceptRequestL(CReqSetSpeedDial* aRequest)
	{
	return DeferWithTimeOutError(aRequest);
	}

/**
 Defer set own card requests  
 The default behaviour of the parent class CState is to process a set own card request, 
 the Closed State defers this request

 @param aRequest Set own card request object  
 @return EDeferred if the request is not processed, EProcessed if the request is
         completed with timeout error.
 @see CState::DeferWithTimeOutError
*/	
TAccept CStateClosed::AcceptRequestL(CReqSetOwnCard* aRequest)
	{
	return DeferWithTimeOutError(aRequest);
	}


/**
 Backup/restore begin notification requests  

 @param aRequest backup/restore begin notification request object  
 @return TAccept EProcessed
*/	
TAccept CStateClosed::AcceptRequestL(CReqBackupRestoreBegin* aRequest)
  	{
  	// Backup/restore can take place from this state without doing anything so
  	// simply complete request.
  	aRequest->Complete();
  	return EProcessed;
  	}


/** 
 CStateTablesClosed Class NewL factory constructor
 @see CState constructor
*/  	
CStateTablesClosed* CStateTablesClosed::NewL(CCntStateMachine& aStateMachine, CPersistenceLayer& aPersistenceLayer)
  	{
  	CStateTablesClosed* stateTablesClosed = new (ELeave) CStateTablesClosed(aStateMachine, aPersistenceLayer);
  	return stateTablesClosed;
  	}
  
/** 
 CStateTablesClosed Class destructor
 @see CState Destructor
*/  	
CStateTablesClosed::~CStateTablesClosed()
  	{
  	}
  
/** 
 CStateTablesClosed Class constructor
 @see CState constructor
*/  	
CStateTablesClosed::CStateTablesClosed(CCntStateMachine& aStateMachine, CPersistenceLayer& aPersistenceLayer) 
  :CStateClosed(aStateMachine, aPersistenceLayer)
  	{
  	}
  
  
/**
 Accept re-open tables requests  
 The tables are closed. A re-open request will result in re-opening the tables.
   
  @param aRequest re-open tables request object  
  @return TAccept EProcessed - the request was processed 
*/
TAccept CStateTablesClosed::AcceptRequestL(CReqReOpen* aRequest)
  	{
  	iPersistenceLayer.ContactsFileL().OpenTablesL(ETrue);
  	// We can only ever come into this state from the writable state
  	// as the database must have been opened
  	iStateMachine.SetCurrentStateL(iStateMachine.StateWritable());
  	aRequest->Complete();
  	return EProcessed;		
  	}
  
/**
  Accept Open database requests  
  The tables are closed. An async open request will result in re-opening the
  tables.
    
  @param aRequest async open request object  
  @return TAccept EProcessed if finished processing request
*/	
TAccept CStateTablesClosed::AcceptRequestL(CReqAsyncOpen* aRequest)
  	{	
  	iPersistenceLayer.ContactsFileL().OpenTablesL(ETrue);
  	// We can only ever come into this state from the writable state
  	// as the database must have been opened
  	iStateMachine.SetCurrentStateL(iStateMachine.StateWritable());
  	aRequest->Complete();
  	return EProcessed;
  	}

/** 
 CStateOpening Class NewL factory constructor
 @see CState constructor
*/ 
CStateOpening* CStateOpening::NewL(CCntStateMachine& aStateMachine, CPersistenceLayer& aPersistenceLayer)
	{
	CStateOpening* stateOpening = new (ELeave) CStateOpening(aStateMachine, aPersistenceLayer);
	CleanupStack::PushL(stateOpening);
	stateOpening->ConstructL();
	CleanupStack::Pop(stateOpening);
	return stateOpening;
	}

/** 
 CStateOpening ConstructL, gets and holds an instance of the persisitnce layer contact file interface
 and creates and ActiveLoop object which facilitates long running operations.  
 @see CState constructor
*/
void CStateOpening::ConstructL()
	{
	iCntFile = &(iPersistenceLayer.ContactsFileL());
	iActive = CActiveLoop::NewL();
	}
/** 
 CStateOpening constructor
 @see CState constructor
*/
CStateOpening::CStateOpening(CCntStateMachine& aStateMachine, CPersistenceLayer& aPersistenceLayer)
:CState(aStateMachine, aPersistenceLayer)
	{
	}

/** 
 CStateOpening Class destructor
 @see CState destructor
*/	
CStateOpening::~CStateOpening()
	{
	delete iActive;
	delete iFileName;
	iOpenReqsStore.ResetAndDestroy();
	}

/**
 Accept Open database requests  
 Opens the database file and tables.
  
 @param aRequest Open database request object  
 @return EOwnershipPassed 
 @see CStateOpening::OpenDatabaseFileL
*/
TAccept CStateOpening::AcceptRequestL(CReqAsyncOpen* aRequest)
	{
	SetFileNameL(aRequest->FileName()); 
	return OpenDatabaseFileL(aRequest);	
	}

/**
 Private member function that starts the database opening process.  
 This method may be called from several AcceptRequestL methods.
 
 @param aRequest Open database request object  
 @return EOwnershipPassed to indicate ownership of the open database request has 
         been transferred
*/
TAccept CStateOpening::OpenDatabaseFileL(CCntRequest* aRequest, TBool aNotify)
	{
	// Add this request to the store where it will be completed on completion of 
	// the open operaion
	//
	// Note that after the request is successfully transferred to the Request 
	// Store, no leave can occur until the caller is notified that ownership
	// of the request has been transferred.
	iOpenReqsStore.AppendL(aRequest);

	iNotify = aNotify;

	// If we are not already processing an open request, start opening the file.
	if (!iActive->IsActive())
		{
		InitialStep();
		}

	return EOwnershipPassed;
	}

/**
 Accept the Reopen tables request as the tables are being opened anyway

 @param aRequest Open database request object  
 @return EOwnershipPassed 
 @see CStateOpening::OpenDatabaseFileL
*/
TAccept CStateOpening::AcceptRequestL(CReqReOpen* aRequest)
	{
	// Open the database and notify (ETrue) all session of the recovery 
	return OpenDatabaseFileL(aRequest, ETrue);	
	}

/** 
 Explicit cancel of an asyncronous Open Database request

 @param aRequest Open database request object  
 @return TAccept EProcessed 
*/
TAccept CStateOpening::AcceptRequestL(CReqCancelAsyncOpen* aRequest)
	{
	TInt max = iOpenReqsStore.Count();
	for (TInt ii = 0; ii < max; ++ii)
		{
		if (aRequest->SessionId() == iOpenReqsStore[ii]->SessionId())
			{
			// If we only have one concurrent open request, cancel the open
			// operation
			if (max == 1)
				{
				iCntFile->Close();	
				iActive->Cancel();
				iStateMachine.SetCurrentStateL(iStateMachine.StateClosed());
				}
			
			iOpenReqsStore[ii]->Complete(KErrCancel);
			aRequest->Complete();
			delete iOpenReqsStore[ii];
			iOpenReqsStore.Remove(ii);
			return EProcessed;			
			}
		}
	aRequest->Complete(KErrNotFound);
	return EProcessed;		
	}

/**
 Start Opening the database using the CActiveLoop class. Opening a file
 is a long running operation and is done in steps allowing other active objects
 processor time between each opening step.
*/
void CStateOpening::InitialStep()
	{
	iActive->Register(*this, ETrue);
	}

/** 
Perform the next step in the opening operation 
@see InitialStep for more details
*/
TBool CStateOpening::DoStepL()
	{
	if (!iFileName)
		{
		User::Leave(KErrGeneral);
		}

#ifdef __VERBOSE_DEBUG__
	RDebug::Print(_L("[CNTMODEL] ------------- Open Database Step -------------"));
#endif

#if defined(__PROFILE_DEBUG__)
	RDebug::Print(_L("[CNTMODEL] MTD:CStateOpening::DoStepL"));
#endif 
		
    iCntFile->OpenL(*iFileName);		
    // always go to this code if using SQLite
	#ifdef __VERBOSE_DEBUG__
  		RDebug::Print(_L("[CNTMODEL] ------------- Database Opened -------------"));
	#endif
		// The file is now open.
		DoCompletion(KErrNone);	
		iStateMachine.SetCurrentStateL(iStateMachine.StateWritable());
		// Check if Backup/Restore Agent indicates backup in progress (i.e. we
		// are opening database after backup starts).
		if (iStateMachine.DbManager().BackupRestoreAgent().BackupInProgress())
			{
			TContactDbObserverEventV2 event;
			event.iType = EContactDbObserverEventBackupBeginning;
			event.iContactId = 0;
			event.iConnectionId = 0;
	        event.iTypeV2 = EContactDbObserverEventV2Null;
	        event.iAdditionalContactId = 0;
			// The HandleBackupRestoreEventL() method of the CCntDbManager that
			// owns this state machine is called to send the appropriate request
			// to the state machine and to notify any observers.  The request
			// causes a transition from CStateWritable to CStateBackupRestore.
			// We need to send this event here since the event has only been
			// heard by CCntDbManager instances (and any of its observers) that
			// existed at the time when the backup first started.
			iStateMachine.DbManager().HandleBackupRestoreEventL(event);
			}
		return EFalse;
	}
	
/** 
 Complete all Opening requests. More than one client (or internal 
 request) may open a database file simultaneously. 
 DoCompletion completes all outstanding Open requests.
 
 @param aError The error code used to complete the request.
 */
void CStateOpening::DoCompletion(TInt aError)
	{
	TInt max = iOpenReqsStore.Count();
	for (TInt ii = 0; ii < max; ++ii)
		{
		iOpenReqsStore[ii]->Complete(aError);
		}
	iOpenReqsStore.ResetAndDestroy();
	}

/**
 Completes all open requests with an error code	

 @param aError The error code used to complete the request. 
*/ 
void CStateOpening::DoError(TInt aError)
	{
	
	DEBUG_PRINT2(__VERBOSE_DEBUG__,_L("[CNTMODEL] ------------- Database Open Error %d -------------"), aError);

	DoCompletion(aError);
	TRAP_IGNORE(iStateMachine.SetCurrentStateL(iStateMachine.StateClosed() ) );
	}

/**
 Sets the name of the file that is being opened. The name is held by
 the opeing state and re-used when database needs to be closed and 
 re-opened (for example during recovery)
 
 @param aFileName The name of the file that is being opened
*/ 
void CStateOpening::SetFileNameL(const TDes& aFileName)
	{
	HBufC* tmpFileName  = HBufC::NewL(aFileName.Length());
	*tmpFileName = aFileName;
	delete iFileName;
	iFileName = tmpFileName;
	}

/**
 Once a Backup or Restore has completed we can re-open the database.
 
 @param aRequest Notification of Backup/Restore end request object  
 @return EOwnershipPassed 
 @see CStateOpening::OpenDatabaseFileL
*/ 
TAccept CStateOpening::AcceptRequestL(CReqBackupRestoreEnd* aRequest) 
	{
	return OpenDatabaseFileL(aRequest);
	}

/** 
 Overridden read-only operations from base class: can't read from the database
 while in the Opening state so defer these requests.
 
 @param aRequest Read contact item request object  
 @return EDeferred if the request is not processed, EProcessed if the request is
         completed with timeout error.
 @see CState::DeferRequest 
*/
TAccept CStateOpening::AcceptRequestL(CReqReadCnt* aRequest)
	{
	return DeferRequest(aRequest); // Uses the requests default timeout error
	}

// @see CStateOpening::AcceptRequestL(CReqReadCnt* )	
TAccept CStateOpening::AcceptRequestL(CReqOpenCnt* aRequest)
	{
	return DeferRequest(aRequest);
	}



///////////////////// CStateWritable Class implementation //////////////////////

/** 
 CStateWritable Class NewL factory constructor
 @see CState constructor
*/ 
CStateWritable* CStateWritable::NewL(CCntStateMachine& aStateMachine, CPersistenceLayer& aPersistenceLayer)
	{
	CStateWritable* stateWritable = new (ELeave) CStateWritable(aStateMachine, aPersistenceLayer);
	return stateWritable;
	}

/** 
 CStateWritable Class constructor
 @see CState constructor
*/ 
CStateWritable::CStateWritable(CCntStateMachine& aStateMachine, CPersistenceLayer& aPersistenceLayer)
:CState(aStateMachine, aPersistenceLayer)
	{
	}

/** 
 CStateWritable Class destructor
 @see CState destructor
*/ 
CStateWritable::~CStateWritable()
	{
	}


// Derived AcceptRequestL methods - Vistor Pattern
	
/**
 Update a contact item in the database

 @param aRequest Update contact item request object  
 @return TAccept EProcessed if finished processing request or one of the 
         values returned by DeferWithTimeOutError
 @see CState::DeferWithTimeOutError 		 	     
*/ 	
TAccept CStateWritable::AcceptRequestL(CReqUpdateCnt* aRequest)
	{
  	if (iStateMachine.LowDisk())
  		{
  		aRequest->Complete(KErrDiskFull);
  		return EProcessed;
  		}
  	// Check if the contact has been locked 
  	if (iStateMachine.TransactionLockL().IsLocked(aRequest->Item().Id()))
  		{
  		// If the request can not be procesed after the timeout period, it should 
  		// complete with KErrInUse to assure binary compatibility with the original model
		return DeferWithTimeOutError(aRequest);
  		}
  
  	TRAPD(updateErr,
  		{
		TransactionStartLC(aRequest->SessionId());
		
		iPersistenceLayer.PersistenceBroker().SetConnectionId(aRequest->SessionId());
		iPersistenceLayer.PersistenceBroker().UpdateL(aRequest->Item(), aRequest->SessionId());
	
		TransactionCommitLP();
  		});
	if (updateErr == KSqlErrGeneral)
		{
		// Write operation failed, probably due to view read activity
		return DeferWithTimeOutError(aRequest);
		}
	else if (updateErr != KErrNone)
		{
		// Unknown error, propagate the leave to the client
		User::Leave(updateErr);
		}

	aRequest->Complete();
	return EProcessed;		
	}		

/**
 Commit an opened contact item to the database  
 Unlock the contact item so other session can modify it.
 Starts and commits a local transaction inorder to handle leaves.
 
 @param aRequest Commit contact item request object  
 @return TAccept EProcessed if finished processing request or one of the 
         values returned by DeferWithTimeOutError
 @see CState::DeferWithTimeOutError 		 	     
*/
TAccept CStateWritable::AcceptRequestL(CReqCommitCnt* aRequest)
	{
  	if (iStateMachine.LowDisk())
  		{
  		aRequest->Complete(KErrDiskFull);
  		return EProcessed;
  		}

	// Check if the contact has been locked by this session
  	if (iStateMachine.TransactionLockL().IsLocked(aRequest->SessionId(), aRequest->Item().Id()))
  		{
  		// If the request can not be procesed after the timeout period, it should 
  		// complete with KErrInUse to assure binary compatibility with the original model
		return DeferWithTimeOutError(aRequest);
  		}
	
  	TRAPD(commitErr,
  		{
		TransactionStartLC(aRequest->SessionId());
	    iPersistenceLayer.PersistenceBroker().SetConnectionId(aRequest->SessionId());
	    iPersistenceLayer.PersistenceBroker().UpdateL(aRequest->Item(), aRequest->SessionId());
		User::LeaveIfError(iStateMachine.TransactionLockL().UnLockL(aRequest->SessionId(), aRequest->Item().Id()));
		TransactionCommitLP();
		});
  	if (commitErr == KSqlErrGeneral)
  		{
  		// Can't commit contact, probably due to idle sorter activity
  		return DeferWithTimeOutError(aRequest);
  		}
  	else
  		{
  		User::LeaveIfError(commitErr);
  		}
  	
	aRequest->Complete();
	return EProcessed;		
	}


/**
 Delete a contact item from the database. 
 Starts and commits a local transaction inorder to handle leaves.
 Checks if the contact item is locked by another session.
 
 @param aRequest Delete contact item request object  
 @return TAccept EProcessed if finished processing request or one of the 
         values returned by DeferWithTimeOutError
 @see CState::DeferWithTimeOutError
*/
TAccept CStateWritable::AcceptRequestL(CReqDeleteCnt* aRequest)
	{
	// Check if the contact has been locked 
	if (iStateMachine.TransactionLockL().IsLocked(aRequest->CntItemId()))
  		{
  		// If the request can not be procesed after the timeout period, it should 
  		// complete with KErrInUse - the contact is locked 
		return DeferWithTimeOutError(aRequest);
		}	
	
	TRAPD(deleteErr,
		{
		TransactionStartLC(aRequest->SessionId());
	
		iPersistenceLayer.PersistenceBroker().SetConnectionId(aRequest->SessionId());
		CContactItem* cntItem = iPersistenceLayer.PersistenceBroker().DeleteLC(aRequest->CntItemId(), aRequest->SessionId(), ESendEvent);
		CleanupStack::PopAndDestroy(cntItem);
	
		TransactionCommitLP();
		});
	if (deleteErr == KSqlErrGeneral)
		{
		// Write operation failed, probably due to view read activity
		return DeferWithTimeOutError(aRequest);
		}
	else if (deleteErr != KErrNone)
		{
		// Unknown error, propagate the leave to the client
		User::Leave(deleteErr);
		}
		
	aRequest->Complete();
	return EProcessed;		
	}		
	
/**
 Create a new contact item

 @param aRequest Create contact item request object  
 @return TAccept EProcessed 
*/
TAccept CStateWritable::AcceptRequestL(CReqCreateCnt* aRequest)
	{
  	if (iStateMachine.LowDisk())
  		{
  		aRequest->Complete(KErrDiskFull);
  		return EProcessed;
  		}
	
  	TContactItemId cntID(KNullContactId);
  	TRAPD(createErr,
  		{
	  	TransactionStartLC(aRequest->SessionId());
	    
		iPersistenceLayer.PersistenceBroker().SetConnectionId(aRequest->SessionId());
	    cntID = iPersistenceLayer.PersistenceBroker().CreateL(aRequest->Item(), aRequest->SessionId());
		
	    TransactionCommitLP();
  		});
	if (createErr == KSqlErrGeneral)
		{
		// Write operation failed, probably due to view read activity
		return DeferWithTimeOutError(aRequest);
		}
	else if (createErr != KErrNone)
		{
		// Unknown error, propagate the leave to the client
		User::Leave(createErr);
		}
	
	aRequest->Complete(cntID);
	return EProcessed;		
	}		

/**
 Change the current state of the model to CStateTablesClosed. 
 The database is unavailable.		
  
 @param aRequest Close database tables request object  
 @return TAccept EProcessed if finished processing request
*/   
TAccept CStateWritable::AcceptRequestL(CReqCloseTables* aRequest)
  	{
  	iPersistenceLayer.ContactsFileL().CloseTablesL(ETrue);	
  	iStateMachine.SetCurrentStateL(iStateMachine.StateTablesClosed());
  	aRequest->Complete();
  	return EProcessed;		
  	}


/** 
 Do nothing for Cancel - it's too late as the database is already open.

 @param aRequest Cancel open database request object  
 @return TAccept EProcessed if finished processing request
*/ 
TAccept CStateWritable::AcceptRequestL(CReqCancelAsyncOpen* aRequest)
	{
	aRequest->Complete();
	return EProcessed;		
	}


/**
 Start a database transaction by moving to a transaction state		
 
 @param aRequest Begin transaction request object  
 @return TAccept EProcessed if finished processing request
 		 	     EDeferred if the request was not processed 
*/ 
TAccept CStateWritable::AcceptRequestL(CReqDbBeginTrans* aRequest)
	{
  	// In the current implementation there are no operations allowed under the
  	// low disk condition that will reduce the size of the database: this is in
  	// line with Contacts Model 1 behaviour.  Later, when we allow operations
  	// that reduce the size of the database, this check should be removed and
  	// allow the transition to CStateTransaction.
  	if (iStateMachine.LowDisk())
  		{
  		aRequest->Complete(KErrDiskFull);
  		return EProcessed;
  		}	
	
	iStateMachine.SetCurrentStateL(iStateMachine.StateTransaction());
	return iStateMachine.CurrentState().AcceptRequestL(aRequest);
	}
		
/**
 Start a database backup/restore by moving to CStateBackupRestore state.  If any
 contact items are locked or there is any asynchronous activity using the
 database then we cannot close the file (and the backup/restore client will not
 be allowed to backup/restore).

 @param aRequest Notification of backup/restore begin request object  
 @return TAccept EProcessed if finished processing request
*/ 
TAccept CStateWritable::AcceptRequestL(CReqBackupRestoreBegin* aRequest)
	{
	if (!iStateMachine.TransactionLockL().AnyLocked() &&
		!iStateMachine.AsyncActivity())
		{
		// First reset collection, since it construct views based on table 
		// Reset will fail if called after closing tables 
		iPersistenceLayer.FactoryL().GetCollectorL().Reset(); 
		// Close the file to allow the backup/restore to take place.	
		iPersistenceLayer.ContactsFileL().Close();
		}
	iStateMachine.SetCurrentStateL(iStateMachine.StateBackupRestore());
	aRequest->Complete();
	return EProcessed;
	}


/**
 Reset the speed dials		

 @param aRequest Reset speed dials request object  
 @return TAccept EProcessed if finished processing request
 		 	     EDeferred if the request was not processed 
*/ 
TAccept CStateWritable::AcceptRequestL(CReqSetSpeedDial* aRequest)
	{
	if (iStateMachine.LowDisk())
  		{
  		aRequest->Complete(KErrDiskFull);
  		return EProcessed;
  		}
	
	TContactItemId contactId = aRequest->TheContactId();
	if(contactId == 0)
		{
		User::Leave(KErrArgument);
		}

	// Obtain the contact ID currently associated with the speed dial index.
	// The phone number is not being used at the moment
	TSpeedDialPhoneNumber phoneNumberFromSpeedDialTable;
	TContactItemId OldContactId = aRequest->SpeedDialTable().SpeedDialContactItem(aRequest->SpeedDialIndex(), phoneNumberFromSpeedDialTable);
	
	// We should not be able to remove a speed dial from an open item, even if
	// it has been opened by the same session: use the IsLocked() method which
	// ignores session ID.
    TBool isLocked = iStateMachine.TransactionLockL().IsLocked(OldContactId);		
				
	// This code resets an entry from the speed dial table, as required when calling either 
	// CContactDatabase::ResetServerSpeedDialsL() (contactId is KNullContactId) or 
	// CContactDatabase::RemoveSpeedDialFieldL()  (contactId must be equal to the OldContactId)
	// If the field index is -1, it indicates that the speed dial entry corresponding 
	// to the speed dial index passed in the request should be reset.	
	TBool doResetOldContactItem = ETrue;
	TBool doResetSpeedDialEntry = aRequest->TheFieldIndex() == -1;			
	
	if (doResetSpeedDialEntry)
		{		
		if (contactId == KNullContactId || contactId == OldContactId)
		    {
		    if (isLocked)
		        {
		        User::Leave(KErrInUse);
		        }			    
		        
		    aRequest->IniFileManager().SetSpeedDialIdForPositionL(aRequest->SpeedDialIndex(), KNullContactId, KNullDesC(), aRequest->SessionId(), EFalse);				    
		    }
		else
		    {
		    doResetOldContactItem = EFalse;
		    }
		}

	//Everything, i.e. removal of the old speed dial reference and 
	//the setting of the new takes place during the same transaction
	//i.e. start transaction here
	TransactionStartLC(aRequest->SessionId());
		{
		// Check if there is already a contact associated with this speed dial
		// index.
		if (OldContactId != KErrNotFound && doResetOldContactItem)
			{
			// Fetch the item from the ID, remember to pop it
			CContactItem* cntItem = iPersistenceLayer.PersistenceBroker().ReadLC(OldContactId, aRequest->ItemViewDef(), EPlAllInfo, aRequest->SessionId());	
			// Remove speed dial attributes from the contact item field.
			TUid fieldTypeUid = CCntServerSpeedDialManager::SpeedDialFieldUidFromSpeedDialPosition(aRequest->SpeedDialIndex());
			TInt fieldIdFound = cntItem->CardFields().Find(fieldTypeUid);
			if (fieldIdFound != KErrNotFound)
				{		
				cntItem->CardFields()[fieldIdFound].RemoveFieldType(fieldTypeUid);
				cntItem->CardFields()[fieldIdFound].SetSpeedDial(EFalse);
				}
			// Update changes to the contact item in the database.
			iPersistenceLayer.PersistenceBroker().SetConnectionId(aRequest->SessionId());
			iPersistenceLayer.PersistenceBroker().UpdateL(*cntItem, aRequest->SessionId(), ETrue);
			CleanupStack::PopAndDestroy(cntItem);
			}
		
		if (!doResetSpeedDialEntry)
			{
			// Fetch the contact item containing the phone number to be used as
			// a speed dial.
			CContactItem* cntItem = iPersistenceLayer.PersistenceBroker().ReadLC(contactId, aRequest->ItemViewDef(), EPlAllInfo, aRequest->SessionId());	
			if (cntItem->CardFields().Count() < 1)
				{
				User::Leave(KErrUnknown);		
				}	
			// Get the field containing the number to be associated with the
			// speed dial.
			CContactItemField& speeddialField = cntItem->CardFields()[aRequest->TheFieldIndex()];
			// Add speed dial attributes to the contact item field.
			TUid fieldTypeUid = CCntServerSpeedDialManager::SpeedDialFieldUidFromSpeedDialPosition(aRequest->SpeedDialIndex());
			if (!speeddialField.ContentType().ContainsFieldType(fieldTypeUid))
				{
				speeddialField.AddFieldTypeL(fieldTypeUid);
				}
			speeddialField.SetUserAddedField(ETrue);
			speeddialField.SetSpeedDial(ETrue);
			// Get the phone number from the field.
			if (speeddialField.StorageType() != KStorageTypeText)
				{
				User::Leave(KErrArgument);		
				}	
			// Truncate it if its length is > KSpeedDialPhoneLength
			TInt numLen = Min(speeddialField.TextStorage()->Text().Length(), KSpeedDialPhoneLength);
			TPtrC phoneNumber(speeddialField.TextStorage()->Text().Mid(0, numLen));
			// Update changes to the contact item in the database.
			iPersistenceLayer.PersistenceBroker().SetConnectionId(aRequest->SessionId());
			// Update the speed dial table.
			aRequest->IniFileManager().SetSpeedDialIdForPositionL(aRequest->SpeedDialIndex(), contactId, phoneNumber, aRequest->SessionId(), EFalse);
			iPersistenceLayer.PersistenceBroker().UpdateL(*cntItem, aRequest->SessionId(), ETrue);
			// Unlock the item.
			User::LeaveIfError(iStateMachine.TransactionLockL().UnLockL(aRequest->SessionId(), contactId));
			CleanupStack::PopAndDestroy(cntItem);
			}
		}
	TransactionCommitLP();
	aRequest->Complete();
	return EProcessed;		
	}


/**
 Set own card data	

 @param aRequest Set own card request object  
 @return TAccept EProcessed if finished processing request
 		 	     EDeferred if the request was not processed 
*/ 		
TAccept CStateWritable::AcceptRequestL(CReqSetOwnCard* aRequest)
	{
	if (iStateMachine.LowDisk())
  		{
  		aRequest->Complete(KErrDiskFull);
  		return EProcessed;
  		}

	TUid aContactType = aRequest->Item().Type();

	// this should leave with kerrnotsupported if the type doesn't match!!!!
	if (aContactType==KUidContactGroup || aContactType==KUidContactTemplate || aContactType==KUidContactCardTemplate)
		{
		User::Leave(KErrNotSupported);	
		}
	// if the requested item is already set as own card then just return
	if (aRequest->Item().Id()==iPersistenceLayer.ContactProperties().OwnCardIdL())
		{
		aRequest->Complete();
		return EProcessed;	
		}
		
	// if the requested ID is system template or KNUllContactID then leave
	if (aRequest->Item().Id()==KGoldenTemplateId || aRequest->Item().Id()==KNullContactId)
		{
		User::Leave(KErrNotFound);
		}
	//Everything, i.e. removal of the old own card reference and 
	//the setting of the new takes place during the same transaction
	//i.e. start transaction here
	TransactionStartLC(aRequest->SessionId());
		{//reset the old own card to become an ordinary contacts card
		if (iPersistenceLayer.ContactProperties().OwnCardIdL() != KNullContactId)
			{
			// Fetch the old current item from the ID, remember to pop it
			CContactItem* oldOwnCard = iPersistenceLayer.PersistenceBroker().ReadLC(iPersistenceLayer.ContactProperties().OwnCardIdL(), aRequest->ItemViewDef(), EPlAllInfo, aRequest->SessionId());	
			iPersistenceLayer.PersistenceBroker().SetConnectionId(aRequest->SessionId());
			//change the type to KUidContactCard for the old ownCard
			iPersistenceLayer.PersistenceBroker().ChangeTypeL(oldOwnCard->Id(), KUidContactCard);
			CleanupStack::PopAndDestroy(oldOwnCard);
			}			
		iPersistenceLayer.PersistenceBroker().SetConnectionId(aRequest->SessionId());
		//Now set the new own card
		iPersistenceLayer.PersistenceBroker().ChangeTypeL(aRequest->Item().Id(), KUidContactOwnCard);
		iPersistenceLayer.ContactProperties().SetOwnCardIdL(aRequest->Item().Id(), EFalse);
		}
	TransactionCommitLP();
	aRequest->Complete();
	return EProcessed;		
	}		

/** 
 The error code which deferred requests use after a timeout from the 
 writable state  this maintians consistency with the original contacts model
 
 @return The most common the timeout error code -KErrInUse- used in the writable state
*/ 
TInt CStateWritable::TimeOutErrorCode()
	{
	return KErrInUse;		
	}



///////////////////// CStateTransaction Implementation/////////////////
/** 
 CStateTransaction Class NewL factory constructor
 Create a transaction state
 @see CState constructor
*/ 
CStateTransaction* CStateTransaction::NewL(CCntStateMachine& aStateMachine, CPersistenceLayer& aPersistenceLayer)
	{
	CStateTransaction* stateTransaction = new (ELeave) CStateTransaction(aStateMachine, aPersistenceLayer);
	CleanupStack::PushL(stateTransaction);
	stateTransaction->ConstructL();
	CleanupStack::Pop(stateTransaction);
	return stateTransaction;
	}

/**
 Instantiate the CTransctionTimer object. The transaction state contains a timer
 because a client may put the server into a transation state and for some unknown 
 reason my never complete the transaction. Such a senario would result in the server
 becoming unusable (ie No other client can modify the database until the transaction 
 is completed or rolled back). If the CStateTransaction class does not hear from the
 client for a given time, the transaction times out and is rolled back. This is the 
 responsibility of the CTransactionTimer.
*/
void CStateTransaction::ConstructL()
	{
	// Pass the transaction state that will timeout.
	iTimeOut = CTransactionTimer::NewL(*this); 
	}

/** 
 CStateTransaction Class constructor
 @see CState constructor
*/ 
CStateTransaction::CStateTransaction(CCntStateMachine& aStateMachine, CPersistenceLayer& aPersistenceLayer)
: CState(aStateMachine, aPersistenceLayer), iEventQ(KCntEventGranularity)
	{
	}

/** 
 CStateTransaction Class constructor
 @see CState constructor
*/ 
CStateTransaction::~CStateTransaction()
	{
	delete iTimeOut;
	iEventQ.Close();
	}
	
/**	
 Cancel the transaction - will result in a database rollback
 implemented in the base CState class
 This overwritten CancelTransaction is only ever called when the transaction
 times out. The base class CState::CancelTransactionL() is called when any 
 state class or persistence layer method leaves
*/
void CStateTransaction::CancelTransactionL()
  	{
  	CState::RollbackTransAndRecoverL(EFalse); 
  	iSessionId = 0; // Allow another session enter a transaction state.
  	}

/**	
 Get the CStateTransaction default timeout error code.
 
 @return TInt ErrLocked
*/
TInt CStateTransaction::TimeOutErrorCode()
	{
	return KErrLocked;	
	}


/**
 Begin a database transaction, start the transaction timeout which rolls-back
 the transaction if the session dies. Reset the transactions event queue - this
 event-queue queues all events until the transaction has been committed.
 
 @param aRequest Begin a database transaction
 @return TAccept EProcessed if finished processing request, or one of the values returned
                 by CState::DeferRequest 
 @see CState::DeferRequest 
*/ 
TAccept CStateTransaction::AcceptRequestL(CReqDbBeginTrans* aRequest)
	{
	
	#if defined(__PROFILE_DEBUG__)
		RDebug::Print(_L("[CNTMODEL] MTD: CStateTransaction::AcceptRequestL"));
	#endif 
	
	// Only one session can ever be in a transaction state
	if (iSessionId == 0)
		{
		iSessionId = aRequest->SessionId();
		iPersistenceLayer.TransactionManager().StartTransactionL();
		iTimeOut->Start();
		aRequest->Complete();
		// Reset the event queue - although it is also reset when a transaction
		// is committed or explicitly rolled back, it will now be reset if a rollback occured 
		// because of a leave. Resetting the queue after commit/explicit rollback free's memory.
		iEventQ.Reset(); 
		return EProcessed;	
		}
	// This session has already started a transaction 	
	if (iSessionId == aRequest->SessionId())
		{
		aRequest->Complete();
		return EProcessed;	
		}
	// Another session has started a transaction
	return DeferRequest(aRequest); 
	}


/**
 Commit the current transaction if started by the same session.
 Stop the transaction timer.
 Propogate all events to the observer.
 Compress the database.
 
 @param aRequest Commit a database transaction
 @return TAccept EProcessed if the transaction has been committed
 @leave KErrLocked if a different session had started the transaction
*/
TAccept CStateTransaction::AcceptRequestL(CReqDbCommitTrans* aRequest)
	{
 	if (iSessionId == aRequest->SessionId())
 		{
		TRAPD(commitErr, iPersistenceLayer.TransactionManager().CommitCurrentTransactionL(aRequest->SessionId()));
		if (commitErr == KSqlErrGeneral)
			{
			// Operation has probably been blocked due to read lock by view idle sorter.
			return DeferWithTimeOutError(aRequest);
			}
		else
			{
			User::LeaveIfError(commitErr);
			}
		
		iTimeOut->Stop(); // Transaction completed - shouldn't timeout

		// The database had been updated. All session should now be notified
		// of events.
		PropagateDatabaseEventsL();
		
		iStateMachine.SetCurrentStateL(iStateMachine.StateWritable());				

		iSessionId = 0;
		// Only complete the request after the last leaving method. If 
		// a leave occurs after the request (message) has been completed, then the method 
		// CCntSession::ServiceError will try to complete the message a second time
		// causing a panic. 
		aRequest->Complete();
 		}
 	else
		{
		StrayRequestL(aRequest); // Only the current session should be able to
		}						 // send a commit transaction request
	return EProcessed;		
	}



/** 
 Rollback the current transaction if started by the same session
 Reset the event queue as no operation has been committed to the database.
 Don't compress the database as it hasn't changed.
 Notify the observer that a rollback has occured.
 
 @param aRequest Rollback a database transaction
 @return TAccept EProcessed if the transaction has been rolled back
 @leave KErrLocked if a different session had started the transaction
*/
TAccept CStateTransaction::AcceptRequestL(CReqDbRollbackTrans* aRequest)
	{
 	if (iSessionId == aRequest->SessionId())
 		{
		iEventQ.Reset(); // Empty the event queue - no operations have been committed
						 // so sessions should never be notified of the event

 		iCurrentTransactionSessionId = aRequest->SessionId();
		CState::RollbackTransAndRecoverL(ETrue);
		iSessionId = 0;

		// Transaction completed - shouldn't timeout
		iTimeOut->Stop(); 
	
		// Rollback event needs to be propagated.
		TContactDbObserverEventV2 event;
		event.iType			= EContactDbObserverEventRollback;
		event.iContactId	= KNullContactId;
		event.iConnectionId = iSessionId;
	    event.iTypeV2 = EContactDbObserverEventV2Null;
	    event.iAdditionalContactId = KNullContactId;		
		iStateMachine.DbManager().HandleDatabaseEventV2L(event);

		aRequest->Complete();
 		}
 	else
		{
		StrayRequestL(aRequest); // Only the current session should be able
		}						 // to rollback a transaction
	return EProcessed;
	}


/** 
 A session tried to commit or rollback a transaction that was started
 by another session. Destroy the request and leave.

 @param aRequest A pointer to a request object scoped to it parent class.
 @leave KErrLocked A different session had started the transaction
*/ 
void CStateTransaction::StrayRequestL(CCntRequest* /* aRequest */)
	{
	User::Leave(KErrLocked);	
	}


/**
 Create a contact item from a the session that started the transaction
 
 @param aRequest The request that will contain the created contact item 
 @return TAccept EProcessed if finished processing request
 		 	     EDeferred if the request was not processed 
*/
TAccept CStateTransaction::AcceptRequestL(CReqCreateCnt* aRequest)
	{
	#if defined(__PROFILE_DEBUG__)
		RDebug::Print(_L("[CNTMODEL] MTD: CStateTransaction::AcceptRequestL"));
	#endif 	
	
  	if (iStateMachine.LowDisk())
  		{
  		aRequest->Complete(KErrDiskFull);
  		return EProcessed;
  		}
	
 	if (iSessionId == aRequest->SessionId())
 		{
 		TContactItemId itemId(KNullContactId);
 		TRAPD(createErr, itemId = iPersistenceLayer.PersistenceBroker().CreateL(aRequest->Item(),  aRequest->SessionId()));
 		if (createErr == KSqlErrGeneral)
 			{
 			// Can't create contact item, probably due to view idle sorter activity
 			return DeferWithTimeOutError(aRequest);
 			}
 		else
 			{
 			User::LeaveIfError(createErr);
 			}
 		
		aRequest->Complete(itemId);
		iTimeOut->Reset(); // restart the timeout as the client session is still alive
		return EProcessed;
		}
 	
 	// The session that is trying to perform this operation has not started the transaction
 	return DeferWithTimeOutError(aRequest);
	}

/**
 Read a contact item - always allow read operation from any session 
 ie the session does not need to have started the transaction to read the contact item
 
 @param aRequest The request that will contain the contact item read from the database
 @return TAccept EProcessed if finished processing request
 		 	     EDeferred if the request was not processed 
*/ 
TAccept CStateTransaction::AcceptRequestL(CReqReadCnt* aRequest)
	{
	iTimeOut->Reset(); // Reset the timeout, the client is still alive
	return CState::AcceptRequestL(aRequest);
	}
	
/**
 Update a contact item from a the session that started the transaction	
 
 @param aRequest The request that contain the contact item that is to be updated in the database
 @return TAccept EProcessed if finished processing request, or one of the values returned
                 by CState::DeferRequest 
 @see CState::DeferRequest 
*/ 
TAccept CStateTransaction::AcceptRequestL(CReqUpdateCnt* aRequest)
	{
  	if (iStateMachine.LowDisk())
  		{
  		aRequest->Complete(KErrDiskFull);
  		return EProcessed;
  		}

	// Check if the contact has been locked by any session - including this session
	// This is for reasons of compatibility with the original model only
  	if (iStateMachine.TransactionLockL().IsLocked(aRequest->Item().Id()) == EFalse)
  		{
  		 if (iSessionId == aRequest->SessionId())
	 		{
	 		TRAPD(updateErr, iPersistenceLayer.PersistenceBroker().UpdateL(aRequest->Item(), aRequest->SessionId()));
			if (updateErr == KSqlErrGeneral)
				{
				// Can't update item, probably due to idle sorter activity
				return DeferWithTimeOutError(aRequest);
				}
			else
				{
				User::LeaveIfError(updateErr);
				}
			
			aRequest->Complete();
			iTimeOut->Reset(); 
			
			return EProcessed;
			}
	 	else
	 		{
	 		// The session that is trying to perform this operation has not started the transaction
	 		return DeferWithTimeOutError(aRequest);
	 		}
  		}
  	else
  		{
  		// If the request can not be procesed after the timeout period, it should 
  		// complete with KErrInUse as the contact is locked
  		aRequest->SetTimeOutError(KErrInUse);  		
  		}
	return DeferRequest(aRequest);	
	}
	
/**
 Delete a contact item if the delete request is from the same session that started the transaction
 
 @param aRequest The request that contain the contact item ID that is to be deleted from the database
 @return TAccept EProcessed if finished processing request, or one of the values returned
                 by CState::DeferRequest 
 @see CState::DeferRequest 
*/ 
TAccept CStateTransaction::AcceptRequestL(CReqDeleteCnt* aRequest)
	{
	if (iStateMachine.LowDisk())
  		{
  		aRequest->Complete(KErrDiskFull);
  		return EProcessed;
  		}	
	
	// Check if the contact has been locked by any session - including this session
  	if (iStateMachine.TransactionLockL().IsLocked(aRequest->CntItemId()) == EFalse)
  		{
	 	if (iSessionId == aRequest->SessionId())
	 		{
	 		CContactItem* item = NULL; 
	 		TRAPD(deleteErr, item = iPersistenceLayer.PersistenceBroker().DeleteLC(aRequest->CntItemId(), aRequest->SessionId(), aRequest->NotificationEventAction());CleanupStack::PopAndDestroy(item));
	 		if (deleteErr == KSqlErrGeneral)
	 			{
	 			// Delete failed, probably due to idle sorter activity
	 			return DeferWithTimeOutError(aRequest);
	 			}
	 		else
	 			{
	 			User::LeaveIfError(deleteErr);
	 			}
	 		
			aRequest->Complete();
			iTimeOut->Reset(); 

			return EProcessed;	
	 		}
	 	else
	 		{
	 		// The session that is trying to perform this operation has not started the transaction
	 		return DeferWithTimeOutError(aRequest);
	 		}
  		}
  	else
  		{
  		// If the request can not be procesed after the timeout period, it should 
  		// complete with KErrInUse as the contact is locked
  		aRequest->SetTimeOutError(KErrInUse);  		
  		}

	return DeferRequest(aRequest);	
	}


/**
 Commit (write & unlock) a contact item that has been locked to the database
 The contact item is unlocked.
 
 @param aRequest The request that contain the contact item that is to be written to the database
 @return TAccept EProcessed if finished processing request
 		 	     EDeferred if the request was not processed 
*/ 
TAccept CStateTransaction::AcceptRequestL(CReqCommitCnt* aRequest)
	{
	if (iStateMachine.LowDisk())
		{
		aRequest->Complete(KErrDiskFull);
		return EProcessed;
		}

 	if (iSessionId == aRequest->SessionId())
 		{
 		User::LeaveIfError(iStateMachine.TransactionLockL().UnLockL(aRequest->SessionId(), aRequest->Item().Id()));
 		TRAPD(updateErr, iPersistenceLayer.PersistenceBroker().UpdateL(aRequest->Item(), aRequest->SessionId()));
 		if (updateErr == KSqlErrGeneral)
 			{
 			// Can't update contact, probably due to idle sorter activity
 			return DeferWithTimeOutError(aRequest);
 			}
 		else
 			{
 			User::LeaveIfError(updateErr);
 			}
 		
		aRequest->Complete();
		iTimeOut->Reset(); 
		
		return EProcessed;	
		}
 	else
 		{
 		// The session that is trying to perform this operation has not started the transaction
 		return DeferWithTimeOutError(aRequest);
 		}
	}

/**
 Open (read and lock) the contact item, returning the opened contact item.
 The contact item is also locked
 
 @param aRequest The request that will contain the contact item that is to be opened (read and locked) 
 				 in the database
 @return TAccept EProcessed if finished processing request
 		 	     EDeferred if the request was not processed 
*/ 
TAccept CStateTransaction::AcceptRequestL(CReqOpenCnt* aRequest)
	{
 	if (iSessionId == aRequest->SessionId())
 		{
 		// As a valid operation has been performed by the session, it is still 
 		// alive so the timeout for the transaction state must be reset.
		iTimeOut->Reset(); 
		return CState::AcceptRequestL(aRequest);
 		}
  	else
 		{
 		// The session that is trying to perform this operation has not started the transaction
 		return DeferWithTimeOutError(aRequest);	
 		}
	}

/**
 Close (unlock) the locked contact without commiting the contact item to the database 
 only if the session also started the transaction
 
 @param  aRequest The request that contain the contact item that is to be closed (unlocked but not updated)
 @return One of the values returned by DeferWithTimeOutError, or CState::AcceptRequest(CReqCloseCnt*)
 @see CState::DeferWithTimeOutError
 @see CState::AcceptRequest(CReqCloseCnt*)
*/ 
TAccept CStateTransaction::AcceptRequestL(CReqCloseCnt* aRequest)
	{
 	if (iSessionId == aRequest->SessionId())
 		{
		iTimeOut->Reset(); 
		return CState::AcceptRequestL(aRequest);
		}
 	else
 		{
 		// The session that is trying to perform this operation has not started the transaction
 		return DeferWithTimeOutError(aRequest);
 		}
	}

/**
 Hanlde a database event while in the transaction state.
 The CStateTransaction holds all events, and does not propagate them to 
 the DbManager, until an explicit commit request is called and the database
 is written to. All events are not valid until a commit is called as a 
 transaction rollback would invalidate the events.
 
 @param aEvent The database event that is being propogated.
*/
void CStateTransaction::HandleDatabaseEventV2L(TContactDbObserverEventV2 aEvent)
	{
	
	DEBUG_PRINT1(__VERBOSE_DEBUG__,_L("[CNTMODEL] Database Event in Transaction"));

	// Do not add a rollback event to the queue. This event will be propagated as 
	// part of the transaction rollback.
	if (aEvent.iType == EContactDbObserverEventRollback)
		{
		return;
		}
	if (iEventQ.Count() <= KMaxNumberOfEventsInEventQueue)
		{
		
		DEBUG_PRINT1(__VERBOSE_DEBUG__,_L("[CNTMODEL] Database Event Added To Q in Transaction"));
	
		iEventQ.AppendL(aEvent);	
		} 
	// else - do nothing as a EContactDbObserverEventUnknownChanges will be propagated
	// to all observers	
	}


/** 
 Propagate events back to the CCntDbManager after a commit transaction request
*/
void CStateTransaction::PropagateDatabaseEventsL()
	{
	if (iEventQ.Count() >= KMaxNumberOfEventsInEventQueue)
		{
		
		DEBUG_PRINT1(__VERBOSE_DEBUG__,_L("[CNTMODEL] Max Database Events reached in Transaction - Unknown changes event"));

		// Tell the observer to re-sync with the database as there has been too many
		// operations for which it needs to recieve notification
		TContactDbObserverEventV2 unknownChangeEvent;
		unknownChangeEvent.iType 		 = EContactDbObserverEventUnknownChanges;
		unknownChangeEvent.iContactId 	 = KNullContactId;
		unknownChangeEvent.iConnectionId = KCntNullConnectionId;
		unknownChangeEvent.iTypeV2 = EContactDbObserverEventV2Null;
		unknownChangeEvent.iAdditionalContactId = KCntNullConnectionId;
		
		iStateMachine.DbManager().HandleDatabaseEventV2L(unknownChangeEvent);
		}
	else
		{
		
		DEBUG_PRINT1(__VERBOSE_DEBUG__,_L("[CNTMODEL] Database Events being propagated in Transaction"));
	
		TInt max = iEventQ.Count();
		TInt ii  = 0;
		// Propagate the events from the beginning of the EventQ
		while(ii != max) 
			{
			iStateMachine.DbManager().HandleDatabaseEventV2L(iEventQ[ii]);	
			++ii;	
			}
		}
	iEventQ.Reset(); // Empty the queue	
	}



////////////////// CTransactionTimer Implementation //////////////////////

/** 
 CTransactionTimer Class NewL factory constructor
 Create a transaction timer

 The transaction timer is used to allow a transaction to timeout should
 the session which put the state machine into a transaction state die
 unexpectedly. It is derived from CTimer and times out after sixty seconds.
*/ 
CTransactionTimer* CTransactionTimer::NewL(CStateTransaction& aTransState)
	{
	CTransactionTimer* self = new (ELeave) CTransactionTimer(aTransState);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
/** 
 CTransactionTimer Class destructor
*/	
CTransactionTimer::~CTransactionTimer()
	{
	CTimer::Cancel();
	}
	
/**
 CTransactionTimer constructor
*/	
CTransactionTimer::CTransactionTimer(CStateTransaction& aTransState) 
				 : CTimer(CActive::EPriorityIdle), iTransState(aTransState)
	{
	}

void CTransactionTimer::ConstructL()
	{
	CTimer::ConstructL();
	CActiveScheduler::Add(this);
	}


/** 
 The Transaction was neither commited nor rolled back 
 The client may have died - clean up the state machine by rolling back
*/ 
void CTransactionTimer::RunL()
	{
	iTransState.CancelTransactionL();
	CTimer::Cancel();
	}

/**
 Start the timer. This is done when a the state machine moves 
 into the transaction state.
*/
void CTransactionTimer::Start()
	{ // wait for 60 seconds
	CTimer::Cancel();
	CTimer::After(KSixtySeconds);
	}

/**
 Stop the timer. This is done when a the state machine moves 
 out of the transaction state.
*/
void CTransactionTimer::Stop()
	{
	CTimer::Cancel();
	}

/**
 When a valid operation is performed within the transaction state
 the timer is reset to sixty seconds, if another transaction operation is not
 performed within sixty seconds, the transaction should timeout.
*/
void CTransactionTimer::Reset()
	{
	CTimer::Cancel();
	CTimer::After(KSixtySeconds);
	}
	
////////////////// CTransactionLock Class Implementation ////////////////////////
/** 
 CTransactionLock Class NewL factory constructor
 The CTransactionLock class locks contacts allowing only the locking session to
 modify the contact item in the database.
*/
CTransactionLock* CTransactionLock::NewL(CCntStateMachine& aStateMachine)
	{
	CTransactionLock* self = new (ELeave) CTransactionLock(aStateMachine);
	return self;
	}
	
// TLockData constructor	
CTransactionLock::TLockData::TLockData(TContactItemId aCntId, const TUint aSessionId):iCntItemId(aCntId), iSessionId(aSessionId)
	{}
	
// --------- Locking Methods -----------
/** 
 Locks a contact item by adding its ID to an array of locked contact items IDs.
 After a session locks the contact item, no other session can modify the locked contact
 item. 
 Adds the lock to the clean up stack, indicated by the 'X' in the method name
 
 @param aCntId The ID of contact item to be locked
 @param aSessionId The Session which is locking the contact Item
 aContact The contact item to add to the database.
 @return KErrNone if locking was successful.
		KErrInUse if the contact item was locked by another session 
*/

TInt CTransactionLock::LockLX(const TUint aSessionId, const TContactItemId aCntId)
	{
	
	DEBUG_PRINT2(__VERBOSE_DEBUG__,_L("[CNTMODEL] *   Lock item %d"), aCntId);
	
	if (IsLocked(aCntId))
		{
		return KErrInUse; // A session can only lock a cnt item once.	
		}
		
	TLockData lockData(aCntId, aSessionId);
	
	iLockedIds.InsertInSignedKeyOrderL(lockData);
	
	CleanupStack::PushL(TCleanupItem(CTransactionLock::CleanupUnlockRecord, this));
		
	return KErrNone;	
	}

	
/**
 Unlocks the last locked contact item after a leave
 
 @param aTransLock The CTransactionLock object from which the last locked contact item
 				  ID must be removed (unlocked).
*/
void CTransactionLock::CleanupUnlockRecord(TAny* aTransLock)
	{
	TRAP_IGNORE(static_cast<CTransactionLock*>(aTransLock)->UnlockLastLockedContactL() );
	}	
	
/** 
 UnLocks a contact item by removing its ID to an array of locked contact items IDs.
 
 @param aCntId The ID of contact item to be unlocked
 @param aSessionId The Session which is unlocking the contact Item
 @return KErrNone if locking was successful.
 		 KErrAccessDenied if the contact item was not successfuly locked  
*/	
TInt CTransactionLock::UnLockL(const TUint aSessionId, const TContactItemId aCntId)
	{
	
	DEBUG_PRINT2(__VERBOSE_DEBUG__,_L("[CNTMODEL] * UnLock item %d"), aCntId);
	
	TLockData lockData(aCntId, aSessionId);
	TInt index = iLockedIds.FindInSignedKeyOrder(lockData);
	if (index < 0)
		return KErrAccessDenied;
	
	if (index > iLockedIds.Count())
		{
		return KErrAccessDenied;	
		}
	
	if (iLockedIds[index].iSessionId == aSessionId)
		{
		iLockedIds.Remove(index);
		ProcessLockedContactsL(); // Process any requests in the Store
		}
	return KErrNone;	
	}

/**
 Process any requests in the Store - Another session
 may have been trying to perform an operation on a  
 locked contact item. This method is called after a contact item
 has been unlocked to allow an operation on the same contact item
 to be performed by another session.
*/
void CTransactionLock::ProcessLockedContactsL()
	{
	if(iStateMachine.ReqStoreL().IsEmpty() == EFalse)
		{
		iStateMachine.ReqStoreL().ActivateRequestsL();	
		}		
	}

/**
 Unlocks all the locked contacts for a given sessionid, the request 
 that calls this method, CReqInternalSessionUnlock, originates in the session destructor
 
 @param aSessionId The session that is being closed.
*/ 
void CTransactionLock::UnLockAllL(const TUint aSessionId)
	{
	TInt ii	= iLockedIds.Count();
	while(ii) 
		{
		--ii;		
		if (iLockedIds[ii].iSessionId == aSessionId)
			{
			iLockedIds.Remove(ii);
			}
		}
	ProcessLockedContactsL(); // Process any requests in the Store
	}
	
/** 
 Unlock the last locked contact after an leave has occured	
 
 @param aSessionId The ID of the session that performed the operation in which 
 				   the leave occured.
*/
void CTransactionLock::UnlockLastLockedContactL(TUint aSessionId)	
	{
	if (aSessionId == nsState::KNoSessionId)
		{
		// Remove the last Locked Contact regardless of session
		iLockedIds.Remove(iLockedIds.Count() - 1);
		ProcessLockedContactsL(); // Process any requests in the Store
		return;
		}
	
	TInt ii	= iLockedIds.Count();
	while(ii) 
		{
		--ii;		
		if (iLockedIds[ii].iSessionId == aSessionId)
			{
			iLockedIds.Remove(ii);
			ProcessLockedContactsL(); // Process any requests in the Store
			return; // Finished
			}
		}
	}
	
/** 
Checks if a contact item is locked by another session.

@param aCntId The ID of contact item to be checked
@param aSessionId The Session which is checking for a lock

@return True if the contact has been locked.
		False if the contact has not been locked by another session.
*/
TBool CTransactionLock::IsLocked(const TUint aSessionId, const TContactItemId aCntId) const
	{
	TInt ii	= iLockedIds.Count();
	
	while(ii) 
		{
		--ii;		
		if (iLockedIds[ii].iCntItemId == aCntId)
			{
			if (iLockedIds[ii].iSessionId != aSessionId)
				{
				return ETrue;	// locked by another session	
				}
			return EFalse; // has not been locked by another session	
			}
		}
	return EFalse; // has not been locked by any session
	}

/**
 Checks if a contact item is locked by this or another session (any session).
 Original behaviour of the contacts model was not to allow a session to lock a contact twice.

 @param aCntId The ID of contact item to be checked
 @return True if the contact has been locked.
		False if the contact has not been locked.
*/
TBool CTransactionLock::IsLocked(const TContactItemId aCntId)const
	{
	TInt ii = iLockedIds.Count();
	
	while(ii) 
		{
		--ii;		
		if (iLockedIds[ii].iCntItemId == aCntId)
			{
			return ETrue;	// locked 
			}
		}
	return EFalse;
	}

/**
 Determines if any contacts items are locked by this or another session (any
 session).
 @return True if there is a locked contact.
		False if there is no locked contact.
*/
TBool CTransactionLock::AnyLocked() const
	{
	return iLockedIds.Count() != 0;
	}

/** CStateBackupRestore
	While in this state neither read nor write operations are allowed.  The
	CStateClosed parent class AcceptRequestL() handles all read and write
	requests.
	
	Methods completes the request with KErrLocked via a call to TimeOutErrorCode() 
	from the parent CStateClosed class.
*/
CStateBackupRestore* CStateBackupRestore::NewL(CCntStateMachine& aStateMachine, CPersistenceLayer& aPersistenceLayer)
	{
	CStateBackupRestore* stateBUR = new (ELeave) CStateBackupRestore(aStateMachine, aPersistenceLayer);
	return stateBUR;
	}
	
	
CStateBackupRestore::~CStateBackupRestore()
	{
	}


CStateBackupRestore::CStateBackupRestore(CCntStateMachine& aStateMachine, CPersistenceLayer& aPersistenceLayer)
:CStateClosed(aStateMachine, aPersistenceLayer)
	{
	}


// Default AsyncOpen requests behaviour to the base implementation
TAccept CStateBackupRestore::AcceptRequestL(CReqAsyncOpen* aRequest)
	{
	return CState::AcceptRequestL(aRequest);
	}


TAccept CStateBackupRestore::AcceptRequestL(CReqBackupRestoreBegin* aRequest)
  	{
  	// A backup/restore is already in progress so don't defer this request -
  	// simply complete it.
	aRequest->Complete();
  	return EProcessed;
  	}	


TAccept CStateBackupRestore::AcceptRequestL(CReqBackupRestoreEnd* aRequest)
	{
	// Once Backup/Restore completes we can re-open the database.  Re-accept
	// request in CStateOpening.	
	iStateMachine.SetCurrentStateL(iStateMachine.StateOpening());  
	return iStateMachine.CurrentState().AcceptRequestL(aRequest);
 	}


/**
 If there was asynchronous activity then it has now finished so safe to close
 database.
 
 @param aRequest Notification of end of async activity request object
 @return TAccept EProcessed - finished processing request
*/  
TAccept CStateBackupRestore::AcceptRequestL(CReqNoAsyncActivity* aRequest)
  	{
  	if (iStateMachine.AsyncActivity())
  		{
  		iStateMachine.SetAsyncActivity(EFalse);
		// Close the file to allow the backup/restore to take place.	
		iPersistenceLayer.ContactsFileL().Close();
		}
  	aRequest->Complete();
  	return EProcessed;
  	}

/** 
 Returns the default error code - KErrLocked - for the backup restore state 
*/
TInt CStateBackupRestore::TimeOutErrorCode()  	
	{
	return KErrLocked;
	}


// ======================================		
// CCntStateMachine Class implementation	
// The main purpose of the CState object 
// is to define the state transition table
	
CCntStateMachine::~CCntStateMachine()
	{
	iStateArray.ResetAndDestroy();
	delete iReqStore;
	delete iTransLock;
	}
	
CCntStateMachine* CCntStateMachine::NewL(CPersistenceLayer& aPersistenceLayer, CCntDbManager& aDbManager)
	{
	CCntStateMachine* stateMachine = new (ELeave) CCntStateMachine(aDbManager);
	CleanupStack::PushL(stateMachine);
	stateMachine->ConstructL(aPersistenceLayer);
	CleanupStack::Pop(stateMachine);
	return stateMachine;
	}
	
/** 
 Create and add all states to the state machine array	
 
 @param aPersistenceLayer. The persistence layer that wraps up access to the database.
*/
void CCntStateMachine::ConstructL(CPersistenceLayer& aPersistenceLayer)
    {
    // The order in which states are appended must be in sync with
    // the declaration order of the state enum.
    for (TInt i = 0; i < 6; ++i)
        {
        CState* state;
        switch (i)
            {
            case 0:  state = CStateClosed::NewL(*this, aPersistenceLayer); iState = state; break;
            case 1:  state = CStateTablesClosed::NewL(*this, aPersistenceLayer); break;
            case 2:  state = CStateWritable::NewL(*this, aPersistenceLayer); break;
            case 3:  state = CStateOpening::NewL(*this, aPersistenceLayer); break;
            case 4:  state = CStateTransaction::NewL(*this, aPersistenceLayer); break;
            default: state = CStateBackupRestore::NewL(*this, aPersistenceLayer); 
            }
        CleanupStack::PushL(state);
        iStateArray.AppendL(state); 
        CleanupStack::Pop(state);
        }	
    }

/**
 Get the current active state		
 
 @return The current active state
*/ 
CState& CCntStateMachine::CurrentState()
	{
	return *iState;
	}
	
/** 
 Get the transaction lock	
 
 @return The Transaction Lock object
*/
CTransactionLock& CCntStateMachine::TransactionLockL()
	{
	if (!iTransLock)
		{
		iTransLock = CTransactionLock::NewL(*this);			
		}

	return *iTransLock;
	}

/** 
 Get the Database Manager
 
 @return the Contact Database Manager object
*/
CCntDbManager& CCntStateMachine::DbManager()
	{
	return iDbManager;
	}

/**
 StateMachine constructor
 
 @param aDbManager The Database Manager.
*/
CCntStateMachine::CCntStateMachine(CCntDbManager& aDbManager)
	:
  	iDbManager(aDbManager),
  	iLowDisk(EFalse),
  	iAsyncActivity(EFalse)
	{
	// Nothing to do.
	}

/**
 Used for debugging the transition between state. 
 Define __STATE_MACHINE_DEBUG__ in the project definition file (mmp).
 
 @param aState The state which is becoming active
 @return A descriptor containing the active state name.
*/ 
#ifdef __STATE_MACHINE_DEBUG__
const TDesC& CCntStateMachine::StateName(CState& aState)
	{
	_LIT(KStateClosedName,   	"EStateClosed");
	_LIT(KStateTablesClosed, 	"EStateTablesClosed");
	_LIT(KStateWritableName, 	"EStateWritable");
	_LIT(KStateOpeningName,  	"EStateOpening");
	_LIT(KStateTransactionName, "EStateTransaction");
	_LIT(KStateBackupRestoreName, "EStateBackupRestore");
	_LIT(KStateUnknownName,  	"Unknown State");

	if (&aState == &StateClosed())
		{
		return KStateClosedName();
		}
		
	if (&aState == &StateTablesClosed())
		{
		return KStateTablesClosed();	
		}
		
	if (&aState == &StateWritable())
		{
		return KStateWritableName();
		}

	if (&aState == &StateOpening())
		{
		return KStateOpeningName();
		}

	if (&aState == &StateTransaction())
		{
		return KStateTransactionName();
		}

	if (&aState == &StateBackupRestore())
		{
		return KStateBackupRestoreName();
		}

	return KStateUnknownName();
	}
#endif

/** 
 Set the active state in the state machine	
 
 @param aState The state which is becoming active
*/
void CCntStateMachine::SetCurrentStateL(CState& aState)
	{
	
#ifdef __STATE_MACHINE_DEBUG__
	RDebug::Print(_L("[CNTMODEL] STA: %S --> %S\r\n"),	&CCntStateMachine::StateName(*iState), &CCntStateMachine::StateName(aState));
#endif

	iState = &aState;
	// Process any requests in the Store on each state change
	// The state may have changed to one where queued requests 
	// can now be processed.
	if(ReqStoreL().IsEmpty() == EFalse)
		{
		iReqStore->ActivateRequestsL();	
		}
	}

/** 
 This is the interface to the state machine (used by the session class). 
 The state machine is asked to process a request by the session. It does this
 by passing the  current active state to the request object (VisitStateL). 
 The request then calls AcceptRequest on the current active state object. The
 state and the request are completely decoupled.
 
 Caller of ProcessRequestL should assume transfer request ownership unless 
 ProcessRequestL leaves.  In the event ProcessRequestL leaves, the original owner
 of the request is responsible for the cleanup of the request.  
 
 Note that if the request is to be transferred to another object, ensure once the 
 transfer has taken place, no leave should occur, as this will trigger both the new 
 owner and the original owner to cleanup the request.
 
 @param aRequest A request that is being processed.
*/ 
void CCntStateMachine::ProcessRequestL(CCntRequest* aRequest)
    {
	// Obtained ownership of the request object.  It is the responsibility
	// of this function to ensure the request is properly disposed after being 
	// processed unless leave occurs.
    TAccept result = aRequest->VisitStateL(*iState);

    switch(result)
        {
        case EDeferred:
            // The visited state cannot process the request at the moment.  Activate the 
            // request's timer and transfer ownership of the request to the request store.  
            // Hopefully, a state change happens before timeout occurs, when the new state 
            // will attempt to process all deferred request.  If timeout occurs before 
            // a change of state, the request will be completed with its' timeout error code.  
            // This timeout error code can be set thru the CCntRequest API SetTimeOutError.
            aRequest->ActivateTimeOutL(ReqStoreL());
            ReqStoreL().AppendL(aRequest);  // ownership transferred
            break;

        case EProcessed:
			// The request has been processed by the visited state - nothing more
			// to do, except to destroy the request now
            delete aRequest;
            break;

        case EOwnershipPassed:
            // the request has been hi-jacked by the visited state, the new owner will
            // assume responsibility of cleanup, no need to do anything here.
        default:
            break;
        }
    }


/**
 Allow the active state to process the event. The only state that actually
 processes events is the Transaction State. All other states (via the base state CState) 
 simply propagate the event back to the CCntDbManager, from where it is propogated to
 each CCntSession contained within the CCntDbManager.
 
 @param aEvent The database event that is being propagated.
*/ 
void CCntStateMachine::HandleDatabaseEventV2L(TContactDbObserverEventV2 aEvent)
	{
	#if defined(__PROFILE_DEBUG__)
		RDebug::Print(_L("[CNTMODEL] MTD: CCntStateMachine::HandleDatabaseEventV2L"));
	#endif 
	
	iState->HandleDatabaseEventV2L(aEvent);
	}

/** 
 Create the state machines request store. When a request cannot be processed in
 the active state, it is deferred until the active state changes or a contact item is unlocked.
 That is the request in this ReqStore is processed by the state machine - again - after either 
 a state change or unlock operation. 
 The store takes a reference to the CCntStateMachine in order to ProcessRequestL
 
 @return The Request Store  	
*/ 
CRequestStore& CCntStateMachine::ReqStoreL()
	{
	if (!iReqStore)
		{
		iReqStore = CRequestStore::NewL(*this);	
		}
	return *iReqStore;	
	}	
