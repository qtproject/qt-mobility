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
const TUint KClassVersion2 = 2;

/*
 * Threshold to choose whether to use base class implementation of 
 * ContactsMatchingCriteria or use remote view implementation.
 * If the filter view has 59% or more of the parent view, it is 
 * usually quicker to use the parent (remote) view.
 */
const TReal KFilterThreshold = 0.592437 ;

EXPORT_C CContactFilteredView* CContactFilteredView::NewL(MContactViewObserver& aObserver,const CContactDatabase& aDb,CContactViewBase& aView,TInt aFilter)
/** Allocates and constructs a CContactFilteredView version 1 object.

When adding/deleting contacts in the view, MContactViewObserver observer will receive 
TContactViewEvent events with iInt parameter set to index into the observed view of the added/deleted item

@param aObserver An observer that receives notifications when this view is 
ready for use and when changes take place in it. The observer receives a TContactViewEvent::EReady 
event when the view is ready. An attempt to use the view before this notification 
causes a panic.
@param aDb The database containing the contact items.
@param aView The underlying contact view over which this view provides a filter.
@param aFilter The filter to use. For possible values, see CContactDatabase::TContactViewFilter.
@return The newly constructed filtered view object. */
	{
#ifdef CNTVIEW_API_PROFILING
	RDebug::Print(_L("[CNTMODEL] CContactFilteredView::NewL()\n"));
#endif
	CContactFilteredView* self=new(ELeave) CContactFilteredView(aDb,aFilter,aView);
	CleanupStack::PushL(self);
	self->ConstructL(aObserver);
	CleanupStack::Pop(self); 
	return self;
	}

EXPORT_C CContactFilteredView* CContactFilteredView::NewL(CContactViewBase& aView,const CContactDatabase& aDb,MContactViewObserver& aObserver,TInt aFilter)
/** Allocates and constructs a CContactFilteredView version 2 object. 

When adding/deleting contacts in the view, MContactViewObserver observer will receive 
TContactViewEvent events with iInt parameter set to index into the observed view of the added/deleted item

@param aObserver An observer that receives notifications when this view is 
ready for use and when changes take place in it. The observer receives a TContactViewEvent::EReady 
event when the view is ready. An attempt to use the view before this notification 
causes a panic.
@param aDb The database containing the contact items.
@param aView The underlying contact view over which this view provides a filter.
@param aFilter The filter to use. For possible values, see CContactDatabase::TContactViewFilter.
@return The newly constructed filtered view object. */
	{
#ifdef CNTVIEW_API_PROFILING
	RDebug::Print(_L("[CNTMODEL] CContactFilteredView::NewL()\n"));
#endif
	CContactFilteredView* self=new(ELeave) CContactFilteredView(aDb,aFilter,aView);
	CleanupStack::PushL(self);
	self->ConstructL(aObserver);
	self->iClassVersion = KClassVersion2;
	CleanupStack::Pop(self); 
	return self;
	}

/*
 * This is a reserved virtual exported function that is used for BC proofing 
 * against present and future additions of new exported virtual functions.
 @return Any return values of the helper methods called from this function or NULL.
 **/
 TAny* CContactFilteredView::CContactViewBase_Reserved_1(TFunction aFunction,TAny* aParams)
	{
	return CContactViewBase::CContactViewBase_Reserved_1(aFunction,aParams);
	}

TContactItemId CContactFilteredView::AtL(TInt aIndex) const
/** Gets the contact item ID at the specified index into the filtered view.

@param aIndex Index of the contact item ID into the filtered view. 
@leave KErrNotReady The view is not ready for use.
@leave KErrNotFound aIndex is outside the bounds of the filtered view.
@return The contact item ID. 
*/
	{
	if (iState != EReady)
		{
		User::Leave(KErrNotReady);
		}
	if (aIndex>=iFilteredIdArray.Count())
		{
		//Out of Bounds.
		User::Leave(KErrNotFound);
		}
	return iFilteredIdArray[aIndex].iId;
	}

