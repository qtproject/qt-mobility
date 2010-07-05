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
*
*/


#include <s32std.h>

#include "cntstd.h"
#include <cntdef.h>
#include <cntfield.h>
#include <cntfldst.h>

#include <cntitem.h>
#include <versit.h>
#include "cntprof.h"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "cntfieldheader.h"
#include "cnthint.h"
#include "cntfield_internal.h"
#endif

//
// class TContactFieldAtts
//
const TUint32 KAttribsMask		    = 0xf0000fff;
const TUint32 KExtendedAttribsMask	= 0x000ff000;
const TUint32 KStorageTypeMask	    = 0x00f00000;

const TUint32 KTypeBitShift		    = 20;
const TUint32 KExtendedAttsBitShift = 12;

/** Set contact field attributes.

@param aAttribs contact field arributes to be set.
@internalTechnology
*/
void TContactFieldAtts::SetAttribs(TUint32 aAttribs)
	{
	iStorage |= aAttribs;
	}
	
/** Set contact field extended arributes.

@param aExtendedAttribs contact field extended arributes to be set.
@internalTechnology
*/
void TContactFieldAtts::SetExtendedAttribs(TUint32 aExtendedAttribs)
	{
	iStorage |= aExtendedAttribs << KExtendedAttsBitShift;
	}

/** Set storage type.

@param aType storage type to set.
@internalTechnology
*/
void TContactFieldAtts::SetType(TStorageType aType)
	{
	iStorage |= aType << KTypeBitShift;
	}

/** Get contact field arributes.

@return contact field arributes.
@internalTechnology
*/
TUint32 TContactFieldAtts::Attribs() const
	{
	return (iStorage & KAttribsMask);
	}
	
/** Get contact field extended arributes.

@return contact field extended arributes.
@internalTechnology
*/
TUint32 TContactFieldAtts::ExtendedAttribs() const
	{
	return (iStorage & KExtendedAttribsMask) >> KExtendedAttsBitShift;
	}
	
/** Get contact field storage type.

@return contact field storage type.
@internalTechnology
*/
TStorageType TContactFieldAtts::Type() const
	{
	return (iStorage & KStorageTypeMask) >> KTypeBitShift;
	}
	
/** Internalize implementation.

@internalTechnology
*/
void TContactFieldAtts::InternalizeL(RReadStream& aStream)
	{
	iStorage=aStream.ReadUint32L();
	}
	
/** Externalize implementation.

@internalTechnology
*/
void TContactFieldAtts::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteUint32L(iStorage);
	}


//
// class CContentType
//
   
EXPORT_C CContentType::~CContentType()
/** Frees all resources owned by the content type object, prior 
to its destruction. */
    {
	delete iFieldTypes;
	}
      
CContentType::CContentType()
    {
	}
      
CContentType::CContentType(TUid aMapping) : iMapping(aMapping)
	{
	}
	
/** Constructs a new content type based on a RReadStream.

@param aStream RReadStream containing object to internalize.
@return Pointer to the newly created CContentType. This is left on the cleanup stack. 
@internalTechnology
*/
CContentType* CContentType::NewLC(RReadStream& aStream)
	{ // static
	CContentType* type = NewL();
	CleanupStack::PushL(type);
	type->InternalizeL(aStream);
	
	return type;
	}		

EXPORT_C CContentType* CContentType::NewL()
/** Allocates and constructs a new default CContentType.

The object has no field types and the mapping is set to KNullUid

@return Pointer to the newly created content type object. */
	{
	CContentType* self=new(ELeave) CContentType(KNullUid);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();	// self
	return(self);
	}

EXPORT_C CContentType* CContentType::NewL(TFieldType aFieldType,TUid aMapping)
/** Allocates and constructs a new CContentType with a single field 
type and a mapping.

@param aFieldType The field type to add to the content type.
@param aMapping The mapping.
@return Pointer to the newly created content type object. */
	{
	CContentType* self=new(ELeave) CContentType(aMapping);
	CleanupStack::PushL(self);
	self->ConstructL();
	self->AddFieldTypeL(aFieldType);
	CleanupStack::Pop();	// self
	return(self);
	}

EXPORT_C CContentType* CContentType::NewL(const CContentType &aContentType)
/** Allocates and constructs a new CContentType based on another one.

@param aContentType The CContentType on which to base the new one.
@return Pointer to the newly created content type object. */
	{ // static
	CContentType* self=new(ELeave) CContentType;
	CleanupStack::PushL(self);
	self->ConstructL();
	self->CloneL(aContentType);
	CleanupStack::Pop();	// self
	return self;
	}
 
void CContentType::ConstructL()
	{
	iFieldTypes=new(ELeave) CArrayFixFlat<TUid>(2);
	}

EXPORT_C void CContentType::AddFieldTypeL(TFieldType aFieldType)
/** Appends a field type to the content type's list of field types.

Note that certain combinations of field types are not valid and should not be
used.

@param aFieldType The field type to append to the list of field types. */
	{
	const TInt KMaxFieldTypes=15;
	if (iFieldTypes->Count()==KMaxFieldTypes)
		{
		User::Leave(KErrOverflow);
		}
    		
   	iFieldTypes->AppendL(aFieldType);
	}

EXPORT_C void CContentType::RemoveFieldType(TFieldType aFieldType)
/** Removes a field type from the list of field types.

@param aFieldType The field type to remove from the list of field types. */
	{
	for(TInt loop=0;loop<iFieldTypes->Count();loop++)
		if ((*iFieldTypes)[loop]==aFieldType)
			{
			iFieldTypes->Delete(loop);
			break;
			}
	}

EXPORT_C void CContentType::SetMapping(TUid aMapping)
/** Sets the vCard mapping.

@param aMapping The new vCard mapping for the content type. */
	{
	iMapping=aMapping;
	}

void CContentType::CloneL(const CContentType &aContentType)
	{
    iMapping=aContentType.iMapping;
	if (aContentType.iFieldTypes)
		for(TInt loop=0;loop<aContentType.iFieldTypes->Count();loop++)
			AddFieldTypeL((*aContentType.iFieldTypes)[loop]);
	}

EXPORT_C TInt CContentType::FieldTypeCount() const
/** Gets the number of field types in the content type's list of field types.

@return The number of field types in the content type. */
	{
	return(iFieldTypes->Count());
	}

EXPORT_C TFieldType CContentType::FieldType(TInt aIndex) const
/** Gets the indexed field type.

@param aIndex Index into the list of field types. The position is relative 
to zero; i.e. zero implies the first element in the array. This value must 
be non-negative and less than the number of objects currently within the array 
otherwise the operator raises a panic.
@return The indexed field type. */
	{
	__ASSERT_DEBUG(aIndex<iFieldTypes->Count(),Panic(ECntPanicFieldTypeIndex));
	return((*iFieldTypes)[aIndex]);
	}

EXPORT_C TUid CContentType::Mapping() const
/** Gets the vCard mapping.

@return The vCard mapping. */
	{
	return(iMapping);
	}

EXPORT_C TBool CContentType::ContainsFieldType(TFieldType aFieldType) const
/** Tests whether the content type object contains the specified field type UID 
either as the mapping value or in its list of field types.

@param aFieldType The field type of interest.
@return ETrue if the CContentType contains the specified field type. EFalse 
if not. */
	{
	if (iMapping==aFieldType)
		return(ETrue);
	for(TInt loop=0;loop<iFieldTypes->Count();loop++)
		if ((*iFieldTypes)[loop]==aFieldType)
			return(ETrue);
	return(EFalse);
	}

