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

#include <cntviewsortplugin.h>
#include "cntviewprivate.h"
#include "cviewcontactmanager.h"

#include <cntviewbase.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <cntviewsortpluginbase.h>
#endif

const TInt KContactsArrayGranularity = 100;
const TInt KUnsortedArrayGranularity = 16;

/**
@SYMPatchable
@publishedPartner
@released
 
Patchable constant for the number of CViewContacts to read into memory 
before merging them into a larger sorted list (via a heap sort).
When the view contacts are merged, their text fields are released from memory. 
The larger the constant the faster a View containing large numbers of items is created. 
However, a larger constant will use more RAM for large Views.
 
The constant can be changed at ROM build time using patchdata OBY keyword.
*/
IMPORT_C extern const TInt KNumberOfContactsToReadPerMerge;

/**
@internalComponent
@released

Number of Contacts to process per invocation of the Sorter
Local Views can't do much whilst sorting, but we want to allow other
Active Objects in the thread to run. Use to be 50 for the DBMS version, 75 here to match
performance (i.e. should occupy the server for the same amount of time).
*/    
const TInt KNumberOfContactsToReadPerChunk = 75;


/** 
Private constructor
*/
CViewContactManager::CViewContactManager(CContactLocalView& aLocalView, MLplViewIteratorManager& aLplViewMgr, TContactViewPreferences aViewPreferences, CViewContactSortPlugin* aSortPlugin)
: iLocalView(aLocalView),
  iViewSessionId(KPLViewSessionIdNull),   
  iViewContacts(NULL), 
  iUnsortedViewContacts(NULL), 
  iViewContactsBuffer(NULL), 
  iSortPluginImpl(aSortPlugin), 
  iViewPreferences(aViewPreferences),
  iLplViewItemMgr(aLplViewMgr) 
    {
    }

    
/** 
Destructor
*/
CViewContactManager::~CViewContactManager()
    {
    if(iViewSessionId != KPLViewSessionIdNull)
        {
    	iLplViewItemMgr.CloseView(iViewSessionId);
        }
	
    delete iIdleSorter;
    
    if(iViewContacts)
    	{
    	iViewContacts->ResetAndDestroy();
		delete iViewContacts;	
    	}	
	if (iUnsortedViewContacts)
		{
		iUnsortedViewContacts->ResetAndDestroy();
		delete iUnsortedViewContacts;	
		}	
	
	if(iViewContactsBuffer)
		{
		iViewContactsBuffer->ResetAndDestroy();
		delete iViewContactsBuffer;
		}
		
	//The class doesn't own iSortPluginImpl,so don't release it here.
    }

    
/** 
Static factory constructor. Uses two phase construction and 
leaves nothing on the CleanupStack.

@return A pointer to the newly created CViewContactManager object.
*/
CViewContactManager* CViewContactManager::NewL(CContactLocalView& aLocalView, MLplPersistenceLayerFactory& aFactory, const CContactTextDef& aTextDef, TContactViewPreferences aViewPreferences, CViewContactSortPlugin* aSortPlugin)
    {
    MLplViewIteratorManager& lplViewIteratorManager = aFactory.GetViewIteratorManagerL();
	CViewContactManager* self = new(ELeave) CViewContactManager(aLocalView, lplViewIteratorManager, aViewPreferences, aSortPlugin);
	CleanupStack::PushL(self);
	self->ConstructL(aFactory, aTextDef);
	CleanupStack::Pop(self);
	return self;
    }

    
/** 
Constructor
*/
void CViewContactManager::ConstructL(MLplPersistenceLayerFactory& aFactory, const CContactTextDef& aTextDef)
	{
	iViewContacts = new(ELeave) RPointerArray<CViewContact>(KContactsArrayGranularity);
	iUnsortedViewContacts = new(ELeave) RPointerArray<CViewContact>(KUnsortedArrayGranularity);
	
	// Collation method by index 0 stays the same (only changes at a reboot)
 	iCollationMethod = *Mem::CollationMethodByIndex(0);
 	iCollationMethod.iFlags |= TCollationMethod::EIgnoreNone;
	
	iViewSessionId = iLplViewItemMgr.OpenViewL(aTextDef, iViewPreferences);

    //Create idle thread to prepare sort asynchronisely		
	iIdleSorter = CIdleContactSorter::NewL(*this, aFactory, iViewPreferences);	
	}


/** 
Start sorting with stored sort order.
*/
void CViewContactManager::SortL()
	{
	iIdleSorter->Stop();	
	
	iLplViewItemMgr.BeginIterateL(iViewSessionId);
	ResetSort();
	
	iIdleSorter->Start();	
	}

