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

#include "t_utils.h"
#include "./testsyncplugin/cnttestsynchroniser.h"

// include templates for CleanupResetAndDestroyPushL(T)
#include "cntviewprivate.h"


/** 
 * @file
 * Test harness for CContactICCEntry and MContactSynchroniser interface.
 * todo Find out why telephone field won't match template correctly 
 *       (Maybe the ICC slot field needs to be added to the "golden" template)
 *
 */

_LIT(KDatabaseFileName,"C:T_ICCENTRY");

_LIT(KTestName,"Basic tests for CContactICCEntry");
_LIT(KGivenName,"Given");
_LIT(KGivenEditName,"EditedName");
_LIT(KTelephoneNum,"020 11111111");
_LIT(KTelephoneNumModified,"020 22222222");
_LIT(KTemplateName,"SIM Card Contacts"); 

_LIT(KPluginName,"phone book synchronizer Implementation");
_LIT(KTestPluginName,"Test phone book synchronizer Implementation");

CCntTest* CntTest=NULL;
LOCAL_D RTest test(KTestName);

LOCAL_D CContactTestSynchroniser* synchroniser;
LOCAL_D CContactSyncChecker* syncChecker;

LOCAL_C TInt findWordSplitterL(TAny *aParams)
	{
	SFindInTextDefWordParser *parser=(SFindInTextDefWordParser *)aParams;
	parser->iWordArray->AppendL(*parser->iSearchString);
	return(KErrNone);
	}

CContactIdArray* TestFindL(CContactDatabase& aDatabase, const TDesC& aText, const CContactItemFieldDef *aFieldDef, RTest& aTest)
	{
	CContactIdArray* retval = NULL;
	
	retval = aDatabase.FindLC(aText, aFieldDef);
	aTest(retval == NULL);
	CleanupStack::Pop( retval );
	
	return retval;
	}
	
CContactIdArray* TestFindInTextDefL(CContactDatabase& aDatabase, const MDesCArray& aFindWords,CContactTextDef* aTextDef, const TCallBack &aWordParserCallback, RTest& aTest)
	{
	CContactIdArray* retval = NULL;
	retval = aDatabase.FindInTextDefLC(aFindWords, aTextDef, aWordParserCallback);
	aTest(retval==NULL);
	CleanupStack::Pop( retval );
	return retval;
	}
		
/** 
 * Test Finding ICC Contacts when the ICC is locked.
 * @param aDb Contact database
 */

void TestFindingL(CContactDatabase& aDb)
	{
	test.Next(_L("Test FindLC"));

	syncChecker->SetValidateResponseL(MContactSynchroniser::ERead,KErrLocked);
	syncChecker->SetValidateResponseL(MContactSynchroniser::ESearch,KErrLocked);
	
	CContactItemFieldDef* fieldDef=new(ELeave) CContactItemFieldDef;
	CleanupStack::PushL(fieldDef);
	fieldDef->AppendL(KUidContactFieldFamilyName); 
	
	
	TRAPD(err,TestFindL(aDb, KGivenEditName, fieldDef, test) );
	test(err==KErrLocked);
	CleanupStack::PopAndDestroy(fieldDef);

	CContactItemFieldDef* fieldDef2=new(ELeave) CContactItemFieldDef;
	CleanupStack::PushL(fieldDef2);
	fieldDef2->AppendL(KUidContactFieldPhoneNumber); 
	TRAP(err, TestFindL(aDb, KTelephoneNum, fieldDef2, test) );
	test(err==KErrLocked);
	CleanupStack::PopAndDestroy(fieldDef2);

	test.Next(_L("Test FindInTextDefL"));

	TCallBack callBack(findWordSplitterL);
	CContactTextDef* textDef=CContactTextDef::NewLC();
	textDef->AppendL(TContactTextDefItem(KUidContactFieldFamilyName));
	CDesCArray* desArray = new(ELeave) CDesCArrayFlat(5);
	desArray->AppendL(_L("e"));
	
	CContactIdArray* arr = NULL;
	TRAP(err,arr = TestFindInTextDefL(aDb,*desArray,textDef, callBack, test) );
	
//	test(err==KErrLocked);
	desArray->Delete(0);
	delete desArray;
//FIX
	delete arr;	
	CleanupStack::PopAndDestroy(textDef); 

	test.Next(_L("Test FindInTextDefL2"));

	TCallBack callBack2(findWordSplitterL);
	CContactTextDef* textDef2=CContactTextDef::NewLC();
	textDef2->AppendL(TContactTextDefItem(KUidContactFieldFamilyName));
	textDef2->AppendL(TContactTextDefItem(KUidContactFieldUrl));
	CDesCArray* desArray2 = new(ELeave) CDesCArrayFlat(5);
	desArray2->AppendL(_L("e"));
	
	TRAP(err,arr = TestFindInTextDefL(aDb,*desArray2,textDef2, callBack2, test) );
//	test(err==KErrLocked);
	desArray2->Delete(0);
	delete desArray2;
//FIX
	delete arr;	
	CleanupStack::PopAndDestroy(textDef2);
	}


