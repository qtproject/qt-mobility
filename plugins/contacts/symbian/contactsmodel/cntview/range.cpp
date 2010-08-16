/*
* Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <cntview.h>
#include "cntstd.h"
#include <collate.h>



//#define CNTVIEW_API_PROFILING


//
// CContactViewRangeBase
//

/**
@internalComponent
*/
CContactViewRangeBase::CContactViewRangeBase(const CContactViewBase& aView)
	: iView(aView),iLow(KErrNotFound),iHigh(KErrNotFound)
	{
	}

CContactViewRangeBase::~CContactViewRangeBase()
	{
	delete iCollateMethod;
	}

void CContactViewRangeBase::ConstructL()
/** Called by derived classes to set the collation method iCollateMethod to be 
the standard collation method for the current locale.
@internalComponent
*/
	{
	// Needed to ensure unicode sorting / inserting into sorted lists works the same as ER5,
	// i.e. that it encludes all spaces and punctuation.
	iCollateMethod = new(ELeave) TCollationMethod;
	*iCollateMethod = *Mem::CollationMethodByIndex(0);
	iCollateMethod->iFlags|=TCollationMethod::EIgnoreNone;
	}

EXPORT_C TBool CContactViewRangeBase::IndicesValid() const
/** Tests whether the lower and upper indexes are valid, i.e. not KErrNotFound.

@return ETrue if the indexes are valid, EFalse if not. */
	{
	if (iLow==KErrNotFound)
		{
		ASSERT(iHigh==KErrNotFound);
		return EFalse;
		}
	return ETrue;
	}

TInt CContactViewRangeBase::FindIndexL(const TDesC& aMatch,TCriteria aCriteria) const
// Binary search through iView to find the first contact that matches aMatch according to aCritera.
/** Called by implementations of SetL() to search the sub view's underlying view 
for the index of the first item whose field content matches aMatch, using 
the specified search criteria.

@internalComponent
@param aMatch The text to search for. 
@param aCriteria The search criteria. 
@return The index of the first matching item in the view. KErrNotFound if the 
view is empty or if none match. */
	{
	const TInt NumberOfCharsToCompare=1;
	TInt result=KErrNotFound;
	TInt min=0;
	TInt max=iView.CountL()-1;
	if (max<0)
		{
		// The parent view is empty.
		return KErrNotFound;
		}
	TBool finished=EFalse;

	HBufC* sortableText;
	while (!finished)
		{
		TInt guess;
		if (max==min)
			{
			finished=ETrue;
			}

		guess=(max-min)/2+min;
		sortableText = iView.AllFieldsLC(guess,KNullDesC);
		TInt compare=sortableText->Left(NumberOfCharsToCompare).CompareC(aMatch.Left(NumberOfCharsToCompare),3,iCollateMethod);
		CleanupStack::PopAndDestroy();//sortableText
		switch (aCriteria)
			{
			case ELessThan: 
				{ 
				if (compare>0 || compare==0)
					{
					// String is greater than aMatch.
					max=guess-1;
					if (max<min)
						{
						finished=ETrue;
						}
					}
				else if (compare<0)
					{
					// String is less than aMatch - could be a hit.
					min=guess+1;
					if (min>max)
						{
						finished=ETrue;
						}
					if (result==KErrNotFound || guess>result)
						{
						// guess is a closer match, so overwrite result.
						result=guess;
						}
					}
				break;
				}
			case ELessThanOrEqualTo: 
				{
				if (compare>0)
					{
					// String is greater than aMatch.
					max=guess-1;
					if (max<min)
						{
						finished=ETrue;
						}
					}
				else if (compare<=0)
					{
					// String is less than or equal to aMatch - possible hit.
					min=guess+1;
					if (min>max)
						{
						finished=ETrue;
						}
					if (result==KErrNotFound || guess>result)
						{
						// guess is a closer match, so overwrite result.
						result=guess;
						}
					}
				break;
				}
			case EGreaterThan: 
				{
				if (compare<0 || compare==0)
					{
					// String is less than aMatch.
					min=guess+1;
					if (min>max)
						{
						finished=ETrue;
						}
					}
				else if (compare>0)
					{
					// String is greater than aMatch - possible hit.
					max=guess-1;
					if (max<min)
						{
						finished=ETrue;
						}
					if (result==KErrNotFound || guess<result)
						{
						// guess is a closer match, so overwrite result.
						result=guess;
						}
					}
				break;
				}
			case EGreaterThanOrEqualTo:
				{
				if (compare<0)
					{
					// String is less than aMatch.
					min=guess+1;
					if (min>max)
						{
						finished=ETrue;
						}
					}
				else if (compare>=0)
					{
					// String is >= aMatch - possible hit.
					max=guess-1;
					if (max<min)
						{
						finished=ETrue;
						}
					if (result==KErrNotFound || guess<result)
						{
						// guess is a closer match, so overwrite result.
						result=guess;
						}
					}
				break;
				}
			};
		}
	return result;
	}