void CContentType::InternalizeAdditionalUidsL(TInt aCount,RReadStream& aStream)
	{
	for (TInt ii=0;ii<aCount;ii++) 
		iFieldTypes->AppendL(TUid::Uid(aStream.ReadInt32L()));
	}

EXPORT_C TBool CContentType::SupportsMultipleLines() const
/** Tests whether the content type supports multiple lines of text. If the content 
type object contains a field type which supports this, either in its list 
of field types, or as its mapping, the function returns ETrue. Examples of 
such field types are address fields (type KUidContactFieldAddress) and note 
fields (type KUidContactFieldNote).

@return ETrue if the CContentType supports multiple lines of text. EFalse 
if not. */
	{
	return(ContainsFieldType(KUidContactFieldAddress) || ContainsFieldType(KUidContactFieldNote));
	}

EXPORT_C TBool CContentType::operator==(const CContentType &aType) const
/** Compares two content type objects for equality. 
Two content type objects are equal according to this method if all following 
conditions are met:
- vCard property mapping matches
- same number of field types
- main field type matches
- additional field types and vCard property parameter mappings match
 
@param aType The content type to compare with this CContentType.
+@return ETrue if aType is equal with current content type. */
	{
	// Compare vCard property mapping.
	if (Mapping() != aType.Mapping())
		{
		return EFalse;
		}
	
	// Compare field type counts.
	const TInt typeCount = FieldTypeCount();
	if (typeCount != aType.FieldTypeCount())
		{
		return EFalse;
		}
	if (typeCount <= 0)
		{
		// Nothing more to compare.
		return ETrue;
		}
	
	// Compare main field type.
	if (FieldType(0) != aType.FieldType(0))
		{
		return EFalse;
		}
	
	// Compare additional field types and vCard property parameter mappings.
	for (TInt lhsIndex = 1; lhsIndex < typeCount; ++lhsIndex)
		{
		TInt rhsIndex = 1;
		for (rhsIndex = 1; rhsIndex < typeCount; ++rhsIndex)
			{
			if (FieldType(lhsIndex) == aType.FieldType(rhsIndex))
			    break;
			}
		if (rhsIndex == typeCount)
			{
			// No match found.
			return EFalse;
			}
		}
	
	return ETrue;
	}

EXPORT_C TBool CContentType::IsEqual(const CContentType &aType) const
/** Compares two content type objects for equality.
Two content type objects are equal according to this method if all following 
conditions are met:
- vCard property mapping matches
- same number of field types
- main field type matches
- additional field types and vCard property parameter mappings match
 
@param aType The content type to compare with this CContentType.
@return ETrue if aType is equal with current content type. */
	{
	// Compare vCard property mapping.
	if (iMapping!=aType.iMapping)
		{
		return(EFalse);	
		}
	
	// Compare field type counts.
	const TInt typeCount = aType.FieldTypeCount();
	if (FieldTypeCount()!= typeCount)
		{
		return(EFalse);	
		}
		
    if (typeCount <= 0)
        {
        // Nothing more to compare.
        return ETrue;
        }
			
    // Compare main field type.
    if (FieldType(0) != aType.FieldType(0))
        {
        return EFalse;
        }

    // Compare additional field types and vCard property parameter mappings.
    for (TInt leftIndex = 1; leftIndex < typeCount; ++leftIndex)
        {
        TUint rightIndex = 1;
        for (rightIndex = 1; rightIndex < typeCount; ++rightIndex)
            {
            if (FieldType(leftIndex) == aType.FieldType(rightIndex))
                break;
            }
        if (rightIndex == typeCount)
            {
            // No match found.
            return EFalse;
            }
        }

    return ETrue;
   	}
  	
void CContentType::Reset()
	{
	iMapping=TUid::Uid(0);
	iFieldTypes->Reset();
	}


/*
 * Determine if content types are suitable to match for synchronization purpose. 
 * Some properties are subject to specific processing depending of the <code>CContentType</code> Mapping. 
 * Notably: VOICE, PREF, VoiceDial and SpeedDial. 
 * VOICE is a a default property parameter for all TEL properties.
 *
 * @param		"const CContentType& aType"
 *				The content type to compare with this
 *				<code>CContentType</code>. Beware, to do a proper comparison,
 *				the <code>aType</code> parameter must not contains VoiceDial or SpeedDial properties.
 *				Otherwise the fields will not match.				  
 *
 * @return 		"TBool"
 *				<code>ETrue</code> if <code>aType</code> match this content type.
 *				Field types do not need to be in the same order in the 
 *				list of field types for a match to be made.
 */
EXPORT_C TBool CContentType::IsEqualForSyncUpdate(const CContentType& aType) const
/** Tests whether the content types are suitable to match for synchronisation purpose. 

@param aType The content type to compare with this CContentType.
@return ETrue if aType is an identical content type
 */
	{
	if (iMapping!=aType.iMapping)
		return(EFalse);	

	switch (iMapping.iUid)
		{
	case KIntContactFieldVCardMapTEL:
		{
		TInt countModifier=0;
		TInt loop=0;

		if (aType.ContainsFieldType(KUidContactFieldVCardMapVOICE))
			{
			++countModifier;
			}

		if (aType.ContainsFieldType(KUidContactFieldVCardMapPREF))
			{
			++countModifier;
			}
		
		if (ContainsFieldType(KUidContactFieldVCardMapVOICE))
			{
			--countModifier;
			}

		if (ContainsFieldType(KUidContactFieldVCardMapPREF))
			{
			--countModifier;
			}

		if (ContainsFieldType(KUidContactsVoiceDialField))
			{
			--countModifier;
			}

		//look for speed dial property only in db field
		for(loop=0;loop<iFieldTypes->Count();loop++)
			{
			TInt uid = (*iFieldTypes)[loop].iUid;
			if (uid >= KUidSpeedDialOneValue && uid <= KUidSpeedDialNineValue)
				{
				--countModifier;
				break; //only one speed dial property allowed for a field
				}
			}
	
		//now check for field count	
		if ((FieldTypeCount()+countModifier)!=aType.FieldTypeCount())
			{
			return(EFalse);
			}
				
		//Check that all type from the imported field match a type in this field but VOICE and PREF
		for(loop=0;loop<aType.FieldTypeCount();loop++)
			{
			if (!ContainsFieldType(aType.FieldType(loop)))
				{
				if (aType.FieldType(loop).iUid==KIntContactFieldVCardMapVOICE || aType.FieldType(loop).iUid==KIntContactFieldVCardMapPREF)
					{
					continue;
					}
				return(EFalse);
				}
			}
		return(ETrue);
		}

	case KIntContactFieldVCardMapEMAILINTERNET:
		{
		TInt countModifier=0;		

		if (aType.ContainsFieldType(KUidContactFieldVCardMapPREF))
			{
			++countModifier;
			}
		
		if (ContainsFieldType(KUidContactFieldVCardMapPREF))
			{
			--countModifier;
			}
	
		//now check for field count	
		if ((FieldTypeCount()+countModifier)!=aType.FieldTypeCount())
			{
			return(EFalse);
			}
				
		//Check that all type from the imported field match a type in this field but PREF
		for(TInt loop=0;loop<aType.FieldTypeCount();loop++)
			{
			if (!ContainsFieldType(aType.FieldType(loop)))
				{
				if (aType.FieldType(loop).iUid==KIntContactFieldVCardMapPREF)
					{
					continue;
					}
				return(EFalse);
				}
			}
		return(ETrue);
		}
	
	case KIntContactFieldVCardMapAGENT:
		{
		return(ETrue);
		}	

	default:
		return *this==aType;
		}
	}
	
