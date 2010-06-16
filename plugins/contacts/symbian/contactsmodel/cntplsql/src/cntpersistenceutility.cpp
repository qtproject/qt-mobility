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

#include "cntpersistenceutility.h"
#include "dbsqlconstants.h"
#include "cntstd.h"
#include <cntfield.h>
#include <cntfldst.h>
#include <s32mem.h>
	
/**
Utility method used to read blob fields from contacts database, Provides a mechanism to 
fill a contact item with informations stored in blob fields within contact database. 
Currently we have two types of blobs within contact database: text blobs and binary blobs.
This method will forward the call to utility methods to read from text blob and from binary
blob as well.
A reference to the contact item to be fill has to be provided. A template has to be provided
if the contact item is based on a template. Template can be NULL. Also a view definition can
be provided to filter which fields are read from blob fields.

@param		aItem Contact item to be filled with information from blob fields.
@param		aView View definition specifying what item fields should be read from blob fields
@param		aTemplate Contact item representing a template based on which aItem should be read. Can be NULL
@param		aDatabase RSqlDatabase reference.
@leave		KErrNotFound if the specified contact item does not exist any more in contact database
*/	
void TCntPersistenceUtility::ReadBlobL(CContactItem& aItem, const CContactItemViewDef& aView, const CContactItem* aTemplate, RSqlDatabase& aDatabase)
	{
	ReadTextBlobL(aItem, aView, aTemplate, aDatabase);
	ReadBinaryBlobL(aItem, aView, aTemplate, aDatabase);
	}

/**
Utility method used to read text blob fields from contacts database. Provides a mechanism to
fill a contact item with informations stored in text blobs within contact database.
A reference to the contact item to be fill has to be provided. A template has to be provided
if the contact item is based on a template. Template can be NULL. Also a view definition can
be provided to filter which fields are read from blob fields.

@param 		aTextHeader reference to a read stream from which header values will be read
@param 		aTextValues reference to a descriptor from which text values will be read
@param		aItem Contact item to be filled with information from text blob field.
@param		aView View definition specifying what item fields should be read from text blob field
@param		aTemplate Contact item representing a template based on which aItem should be read. Can be NULL
@leave		KErrNotFound if the specified contact item does not exist any more in contact database
*/	
void TCntPersistenceUtility::ReadTextBlobL(CEmbeddedStore& aTextHeaderStore, TPtrC& aTextValues, CContactItem& aItem, const CContactItemViewDef& aView, const CContactItem* aTemplate)
	{
	HBufC* textFieldsBuf = aTextValues.AllocLC();
	
	if (aTemplate) 
		{
		// If a system template is provided, we create a new CContactItemFieldSet object
		// and restore it based on provided template (CContactItemField objects composing
		// template). CContactItem object will be set with the newly created CContactItemFieldSet.
		CContactItemFieldSet* original = CContactItemFieldSet::NewLC();
		RestoreTextL(*original, aTextHeaderStore, aTextHeaderStore.Root(), textFieldsBuf, aView, aTemplate);
		
		for(TInt loop = 0;loop < original->Count();loop++)
			{
			CContactItemField* additionalField = CContactItemField::NewLC((*original)[loop]);
			aItem.CardFields().AddL(*additionalField);
			CleanupStack::Pop(additionalField);
			}
		CleanupStack::PopAndDestroy(original);
		}
	else
		{
		// If there is no template provided, we will fill the CContactItemField set provided
		// in the curent CContactItem object
		RestoreTextL(aItem.CardFields(), aTextHeaderStore, aTextHeaderStore.Root(), textFieldsBuf, aView, NULL);
		}
	
	CleanupStack::PopAndDestroy(textFieldsBuf); 
	}


