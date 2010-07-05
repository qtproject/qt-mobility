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
#include <cntitem.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "cnthint.h"
#endif


//#define CNTVIEW_API_PROFILING

// To see the diferences between class versions check the in source documentation of TContactViewEvent
const TUint KClassVersion1 = 1;
const TUint KClassVersion2 = 2;
 

CContactGroupView::CContactGroupView(const CContactDatabase& aDb,CContactViewBase& aView,TGroupType aGroupType)
:CContactViewBase(aDb),iGroupId(KErrNotFound),iView(aView),iGroupType(aGroupType), iClassVersion(KClassVersion1)
	{}

CContactGroupView::~CContactGroupView() 
/** Destructor */
	{
	iView.Close(*this);
	iGroupContacts.Close();
	}

EXPORT_C CContactGroupView* CContactGroupView::NewL(const CContactDatabase& aDb,CContactViewBase& aView,MContactViewObserver& aObserver,const TContactItemId aGroupId,const TGroupType aGroupType)
/** Allocates and constructs a CContactGroupView version 1 object, identifying the group 
by its ID.

A group with the specified ID must exist in the database, otherwise when an 
attempt is made to update the view, a leave will occur with KErrNotFound. To 
create an unfiled view, the group id must be KNullContactId and the group type 
'EShowContactsNotInAnyGroup'. In this case the group id isn't used and no leave
will occur.

When adding contacts in the view, MContactViewObserver observer will receive 
TContactViewEvent events with iInt parameter set to KErrNone.
When deleting contacts in the view, MContactViewObserver observer will receive 
TContactViewEvent events with iInt parameter set to index into the observed view of the deleted item


@param aDb The database containing the contact group.
@param aView The underlying view.
@param aObserver An observer that receives notifications when this view is 
ready for use and when changes take place in it. The observer receives a TContactViewEvent::EReady 
event when the view is ready. An attempt to use the view before this notification 
causes a panic.
@param aGroupId The ID of the contact group.
@param aGroupType Controls whether the view displays items belonging to the 
group, items that do not belong to the group or items that do not belong to 
any group.
@return The newly constructed group view object. */
	{
#ifdef CNTVIEW_API_PROFILING
	RDebug::Print(_L("[CNTMODEL] CContactGroupView::NewL(Group Id %u)\n"), aGroupId);
#endif
	CContactGroupView* self=new(ELeave) CContactGroupView(aDb,aView,aGroupType);
	CleanupStack::PushL(self);
	self->ConstructL(aObserver,aGroupId);
	CleanupStack::Pop(self); 
	return self;
	}

EXPORT_C CContactGroupView* CContactGroupView::NewL(const CContactDatabase& aDb,CContactViewBase& aView,MContactViewObserver& aObserver,const TDesC& aGroupName,const TGroupType aGroupType)
/** Allocates and constructs a CContactGroupView version 1 object, identifying the group 
by its label.

A group with the specified label must exist in the database, otherwise when 
an attempt is made to update the view, a leave will occur with KErrNotFound. 

When adding contacts in the view, MContactViewObserver observer will receive 
TContactViewEvent events with iInt parameter set to KErrNone.
When deleting contacts in the view, MContactViewObserver observer will receive 
TContactViewEvent events with iInt parameter set to index into the observed view of the deleted item


@param aDb The database containing the contact group.
@param aView The underlying view.
@param aObserver An observer that receives notifications when this view is 
ready for use and when changes take place in it. The observer receives a TContactViewEvent::EReady 
event when the view is ready. An attempt to use the view before this notification 
causes a panic.
@param aGroupName The group label.
@param aGroupType Controls whether the view displays items belonging to the 
group, items that do not belong to the group or items that do not belong to 
any group.
@return The newly constructed group view object. */
	{
#ifdef CNTVIEW_API_PROFILING
	RDebug::Print(_L("[CNTMODEL] CContactGroupView::NewL(Group Name \"%s\")\n"), &aGroupName);
#endif
	CContactGroupView* self=new(ELeave) CContactGroupView(aDb,aView,aGroupType);
	CleanupStack::PushL(self);
	TContactItemId groupId = self->GetGroupIdbyNameL(aGroupName);
	self->ConstructL(aObserver,groupId);
	CleanupStack::Pop(self); 
	return self;
	}
  EXPORT_C CContactGroupView* CContactGroupView::NewL(CContactViewBase& aView,const CContactDatabase& aDb, MContactViewObserver& aObserver,const TContactItemId aGroupId,const TGroupType aGroupType)
