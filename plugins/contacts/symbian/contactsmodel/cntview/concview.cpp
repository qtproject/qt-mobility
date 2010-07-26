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


//
// CContactConcatenatedView.
//

CContactConcatenatedView::~CContactConcatenatedView()
	{
	CloseComponentViews();
	}

CContactConcatenatedView::CContactConcatenatedView(const CContactDatabase& aDb) : CContactViewBase(aDb)
	{
	}

EXPORT_C CContactConcatenatedView* CContactConcatenatedView::NewL(MContactViewObserver& aObserver,const CContactDatabase& aDb,RPointerArray<CContactViewBase>& aComponentViews)
/** Allocates and constructs a new CContactConcatenatedView object,

@param aObserver An observer that receives notifications when this view is 
ready for use and when changes take place in it. The observer receives a TContactViewEvent::EReady 
event when the view is ready. An attempt to use the view before this notification 
causes a panic.
@param aDb The database that contains the contact items.
@param aComponentViews An array of pointers to the component views to append 
to this view. The concatenated view creates its own copy of this array and 
sets itself to be the observer of each of the component views, unless an error 
occurs, in which case the function leaves.
@return The newly constructed concatenated view object. */
	{
#ifdef CNTVIEW_API_PROFILING
	RDebug::Print(_L("[CNTMODEL] CContactConcatenatedView::NewL()\n"));
#endif
	CContactConcatenatedView* self=new(ELeave) CContactConcatenatedView(aDb);
	CleanupStack::PushL(self);
	self->ConstructL(aObserver,aComponentViews);
	CleanupStack::Pop(); // self.
	return self;
	}

/*
 * This is a reserved virtual exported function that is used for BC proofing 
 * against present and future additions of new exported virtual functions.
 @return Any return values of the helper functions called from this function or NULL.
 */
TAny* CContactConcatenatedView::CContactViewBase_Reserved_1(TFunction aFunction,TAny* aParams)
	{
	return CContactViewBase::CContactViewBase_Reserved_1(aFunction,aParams);
	}
void CContactConcatenatedView::ConstructL(MContactViewObserver& aObserver,RPointerArray<CContactViewBase>& aComponentViews)
	{
	CContactViewBase::ConstructL();
	OpenL(aObserver);
	CopyComponentViewsL(aComponentViews);
	OpenComponentViewsL();
	}

void CContactConcatenatedView::CopyComponentViewsL(RPointerArray<CContactViewBase>& aComponentViews)
	{
	const TInt numComponentViews=aComponentViews.Count();
	for (TInt ii=0;ii<numComponentViews;++ii)
		{
		User::LeaveIfError(iComponentView.Append(TComponentView(*aComponentViews[ii],EInitializing)));
		}
	}

void CContactConcatenatedView::OpenComponentViewsL()
	{
	TInt err=KErrNone;
	const TInt numComponentViews=iComponentView.Count();
	TInt ii;
	for (ii=0;ii<numComponentViews;++ii)
		{
		err=iComponentView[ii].iView.Open(*this);
		if (err)
			{
			goto cleanup;
			}
		}
	return;

cleanup:
	for (TInt jj=0;jj<ii;++jj)
		{
		iComponentView[jj].iView.Close(*this);
		}
	User::Leave(err);
	}

void CContactConcatenatedView::CloseComponentViews()
	{
	const TInt numComponentViews=iComponentView.Count();
	for (TInt ii=0;ii<numComponentViews;++ii)
		{
		iComponentView[ii].iView.Close(*this);
		}
	iComponentView.Close();
	}

TBool CContactConcatenatedView::ComponentViewsReady()
	{
	const TInt numComponentViews=iComponentView.Count();
	for (TInt ii=0;ii<numComponentViews;++ii)
		{
		if (iComponentView[ii].iState!=EReady)
			{
			return EFalse;
			}
		}
	return ETrue;
	}

void CContactConcatenatedView::SetComponentViewReady(const CContactViewBase& aView)
	{
	iComponentView[FindComponentView(aView)].iState=EReady;
	}

TBool CContactConcatenatedView::ComponentViewsEqual(const TComponentView& aFirst,const TComponentView& aSecond)
	{
	if (&aFirst.iView==&aSecond.iView)
		{
		return ETrue;
		}
	return EFalse;
	}

TInt CContactConcatenatedView::FindComponentView(const CContactViewBase& aView)
	{
	TComponentView view(CONST_CAST(CContactViewBase&,aView),EReady); // Note, find ignores TComponentView::iState.
	TInt index=iComponentView.Find(view,TIdentityRelation<TComponentView>(ComponentViewsEqual));
	__ASSERT_DEBUG(index!=KErrNotFound,Panic(ECntPanicComponentViewNotFound));
	return index;
	}

TContactItemId CContactConcatenatedView::AtL(TInt aIndex) const
/** Gets the contact item ID at the specified index into the concatenated view.

@param aIndex Index into the concatenated view.
@leave KErrNotReady The view is not ready for use.
@leave KErrNotFound The index is out of bounds. It must be less than the 
total number of items in all component views.
@return The contact item ID. 
*/
	{
	if (iState != EReady)
		{
		User::Leave(KErrNotReady);
		}
	TInt count=0;
	const TInt numComponentViews=iComponentView.Count();
	for (TInt ii=0;ii<numComponentViews;++ii)
		{
		const TInt thisCount=iComponentView[ii].iView.CountL();
		if (aIndex<(count+thisCount))
			{
			return iComponentView[ii].iView.AtL(aIndex-count);
			}
		count+=thisCount;
		}
	//Out of Bounds 
	User::Leave(KErrNotFound);
	return KErrNotFound;
	}


