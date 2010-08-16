/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
@SYMPREQ                  PREQ1187
@SYMComponent             app-engines_cntmodel
@SYMAuthor                Simon Mellor, JamesCl
@SYMTestStatus            Implemented
@SYMTestType              CT

@SYMTestCaseDesc          Tests the performance of Reading and Updating a single contact and of
						  Matching Contacts by Phone Number on a corporate profile database of 
						  1000 contacts. 

@SYMTestActions           Measures the time taken to: 
						  -- Attempt to match a phone number using CContactDatabase::MatchPhoneNumberL()
						  -- Create a contact using CContactDatabase::AddNewContactL()
						  -- Read   a contact using CContactDatabase::OpenContactLX()
						  -- Update a contact using CContactDatabase::CommitContactL()
						  -- Delete a contact using CContactDatabase::DeleteContactL()

@SYMTestExpectedResults   Test cases will run without leaving and will output timing information.

*/

#include "t_perfcrudoperations.h"
#include "../t_utils.h"
#include <cntitem.h>

// Constants   
_LIT(KDbPathName,"C:Contacts.cdb");
const TPtrC KDbPath( (KDbPathName) );


CCrudOperations::CCrudOperations()
	{
	} 

CCrudOperations::~CCrudOperations()
	{
	iDb->CloseTables();
	delete iDb;
	} 
	
CCrudOperations* CCrudOperations::NewLC(RTest& aTest)
	{
	CCrudOperations* self = new(ELeave) CCrudOperations();
	CleanupStack::PushL(self);
	self->ConstructL(aTest);
	return(self);
	}  

void CCrudOperations::ConstructL(RTest& aTest)
	{
	iTest = &aTest;
	iDb = CContactDatabase::OpenL(KDbPath);
	CCntTest::ProfileReset(0, 59);
	} 

/**
	Main test routine.
*/
void CCrudOperations::DoTestsL(TInt aNumMsToGet20ItemsFromView)
	{
	iNumMsToGet20ItemsFromView= aNumMsToGet20ItemsFromView;

	// As, at this point, we haven't used the database before, this is the 
	// equivalent of startup. We fetch a contact as a bare minimum operation 
	// on the database to ensure that we fetch and cache the system template.
	// Then, we just get rid of the contact item as we don't actually want it.
	const TInt KContactIdOne(1);
	CContactItem* tmpItem = iDb->ReadContactLC(KContactIdOne);
	CleanupStack::PopAndDestroy(tmpItem);
	tmpItem = NULL;

	MatchContactsByPhoneNumL();
	MatchContactsByEmailAndSipL();
	DoCrudTestsL();
	}

/**
	Try to match the phone number given with existing numbers in the database.
*/
void CCrudOperations::MatchContactsByPhoneNumL()
	{
	_LIT(KPass, "Pass");
	_LIT(KFail, "Fail");
	_LIT(KMatchOutputFormat1, "Number of phone numbers matched: %d -- %s\n");
	_LIT(KMatchOutputFormat2, "Phone number lookup took: %d s %03d\n");
	_LIT(KMatchOutputFormat3, "The contact's first name: %S\n");

	_LIT(KPhoneNumberToFind, "241867306233253164487125"); // this number belongs to the 1000th
														  // contact item in the database, which 
														  // has a photo. 
	
	const TInt KMatchLengthFromRight(7); // match on the number without the dialling code prefix
										 // to avoid differences like +447876... and 07876...
	_LIT(KPhoneNumberTestTitle, "\nBeginning Phone Number Match testing...\n");


	iTest->Printf(KPhoneNumberTestTitle);
	TCntPerfTimer timer;
	timer.StartTimer();
	
	// Search the database...
	CContactIdArray* idArray = iDb->MatchPhoneNumberL(KPhoneNumberToFind, KMatchLengthFromRight);
	CleanupStack::PushL(idArray);
	TInt numMatches = idArray->Count();
	iTest->Printf(KMatchOutputFormat1, numMatches, numMatches ? KPass().Ptr() : KFail().Ptr());

	if (numMatches)
		{
		//fetch first contact item in array
		CContactItem* item = iDb->ReadContactLC((*idArray)[0]);
		timer.StopTimer();
		CContactItemFieldSet& fieldSet = item->CardFields();
		TInt firstNameFieldId = fieldSet.Find(KUidContactFieldGivenName);
		if (firstNameFieldId == KErrNotFound)
			{
			User::Leave(KErrNotFound);
			}
		CContactItemField& firstNameField = fieldSet[firstNameFieldId];
		CContactTextField* firstNameText = firstNameField.TextStorage();
		TPtrC firstName(firstNameText->Text());
		iTest->Printf(KMatchOutputFormat3, &firstName);

		CleanupStack::PopAndDestroy(item);
		}
	else
		{
		timer.StopTimer();
		}

	CleanupStack::PopAndDestroy(idArray);

	TInt result = timer.Result();
	TBuf<64> formattable;
	formattable.Format(KMatchOutputFormat2, result / 1000000, (result / 1000) % 1000);
	iTest->Printf(formattable);
	}

