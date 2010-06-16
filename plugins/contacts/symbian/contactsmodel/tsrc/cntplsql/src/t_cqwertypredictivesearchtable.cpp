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
#include "t_cqwertypredictivesearchtable.h"
#include "cqwertypredictivesearchtable.h"
#include "dbsqlconstants.h"
#include "cqwertykeymap.h"
#include "t_predictivesearchtabledefs.h"

//  SYSTEM INCLUDES
#include <digia/eunit/eunitmacros.h>

// Used to create HbKeymapFactory singleton to get rid of resource leak
#include <QLocale>
#include <hbinputkeymapfactory.h>
    

_LIT(KFirstName, "qfirstname r");
_LIT(KFirstName2, "otherFirstName");
_LIT(KLastName, "elastname");
_LIT(KLastName2, "anotherLastName");
_LIT(KMail, "tmail1");
_LIT(KMail2, "ymail2");
_LIT(KMail3, "imail3");

// -----------------------------------------------------------------------------
// UT_CQwertyPredictiveSearchTable::NewL
// -----------------------------------------------------------------------------
//
UT_CQwertyPredictiveSearchTable* UT_CQwertyPredictiveSearchTable::NewL()
    {
    UT_CQwertyPredictiveSearchTable* self = UT_CQwertyPredictiveSearchTable::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// UT_CQwertyPredictiveSearchTable::NewLC
// -----------------------------------------------------------------------------
//
UT_CQwertyPredictiveSearchTable* UT_CQwertyPredictiveSearchTable::NewLC()
    {
    UT_CQwertyPredictiveSearchTable* self =
		new( ELeave ) UT_CQwertyPredictiveSearchTable();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// UT_CQwertyPredictiveSearchTable::~UT_CQwertyPredictiveSearchTable
// -----------------------------------------------------------------------------
//
UT_CQwertyPredictiveSearchTable::~UT_CQwertyPredictiveSearchTable()
    {
    delete iTable;
    iTable = NULL;
    
    iDB.Close(); // Must close DB before it can be deleted
    RSqlDatabase::Delete(KDBFile);
    }

// -----------------------------------------------------------------------------
// UT_CQwertyPredictiveSearchTable::UT_CQwertyPredictiveSearchTable
// -----------------------------------------------------------------------------
//
UT_CQwertyPredictiveSearchTable::UT_CQwertyPredictiveSearchTable()
    {
    }

// -----------------------------------------------------------------------------
// UT_CQwertyPredictiveSearchTable::ConstructL
// -----------------------------------------------------------------------------
//
void UT_CQwertyPredictiveSearchTable::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    

#if defined(USE_ORBIT_KEYMAP)
    // Create singleton outside actual test cases so that it is not treated as
    // resource leak, since it can't be deleted.
    HbKeymapFactory::instance();
#endif
    }
    
// -----------------------------------------------------------------------------
// UT_CQwertyPredictiveSearchTable::SetupL
// Must start with an empty DB file for each test case
// -----------------------------------------------------------------------------
//
void UT_CQwertyPredictiveSearchTable::SetupL()
    {
    // Ignore error
	RSqlDatabase::Delete(KDBFile);
    // If this causes KErrAlreadyExists, iDB won't be fully constructed
	iDB.Create(KDBFile);

	iTable = CQwertyPredictiveSearchTable::NewL(iDB);
	// Create (empty) predictive search tables to DB
	iTable->CreateTableL();
	}
    
// -----------------------------------------------------------------------------
// UT_CQwertyPredictiveSearchTable::Teardown
// -----------------------------------------------------------------------------
//
void UT_CQwertyPredictiveSearchTable::Teardown()
    {
    delete iTable;
	iTable = NULL;

	iDB.Close(); // Must close DB before it can be deleted
	RSqlDatabase::Delete(KDBFile);
    }


// TEST CASES

// -----------------------------------------------------------------------------
// UT_CQwertyPredictiveSearchTable::UT_CreateInDbLL
// The first test case shows resource leak if Orbit keymap is used. If harcoded
// keymap is used, there is no resource leak.
// -----------------------------------------------------------------------------
//
void UT_CQwertyPredictiveSearchTable::UT_CreateInDbLL()
    {
	// Initially all tables are empty
	CheckItemCountL(InitTableVector());

	AddContactL(KTestContactId, KFirstName, KLastName, KMail, KMail2, KMail3);
    
	QVector<TInt> result = InitTableVector();
	result[0] = 1;
	result[2] = 1;
	result[3] = 1;
	result[4] = 1;
	result[5] = 1;
	result[7] = 1;
	CheckItemCountL(result);

	AddContactL(KTestContactId2, KNullDesC, KNullDesC, KNullDesC, KMail);
	++result[4];
	CheckItemCountL(result);
    }

// -----------------------------------------------------------------------------
// UT_CQwertyPredictiveSearchTable::UT_UpdateLL
// Update couple of fields, including mail address and adding a new mail
// addresses
// -----------------------------------------------------------------------------
//
void UT_CQwertyPredictiveSearchTable::UT_UpdateLL()
	{
	_LIT(KNewMailAddress, "uNew.mail@add.ress");
	_LIT(KNewFirstName, "tNewFn");
    
    AddContactL(KTestContactId, KFirstName, KLastName, KMail);

	QVector<TInt> result = InitTableVector();
	result[0] = 1;
	result[2] = 1;
	result[3] = 1;
	result[4] = 1;
	CheckItemCountL(result);

	CContactItem* contact =
		FillContactItemLC(KTestContactId, KNewFirstName, KLastName,
                          KNewMailAddress, KMail2, KMail);
    iTable->UpdateL(*contact);
    CleanupStack::PopAndDestroy(contact);

    result = InitTableVector();
	result[2] = 1;
	result[4] = 1;
	result[5] = 1;
	result[6] = 1;
	CheckItemCountL(result);
	}

// -----------------------------------------------------------------------------
// UT_CQwertyPredictiveSearchTable::UT_DeleteLL
// -----------------------------------------------------------------------------
//
void UT_CQwertyPredictiveSearchTable::UT_DeleteLL()
    {
    // Try to delete a non-existing item
    CContactItem* contact =
        FillContactItemLC(KTestContactId, KFirstName, KLastName, KMail2);
    TBool lowDiskErrorOccurred(EFalse);
    iTable->DeleteL(*contact, lowDiskErrorOccurred);
    
    // Add two contacts
    iTable->CreateInDbL(*contact);
    CContactItem* contact2 =
        FillContactItemLC(KTestContactId2, KFirstName2, KLastName2, KMail3);
    iTable->CreateInDbL(*contact2);

    QVector<TInt> result = InitTableVector();
    result[0] = 1;
    result[2] = 1;
    result[3] = 1;
    result[5] = 1;
    result[7] = 1;
    result[8] = 1;
    result[10] = 1;
    CheckItemCountL(result);
    
    iTable->DeleteL(*contact2, lowDiskErrorOccurred);
    CleanupStack::PopAndDestroy(contact2);
    
    result = InitTableVector();
    result[0] = 1;
    result[2] = 1;
    result[3] = 1;
    result[5] = 1;
    CheckItemCountL(result);

    iTable->DeleteL(*contact, lowDiskErrorOccurred);
    CleanupStack::PopAndDestroy(contact);
    
    CheckItemCountL(InitTableVector());
    }

// -----------------------------------------------------------------------------
// UT_CQwertyPredictiveSearchTable::InitTableVector
// -----------------------------------------------------------------------------
//
QVector<TInt> UT_CQwertyPredictiveSearchTable::InitTableVector() const
	{
	QVector<TInt> allTablesAreEmpty(CQwertyKeyMap::EAmountOfKeysInQwertyKeypad, 0);
	return allTablesAreEmpty;
	}

// -----------------------------------------------------------------------------
// UT_CQwertyPredictiveSearchTable::AddContactL
// -----------------------------------------------------------------------------
//
void UT_CQwertyPredictiveSearchTable::AddContactL(TContactItemId aContactId,
                                                  const TDesC& aFirstName,
                                                  const TDesC& aLastName,
                                                  const TDesC& aMail,
                                                  const TDesC& aMail2,
                                                  const TDesC& aMail3)
    {
	CContactItem* contact = FillContactItemLC(aContactId,
                                              aFirstName,
                                              aLastName,
                                              aMail,
                                              aMail2,
                                              aMail3);
	iTable->CreateInDbL(*contact);
	CleanupStack::PopAndDestroy(contact);
    }

// -----------------------------------------------------------------------------
// UT_CQwertyPredictiveSearchTable::FillContactItemLC
// -----------------------------------------------------------------------------
//
CContactItem*
UT_CQwertyPredictiveSearchTable::FillContactItemLC(TContactItemId aContactId,
                                                   const TDesC& aFirstName,
                                                   const TDesC& aLastName,
                                                   const TDesC& aMail,
                                                   const TDesC& aMail2,
                                                   const TDesC& aMail3) const
    {
    CContactItem* contact = CContactItem::NewLC(KUidContactCard);
    contact->SetId(aContactId);

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

    if (aMail.Length() > 0)
        {
        CContactItemField* field =
            CContactItemField::NewL(KStorageTypeText, KUidContactFieldEMail);
        CleanupStack::PushL(field);
        field->TextStorage()->SetTextL(aMail);
        contact->AddFieldL(*field); // Takes ownership
        CleanupStack::Pop(field);
        }
    
    if (aMail2.Length() > 0)
        {
        CContactItemField* field =
            CContactItemField::NewL(KStorageTypeText, KUidContactFieldEMail);
        CleanupStack::PushL(field);
        field->TextStorage()->SetTextL(aMail2);
        contact->AddFieldL(*field); // Takes ownership
        CleanupStack::Pop(field);
        }
    
    if (aMail3.Length() > 0)
        {
        CContactItemField* field =
            CContactItemField::NewL(KStorageTypeText, KUidContactFieldEMail);
        CleanupStack::PushL(field);
        field->TextStorage()->SetTextL(aMail3);
        contact->AddFieldL(*field); // Takes ownership
        CleanupStack::Pop(field);
        }    
	return contact;
    }

// -----------------------------------------------------------------------------
// UT_CQwertyPredictiveSearchTable::CheckItemCountL
// -----------------------------------------------------------------------------
//
void UT_CQwertyPredictiveSearchTable::CheckItemCountL(
	QVector<TInt> aCountInTables)
    {
	const TInt KMaxTableNameLength = 4;

    HBufC* s = HBufC::NewLC(KCountSelect().Length() + KMaxTableNameLength);
	TPtr ptr = s->Des();

	QList<QChar> tables;
	QT_TRYCATCH_LEAVING(tables = iTable->FillAllTables());
	HBufC* tableName(NULL);
	TInt tableNumber(0);
	while ((tableName = iTable->GetNextTableNameL(tables)) != NULL)
		{
		ptr.Format(KCountSelect, tableName);
		delete tableName;
		tableName = NULL;
        TSqlScalarFullSelectQuery scalarQuery(iDB);
    
        TInt rowCount = scalarQuery.SelectIntL(ptr);
        
		EUNIT_ASSERT_EQUALS(aCountInTables.at(tableNumber), rowCount);
		++tableNumber;
        }

    CleanupStack::PopAndDestroy(s);
	}
    
//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE(
    UT_CQwertyPredictiveSearchTable,
    "UT_CQwertyPredictiveSearchTable",
    "UNIT" )
        
EUNIT_TEST(
    "CreateInDbL - test",
    "UT_CQwertyPredictiveSearchTable",
    "CreateInDbL",
    "FUNCTIONALITY",
    SetupL, UT_CreateInDbLL, Teardown )

EUNIT_TEST(
    "UpdateL - test update mail address",
    "UT_CQwertyPredictiveSearchTable",
    "UpdateL",
    "FUNCTIONALITY",
    SetupL, UT_UpdateLL, Teardown )

EUNIT_TEST(
    "Delete - test",
    "UT_CQwertyPredictiveSearchTable",
    "DeleteL",
    "FUNCTIONALITY",
    SetupL, UT_DeleteLL, Teardown )

EUNIT_END_TEST_TABLE

//  END OF FILE