/** 
Start sorting with given sort order/text definition.

@param aTextDef new text definition would be used for sorting.
*/
void CViewContactManager::SortL(const CContactTextDef& aTextDef)
	{
	//Stop current sorting.
	iIdleSorter->Stop();	
	
	iLplViewItemMgr.EndIterateL(iViewSessionId);
	iLplViewItemMgr.ChangeSortOrderL(iViewSessionId, aTextDef);
	
	//Restart iterating with changed sort order 
	iLplViewItemMgr.BeginIterateL(iViewSessionId);
	
	ResetSort();
	iIdleSorter->Start();	
	}

	
/** 
Start current sorting.
*/
void CViewContactManager::StopSortL()
	{
	//Stop current sorting.
	iIdleSorter->Stop();	
	
	//Call the lpl layer to stop iterating view contacts.
	iLplViewItemMgr.EndIterateL(iViewSessionId);
	}

	
/** 
Reset contact ids arrays for resort or initial sort, this is called 
when database complete restoring data or view is creating(first sort).
*/
void CViewContactManager::ResetSort()
    {
	ASSERT(iViewContacts);
    iViewContacts->ResetAndDestroy();
	
	ASSERT(iUnsortedViewContacts);
    iUnsortedViewContacts->ResetAndDestroy();
	
	if(iViewContactsBuffer)
		{
		iViewContactsBuffer->ResetAndDestroy();
		delete iViewContactsBuffer;
		iViewContactsBuffer = NULL;
		}
    }


/** 
Implementation of MContactViewSortObserver interface, this is called with:
1. TIccViewNotify_IccOnlyLocked: The view is ICCEntryOnly view and ICC store is locked.
2. TIccViewNotify_IccUnlocked: The view include ICCEntry and ICC store is unlocked 
                               (resort is needed)

@param  aIccViewNotify flag to show if ICC store is locked or not.
@return ETrue if there is still view contact in database and needs idle
        sorting asynchroniser call back again.

@leave KErrNoMemory Out of memory.
@leave KErrNotFound The view session cannot be found.
*/
void  CViewContactManager::IccViewNotifyL(TInt aIccViewNotify)
    {
    switch(aIccViewNotify)
        {
        case TIccViewNotify_IccOnlyLocked:
            iLocalView.SetState(CContactLocalView::EReady);
            break;
        case TIccViewNotify_IccUnlocked:
            iLocalView.SetState(CContactLocalView::ENotReady);
        	iLplViewItemMgr.EndIterateL(iViewSessionId);
            break;
        }
    }


/** 
Implementation of MContactViewSortObserver interface, this is called by
the idle sorting asynchroniser repeatly. 

@param  aSortErr if there is error occurs in idle sorting asynchroniser.
@return ETrue if there is still view contact in database and needs idle
        sorting asynchroniser call back again.
*/
TBool CViewContactManager::IdleSorterNotifyL(TInt aSortErr)
	{
	if(aSortErr != KErrNone)
		{
		TInt err = KErrNone;
		TRAP(err, iLplViewItemMgr.EndIterateL(iViewSessionId));
		iLocalView.SortComplete(aSortErr);
		return EFalse;
		}
		
	if(!iViewContactsBuffer)
		{
		//Create a temporary buffer for reading view contact objects.
		iViewContactsBuffer = new(ELeave) RPointerArray<CViewContact>(KContactsArrayGranularity);
		}
		
	CViewContactManager::TReadState readState = ReadInViewContactsL(*iViewContactsBuffer);
    
    if(readState == EReadFullChunk)
		{
		// Full chunk of view contact items have been read, so request another read.
		return ETrue;
		}
		
	//Reach here only when readState is EReadFullForMerge or EReadCompleted
	if (iSortPluginImpl)
		{
		// prepare View Sort plug-in
		User::LeaveIfError(iSortPluginImpl->SortStart(CViewContactSortPlugin::ESortStartFull, iViewContactsBuffer->Count()));
		}
	
	// sort the view contacts in buffer
	HeapSortL(*iViewContactsBuffer);

	if(iViewContacts->Count() > 0)		
		{
		//There are sorted view objects stored in iViewContacts, merging is needed.
		RPointerArray<CViewContact>* mergedArray = MergeL(*iViewContacts, *iViewContactsBuffer);	
		
		// all view contact objects have been merged into iViewContacts, 
		// clean up the buffer array. 
		iViewContactsBuffer->Reset();
		
		iViewContacts->Close();
		delete iViewContacts;
		iViewContacts = mergedArray;
		}
	else
		{
		//It's first time we get a list of sorted view contat objects
		iViewContacts->Close();
		delete iViewContacts;
		iViewContacts = iViewContactsBuffer;	
		iViewContactsBuffer = NULL;
		}
		
	if (iSortPluginImpl)
		{
		iSortPluginImpl->SortCompleted();
		}
		
	if(readState == EReadCompleted)
		{
		//Sorting is complete.
		iLplViewItemMgr.EndIterateL(iViewSessionId);
		iLocalView.SortComplete(KErrNone);
		return EFalse;
		}
    
	return ETrue;
	}