/** Allocates and constructs a CContactGroupView version 2 object, identifying the group 
by its ID.

A group with the specified ID must exist in the database, otherwise when an 
attempt is made to update the view, a leave will occur with KErrNotFound. To 
create an unfiled view, the group id must be KNullContactId and the group type 
'EShowContactsNotInAnyGroup'. In this case the group id isn't used and no leave
will occur.

When adding/deleting contacts in the view, MContactViewObserver observer will receive 
TContactViewEvent events with iInt parameter set to index into the observed view of the added/deleted item

@param aDb The database containing the contact group.
@param aView The underlying view.
@param aObserver An observer that receives notifications when this view is 
ready for use and when changes take place in it. The observer receives a TContactViewEvent::EReady 
event when the view is ready. An attempt to use the view before this notification 
causes a panic.
@param aGroupId The ID of the contact group.
@param aGroupType Controls whether the view displays items belonging to the 
group, items that do not belong to the group or items that do not belong to 
any group.
@return The newly constructed group view object. */
	{
#ifdef CNTVIEW_API_PROFILING
	RDebug::Print(_L("[CNTMODEL] CContactGroupView::NewL(Group Id %u)\n"), aGroupId);
#endif
	CContactGroupView* self=new(ELeave) CContactGroupView(aDb,aView,aGroupType);
	CleanupStack::PushL(self);
	self->ConstructL(aObserver,aGroupId);
	self->iClassVersion = KClassVersion2;
	CleanupStack::Pop(self); 
	return self;
	}

EXPORT_C CContactGroupView* CContactGroupView::NewL(CContactViewBase& aView, const CContactDatabase& aDb, MContactViewObserver& aObserver,const TDesC& aGroupName,const TGroupType aGroupType)
/** Allocates and constructs a CContactGroupView version 2 object, identifying the group 
by its label. 

A group with the specified label must exist in the database, otherwise when 
an attempt is made to update the view, a leave will occur with KErrNotFound.

When adding/deleting contacts in the view, MContactViewObserver observer will receive 
TContactViewEvent events with iInt parameter set to index into the observed view of the added/deleted item
 
@param aDb The database containing the contact group.
@param aView The underlying view.
@param aObserver An observer that receives notifications when this view is 
ready for use and when changes take place in it. The observer receives a TContactViewEvent::EReady 
event when the view is ready. An attempt to use the view before this notification 
causes a panic.
@param aGroupName The group label.
@param aGroupType Controls whether the view displays items belonging to the 
group, items that do not belong to the group or items that do not belong to 
any group.
@return The newly constructed group view object. */
	{
#ifdef CNTVIEW_API_PROFILING
	RDebug::Print(_L("[CNTMODEL] CContactGroupView::NewL(Group Name \"%s\")\n"), &aGroupName);
#endif
	CContactGroupView* self=new(ELeave) CContactGroupView(aDb,aView,aGroupType);
	CleanupStack::PushL(self);
	TContactItemId groupId = self->GetGroupIdbyNameL(aGroupName);
	self->ConstructL(aObserver,groupId);
	self->iClassVersion = KClassVersion2;
	CleanupStack::Pop(self); 
	return self;
	}
	


/*
 * Second phase constructor 
 * @param aObserver view observer
 */
void CContactGroupView::ConstructL(MContactViewObserver& aObserver,const TContactItemId aGroupId)
	{
	CContactViewBase::ConstructL();
	OpenL(aObserver);
	iView.OpenL(*this);
	iGroupId=aGroupId;
	}	

