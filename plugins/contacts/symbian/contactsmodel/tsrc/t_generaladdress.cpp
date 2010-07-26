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

#include <e32test.h>
#include <f32file.h>
#include <s32file.h>
#include <cntdb.h>
#include <cntdef.h>
#include <cntitem.h>
#include <cntfield.h>
#include <cntfldst.h>
#include <bautils.h>
#include <barsc.h>
#include <barsread.h>
#include <versit.h>
#include <vcard.h>
#include <e32def.h>
#include <coreappstest/testserver.h>
// User includes
#include "t_utils.h"

// Flags
#define OOM_TESTS

// Globals
static RTest TheTest(_L("T_GENERALADDRESS - Test General Address Export and Import"));
static CCntTest* TheTestHelper;

// Type definitions
typedef void (*TGAFunction)(TBool& aPrintedTestTitle);

// Constants
const TInt KIntContactFieldVCardMapGENERALADDRESS = 0;
const TUid KUidContactFieldVCardMapGENERALADDRESS = { KIntContactFieldVCardMapGENERALADDRESS };

// Literal constants
_LIT(KTestDatabaseFile, "C:GeneralAddressDatabase.cdb");
//
_LIT(KTestNameFamily, "Bloggs");
_LIT(KTestNameGiven, "Joe");
//
_LIT(KTestAddressHOME, "_HOME");
_LIT(KTestAddressWORK, "_WORK");
_LIT(KTestAddressPREF, "_PREF");
_LIT(KTestAddressGENERALADDRESS, "_GENERALADDRESS");
//
_LIT(KTestAddressPOSTOFFICE, "POSTOFFICE");
_LIT(KTestAddressEXTENDEDADR, "EXTENDEDADR");
_LIT(KTestAddressADR, "ADR");
_LIT(KTestAddressLOCALITY, "LOCALITY");
_LIT(KTestAddressREGION, "REGION");
_LIT(KTestAddressPOSTCODE, "POSTCODE");
_LIT(KTestAddressCOUNTRY, "COUNTRY");
//
_LIT(KTestVCardExportFileName, "C:\\T_GENERALADDRESS_EXPORTED.TXT");
_LIT(KTestVCardImportFileName, "C:\\T_GENERALADDRESS_IMPORTED.TXT");
//
_LIT8(KVersitParam8GeneralAddress, "");


// Utility methods

/**
 * Create a simple contact with just the name fields populated
 */
static TContactItemId CreateContactL(CContactDatabase& aDatabase)
	{
	const TContactItemId templateId = aDatabase.TemplateId();
	//
	CContactItem* templateCard = aDatabase.ReadContactLC(templateId);
	CContactCard* card = CContactCard::NewL(templateCard);
	CleanupStack::PushL(card);
	//
	SetNameL(*card, KUidContactFieldFamilyName, KUidContactFieldVCardMapUnusedN, KTestNameFamily, EFalse);
	SetNameL(*card, KUidContactFieldGivenName, KUidContactFieldVCardMapUnusedN, KTestNameGiven, EFalse);
	//
	const TContactItemId id = aDatabase.AddNewContactL(*card);
	CleanupStack::PopAndDestroy(2, templateCard);
	return id;
	}

/**
 * Combine a value and a postfix to create a real unified value
 */
static HBufC* ConstructAddressValueLC(const TDesC& aValue, const TDesC& aPostFix)
	{
	HBufC* value = HBufC::NewLC(aValue.Length() + aPostFix.Length());
	TPtr pValue(value->Des());
	pValue = aValue;
	pValue.Append(aPostFix);
	//
	return value;
	}

/**
 * Add a specific address field to a vCard
 */
static void AddAddressFieldL(CContactItem& aContact, TFieldType aFieldType, TUid aMapping, TFieldType aExtraFieldType, const TDesC& aValue, const TDesC& aValuePostFix)
	{
	CContentType* content = CContentType::NewL(aFieldType, aMapping);
	CleanupStack::PushL(content);
	content->AddFieldTypeL(aExtraFieldType);
	//
	CContactItemField* contactItemField = CContactItemField::NewLC(KStorageTypeText, *content);
	//
	HBufC* value = ConstructAddressValueLC(aValue, aValuePostFix);
	//
	contactItemField->TextStorage()->SetStandardTextL(*value);
	CleanupStack::PopAndDestroy(value);
	//
	aContact.AddFieldL(*contactItemField);
	CleanupStack::Pop(contactItemField);
	//
	CleanupStack::PopAndDestroy(content);
	}

/**
 * Return the post fix descriptor for a given field type (HOME, WORK, PREF, General)
 */
