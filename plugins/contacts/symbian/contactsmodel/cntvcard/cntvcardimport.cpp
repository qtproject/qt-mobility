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


#include "cntvcardutils.h"

// System includes
#include <badesca.h>
#include <conlist.h>
#include <s32mem.h> 


// User includes
#include <cntfldst.h>
#include <cntfield.h>
#include <cntdef.h>
#include <cntitem.h>


// Constants
const TInt KVCardImportAddressArrayGranularity = 4;
const TInt KContactGivenName = 1;
const TInt KContactFamilyName = 0;
const TInt KContactAdditionalName = 2;
const TInt KContactPrefixName = 3;
const TInt KContactSuffixName = 4;
const TInt KContactPostOffice = 0;
const TInt KContactExtendedAddress = 1;
const TInt KContactAddress = 2;
const TInt KContactLocality = 3;
const TInt KContactRegion = 4;
const TInt KContactPostcode = 5;
const TInt KContactCountry = 6;

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
const TInt KContactMaxFieldNumber = 32;
#endif

/**
 * Delete name fields from a contact
 *
 * @param aContact Contact item
 */
void CVCardToContactsAppConverter::DeleteNameFields(CContactItem& aContact)
	{
	const CContactItemFieldSet& fieldSet = aContact.CardFields();
	//
	DeleteField(aContact, fieldSet, KUidContactFieldGivenName, KUidContactFieldVCardMapUnusedN);
	DeleteField(aContact, fieldSet, KUidContactFieldFamilyName, KUidContactFieldVCardMapUnusedN);
	DeleteField(aContact, fieldSet, KUidContactFieldAdditionalName, KUidContactFieldVCardMapUnusedN);
	DeleteField(aContact, fieldSet, KUidContactFieldPrefixName, KUidContactFieldVCardMapUnusedN);
	DeleteField(aContact, fieldSet, KUidContactFieldSuffixName, KUidContactFieldVCardMapUnusedN);
	}


/**
 * Delete a specific field from the contact card
 *
 * @param aContact The contact
 * @param aFieldSet The contact's field set
 * @param aFieldType The type of field to delete from the field set
 * @param aMapping The additional mapping which the field must contain for it to be deleted
 */
void CVCardToContactsAppConverter::DeleteField(CContactItem& aContact, const CContactItemFieldSet& aFieldSet, TFieldType aFieldType, TUid aMapping)
	{
	const TInt pos = aFieldSet.Find(aFieldType, aMapping);
	if	(pos != KErrNotFound)
		aContact.RemoveField(pos);
	}


void CVCardToContactsAppConverter::MergeSpecifiedNameFieldL(CContactItem& aContact, TFieldType aFieldType, CVCardItemAndLabel& aNames, TInt aOption, TInt aNameIndex)
/**
 * Merge specific name fields from a contact
 *
 * @param aContact Contact item to add fields to
 * @param aFieldType Field type of field to add (TFieldType)
 * @param aNames An object containing the name and labels for 'N' property fields
 * @param aOption Import preferences (available options defined in CContactDatabase::TOptions)
 * @param aNameIndex Index into aNames
 */
	{
	CContactItemFieldSet& oldfieldset=aContact.CardFields();
	const TInt pos = oldfieldset.Find(aFieldType, KUidContactFieldVCardMapUnusedN);
	const TBool processWhitespace = (aOption & CContactVCardConverter::EConnectWhitespace);

	// First check whether the field is present in the contact card
	// Also verify that the array of address sub-values actually contains a specific
	// value for the requested index.
	if (aNames.ItemCount() > aNameIndex)
		{
		const TPtrC pValue = aNames.Item(aNameIndex);
		const TInt length = pValue.Length();
		if (processWhitespace)
			{
			TBool isSingleSpace = EFalse;
			if (length == 1)
				{
				isSingleSpace = (pValue[0] == KContactVCardSpaceCharacter);
				}
			if	((pos != KErrNotFound) && (length || isSingleSpace))
				{
				// This means the PC side field is empty, so delete the corresponding device-side field.
				aContact.RemoveField(pos);
				}
			if	(length && !isSingleSpace)
				{
				// This means the PC side field is unsupported, so ignore the corresponding contents.
				TInt insertPos = 0;
				SetNameFieldL(aNames, aContact, aOption, aNameIndex, aFieldType, insertPos);
				}
			}
		else
			{
			if (pos != KErrNotFound)
				{
				// This means the PC side field is empty, so delete the corresponding device-side field.
				aContact.RemoveField(pos);
				}
			if (length)
				{
				// This means the PC side field is not empty, so add the corresponding contents.
				TInt insertPos = 0;
				SetNameFieldL(aNames, aContact, aOption, aNameIndex, aFieldType, insertPos);
				}
			}
		}
	}


void CVCardToContactsAppConverter::MergeNameFieldsL(CContactItem& aContact, CVCardItemAndLabel& aNames, TInt aOption, TBool aTreatAsPrn)
/**
 * Merge name fields from a contact
 *
 * @param aContact Contact item to add fields to
 * @param aNames An object containing the name and labels for 'N' property fields
 * @param aOption Import preferences (available options defined in CContactDatabase::TOptions)
 */
	{
	if(aTreatAsPrn)
		{
		MergeSpecifiedNameFieldL(aContact, KUidContactFieldGivenNamePronunciation,	aNames, aOption, KContactGivenName);
		MergeSpecifiedNameFieldL(aContact, KUidContactFieldFamilyNamePronunciation,	aNames, aOption, KContactFamilyName);
		}
	else
		{
		MergeSpecifiedNameFieldL(aContact, KUidContactFieldPrefixName,		aNames, aOption, KContactPrefixName);
		MergeSpecifiedNameFieldL(aContact, KUidContactFieldGivenName,		aNames, aOption, KContactGivenName);
		MergeSpecifiedNameFieldL(aContact, KUidContactFieldAdditionalName,	aNames, aOption, KContactAdditionalName);
		MergeSpecifiedNameFieldL(aContact, KUidContactFieldFamilyName,		aNames, aOption, KContactFamilyName);
		MergeSpecifiedNameFieldL(aContact, KUidContactFieldSuffixName,		aNames, aOption, KContactSuffixName);
		}
	}


void CVCardToContactsAppConverter::MergeSpecifiedAddressFieldL(CContactItem& aContact, const CVCardAddress& aAddress, const TUid& aFieldUid, const TUid& aMappingUid, TInt aAddressIndex, TInt aOption)
/**
 * Merge specific address fields from a contact
 *
 * @param aContact Contact item to add fields to
 * @param aUid The Uid of the contact
 * @param aFieldUid Contacts database Field ID
 * @param aMappingUid VCard Mapping ID
 * @param aAddress An object containing the name and labels for 'ADR' property fields
 * @param aAddressIndex Index into aAddress
 * @param aOption Import preferences (available options defined in CContactDatabase::TOptions)
 */
	{
	CContactItemFieldSet& oldfieldset = aContact.CardFields();
	TInt pos = 0;
	const TBool processWhitespace = (aOption & CContactVCardConverter::EConnectWhitespace);

	if (aAddress.Mapping() == KNullUid)
		{
		TBool fieldFound = EFalse;
		TInt startPos = KContactFieldSetSearchAll;

		while(!fieldFound && !(pos == KErrNotFound))
			{
			pos = oldfieldset.FindNext(aMappingUid, startPos);
			startPos = pos + 1;
			if (pos != KErrNotFound )
				{
				CContactItemField& tempField = oldfieldset[pos];
				const CContentType& tempContentType = tempField.ContentType();
				TBool additionalMapFound = EFalse;
				additionalMapFound |= tempContentType.ContainsFieldType(KUidContactFieldVCardMapHOME);
				additionalMapFound |= tempContentType.ContainsFieldType(KUidContactFieldVCardMapWORK);
				additionalMapFound |= tempContentType.ContainsFieldType(KUidContactFieldVCardMapPREF);
				if (!additionalMapFound)
					{
					fieldFound = ETrue;
					}
				}
			}
		}
	else
		{
		pos = oldfieldset.Find(aAddress.Mapping(), aMappingUid);
		}

	// First check whether the field is present in the contact card
	// Also verify that the array of address sub-values actually contains a specific
	// value for the requested index.
	if (aAddress.ItemCount() > aAddressIndex)
		{
		const TPtrC pValue = aAddress.Item(aAddressIndex);
		const TInt length = pValue.Length();
		if (processWhitespace)
			{
			TBool isSingleSpace = EFalse;
			if (length == 1)
				{
				isSingleSpace = (pValue[0] == KContactVCardSpaceCharacter);
				}
			if	((length || isSingleSpace) && (pos != KErrNotFound))
				{
				// This means the PC side field is empty, so delete the corresponding device-side field.
				aContact.RemoveField(pos);
				}
			if	(length && !isSingleSpace)
				{
				TInt insertPos = 0;
				SetAddressFieldL(aAddress, aContact, aOption, aAddressIndex, aFieldUid, insertPos, aMappingUid);
				}
			}
		else
			{
			if (pos != KErrNotFound)
				{
				// This means the PC side field is empty, so delete the corresponding device-side field.
				aContact.RemoveField(pos);
				}
			if (length)
				{
				// This means the PC side field is not empty, so add the corresponding contents.
				TInt insertPos = 0;
				SetAddressFieldL(aAddress, aContact, aOption, aAddressIndex, aFieldUid, insertPos, aMappingUid);
				}
			}
		}
	}


void CVCardToContactsAppConverter::MergeAddressFieldsL(CContactItem& aContact, const CVCardAddress& aAddress, TInt aOption)
/**
 * Merge a specific field from the contact card
 *
 * @param aContact Contact item to add fields to
 * @param aUid The Uid of the contact
 * @param aAddresses Address of locally stored contact
 * @param aOption Import preferences (available options defined in CContactDatabase::TOptions)
 */
	{
	MergeSpecifiedAddressFieldL(aContact, aAddress, KUidContactFieldPostOffice, KUidContactFieldVCardMapPOSTOFFICE, KContactPostOffice, aOption);
	MergeSpecifiedAddressFieldL(aContact, aAddress, KUidContactFieldExtendedAddress, KUidContactFieldVCardMapEXTENDEDADR, KContactExtendedAddress, aOption);
	MergeSpecifiedAddressFieldL(aContact, aAddress, KUidContactFieldAddress, KUidContactFieldVCardMapADR, KContactAddress, aOption);
	MergeSpecifiedAddressFieldL(aContact, aAddress, KUidContactFieldLocality, KUidContactFieldVCardMapLOCALITY, KContactLocality, aOption);
	MergeSpecifiedAddressFieldL(aContact, aAddress, KUidContactFieldRegion, KUidContactFieldVCardMapREGION, KContactRegion, aOption);
	MergeSpecifiedAddressFieldL(aContact, aAddress, KUidContactFieldPostcode, KUidContactFieldVCardMapPOSTCODE, KContactPostcode, aOption);
	MergeSpecifiedAddressFieldL(aContact, aAddress, KUidContactFieldCountry, KUidContactFieldVCardMapCOUNTRY, KContactCountry, aOption);
	}


