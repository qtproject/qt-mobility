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


#include <e32test.h>
#include <cntdb.h>
#include <cntitem.h>
#include <cntfldst.h>
#include <logcntmodel.h>
#include "test.h"

GLREF_D RTest theTest;
GLDEF_D CTrapCleanup* theCleanup;

RTest theTest(_L("Log View Contact Matching plugin test"));

LOCAL_D void Check(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		theTest(EFalse, aLine);
		}
	}

_LIT(KFirstName1, "Barney");
_LIT(KLastName1, "Rubble");
_LIT(KNumber1, "447774770432");
_LIT(KFirstName2, "Elma");
_LIT(KLastName2, "Fudd");

TBool OomTesting;


//
// Set the contents of a text field, creating the field if required
//
LOCAL_D void SetNameL(CContactItem& aItem,const TUid& aType,const TDesC& aName, TBool aAddField)
	{
	CContactItemFieldSet& fieldSet=aItem.CardFields();
	const TInt pos=fieldSet.Find(aType);
	if (!aAddField && pos!=KErrNotFound)
		fieldSet[pos].TextStorage()->SetTextL(aName);
	else
		{
		CContactItemField* field=CContactItemField::NewLC(KStorageTypeText,aType);
   		field->SetMapping(KUidContactFieldVCardMapUnusedN);
		field->TextStorage()->SetTextL(aName);
		aItem.AddFieldL(*field);
		CleanupStack::Pop(); // field
		}
	}

//Creates a contact item. Then adds it to the default contacts database.
LOCAL_D TLogContactItemId CreateContactL(const TDesC& aGivenName, 
									 const TDesC& aFamilyName, 
									 const TDesC& aPhoneNumber)
	{
	CContactItem* item=CContactCard::NewLC();

	if (aFamilyName.Length())
		SetNameL(*item, KUidContactFieldFamilyName, aFamilyName, ETrue);

	if (aGivenName.Length())
		SetNameL(*item, KUidContactFieldGivenName, aGivenName, ETrue);

	if (aPhoneNumber.Length())
		SetNameL(*item, KUidContactFieldPhoneNumber, aPhoneNumber, ETrue);

	CContactItemFieldSet& fieldSet= item->CardFields();

	TLogContactItemId id = KLogNullContactId;
	
	if (fieldSet.Count())
		{
		// Open the DB for writing
		CContactDatabase* TheContacts = CContactDatabase::OpenL();
		CleanupStack::PushL(TheContacts);
		id = TheContacts->AddNewContactL(*item);
		CleanupStack::PopAndDestroy(); // TheContacts,
		}
	else
		User::Leave(KErrNotSupported);
	CleanupStack::PopAndDestroy(item);
	return id;
	}
	

//This function checks the name read from the contacts DB is the same as the name saved in the DB.
//Contact name format is taken into account.
LOCAL_D void CheckContactName(TDesC& aName, const TDesC& aGivenName, const TDesC& aFamilyName, TLogContactNameFormat aContactNameFormat)
	{
	TBuf<128> fullname;
	if(aContactNameFormat == ELogWesternFormat)
		{
		fullname.Format(_L("%S %S"), &aGivenName, &aFamilyName);
		}
	else //ELogChineseFormat
		{
		fullname.Format(_L("%S %S"), &aFamilyName, &aGivenName);
		}
	TEST(aName == fullname);
	}
	
_LIT(KRemotePartyNameSeparator, " ");


/**
@SYMTestCaseID				PIM-APPENG-CNTMODEL-EC021-CIT-0001
@SYMTestCaseDesc			Load the plug-in
@SYMTestType				CIT
@SYMTestPriority			High
@SYMTestActions				A call is made to the NewL method of the interface class. 
@SYMTestExpectedResults		A pointer to the plug-in should be returned. No errors should occur.
@SYMTestStatus				Implemented
@SYMTestCaseDependencies	Plug-in implementation must exist
@SYMEC						EC021 Break dependency of Log engine (Syslibs/Logeng) on Contacts model (App-engines/Cntmodel)
*/

