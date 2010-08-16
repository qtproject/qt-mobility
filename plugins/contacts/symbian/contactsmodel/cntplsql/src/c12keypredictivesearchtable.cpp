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

#include "c12keypredictivesearchtable.h"
#include "dbsqlconstants.h"
#include "cntitem.h"
#include "c12keykeymap.h"
#include <QStringList>

// This macro suppresses log writes
//#define NO_PRED_SEARCH_LOGS
#include "predictivesearchlog.h"


// Max amount of tokens stored from contact
const TInt KMaxTokens = 4;

// How many digits are stored at most in the numeric field
// Since BIGINT is a signed 64-bit integer, store only 15 hexadecimal characters
// to prevent overflow when comparing upper and lower limits.
const TInt KMaxTokenLength = 15;

const quint64 KConversionError = 0xeeeeeeeeeeeeeee;

#define MAPPED_CHAR_FOR_STAR	'a'
#define MAPPED_CHAR_FOR_HASH	'b'

const QChar KPadChar = 'f'; // Pad with hex-digit 0xF


/**
@param aDatabase A handle to the database.

@return A pointer to a new C12keyPredictiveSearchTable object.
*/
C12keyPredictiveSearchTable*
C12keyPredictiveSearchTable::NewL(RSqlDatabase& aDatabase)
	{
	PRINT(_L("C12keyPredictiveSearchTable::NewL"));
	C12keyPredictiveSearchTable* self = C12keyPredictiveSearchTable::NewLC(aDatabase);
	CleanupStack::Pop(self);
	PRINT(_L("C12keyPredictiveSearchTable::NewL ends"));
	return self;
	}


/**
@param aDatabase A handle to the database.

@return A pointer to a new C12keyPredictiveSearchTable object.
*/
C12keyPredictiveSearchTable*
C12keyPredictiveSearchTable::NewLC(RSqlDatabase& aDatabase)
	{
	PRINT(_L("C12keyPredictiveSearchTable::NewLC"));
	C12keyPredictiveSearchTable* self =
		new (ELeave) C12keyPredictiveSearchTable(aDatabase);
	CleanupStack::PushL(self);
	self->ConstructL();
	PRINT(_L("C12keyPredictiveSearchTable::NewLC ends"));
	return self;
	}


/**
Destructor
*/
C12keyPredictiveSearchTable::~C12keyPredictiveSearchTable()
	{
	PRINT(_L("C12keyPredictiveSearchTable dtor"));
	PRINT(_L("C12keyPredictiveSearchTable dtor ends"));
	}


/**
Creates the 12-key predictive search tables and its indexes in the database.
*/
void C12keyPredictiveSearchTable::CreateTableL()
	{
	PRINT(_L("C12keyPredictiveSearchTable::CreateTableL"));

	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateTable0Stmnt));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateTable1Stmnt));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateTable2Stmnt));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateTable3Stmnt));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateTable4Stmnt));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateTable5Stmnt));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateTable6Stmnt));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateTable7Stmnt));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateTable8Stmnt));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateTable9Stmnt));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateTable10Stmnt));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateTable11Stmnt));


	PRINT(_L("Create indexes"));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbrIndexTable0));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbr2IndexTable0));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbr3IndexTable0));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbr4IndexTable0));

	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbrIndexTable1));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbr2IndexTable1));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbr3IndexTable1));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbr4IndexTable1));

	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbrIndexTable2));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbr2IndexTable2));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbr3IndexTable2));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbr4IndexTable2));

	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbrIndexTable3));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbr2IndexTable3));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbr3IndexTable3));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbr4IndexTable3));

	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbrIndexTable4));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbr2IndexTable4));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbr3IndexTable4));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbr4IndexTable4));

	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbrIndexTable5));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbr2IndexTable5));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbr3IndexTable5));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbr4IndexTable5));

	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbrIndexTable6));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbr2IndexTable6));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbr3IndexTable6));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbr4IndexTable6));

	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbrIndexTable7));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbr2IndexTable7));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbr3IndexTable7));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbr4IndexTable7));

	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbrIndexTable8));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbr2IndexTable8));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbr3IndexTable8));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbr4IndexTable8));

	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbrIndexTable9));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbr2IndexTable9));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbr3IndexTable9));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbr4IndexTable9));

	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbrIndexTable10));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbr2IndexTable10));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbr3IndexTable10));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbr4IndexTable10));

	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbrIndexTable11));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbr2IndexTable11));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbr3IndexTable11));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbr4IndexTable11));

	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateFNIndexInTable0));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateLNIndexInTable0));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateFNIndexInTable1));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateLNIndexInTable1));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateFNIndexInTable2));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateLNIndexInTable2));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateFNIndexInTable3));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateLNIndexInTable3));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateFNIndexInTable4));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateLNIndexInTable4));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateFNIndexInTable5));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateLNIndexInTable5));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateFNIndexInTable6));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateLNIndexInTable6));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateFNIndexInTable7));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateLNIndexInTable7));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateFNIndexInTable8));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateLNIndexInTable8));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateFNIndexInTable9));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateLNIndexInTable9));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateFNIndexInTable10));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateLNIndexInTable10));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateFNIndexInTable11));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateLNIndexInTable11));

	PRINT(_L("C12keyPredictiveSearchTable::CreateTableL ends"));
	}


