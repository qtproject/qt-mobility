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

#include "cqwertypredictivesearchtable.h"
#include "cqwertykeymap.h"
#include "dbsqlconstants.h"
#include <QStringList>

// This macro suppresses own logs
//#define NO_PRED_SEARCH_LOGS
#include "predictivesearchlog.h"


// Max amount of tokens stored from contact
const TInt KMaxTokens = 7;
// Max amount of mail addresses stored from contact
const TInt KMaxMailAddresses = 3;

// How many characters are stored at most in the tokens
// Since BIGINT is a signed 64-bit integer, store only 10 characters
// to prevent overflow when comparing upper and lower limits.
const TInt KMaxTokenLength = 10;

// Template for table names
const TInt KMaxTableNameLength = 4;
_LIT(KTableNameFormat, "qm%d");

// Template for create table commands
_LIT(KPredSearchCreateQwertyMailTableFormat,
"CREATE TABLE %S (contact_id INTEGER PRIMARY KEY,\
 n BIGINT NULL, n2 BIGINT NULL, n3 BIGINT NULL, n4 BIGINT NULL,\
 n5 BIGINT NULL, n6 BIGINT NULL, n7 BIGINT NULL,\
 first_name CHAR(16) NULL, last_name CHAR(16) NULL);");

// Template for index names
// e.g. index0_n2
_LIT(KIndexNameFormat, "index%d_%S");

// Template for create index commands
// CREATE INDEX <index name> on <table> (<column>);");
_LIT(KPredSearchCreateQwertyMailIndexFormat, "CREATE INDEX %S on %S (%S);");

const QString KMailPrefix = "mailto:";


/**
@param aDatabase A handle to the database.
@param aProperties A contact properties object.

@return A pointer to a new CQwertyPredictiveSearchTable object.
*/
CQwertyPredictiveSearchTable*
CQwertyPredictiveSearchTable::NewL(RSqlDatabase& aDatabase)
	{
	PRINT(_L("CQwertyPredictiveSearchTable::NewL"));
	CQwertyPredictiveSearchTable* self = CQwertyPredictiveSearchTable::NewLC(aDatabase);
	CleanupStack::Pop(self);
	PRINT(_L("CQwertyPredictiveSearchTable::NewL ends"));
	return self;
	}


/**
@param aDatabase A handle to the database.
@param aProperties A contact properties object.

@return A pointer to a new CQwertyPredictiveSearchTable object.
*/
CQwertyPredictiveSearchTable*
CQwertyPredictiveSearchTable::NewLC(RSqlDatabase& aDatabase)
	{
	PRINT(_L("CQwertyPredictiveSearchTable::NewLC"));
	CQwertyPredictiveSearchTable* self =
		new (ELeave) CQwertyPredictiveSearchTable(aDatabase);
	CleanupStack::PushL(self);
	self->ConstructL();
	PRINT(_L("CQwertyPredictiveSearchTable::NewLC ends"));
	return self;
	}


/**
Destructor
*/
CQwertyPredictiveSearchTable::~CQwertyPredictiveSearchTable()
	{
	PRINT(_L("CQwertyPredictiveSearchTable dtor"));
	PRINT(_L("CQwertyPredictiveSearchTable dtor ends"));
	}