/**
Utility method used to read text blob fields from contacts database. Provides a mechanism to
fill a contact item with informations stored in text blobs within contact database.
A reference to the contact item to be fill has to be provided. A template has to be provided
if the contact item is based on a template. Template can be NULL. Also a view definition can
be provided to filter which fields are read from blob fields.

@param		aItem Contact item to be filled with information from text blob field.
@param		aView View definition specifying what item fields should be read from text blob field
@param		aTemplate Contact item representing a template based on which aItem should be read. Can be NULL
@param		aDatabase RSqlDatabase reference.
@leave		KErrNotFound if the specified contact item does not exist any more in contact database
*/	
void TCntPersistenceUtility::ReadTextBlobL(CContactItem& aItem, const CContactItemViewDef& aView, const CContactItem* aTemplate, RSqlDatabase& aDatabase)
	{
	HBufC* selectString = HBufC::NewLC(KSelectTwoFields().Length() + KContactTextFieldHeader().Length() + KContactTextFields().Length() + KSqlContactTableName().Length() + KContactId().Length());
	TPtr ptrSelectString = selectString->Des();
	ptrSelectString.Format(KSelectTwoFields, &KContactTextFieldHeader, &KContactTextFields, &KSqlContactTableName, &KContactId, aItem.Id());
	
	RSqlStatement selectStmt;
	CleanupClosePushL(selectStmt);
	User::LeaveIfError(selectStmt.Prepare(aDatabase, ptrSelectString));

	TInt err = selectStmt.Next();
	if(err != KSqlAtRow)
		{
		if(err == KSqlAtEnd) 
			{
			User::Leave(KErrNotFound);	
			}
		else
			{
			User::Leave(err);	
			}	
		}
	
	TPtrC8 textHeader;
	selectStmt.ColumnBinary(User::LeaveIfError(selectStmt.ColumnIndex(KContactTextFieldHeader)), textHeader);
	RDesReadStream textHeaderStream(textHeader);
	CleanupClosePushL(textHeaderStream);

	CEmbeddedStore* textHeaderStore = CEmbeddedStore::FromLC(textHeaderStream);
		
	TPtrC textFieldPtrC = selectStmt.ColumnTextL(User::LeaveIfError(selectStmt.ColumnIndex(KContactTextFields)));
	
	ReadTextBlobL(*textHeaderStore, textFieldPtrC, aItem, aView, aTemplate);

	CleanupStack::PopAndDestroy(4, selectString); // textHeaderStore, textHeaderStream, selectStmt, selectString 	
	}
	
/**
Utility method used to read binary blob fields from contacts database. Provides a mechanism to
fill a contact item with informations stored in binary blobs within contact database.
A reference to the contact item to be fill has to be provided. A template has to be provided
if the contact item is based on a template. Template can be NULL. Also a view definition can
be provided to filter which fields are read from blob fields.

@param		aBinaryHeader reference to a read stream from which header will be read
@param		aBinaryValues reference to a read stream from which binary values will be read
@param		aItem Contact item to be filled with information from binary blob field.
@param		aView View definition specifying what item fields should be read from binary blob field
@param		aTemplate Contact item representing a template based on which aItem should be read. Can be NULL
@leave		KErrNotFound if the specified contact item does not exist any more in contact database
*/		
void TCntPersistenceUtility::ReadBinaryBlobL(CEmbeddedStore& aBinaryHeaderStore, CEmbeddedStore& aBinaryBlobStore, CContactItem& aItem, const CContactItemViewDef& aView, const CContactItem* aTemplate)
	{
	if (aTemplate)
		{
		// If a system template is provided, we create a new CContactItemFieldSet object
		// and restore it based on provided template (CContactItemField objects composing
		// template).CContactItem object will be set with the newly created CContactItemFieldSet.		
		CContactItemFieldSet* original = CContactItemFieldSet::NewLC();
		RestoreBinaryL(*original, aBinaryHeaderStore, aBinaryHeaderStore.Root(), &aBinaryBlobStore, aView, aTemplate);
		
		for(TInt loop=0;loop < original->Count();loop++)
			{
			CContactItemField* additionalField = CContactItemField::NewLC((*original)[loop]);
			aItem.CardFields().AddL(*additionalField);
			CleanupStack::Pop(additionalField);
			}
		CleanupStack::PopAndDestroy(original);
		}
	else
		{
		// If there is no template provided, we will fill the CContactItemField set provided
		// in the curent CContactItem object		
		RestoreBinaryL(aItem.CardFields(), aBinaryHeaderStore, aBinaryHeaderStore.Root(), &aBinaryBlobStore, aView, NULL);
		}
	}
	
