/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "plplugins.h"
#include <cntdb.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <cntphonenumparser.h>
#endif

/**
@param aDatabase A handle to the database.
@param aProperties A contact properties object.

@return A pointer to a new CPplCommAddrTable object.
*/
CPplCommAddrTable* CPplCommAddrTable::NewL(RSqlDatabase& aDatabase, CLplContactProperties& aProperties)
	{
	CPplCommAddrTable* self = CPplCommAddrTable::NewLC(aDatabase, aProperties);
	CleanupStack::Pop(self);
	return self;
	}


/**
@param aDatabase A handle to the database.
@param aProperties A contact properties object.

@return A pointer to a new CPplCommAddrTable object.
*/
CPplCommAddrTable* CPplCommAddrTable::NewLC(RSqlDatabase& aDatabase, CLplContactProperties& aProperties)
	{
	CPplCommAddrTable* self = new (ELeave) CPplCommAddrTable(aDatabase, aProperties);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

/**
Set up the CCntSqlStatement objects held by the class.
*/
void CPplCommAddrTable::ConstructL()
	{
	// Statement types
	TCntSqlStatementType insertType(EInsert, KSqlContactCommAddrTableName() );
	TCntSqlStatementType selectType(ESelect, KSqlContactCommAddrTableName() );
	TCntSqlStatementType updateType(EUpdate, KSqlContactCommAddrTableName() );
	TCntSqlStatementType deleteType(EDelete, KSqlContactCommAddrTableName() );

	// Where clauses

	// sizes of the clauses
	const TInt KWhereContactIdBufSize(KCommAddrContactId().Size() +
		KWhereStringEqualsStringFormatText().Size() + KCommAddrContactIdParam().Size() );
	const TInt KWhereCommAddrIdBufSize(KCommAddrId().Size() +
		KWhereStringEqualsStringFormatText().Size() + KCommAddrIdParam().Size() );
	const TInt KWhereAndClauseBufSize(KWhereCommAddrIdBufSize + KSqlAnd().Size() + KWhereCommAddrIdBufSize);

	HBufC* whereContactIdClause = HBufC::NewLC(KWhereContactIdBufSize);
	// for WHERE contact_id = [contact id value]
	whereContactIdClause->Des().AppendFormat(KWhereStringEqualsStringFormatText,
		&KCommAddrContactId, &KCommAddrContactIdParam );

	// for WHERE comm_addr_id = [comm addr id value]
	HBufC* whereCommAddrIdClause = HBufC::NewLC(KWhereCommAddrIdBufSize);
	whereCommAddrIdClause->Des().AppendFormat(KWhereStringEqualsStringFormatText,
		&KCommAddrId, &KCommAddrIdParam );

	// for WHERE value = [value string] AND type = [type value]
	HBufC* whereValueAndTypeClause = HBufC::NewLC(KWhereAndClauseBufSize);
	TPtr whereValueAndTypeClausePtr = whereValueAndTypeClause->Des();
	whereValueAndTypeClausePtr.AppendFormat(KWhereStringEqualsStringFormatText,
		&KCommAddrValue, &KCommAddrValueParam );
	whereValueAndTypeClausePtr.Append(KSqlAnd);
	whereValueAndTypeClausePtr.AppendFormat(KWhereStringEqualsStringFormatText,
		&KCommAddrType, &KCommAddrTypeParam );

	// INSERT

	// insert new comm addr record
	// 	INSERT INTO comm_addr
	//		(comm_addr_id, contact_id, type, value, extra_value)
	//		VALUES (NULL, [contact id value], [type value],
	//			[value string], [extra value string]);
	//
	iInsertStmnt = TSqlProvider::GetSqlStatementL(insertType);
	iInsertStmnt->SetParamL(KCommAddrId(), KCommAddrIdParam() );
	iInsertStmnt->SetParamL(KCommAddrContactId(), KCommAddrContactIdParam() );
	iInsertStmnt->SetParamL(KCommAddrValue(), KCommAddrValueParam() );
	iInsertStmnt->SetParamL(KCommAddrExtraValue(), KCommAddrExtraValueParam() );
	iInsertStmnt->SetParamL(KCommAddrType(), KCommAddrTypeParam() );

	// SELECT

	// select all fields for all comm addrs for a particular item id
	// For a statement in the following format:
	// 	SELECT comm_addr_id, type, value, extra_value FROM comm_addr
	//		WHERE contact_id = [contact id value];
	//
	iWholeSelectStmnt = TSqlProvider::GetSqlStatementL(selectType);
	iWholeSelectStmnt->SetParamL(KCommAddrId(), KNullDesC() );
	iWholeSelectStmnt->SetParamL(KCommAddrType(), KNullDesC() );
	iWholeSelectStmnt->SetParamL(KCommAddrValue(), KNullDesC() );
	iWholeSelectStmnt->SetParamL(KCommAddrExtraValue(), KNullDesC() );
	iWholeSelectStmnt->SetConditionL(*whereContactIdClause);

	// select fields for contacts that match phone, email or SIP lookup
	// For a statement in the following format:
	// 	SELECT contact_id, extra_value FROM comm_addr
	//		WHERE value = [value string] AND type = [type value];
	//
	iMatchSelectStmnt = TSqlProvider::GetSqlStatementL(selectType);
	iMatchSelectStmnt->SetParamL(KCommAddrContactId(), KNullDesC() );
	iMatchSelectStmnt->SetParamL(KCommAddrExtraValue(), KNullDesC() );
	iMatchSelectStmnt->SetConditionL(*whereValueAndTypeClause);

	// UPDATE

	// update comm addr record
	// For a statement in the following format:
	// 	UPDATE comm_addr SET
	//		contact_id = [contact id value],
	//		type = [type value],
	//		value = [value string],
	//		extra_value = [extra value string]
	//		WHERE comm_addr_id = [comm addr id value];
	//
	iUpdateStmnt = TSqlProvider::GetSqlStatementL(updateType);
	iUpdateStmnt->SetParamL(KCommAddrContactId(), KCommAddrContactIdParam() );
	iUpdateStmnt->SetParamL(KCommAddrValue(), KCommAddrValueParam() );
	iUpdateStmnt->SetParamL(KCommAddrExtraValue(), KCommAddrExtraValueParam() );
	iUpdateStmnt->SetParamL(KCommAddrType(), KCommAddrTypeParam() );
	iUpdateStmnt->SetConditionL(*whereCommAddrIdClause);

	// DELETE

	// delete single comm addr record
	// For a statement in the following format:
	// 	DELETE FROM comm_addr WHERE comm_addr_id = [comm addr id value];
	//
	iSingleDeleteStmnt = TSqlProvider::GetSqlStatementL(deleteType);
	iSingleDeleteStmnt->SetConditionL(*whereCommAddrIdClause);

	// delete all comm addrs for a particular contact item
	// For a statement in the following format:
	// 	DELETE FROM comm_addr WHERE contact_id = [contact id value];
	//
	iAllForItemDeleteStmnt = TSqlProvider::GetSqlStatementL(deleteType);
	iAllForItemDeleteStmnt->SetConditionL(*whereContactIdClause);

	CleanupStack::PopAndDestroy(3, whereContactIdClause); // and whereCommAddrIdClause, whereValueAndTypeClause
	}


/**
Insert new communication addresses into the comm_addr table.
@param aItem A contact item whose communication addresses are to be added to the contacts database.
*/
void CPplCommAddrTable::CreateInDbL(CContactItem& aItem)
	{
	// Check that the contact item is a card, own card or ICC entry.
	const TUid KType = aItem.Type();
	if (KType != KUidContactCard && KType != KUidContactOwnCard && KType != KUidContactICCEntry)
		{
		return;
		}

	// create lists for comm_addrs to keep track of what we have already seen so as to avoid duplicates
	RArray<TMatch> newPhones;
	RArray<TPtrC>  newEmails;
	RArray<TPtrC>  newSips;
	CleanupClosePushL(newPhones);
	CleanupClosePushL(newEmails);
	CleanupClosePushL(newSips);

	for (TInt fieldNum = aItem.CardFields().Count() - 1; fieldNum >= 0; --fieldNum)
		{
		CContactItemField& currField = aItem.CardFields()[fieldNum];
		const CContentType& contType = currField.ContentType();
		TBool isPhone(contType.ContainsFieldType(KUidContactFieldPhoneNumber) ||
					  contType.ContainsFieldType(KUidContactFieldFax)		  ||
					  contType.ContainsFieldType(KUidContactFieldSms)		  );
		TBool isEmail(contType.ContainsFieldType(KUidContactFieldEMail) );
		TBool isSip(contType.ContainsFieldType(KUidContactFieldSIPID) );

		// check it's a field we want and that it's not empty
		// insert a new address only if we haven't already seen it -- no point storing the same one twice.
		if ((isPhone || isEmail || isSip) && currField.StorageType() == KStorageTypeText
				&& currField.TextStorage()->IsFull() )
			{
			const TContactItemId KItemId(aItem.Id());

			// get phone numbers
			if (isPhone)
				{
				TMatch phoneNumber;
				phoneNumber = CreatePaddedPhoneDigitsL(currField.TextStorage()->Text(), KLowerSevenDigits,
													   KMaxPhoneMatchLength - KLowerSevenDigits);
				if (newPhones.Find(phoneNumber, TIdentityRelation<TMatch>(&TMatch::Equals) ) == KErrNotFound)
					{
					DoPhoneNumWriteOpL(phoneNumber, EInsert, KItemId);
					newPhones.AppendL(phoneNumber);
					}
				}
			// get email addresses
			else if (isEmail && newEmails.Find(currField.TextStorage()->Text() ) == KErrNotFound)
				{
				DoNonPhoneWriteOpL(currField.TextStorage()->Text(), EInsert, KItemId, EEmailAddress);
				newEmails.AppendL(currField.TextStorage()->Text() );
				}
			// get SIP addresses
			else if (newSips.Find(currField.TextStorage()->Text() ) == KErrNotFound)
				{
				DoNonPhoneWriteOpL(currField.TextStorage()->Text(), EInsert, KItemId, ESipAddress);
				newSips.AppendL(currField.TextStorage()->Text() );
				}
			}
		}

	CleanupStack::PopAndDestroy(3, &newPhones); // and newSips, newEmails
	}


/**
Updates communication addresses in the database.

If there are the same number of items to be updated as are already in the database, the
existing records are overwritten using the update statement. However, if there is a
different number of addresses to be updated, records in the database (if there are any)
are deleted (using DeleteL() ) and the new data is inserted (using CreateInDbL() ).

@param aItem A contact item whose communication addresses are to be updated in the contacts database.
*/
void CPplCommAddrTable::UpdateL(const CContactItem& aItem)
	{
	// Check that the contact item is a card, own card or ICC entry.
	const TUid type(aItem.Type() );
	if (type != KUidContactCard && type != KUidContactOwnCard && type != KUidContactICCEntry)
		{
		return;
		}

	const TContactItemId KItemId(aItem.Id() );

	// create lists for comm_addrs and go through contact item to populate them with any new ones we find
	RArray<TMatch> newPhones;
	RArray<TPtrC>  newEmails;
	RArray<TPtrC>  newSips;
	CleanupClosePushL(newPhones);
	CleanupClosePushL(newEmails);
	CleanupClosePushL(newSips);

	for (TInt fieldNum = aItem.CardFields().Count() - 1; fieldNum >= 0; --fieldNum)
		{
		CContactItemField& currField = aItem.CardFields()[fieldNum];
		const CContentType& contType = currField.ContentType();
		TBool isPhone(contType.ContainsFieldType(KUidContactFieldPhoneNumber) ||
					  contType.ContainsFieldType(KUidContactFieldFax)		  ||
					  contType.ContainsFieldType(KUidContactFieldSms)		  );
		TBool isEmail(contType.ContainsFieldType(KUidContactFieldEMail) );
		TBool isSip(contType.ContainsFieldType(KUidContactFieldSIPID) );

		// check it's a field we want and that it's not empty
		// store a new address if we haven't already seen it -- no point storing the same one twice.
		if ((isPhone || isEmail || isSip) && currField.StorageType() == KStorageTypeText
				&& currField.TextStorage()->IsFull() )
			{
			// get phone numbers
			if (isPhone)
				{
				TMatch phoneNumber;
				phoneNumber = CreatePaddedPhoneDigitsL(currField.TextStorage()->Text(), KLowerSevenDigits,
													   KMaxPhoneMatchLength - KLowerSevenDigits);
				if (newPhones.Find(phoneNumber, TIdentityRelation<TMatch>(&TMatch::Equals) ) == KErrNotFound)
					{
					newPhones.AppendL(phoneNumber);
					}
				}
			// get email addresses
			else if (isEmail && newEmails.Find(currField.TextStorage()->Text() ) == KErrNotFound)
				{
				newEmails.AppendL(currField.TextStorage()->Text() );
				}
			// get SIP addresses
			else if (newSips.Find(currField.TextStorage()->Text() ) == KErrNotFound)
				{
				newSips.AppendL(currField.TextStorage()->Text() );
				}
			}
		}

	// if there are no comm addresses in the contact item, delete any from the database
	if (!(newPhones.Count() + newEmails.Count() + newSips.Count() ) )
		{
		TBool lowDiskErr(EFalse);
		DeleteL(aItem, lowDiskErr);
		CleanupStack::PopAndDestroy(3, &newPhones); // and newSips, newEmails
		if (lowDiskErr)
			{
			User::Leave(KErrDiskFull);
			}
		return;
		}

	// create from the database a list of comm_addr_ids that can be recycled as their
	// comm_addrs are in the database but not in the new version of the contact item
	RArray<TInt> freeCommAddrIds;
	CleanupClosePushL(freeCommAddrIds);

	// weed out addresses from the list that are already in the db but haven't changed
	// and populate the freeCommAddrIds list
	RemoveNonUpdatedAddrsL(newPhones, newEmails, newSips, freeCommAddrIds, KItemId);

	// do the actual updating on an address-by-address basis
	DoUpdateCommAddrsL(newPhones, newEmails, newSips, freeCommAddrIds, KItemId);

	CleanupStack::PopAndDestroy(4, &newPhones); // and freeCommAddrIds, newSips, newEmails
	}


/**
Removes comm addresses from the 3 lists that are already in the database and have been updated.
It takes the 3 lists in as parameters and modifies them accordingly. It also populates the list
of comm address ids that are free to be recycled during updating.
*/
void CPplCommAddrTable::RemoveNonUpdatedAddrsL(RArray<TMatch>& aNewPhones, RArray<TPtrC>& aNewEmails, RArray<TPtrC>& aNewSips,
					  RArray<TInt>& aFreeCommAddrIds, const TInt aItemId)
	{
	// build the RSqlStatement
	RSqlStatement stmnt;
	CleanupClosePushL(stmnt);
	stmnt.PrepareL(iDatabase, iWholeSelectStmnt->SqlStringL() );
	const TInt KContactIdParamIndex(KFirstIndex); // first and only parameter in the query
	User::LeaveIfError(stmnt.BindInt(KContactIdParamIndex, aItemId) ) ;

	// fetch the results from the query and compare them with the new comm_addrs we have
	TInt err(KErrNone);
	while ((err = stmnt.Next() ) == KSqlAtRow)
		{
		const TInt KType(stmnt.ColumnInt(iWholeSelectStmnt->ParameterIndex(KCommAddrType() ) ) );
		if (KType == EPhoneNumber)
			{
			TMatch phoneNumber;
			TPtrC valString    = stmnt.ColumnTextL(iWholeSelectStmnt->ParameterIndex(KCommAddrValue() ) );
			TPtrC extValString = stmnt.ColumnTextL(iWholeSelectStmnt->ParameterIndex(KCommAddrExtraValue() ) );
			User::LeaveIfError(TLex(valString).Val(phoneNumber.iLowerSevenDigits) );
			User::LeaveIfError(TLex(extValString).Val(phoneNumber.iUpperDigits) );

			TInt matchIndex(aNewPhones.Find(phoneNumber, TIdentityRelation<TMatch>(&TMatch::Equals) ) );
			// remove any phone numbers from the new list if we already
			// have them in the db and they haven't changed...
			if (matchIndex != KErrNotFound)
				{
				aNewPhones.Remove(matchIndex);
				}
			// ...and add any spare ids to the recycle list
			else
				{
				aFreeCommAddrIds.AppendL(
					stmnt.ColumnInt(iWholeSelectStmnt->ParameterIndex(KCommAddrId() ) ) );
				}
			}
		else // is Email or SIP
			{
			TPtrC valString = stmnt.ColumnTextL(iWholeSelectStmnt->ParameterIndex(KCommAddrValue() ) );
			TInt matchIndex(0);

			// remove any email and sip addresses from the new list if
			// we already have them in the db and they haven't changed...
			if (KType == EEmailAddress)
				{
				matchIndex = aNewEmails.Find(valString);
				if (matchIndex != KErrNotFound)
					{
					aNewEmails.Remove(matchIndex);
					}
				}
			else // SIP
				{
				matchIndex = aNewSips.Find(valString);
				if (matchIndex != KErrNotFound)
					{
					aNewSips.Remove(matchIndex);
					}
				}

			// ...and add any spare ids to the recycle list
			if (matchIndex == KErrNotFound)
				{
				aFreeCommAddrIds.AppendL(
					stmnt.ColumnInt(iWholeSelectStmnt->ParameterIndex(KCommAddrId() ) ) );
				}
			}
		}
	// leave if we didn't complete going through the results properly
	if(err != KSqlAtEnd)
		{
		User::Leave(err);
		}
	CleanupStack::PopAndDestroy(&stmnt);
	}


/**
Does comm_addr updating on an address-by-address basis.
Takes 3 lists of new addresses and a list of free comm_addr_ids.
*/
void CPplCommAddrTable::DoUpdateCommAddrsL(RArray<TMatch>& aNewPhones, RArray<TPtrC>& aNewEmails, RArray<TPtrC>& aNewSips,
					  RArray<TInt>& aFreeCommAddrIds, const TInt aItemId)
	{
	// if we have free ids to recycle and new comm_addrs, insert them by UPDATE
	const TInt KFirstElementId(0);
	while (aFreeCommAddrIds.Count() && (aNewPhones.Count() || aNewEmails.Count() || aNewSips.Count() ) )
		{
		if(aNewPhones.Count() )
			{
			DoPhoneNumWriteOpL(aNewPhones[KFirstElementId], EUpdate, aItemId, aFreeCommAddrIds[KFirstElementId]);
			aNewPhones.Remove(KFirstElementId);
			aFreeCommAddrIds.Remove(KFirstElementId);
			}
		else if(aNewEmails.Count() )
			{
			DoNonPhoneWriteOpL(aNewEmails[KFirstElementId], EUpdate, aItemId, EEmailAddress,
								aFreeCommAddrIds[KFirstElementId]);
			aNewEmails.Remove(KFirstElementId);
			aFreeCommAddrIds.Remove(KFirstElementId);
			}
		else if(aNewSips.Count() )
			{
			DoNonPhoneWriteOpL(aNewSips[KFirstElementId], EUpdate, aItemId, ESipAddress,
								aFreeCommAddrIds[KFirstElementId]);
			aNewSips.Remove(KFirstElementId);
			aFreeCommAddrIds.Remove(KFirstElementId);
			}
		}

	// if we still have free ids to recycle but no new comm_addrs,
	// delete the existing comm_addrs for these ids
	const TInt KNumFreeIds(aFreeCommAddrIds.Count() );
	for (TInt i = 0; i < KNumFreeIds; ++i)
		{
		TBool lowDiskErr(EFalse);
		DeleteSingleCommAddrL(aFreeCommAddrIds[i], lowDiskErr);
		if (lowDiskErr)
			{
			User::Leave(KErrDiskFull);
			}
		}

	// if we still have new comm_addrs but no free ids to recycle,
	// put them in the database using INSERT
	const TInt KNumNewPhones(aNewPhones.Count() );
	const TInt KNumNewEmails(aNewEmails.Count() );
	const TInt KNumNewSips(aNewSips.Count() );
	for (TInt i = 0; i < KNumNewPhones; ++i)
		{
		DoPhoneNumWriteOpL(aNewPhones[i], EInsert, aItemId);
		}
	for (TInt i = 0; i < KNumNewEmails; ++i)
		{
		DoNonPhoneWriteOpL(aNewEmails[i], EInsert, aItemId, EEmailAddress);
		}
	for (TInt i = 0; i < KNumNewSips; ++i)
		{
		DoNonPhoneWriteOpL(aNewSips[i], EInsert, aItemId, ESipAddress);
		}
	}


/**
Deletes individual communication addresses from the database. In other words, deletes at
the sub-contact item level rather than deleting everything with a specific contact item ID.
*/
void CPplCommAddrTable::DeleteSingleCommAddrL(TInt aCommAddrId, TBool& aLowDiskErrorOccurred)
	{
	RSqlStatement stmnt;
	CleanupClosePushL(stmnt);
	stmnt.PrepareL(iDatabase, iSingleDeleteStmnt->SqlStringL() );
	const TInt KCommAddrIdParamIndex(KFirstIndex); // first and only parameter in the query
	User::LeaveIfError(stmnt.BindInt(KCommAddrIdParamIndex, aCommAddrId ) );
	TInt err = stmnt.Exec();
	CleanupStack::PopAndDestroy(&stmnt);

	if (err == KErrDiskFull)
		{
		aLowDiskErrorOccurred = ETrue;
		}
	else
		{
		User::LeaveIfError(err);
		}
	}

/**
Performs write operations for individual communication addresses of type "phone number".
This version of the method does not require a CommAddrId argument. Therefore, this can only be used
for insertion and not updating.
*/
void CPplCommAddrTable::DoPhoneNumWriteOpL(const CPplCommAddrTable::TMatch& aPhoneNum, TCntSqlStatement aType,
											TInt aCntId)
	{
	// provide a commaddr of 0 as a default argument
	DoPhoneNumWriteOpL(aPhoneNum, aType, aCntId, 0);
	}


/**
Performs write (Insert/Update) operations for indiviual communication addresses of type "phone number".
*/
void CPplCommAddrTable::DoPhoneNumWriteOpL(const CPplCommAddrTable::TMatch& aPhoneNum, TCntSqlStatement aType,
											TInt aCntId, TInt aCommAddrId)
	{
	// leave if the statement type is not insert or update.
	// also, we can't update if aCommAddrId is 0 as we don't know the record's id
	if ((aType != EUpdate && aType != EInsert) || (aType == EUpdate && aCommAddrId == 0) )
		{
		User::Leave(KErrArgument);
		}

	RSqlStatement stmnt;
	CleanupClosePushL(stmnt);

	// temporary reference to the CCntSqlStatements member variables to take advantage
	// of the commonality between update and insert operations.
	CCntSqlStatement* tempCntStmnt = iUpdateStmnt;
	if (aType == EInsert)
		{
		tempCntStmnt = iInsertStmnt;
		}

	User::LeaveIfError(stmnt.Prepare(iDatabase, tempCntStmnt->SqlStringL() ) );
	User::LeaveIfError(stmnt.BindInt(tempCntStmnt->ParameterIndex(KCommAddrContactId() ), aCntId) );
	User::LeaveIfError(stmnt.BindInt(tempCntStmnt->ParameterIndex(KCommAddrExtraValue() ), aPhoneNum.iUpperDigits) );
	User::LeaveIfError(stmnt.BindInt(tempCntStmnt->ParameterIndex(KCommAddrValue() ), aPhoneNum.iLowerSevenDigits) );
	User::LeaveIfError(stmnt.BindInt(tempCntStmnt->ParameterIndex(KCommAddrType() ), EPhoneNumber) );

	if (aType == EInsert)
		{
		User::LeaveIfError(stmnt.BindNull(tempCntStmnt->ParameterIndex(KCommAddrId() ) ) );
		}
	else
		{
		// it's the fifth parameter in the query and is in the WHERE
		// clause so we can't get its index from the CCntSqlStatement
		const TInt KCommAddrIdParamIndex(KFirstIndex + 4);
		User::LeaveIfError(stmnt.BindInt(KCommAddrIdParamIndex, aCommAddrId) );
		}

	User::LeaveIfError(stmnt.Exec() );
	CleanupStack::PopAndDestroy(&stmnt);
	}


/**
Performs write operations for individual communication addresses of type "email" or "SIP" address.
This version of the method does not require a CommAddrId argument. Therefore, this can only be used
for insertion and not updating.
*/
void CPplCommAddrTable::DoNonPhoneWriteOpL(const TDesC& aAddress, const TCntSqlStatement aType,
										 TInt aCntId, TCommAddrType aAddrType)
	{
	// provide a commaddr of 0 as a default argument
	DoNonPhoneWriteOpL(aAddress, aType, aCntId, aAddrType, 0);
	}


/**
Performs write (Insert/Update) operations for indiviual communication addresses of
type "Email address" or "SIP Address".
*/
void CPplCommAddrTable::DoNonPhoneWriteOpL(const TDesC& aAddress, const TCntSqlStatement aType,
										 TInt aCntId, TCommAddrType aAddrType, TInt aCommAddrId)
	{
	// leave if the statement type is not insert or update.
	// also, we can't update if aCommAddrId is 0 as we don't know the record's id
	if ((aType != EUpdate && aType != EInsert) || (aType == EUpdate && aCommAddrId == 0) )
		{
		User::Leave(KErrNotFound);
		}

	RSqlStatement stmnt;
	CleanupClosePushL(stmnt);

	// temporary reference to the CCntSqlStatements member variables to take advantage
	// of the commonality between update and insert operations.
	CCntSqlStatement* tempCntStmnt = iUpdateStmnt;
	if (aType == EInsert)
		{
		tempCntStmnt = iInsertStmnt;
		}

	User::LeaveIfError(stmnt.Prepare(iDatabase, tempCntStmnt->SqlStringL() ) );
	User::LeaveIfError(stmnt.BindInt(tempCntStmnt->ParameterIndex(KCommAddrContactId() ), aCntId) );
	User::LeaveIfError(stmnt.BindText(tempCntStmnt->ParameterIndex(KCommAddrValue() ), aAddress) );
	User::LeaveIfError(stmnt.BindNull(tempCntStmnt->ParameterIndex(KCommAddrExtraValue() ) ) );
	User::LeaveIfError(stmnt.BindInt(tempCntStmnt->ParameterIndex(KCommAddrType() ), aAddrType) );

	if (aType == EInsert)
		{
		User::LeaveIfError(stmnt.BindNull(tempCntStmnt->ParameterIndex(KCommAddrId() ) ) );
		}
	else
		{
		// it's the fifth parameter in the query and is in the WHERE
		// clause so we can't get its index from the CCntSqlStatement
		const TInt KCommAddrIdParamIndex(KFirstIndex + 4);
		User::LeaveIfError(stmnt.BindInt(KCommAddrIdParamIndex, aCommAddrId) );
		}

	User::LeaveIfError(stmnt.Exec() );
	CleanupStack::PopAndDestroy(&stmnt);
	}


/**
Deletes all the communication addresses for a particular contact item. Should be used when
deleting a contact item from the database altogether.

@param aItem The contact item whose communcation addresses are to be deleted.
*/
void CPplCommAddrTable::DeleteL(const CContactItem& aItem, TBool& aLowDiskErrorOccurred)
	{
	const TUid KType = aItem.Type();
	if (KType != KUidContactCard && KType != KUidContactOwnCard && KType != KUidContactICCEntry)
		{
		return;
		}

	RSqlStatement stmnt;
	CleanupClosePushL(stmnt);
	stmnt.PrepareL(iDatabase, iAllForItemDeleteStmnt->SqlStringL() );
	const TInt KContactIdParamIndex(KFirstIndex); // first and only parameter in query
	User::LeaveIfError(stmnt.BindInt(KContactIdParamIndex, aItem.Id() ) );
	TInt err = stmnt.Exec();
	CleanupStack::PopAndDestroy(&stmnt);

	if (err == KErrDiskFull)
		{
		aLowDiskErrorOccurred = ETrue;
		}
	else
		{
		User::LeaveIfError(err);
		}
	}


/**
Creates the comm_addr table and its indexes in the database.
*/
void CPplCommAddrTable::CreateTableL()
	{
	User::LeaveIfError(iDatabase.Exec(KCommAddrCreateStmnt) );
	}


/**
Returns an array of contact item IDs for all the contact items which may contain
the specified telephone number in a telephone, fax or SMS type field.

The comparison method used is not exact.  The number is compared starting from
the right side of the number and the method returns an array of candidate
matches.  Punctuation (e.g. spaces) and other alphabetic characters are ignored
when comparing.

Additionally, if the Contacts Model Phone Parser (CNTPHONE.DLL) is available,
then any DTMF digits are also excluded from the comparision.

Note that due to the way numbers are stored in the database, it is recommended
that at least 7 match digits are specified even when matching a number
containing fewer digits.  Failure to follow this guideline may (depending on the
database contents) mean that the function will not return the expected Contact
IDs.

@param aNumber Phone number string.
@param aMatchLengthFromRight Number of digits from the right of the phone number
to use.  Up to 15 digits can be specified, and it is recommended that at least 7
match digits are specified.
@param aDatabase The database.

@return Array of contact IDs which are candidate matches.
*/
CContactIdArray* CPplCommAddrTable::MatchPhoneNumberL(const TDesC& aNumber, const TInt aMatchLengthFromRight)
	{
	CContactIdArray* phoneMatchArray = CContactIdArray::NewLC();

	TInt numLowerDigits = aMatchLengthFromRight;
	TInt numUpperDigits = 0;

	if(numLowerDigits > KLowerSevenDigits)
		{
		// New style matching.
		numLowerDigits = KLowerSevenDigits;
		numUpperDigits = aMatchLengthFromRight - KLowerSevenDigits;
		}

	TMatch phoneDigits = CreatePaddedPhoneDigitsL(aNumber, numLowerDigits, numUpperDigits);

	if (phoneDigits.iNumLowerDigits + phoneDigits.iNumUpperDigits > 0)
		{
		// build statement
		RSqlStatement stmnt;
		CleanupClosePushL(stmnt);
		stmnt.PrepareL(iDatabase, iMatchSelectStmnt->SqlStringL() );

		const TInt KValueParamIndex(KFirstParam);			// first parameter in query...
		const TInt KTypeParamIndex(KValueParamIndex + 1);	// ...and the second.

    	User::LeaveIfError(stmnt.BindInt(KValueParamIndex, phoneDigits.iLowerSevenDigits ));
    	User::LeaveIfError(stmnt.BindInt(KTypeParamIndex, EPhoneNumber ));

		// fetch the list of any matching contact ids
		TInt err(KErrNone);
		const TInt KContactIdIdx(iMatchSelectStmnt->ParameterIndex(KCommAddrContactId() ) );
		const TInt KExtraValueIdx(iMatchSelectStmnt->ParameterIndex(KCommAddrExtraValue() ) );
		while ((err = stmnt.Next() ) == KSqlAtRow)
			{
			if (aMatchLengthFromRight <= KLowerSevenDigits)
				{
				// Matching 7 or less digits...we've already matched.
				phoneMatchArray->AddL(stmnt.ColumnInt(KContactIdIdx) );
				}
			else
				{
				// Check the upper digits...
				TInt32 storedUpperDigits(0);
				TPtrC extValString = stmnt.ColumnTextL(KExtraValueIdx);
				User::LeaveIfError(TLex(extValString).Val(storedUpperDigits) );

				const TInt KDigitsToRemove = KMaxPhoneMatchLength - KLowerSevenDigits - phoneDigits.iNumUpperDigits;
				for(TInt i = 0; i < KDigitsToRemove; ++i)
					{
					// repeatedly divide by 10 to lop off the appropriate number of digits from the right
					storedUpperDigits /= 10;
					}

				storedUpperDigits = TMatch::PadOutPhoneMatchNumber(storedUpperDigits, KDigitsToRemove);

				if (phoneDigits.iUpperDigits == storedUpperDigits)
					{
					phoneMatchArray->AddL(stmnt.ColumnInt(KContactIdIdx) );
					}
				}
			}

		// leave if we didn't complete going through the results properly
		if(err != KSqlAtEnd)
			{
			User::Leave(err);
			}
		CleanupStack::PopAndDestroy(&stmnt);
		}

	CleanupStack::Pop(phoneMatchArray);
	return phoneMatchArray;
	}

/**
Returns an array of contact item IDs for all the contact items which may contain
the specified telephone number in a telephone, fax or SMS type field.

This is improved version of MatchPhoneNumberL method.
The number is compared starting from the right side of the number and 
the method returns an array of candidate matches.  
Punctuation (e.g. spaces) and other alphabetic characters are ignored
when comparing. Leading zeros are removed. Digits are compared up to 
the lenght of shorter number.

@param aNumber Phone number string.
@return Array of contact IDs which are candidate matches.
*/
CContactIdArray* CPplCommAddrTable::BestMatchingPhoneNumberL(const TDesC& aNumber)
    {
    const TInt KUpperMaxLength = KMaxPhoneMatchLength - KLowerSevenDigits;

    CContactIdArray* phoneMatchArray = CContactIdArray::NewLC();

    TMatch phoneDigits = CreatePaddedPhoneDigitsL(aNumber, KLowerSevenDigits, KUpperMaxLength);

    if (phoneDigits.iNumLowerDigits + phoneDigits.iNumUpperDigits > 0)
        {
        // build statement
        RSqlStatement stmnt;
        CleanupClosePushL(stmnt);
        stmnt.PrepareL(iDatabase, iMatchSelectStmnt->SqlStringL());

        const TInt KValueParamIndex(KFirstParam); // first parameter in query...
        const TInt KTypeParamIndex(KValueParamIndex + 1); // ...and the second.

        User::LeaveIfError(stmnt.BindInt(KValueParamIndex,
                phoneDigits.iLowerSevenDigits));
        User::LeaveIfError(stmnt.BindInt(KTypeParamIndex, EPhoneNumber));

        // fetch the list of any matching contact ids
        TInt err(KErrNone);
        const TInt KContactIdIdx(iMatchSelectStmnt->ParameterIndex( KCommAddrContactId()));
        const TInt KExtraValueIdx(iMatchSelectStmnt->ParameterIndex(KCommAddrExtraValue()));
        while ((err = stmnt.Next()) == KSqlAtRow)
            {
            // Check the upper digits...
            TInt32 number = phoneDigits.iUpperDigits;
            TPtrC extValString = stmnt.ColumnTextL(KExtraValueIdx);
            TInt32 storedUpperDigits;
            User::LeaveIfError(TLex(extValString).Val(storedUpperDigits));
            TInt32 stored = storedUpperDigits;

            TBool nonZeroInStoredFound = EFalse;
            TBool nonZeroInNumberFound = EFalse;
            while ((number != 0) && (stored != 0))
                {
                nonZeroInNumberFound |= (number % 10 != 0);
                nonZeroInStoredFound |= (stored % 10 != 0);
                if (nonZeroInStoredFound && nonZeroInNumberFound)
                    {
                    break;
                    }
                number /= 10;
                stored /= 10;
                }

            if ( (phoneDigits.iUpperDigits == 0) || (storedUpperDigits == 0) ||
                 (number == stored) )
                {
                phoneMatchArray->AddL(stmnt.ColumnInt(KContactIdIdx));
                }
            }

        // leave if we didn't complete going through the results properly
        if (err != KSqlAtEnd)
            {
            User::Leave(err);
            }
        CleanupStack::PopAndDestroy(&stmnt);
        }

    CleanupStack::Pop(phoneMatchArray);
    return phoneMatchArray;
    }

/**
Searches the contacts database to find any contact items with an exact match on the email address supplied.

@param aEmailAddr A descriptor containing the email address to be found in the database.
@return An array of contact IDs which match the supplied email address.
*/
CContactIdArray* CPplCommAddrTable::MatchEmailAddressL(const TDesC& aEmailAddr)
	{
	return MatchNonPhoneAddrL(aEmailAddr, EEmailAddress);
	}


/**
Searches the contacts database to find any contact items with an exact match on the SIP address supplied.

@param aSipAddr A descriptor containing the SIP address to be found in the database.
@return An array of contact IDs which match the supplied SIP address.
*/
CContactIdArray* CPplCommAddrTable::MatchSipAddressL(const TDesC& aSipAddr)
	{
	return MatchNonPhoneAddrL(aSipAddr, ESipAddress);
	}


/**
Searches the contacts database to find any contact items with an exact match on the address supplied.

@param aCommAddr A descriptor containing the address to be found in the database.
@param aAddrType The type of addresses that is being sought.
@return An array of contact IDs which match the supplied address.
*/
CContactIdArray* CPplCommAddrTable::MatchNonPhoneAddrL(const TDesC& aCommAddr, TCommAddrType aAddrType)
	{

	// build statement
	RSqlStatement stmnt;
	CleanupClosePushL(stmnt);
	stmnt.PrepareL(iDatabase, iMatchSelectStmnt->SqlStringL() );

	const TInt KValueParamIndex(KFirstParam);					// first parameter in query...
	const TInt KTypeParamIndex(KValueParamIndex + 1);	// ...and the second.
	User::LeaveIfError(stmnt.BindText(KValueParamIndex, aCommAddr) );
	User::LeaveIfError(stmnt.BindInt(KTypeParamIndex, aAddrType) );

	// fetch the list of any matching contact ids
	CContactIdArray* idArray = CContactIdArray::NewLC();
	TInt err(KErrNone);
	const TInt KContactIdIdx(iMatchSelectStmnt->ParameterIndex(KCommAddrContactId() ) );
	while ((err = stmnt.Next() ) == KSqlAtRow)
		{
		idArray->AddL(stmnt.ColumnInt(KContactIdIdx) );
		}

	// leave if we didn't complete going through the results properly
	if(err != KSqlAtEnd)
		{
		User::Leave(err);
		}

	CleanupStack::Pop(idArray);
	CleanupStack::PopAndDestroy(&stmnt);

	return idArray;
	}


/**
CPplCommAddrTable constructor
*/
CPplCommAddrTable::CPplCommAddrTable(RSqlDatabase& aDatabase, CLplContactProperties& aProperties)
	: iProperties(aProperties),
	iDatabase(aDatabase)
	{
	}


/**
Destructor

Tidy up CCntSqlStatement objects
*/
CPplCommAddrTable::~CPplCommAddrTable()
	{
	delete iInsertStmnt;
	delete iWholeSelectStmnt;
	delete iMatchSelectStmnt;
	delete iUpdateStmnt;
	delete iSingleDeleteStmnt;
	delete iAllForItemDeleteStmnt;
	}

/**
Convert the supplied string to a matchable phone number.

@param aText Descriptor containing phone number.
@param aLowerMatchlength Number of least significant phone digits to use.
@param aUpperMatchLength Number of most significant phone digits to use.

@return The hash code(s) to use when matching the supplied phone number.
*/
CPplCommAddrTable::TMatch CPplCommAddrTable::CreatePaddedPhoneDigitsL(const TDesC& aNumber, const TInt aNumLowerDigits, const TInt aNumUpperDigits)
	{
	CPplCommAddrTable::TMatch phoneNumber = CreatePhoneMatchNumberL(aNumber, aNumLowerDigits, aNumUpperDigits);

	if (phoneNumber.iNumLowerDigits + phoneNumber.iUpperDigits == 0)
		{
		// No digits, do nothing
		}
	else if(phoneNumber.iNumLowerDigits < KLowerSevenDigits)
		{
		// Only the lower-digits hash is used, pad out the number to
		// KLowerSevenDigits.
		TInt pad = KLowerSevenDigits - phoneNumber.iNumLowerDigits;
		phoneNumber.iLowerSevenDigits = TMatch::PadOutPhoneMatchNumber(phoneNumber.iLowerSevenDigits,pad);
		}
	else if(phoneNumber.iNumUpperDigits < (KMaxPhoneMatchLength - KLowerSevenDigits) )
		{
		// The lower-digits hash is full, pad out the upper hash if less than 15
		// digits total.
		TInt pad = KMaxPhoneMatchLength - KLowerSevenDigits - phoneNumber.iNumUpperDigits;
		phoneNumber.iUpperDigits = TMatch::PadOutPhoneMatchNumber(phoneNumber.iUpperDigits,pad);
		}

	return phoneNumber;
	}


/**
CPplCommAddrTable::TMatch constructor.
*/
CPplCommAddrTable::TMatch::TMatch()
	:
	iLowerSevenDigits(0),
	iUpperDigits(0),
	iNumLowerDigits(0),
	iNumUpperDigits(0)
	{
	}


TBool CPplCommAddrTable::TMatch::operator==(const TMatch& aSecondMatch)const
	{
	return (iLowerSevenDigits == aSecondMatch.iLowerSevenDigits)&& (iUpperDigits == aSecondMatch.iUpperDigits);
	}


TBool CPplCommAddrTable::TMatch::Equals(const TMatch& aRMatch, const TMatch& aLMatch)
	{
	return aRMatch == aLMatch;
	}

/**
Pad out the supplied phone digits with zeroes.

@param aPhoneNumber The number to pad.
@param aPadOutLength The number of digits to pad out.

@return The padded number.
*/
TInt32 CPplCommAddrTable::TMatch::PadOutPhoneMatchNumber(TInt32& aPhoneNumber, TInt aPadOutLength)
	{
	TInt32 result(aPhoneNumber);
	const TInt KMult(10);
	for(TInt i = 0; i < aPadOutLength; ++i)
		{
		result *= KMult;
		}
	aPhoneNumber = result;
	return result;
	}


/**
Returns the hash code(s) to use when matching the supplied phone number.  If the
number supplied has more actual phone digits (i.e. not including spaces) than
KLowerSevenDigits, a second hash is generated to hold the remaining most
significant phone digits.  Extracts DTMF digits from the phone number if a
parser is available, otherwise just removes the non-digit characters.

@param aText Descriptor containing contacts phone number field.
@param aLowerMatchlength Number of least significant phone digits to use.
@param aUpperMatchLength Number of most significant phone digits to use.

@return The hash code(s) to use when matching the supplied phone number.
*/
CPplCommAddrTable::TMatch CPplCommAddrTable::CreatePhoneMatchNumberL(const TDesC& aText, TInt aLowerMatchLength, TInt aUpperMatchLength)
	{
	__ASSERT_DEBUG( ((aLowerMatchLength == KLowerSevenDigits) && (aUpperMatchLength > 0) )		// upper 8 digits
					|| ((aLowerMatchLength <= KLowerSevenDigits) && (aUpperMatchLength == 0) ),	// lower 7 digits
					User::Leave(KErrNotSupported) );

	const TInt KBufLength = KCntMaxTextFieldLength+1;
	TBuf<KBufLength> buf;

	CContactPhoneNumberParser* parser = iProperties.ContactPhoneParserL().Parser();

	if (parser)
		{
		parser->ExtractRawNumber(aText.Left(KCntMaxTextFieldLength),buf);
		}
	else
		{
		if(aText.Length() <= KBufLength)
			{
			buf = aText;
			}
		else
			{
			buf = aText.Right(KBufLength);
			}
		TMatch::StripOutNonDigitChars(buf);
		}

	TMatch phoneNumber;

	if (buf.Length() == 0)
		{
		return phoneNumber;
		}

	// Generate a hash for the upper digits only if the phone number string is
	// large enough and more than 7 digits are to be matched.
	TInt phoneNumberlength = buf.Length();
	if ( (phoneNumberlength > KLowerSevenDigits) && (aUpperMatchLength > 0) )
		{
		TPtrC upperPart = buf.Left(phoneNumberlength - KLowerSevenDigits);
		phoneNumber.iUpperDigits = TMatch::CreateHashL(upperPart,
			aUpperMatchLength, phoneNumber.iNumUpperDigits);
		}

	// Generate a hash of the lower digits.
	phoneNumber.iLowerSevenDigits = TMatch::CreateHashL(buf, aLowerMatchLength, phoneNumber.iNumLowerDigits);

	return phoneNumber;
	}


/**
Strip out any non-digit characters before we convert the phone number to an
integer.

@param aText Phone number which on return will have any non-digit characters
removed.
*/
void CPplCommAddrTable::TMatch::StripOutNonDigitChars(TDes& aText)
	{
	for(TInt chrPos = 0; chrPos < aText.Length(); ++chrPos)
		{
		TChar chr = aText[chrPos];
		if (!chr.IsDigit() )
			{
			aText.Delete(chrPos, 1);
			--chrPos;
			}
		}
	}


/**
Generates a hash value by reversing the aMatchLength least significant digits,
ignoring non-digits and zeroes at the end of the number.  Asserts if no phone
digits are supplied.

@param aPhoneNumberString A descriptor containing a phone number.
@param aMatchLength The number of digits from the right of the phone number to use.
@param aNumPhoneDigits The number of digits found in the phone number string.

@return An integer representation of the phone number string in reverse.
*/
TInt32 CPplCommAddrTable::TMatch::CreateHashL(const TDesC& aPhoneNumberString, TInt aMatchLength, TInt& aNumPhoneDigits)
	{
	TInt phoneNumberLength = aPhoneNumberString.Length();
	__ASSERT_DEBUG(phoneNumberLength > 0, User::Leave(KErrNotSupported) );

	TInt startIndex = 0;

	if (phoneNumberLength > aMatchLength)
		{
		startIndex = phoneNumberLength - aMatchLength;
		}

	aNumPhoneDigits = 0;
	TUint reversedDigits = 0;
	TInt mult = 1;

	for (TInt chrIndex = startIndex; (aNumPhoneDigits < aMatchLength) && (chrIndex < phoneNumberLength); chrIndex++)
		{
		TChar chr = aPhoneNumberString[chrIndex];
		if (chr.IsDigit() )
			{
			reversedDigits += (chr.GetNumericValue() ) * mult;
			mult = mult * 10;
			++aNumPhoneDigits;
			}
		}

	return reversedDigits ;
	}
