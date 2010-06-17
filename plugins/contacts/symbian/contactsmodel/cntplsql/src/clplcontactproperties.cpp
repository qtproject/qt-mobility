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


#include <barsc.h> // For resource reading.
#include <bautils.h>
#include <cntmodel.rsg> // For System template resources.
#include <phbksync.h> // For KUidIccGlobalAdnPhonebook.

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "cntsyncecom.h"
#endif

#include "persistencelayerimpl.h"
#include "clplcontactproperties.h" 
#include "plplugins.h"


CLplContactProperties::CLplContactProperties(RFs& aFs)
	:
	iFs(aFs),
	iDbViewContactType(KUidContactItem)
	{
	}


CLplContactProperties::~CLplContactProperties()
	{
	delete iPhoneParser;
	if (iSyncPlugin)
		{
		iSyncPlugin->Release();
		iSyncPlugin = NULL;
		}
	delete iSyncDummy;
	delete iSystemTemplate;
	delete iGroupIdList;
	}


void CLplContactProperties::SetContactItemManagerL(CPplContactItemManager& aContactItemManager)
	{
	__ASSERT_ALWAYS(iSystemTemplate == NULL, User::Leave(KErrArgument));
	iContactItemManager = &aContactItemManager;
	iSystemTemplate = CSystemTemplateManager::NewL(*iContactItemManager, iFs);
	}


void CLplContactProperties::SetDbViewContactType(TUid aDbViewContactType)
	{
	iDbViewContactType = aDbViewContactType;
	}


TUid CLplContactProperties::GetDbViewContactType() const
	{
	return iDbViewContactType;
	}

CContactPhoneParser& CLplContactProperties::ContactPhoneParserL()
	{
	if (!iPhoneParser)
		{
		iPhoneParser = CContactPhoneParser::NewL();
		}
	__ASSERT_DEBUG(iPhoneParser != NULL, User::Leave(KErrGeneral));
	return *iPhoneParser;
	}


void CLplContactProperties::DisableSynchroniser(TUint aSessionId)
	{
	iDisabledPhonebookSessionId = aSessionId;
	}


void CLplContactProperties::SessionDeleted(TUint aSessionId)
	{
	if (aSessionId == KIgnorePhbkSync || aSessionId == iDisabledPhonebookSessionId)
		{
		iDisabledPhonebookSessionId = 0;			
		}
	}


MContactSynchroniser& CLplContactProperties::ContactSynchroniserL(TUint aSessionId)
	{
	// If the caller asked to ignore the plugin the return the dummy plugin.
	if (aSessionId == KIgnorePhbkSync || aSessionId == iDisabledPhonebookSessionId)
		{
		if (!iSyncDummy)
			{
			iSyncDummy = new (ELeave) CDummySynchroniser();
			}
		// Return the dummy plugin.
		return *iSyncDummy;
		}
	else
		{
		if (!iSyncPlugin)
			{
			iSyncPlugin = CContactSynchroniser::NewL();
			}
		__ASSERT_DEBUG(iSyncPlugin != NULL, User::Leave(KErrGeneral));
		// Return the real plugin.
		return *iSyncPlugin;
		}
	}


CSystemTemplateManager& CLplContactProperties::SystemTemplateManager() const
   	{
  	return *iSystemTemplate;
  	}


const CContactTemplate& CLplContactProperties::SystemTemplateL() const
   	{
  	return iSystemTemplate->TemplateL();
  	}


const RArray<TUid>& CLplContactProperties::CustomFilterableFieldsL()
	{
	return iSystemTemplate->CustomFilterableFieldsL();
	}


void CLplContactProperties::RecreateSystemTemplateL()
	{
	iSystemTemplate->RecreateSystemTemplateL();
	}


TInt CLplContactProperties::CardTemplatePrefIdL() const
	{
	return iContactItemManager->CardTemplatePrefIdL();
	}

	
void CLplContactProperties::SetCardTemplatePrefIdL(TInt aCardTemplatePrefId)
	{
	iContactItemManager->SetCardTemplatePrefIdL(aCardTemplatePrefId);
	}


CContactIdArray& CLplContactProperties::CardTemplateIdsL()
	{
	return iContactItemManager->CardTemplateIdsL();
	}	


CContactIdArray& CLplContactProperties::GroupIdListL()
	{
	delete iGroupIdList;
	iGroupIdList = NULL;
	iGroupIdList = iContactItemManager->GroupIdListL();
	return *iGroupIdList;
	}	