/**
Utility method used to read binary blob fields from contacts database. Provides a mechanism to
fill a contact item with informations stored in binary blobs within contact database.
A reference to the contact item to be fill has to be provided. A template has to be provided
if the contact item is based on a template. Template can be NULL. Also a view definition can
be provided to filter which fields are read from blob fields.

@param		aItem Contact item to be filled with information from binary blob field.
@param		aView View definition specifying what item fields should be read from binary blob field
@param		aTemplate Contact item representing a template based on which aItem should be read. Can be NULL
@param		aDatabase RSqlDatabase reference.
@leave		KErrNotFound if the specified contact item does not exist any more in contact database
*/	
void TCntPersistenceUtility::ReadBinaryBlobL(CContactItem& aItem, const CContactItemViewDef& aView, const CContactItem* aTemplate, RSqlDatabase& aDatabase)
	{
	HBufC* selectString = HBufC::NewLC(KSelectTwoFields().Length() + KContactBinaryFieldHeader().Length() + KContactBinaryFields().Length() + KSqlContactTableName().Length() + KContactId().Length());
	TPtr ptrSelectString = selectString->Des();
	ptrSelectString.Format(KSelectTwoFields, &KContactBinaryFieldHeader, &KContactBinaryFields, &KSqlContactTableName, &KContactId, aItem.Id());
	
	RSqlStatement selectStmt;
	CleanupClosePushL(selectStmt);
	User::LeaveIfError(selectStmt.Prepare(aDatabase, ptrSelectString));

	TInt err = selectStmt.Next();
	if(err != KSqlAtRow)
		{
		if(err == KSqlAtEnd)
			{
			User::Leave(KErrNotFound);	
			}
		else
			{
			User::Leave(err);	
			}	
		}
	
	TPtrC8 binaryHeader;
	selectStmt.ColumnBinary(User::LeaveIfError(selectStmt.ColumnIndex(KContactBinaryFieldHeader)), binaryHeader);
	RDesReadStream binaryHeaderStream(binaryHeader);
	CleanupClosePushL(binaryHeaderStream);
	CEmbeddedStore* binaryHeaderStore = CEmbeddedStore::FromLC(binaryHeaderStream);
	
	TPtrC8 binaryFields;
	selectStmt.ColumnBinary(User::LeaveIfError(selectStmt.ColumnIndex(KContactBinaryFields)), binaryFields);
	RDesReadStream binaryFieldsStream(binaryFields);
	CleanupClosePushL(binaryFieldsStream);
	CEmbeddedStore* binaryBlobStore = CEmbeddedStore::FromLC(binaryFieldsStream);

	ReadBinaryBlobL(*binaryHeaderStore, *binaryBlobStore, aItem, aView, aTemplate);	

	CleanupStack::PopAndDestroy(6, selectString); //binaryHeaderStore, binaryHeaderStrean, binaryBlobStore, binaryBlobStream, selectStmt, selectString
	}
	
	
