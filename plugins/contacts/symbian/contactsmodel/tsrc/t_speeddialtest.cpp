/*
* Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32std.h>
#include <e32test.h>
#include <cntdb.h>
#include <cntitem.h>
#include <cntfldst.h>
#include <cntview.h>
#include "t_utils2.h"
#include <coreappstest/testserver.h>
#include "t_speeddialtest.h"

///////////////////////////////////////////////////////////////////////////////
// Constants.
///////////////////////////////////////////////////////////////////////////////
_LIT(KTestName,"T_SpeedDialTest");

// Configure database locations
_LIT(KDbFileName,"c:t_speeddialtest.cdb");
_LIT(KLongNumSpeedDialDbName, "c:LongNumSpeedDialDb.cdb");

// Configure .ini file location
_LIT(KContactsModelIniFile,"C:\\private\\10003A73\\CntModel.ini");


const TInt KNumSmsContacts=5;
const TInt KSpeedDialPositionOne = 1;
const TInt KSpeedDialPositionTwo  = 2;
const TInt KSpeedDialContact = 3;
const TInt KSpeedDialContactA = 4;
const TInt KSpeedDialContactB = 5;


///////////////////////////////////////////////////////////////////////////////
// Global Data
///////////////////////////////////////////////////////////////////////////////
RTest g_test( KTestName );

///////////////////////////////////////////////////////////////////////////////
// Class CLockServerTester Implementation
///////////////////////////////////////////////////////////////////////////////

CLockServerTester* CLockServerTester::NewL()
	{
	CLockServerTester* self=new(ELeave) CLockServerTester();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}
 
CLockServerTester::~CLockServerTester()
 	{
 	DeleteProcess();
 	}
 
CLockServerTester::CLockServerTester()
 	{
 	}
 	
void CLockServerTester::ConstructL()
 	{
 	}
 
void CLockServerTester::FindServerAndKillItL()
 	{
 	SetupProcessL();
 	OpenAndKillProcess();
 	}
 	
void CLockServerTester::OpenAndKillProcess()
	{
	iProcess.iExecution->Open(iProcessFullName);
	iProcess.iExecution->Kill(KErrNone);
	}

void CLockServerTester::SetupProcessL()
	{
	iProcess.iFind = new (ELeave) TFindProcess(_L("cntsrv*"));
	iProcess.iExecution = new (ELeave) RProcess();
	iProcess.iFind->Next(iProcessFullName);
	}

void CLockServerTester::DeleteProcess()
	{
	delete iProcess.iFind;
	if (iProcess.iExecution != NULL) iProcess.iExecution->Close();
	delete iProcess.iExecution;
   
	iProcess.iFind = NULL;
	iProcess.iExecution = NULL;
	}

///////////////////////////////////////////////////////////////////////////////
// Function Implementation
///////////////////////////////////////////////////////////////////////////////
TBool ContactIsASpeedDial(const CContactItem& aItem)
	{
	TBool retval = EFalse;
	CContactItemFieldSet& fields = aItem.CardFields();
	const TInt max = fields.Count();
	TInt counter = 0;

	for (counter = 0; counter < max; counter++)
		{
		retval = fields[counter].IsSpeedDial();
		if (retval) break;
		}

	return retval;
	}
	

TBool ContactIsASpeedDialL(TInt aContactId, CContactDatabase& aDatabase)
    {
    CContactItem* item = NULL;
    TBool result = EFalse;
    
    item = aDatabase.OpenContactL(aContactId);
    CleanupStack::PushL(item);
    
    const CContactItemFieldSet& fields = item->CardFields();
    const TInt count = fields.Count();
    
    for (TInt index = 0; index < count; ++index)
        {
        if (fields[index].IsSpeedDial())
            {
            result = ETrue;
            break;
            }
        }
    aDatabase.CloseContactL(aContactId);        
       
    CleanupStack::PopAndDestroy(item);        
    return result;    
    }


TInt DeleteIniFile()
	{
	TInt retval = KErrNone;
	//TInt deleteSuccess;

	// Delete any existing ini file so that we can be sure this test is ok

    RPIMTestServer serv;
    User::LeaveIfError(serv.Connect());
    
    TRAPD (deleteSuccess, serv.DeleteFileL(KContactsModelIniFile));
    
    if ( deleteSuccess != KErrNotFound && deleteSuccess != KErrNone )
    {
		  retval = deleteSuccess;
	}
		
	return retval;
	}

void AddContactsL(CRandomContactGenerator& aGenerator)
	{
	TInt ii;
	for (ii=0;ii<KNumSmsContacts;ii++)
		{
		TInt bit=0;
		bit |= CContactDatabase::ESmsable;
		aGenerator.AddTypicalContactForFilterL(bit);
		}
	}

CContactDatabase* ReplaceDatabaseAndCreateContactsLC()
	{
	CContactDatabase* database = CContactDatabase::ReplaceL(KDbFileName);
	CleanupStack::PushL(database);

	CreateContactsL(*database);

	return database;
	}

void CreateContactsL(CContactDatabase& aDb)
	{
	CRandomContactGenerator* generator = CRandomContactGenerator::NewL();
	CleanupStack::PushL( generator );
	generator->SetDbL(aDb);
	AddContactsL(*generator);
	CleanupStack::PopAndDestroy( generator );
	}

CContactDatabase* SetupLC()
	{
	User::LeaveIfError( DeleteIniFile() );
	return ReplaceDatabaseAndCreateContactsLC();
	}


void AssignSpeedDialL( TInt aContact, TInt aSpeedDialID, CContactDatabase& database )
	{
	CContactItem* item = NULL;
	item = database.OpenContactL( aContact );
	
	CleanupStack::PushL( item );

	if ( ContactIsASpeedDial( *item ) )
		{
		g_test.Printf( _L("Contact is a speed dial !!! \n") );
		}
	else
		{
		g_test.Printf( _L("Contact is NOT A SPEED DIAL \n") );
		}

	CContactItemFieldSet &fieldSet = item->CardFields();
    for(TInt index = fieldSet.Count() - 1; index > 0; --index)
        {
        if(fieldSet[index].StorageType() == KStorageTypeText)
            {
            //set the last text field from the fieldset as speeddial field.
        	database.SetFieldAsSpeedDialL(*item, index, aSpeedDialID );
            break;
            }
        }
        
	g_test.Next( _L("Contact changed to speed dial") );
	g_test(ContactIsASpeedDial( *item ));

	CleanupStack::PopAndDestroy( item );
	}

void UpdateSpeedDialL(TInt aContact, TInt aSpeedDialID, CContactDatabase& database )
	{
	CContactItem* item = NULL;
	item = database.OpenContactL( aContact );
	CleanupStack::PushL( item );

	if ( ContactIsASpeedDial( *item ) )
		{
		g_test.Printf( _L("Contact is a speed dial !!! \n") );
		database.SetFieldAsSpeedDialL(*item, item->Id(), aSpeedDialID );
		}
	else
		{
		g_test.Printf( _L("Contact is NOT A SPEED DIAL \n") );
		}

	CleanupStack::PopAndDestroy( item );
	}
		
void RemoveSpeedDialL(TInt aContact, TInt aSpeedDialId, CContactDatabase& aDatabase)
    {    
    TInt error = KErrNone;
        
    TRAP(error, aDatabase.RemoveSpeedDialFieldL(aContact, aSpeedDialId));
    g_test(error == KErrNone);           
    }    

void RemoveSpeedDialWhenOpenL(TInt aContact, TInt aSpeedDialID, CContactDatabase& aDatabase)
	{
	CContactItem* card1 = 0;
	TInt error = KErrNone;

	// First open the contact, which locks it:
	card1 = aDatabase.OpenContactL(aContact);
	CleanupStack::PushL(card1);

	// Now attempt to open the contact (must fail with in use):
	error = KErrNone;
	TRAP(error, aDatabase.OpenContactL(aContact));
	g_test(error == KErrInUse);

	// Now remove the speed dial on that contact (must fail with in use):
	error = KErrNone;
	TRAP(error, aDatabase.RemoveSpeedDialFieldL(aContact, aSpeedDialID));
	g_test(error == KErrInUse);
	// Now remove the speed dial on that contact again (must fail with in use):
	// Defect was application crash.
	error = KErrNone;
	TRAP(error, aDatabase.RemoveSpeedDialFieldL(aContact, aSpeedDialID));
	g_test(error == KErrInUse);

	// Now attempt to open the contact (must fail with in use):
	// Defect was fails with wrong error/no error.
	error = KErrNone;
	TRAP(error, aDatabase.OpenContactL(aContact));
	g_test(error == KErrInUse);

	// Cleanup and close:
	CleanupStack::PopAndDestroy(card1);
	TRAP(error, aDatabase.CloseContactL(aContact));
	g_test(error == KErrNone);
	}

void TestDatabaseWithSpeedDialsL()
	{
	g_test.Next(_L("Setting up..."));

	CContactDatabase* database = SetupLC();

	g_test.Next(_L("First Speed dial assignment"));
	// Assign First time ok !
	TRAPD(error, AssignSpeedDialL( KSpeedDialContact, KSpeedDialPositionOne, *database ) );
	g_test(error == KErrNone);

	g_test.Next(_L("Second Speed dial assignment"));
	// Assign Second time falls over !
	TRAP(error, AssignSpeedDialL( KSpeedDialContact, KSpeedDialPositionOne, *database ) );
	g_test(error == KErrNone);

	g_test.Next(_L("Open contact, remove from speed dial, open again"));
	// The remove from speed dial on the open contact should return KErrInUse.
	// The second open should also return KErrInUse.
	TRAP(error, RemoveSpeedDialWhenOpenL(KSpeedDialContact, KSpeedDialPositionOne, *database));
	g_test(error == KErrNone);

	CleanupStack::PopAndDestroy( database );

	// wait until Contacts Lock Server shuts down
	User::After(6000000);

	g_test.Next( _L("Try to open db again"));

	database =  CContactDatabase::OpenL(KDbFileName);
	CleanupStack::PushL(database);
	
	CContactItem* item = database->ReadContactL(KSpeedDialContact);
	CleanupStack::PushL(item);
	
	g_test.Next( _L("Check that Speed dial is remembered"));
	g_test(item->Id() == KSpeedDialContact);
	// retored item should be a SpeedDial
	g_test(ContactIsASpeedDial( *item ));

	CleanupStack::PopAndDestroy(item);
	CleanupStack::PopAndDestroy(database);
	}

/**

@SYMTestCaseID PIM-T-SPEEDDIALTEST-0001

*/

