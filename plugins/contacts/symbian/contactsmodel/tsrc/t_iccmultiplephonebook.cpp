/*
* Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <cntdb.h>
#include <cntitem.h>
#include <cntfield.h>
#include <cntfldst.h>
#include <phbksync.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "cntsyncecom.h"
#endif

// include templates for CleanupResetAndDestroyPushL(T)
#include "cntviewprivate.h"

#include "t_utils.h"
#include "cntsyncchecker.h"

_LIT(KTestName,"CContactICCEntry tests with multiple phonebooks");
_LIT(KDatabaseFileName,"C:T_ICCMULTIPLEPHONEBOOK");
_LIT(KGivenEditName,"edited name");
_LIT(KIccName,"icc entry name");
_LIT(KNonIccName,"non-icc name");
_LIT(KIccNumber,"020 794 60123");
_LIT(KNonIccNumber,"020 794 60060");

_LIT(KPluginName,"phone book synchronizer Implementation");
_LIT(KTestPluginName,"Test phone book synchronizer Implementation");

CCntTest* CntTest=NULL;
LOCAL_D RTest test(KTestName);

LOCAL_D CContactSyncChecker* syncChecker;

//===============================================================================================
// SETUP & UTILITIES
//===============================================================================================
class TFieldEntry
	{
	public:
		TFieldEntry(TFieldType aField);
		TFieldEntry(TFieldType aField,TUid aFieldType);
	public:
		TFieldType iField;
		TUid iFieldType;
	};

TFieldEntry::TFieldEntry(TFieldType aField)
:iField(aField)
,iFieldType(KUidIccPhonebookNotFound)
	{}

TFieldEntry::TFieldEntry(TFieldType aField,TUid aFieldType)
:iField(aField)
,iFieldType(aFieldType)
	{}

/**
 * Create a new template and add it to the database
 * @param aDb Contact database
 * @param aTemplateName Name for the template
 * @param aFieldSet Array of fields which should are added to the template
 * @return TContactItemId Id of the newly created template
 */
TContactItemId CreatePhonebookTemplateL(CContactDatabase& aDb, const TDesC& aTemplateName, RArray<TFieldEntry>& aFieldSet)
	{
	CContactItem* temp = aDb.CreateContactCardTemplateLC(aTemplateName);
	TContactItemId templateId = temp->Id();
	CleanupStack::PopAndDestroy(temp);
	temp=NULL;

	//Remove all the unnecessary fields
	temp = aDb.OpenContactLX(templateId);
	CleanupStack::PushL(temp);	
	const TInt fieldCount = temp->CardFields().Count();
	for(TInt i=fieldCount-1;i>=0;i--)
		temp->RemoveField(i);

	// Add each of the required fields to the template
	for (TInt j=0; j<aFieldSet.Count(); ++j)
		{
		TFieldEntry fieldEntry=aFieldSet[j];

		CContactItemField* field = CContactItemField::NewLC(KStorageTypeText,fieldEntry.iField);
		if (fieldEntry.iFieldType!=KNullUid)
			field->AddFieldTypeL(fieldEntry.iFieldType);
		temp->AddFieldL(*field);
		CleanupStack::Pop(field);
		}

	aDb.CommitContactL(*temp);
	CleanupStack::PopAndDestroy(2); // temp, close template
	
	return templateId;
	}

/**
 * Return an array of fields for the phonebook specified
 * @param aPhonebookUid The phonebook uid
 * @return RArray<TFieldEntry> Field set for this specific phonebook.  Any contact item
 * which belongs to this phonebook should contain these fields
 */
RArray<TFieldEntry> fieldSetForPhonebook(TUid aPhonebookUid)
	{
	RArray<TFieldEntry> entries;
	entries.Append(TFieldEntry(KUidContactFieldFamilyName));
	entries.Append(TFieldEntry(KUidContactFieldGivenName));
	entries.Append(TFieldEntry(KUidContactFieldPhoneNumber,KUidContactFieldVCardMapHOME));
	entries.Append(TFieldEntry(KUidContactFieldICCPhonebook));
	
	if (aPhonebookUid==KUidUsimAppAdnPhonebook)
		{
		entries.Append(TFieldEntry(KUidContactFieldEMail,KUidContactFieldVCardMapHOME));
		entries.Append(TFieldEntry(KUidContactFieldEMail,KUidContactFieldVCardMapWORK));
		entries.Append(TFieldEntry(KUidContactFieldPhoneNumber,KUidContactFieldVCardMapHOME));
		entries.Append(TFieldEntry(KUidContactFieldPhoneNumber,KUidContactFieldVCardMapWORK));
		entries.Append(TFieldEntry(KUidContactFieldPhoneNumber,KUidContactFieldVCardMapCELL));
		entries.Append(TFieldEntry(KUidContactFieldPhoneNumber,KUidContactFieldVCardMapPAGER));
		}

	return entries;
	}

void SetUpIccPhonebooksL(CContactDatabase& aDb)
	{
	test.Next(_L("Set up phonebooks"));
	RArray<TUid>	phbkList;
	CleanupClosePushL(phbkList);

	phbkList.AppendL(KUidIccGlobalAdnPhonebook);
	phbkList.AppendL(KUidIccGlobalSdnPhonebook);
	phbkList.AppendL(KUidIccGlobalLndPhonebook);
	phbkList.AppendL(KUidUsimAppAdnPhonebook);
	// tell phonebook sync simulator what phonebooks to use
	test(syncChecker->SetPhonebookListL(phbkList) == KErrNone);

	// simulate synchronisation - includes creating/finding template & group
	// also defaults all validate responses to KErrNone
	test(syncChecker->SynchroniseIccAllPhonebooksL(aDb, KErrNone, KErrNone) == KErrNone);
	
	CleanupStack::PopAndDestroy(); // phbkList
	}