/** 
Fill in the given view contacts buffer by asking underlying persistence layer,
and add all unsorted view contacts into unsorted array directly.

@param  aViewContacts CViewContact objects array to fill in and sort.
@return One of enum in TReadState, see the definition in TReadState for detail
*/
CViewContactManager::TReadState CViewContactManager::ReadInViewContactsL(RPointerArray<CViewContact>& aViewContacts)
    {
	TInt total = aViewContacts.Count();
	
	// process a chunk of contacts
	CViewContact* contact = NULL;
	
	//Here to use current sorting view preference in iIdleSorter who knows
	//if we can access ICCEntry store now, therefore decides if we should read in
	//ICCEntry contacts from the database.
	TContactViewPreferences sortViewPref = iIdleSorter->SortViewPreferences();
	
	for(TInt i = 0; i<KNumberOfContactsToReadPerChunk; ++i)
		{
		contact = iLplViewItemMgr.NextItemL(iViewSessionId, sortViewPref);
		if(contact == NULL)
			{
    		return EReadCompleted;
			}
			
		CleanupStack::PushL(contact);
		if(IsContactSortable(*contact,iViewPreferences))
			{
			aViewContacts.AppendL(contact);
			CleanupStack::Pop(contact);
		    if(++total >= KNumberOfContactsToReadPerMerge)
		        {
		        return EReadFullForMerge;
		        }
			}
		else if(iViewPreferences & (EUnSortedAtBeginning | EUnSortedAtEnd))
			{
			// It's an unsortable contact, just append its lightweight object
			// to unsortable view contacts list.
			contact->ChangeToLightweightObject();
			iUnsortedViewContacts->AppendL(contact);
			CleanupStack::Pop(contact);
			}
		else
			{
			CleanupStack::PopAndDestroy(contact);
			}
		}
		
	return EReadFullChunk;
    }


/** 
Check if the contact type uid matchs the view preferences.

@param  aContactTypeUid contact type uid to check.
@param  aTypeToInclude  view preferences have flags to define which contact the view should have.
@return ETrue if the uid match or reverse.
*/
TBool CViewContactManager::ContactCorrectType(TUid aContactTypeUid, TContactViewPreferences aTypeToInclude)
	{
	TBool correctType(EFalse);
	if (aContactTypeUid==KUidContactCard || aContactTypeUid==KUidContactOwnCard)
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
	else if (aContactTypeUid == KUidContactGroup)
		{
		if (aTypeToInclude & (EGroupsOnly | EContactAndGroups))
			{
			correctType = ETrue;
			}
		}
	else if (aContactTypeUid == KUidContactICCEntry)
		{
		if (aTypeToInclude & (EICCEntriesOnly | EICCEntriesAndContacts))
			{
			correctType = ETrue;
			}
		}
	return correctType;
	}


/** 
Heap sort the give view contacts array.

@param  aContacts the array of view contacts to be sorted.
@leave  leave errors from CContactViewBase::CompareContactsAndIdsL
*/
void CViewContactManager::HeapSortL(RPointerArray<CViewContact>& aContacts)
	{
	// HeapSort (stolen from RPointerArrayBase)
	TInt ss = aContacts.Count();
	if ( ss>1 )
		{
		TInt sh = ss>>1;
		FOREVER
			{
			CViewContact* si;
			if (sh != 0)
				{
				// make heap
				--sh;
				si = aContacts[sh];
				}
			else
				{
				// sort heap
				--ss;
				si = aContacts[ss];
				aContacts[ss] = aContacts[0];
				if (ss == 1)
					{
					aContacts[0] = si;
					break;
					}
				}

			// sift down
			TInt ii = sh;
			TInt jj = sh;
			FOREVER
				{
				jj = (jj+1)<<1;
				if ((jj >= ss) || (iLocalView.CompareContactsAndIdsL(*aContacts[jj-1],*aContacts[jj]) > 0))
					{
					--jj;
					}
					
				if ((jj >= ss) || (iLocalView.CompareContactsAndIdsL(*aContacts[jj],*si) <= 0))
					{
					break;
					}
					
				aContacts[ii] = aContacts[jj];
				ii = jj;
				} //FOREVER
				
			aContacts[ii] = si;
			} //FOREVER
		} //if (ss > 1)
	}


