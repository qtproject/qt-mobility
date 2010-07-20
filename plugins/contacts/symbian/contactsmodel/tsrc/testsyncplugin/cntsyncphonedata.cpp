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


#include "cntsyncchecker.h"
#include "cntsyncphonedata.h"
#include <cntdef.h>
#include <cntdb.h>
#include "cntsyncdata.h"

#include <centralrepository.h>  // crepository.
#include <e32property.h>
#include <phbksync.h>

//---------------------------------------------------
CContactSyncPhonebookData* CContactSyncPhonebookData::NewL(TUint32& aCRKey, CRepository* aRepository)
	{
	TUid temp;
	temp.iUid = 0;
	CContactSyncPhonebookData* self = new(ELeave) CContactSyncPhonebookData(temp, aRepository);
	self->InternaliseFromCRL(aCRKey);
	return self;
	}


CContactSyncPhonebookData* CContactSyncPhonebookData::NewLC(TUid aPhonebookUid, CRepository* aRepository)
	{
	CContactSyncPhonebookData* self = new(ELeave) CContactSyncPhonebookData(aPhonebookUid, aRepository);
	CleanupStack::PushL(self);
	return self;
	}


CContactSyncPhonebookData::CContactSyncPhonebookData(TUid aPhonebookUid, CRepository* aRepository)
	: iPhonebookUid(aPhonebookUid), iPhonebookTemplateId(KNullContactId), iPhonebookGroupId(KNullContactId), iRepository(aRepository)
	{
	}

const TDesC& CContactSyncPhonebookData::TemplateLabel()
	{
	_LIT(KPhbkTemplateADN,"SIM Card Contacts ADN"); // should be in a resource file for localisation really ;) 
	_LIT(KPhbkTemplateSDN,"SIM Card Contacts SDN");
	_LIT(KPhbkTemplateLND,"SIM Card Contacts LND");
	_LIT(KPhbkTemplateUSimApp,"SIM Card Contacts USIMAPP");
	_LIT(KPhbkTemplateFDN,"SIM Card Contacts FDN");
	_LIT(KPhbkTemplateNotSpecified,"SIM Card Contacts Name Not Specified");


	/* Some Contacts test code used to set group names
	_LIT(KGlobalAdnGroupName,"Global Adn Group");
	_LIT(KGlobalSdnGroupName,"Global Sdn Group");
	_LIT(KGlobalLdnGroupName,"Global Lnd Group");
	_LIT(KUsimAppAdnGroupName,"Usim App Adn Group");
	*/

	switch (iPhonebookUid.iUid)
		{
		case KUidIccGlobalAdnPhonebookValue:
			return KPhbkTemplateADN;

		case KUidIccGlobalSdnPhonebookValue:
			return KPhbkTemplateSDN;

		case KUidIccGlobalLndPhonebookValue:
			return KPhbkTemplateLND;

		case KUidUsimAppAdnPhonebookValue:
			return KPhbkTemplateUSimApp;

		case KUidIccGlobalFdnPhonebookValue:
			return KPhbkTemplateFDN;

		default:
			break;
		}

	return KPhbkTemplateNotSpecified;
	}


CContactSyncPhonebookData::~CContactSyncPhonebookData()
	{
	delete iIccTemplate;
	iStatusArray.Close();
	iPhonebookContacts.ResetAndDestroy();
	}


void CContactSyncPhonebookData::AddPhonebookContactL(const TDesC& aName, const TDesC& aNumber)
	{
	iPhonebookContacts.AppendL(CPhonebookContact::NewLC(aName, aNumber, iRepository));
	CleanupStack::Pop();	// CPhonebookContact
	}


