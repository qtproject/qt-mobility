/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Persistence layer exports
*
*/


#if !defined(__CNTITEM_H__)
#define __CNTITEM_H__

#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif

#if !defined(__CNTDEF_H__)
#include <cntdef.h>
#endif

#if !defined(__CNTFIELD_H__)
#include <cntfield.h>
#endif

#include <cntdb.h>

/** Maximum length of UID string.
@publishedAll
@released
*/
const TInt KUidStringLength =244;

/** UID string separator character
@publishedAll
@released
*/
#define KUidStringSeparator '-'

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
/** Constant used by vCard import/export.
@internalComponent
*/
#define KContactMaxFieldNumber 32
#endif

class CContactItemField;
class CContactItemViewDef;
class CContactItem;

/** Constant to indicate that all fields in the fieldset should be searched.
@publishedAll
@released
*/
const TInt KContactFieldSetSearchAll=-1;

class CContactItemFieldSet : public CBase
/** A contact item's field set. The field set owns an array of contact item fields
(CContactItemFields). The field set is owned by a contact item, and can be
retrieved using CContactItem::CardFields(). Use functions in class
CContactItem to add and remove fields to/from the field set.

A field set can contain more than one field of the same type, but this is
not advisable as it may cause problems when synchronising the contacts database.
@publishedAll
@released
*/
	{
	friend class CContactItemField;
	friend class CContactDatabase;
	friend class CContactTables;
	friend class RPplContactTable;
    friend class TCntPersistenceUtility;

public:
	IMPORT_C static CContactItemFieldSet* NewL();
	IMPORT_C static CContactItemFieldSet* NewLC();
	IMPORT_C ~CContactItemFieldSet();
	inline const CContactItemField& operator[](TInt aIndex) const;
	inline CContactItemField& operator[](TInt aIndex);
	inline TInt Find(TFieldType aFieldType) const; // will only find the first such occurence
	inline TInt Find(TFieldType aFieldType,TUid aMapping) const;
	IMPORT_C TInt FindNext(TFieldType aFieldType,TInt aStartPos=KContactFieldSetSearchAll) const;
	IMPORT_C TInt FindNext(TFieldType aFieldType,TUid aMapping,TInt aStartPos=KContactFieldSetSearchAll) const;
	inline TInt Count() const;
	inline void Reset();
	IMPORT_C void UpdateFieldL(const CContactItemField& aField, TInt aMatchCount);
	IMPORT_C void UpdateFieldSyncL(const CContactItemField& aField, TInt aMatchCount);
	IMPORT_C CContactItemFieldSet& AddL(CContactItemField& aField);
	IMPORT_C void Remove(TInt aIndex);
	IMPORT_C void InsertL(TInt aIndex,CContactItemField& aField);
	IMPORT_C void Move(TInt aFrom, TInt aTo);
	IMPORT_C TStreamId StoreL(CStreamStore& aStore,RWriteStream& aTextStream,CStreamStore& aBlobStore);
	TStreamId StoreL(CStreamStore& aStore,RWriteStream& aTextStream,CStreamStore& aBlobStore,CContactTables* aTables);
	IMPORT_C void RestoreL(CStreamStore& aStore, TStreamId anId,CStreamStore* aBlobStore,const CContactItemViewDef& aViewDef,RReadStream& aReadStream);
	IMPORT_C void RestoreL(CStreamStore& aStore, TStreamId anId,CStreamStore* aBlobStore,const CContactItemViewDef& aViewDef,const CContactItem* aTemplate, HBufC* aTextBuf);
	TInt FieldText(TFieldType aFieldType, TDes &aText, TInt aStartPosition) const;
	CArrayFix<TFieldHeader>* ConstructFieldHeaderArrayLC(RWriteStream& aTextStream, CStreamStore& aBlobStore);
	TInt FieldTypeCount(const CContactItemFieldSet& aSystemTemplateFields, TInt aStartIndex, const CContactItemField& aField) const;
	TBool ContainsFieldTypeMapping(const CContentType& aBaseFieldContentType, const TFieldType& aContactFieldType) const;
	void ExternalizeL(RWriteStream& aStream) const;
	void InternalizeL(RReadStream& aStream);

private:
	CContactItemFieldSet();
	void ConstructL();
	void SetFieldId(CContactItemField& aField);
	const CContactItemField* FindById(TInt aId) const;
	IMPORT_C TStreamId StoreL(CStreamStore& aStore,const CContactItem* aTemplate,RWriteStream& aStream,CStreamStore& aBlobStore,CContactTables* aTables);
	void RestoreL(CStreamStore& aStore, TStreamId anId, CStreamStore* aBlobStore,const CContactItemViewDef& aViewDef,const CContactItem* aTemplate,RReadStream& aReadStream,CContactTables* aContactsTable, RArray<TInt>* aEmailIdArray);
	void RestoreAndAddTemplateL(CStreamStore& aStore, TStreamId aId,CStreamStore* aBlobStore,const CContactItemViewDef& aViewDef,const CContactItem* aTemplate, RReadStream& aReadStream,CContactTables* aContactsTable,RArray<TInt>* aEmailIdArray);
	TInt MatchTemplateField(const CContentType& aContentType,TUint aUserFlags,TBool &aExactMatch) const;
	static HBufC* LoadTextStreamLC(RReadStream& aStream);
	void NonZeroFieldText(TFieldType aFieldType, TDes &aText) const;

private:
	CArrayPtr<CContactItemField>* iFields;
	};

