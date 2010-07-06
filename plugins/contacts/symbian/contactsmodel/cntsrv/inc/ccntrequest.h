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



 
#ifndef __CCNTREQUEST_H__
#define __CCNTREQUEST_H__

#include <e32base.h>
#include <cntdb.h>

#include "cactiveloop.h"

enum TAccept		// TAccept is the return value for CState::AcceptRequestL methods
	{
	EProcessed = 0,	    // The State object has processed the request
	EDeferred,		    // The State object cannot processed the request at this moment and wish to defer processing
	EOwnershipPassed    // The State object has seized the request from its owner
	};
	
const TInt KFirstRequest = 0;
const TInt KOneSecond 	 = 1000000;
const TInt KSixtySeconds = 60000000;

class CState;
class CCntPackager;
class CContactItem;
class CCntStateMachine;
class CCntRequestTimer;
class CCntServerSpeedDialManager;
class CCntServerSpeedDialTable;
class MIniFileManager;
class CRequestStore;

/**
 These request classes are based on the Command Pattern
 All request class derive from the base class CCntRequest and implement
 a VisitStateL method. The VisitStateL method is called by the 
 CStateMachine class. When invoking the VisitStateL on a request object, 
 the CStateMachine provides the current active state class (a class derived from CState).
*/

class CCntRequest : public CBase
	{
public:
	// Complete the RMessage
	virtual void Complete(TInt aErrorCode = KErrNone);

	// Base declaration of the visitor pattern method.
	virtual TAccept VisitStateL(CState& aState) = 0; 
	virtual ~CCntRequest();
	inline const TUint SessionId();
	inline const TInt  TimeOut();
	// Request timeout methods - requests timeout when the 
	// statemachine is not able to process the request.
	void ActivateTimeOutL (CRequestStore& aReqStore);	
	void DeActivateTimeOut();	
	void SetTimeOutError  (TInt aError);
	TInt TimeOutError() const;
	
protected:
	CCntRequest(const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut);
	inline CCntRequest(const TUint aSessionId);
	inline CCntRequest();

protected:
	RMessage2 	iMessage;
	const TUint iSessionId;

private:
	// A request is only valid for a set period of time.
	// if not completed within this given time, the request object
	// should be completed and destroyed.
	const TInt  		iTimeOut;
	TInt				iTimeOutError;
	CCntRequestTimer*	iTimer;
	};


/**
   The CReqInternal class is the parent class for a group of special request classes.
   These classes are internal classes, they are NOT created in response to a Contact Model
   client proxy request. That is, classes derived from CReqInternal don't originate 
   in the Session. They contain neither a message nor a session ID.
*/
class CReqInternal : public CCntRequest
	{
public:
	// Override these methods so that they do not refer to iMessage which does
	// not exist for internal requests.
	void Complete(TInt aErrorCode = KErrNone);
	
protected:	
	~CReqInternal();
	CReqInternal();
	CReqInternal(const TUint aSessionId);
	};




/**
 When a request is deferred by the state machine it can only live for a give time
 The CCntRequestTimer monitors the life of a deferred request.
*/ 
class CCntRequestTimer : CTimer
	{
public:
	static CCntRequestTimer* NewL(CCntRequest& aRequest, CRequestStore& aReqStore);
	~CCntRequestTimer();
	void Start();
	void Stop ();
private:
	CCntRequestTimer(CCntRequest& aRequest, CRequestStore& aReqStore);
	void RunL();
	void ConstructL();
private:
	CCntRequest& iRequest;		
	// After a request timeout it must be removed from the request store
	// refer to note 3 in the CCntRequest.cpp
	CRequestStore& iReqStore; 
							  
	};


/**
 Open a file asyncronously - syncronous open works in the same manner as
 async open and also result in this request being created.
*/ 
class CReqAsyncOpen : public CCntRequest
	{
public:	
	static CReqAsyncOpen* NewLC(const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut);

	TAccept VisitStateL(CState& aState);
	// The FileManagerController can modify the filename 
	// - it is responsible for setting the default name
	TDes& FileName(); 
	
protected:
	inline CReqAsyncOpen(const TUint aSessionId);
	
private:
	CReqAsyncOpen(const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut);
	void ConstructL();
	
protected:
	TFileName iFilename;	
	};


