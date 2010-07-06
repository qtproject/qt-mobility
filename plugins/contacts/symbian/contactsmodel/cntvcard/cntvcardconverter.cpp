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


#include <cntvcard.h>
#include "cntvcardutils.h"

// System includes
#include <badesca.h>
#include <s32std.h>
#include <s32mem.h>
#include <ecom/implementationproxy.h>

// User includes
#include "cntprof.h"
#include <cntfldst.h>
#include <cntfield.h>
#include <cntdef.h>
#include <cntitem.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "cntconvertercallback.h"
#endif



/** 
 * Imports one or more vCards from a read stream. The vCards are converted 
 * into contact items, and added to the database. If at least one contact
 * item was successfully imported, aImportSuccessful is set to ETrue. 
 * 
 * @param aDb Contacts database
 * @param aReadStream The stream to read from.
 * @param aImportSuccessful On return, ETrue if at least one contact was successfully imported. EFalse if not
 * @param aOption Import preferences (available options defined in CContactDatabase::TOptions)
 * @param aImportSingleContact Import a single vCard entity
 * @return Array of CContactItem objects
 */
CArrayPtr<CContactItem>* CContactVCardConverter::ImportL(CContactDatabase& aDb,RReadStream& aReadStream,TBool& aImportSuccessful,TInt aOption,TBool aImportSingleContact)
	{
__START_PROFILE(23);
	TBool increaseAccessCount=aOption & (EIncreaseAccessCount);
	TBool decreaseAccessCount=aOption & (EDecreaseAccessCount);
	aImportSuccessful=EFalse;	
	CVCardToContactsAppConverter* converter=new(ELeave)	CVCardToContactsAppConverter;
	CleanupStack::PushL(converter);
	CArrayPtr<CContactItem>* contactItems=new(ELeave) CArrayPtrFlat<CContactItem>(4);
	CleanupStack::PushL(TCleanupItem(CleanUpResetAndDestroy,contactItems));
	TInt err=KErrNone;
	if (!aImportSingleContact)
		{
		aDb.DatabaseBeginLC(EFalse);
		}

	while (err!=KErrEof)
		{
		CParserVCard* vCard= CParserVCard::NewL();
		CleanupStack::PushL(vCard);
__START_PROFILE(20);
		TRAP(err,vCard->InternalizeL(aReadStream));
__END_PROFILE(20);
		if ((err != KErrNotFound) && (err != KErrEof))
	  		{
			User::LeaveIfError(err);
				
				TContactItemId pos = IsVCardMergeNeededL(*converter, *vCard, aDb, aOption);
				//Check if we have been asked to replaced rather than merge the
				//contact item.		
				TContactItemId posId = KErrNotFound;
				if ((aOption & EReplaceIfExists) && (pos != KErrNotFound))
					{
					posId = pos;
					pos = KErrNotFound;						
					}
					
				if (pos == KErrNotFound)
					{
					//this is a vCard previously exported but now deleted from the database
					//treat as a new card
					//or an existing contact item which is to be
					//updated (replaced) with the given vCard.
					doImportL(*converter, *vCard, aDb, aOption, increaseAccessCount, decreaseAccessCount, aImportSuccessful, contactItems, !aImportSingleContact, posId);
					}
				else
					{
					//this is a existing contact to update
					CContactItem* item=aDb.OpenContactLX(pos);
					CleanupStack::PushL(item);
					
					ModifyAccessCountL(*item, increaseAccessCount, decreaseAccessCount);

					TUid type = item->Type();
					if (type != KUidContactCard && type != KUidContactOwnCard)
						{
						User::Leave(KErrNotFound);
						}

					CArrayPtr<CParserProperty>* agentPropertyList = NULL;
					
					agentPropertyList = vCard->PropertyL(KVersitTokenAGENT, TUid::Uid(KVCardPropertyAgentUid));
					
					CleanupStack::PushL(TCleanupItem(CleanUpResetAndDestroy, agentPropertyList));
					if(agentPropertyList && agentPropertyList->Count())
						{
						//Add agents to contact item and if needed, merge agentcards with existing contacts
						HandleAgentsInVCardL(*converter, agentPropertyList, *item, aDb, aOption, increaseAccessCount, decreaseAccessCount, contactItems,ETrue);
						}		
					CleanupStack::PopAndDestroy(agentPropertyList);

					TBool deleteItem = converter->MergeVCardWithContactItemL(*item, *vCard, CVCardToContactsAppConverter::EPreserveAllProperties, aOption);
		
					aDb.doCommitContactL(*item, ETrue, ETrue);  
					
					if (deleteItem)
						{
						CleanupStack::PopAndDestroy(2); // item, item close
						aDb.doDeleteContactL(pos, ETrue, ETrue, decreaseAccessCount);
						}
					else
						{
						contactItems->AppendL(item);
						CleanupStack::Pop(); // item
						CleanupStack::PopAndDestroy(); // item close
						}
						
					aImportSuccessful = ETrue;
 				}
			}	
		CleanupStack::PopAndDestroy();	// vCard
		if (aImportSingleContact)
			{
			break;
			}
		}

	if (!aImportSingleContact)
		{
		aDb.DatabaseCommitLP(EFalse);
		}

	CleanupStack::Pop();	// contactItems
	CleanupStack::PopAndDestroy(); //converter
__END_PROFILE(23);
	return contactItems;
	}