LOCAL_D void TestLoadContactMatchingPluginL(CLogCntModel* &aCntModel)
	{
	// Delete the contacts DB
	
	// Note, suppressing test prints during OOM test because they flood epocwind.out and use memory
	if(!OomTesting)
		theTest.Next(_L("@SYMTESTCaseID:PIM-APPENG-CNTMODEL-EC021-CIT-0001 Delete the contacts DB to start the test"));

	TRAPD(err, CContactDatabase::DeleteDefaultFileL());
	if(OomTesting && err==KErrNoMemory)
		User::Leave(KErrNoMemory);
	TEST(err == KErrNone || err == KErrNotFound);

	// Load the plugin
	aCntModel=NULL;
	if(!OomTesting)
		theTest.Next(_L("Load contact matching plugin"));

	TRAP(err,aCntModel=CLogCntModel::NewL());
	if( err==KEComErrNoInterfaceIdentified)
		{
		RDebug::Print(_L("Plugin not found"));
		}
	// Remove this line when DEF098715 is fixed
	if(err==KErrNoMemory)REComSession::FinalClose();
	
	if(OomTesting && err==KErrNoMemory)

		User::Leave(KErrNoMemory);

	TEST2(err, KErrNone);
	}

/**
@SYMTestCaseID				PIM-APPENG-CNTMODEL-EC021-CIT-0002
@SYMTestCaseDesc			Open the contacts session
@SYMTestType				CIT
@SYMTestPriority			High
@SYMTestActions				A call is made to the OpenContactsL method of the interface class. 
@SYMTestExpectedResults		The session should be created and opened. No errors should occur.
@SYMTestStatus				Implemented
@SYMTestCaseDependencies	Plug-in implementation must exist, Contacts DB should not exist
@SYMEC						EC021 Break dependency of Log engine (Syslibs/Logeng) on Contacts model (App-engines/Cntmodel)
*/

LOCAL_D void TestOpenContactMatchingPluginL(CLogCntModel* aCntModel)
	{	
	// Open contacts DB
	if(!OomTesting)
		theTest.Next(_L("@SYMTESTCaseID:PIM-APPENG-CNTMODEL-EC021-CIT-0002 Open contacts DB using contact matching plugin"));

	TRAPD(err, aCntModel->OpenContactsL());
	
	if(OomTesting && err==KErrNoMemory)

		User::Leave(KErrNoMemory);
	TEST2(err,KErrNone);
	}

/**
@SYMTestCaseID				PIM-APPENG-CNTMODEL-EC021-CIT-0003
@SYMTestCaseDesc			Match a phone number to a contacts item 
@SYMTestType				CIT
@SYMTestPriority			High
@SYMTestActions				A call is made to the MatchPhoneNumberL method of the interface class. 
@SYMTestExpectedResults		A non-null contacts Id should be returned. No errors should occur.
@SYMTestStatus				Implemented
@SYMTestCaseDependencies	Plug-in implementation must exist, contacts DB should exist, a contact item for the test phone number should exist in the DB.
@SYMEC						EC021 Break dependency of Log engine (Syslibs/Logeng) on Contacts model (App-engines/Cntmodel)
*/

LOCAL_D TLogContactItemId TestMatchContactL(CLogCntModel* aCntModel)
	{
	// Read back a contact Id using the plugin interface, should be null
	TLogContactItemId  contactId = KLogNullContactId;
	if(!OomTesting)
		theTest.Next(_L("@SYMTESTCaseID:PIM-APPENG-CNTMODEL-EC021-CIT-0003 Check that correct id read back when no contact exists using contact matching plugin"));

	TRAPD(err, contactId=aCntModel->MatchPhoneNumberL(KNumber1, 7));
	if(OomTesting && err==KErrNoMemory)
		User::Leave(KErrNoMemory);	
	TEST(KErrNone==err);
	TEST2(contactId,KLogNullContactId);
		
	// Create a contact
	if(!OomTesting)
		theTest.Next(_L("Create a test entry to match"));

	TLogContactItemId id1 = CreateContactL(KFirstName1, KLastName1, KNumber1);
	
	// Read back the contact Id using the plugin interface
	contactId = KLogNullContactId;
	if(!OomTesting)
		theTest.Next(_L("Check that correct id read back using contact matching plugin"));

	TRAP(err, contactId=aCntModel->MatchPhoneNumberL(KNumber1, 7));
	if(OomTesting && err==KErrNoMemory)
		User::Leave(KErrNoMemory);	
	TEST(KErrNone==err);
	TEST2(id1,contactId);
	return contactId;
	}

