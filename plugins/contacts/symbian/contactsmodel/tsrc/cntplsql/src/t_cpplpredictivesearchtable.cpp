/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Unit test class for testing 12-key predictive search table
*
*/

//  INTERNAL INCLUDES
#include "t_cpplpredictivesearchtable.h"
#include "pltables.h"
#include "dbsqlconstants.h"
#include "c12keykeymap.h"
#include "c12keypredictivesearchtable.h"
#include "cqwertypredictivesearchtable.h"
#include "cpredictivesearchsettingstable.h"
#include "cpredictivesearchsynchronizer.h"
#include "t_predictivesearchtabledefs.h"
#include "predictivesearchlog.h"

//  SYSTEM INCLUDES
#include <digia/eunit/eunitmacros.h>
#include <f32file.h> // CFileMan

// Used to create HbKeymapFactory singleton to get rid of resource leak
#include <QLocale>
#include <hbinputkeymapfactory.h>


// Must have same value as KMaxTokenLength in c12keypredictivesearchtable.cpp
const TInt KMaxTokenLength = 15;


const TInt KTableCount = 12;

// Must have same value as KConversionError in cpplpredictivesearchtable.cpp
const quint64 KConversionError = 0xeeeeeeeeeeeeeee;
    



// -----------------------------------------------------------------------------
// UT_CPplPredictiveSearchTable::NewL
// -----------------------------------------------------------------------------
//
UT_CPplPredictiveSearchTable* UT_CPplPredictiveSearchTable::NewL()
    {
    UT_CPplPredictiveSearchTable* self = UT_CPplPredictiveSearchTable::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// UT_CPplPredictiveSearchTable::NewLC
// -----------------------------------------------------------------------------
//
UT_CPplPredictiveSearchTable* UT_CPplPredictiveSearchTable::NewLC()
    {
    UT_CPplPredictiveSearchTable* self = new( ELeave ) UT_CPplPredictiveSearchTable();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// UT_CPplPredictiveSearchTable::~UT_CPplPredictiveSearchTable
// -----------------------------------------------------------------------------
//
UT_CPplPredictiveSearchTable::~UT_CPplPredictiveSearchTable()
    {
    delete iTable;
    iTable = NULL;
    
    iDB.Close(); // Must close DB before it can be deleted
    RSqlDatabase::Delete(KDBFile);
    }

// -----------------------------------------------------------------------------
// UT_CPplPredictiveSearchTable::UT_CPplPredictiveSearchTable
// -----------------------------------------------------------------------------
//
UT_CPplPredictiveSearchTable::UT_CPplPredictiveSearchTable()
    {
    }

// -----------------------------------------------------------------------------
// UT_CPplPredictiveSearchTable::ConstructL
// -----------------------------------------------------------------------------
//
void UT_CPplPredictiveSearchTable::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    
#if defined(USE_ORBIT_KEYMAP)
    // Create singleton outside actual test cases so that it is not treated as
    // resource leak, since it can't be deleted.
    HbKeymapFactory::instance();
#else
    // If Orbit keymap is not used, EUnit shows there is a resource leak in the
    // first test case of UT_CPplPredictiveSearchTable that writes something to
    // the database.
    // To get rid of the resource leak, the following code is here to create DB,
    // and one contact, delete it, delete DB. 
    SetupL();
    UT_DeleteLL(); // Adds and deletes 1 contact
    Teardown();
#endif
    }
    
// -----------------------------------------------------------------------------
// UT_CPplPredictiveSearchTable::SetupL
// Must start with an empty DB file for each test case
// -----------------------------------------------------------------------------
//
void UT_CPplPredictiveSearchTable::SetupL()
    {
    // Ignore error
	RSqlDatabase::Delete(KDBFile);
    // If this causes KErrAlreadyExists, iDB won't be fully constructed
	iDB.Create(KDBFile);

	iTable = C12keyPredictiveSearchTable::NewL(iDB);
	iPredSearchQwertyTable = CQwertyPredictiveSearchTable::NewL(iDB);
	iPredSearchSettingsTable = CPredictiveSearchSettingsTable::NewL(iDB);
	// Create (empty) predictive search tables to DB
	iTable->CreateTableL();
	iPredSearchQwertyTable->CreateTableL();
    iPredSearchSettingsTable->CreateTableL();
    
    iPredictiveSearchSynchronizer =
        CPredictiveSearchSynchronizer::NewL(iDB,
                                            *iTable,
                                            *iPredSearchQwertyTable,
                                            *iPredSearchSettingsTable);  
#if defined(USE_ORBIT_KEYMAP)
    HbKeymapFactory::instance();
#endif
	}
    
// -----------------------------------------------------------------------------
// UT_CPplPredictiveSearchTable::SetupSyncL
// For synchronize tables test case
// -----------------------------------------------------------------------------
//
void UT_CPplPredictiveSearchTable::SetupSyncL()
    {
	UseSpecificDbL(KDBFileWithoutPredSearch);
    }

// -----------------------------------------------------------------------------
// UT_CPplPredictiveSearchTable::SetupSyncJust12keyExistsL
// For synchronize tables test case
// -----------------------------------------------------------------------------
//
void UT_CPplPredictiveSearchTable::SetupSyncJust12keyExistsL()
    {
	UseSpecificDbL(KDBFile12keyButNoQwerty);
    }

// -----------------------------------------------------------------------------
// UT_CPplPredictiveSearchTable::SetupLanguageChangesL
// For UT_LanguageChangesL case
// -----------------------------------------------------------------------------
//
void UT_CPplPredictiveSearchTable::SetupLanguageChangesL()
    {
	UseSpecificDbL(KDBFileOtherLanguage);
    }

// -----------------------------------------------------------------------------
// UT_CPplPredictiveSearchTable::UseSpecificDbL
// -----------------------------------------------------------------------------
//
void UT_CPplPredictiveSearchTable::UseSpecificDbL(const TDesC& aDbFile)
    {
    // Copy an existing DB file that does not contain predictive search tables
    // to a DB file that will be used in test.
    RFs fs;
    User::LeaveIfError(fs.Connect());
    CFileMan* fm = CFileMan::NewL(fs);
    CleanupStack::PushL(fm);

    User::LeaveIfError(fm->Copy(aDbFile, KDBFile));
    CleanupStack::PopAndDestroy(fm);
    fs.Close();
    
    
    User::LeaveIfError(iDB.Open(KDBFile));

    iTable = C12keyPredictiveSearchTable::NewL(iDB);
    iPredSearchQwertyTable = CQwertyPredictiveSearchTable::NewL(iDB);
	iPredSearchSettingsTable = CPredictiveSearchSettingsTable::NewL(iDB);

    iPredictiveSearchSynchronizer =
        CPredictiveSearchSynchronizer::NewL(iDB,
                                            *iTable,
                                            *iPredSearchQwertyTable,
                                            *iPredSearchSettingsTable);
    }

// -----------------------------------------------------------------------------
// UT_CPplPredictiveSearchTable::Teardown
// -----------------------------------------------------------------------------
//
void UT_CPplPredictiveSearchTable::Teardown()
    {
    delete iPredictiveSearchSynchronizer;
    iPredictiveSearchSynchronizer = NULL;

    delete iTable;
	iTable = NULL;
	
	delete iPredSearchQwertyTable;
	iPredSearchQwertyTable = NULL;

	delete iPredSearchSettingsTable;
	iPredSearchSettingsTable = NULL;

	iDB.Close(); // Must close DB before it can be deleted
	RSqlDatabase::Delete(KDBFile);
    }


// TEST CASES

// Dummy case to see if the first case always results a resource leak (2 heap cells)
void UT_CPplPredictiveSearchTable::UT_DummyL()
    {    
    // The first test case that writes to tables, seems to cause resource leak
    // if the DB file did not have predictive search tables already.
    AddContactL(KTestFirstName, KNullDesC, KTestContactId);
    }

// -----------------------------------------------------------------------------
// UT_CPplPredictiveSearchTable::UT_CreateInDbLL
// -----------------------------------------------------------------------------
//
void UT_CPplPredictiveSearchTable::UT_CreateInDbLL()
    {
    CheckItemCountL(); // all empty
    AddContactL(KTestFirstName, KTestLastName, KTestContactId);
    CheckItemCountL(0, 1, 0, 0, 1); // Tables 1 and 4 have one entry
    }

// -----------------------------------------------------------------------------
// UT_CPplPredictiveSearchTable::UT_CreateInDbManyContactsL
// -----------------------------------------------------------------------------
//
void UT_CPplPredictiveSearchTable::UT_CreateInDbManyContactsL()
    {
	AddContactL(_L(" 1 22 333 4"), _L("  505 6768"), KTestContactId);
	// Only 4 tokens are stored: first two LN and first two FN. 
	CheckItemCountL(0, 1, 1, 0, 0, 1, 1);

	
	AddContactL(_L(" 1 22 333 4"), _L("777  505 6768"), KTestContactId2);
	// Adds contact to tables 1 2 7 5
	CheckItemCountL(0, 2, 2, 0, 0, 2, 1, 1);

	
	AddContactL(_L(" 1 22 333 4"), _L("  5 6"), KTestContactId3);
	// Adds contact to tables 1 2 5 6
	CheckItemCountL(0, 3, 3, 0, 0, 3, 2, 1);

	
	AddContactL(_L("858585"), _L("88"), KTestContactId4);
	// Adds contact to table 8
	CheckItemCountL(0, 3, 3, 0, 0, 3, 2, 1, 1);

	
	TContactItemId id = KTestContactId4;
	AddContactL(KNullDesC, KNullDesC, ++id);
	// Contact is not added to any table
	CheckItemCountL(0, 3, 3, 0, 0, 3, 2, 1, 1);

	
	AddContactL(_L("1 1 1 1 1 1 1"), KNullDesC, ++id);
	// Adds contact to table 1
	CheckItemCountL(0, 4, 3, 0, 0, 3, 2, 1, 1);

	
	AddContactL(KNullDesC, _L("  22 22 2 2 222222"), ++id);
	// Adds contact to table 2
	CheckItemCountL(0, 4, 4, 0, 0, 3, 2, 1, 1);
	}

// -----------------------------------------------------------------------------
// UT_CPplPredictiveSearchTable::UT_CreateInDbWithHashAndStarL
// -----------------------------------------------------------------------------
//
void UT_CPplPredictiveSearchTable::UT_CreateInDbWithHashAndStarL()
    {
	AddContactL(_L(" # * +"), _L(" +*# **"), KTestContactId);
	AddContactL(_L("*"), _L("+355"), KTestContactId2); 
    CheckItemCountL(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1);
	}

// -----------------------------------------------------------------------------
// UT_CPplPredictiveSearchTable::UT_UpdateLL
// Update an existing contact
// -----------------------------------------------------------------------------
//
void UT_CPplPredictiveSearchTable::UT_UpdateLL()
    {
    _LIT(KTestUpdatedName, "0581");
    
    // Add one contact to DB
    AddContactL(KTestFirstName, KTestLastName, KTestContactId);

    // Update some field
    TInt id = KUidContactCardValue;
    TUid uid;
    uid.iUid = id;
    CContactItem* contact = CContactItem::NewLC(uid);
    
    // Only first name is changed, but also the last name must be filled to
    // CContactItemField.
    CContactItemField* field =
        CContactItemField::NewL(KStorageTypeText, KUidContactFieldGivenName);
    field->TextStorage()->SetTextL(KTestUpdatedName);
    contact->AddFieldL(*field); // Takes ownership
    field = NULL;
    
    field = CContactItemField::NewL(KStorageTypeText, KUidContactFieldFamilyName);
    field->TextStorage()->SetTextL(KTestLastName);  
    contact->AddFieldL(*field); // Takes ownership

	contact->SetId(KTestContactId);

    iTable->UpdateL(*contact);
    CleanupStack::PopAndDestroy(contact);

    CheckItemCountL(1, 0, 0, 0, 1); // tables 0 and 4 have one entry
    
    RArray<TInt32> results = DoPredictiveSearchL(KTestUpdatedName);
    CleanupClosePushL(results);
    EUNIT_ASSERT_EQUALS(1, results.Count());
    EUNIT_ASSERT_EQUALS(KTestContactId, results[0]);
    CleanupStack::PopAndDestroy(&results);
    

    RArray<TInt32> results2 = DoPredictiveSearchL(KTestFirstName);
    CleanupClosePushL(results2);
    EUNIT_ASSERT_EQUALS(0, results.Count());
    CleanupStack::PopAndDestroy(&results2);
    }

// -----------------------------------------------------------------------------
// UT_CPplPredictiveSearchTable::UT_UpdateLBothFieldsL
// Update an existing contact
// -----------------------------------------------------------------------------
//
void UT_CPplPredictiveSearchTable::UT_UpdateLBothFieldsL()
    {
    _LIT(KTestUpdatedFirstName, "777");
    _LIT(KTestUpdatedLastName, "012345");
    
    // Add some contacts
    AddContactL(KTestFirstName, KTestLastName, KTestContactId);
    AddContactL(_L("9876"), _L("888"), KTestContactId2);
    AddContactL(_L("5050"), _L("2"), KTestContactId3);
    CheckItemCountL(0, 1, 1, 0, 1, 1, 0, 0, 1, 1);
    
    // Update FN and LN of second contact
    TInt id = KUidContactCardValue;
    TUid uid;
    uid.iUid = id;
    CContactItem* contact = CContactItem::NewLC(uid);
    
    // Only first name is changed, but also the last name must be filled to
    // CContactItemField.
    CContactItemField* field =
       CContactItemField::NewL(KStorageTypeText, KUidContactFieldGivenName);
    field->TextStorage()->SetTextL(KTestUpdatedFirstName);
    contact->AddFieldL(*field); // Takes ownership
    field = NULL;
    
    field = CContactItemField::NewL(KStorageTypeText, KUidContactFieldFamilyName);
    field->TextStorage()->SetTextL(KTestUpdatedLastName);  
    contact->AddFieldL(*field); // Takes ownership

    contact->SetId(KTestContactId2);

    iTable->UpdateL(*contact);
    CleanupStack::PopAndDestroy(contact);
    
    CheckItemCountL(1, 1, 1, 0, 1, 1, 0, 1, 0, 0);
    
    RArray<TInt32> results = DoPredictiveSearchL(KTestUpdatedFirstName);
    CleanupClosePushL(results);
    EUNIT_ASSERT_EQUALS(1, results.Count());
    EUNIT_ASSERT_EQUALS(KTestContactId2, results[0]);
    CleanupStack::PopAndDestroy(&results);
    
    
    RArray<TInt32> results2 = DoPredictiveSearchL(KTestUpdatedLastName);
    CleanupClosePushL(results2);
    EUNIT_ASSERT_EQUALS(1, results2.Count());
    EUNIT_ASSERT_EQUALS(KTestContactId2, results2[0]);
    CleanupStack::PopAndDestroy(&results2);
    }

// -----------------------------------------------------------------------------
// UT_CPplPredictiveSearchTable::UT_SearchL
// Search contacts from the table 
// -----------------------------------------------------------------------------
//
void UT_CPplPredictiveSearchTable::UT_SearchL()
    {
    // Create some contacts
    AddContactL(_L("123704"), _L("39"), KTestContactId);
    AddContactL(_L("908"), KNullDesC, KTestContactId2);
    AddContactL(KNullDesC, _L("967"), KTestContactId3);
    // Tables 0 and 2 are empty, since company name is not used
    CheckItemCountL(0, 1, 0, 1, 0, 0, 0, 0, 0, 2); 
    
    // One contact matches
    RArray<TContactItemId> results = DoPredictiveSearchL(_L("1"));
    CleanupClosePushL(results);
    EUNIT_ASSERT_EQUALS(1, results.Count());
    EUNIT_ASSERT_EQUALS(KTestContactId, results[0]);
    CleanupStack::PopAndDestroy(&results);
  
    // No matches
    RArray<TContactItemId> results2 = DoPredictiveSearchL(_L("124"));
    CleanupClosePushL(results2);
    EUNIT_ASSERT_EQUALS(0, results2.Count());
    CleanupStack::PopAndDestroy(&results2);


    // Two matches
    RArray<TContactItemId> results3 = DoPredictiveSearchL(_L("9"));
    CleanupClosePushL(results3);
    EUNIT_ASSERT_EQUALS(2, results3.Count());
    EUNIT_ASSERT_EQUALS(KTestContactId2, results3[0]);
    EUNIT_ASSERT_EQUALS(KTestContactId3, results3[1]);
    CleanupStack::PopAndDestroy(&results3);
    }

// -----------------------------------------------------------------------------
// UT_CPplPredictiveSearchTable::UT_SearchWithSpacesL
// Search contacts having spaces from the table 
// -----------------------------------------------------------------------------
//
void UT_CPplPredictiveSearchTable::UT_SearchWithSpacesL()
    {
    // Create some contacts
    
    // Goes to tables 8, 4 and 0
    AddContactL(_L(" 8075 4702 380"), _L(" 470 038"), KTestContactId);
    
    // Goes to tables 0 and 4
    AddContactL(_L(" 0 00"), _L(" 470 038  38"), KTestContactId2);
    
    // Goes to table 1 and 4
    AddContactL(KNullDesC, _L(" 123 47  "), KTestContactId3);
    CheckItemCountL(2, 1, 0, 0, 3, 0, 0, 0, 1, 0);
    
    // No matches
    RArray<TContactItemId> results = DoPredictiveSearchL(_L("2"));
    TInt count = results.Count();
    results.Close();
    EUNIT_ASSERT_EQUALS(0, count);
    
    
    RArray<TContactItemId> results2 = DoPredictiveSearchL(_L("07"));
    count = results2.Count();
    results2.Close();
    EUNIT_ASSERT_EQUALS(0, count);

    
    RArray<TContactItemId> results3 = DoPredictiveSearchL(_L("1230"));
    count = results3.Count();
    results3.Close();
    EUNIT_ASSERT_EQUALS(0, count);

    
    RArray<TContactItemId> results4 = DoPredictiveSearchL(_L("5047"));
    count = results4.Count();
    results4.Close();
    EUNIT_ASSERT_EQUALS(0, count);
    
    
    RArray<TContactItemId> results5 = DoPredictiveSearchL(_L("4700"));
    count = results5.Count();
    results5.Close();
    EUNIT_ASSERT_EQUALS(0, count);
    
    
    // One match
    RArray<TContactItemId> results6 = DoPredictiveSearchL(_L("00"));
    CleanupClosePushL(results6);
    EUNIT_ASSERT_EQUALS(1, results6.Count());
    EUNIT_ASSERT_EQUALS(KTestContactId2, results6[0]);
    CleanupStack::PopAndDestroy(&results6);
    
    
    RArray<TContactItemId> results7 = DoPredictiveSearchL(_L("4702"));
    CleanupClosePushL(results7);
    EUNIT_ASSERT_EQUALS(1, results7.Count());
    EUNIT_ASSERT_EQUALS(KTestContactId, results7[0]);
    CleanupStack::PopAndDestroy(&results7);

 
    // Two matches
    RArray<TContactItemId> results8 = DoPredictiveSearchL(_L("470"));
    CleanupClosePushL(results8);
    EUNIT_ASSERT_EQUALS(2, results8.Count());
    EUNIT_ASSERT_EQUALS(KTestContactId, results8[0]);
    EUNIT_ASSERT_EQUALS(KTestContactId2, results8[1]);
    CleanupStack::PopAndDestroy(&results8);
    
    
    RArray<TContactItemId> results9 = DoPredictiveSearchL(_L("0"));
    CleanupClosePushL(results9);
    EUNIT_ASSERT_EQUALS(2, results9.Count());
    EUNIT_ASSERT_EQUALS(KTestContactId, results9[0]);
    EUNIT_ASSERT_EQUALS(KTestContactId2, results9[1]);
    CleanupStack::PopAndDestroy(&results9);
    

    RArray<TContactItemId> results10 = DoPredictiveSearchL(_L("038"));
    CleanupClosePushL(results10);
    EUNIT_ASSERT_EQUALS(2, results10.Count());
    EUNIT_ASSERT_EQUALS(KTestContactId, results10[0]);
    EUNIT_ASSERT_EQUALS(KTestContactId2, results10[1]);
    CleanupStack::PopAndDestroy(&results10);
    
    
    // Three matches
    RArray<TContactItemId> results11 =  DoPredictiveSearchL(_L("47"));
    CleanupClosePushL(results11);
    EUNIT_ASSERT_EQUALS(3, results11.Count());
    CleanupStack::PopAndDestroy(&results11);
    }
    
// -----------------------------------------------------------------------------
// UT_CPplPredictiveSearchTable::UT_DeleteLL
// Delete a the only contact from the table 
// -----------------------------------------------------------------------------
//
void UT_CPplPredictiveSearchTable::UT_DeleteLL()
    {
    AddContactL(KTestFirstName, KTestLastName, KTestContactId);
    CheckItemCountL(0, 1, 0, 0, 1, 0, 0, 0, 0, 0);
    
    TInt id = KUidContactCardValue;
    TUid uid;
    uid.iUid = id;
    CContactItem* contact = CContactItem::NewLC(uid);
    contact->SetId(KTestContactId);

    TBool lowDiskErrorOccurred(EFalse);
    iTable->DeleteL(*contact, lowDiskErrorOccurred);
    CleanupStack::PopAndDestroy(contact);
    CheckItemCountL();
    }

// -----------------------------------------------------------------------------
// UT_CPplPredictiveSearchTable::UT_DeleteContactsL
// Delete contacts one-by-one from the table that has multiple contacts
// -----------------------------------------------------------------------------
//
void UT_CPplPredictiveSearchTable::UT_DeleteContactsL()
    {
    _LIT(KTestFN, "7 8 ");
    _LIT(KTestLN, "  4  56  1  ");
    TContactItemId contactId = KTestContactId4 + 1;
	// Goes to tables 1 and 4
    AddContactL(KTestFirstName, KTestLastName, KTestContactId);
	// Goes to tables 1 and 4
    AddContactL(KTestFirstName, KTestLastName, KTestContactId2);
	// Goes to table 4
    AddContactL(KNullDesC, KTestLastName, KTestContactId3);
	// This contact does not go into any table
    AddContactL(KNullDesC, KNullDesC, KTestContactId4);
	// Goes to tables 4, 5, 7 and 8 
    AddContactL(KTestFN, KTestLN, contactId);
    CheckItemCountL(0, 2, 0, 0, 4, 1, 0, 1, 1, 0);
    
    TInt id = KUidContactCardValue;
    TUid uid;
    uid.iUid = id;
    CContactItem* contact = CContactItem::NewLC(uid);
    contact->SetId(KTestContactId3);

    TBool lowDiskErrorOccurred(EFalse);
    iTable->DeleteL(*contact, lowDiskErrorOccurred);
    CleanupStack::PopAndDestroy(contact);
    contact = NULL;
    // Contact was removed from table 1
    CheckItemCountL(0, 2, 0, 0, 3, 1, 0, 1, 1, 0);
    
    
    contact = CContactItem::NewLC(uid);
    contact->SetId(KTestContactId4);

    iTable->DeleteL(*contact, lowDiskErrorOccurred);
    CleanupStack::PopAndDestroy(contact);
    contact = NULL;
    // This contact was not in any table in the first place, so no changes
    CheckItemCountL(0, 2, 0, 0, 3, 1, 0, 1, 1, 0);
    
    
    contact = CContactItem::NewLC(uid);
    contact->SetId(KTestContactId);
    
    iTable->DeleteL(*contact, lowDiskErrorOccurred);
    CleanupStack::PopAndDestroy(contact);
    contact = NULL;
    // Contact was removed from tables 1 and 4
    CheckItemCountL(0, 1, 0, 0, 2, 1, 0, 1, 1, 0);

    
    contact = CContactItem::NewLC(uid);
    contact->SetId(KTestContactId2);
    
    iTable->DeleteL(*contact, lowDiskErrorOccurred);
    CleanupStack::PopAndDestroy(contact);
    contact = NULL;
    
    // Contact was removed from tables 1 and 4
    CheckItemCountL(0, 0, 0, 0, 1, 1, 0, 1, 1, 0);
    
    
    contact = CContactItem::NewLC(uid);
    contact->SetId(contactId);
    
    iTable->DeleteL(*contact, lowDiskErrorOccurred);
    CleanupStack::PopAndDestroy(contact);
    CheckItemCountL(); // all empty
    }

// -----------------------------------------------------------------------------
// UT_CPplPredictiveSearchTable::UT_DeleteNonexistingContactL
// Try to delete a contact that does not existing in the table
// -----------------------------------------------------------------------------
//
void UT_CPplPredictiveSearchTable::UT_DeleteNonexistingContactL()
    {
    AddContactL(KTestFirstName, KTestLastName, KTestContactId);
    CheckItemCountL(0, 1, 0, 0, 1, 0, 0, 0, 0, 0);
    
    TInt id = KUidContactCardValue;
    TUid uid;
    uid.iUid = id;
    CContactItem* contact = CContactItem::NewLC(uid);
    contact->SetId(KTestContactId2);
    
    TBool lowDiskErrorOccurred(EFalse);
    // Note: DeleteL does not leave if the contact could not be deleted
    iTable->DeleteL(*contact, lowDiskErrorOccurred);
    CleanupStack::PopAndDestroy(contact);
    CheckItemCountL(0, 1, 0, 0, 1, 0, 0, 0, 0, 0);
    }

// -----------------------------------------------------------------------------
// UT_CPplPredictiveSearchTable::UT_CheckIfTableExistsL
// Predictive search table does not exist in the database.
// -----------------------------------------------------------------------------
//
void UT_CPplPredictiveSearchTable::UT_CheckIfTableExistsL()
    {
    // Delete DB that was created in setup()
    iDB.Close(); // Must close DB before it can be deleted
    RSqlDatabase::Delete(KDBFile);

    // Create DB, but do not create pred search table in it
    iDB.Create(KDBFile);

    EUNIT_ASSERT_EQUALS(EFalse,
        iPredictiveSearchSynchronizer->CheckIfPredSearchTableExistsL(KSqlContactPredSearchTable0));

    iDB.Close(); // Must close DB before it can be deleted
    RSqlDatabase::Delete(KDBFile);
    }

// -----------------------------------------------------------------------------
// UT_CPplPredictiveSearchTable::UT_CheckIfTableExists2L
// Predictive search table exists in the database.
// -----------------------------------------------------------------------------
//
void UT_CPplPredictiveSearchTable::UT_CheckIfTableExists2L()
    {
    EUNIT_ASSERT_EQUALS(ETrue,
        iPredictiveSearchSynchronizer->CheckIfPredSearchTableExistsL(KSqlContactPredSearchTable0));
    }


// IMPORTANT NOTE:
// If this case fails or a resource leak is reported on it, make sure
// KDBFileWithoutPredSearch file exists, and it does not contain the predictive
// search tables!
// If Orbit keymap is used, this test case leaks resources,
// otherwise it does not leak.
void UT_CPplPredictiveSearchTable::UT_SynchronizeTableL()
    {
	iPredictiveSearchSynchronizer->SynchronizeTablesL();
    }

void UT_CPplPredictiveSearchTable::UT_SynchronizeTableJust12keyExistsL()
	{
	iPredictiveSearchSynchronizer->SynchronizeTablesL();
	}

void UT_CPplPredictiveSearchTable::UT_DeleteTablesL()
	{
	// Delete tables
	iPredictiveSearchSynchronizer->DeletePredSearchTablesL();

	// Check tables have been deleted
	EUNIT_ASSERT_EQUALS(EFalse,
        iPredictiveSearchSynchronizer->CheckIfPredSearchTableExistsL(KSqlContactPredSearchTable0));

	// Try to delete tables when they do not exist
	iPredictiveSearchSynchronizer->DeletePredSearchTablesL();
	}

void UT_CPplPredictiveSearchTable::UT_LanguageChangesL()
	{
	iPredictiveSearchSynchronizer->SynchronizeTablesL();
	}

void UT_CPplPredictiveSearchTable::UT_TokenizeNamesL()
    {
    _LIT(KFirstNames, "99 12345 234567 987 99");
    _LIT(KLastNames, "12355 987 402");
    HBufC* fn = KFirstNames().AllocLC();
    HBufC* ln = KLastNames().AllocLC();
    QStringList emptyList;

    // This constant must have same value as declared in cpplpredictivesearchtable.cpp
    const TInt KMaxTokens = 4; 
    QStringList tokens = iTable->GetTokens(emptyList, fn, ln);
    EUNIT_ASSERT_EQUALS(KMaxTokens, tokens.count());
    tokens.clear();
        
    tokens = iTable->GetTokens(emptyList, NULL, ln);
    EUNIT_ASSERT_EQUALS(3, tokens.count());
    tokens.clear();
    
    tokens = iTable->GetTokens(emptyList, fn, NULL);
    EUNIT_ASSERT_EQUALS(4, tokens.count());
    tokens.clear();

    tokens = iTable->GetTokens(emptyList, NULL, NULL);
    EUNIT_ASSERT_EQUALS(0, tokens.count());
    tokens.clear();

    QStringList mailList;
    mailList << "mail.addr1";
    mailList << "mail.addr2";
    tokens = iTable->GetTokens(mailList, NULL, NULL);
    EUNIT_ASSERT_EQUALS(2, tokens.count());
    tokens.clear();
    
    tokens = iTable->GetTokens(mailList, fn, ln);
    EUNIT_ASSERT_EQUALS(4, tokens.count());
    tokens.clear();
    
    CleanupStack::PopAndDestroy(ln);
    CleanupStack::PopAndDestroy(fn);
    }
    
// Write contacts with and without FN & LN.
void UT_CPplPredictiveSearchTable::UT_WriteToDbL()
    {
	const TContactItemId KTestContactId5 = 1024;
	const TContactItemId KTestContactId6 = 1025;

	// Just FN
	AddContactL(KTestFirstName, KNullDesC, KTestContactId);
	// Just LN
	AddContactL(KNullDesC, KTestLastName, KTestContactId2);
	// Both FN and LN
	AddContactL(KTestFirstName, KTestLastName, KTestContactId3);
	// Neither FN and LN
	AddContactL(KNullDesC, KNullDesC, KTestContactId4);
	
	// Long names
	_LIT(KTooLongName, "abcdefghijklmnopqrstuwvxyz aabbccddeeffgghhiijjkkllmm");
	AddContactL(KTooLongName, KTestLastName, KTestContactId5);
    AddContactL(KTestFirstName, KTooLongName, KTestContactId6);
    }

void UT_CPplPredictiveSearchTable::UT_ConvertToHexL()
	{
	// Basic case
	EUNIT_ASSERT_EQUALS(0x4458fffffffffff, iTable->ConvertToHex("4458"));
	
	// Long digit
	EUNIT_ASSERT_EQUALS(0x123456789012345, iTable->ConvertToHex("123456789012345"));

	// Trailing zeros
	EUNIT_ASSERT_EQUALS(0x12345678900ffff, iTable->ConvertToHex("12345678900"));

	// Leading zeros
	EUNIT_ASSERT_EQUALS(0x00123456789ffff, iTable->ConvertToHex("00123456789"));

	// Just zeros
	EUNIT_ASSERT_EQUALS(0x00000ffffffffff, iTable->ConvertToHex("00000"));
	EUNIT_ASSERT_EQUALS(0x00000000000000f, iTable->ConvertToHex("00000000000000"));
	EUNIT_ASSERT_EQUALS(0x000000000000000, iTable->ConvertToHex("000000000000000"));
	EUNIT_ASSERT_EQUALS(0x0ffffffffffffff, iTable->ConvertToHex("0"));

	// Zeros in the middle
	EUNIT_ASSERT_EQUALS(0x12300450008000f, iTable->ConvertToHex("12300450008000"));

	// Empty string
	EUNIT_ASSERT_EQUALS(0xfffffffffffffff, iTable->ConvertToHex(""));
	
	// Unmapped characters
	EUNIT_ASSERT_EQUALS(0x123ffffffffffff, iTable->ConvertToHex("123??45??67"));
	EUNIT_ASSERT_EQUALS(0x00fffffffffffff, iTable->ConvertToHex("00?1234567"));
	EUNIT_ASSERT_EQUALS(0xfffffffffffffff, iTable->ConvertToHex("?1234567"));
	EUNIT_ASSERT_EQUALS(0xfffffffffffffff, iTable->ConvertToHex("???"));
	
	// Too many digits
	EUNIT_ASSERT_EQUALS(KConversionError, iTable->ConvertToHex("12345678901234567890"));
	}

void UT_CPplPredictiveSearchTable::AddContactL(const TDesC& aFirstName,
                                               const TDesC& aLastName,
                                               TContactItemId aContactId)
    {
    TInt id = KUidContactCardValue; // Defined by macro, so lacks type
    TUid uid;
    uid.iUid = id;
    CContactItem* contact = CContactItem::NewLC(uid);

    // Fill contact fields before writing to DB
    if (aFirstName.Length() > 0)
        {
        CContactItemField* field =
            CContactItemField::NewL(KStorageTypeText, KUidContactFieldGivenName);
        CContactTextField* textfield = field->TextStorage();
        CleanupStack::PushL(field);
        textfield->SetTextL(aFirstName);
        contact->AddFieldL(*field); // Takes ownership
        CleanupStack::Pop(field);
        }
    
    if (aLastName.Length() > 0)
        {
        CContactItemField* field =
            CContactItemField::NewL(KStorageTypeText, KUidContactFieldFamilyName);
        CleanupStack::PushL(field);
        field->TextStorage()->SetTextL(aLastName);
        contact->AddFieldL(*field); // Takes ownership
        CleanupStack::Pop(field);
        }

    contact->SetId(aContactId);
    
    iTable->CreateInDbL(*contact);
    CleanupStack::PopAndDestroy(contact);
    }

void UT_CPplPredictiveSearchTable::CheckItemCountL(
    TInt aCountInTable0,
    TInt aCountInTable1,
    TInt aCountInTable2,
    TInt aCountInTable3,
    TInt aCountInTable4,
    TInt aCountInTable5,
    TInt aCountInTable6,
    TInt aCountInTable7,
    TInt aCountInTable8,
    TInt aCountInTable9,
    TInt aCountInTable10,
    TInt aCountInTable11)
    {
    TPtrC tableNames[KTableCount] =
        {
        KSqlContactPredSearchTable0,
        KSqlContactPredSearchTable1,
        KSqlContactPredSearchTable2,
        KSqlContactPredSearchTable3,
        KSqlContactPredSearchTable4,
        KSqlContactPredSearchTable5,
        KSqlContactPredSearchTable6,
        KSqlContactPredSearchTable7,
        KSqlContactPredSearchTable8,
        KSqlContactPredSearchTable9,
        KSqlContactPredSearchTable10,
        KSqlContactPredSearchTable11
        };
    TInt rowCounts[KTableCount] = {0};
    
    for (TInt i = 0; i < KTableCount; ++i)
        {
        HBufC* s = HBufC::NewLC(KCountSelect().Length() +
            // Enough space for longest table name
            KSqlContactPredSearchTable11().Length());
        TPtr ptr = s->Des();
        ptr.Format(KCountSelect, &tableNames[i]);

        TSqlScalarFullSelectQuery scalarQuery(iDB);
    
        rowCounts[i] = scalarQuery.SelectIntL(ptr);
        CleanupStack::PopAndDestroy(s);
        }

    EUNIT_ASSERT_EQUALS(aCountInTable0, rowCounts[0]);
    EUNIT_ASSERT_EQUALS(aCountInTable1, rowCounts[1]);
    EUNIT_ASSERT_EQUALS(aCountInTable2, rowCounts[2]);
    EUNIT_ASSERT_EQUALS(aCountInTable3, rowCounts[3]);
    EUNIT_ASSERT_EQUALS(aCountInTable4, rowCounts[4]);
    EUNIT_ASSERT_EQUALS(aCountInTable5, rowCounts[5]);
    EUNIT_ASSERT_EQUALS(aCountInTable6, rowCounts[6]);
    EUNIT_ASSERT_EQUALS(aCountInTable7, rowCounts[7]);
    EUNIT_ASSERT_EQUALS(aCountInTable8, rowCounts[8]);
    EUNIT_ASSERT_EQUALS(aCountInTable9, rowCounts[9]);
    EUNIT_ASSERT_EQUALS(aCountInTable10, rowCounts[10]);
    EUNIT_ASSERT_EQUALS(aCountInTable11, rowCounts[11]);
    }

// There is only need to search from one table (unless search string begins
// with a zero).
// E.g. if search string is "102", the results are both in tables 1 and 2
// and it does not matter to which one the search is made.
// E.g. "0102", search from table 0 (for exact match "0102",and from 
// table 1 or 2 (for something that begins with 1 and 2)
RArray<TContactItemId>
UT_CPplPredictiveSearchTable::DoPredictiveSearchL(const TDesC& aSearchString)
    {
	RArray<TContactItemId> foundContactIds;
	if (aSearchString.Length() == 0)
		{
		return foundContactIds;
		}

	const TPtrC& tableName = DetermineTableName(aSearchString);
	HBufC* select(NULL);
	if (aSearchString.Length() == 1)
		{
		// No need to use "WHERE.." or "ORDER BY first_name ASC"
		_LIT(KSearchOneDigitFormat, "SELECT contact_id FROM %S");
		select = HBufC::NewLC(KSearchOneDigitFormat().Length() +
							  KSqlContactPredSearchTable11().Length());
		select->Des().AppendFormat(KSearchOneDigitFormat, &tableName);
		}
	else
		{
		// This does not yet support zero in the middle
		TInt64 lowerLimit = LowerLimitL(aSearchString) - 1;
		TInt64 upperLimit = UpperLimitL(aSearchString) + 1;
		// Write limits as decimal numbers
		_LIT(KSearchFormat, "SELECT contact_id FROM %S WHERE \
(nbr>%ld AND nbr<%ld) OR (nbr2>%ld AND nbr2<%ld) OR (nbr3>%ld AND nbr3<%ld) OR (nbr4>%ld AND nbr4<%ld);");
		TInt KNbrColumns = 4;
		TInt KSpaceForLimits = KNbrColumns * 2 * (KMaxTokenLength + 2); // Two extra for decimal representation of max 15 hex digits
		select = HBufC::NewLC(KSearchFormat().Length() +
							  KSqlContactPredSearchTable11().Length() +
							  KSpaceForLimits);
		select->Des().AppendFormat(KSearchFormat, &tableName,
								   lowerLimit, upperLimit,
								   lowerLimit, upperLimit,
								   lowerLimit, upperLimit,
								   lowerLimit, upperLimit);
		}
	RSqlStatement stmnt;
	CleanupClosePushL(stmnt);
	stmnt.PrepareL(iDB, *select);

	// Fetch the list of any matching contact ids
	const TInt KContactIdIndex(0);
	TInt err(KErrNone);
	while ((err = stmnt.Next()) == KSqlAtRow)
		{
		foundContactIds.AppendL(stmnt.ColumnInt(KContactIdIndex));
		}

	if (err != KSqlAtEnd)
		{
		User::Leave(err);
		}
	CleanupStack::PopAndDestroy(&stmnt);
	CleanupStack::PopAndDestroy(select);
	return foundContactIds;
    }

const TDesC& UT_CPplPredictiveSearchTable::DetermineTableName(
	const TDesC& aSearchString)
	{
	switch (aSearchString[0])
		{
		case '0':
		    return KSqlContactPredSearchTable0;
		case '1':
			return KSqlContactPredSearchTable1;
		case '2':
			return KSqlContactPredSearchTable2;
		case '3':
			return KSqlContactPredSearchTable3;
		case '4':
			return KSqlContactPredSearchTable4;
		case '5':
			return KSqlContactPredSearchTable5;
		case '6':
			return KSqlContactPredSearchTable6;
		case '7':
			return KSqlContactPredSearchTable7;	
		case '8':
			return KSqlContactPredSearchTable8;
		case '9':
			return KSqlContactPredSearchTable9;
		case 'a':
		    return KSqlContactPredSearchTable10;
		case 'b':
		    return KSqlContactPredSearchTable11;
		default:
			return KNullDesC;
		}
	}

TInt64 UT_CPplPredictiveSearchTable::LowerLimitL(const TDesC& aString) const
	{
	return ConvertToNbrL(aString, '0');
	}

TInt64 UT_CPplPredictiveSearchTable::UpperLimitL(const TDesC& aString) const
	{
	return ConvertToNbrL(aString, 'f');
	}

TInt64 UT_CPplPredictiveSearchTable::ConvertToNbrL(const TDesC& aString,
												   TChar aPadChar) const
	{
	HBufC* nbrBuffer = HBufC::NewLC(KMaxTokenLength);
	TPtrC p = aString.Left(KMaxTokenLength);
	TPtr nbrPtr = nbrBuffer->Des();
	nbrPtr.Append(p);
	// Append pad chars until length is KMaxTokenLength
	while (nbrPtr.Length() < KMaxTokenLength)
		{
		nbrPtr.Append(aPadChar);
		}	
//    RDebug::Print(_L("UT_CPplPredictiveSearchTable::ConvertToNbrL padded '%S'"), nbrBuffer);
    
	TLex16 lex(*nbrBuffer);
	TInt64 nbrValue(0);
	User::LeaveIfError(lex.Val(nbrValue, EHex));
	CleanupStack::PopAndDestroy(nbrBuffer);

//	RDebug::Print(_L("UT_CPplPredictiveSearchTable::ConvertToNbrL result 0x%lx"), nbrValue);        
	return nbrValue;
	}


//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE(
    UT_CPplPredictiveSearchTable,
    "UT_CPplPredictiveSearchTable",
    "UNIT" )

