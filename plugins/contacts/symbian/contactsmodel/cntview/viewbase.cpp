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

// System includess
#include "ecom/ecom.h"		// For REComSession
#include <ecom/ecomresolverparams.h>

// User includes
#include <cntviewbase.h>
#include "cntstd.h"
#include <cntviewfindconfig.h>
#include "cviewcontactextension.h"
#include <cntviewsortplugin.h>
#include "cntviewprivate.h"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <cntviewsortpluginbase.h>
#endif

extern void DebugLogViewNotification(const TDesC& aMethod, const TContactViewEvent& aEvent);

//uncomment in conjunction with LocalView and T_Cnt_ViewSortProfiling
//#define __PROFILE_SORT__


EXPORT_C RContactViewSortOrder::RContactViewSortOrder() : iSpare(KErrNone), iSpare2(0), iSpare3(0)
/** Default C++ constructor. */
	{
	}

EXPORT_C void RContactViewSortOrder::Close()
/** Closes the sort order array and frees all memory allocated to it. */
	{
	iFields.Close();
	}

EXPORT_C void RContactViewSortOrder::CopyL(const RContactViewSortOrder& aSortOrder)
/** Copies the field types contained in the specified sort order object into this 
object.

Any existing field types in this object are replaced.

@param aSortOrder The sort order to be copied. */
	{
	iFields.Reset();
	const TInt numFields=aSortOrder.iFields.Count();
	for (TInt ii=0;ii<numFields;++ii)
		{
		TFieldType field = aSortOrder.iFields[ii];
		AppendL(field);
		}
	}

EXPORT_C void RContactViewSortOrder::InternalizeL(RReadStream& aStream) 
/** Internalises a sort order object from a read stream.

@param aStream The stream to read from. */
	{
	iSpare = aStream.ReadInt32L();
	iSpare2 = aStream.ReadInt32L();
	iSpare3 = aStream.ReadInt32L();
	TInt numFields;
	numFields = aStream.ReadInt32L();
	for (TInt ii=0;ii<numFields;++ii)
		{
		TFieldType thisField;
		aStream >> thisField.iUid;
		AppendL(thisField);
		}
	}

EXPORT_C void RContactViewSortOrder::ExternalizeL(RWriteStream& aStream) const
/** Externalises a sort order object to a write stream.

@param aStream The stream to write to. */
	{
	aStream.WriteInt32L(iSpare);
	aStream.WriteInt32L(iSpare2);
	aStream.WriteInt32L(iSpare3);
	const TInt numFields=iFields.Count();
	aStream.WriteInt32L(numFields);
	for (TInt ii=0;ii<numFields;++ii)
		{
		aStream << iFields[ii].iUid;
		}
	}

/* 
 * The current implementation relies on the externalized size provided by 
 * the STORE implementation.
 */
EXPORT_C TInt RContactViewSortOrder::ExternalizedSize() const
/** Retrieves the number of bytes required to store the sort order when externalised.

@return The size of the sort order object. */
	{
	return (sizeof(TInt)*4)+(iFields.Count()*sizeof(TFieldType));
	}

EXPORT_C TBool RContactViewSortOrder::operator==(const RContactViewSortOrder& aSortOrder) const
/** Compares two sort order objects for equivalence.

@param aSortOrder The sort order object to compare with this one.
@return ETrue if the two sort orders are the same, EFalse if not. */
	{
	TBool sortOrdersMatch = ETrue;

	const TInt numFields=iFields.Count();
	if (numFields!=aSortOrder.Count())
		{
		sortOrdersMatch = EFalse;
		}
	else
		{
		for (TInt ii=0;ii<numFields;++ii)
			{
			if (iFields[ii]!=aSortOrder.iFields[ii])
				{
				sortOrdersMatch = EFalse;
				break;
				}
			}
		}

	return sortOrdersMatch;
	}


EXPORT_C void CContactViewBase::OpenL(MContactViewObserver& aObserver)
/** Appends an observer to the view's observer array.

If the view's state is EReady, the observer is notified asynchronously that 
the view is ready.

An error causes the function to leave. 

@param aObserver The contact view observer to be added. */
	{
#ifdef CONTACTS_API_PROFILING
	TContactsApiProfile::CntViewMethodLog(TContactsApiProfile::ECntVwClassViewBase, TContactsApiProfile::ECntViewApiOpenL);
#endif
    iObserverArray.AppendL(&aObserver);
	if (GetErrorValueFromExtensionClass()!=KErrNone)
		{
		NotifyObservers(TContactViewEvent(TContactViewEvent::ESortError,iExtension->iError));
		}
	if (iState==EReady)
		{
		User::LeaveIfError(NotifyObserverAsync(aObserver,TContactViewEvent(TContactViewEvent::EReady)));
		}
	}

EXPORT_C TInt CContactViewBase::Open(MContactViewObserver& aObserver)
/** Appends an observer to the view's observer array.

If the view's state is EReady, the observer is notified asynchronously that 
the view is ready.

This function does not leave; an error code is returned instead.

@param aObserver The contact view observer to add. 
@return A standard error code, or KErrNone if the function completed without 
an error. */
	{
#ifdef CONTACTS_API_PROFILING
	TContactsApiProfile::CntViewMethodLog(TContactsApiProfile::ECntVwClassViewBase, TContactsApiProfile::ECntViewApiOpen);
#endif
	TInt err=iObserverArray.Append(&aObserver);
	if (iExtension->iError!=KErrNone)
		{
		NotifyObservers(TContactViewEvent(TContactViewEvent::ESortError,iExtension->iError));
		}
	if (err)
		{
		return err;
		}
	if (iState==EReady)
		{
		err = NotifyObserverAsync(aObserver,TContactViewEvent(TContactViewEvent::EReady));
		if (err!=KErrNone)
			{
			return err;
			}
		}
	return err;
	}

/** 
Removes an observer from the view's observer array.

Any outstanding notifications for aObserver are first cancelled.

@capability None
@param aObserver The contact view observer to be removed.
@return ETrue if the view's observer array is empty as a result of the removal; 
EFalse if it is not empty or if aObserver could not be found in the observer 
array. 
*/
EXPORT_C TBool CContactViewBase::Close(const MContactViewObserver& aObserver)
	{
#ifdef CONTACTS_API_PROFILING
	TContactsApiProfile::CntViewMethodLog(TContactsApiProfile::ECntVwClassViewBase, TContactsApiProfile::ECntViewApiClose);
#endif
	const TInt observerIndex = iObserverArray.Find(&aObserver);
	if (observerIndex != KErrNotFound)
		{
		//the observer that just closed may be expecting a notification
        for (TInt outstandingIndex=iOutstandingNotifications.Count()-1; outstandingIndex>=0; --outstandingIndex)
            {
            if (iOutstandingNotifications[outstandingIndex].iObserverToNotify == &aObserver)
                {
			    iOutstandingNotifications.Remove(outstandingIndex);
                }
            }
		if(iOutstandingNotifications.Count()==0)
			{
			iOutstandingNotifications.Reset();
			if(iAsyncNotifier->IsActive())
				{
				//AsyncNotifier is Active with nothing to notifiy so we need to cancel it.
				iAsyncNotifier->Cancel();
				}
			}
		//remove observer from main array.
		iObserverArray.Remove(observerIndex);
		if (iObserverArray.Count()==0)
			{
			delete this;
			return ETrue;
			}
		}
	return EFalse;
	}

