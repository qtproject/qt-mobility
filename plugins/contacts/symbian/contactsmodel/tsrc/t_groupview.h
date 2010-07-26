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
	CRandomContactGenerator* iRandomGenerator;
	TInt iTotalContacts;
	TInt iTestError;
	};

//
// CGroupViewTester.
//

class CGroupViewTester : public CActive, public MContactViewObserver
	{
public:
	static CGroupViewTester* NewL(CLog& aLog,CTestConductor* aTestConductor,CContactDatabase& iDb);
	~CGroupViewTester();
private:
	CGroupViewTester(CLog& aLog,CTestConductor* aTestConductor,CContactDatabase& iDb);
	void ConstructL();
	void NextTest();
	void ExceriseViewL(CContactViewBase& aView);
	void TestGroupViewSortOrderL(CContactGroupView& aView);
	void CreateGroupTestDataL();
private: // From CActive.
	void RunL();
	TInt RunError(TInt aError);
	void DoCancel();
	static TInt EventConsumerCallBack(TAny* aThis);
private: // From MContactViewObserver.
	virtual void HandleContactViewEvent(const CContactViewBase& aView,const TContactViewEvent& aEvent);
	void HandleDatabaseEventL(TContactDbObserverEvent aEvent);
private:
	enum TTest
		{
		ECreateLocalView,
		EExerciseLocalView,
		ECreateGroupOneView,
		ETestGroupOneView,
		ECreateGroupOneViewByName,
		ETestGroupOneViewByName,
		ECreateGroupOneViewNotInGroup,
		ETestGroupOneViewNotInGroup,
		EAllViewsOutOfBoundsAccess,
		ECreateUnfiledGroupView,
		ETestUnfiledGroupView,
		ETestUnfiledGroupAddition,
		ENumTests
		};
private:
	CLog& iLog;
	CTestConductor* iTestConductor;
	CContactDatabase& iDb;
	TInt iCurrentTest;
	RContactViewSortOrder iSortOrder_1;
	CContactTextDef* iTextDef;
	TBuf<128> iScratchBuf;
	CContactLocalView* iLocalView;
	//GroupView_One
	CContactGroup* iGroupOne;
	TContactItemId iGroupOneId;
	HBufC* iGroupView_One_Name;
	CContactGroupView* iGroupViewOne;
	CContactGroupView* iGroupViewOneByName;
	CContactGroupView* iGroupViewOneNotInGroup;
	CContactGroupView* iGroupViewUnfiled;
	CContactIdArray* iIdsInGroupViewOne;
	//
	CEventConsumer*	iEventConsumer;

	TInt iNumNotificationExpected;
	};
