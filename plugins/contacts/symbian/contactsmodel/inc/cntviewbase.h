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


#ifndef __CNTVIEWBASE_H__
#define __CNTVIEWBASE_H__

// System includes
#include <e32std.h>
#include <s32mem.h>

#include <cntdb.h>

// Classes referenced
class CViewContactExtension;
class CContactViewBase;
class CContactRemoteViewNotifier;
class CIdleContactSorter;
class CContactViewFindConfigInterface;
class CViewContactSortPlugin;
class TSortPluginViewParams;

/** View preferences for sorted contact views.

This is used to specify the types of contact item that should be sorted and 
included in the view and the behaviour for items that cannot be sorted because 
they do not have content in any of the fields specified in the view's sort 
order.

The default behaviour is to include contact cards only and to sort contact 
cards without content in any of the sort order fields using the first available 
field containing any text.

The view preferences for all concrete contact view classes can be retrieved 
using their implementation of CContactViewBase::ContactViewPreferences(). 
The view preferences are set during construction of a CContactLocalView. 

@publishedAll
@released
*/
enum TContactViewPreferences
	{
	/** Only contact cards (of type KUidContactCard or KUidContactOwnCard) are included 
	in the view. This is the default. */
	EContactsOnly					=0x00000000,
	/** Only contact groups (of type KUidContactGroup) are included in the view. */
	EGroupsOnly						=0x00000001, 
	/** Contact groups and contact cards (of type KUidContactGroup, KUidContactCard 
	or KUidContactOwnCard) are included in the view. */
	EContactAndGroups				=0x00000002, 
	/** Excludes contact items from the view which don't have content in any of the 
	fields specified in the sort order. */
	EIgnoreUnSorted					=0x00000004, 
	/** Includes contacts in the view which don't have content in any of the fields 
	specified in the sort order. These contacts are placed in an unsorted contact 
	list which is located before the sorted list. */
	EUnSortedAtBeginning			=0x00000008, 
	/** Includes contacts in the view which don't have content in any of the fields 
	specified in the sort order. These contacts are placed in an unsorted contact 
	list which is located after the sorted list. */
	EUnSortedAtEnd					=0x00000010, 
	/** Fields containing a single white space only are treated as empty, and therefore 
	unsortable. */
	ESingleWhiteSpaceIsEmptyField	=0x00000020,
	/** Only ICC entries (of type KUidContactICCEntry) are included in the view. */
	EICCEntriesOnly					=0x00000040, 
	/** Only contact cards and ICC entries (of type KUidContactCard, KUidContactOwnCard 
	or KUidContactICCEntry) are included in the view. */
	EICCEntriesAndContacts			=0x00000080  
	};


class TContactViewEvent
/** Identifies a contact view event.

Sent by contact views when notifying their observers of an event.

@see MContactViewObserver::HandleContactViewEvent() 
@publishedAll
@released
*/
	{
public:
	/** Identifies the event's type. */
	enum TEventType
		{
		/** The observed view's state has changed from EReady to either ENotReady or EInitializing, 
		so is not available for use. */
		EUnavailable,
		/** The observed view's state has changed from ENotReady or EInitializing to EReady 
		so is available for use. */
		EReady,
		/** The observed view's sort order has changed, so observer views need to update 
		themselves. */
		ESortOrderChanged,
		/** An error occurred when sorting the observed view or when appending an observer 
		to its observer array. 
	
		The error code is provided in iInt. */
		ESortError,
		/** An error occurred in the contacts server.
	
		The error code is provided in iInt. */
		EServerError,
		/** An error occurred when setting the range for a CContactSubView.
	
		The error code is provided in iInt. */
		EIndexingError,
		/** An item has been added to the observed view. 
	
		The ID of the added item is provided in iContactId and the index into the 
		observed view of the added item is provided in iInt. 
		Exception from this are the first version of CContactFindView, CContactSubView and
		CContactGroupView classes.
		For these classes, iInt has KErrNone value */

		EItemAdded,
		/** An item has been removed from the observed view. 
	
		The ID of the removed item is provided in iContactId and the index into the 
		observed view of the item is provided in iInt. 
		Exception from this are the first version of CContactFindView and CContactSubView classes.
		For these classes, iInt has KErrNone value */

		EItemRemoved,
		/** A change has occurred in a contact group, for instance a contact item has been 
		moved into or out of the group.
	
		The ID of the group affected is provided in iContactId. */
		EGroupChanged
		};
public:
	inline TContactViewEvent();
	inline TContactViewEvent(TEventType aEventType,TInt aInt = KErrUnknown,TContactItemId aContactId = KErrUnknown); 
public:
	/** The type of event. */
	TEventType iEventType;
	/** The error code (where relevant) or the index of the contact item added to/removed 
	from the underlying view. */
	TInt iInt; 
	/** The ID of the contact item that has been added or removed or the group ID, where 
	relevant. */
	TContactItemId iContactId;//ContactId that has been added / removed if relevant.
	};


