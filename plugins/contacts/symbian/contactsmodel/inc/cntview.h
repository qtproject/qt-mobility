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


#ifndef __CNTVIEW_H__
#define __CNTVIEW_H__

#include <cntviewbase.h>

/** The maximum length of the string used by contact sub view ranges to find the 
item at the upper or lower limit of the range.

In practice, only the first character in the string is used and this value 
is ignored. 
@publishedAll
@released
*/
const TInt KMaxBoundaryMatchLength=8;

class CContactGroupView: public CContactViewBase, public MContactViewObserver
/** Provides a view of a group of contact items.

The view uses an underlying view. For items to appear in the group view, they 
must exist in the underlying view.

The TGroupType enumeration defines whether the view displays items that belong 
to the group, items that do not belong to the group, or items that do not 
belong to any group.

The group view observes its underlying view so that it is kept up to date 
if the contents of the underlying view change.

The group can be identified by a contact item ID or by a label, e.g. "family", 
or "colleagues".

@see CContactGroup 
@publishedAll
@released
*/
	{
friend class CGroupViewTester;
public:
	/** Defines the types of contact group view. */
	enum TGroupType
		{
		/** The view contains contact items in the group. */
		EShowContactsInGroup,
		/** The view contains contact items not in the group. */
		EShowContactsNotInGroup,
		/** The view contains contact items not in any group. */
		EShowContactsNotInAnyGroup
		};
public:
	IMPORT_C static CContactGroupView* NewL(const CContactDatabase& aDb,CContactViewBase& aView,MContactViewObserver& aObserver,const TContactItemId aGroupId,const TGroupType aGroupType);
	IMPORT_C static CContactGroupView* NewL(const CContactDatabase& aDb,CContactViewBase& aView,MContactViewObserver& aObserver, const TDesC& aGroupName,const TGroupType aGroupType);

	// Factory constructor for version 2 behaviour
	IMPORT_C static CContactGroupView* NewL(CContactViewBase& aView,const CContactDatabase& aDb, MContactViewObserver& aObserver,const TContactItemId aGroupId,const TGroupType aGroupType);
	// Factory constructor for version 2 behaviour
	IMPORT_C static CContactGroupView* NewL(CContactViewBase& aView,const CContactDatabase& aDb, MContactViewObserver& aObserver, const TDesC& aGroupName,const TGroupType aGroupType);	
	//All CContactViewBase derived classes in public headers,
	//should mandatorily implement this reserved function.	
	TAny* CContactViewBase_Reserved_1(TFunction aFunction,TAny* aParams);
	~CContactGroupView();
public: // From CContactViewBase
	TContactItemId AtL(TInt aIndex) const;
	TInt CountL() const;
	TInt FindL(TContactItemId aId) const;
	HBufC* AllFieldsLC(TInt aIndex,const TDesC& aSeparator) const;
	const CViewContact& ContactAtL(TInt aIndex) const;
	TContactViewPreferences ContactViewPreferences();
	const RContactViewSortOrder& SortOrderL() const;
protected:
	IMPORT_C virtual void UpdateGroupViewL();
private: // From MContactViewObserver.
	void HandleContactViewEvent(const CContactViewBase& aView,const TContactViewEvent& aEvent);
private: 
	void ConstructL(MContactViewObserver& aObserver,TContactItemId aGroupId);
	void UpdateForContactsInGroupL();
	void UpdateForContactsNotInThisGroupL();
	void UpdateForUnfiledContactsL();
	void UpdateForContactListL(const CContactIdArray* aArray);
	TContactItemId GetGroupIdbyNameL(const TDesC& aGroupName);
	TBool HandleRemoveEventL(TContactViewEvent& aEvent);
	TBool HandleAddEventL(TContactViewEvent& aEvent);
	TBool HandleAddEventForUnfiledContactsL(const TContactViewEvent& aEvent );
	static TInt CompareMappingsL(const TContactIdWithMapping& aFirst,const TContactIdWithMapping& aSecond);
	TInt static CompareIndexesAllowingDuplicates(const TContactIdWithMapping& aFirst, const TContactIdWithMapping& aSecond);
private:
	CContactGroupView(const CContactDatabase& aDb,CContactViewBase& aView,TGroupType aGroupType);
	void UpdateMappingsL();
	void NotifyRemovedMembersL(const CContactIdArray* aArray);
private:
	RArray<TContactIdWithMapping> iGroupContacts;
	TContactItemId iGroupId;
	CContactViewBase& iView;
	TGroupType iGroupType;
	TUint iClassVersion; //version class - used for TContactViewEvent dispatching
	};