/**
 * Check the content of the phonebook field in the contact item
 * @param aContactItem Contact item
 * @param aPhonebookUid The phonebook uid
 * @param aFieldIsSet EFalse if the phonebook field should not contain a value, else ETrue
 */
void CheckPhonebookField(CContactItem& aContactItem,TUid aPhonebookUid,TBool aFieldIsSet)
	{
	const CContactItemFieldSet& fieldset = aContactItem.CardFields();
	const TInt pos = fieldset.Find(KUidContactFieldICCPhonebook);
	test(pos!=KErrNotFound);
	TPtrC phonebookField=fieldset[pos].TextStorage()->Text();

	if(aFieldIsSet)
		{
		TLex input(phonebookField);
		TInt phonebookFieldVal;
		/*TInt ignore  = */ input.Val(phonebookFieldVal);
		TUid phonebookFieldUid;
		phonebookFieldUid.iUid=phonebookFieldVal;
		test(phonebookFieldUid==aPhonebookUid);
		}
	else
		test(phonebookField==KNullDesC);
	}

/**
 * Verify that the template id returned by CContactDatabase::ICCTemplateIdL is the
 * same as that returned directly from the plugin
 * @param aDb Contact database
 */
void VerifyPhonebookTemplateIdsL(CContactDatabase& aDb)
	{
	test.Next(_L("Verify template ids"));
	syncChecker->ResetMethodCallCountsL();
	test(aDb.ICCTemplateIdL() == syncChecker->ICCTemplateIdL(KUidIccGlobalAdnPhonebook));
	test(aDb.ICCTemplateIdL(KUidIccGlobalAdnPhonebook) == syncChecker->ICCTemplateIdL(KUidIccGlobalAdnPhonebook));
	test(aDb.ICCTemplateIdL(KUidIccGlobalSdnPhonebook) == syncChecker->ICCTemplateIdL(KUidIccGlobalSdnPhonebook));
	test(aDb.ICCTemplateIdL(KUidIccGlobalLndPhonebook) == syncChecker->ICCTemplateIdL(KUidIccGlobalLndPhonebook));
	test(aDb.ICCTemplateIdL(KUidUsimAppAdnPhonebook) == syncChecker->ICCTemplateIdL(KUidUsimAppAdnPhonebook));
	test(aDb.ICCTemplateIdL(KNullUid) == KNullContactId);
	test(syncChecker->ValidateMethodCallCountL() == 6);
	}

/**
 * Set the name field of the contact with id aId to contain the name aName
 * @param aDb Contact database
 * @param aId Contact item id
 * @param aName A name
 */
void SetNameField(CContactDatabase& aDb,TContactItemId aId,const TDesC& aName)
	{
	syncChecker->ResetMethodCallCountsL();
	CContactItem* item = aDb.OpenContactLX(aId);
	CleanupStack::PushL(item);
	if (item->Type() == KUidContactICCEntry)
		{
		test(syncChecker->ValidateMethodCallCountL() == 2);
		}
	else
		{
		test(syncChecker->ValidateMethodCallCountL() == 0);
		}
	CContactItemFieldSet& fieldset = item->CardFields();
	TInt pos = fieldset.Find(KUidContactFieldFamilyName);
	test(pos!=KErrNotFound);
	CContactItemField& field = fieldset[pos];
	CContactTextField* textfield = field.TextStorage();
	textfield->SetTextL(aName);

	syncChecker->ResetMethodCallCountsL();
	aDb.CommitContactL(*item);
	if (item->Type() == KUidContactICCEntry)
		{
		test(syncChecker->ValidateMethodCallCountL() == 2);
		}
	else
		{
		test(syncChecker->ValidateMethodCallCountL() == 0);
		}
	CleanupStack::PopAndDestroy(2); // item, lock record
	}

/**
 * Set the phone number field of the contact with id aId to contain the number aNumber
 * @param aDb Contact database
 * @param aId Contact item id
 * @param aNumber A telephone number
 */
void SetPhoneNumberField(CContactDatabase& aDb,TContactItemId aId,const TDesC& aNumber)
	{
	syncChecker->ResetMethodCallCountsL();
	CContactItem* item = aDb.OpenContactLX(aId);
	CleanupStack::PushL(item);
	if (item->Type() == KUidContactICCEntry)
		{
		test(syncChecker->ValidateMethodCallCountL() == 2);
		}
	else
		{
		test(syncChecker->ValidateMethodCallCountL() == 0);
		}
	CContactItemFieldSet& fieldset = item->CardFields();
	TInt pos = fieldset.Find(KUidContactFieldPhoneNumber);
	test(pos!=KErrNotFound);
	
	CContactItemField& field = fieldset[pos];
	CContactTextField* textfield = field.TextStorage();
	textfield->SetTextL(aNumber);

	syncChecker->ResetMethodCallCountsL();
	aDb.CommitContactL(*item);
	if (item->Type() == KUidContactICCEntry)
		{
		test(syncChecker->ValidateMethodCallCountL() == 2);
		}
	else
		{
		test(syncChecker->ValidateMethodCallCountL() == 0);
		}
	CleanupStack::PopAndDestroy(2); // item, lock record
	}

