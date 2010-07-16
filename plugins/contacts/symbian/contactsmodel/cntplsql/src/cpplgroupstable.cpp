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


const TInt KArrayGranularity = 4;

/**
@param aDatabase A handle to the database.

@return A pointer to a new CPplGroupsTable object.
*/
CPplGroupsTable* CPplGroupsTable::NewL(RSqlDatabase& aDatabase)
	{
	CPplGroupsTable* self = CPplGroupsTable::NewLC(aDatabase);
	CleanupStack::Pop(self);
	return self;
	}

/**
@param aDatabase A handle to the database.

@return A pointer to a new CPplGroupsTable object.
*/
CPplGroupsTable* CPplGroupsTable::NewLC(RSqlDatabase& aDatabase)
	{
	CPplGroupsTable* self = new (ELeave) CPplGroupsTable(aDatabase);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

/**
Set up the CCntSqlStatement objects held by the class.
*/
void CPplGroupsTable::ConstructL()
	{
	// Statement types
	TCntSqlStatementType insertType(EInsert, KSqlContactGroupTableName);
	TCntSqlStatementType selectType(ESelect, KSqlContactGroupTableName);
	TCntSqlStatementType updateType(EUpdate, KSqlContactGroupTableName);
	TCntSqlStatementType deleteType(EDelete, KSqlContactGroupTableName);
	TCntSqlStatementType countContactsType(ESelect, KSqlContactTableName);
	
	// Where clauses

	// sizes of the clauses
	const TInt KWhereGroupClauseBufSize(KGroupContactGroupId().Size() + 
		KWhereStringEqualsStringFormatText().Size() + KGroupContactGroupIdParam().Size() );
	const TInt KWhereMemberClauseBufSize(KGroupContactGroupMemberId().Size() + 
		KWhereStringEqualsStringFormatText().Size() + KGroupContactGroupMemberIdParam().Size() );
	const TInt KWhereOrClauseBufSize(KWhereGroupClauseBufSize + KSqlOr().Size() + KWhereMemberClauseBufSize);
	
	// for WHERE contact_group_id = [contact id value]
	HBufC* whereGroupIdClause = HBufC::NewLC(KWhereGroupClauseBufSize);
	whereGroupIdClause->Des().AppendFormat(KWhereStringEqualsStringFormatText, 
		&KGroupContactGroupId, &KGroupContactGroupIdParam );

	// for WHERE contact_group_member_id = [contact id value]
	HBufC* whereMemberIdClause = HBufC::NewLC(KWhereMemberClauseBufSize);
	whereMemberIdClause->Des().AppendFormat(KWhereStringEqualsStringFormatText, 
		&KGroupContactGroupMemberId, &KGroupContactGroupMemberIdParam );

	// for WHERE contact_group_id = [contact id value] 
	//	OR contact_group_member_id = [contact id value]
	HBufC* whereGroupOrMemberIdClause = HBufC::NewLC(KWhereOrClauseBufSize);
	TPtr whereGroupOrMemberIdClausePtr = whereGroupOrMemberIdClause->Des();
	whereGroupOrMemberIdClausePtr.AppendFormat(KWhereStringEqualsStringFormatText, 
		&KGroupContactGroupId, &KGroupContactGroupIdParam);
	whereGroupOrMemberIdClausePtr.Append(KSqlOr);
	whereGroupOrMemberIdClausePtr.AppendFormat(KWhereStringEqualsStringFormatText, 
		&KGroupContactGroupMemberId, &KGroupContactGroupMemberIdParam);

	// for WHERE contact_id = [contact_id]
	HBufC* whereContactIdClause = HBufC::NewLC(KWhereGroupClauseBufSize);
	whereContactIdClause->Des().AppendFormat(KWhereStringEqualsStringFormatText, 
		&KContactId, &KContactIdParam );
		
	// INSERT

	// insert group-member relationships
	// For a statement in the following format:
	// 	INSERT INTO groups 
	//		(group_id, contact_group_id, contact_group_member_id) 
	//		VALUES (NULL, [contact group id value], [contact group member id value]);
	//
	iInsertStmnt = TSqlProvider::GetSqlStatementL(insertType);
	iInsertStmnt->SetParamL(KGroupContactGroupId(), KGroupContactGroupIdParam());
	iInsertStmnt->SetParamL(KGroupContactGroupMemberId(), KGroupContactGroupMemberIdParam());

	// SELECT
	
	// select group id
	// For a statement in the following format:
	// 	SELECT contact_group_id FROM groups 
	//		WHERE contact_group_member_id = [contact id value];
	//
	iSelectGroupsStmnt = TSqlProvider::GetSqlStatementL(selectType);
	iSelectGroupsStmnt->SetParamL(KGroupContactGroupId(), KNullDesC() );
	iSelectGroupsStmnt->SetConditionL(*whereMemberIdClause);

	// select member id
	// For a statement in the following format:
	// 	SELECT contact_group_member_id FROM groups 
	//		WHERE contact_group_id = [contact id value];
	//
	iSelectMembersStmnt = TSqlProvider::GetSqlStatementL(selectType);
	iSelectMembersStmnt->SetParamL(KGroupContactGroupMemberId(), KNullDesC() );
	iSelectMembersStmnt->SetConditionL(*whereGroupIdClause);

	// DELETE

	// delete all where group or member equals id
	// For a statement in the following format:
	// 	DELETE FROM groups WHERE contact_group_id = [contact id value]
	//		OR contact_group_member_id = [contact id value];
	//
	iDeleteStmnt = TSqlProvider::GetSqlStatementL(deleteType);
	iDeleteStmnt->SetConditionL(*whereGroupOrMemberIdClause);

	// SELECT
	
	// SELECt count(*) FROM contact WHERE contact_id = [contact_id]
	iCountContactsStmnt = TSqlProvider::GetSqlStatementL(countContactsType);
	iCountContactsStmnt->SetParamL(KSqlCount, KSpace);
	iCountContactsStmnt->SetConditionL(*whereContactIdClause);
	
	CleanupStack::PopAndDestroy(4, whereGroupIdClause); // and whereContactIdClause, whereMemberIdClause, whereGroupOrMemberIdClause
	}


/**
Destructor

Tidy up CCntSqlStatement objects
*/
CPplGroupsTable::~CPplGroupsTable()
	{
	delete iInsertStmnt;
	delete iSelectGroupsStmnt;
	delete iSelectMembersStmnt;
	delete iDeleteStmnt;
	delete iCountContactsStmnt;
	}


/**
Does nothing. An empty implementation to override the pure virtual method in the base class.
*/
void CPplGroupsTable::CreateInDbL(CContactItem& /*aItem*/)
	{
	// Do nothing.
	
	// Inserting new records in the groups table is done through the UpdateL() method.
	}


/**
ReadL has a dual functionality. If passed parameter is a group, will be filled to contacts
bellonging to that group. Otherwirse, the contact item will be filed with all groups to which
it belongs

@param aItem Reference to contact item.
*/
void CPplGroupsTable::ReadL(CContactItem& aItem)
	{
	const TContactItemId KItemId(aItem.Id() );
	const TUid KType(aItem.Type() );
	if (KType == KUidContactGroup)
		{
		CContactGroup& group = static_cast<CContactGroup&>(aItem);
		group.ResetItems();
		group.SetItems(GetListForItemL(KItemId, ETrue));
		}	


	if (KType == KUidContactCard || KType == KUidContactOwnCard || KType == KUidContactICCEntry || KType == KUidContactGroup)
		{
		CContactItemPlusGroup& item = static_cast<CContactItemPlusGroup&>(aItem);
		item.ResetGroups();
		item.SetGroups(GetListForItemL(KItemId, EFalse));
		}	
	}


/**
Updates informations related to passed contact item within group table

@param aItem Reference to contact item.
*/
void CPplGroupsTable::UpdateL(const CContactItem& aItem)
	{
	// Only write to the table if it's a group. If we add the relationship from both sides 
	// (i.e. once for the group and once for the item) we will have duplicate records.	
	if (aItem.Type() == KUidContactGroup)
		{
		WriteGroupMembersL(aItem);
		}	
	}


/**
Deletes group informations related to passed contact item from group table

@param aItem Reference to contact item.
@param aLowDiskErrorOccurred out parameter; will be set to ETrue if there was an attempt to delete
		in low disk condition
*/
void CPplGroupsTable::DeleteL(const CContactItem& aItem, TBool& aLowDiskErrorOccurred)
	{
	DeleteItemL(aItem.Id(), aLowDiskErrorOccurred);
	}


/**
Creates the groups table and its indexes in the database.
*/
void CPplGroupsTable::CreateTableL()
	{
	User::LeaveIfError(iDatabase.Exec(KGroupsCreateStmnt() ) );
	}


/**
CPplGroupsTable constructor 

@param aDatabase reference to contact database
*/
CPplGroupsTable::CPplGroupsTable(RSqlDatabase& aDatabase) :
	iDatabase(aDatabase)
	{
	}


/**
GetListForItemL has a dual nature. If aIsGroup is ETrue, a list of contact items belonging to
specified group is returned. Otherwise a list of group ids to which contact id belongs is returned.

@param aItemId contact item id
@param aIsGroup ETrue if the method will fill a group.
*/
CContactIdArray* CPplGroupsTable::GetListForItemL(TContactItemId aItemId, TBool aIsGroup)
	{
	/*
	// Check if group membership information was not requested or if the item
	// is not derived from CContactItemPlusGroup.
	if (!(aType == KUidContactGroup	  || aType == KUidContactCard ||
		  aType == KUidContactOwnCard || aType == KUidContactICCEntry) )
		{
		return NULL; 
		}
	*/	

	// build the RSqlStatement
	RSqlStatement stmnt;
	CleanupClosePushL(stmnt);
	TInt idIndex;

	// build the CCntSqlStatement statement
	const TInt KWhereParamIndex(KFirstIndex); // only one parameter in the query
	if (aIsGroup)
		{
		// group -> select members
		stmnt.PrepareL(iDatabase, iSelectMembersStmnt->SqlStringL() );
		User::LeaveIfError(stmnt.BindInt(KWhereParamIndex, aItemId ) );
		idIndex = stmnt.ColumnIndex(KGroupContactGroupMemberId() );
		}
	else
		{
		// member -> select groups
		stmnt.PrepareL(iDatabase, iSelectGroupsStmnt->SqlStringL() );
		User::LeaveIfError(stmnt.BindInt(KWhereParamIndex, aItemId ) );
		idIndex = stmnt.ColumnIndex(KGroupContactGroupId() );
		}
	User::LeaveIfError(idIndex);
	// fetch the list of any matching ids
	CContactIdArray* items = CContactIdArray::NewLC();
	TInt err(KErrNone);
	while ((err = stmnt.Next() ) == KSqlAtRow)
		{
		items->AddL(stmnt.ColumnInt(idIndex) );
		}

	// leave if we didn't complete going through the results properly
	if(err != KSqlAtEnd)
		{
		User::Leave(err);
		}
	
	CleanupStack::Pop(items);
	CleanupStack::PopAndDestroy(&stmnt);
	return items;
	}


/**
Persist the items belonging to curent group into group table

@param aGroup referece to a contact group
*/
void CPplGroupsTable::WriteGroupMembersL(const CContactItem& aGroup)
	{
	if (aGroup.Type() != KUidContactGroup)
		{
		return;
		}

	const TContactItemId KGroupId(aGroup.Id() );

	// make sure we clear out any previous, out-of-date data
	TBool lowDiskErr(EFalse);
	DeleteItemL(KGroupId, lowDiskErr);
	if (lowDiskErr)
		{
		User::Leave(KErrDiskFull);
		}

	// build the RSqlStatement
	RSqlStatement stmnt;
	CleanupClosePushL(stmnt);
	stmnt.PrepareL(iDatabase, iInsertStmnt->SqlStringL() );
	const TInt KGroupIdIndex(KFirstIndex); 			// first parameter in query...	
	const TInt KMemberIdIndex(KGroupIdIndex + 1); 	// ...and the second parameter
	
	// copy and sort the member id array so we can see if there are duplicates
	const CContactIdArray* contactIdArray = static_cast<const CContactGroup&>(aGroup).ItemsContained();  //does not take the ownership
	
	const TInt arrayCount = contactIdArray->Count();
	CArrayFixFlat<TContactItemId>* sortedList = new(ELeave) CArrayFixFlat<TContactItemId>(KArrayGranularity);	
	CleanupStack::PushL(sortedList);
    for(TInt loop = 0;loop < arrayCount; ++loop)
    	{
    	sortedList->AppendL((*contactIdArray)[loop]);		
    	}
	TKeyArrayFix key(0,ECmpTInt);
	sortedList->Sort(key);

	// insert the group-member relationships
	const TInt KCountStmntParamIndex(KFirstIndex); // first and only parameter in query
	const TInt listLen(sortedList->Count() );
	TInt lastId(0);
	for (TInt i = 0; i < listLen; ++i)
		{
		TInt itemId((*sortedList)[i]);
		
		//check if a contact item with itemId id really exists in contact database
		RSqlStatement countStmnt;
		CleanupClosePushL(countStmnt);
		countStmnt.PrepareL(iDatabase, iCountContactsStmnt->SqlStringL() );
		User::LeaveIfError(countStmnt.BindInt(KCountStmntParamIndex, itemId) );
		TInt count = 0;
		TInt err = KErrNone;
		if((err = countStmnt.Next() ) == KSqlAtRow)
			{
			count = countStmnt.ColumnInt(iCountContactsStmnt->ParameterIndex(KSqlCount) );
			}
		else
			{
			User::LeaveIfError(err);	
			}	

		if(count == 0) 
			{
			User::Leave(KErrNotFound);	
			}
		CleanupStack::PopAndDestroy(&countStmnt);
			
		// only insert this if we haven't already seen it
		if (itemId != lastId || i == 0)
			{
			User::LeaveIfError(stmnt.BindInt(KGroupIdIndex, KGroupId) );
			User::LeaveIfError(stmnt.BindInt(KMemberIdIndex, itemId) );
			User::LeaveIfError(stmnt.Exec() );
			User::LeaveIfError(stmnt.Reset() );
			}
		lastId = itemId;
		}

	CleanupStack::PopAndDestroy(2, &stmnt); // and sortedList
	}


/**
Deletes information about group for the passed contact item id

@param aItemId contact item id
@param aLowDiskErrorOccurred out parameter; will be set to ETrue if there was a deletion in 
		low disk condition
*/
void CPplGroupsTable::DeleteItemL(TContactItemId aItemId, TBool& aLowDiskErrorOccurred)
	{
	RSqlStatement stmnt;
	CleanupClosePushL(stmnt);
	stmnt.PrepareL(iDatabase, iDeleteStmnt->SqlStringL() );

	const TInt KGroupIdIndex(KFirstIndex); 			// first parameter in query...	
	const TInt KMemberIdIndex(KGroupIdIndex + 1); 	// ...and the second parameter
	User::LeaveIfError(stmnt.BindInt(KGroupIdIndex, aItemId) );
	User::LeaveIfError(stmnt.BindInt(KMemberIdIndex, aItemId) );
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