class CContactFindView: public CContactViewBase, public MContactViewObserver
/** Provides a view of an existing CContactViewBase-derived object, populated using 
search criteria.

The find view only contains contact items from the underlying view that match 
the search criteria.

The find view observes its underlying view so that it is kept up to date if 
the contents of the underlying view change.

The search criteria (one or more words) are held in an MDesCArray object. 
The fields involved in the search are those that have been used to sort the 
underlying view. 
@publishedAll
@released
*/
	{
public:
	IMPORT_C static CContactFindView* NewL(const CContactDatabase& aDb,CContactViewBase& aView,MContactViewObserver& aObserver,MDesCArray* aFindWords);
	IMPORT_C static CContactFindView* NewL(const CContactDatabase& aDb,CContactViewBase& aView,MContactViewObserver& aObserver,MDesCArray* aFindWords, TSearchType aSearchType);
	
	// Factory constructor for version 2 behaviour
	IMPORT_C static CContactFindView* NewL(CContactViewBase& aView,const CContactDatabase& aDb,MContactViewObserver& aObserver,MDesCArray* aFindWords);
	// Factory constructor for version 2 behaviour
	IMPORT_C static CContactFindView* NewL(CContactViewBase& aView,const CContactDatabase& aDb,MContactViewObserver& aObserver,MDesCArray* aFindWords, TSearchType aSearchType);

	~CContactFindView();
public: // From CContactViewBase
	TContactItemId AtL(TInt aIndex) const;
	TInt CountL() const;
	TInt FindL(TContactItemId aId) const;
	HBufC* AllFieldsLC(TInt aIndex,const TDesC& aSeparator) const;
	const CViewContact& ContactAtL(TInt aIndex) const;
	TContactViewPreferences ContactViewPreferences();
	const RContactViewSortOrder& SortOrderL() const;
protected:
	IMPORT_C virtual void UpdateFindViewL();
public:
	IMPORT_C void RefineFindViewL(MDesCArray* aFindWords);
	//All CContactViewBase derived classes in public headers,
	//should mandatorily implement this reserved function.	
	TAny* CContactViewBase_Reserved_1(TFunction aFunction,TAny* aParams);
private: // From MContactViewObserver.
	virtual void HandleContactViewEvent(const CContactViewBase& aView,const TContactViewEvent& aEvent);
private: 
	void ConstructL(MContactViewObserver& aObserver,MDesCArray* aFindWords);
	void DeleteFindContacts();
	void TightenFindViewCriteriaL();
	void HandleRemoveEventL(TContactViewEvent& aEvent);
	void HandleAddEventL(TContactViewEvent& aEvent);
	void CopyNewFindWordsL(MDesCArray* aFindWords);
	void DestroyFindWordsArray();
	TInt InsertContact(const CViewContact* aNewContact);
private:
	CContactFindView(const CContactDatabase& aDb,CContactViewBase& aView, TSearchType aSearchType);
private:
	RPointerArray<CViewContact> iFindContacts;
	CPtrCArray* iFindWords;
	CContactViewBase& iView;
	const TSearchType iSearchType;
	TUint iClassVersion; //version class - used for TContactViewEvent dispatching
	};