struct TContactIdWithMapping
/** A struct used internally by filtered and group views to pair a contact item 
ID and its index into the underlying view. 
@publishedAll
@released
*/
	{
	/** The contact item's ID. */
	TContactItemId iId;
	/** The index of the item into the view's underlying view. */
	TInt iMapping;
	};


class MContactViewObserver
/** The interface for a contact view observer.

Objects that need to observe a contact view should implement this interface. 
The view observer should be passed to the observed view's OpenL() or Open() 
function. This adds the observer to the view's observer array: a view can 
have more than one observer. The observers receive notifications when the 
observed view becomes ready for use and when changes occur in it.

Many contact view classes implement this interface to observe an underlying 
view. They in turn send notification to any objects observing them.

@see CContactViewBase::NotifyObservers()
@see CContactViewBase::NotifyObserverAsync() 
@publishedAll
@released
*/
	{
public:
	/** Handles an event in an observed contact view.
	
	@param aView The contact view causing the notification.
	@param aEvent The event. */
	virtual void HandleContactViewEvent(const CContactViewBase& aView,const TContactViewEvent& aEvent)=0;
	};


class RContactViewSortOrder
/** Specifies the sort order for a contact view.

It is implemented as an array of TFieldType UIDs, which define the fields 
whose contents are used to sort on, and their order.

The sort order for all concrete contact view classes can be retrieved using 
their implementation of CContactViewBase::SortOrderL(). The sort order is 
set during construction of local and remote views. 
@publishedAll
@released
*/
	{
public:
	IMPORT_C RContactViewSortOrder();
	IMPORT_C void Close();
	IMPORT_C void CopyL(const RContactViewSortOrder& aSortOrder);
	IMPORT_C void InternalizeL(RReadStream& aStream);
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
	IMPORT_C TInt ExternalizedSize() const;
	IMPORT_C TBool operator==(const RContactViewSortOrder& aSortOrder) const;
	inline void AppendL(TFieldType aField);
	inline TFieldType operator[](TInt aIndex) const;
	inline TInt Count() const; 
private:
	RArray<TFieldType> iFields;
	TInt iSpare;
	TInt32 iSpare2;
	TInt32 iSpare3;
	};


/**
Wrapper around RArray, to container TTextFieldMinimal buffers.

Features: minimum size 1, easy resize of array, persistent last counted total.

@internalComponent
@released
*/
class RFieldBufferArray : public RArray<CContactDatabase::TTextFieldMinimal>
    {
public:
	explicit RFieldBufferArray();

	void ResizeL(TInt aNewSize);
	void ZeroAll();
	TInt NewTotal();
	TInt LastTotal() const;

private:
	TInt iTotalLength;
    };



class CViewContact : public CBase
/** The representation of a contact item used in contact views.

It stores the contact item ID and other information. This includes a buffer 
holding the contents of all the item's fields, an array of indexes into the 
buffer indicating the start position of each field and a hint bit field (a 
combination of CContactDatabase::TContactViewFilter values) which is used 
in filtered views.

The fields are defined by the view's sort order (RContactViewSortOrder). 
@publishedAll
@released
*/
	{
public:
	/** Defines whether the contact view item is a contact group. */
	enum TViewContactType
		{
		/** The contact view item is not a contact group. */
		EContactItem,
		/** The contact view item is a contact group (CContactGroup). */
		EGroup
		};
	IMPORT_C CViewContact(TContactItemId aId);
	IMPORT_C static CViewContact* NewL(const CViewContact& aContact);
	IMPORT_C static CViewContact* NewLC(TContactItemId aId);
	IMPORT_C static CViewContact* NewL(TContactItemId aId, TInt aLength);
	static CViewContact* NewLC(RReadStream& aStream);
	
	IMPORT_C ~CViewContact();
	IMPORT_C void InternalizeL(RReadStream& aStream);
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
	IMPORT_C TInt ExternalizedSize() const;
	IMPORT_C TInt FieldCount() const;
	IMPORT_C TPtrC Field(TInt aPosition) const;
	IMPORT_C TBool ContactMatchesFilter(TInt aFilter) const;
	IMPORT_C void AddFieldL(const TDesC& aField);
	IMPORT_C TBool IsSortable() const;
	IMPORT_C void SetFirstFieldForBlankContactL(const TDesC& aFirstField);
	IMPORT_C void Reset();
	IMPORT_C TInt ContactHint() const;
	IMPORT_C void SetContactHint(TInt aHint);
	IMPORT_C TUid ContactTypeUid() const;
	IMPORT_C void SetContactTypeUid(TUid aUid);

	inline TContactItemId Id() const;
	inline TViewContactType ContactType() const;
public:
	inline void SetId(TContactItemId aId);
	inline void SetContactType(CViewContact::TViewContactType aContactType);
	static TBool HintFieldMatchesFilter(TInt aHintField, TInt aFilter);
	TPtrC FindFirstPopulatedField(TInt aOffset, TInt& aFoundPosition) const;
	
    IMPORT_C void  ChangeToLightweightObject();
    TBool IsLightweightObject() const;	
    void  CopyL(const CViewContact& aContact);

private: 
	void ConstructL(TInt aLength = 0);
	void ConstructL(const CViewContact& aContact);
	friend class CContactViewBase;
private:
	TContactItemId iId;
	TViewContactType iContactType;
	CViewContactExtension* iExtension;
	RArray<TInt> iTextIndexes;
	};