/** 
 * Test ICC contacts can be matched using MatchPhoneNumberL API. The 
 * synchroniser is not called to validate the IDs at for this method because 
 * phone number resolution needs to be as quick as possible. 
 * Instead, validation is done when the client attempts to read the items.
 * @param aDb Contact database
 */
 

void TestPhoneMatchingL(CContactDatabase& aDb)
	{
	test.Next(_L("Test phone match"));

	CContactIdArray* array = aDb.MatchPhoneNumberL(KTelephoneNum,KMaxPhoneMatchLength);	
	test(array->Count() == 4);
	delete array;

	syncChecker->SetValidateResponseL(MContactSynchroniser::ERead,KErrNone);
	syncChecker->SetValidateResponseL(MContactSynchroniser::EEdit, KErrNone);
	syncChecker->SetValidateWriteResponseL(KErrNone);
	aDb.SetDbViewContactType(KUidContactICCEntry);
	TContactItemId id = (*aDb.SortedItemsL())[0];
	CContactItem* item = aDb.OpenContactLX(id);
	CleanupStack::PushL(item);
	CContactItemFieldSet& fieldset = item->CardFields();
	const TInt pos = fieldset.Find(KUidContactFieldPhoneNumber);	
	CContactItemField& field = fieldset[pos];
	CContactTextField* textfield = field.TextStorage();
	textfield->SetTextL(KTelephoneNumModified);
	aDb.CommitContactL(*item);
	CleanupStack::PopAndDestroy(2); //item, lock record

	CContactIdArray* array2 = aDb.MatchPhoneNumberL(KTelephoneNumModified,KMaxPhoneMatchLength);	
	test(array2->Count() == 1);
	delete array2;
	}


/** 
 * Test database iteration using TContactIter.
 * @param aDb Contact database
 */
 

void TestDatabaseIterationL(CContactDatabase& aDb)
	{
	test.Next(_L("Test database iteration"));

	aDb.SetDbViewContactType(KUidContactICCEntry);
	TContactIter iterator(aDb);
	TContactItemId id = iterator.FirstL();
	while (id!=KNullContactId)
		{
		CContactItem* item = aDb.ReadContactLC(id);
		test(item->Type() == KUidContactICCEntry);
		CleanupStack::PopAndDestroy(item);
		id = iterator.NextL();
		}
	}



CContactItem* ReadItemL( CContactDatabase& aDatabase, TContactItemId aId )
	{
	CContactItem* retval = NULL;
	retval = aDatabase.ReadMinimalContactLC( aId );
	CleanupStack::Pop( retval );
	return retval;
	}

/** 
 * Verify that the plug-in implementation is called when the 
 * ReadMinimalContactL API is used. 
 * 
 * Tests methods:
 * IMPORT_C CContactItem* ReadMinimalContactL(TContactItemId aContactId);
 * IMPORT_C CContactItem* ReadMinimalContactLC(TContactItemId aContactId);
 * 
 * @param aDb Contact database
 * @param aId Contact item ID to read
 */