EUNIT_TEST(
    "CreateInDbL - dummy case to check resource leak",
    "UT_CPplPredictiveSearchTable",
    "",
    "FUNCTIONALITY",
    SetupL, UT_DummyL, Teardown )
        
EUNIT_TEST(
    "CreateInDbL - test",
    "UT_CPplPredictiveSearchTable",
    "CreateInDbL",
    "FUNCTIONALITY",
    SetupL, UT_CreateInDbLL, Teardown )

EUNIT_TEST(
    "CreateInDbL - test add many contacts",
    "UT_CPplPredictiveSearchTable",
    "CreateInDbL",
    "FUNCTIONALITY",
    SetupL, UT_CreateInDbManyContactsL, Teardown )

EUNIT_TEST(
	"CreateInDbL - test contacts with *,+,# characters",
    "UT_CPplPredictiveSearchTable",
    "CreateInDbL",
    "FUNCTIONALITY",
    SetupL, UT_CreateInDbWithHashAndStarL, Teardown )
	
EUNIT_TEST(
    "UpdateL - test updating FN",
    "UT_CPplPredictiveSearchTable",
    "UpdateL",
    "FUNCTIONALITY",
    SetupL, UT_UpdateLL, Teardown )

EUNIT_TEST(
    "UpdateL - test updating FN and LN",
    "UT_CPplPredictiveSearchTable",
    "UpdateL",
    "FUNCTIONALITY",
    SetupL, UT_UpdateLBothFieldsL, Teardown )

