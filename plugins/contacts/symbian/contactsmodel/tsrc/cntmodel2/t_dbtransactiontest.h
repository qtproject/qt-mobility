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



#ifndef __T_DB_TRANSACTION_TEST__
#define __T_DB_TRANSACTION_TEST__

#include <cntdb.h>
#include "nbcnttestlib.h"
#include <e32test.h>

namespace nsPlTransMgr 
{
	const TInt KOnce 	   = 1;
	_LIT(KFilename, "C:TransMgr.cdb");
	_LIT(KNumber,  "123456789");
	_LIT(KName,    "NAME");
	_LIT(KEmail,   "name@email.com");
	_LIT(KHomeAdd, "Home address");
}

enum TTestTransactionCodes
	{
	ECreateAndCommitTest,
	ERollbackAndRecoverTest,
	EDeleteAndCommitTest,
	EUpdateAndCommitTest,
	};

enum TTestCodes
	{
	ETransactionTest,
	ETimeOutTest,
	ENonTimeOutTest,
	EAsyncOpenTest
	};

const TTimeIntervalMicroSeconds32 KOneHalfSecondTimeOut =   500000;
const TTimeIntervalMicroSeconds32 KSixtySecondsTimeOut  = 60000000;

class CConcurrentTester;
class CConcurrentController;
// Transaction Tests
class CTransactionThreadTest;
// Request Timeout test classes
class CTransTimeOutThread;
class CReqTimeOutThread;


// Responsible for signalling the controller.
// It is created by the controller, passed into the Tester.
// The tester contains the response, and destroys it.
// The response is "completed" by the ThreadTest.
class CEventResponse : CActive
	{
public:	
	static CEventResponse* NewL(CConcurrentController& aController);
	~CEventResponse();
	void CompleteRequest(TInt aError);
	TRequestStatus& RequestStatus();

private:
	TInt RunError(TInt aError);
	void RunL();
	void DoCancel();
	CEventResponse (CConcurrentController& aController);

private:
	CConcurrentController& iController;
	
	};
	


class CConcurrentController : CBase
	{
public:
	~CConcurrentController();
	static CConcurrentController* NewLC();
	void  StartTestL(TInt aNoOfTesters);
	void  PublishError(TInt aError);

private:
	void CreateTestersL		  (TInt aNoOfTesters);
	void StartTransactionTestL(TInt aNoOfTesters);
	void StartTimeOutTestL	  (TInt aNoOfTesters);
	void StartAsyncOpenTestL  (TInt aNoOfTesters);
	void OpenNamedDatabaseTestL();
		 CConcurrentController();
	void ConstructL();

	TBool Completed();
private:
	TInt 						   	 iTesterIndex;
	RPointerArray<CConcurrentTester> iStore;
	};



class CConcurrentTester : CBase
	{
public:
	~CConcurrentTester();
	static CConcurrentTester* NewL(CEventResponse* aResponse, const TDesC& aThreadName);
	static TInt ThreadFunction(TAny* aResponse);
	// Create a new worker CTransactionThreadTest - called within ThreadFunction.
	void RunTestThreadL(TInt iTestCode = ETransactionTest);
	TBool IsCompleted();
	
private:
	inline CConcurrentTester(CEventResponse* aResponse, const TDesC& aThreadName) : 
							iResponse(aResponse), 
							iThreadName(aThreadName), 
							iTestCode(ETransactionTest) {}
	void RunTestL();
	
private:
	RThread 				iThread;
	CEventResponse* 		iResponse;
	TBuf<256>				iThreadName;
	TInt 					iTestCode;
	};


// CTransactionThreadTest is run in a seperate thread.
// It performs syncronous tests on CContactDatabase.
// It is created and executed in a new worker 
// thread. 

class CTransactionThreadTest : CBase
	{
public:
	~CTransactionThreadTest();
	static CTransactionThreadTest* NewLC();
	void RunTestL();

private:
	void ConstructL();
	inline CTransactionThreadTest(){}
	// Tests
	void CreateAndCommitTestL();
	void RollbackAndRecoverTestL();
	void DeleteAndCommitTestL();
	void UpdateAndCommitTestL();
	// Test helper methods
	TBool IsContactInDbL(TInt aCntID);
	TInt AddContactL();
	void DeleteContactL(TInt aCntID); 
	void UpdateContactL(TInt aCntID);
	TInt AddAndCommitContactL();
	const CContactTemplate& GetSysTemplateL();
	TBool CheckChangedFieldL(TInt aCntID);	
	void TimeOutTestL();
	

private:
	CContactTemplate*		iGoldenTemplate;
	CContactItemViewDef* 	iMatchAll;
	CContactDatabase* 		iDB;
	CCntItemBuilder*		iCntItemBldr;
	RTest*					iTest;
	};


// Request Timeout test classes
class CTransTimeOutThread: public CBase
	{
public:	
	static CTransTimeOutThread* NewLC();
	~CTransTimeOutThread();
	void RunTestL();
	
private:
	void ConstructL();
	
private:
	RTest*					iTest;
	CContactDatabase* 		iDB;

	};
	
class CReqTimeOutThread: public CBase
	{
public:	
	static CReqTimeOutThread* NewLC();
	~CReqTimeOutThread();
	void RunTestL();
	
private:
	void ConstructL();
	void AddContactL();
	const CContactTemplate&  GetSysTemplateL();
	
private: // member variables
	CContactTemplate*		iGoldenTemplate;
	CContactItemViewDef* 	iMatchAll;
	CContactDatabase* 		iDB;
	CCntItemBuilder*		iCntItemBldr;
	RTest*					iTest;
	};
	




class CDBTransTest : public CBase
	{
public:	
	static CDBTransTest* NewLC();
	~CDBTransTest();
	void RunTestsL();
	
private:
	void ConstructL();
	void CreateAndCommitTestL();
	void RollbackAndRecoverTestL();
	void OutOfMemoryAddTestL();
	void OutOfMemoryDeleteTestL();
	void OutOfMemoryUpdateTestL();	
	TBool IsContactInDbL(TInt aCntID);
	TInt AddContactL();
	void DeleteContactL(TInt aCntID); 
	void UpdateContactL(TInt aCntID);
	TInt AddAndCommitContactL();
	void UpdateAndCommitTestL();
	TBool CheckChangedFieldL(TInt aCntID);
	void DeleteAndCommitTestL();
	const CContactTemplate&  GetSysTemplateL();
	
private: // member variables
	CContactTemplate*		iGoldenTemplate;
	CContactItemViewDef* 	iMatchAll;
	CContactDatabase* 		iDB;
	CCntItemBuilder*		iCntItemBldr;
	};
	
	
#endif //__T_DB_TRANSACTION_TEST__