/** 
 * Import a vCard object
 * 
 * @param aConverter Import converter
 * @param aVCard vCard object to import
 * @param aDb Contacts database reference
 * @param aOption Import preferences (available options defined in CContactDatabase::TOptions)
 * @param aIncAccessCount Increment the access count of contact items imported
 * @param aDecAccessCount Decrement the access count of contact items imported
 * @param aImportSuccessful vCard object was imported successfully
 * @param aContactItems Array of imported contact items
 * @param aIdForUpdate  contact ID which is to be updated / replaced with the imported vCrad.
 */
void CContactVCardConverter::doImportL(CVCardToContactsAppConverter& aConverter, CParserVCard& aVCard, CContactDatabase& aDb, TInt aOption, TBool aIncAccessCount, TBool aDecAccessCount, TBool& aImportSuccessful, CArrayPtr<CContactItem>* aContactItems, TBool aIsInTransaction, TContactItemId aIdForUpdate)
	{
	CArrayPtr<CParserProperty>* agentPropertyList = NULL;
	
	agentPropertyList = aVCard.PropertyL(KVersitTokenAGENT, TUid::Uid(KVCardPropertyAgentUid));
	CleanupStack::PushL(TCleanupItem(CleanUpResetAndDestroy, agentPropertyList));

	CContactItem* mainItem = aConverter.GetVCardAsContactItemLC(aVCard, CVCardToContactsAppConverter::EPreserveAllProperties, aOption);

	if (agentPropertyList && agentPropertyList->Count())
		{
		//Add agents to contact item and if needed, merge agentcards with existing contacts
		HandleAgentsInVCardL(aConverter, agentPropertyList, *mainItem, aDb, aOption, aIncAccessCount, aDecAccessCount, aContactItems, EFalse);
		}
	
	if	(mainItem->CardFields().Count()) // checks vcard is not empty
		{
		ModifyAccessCountL(*mainItem, aIncAccessCount, aDecAccessCount);
		if	(aOption & ENullTemplateId)
			{
			mainItem->SetTemplateRefId(KNullContactId);
			}
		else
			{
			mainItem->SetTemplateRefId(KGoldenTemplateId);
			}

		//Check if we have been asked to replace (update) rather than add 
		//the contact item.
		if ((aOption & EReplaceIfExists) && (aIdForUpdate != KErrNotFound))
			{
			CContactItem* oldItem;
			oldItem = aDb.OpenContactL(aIdForUpdate);
			CleanupStack::PushL(oldItem);								
			CContactItemFieldSet& fieldSet = oldItem->CardFields();			
			fieldSet.Reset();			
			aDb.CommitContactL(*oldItem);
			CleanupStack::PopAndDestroy(oldItem);
			CContactItem* updateItem = NULL;
			updateItem = aDb.UpdateContactLC(aIdForUpdate, mainItem);
			aContactItems->AppendL(updateItem); 
			CleanupStack::Pop(updateItem);
			CleanupStack::PopAndDestroy(mainItem);		
			}
		else
			{			
			aDb.doAddNewContactL(*mainItem, EFalse, aIsInTransaction);
			aContactItems->AppendL(mainItem);
			CleanupStack::Pop(mainItem);
			}
		aImportSuccessful = ETrue;
		}
	else
		{
		// Just cleanup
		CleanupStack::PopAndDestroy(mainItem);
		}
	CleanupStack::PopAndDestroy(agentPropertyList);
	}