EXPORT_C CContactViewBase::CContactViewBase(const CContactDatabase& aDb)
	: iDb(aDb),iState(EInitializing)
/** Protected C++ constructor.

Initialises the view's state to EInitializing.

@param aDb The database that contains the contact items. */
	{
	}

EXPORT_C CContactViewBase::~CContactViewBase()
/** Destructor.

Cancels all outstanding notifications and deletes all resources owned by the object. */
	{
#ifdef CONTACTS_API_PROFILING
	TContactsApiProfile::CntViewMethodLog(TContactsApiProfile::ECntVwClassViewBase, TContactsApiProfile::ECntViewDestructor);
#endif
	delete iExtension;
	REComSession::FinalClose(); // This line is necessary to make sure the plug-in is unloaded properly
	iObserverArray.Close();
	delete iAsyncNotifier;
	iOutstandingNotifications.Close();
	}

EXPORT_C void CContactViewBase::ConstructL()
/** Protected second phase constructor.

Called from a derived class's ConstructL(). Creates the view's asynchronous 
notifier and adds it to the active scheduler. */
	{
#ifdef CONTACTS_API_PROFILING
	TContactsApiProfile::CntViewMethodLog(TContactsApiProfile::ECntVwClassViewBase, TContactsApiProfile::ECntViewApiConstructL);
#endif
	iAsyncNotifier=CIdle::NewL(CActive::EPriorityStandard);
    iExtension = CContactViewBaseExtension::NewL();
	if(iExtension)
		{
		iExtension->iFindPluginUid = KNullUid;
		iExtension->iError = KErrNone;
		}	
	}

EXPORT_C void CContactViewBase::NotifyObservers(const TContactViewEvent& aEvent)
/** Called by derived view classes to notify their observers synchronously of an 
event, by calling their HandleContactViewEvent() function.

@param aEvent Identifies the event that occurred. */
	{
#ifdef CONTACTS_API_PROFILING
	TContactsApiProfile::CntViewMethodLog(TContactsApiProfile::ECntVwClassViewBase, TContactsApiProfile::ECntViewApiNotifyObservers, aEvent);
#endif
	TInt Observer=iObserverArray.Count();
#if defined(__VERBOSE_DEBUG__)
	//Print the message to be broadcated to the views
	if (Observer)
		{	
		DebugLogViewNotification(_L("[CNTMODEL] CContactViewBase::NotifyObservers"), aEvent);
		}
#endif
	// We iterate backwards to allow for observers removing themselves from the array
	while (Observer--)
		{
		iObserverArray[Observer]->HandleContactViewEvent(*this,aEvent);
		}
	}

EXPORT_C TInt CContactViewBase::NotifyObserverAsync(MContactViewObserver& aObserver,const TContactViewEvent& aEvent)
/** Called to notify a single view observer asynchronously of an event.

If other notifications are outstanding, this notification is appended to the 
queue.

If the view's asynchronous notifier is not active, it is started as a background 
task. 

This function is called by OpenL() and Open() to notify the observer that 
the view is ready.

@param aObserver The contact view observer to notify. This must have previously 
been added to the view's observer array.
@param aEvent Identifies the event that occurred. 
@return KErrNone if the function completed without error, otherwise one of 
the standard error codes. */
	{
#ifdef CONTACTS_API_PROFILING
	TContactsApiProfile::CntViewMethodLog(TContactsApiProfile::ECntVwClassViewBase, TContactsApiProfile::ECntViewApiNotifyObserverAsync, aEvent);
#endif
	TInt err = KErrNone;
	__ASSERT_DEBUG(iObserverArray.Find(&aObserver)!=KErrNotFound,Panic(ECntPanicObserverNotFound));
	TObserverAndEvent outstandingNotification;
	outstandingNotification.iAsyncEvent=aEvent;
	outstandingNotification.iObserverToNotify=&aObserver;
	err = iOutstandingNotifications.Append(outstandingNotification);
	if (err==KErrNone)
		{
		if(!(iAsyncNotifier->IsActive()))
			{
			//No current outstanding notifications, start the asyn notifier to deal with the event that
			//was just added
			__ASSERT_DEBUG(iAsyncNotifier->IsActive()==EFalse,Panic(ECntPanicAsyncViewNotificationPending));
			iAsyncNotifier->Start(TCallBack(AsyncNotifyCallBack,this));
			}
		}
	return err;
	}

TInt CContactViewBase::AsyncNotifyCallBack(TAny* aSelf)
	{
	//Notifiy the bottom element in the outstanding notifiers array,
	//at the end of the function if the array is not empty there are further observers to notify.
	CContactViewBase* self=STATIC_CAST(CContactViewBase*,aSelf);
#ifdef _DEBUG
	TInt outstandingNotifCount = self->iOutstandingNotifications.Count();
	__ASSERT_DEBUG(outstandingNotifCount>0,Panic(ECntPanicObserverNotFound));
#endif
	TContactViewEvent event=self->iOutstandingNotifications[0].iAsyncEvent;
	MContactViewObserver* observerToNotify=self->iOutstandingNotifications[0].iObserverToNotify;
    __ASSERT_DEBUG(observerToNotify && self->iObserverArray.Find(observerToNotify)!=KErrNotFound,Panic(ECntPanicObserverNotFound));
	self->iOutstandingNotifications.Remove(0);
	observerToNotify->HandleContactViewEvent(*self,event);
	return self->iOutstandingNotifications.Count();
	}

EXPORT_C TBool CContactViewBase::IdsMatch(const CViewContact& aFirst,const CViewContact& aSecond)
/** Tests whether the IDs of two contact items are the same.

@param aFirst The first contact item.
@param aSecond The second contact item.
@return True if the IDs match, otherwise false. */
	{
	return (aFirst.Id()==aSecond.Id());
	}