/**
Utility method used to read text fields from blob and fill item field set,Provides a mechanism to restore a 
contact item field set from text blob field within contact database.
Blob informations are stored based on contact item field. At restore, a reference to a contact item field set
is provided. For every contact item field in the item field set, a text restore is made.

@param		aFieldSet Reference to CContactItemFieldSet. Item field set that has to be filled with informations from blob
@param		aHeaderStore Stream store containing the header informations
@param		aId Root id for the header stream store
@param		aValuesStore Read stream used to read text fields from text blob
@param		aViewDef View definition indicating which fields have to be read from blob
@param		aTemplate Template indicating if current field set should be filled based on a template
*/	
void TCntPersistenceUtility::RestoreTextL(CContactItemFieldSet& aFieldSet, CStreamStore& aHeaderStore, TStreamId aId, HBufC* textStream, const CContactItemViewDef& aViewDef, const CContactItem* aTemplate)
	{
	const TBool KIncludeFields = ( aViewDef.Use() == CContactItemViewDef::EIncludeFields );
	
	if (KIncludeFields && aViewDef.Count() == 0)
		{
		// If view definition does not contain any field we don't do anything (don't read from 
		// blob). We simply return from method without doing anything.
		// This is not an error condition
		return;	
		}
		
	RStoreReadStream stream;
	stream.OpenLC(aHeaderStore,aId);
	
	TCardinality fieldCount;
	stream>>fieldCount;
	
	TInt textFieldIndex=0;

	for (TInt ii = 0; ii < fieldCount; ++ii)
		{
		// Restore text for every CContactItemField in provided CContactItemFieldSet.
		CContactItemField* field = CContactItemField::NewLC();
		
		if(aTemplate)
		    {
    		field->RestoreFieldTypesL(stream, &(aTemplate->CardFields()));
		    }
		else
		    {
    		field->RestoreFieldTypesL(stream, NULL);
		    }    
		    
		ASSERT(field->StorageType() == KStorageTypeText);

		TBool fieldDefined = ETrue;
		if(!aViewDef.MatchesAll())
			{
			fieldDefined = (aViewDef.Find(field->ContentType()) != KErrNotFound);
			}

		if ((!((fieldDefined && KIncludeFields) || (!fieldDefined && !KIncludeFields))) || 
			(field->IsHidden() && aViewDef.Mode() == CContactItemViewDef::EMaskHiddenFields))
			{
    		CleanupStack::PopAndDestroy(field); 	
			}
		else
		    {
    		field->RestoreTextL(textStream, textFieldIndex);
			aFieldSet.AddL(*field);
    		CleanupStack::Pop(field); 	
		    }	
		    
        ++textFieldIndex;
		}
	CleanupStack::PopAndDestroy(&stream);	
	}

	
/**
Utility method used to read binary fields from blob and fill item field set,Provides a mechanism to restore a 
contact item field set from binary blob field within contact database.
Blob informations are stored based on contact item field. At restore, a reference to a contact item field set
is provided. For every contact item field in the item field set, a text restore is made.

@param		aFieldSet Reference to CContactItemFieldSet. Item field set that has to be filled with informations from blob
@param		aHeaderStore Stream store containing the header informations
@param		aId Root id for the header stream store
@param		aValuesStore Read stream used to read text fields from binary blob
@param		aViewDef View definition indicating which fields have to be read from blob
@param		aTemplate Template indicating if current field set should be filled based on a template			
*/	
void TCntPersistenceUtility::RestoreBinaryL(CContactItemFieldSet& aFieldSet, CStreamStore& aHeaderStore, TStreamId aId, CStreamStore* aBlobStore, const CContactItemViewDef& aViewDef, const CContactItem* aTemplate)
	{
	const TBool KIncludeFields = aViewDef.Use() == CContactItemViewDef::EIncludeFields;
	
	if (KIncludeFields && aViewDef.Count() == 0)
		{
		// If view definition does not contain any field we don't do anything (don't read from 
		// blob). We simply return from method without doing anything.
		// This is not an error condition		
		return;	
		}
		
	RStoreReadStream stream;
	stream.OpenLC(aHeaderStore,aId);
	TCardinality fieldCount;
	stream>>fieldCount;
	TStreamId nestedId;
	
	for (TInt ii = 0; ii < fieldCount; ++ii)
		{
		// Restore binary value for every CContactItemField in provided CContactItemFieldSet.
		CContactItemField* field = CContactItemField::NewLC();
		if(aTemplate)
			{
			nestedId = field->RestoreFieldTypesL(stream, &(aTemplate->CardFields()));
			}
		else
			{
			nestedId = field->RestoreFieldTypesL(stream, NULL);	
			}
			
		ASSERT(field->StorageType() != KStorageTypeText);
		
		TBool fieldDefined = ETrue;
		if(!aViewDef.MatchesAll())
			{
			fieldDefined = (aViewDef.Find(field->ContentType()) != KErrNotFound);
			}

		if ((!((fieldDefined && KIncludeFields) || (!fieldDefined && !KIncludeFields))) || 
			(field->IsHidden() && aViewDef.Mode() == CContactItemViewDef::EMaskHiddenFields))
			{
       		CleanupStack::PopAndDestroy(field);
			}
        else
            {
    		RStoreReadStream readStream;
        	readStream.OpenLC(*aBlobStore, nestedId);
    		field->Storage()->RestoreL(*aBlobStore, readStream);
    		CleanupStack::PopAndDestroy(&readStream);
    		aFieldSet.AddL(*field);
    		CleanupStack::Pop(field); 
            }			
		}
		
	CleanupStack::PopAndDestroy(&stream); 
	}
	