/** 
Merge two view contacts array into a result array, here we always assume the items in 
aRightContacts are fulfilled objects.

@param  aLeftContacts  the left array of view contacts to be merged.
@param  aRightContacts the right array of view contacts to be merged.
@return the final merged array.
*/
RPointerArray<CViewContact>* CViewContactManager::MergeL(RPointerArray<CViewContact>& aLeftContacts, RPointerArray<CViewContact>& aRightContacts)
    {
	TInt indexLeft(0);
	TInt indexRight(0);
	const TInt KCountLeft = aLeftContacts.Count();
	
	RPointerArray<CViewContact>* resultContacts = new(ELeave) RPointerArray<CViewContact>(KContactsArrayGranularity);	
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyRPointerArrayPointer, resultContacts));
	
    while(indexLeft < KCountLeft && indexRight < aRightContacts.Count())
		{
		CViewContact* firstLeftContact = static_cast<CViewContact*>(aLeftContacts[indexLeft]);
		CViewContact* firstRightContact = static_cast<CViewContact*>(aRightContacts[indexRight]);
		
		if(firstLeftContact->IsLightweightObject())
			{
			//The current contact in iViewContact is lightweight object, so we need to load the
			//fulfilled view contact to compare.
			CViewContact* contact = iLplViewItemMgr.ItemAtL(firstLeftContact->Id(), iViewSessionId);				
			if(contact == NULL)
				{
				//The already sorted contact is not in database!!!
				User::Leave(KErrNotFound);
				}
				
			CleanupStack::PushL(contact);    
			firstLeftContact->CopyL(*contact);
			CleanupStack::PopAndDestroy(contact);
			}

        TInt diff = CompareContactsAndIdsL(*firstLeftContact, *firstRightContact);
        
		if(diff > 0)
			{
			resultContacts->AppendL(firstRightContact);
			firstRightContact->ChangeToLightweightObject();
			++indexRight;
			}
		else
			{		
			resultContacts->AppendL(firstLeftContact);
			firstLeftContact->ChangeToLightweightObject();
			++indexLeft;
			if(diff == 0)
			    {
			    aRightContacts.Remove(indexRight);
			    delete firstRightContact;
			    }
			}
		}
		
	while(indexLeft < KCountLeft)
		{
		CViewContact* firstLeftContact = static_cast<CViewContact*>(aLeftContacts[indexLeft]);
		resultContacts->AppendL(firstLeftContact);
		++indexLeft;
		}
	
	while(indexRight < aRightContacts.Count())
		{
		CViewContact* firstRightContact = static_cast<CViewContact*>(aRightContacts[indexRight]);
		
		//change all the fullfil view objects to lightweight into the merged list.
		firstRightContact->ChangeToLightweightObject();
		
		resultContacts->AppendL(firstRightContact);
		++indexRight;
		}
		
    CleanupStack::Pop(resultContacts);
	
	return resultContacts;
    }