/*
 * A view can be named or un-named. An un-named view has a fixed sort order - the 
 * only way to change this is by closing the object and creating a new one with a 
 * different order. Named views may have their sort order changed. When this is 
 * done, first all observers will be informed that the view is 'unavailable'. This
 * notification will be followed by 'sort order changed' and 'ready'. Only once 
 * the 'ready' notification has been received may the view be used again.
 */
class CContactViewBase : public CBase
/** The abstract base class for all contact view classes.

All contact views operate asynchronously, so users of the view must observe 
it, by implementing the MContactViewObserver interface. Immediately after 
construction, views are not in a usable state (the underlying data may be 
being sorted, for instance). Only after the user has received a TContactViewEvent::EReady 
event may the view be used.

View observers are also notified when changes occur, for instance when the 
view's state changes (see TState), when an error occurs, or when an item is 
added or removed. 
@publishedAll
@released
*/
	{
public:
	class CContactViewBaseExtension : public CBase
	/** An extension class that holds member data added in v7.0s.

	It was created for binary compatibility purposes. 
	@internalComponent
	@released */
		{
		public:
			static CContactViewBaseExtension* NewL();
			~CContactViewBaseExtension();

		private:
			CContactViewBaseExtension();
			void ConstructL();

		public:
			/** A standard error code that is passed to view observers by NotifyObservers() 
			(unless its value is KErrNone). */
			TInt iError;
			/** The UID of the view's find configuration plug-in.
	
			On construction, this is initialised to KNullUid.
	
			The plug-in is loaded when ContactsMatchingCriteriaL(), ContactsMatchingPrefixL() 
			or MatchesCriteriaL() is called. */
			TUid iFindPluginUid;
			/** The UID of the view's sort plug-in. */
			TUid iSortPluginUid;
		private:
			CContactViewFindConfigInterface*	iFindPluginImpl;
			CViewContactSortPlugin*				iSortPluginImpl;
			TCollationMethod					iCollationMethod;
			friend class CContactViewBase;
		};
	class TVirtualFunction1Params
	/** Holds the two parameters passed to the helper method GetContactIdsL() from 
	the reserved function CContactViewBase_Reserved_1(). It has an inline constructor 
	to initialise the data members. 
	@publishedAll
	@released */
		{
		public:
		   /** Inline constructor to initialize member data
			  @param aIndexes Pointer to an array of indexes in a view
	          @param aIdArray Pointer to array of contact IDs  */
			inline TVirtualFunction1Params(const CArrayFix<TInt>* aIndexes, CContactIdArray* aIdArray) :iIndexes(aIndexes), iIdArray(aIdArray){};
			/** Pointer to an array of indexes in a view  */
			const CArrayFix<TInt>* iIndexes;
			/** Pointer to array of contact IDs  */
			CContactIdArray* iIdArray;
		};
	class TVirtualFunction2Params
	/** Holds the two parameters passed to the method GetContactsMatchingFilterL() 
	from the reserved function CContactViewBase_Reserved_1(). It has an inline 
	constructor to initialise the data members.
	*/
		{
		public:
		   /** Inline constructor to initialize member data.
			  @param aFilter Filter for the contacts
	          @param aMatchingContacts Contacts matching a particular criterion  */
			inline TVirtualFunction2Params(TInt aFilter, RArray<TContactIdWithMapping>& aMatchingContacts) : iFilter(aFilter), iMatchingContacts(aMatchingContacts){};
		public:
			/** Filter for the contacts. */
			TInt iFilter;
			/** List of contacts matching the criteria. */
			RArray<TContactIdWithMapping>& iMatchingContacts;
		};
	class TVirtualFunction3Params
	/** Holds the two parameters passed to the method InsertContactInView() from the 
	reserved function CContactViewBase_Reserved_1(). It has an inline 
	constructor to initialise the data members.

	@internalAll */
		{
		public:
			inline TVirtualFunction3Params(RPointerArray<CViewContact>& aContacts, const CViewContact* aNewContact) :iContacts(aContacts), iNewContact(aNewContact){};
			RPointerArray<CViewContact>&	iContacts;
			const CViewContact*				iNewContact;
		};
	/** Search type.

	This controls whether a search term can occur anywhere in a contact item field, 
	or just at the beginning of the field. */
	enum TSearchType
		{
		/** The search uses full wildcard matching so that the search string can occur anywhere 
		in the item's fields. */
		EFullSearch,
		/** The search uses partial wildcard matching so that the search string can only 
		occur at the beginning of the item's fields. */
		EPrefixSearch
		};
	/** Identifies the functions that have been added to CContactViewBase, or that 
	may be added in the future. This identifier is passed to the reserved virtual 
	function CContactViewBase_Reserved_1() and enables the addition of new virtual 
	methods without breaking binary compatibility. */
	enum TFunction
		{
		/** Identifies the function CContactViewBase::GetContactIdsL(). */
		ECContactViewBaseVirtualFunction1,
		/** Identifies the function CContactViewBase::GetContactsMatchingFilterL(). */
		ECContactViewBaseVirtualFunction2,
		// communication with Sort Plug-in
		/** Identifies the function CContactViewBase::InsertContactInView(). */
		ECContactViewBaseVirtualFunction3,
		};
protected:
	/** Defines the contact view states. */
	enum TState
		{
		/** The view is initialising.
	
		This is the view's state immediately after construction or after a significant change 
		(e.g CurrentDatabaseChanged or UnknownChanges),	indicating that it can't be used yet. */
		EInitializing,
		/** The view is ready to be used. */
		EReady,
		/** The view is not ready to be used, for instance immediately after the sort order 
		has changed, or after an error has occurred. */
		ENotReady
		};
public:
	IMPORT_C void OpenL(MContactViewObserver& aObserver);
	IMPORT_C TInt Open(MContactViewObserver& aObserver);
	IMPORT_C TBool Close(const MContactViewObserver& aObserver);
	/** Returns the contact item ID at the specified index into the view.
	
	@capability ReadUserData
	@param aIndex Index of the contact item ID into the view.
	@return The contact item ID. */
	virtual TContactItemId AtL(TInt aIndex) const=0; 
	/** Returns the contact item at the specified index into the view.
	
	@capability ReadUserData
	@param aIndex Index of the contact item into the view.
	@return The contact item. */
	virtual const CViewContact& ContactAtL(TInt aIndex) const =0;
	/** Returns the number of contact items in the view.
		
	@capability ReadUserData
	@return The number of contact items in the view. */
	virtual TInt CountL() const=0;
	/** Returns the index into the view of the specified contact item.
	
	@capability ReadUserData
	@param aId The contact item ID to search for.
	@return The index into the view of the contact item ID, or KErrNotFound if 
	no matching ID can be found. */
	virtual TInt FindL(TContactItemId aId) const=0;
	/** Returns a descriptor containing the contents of all fields for an item in the view.
	
	The field separator is used to separate the contents of each field. It is 
	not appended to the last field.
	
	@capability ReadUserData
	@param aIndex The index of the contact item into the view.
	@param aSeparator The string to use to separate the fields.
	@return Pointer to the contact item descriptor. */
	virtual HBufC* AllFieldsLC(TInt aIndex,const TDesC& aSeparator) const=0;
	IMPORT_C virtual void ContactsMatchingCriteriaL(const MDesCArray& aFindWords,RPointerArray<CViewContact>& aMatchedContacts);
	/** Gets the view preferences.
	
	@capability ReadUserData
	@return The view preferences. */
	virtual TContactViewPreferences ContactViewPreferences()=0;
	/** Gets the view's sort order.
	
	@capability ReadUserData
	@return The sort order. */
	virtual const RContactViewSortOrder& SortOrderL() const = 0;
	IMPORT_C virtual void ContactsMatchingPrefixL(const MDesCArray& aFindWords, RPointerArray<CViewContact>& aMatchedContacts);
	//This is a reserved virtual exported function that is used for BC proofing against present 
	//and future additions of new exported virtual functions. Existing exported virtual methods 
	//that broke BC are now non-virtual exported helper functions called from this method. 
	//All derived classes of CContactViewBase that are public must mandatorily 
	//implement this reserved exported virtual method.
	IMPORT_C virtual TAny* CContactViewBase_Reserved_1(TFunction aFunction,TAny* aParams);
	//New exported method to set the ECOM plug-in to be used
	IMPORT_C void SetViewFindConfigPlugin(TUid aUid);
	IMPORT_C TUid GetViewFindConfigPlugin();
	IMPORT_C virtual void GetContactIdsL(const CArrayFix<TInt>& aIndexes, CContactIdArray& aContactIds);
	IMPORT_C TInt Error() const; 
	IMPORT_C TUid GetViewSortPluginImplUid() const;

protected:
	IMPORT_C ~CContactViewBase();
	IMPORT_C void ConstructL();

	IMPORT_C CContactViewBase(const CContactDatabase& aDb);
	IMPORT_C void NotifyObservers(const TContactViewEvent& aEvent);
	IMPORT_C TInt NotifyObserverAsync(MContactViewObserver& aObserver,const TContactViewEvent& aEvent);
	IMPORT_C static TBool IdsMatch(const CViewContact& aFirst,const CViewContact& aSecond);
	IMPORT_C HBufC* FieldsWithSeparatorLC(const RPointerArray<CViewContact>& aContacts,TInt aIndex,const TDesC& aSeparator) const;
	//Find helper functions
	IMPORT_C virtual TBool MatchesCriteriaL(const CViewContact& aContact,const MDesCArray& aFindWords);
	IMPORT_C static TBool MatchesCriteriaL(const CViewContact& aContact,const MDesCArray& aFindWords, TSearchType aSearchType);
	IMPORT_C static TBool MatchesCriteriaL(const CViewContact& aContact,const MDesCArray& aFindWords, TSearchType aSearchType,CContactViewBase::CContactViewBaseExtension* aExtension);
	IMPORT_C static TBool IdsEqual(const TContactIdWithMapping& aFirst,const TContactIdWithMapping& aSecond);
	IMPORT_C static TBool IndexesEqual(const TContactIdWithMapping& aFirst,const TContactIdWithMapping& aSecond);
	IMPORT_C static TInt CompareFieldsL(const CViewContact& aFirst, const CViewContact& aSecond);
	// View Sort Plugin changes
	IMPORT_C static TInt CompareContactIds(const CViewContact& aFirst, const CViewContact& aSecond);
	IMPORT_C static TBool ContactIsSortable(const CViewContact& aContact);
	IMPORT_C TInt CompareContactsAndIdsL(const CViewContact& aFirst, const CViewContact& aSecond) const;
protected:	// these are needed by CContactLocalView only
	TInt InsertContactInView(RPointerArray<CViewContact>& aContacts, const CViewContact* aNewContact, TBool aSortByIdOnly, TInt* aIndex) const;
	TBool IsContactSortable(const CViewContact& aContact, TContactViewPreferences& aViewPreferences) const;
	CViewContactSortPlugin* SortPluginImpl() const;
	TUid FindSortPluginImplL (const TDesC8& aSortPluginName,TBool aWildCard=EFalse) const;
	TUid FindDefaultViewSortPluginImplL () const;
	void LoadViewSortPluginL (TUid aSortPluginUid,TContactViewPreferences& aViewPreferences);

private:
	static TInt AsyncNotifyCallBack(TAny* aSelf);
	static CDesCArrayFlat* CreateFindWordArrayLC(const MDesCArray& aFindWords, TSearchType aSearchType);
	static TBool ContactMatchesCriteriaL(const CViewContact& aContact,const MDesCArray& aFindWords,CContactViewBase::CContactViewBaseExtension* aExtension);
	void MatchContactsL(const MDesCArray& aFindWords,RPointerArray<CViewContact>& aMatchedContacts, TSearchType aSearchType);
	static TInt CompareFieldsWithCollationLevel(const CViewContact& aFirst, const CViewContact& aSecond, 
												TInt aCollationLevel, TCollationMethod* aCollateMethod);
	TInt GetErrorValueFromExtensionClass();	
	void GetContactsMatchingFilterL(TInt aFilter, RArray<TContactIdWithMapping>& aMatchingContacts);	
	TInt TextCompareFieldsL(const CViewContact& aFirst, const CViewContact& aSecond) const;


private:
	struct TObserverAndEvent
		{
		TContactViewEvent iAsyncEvent;
		MContactViewObserver* iObserverToNotify;
		};
protected:
	// Reference to CContactDatabase class.
	const CContactDatabase& iDb;
    // Different Contacts view states.
	TState iState;
	// The pointer to the ContactViewBase BC extension class.
	CContactViewBaseExtension* iExtension; 
private:
	RPointerArray<MContactViewObserver> iObserverArray;
	CIdle* iAsyncNotifier;
	RArray<TObserverAndEvent> iOutstandingNotifications;
	};