QList<QChar> C12keyPredictiveSearchTable::FillAllTables() const
	{
	QList<QChar> tables;

	const TInt KLargestDigitKey = '9';
	for (TInt i = '0'; i <= KLargestDigitKey; ++i)
		{
		QChar ch = i;
		tables.append(ch);
		}
	tables.append(MAPPED_CHAR_FOR_STAR);
	tables.append(MAPPED_CHAR_FOR_HASH);

	return tables;
	}


HBufC* C12keyPredictiveSearchTable::TableNameL(const QChar aCh) const
	{
	// Enough space for the longest table name
    HBufC* tableName = HBufC::NewLC(KSqlContactPredSearchTable11().Length());
    TPtr ptr = tableName->Des();
	ptr.Append(GetTableNameL(aCh));
	CleanupStack::Pop(tableName);
	return tableName;
	}


TBool C12keyPredictiveSearchTable::IsValidChar(const QChar aChar) const
	{
	return (aChar >= '0' && aChar <= '9') ||
			aChar == MAPPED_CHAR_FOR_STAR ||
			aChar == MAPPED_CHAR_FOR_HASH;
	}


void C12keyPredictiveSearchTable::FillKeyboardSpecificFieldsL(
	RSqlStatement& aSqlStatement,
	QStringList aTokens)
	{
	const TDesC* paramNames[] = {
		&KPredSearchNameAsNumberParam,
		&KPredSearchNameAsNumber2Param,
		&KPredSearchNameAsNumber3Param,
		&KPredSearchNameAsNumber4Param};
	for (TInt i = 0; i < aTokens.count(); ++i)
		{
		quint64 hex(0);
		QT_TRYCATCH_LEAVING(hex = ConvertToHex(aTokens[i]));
		if (hex == KConversionError)
			{
			User::Leave(KErrArgument);
			}
		User::LeaveIfError(aSqlStatement.BindInt64(
			User::LeaveIfError(aSqlStatement.ParameterIndex(*paramNames[i])), hex));
		}
	}