TBool CVCardToContactsAppConverter::MergeVCardWithContactItemL(CContactItem &aContact, CParserVCard& aVCard, TUnknownPropertyBehaviour aUnknownPropertyBehaviour, TInt aOption)
/**
 * Merge a vCard with an existing contact item
 *
 * @param aContact Contact item to add fields to
 * @param aVCard vCard parser object
 * @param aUnknownPropertyBehaviour Specifies how extension properties are handled
 * @param aOption Import preferences (available options defined in CContactDatabase::TOptions)
 * @return ETrue if contact item should be deleted, EFalse otherwise
 */
	{
	TInt ii = 0;
	TInt count = 0;
	//
	TBool deleteContact = ETrue;
	iUnknownPropertyBehaviour = aUnknownPropertyBehaviour;
	TTime lastModified;
	GetVCardModifiedTimeL(aVCard, lastModified);

	// We're performing a merge
	SetImportType(ECntVCardImportTypeMerge);

	// Get Name
	CVCardItemAndLabel* names = GetContactNameLC(aVCard, aOption);
	if	(names && names->ItemCount())
		{
		MergeNameFieldsL(aContact, *names, aOption);
		deleteContact = EFalse;
		}
	CleanupStack::PopAndDestroy(names);
	// Get Name pronunciation
 	names = GetContactNamePrnLC(aVCard, aOption);
 	if (names && names->ItemCount())
 		{
 		MergeNameFieldsL(aContact, *names, aOption, ETrue);
 		}
 	CleanupStack::PopAndDestroy(names);

	// Create address container
	RPointerArray<CVCardAddress> addresses(KVCardImportAddressArrayGranularity);
	CleanupStack::PushL(TCleanupItem(CVCardItemAndLabel::CleanUpResetDestroyAndCloseArray, &addresses));

	// Get addresses from the vCard. This actually only retrieves four types of addresses:
	// HOME, WORK, PREF and now additionally, 'general' addresses
	GetAddressesL(aVCard, aOption, addresses);

	// Import each of the located address field into the contact card
	count = addresses.Count();
    for (ii=0; ii<count; ii++)
		{
		const CVCardAddress* address = addresses[ii];
		MergeAddressFieldsL(aContact, *address, aOption);
		deleteContact = EFalse;
		}

	// Finished with addresses now, so clean up
	CleanupStack::PopAndDestroy(&addresses);

	// Get Organization related information from the vCard. This actually only retrieves the Company and the Department Name
	CDesCArrayFlat* orgList = new (ELeave)CDesCArrayFlat(4);
	CleanupStack::PushL(orgList);
	TInt orgCount = GetVCardPropertyAsArrayOfValuesL(aVCard, KVersitTokenORG, *orgList);
	if(orgCount)
		{
		MergeOrgDetailsL(aContact,*orgList,aOption);
		deleteContact = EFalse;
		}
	CleanupStack::PopAndDestroy(orgList); // orgList
	
	// Get Single Instance of Class Field from the vCard.
	HBufC* singleClass = NULL;
	singleClass = HBufC::NewLC(256);
   	TPtr ptr(singleClass->Des());

	TInt classCount = GetSingleInstanceL(aVCard, KVersitTokenClass, ptr);
	if(classCount)
		{
		MergeSingleInstanceL(aContact,ptr,KUidContactFieldClass, KUidContactFieldVCardMapClass,aOption);
		deleteContact = EFalse;
		}
	CleanupStack::PopAndDestroy(singleClass);

	// Get other properties
	CArrayPtr<CParserProperty>* arrayOfProperties = aVCard.ArrayOfProperties();
	CleanupStack::PushL(TCleanupItem(CleanUpResetAndDestroy, arrayOfProperties));
	count = arrayOfProperties->Count();

	//Within this loop we store the content type of the last field updated.
	//If the content type of the current field matches, we update fieldCount
	//This holds the position of the field to update, to prevent us from
	//overwriting the field we just updated.

	TInt fieldCount = 0;
	CContactItemField* oldField = NULL;
	CContactItemField* newField = NULL;

	for (ii = 0; ii < count; ii++)
 	  	{
        if (((*arrayOfProperties)[ii]->Name() != KVersitTokenADR) && ((*arrayOfProperties)[ii]->Name() != KVersitTokenORG) && ((*arrayOfProperties)[ii]->Name() != KVersitTokenClass))
		 {
		  TBool unsupportedProperty=EFalse;
		 
			newField = GetVCardPropertyAsContactFieldLC((*arrayOfProperties)[ii], aOption,unsupportedProperty);
			if (newField)
				{
				TInt ttnumber;
				if (!oldField)
					{
					fieldCount = 1;
					ttnumber = 1;
					if (aOption & CContactVCardConverter::ETTFormat)
						{
						ttnumber = GetVCardPropertyTTNumber((*arrayOfProperties)[ii]);
						}
					}
				else if (newField->ContentType().IsEqualForSyncUpdate(oldField->ContentType()))
					{
					ttnumber = ++fieldCount;
					}
				else
					{
					fieldCount = 1;
					ttnumber = 1;
					if (aOption & CContactVCardConverter::ETTFormat)
						{
						ttnumber = GetVCardPropertyTTNumber((*arrayOfProperties)[ii]);
						}
					}
				if (oldField)
					{
					CleanupStack::Pop(); //newField
					CleanupStack::PopAndDestroy(oldField);
					oldField = NULL;
					CleanupStack::PushL(newField);
					}
				oldField = newField;
				aContact.CardFields().UpdateFieldSyncL(*newField, ttnumber);
				if(newField->Storage()->IsFull())
					{
					deleteContact = EFalse;
					}
				newField = NULL;	
				}
			else if (unsupportedProperty)
				{
				CleanupStack::Pop();
				deleteContact = EFalse;
				}
			else{
				CleanupStack::Pop();
				}
			}
    	}
    //Remove all empty fields after merge
    CContactItemFieldSet &fieldset = aContact.CardFields();
    TInt i = 0;
    for(; i < fieldset.Count(); ++i)
    	{
    	if( !fieldset[i].Storage()->IsFull() )
    		{
    		fieldset.Remove( i-- );
    		}
    	}

	if (newField)
		//coverity [dead_error_begin]
		{	
		CleanupStack::PopAndDestroy(newField);
		newField = NULL;
		}

	if (oldField)
		{
		CleanupStack::PopAndDestroy(oldField);
		oldField = NULL;
		}
	
	if (lastModified != Time::NullTTime())
		{
    	aContact.SetLastModified(lastModified);
		}
    CleanupStack::PopAndDestroy(arrayOfProperties);
	return(deleteContact);
	}
/**
 * Set a name field. Only creates a field if the specified name text is not empty.
 *
 * @param aNames An object containing the name and labels for 'N' property fields
 * @param aContact Contact item to add fields to
 * @param aOption Import preferences (available options defined in CContactDatabase::TOptions)
 * @param aIndex Array position of field to use
 * @param aFieldType Field type of field to add (TFieldType)
 * @param aInsertPos Position in contact field set to add field
*/
void CVCardToContactsAppConverter::SetNameFieldL(const CVCardItemAndLabel& aNames, CContactItem& aContact, TInt aOption, TInt aIndex, TFieldType aFieldType, TInt& aInsertPos) const
	{
	const TInt count = aNames.ItemCount();
	const TInt labelCount = aNames.LabelCount();
	//
	if	(aIndex >= 0 && aIndex < count)
		{
		const TPtrC pFieldText(aNames.Item(aIndex));
		TBool addField = EFalse;

		if (CContactVCardConverter::EConnectWhitespace & aOption)
			{
			addField = CContactVCardConverter::ContainsImportableData(pFieldText, CContactVCardConverter::EPropertyValueComposite, ImportType());
			}
		else
			{
			addField = (pFieldText.Length());
			}
		// Only add the field if it contains some data
		if	(addField)
			{
			CContactItemField* contactItemField = CContactItemField::NewLC(KStorageTypeText, aFieldType);
			contactItemField->SetMapping(KUidContactFieldVCardMapUnusedN);
			if ((aOption & CContactVCardConverter::EIncludeX) && (aIndex >= 0 && aIndex < labelCount))
				contactItemField->SetLabelL(aNames.Label(aIndex));
			//
			HBufC* encodedText = EncodeL(pFieldText, ETrue);
			contactItemField->TextStorage()->SetText(encodedText); // takes ownership
			
			aContact.InsertFieldL(*contactItemField, aInsertPos++);
			CleanupStack::Pop(contactItemField);
			}
		}
	}

HBufC* CVCardToContactsAppConverter::EncodeL(const TDesC& aText, TBool aTextTobeTruncated) const
	{
	const TUid KUidTextToEtextNoTrim={0x10281B4C};
	
	// Make a copy of aText and truncate if necessary.
	TPtr truncText(const_cast<TUint16*>(aText.Ptr()),aText.Length());
	
	if(aTextTobeTruncated)
		truncText.SetLength(aText.Length()>KCntMaxTextFieldLength ? KCntMaxTextFieldLength : aText.Length());
	else
		truncText.SetLength(aText.Length());
			
	HBufC8* text=HBufC8::NewLC(truncText.Length()*2);
	TPtr8 ptr = text->Des();
	TInt i;
	for (i=0; i < truncText.Length(); i++)
		{
		ptr.Append(truncText[i] & 0x00FF);
		ptr.Append((truncText[i] >> 8) & 0x00FF);
		}
	CCnaConverterList* convList=CCnaConverterList::NewLC(); 
	CConverterBase* conv = convList->NewConverterL(KUidTextToEtextNoTrim); 
	if (!conv)
		{
		CleanupStack::PopAndDestroy();          // convList 
		User::Leave(KErrNotSupported);
		}
	CleanupStack::PushL(conv);
	CBufFlat* decodeBuffer = CBufFlat::NewL(256); 
	CleanupStack::PushL(decodeBuffer);
	CBufFlat* encodedBuffer = CBufFlat::NewL(256); 
	CleanupStack::PushL(encodedBuffer);
	decodeBuffer->InsertL(0,ptr);
	RBufReadStream readStream;
	RBufWriteStream writeStream;
	readStream.Open(*decodeBuffer);
	writeStream.Open(*encodedBuffer);
	conv->ConvertObjectL(readStream, writeStream); 
	readStream.Close();
	TInt size=encodedBuffer->Size();
	HBufC* writeBuf=HBufC::NewLC(size); 
	TPtr resulttext = writeBuf->Des();
	for(i = 0; i < (size - 1); i += 2)
		{
		resulttext.Append((encodedBuffer->Ptr(0)[i + 1] << 8) | 
		encodedBuffer->Ptr(0)[i]);
		}

	writeStream.CommitL();
	writeStream.Close();
	CleanupStack::Pop(); // writebuf
	CleanupStack::PopAndDestroy(2); // buffers 
	CleanupStack::PopAndDestroy(2); //conv+convList 
	CleanupStack::PopAndDestroy();  //text
	return writeBuf;
	}

/**
 * Set name fields
 *
 * @param aNames An object containing the name and labels for 'N' property fields
 * @param aContact Contact item to add fields to
 * @param aOption Import preferences (available options defined in CContactDatabase::TOptions)
 * @param aTreatAsPronunciation If true, save name data in pronunciation fields, otherwise save as an actual name (the default)
 */
void CVCardToContactsAppConverter::SetNameFieldsL(const CVCardItemAndLabel& aNames, CContactItem& aContact, TInt aOption, TBool aTreatAsPronunciation) const
	{
	TInt insertPos=0;
	if( aTreatAsPronunciation )
		{
		SetNameFieldL(aNames, aContact, aOption, KContactGivenName, KUidContactFieldGivenNamePronunciation, insertPos);
		SetNameFieldL(aNames, aContact, aOption, KContactFamilyName, KUidContactFieldFamilyNamePronunciation, insertPos);
		}
	else
		{
		SetNameFieldL(aNames, aContact, aOption, KContactPrefixName, KUidContactFieldPrefixName, insertPos);
		SetNameFieldL(aNames, aContact, aOption, KContactGivenName, KUidContactFieldGivenName, insertPos);
		SetNameFieldL(aNames, aContact, aOption, KContactAdditionalName, KUidContactFieldAdditionalName, insertPos);
		SetNameFieldL(aNames, aContact, aOption, KContactFamilyName, KUidContactFieldFamilyName, insertPos);
		SetNameFieldL(aNames, aContact, aOption, KContactSuffixName, KUidContactFieldSuffixName, insertPos);
		}
	}