CBufSeg* CLplContactProperties::DetailsListL(const TDesC& aSearchQuery) const 
    {
	return iContactItemManager->DetailsListL(aSearchQuery);
    }   


TInt64 CLplContactProperties::MachineIdL() const
	{
	return iContactItemManager->PreferencesPersitor().MachineIdL();
	}

	
void CLplContactProperties::SetMachineIdL(TInt64 aMachineId)
	{
	iContactItemManager->PreferencesPersitor().SetMachineIdL(aMachineId);
	}

	
TPtrC CLplContactProperties::UniqueIdL()
	{
	return iContactItemManager->PreferencesPersitor().DatabaseUidL();  
	}
	
	
CContactTemplate* CLplContactProperties::SystemTemplateFromResourceL(const TDesC& aResouceFileName) const
	{
	return iSystemTemplate->SystemTemplateFromResourceL(aResouceFileName);
	}	


TContactItemId CLplContactProperties::OwnCardIdL()
	{
	return iContactItemManager->OwnCardIdL();
	}


void CLplContactProperties::SetOwnCardIdL(TContactItemId aId, TBool aPersist)
	{
	iContactItemManager->SetOwnCardIdL(aId, aPersist);
	}


const CArrayFix<CContactDatabase::TSortPref>& CLplContactProperties::SortPrefsL()
	{
	return iContactItemManager->PreferencesPersitor().PreferredSortOrderL();
	}
 

void CLplContactProperties::SetSortPrefsL(CArrayFix<CContactDatabase::TSortPref>* aPrefs)
	{
	iContactItemManager->PreferencesPersitor().SetPreferredSortOrderL(aPrefs);
	}


/**
Determines whether or note the given contact type UID complies with one of a
set of rules.
*/
TBool CLplContactProperties::CheckType(TUid aUid) const
	{
	// Rule 1: Contact type UID is not the System template UID.
	if (aUid == KUidContactTemplate)
		{
		return EFalse;
		}

	// Rule 2: Contact type UID matches current view contact type UID.
	if (iDbViewContactType == KUidContactItem)
		{
		return ETrue;
		}

	// Rule 3: Contact type UID is a card or group and current view contact type
	// UID is cards or groups.
	if (iDbViewContactType == KUidContactCardOrGroup &&
		(aUid == KUidContactCard || aUid == KUidContactGroup))
		{
		return ETrue;
		}

	// Rule 4: Contact type UID is Own Card and current view contact type UID is
	// cards.
	if (iDbViewContactType == KUidContactCard && aUid == KUidContactOwnCard)
		{
		return ETrue;
		}
	// Rule 5: Contact type UID matches current view contact type UID.
	else if (aUid == iDbViewContactType)
		{
		return ETrue;
		}
	
	// Did not comply with any of the rules.	
	return EFalse;
	}


TContactItemId CLplContactProperties::ICCTemplateIdL(TUid aPhonebookUid, TUint aSessionId)
	{
	TContactItemId id = ContactSynchroniserL(aSessionId).ICCTemplateIdL(aPhonebookUid);
	return (aPhonebookUid == KNullUid) ? KNullContactId : id;
	}


TContactItemId CLplContactProperties::PhonebookGroupIdL(TUint aSessionId)
	{
	return ContactSynchroniserL(aSessionId).GroupIdL(KUidIccGlobalAdnPhonebook);
	}


CSystemTemplateManager* CSystemTemplateManager::NewL(CPplContactItemManager& aContactItemManager, RFs& aFileSession)
	{
	return new(ELeave) CSystemTemplateManager(aContactItemManager, aFileSession);
	}

	
CSystemTemplateManager::CSystemTemplateManager(CPplContactItemManager& aContactItemManager, RFs& aFileSession)
	:
	iContactItemManager(aContactItemManager),
	iFileSession(aFileSession)
	{
	}


CSystemTemplateManager::~CSystemTemplateManager()	
	{
	delete iSystemTemplate;
	iCustFiltFields.Reset();
	iCustFiltFields.Close();
	}


const CContactTemplate& CSystemTemplateManager::TemplateL()
	{
	if (iSystemTemplate)
		{
		return *iSystemTemplate;
		}

 	CContactItemViewDef* vDef = CContactItemViewDef::NewLC(CContactItemViewDef::EIncludeFields, CContactItemViewDef::EIncludeHiddenFields);
	vDef->AddL(KUidContactFieldMatchAll);
	iSystemTemplate = static_cast<CContactTemplate*>(iContactItemManager.ReadLC(KGoldenTemplateId, *vDef, EPlAllInfo, KIgnorePhbkSync)); 
	
	CleanupStack::Pop(); 
	CleanupStack::PopAndDestroy(vDef);

	return *iSystemTemplate;
	}