static TPtrC PostFixForFieldType(TFieldType aType)
	{
	TPtrC postFix(KNullDesC);
	switch(aType.iUid)
		{
	case KIntContactFieldVCardMapHOME:
		postFix.Set(KTestAddressHOME);
		break;
	case KIntContactFieldVCardMapWORK:
		postFix.Set(KTestAddressWORK);
		break;
	case KIntContactFieldVCardMapPREF:
		postFix.Set(KTestAddressPREF);
		break;
	case KIntContactFieldVCardMapGENERALADDRESS:
		postFix.Set(KTestAddressGENERALADDRESS);
		break;
	default:
		TheTest(EFalse);
		break;
		}
	//
	return postFix;
	}

/**
 * Add a specific address type to a contact card
 */
static void AddAddressL(CContactDatabase& aDatabase, TContactItemId aId, TFieldType aExtraFieldType)
	{
	const TPtrC postFix(PostFixForFieldType(aExtraFieldType));
	//
	CContactItem* card = aDatabase.OpenContactLX(aId);
	CleanupStack::PushL(card);
	//
	AddAddressFieldL(*card, KUidContactFieldPostOffice, KUidContactFieldVCardMapPOSTOFFICE, aExtraFieldType, KTestAddressPOSTOFFICE, postFix);
	AddAddressFieldL(*card, KUidContactFieldExtendedAddress, KUidContactFieldVCardMapEXTENDEDADR, aExtraFieldType, KTestAddressEXTENDEDADR, postFix);
	AddAddressFieldL(*card, KUidContactFieldAddress, KUidContactFieldVCardMapADR, aExtraFieldType, KTestAddressADR, postFix);
	AddAddressFieldL(*card, KUidContactFieldLocality, KUidContactFieldVCardMapLOCALITY, aExtraFieldType, KTestAddressLOCALITY, postFix);
	AddAddressFieldL(*card, KUidContactFieldRegion, KUidContactFieldVCardMapREGION, aExtraFieldType, KTestAddressREGION, postFix);
	AddAddressFieldL(*card, KUidContactFieldPostcode, KUidContactFieldVCardMapPOSTCODE, aExtraFieldType, KTestAddressPOSTCODE, postFix);
	AddAddressFieldL(*card, KUidContactFieldCountry, KUidContactFieldVCardMapCOUNTRY, aExtraFieldType, KTestAddressCOUNTRY, postFix);
	//
	aDatabase.CommitContactL(*card);
	CleanupStack::PopAndDestroy(card);
	CleanupStack::Pop(); // lock record
	}

/**
 * Export a specific contact to a vCard file
 */
static void ExportContactAsVCardL(CContactDatabase& aDatabase, TContactItemId aId, RFs& aFsSession, const TDesC& aVCardFileName)
	{
	CContactIdArray* idArray = CContactIdArray::NewLC();
	idArray->AddL(aId);
	//
	RFile outfile;
	const TInt error = outfile.Replace(aFsSession, aVCardFileName, EFileWrite);
	User::LeaveIfError(error);
	CleanupClosePushL(outfile);
	//
	RFileWriteStream writeStream(outfile);
	CleanupClosePushL(writeStream);
	//
	const TUid KVersitVCardUid = { KUidVCardConvDefaultImpl };
	aDatabase.ExportSelectedContactsL(KVersitVCardUid, *idArray, writeStream, CContactDatabase::EDefault);
	writeStream.CommitL();
	CleanupStack::PopAndDestroy(3, idArray); // writeStream, outfile, idArray
	}

/**
 * Parse a file using the Symbian vCard parser and return the parsed data structures
 * to the caller
 */
static CParserVCard* ParseVCardFileLC(RFs& aFsSession, const TDesC& aVCardFileName)
	{
	CParserVCard* parser = CParserVCard::NewL();
	CleanupStack::PushL(parser);
	//
	RFile readFile;
	const TInt error = readFile.Open(aFsSession, aVCardFileName, EFileRead);
	User::LeaveIfError(error);
	CleanupClosePushL(readFile);
	//
	RFileReadStream readStream(readFile);
	CleanupClosePushL(readStream);
	parser->InternalizeL(readStream);
	CleanupStack::PopAndDestroy(2); // readStream, readFile
	//
	return parser;
	}

/**
 * Returns the property parameter name value used by Versit for this particular
 * Contacts Model field type (E.g. HOME, WORK, PREF, etc). Returns KNullDesC in
 * the case of General Address. Panic (fails the test) if the type is not recognised.
 */
static TPtrC8 VersitPropertyParameterNameForContactsModelFieldType(TFieldType aAddressType)
	{
	TPtrC8 propParamValue(KNullDesC8);
	//
	switch(aAddressType.iUid)
		{
	case KIntContactFieldVCardMapHOME:
		propParamValue.Set(KVersitParam8Home);
		break;
	case KIntContactFieldVCardMapWORK:
		propParamValue.Set(KVersitParam8Work);
		break;
	case KIntContactFieldVCardMapPREF:
		propParamValue.Set(KVersitParam8Pref);
		break;
	case KIntContactFieldVCardMapGENERALADDRESS:
		break;
	default:
		TheTest(EFalse);
		break;
		}
	//
	return propParamValue;
	}

