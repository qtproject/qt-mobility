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

#if (!defined __VIEWDEF_ALLFIELDS_TESTS_H__)
#define __VIEWDEF_ALLFIELDS_TESTS_H__

#include "viewdefbase.h"

class CViewDefAllFieldsTest : public CViewDefBase
	{
public:
	CViewDefAllFieldsTest();
	virtual TVerdict doTestStepL();
	virtual void PreTestL()
		{
		}
private:
	void FieldCountTestL();
	TBool FieldCountCompL(const TContactItemId aCid, const CContactItemViewDef &aView, 
	const TBool aDefault,const TBool aZeroN,const TBool aZeroH);
	};

#endif
