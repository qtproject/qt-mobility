/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <cntviewbase.h>
#include <cntitem.h>
#include "cntviewprivate.h"
#include <cntviewsortplugin.h>
#include "persistencelayer.h"
#include "cviewcontactmanager.h"
#include "cntstd.h"
#include <phbksync.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <cntviewsortpluginbase.h>
#endif
//uncomment to test
//#define __PROFILE_SORT__

//uncomment for commonly required debug printing
//#define __VERBOSE_DEBUG__

//Define the patchable constants for the number of CViewContacts to read into memory 
//before merging them into a larger sorted list (via a heap sort).This is actually used
//in cviewcontactmanager.cpp but must be defined here so that the compiler can not 
//perform any "constant folding" with the value   
EXPORT_C extern const TInt KNumberOfContactsToReadPerMerge = 500;

extern void DebugLogNotification(const TDesC& aMethod, const TContactDbObserverEvent &aEvent);


//
// CContactLocalView.
//
CContactLocalView::CContactLocalView(const CContactDatabase& aDb,TContactViewPreferences aContactTypes, MLplPersistenceLayerFactory* aFactory) 
: CContactViewBase(aDb), 
  iFactory(aFactory),
  iContacts(), 
  iUnSortedContacts(),
  iViewPreferences(aContactTypes)
	{
	}

	
/**
Protected C++ constructor.

Called by NewL().

@param aDb The underlying database that contains the contact items.
@param aContactTypes Specifies which types of contact items should be included 
in the view and the behaviour for items that do not have content in any of 
the fields specified in the sort order. 
*/
EXPORT_C CContactLocalView::CContactLocalView(const CContactDatabase& aDb,TContactViewPreferences aContactTypes) 
: CContactViewBase(aDb), 
  iContacts(), 
  iUnSortedContacts(),
  iViewPreferences(aContactTypes)
	{
	}

	
/**
Destructor.

Deletes all resources owned by the object, and removes itself as the contact 
database observer. 
*/
EXPORT_C CContactLocalView::~CContactLocalView()
	{
#ifdef CONTACTS_API_PROFILING
	TContactsApiProfile::CntViewMethodLog(TContactsApiProfile::ECntVwClassLocalView, TContactsApiProfile::ECntViewDestructor);
#endif
	delete iTextDef;
	iContacts.ResetAndDestroy();
	iUnSortedContacts.ResetAndDestroy();
	iOutstandingEvents.Close();
	iSortOrder.Close();

	if (&iDb != NULL)
		{
        const_cast<CContactDatabase&>(iDb).RemoveObserver(*this);
		}
		
	delete iAsyncSorter; //Add this though we don't use it in local view any more
	delete iViewCntMgr;
	}

	
/**
Allocates and constructs the local view object.

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
@return The newly constructed local view object. 
*/
EXPORT_C CContactLocalView* CContactLocalView::NewL(MContactViewObserver& aObserver,const CContactDatabase& aDb,
													const RContactViewSortOrder& aSortOrder,TContactViewPreferences aContactTypes)
	{
#ifdef CONTACTS_API_PROFILING
	TContactsApiProfile::CntViewMethodLog(TContactsApiProfile::ECntVwClassLocalView, TContactsApiProfile::ECntViewApiNewL, aSortOrder, aContactTypes);
#endif
	CContactLocalView* self = new(ELeave) CContactLocalView(aDb, aContactTypes);
	CleanupStack::PushL(self);
	self->ConstructL(aObserver, aSortOrder);
	CleanupStack::Pop(self); 
	return self;
	}

	
/**
Protected second phase constructor.

The view is sorted according to the sort order and view preferences specified, 
using a low priority idle time active object. The specified view observer 
is notified when the view is sorted and ready for use.

Called by NewL().

@param aObserver An observer that receives notifications when this view is 
ready for use and when changes take place in it. The observer receives a TContactViewEvent::EReady 
event when the view is ready. Any attempt to use the view before this notification will Leave with KErrNotReady.
@param aSortOrder Specifies the fields to use to sort the items in the view. 
*/
EXPORT_C void CContactLocalView::ConstructL(MContactViewObserver& aObserver, const RContactViewSortOrder& aSortOrder)
	{
	// call new ConstructL
	ConstructL(aObserver, aSortOrder, EFalse, KNullDesC8);
	}

	