EXPORT_C HBufC* CContactViewBase::FieldsWithSeparatorLC(const RPointerArray<CViewContact>& aContacts,TInt aIndex,const TDesC& aSeparator) const
/** Allocates and returns a descriptor filled with the contents of all the fields 
in a contact item.

The fields are separated by the specified separator.

@param aContacts An array of contact items.
@param aIndex An index into the specified array.
@param aSeparator The text to use to separate the fields.
@return A pointer to a heap descriptor containing the contents of each of the 
contact item's fields. The field separator is appended to each field except 
the last one. The pointer is left on the cleanup stack. */
	{
#ifdef CONTACTS_API_PROFILING
	TContactsApiProfile::CntViewMethodLog(TContactsApiProfile::ECntVwClassViewBase, TContactsApiProfile::ECntViewApiFieldsWithSeparatorLC);
#endif
	// Calculate the length of the buffer.
	TInt bufLength=0;
	const CViewContact& viewContact = *(aContacts[aIndex]);
	const TInt separatorLength=aSeparator.Length();
	const TInt numFields=viewContact.FieldCount();
	for (TInt ii=0;ii<numFields;++ii)
		{
		bufLength+=viewContact.Field(ii).Length()+separatorLength;
		}
	HBufC* buf=HBufC::NewLC(bufLength);
	TPtr bufPtr(buf->Des());

	// Fill the buffer.
	for (TInt j=0;j<numFields;++j)
		{
		bufPtr.Append(viewContact.Field(j));

		// Only put a separator in if this isn't the last field.
		if (j!=numFields-1)
			{
			bufPtr.Append(aSeparator);
			}
		}
	return buf;
	}

EXPORT_C TBool CContactViewBase::MatchesCriteriaL(const CViewContact& aContact,const MDesCArray& aFindWords)
/** Searches a single contact item for fields that contain one or more search strings.

The search uses wildcard matching so that the search strings can occur anywhere 
in any of the item's fields. For a match to occur, all of the search strings 
must be found in the contact item.

Any find configuration plug-in that has been set (by calling SetViewFindConfigPlugin()) 
will be used.

@param aContact The contact item to search.
@param aFindWords A descriptor array containing one or more search strings.
@return True if all of the search strings were present in the contact item's 
fields. False if not. */
	{
	return MatchesCriteriaL(aContact,aFindWords,EFullSearch,iExtension);
	}

EXPORT_C TBool CContactViewBase::MatchesCriteriaL(const CViewContact& aContact,const MDesCArray& aFindWords, TSearchType aSearchType)
/** Searches a single contact item for fields that contain one or more search strings.

The search type indicates whether the search strings can occur anywhere in 
a field, or must occur at the start of a field only. For a match to occur, 
all of the search strings must be found in the contact item.

This function does not use any find configuration plug-in that has been set 
(by calling SetViewFindConfigPlugin()), so new code should use one of the other 
overloads instead. This function has been maintained for binary compatibility 
with previous OS versions.

@param aContact The contact item to search.
@param aFindWords A descriptor array containing one or more search strings.
@param aSearchType The search type. This controls whether a search term can 
occur anywhere in a contact item field, or just at the beginning of a field.
@return True if all of the search strings were present in the contact item's 
fields. False if not. */
	{
	return MatchesCriteriaL(aContact,aFindWords,aSearchType,NULL);
	}

EXPORT_C TBool CContactViewBase::MatchesCriteriaL(const CViewContact& aContact,const MDesCArray& aFindWords, TSearchType aSearchType,CContactViewBase::CContactViewBaseExtension* aExtension)
/** Searches a single contact item for fields that contain one or more search strings.

The search type indicates whether the search strings can occur anywhere in 
a field, or must occur at the start of a field only. For a match to occur, 
all of the search strings must be found in the contact item.

If a find configuration plug-in UID is specified in the fourth parameter, 
this overload uses it, loading it first if necessary. This overrides any plug-in 
that had previously been set by a call to SetViewFindConfigPlugin().

@param aContact The contact item to search.
@param aFindWords A descriptor array containing one or more search strings.
@param aSearchType The search type. This controls whether a search term can 
occur anywhere in a contact item field, or just at the beginning of a field.
@param aExtension Optionally, this can be used to specify the UID of a find 
configuration plug-in, overriding any that has previously been set by a call 
to SetViewFindConfigPlugin().
@return True if all of the search strings were present in the contact item's 
fields. False if not. */
	{
#ifdef CONTACTS_API_PROFILING
	TContactsApiProfile::CntViewMethodLog(TContactsApiProfile::ECntVwClassViewBase, TContactsApiProfile::ECntViewApiMatchesCriteriaL, aContact.Id());
#endif
	CDesCArrayFlat* findWordArray = CreateFindWordArrayLC(aFindWords, aSearchType);
	TBool result = ContactMatchesCriteriaL(aContact, *findWordArray,aExtension);
	CleanupStack::PopAndDestroy(findWordArray);
	return result;
	}

/*
 * Create an array of words to use when matching contact data. 
 * @param aFindWords Array of words
 * @param aSearchType Specifies a prefix or substring match
 * @return Array of words
 */
CDesCArrayFlat* CContactViewBase::CreateFindWordArrayLC(const MDesCArray& aFindWords, TSearchType aSearchType)
	{
	const TInt numFindWords=aFindWords.MdcaCount();
	CDesCArrayFlat* findWordArray = new (ELeave) CDesCArrayFlat(numFindWords);
	CleanupStack::PushL(findWordArray);

	for (TInt i=0; i<numFindWords; ++i)
		{
		TPtrC original(aFindWords.MdcaPoint(i));
		HBufC* findString=HBufC::NewLC(original.Length()+2);
		TPtr ptr(findString->Des());
		switch(aSearchType)
			{
			case EFullSearch:
				ptr.Append('*');
				break;
			case EPrefixSearch:
				break;
			default: // programming error.
				break;
			}
		ptr.Append(original);
		ptr.Append('*');
		findWordArray->AppendL(*findString);
		CleanupStack::PopAndDestroy(findString);
		}
	return findWordArray;
	}

/* 
 * Check if aContact matches aFindWords 
 * @return ETrue if matched, EFalse otherwise
 * new parameter has been added aExtension CContactViewBase extension class.
 */
TBool CContactViewBase::ContactMatchesCriteriaL(const CViewContact& aContact,const MDesCArray& aFindWords,CContactViewBase::CContactViewBaseExtension* aExtension)
	{
	TInt hits = 0;
	const TInt numFields=aContact.FieldCount();
	const TInt numFindWords=aFindWords.MdcaCount();
	if(aExtension->iFindPluginUid != KNullUid && aExtension->iFindPluginImpl == NULL)
	{
		//Load plug-in
		aExtension->iFindPluginImpl = CContactViewFindConfigInterface::NewL(aExtension->iFindPluginUid);
	}
	if (aExtension->iFindPluginImpl)
		{
		for (TInt jj=0;jj<numFindWords;++jj)
			{
			TPtrC word(aFindWords.MdcaPoint(jj));
		    if (aExtension->iFindPluginImpl->IsWordValidForMatching(word))
				{
				for (TInt kk=0;kk<numFields;++kk)
					{
					if (aExtension->iFindPluginImpl->Match(aContact.Field(kk), word))
						{
						++hits;
						break;
						}
					}
				}
			else
				{
				for (TInt kk=0;kk<numFields;++kk)
					{
					if (aContact.Field(kk).MatchC(word)!=KErrNotFound)
						{
						++hits;
						break;
						}
					}
				}
			}
		}
	else
		{
		for (TInt jj=0;jj<numFindWords;++jj)
			{
			TPtrC word(aFindWords.MdcaPoint(jj));
			for (TInt kk=0;kk<numFields;++kk)
				{
				if (aContact.Field(kk).MatchC(word)!=KErrNotFound)
					{
					++hits;
					break;
					}
				}
			}
		}
	return (hits==numFindWords);
	}