inline TInt CContactViewBase::GetErrorValueFromExtensionClass()
	{
	return iExtension->iError;
	}
	
class MLplPersistenceLayerFactory;
class CViewContactManager;

class CContactLocalView : public CContactViewBase, public MContactDbObserver
/** An instantiable base class for contact views.

The data associated with a local view is allocated within the client's memory 
space; CContactRemoteView should be used in preference if the view is likely 
to be shared between multiple clients. It is kept up to date by receiving 
change events from the underlying CContactDatabase object which it observes. 
The view preferences and sort order are specified on construction. 
@publishedAll
@released
*/
	{
public:
	IMPORT_C static CContactLocalView* NewL(MContactViewObserver& aObserver,const CContactDatabase& aDb,const RContactViewSortOrder& aSortOrder,TContactViewPreferences aContactTypes);
	IMPORT_C static CContactLocalView* NewL(MContactViewObserver& aObserver,const CContactDatabase& aDb,const RContactViewSortOrder& aSortOrder,TContactViewPreferences aContactTypes,
		const TDesC8& aSortPluginName);
	IMPORT_C static CContactLocalView* NewL(MContactViewObserver& aObserver,const CContactDatabase& aDb,const RContactViewSortOrder& aSortOrder,TContactViewPreferences aContactTypes,
		MLplPersistenceLayerFactory* aFactory,const TDesC8& aSortPluginName);

	IMPORT_C const RContactViewSortOrder& SortOrder() const;
public: // From CContactViewBase.
	TContactItemId AtL(TInt aIndex) const;
	TInt CountL() const;
	TInt FindL(TContactItemId aId) const;
	HBufC* AllFieldsLC(TInt aIndex,const TDesC& aSeparator) const;
	const CViewContact&  ContactAtL(TInt aIndex) const;
	TContactViewPreferences ContactViewPreferences();
	const RContactViewSortOrder& SortOrderL() const;
	void NotifySortError(TInt aError);
	//All derived classes of CContactViewBase that are public should mandatorily 
	//implement this reserved exported virtual method.
	IMPORT_C TAny* CContactViewBase_Reserved_1(TFunction aFunction,TAny* aParams);
protected:
	IMPORT_C ~CContactLocalView();
	IMPORT_C CContactLocalView(const CContactDatabase& aDb,TContactViewPreferences aContactTypes);
	CContactLocalView(const CContactDatabase& aDb,TContactViewPreferences aContactTypes,MLplPersistenceLayerFactory* aIterFactory);
	IMPORT_C void ConstructL(MContactViewObserver& aObserver,const RContactViewSortOrder& aSortOrder);
	void ConstructL(MContactViewObserver& aObserver,const RContactViewSortOrder& aSortOrder, const TBool aUseNamedPlugin, const TDesC8& aSortPluginName);
	IMPORT_C virtual void SortL(const RContactViewSortOrder& aSortOrder);
	IMPORT_C virtual TInt InsertL(TContactItemId aId);
	IMPORT_C virtual TInt RemoveL(TContactItemId aId);
protected:
	void SetState(TState aState);
private: // From MContactDbObserver.
	virtual void HandleDatabaseEventL(TContactDbObserverEvent aEvent);
	
private:
	void SortComplete(TInt aSortErr);	
	void SetSortOrderL(const RContactViewSortOrder& aSortOrder);
	void SortL();
	void SafeResort();
	friend class CViewContactManager;

private: // Metheds for event handling
	void HandleOutstandingEvents();
	void HandleOutstandingEventL();
	friend class CIdleContactSorter;
protected:
	/** The sort order. 
	This is set during construction or when SortL() is called. */
	RContactViewSortOrder iSortOrder ;
private:
	IMPORT_C virtual void CContactLocalView_Reserved_1();
	IMPORT_C virtual void CContactLocalView_Reserved_2();

private:
	MLplPersistenceLayerFactory*	iFactory;
	RPointerArray<CViewContact>	iContacts;
	RPointerArray<CViewContact>	iUnSortedContacts;
	RArray<TContactDbObserverEvent> iOutstandingEvents;
	CIdleContactSorter* iAsyncSorter;
	CViewContactManager* iViewCntMgr;
	CContactTextDef*	iTextDef;
	TContactViewPreferences iViewPreferences;
	TBool iSpare0;
	TInt iSpare;
	};