/**
	Try to match the phone number given with existing numbers in the database.
*/
void CCrudOperations::MatchContactsByEmailAndSipL()
	{
	_LIT(KPass, "Pass");
	_LIT(KFail, "Fail");
	_LIT(KEmailMatchOutputFormat1,"Number of email addresses matched: %d -- %s\n");
	_LIT(KEmailMatchOutputFormat2,"Email address lookup took: %d s %03d\n");
	_LIT(KMatchOutputFormat3, "The contact's first name: %S\n");

	// Test Email address matching

	// this address belongs to the 1000th contact item in the database, which has a photo. 
	_LIT(KEmailAddressToFind, "fred.bloggs@symbian.com"); 
	_LIT(KEmailTestTitle, "\nBeginning Email Address Match testing...\n");

	iTest->Printf(KEmailTestTitle);
	TCntPerfTimer timer;
	timer.StartTimer();
	
	// Search the database...
	CContactItemFieldDef* fieldsToSearch = new (ELeave) CContactItemFieldDef();
	CleanupStack::PushL(fieldsToSearch);
	fieldsToSearch->AppendL(KUidContactFieldEMail);
	CContactIdArray* idArray = iDb->FindLC(KEmailAddressToFind, fieldsToSearch);
	TInt numMatches = idArray->Count();
	iTest->Printf(KEmailMatchOutputFormat1, numMatches, numMatches ? KPass().Ptr() : KFail().Ptr());

	if (numMatches)
		{
		//fetch first contact item in array
		CContactItem* item = iDb->ReadContactLC((*idArray)[0]);
		CContactItemFieldSet& fieldSet = item->CardFields();
		TInt firstNameFieldId = fieldSet.Find(KUidContactFieldGivenName);
		if (firstNameFieldId == KErrNotFound)
			{
			User::Leave(KErrNotFound);
			}
		CContactItemField& firstNameField = fieldSet[firstNameFieldId];
		CContactTextField* firstNameText = firstNameField.TextStorage();
		TPtrC firstName(firstNameText->Text());
		iTest->Printf(KMatchOutputFormat3, &firstName);
		CleanupStack::PopAndDestroy(item);
		}
	
	timer.StopTimer();
	CleanupStack::PopAndDestroy(2, fieldsToSearch); // and idArray

	
	TInt result = timer.Result();
	TBuf<64> formattable;
	formattable.Format(KEmailMatchOutputFormat2, result / 1000000, (result / 1000) % 1000);
	iTest->Printf(formattable);

	// reset variables
	fieldsToSearch = NULL;
	idArray = NULL;
	numMatches = 0;
	timer.ResetTimer();
	result = 0;
	formattable.Zero();

	
	// Test SIP address matching

	// this address belongs to the 1000th contact item in the database, which has a photo. 
	_LIT(KSipAddressToFind, "sip:fred.bloggs@symbian.com"); 
	_LIT(KSipTestTitle, "\nBeginning SIP Address Match testing...\n");
	_LIT(KSipMatchOutputFormat1,"Number of SIP addresses matched: %d -- %s\n");
	_LIT(KSipMatchOutputFormat2,"SIP address lookup took: %d s %03d\n");

	iTest->Printf(KSipTestTitle);
	timer.StartTimer();
	
	// Search the database...
	fieldsToSearch = new (ELeave) CContactItemFieldDef();
	CleanupStack::PushL(fieldsToSearch);
	fieldsToSearch->AppendL(KUidContactFieldSIPID);
	idArray = iDb->FindLC(KSipAddressToFind, fieldsToSearch);
	numMatches = idArray->Count();
	iTest->Printf(KSipMatchOutputFormat1, numMatches, numMatches ? KPass().Ptr() : KFail().Ptr());

	if (numMatches)
		{
		//fetch first contact item in array
		CContactItem* item = iDb->ReadContactLC((*idArray)[0]);
		CContactItemFieldSet& fieldSet = item->CardFields();
		TInt firstNameFieldId = fieldSet.Find(KUidContactFieldGivenName);
		if (firstNameFieldId == KErrNotFound)
			{
			User::Leave(KErrNotFound);
			}
		CContactItemField& firstNameField = fieldSet[firstNameFieldId];
		CContactTextField* firstNameText = firstNameField.TextStorage();
		TPtrC firstName(firstNameText->Text());
		iTest->Printf(KMatchOutputFormat3, &firstName);
		CleanupStack::PopAndDestroy(item);
		}
	
	timer.StopTimer();
	CleanupStack::PopAndDestroy(2, fieldsToSearch); // and idArray

	result = timer.Result();
	formattable.Format(KSipMatchOutputFormat2, result / 1000000, (result / 1000) % 1000);
	iTest->Printf(formattable);
	}


