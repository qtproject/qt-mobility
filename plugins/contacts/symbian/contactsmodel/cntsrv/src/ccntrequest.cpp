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
 @internalTechnology
 @released 
*/

#include "ccntrequest.h"
#include "ccntstatemachine.h"
#include "ccntpackager.h"
#include <cntitem.h>

// CCntRequest 
// CCntRequest is the base class for all Request classes that handle external
// requests (requests that originate from the CContactDatabase). Internal requests
// derive from the CReqInternal class. 
// Note 1: All unpacking of objects should be done within Construction.
// Note 2: The destruction of request objects is a little unusual.
//		   The destruction of requests objects is usually the responsibility of the CStatemachine or in the CRequestStore class.
//         The destruction of requests is usually performed using the CCntRequest::CompletePD method. See below
// Note 3: Each request has a timeout member - the time for which the request remains valid - and a timeouterror member variable.
//		   The iTimeOutError is the error that will be returned to the client in the event of the request timing out. The default 
//		   timeout value is KErrNotReady but the CStateMachine can reset this depending on the last action the statemachine attempted
// 		   to perform before the request timedout.
//		   Timeouts work as follows - the statemachine returns TDeferred indicating it could not process the request in the current state
//		   The request is added to the request store, CRequestStore, and the timeout is activated via the CCntRequest::ActivateTimeOutL 		
//		   method. There are two possible outcomes:
//			1. The state changes, or a contact item is unlocked, and the request is activated and processed by the CStateMachine		
//			2. The request times out, CCntRequestTimer::RunL is called and the request is completed with the iTimeOutError error code.

/** 
  Base CCntRequest constructor
  @param  aSessionId The session from which the request originated
  @param  aMessage The RMessage from the client
  @param  aTimeOut The lenght of time in milliseconds for which the request remains valid
*/

CCntRequest::CCntRequest(const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut)
						:iMessage(aMessage),
						 iSessionId(aSessionId), 
						 iTimeOut(aTimeOut),
						 iTimeOutError(KErrNotReady)
	{
	ASSERT(aSessionId); // Should never be NULL
	// All internal requests do not have a session ID, such requests derive from the CReqInternal
	// and NOT from CCntRequest.
	}

/** 
  Complete the requests internal iMessage

  @param  aErrCode The error code with which the message will be completed
*/
void CCntRequest::Complete(TInt aErrCode)
	{
	iMessage.Complete(aErrCode); 
	}

CCntRequest::~CCntRequest()
	{
	delete iTimer;
	}

/** 
  Start the timeout 
  A timer is only ever added when it does not exist and a non-zero timeout value has
  been specified, because the request may be activated, processed and deferred more than once.
  The result of request processing could be to add the request to the request queue again. 
 
  @param  aReqStore When a times out, the timer access the store to complete the request with
  	 		a timeout error.
 */
void CCntRequest::ActivateTimeOutL(CRequestStore& aReqStore)
	{
	if ((iTimer == NULL) && (iTimeOut != 0))
		{
		iTimer = CCntRequestTimer::NewL(*this, aReqStore);
		iTimer->Start();
		}
	}

/**
  Stop the timeout
 */
void CCntRequest::DeActivateTimeOut()
	{
	if (iTimer)
		{
		iTimer->Stop();	
		}
		
	delete iTimer;
	iTimer = NULL;
	}

/**
  Set the timeout error code
  Inorder to assure binary compatibility with the original Contacts Model,
  which did not retry processing client requests, the error code - reason 
  why an request can not be processed - needs to be stored and returned to
  to the client if a subsequest/retry to process the request fails.
  For example if a contact item has been locked by one session, a request
  to open the contact item by a second session should complete with KErrInUse
  however if the server has been put into a transaction state by one session,
  the seconds session request should complete with KErrNotReady 
  KErrNotReady is the default error timeout value set by the base state CState
  it is also be set the CCntRequest constructor
 
  @param  aError The error code with which the message will be completed if it times out 
  
 */ 
void CCntRequest::SetTimeOutError(TInt aError)
	{
	iTimeOutError = aError;
	}
	
/**
 Get the timeout error code	
 @return The timeout error code
*/ 
TInt CCntRequest::TimeOutError()const
	{
	return iTimeOutError;
	}


/** 
  Does not complete a RMessage - Internal Request do not contain a RMessage
  as they don't originate from the client.  Do nothing.
  @param  aErrorCode Not used
*/
void CReqInternal::Complete(TInt /* aErrorCode*/) 
	{
	}


