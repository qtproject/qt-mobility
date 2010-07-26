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


//#define CNTVIEW_API_PROFILING
// To see the diferences between class versions check the in source documentation of TContactViewEvent
const TUint KClassVersion1 = 1;
const TUint KClassVersion2 = 2;

//
// CContactSubView.
//

CContactSubView::CContactSubView(const CContactDatabase& aDb,CContactViewBase& aView)
	: CContactViewBase(aDb),iView(aView), iClassVersion(KClassVersion1)
	{
	}

CContactSubView::CContactSubView(const CContactDatabase& aDb,const CContactSubView& aView)
	: CContactViewBase(aDb),iView(aView.iView)
	{
	}

CContactSubView::~CContactSubView()
	{
	iView.Close(*this);
	delete iRange;
	iRange=NULL;
	}

EXPORT_C CContactSubView* CContactSubView::NewL(MContactViewObserver& aObserver,const CContactDatabase& aDb,CContactViewBase& aView,const TDesC& aBoundary)
/** Allocates and constructs a new CContactSubView version 1 object, specifying the sub view's
criteria.

When adding/deleting contacts in the view, MContactViewObserver observer will receive 
TContactViewEvent events with iInt parameter set to KErrNone.

@param aObserver An observer that receives notifications when this view is 
ready for use and when changes take place in it. The observer receives a TContactViewEvent::EReady 
event when the view is ready. An attempt to use the view before this notification 
causes a panic.
@param aDb The database containing the contact items.
@param aView The underlying view.
@param aBoundary A string containing the sub view criteria. Possible values 
are: <, >, <=, or >=, followed by a character.
@return The newly constructed sub view object. */
	{
#ifdef CNTVIEW_API_PROFILING
	RDebug::Print(_L("[CNTMODEL] CContactSubView::NewL()\n"));
#endif
	CContactSubView* self=new(ELeave) CContactSubView(aDb,aView);
	CleanupStack::PushL(self);
	self->ConstructL(aObserver,aBoundary);
	CleanupStack::Pop(self); 
	return self;
	}

EXPORT_C CContactSubView* CContactSubView::NewL(MContactViewObserver& aObserver,const CContactDatabase& aDb,CContactViewBase& aView,const TDesC& aLowBoundary,const TDesC& aHighBoundary)
/** Allocates and constructs a new CContactSubView version 1 object, specifying the sub view's 
upper and lower boundary criteria.

When adding/deleting contacts in the view, MContactViewObserver observer will receive 
TContactViewEvent events with iInt parameter set to KErrNone.

@param aObserver An observer that receives notifications when this view is 
ready for use and when changes take place in it. The observer receives a TContactViewEvent::EReady 
event when the view is ready. An attempt to use the view before this notification 
causes a panic.
@param aDb The database containing the contact items.
@param aView The underlying view.
@param aLowBoundary A string containing the sub view's lower boundary criteria. 
Possible values are: > or >=, followed by a character.
@param aHighBoundary A string containing the sub view's upper boundary criteria. 
Possible values are: < or <=, followed by a character.
@return The newly constructed sub view object. */
	{
#ifdef CNTVIEW_API_PROFILING
	RDebug::Print(_L("[CNTMODEL] CContactSubView::NewL()\n"));
#endif
	CContactSubView* self=new(ELeave) CContactSubView(aDb,aView);
	CleanupStack::PushL(self);
	self->ConstructL(aObserver,aLowBoundary,aHighBoundary);
	CleanupStack::Pop(self); 
	return self;
	}