/**
Set up the CCntSqlStatement objects held by the class.
*/
void C12keyPredictiveSearchTable::ConstructL()
	{
	PRINT(_L("C12keyPredictiveSearchTable::ConstructL"));

	CPplPredictiveSearchTableBase::ConstructL();
	// Set details of INSERT
	iInsertStmnt->SetParamL(KPredSearchContactId,
	                        KPredSearchContactIdParam);
	iInsertStmnt->SetParamL(KPredSearchNameAsNumber,
							KPredSearchNameAsNumberParam);
	iInsertStmnt->SetParamL(KPredSearchNameAsNumber2,
							KPredSearchNameAsNumber2Param);
	iInsertStmnt->SetParamL(KPredSearchNameAsNumber3,
							KPredSearchNameAsNumber3Param);
	iInsertStmnt->SetParamL(KPredSearchNameAsNumber4,
							KPredSearchNameAsNumber4Param);
	iInsertStmnt->SetParamL(KPredSearchFirstName,
							KPredSearchFirstNameParam);
	iInsertStmnt->SetParamL(KPredSearchLastName,
							KPredSearchLastNameParam);

	PRINT(_L("C12keyPredictiveSearchTable::ConstructL create key map"));
	iKeyMap = C12keyKeyMap::NewL();

	PRINT(_L("C12keyPredictiveSearchTable::ConstructL ends"));
	}


/**
Constructor
*/
C12keyPredictiveSearchTable::C12keyPredictiveSearchTable(RSqlDatabase& aDatabase) :
	CPplPredictiveSearchTableBase(aDatabase, KMaxTokens, KMaxTokenLength)
	{
	}


// TODO: maybe move this fn to CPcsKeyMap, and it can use code of GetNumericLimits()

// E.g. aToken = "01230" -> append KPadChar until has KMaxTokenLength characters
// -> "01230ffffffffff" -> convert to hexadecimal number -> 0x01230ffffffffff.
// If this function would leave, causes panic, perhaps because of QString parameter?
quint64 C12keyPredictiveSearchTable::ConvertToHex(QString aToken) const
	{
	if (aToken.length() > KMaxTokenLength)
		{
		return KConversionError;
		}
    QString padded = aToken.leftJustified(KMaxTokenLength, KPadChar);

    TBuf<KMaxTokenLength> log(padded.utf16());
    PRINT1(_L("C12keyPredictiveSearchTable::ConvertToHex padded '%S'"), &log);

	// Replace unmapped char and the following characters with KPadChar.
    QString replaced = padded;
    bool done(false);
    for (TInt i = 0; i < KMaxTokenLength && !done; ++i)
        {
        if (!IsValidChar(padded[i]))
            {
            // replace() does not work, it puts just one KPadChar at end
            // replaced = padded.replace(i, KMaxTokenLength - i, KPadChar);

            padded.remove(i, KMaxTokenLength - i);
            replaced = padded.leftJustified(KMaxTokenLength, KPadChar);
            done = true;

			TBuf<KMaxTokenLength> log2(replaced.utf16());
			PRINT1(_L("After replacing '%S'"), &log2);
            }
        }

    const TInt KHexadecimalBase = 16;
    bool ok(true);
    quint64 hex = replaced.toULongLong(&ok, KHexadecimalBase);
    if (!ok)
        {
		PRINT(_L("conv to hex failed"));
		return KConversionError;
        }
    
    PRINT1(_L("C12keyPredictiveSearchTable::ConvertToHex result 0x%lx"), hex);
    return hex;
	}


const TDesC& C12keyPredictiveSearchTable::GetTableNameL(const QChar aCh) const
	{
	switch (aCh.toAscii())
		{
		case '0': return KSqlContactPredSearchTable0;
		case '1': return KSqlContactPredSearchTable1;
		case '2': return KSqlContactPredSearchTable2;
		case '3': return KSqlContactPredSearchTable3;
		case '4': return KSqlContactPredSearchTable4;
		case '5': return KSqlContactPredSearchTable5;
		case '6': return KSqlContactPredSearchTable6;
		case '7': return KSqlContactPredSearchTable7;
		case '8': return KSqlContactPredSearchTable8;
		case '9': return KSqlContactPredSearchTable9;
		case MAPPED_CHAR_FOR_STAR: return KSqlContactPredSearchTable10;
		case MAPPED_CHAR_FOR_HASH: return KSqlContactPredSearchTable11;
		default:
			PRINT1(_L("C12keyPredictiveSearchTable::GetTableNameL unknown char '%c'"),
				   aCh.toAscii());
			User::Leave(KErrArgument);
			return KNullDesC;
		}
	}