/** 
 * Export a contact as vCard.
 * 
 * @param aDb Contact database
 * @param aSelectedContactIds Array of contact items IDs to export
 * @param aWriteStream Stream to externalize vCard data to 
 * @param aOption Export preferences (available options defined in CContactDatabase::TOptions)
 * @param aCharSet Default character set to pass to Versit parser component
 * @param aExportPrivateFields Specify whether private fields are included
 * @param aCommitNumber Number of contacts to be exported before commit of database
 */
void CContactVCardConverter::ExportL(CContactDatabase& aDb,const CContactIdArray& aSelectedContactIds,RWriteStream& aWriteStream,TInt aOption,const Versit::TVersitCharSet aCharSet, TBool aExportPrivateFields, TInt aCommitNumber=10)
	{
	const TBool increaseAccessCount=aOption & (EIncreaseAccessCount);
	const TBool decreaseAccessCount=aOption & (EDecreaseAccessCount);
	const TBool accessCountChanges=decreaseAccessCount || increaseAccessCount;
	CContactsAppToVCardConverter* converter = new(ELeave)CContactsAppToVCardConverter(aDb.MachineId(), aCharSet, EVCard21);
	CleanupStack::PushL(converter);

	const TInt count=aSelectedContactIds.Count();
	for (TInt ii=0; ii<count; ii++)
		{
		CContactItem* contactItem=NULL;
		TContactItemId id=aSelectedContactIds[ii];
		if (accessCountChanges)
			{
			if (ii % aCommitNumber == 0)	// 1st item of N
				{
				aDb.DatabaseBeginLC(EFalse);	
				}
			contactItem=aDb.OpenContactLX(id);
			CleanupStack::PushL(contactItem);
			}
		else
			{
			contactItem=aDb.ReadContactLC(id,*aDb.AllFieldsView());
			}
		
		CContactItemFieldSet& fields=contactItem->CardFields(); 
		TInt agentPos = KContactFieldSetSearchAll; 

		CContactItem* agentItem = NULL;
		CArrayPtr<CContactItem>* agentItemArray = new(ELeave) CArrayPtrFlat<CContactItem>(4);
		CleanupStack::PushL(TCleanupItem(CleanUpResetAndDestroy, agentItemArray));

		while(1)
			{
			agentPos = fields.FindNext(KUidContactFieldVCardMapAGENT, agentPos);
			TContactItemId agentId;
			
			if (agentPos != KErrNotFound)
				{
		  		agentId = fields[agentPos].AgentStorage()->Value();		
			  	TRAPD(err, agentItem = aDb.ReadContactL(agentId, *aDb.AllFieldsView()));

				if(err != KErrNotFound) 
	 				{
	 				User::LeaveIfError(err);
					agentItemArray->AppendL(agentItem);
				  	}
				++agentPos;
				agentItem = NULL;
				}
				
			else
				{
				break;
				}
		}//while
		CParserVCard* vCard=converter->GetContactItemAsVCardL(contactItem, agentItemArray, aOption, aExportPrivateFields);
	
 		CleanupStack::PushL(vCard);
		vCard->SetDefaultCharSet(aCharSet);
		vCard->ExternalizeL(aWriteStream); 
		CleanupStack::PopAndDestroy(vCard);

		CleanupStack::PopAndDestroy(agentItemArray);
			
		if (accessCountChanges)
			{
			if (decreaseAccessCount)
				{
				contactItem->DecAccessCount();
				}
			if (increaseAccessCount)
				{
				contactItem->IncAccessCount();
				}
			aDb.doCommitContactL(*contactItem,ETrue,EFalse);	// commit every 10 by default
			CleanupStack::PopAndDestroy(); // contactItem
			CleanupStack::PopAndDestroy(); // contact close from OpenContactLX
			// Nth item or last item
			if (((ii + 1) % aCommitNumber == 0) || ii == (count - 1))
				{
				aDb.DatabaseCommitLP(EFalse);
				}
			}
		else	// access count does not change
			{
			CleanupStack::PopAndDestroy(); // contactItem
			}
		}
		
	CleanupStack::PopAndDestroy(converter); 
	}


