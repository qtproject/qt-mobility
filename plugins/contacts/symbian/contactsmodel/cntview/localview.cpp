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


#include <phbksync.h>
#include "cntstd.h"
#include <cntviewbase.h>
#include <cntitem.h>
#include "cntviewprivate.h"
#include <cntviewsortplugin.h>
#include "persistencelayer.h"
#include "cviewiterator.h"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <cntviewsortpluginbase.h>
#endif
//uncomment to test
//#define __PROFILE_SORT__

//uncomment for commonly required debug printing
//#define __VERBOSE_DEBUG__

extern void DebugLogNotification(const TDesC& aMethod, const TContactDbObserverEvent &aEvent);

//
// CContactLocalView.
//

// CIdle Callback function's return values: 0 - finished, 1 - call again
// (CIdle is used to Insert & Sort contacts one at a time into the view)

const TInt	KSortFinished = 0;
const TInt	KSortCallAgain = 1;

// Tunable constants
// These seem to be useful values for memory allocations with RPointerArray
const TInt	KContactsArrayGranularity = 100;
const TInt	KUnsortedArrayGranularity = 16;
// Number of Contacts to process per invocation of the Sorter
// Local Views can't do much whilst sorting, but we want to allow other
// Active Objects in the thread to run.
const TInt	KNumberOfContactsPerChunk = 50;

CContactLocalView::CContactLocalView(const CContactDatabase& aDb,TContactViewPreferences aContactTypes, MLplPersistenceLayerFactory* aFactory) 
: CContactViewBase(aDb), 
iFactory(aFactory),
iContacts(KContactsArrayGranularity), 
iUnSortedContacts(KUnsortedArrayGranularity),
iViewPreferences(aContactTypes)
/**
@internalComponent
*/
	{
	}

EXPORT_C CContactLocalView::CContactLocalView(const CContactDatabase& aDb,TContactViewPreferences aContactTypes) 
: CContactViewBase(aDb), iContacts(KContactsArrayGranularity), iUnSortedContacts(KUnsortedArrayGranularity),
iViewPreferences(aContactTypes)
/** Protected C++ constructor.

Called by NewL().

@param aDb The underlying database that contains the contact items.
@param aContactTypes Specifies which types of contact items should be included 
in the view and the behaviour for items that do not have content in any of 
the fields specified in the sort order. */
	{
	}

EXPORT_C CContactLocalView::~CContactLocalView()
/** Destructor.

Deletes all resources owned by the object, and removes itself as the contact 
database observer. */
	{
#ifdef CONTACTS_API_PROFILING
	TContactsApiProfile::CntViewMethodLog(TContactsApiProfile::ECntVwClassLocalView, TContactsApiProfile::ECntViewDestructor);
#endif
	delete iTextDef;
	delete iAsyncSorter;
	iContacts.ResetAndDestroy();
	iUnSortedContacts.ResetAndDestroy();
	iOutstandingEvents.Close();
	iSortOrder.Close();

	if (&iDb != NULL)
		{
        const_cast<CContactDatabase&>(iDb).RemoveObserver(*this);
		}
	delete iViewIterator;
	}

EXPORT_C CContactLocalView* CContactLocalView::NewL(MContactViewObserver& aObserver,const CContactDatabase& aDb,
													const RContactViewSortOrder& aSortOrder,TContactViewPreferences aContactTypes)
/** Allocates and constructs the local view object.

The view is sorted according to the sort order and view preferences specified, 
using a low priority idle time active object. The specified view observer 
is notified when the view is sorted and ready for use.

@param aObserver An observer that receives notifications when this view is 
ready for use and when changes take place in it. The observer receives a TContactViewEvent::EReady 
event when the view is ready. Any attempt to use the view before this notification will Leave with KErrNotReady
@param aDb The underlying database that contains the contact items. The view 
observes the database, so that it handles change events sent from the database.
@param aSortOrder Specifies the fields to use to sort the items in the view.
@param aContactTypes Specifies which types of contact items should be included 
in the view and the behaviour for items that do not have content in any of 
the fields specified in the sort order.
@return The newly constructed local view object. */
	{
#ifdef CONTACTS_API_PROFILING
	TContactsApiProfile::CntViewMethodLog(TContactsApiProfile::ECntVwClassLocalView, TContactsApiProfile::ECntViewApiNewL, aSortOrder, aContactTypes);
#endif
	CContactLocalView* self=new(ELeave) CContactLocalView(aDb,aContactTypes);
	CleanupStack::PushL(self);
	self->ConstructL(aObserver,aSortOrder);
	CleanupStack::Pop(self); 
	return self;
	}

EXPORT_C void CContactLocalView::ConstructL(MContactViewObserver& aObserver,const RContactViewSortOrder& aSortOrder)
/** Protected second phase constructor.

The view is sorted according to the sort order and view preferences specified, 
using a low priority idle time active object. The specified view observer 
is notified when the view is sorted and ready for use.

Called by NewL().

@param aObserver An observer that receives notifications when this view is 
ready for use and when changes take place in it. The observer receives a TContactViewEvent::EReady 
event when the view is ready. Any attempt to use the view before this notification will Leave with KErrNotReady.
@param aSortOrder Specifies the fields to use to sort the items in the view. */
	{
	// call new ConstructL
	ConstructL(aObserver, aSortOrder, EFalse, KNullDesC8);
	}

EXPORT_C CContactLocalView* CContactLocalView::NewL(MContactViewObserver& aObserver,const CContactDatabase& aDb,
													const RContactViewSortOrder& aSortOrder,TContactViewPreferences aContactTypes,
													const TDesC8& aSortPluginName)
/** Allocates and constructs the local view object.

The view is sorted according to the sort order and view preferences specified, 
using a low priority idle time active object. The specified view observer 
is notified when the view is sorted and ready for use.

@param aObserver An observer that receives notifications when this view is 
ready for use and when changes take place in it. The observer receives a TContactViewEvent::EReady 
event when the view is ready. Any attempt to use the view before this notification will Leave with KErrNotReady
@param aDb The underlying database that contains the contact items. The view 
observes the database, so that it handles change events sent from the database.
@param aSortOrder Specifies the fields to use to sort the items in the view.
@param aContactTypes Specifies which types of contact items should be included 
in the view and the behaviour for items that do not have content in any of 
the fields specified in the sort order.
@param aSortPluginName Specifies a plug-in that will be used to compare view contacts
when the the view is sorted. This name is used by ECOM to select the plugin, and is matched
with the "default_data" of all ECOM plugins that support the required interface.
@return The newly constructed local view object. */
	{
#ifdef CONTACTS_API_PROFILING
	TContactsApiProfile::CntViewMethodLog(TContactsApiProfile::ECntVwClassLocalView, TContactsApiProfile::ECntViewApiNewL, aSortOrder, aContactTypes, aSortPluginName);
#endif
	CContactLocalView* self=new(ELeave) CContactLocalView(aDb,aContactTypes);
	CleanupStack::PushL(self);
	self->ConstructL(aObserver, aSortOrder, ETrue, aSortPluginName);
	CleanupStack::Pop(self); 
	return self;
	}

/** CContactLocalView contructor, used in the server
@internalTechnology 
 */
EXPORT_C CContactLocalView* CContactLocalView::NewL(MContactViewObserver& aObserver,const CContactDatabase& aDb,const RContactViewSortOrder& aSortOrder,TContactViewPreferences aContactTypes,
		MLplPersistenceLayerFactory* aFactory,const TDesC8& aSortPluginName)
	{
	CContactLocalView* self=new(ELeave) CContactLocalView(aDb,aContactTypes,aFactory);
	CleanupStack::PushL(self);
	self->ConstructL(aObserver, aSortOrder, ETrue, aSortPluginName);
	CleanupStack::Pop(self); 
	return self;
	}