/**
Allocates and constructs the local view object.

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
@return The newly constructed local view object. 
*/
EXPORT_C CContactLocalView* CContactLocalView::NewL(MContactViewObserver& aObserver,const CContactDatabase& aDb, const RContactViewSortOrder& aSortOrder, TContactViewPreferences aContactTypes, const TDesC8& aSortPluginName)
	{
#ifdef CONTACTS_API_PROFILING
	TContactsApiProfile::CntViewMethodLog(TContactsApiProfile::ECntVwClassLocalView, TContactsApiProfile::ECntViewApiNewL, aSortOrder, aContactTypes, aSortPluginName);
#endif
	CContactLocalView* self = new(ELeave) CContactLocalView(aDb, aContactTypes);
	CleanupStack::PushL(self);
	self->ConstructL(aObserver, aSortOrder, ETrue, aSortPluginName);
	CleanupStack::Pop(self); 
	return self;
	}

	
/**
CContactLocalView contructor, used in the server
@internalTechnology 
*/
EXPORT_C CContactLocalView* CContactLocalView::NewL(MContactViewObserver& aObserver,const CContactDatabase& aDb,const RContactViewSortOrder& aSortOrder,TContactViewPreferences aContactTypes, MLplPersistenceLayerFactory* aFactory, const TDesC8& aSortPluginName)
	{
	CContactLocalView* self = new (ELeave) CContactLocalView(aDb, aContactTypes, aFactory);
	CleanupStack::PushL(self);
	self->ConstructL(aObserver, aSortOrder, ETrue, aSortPluginName);
	CleanupStack::Pop(self); 
	return self;
	}

	
/**
Protected second phase constructor.

The view is sorted according to the sort order and view preferences specified, 
using a low priority idle time active object. The specified view observer 
is notified when the view is sorted and ready for use.

Called by NewL().

@param aObserver An observer that receives notifications when this view is 
ready for use and when changes take place in it. The observer receives a TContactViewEvent::EReady 
event when the view is ready. Any attempt to use the view before this notification will Leave with KErrNotReady.
@param aSortOrder Specifies the fields to use to sort the items in the view. 
@param aUseNamedPlugin A flag indicates whether the aSortPluginName parameter is valid.
@param aSortPluginName Specifies a plug-in that will be used to compare view contacts
when the the view is sorted. This name is used by ECOM to select the plugin, and is matched
with the "default_data" of all ECOM plugins that support the required interface.
*/
void CContactLocalView::ConstructL(MContactViewObserver& aObserver,const RContactViewSortOrder& aSortOrder, TBool aUseNamedPlugin, const TDesC8& aSortPluginName)
	{
	CContactViewBase::ConstructL();
	if(iFactory == NULL)
		{
		iFactory = const_cast<CContactDatabase&>(iDb).FactoryL();
		}
    
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
	
	//Initialise sort order and textdef.
	SetSortOrderL(aSortOrder);
	
	//Create view contact manager to handle sorting
    iViewCntMgr = CViewContactManager::NewL(*this, *iFactory, *iTextDef, iViewPreferences, SortPluginImpl());
	
	if (&iDb != NULL)
		{
        const_cast<CContactDatabase&>(iDb).AddObserverL(*this);
		}
	
	//Doing the sort.
	SortL();
	}


/**
Gets the sort order, as set during construction.

@return The sort order. 
*/
EXPORT_C const RContactViewSortOrder& CContactLocalView::SortOrder() const
	{
#ifdef CONTACTS_API_PROFILING
	TContactsApiProfile::CntViewMethodLog(TContactsApiProfile::ECntVwClassLocalView, TContactsApiProfile::ECntViewApiSortOrder);
#endif
	return iSortOrder;
	}

	
