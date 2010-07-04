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
	static CTestConductor* NewL();
	~CTestConductor();
	void AddContactL(TInt aBitwiseFilterType);
	void SetTestError(TInt aTestError);

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
// CViewTester.
//

class CViewTester : public CActive, public MContactViewObserver
	{
public:
	static CViewTester* NewL(CLog& aLog,CContactDatabase& iDb,CTestConductor* aTestConductor);
	~CViewTester();

private:
	CViewTester(CLog& aLog,CContactDatabase& iDb,CTestConductor* aTestConductor);
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
		EStartingTests,

		EConstructRemoteView,
		ERemoteViewSortOrderChange,

		EConstructFilterView,
		EFilterViewSortOrderChange,

		EConstructGroupView,
		EGroupViewSortOrderChange,
		
		
       	EConstructGroupView2,
       	EGroupView2DeleteContactEvent,       

		ERemoteViewPlugin,
		ERemoteViewPlugin2,


		EEndTest
		};

	TTest 						iTestState;
	CLog& 						iLog;
	CContactDatabase& 			iDb;

	RArray<TInt>				iLocalViewResults;
	RArray<TInt>				iReceivedResults;


	CContactNamedRemoteView* 	iRemoteView;
	CContactFilteredView*	 	iFilteredView;
	CContactRemoteView* 	iRemoteView2;
	CContactRemoteView* 	iRemoteView3;

	RContactViewSortOrder		iViewSortOrder;
	CContactGroup* 				iGroup;
	CContactGroupView* 			iGroupView;

   	CContactGroup*             	iGroup2;
   	CContactGroupView*        	iGroupView2;
	TContactIdWithMapping     	iContactToDeleteMapping;

	CTestConductor* iTestConductor;
	TInt iItemEventsExpected;
	TInt iItemsEventsRecieved;

private:
	void AssignAllContactsToGroupL();
	void AssignContactsToGroup2L();

	void ConstructRemoteViewL(const TDesC8& aSortPluginName = KNullDesC8);
	void RemoteViewSortOrderChangeL();
	void DestructRemoteView();

	void ConstructFilterViewL();
	void FilterViewSortOrderChangeL();
	void DestructFilterView();

	void ConstructGroupViewL();
	void GroupViewSortOrderChangeL();
	void DestructGroupView();

   	void ConstructGroupView2L();
   	void GroupView2DeleteOnlyContactL();
   	void DestructGroupView2();

	void CompareRecordedResultWithExpected();
	CContactRemoteView* ExtracttRemoteViewLC();
	static CArrayFix<TContactItemId >* OrderOfContactsLC(const CContactViewBase& aView);
	};