void CVCardToContactsAppConverter::SetAddressFieldL(const CVCardAddress& aAddress, CContactItem& aContact, TInt aOption, TInt aIndex, TFieldType aFieldType, TInt& aInsertPos, TUid aMapping) const
/**
 * Set an address field
 *
 * @param aAddress An object containing the name and labels for 'ADR' property fields
 * @param aContact Contact item to add fields to
 * @param aOption Import preferences (available options defined in CContactVCardConverter::TOptions)
 * @param aIndex Array position of field to use
 * @param aFieldType Contact field type for address field
 * @param aInsertPos Position in contact field set to add field
 * @param aMapping vCard field mapping (eg. KUidContactFieldVCardMapPOSTCODE)
 */
	{
	const TInt count = aAddress.ItemCount();
	if	(aIndex >= 0 && aIndex < count)
		{
		const TPtrC pFieldText(aAddress.Item(aIndex));
		TBool doInsert = ETrue; // By default we process the data, but if the TimeIS flag is set, then we need to check the content first.

		if (CContactVCardConverter::EConnectWhitespace & aOption)
			{
			doInsert = (CContactVCardConverter::ContainsImportableData(pFieldText, CContactVCardConverter::EPropertyValueComposite, ImportType()));
			}
		if (doInsert)
			{
			CContentType* content = CContentType::NewL(aFieldType, aMapping);
			CleanupStack::PushL(content);

			if	(aAddress.Mapping() != KNullUid) // KNullUid corresponds to general address
				{
				content->AddFieldTypeL(aAddress.Mapping());
				}

			CContactItemField* contactItemField = CContactItemField::NewLC(KStorageTypeText, *content);
			if ((aOption & CContactVCardConverter::EIncludeX) && (aIndex >= 0 && aIndex < aAddress.LabelCount()))
				{
				// get the correct label
				TBuf<25> labelName(KContactVarVCardXDashEPOCCNTMODEL);
				labelName.AppendFormat(KContactVarVCardLABELn, aIndex);
				TInt position=KErrNotFound;
				TInt err = aAddress.FindLabel(labelName, position);
				if	(err==KErrNone && position!=KErrNotFound)
					contactItemField->SetLabelL(aAddress.Label(position));
				}
			
			HBufC* encodedText = EncodeL(pFieldText, ETrue);
			contactItemField->TextStorage()->SetText(encodedText); // takes ownership
			
			// The contact takes ownership of the field.
			aContact.InsertFieldL(*contactItemField, aInsertPos++);
			CleanupStack::Pop(contactItemField);
			CleanupStack::PopAndDestroy(content);
			}
		}
	}


void CVCardToContactsAppConverter::SetAddressFieldsL(const CVCardAddress& aAddress, CContactItem& aContact, TInt aOption) const
/**
 * Set the address fields
 *
 * @param aAddressses An object containing the name and labels for 'ADR' property fields
 * @param aContact Contact item to add fields to
 * @param aOption Import preferences (available options defined in CContactDatabase::TOptions)
 */
	{
	TInt insertPos = 0;
	SetAddressFieldL(aAddress, aContact, aOption, KContactPostOffice, KUidContactFieldPostOffice, insertPos, KUidContactFieldVCardMapPOSTOFFICE);
	SetAddressFieldL(aAddress, aContact, aOption, KContactExtendedAddress, KUidContactFieldExtendedAddress, insertPos, KUidContactFieldVCardMapEXTENDEDADR);
	SetAddressFieldL(aAddress, aContact, aOption, KContactAddress, KUidContactFieldAddress, insertPos, KUidContactFieldVCardMapADR);
	SetAddressFieldL(aAddress, aContact, aOption, KContactLocality, KUidContactFieldLocality, insertPos, KUidContactFieldVCardMapLOCALITY);
	SetAddressFieldL(aAddress, aContact, aOption, KContactRegion, KUidContactFieldRegion, insertPos, KUidContactFieldVCardMapREGION);
	SetAddressFieldL(aAddress, aContact, aOption, KContactPostcode, KUidContactFieldPostcode, insertPos, KUidContactFieldVCardMapPOSTCODE);
	SetAddressFieldL(aAddress, aContact, aOption, KContactCountry, KUidContactFieldCountry, insertPos, KUidContactFieldVCardMapCOUNTRY);
	}


/**
 * Convert aVCard into a ContactItem. This method leaves a CContactItem instance on the cleanup stack upon exit.
 *
 * @param aVCard					vCard parser object
 * @param aAgentContact             Agent contact item. Note that this object is passed by reference but is not
 *                                  left on the cleanup stack upon exit from this function. Clients shoul re-Push
 *									this object if needed.
 * @param aUnknownPropertyBehaviour Specifies how extension properties are handled
 * @param aOption                   Import preferences (available options defined in CContactDatabase::TOptions)
 *
 * @return                          A CContactItem instance (on the cleanup stack) that has been constructed from
 *                                  the vCard data.
 */
CContactItem* CVCardToContactsAppConverter::GetVCardAsContactItemLC(CParserVCard& aVCard, TUnknownPropertyBehaviour aUnknownPropertyBehaviour, TInt aOption)
	{
	TInt ii=0;
	TInt count = 0;
	//
	iUnknownPropertyBehaviour = aUnknownPropertyBehaviour;
	TTime lastModified;
	TBuf<KUidStringLength> uidstring;
	GetVCardModifiedTimeL(aVCard, lastModified);
	GetVCardUidStringL(aVCard, uidstring);

	//
	CContactCard* mainContact = CContactCard::NewLC();
	//
	// We're performing an initial import
	SetImportType(ECntVCardImportTypeFirstSync);

	// Get Name
	CVCardItemAndLabel* names = GetContactNameLC(aVCard, aOption);
	if (names && names->ItemCount())
		SetNameFieldsL(*names, *mainContact, aOption);
	CleanupStack::PopAndDestroy(names);

	// Get Name pronunciation
	names = GetContactNamePrnLC(aVCard, aOption);
	if (names && names->ItemCount())
		{
		SetNameFieldsL(*names, *mainContact, aOption, ETrue);
		}
	CleanupStack::PopAndDestroy(names);

	// Create address container
	RPointerArray<CVCardAddress> addresses(KVCardImportAddressArrayGranularity);
	CleanupStack::PushL(TCleanupItem(CVCardItemAndLabel::CleanUpResetDestroyAndCloseArray, &addresses));

	// Get addresses
	GetAddressesL(aVCard, aOption, addresses);

	// Import each address field into the contact card
	count = addresses.Count();
    for (ii=0; ii<count; ii++)
		{
		const CVCardAddress* address = addresses[ii];
		SetAddressFieldsL(*address, *mainContact, aOption);
		}

	// Finished with addresses now, so clean up
	CleanupStack::PopAndDestroy(&addresses);

// Get Organization related information from the vCard. This actually only retrieves the Company and the Department Name
	CDesCArrayFlat* orgList = new (ELeave)CDesCArrayFlat(4);
	CleanupStack::PushL(orgList);
	TInt orgCount = GetVCardPropertyAsArrayOfValuesL(aVCard, KVersitTokenORG, *orgList);
	if(orgCount)
		{
		SetOrgDetailsL(*mainContact, *orgList, aOption);
		}
	CleanupStack::PopAndDestroy(orgList); // orgList

// Get Single Instance of Class Field from the vCard.
	HBufC* singleClass = NULL;
	singleClass = HBufC::NewLC(256);
	TPtr ptr(singleClass->Des());

	TInt classCount = GetSingleInstanceL(aVCard, KVersitTokenClass, ptr);
	if(classCount)
		{
		SetSingleInstanceL(*mainContact, ptr, KUidContactFieldClass, KUidContactFieldVCardMapClass,aOption);
		}
	CleanupStack::PopAndDestroy(singleClass);

	// Get other properties
    CArrayPtr<CParserProperty>* arrayOfProperties = aVCard.ArrayOfProperties();
	CleanupStack::PushL(TCleanupItem(CleanUpResetAndDestroy,arrayOfProperties));
	//
	if	(arrayOfProperties)
		{
		count = arrayOfProperties->Count();
		for (ii=0; ii<count; ii++)
			{
			CParserProperty* property = arrayOfProperties->At(ii);
			// Address fields are handled in the above sections
			if	((property->Name() != KVersitTokenADR) && (property->Name() != KVersitTokenORG) && (property->Name() != KVersitTokenClass))
				{
				// NOTE: This method can return a NULL object pushed onto the cleanup stack, so must
				// always be popped!
				TBool unsupportedProperty=EFalse;
				CContactItemField* field = GetVCardPropertyAsContactFieldLC(property, aOption,unsupportedProperty);
				if	(field)
					{
					mainContact->AddFieldL(*field);
					}
				CleanupStack::Pop(field);
				}
			}
		// Email and Tel properties should only have a single Pref parameter
		AdjustForPrefRule(*mainContact, KUidContactFieldEMail, KUidContactFieldVCardMapEMAILINTERNET);
		AdjustForPrefRule(*mainContact, KUidContactFieldPhoneNumber, KUidContactFieldVCardMapTEL);
		}
	//
    if	(lastModified != Time::NullTTime())
    	{
    	mainContact->SetLastModified(lastModified);
    	}
	//
	mainContact->SetUidStringL(uidstring);
	//
    CleanupStack::PopAndDestroy(); // arrayOfProperties->ResetAndDestroy()
	//
	return mainContact;
    }


/**
 * Extract the last modified date of the vCard.
 * If there is no 'REV' property the last modified time is returned as as NULL TTime value.
 *
 * @param aVCard vCard parser object
 * @param aLastModified Last modified time
 */
void CVCardToContactsAppConverter::GetVCardModifiedTimeL(CParserVCard& aVCard,TTime& aLastModified)
	{
	CArrayPtr<CParserProperty>* arrayOfRevisions=aVCard.PropertyL(KVersitTokenREV,TUid::Uid(KVersitPropertyDateTimeUid),EFalse);
	if (arrayOfRevisions && arrayOfRevisions->Count())
		{
		CleanupStack::PushL(arrayOfRevisions);
   		CParserPropertyValueDateTime* revision=(static_cast<CParserPropertyValueDateTime*>((*arrayOfRevisions)[0]->Value()));

   		aLastModified=revision->Value()->iDateTime;
		if (revision->Value()->iRelativeTime != TVersitDateTime::EIsUTC )
			// The REV property isn't in UTC time, so let's (try to) convert it
			{
			CArrayPtr<CParserProperty>* arrayOfTimeZones=aVCard.PropertyL(KVersitTokenTZ, TUid::Uid(KVersitPropertyTimeZoneUid),EFalse);
			if (arrayOfTimeZones && arrayOfTimeZones->Count())
				// If we have the TZ property, adjust the machine local timestamp.
				// If we don't have the TZ property, we'll just pretend that the REV
				// property was already a UTC value.
				{
				CleanupStack::PushL(arrayOfTimeZones);
				CParserPropertyValueTimeZone* timeZone =
						static_cast<CParserPropertyValueTimeZone*> ((*arrayOfTimeZones)[0]->Value());
				// Subtract the offset: UTC + Offset = Local => UTC = Local - Offset.
				TTimeIntervalSeconds utcOffset = timeZone->Value().Int();
				aLastModified -= utcOffset;
				CleanupStack::PopAndDestroy(arrayOfTimeZones);
				}
			}
		CleanupStack::PopAndDestroy(arrayOfRevisions);
   		}
   	else
		{
   		aLastModified=Time::NullTTime();
		}
    }