TBool CContactVCardConverter::ContainsExportableData(const TDesC& aText)
/**  
 * Two different export rules:
 * 
 * RULE 1: Single property values, e.g. TEL, LABEL, FN, NOTE etc etc
 * For this type of property value, whitespace is not exported to the PIM, i.e this property
 * value should not be sent to the PIM for merging.
 * 
 * RULE 2: Multi property values, e.g. ADR, N, ORG etc etc
 * 
 * a) NULL means that the field is not supported by the device. It does NOT mean that the
 * field should be deleted from the PIM. Therefore, when a NULL value is sent by the device to the PC
 * the existing PC-PIM contact field should be preserved.
 * 
 * b) a SINGLE SPACE means that the field is supported by the device, but currently has an empty value (i.e.
 * the field has been deleted or is currently empty).
 * 
 * =====> This implementation only current obeys RULE1.
 * 
 * @param aText The text to be analyzed
 */
	{
	return ContainsData(aText);
	}

TBool CContactVCardConverter::ContainsImportableData(const TDesC& aText, TVersitPropertyType aType, TCntVCardImportType aImportType)
/**
 * Two different import rules:
 * 
 * RULE 1: Single property values, e.g. TEL, LABEL, FN, NOTE etc etc
 * For this type of property value, a NULL value (i.e aText.Length() == 0) means that
 * the specified field should be deleted from the contact card. Any whitespace will be
 * ignored and not imported into the Contacts Database.
 * 
 * RULE 2: Multi property values, e.g. ADR, N, ORG etc etc
 * PC-based PIM's have varying support for the different sub-fields within a multi-property
 * object. Therefore, in a multi-property value object, the following semantics are observed
 * 
 * a) NULL means that the field is not supported by the PIM software. It does NOT mean that the
 * field should be deleted. Therefore, when a NULL value is sent by the PC Sync Engine to the
 * device no changes should be made to the corresponding field in the contact card.
 * 
 * b) a SINGLE SPACE means that the field is supported by the PC PIM, but has an empty value (i.e.
 * the field has been deleted or is currently empty).
 * 
 * A further complication is that during a non-merge sync, its entirely possible for the PC to
 * send vCard data which is effectively empty. One such example is TimeIS data extracted from
 * MS Outlook which then sent to the device. In this case, even if the PIM shows an address
 * as being empty, TimeIS sync drivers still send this empty address, e.g.:
 * 
 * ADR;HOME: ;; ; ; ; ; 
 * 
 * In this instance, we must ensure that we do not import a completely empty property. Therefore
 * the SPACE/NULL syntax is only used explicitly during a MERGE operation. An INITIAL SYNC
 * doesn't need to understand the SPACE/NULL syntax.
 * 
 * =====> This implementation obeys both rules completely.
 * @param aText The text to be analyzed.
 * @param aType Specify if the property is a single or multi-fielded type
 * @param aImportType Specify if this is the first time the card is being imported or if its a merge.
*/
	{
	TBool validDataForImport = EFalse;
 
	const TInt length = aText.Length();
	if	(aType == EPropertyValueComposite)
		{
		if	(aImportType == ECntVCardImportTypeFirstSync)
			{
			if	(!length || (length == 1 && aText[0] == KContactVCardSpaceCharacter))
				{
				// We do not import either
				//
				// - empty (but supported) PIM fields 
				// - empty (non supported) PIM fields
				//
				// during initial sync since these would lead to fields in the contact card
				// which contain spaces.
				validDataForImport = EFalse;
				}
			else
				{
				// If its non-whitespace, we'll allow it for import
				validDataForImport = ContainsData(aText);
				}
			}
		else if (aImportType == ECntVCardImportTypeMerge)
			{
			if	(!length)
				{
				// Don't process (i.e. delete) fields which are not supported by the PC PIM.
				validDataForImport = EFalse;
				}
			else if	(length == 1 && aText[0] == KContactVCardSpaceCharacter)
				{
				// This field is empty in the PIM and therefore we must 'import it' (which in fact,
				// means we must delete the corresponding device-side field).
				validDataForImport = ETrue;
				}
			else 
				{
				// Otherwise check each character to weed out the whitespace
				validDataForImport = ContainsData(aText);
				}
			}
		else
			Panic(ECntVPanicInvalidImportType);
		}
	else if (aType == EPropertyValueSingle)
		{
		// We must import (process) NULL values, since they effectively mean 'DELETE'
		// We do not import fields containing only whitespace
		validDataForImport = (!length || ContainsData(aText));
		}
	return validDataForImport;
	}