TInt CContactSyncPhonebookData::SynchronisePhonebook(CContactDatabase& aDb, TInt aCompletionError, TInt aLeaveError)
	{
	TInt error(KErrNone);

	__ASSERT_DEBUG(iIsSynchronised == EFalse, User::Invariant());


	if ((aCompletionError == KErrNone) && (aLeaveError == KErrNone))
		{
		// Validate any template ID remembered from previous test
		if ((iPhonebookTemplateId != KNullContactId) &&    // if value has not been set
			(iPhonebookTemplateId != KGoldenTemplateId) )  // internally initialised value by cntmodel
			{
			TRAP(error, ValidateTemplateIdAndLabelL(aDb));

			if (error)
				{
				if (error != KErrNotFound)
					{
					return error;
					}
				iPhonebookTemplateId = KNullContactId;
				}
			}

		// Need to create a new template?
		if ((iPhonebookTemplateId == KNullContactId) ||  // if value has not been set
			(iPhonebookTemplateId == KGoldenTemplateId)) // internally initialised value by cntmodel
			{
			if (iIccTemplate)
				{
				delete iIccTemplate;
				iIccTemplate = NULL;
				}

			TRAP(error, CreateTemplateIdL(aDb)); // First create Template ID

			if (error == KErrNone)
				{
				// cache template
 				TRAP(error, iIccTemplate = aDb.ReadContactL(iPhonebookTemplateId));
				}

			if (error)
				{
				return error;
				}
			}



		// Validate any group ID remembered from previouzs test
		if ((iPhonebookGroupId != KNullContactId) &&    // if value has not been set
			(iPhonebookGroupId != KGoldenTemplateId) )  // internally initialised value by cntmodel
			{
			TRAP(error, ValidateGroupIdAndTemplateL(aDb));

			if (error)
				{
				if (error != KErrNotFound)
					{
					return error;
					}
				iPhonebookGroupId = KNullContactId;
				}
			}

		if ((iPhonebookGroupId == KNullContactId) ||    // if value has not been set
			(iPhonebookGroupId == KGoldenTemplateId) )  // internally initialised value by cntmodel
			{
			// Now create group ID for this phonebook
			TRAP(error, CreateGroupIdL(aDb));
			}

		if (error == KErrNone && iPhonebookContacts.Count() > 0)
			{
			TRAP(error, SynchroniseContactsL(aDb));
			}

		if (error)
			{
			return error;
			}

		// successfully synchronised
		iIsSynchronised = ETrue;
		}


	CompleteNotificationRequest(aCompletionError);
	iSynchronisedLeaveCode = aLeaveError;
	
	// simulated synchronisation completed
	return KErrNone;
	}


void CContactSyncPhonebookData::ValidateTemplateIdAndLabelL(CContactDatabase& aDb)
	{
	CContactItem* item = aDb.ReadContactLC(iPhonebookTemplateId);

	// The following test confirms if a template ID relates to this phonebook's 
	// template by checking the label employed.
	if ((item->Type() != KUidContactCardTemplate) ||
			(static_cast<CContactCardTemplate*>(item)->GetTemplateLabelL() != TemplateLabel()))
		{
		iPhonebookTemplateId = KNullContactId;
		}

	CleanupStack::PopAndDestroy(item);
	}


void CContactSyncPhonebookData::ValidateGroupIdAndTemplateL(CContactDatabase& aDb)
	{
	CContactGroup* group = static_cast<CContactGroup*> (aDb.ReadContactL(iPhonebookGroupId));

	CleanupStack::PushL(group);

	// check that contact id refers to a group
	if (group->Type() == KUidContactGroup)
		{
		// Retrieve the first entry in the group, and check that entry's template ID
		CContactIdArray* itemList = group->ItemsContainedLC();
		if (itemList->Count() > 0)
			{
			CContactItem* groupItem = aDb.ReadContactLC((*itemList)[0]);
			if (groupItem)
				{
				if (groupItem->TemplateRefId() != iPhonebookTemplateId)
					{
					// wrong group
					iPhonebookGroupId = KNullContactId;
					}
				CleanupStack::PopAndDestroy(groupItem);
				}
			}
		CleanupStack::PopAndDestroy(itemList);
		}
	else
		{
		// id is not really a group
		iPhonebookGroupId = KNullContactId;
		}

	CleanupStack::PopAndDestroy(group);
	}


/**
 Create Template UID for the phonebook.

 (Beware - can leave partially created Template)

 @param aDb Handle to the contacts database.
 */