void CContentType::InternalizeL(RReadStream& aStream)
/** Internalises a CContentType object from a read stream. 
@param aStream Stream from which the object should be internalised. */
	{
    // TUid iMapping;
	iMapping.iUid = aStream.ReadInt32L();
	
	// CArrayFix<TUid>* iFieldTypes;
	const TInt count=aStream.ReadInt32L();

	// Allocated in constructor
	TUid tempID;
	for(TInt index=0; index<count; ++index)
		{
		
		aStream>>tempID;		
		iFieldTypes->AppendL(tempID);
		}
	}

void CContentType::ExternalizeL(RWriteStream& aStream) const 
/** Externalises a CContentType object to a write stream.
@param aStream Stream to which the object should be externalised. */
	{
	aStream.WriteInt32L(iMapping.iUid);
	
	const TInt count = iFieldTypes->Count();
	aStream.WriteInt32L(count);	
	for(TInt index=0; index<count; ++index)
		{
		aStream<<iFieldTypes->At(index);
		}
	}		


//
// class CContactItemField
//

EXPORT_C CContactItemField* CContactItemField::NewLC()
/** Allocates and constructs a new default contact item field. 

The field's storage type, content type and label are unspecified. 
The ESynchronize attribute is set.

@return Pointer to the newly created contact item field. */
	{
    CContactItemField* self=new(ELeave) CContactItemField;
    CleanupStack::PushL(self);
	self->iContentType=CContentType::NewL();
    return self;
	}

EXPORT_C CContactItemField* CContactItemField::NewL(TStorageType aType)
/** Allocates and constructs a contact item field with a storage type. 

The field's label and content type are unspecified.

@param aType The field's storage type.
@return Pointer to the newly created contact item field. */
	{
    CContactItemField* self=CContactItemField::NewLC(aType);
    CleanupStack::Pop(); // self
    return self;
	}

EXPORT_C CContactItemField* CContactItemField::NewLC(TStorageType aType)
/** Allocates and constructs a contact item field with a storage type. 

The field's label and content type are unspecified.

@param aType The field's storage type.
@return Pointer to the newly created contact item field. This is left on 
the cleanup stack. */
	{
    CContactItemField* self=new(ELeave) CContactItemField(aType);
    CleanupStack::PushL(self);
    self->ConstructStorageL();
	self->iContentType=CContentType::NewL();
    return self;
	}

EXPORT_C CContactItemField* CContactItemField::NewL(TStorageType aType, TFieldType aFieldType)
/** Allocates and constructs a contact item field with a storage type 
and a field type.

The field's content type is initialised with the field type, 
and its vCard mapping is set by default to KNullUid. The field's label is 
unspecified.

@param aType The field's storage type.
@param aFieldType The field type as defined in cntdef.h.
@return Pointer to the newly created contact item field. */
    { // static
    CContactItemField* self=CContactItemField::NewLC(aType,aFieldType);
    CleanupStack::Pop(); // self
    return self;
    }

EXPORT_C CContactItemField* CContactItemField::NewLC(TStorageType aType, TFieldType aFieldType)
/** Allocates and constructs a contact item field with a storage type 
and a field type.

The field's content type is initialised with the field type, 
and its vCard mapping is set by default to KNullUid. The field's label is 
unspecified.

@param aType The field's storage type.
@param aFieldType The field type as defined in cntdef.h.
@return Pointer to the newly created contact item field. This is left on 
the cleanup stack. */
    { // static
    CContactItemField* self=new(ELeave) CContactItemField(aType);
    CleanupStack::PushL(self);
    self->ConstructStorageL();
	self->iContentType=CContentType::NewL(aFieldType);
    return self;
    }

EXPORT_C CContactItemField* CContactItemField::NewL(const CContactItemField &aField)
/** Allocates and constructs a contact item field based on another one.

All details (content type, storage type, attributes and label) are copied 
from the specified field.

@param aField The contact field to copy.
@return Pointer to the newly created contact item field. */
	{ // static
	CContactItemField* self=CContactItemField::NewLC(aField);
	CleanupStack::Pop(); // self
	return self;
	}

EXPORT_C CContactItemField* CContactItemField::NewLC(const CContactItemField &aField)
/** Allocates and constructs a contact item field based on another one.

All details (content type, storage type, attributes and label) are copied 
from the specified field.

@param aField The contact field to copy.
@return Pointer to the newly created contact item field. This is left on 
the cleanup stack. */
	{ // static
	CContactItemField* self=new(ELeave) CContactItemField(aField.StorageType());
	CleanupStack::PushL(self);
	self->ConstructStorageL();
	self->CloneL(aField);
	return self;
	}
 
EXPORT_C CContactItemField* CContactItemField::NewL(TStorageType aType, const CContentType &aContentType)
/** Allocates and constructs a contact item field with a content type 
and a storage type. 

The field's label is unspecified.

@param aType The field's storage type.
@param aContentType The field's content type. 
@return Pointer to the newly created contact item field. */
	{ // static
	CContactItemField* self=CContactItemField::NewLC(aType,aContentType);
	CleanupStack::Pop(); // self
	return self;
	}

EXPORT_C CContactItemField* CContactItemField::NewLC(TStorageType aType, const CContentType &aContentType)
/** Allocates and constructs a contact item field with a content type 
and a storage type. 

The field's label is unspecified.

@param aType The field's storage type.
@param aContentType The field's content type. 
@return Pointer to the newly created contact item field. This is left on 
the cleanup stack. */
	{ // static
	CContactItemField* self=new(ELeave) CContactItemField(aType);
	CleanupStack::PushL(self);
	self->ConstructStorageL();
	self->iContentType=CContentType::NewL(aContentType);
	return self;
	}
 
CContactItemField::CContactItemField(TStorageType aType)
	: iStorageType(aType), iAttributes(ESynchronize|EOverRidesLabel|ELabelUnspecified),iTemplateFieldId(KNullFieldId)
    {}

CContactItemField::CContactItemField()
	: iAttributes(ESynchronize),iTemplateFieldId(KNullFieldId)
    {}

void CContactItemField::ConstructStorageL()
    {
	__ASSERT_DEBUG(iStorage==NULL,Panic(ECntPanicStorageAlreadyAllocated));
    switch (iStorageType)
        {
    case KStorageTypeText:
        iStorage=new(ELeave) CContactTextField;
        break;
    case KStorageTypeStore:
        iStorage=new(ELeave) CContactStoreField;
        break;
    case KStorageTypeDateTime:
        iStorage=new(ELeave) CContactDateField;
        break;
    case KStorageTypeContactItemId:
        iStorage=new(ELeave) CContactAgentField;
        break;
    default:
    	User::Leave(KErrNotSupported);
    	break;
        }
    }

void CContactItemField::CloneL(const CContactItemField &aField)
	{
	iContentType=CContentType::NewL(aField.ContentType());
	if (aField.iLabel)
		iLabel=aField.iLabel->AllocL();
	iAttributes=aField.iAttributes;
	// copy extended attributes as well
	iExtendedAttributes=aField.iExtendedAttributes; 
	iTemplateFieldId=aField.iTemplateFieldId;
	CopyStorageL(aField);
	}

void CContactItemField::CopyStorageL(const CContactItemField &aField)
	{
	switch(aField.StorageType())
		{
		case KStorageTypeText:
			TextStorage()->SetTextL(aField.TextStorage()->Text());
			break;
		case KStorageTypeDateTime:
			DateTimeStorage()->SetTime(aField.DateTimeStorage()->Time());
			break;
		case KStorageTypeContactItemId:
			AgentStorage()->SetAgentId(aField.AgentStorage()->Value());
			break;
		case KStorageTypeStore:
			if (aField.StoreStorage()->Thing())
				{
				StoreStorage()->SetThingL(*aField.StoreStorage()->Thing());
				}
			break;
		}
	}