/**
Utility method used to write text and binary blob fields into write streams. The write
streams will be used to persist the blob informations in contact database.
Provides a mechanism to get information from a contact item and store them in the
right blob fields within contact database. Template can be NULL. 

@param		aTextHeader reference to a write stream in which text header will be written
@param		aTextValues reference to a write stream in which text values will be written.
			From the caller point of view this reference should be a reference to a RSqlParamWriteStream instance
@param		aBinaryHeader reference to a write stream in which binary header will be written
@param		aBinaryValues reference to a write stream in which binary values will be written.
@param		aItem Contact item to be filled with information from text blob field.
@param      aSysTemplate System template item.
*/		
void TCntPersistenceUtility::WriteBlobL(CEmbeddedStore& aTextEmbeddedStore, RWriteStream& aTextValues, CEmbeddedStore& aBinaryEmbeddedStore, CEmbeddedStore& aBinaryEmbeddedBlobStore, const CContactItem& aItem, const CContactTemplate* aSysTemplate)
	{
	CContactItemFieldSet& fieldSet = aItem.CardFields();
	CContactItemFieldSet* textFieldSet = CContactItemFieldSet::NewLC();
	CContactItemFieldSet* binaryFieldSet = CContactItemFieldSet::NewLC();
	
	for(TInt i = 0; i < fieldSet.Count(); ++i)
		{
		CContactItemField* item	= CContactItemField::NewL((aItem.CardFields())[i]);
		CleanupStack::PushL(item);
		if(item->StorageType() == KStorageTypeText)
			{
			textFieldSet->AddL(*item);	
			}
		else
			{
			binaryFieldSet->AddL(*item);	
			}	
		CleanupStack::Pop(item);	
		}
	
	TStreamId rootId = textFieldSet->StoreL(aTextEmbeddedStore, aSysTemplate, aTextValues, aBinaryEmbeddedBlobStore, NULL);// *textEmbeddedBlobStore); 
	aTextEmbeddedStore.SetRootL(rootId);
	aTextEmbeddedStore.CommitL();

	rootId = binaryFieldSet->StoreL(aBinaryEmbeddedStore, aSysTemplate, aTextValues, aBinaryEmbeddedBlobStore, NULL); 
	aBinaryEmbeddedStore.SetRootL(rootId);
	aBinaryEmbeddedStore.CommitL();
	aBinaryEmbeddedBlobStore.CommitL();
		
	CleanupStack::PopAndDestroy(2, textFieldSet);  //binaryFieldSet, textFieldSet
	}

	