class RContactRemoteView : public RSubSessionBase
/** This class is used internally by remote contact views as the handle to the 
server side view object.

@see CContactRemoteViewBase 
@internalComponent
@released
*/
	{
public:
	void OpenL(const CContactDatabase& aDb,const RContactViewSortOrder& aSortOrder,TContactViewPreferences aContactTypes,const TUid& aSortPluginImplUid,const TDesC8& aSortPluginName);
	void OpenL(const CContactDatabase& aDb,const TDesC& aName,const RContactViewSortOrder& aSortOrder,TContactViewPreferences aContactTypes,const TUid& aSortPluginImplUid,const TDesC8& aSortPluginName);
	void Close();
public: // From CContactViewBase
	TContactItemId AtL(TInt aIndex) const;
	CViewContact*  ContactAtL(TInt aIndex);
	TInt CountL() const;
	TInt FindL(TContactItemId aId) const;
	HBufC* AllFieldsLC(TInt aIndex,const TDesC& aSeparator) const;
	const RContactViewSortOrder& SortOrderL();
public:	
	void ContactsMatchingCriteriaL(const MDesCArray& aFindWords,RPointerArray<CViewContact>& aMatchedContacts, TBool aPrefixSearch,TUid aUid);
	void ChangeSortOrderL(const RContactViewSortOrder& aSortOrder);
	void GetSortOrderL(RContactViewSortOrder& aSortOrder);
	TContactViewPreferences ContactViewPreferencesL();

	void RequestViewEvent(TPckgBuf<TContactViewEvent>& aEvent,TRequestStatus& aStatus);
	TInt CancelRequestViewEvent();
	void GetContactIdsL(const CArrayFix<TInt>& aIndexes, CContactIdArray& aContactIds);
	void GetContactsMatchingFilterL(TInt aFilter, RArray<TContactIdWithMapping>& aMatchingContacts);
	TUid GetViewSortPluginImplUidL() const;
private:
	HBufC8* PackageSortOrderLC(const RContactViewSortOrder& aSortOrder,TContactViewPreferences aContactTypes) const;
	HBufC8* PackageSortOrderAndPluginDetailsLC(const RContactViewSortOrder& aSortOrder,TContactViewPreferences aContactTypes,const TUid& aSortPluginImplUid,const TDesC8& aSortPluginName) const;
private:
	CViewContact* iContact;
	RContactViewSortOrder iSortOrder ;
	};