void TestOneL()
	{
	g_test.Start(_L("@SYMTestCaseID:PIM-T-SPEEDDIALTEST-0001 Create Database with Speed Dials then Delete and Create database"));

	TestDatabaseWithSpeedDialsL();

	g_test.Next(_L("Delete database"));
	CContactDatabase::DeleteDatabaseL(KDbFileName);
	
	g_test.Next( _L("open database should fail"));
	CContactDatabase* database = NULL;

	TRAPD(openError, database =  CContactDatabase::OpenL(KDbFileName));
	g_test(openError == KErrNotFound && database == NULL);
	

	g_test.Next( _L("create replacement database"));
	database =  CContactDatabase::CreateL(KDbFileName);
	CleanupStack::PushL(database);

	CreateContactsL(*database);

	CContactItem* item = database->ReadContactL(KSpeedDialContact);
	CleanupStack::PushL(item);
	
	g_test.Next( _L("Check that Speed dial is forgotten"));
	g_test(item->Id() == KSpeedDialContact);
	// retored item should not be a SpeedDial
	g_test(!ContactIsASpeedDial( *item ));

	CleanupStack::PopAndDestroy(item);
	CleanupStack::PopAndDestroy(database);

	g_test.End();
	}

void TestTwoL()
	{
	g_test.Start(_L("Create Database with Speed Dials then Replace database"));

	TestDatabaseWithSpeedDialsL();

	g_test.Next(_L("Replace database"));
	CContactDatabase* database = ReplaceDatabaseAndCreateContactsLC();

	CContactItem* item = database->ReadContactL(KSpeedDialContact );	
	CleanupStack::PushL(item);
	
	g_test.Next(_L("Check that Speed dial is forgotten"));
	g_test(item->Id() == KSpeedDialContact);
	// retored item should not be a SpeedDial
	g_test(!ContactIsASpeedDial( *item ));

	CleanupStack::PopAndDestroy(item);
	CleanupStack::PopAndDestroy(database);

	g_test.End();
	}