const CViewContact& CContactFilteredView::ContactAtL(TInt aIndex) const
/** Gets the contact item at the specified index into the view.

@param aIndex Index into the view of the required item.
@leave KErrNotReady The view is not ready for use.
@leave KErrNotFound aIndex is outside the bounds of the filtered ID array.
@return The contact item. 
*/
	{
	if (iState != EReady)
		{
		User::Leave(KErrNotReady);
		}
	if (aIndex>=iFilteredIdArray.Count())
		{
		//Out of Bounds.
		User::Leave(KErrNotFound);
		}
	TContactIdWithMapping idWithMapping = (iFilteredIdArray)[aIndex];
	return iView.ContactAtL(idWithMapping.iMapping);
	}


TInt CContactFilteredView::CountL() const
/** Gets the number of contact item IDs in the filtered view.
@leave KErrNotReady The view is not ready for use. 
@return The number of contact items in the filtered view. 
*/
	{
	if (iState != EReady)
		{
		User::Leave(KErrNotReady);
		}
	return iFilteredIdArray.Count();
	}

TInt CContactFilteredView::FindL(TContactItemId aId) const
/** Finds the index into the filtered view of the specified contact item.

@param aId The contact item ID to search for. 
@leave KErrNotReady The view is not ready for use.
@return The index of the first matching item in the array or KErrNotFound if 
no matching object can be found. 
*/
	{
	if (iState != EReady)
		{
		User::Leave(KErrNotReady);
		}
	TContactIdWithMapping idWithDummyMapping;
	idWithDummyMapping.iId=aId;
	return iFilteredIdArray.Find(idWithDummyMapping,TIdentityRelation<TContactIdWithMapping>(IdsEqual));
	}

HBufC* CContactFilteredView::AllFieldsLC(TInt aIndex,const TDesC& aSeparator) const
/** Returns a descriptor containing the contents of all fields for an item in the 
view.

The fields are separated by aSeparator.

@param aIndex The index into the filtered view of the contact item.
@param aSeparator The string to use to separate the fields.
@return Pointer to the specified contact item descriptor. */
	{
	return iView.AllFieldsLC((iFilteredIdArray)[aIndex].iMapping,aSeparator);
	}

#ifdef _DEBUG
void CContactFilteredView::HandleContactViewEvent(const CContactViewBase& aView,const TContactViewEvent& aEvent)
#else
void CContactFilteredView::HandleContactViewEvent(const CContactViewBase& /*aView*/,const TContactViewEvent& aEvent)
#endif
	{
	ASSERT(&aView==&iView);
	TBool doUpdate(EFalse);
	TContactViewEvent event=aEvent;
	switch (event.iEventType)
		{
		case TContactViewEvent::ESortError:
		case TContactViewEvent::EServerError:
		case TContactViewEvent::EUnavailable:
			iState=ENotReady;
			break;
		case TContactViewEvent::EReady:
		case TContactViewEvent::ESortOrderChanged:
			{
			//Underlying view is ready, create filtered view.
			CreateFilteredIdArray();
			return; // Notification is handled in CreateFilteredIdArray
			}
		case TContactViewEvent::EItemAdded:
			{
			TBool viewModified(EFalse);
			TRAPD(err,UpdateFilteredIdArrayL(event,viewModified));
			if(err!=KErrNone && err!=KErrNotFound)
				{
				NotifyObservers(TContactViewEvent(TContactViewEvent::ESortError,err));
				return;
				}
			if (!viewModified)
				return; //If view wasn't modified, don't notify the observers
			}
			break;
		case TContactViewEvent::EItemRemoved:
			{
			TBool viewModified(EFalse);
			doUpdate = ETrue;
			TRAPD(err,UpdateFilteredIdArrayL(event,viewModified));
			if(err!=KErrNone && err!=KErrNotFound)
				{
				NotifyObservers(TContactViewEvent(TContactViewEvent::ESortError,err));
				return;
				}
			if (!viewModified)
				{
				UpdateMappingsL();
				return; //If view wasn't modified, don't notify the observers
				}
			}
			break;
		case TContactViewEvent::EGroupChanged:
			break;
		default:
			ASSERT(EFalse);
		}

	NotifyObservers(event);
	if(doUpdate) 
		{ 
		UpdateMappingsL(); //Update mappings for multiple changes and notify observers. 
		} 

	}

CContactFilteredView::~CContactFilteredView()
	{
	iFilteredIdArray.Close();
	iView.Close(*this);
	}

CContactFilteredView::CContactFilteredView(const CContactDatabase& aDb,TInt aFilter,CContactViewBase& aView)
	: CContactViewBase(aDb),iFilter(aFilter),iView(aView)
	{
	}
	