EUNIT_TEST(
    "Search - test",
    "UT_CPplPredictiveSearchTable",
    "predictive search",
    "FUNCTIONALITY",
    SetupL, UT_SearchL, Teardown )

EUNIT_TEST(
    "Search with spaces - test",
    "UT_CPplPredictiveSearchTable",
    "predictive search",
    "FUNCTIONALITY",
    SetupL, UT_SearchWithSpacesL, Teardown )

EUNIT_TEST(
    "Delete the only contact - test",
    "UT_CPplPredictiveSearchTable",
    "DeleteL",
    "FUNCTIONALITY",
    SetupL, UT_DeleteLL, Teardown )

EUNIT_TEST(
    "Delete many contacts - test",
    "UT_CPplPredictiveSearchTable",
    "DeleteL",
    "FUNCTIONALITY",
    SetupL, UT_DeleteContactsL, Teardown )

EUNIT_TEST(
    "Delete non-existing contact - test",
    "UT_CPplPredictiveSearchTable",
    "DeleteL",
    "FUNCTIONALITY",
    SetupL, UT_DeleteNonexistingContactL, Teardown )

EUNIT_TEST(
    "Check if predictive search table exists (table does not exist)",
    "UT_CPplPredictiveSearchTable",
    "DoesPredSearchTableExistL",
    "FUNCTIONALITY",
    SetupL, UT_CheckIfTableExistsL, Teardown )