class CContactDatabase;
class RCntModel;
class CContactItem : public CBase
/** The abstract base class for contact cards, templates and groups. All contact
items are identified by a contact ID, (TContactItemId), have a last modified
date/time and own one or more fields (the field set). Contact items also
have an access count and attributes (e.g. hidden). Note that fields in a contact
item also have attributes. Attribute values specified in the contact item
override those in the contained fields. The access count is a record of the
number of objects referencing a contact item. A contact item cannot be fully
deleted until its access count is zero.
@publishedAll
@released
*/
	{
	friend class CContactCardTemplate;
	friend class CContactItemPlusGroup;
	friend class CContactCard;
	friend class CContactTemplate;
	friend class CContactGroup;
	friend class CContactOwnCard;
	friend class CVCardToContactsAppConverter;
	friend class CContactICCEntry;
	friend class RPplContactTable;
	friend class RPplIdentityTable;
	friend class RPplPreferencesTable;
	friend class CPackagerTests; //Comparison test
	friend class CPackagerCntComparator;
	friend class CContactDatabase;
	friend class CPplContactItemManager;
public:
	IMPORT_C ~CContactItem();
	/** Gets the contact item's type.

	@return The contact item's type. */
	virtual TUid Type() const=0;
	static CContactItem* NewLC(RReadStream& aStream);
	IMPORT_C static CContactItem* NewLC(TUid aType);
	IMPORT_C TContactItemId Id() const;
	IMPORT_C TContactItemId TemplateRefId() const;
	IMPORT_C TTime LastModified() const;
	IMPORT_C void SetLastModified(const TTime& aLastModified);
	IMPORT_C void AddFieldL(CContactItemField& aField);
	IMPORT_C void RemoveField(TInt aFieldPos);
	IMPORT_C void InsertFieldL(CContactItemField& aField,TInt aFieldPos);
	IMPORT_C CContactItemFieldSet& CardFields() const;
	IMPORT_C void SetHidden(TBool aHidden);
	IMPORT_C void SetSystem(TBool aSystem);
	IMPORT_C TBool IsHidden();
	IMPORT_C TBool IsSystem();
	IMPORT_C void UpdateFieldSet(CContactItemFieldSet* aNewFieldSet);
	IMPORT_C void SetDeleted(TBool aDeleted);
	IMPORT_C TBool IsDeleted() const;
	IMPORT_C void SetTemplateRefId(TContactItemId aUid);
	TContactItemId Agent();
	inline TBool IsDeletable();
	inline void IncAccessCount();
	inline void DecAccessCount();
	inline TInt AccessCount() const;
	IMPORT_C void SetUidStringL(TDesC& aString);
	IMPORT_C TPtrC UidStringL(TInt64 aMachineUniqueId) const;
	TStreamId PopulateStoreL(CStreamStore& aStore, CArrayFix<TFieldHeader>& aFieldHeaderArray) const;
	void AddLabelFieldL();
	IMPORT_C void RestoreTemplateFieldsL(const CContactItemFieldSet& aSystemTemplateFields, const CContactItemFieldSet& aTemplateFields, const CContactItemViewDef& aViewDef);
	void ClearFieldContent();
	inline TPtrC Guid();
	virtual void ExternalizeL(RWriteStream& aStream) const;
	virtual void InternalizeL(RReadStream& aStream);

	IMPORT_C void SetId(TContactItemId aId);
	IMPORT_C TUint32 Attributes() const;
	IMPORT_C void SetAttributes(TUint32 aAttributes);
	IMPORT_C void SetAccessCount(TUint32 aAccessCount);
	IMPORT_C void SetCreationDate(const TTime& aTime);

public:
	/** Contact item's attribute flags

	These flags can be used to set the various attributes of a contact item. */
	enum
	{
	/** To set the contact item's system attribute. */
	ESystem=0x01,
	/** To set the contact item's hidden attribute. */
	EHidden=0x02,
	/** To set the contact item's hidden attribute. */
	ECompressedGuid=0x04,
	/** To set the contact item's Is deleted attribute. */
	EDeleted=0x08
	};

private:
	CContactItem();
	void ConstructL();
	void ConstructL(const CContactItem *aTemplate);
	void MakeUidStringL(TInt64 aMachineUniqueId);
	IMPORT_C void SetHasCompressedGuid(TBool aCompressed);
	TBool GuidIsCompressed() const;
	TInt NumberOfFieldsToStore() const;

private:
	CContactItemFieldSet* iFieldSet;
	TUint32 iAttributes;
	TContactItemId iId;
	TContactItemId iTemplateRefId;
	TTime iLastModified;
	TTime iCreationDate;
	TUint32 iAccessCount;
	HBufC* iGuid;
	};