class CContactFilteredView : public CContactViewBase, public MContactViewObserver
/** Provides a filtered view over an existing CContactViewBase derived object.

For example, it might provide a view containing only contacts that have an 
email address. For efficiency reasons there are a limited set of filters available 
(defined in CContactDatabase::TContactViewFilter).

The filtered view observes its underlying view so that it is kept up to date 
if the contents of the underlying view change. It will, in turn, notify its 
observers.

Filters only apply to non-hidden contact fields with content. 
@publishedAll
@released
*/
	{
public:
	IMPORT_C static CContactFilteredView* NewL(MContactViewObserver& aObserver,const CContactDatabase& aDb,CContactViewBase& aView,TInt aFilter);
	// Factory constructor for version 2 behaviour
	IMPORT_C static CContactFilteredView* NewL(CContactViewBase& aView,const CContactDatabase& aDb,MContactViewObserver& aObserver,TInt aFilter);

	//All CContactViewBase derived classes in public headers,
	//should mandatorily implement this reserved function.	
	TAny* CContactViewBase_Reserved_1(TFunction aFunction,TAny* aParams);
	~CContactFilteredView();
public: // From CContactViewBase.
	TContactItemId AtL(TInt aIndex) const;
	TInt CountL() const;
	TInt FindL(TContactItemId aId) const;
	HBufC* AllFieldsLC(TInt aIndex,const TDesC& aSeparator) const;
	const CViewContact& ContactAtL(TInt aIndex) const;
	TContactViewPreferences ContactViewPreferences();
	const RContactViewSortOrder& SortOrderL() const;
	virtual void ContactsMatchingCriteriaL(const MDesCArray& aFindWords,RPointerArray<CViewContact>& aMatchedContacts);
	virtual void ContactsMatchingPrefixL(const MDesCArray& aFindWords, RPointerArray<CViewContact>& aMatchedContacts);
private: // From MContactViewObserver.
	virtual void HandleContactViewEvent(const CContactViewBase& aView,const TContactViewEvent& aEvent);
private:
	CContactFilteredView(const CContactDatabase& aDb,TInt aFilter,CContactViewBase& aView);
	void ConstructL(MContactViewObserver& aObserver);
	void CreateFilteredIdArray();
	void UpdateFilteredIdArrayL(TContactViewEvent& aEvent, TBool& aViewModified);
	void HandleAddEventL(TContactViewEvent& aEvent, TBool& aViewModified);
	void HandleRemoveEventL(TContactViewEvent& aEvent, TBool& aViewModified);
	TInt BinaryInsertL(TContactIdWithMapping aId);
	static TInt FilterCallBack(TAny* aSelf);
	static TBool CompareMappings(const TContactIdWithMapping& aFirst,const TContactIdWithMapping& aSecond);
	void FilterResultsArray(RPointerArray<CViewContact>& aMatchedContacts);
	TInt DoFilterIncrementL();
	void UpdateMappingsL();
private:
	const TInt iFilter;
	CContactViewBase& iView;
	RArray<TContactIdWithMapping> iFilteredIdArray;
	TUint iClassVersion; //version class - used for TContactViewEvent dispatching
	};


class CContactViewRangeBase : public CBase
/** The abstract base class for all contact view range classes.

Range classes are used by contact sub views to specify the upper and lower 
range boundaries and for searching their underlying view.

@see CContactSubView 
@publishedAll
@released
*/
	{
public:
	/** Defines the range criteria. */
	enum TCriteria
		{
		/** Less than. */
		ELessThan,
		/** Less than or equal to. */
		ELessThanOrEqualTo,
		/** Greater than. */
		EGreaterThan,
		/** Greater than or equal to. */
		EGreaterThanOrEqualTo
		};
public:
	~CContactViewRangeBase();
	inline TInt LowIndex() const;
	inline TInt HighIndex() const;
	/** Sets the iLow and iHigh members. */
	virtual void SetL()=0;
	IMPORT_C TBool IndicesValid() const;
protected:
	CContactViewRangeBase(const CContactViewBase& aView);
	void ConstructL();
	TInt FindIndexL(const TDesC& aMatch,TCriteria aCriteria) const;
	TBool MatchesCriteriaL(TCriteria aCriteria,const TDesC& aMatch,TInt aIndex) const;
	void ValidateIndices();
protected:
	/** The sub view's underlying view. */
	const CContactViewBase& iView;
	/** The index into the sub view's underlying view of the item at the lower limit 
	of the range. */
	TInt iLow;
	/** The index into the sub view's underlying view of the item at the upper limit 
	of the range. */
	TInt iHigh;
	/** The collation method used to sort the strings.
	
	By default, this is the standard collation method for the current locale. */
	TCollationMethod* iCollateMethod;
	};


