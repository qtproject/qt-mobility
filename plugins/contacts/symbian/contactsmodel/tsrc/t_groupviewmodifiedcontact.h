/*
* Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
	void SetTestError(TInt aTestError);
	~CTestConductor();
private:
	CTestConductor();
	void ConstructL();
	void RunTestsL();
	void AddContactsL();

private:
	RFs iFs;
	CLog* iLog;
	CContactDatabase* iDb;
	TInt iTestError;
	};


//
// CGroupViewTester.
//

class CGroupViewTester : public CActive, public MContactViewObserver
	{
public:
	static CGroupViewTester* NewL(CLog& aLog, CTestConductor* aTestConductor, CContactDatabase& iDb);
	~CGroupViewTester();
private:
	CGroupViewTester(CLog& aLog, CTestConductor* iTestConductor, CContactDatabase& iDb);
	void ConstructL();
	void NextTest();
private: // From CActive.
	void RunL();
	TInt RunError(TInt aError);
	void DoCancel();
private: // From MContactViewObserver.
	virtual void HandleContactViewEvent(const CContactViewBase& aView,const TContactViewEvent& aEvent);
private:
	enum TTest
		{
		ECreateLocalView,
		ECreateGroupOne,
		ECreateGroupOneView,
		EAddContact,
		EAddContactToGroupOne,
		EModifyContactInGroupOne,
		ENumTests
		};
private:
	CLog& iLog;
	CTestConductor* iTestConductor;
	CContactDatabase& iDb;
	TInt iCurrentTest;
	RContactViewSortOrder iSortOrder_1;
	CContactLocalView* iLocalView;
	//GroupView_One
	CContactGroup* iGroupOne;
	TContactItemId iGroupOneId;
	CContactGroupView* iGroupViewOne;

	//

	TContactItemId iSingleContactId;
	TInt iNumNotificationExpected;
	};
