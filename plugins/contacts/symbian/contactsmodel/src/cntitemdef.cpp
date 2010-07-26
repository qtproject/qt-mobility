/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <cntfield.h>
#include <cntitem.h>
#include "cntstd.h"

//
// TContactTextDefItem
//

EXPORT_C TContactTextDefItem::TContactTextDefItem() : iFieldType(KNullUid)
/** Default constructor; initialises the field type to KNullUid. */
	{}

EXPORT_C TContactTextDefItem::TContactTextDefItem(TFieldType aFieldType) : iFieldType(aFieldType)
/** Constructs the text definition item with a field type. 

@param aFieldType The field type. */	
	{}

EXPORT_C TContactTextDefItem::TContactTextDefItem(TFieldType aFieldType, const TDesC &aSeperator) : iFieldType(aFieldType)
/** C++ constructor with a field type and a separator string. 

@param aFieldType The field type.
@param aSeperator The string used to separate fields in the text definition. */
	{
	iSeperator.Copy(aSeperator);
	}

//
// CContactTextDef
//

CContactTextDef::CContactTextDef()
	: CArrayFixFlat<TContactTextDefItem>(5),
	iFallbackFieldType(KUidContactFieldNone),
	iExactMatchOnly(EFalse)
	{}

EXPORT_C CContactTextDef* CContactTextDef::NewL()
/** Allocates and constructs a new text definition. The fallback field type is 
initialised to KUidContactFieldNone.

@return Pointer to the newly created text definition. */
	{
	return new(ELeave) CContactTextDef;
	}

EXPORT_C CContactTextDef* CContactTextDef::NewLC()
/** Allocates and constructs a new text definition. If the text definition is successfully 
constructed, it is left on the cleanup stack. The fallback field type is initialised 
to KUidContactFieldNone.

@return Pointer to the newly created text definition. */
	{
	CContactTextDef* textDef=CContactTextDef::NewL();
	CleanupStack::PushL(textDef);
	return textDef;
	}

/** Allocates and constructs a new text definition based on a RReadStream. If the text definition is successfully 
constructed, it is left on the cleanup stack. The fallback field type is initialised 
to KUidContactFieldNone.

@param aStream RReadStream containing object to internalize.
@return Pointer to the newly created text definition. 
@internalTechnology 
*/	
CContactTextDef* CContactTextDef::NewLC(RReadStream& aStream)
	{
	CContactTextDef* textDef=CContactTextDef::NewLC();
	textDef->InternalizeL(aStream);	
	return textDef;
	}	

EXPORT_C void CContactTextDef::SetFallbackField(TFieldType aFieldType)
/** Sets the fallback field type.

@param aFieldType The fallback field type. */
	{
	iFallbackFieldType=aFieldType;
	}

EXPORT_C TFieldType CContactTextDef::FallbackField() const
/** Gets the fallback field type.

@return The fallback field type. */
	{
	return(iFallbackFieldType);
	}

EXPORT_C TBool CContactTextDef::ExactMatchOnly()
/** Gets the exact match for contact fields
@return ETrue if it gets the exact match   */
	{
	return iExactMatchOnly;
	}

EXPORT_C void CContactTextDef::SetExactMatchOnly(TBool aExactMatchOnly)
/** Sets the exact match for contact fields
@param aExactMatchOnly Contains the value for exact match. It takes ETrue when an exact match is required, EFalse otherwise.   */
	{
	iExactMatchOnly=aExactMatchOnly;
	}

void CContactTextDef::InternalizeL(RReadStream& aStream)
/** Internalises a CContactItem object from a read stream. 
@param aStream Stream from which the object should be internalised. */
	{
	this->Reset();
	TInt count = aStream.ReadInt32L();

	TInt separatorLength = 0;
	for (TInt i=0; i<count; ++i)
		{
		TContactTextDefItem textDefItem;
		textDefItem.iFieldType.iUid = aStream.ReadInt32L();
		separatorLength = aStream.ReadInt32L();
		if(separatorLength)
			{
			aStream>>textDefItem.iSeperator;
			}

		this->AppendL(textDefItem);
		}
	
	TFieldType fieldType;
	fieldType.iUid = aStream.ReadInt32L();
	SetFallbackField(fieldType);
	SetExactMatchOnly(aStream.ReadInt32L());
		
	}