void CContactSyncPhonebookData::CreateTemplateIdL(CContactDatabase &aDb)
	{
	__ASSERT_DEBUG((iPhonebookTemplateId == KNullContactId || iPhonebookTemplateId == KGoldenTemplateId), User::Invariant());

	// First check if there is an existing template in the contact database for this phonebook
	CContactIdArray* idList = aDb.GetCardTemplateIdListL();
	if (idList)
		{
		CleanupStack::PushL(idList);
		const TInt idListCount = idList->Count();
		if (idListCount > 0)
			{
			TInt i;
			for (i = 0; i < idListCount; i++)
				{
				CContactCardTemplate* item = static_cast<CContactCardTemplate*> (aDb.ReadContactLC((*idList)[i]));
				if (item->GetTemplateLabelL() == TemplateLabel())
					{
					iPhonebookTemplateId = (*idList)[i];
					i = idListCount; // force loop to exit
					}
				CleanupStack::PopAndDestroy(item);
				}
			}
		CleanupStack::PopAndDestroy(idList);
		}

	// If no suitable template been found create a new one
	if (iPhonebookTemplateId == KNullContactId ||
			iPhonebookTemplateId == KGoldenTemplateId)
		{
		// "Creating template for this phonebook"

		CContactItem* newTemplate = aDb.CreateContactCardTemplateLC(TemplateLabel());

		TContactItemId templateId = newTemplate->Id();
		CleanupStack::PopAndDestroy(newTemplate);

		// Remove all the unnecessary fields
		newTemplate = aDb.OpenContactLX(templateId);
		CleanupStack::PushL(newTemplate);	
		const TInt fieldCount = newTemplate->CardFields().Count();
		for(TInt i=fieldCount-1;i>=0;i--)
			newTemplate->RemoveField(i);
			
		// Add default name field
		CContactItemField* name = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldFamilyName);

		name->SetLabelL(_L("Name"));
		name->SetMapping(KUidContactFieldVCardMapUnusedN);
		newTemplate->AddFieldL(*name);
		CleanupStack::Pop(name);
			
		// Add second name field
		CContactItemField* secondName = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldSecondName);
		secondName->SetLabelL(_L("Additional Name"));
		secondName->SetMapping(KUidContactFieldVCardMapSECONDNAME);
		secondName->SetUserFlags(EContactCategoryHome);
		newTemplate->AddFieldL(*secondName);
		CleanupStack::Pop(secondName);
			
		// Add default number field
		CContactItemField* number = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldPhoneNumber);
		number->SetLabelL(_L("Phone Number"));
		number->SetMapping(KUidContactFieldVCardMapTEL);
		number->AddFieldTypeL(KUidContactFieldVCardMapWORK);
		number->AddFieldTypeL(KUidContactFieldVCardMapVOICE);
		number->AddFieldTypeL(KUidContactFieldVCardMapCELL);
		newTemplate->AddFieldL(*number);
		CleanupStack::Pop(number);

		// Add Slot Number field
		CContactItemField* slotnum = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldICCSlot);
		slotnum->SetLabelL(_L("Slot"));
		slotnum->SetMapping(KUidContactFieldVCardMapNotRequired);
		newTemplate->AddFieldL(*slotnum);
		CleanupStack::Pop(slotnum);

		// Add Phonebook type field
		CContactItemField* phonebook = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldICCPhonebook);
		phonebook->SetLabelL(_L("Phonebook UID"));
		phonebook->SetMapping(KUidContactFieldVCardMapNotRequired);
		newTemplate->AddFieldL(*phonebook);
		CleanupStack::Pop(phonebook);

		// presume this is a 3G ICC so there are additional fields for ADN and USIM App phonebooks 
		if((iPhonebookUid.iUid == KUidIccGlobalAdnPhonebookValue) || 
		   (iPhonebookUid.iUid == KUidUsimAppAdnPhonebookValue))
			{
			// Add e-mail field
			CContactItemField* emailField = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldEMail);
			emailField->SetLabelL(_L("Email"));
			emailField->SetMapping(KUidContactFieldVCardMapEMAILINTERNET);
			emailField->AddFieldTypeL(KUidContactFieldVCardMapWORK);
			emailField->SetUserFlags(EContactCategoryOther);
			newTemplate->AddFieldL(*emailField);
			CleanupStack::Pop(emailField);

			// Add group field - this is different from contacts group. This field indicates 
			// group that this ICC entry belongs to. User can add this entry to a number of 
			// groups on ICC i.e. business, private, etc. 
			CContactItemField* group = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldICCGroup);
			group->SetLabelL(_L("Group"));
			group->SetMapping(KUidContactFieldVCardMapUnusedN);
			newTemplate->AddFieldL(*group);
			CleanupStack::Pop(group);
			}

 		// Now that we know nothing as left, assign member variables (this avoids member variables being left in a bad state in the event of a leave).
 		iPhonebookTemplateId = templateId;

		aDb.CommitContactL(*newTemplate);
		CleanupStack::PopAndDestroy(2);	// newTemplate plus locked record
		}
	}