/* Implementation of ContactsMatchingCriteriaL and ContactsMatchingPrefixL */
void CContactViewBase::MatchContactsL(const MDesCArray& aFindWords,RPointerArray<CViewContact>& aMatchedContacts, TSearchType aSearchType)
	{
	CDesCArrayFlat* findWordArray = CreateFindWordArrayLC(aFindWords, aSearchType);
	const TInt viewCount = CountL();
	TBool match = EFalse;
	for (TInt ii=0;ii<viewCount;++ii)
		{
		const CViewContact& contact = ContactAtL(ii);
		match=ContactMatchesCriteriaL(contact,*findWordArray,iExtension);
		if (match)
			{
			CViewContact* thisContact = CViewContact::NewL(contact);
			CleanupStack::PushL(thisContact);
			aMatchedContacts.AppendL(thisContact);
			CleanupStack::Pop(thisContact);
			}
		}
	CleanupStack::PopAndDestroy(findWordArray);
	}

/** Searches all contact items in the view for fields that contain all of the search 
strings specified.

The search uses wildcard matching so that the search strings can occur anywhere 
in the item's fields. For a match to occur, all of the search strings must 
be found in a contact item.

@capability ReadUserData
@param aFindWords A descriptor array containing one or more search strings.
@param aMatchedContacts On return, an array of matching contact items. */
EXPORT_C void CContactViewBase::ContactsMatchingCriteriaL(const MDesCArray& aFindWords, RPointerArray<CViewContact>& aMatchedContacts)
	{
	MatchContactsL(aFindWords,aMatchedContacts,EFullSearch);
	}

/** Searches all contact items in the view for fields that contain all of the search 
strings specified.

Unlike ContactsMatchingCriteriaL(), the search term can only occur at the 
beginning of a field.

@capability ReadUserData
@param aFindWords A descriptor array containing one or more search strings.
@param aMatchedContacts On return, an array of matching contact items. */
EXPORT_C void CContactViewBase::ContactsMatchingPrefixL(const MDesCArray& aFindWords, RPointerArray<CViewContact>& aMatchedContacts)
	{
	MatchContactsL(aFindWords,aMatchedContacts, EPrefixSearch);
	}

/**
This is a reserved virtual exported function that is used for BC proofing against 
addition of new exported virtual functions.
This function now aids addition of new exported virtual methods without 
having to break  BC,by simply having those virtual methods as 
non-virtual and helper methods, called from this one and only reserved virtual 
exported method.A public enumeration type that is defined in the base class
CContactViewBase identifies which of the helper 'virtual' methods are being called.
All derived classes of CContactViewBase that are public should mandatorily implement
this method.

Returns any return values of the helper methods called from this function.

@capability ReadUserData
@param aFunction an enum value that identifies which helper method is called.
@param aParams parameters to the helper method being called.
@return Any return values of the helper methods called from this function or NULL.
*/
EXPORT_C TAny* CContactViewBase::CContactViewBase_Reserved_1(TFunction aFunction,TAny* aParams)
	{
#ifdef CONTACTS_API_PROFILING
	TContactsApiProfile::CntViewMethodLog(TContactsApiProfile::ECntVwClassViewBase, TContactsApiProfile::ECntViewApiContactViewBase_Reserved_1);
#endif
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
		case ECContactViewBaseVirtualFunction3:
			{
			TVirtualFunction3Params* vFuncParams = STATIC_CAST(TVirtualFunction3Params*,aParams);
			TInt error = KErrNone;
			error = InsertContactInView(vFuncParams->iContacts,vFuncParams->iNewContact,EFalse,NULL);
			// stuff error code into a TAny *
			return reinterpret_cast<TAny*> (error);
			}
		default:
			break;
		}
	return NULL;
	}

EXPORT_C void CContactViewBase::SetViewFindConfigPlugin(TUid aUid)
/** Sets the UID of the view's find configuration plug-in to use in calls to ContactsMatchingCriteriaL(), 
ContactsMatchingPrefixL() and MatchesCriteriaL().

@param aUid The UID of the view's find configuration plug-in.
@see CContactViewFindConfigInterface */
	{
#ifdef CONTACTS_API_PROFILING
	TContactsApiProfile::CntViewMethodLog(TContactsApiProfile::ECntVwClassViewBase, TContactsApiProfile::ECntViewApiSetViewFindConfigPlugin);
#endif
	iExtension->iFindPluginUid = aUid;
	}

EXPORT_C TUid CContactViewBase::GetViewFindConfigPlugin()
/** Gets the UID of the view's find configuration plug-in, as set by SetViewFindConfigPlugin().

@return The UID of the view's find configuration plug-in. KNullUid if none 
has been set.
@see CContactViewFindConfigInterface */
	{
#ifdef CONTACTS_API_PROFILING
	TContactsApiProfile::CntViewMethodLog(TContactsApiProfile::ECntVwClassViewBase, TContactsApiProfile::ECntViewApiGetViewFindConfigPlugin);
#endif
	return iExtension->iFindPluginUid ;
	}

/** Gets an array containing the IDs of the contact items at the specified view 
indexes.

The IDs are appended to the array.

@capability ReadUserData
@param aIndexes An array of indexes into this view.
@param aContactIds On return, an array to which the contact item IDs of each 
of the indexed items are appended. */
EXPORT_C void CContactViewBase::GetContactIdsL(const CArrayFix<TInt>& aIndexes, CContactIdArray& aContactIds)
    {
#ifdef CONTACTS_API_PROFILING
	TContactsApiProfile::CntViewMethodLog(TContactsApiProfile::ECntVwClassViewBase, TContactsApiProfile::ECntViewApiGetContactIdsL);
#endif
    const TInt myCount = CountL();
    const TInt indexCount = aIndexes.Count();
    for (TInt i=0; i < indexCount; ++i)
        {
        const TInt index = aIndexes[i];
        if (index >= 0 && index < myCount)
            {
            aContactIds.AddL(AtL(index));
            }
        }
    }
/** 
 * Gets an array containing the IDs of the contact items that match
 * the filter supplied by the client.
 * 
 * @param aFilter Filter The filter to use
 * @param aMatchingContacts Array of matching contact IDs
 */
void CContactViewBase::GetContactsMatchingFilterL(TInt aFilter, RArray<TContactIdWithMapping>& aMatchingContacts)
	{
	TContactIdWithMapping idMap;
	const TInt viewCount(CountL());
	for (TInt i=0;i<viewCount;++i)
		{
		const CViewContact& contact = ContactAtL(i);
		if(contact.ContactMatchesFilter(aFilter))
			{
			idMap.iId=contact.Id();
			idMap.iMapping=i;
			aMatchingContacts.AppendL(idMap);
            }
        }
    }

