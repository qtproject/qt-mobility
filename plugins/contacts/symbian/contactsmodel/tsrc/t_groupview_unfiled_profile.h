/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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


//
// Forward declarations.
//

class CGroupViewTesterBase;


//
// CTestConductor.
//

class CTestConductor : public CBase
	{
public:
	static CTestConductor* NewL();
	~CTestConductor();
	void AddSingleContactL();
	void PrintTimeTaken(TInt aTime);
	void SetTestError(TInt aTestError);

private:
	CTestConductor();
	void ConstructL();
	void RunTestsL();
	void AddContactsL();
	void CreateGroupTestDataL();
private:
	RFs iFs;
	CLog* iLog;
	CContactDatabase* iDb;
	CRandomContactGenerator* iRandomGenerator;
	TInt iTotalContacts;
	//TInt iCurrentContactId;
	TContactItemId iGroupOneId;
	TInt iTestError;
	};


//
// CGroupViewTester.
//

class CUnfiledGroupViewTester : public CActive, public MContactViewObserver
	{
public:
	static CUnfiledGroupViewTester* NewL(CLog& aLog,  CContactDatabase& aDb, CTestConductor& aTestConductor, CContactViewBase& aView, TRequestStatus& aClientStatus);
	CUnfiledGroupViewTester(CLog& aLog,  CContactDatabase& aDb, CTestConductor& aTestConductor, CContactViewBase& aView, TRequestStatus& aClientStatus);
	~CUnfiledGroupViewTester();
	void ConstructL();
	void NextTest();
	void StartL();
private: // From CActive.
	void RunL();
	TInt RunError(TInt aError);
	void DoCancel();
private: // From MContactViewObserver.
	virtual void HandleContactViewEvent(const CContactViewBase& aView,const TContactViewEvent& aEvent);
private:
	enum TTest
		{
		EPreStartTest,
		ECreateUnfiledGroupView,
		ETestAddUnfiledContacts,
		ETestRemoveUnfiledContacts,
		ENumTests
		};
private:
	CLog& iLog;
	CContactDatabase& iDb;
	CContactViewBase& iView;
	CTestConductor& iTestConductor;
	TTest iCurrentTest;
	TRequestStatus& iClientStatus;
	CContactGroupView* iGroupViewUnfiled;
	TInt iSizeOfViewToExpect;
	};


//
// CGroupViewTester
//
class CGroupViewTester : public CActive, public MContactViewObserver
	{
public:
	static CGroupViewTester* NewL(CLog& aLog,CContactDatabase& iDb, CTestConductor& aTestConductor);
	~CGroupViewTester();
private:
	CGroupViewTester(CLog& aLog,CContactDatabase& iDb, CTestConductor& aTestConductor);
	void ConstructL();
	void NextTest();
	void ExceriseViewL(CContactViewBase& aView);
	void TestGroupViewSortOrderL(CContactGroupView& aView);
private: // From CActive.
	void RunL();
	TInt RunError(TInt aError);
	void DoCancel();
private: // From MContactViewObserver.
	virtual void HandleContactViewEvent(const CContactViewBase& aView,const TContactViewEvent& aEvent);
private:
	enum TTest
		{
		EPreStartTest,
		ECreateLocalView,
		ECreateRemoteNamedView,
		EStartLocalTests,
		EEndLocalTests,
		EStartRemoteTests,
		EEndRemoteTests,
		ENumTests
		};
private:
	CLog& iLog;
	CContactDatabase& iDb;
	CTestConductor& iTestConductor;
	TTest iCurrentTest;
	RContactViewSortOrder iSortOrder_1;
	CContactTextDef* iTextDef;
	TBuf<128> iScratchBuf;

	CContactLocalView* iLocalView;
	CContactNamedRemoteView* iNamedRemoteView;
	CUnfiledGroupViewTester* iTester;
	};
