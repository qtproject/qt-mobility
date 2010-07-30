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


#include <cntviewbase.h>
#include "cntviewprivate.h"
#include <cntviewsortplugin.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <cntviewsortpluginbase.h>
#endif

#include "ccntserver.h" //for kslot1 and kslot2
#include "rcntmodel.h"

// uncomment define in header to select debug prints for API profiling 
#include "cntapiprofile.h"


//
// RContactRemoteView.
//

const TInt KCachedItemCountInvalid = -1;

/**
@capability ReadUserData
*/
void RContactRemoteView::OpenL(const CContactDatabase& aDb,const RContactViewSortOrder& aSortOrder,TContactViewPreferences aContactTypes,const TUid& aSortPluginImplUid,const TDesC8& aSortPluginName) 
	{
	HBufC8* pckgBuf = PackageSortOrderAndPluginDetailsLC(aSortOrder,aContactTypes,aSortPluginImplUid,aSortPluginName);
	TIpcArgs args(pckgBuf->Length());
	args.Set(KSlot1,pckgBuf);
	TInt err = CreateSubSession(*aDb.iCntSvr, ECntCreateView, args );	

	CleanupStack::PopAndDestroy(pckgBuf);
	User::LeaveIfError(err);
	}

/**
@capability ReadUserData
*/
void RContactRemoteView::OpenL(const CContactDatabase& aDb,const TDesC& aName,const RContactViewSortOrder& aSortOrder,TContactViewPreferences aContactTypes,const TUid& aSortPluginImplUid,const TDesC8& aSortPluginName)
	{
	HBufC8* pckgBuf = PackageSortOrderAndPluginDetailsLC(aSortOrder,aContactTypes,aSortPluginImplUid,aSortPluginName);
	TIpcArgs args(pckgBuf->Length());
	args.Set(KSlot1,pckgBuf);
	args.Set(KSlot2,&aName);
	TInt err = CreateSubSession(*aDb.iCntSvr, ECntCreateNamedView, args );	

	CleanupStack::PopAndDestroy(pckgBuf);
	User::LeaveIfError(err);
	}

/**
@capability None
*/
void RContactRemoteView::Close()
	{
	CloseSubSession(ECntCloseView);
	if(iContact!=NULL)
		{
		delete iContact;
		}
	iSortOrder.Close();
	}

/**
Returns the contact item at the specified index into the view.
@capability ReadUserData
@param aIndex Index
*/
CViewContact* RContactRemoteView::ContactAtL(TInt aIndex)
	{
	const TContactItemId KUnknownContactId = -1;

	// 4 context switches
	TPckgBuf<TInt>pckg;
	TIpcArgs args(aIndex,&pckg);
	User::LeaveIfError(SendReceive(ECntViewContactAtLength,args));
	//
	CViewContact* contact = CViewContact::NewLC(KUnknownContactId);
	delete iContact;
	iContact = contact;
	CleanupStack::Pop(contact);
	//
	TInt contactLength=pckg();
	HBufC8* buf=HBufC8::NewLC(contactLength);
	TPtr8 bufPtr(buf->Des());
	TIpcArgs args2(&bufPtr);
	User::LeaveIfError(SendReceive(ECntViewContactAt,args2));
	RDesReadStream readStream(bufPtr);
	CleanupClosePushL(readStream);
	readStream >> *iContact;
	CleanupStack::PopAndDestroy(2); // readStream , buf.
	return iContact;
	}

/**
Searches all contact items in the view for fields that contain the search
strings specified. 

@capability ReadUserData
@param aFindWords A descriptor array containing one or more search strings
@param aMatchedContacts On return, an array of matching contact items
@param find behaviour configuration uid to be passed to the server.
 */
void RContactRemoteView::ContactsMatchingCriteriaL(const MDesCArray& aFindWords, RPointerArray<CViewContact>& aMatchedContacts, TBool aPrefixSearch,TUid aUid)
	{
	if(aUid != KNullUid)
		{
		TIpcArgs args(aUid.iUid);
		User::LeaveIfError(SendReceive(ECntSendPluginUidToServer,args));
		}
	CBufBase* buffer = CBufFlat::NewL(32);
	CleanupStack::PushL(buffer);
	RBufWriteStream writeStream(*buffer);
	CleanupClosePushL(writeStream);

	writeStream.WriteUint32L(aPrefixSearch);
	const TInt count = aFindWords.MdcaCount();
	writeStream.WriteUint32L(count);
	for (TInt i=0; i<count; ++i)
		{
		TPtrC ptr = aFindWords.MdcaPoint(i);
		writeStream.WriteUint32L(ptr.Length());
		writeStream << ptr;
		}
	
	writeStream.CommitL();
	CleanupStack::PopAndDestroy(&writeStream); //writeStream.Close()

	TPtr8 ptr(buffer->Ptr(0));
	const TInt bufferSize = buffer->Size();
	TPckg<TInt> size(bufferSize);

	TPckgBuf<TInt> pckg;
	TIpcArgs args(&pckg,&size,&ptr);
	User::LeaveIfError(SendReceive(ECntContactMatchingCriteriaExternalizedSize,args));
	CleanupStack::PopAndDestroy(buffer);

	//Internalize Contacts
	HBufC8* buf=HBufC8::NewLC(pckg());
	TPtr8 contactsbufPtr(buf->Des());
	TIpcArgs args2(&contactsbufPtr);
	User::LeaveIfError(SendReceive(ECntGetContactMatchingCriteria,args2));

	RDesReadStream readStream(contactsbufPtr);
	CleanupClosePushL(readStream);
	const TInt findCount = readStream.ReadUint32L();
	for (TInt zz=0;zz<findCount;++zz)
		{
		CViewContact* thisContact = CViewContact::NewLC(KNullContactId);
		readStream >> *thisContact;
		aMatchedContacts.AppendL(thisContact);
		CleanupStack::Pop(thisContact);
		}
	CleanupStack::PopAndDestroy(2, buf);
	}