class CContactItemPlusGroup : public CContactItem
/** Abstract base class for CContactGroup, CContactCard and CContactOwnCard.

The purpose of this class is to avoid duplication of group functionality
in its derived classes.
@publishedAll
@released
*/
	{
public:
	IMPORT_C const CContactIdArray* GroupsJoined() const;
	IMPORT_C CContactIdArray* GroupsJoinedLC() const;
protected:
	CContactItemPlusGroup();
	IMPORT_C ~CContactItemPlusGroup();
public:
	virtual void ExternalizeL(RWriteStream& aStream) const;
	virtual void InternalizeL(RReadStream& aStream);

	IMPORT_C void ResetGroups();
	IMPORT_C void SetGroups(CContactIdArray* aGroups);

private:
	CContactIdArray* iGroups;
	friend class CContactDatabase;
	friend class RPplLookupGroupsTable;
	friend class RPplGroupMembershipTable;
	};

class CContactGroup : public CContactItemPlusGroup
/** A contact group.

A contact group is a contact item which holds a set of associated contact
item IDs. The members of the group may be contact cards, own cards, or even
other groups. The group has a label which identifies the group to users, e.g.
"family", or "colleagues". The type of a contact group is KUidContactGroup,
as returned by Type().

Objects of this class are constructed using CContactDatabase::CreateContactGroupL()
or CreateContactGroupLC(). These functions create the group, optionally with
a label, add it to the database, and return a pointer to it.

To create an association between a card and a group, use CContactDatabase::AddContactToGroupL()
and to remove the association, use RemoveContactFromGroupL(). To find out
which groups a card belongs to, use CContactCard::GroupsJoinedLC() or CContactOwnCard::GroupsJoinedLC().

The function CContactDatabase::GetGroupIdListL() may be used to retrieve a
list of IDs for all groups in the database.
@publishedAll
@released
*/
	{
public:
	IMPORT_C static CContactGroup* NewL();
	IMPORT_C static CContactGroup* NewLC();
	IMPORT_C ~CContactGroup();
public: // from CContactItem
	IMPORT_C TUid Type() const;
public:
	// default sorted item array is ascending
	IMPORT_C CContactIdArray* ItemsContainedLC() const;
	IMPORT_C const CContactIdArray* ItemsContained() const;
	IMPORT_C TBool IsSystem() const;
	IMPORT_C void SetSystem(TBool aSystem);
	IMPORT_C TBool ContainsItem(TContactItemId aContactId);
	IMPORT_C void SetGroupLabelL(const TDesC& aLabel);
	IMPORT_C TPtrC GetGroupLabelL();
	IMPORT_C TBool HasItemLabelField();
	IMPORT_C CContactIdArray* GroupsJoinedLC() const;

	IMPORT_C void AddContactL(TContactItemId aContactId);
	IMPORT_C void RemoveContactL(TContactItemId aContactId);
	virtual void ExternalizeL(RWriteStream& aStream) const;
	virtual void InternalizeL(RReadStream& aStream);

	IMPORT_C void ResetItems();
	IMPORT_C void SetItems(CContactIdArray* aItems);

private:
	CContactGroup();

private:
	CContactIdArray* iItems;
	friend class CContactDatabase;
	friend class RPplGroupMembershipTable;
	friend class CLplAnalyserProxy;
	friend class RCntModel;
	
	//list of contacts belonging to a group (updated on open) 
	mutable CContactIdArray* iInitialContactIds;
	
	//contacts added to a group
	mutable CContactIdArray* iAddedContactIds;
	
	//contact removed from a group
    mutable CContactIdArray* iRemovedContactIds;
	};