NONSHARABLE_CLASS(CContactRemoteViewBase) : public CContactViewBase
/** Base class for all remote contact view classes.

It implements all the pure virtual functions in CContactViewBase and additionally 
it handles remote view notifications. The data associated with remote views 
is allocated within the memory space of the contacts server. This means that 
multiple clients can share the same server side view object, thereby reducing 
the overhead associated with initial sorting of the view data and keeping 
it in sync with the underlying data. Remote views should therefore be used 
when the same view is likely to be needed by more than one client. 
@internalComponent
@released
*/
	{
public: // From CContactViewBase.
	TContactItemId AtL(TInt aIndex) const;
	TInt CountL() const;
	TInt FindL(TContactItemId aId) const;
	HBufC* AllFieldsLC(TInt aIndex,const TDesC& aSeparator) const;
	const CViewContact&  ContactAtL(TInt aIndex) const;
	TContactViewPreferences ContactViewPreferences();
	void ContactsMatchingCriteriaL(const MDesCArray& aFindWords,RPointerArray<CViewContact>& aMatchedContacts);
	void ContactsMatchingPrefixL(const MDesCArray& aFindWords, RPointerArray<CViewContact>& aMatchedContacts);
	const RContactViewSortOrder& SortOrderL() const;
	//All derived classes of CContactViewBase that are public should mandatorily 
	//implement this reserved exported virtual method.
	TAny* CContactViewBase_Reserved_1(TFunction aFunction,TAny* aParams);
	//This function was virtual in a previous release, if you still need to use it
	//in a virtual way then you can call it via the Reserved function.
	//The functionality of this function ,however,remains the same.
	void GetContactIdsL(const CArrayFix<TInt>& aIndexes, CContactIdArray& aContactIds);
	void GetContactsMatchingFilterL(TInt aFilter, RArray<TContactIdWithMapping>& aMatchingContacts);
protected:
	~CContactRemoteViewBase();
	CContactRemoteViewBase(const CContactDatabase& aDb);
	void ConstructL(MContactViewObserver& aObserver);
private:
	static TInt NotifierCallBack(TAny* aSelf);
	void HandleContactViewEvent(const TContactViewEvent& aEvent);
protected:
	/** A handle to the server side view. */
	RContactRemoteView iView;
private:
	CContactRemoteViewNotifier* iNotifier;
	/** Cached copy of the count of the local view in the contacts server. 
		If this is KCachedItemCountInvalid then next time CountL is called
		the current count is retrieved from the contacts server. */
	mutable TInt iCount;
	};