/**
@capability ReadUserData
*/
TContactItemId RContactRemoteView::AtL(TInt aIndex) const
	{
	TPckgBuf<TContactItemId> pckg;
	TIpcArgs args(aIndex,&pckg);
	User::LeaveIfError(SendReceive(ECntViewAt,args));
	return pckg();
	}

/**
@capability ReadUserData
*/
TInt RContactRemoteView::CountL() const
	{
	TPckgBuf<TInt> pckg;
	TIpcArgs args(&pckg);
	User::LeaveIfError(SendReceive(ECntViewCount,args));
	return pckg();
	}

/**
@capability ReadUserData
*/
TInt RContactRemoteView::FindL(TContactItemId aId) const
	{
	TPckgBuf<TInt> pckg;
	TIpcArgs args(aId, &pckg);
	User::LeaveIfError(SendReceive(ECntViewFind,args));
	return pckg();
	}

/**
@capability ReadUserData
*/
HBufC* RContactRemoteView::AllFieldsLC(TInt aIndex,const TDesC& aSeparator) const
	{
	// 4 context switches
	TPckgBuf<TInt>pckg;
	TIpcArgs args(aIndex,&aSeparator,&pckg);
	User::LeaveIfError(SendReceive(ECntAllFieldsLength,args));
	TInt fieldLength = pckg();
	HBufC* buf=HBufC::NewLC(fieldLength);
	TPtr8 narrowBufPtr((TUint8*)buf->Ptr(),buf->Des().MaxLength()*2); // Note, must call MaxLength because the cell allocated may be larger than aLength.
	TIpcArgs args2(&narrowBufPtr);
	User::LeaveIfError(SendReceive(ECntAllFieldsText,args2));
	TPtr bufPtr(buf->Des());
	bufPtr.SetLength(narrowBufPtr.Length()/2);
	return buf;
	}

/**
@capability ReadUserData
*/
void RContactRemoteView::ChangeSortOrderL(const RContactViewSortOrder& aSortOrder)
	{
	TInt size = aSortOrder.ExternalizedSize();
	size +=sizeof(TInt32);//TContactViewPreferences
	TIpcArgs args(size);
	args.Set(KSlot1,PackageSortOrderLC(aSortOrder,ContactViewPreferencesL()));
	User::LeaveIfError(SendReceive(ECntChangeViewSortOrder,args));
	CleanupStack::PopAndDestroy(); // buf.
	}

/**
@capability ReadUserData
*/
void RContactRemoteView::GetSortOrderL(RContactViewSortOrder& aSortOrder)
	{
	TPckgBuf<TInt> pckg;
	TIpcArgs args(&pckg);
	User::LeaveIfError(SendReceive(ECntViewSortOrderExternalizedSize,args));
	HBufC8* buf=HBufC8::NewLC(pckg());
	TPtr8 bufPtr(buf->Des());
	TIpcArgs args2(&bufPtr);
	User::LeaveIfError(SendReceive(ECntGetViewSortOrder,args2));
	RDesReadStream readStream(bufPtr);
	CleanupClosePushL(readStream);
	readStream >> aSortOrder;
	CleanupStack::PopAndDestroy(2); //readStream // buf.
	}

/**
@capability ReadUserData
*/
void RContactRemoteView::RequestViewEvent(TPckgBuf<TContactViewEvent>& aEvent,TRequestStatus& aStatus)
	{
	TIpcArgs args(&aEvent);
	SendReceive(ECntRequestViewEvent,args,aStatus);
	}

/**
@capability None
*/
TInt RContactRemoteView::CancelRequestViewEvent()
	{
	TIpcArgs args;
	return SendReceive(ECntCancelRequestViewEvent,args);
	}

HBufC8* RContactRemoteView::PackageSortOrderLC(const RContactViewSortOrder& aSortOrder,TContactViewPreferences aContactTypes) const
	{
	TInt bufLength=aSortOrder.ExternalizedSize();
	bufLength+=sizeof(TContactViewPreferences);//
	HBufC8* buf=HBufC8::NewLC(bufLength);
	TPtr8 bufPtr(buf->Des());
	RDesWriteStream writeStream(bufPtr);
	writeStream << (TInt32&)aContactTypes;
	writeStream << aSortOrder;
	bufPtr.SetLength(bufLength);
	return buf;
	}