void CContactTextDef::ExternalizeL(RWriteStream& aStream) const 
/** Externalises a CContactItem object to a write stream.
@param aStream Stream to which the object should be externalised. */
	{
	TInt count = this->Count();
	aStream.WriteInt32L(count);	
	
	TInt separatorLength = 0;
	for(TInt i=0; i<count;++i)
		{
		aStream.WriteInt32L(this->At(i).iFieldType.iUid);
		separatorLength = this->At(i).iSeperator.Length();
		aStream.WriteInt32L(separatorLength);
		if(separatorLength)
			{
			aStream << this->At(i).iSeperator;
			}			
		}
	aStream.WriteInt32L(this->FallbackField().iUid);	
	aStream.WriteInt32L(const_cast<CContactTextDef*>(this)->ExactMatchOnly());
		
	}

EXPORT_C CContactItemViewDef* CContactItemViewDef::NewL(TUse aUse, TMode aMode)
/** Allocates and constructs a new CContactItemViewDef, with a use and a mode.

@param aUse Specifies whether to include or exclude specified fields.
@param aMode Specifies whether to include or exclude hidden fields.
@return Pointer to the newly created view definition. */
	{ // static
	return new(ELeave) CContactItemViewDef(aUse, aMode);
	}

EXPORT_C CContactItemViewDef* CContactItemViewDef::NewLC(TUse aUse, TMode aMode)
/** Allocates and constructs a new CContactItemViewDef, specifying a use and a 
mode, leaving the object on the cleanup stack.

@param aUse Specifies whether to include or exclude specified fields.
@param aMode Specifies whether to include or exclude hidden fields.
@return Pointer to the newly created view definition. */
	{ // static
	CContactItemViewDef* viewDef=CContactItemViewDef::NewL(aUse, aMode);
	CleanupStack::PushL(viewDef);
	return viewDef;
	}

/** Allocates and constructs a new CContactItemViewDef based on a RReadStream, 
specifying a use and a mode, leaving the object on the cleanup stack.

@param aStream RReadStream containing object to internalize.
@return Pointer to the newly created view definition. 
@internalTechnology
*/
CContactItemViewDef* CContactItemViewDef::NewLC(RReadStream& aStream)
	{
	CContactItemViewDef::TUse aUse = (CContactItemViewDef::TUse)aStream.ReadInt32L();
	CContactItemViewDef::TMode aMode = (CContactItemViewDef::TMode)aStream.ReadInt32L();	
	CContactItemViewDef* viewDef=CContactItemViewDef::NewLC(aUse, aMode);
	viewDef->InternalizeL(aStream);
	
	return viewDef;
	}	

CContactItemViewDef::CContactItemViewDef(TUse aUse, TMode aMode)
	: iFieldTypes(5),iUse(aUse),iMode(aMode)
	{}

EXPORT_C TInt CContactItemViewDef::Find(TFieldType aFieldType) const
/** Searches the view definition for the specified field type.

@param aFieldType The field type to search for.
@return The index in the view definition of the matching field type, or KErrNotFound. */
	{
	const TInt count=Count();
	for (TInt ii=0;ii<count;ii++)
		{
		TUid fieldType=iFieldTypes[ii];
		if (fieldType==KUidContactFieldMatchAll || aFieldType==KUidContactFieldMatchAll || fieldType==aFieldType)
			return ii;
		}
	return KErrNotFound;
	}

EXPORT_C TInt CContactItemViewDef::Find(const CContentType &aContentType) const
/** Searches the view definition for any field type contained in the specified 
content type.

@param aContentType Content type containing the field types to search for.
@return The index in the view definition of the first matching field type or 
KErrNotFound. */
	{
	TInt ret=KErrNotFound;
	for(TInt loop=0;loop<aContentType.FieldTypeCount();loop++)
		{
		ret=Find(aContentType.FieldType(loop));
		if (ret!=KErrNotFound)
			break;
		}
	return(ret);
	}