void TestReadMinimalContactL(CContactDatabase& aDb, TContactItemId aId)
	{
	syncChecker->SetValidateResponseL(MContactSynchroniser::ERead,KErrNone);
	syncChecker->ResetMethodCallCountsL();
	test(syncChecker->ValidateMethodCallCountL() == 0);	
	CContactItem* item = aDb.ReadMinimalContactLC(aId);
	test(syncChecker->ValidateMethodCallCountL() == 1);	
	CleanupStack::PopAndDestroy(item);

	syncChecker->SetValidateResponseL(MContactSynchroniser::ERead,KErrAccessDenied);
	syncChecker->ResetMethodCallCountsL();
	test(syncChecker->ValidateMethodCallCountL() == 0);
	item=NULL;
	//TRAPD(err, item = aDb.ReadMinimalContactLC(aId));
	TRAPD( err, item = ReadItemL( aDb, aId ) );
	delete item;
	test(err==KErrAccessDenied);
	test(syncChecker->ValidateMethodCallCountL() == 1);	
	}

/** 
 * Verify that the plug-in implementation is called when the 
 * ReadContactL API is used. 
 * 
 * Tests code called by methods:
 * IMPORT_C CContactItem* ReadContactL(TContactItemId aContactId);
 * IMPORT_C CArrayPtr<CContactItem>* ReadContactAndAgentL(TContactItemId aContactId);
 * IMPORT_C CContactItem* ReadContactL(TContactItemId aContactId,const CContactItemViewDef& aViewDef);
 * IMPORT_C CContactItem* ReadContactLC(TContactItemId aContactId);
 * IMPORT_C CContactItem* ReadContactLC(TContactItemId aContactId,const CContactItemViewDef& aViewDef);
 * 
 * @param aDb Contact database
 * @param aId Contact item ID to read
 */
void TestReadContactL(CContactDatabase& aDb, TContactItemId aId)
	{
	syncChecker->SetValidateResponseL(MContactSynchroniser::ERead,KErrNone);
	syncChecker->ResetMethodCallCountsL();
	test(syncChecker->ValidateMethodCallCountL() == 0);	
	CContactItem* item = aDb.ReadContactLC(aId);
	test(syncChecker->ValidateMethodCallCountL() == 1);	
	CleanupStack::PopAndDestroy(item);

	syncChecker->SetValidateResponseL(MContactSynchroniser::ERead,KErrAccessDenied);
	syncChecker->ResetMethodCallCountsL();
	test(syncChecker->ValidateMethodCallCountL() == 0);
	item=NULL;
	TRAPD(err, item = aDb.ReadContactL(aId));
	delete item;
	test(err==KErrAccessDenied);
	test(syncChecker->ValidateMethodCallCountL() == 1);	
	}

/** 
 * Verify that read validation is done for ReadContactTextDefL
 * 
 * @param aDb Contact database
 * @param aId Contact item ID to read
 */
void TestReadTextDefinitionL(CContactDatabase& aDb, TContactItemId aId)
	{
	syncChecker->SetValidateResponseL(MContactSynchroniser::ERead,KErrNone);
	syncChecker->ResetMethodCallCountsL();
	test(syncChecker->ValidateMethodCallCountL() == 0);
	TBuf<64> textDef;
	aDb.ReadContactTextDefL(aId,textDef);
	test(syncChecker->ValidateMethodCallCountL() == 1);	

	syncChecker->SetValidateResponseL(MContactSynchroniser::ERead,KErrAccessDenied);
	syncChecker->ResetMethodCallCountsL();
	test(syncChecker->ValidateMethodCallCountL() == 0);
	TRAPD(err, aDb.ReadContactTextDefL(aId,textDef));
	test(err==KErrAccessDenied);
	test(syncChecker->ValidateMethodCallCountL() == 1);	
	}

/** 
 * Create example ICC Entry 
 * @param aDb Contact database
 */
TContactItemId CreateTestICCEntryL(CContactDatabase& aDb)
	{
	TContactItemId templateId = aDb.ICCTemplateIdL(KUidIccGlobalAdnPhonebook);
	CContactItem* iccTemplate = aDb.ReadContactLC(templateId);
	CContactICCEntry* entry = CContactICCEntry::NewL(*iccTemplate);
	CleanupStack::PopAndDestroy(iccTemplate);
	CleanupStack::PushL(entry);
	SetNameL(*entry,KUidContactFieldFamilyName,KUidContactFieldVCardMapUnusedN,KGivenName,EFalse);
	SetNameL(*entry,KUidContactFieldPhoneNumber,KUidContactFieldVCardMapTEL,KTelephoneNum,EFalse);

	//Add to database
	syncChecker->SetValidateWriteResponseL(KErrNone);
	syncChecker->SetValidateResponseL(MContactSynchroniser::ERead,KErrNone);
	syncChecker->SetDeleteContactResponseL(KErrNone);

	TContactItemId id = aDb.AddNewContactL(*entry); 

	CleanupStack::PopAndDestroy(entry);	
	test(id!=KNullContactId);
	return id;
	}