void CContactLocalView::ConstructL(MContactViewObserver& aObserver,const RContactViewSortOrder& aSortOrder,
								   TBool aUseNamedPlugin, const TDesC8& aSortPluginName)
/** Protected second phase constructor.

The view is sorted according to the sort order and view preferences specified, 
using a low priority idle time active object. The specified view observer 
is notified when the view is sorted and ready for use.

Called by NewL().

@internalComponent
@param aObserver An observer that receives notifications when this view is 
ready for use and when changes take place in it. The observer receives a TContactViewEvent::EReady 
event when the view is ready. Any attempt to use the view before this notification will Leave with KErrNotReady.
@param aSortOrder Specifies the fields to use to sort the items in the view. 
@param aUseNamedPlugin A flag indicates whether the aSortPluginName parameter is valid.
@param aSortPluginName Specifies a plug-in that will be used to compare view contacts
when the the view is sorted. This name is used by ECOM to select the plugin, and is matched
with the "default_data" of all ECOM plugins that support the required interface.
*/
	{
	CContactViewBase::ConstructL();
	if(iFactory == NULL)
		{
		iFactory = const_cast<CContactDatabase&>(iDb).FactoryL();
		}
	iAsyncSorter = CIdleContactSorter::NewL(*this, *iFactory);

	OpenL(aObserver);
	if (aUseNamedPlugin)
		{
		// find and load Sort plug-in
		if (aSortPluginName.Length())
			{
			TUid sortPluginUid = FindSortPluginImplL (aSortPluginName);
			LoadViewSortPluginL(sortPluginUid, iViewPreferences);
			}
		}
	else
		{
		// find and load default Sort plug-in (if any)
		TUid sortPluginUid = FindDefaultViewSortPluginImplL();
		if (sortPluginUid != KNullUid)
			{
			LoadViewSortPluginL(sortPluginUid, iViewPreferences);
			}
		}
	// initialise for sort, and start if the database is ready
	InitialiseSortL(aSortOrder, EFalse);
	if (&iDb != NULL)
		{
        const_cast<CContactDatabase&>(iDb).AddObserverL(*this);
		}
	}


EXPORT_C const RContactViewSortOrder& CContactLocalView::SortOrder() const
/** Gets the sort order, as set during construction.

@return The sort order. */
	{
#ifdef CONTACTS_API_PROFILING
	TContactsApiProfile::CntViewMethodLog(TContactsApiProfile::ECntVwClassLocalView, TContactsApiProfile::ECntViewApiSortOrder);
#endif
	return iSortOrder;
	}

TContactItemId CContactLocalView::AtL(TInt aIndex) const
/** Returns the ID of the contact item at a specified index into the view.

@param aIndex An index into the view.
@leave KErrNotFound The index is out of bounds.
@return The ID of the contact item at the specified index.
@leave KErrNotReady The view is not ready for use.  */
	{
#ifdef CONTACTS_API_PROFILING
	TContactsApiProfile::CntViewMethodLog(TContactsApiProfile::ECntVwClassLocalView, TContactsApiProfile::ECntViewApiAtL, aIndex);
#endif
	const CViewContact& contact = ContactAtL(aIndex);
	return contact.Id();
	}

const CViewContact& CContactLocalView::ContactAtL(TInt aIndex) const
/** Returns the contact item at a specified index into the view.

@param aIndex An index into the view.
@leave KErrNotFound The index is out of bounds.
@leave KErrNotReady The view is not ready for use.
@return The contact item at the specified index. */
	{
#ifdef CONTACTS_API_PROFILING
	TContactsApiProfile::CntViewMethodLog(TContactsApiProfile::ECntVwClassLocalView, TContactsApiProfile::ECntViewApiContactAtL, aIndex);
#endif
	// state cannot be EInitializing or ENotReady
	if( iState != EReady )
		{
		User::Leave(KErrNotReady);
		}

	TInt offsetIndex=aIndex;
	const TInt unsortedCount=iUnSortedContacts.Count();
	const TInt sortedCount=iContacts.Count();
	if(offsetIndex >= (unsortedCount+sortedCount))
		{
		//Out of Bounds.
		User::Leave(KErrNotFound);
		}

	if(unsortedCount>0)
		{
		if(iViewPreferences & EUnSortedAtBeginning)
			{
			if(aIndex<unsortedCount)
				{
				//contact in unsorted array
				return *iUnSortedContacts[aIndex];
				}
			else
				{
				//contact in sorted array
				offsetIndex-=unsortedCount;
				}
			}
		else if ((iViewPreferences & EUnSortedAtEnd) && (aIndex>=sortedCount))
			{
			offsetIndex-=sortedCount;
			return *iUnSortedContacts[offsetIndex];
			}

		}
	return *iContacts[offsetIndex];
	}

TInt CContactLocalView::CountL() const
/** Gets the total number of contact items in the view.

@return The number of contact items in the view. This includes both sorted 
and unsorted items.
@leave KErrNotReady The view is not ready for use. */
	{
#ifdef CONTACTS_API_PROFILING
	TContactsApiProfile::CntViewMethodLog(TContactsApiProfile::ECntVwClassLocalView, TContactsApiProfile::ECntViewApiCountL);
#endif
	// state cannot be EInitializing or ENotReady
	if( iState != EReady )
		{
		User::Leave(KErrNotReady);
		}
	
	TInt count(iUnSortedContacts.Count());
	count+=iContacts.Count();
	return count;
	}

TInt CContactLocalView::FindL(TContactItemId aId) const
/** Searches for a contact item in the view with the specified ID.

@param aId The ID of the contact item to search for.
@return If found, the index into the view of the matching item. Otherwise, 
KErrNotFound.
@leave KErrNotReady The view is not ready for use.  */
	{
#ifdef CONTACTS_API_PROFILING
	TContactsApiProfile::CntViewMethodLog(TContactsApiProfile::ECntVwClassLocalView, TContactsApiProfile::ECntViewApiFindL, aId);
#endif
	// state cannot be EInitializing or ENotReady
	if( iState != EReady )
		{
		User::Leave(KErrNotReady);
		}

	TInt index=KErrNotFound;
	CViewContact* contact = CViewContact::NewLC(aId);
	const TInt unSortedCount=iUnSortedContacts.Count();
	// first look in unsorted contacts
	if(unSortedCount > 0)
		{
		// contact may be in the unsorted array
		index = iUnSortedContacts.Find(contact,TIdentityRelation<CViewContact>(IdsMatch));

		if ((index != KErrNotFound) && (iViewPreferences & EUnSortedAtEnd))
			{
			// account for sorted array size
			index = index + iContacts.Count();
			}
		}

	// if not found try sorted contacts
	if (index == KErrNotFound)
		{
		//contact may be in the sorted array
		index = iContacts.Find(contact,TIdentityRelation<CViewContact>(IdsMatch));

		if ((index != KErrNotFound) && (iViewPreferences & EUnSortedAtBeginning))
			{
			// account for unsorted array size
			index = index + unSortedCount;
			}
		}

	CleanupStack::PopAndDestroy(contact);
	return index;
	}