/** 
Insert a view contact object into existing sorted view contacts array(iViewContacts).

@param  aNewContact   the new view contact object to be inserted.
@param  aSortByIdOnly if the sorting method is sorting by id only
@param  aStart        start searching position in existing view contacts array.
@param  aIndex        the index of the view contact after insertion.
@leave  KErrNoMemory or leave error from CContactViewBase::CompareContactsAndIdsL
*/
TInt CViewContactManager::InsertViewContactL(const CViewContact* aNewContact, TBool aSortByIdOnly, TInt aStart)
	{
	TInt maxPos = iViewContacts->Count() - 1;
	TInt minPos = aStart;
	TInt position = 0;
    TInt error(KErrNone);
    
    //Using binary search to find the sorting position for the new contact.    
	if (maxPos >= 0)
		{
    	if (iSortPluginImpl)
    		{
    		// prepare View Sort plug-in
    		User::LeaveIfError(iSortPluginImpl->SortStart(CViewContactSortPlugin::ESortStartInsertOne, 1));
    		}

		position = (minPos + maxPos + 1) / 2;

		while (maxPos >= minPos)
			{
			TInt diff = 0;
            CViewContact* currContact = static_cast<CViewContact*>((*iViewContacts)[position]);
            
			if (aSortByIdOnly)
				{
				diff = aNewContact->Id() - currContact->Id();
				}
			else
				{
				//The view contacts stored in sortable contact list(iViewContacts) have been changed to
				//lightweight objects after sorting has done for memory saving purpose. 
				if(currContact->IsLightweightObject())
				    {
				    //The current contact in iViewContact is lightweight object, so we need to load the
				    //full content into the view contact before compare.
                    CViewContact* contact = iLplViewItemMgr.ItemAtL(currContact->Id(), iViewSessionId);				
					if(contact == NULL)
						{
						//the sorted contact is not in the database!!!
						User::Leave(KErrNotFound);
						}
                    
                    CleanupStack::PushL(contact);    
    				diff = CompareContactsAndIdsL(*aNewContact, *contact);
    				CleanupStack::PopAndDestroy(contact);
				    }
				else
				    {
				    //It's fulfilled object already, use it to compare directly
    				diff = CompareContactsAndIdsL(*aNewContact, *currContact);
				    }    
				}


			if (diff == 0)
				{
				// duplicate Id
				error = KErrAlreadyExists;
				break;
				}
				
			if (diff < 0)
				{
				maxPos = position - 1;
				}
			else // diff > 0
				{
				minPos = position + 1;
				}

			position = (minPos + maxPos + 1) / 2;
			} //while (maxPos >= minPos)
			
    	if (iSortPluginImpl)
    		{
    		iSortPluginImpl->SortCompleted();
    		}
		}

    User::LeaveIfError(error);
    
    //Make the view contact object to be lightweighted before insert to iViewContacts.
    const_cast<CViewContact*>(aNewContact)->ChangeToLightweightObject();
	iViewContacts->InsertL(aNewContact, position);

	return position;
	}

/** 
Get a reference of a view contact by given index to existing view contacts array,
if the the requested view object is in lightweight(only stored contact item id and
contact type) the function will fill the whole content to the object.

@param  aIndex  the index to the contact ids array.
@leave  KErrNoMemory or KErrNotFound
@return the reference to the requested contact view object(always be fulfiled), 
        the constrains of const is to keep to the same definition of the function 
		in local view which calls down to this function.
*/
const CViewContact& CViewContactManager::ContactAtL(TInt aIndex) const
    {
	CViewContact& viewContact = ViewContactAtL(aIndex);
	
	if(viewContact.IsLightweightObject())
		{
		CViewContact* contact = iLplViewItemMgr.ItemAtL(viewContact.Id(), iViewSessionId);
		if(contact == NULL)
			{
			User::Leave(KErrNotFound);
			}
		
		CleanupStack::PushL(contact);    
		viewContact.CopyL(*contact);
		CleanupStack::PopAndDestroy(contact);
		}
	
    return viewContact;    	
    }


/** 
Get the view contact object by given index to existing view contacts array.

@param  aIndex  the index to the contact ids array.
@param  aLoadFullContent  if the view contact need to load into full content.
@leave  KErrNotFound
@return the pointer to the requested contact view object but caller doesn't own the object
        aLoadFullContent is ETrue if the object was a lightweight object but 
		has been loaded into full content in the function.
*/
CViewContact& CViewContactManager::ViewContactAtL(TInt aIndex) const
    {
	const TInt unsortedCount = iUnsortedViewContacts->Count();
	const TInt sortedCount = iViewContacts->Count();
	
	if(aIndex >= (unsortedCount + sortedCount))
		{
		//Out of Bounds.
		User::Leave(KErrNotFound);
		}

	CViewContact* viewContact = NULL;
    
	if(iViewPreferences & EUnSortedAtBeginning)
		{
		if(aIndex < unsortedCount)
			{
			//contact in unsorted array
			viewContact = (*iUnsortedViewContacts)[aIndex];
			}
		else
			{
			//contact in sorted array
			viewContact = (*iViewContacts)[aIndex - unsortedCount];
			}
		}
	else if ((iViewPreferences & EUnSortedAtEnd) && (aIndex>=sortedCount))
		{
		viewContact = (*iUnsortedViewContacts)[aIndex - sortedCount];
		}
	else
	    {
	    viewContact = (*iViewContacts)[aIndex];
	    }    
	
	ASSERT(viewContact != NULL);	
	return *viewContact;
    }


/** 
Get the contact item id by given index.

@param  aIndex  the index to the contact ids array.
@leave  KErrNotFound
@return the contact item id.
*/
TContactItemId CViewContactManager::AtL(TInt aIndex) const
    {
    return ViewContactAtL(aIndex).Id();
    }


