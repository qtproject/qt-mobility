/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "cntvcardutils.h"

// System Includes
#include <e32base.h>

// User includes
#include <cntitem.h>

// Constants
const TInt KCntVCardItemAndLabelGranularity = 7;


/**
 * Panic the thread with CNTVCARD as the category
 * 
 * @param aPanic Panic number
 */
GLDEF_C void Panic(TCntVCardPanic aPanic)
	{
	_LIT(KCntVCardPanicCategory,"CNTVCARD");
	User::Panic(KCntVCardPanicCategory,aPanic);
	}

/**
 * Array clean-up method 
 */
GLDEF_C void CleanUpResetAndDestroy(TAny *aArray)
	{
	if (aArray)
		{
		CArrayPtr<CContactItem>* array=(CArrayPtr<CContactItem>*)aArray;
		array->ResetAndDestroy();
		delete array;
		}
	}


/**
 * Standard Epoc32 Dll Entry point
 */




/**
 * Constructor
 */
CVCardItemAndLabel::CVCardItemAndLabel()
	{
	}

/**
 * Destructor
 */
CVCardItemAndLabel::~CVCardItemAndLabel()
	{
	delete iItems;
	delete iLabels;
	}

/**
 * Second phase constructor
 */
void CVCardItemAndLabel::ConstructL()
	{
	iItems = new(ELeave) CDesCArrayFlat(KCntVCardItemAndLabelGranularity);
	iLabels = new(ELeave) CDesCArrayFlat(KCntVCardItemAndLabelGranularity);
	}

/**
 * Static constructor
 * @return The constructed item and label
 */
CVCardItemAndLabel* CVCardItemAndLabel::NewLC()
	{
	CVCardItemAndLabel* self = new(ELeave) CVCardItemAndLabel();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

/**
 * Add an item
 * @param aItem The item name
 */
void CVCardItemAndLabel::AddItemL(const TDesC& aItem)
	{
	iItems->AppendL(aItem);
	}

/**
 * Add a label for an item
 * @param aLabel The item label
 */
void CVCardItemAndLabel::AddLabelL(const TDesC& aLabel)
	{
	iLabels->AppendL(aLabel);
	}

/**
 * The number of items present
 * @return The number of items present
 */
TInt CVCardItemAndLabel::ItemCount() const
	{
	return iItems->Count();
	}

/**
 * The number of labels present
 * @return The number of labels present
 */
TInt CVCardItemAndLabel::LabelCount() const
	{
	return iLabels->Count();
	}

/**
 * Access an item by index
 * @return The item at the specified index
 */
TPtrC CVCardItemAndLabel::Item(TInt aIndex) const
	{
	return iItems->MdcaPoint(aIndex);
	}

/**
 * Access a label by index
 * @return The label at the specified index
 */
TPtrC CVCardItemAndLabel::Label(TInt aIndex) const
	{
	return iLabels->MdcaPoint(aIndex);
	}

/**
 * Locate the index of a specified label.
 * @param aName The label being searched for
 * @param aPosition A reference parameter which is updated with the position of the
 *                  item if it is located.
 * @return An error code indicating whether the item was found or otherwise
 */
TInt CVCardItemAndLabel::FindLabel(const TDesC& aName, TInt& aPosition) const
	{
	return iLabels->Find(aName, aPosition);
	}

/**
 * Utility cleanup method
 * @param aArray An object of RPointerArray<CVCardItemAndLabel> which must be cleaned up
 */
void CVCardItemAndLabel::CleanUpResetDestroyAndCloseArray(TAny* aArray)
	{
	RPointerArray<CVCardItemAndLabel>* array = reinterpret_cast<RPointerArray<CVCardItemAndLabel>*>(aArray);
	if	(array)
		{
		array->ResetAndDestroy();
		array->Close();
		}
	}






/**
 * Constructor
 */
CVCardAddress::CVCardAddress(TUid aMapping)
:	iMapping(aMapping)
	{
	}

/**
 * Static constructor
 */
CVCardAddress* CVCardAddress::NewLC(TUid aMapping)
	{
	CVCardAddress* self = new(ELeave) CVCardAddress(aMapping);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
