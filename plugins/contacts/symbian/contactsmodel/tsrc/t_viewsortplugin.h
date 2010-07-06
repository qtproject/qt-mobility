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


class CContactDatabase;
#include <cntviewbase.h>

class CContactFindView;

class CTestMachine : public CTimer, public MContactViewObserver
	{
public: 
	static CTestMachine* NewLC();

//From MContactViewObserver
	void HandleContactViewEvent(const CContactViewBase& aView,const TContactViewEvent& aEvent);

//Steps:
	void StepRestoreDatabaseL();
	void StepFindViewsL();
	void StepNamedRemoteViewsL();
	void StepDone();
	void StepOOML();

private:
	CTestMachine();
	~CTestMachine();
	void ConstructL();

	//state machine related
	void NextStep();
	void RunL();
	void DoCancel() {};
	TInt RunError(TInt aErrNum);

	//helpers
	void CheckViewOrderL(const CContactViewBase* aView, TBool aPrint = EFalse, TBool aReverseOrder = ETrue);
	TContactItemId SearchInViewL(CContactViewBase* aView, const TDesC& aSurname);
	inline void CloseView(CContactViewBase* aView);

	TInt64 iSeed;
	TInt   iStep;
	TInt   iSubStep;
	TInt   iOOMCounter;

	TInt  iError; //Error received in the callback function
	TInt  iNumOfEvents;

	CContactDatabase*  iDb;
	CContactLocalView* iLocalView;
	CContactFindView*  iLocalFindView;

	CContactRemoteView*		iRemoteViewReverse;
	CContactRemoteView*		iRemoteViewForward;
	CContactFindView*		iRemoteFindView;
	CContactNamedRemoteView* iNamedRemoteView;
	CContactNamedRemoteView* iNamedRemoteView2;
	CContactFindView*		iNamedRemoteFindView;
	CContactFindView*		iNamedRemoteFindView2;


	RContactViewSortOrder iSurnNameSortOrder;
	

	CPtrC16Array* iFindDesArray;
	};