/**
 * Verify Read validation is being done for reading items.
 * @param aDb Contact database
 */

void TestReadL(CContactDatabase& aDb)
	{
	TContactItemId id = CreateTestICCEntryL(aDb);
	test.Next(_L("TestReadMinimalContactL"));

	TestReadMinimalContactL(aDb,id);
	test.Next(_L("TestReadContactL"));

	TestReadContactL(aDb,id);
	test.Next(_L("TestReadTextDefinitionL"));

	TestReadTextDefinitionL(aDb,id);
	}

/**
 * Verify that edit validation is being done for ICC entries
 * and write validation is done when Commit is called.
 * @param aDb Contact database
 * @param aId Item Id to edit
 */
void TestSuccessfulEditL(CContactDatabase& aDb, TContactItemId aId)
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
	TContactItemId id = CreateTestICCEntryL(aDb);
	test.Next(_L("Test successful edit"));

	TestSuccessfulEditL(aDb,id);
	test.Next(_L("Test unsuccessful edit - fail edit"));

	FailEditValidationL(aDb,id);
	test.Next(_L("Test unsuccessful edit - fail write"));

	FailWriteValidationL(aDb,id);
	}

/** 
 * Test an unsuccessful delete
 * @param aDb Contact database
 */
void TestUnsuccessfulDeleteL(CContactDatabase& aDb)
	{
	syncChecker->SetValidateResponseL(MContactSynchroniser::ERead,KErrNone);
	syncChecker->SetValidateResponseL(MContactSynchroniser::EEdit, KErrNone);
	TContactItemId id = CreateTestICCEntryL(aDb);
	// Creating the contact sets the delete error to KErrNone, so must do this afterwards
	syncChecker->SetDeleteContactResponseL(KErrCorrupt);
	syncChecker->ResetMethodCallCountsL();
	TRAPD(err, aDb.DeleteContactL(id));
	test(syncChecker->ValidateMethodCallCountL() == 2);
	test(err==KErrCorrupt);
	//Read contact to check it wasn't deleted anyway
	CContactItem* item = NULL;
	TRAP(err,item = aDb.ReadContactL(id));
	test(err==KErrNone);
	delete item;
	}


/** 
 * Verify that delete validation is being done for ICC entries.
 * @param aDb Contact database
 */

void TestDeleteL(CContactDatabase& aDb)
	{
	test.Next(_L("Test successful delete"));

	TContactItemId id = CreateTestICCEntryL(aDb);
	syncChecker->ResetMethodCallCountsL();
	aDb.DeleteContactL(id);
	test(syncChecker->ValidateMethodCallCountL() == 2);
	test.Next(_L("Test unsuccessful delete"));

	TestUnsuccessfulDeleteL(aDb);
	}


/** 
 * Create template card for ICC contact items 
 * @param aDb Contact database
 */
TContactItemId CreateICCTemplateL(CContactDatabase& aDb)
	{
	CContactItem* temp = aDb.CreateContactCardTemplateLC(KTemplateName);
	TContactItemId templateId = temp->Id();
	CleanupStack::PopAndDestroy(temp);
	temp=NULL;

	//Remove all the unnecessary fields
	temp = aDb.OpenContactLX(templateId);
	CleanupStack::PushL(temp);	
	const TInt fieldCount = temp->CardFields().Count();
	for(TInt i=fieldCount-1;i>=0;i--)
		temp->RemoveField(i);
		
	CContactItemField* name = CContactItemField::NewLC(KStorageTypeText,KUidContactFieldFamilyName);
	name->SetMapping(KUidContactFieldVCardMapUnusedN);
	temp->AddFieldL(*name);
	CleanupStack::Pop(name);

	CContactItemField* number = CContactItemField::NewLC(KStorageTypeText,KUidContactFieldPhoneNumber);
	number->SetMapping(KUidContactFieldVCardMapTEL);
	temp->AddFieldL(*number);
	CleanupStack::Pop(number);
				
	CContactItemField* slotnum = CContactItemField::NewLC(KStorageTypeText,KUidContactFieldICCSlot);
	temp->AddFieldL(*slotnum);
	CleanupStack::Pop(slotnum);

	CContactItemField* phonebook = CContactItemField::NewLC(KStorageTypeText,KUidContactFieldICCPhonebook);
	temp->AddFieldL(*phonebook);
	CleanupStack::Pop(phonebook);

	aDb.CommitContactL(*temp);
	CleanupStack::PopAndDestroy(2);	// temp, close template	
	
	return templateId;
	}