TBool CContactVCardConverter::ContainsData(const TDesC& aText)
/**
 * Determine if the text contains any data that can be imported
 * @param aText The text to be analyzed.
*/
	{
	const TInt length = aText.Length();
	TInt whiteSpaceCount = 0;
	TChar character;

	for(TInt i=0; i<length; i++)
		{
		// If the character is whitespace, then check the next character until
		// all characters have been reached. If its not-whitespace, then this field
		// contains data which can be exported.
		character = aText[i];
		if	(character.IsSpace())
			{
			++whiteSpaceCount;
			}
		else
			{
			return ETrue;
			}
		}
	return (whiteSpaceCount < length);
	}

CContactVCardConverter* CContactVCardConverter::NewL()
	{
	return new(ELeave) CContactVCardConverter();
	}
	
// Export the implementation collection function
const TImplementationProxy ImplementationTable[] = 
    {
    IMPLEMENTATION_PROXY_ENTRY(0x102035F9, 	CContactVCardConverter::NewL),
    IMPLEMENTATION_PROXY_ENTRY(0xA00015C1, 	CPBAPContactVCardConverter::NewL)
    };

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

    return ImplementationTable;
    }
    
void CPBAPContactVCardConverter::ExportL(CContactDatabase& aDb, const CContactIdArray& aSelectedContactIds, RWriteStream& aWriteStream, TInt aOption, const Versit::TVersitCharSet aCharSet, TBool aExportPrivateFields, TInt /*aCommitNumber = 10*/)
	{
	CContactsAppToVCardConverter* converter = new(ELeave)CContactsAppToVCardConverter(aDb.MachineId(), aCharSet, GetVersion());
	CleanupStack::PushL(converter);

	const TInt count = aSelectedContactIds.Count();

	converter->SetFilter(PrepareFilterAndOption(aOption));
	
	for (TInt ii = 0; ii < count; ii++)
		{
		CContactItem* contactItem = NULL;
		TContactItemId id = aSelectedContactIds[ii];
		contactItem = aDb.ReadContactLC(id, *aDb.AllFieldsView());
	
		CParserVCard* vCard = converter->GetContactItemAsVCardL(contactItem, NULL, aOption, aExportPrivateFields);
 		CleanupStack::PushL(vCard);
		vCard->SetDefaultCharSet(aCharSet);
		//Intra-Contact Properties start
		MConverterCallBack* clientCallback = GetCallback();
		if(clientCallback)
			{
			CArrayPtr<CParserProperty>* intraPropertyList = NULL;
			intraPropertyList = new(ELeave) CArrayPtrFlat<CParserProperty>(5);
			CleanupStack::PushL(TCleanupItem(CVersitParser::ResetAndDestroyArrayOfProperties, intraPropertyList));
			clientCallback->AddIntraContactPropertiesL(id, intraPropertyList);	
			TInt count = intraPropertyList->Count();
			for(TInt loop = 0; loop < count; ++loop)
				{
				CParserProperty* parserProperty = (*intraPropertyList)[loop];
				(*intraPropertyList)[loop] = NULL;
				//AddpropertyL takes ownership
				vCard->AddPropertyL(parserProperty, ETrue);
				}
			CleanupStack::PopAndDestroy(intraPropertyList);
			}
		//Intra-Contact Properties ends
		vCard->ExternalizeL(aWriteStream);
		CleanupStack::PopAndDestroy(vCard); 
		CleanupStack::PopAndDestroy(); // contactItem
		}
	CleanupStack::PopAndDestroy(converter); 
	}