/**
 * Search a vCard property for the presence of a specific address type, e.g. HOME, WORK, PREF or "General"
 */
static TBool ValidateVCardPropertyForAddressType(const CParserProperty& aProperty, const TDesC8& aPropertyParameterType)
	{
	TBool validated = EFalse;

	// Have to perform more substancial checks if we're looking for just a general address...
	if	(aPropertyParameterType.Length())
		{
		// HOME, WORK, PREF
		validated = (aProperty.Param(aPropertyParameterType) != NULL);
		}
	else
		{
		// In the case of General Address, which won't have an associated property parameter
		// we must check that none of the other types are present for this address. If none are
		// matched, then this address can be considered general.
		const TBool foundHOME = (aProperty.Param(KVersitParam8Home) != NULL);
		const TBool foundWORK = (aProperty.Param(KVersitParam8Work) != NULL);
		const TBool foundPREF = (aProperty.Param(KVersitParam8Pref) != NULL);
		//
		validated = !(foundHOME || foundWORK || foundPREF);
		}
	//
	return validated;
	}

/**
 * Check that the specified actual value found in the vCard matches the expected value
 * which is constructed from the raw value and post fix.
 */
static TBool ValidateAddressSubFieldL(const TDesC& aActualValue, const TDesC& aExpectedRawValue, const TDesC& aPostFix)
	{
	HBufC* expectedValue = ConstructAddressValueLC(aExpectedRawValue, aPostFix);
	const TBool match = (expectedValue->CompareF(aActualValue) == 0);
	CleanupStack::PopAndDestroy(expectedValue);
	//
	return match;
	}

/**
 * Check that the specified array contains exact matches for each of the 7 address
 * sub field values.
 */
static TBool ValidateAddressSubFieldValuesL(const CDesCArray& aArray, const TDesC& aPostFix)
	{
	TPtrC actualValue;
	//
	actualValue.Set(aArray[0]);
	if	(!ValidateAddressSubFieldL(actualValue, KTestAddressPOSTOFFICE, aPostFix))
		return EFalse;
	actualValue.Set(aArray[1]);
	if	(!ValidateAddressSubFieldL(actualValue, KTestAddressEXTENDEDADR, aPostFix))
		return EFalse;
	actualValue.Set(aArray[2]);
	if	(!ValidateAddressSubFieldL(actualValue, KTestAddressADR, aPostFix))
		return EFalse;
	actualValue.Set(aArray[3]);
	if	(!ValidateAddressSubFieldL(actualValue, KTestAddressLOCALITY, aPostFix))
		return EFalse;
	actualValue.Set(aArray[4]);
	if	(!ValidateAddressSubFieldL(actualValue, KTestAddressREGION, aPostFix))
		return EFalse;
	actualValue.Set(aArray[5]);
	if	(!ValidateAddressSubFieldL(actualValue, KTestAddressPOSTCODE, aPostFix))
		return EFalse;
	actualValue.Set(aArray[6]);
	if	(!ValidateAddressSubFieldL(actualValue, KTestAddressCOUNTRY, aPostFix))
		return EFalse;
	//
	return ETrue;
	}

/**
 * Check a vCard for the presence of a specific address type. Returns ETrue
 * if the type is found within the vCard.
 */
static TBool ValidateVCardForAddressL(CParserVCard& aVCard, TFieldType aAddressType)
	{
	TBool validated = EFalse;
	const TUid KVersitAddressPropertyValueType = { KVersitPropertyCDesCArrayUid };
	const TPtrC8 propValueType = VersitPropertyParameterNameForContactsModelFieldType(aAddressType);
	//
	CArrayPtr<CParserProperty>* properties = aVCard.PropertyL(KVersitTokenADR, KVersitAddressPropertyValueType, EFalse);
	if	(properties)
		{
		CleanupStack::PushL(properties);
		const TInt count = properties->Count();
		for(TInt i=0; i<count || !validated; i++)
			{
			// First verify the property parameters
			CParserProperty& property = *properties->At(i);
			if	(ValidateVCardPropertyForAddressType(property, propValueType))
				{
				// Next verify the property values
				CParserPropertyValueCDesCArray* value = static_cast<CParserPropertyValueCDesCArray*>(property.Value());
				CDesCArray* array = value->Value();

				// There must be 7 sub-fields
				if	(array->MdcaCount() == 7)
					{
					// Check each sub-field for the correct value
					const TPtrC postFix = PostFixForFieldType(aAddressType);
					validated = ValidateAddressSubFieldValuesL(*array, postFix);
					}
				}
			}
		CleanupStack::PopAndDestroy(properties);
		}
	return validated;
	}