EXPORT_C CContactItemField::~CContactItemField()
/** Frees all resources owned by the field (the label, the stored 
data and the content type), prior to its destruction. */
    {
    delete iLabel;
    delete iStorage;
	delete iContentType;
    }

/**
@internalTechnology
  */
EXPORT_C void CContactItemField::Reset()
	{
    delete iLabel;
	iLabel=NULL;
    delete iStorage;
	iStorage=NULL;
	iContentType->Reset();
	iStorageType=0;
	iId=KNullFieldId	;
	iAttributes=ESynchronize;
	iExtendedAttributes=0;
	iTemplateFieldId=KNullFieldId;
	}

void CContactItemField::MapHintsToFieldTypesL(THint aHint)
	{
	if (aHint.IsPhone())
		AddFieldTypeL(KUidContactFieldPhoneNumber);
	if (aHint.IsMsg())
		AddFieldTypeL(KUidContactFieldMsg);
	if (aHint.IsCompanyName())
		AddFieldTypeL(KUidContactFieldCompanyName);
	if (aHint.IsFamilyName())
		AddFieldTypeL(KUidContactFieldFamilyName);
	if (aHint.IsGivenName())
		AddFieldTypeL(KUidContactFieldGivenName);
	if (aHint.IsCompanyNamePronunciation())
		AddFieldTypeL(KUidContactFieldCompanyNamePronunciation);
	if (aHint.IsFamilyNamePronunciation())
		AddFieldTypeL(KUidContactFieldFamilyNamePronunciation);
	if (aHint.IsGivenNamePronunciation())
		AddFieldTypeL(KUidContactFieldGivenNamePronunciation);
	if (aHint.IsAddress())
		AddFieldTypeL(KUidContactFieldAddress);
	if (aHint.IsAdditionalName())
		AddFieldTypeL(KUidContactFieldAdditionalName);
	if (aHint.IsSuffixName())
		AddFieldTypeL(KUidContactFieldSuffixName);
	if (aHint.IsPrefixName())
		AddFieldTypeL(KUidContactFieldPrefixName);
	if (aHint.IsEmail())
		AddFieldTypeL(KUidContactFieldEMail);
	if (aHint.IsStorageInline())
		AddFieldTypeL(KUidContactStorageInline);
	}

TBool CContactItemField::AddFieldToHint(TFieldType aFieldType, CContactItemField::THint &aHint) const 
  	{
	TBool matchedHint = ETrue;
	switch(aFieldType.iUid)
		{
		case KUidContactFieldAddressValue:
			aHint.SetIsAddress();
			break;
		case KUidContactFieldCompanyNameValue:
			aHint.SetIsCompanyName();
			break;
		case KUidContactFieldPhoneNumberValue:
			aHint.SetIsPhone();
			break;
		case KUidContactFieldGivenNameValue:
			aHint.SetIsGivenName();
			break;
		case KUidContactFieldFamilyNameValue:
			aHint.SetIsFamilyName();
			break;
		case KUidContactFieldCompanyNamePronunciationValue:
			aHint.SetIsCompanyNamePronunciation();
			break;
		case KUidContactFieldGivenNamePronunciationValue:
			aHint.SetIsGivenNamePronunciation();
			break;
		case KUidContactFieldFamilyNamePronunciationValue:
			aHint.SetIsFamilyNamePronunciation();
			break;
		case KUidContactFieldAdditionalNameValue:
			aHint.SetIsAdditionalName();
			break;
		case KUidContactFieldSuffixNameValue:
			aHint.SetIsSuffixName();
			break;
		case KUidContactFieldPrefixNameValue:
			aHint.SetIsPrefixName();
			break;
		case KUidContactFieldEMailValue:
			aHint.SetIsEmail();
			break;
		case KUidContactFieldMsgValue:
			aHint.SetIsMsg();
			break;
		case KUidContactFieldStorageInlineValue:
			aHint.SetStorageIsInline();
			break;
		default:
			matchedHint=EFalse;
			break;
		}
	return(matchedHint);
	}


/**
Encode contact field data into stream store.

@param aTextStream the text blob stream to export text data.
@param aBlobStore the binary blob stream to export binary data.
@param aTextFieldIndex the index of text field stored in the storage

@return field header which is to be stored into header blob. 
@internalTechnology 
*/
TFieldHeader CContactItemField::StoreL(RWriteStream& aTextStream, CStreamStore& aBlobStore, TInt aTextFieldIndex)
	{
	TStreamId streamId = KNullStreamIdValue;
	if (iStorageType == KStorageTypeText)
		{
		STATIC_CAST(CContactTextField*,iStorage)->ExternalizeL(aTextStream, ETrue, aTextFieldIndex);
		}
	else
	    {
		streamId = iStorage->StoreL(aBlobStore);
	    }
	
	TContactFieldAtts atts;
	/* sets all attributes of the field:- hidden private etc.. + type (text etc..)
	   into TContactFieldAtts which is then stored ahead of the field data */
	atts.SetAttribs(iAttributes);
	atts.SetExtendedAttribs(iExtendedAttributes);
	atts.SetType(iStorageType);
    return TFieldHeader(atts, iId, streamId);
	}


/**
Decode given blob header stream into contact field relevent data.

@param aStream reference to the blob header stream to be decoded.
@param aSystemTemplateFields cached template fields.

@return the stream id of content data stored in data store 
@internalTechnology 
*/
EXPORT_C TStreamId CContactItemField::RestoreFieldTypesL(RReadStream &aStream, const CContactItemFieldSet *aSystemTemplateFields)
  	{
	THint hint;
	TStreamId nestedId(KNullStreamIdValue);
	TContactFieldAtts fieldAtts;

	aStream >> fieldAtts;
	
	iAttributes = fieldAtts.Attribs();
	iExtendedAttributes = fieldAtts.ExtendedAttribs();
	iStorageType = fieldAtts.Type();
	
    if(fieldAtts.Type() != KStorageTypeText)
        {
   	    //Only import stream id when the storage type is not text
        aStream >> nestedId;
        }
        
    iId = aStream.ReadUint32L();
    iTemplateFieldId = aStream.ReadUint32L();
    
	const CContactItemField* templateField = NULL;
	
	if (UsesTemplateTypes() || !OverRidesLabel())
		{
		if (aSystemTemplateFields == NULL)
		    {
			User::Leave(KErrCorrupt);
		    }
		templateField = aSystemTemplateFields->FindById(iTemplateFieldId);
		}
		
	if (templateField)
		{
		iAttributes |= (templateField->iAttributes&ETemplateMask);
		}
		
	if (UsesTemplateTypes() && templateField)
		{
		CContentType* newContent=CContentType::NewL(templateField->ContentType());
		delete iContentType;
		iContentType = NULL;
		iContentType=newContent;
		}
	else
		{
  		hint = aStream.ReadInt32L();
  		
 		if(hint.HasVCardMappingUid())
 		    {
        	iContentType->SetMapping(TUid::Uid(aStream.ReadInt32L()));
 		    }
 		    
		MapHintsToFieldTypesL(hint);
 		iContentType->InternalizeAdditionalUidsL(hint.AdditionalUidsNum(),aStream);
		}
		
	ConstructStorageL();
	
	if (OverRidesLabel())
		{
		const TInt length = aStream.ReadInt32L();
		if (length)
		    {
			iLabel = HBufC::NewL(aStream,length);
		    }
		}
	else if(templateField)
	    {
		iLabel = templateField->Label().AllocL();
	    }
		    
	return nestedId;
    }
	

