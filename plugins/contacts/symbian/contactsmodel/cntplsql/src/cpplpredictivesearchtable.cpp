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
* Description: 
*
*/

#include "pltables.h"
#include "dbsqlconstants.h"
#include "cpcskeymap.h"
#include "cntitem.h"
#include <QStringList>
#include "predictivesearchlog.h"


// How many characters from the beginning of the first name and last name are
// stored. This only affects how precisely the results are put in alphabetic order.
const TInt KCharactersFromName = 16;


/**
Destructor
*/
CPplPredictiveSearchTableBase::~CPplPredictiveSearchTableBase()
	{
	PRINT(_L("CPplPredictiveSearchTableBase dtor"));
	delete iInsertStmnt;
	delete iDeleteStmnt;
	delete iKeyMap;
	PRINT(_L("CPplPredictiveSearchTableBase dtor ends"));
	}


/**
@param aItem A contact item whose data are added to the table.
*/
void CPplPredictiveSearchTableBase::CreateInDbL(CContactItem& aItem)
	{
	PRINT(_L("CPplPredictiveSearchTableBase::CreateInDbL"));
	WriteToDbL(aItem);
	PRINT(_L("CPplPredictiveSearchTableBase::CreateInDbL ends"));
	}


/**
Update is done in two steps: delete contact from all predictive search tables,
then insert it into relevant tables.

@param aItem A contact item whose data is updated in the database.
*/
void CPplPredictiveSearchTableBase::UpdateL(const CContactItem& aItem)
	{
	PRINT(_L("CPplPredictiveSearchTableBase::UpdateL"));

	TBool lowDiskErrorOccurred(EFalse);
	DeleteFromAllTablesL(aItem.Id(), lowDiskErrorOccurred);
	if (lowDiskErrorOccurred)
	    {
        User::Leave(KErrGeneral);
	    }
	WriteToDbL(aItem);

	PRINT(_L("CPplPredictiveSearchTableBase::UpdateL ends"));
	}


/**
Deletes a contact item from predictive search tables.

@param aItem The contact item to be deleted. It contains contact id, but not
             first name or last name fields.
*/
void CPplPredictiveSearchTableBase::DeleteL(const CContactItem& aItem,
											TBool& aLowDiskErrorOccurred)
	{
	PRINT(_L("CPplPredictiveSearchTableBase::DeleteL"));

	DeleteFromAllTablesL(aItem.Id(), aLowDiskErrorOccurred); 

	PRINT(_L("CPplPredictiveSearchTableBase::DeleteL ends"));
	}


/**
Default implementation returns empty list.
*/
QStringList CPplPredictiveSearchTableBase::GetTableSpecificFields(
	const CContactItem& /*aItem*/,
	TBool& aMandatoryFieldsPresent) const
	{
	aMandatoryFieldsPresent = ETrue;
	QStringList emptyList;
	return emptyList;
	}


HBufC* CPplPredictiveSearchTableBase::GetNextTableNameL(QList<QChar>& aTables) const
	{
	HBufC* tableName(NULL);
	if (aTables.count() > 0)
		{
		tableName = TableNameL(aTables[0]);
		aTables.removeFirst();
//		PRINT1(_L("CPplPredictiveSearchTableBase::GetNextTableNameL '%S'"), tableName);
        }
	return tableName;
	}