EXPORT_C TBool CContactItemViewDef::MatchesAll() const
/** Tests whether the view definition contains a field type with the value KUidContactFieldMatchAll. 

If this is the case, all fields in the contact item are retrieved, regardless 
of the other field types specified in the view definition.

@return ETrue if all field types are matched, EFalse if not. */
	{
	const TInt count=Count();
	for (TInt ii=0;ii<count;ii++)
		{
		TUid fieldType=iFieldTypes[ii];
		if (fieldType==KUidContactFieldMatchAll)
			return(ETrue);
		}
	return(EFalse);
	}

EXPORT_C void CContactItemViewDef::AddL(TFieldType aFieldType)
/** Appends a field type to the view definition's array of field types.

@param aFieldType The field type to append to the array of field types. */
	{
	__ASSERT_DEBUG(Find(aFieldType)<=KErrNotFound,Panic(ECntPanicDuplicateViewFields));
	iFieldTypes.AppendL(aFieldType);
	}

EXPORT_C void CContactItemViewDef::Remove(TFieldType aFieldType)
/** Removes a field type, identified by its UID.

@param aFieldType The field type to remove from the list. This must be 
present in the CContactItemViewDef object, or the function raises a panic. */
	{
	const TInt index=Find(aFieldType);
	__ASSERT_ALWAYS(index!=KErrNotFound,Panic(ECntPanicFieldDoesNotExist));
	iFieldTypes.Delete(index);
	}

EXPORT_C void CContactItemViewDef::Remove(TInt aIndex)
/** Removes a field type, identified by its index into the array of field types.

@param aIndex The index of the element to delete. The position is relative 
to zero. This value must not be negative and must not be greater than the 
number of elements currently in the list, or the function raises a panic. */
	{ // inline this ?? !!!
	iFieldTypes.Delete(aIndex);
	}

EXPORT_C void CContactItemViewDef::InternalizeL(RReadStream& aStream)
/** Internalises the item view definition from a read stream. The presence of this 
function means that the standard templated operator>>() (defined in s32strm.h) 
is available to internalise objects of this class.

@param aStream Stream from which the item view definition should be internalised. */
	{
	iUse=STATIC_CAST(CContactItemViewDef::TUse,aStream.ReadInt32L());
	TInt count;
	count=aStream.ReadInt32L();
	Reset();
	for(TInt ii=0;ii<count;ii++)
		{
		TUid fieldType;
		aStream>>fieldType;
		iFieldTypes.AppendL(fieldType);
		}
	}

EXPORT_C void CContactItemViewDef::ExternalizeL(RWriteStream& aStream) const
/** Externalises the item view definition to a write stream. The presence of this 
function means that the standard templated operator<<() (defined in s32strm.h) 
is available to externalise objects of this class.

@param aStream Stream to which the item view definition should be externalised. */
	{
	aStream.WriteInt32L(iUse);
	TInt count=Count();
	aStream.WriteInt32L(count);
	for(TInt ii=0;ii<count;ii++)
		aStream << iFieldTypes[ii];
	}

EXPORT_C TBool CContactItemViewDef::Contains(const CContactItem& aItem)
/** Tests whether a contact item complies with the view definition.

This indicates whether or not the item will be included in the view. 

@param aItem The contact item to test.
@return ETrue if any of the item's fields are contained in the view definition 
(so that the item will be included in the view), EFalse if none of the item's 
fields are contained in the view definition (so that the item will not be 
included in the view). */
	{
	CContactItemFieldSet& fieldSet=aItem.CardFields();
	const TInt count=fieldSet.Count();
	for (TInt ii=0;ii<count;ii++)
		if (Find(iFieldTypes[ii])!=KErrNotFound)
			return ETrue;
	return EFalse;
	}