TInt64 CPBAPContactVCardConverter::PrepareFilterAndOption(TInt& aOption)
	{
	//These fields are not required for PBAP
	if(aOption & CContactVCardConverter::EIncludeX)
		{
		aOption ^= CContactVCardConverter::EIncludeX;
		}
	if(aOption & CContactVCardConverter::ETTFormat) 
		{
		aOption ^= CContactVCardConverter::ETTFormat;
		}
	if(aOption & CContactVCardConverter::EConnectWhitespace)
		{
		aOption ^= CContactVCardConverter::EConnectWhitespace;
		}
		
	//Filter starts
	TInt64 pbapFilter = GetFilter();
	
	//If bit filter is unset (0x0000000), then export all supported fields.
	//0xFFFFFFF sets all the property bit fields specified by PBAP spec.
	if(pbapFilter == 0)
		{
		pbapFilter = EAllProperties;
		}
	//if iExportTel is TRUE then TEL will be exported (even if empty)
	if(IsExportTel())
		{
		pbapFilter |= EPropertyTEL;
		}
	//if iExportTel is FALSE then TEL will not be exported.
	else
		{ 
		if(pbapFilter & EPropertyTEL)
			{
			pbapFilter ^= EPropertyTEL;
			}
		}			
	pbapFilter |= EPropertyN; // Mandatory for both 2.1 and 3.0
	//Export of FN property is mandatory for vCard 3.0 but for vCard 2.1 only if provided in filter.
	if(GetVersion() == EPBAPVCard30) 
		{
		pbapFilter |= EPropertyFN;
		//these properties are not supported for vCard3.0
		if(pbapFilter & EPropertyAGENT)
			{
			pbapFilter ^= EPropertyAGENT;
			}
		if(pbapFilter & EPropertyTZ)
			{
			pbapFilter ^= EPropertyTZ;
			}
		if(pbapFilter & EPropertyGEO)
			{
			pbapFilter ^= EPropertyGEO;
			}
		if(pbapFilter & EPropertySOUND)
			{
			pbapFilter ^= EPropertySOUND;
			}
		if(pbapFilter & EPropertyCLASS)
			{
			pbapFilter ^= EPropertyCLASS;
			}																			
		}
	//If UID is not in filter, it should not be exported,
	if(!(pbapFilter & EPropertyUID))
		{
		aOption |= CContactVCardConverter::EExcludeUid;
		}
	return pbapFilter;
	}

CArrayPtr<CContactItem>* CPBAPContactVCardConverter::ImportL(CContactDatabase& /*aDb*/, RReadStream& /*aReadStream*/, TBool& /*aImportSuccessful*/, TInt /*aOption*/, TBool /*aImportSingleContact*/)
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

CPBAPContactVCardConverter::CPBAPContactVCardConverter(TInt64 aFilter, MConverterCallBack* aCallback, TVCardVersion aVersion, TBool aExportTel):
	iFilter(aFilter),
	iCallback(aCallback),
	iVersion(aVersion),
	iExportTel(aExportTel)
	{
	}

CPBAPContactVCardConverter* CPBAPContactVCardConverter::NewL(TAny* param)
	{
	TPluginParameters* ptr = static_cast<TPluginParameters*>(param);
	CPBAPContactVCardConverter* self = new(ELeave) CPBAPContactVCardConverter(ptr->GetFilter(), ptr->GetCallback(), ptr->GetExportVersion(), ptr->IsExportTel());
	return self;
	}
	