void CContactItemField::RestoreDataL(CStreamStore& aStore,TStreamId aId)
    {
    RStoreReadStream stream;
    stream.OpenLC(aStore,aId);
	iStorage->RestoreL(aStore,stream);
	CleanupStack::PopAndDestroy(); // stream
	}

/**
@internalTechnology 
*/
EXPORT_C void CContactItemField::RestoreTextL(HBufC *aTextStream,TInt aTextFieldIndex)
    {
	__ASSERT_ALWAYS(iStorageType==KStorageTypeText,Panic(ECntPanicInvalidStorageType));
    STATIC_CAST(CContactTextField*,iStorage)->InternalizeL(aTextStream,aTextFieldIndex);
	}

/**
@internalTechnology 
*/
EXPORT_C TBool CContactItemField::RestoreIfMatchL(RReadStream& aStream,const CContactItemFieldDef *aFieldDef, const CContactItemFieldSet *aSystemTemplateFields,HBufC *aTextStream,TInt aTextIndex)
	{
	TStreamId nestedId;
	nestedId=RestoreFieldTypesL(aStream,aSystemTemplateFields);
	TBool match=EFalse;
	if (aFieldDef==NULL)
		match=ETrue;
	else
		{
		for(TInt loop=0;loop<aFieldDef->Count();loop++)
			{
			TUid fieldType=(*aFieldDef)[loop];
			if (fieldType==KUidContactFieldMatchAll || iContentType->ContainsFieldType(fieldType))
				{
				match=ETrue;
				break;
				}
			}
		}
	if (match && iStorageType==KStorageTypeText)
		RestoreTextL(aTextStream,aTextIndex);
	return match;
	}
	        
TBool CContactItemField::RestoreIfMatchL(RReadStream& aRootStream,TFieldType aFieldType, const CContactItemFieldSet *aSystemTemplateFields,HBufC *aTextStream,TInt aTextIndex)
	{
	CContactItemFieldDef* fieldDef=new(ELeave) CContactItemFieldDef();
	CleanupStack::PushL(fieldDef);
	fieldDef->AppendL(aFieldType);
	TBool ret=RestoreIfMatchL(aRootStream,fieldDef, aSystemTemplateFields,aTextStream,aTextIndex);
	CleanupStack::PopAndDestroy();	// fieldDef
	return(ret);
	}

EXPORT_C TStorageType CContactItemField::StorageType() const
/** Gets the field's storage type.

@return The field's storage type. */
    {
    return iStorageType;
    }

EXPORT_C const CContentType &CContactItemField::ContentType() const
/** Gets the field's content type.

@return Reference to the field's content type. */
    {
    return *iContentType;
    }

const CContentType &CContactItemField::TemplateContentType(const CContactItemFieldSet &aSystemTemplateFields) const
	{
	if (UsesTemplateTypes() || OverRidesLabel())
		return(*iContentType);
	const CContactItemField *templateField=aSystemTemplateFields.FindById(iTemplateFieldId);
	if (!templateField)
		return(*iContentType);
	return(templateField->ContentType());
	}

EXPORT_C TPtrC CContactItemField::Label() const
/** Gets the field's label.

@return The field label. If no label has been set, its length is zero. */
    {
    TPtrC label;
    if (iLabel)
        label.Set(*iLabel);
    return label;
    }

EXPORT_C void CContactItemField::ResetStore()
/** Resets the field storage. The field's store is deleted, then re-allocated. */
    {
    delete iStorage;
	iStorage=NULL;
	ConstructStorageL();
	}

EXPORT_C CContactFieldStorage *CContactItemField::Storage() const
/** Gets a pointer to the field's base storage.

Rather than using this function and then casting to a specific storage class, 
one of the following functions should normally be used: TextStorage(), 
StoreStorage(), AgentStorage(), or DateTimeStorage().

@return The field's base storage type. */
    {
    return iStorage;
    }

EXPORT_C CContactTextField *CContactItemField::TextStorage() const
/** Gets a pointer to the field's storage as a CContactTextField. 

If the field's storage type is not KStorageTypeText, this function raises a panic.

@return The field's storage as a CContactTextField*. */
    {
	__ASSERT_ALWAYS(iStorageType==KStorageTypeText,Panic(ECntPanicInvalidStorageType));
    return (CContactTextField *)iStorage;
	}

EXPORT_C CContactStoreField *CContactItemField::StoreStorage() const
/** Gets a pointer to the field's storage as a CContactStoreField. 

This indicates the field data is stored in a descriptor or descriptor array. 
If the field storage type is not KStorageTypeStore, this function raises a panic.

@return Field's storage as a CContactStoreField*. */
    {
	__ASSERT_ALWAYS(iStorageType==KStorageTypeStore,Panic(ECntPanicInvalidStorageType));
    return (CContactStoreField *)iStorage;
	}

EXPORT_C CContactAgentField *CContactItemField::AgentStorage() const
/** Gets a pointer to the field's storage as a CContactAgentField. 

An agent is a property in a vCard which contains another person's contact details. 
If the field storage type is not KStorageTypeContactItemId, this function raises 
a panic.

@return Field's storage as a CContactAgentField*. */
    {
	__ASSERT_ALWAYS(iStorageType==KStorageTypeContactItemId,Panic(ECntPanicInvalidStorageType));
    return (CContactAgentField *)iStorage;
	}

EXPORT_C CContactDateField *CContactItemField::DateTimeStorage() const
/** Returns a pointer to the field's storage as a CContactDateField. 

If the field storage type is not KStorageTypeDateTime, this function raises a panic.

@return Field's storage as a CContactDateField*. */
    {
	__ASSERT_ALWAYS(iStorageType==KStorageTypeDateTime,Panic(ECntPanicInvalidStorageType));
    return (CContactDateField *)iStorage;
	}

EXPORT_C void CContactItemField::AddFieldTypeL(TFieldType aFieldType)
/** Appends a field type to the field's content type.

Note that certain combinations of field types are not valid and should not be
used.

@param aFieldType The field type to append to the field's content type. */
    {
    iContentType->AddFieldTypeL(aFieldType);
	}

EXPORT_C void CContactItemField::RemoveFieldType(TFieldType aFieldType)
/** Removes a field type from the field's content type.

@param aFieldType The field type to remove from the field's content type. */
	{
    iContentType->RemoveFieldType(aFieldType);
	}

EXPORT_C void CContactItemField::SetMapping(TUid aMapping)
/** Sets the vCard mapping for the field's content type.

@param aMapping The new mapping for the field's content type. */
    {
    iContentType->SetMapping(aMapping);
    }

EXPORT_C void CContactItemField::SetHidden(TBool aHidden)
/** Sets the value of the hidden attribute. 

If hidden fields are included in the view definition, the field is displayed 
like other fields. If the view definition masks hidden fields, it is not displayed. 
See the TMode enumeration defined in class CContactItemViewDef.

@param aHidden ETrue for hidden, EFalse for displayed. */
    {
    if (aHidden)
        iAttributes|=EHidden;
    else
        iAttributes&=~EHidden;
    }

EXPORT_C void CContactItemField::SetReadOnly(TBool aReadOnly)
/** Sets the value of the field's read only attribute.

@param aReadOnly ETrue to set the field's read only attribute, EFalse to unset 
the attribute. */
    {
    if (aReadOnly)
        iAttributes|=EReadOnly;
    else
        iAttributes&=~EReadOnly;
    }

EXPORT_C void CContactItemField::SetSynchronize(TBool aSynchronize)
/** Sets the value of the field's synchronise attribute.

@param aSynchronize ETrue to set synchronise attribute, EFalse to unset it. */
    {
    if (aSynchronize)
        iAttributes|=ESynchronize;
    else
        iAttributes&=~ESynchronize;
    }