/**
 * Extract the vCard 'UID' property from the vCard.
 *
 * @param aVCard vCard parser object
 * @param aUidString UID property value
 */
void CVCardToContactsAppConverter::GetVCardUidStringL(CParserVCard& aVCard,TDes& aUidString) const
	{
	const CArrayPtr<CParserProperty>* arrayOfProperties=aVCard.PropertyL(KVersitTokenUID,TUid::Uid(KVersitPropertyHBufCUid),EFalse);
    if (arrayOfProperties)
    	{
    	TInt count=arrayOfProperties->Count();
    	if (count)
    		{
    		CParserPropertyValueHBufC* uidstring=(STATIC_CAST(CParserPropertyValueHBufC*,(*arrayOfProperties)[0]->Value()));
			if (uidstring->Value().Length() > KUidStringLength )
				aUidString=uidstring->Value().Left(KUidStringLength) ; // truncate
			else
				aUidString=uidstring->Value();
    		}
    	delete arrayOfProperties;
    	}
    }


/**
 * Convert aVCardProperty into contact field.
 *
 * @param aProperty vCard property
 * @param aOption Import preferences (available options defined in CContactDatabase::TOptions)
 * @param aUnsupportedProperty Return whether or not the field is supported by the PC PIM, only relevant when using the EConnectWhitespace flag
 * @return Contact field or a NULL pointer (pushed onto the cleanup stack) in the case where
 *         it wasn't possible to create a contact field for this vCard property.
 */
CContactItemField* CVCardToContactsAppConverter::GetVCardPropertyAsContactFieldLC(CParserProperty* aProperty,TInt aOption,TBool& aUnsupportedProperty)
 	{
	TUid mappingUid;
	TStorageType storageType;
	CContentType* content=MapVCardPropertyToContentAndStorageTypeL(*aProperty,storageType);
	CleanupStack::PushL(content);
	mappingUid=content->Mapping();
	TBool noConversion=EFalse;
	aUnsupportedProperty=EFalse;

	if (mappingUid==KUidContactFieldVCardMapNotRequired)
		{
		noConversion=ETrue;
		}
	else
		{	/*Could preserve unknown non X- if VCard version no>2.1*/
		switch (iUnknownPropertyBehaviour)
			{
			case EDiscardAllUnknownProperties:
				if (mappingUid==KUidContactFieldVCardMapUnknownXDash)
					noConversion=ETrue;
					/* fall through */
			case EDiscardNonXDashUnknownProperties:
				if (mappingUid==KUidContactFieldVCardMapUnknown)
					noConversion=ETrue;
					/* fall through */
			default:
				break;
			}
		}

	CContactItemField* contactItemField=NULL;
	if (!noConversion)
		{
		TBool validImportableDataFound = ETrue;
		contactItemField=CContactItemField::NewLC(storageType,*content);
		if ((mappingUid==KUidContactFieldVCardMapUnknown) ||(mappingUid==KUidContactFieldVCardMapUnknownXDash))
	    		{
	    		TBool propertyTobeTruncated = PropertyTobeTruncated(aProperty->Name());
	    		HBufC* encodedText = EncodeL(STATIC_CAST(CParserPropertyValueHBufC*,aProperty->Value())->Value(), propertyTobeTruncated);
	    		contactItemField->TextStorage()->SetText(encodedText); // takes ownership
				
	    		contactItemField->SetHidden(ETrue);
	    		}
		else
			{
	    		switch (contactItemField->StorageType())
	    			{
	    			case KStorageTypeText:
	    				{
					TInt id = aProperty->Value()->Uid().iUid;
					if(mappingUid == KUidContactFieldVCardMapORGPronunciation)
						{ // this should be treated as an array instead of a single string
						id = KVersitPropertyCDesCArrayUid;
						}
	    				switch (id)
	    					{
	    					case KVersitPropertyCDesCArrayUid:
	    						{
								TBool doConvert = ETrue;
								 TBool pushed = EFalse;
								CParserPropertyValueCDesCArray* propertyValueWrapper=NULL;
								if(mappingUid == KUidContactFieldVCardMapORGPronunciation)
									{ // Treat a SOUND with X-IRMC-ORG param as an array
									const CParserPropertyValueHBufC* valueScalar = static_cast<CParserPropertyValueHBufC*>(aProperty->Value());
									if(valueScalar)
										{
										propertyValueWrapper = valueScalar->TreatAsArrayPropertyLC(*aProperty);
										pushed= ETrue;
										}
									}
								else
									{
									propertyValueWrapper = static_cast<CParserPropertyValueCDesCArray*>(aProperty->Value());
									}
								CDesCArray* propertyValue = NULL;
								if	(propertyValueWrapper)
									{
									propertyValue = propertyValueWrapper->Value();
									}

								if ((CContactVCardConverter::EConnectWhitespace & aOption) && propertyValue)
									{
									doConvert = TextArrayContainsImportableData(*propertyValue);
									if (doConvert)
										{
										RemoveWhitespaceFromCompositePropertyL(*propertyValue);
										}
									}
								if (doConvert && propertyValue)
									{
	   								TInt count = propertyValue->MdcaCount();
								    HBufC *txt=HBufC::NewL(0);
									CleanupStack::PushL(txt);
									// concatenate the array
								    for (TInt ii=0;ii<count;ii++)
								    	{
										TInt mdcaLen=propertyValue->MdcaPoint(ii).Size();
										if (mdcaLen>0)
											{
								    			if (txt->Length()>0)
												{
												txt->Des().Append(TChar('\n'));
												}
											txt=txt->ReAllocL(txt->Length()+mdcaLen+1);	// +1 In case we add '\n'
											CleanupStack::Pop();	// txt(old value)
											CleanupStack::PushL(txt);
											TPtrC temp(propertyValue->MdcaPoint(ii));
											txt->Des().Append(temp);
											}
								    	}
			   						TBool propertyTobeTruncated = PropertyTobeTruncated(aProperty->Name());
			   						HBufC* encodedText = EncodeL(*txt, propertyTobeTruncated);
			   						CleanupStack::PopAndDestroy(txt);
			   						contactItemField->TextStorage()->SetText(encodedText); // takes ownership
									}
								else
									{
									validImportableDataFound = EFalse;
									}
								if(pushed)
									{
									CleanupStack::PopAndDestroy(propertyValueWrapper);
									}
	    						}
			    				break;
	   					case KVersitPropertyHBufCUid:
							{
							TBool doConvert = ETrue;

							CParserPropertyValueHBufC* propertyValueWrapper = static_cast<CParserPropertyValueHBufC*>(aProperty->Value());
							const TPtrC propertyValue(propertyValueWrapper->Value());

							if (CContactVCardConverter::EConnectWhitespace & aOption)
								{
								if (propertyValue==KSingleSpacePropertyValue)
									{
									aUnsupportedProperty=ETrue;
									}
								doConvert = (CContactVCardConverter::ContainsImportableData(propertyValue, CContactVCardConverter::EPropertyValueSingle, ImportType()));
								}
							if (doConvert)
								{
								TBool propertyTobeTruncated = PropertyTobeTruncated(aProperty->Name());
								HBufC* encodedText = EncodeL(propertyValue, propertyTobeTruncated);
								contactItemField->TextStorage()->SetText(encodedText); // takes ownership
								}
							else
								{
								validImportableDataFound = EFalse;
								}
							}
			    				break;
		   				default:;
	    					}
	    				}
		    			break;
		    		case KStorageTypeDateTime:
	    				{
		    			switch (aProperty->Value()->Uid().iUid)
		    				{
		    				case KVersitPropertyDateTimeUid:
		    					STATIC_CAST(CContactDateField*,contactItemField->Storage())->SetTime(STATIC_CAST(CParserPropertyValueDateTime*,aProperty->Value())->Value()->iDateTime);
		    					break;
						case KVersitPropertyDateUid:
							STATIC_CAST(CContactDateField*,contactItemField->Storage())->SetTime(STATIC_CAST(CParserPropertyValueDate*,aProperty->Value())->Value()->iDateTime);
							break;
		    				default:
			    				break;
		    				}
		    			}
	    				break;
		    		case KStorageTypeStore:
		    			{
		    			STATIC_CAST(CContactStoreField*,contactItemField->Storage())->SetThingL(STATIC_CAST(CParserPropertyValueBinary*,aProperty->Value())->Value());
		    			}
		    			break;
		    		default:
		    			break;
		    		}
	    		}
		if (aOption & CContactVCardConverter::EIncludeX)
			{
			TBuf8<KContactMaxVCardPropertyNameLength> paramName;
			paramName=KContactVCardXDashEPOCCNTMODEL;
			paramName.Append(KContactVCardFIELDHIDDEN);
			if (aProperty->Param(paramName))
				contactItemField->SetHidden(ETrue);
			paramName.Zero();
			paramName=KContactVCardXDashEPOCCNTMODEL;
			paramName.Append(KContactVCardFIELDREADONLY);
			if (aProperty->Param(paramName))
				contactItemField->SetReadOnly(ETrue);
			paramName.Zero();
			paramName=KContactVCardXDashEPOCCNTMODEL;
			paramName.Append(KContactVCardFIELDLABEL);
			if (aProperty->Param(paramName))
				contactItemField->SetLabel(aProperty->Param(paramName)->ValueL());
			}
		CleanupStack::Pop(contactItemField);
	// The field is only added to the contact card if it was found to contain
	// legitimate importable data (i.e. has content that does not simply consist of LWSP characters).
	// This is important, since TimeIS sync drivers can create vCard objects (pushed over to the device)
	// that contain only space characters for their Versit 'Property Parameter Values'.
		if (!validImportableDataFound)
			{
			delete contactItemField;
			contactItemField = NULL;
			}
		}
	CleanupStack::PopAndDestroy(content);
	// We may be pushing a NULL pointer back onto the cleanup stack, but clients of this
	// method understand this feature.
	CleanupStack::PushL(contactItemField);
	return(contactItemField);
    }


/**
 * Find a vCard property parameter in a vCard property
 *
 * @param aVCardProperty vCard property
 * @return Property parameter number
 */
TInt CVCardToContactsAppConverter::GetVCardPropertyTTNumber(CParserProperty* aVCardProperty)
    {
    TInt ii;
	TBuf8<KContactMaxVCardPropertyNameLength> paramName;
	for (ii=1;ii<=KContactMaxFieldNumber;ii++)
		{
		paramName.Format(_L8("%d"),ii);
		if (aVCardProperty->Param(paramName))
			{
			return (ii);
			}
		}
	return 1;	// Default to 1 if no number found
	}


/**
 * Extract the contact name fields from the vCard 'N' property.
 * This method takes values from the 'N' vCard property and returns the family name,
 * given name, middle name, prefix and suffix.
 *
 * Only the first instance of the 'N' property is used.
 *
 * @param aVCard vCard parser object
 * @param aOption Import preferences (available options defined in CContactDatabase::TOptions)
 * @return An instance of CVCardItemAndLabel which encapsulates the names and their associated
 *         labels contained within the vCard or a NULL pointer pushed onto the cleanup stack
 *         if it wasn't possible to construct the object.
 */
