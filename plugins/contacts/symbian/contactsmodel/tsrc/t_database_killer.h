/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <cntview.h>
#include "t_utils2.h"

class CDatabaseTesterBase;

class CTestConductor : public CBase
	{
public:
	static CTestConductor* NewL(TBool aKillFourTimes);
	~CTestConductor();
	void SetTestError(TInt aTestError);
private:
	CTestConductor(TBool aKillFourTimes);
	void ConstructL();
	void RunTestsL();
private:
	RFs iFs;
	CLog* iLog;
	CContactDatabase* iDb;
	TBool iKillFourTimes;
	CTestRegister* iTempFiles;
	TInt iTestError;
	};

class CDatabaseTester : public CActive
	{
public:
	static CDatabaseTester* NewL(CLog& aLog,CContactDatabase& iDb,CTestConductor* aTestConductor, TBool aKillFourTimes);
	~CDatabaseTester();
private:
	CDatabaseTester(CLog& aLog,CContactDatabase& iDb,CTestConductor* aTestConductor, TBool aKillFourTimes);
	void ConstructL();
	void NextTest();
	void AddContactsL();
private: // From CActive.
	void RunL();
	TInt RunError(TInt aError);
	void DoCancel();
private:
	void FindServerAndKillItL(TInt aDelay);
	void SetupThreadL();
	void SetupProcessL();
	void DeleteThread();
	void DeleteProcess();
	void OpenAndKillProcess();
	void OpenAndKillThread();
	void WaitForServerToClose();
	void AddContactL(TInt aBitwiseFilterType);

private:
	enum TTest
		{
		EInitialisingTests,
		EAddContacts,
		EKillThreadFirstTime,
		ERemoveContacts,
		EKillThreadSecondTime,
		EAddMoreContacts,
		EKillThreadThirdTime,
		ERemoveMoreContacts,
		EKillThreadForthTime,
		ENumTests
		};
private:
	CLog& iLog;
	CContactDatabase& iDb;
	TTest iCurrentTest;
	TBuf<128> iScratchBuf;
	TInt iFilter;
	TInt iNumNotificationExpected;
	CTestConductor* iTestConductor;
	TInt iItemEventsExpected;
	TInt iItemsEventsRecieved;
	CRandomContactGenerator* iRandomGenerator;
	TInt iTotalContacts;
	TBool iKillFourTimes;
private:
	union
		{
		struct
			{
			TFindThread* iFind;
			RThread* iExecution;
			} iThread;
		struct
			{
			TFindProcess* iFind;
			RProcess* iExecution;
			} iProcess;
		} iProcessOrThread;
	TFullName iProcessOrThreadFullName;

	};