EXPORT_C CContactSubView* CContactSubView::NewL(CContactViewBase& aView,const CContactDatabase& aDb,MContactViewObserver& aObserver,const TDesC& aBoundary)
/** Allocates and constructs a new CContactSubView version 2 object, specifying the sub view's 
criteria.

When adding/deleting contacts in the view, MContactViewObserver observer will receive 
TContactViewEvent events with iInt parameter set to index into the observed view of the added/deleted item

@param aObserver An observer that receives notifications when this view is 
ready for use and when changes take place in it. The observer receives a TContactViewEvent::EReady 
event when the view is ready. An attempt to use the view before this notification 
causes a panic.
@param aDb The database containing the contact items.
@param aView The underlying view.
@param aBoundary A string containing the sub view criteria. Possible values 
are: <, >, <=, or >=, followed by a character.
@return The newly constructed sub view object. */
	{
#ifdef CNTVIEW_API_PROFILING
	RDebug::Print(_L("[CNTMODEL] CContactSubView::NewL()\n"));
#endif
	CContactSubView* self=new(ELeave) CContactSubView(aDb,aView);
	CleanupStack::PushL(self);
	self->ConstructL(aObserver,aBoundary);
	self->iClassVersion = KClassVersion2;
	CleanupStack::Pop(self); 
	return self;
	}

EXPORT_C CContactSubView* CContactSubView::NewL(CContactViewBase& aView,const CContactDatabase& aDb,MContactViewObserver& aObserver,const TDesC& aLowBoundary,const TDesC& aHighBoundary)
/** Allocates and constructs a new CContactSubView version 2 object, specifying the sub view's
upper and lower boundary criteria.

When adding/deleting contacts in the view, MContactViewObserver observer will receive 
TContactViewEvent events with iInt parameter set to index into the observed view of the added/deleted item

@param aObserver An observer that receives notifications when this view is 
ready for use and when changes take place in it. The observer receives a TContactViewEvent::EReady 
event when the view is ready. An attempt to use the view before this notification 
causes a panic.
@param aDb The database containing the contact items.
@param aView The underlying view.
@param aLowBoundary A string containing the sub view's lower boundary criteria. 
Possible values are: > or >=, followed by a character.
@param aHighBoundary A string containing the sub view's upper boundary criteria. 
Possible values are: < or <=, followed by a character.
@return The newly constructed sub view object. */
	{
#ifdef CNTVIEW_API_PROFILING
	RDebug::Print(_L("[CNTMODEL] CContactSubView::NewL()\n"));
#endif
	CContactSubView* self=new(ELeave) CContactSubView(aDb,aView);
	CleanupStack::PushL(self);
	self->ConstructL(aObserver,aLowBoundary,aHighBoundary);
	self->iClassVersion = KClassVersion2;
	CleanupStack::Pop(self);
	return self;
	}

/**
 * This is a reserved virtual exported function that is used for BC proofing 
 * against present and future additions of new exported virtual functions.
 @return Any return values of the helper methods called from this function or NULL.
 */
TAny* CContactSubView::CContactViewBase_Reserved_1(TFunction aFunction,TAny* aParams)
	{
	return CContactViewBase::CContactViewBase_Reserved_1(aFunction,aParams);
	}

void CContactSubView::CommonConstructL(MContactViewObserver& aObserver)
	{
	CContactViewBase::ConstructL();
	OpenL(aObserver);
	iView.OpenL(*this);
	}

void CContactSubView::ConstructL(MContactViewObserver& aObserver,const TDesC& aBoundary)
	{
	CommonConstructL(aObserver);
	TBuf<KMaxBoundaryMatchLength+2> boundary(aBoundary);
	CContactViewRangeBase::TCriteria criteria(DecodeBoundary(boundary));
	switch (criteria)
		{
		case CContactViewRangeBase::ELessThan:
		case CContactViewRangeBase::ELessThanOrEqualTo:
			iRange=NULL;
			iRange=CContactViewLowRange::NewL(iView,boundary,criteria);
			break;
		case CContactViewRangeBase::EGreaterThan:
		case CContactViewRangeBase::EGreaterThanOrEqualTo:
			iRange=NULL;
			iRange=CContactViewHighRange::NewL(iView,boundary,criteria);
			break;
		default:
			ASSERT(EFalse);
		}
	}