/**
 * Create a vCard N property and add it to the specified vCard parser object
 */
static void AddNameToVCardL(CParserVCard& aVCard)
	{
	// For the property value itself
	CDesCArrayFlat* desArray = new (ELeave)CDesCArrayFlat(4);
	CleanupStack::PushL(desArray);

	// Add the names
	desArray->AppendL(KTestNameFamily);
	desArray->AppendL(KTestNameGiven);
	desArray->AppendL(KNullDesC);
	desArray->AppendL(KNullDesC);
	desArray->AppendL(KNullDesC);

	// Create the property value
	CParserPropertyValue* value = new(ELeave) CParserPropertyValueCDesCArray(desArray);
	CleanupStack::Pop(desArray);
	CleanupStack::PushL(value);
	//
	CParserGroupedProperty* property = CParserGroupedProperty::NewL(*value, KVersitTokenN, NULL, NULL);
	CleanupStack::Pop(value); // value

	// This pushes property before anything can leave...
	aVCard.AddPropertyL(property);
	}

/**
 * Combine the address data and the post fix to form a new combined address data value
 * and append it to the specified array
 */
static void AddAddressSubFieldToArrayL(CDesCArray& aArray, const TDesC& aAddress, const TDesC& aPostFix)
	{
	HBufC* value = ConstructAddressValueLC(aAddress, aPostFix);
	aArray.AppendL(*value);
	CleanupStack::PopAndDestroy(value);
	}

/**
 * Create a vCard ADR property and add it to the specified vCard parser object
 */
static void AddAddressToVCardL(CParserVCard& aVCard, const TDesC8& aAddressType)
	{
	// For the property parameters
    CArrayPtr<CParserParam>* arrayOfParams = new(ELeave) CArrayPtrFlat<CParserParam>(5);
	CleanupStack::PushL(TCleanupItem(CleanUpResetAndDestroy, arrayOfParams));

	// If the address type is NULL then its a general address, in which case it doesn't
	// need a property parameter. Otherwise, assume it does
	if	(aAddressType.Length())
		{
		CParserParam* parserParam = CParserParam::NewL(aAddressType, KNullDesC);
		CleanupStack::PushL(parserParam);
		arrayOfParams->AppendL(parserParam);
		CleanupStack::Pop(parserParam);
		}

	// Create the value array
	CDesCArrayFlat* desArray = new (ELeave)CDesCArrayFlat(4);
	CleanupStack::PushL(desArray);

	// Pick the correct postfix
	TPtrC postFix(KNullDesC);
	if	(aAddressType == KVersitParam8Home)
		postFix.Set(KTestAddressHOME);
	else if (aAddressType == KVersitParam8Work)
		postFix.Set(KTestAddressWORK);
	else if (aAddressType == KVersitParam8Pref)
		postFix.Set(KTestAddressPREF);
	else if (aAddressType == KVersitParam8GeneralAddress)
		postFix.Set(KTestAddressGENERALADDRESS);

	// Add the names
	AddAddressSubFieldToArrayL(*desArray, KTestAddressPOSTOFFICE, postFix);
	AddAddressSubFieldToArrayL(*desArray, KTestAddressEXTENDEDADR, postFix);
	AddAddressSubFieldToArrayL(*desArray, KTestAddressADR, postFix);
	AddAddressSubFieldToArrayL(*desArray, KTestAddressLOCALITY, postFix);
	AddAddressSubFieldToArrayL(*desArray, KTestAddressREGION, postFix);
	AddAddressSubFieldToArrayL(*desArray, KTestAddressPOSTCODE, postFix);
	AddAddressSubFieldToArrayL(*desArray, KTestAddressCOUNTRY, postFix);

	// Create the property value
	CParserPropertyValue* value = new (ELeave) CParserPropertyValueCDesCArray(desArray);
	CleanupStack::Pop(desArray);
	CleanupStack::PushL(value);

	// And finally, the property itself
	CParserGroupedProperty* property = CParserGroupedProperty::NewL(*value, KVersitTokenADR, NULL, arrayOfParams);
	CleanupStack::Pop(2, arrayOfParams); // value, arrayOfParams

	// Add to vCard object - takes ownership of property immediately
	aVCard.AddPropertyL(property);
	}

/**
 * Create a simple vCard which contains only an N (Name) property
 */
static CParserVCard* CreateVCardLC()
	{
	CParserVCard* parser = CParserVCard::NewL();
	CleanupStack::PushL(parser);
	//
	AddNameToVCardL(*parser);
	//
	return parser;
	}

/**
 * Export the specified vCard object to a file
 */