/**
	Perform CRUD operations with 1 item in the database.
*/
void CCrudOperations::DoCrudTestsL()
	{
	// chosen because they have a photo field
	const TContactItemId K901stContactItemId(901);
	const TContactItemId K973rdContactItemId(973); 
	const TContactItemId K926thContactItemId(926);

	_LIT(KCRUDTestTitle, "\nBeginning CRUD Operations on 1 Contact (with photo) testing ...\n");
	_LIT(KCRUDTestNote, "[Figures for each write operation include time to fetch first 20 items in view.]\n");
	iTest->Printf(KCRUDTestTitle);
	iTest->Printf(KCRUDTestNote);
	
	TCntPerfTimer timer;

	// fetch a contact item to seed a new one
	CContactItem* temp = iDb->OpenContactL(K901stContactItemId);
	CleanupStack::PushL(temp);
	CContactItem* item = CContactCard::NewL(temp);
	CleanupStack::PopAndDestroy(temp);
	CleanupStack::PushL(item);

	// Create (add) a contact with a photo
	timer.StartTimer();
	iDb->AddNewContactL(*item);
	timer.StopTimer();
	TInt createResult = timer.Result() + iNumMsToGet20ItemsFromView; // add the time to fetch 20 items from view.
	timer.ResetTimer();
	CleanupStack::PopAndDestroy(item);
	item = NULL;

	// Read a contact with a photo
	timer.StartTimer();
	item = iDb->OpenContactLX(K973rdContactItemId); // lock object put on the CleanupStack
	CleanupStack::PushL(item);
	timer.StopTimer();
	TInt readResult = timer.Result();
	timer.ResetTimer();
	
	// Update the contact in the database
	timer.StartTimer();
	iDb->CommitContactL(*item);
	timer.StopTimer();
	TInt updateResult = timer.Result() + iNumMsToGet20ItemsFromView; // add the time to fetch 20 items from view.
	timer.ResetTimer();
	CleanupStack::PopAndDestroy(item);
	item = NULL;
	CleanupStack::PopAndDestroy(); // lock object

	// Delete a contact with a photo
	timer.StartTimer();
	iDb->DeleteContactL(K926thContactItemId);
	timer.StopTimer();
	TInt deleteResult = timer.Result() + iNumMsToGet20ItemsFromView; // add the time to fetch 20 items from view.
	timer.ResetTimer();
	
	// output timing result
	_LIT(KCreateOutputFormat,"Creating (adding) 1 contact took: %d s %03d\n");
	iTest->Printf(KCreateOutputFormat, createResult / 1000000, (createResult / 1000) % 1000);

	_LIT(KReadOutputFormat,"Reading 1 contact took: %d s %03d\n");
	iTest->Printf(KReadOutputFormat, readResult / 1000000, (readResult / 1000) % 1000);

	_LIT(KUpdateOutputFormat,"Updating 1 contact took: %d s %03d\n");
	iTest->Printf(KUpdateOutputFormat, updateResult / 1000000, (updateResult / 1000) % 1000);

	_LIT(KDeleteOutputFormat,"Deleting 1 contact took: %d s %03d\n");
	iTest->Printf(KDeleteOutputFormat, deleteResult / 1000000, (deleteResult / 1000) % 1000);
	}