class CContactViewRange : public CContactViewRangeBase
/** A sub view range with both a lower and upper limit.

This is used by sub views when lower and upper boundary criteria are specified.

@see CContactSubView 
@publishedAll
@released
*/
	{
public:
	~CContactViewRange();
	IMPORT_C static CContactViewRange* NewL(const CContactViewBase& aView,const TDesC& aLowMatch,TCriteria aLowCriteria,const TDesC& aHighMatch,TCriteria aHighCriteria);
private: // From CContactViewRangeBase.
	void SetL();
private:
	CContactViewRange(const CContactViewBase& aView,const TDesC& aLowMatch,TCriteria aLowCriteria,const TDesC& aHighMatch,TCriteria aHighCriteria);
	void ConstructL();
private:
	TBuf<KMaxBoundaryMatchLength> iLowMatch;
	TCriteria iLowCriteria;
	TBuf<KMaxBoundaryMatchLength> iHighMatch;
	TCriteria iHighCriteria;
	};


class CContactViewLowRange : public CContactViewRangeBase
/** A sub view range with an upper limit only.

This is used by sub views when the range criteria are CContactViewRangeBase::ELessThan 
or CContactViewRangeBase::ELessThanOrEqualTo.

@see CContactSubView 
@publishedAll
@released
*/
	{
public:
	IMPORT_C static CContactViewLowRange* NewL(const CContactViewBase& aView,const TDesC& aMatch,TCriteria aCriteria);
private: // From CContactViewRangeBase.
	void SetL();
private:
	CContactViewLowRange(const CContactViewBase& aView,const TDesC& aMatch,TCriteria aCriteria);
	~CContactViewLowRange();
private:
	TBuf<KMaxBoundaryMatchLength> iMatch;
	TCriteria iCriteria;
	};


class CContactViewHighRange : public CContactViewRangeBase
/** A sub view range with a lower limit only.

This is used by sub views when the range criteria are CContactViewRangeBase::EGreaterThan 
or CContactViewRangeBase::EGreaterThanOrEqualTo.

@see CContactSubView 
@publishedAll
@released
*/
	{
public:
	IMPORT_C static CContactViewHighRange* NewL(const CContactViewBase& aView,const TDesC& aMatch,TCriteria aCriteria);
private: // From CContactViewRangeBase.
	void SetL();
private:
	CContactViewHighRange(const CContactViewBase& aView,const TDesC& aMatch,TCriteria aCriteria);
	~CContactViewHighRange();
private:
	TBuf<KMaxBoundaryMatchLength> iMatch;
	TCriteria iCriteria;
	};


class CContactSubView : public CContactViewBase, public MContactViewObserver
/** Provides a view of a range of contact items from a potentially larger underlying 
view (another CContactViewBase-derived object).

All contact items between a lower and upper boundary in the underlying view 
are included in the sub view. For example, the sub view might contain all 
items whose name begins with a character between 'd' and 'f'.

The sub view observes its underlying view so that as the underlying view changes, 
the sub view is kept up to date. 
@publishedAll
@released
*/
	{
public:
	IMPORT_C static CContactSubView* NewL(MContactViewObserver& aObserver,const CContactDatabase& aDb,CContactViewBase& aView,const TDesC& aBoundary);//>= > is infinity <= < =0;
	IMPORT_C static CContactSubView* NewL(MContactViewObserver& aObserver,const CContactDatabase& aDb,CContactViewBase& aView,const TDesC& aLowBoundary,const TDesC& aHighBoundary);
	
	// Factory constructor for version 2 behaviour
	IMPORT_C static CContactSubView* NewL(CContactViewBase& aView,const CContactDatabase& aDb,MContactViewObserver& aObserver,const TDesC& aBoundary);//>= > is infinity <= < =0;
	// Factory constructor for version 2 behaviour
	IMPORT_C static CContactSubView* NewL(CContactViewBase& aView,const CContactDatabase& aDb,MContactViewObserver& aObserver,const TDesC& aLowBoundary,const TDesC& aHighBoundary);

	//All CContactViewBase derived classes in public headers,
	//should mandatorily implement this reserved function.	
	TAny* CContactViewBase_Reserved_1(TFunction aFunction,TAny* aParams);
public: // From CContactViewBase.
	TContactItemId AtL(TInt aIndex) const;
	TInt CountL() const;
	TInt FindL(TContactItemId aId) const;
	HBufC* AllFieldsLC(TInt aIndex,const TDesC& aSeparator) const;
	const CViewContact& ContactAtL(TInt aIndex) const;
	TContactViewPreferences ContactViewPreferences();
	const RContactViewSortOrder& SortOrderL() const;
private: // From MContactViewObserver.
	virtual void HandleContactViewEvent(const CContactViewBase& aView,const TContactViewEvent& aEvent);
private:
	~CContactSubView();
	CContactSubView(const CContactDatabase& aDb,CContactViewBase& aView);
	CContactSubView(const CContactDatabase& aDb,const CContactSubView& aView);
	void CommonConstructL(MContactViewObserver& aObserver);
	void ConstructL(MContactViewObserver& aObserver,const TDesC& aBoundary);
	void ConstructL(MContactViewObserver& aObserver,const CContactSubView& aView);
	void ConstructL(MContactViewObserver& aObserver,const CContactSubView& aView,const TDesC& aBoundary);
	void ConstructL(MContactViewObserver& aObserver,const TDesC& aLowBoundary,const TDesC& aHighBoundary);
	CContactViewRangeBase::TCriteria DecodeBoundary(TDes& aBoundary) const;
	TInt MapToUnderlyingViewIndex(TInt aSubViewIndex) const;
	TInt MapToSubViewIndex(TInt aUnderlyingViewIndex) const;
private:
	CContactViewBase& iView;
	CContactViewRangeBase* iRange;
	TUint iClassVersion; //version class - used for TContactViewEvent dispatching
	};


