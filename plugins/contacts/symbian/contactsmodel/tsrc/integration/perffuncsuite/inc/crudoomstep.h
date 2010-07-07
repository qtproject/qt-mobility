/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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




/**
 @file 
 @publishedAll
 @released
*/
#if (!defined __CCRUDOOM_STEP_H__)
#define __CCRUDOOM_STEP_H__
#include <testexecutestepbase.h>
#include <cntviewbase.h>
#include "performancefunctionalitybase.h"


class CPerformanceFunctionalityTestsSuite;

class CCRUDOOMStep : public CPerformanceFunctionalityBase, public MContactViewObserver
	{
class CDestroyView;
friend class CDestroyView;
public:
	CCRUDOOMStep();
	virtual TVerdict doTestStepL();

private:
	void ReadOOML(const TBool aClient);
	void OpenOOML(const TBool aClient);
	void DeleteOOML(const TBool aClient);
	void AddContactOOML(const TBool aClient);
	void CloseContactOOML(const TBool aClient);
	void CommitContactOOML(const TBool aClient);
	void UpdateContactOOML(const TBool aClient);
	void PopulateFullViewL(const TBool aRemote, RContactViewSortOrder& aSortOrder);
	void ViewOOMTestL(const TBool aRemote, const TBool aClient);

//utility functions
private:
	void PreTestL();
	void InitializeL();
	void Cleanup();
	void RunAllTestsL(const TBool aClient);
	void StartHeapCheck(const TInt aFailAt, const TBool aClient);
	TBool EndHeapCheckL(const TInt aErr, const TBool aClient);
	void ErrorProcess(	const TInt aFailAt, const TInt aErr,
						const TBool aClient, const TBool aCleanup);
	void RecoverL(const TInt aErr);
	
//from MContactViewObserver
private:
	void HandleContactViewEvent(const CContactViewBase& aView,const TContactViewEvent& aEvent);

private:
	TContactViewEvent	 iEvent;
	CContactViewBase 	*iView;
	CDestroyView	 	*iDestroyView;
	TInt				iError;


private:		
	class CDestroyView : public CActive
		{
		public:
			CDestroyView(CCRUDOOMStep *aObserver) : CActive( EPriorityStandard ), iTest( aObserver )
				{
				}
			void RunL();
			void DoCancel();
			void Activate();

		private:
			CCRUDOOMStep 	*iTest;
			TBool 			iInitial;
		};
	};

_LIT(KCRUDOOMStep,"CRUDOOMStep");


#endif