CVCardItemAndLabel* CVCardToContactsAppConverter::GetContactNameLC(CParserVCard& aVCard, TInt aOption)
    {
	CVCardItemAndLabel* names = NULL;

	CArrayPtr<CParserProperty>* arrayOfProperties=aVCard.PropertyL(KVersitTokenN,TUid::Uid(KVersitPropertyCDesCArrayUid));
  	if (arrayOfProperties && arrayOfProperties->Count())
		{
		CleanupStack::PushL(TCleanupItem(CleanUpResetAndDestroy,arrayOfProperties));
		//
		//
		CParserProperty* property = arrayOfProperties->At(0);
		CDesCArray& items = *static_cast<CParserPropertyValueCDesCArray*>(property->Value())->Value();
		names = MakeNamesFromItemsL(items, aOption, property );
		CleanupStack::PopAndDestroy(arrayOfProperties);
    	}

	// Clients of this method assume that something is pushed onto the Cleanup Stack, so
	// this might potentially be NULL.
	CleanupStack::PushL(names);
	return names;
    }



/**
 * Extract the contact name fields from the vCard 'N' property.
 * This method takes values from the 'N' vCard property and returns the family name,
 * given name, middle name, prefix and suffix.
 *
 * Only the first instance of the 'N' property is used.
 *
 * @param aVCard vCard parser object
 * @param aOption Import preferences (available options defined in CContactDatabase::TOptions)
 * @return An instance of CVCardItemAndLabel which encapsulates the names and their associated
 *         labels contained within the vCard or a NULL pointer pushed onto the cleanup stack
 *         if it wasn't possible to construct the object.
 */
CVCardItemAndLabel* CVCardToContactsAppConverter::GetContactNamePrnLC(CParserVCard& aVCard, TInt aOption)
	{
	CVCardItemAndLabel* names = NULL;

	CArrayPtr<CParserProperty>* arrayOfProperties=aVCard.PropertyL(KVersitTokenSOUND,TUid::Uid(KVersitPropertyHBufCUid), EFalse);

	if (arrayOfProperties )
		{
		CleanupStack::PushL(arrayOfProperties);
		TInt count = arrayOfProperties->Count();
		const CParserProperty* property=NULL;
		for (TInt index=0;index<count;index++)
			{
			property = arrayOfProperties->At(index);
			if(property->Param(KVersitParam8NamePrn))
				{
				index=count;
				}
			else
				{
				property=NULL;
				}
			}
		if (property) // property is the 1st SOUND property with an X-IRMC-N parameter
			{
			const CParserPropertyValueHBufC* valueScalar = static_cast<CParserPropertyValueHBufC*>(property->Value());
			if(valueScalar)
				{
				CParserPropertyValueCDesCArray* valueArray = valueScalar->TreatAsArrayPropertyLC(*property);
				names = MakeNamesFromItemsL( *valueArray->Value(), aOption, property );// valueArray will never be NULL
				CleanupStack::PopAndDestroy(valueArray);
				}
			}
		CleanupStack::PopAndDestroy(arrayOfProperties);
    		}

	// Clients of this method assume that something is pushed onto the Cleanup Stack, so
	// this might potentially be NULL.
	CleanupStack::PushL(names);
	return names;
	}

CVCardItemAndLabel* CVCardToContactsAppConverter::MakeNamesFromItemsL(const CDesCArray& aItems, TInt aOption, const CParserProperty* aProperty )
	{
	CVCardItemAndLabel* names = CVCardItemAndLabel::NewLC();

	const TInt fieldcount = aItems.Count();
	//
    	for(TInt ii=0; ii<fieldcount; ii++)
    		{
		const TPtrC pField(aItems.MdcaPoint(ii));
    		names->AddItemL(pField);
		if (aOption & CContactVCardConverter::EIncludeX)
			{
			TBuf8<KContactMaxVCardPropertyNameLength> paramName(KContactVCardXDashEPOCCNTMODEL);
			paramName.AppendFormat(KContactVCardLABELn,ii);
			//
			if(aProperty->Param(paramName))
				{
				HBufC* value = aProperty->Param(paramName)->ValueL();
				CleanupStack::PushL(value);
				names->AddLabelL(*value);
				CleanupStack::PopAndDestroy(value);
				}
    			}
		}
	CleanupStack::Pop(names);
	return names;
	}

/**
 * Extract specific address properties

 * @param aProperties An array of vCard properties which should be searched
 * @param aOption Import preferences (available options defined in CContactDatabase::TOptions)
 * @param aParamMustMatch A property parameter which must be found in the specified vCard property. If this
 *                        has a NULL (KNullDesC) value then the property is treated as satisfying this condition
 * @param aParamMustNotMatch1 A property parameter which must not be found in the specified vCard property. If this
 *                        has a NULL (KNullDesC) value then the property is treated as satisfying this condition
 * @param aParamMustNotMatch2 Another property parameter which must not be found in the specified vCard property. If this
 *                        has a NULL (KNullDesC) value then the property is treated as satisfying this condition
 * @param aMapping The address mapping which is being searched for (e.g. HOME, WORK, etc)
 * @return An address object which contains the name and labels (optionally) located in aProperties for this particular mapping
 */
CVCardAddress* CVCardToContactsAppConverter::GetSpecifiedAddressLC(const CArrayPtr<CParserProperty>& aProperties, TInt aOption, const TDesC8& aParamMustMatch, const TDesC8& aParamMustNotMatch1, const TDesC8& aParamMustNotMatch2, TUid aMapping)
	{
	CVCardAddress* address = NULL;
	const TInt propertyCount = aProperties.Count();
	//
	for (TInt ii=0; ii<propertyCount; ii++)
		{
		CParserProperty* property = aProperties[ii];
		
		// DEF084708, also get and compare for ADDR;TYPE=HOME etc in addition to ADDR;HOME. 
		// The param to be matched (or not matched) can be found either as the param name (ADDR;HOME)
		// or in the param value of param name - TYPE (case ADDR;TYPE=HOME)
		CParserParam* paramType = property->Param(KVersitParam8Type);
		if (
			(property->Name() == KVersitTokenADR) &&
			(property->Value()->Uid() == TUid::Uid(KVersitPropertyCDesCArrayUid)) &&
			
			(!aParamMustMatch.Length()     || property->Param(aParamMustMatch) != NULL ||
			(paramType != NULL && paramType->Value().CompareF(aParamMustMatch) == 0)) &&
			
			(!aParamMustNotMatch1.Length() || (property->Param(aParamMustNotMatch1) == NULL &&
			(paramType == NULL || paramType->Value().CompareF(aParamMustNotMatch1) != 0))) &&
			
			(!aParamMustNotMatch2.Length() || (property->Param(aParamMustNotMatch2) == NULL &&
			(paramType == NULL || paramType->Value().CompareF(aParamMustNotMatch2) != 0)))
			)
			{
			
			CDesCArray& items = *static_cast<CParserPropertyValueCDesCArray*>(property->Value())->Value();
			const TInt fieldcount = items.Count();

			// Create the address
			address = CVCardAddress::NewLC(aMapping);

			for(TInt jj = 0; jj < fieldcount; jj++)
    			{
				const TPtrC pField(items.MdcaPoint(jj));
    			address->AddItemL(pField);
				//
				if	(aOption & CContactVCardConverter::EIncludeX)
					{
					TBuf8<KContactMaxVCardPropertyNameLength> paramName(KContactVCardXDashEPOCCNTMODEL);
					paramName.AppendFormat(KContactVCardLABELn, jj);
					//
					if	(property->Param(paramName))
						{
						HBufC* value = property->Param(paramName)->ValueL();
						CleanupStack::PushL(value);
						address->AddLabelL(*value);
						CleanupStack::PopAndDestroy(value);
						}
    				}
				}

			// End for-loop now we've found the specified item
			break;
			}
		}

	// Have to push something to ensure cleanup stack is balanced
	if	(!address)
		CleanupStack::PushL((TAny*) NULL);
	return address;
	}


/**
 * Extract home and work address properties from the vCard
 *
 * @param aVCard vCard parser object
 * @param aOption Import preferences (available options defined in CContactDatabase::TOptions)
 * @param aAddresses An array reference which contains any located address fields upon exit from this method
 */
void CVCardToContactsAppConverter::GetAddressesL(CParserVCard& aVCard, TInt aOption, RPointerArray<CVCardAddress>& aAddresses)
    {
	CArrayPtr<CParserProperty>* arrayOfProperties = aVCard.ArrayOfProperties(EFalse);
	if	(arrayOfProperties)
		{
		// Find ADR;HOME
		CVCardAddress* home = GetSpecifiedAddressLC(*arrayOfProperties,
													aOption,
													KVersitParam8Home, // Must be ADR;HOME and not ADR;HOME;WORK
													KVersitParam8Work,
													KNullDesC8,
													KUidContactFieldVCardMapHOME
													);
		if	(home)
			User::LeaveIfError(aAddresses.Append(home));
		CleanupStack::Pop(home);

		// Find ADR;WORK
		CVCardAddress* work = GetSpecifiedAddressLC(*arrayOfProperties,
													aOption,
													KVersitParam8Work, // Must be ADR;WORK
													KNullDesC8,
													KNullDesC8,
													KUidContactFieldVCardMapWORK
													);
		if	(work)
			User::LeaveIfError(aAddresses.Append(work));
		CleanupStack::Pop(work);

		// Find ADR;PREF
		CVCardAddress* pref = GetSpecifiedAddressLC(*arrayOfProperties,
													aOption,
													KVersitParam8Pref, // Must be ADR;PREF and not ADR;HOME or ADR;WORK
													KVersitParam8Home,
													KVersitParam8Work,
													KUidContactFieldVCardMapPREF
													);
		if	(pref)
			User::LeaveIfError(aAddresses.Append(pref));
		CleanupStack::Pop(pref);

		// Find general ADR: address
		CVCardAddress* general = GetSpecifiedAddressLC(*arrayOfProperties,
													aOption,
													KNullDesC8, // Just has to be ADR
													KVersitParam8Home,
													KVersitParam8Work,
													KNullUid
													);
		if	(general)
			User::LeaveIfError(aAddresses.Append(general));
		CleanupStack::Pop(general);
		}
	}



/**
 * Map vCard property to contacts field content and storage type
 *
 * @param aVCardProperty vCard property
 * @param aStorageType Type of contact field storage (TStorageType)
 * @return Content type for vCard property
 */