EUNIT_TEST(
    "Check if predictive search table exists (table exists)",
    "UT_CPplPredictiveSearchTable",
    "DoesPredSearchTableExistL",
    "FUNCTIONALITY",
    SetupL, UT_CheckIfTableExists2L, Teardown )

EUNIT_TEST(
    "Synchronize DB w/o pred.search tables",
    "UT_CPplPredictiveSearchTable",
    "CreatePredSearchTablesL",
    "FUNCTIONALITY",
    SetupSyncL, UT_SynchronizeTableL, Teardown )

EUNIT_TEST(
    "Synchronize DB with 12-key, but w/o QWERTY tables",
    "UT_CPplPredictiveSearchTable",
    "CreatePredSearchTablesL",
    "FUNCTIONALITY",
    SetupSyncJust12keyExistsL, UT_SynchronizeTableJust12keyExistsL, Teardown )

EUNIT_TEST(
    "Delete predictive search tables",
    "UT_CPplPredictiveSearchTable",
    "DeletePredSearchTablesL",
    "FUNCTIONALITY",
    SetupL, UT_DeleteTablesL, Teardown )

EUNIT_TEST(
	"Language changes: re-create QWERTY tables",
    "UT_CPplPredictiveSearchTable",
    "DeletePredSearchTablesL",
    "FUNCTIONALITY",
    SetupLanguageChangesL, UT_LanguageChangesL, Teardown )

EUNIT_TEST(
    "Tokenize names",
    "UT_CPplPredictiveSearchTable",
    "TokenizeNames",
    "FUNCTIONALITY",
    SetupL, UT_TokenizeNamesL, Teardown )

EUNIT_TEST(
    "Write to table",
    "UT_CPplPredictiveSearchTable",
    "WriteToDbL",
    "FUNCTIONALITY",
    SetupL, UT_WriteToDbL, Teardown )

EUNIT_TEST(
    "ConvertToHexL",
    "UT_CPplPredictiveSearchTable",
    "ConvertToHexL",
    "FUNCTIONALITY",
    SetupL, UT_ConvertToHexL, Teardown )

EUNIT_END_TEST_TABLE

//  END OF FILE