/**
@SYMTestCaseID				PIM-APPENG-CNTMODEL-EC021-CIT-0004
@SYMTestCaseDesc			Fetch the string data for the contact item matching the phone number in Western format.
@SYMTestType				CIT
@SYMTestPriority			High
@SYMTestActions				A call is made to the ReadContactNameL method of the interface class using the contacts Id retrieved by APPENG-CNTMODEL-EC021-CIT-0003 using Western format.  
@SYMTestExpectedResults		The text for the contacts item should be retrieved and should be the text in the item created for the test, in Western format. No errors should occur.
@SYMTestStatus				Implemented
@SYMTestCaseDependencies	Plug-in implementation must exist, contacts DB should exist, a contact item for the test phone number should exist.
@SYMEC						EC021 Break dependency of Log engine (Syslibs/Logeng) on Contacts model (App-engines/Cntmodel)
*/

LOCAL_D void TestGetWesternContactStringL(CLogCntModel* aCntModel, TLogContactItemId aContactId)
	{
	// Read back the text information in Western format using the plugin interface
	if(!OomTesting)
		theTest.Next(_L("@SYMTESTCaseID:PIM-APPENG-CNTMODEL-EC021-CIT-0004 Check that correct string ( Western Format ) read back using contact matching plugin"));

	// Setup buffer to contain concatenated result
	TBuf<128> buf;
	TRAPD(err, aCntModel->ReadContactNameL(aContactId, buf, ELogWesternFormat));
	if(OomTesting && err==KErrNoMemory)
		User::Leave(KErrNoMemory);	
	TEST(KErrNone==err);
	::CheckContactName(buf, KFirstName1, KLastName1, ELogWesternFormat);
	}

/**
@SYMTestCaseID				PIM-APPENG-CNTMODEL-EC021-CIT-0005
@SYMTestCaseDesc			Fetch the string data for the contact item matching the phone number in Chinese format.
@SYMTestType				CIT
@SYMTestPriority			High
@SYMTestActions				A call is made to the ReadContactNameL method of the interface class using the contacts Id retrieved by APPENG-CNTMODEL-EC021-CIT-0003 using Chinese format. 
@SYMTestExpectedResults		The text for the contacts item should be retrieved and should be the text in the item created for the test, in Chinese format. No errors should occur.
@SYMTestStatus				Implemented
@SYMTestCaseDependencies	Plug-in implementation must exist, contacts DB should exist, a contact item for the test phone number should exist.
@SYMEC						EC021 Break dependency of Log engine (Syslibs/Logeng) on Contacts model (App-engines/Cntmodel)
*/


LOCAL_D void TestGetChineseContactStringL(CLogCntModel* aCntModel, TLogContactItemId aContactId)
	{
	// Read back the text information in Chinese format using the plugin interface
	if(!OomTesting)
		theTest.Next(_L("@SYMTESTCaseID:PIM-APPENG-CNTMODEL-EC021-CIT-0005 Check that correct string ( Chinese Format ) read back using contact matching plugin"));

	// Setup buffer to contain concatenated result
	TBuf<128> buf;
	TRAPD(err, aCntModel->ReadContactNameL(aContactId, buf, ELogChineseFormat));
	if(OomTesting && err==KErrNoMemory)
		User::Leave(KErrNoMemory);	
	TEST(KErrNone==err);
	::CheckContactName(buf, KFirstName1, KLastName1, ELogChineseFormat);
	}

/**
@SYMTestCaseID				PIM-APPENG-CNTMODEL-EC021-CIT-0012
@SYMTestCaseDesc			Check failure cases for String Get.
@SYMTestType				CIT
@SYMTestPriority			High
@SYMTestActions				Calls are made to the ReadContactNameL method for 3 failure cases
							A buffer that is too small, 
							A non-existent contact id 
							An existing contact id with an incorrect format parameter.
@SYMTestExpectedResults		For the buffer being too small no errors should occur. 
							For a non-existent contact id no string should be found.
							For an existing contact id with an incorrect format parameter the Western format string should be returned.
@SYMTestStatus				Implemented
@SYMTestCaseDependencies	Plug-in implementation must exist, contacts DB should exist, a contact item for the test phone number should exist.
@SYMEC						EC021 Break dependency of Log engine (Syslibs/Logeng) on Contacts model (App-engines/Cntmodel)
*/