HBufC8* RContactRemoteView::PackageSortOrderAndPluginDetailsLC(const RContactViewSortOrder& aSortOrder,TContactViewPreferences aContactTypes,const TUid& aSortPluginImplUid,const TDesC8& aSortPluginName) const
	{
	// Make a packe that includes the sort plugin name
	TInt bufLength = aSortOrder.ExternalizedSize();
	bufLength += sizeof(TContactViewPreferences);
	bufLength +=sizeof(TInt32);//length of UID
	bufLength +=sizeof(TInt32);//length of aSortPluginName
	bufLength +=aSortPluginName.Length();

	HBufC8* buf=HBufC8::NewLC(bufLength);
	TPtr8 bufPtr(buf->Des());
	RDesWriteStream writeStream(bufPtr);
	writeStream << (TInt32&)aContactTypes;
	writeStream << aSortOrder;
	writeStream.WriteInt32L(aSortPluginImplUid.iUid);
	writeStream.WriteInt32L(aSortPluginName.Length());
	writeStream.WriteL(aSortPluginName);
	bufPtr.SetLength(bufLength);
	return buf;
	}
/**
@capability ReadUserData
*/
const RContactViewSortOrder& RContactRemoteView::SortOrderL()
	{
	iSortOrder.Close();
	GetSortOrderL(iSortOrder);
	return iSortOrder;
	}

/**
@capability ReadUserData
*/
TContactViewPreferences RContactRemoteView::ContactViewPreferencesL()
	{
	TPckgBuf<TContactViewPreferences> pckg;
	TIpcArgs args(&pckg);
	User::LeaveIfError(SendReceive(ECntGetIncludedTypes,args));
	return pckg();
	}

/**
Convert between view indexes and contact ids.
This method makes the request to the server.
@capability ReadUserData
 */
void RContactRemoteView::GetContactIdsL(const CArrayFix<TInt>& aIndexes, CContactIdArray& aContactIds)
	{
	CBufBase* buffer = CBufFlat::NewL(32);
	CleanupStack::PushL(buffer);
	RBufWriteStream writeStream(*buffer);
	CleanupClosePushL(writeStream);

	const TInt count = aIndexes.Count();
	writeStream.WriteUint32L(count);
	for (TInt i=0; i<count; ++i)
		{
		writeStream.WriteUint32L(aIndexes[i]);
		}
	writeStream.CommitL();
	CleanupStack::PopAndDestroy(&writeStream); //writeStream.Close()


	TPtr8 indexes(buffer->Ptr(0));
	const TInt bufferSize = buffer->Size();
	TPckg<TInt> size(bufferSize);
	
	HBufC8* buf=HBufC8::NewLC(bufferSize);
	TPtr8 ids(buf->Des());
	
	TIpcArgs args(&size, &indexes, &ids);
	User::LeaveIfError(SendReceive(ECntGetContactIds,args));


	RDesReadStream readStream(ids);
	CleanupClosePushL(readStream);
	CContactIdArray* array = CContactIdArray::NewLC();
	readStream >> *array;

	const TInt arrayCount = array->Count();
	for(TInt loop=0;loop<arrayCount;loop++)
		aContactIds.AddL((*array)[loop]);

	CleanupStack::PopAndDestroy(4,buffer); //array, &readStream, buf, buffer
	}

/** 
Requests an array of contact IDs from the server-side view which
match the filter provided by the client.

@capability ReadUserData
@param aFilter Filter supplied by client
@param aMatchingContacts Array of contacts which match the filter
 */
void RContactRemoteView::GetContactsMatchingFilterL(TInt aFilter, RArray<TContactIdWithMapping>& aMatchingContacts)
	{
	// Create a large enough buffer to hold whole view and ask server which contacts match
	const TInt viewCount(CountL());
	const TInt maxBufferSize = (1+(viewCount*2))*sizeof(TInt);
	HBufC8* buf=HBufC8::NewLC(maxBufferSize);
	TPtr8 ids(buf->Des());
	TIpcArgs args(aFilter,&ids);
	User::LeaveIfError(SendReceive(ECntGetContactsMatchingFilter,args));

	// Read results from buffer externalized by server
	RDesReadStream readStream(ids);
	CleanupClosePushL(readStream);
	const TInt count = readStream.ReadUint32L();
	TContactIdWithMapping idMap;
	for(TInt i=0;i<count;++i)
		{
		idMap.iId=readStream.ReadInt32L();
		idMap.iMapping=readStream.ReadInt32L();
		aMatchingContacts.AppendL(idMap);
		}
	CleanupStack::PopAndDestroy(2,buf); //readStream, buf
	}

/*
 * Get Implementation Uid of loaded Sort Plug-in from Server View
 */
TUid RContactRemoteView::GetViewSortPluginImplUidL() const
	{
	TUid uid;
	TPckgBuf<TInt> pckg;
	TIpcArgs args(&pckg);
	User::LeaveIfError(SendReceive(ECntGetSortPluginUidFromServer,args));
	uid.iUid = pckg();
	return uid;
	}

//
// CContactRemoteViewNotifier.
//

CContactRemoteViewNotifier::CContactRemoteViewNotifier(RContactRemoteView& aView,TCallBack aCallBack)
	: CActive(EPriorityStandard),iView(aView),iCallBack(aCallBack)
	{
	CActiveScheduler::Add(this);
	}

CContactRemoteViewNotifier::~CContactRemoteViewNotifier()
		{
	Cancel();
		}