EXPORT_C TBool CContactViewBase::IdsEqual(const TContactIdWithMapping& aFirst,const TContactIdWithMapping& aSecond)
/** Tests whether the IDs of two contact items are the same.

@param aFirst The contact ID/view index struct representing the first item.
@param aSecond The contact ID/view index struct representing the second item.
@return True if the IDs match, false if not. */
	{
	return (aFirst.iId==aSecond.iId);
	}

EXPORT_C TBool CContactViewBase::IndexesEqual(const TContactIdWithMapping& aFirst,const TContactIdWithMapping& aSecond)
/** Tests whether the view indexes of two contact items are the same.

@param aFirst The contact ID/view index struct representing the first item.
@param aSecond The contact ID/view index struct representing the second item. 
@return True if the indexes match, false if not. */
	{
	return (aFirst.iMapping==aSecond.iMapping);
	}

/*
 See defect EXT-59BFK6 "Contact view sorting does not use default collation rules"
 for further information.
 This 'static' function is used by Sort Plugin.
 See method below for version used by default sort.
 Generally fixes should be applied to both!
 */
EXPORT_C TInt CContactViewBase::CompareFieldsL(const CViewContact& aFirst,const CViewContact& aSecond)
/** Collates two contact items' field contents.

This is done by comparing each contact item on a field by field basis starting 
with the loosest collation level initially, and then progressing to tighter 
collation levels only if the items are considered the same at the looser level. 
This is required so that items differing by case only are sorted correctly. 
If a field isn't present, then the comparison is done using the first field 
that is present.

@param aFirst The first contact item.
@param aSecond The second contact item.
@return A positive value indicates that aFirst's field contents are greater 
than aSecond's (so that aFirst would occur after aSecond in a sort order). 
A negative value indicates that aFirst's field contents are less than aSecond's 
(so that aFirst would occur before aSecond in a sort order). Zero indicates 
that aFirst and aSecond have identical field contents. */
	{
	const TInt KDefaultCollationLevel=3;
	TInt comparison = 0; // result of comparison, Zero = fields are identical
	TInt collationLevel = 0;

	TCollationMethod collateMethod = *Mem::CollationMethodByIndex(0);
	collateMethod.iFlags|=TCollationMethod::EIgnoreNone;

	do
		{
		comparison = CompareFieldsWithCollationLevel(aFirst, aSecond, collationLevel, &collateMethod);
		++collationLevel;
		}
		while (comparison == 0 && collationLevel <= KDefaultCollationLevel);

	return comparison;
	}

/*
 Similar to 'static', exported CompareFieldsL() method above.
 Except (for performance) it uses cached copy of TCollationMethod belonging to the view.
 Generally fixes should be applied to both!
 */
TInt CContactViewBase::TextCompareFieldsL(const CViewContact& aFirst,const CViewContact& aSecond) const
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
	{
	const TInt KDefaultCollationLevel=3;
	TInt comparison = 0; // result of comparison, Zero = fields are identical
	TInt collationLevel = 0;

	do
		{
		comparison = CompareFieldsWithCollationLevel(aFirst, aSecond, collationLevel, &iExtension->iCollationMethod);
		++collationLevel;
		}
		while (comparison == 0 && collationLevel <= KDefaultCollationLevel);

	return comparison;
	}

/**  Gets the recorded error value. 
If an error occurs within the view, before each of the views
observers are notified, then the view records the error code internally.
This function provides access to a copy of that error code.


@return The recorded error value. If this does not equal KErrNone, then something is wrong with the view.
*/
EXPORT_C TInt CContactViewBase::Error() const 
	{
	return iExtension->iError;
	}

/* 
 * Extension class for maintaining BC in member variables. 
 */
CContactViewBase::CContactViewBaseExtension* CContactViewBase::CContactViewBaseExtension::NewL()
/** Constructs a new instance of the class. 

If a plug-in has been loaded, its OpenL() function is called, see CContactViewFindConfigInterface::OpenL().

@return Pointer to the newly created extension object. */
    {
    CContactViewBaseExtension* self =
    new( ELeave ) CContactViewBaseExtension;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    } 

CContactViewBase::CContactViewBaseExtension::CContactViewBaseExtension()
    {
    }

CContactViewBase::CContactViewBaseExtension::~CContactViewBaseExtension()
/* Destructor.

If a Find plug-in has been loaded, its Close() function is called. See CContactViewFindConfigInterface::Close()  */
    {
    if ( iFindPluginImpl )
        {
        delete iFindPluginImpl;
        }
	if (iSortPluginImpl)
		{ // unload
		delete iSortPluginImpl;
		}
    }


TInt CContactViewBase::CompareFieldsWithCollationLevel(const CViewContact& aFirst, const CViewContact& aSecond, 
														TInt aCollationLevel, TCollationMethod* aCollateMethod)
	{
	const TInt KLastField = aFirst.FieldCount() - 1;

	TInt retval = 0; // result of comparison, Zero = fields are identical
	TInt firstField(-1);
	TInt secondField(-1);
	TPtrC first;
	TPtrC second;

	for (TInt counter=0; !retval && (counter <= KLastField); counter++)
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
				counter++;

			retval = first.CompareC(second, aCollationLevel, aCollateMethod);
			}
		}

	return retval;
	}


void CContactViewBase::CContactViewBaseExtension::ConstructL()
	{
	if ( iFindPluginImpl )
		{
		iFindPluginImpl->OpenL();
		}

	// Collation method by index 0 stays the same (only changes at a reboot)
 	iCollationMethod = *Mem::CollationMethodByIndex(0);
 	iCollationMethod.iFlags |= TCollationMethod::EIgnoreNone;
	}

/*
 * Finds the first populated field within a contact.
 *
 * Starting at aOffset search for the first populated field. Search up to
 * the maximum number of fields available within the contact.
 *
 * If no populated field is found, then this function will return a zero
 * length TPtrC and set aFoundPosition to -1.
 *
 * @param aOffset          The starting offset
 * @param aFoundPosition   The offset of the field found
 * @return                 The contents of the field located at the found position
 */
TPtrC CViewContact::FindFirstPopulatedField(TInt aOffset, TInt& aFoundPosition) const
	{
	if (iExtension)
		{
		const TInt max = iTextIndexes.Count() - 1;
		TInt lengthOfText=0;

		for (TInt counter = aOffset; counter <= max; counter++)
			{
			const TInt fieldStartPos = iTextIndexes[counter];

			if (counter < max)
				{
				lengthOfText = iTextIndexes[counter+1] - fieldStartPos;
				}
			else
				{
				lengthOfText = iExtension->FieldText().Length() - fieldStartPos;
				}

			if (lengthOfText > 0 )
				{
				aFoundPosition = counter;
				return iExtension->FieldText().Mid(fieldStartPos, lengthOfText);
				}
			}
		}

	// no more fields with text
	aFoundPosition = -1;
	return TPtrC(KNullDesC);
	}

/**
@internalComponent
*/
EXPORT_C CViewContact::CViewContact(TContactItemId aId)
:	iId(aId), iExtension(NULL)
/** Constructs a CViewContact object.

This should not be called by clients. CViewContact::NewLC() should be used instead.

@param aId The contact ID to assign to the object. */
	{
	}