CContentType* CVCardToContactsAppConverter::MapVCardPropertyToContentAndStorageTypeL(const CParserProperty& aVCardProperty, TStorageType& aStorageType)
    {
	TUid mapping=KUidContactFieldNone;
	TFieldType fieldType=KUidContactFieldNone;
	TPtrC8 vpropNameConst = aVCardProperty.Name();
    aStorageType=KStorageTypeStore;

	switch (aVCardProperty.Value()->Uid().iUid)
    	{
    case KVersitPropertyCDesCArrayUid:
    	{
    	aStorageType=KStorageTypeText;
    	if (vpropNameConst.CompareF(KVersitTokenN)==0)
			{
    		fieldType=KUidContactFieldFamilyName;
			mapping=KUidContactFieldVCardMapUnusedN;
			}
    	else if (vpropNameConst.CompareF(KVersitTokenADR)==0)
			{
    		fieldType=KUidContactFieldAddress;
			mapping=KUidContactFieldVCardMapADR;
			}
 		else if (vpropNameConst.CompareF(KVersitTokenChildren) == 0)
 			{
 			mapping = KUidContactFieldVCardMapChildren;
 			fieldType = KUidContactFieldChildren;
 			}
    	}
    	break;
    case KVersitPropertyDateTimeUid:
    	aStorageType=KStorageTypeDateTime;
    	if (vpropNameConst.CompareF(KVersitTokenREV)==0)
    		mapping=KUidContactFieldVCardMapNotRequired;
    	break;
    case KVersitPropertyDateUid:
    	aStorageType=KStorageTypeDateTime;
    	if (vpropNameConst.CompareF(KVersitTokenBDAY)==0)
			{
    		mapping=KUidContactFieldVCardMapBDAY;
			fieldType=KUidContactFieldBirthday;
			}
 		else if (vpropNameConst.CompareF(KVersitTokenAnniversary) == 0)
 			{
 			mapping = KUidContactFieldVCardMapAnniversary;
 			fieldType = KUidContactFieldAnniversary;
 			}
    	break;
    case KVCardPropertyAgentUid:
		aStorageType = KStorageTypeContactItemId ; 
		if (vpropNameConst.CompareF(KVersitTokenAGENT) == 0)
			{
			mapping = KUidContactFieldVCardMapAGENT;
			}
    	break;

    case KVersitPropertyTimeZoneUid:
    	aStorageType=KStorageTypeStore;

    	if(vpropNameConst.CompareF(KVersitTokenTZ) == 0)
    		{
    		mapping=KUidContactFieldVCardMapNotRequired;
    		}
    	break;
//////////////
    default:	// aka case KVersitPropertyHBufCUid:
    	aStorageType=KStorageTypeText;
		mapping = KUidContactFieldVCardMapUnknown; // Initialize to something sensible
		switch (vpropNameConst[0])
			{
		case 'B':
		case 'b':
			if (vpropNameConst.CompareF(KVersitTokenBDAY)==0)
				{
    			mapping=KUidContactFieldVCardMapBDAY;
				fieldType=KUidContactFieldBirthday;
				}
			break;
    	case 'E':
    	case 'e':
			if (vpropNameConst.CompareF(KVersitTokenEMAIL)==0)
				{
    			fieldType=KUidContactFieldEMail;
    			mapping=KUidContactFieldVCardMapEMAILINTERNET;
				}
			break;
    	case 'F':
    	case 'f':
			if (vpropNameConst.CompareF(KVersitTokenFN)==0)
    			mapping=KUidContactFieldVCardMapUnusedFN;
			break;
    	case 'G':
    	case 'g':
			if (vpropNameConst.CompareF(KVersitTokenGEO)==0)
				{
				fieldType=KUidContactFieldGEO;
    			mapping=KUidContactFieldVCardMapGEO;
				}
			break;
    	case 'K':
    	case 'k':
			if (vpropNameConst.CompareF(KVersitTokenKEY)==0)
				{
				if(aVCardProperty.Value()->Uid().iUid == KVersitPropertyBinaryUid)
					{					
					aStorageType = KStorageTypeStore;
			 		}
    			mapping=KUidContactFieldVCardMapKEY;
				}
			break;
    	case 'L':
    	case 'l':
			if (vpropNameConst.CompareF(KVersitTokenLABEL)==0)
    			mapping=KUidContactFieldVCardMapLABEL;
			else if (vpropNameConst.CompareF(KVersitTokenLOGO)==0)
				{
				aStorageType=KStorageTypeStore;
    			mapping=KUidContactFieldVCardMapLOGO;
				}
    		break;
    	case 'M':
    	case 'm':
			if  (vpropNameConst.CompareF(KVersitTokenMAILER)==0)
    			mapping=KUidContactFieldVCardMapMAILER;
			break;
    	case 'N':
    	case 'n':
			if (vpropNameConst.CompareF(KVersitTokenNOTE)==0)
				{
				fieldType=KUidContactFieldNote;
    				mapping=KUidContactFieldVCardMapNOTE;
				}
			break;
    	case 'P':
    	case 'p':
			if (vpropNameConst.CompareF(KVersitTokenPHOTO)==0)
				{
    			mapping=KUidContactFieldVCardMapPHOTO;
				aStorageType=KStorageTypeStore;
				}
			break;
    	case 'R':
    	case 'r':
			if (vpropNameConst.CompareF(KVersitTokenROLE)==0)
    			mapping=KUidContactFieldVCardMapROLE;
			break;
    	case 'S':
    	case 's':
			if (vpropNameConst.CompareF(KVersitTokenSOUND)==0)
				{
				if(aVCardProperty.Param(KVersitParam8CompanyPrn))
					{
					mapping = KUidContactFieldVCardMapORGPronunciation;
					fieldType=KUidContactFieldCompanyNamePronunciation;
					}
				else if(aVCardProperty.Param(KVersitParam8NamePrn))
					{// this is handled elsewhere
					mapping = KUidContactFieldVCardMapNotRequired;
					}
				else // if we ever support any of the other pronunication extensions we'd have to add the checking here
					{
					mapping=KUidContactFieldVCardMapSOUND;
					}
				}
			break;
    	case 'T':
    	case 't':    	
			 if (vpropNameConst.CompareF(KVersitTokenTEL)==0)
    			{
    			fieldType=KUidContactFieldPhoneNumber;
				mapping=KUidContactFieldVCardMapTEL;
    			}
			 else if (vpropNameConst.CompareF(KVersitTokenTITLE)==0)
				{
				fieldType=KUidContactFieldJobTitle;
				mapping=KUidContactFieldVCardMapTITLE;
				}
			 else if (vpropNameConst.CompareF(KVersitTokenTZ)==0)
				{
				mapping=KUidContactFieldVCardMapNotRequired;
				}
			break;
    	case 'U':
    	case 'u':    	
			if (vpropNameConst.CompareF(KVersitTokenURL)==0)
				{
    			mapping=KUidContactFieldVCardMapURL;
				fieldType=KUidContactFieldUrl;
				}
			else if (vpropNameConst.CompareF(KVersitTokenUID)==0)
				{
				mapping=KUidContactFieldVCardMapNotRequired;
				}
			break;
    	case 'V':
    	case 'v':    	
			if (vpropNameConst.CompareF(KVersitTokenVERSION)==0)
				{
    			mapping=KUidContactFieldVCardMapNotRequired;
				}
			break;
    	case 'X':
    	case 'x':    	
			if (vpropNameConst.CompareF(KVersitTokenSECONDNAME)==0)
				{
				mapping=KUidContactFieldVCardMapSECONDNAME;
				fieldType=KUidContactFieldSecondName;
				}
			else if (vpropNameConst.CompareF(KVersitTokenSIPID)==0)
				{
				mapping=KUidContactFieldVCardMapSIPID;
				fieldType=KUidContactFieldSIPID;
				}
			else if (vpropNameConst.CompareF(KVersitTokenWVID)==0)
				{
				mapping=KUidContactFieldVCardMapWV;
				fieldType=KUidContactFieldIMAddress;
				}
			else if (vpropNameConst.CompareF(KVersitTokenAssistant) == 0)
				{
				mapping = KUidContactFieldVCardMapAssistant;
				fieldType = KUidContactFieldAssistant;
				}
			else if (vpropNameConst.CompareF(KVersitTokenAssistantTel) == 0)
				{
				mapping = KUidContactFieldVCardMapAssistantTel;
				fieldType = KUidContactFieldPhoneNumber;
				}
			else if (vpropNameConst.CompareF(KVersitTokenAnniversary) == 0)
				{
				mapping = KUidContactFieldVCardMapAnniversary;
				fieldType = KUidContactFieldAnniversary;
				}
			else if (vpropNameConst.CompareF(KVersitTokenSpouse) == 0)
				{
				mapping = KUidContactFieldVCardMapSpouse;
				fieldType = KUidContactFieldSpouse;
				}
			else if (vpropNameConst.Length()>=2)
    			{
    			if (vpropNameConst.Left(2).CompareF(KVersitTokenXDash)==0)
    				mapping=KUidContactFieldVCardMapUnknownXDash;
				else
 					mapping=KUidContactFieldVCardMapUnknown;
				}
			break;
    	default:
			break; // Leave mapping set to KUidContactFieldVCardMapUnknown
			};
		};

	CContentType* type=CContentType::NewL();
	CleanupStack::PushL( type );
	type->SetMapping(mapping);
	if (mapping==KUidContactFieldVCardMapPHOTO || mapping==KUidContactFieldVCardMapLOGO)
		{
		type->AddFieldTypeL(KUidContactFieldPicture);
        TFieldType bitmapFormat;
        GetBitMapFormat(aVCardProperty, bitmapFormat);
           
        if (bitmapFormat.iUid!=KUidContactFieldNone.iUid)
			{
            type->AddFieldTypeL(bitmapFormat);
			}
		}
	if(fieldType!=KUidContactFieldNone)
		{
		type->AddFieldTypeL(fieldType);
		}

	CArrayPtr<CParserParam>* paramArray = NULL ;
	paramArray = aVCardProperty.ParamArray();

	// parameters
	if (IsParameterValuePresent(paramArray, KVersitTokenTYPE, KVersitParam8Internet))
		{
		type->AddFieldTypeL(KUidContactFieldVCardMapINTERNET);
		}
	if  (IsParameterValuePresent(paramArray, KVersitTokenTYPE, KVersitParam8Home))
		{
		type->AddFieldTypeL(KUidContactFieldVCardMapHOME);
		}
    if 	(IsParameterValuePresent(paramArray, KVersitTokenTYPE, KVersitParam8Work))
		{
		type->AddFieldTypeL(KUidContactFieldVCardMapWORK);
		}
    if 	(IsParameterValuePresent(paramArray, KVersitTokenTYPE, KVersitParam8Voice))
		{
		type->AddFieldTypeL(KUidContactFieldVCardMapVOICE);
		}
    if 	(IsParameterValuePresent(paramArray, KVersitTokenTYPE, KVersitParam8Fax))
        {
        type->RemoveFieldType(KUidContactFieldPhoneNumber);
        type->AddFieldTypeL(KUidContactFieldFax);
        type->AddFieldTypeL(KUidContactFieldVCardMapFAX);
        }
    if 	(IsParameterValuePresent(paramArray, KVersitTokenTYPE, KVersitParam8Pref))
		{
		type->AddFieldTypeL(KUidContactFieldVCardMapPREF);
		}
    if 	(IsParameterValuePresent(paramArray, KVersitTokenTYPE, KVersitParam8Cell))
		{
		type->AddFieldTypeL(KUidContactFieldVCardMapCELL);
		}
    if 	(IsParameterValuePresent(paramArray, KVersitTokenTYPE, KVersitParam8Pager))
		{
		type->AddFieldTypeL(KUidContactFieldVCardMapPAGER);
		}
    if 	(IsParameterValuePresent(paramArray, KVersitTokenTYPE, KVersitParam8Bbs))
		{
		type->AddFieldTypeL(KUidContactFieldVCardMapBBS);
		}
    if 	(IsParameterValuePresent(paramArray, KVersitTokenTYPE, KVersitParam8Modem))
		{
		type->AddFieldTypeL(KUidContactFieldVCardMapMODEM);
		}
    if 	(IsParameterValuePresent(paramArray, KVersitTokenTYPE, KVersitParam8Car))
		{
		type->AddFieldTypeL(KUidContactFieldVCardMapCAR);
		}
    if 	(IsParameterValuePresent(paramArray, KVersitTokenTYPE, KVersitParam8Isdn))
		{
		type->AddFieldTypeL(KUidContactFieldVCardMapISDN);
		}
    if 	(IsParameterValuePresent(paramArray, KVersitTokenTYPE, KVersitParam8Video))
		{
		type->AddFieldTypeL(KUidContactFieldVCardMapVIDEO);
		}
	if 	(IsParameterValuePresent(paramArray, KVersitTokenTYPE, KVersitParam8Msg))
		{
		type->AddFieldTypeL(KUidContactFieldVCardMapMSG);
		}
	if 	(IsParameterValuePresent(paramArray, KVersitTokenTYPE, KVersitParam8Dom))
		{
		type->AddFieldTypeL(KUidContactFieldVCardMapDOM);
		}
	if 	(IsParameterValuePresent(paramArray, KVersitTokenTYPE, KVersitParam8X509))
		{
    	type->AddFieldTypeL(KUidContactFieldVCardMapX509);
		}
	if 	(IsParameterValuePresent(paramArray, KVersitTokenTYPE, KVersitParam8PGP))
		{
    	type->AddFieldTypeL(KUidContactFieldVCardMapPGP);
		}
	if (IsParameterValuePresent(paramArray, KVersitTokenTYPE, KVersitParamPOC))
		{
		type->AddFieldTypeL(KUidContactFieldVCardMapPOC);
		}
	if (IsParameterValuePresent(paramArray, KVersitTokenTYPE, KVersitParamSWIS))
		{
		type->AddFieldTypeL(KUidContactFieldVCardMapSWIS);
		}
	if (IsParameterValuePresent(paramArray, KVersitTokenTYPE, KVersitParamVOIP))
		{
		type->AddFieldTypeL(KUidContactFieldVCardMapVOIP);
		}
	

	CleanupStack::Pop(type);
	return(type);
    }