//===============================================================================================
// ADDING entries
//===============================================================================================
/**
 * Create an entry based on the template for this specific phonebook, and add
 * the entry to the contact database
 * @param aDb Contact database
 * @param aPhonebookUid The phonebook uid
 * @return TContactItemId Id of the newly created icc entry
 */
TContactItemId doAddIccEntryL(CContactDatabase& aDb,TUid aPhonebookUid)
	{
	syncChecker->ResetMethodCallCountsL();
	TContactItemId templateId = aDb.ICCTemplateIdL(aPhonebookUid);
	test(syncChecker->ValidateMethodCallCountL() == 1);
	CContactItem* iccTemplate = aDb.ReadContactLC(templateId);
	CContactICCEntry* entry = CContactICCEntry::NewL(*iccTemplate);
	CleanupStack::PopAndDestroy(iccTemplate);
	CleanupStack::PushL(entry);
	// Add to the database
	CheckPhonebookField(*entry,aPhonebookUid, EFalse);
	syncChecker->ResetMethodCallCountsL();
	TContactItemId id = aDb.AddNewContactL(*entry); 
	test(syncChecker->ValidateMethodCallCountL() == 3);
	CleanupStack::PopAndDestroy(entry);
	test(id!=KNullContactId);
	// Check group membership
	syncChecker->ResetMethodCallCountsL();
	CContactICCEntry* fetchedItem = static_cast<CContactICCEntry*>(aDb.ReadContactL(id));
	test(syncChecker->ValidateMethodCallCountL() == 1);
	CleanupStack::PushL(fetchedItem);	
	const CContactIdArray* owned = fetchedItem->GroupsJoined();
	test(owned!=NULL && owned->Count() == 1);
	test((*owned)[0]==syncChecker->GroupIdL(aPhonebookUid));
	test(fetchedItem->Type() == KUidContactICCEntry);
	// Verify that the phonebook field has been set
	CheckPhonebookField(*fetchedItem,aPhonebookUid, ETrue);
	CleanupStack::PopAndDestroy(fetchedItem);	

	return id;
	}

TContactItemId AddIccEntryL(CContactDatabase& aDb,TUid aPhonebookUid)
	{
	syncChecker->SetValidateWriteResponseL(KErrNone);
	return doAddIccEntryL(aDb, aPhonebookUid);
	}

void UnsuccessfulAddIccEntryL(CContactDatabase& aDb,TUid aPhonebookUid, TInt aReason)
	{
	syncChecker->SetValidateWriteResponseL(aReason);
	TRAPD(err,doAddIccEntryL(aDb, aPhonebookUid));
	test(err==aReason);
	}

void AddEntriesBasedOnEachOfTheTemplatesL(CContactDatabase& aDb)
	{
	test.Next(_L("Test successful adding"));
	TInt numberOfAdnEntries=3;
	TInt numberOfSdnEntries=2;
	TInt numberOfLdnEntries=1;
	TInt numberOfUsimEntries=4;
	TInt iter=0;
	for (iter=0; iter<numberOfAdnEntries; ++iter)
		{
		TContactItemId id=AddIccEntryL(aDb,KUidIccGlobalAdnPhonebook);
		CContactGroup* group=static_cast<CContactGroup*>(aDb.ReadContactLC(syncChecker->GroupIdL(KUidIccGlobalAdnPhonebook)));
		CContactIdArray* entries=group->ItemsContainedLC();
		test(entries->Count() == iter+1);
		CleanupStack::PopAndDestroy(entries);
		test(group->ContainsItem(id));
		CleanupStack::PopAndDestroy(group);
		}
	for (iter=0; iter<numberOfSdnEntries; ++iter)
		{
		TContactItemId id=AddIccEntryL(aDb,KUidIccGlobalSdnPhonebook);
		CContactGroup* group=static_cast<CContactGroup*>(aDb.ReadContactLC(syncChecker->GroupIdL(KUidIccGlobalSdnPhonebook)));
		CContactIdArray* entries=group->ItemsContainedLC();
		test(entries->Count() == iter+1);
		CleanupStack::PopAndDestroy(entries);
		test(group->ContainsItem(id));
		CleanupStack::PopAndDestroy(group);
		}
	for (iter=0; iter<numberOfLdnEntries; ++iter)
		{
		TContactItemId id=AddIccEntryL(aDb, KUidIccGlobalLndPhonebook);
		CContactGroup* group=static_cast<CContactGroup*>(aDb.ReadContactLC(syncChecker->GroupIdL(KUidIccGlobalLndPhonebook)));
		CContactIdArray* entries=group->ItemsContainedLC();
		test(entries->Count() == iter+1);
		CleanupStack::PopAndDestroy(entries);
		test(group->ContainsItem(id));
		CleanupStack::PopAndDestroy(group);
		}
	for (iter=0; iter<numberOfUsimEntries; ++iter)
		{
		TContactItemId id=AddIccEntryL(aDb,KUidUsimAppAdnPhonebook);
		CContactGroup* group=static_cast<CContactGroup*>(aDb.ReadContactLC(syncChecker->GroupIdL(KUidUsimAppAdnPhonebook)));
		CContactIdArray* entries=group->ItemsContainedLC();
		test(entries->Count() == iter+1);
		CleanupStack::PopAndDestroy(entries);
		test(group->ContainsItem(id));
		CleanupStack::PopAndDestroy(group);
		}
	// Add an ICC entry based on the system template, not one of the ICC templates
	TContactItemId templateId=aDb.TemplateId();
	CContactItem* systemTemplate = aDb.ReadContactLC(templateId);
	CContactICCEntry* entry = CContactICCEntry::NewL(*systemTemplate);
	CleanupStack::PopAndDestroy(systemTemplate);
	CleanupStack::PushL(entry);
	syncChecker->ResetMethodCallCountsL();
	TContactItemId id = aDb.AddNewContactL(*entry); 
	test(syncChecker->ValidateMethodCallCountL() == 3);

	syncChecker->ResetMethodCallCountsL();
	CContactItem* item=aDb.ReadContactLC(id);
	test(syncChecker->ValidateMethodCallCountL() == 1);
	// check the entry hasn't been added to any group
	CContactIdArray* array=static_cast<CContactICCEntry*>(item)->GroupsJoinedLC();
	test(array->Count() == 0);
	CleanupStack::PopAndDestroy(3); // array, close item, item
	}