TInt64 CPBAPContactVCardConverter::GetFilter()const
	{
	return iFilter;
	}

MConverterCallBack* CPBAPContactVCardConverter::GetCallback()const
	{
	return iCallback;
	}

TVCardVersion CPBAPContactVCardConverter::GetVersion ()const
	{
	return iVersion;
	}

TBool CPBAPContactVCardConverter::IsExportTel()const
	{
	return iExportTel;
	}

/** 
 * Checks if an incoming vCard should be merged to an existing contact in database or added as a new contact.
 * 
 * @param aConverter Import converter
 * @param aVCard vCard object of contact being imported. 
 * @param aDb Contacts database reference
 * @param aOption Import preferences (available options defined in CContactDatabase::TOptions)
 * @return  If incoming vCard has to be merged then contactItem Id of a contact, otherwise KErrNotFound.
 */
TContactItemId CContactVCardConverter::IsVCardMergeNeededL(CVCardToContactsAppConverter& aConverter, CParserVCard& aVCard, CContactDatabase& aDb, TInt aOption)
	{
	TContactItemId pos = KErrNotFound;
	CContactItem* contact = NULL;
	TBuf<KUidStringLength> uidString;

	//
	// Have we been asked to ignore the UID property value?  If not,
	// we must extract the contact ID from the UID.  Otherwise, assume
	// that client which provided the EIgnoreUid option knows that this
	// contact does not already exist in the database.
	//

	if (!(aOption & (EIgnoreUid)))
		{
		aConverter.GetVCardUidStringL(aVCard, uidString);	    				

		//
		// Is there a UID property value?  If so, check whether or not
		// there is a Contact Item with the associated UID already
		// present in the database.  If there is, the Contact Item will
		// be updated rather than added.
		//
	
		if (uidString.Length())
			{
			//
			// Parse the UID property value to see if the Machine ID
			// field matches this database's Machine ID (i.e. this vCard
			// was originally created in this database and is being
			// updated).  If the Machine IDs match then return the
			// Contact ID field from the UID property value.
			//

			pos = ContactGuid::IsLocalContactUidString(uidString, aDb.MachineId());
			//vCard was created from a contact in current database.
			if (pos != KErrNotFound)
				{
				TRAPD(err, contact = aDb.ReadContactL(pos));
				if (err == KErrNotFound)
					{
					pos = KErrNotFound;	
					}
				else
					{
					User::LeaveIfError(err);					
					if (contact->IsDeleted())
						{
						//we dont want to update this contact as it has already been deleted.
						//so assign it a value other than KErrNotFound, this will avoid execution of "if" condition below
						//as we dont have to make use of ContactIdByGuidL since contact already exists in database.
						pos = KErrGeneral;
						}
					delete contact;
					}
				}
			//No Positon found in UID, so check using Guid of VCard.
			if (pos == KErrNotFound)
				{
				//
				// If the Contact item represented by ID extracted from the UID
				// does not exist, then this vCard was either created elsewhere or the original
				// contact has been deleted from the database.
				// (Contacts Model on another device, PC application, etc).
				// In this case we need to attempt to find the "foreign" UID
				// in the database since it may have been imported
				// previously and if so should be updated.
				//

				pos = aDb.ContactIdByGuidL(uidString);
				if (pos != KNullContactId)
					{
					TRAPD(err, contact = aDb.ReadContactL(pos));
					if (err != KErrNotFound)	
						{
						User::LeaveIfError(err);
						if (contact->IsDeleted())
							{
							pos = KErrNotFound;
							}
						}
					delete contact;
					}
				}						
			}
		}
		
		if (pos == KErrGeneral)
			{
			pos = KErrNotFound;	
			}
	return pos;
	}

/** 
 * Modifies access count of any contact.
 * 
 * @param aContact Contact item whose access count has to be modified.
 * @param aIncAccessCount Increment the access count of contact items imported
 * @param aDecAccessCount Decrement the access count of contact items imported
 */