// Update iGroupContacts based on group view options
EXPORT_C void CContactGroupView::UpdateGroupViewL()
/** Populates the group view.

Removes any existing items from the group view then searches the underlying 
view. The group view is repopulated with those items, maintaining the order 
in which they occur in the underlying view.

@leave KErrNotFound The group, identified by the label or ID specified during 
construction, does not exist in the database. */
	{
	switch(iGroupType)
		{
		case EShowContactsInGroup:
			UpdateForContactsInGroupL();
			break;
		case EShowContactsNotInGroup:
			iGroupContacts.Reset();
			UpdateForContactsNotInThisGroupL();
			break;
		case EShowContactsNotInAnyGroup:
			iGroupContacts.Reset();
			UpdateForUnfiledContactsL();
			break;
		default:
			ASSERT(EFalse);
		};
	}

/*
 * This is a reserved virtual exported function that is used for BC proofing 
 * against present and future additions of new exported virtual functions.
 **/
TAny* CContactGroupView::CContactViewBase_Reserved_1(TFunction aFunction,TAny* aParams)
	{
	return CContactViewBase::CContactViewBase_Reserved_1(aFunction,aParams);
	}

/* Identify all contacts in the view which are not members of the group */
void CContactGroupView::UpdateForContactsNotInThisGroupL()
	{
	CContactGroup* group = STATIC_CAST(CContactGroup*,MUTABLE_CAST(CContactDatabase&,iDb).ReadContactLC(iGroupId));
	const CContactIdArray* memberArray = group->ItemsContained();
	if (memberArray)
		{
		const TInt numContacts=iView.CountL();
		TContactIdWithMapping idMapping;
		TInt insertError=0;
		for (TInt ii=0;ii<numContacts;++ii)
			{
			const TContactItemId id = iView.AtL(ii);
			if(memberArray->Find(id)==KErrNotFound)
				{
				idMapping.iId=id;
				idMapping.iMapping=ii;

				TRAPD(err,insertError=iGroupContacts.InsertInOrder(idMapping,TLinearOrder<TContactIdWithMapping>(CompareMappingsL)));
				User::LeaveIfError(err);
				User::LeaveIfError(insertError);
				}
			}
		}
	CleanupStack::PopAndDestroy(group);
	}

/* Identify all members of the group in the view */
void CContactGroupView::UpdateForContactsInGroupL()
	{
	CContactGroup* group = STATIC_CAST(CContactGroup*,MUTABLE_CAST(CContactDatabase&,iDb).ReadContactLC(iGroupId));
	const CContactIdArray* memberArray = group->ItemsContained();
		
	if (memberArray && iClassVersion == KClassVersion2)
		{
		//Simulate members removed event. 
		//We have to do this because the item removed event which arrives after group change event would be filtered out.
		NotifyRemovedMembersL(memberArray);
		}
	
	iGroupContacts.Reset();
	if (memberArray)
		{
		UpdateForContactListL(memberArray);
		}
	CleanupStack::PopAndDestroy(group);
	}

/* 
 * Identify all "unfiled" contacts in the view. 
 * "Unfiled" contacts are all contacts which do not belong to any group
 */
void CContactGroupView::UpdateForUnfiledContactsL()
	{
	CContactIdArray* unfiled= MUTABLE_CAST(CContactDatabase&,iDb).UnfiledContactsL();
	CleanupStack::PushL(unfiled);
	UpdateForContactListL(unfiled);
	CleanupStack::PopAndDestroy(unfiled);
	}

/* Iterate through aArray and if they are in the view, add them to iGroupContacts */
void CContactGroupView::UpdateForContactListL(const CContactIdArray* aArray)
	{
	const TInt count = aArray->Count();
	TInt pos=KErrNotFound;
	TContactIdWithMapping idMapping;
	TInt insertError=KErrNone;

	for (TInt ii=0;ii<count;++ii)
		{
		pos = iView.FindL((*aArray)[ii]);
		if(pos!=KErrNotFound)
			{
			idMapping.iId=iView.AtL(pos);
			idMapping.iMapping=pos;
			TRAPD(err,insertError=iGroupContacts.InsertInOrder(idMapping,TLinearOrder<TContactIdWithMapping>(CompareMappingsL)));
			User::LeaveIfError(err);
			User::LeaveIfError(insertError);
			}
		}
	}