/**
 Create Group UID for the phonebook.
 @param aDb Handle to the contacts database.
 */
void CContactSyncPhonebookData::CreateGroupIdL(CContactDatabase &aDb)
	{
	__ASSERT_DEBUG((iPhonebookGroupId == KNullContactId ||
		iPhonebookGroupId == KGoldenTemplateId), User::Invariant());

	// First check if there is an existing group in the contact database for this phonebook
	CContactIdArray* idList = aDb.GetGroupIdListL();
	if (idList)
		{
		CleanupStack::PushL(idList);

		const TInt idListCount = idList->Count();
		if (idListCount > 0)
			{
			TInt i;
			for (i = 0; i < idListCount; i++)
				{
				// Retrieve the first entry in each group, and check that entry's template ID
				CContactGroup* group = static_cast<CContactGroup*> (aDb.ReadContactLC((*idList)[i]));
				CContactIdArray* itemList = group->ItemsContainedLC();
				if (itemList->Count() > 0)
					{
					CContactItem* groupItem = aDb.ReadContactLC((*itemList)[0]);
					if (groupItem->TemplateRefId() == iPhonebookTemplateId)
						{
						iPhonebookGroupId = (*idList)[i];
						i = idListCount; // force exit from loop
						}
					CleanupStack::PopAndDestroy(groupItem);
					}
				CleanupStack::PopAndDestroy(2, group); // itemList, group
				}
			}

		CleanupStack::PopAndDestroy(idList);
		}

	// If no suitable group has been found create a new one
	if (iPhonebookGroupId==KNullContactId ||
			iPhonebookGroupId==KGoldenTemplateId)
		{
		CContactItem* group = aDb.CreateContactGroupLC(KNullDesC);
 		iPhonebookGroupId = group->Id();
		CleanupStack::PopAndDestroy(group);
		}
	}


void CContactSyncPhonebookData::SynchroniseContactsL(CContactDatabase &aDb)
	{
	const TInt nContacts = iPhonebookContacts.Count();

	// simple version of phonebook sync
	for (TInt idx = 0; idx < nContacts; ++idx)
		{
		CContactICCEntry* item = CContactICCEntry::NewL(*iIccTemplate);
		CleanupStack::PushL(item); 

		CContactItemFieldSet& fieldSet = item->CardFields();

		// add name text
		TInt pos = fieldSet.Find(KUidContactFieldFamilyName);
		if (pos != KErrNotFound)
			{
			fieldSet[pos].TextStorage()->SetTextL(*iPhonebookContacts[idx]->Name());
			}

		// add phone number
		pos = fieldSet.Find(KUidContactFieldPhoneNumber);
		if (pos != KErrNotFound)
			{
			fieldSet[pos].TextStorage()->SetTextL(*iPhonebookContacts[idx]->Number());
			}

		// ICC Phonebook slot
		pos = fieldSet.Find(KUidContactFieldICCSlot);
		if (pos != KErrNotFound)
			{
			TBuf16<20> numberBuffer;
			numberBuffer.Format(_L("%d"), idx + 1);
			fieldSet[pos].TextStorage()->SetTextL(numberBuffer);
			}

		TContactItemId id = aDb.AddNewContactL(*item);



		CleanupStack::PopAndDestroy(item);
		
		// debug print fields
		CContactItem* contact = aDb.ReadContactLC(id);
		
		CContactItemFieldSet& contactFields = contact->CardFields();
		const TInt fieldCount = contactFields.Count();

		for (TInt i=0; i < fieldCount; i++)
			{
			CContactItemField &field = contactFields[i];
			if (field.StorageType() == KStorageTypeText)
				{
				TPtrC fieldText = field.TextStorage()->Text();
				TPtrC labelText = field.Label();
			    RDebug::Print(_L("--Field(%d) %S: %S \n\r"), i, &labelText, &fieldText);
				}
			}
		CleanupStack::PopAndDestroy(contact);	

		}

	if (nContacts)
		{
		aDb.CompactL();
		}
	}