/*
 * Gets the bitmap format. This is done by looking for the name and value items in the parameter eg. TYPE=GIF
 * the if construct assumes that this is  present in the vcf file. It is also possible to just have the format without
 * the TYPE string and this is stored in the name part of the parameter so need to check through all supported formats
 * to find if one is present, this is done in the else construct.
 *
 * @param aVCardProperty and aBitmapFormat
 * @return void.
 */   
void CVCardToContactsAppConverter::GetBitMapFormat(const CParserProperty& aVCardProperty, TFieldType& aBitmapFormat)
    {
    aBitmapFormat.iUid=KUidContactFieldNone.iUid;
   
    CParserParam* param=aVCardProperty.Param(KVersitTokenTYPE);
    if (param)
        {
        TFieldType bitmapFormat=MapVCardPhotoTypeToFieldType(param->Value());
        if (bitmapFormat.iUid!=KUidContactFieldNone.iUid)
            {
            aBitmapFormat.iUid = bitmapFormat.iUid;
            }
        } //if
    else
        {   
            const TInt KNumberOfBitmaps = 16;
            //Set up table of bitmap formats
            const TBufC8<5> bitmapString[] =
            {
                KVersitParam8Gif(),
                KVersitParam8Jpeg(),
                KVersitParam8Bmp(),
                KVersitParam8Tiff(),
                KVersitParam8Pict(),
                KVersitParam8Cgm(),
                KVersitParam8Wmf(),
                KVersitParam8Ps(),
                KVersitParam8Pdf(),
                KVersitParam8Mpeg(),
                KVersitParam8Mpeg2(),
                KVersitParam8Avi(),
                KVersitParam8Qtime(),   
                KVersitParam8Dib(),
                KVersitParam8Pmb(),
                KVersitParam8Met()     
            };
           
            // Loop through the table looking for a param whose name matches the bitmap format
            for (TInt i=0; i < KNumberOfBitmaps - 1; ++i)
                {
                CParserParam* paramNameCheck = aVCardProperty.Param(bitmapString[i]);
                if (paramNameCheck)
                    {//ok found something
                    TFieldType bitmapFormat=MapVCardPhotoTypeToFieldType(paramNameCheck->Name());
                    aBitmapFormat.iUid = bitmapFormat.iUid;
                    break;
                    }
                }   
        } //else
   
    }

TFieldType CVCardToContactsAppConverter::MapVCardPhotoTypeToFieldType(/*const CParserParam& aParserParam*/TPtrC8 aBitmapStringPtr)
	{
    if (aBitmapStringPtr.Length()<2)
		{
		return KUidContactFieldNone; //no bitmap type token are less than 2 characters long
		}

    TChar firstChar(aBitmapStringPtr[0]);
	firstChar=firstChar.GetUpperCase();

	switch (firstChar)
		{
	case 'G':
		return KUidContactFieldVCardMapGIF;
	case 'C':
		return KUidContactFieldVCardMapCGM;
	case 'W':
		return KUidContactFieldVCardMapWMF;
	case 'B':
		return KUidContactFieldVCardMapBMP;
	case 'D':
		return KUidContactFieldVCardMapDIB;
	case 'P':
		{
	    TChar secondChar(aBitmapStringPtr[1]);
		switch (secondChar.GetUpperCase())
			{
		case 'S':
			return KUidContactFieldVCardMapPS;
		case 'M':
			return KUidContactFieldVCardMapPMB;
		case 'D':
			return KUidContactFieldVCardMapPDF;
		case 'I':
			return KUidContactFieldVCardMapPICT;
		default:
			return KUidContactFieldNone;
			}				
		}
	case 'T':
		return KUidContactFieldVCardMapTIFF;
	case 'J':
		return KUidContactFieldVCardMapJPEG;
	case 'M':
        switch (aBitmapStringPtr.Length())
			{
		case 3:
			return KUidContactFieldVCardMapMET;
		case 4:
			return KUidContactFieldVCardMapMPEG;
		case 5:
			return KUidContactFieldVCardMapMPEG2;
		default:
			return KUidContactFieldNone;
			}
	case 'A':
		return KUidContactFieldVCardMapAVI;
	case 'Q':
		return KUidContactFieldVCardMapQTIME;
	default:
		return KUidContactFieldNone;
		}
	}


/**
 * Checks each field in an array to see if one or more contain importable data. Assuming one field within
 * the array does, then the whole array should be imported.
 *
 * @param aArray The array of text fields to be checked for importable data
 * @return Whether the array should be imported.
 */
TBool CVCardToContactsAppConverter::TextArrayContainsImportableData(const CDesCArray& aArray) const
	{
	// Text arrays are implicitly composite in nature
	const TInt count = aArray.Count();
	for(TInt i=0; i<count; i++)
		{
		const TPtrC pItem(aArray.MdcaPoint(i));
		if	(CContactVCardConverter::ContainsImportableData(pItem, CContactVCardConverter::EPropertyValueComposite, ImportType()))
			return ETrue;
		}
	//
	return EFalse;
	}

/**
 * Remove single space character entries from the specified composite property array and replace them
 * with null values.
 *
 * The TimeIS sync engines use spaces and NULLs to indicate special behaviour. Space = supported
 * but empty property. NULL = unsupported property.
 *
 * Contacts model fields do not support composite properties themselves. Instead, the following
 * method is used to converted a composite (array) property to a non composite value:
 *
 * CContactTextField::SetStandardTextArray(...)
 *
 * This method simply concatenates all the array values into one non-composite value. A CRLF
 * is added between each sub-field. Therefore, if a composite property reads thus:
 *
 * {"A","B"}
 *
 * contacts model will store the result as "A\r\n\B".
 *
 * In the case where a composite property from TimeIS contains a supported but empty field value,
 * without special handling, the space character (implicit meaning: "supported but empty")
 * will be interpreted literally and stored within the contacts model field as real data.
 *
 * For example: {"Symbian"," "}
 *
 * The implied meaning of this (in terms of PC sync) is:
 *
 * OrgName: Symbian
 * OrgUnit: (a supported field of the PC PIM, but contains no data at this time).
 *
 * In the above example, without special processing, the resultant contacts model field would read thus:
 *
 * "Symbian\r\n "
 *
 * The trailing space would also be exported during any synchronisation, and therefore introduce
 * unexpected data in the sync chain.
 *
 * This method simply ensures that single space characters are mapped back to their 'real' meaning,
 * i.e. a NULL.
 *
 * @param aArray The composite property which needs fixing up
 */
void CVCardToContactsAppConverter::RemoveWhitespaceFromCompositePropertyL(CDesCArray& aArray)
	{
	const TInt count = aArray.Count();
	for(TInt i=0; i<count; i++)
		{
		TPtrC pItem(aArray[i]);
		if	(pItem == KContactVCardCompositeSupportedButEmptyFieldValue)
			{
			aArray.Delete(i);
			aArray.InsertL(i, KContactVCardEmptyFieldValue);
			}
		}
	}

/**
 * Get a Pointer to the First Instance of the specified Property's value in the VCard Object
 *
 * @param aVCard A vCard Object containing Array of Properties
 * @param aToken A String with desired Property Name
 * @param A buffer descriptor expected to contain the first instance of desired property Value on return
 * @return Count of Class Properties found
 */
TInt CVCardToContactsAppConverter::GetSingleInstanceL(const CParserVCard& aVCard,const TDesC8& aToken, TDes& aClass)
	{
	CArrayPtr<CParserProperty>* arrayOfProp=aVCard.PropertyL(aToken, TUid::Uid(KVersitPropertyHBufCUid), EFalse);
	if(arrayOfProp)
		{
		CleanupStack::PushL(arrayOfProp);
		TInt propCount = arrayOfProp->Count();
		CParserProperty* property = arrayOfProp->At(0);
		CParserPropertyValueHBufC* propertyAsHBufC = static_cast<CParserPropertyValueHBufC*>(property->Value());
		aClass = propertyAsHBufC->Value();
		if( !aClass.Length() )
			{
			propCount = 0;
			}
		CleanupStack::PopAndDestroy(arrayOfProp);
		return 	propCount;
		}
	return 0;
	}

/**
 * Add a specific field into a contact
 *
 * @param aContact Contact item to add fields to
 * @param aValue Reference to first Instance of the Property Value
 * @param aFieldType Field type of field to add (TFieldType)
 * @param aMapping vCard mapping Id of the field to add.
 * @param aNames An object containing the name and labels for 'N' property fields
 * @param aOption Import preferences (available options defined in CContactDatabase::TOptions)
 */
void CVCardToContactsAppConverter::SetSingleInstanceL(CContactItem& aContact,const TDes& aValue,const TFieldType& aFieldType, const TUid& aMapping, TInt aOption)
	{
	//
	TBool addField = EFalse;
	if (CContactVCardConverter::EConnectWhitespace & aOption)
		{
		addField = CContactVCardConverter::ContainsImportableData(aValue, CContactVCardConverter::EPropertyValueSingle, ImportType());
		}
	else
		{
		addField = (aValue.Length());
		}
	// Only add the field if it contains some data
	if	(addField)
		{
		CContactItemField* contactItemField = CContactItemField::NewLC(KStorageTypeText, aFieldType);
		contactItemField->SetMapping(aMapping);
		contactItemField->TextStorage()->SetStandardTextL(aValue);
		aContact.AddFieldL(*contactItemField);
		CleanupStack::Pop(contactItemField);
		}
	}

/**
 * Merge a specific field from a contact
 *
 * @param aContact Contact item to add fields to
 * @param aValue Pointer to first Instance of the Property Value
 * @param aFieldType Field type of field to add (TFieldType)
 * @param aMapping vCard mapping Id of the field to add.
 * @param aNames An object containing the name and labels for 'N' property fields
 * @param aOption Import preferences (available options defined in CContactDatabase::TOptions)
 */
