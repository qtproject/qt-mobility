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
* Description: 
*
*/

#include "cpredictivesearchsynchronizer.h"
#include "c12keypredictivesearchtable.h"
#include "cqwertypredictivesearchtable.h"
#include "cpredictivesearchsettingstable.h"
#include "dbsqlconstants.h"

// This macro suppresses log writes
//#define NO_PRED_SEARCH_LOGS
#include "predictivesearchlog.h"

_LIT(KFirstQwertyTableName, "qm0");


CPredictiveSearchSynchronizer*
CPredictiveSearchSynchronizer::NewL(RSqlDatabase& aDatabase,
									C12keyPredictiveSearchTable& a12keyTable,
									CQwertyPredictiveSearchTable& aQwertyTable,
									CPredictiveSearchSettingsTable& aSettingsTable)
	{
	PRINT(_L("CPredictiveSearchSynchronizer::NewL"));
	CPredictiveSearchSynchronizer* self =
		CPredictiveSearchSynchronizer::NewLC(aDatabase, a12keyTable,
											 aQwertyTable, aSettingsTable);
	CleanupStack::Pop(self);
	PRINT(_L("CPredictiveSearchSynchronizer::NewL ends"));
	return self;
	}

CPredictiveSearchSynchronizer*
CPredictiveSearchSynchronizer::NewLC(RSqlDatabase& aDatabase,
									 C12keyPredictiveSearchTable& a12keyTable,
									 CQwertyPredictiveSearchTable& aQwertyTable,
									 CPredictiveSearchSettingsTable& aSettingsTable)
	{
	PRINT(_L("CPredictiveSearchSynchronizer::NewLC"));
	CPredictiveSearchSynchronizer* self =
		new (ELeave) CPredictiveSearchSynchronizer(aDatabase, a12keyTable,
												   aQwertyTable, aSettingsTable);
	CleanupStack::PushL(self);
	self->ConstructL();
	PRINT(_L("CPredictiveSearchSynchronizer::NewLC ends"));
	return self;
	}

CPredictiveSearchSynchronizer::~CPredictiveSearchSynchronizer()
	{
	PRINT(_L("CPredictiveSearchSynchronizer dtor"));
	PRINT(_L("CPredictiveSearchSynchronizer dtor ends"));
	}

void CPredictiveSearchSynchronizer::ConstructL()
	{
	}

/**
Constructor
*/
CPredictiveSearchSynchronizer::CPredictiveSearchSynchronizer(
	RSqlDatabase& aDatabase,
	C12keyPredictiveSearchTable& a12keyTable,
	CQwertyPredictiveSearchTable& aQwertyTable,
	CPredictiveSearchSettingsTable& aSettingsTable) :
	iDatabase(aDatabase),
	i12keyTable(a12keyTable),
	iQwertyTable(aQwertyTable),
	iSettingsTable(aSettingsTable)
	{
	}

// There are 3 cases:
// - none of the pred.search tables exist
// - just 12-key pred.search tables exist
// - all pred.search tables exist (12-key, QWERTY, settings)
// There is no case where QWERTY tables exist but settings table does not,
// since such a version was not released.
void CPredictiveSearchSynchronizer::SynchronizeTablesL()
	{
	PRINT(_L("CPredictiveSearchSynchronizer::SynchronizeTablesL"));

	if (CheckIfPredSearchTableExistsL(KSqlContactPredSearchTable0))
		{
		if (CheckIfPredSearchTableExistsL(KFirstQwertyTableName))
			{
			// All tables exist, check if language has been changed
			if (!iSettingsTable.IsLanguageValidL())
				{
				PRINT(_L("language has changed, re-create QWERTY tables"));
				DeletePredSearchTableL(iQwertyTable);
				CreatePredSearchTablesL(EFalse);
				iSettingsTable.StoreCurrentLanguageL();
				}
			}
		else
			{
			PRINT(_L("QWERTY and settings tables missing, re-create all tables"));
			DeletePredSearchTablesL();
			CreatePredSearchTablesL();
			}
		}
	else
		{
		// All predictive search tables missing, create all
		CreatePredSearchTablesL();
		}

	PRINT(_L("CPredictiveSearchSynchronizer::SynchronizeTablesL ends"));
	}