const CViewContact& CContactConcatenatedView::ContactAtL(TInt aIndex) const
/** Gets the contact item at the specified index into the view.

@param aIndex Index into the view.
@leave KErrNotReady The view is not ready for use. 
@leave KErrNotFound The index is invalid. It must be less that the total 
number of items in the concatenated view.
@return The contact item. 
*/
	{
	if (iState != EReady)
		{
		User::Leave(KErrNotReady);
		}
	TInt count=0;
	const TInt numComponentViews=iComponentView.Count();
	for (TInt ii=0;ii<numComponentViews;++ii)
		{
		const TInt thisCount=iComponentView[ii].iView.CountL();
		if (aIndex<(count+thisCount))
			{
			return iComponentView[ii].iView.ContactAtL(aIndex-count);
			}
		count+=thisCount;
		}
	//Out of Bounds 
	User::Leave(KErrNotFound);
	// the following code is never executed and is purely to stop the compiler warnings
	const CViewContact* nullContact=NULL;
	return *nullContact;
	}

TInt CContactConcatenatedView::CountL() const
/** Gets the number of contact items in the concatenated view.

This is the total number of items in all component views.
@leave KErrNotReady The view is not ready for use.
@return The number of items in the concatenated view. 
*/
	{
	if (iState != EReady)
		{
		User::Leave(KErrNotReady);
		}
	TInt count=0;
	const TInt numComponentViews=iComponentView.Count();
	for (TInt ii=0;ii<numComponentViews;++ii)
		{
		count+=iComponentView[ii].iView.CountL();
		}
	return count;
	}

TInt CContactConcatenatedView::FindL(TContactItemId aId) const
/** Finds the index into the concatenated view of the specified contact item.

@param aId The contact item ID to search for.
@leave KErrNotReady The view is not ready for use.
@return The index of the first matching item in the concatenated view or KErrNotFound 
if no matching item can be found. 
*/
	{
	if (iState != EReady)
		{
		User::Leave(KErrNotReady);
		}
	TInt count=0;
	const TInt numComponentViews=iComponentView.Count();
	for (TInt ii=0;ii<numComponentViews;++ii)
		{
		TInt index=iComponentView[ii].iView.FindL(aId);
		if (index!=KErrNotFound)
			{
			return count+index;
			}
		count+=iComponentView[ii].iView.CountL();
		}
	return KErrNotFound;
	}

HBufC* CContactConcatenatedView::AllFieldsLC(TInt aIndex,const TDesC& aSeparator) const
/** Returns a descriptor containing the contents of all fields in an item in the 
view.

The fields are separated by aSeparator.

@param aIndex The index into the view of the contact item.
@param aSeparator The string to use to separate the fields.
@leave KErrNotReady The view is not ready for use.
@return Pointer to the contact item descriptor.
@panic CNTMODEL 29 In debug builds, this indicates that the index is invalid. 
It must be less that the total number of items in the concatenated view. 
In release builds, a NULL pointer is returned instead. 
*/
	{
	if (iState != EReady)
		{
		User::Leave(KErrNotReady);
		}
	TInt count=0;
	const TInt numComponentViews=iComponentView.Count();
	for (TInt ii=0;ii<numComponentViews;++ii)
		{
		count+=iComponentView[ii].iView.CountL();
		if (aIndex<count)
			{
			return iComponentView[ii].iView.AllFieldsLC(aIndex-count,aSeparator);
			}
		}
	__ASSERT_DEBUG(EFalse,Panic(ECntPanicInvalidIndexForConcatenatedView));
	return NULL;	
	}


void CContactConcatenatedView::HandleContactViewEvent(const CContactViewBase& aView,const TContactViewEvent& aEvent)
	{
	TBool passOnNotification=ETrue;
	TContactViewEvent event=aEvent;
	
	TInt err;

	switch (event.iEventType)
		{
		case TContactViewEvent::ESortError:
		case TContactViewEvent::EServerError:
		case TContactViewEvent::EUnavailable:
			iState=ENotReady;
			break;
		case TContactViewEvent::EReady:
			SetComponentViewReady(aView);
			if (ComponentViewsReady()) // Need to wait for all views to be ready before changing state.
				{
				iState=EReady;
				}
			else
				{
				passOnNotification=EFalse;
				}
			break;
		case TContactViewEvent::ESortOrderChanged:
			iState=EReady;
			break;
		case TContactViewEvent::EItemAdded:
		case TContactViewEvent::EItemRemoved:
			TRAP(err,event.iInt+=OffsetL(aView));
			if (err != KErrNone)
				{
				NotifyObservers(TContactViewEvent(TContactViewEvent::ESortError,err));
				passOnNotification = EFalse;
				}
			break;
		case TContactViewEvent::EGroupChanged:
			break;
		default:
			ASSERT(EFalse);
		}

	if (passOnNotification)
		{
		NotifyObservers(event);
		}
	}

TInt CContactConcatenatedView::OffsetL(const CContactViewBase& aView)
	{
	const TInt eventOwningViewIndex=FindComponentView(aView);
	TInt offset=0;
	for (TInt ii=0;ii<eventOwningViewIndex;++ii)
		{
		offset+=iComponentView[ii].iView.CountL();
		}
	return offset;
	}

TContactViewPreferences CContactConcatenatedView::ContactViewPreferences()
/** Gets the view preferences for the first component view.
@return The view preferences. 
*/
	{
	return iComponentView[0].iView.ContactViewPreferences();
	}

const RContactViewSortOrder& CContactConcatenatedView::SortOrderL() const
/** Gets the sort order for the first component view.

@return The sort order. */
	{
	return iComponentView[0].iView.SortOrderL();
	}

CContactConcatenatedView::TComponentView::TComponentView(CContactViewBase& aView,TState aState) : iView(aView),iState(aState) 
	{
	}