/**
Utility class to read text blob from given header stream and text buffer, and
return an array of contact item fields defined in aTextDef.

The method loops through the fields of the given text definition and adds (text)
fields from text buffer which match the text definition field types.

@param aHeaderStream the read stream contains text header blob data 
@param aTextBuffer  text buffer storing text fields.
@param aTextDef The text defintion containing the fields IDs required in the
				view contact.
@param aSearchFastAccessFields Return value indicating that additional fields can
							   be found in the Fast Access fields.

@return An array of contact item fields.
*/	
void TCntPersistenceUtility::ReadTextBlobL(RReadStream& aHeaderStream, const HBufC* aTextBuffer, const CContactTextDef& aTextDef, const CContactTemplate& aSystemTemplate, RPointerArray<CContactItemField>& aFields, TBool& aSearchFastAccessFields)
	{
	TInt max = aTextDef.Count();
	for(TInt i=0; i<max; ++i)
		{
		aFields.AppendL(NULL);
		}	
	
	// Extract the number of fields from the header stream.
	TCardinality headerFieldCount;
	aHeaderStream>>headerFieldCount;
	
	TInt textFieldIndex = 0;
	const TInt KMaxTextDefFlds = aTextDef.Count();
	
	CContactItemField* itemField = CContactItemField::NewLC();
	for(TInt hdrFldIndex = 0; hdrFldIndex < headerFieldCount; ++hdrFldIndex)
		{
		// Internalization in RestoreFieldTypesL() moves headerStream on to
		// point to the next element.  RestoreFieldTypesL() knows how to format
		// a header element.
		itemField->RestoreFieldTypesL(aHeaderStream, &aSystemTemplate.CardFields());
	
		// Loop through all the text definition fields.
		for(TInt defIndex = 0; defIndex < KMaxTextDefFlds; ++defIndex)
			{
			// Check if text definition field and current header field types
			// match.
			if((itemField->ContentType().ContainsFieldType(aTextDef.At(defIndex).iFieldType) 
				|| aTextDef.At(defIndex).iFieldType == KUidContactFieldMatchAll) 
				&& itemField->StorageType() == KStorageTypeText)
				{
				// They do match but take care of duplicates.
				if(aFields[defIndex] == NULL)
					{
					itemField->ResetStore();
					// Restore field text from the searchable buffer.
					itemField->RestoreTextL(const_cast<HBufC*>(aTextBuffer), textFieldIndex);
					// Although there's an entry there may be more tables to
					// read.
					if(itemField->TextStorage()->Text().Length() == KNullBlobTextField().Length()
						&& itemField->TextStorage()->Text().Compare(KNullBlobTextField) == 0)
						{
						aSearchFastAccessFields = ETrue;
						}
						
					// Assign the array in correct sequence then clone a new one
					// to continue the search in case there's more than one of
					// this field type.			
					aFields[defIndex] = CContactItemField::NewL(*itemField);
					}
				}
			}
			
		if(itemField->StorageType() == KStorageTypeText)
			{
			++textFieldIndex;
			}
			
		// Always one left over.
		itemField->Reset();
		}
		CleanupStack::PopAndDestroy(itemField);
	}

	