void TestUnsuccessfulAddL(CContactDatabase& aDb)
	{
	// Test adding entries to each of the phonebooks when the ICC is locked, unaccessible or corrupt
	test.Next(_L("Test unsuccessful adding"));
	UnsuccessfulAddIccEntryL(aDb,KUidIccGlobalAdnPhonebook,KErrLocked);
	UnsuccessfulAddIccEntryL(aDb,KUidIccGlobalAdnPhonebook,KErrAccessDenied);
	UnsuccessfulAddIccEntryL(aDb,KUidIccGlobalAdnPhonebook,KErrCorrupt);
	UnsuccessfulAddIccEntryL(aDb,KUidIccGlobalSdnPhonebook,KErrLocked);
	UnsuccessfulAddIccEntryL(aDb,KUidIccGlobalSdnPhonebook,KErrAccessDenied);
	UnsuccessfulAddIccEntryL(aDb,KUidIccGlobalSdnPhonebook,KErrCorrupt);
	UnsuccessfulAddIccEntryL(aDb,KUidIccGlobalLndPhonebook,KErrLocked);
	UnsuccessfulAddIccEntryL(aDb,KUidIccGlobalLndPhonebook,KErrAccessDenied);
	UnsuccessfulAddIccEntryL(aDb,KUidIccGlobalLndPhonebook,KErrCorrupt);
	UnsuccessfulAddIccEntryL(aDb,KUidUsimAppAdnPhonebook,KErrLocked);
	UnsuccessfulAddIccEntryL(aDb,KUidUsimAppAdnPhonebook,KErrAccessDenied);
	UnsuccessfulAddIccEntryL(aDb,KUidUsimAppAdnPhonebook,KErrCorrupt);
	}

/** 
 * Verify that adding contact cards doesn't call the synchroniser interface
 * @param aDb Contact database
 */
TContactItemId AddContactCardL(CContactDatabase& aDb)
	{
	_LIT(KTemplateName,"contact card template");
	RArray<TFieldEntry> entries=fieldSetForPhonebook(KUidIccGlobalAdnPhonebook);
	TContactItemId templateId=CreatePhonebookTemplateL(aDb,KTemplateName,entries);
	entries.Close();
	CContactItem* iccTemplate = aDb.ReadContactLC(templateId);
	CContactCard* card = CContactCard::NewLC(iccTemplate);
	syncChecker->ResetMethodCallCountsL();
	TContactItemId id=aDb.AddNewContactL(*card);
	test(syncChecker->ValidateMethodCallCountL() == 0);
	CleanupStack::PopAndDestroy(2,iccTemplate);
	return id;
	}
//===============================================================================================
// DELETING entries
//===============================================================================================
/**
 * Verify that the synchroniser is called when deleting an icc entry, and that the 
 * deletion of an icc entry fails if the icc is locked.
 * @param aDb Contact database
 */
void TestDeleteL(CContactDatabase& aDb)
	{
	test.Next(_L("Test delete when ICC locked"));
	syncChecker->SetDeleteContactResponseL(KErrLocked);
	TContactItemId iccId=AddIccEntryL(aDb,KUidIccGlobalAdnPhonebook);
	syncChecker->ResetMethodCallCountsL();
	TRAPD(err,aDb.DeleteContactL(iccId));
	test(err==KErrLocked);
	test(syncChecker->ValidateMethodCallCountL() == 2);
	syncChecker->ResetMethodCallCountsL();
	CContactItem* item=aDb.ReadContactLC(iccId);
	test(syncChecker->ValidateMethodCallCountL() == 1);
	test(item!=NULL);
	CleanupStack::PopAndDestroy(item);
	item=NULL;

	test.Next(_L("Test successful delete of non-icc entry when ICC locked"));
	TContactItemId nonIccId=AddContactCardL(aDb);
	syncChecker->ResetMethodCallCountsL();
	TRAP(err,aDb.DeleteContactL(nonIccId));
	test(err==KErrNone);
	test(syncChecker->ValidateMethodCallCountL() == 0);

	test.Next(_L("Test successful delete"));
	syncChecker->SetDeleteContactResponseL(KErrNone);
	syncChecker->ResetMethodCallCountsL();
	TRAP(err,aDb.DeleteContactL(iccId));
	test(syncChecker->ValidateMethodCallCountL() == 2);
	test(err==KErrNone);
	syncChecker->ResetMethodCallCountsL();
	TRAP(err,aDb.ReadContactLC(iccId));
	test(syncChecker->ValidateMethodCallCountL() == 0);
	test(err==KErrNotFound);
	}