//
// class CContactViewDef
//

EXPORT_C CContactViewDef* CContactViewDef::NewL()
/**  Allocates and constructs a view definition.

@return Pointer to the new view definition. */
	{
	CContactViewDef* viewDef=CContactViewDef::NewLC();
	CleanupStack::Pop(viewDef);
	return viewDef;
	}

EXPORT_C CContactViewDef* CContactViewDef::NewLC()
/** Allocates and constructs a view definition, leaving it on the cleanup stack.

@return Pointer to the new view definition. */
	{
	CContactViewDef* viewDef=new(ELeave) CContactViewDef();
	CleanupStack::PushL(viewDef);
	return(viewDef);
	}

EXPORT_C CContactViewDef* CContactViewDef::NewL(CContactItemViewDef* aItemDef)
/** Allocates and constructs a view definition. 

@param aItemDef Pointer to the item view definition. This contains the view definition's 
field types, use and mode. The view definition takes ownership of this.
@return Pointer to the new view definition. */
	{ // static
	CContactViewDef* viewDef=CContactViewDef::NewLC(aItemDef);
	CleanupStack::Pop(viewDef);
	return viewDef;
	}

EXPORT_C CContactViewDef* CContactViewDef::NewLC(CContactItemViewDef* aItemDef)
/** Allocates and constructs a view definition. 

@param aItemDef Pointer to the item view definition. This contains the view definition's 
field types, use and mode. The view definition takes ownership of this.
@return Pointer to the new view definition. This is left on the cleanup 
stack.*/
	{ // static
	CContactViewDef* viewDef=new(ELeave) CContactViewDef();
	CleanupStack::PushL(viewDef);
	viewDef->ConstructL(aItemDef);
	return viewDef;
	}
	
/** Allocates and constructs a view definition based on a RReadStream.

@param aStream RReadStream containing object to internalize.
@return Pointer to the new view definition. This is left on the cleanup stack. 
@internalTechnology 
*/
CContactViewDef* CContactViewDef::NewLC(RReadStream& aStream)
	{
	CContactViewDef* viewDef=CContactViewDef::NewLC();
	CContactItemViewDef* itemDef=CContactItemViewDef::NewLC(aStream);	
	viewDef->ConstructL(itemDef);
	CleanupStack::Pop(itemDef);
	return viewDef;
	}		

EXPORT_C CContactViewDef::~CContactViewDef()
/** Destroys the view definition object and deletes its owned item view definition. */
	{
	delete iItemDef;
	}

EXPORT_C void CContactViewDef::InternalizeL(RReadStream& aStream)
/** Internalises the object's item view definition from a read stream.

@param aStream Stream from which the object should be internalised. */
	{
	CContactItemViewDef* itemDef=CContactItemViewDef::NewLC(CContactItemViewDef::EIncludeFields,CContactItemViewDef::EIncludeHiddenFields);
	itemDef->InternalizeL(aStream);
	CleanupStack::Pop(itemDef);
	delete iItemDef;
	iItemDef=itemDef;
	}

EXPORT_C void CContactViewDef::ExternalizeL(RWriteStream& aStream) const
/** Externalises the object's item view definition to a write stream.

@param aStream Stream to which the object should be externalised. */
	{
	iItemDef->ExternalizeL(aStream);
	}
	
CContactViewDef::CContactViewDef()
	{}

void CContactViewDef::ConstructL(CContactItemViewDef* aItemDef)
	{
	iItemDef=aItemDef;
	}
	
EXPORT_C CContactItemViewDef& CContactViewDef::ItemDef() const
/** Gets the object's item view definition.

@return Reference to the item view definition. */
	{
	return *iItemDef;
	}

EXPORT_C CContactIdArray& CContactViewDef::Groups() const
/** This function is not supported. 

If called, it leaves with KErrNotSupported. */
	{
	User::Leave(KErrNotSupported); // leaves...
	CContactIdArray* contactIds=NULL;
	return *contactIds;
	}