static void CreateVCardFileFromVCardObjectL(CParserVCard& aVCard, RFs& aFsSession, const TDesC& aVCardFileName)
	{
	RFile outfile;
	const TInt error = outfile.Replace(aFsSession, aVCardFileName, EFileWrite);
	User::LeaveIfError(error);
	CleanupClosePushL(outfile);
	//
	RFileWriteStream writeStream(outfile);
	CleanupClosePushL(writeStream);
	//
	aVCard.ExternalizeL(writeStream);
	writeStream.CommitL();
	CleanupStack::PopAndDestroy(2); // writeStream, outfile
	}

/**
 * Import the specified vCard to the contacts database and return the newly created contact item
 */
static CContactItem* ImportVCardLC(CContactDatabase& aDatabase, RFs& aFsSession, const TDesC& aVCardFileName)
	{
	RFile readFile;
	const TInt error = readFile.Open(aFsSession, aVCardFileName, EFileRead);
	User::LeaveIfError(error);
	CleanupClosePushL(readFile);
	//
	RFileReadStream readStream(readFile);
	CleanupClosePushL(readStream);
	//
	TBool importSuccessful = EFalse;
	const TInt options = CContactDatabase::EDefault;
	const TUid KVersitVCardUid = { KUidVCardConvDefaultImpl };
	//
	CArrayPtr<CContactItem>* contacts = aDatabase.ImportContactsL(KVersitVCardUid, readStream, importSuccessful, options);
	CleanupStack::PopAndDestroy(2); // readStream, readFile

	// Check import results
	TheTest(importSuccessful);
	TheTest(contacts != NULL);
	TheTest(contacts->Count() == 1);

	// Delete container, but not the contact itself
	CContactItem* item = contacts->At(0);
	CleanupStack::PushL(item);
	delete contacts;
	return item;
	}

/**
 * Check the specified field set for the presence of a specific field which conforms
 * to the specified address type
 */
static TBool LocateAddressFieldByType(CContactItemFieldSet& aFieldSet, TFieldType aFieldType, TUid aMapping, const TDesC& aExpectedValue, const TDesC& aExpectedPostFix)
	{
	TBool located = EFalse;

	// Locate field
	TInt pos = KErrNotFound;
	if	(aFieldType != KNullUid) // KNullUid is for general address
		pos = aFieldSet.Find(aFieldType, aMapping);
	else
		pos = aFieldSet.Find(aMapping);
	//
	if	(pos != KErrNotFound)
		{
		// Make real value
		HBufC* value = ConstructAddressValueLC(aExpectedValue, aExpectedPostFix);

		// Verify
		CContactItemField& field = aFieldSet[pos];
		CContactTextField* storage = field.TextStorage();
		located = (storage->Text().CompareF(*value) == 0);
		//
		CleanupStack::PopAndDestroy(value);
		}
	//
	return located;
	}

/**
 * Check the specified contact card contains an address corresponding to the specified type
 */
static TBool ValidateContactForAddressTypeL(CContactItem& aContact, TFieldType aAddressType)
	{
	CContactItemFieldSet& fieldSet = aContact.CardFields();
	const TPtrC postFix(PostFixForFieldType(aAddressType));
	//
	if	(!LocateAddressFieldByType(fieldSet, aAddressType, KUidContactFieldVCardMapPOSTOFFICE, KTestAddressPOSTOFFICE, postFix))
		return EFalse;
	if	(!LocateAddressFieldByType(fieldSet, aAddressType, KUidContactFieldVCardMapEXTENDEDADR, KTestAddressEXTENDEDADR, postFix))
		return EFalse;
	if	(!LocateAddressFieldByType(fieldSet, aAddressType, KUidContactFieldVCardMapADR, KTestAddressADR, postFix))
		return EFalse;
	if	(!LocateAddressFieldByType(fieldSet, aAddressType, KUidContactFieldVCardMapLOCALITY, KTestAddressLOCALITY, postFix))
		return EFalse;
	if	(!LocateAddressFieldByType(fieldSet, aAddressType, KUidContactFieldVCardMapREGION, KTestAddressREGION, postFix))
		return EFalse;
	if	(!LocateAddressFieldByType(fieldSet, aAddressType, KUidContactFieldVCardMapPOSTCODE, KTestAddressPOSTCODE, postFix))
		return EFalse;
	if	(!LocateAddressFieldByType(fieldSet, aAddressType, KUidContactFieldVCardMapCOUNTRY, KTestAddressCOUNTRY, postFix))
		return EFalse;
	//
	return ETrue;
	}


// Tests