void CVCardToContactsAppConverter::MergeSingleInstanceL(CContactItem& aContact, const TDes& aValue,const TFieldType& aFieldType, const TUid& aMapping, TInt aOption)
	{
	CContactItemFieldSet& oldfieldset = aContact.CardFields();
	const TInt pos = oldfieldset.Find(aFieldType, aMapping);
	const TBool processWhitespace = (aOption & CContactVCardConverter::EConnectWhitespace);

	// First check whether the field is present in the contact card
	// Also verify that the array of address sub-values actually contains a specific
	// value for the requested index.
	const TInt Klength = aValue.Length();
	if (processWhitespace)
		{
		TBool isSingleSpace = EFalse;
		if (Klength == 1)
			{
			isSingleSpace = (aValue[0] == KContactVCardSpaceCharacter);
			}
		if	((pos != KErrNotFound) && (Klength || isSingleSpace))
			{
			// This means the PC side field is empty, so delete the corresponding device-side field.
			aContact.RemoveField(pos);
			}
		if	(Klength && !isSingleSpace)
			{
			// This means the PC side field is unsupported, so ignore the corresponding contents.
			SetSingleInstanceL(aContact, aValue, aFieldType,aMapping,aOption);
			}
		}
	else
		{
		if (pos != KErrNotFound)
			{
			// This means the PC side field is empty, so delete the corresponding device-side field.
			aContact.RemoveField(pos);
			}
		if (Klength)
			{
			// This means the PC side field is not empty, so add the corresponding contents.
			SetSingleInstanceL(aContact, aValue, aFieldType,aMapping,aOption);
			}
		}
	}

/**
 * Get an Array containing Property's values from a VCard
 *
 * @param aVCard A vCard Object containing Array of Properties
 * @param aToken A String with desired Property Name
 * @param A Composite Descriptor array with desired vCard Property's Values on return
 * @return Count of desired properties found.
 */

TInt CVCardToContactsAppConverter::GetVCardPropertyAsArrayOfValuesL(const CParserVCard& aVCard, const TDesC8& aToken, CDesCArray& aItems)
	{
	CArrayPtr<CParserProperty>* arrayOfProp =aVCard.PropertyL(aToken,TUid::Uid(KVersitPropertyCDesCArrayUid),EFalse);
	TInt propCount = 0;
	if(arrayOfProp)
		{
		CleanupStack::PushL(arrayOfProp);
		propCount = arrayOfProp->Count();
		CParserProperty* property = arrayOfProp->At(0);
		CDesCArray& value = *static_cast<CParserPropertyValueCDesCArray*>(property->Value())->Value();
		if(value.Count())
			{
			for(TInt i = 0;i < value.Count();i++)
				{
				aItems.AppendL(value.MdcaPoint(i));
				}
			}
		else
			{
			propCount = 0;
			}
		CleanupStack::PopAndDestroy(arrayOfProp);
		}

	return propCount;
	}

/**
 * Add Organization Information like Company Name and Department Name into contact
 *
 * @param aContact Contact item to add fields to
 * @param aItems A CDesC Array containing the Property's value
 * @param aOption Import preferences (available options defined in CContactDatabase::TOptions)
 */
void CVCardToContactsAppConverter::SetOrgDetailsL(CContactItem& aContact,CDesCArray& aItems,const TInt aOption)
	{
	TInt orgCount = aItems.MdcaCount();
	SetSpecificFieldL(aContact, aItems, KUidContactFieldCompanyName, KUidContactFieldVCardMapORG, aOption, 0, 1);
	SetSpecificFieldL(aContact, aItems, KUidContactFieldDepartmentName,	KUidContactFieldVCardMapDepartment, aOption, 1, orgCount);
	}

/**
 * Add a specific field from the Array of Property's values into contact
 *
 * @param aContact Contact item to add fields to
 * @param aItems A CDesC Array containing the Property's values
 * @param aFieldType Field type of field to add (TFieldType)
 * @param aMapping vCardMapping Id of field to add
 * @param aOption Import preferences (available options defined in CContactDatabase::TOptions)
 * @param aStartIndex Starting Index in the Property Value Array
 * @param aEndIndex   Ending Index in the Property Value Array
 */
void CVCardToContactsAppConverter::SetSpecificFieldL(CContactItem& aContact,CDesCArray& aItems,const TUid& aFieldType,const TUid& aMapping,TInt aOption, TInt aStartIndex, TInt aEndIndex)
	{
	TBool doConvert = ETrue;
	TInt ii = 0;

	CDesCArrayFlat* orgList = new (ELeave)CDesCArrayFlat(4);
	CleanupStack::PushL(orgList);

    for (ii = aStartIndex;ii < aEndIndex;ii++)
    	{
    	orgList->AppendL(aItems.MdcaPoint(ii));
    	}
	if ((CContactVCardConverter::EConnectWhitespace & aOption) && orgList)
		{
		doConvert = TextArrayContainsImportableData(*orgList);
		if (doConvert)
			{
			RemoveWhitespaceFromCompositePropertyL(*orgList);
			}
		}
	if (doConvert)
		{
		CContactItemField* contactItemField = CContactItemField::NewLC(KStorageTypeText, aFieldType);
		contactItemField->SetMapping(aMapping);
		contactItemField->TextStorage()->SetStandardTextArray(orgList);
		aContact.AddFieldL(*contactItemField);
		CleanupStack::Pop(contactItemField);
		}

	CleanupStack::PopAndDestroy(orgList); // orgList
}

/**
 * Merge Organization Information like Company Name and Department Name into contact
 *
 * @param aContact Contact item to add fields to
 * @param aItems A CDesC Array containing the Property's value
 * @param aFieldType Field type of field to add (TFieldType)
 * @param aMapping vCardMapping Id of field to add
 * @param aOption Import preferences (available options defined in CContactDatabase::TOptions)
 * @param aStartIndex Starting Index in the Property Value Array
 * @param aEndIndex   Ending Index in the Property Value Array
 */
void CVCardToContactsAppConverter::MergeOrgDetailsL(CContactItem& aContact, CDesCArray& aItems, TInt aOption)
	{
	TInt orgCount = aItems.MdcaCount();
	MergeSpecificFieldL(aContact, aItems, KUidContactFieldCompanyName,	KUidContactFieldVCardMapORG, aOption, 0, 1);
	MergeSpecificFieldL(aContact, aItems, KUidContactFieldDepartmentName,	KUidContactFieldVCardMapDepartment, aOption, 1, orgCount);
	}

/**
 * Merge specific Field from the array of Property's values into contact
 *
 * @param aContact Contact item to add fields to
 * @param aItems A CDesC Array containing the Property's value
 * @param aFieldType Field type of field to add (TFieldType)
 * @param aMapping vCardMapping Id of field to add
 * @param aOption Import preferences (available options defined in CContactDatabase::TOptions)
 * @param aStartIndex Starting Index in the Property Value Array
 * @param aEndIndex   Ending Index in the Property Value Array
 */
void CVCardToContactsAppConverter::MergeSpecificFieldL(CContactItem& aContact, CDesCArray& aItems, const TFieldType& aFieldType, const TUid& aMapping, TInt aOption, TInt aStartIndex, TInt aEndIndex)
	{
	CContactItemFieldSet& oldfieldset = aContact.CardFields();
	const TInt pos = oldfieldset.Find(aFieldType, aMapping);
	const TBool processWhitespace = (aOption & CContactVCardConverter::EConnectWhitespace);

	// First check whether the field is present in the contact card
	// Also verify that the array of address sub-values actually contains a specific
	// value for the requested index.
	
	if(aItems.MdcaCount() > aStartIndex)
		{	
		const TPtrC pValue = aItems.MdcaPoint(aStartIndex);
		const TInt Klength = pValue.Length();
		if (processWhitespace)
			{
			TBool isSingleSpace = EFalse;
			if (Klength == 1)
				{
				isSingleSpace = (pValue[0] == KContactVCardSpaceCharacter);
				}
			if	((pos != KErrNotFound) && (Klength || isSingleSpace))
				{
				// This means the PC side field is empty, so delete the corresponding device-side field.
				aContact.RemoveField(pos);
				}
			if	(Klength && !isSingleSpace)
				{
				// This means the PC side field is unsupported, so ignore the corresponding contents.
				SetSpecificFieldL(aContact, aItems, aFieldType,	aMapping, aOption, aStartIndex, aEndIndex);
				}
			}
		else
			{
			if (pos != KErrNotFound)
				{
				// This means the PC side field is empty, so delete the corresponding device-side field.
				aContact.RemoveField(pos);
				}
			if (Klength)
				{
				// This means the PC side field is not empty, so add the corresponding contents.
				SetSpecificFieldL(aContact, aItems, aFieldType,	aMapping, aOption, aStartIndex, aEndIndex);
				}
			}
		}
	else if(pos != KErrNotFound)
		{
		aContact.RemoveField(pos);
		}
	}		

/**
 * Check if a specific field from vCard should be truncated or not.
 *
 * @param aPropertyName vCard property's name
 *
 * @return ETrue if the property must be truncated and EFalse otherwise
 *
 */
TBool CVCardToContactsAppConverter::PropertyTobeTruncated(const TPtrC8& aPropertyName) const
	{
	//Test is the field is one that has to be truncated
	if( aPropertyName == KVersitTokenN || aPropertyName == KVersitTokenORG || 
		aPropertyName == KVersitTokenTEL || aPropertyName == KVersitTokenEMAIL )
		return ETrue;
	//	
	return EFalse;
	}

/**
 * Check if a specific parameter value exists in a parameter array.
 *
 * @param aParamArray Array of property parameters in which value has to be searched.
 * @param aParamName A parameter name whose corresponding parameter value present in aParamArray has to be matched with aParamValue.
 * @param aParamValue A parameter value which will be used to search.
 * @return TBool ETrue if value is found in parameter array, otherwise EFalse.
 */
TBool CVCardToContactsAppConverter::IsParameterValuePresent(CArrayPtr<CParserParam>* aParamArray, const TDesC8& aParamName, const TDesC8& aParamValue)
	{
	if(!aParamArray)
		{
		return EFalse;
		}
	const TInt count = aParamArray->Count();
	for (TInt ii = 0; ii < count; ii++)
		{
		if((*aParamArray)[ii]->Name().CompareF(aParamValue) == 0)
			{
			return ETrue;		
			}
		else if((*aParamArray)[ii]->Name().CompareF(aParamName) == 0)
			{
			if((*aParamArray)[ii]->Value().CompareF(aParamValue) == 0)
				{
				return ETrue;		
				}	
			}
		}
	return EFalse;	
	}

/**
 * If there are multiple EMAIL and TEL properties in a vCard, then there should only be a single EMAIL or TEL property 
 * with a PREF parameter. If more than one EMAIL or TEL properties contain a PREF parameter,then only the first property
 * with the PREF parameter is stored and the other properties are stored with their other parameters but without the
 * PREF parameter.
 * 
 * @param aMainContact, Contact item for the vCard
 * @param aFieldType, the fieldtype in conjuction with the mapping is used to search for fields.
 * @param aMapping, the mapping is used for searching a field i.e. EMAIL or TEL fields.
 */

void CVCardToContactsAppConverter::AdjustForPrefRule(CContactItem& aContact, TFieldType aFieldType, TUid aMapping)
	{
	TInt pos = 0;
	TInt prefCount = 0;
	CContactItemFieldSet& contactItemFieldSet = aContact.CardFields();
	pos = contactItemFieldSet.FindNext(aFieldType, aMapping, pos);
	while (pos != KErrNotFound)
		{
		CContactItemField& contactItemField = contactItemFieldSet[pos];
		const CContentType& contentType = contactItemField.ContentType();
		TBool pref = contentType.ContainsFieldType(KUidContactFieldVCardMapPREF);
		if (pref)
			{
			++prefCount;
			if (prefCount > 1)
				{
				contactItemField.RemoveFieldType(KUidContactFieldVCardMapPREF);
				}
			}
		++pos;
		pos = contactItemFieldSet.FindNext(aFieldType, aMapping, pos);
		}
	}