/* Static comparision method used for iGroupContacts ordering */
TInt CContactGroupView::CompareMappingsL(const TContactIdWithMapping& aFirst,const TContactIdWithMapping& aSecond)
	{
	if(aFirst.iMapping>aSecond.iMapping)
		{
		return 1;
		}
	if(aFirst.iMapping<aSecond.iMapping)
		{
		return -1;
		}
	ASSERT(aFirst.iMapping!=aSecond.iMapping);
	return 0;
	}

/* 
 * Find a group ID given a group name.
 * @param aGroupName Group name
 * @return Group Item ID if found, KNullContactItemId if group doesn't exist
 */
TContactItemId CContactGroupView::GetGroupIdbyNameL(const TDesC& aGroupName)
	{
	ASSERT(aGroupName.Length()>0);
	
	TContactItemId groupId = KNullContactId;
	CContactIdArray* groups =  iDb.GetGroupIdListL();
	CleanupStack::PushL(groups);
	const TInt groupCount = groups->Count();
	for (TInt ii=0;ii<groupCount;++ii)
		{
		CContactGroup* group = STATIC_CAST(CContactGroup*,MUTABLE_CAST(CContactDatabase&,iDb).ReadContactLC((*groups)[ii]));	
		if(group->GetGroupLabelL().Compare(aGroupName)==0)
			{
			//found Group
			groupId = (*groups)[ii];
			CleanupStack::PopAndDestroy(group);
			break;
			}
		CleanupStack::PopAndDestroy(group);
		}
	CleanupStack::PopAndDestroy(groups);

	return groupId;
	}

/* Handle events from the parent view */
void CContactGroupView::HandleContactViewEvent(const CContactViewBase& aView,const TContactViewEvent& aEvent)
	{
	ASSERT(&aView==&iView);
	TContactViewEvent event=aEvent;
	TBool notifyObservers = ETrue;
	TInt err;
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
				TState oldState = iState;
				iState = EReady;
				TRAP(err,UpdateGroupViewL());
				if(err!=KErrNone)
					{
					iState = oldState;
					NotifyObservers(TContactViewEvent(TContactViewEvent::ESortError,err));
					notifyObservers = EFalse;
					break;
					}
				}
			}
			break;
		case TContactViewEvent::EItemAdded:
			if(&iView==&aView)
				{
				TRAP(err, notifyObservers = HandleAddEventL(event));
				if(err!=KErrNone)
					{
					NotifyObservers(TContactViewEvent(TContactViewEvent::ESortError,err));
					notifyObservers = EFalse;
					break;
					}
				}
			break;
		case TContactViewEvent::EItemRemoved:
			if(&iView==&aView)
				{
                if (event.iContactId == iGroupId)
                    {
                    // The group which this class represents has been deleted
                    // so we need to re-build.
                    TRAP(err, UpdateGroupViewL());
					}
				else
					{
                    // Just a single contact has been deleted.
                    TRAP(err, notifyObservers = HandleRemoveEventL(event));
					}
					
				if(err!=KErrNone)
					{
					NotifyObservers(TContactViewEvent(TContactViewEvent::ESortError,err));
					notifyObservers = EFalse;
					break;
					}
				}
			break;
		case TContactViewEvent::EGroupChanged:
			{
			if(aEvent.iContactId==iGroupId || iGroupId==KNullContactId)
				{
				// The group which this class represents has changed or this
				// view represents the "unfiled" group so we need to re-build.
				TRAP(err,UpdateGroupViewL());
				event.iEventType = TContactViewEvent::ESortOrderChanged;//force children views to re-build themselves.
				//ESortOrderChanged should be changed in future to something like EUnderlyingDataChanged
				event.iContactId = KNullContactId;
				if(err!=KErrNone)
					{
					NotifyObservers(TContactViewEvent(TContactViewEvent::ESortError,err));
					notifyObservers = EFalse;
					break;
					}
				}

			}
			break;
		default:
			ASSERT(EFalse);
		}	

	if (notifyObservers)
		{
		NotifyObservers(event);
		}
	}