HBufC* CContactLocalView::AllFieldsLC(TInt aIndex,const TDesC& aSeparator) const
/** Gets a descriptor containing the contents of all fields specified in the view's 
sort order for an item in the view.

The field separator is used to separate the contents of each field. It is 
not appended to the last field.

@param aIndex The index of the contact item into the view.
@param aSeparator The string to use to separate the fields.
@return Pointer to the contact item descriptor. */
	{
#ifdef CONTACTS_API_PROFILING
	TContactsApiProfile::CntViewMethodLog(TContactsApiProfile::ECntVwClassLocalView, TContactsApiProfile::ECntViewApiAllFieldsLC, aIndex);
#endif

	if( iState != EReady )
		{
		User::Leave(KErrNotReady);
		}

	TInt offsetIndex=aIndex;
	const TInt unSortedCount=iUnSortedContacts.Count();
	if(unSortedCount>0)
		{
		if(iViewPreferences & EUnSortedAtBeginning)
			{
			if(aIndex<unSortedCount)
				{
				//contact in unsorted array
				return FieldsWithSeparatorLC(iUnSortedContacts,aIndex,aSeparator);
				}
			else
				{
				//contact in sorted array
				offsetIndex-=unSortedCount;
				}
			}
		else if(iViewPreferences & EUnSortedAtEnd)
			{
			const TInt sortedCount=iContacts.Count();
			if(aIndex>=sortedCount)
				{
				offsetIndex-=sortedCount;
				return FieldsWithSeparatorLC(iUnSortedContacts,offsetIndex,aSeparator);
				}
			}
		}
	return FieldsWithSeparatorLC(iContacts,offsetIndex,aSeparator);
	}

EXPORT_C void CContactLocalView::SortL(const RContactViewSortOrder& aSortOrder)
/** Sorts the view using the specified sort order, using a low priority idle time 
active object.

This function is called during view construction and on receipt of certain 
change events from the underlying database.

@param aSortOrder Specifies the fields to use to sort the items in the view. */
	{
#ifdef CONTACTS_API_PROFILING
	TContactsApiProfile::CntViewMethodLog(TContactsApiProfile::ECntVwClassLocalView, TContactsApiProfile::ECntViewApiSortL);
#endif
	// re-initialise sort, and try to start it
	InitialiseSortL(aSortOrder, ETrue);
	}


/*
 Start first sort of view, or restart after SortL() API has changed the order.
 */
void CContactLocalView::InitialiseSortL(const RContactViewSortOrder& aSortOrder, TBool aChangingSortOrder)
	{
	if (aChangingSortOrder)
		{
		if (&iDb != NULL)
			{
			if (!iDb.DatabaseReadyL())
				{
				User::Leave(KErrNotReady);
				}
			}
		}

	// copy new sort order
	TRAPD(sortStartError, iSortOrder.CopyL(aSortOrder));
	
	if (sortStartError)
		{
		// ensure Db Recover (close then open tables) cannot push view to EReady
		iExtension->iError = sortStartError;
		User::Leave(sortStartError);
		}

	// New sort order for Sort Plugin
	CViewContactSortPlugin* sortPluginImpl = SortPluginImpl();
	if (sortPluginImpl)
		{
		sortPluginImpl->SetSortOrderL(aSortOrder);
		}

	// View can Sort only if database is 'ready'.
	if (&iDb != NULL)
		{
		if (!iDb.DatabaseReadyL())
			{
			return;
			}
		}

	// database is ready for reading - so start the Sort
	SortL();
	}


/**
Safe resort of view after Recover, Backup/Restore, etc...

@internalComponent
@released
*/
void CContactLocalView::SafeResort()
	{
	TInt sortError(KErrNone);
	
	// Database tables are closed across backup or restore so we may need to
	// re-open view iterator here.
	if (iViewIterator == NULL)
		{
		const TContactViewPreferences viewPrefs = iAsyncSorter->SortViewPreferences();
		TRAP(sortError,
			if(iFactory == NULL)
				{
				iFactory = const_cast<CContactDatabase&>(iDb).FactoryL();
				}
			MLplViewIteratorManager& manager = iFactory->GetViewIteratorManagerL();
			iViewIterator = new (ELeave) CViewIterator(manager,*iTextDef,viewPrefs);
			) // TRAP
		}

	if (!sortError)
		{
		TRAP(sortError, SortL());
		}

	// notify any error
	if (sortError)
		{
		NotifySortError(sortError);
		}
	}


/**
@internalComponent
@released
*/
void CContactLocalView::SortL()
	{
	// Initialisation for each explicitly requested sort
	// Construct a text def to read out the required fields from the db.
	CContactTextDef* textDef=CContactTextDef::NewLC();
	TInt sortOrderCount=iSortOrder.Count();

	for (TInt sortIndex=0;sortIndex<sortOrderCount;sortIndex++)
		{
		textDef->AppendL(TContactTextDefItem(iSortOrder[sortIndex]));
		}
	CleanupStack::Pop(); // textDef.
	delete iTextDef;
	iTextDef=textDef;

	// NB ResetSortL() requires iTextDef to be initialised

	// initialisation for each pass (of the insert sort) through the db
	// (2 passes may be required if the SIM card starts locked and is then unlocked)
	// (such a 2nd pass is kicked off by CIdleContactSorter)
	ResetSortL();

	// Delete existing sort if present.
	iAsyncSorter->Stop();

	iContacts.ResetAndDestroy();
	iUnSortedContacts.ResetAndDestroy();

#ifdef __PROFILE_SORT__
	RDebug::Print(_L("[CNTMODEL] CntModel View, , %u, %u, Starting sort\n"), 
		static_cast<TUint>(RProcess().Id()), 
		static_cast<TUint>(RThread().Id()));

	// 3 timers: 1st for read/Append; 2nd for Sort, 3rd for Compare
	RDebug::ProfileReset(1,3);
	RDebug::ProfileStart(1);
#endif

	// reset sort error
	iExtension->iError = KErrNone;

	// Kick off idler.
	iAsyncSorter->Start();
	}

void CContactLocalView::ResetSortL()
/**
 * Setup for a fresh pass through the Contacts database table
 *
 * (Code was in SortL)
 */
	{
	delete iViewIterator;
	iViewIterator = NULL;
	}

EXPORT_C TInt CContactLocalView::InsertL(TContactItemId aId)
/** Inserts a contact item into the view, maintaining the view's sort order.

For the item to be inserted, it must exist in the underlying database, and 
it must be of the correct type according to the view preferences.

This function is called when a contact item or group is added to or changed 
in the underlying database.

@param aId The ID of a contact item that exists in the underlying database.
@return The index at which the item was inserted into the view, or KErrNotFound 
if the contact item was not found in the underlying database, or it already 
exists in the view. */
	{
	TInt index=KErrNotFound;
#if defined(__VERBOSE_DEBUG__)
	RDebug::Print(_L("[CNTMODEL] CContactLocalView{ViewPrefs = 0x%08X}::InsertL into view Contact Id %i\r\n"), 
		iViewPreferences, aId);
#endif
	TContactViewPreferences view = iViewPreferences;
	if(!iAsyncSorter->InsertViewPreferences(view))
		{
		return KErrNotFound;
		}
	if(iFactory == NULL)
		{
		iFactory = const_cast<CContactDatabase&>(iDb).FactoryL();
		}
	MLplViewIteratorManager& manager = iFactory->GetViewIteratorManagerL();
	CViewIterator* iter = new (ELeave) CViewIterator(manager,*iTextDef,view);
	CleanupStack::PushL(iter);
	CViewContact* contact = iter->ItemAtL(aId);
	CleanupStack::PopAndDestroy(iter);	
	if(contact != NULL && ContactCorrectType(contact->ContactTypeUid(),view))
		{
		CleanupStack::PushL(contact);
		if(IsContactSortable(*contact, iViewPreferences))
			{
			//Contact has normal fields and can be added to the standard sorted array				
#if defined(__VERBOSE_DEBUG__)
				RDebug::Print(_L("[CNTMODEL] > > > > > View Insert into RPointerArray [Count = %i]\r\n"), iContacts.Count());
#endif
				
			// Insert using Sort Plugin compare method, and get new index
			User::LeaveIfError(InsertContactInView(iContacts, contact, EFalse, &index));
			CleanupStack::Pop(contact);
			if (iViewPreferences & EUnSortedAtBeginning)
				{
				index += iUnSortedContacts.Count();
				}
			}
		else if (iViewPreferences & (EUnSortedAtBeginning | EUnSortedAtEnd))
			{
			// unsortable contacts go at the end or beginning
			// we want this to be stable (e.g. when ICC becomes unlocked)
			User::LeaveIfError(InsertContactInView(iUnSortedContacts, contact, ETrue, &index));
			CleanupStack::Pop(contact);
			// calc new index
			if (iViewPreferences & EUnSortedAtEnd)
				{
				index += iContacts.Count();
				}
			}
		else // EIgnoreUnSorted
			{
			CleanupStack::PopAndDestroy(contact);
			}
		}
	else if(contact)
		{
		delete contact;
		}

	return index;
	}