void CContactSyncPhonebookData::ResetSynchronised(TBool aResetTemplateAndGroupIds)
	{
	if (aResetTemplateAndGroupIds)
		{
		iPhonebookTemplateId = KNullContactId;
		iPhonebookGroupId = KNullContactId;
		}

	// tell observers that synchronisation state has changed
	CompleteNotificationRequest(KErrNone);

	// not synchronised
	iIsSynchronised = EFalse;
	iSynchronisedLeaveCode = KErrNone;
	}


TInt CContactSyncPhonebookData::StoreNotificationRequest(TInt aStatus)
	{
//	aStatus = KRequestPending;
	iNotificationPending++;
	return iStatusArray.Append(aStatus);
	}


void CContactSyncPhonebookData::CompleteNotificationRequest(TInt aError)
	{	
	const TInt count = iStatusArray.Count();
	
	for (TInt k = count; k > 0; --k)
		{
		iNotificationPending--; 
		RProperty::Set(
			SyncNotification::KPropertyCategory, 
			iPhonebookUid.iUid, 
			aError);
		}
	iErrorCode = aError;
	iStatusArray.Reset();	
	}
	
void CContactSyncPhonebookData::CancelNotificationRequestL()
	{	
	const TInt count = iStatusArray.Count();
	RProperty property;
	
	for (TInt k = count; k > 0; --k)
		{
		iNotificationPending--; 
		User::LeaveIfError(property.Attach(SyncNotification::KPropertyCategory,iPhonebookUid.iUid));
		property.Cancel();
		}
	iStatusArray.Reset();	
	}


TInt CContactSyncPhonebookData::NotificationPendingCount()
	{
	return iStatusArray.Count();
	}


void CContactSyncPhonebookData::SetTemplateId(TInt aPhonebookTemplateId)
	{
	iPhonebookTemplateId = aPhonebookTemplateId;
	}


void CContactSyncPhonebookData::SetGroupId(TInt aPhonebookGroupId)
	{
	iPhonebookGroupId = aPhonebookGroupId;
	}


const TUid& CContactSyncPhonebookData::PhonebookUid()
	{
	return iPhonebookUid;
	}


TInt CContactSyncPhonebookData::TemplateId()
	{
	return iPhonebookTemplateId;
	}


TInt CContactSyncPhonebookData::GroupId()
	{
	return iPhonebookGroupId;
	}


void CContactSyncPhonebookData::SetSynchronisedLeaveCode(TInt aLeaveError)
	{
	iSynchronisedLeaveCode = aLeaveError;
	}


void CContactSyncPhonebookData::SetSynchronised(TInt aSynchronised)
	{
	iIsSynchronised = aSynchronised;
	}


TBool CContactSyncPhonebookData::IsSynchronisedL()
	{
	User::LeaveIfError(iSynchronisedLeaveCode);
	return iIsSynchronised;
	}


TBool CContactSyncPhonebookData::SynchroniseRequired()
	{
	return !iIsSynchronised;
	}
	