/**
@internalComponent
*/
TBool CContactViewRangeBase::MatchesCriteriaL(TCriteria aCriteria,const TDesC& aMatch,TInt aIndex) const
	{
	TBool match=EFalse;
	HBufC* sortableText=iView.AllFieldsLC(aIndex,KNullDesC);
	TInt compare=sortableText->CompareC(aMatch,3,iCollateMethod);

	switch(aCriteria)
		{
		case ELessThan:
			{
			if(compare<0)
				{
				//RDebug::Print(_L("Match of %S against < %S PASSED\x0D\x00\x0a\x00"),&sortableText,&aMatch);
				match=ETrue;
				}
			else
				{
				//RDebug::Print(_L("Match of %S against < %S FAILED\x0D\x00\x0a\x00"),&sortableText,&aMatch);
				}
			break;
			};
		case ELessThanOrEqualTo:
			{
			if(compare<=0)
				match=ETrue;
			break;
			};
		case EGreaterThan:
			{
			if(compare>0)
				match=ETrue;
			break;
			};
		case EGreaterThanOrEqualTo:
			{
			if(compare>=0)
				{
				//RDebug::Print(_L("Match of %S against >= %S PASSED\x0D\x00\x0a\x00"),&sortableText,&aMatch);
				match=ETrue;
				}
			else
				{
				//RDebug::Print(_L("Match of %S against >= %S FAILED\x0D\x00\x0a\x00"),&sortableText,&aMatch);
				}
			break;
			};
		};

	return match;
	}

void CContactViewRangeBase::ValidateIndices()
/** Called by derived classes to validate the upper and lower indexes (iLow and 
iHigh).

If either index is KErrNotFound, or iLow is greater than iHigh, then both 
are set to KErrNotFound. 
@internalComponent
*/
	{
	// Check that range isn't inside out, and both ranges are valid.
	if (iLow>iHigh || iHigh==KErrNotFound || iLow==KErrNotFound)
		{
		// Invalidate both indicies.
		iLow=KErrNotFound;
		iHigh=KErrNotFound;
		}
	}

//
// CContactViewRange
//


CContactViewRange::CContactViewRange(const CContactViewBase& aView,const TDesC& aLowMatch,TCriteria aLowCriteria,const TDesC& aHighMatch,TCriteria aHighCriteria)
	:CContactViewRangeBase(aView),iLowMatch(aLowMatch),iLowCriteria(aLowCriteria),iHighMatch(aHighMatch),iHighCriteria(aHighCriteria)
	{
	}