/**
@capability ReadUserData
*/
void CContactRemoteViewNotifier::Start()
	{
	ASSERT(!IsActive());
	iView.RequestViewEvent(iEvent,iStatus);
	SetActive();
	}

const TContactViewEvent& CContactRemoteViewNotifier::Event() const
	{
	return MUTABLE_CAST(CContactRemoteViewNotifier*,this)->iEvent();
	}

void CContactRemoteViewNotifier::RunL()
	{
	iCallBack.CallBack();
	Start();
	}

void CContactRemoteViewNotifier::DoCancel()
	{
	iView.CancelRequestViewEvent();
	}


//
// CContactRemoteViewBase.
//

/**
Cancels the active notifier and closes the session with the contacts server.
@internalComponent
@capability None
*/
CContactRemoteViewBase::~CContactRemoteViewBase()
	{
	delete iNotifier;
	iView.Close();
	}

CContactRemoteViewBase::CContactRemoteViewBase(const CContactDatabase& aDb) 
	: CContactViewBase(aDb), iCount(KCachedItemCountInvalid)
/** Protected C++ constructor.

@internalComponent
@param aDb The underlying database containing the contact items. */
	{
	}

void CContactRemoteViewBase::ConstructL(MContactViewObserver& aObserver)
/** Protected second phase constructor. 

Starts an active object which notifies this view (by calling its HandleContactViewEvent() 
function) when view events occur in the server side view. This remote view 
in turn passes the events to its observers.

@capability ReadUserData
@param aObserver An observer that receives notifications when this view is 
ready for use and when changes take place in it. The observer receives a TContactViewEvent::EReady 
event when the view is ready. An attempt to use the view before this notification 
causes a panic. */
	{
	CContactViewBase::ConstructL();
	OpenL(aObserver);
	iNotifier=new(ELeave) CContactRemoteViewNotifier(iView,TCallBack(NotifierCallBack,this));
	iNotifier->Start();
	}

/** Returns the ID of the contact item at a specified index into the the server 
side view.

@capability ReadUserData
@param aIndex An index into the server side view.
@leave KErrNotFound The index is greater than or equal to the number of items 
in the view.
@leave KErrNotReady The view is not ready for use.
@return The ID of the contact item at the specified index.
@panic CntLock 6 The index is less than zero. */
TContactItemId CContactRemoteViewBase::AtL(TInt aIndex) const
	{
#ifdef CONTACTS_API_PROFILING
	TContactsApiProfile::CntViewMethodLog(TContactsApiProfile::ECntVwClassRemoteViewBase, TContactsApiProfile::ECntViewApiAtL, aIndex);
#endif
	if (iState != EReady)
		{
		User::Leave(KErrNotReady);
		}
	return iView.AtL(aIndex);
	}

/** Returns the contact item at a specified index into the server side view.

@capability ReadUserData
@param aIndex An index into the server side view.
@leave KErrNotFound The index is greater than or equal to the number of items 
in the view.
@leave KErrNotReady The view is not ready for use.
@return The contact item at the specified index.
@panic CntLock 6 The index is less than zero. */
const CViewContact& CContactRemoteViewBase::ContactAtL(TInt aIndex) const
	{
	if (iState != EReady)
		{
		User::Leave(KErrNotReady);
		}
	return *CONST_CAST(RContactRemoteView&,iView).ContactAtL(aIndex);
	}

/** Gets the total number of contact items in the server side view.

@capability ReadUserData
@leave KErrNotReady The view is not ready for use.
@return The number of contact items in the view. 
*/
TInt CContactRemoteViewBase::CountL() const
	{
	if (iState != EReady)
		{
		User::Leave(KErrNotReady);
		}
	if (iCount == KCachedItemCountInvalid)
		{
		iCount = iView.CountL();
		}
	return iCount;
	}

/** Searches for a contact item in the server side view with the specified ID.

@capability ReadUserData
@param aId The ID of the contact item to search for.
@leave KErrNotReady The view is not ready for use.
@return If found, the index into the view of the matching item. Otherwise, 
KErrNotFound.
*/
TInt CContactRemoteViewBase::FindL(TContactItemId aId) const
	{
	if (iState != EReady)
		{
		User::Leave(KErrNotReady);
		}
	return iView.FindL(aId);
	}

/** Gets a descriptor containing the contents of all fields specified in the server 
side view's sort order for an item in the view.

The field separator is used to separate the contents of each field. It is 
not appended to the last field.

@capability ReadUserData
@param aIndex The index of the contact item into the view.
@param aSeparator The string to use to separate the fields.
@leave KErrNotFound The index is greater than or equal to the number of items 
in the view.
@leave KErrNotReady The view is not ready for use.
@return Pointer to the contact item descriptor.
@panic CntLock 6 The index is less than zero. */
HBufC* CContactRemoteViewBase::AllFieldsLC(TInt aIndex,const TDesC& aSeparator) const
	{
	if (iState != EReady)
		{
		User::Leave(KErrNotReady);
		}
	return iView.AllFieldsLC(aIndex,aSeparator);
	}