/**
Reads the first text field from text fields blob based on aFieldType.  

1. If aFieldType is KUidContactFieldMatchAll: 
If the first text field found in the text fields blob is an Email field, 
then the email address is returned in aText and found exit with ETrue returned, 
If an email address is not returned, the first text field found in the text 
field blob is returned, however it is considered as the best effort text, so 
the calling function will return EFalse indicate the caller to found suitable
text in Fast Access fields.

2. If aFieldType is other than KUidContactFieldMatchAll: 
Find the first text field matching aFieldType from given text fields buffer and 
returned the field content in aText with ETrue returned, returning EFalse if it 
can't find matched field.

@param aHeaderStream The read stream contains text field header
@param aTextFieldsBuf The text fields blob buffer.
@param aSystemTemplate Reference to system template class.
@param aFieldType Field type to match the field
@param aText Text found in text fields buffer based on a FieldType. 
*/
TBool TCntPersistenceUtility::FindTxtFieldInTextBlobL(RReadStream& aHeaderStream, HBufC* aTextFieldsBuf, const CContactTemplate& aSystemTemplate, const TFieldType& aFieldType, TDes& aText)
	{
	// Extract the number of fields from the header stream.
	TCardinality headerFieldCount;
	aHeaderStream>>headerFieldCount;
	
	TInt txtFldIndex = 0;
	CContactItemField* itemField = NULL;
	// Assign to a TInt variable to avoid using the overloaded int operator on
	// each iteration - improves performance.
	TInt max = headerFieldCount;
	
	// Loop through the header fields and try to retrieve the text from the
	// searchable text buffer.
	for(TInt hdrFldIndex = 0; hdrFldIndex < max; ++hdrFldIndex)
		{
		itemField = CContactItemField::NewLC();

		// Using both header-fields and template-fields, setup the field to hold
		// the correct type UIDs.
		itemField->RestoreFieldTypesL(aHeaderStream, &aSystemTemplate.CardFields());

		// Only restore text fields - ignore all other fields.
		if(itemField->StorageType() == KStorageTypeText)
			{
			if(aFieldType == KUidContactFieldMatchAll)
				{
				// Restore the field text from the searchable text buffer.
				itemField->RestoreTextL(aTextFieldsBuf, txtFldIndex);
				
				if(itemField->ContentType().ContainsFieldType(KUidContactFieldEMail))
					{
					if(CopyMinFieldText(itemField->TextStorage()->Text(), aText))
						{
						CleanupStack::PopAndDestroy(itemField); 
						return ETrue;
						}
					}
				else if(aText.Length() == 0 && itemField->TextStorage()->Text().Length() > 0)
					{
					// If there is text in the field then make a copy of the first
					// KTextFieldMinimalLength characters.
					CopyMinFieldText(itemField->TextStorage()->Text(), aText);
					} //else if
				} 
			else if(itemField->ContentType().ContainsFieldType(aFieldType))
				{
				// Restore the field text from the searchable text buffer.
				itemField->RestoreTextL(aTextFieldsBuf, txtFldIndex);
				itemField->GetFieldText(aText);
				//CopyMinFieldText(itemField->TextStorage()->Text(), aText);	
				CleanupStack::PopAndDestroy(itemField); 
				return ETrue;
				} //else if
				
			++txtFldIndex;
			} //if
		CleanupStack::PopAndDestroy(itemField);
		} // for
	return EFalse;
	}
	

/**
Check if the field type is a identity field type.

@param aFieldType The specific field requested.

@return ETrue if the type is a identity field type.
*/
TBool TCntPersistenceUtility::IsFastAccessField(TFieldType aFieldType)	
	{
	if(  aFieldType == KUidContactFieldGivenName 
	   ||aFieldType == KUidContactFieldFamilyName
	   ||aFieldType == KUidContactFieldCompanyName
	   ||aFieldType == KUidContactFieldGivenNamePronunciation
	   ||aFieldType == KUidContactFieldFamilyNamePronunciation
	   ||aFieldType == KUidContactFieldCompanyNamePronunciation )
	    {
	    return ETrue;
	    }
	return EFalse;    
	}		


/**
Copy a maximum of KTextFieldMinimalLength characters from aSrc to aDest.

@param aSrc Source text buffer.
@param aDest Destination text buffer.

@return ETrue if copy was successfull, EFalse otherwise.
*/
TBool TCntPersistenceUtility::CopyMinFieldText(TPtrC aSrc,TDes& aDest)
	{
	TBool ret(EFalse);
	if(aSrc.Length() > 0)
		{
		TInt  length = aSrc.Length();
		if (length>KTextFieldMinimalLength)
			{
			length=KTextFieldMinimalLength;
			}	
		aDest.Copy(aSrc.Left(length));
		ret = ETrue;
		}
	return ret;
	}


