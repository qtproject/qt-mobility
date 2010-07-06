/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <e32svr.h>
#include <cntviewbase.h>
#include "t_utils.h"

#ifndef __TSECASYNCOPEN__
#define __TSECASYNCOPEN__

// Main test conducting class for Secure Async Open Testing
class CTestConductor : public CActive
	{
public:
	static CTestConductor* NewL();
	~CTestConductor();
	
private:
	enum TTest
		{
		EAsyncOpenNamed  = 0,
		EAsyncOpenedNamed,
		EAsyncOpenDefault,
		EAsyncOpenedDefault,
		EAsyncOpenNonExistent,
		EAsyncOpenedNonExistent,
		EAsyncCancelOpenDefault,
		EAsyncCanceledOpenedDefault,
		EAsyncOpenDamaged,
		EAsyncOpenedDamaged,
#ifndef _TEST_ON_SQLITE_		
		EAsyncOpenPreviousVersion,
		EAsyncOpenedPreviousVersion,
		EOpenAsyncAndAsync,
		EOpenedAsyncAndAsync,
		EWaitState,
		EWaitedState,
		EOpenAsyncAndSync,
		EOpenedAsyncAndSync,
#endif // _TEST_ON_SQLITE_		
		ENumTests
		};	
		
	CTestConductor();
	void ConstructL();
	TInt RunError(TInt aError);
	void NextTest();
	void CopyCdbFileL(const TDesC& aFileName, const TDesC& aToFileName);
	void RunL();      // From CActive.
	void DoCancel();  // From CActive.

private:
    CContactOpenOperation* iContactOpenOperation;
	TInt iCurrentTestNumber;
	CContactDatabase* iDb;
	TInt iWaitCounter;
	TInt iTestError;
	};



// Active object to Test Async open from two threads at once
class CTester : public CActive
	{
public:
	static CTester* NewL();
	~CTester();
	
private:
	enum TTest
		{
		EAsyncOpenNamed,
		EAsyncOpenedNamed,
		ENumTests
		};	


	CTester();
	void ConstructL();
	void NextTest();
	void RunL();      // From CActive.
	void DoCancel();  // From CActive.

private:
    CContactOpenOperation* iContactOpenOperation;
    TInt iCurrentTestNumber;
	CContactDatabase* iDb;
	};


// This class (CDbOpenLauncher) creates a thread to do async and sync opens 
class CDbOpenLauncher : public CBase	
	{
public:
	static void Create(const TDesC& aName, const TDesC& aViewName);

private:
	static void DoMainL(const TDesC& aName);
	static TInt LaunchThread(TAny* aAny);	
		
	class CViewer : public CBase
		{
	public: 
		static CViewer* NewL(const TDesC& aViewName);
		~CViewer();
	private: 
		void ConstructL(const TDesC& aViewName);
		
	private:
		CContactDatabase* iDatabase;
		CTester* tester;
		
	};
			
};

#endif // __TSECASYNCOPEN__