EXPORT_C TInt CContactLocalView::RemoveL(TContactItemId aId)
/** Removes a contact item from the view.

This function is called when a contact item or group is deleted from or changed 
in the underlying database.

@param aId The ID of the contact item to remove from the view.
@return The index of the removed item into the view's list of sorted or unsorted 
contact items, or KErrNotFound if the item was not found in the view. */
	{
	CViewContact* contact = CViewContact::NewLC(aId);
	TInt index=KErrNotFound;
	index=iContacts.Find(contact,TIdentityRelation<CViewContact>(IdsMatch));
	if (index!=KErrNotFound)
		{
		CViewContact* temp= iContacts[index];
		iContacts.Remove(index);
		delete temp;
		if (iViewPreferences & EUnSortedAtBeginning)
			{
			index+=iUnSortedContacts.Count();
			}
		}
	else
		{
		if(iUnSortedContacts.Count()>0)
			{
			index=iUnSortedContacts.Find(contact,TIdentityRelation<CViewContact>(IdsMatch));
			if (index!=KErrNotFound)
				{
				CViewContact* temp= iUnSortedContacts[index];
				iUnSortedContacts.Remove(index);
				delete temp;
				if (iViewPreferences & EUnSortedAtEnd)
					{	
					index+=iContacts.Count();
					}
				// NB - If EIgnoreUnsorted, then this clause would not be running,
				// as the contact would never be added to the view.
				}
			}
		}
	CleanupStack::PopAndDestroy(contact);
	return index;
	}

EXPORT_C void CContactLocalView::CContactLocalView_Reserved_1()
	{
	}

EXPORT_C void CContactLocalView::CContactLocalView_Reserved_2()
	{
	}

void CContactLocalView::HandleDatabaseEventL(TContactDbObserverEvent aEvent)
	{
	// handle Backup / Restore notifications before checking View State
	switch (aEvent.iType)
		{
		case EContactDbObserverEventBackupBeginning:
		case EContactDbObserverEventRestoreBeginning:
#if defined(__VERBOSE_DEBUG__)
			RDebug::Print(_L("[CNTMODEL] CContactLocalView{ViewPrefs = 0x%08X}::HandleDatabaseEventL -> Backup/Restore Beginning, state = %i\r\n"), 
				iViewPreferences, iState);
#endif
			if (iState == EReady)
				{
				SetState(ENotReady);
				}
			else
				{
				// stop sorting
				iAsyncSorter->Stop();
				}
			ResetSortL();
			return;

		case EContactDbObserverEventBackupRestoreCompleted:
#if defined(__VERBOSE_DEBUG__)
			RDebug::Print(_L("[CNTMODEL] CContactLocalView{ViewPrefs = 0x%08X}::HandleDatabaseEventL -> Backup/Restore Completed, state = %i, old sort error %i\r\n"), 
				iViewPreferences, iState, iExtension->iError);
#endif
			if (iState == ENotReady && iExtension->iError == KErrNone)
				{
				// view was ready before tables were closed
				SetState(EReady);
				}
			else // view was Initializing (sorting) before tables were closed
				{
				// re-read database and sort
				SafeResort();
				}
			return;

		default:
			// other events dealt with below
			break;
		}


	if (iState!=EReady)
		{
        // The tables have been closed so the the sort must be cancelled.
		if (aEvent.iType == EContactDbObserverEventTablesClosed)
			{
	    	iAsyncSorter->Stop();
			}

		if (iAsyncSorter->QueueViewEvents())
			{
       			
#if defined(__VERBOSE_DEBUG__)
			DebugLogNotification(_L("[CNTMODEL] . . . . . Queueing Database Event "), aEvent);
#endif
			iOutstandingEvents.AppendL(aEvent);
			// The view state is set to ENotReady when a recovery takes place, and also when the tables
			// are closed, so set ready here.
			if (iState==ENotReady && (aEvent.iType==EContactDbObserverEventRecover || aEvent.iType==EContactDbObserverEventTablesOpened))
				{
				SetState(EReady);
				}
			// view was Initializing (sorting) before recovery or compression started!	
			if (iState==EInitializing && (aEvent.iType==EContactDbObserverEventRecover || aEvent.iType==EContactDbObserverEventCompress))
				{
				// re-read database and sort
				SafeResort();
				}
			}		
			
			
#if defined(__VERBOSE_DEBUG__)
		else
			{
			DebugLogNotification(_L("[CNTMODEL] . . . . . Discarding Database Event "), aEvent);
			}
#endif
		}
	else
		{
		TContactViewEvent event;
		event.iInt = KErrNone;
		switch(aEvent.iType)
			{
			case EContactDbObserverEventGroupChanged:
				{
				//Groups are a special case the base view may not contain the group
				//but a sub view may be such a group and need to know its changed
				//Local views can contain groups so this case carries on to the next so no break;
				event.iEventType=TContactViewEvent::EGroupChanged;
				event.iContactId=aEvent.iContactId;
				NotifyObservers(event);
				}
			case EContactDbObserverEventContactChanged:
			case EContactDbObserverEventOwnCardChanged:
				{// Remove from old position, and notify.
				TRAPD(err,event.iInt=RemoveL(aEvent.iContactId));

                if (err == KErrNone && event.iInt != KErrNotFound)
                    {
				    event.iEventType=TContactViewEvent::EItemRemoved;
				    event.iContactId=aEvent.iContactId;
				    NotifyObservers(event);
					}
				
				// Insert at new position, and notify.
				event.iInt=InsertL(aEvent.iContactId);
                if (event.iInt != KErrNotFound)
					{
				    event.iEventType=TContactViewEvent::EItemAdded;
				    event.iContactId=aEvent.iContactId;
				    NotifyObservers(event);
                    }
				break;
				}
			case EContactDbObserverEventContactAdded:
			case EContactDbObserverEventGroupAdded:
#if defined(__VERBOSE_DEBUG__)
			DebugLogNotification(_L("[CNTMODEL] DatabaseEvent -> Contact/Group Added"), aEvent);
#endif
				event.iInt=InsertL(aEvent.iContactId);
				if (event.iInt != KErrNotFound)
					{
					event.iEventType=TContactViewEvent::EItemAdded;
					event.iContactId=aEvent.iContactId;
					NotifyObservers(event);
					}
				break;
			case EContactDbObserverEventContactDeleted:
				if(aEvent.iContactId == KNullContactId)// KNullContactId indicates a bulk delete 
					{
					SetState(EInitializing); // Use initializing state to avoid ESortOrderChanged event being sent to observers.
					SafeResort();
					}
				else
					{
					event.iInt=RemoveL(aEvent.iContactId);
					if (event.iInt != KErrNotFound)
						{
						event.iEventType=TContactViewEvent::EItemRemoved;
						event.iContactId=aEvent.iContactId;
						NotifyObservers(event);
						}
					}
				break;
			case EContactDbObserverEventGroupDeleted:
			case EContactDbObserverEventOwnCardDeleted:
				event.iInt=RemoveL(aEvent.iContactId);
				if (event.iInt != KErrNotFound)
					{
					event.iEventType=TContactViewEvent::EItemRemoved;
					event.iContactId=aEvent.iContactId;
					NotifyObservers(event);
					}
				break;
			case EContactDbObserverEventUnknownChanges:
			case EContactDbObserverEventCurrentDatabaseChanged:
				SetState(EInitializing); // Use initializing state to avoid ESortOrderChanged event being sent to observers.
				SafeResort();
				break;
			case EContactDbObserverEventSortOrderChanged: // event is not currently used
				SetState(ENotReady);
				SafeResort();
				break;
			case EContactDbObserverEventTablesClosed:
				if (iState == EReady)
					{
					SetState(ENotReady);
					}
				break;
			case EContactDbObserverEventTablesOpened:
				// re-read database and sort
				SafeResort();
				break;

			case EContactDbObserverEventNull:
			case EContactDbObserverEventUnused:
			case EContactDbObserverEventRecover:
			case EContactDbObserverEventCompress:
			case EContactDbObserverEventRollback:
			case EContactDbObserverEventTemplateChanged:
			case EContactDbObserverEventTemplateDeleted:
			case EContactDbObserverEventTemplateAdded:
			case EContactDbObserverEventCurrentItemDeleted:
			case EContactDbObserverEventCurrentItemChanged:				
			case EContactDbObserverEventPreferredTemplateChanged:
			case EContactDbObserverEventSpeedDialsChanged:
			case EContactDbObserverEventRestoreBadDatabase:
				break;

			// these events should not come here, but be dealt with at the top of HandleDatabaseEventL
			case EContactDbObserverEventBackupBeginning:
			case EContactDbObserverEventRestoreBeginning:
			case EContactDbObserverEventBackupRestoreCompleted:
				break;
				
			default:
				ASSERT(EFalse);
			}
		}
	}