LOCAL_D void TestFailGetContactStringL(CLogCntModel* aCntModel, TLogContactItemId aContactId)
	{
	// Read back the text information into a buffer too small to hold it using the plugin interface
	if(!OomTesting)
		theTest.Next(_L("@SYMTESTCaseID:PIM-APPENG-CNTMODEL-EC021-CIT-0012 Read back the text information into a buffer too small to hold it using the plugin interface"));

	// Setup buffer to contain concatenated result
	TBuf<1> overflow;
	TRAPD(err, aCntModel->ReadContactNameL(aContactId, overflow, ELogWesternFormat));
	if(OomTesting && err==KErrNoMemory)
		User::Leave(KErrNoMemory);	
	TEST(KErrNone==err);
	
	// Read back the text information for a non-existent Id using the plugin interface
	if(!OomTesting)
		theTest.Next(_L("Check no string read back for a non-existent Id using contact matching plugin"));

	// Set id1 to a non-existent Id
	aContactId=100;
	// Re-initialise buffer
	// Setup buffer to contain concatenated result
	TBuf<128> buf;
	// Initialise buffer
	buf=_L("");
	TRAP(err, aCntModel->ReadContactNameL(aContactId, buf, ELogWesternFormat));
	if(OomTesting && err==KErrNoMemory)
		User::Leave(KErrNoMemory);
	TEST(KErrNotFound==err);
	TEST(buf.Length()==0);

	// Read back the text information for an existing Id when making a call with an incorrect format parameter using the plugin interface
	if(!OomTesting)
		theTest.Next(_L("Check that correct string for an existing Id ( Western Format ) read back when making a call with an incorrect format parameter using contact matching plugin"));

	// Set id1 to a existing Id
	TLogContactItemId contactId=1;
	buf=_L("");
	TRAP(err, aCntModel->ReadContactNameL(contactId, buf, static_cast<TLogContactNameFormat>(100)));
	if(OomTesting && err==KErrNoMemory)
		User::Leave(KErrNoMemory);
	TEST(KErrNone==err);
	::CheckContactName(buf, KFirstName1, KLastName1, ELogWesternFormat);
	}

/**
@SYMTestCaseID				PIM-APPENG-CNTMODEL-EC021-CIT-0006
@SYMTestCaseDesc			Attempt to match a phone number to a contacts Id when more than one contacts items exist that match the phone number
@SYMTestType				CIT
@SYMTestPriority			High
@SYMTestActions				A call is made to the MatchPhoneNumberL method of the interface class. 
@SYMTestExpectedResults		The null Id should be returned. No errors should occur.
@SYMTestStatus				Implemented
@SYMTestCaseDependencies	Plug-in implementation must exist, contacts DB should exist, more than one contact item for the test phone number should exist ( these should be created for the test ).
@SYMEC						EC021 Break dependency of Log engine (Syslibs/Logeng) on Contacts model (App-engines/Cntmodel)
*/

LOCAL_D void TestMultiContactMatchL(CLogCntModel* aCntModel)
	{
	// Check that NULL Id is returned for phone number with > 1 contact
	if(!OomTesting)
		theTest.Next(_L("@SYMTESTCaseID:PIM-APPENG-CNTMODEL-EC021-CIT-0006 Test contacts matching for a phone number with multiple contacts defined"));

	// Create another contact with the same number
	(void)CreateContactL(KFirstName2, KLastName2, KNumber1);
	aCntModel->OpenContactsL();
	TLogContactItemId contactId=KLogNullContactId;
	TRAPD(err, contactId=aCntModel->MatchPhoneNumberL(KNumber1, 7));
	if(OomTesting && err==KErrNoMemory)
		User::Leave(KErrNoMemory);
	TEST(KErrNone==err);
	// Check no contact is returned
	TEST(KLogNullContactId==contactId);
	}

