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

class CViewTesterBase;

class CTestConductor : public CBase
	{
public:
	static CTestConductor* NewL();
	~CTestConductor();
	void AddContactL(TInt aBitwiseFilterType);
	void AddContactsL();
	void SetTestError(TInt aTestError);
	CContactIdArray*			contacts;

private:
	CTestConductor();
	void ConstructL();
	void RunTestsL();
private:
	RFs iFs;
	CLog* iLog;
	CContactDatabase* iDb;
	CRandomContactGenerator* iRandomGenerator;
	TInt iTotalContacts;
	TInt iTestError;
	};

class CViewTester : public CActive, public MContactViewObserver
	{
public:
	static CViewTester* NewL(CLog& aLog,CContactDatabase& iDb,CTestConductor* aTestConductor);
	~CViewTester();

private:
	CViewTester(CLog& aLog,CContactDatabase& iDb,CTestConductor* aTestConductor);
	void ConstructL();
	void CompleteRequest();
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
		
		EAddPhoneNumber1,
		EPhoneNumberChangedEvent1,
		EAddPhoneNumber2,
		EPhoneNumberChangedEvent2,
		EAddPhoneNumber3,
		EPhoneNumberChangedEvent3,
		EAddPhoneNumber4,
		EPhoneNumberChangedEvent4,
		
		
		
		ERemoteViewPlugin,
		ERemoteViewPlugin2,

	
		EEndTest
		};

	TTest 						iTestState;
	CLog& 						iLog;
	CContactDatabase& 			iDb;

	RArray<TInt>				iLocalViewResults;
	RArray<TInt>				iReceivedResults;
	
	RArray<TInt>				iAddNumberRemoteViewResults1;
	RArray<TInt>				iAddNumberRemoteViewResults2;
	RArray<TInt>				iAddNumberRemoteViewResults3;
	RArray<TInt>				iAddNumberRemoteViewResults4;	
	RArray<TInt>				iAddNumberReceivedResults;

	CContactNamedRemoteView* 	iRemoteView;
	CContactRemoteView* 	iRemoteView2;
	CContactRemoteView* 	iRemoteView3;

	RContactViewSortOrder		iViewSortOrder;

   	TContactIdWithMapping       iContactToDeleteMapping;

	CTestConductor* iTestConductor;
	TInt iItemEventsExpected;
	TInt iItemsEventsRecieved;

private:
	void AssignAllContactsToGroup();
	void AssignContactsToGroup2();

	void ConstructRemoteViewL(const TDesC8& aSortPluginName = KNullDesC8);
	void RemoteViewSortOrderChangeL();
	void DestructRemoteView();


	void CompareRecordedResultWithExpected();
	void AddNumberCompareRecordedResultWithExpected(const RArray<TInt>&);
	CContactRemoteView* ExtracttRemoteViewLC();
	static CArrayFix<TContactItemId >* OrderOfContactsLC(const CContactViewBase& aView);
	void AddPhoneNumberL(const TDesC& aWorkPhone,TContactItemId& ItemId);
	};