EXPORT_C void CContactItemField::SetDisabled(TBool aDisabled)
/** Sets the value of the disabled attribute.

@param aDisabled ETrue to set the disabled attribute, EFalse to unset the 
attribute. */
    {
    if (aDisabled)
        iAttributes|=EDisabled;
    else
        iAttributes&=~EDisabled;
    }

/** 
@internalComponent
*/
void CContactItemField::UsesTemplateData(TInt aTemplateFieldId)
	{
	iTemplateFieldId=aTemplateFieldId;
	}

void CContactItemField::SetLabelUnspecified(TBool aUnspecified)
	{
	if (aUnspecified)
		iAttributes|=ELabelUnspecified;
	else
		iAttributes&=~ELabelUnspecified;
	}

EXPORT_C void CContactItemField::SetUserAddedField(TBool aUserAddedField)
/** Sets the user added field attribute.

@param aUserAddedField ETrue to set the field's user added attribute, EFalse 
to unset it. */
	{
	if (aUserAddedField)
		iAttributes|=EUserAddedField;
	else
		iAttributes&=~EUserAddedField;
	}

EXPORT_C void CContactItemField::SetTemplateField(TBool aTemplateField)
/** Sets whether the field is a template field.

@param aTemplateField ETrue to set the field's Is template attribute. EFalse 
to unset it. */
	{
	if (aTemplateField)
		iAttributes|=ETemplate;
	else
		iAttributes&=~ETemplate;
	}

EXPORT_C void CContactItemField::SetPrivate(TBool aPrivateField)
/** Sets the value of the field's private attribute. 

This is used by the contact database when exporting a contact item as a vCard, 
to identify fields which should not be exported.

@param aTemplateField ETrue to set the field's private attribute, EFalse to 
unset it. */
	{
	if (aPrivateField)
		iExtendedAttributes|=EPrivate;
	else
		iExtendedAttributes&=~EPrivate;
	}

EXPORT_C void CContactItemField::SetSpeedDial(TBool aSpeedDialField)
/** Sets the value of the field's speed dial attribute.

@param aSpeedDialField ETrue if the field should be a speed dial field, EFalse 
if not. */
	{
	if (aSpeedDialField)
		iExtendedAttributes|=ESpeedDial;
	else
		iExtendedAttributes&=~ESpeedDial;
	}

/**
@internalTechnology 
*/
EXPORT_C void CContactItemField::SetCustomFilterable(EContactFieldFlags aContactFilterType)
	{
	if (aContactFilterType == EContactFieldFlagFilterable) 
		{
 		iExtendedAttributes |= EUserDefinedFilter;
 		}
  	else if (aContactFilterType == EContactFieldFlagFilterable1) 
 		{
 		iExtendedAttributes |= EUserDefinedFilter1;
 		}
 	else if (aContactFilterType == EContactFieldFlagFilterable2) 
 		{
 		iExtendedAttributes |= EUserDefinedFilter2;
 		}
 	else if (aContactFilterType == EContactFieldFlagFilterable3) 
 		{
 		iExtendedAttributes |= EUserDefinedFilter3;
 		}
 	else if (aContactFilterType == EContactFieldFlagFilterable4) 
 		{
 		iExtendedAttributes |= EUserDefinedFilter4;
 		}
	}

/** 
Determine if a custom filter exists. If it does, return the filter type.

@param aContactFieldFlag The custom filter type if one exists.
@return ETrue if custom filter exists.
@internalTechnology 
 */	
EXPORT_C TBool CContactItemField::HasCustomFilter(EContactFieldFlags& aContactFieldFlag) const
	{
	if (iExtendedAttributes & EUserDefinedFilter) 
 		{
 		aContactFieldFlag = EContactFieldFlagFilterable;
 		return ETrue;
 		}
	else if (iExtendedAttributes & EUserDefinedFilter1) 
 		{
 		aContactFieldFlag = EContactFieldFlagFilterable1;
 		return ETrue;
 		}
 	else if (iExtendedAttributes & EUserDefinedFilter2) 
 		{
 		aContactFieldFlag = EContactFieldFlagFilterable2;
 		return ETrue;
 		}
 	else if (iExtendedAttributes & EUserDefinedFilter3) 
 		{
 		aContactFieldFlag = EContactFieldFlagFilterable3;
 		return ETrue;
 		}
 	else if (iExtendedAttributes & EUserDefinedFilter4) 
 		{
 		aContactFieldFlag = EContactFieldFlagFilterable4;
 		return ETrue;
 		}
 		
 	return EFalse;
  	}
  	

void CContactItemField::UsesTemplateLabel()
	{
	iAttributes&=~EOverRidesLabel;
	SetLabelUnspecified(EFalse);
	}

void CContactItemField::SetOverRidesLabel(TBool aValue)
	{
	if (aValue)
		{
		iAttributes|=EOverRidesLabel;
		}
	else
		{
		iAttributes&=~EOverRidesLabel;
		}
	}

void CContactItemField::SetUsesTemplateTypes(TBool aUsesTemplateTypes)
	{
	if (aUsesTemplateTypes)
		iAttributes|=EUsesTemplateData;
	else
		iAttributes&=~EUsesTemplateData;
	}

EXPORT_C void CContactItemField::SetLabelL(const TDesC& aLabel)
/** Sets the field label. 

The label is allocated using TDesC::AllocL(), so the function can leave. 
Any existing label is replaced.

@param aLabel The new field label. */
    {
	SetLabel(aLabel.AllocL());
    }

EXPORT_C void CContactItemField::SetLabel(HBufC* aLabel)
/** Sets the field label. 

The field takes ownership of aLabel so the function cannot leave.

@param aLabel The new field label. */
	{
	delete iLabel;
	iLabel=aLabel;
	SetLabelUnspecified(EFalse);
	iAttributes|=EOverRidesLabel;
	}

EXPORT_C void CContactItemField::SetId(TInt aId)
/** Sets the ID which uniquely identifies a field within a field set..

Note that the field ID value is initialised when the field is added to the 
field set (using CContactItemFieldSet::AddL()). It is equivalent to the field's 
index within the field set array, and should not normally be changed.

@param aId The new field ID. */
	{
	iId=aId;
	}

EXPORT_C TInt CContactItemField::Id() const
/** Gets the field ID.

@return The field ID. */
	{
	return(iId);
	}

EXPORT_C TUint CContactItemField::UserFlags() const
/** Gets the value of the user flags, as set by SetUserFlags().

@return The user flags value. */
	{
	return((iAttributes&EUserMask)>>EUserMaskShift);
	}

EXPORT_C void CContactItemField::SetUserFlags(TUint aFlags)
/** Sets the value of the user flags.

@param aFlags The user flags value. */
	{
	iAttributes&=~EUserMask;
	iAttributes|=((aFlags<<EUserMaskShift)&EUserMask);
	}

TInt CContactItemField::TemplateFieldId() const
	{
	return iTemplateFieldId;
	}

void CContactItemField::SetDeleted(TBool aDeleted)
	{
	if (aDeleted)
		iAttributes|=STATIC_CAST(TUint, EDeleted); 
	else
		iAttributes&=~EDeleted;
	}