/**
 * Test SetDbViewContactType can be used for KUidContactICCEntry items.
 * @param aDb Contact database
 */
void TestDatabaseViewL(CContactDatabase& aDb)
	{
	syncChecker->SetValidateResponseL(MContactSynchroniser::ERead,KErrNone);
	syncChecker->SetValidateWriteResponseL(KErrNone);
	CntTest->CloseDatabase();
	CntTest->DeleteDatabaseL();
	// re-initialise the TLS
	syncChecker->EnableVerboseLoggingL();

	CntTest->CreateDatabaseL();	
	CntTest->OpenDatabaseL();
	delete synchroniser;
	synchroniser = CContactTestSynchroniser::NewL();
	// test with GSM ADN phonebook only
	//test(KErrNone == syncChecker->SetGsmPhonebook());

	// Start from clean
	CContactItem* card = CContactCard::NewLC();
	aDb.AddNewContactL(*card);
	aDb.CreateContactGroupLC();
	aDb.CreateOwnCardLC();
	TContactItemId id=CreateICCTemplateL(aDb);
	test(KErrNone == syncChecker->UpdatePhonebookEntryL(KUidIccGlobalAdnPhonebook, id, KNullContactId));
	TContactItemId templateId = aDb.ICCTemplateIdL(KUidIccGlobalAdnPhonebook);
	CContactItem* iccTemplate = aDb.ReadContactLC(templateId);
	CContactICCEntry* iccentry = CContactICCEntry::NewL(*iccTemplate);
	CleanupStack::PopAndDestroy(iccTemplate);
	CleanupStack::PushL(iccentry);
	aDb.AddNewContactL(*iccentry);
	TContactItemId iccId = iccentry->Id();
	CleanupStack::PopAndDestroy(4); //card,group,owncard,iccentry
	
	test(aDb.SortedItemsL()->Count() == 5); //default view includes ALL contact types
	CntTest->CloseDatabase();
	delete synchroniser;
	synchroniser = NULL;
	CntTest->OpenDatabaseL();
	aDb.SetDbViewContactType(KUidContactICCEntry);
	test(aDb.SortedItemsL()->Count() == 1);
	test((*aDb.SortedItemsL())[0]==iccId);
	}

/** 
 * Create phonebook group 
 * @param aDb Contact database
 */
TContactItemId CreatePhonebookGroupL(CContactDatabase& aDb)
	{
	_LIT(KPhbkGroup,"ADN Phonebook"); 
	CContactItem* group = aDb.CreateContactGroupLC(KPhbkGroup);
	TContactItemId groupId = group->Id();
	CleanupStack::PopAndDestroy(group);	
	return groupId;
	}

/** 
 * Verify that the template was correctly added 
 * @param aTemplate CContactCardTemplate template item
 */
void CheckTemplateL(CContactItem& aTemplate)
	{
	test(aTemplate.Type() == KUidContactCardTemplate);
	test(aTemplate.TemplateRefId() == KNullContactId);
	test(static_cast<CContactCardTemplate&>(aTemplate).GetTemplateLabelL() == KTemplateName);
	CContactItemFieldSet& fieldset = aTemplate.CardFields();
	TInt pos = fieldset.Find(KUidContactFieldTemplateLabel);
	test(pos!=KErrNotFound);
	TInt count = fieldset.Count();
	test(count==5); //label, name, number, ICC slot, phonebook
	}