/**
Set up the CCntSqlStatement objects held by the class.
*/
void CPplPredictiveSearchTableBase::ConstructL()
	{
	PRINT(_L("CPplPredictiveSearchTableBase::ConstructL"));

	// Using dummy table names here
	TCntSqlStatementType insertType(EInsert, KSqlContactPredSearchTable0);
	TCntSqlStatementType deleteType(EDelete, KSqlContactPredSearchTable0);
	iInsertStmnt = TSqlProvider::GetSqlStatementL(insertType);
	// Details of INSERT are done in subclass


	const TInt KWhereContactIdBufSize(
		KWhereStringEqualsStringFormatText().Size() +
		KPredSearchContactId().Size() +
		KPredSearchContactIdParam().Size() );
	HBufC* whereContactIdClause = HBufC::NewLC(KWhereContactIdBufSize);
	// for WHERE contact_id = [contact id value]
	whereContactIdClause->Des().AppendFormat(KWhereStringEqualsStringFormatText,
		&KPredSearchContactId, &KPredSearchContactIdParam);

	// Delete information of a particular contact item
	// 	DELETE FROM predictivesearchX (X=0..11)
	//	WHERE contact_id = [contact id value];
	iDeleteStmnt = TSqlProvider::GetSqlStatementL(deleteType);
	iDeleteStmnt->SetConditionL(*whereContactIdClause);
	CleanupStack::PopAndDestroy(whereContactIdClause);

	PRINT(_L("CPplPredictiveSearchTableBase::ConstructL ends"));
	}


/**
Constructor
*/
CPplPredictiveSearchTableBase::CPplPredictiveSearchTableBase(
	RSqlDatabase& aDatabase, TInt aMaxTokens, TInt aMaxTokenLength) :
	iDatabase(aDatabase),
	iMaxTokens(aMaxTokens),
	iMaxTokenLength(aMaxTokenLength)
	{
	}


QList<QChar> CPplPredictiveSearchTableBase::DetermineTables(QStringList aTokens) const
	{
	QList<QChar> tables;
	for (TInt i = aTokens.count() - 1; i >= 0; --i)
		{
		QChar ch = aTokens[i][0];
		__ASSERT_ALWAYS(IsValidChar(ch),
						User::Panic(_L("DetermineTables"), KErrArgument));
		if (!tables.contains(ch))
			{
			tables.append(ch);
			}
		}
	return tables;
	}


// Insert a contact to predictive search tables.
// Write contact's all tokens to each associate pred.search table.
// E.g. if FN="11 22" LN="2 333", write "11","22","2" and "333" to tables 1, 2 and 3.
void CPplPredictiveSearchTableBase::WriteToDbL(const CContactItem& aItem)
	{
	PRINT(_L("CPplPredictiveSearchTableBase::WriteToDbL"));

	HBufC* firstNameAsNbr(NULL); // owned
    HBufC* lastNameAsNbr(NULL);  // owned
	HBufC* firstName(NULL); // owned
    HBufC* lastName(NULL);  // owned
	GetFieldsLC(aItem, &firstNameAsNbr, &lastNameAsNbr, &firstName, &lastName);

	QStringList tokens;
	QList<QChar> tables;
	QT_TRYCATCH_LEAVING({
		TBool mandatoryFieldsPresent(EFalse);
		QStringList tableSpecificFields =
			GetTableSpecificFields(aItem, mandatoryFieldsPresent);
		if (mandatoryFieldsPresent)
			{
			tokens = GetTokens(tableSpecificFields, firstNameAsNbr, lastNameAsNbr);
			tables = DetermineTables(tokens);
			}
		});

	HBufC* tableName(NULL);
	while ((tableName = GetNextTableNameL(tables)) != NULL)
		{
		// Takes ownership. Clears also earlier SQL statement.
		iInsertStmnt->SetTableName(tableName);
		RSqlStatement stmnt;
		CleanupClosePushL( stmnt ); 
		PRINT1(_L("CPplPredictiveSearchTableBase::WriteToDbL SQL='%S'"),
			   &iInsertStmnt->SqlStringL());
		stmnt.PrepareL(iDatabase, iInsertStmnt->SqlStringL());

// TODO: while this works, it is inefficient, since the BIGINT values are
// computed in every iteration of the while-loop, even though the data is
// always the same.
		FillKeyboardSpecificFieldsL(stmnt, tokens);

		User::LeaveIfError(stmnt.BindInt(
			User::LeaveIfError(stmnt.ParameterIndex(KPredSearchContactIdParam)),
			aItem.Id()));

		if (firstName)
			{
			User::LeaveIfError(stmnt.BindText(
				User::LeaveIfError(stmnt.ParameterIndex(KPredSearchFirstNameParam)),
				*firstName));
			}
		if (lastName)
			{
			User::LeaveIfError(stmnt.BindText(
				User::LeaveIfError(stmnt.ParameterIndex(KPredSearchLastNameParam)),
				*lastName));
			}

//		PRINT(_L("CPplPredictiveSearchTableBase::WriteToDbL execute SQL statement"));
		// Execute the SQL statement
		User::LeaveIfError(stmnt.Exec());
		CleanupStack::PopAndDestroy(&stmnt);
		}

	CleanupStack::PopAndDestroy(lastNameAsNbr);
	CleanupStack::PopAndDestroy(lastName);
	CleanupStack::PopAndDestroy(firstNameAsNbr);
	CleanupStack::PopAndDestroy(firstName);

	PRINT(_L("CPplPredictiveSearchTableBase::WriteToDbL ends"));
	}