/** 
Get the number of contacts in the veiw.

@return the number of contacts in the veiw.
*/
TInt CViewContactManager::Count() const
    {
    ASSERT(iViewContacts);
    ASSERT(iUnsortedViewContacts);
    
	return iViewContacts->Count() + iUnsortedViewContacts->Count();
    }


/** 
Find if the given contact item id is in the view.

@param  aContactId The contact item id to be retrieved from database.
@return Index of the requested view contact or KErrNotFound.
*/
TInt CViewContactManager::FindL(TContactItemId aContactId) const
    {
	TInt index = KErrNotFound;
	CViewContact* contact = CViewContact::NewLC(aContactId);
	const TInt unSortedCount = iUnsortedViewContacts->Count();
	
	// first look in unsorted contacts
	if(unSortedCount > 0)
		{
		// contact may be in the unsorted array
		index = iUnsortedViewContacts->Find(contact,TIdentityRelation<CViewContact>(CContactViewBase::IdsMatch));

		if ((index != KErrNotFound) && (iViewPreferences & EUnSortedAtEnd))
			{
			// account for sorted array size
			index = index + iViewContacts->Count();
			}
		}

	// if not found try sorted contacts
	if (index == KErrNotFound)
		{
		//contact may be in the sorted array
		index = iViewContacts->Find(contact,TIdentityRelation<CViewContact>(CContactViewBase::IdsMatch));

		if ((index != KErrNotFound) && (iViewPreferences & EUnSortedAtBeginning))
			{
			// account for unsorted array size
			index = index + unSortedCount;
			}
		}

	CleanupStack::PopAndDestroy(contact);
	return index;
    }
    

/** 
Get all fields content of a view contact and separate them with charaters in aSeparators.

@param  aIndex     the index of view contact.
@param  aSeparator the charactors to split the fields content.
@return Buffer of the formated fields content.
*/
HBufC* CViewContactManager::AllFieldsLC(TInt aIndex, const TDesC& aSeparator) const
    {
	TBool ifWasFullObject = EFalse;
	
	CViewContact& viewContact = ViewContactAtL(aIndex); 
	if(viewContact.IsLightweightObject())
		{
		CViewContact* contact = iLplViewItemMgr.ItemAtL(viewContact.Id(), iViewSessionId);
		if(contact == NULL)
			{
			User::Leave(KErrNotFound);
			}
		
		CleanupStack::PushL(contact);    
		viewContact.CopyL(*contact);
		CleanupStack::PopAndDestroy(contact);
		
		ifWasFullObject = ETrue;
		}

	HBufC* buf = FieldsWithSeparatorLC(viewContact,aSeparator);
	
	if(ifWasFullObject)
		{
		//loadFullContent is set in ViewContactAtL if the viewContact was a lightweight object and
		//loaded to be full content object in that function. So we need to change it to lightweight
		//in order to save more memory.
		viewContact.ChangeToLightweightObject();
		}
    return buf;	
    }


/** 
Insert a contact into the view.

@param  aContactId       the contact item id of the contact to be inserted.
@param  aViewPreferences view preference which defines the view.
@return Index of inserted contact or KErrNotFound.
*/
TInt CViewContactManager::InsertL(const TContactItemId aContactId, TContactViewPreferences& aViewPreferences)
    {
	if(!iIdleSorter->InsertViewPreferences(aViewPreferences))
		{
		return KErrNotFound;
		}

	TInt index = KErrNotFound;
	
	CViewContact* contact = iLplViewItemMgr.ItemAtL(aContactId, iViewSessionId);
	if(contact == NULL)
		{
		//there is not a contact in database owning the contact id.
		return KErrNotFound;
		}
		
	CleanupStack::PushL(contact);
	
	if(ContactCorrectType(contact->ContactTypeUid(),aViewPreferences))
		{
		if(IsContactSortable(*contact, iViewPreferences))
			{
			//Contact has normal fields and can be added to the standard sorted array				
			//Insert using Sort Plugin compare method, and get new index
			index = InsertViewContactL(contact, EFalse, 0);
			if (iViewPreferences & EUnSortedAtBeginning)
				{
				index += iUnsortedViewContacts->Count();
				}
			}
		else if (iViewPreferences & (EUnSortedAtBeginning | EUnSortedAtEnd))
			{
			// unsortable contacts go at the end or beginning
			// we want this to be stable (e.g. when ICC becomes unlocked)
			contact->ChangeToLightweightObject();
			iUnsortedViewContacts->AppendL(contact);
            index = iUnsortedViewContacts->Count() - 1; 
            			
			// calc new index
			if (iViewPreferences & EUnSortedAtEnd)
				{
				index += iViewContacts->Count();
				}
			}
		}
		
    CleanupStack::Pop(contact);
    
    if(index == KErrNotFound)
        {
        //The contact is not inserted, so delete it.
        delete contact;
        }
        
	return index;
    }

    