static void Test1L(TBool& aPrintedTestTitle)
	{
	if	(!aPrintedTestTitle)
		{
		TheTest.Next(_L("EXPORT General address"));

		aPrintedTestTitle = ETrue;
		}
	//
	RFs& fsSession = TheTestHelper->Fs();
	CContactDatabase& database = *TheTestHelper->Db();
	//
	const TContactItemId contactId = CreateContactL(database);
	AddAddressL(database, contactId, KUidContactFieldVCardMapGENERALADDRESS);
	ExportContactAsVCardL(database, contactId, fsSession, KTestVCardExportFileName);
	CParserVCard* parser = ParseVCardFileLC(fsSession, KTestVCardExportFileName);
	TheTest(ValidateVCardForAddressL(*parser, KUidContactFieldVCardMapGENERALADDRESS));
	CleanupStack::PopAndDestroy(parser);
	}

static void Test2L(TBool& aPrintedTestTitle)
	{
	if	(!aPrintedTestTitle)
		{
		TheTest.Next(_L("EXPORT General, Home address"));

		aPrintedTestTitle = ETrue;
		}
	//
	RFs& fsSession = TheTestHelper->Fs();
	CContactDatabase& database = *TheTestHelper->Db();
	//
	const TContactItemId contactId = CreateContactL(database);
	AddAddressL(database, contactId, KUidContactFieldVCardMapGENERALADDRESS);
	AddAddressL(database, contactId, KUidContactFieldVCardMapHOME);
	ExportContactAsVCardL(database, contactId, fsSession, KTestVCardExportFileName);
	CParserVCard* parser = ParseVCardFileLC(fsSession, KTestVCardExportFileName);
	TheTest(ValidateVCardForAddressL(*parser, KUidContactFieldVCardMapGENERALADDRESS));
	TheTest(ValidateVCardForAddressL(*parser, KUidContactFieldVCardMapHOME));
	CleanupStack::PopAndDestroy(parser);
	}

static void Test3L(TBool& aPrintedTestTitle)
	{
	if	(!aPrintedTestTitle)
		{
		TheTest.Next(_L("EXPORT General, Work address"));

		aPrintedTestTitle = ETrue;
		}
	//
	RFs& fsSession = TheTestHelper->Fs();
	CContactDatabase& database = *TheTestHelper->Db();
	//
	const TContactItemId contactId = CreateContactL(database);
	AddAddressL(database, contactId, KUidContactFieldVCardMapGENERALADDRESS);
	AddAddressL(database, contactId, KUidContactFieldVCardMapWORK);
	ExportContactAsVCardL(database, contactId, fsSession, KTestVCardExportFileName);
	CParserVCard* parser = ParseVCardFileLC(fsSession, KTestVCardExportFileName);
	TheTest(ValidateVCardForAddressL(*parser, KUidContactFieldVCardMapGENERALADDRESS));
	TheTest(ValidateVCardForAddressL(*parser, KUidContactFieldVCardMapWORK));
	CleanupStack::PopAndDestroy(parser);
	}


static void Test4L(TBool& aPrintedTestTitle)
	{
	if	(!aPrintedTestTitle)
		{
		TheTest.Next(_L("EXPORT General, Home, Work address"));

		aPrintedTestTitle = ETrue;
		}
	//
	RFs& fsSession = TheTestHelper->Fs();
	CContactDatabase& database = *TheTestHelper->Db();
	//
	const TContactItemId contactId = CreateContactL(database);
	AddAddressL(database, contactId, KUidContactFieldVCardMapGENERALADDRESS);
	AddAddressL(database, contactId, KUidContactFieldVCardMapHOME);
	AddAddressL(database, contactId, KUidContactFieldVCardMapWORK);
	ExportContactAsVCardL(database, contactId, fsSession, KTestVCardExportFileName);
	CParserVCard* parser = ParseVCardFileLC(fsSession, KTestVCardExportFileName);
	TheTest(ValidateVCardForAddressL(*parser, KUidContactFieldVCardMapGENERALADDRESS));
	TheTest(ValidateVCardForAddressL(*parser, KUidContactFieldVCardMapHOME));
	TheTest(ValidateVCardForAddressL(*parser, KUidContactFieldVCardMapWORK));
	CleanupStack::PopAndDestroy(parser);
	}


static void Test5L(TBool& aPrintedTestTitle)
	{
	if	(!aPrintedTestTitle)
		{
		TheTest.Next(_L("IMPORT General address"));

		aPrintedTestTitle = ETrue;
		}
	//
	RFs& fsSession = TheTestHelper->Fs();
	CContactDatabase& database = *TheTestHelper->Db();
	//
	CParserVCard* vCard = CreateVCardLC();
	AddAddressToVCardL(*vCard, KVersitParam8GeneralAddress);
	CreateVCardFileFromVCardObjectL(*vCard, fsSession, KTestVCardImportFileName);
	CleanupStack::PopAndDestroy(vCard);
	CContactItem* item = ImportVCardLC(database, fsSession, KTestVCardImportFileName);
	TheTest(ValidateContactForAddressTypeL(*item, KUidContactFieldVCardMapGENERALADDRESS));
	CleanupStack::PopAndDestroy(item);
	}