TInt CContactLocalView::SortCallBack()
	{
	TInt ret=KErrNotFound;
	TRAPD(err, ret = DoReadIncrementL());

#if defined(__VERBOSE_DEBUG__)
	if (err)
		{
		RDebug::Print(_L("[CNTMODEL] CContactLocalView{ViewPrefs = 0x%08X} . . . DoReadIncrementL ERROR %i\r\n"),
			iViewPreferences, err);
		}
	else
		{
		RDebug::Print(_L("[CNTMODEL] CContactLocalView{ViewPrefs = 0x%08X} . . . DoReadIncrementL returned %i\r\n"),
			iViewPreferences, ret);
		}
#endif

	if(err!=KErrNone)
		{
		ret=err;
		}
	if (ret<0)
		{
		// There was an error, so notify observers and stop any further callbacks.
		NotifySortError(ret);
		return KSortFinished;
		}
	if (ret==0)
		{
		//Read Has Finished.
#ifdef __PROFILE_SORT__
		RDebug::ProfileEnd(1);

		RDebug::ProfileStart(2);
#endif

		// is there a View Sort ECOM plug-in present?
		CViewContactSortPlugin*	sortPluginImpl = SortPluginImpl();

		if (sortPluginImpl)
			{
			// prepare View Sort plug-in
			ret = sortPluginImpl->SortStart(CViewContactSortPlugin::ESortStartFull, iContacts.Count());

			if (ret < 0)
				{
				return ret;
				}
			}

		// customised array sort implementation
		TRAP(err, ContactsArraySortL());

		//Sort Has Finished.
		if (sortPluginImpl)
			{
			sortPluginImpl->SortCompleted();
			}


#ifdef __PROFILE_SORT__

		RDebug::ProfileEnd(2);
		TProfile profile[3];
		RDebug::ProfileResult(profile,1,3);

		RDebug::Print(_L("[CNTMODEL] CntModel View, , %u, %u, Finished sort total, %u us\n"), 
			static_cast<TUint>(RProcess().Id()), static_cast<TUint>(RThread().Id()),
			profile[1].iTime + profile[0].iTime);

		RDebug::Print(_L("[CNTMODEL] CntModel View, , , , Data Read time, %u us\n"), profile[0].iTime);
		RDebug::Print(_L("[CNTMODEL] CntModel View, , , , Data Sort time, %u us\n"), profile[1].iTime);
		RDebug::Print(_L("[CNTMODEL] CntModel View, , , , Compare time, %u us\n"), profile[2].iTime);

#endif

		// sort finished, change state, allow for 2nd pass for ICC entries
		TInt result = iAsyncSorter->SortComplete();

		if (iState != EInitializing)
			{
			//The view has just been re-sorted notifiy observers ESortOrderChanged
			iState = EReady;
			NotifyObservers(TContactViewEvent(TContactViewEvent::ESortOrderChanged));
			HandleOutstandingEvents();
			return result;
			}
		// Sorted for the first time, notifiy ready
		SetState(EReady);
		return result;
		}
	// There's more reading to be done, so request another callback.
	return KSortCallAgain;
	}

TInt CContactLocalView::DoReadIncrementL()
	{
#if defined(__VERBOSE_DEBUG__)
	RDebug::Print(_L("[CNTMODEL] CContactLocalView{ViewPrefs = 0x%08X}::DoReadIncrement()"), iViewPreferences);
#endif

	// what contacts are we adding to the View?
	const TContactViewPreferences viewPrefs = iAsyncSorter->SortViewPreferences();

	if(iViewIterator == NULL)
		{
		if(iFactory == NULL)
			{
			iFactory = const_cast<CContactDatabase&>(iDb).FactoryL();
			}
		MLplViewIteratorManager& manager = iFactory->GetViewIteratorManagerL();
		iViewIterator = new (ELeave) CViewIterator(manager,*iTextDef,viewPrefs);
		iViewIterator->GoFirstL();
		}
	TInt i(0);
	// process a chunk of contacts
	CViewContact* contact;
	for(;i<KNumberOfContactsPerChunk;++i)
		{
		contact = iViewIterator->NextItemL();
		if(contact == NULL)
			{
			break; // No more contacts so quick exit
			}
		else if(!ContactCorrectType(contact->ContactTypeUid(),viewPrefs))
			{
			delete contact;
			}
		else
			{
			CleanupStack::PushL(contact);
			if(IsContactSortable(*contact,iViewPreferences))
				{
				iContacts.AppendL(contact);
				CleanupStack::Pop(contact);
				}
			else if(iViewPreferences & (EUnSortedAtBeginning | EUnSortedAtEnd))
				{
				// unsortable contacts go at the end or beginning
				iUnSortedContacts.AppendL(contact);
				CleanupStack::Pop(contact);
				}
			else
				{
				CleanupStack::PopAndDestroy(contact);
				}
			}	
		}
	if(i== KNumberOfContactsPerChunk)
		{
		// Loop did not break so more contacts
		return ETrue;
		}
	else
		{
		// Loop break so no more contacts
		return EFalse;
		}
	}

