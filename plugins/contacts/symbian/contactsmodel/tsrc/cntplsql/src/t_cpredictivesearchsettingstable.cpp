/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "t_cpredictivesearchsettingstable.h"
#include "cpredictivesearchsettingstable.h"
//#include "dbsqlconstants.h"
#include "t_predictivesearchtabledefs.h"

//  SYSTEM INCLUDES
#include <digia/eunit/eunitmacros.h>

// Used to create HbKeymapFactory singleton to get rid of resource leak
#include <QLocale>
#include <hbinputkeymapfactory.h>
    

// -----------------------------------------------------------------------------
// UT_CPredictiveSearchSettingsTable::NewL
// -----------------------------------------------------------------------------
//
UT_CPredictiveSearchSettingsTable* UT_CPredictiveSearchSettingsTable::NewL()
    {
    UT_CPredictiveSearchSettingsTable* self =
		UT_CPredictiveSearchSettingsTable::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// UT_CPredictiveSearchSettingsTable::NewLC
// -----------------------------------------------------------------------------
//
UT_CPredictiveSearchSettingsTable* UT_CPredictiveSearchSettingsTable::NewLC()
    {
    UT_CPredictiveSearchSettingsTable* self =
		new ( ELeave ) UT_CPredictiveSearchSettingsTable();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// UT_CPredictiveSearchSettingsTable::~UT_CPredictiveSearchSettingsTable
// -----------------------------------------------------------------------------
//
UT_CPredictiveSearchSettingsTable::~UT_CPredictiveSearchSettingsTable()
    {
    delete iTable;
    iTable = NULL;
    
    iDB.Close(); // Must close DB before it can be deleted
    RSqlDatabase::Delete(KDBFile);
    }

// -----------------------------------------------------------------------------
// UT_CPredictiveSearchSettingsTable::UT_CPredictiveSearchSettingsTable
// -----------------------------------------------------------------------------
//
UT_CPredictiveSearchSettingsTable::UT_CPredictiveSearchSettingsTable()
    {
    }

// -----------------------------------------------------------------------------
// UT_CPredictiveSearchSettingsTable::ConstructL
// -----------------------------------------------------------------------------
//
void UT_CPredictiveSearchSettingsTable::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }
    
// -----------------------------------------------------------------------------
// UT_CPredictiveSearchSettingsTable::SetupL
// Must start with an empty DB file for each test case
// -----------------------------------------------------------------------------
//
void UT_CPredictiveSearchSettingsTable::SetupL()
    {
    // Ignore error
	RSqlDatabase::Delete(KDBFile);
    // If this causes KErrAlreadyExists, iDB won't be fully constructed
	iDB.Create(KDBFile);

	iTable = CPredictiveSearchSettingsTable::NewL(iDB);
	// Create (empty) predictive search tables to DB
	iTable->CreateTableL();
	}
    
// -----------------------------------------------------------------------------
// UT_CPredictiveSearchSettingsTable::Teardown
// -----------------------------------------------------------------------------
//
void UT_CPredictiveSearchSettingsTable::Teardown()
    {
    delete iTable;
	iTable = NULL;

	iDB.Close(); // Must close DB before it can be deleted
	RSqlDatabase::Delete(KDBFile);
    }


// TEST CASES

// -----------------------------------------------------------------------------
// UT_CPredictiveSearchSettingsTable::UT_CreateTableLL
// -----------------------------------------------------------------------------
//
void UT_CPredictiveSearchSettingsTable::UT_CreateTableLL()
    {
	// Check that when starting w/o DB file, the settings table is created and
	// filled correctly.
    enum QLocale::Language currLang = iTable->GetCurrentLanguageL();
    enum QLocale::Language storedLang = iTable->ReadStoredLanguageL();
    EUNIT_ASSERT_EQUALS(currLang, storedLang);
    }

    
//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE(
    UT_CPredictiveSearchSettingsTable,
    "UT_CPredictiveSearchSettingsTable",
    "UNIT" )
        
EUNIT_TEST(
    "CreateInDbL - test",
    "UT_CPredictiveSearchSettingsTable",
    "CreateTableL",
    "FUNCTIONALITY",
    SetupL, UT_CreateTableLL, Teardown )

EUNIT_END_TEST_TABLE

//  END OF FILE