/** 
Remove a contact from the view.

@param  aContactId the contact item id of the contact to be removed.
@return Index of removed contact or KErrNotFound.
*/
TInt CViewContactManager::RemoveL(const TContactItemId aContactId)
    {
	TInt index=KErrNotFound;
	const TInt unSortedCount=iUnsortedViewContacts->Count();
	
	CViewContact* contact = CViewContact::NewLC(aContactId);
	
	// first look in unsorted contacts
	if(unSortedCount > 0)
		{
		// contact may be in the unsorted array
		index = iUnsortedViewContacts->Find(contact,TIdentityRelation<CViewContact>(CContactViewBase::IdsMatch));

		if ((index != KErrNotFound) && (iViewPreferences & EUnSortedAtEnd))
			{
			CViewContact* viewContact = (*iUnsortedViewContacts)[index];
			iUnsortedViewContacts->Remove(index);
			delete viewContact;
			
			// account for sorted array size
			index = index + iViewContacts->Count();
			}
		}

	// if not found try sorted contacts
	if (index == KErrNotFound)
		{
		//contact may be in the sorted array
		index = iViewContacts->Find(contact,TIdentityRelation<CViewContact>(CContactViewBase::IdsMatch));

		if (index != KErrNotFound)
		    {
			CViewContact* viewContact = (*iViewContacts)[index];
			iViewContacts->Remove(index);
			delete viewContact;
			
		    if (iViewPreferences & EUnSortedAtBeginning)
    			{
    			// account for unsorted array size
    			index = index + unSortedCount;
    			}
		    }
		}
    
    CleanupStack::PopAndDestroy(contact);    		
	return index;
    }

	
/**
This a delegating method to call IsICCSynchronised in iIdleSorter which is
also observing ICC synchronization

@return EFalse when iIdleSorter is waiting for ICC cards to be synchronized.
*/
TBool CViewContactManager::IsICCSynchronised() const
	{
	return iIdleSorter->IsICCSynchronised();
	}
	
	
/**
Cleanup stack call up to handle RPointerArray.

@param aArray Pointer to an RPointerArray<CContactItemField> instance.

@return None.
*/
void CViewContactManager::ResetAndDestroyRPointerArrayPointer(TAny *aArray)
	{
	RPointerArray<CViewContact> *parray = static_cast< RPointerArray<CViewContact> * >(aArray);
	parray->Close();			
	delete parray;
	}
	
/** 
This function determines whether a contact should be added to the normal
sorted array (iContacts) or put into the alternative iUnSortedContacts array. 
Depending on the view preferences, these "unsorted" contacts are either 
ignored (deleted), or added to the beginning or end of the iContacts 
sorted list by At(), Find() etc... methods.

@param aContact the view contact object to be checked
@param aViewPreferences view preferences that determine if aContact is sortable.
@return ETrue if the view contact object is sortable.
*/
TBool CViewContactManager::IsContactSortable(const CViewContact& aContact, TContactViewPreferences& aViewPreferences) const
	{
	TBool sortableContact=EFalse;

	// only test contact if it matters
	if(aViewPreferences & (EIgnoreUnSorted | EUnSortedAtBeginning | EUnSortedAtEnd)) //Distinguish Unsorted
		{
		if (iSortPluginImpl)
			{
			// use plugin
			sortableContact = iSortPluginImpl->ViewContactIsSortable(aContact);
			}
		else
			{
			// ask contact itself
			sortableContact = aContact.IsSortable();
			}
		}
	else
		{
		// Sortable
		sortableContact = ETrue;
		}

	return sortableContact;
	}


/** 
Used for view sorting and insersion. In order to give a stable result 
if contact details match, it falls back to comparing contact IDs.
If a contact view sort plugin is loaded it uses its SortCompareViewContactsL() method.

@param aFirst the first view contact to be compared.
@param aSecond the second view contact to be compared.
@return 0 if two view contacts' id are equal.
*/
TInt CViewContactManager::CompareContactsAndIdsL(const CViewContact& aFirst, const CViewContact& aSecond) const
	{
	TInt result(0);

	if (iSortPluginImpl)
		{
		// use View Sort plugin
		result = iSortPluginImpl->SortCompareViewContactsL(aFirst,aSecond);
		}
	else
		{
		// no View sort plugin loaded
		result = TextCompareFieldsL(aFirst, aSecond);
		}

	if (result == 0)
		{
		result = aFirst.Id() - aSecond.Id();
		}

	return result;
	}

	