RArray<TUid>& CSystemTemplateManager::CustomFilterableFieldsL()
	{
	if (iCustFiltFields.Count() == 0) // Array is not yet initialised.
		{
		// Field order used to store user defined filter.
		TInt orderOfPriority = 0;

		// Initialise custom filterable fields.
		const TUid KUidCustFilterNullValue = {KNullContactId}; 
		TInt ii = 0;
		for (; ii < KMaxCustomFilterableFields; ++ii)
			{
			iCustFiltFields.InsertL(KUidCustFilterNullValue,ii);
			}

		ii = iSystemTemplate->CardFields().Count() - 1;
		EContactFieldFlags contactFieldFlag = EContactFieldFlagFilterable; 
		
		// For all the fields of the template find any fields which have one
		// of the filterable field flags and add them to the custom filterable
		// fields array.
		for (; ii>=0; --ii) 
			{
			// Current template field.
			CContactItemField& field = iSystemTemplate->CardFields()[ii]; 
				
			if (field.HasCustomFilter(contactFieldFlag))
				{
				const TFieldType fieldType = (field.ContentType()).FieldType(0);

				if (contactFieldFlag == EContactFieldFlagFilterable)
					{
					if (orderOfPriority > KMaxCustomFilterableFields)
						{
						User::Leave(KErrNotSupported);
						}
					iCustFiltFields[orderOfPriority++] = fieldType;
					}
				else if (contactFieldFlag == EContactFieldFlagFilterable1)
					{
					// Store filter value at highest priority position zero.
					iCustFiltFields[0] = fieldType;
					}
				else if (contactFieldFlag == EContactFieldFlagFilterable2)
					{
					// Store filter value at next priority position.
					iCustFiltFields[1] = fieldType;
					}
				else if (contactFieldFlag == EContactFieldFlagFilterable3)
					{
					iCustFiltFields[2] = fieldType;
					}
				else if (contactFieldFlag == EContactFieldFlagFilterable4)
					{
					iCustFiltFields[3] = fieldType;
					}
				}
			}

		// Validate array with customer filterable fields.  Leave if highest
		// priority UID is KUidCustFilterNullValue or if KUidCustFilterNullValue
		// appears between valid field type UIDs.
		ii = 0;
		for(; ii < KMaxCustomFilterableFields - 1; ++ii)
			{
			if (iCustFiltFields[ii] == KUidCustFilterNullValue && (iCustFiltFields[ii+1] != KUidCustFilterNullValue))
          		{
				User::Leave(KErrNotSupported);	
           		}
			}
			
		// Remove KUidCustFilterNullValue values from the end of the array.
		ii = KMaxCustomFilterableFields - 1;
		for(; ii >= 0; --ii)
			{
			if (iCustFiltFields[ii] == KUidCustFilterNullValue)	
			   {
			   iCustFiltFields.Remove(ii);
			   }	
			}
		}

		return iCustFiltFields;
	}
	

void CSystemTemplateManager::RecreateSystemTemplateL()
	{
 	#if defined(__PROFILE_DEBUG__)
		RDebug::Print(_L("[CNTMODEL] MTD: CSystemTemplateManager::RecreateSystemTemplateL"));
	#endif 
	
    _LIT(KCntSystemTemplate,"z:\\resource\\cntmodel\\cntmodel.rsc");

	CContactTemplate* systemTemplate = SystemTemplateFromResourceL(KCntSystemTemplate);
	CleanupStack::PushL(systemTemplate);

	if (iContactItemManager.IsDatabaseEmptyL())
		{
	    iContactItemManager.CreateL(*systemTemplate, KIgnorePhbkSync);
		}
	else
		{
		iContactItemManager.UpdateL(*systemTemplate, KIgnorePhbkSync);
		}
	
	CleanupStack::PopAndDestroy(systemTemplate);
	}


void CSystemTemplateManager::DeleteTemplate()
	{
	delete iSystemTemplate;
	iSystemTemplate = NULL;
	iCustFiltFields.Reset();
	iCustFiltFields.Close();
	}