/** Reopen the database file - Note that unlike the CReqAsyncOpen class the CReqReOpen does not contain
    a filename. Reopen opens the last opened file and does not have a filename. 
*/ 
class CReqReOpen : public CCntRequest
	{
public:	
	static CReqReOpen* NewLC(const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut);
	TAccept VisitStateL(CState& aState);

private:
	CReqReOpen(const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut);
	};


/**
  Close the database tables
*/ 
class CReqCloseTables : public CCntRequest
	{
public:	
 	static CReqCloseTables* NewLC(const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut);
  	TAccept VisitStateL(CState& aState);
  
private:
  	CReqCloseTables(const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut);
 
  	};

	
// Database Transaction Request Classes	
	
/**
 Request to begin a transaction	
*/ 
class CReqDbBeginTrans : public CCntRequest
	{
public:	
	static CReqDbBeginTrans* NewLC(const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut);
	TAccept VisitStateL(CState& aState);

private:
	CReqDbBeginTrans (const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut);
	};	
	
	
/**
 Request to Commit a transaction	
*/ 
class CReqDbCommitTrans : public CCntRequest
	{
public:	
	static CReqDbCommitTrans* NewLC(const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut);
	TAccept VisitStateL(CState& aState);

private:
	CReqDbCommitTrans(const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut);
	};
	
	
	
/**
 Request to Rollback a transaction
*/ 
class CReqDbRollbackTrans : public CCntRequest
	{
public:	
	static CReqDbRollbackTrans* NewLC(const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut);
	TAccept VisitStateL(CState& aState);

private:
	CReqDbRollbackTrans(const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut);
	};
	
	





/** 
 Parent Class for CReqUpdateCnt & CReqCommitCnt
*/
class CReqUpdate : public CCntRequest
	{
public:	
	TAccept VisitStateL(CState& aState) = 0;
	~CReqUpdate();
	inline CContactItem& Item();

protected:
	CReqUpdate(const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut);
	void ConstructL(CCntPackager& aPackager);

private:
	CContactItem* iCntItem;
	};



/** 
 Update a contact item - used in conjunction with Read
*/
class CReqUpdateCnt : public CReqUpdate
	{
public:	
	static CReqUpdateCnt* NewLC(const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut, CCntPackager& aPackager);
	TAccept VisitStateL(CState& aState);
	~CReqUpdateCnt(){};

private:
	CReqUpdateCnt(const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut);
	};




class CReqSetSpeedDial : public CCntRequest
	{
public:	
	static CReqSetSpeedDial* NewLC(	const TUint aSessionId, 
									const RMessage2& aMessage, 
									const TInt aTimeOut, 
									CContactItemViewDef& aItemViewDef,
									CCntServerSpeedDialManager& aSpeedDialManager,
									const CCntServerSpeedDialTable& aTable,
									MIniFileManager& aIniFileManager);
	TAccept VisitStateL(CState& aState);
	~CReqSetSpeedDial(){};
	const CContactItemViewDef& ItemViewDef();
	TInt SpeedDialIndex();
	TContactItemId TheContactId();
	TInt TheFieldIndex();
	CCntServerSpeedDialManager& SpeedDialManager();
	const CCntServerSpeedDialTable& SpeedDialTable();
	MIniFileManager& IniFileManager();
private:
	CReqSetSpeedDial(	const TUint aSessionId, 
						const RMessage2& aMessage, 
						const TInt aTimeOut, 
						CContactItemViewDef& aItemViewDef,
						CCntServerSpeedDialManager& aSpeedDialManager,
						const CCntServerSpeedDialTable& aTable,
						MIniFileManager& aIniFileManager);
	void ConstructL();
	CContactItemViewDef* iItemViewDef;
	TInt iSpeedDialIndex;
	TContactItemId iContactId;
	TInt iTheFieldIndex;
	CCntServerSpeedDialManager& iSpeedDialManager;
	const CCntServerSpeedDialTable& iTable;
	MIniFileManager& iIniFileManager;
	};