/** Searches all contact items in the server side view for fields that contain 
all of the search strings specified.

The search uses wildcard matching so that the search strings can occur anywhere 
in the item's fields. For a match to occur, all of the search strings must 
be found in a contact item.

@capability ReadUserData
@param aFindWords A descriptor array containing one or more search strings.
@param aMatchedContacts On return, an array of matching contact items. */
void CContactRemoteViewBase::ContactsMatchingCriteriaL(const MDesCArray& aFindWords, RPointerArray<CViewContact>& aMatchedContacts)
	{
	iView.ContactsMatchingCriteriaL(aFindWords,aMatchedContacts,EFalse,GetViewFindConfigPlugin());
	}

/** Searches all contact items in the server side view for fields that contain 
all of the search strings specified.

Unlike ContactsMatchingCriteriaL(), the search term can only occur at the 
beginning of a field.

@capability ReadUserData
@param aFindWords A descriptor array containing one or more search strings.
@param aMatchedContacts On return, an array of matching contact items. */
void CContactRemoteViewBase::ContactsMatchingPrefixL(const MDesCArray& aFindWords, RPointerArray<CViewContact>& aMatchedContacts)
	{
	iView.ContactsMatchingCriteriaL(aFindWords,aMatchedContacts,ETrue,GetViewFindConfigPlugin());
	}


TInt CContactRemoteViewBase::NotifierCallBack(TAny* aSelf)
	{
	CContactRemoteViewBase* self=STATIC_CAST(CContactRemoteViewBase*,aSelf);
	self->HandleContactViewEvent(self->iNotifier->Event());
	return 0;
	}

void CContactRemoteViewBase::HandleContactViewEvent(const TContactViewEvent& aEvent)
	{
	const TInt KInvalidValueForRemoteView = -1;
	
	iCount = KCachedItemCountInvalid;

	switch (aEvent.iEventType)
		{
		case TContactViewEvent::EUnavailable:
		case TContactViewEvent::ESortError:
		case TContactViewEvent::EServerError:
			iState=ENotReady;
			break;
		case TContactViewEvent::EReady:
		case TContactViewEvent::ESortOrderChanged:
			iState=EReady;
			break;
		case TContactViewEvent::EItemAdded:
			// should we consume the event?
			// the very first EItemAdded event will be consumed in this view
			// and not be broadcasted forward. This is because on server side
			// EItemAdded events are sorted before being sent
			if(	aEvent.iContactId == KInvalidValueForRemoteView &&
				aEvent.iInt == KInvalidValueForRemoteView)
				{
				return;
				}
		
			break;
		case TContactViewEvent::EItemRemoved:
			break;
		case TContactViewEvent::EGroupChanged:
			break;
		default:
			ASSERT(EFalse);
		}

	NotifyObservers(aEvent);
	}

/** Gets the server side view's view preferences.

@capability ReadUserData
@return The view preferences. */
TContactViewPreferences CContactRemoteViewBase::ContactViewPreferences()
	{
#ifdef CONTACTS_API_PROFILING
	TContactsApiProfile::CntViewMethodLog(TContactsApiProfile::ECntVwClassRemoteViewBase, TContactsApiProfile::ECntViewApiContactViewPreferences);
#endif
	TContactViewPreferences type=EContactsOnly;
	TRAP_IGNORE(type = iView.ContactViewPreferencesL());
	return type;
	}

/** Gets the server side view's sort order.

@capability ReadUserData
@return The sort order. 
*/
const RContactViewSortOrder& CContactRemoteViewBase::SortOrderL() const
	{
#ifdef CONTACTS_API_PROFILING
	TContactsApiProfile::CntViewMethodLog(TContactsApiProfile::ECntVwClassRemoteViewBase, TContactsApiProfile::ECntViewApiSortOrder);
#endif
	return CONST_CAST(RContactRemoteView&,iView).SortOrderL();
	}

/**
This is a reserved virtual exported function that is used for BC proofing 
against present and future additions of new exported virtual functions.

@capability ReadUserData
*/
TAny* CContactRemoteViewBase::CContactViewBase_Reserved_1(TFunction aFunction,TAny* aParams)
	{
	switch(aFunction)
		{
		case ECContactViewBaseVirtualFunction1:
			{
			TVirtualFunction1Params* vFuncParams = STATIC_CAST(TVirtualFunction1Params*,aParams);
			GetContactIdsL(*vFuncParams->iIndexes,*vFuncParams->iIdArray);
			}
			break;
		case ECContactViewBaseVirtualFunction2:
			{
			TVirtualFunction2Params* vFuncParams = STATIC_CAST(TVirtualFunction2Params*,aParams);
			GetContactsMatchingFilterL(vFuncParams->iFilter, vFuncParams->iMatchingContacts);
			}
			break;
		default:
			return CContactViewBase::CContactViewBase_Reserved_1(aFunction,aParams);
		}
	return NULL;
	}

/*
 * This function was earlier virtual.But it broke BC.so now,this function is 
 * a helper function called from the reserved virtual function that is implemented
 * for BC proofing.The functionality of this function ,however,remains the same.
 */

/** Gets an array containing the IDs of the contact items at the specified indexes 
into the server side view.

The IDs are appended to the array.

@capability ReadUserData
@param aIndexes An array of indexes into the server side view.
@param aContactIds On return, an array to which the contact item IDs of each 
of the indexed items are appended. */
void CContactRemoteViewBase::GetContactIdsL(const CArrayFix<TInt>& aIndexes, CContactIdArray& aContactIds)
	{
	iView.GetContactIdsL(aIndexes, aContactIds);
	}