/**
@SYMTestCaseID				PIM-APPENG-CNTMODEL-EC021-CIT-0007
@SYMTestCaseDesc			Close the open contacts session.
@SYMTestType				CIT
@SYMTestPriority			High
@SYMTestActions				A call is made to the CloseContacts method of the interface class. 
@SYMTestExpectedResults		The session should close. No errors should occur.
@SYMTestStatus				Implemented
@SYMTestCaseDependencies	Plug-in implementation must exist. Contacts session should be open.
@SYMEC						EC021 Break dependency of Log engine (Syslibs/Logeng) on Contacts model (App-engines/Cntmodel)
*/

LOCAL_D void TestCloseContactMatchingPluginL(CLogCntModel* aCntModel)
	{

	// Close contacts DB
	if(!OomTesting)
		theTest.Next(_L("@SYMTESTCaseID:PIM-APPENG-CNTMODEL-EC021-CIT-0007 Close contacts DB using contact matching plugin"));

	aCntModel->CloseContacts();
	}

/**
@SYMTestCaseID				PIM-APPENG-CNTMODEL-EC021-CIT-0008
@SYMTestCaseDesc			Attempt to open the contacts session when it is already opened.
@SYMTestType				CIT
@SYMTestPriority			High
@SYMTestActions				Two consecutive calls are made to the OpenContactsL method of the interface class. 
@SYMTestExpectedResults		The session should remain open. No errors should occur.
@SYMTestStatus				Implemented
@SYMTestCaseDependencies	Plug-in implementation must exist.
@SYMEC						EC021 Break dependency of Log engine (Syslibs/Logeng) on Contacts model (App-engines/Cntmodel)
*/

LOCAL_D void TestMultiOpenContactMatchingPluginL(CLogCntModel* aCntModel)
	{
	// Test consecutive opens of contacts DB using contact matching plugin
	if(!OomTesting)
		theTest.Next(_L("@SYMTESTCaseID:PIM-APPENG-CNTMODEL-EC021-CIT-0008 Test consecutive opens of contacts DB using contact matching plugin"));

	aCntModel->OpenContactsL();
	aCntModel->OpenContactsL();
	}
 

/**
@SYMTestCaseID				PIM-APPENG-CNTMODEL-EC021-CIT-0009
@SYMTestCaseDesc			Attempt to close the contacts session when it is not open.
@SYMTestType				CIT
@SYMTestPriority			High
@SYMTestActions				Two consecutive calls are made to the CloseContacts method of the interface class. 
@SYMTestExpectedResults		Nothing should happen. No errors should occur.
@SYMTestStatus				Implemented
@SYMTestCaseDependencies	Plug-in implementation must exist. 
@SYMEC						EC021 Break dependency of Log engine (Syslibs/Logeng) on Contacts model (App-engines/Cntmodel)
*/


LOCAL_D void TestMultiCloseContactMatchingPluginL(CLogCntModel *aCntModel)
	{
	// Test consecutive closes of contacts DB using contact matching plugin
	if(!OomTesting)
		theTest.Next(_L("@SYMTESTCaseID:PIM-APPENG-CNTMODEL-EC021-CIT-0009 Test consecutive closes of contacts DB using contact matching plugin"));

	aCntModel->CloseContacts();
	aCntModel->CloseContacts();
	}

// Call the functional tests for the contacts matching plug-in


LOCAL_D void TestContactMatchingPluginL()
	{
	CLogCntModel* cntModel=NULL;
	TestLoadContactMatchingPluginL(cntModel);
	CleanupStack::PushL(cntModel);
	
	TestOpenContactMatchingPluginL(cntModel);
	TLogContactItemId contactId = TestMatchContactL(cntModel);
	TestGetWesternContactStringL(cntModel, contactId);
	TestGetChineseContactStringL(cntModel, contactId);
	TestFailGetContactStringL(cntModel, contactId);
	TestMultiContactMatchL(cntModel);
	TestCloseContactMatchingPluginL(cntModel);
	TestMultiOpenContactMatchingPluginL(cntModel);
	TestMultiCloseContactMatchingPluginL(cntModel);
		
	// Delete the plugin
	if(!OomTesting)
		theTest.Next(_L("Unload the plugin"));

	CleanupStack::PopAndDestroy(cntModel);
	}
	
const TInt KMaxIterations     =   100;

