/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <cntviewsortplugin.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <cntviewsortpluginbase.h>
#endif

//#define CNTVIEW_API_PROFILING
// To see the diferences between class versions check the in source documentation of TContactViewEvent
const TUint KClassVersion1 = 1;
const TUint KClassVersion2 = 2;

/* Default C++ Constructor */
CContactFindView::CContactFindView(const CContactDatabase& aDb,CContactViewBase& aView, TSearchType aSearchType)
:CContactViewBase(aDb),iView(aView),iSearchType(aSearchType), iClassVersion(KClassVersion1)

	{}

CContactFindView::~CContactFindView() 
/** Destructor */
	{
	iView.Close(*this);
	DeleteFindContacts();
	DestroyFindWordsArray();
	}

/* Helper method to delete find results */
void CContactFindView::DeleteFindContacts()
	{
	iFindContacts.ResetAndDestroy();
	}

EXPORT_C CContactFindView* CContactFindView::NewL(const CContactDatabase& aDb,CContactViewBase& aView,MContactViewObserver& aObserver,MDesCArray* aFindWords)
/** Allocates and constructs a new CContactFindView version 1 object, using a default search  
type of CContactViewBase::EFullSearch.

When adding/deleting contacts in the view, MContactViewObserver observer will receive 
TContactViewEvent events with iInt parameter set to KErrNone.

@param aDb The database containing the contact items.
@param aView The underlying contact view.
@param aObserver An observer that receives notifications when this view is 
ready for use and when changes take place in it. The observer receives a TContactViewEvent::EReady 
event when the view is ready. An attempt to use the view before this notification 
causes a panic.
@param aFindWords The array of search terms. The view does not take ownership 
of this - it makes its own copy of it to prevent it from going out of scope.
@return The newly constructed find view object. */
	{
	return NewL(aDb,aView,aObserver,aFindWords,EFullSearch);
	}