//===============================================================================================
// READING entries
//===============================================================================================
void TestReadMinimalContactL(CContactDatabase& aDb, TContactItemId aIccId, TContactItemId aNonIccId)
	{
	test.Next(_L("Test ReadMinimalContactL"));
	// Successful read of icc entry
	syncChecker->SetValidateResponseL(MContactSynchroniser::ERead,KErrNone);
	syncChecker->ResetMethodCallCountsL();
	CContactItem* item=aDb.ReadMinimalContactLC(aIccId);
	test(syncChecker->ValidateMethodCallCountL() == 1);
	test(item!=NULL);
	CleanupStack::PopAndDestroy(item);
	item=NULL;

	// Unsuccessful read of icc entry because icc locked
	syncChecker->SetValidateResponseL(MContactSynchroniser::ERead,KErrAccessDenied);
	syncChecker->ResetMethodCallCountsL();
	TRAPD(err,item=aDb.ReadMinimalContactLC(aIccId));
	test(syncChecker->ValidateMethodCallCountL() == 1);
	test(item==NULL);
	test(err==KErrAccessDenied);
	
	// successful read of non-icc entry, even though icc locked
	syncChecker->ResetMethodCallCountsL();
	item=aDb.ReadMinimalContactLC(aNonIccId);
	test(syncChecker->ValidateMethodCallCountL() == 0);
	test(item!=NULL);
	CleanupStack::PopAndDestroy(item);
	item=NULL;
	}

void TestReadContactL(CContactDatabase& aDb, TContactItemId aIccId, TContactItemId aNonIccId)
	{
	test.Next(_L("Test ReadContactL"));
	// Successful read of icc entry
	syncChecker->SetValidateResponseL(MContactSynchroniser::ERead,KErrNone);
	syncChecker->ResetMethodCallCountsL();
	test(syncChecker->ValidateMethodCallCountL() == 0);	
	CContactItem* item=aDb.ReadContactLC(aIccId);
	test(syncChecker->ValidateMethodCallCountL() == 1);
	test(item!=NULL);
	CleanupStack::PopAndDestroy(item);
	item=NULL;

	// Unsuccessful read of icc entry because icc locked
	syncChecker->SetValidateResponseL(MContactSynchroniser::ERead,KErrAccessDenied);
	syncChecker->ResetMethodCallCountsL();
	test(syncChecker->ValidateMethodCallCountL() == 0);
	TRAPD(err,item=aDb.ReadContactL(aIccId));
	test(item==NULL);
	test(err==KErrAccessDenied);
	test(syncChecker->ValidateMethodCallCountL() == 1);
	
	// successful read of non-icc entry, even though icc locked
	syncChecker->ResetMethodCallCountsL();
	test(syncChecker->ValidateMethodCallCountL() == 0);	
	item=aDb.ReadContactLC(aNonIccId);
	test(syncChecker->ValidateMethodCallCountL() == 0);
	test(item!=NULL);
	CleanupStack::PopAndDestroy(item);
	item=NULL;
	}

void TestReadTextDefinitionL(CContactDatabase& aDb, TContactItemId aIccId, TContactItemId aNonIccId)
	{
	test.Next(_L("Test ReadTextDefinitionL"));
	// Successful read of icc entry
	CContactTextDef* textDef=CContactTextDef::NewLC();
	textDef->AppendL(TContactTextDefItem(KUidContactFieldFamilyName));
	TBuf<64> result;
	syncChecker->SetValidateResponseL(MContactSynchroniser::ERead,KErrNone);
	syncChecker->ResetMethodCallCountsL();
	aDb.ReadContactTextDefL(aIccId,result,textDef);
	test(syncChecker->ValidateMethodCallCountL() == 1);
	test(result==KIccName);
	result.Zero();

	// Unsuccessful read of icc entry because icc locked
	syncChecker->SetValidateResponseL(MContactSynchroniser::ERead,KErrAccessDenied);
	syncChecker->ResetMethodCallCountsL();
	TRAPD(err, aDb.ReadContactTextDefL(aIccId,result,textDef));
	test(err==KErrAccessDenied);
	test(result.Length() == 0);
	test(syncChecker->ValidateMethodCallCountL() == 1);

	// successful read of non-icc entry, even though icc locked
	syncChecker->ResetMethodCallCountsL();
	TRAP(err, aDb.ReadContactTextDefL(aNonIccId,result,textDef));
	test(err==KErrNone);
	test(result==KNonIccName);
	test(syncChecker->ValidateMethodCallCountL() == 0);
	CleanupStack::PopAndDestroy(textDef);
	}

/**
 * Verify that the synchroniser is called when reading an icc entry
 * @param aDb Contact database
 */
void TestReadL(CContactDatabase& aDb)
	{
	TContactItemId iccId=AddIccEntryL(aDb,KUidIccGlobalAdnPhonebook);
	SetNameField(aDb,iccId,KIccName);
	TContactItemId nonIccId = AddContactCardL(aDb);
	SetNameField(aDb,nonIccId,KNonIccName);
	TestReadMinimalContactL(aDb,iccId,nonIccId);
	TestReadContactL(aDb,iccId,nonIccId);
	TestReadTextDefinitionL(aDb,iccId,nonIccId);
	syncChecker->SetValidateResponseL(MContactSynchroniser::ERead,KErrNone);
	syncChecker->ResetMethodCallCountsL();
	aDb.DeleteContactL(iccId);
	aDb.DeleteContactL(nonIccId);
	test(syncChecker->ValidateMethodCallCountL() == 2);
	}

//===============================================================================================
// EDITING entries
//===============================================================================================
/**
 * Verify that edit validation is being done for ICC entries
 * and write validation is done when Commit is called.
 * @param aDb Contact database
 * @param aId Item Id to edit
 */