void CContactSubView::ConstructL(MContactViewObserver& aObserver,const TDesC& aLowBoundary,const TDesC& aHighBoundary)
	{
	CommonConstructL(aObserver);
	TBuf<KMaxBoundaryMatchLength+2> lowBoundary(aLowBoundary);
	CContactViewRangeBase::TCriteria lowCriteria(DecodeBoundary(lowBoundary));
	TBuf<KMaxBoundaryMatchLength+2> highBoundary(aHighBoundary);
	CContactViewRangeBase::TCriteria highCriteria(DecodeBoundary(highBoundary));
	iRange=NULL;
	iRange=CContactViewRange::NewL(iView,lowBoundary,lowCriteria,highBoundary,highCriteria);
	}


CContactViewRangeBase::TCriteria CContactSubView::DecodeBoundary(TDes& aBoundary) const
	{
	CContactViewRangeBase::TCriteria criteria(CContactViewRangeBase::ELessThan);

	switch (aBoundary[0])
		{
		case '<':
			criteria=CContactViewRangeBase::ELessThan;
			break;
		case '>':
			criteria=CContactViewRangeBase::EGreaterThan;
			break;
		default:
			__ASSERT_DEBUG(EFalse,Panic(ECntPanicNoViewIndexMatchCriteria));
		}

	aBoundary.Delete(0,1);	// Delete first char.

	if (aBoundary[0]=='=')
		{
		switch (criteria)
			{
			case CContactViewRangeBase::ELessThan:
				criteria=CContactViewRangeBase::ELessThanOrEqualTo;
				break;
			case CContactViewRangeBase::EGreaterThan:
				criteria=CContactViewRangeBase::EGreaterThanOrEqualTo;
				break;
			}
		aBoundary.Delete(0,1);	// Delete first char.
		}

	return criteria;
	}

TContactItemId CContactSubView::AtL(TInt aIndex) const
/** Gets the contact item ID at the specified index into the sub view.

In release builds, zero is returned if the sub view's upper and lower boundaries 
have not been set, (in debug builds, a panic occurs).

@param aIndex Index into the sub view of a contact item ID. 
@leave KErrNotFound aIndex is outside the bounds of the sub view's array.
@return The contact item ID. */
	{
	if (iRange->IndicesValid())
		{
		if(!(iRange->LowIndex()+aIndex<=iRange->HighIndex()))
			{
			//Out of Bounds 
			User::Leave(KErrNotFound);
			}	
		return iView.AtL(MapToUnderlyingViewIndex(aIndex));
		}
	__ASSERT_DEBUG(EFalse,Panic(ECntPanicInvalidIndexForSubView));
	return 0;
	}

const CViewContact& CContactSubView::ContactAtL(TInt aIndex) const
/** Gets the contact item at the specified index into the sub view.

A NULL contact item is returned if the sub view's upper and lower boundaries 
have not been set (in debug builds, a panic occurs).

@param aIndex Index into the sub view of the required item.
@leave KErrNotFound aIndex is outside the bounds of the sub view's array.
@return The contact item. */
	{
	if (iRange->IndicesValid())
		{
		if(!(iRange->LowIndex()+aIndex<=iRange->HighIndex()))
			{
			//Out of Bounds 
			User::Leave(KErrNotFound);
			}	
		return iView.ContactAtL(MapToUnderlyingViewIndex(aIndex));
		}
	__ASSERT_DEBUG(EFalse,Panic(ECntPanicInvalidIndexForSubView));
	// the following code is never executed and is purely to stop the compiler warnings
	const CViewContact* nullContact=NULL;
	return *nullContact;
	}

TInt CContactSubView::CountL() const
/** Gets the number of contact item IDs in the sub view.

Zero is returned if the sub view's upper and lower boundaries have not been 
set.

@return The number of contact item IDs in the sub view. */
	{
	if (iRange->IndicesValid())
		{
		return iRange->HighIndex()-iRange->LowIndex()+1;
		}
	return 0;
	}