class CReqSetOwnCard : public CReqUpdate
	{
public:	
	static CReqSetOwnCard* NewLC(	const TUint aSessionId, 
									const RMessage2& aMessage, 
									const TInt aTimeOut, 
									CCntPackager& aPackager,
									CContactItemViewDef& aItemViewDef,
									MIniFileManager& aIniFileManager);
	TAccept VisitStateL(CState& aState);
	~CReqSetOwnCard(){};
	const CContactItemViewDef& ItemViewDef();
	TContactItemId TheContactId();
	MIniFileManager& IniFileManager();


private:
	CReqSetOwnCard(	const TUint aSessionId, 
						const RMessage2& aMessage, 
						const TInt aTimeOut, 
						CContactItemViewDef& aItemViewDef,
						MIniFileManager& aIniFileManager);
	CContactItemViewDef* iItemViewDef;
	TContactItemId iContactId;
	MIniFileManager& iIniFileManager;
	};
	
	


/**
 Commit a contact item - used in conjunction with Open
*/ 
class CReqCommitCnt : public CReqUpdate
	{
public:	
	static CReqCommitCnt* NewLC(const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut, CCntPackager& aPackager);
	TAccept VisitStateL(CState& aState);
	~CReqCommitCnt(){};

private:
	CReqCommitCnt(const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut);
	};




/**
 Parent Class for CReqOpenCnt & CReqReadCnt
*/ 
class CReqRead : public CCntRequest
	{
public:	
	TAccept VisitStateL(CState& aState) = 0;
	~CReqRead();
	const CContactItemViewDef& ItemViewDef();
	inline TContactItemId CntItemId() const;
	void CompleteL(const CContactItem& aCntItem);

protected:
	CReqRead(const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut, CCntPackager& aPackager, CContactItemViewDef& aItemViewDef);
	void ConstructL();

private:
	CCntPackager& iPackager;
	CContactItemViewDef* iItemViewDef;
	TBool iViewDefCreated;
	};


/**
 Stop opening the database 
*/ 
class CReqCancelAsyncOpen : public CCntRequest
	{
public:
	static CReqCancelAsyncOpen* NewLC(const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut);
	TAccept VisitStateL(CState& aState);
	
protected:
	inline CReqCancelAsyncOpen(const TUint aSessionId);

private:
	CReqCancelAsyncOpen(const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut);
	};



/**
 Open a contact item
*/ 
class CReqOpenCnt : public CReqRead
	{
public:	
	static CReqOpenCnt* NewLC(const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut, CCntPackager& aPackager, CContactItemViewDef& aItemViewDef);
	TAccept VisitStateL(CState& aState);
	~CReqOpenCnt();

private:
	CReqOpenCnt(const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut, CCntPackager& aPackager, CContactItemViewDef& aItemViewDef);
	};






/** 
 Read a contact item
*/
class CReqReadCnt : public CReqRead
	{
public:	
	static  CReqReadCnt* NewLC(const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut, CCntPackager& aPackager, CContactItemViewDef& aItemViewDef);
	TAccept VisitStateL(CState& aState);
	~CReqReadCnt();

private:
	CReqReadCnt(const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut, CCntPackager& aPackager, CContactItemViewDef& aItemViewDef);
	};




/**
 Delete a contact item
*/ 
class CReqDeleteCnt : public CCntRequest
	{
public:	
	static CReqDeleteCnt* NewLC(const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut);
	TAccept VisitStateL(CState& aState);
	~CReqDeleteCnt();
	inline TContactItemId CntItemId() const;
	inline TCntSendEventAction NotificationEventAction() const;

private:
	CReqDeleteCnt(const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut);
	};


// Close a contact item - Used in conjunction with open
class CReqCloseCnt : public CCntRequest
	{
public:	
	static CReqCloseCnt* NewLC(const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut);
	TAccept VisitStateL(CState& aState);
	~CReqCloseCnt();
	inline TContactItemId CntItemId()const;

private:
	CReqCloseCnt(const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut);
	};