EXPORT_C CContactFindView* CContactFindView::NewL(const CContactDatabase& aDb,CContactViewBase& aView,MContactViewObserver& aObserver,MDesCArray* aFindWords, TSearchType aSearchType)
/** Allocates and constructs a new CContactFindView version 1 object, specifying a search 
type.

When adding/deleting contacts in the view, MContactViewObserver observer will receive 
TContactViewEvent events with iInt parameter set to KErrNone.

@param aDb The database containing the contact items.
@param aView The underlying contact view.
@param aObserver An observer that receives notifications when this view is 
ready for use and when changes take place in it. The observer receives a TContactViewEvent::EReady 
event when the view is ready. An attempt to use the view before this notification 
causes a panic.
@param aFindWords The array of search terms. The view does not take ownership 
of this - it makes its own copy of it to prevent it from going out of scope.
@param aSearchType The search type. This controls whether a search term can 
occur anywhere in a contact item field, or just at the beginning of the field.
@return The newly constructed find view object. */
	{
#ifdef CNTVIEW_API_PROFILING
	RDebug::Print(_L("[CNTMODEL] CContactFindView::NewL()\n"));
#endif
	CContactFindView* self=new(ELeave) CContactFindView(aDb, aView, aSearchType);
	CleanupStack::PushL(self);
	self->ConstructL(aObserver,aFindWords);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CContactFindView* CContactFindView::NewL(CContactViewBase& aView,const CContactDatabase& aDb,MContactViewObserver& aObserver,MDesCArray* aFindWords)
/** Allocates and constructs a new CContactFindView version 2 object, using a default search 
type of CContactViewBase::EFullSearch.

When adding/deleting contacts in the view, MContactViewObserver observer will receive 
TContactViewEvent events with iInt parameter set to index into the observed view of the added/deleted item

@param aDb The database containing the contact items.
@param aView The underlying contact view.
@param aObserver An observer that receives notifications when this view is 
ready for use and when changes take place in it. The observer receives a TContactViewEvent::EReady 
event when the view is ready. An attempt to use the view before this notification 
causes a panic.
@param aFindWords The array of search terms. The view does not take ownership 
of this - it makes its own copy of it to prevent it from going out of scope.
@return The newly constructed find view object. */
	{
	return NewL(aView,aDb,aObserver,aFindWords,EFullSearch);
	}

EXPORT_C CContactFindView* CContactFindView::NewL(CContactViewBase& aView,const CContactDatabase& aDb,MContactViewObserver& aObserver,MDesCArray* aFindWords, TSearchType aSearchType)
/** Allocates and constructs a new CContactFindView version 2 object, specifying a search 
 type.
 
When adding/deleting contacts in the view, MContactViewObserver observer will receive 
TContactViewEvent events with iInt parameter set to index into the observed view of the added/deleted item

@param aDb The database containing the contact items.
@param aView The underlying contact view.
@param aObserver An observer that receives notifications when this view is 
ready for use and when changes take place in it. The observer receives a TContactViewEvent::EReady 
event when the view is ready. An attempt to use the view before this notification 
causes a panic.
@param aFindWords The array of search terms. The view does not take ownership 
of this - it makes its own copy of it to prevent it from going out of scope.
@param aSearchType The search type. This controls whether a search term can 
occur anywhere in a contact item field, or just at the beginning of the field.
@return The newly constructed find view object. */
	{
#ifdef CNTVIEW_API_PROFILING
	RDebug::Print(_L("[CNTMODEL] CContactFindView::NewL()\n"));
#endif
	CContactFindView* self=new(ELeave) CContactFindView(aDb, aView, aSearchType);
	CleanupStack::PushL(self);
	self->ConstructL(aObserver,aFindWords);
	self->iClassVersion = KClassVersion2;
	CleanupStack::Pop(self);
	return self;
	}

/* 
 * Second phase constructor 
 * @param aObserver view observer
 * @param aFindWords Array of words to find
 */
void CContactFindView::ConstructL(MContactViewObserver& aObserver,MDesCArray* aFindWords)
	{
	CContactViewBase::ConstructL();
	OpenL(aObserver);
	iView.OpenL(*this);
	CopyNewFindWordsL(aFindWords);
	}

/* Helper method for deleting iFindWords member */
void CContactFindView::DestroyFindWordsArray()
	{
	if(iFindWords)
		{
		iFindWords->Reset();
		delete iFindWords;
		iFindWords=NULL;
		}
	}

/* Duplicate array provided by client to prevent it going out of scope */
void CContactFindView::CopyNewFindWordsL(MDesCArray* aFindWords)
	{
	DestroyFindWordsArray();
	const TInt count = aFindWords->MdcaCount();
	CPtrCArray* findWordArray = new (ELeave) CPtrCArray(count);
	CleanupStack::PushL(findWordArray);
	findWordArray->CopyL(*aFindWords);
	CleanupStack::Pop(findWordArray);
	iFindWords=findWordArray;
	}

EXPORT_C void CContactFindView::UpdateFindViewL()
/** Populates the view.

This deletes any existing matching contacts list and searches the underlying 
view for items matching the search criteria. */
	{
	DeleteFindContacts();
	if (iSearchType==EFullSearch)
		iView.ContactsMatchingCriteriaL(*iFindWords,iFindContacts);
	else
		iView.ContactsMatchingPrefixL(*iFindWords,iFindContacts);
	}

/**
 * This is a reserved virtual exported function that is used for BC proofing 
 * against present and future additions of new exported virtual functions.
 @return Any return values of the helper methods called from this function or NULL.
 */
TAny* CContactFindView::CContactViewBase_Reserved_1(TFunction aFunction,TAny* aParams)
	{
	return CContactViewBase::CContactViewBase_Reserved_1(aFunction,aParams);
	}

/* Refines an existing find view selection based on new criteria */
void CContactFindView::TightenFindViewCriteriaL()
	{
	RPointerArray<CViewContact> updatedContacts;
	CleanupClosePushL(updatedContacts);
	const TInt viewCount = iFindContacts.Count();
	CViewContact* contact;
	TBool match = EFalse;
	for (TInt ii=0;ii<viewCount;++ii)
		{
		contact =(iFindContacts)[ii];
		match=MatchesCriteriaL(*contact,*iFindWords, iSearchType,iExtension);
		if (match)
			{
			TContactItemId id = contact->Id();
			CViewContact* matchContact = CViewContact::NewLC(id);
			User::LeaveIfError(updatedContacts.Append(matchContact));
			CleanupStack::Pop(matchContact);
			}
		}
	DeleteFindContacts();
	iFindContacts=updatedContacts;
	CleanupStack::Pop(&updatedContacts);
	}

EXPORT_C void CContactFindView::RefineFindViewL(MDesCArray* aFindWords)
/** Refines the search criteria and updates the find view.

This may involve either narrowing or replacing the search criteria.

This function can be used to narrow the existing search criteria, by specifying 
an array of search terms that consists of the existing search terms (as passed 
to the NewL()) with additional terms appended to it. In this case, the function 
searches the existing list of matching contact items only. This assumes that 
UpdateFindViewL() has previously been called.

It can alternatively be used to replace entirely the existing search criteria. 
In this case, the whole underlying view is searched again.

@param aFindWords The new search criteria. This can be a superset of the existing 
search criteria, i.e. the existing criteria with additional terms appended 
to it; otherwise it replaces the existing search criteria entirely. */
	{
	const TInt originalFindWords=iFindWords->MdcaCount();
	if(aFindWords->MdcaCount()>=originalFindWords)
		{
		TInt compare = KErrNotFound;
		TBool hit=EFalse;
		//Check that the new words are equal to a greater than the original find words.
		for (TInt ii=0;ii<originalFindWords;++ii)
			{
			TPtrC originalWord(iFindWords->MdcaPoint(ii));
			//RDebug::Print(_L("orig=== %S"),&originalWord);
			//RDebug::Print(_L("new=== %S"),&aFindWords->MdcaPoint(ii));
			compare = aFindWords->MdcaPoint(ii).CompareF(originalWord);
			if(compare>=0)
				{
				hit=ETrue;
				}
			else
				{
				//its something completely different we need to do a fresh search
				hit=EFalse;
				break;
				}
			}
		if(hit)
			{
			//Results is a subset of what we already have so just refine it.
			CopyNewFindWordsL(aFindWords);
			TightenFindViewCriteriaL();
			return;
			}
		}
	CopyNewFindWordsL(aFindWords);
	//Do a new find from scratch
	UpdateFindViewL();
	}

/* Handle events from the parent view */
void CContactFindView::HandleContactViewEvent(const CContactViewBase& aView,const TContactViewEvent& aEvent)
	{
	ASSERT(&aView==&iView);
	TContactViewEvent event=aEvent;

	TInt err = KErrNone;

	switch (event.iEventType)
		{
		case TContactViewEvent::EUnavailable:
		case TContactViewEvent::ESortError:
		case TContactViewEvent::EServerError:
			iState=ENotReady;
			break;
		case TContactViewEvent::ESortOrderChanged:
		case TContactViewEvent::EReady:
			{
			if(&iView==&aView)
				{
				TRAP(err,UpdateFindViewL());
				iState=EReady;
				}
			}
			break;
		case TContactViewEvent::EItemAdded:
			if(&iView==&aView)
				{
				TRAP(err,HandleAddEventL(event));
				}
			break;
		case TContactViewEvent::EItemRemoved:
			if(&iView==&aView)
				{
				TRAP(err,HandleRemoveEventL(event));
				}
			break;
		case TContactViewEvent::EGroupChanged:
			break;
		default:
			ASSERT(EFalse);
		}	
	
	if (err != KErrNone)
		{
		NotifyObservers(TContactViewEvent(TContactViewEvent::ESortError,err));
		}
	else
		{
		NotifyObservers(event);
		}
	}

/* Handles a EItemRemoved event from the underlying view */
void CContactFindView::HandleRemoveEventL(TContactViewEvent& aEvent)
	{
	aEvent.iInt = KErrNone;
	const TInt pos = FindL(aEvent.iContactId);
	if (pos != KErrNotFound)
		{
		delete iFindContacts[pos];
		iFindContacts.Remove(pos);
		if(iClassVersion == KClassVersion2) //update the iInt with correct value
			{
			aEvent.iInt = pos;	
			}
		}
	}

/* Handles a EItemAdded event from the underlying view */
void CContactFindView::HandleAddEventL(TContactViewEvent& aEvent)
	{
	// The id contained in aEvent can refer to a inexistent contact by now
	// The contact may have been deleted or changed index position,
	// so check that the contact with that id still exist first
	TInt underlyingViewIndex = iView.FindL(aEvent.iContactId);
	User::LeaveIfError(underlyingViewIndex);
	aEvent.iInt = KErrNone;
	// get contact data from view rather than re-read from disk (speeds up access time)
	const CViewContact& contact = iView.ContactAtL(underlyingViewIndex);
	const TBool match=MatchesCriteriaL(contact,*iFindWords, iSearchType,iExtension);
	if (match)
		{
		CViewContact* thisContact = CViewContact::NewL(contact);
		// Insert using Sort Plugin compare method
		const TInt insertError = InsertContact(thisContact);
		if(insertError!=KErrNone)
			{
			delete thisContact;
			User::Leave(insertError);
			}

		if(iClassVersion == KClassVersion2)  //update iInt with the correct value
			{
			TInt pos = FindL(aEvent.iContactId);
			if ( pos != KErrNotFound ) // contact is a member of our list
				{
				aEvent.iInt = pos;	//update the iInt according
				}		
			}
		}
	}

TContactItemId CContactFindView::AtL(TInt aIndex) const
/** Gets the contact item ID at the specified index into the find view.

@param aIndex Index of the contact item ID into the find view.
@leave KErrNotFound aIndex is outside the bounds of the array.
@return The contact item ID. */
	{
	if(aIndex>=iFindContacts.Count())
		{
		//Out of Bounds.
		User::Leave(KErrNotFound);
		}
	return (iFindContacts)[aIndex]->Id();
	}

const CViewContact& CContactFindView::ContactAtL(TInt aIndex) const
/** Gets the contact item at the specified index into the view.

@param aIndex Index into the view of the required item.
@leave KErrNotFound aIndex is outside the bounds of the array.
@return The contact item. */
	{
	if(aIndex>=iFindContacts.Count())
		{
		//Out of Bounds.
		User::Leave(KErrNotFound);
		}
	return *(iFindContacts)[aIndex];
	}

TInt CContactFindView::CountL() const
/** Gets the number of contact item IDs in the find view.

@return The number of contact items in the find view. */
	{
	return iFindContacts.Count();
	}

TInt CContactFindView::FindL(TContactItemId aId) const
/** Finds the index into the array of the specified contact item.

@param aId The contact item ID to search for. 
@leave KErrNotReady The view is not ready for use.
@return The index of the first matching item in the find view or KErrNotFound 
if no matching item can be found. 
*/
	{
	if(iState != EReady)
		{
		User::Leave(KErrNotReady);
		}
	TInt index=KErrNotFound;
	CViewContact* contact = CViewContact::NewLC(aId);
	index = iFindContacts.Find(contact,TIdentityRelation<CViewContact>(IdsMatch));
	CleanupStack::PopAndDestroy(contact);
	return index;
	}

HBufC* CContactFindView::AllFieldsLC(TInt aIndex,const TDesC& aSeparator) const
/** Returns a descriptor containing the contents of all fields in an item in the 
view.

The fields are separated by aSeparator.

@param aIndex The index into the view of the contact item.
@param aSeparator The string to use to separate the fields.
@return Pointer to the contact item descriptor. */
	{
	return FieldsWithSeparatorLC(iFindContacts,aIndex,aSeparator);
	}

TContactViewPreferences CContactFindView::ContactViewPreferences()
/** Gets the underlying view's view preferences.

@return The view preferences. */
	{
	return iView.ContactViewPreferences();
	}

const RContactViewSortOrder& CContactFindView::SortOrderL() const
/** Gets the underlying view's sort order.

@return The sort order. */
	{
	return iView.SortOrderL();
	}

/**
 * Do binary search of Contacts Array, and insert new contact.
 * Uses current Sort Plugin, if any.
 *
 * @internalComponent
 * @released
 */
TInt CContactFindView::InsertContact(const CViewContact* aNewContact)
	{
	TInt error = KErrNone;

	if (iFindContacts.Count() == 0)
		{ // easy
		error = iFindContacts.Append(aNewContact);
		}
	else
		{
		// To insert in correct order we use parent view & its Sort method (Plug-in).
		// Virtual Function 3 calls  CContactViewBase::InsertContactInView() method
		// in the parent view.
		TVirtualFunction3Params vFuncParams(iFindContacts,aNewContact);

		error = reinterpret_cast<TInt>(iView.CContactViewBase_Reserved_1(CContactViewBase::ECContactViewBaseVirtualFunction3, &vFuncParams));
		}

	return error;
	}

