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

#include "cpredictivesearchsettingstable.h"
#include "dbsqlconstants.h"

// This macro suppresses log writes
//#define NO_PRED_SEARCH_LOGS
#include "predictivesearchlog.h"


_LIT(KSqlContactPredSearchSettingsTable, "pred_search_settings");

_LIT(KCreatePredSearchSettingsTable, "CREATE TABLE pred_search_settings (language INTEGER);");

_LIT(KPredSearchSettingsLanguage, "language");

// How many characters the decimal representation of enum QLocale::Language
// requires.
const TInt KSizeOfLanguageValue = 8;

//_LIT(KInsertFormat, "INSERT INTO pred_search_settings (language) VALUES (%d);");
// e.g. "INSERT INTO pred_search_settings (language) VALUES (8);"
_LIT(KInsertFormat, "INSERT INTO %S (%S) VALUES (%d);");

// e.g. "UPDATE pred_search_settings SET language = 9;"
_LIT(KUpdateFormat, "UPDATE %S SET %S = %d;");

// e.g. "SELECT language FROM pred_search_settings;"
_LIT(KSelectFormat, "SELECT %S FROM %S;");



CPredictiveSearchSettingsTable*
CPredictiveSearchSettingsTable::NewL(RSqlDatabase& aDatabase)
	{
	PRINT(_L("CPredictiveSearchSettingsTable::NewL"));
	CPredictiveSearchSettingsTable* self =
		CPredictiveSearchSettingsTable::NewLC(aDatabase);
	CleanupStack::Pop(self);
	PRINT(_L("CPredictiveSearchSettingsTable::NewL ends"));
	return self;
	}

CPredictiveSearchSettingsTable*
CPredictiveSearchSettingsTable::NewLC(RSqlDatabase& aDatabase)
	{
	PRINT(_L("CPredictiveSearchSettingsTable::NewLC"));
	CPredictiveSearchSettingsTable* self =
		new (ELeave) CPredictiveSearchSettingsTable(aDatabase);
	CleanupStack::PushL(self);
	self->ConstructL();
	PRINT(_L("CPredictiveSearchSettingsTable::NewLC ends"));
	return self;
	}

CPredictiveSearchSettingsTable::~CPredictiveSearchSettingsTable()
	{
	PRINT(_L("CPredictiveSearchSettingsTable dtor"));
	PRINT(_L("CPredictiveSearchSettingsTable dtor ends"));
	}

void CPredictiveSearchSettingsTable::CreateInDbL(CContactItem& /*aItem*/)
	{
	}

void CPredictiveSearchSettingsTable::UpdateL(const CContactItem& /*aItem*/)
	{
	}

void CPredictiveSearchSettingsTable::DeleteL(const CContactItem& /*aItem*/,
												TBool& /*aLowDiskErrorOccurred*/)
	{
	}

// Create table to an empty database, so there is no need to re-generate QWERTY
// tables.
void CPredictiveSearchSettingsTable::CreateTableL()
	{
	PRINT(_L("CPredictiveSearchSettingsTable::CreateTableL"));

	User::LeaveIfError(iDatabase.Exec(KCreatePredSearchSettingsTable));
	enum QLocale::Language language = GetCurrentLanguageL();
	HBufC* insert = HBufC::NewLC(KInsertFormat().Length() +
								 KSqlContactPredSearchSettingsTable().Length() +
							 	 KPredSearchSettingsLanguage().Length() +
							 	 KSizeOfLanguageValue);
	insert->Des().AppendFormat(KInsertFormat,
							   &KSqlContactPredSearchSettingsTable,
							   &KPredSearchSettingsLanguage,
							   language);
	User::LeaveIfError(iDatabase.Exec(*insert));
	CleanupStack::PopAndDestroy(insert);

	PRINT(_L("CPredictiveSearchSettingsTable::CreateTableL ends"));
	}

QList<QChar> CPredictiveSearchSettingsTable::FillAllTables() const
	{
	QList<QChar> tables;
	tables.append('1'); // Add one character (does not matter what the character is)
	return tables;
	}

HBufC* CPredictiveSearchSettingsTable::TableNameL(const QChar /*aCh*/) const
	{
	return KSqlContactPredSearchSettingsTable().AllocL();
	}

TBool CPredictiveSearchSettingsTable::IsLanguageValidL() const
	{
	PRINT(_L("CPredictiveSearchSettingsTable::IsLanguageValidL"));
	return ReadStoredLanguageL() == GetCurrentLanguageL();
	}

void CPredictiveSearchSettingsTable::StoreCurrentLanguageL() const
	{
	PRINT(_L("CPredictiveSearchSettingsTable::StoreCurrentLanguageL"));

	enum QLocale::Language language = GetCurrentLanguageL();
	HBufC* update = HBufC::NewLC(KUpdateFormat().Length() +
								 KSqlContactPredSearchSettingsTable().Length() +
							 	 KPredSearchSettingsLanguage().Length() +
							 	 KSizeOfLanguageValue);
	update->Des().AppendFormat(KUpdateFormat,
							   &KSqlContactPredSearchSettingsTable,
							   &KPredSearchSettingsLanguage,
							   language);
	User::LeaveIfError(iDatabase.Exec(*update));
	CleanupStack::PopAndDestroy(update);

	PRINT(_L("CPredictiveSearchSettingsTable::StoreCurrentLanguageL ends"));
	}

void CPredictiveSearchSettingsTable::ConstructL()
	{
	}

enum QLocale::Language CPredictiveSearchSettingsTable::GetCurrentLanguageL() const
	{
    enum QLocale::Language language(QLocale::C);
	QT_TRYCATCH_LEAVING(language = QLocale::system().language());
	PRINT1(_L("CPredictiveSearchSettingsTable::GetCurrentLanguageL ret=%d"), language);
	return language;
	}

enum QLocale::Language CPredictiveSearchSettingsTable::ReadStoredLanguageL() const
	{
	TInt bufSize = KSelectFormat().Length() +
				   KPredSearchSettingsLanguage().Length() +
				   KSqlContactPredSearchSettingsTable().Length();
	HBufC* select = HBufC::NewLC(bufSize);
	select->Des().AppendFormat(KSelectFormat,
		&KPredSearchSettingsLanguage,
		&KSqlContactPredSearchSettingsTable);

	RSqlStatement stmnt;
	CleanupClosePushL(stmnt);
	PRINT1(_L("CPredictiveSearchSettingsTable::ReadStoredLanguageL SQL:%S"), select);
    stmnt.PrepareL(iDatabase, *select);

	const TInt KLanguageIndex = 0;
	TInt err = stmnt.Next();
	enum QLocale::Language language(QLocale::C);
	if (err == KSqlAtRow)
        {
		language = static_cast<QLocale::Language>(stmnt.ColumnInt(KLanguageIndex));
		PRINT1(_L("read language=%d from DB"), language);
        }
	CleanupStack::PopAndDestroy(&stmnt);
	CleanupStack::PopAndDestroy(select);

    if (err != KSqlAtRow)
        {
		PRINT1(_L("CPredictiveSearchSettingsTable::ReadStoredLanguageL err=%d"), err);
        User::Leave(err);
        }
	PRINT1(_L("CPredictiveSearchSettingsTable::ReadStoredLanguageL ret=%d"), language);
	return language;
	}

/**
Constructor
*/
CPredictiveSearchSettingsTable::CPredictiveSearchSettingsTable(
	RSqlDatabase& aDatabase) :
	CPplPredictiveSearchTableBase(aDatabase, 0, 0)
	{
	}