/**
Returns the ID of the contact item at a specified index into the view.

@param aIndex An index into the view.
@leave KErrNotFound The index is out of bounds.
@return The ID of the contact item at the specified index.
@leave KErrNotReady The view is not ready for use.  
*/
TContactItemId CContactLocalView::AtL(TInt aIndex) const
	{
#ifdef CONTACTS_API_PROFILING
	TContactsApiProfile::CntViewMethodLog(TContactsApiProfile::ECntVwClassLocalView, TContactsApiProfile::ECntViewApiAtL, aIndex);
#endif
	return iViewCntMgr->AtL(aIndex);
	}

	
/**
Returns the contact item at a specified index into the view.

@param aIndex An index into the view.
@leave KErrNotFound The index is out of bounds.
@leave KErrNotReady The view is not ready for use.
@return The reference to the copy of contact item at the specified index. 
*/
const CViewContact& CContactLocalView::ContactAtL(TInt aIndex) const
	{
#ifdef CONTACTS_API_PROFILING
	TContactsApiProfile::CntViewMethodLog(TContactsApiProfile::ECntVwClassLocalView, TContactsApiProfile::ECntViewApiContactAtL, aIndex);
#endif
	// state cannot be EInitializing or ENotReady
	if( iState != EReady )
		{
		User::Leave(KErrNotReady);
		}

	return iViewCntMgr->ContactAtL(aIndex);
	}

	
/**
Gets the total number of contact items in the view.

@return The number of contact items in the view. This includes both sorted 
and unsorted items.
@leave KErrNotReady The view is not ready for use. 
*/
TInt CContactLocalView::CountL() const
	{
#ifdef CONTACTS_API_PROFILING
	TContactsApiProfile::CntViewMethodLog(TContactsApiProfile::ECntVwClassLocalView, TContactsApiProfile::ECntViewApiCountL);
#endif
	// state cannot be EInitializing or ENotReady
	if( iState != EReady )
		{
		User::Leave(KErrNotReady);
		}
	
	return iViewCntMgr->Count();
	}

	
/**
Searches for a contact item in the view with the specified ID.

@param aId The ID of the contact item to search for.
@return If found, the index into the view of the matching item. Otherwise, 
KErrNotFound.
@leave KErrNotReady The view is not ready for use.  
*/
TInt CContactLocalView::FindL(TContactItemId aId) const
	{
#ifdef CONTACTS_API_PROFILING
	TContactsApiProfile::CntViewMethodLog(TContactsApiProfile::ECntVwClassLocalView, TContactsApiProfile::ECntViewApiFindL, aId);
#endif
	// state cannot be EInitializing or ENotReady
	if( iState != EReady )
		{
		User::Leave(KErrNotReady);
		}

	return iViewCntMgr->FindL(aId);
	}

	
/**
Gets a descriptor containing the contents of all fields specified in the view's 
sort order for an item in the view.

The field separator is used to separate the contents of each field. It is 
not appended to the last field.

@param aIndex The index of the contact item into the view.
@param aSeparator The string to use to separate the fields.
@return Pointer to the contact item descriptor. 
*/
HBufC* CContactLocalView::AllFieldsLC(TInt aIndex,const TDesC& aSeparator) const
	{
#ifdef CONTACTS_API_PROFILING
	TContactsApiProfile::CntViewMethodLog(TContactsApiProfile::ECntVwClassLocalView, TContactsApiProfile::ECntViewApiAllFieldsLC, aIndex);
#endif
	if( iState != EReady )
		{
		User::Leave(KErrNotReady);
		}

	return iViewCntMgr->AllFieldsLC(aIndex, aSeparator);
	}

	