/** 
 * Check that the field of type aFieldType has the same content as 
 * aExpectedContent
 * @param aFieldset Contact item fieldset
 * @param aFieldType Field type to test
 * @param aExpectedContent Expected data
 */
void CheckFieldContentL(CContactItemFieldSet& aFieldset, TFieldType aFieldType, const TDesC& aExpectedContent)
	{
	TInt pos = aFieldset.Find(aFieldType);
	CContactItemField& field = aFieldset[pos];
	CContactTextField* textField = field.TextStorage();
	test(textField != NULL);
	test(textField->Text() == aExpectedContent);
	}

/** 
 * Test creation of CContactICCEntry items.
 * 
 * Set-up appropriate template and group ready...
 * @param aDb Contact database
 */

/**

@SYMTestCaseID     PIM-T-ICCENTRY-0001

*/

void TestCreationL(CContactDatabase& aDb)
	{
	_LIT(KTestCreation,"Create CContactICCEntry item");
	test.Start(_L("@SYMTESTCaseID:PIM-T-ICCENTRY-0001 Create CContactICCEntry item"));

	//System Template
	TContactItemId systemTemplateId = aDb.TemplateId();
	CContactItem* systemTemplate = aDb.ReadContactLC(systemTemplateId);
	CContactICCEntry* entry = CContactICCEntry::NewL(*systemTemplate);
	CleanupStack::PopAndDestroy(systemTemplate);
	CleanupStack::PushL(entry);
	
	//Test CContactICCEntry items can be identified from normal contact cards
	test(entry->Type() == KUidContactICCEntry);
	test(entry->TemplateRefId() == systemTemplateId);
	CleanupStack::PopAndDestroy(entry);

	//Create user defined template for ICC contacts
	test.Next(_L("CContactDatabase::ICCTemplateL()"));

	TContactItemId templateId = CreateICCTemplateL(aDb);
	test(KErrNone == syncChecker->UpdatePhonebookEntryL(KUidIccGlobalAdnPhonebook, templateId, KNullContactId));

	//Check that contacts model calls the plug-in when asked for the
	//template ID.
	TInt callCount = syncChecker->ValidateMethodCallCountL();
	test(callCount==0);
	TContactItemId templateIdFromCntmodel;
	templateIdFromCntmodel = aDb.ICCTemplateIdL(KUidIccGlobalAdnPhonebook);

	test(syncChecker->ValidateMethodCallCountL() == 1);
	test(templateId == templateIdFromCntmodel);
	
	CContactItem* iccTemplate = aDb.ReadContactLC(templateId);
	CheckTemplateL(*iccTemplate);
	//create ICC contact
	CContactICCEntry* iccentry = CContactICCEntry::NewL(*iccTemplate);
	CleanupStack::PopAndDestroy(iccTemplate);
	CleanupStack::PushL(iccentry);
	test(iccentry->Type() == KUidContactICCEntry);
	test(iccentry->TemplateRefId() == templateId);
	CleanupStack::PopAndDestroy(iccentry);
	
	test.End();
	}



/** 
 * Test successful addition to database.
 * @param aDb Contact database
 */