// Create a contact item
class CReqCreateCnt : public CCntRequest
	{
public:	
	static CReqCreateCnt* NewLC(const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut, CCntPackager& aPackager);
	TAccept VisitStateL(CState& aState);
	~CReqCreateCnt();
	inline CContactItem& Item() const;
private:
	void ConstructL(CCntPackager& aPackager);
	CReqCreateCnt(const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut);
private:
	CContactItem* iCntItem;
	};


/** Backup and Restore requests.*/

/** Backup/restore beginning request.
*/
class CReqBackupRestoreBegin : public CReqInternal
	{
public:	
	static CReqBackupRestoreBegin* NewLC();
	inline ~CReqBackupRestoreBegin();
	TAccept VisitStateL(CState& aState);
	
private:
	inline CReqBackupRestoreBegin();
	};

/** Backup or restore completed request.
*/
class CReqBackupRestoreEnd : public CReqInternal
	{
public:	
	static CReqBackupRestoreEnd* NewLC();
	inline ~CReqBackupRestoreEnd();
	TAccept VisitStateL(CState& aState);
	
private:
	inline CReqBackupRestoreEnd();
	};



/** Low disk space requests.*/


/** Low disk space request.
*/
class CReqDiskSpaceLow : public CReqInternal
	{
public:	
	static CReqDiskSpaceLow* NewLC();
	inline ~CReqDiskSpaceLow();
	TAccept VisitStateL(CState& aState);
	
private:
	inline CReqDiskSpaceLow();
	};

	
/** Normal (i.e. not low) disk space request.
*/
class CReqDiskSpaceNormal : public CReqInternal
	{
public:	
	static CReqDiskSpaceNormal* NewLC();
	inline ~CReqDiskSpaceNormal();
	TAccept VisitStateL(CState& aState);
	
private:
	inline CReqDiskSpaceNormal();
	};


/** Async activity requests.*/


/** Indicate async activity request.
*/
class CReqAsyncActivity : public CCntRequest
	{
public:	
	static CReqAsyncActivity* NewLC(const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut);
	TAccept VisitStateL(CState& aState);
	
private:
	CReqAsyncActivity(const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut);
	};

	
/** Indicate no async activity request.
*/
class CReqNoAsyncActivity : public CCntRequest
	{
public:	
	static CReqNoAsyncActivity* NewLC(const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut);
	TAccept VisitStateL(CState& aState);
	
private:
	CReqNoAsyncActivity(const TUint aSessionId, const RMessage2& aMessage, const TInt aTimeOut);
	};


/* The CReqInternalSessionUnlock is created when a session closes. Any locked contact
   items that the session left locked are removed by this internal request.
   The CReqInternalSessionUnlock is called from the session destructor
*/
class CReqInternalSessionUnlock : public CReqInternal
	{
public:	
	static CReqInternalSessionUnlock* NewLC(TInt aSessionId);
	TAccept VisitStateL(CState& aState);
	inline ~CReqInternalSessionUnlock();

private:
	inline CReqInternalSessionUnlock(TInt aSessionId);
	};
	
/* Open a file asyncronously - this one is used within the server,
   so it does not need to hold a RMessage to Complete().
*/
class CReqInternalAsyncOpen : public CReqAsyncOpen
	{
public:	
	static CReqInternalAsyncOpen* NewLC(const TUint aSessionId, const TDesC& aFileName, TRequestStatus& aStatus);

	// Override these methods so that they do not refer to iMessage which does
	// not exist for this internal request.
	void Complete(TInt aErrorCode = KErrNone);
	
private:
	CReqInternalAsyncOpen(const TUint aSessionId, const TDesC& aFileName, TRequestStatus& aStatus);
	void ConstructL();
	
private:
	TRequestStatus* iStatus;
	};	

// Stop opening the database - this version is used internal to the server
class CReqCancelInternalAsyncOpen : public CReqCancelAsyncOpen
	{
public:
	static CReqCancelInternalAsyncOpen* NewLC(const TUint aSessionId);
	
	// Override these methods so that they do not refer to iMessage which does
	// not exist for this internal request.
	void Complete(TInt aErrorCode = KErrNone);

private:
	inline CReqCancelInternalAsyncOpen(const TUint aSessionId);
	};



