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
#ifndef __REQUESTMIX_STEP_H__
#define __REQUESTMIX_STEP_H__
#include <testexecutestepbase.h>
#include "performancefunctionalitybase.h"

class CRequestMixStep : public CPerformanceFunctionalityBase
	{
public:
	CRequestMixStep();
	~CRequestMixStep();
	virtual TVerdict doTestStepL();

private:
	void PreTestL()
		{
		}
	
	void SingularTestsL(CContactDatabase *aDb2=NULL );
	void TransactionTestsL(CContactDatabase *aDb2=NULL );
	void UpdateTestL(CContactDatabase *aDb2 );
	void CloseTestL( CContactDatabase *aDb2 );
	void OpenCloseTestL( CContactDatabase *aDb2 );
	void ReadOpenCloseTestL( CContactDatabase *aDb2 );
	void MultipleOpenTestL(CContactDatabase *aDb2);
	void MultipleDestroyTestL();
	void TransactionCommitTestL( CContactDatabase *aDb2);
	void TransactionCloseTestL( CContactDatabase *aDb2 );
	void TransactionReadCloseTestL( CContactDatabase *aDb2 );
	void TransactionDeleteTestL( CContactDatabase *aDb2 );
	void TransactionOddSessionCommitTestL( CContactDatabase *aDb2 );
	void EndlessLockTestL( );
	};

_LIT(KRequestMixStep,"RequestMixStep");

#endif