/**
Create the QWERTY tables and its indexes in the database.
*/
void CQwertyPredictiveSearchTable::CreateTableL()
	{
	PRINT(_L("CQwertyPredictiveSearchTable::CreateTableL"));

	// How many columns have index
    const TInt KIndexedColumnCount = 9;
    // Names of columns that have index
    const TDesC* indexColumns[] = {
        &KPredSearchQwertyMailNameAsNumber,
        &KPredSearchQwertyMailNameAsNumber2,
        &KPredSearchQwertyMailNameAsNumber3,
        &KPredSearchQwertyMailNameAsNumber4,
        &KPredSearchQwertyMailNameAsNumber5,
        &KPredSearchQwertyMailNameAsNumber6,
        &KPredSearchQwertyMailNameAsNumber7,
        &KPredSearchQwertyMailFirstName,
        &KPredSearchQwertyMailLastName};
    
    TInt maxColumnLength(0); // Length of longest column name
    for (TInt column = 0; column < KIndexedColumnCount; ++column)
        {
        TInt columnNameLength = indexColumns[column]->Length();
        if (columnNameLength > maxColumnLength)
            {
            maxColumnLength = columnNameLength;
            }
        }    
    
	// Space needed to represent number CQwertyKeyMap::EAmountOfKeysInQwertyKeypad
	const TInt KCharsNeededForTableNumber = 2;
	const TInt KMaxIndexNameLength =
		KIndexNameFormat().Length() + maxColumnLength + KCharsNeededForTableNumber;

	HBufC* tableName = HBufC::NewLC(KMaxTableNameLength);
	TPtr ptrTableName = tableName->Des();
	HBufC* createTableCmd =
		HBufC::NewLC(KPredSearchCreateQwertyMailTableFormat().Length() +
					 KMaxTableNameLength);
	TPtr ptrCreateTableCmd = createTableCmd->Des();

	HBufC* indexName = HBufC::NewLC(KMaxIndexNameLength);
	TPtr ptrIndexName = indexName->Des();
	HBufC* createIndexCmd = HBufC::NewLC(KPredSearchCreateQwertyMailIndexFormat().Length() +
									  KMaxIndexNameLength +
									  KMaxTableNameLength +
									  maxColumnLength);
	TPtr ptrCreateIndexCmd = createIndexCmd->Des();

	for (TInt table = 0; table < CQwertyKeyMap::EAmountOfKeysInQwertyKeypad; ++table)
		{
		ptrTableName.Format(KTableNameFormat, table);

		ptrCreateTableCmd.Format(KPredSearchCreateQwertyMailTableFormat, tableName);
		PRINT1(_L("SQL command: %S"), createTableCmd);
		User::LeaveIfError(iDatabase.Exec(*createTableCmd));

		// Create indexes for each required column of the current table
		for (TInt column = 0; column < KIndexedColumnCount; ++column)
			{
			ptrIndexName.Format(KIndexNameFormat, table, indexColumns[column]);

			ptrCreateIndexCmd.Format(KPredSearchCreateQwertyMailIndexFormat,
								     indexName, tableName, indexColumns[column]);
//			PRINT1(_L("SQL command: %S"), createIndexCmd);
			User::LeaveIfError(iDatabase.Exec(*createIndexCmd));			
			}
		}
	CleanupStack::PopAndDestroy(createIndexCmd);
	CleanupStack::PopAndDestroy(indexName);
	CleanupStack::PopAndDestroy(createTableCmd);
	CleanupStack::PopAndDestroy(tableName);
	
	PRINT(_L("CQwertyPredictiveSearchTable::CreateTableL ends"));
	}


TBool CQwertyPredictiveSearchTable::IsValidChar(const QChar aChar) const
	{
#if defined(USE_ORBIT_KEYMAP)
	return static_cast<CQwertyKeyMap*>(iKeyMap)->IsValidChar(aChar);
#else
	const QChar PAD_CHAR = '!'; // This is a hack, must have same value as in cqwertykeymap.cpp
	return static_cast<CQwertyKeyMap*>(iKeyMap)->UseHardcodedKeyMap(aChar) != PAD_CHAR;
#endif
	}


HBufC* CQwertyPredictiveSearchTable::TableNameL(const QChar aCh) const
	{
	TInt tableNumber = static_cast<CQwertyKeyMap*>(iKeyMap)->MapKeyNameToValue(aCh);
	if (tableNumber == CQwertyKeyMap::KPadCharValue)
		{
		User::Leave(KErrArgument);
		}

	HBufC* tableName = HBufC::NewL(KMaxTableNameLength);
	TPtr ptrTableName = tableName->Des();
	ptrTableName.Format(KTableNameFormat, tableNumber);
	return tableName;
	}


QList<QChar> CQwertyPredictiveSearchTable::FillAllTables() const
	{
	QList<QChar> tables;

	for (TInt key = 0; key < CQwertyKeyMap::EAmountOfKeysInQwertyKeypad; ++key)
		{
		tables.append(iKeyMap->ArrayIndexToMappedChar(key));
		}

	return tables;
	}


void CQwertyPredictiveSearchTable::FillKeyboardSpecificFieldsL(
	RSqlStatement& aSqlStatement,
	QStringList aTokens)
	{
	const TDesC* paramNames[] = {
		&KPredSearchQwertyMailNameAsNumberParam,
		&KPredSearchQwertyMailNameAsNumberParam2,
		&KPredSearchQwertyMailNameAsNumberParam3,
		&KPredSearchQwertyMailNameAsNumberParam4,
		&KPredSearchQwertyMailNameAsNumberParam5,
		&KPredSearchQwertyMailNameAsNumberParam6,
		&KPredSearchQwertyMailNameAsNumberParam7};
	for (TInt i = 0; i < aTokens.count(); ++i)
		{
		// TODO: It'd be better to add new fn into CQwertyKeyMap, that computes
		// the qint64 value like CQwertyKeyMap::ComputeValue().
		QString dummyLowerLimit;
		QString upperLimit;
		User::LeaveIfError(iKeyMap->GetNumericLimits(aTokens[i],
													 dummyLowerLimit,
													 upperLimit));
		bool ok(false);
		qint64 value(0); // qint64 is same as qlonglong
		QT_TRYCATCH_LEAVING(value = upperLimit.toLongLong(&ok));
		if (!ok)
			{
			User::Leave(KErrArgument);
			}
		// Decrement by one to get the correct value
		User::LeaveIfError(aSqlStatement.BindInt64(
			User::LeaveIfError(aSqlStatement.ParameterIndex(*paramNames[i])), --value));
		}
	}