/**
@SYMTestCaseID				PIM-APPENG-CNTMODEL-EC021-CIT-0011
@SYMTestCaseDesc			Performance measurement test
@SYMTestType				CIT
@SYMTestPriority			High
@SYMTestActions				Make 100 calls to all the interface methods and report the average time for each call. 
@SYMTestExpectedResults		No errors should occur. The average time taken for all the API’s should be printed to epocwind.out.
@SYMTestStatus				Implemented
@SYMTestCaseDependencies	Plug-in implementation must exist. The contacts DB should contain some contact items for test purposes.
@SYMEC						EC021 Break dependency of Log engine (Syslibs/Logeng) on Contacts model (App-engines/Cntmodel)
*/
LOCAL_D void PerformanceTestContactMatchingPluginL()
	{
	TIMERINSTALL;
	TInt i;

	// Delete the contacts DB so when we open it teh 1st time the DB will be created
	theTest.Printf(_L("Delete the contacts DB\n"));
	TRAPD(err, CContactDatabase::DeleteDefaultFileL());
	TEST(err == KErrNone || err == KErrNotFound);
	
	CLogCntModel *cntModel=NULL;
	for (i = 0; i < KMaxIterations; i++)
		{
		TIMERSTART;
		// Don't trap open, want the load time only
		cntModel=CLogCntModel::NewL();
		TIMERSTOP;
		TIMERPRINT;
		// Delete to allow next create
		delete cntModel;
		}
	theTest.Printf(_L("Average: Plugin load           (us) %08d\n"), TIMERAVERAGE);

	// Load the plug-in for the rest of the test
	cntModel=CLogCntModel::NewL();
	CleanupStack::PushL(cntModel);

	TIMERRESET;
	TIMERSTART;
	// Open contacts DB - this will create the DB because it doesn't exist
	cntModel->OpenContactsL();
	TIMERSTOP;
	theTest.Printf(_L("First open of contacts DB      (us) %08d\n"), delta);
	// Close DB so we can reopen it in the test below.
	cntModel->CloseContacts();
	
	TIMERRESET;
	for (i = 0; i < KMaxIterations; i++)
		{
		TIMERSTART;
		// Don't trap open, want the load time only
		cntModel->OpenContactsL();
		TIMERSTOP;
		TIMERPRINT;		
		// Close to allow next open
		cntModel->CloseContacts();
		}
	theTest.Printf(_L("Average: Open contacts DB      (us) %08d\n"), TIMERAVERAGE);
	
	// Open the contacts DB for the rest of the test.
	cntModel->OpenContactsL();
	
	// Create a contact
	TLogContactItemId id1 = CreateContactL(KFirstName1, KLastName1, KNumber1);
	
	// Setup buffer to contain concatenated result
	TBuf<128> buf;
	// Separator
	TBufC<2> separator(KRemotePartyNameSeparator);
	
	// Get average time to read the contact Id using the plugin interface
	TIMERRESET;
	for (i = 0; i < KMaxIterations; i++)
		{
		TLogContactItemId  contactId;
		TIMERSTART;
		contactId=cntModel->MatchPhoneNumberL(KNumber1, 7);
		TIMERSTOP;
		TIMERPRINT;
		TEST2(id1,contactId);
		}
	theTest.Printf(_L("Average: Match phone number    (us) %08d\n"), TIMERAVERAGE);
	
	// Get average time to read the text information using the plugin interface
	TIMERRESET;
	for (i = 0; i < KMaxIterations; i++)
		{
		TIMERSTART;
		cntModel->ReadContactNameL(id1, buf, ELogWesternFormat);
		TIMERSTOP;
		TIMERPRINT;
		}
	theTest.Printf(_L("Average: Read contact info     (us) %08d\n"), TIMERAVERAGE);

	// Get average time to close the DB using the plugin interface
	TIMERRESET;
	for (i = 0; i < KMaxIterations; i++)
		{
		TIMERSTART;
		cntModel->CloseContacts();
		TIMERSTOP;
		TIMERPRINT;
		// Reopen to allow next close
		cntModel->OpenContactsL();
		}
	theTest.Printf(_L("Average: Close contacts DB     (us) %08d\n"), TIMERAVERAGE);

	cntModel->CloseContacts();
	CleanupStack::PopAndDestroy(); // cntModel;
	
	// Check memory usage for plugin. At this stage ECOM and contacts are running so the HEAP usage
	// detected by loading the plugin should be just due to the plugin

	// Save initial heap available size
	TInt largestBlock;
	TInt initialSize=User::Heap().Available(largestBlock);

	// Load plugin and save heap available size
	cntModel=CLogCntModel::NewL();
	TInt size=User::Heap().Available(largestBlock);
	
	// Delete the plugin and check that the heap size reverts to the initial size
	delete cntModel;
	TEST(initialSize==User::Heap().Available(largestBlock));
	
	// Print heap used by plugin load
	theTest.Printf(_L("Plugin load uses %08d bytes on the heap\n"), initialSize-size);
	}