void TestSuccessfulAddL(CContactDatabase& aDb)
	{
	//Create group 
	TContactItemId groupId = CreatePhonebookGroupL(aDb);
	test(KErrNone == syncChecker->UpdatePhonebookGroupIdL(KUidIccGlobalAdnPhonebook, groupId));
	//Create item and add fields
	TContactItemId templateId = aDb.ICCTemplateIdL(KUidIccGlobalAdnPhonebook);
	CContactItem* iccTemplate = aDb.ReadContactLC(templateId);
	CContactICCEntry* entry = CContactICCEntry::NewL(*iccTemplate);
	CleanupStack::PopAndDestroy(iccTemplate);
	CleanupStack::PushL(entry);
	SetNameL(*entry,KUidContactFieldFamilyName,KUidContactFieldVCardMapUnusedN,KGivenName,EFalse);
	SetNameL(*entry,KUidContactFieldPhoneNumber,KUidContactFieldVCardMapTEL,KTelephoneNum,EFalse);

	//Add to database
	syncChecker->ResetMethodCallCountsL();
	syncChecker->SetValidateWriteResponseL(KErrNone);
	syncChecker->SetValidateResponseL(MContactSynchroniser::ERead,KErrNone);

	test(syncChecker->ValidateMethodCallCountL() == 0);
	TContactItemId id = aDb.AddNewContactL(*entry); 
	CleanupStack::PopAndDestroy(entry);	
	test(syncChecker->ValidateMethodCallCountL() == 3);
	test(id!=KNullContactId);

	CContactICCEntry* fetchedItem = static_cast<CContactICCEntry*>(aDb.ReadContactL(id));
	CleanupStack::PushL(fetchedItem);
	//Check group membership	
	const CContactIdArray* owned = fetchedItem->GroupsJoined();
	test(owned!=NULL && owned->Count() == 1);
	test((*owned)[0]==groupId);
	
	//Check number of fields and content
	
	CContactItemFieldSet& fieldset = fetchedItem->CardFields();
	TInt pos = fieldset.Find(KUidContactFieldTemplateLabel);
	test(pos==KErrNotFound);
	
//	test(fieldset.Count() == 3); 
	
/*	for (TInt i=0; i<fieldset.Count(); i++)
		{
		CContactItemField& field = fieldset[i];
		TInt count = field.ContentType().FieldTypeCount();
		for (TInt j=0; j<count; j++)
			{
			TFieldType ft= field.ContentType().FieldType(j);
			}


		TUid mapping = field.ContentType().Mapping();
		}
*/	
	CheckFieldContentL(fieldset,KUidContactFieldFamilyName,KGivenName);
//	CheckFieldContentL(fieldset,KUidContactFieldPhoneNumber,KTelephoneNum);

	CleanupStack::PopAndDestroy(fetchedItem);
	}

/** 
 * Test an unsuccessful addition to the database
 * @param aDb Contact database
 */
void TestUnsuccessfulAddL(CContactDatabase& aDb)
	{
	TContactItemId templateId = aDb.ICCTemplateIdL(KUidIccGlobalAdnPhonebook);
	CContactItem* iccTemplate = aDb.ReadContactLC(templateId);
	CContactICCEntry* entry = CContactICCEntry::NewL(*iccTemplate);
	CleanupStack::PopAndDestroy(iccTemplate);
	CleanupStack::PushL(entry);
	SetNameL(*entry,KUidContactFieldFamilyName,KUidContactFieldVCardMapUnusedN,KGivenName,EFalse);
	SetNameL(*entry,KUidContactFieldPhoneNumber,KUidContactFieldVCardMapTEL,KTelephoneNum,EFalse);

	syncChecker->ResetMethodCallCountsL();
	syncChecker->SetValidateWriteResponseL(KErrAccessDenied);
	TRAPD(error, aDb.AddNewContactL(*entry)); 	
	test(error==KErrAccessDenied);
	test(syncChecker->ValidateMethodCallCountL() == 1);
	CleanupStack::PopAndDestroy(entry);
	}

/** 
 * Test addition to database without adding to a group
 * @param aDb Contact database
 */
void TestAddingWithoutGroupL(CContactDatabase& aDb)
	{
	TContactItemId templateId = aDb.ICCTemplateIdL(KUidIccGlobalAdnPhonebook);
	CContactItem* iccTemplate = aDb.ReadContactLC(templateId);
	CContactICCEntry* entry = CContactICCEntry::NewL(*iccTemplate);
	CleanupStack::PopAndDestroy(iccTemplate);
	CleanupStack::PushL(entry);
	SetNameL(*entry,KUidContactFieldFamilyName,KUidContactFieldVCardMapUnusedN,KGivenName,EFalse);
	SetNameL(*entry,KUidContactFieldPhoneNumber,KUidContactFieldVCardMapTEL,KTelephoneNum,EFalse);

	syncChecker->ResetMethodCallCountsL();
	syncChecker->SetValidateWriteResponseL(KErrNone);
	TInt oldGroupId = syncChecker->GroupIdL(KUidIccGlobalAdnPhonebook);
	test(KErrNone == syncChecker->UpdatePhonebookGroupIdL(KUidIccGlobalAdnPhonebook, KNullContactId));
	TContactItemId id = aDb.AddNewContactL(*entry); 	
	test(syncChecker->ValidateMethodCallCountL() == 3);
	CleanupStack::PopAndDestroy(entry);

	CContactICCEntry* fetchedItem = static_cast<CContactICCEntry*>(aDb.ReadContactL(id));
	CleanupStack::PushL(fetchedItem);
	//Check group membership	
	const CContactIdArray* owned = fetchedItem->GroupsJoined();
	test(owned==NULL || owned->Count() == 0);
	CleanupStack::PopAndDestroy(fetchedItem);
		
	test(KErrNone == syncChecker->UpdatePhonebookGroupIdL(KUidIccGlobalAdnPhonebook, oldGroupId));
	}