void CContactVCardConverter::ModifyAccessCountL(CContactItem& aContact, TBool aIncAccessCount, TBool aDecAccessCount)
	{				
	if (aIncAccessCount)
		{
		aContact.IncAccessCount();
		}
	if (aDecAccessCount)
		{
		aContact.DecAccessCount();
		}
	}

/** 
 * Imports one or more agent vCard objects present in the vCard.
 * 
 * @param aConverter Import converter
 * @param aAgentProperties Array of agent properties extracted from parent vCard object.
 * @param aContact Contact item to which the agents will be added.
 * @param aDb Contacts database reference
 * @param aOption Import preferences (available options defined in CContactDatabase::TOptions)
 * @param aIncAccessCount Increment the access count of contact items imported
 * @param aDecAccessCount Decrement the access count of contact items imported
 * @param aImportSuccessful vCard object was imported successfully
 * @param aContactItems Array of imported contact items
 * @param aMerge  aContact is being merged or added as a new contact.
 */
void CContactVCardConverter::HandleAgentsInVCardL(CVCardToContactsAppConverter& aConverter, CArrayPtr<CParserProperty>* aAgentProperties, CContactItem& aContact, CContactDatabase& aDb, TInt aOption, TBool aIncAccessCount, TBool aDecAccessCount, CArrayPtr<CContactItem>* aContactItemArray, TBool aMerge)
	{
	CContactItem* contact = NULL;
	CContactItem* agentItem = NULL;
	
	__ASSERT_DEBUG(aAgentProperties->Count() != 0, User::Leave(KErrArgument));

	RArray <TContactItemId> contactIdArray;
	CleanupClosePushL(contactIdArray);

	TContactItemId pos = KErrNotFound;
	const TInt count = aAgentProperties->Count();
	for(TInt loop = 0;loop < count;++loop)
		{
		CParserVCard* agentcard = static_cast<CParserPropertyValueAgent*>((*aAgentProperties)[loop]->Value())->Value();
		pos = IsVCardMergeNeededL(aConverter, *agentcard, aDb, aOption);
		if(pos != KErrNotFound) //contact similar to Agent exists in database, so merge both.
			{
			contact = aDb.OpenContactL(pos); 
			CleanupStack::PushL(contact);
			TBool deleteItem = aConverter.MergeVCardWithContactItemL(*contact, *agentcard, CVCardToContactsAppConverter::EPreserveAllProperties, aOption);
			ModifyAccessCountL(*contact, aIncAccessCount, aDecAccessCount);
			aDb.CommitContactL(*contact);
			CleanupStack::PopAndDestroy(contact);				
			if(deleteItem)
				{
				aDb.DeleteContactL(pos);	
				}				
			}
		else //Agent present in vCard but should be added as a new contact
			{
			agentItem = aConverter.GetVCardAsContactItemLC(*agentcard, CVCardToContactsAppConverter::EPreserveAllProperties, aOption);
			ModifyAccessCountL(*agentItem, aIncAccessCount, aDecAccessCount);
			agentItem->SetLastModified(aContact.LastModified());
			pos = aDb.doAddNewContactL(*agentItem, EFalse, ETrue);
			aContactItemArray->AppendL(agentItem);
			CleanupStack::Pop(agentItem);
			}	
		contactIdArray.Append(pos);
		}
		
	
	const TInt idCount = contactIdArray.Count();
	if (idCount)
		{
		for(TInt idLoop = 0;idLoop < idCount;++idLoop)	
			{
			// Include agendid in a field in maincontact
			CContactItemField* field = CContactItemField::NewLC(KStorageTypeContactItemId);
			field->SetMapping(KUidContactFieldVCardMapAGENT);
			field->AgentStorage()->SetAgentId(contactIdArray[idLoop]);
			if (aMerge)
				{
				aContact.CardFields().UpdateFieldSyncL(*field, idLoop+1);
				CleanupStack::PopAndDestroy(); // field
				}
			else
				{
				aContact.AddFieldL(*field);				
				CleanupStack::Pop(); // takes ownership of field.			
				}
			}
		}
	CleanupStack::PopAndDestroy(&contactIdArray);
	}
 
    
    