class CContactCardTemplate : public CContactItem
/** A contact card template.

This is a contact item containing a set of fields on which new contact items can
be based. Templates have a label which is a string which identifies the template
to a user. For instance, 'work template' could indicate a template used
to create contact cards in the style of a work colleague. Contact card templates
have a type of KUidContactCardTemplate, as returned by Type().

Objects of this class cannot be constructed directly because its constructors
are protected. Instead, use either CContactDatabase::CreateContactCardTemplateL()
or CreateContactCardTemplateLC(). These functions create a contact card template,
add it to the database, and return a pointer to it.

The function CContactDatabase::GetCardTemplateIdListL() gets a list of the
IDs of all contact card templates in the database.
@publishedAll
@released
*/
	{
	friend class CContactDatabase;
	friend class CContactTables;
	friend class RPplContactTable;
	friend class CContactItem;
public:
	IMPORT_C void SetTemplateLabelL(const TDesC& aLabel);
	IMPORT_C TPtrC GetTemplateLabelL();
protected:
	IMPORT_C static CContactCardTemplate* NewL();
	IMPORT_C static CContactCardTemplate* NewLC();
	IMPORT_C static CContactCardTemplate* NewL(const CContactItem *aTemplate);
	IMPORT_C static CContactCardTemplate* NewLC(const CContactItem *aTemplate);
	IMPORT_C TBool HasItemLabelField();

/** Intended usage: Reserved to preserve future BC */
	IMPORT_C void ReservedFunction1();
/** Intended usage: Reserved to preserve future BC */
	IMPORT_C void ReservedFunction2();

protected: // from CContactItem
	IMPORT_C TUid Type() const;

private:
	CContactCardTemplate();
	};

class CContactCard : public CContactItemPlusGroup
/** A contact card.

Implements the Type() function declared in class CContactItem.
Contact cards may optionally be constructed from a template.
@publishedAll
@released
*/
	{
public:
	IMPORT_C ~CContactCard();
	IMPORT_C static CContactCard* NewL();
	IMPORT_C static CContactCard* NewLC();
	IMPORT_C static CContactCard* NewL(const CContactItem *aTemplate);
	IMPORT_C static CContactCard* NewLC(const CContactItem *aTemplate);
public: // from CContactItem
	IMPORT_C TUid Type() const;
public:
	IMPORT_C CContactIdArray* GroupsJoinedLC() const;
private:
	CContactCard();
private:
	friend class CContactDatabase;
	};

class CContactOwnCard : public CContactItemPlusGroup
/** Own card.

An own card is a contact card which contains information about the device's
owner. This can be sent to another compatible electronic device as a vCard.
The contact database recognises a single own card, referred to as the
current own card; its ID is returned by CContactDatabase::OwnCardId(). Like
a contact card, an own card can be a member of one or more contact card groups.
The own card type is identified by a UID of KUidContactOwnCard.

Own cards can be constructed using either CContactDatabase::CreateOwnCardLC()
or CreateOwnCardL(). These functions create an own card, based on the system
template, add it to the database, set it as the database's current own card
and return a pointer to it. To change the database's current own card, use
CContactDatabase::SetOwnCardL().
@publishedAll
@released
*/
	{
public:
	IMPORT_C ~CContactOwnCard();
	IMPORT_C static CContactOwnCard* NewL();
	IMPORT_C static CContactOwnCard* NewLC();
	IMPORT_C static CContactOwnCard* NewL(const CContactItem *aTemplate);
	IMPORT_C static CContactOwnCard* NewLC(const CContactItem *aTemplate);
public: // from CContactItem
	IMPORT_C TUid Type() const;
public:
	IMPORT_C CContactIdArray* GroupsJoinedLC() const;
private:
	CContactOwnCard();
private:
	friend class CContactDatabase;
	};

