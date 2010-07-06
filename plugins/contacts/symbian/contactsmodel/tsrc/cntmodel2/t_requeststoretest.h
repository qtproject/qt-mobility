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


#ifndef __T_REQUESTSTOREFAILURE_H__
#define __T_REQUESTSTOREFAILURE_H__

#include <e32test.h>
#include <cntdb.h>

enum TTestCodes
    {
    ELockDatabase,
    EDeleteInvalidCnt
    };

const TTimeIntervalMicroSeconds32 KOneHalfSecondTimeOut =   500000;
const TTimeIntervalMicroSeconds32 KSixtySecondsTimeOut  = 60000000;

// forward declaration
class CConcurrentTester;
class CConcurrentController;


//===========================================================
// CLockDatabase Class
//===========================================================
//
class CLockDatabase: private CActive
    {
    public:
        ~CLockDatabase();
        static CLockDatabase* NewLC();

    public:
        // from CActive
        void DoCancel();
        TInt RunError(TInt aError);
        void RunL();

    private:
        // constructor & 2nd phase constructor
        CLockDatabase();
        void ConstructL();
        
        void OpenDatabaseL();        

    private:
        CContactDatabase* iContactDatabase;
        RTest* iTest;
        TBool iLocked;
    };

//===========================================================
// CDeleteInvalidCnt Class
//===========================================================
//
class CDeleteInvalidCnt: public CActive
    {
public:
    ~CDeleteInvalidCnt();
    static CDeleteInvalidCnt* NewLC();

public:
    // from CActive
    void DoCancel();
    TInt RunError(TInt aError);
    void RunL();

private:
    // constructor and 2nd phase constructor
    CDeleteInvalidCnt();
    void ConstructL();

    void OpenDatabaseL();

private:
    TInt iStep;
    CContactDatabase* iContactDatabase;
    RTest* iTest;
    };


//===========================================================
// CEventResponse Class
//
// Responsible for signalling the controller.
// It is created by the controller, passed into the Tester.
// The tester contains the response, and destroys it.
// The response is "completed" by the ThreadTest.
//===========================================================
//
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


//===========================================================
// CConcurrentController Class
//===========================================================
//
class CConcurrentController : CBase
    {
public:
    ~CConcurrentController();
    static CConcurrentController* NewLC();
    void  StartTestL();
    void  PublishError(TInt aError);

private:
    void CreateContactsL(TInt aNumContacts);

    void CreateTestersL(TInt aNoOfTesters);
    void StartRequestStoreFailureTestL();

    // constructor & 2nd-phase constructor
    CConcurrentController();
    void ConstructL();

    TBool Completed();

private:
    RPointerArray<CConcurrentTester> iStore;
    };


//===========================================================
// CConcurrentTester Class
//===========================================================
//
class CConcurrentTester : CBase
    {
public:
    ~CConcurrentTester();
    static CConcurrentTester* NewL(CEventResponse* aResponse, const TDesC& aThreadName);
    static TInt ThreadFunction(TAny* aResponse);
    void RunTestThreadL(TInt iTestCode);
    TBool IsCompleted();

private:
    inline CConcurrentTester(CEventResponse* aResponse, const TDesC& aThreadName) :
                            iResponse(aResponse),
                            iThreadName(aThreadName),
                            iTestCode(-1) {}
    void RunTestL();

private:
    RThread                 iThread;
    CEventResponse*         iResponse;
    TBuf<256>                iThreadName;
    TInt                     iTestCode;
    };

#endif //__T_REQUESTSTOREFAILURE_H__