void CContactItemField::UpdateFieldFlags(const CContactItemFieldSet& aTemplateFieldSet)
	{
	TInt templatePosition;
	TBool exactMatch;

	if (!Storage()->IsFull() && !IsTemplate())
		{
		SetDeleted(ETrue);
		}
	else
		{
		SetDeleted(EFalse);
		templatePosition = aTemplateFieldSet.MatchTemplateField(ContentType(),UserFlags(),exactMatch);
		if (templatePosition != KErrNotFound)
			{
			const CContactItemField& templateField = aTemplateFieldSet[templatePosition];
			UsesTemplateData(templateField.Id());
			SetUsesTemplateTypes(exactMatch);
			if (LabelUnspecified() || templateField.Label().CompareF(Label())==0)
				{
				UsesTemplateLabel();
				}
			}
		}
	SetLabelUnspecified(EFalse);
	}

/** 
@internalTechnology 
 */
EXPORT_C void CContactItemField::GetFieldText(TDes &aText) const 
	{
	if (StorageType()==KStorageTypeText)
		{
		TPtrC text=TextStorage()->Text();
		TInt length=text.Length();
		const TInt carriageReturn=13;
		const TInt lineFeed=10;
		for(TInt loop=0;loop<length;loop++)
  			if ((TChar(text[loop])==carriageReturn) || (TChar(text[loop])==lineFeed))
				{
				length=loop;
				break;
				}
		if (length>KTextFieldMinimalLength)
			length=KTextFieldMinimalLength;
		aText.Copy(text.Left(length));
		}
	}

EXPORT_C TBool CContactItemField::IsValidLabel(const TDesC& aLabel,TInt& aInvalidPos)
/** Tests whether a field label is valid.

Note: the label is invalid if it contains any of the following characters: 

[] (left or right square bracket) 

= (equals sign)

. (dot) 

: (colon) 

, (comma)

@param aLabel The field label to test.
@param aInvalidPos On return, contains the character position within the label 
of the first invalid character. The first character position is zero.
@return ETrue if valid, EFalse if invalid. */
	{
		return !(((aInvalidPos = aLabel.Locate(KVersitTokenLSquareBracketVal)) != KErrNotFound) ||
		((aInvalidPos = aLabel.Locate(KVersitTokenRSquareBracketVal)) != KErrNotFound) ||
		((aInvalidPos = aLabel.Locate(KVersitTokenEqualsVal)) != KErrNotFound) ||
		((aInvalidPos = aLabel.Locate(KVersitTokenColonVal)) != KErrNotFound) ||
		((aInvalidPos = aLabel.Locate(KVersitTokenPeriodVal)) != KErrNotFound) ||
		((aInvalidPos = aLabel.Locate(KVersitTokenCommaVal)) != KErrNotFound));
	}

EXPORT_C TBool CContactItemField::IsTemplateLabelField() const
/** Tests whether the field is a template label field (a field which holds 
the label for a contact card template: see class CContactCardTemplate).

@return ETrue if the field is a template label field, EFalse if not. */
	{
	return (iContentType->ContainsFieldType(KUidContactFieldTemplateLabel));
	}


/** 
Part of the system template update implementation. 
This could be used for a generic update method at a later stage.
@internalTechnology
*/
void CContactItemField::PopulateStoreL(RStoreWriteStream& aRootStream, TInt aCount, CArrayFix<TFieldHeader>& aFieldHeaderArray) const
	{
	if (!IsDeleted())
		{
		//Store attributes
        aRootStream << aFieldHeaderArray[aCount].FieldAtts();
        
		// since field is in fullFields it cannot be empty, so there is no need to check
        if (StorageType() != KStorageTypeText)
            {
            aRootStream << aFieldHeaderArray[aCount].StreamId();
            }
		
		//Store field id.
		aRootStream.WriteUint32L(iId);
		aRootStream.WriteUint32L(iTemplateFieldId);
        
		CContactItemField::THint hint(0);
        if(!UsesTemplateTypes())
            {
			CArrayFixFlat<TUid>* additionalFields = new(ELeave) CArrayFixFlat<TUid>(5);
			CleanupStack::PushL(additionalFields);
			
			for(TInt loop=0; loop < iContentType->FieldTypeCount(); loop++)
				{
				TFieldType fieldType = iContentType->FieldType(loop);
				if (!AddFieldToHint(fieldType, hint))
				    {
					additionalFields->AppendL(fieldType);
				    }
				}
				
			const TInt KAdditionalUidsCount = additionalFields->Count();
			hint.SetAdditionalUidsNum(KAdditionalUidsCount);
			
            if(iContentType->Mapping() != KNullUid)
                {
                //Store vCard mapping uid if there is a one.
                hint.SetHasVCardMappingUid();
                aRootStream.WriteInt32L(hint.iHintValue);
    			aRootStream.WriteInt32L(iContentType->Mapping().iUid); // mapping
                }
            else
                {
                aRootStream.WriteInt32L(hint.iHintValue);            
                }			
			
			for (TInt ii = 0; ii < KAdditionalUidsCount; ++ii) 
				{
				aRootStream.WriteInt32L((*additionalFields)[ii].iUid); // additional typing uids
				}
			CleanupStack::PopAndDestroy(); // additionalFields
            }    
		
		if (OverRidesLabel())
			{ // store label if not inherited from template
			
			TInt length;
			
			if(iLabel)
				{
				length = iLabel->Length();
				}
			else
				{
				length = 0;
				}			
			
			aRootStream.WriteInt32L(length);	
				
			if (length > 0)
			    {
				aRootStream << *(iLabel);
			    }
			}
		} //if (!IsDeleted())
	}


/** 
Part of the system template update implementation. 
This could be used for a generic update method at a later stage.
@since 7.0
@internalTechnology
*/
void CContactItemField::PrepareFieldAsTemplateL(CContactItemFieldSet& aSystemTemplateFieldSet)
	{
	TInt templatePosition = KErrNotFound;
	TBool exactMatch = EFalse;
	
	if ( !iStorage->IsFull() && !IsTemplate() )
		{
		SetDeleted(ETrue);
		} // if
	else
		{
		SetDeleted(EFalse);
		templatePosition = aSystemTemplateFieldSet.MatchTemplateField(ContentType(), UserFlags(), exactMatch);
		if (templatePosition != KErrNotFound)
			{
			const CContactItemField& systemTemplateField = aSystemTemplateFieldSet[templatePosition];
			UsesTemplateData( systemTemplateField.Id() );
			SetUsesTemplateTypes( exactMatch );
			if( LabelUnspecified() || systemTemplateField.Label().CompareF(Label()) == 0 )
				{
				UsesTemplateLabel();
				} 
			} //if
		} //else

	SetLabelUnspecified(EFalse);
	}

void CContactItemField::InternalizeL(RReadStream& aStream)
/** Internalises a CContactItemField object from a read stream. 
@param aStream Stream from which the object should be internalised. */
	{

	delete iContentType;
	iContentType = NULL;
	iContentType = CContentType::NewL();
	iContentType->InternalizeL(aStream);
	
	iStorageType = aStream.ReadUint32L();
	
	if(iLabel)
		{
		delete iLabel;
		iLabel = NULL;
		}
	const TInt length=aStream.ReadInt32L();
	if (length)
		{
		iLabel=HBufC::NewL(aStream,length);	
		}

	iId = aStream.ReadInt32L();
	
	iAttributes = aStream.ReadUint32L();
		
	iExtendedAttributes = aStream.ReadUint32L();
	
	delete iStorage;
	iStorage = NULL;
	ConstructStorageL();
	iStorage->InternalizeL(aStream);
	
	iTemplateFieldId = aStream.ReadInt32L();		

	}

