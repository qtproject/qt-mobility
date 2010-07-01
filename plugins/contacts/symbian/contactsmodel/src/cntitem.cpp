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
#include <f32file.h>

#include <cntdef.h>
#include <cntitem.h>
#include <cntfield.h>
#include <cntfldst.h>
#include <cntdb.h>

#include "cntstd.h"
#include "cntprof.h"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "cntfieldheader.h"
#endif


EXPORT_C CContactItemFieldSet* CContactItemFieldSet::NewL()
/** Allocates and constructs a new field set.

@return Pointer to the newly created field set. */
	{ // static 
	CContactItemFieldSet* self=CContactItemFieldSet::NewLC();
	CleanupStack::Pop(); // self
	return self;
	}

EXPORT_C CContactItemFieldSet* CContactItemFieldSet::NewLC()
/** Allocates and constructs a new field set.

@return Pointer to the newly created field set. This is left on the cleanup 
stack. */
	{ // static
	CContactItemFieldSet* self=new(ELeave) CContactItemFieldSet;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

EXPORT_C CContactItemFieldSet::~CContactItemFieldSet()
/** The destructor frees all resources owned by the field set, prior to its destruction. */
	{
	if (iFields)
		{
		iFields->ResetAndDestroy();
		delete iFields;
		}
	}

CContactItemFieldSet::CContactItemFieldSet()
	{}

void CContactItemFieldSet::ConstructL()
	{
	iFields=new(ELeave) CArrayPtrFlat<CContactItemField>(4);
	}

const CContactItemField* CContactItemFieldSet::FindById(TInt aId) const
	{
	const CContactItemField* field=NULL;
	for (TInt ii=0;ii<iFields->Count();ii++)
		{
		field=iFields->At(ii);
		if (field->Id()==aId)
			return(field);
		}
	return(NULL);
	}

EXPORT_C TInt CContactItemFieldSet::FindNext(TFieldType aFieldType,TInt aStartPos) const
/** Finds the next field in the field set with the specified field type.

@param aFieldType The field type of interest.
@param aStartPos The index within the field set array at which to start the 
search. By default set to KContactFieldSetSearchAll, to find the first field. 
Must be a valid array index, or the function raises a panic.
@return If found, the index of the field within the field set, or KErrNotFound 
if not found. */
	{
	return(FindNext(aFieldType,KUidContactFieldMatchAll,aStartPos));
	}

EXPORT_C TInt CContactItemFieldSet::FindNext(TFieldType aFieldType,TUid aMapping,TInt aStartPos) const
/** Finds the next field in the field set containing both the content type mapping 
and the field type specified.

@param aFieldType The field type of interest.
@param aMapping The content type mapping of interest.
@param aStartPos The index within the array at which to start the search. By 
default set to KContactFieldSetSearchAll, to find the first field. Must be 
a valid array index, or the function raises a panic.
@return If found, the index of the field within the field set, or KErrNotFound 
if not found. */
	{
	const TInt count=iFields->Count();
	TInt pos=(aStartPos==KContactFieldSetSearchAll? 0 : aStartPos);
	TInt ii=KErrNotFound;
	for (ii=pos;ii<count;ii++)
		{
		const CContentType& contentType=(*iFields)[ii]->ContentType();
		if (contentType.Mapping()==KUidContactFieldMatchAll || aMapping==KUidContactFieldMatchAll || contentType.Mapping()==aMapping)
			if (aFieldType==KUidContactFieldMatchAll || contentType.ContainsFieldType(aFieldType))
				return ii;
		}
	return KErrNotFound;
	}

void CContactItemFieldSet::SetFieldId(CContactItemField& aField)
	{
	TInt id=iFields->Count();
	TInt count=iFields->Count();
	FOREVER
		{
		TInt loop=0;
		for(;loop<count;loop++)
			if ((*iFields)[loop]->Id()==id)
				break;
		if (loop==count)
			{
			aField.SetId(id);
			break;
			}
		id++;
		}
	}

EXPORT_C CContactItemFieldSet& CContactItemFieldSet::AddL(CContactItemField& aField)
/** Appends a field to the field set. The field set takes ownership of the field.

@param aField Reference to the field to add to the field set. The field's 
ID is updated.
@return Reference to the current field set. */
	{
	__ASSERT_DEBUG(&aField!=NULL,Panic(ECntPanicNullPointer));
	SetFieldId(aField);
	iFields->AppendL(&aField);
	return (*this);
	}

EXPORT_C void CContactItemFieldSet::Remove(TInt aIndex)
/** Removes a field from the field set.

@param aIndex The index of the field to delete. Must be a valid index within 
the field set or the function raises a panic. */
	{
	delete (*iFields)[aIndex];
	iFields->Delete(aIndex);
	}

EXPORT_C void CContactItemFieldSet::Move(TInt aFrom, TInt aTo)
/** Changes a field's position within the field set.

Both indexes specified must be valid (i.e. between zero and Count()-1 inclusive), 
or the function raises a panic.

@param aFrom The index of the field to move.
@param aTo The index of the new position within the field set. */
	{
	CContactItemField *moveField=(*iFields)[aFrom];
	if (aFrom<aTo)
		{
		for(TInt loop=aFrom;loop<aTo;loop++)
			(*iFields)[loop]=(*iFields)[loop+1];
		}
	else
		{
		for(TInt loop=aFrom;loop>aTo;loop--)
			(*iFields)[loop]=(*iFields)[loop-1];
		}
	(*iFields)[aTo]=moveField;
	}

EXPORT_C void CContactItemFieldSet::InsertL(TInt aIndex,CContactItemField& aField)
/** Inserts a field into the field set.

@param aIndex The position in the field set at which to insert the field. 
If equal to or greater than the number of elements in the field set, the field 
is appended to the field set.
@param aField Reference to the field to add to the field set. The field's ID 
is updated. */
	{
	__ASSERT_DEBUG(&aField!=NULL,Panic(ECntPanicNullPointer));
	SetFieldId(aField);
	TInt count=iFields->Count();
	if (count>aIndex)
		iFields->InsertL(aIndex,&aField);
	else
		iFields->AppendL(&aField);
	}

/**

Find and return the index of the best matched template field. The method uses a matching algorithm which compares the content type of the template fields with that of the incoming contact item.

Matching Algorithm Steps:
========================

The algorithm is based on a scoring mechanism counting 'hits' and 'misses' in order to find the best possible field match within the fieldset of the template.

1. Loop through all the contact item fields of the template.

For each template field:

2. Perform a strict, or exact, match between the vCard mappings of the template field and the field to be matched.
3. If the vCard mappings match exactly, compare User Flags values and amend the overall score accordingly.
4. Loop through all the field types (TFieldType array) stored as part of the content type of the field to be matched.

For each field type of the field to be matched:

5. Loop through the field types of the iterated template field to find a field type match. Amend the 'hits' and 'misses' count accordingly.
6. Calculate the overall score weighing the 'hits' and 'misses' count.
7. Amend the overall score count in order to account for special cases (e.g. user defined fields)
8. Compare the overall score with the best score. If score > current best score, replace the 'bestScore' count and 'bestMatch' index.

9. Return the 'bestMatch' field index of the template field.

@param aContentType		The content type of the field to find a match for.
@param aUserFlags		The user flags of the field to find a match for.
@param aExactMatch		Boolean to indicate an exact field match within the template.

@return The index of the best matched template field, if found, else KErrNotFound.

*/

TInt CContactItemFieldSet::MatchTemplateField(const CContentType& aContentType,TUint aUserFlags,TBool &aExactMatch) const
	{
	const TInt count=Count();
	TInt bestScore=5;	// Has to be better than one or fail the match
	TInt bestMatch=KErrNotFound;
	TInt contentTypeCount=aContentType.FieldTypeCount();

	// Loop through all the template fields to find the best field match
	for (TInt ii=0;ii<count;ii++)
		{
		const CContactItemField& field=*(*iFields)[ii];
		const CContentType &fieldContentType=field.ContentType();

		// Strict matching executed on the vCard mappings in order to avoid matching non-related fields e.g. Fax and Address fields.
		// NOTE: The vCard mappings are intended to match the vCard PROPERTY (e.g. TEL in TEL;HOME;FAX:01234), wherein the field is to be exported into a vCard.
		if (fieldContentType.Mapping()==aContentType.Mapping())
			{
			TInt score=fieldContentType.Mapping()==KUidContactFieldVCardMapUnusedN?0:100;
			TInt hits=0;
			TInt misses=0;

			// The criteria for an exact match of the User Flags has been changed
			// from a mandatory requirement to a preference within the scoring system
			if (aUserFlags==0 || field.UserFlags()==aUserFlags)
				{
				++score;
				}

			// NOTE: A common Field Types array stores BOTH the Field Type in addition to the Extra Mappings
			//
			// A. Field Type     = Contact item field's type defined internally
			// B. Extra Mappings = Refer to the vCard PARAMETERS (e.g. FAX and HOME in TEL;HOME;FAX:01234)
			//
			// The Field Type and Extra Mappings are defined as part of the FIELD structure within the template (cntmodel.rss)
			//
			// C. Field Types array = A (Field Type, first element) + B (Extra Mappings, all elements after the first element)
			
			// Loop through the incoming contact item field's Field Types (C)
			for(TInt contactFieldTypeLoop=0; contactFieldTypeLoop < contentTypeCount; ++contactFieldTypeLoop)
				{
				// Match EACH of the incoming contact item field's Field Types (C) within the template field's Field Types (C)
				if(ContainsFieldTypeMapping(fieldContentType,aContentType.FieldType(contactFieldTypeLoop)))
					{
					++hits;
					}
				else
					{
					++misses;
					}				
				}

			// Check if the incoming contact item has a vCard property mapping
			// that is also present in the current template field's Field Types.
			// This covers the (unusual) case where a template field is defined
			// to have a vCard property mapping which is also the same as one of
			// field types (which contains property parameter mappings).
			for(TInt fieldTypeIdx=0; fieldTypeIdx < fieldContentType.FieldTypeCount(); ++fieldTypeIdx)
				{
				if(fieldContentType.FieldType(fieldTypeIdx) == aContentType.Mapping())
					{
					++hits;
					}
				}
			
			misses+=fieldContentType.FieldTypeCount()-hits;
			score+=10*hits-5*misses;
			
			if (!field.UserAddedField())
				{
				score+=3;	// Non-user added field takes priority over user added fields
				}
			
			if (fieldContentType.ContainsFieldType(KUidContactFieldVCardMapWORK))
				{
				score+=1;	// Work fields take priority over home fields
				}
				
			if (score>bestScore)
				{
				aExactMatch=(misses==0);
				bestScore=score;
				bestMatch=ii;
				}
			}
		}
	return bestMatch;
	}

// Attempt to find a contact item field type within the Field Types array (Field Types array = Field Type (internal) + Extra Mappings) of the base contact item.
// NOTE: This function does not include the base contact item field's vCard mapping whilst looking for the Field Type
TBool CContactItemFieldSet::ContainsFieldTypeMapping(const CContentType& aBaseFieldContentType, const TFieldType& aContactFieldType) const
	{
	// Loop through all the template Field Types to attempt to match the individual Field Type of the incoming contact item's field
	for(TInt templateFieldTypeLoop=0; templateFieldTypeLoop < aBaseFieldContentType.FieldTypeCount(); ++templateFieldTypeLoop)
		{
		if(aBaseFieldContentType.FieldType(templateFieldTypeLoop) == aContactFieldType)
			{
			return ETrue;
			}
		}
	return EFalse;
	}

/**
@internalTechnology 
*/
EXPORT_C TStreamId CContactItemFieldSet::StoreL(CStreamStore& aStore,const CContactItem* aTemplate,RWriteStream& aTextStream,CStreamStore& aBlobStore,CContactTables* aTables)
	{
	const TInt count=Count();
	for (TInt ii=0;ii<count;ii++)
		{
		CContactItemField* field=(*iFields)[ii];
		if (aTemplate)
			{
			const CContactItemFieldSet& templateFields = aTemplate->CardFields();
			field->UpdateFieldFlags(templateFields);
			}
		else
			{
			field->SetLabelUnspecified(EFalse);
			}
		}
	return StoreL(aStore,aTextStream,aBlobStore,aTables);
	}

EXPORT_C TStreamId CContactItemFieldSet::StoreL(CStreamStore& aStore,RWriteStream& aTextStream,CStreamStore& aBlobStore)
/** Stores the field set to a stream store.
@internalTechnology
@param aStore Stream store to which the field set is written.
@param aTextStream Stream to which data stored in text fields (with a storage 
type of KStorageTypeText) is written.
@param aBlobStore Stream store to which data stored in non-text fields is written.
@return The ID of the stream store. */
	{
	//Overloaded to maintain SC.
	return (StoreL(aStore,aTextStream,aBlobStore,NULL));
	}
	
/* 
Store all contact fields in this field set into given header and data stores.

In SQLite data schema, contact field data is saved in the stream store (aStore) as:

contact-item-field-set = number-of-fields *field		
    ; <field> repeated <number-of-fields> times
 
field = contact-field-atts [ stream-id ] field-id template-id [ hint-value *mapping *additional-field ] [ label-length [ label ] ]
	; 1st optional part is there if the contact field is NOT stored as text.
	; if the field uses the template's content type, the 2nd optional part is left out
	; <additional-field> is repeated a number of times as specified by <number-of-additional-fields> inside hint-value
	; mapping is there if vcard-mapping-id-mask in hint-valute is set
	; 3rd optional part is there if there is a label which overrides the template label. <label> is left out if <label-length> is zero

contact-field-atts = attrib-store

attrib-store =	attribs field-type extended-attribs				
	; 32 bits -- order is endian-dependent

attribs	= 14BIT						
	; 14 bitfields: Hidden, ReadOnly, Synchronize, Disabled, 
	;	4 user-defined fields, OverRidesLabel, UsesTemplateData, 
	;	UserAddedField, Template, LabelUnspecified and Deleted (in that order), 
	
field-type = 4BIT						; 0..16
extended-attribs = 8BIT
    ; 8 bitfields: Private, SpeedDial, EUserDefinedFilter and 
    ; EUserDefinedFilter1-4(in that order)		

hint-value = vcard-mapping-id-mask *number-of-additional-fields *hint-value
    ; 32 bits -- order is endian-dependent
    ; hint-value and number-of-additional-fields is there if UsesTemplateData is NOT set in attribs
vcard-mapping-id-mask= 1BIT   
number-of-additional-fields = 7BIT
hint-value = 24BIT

mapping = Int32
field-id =UInt32
template-id =UInt32
stream-id =UInt32
label-length = Int32
label = Descriptor

@param aStore the stream store for data header
@param aTextStream the stream to store text data
@param aBlobStore  the stream store for blob data

@return stream id the data header stored in aStore.
*/

TStreamId CContactItemFieldSet::StoreL(CStreamStore& aStore, RWriteStream& aTextStream, CStreamStore& aBlobStore, CContactTables* /*aTables*/)
	{
	CArrayFix<TInt>* fullFields = new(ELeave) CArrayFixFlat<TInt>(4);
	CleanupStack::PushL(fullFields);  // push here to make cleanup easier
	
	CArrayFix<TFieldHeader>* fieldHeader = new(ELeave) CArrayFixFlat<TFieldHeader>(5);
	CleanupStack::PushL(fieldHeader);
	
	TInt textFieldIndex = 0;

	const TInt KCount = Count();
	for(TInt ii = 0; ii < KCount; ++ii)
	    {  
	    // find all non-empty fields
		CContactItemField* field = (*iFields)[ii];

		if (!field->IsDeleted()) 
			{
			// only save non-empty content in the blob
			fullFields->AppendL(ii); // save for later
			
			//Add the text storage field data from the field to the blob
			fieldHeader->AppendL(field->StoreL(aTextStream, aBlobStore, textFieldIndex));
			if (field->StorageType() == KStorageTypeText)
			    {
				++textFieldIndex;
			    }
			}
		}

	// adjust the hint values for non-empty fields only
	RStoreWriteStream rootStream;
	TStreamId streamId = rootStream.CreateLC(aStore);
	
	const TInt KHintCount = fieldHeader->Count();
	rootStream << TCardinality(KHintCount);
	
	for (TInt ii = 0; ii < KHintCount; ++ii)
		{
		CContactItemField* field=(*iFields)[(*fullFields)[ii]];
		field->PopulateStoreL(rootStream, ii, *fieldHeader);
		}	
		
	rootStream.CommitL();
	aStore.CommitL();
		
	CleanupStack::PopAndDestroy(3, fullFields); // rootStream, fieldHeader, fullFields
	return streamId;
	}
	
HBufC* CContactItemFieldSet::LoadTextStreamLC(RReadStream& aStream)
	{
	TInt maxlen=0;
	if (aStream.Source())
		maxlen=aStream.Source()->SizeL(); 
	maxlen=maxlen/2;
	HBufC* textStream=HBufC::NewLC(maxlen);
	if (maxlen)
		{
		TPtr des=textStream->Des();
		aStream.ReadL(des,maxlen);
		}
	return(textStream);
	}

void CContactItemFieldSet::RestoreAndAddTemplateL(CStreamStore& aStore, TStreamId aId,CStreamStore* aBlobStore,const CContactItemViewDef& aViewDef,
									const CContactItem* aTemplate,RReadStream& aReadStream,CContactTables* aContactsTable,RArray<TInt>* aEmailIdArray)
	{
	if (aViewDef.Use()==CContactItemViewDef::EIncludeFields && aViewDef.Count()==0)
		return; // zzz temp kludge, fix this properly
	if (aTemplate)
		{
		CContactItemFieldSet *original=CContactItemFieldSet::NewLC();
		original->RestoreL(aStore,aId,aBlobStore,aViewDef,aTemplate,aReadStream,aContactsTable,aEmailIdArray);
		TInt count=original->Count();
		for(TInt loop=0;loop<count;loop++)
			{
			CContactItemField* additionalField=CContactItemField::NewLC((*original)[loop]);
			AddL(*additionalField);
			CleanupStack::Pop(); // additionalField
			}
		CleanupStack::PopAndDestroy(); // original
		}
	else
		RestoreL(aStore,aId,aBlobStore,aViewDef,NULL,aReadStream,aContactsTable,aEmailIdArray);
	}

/** 
Restores the field set using a view definition.
@internalTechnology
@param aStore Store from which to restore the field set.
@param anId The root stream ID of aStore.
@param aBlobStore If specified, the stream store from which data stored in 
non-text fields is read.
@param aViewDef The view definition to use.
@param aReadStream Read stream from which data stored in text fields (with 
a storage type of KStorageTypeText) is read. 
*/
EXPORT_C void CContactItemFieldSet::RestoreL(CStreamStore& aStore, TStreamId anId,CStreamStore* aBlobStore,const CContactItemViewDef& aViewDef,RReadStream& aReadStream)
    {
	//THIS FUNCTION IS TO BE REMOVED....
	RestoreL(aStore,anId,aBlobStore,aViewDef,NULL,aReadStream,NULL,NULL);
	}


void CContactItemFieldSet::RestoreL(CStreamStore& aStore, TStreamId anId, CStreamStore* aBlobStore,const CContactItemViewDef& aViewDef,const CContactItem* aTemplate,RReadStream& aReadStream,CContactTables* /*aContactsTable*/, RArray<TInt>* /*aEmailIdArray*/)
	// Overloaded looks in the Contacts Table for cetain fields.
	{
	const TBool includeFields=aViewDef.Use()==CContactItemViewDef::EIncludeFields;
	if (includeFields && aViewDef.Count()==0)
		return;
	RStoreReadStream stream;
	stream.OpenLC(aStore,anId);
	TCardinality fieldCount;
	stream>>fieldCount;
	TStreamId nestedId;
	TInt textFieldIndex=0;
	HBufC *textStream=CContactItemFieldSet::LoadTextStreamLC(aReadStream);

	for (TInt ii=0; ii<fieldCount; ++ii)
		{
		CContactItemField* field=CContactItemField::NewLC();
		nestedId=field->RestoreFieldTypesL(stream,aTemplate?&(aTemplate->CardFields()):NULL);
		const CContentType &type=field->ContentType();
		TBool fieldDefined=aViewDef.MatchesAll() || (aViewDef.Find(type)!=KErrNotFound);
		if (!((fieldDefined&&includeFields) || (!fieldDefined&&!includeFields)) || 
			(field->IsHidden() && aViewDef.Mode()==CContactItemViewDef::EMaskHiddenFields)
			)
			{
			if (field->StorageType()==KStorageTypeText)  
				textFieldIndex++;
			CleanupStack::PopAndDestroy(); // field
			}
		else
			{
			if (field->StorageType()==KStorageTypeText)  // restore from text stream
				{
				// restore text from contacts table.
				TPtrC textPtr;
				if(textPtr.Length())
					{
					field->TextStorage()->SetTextL(textPtr);
					}
				else
					{
					field->RestoreTextL(textStream,textFieldIndex);
					}
				iFields->AppendL(field);
				CleanupStack::Pop(); // field
				textFieldIndex++;
				}
			else if (aBlobStore)
				{
				field->RestoreDataL(*aBlobStore, nestedId);
				iFields->AppendL(field);
				CleanupStack::Pop(); // field
				}
			else
				{
				CleanupStack::PopAndDestroy(); // field
				}
			}
		}
	CleanupStack::PopAndDestroy(2); // textStream,stream
    }

/**
The overload that doesn't use dangerous CContactItemFieldSet::LoadTextStreamLC(aReadStream) method.
LoadTextStreamLC relies on the stream to know its size, but now all types of streams support this functionality.
In order to avoid calling LoadTextStreamLC, aTextBuf is already loaded in HBufC in the caller function.
The caller function had access to the table and can use ColLength() API to get the lebgth of the field.
@internalTechnology
*/
EXPORT_C void CContactItemFieldSet::RestoreL(CStreamStore& aStore, TStreamId anId, CStreamStore* aBlobStore,const CContactItemViewDef& aViewDef,const CContactItem* aTemplate,HBufC* aTextBuf)
	{
	const TBool includeFields=aViewDef.Use()==CContactItemViewDef::EIncludeFields;
	if (includeFields && aViewDef.Count()==0)
		return;
	RStoreReadStream stream;
	stream.OpenLC(aStore,anId);
	TCardinality fieldCount;
	stream>>fieldCount;
	TStreamId nestedId;
	TInt textFieldIndex=0;

	for (TInt ii=0;ii<fieldCount;ii++)
		{
		CContactItemField* field=CContactItemField::NewLC();
		nestedId=field->RestoreFieldTypesL(stream,aTemplate?&(aTemplate->CardFields()):NULL);
		const CContentType &type=field->ContentType();
		TBool fieldDefined=aViewDef.MatchesAll() || (aViewDef.Find(type)!=KErrNotFound);
		if (!((fieldDefined&&includeFields) || (!fieldDefined&&!includeFields)) || 
			(field->IsHidden() && aViewDef.Mode()==CContactItemViewDef::EMaskHiddenFields)
			)
			{
			if (field->StorageType()==KStorageTypeText)  
				textFieldIndex++;
			CleanupStack::PopAndDestroy(field);
			}
		else
			{
			if (field->StorageType()==KStorageTypeText)  // restore from text stream
				{
				// restore text from contacts table.
				TPtrC textPtr;
				if(textPtr.Length())
					{
					field->TextStorage()->SetTextL(textPtr);
					}
				else
					{
					field->RestoreTextL(aTextBuf,textFieldIndex);
					}

                if (aTemplate)
                    {
                    AddL(*field);
                    }
                else
                    {
                    iFields->AppendL(field);
                    }

				CleanupStack::Pop(field);
				textFieldIndex++;
				}
			else if (aBlobStore)
				{
				field->RestoreDataL(*aBlobStore, nestedId);

                if (aTemplate)
                    {
                    AddL(*field);
                    }
                else
                    {
                    iFields->AppendL(field);
                    }

				CleanupStack::Pop(field);
				}
			else
				{
				CleanupStack::PopAndDestroy(field);
				}
			}
		}
	CleanupStack::PopAndDestroy(&stream); 
    }



EXPORT_C void CContactItemFieldSet::UpdateFieldL(const CContactItemField &aField, TInt aMatchCount)
/** Updates a field in the field set. The field whose content type matches aField's 
content type is replaced by aField. The second parameter is used to identify 
which matching field is updated. Specify 1 to update the first matching field, 
2 for the second and so on. If there is no field in the field set which matches 
aField's content type, then aField is appended to the field set.

@param aField The new field information.
@param aMatchCount Identifies which matching field in the field set should 
be updated. */
	{
	const CContentType &newContentType=aField.ContentType();
	TInt fieldPos=0;
	for(;fieldPos<iFields->Count();fieldPos++)
		{
		if (newContentType==(*iFields)[fieldPos]->ContentType())
			{
			--aMatchCount;
			if (aMatchCount==0)
				{
				Remove(fieldPos); 
				break;
				}
			}
		}
	if (aField.Storage()->IsFull())
		{
		CContactItemField* field=CContactItemField::NewLC(aField);
		InsertL(fieldPos,*field);
		CleanupStack::Pop();	// field
		}
	}


EXPORT_C void CContactItemFieldSet::UpdateFieldSyncL(const CContactItemField& aField, TInt aMatchCount)
/**
 *  Updates a field in the field set. 
 *  Searches the set for a matching field. Deletes the matching set's field if any.
 *  Adds the aField to the set anyway.
 *  Uses the IsEqualForSyncUpdate() function to compare two CContentType objects,
 *  rather than the default equals operator.
 *  Takes care to hold the PREF, VoiceDial, and SpeedDial properties in the updated field.  
 *
 *  @param aField Specifies both the type of the field to update and the updated field's value.
 *  @param aMatchCount Gives the offset of the matching field to update in the order of the set
 *          (i.e. 1, means update the first matching field in the set;
 *                2, means update the second matching field in the set).
 */
	{
	TBool mustAddFieldTypePREF=EFalse;
	TBool mustAddFieldTypeVoiceDial=EFalse;
	TUid voiceDialUid=KNullUid;
		
	const CContentType& newContentType=aField.ContentType();
	TInt fieldPos=0;
	for(;fieldPos<iFields->Count();fieldPos++)
		{
		if ((*iFields)[fieldPos]->ContentType().IsEqualForSyncUpdate(newContentType))
			{
			--aMatchCount;
			if (aMatchCount==0)
				{
				const CContentType& contentType=(*iFields)[fieldPos]->ContentType(); 

				if (contentType.Mapping().iUid==KIntContactFieldVCardMapTEL)
					{
					for(TInt loop=0;loop<contentType.FieldTypeCount();loop++)
						{
						if (contentType.FieldType(loop).iUid>=KUidSpeedDialOneValue && contentType.FieldType(loop).iUid<=KUidSpeedDialNineValue)
							{
							voiceDialUid.iUid=contentType.FieldType(loop).iUid;
							break; //only one speed dial property allowed for a field
							}
						}
					
					if (contentType.ContainsFieldType(KUidContactsVoiceDialField))
						{
						mustAddFieldTypeVoiceDial=ETrue;
						}
					}

				if (contentType.ContainsFieldType(KUidContactFieldVCardMapPREF) && !newContentType.ContainsFieldType(KUidContactFieldVCardMapPREF))
					{
					mustAddFieldTypePREF=ETrue;
					}	
				break;
				}
			}
		}
	if (aField.Storage()->IsFull())
		{
		if( fieldPos<iFields->Count() )
			{
			Remove(fieldPos);//if updating with full field, then remove old field first
			}
		CContactItemField* field=CContactItemField::NewLC(aField);
		if (mustAddFieldTypePREF)
			{
			field->AddFieldTypeL(KUidContactFieldVCardMapPREF);
			}

		if (mustAddFieldTypeVoiceDial)
			{
			field->AddFieldTypeL(KUidContactsVoiceDialField);
			}

		if (voiceDialUid!=KNullUid)
			{
			field->AddFieldTypeL(voiceDialUid);
			}

		InsertL(fieldPos,*field);
		CleanupStack::Pop(field);	
		}
	else if( fieldPos < iFields->Count() )
		{
		(*iFields)[fieldPos]->ResetStore();//if updating with empty field, just reset the field
		}
	}



TInt CContactItemFieldSet::FieldText(TFieldType aFieldType, TDes  &aText, TInt aStartPosition) const
	{
	TInt fieldPos=FindNext(aFieldType, aStartPosition);
	if (fieldPos!=KErrNotFound)
		(*iFields)[fieldPos]->GetFieldText(aText);
	return fieldPos;
	}

/** Get the first non-empty text field*/
void CContactItemFieldSet::NonZeroFieldText(TFieldType aFieldType, TDes &aText) const
	{
	TInt fieldPos=-1;
	do
		{
		fieldPos=FindNext(aFieldType,fieldPos+1);
		if (fieldPos==KErrNotFound)
			break;
		(*iFields)[fieldPos]->GetFieldText(aText);
		} while(aText.Length()==0);
	}


/** 
Part of the system template update implementation. 
This could be used for a generic update method at a later stage.
@since 7.0
@internalTechnology
*/
CArrayFix<TFieldHeader>* CContactItemFieldSet::ConstructFieldHeaderArrayLC(RWriteStream& aTextStream, CStreamStore& aBlobStore)
	{
	CArrayFix<TFieldHeader>* retval = new(ELeave) CArrayFixFlat<TFieldHeader>(5);
	CleanupStack::PushL(retval);
	CContactItemField* field = NULL;
	TPtrC textPointer;
	HBufC* buffer = NULL;
	TInt counter = 0;
	TInt maxFields = 0;
	TInt textFieldIndex = 0;

	maxFields = Count();
	for (counter=0;counter < maxFields; counter++)
		{
		field = (*iFields)[counter];
		if (!field->IsDeleted() )
			{
			if (field->StorageType()==KStorageTypeText)
				{
				textPointer.Set(field->TextStorage()->Text());
				buffer = textPointer.AllocLC();
				} // if

			retval->AppendL(field->StoreL(aTextStream,aBlobStore,textFieldIndex));

			if (buffer && buffer->Length() > 0 )
				{
				field->TextStorage()->SetTextL(*buffer);
				}

			if (buffer)
				{
				CleanupStack::PopAndDestroy(buffer);
				buffer = NULL;
				}

			if (field->StorageType()==KStorageTypeText)
				{
				textFieldIndex++;
				}
			}//if
		} // for

	return retval;
	}



/*
 * Iterate through the fields.
 * If the current field matches the field we are inserting from the template 
 * return the position of the match. 
 * 
 * @param aSystemTemplateFields System template fieldset
 * @param aStartIndex Field to start from
 * @param aField Template field
 * @return KErrNotFound, if no field matching the template field in the fieldset, 
 * else returns position of match in fieldset
 */

TInt CContactItemFieldSet::FieldTypeCount(const CContactItemFieldSet& aSystemTemplateFields, TInt aStartIndex, const CContactItemField& aField) const
	{
	const TInt fieldSetCount = Count();
	for (TInt ii=aStartIndex;ii<fieldSetCount;ii++)
		{
		const CContactItemField& field=*((*iFields)[ii]);
		TInt tfId = field.TemplateFieldId();
		if(tfId==KNullFieldId)
			{
			if (field.ContentType()==aField.ContentType())
				return(ii);
			}
		else
			{
			const CContactItemField* sysField=aSystemTemplateFields.FindById(tfId);
			if (sysField->ContentType()==aField.ContentType())
   				return(ii);
			}
		}		
	return KErrNotFound;
	}
	
void CContactItemFieldSet::InternalizeL(RReadStream& aStream)
/** Internalises a CContactItemFieldSet object from a read stream. 
@param aStream Stream from which the object should be internalised. */
	{
	TInt fieldCount = aStream.ReadInt32L(); // Number of fields.	
	
	for(TInt index=0; index<fieldCount; ++index)
		{
		CContactItemField* currentField = CContactItemField::NewLC();	
		currentField->InternalizeL(aStream);	
		iFields->AppendL(currentField);
		CleanupStack::Pop(currentField);	
		}

	}

void CContactItemFieldSet::ExternalizeL(RWriteStream& aStream) const 
/** Externalises a CContactItemFieldSet object to a write stream.
@param aStream Stream to which the object should be externalised. */
	{
	TInt fieldCount = Count(); // Number of fields.
	aStream.WriteInt32L(fieldCount);	
	for(TInt index=0; index<fieldCount; ++index)
		{
			(*iFields)[index]->ExternalizeL(aStream);
			
		}
	}		


/*
 * Constructor. 
 * This is marked as private because CContactItem is an abstract base class.
 */
CContactItem::CContactItem()
	{
	}

/** Constructs a new contact item based on a RReadStream.
@param aStream RReadStream containing object to internalize.
@return Pointer to the newly created CContactItem. This is left on the cleanup stack. 
@internalTechnology 
*/	
CContactItem* CContactItem::NewLC(RReadStream& aStream)
	{ // static
	TUid ContactItemType;
	ContactItemType.iUid = aStream.ReadInt32L();

	CContactItem* item = NewLC(ContactItemType);
	item->InternalizeL(aStream);
	
	return item;
	}	
	
/** Constructs a new contact item.

@return Pointer to the newly created CContactItem. This is left on the cleanup stack. 
@internalTechnology 
*/
EXPORT_C CContactItem* CContactItem::NewLC(TUid aType)
	{ // static
	CContactItem* item=NULL;
		
	switch(aType.iUid)
		{
		case KUidContactCardValue:
			item = CContactCard::NewLC();
			break;
			
		case KUidContactGroupValue:
			item = CContactGroup::NewLC();
			break;
						
		case KUidContactOwnCardValue:
			item = CContactOwnCard::NewLC();
			break;
			
		case KUidContactICCEntryValue:
			item=CContactICCEntry::NewL();
			CleanupStack::PushL(item);
			break;

		case KUidContactTemplateValue:
			item = CContactTemplate::NewLC();
			break;												
			
		case KUidContactCardTemplateValue:
			item = CContactCardTemplate::NewLC();
			break;	
			
		default:
			User::Leave(KErrNotSupported);
			break;																
		
		}		
		
	return item;
	}			

/*
 * Second-phase construction.
 * Set template ID on which this contact item is based as KNullContactId.
 */
void CContactItem::ConstructL()
	{
	iFieldSet=CContactItemFieldSet::NewL();
	SetTemplateRefId(KNullContactId);
	}

/*
 * Second-phase construction overload.
 */
void CContactItem::ConstructL(const CContactItem* aTemplate)
	{
	ConstructL();

	const TUid type = Type();
	if (type==KUidContactCardTemplate || type==KUidContactOwnCard || type==KUidContactGroup)
		{
		SetTemplateRefId(KNullContactId);
		}
	else if (aTemplate->TemplateRefId()==KNullContactId) // can only be template
		{//record the contacts template
		SetTemplateRefId(aTemplate->Id());
		}

	const CContactItemFieldSet& fieldSet=aTemplate->CardFields();
	const TInt fieldCount = fieldSet.Count();
	for(TInt loop=0;loop<fieldCount;loop++)
		{
		const CContactItemField& templateField=fieldSet[loop];
		if (!templateField.IsTemplateLabelField())
			{
			CContactItemField* field=CContactItemField::NewLC(templateField);
			AddFieldL(*field);
			CleanupStack::Pop(field);	
			field->CopyStorageL(templateField);
			}
		}
	}

EXPORT_C CContactItem::~CContactItem()
/** The destructor frees all resources owned by the contact item, prior to its 
destruction. */
	{
	delete iFieldSet;
	delete iGuid;
	}

EXPORT_C TContactItemId CContactItem::Id() const
/** Gets the contact item's ID.

@return The contact item's ID. */
	{
	return iId;
	}
	
/**
Sets the contact item's ID.

@internalTechnology
@released
*/	
EXPORT_C void CContactItem::SetId(TContactItemId aId)
	{
	iId = aId;
	}
	
/**
Returns the contact item's attributes.

@return contact item's attributes

@internalTechnology
@released
*/		
EXPORT_C TUint32 CContactItem::Attributes() const
	{
	return iAttributes;	
	}

/**
Sest the contact item's attributes.

@internalTechnology
@released
*/			
EXPORT_C void CContactItem::SetAttributes(TUint32 aAttributes)
	{
	iAttributes = aAttributes;	
	}
	
/**
Sest the contact item's access count.

@internalTechnology
@released
*/				
EXPORT_C void CContactItem::SetAccessCount(TUint32 aAccessCount)
	{
	iAccessCount = aAccessCount;	
	}
	
/**
Sest the contact item's creation time.

@internalTechnology
@released
*/				
EXPORT_C void CContactItem::SetCreationDate(const TTime& aTime)
	{
	iCreationDate = aTime.Int64();	
	}
	
EXPORT_C TContactItemId CContactItem::TemplateRefId() const
/** Gets the ID of the template which was used to create this contact item.

@return The ID of the template on which this item is based. KNullContactId 
if the item is not based on a template. */
	{
	return iTemplateRefId;
	}

EXPORT_C TTime CContactItem::LastModified() const
/** Gets the contact item's last modified date/time.

@return The contact item's last modified date/time. */
	{
	return iLastModified;
	}

EXPORT_C void CContactItem::SetLastModified(const TTime& aLastModified)
/** Sets the last modified date/time value stored in the local copy of the contact 
item. This value is returned by LastModified() for this copy of the item.

This function has no effect on the item's last modified date/time which is 
stored in the database; this is always the date/time the contact was last 
committed.

This function is provided for use when synchronising 
contact items.

@param aLastModified The contact item's last modified date/time. */
	{
	iLastModified=aLastModified.Int64();
	}

EXPORT_C void CContactItem::SetHidden(TBool aHidden)
/** Sets the contact item's hidden attribute. Hidden means that the item is not 
displayed if the view definition excludes hidden fields.

@param aHidden ETrue to set the hidden attribute for the item, EFalse to unset 
it. */
    	{
    	if (aHidden)
    		iAttributes|=EHidden;
    	else
    		iAttributes&=~EHidden;
    	} 
EXPORT_C void CContactItem::SetSystem(TBool aSystem)
/** Sets the contact item's system attribute. Note that the system attribute is 
not currently used in the contacts model.

@param aSystem ETrue to set the system attribute, EFalse to unset it. */
    	{
    	if (aSystem)
    		iAttributes|=ESystem;
    	else
    		iAttributes&=~ESystem;
    	}

EXPORT_C void CContactItem::SetDeleted(TBool aDeleted)
/** Sets the value of the contact item's Is deleted attribute.

If the attribute is set, this means that an attempt has been made to delete 
the contact item, but because the item's access count is greater than zero, 
its data persists and the item is just marked as deleted.

@param aDeleted ETrue to set the Is deleted attribute. EFalse to unset it. */
    	{
    	if (aDeleted)
    		iAttributes|=EDeleted;
    	else
    		iAttributes&=~EDeleted;
    	}

EXPORT_C TBool CContactItem::IsHidden() 
/** Gets the value of the contact item's hidden attribute. Hidden means that the 
item is not displayed if the view definition excludes hidden fields.

@return ETrue if hidden, EFalse if not. */
	{ return iAttributes&EHidden; }

EXPORT_C TBool CContactItem::IsSystem() 
/** Gets the value of the contact item's system attribute. Note that the system 
attribute is not currently used in the contacts model.

@return ETrue if system, EFalse if not. */
	{ return iAttributes&ESystem; }

EXPORT_C TBool CContactItem::IsDeleted()  const
/** Gets the value of the Is deleted attribute as set by SetDeleted().

@return ETrue if deleted, EFalse if not. */
	{ return iAttributes&EDeleted; }

EXPORT_C void CContactItem::SetTemplateRefId(TContactItemId aUid)
/** Sets the ID of the template on which this contact item is based.

@param aUid The ID of the template on which this item is based. */
	{
	iTemplateRefId=aUid;
	}

EXPORT_C void CContactItem::AddFieldL(CContactItemField& aField)
/** Appends a field to the contact item's field set. The contact item takes ownership 
of the field.

@param aField The field to append to the contact item's field set. */
	{
	__ASSERT_DEBUG(&aField!=NULL,Panic(ECntPanicNullPointer));
	if (this->Type()==KUidContactCardTemplate)
		aField.SetTemplateField(ETrue);
	else 
		aField.SetTemplateField(EFalse);
	iFieldSet->AddL(aField);
	}

EXPORT_C void CContactItem::RemoveField(TInt aFieldPos)
/** Removes a field from the contact item's field set. A panic occurs if the specified 
field does not exist in the field set.

@param aFieldPos Index within the field set of the field to remove. */
	{
	if (!(*iFieldSet)[aFieldPos].IsTemplateLabelField())
		iFieldSet->Remove(aFieldPos);
	}

EXPORT_C void CContactItem::InsertFieldL(CContactItemField& aField,TInt aFieldPos)
/** Inserts a field into the contact item's field set. The contact item takes ownership 
of the field.

@param aField The field to insert into the contact item's field set.
@param aFieldPos The position in the field set at which to insert the field. 
If this value is greater than the total number of fields, it is appended. 
The position is relative to zero, i.e. zero implies that the element is inserted 
at the beginning of the array. */
	{
	if (this->Type()==KUidContactCardTemplate)
		aField.SetTemplateField(ETrue);
	iFieldSet->InsertL(aFieldPos,aField);
	}

EXPORT_C void CContactItem::UpdateFieldSet(CContactItemFieldSet* aNewFieldSet)
/** Replaces the contact item's field set.

@param aNewFieldSet The new field set with which to replace the existing one. */
	{
	delete iFieldSet;
	iFieldSet = NULL;
	iFieldSet=aNewFieldSet;
	}

TContactItemId  CContactItem::Agent()
	{
	TContactItemId fieldindex=iFieldSet->FindNext(KUidContactFieldMatchAll,KUidContactFieldVCardMapAGENT);
	if (fieldindex!=KErrNotFound)
		return(TContactItemId(
		(*iFieldSet)[fieldindex].AgentStorage()->Value()));
	else
		return KNullContactId;
	}

EXPORT_C CContactItemFieldSet& CContactItem::CardFields() const
/** Gets a reference to the contact item's field set.

@return A reference to the contact item's field set. */
	{
	return *iFieldSet;
	}


EXPORT_C void CContactItem::SetUidStringL(TDesC& aString) 
/** Sets the item's UID string. This replaces any existing string. Contact items have 
a globally unique identifier, stored as a descriptor which is provided for 
vCard support. It is a combination of the database's unique identifier (see 
CContactDatabase::MachineId()), the contact item ID and the date/time of the 
contact item's creation.

@param aString The contact item's UID string. */
	{
	HBufC* guid=aString.AllocL();
	if (iGuid)
		delete iGuid;
	iGuid=guid;
	}

EXPORT_C TPtrC CContactItem::UidStringL(TInt64 aMachineUniqueId) const
/** Returns a descriptor which contains the item's UID string. Contact items have 
a globally unique identifier, stored as a descriptor. This is provided for 
vCard support. It is a combination of the database's unique identifier, the 
contact item ID and the date/time of the contact item's creation.

@param aMachineUniqueId The database's unique identifier. This can be retrieved 
using CContactDatabase::MachineId().
@return The item's UID string. */
	{
    TPtrC guid;
	if (iGuid)
		{
		if (GuidIsCompressed())
			CONST_CAST(CContactItem &,*this).MakeUidStringL(aMachineUniqueId);  
		guid.Set(*iGuid);
		}
    return guid;
	}

void CContactItem::MakeUidStringL(TInt64 aMachineUniqueId) 
	{
	HBufC* guid = NULL;
	if (iGuid->Length())
		guid=ContactGuid::CreateGuidLC(iGuid->Des(),iId,aMachineUniqueId); // pass creation time
	else
		guid=ContactGuid::CreateGuidLC(iId, aMachineUniqueId);
	delete iGuid;
	iGuid=guid;
	iAttributes&=~(ECompressedGuid);
	CleanupStack::Pop(); // iGuid
	}

/**
@internalTechnology 
*/
EXPORT_C void CContactItem::SetHasCompressedGuid(TBool aCompressed)
	{
	if (aCompressed)
		iAttributes|=ECompressedGuid;
	else
		iAttributes&=~(ECompressedGuid);
	}


TBool CContactItem::GuidIsCompressed() const
	{return iAttributes&ECompressedGuid;}



/** 
Part of the system template update implementation. 
This could be used for a generic update method at a later stage.
@since 7.0
@internalTechnology
*/
TInt CContactItem::NumberOfFieldsToStore() const
	{
	TInt retval = 0, counter = 0;
	const TInt max = iFieldSet->Count();
	for (counter = 0; counter < max; counter++)
		{
		if ( !   (*iFieldSet)[counter].IsDeleted() ) retval++;
		}
	return retval;
	}

/** 
Part of the system template update implementation. 
This could be used for a generic update method at a later stage.
@since 7.0
@internalTechnology
*/
TStreamId CContactItem::PopulateStoreL(CStreamStore& aStore, CArrayFix<TFieldHeader>& aFieldHeaderArray) const
	{
	RStoreWriteStream rootStream;
	TInt counter = 0;
	TStreamId streamId = rootStream.CreateLC(aStore);
	const TInt hintCount = aFieldHeaderArray.Count();

	rootStream <<TCardinality(NumberOfFieldsToStore());

	for (counter = 0;counter < hintCount; counter++)
		{
		(*iFieldSet)[counter].PopulateStoreL(rootStream, counter, aFieldHeaderArray);
		}
	rootStream.CommitL();
	aStore.CommitL();
	CleanupStack::PopAndDestroy(&rootStream); 
	return streamId;
	}


void CContactItem::AddLabelFieldL()
	{
	TInt pos = iFieldSet->Find(KUidContactFieldTemplateLabel);
	if (pos==KErrNotFound)	// !HasItemLabelField()
		{
		CContactItemField* labelField = CContactItemField::NewLC(KStorageTypeText);
		labelField->AddFieldTypeL(KUidContactFieldTemplateLabel);
		if (Type()==KUidContactGroup)
			{
			_LIT(KGroupLabel,"Group Label");
			labelField->SetLabelL(KGroupLabel);
			}
		else if (Type()==KUidContactCardTemplate)
			{
			_LIT(KTemplateLabel,"Template Label");
			labelField->SetLabelL(KTemplateLabel);
			}
	// field needs to be first in the list
	// bug in stream retrieval of fields
		InsertFieldL(*labelField,0);
		CleanupStack::Pop(); // labelField
		}
	else	if (pos!=0)
		{
		iFieldSet->Move(pos,0);
		}
	}


/**
 Restore fields from the template to the contact item.
@param aTemplateFields Fields to add
@param aViewDef View definition to use
@internalTechnology 
 */
EXPORT_C void CContactItem::RestoreTemplateFieldsL(const CContactItemFieldSet& aSystemTemplateFields, const CContactItemFieldSet& aTemplateFields, const CContactItemViewDef& aViewDef)
	{
	TInt insertPos=0;
	const TInt templateFieldCount=aTemplateFields.Count();
	for (TInt ii=0;ii<templateFieldCount;ii++)
		{
		const CContactItemField& templateField=aTemplateFields[ii];
		TInt fieldPos=iFieldSet->FieldTypeCount(aSystemTemplateFields, insertPos, templateField);
		if (fieldPos<0 && ((aViewDef.Mode()==CContactItemViewDef::EMaskHiddenFields && !templateField.IsHidden())
						|| (aViewDef.Mode()==CContactItemViewDef::EIncludeHiddenFields))
						&& (!templateField.ContentType().ContainsFieldType(KUidContactFieldTemplateLabel)))
			{
			CContactItemField* field = CContactItemField::NewLC(templateField);
			field->SetTemplateField(EFalse); //make sure it isn't set as a template field
			field->SetOverRidesLabel(EFalse);
			iFieldSet->InsertL(insertPos++,*field);
			CleanupStack::Pop(field); 
			}
		else 
			{
			while(fieldPos>=0)
				{
				iFieldSet->Move(fieldPos,insertPos++); //move the field to the correct position
				fieldPos=iFieldSet->FieldTypeCount(aSystemTemplateFields, insertPos, templateField);
				}
			}
		}
	}



void CContactItem::ClearFieldContent()
	{	
	for (TInt i=0;i<iFieldSet->Count();i++)
		{
		CContactItemField& field=(*iFieldSet)[i];
		if (!field.IsTemplateLabelField())
			field.ResetStore();
		}
	}	
	
void CContactItem::InternalizeL(RReadStream& aStream)
/** Internalises a CContactItem object from a read stream. 
@param aStream Stream from which the object should be internalised. */
	{
	
	delete iFieldSet;
	iFieldSet = NULL;
	iFieldSet=CContactItemFieldSet::NewL();
	iFieldSet->InternalizeL(aStream);
	
	iAttributes = aStream.ReadUint32L();
	
	iId = aStream.ReadInt32L();
	
	iTemplateRefId = aStream.ReadInt32L();
	
	TInt64 tempInt64;
	aStream >> tempInt64;
	iLastModified = TTime(tempInt64);	
	
	aStream >> tempInt64;
	iCreationDate = TTime(tempInt64);	
		
	iAccessCount = aStream.ReadUint32L();
	
	if (iGuid)
		{
		delete iGuid;		
		iGuid = NULL;
		}

	const TInt length=aStream.ReadInt32L();
	if (length)
		{
		iGuid=HBufC::NewL(aStream,length);	
		}	
				
	}

void CContactItem::ExternalizeL(RWriteStream& aStream) const 
/** Externalises a CContactItem object to a write stream.
@param aStream Stream to which the object should be externalised. */
	{
	aStream.WriteInt32L(Type().iUid);
		
	iFieldSet->ExternalizeL(aStream);
	
	aStream.WriteUint32L(iAttributes);
	
	aStream.WriteInt32L(iId);
	
	aStream.WriteInt32L(iTemplateRefId);
	
	aStream << iLastModified.Int64();
	
	aStream << iCreationDate.Int64();
		
	aStream.WriteUint32L(iAccessCount);
	
	if(iGuid)
		{
		
		const TInt length=iGuid->Length();
		aStream.WriteInt32L(length);	
		if (length)
			{		
			aStream << *(iGuid);
			}
		}
	else
		{
		aStream.WriteInt32L(0);
		}
		
	}		


//
// class CContactCardTemplate
//

EXPORT_C CContactCardTemplate* CContactCardTemplate::NewL()
	{ // static
	/** Allocates and constructs a new Contact Card Template .
	@return Pointer to the newly created Contact Card Template.     */
	CContactCardTemplate* card=CContactCardTemplate::NewLC();
	CleanupStack::Pop(); // card
	return card;
	}

EXPORT_C CContactCardTemplate* CContactCardTemplate::NewLC()
	{ // static
	/** Allocates and constructs a new Contact Card Template .
	@return  Pointer to the newly created Contact Card Template. This is left on the cleanup stack. */
	CContactCardTemplate* card=new(ELeave) CContactCardTemplate;
	CleanupStack::PushL(card);
	card->ConstructL();
	return card;
	}

EXPORT_C CContactCardTemplate* CContactCardTemplate::NewL(const CContactItem *aTemplate)
	{
    /** Allocates and constructs a new Contact Card Template .
	@param aTemplate Pointer to the template whose field set and field data are copied into the new contact card template
	@return Pointer to the newly created Contact Card Template.      */   
	
	CContactCardTemplate* card=CContactCardTemplate::NewLC(aTemplate);
	CleanupStack::Pop(); // card
	return card;
	}

EXPORT_C CContactCardTemplate* CContactCardTemplate::NewLC(const CContactItem *aTemplate)
	{ // static
    /** Allocates and constructs a new Contact Card Template .
	@param aTemplate Pointer to the template whose field set and field data are copied into the new contact card template 
	@return  Pointer to the newly created Contact Card Template. This is left on the cleanup stack. */
	
	CContactCardTemplate* card=new(ELeave) CContactCardTemplate;
	CleanupStack::PushL(card);
	card->ConstructL(aTemplate);
	return card;
	}

EXPORT_C void CContactCardTemplate::SetTemplateLabelL(const TDesC& aLabel)
/** Changes the label for a contact card template. The label is initialised when 
the template is created. The template label is stored in a text field in the 
template. This field has a unique content type mapping of KUidContactFieldTemplateLabel. 
By default, this field is the first field in the field set; it must not be 
moved from this position.

@param aLabel The new template label.
@leave KErrNotFound Indicates there is no template label field in the template. */
	{
	CContactItemFieldSet& labelFields = this->CardFields();
	// find label field
	TInt pos = labelFields.Find(KUidContactFieldTemplateLabel);
	if (pos==KErrNotFound)
		User::Leave(pos);
	labelFields[pos].TextStorage()->SetTextL(aLabel);
	}		

EXPORT_C TPtrC CContactCardTemplate::GetTemplateLabelL()
/** Gets the label for a contact card template.

@leave KErrNotFound Indicates there is no template label field in the template. 

@return The template label. */
	{
	CContactItemFieldSet& labelFields = this->CardFields();
	// find 
	TInt pos = labelFields.Find(KUidContactFieldTemplateLabel);
	if (pos==KErrNotFound)
		User::Leave(pos);
	return(labelFields[pos].TextStorage()->Text());
	}

EXPORT_C TBool CContactCardTemplate::HasItemLabelField()
	{
/** Tests whether a Template label field is present.
@return ETrue if Template Label Field is present, EFalse otherwise.  */
	CContactItemFieldSet& labelFields = this->CardFields();
	TInt pos = labelFields.Find(KUidContactFieldTemplateLabel);
	if (pos==KErrNotFound)
		return EFalse;
	return ETrue;
	}

EXPORT_C TUid CContactCardTemplate::Type() const
	{
	return KUidContactCardTemplate;
	}

CContactCardTemplate::CContactCardTemplate()
	{
	}

//
// CContactItemPlusGroup
//

/**
@internalComponent
*/
CContactItemPlusGroup::CContactItemPlusGroup()
	{}

EXPORT_C CContactItemPlusGroup::~CContactItemPlusGroup()
/** 
Frees the array of group IDs to which the derived class object 
belongs.
Frees the array of contact item IDs which identify the groups to 
which the derived class object belongs, prior to its destruction. */

	{
	delete iGroups;
	}

EXPORT_C const CContactIdArray* CContactItemPlusGroup::GroupsJoined() const
/** Returns a pointer to a list of contact item IDs which identify the groups to 
which the derived class object belongs.

@return A pointer to the array of groups IDs to which the derived class object 
belongs. NULL if the object is not a member of any groups. */
	{
	return(iGroups);
	}

EXPORT_C CContactIdArray* CContactItemPlusGroup::GroupsJoinedLC() const
/** Returns a pointer to a list of contact item IDs which identify the groups to 
which the derived class object belongs.

@return A pointer to a copy of the array of groups IDs to which the derived 
class object belongs. This array is empty if the object is not a member of 
any groups. The caller takes ownership of this object, so is responsible for 
its deletion. */
	{
	CContactIdArray* copy=NULL;
	if (iGroups)
		copy=CContactIdArray::NewLC(iGroups);
	else
		copy=CContactIdArray::NewLC();
	return copy;
	}
	
void CContactItemPlusGroup::InternalizeL(RReadStream& aStream)
/** Internalises a CContactItemPlusGroup object from a read stream. 
@param aStream Stream from which the object should be internalised. */
	{
	// Call Parent InternalizeL to internalize all members.
	CContactItem::InternalizeL(aStream);
	
	if (iGroups)
		{
		delete iGroups;		
		iGroups = NULL;
		}
	
	// TBool CheckIfExists = aStream.ReadInt32L();
	if (aStream.ReadInt32L())
		{
		iGroups = CContactIdArray::NewL();		
		iGroups->InternalizeL(aStream);
		}	
		
	}

void CContactItemPlusGroup::ExternalizeL(RWriteStream& aStream) const 
/** Externalises a CContactItemPlusGroup object to a write stream.
@param aStream Stream to which the object should be externalised. */
	{	
	// Call Parent ExternalizeL to externalize all members.
	CContactItem::ExternalizeL(aStream);
	if(iGroups)
		{
		aStream.WriteInt32L(1);
		iGroups->ExternalizeL(aStream);
		}
	else
		{
		aStream.WriteInt32L(0); // Empty group
		}		

	}		

/**
Resets groups.

@internalTechnology
@released
*/	
EXPORT_C void CContactItemPlusGroup::ResetGroups()
	{
	delete iGroups;
	iGroups = NULL;	
	}
	
/**
Sets groups.

@internalTechnology
@released
*/		
EXPORT_C void CContactItemPlusGroup::SetGroups(CContactIdArray* aGroups)
	{
	iGroups = aGroups;		
	}

//
// class CContactCard
//

EXPORT_C CContactCard::~CContactCard()
/** Frees all resources owned by the contact card, prior to its 
destruction. */
	{
	}

EXPORT_C CContactCard* CContactCard::NewL()
/** Allocates and constructs a new contact card.

@return Pointer to the newly created contact card. */
	{ // static
	CContactCard* card=CContactCard::NewLC();
	CleanupStack::Pop(); // card
	return card;
	}

EXPORT_C CContactCard* CContactCard::NewLC()
/** Allocates and constructs a new contact card.

@return Pointer to the newly created contact card. This is left on the 
cleanup stack.*/
	{ // static
	CContactCard* card=new(ELeave) CContactCard;
	CleanupStack::PushL(card);
	card->ConstructL();
	return card;
	}

EXPORT_C CContactCard* CContactCard::NewL(const CContactItem *aTemplate)
/** Allocates and constructs a new contact card whose field set is seeded from 
a template.

@param aTemplate Pointer to the template whose field set and field data are 
copied into the new contact card.
@return Pointer to the newly created contact card. */
	{
	CContactCard* card=CContactCard::NewLC(aTemplate);
	CleanupStack::Pop(); // card
	return card;
	}

EXPORT_C CContactCard* CContactCard::NewLC(const CContactItem *aTemplate)
/** Allocates and constructs a new contact card whose field set is seeded from 
a template.

@param aTemplate Pointer to the template whose field set and field data are 
copied into the new contact card.
@return Pointer to the newly created contact card. This is left on the cleanup stack. */
	{ // static
	CContactCard* card=new(ELeave) CContactCard;
	CleanupStack::PushL(card);
	card->ConstructL(aTemplate);
	return card;
	}

EXPORT_C TUid CContactCard::Type() const
/** Implements CContactItem::Type().

@return KUidContactCard. */
	{
	return KUidContactCard;
	}

EXPORT_C CContactIdArray* CContactCard::GroupsJoinedLC() const
/** Returns a pointer to a list of contact item IDs which identify the groups to 
which this contact card belongs.

@return A pointer to a copy of the array of groups IDs to which this contact 
card belongs. This array is empty if the card is not a member of any groups. 
The caller takes ownership of this object, so is responsible for its deletion. */
	{
	return(CContactItemPlusGroup::GroupsJoinedLC());
	}

CContactCard::CContactCard()
	{
	}

//
// class CContactOwnCard
//

EXPORT_C CContactOwnCard::~CContactOwnCard()
/** Frees all resources owned by the own card, prior to its destruction. */
	{
	}

EXPORT_C CContactOwnCard* CContactOwnCard::NewL()
/** Allocates and constructs a new own card. 

Note: own cards should normally be constructed using the factory functions provided 
in class CContactDatabase, for example CreateOwnCardL().

@return Pointer to the newly created own card. */
	{ // static
	CContactOwnCard* card=CContactOwnCard::NewLC();
	CleanupStack::Pop(); // card
	return card;
	}

EXPORT_C CContactOwnCard* CContactOwnCard::NewLC()
/** Allocates and constructs a new own card. 

Note: own cards should normally be constructed using the factory functions provided 
in class CContactDatabase, for example CreateOwnCardL().

@return Pointer to the newly created own card. This is left on the cleanup stack. */
	{ // static
	CContactOwnCard* card=new(ELeave) CContactOwnCard;
	CleanupStack::PushL(card);
	card->ConstructL();
	return card;
	}

EXPORT_C CContactOwnCard* CContactOwnCard::NewL(const CContactItem *aTemplate)
/** Allocates and constructs a new own card whose field set is seeded from a template. 

Note: own cards should normally be constructed using the factory functions provided 
in class CContactDatabase, for example CreateOwnCardL().

@param aTemplate Pointer to the template whose field set and field data are 
copied into the new own card.
@return Pointer to the newly created own card. */
	{
	CContactOwnCard* card=CContactOwnCard::NewLC(aTemplate);
	CleanupStack::Pop(); // card
	return card;
	}

EXPORT_C CContactOwnCard* CContactOwnCard::NewLC(const CContactItem *aTemplate)
/** Allocates and constructs a new own card whose field set is seeded from a template. 

Note: own cards should normally be constructed using the factory functions provided 
in class CContactDatabase, for example CreateOwnCardL().

@param aTemplate Pointer to the template whose field set and field data are 
copied into the new own card.
@return Pointer to the newly created own card. This is left on the cleanup stack. */
	{ // static
	CContactOwnCard* card=new(ELeave) CContactOwnCard;
	CleanupStack::PushL(card);
	card->ConstructL(aTemplate);
	return card;
	}

EXPORT_C TUid CContactOwnCard::Type() const
/** Implements CContactItem::Type(). 

@return KUidContactOwnCard. */
	{
	return KUidContactOwnCard;
	}

EXPORT_C CContactIdArray* CContactOwnCard::GroupsJoinedLC() const
/** Returns a pointer to a list of contact item IDs which identify the groups to 
which the current own card belongs.

@return A pointer to a copy of the array of groups IDs to which this own card 
belongs. This array is empty if the own card is not a member of any groups. 
The caller takes ownership of this object, so is responsible for its deletion. */
	{
	return(CContactItemPlusGroup::GroupsJoinedLC());
	}

/*
 * Own card.
 *
 * An own card is a contact card which contains information about the
 * device's owner. This can be sent to another compatible electronic
 * device as a vCard. The contact database recognises a single own card
 * referred to as the current own card; its ID is returned by
 * <code>CContactDatabase::OwnCardId()</code>. Like a contact card, an
 * own card can be a member of one or more contact card groups. The own
 * card type is identified by a UID of <code>KUidContactOwnCard</code>,
 * as returned by <code>Type()</code>.Own cards can be constructed using
 * either <code>CContactDatabase::CreateOwnCardLC()</code> or
 * <code>CreateOwnCardL()</code>. These functions create an own card,
 * based on the system template, add it to the database, set it as the
 * database's current own card and return a pointer to it. To change the
 * database's current own card, use
 * <code>CContactDatabase::SetOwnCardL()</code>.
 *
 * @since 5.2
 */
CContactOwnCard::CContactOwnCard()
	{
	}

//
// class CContactGroup
//
EXPORT_C CContactGroup* CContactGroup::NewL()
/** Allocates and constructs a new default contact card group. The group has no label and 
its list of members is NULL.

Contact groups should be constructed using the factory functions provided 
in class CContactDatabase, for example CreateContactGroupL().

@return Pointer to the new contact card group. */
	{ // static
	CContactGroup* group=CContactGroup::NewLC();
	CleanupStack::Pop();
	return group;
	}

EXPORT_C CContactGroup* CContactGroup::NewLC()
/** Allocates and constructs a new default contact card group.

The group has no label and its list of members is NULL.

Contact groups should be constructed using the factory functions provided 
in class CContactDatabase, for example CreateContactGroupLC().

@return Pointer to the new contact card group. This is left on the cleanup stack.*/
	{
	CContactGroup* group=new(ELeave) CContactGroup;
	CleanupStack::PushL(group);
	group->ConstructL();
	return group;
	}

CContactGroup::CContactGroup()
	{
	}

EXPORT_C CContactGroup::~CContactGroup()
/** The destructor frees all resources owned by the contact group, prior to its 
destruction. */
	{
	delete iItems;
	delete iInitialContactIds;
	delete iAddedContactIds;
	delete iRemovedContactIds;
	}

EXPORT_C TUid CContactGroup::Type() const
/** Implements CContactItem::Type(). 

@return KUidContactGroup. */
	{
	return KUidContactGroup;
	}

EXPORT_C CContactIdArray* CContactGroup::ItemsContainedLC() const
/** Returns a pointer to a copy of the group's list of members.

@return Pointer to a copy of the group's list of members. This is left on 
the cleanup stack. */
	{
	if (iItems)
		{
		CContactIdArray* copy = CContactIdArray::NewLC(iItems);
		return copy;
		}
	else
		{
		// iItems is NULL
		CleanupStack::PushL(iItems);
		return NULL;
		}
	}

EXPORT_C const CContactIdArray* CContactGroup::ItemsContained() const
/** Returns a constant pointer to the group's list of members.

@return Pointer to the group's list of members. */
	{
	return iItems;
	}

EXPORT_C TBool CContactGroup::IsSystem() const
/** Tests the value of the group's system attribute. 

The system attribute is not currently used in the Contacts Model API.

@return ETrue if system, EFalse if not. */
	{
	return iAttributes&ESystem;
	}

EXPORT_C void CContactGroup::SetSystem(TBool aSystem)
/** Sets the value of the group's system attribute. 

The system attribute is not currently used in the Contacts Model API.

@param aSystem ETrue to set the system attribute, EFalse to unset it. */
	{
	if (aSystem)
		iAttributes|=ESystem;
	else
		iAttributes&=~ESystem;
	}

EXPORT_C TBool CContactGroup::ContainsItem(TContactItemId aContactId)
/** Tests whether a contact item is a member of the group.

@param aContactId The ID of the contact item to test.
@return ETrue if the item is a member of the group, EFalse if not. */
	{
	if (iItems && iItems->Count())
		{
		TInt index=iItems->Find(aContactId);
		if (index==KErrNotFound)
			return EFalse;
		}
	else
		return EFalse;
	return ETrue;
	}

EXPORT_C void CContactGroup::SetGroupLabelL(const TDesC& aLabel)
/** Sets the group label, replacing any existing one.

@param aLabel The new group label.
@leave KErrNotFound The group has no label field (of type KUidContactFieldTemplateLabel). */
	{
	CContactItemFieldSet& labelFields = this->CardFields();
	// find label field
	TInt pos = labelFields.Find(KUidContactFieldTemplateLabel);
	if (pos==KErrNotFound)
		User::Leave(pos);
	labelFields[pos].TextStorage()->SetTextL(aLabel);
	}		

EXPORT_C TPtrC CContactGroup::GetGroupLabelL()
/** Gets the group label.

@leave KErrNotFound The group has no label field (of type KUidContactFieldTemplateLabel).

@return The new group label. */
	{
	CContactItemFieldSet& labelFields = this->CardFields();
	// find 
	TInt pos = labelFields.Find(KUidContactFieldTemplateLabel);
	if (pos==KErrNotFound)
		User::Leave(pos);
	return(labelFields[pos].TextStorage()->Text());
	}

EXPORT_C TBool CContactGroup::HasItemLabelField()
/** Tests whether the group has a label field (of type KUidContactFieldTemplateLabel).

@return ETrue if the group has a label field. EFalse if not. */
	{
	CContactItemFieldSet& labelFields = this->CardFields();
	TInt pos = labelFields.Find(KUidContactFieldTemplateLabel);
	if (pos==KErrNotFound)
		return EFalse;
	return ETrue;
	}

EXPORT_C CContactIdArray* CContactGroup::GroupsJoinedLC()const
/** Returns a pointer to a list of contact groups to which the current group belongs 
(a contact group can belong to other groups).

@return A pointer to a copy of the array of groups IDs to which this group 
belongs. This array is empty if the group is not a member of any other groups. 
The caller takes ownership of this object, so is responsible for its deletion. */
	{
	return(CContactItemPlusGroup::GroupsJoinedLC());
	}


/**
Adds a contact with the given Id to the group. This method doesn't make any changes to the database.
The change should be persisted by means of CContactDatabase::CommitContactL() API.

@param aContactId The Id of the contact being added to the group.
@leave KErrAlreadyExists The contact is already a member of this group.

@internalTechnology
*/
EXPORT_C void CContactGroup::AddContactL(TContactItemId aContactId)
	{
	if (!iItems)
		{
		iItems = CContactIdArray::NewL(); //No need to push on the stack because it is a member
		}
	if (iItems->Find(aContactId) == KErrNotFound)
		{
		iItems->AddL(aContactId);
		}
	else
		{
		User::Leave(KErrAlreadyExists);
		}
	}

/**
Removes a contact with the given Id from the group. This method doesn't make any changes to the database.
The change should be persisted by means of CContactDatabase::CommitContactL() API.

@param aContactId The Id of the contact being removed from the group.
@leave KErrNotFound The contact is not a member of this group.

@internalTechnology
*/
EXPORT_C void CContactGroup::RemoveContactL(TContactItemId aContactId)
	{
	TInt arrIndex(0);
	if (!iItems || KErrNotFound == (arrIndex = iItems->Find(aContactId)))
		{ //If array is empty of the contact ID is not found
		User::Leave(KErrNotFound);
		}
	iItems->Remove(arrIndex);
	}

void CContactGroup::InternalizeL(RReadStream& aStream)
/** Internalises a CContactGroup object from a read stream. 
@param aStream Stream from which the object should be internalised. */
	{
	// Call Parent InternalizeL to internalize all members.
	CContactItemPlusGroup::InternalizeL(aStream);
		
	if (iItems)
		{
		delete iItems;
		iItems = NULL;		
		}

	const TBool iItemsExists=aStream.ReadInt32L();
	if (iItemsExists)
		{
		iItems = CContactIdArray::NewL();
		iItems->InternalizeL(aStream);		
		}	
	
    if (iAddedContactIds)
        {
        delete iAddedContactIds;
        iAddedContactIds = NULL;      
        }

    const TBool iAddedContactsExists=aStream.ReadInt32L();
    if (iAddedContactsExists)
        {
        iAddedContactIds = CContactIdArray::NewL();
        iAddedContactIds->InternalizeL(aStream);      
        }	
    
    if (iRemovedContactIds)
        {
        delete iRemovedContactIds;
        iRemovedContactIds = NULL;      
        }

    const TBool iRemovedContactsExists=aStream.ReadInt32L();
    if (iRemovedContactsExists)
        {
        iRemovedContactIds = CContactIdArray::NewL();
        iRemovedContactIds->InternalizeL(aStream);      
        }
	}

void CContactGroup::ExternalizeL(RWriteStream& aStream) const 
/** Externalises a CContactGroup object to a write stream.
@param aStream Stream to which the object should be externalised. */
	{
	// Call Parent ExternalizeL to externalize all members.
	CContactItemPlusGroup::ExternalizeL(aStream);

	if(iItems)
		{
		aStream.WriteInt32L(1);
		iItems->ExternalizeL(aStream);
		}
		
	else
		{
		aStream.WriteInt32L(0);
		}
	
    if(iAddedContactIds)
        {
        aStream.WriteInt32L(1);
        iAddedContactIds->ExternalizeL(aStream);
        }
        
    else
        {
        aStream.WriteInt32L(0);
        }
    
    if(iRemovedContactIds)
        {
        aStream.WriteInt32L(1);
        iRemovedContactIds->ExternalizeL(aStream);
        }
        
    else
        {
        aStream.WriteInt32L(0);
        }    
	}	
	
/**
Resets items.

@internalTechnology
@released
*/	
EXPORT_C void CContactGroup::ResetItems()
	{
	delete iItems;
	iItems = NULL;	
	}
	
/**
Sets groups.

@internalTechnology
@released
*/		
EXPORT_C void CContactGroup::SetItems(CContactIdArray* aItems)
	{
	iItems = aItems;		
	}
	
//
// class CContactTemplate
//

EXPORT_C CContactTemplate* CContactTemplate::NewL()
/** Allocates and constructs a new default system template.

@return Pointer to the newly created system template. */
	{ // static
	CContactTemplate* item=CContactTemplate::NewLC();
	CleanupStack::Pop(); // item
	return item;
	}

EXPORT_C CContactTemplate* CContactTemplate::NewLC()
/** Allocates and constructs a new default system template.

@return Pointer to the newly created system template. This is left on the cleanup stack.*/
	{ // static
	CContactTemplate* item=new(ELeave) CContactTemplate;
	CleanupStack::PushL(item);
	item->ConstructL();
	return item;
	}

EXPORT_C CContactTemplate* CContactTemplate::NewL(const CContactItem *aTemplate)
/** Allocates and constructs a new system template seeded from another one.

@param aTemplate The contact template to copy.
@return Pointer to the newly created system template. */
	{
	CContactTemplate* item=CContactTemplate::NewLC(aTemplate);
	CleanupStack::Pop(); // item
	return item;
	}

EXPORT_C CContactTemplate* CContactTemplate::NewLC(const CContactItem *aTemplate)
/** Allocates and constructs a new system template seeded from another one.

@param aTemplate The contact template to copy.
@return Pointer to the newly created system template. This is left on the cleanup stack.*/
	{
	CContactTemplate* item=new(ELeave) CContactTemplate;
	CleanupStack::PushL(item);
	item->ConstructL(aTemplate);
	return item;
	}

EXPORT_C TUid CContactTemplate::Type() const
/** Implements CContactItem::Type(). 

@return KUidContactTemplate. */
	{
	return KUidContactTemplate;
	}

CContactTemplate::CContactTemplate()
	{
	}


EXPORT_C CContactICCEntry::~CContactICCEntry()
/** Empty destructor. */
	{
	}


/** Allocates and constructs a new ICC entry.

@param The contact template to use.
@return Pointer to newly created CContactICCEntry.  
@publishedAll
@released
*/
EXPORT_C CContactICCEntry* CContactICCEntry::NewL(const CContactItem& aTemplate)
	{
	CContactICCEntry* entry = new (ELeave) CContactICCEntry();
	CleanupStack::PushL(entry);
	entry->ConstructL(&aTemplate);
	CleanupStack::Pop(entry);
	return entry;
	}

/*
 * Allocates and constructs a new ICC entry.
 * Not exported - only used by CNTMODEL code.
 * 
 * @return Pointer to newly created CContactEntry
 * @since 7.0
 */
CContactICCEntry* CContactICCEntry::NewL()
	{
	CContactICCEntry* entry = new (ELeave) CContactICCEntry();
	CleanupStack::PushL(entry);
	entry->ConstructL();
	CleanupStack::Pop(entry);
	return entry;
	}

TUid CContactICCEntry::Type() const
/** Implements CContactItem::Type().

@return KUidContactICCEntry. */
	{
	return KUidContactICCEntry;
	}

CContactICCEntry::CContactICCEntry()
	{
	}


EXPORT_C TContactItemId ContactGuid::IsLocalContactUidString(const TDesC& aString, TInt64 aMachineUniqueId)
/** Tests whether a contact item was created in the database with the unique ID 
specified. If so, the item's contact item ID is returned. If not, returns 
KErrNotFound.

@param aString The contact item's UID string. Use CContactItem::UidStringL() 
to get a pointer to the item's UID string.
@param aMachineUniqueId The database's unique ID. This can be found by calling 
CContactDatabase::MachineId().
@return KErrNotFound if the database ID specified is not found in the UID string. 
Otherwise, the contact item's contact item ID. */
	{ // static
	TLex lex(aString);
	TInt ii=0;
	TChar c;
	TInt64 pos=KErrNotFound;
	// check machine id
	c=lex.Peek();
	if (c==KUidStringSeparator)
		;
	else
		{
		lex.Val(pos,EHex);
		if ((TInt64)pos!=aMachineUniqueId)
			return (KErrNotFound);
		}
	for(;;)
		{
		c=lex.Get();
		if (c)
			{
			if (c==KUidStringSeparator)
				{
				ii++;
				}
			else
				{
				if (ii==2) // extract the field id
					{
					lex.UnGet();
					lex.Val(pos);
					break;
					}
				}
			}
		else //EOS
			break;
		}
	return I64INT(pos);
	}

/** 
@internalTechnology 
 */
EXPORT_C TBool ContactGuid::GetCreationDate(TDes& aString, TInt64 aMachineUniqueId)
	{ // static
	if (ContactGuid::IsLocalContactUidString(aString,aMachineUniqueId)>=0)
		{
		const TInt startPos=aString.Locate(TChar(KUidStringSeparator));
		if (startPos!=KErrNotFound)
			{
			TInt endPos=aString.Length();
			while (--endPos>startPos)
				{
				if (aString[endPos]==KUidStringSeparator)
					{
					aString=aString.Left(endPos);
					aString=aString.Right(aString.Length()-startPos-1);
					return ETrue;
					}
				}
			}
		}
	return EFalse;
	}

HBufC* ContactGuid::CreateGuidLC(const TDesC& aCreationDate,TContactItemId aId, TInt64 aMachineUniqueId)
	{ // static
	HBufC* guid=HBufC::NewLC(KUidStringLength);
	TPtr ptr=guid->Des();
	ptr.AppendFormat(_L("%08x"),I64HIGH(aMachineUniqueId));
	ptr.AppendFormat(_L("%08x"),I64LOW(aMachineUniqueId));
	ptr.Append(KUidStringSeparator);
	ptr.Append(aCreationDate);
	ptr.Append(KUidStringSeparator);
	TBuf<20> localId;
	localId.Num((TInt)aId);
	ptr.Append(localId);
	return guid;
	}

HBufC* ContactGuid::CreateGuidLC(const TTime& aCreationDate,TContactItemId aId, TInt64 aMachineUniqueId)
	{ // static
	TBuf<32> time;
	TInt64 num=aCreationDate.Int64();
	time.AppendFormat(_L("%08x"),(TInt32)I64HIGH(num));
	time.AppendFormat(_L("%08x"),(TInt32)I64LOW(num));
	return ContactGuid::CreateGuidLC(time,aId,aMachineUniqueId);
	}

HBufC* ContactGuid::CreateGuidLC(TContactItemId aId, TInt64 aMachineUniqueId)
	{ // static
	TTime time;
	time.UniversalTime();
	return ContactGuid::CreateGuidLC(time,aId,aMachineUniqueId);
	}
