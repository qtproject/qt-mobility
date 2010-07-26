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

#ifndef __UNIFIED_SINGULAR_STEP_H__
#define __UNIFIED_SINGULAR_STEP_H__

#include "cntbasestep.h"

class CUnifiedSingular : public CCntBaseStep
	{
public:
	CUnifiedSingular();
	virtual TVerdict doTestStepL();
protected:
	void PreTestL()
	{
	}

protected:
	enum TUnitType
		{
		EAddUid = 0,
		ESetLabel,
		EAddField,
		ESetFieldValue,
		};
private:
	void UnifiedSingularTestsL();
	void SetSingularL(	const TContactItemId &aCid, 
						const TInt &aPos, 
						const TUnitType &aType, 
						const TInt32 &aUid, 
						const TDesC &aLabel=KNullDesC, 
						CContentType ** const aCtype=NULL, 
						const TBool aEmpty=EFalse);
						
	TBool CheckSingularL(const TContactItemId &aCid, 
						const TInt &aPos, 
						const TUnitType &aType, 
						const TInt32 &aUid, 
						const CContentType &aCtype, 
						const TDesC &aLabel=KNullDesC, 
						const TBool aEmpty=EFalse);
						
	TBool CompareCTypes(const CContentType &aType1,const CContentType &aType2) ;
	void EmptyNewFieldTestL(const TContactItemId &aCid) ;
	};
#endif