void CContactLocalView::ContactsArraySortL()
	{

	// HeapSort (stolen from RPointerArrayBase)
	TInt ss = iContacts.Count();
	if (ss>1)
		{
		TInt sh = ss>>1;
		FOREVER
			{
			CViewContact* si;
			if (sh!=0)
				{
				// make heap
				--sh;
				si = iContacts[sh];
				}
			else
				{
				// sort heap
				--ss;
				si = iContacts[ss];
				iContacts[ss] = iContacts[0];
				if (ss==1)
					{
					iContacts[0] = si;
					break;
					}
				}

			// sift down
			TInt ii = sh;
			TInt jj = sh;
			FOREVER
				{
				jj = (jj+1)<<1;
				if (jj>=ss || CompareContactsAndIdsL(*iContacts[jj-1],*iContacts[jj])>0 )
					--jj;
				if (jj>=ss || CompareContactsAndIdsL(*iContacts[jj],*si)<=0 )
					break;
				iContacts[ii] = iContacts[jj];
				ii = jj;
				}
			iContacts[ii]=si;
			}
		}

	}


/**
@internalComponent
*/
void CContactLocalView::SetState(TState aState)
	{
	switch (iState)
		{
		case EInitializing:
		case ENotReady:
			ASSERT(aState==EReady);
			iState=EReady;
			NotifyObservers(TContactViewEvent(TContactViewEvent::EReady));
			HandleOutstandingEvents();
			break;
		case EReady:
			ASSERT(aState==ENotReady || aState==EInitializing);
			// ensure sort error is reset
			iExtension->iError = KErrNone;
			iState=aState;
			NotifyObservers(TContactViewEvent(TContactViewEvent::EUnavailable));
			break;
		default:
			ASSERT(EFalse);
		}
	}


void CContactLocalView::HandleOutstandingEventL()
	{
	TContactDbObserverEvent event = iOutstandingEvents[0];
	iOutstandingEvents.Remove(0);
	HandleDatabaseEventL(event);
	}

void CContactLocalView::HandleOutstandingEvents()
	{
	while (iOutstandingEvents.Count() > 0)
		{
		// loop through as many events as possible in the one Trap harness
		TRAP_IGNORE(HandleOutstandingEventL());
		// if HandleDatabaseEventL left we must remove the event
		}
	}

TContactViewPreferences CContactLocalView::ContactViewPreferences()
/** Gets the view preferences, as set during construction.

@return The view preferences. */
	{
	return iViewPreferences;
	}

const RContactViewSortOrder& CContactLocalView::SortOrderL() const
/** Gets the sort order, as set during construction.

This function cannot leave.

@return The sort order. */
	{
	return iSortOrder;
	}

/*
 * Notify observers that view construction failed.
 * The error is stored so that if another client tries to open the view
 * they will receive the same error.
 * @param aError Leave code from CIdleContactSorter::RunL
 */
void CContactLocalView::NotifySortError(TInt aError)
	{
	iExtension->iError = aError;
	NotifyObservers(TContactViewEvent(TContactViewEvent::ESortError, aError));
	}

/*
 * This is a reserved virtual exported function that is used for BC proofing 
 * against present and future additions of new exported virtual functions.
 @return Any return values of the helper methods called from this function or NULL.
*/
EXPORT_C TAny* CContactLocalView::CContactViewBase_Reserved_1(TFunction aFunction,TAny* aParams)
	{
	return CContactViewBase::CContactViewBase_Reserved_1(aFunction,aParams);
	}


/*
 * Factory constructor.
 * @since 7.0
 * @param aView Reference to CContactLocalView object
 */