void CContactFilteredView::ConstructL(MContactViewObserver& aObserver)
	{
	CContactViewBase::ConstructL();
	OpenL(aObserver);
	iView.OpenL(*this);
	}

/* 
 * Construct the filtered view. 
 * This method is called when the underlying view sends an EReady or 
 * ESortOrderChanged event. It is implemented by asking the underlying
 * view which contacts match the filter (via CContactViewBase_Reserved_1)
 */
void CContactFilteredView::CreateFilteredIdArray()
	{
	iFilteredIdArray.Reset();

	CContactViewBase::TVirtualFunction2Params structOfParams(iFilter, iFilteredIdArray);
	TRAPD(err, iView.CContactViewBase_Reserved_1(CContactViewBase::ECContactViewBaseVirtualFunction2, &structOfParams)); //leaving method

	if(err==KErrNone) // View built OK, set to EReady or ESortOrderChanged
		{
		TState oldState = iState;
 		iState=EReady;
 
 		if (oldState == EInitializing)
			{
			NotifyObservers(TContactViewEvent(TContactViewEvent::EReady));
			}
 		else
 			{
 			NotifyObservers(TContactViewEvent(TContactViewEvent::ESortOrderChanged));
 			}
		}
	else // View building had problems
		{
		NotifyObservers(TContactViewEvent(TContactViewEvent::ESortError,err));
		}
	}

void CContactFilteredView::UpdateFilteredIdArrayL(TContactViewEvent& aEvent, TBool& aViewModified)
	{
	// Note, this method should update the index in aEvent - it will be passed on to observers of this view.
	switch(aEvent.iEventType)
		{
		case TContactViewEvent::EItemAdded:
			HandleAddEventL(aEvent, aViewModified);
			break;
		case TContactViewEvent::EItemRemoved:
			HandleRemoveEventL(aEvent, aViewModified);
			break;
		default:
			ASSERT(EFalse);
		};
	}

void CContactFilteredView::HandleAddEventL(TContactViewEvent& aEvent, TBool& aViewModified)
	{
	// The id contained in aEvent can refer to a inexistent contact by now 
	// so check that the contact with that id still exist first
	TInt underlyingViewIndex = iView.FindL(aEvent.iContactId);
	User::LeaveIfError(underlyingViewIndex);

	// get contact data from view rather than re-read from disk (speeds up access time)
	const CViewContact& contact = iView.ContactAtL(underlyingViewIndex);
	TContactIdWithMapping findMap;
	findMap.iId=contact.Id();
	findMap.iMapping=underlyingViewIndex;
	UpdateMappingsL();
	if(contact.ContactMatchesFilter(iFilter))
		{
		aViewModified=ETrue;
		//Contact Matches Filter, need to find the correct position to insert.
		aEvent.iInt=BinaryInsertL(findMap);
		}
	}

void CContactFilteredView::HandleRemoveEventL(TContactViewEvent& aEvent, TBool& aViewModified)
	{
	//Remove the contact from the filter array and alter the Event to local mapping
	TContactIdWithMapping findMap;
	findMap.iId = aEvent.iContactId;
	findMap.iMapping=KErrNotFound;//not important
	const TInt position = iFilteredIdArray.Find(findMap,TIdentityRelation<TContactIdWithMapping>(IdsEqual));
	if (position!=KErrNotFound)
		{
		aViewModified=ETrue;
		iFilteredIdArray.Remove(position);
		}
	aEvent.iInt=position;
	}

/* Update index mappings used in the filtered view. */
void CContactFilteredView::UpdateMappingsL()
	{
	 // View mappings are now invalid Fix all mapped indexes
	TInt updatedViewIndex=KErrNotFound;
	for (TInt i=0; i < iFilteredIdArray.Count(); ++i)
		{
		TContactItemId idUpdate = iFilteredIdArray[i].iId;
		updatedViewIndex=iView.FindL(idUpdate);
		if(updatedViewIndex==KErrNotFound)
			{
			//local view has removed this contact, so we need to as well.
			iFilteredIdArray.Remove(i);
			NotifyObservers(TContactViewEvent(TContactViewEvent::EItemRemoved, i, idUpdate));
			i--;
			continue;
			}
		iFilteredIdArray[i].iMapping=updatedViewIndex;
		}
	}