void TestThreeL()
	{
	g_test.Start(_L("Check that Settings are saved - normal shutdown"));
	
    g_test.Next(_L("Setting up..."));
	CContactDatabase* database = SetupLC();

	g_test.Next( _L("Assign Speed Dial"));
	TInt error = KErrNone;
	TRAP(error, AssignSpeedDialL( KSpeedDialContact, KSpeedDialPositionOne, *database ) );
	g_test(error == KErrNone);
	
	g_test.Next( _L("Update Speed Dial"));
	error = KErrNone;
	TRAP(error, UpdateSpeedDialL( KSpeedDialContact, KSpeedDialPositionTwo, *database ) );
	g_test(error == KErrNone);
	
	g_test.Next( _L("Change Database Drive"));
	CContactDatabase::SetDatabaseDriveL(TDriveUnit(EDriveD),EFalse);
	
	CleanupStack::PopAndDestroy( database );

	g_test.Next(_L("Wait for Contact Server to shutdown"));
	// wait until Contacts Lock Server shuts down
	// applicabale only in the case of transient server.
	User::After(6000000);
	
	g_test.Next( _L("Try to open Database again"));

	database =  CContactDatabase::OpenL(KDbFileName);
	CleanupStack::PushL(database);
		
	TDriveUnit driveUnit;
	CContactDatabase::DatabaseDrive(driveUnit);
	g_test.Next( _L("Check that Database Drive is changed"));
	g_test(driveUnit==TDriveUnit(EDriveD));
	
	CContactItem* item = database->ReadContactL(KSpeedDialContact);
	CleanupStack::PushL(item);
	
	g_test.Next( _L("Check that Speed Dial is remembered"));
	g_test(item->Id() == KSpeedDialContact);
	// retored item should be a SpeedDial
	g_test(ContactIsASpeedDial( *item ));
	
	// reset Database Drive
	g_test.Next( _L("Reset Database Drive setting"));
	CContactDatabase::SetDatabaseDriveL(TDriveUnit(EDriveC),EFalse);

	CleanupStack::PopAndDestroy(item);
	CleanupStack::PopAndDestroy(database);
	
	g_test.End();
	}