TInt CContactSubView::FindL(TContactItemId aId) const
/** Finds the index into the sub view of the specified contact item.

@param aId The contact item ID to search for. 
@return The index of the first matching item in the sub view or KErrNotFound 
if the sub view's upper and lower boundaries have not been set or if the item 
is not in the sub view. */
	{
	const TInt index=iView.FindL(aId);
	if (iRange->IndicesValid() && index>=iRange->LowIndex() && index<=iRange->HighIndex())
		{
		return MapToSubViewIndex(index);
		}
	return KErrNotFound;
	}

HBufC* CContactSubView::AllFieldsLC(TInt aIndex,const TDesC& aSeparator) const
/** Returns a descriptor containing the contents of all fields for an item in the 
sub view.

NULL is returned if the sub view's upper and lower boundaries have not been 
set (in debug builds, a panic occurs).

@param aIndex The index into the sub view of the contact item.
@param aSeparator The string to use to separate the fields.
@return Pointer to the contact item descriptor. */
	{
	if (iRange->IndicesValid())
		{
		return iView.AllFieldsLC(MapToUnderlyingViewIndex(aIndex),aSeparator);
		}
	__ASSERT_DEBUG(EFalse,Panic(ECntPanicInvalidIndexForSubView));
	return NULL;
	}

const RContactViewSortOrder& CContactSubView::SortOrderL() const
/** Gets the underlying view's sort order.

@return The sort order. */
	{
	return iView.SortOrderL();
	}

TContactViewPreferences CContactSubView::ContactViewPreferences()
/** Gets the underlying view's view preferences.

@return The view preferences. */
	{
	return iView.ContactViewPreferences();
	}

#ifdef _DEBUG
void CContactSubView::HandleContactViewEvent(const CContactViewBase& aView,const TContactViewEvent& aEvent)
#else
void CContactSubView::HandleContactViewEvent(const CContactViewBase& /*aView*/,const TContactViewEvent& aEvent)
#endif
	{
	ASSERT(&aView==&iView);
	TBool notifyObservers=ETrue;
	TContactViewEvent event=aEvent;
	switch (event.iEventType)
		{
		case TContactViewEvent::EUnavailable:
		case TContactViewEvent::ESortError:
		case TContactViewEvent::EServerError:
			iState=ENotReady;
			break;
		case TContactViewEvent::EReady:
		case TContactViewEvent::ESortOrderChanged:
			{
			TRAPD(err,iRange->SetL());
			if (err)
				{
				event.iEventType=TContactViewEvent::EIndexingError;
				event.iInt=err;
				}
			else
				{
				iState=EReady;
				}
			}
			break;
		case TContactViewEvent::EItemAdded:
		case TContactViewEvent::EItemRemoved:
//			notifyObservers=iRange->Update(event);
			TRAPD(err,iRange->SetL());
			if (err)
				{
				event.iEventType=TContactViewEvent::EIndexingError;
				event.iInt=err;
				}
			else
				{
				//get the contact index within subview
				if(iClassVersion == KClassVersion2)
					{
					if(iRange->LowIndex() == KErrNotFound)
						{
						notifyObservers = EFalse;
						}
					else
						{
						event.iInt -= iRange->LowIndex();
						}
					}
				else
					{
					event.iInt = KErrNone;
					}
				}	
			break;
		case TContactViewEvent::EGroupChanged:
			break;
		default:
			ASSERT(EFalse);
		}

	if (notifyObservers)
		{
		NotifyObservers(event);
		}
	}

TInt CContactSubView::MapToUnderlyingViewIndex(TInt aSubViewIndex) const
	{
	ASSERT(iRange->IndicesValid());
	return aSubViewIndex+iRange->LowIndex();
	}

TInt CContactSubView::MapToSubViewIndex(TInt aUnderlyingViewIndex) const
	{
	ASSERT(iRange->IndicesValid());
	return aUnderlyingViewIndex-iRange->LowIndex();
	}