EXPORT_C void CViewContact::InternalizeL(RReadStream& aStream)
/** Internalises a CViewContact object from a read stream. 

@param aStream Stream from which the object is internalised. */
	{
	//
	if(!iExtension)
	    {
	    iExtension = CViewContactExtension::NewL();
	    }
	    
	iId = aStream.ReadInt32L();
	iContactType = STATIC_CAST(TViewContactType,aStream.ReadInt32L());
	SetContactTypeUid(TUid::Uid(aStream.ReadInt32L()));
	iExtension->HintBitField() = aStream.ReadInt32L();
	const TInt indxCount = aStream.ReadInt32L();
	TInt fieldpos=0;
	for (TInt ii=0;ii<indxCount;++ii)
		{
		fieldpos=aStream.ReadInt32L();
		iTextIndexes.AppendL(fieldpos);
		}

	const TInt fieldLength = aStream.ReadInt32L();
	if(fieldLength>0)
		{
		HBufC* field = HBufC::NewLC(fieldLength);
		TPtr ptr(field->Des());
		aStream.ReadL(ptr, fieldLength);
		CleanupStack::Pop(field);
		iExtension->SetFieldText(field);
		}
	else
		{
		iExtension->CreateEmptyFieldTextL();
		}
	}

/*
 * Stores the data in a CViewContact.
 * 
 * The chevron operator is not used when externalizing the field text buffer
 * because using the Unicode compression scheme varies the size of the 
 * externalized text. Using the standard operators is less efficient but 
 * guaranteed to be consistent.
 * 
 * The descriptor is only externalized if it has some length. Store doesn't
 * like externalizing Null descriptors.
 * 
 * @param aStream Stream to write data to
 */
EXPORT_C void CViewContact::ExternalizeL(RWriteStream& aStream) const
/** Externalises a CViewContact object to a write stream.

@param aStream Stream to which the object is externalised. */
	{
	aStream.WriteInt32L(iId);
	aStream.WriteInt32L(iContactType);
	aStream.WriteInt32L(ContactTypeUid().iUid);

	TPtrC pFieldText(KNullDesC);
	TInt hintBitValue = 0;
	if	(iExtension)
		{
		hintBitValue = iExtension->HintBitField();
		pFieldText.Set(iExtension->FieldText());
		}
	//
	aStream.WriteInt32L(hintBitValue);
	const TInt indxCount = iTextIndexes.Count();
	aStream.WriteInt32L(indxCount);
	for (TInt ii=0;ii<indxCount;++ii)
		{
		aStream.WriteInt32L((iTextIndexes)[ii]);
		}
	//
	const TInt fieldLength = pFieldText.Length();
	aStream.WriteInt32L(fieldLength);
	if (fieldLength>0)
		{
		aStream.WriteL(pFieldText);
		}
	}

EXPORT_C TInt CViewContact::ExternalizedSize() const
/** Retrieves the number of bytes required to store the contact view item when 
externalised.

@return The size of the contact view item. */
	{
	TInt size = sizeof(TContactItemId);
	const TInt indxCount = iTextIndexes.Count();
	size += sizeof(TViewContactType);//for contact Type;
	size += sizeof(TUid);
	size += sizeof(TInt); // for iExtension->HintBitField()
	size += sizeof(TInt);//for index count;
	size += sizeof(TInt)*indxCount;//for indexes
	size += sizeof(TInt);//for fieldTextSize
	//
	TInt fieldTextSize = 0;
	if	(iExtension)
		{
		fieldTextSize = iExtension->FieldText().Size();
		}
	size += fieldTextSize;
	//
	return size;
	}

/** 
@internalTechnology
 */
EXPORT_C TUid CViewContact::ContactTypeUid() const
	{
	if(iExtension)
	    {
	    return iExtension->ContactTypeUid();
	    }
	    
	return TUid::Null();
	}
	
/** 
@internalTechnology
Note: This method can leave.

@leave KErrNoMemory The CViewContactExtension cannot be allocated.
 */
EXPORT_C void CViewContact::SetContactTypeUid(TUid aUid)
	{
	if(!iExtension)
	    {
	    iExtension = CViewContactExtension::NewL(); // can leave
	    }
	    
	iExtension->SetContactTypeUid(aUid);
	}

EXPORT_C CViewContact::~CViewContact()
/** Destructor. */
	{
	iTextIndexes.Close();
	delete iExtension;
	}

/** 
@internalTechnology 
 */
CViewContact* CViewContact::NewLC(RReadStream& aStream)
	{
	CViewContact* viewContact = new (ELeave) CViewContact(KErrNotFound);
	CleanupStack::PushL(viewContact);
	viewContact->ConstructL();
	viewContact->InternalizeL(aStream);
	return viewContact;
	}

/*
 * Second phase constructor
 */
void CViewContact::ConstructL(TInt aLength)
	{
	iExtension = CViewContactExtension::NewL(aLength);
	}

/* 
 * Second phase constructor 
 * @param aContact Contact item to copy
 */
void CViewContact::ConstructL(const CViewContact& aContact)
	{
	CopyL(aContact);
	}

EXPORT_C CViewContact* CViewContact::NewLC(TContactItemId aId)
/** Allocates and constructs a CViewContact object, based on an existing one.
@param aId ID of the existing CViewContact object whose values are used to initialise 
the new one. 
@return Pointer to the newly created representation of a contact item used in contact views. 
*/
	{
	CViewContact* contact = new(ELeave) CViewContact(aId);
	CleanupStack::PushL(contact);
	contact->ConstructL();
	return contact;
	}


/** Allocates and constructs a CViewContact object, based on an existing one.

@param aId ID of the existing CViewContact object whose values are used 
to initialise the new one. 
@param aLength initial MaxLenght of a contact buffer 
@internalTechnology 
*/	
EXPORT_C CViewContact* CViewContact::NewL(TContactItemId aId, TInt aLength)
	{
	CViewContact* contact = new(ELeave) CViewContact(aId);
	CleanupStack::PushL(contact);
	contact->ConstructL(aLength);
	CleanupStack::Pop(contact);	
	return contact;
	}	



EXPORT_C CViewContact* CViewContact::NewL(const CViewContact& aContact)
/** Allocates and constructs a CViewContact object, based on an existing one.
@param aContact An existing CViewContact object whose values are used to initialise 
the new one.
@return Pointer to the newly created representation of a contact item used in contact views.
*/
	{
	CViewContact* contact=new(ELeave)CViewContact(aContact.iId);
	CleanupStack::PushL(contact);
	contact->ConstructL(aContact);
	CleanupStack::Pop(contact);
	return contact;
	}

EXPORT_C TBool CViewContact::ContactMatchesFilter(TInt aFilter) const
/** Checks if this contact matches the supplied filter.
@return ETrue, if this contact matches the filter supplied; EFalse, otherwise.
 */
	{
	TInt hintBitField = 0;
	if	(iExtension)
		{
		hintBitField = iExtension->HintBitField();
		}
	//
	return HintFieldMatchesFilter(hintBitField, aFilter);
	}