/**
Requests an array of contact IDs from the server-side view which
match the filter provided by the client.

@capability ReadUserData
*/
void CContactRemoteViewBase::GetContactsMatchingFilterL(TInt aFilter, RArray<TContactIdWithMapping>& aMatchingContacts)
	{
	iView.GetContactsMatchingFilterL(aFilter, aMatchingContacts);
	}

//
// CContactRemoteView.
//

/** Allocates and constructs a remote contact view object.

This function causes a new local view object to be created in the contacts 
server, unless one already exists with the specified view preferences and 
sort order. In that case, this client is given shared access to it.

Starts the active notifier, so that this view receives change events from 
the server side view object. These change events are in turn passed to this 
view's observers.

@capability ReadUserData
@param aObserver An observer that receives notifications when this view is 
ready for use and when changes take place in it. The observer receives a TContactViewEvent::EReady 
event when the view is ready. An attempt to use the view before this notification 
causes a panic.
@param aDb The underlying database containing the contact items.
@param aSortOrder Specifies the fields to use to sort the items in the server 
side view.
@param aContactTypes Specifies which types of contact items should be included 
in the view and the behaviour for items that do not have content in any of 
the fields specified in the sort order.
@return The newly constructed remote view object. */
EXPORT_C CContactRemoteView* CContactRemoteView::NewL(MContactViewObserver& aObserver,const CContactDatabase& aDb,
													  const RContactViewSortOrder& aSortOrder,TContactViewPreferences aContactTypes)
	{
#ifdef CONTACTS_API_PROFILING
	TContactsApiProfile::CntViewMethodLog(TContactsApiProfile::ECntVwClassRemoteView, TContactsApiProfile::ECntViewApiNewL, aSortOrder, aContactTypes);
#endif
	CContactRemoteView* self=new(ELeave) CContactRemoteView(aDb);
	CleanupStack::PushL(self);
	self->ConstructL(aObserver,aSortOrder,aContactTypes);
	CleanupStack::Pop(); // self;
	return self;
	}

/** 
Allocates and constructs a remote contact view object.

This function causes a new local view object to be created in the contacts 
server, unless one already exists with the specified view preferences and 
sort order. In that case, this client is given shared access to it.

Starts the active notifier, so that this view receives change events from 
the server side view object. These change events are in turn passed to this 
view's observers.

@capability ReadUserData
@param aObserver An observer that receives notifications when this view is 
ready for use and when changes take place in it. The observer receives a TContactViewEvent::EReady 
event when the view is ready. An attempt to use the view before this notification 
causes a panic.
@param aDb The underlying database containing the contact items.
@param aSortOrder Specifies the fields to use to sort the items in the server 
side view.
@param aContactTypes Specifies which types of contact items should be included 
in the view and the behaviour for items that do not have content in any of 
the fields specified in the sort order.
@param aSortPluginName Specifies a plug-in that will be used to compare view contacts
when the the view is sorted. This name is used by ECOM to select the plugin, and is matched
with the "default_data" of all ECOM plugins that support the required interface.
@return The newly constructed remote view object. 
*/
EXPORT_C CContactRemoteView* CContactRemoteView::NewL(MContactViewObserver& aObserver,const CContactDatabase& aDb,
													  const RContactViewSortOrder& aSortOrder,TContactViewPreferences aContactTypes,
													  const TDesC8& aSortPluginName)
	{
#ifdef CONTACTS_API_PROFILING
	TContactsApiProfile::CntViewMethodLog(TContactsApiProfile::ECntVwClassRemoteView, TContactsApiProfile::ECntViewApiNewL, aSortOrder, aContactTypes, aSortPluginName);
#endif
	CContactRemoteView* self=new(ELeave) CContactRemoteView(aDb);
	CleanupStack::PushL(self);
	self->ConstructL(aObserver,aSortOrder,aContactTypes, aSortPluginName);
	CleanupStack::Pop(); // self;
	return self;
	}

/**
Gets the server side view's sort order, as set in the NewL() function.

@capability ReadUserData
@param aSortOrder On return, the sort order for the server side view. 
*/
EXPORT_C void CContactRemoteView::GetSortOrderL(RContactViewSortOrder& aSortOrder)
	{
#ifdef CONTACTS_API_PROFILING
	TContactsApiProfile::CntViewMethodLog(TContactsApiProfile::ECntVwClassRemoteView, TContactsApiProfile::ECntViewApiGetSortOrderL);
#endif
	iView.GetSortOrderL(aSortOrder);
	}
/**
This is a reserved virtual exported function that is used for BC proofing 
against present and future additions of new exported virtual functions.
@capability ReadUserData
@param aFunction An enum value that identifies which helper function is called.
@param aParams Parameters to the helper function being called.
@return Any return values of the helper function called from this function or NULL.
*/
TAny* CContactRemoteView::CContactViewBase_Reserved_1(TFunction aFunction,TAny* aParams)
	{
	return CContactRemoteViewBase::CContactViewBase_Reserved_1(aFunction,aParams);
	}