void TestSuccessfulIccEditL(CContactDatabase& aDb, TContactItemId aId)
	{
	syncChecker->SetValidateResponseL(MContactSynchroniser::ERead,KErrNone);
	syncChecker->SetValidateResponseL(MContactSynchroniser::EEdit, KErrNone);
	syncChecker->SetValidateWriteResponseL(KErrNone);
	syncChecker->ResetMethodCallCountsL();
	test(syncChecker->ValidateMethodCallCountL() == 0);
	CContactItem* item = aDb.OpenContactLX(aId);
	CleanupStack::PushL(item);
	test(syncChecker->ValidateMethodCallCountL() == 2);	

	CContactItemFieldSet& fieldset = item->CardFields();
	const TInt pos = fieldset.Find(KUidContactFieldFamilyName);	
	CContactItemField& field = fieldset[pos];
	CContactTextField* textfield = field.TextStorage();
	textfield->SetTextL(KGivenEditName);

	aDb.CommitContactL(*item);
	test(syncChecker->ValidateMethodCallCountL() == 4);	
	CleanupStack::PopAndDestroy(2); //item, lock record
	}

/**
 * Verify that edit of non-icc entries is successful when the icc is
 * locked, and that the synchroniser is not called
 * @param aDb Contact database
 * @param aId Item Id to edit
 */
void TestSuccessfulNonIccEditL(CContactDatabase& aDb, TContactItemId aId)
	{
	syncChecker->SetValidateResponseL(MContactSynchroniser::ERead,KErrLocked);
	syncChecker->SetValidateResponseL(MContactSynchroniser::EEdit, KErrLocked);
	syncChecker->SetValidateWriteResponseL(KErrLocked);
	syncChecker->ResetMethodCallCountsL();
	test(syncChecker->ValidateMethodCallCountL() == 0);
	CContactItem* item = aDb.OpenContactLX(aId);
	CleanupStack::PushL(item);
	test(syncChecker->ValidateMethodCallCountL() == 0);	

	CContactItemFieldSet& fieldset = item->CardFields();
	const TInt pos = fieldset.Find(KUidContactFieldFamilyName);	
	CContactItemField& field = fieldset[pos];
	CContactTextField* textfield = field.TextStorage();
	textfield->SetTextL(KGivenEditName);

	aDb.CommitContactL(*item);
	test(syncChecker->ValidateMethodCallCountL() == 0);	
	CleanupStack::PopAndDestroy(2); //item, lock record
	}

/**
 * Unsuccessful edit. Fail the edit validation request
 * @param aDb Contact database
 * @param aId Item Id to edit
 */
void FailEditValidationL(CContactDatabase& aDb, TContactItemId aId)
	{
	syncChecker->SetValidateResponseL(MContactSynchroniser::ERead,KErrNone);
	syncChecker->SetValidateResponseL(MContactSynchroniser::EEdit, KErrAccessDenied);
	syncChecker->ResetMethodCallCountsL();
	test(syncChecker->ValidateMethodCallCountL() == 0);
	TRAPD(err, aDb.OpenContactL(aId));
	test(syncChecker->ValidateMethodCallCountL() == 2);	
	test(err==KErrAccessDenied);
	}

/**
 * Unsuccessful edit. Fail the write validation request
 * @param aDb Contact database
 * @param aId Item Id to edit
 */
void FailWriteValidationL(CContactDatabase& aDb, TContactItemId aId)
	{
	syncChecker->SetValidateResponseL(MContactSynchroniser::ERead,KErrNone);
	syncChecker->SetValidateResponseL(MContactSynchroniser::EEdit, KErrNone);
	syncChecker->SetValidateWriteResponseL(KErrAccessDenied);
	syncChecker->ResetMethodCallCountsL();

	CContactItem* item = aDb.OpenContactLX(aId);
	CleanupStack::PushL(item);
	CContactItemFieldSet& fieldset = item->CardFields();
	const TInt pos = fieldset.Find(KUidContactFieldFamilyName);	
	CContactItemField& field = fieldset[pos];
	CContactTextField* textfield = field.TextStorage();
	textfield->SetTextL(KGivenEditName);

	TRAPD(err, aDb.CommitContactL(*item));
	test(syncChecker->ValidateMethodCallCountL() == 4);	
	test(err==KErrAccessDenied);
	CleanupStack::PopAndDestroy(2); //item, lock record
	}

/**
 * Verify that edit validation is being done for ICC entries
 * and write validation is done when Commit is called.
 * @param aDb Contact database
 */
void TestEditL(CContactDatabase& aDb)
	{
	syncChecker->SetValidateResponseL(MContactSynchroniser::ERead,KErrNone);
	TContactItemId iccId = AddIccEntryL(aDb,KUidIccGlobalAdnPhonebook);
	TContactItemId nonIccId = AddContactCardL(aDb);
	test.Next(_L("Test successful edit"));
	TestSuccessfulIccEditL(aDb,iccId);
	TestSuccessfulNonIccEditL(aDb,nonIccId);
	test.Next(_L("Test unsuccessful edit - fail edit"));
	FailEditValidationL(aDb,iccId);
	test.Next(_L("Test unsuccessful edit - fail write"));
	FailWriteValidationL(aDb,iccId);
	}