void CContactItemField::ExternalizeL(RWriteStream& aStream) const 
/** Externalises a CContactItemField object to a write stream.
@param aStream Stream to which the object should be externalised. */
	{
	iContentType->ExternalizeL(aStream);
	
	aStream.WriteUint32L(iStorageType);
	
	if(iLabel)
		{
		
		const TInt length=iLabel->Length();
		aStream.WriteInt32L(length);	
		if (iLabel && length)
			{
			aStream<<*(iLabel);
			}
		}
		
	else
		{
		aStream.WriteInt32L(0);
		}
	
	aStream.WriteInt32L(iId);
	
	aStream.WriteUint32L(iAttributes);
		
	aStream.WriteUint32L(iExtendedAttributes);
	
	iStorage->ExternalizeL(aStream);
	
	aStream.WriteInt32L(iTemplateFieldId);
	}



//
//	class THint
//
CContactItemField::THint::THint() : iHintValue(0)
	{}

CContactItemField::THint::THint(TInt aValue)
	{
	iHintValue = aValue;
	}

TBool CContactItemField::THint::operator==(const THint& aHint) const
	{
	return (iHintValue == aHint.iHintValue);
	}

TBool CContactItemField::THint::operator!=(const THint& aHint) const	
	{
	return (iHintValue != aHint.iHintValue);
	}

TInt  CContactItemField::THint::HintType() const
	{
	return (iHintValue & KHintTypeMask);
	}

TInt CContactItemField::THint::TemplateFieldId() const
    {
    return (iHintValue & KHintTemplateFieldMask);    
    }
    
void  CContactItemField::THint::SetTemplateFieldId(TInt aTemplateFieldId)
    {
    iHintValue |= (aTemplateFieldId & KHintTemplateFieldMask);
    }

inline void CContactItemField::THint::SetHasVCardMappingUid()
    {
    iHintValue |= KHintVCardMappingMask;    
    }
    
inline TBool CContactItemField::THint::HasVCardMappingUid() const
    {
    return (iHintValue & KHintVCardMappingMask);    
    }

inline TInt CContactItemField::THint::AdditionalUidsNum() const
    {
    return ((iHintValue & KHintAdditionalMask) >> KHintAdditionalMaskShift);
    }
    
inline void CContactItemField::THint::SetAdditionalUidsNum(TInt aNumber)
    {
    iHintValue |= ((aNumber << KHintAdditionalMaskShift) & KHintAdditionalMask);       
    }
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS	
inline void CContactItemField::THint::SetIsPhone()
	{iHintValue|=KIntContactHintIsPhone;}
inline void CContactItemField::THint::SetIsMsg()
	{iHintValue|=KIntContactHintIsMsg;}
// turn off Pronunciation bit too?
inline void CContactItemField::THint::SetIsCompanyName()
	{iHintValue|=KIntContactHintIsCompanyName;}
inline void CContactItemField::THint::SetIsFamilyName()
	{iHintValue|=KIntContactHintIsFamilyName;}
inline void CContactItemField::THint::SetIsGivenName()
	{iHintValue|=KIntContactHintIsGivenName;}
inline void CContactItemField::THint::SetIsCompanyNamePronunciation()
	{iHintValue|=KIntContactHintIsCompanyName|KIntContactHintIsPronunciation;}
inline void CContactItemField::THint::SetIsFamilyNamePronunciation()
	{iHintValue|=KIntContactHintIsFamilyName|KIntContactHintIsPronunciation;}
inline void CContactItemField::THint::SetIsGivenNamePronunciation()
	{iHintValue|=KIntContactHintIsGivenName|KIntContactHintIsPronunciation;}
inline void CContactItemField::THint::SetIsAddress()
	{iHintValue|=KIntContactHintIsAddress;}
inline void CContactItemField::THint::SetIsAdditionalName()
	{iHintValue|=KIntContactHintIsAdditionalName;}
inline void CContactItemField::THint::SetIsSuffixName()
	{iHintValue|=KIntContactHintIsSuffixName;}
inline void CContactItemField::THint::SetIsPrefixName()
	{iHintValue|=KIntContactHintIsPrefixName;}
inline void CContactItemField::THint::SetStorageIsInline()
	{iHintValue|=KIntContactHintStorageInline;}
inline void CContactItemField::THint::SetIsEmail()
	{iHintValue|=KIntContactHintIsEmail;}
inline TBool CContactItemField::THint::IsPhone() const
	{return (iHintValue&KIntContactHintIsPhone);}
inline TBool CContactItemField::THint::IsMsg() const
	{return (iHintValue&KIntContactHintIsMsg);}
inline TBool CContactItemField::THint::IsCompanyName() const
	{return ((iHintValue&KIntContactHintIsCompanyNamePronunciation) == KIntContactHintIsCompanyName);}
inline TBool CContactItemField::THint::IsFamilyName() const
	{return ((iHintValue&KIntContactHintIsFamilyNamePronunciation)==KIntContactHintIsFamilyName);}
inline TBool CContactItemField::THint::IsGivenName() const
	{return ((iHintValue&KIntContactHintIsGivenNamePronunciation)==KIntContactHintIsGivenName);}
inline TBool CContactItemField::THint::IsCompanyNamePronunciation() const
	{return ((iHintValue&KIntContactHintIsCompanyNamePronunciation) == KIntContactHintIsCompanyNamePronunciation);}
inline TBool CContactItemField::THint::IsFamilyNamePronunciation() const
	{return ((iHintValue&KIntContactHintIsFamilyNamePronunciation)==KIntContactHintIsFamilyNamePronunciation);}
inline TBool CContactItemField::THint::IsGivenNamePronunciation() const
	{return ((iHintValue&KIntContactHintIsGivenNamePronunciation)==KIntContactHintIsGivenNamePronunciation);}
inline TBool CContactItemField::THint::IsAddress() const
	{return (iHintValue&KIntContactHintIsAddress);}
inline TBool CContactItemField::THint::IsAdditionalName() const
	{return (iHintValue&KIntContactHintIsAdditionalName);}
inline TBool CContactItemField::THint::IsSuffixName() const
	{return (iHintValue&KIntContactHintIsSuffixName);}
inline TBool CContactItemField::THint::IsPrefixName() const
	{return (iHintValue&KIntContactHintIsPrefixName);}
inline TBool CContactItemField::THint::IsStorageInline() const
	{return (iHintValue&KIntContactHintStorageInline);}
inline TBool CContactItemField::THint::IsEmail() const
	{return (iHintValue&KIntContactHintIsEmail);}
	
#ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__	
inline void CContactItemField::THint::SetHasAdditionalUids()
	{iHintValue|=KHintAdditionalMask;}
#endif //__SYMBIAN_CNTMODEL_USE_SQLITE__ 
inline TBool CContactItemField::THint::Contains(const THint& aHint) const
	{return (iHintValue&aHint.iHintValue);}
#endif	
//
//	class TFieldHeader
//

/*
 * Default constructor
 */
TFieldHeader::TFieldHeader()
: iFieldUid(0),
  iStreamId(KNullStreamIdValue)
    {
    }

/*
 * Overloaded constructor
 */
TFieldHeader::TFieldHeader(TContactFieldAtts aAtts, TUint32 aFieldUid, TStreamId aId)
: iAtts(aAtts),
  iFieldUid(aFieldUid),
  iStreamId(aId)
    {
    }
    
inline TInt TFieldHeader::FieldId() const
	{
	return iFieldUid;
	}

inline void TFieldHeader::SetFieldId(TInt aId) 
	{
    iFieldUid = aId;
	}

inline TContactFieldAtts TFieldHeader::FieldAtts() const
    {
    return iAtts;
    }
    
inline void TFieldHeader::SetFieldAtts(TContactFieldAtts aAtts)
    {
    iAtts = aAtts; 
    }

inline TStreamId TFieldHeader::StreamId() const
    {
    return iStreamId;    
    }
    
inline void TFieldHeader::SetStreamId(TStreamId aId)
    {
    iStreamId = aId;
    }
