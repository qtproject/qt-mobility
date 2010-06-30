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

#ifndef __VIEWDEF_ANYFIELDS_TESTS_H__
#define __VIEWDEF_ANYFIELDS_TESTS_H__

#include "viewdefbase.h"


class CViewDefAnyFieldsTest : public CViewDefBase
	{
public:
	CViewDefAnyFieldsTest();
	virtual TVerdict doTestStepL();
protected:
	virtual void PreTestL()
		{
		}
private:
	void doAnyFieldsTestL(const TContactItemId &aCid);
	void doUniqueUidsTestL(const TContactItemId &aCid);
	void doIdenticalUidsTestL(const TContactItemId &aCid);
	TBool FieldCountCompL(	const TContactItemId &aCid, CContactItemViewDef &aView, 
							const TBool aExcludeN, const TBool aExcludeH, 
							const TBool aZeroH = EFalse, const TBool aMultiple = EFalse);
	TBool MultipleUidCountCompL(const TContactItemId &aCid, CContactItemViewDef &aView, 
								const TBool aExcludeN, const TBool aExcludeH);

	};
	
#endif