void CContactSyncPhonebookData::InternaliseFromCRL(TUint32& aCRKey)
	{
	TInt temp;
	CContactSyncStaticRepository::ReadIntFromCentralRepositoryL(aCRKey, temp, *iRepository);
    iPhonebookUid.iUid = temp;
	CContactSyncStaticRepository::ReadIntFromCentralRepositoryL(aCRKey, temp, *iRepository);
    iPhonebookTemplateId = temp;
	CContactSyncStaticRepository::ReadIntFromCentralRepositoryL(aCRKey, temp, *iRepository);
    iPhonebookGroupId = temp;
	CContactSyncStaticRepository::ReadIntFromCentralRepositoryL(aCRKey, iIsSynchronised, *iRepository);
	CContactSyncStaticRepository::ReadIntFromCentralRepositoryL(aCRKey, iNotificationPending, *iRepository);
	CContactSyncStaticRepository::ReadIntFromCentralRepositoryL(aCRKey, iErrorCode, *iRepository);
	CContactSyncStaticRepository::ReadIntFromCentralRepositoryL(aCRKey, iSynchronisedLeaveCode, *iRepository); 
	TInt notificationVectorSize = 0;
	CContactSyncStaticRepository::ReadIntFromCentralRepositoryL(aCRKey, notificationVectorSize, *iRepository);
    for (TInt i = 0 ; i < notificationVectorSize ; i++)
	    {    
		TInt notification;
		CContactSyncStaticRepository::ReadIntFromCentralRepositoryL(aCRKey, notification, *iRepository);
	    iStatusArray.Append(notification);	
	    }
	TInt numberOfPhonebookContacts = 0;
	CContactSyncStaticRepository::ReadIntFromCentralRepositoryL(aCRKey, numberOfPhonebookContacts, *iRepository);
	aCRKey+=10 - iStatusArray.Count();
    for (TInt i2 = 0 ; i2 < numberOfPhonebookContacts ; i2++)
	    {    
		CPhonebookContact* temp;
		temp = CPhonebookContact::NewL(aCRKey, iRepository);
		iPhonebookContacts.Append(temp);
	    }
	aCRKey+=10;
	}
	
void CContactSyncPhonebookData::ExternaliseToCRL(TUint32& aCRKey)
	{
	TInt temp = iPhonebookUid.iUid;
    //TRAP(Err, iRepository->Set(aCRKey++, temp));
	CContactSyncStaticRepository::WriteIntToCentralRepositoryL(aCRKey, temp, *iRepository);

    temp = iPhonebookTemplateId;
	CContactSyncStaticRepository::WriteIntToCentralRepositoryL(aCRKey, temp, *iRepository);
    temp = iPhonebookGroupId;
	CContactSyncStaticRepository::WriteIntToCentralRepositoryL(aCRKey, temp, *iRepository);
	CContactSyncStaticRepository::WriteIntToCentralRepositoryL(aCRKey, iIsSynchronised, *iRepository);
 	CContactSyncStaticRepository::WriteIntToCentralRepositoryL(aCRKey, iNotificationPending, *iRepository);
 	CContactSyncStaticRepository::WriteIntToCentralRepositoryL(aCRKey, iErrorCode, *iRepository);
	CContactSyncStaticRepository::WriteIntToCentralRepositoryL(aCRKey, iSynchronisedLeaveCode, *iRepository); 
 	CContactSyncStaticRepository::WriteIntToCentralRepositoryL(aCRKey, iStatusArray.Count(), *iRepository);
	for (TInt i = 0 ; i < iStatusArray.Count() ; i++)
	    {    
		TInt notification(iStatusArray[i]);
 		CContactSyncStaticRepository::WriteIntToCentralRepositoryL(aCRKey, notification, *iRepository);
	    }	
 	CContactSyncStaticRepository::WriteIntToCentralRepositoryL(aCRKey, iPhonebookContacts.Count(), *iRepository);
	aCRKey+=10 - iStatusArray.Count();
    for (TInt i2 = 0 ; i2 < iPhonebookContacts.Count() ; i2++)
	    { 
	    CPhonebookContact* temp = iPhonebookContacts[i2];
	    temp->WriteToCRL(aCRKey);   
	    }
	aCRKey+=10;
	}

TInt CContactSyncPhonebookData::ErrorCode()
	{
	return iErrorCode;
	}