/** Collates two contact items' field contents.

This is done by comparing each contact item on a field by field basis starting 
with the loosest collation level initially, and then progressing to tighter 
collation levels only if the items are considered the same at the looser level. 
This is required so that items differing by case only are sorted correctly. 
If a field isn't present, then the comparison is done using the first field 
that is present.

Faster than static CompareFieldsL() method as it uses prefetched collation method.

@param aFirst The first contact item.
@param aSecond The second contact item.
@return A positive value indicates that aFirst's field contents are greater 
than aSecond's (so that aFirst would occur after aSecond in a sort order). 
A negative value indicates that aFirst's field contents are less than aSecond's 
(so that aFirst would occur before aSecond in a sort order). Zero indicates 
that aFirst and aSecond have identical field contents. */
TInt CViewContactManager::TextCompareFieldsL(const CViewContact& aFirst,const CViewContact& aSecond) const
	{
	const TInt KDefaultCollationLevel=3;
	TInt comparison = 0; // result of comparison, Zero = fields are identical
	TInt collationLevel = 0;

	do
		{
		comparison = CViewContactManager::CompareFieldsWithCollationLevel(aFirst, aSecond, collationLevel, &iCollationMethod);
		++collationLevel;
		}
		while (comparison == 0 && collationLevel <= KDefaultCollationLevel);

	return comparison;
	}
	

TInt CViewContactManager::CompareFieldsWithCollationLevel(const CViewContact& aFirst, const CViewContact& aSecond, TInt aCollationLevel, const TCollationMethod* aCollateMethod)
	{
	const TInt KLastField = aFirst.FieldCount() - 1;

	TInt retval = 0; // result of comparison, Zero = fields are identical
	TInt firstField(-1);
	TInt secondField(-1);
	TPtrC first;
	TPtrC second;

	for (TInt counter=0; !retval && (counter <= KLastField); ++counter)
		{
		// if the 1st populated field has a greater index than counter, 
		//	that means we'd get the same result from FindFirstPopulatedField.
		//	So, don't bother. Of course we always have to run it at least once.

		if (firstField < counter)
			{
			first.Set(aFirst.FindFirstPopulatedField(counter, firstField));
			}

		if (secondField < counter)
			{
			second.Set(aSecond.FindFirstPopulatedField(counter, secondField));
			}

		// no fields in either item
		if ((firstField < 0) && (secondField < 0))
			{
			break;
			}

		if (firstField < 0)
			{
			// first item sorts lower
			retval = -1;
			}
		else if (secondField < 0)
			{
			// second item sorts lower
			retval = 1;
			}
		else
			{
			// set counter to the first field populated by either contact
			while ((firstField > counter) && (secondField > counter))
				{
				++counter;
				}
			
			retval = first.CompareC(second, aCollationLevel, aCollateMethod);
			}
		}

	return retval;
	}
	

/** Allocates and returns a descriptor filled with the contents of all the fields 
in a contact item.

The fields are separated by the specified separator.

@param aContacts An array of contact items.
@param aIndex An index into the specified array.
@param aSeparator The text to use to separate the fields.
@return A pointer to a heap descriptor containing the contents of each of the 
contact item's fields. The field separator is appended to each field except 
the last one. The pointer is left on the cleanup stack. */
HBufC* CViewContactManager::FieldsWithSeparatorLC(const CViewContact& aViewContact,const TDesC& aSeparator) const
	{
	// Calculate the length of the buffer.
	TInt bufLength = 0;
	const TInt separatorLength = aSeparator.Length();
	const TInt numFields = aViewContact.FieldCount();
	
	for (TInt ii = 0; ii < numFields; ++ii)
		{
		bufLength += aViewContact.Field(ii).Length() + separatorLength;
		}
		
	HBufC* buf = HBufC::NewLC(bufLength);
	TPtr bufPtr(buf->Des());

	// Fill the buffer.
	for (TInt j = 0; j < numFields; ++j)
		{
		bufPtr.Append(aViewContact.Field(j));

		// Only put a separator in if this isn't the last field.
		if (j != numFields-1)
			{
			bufPtr.Append(aSeparator);
			}
		}
	return buf;
	}