class CContactConcatenatedView : public CContactViewBase, public MContactViewObserver
/** Concatenates multiple contact views into a single contiguous view.

The component views are ordered according to their order in the array passed 
into the NewL(). 
@publishedAll
@released
*/
	{
public:
	IMPORT_C static CContactConcatenatedView* NewL(MContactViewObserver& aObserver,const CContactDatabase& aDb,RPointerArray<CContactViewBase>& aComponentViews);
	//All CContactViewBase derived classes in public headers,
	//should mandatorily implement this reserved function.	
	TAny* CContactViewBase_Reserved_1(TFunction aFunction,TAny* aParams);
public: // From CContactViewBase.
	TContactItemId AtL(TInt aIndex) const;
	TInt CountL() const;
	TInt FindL(TContactItemId aId) const;
	HBufC* AllFieldsLC(TInt aIndex,const TDesC& aSeparator) const;
	const CViewContact& ContactAtL(TInt aIndex) const;
	TContactViewPreferences ContactViewPreferences();
	const RContactViewSortOrder& SortOrderL() const;
private: // From MContactViewObserver.
	virtual void HandleContactViewEvent(const CContactViewBase& aView,const TContactViewEvent& aEvent);
private:
	struct TComponentView
		{
		TComponentView(CContactViewBase& aView,TState aState);
		CContactViewBase& iView;
		TState iState;
		};
private:
	~CContactConcatenatedView();
	CContactConcatenatedView(const CContactDatabase& aDb);
	void ConstructL(MContactViewObserver& aObserver,RPointerArray<CContactViewBase>& aComponentViews);
	void CopyComponentViewsL(RPointerArray<CContactViewBase>& aComponentViews);
	void OpenComponentViewsL();
	void CloseComponentViews();
	TBool ComponentViewsReady();
	void SetComponentViewReady(const CContactViewBase& aView);
	TInt OffsetL(const CContactViewBase& aView);
	static TBool ComponentViewsEqual(const TComponentView& aFirst,const TComponentView& aSecond);
	TInt FindComponentView(const CContactViewBase& aView);
private:
	RArray<TComponentView> iComponentView;
	};


inline TInt CContactViewRangeBase::LowIndex() const 
/** Gets the index into the sub view's underlying view of the item at the bottom 
of the range.

@return The index of the item at the bottom of the range. */
	{ return iLow; }

inline TInt CContactViewRangeBase::HighIndex() const 
/** Gets the index into the sub view's underlying view of the item at the top of 
the range.
	
@return The index of the item at the top of the range. */
	{ return iHigh; }

#endif