EXPORT_C CContactViewRange* CContactViewRange::NewL(const CContactViewBase& aView,const TDesC& aLowMatch,TCriteria aLowCriteria,const TDesC& aHighMatch,TCriteria aHighCriteria)
/** Allocates and constructs a new sub view range.

@param aView The sub view's underlying view. 
@param aLowMatch The match string for the bottom of the range. Only the first 
character in the string is used.
@param aLowCriteria The query search condition for the bottom of the range; 
either CContactViewRangeBase::EGreaterThan or CContactViewRangeBase::EGreaterThanOrEqualTo.
@param aHighMatch The match string for the top of the range. Only the first 
character in the string is used. 
@param aHighCriteria The query search condition for the top of the range; either 
CContactViewRangeBase::ELessThan or CContactViewRangeBase::ELessThanOrEqualTo.
@return Pointer to a newly created sub view range object    */
	{
#ifdef CNTVIEW_API_PROFILING
	RDebug::Print(_L("[CNTMODEL] CContactViewRange::NewL()\n"));
#endif
	CContactViewRange* self=new(ELeave) CContactViewRange(aView,aLowMatch,aLowCriteria,aHighMatch,aHighCriteria);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self.
	return self;
	}

CContactViewRange::~CContactViewRange()
/** Destructor. */
	{
	}

void CContactViewRange::ConstructL()
	{
	CContactViewRangeBase::ConstructL();
	}

void CContactViewRange::SetL()
	{
	iLow=FindIndexL(iLowMatch,iLowCriteria);
	iHigh=FindIndexL(iHighMatch,iHighCriteria);
	ValidateIndices();
	}


//
// CContactViewLowRange
//

CContactViewLowRange::CContactViewLowRange(const CContactViewBase& aView,const TDesC& aMatch,TCriteria aCriteria)
	: CContactViewRangeBase(aView),iMatch(aMatch),iCriteria(aCriteria)
	{
	}

CContactViewLowRange::~CContactViewLowRange()
	{
	}

EXPORT_C CContactViewLowRange* CContactViewLowRange::NewL(const CContactViewBase& aView,const TDesC& aMatch,TCriteria aCriteria)
/** Allocates and constructs a CContactViewLowRange object.

@param aView The sub view's underlying view.
@param aMatch The string to match items against. Only the first character in 
the string is used.
@param aCriteria The query search condition; either CContactViewRangeBase::ELessThanOrEqualTo 
or CContactViewRangeBase::ELessThan.
@return Pointer to a newly created CContactViewLowRange object  */
	{
#ifdef CNTVIEW_API_PROFILING
	RDebug::Print(_L("[CNTMODEL] CContactViewLowRange::NewL()\n"));
#endif
	CContactViewLowRange* self=new(ELeave) CContactViewLowRange(aView,aMatch,aCriteria);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self.
	return self;
	}

void CContactViewLowRange::SetL()
	{
	iHigh=FindIndexL(iMatch,iCriteria);
	if(iHigh!=KErrNotFound && iHigh >=0)
		{
		iLow=0;
		}
	ValidateIndices();
	}


//
// CContactViewHighRange 
//

CContactViewHighRange::CContactViewHighRange (const CContactViewBase& aView,const TDesC& aMatch,TCriteria aCriteria)
	: CContactViewRangeBase(aView),iMatch(aMatch),iCriteria(aCriteria)
	{
	}

CContactViewHighRange::~CContactViewHighRange()
	{
	}

EXPORT_C CContactViewHighRange * CContactViewHighRange::NewL(const CContactViewBase& aView,const TDesC& aMatch,TCriteria aCriteria)
/** Allocates and constructs a CContactViewHighRange object.

@param aView The sub view's underlying view.
@param aMatch The string to match items against. Only the first character in 
the string is used.
@param aCriteria The query search condition; either CContactViewRangeBase::EGreaterThan 
or CContactViewRangeBase::EGreaterThanOrEqualTo. 
@return Pointer to newly created CContactViewHighRange object  */
	{
#ifdef CNTVIEW_API_PROFILING
	RDebug::Print(_L("[CNTMODEL] CContactViewHighRange::NewL()\n"));
#endif
	CContactViewHighRange * self=new(ELeave) CContactViewHighRange (aView,aMatch,aCriteria);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self.
	return self;
	}

void CContactViewHighRange::SetL()
	{
	iLow=FindIndexL(iMatch,iCriteria);
	if (iLow!=KErrNotFound)
		{
		iHigh=iView.CountL()-1;
		}
	ValidateIndices();
	}