class CContactRemoteView : public CContactRemoteViewBase
/** An instantiable remote contact view class.

Remote views are associated with a CContactLocalView object held in the contacts 
server. They provide an efficient means for multiple clients to share the 
same underlying view.

The view preferences and sort order are specified on construction. 
@publishedAll
@released
*/
	{
public:
	IMPORT_C static CContactRemoteView* NewL(MContactViewObserver& aObserver,const CContactDatabase& aDb,const RContactViewSortOrder& aSortOrder,TContactViewPreferences aContactTypes);
	IMPORT_C static CContactRemoteView* NewL(MContactViewObserver& aObserver,const CContactDatabase& aDb,const RContactViewSortOrder& aSortOrder,TContactViewPreferences aContactTypes,
		const TDesC8& aSortPluginName);
	IMPORT_C void GetSortOrderL(RContactViewSortOrder& aSortOrder);
	//All derived classes of CContactViewBase that are public should mandatorily 
	//implement this reserved exported virtual method.
	TAny* CContactViewBase_Reserved_1(TFunction aFunction,TAny* aParams);
protected:
	~CContactRemoteView();
	CContactRemoteView(const CContactDatabase& aDb);
private:
	void ConstructL(MContactViewObserver& aObserver,const RContactViewSortOrder& aSortOrder,TContactViewPreferences aContactTypes);
	void ConstructL(MContactViewObserver& aObserver,const RContactViewSortOrder& aSortOrder,TContactViewPreferences aContactTypesconst, const TDesC8& aSortPluginName);
	};


