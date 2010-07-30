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


#include <cntview.h>
#include "t_utils2.h"

class CGroupViewTesterBase;


class CTestConductor : public CBase
	{
public:
	static CTestConductor* NewL();
	void SetTestError(TInt aTestError);
    TContactItemId AddRandomContactL();
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

class CGroupViewTester : public CActive, public MContactViewObserver
	{
public:
	static CGroupViewTester* NewL( CLog& aLog, CTestConductor* aTestConductor, 
        CContactDatabase& iDb );
	~CGroupViewTester();

private:
	CGroupViewTester( CLog& aLog, CTestConductor* aTestConductor, 
        CContactDatabase& iDb );
	void ConstructL();
	void NextTest();
	void CreateGroupTestDataL();
    void LogEvent( TContactViewEvent& aEvent );

private: // From CActive.
	void RunL();
	TInt RunError( TInt aError );
	void DoCancel();

private: // From MContactViewObserver.
	virtual void HandleContactViewEvent( const CContactViewBase& aView, 
        const TContactViewEvent& aEvent);

private:
	enum TTest
		{
		ECreateLocalView,
		ECreateGroupOneView,
        ECreateGroupTwoView,
        EDeleteItemsFromView,
        EAddItemToGroup,
		ENumTests
		};

private:
	CLog&                  iLog;
	CTestConductor*        iTestConductor;
	CContactDatabase&      iDb;
	TInt                   iCurrentTest;
	RContactViewSortOrder  iSortOrder_1;
	CContactTextDef*       iTextDef;
	CContactLocalView*     iLocalView;
	CContactGroup*         iGroupOne;
	TContactItemId         iGroupOneId;
	CContactGroupView*     iGroupViewOne;
	CContactGroupView*     iGroupViewTwo;
	CContactIdArray*       iIdsInGroupViewOne;
    TInt                   iNumNotificationExpected;
    TInt                   iNumRemovedEvents; 
    TContactItemId         iIdAdded;
    TContactItemId         iUnfiledContact;
	TContactItemId 		   iLastContactID;
	TContactItemId 		   iSecondLastContactID;
	TContactItemId 		   iThirdLastContactID;
	};
