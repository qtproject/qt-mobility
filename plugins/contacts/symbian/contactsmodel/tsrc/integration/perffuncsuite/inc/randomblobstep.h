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
#if (!defined __RANDOMBLOB_STEP_H__)
#define __RANDOMBLOB_STEP_H__
#include <testexecutestepbase.h>
#include "performancefunctionalitybase.h"

class CRandomBlobStep : public CPerformanceFunctionalityBase
	{
public:
	CRandomBlobStep();
	virtual TVerdict doTestStepL();

// Please add/modify your class members here:
private:
	void PreTestL()
		{
		}
	HBufC8 *RandomBlobLC(const TInt aSize);
	void SetAllBlobsL(const TContactItemId aCid, const HBufC8 &aBuf);
	TBool CheckAllBlobsL(const TContactItemId aCid, const HBufC8 &aBuf);
	void BlobTestL();
	void MaxBlobTestL();
private:
	TInt64 iSeed;
	};

_LIT(KRandomBlobStep,"RandomBlobStep");

#endif