CIdleContactSorter* CIdleContactSorter::NewL(CContactLocalView& aView, MLplPersistenceLayerFactory& aFactory)
	{
	CIdleContactSorter* self = new (ELeave) CIdleContactSorter(aView, aFactory);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/* Destructor */
CIdleContactSorter::~CIdleContactSorter()
	{
	Cancel();

	if (iPhbkSyncWatcher)
		{
		iPhbkSyncWatcher->RemovePhbkObserver(*this);
		ReleasePhbkSyncWatcher();
		}
	}

/* Cancel any active requests to the phonebook synchroniser */
void CIdleContactSorter::DoCancel()
	{
	// Nothing to do.
	}

/** 
 * Uses a simple state machine, initial iSortState is set by Start() to either
 * EInsertSortFinal or EWaitingForInitialICCReady
 *
 * Either Insert Sort all or part of the requested view.
 * (CIdle::RunL calls back to the Insert Sort code.)
 *    State
 *    EInsertContactsOnlyIccLocked		  insert Contacts only (in a mixed view)
 *        goes to EContactsReadyWaitICCUnlock
 *    EInsertSortFinal                    insert all Contacts & ICC entries, or
 *        goes to ESortDone               add ICC entries to mixed view
 *                                        (iSortView specifies which)
 * Or wait for Phonebook Synchroniser to either finish or fail
 *    (failure other than SIM Locked causes a Sort Error) 
 *    State
 *    EWaitingForInitialICCReady          the view has nothing in: Phonebook
 *        goes to EInsertSortFinal        Synchronised allows full view to be available;
 *        or EInsertContactsOnlyIccLocked SIM Locked allows a view without ICC entries to 
 *                                        accessible
 *    EContactsReadyWaitICCUnlock         SIM was previously found to be locked, if/when
 *        goes to EInsertSortFinal        Phonebook Synchroniser completes we can merge in
 *                                        requested ICC entries
 *
 * The check whether the phonebook synchroniser is in a cache-valid state:-
 *
 * This check is done by making a async request to be completed when the 
 * phbksync cache state has changed, checking the current cache state and 
 * if the cache is valid already cancelling the request.
 * (The cancelled request will complete, causing RunL to run again.)
 * If there was a phbksync error check the error code, if it is not due to the
 * SIM card being locked then Leave.
 */
void CIdleContactSorter::RunL()
	{
#if defined(__VERBOSE_DEBUG__)
	RDebug::Print(_L("[CNTMODEL] CIdleContactSorter{RequestedView = 0x%08X, SortView = 0x%08X}::RunL()\r\n"),
		iRequestedView, iSortView);
	TSorterState oldSortState = iSortState;		// for debug messages only
#endif

	User::LeaveIfError(iStatus.Int());

	// either sort or wait for ICC ready / phonebook synch state change
	switch (iSortState)
		{
		// states that are sorting all or part of view
	case EInsertSortFinal:				// full insert sort or Phonebook Synched so add ICC entries
	case EInsertContactsOnlyIccLocked:	// insert Contacts for now, then wait for SIM to be unlocked
		// do slice of full / Contacts only /ICC only insert sort
		if (iView.SortCallBack() == KSortCallAgain)
			{ // CAsyncOneShot::Call()
			Call();
			}
		break;

		// states that are waiting for a phonebook sync event
	case EWaitingForInitialICCReady:	// ICC entries in view, waiting for Phonebook Synch state change
		// ICC entries are included in view:
		if (iPhbkSyncWatcher->PhonebooksReady() > 0)
			{
			// ICC sync complete - can immediately sort everything
			ChangeSortState(EInsertSortFinal);
			}
		else if (iPhbkSyncWatcher->PhonebooksWaiting() > 0)
			{
			// SIM card is locked and, this is the first time we've seen this

			// Insert Contacts (if wanted) into View now
			// Afterwards we will wait again for SIM to unlock & Phonebook Synch to complete
			if(iRequestedView & EICCEntriesAndContacts)
				{
				// insert/sort view, but without the requested ICC entries
				iSortView = static_cast<TContactViewPreferences>(iSortView & ~EICCEntriesAndContacts);
				ChangeSortState(EInsertContactsOnlyIccLocked);
				}
			else 
				{
				// only ICC entries were wanted in the first place
				// so make the (empty) View Ready
				const_cast<CContactLocalView&>(iView).SetState(CContactLocalView::EReady);
				// now wait for SIM to unlock & Phonebook Synch to complete
				ChangeSortState(EContactsReadyWaitICCUnlock);
				}
			}
		else
			{
			// synchronisation finished with an error?
			User::LeaveIfError(iPhbkSyncWatcher->PhonebookSyncError());
			}

		// otherwise wait for a Phonebook Synch event
		break;

	case EContactsReadyWaitICCUnlock:	// when SIM is unlocked add ICC Entries to this view
		// ICC entries are included in view:
		if (iPhbkSyncWatcher->PhonebooksReady() > 0)
			{
			// ICC sync complete - can sort everything
			ChangeSortState(EInsertSortFinal);

			// add requested ICC entries into the sorted view
			iSortView = STATIC_CAST(TContactViewPreferences, (iSortView & ~EContactsOnly) | EICCEntriesOnly);
			const_cast<CContactLocalView&>(iView).SetState(CContactLocalView::ENotReady);

			// another pass through contacts database is needed
			// ResetSortL should not Leave (especially as it must have worked previously)
			const_cast<CContactLocalView&>(iView).ResetSortL();
			}	

		// otherwise wait for a Phonebook Synch event
		break;

	case ESortAllDone: // shouldn't have come back here
	default:
		Panic(ECntPanicViewSorterStateMachine);
		break;
		}

#if defined(__VERBOSE_DEBUG__)
	if (oldSortState != iSortState)
		{
		switch (iSortState)
			{
			case EInsertSortFinal:
				RDebug::Print(_L("[CNTMODEL] {RequestedView = 0x%08X} * * * new Sort State = EInsertSortFinal, SortView = 0x%08X\r\n"),
					iRequestedView, iSortView);
				break;
			case EInsertContactsOnlyIccLocked:
				RDebug::Print(_L("[CNTMODEL] {RequestedView = 0x%08X} * * * new Sort State = EInsertContactsOnlyIccLocked, SortView = 0x%08X\r\n"),
					iRequestedView, iSortView);
				break;
			case EWaitingForInitialICCReady:
				RDebug::Print(_L("[CNTMODEL] {RequestedView = 0x%08X} * * * new Sort State = EWaitingForInitialICCReady\r\n"),
					iRequestedView);
				break;
			case EContactsReadyWaitICCUnlock:
				RDebug::Print(_L("[CNTMODEL] {RequestedView = 0x%08X} * * * new Sort State = EContactsReadyWaitICCUnlock, SortView = 0x%08X\r\n"),
					iRequestedView, iSortView);
				break;
			case ESortAllDone:
				RDebug::Print(_L("[CNTMODEL] {RequestedView = 0x%08X} * * * new Sort State = ESortAllDone\r\n"), iRequestedView);
				break;
			}
		}
	RDebug::Print(_L("[CNTMODEL] [Unsorted Contacts = %i, Sorted Contacts = %i, IsActive = %i]\r\n"),
		const_cast<CContactLocalView&>(iView).iUnSortedContacts.Count(),
		const_cast<CContactLocalView&>(iView).iContacts.Count(),
		IsActive());
#endif

	}


void CIdleContactSorter::ChangeSortState(TSorterState aNewSortState)
	{
#if defined(__VERBOSE_DEBUG__)
	RDebug::Print(_L("[CNTMODEL] CIdleContactSorter{RequestedView = 0x%08X, SortView = 0x%08X}::ChangeSortState(%i)\r\n"),
		iRequestedView, iSortView, static_cast<TInt>(aNewSortState));
#endif
	// new state
	iSortState = aNewSortState;
	// make the active object to run, CAsyncOneShot::Call()
	Call();
	}


/* 
 * Handle any leave during CIdleContactSorter::RunL. 
 * The local view is informed of that the view construction failed.
 * It will broadcast an ESortError view event to all clients of this view.
 * 
 * @param aError Leave code from RunL
 */
TInt CIdleContactSorter::RunError(TInt aError)
	{
#if defined(__VERBOSE_DEBUG__)
	RDebug::Print(_L("[CNTMODEL] CIdleContactSorter{RequestedView = 0x%08X, SortView = 0x%08X}::RunError(error = %i)\r\n"),
		iRequestedView, iSortView, aError);
#endif
	if ((aError != KErrCancel) && (iSortState != ESortAllDone))
		{
		const_cast<CContactLocalView&>(iView).NotifySortError(aError);
		iSortState = ESortAllDone;
		}
	return KErrNone;
	}


TInt CIdleContactSorter::SortComplete()
/**
 * Sort or partial sort completed, decide if there is more to do:
 * EInsertSortFinal -> ESortAllDone
 * EInsertContactsOnlyIccLocked -> EContactsReadyWaitICCUnlock (wait for SIM to become unlocked)
 *
 * return KSortCallAgain if there is more work to do, KSortFinished otherwise
 */
	{
	if(iSortState == EInsertContactsOnlyIccLocked)
		{
		// we are now waiting for phbksync, so that we can add ICC entries
		iSortState = EContactsReadyWaitICCUnlock;
#if defined(__VERBOSE_DEBUG__)
		RDebug::Print(_L("[CNTMODEL] * * * * * SortComplete: New Sort State = EContactsReadyWaitICCUnlock\r\n"));
#endif
		// CIdleContactSorter has more to do
		return KSortCallAgain;
		}

	// CIdleContactSorter all done
	iSortState = ESortAllDone;
#if defined(__VERBOSE_DEBUG__)
	RDebug::Print(_L("[CNTMODEL] * * * * * SortComplete: New Sort State = ESortAllDone\r\n"));
#endif
	return KSortFinished;
	}

/*
 Initialise Idle Contact Sorter for a new sort

 Re-init iSortView - the view filter for the Insert Sort
 Decide the initial iSortState for RunL:
   Contacts only view -> EInsertSortFinal
   ICC entries included -> EWaitingForInitialICCReady
 */
 void CIdleContactSorter::Start()
	{
	// initially we will try to insert sort everything requested
	iSortView = iRequestedView;

	if (iPhbkSyncWatcher)
		{
		// ICC entries included in view, must wait for Phonebook Synch
		iSortState = EWaitingForInitialICCReady;
		}
	else
		{
		// Only Contacts wanted in view, we can Sort straight away
		iSortState = EInsertSortFinal;
		}

#if defined(__VERBOSE_DEBUG__)
	RDebug::Print(
		(iSortState == EInsertSortFinal) ? 
			_L("[CNTMODEL] CIdleContactSorter{RequestedView = 0x%08X}::Start() Sort State = EInsertSortFinal\r\n") :
			_L("[CNTMODEL] CIdleContactSorter{RequestedView = 0x%08X}::Start() Sort State = EWaitingForInitialICCReady\r\n"),
		iRequestedView);
#endif

	// set Active for the first time, CAsyncOneShot::Call()
	Call();
	}


/*
 Stop any sort that is already in progress
 */
void CIdleContactSorter::Stop()
	{
	if (iSortState != ESortAllDone)
 		{
		// stop sorting
		iSortState = ESortAllDone;
		Cancel();
 		}
	}


/* 
 * Second phase construction.
 * Copy the View's requested preferences, get link to ICC phonebook watcher
 */
void CIdleContactSorter::ConstructL()
	{
	iRequestedView = CONST_CAST(CContactLocalView&,iView).ContactViewPreferences();

	// is ICC phonebook sync expected?
	if (iRequestedView & (EICCEntriesOnly | EICCEntriesAndContacts))
		{
		iPhbkSyncWatcher = GetPhbkSyncWatcherL();

		// observe ICC sync events
		iPhbkSyncWatcher->AddPhbkObserverL(*this);
		}
	}

 /*
 Utility method to return a reference to the ICC synchroniser watcher.
 Always returns a pointer to the real phonebook synchroniser plugin.
 It is assumed that Phonebook synchronising server never creates the view,
 so the deadlock in not possible and the dummy plugin is not required.

 @internalTechnology
 @leave KErrNotSupported if contact synchroniser plug-in cannot be found
 @leave KErrNoMemory if not enough memory
 @return Pointer to CContactPhbkSyncWatcher instance.
 */
CContactPhbkSyncWatcher* CIdleContactSorter::GetPhbkSyncWatcherL()
	{
	if (!iPhbkSyncWatcher)
		{
		iPhbkSyncWatcher = CContactPhbkSyncWatcher::NewL(iFactory.GetContactSynchroniserL(KMaxTUint32)); //Always use the Contact Synchroniser
		}
	return iPhbkSyncWatcher;
	}


void CIdleContactSorter::ReleasePhbkSyncWatcher()
	{
	if (iPhbkSyncWatcher)
		{
		if (iPhbkSyncWatcher->ObserverCount() == 0)
			{
			delete iPhbkSyncWatcher;
			iPhbkSyncWatcher = NULL;
			}
		}
	}


/* Constructor */
  CIdleContactSorter::CIdleContactSorter(CContactLocalView& aView, MLplPersistenceLayerFactory& aFactory) 
  : CAsyncOneShot(CActive::EPriorityLow), iView(aView), iFactory(aFactory)
	{
	}

/* 
 * Determines whether view events should be queued.
 *
 * View events are only queued when the ICC has been synchronised. This prevents
 * duplicate contacts in an ICC view because add events are not queued until the 
 * SIM is fully synchronised. 
 * 
 * See LUD-5EBHZF "ICC contacts view broadcasts add item events after view is 
 * ready" for more detail.
 * 
 * @return ETrue, if view events should be queued. EFalse, otherwise
 */
TBool CIdleContactSorter::QueueViewEvents() const
	{
	// Initial wait for phonebook synch (i.e. waiting for ICC ready or locked) ?
	if(iSortState == EWaitingForInitialICCReady)
		{
		return EFalse;
		}
	return ETrue;
	}

TContactViewPreferences CIdleContactSorter::SortViewPreferences() const
/**
 * Current View Preferences for insert sort in to View
 *
 * May be a subset of the requested View.
 * If the SIM card is locked this will initially be a View without ICC entries.
 * If the SIM becomes unlocked a second pass then picks out ICC entries only.
 *
 */
	{
	return iSortView;
	}


TBool CIdleContactSorter::InsertViewPreferences(TContactViewPreferences &aInsertView) const
/**
 * Modifies View Preferences for inserting into View
 *
 * May be a subset of the requested View:
 * If a Mixed (Contacts & ICC view) is requested and the Phonebook Synch has NOT
 * completed then only Contacts entries are added to the view. When the PhoneBook Synch 
 * completes all ICC entries will at the same time.
 *
 */
	{
	TBool okayToInsert = ETrue;

	switch (iSortState)
		{
	case EInsertSortFinal:				// full insert sort or Phonebook Synched so add ICC entries
	case ESortAllDone:
		// view is finished or finishing, can Insert any contact
		break;

	case EInsertContactsOnlyIccLocked:	// insert Contacts for now, then wait for SIM to be unlocked
		// only Contacts can be inserted now, no ICC entries
		aInsertView = iSortView;
		break;

	case EWaitingForInitialICCReady:	// ICC entries in view, waiting for Phonebook Synch state change
		// Waiting for initial ICC Synch result, insert nothing
		okayToInsert = EFalse;
		break;

	case EContactsReadyWaitICCUnlock:	// when SIM is unlocked add ICC Entries to this view
		if (aInsertView & EICCEntriesOnly)
			{
			okayToInsert = EFalse;		// can't insert ICC entries yet
			}
		else
			{
			aInsertView = iSortView;	// only Insert Contacts
			}
		break;
		}

	return okayToInsert;
	}

TBool CContactLocalView::ContactCorrectType(TUid aType,TContactViewPreferences aTypeToInclude)
	{
	TBool correctType(EFalse);
	if (aType==KUidContactCard || aType==KUidContactOwnCard)
		{
		// Ignore Unsorted Contacts flags & White Space flag
		// catch non- contact views
		// Should be EContactsOnly, EContactAndGroups & EICCEntriesAndContacts
		if (0 == ((aTypeToInclude & ~(ESingleWhiteSpaceIsEmptyField | EIgnoreUnSorted | EUnSortedAtBeginning | EUnSortedAtEnd))
				& (EGroupsOnly | EICCEntriesOnly))) // Ignore 'UnSorted' flags, exclude Groups Only & ICC Only 
			{
			correctType = ETrue;
			}
		}
	else if (aType==KUidContactGroup)
		{
		if (aTypeToInclude & (EGroupsOnly | EContactAndGroups))
			{
			correctType = ETrue;
			}
		}
	else if (aType == KUidContactICCEntry)
		{
		if (aTypeToInclude & (EICCEntriesOnly | EICCEntriesAndContacts))
			{
			correctType = ETrue;
			}
		}
	return correctType;
	}


void CIdleContactSorter::ContactPhbkSyncEventHandler(TPhonebookState aPhbkState)
	{
#if defined(__VERBOSE_DEBUG__)
	RDebug::Print(_L("[CNTMODEL] CIdleContactSorter{RequestedView = 0x%08X, SortView = 0x%08X}::ContactPhbkSyncEventHandler\r\n"),
		iRequestedView, iSortView);
#endif
	switch (aPhbkState)
		{
		case EIccPhbkNotSynchronised:
			/* Initial state, or ICC card has 'gone away' (e.g. ICC is resetting). */
			// no action - may want to act on this in future
			break;

		case EIccPhbkSynchronised:	// ICC Phonebook has completed synchronisation.

		case EIccWaitForPhbkToBeReady:	// Sync failed due to ICC being locked or not ready.

		case EIccPhbkSyncError:		//	Sync with Phbk Server failed.

#if defined(__VERBOSE_DEBUG__)
			RDebug::Print(aPhbkState == EIccPhbkSynchronised ? _L("[CNTMODEL]     state = ICC phonebook synchronised)\r\n") :
				aPhbkState == EIccWaitForPhbkToBeReady ? _L("    state = ICC phonebook Locked)\r\n") :
				 _L("    state = ICC phonebook Sync Error)\r\n"));
#endif
			// in a sorting state where we care?
			if ((iSortState == EWaitingForInitialICCReady) || (iSortState == EContactsReadyWaitICCUnlock))
				{
				// let state machine in RunL() deal with the event
				if (!IsActive())
					{ // CAsyncOneShot::Call()
					Call();
					}
				}
			break;
		}
	}