void CPplPredictiveSearchTableBase::GetFieldsLC(const CContactItem& aItem,
										  	    HBufC** aFirstNameAsNbr,
											    HBufC** aLastNameAsNbr,
											    HBufC** aFirstName,
  											    HBufC** aLastName) const
	{
	PRINT(_L("CPplPredictiveSearchTableBase::GetFieldsLC"));
	__ASSERT_ALWAYS(aFirstNameAsNbr != NULL && *aFirstNameAsNbr == NULL,
				    User::Leave(KErrArgument));
	__ASSERT_ALWAYS(aLastNameAsNbr != NULL && *aLastNameAsNbr == NULL,
				    User::Leave(KErrArgument));
	__ASSERT_ALWAYS(aFirstName != NULL && *aFirstName == NULL,
					User::Leave(KErrArgument));
	__ASSERT_ALWAYS(aLastName != NULL && *aLastName == NULL,
					User::Leave(KErrArgument));

	CContactItemFieldSet& fieldset = aItem.CardFields();
    TInt pos = fieldset.Find(KUidContactFieldGivenName);
    if (pos != KErrNotFound)
        {
        CContactTextField* textfield = fieldset[pos].TextStorage();
        if (textfield)
            {
            TPtrC firstName = textfield->Text();
			*aFirstName = firstName.Left(KCharactersFromName).AllocLC();
			*aFirstNameAsNbr = iKeyMap->GetMappedStringL(firstName);
            }
        }
	// If aFirstName was not pushed to cleanupstack above, do it now
	if (*aFirstName == NULL)
		{
		CleanupStack::PushL(*aFirstName);
		}
	CleanupStack::PushL(*aFirstNameAsNbr);

    pos = fieldset.Find(KUidContactFieldFamilyName);
    if (pos != KErrNotFound)
        {
        CContactTextField* textfield = fieldset[pos].TextStorage();
        if (textfield)
            {
            TPtrC lastName = textfield->Text();
			*aLastName = lastName.Left(KCharactersFromName).AllocLC();
			*aLastNameAsNbr = iKeyMap->GetMappedStringL(lastName);
            }
        }
	// If aLastName was not pushed to cleanupstack above, do it now
	if (*aLastName == NULL)
		{
		CleanupStack::PushL(*aLastName);	
		}
	CleanupStack::PushL(*aLastNameAsNbr);

	PRINT5(_L("CPplPredictiveSearchTableBase::GetFieldsLC id=%d FNnbr='%S' LNnbr='%S' FN='%S' LN='%S'"),
		aItem.Id(),
	    *aFirstNameAsNbr ? *aFirstNameAsNbr : &KNullDesC,
	    *aLastNameAsNbr ? *aLastNameAsNbr : &KNullDesC,
		*aFirstName ? *aFirstName : &KNullDesC,
	    *aLastName ? *aLastName: &KNullDesC);
	}