void CPredictiveSearchSynchronizer::CreatePredSearchTablesL(TBool aAllTables)
	{
	PRINT1(_L("CPredictiveSearchSynchronizer::CreatePredSearchTablesL all=%d"), aAllTables);

	if (aAllTables)
		{
		i12keyTable.CreateTableL();
		iSettingsTable.CreateTableL();
		iSettingsTable.StoreCurrentLanguageL();
		}
	iQwertyTable.CreateTableL();

	_LIT(KSelectAllContactsFormat, "SELECT %S,%S,%S FROM %S;");
	TInt bufSize = KSelectAllContactsFormat().Length() +
				   KContactId().Length() +
				   KContactFirstName().Length() +
				   KContactLastName().Length() +
				   KSqlContactTableName().Length();
	HBufC* sqlStatement = HBufC::NewLC(bufSize);
	sqlStatement->Des().AppendFormat(KSelectAllContactsFormat,
		&KContactId,
		&KContactFirstName,
		&KContactLastName,
		&KSqlContactTableName);

	RSqlStatement stmnt;
	CleanupClosePushL(stmnt);
	PRINT1(_L("CreatePredSearchTablesL prepare SQL statement:%S"), sqlStatement);
    stmnt.PrepareL(iDatabase, *sqlStatement);

	const TInt KContactIdIndex = 0;
	const TInt KFirstNameIndex = 1;
	const TInt KLastNameIndex = 2;
	TInt err(KErrNone);
    while ((err = stmnt.Next()) == KSqlAtRow)
        {
		PRINT(_L("CreatePredSearchTablesL create CContactItem"));

		TInt id = KUidContactCardValue;
		TUid uid;
		uid.iUid = id;
		CContactItem* contact = CContactItem::NewLC(uid);
		contact->SetId(stmnt.ColumnInt(KContactIdIndex));

		// If first name exists, write it to contact item
		TPtrC firstName;
		if (stmnt.ColumnText(KFirstNameIndex, firstName) == KErrNone)
			{
			CContactItemField* field =
				CContactItemField::NewLC(KStorageTypeText, KUidContactFieldGivenName);
			CContactTextField* textfield = field->TextStorage();
			textfield->SetTextL(firstName);
			contact->AddFieldL(*field); // Takes ownership
			CleanupStack::Pop(field);
			}

		TPtrC lastName;
		if (stmnt.ColumnText(KLastNameIndex, lastName) == KErrNone)
			{
			CContactItemField* field =
				CContactItemField::NewLC(KStorageTypeText, KUidContactFieldFamilyName);
			CContactTextField* textfield = field->TextStorage();
			textfield->SetTextL(lastName);
			contact->AddFieldL(*field); // Takes ownership
			CleanupStack::Pop(field);
			}
		PRINT(_L("CreatePredSearchTablesL create entry to tables"));
		if (aAllTables)
			{
			i12keyTable.CreateInDbL(*contact);
			}
		if (ReadMailAddressesL(*contact))
			{
			iQwertyTable.CreateInDbL(*contact);
			}

		CleanupStack::PopAndDestroy(contact);
        }

    // Leave if we didn't complete going through the results properly
    if (err != KSqlAtEnd)
        {
		PRINT1(_L("CreatePredSearchTablesL SQL err=%d"), err);
        User::Leave(err);
        }
    CleanupStack::PopAndDestroy(&stmnt);
	CleanupStack::PopAndDestroy(sqlStatement);

	PRINT(_L("CPredictiveSearchSynchronizer::CreatePredSearchTablesL ends"));
	}

void CPredictiveSearchSynchronizer::DeletePredSearchTablesL()
	{
	PRINT(_L("CPredictiveSearchSynchronizer::DeletePredSearchTablesL"));

	DeletePredSearchTableL(i12keyTable);
	DeletePredSearchTableL(iQwertyTable);
	DeletePredSearchTableL(iSettingsTable);
	
	PRINT(_L("CPredictiveSearchSynchronizer::DeletePredSearchTablesL ends"));
	}

TBool CPredictiveSearchSynchronizer::CheckIfPredSearchTableExistsL(
	const TDesC& aTableName) const
	{
	PRINT1(_L("CPredictiveSearchSynchronizer::CheckIfPredSearchTableExistsL table='%S'"),
		   &aTableName);

	_LIT(KSelectFirstTableFormat,
		 "SELECT name FROM sqlite_master WHERE type='table' AND name='%S';");

	TInt bufSize = KSelectFirstTableFormat().Length() +
				   aTableName.Length();
	HBufC* select = HBufC::NewLC(bufSize);
	select->Des().AppendFormat(KSelectFirstTableFormat, &aTableName);
	RSqlStatement stmnt;
	CleanupClosePushL(stmnt);
    stmnt.PrepareL(iDatabase, *select);

	TBool tableExists = (stmnt.Next() == KSqlAtRow);

	CleanupStack::PopAndDestroy(&stmnt);
	CleanupStack::PopAndDestroy(select);

	PRINT1(_L("CPredictiveSearchSynchronizer::CheckIfPredSearchTablesExistL return %d"),
		   tableExists);
	return tableExists;
	}