void TestFourL()
	{
	g_test.Start(_L("Check that Settings are saved - killing ContactLockSrv"));
	
    g_test.Next(_L("Setting up..."));
	CLockServerTester* lockSrvTester = CLockServerTester::NewL();
	CleanupStack::PushL(lockSrvTester);
	CContactDatabase* database = SetupLC();
	
	g_test.Next( _L("Assign Speed Dial"));
	TInt error = KErrNone;
	TRAP(error, AssignSpeedDialL( KSpeedDialContact, KSpeedDialPositionOne, *database ) );
	g_test(error == KErrNone);
	
	g_test.Next( _L("Update Speed Dial"));
	error = KErrNone;
	TRAP(error, UpdateSpeedDialL( KSpeedDialContact, KSpeedDialPositionTwo, *database ) );
	g_test(error == KErrNone);
	
	g_test.Next( _L("Change Database Drive"));
	CContactDatabase::SetDatabaseDriveL(TDriveUnit(EDriveD),EFalse);
	
	CleanupStack::PopAndDestroy(database);
	
	// Kill ContactLockServer process
	g_test.Next( _L("Kill CntServer"));
	lockSrvTester->FindServerAndKillItL();
	CleanupStack::PopAndDestroy( lockSrvTester );

	g_test.Next( _L("Open Database"));
	database =  CContactDatabase::OpenL(KDbFileName);
	CleanupStack::PushL(database);
		
	TDriveUnit driveUnit;
	CContactDatabase::DatabaseDrive(driveUnit);
	g_test.Next( _L("Check that Database Drive is changed"));
	g_test(driveUnit==TDriveUnit(EDriveD));
	
	CContactItem* item = database->ReadContactL(KSpeedDialContact);
	CleanupStack::PushL(item);
	
	g_test.Next( _L("Check that Speed Dial is remembered"));
	g_test(item->Id() == KSpeedDialContact);
	// retored item should be a SpeedDial
	g_test(ContactIsASpeedDial( *item ));
	
	// reset Database Drive
	g_test.Next( _L("Reset Database Drive setting"));
	CContactDatabase::SetDatabaseDriveL(TDriveUnit(EDriveC),EFalse);

	CleanupStack::PopAndDestroy(item);
	CleanupStack::PopAndDestroy(database);
	
	g_test.End();		
	}
	
	