// 1. get first token of LN
// 2. get first token of FN
// 3. get second token of LN
// 4. get second token of FN
// :
// :
// If LN or FN runs out of tokens before maximum amount of tokens have been found,
// keep getting tokens from the other field.
QStringList
CPplPredictiveSearchTableBase::GetTokens(QStringList aNonTokenizedFields,
										 HBufC* aFirstName,
										 HBufC* aLastName) const
	{
	PRINT2(_L("CPplPredictiveSearchTableBase::GetTokens FN='%S',LN='%S'"),
		   aFirstName ? aFirstName : &KNullDesC,
		   aLastName ? aLastName : &KNullDesC);

	QStringList tokens;
	while (tokens.count() < iMaxTokens && !aNonTokenizedFields.isEmpty())
		{
		GetNextToken(aNonTokenizedFields, tokens);
		}

	QStringList firstNameTokens;
	QStringList lastNameTokens;
	AddTokens(aFirstName, firstNameTokens);
	AddTokens(aLastName, lastNameTokens);
	
	while (tokens.count() < iMaxTokens &&
		   (!firstNameTokens.isEmpty() || !lastNameTokens.isEmpty()))
		{
		GetNextToken(lastNameTokens, tokens);
		GetNextToken(firstNameTokens, tokens);
		}
	PRINT1(_L("CPplPredictiveSearchTableBase::GetTokens found %d tokens"),
           tokens.count());
	return tokens;
	}


// Ignore tokens beginning with invalid (unknown) character.
// Keep duplicate tokens to support e.g. search "202" when both FN and LN are "23".
void
CPplPredictiveSearchTableBase::AddTokens(HBufC* aString, QStringList& aTokens) const
	{
	if (aString)
		{
		QString s((QChar*)aString->Ptr(), aString->Length());
#if defined(USE_ORBIT_KEYMAP)
		QStringList tokens = s.split(iKeyMap->Separator(), QString::SkipEmptyParts);
#else
		QStringList tokens = s.split(' ', QString::SkipEmptyParts);
#endif

		// Select tokens in the same order they are in original aString
		for (TInt i = 0; i < tokens.count(); ++i)
			{
			if (IsValidChar(tokens[i][0]))
				{
				aTokens.append(tokens[i]);
				}
			}
		}
	}


void CPplPredictiveSearchTableBase::GetNextToken(QStringList& aSource,
												 QStringList& aDestination) const
	{
	if (!aSource.isEmpty() && aDestination.count() < iMaxTokens)
		{
        QString padded = aSource[0].left(iMaxTokenLength);
		aDestination.append(padded);
		aSource.removeFirst();
		}
	}


void
CPplPredictiveSearchTableBase::DeleteFromAllTablesL(TContactItemId aContactId,
												    TBool& aLowDiskErrorOccurred) const
	{
	QList<QChar> tables;
	QT_TRYCATCH_LEAVING(tables = FillAllTables());

	HBufC* tableName(NULL);
	while ((tableName = GetNextTableNameL(tables)) != NULL)
		{
		iDeleteStmnt->SetTableName(tableName); // Clears also earlier SQL statement

		RSqlStatement stmnt;
		CleanupClosePushL(stmnt);

		PRINT1(_L("CPplPredictiveSearchTableBase::DeleteFromAllTablesL SQL='%S'"),
			   &iDeleteStmnt->SqlStringL());
		stmnt.PrepareL(iDatabase, iDeleteStmnt->SqlStringL());
	
		// Contact id was not added with iDeleteStmnt->SetParamL() so it can not be
		// accessed with iDeleteStmnt->ParameterIndex().
		// It is the first and only parameter in query
		const TInt KContactIdParamIndex(KFirstIndex);
		User::LeaveIfError(stmnt.BindInt(KContactIdParamIndex, aContactId));

		// Returns the amount of affected rows. As contact is not present each
		// table, some operations return 0, it is not an error.
		TInt status = stmnt.Exec();
#if defined(WRITE_PRED_SEARCH_LOGS)
		if (status != 0)
			{
			PRINT1(_L("  rows deleted=%d"), status);
			}
#endif
		CleanupStack::PopAndDestroy(&stmnt);

		if (status == KErrDiskFull)
			{
			PRINT(_L("CPplPredictiveSearchTableBase::DeleteFromAllTablesL disk full"));
			aLowDiskErrorOccurred = ETrue;
			}
		else
			{
			User::LeaveIfError(status);
			}
		}
	}