/**
* Fetch up to 3 mail addresses
*/
QStringList CQwertyPredictiveSearchTable::GetTableSpecificFields(
	const CContactItem& aItem,
	TBool& aMandatoryFieldsPresent) const
	{
	PRINT(_L("CQwertyPredictiveSearchTable::GetTableSpecificFields"));

	QStringList mailAddresses;
	
	// Check that the contact item is a card, own card or ICC entry.
	const TUid KType = aItem.Type();
	if (KType != KUidContactCard &&
		KType != KUidContactOwnCard &&
		KType != KUidContactICCEntry)
		{
		aMandatoryFieldsPresent = EFalse;
		return mailAddresses;
		}

	TInt storedAddressCount(0);
	for (TInt i = aItem.CardFields().Count();
		 i > 0 && storedAddressCount < KMaxMailAddresses;
		 --i)
		{
		CContactItemField& field = aItem.CardFields()[i - 1];
		if (field.ContentType().ContainsFieldType(KUidContactFieldEMail) &&
			field.StorageType() == KStorageTypeText &&
			field.TextStorage()->IsFull()) // IsFull() returns true if field not empty
			{
			TPtrC mailAddress = field.TextStorage()->Text();
			PRINT2(_L("contact id=%d has mail='%S'"), aItem.Id(), &mailAddress);

			QString wholeAddress((QChar*)mailAddress.Ptr(), mailAddress.Length());
			QString address = wholeAddress;
			if (wholeAddress.startsWith(KMailPrefix)) // Skip prefix
				{
				address = wholeAddress.mid(KMailPrefix.length());
#if defined(WRITE_PRED_SEARCH_LOGS)
				const TInt KLogLength = 40;
				TBuf<KLogLength> log(address.left(KLogLength).utf16());
				PRINT1(_L("prefix removed, mail='%S'"), &log);
#endif
				}
			mailAddresses.append(iKeyMap->GetMappedString(address));
			++storedAddressCount;
			}
		}
	PRINT1(_L("CQwertyPredictiveSearchTable::GetTableSpecificFields found %d mail addrs"),
		   mailAddresses.count());
	aMandatoryFieldsPresent = (mailAddresses.count() > 0);
	return mailAddresses;
	}


/**
Set up the CCntSqlStatement objects held by the class.
*/
void CQwertyPredictiveSearchTable::ConstructL()
	{
	PRINT(_L("CQwertyPredictiveSearchTable::ConstructL"));

	CPplPredictiveSearchTableBase::ConstructL();

	// Set details of INSERT
	iInsertStmnt->SetParamL(KPredSearchQwertyMailContactId,
	                        KPredSearchQwertyMailContactIdParam);
	iInsertStmnt->SetParamL(KPredSearchQwertyMailNameAsNumber,
							KPredSearchQwertyMailNameAsNumberParam);
	iInsertStmnt->SetParamL(KPredSearchQwertyMailNameAsNumber2,
							KPredSearchQwertyMailNameAsNumberParam2);
	iInsertStmnt->SetParamL(KPredSearchQwertyMailNameAsNumber3,
							KPredSearchQwertyMailNameAsNumberParam3);
	iInsertStmnt->SetParamL(KPredSearchQwertyMailNameAsNumber4,
							KPredSearchQwertyMailNameAsNumberParam4);
	iInsertStmnt->SetParamL(KPredSearchQwertyMailNameAsNumber5,
							KPredSearchQwertyMailNameAsNumberParam5);
	iInsertStmnt->SetParamL(KPredSearchQwertyMailNameAsNumber6,
							KPredSearchQwertyMailNameAsNumberParam6);
	iInsertStmnt->SetParamL(KPredSearchQwertyMailNameAsNumber7,
							KPredSearchQwertyMailNameAsNumberParam7);

	iInsertStmnt->SetParamL(KPredSearchQwertyMailFirstName,
							KPredSearchQwertyMailFirstNameParam);
	iInsertStmnt->SetParamL(KPredSearchQwertyMailLastName,
							KPredSearchQwertyMailLastNameParam);

	PRINT(_L("CQwertyPredictiveSearchTable::ConstructL create key map"));
	iKeyMap = CQwertyKeyMap::NewL();

	PRINT(_L("CQwertyPredictiveSearchTable::ConstructL ends"));
	}


/**
Constructor
*/
CQwertyPredictiveSearchTable::CQwertyPredictiveSearchTable(RSqlDatabase& aDatabase) :
	CPplPredictiveSearchTableBase(aDatabase, KMaxTokens, KMaxTokenLength)
	{
	}
