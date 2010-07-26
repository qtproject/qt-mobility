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
#ifndef __LOWDISKSPACE_STEP_H__
#define __LOWDISKSPACE_STEP_H__
#include <testexecutestepbase.h>
#include "performancefunctionalitybase.h"

class CLowDiskSpaceStep : public CPerformanceFunctionalityBase
	{
class CLowDiskSpaceActive;
friend class CLowDiskSpaceActive;
private:
    enum TTransactionType
        {
        ERollbackTransaction,
        ECommitTransaction,
        };
public:
	CLowDiskSpaceStep(CPerformanceFunctionalityTestsSuite &aParent);
	TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();

private:
	void InitializeL();
	void Cleanup();
	void PreTestL();
	void Activate();
	void FillDiskL();
	void ClearDiskL();
	void HighDiskModeL();
	void RefillL();
	void PrintFail( const TDesC &aFunc, const TBool aRet, const TInt aErr );
	TBool CrudTestL();
	TBool DatabaseTestsL();
	TBool MiscDbTestsL();
	TBool StartHighdiskModeL();
	TBool StartLowDiskModeL();
	void FailTransactionTestL( const TTransactionType aTransactionType );
	void StartServerTestL();
	
private: // From CActive.
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);
	
private:
	RFile *iFile;
	TInt iManyFiles;
	RArray< TBool (CLowDiskSpaceStep::*)() > *iTests;
	TInt iNextTest;
	TInt iRecievedError;
	CContactItem  	*iGroup;
	CContactItem  	*iCustomTemplate;
	TContactItemId 	iGroupedContact;
	TInt 			iExpectedError;
	TBool			iCleanup;
	TBool 			iErrorPrinted;
	CLowDiskSpaceStep::CLowDiskSpaceActive *iMyActive;
	TDesC 			*iConfigSection;
	
private:
	class CLowDiskSpaceActive : public CActive
		{
	public:
		CLowDiskSpaceActive(CLowDiskSpaceStep *aStep) : 	CActive( EPriorityStandard ),
												iStep ( aStep )	
												{}
		void Activate();
	private: // From CActive.
		void RunL();
		void DoCancel();
		TInt RunError(TInt aError);
	private:
		CLowDiskSpaceStep *iStep;
		};
	};

_LIT(KLowDiskSpaceStep,"LowDiskSpaceStep");

#endif