/**
Sorts the view using the specified sort order, using a low priority idle time 
active object.

This function is called during view construction and on receipt of certain 
change events from the underlying database.

@param aSortOrder Specifies the fields to use to sort the items in the view. 
*/
EXPORT_C void CContactLocalView::SortL(const RContactViewSortOrder& aSortOrder)
	{
#ifdef CONTACTS_API_PROFILING
	TContactsApiProfile::CntViewMethodLog(TContactsApiProfile::ECntVwClassLocalView, TContactsApiProfile::ECntViewApiSortL);
#endif

	if (&iDb != NULL)
		{
		if (!iDb.DatabaseReadyL())
			{
			User::Leave(KErrNotReady);
			}
		}
	
	//set new sort order which also updates iTextDef.
	SetSortOrderL(aSortOrder);	
	
	// reset sort error
	iExtension->iError = KErrNone;
	iViewCntMgr->SortL(*iTextDef);
	}


/**
 Set sort order used to sorting the view contacts.
 */
void CContactLocalView::SetSortOrderL(const RContactViewSortOrder& aSortOrder)
	{
	// copy new sort order
	iSortOrder.CopyL(aSortOrder);
	
	// New sort order for Sort Plugin
	CViewContactSortPlugin* sortPluginImpl = SortPluginImpl();
	if (sortPluginImpl)
		{
		sortPluginImpl->SetSortOrderL(aSortOrder);
		}

	// Initialisation for each explicitly requested sort or initialise sort
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
	}


/**
Start sorting if database is ready
*/
void CContactLocalView::SortL()
	{
	// View can Sort only if database is 'ready'.
	if (&iDb != NULL)
		{
		if (!iDb.DatabaseReadyL())
			{
			return;
			}
		}

	// reset sort error
	iExtension->iError = KErrNone;
	iViewCntMgr->SortL();
	}


/**
Unleavable(safe) resorting
*/
void CContactLocalView::SafeResort()
	{
	TRAPD(sortError, SortL());

	// notify any error
	if (sortError)
		{
		NotifySortError(sortError);
		}
	}

	
/**
Inserts a contact item into the view, maintaining the view's sort order.

For the item to be inserted, it must exist in the underlying database, and 
it must be of the correct type according to the view preferences.

This function is called when a contact item or group is added to or changed 
in the underlying database.

@param aId The ID of a contact item that exists in the underlying database.
@return The index at which the item was inserted into the view, or KErrNotFound 
if the contact item was not found in the underlying database, or it already 
exists in the view. 
*/
EXPORT_C TInt CContactLocalView::InsertL(TContactItemId aId)
	{
#if defined(__VERBOSE_DEBUG__)
	RDebug::Print(_L("[CNTMODEL] CContactLocalView{ViewPrefs = 0x%08X}::InsertL into view Contact Id %i\r\n"), 
		iViewPreferences, aId);
#endif

	TContactViewPreferences view = iViewPreferences;
	return iViewCntMgr->InsertL(aId, view);
	}

	
/**
Removes a contact item from the view.

This function is called when a contact item or group is deleted from or changed 
in the underlying database.

@param aId The ID of the contact item to remove from the view.
@return The index of the removed item into the view's list of sorted or unsorted 
contact items, or KErrNotFound if the item was not found in the view. 
*/
EXPORT_C TInt CContactLocalView::RemoveL(TContactItemId aId)
	{
	return iViewCntMgr->RemoveL(aId);
	}

EXPORT_C void CContactLocalView::CContactLocalView_Reserved_1()
	{
	}

EXPORT_C void CContactLocalView::CContactLocalView_Reserved_2()
	{
	}