/* Handles a EItemRemoved event from the underlying view */
TBool CContactGroupView::HandleRemoveEventL(TContactViewEvent& aEvent)
	{
	TBool retval = EFalse;
	TContactIdWithMapping mapping;
	TInt pos = 0;

	// check to see if the contact being removed belongs to us,
	mapping.iMapping = aEvent.iInt;
	mapping.iId = aEvent.iContactId;
	pos = iGroupContacts.Find(mapping,TIdentityRelation<TContactIdWithMapping>(IdsEqual));
	if ( pos != KErrNotFound ) // contact is a member of our list
		{
		retval = ETrue; // inform clients.
		iGroupContacts.Remove(pos); // remove from our list
		aEvent.iInt = pos;
		}

	UpdateMappingsL();
	return retval;
	}

/* Handles a EItemAdded event from the underlying view */
TBool CContactGroupView::HandleAddEventL(TContactViewEvent& aEvent)
	{
	TBool retval = ETrue;
    aEvent.iInt = KErrNone;

	switch(iGroupType)
		{
		case EShowContactsInGroup:
		case EShowContactsNotInGroup:
				UpdateGroupViewL();
				//Positions have changed in underlying view
				UpdateMappingsL();
		break;


		case EShowContactsNotInAnyGroup:
			{
			retval = HandleAddEventForUnfiledContactsL(aEvent);
			}
		break;

		};
    if(retval && iClassVersion == KClassVersion2) 	//if is the second version of the class put the right values in event
		{
		TContactIdWithMapping mapping;
		mapping.iMapping = aEvent.iInt;
		mapping.iId = aEvent.iContactId;
		TInt pos = iGroupContacts.Find(mapping,TIdentityRelation<TContactIdWithMapping>(IdsEqual));
		
		if ( pos != KErrNotFound ) // contact is a member of our list
			{
			aEvent.iInt = pos;
			}
		else
			{
			retval = EFalse;
			}
		}

	return retval;
	}


TInt CContactGroupView::CompareIndexesAllowingDuplicates(const TContactIdWithMapping& aFirst,const TContactIdWithMapping& aSecond)
	{
	TInt retval = 0;
	if (aFirst.iMapping > aSecond.iMapping) retval = 1;
	if (aFirst.iMapping < aSecond.iMapping) retval = -1;
	return retval;
	}


TBool CContactGroupView::HandleAddEventForUnfiledContactsL(const TContactViewEvent& aEvent )
	{
	TBool retval = EFalse;
	TContactIdWithMapping contactIdWithMapping;
	CContactItem* contact = const_cast<CContactDatabase&>(iDb).ReadContactLC(aEvent.iContactId);


	// If contact is a group then don't do anything - the add event is of no
	// consequence.
	if (contact->Type() == KUidContactGroup)
		{
		CleanupStack::PopAndDestroy(contact);
		// No need to notify observers so return EFalse.
		return EFalse;
		}


	UpdateMappingsL();

	const CContactItemPlusGroup* contactCard = static_cast<const CContactCard*>(contact);
	const CContactIdArray* idArray = contactCard->GroupsJoined();

	if (idArray == NULL || idArray->Count() == 0) //
		{
		contactIdWithMapping.iId = aEvent.iContactId;
		
		// iGroupContacts contains contactIdWithMapping objects which have an index 
		// to the contact item held in the underlying array iView.
		// The mapping needs to be updated with this index before being added to iGroupContacts. 
		contactIdWithMapping.iMapping = iView.FindL(contactIdWithMapping.iId);
		
		iGroupContacts.InsertInOrder(contactIdWithMapping,
							TLinearOrder<TContactIdWithMapping>(CompareIndexesAllowingDuplicates));
		retval = ETrue;
		}


	CleanupStack::PopAndDestroy(contact);
	UpdateMappingsL();
	return retval;
	}

/* 
 * Update view mappings.
 * This is called when an item has been added or removed from the underlying
 * view and the current mappings are therefore invalid.
 */