class CContactTemplate : public CContactItem
/** A contact item template.

This is a contact item which is used to seed the initial field set for
other contact items.

Non-system (i.e. user-defined) templates are implemented
by the CContactCardTemplate class. CContactCardTemplate should be
used in preference to CContactTemplate.
@publishedAll
@released
*/
	{
public:
	IMPORT_C static CContactTemplate* NewL();
	IMPORT_C static CContactTemplate* NewLC();
	IMPORT_C static CContactTemplate* NewL(const CContactItem *aTemplate);
	IMPORT_C static CContactTemplate* NewLC(const CContactItem *aTemplate);
public: // from CContactItem
	IMPORT_C TUid Type() const;
private:
	CContactTemplate();
	};


class CContactICCEntry : public CContactItemPlusGroup
/** A contact ICC entry.
@publishedAll
@released
*/
	{
public:
	IMPORT_C ~CContactICCEntry();
	IMPORT_C static CContactICCEntry* NewL(const CContactItem& aTemplate);
	static CContactICCEntry* NewL();
public: //from CContactItem
	TUid Type() const;
private:
	CContactICCEntry();
	};


class ContactGuid
/** A globally unique identifier enquiry utility.

Each contact item has a unique identifier, stored as a descriptor. It is
referred to as the 'UID string'. This is a combination of the unique
identifier of the database in which the contact item was created, the
contact item ID and the date/time of the contact item's creation. ContactGuid
provides a single static exported function to enquire whether an item was
created in a specified database.
@publishedAll
@released
*/
	{
public:
	IMPORT_C static TContactItemId IsLocalContactUidString(const TDesC& aString, TInt64 aMachineUniqueId);
	IMPORT_C static TBool GetCreationDate(TDes& aString, TInt64 aMachineUniqueId);
	static HBufC* CreateGuidLC(const TDesC& aCreationDate,TContactItemId aId, TInt64 aMachineUniqueId);
	static HBufC* CreateGuidLC(const TTime& aCreationDate,TContactItemId aId, TInt64 aMachineUniqueId);
	static HBufC* CreateGuidLC(TContactItemId aId, TInt64 aMachineUniqueId);
	};

inline const CContactItemField& CContactItemFieldSet::operator[](TInt aIndex) const
/** Gets the field located at a specified position in the field set.

@param aIndex The position of the field in the field set. This is relative to zero.
It must be non-negative and less than the number of objects in the array, otherwise
the operator raises a panic.

@return A const reference to an element in the array. */
	{ return *(*iFields)[aIndex]; }

inline CContactItemField& CContactItemFieldSet::operator[](TInt aIndex)
/** Gets the field located at a specified position in the field set.

@param aIndex The position of the field in the field set. This is relative to zero.
It must be non-negative and less than the number of objects in the array, otherwise
the operator raises a panic.

@return A non-const reference to an element in the array. */
	{ return *(*iFields)[aIndex]; }

inline TInt CContactItemFieldSet::Find(TFieldType aFieldType) const
/** Finds the first field in the field set with the specified field type.

@param aFieldType The field type of interest.
@return If found, the index of the field within the field set, or KErrNotFound
if not found. */
	{ return FindNext(aFieldType,KContactFieldSetSearchAll); }

inline TInt CContactItemFieldSet::Find(TFieldType aFieldType,TUid aMapping) const
/** Finds the first field in the field set containing both the content type mapping
and the field type specified.

@param aFieldType The field type of interest.
@param aMapping The content type mapping of interest.
@return If found, the index of the field within the field set, or KErrNotFound
if not found. */
	{ return FindNext(aFieldType,aMapping,KContactFieldSetSearchAll); }

inline TInt CContactItemFieldSet::Count() const
/** Gets the number of fields in the field set.

@return The number of fields in the field set. */
	{ return iFields->Count(); }

inline void CContactItemFieldSet::Reset()
/** Deletes all fields in the field set. */
	{ iFields->ResetAndDestroy(); }

inline TBool CContactItem::IsDeletable()
/** Tests whether the contact item is deletable.

This is true if the item's access count is zero.

@return ETrue if deletable, EFalse if not deletable. */
	{return (iAccessCount == 0);}

inline void CContactItem::IncAccessCount()
/** Increments the contact item's access count. */
	{iAccessCount++;}

inline void CContactItem::DecAccessCount()
/** Decrements the contact item's access count. */
	{if(iAccessCount) iAccessCount--;}

inline TInt CContactItem::AccessCount() const
/** Gets the contact item's access count.

@return The item's access count. */
	{return(iAccessCount);}

inline TPtrC CContactItem::Guid()
/** Accessor function for Contact Guid.
 * This is used to cache contact items that are added during a sync.
 @return Guid    */
 { return iGuid ? *iGuid : TPtrC(KNullDesC); }

#endif
