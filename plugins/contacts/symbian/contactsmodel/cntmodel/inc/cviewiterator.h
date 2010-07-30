/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent
 @released
*/


#ifndef CVIEWITERATOR_H
#define CVIEWITERATOR_H

#include "persistencelayer.h"

#define KIgnoreSessionId 0


/**
The CViewIterator class prepares and saves data for calls to the
MlplViewIteratorManager::ItemAtL() method which does the real work on database
views.
*/
NONSHARABLE_CLASS(CViewIterator) : public CBase
	{
public:
	CViewIterator(MLplViewIteratorManager& aManager,const CContactTextDef& aTextDef, TContactViewPreferences aViewPreferences);
	~CViewIterator();

	void GoFirstL();
	CViewContact* NextItemL();
	CViewContact* ItemAtL(TInt aCntItemId);

private:
	MLplViewIteratorManager::TTableMask TableMask(const CContactTextDef& aTextDef);

	TBool UsesIdentityTableOnly(TInt aFindFlags);
	TBool SearchIdentityTableRequired(TInt aFindFlags);

	void SetFindFlagsAndColumnsCount(TInt32 aUid,TInt& aFindFlags,TInt& aIdentityColumnsCount);
	void ConstructBitwiseFlagsFromTextDef(TInt& aFindFlags,TInt& aIdentityColumnsCount,const CContactTextDef* aTextDef);
	void ConstructBitwiseFindFlags(TInt& aFindFlags,TInt& aIdentityColumnsCount,const CContactItemFieldDef* aFieldDef);

private:
	enum TFindFieldFlags
		{
		EFindInAllFields				=0x00000001,
		EFindFirstName					=0x00000002,
		EFindLastName					=0x00000004,
		EFindCompanyName				=0x00000008,
		EFindInEmailTableOnly			=0x00000010,
		EFindInAnyIdentityField			=0x00000020,
		EFindFirstNamePronunciation		=0x00000040,
		EFindLastNamePronunciation		=0x00000080,
		EFindCompanyNamePronunciation	=0x00000100,
		};

	MLplViewIteratorManager& iManager;
	TInt iCurrentItemId;
	/**	Mask of tables required for returning a view. */
	MLplViewIteratorManager::TTableMask	iTableMask;
	TContactViewPreferences	iViewPreferences;
	const CContactTextDef& iTextDef;
	};


#endif // CVIEWITERATOR_H