void CContactGroupView::UpdateMappingsL()
	{
	TInt updatedViewIndex=KErrNotFound;
	for (TInt i=0; i < iGroupContacts.Count(); ++i)
		{
		const TContactItemId idUpdate = iGroupContacts[i].iId;
		updatedViewIndex=iView.FindL(idUpdate);
		if(updatedViewIndex==KErrNotFound)
			{
			//local view has removed this contact, so we need to as well.
			iGroupContacts.Remove(i);
			i--;
			continue;
			}
		iGroupContacts[i].iMapping=updatedViewIndex;
		}
	}

TContactItemId CContactGroupView::AtL(TInt aIndex) const
/** Gets the contact item ID at the specified view index.

@param aIndex Index into the group view.
@leave KErrNotFound aIndex is outside the bounds of the array.
@return The contact item ID. */
	{
	if(aIndex>=iGroupContacts.Count())
		{
		//Out of Bounds.
		User::Leave(KErrNotFound);
		}
	return (iGroupContacts)[aIndex].iId;
	}

const CViewContact& CContactGroupView::ContactAtL(TInt aIndex) const
/** Gets the contact item at the specified view index.

@param aIndex Index into the view of the required item.
@leave KErrNotFound aIndex is outside the bounds of the array.
@return The contact item. */
	{
	if(aIndex>=iGroupContacts.Count())
		{
		//Out of Bounds.
		User::Leave(KErrNotFound);
		}	
	return iView.ContactAtL((iGroupContacts)[aIndex].iMapping);
	}

TInt CContactGroupView::CountL() const
/** Gets the number of contact item IDs in the group view.

@return The number of contact items in the group view. */
	{
	return iGroupContacts.Count();
	}

TInt CContactGroupView::FindL(TContactItemId aId) const
/** Finds the index into the group view of the specified contact item.

@param aId The contact item ID to search for.
@leave KErrNotReady The view is not ready for use.
@return The index of the first matching item in the view or KErrNotFound if 
no matching item can be found. 
*/
	{
	if (iState != EReady)
		{
		User::Leave(KErrNotReady);
		}
	TContactIdWithMapping idWithDummyMapping;
	idWithDummyMapping.iId=aId;
	return iGroupContacts.Find(idWithDummyMapping,TIdentityRelation<TContactIdWithMapping>(IdsEqual));
	}

HBufC* CContactGroupView::AllFieldsLC(TInt aIndex,const TDesC& aSeparator) const
/** Gets a descriptor containing the contents of all fields in an item in the 
view.

The fields are separated by aSeparator.

@param aIndex The index into the view of the contact item.
@param aSeparator The string to use to separate the fields.
@return Pointer to the contact item descriptor. */
	{
	return iView.AllFieldsLC((iGroupContacts)[aIndex].iMapping,aSeparator);
	}

TContactViewPreferences CContactGroupView::ContactViewPreferences()
/** Gets the underlying view's view preferences.

@return The view preferences. */
	{
	return iView.ContactViewPreferences();
	}

const RContactViewSortOrder& CContactGroupView::SortOrderL() const
/** Gets the underlying view's sort order.

@return The sort order. */
	{
	return iView.SortOrderL();
	}

void CContactGroupView::NotifyRemovedMembersL(const CContactIdArray* aArray)
	{
	
	TInt count = iGroupContacts.Count();
	
	if(count > aArray->Count())
		{
		TInt pos = KErrNotFound;
		TContactViewEvent event(TContactViewEvent::EItemRemoved);				

		for(TInt loop = 0;loop < count;++loop)
			{
			pos = aArray->Find(iGroupContacts[loop].iId);
			if(pos != KErrNotFound) 
				{
				iGroupContacts[loop].iMapping = iView.FindL(iGroupContacts[loop].iId);	
				}
			}
		
		for(TInt loop = 0;loop < count;++loop)
			{
			pos = aArray->Find(iGroupContacts[loop].iId);
			if(pos == KErrNotFound)
				{
				event.iContactId = iGroupContacts[loop].iId;
				event.iInt = loop;
				iGroupContacts.Remove(loop);
				--count;
				--loop;
				NotifyObservers(event);
				}
			} // for
		} // if
	}