CContactTemplate* CSystemTemplateManager::SystemTemplateFromResourceL(const TDesC& aResouceFileName)
	{
	TFileName fileName(aResouceFileName);
	BaflUtils::NearestLanguageFile(iFileSession, fileName);

	RResourceFile resourceFile;
	resourceFile.OpenL(iFileSession, fileName);
	CleanupClosePushL(resourceFile);
	resourceFile.ConfirmSignatureL(4);
	
	TResourceReader resReader;
	resReader.SetBuffer(resourceFile.AllocReadLC(R_CNTUI_NEW_FIELD_DEFNS));

	CContactTemplate* templ = CContactTemplate::NewLC();
	TInt count 				= resReader.ReadInt16(); // num fields
	
	while(count--)
		{
		TStorageType storageType = resReader.ReadInt32();
		TUid contactFieldUid	 = TUid::Uid(resReader.ReadInt32());
		TUid vCardMappingUid 	 = TUid::Uid(resReader.ReadInt32());
		CContactItemField* field = CContactItemField::NewLC(storageType);

		field->SetMapping (vCardMappingUid);
		if (contactFieldUid!=KUidContactFieldNone)
			{
			field->AddFieldTypeL (contactFieldUid);
			}

		TInt mappingCount = 0;
		if((mappingCount=resReader.ReadInt16())!=0)
			{
			while(mappingCount--)
				{
				field->AddFieldTypeL(TUid::Uid(resReader.ReadInt32()));
				}
			}

		field->SetUserFlags(resReader.ReadInt32());
		field->SetLabel(resReader.ReadHBufCL());

		TUint flags=resReader.ReadInt32();
		if (flags&EContactFieldFlagHidden)
			{
			field->SetHidden(ETrue);
			}
		if (flags&EContactFieldFlagReadOnly)
			{
			field->SetReadOnly(ETrue);
			}
		if (flags&EContactFieldFlagSynchronize)
			{
			field->SetSynchronize(ETrue);
			}
		if (flags&EContactFieldFlagDisabled)
			{
			field->SetDisabled(ETrue);
			}

		// Custom filterable fields.  Here we only set field flags, construction
		// and validation of the custom filterable fields array happens on
		// demand in CustomFilterableFieldsL() method.
		if (flags&EContactFieldFlagFilterable)
			{
			field->SetCustomFilterable(EContactFieldFlagFilterable);
			}
		else if (flags&EContactFieldFlagFilterable1)
			{
			field->SetCustomFilterable(EContactFieldFlagFilterable1);
			}
		else if (flags&EContactFieldFlagFilterable2)
			{
			field->SetCustomFilterable(EContactFieldFlagFilterable2);
			}
		else if (flags&EContactFieldFlagFilterable3)
			{
			field->SetCustomFilterable(EContactFieldFlagFilterable3);
			}
		else if (flags&EContactFieldFlagFilterable4)
			{
			field->SetCustomFilterable(EContactFieldFlagFilterable4);
			}

		templ->AddFieldL(*field);
		CleanupStack::Pop(field);
		}
	
	CleanupStack::Pop(templ);
	CleanupStack::PopAndDestroy(2, &resourceFile); // resRead buf, close res files

	return templ;
	}


TInt CDummySynchroniser::ValidateContact(TValidateOperation, TContactItemId)
	{
	return KErrNone;
	}


TInt CDummySynchroniser::ValidateWriteContact(CContactICCEntry&)
	{
	return KErrNone;
	}


TInt CDummySynchroniser::DeleteContact(TContactItemId)
	{
	return KErrNone;
	}


void CDummySynchroniser::Release()
	{
	}


void CDummySynchroniser::UpdatePostWriteL(const CContactICCEntry&)
	{
	}


TContactItemId CDummySynchroniser::ICCTemplateIdL(TUid)
	{
	return KNullContactId;
	}


TContactItemId CDummySynchroniser::GroupIdL(TUid)
	{
	return KNullContactId;
	}


void CDummySynchroniser::NotifySyncStateChange(TRequestStatus& aStatus, TUid)
	{
	aStatus = KRequestPending;
	TRequestStatus* stat = &aStatus;
	User::RequestComplete(stat, KErrNone);
	}


void CDummySynchroniser::CancelNotifyRequest(TUid)
	{
	}


TBool CDummySynchroniser::IsSynchronisedL(TUid)
	{
	return ETrue;
	}


TInt CDummySynchroniser::PhonebookList(RArray<TUid>&)
	{
	return KErrNone;
	}