//===============================================================================================
// SEARCHING entries
//===============================================================================================
void TestFindLC(CContactDatabase& aDb, TContactItemId aIccId, TContactItemId aNonIccId)
	{
	test.Next(_L("Test FindLC"));
	// Successful find of icc entry
	CContactItemFieldDef* fieldDef=new(ELeave) CContactItemFieldDef;
	CleanupStack::PushL(fieldDef);
	fieldDef->AppendL(KUidContactFieldFamilyName);
	syncChecker->ResetMethodCallCountsL();
	CContactIdArray* array = aDb.FindLC(KIccName,fieldDef);
	test(syncChecker->ValidateMethodCallCountL() == 1);
	test(array!=NULL);
	test(array->Count() == 1);
	test((*array)[0]==aIccId);
	CleanupStack::PopAndDestroy(array);
	array=NULL;

	test.Next(_L("Test searching when ICC locked"));
	// Unsuccessful find of icc entry because icc locked
	syncChecker->SetValidateResponseL(MContactSynchroniser::ESearch,KErrLocked);
	syncChecker->ResetMethodCallCountsL();
	TRAPD(err,array = aDb.FindLC(KIccName,fieldDef));
	test(syncChecker->ValidateMethodCallCountL() == 1);
	test(err==KErrLocked);
	test(array==NULL);

	// successful find of non-icc entry, even though icc locked
	syncChecker->ResetMethodCallCountsL();
	array = aDb.FindLC(KNonIccName,fieldDef);
	test(syncChecker->ValidateMethodCallCountL() == 0);
	test(array!=NULL);
	test(array->Count() == 1);
	test((*array)[0]==aNonIccId);
	CleanupStack::PopAndDestroy(2,fieldDef); // array, fieldDef
	}

TInt findWordSplitterL(TAny *aParams)
	{
	SFindInTextDefWordParser *parser=(SFindInTextDefWordParser *)aParams;
	parser->iWordArray->AppendL(*parser->iSearchString);
	return(KErrNone);
	}

void TestFindInTextDefLC(CContactDatabase& aDb, TContactItemId aIccId, TContactItemId aNonIccId)
	{
	test.Next(_L("Test FindInTextDefLC"));
	// Successful find of icc entry
	syncChecker->SetValidateResponseL(MContactSynchroniser::ESearch,KErrNone);
	syncChecker->SetValidateResponseL(MContactSynchroniser::ERead,KErrNone);

	TCallBack callBack(findWordSplitterL);
	CContactTextDef* textDef=CContactTextDef::NewLC();
	textDef->AppendL(TContactTextDefItem(KUidContactFieldFamilyName));
	CDesCArray* desArray = new(ELeave) CDesCArrayFlat(5);
	desArray->AppendL(_L("icc"));
	syncChecker->ResetMethodCallCountsL();
	CContactIdArray* array = aDb.FindInTextDefLC(*desArray,textDef,callBack);
	//test(syncChecker->ValidateMethodCallCountL() == 3);
	test(array!=NULL);
	test(array->Count() == 1);
	test((*array)[0]==aIccId);
	CleanupStack::PopAndDestroy(array);
	array=NULL;

	test.Next(_L("Test searching when ICC locked"));
	// Unsuccessful find of icc entry because icc locked
	syncChecker->SetValidateResponseL(MContactSynchroniser::ESearch,KErrLocked);
	syncChecker->ResetMethodCallCountsL();
	TRAPD(err,array = aDb.FindInTextDefLC(*desArray,textDef,callBack));
	test(syncChecker->ValidateMethodCallCountL() == 1);
	test(err==KErrLocked);
	test(array==NULL);
	desArray->Delete(0);
	delete desArray;

	// successful find of non-icc entry, even though icc locked
	CDesCArray* desArray2 = new(ELeave) CDesCArrayFlat(5);
	desArray2->AppendL(_L("non-icc"));
	syncChecker->ResetMethodCallCountsL();
	array = aDb.FindInTextDefLC(*desArray2,textDef,callBack);
	test(syncChecker->ValidateMethodCallCountL() == 0);
	test(array!=NULL);
	test(array->Count() == 1);
	test((*array)[0]==aNonIccId);

	// both the icc and non-icc entry should match the search criteria, but only the
	// non-icc entry should be returned since icc is locked
	SetNameField(aDb,aIccId,KNonIccName);
	syncChecker->ResetMethodCallCountsL();
	TRAP(err,array = aDb.FindInTextDefLC(*desArray2,textDef,callBack));
	test(syncChecker->ValidateMethodCallCountL() == 1);
	test(err==KErrLocked);
	test(array!=NULL);
	test(array->Count() == 1);
	test((*array)[0]==aNonIccId);
	desArray2->Delete(0);
	delete desArray2;
	SetNameField(aDb,aIccId,KIccName);
	CleanupStack::PopAndDestroy(2,textDef); // array, textDef
	}

void TestMatchPhoneNumberL(CContactDatabase& aDb, TContactItemId aIccId, TContactItemId aNonIccId)
	{
	// The synchroniser is not called to validate the IDs at for this method because 
	// phone number resolution needs to be as quick as possible. 
	// Instead, validation is done when the client attempts to read the items.
	test.Next(_L("Test MatchPhoneNumberL"));
	// Successful find of icc entry phone number
	syncChecker->ResetMethodCallCountsL();
	CContactIdArray* array = aDb.MatchPhoneNumberL(KIccNumber,KMaxPhoneMatchLength);
	test(syncChecker->ValidateMethodCallCountL() == 0);
	test(array!=NULL);
	test(array->Count() == 1);
	test((*array)[0]==aIccId);
	delete array;
	array=NULL;

	test.Next(_L("Test searching when ICC locked"));
	// successful find when icc locked - validation done when entry read
	syncChecker->SetValidateResponseL(MContactSynchroniser::ESearch,KErrLocked);
	syncChecker->ResetMethodCallCountsL();
	TRAPD(err,array = aDb.MatchPhoneNumberL(KIccNumber,KMaxPhoneMatchLength));
	test(syncChecker->ValidateMethodCallCountL() == 0);
	test(array!=NULL);
	test(err==KErrNone);
	test((*array)[0]==aIccId);
	delete array;
	array=NULL;
	syncChecker->ResetMethodCallCountsL();
	array = aDb.MatchPhoneNumberL(KNonIccNumber,KMaxPhoneMatchLength);
	test(syncChecker->ValidateMethodCallCountL() == 0);
	test(array!=NULL);
	test(array->Count() == 1);
	test((*array)[0]==aNonIccId);
	delete array;
	array=NULL;
	}

