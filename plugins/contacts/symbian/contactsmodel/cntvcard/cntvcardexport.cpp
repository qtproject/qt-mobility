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


#include <badesca.h>
#include <vtoken.h>
#include <cntfldst.h>
#include <cntfield.h>
#include "cntvcardutils.h"
#include <cntdef.h>
#include <cntitem.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "vcard3.h"
#include "cntdb_internal.h"
#endif



/**
 * Destructor
 */
CContactsAppToVCardConverter::~CContactsAppToVCardConverter()
    {
    delete iVCard;
    }


/** 
 * Constructor
 * 
 * @param aMachineUniqueId Integer unique to the database
 * @param aCharSet Default character set to pass to Versit parser component
 * @param aVCardVersion Version of vCard being exported
 */
CContactsAppToVCardConverter::CContactsAppToVCardConverter(TInt64 aMachineUniqueId, Versit::TVersitCharSet aCharSet, TVCardVersion aVCardVersion) :
	iMachineUniqueId(aMachineUniqueId),
	iCharSet(aCharSet),
	iVCardVersion(aVCardVersion),
	iFilter(0)
	{
	}


/** 
 * Convert a contact item into a vCard.
 * 
 * @param aMainItem Contact item to convert
 * @param aAgentItem Agent contact item to convert
 * @param aOption Export preferences (options available defined in CContactDatabase::TOptions)
 * @param aExportPrivateFields Specify whether private fields are exported
 * @return vCard representation of contact item
 */
CParserVCard* CContactsAppToVCardConverter::GetContactItemAsVCardL(CContactItem* aMainItem, CArrayPtr<CContactItem>* aAgentItemArray, TInt aOption, TBool aExportPrivateFields)
    {
    switch(iVCardVersion)
		{
		case EVCard21:
		case EPBAPVCard21:
			iVCard = CParserVCard::NewL();
		break;
		
		case EPBAPVCard30:
			iVCard = CParserVCard3::NewL();
		break;
		
		default:
		User::Leave(KErrArgument);
		break;
		}
	iVCard->SetDefaultCharSet(iCharSet);
	EncodeItemPropertiesL(*aMainItem,aOption);
	EncodeFieldPropertiesL(*aMainItem, aAgentItemArray, aOption, aExportPrivateFields);
    CParserVCard* vCard=iVCard;
    iVCard=NULL;
    return vCard;
    }


/** 
 * Encode the properties associated with a contact item which are not fields
 * 
 * @param aContactItem Contact item to export
 * @param aOption Export preferences (options available defined in CContactDatabase::TOptions)
 */
void CContactsAppToVCardConverter::EncodeItemPropertiesL(const CContactItem& aContactItem,TInt aOption)
	{
	TBool pbapExp = IsPBAPExport();
	if( !pbapExp || (pbapExp && NeedsToBeExported(EPropertyREV)) )
		{
		// PREQ234 stipulates that times are stored as UTC time; as long as we
		// put UTC time, we can assume that it's UTC time out.
		CParserPropertyValueDateTime* valueDateTime = CreateDateTimePropertyL(aContactItem.LastModified(), TVersitDateTime::EIsUTC);
		CleanupStack::PushL(valueDateTime);
		CParserProperty* property = CParserGroupedProperty::NewL(*valueDateTime, KVersitTokenREV, NULL, NULL);
		CleanupStack::Pop(valueDateTime);	
		iVCard->AddPropertyL(property); //takes ownership
		}
	if (!(aOption & CContactVCardConverter::EExcludeUid))
		{
		TPtrC guid=aContactItem.UidStringL(iMachineUniqueId);
		CParserPropertyValueHBufC* uidValue=CParserPropertyValueHBufC::NewL(guid);
		CleanupStack::PushL(uidValue);
		CParserProperty* uidproperty=CParserGroupedProperty::NewL(*uidValue,KVersitTokenUID,NULL,NULL);
		CleanupStack::Pop(uidValue);	
		iVCard->AddPropertyL(uidproperty); //takes ownership
		}
	}

    
/** 
 * Convert all CContactItemFields into VCard properties and add them to iVCard.
 * Contact fields with no value (ie. empty fields) are not exported.
 * 
 * @param aMainItem Contact item to convert
 * @param aAgentItem Agent contact item to convert
 * @param aOption Export preferences (options available defined in CContactDatabase::TOptions)
 * @param aExportPrivateFields Specify whether private fields are exported
 */
void CContactsAppToVCardConverter::EncodeFieldPropertiesL(const CContactItem& aContactItem, CArrayPtr<CContactItem>* aAgentItemArray, TInt aOption, TBool aExportPrivateFields)
	{
	CContactItemFieldSet& fieldSet = aContactItem.CardFields();
	TBool pbapExp = IsPBAPExport();
	// Name
	GetVCardNameL(fieldSet, aOption, aExportPrivateFields);
	if(GetExportVersion() == EVCard21)
		{
		GetVCardFormattedNameL(fieldSet,aOption,aExportPrivateFields);
		GetVCardNamePrnL(fieldSet, aOption, aExportPrivateFields);
		// Addresses (home, work, pref and general)
		GetVCardAddressL(aContactItem, KUidContactFieldVCardMapHOME, KVersitParam8Home, aOption, aExportPrivateFields);
		GetVCardAddressL(aContactItem, KUidContactFieldVCardMapWORK, KVersitParam8Work, aOption, aExportPrivateFields);
		GetVCardAddressL(aContactItem, KUidContactFieldVCardMapPREF, KVersitParam8Pref, aOption, aExportPrivateFields);
		GetVCardAddressL(aContactItem, KNullUid, KNullDesC8, aOption, aExportPrivateFields);
		// Organization Information (Company Name and Department Name)
		GetVCardOrgInfoL(fieldSet, aOption, aExportPrivateFields);
		}
	else if(pbapExp)
		{
		//check if the mapping is contained in the filter
		if(NeedsToBeExported(EPropertyFN))				
			{
			GetVCardFormattedNameL(fieldSet, aOption, aExportPrivateFields);
			}
		if(NeedsToBeExported(EPropertySOUND))
			{
			GetVCardNamePrnL(fieldSet, aOption, aExportPrivateFields);	
			}
		// Addresses (home, work, pref and general)
		if(NeedsToBeExported(EPropertyADR))
			{
			GetVCardAddressL(aContactItem, KUidContactFieldVCardMapHOME, KVersitParam8Home, aOption, aExportPrivateFields);
			GetVCardAddressL(aContactItem, KUidContactFieldVCardMapWORK, KVersitParam8Work, aOption, aExportPrivateFields);
			GetVCardAddressL(aContactItem, KUidContactFieldVCardMapPREF, KVersitParam8Pref, aOption, aExportPrivateFields);
			GetVCardAddressL(aContactItem, KNullUid, KNullDesC8, aOption, aExportPrivateFields);	
			}
		if(NeedsToBeExported(EPropertyORG))
			{
			// Organization Information (Company Name and Department Name)
			GetVCardOrgInfoL(fieldSet, aOption, aExportPrivateFields);	
			}
		}
		
	CContactItem* agentItem;
	TInt numAgent = 0;
	// Other fields
	TBool telFound = EFalse;
	const TInt count = fieldSet.Count();
	TInt agentcount=0;
	if(aAgentItemArray &&  aAgentItemArray->Count())
	agentcount = aAgentItemArray->Count();
	for (TInt ii = 0; ii < count; ++ii)

		{
		agentItem = NULL;

		const CContactItemField& field = fieldSet[ii];
		if (!field.Storage()->IsFull())
			continue;

		const CContentType& content=field.ContentType();
		TInt before=1;
		TInt total=0;

		for (TInt jj=0; jj<count; ++jj)
			{
			// This content type comparison should ignore insignificant content types (e.g. Pref et al).
			if (content==fieldSet[jj].ContentType())
				{
				if (jj<ii)
					{
					++before;
					}
				++total;
				}
			}
		if (total==1)
			{
			before=0;
			}
	
		if(field.StorageType() == KStorageTypeContactItemId && agentcount > 0)
			{
			agentItem = aAgentItemArray->At(numAgent);
			++numAgent;
			}

		MapContactFieldToVCardPropertyL(field, agentItem, aOption, before, aExportPrivateFields);
	
		if (field.ContentType().Mapping() == KUidContactFieldVCardMapTEL)
			{//at least one non empty telephone field exists
			telFound = ETrue;
			}
	    }
	    	
	//if the TEL property has been requested then it must be exported. If no non-empty
	//telephone fields have been found at this point create a single empty TEL property
	if (NeedsToBeExported(EPropertyTEL) && !telFound)
		{
		CParserPropertyValue* value = CParserPropertyValueHBufC::NewL(KNullDesC);	
		CleanupStack::PushL(value);
		CParserGroupedProperty* property = CParserGroupedProperty::NewL(*value, KVersitTokenTEL, NULL, NULL);
		CleanupStack::Pop(value);
		//ownership passed
		iVCard->AddPropertyL(property);	
	    }
    }