/**
@SYMTestCaseID PIM-T-SPEEDDIALTEST-INC097895-0001
@SYMTestType UT
@SYMTestPriority High
@SYMDEF INC097895
@SYMTestCaseDesc Pbk2 AIW: Speed dial is removed from incorrect entry 
 
@SYMTestActions
1. Add speed dials for contact A and B
2. Try to reset speed dial belonging to A by using B's id
3. Reset speed dials using the correct contacts
 
@SYMTestExpectedResults Speed dial is still active when reset is attempted by wrong contact
*/	
void TestFiveL()
    {
    g_test.Start(_L("@SYMTestCaseID:PIM-T-SPEEDDIALTEST-INC097895-0001 Checking that the correct contact resets the speed dial number"));
	
   	g_test.Next(_L("Setting up..."));
	CContactDatabase* database = SetupLC();

	g_test.Next(_L("First Speed dial assignment"));	
	TRAPD(error, AssignSpeedDialL( KSpeedDialContactA, KSpeedDialPositionOne, *database ) );
	g_test(error == KErrNone);
	
	// Check that a speed dial is assigned
	g_test(ContactIsASpeedDialL(KSpeedDialContactA, *database));

	g_test.Next(_L("Second Speed dial assignment"));	
	TRAP(error, AssignSpeedDialL( KSpeedDialContactB, KSpeedDialPositionTwo, *database ) );
	g_test(error == KErrNone);
	
	// Check that a speed dial is assigned
	g_test(ContactIsASpeedDialL(KSpeedDialContactB, *database));
		
	// Here, the wrong contact (B) is trying to reset the speed dial number
	// which is set by contact A. 
	g_test.Next(_L("Wrong contact - should be ignored"));	
	TRAP(error, RemoveSpeedDialL(KSpeedDialContactB, KSpeedDialPositionOne, *database));
	g_test(error == KErrNone);
		
	g_test.Next(_L("The first speed dial is still active"));	
	g_test(ContactIsASpeedDialL(KSpeedDialContactA, *database));		
	
	g_test.Next(_L("and the second speed dial is still active"));	
	g_test(ContactIsASpeedDialL(KSpeedDialContactB, *database));				
	
	g_test.Next(_L("Reset allowed by the correct contact"));	
	TRAP(error, RemoveSpeedDialL(KSpeedDialContactA, KSpeedDialPositionOne, *database));
	g_test(error == KErrNone);
	
	g_test.Next(_L("Check speed dial One has been reset"));	
	g_test(!ContactIsASpeedDialL(KSpeedDialContactA, *database));		
					
	TRAP(error, RemoveSpeedDialL(KSpeedDialContactB, KSpeedDialPositionTwo, *database));
	g_test(error == KErrNone);
	
	g_test.Next(_L("Check speed dial Two has been reset"));	
	g_test(!ContactIsASpeedDialL(KSpeedDialContactB, *database));		
	
	CleanupStack::PopAndDestroy(database);	
	
	g_test.End();		
    }


