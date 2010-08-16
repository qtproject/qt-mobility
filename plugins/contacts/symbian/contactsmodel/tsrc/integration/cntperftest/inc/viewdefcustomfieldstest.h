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

#ifndef __VIEWDEF_CUSTOMFIELDS_TEST_H__
#define __VIEWDEF_CUSTOMFIELDS_TEST_H__

#include "viewdefbase.h"


class CViewDefCustomFieldsTest : public CViewDefBase
	{
public:
	CViewDefCustomFieldsTest();
	virtual TVerdict doTestStepL();
protected:
	virtual void PreTestL()
		{
		}
private:
	void MatchAllTestsL(const TContactItemId &aCid);
	void ManyUidsTestsL(const TContactItemId &aCid);
	TBool doAllMatchL(	const TContactItemId &aCid, const CContactItemViewDef &aView, const TInt aFieldCount, 
						const TBool aZeroN, const TBool aZeroH, const TBool aNew = EFalse);
	void NewFieldsMATestsL(const TContactItemId &aCid, const TBool aOnlyNew);
	TBool doManyUidsL(	const TContactItemId &aCid, const CContactItemViewDef &aView, const TInt aFieldCount, 
						const TBool aExcludeN, const TBool aExcludeH);
	void NewFieldsMUITestsL(const TContactItemId &aCid, const TBool aOnlyNew);
	void AddMultipleCustomUidsL( CContactItemViewDef &aView, TInt &aPos, const TInt &aMany );
	};
	
#endif