/**
Get the contact hint value from type flags.

@param aTypeFlags the type flags get from contact table.

@return the contact hint value.
*/
TInt TCntPersistenceUtility::TypeFlagsToHint(TInt aTypeFlags)
	{
	return aTypeFlags & EContactHintFlags_Mask;
	}	

	
/**
Get Contact Type Uid from type flags.

@param aTypeFlags the type flags get from contact table.

@return the contact type UID.
*/
TUid TCntPersistenceUtility::TypeFlagsToContactTypeUid(TInt aTypeFlags)
	{
	TInt contactTypeFlags = aTypeFlags >> EContactType_Shift;
	TUid typeUid;
	switch(contactTypeFlags)
		{
		case EContactTypeFlags_ContactCard:
			typeUid = KUidContactCard;
			break;
		case EContactTypeFlags_Group:	
			typeUid = KUidContactGroup;
			break;
		case EContactTypeFlags_OwnCard:	
			typeUid = KUidContactOwnCard;
			break;
		case EContactTypeFlags_ICCEntry:
			typeUid =  KUidContactICCEntry;
			break;
		case EContactTypeFlags_Template:
			typeUid =  KUidContactTemplate;
			break;
		case EContactTypeFlags_CardTemplate:
			typeUid =  KUidContactCardTemplate;
			break;
		default:
			//This is error type, something wrong in database?
			typeUid = KNullUid;
			break;
		}
		
	return typeUid;	
	}	


/**
Convert Contact Type Uid to type flags bits.

@param aTypeFlags the type flags get from contact table.

@return type flag which put in contact type.
*/
TInt TCntPersistenceUtility::ContactTypeUidToTypeFlags(TUid aContactTypeUid)
	{
	TInt contactTypeFlags = EContactTypeFlags_UnknownType;
	switch(aContactTypeUid.iUid)
		{
		case KUidContactCardValue:
    		contactTypeFlags = EContactTypeFlags_ContactCard;
			break;
		case KUidContactGroupValue:
		    contactTypeFlags = EContactTypeFlags_Group;	
			break;
		case KUidContactOwnCardValue:
		    contactTypeFlags = EContactTypeFlags_OwnCard;	
			break;
		case KUidContactICCEntryValue:
		    contactTypeFlags = EContactTypeFlags_ICCEntry;
			break;
		case KUidContactTemplateValue:
		    contactTypeFlags = EContactTypeFlags_Template;
			break;
		case KUidContactCardTemplateValue:
		    contactTypeFlags = EContactTypeFlags_CardTemplate;
			break;
		}
		
	return contactTypeFlags << EContactType_Shift;	
	}	


/**
Get regarding column name from given fast access field type

@param aFieldUid The field type uid for specific field requested.

@return fast access column name for the specific field requrested.
*/
const TDesC& TCntPersistenceUtility::GetFastAccessColumnNameById(TInt32 aFieldUid)
	{
	switch(aFieldUid)
		{
	    case KUidContactFieldGivenNameValue:
			return KContactFirstName();
	    case KUidContactFieldFamilyNameValue:
			return KContactLastName();
	    case KUidContactFieldCompanyNameValue:
			return KContactCompanyName();
	    case KUidContactFieldGivenNamePronunciationValue:
			return KContactFirstNamePrn();
	    case KUidContactFieldFamilyNamePronunciationValue:
			return KContactLastNamePrn();
	    case KUidContactFieldCompanyNamePronunciationValue:
			return KContactCompanyNamePrn();
		default:
			return KNullDesC();
		}
	}


/**
Cleanup stack call up to handle RPointerArray.

@param aArray Pointer to an RPointerArray<CContactItemField> instance.

@return None.
*/
void TCntPersistenceUtility::ResetAndDestroyRPointerArray(TAny *aArray)
	{
	RPointerArray<CContactItemField> *parray = static_cast< RPointerArray<CContactItemField> * >(aArray);
	parray->ResetAndDestroy();			
	parray->Close();
	}