TInt CContactFilteredView::BinaryInsertL(TContactIdWithMapping aId)
	{
	TInt min=0;
	TInt max=iFilteredIdArray.Count();
	TInt match = aId.iMapping;
	FOREVER
		{
		if (min==max)
			break; // min is the new position
		TInt index=(max-min)/2+min;
		TInt compare = (iFilteredIdArray)[index].iMapping;
		if (compare<=match)
			{
			if (min==index)
				min++;
			else
				min=index;
			}
		else if (compare>match)
			max=index;
		}
	TInt err = iFilteredIdArray.Insert(aId,min);
	User::LeaveIfError(err);
	return min;
	}

TContactViewPreferences CContactFilteredView::ContactViewPreferences()
/** Gets the underlying view's view preferences.

@return The view preferences. */
	{
	return iView.ContactViewPreferences();
	}

const RContactViewSortOrder& CContactFilteredView::SortOrderL() const
/** Gets the underlying view's sort order.

@return The sort order. */
	{
	return iView.SortOrderL();
	}


void CContactFilteredView::ContactsMatchingCriteriaL(const MDesCArray& aFindWords,RPointerArray<CViewContact>& aMatchedContacts)
/** Searches all contact items in the filtered view for fields that contain all 
of the search strings specified.

The search uses wildcard matching so that the search strings can occur anywhere 
in an item's fields. For a match to occur, all of the search strings must 
be found in the contact item.

@param aFindWords A descriptor array containing one or more search strings.
@param aMatchedContacts On return, an array of matching contact items. */
	{
	const TReal parentViewCount = static_cast<TReal>(iView.CountL());
	const TReal filterViewCount = static_cast<TReal>(iFilteredIdArray.Count());
	
	if (parentViewCount!=0 && (filterViewCount/parentViewCount) < KFilterThreshold)
		{
		CContactViewBase::ContactsMatchingCriteriaL(aFindWords, aMatchedContacts);
		}
	else
		{
		iView.ContactsMatchingCriteriaL(aFindWords, aMatchedContacts);
		FilterResultsArray(aMatchedContacts);
		} 
	}

void CContactFilteredView::ContactsMatchingPrefixL(const MDesCArray& aFindWords, RPointerArray<CViewContact>& aMatchedContacts)
/** Searches all contact items in the filtered view for fields that contain all 
of the search strings specified.

Unlike ContactsMatchingCriteriaL(), the search term can only occur at the 
beginning of a field.

@param aFindWords A descriptor array containing one or more search strings.
@param aMatchedContacts On return, an array of matching contact items. */
	{
	iView.ContactsMatchingPrefixL(aFindWords, aMatchedContacts);
	FilterResultsArray(aMatchedContacts);
	}

/* 
 * Filter the results from ContactsMatchingCriteria/ContactsMatchingPrefix
 * removing contacts which are not in the current filtered view
 */
void CContactFilteredView::FilterResultsArray(RPointerArray<CViewContact>& aMatchedContacts)
	{
	TInt counter = 0;
	CViewContact* contactPtr;
	TContactIdWithMapping contactIdWithMapping;
	TInt max = aMatchedContacts.Count();
	// for each returned CContactFilteredView, check to
	// make sure that it is a contact which exists in
	// our list.

	// our list						: iFilteredIdArray
	// list to export is 			: aMatchedContacts
	while (counter < max)
		{
		contactPtr = aMatchedContacts[counter];
		contactIdWithMapping.iId = contactPtr->Id();

		// if the contact does not exist in our filtered list, then
		if ( iFilteredIdArray.Find(contactIdWithMapping, CompareMappings) == KErrNotFound )
			{
			// remove it from our list
			aMatchedContacts.Remove(counter); // does not delete pointer
			delete contactPtr;
			// we've removed an item from the array
			// this means that the item above, will drop to fill the hole left by
			// the item we've deleted.
			// so there is no need to increment the counter, but we do need to
			// decrement the max number of contacts we're parsing.
			max--;
			}
		else
			{
			counter++;
			}
		} 
	}

//static
TBool CContactFilteredView::CompareMappings(const TContactIdWithMapping& aFirst,const TContactIdWithMapping& aSecond)
	{
	return (aFirst.iId == aSecond.iId);
	}