EXPORT_C TInt CViewContact::FieldCount() const
/** Gets the number of fields in the contact view item.

@return The number of fields in the item. */
	{
	return iTextIndexes.Count();
	}

EXPORT_C TPtrC CViewContact::Field(TInt aPosition) const
/** Gets a pointer descriptor to the contents of the single field located 
at the specified field index.

@param aPosition A field index. 
@return The contents of the field located at the specified field index.
@panic CNTMODEL 27 In debug builds, this indicates that aPosition is invalid. 
It must be less than the number of fields in the item. */
	{
	__ASSERT_DEBUG(aPosition >= 0 && aPosition < FieldCount(),Panic(ECntPanicInvalidViewIndex));
	//
	if	(!iExtension)
		{
		// Have to resort to default value if there is no extension
		// since we cannot obtain the field text without it.
		return TPtrC(KNullDesC);
		}
	//
	const TPtrC pFullFieldText(iExtension->FieldText());
	const TInt fieldStartPos = iTextIndexes[aPosition];
	TInt lengthOfText=0;
	TPtrC fieldPtr;
	if(aPosition<FieldCount()-1)
		{
		//work out the length of the buffer.
		TInt nextFieldPos = iTextIndexes[aPosition+1];
		lengthOfText=nextFieldPos-fieldStartPos;
		fieldPtr.Set(pFullFieldText.Mid(fieldStartPos,lengthOfText));
		}
	else
		{
		//this is the last field.
		fieldPtr.Set(pFullFieldText.Mid(fieldStartPos));
		}
	return fieldPtr;
	}

/*
 * Adds a new field to the CViewContact.
 * Fields are stored in one HBufC. Each field can be accessed using the 
 * indexes stored in the iTextIndexes member.
 * @param aField Field data
 */
EXPORT_C void CViewContact::AddFieldL(const TDesC& aField)
	{
	if(!iExtension)
	    {
	    iExtension = CViewContactExtension::NewL();
	    }

	const TInt insertionPosition = iExtension->FieldText().Length();
	if (aField.Length())
		{
		iExtension->AppendToFieldTextL(aField);
		}
	iTextIndexes.AppendL(insertionPosition);

#ifdef _DEBUG
	// Ensure the field added to single hbuf & index is identical to aField when retrieved.
	TPtrC addedField= Field(FieldCount()-1);
	TInt comp=addedField.Compare(aField);
	__ASSERT_DEBUG(comp==0,Panic(ECntPanicInvalidFieldText));
#endif
	}


/**
Set the first field text for the CViewContact.
Fields are stored in one HBufC. Each field can be accessed using the 
indexes stored in the iTextIndexes member.

@param aField Field data
@internalTechnology 
 */
EXPORT_C void CViewContact::SetFirstFieldForBlankContactL(const TDesC& aFirstField)
	{
	if(!iExtension)
	    {
	    iExtension = CViewContactExtension::NewL();
	    }
	    
	// shouldn't be any existing text
	__ASSERT_DEBUG(iExtension->FieldText().Length()==0,Panic(ECntPanicFieldTextNotBlank));

	iExtension->AppendToFieldTextL(aFirstField);
	
	const TInt length = aFirstField.Length();
	const TInt count = iTextIndexes.Count();
	if (count)
		{
		for (TInt j = 1; j < count; ++j)
			{
			iTextIndexes[j] = length;
			}
		}
	else
		{
		iTextIndexes.AppendL(0);
		}

#ifdef _DEBUG
	// Ensure the field added to single hbuf & index is identical to aField when retrieved.
	TPtrC addedField= Field(0);
	TInt comp=addedField.Compare(aFirstField);
	__ASSERT_DEBUG(comp==0,Panic(ECntPanicInvalidFieldText));
#endif
	}



/** 
Used for creating a view.
Allows to re-use the same contact object what reduces the number of memory re-allocations.
Resets the indexes array and contact buffer. The buffer MaxLength stays the same.
@internalTechnology 
*/
EXPORT_C void CViewContact::Reset()
	{
	SetId(KNullContactId);
	SetContactType(EContactItem);
	SetContactTypeUid(KNullUid);
	iTextIndexes.Reset();
	if(iExtension)
	    {
    	iExtension->SetLengthZero();
	    }
	}



/**
* Compares a given hint field with any given filter.
* static function. Used to support member function ContactMatchesFilter
* and exported function ContactMatchesHintFieldL, from CContactDatabase.
*
* @param aHintField the hint bit flag field
* @param aFilter the filter to compare against
*/
TBool CViewContact::HintFieldMatchesFilter(TInt aHintField, TInt aFilter)
	{
	TBool match=EFalse;

	if (aFilter==CContactDatabase::EUnfiltered)
		{
		return ETrue;
		}

	if (aFilter & ~aHintField & (CContactDatabase::EWork | CContactDatabase::EHome))
		{
		return EFalse;
		}

	match = aFilter & aHintField
		& (
			  CContactDatabase::ELandLine
			| CContactDatabase::ESmsable
			| CContactDatabase::EMailable
			| CContactDatabase::EFaxable
			| CContactDatabase::EPhonable
			| CContactDatabase::ERingTone
			| CContactDatabase::EVoiceDial
			| CContactDatabase::EIMAddress
			| CContactDatabase::EWirelessVillage
			| CContactDatabase::ECustomFilter1
			| CContactDatabase::ECustomFilter2
			| CContactDatabase::ECustomFilter3
			| CContactDatabase::ECustomFilter4
			);

	return match;
	}

/** 
Get this contact's hint field value
@return The contact hint value
@internalTechnology 
 */
EXPORT_C TInt CViewContact::ContactHint() const
	{
	TInt hint = 0;
	if	(iExtension)
		{
		hint = iExtension->HintBitField();
		}
	return hint;
	}

/** 
Set this contact's hint field value

@param aHint The new contact hint value
@internalTechnology 
 */
EXPORT_C void CViewContact::SetContactHint(TInt aHint)
	{
	if(iExtension)
	    {
    	iExtension->HintBitField() = aHint;
	    }
	}


/** 
Test if contact is 'Sortable'
@internalTechnology 
 */
EXPORT_C TBool CViewContact::IsSortable() const
	{
	// If total field text length > 0 then contact is sortable
	if (iExtension && iExtension->FieldText().Length())
		{
		return ETrue;
		}

	return EFalse;
	}

/** 
Change the view contact object to lightweight object to save memory.
It actually just delete the instance of CViewContactExtension.

@internalTechnology 
 */
EXPORT_C void  CViewContact::ChangeToLightweightObject()
    {
	iTextIndexes.Reset();
	delete iExtension;
    iExtension = NULL;    
    }


/** 
Check if the view contact object is a lightweight object.

@internalComponent 
 */
TBool CViewContact::IsLightweightObject() const	
    {
    return (iExtension == NULL);
    }