/**
 Store for deferred requests, which could not be processed by the state machine.
*/ 
class CRequestStore : public CBase, public MActiveLoopCallBack 
	{
public:
	static CRequestStore* NewL(CCntStateMachine& aStateMachine);
	~CRequestStore();
	void  AppendL   (CCntRequest* aRequest);
	TBool IsEmpty ();
	void  ActivateRequestsL();	
	TBool DoStepL();
	void  DoError(TInt aError);
	void  RemoveD(CCntRequest* aRequest);

private:
	CRequestStore(CCntStateMachine& aStateMachine);
	CCntRequest* Request();
	
private:	
	CCntStateMachine& 		   iStateMachine;
	RPointerArray<CCntRequest> iStore;
	CActiveLoop* 			   iActive;
	CCntRequest* 			   iCurrentRequest;
	TInt					   iNoOfRequests;
	};




inline const TUint CCntRequest::SessionId()  
	{ 
	return iSessionId;
	}


inline const TInt CCntRequest::TimeOut()
    { 
    return iTimeOut;
    }


/**
 This base constructor is used by derived request classes that represent requests
 originating outside the server - from the client session
*/ 
inline CCntRequest::CCntRequest(const TUint aSessionId):iSessionId(aSessionId), iTimeOut(0)
	{
	}
	
/**
 This base constructor is used by derived request class that represent requests
 originating FROM WITHIN the server 
*/
inline CCntRequest::CCntRequest():iSessionId(0), iTimeOut(0)
	{
	}

/**
 The constructor for the parent class of all requests originating within the server
*/
inline CReqInternal::CReqInternal()
	{
	};
	
inline CReqInternal::CReqInternal(const TUint aSessionId) : 
	CCntRequest(aSessionId)
	{
	};
	
inline CReqInternal::~CReqInternal()
	{
	};

inline CContactItem& CReqUpdate::Item() 
	{
	return *iCntItem;
	}

/**
 Get the contact ID
*/
inline TContactItemId CReqRead::CntItemId() const
	{
	return iMessage.Int2();
	}

inline TContactItemId CReqDeleteCnt::CntItemId()const
	{
	return iMessage.Int0();
	}

inline TCntSendEventAction CReqDeleteCnt::NotificationEventAction() const
    {
    return static_cast<TCntSendEventAction>(iMessage.Int1());
    }

inline TContactItemId CReqCloseCnt::CntItemId()const
 	{
 	return iMessage.Int0();
 	}

/**
 Get the Contact Item
*/
inline CContactItem& CReqCreateCnt::Item() const 
	{ 
	return *iCntItem;
	}

inline CReqBackupRestoreBegin::CReqBackupRestoreBegin()
	{
	}

inline CReqBackupRestoreEnd::CReqBackupRestoreEnd()
	{
	}

inline CReqDiskSpaceLow::CReqDiskSpaceLow()
	{
	}

inline CReqDiskSpaceNormal::CReqDiskSpaceNormal()
	{
	}

inline CReqInternalSessionUnlock::~CReqInternalSessionUnlock()
	{
	}

inline CReqInternalSessionUnlock::CReqInternalSessionUnlock(TInt aSessionId) : CReqInternal (aSessionId) 
	{
	}

inline CReqDiskSpaceNormal::~CReqDiskSpaceNormal()
	{
	}

inline CReqDiskSpaceLow::~CReqDiskSpaceLow()
	{
	}

inline CReqBackupRestoreEnd::~CReqBackupRestoreEnd()
	{
	}

inline CReqBackupRestoreBegin::~CReqBackupRestoreBegin()
	{
	}

inline CReqAsyncOpen::CReqAsyncOpen(const TUint aSessionId) : CCntRequest(aSessionId)
	{
	}

inline CReqCancelAsyncOpen::CReqCancelAsyncOpen(const TUint aSessionId) : CCntRequest(aSessionId)
	{
	}

inline CReqCancelInternalAsyncOpen::CReqCancelInternalAsyncOpen(const TUint aSessionId) : CReqCancelAsyncOpen(aSessionId)
	{
	}

#endif //CCNTREQUEST_H
