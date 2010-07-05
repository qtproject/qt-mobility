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

#ifndef __CUSTOM_LABELUID_H__
#define __CUSTOM_LABELUID_H__

#include "cntbasestep.h"

class CCustomLabelUid : public CCntBaseStep
	{
private:
	enum TUidTypeTest
		{
		EDuplicateUids,
		EUniqueUids,
		};
public:
	CCustomLabelUid();
	virtual TVerdict doTestStepL();
private:
	void TestCustomUidsL( const TContactItemId aCid );
	void TestCustomLabelsL( const TContactItemId aCid );
	void TestCustomUidLabelsL( const TContactItemId aCid, const TBool aNewFields );
	void DuplicateUidsTestL( const TContactItemId aCid );
	void ExistingUidsTestL( const TUidTypeTest aTestType );
	void PreTestL()
		{
		}
	};
#endif