/** 
Copy the given view contact object to this instance.

@param aContact the view contact to be copied.
@leave KErrNoMemory
@internalComponent 
 */
void CViewContact::CopyL(const CViewContact& aContact)
    {
   	iTextIndexes.Reset();
    if(!iExtension)
        {
	    iExtension = CViewContactExtension::NewL();
        }    
        
	iContactType = aContact.iContactType;
	SetContactTypeUid( aContact.ContactTypeUid() );
	iExtension->HintBitField() = aContact.iExtension->HintBitField();
	HBufC* fieldText = aContact.iExtension->FieldText().AllocL();
	iExtension->SetFieldText(fieldText);
	//
	const TInt indxCount= aContact.iTextIndexes.Count();
	TInt textPos=0;
	for(TInt ii=0;ii<indxCount;++ii)
		{
		textPos = aContact.iTextIndexes[ii];
		iTextIndexes.AppendL(textPos);
		}
    }


/** Used for view sorting and insersion. In order to give a stable result 
if contact details match, it falls back to comparing contact IDs.
If a contact view sort plugin is loaded it uses its SortCompareViewContactsL() method.

@internalAll */
EXPORT_C TInt CContactViewBase::CompareContactsAndIdsL(const CViewContact& aFirst, const CViewContact& aSecond) const
	{
	TInt result = 0;

	// Profiler
#ifdef __PROFILE_SORT__
	RDebug::ProfileStart(3);
#endif

	CViewContactSortPlugin* sortPluginImpl = iExtension->iSortPluginImpl;
	if (sortPluginImpl)
		{
		// use View Sort plugin
		result = sortPluginImpl->SortCompareViewContactsL(aFirst,aSecond);
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

	// Profiler
#ifdef __PROFILE_SORT__
	RDebug::ProfileEnd(3);
#endif

	return result;
	}


/** @internalAll */
EXPORT_C TInt CContactViewBase::CompareContactIds(const CViewContact& aFirst, const CViewContact& aSecond)
	{
	return aFirst.Id() - aSecond.Id();
	}


/** Asks the contact if it is sortable.
'static' function used by Sort Plugin.

@internalAll */
EXPORT_C TBool CContactViewBase::ContactIsSortable(const CViewContact& aContact)
	{
	return aContact.IsSortable();
	}


/** @internalAll */
EXPORT_C TUid CContactViewBase::GetViewSortPluginImplUid() const
	{
	return iExtension->iSortPluginUid;
	}


/** @internalComponent */
TUid CContactViewBase::FindSortPluginImplL (const TDesC8& aSortPluginName,TBool aWildCard) const
	{
	TUid	result = KNullUid;

	TEComResolverParams resolverParams;
	RImplInfoPtrArray	implInfoArray;

	CleanupResetAndDestroyPushL(implInfoArray);

	resolverParams.SetDataType(aSortPluginName);
	resolverParams.SetWildcardMatch(aWildCard);

	// look for ECOM implementations, only in ROM
	REComSession::ListImplementationsL(KCntSortPluginInterfaceUid, resolverParams,  
		KRomOnlyResolverUid, implInfoArray);

	if (implInfoArray.Count() == 1)
		{
		result = implInfoArray[0]->ImplementationUid();
		}

	CleanupStack::PopAndDestroy(&implInfoArray);
	return result;
	}


/** @internalComponent */
void CContactViewBase::LoadViewSortPluginL (TUid aSortPluginUid, TContactViewPreferences& aViewPreferences)
	{
	if (aSortPluginUid == KNullUid)
		{
		User::Leave(KErrNotFound);
		}

	// Parameter block for View
	TSortPluginViewParamsRev1 pluginViewParams(aViewPreferences, CompareFieldsL, ContactIsSortable);
	// General parameter block to load plug-in
	TSortPluginParams pluginParams(KCntSortPluginInterfaceUid, aSortPluginUid, &pluginViewParams);

	iExtension->iSortPluginImpl = CViewContactSortPlugin::NewL(&pluginParams);
	iExtension->iSortPluginUid = aSortPluginUid;
	}


/** @internalComponent */
TUid CContactViewBase::FindDefaultViewSortPluginImplL () const
	{
	// _LIT8(KTestDefaultPluginName, "contacts.vnd.symbian.com/reverse sort");
	return FindSortPluginImplL (KViewSortPluginDefaultName /* KTestDefaultPluginName */ ,ETrue);
	}

/** This function determines whether a contact should be added to the normal
sorted array (iContacts) or put into the alternative iUnSortedContacts array. 
Depending on the view preferences, these "unsorted" contacts are either 
ignored (deleted), or added to the beginning or end of the iContacts 
sorted list by At(), Find() etc... methods.

@internalComponent */
TBool CContactViewBase::IsContactSortable(const CViewContact& aContact, TContactViewPreferences& aViewPreferences) const
	{
	TBool sortableContact=EFalse;

	// only test contact if it matters
	if(aViewPreferences & (EIgnoreUnSorted | EUnSortedAtBeginning | EUnSortedAtEnd)) //Distinguish Unsorted
		{
		CViewContactSortPlugin* sortPluginImpl = iExtension->iSortPluginImpl;
		if (sortPluginImpl)
			{
			// use plugin
			sortableContact = sortPluginImpl->ViewContactIsSortable(aContact);
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

/** @internalComponent */
CViewContactSortPlugin* CContactViewBase::SortPluginImpl() const
	{
	return iExtension->iSortPluginImpl;
	}


/** Does a binary search of the contacts array, and inserts a new contact.
Uses the current sort plugin, if any.

@internalComponent */
TInt CContactViewBase::InsertContactInView(RPointerArray<CViewContact>& aContacts, const CViewContact* aNewContact, TBool aSortByIdOnly, TInt* aIndex) const
	{
	CViewContactSortPlugin* sortPluginImpl = SortPluginImpl();

	TInt error = KErrNone;

	TInt maxPos = aContacts.Count() - 1;
	TInt minPos = 0;
	TInt position = 0;

	if (maxPos >= 0)
		{
		// at least one compare required
		if (sortPluginImpl)
			{
			error = sortPluginImpl->SortStart(CViewContactSortPlugin::ESortStartInsertOne, 1);
			if (error)
				{
				return error;
				}
			}


		position = (minPos + maxPos + 1) / 2;

		while (maxPos >= minPos)
			{
			TInt diff = 0;

			if (aSortByIdOnly)
				{
				diff = CompareContactIds(*aNewContact, *aContacts[position]);
				}
			else
				{
				TRAP(error,diff = CompareContactsAndIdsL(*aNewContact, *aContacts[position]));
				if (error)
					{
					return error;
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
			}

		// all compares done
		if (sortPluginImpl)
			{
			sortPluginImpl->SortCompleted();
			}
		}


	if (error == KErrNone)
		{
		error = aContacts.Insert(aNewContact, position);
		}

	if (aIndex)
		{
		// index of new item for caller
		*aIndex = position;
		}
	return error;
	}