// Request Helper classes
// -----------------------
// CCntRequestTimer
// CRequestStore

/** 
  CCntRequestTimer constructor
  CCntRequestTimer is an active object derived from CTimer
  @param aRequest The request which will timeout
  @param aReqStore The array that holds the deferred request    
  				   The request must be removed from the request store after 
  				   it is completed with an error code.
*/
CCntRequestTimer* CCntRequestTimer::NewL(CCntRequest& aRequest, CRequestStore& aReqStore)
	{
	CCntRequestTimer* self = new (ELeave) CCntRequestTimer(aRequest, aReqStore);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
	
CCntRequestTimer::~CCntRequestTimer()
	{
	CTimer::Cancel();
	}
	
	
CCntRequestTimer::CCntRequestTimer(CCntRequest& aRequest, CRequestStore& aReqStore) 
				 : CTimer(CActive::EPriorityIdle), 
				 iRequest(aRequest), 
				 iReqStore(aReqStore) 
	{								  
	}


void CCntRequestTimer::ConstructL()
	{
	CTimer::ConstructL();
	CActiveScheduler::Add(this);
	}

void CCntRequestTimer::RunL()
	{
	// Complete the contact with the timeout error
	iRequest.Complete(iRequest.TimeOutError());
	CTimer::Cancel();
	iReqStore.RemoveD(&iRequest); 
	}

void CCntRequestTimer::Start()
	{ 
	CTimer::After(iRequest.TimeOut());
	}
		
void CCntRequestTimer::Stop()
	{
	CTimer::Cancel();
	}




/**
 Object are added when the session requests a Transaction to begin
 but the database is already in a transaction state for another session
 There can only ever handle one transaction request - no concurrency!

 @param aStateMachine The request store calls the statemachine to process the
 					  request.
*/ 
CRequestStore* CRequestStore::NewL(CCntStateMachine& aStateMachine)
	{
	CRequestStore* self = new (ELeave) CRequestStore(aStateMachine);
	return self;
	}

CRequestStore::CRequestStore(CCntStateMachine& aStateMachine): 
				iStateMachine(aStateMachine)
	{
	}

CRequestStore::~CRequestStore()
	{
	iStore.ResetAndDestroy();
	delete iActive;
	}

/**
 Remove the request from the store and delete the request
 
 @param aRequest The request that should be removed from the store and deleted
 				 The Request Store takes ownership of this object
*/
void CRequestStore::RemoveD(CCntRequest* aRequest)
	{
	TInt index = iStore.Find(aRequest);
	if (index > KErrNotFound)
		{
		iStore.Remove(index);	
		}
	delete aRequest;
	aRequest = NULL;
	}


/**
 Append a request to the request store

 @param aRequest  The request that is to be appended to the store
*/	
void CRequestStore::AppendL(CCntRequest* aRequest)
	{
	// The heap is safe since the request is appended to the contained store.
	// Pop as the stack will be out of sync when the statemachine processes 
	// the request.
	User::LeaveIfError(iStore.Append(aRequest));
	}

/** 
 Get the first request from the store 

 @return The request a the beginning of the stores array
 		 Ownership of the request is given to the calling method
*/
CCntRequest* CRequestStore::Request()
	{
	CCntRequest* request = iStore[KFirstRequest]; 
	iStore.Remove(KFirstRequest);
	
	return request;     // Return ownership
	}

/**
 Is there requests in the store
 
 @return ETrue if there are no request, EFalse otherwise
*/
TBool CRequestStore::IsEmpty()
	{
	if (iStore.Count() > 0)
		return EFalse;
	
	return ETrue;
	}

/**
  Activate the store.
  The request store uses the CActiveLoop for callbacks on each iteration 
  of the CActiveLoop only one request is processed until all
  requests have been processed.
 */
void CRequestStore::ActivateRequestsL()
	{
	if (!iActive)
		{
		iActive = CActiveLoop::NewL();
		}
	
	// Only process the requests in the store when it has been activated.
	// Don't process any requests added to the store after it has been activated.
	// Requests may be readded to the store & should not be processed twice 
	// in the same batch.
	// The store may already have been activated. 
	if ((!iActive->IsActive()) && (iNoOfRequests == 0))
		{
		iActive->Register(*this);	
		iNoOfRequests = iStore.Count();
		}
	}

/**
 Process the next request in the store.	
 If the request has timed out return an error
 otherwise process the request as normal.
 
 @ return ETrue if another step is required, EFalse if this is the last step
 */
TBool CRequestStore::DoStepL()
	{
	ASSERT(iCurrentRequest == NULL);

	iCurrentRequest = Request();
	iStateMachine.ProcessRequestL(iCurrentRequest); // ownership transferred
	
	// ProcessRequestL received ownership of the request, iCurrentRequest can be 
	// reset to NULL
	iCurrentRequest = NULL;
	
	--iNoOfRequests;

	if (iNoOfRequests == 0) 
		{
		return EFalse; // The store has tried to process all requests it holds	
		}

	return ETrue;
	}
	
/** 
 Error callback from the CActiveLoop class.	
 
 @ param aError The error to complete the current request (the request that is
 				being processed)
*/
void CRequestStore::DoError(TInt aError)
	{
	// there should always be a current request
	ASSERT(iCurrentRequest);
	if (iCurrentRequest)
		{
		iCurrentRequest->Complete(aError);
		delete iCurrentRequest;
		
		iCurrentRequest = NULL;
		}
	}


/** 
 CReqAsyncOpen constructor
 Request to open the database asyncronously 
 
 @see CCntRequest constructor   
*/
CReqAsyncOpen* CReqAsyncOpen::NewLC(const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut)
	{
	CReqAsyncOpen* self = new (ELeave) CReqAsyncOpen(aSessionId, aMessage, aTimeOut);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CReqAsyncOpen::CReqAsyncOpen(const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut )
: CCntRequest(aSessionId, aMessage, aTimeOut)
	{
	}

void CReqAsyncOpen::ConstructL()
	{
	iMessage.ReadL(0,iFilename);
	}

/**
 Get the name of the file that is to be opened
 The DBManager may change this name. If no name has been passed by the client
 then the default is set by the manager
 
 @return The name of the file that should be opened
*/ 
TDes& CReqAsyncOpen::FileName()
	{
	return iFilename;
	}

TAccept CReqAsyncOpen::VisitStateL(CState& aState)
	{
	return aState.AcceptRequestL(this);
	}

/**
 CReqCloseTables constructor
 Close the database tables

 @see CCntRequest constructor    
*/ 
CReqCloseTables* CReqCloseTables::NewLC(const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut)
  	{
  	CReqCloseTables* self = new (ELeave) CReqCloseTables(aSessionId, aMessage, aTimeOut);
  	CleanupStack::PushL(self);
  	return self;
  	};
  
CReqCloseTables::CReqCloseTables(const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut)
  :CCntRequest(aSessionId, aMessage, aTimeOut)
  	{
  	}
  
  
TAccept CReqCloseTables::VisitStateL(CState& aState)
  	{
  	return aState.AcceptRequestL(this);
  	}
  
  
/** 
  CReqReOpen 
  Re Open the database
  
  @see CCntRequest constructor    
*/
CReqReOpen* CReqReOpen::NewLC(const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut)
	{
	CReqReOpen* self = new (ELeave) CReqReOpen(aSessionId, aMessage, aTimeOut);
	CleanupStack::PushL(self);
	return self;
	}

CReqReOpen::CReqReOpen(const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut)
:CCntRequest(aSessionId, aMessage, aTimeOut)
	{
	}

TAccept CReqReOpen::VisitStateL(CState& aState)
	{
	return aState.AcceptRequestL(this);
	}


/**
 CReqCancelAsyncOpen 
 Cancel an asyncronous open request
 
 @see CCntRequest constructor    
*/ 
CReqCancelAsyncOpen* CReqCancelAsyncOpen::NewLC(const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut)
	{
	CReqCancelAsyncOpen* self = new (ELeave) CReqCancelAsyncOpen(aSessionId, aMessage, aTimeOut);
	CleanupStack::PushL(self);
	return self;
	}

CReqCancelAsyncOpen::CReqCancelAsyncOpen(const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut)
:CCntRequest(aSessionId, aMessage, aTimeOut)
	{
	}
	
TAccept CReqCancelAsyncOpen::VisitStateL(CState& aState)
	{
	return aState.AcceptRequestL(this);
	}



/**
 CReqUpdate 
 Parent Class for CReqUpdateCnt & CReqCommitCnt
 
 @see CCntRequest constructor   
*/
void CReqUpdate::ConstructL(CCntPackager& aPackager)
	{
	aPackager.SetBufferFromMessageL(iMessage);
	iCntItem = aPackager.UnpackCntItemLC();
	CleanupStack::Pop(iCntItem);
	}

CReqUpdate::~CReqUpdate()
	{
	delete iCntItem;
	}

CReqUpdate::CReqUpdate(const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut)
:CCntRequest(aSessionId, aMessage, aTimeOut)
	{
	}



/** CReqUpdateCnt constructor
 Update a contact item - derived from CReqUpdate. Used in conjunction with CReqReadCnt. 
 Note: CReqCommitCnt is very similar to CReqUpdateCnt, but CReqCommitCnt is used with CReqOpenCnt
       and locks the contact. CReqUpdateCnt does not perform locking.
 
 @see CCntRequest constructor          
*/
CReqUpdateCnt* CReqUpdateCnt::NewLC(const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut, CCntPackager& aPackager)
	{
	CReqUpdateCnt* self = new (ELeave) CReqUpdateCnt(aSessionId, aMessage, aTimeOut);
	CleanupStack::PushL(self);
	self->ConstructL(aPackager);
	return self;
	}

CReqUpdateCnt::CReqUpdateCnt(const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut)
:CReqUpdate(aSessionId, aMessage, aTimeOut)
	{
	}

TAccept CReqUpdateCnt::VisitStateL(CState& aState)
	{
	return aState.AcceptRequestL(this);
	}
	
	
/** 
 CReqSetSpeedDial constructor
 Set a speed dial key
 
 @see CCntRequest constructor   
*/ 
CReqSetSpeedDial* CReqSetSpeedDial::NewLC(	const TUint aSessionId, 
									const RMessage2& aMessage, 
									const TInt aTimeOut, 
									CContactItemViewDef& aItemViewDef,
									CCntServerSpeedDialManager& aSpeedDialManager,
									const CCntServerSpeedDialTable& aTable,
									MIniFileManager& aIniFileManager)
	{
	CReqSetSpeedDial* self = new (ELeave) CReqSetSpeedDial(	aSessionId, 
															aMessage, 
															aTimeOut, 
															aItemViewDef,
															aSpeedDialManager,
															aTable,
															aIniFileManager);
	CleanupStack::PushL(self);
	self->ConstructL();															
	return self;
	}

CReqSetSpeedDial::CReqSetSpeedDial(	const TUint aSessionId, 
									const RMessage2& aMessage, 
									const TInt aTimeOut, 
									CContactItemViewDef& aItemViewDef,
									CCntServerSpeedDialManager& aSpeedDialManager,
									const CCntServerSpeedDialTable& aTable,
									MIniFileManager& aIniFileManager)
:CCntRequest(aSessionId, aMessage, aTimeOut), iItemViewDef(&aItemViewDef), iSpeedDialManager(aSpeedDialManager), iTable(aTable), iIniFileManager(aIniFileManager)
	{
	}
	
void CReqSetSpeedDial::ConstructL()
	{
	// Read the message
	// Read position
	iSpeedDialIndex = iMessage.Int0();
	// Read contact id
	iContactId = static_cast<TContactItemId> (iMessage.Int1());
	// Read Field Index
	iTheFieldIndex = iMessage.Int2();	
	}

TAccept CReqSetSpeedDial::VisitStateL(CState& aState)
	{
	return aState.AcceptRequestL(this);
	}
	
TInt CReqSetSpeedDial::SpeedDialIndex()
	{
	return iSpeedDialIndex;
	}

TContactItemId CReqSetSpeedDial::TheContactId()
	{
	return iContactId;
	}

TInt CReqSetSpeedDial::TheFieldIndex()
	{
	return iTheFieldIndex;	
	}
		
const CContactItemViewDef& CReqSetSpeedDial::ItemViewDef()
	{
	return *iItemViewDef;
	}
	
CCntServerSpeedDialManager& CReqSetSpeedDial::SpeedDialManager()
	{
	return iSpeedDialManager;	
	}
	
const CCntServerSpeedDialTable& CReqSetSpeedDial::SpeedDialTable()
	{
	return iTable;
	}
	
MIniFileManager& CReqSetSpeedDial::IniFileManager()
	{
	return iIniFileManager;
	}

/**
 CReqSetOwnCard constructor
 Set the own card
 
 @see CCntRequest constructor    
*/ 
CReqSetOwnCard* CReqSetOwnCard::NewLC(	const TUint aSessionId, 
									const RMessage2& aMessage, 
									const TInt aTimeOut,
									CCntPackager& aPackager, 
									CContactItemViewDef& aItemViewDef,
									MIniFileManager& aIniFileManager)
	{
	CReqSetOwnCard* self = new (ELeave) CReqSetOwnCard(	aSessionId, 
														aMessage, 
														aTimeOut, 
														aItemViewDef,
														aIniFileManager);
	CleanupStack::PushL(self);
	self->ConstructL(aPackager);															
	return self;
	}

CReqSetOwnCard::CReqSetOwnCard(	const TUint aSessionId, 
									const RMessage2& aMessage, 
									const TInt aTimeOut, 
									CContactItemViewDef& aItemViewDef,
									MIniFileManager& aIniFileManager)
:CReqUpdate(aSessionId, aMessage, aTimeOut), iItemViewDef(&aItemViewDef), iIniFileManager(aIniFileManager)
	{
	}
	


TAccept CReqSetOwnCard::VisitStateL(CState& aState)
	{
	return aState.AcceptRequestL(this);
	}
	
TContactItemId CReqSetOwnCard::TheContactId()
	{
	return iContactId;
	}
		
const CContactItemViewDef& CReqSetOwnCard::ItemViewDef()
	{
	return *iItemViewDef;
	}
	
	
MIniFileManager& CReqSetOwnCard::IniFileManager()
	{
	return iIniFileManager;
	}
	
/** 
 CReqCommitCnt constructor
 Commit a contact item - used in conjunction with CReqOpenCnt
 Derived from CReqUpdate, and is very similar to CReqUpdateCnt but also locks the contact item
 so no other session can modify it.
 
 @see CCntRequest constructor   
*/ 
CReqCommitCnt* CReqCommitCnt::NewLC(const TUint aSessionId, 
									const RMessage2& aMessage, 
									const TInt aTimeOut, 
									CCntPackager& aPackager)
	{
	CReqCommitCnt* self = new (ELeave) CReqCommitCnt(aSessionId, aMessage, aTimeOut);
	CleanupStack::PushL(self);
	self->ConstructL(aPackager);
	return self;
	}

CReqCommitCnt::CReqCommitCnt(const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut)
:CReqUpdate(aSessionId, aMessage, aTimeOut)
	{
	}

TAccept CReqCommitCnt::VisitStateL(CState& aState)
	{
	return aState.AcceptRequestL(this);
	}





/** CReqRead constructor   
 Parent class of CReqReadCnt & CReqOpenCnt
 CReqOpenCnt and CReqReadCnt are very similar 
 CReqOpenCnt results in the contact item being locked by the session, so no other session
 can modify the contact item.
 CReqReadCnt results in no locking.
 Note: The CRequest::CompleteL method is overridden in this class
 
 @see CCntRequest constructor    
*/ 
CReqRead::CReqRead(const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut, CCntPackager& aPackager, CContactItemViewDef& aItemViewDef)
	:CCntRequest(aSessionId, aMessage, aTimeOut), 
	iPackager(aPackager), 
	iItemViewDef(&aItemViewDef),
	iViewDefCreated(EFalse)
	{
	}
	
void CReqRead::ConstructL()
	{
	if (iMessage.Int3())
		{
		iPackager.SetBufferFromMessageL(iMessage);
		iItemViewDef = iPackager.UnpackCntItemViewDefLC();
		iViewDefCreated = ETrue;
		iPackager.Clear();
		CleanupStack::Pop(iItemViewDef);
		}
	}

CReqRead::~CReqRead()
	{
	if (iViewDefCreated)	// Required - determine CntItemViewdef was extracted from iMessage
		{					// encapsulates ItemViewDef nicely. 
		delete iItemViewDef;
		}	
	}

const CContactItemViewDef& CReqRead::ItemViewDef()
	{
	return *iItemViewDef;
	}

/** Overridden CompleteL method 
 Completes the request item by packing the contact item into the RMessage slot
 and returning it to the client
 
 @param aCntItem The contact item that is returned to the client.
*/ 
void CReqRead::CompleteL(const CContactItem& aCntItem)
	{
	TInt size(0);
	TPtr8 cntItemBuff = iPackager.PackL(aCntItem, size); 
	// Write data if client buffer is large enough
	// otherwise return the required buffer size.
	if(iMessage.GetDesMaxLength(1) >= size)
		{ 
		iMessage.WriteL(1, cntItemBuff); 
		Complete(KErrNone);
		}
	else
		{ 
		Complete(size);	
		}
	}
	

/** CReqOpenCnt constructor
 Open a Contact Item - derived from CReqRead. 
 Very similar to CReqReadCnt but also results in the contact item being locked

 @see CReqRead constructor   
*/ 
CReqOpenCnt* CReqOpenCnt::NewLC(const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut, CCntPackager& aPackager, CContactItemViewDef& aItemViewDef)
	{
	CReqOpenCnt* self = new (ELeave) CReqOpenCnt(aSessionId, aMessage, aTimeOut, aPackager, aItemViewDef);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CReqOpenCnt::CReqOpenCnt(const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut, CCntPackager& aPackager, CContactItemViewDef& aItemViewDef)
	:CReqRead(aSessionId, aMessage, aTimeOut, aPackager, aItemViewDef) 
	{
	}

CReqOpenCnt::~CReqOpenCnt()
	{
	}


TAccept CReqOpenCnt::VisitStateL(CState& aState)
	{
	return aState.AcceptRequestL(this);
	}


/**
 CReqReadCnt constructor
 Read a contact item - derived from CReqRead
 Very similar to CReqOpenCnt but does not result in locking the contact item

 @see CReqRead constructor   
 
*/ 
CReqReadCnt* CReqReadCnt::NewLC(const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut, CCntPackager& aPackager, CContactItemViewDef& aItemViewDef)
	{
	CReqReadCnt* self = new (ELeave) CReqReadCnt(aSessionId, aMessage, aTimeOut, aPackager, aItemViewDef);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CReqReadCnt::CReqReadCnt(const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut, CCntPackager& aPackager, CContactItemViewDef& aItemViewDef)
	:CReqRead(aSessionId, aMessage, aTimeOut, aPackager, aItemViewDef) 
	{
	}

CReqReadCnt::~CReqReadCnt()
	{
	}


TAccept CReqReadCnt::VisitStateL(CState& aState)
	{
	return aState.AcceptRequestL(this);
	}
	
	
	
/** 
 CReqDeleteCnt constructor
 Delete a contact Item
 
 @see CCntRequest constructor 
*/ 
CReqDeleteCnt* CReqDeleteCnt::NewLC(const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut)
	{
	CReqDeleteCnt* self = new (ELeave) CReqDeleteCnt(aSessionId, aMessage, aTimeOut);
	CleanupStack::PushL(self);
	return self;
	}

CReqDeleteCnt::CReqDeleteCnt(const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut)
:CCntRequest(aSessionId, aMessage, aTimeOut)
	{
	}

CReqDeleteCnt::~CReqDeleteCnt()
	{
	}

TAccept CReqDeleteCnt::VisitStateL(CState& aState)
	{
	return aState.AcceptRequestL(this);
	}
	
	
	
	
/** CReqCloseCnt 
 Close a contact Item - Used in conjunction with CReqOpenCnt to remove the lock on the contact item 
 without committing.
 
 @see CCntRequest constructor
*/ 
CReqCloseCnt* CReqCloseCnt::NewLC(const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut)
	{
	CReqCloseCnt* self = new (ELeave) CReqCloseCnt(aSessionId, aMessage, aTimeOut);
	CleanupStack::PushL(self);
	return self;
	}

CReqCloseCnt::CReqCloseCnt(const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut)
:CCntRequest(aSessionId, aMessage, aTimeOut)
	{
	}

CReqCloseCnt::~CReqCloseCnt()
	{
	}

TAccept CReqCloseCnt::VisitStateL(CState& aState)
	{
	return aState.AcceptRequestL(this);
	}	
	
/** 
  CReqCreateCnt constructor
  Create a contact item, this request is completed with the new contact item id or an error code.
  
  @see CCntRequest constructor
*/ 
CReqCreateCnt* CReqCreateCnt::NewLC(const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut, CCntPackager& aPackager)
	{
	CReqCreateCnt* self = new (ELeave) CReqCreateCnt(aSessionId, aMessage, aTimeOut);
	CleanupStack::PushL(self);
	self->ConstructL(aPackager);
	return self;
	}

CReqCreateCnt::CReqCreateCnt(const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut)
:CCntRequest(aSessionId, aMessage, aTimeOut)
	{
	}

CReqCreateCnt::~CReqCreateCnt()
	{
	delete iCntItem;
	}

TAccept CReqCreateCnt::VisitStateL(CState& aState)
	{
	#if defined(__PROFILE_DEBUG__)
		RDebug::Print(_L("[CNTMODEL] MTD: CCntStateMachine::ProcessRequestL"));
	#endif 	
	
	return aState.AcceptRequestL(this);
	}

void CReqCreateCnt::ConstructL(CCntPackager& aPackager)
	{
	aPackager.SetBufferFromMessageL(iMessage);
	iCntItem = aPackager.UnpackCntItemLC();
	CleanupStack::Pop(iCntItem);
	}




///////////////// Database Transactin Request Class implemenations ////////////


/** 
 CReqDbBeginTrans constructor
 Request to begin a transaction - moves the state machine into a transaction state	
 
 @see CCntRequest constructor
*/
CReqDbBeginTrans* CReqDbBeginTrans::NewLC(const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut)
	{
	CReqDbBeginTrans* self = new (ELeave) CReqDbBeginTrans(aSessionId, aMessage, aTimeOut);
	CleanupStack::PushL(self);
	return self;
	}
	
TAccept CReqDbBeginTrans::VisitStateL(CState& aState)
	{
	return aState.AcceptRequestL(this);
	}


	
CReqDbBeginTrans::CReqDbBeginTrans(const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut)
				 :CCntRequest(aSessionId, aMessage, aTimeOut)
	{
	}

/** 
 CReqDbCommitTrans constructor
 Request to Commit a transaction	
 Moves the database out of a transaction state and (usually) into a writable state
 
 @see CCntRequest constructor
 */
CReqDbCommitTrans* CReqDbCommitTrans::NewLC(const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut)
	{
	CReqDbCommitTrans* self = new (ELeave) CReqDbCommitTrans(aSessionId, aMessage, aTimeOut);
	CleanupStack::PushL(self);
	return self;	
	}
	
TAccept CReqDbCommitTrans::VisitStateL(CState& aState)
	{
	return aState.AcceptRequestL(this);	
	}

CReqDbCommitTrans::CReqDbCommitTrans(const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut)	
				  :CCntRequest(aSessionId, aMessage, aTimeOut)

	{
	}
	
	
/**
 CReqDbRollbackTrans constructor
 Request to Rollback a transaction - results in a database recovery 
 so it closes and re-opens the database if it has been damaged
 
 @see CCntRequest constructor 
*/ 
CReqDbRollbackTrans* CReqDbRollbackTrans::NewLC(const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut)
	{
	CReqDbRollbackTrans* self = new (ELeave) CReqDbRollbackTrans(aSessionId, aMessage, aTimeOut);
	CleanupStack::PushL(self);
	return self;	
	}
	
TAccept CReqDbRollbackTrans::VisitStateL(CState& aState)
	{
	return aState.AcceptRequestL(this);	
	}

CReqDbRollbackTrans::CReqDbRollbackTrans(const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut)
				    :CCntRequest(aSessionId, aMessage, aTimeOut)
	{
	}

	
/** 
 CReqBackupRestoreBegin constructor
 Notifys the state machine that a backup/restore is beginning

 @see CCntRequest constructor 
*/ 
CReqBackupRestoreBegin* CReqBackupRestoreBegin::NewLC()
	{
	CReqBackupRestoreBegin* self = new (ELeave) CReqBackupRestoreBegin();
	CleanupStack::PushL(self);
	return self;
	}

TAccept CReqBackupRestoreBegin::VisitStateL(CState& aState)
	{
	return aState.AcceptRequestL(this);
	}

/**
 CReqBackupRestoreEnd constructor
 Notifys the state machine that a restore has ended
 
 @see CCntRequest constructor 
 */
CReqBackupRestoreEnd* CReqBackupRestoreEnd::NewLC()
	{
	CReqBackupRestoreEnd* self = new (ELeave) CReqBackupRestoreEnd();
	CleanupStack::PushL(self);
	return self;
	}

TAccept CReqBackupRestoreEnd::VisitStateL(CState& aState)
	{
	return aState.AcceptRequestL(this);
	}


/**
 CReqDiskSpaceLow constructor
 @see CCntRequest constructor 
 */
CReqDiskSpaceLow* CReqDiskSpaceLow::NewLC()
	{
	CReqDiskSpaceLow* self = new (ELeave) CReqDiskSpaceLow();
	CleanupStack::PushL(self);
	return self;
	}

TAccept CReqDiskSpaceLow::VisitStateL(CState& aState)
	{
	return aState.AcceptRequestL(this);
	}


/** 
 CReqDiskSpaceNormal constructor
 @see CCntRequest constructor
*/
CReqDiskSpaceNormal* CReqDiskSpaceNormal::NewLC()
	{
	CReqDiskSpaceNormal* self = new (ELeave) CReqDiskSpaceNormal();
	CleanupStack::PushL(self);
	return self;
	}

TAccept CReqDiskSpaceNormal::VisitStateL(CState& aState)
	{
	return aState.AcceptRequestL(this);
	}


/**
 CReqAsyncActivity constructor
 @see CCntRequest constructor 
 */
CReqAsyncActivity* CReqAsyncActivity::NewLC(const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut)
	{
	CReqAsyncActivity* self = new (ELeave) CReqAsyncActivity(aSessionId, aMessage, aTimeOut);
	CleanupStack::PushL(self);
	return self;
	}

CReqAsyncActivity::CReqAsyncActivity(const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut )
: CCntRequest(aSessionId, aMessage, aTimeOut)
	{
	}

TAccept CReqAsyncActivity::VisitStateL(CState& aState)
	{
	return aState.AcceptRequestL(this);
	}


/** 
 CReqNoAsyncActivity constructor
 @see CCntRequest constructor
*/
CReqNoAsyncActivity* CReqNoAsyncActivity::NewLC(const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut)
	{
	CReqNoAsyncActivity* self = new (ELeave) CReqNoAsyncActivity(aSessionId, aMessage, aTimeOut);
	CleanupStack::PushL(self);
	return self;
	}

CReqNoAsyncActivity::CReqNoAsyncActivity(const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut )
: CCntRequest(aSessionId, aMessage, aTimeOut)
	{
	}

TAccept CReqNoAsyncActivity::VisitStateL(CState& aState)
	{
	return aState.AcceptRequestL(this);
	}

/**
  CReqInternalSessionUnlock constructor
  CReqInternalSessionUnlock unlocks all remaining locked contact items when
  a session closes. It is called from the session class destructor
  
  @see CCntReqInternal constructor  	
*/
CReqInternalSessionUnlock* CReqInternalSessionUnlock::NewLC(TInt aSessionId)
	{
	CReqInternalSessionUnlock* self = new (ELeave) CReqInternalSessionUnlock(aSessionId);
	CleanupStack::PushL(self);
	return self;		
	}
	
TAccept CReqInternalSessionUnlock::VisitStateL(CState& aState)
	{
	return aState.AcceptRequestL(this);		
	}

	
/////////////////////////CReqInternalAsyncOpen ////////////////////	
// Similar to CReqAsyncOpen but the request originates from inside the server.
CReqInternalAsyncOpen* CReqInternalAsyncOpen::NewLC(const TUint aSessionId, const TDesC& aFileName, TRequestStatus& aStatus)
	{
	CReqInternalAsyncOpen* self = new (ELeave) CReqInternalAsyncOpen(aSessionId, aFileName, aStatus);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
	
CReqInternalAsyncOpen::CReqInternalAsyncOpen(const TUint aSessionId, const TDesC& aFileName, TRequestStatus& aStatus)
 : CReqAsyncOpen(aSessionId), iStatus(&aStatus)
	{
	iFilename = aFileName;
	}

void CReqInternalAsyncOpen::ConstructL()
	{
	*iStatus = KRequestPending;	
	}

void CReqInternalAsyncOpen::Complete(TInt aErrorCode)
	{
	User::RequestComplete(iStatus, aErrorCode);
	}

/////////////////////////CReqCancelInternalAsyncOpen ////////////////////	
// Similar to CReqCancelAsyncOpen but the request originates from inside the server,
// so there is no need to signal a RMessage that the request has completed
CReqCancelInternalAsyncOpen* CReqCancelInternalAsyncOpen::NewLC(const TUint aSessionId)
	{
	CReqCancelInternalAsyncOpen* self = new (ELeave) CReqCancelInternalAsyncOpen(aSessionId);
	CleanupStack::PushL(self);
	return self;	
	}
	
void CReqCancelInternalAsyncOpen::Complete(TInt /*errorCode*/)
	{
	}