class CContactNamedRemoteView : public CContactRemoteView
/** A named remote contact view.

This class enables multiple clients to share a named server side view. It 
also provides a function to change the sort order. If the sort order is changed, 
a notification is sent to all users of the named view. 
@publishedAll
@released
*/
	{
public:
	IMPORT_C static CContactNamedRemoteView* NewL(MContactViewObserver& aObserver,const TDesC& aName,const CContactDatabase& aDb,const RContactViewSortOrder& aSortOrder,TContactViewPreferences aContactTypes);
	IMPORT_C static CContactNamedRemoteView* NewL(MContactViewObserver& aObserver,const TDesC& aName,const CContactDatabase& aDb,const RContactViewSortOrder& aSortOrder,TContactViewPreferences aContactTypes,
		const TDesC8& aSortPluginName);
	IMPORT_C void ChangeSortOrderL(const RContactViewSortOrder& aSortOrder);
	//All derived classes of CContactViewBase that are public should mandatorily 
	//implement this reserved exported virtual method.
	TAny* CContactViewBase_Reserved_1(TFunction aFunction,TAny* aParams);
private:
	~CContactNamedRemoteView();
	CContactNamedRemoteView(const CContactDatabase& aDb);
	void ConstructL(MContactViewObserver& aObserver,const TDesC& aName,const RContactViewSortOrder& aSortOrder,TContactViewPreferences aContactTypes);
	void ConstructL(MContactViewObserver& aObserver,const TDesC& aName,const RContactViewSortOrder& aSortOrder,TContactViewPreferences aContactTypes, const TDesC8& aSortPluginName);
	};

inline TContactViewEvent::TContactViewEvent() 
/** Empty default constructor. */
	{};

inline TContactViewEvent::TContactViewEvent(TEventType aEventType,TInt aInt,TContactItemId aContactId) : iEventType(aEventType),iInt(aInt),iContactId(aContactId) 
/** Constructor with an event type and an optional error code and contact item ID.

@param aEventType The event type.
@param aInt Optional standard error code.
@param aContactId Optional contact item ID. */
	{};

inline void RContactViewSortOrder::AppendL(TFieldType aField) 
/** Appends a field type to the sort order object.

@param aField The field type to append. */
	{ User::LeaveIfError(iFields.Append(aField)); }

inline TFieldType RContactViewSortOrder::operator[](TInt aIndex) const 
/** Gets an indexed field type.

@param aIndex Index of the required field type. A panic occurs if this is 
invalid.
@return The field type located at the indexed position in the array. */
	{ return iFields[aIndex]; }

inline TInt RContactViewSortOrder::Count() const 
/** Gets the number of field types in the sort order array.

@return The number of field types in the array. */
	{ return iFields.Count(); }

inline TContactItemId CViewContact::Id() const 
/** Gets the view item's ID.

@return The view item's ID. */
	{return iId;}

inline CViewContact::TViewContactType CViewContact::ContactType() const 
/** Gets the view item's type.

@return The view item's type. */
	{return iContactType;}

inline void CViewContact::SetId(TContactItemId aId) 
/** Sets the view item's ID.

@param aId The contact item ID. */
	{iId=aId;}

inline void CViewContact::SetContactType(CViewContact::TViewContactType aContactType) 
/** Sets the view item's type.

@param aContactType The view item's type. */
	{iContactType=aContactType;}
#endif