/** Empty destructor. */
CContactRemoteView::~CContactRemoteView()
	{
#ifdef CONTACTS_API_PROFILING
	TContactsApiProfile::CntViewMethodLog(TContactsApiProfile::ECntVwClassRemoteView, TContactsApiProfile::ECntViewDestructor);
#endif
	}

/** 
Protected C++ constructor.

@param aDb The underlying database containing the contact items. 
*/
CContactRemoteView::CContactRemoteView(const CContactDatabase& aDb) : CContactRemoteViewBase(aDb)
	{
	}

/**
@capability ReadUserData
*/
void CContactRemoteView::ConstructL(MContactViewObserver& aObserver,const RContactViewSortOrder& aSortOrder,TContactViewPreferences aContactTypes)
	{
	TUid sortPluginImplUid = FindDefaultViewSortPluginImplL();
	iView.OpenL(iDb,aSortOrder,aContactTypes,sortPluginImplUid,KNullDesC8);
	CContactRemoteViewBase::ConstructL(aObserver);
	// load the (default) Sort Plugin on client side too
	if (sortPluginImplUid != KNullUid)
		{
		LoadViewSortPluginL (sortPluginImplUid, aContactTypes);
		// pass sort order to Sort Plugin
		CViewContactSortPlugin* sortPluginImpl = SortPluginImpl();
		sortPluginImpl->SetSortOrderL(aSortOrder);
		}
	}

/**
@capability ReadUserData
*/
void CContactRemoteView::ConstructL(MContactViewObserver& aObserver,const RContactViewSortOrder& aSortOrder,TContactViewPreferences aContactTypes, const TDesC8& aSortPluginName)
	{
	TUid sortPluginImplUid = KNullUid;
	if (aSortPluginName.Length())
		{
		sortPluginImplUid = FindSortPluginImplL(aSortPluginName);
		}
	iView.OpenL(iDb,aSortOrder,aContactTypes,sortPluginImplUid,aSortPluginName);
	CContactRemoteViewBase::ConstructL(aObserver);
	// load the Sort Plugin on client side too
	if (sortPluginImplUid != KNullUid)
		{
		LoadViewSortPluginL (sortPluginImplUid, aContactTypes);
		// pass sort order to Sort Plugin
		CViewContactSortPlugin* sortPluginImpl = SortPluginImpl();
		sortPluginImpl->SetSortOrderL(aSortOrder);
		}
	}


//
// CContactNamedRemoteView.
//

/** Allocates and constructs a named remote contact view object.

This function causes a new named local view object to be created in the contacts 
server, unless one already exists with the specified name. In that case, this 
client is given shared access to it.

The sort order and view preferences parameters are only used if a new view 
object is created in the server. They cannot be used to change the sort order 
or view preferences of an existing view in the server. If you want to re-sort 
a shared view, use ChangeSortOrderL().

@capability ReadUserData 
@param aObserver An observer that receives notifications when this view is 
ready for use and when changes take place in it. The observer receives a TContactViewEvent::EReady 
event when the view is ready. An attempt to use the view before this notification 
causes a panic.
@param aName The name of the view object in the server. If a view with this 
name already exists, it is shared. If not, a new view with this name is created.
@param aDb The underlying database containing the contact items.
@param aSortOrder Specifies the fields to use to sort the items in the new 
server side view, if one is created.
@param aContactTypes Specifies which types of contact items should be included 
in the new server side view and the behaviour for items that do not have content 
in any of the fields specified in the sort order. This is only used if a new 
server side view is created.
@return The newly constructed named remote view object. */
EXPORT_C CContactNamedRemoteView* CContactNamedRemoteView::NewL(MContactViewObserver& aObserver,const TDesC& aName,const CContactDatabase& aDb,const RContactViewSortOrder& aSortOrder,TContactViewPreferences aContactTypes)
	{
#ifdef CONTACTS_API_PROFILING
	TContactsApiProfile::CntViewMethodLog(TContactsApiProfile::ECntVwClassNamedRemoteView, TContactsApiProfile::ECntViewApiNewL, aName, aSortOrder, aContactTypes);
#endif
	CContactNamedRemoteView* self=new(ELeave) CContactNamedRemoteView(aDb);
	CleanupStack::PushL(self);
	self->ConstructL(aObserver,aName,aSortOrder,aContactTypes);
	CleanupStack::Pop(); // self;
	return self;
	}