void CPredictiveSearchSynchronizer::DeletePredSearchTableL(CPplPredictiveSearchTableBase& aTable)
	{
	PRINT(_L("CPredictiveSearchSynchronizer::DeletePredSearchTableL"));

	// IF EXISTS suppresses error that would occur if table does not exist
	_LIT(KDropTable, "DROP TABLE IF EXISTS %S;");

	QList<QChar> tables;
	QT_TRYCATCH_LEAVING(tables = aTable.FillAllTables());
	HBufC* tableName(NULL);
	while ((tableName = aTable.GetNextTableNameL(tables)) != NULL)
		{
		CleanupStack::PushL(tableName);
		HBufC* dropTable = HBufC::NewLC(KDropTable().Length() + tableName->Length());
        dropTable->Des().Format(KDropTable, tableName);

		User::LeaveIfError(iDatabase.Exec(*dropTable));

        CleanupStack::PopAndDestroy(dropTable);
		CleanupStack::PopAndDestroy(tableName);
		}
	PRINT(_L("CPredictiveSearchSynchronizer::DeletePredSearchTableL ends"));
	}

TBool CPredictiveSearchSynchronizer::ReadMailAddressesL(CContactItem& aContact)
	{
	PRINT(_L("CPredictiveSearchSynchronizer::ReadMailAddressesL"));

	// SELECT value FROM comm_addr
	//	 WHERE contact_id = [contact id value] AND type = [type value];
    _LIT(KSelectMailAddrFormat, "SELECT %S FROM %S WHERE %S = %d AND %S = %d;");
	const TInt KContactIdLength = 10;
	const TInt KCommAddrTypeLength = 2; // CPplCommAddrTable::EEmailAddress is enum
	TInt bufSize = KSelectMailAddrFormat().Length() +
				   KCommAddrValue().Length() +
				   KSqlContactCommAddrTableName().Length() +
				   KCommAddrContactId().Length() +
				   KContactIdLength +
				   KCommAddrType().Length() +
				   KCommAddrTypeLength;
	HBufC* sqlStatement = HBufC::NewLC(bufSize);
	sqlStatement->Des().AppendFormat(KSelectMailAddrFormat,
		&KCommAddrValue,
		&KSqlContactCommAddrTableName,
		&KCommAddrContactId,
		aContact.Id(),
		&KCommAddrType,
		CPplCommAddrTable::EEmailAddress);

	RSqlStatement stmnt;
	CleanupClosePushL(stmnt);
	PRINT1(_L("prepare SQL statement:%S"), sqlStatement);
    stmnt.PrepareL(iDatabase, *sqlStatement);

	const TInt KValueIndex = 0;
	TBool foundMailAddress(EFalse);
	TInt err(KErrNone);
    while ((err = stmnt.Next()) == KSqlAtRow)
        {
		TPtrC value;
		if (stmnt.ColumnText(KValueIndex, value) == KErrNone)
			{
			PRINT2(_L("  id=%d, found mail address=%S"), aContact.Id(), &value);
			CContactItemField* field =
				CContactItemField::NewLC(KStorageTypeText, KUidContactFieldEMail);
			CContactTextField* textfield = field->TextStorage();
			textfield->SetTextL(value);
			aContact.AddFieldL(*field); // Takes ownership
			CleanupStack::Pop(field);
			foundMailAddress = ETrue;
			}
        }

    if (err != KSqlAtEnd)
        {
		PRINT1(_L("CPredictiveSearchSynchronizer::ReadMailAddressesL SQL err=%d"), err);
        User::Leave(err);
        }
    CleanupStack::PopAndDestroy(&stmnt);
	CleanupStack::PopAndDestroy(sqlStatement);
	PRINT1(_L("CPredictiveSearchSynchronizer::ReadMailAddressesL return %d"), foundMailAddress);
	return foundMailAddress;
	}
