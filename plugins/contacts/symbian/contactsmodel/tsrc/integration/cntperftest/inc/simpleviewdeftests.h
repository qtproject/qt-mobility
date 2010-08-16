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

#ifndef __SIMPLE_VIEWDEF_TESTS_H__
#define __SIMPLE_VIEWDEF_TESTS_H__

#include "viewdefbase.h"


class CSimpleViewDefTest : public CViewDefBase
	{
public:
	CSimpleViewDefTest();
	virtual TVerdict doTestStepL();
protected:
	virtual void PreTestL()
		{
		}
private:
	void SetupTestL(const TContactItemId aCid, CContactDatabase &aContactsDatabase,
					const CContactItemViewDef &aViewAll);
	void MIInverseTestL(	const TContactItemId aCid, CContactDatabase &aContactsDatabase,
							const CContactItemViewDef &aViewAll);
	void PrimayMappingsTestL(	const TContactItemId aCid, CContactDatabase &aContactsDatabase,
								const CContactItemViewDef &aViewAll);
	void AllPrimayMappingsTestL(	const TContactItemId aCid, CContactDatabase &aContactsDatabase,
									const CContactItemViewDef &aViewAll);
	void doPrimayMappingsTestL(const TContactItemId aCid, CContactDatabase &aContactsDatabase,
								const CContactItemViewDef &aViewAll, const TInt32 aUid);

	};
	
#endif