/**
@SYMTestCaseID				PIM-APPENG-CNTMODEL-EC021-CIT-0010
@SYMTestCaseDesc			Resource leak test
@SYMTestType				CIT
@SYMTestPriority			High
@SYMTestActions				Repeat all the previous test cases in an OOM test and check for handle leaks. 
@SYMTestExpectedResults		No memory leaks or handle leaks should occur.
@SYMTestStatus				Implemented
@SYMTestCaseDependencies	Plug-in implementation must exist
@SYMEC						EC021 Break dependency of Log engine (Syslibs/Logeng) on Contacts model (App-engines/Cntmodel)
*/
LOCAL_C void OomTest(void (*testFuncL)(void))
	{	
	TInt error;
	TInt count = 0;
	OomTesting = ETrue;

	do
		{
		User::__DbgSetAllocFail(RHeap::EUser, RHeap::EFailNext, ++count);
		User::__DbgMarkStart(RHeap::EUser);
		
  		// find out the number of open handles
		TInt startProcessHandleCount;
		TInt startThreadHandleCount;
		RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);
		
		TRAP(error, (testFuncL)());
		
		// check that no handles have leaked
		TInt endProcessHandleCount;
		TInt endThreadHandleCount;
		RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);
		
		TEST2(endProcessHandleCount, startProcessHandleCount);
		TEST2(endThreadHandleCount, startThreadHandleCount);
		
		User::__DbgMarkEnd(RHeap::EUser, 0);
		} while(error == KErrNoMemory);

	_LIT(KTestFailed, "Out of memory test failure on iteration %d\n");
	__ASSERT_ALWAYS(error==KErrNone, theTest.Panic(error, KTestFailed, count));

	// Reset alloc fail
	User::__DbgSetAllocFail(RHeap::EUser, RHeap::ENone, 1);
	
	theTest.Printf(_L("OOM testing succeeded at heap failure rate of %d\n"), count);
	OomTesting = EFalse;
	}

/**

@SYMTestCaseID     PIM-T-LOGCNTMODEL-0001

*/

LOCAL_D void doTestsL()
	{
	theTest.Start(_L("@SYMTESTCaseID:PIM-T-LOGCNTMODEL-0001 Test contact matching plugin"));

	
	TInt startProcessHandleCount;
	TInt startThreadHandleCount;
	RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);
	
	CActiveScheduler *testScheduler = new (ELeave) CActiveScheduler;
	CleanupStack::PushL( testScheduler );
	CActiveScheduler::Install( testScheduler );
	
	// Test main plugin functionality
	theTest.Next(_L("Test contacts plugin functionality"));

	TestContactMatchingPluginL();
	
	// OOM test main plug-in functionality
	theTest.Next(_L("OOM test contacts plugin functionality"));

	OomTest(TestContactMatchingPluginL);
	
	// Perform plugin performance measurements - no benchmark testing
	theTest.Next(_L("Test contacts matching plugin performance"));

	PerformanceTestContactMatchingPluginL();
		
	CleanupStack::PopAndDestroy(); // Scheduler,
	testScheduler = NULL;
	
	TInt endProcessHandleCount;
	TInt endThreadHandleCount;
	RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);
	TEST2(endProcessHandleCount, startProcessHandleCount);
	TEST2(endThreadHandleCount, startThreadHandleCount);
	}

TInt E32Main()
	{	
	__UHEAP_MARK;

	theCleanup = CTrapCleanup::New();

	TRAPD(err, ::doTestsL());	
	TEST2(err, KErrNone);

	delete theCleanup;	

	theTest.End();
	theTest.Close();

	__UHEAP_MARKEND;

	return KErrNone;
	}