static void Test6L(TBool& aPrintedTestTitle)
	{
	if	(!aPrintedTestTitle)
		{
		TheTest.Next(_L("IMPORT General, Home address"));

		aPrintedTestTitle = ETrue;
		}
	//
	RFs& fsSession = TheTestHelper->Fs();
	CContactDatabase& database = *TheTestHelper->Db();
	//
	CParserVCard* vCard = CreateVCardLC();
	AddAddressToVCardL(*vCard, KVersitParam8GeneralAddress);
	AddAddressToVCardL(*vCard, KVersitParam8Home);
	CreateVCardFileFromVCardObjectL(*vCard, fsSession, KTestVCardImportFileName);
	CleanupStack::PopAndDestroy(vCard);
	CContactItem* item = ImportVCardLC(database, fsSession, KTestVCardImportFileName);
	TheTest(ValidateContactForAddressTypeL(*item, KUidContactFieldVCardMapGENERALADDRESS));
	TheTest(ValidateContactForAddressTypeL(*item, KUidContactFieldVCardMapHOME));
	CleanupStack::PopAndDestroy(item);
	}


static void Test7L(TBool& aPrintedTestTitle)
	{
	if	(!aPrintedTestTitle)
		{
		TheTest.Next(_L("IMPORT General, Work address"));

		aPrintedTestTitle = ETrue;
		}
	//
	RFs& fsSession = TheTestHelper->Fs();
	CContactDatabase& database = *TheTestHelper->Db();
	//
	CParserVCard* vCard = CreateVCardLC();
	AddAddressToVCardL(*vCard, KVersitParam8GeneralAddress);
	AddAddressToVCardL(*vCard, KVersitParam8Work);
	CreateVCardFileFromVCardObjectL(*vCard, fsSession, KTestVCardImportFileName);
	CleanupStack::PopAndDestroy(vCard);
	CContactItem* item = ImportVCardLC(database, fsSession, KTestVCardImportFileName);
	TheTest(ValidateContactForAddressTypeL(*item, KUidContactFieldVCardMapGENERALADDRESS));
	TheTest(ValidateContactForAddressTypeL(*item, KUidContactFieldVCardMapWORK));
	CleanupStack::PopAndDestroy(item);
	}


static void Test8L(TBool& aPrintedTestTitle)
	{
	if	(!aPrintedTestTitle)
		{
		TheTest.Next(_L("IMPORT General, Home, Work address"));

		aPrintedTestTitle = ETrue;
		}
	RFs& fsSession = TheTestHelper->Fs();
	CContactDatabase& database = *TheTestHelper->Db();
	//
	CParserVCard* vCard = CreateVCardLC();
	AddAddressToVCardL(*vCard, KVersitParam8GeneralAddress);
	AddAddressToVCardL(*vCard, KVersitParam8Home);
	AddAddressToVCardL(*vCard, KVersitParam8Work);
	CreateVCardFileFromVCardObjectL(*vCard, fsSession, KTestVCardImportFileName);
	CleanupStack::PopAndDestroy(vCard);
	CContactItem* item = ImportVCardLC(database, fsSession, KTestVCardImportFileName);
	TheTest(ValidateContactForAddressTypeL(*item, KUidContactFieldVCardMapGENERALADDRESS));
	TheTest(ValidateContactForAddressTypeL(*item, KUidContactFieldVCardMapHOME));
	TheTest(ValidateContactForAddressTypeL(*item, KUidContactFieldVCardMapWORK));
	CleanupStack::PopAndDestroy(item);
	}

static void RunListOfTestsL(const RArray<TGAFunction>& aTests)
	{
	TBool printedTestTitle = EFalse;
	const TInt count = aTests.Count();
	for(TInt i=0; i<count; i++)
		{
		TGAFunction test = aTests[i];
		TheTestHelper->CreateDatabaseL();
		TRAPD(err, (*test)(printedTestTitle));

		// Ensure that we close the database if a leave occurs or
		// else the OOM test code thinks we've leaked it
		TheTestHelper->CloseDatabase();

		// Cascade error
		User::LeaveIfError(err);

		printedTestTitle = EFalse;
		}
	}

static void TestExportL()
	{
	RArray<TGAFunction> array;
	CleanupClosePushL(array);
	//
	User::LeaveIfError(array.Append(static_cast<TGAFunction>(Test1L)));
	User::LeaveIfError(array.Append(static_cast<TGAFunction>(Test2L)));
	User::LeaveIfError(array.Append(static_cast<TGAFunction>(Test3L)));
	User::LeaveIfError(array.Append(static_cast<TGAFunction>(Test4L)));
	//
	RunListOfTestsL(array);
	//
	CleanupStack::PopAndDestroy(&array);
	}

