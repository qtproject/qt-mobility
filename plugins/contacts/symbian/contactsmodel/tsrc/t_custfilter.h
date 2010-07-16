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

class CViewTesterBase;


//
// CTestConductor.
//

class CTestConductor : public CBase
	{
public:
	enum TContactFieldToFill
		{
		ELocation,
		ENickname
		};

	static CTestConductor* NewL();
	~CTestConductor();
	void AddContactL(TInt aBitwiseFilterType);
	void SetTestError(TInt aTestError);

private:
	CTestConductor();
	void ConstructL();
	void RunTestsL();
	void AddContactsL();

	void RestoreFilterableFieldsFromSystemTemplateL();
	TUid CustomFilterableField(TInt aIndex) const;

	//
	TContactItemId CreateContactL(TContactFieldToFill aFld);

public:
	TInt iAcceptCount;
	TInt iAskCount;

	TUid iLocationFieldUID;
	TUid iNicknameFieldUID;

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
	void ExceriseViewL(CContactViewBase& aView);
private: // From CActive.
	void RunL();
	TInt RunError(TInt aError);
	void DoCancel();
private: // From MContactViewObserver.
	virtual void HandleContactViewEvent(const CContactViewBase& aView,const TContactViewEvent& aEvent);
private:
	void TestViewNotificationsL(CContactViewBase& aView);
private:
	void TestViewIndiciesL(const CContactViewBase& aView);
	void PrintTimeTaken(TInt aProfile);
	void PrintTotalTimeTaken();

private:
	void HandleViewCreation(const CContactViewBase& aView, const TContactViewEvent& aEvent, CContactViewBase* aPtrView);

private:
	CContactFilteredView* CreateFilterViewL();

private:
	enum TTest
		{
		ECreateLocalView,
		EExerciseLocalView,
		ECreateCustom1View,
		ECreateCustom2View,
		ETestCustom1,
		ETestCustom2,

		ENumTests
		};
private:
	CLog& iLog;
	CContactDatabase& iDb;
	TInt iCurrentTest;
	RContactViewSortOrder iSortOrder_1;
	CContactTextDef* iTextDef;
	TBuf<128> iScratchBuf;
	CContactNamedRemoteView* iNamedRemoteView;
	CContactFilteredView* iCustom1View;
	CContactFilteredView* iCustom2View;
	
	TInt iFilter;
	TInt iNumNotificationExpected;
	CTestConductor* iTestConductor;
	TInt iItemEventsExpected;
	TInt iItemsEventsRecieved;
	TInt iTimeForTestsToComplete[ENumTests];
	TCntProfile iProfile;
	};
