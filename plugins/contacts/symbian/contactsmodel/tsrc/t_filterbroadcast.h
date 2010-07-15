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

class CViewTesterBase;


//
// CTestConductor.
//

class CTestConductor : public CBase
	{
public:
	static CTestConductor* NewL(RTest& aTest);
	~CTestConductor();
	void AddContactL(TInt aBitwiseFilterType);
	void SetTestError(TInt aTestError);
private:
	CTestConductor(RTest& aTest);
	void ConstructL();
	void RunTestsL();
	void AddContactsL();
	void AddContactsThatMatchFilterL(TInt aNumberOfContacts, TInt aFilter);
private:
	RTest& iTest;
	CContactDatabase* iDb;
	CRandomContactGenerator* iRandomGenerator;
	TInt iTestError;
	};


//
// CViewTester.
//

class CViewTester : public CActive, public MContactViewObserver
	{
		
public: // Construction
	static CViewTester* NewL(RTest& aTest, CContactDatabase& iDb, CTestConductor& aTestConductor);
	~CViewTester();
private:
	CViewTester(RTest& aTest, CContactDatabase& iDb, CTestConductor& aTestConductor);
	void ConstructL();

public:
	void StartTest();

private: // Functionality. 
	virtual void RunL();
	TInt RunError(TInt aError);
	virtual void DoCancel();
	virtual void HandleContactViewEvent(const CContactViewBase& aView,const TContactViewEvent& aEvent);
	
private: // utility
	void NextState();
	void CallRunLAgain();

private: // data type definitions
	enum TState
		{
		ECreate_RemoteView,
		EWait_RemoteView,
		ECreate_FilteredView,
		EWait_FilteredView,
		EChangeSortOrder,
		EWait_SortViewEvents,
		EFinished
		};

private: // member data
	RTest& 						iTest;
	CContactDatabase& 			iDb;
	CTestConductor& 			iTestConductor;
	TState 						iState;
	RTimer						iTimer;
	
	RContactViewSortOrder		iSortOrder[2];
	
	CContactNamedRemoteView* 	iNamedRemoteView;
	CContactFilteredView*		iFilteredView;
	TContactViewEvent::TEventType	iRemoteViewEvent;
	TContactViewEvent::TEventType	iFilteredViewEvent;
	};