/** Allocates and constructs a named remote contact view object.

This function causes a new named local view object to be created in the contacts 
server, unless one already exists with the specified name. In that case, this 
client is given shared access to it.

The sort order and view preferences parameters are only used if a new view 
object is created in the server. They cannot be used to change the sort order 
or view preferences of an existing view in the server. If you want to re-sort 
a shared view, use ChangeSortOrderL().

@capability ReadUserData 
@param aObserver An observer that receives notifications when this view is 
ready for use and when changes take place in it. The observer receives a TContactViewEvent::EReady 
event when the view is ready. An attempt to use the view before this notification 
causes a panic.
@param aName The name of the view object in the server. If a view with this 
name already exists, it is shared. If not, a new view with this name is created.
@param aDb The underlying database containing the contact items.
@param aSortOrder Specifies the fields to use to sort the items in the new 
server side view, if one is created.
@param aContactTypes Specifies which types of contact items should be included 
in the new server side view and the behaviour for items that do not have content 
in any of the fields specified in the sort order. This is only used if a new 
server side view is created.
@param aSortPluginName Specifies a plug-in that will be used to compare view contacts
when the the view is sorted. This name is used by ECOM to select the plugin, and is matched
with the "default_data" of all ECOM plugins that support the required interface.
@return The newly constructed named remote view object. */
EXPORT_C CContactNamedRemoteView* CContactNamedRemoteView::NewL(MContactViewObserver& aObserver,const TDesC& aName,const CContactDatabase& aDb,const RContactViewSortOrder& aSortOrder,TContactViewPreferences aContactTypes,
															    const TDesC8& aSortPluginName)
	{
#ifdef CONTACTS_API_PROFILING
	TContactsApiProfile::CntViewMethodLog(TContactsApiProfile::ECntVwClassNamedRemoteView, TContactsApiProfile::ECntViewApiNewL, aName, aSortOrder, aContactTypes, aSortPluginName);
#endif
	CContactNamedRemoteView* self=new(ELeave) CContactNamedRemoteView(aDb);
	CleanupStack::PushL(self);
	self->ConstructL(aObserver,aName,aSortOrder,aContactTypes,aSortPluginName);
	CleanupStack::Pop(); // self;
	return self;
	}

/** Re-sorts the server side view using the specified sort order.

The sort is done using a low priority idle time active object. When sorting 
is complete, all view observers are notified.

@capability ReadUserData
@param aSortOrder Specifies the fields to use to sort the items in the view. */
EXPORT_C void CContactNamedRemoteView::ChangeSortOrderL(const RContactViewSortOrder& aSortOrder)
	{
#ifdef CONTACTS_API_PROFILING
	TContactsApiProfile::CntViewMethodLog(TContactsApiProfile::ECntVwClassNamedRemoteView, TContactsApiProfile::ECntViewApiChangeSortOrderL, aSortOrder);
#endif
	iView.ChangeSortOrderL(aSortOrder);
	// pass new sort order to Sort Plugin
	CViewContactSortPlugin* sortPluginImpl = SortPluginImpl();
	if (sortPluginImpl)
		{
		sortPluginImpl->SetSortOrderL(aSortOrder);
		}
	}

/**
This is a reserved virtual exported function that is used for BC proofing 
against present and future additions of new exported virtual functions.
@capability ReadUserData 
@param aFunction An enum value that identifies which helper function is called.
@param aParams Parameters to the helper function being called.
@return Any return values of the helper function called from this function or NULL.
*/
TAny* CContactNamedRemoteView::CContactViewBase_Reserved_1(TFunction aFunction,TAny* aParams)
	{
	return CContactRemoteView::CContactViewBase_Reserved_1(aFunction,aParams);
	}

CContactNamedRemoteView::~CContactNamedRemoteView()
	{
#ifdef CONTACTS_API_PROFILING
	TContactsApiProfile::CntViewMethodLog(TContactsApiProfile::ECntVwClassNamedRemoteView, TContactsApiProfile::ECntViewDestructor);
#endif
	}

CContactNamedRemoteView::CContactNamedRemoteView(const CContactDatabase& aDb) : CContactRemoteView(aDb)
	{
	}

/**
@capability ReadUserData 
*/
void CContactNamedRemoteView::ConstructL(MContactViewObserver& aObserver,const TDesC& aName,const RContactViewSortOrder& aSortOrder,TContactViewPreferences aContactTypes)
	{
	TUid sortPluginImplUid = FindDefaultViewSortPluginImplL();
	iView.OpenL(iDb,aName,aSortOrder,aContactTypes,sortPluginImplUid,KNullDesC8);
	CContactRemoteViewBase::ConstructL(aObserver);
	// load the (default) Sort Plugin on client side too
	if (sortPluginImplUid != KNullUid)
		{
		LoadViewSortPluginL (sortPluginImplUid, aContactTypes);
		// pass sort order to Sort Plugin
		CViewContactSortPlugin* sortPluginImpl = SortPluginImpl();
		sortPluginImpl->SetSortOrderL(aSortOrder);
		}
	}

/**
@capability ReadUserData
*/
void CContactNamedRemoteView::ConstructL(MContactViewObserver& aObserver,const TDesC& aName,const RContactViewSortOrder& aSortOrder,TContactViewPreferences aContactTypes, const TDesC8& aSortPluginName)
	{
	TUid sortPluginImplUid = KNullUid;
	if (aSortPluginName.Length())
		{
		sortPluginImplUid = FindSortPluginImplL(aSortPluginName);
		if (sortPluginImplUid == KNullUid)
			{
			User::Leave(KErrNotFound);
			}
		}
	iView.OpenL(iDb,aName,aSortOrder,aContactTypes,sortPluginImplUid,aSortPluginName);
	CContactRemoteViewBase::ConstructL(aObserver);
	// load the same Sort Plugin on client side as on the Server side
	sortPluginImplUid = iView.GetViewSortPluginImplUidL();
	if (sortPluginImplUid != KNullUid)
		{
		LoadViewSortPluginL (sortPluginImplUid, aContactTypes);
		// pass sort order to Sort Plugin
		CViewContactSortPlugin* sortPluginImpl = SortPluginImpl();
		sortPluginImpl->SetSortOrderL(aSortOrder);
		}
	}