void TestSearchL(CContactDatabase& aDb)
	{
	test.Next(_L("Test searching"));
	syncChecker->SetValidateResponseL(MContactSynchroniser::ERead,KErrNone);
	syncChecker->SetValidateResponseL(MContactSynchroniser::ESearch,KErrNone);
	TContactItemId iccId = AddIccEntryL(aDb,KUidIccGlobalAdnPhonebook);
	SetNameField(aDb,iccId,KIccName);
	SetPhoneNumberField(aDb,iccId,KIccNumber);
	TContactItemId nonIccId = AddContactCardL(aDb);
	SetNameField(aDb,nonIccId,KNonIccName);
	SetPhoneNumberField(aDb,nonIccId,KNonIccNumber);

	TestFindLC(aDb,iccId,nonIccId);
	TestFindInTextDefLC(aDb,iccId,nonIccId);
	TestMatchPhoneNumberL(aDb,iccId,nonIccId);
	}


void CheckForPhbkSyncPluginL()
	{
	test.Next(_L("Check for PhbkSync test plug-in"));

	RImplInfoPtrArray	implInfoArray;
	CleanupResetAndDestroyPushL(implInfoArray);
	REComSession::ListImplementationsL(KUidEcomCntPhBkSyncInterface, implInfoArray);
	//Find implementations of KUidEcomCntPhBkSyncInterface
	TInt availCount = implInfoArray.Count(); 
	TInt count;
	for(count = 0; count < availCount; count++)
		{
		const TUid firstImplementationFound = implInfoArray[count]->ImplementationUid();
		CImplementationInformation *info = implInfoArray[count];
		test.Printf(_L("\n"));
		test.Printf(_L("PhbkSync plugin #%i, Implementation UID 0x%08X version %i\n"),
			count + 1, info->ImplementationUid(), info->Version());
		test.Printf(_L("Plugin name = \"%S\"\n"), &(info->DisplayName()));
		}

	// is telephony's plug-in in the list?
	for(count = 0; count < availCount; count++)
		{
		const TUid firstImplementationFound = implInfoArray[count]->ImplementationUid();
		CImplementationInformation *info = implInfoArray[count];
		if(info->DisplayName() == KTestPluginName)
			{
			test.Printf(_L("\n"));
			test.Printf(_L("This test has now loaded the test plugin"));
			test.Printf(_L("\n"));
			availCount = 1;
			break;
			}		
		
		if(info->DisplayName() == KPluginName)
			{
			test.Printf(_L("\n"));
			test.Printf(_L("This test only works with Contacts the test plugin and not the original phonebooksync plugin."));
			test.Printf(_L("Depending on the build to removed the plugin in different ways:"));
			test.Printf(_L("hardware - delete the line \"ECOM_PLUGIN(phbksyncplugin.dll,1020428C.rsc)\" from phbksync.iby"));
			test.Printf(_L("winscw - delete phbksyncplugin.dll from %epocroot%/epoc32/release/winscw/udeb or similarly named directory"));
			test.Printf(_L("\n"));
			test(0);  // stop
			break;
			}
		}

	// only continue test if there is exactly one plug-in present
	test(availCount == 1);	

	CleanupStack::PopAndDestroy(&implInfoArray);
	}

	
//===============================================================================================

/** Lists tests to execute */

/**

@SYMTestCaseID     PIM-T-ICCMULTIPLEPHONEBOOK-0001

*/
void DoTestsL()
    {
	test.Start(_L("@SYMTestCaseID:PIM-T-ICCMULTIPLEPHONEBOOK-0001 CContactICCEntry tests with multiple phonebooks"));

	CheckForPhbkSyncPluginL();

	CntTest->CreateDatabaseL();	
	CntTest->OpenDatabaseL();
	CContactDatabase& db= *CntTest->Db();

	syncChecker = CContactSyncChecker::NewL();
	syncChecker->ResetEverythingL();

	SetUpIccPhonebooksL(db);
	VerifyPhonebookTemplateIdsL(db);
	AddEntriesBasedOnEachOfTheTemplatesL(db);
	TestUnsuccessfulAddL(db);
	TestDeleteL(db);
	TestReadL(db);
	TestEditL(db);
	TestSearchL(db);
	syncChecker->ResetMethodCallCountsL();

	CntTest->CloseDatabase();
	CntTest->DeleteDatabaseL();


	delete syncChecker;
	syncChecker = NULL;
    }


/** Standard E32Main method */
GLDEF_C TInt E32Main()
	{
    CntTest=new(ELeave) CCntTest;
	CntTest->ConstructL(test,KDatabaseFileName);
    TRAPD(err,DoTestsL());
	if (syncChecker)
		{
		delete syncChecker;
		syncChecker = NULL;
		}
	CntTest->EndTestLib(err);
	return KErrNone;
    }