/**
 * Find an address field which does not correspond to the HOME or WORK field type
 * (i.e. a General Address).
 *
 * @return The index into the field set of the located field, or KErrNotFound if the field was not located
 */
TInt CContactsAppToVCardConverter::FindAddressFieldByMappingAndType(const CContactItemFieldSet& aFields, TFieldType aFieldType, TUid aMapping)
	{
	TInt pos = 0;
	if (aFieldType != KNullUid)
		{
		pos = aFields.Find(aFieldType, aMapping);
		}
	else
		{
		TBool fieldFound = EFalse;
		TInt startPos = KContactFieldSetSearchAll;
				
		while(!fieldFound && !(pos == KErrNotFound))
			{
			pos = aFields.FindNext(aMapping, startPos);
			startPos = pos + 1;
			if (pos != KErrNotFound )
				{
				const CContactItemField& tempField = aFields[pos];
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
	return pos;
	}


/**
 * Retrieve an address field from the contact card's field set
 * 
 * @param aFields Contact item field set
 * @param aParams Array of CParserParam
 * @param aDesArray Descriptor array
 * @param aUid1 Contact model vCard mapping (ie. KUidContactFieldVCardMapXXX)
 * @param aUid2 Contact model vCard mapping (ie. KUidContactFieldVCardMapXXX)
 * @param aLabel Label name to append to standard KContactVCardXDashEPOCCNTMODEL property parameter extension 
 * @param aOption Export preferences (options available defined in CContactvCardConverter::TOptions)
 * @param aFound Field was found and is not empty
 * @param aExportPrivateFields Specify whether private fields are included
 * @param aWithoutPref If ETrue exclude preferred fields
 */
void CContactsAppToVCardConverter::RetrieveAddressFieldL(const CContactItemFieldSet& aFields, CArrayPtr<CParserParam>& aParams, CDesCArrayFlat& aDesArray, TFieldType aFieldType, TUid aMapping, const TDesC& aLabel, TInt aOption, TBool& aFound, TBool aExportPrivateFields, TBool aWithoutPref) const
	{
	const TInt pos = FindAddressFieldByMappingAndType(aFields, aFieldType, aMapping);
	const TBool processWhitespace = (aOption & CContactVCardConverter::EConnectWhitespace);
	if	(pos == KErrNotFound)
		{
		if (processWhitespace)
			{
			aDesArray.AppendL(KContactVCardCompositeSupportedButEmptyFieldValue);
			}
		else
			{
			aDesArray.AppendL(KNullDesC);
			}
		return;
		}
	
	const CContactItemField& field = aFields[pos];
	if (aFieldType == KUidContactFieldVCardMapHOME && field.ContentType().ContainsFieldType(KUidContactFieldVCardMapWORK))
		{
		// Only want to skip this field if it contains both the work mapping and 
		// we have searched on the home mapping.
		// 
		// Next time we come into this function it will be looking for the work mapping
		// i.e. aUid1==KUidContactFieldVCardMapWORK that way if an ADR is both work and home
		// it will only be processed once
		if (processWhitespace)
			{
			aDesArray.AppendL(KContactVCardCompositeSupportedButEmptyFieldValue);
			}
		else
			{
			aDesArray.AppendL(KNullDesC);
			}
		return;
		}

	if (aWithoutPref && field.ContentType().ContainsFieldType(KUidContactFieldVCardMapPREF))
		{
		if (processWhitespace)
			{
			aDesArray.AppendL(KContactVCardCompositeSupportedButEmptyFieldValue);
			}
		return;
		}

	// Assume we're not going to export this field - unless we find some real data
	TBool validItemToExport = EFalse;

	if (!field.IsPrivate() || aExportPrivateFields)
		{
		TBool onCleanupStack = EFalse;
		TPtrC pText(KNullDesC);
		// If the field contains text, then get the text and examine it depending on the flags.
		// Note that the text is created in another buffer, which is pushed onto the stack. This
		// will only happen if there is actually text - thus the need for the onCleanupStack flag.
		if	(field.TextStorage() && field.TextStorage()->IsFull())
			{
			// This step puts the std text onto the cleanup stack - set the flag so it can be 
			// cleaned up later.
			pText.Set(field.TextStorage()->StandardTextLC());
			onCleanupStack = ETrue;
			if (processWhitespace)
				{
				validItemToExport = CContactVCardConverter::ContainsExportableData(pText);
				}
			else
				{
				validItemToExport = ETrue;
				}
			}
		if	(validItemToExport)
			{
			aFound = ETrue;
			aDesArray.AppendL(pText);
			//
			const TBool includeX = aOption & CContactDatabase::EIncludeX;
 			const TPtrC label = field.Label();
  			if	(label.Size() && includeX && field.OverRidesLabel())
   				{
				TBuf8<KContactMaxVCardPropertyNameLength> param(KContactVCardXDashEPOCCNTMODEL);
   				param.Append(aLabel);
				//
				CParserParam* parserParam = CParserParam::NewL(param, label);
				CleanupStack::PushL(parserParam);
   				aParams.AppendL(parserParam);
				CleanupStack::Pop(parserParam);
   				}
			}
		if	(onCleanupStack)
			{
			CleanupStack::PopAndDestroy(); // StandardText
			}
		}
	if (!validItemToExport)
		{
		if (processWhitespace)
			{
			aDesArray.AppendL(KContactVCardCompositeSupportedButEmptyFieldValue);
			}
		else
			{
			aDesArray.AppendL(KNullDesC);
			}
		}
	}


/** 
 * Build up name of VCard from the N and FN properties 
 * 
 * @param aContact Contact item to export
 * @param aOption Export preferences (options available defined in CContactDatabase::TOptions)
 * @param aExportPrivateFields Specify whether private fields are included
 */ 
void CContactsAppToVCardConverter::GetVCardNamePrnL(CContactItemFieldSet& aFields,TInt aOption,TBool aExportPrivateFields)
	{
	// For property parameters (should they be necessary) for this exported property
	CArrayPtr<CParserParam>* arrayOfParams = new(ELeave) CArrayPtrFlat<CParserParam>(5);
	CleanupStack::PushL(TCleanupItem(CleanUpResetAndDestroy,arrayOfParams));

	// For the property value itself
	CDesCArrayFlat* desArray=new (ELeave)CDesCArrayFlat(4);
	CleanupStack::PushL(desArray);
   
	// Export each item in turn (where the field of the specified type is found 
	// in the contact card).
	TInt foundCount = 0;
	foundCount += LocateSpecificNameValueL(aFields, KUidContactFieldFamilyNamePronunciation,	*desArray, *arrayOfParams, KContactVCardLABEL0, aOption, aExportPrivateFields);
	foundCount += LocateSpecificNameValueL(aFields, KUidContactFieldGivenNamePronunciation,		*desArray, *arrayOfParams, KContactVCardLABEL1, aOption, aExportPrivateFields);

  	if (foundCount > 0)
		{
		CParserPropertyValue* value = new(ELeave) CParserPropertyValueCDesCArray(desArray);
	  	CleanupStack::Pop(desArray);
		//
	  	CleanupStack::PushL(value);
	  	CParserGroupedProperty* property = CParserGroupedProperty::NewL(*value,KVersitTokenSOUND, NULL, arrayOfParams);
	  	CleanupStack::Pop(2, arrayOfParams); // value, arrayOfParams
		CleanupStack::PushL(property);
  		CParserParam* param = CParserParam::NewL(KVersitParam8NamePrn, KNullDesC8);
		CleanupStack::PushL(param);
		property->AddParamL(param);
		CleanupStack::Pop(2,property); // + param
		// This pushes property before anything can leave...
		iVCard->AddPropertyL(property);
		}
	else
		{
  		CleanupStack::PopAndDestroy(2, arrayOfParams); // desArray, arrayOfParams
  		}
	}

/** 
 * Build up name of VCard from the N and FN properties 
 * 
 * @param aContact Contact item to export
 * @param aOption Export preferences (options available defined in CContactDatabase::TOptions)
 * @param aExportPrivateFields Specify whether private fields are included
 */ 
void CContactsAppToVCardConverter::GetVCardNameL(CContactItemFieldSet& aFields,TInt aOption,TBool aExportPrivateFields)
	{
	// For property parameters (should they be necessary) for this exported property
	CArrayPtr<CParserParam>* arrayOfParams = new(ELeave) CArrayPtrFlat<CParserParam>(5);
	CleanupStack::PushL(TCleanupItem(CleanUpResetAndDestroy,arrayOfParams));

	// For the property value itself
	CDesCArrayFlat* desArray=new (ELeave)CDesCArrayFlat(4);
	CleanupStack::PushL(desArray);
   
	// Export each item in turn (where the field of the specified type is found 
	// in the contact card).
	TInt foundCount = 0;

	foundCount += LocateSpecificNameValueL(aFields, KUidContactFieldFamilyName,		*desArray, *arrayOfParams, KContactVCardLABEL0, aOption, aExportPrivateFields);
	foundCount += LocateSpecificNameValueL(aFields, KUidContactFieldGivenName,		*desArray, *arrayOfParams, KContactVCardLABEL1, aOption, aExportPrivateFields);
	foundCount += LocateSpecificNameValueL(aFields, KUidContactFieldAdditionalName,	*desArray, *arrayOfParams, KContactVCardLABEL2, aOption, aExportPrivateFields);
	foundCount += LocateSpecificNameValueL(aFields, KUidContactFieldPrefixName,		*desArray, *arrayOfParams, KContactVCardLABEL3, aOption, aExportPrivateFields);
	foundCount += LocateSpecificNameValueL(aFields, KUidContactFieldSuffixName,		*desArray, *arrayOfParams, KContactVCardLABEL4, aOption, aExportPrivateFields);
	
	//The N property must be exported even if empty for vCard 2.1 and 3.0 
	//as per PBAP specification.
	if (foundCount == 0 && IsPBAPExport())
		{
		//replace with single empty name value so that property is N: instead of N:;;;;
		desArray->Reset();
		desArray->AppendL(KNullDesC);
		foundCount = 1; //forces the property to be created
		}
	
	if (foundCount > 0)
		{
		CParserPropertyValue* value = new(ELeave) CParserPropertyValueCDesCArray(desArray);
	  	CleanupStack::Pop(desArray);
		//
	  	CleanupStack::PushL(value);
	  	CParserGroupedProperty* property = CParserGroupedProperty::NewL(*value,KVersitTokenN, NULL, arrayOfParams);

	  	CleanupStack::Pop(2, arrayOfParams); // value, arrayOfParams
		// This pushes property before anything can leave...
		iVCard->AddPropertyL(property);
		}
	else
		{
  		CleanupStack::PopAndDestroy(2, arrayOfParams); // desArray, arrayOfParams
  		}
	}


/** 
 * Export address fields. This method exports the 7 address fields in the contact.
 * 
 * @param aContact Contact item to export
 * @param aVCardMapping UID specifying whether this is a HOME, WORK or PREF address
 * @param aToken The name of the vCard property parameter (eg. HOME, WORK, PREF)
 * @param aOption Export preferences (options available defined in CContactDatabase::TOptions)
 * @param aExportPrivateFields Specify whether private fields are included
 */ 
void CContactsAppToVCardConverter::GetVCardAddressL(const CContactItem& aContact, TUid aVCardMapping, const TDesC8& aToken, TInt aOption, TBool aExportPrivateFields)
	{
    CArrayPtr<CParserParam>* arrayOfParams = new(ELeave) CArrayPtrFlat<CParserParam>(5);
	CleanupStack::PushL(TCleanupItem(CleanUpResetAndDestroy, arrayOfParams));

	if	(aToken.Length())
		{	
		CParserParam* parserParam;
		//nameless parameters not allowed for vCard 3.0, so add "TYPE" parameter name 
		if (GetExportVersion() == EPBAPVCard30)
			{
			parserParam = CParserParam::NewL(KVersitTokenTYPE, aToken);
			}
		else
			{
			parserParam = CParserParam::NewL(aToken, KNullDesC);	
			}
		CleanupStack::PushL(parserParam);
		arrayOfParams->AppendL(parserParam);
		CleanupStack::Pop(parserParam);
		}

    const CContactItemFieldSet& fields = aContact.CardFields();
	CDesCArrayFlat* desArray = new(ELeave) CDesCArrayFlat(4);
	CleanupStack::PushL(desArray);

	TBool found = EFalse;
	RetrieveAddressFieldL(fields, *arrayOfParams, *desArray, aVCardMapping, KUidContactFieldVCardMapPOSTOFFICE, KContactVCardLABEL0, aOption, found, aExportPrivateFields,EFalse);
	RetrieveAddressFieldL(fields, *arrayOfParams, *desArray, aVCardMapping, KUidContactFieldVCardMapEXTENDEDADR, KContactVCardLABEL1, aOption, found, aExportPrivateFields,EFalse);
	RetrieveAddressFieldL(fields, *arrayOfParams, *desArray, aVCardMapping, KUidContactFieldVCardMapADR, KContactVCardLABEL2, aOption, found, aExportPrivateFields,EFalse);
	RetrieveAddressFieldL(fields, *arrayOfParams, *desArray, aVCardMapping, KUidContactFieldVCardMapLOCALITY, KContactVCardLABEL3, aOption, found, aExportPrivateFields,EFalse);
	RetrieveAddressFieldL(fields, *arrayOfParams, *desArray, aVCardMapping, KUidContactFieldVCardMapREGION, KContactVCardLABEL4, aOption, found, aExportPrivateFields,EFalse);
	RetrieveAddressFieldL(fields, *arrayOfParams, *desArray, aVCardMapping, KUidContactFieldVCardMapPOSTCODE, KContactVCardLABEL5, aOption, found, aExportPrivateFields,EFalse);
	RetrieveAddressFieldL(fields, *arrayOfParams, *desArray, aVCardMapping, KUidContactFieldVCardMapCOUNTRY, KContactVCardLABEL6, aOption, found, aExportPrivateFields,EFalse);

	if (found)
		{
	    CParserPropertyValue* value = new (ELeave) CParserPropertyValueCDesCArray(desArray);
	  	CleanupStack::Pop(desArray); 
	  	CleanupStack::PushL(value); 
		CParserGroupedProperty* property = CParserGroupedProperty::NewL(*value, KVersitTokenADR, NULL, arrayOfParams);
	  	CleanupStack::Pop(2); //value, arrayOfParams
		iVCard->AddPropertyL(property);
		}
	else
  		CleanupStack::PopAndDestroy(2); // desArray,arrayOfParams
	}


/** 
 * Convert CContactItemField into a VCard property and add it to iVCard 	
 *
 * @param aField Contact field
 * @param aAgent Agent item
 * @param aOption Export preferences (options available defined in CContactDatabase::TOptions)
 * @param aDuplicateCount Number of fields of this field type in the contact item
 * @param aExportPrivateFields Specify whether private fields are included
 */
void CContactsAppToVCardConverter::MapContactFieldToVCardPropertyL(const CContactItemField& aField,CContactItem* aAgent,TInt aOption,TInt aDuplicateCount,TBool aExportPrivateFields)
	{
	if (aField.IsPrivate() && !aExportPrivateFields)
		{
		return; // If aExportPrivateFields==EFalse, do not export private fields	
		}
		
	TUid mapping=aField.ContentType().Mapping();
	
	//check if the mapping is contained in the filter
	if(IsPBAPExport() && !NeedsToBeExported(AttributeFromMapping(mapping))) 
		{
			return;		
		}
	
	if ((mapping==KUidContactFieldVCardMapUnusedN) ||  
			(mapping==KUidContactFieldVCardMapADR) || (mapping==KUidContactFieldVCardMapPOSTOFFICE) ||
			(mapping==KUidContactFieldVCardMapEXTENDEDADR) ||   (mapping==KUidContactFieldVCardMapLOCALITY)  ||
			(mapping==KUidContactFieldVCardMapREGION) || (mapping==KUidContactFieldVCardMapPOSTCODE)  ||
			(mapping==KUidContactFieldVCardMapCOUNTRY )||  // Above are dealt with in GetVCardNameL() or GetVCardAddressL()
			(mapping == KUidContactFieldVCardMapUnknownXDash) || (mapping == KUidContactFieldVCardMapUnknown) || // unused
			(mapping == KUidContactFieldVCardMapORG) || (mapping == KUidContactFieldVCardMapDepartment)|| // These are dealt with in GetVcardOrgInfoL() 
			(mapping == KUidContactFieldVCardMapUnusedFN))// dealt with in GetVCardFormattedNameL
		return; 

	TBuf8<KContactMaxVCardPropertyNameLength> name;
	TUid propertyUid=KNullUid;
	CArrayPtr<CParserParam>* arrayOfParams=new(ELeave)CArrayPtrFlat<CParserParam>(5);
	CleanupStack::PushL(TCleanupItem(CleanUpResetAndDestroy,arrayOfParams));

	MapContactMappingToVCardPropertyNameAndTypeL(aField.ContentType(),name,propertyUid,arrayOfParams);
	CParserPropertyValue* value=NULL;
	TStorageType storage=aField.StorageType();
	switch(propertyUid.iUid)
		{
		case KVersitPropertyCDesCArrayUid:
		case KVCardPropertySoundUid:
			if (storage==KStorageTypeText)
				{
				CDesCArrayFlat* desArray=new (ELeave)CDesCArrayFlat(4);
				CleanupStack::PushL(desArray);
				const TPtrC KArrayField(aField.TextStorage()->StandardTextLC());
				switch (mapping.iUid)
					{
					case KIntContactFieldVCardMapORGPronunciation:
						{
						RemoveNewLineL(*desArray, KArrayField);
						}
						break;	// KIntContactFieldVCardMapORGPronunciation
						
					case KIntContactFieldVCardMapADR:
						desArray->AppendL(KNullDesC);			//first sub field is empty
						desArray->AppendL(KNullDesC);			//second sub field is empty
						RemoveNewLineL(*desArray, KArrayField);
						desArray->AppendL(KNullDesC);			
						desArray->AppendL(KNullDesC);			
						desArray->AppendL(KNullDesC);			
						break;	// 
						
					default:
						RemoveNewLineL(*desArray, KArrayField);
						break;
					}
				value=new (ELeave) CParserPropertyValueCDesCArray(desArray);
				CleanupStack::PopAndDestroy();
				CleanupStack::Pop();	// desArray
				}
    			/*else if (storage==KStorageTypeStore)
				{}	*/	// zzz does nothing    		(aField.StoreStorage()->Thing()); //value=??
    			break;	//KVersitPropertyCDesCArrayUid
    			
		case KVersitPropertyDateUid:
			if (storage==KStorageTypeDateTime)
				{
				//After fixing defect DEF035264, the local time is stored instead of UTC.
				//Continue to store time as local time, as the only date field that is stored
				//is birthday.
				value = CreateDatePropertyL(aField.DateTimeStorage()->Time());    	
				}
			else if (storage==KStorageTypeText)
				{
				const TPtrC pText(aField.TextStorage()->StandardTextLC());
				
				if (aOption & CContactVCardConverter::EConnectWhitespace)
					{
					if (CContactVCardConverter::ContainsExportableData(pText))
						{
						value = CParserPropertyValueHBufC::NewL(pText);
						}
					}
				else
					{
					value = CParserPropertyValueHBufC::NewL(pText);
					}

				CleanupStack::PopAndDestroy(); // StandardTextLC
				}

			break;	//KVersitPropertyDateUid
		case KVCardPropertyAgentUid:
			if ((storage == KStorageTypeContactItemId) && aAgent)
				{
				CParserVCard * agentcard=NULL;
				CContactsAppToVCardConverter* converter = new(ELeave)CContactsAppToVCardConverter(iMachineUniqueId, iCharSet, GetExportVersion()); 
				CleanupStack::PushL(converter);
				agentcard=converter->GetContactItemAsVCardL(aAgent,NULL,aOption,aExportPrivateFields);
				CleanupStack::PushL(agentcard);
				value=new(ELeave) CParserPropertyValueAgent(agentcard);
				CleanupStack::Pop();	// agentcard
				CleanupStack::PopAndDestroy(1); 
				}
			break;	// KVCardPropertyAgentUid
			
		case KVersitPropertyHBufCUid:
			if (storage==KStorageTypeText)
				{
	 			const TPtrC pText(aField.TextStorage()->StandardTextLC());
				if (aOption & CContactVCardConverter::EConnectWhitespace)
					{
					if (CContactVCardConverter::ContainsExportableData(pText))
						{
						value = CParserPropertyValueHBufC::NewL(pText);
						}	
					}
				else
					{
					value = CParserPropertyValueHBufC::NewL(pText);
					}
				CleanupStack::PopAndDestroy(); //arrayOfParams
				}
			else if (storage==KStorageTypeStore)
				{
	    			value=CParserPropertyValueBinary::NewL(*(aField.StoreStorage()->Thing()));
				}
		    	break; 		//KVersitPropertyHBufCUid
		case KVersitPropertyBinaryUid:
			if (storage==KStorageTypeStore)
				{
				value=CParserPropertyValueBinary::NewL(*(aField.StoreStorage()->Thing()));
				}	
			break;		// KVersitPropertyBinaryUid
			
		default:
    			break;
    		}
    		
	if (value)
		{
		CleanupStack::PushL(value);
  		if ((aOption & CContactVCardConverter::EIncludeX) && aField.OverRidesLabel())
  			{
			TBuf8<KContactMaxVCardPropertyNameLength> param;
			TPtrC label=aField.Label();
			if (label.Size())
				{
				param=KContactVCardXDashEPOCCNTMODEL;
				param.Append(KContactVCardFIELDLABEL);

				CParserParam* paramValue = CParserParam::NewL(param, label);
				CleanupStack::PushL(paramValue);
				arrayOfParams->AppendL(paramValue);
				CleanupStack::Pop(paramValue);
				}
			if (aField.IsHidden())
				{
				param=KContactVCardXDashEPOCCNTMODEL;
				param.Append(KContactVCardFIELDHIDDEN);
				CParserParam* paramValue = CParserParam::NewL(param, KNullDesC8);
				CleanupStack::PushL(paramValue);
				arrayOfParams->AppendL(paramValue);
				CleanupStack::Pop(paramValue);
				}
			if (aField.IsReadOnly())
				{
				param=KContactVCardXDashEPOCCNTMODEL;
				param.Append(KContactVCardFIELDREADONLY);
				CParserParam* paramValue = CParserParam::NewL(param, KNullDesC8);
				CleanupStack::PushL(paramValue);
				arrayOfParams->AppendL(paramValue);
				CleanupStack::Pop(paramValue);
				}
			}
		if ((aOption & CContactVCardConverter::ETTFormat) && aDuplicateCount>0)
			{
			//check if multiple field case
			// export id for multiple fields.
			TBuf8<4> valuebuffer;
  			valuebuffer.Num(aDuplicateCount);
			CParserParam* paramValue = CParserParam::NewL(valuebuffer, KNullDesC8);
			CleanupStack::PushL(paramValue);
			arrayOfParams->AppendL(paramValue);
			CleanupStack::Pop(paramValue);
    		}
		CParserGroupedProperty *property=CParserGroupedProperty::NewL(*value,name,NULL,arrayOfParams);
		CleanupStack::Pop(2); //arrayOfParams,value
		iVCard->AddPropertyL(property);
		}
	else
		{	
		CleanupStack::PopAndDestroy(); //arrayOfParams
		}
    }
  
  	
/**	
 * Map contact field's content type to VCard Property name and a Property Uid.
 * This method should be extended if new field types are added to Contacts 
 * model.
 * 
 * @param aContactType Field content type 
 * @param aName On return this contains the vCard property name
 * @param aPropertyUid On return this contains the vCard property UID
 * @param aArrayOfParams On return this array contains the property parameters for the field
 */
void CContactsAppToVCardConverter::MapContactMappingToVCardPropertyNameAndTypeL(const CContentType& aContactType,TDes8& aName,TUid& aPropertyUid,CArrayPtr<CParserParam>* aArrayOfParams) const
    {
    switch (aContactType.Mapping().iUid)
    	{
    case KIntContactFieldVCardMapADR:
    	aName=KVersitTokenADR;
    	break;
    case KIntContactFieldVCardMapAGENT:
    	aName=KVersitTokenAGENT;
    	break;
    case KIntContactFieldVCardMapBDAY:
        aName=KVersitTokenBDAY;
    	break;	
    case KIntContactFieldVCardMapEMAILINTERNET:
    	aName=KVersitTokenEMAIL;
    	break;
    case KIntContactFieldVCardMapGEO:
    	aName=KVersitTokenGEO;
    	break;
    case KIntContactFieldVCardMapLABEL:
    	aName=KVersitTokenLABEL;
    	break;
    case KIntContactFieldVCardMapLOGO:
    	aName=KVersitTokenLOGO;
		break;
    case KIntContactFieldVCardMapMAILER:
    	aName=KVersitTokenMAILER;
    	break;
    case KIntContactFieldVCardMapNOTE:
    	aName=KVersitTokenNOTE;			
    	break;
    case KIntContactFieldVCardMapPHOTO:
    	aName=KVersitTokenPHOTO;
    	break;
    case KIntContactFieldVCardMapROLE:
    	aName=KVersitTokenROLE;
    	break;
    case KIntContactFieldVCardMapORGPronunciation:
    	{
    	CParserParam* param = CParserParam::NewL(KVersitParam8CompanyPrn, KNullDesC8);
	CleanupStack::PushL(param);
	aArrayOfParams->AppendL(param);	
	CleanupStack::Pop(param);	
	} // no break here. It's intentional
    case KIntContactFieldVCardMapSOUND:
    	aName=KVersitTokenSOUND;
    	break;	
    case KIntContactFieldVCardMapTITLE:
    	aName=KVersitTokenTITLE;
    	break;
    case KIntContactFieldVCardMapURL:
    	aName=KVersitTokenURL;
    	break;
    case KIntContactFieldVCardMapUnusedN:
    	aName=KVersitTokenN;
    	break;
    case KIntContactFieldVCardMapUnusedFN:
    	aName=KVersitTokenFN;
    	break;
	case KIntContactFieldVCardMapTEL:
		aName=KVersitTokenTEL;
    	/*embedded objects and rich text markup could be stored using X-?*/
		break;
	case KIntContactFieldVCardMapKEY:
		aName=KVersitTokenKEY;
    	break;
	case KIntContactFieldVCardMapSECONDNAME:
		aName=KVersitTokenSECONDNAME;
		break;
   	// vCard extension properties (like X-SIPID, X-WV-ID) are not recognised
   	// as tokens by Versit. So, we need to assign the property UID.
    case KIntContactFieldVCardMapSIPID:
    	{
    	aName=KVersitTokenSIPID;
    	aPropertyUid.iUid=KVersitPropertyHBufCUid;
    	}
    	break;
    case KIntContactFieldVCardMapWV:
    	{
    	aName=KVersitTokenWVID;
    	aPropertyUid.iUid=KVersitPropertyHBufCUid;
    	}
    	break;
	case KIntContactFieldVCardMapAssistant:
		{
		aName = KVersitTokenAssistant;
		aPropertyUid.iUid = KVersitPropertyHBufCUid;
		}
		break;
	case KIntContactFieldVCardMapAssistantTel:
		{
		aName = KVersitTokenAssistantTel;
		aPropertyUid.iUid = KVersitPropertyHBufCUid;
		}
		break;
	case KIntContactFieldVCardMapAnniversary:
		{
		aName = KVersitTokenAnniversary;
		aPropertyUid.iUid = KVersitPropertyDateUid;
		}
		break;
	case KIntContactFieldVCardMapSpouse:
		{
		aName = KVersitTokenSpouse;
		aPropertyUid.iUid = KVersitPropertyHBufCUid;
		}
		break;
	case KIntContactFieldVCardMapChildren:
		{
		aName = KVersitTokenChildren;
		aPropertyUid.iUid = KVersitPropertyCDesCArrayUid;
		}
		break;
	case KIntContactFieldVCardMapClass:
		{
		aName = KVersitTokenClass;
		aPropertyUid.iUid = KVersitPropertyHBufCUid;
		}
		break;
    default: 
		break;
    	};

	// Check vCard mappings for contact field
	TInt ii;
	const TInt count=aContactType.FieldTypeCount();
	for (ii=0;ii<count;ii++)
		{
		TPtrC8 name;
		TPtrC8 value;
		switch (aContactType.FieldType(ii).iUid)
			{
		case KIntContactFieldVCardMapWORK:
			name.Set(KVersitParam8Work);
			break;
		case KIntContactFieldVCardMapHOME:
			name.Set(KVersitParam8Home);		
			break;
		case KIntContactFieldVCardMapMSG:
			name.Set(KVersitParam8Msg);		
			break;
		case KIntContactFieldVCardMapVOICE:
			name.Set(KVersitParam8Voice);		
			break;
		case KIntContactFieldVCardMapFAX:
			name.Set(KVersitParam8Fax);		
			break;
		case KIntContactFieldVCardMapPREF:
			name.Set(KVersitParam8Pref);		
			break;
		case KIntContactFieldVCardMapCELL:
			name.Set(KVersitParam8Cell);		
			break;
		case KIntContactFieldVCardMapPAGER:
			name.Set(KVersitParam8Pager);		
			break;
		case KIntContactFieldVCardMapBBS:
			name.Set(KVersitParam8Bbs);		
			break;
		case KIntContactFieldVCardMapMODEM:
			name.Set(KVersitParam8Modem);		
			break;
		case KIntContactFieldVCardMapCAR:
			name.Set(KVersitParam8Car);		
			break;
		case KIntContactFieldVCardMapISDN:
			name.Set(KVersitParam8Isdn);		
			break;
		case KIntContactFieldVCardMapVIDEO:
			name.Set(KVersitParam8Video);		
			break;
		case KIntContactFieldVCardMapDOM:
			name.Set(KVersitParam8Dom);		
			break;
		case KUidContactFieldEMailValue:
		    if (DefaultInternetParameterRequired(aContactType))
		    	{//only create a default parameter of internet if one is required
		    	 //else the other parameters in the list will be added instead
		    	name.Set(KVersitTokenINTERNET);
		    	} 
			break;
		// need to check all the graphics types
		case KIntContactFieldVCardMapGIF:
			value.Set(KVersitParam8Gif);
			name.Set(KVersitTokenTYPE);
			break;
		case KIntContactFieldVCardMapCGM:
			value.Set(KVersitParam8Cgm);
			name.Set(KVersitTokenTYPE);
			break;
		case KIntContactFieldVCardMapWMF:
			value.Set(KVersitParam8Wmf);
			name.Set(KVersitTokenTYPE);
			break;
		case KIntContactFieldVCardMapBMP:
			value.Set(KVersitParam8Bmp);
			name.Set(KVersitTokenTYPE);
			break;
		case KIntContactFieldVCardMapMET:
			value.Set(KVersitParam8Met);
			name.Set(KVersitTokenTYPE);
			break;
		case KIntContactFieldVCardMapPMB:
			value.Set(KVersitParam8Pmb);
			name.Set(KVersitTokenTYPE);
			break;
		case KIntContactFieldVCardMapDIB:
			value.Set(KVersitParam8Dib);
			name.Set(KVersitTokenTYPE);
			break;
		case KIntContactFieldVCardMapPICT:
			value.Set(KVersitParam8Pict);
			name.Set(KVersitTokenTYPE);
			break;
		case KIntContactFieldVCardMapTIFF:
			value.Set(KVersitParam8Tiff);
			name.Set(KVersitTokenTYPE);
			break;
		case KIntContactFieldVCardMapPDF:
			value.Set(KVersitParam8Pdf);
			name.Set(KVersitTokenTYPE);
			break;
		case KIntContactFieldVCardMapPS:
			value.Set(KVersitParam8Ps);
			name.Set(KVersitTokenTYPE);
			break;
		case KIntContactFieldVCardMapJPEG:
			value.Set(KVersitParam8Jpeg);
			name.Set(KVersitTokenTYPE);
			break;
		case KIntContactFieldVCardMapMPEG:
			value.Set(KVersitParam8Mpeg);
			name.Set(KVersitTokenTYPE);
			break;
		case KIntContactFieldVCardMapMPEG2:
			value.Set(KVersitParam8Mpeg2);
			name.Set(KVersitTokenTYPE);
			break;
		case KIntContactFieldVCardMapAVI:
			value.Set(KVersitParam8Avi);
			name.Set(KVersitTokenTYPE);
			break;
		case KIntContactFieldVCardMapQTIME:
			value.Set(KVersitParam8Qtime);
			name.Set(KVersitTokenTYPE);
			break;
		case KIntContactFieldVCardMapX509:
			name.Set(KVersitParam8X509);
			break;
		case KIntContactFieldVCardMapPGP:
			name.Set(KVersitParam8PGP);
			break;
		case KIntContactFieldVCardMapPOC:
			name.Set(KVersitParamPOC);
			break;
		case KIntContactFieldVCardMapSWIS:
			name.Set(KVersitParamSWIS);
			break;
		case KIntContactFieldVCardMapVOIP:
			name.Set(KVersitParamVOIP);
			break;
		case KIntContactFieldVCardMapINTERNET:
		    name.Set(KVersitParam8Internet);
		    break;
			}

		if (name.Length())
			{
			CParserParam* parserParam;
			//nameless parameters not allowed for vCard 3.0, so add "TYPE" parameter name 
			if (GetExportVersion() == EPBAPVCard30 && value.Length() == 0)
				{
				parserParam = CParserParam::NewL(KVersitTokenTYPE, name);
				}
			else
				{
				parserParam = CParserParam::NewL(name, value);
				}
			CleanupStack::PushL(parserParam);
			
			AddParamToArrayL(aArrayOfParams, parserParam, aContactType.Mapping().iUid, aContactType.FieldType(ii).iUid);
				
			CleanupStack::Pop(parserParam);	
			}

   		}

	// For standard vCard properties RecognizeToken is called. 
	// aPropertyUid will already be assigned for vCard extension 
	// properties.
   	if (aPropertyUid==KNullUid)
   		{
		aPropertyUid=iVCard->RecognizeToken(aName);
		}
    }
    
/** 
 * Utility method to create a Versit date time property
 * 
 * @param aDateTime Date and Time
 * @param aRelativeTime Specifies whether the date is in local or UTC time
 * @return Versit date property corresponding to aDateTime
 */
 CParserPropertyValueDateTime* CContactsAppToVCardConverter::CreateDateTimePropertyL(const TTime& aDateTime, TVersitDateTime::TRelativeTime aRelativeTime)
	{
	TVersitDateTime* dateTime= new(ELeave)TVersitDateTime(aDateTime.DateTime(), aRelativeTime);
	CleanupStack::PushL(dateTime);
	CParserPropertyValueDateTime* dateTimeValue=new(ELeave) CParserPropertyValueDateTime(dateTime);
	CleanupStack::Pop(dateTime);	
	return dateTimeValue;
	}


CParserPropertyValueDate* CContactsAppToVCardConverter::CreateDatePropertyL(const TTime& aDateTime)	
/** 
 * Utility method to create a Versit date property.
 * 
 * @param aDateTime Date (and Time) in Local Time. The only property that uses this field is BDAY, which should continue to be stored as MachineLocal time.
 * @return Versit date property corresponding to aDateTime which does not contain a time
 */	
	{
	TVersitDateTime* dateTime= new(ELeave)TVersitDateTime(aDateTime.DateTime(), TVersitDateTime::EIsMachineLocal); 
	CleanupStack::PushL(dateTime);
	CParserPropertyValueDate* dateValue=new(ELeave) CParserPropertyValueDate(dateTime);
	CleanupStack::Pop(dateTime);	
	return dateValue;
	}


 /** 
  * Searches the contacts field set for a particular 'name' field (e.g. searches for the given name,
  * family name, etc) and should the field be located, its value is appended to the specified array.
  * If the options specify that field labels should be exported, then an extension (X-Dash) property
  * parameter is created that corresponds to the specified label, and is added to the property
  * parameter array.
  *
  * If no matching field is found, then KNullDesC is added to the specified array to preserve the
  * vCard 2.1 ordering associated with the N 'name' property.
  *
  * @param aFieldSet The contact card fields
  * @param aSearchField The field being searched for
  * @param aPropValueArray The array to append the field's value to, should it be located.
  * @param aPropParamArray The array to append the label property parameter to, should it be necessary
  * @param aLabelPrefix The label value used to create the property parameter value
  * @param aOption The options required when processing the fields
  * @param aExportPrivateFields Whether or not private fields should be allowed to be exported
  * @return Whether the specified field was located and added to the property value array
  */
TBool CContactsAppToVCardConverter::LocateSpecificNameValueL(const CContactItemFieldSet& aFieldSet, TUid aSearchField, CDesCArray& aPropValueArray, CArrayPtr<CParserParam>& aPropParamArray, const TDesC& aLabelPrefix, TInt aOption, TBool aExportPrivateFields)
	{
	const TBool processWhitespace = (aOption & CContactVCardConverter::EConnectWhitespace);
	TBool foundExportableItem = EFalse;
	//
	TInt pos = aFieldSet.Find(aSearchField);
	//
	while(!foundExportableItem && pos != KErrNotFound)
		{
		const CContactItemField& field = aFieldSet[pos];
		const TBool isPrivate = field.IsPrivate();
		//
		if	(!isPrivate || aExportPrivateFields)
			{
			const TPtrC pFieldContents(field.TextStorage()->StandardTextLC());
			if (processWhitespace)
				{
				foundExportableItem = CContactVCardConverter::ContainsExportableData(pFieldContents);
				}
			else
				{
				foundExportableItem = (pFieldContents.Length() > 0);
				}
			if	(foundExportableItem)
				{
				RemoveNewLineL(aPropValueArray, pFieldContents);
				//
				const TPtrC& label = field.Label();
				if	(label.Length() && (aOption & CContactVCardConverter::EIncludeX) && field.OverRidesLabel())
					{
					TBuf8<KContactMaxVCardPropertyNameLength> param(KContactVCardXDashEPOCCNTMODEL);
					param.Append(aLabelPrefix);
					//
					CParserParam* parserParam = CParserParam::NewL(param, label);
					CleanupStack::PushL(parserParam);
					aPropParamArray.AppendL(parserParam);
					CleanupStack::Pop(parserParam);
					}
				}
			CleanupStack::PopAndDestroy(); // field contents heap descriptor
			}
        
        //find next matched name fields
        pos = aFieldSet.FindNext(aSearchField, pos + 1);			
		}

	// If we didn't find something valid to export, then we write the composite "supported
	// but empty" marker to the array in order to preserve the correct sub-property
	// ordering, i.e. Given Name;First Name;Additional Name;Prefix;Suffix.
	if	(!foundExportableItem)
		{
		if (processWhitespace)
			{
			aPropValueArray.AppendL(KContactVCardCompositeSupportedButEmptyFieldValue);
			}
		else
			{
			aPropValueArray.AppendL(KNullDesC);
			}
		}
	//
	return foundExportableItem;
	}

/** 
 * Extract Organization information like Company Name & Department Name 
 * 
 * @param aFields Contact item field set
 * @param aOption Export preferences (options available defined in CContactDatabase::TOptions)
 * @param aExportPrivateFields Specify whether private fields are included
 */ 
void CContactsAppToVCardConverter::GetVCardOrgInfoL(const CContactItemFieldSet& aFields,TInt aOption,TBool aExportPrivateFields)
	{
	CDesCArrayFlat* desArray = new (ELeave)CDesCArrayFlat(4);
	CleanupStack::PushL(desArray);

	CArrayPtr<CParserParam>* arrayOfParams = new(ELeave) CArrayPtrFlat<CParserParam>(5);
	CleanupStack::PushL(TCleanupItem(CleanUpResetAndDestroy,arrayOfParams));

	TInt foundCount = 0;
	foundCount += LocateSpecificNameValueL(aFields, KUidContactFieldCompanyName, *desArray, *arrayOfParams, KNullDesC, aOption, aExportPrivateFields);
	
	foundCount += LocateSpecificNameValueL(aFields, KUidContactFieldDepartmentName, *desArray, *arrayOfParams, KNullDesC, aOption, aExportPrivateFields);
	
	if(foundCount == 0)
		{
		desArray->AppendL(KNullDesC);
		}
	CleanupStack::PopAndDestroy(arrayOfParams);
	if (foundCount > 0)
		{
		CParserPropertyValue* value = new(ELeave) CParserPropertyValueCDesCArray(desArray);
	  	CleanupStack::Pop(desArray);
	  	CleanupStack::PushL(value);
	  	CParserGroupedProperty* property = CParserGroupedProperty::NewL(*value,KVersitTokenORG, NULL, NULL);
	  	CleanupStack::Pop(value); // value
		iVCard->AddPropertyL(property);
		}
	else
		{
  		CleanupStack::PopAndDestroy(desArray); // desArray
  		}
	}

/** 
  * Removes any NewLine character found in the Contact Field content 
  * and appends the content to the Property Array
  * @param aPropValueArray The array to append the field's value to.
  * @param aFieldContents Contact field content
  */
void CContactsAppToVCardConverter::RemoveNewLineL(CDesCArray& aPropValueArray, const TPtrC& aFieldContents)
	{
	TBool arrayFlag = ETrue;
	HBufC* buf = HBufC::NewLC(256);
	TPtr newCompanyField(buf->Des());
	const TUint length = aFieldContents.Length();
	for(TInt i = 0; i < length; i++)
		{
		if((i < length-1) && (aFieldContents[i] == 0x0D) && (aFieldContents[i+1] == 0x0A))
			{
			i++;
			aPropValueArray.AppendL(newCompanyField);
			newCompanyField.Zero();
			arrayFlag = EFalse;
			continue;
			}
		else
			{
			newCompanyField.Append(aFieldContents[i]);
			arrayFlag = ETrue;
			}
		}
	if(arrayFlag)
		{
		aPropValueArray.AppendL(newCompanyField);		
		}
	CleanupStack::PopAndDestroy(); // buf.
	}


TVCardVersion CContactsAppToVCardConverter::GetExportVersion()const
	{
		return iVCardVersion;
	}

/** 
Create formatted name (FN) property. An empty value maybe exported
@param aFields Contact item fields to be exported
*/ 
void CContactsAppToVCardConverter::GetVCardFormattedNameL(CContactItemFieldSet& aFields, TInt aOption, TBool aExportPrivateFields)
	{
	TBool pos = KErrNotFound;
	const TInt count = aFields.Count();
	for (TInt ii = 0; ii < count; ++ii)
		{
		const CContactItemField& field = aFields[ii];
		if (field.ContentType().Mapping() == KUidContactFieldVCardMapUnusedFN &&
			field.TextStorage() && field.Storage()->IsFull() &&
			(!field.IsPrivate() || aExportPrivateFields))
			{
			//found a valid formatted name, stop searching
			pos = ii;
			break;
			}
		}
	
	CParserPropertyValue* value = NULL;
	if (pos != KErrNotFound)
		{
		const CContactItemField& field = aFields[pos];
	 	const TPtrC pText(field.TextStorage()->StandardTextLC());
	 	if (aOption & CContactVCardConverter::EConnectWhitespace)
			{
			if (CContactVCardConverter::ContainsExportableData(pText))
				{
				value = CParserPropertyValueHBufC::NewL(pText);
				}	
			}
			else
			{
				value = CParserPropertyValueHBufC::NewL(pText);
			}

		CleanupStack::PopAndDestroy(); //field text
		}
	else
		{
		//FN is mandatory for vCard 3.0 so send empty value, otherwise do nothing
		if (GetExportVersion() != EPBAPVCard30)
			{
			return;
			}
		value = CParserPropertyValueHBufC::NewL(KNullDesC);
		}
	CleanupStack::PushL(value);
	CParserGroupedProperty* property = CParserGroupedProperty::NewL(*value, KVersitTokenFN, NULL, NULL);
	CleanupStack::Pop(value);
	//ownership passed
	iVCard->AddPropertyL(property);
	}

void CContactsAppToVCardConverter::SetFilter(TInt64 aFilter)
	{
	iFilter = aFilter;
	}

/**
Convert contacts model vCard mapping uids to PBAP filter attributes
@param TUid aMapping Mapping for a contact field.
*/	
TPBAPProperties CContactsAppToVCardConverter::AttributeFromMapping(TUid aMapping) const
	{
	TPBAPProperties attribute = (TPBAPProperties)0;
    switch (aMapping.iUid)
    	{
		case KIntContactFieldVCardMapADR:
			attribute = EPropertyADR;
			break;
		case KIntContactFieldVCardMapAGENT:
			attribute = EPropertyAGENT;
			break;
		case KIntContactFieldVCardMapBDAY:
			attribute = EPropertyBDAY;
			break;	
		case KIntContactFieldVCardMapEMAILINTERNET:
			attribute = EPropertyEMAIL;
			break;
		case KIntContactFieldVCardMapGEO:
			attribute = EPropertyGEO;
			break;
		case KIntContactFieldVCardMapLABEL:
			attribute = EPropertyLABEL;
			break;
		case KIntContactFieldVCardMapLOGO:
			attribute = EPropertyLOGO;
			break;
		case KIntContactFieldVCardMapMAILER:
			attribute = EPropertyMAILER;
			break;
		case KIntContactFieldVCardMapNOTE:
			attribute = EPropertyNOTE;			
			break;
		case KIntContactFieldVCardMapPHOTO:
			attribute = EPropertyPHOTO;
			break;
		case KIntContactFieldVCardMapROLE:
			attribute = EPropertyROLE;
			break;
		case KIntContactFieldVCardMapORGPronunciation:
		case KIntContactFieldVCardMapSOUND:
			attribute = EPropertySOUND;
			break;
		case KIntContactFieldVCardMapTITLE:
			attribute = EPropertyTITLE;
			break;
		case KIntContactFieldVCardMapURL:
			attribute = EPropertyURL;
			break;
		case KIntContactFieldVCardMapUnusedN:
			attribute = EPropertyN;
			break;
		case KIntContactFieldVCardMapUnusedFN:
			attribute = EPropertyFN;
			break;
		case KIntContactFieldVCardMapTEL:
			attribute = EPropertyTEL;
			break;
		case KIntContactFieldVCardMapKEY:
			attribute = EPropertyKEY;
			break;
		case KIntContactFieldVCardMapClass:
			attribute = EPropertyCLASS;
			break;		
		default:
			break;
    	};
    return attribute;	
	}
	
TBool CContactsAppToVCardConverter::NeedsToBeExported(TPBAPProperties aProp) const
	{
	return iFilter & aProp;
	}

TBool CContactsAppToVCardConverter::IsPBAPExport() const
	{
	return (iVCardVersion == EPBAPVCard21 || iVCardVersion == EPBAPVCard30);
	}

TBool CContactsAppToVCardConverter::DefaultInternetParameterRequired(const CContentType& aContactType) const
	{
	if (aContactType.ContainsFieldType(KUidContactFieldVCardMapINTERNET) || aContactType.ContainsFieldType(KUidContactFieldVCardMapCELL)
		|| 	aContactType.ContainsFieldType(KUidContactFieldVCardMapHOME) || aContactType.ContainsFieldType(KUidContactFieldVCardMapWORK))
		{
		return EFalse; // don't need to do anything as a type fieldtype is present and that parameter will be added.
		}
	return ETrue; // we need to add a default internet parameter
	}

/**
The PREF parameter is inserted as the first one in the array of params to ensure it appears first in the VCard.
The ordering of the parameters is usually preserved, however for TEL there is a specific sequencing.
The vObject minimum interoperability profile from the OMA specifies the following parameter sequence for the TEL property
TEL;["PREF;"]["WORK;"/"HOME;"]"VOICE;"/"FAX;"/"MSG;"/"CELL;"/"PAGER;"/"BBS;"/"MODEM;"/"CAR;"/"ISDN;"/"VIDEO;"
@param aArrayOfParams An array of CParserParam
@param aParserParam const CParserParam*
@param aMappingUid mapping uid used to check if it is the TEL property
@param aFieldTypeUid fieldtype uid used to check for PREF, HOME and WORK field types
*/
void CContactsAppToVCardConverter::AddParamToArrayL(CArrayPtr<CParserParam>* aArrayOfParams, CParserParam* aParserParam, TInt32 aMappingUid, TInt32 aFieldTypeUid) const
	{
	// PREF parameter should be the first parameter in the list, so insert in first position.
	if (aFieldTypeUid == KIntContactFieldVCardMapPREF)
		{
	 	aArrayOfParams->InsertL(0, aParserParam);
	 	return;
		}
	// Sequencing only applies for the TEL property for its HOME and WORK parameters
    // otherwise just preserve ordering.
	if (aMappingUid != KIntContactFieldVCardMapTEL)
		{
		aArrayOfParams->AppendL(aParserParam);
		}
	else
		{
		//This is a TEL property so need to do some sequencing 
		if (aFieldTypeUid == KIntContactFieldVCardMapHOME || aFieldTypeUid == KIntContactFieldVCardMapWORK)
			{
			//contains HOME or WORK  so must insert either in first or second place
			if (aArrayOfParams->Count())
				{
				CParserParam* parserParam = aArrayOfParams->At(0);
				if (parserParam->Name().CompareF(KVersitParam8Pref)==0)
					{
					aArrayOfParams->InsertL(1, aParserParam);
					}
				else if (parserParam->Name().CompareF(KVersitTokenTYPE)==0)//vCard version 3.0 
					{
					if (parserParam->Value().CompareF(KVersitParam8Pref)==0)
						{
						aArrayOfParams->InsertL(1, aParserParam);
						}
					}
				else
					{
					aArrayOfParams->InsertL(0, aParserParam);
					}
				}
			else
				{//No params present so can just append.
				aArrayOfParams->AppendL(aParserParam);
				}
			}
		else
			{// Does not contain HOME or WORK so just append to preserve order of parameters.
			aArrayOfParams->AppendL(aParserParam);
			}
		}
	}