static void TestImportL()
	{
	RArray<TGAFunction> array;
	CleanupClosePushL(array);
	//
	User::LeaveIfError(array.Append(static_cast<TGAFunction>(Test5L)));
	User::LeaveIfError(array.Append(static_cast<TGAFunction>(Test6L)));
	User::LeaveIfError(array.Append(static_cast<TGAFunction>(Test7L)));
	User::LeaveIfError(array.Append(static_cast<TGAFunction>(Test8L)));
	//
	RunListOfTestsL(array);
	//
	CleanupStack::PopAndDestroy(&array);
	}


static void RunTestsL()
	{
	TRAPD(errExport, TestExportL());
	TheTest(errExport == KErrNone || errExport == KErrNoMemory);
	//
	TRAPD(errImport, TestImportL());
	TheTest(errImport == KErrNone || errImport == KErrNoMemory);
	}

#ifdef OOM_TESTS

static void ExpandCleanupStackL()
	{
	// Reserve some space on the cleanup stack:
		{
		for (TInt ii=0;ii<1000;++ii)
			CleanupStack::PushL(&ii);
		}
	CleanupStack::Pop(1000);
	}

static void PrepareTestSkipsL(RArray<TInt>& aSkipArray, TInt aCount, ...)
	{
	aSkipArray.Reset();
	//
    VA_LIST list;
    VA_START(list, aCount);
	//
	TInt* startPtr = reinterpret_cast<TInt*>(&list); // Same as list[0]
	for(TInt i=0; i<aCount; i++)
		{
		const TInt cycle = startPtr[i];
		aSkipArray.AppendL(cycle);
		}
	}


static TInt RunOOMTestL(TGAFunction aFunction, const RArray<TInt>& aSkipArray)
	{
	//
	TBool printedTestTitle = EFalse;
	TInt ret = KErrNoMemory;
	TInt failAt = 0;
	while(ret != KErrNone)
		{
		failAt += failAt/50 + 1;
		TheTest.Printf(_L("OOM step: %d\n"), failAt);
		if (aSkipArray.Find(failAt) != KErrNotFound)
			continue;
		//
		__UHEAP_MARK;
		TheTestHelper->CreateDatabaseL();

		__UHEAP_SETFAIL(RHeap::EDeterministic, failAt);
		TRAP(ret, aFunction(printedTestTitle));
		__UHEAP_RESET;

		// Ensure that we close the database
		TheTestHelper->CloseDatabase();


		__UHEAP_MARKEND;
		//
		if	(ret != KErrNoMemory && ret != KErrNone)
			{
			TheTest.Printf(_L("Non standard error: %d\n"),ret);
			TheTest.Getch();
			}
		TheTest(ret == KErrNoMemory || ret == KErrNone);
		}
	//
	return KErrNone;
	}

static void RunOOMTestsL()
	{
	ExpandCleanupStackL();
	//
	RArray<TInt> knownFailures;
	CleanupClosePushL(knownFailures);

	// EXPORT
	PrepareTestSkipsL(knownFailures, 0);
	RunOOMTestL(Test1L, knownFailures);
	//
	// (Skip OOM on Test2L, Test3L and Test4L as they execute the same code as Test1L)

	// IMPORT
	PrepareTestSkipsL(knownFailures, 0);
	RunOOMTestL(Test5L, knownFailures);
	//
	// (Skip OOM on Test6L, Test7L and Test8L as they execute the same code as Test5L)

	CleanupStack::PopAndDestroy(&knownFailures);
	}
#endif

static void DoMainL()
	{
	CTestRegister* TempFiles = CTestRegister::NewLC();
	TempFiles->RegisterL(KTestDatabaseFile, EFileTypeCnt);
	TempFiles->RegisterL(KTestVCardExportFileName);
	TempFiles->RegisterL(KTestVCardImportFileName);

	// Run normal testing
	TRAPD(testRet, RunTestsL());
	TheTest(testRet == KErrNone);

	// Run OOM testing
#ifdef OOM_TESTS
	TRAPD(oomRet, RunOOMTestsL());
	TheTest(oomRet == KErrNone);
#endif

	CleanupStack::PopAndDestroy(TempFiles);
	}

/**

@SYMTestCaseID     PIM-T-GENERALADDRESS-0001

*/

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	TheTest.Start(_L("@SYMTESTCaseID:PIM-T-GENERALADDRESS-0001 T_GENERALADDRESS - Test General Address Export and Import"));


	// Stupid test library alert...
	TheTestHelper = new(ELeave) CCntTest();
	TheTestHelper->ConstructL(TheTest, KTestDatabaseFile);

	TRAP_IGNORE(DoMainL());
	//
	TheTestHelper->EndTestLib(KErrNone);
	//
	__UHEAP_MARKEND;
	return(KErrNone);
    }