LOCAL_C void LongNumSpeedDialTestsL()
    {
	_LIT(KLongNumSpeedDialTest, "Long Phone Number Speed Dial Test");
	g_test.Start(KLongNumSpeedDialTest);

	// create default, empty database
	CContactDatabase* db = CContactDatabase::ReplaceL(KLongNumSpeedDialDbName);
	CleanupStack::PushL(db);
	
	// create a contact and add it to the db
	CContactItem* contact = CContactCard::NewLC();
	CContactItemField* fname = CContactItemField::NewLC(KStorageTypeText, 
			KUidContactFieldGivenName);
	_LIT(KFname, "James");
	fname->TextStorage()->SetTextL(KFname() );
	contact->AddFieldL(*fname);
	CleanupStack::Pop(fname);
	CContactItemField* phone = CContactItemField::NewLC(KStorageTypeText,
			KUidContactFieldPhoneNumber);
	_LIT(KPhoneNum,	"01234567890123456789012345678901234567890123456789012345678901234567890123456789"); // 80 chars
	phone->TextStorage()->SetTextL(KPhoneNum() );
	contact->AddFieldL(*phone);
	CleanupStack::Pop(phone);
	const TContactItemId KContactId = db->AddNewContactL(*contact);
	CleanupStack::PopAndDestroy(contact);
	contact = NULL;

	// retrieve contact and assign its number to speed dial #1
	contact = db->OpenContactL(KContactId);
	CleanupStack::PushL(contact);
	const TInt KSpeedDial1(1);
	const TInt KPhoneNumIndex = contact->CardFields().Find(KUidContactFieldPhoneNumber);
	db->SetFieldAsSpeedDialL(*contact, KPhoneNumIndex, KSpeedDial1);
	TBuf<100> speedDialNumberText;
	TContactItemId speedDialId = db->GetSpeedDialFieldL(KSpeedDial1, speedDialNumberText);
	_LIT(KOutputFormat, "retrieved speed dial id: %d;\nretrieved speed dial phone number: ...\n%S\n");
	g_test.Printf(KOutputFormat, speedDialId, &speedDialNumberText);
	db->CloseContactL(KContactId);
	
	// cleanup
	CleanupStack::PopAndDestroy(2, db);	// and contact
	CContactDatabase::DeleteDatabaseL(KLongNumSpeedDialDbName);

	// Since PDEF121954, long phone numbers set as speed dial are truncated to 
	// the length of KSpeedDialPhoneLength. So, we need to get the truncated 
	// version of the phone number for comparison with the speed dial value.
	TPtrC phoneNum(KPhoneNum().Mid(0, KSpeedDialPhoneLength));
	g_test(speedDialId == KContactId && speedDialNumberText.CompareC(phoneNum) == 0);
	g_test.End();
	g_test.Close();
    }



void DoTestL()
	{
	CTestRegister * TempFiles = CTestRegister::NewLC();
	TempFiles->RegisterL(KDbFileName, EFileTypeCnt);
	TempFiles->RegisterL(KLongNumSpeedDialDbName, EFileTypeCnt);
	TempFiles->RegisterL(KContactsModelIniFile);
	
	TestOneL();

	g_test.Next(_L(""));
	TestTwoL();

	// Added for DEF075241
	g_test.Next( _L(""));
	TestThreeL();

	g_test.Next( _L(""));
	TestFourL();
	// End of tests for DEF075241
		
	// INC097895 start
	g_test.Next(_L(""));
	TestFiveL();
	// INC097895 end

	// PDEF121954 start
	g_test.Next(_L(""));
	LongNumSpeedDialTestsL();
	// PDEF121954 end
	
	// Delete cntmodel.ini
	User::LeaveIfError( DeleteIniFile() );	
	CleanupStack::PopAndDestroy(TempFiles);

	g_test.End();
	}


//
// Main.
//

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	g_test.Start(_L("Speed dial test"));

	CActiveScheduler* scheduler=new CActiveScheduler;
	if (scheduler)
		{
		CActiveScheduler::Install(scheduler);
		CTrapCleanup* cleanup=CTrapCleanup::New();
		if (cleanup)
			{
			TRAPD(err,DoTestL() );
			g_test(err == KErrNone);
			delete cleanup;
			}
		delete scheduler;
		}
	g_test.Close();
	__UHEAP_MARKEND;
	return KErrNone;
    }