/**
Database event handler.

@param aEvent the database event.
*/
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
				iViewCntMgr->StopSortL();
				}
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
		if (iViewCntMgr->IsICCSynchronised())
			{
			/*
			 * View events are only queued when the ICC has been synchronised. This prevents
			 * duplicate contacts in an ICC view because add events are not queued until the 
			 * SIM is fully synchronised. 
			 * 
			 * See LUD-5EBHZF "ICC contacts view broadcasts add item events after view is 
			 * ready" for more detail.
       		 */
			
#if defined(__VERBOSE_DEBUG__)
			DebugLogNotification(_L("[CNTMODEL] . . . . . Queueing Database Event "), aEvent);
#endif
			iOutstandingEvents.AppendL(aEvent);
			
            // The view state is set to ENotReady when a recovery takes place, and also when the tables
            // are closed, so set ready here.
            if (iState == ENotReady && (aEvent.iType
                    == EContactDbObserverEventRecover || aEvent.iType
                    == EContactDbObserverEventTablesOpened))
                {
                SetState(EReady);
                }
            // view was Initializing (sorting) before recovery or compression started!  
            if (iState == EInitializing && (aEvent.iType
                    == EContactDbObserverEventRecover || aEvent.iType
                    == EContactDbObserverEventCompress))
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
			case EContactDbObserverEventContactChanged:
			case EContactDbObserverEventOwnCardChanged:
				{
				if (aEvent.iType == EContactDbObserverEventGroupChanged)
				    {
	    			//Groups are a special case the base view may not contain the group
    				//but a sub view may be such a group and need to know its changed
				    event.iEventType=TContactViewEvent::EGroupChanged;
				    event.iContactId=aEvent.iContactId;
				    NotifyObservers(event);
				    }
				
				// Remove from old position, and notify.
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
				if(aEvent.iContactId == KNullContactId) // KNullContactId indicates a bulk delete.
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

	
/**
Called from view contact item manager to notify the sorting has finished.

@param aSortErr error occurs in sorting or KErrNone if sorting completes without error.
*/
void CContactLocalView::SortComplete(TInt aSortErr)
	{
    if(aSortErr != KErrNone)
        {
		NotifySortError(aSortErr);
		return;
        }
        
	if (iState != EInitializing)
		{
		//The view has just been re-sorted, notifiy observers ESortOrderChanged
		iState = EReady;
		NotifyObservers(TContactViewEvent(TContactViewEvent::ESortOrderChanged));
		HandleOutstandingEvents();
		}
	else
		{
		// Sorted for the first time, notifiy ready
		SetState(EReady);
		}
	}

	
/**
Set database state

@param aState database state to set.
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


/**
Handle a particular queued event 
*/
void CContactLocalView::HandleOutstandingEventL()
	{
	TContactDbObserverEvent event = iOutstandingEvents[0];
	iOutstandingEvents.Remove(0);
	HandleDatabaseEventL(event);
	}

/**
Handle queued events when view is ready - sorting completes
*/
void CContactLocalView::HandleOutstandingEvents()
	{
	while (iOutstandingEvents.Count() > 0)
		{
		// loop through as many events as possible in the one Trap harness
		TRAP_IGNORE(HandleOutstandingEventL());
		// if HandleDatabaseEventL left we must remove the event
		}
	}

/**
Gets the view preferences, as set during construction.

@return The view preferences. 
*/
TContactViewPreferences CContactLocalView::ContactViewPreferences()
	{
	return iViewPreferences;
	}


/**
Gets the sort order, as set during construction.

This function cannot leave.

@return The sort order. 
*/
const RContactViewSortOrder& CContactLocalView::SortOrderL() const
	{
	return iSortOrder;
	}


/**
Notify observers that view construction failed.
The error is stored so that if another client tries to open the view
they will receive the same error.

@param aError Leave code from CIdleContactSorter::RunL
*/
void CContactLocalView::NotifySortError(TInt aError)
	{
	iExtension->iError = aError;
	NotifyObservers(TContactViewEvent(TContactViewEvent::ESortError, aError));
	}


/**
This is a reserved virtual exported function that is used for BC proofing 
against present and future additions of new exported virtual functions.

@return Any return values of the helper methods called from this function or NULL.
*/
EXPORT_C TAny* CContactLocalView::CContactViewBase_Reserved_1(TFunction aFunction,TAny* aParams)
	{
	return CContactViewBase::CContactViewBase_Reserved_1(aFunction,aParams);
	}