/** 
 * Verify that adding contact cards doesn't call the synchroniser interface
 * @param aDb Contact database
 */
void TestAddingContactCardL(CContactDatabase& aDb)
	{
	CContactCard* card = CContactCard::NewLC();
	syncChecker->ResetMethodCallCountsL();
	aDb.AddNewContactL(*card);
	test(syncChecker->ValidateMethodCallCountL() == 0);
	CleanupStack::PopAndDestroy(card);
	}

/** 
 * Test addition of CContactICCEntry items.
 * @param aDb Contact database
 */
void TestAddL(CContactDatabase& aDb)
	{
	TestCreationL(aDb);
	TestSuccessfulAddL(aDb);
	TestUnsuccessfulAddL(aDb);
	TestAddingWithoutGroupL(aDb);
	TestAddingContactCardL(aDb);
	}

/** 
 * Test PhonebookGroupIdL method.
 * @param aDb Contact database
 */
void TestPhonebookGroupIdL(CContactDatabase& aDb)
	{
	synchroniser = CContactTestSynchroniser::NewL();
	const TContactItemId expectedGroupId(KNullContactId);
	test(KErrNone == syncChecker->UpdatePhonebookGroupIdL(KUidIccGlobalAdnPhonebook, expectedGroupId));
	TContactItemId id = aDb.PhonebookGroupIdL();
	test(id == expectedGroupId);
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
		//const TUid firstImplementationFound = implInfoArray[count]->ImplementationUid();
		CImplementationInformation *info = implInfoArray[count];
		test.Printf(_L("\n"));
		test.Printf(_L("PhbkSync plugin #%i, Implementation UID 0x%08X version %i\n"),
			count + 1, info->ImplementationUid(), info->Version());
		test.Printf(_L("Plugin name = \"%S\"\n"), &(info->DisplayName()));
		}

	// is telephony's plug-in in the list?
	for(count = 0; count < availCount; count++)
		{
		//const TUid firstImplementationFound = implInfoArray[count]->ImplementationUid();
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

/** Lists tests to execute */

void DoTestsL()
    {
	test.Start(_L("Basic tests for CContactICCEntry"));


	CheckForPhbkSyncPluginL();

	CntTest->CreateDatabaseL();	
	CntTest->OpenDatabaseL();

	syncChecker = CContactSyncChecker::NewL();
	synchroniser = CContactTestSynchroniser::NewL();
	syncChecker->ResetL();
	syncChecker->EnableVerboseLoggingL();
	// test with GSM ADN phonebook only
	test(KErrNone == syncChecker->SetGsmPhonebookL());
	
	CContactDatabase& db= *CntTest->Db();
	TestAddL(db);
	TestReadL(db);
	TestEditL(db);
	TestPhoneMatchingL(db);
	TestFindingL(db);
	TestDeleteL(db);
	TestDatabaseIterationL(db);
	TestDatabaseViewL(db);
	TestPhonebookGroupIdL(db);
	
	CntTest->CloseDatabase();
	CntTest->DeleteDatabaseL();

	delete synchroniser;
	synchroniser = NULL;
	delete syncChecker;
	syncChecker = NULL;
    }

/** Standard E32Main method */
GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
    CntTest=new(ELeave) CCntTest;
	CntTest->ConstructL(test,KDatabaseFileName);
    TRAPD(err,DoTestsL());
	if (err)
		{
		// delete globals
		delete synchroniser;
		synchroniser = NULL;
		delete syncChecker;
		syncChecker = NULL;
		}
	CntTest->EndTestLib(err);
	__UHEAP_MARKEND;
	return KErrNone;
    }
