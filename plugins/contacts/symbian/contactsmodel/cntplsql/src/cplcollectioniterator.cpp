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


/**
 @file
 @internalComponent
 @released
*/

#include "persistencelayerimpl.h"
#include "clplcontactproperties.h"
#include <cntfilt.h>
#include "cntsqlprovider.h"
#include "dbsqlconstants.h"
#include "cntpersistenceutility.h"
#include "cntstd.h"
#include <sqldb.h>

//
// The TFindFieldFlags enumeration is duplicated in cviewiterator.h.  It should
// be refactored into a shared header if possible.
//
enum TFindFieldFlags
	{
	EFindInAllFields				=0x00000001,
	EFindFirstName					=0x00000002,
	EFindLastName					=0x00000004,
	EFindCompanyName				=0x00000008,
	EFindInEmailField				=0x00000010,
	EFindInAnyIdentityField			=0x00000020,
	EFindFirstNamePronunciation		=0x00000040,
	EFindLastNamePronunciation		=0x00000080,
	EFindCompanyNamePronunciation	=0x00000100,
	EFindInSIPField					=0x00000200
	};


// Maximum number of find iterations to perform in the CPlCollection::FindL()
// method before returning.
const TInt KFindIterations = 16;

// These flags must be in the same order as the corresponding fields in
// RPplIdentityTable::TIdentityField.
const TFindFieldFlags KNameFlags[] = 
	{
	EFindFirstName,
	EFindLastName,
	EFindCompanyName,
	EFindFirstNamePronunciation,
	EFindLastNamePronunciation,
	EFindCompanyNamePronunciation
	};

// Collation level that ignore accents (i.e. 'a' == 'ä').
const TInt KCollationLevel= 0;

//
// The TKeyCmpTextLength class is duplicated in ccontactprivate.cpp.  It should
// be refactored into shared code/library if possible.
//
NONSHARABLE_CLASS(TKeyCmpTextLength) : public TKeyArrayFix
	{
public:
	TKeyCmpTextLength();
protected:
	// From TKey.
	virtual TInt Compare(TInt aLeft,TInt aRight) const;
	};


TKeyCmpTextLength::TKeyCmpTextLength()
	// Doesn't matter using ECmpNormal, it's ignored.
	:
	TKeyArrayFix(0,ECmpNormal,0)
	{
	}


/**
Compare the lengths of the words.
*/
TInt TKeyCmpTextLength::Compare(TInt aLeft,TInt aRight) const
	{
	TDesC* left=(*((TDesC**)At(aLeft)));
	TDesC* right=(*((TDesC**)At(aRight)));
	return right->Length()-left->Length();
	}

	
/**
Used as a cleanup item for buffer allocation in CreateFindTextL().

@param aHBufC The buffer to be cleaned up (deleted).
*/
GLDEF_C void DestroyHBufC(TAny* aHBufC)
	{
	delete *STATIC_CAST(HBufC**, aHBufC);
	}


/**
Create the "find text", used for the SQL query on the view, from the given text.

@param aText Text to turn into "find text".

@return "Find text" form of the given text.
*/
HBufC* CreateFindTextL(const TDesC& aText)
	{
	_LIT(KApostrophe, "'");
	TInt location = aText.Find(KApostrophe);

	HBufC* buf=HBufC::NewL(aText.Length()+2);
	TPtr ptr=buf->Des();
	ptr.Zero();
	ptr.Append('%');
	ptr += aText;
	ptr.Append('%');
	
	// Insert additional apostrophes if required.  This is so the SQL search
	// string behaves correctly.
	if(location != KErrNotFound)
		{
		CleanupStack::PushL(buf);

		TPtrC rightBuf = *buf;		
		HBufC* leftBuf=HBufC::NewL(0);
		CleanupStack::PushL(TCleanupItem(DestroyHBufC, &leftBuf));

		// Increment location because we have added the * character.
		location++;
		
		while (location != KErrNotFound)
			{
			// Copy left hand string up to the first apostrophe into a new
			// buffer and append an apostrophe.
			leftBuf = leftBuf->ReAllocL(leftBuf->Length()+location+2); 
			TPtr ptrLeftBuf = leftBuf->Des();
			ptrLeftBuf.Append(rightBuf.Left(location+1));
			ptrLeftBuf += (KApostrophe);

			// Make a copy of the right hand part.
			TInt rightLength = rightBuf.Length()-location-1;
			rightBuf.Set(rightBuf.Right(rightLength));
			
			// Search for another apostrophe.
			location = rightBuf.Find(KApostrophe);
			}
		
		// Now append the rest of the right hand part.
		leftBuf = leftBuf->ReAllocL(leftBuf->Length()+rightBuf.Length()); 
		TPtr ptrLeftBuf = leftBuf->Des();
		ptrLeftBuf.Append(rightBuf);

		CleanupStack::Pop(&leftBuf);	
		CleanupStack::PopAndDestroy(buf);
		
		return (leftBuf);
		}	
	
	return (buf);	
	}


/**
For the given field type UID, increase aIdentityColumnsCount if the field maps
to one of the columns in the Identity table and add the relevant find flag.

@param aUid Field type UID.
@param aFindFlags Updated with find flag which maps to the given field type UID.
@param aIdentityColumnsCount Incremented if the field type UID maps to a column
in the Identity table.
*/
//
// This method is duplicated in cviewiterator.cpp.  It should be refactored into
// shared code/library if possible.
//
void SetFindFlagsAndColumnsCount(TInt32 aUid,TInt& aFindFlags,TInt& aIdentityColumnsCount)
	{
	switch(aUid)
		{
		case KUidContactFieldGivenNameValue:
			{
			++aIdentityColumnsCount;
			aFindFlags |= EFindFirstName;
			break;
			}
		case KUidContactFieldFamilyNameValue:
			{
			++aIdentityColumnsCount;
			aFindFlags |= EFindLastName;
			break;
			}
		case KUidContactFieldCompanyNameValue:
			{
			++aIdentityColumnsCount;
			aFindFlags |= EFindCompanyName;
			break;
			}
		case KUidContactFieldGivenNamePronunciationValue:
			{
			++aIdentityColumnsCount;
			aFindFlags |= EFindFirstNamePronunciation;
			break;
			}
		case KUidContactFieldFamilyNamePronunciationValue:
			{
			++aIdentityColumnsCount;
			aFindFlags |= EFindLastNamePronunciation;
			break;
			}
		case KUidContactFieldCompanyNamePronunciationValue:
			{
			++aIdentityColumnsCount;
			aFindFlags |= EFindCompanyNamePronunciation;
			break;
			}
		case KUidContactFieldEMailValue:
			{
			aFindFlags |= EFindInEmailField;
			break;
			}
		case KUidContactFieldSIPIDValue:
			{
			aFindFlags |= EFindInSIPField;
			break;			
			}
		case KUidContactFieldMatchAllValue:
			{
			//added to address issues raised in INC049017
			//needed as a seperate case instead of modifying the default value because Identitys should not always be searched
			//if a valid KUid is used. e.g.: KUidContactFieldAddress. If the default value was modified and someone wanted to
			//perform an address search using Elizabeth (as in elizabeth road) contacts of that name would also be found.
			aFindFlags |= EFindInAllFields|EFindInAnyIdentityField;
			break;
			}
		default:
			aFindFlags |= EFindInAllFields;
		}
	}


/**
Determine if the Identity table requires to be searched for the given find
flags.

@param aFindFlags Set of find flags describing which fields will be searched.

@return ETrue If one of the flags in aFindFlags maps to a column in the Identity
table, EFalse otherwise.
*/
//
// This method is duplicated in cviewiterator.cpp.  It should be refactored into
// shared code/library if possible.
//
TBool SearchIdentityFieldsRequired(TInt aFindFlags)
	{
	return aFindFlags &
		( EFindInAnyIdentityField | EFindFirstName | EFindLastName | EFindCompanyName |
			EFindFirstNamePronunciation | EFindLastNamePronunciation | EFindCompanyNamePronunciation);
	}


/**
Determine if only the Identity table requires to be searched for the given find
flags.

@param aFindFlags Set of find flags describing which fields will be searched.

@return ETrue If one of the flags in aFindFlags maps to a column in the Identity
table and no columns in any other table, EFalse otherwise.
*/
//
// This method is duplicated in cviewiterator.cpp.  It should be refactored into
// shared code/library if possible.
//
TBool CPlCollection::UsesIdentityFieldsOnly(TInt aFindFlags)
	{
	return (aFindFlags & (EFindFirstName | EFindLastName | EFindCompanyName |
			EFindFirstNamePronunciation |EFindLastNamePronunciation |EFindCompanyNamePronunciation) ) &&
		! (aFindFlags & (EFindInAllFields | EFindInEmailField | EFindInSIPField) );
	}


/**
Update a set of find flags and an Identity table column count from the field
type UIDs in the given field definition.

@param aFindFlags Updated with those flags which map to the field type UIDs in
aFieldDef.
@param aIdentityColumnsCount Updated with the number of field type UIDs in
aFieldDef which map to columns in the Identity table.
@param aFieldDef Field definition from which to create set of find flags and
Identity table column count.
*/
//
// This method is duplicated in cviewiterator.cpp.  It should be refactored into
// shared code/library if possible.
//
void ConstructBitwiseFindFlags(TInt& aFindFlags,TInt& aIdentityColumnsCount,const CContactItemFieldDef* aFieldDef)
	{
	if(aFieldDef!=NULL && aFieldDef->Count()>0)
		{
		for(TInt ii = 0;ii < aFieldDef->Count();ii++)
			{
			SetFindFlagsAndColumnsCount(aFieldDef->At(ii).iUid,aFindFlags,aIdentityColumnsCount);
			}
		}
	else
		{
		aFindFlags |= EFindInAllFields|EFindInAnyIdentityField;
		}
	}


/**
Update a set of find flags and an Identity table column count from the field
type UIDs in the given text definition.  This can be used to tell the find
method what tables need to be searched for a given text definition.  If the text
definition is NULL we search in all tables.

@param aFindFlags Updated with those flags which map to the field type UIDs in
aTextDef.
@param aIdentityColumnsCount Updated with the number of field type UIDs in
aTextDef which map to columns in the Identity table.
@param aTextDef Text definition from which to create set of find flags and
Identity table column count.
*/
//
// This method is duplicated in cviewiterator.cpp.  It should be refactored into
// shared code/library if possible.
//
void CPlCollection::ConstructBitwiseFlagsFromTextDef(TInt& aFindFlags,TInt& aIdentityColumnsCount,const CContactTextDef* aTextDef)
	{
	if(aTextDef != NULL && aTextDef->Count()>0)
		{
		for(TInt ii = 0;ii < aTextDef->Count();ii++)
			{
			SetFindFlagsAndColumnsCount(aTextDef->At(ii).iFieldType.iUid,aFindFlags,aIdentityColumnsCount);
			}
		TFieldType fallback = aTextDef->FallbackField();
		if (fallback != KUidContactFieldNone)
		SetFindFlagsAndColumnsCount(fallback.iUid,aFindFlags,aIdentityColumnsCount);
		}
	else
		{
		aFindFlags |= EFindInAllFields|EFindInAnyIdentityField;
		}
	}


/**
Utility method used to search a contact id in contact database.

@param aReqId requested contact id to look for
@param aId out parameter; will be filled with the actual found contact item id 
@param aContactType specifies contact type to be search
@param aDeleted set to ETrue if a search through deleted contacts is required

@return ETrue if a contact item with contact id greater or equal with aReqId exists in contact database
		EFalse otherwise
*/
TBool CPlCollection::SeekContactL(TContactItemId aReqId,TContactItemId& aId,TUid& aContactType, TBool& aDeleted)
	{
	TBool ret = ETrue;

	HBufC* selectString = HBufC::NewLC(KSelectTwoFieldsWithGreater().Length() + 
									KContactId().Length() + 
									KContactTypeFlags().Length() + 
									KSqlContactTableName().Length() + 					
									KContactId().Length() + 3); 
	TPtr ptrSelectString = selectString->Des();
	ptrSelectString.Format(KSelectTwoFieldsWithGreater, &KContactId, &KContactTypeFlags, &KSqlContactTableName, &KContactId, aReqId);

	RSqlStatement selectStatement;
	CleanupClosePushL(selectStatement);
		
	User::LeaveIfError(selectStatement.Prepare(iContactsFile.NamedDatabase(), ptrSelectString));

	const TInt idIndex = selectStatement.ColumnIndex(KContactId);
	User::LeaveIfError(idIndex);
	const TInt typeIndex = selectStatement.ColumnIndex(KContactTypeFlags);
	User::LeaveIfError(typeIndex);
	TInt err;
	if((err = selectStatement.Next()) == KSqlAtRow) 
		{
		aId = selectStatement.ColumnInt(idIndex);
		TInt typeFlags = selectStatement.ColumnInt(typeIndex);
		aContactType = TCntPersistenceUtility::TypeFlagsToContactTypeUid(typeFlags); 
		TInt attr = (typeFlags & EContactAttributes_Mask) >> EContactAttributes_Shift; 
		aDeleted = (attr == EContactAttrsFlags_Deleted);
		}
	else 
		{
		User::LeaveIfError(err);
		ret = EFalse;	
		}
	
	CleanupStack::PopAndDestroy(2, selectString); //selectStatement, selectString	
	return ret;
	}
	
/**
CPlCollection constructor.

@param aContactsFile Contacts file object from the Persistence Layer.
*/
CPlCollection::CPlCollection(CPplContactsFile& aContactsFile)
	: 
	iContactsFile(aContactsFile)
	{
	}
	
/**
CPlCollection ConstructL
*/	
void CPlCollection::ConstructL()
	{
	TCntSqlStatementType statementType(ESelect, KSqlContactTableName);
	iSelectStatement = TSqlProvider::GetSqlStatementL(statementType);
	}
	
/**
CPlCollection NewL
*/		
CPlCollection* CPlCollection::NewL(CPplContactsFile& aContactsFile)
	{
	CPlCollection* self = new (ELeave) CPlCollection(aContactsFile);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	

/**
CPlCollection destructor.
*/
CPlCollection::~CPlCollection()
	{
	Reset();
	delete iSelectStatement;
	iSelectStatement = NULL;
	}


/**
Get the count of contacts in the Contacts table.

@return Count of contacts in the Contacts table.
*/
TInt CPlCollection::ContactCountL()
	{
	TInt count = 0;

	//count contact cards
	HBufC* selectString = HBufC::NewLC(KCountTypeSelect().Length() + 
						KSqlContactTableName().Length() + KContactTypeFlags().Length() + KContactTypeFlags().Length() + 6); 
	TPtr ptrSelectString = selectString->Des();
	ptrSelectString.Format(KCountTypeSelect, &KSqlContactTableName, &KContactTypeFlags, EContactType_Shift, EContactTypeFlags_ContactCard,
							&KContactTypeFlags, EContactAttributes_Shift, EContactAttrsFlags_Deleted);

	TSqlScalarFullSelectQuery scalarQuery(iContactsFile.NamedDatabase());
	
	count = scalarQuery.SelectIntL(ptrSelectString);
	CleanupStack::PopAndDestroy(selectString);

	//count own card
	selectString = HBufC::NewLC(KCountTypeSelect().Length() + 
				KSqlContactTableName().Length() + KContactTypeFlags().Length() + KContactTypeFlags().Length() + 6); 
	ptrSelectString = selectString->Des();
	ptrSelectString.Format(KCountTypeSelect, &KSqlContactTableName, &KContactTypeFlags, EContactType_Shift, EContactTypeFlags_OwnCard,
							&KContactTypeFlags, EContactAttributes_Shift, EContactAttrsFlags_Deleted);
	
	count += scalarQuery.SelectIntL(ptrSelectString);
	CleanupStack::PopAndDestroy(selectString);
	
	//count groups
	selectString = HBufC::NewLC(KCountTypeSelect().Length() + 
				KSqlContactTableName().Length() + KContactTypeFlags().Length() + KContactTypeFlags().Length() + 6); 
	ptrSelectString = selectString->Des();
	ptrSelectString.Format(KCountTypeSelect, &KSqlContactTableName, &KContactTypeFlags, EContactType_Shift, EContactTypeFlags_Group,
							&KContactTypeFlags, EContactAttributes_Shift, EContactAttrsFlags_Deleted);
	
	count += scalarQuery.SelectIntL(ptrSelectString);
	CleanupStack::PopAndDestroy(selectString);
	
	//card template
	selectString = HBufC::NewLC(KCountTypeSelect().Length() + 
				KSqlContactTableName().Length() + KContactTypeFlags().Length() + KContactTypeFlags().Length() + 6); 
	ptrSelectString = selectString->Des();
	ptrSelectString.Format(KCountTypeSelect, &KSqlContactTableName, &KContactTypeFlags, EContactType_Shift, EContactTypeFlags_CardTemplate,
							&KContactTypeFlags, EContactAttributes_Shift, EContactAttrsFlags_Deleted);
	
	count += scalarQuery.SelectIntL(ptrSelectString);
	CleanupStack::PopAndDestroy(selectString);
			
	return count;
	}


/**
Get a collection of contact IDs using the specified view/collection parameters.

@param aViewType Type of view/collection.
@param aTime Used if view/collection type is "changed since".
@param aGuid Used if view/collection type is "find GUID".

@return Contact ID array of those contacts which match the specified collection
parameters.
*/
CContactIdArray* CPlCollection::CollectionL(TLplViewType aViewType,TTime aTime,const TDesC& aGuid)
	{
	CContactIdArray* idArray=NULL;
	switch(aViewType)
		{
		case EChangedSince :
			{
			idArray = ChangedSinceL(aTime);
			}
			break;
		case EDeleted :
			{
			idArray = DeletedL();
			}
			break;
		case EUnfiled :
			{
			idArray = UnfiledL();
			}
			break;
		case EFindGuid :
			{
			idArray = GuidL(aGuid);
			}
			break;
		default:
			{
			User::Leave(KErrNotFound);
			}
			break;
		}
	return idArray;
	}

	
/**
Utility method used to serach hint field for provided contact id

@param aBitWiseFileter filter used for hint matching
@param aContactId contact item id

@return ETrue if a hint is found 
		EFalse otherwise
*/	
TBool CPlCollection::ContactMatchesHintFieldL(TInt aBitWiseFilter, TContactItemId aContactId)
	{
	TBool hintMatch = EFalse;
	
	HBufC* searchCond = HBufC::NewLC(KConditionClause().Length() + KContactId().Length() + 32);
	searchCond->Des().Format(KConditionClause, &KContactId, aContactId);

	RSqlStatement selectStatement;
	CleanupClosePushL(selectStatement);
		
	iSelectStatement->Reset();	
	iSelectStatement->SetParamL(KContactId, KSpace);
	iSelectStatement->SetParamL(KContactTypeFlagsParam, KSpace);
	iSelectStatement->SetConditionL(*searchCond);
	
	User::LeaveIfError(selectStatement.Prepare(iContactsFile.NamedDatabase(), iSelectStatement->SqlStringL()));

	TInt err;
	if((err = selectStatement.Next()) == KSqlAtRow)
		{
		const TInt typeFlagsIndex = iSelectStatement->ParameterIndex(KContactTypeFlags);
		TInt typeFlags = selectStatement.ColumnInt(typeFlagsIndex);
		TUid contactType = TCntPersistenceUtility::TypeFlagsToContactTypeUid(typeFlags);
		if (iContactsFile.ContactProperties().CheckType(contactType))
			{
			TInt hint = typeFlags & EContactHintFlags_Mask;
			hintMatch = HintFieldMatchesFilter(hint, aBitWiseFilter);	
			}
		}
	else
		{
		User::LeaveIfError(err);
		}	
	
	CleanupStack::PopAndDestroy(2, searchCond); //searchCond, selectStatement
	return hintMatch;
	}

/**
Utility method used for phone matching

@param aNumber phone number to be matched
@param aMatchLengthFromRight specifies the number of relevant characters to be matched
@return array with contact ids matching passed phone number
*/
CContactIdArray* CPlCollection::MatchPhoneNumberL(const TDesC& aNumber, const TInt aMatchLengthFromRight)
	{
	return iContactsFile.FieldMatcher().MatchPhoneNumberL(aNumber, aMatchLengthFromRight);
	}


/**
Get a collection of contact IDs which have the given GUID.  Since GUIDs are
unique (i.e. no two contacts can have the same GUID) there will only ever be
one contact ID in this collection.

@param aGuid Contact GUID.

@return Contact ID array containing the contact with the given GUID.
*/
CContactIdArray* CPlCollection::GuidL(const TDesC& aGuid)
	{
	// Create the database query string.
	TBuf<256> searchCond;
	_LIT(KFormat,"%S = '%S'");
	searchCond.Format(KFormat, &KContactGuidString, &aGuid);

	RSqlStatement selectStatement;
	CleanupClosePushL(selectStatement);
		
	iSelectStatement->Reset();	
	iSelectStatement->SetParamL(KContactId, KSpace);
	iSelectStatement->SetConditionL(searchCond);
	
	User::LeaveIfError(selectStatement.Prepare(iContactsFile.NamedDatabase(), iSelectStatement->SqlStringL()));

	CContactIdArray* array = CContactIdArray::NewL();
	CleanupStack::PushL(array);
	
	TInt idIndex = iSelectStatement->ParameterIndex(KContactId);
	TInt err;
	while((err = selectStatement.Next()) == KSqlAtRow)
		{
		array->AddL(selectStatement.ColumnInt(idIndex));	
		}

	if(err != KSqlAtEnd)
		{
		User::LeaveIfError(err);	
		}
		
	CleanupStack::Pop(array);
	CleanupStack::PopAndDestroy(&selectStatement);
	
	return array;
	}


/**
Get a collection of contact IDs which have changed since the given time.

@param aTime Changed since time.

@return Contact ID array of those contacts which have changed since the given
time.
*/
CContactIdArray* CPlCollection::ChangedSinceL(TTime aTime)
	{
	// Create the database query string.
	_LIT(KFormat,"%S >= %LD or %S >= %LD");
	HBufC* searchCond = HBufC::NewLC(KFormat().Length()+
		KContactCreationDate().Length()+KContactLastModified().Length()+
		2 * 64);

	TPtr ptr = searchCond->Des();
	ptr.Format(KFormat, &KContactCreationDate, aTime.Int64(), &KContactLastModified, aTime.Int64());

	RSqlStatement selectStatement;
	CleanupClosePushL(selectStatement);
		
	iSelectStatement->Reset();	
	iSelectStatement->SetParamL(KContactId, KSpace);
	iSelectStatement->SetParamL(KContactTypeFlags, KSpace);
	iSelectStatement->SetConditionL(*searchCond);
	
	User::LeaveIfError(selectStatement.Prepare(iContactsFile.NamedDatabase(), iSelectStatement->SqlStringL()));

	CContactIdArray* array = CContactIdArray::NewL();
	CleanupStack::PushL(array);

	const TInt idIndex = iSelectStatement->ParameterIndex(KContactId);
	const TInt typeFlagsIndex = iSelectStatement->ParameterIndex(KContactTypeFlags);
	TInt err;

	// Iterate through the view and include only those contacts of the desired
	// type (as determined by CheckType()).
	while((err = selectStatement.Next()) == KSqlAtRow)
		{
		TInt typeFlags = selectStatement.ColumnInt(typeFlagsIndex);
		TUid contactType = TCntPersistenceUtility::TypeFlagsToContactTypeUid(typeFlags);
		if (iContactsFile.ContactProperties().CheckType(contactType))
			{
			array->AddL(selectStatement.ColumnInt(idIndex));		
			}
		}

	if(err != KSqlAtEnd)
		{
		User::LeaveIfError(err);
		}
		
	CleanupStack::Pop(array);
	CleanupStack::PopAndDestroy(2,searchCond); // searchCond, selectStatement

	return array;
	}


/**
Get a collection of contact IDs which are 'unfiled'.

@return Contact ID array of those contacts which are 'unfiled'.
*/
CContactIdArray* CPlCollection::UnfiledL()
	{
	RSqlStatement selectStmt;
	CleanupClosePushL(selectStmt);
		
	iSelectStatement->Reset();	
	iSelectStatement->SetParamL(KContactId, KSpace);
	iSelectStatement->SetParamL(KContactTypeFlags, KSpace);
	
    User::LeaveIfError(selectStmt.Prepare(iContactsFile.NamedDatabase(), iSelectStatement->SqlStringL()));

	CContactIdArray* array = CContactIdArray::NewL();
	CleanupStack::PushL(array);
	
	TInt idIndex = iSelectStatement->ParameterIndex(KContactId);
	TInt typeFlagsIndex = iSelectStatement->ParameterIndex(KContactTypeFlags);

	// for WHERE contact_group_member_id = [contact id value]
	const TInt KWhereMemberClauseBufSize(KGroupContactGroupMemberId().Size() + KWhereStringEqualsStringFormatText().Size() + KGroupContactGroupMemberIdParam().Size() );
	HBufC* whereMemberIdClause = HBufC::NewLC(KWhereMemberClauseBufSize);
	whereMemberIdClause->Des().AppendFormat(KWhereStringEqualsStringFormatText, &KGroupContactGroupMemberId, &KGroupContactGroupMemberIdParam );
	
	// select group id
	// For a statement in the following format:
	// 	SELECT contact_group_id FROM groups 
	//		WHERE contact_group_member_id = [contact id value];
	//
	TCntSqlStatementType statementType(ESelect, KSqlContactGroupTableName);
	CCntSqlStatement* selectGroupsStmnt = TSqlProvider::GetSqlStatementL(statementType);
	CleanupStack::PushL(selectGroupsStmnt);
	selectGroupsStmnt->SetParamL(KGroupContactGroupId(), KNullDesC() );
	selectGroupsStmnt->SetConditionL(*whereMemberIdClause);
	const TInt KGroupContactGroupMemberIdParamIndex(KFirstIndex); // second parameter in the query

	TInt err;
	while((err = selectStmt.Next()) == KSqlAtRow)
		{
		TInt typeFlags = selectStmt.ColumnInt(typeFlagsIndex);
		TUid contactType = TCntPersistenceUtility::TypeFlagsToContactTypeUid(typeFlags);
		TInt contactAttrib = (typeFlags & EContactAttributes_Mask) >> EContactAttributes_Shift;
		
		if (contactAttrib & CContactItem::EDeleted)
			{
			continue;
			}
			
		if (contactType == KUidContactCard || contactType == KUidContactOwnCard)
			{
			TInt contactId = selectStmt.ColumnInt(idIndex);
            			
        	RSqlStatement selectGroupSqlStmt;
        	CleanupClosePushL(selectGroupSqlStmt);
        	
        	selectGroupSqlStmt.PrepareL(iContactsFile.NamedDatabase(), selectGroupsStmnt->SqlStringL() );
        	User::LeaveIfError(selectGroupSqlStmt.BindInt(KGroupContactGroupMemberIdParamIndex, contactId));
        	if((err = selectGroupSqlStmt.Next()) == KSqlAtEnd)
        	    {
        	    //add the contact id which is not in any group
    			array->AddL(contactId);
        	    }
        	    
        	User::LeaveIfError(err);    
        	CleanupStack::PopAndDestroy(&selectGroupSqlStmt); 
			}
		}

	if(err != KSqlAtEnd)
		{
		User::LeaveIfError(err);
		}
		
	CleanupStack::PopAndDestroy(2, whereMemberIdClause);  //whereMemberIdClause, selectGroupsStmnt #
	CleanupStack::Pop(array);
	CleanupStack::PopAndDestroy(&selectStmt);
	return array;
	}


/**
Get a collection of contact IDs which are marked as deleted.

@return Contact ID array of those contacts which are marked as deleted.
*/
CContactIdArray* CPlCollection::DeletedL()
	{
	RSqlStatement selectStatement;
	CleanupClosePushL(selectStatement);
		
	iSelectStatement->Reset();	
	iSelectStatement->SetParamL(KContactId, KSpace);
	iSelectStatement->SetParamL(KContactTypeFlags, KSpace);

	User::LeaveIfError(selectStatement.Prepare(iContactsFile.NamedDatabase(), iSelectStatement->SqlStringL()));
	
	CContactIdArray* array = CContactIdArray::NewL();
	CleanupStack::PushL(array);
	
	TInt idIndex = iSelectStatement->ParameterIndex(KContactId);
	TInt typeFlagsIndex = iSelectStatement->ParameterIndex(KContactTypeFlags);
	TInt err;
	while((err = selectStatement.Next()) == KSqlAtRow)
		{
		TInt typeFlags = selectStatement.ColumnInt(typeFlagsIndex);
		TInt contactAttrib = (typeFlags & EContactAttributes_Mask) >> EContactAttributes_Shift;
		
		if (contactAttrib & CContactItem::EDeleted)
			{
			array->AddL(selectStatement.ColumnInt(idIndex));
			}			
		}
	
	if(err != KSqlAtEnd)
		{
		User::LeaveIfError(err);	
		}
	
	CleanupStack::Pop(array);
	CleanupStack::PopAndDestroy(&selectStatement); 

	return array;
	}


/**
Perform a synchronous find for the given text and field definition.

@param aText Text to find.
@param aFieldDef Field definition to use.
@param aSessionID

@return Array of contact IDs resulting from the search.
*/
CContactIdArray* CPlCollection::FindL(const TDesC& aText, const CContactItemFieldDef* aFieldDef, TUint aSessionId)
	{
	CContactIdArray* idsFound = CContactIdArray::NewL();
	CleanupStack::PushL(idsFound);

	// Create the "find text" from the given text.
	HBufC *findText = CreateFindTextL(aText);
	CleanupStack::PushL(findText);

	// Find out what tables need to included in the search as determined by the
	// given field definition.
	TInt findFlags = 0;
	TInt identityTableFieldsCount = 0;
	ConstructBitwiseFindFlags(findFlags,identityTableFieldsCount,aFieldDef);
	
	// Search through the first/last name (and their pronunciation) and company
	// names fields in the Identity table.
	if ( SearchIdentityFieldsRequired(findFlags) )
		{
		_LIT(KWhereClause,"(%S LIKE '%S') OR (%S LIKE '%S') OR (%S LIKE '%S') OR (%S LIKE '%S') OR (%S LIKE '%S') OR (%S LIKE '%S')");
		HBufC* searchCond = HBufC::NewLC(KWhereClause().Length() +
								KContactFirstName().Length() + findText->Length() +
								KContactLastName().Length() + findText->Length() +
								KContactCompanyName().Length() + findText->Length() +
								KContactFirstNamePrn().Length() + findText->Length() +
								KContactLastNamePrn().Length() + findText->Length() +
								KContactCompanyNamePrn().Length() + findText->Length());
		searchCond->Des().Format(KWhereClause, &KContactFirstName, findText,
								&KContactLastName, findText, &KContactCompanyName, findText,
								&KContactFirstNamePrn, findText, &KContactLastNamePrn, findText,
								&KContactCompanyNamePrn, findText);
			
		RSqlStatement selectStatement;
		CleanupClosePushL(selectStatement);
		
		iSelectStatement->Reset();	
		iSelectStatement->SetParamL(KContactId, KSpace);
		iSelectStatement->SetParamL(KContactTypeFlags, KSpace);
		iSelectStatement->SetParamL(KContactFirstName, KSpace);
		iSelectStatement->SetParamL(KContactLastName, KSpace);
		iSelectStatement->SetParamL(KContactCompanyName, KSpace);
		iSelectStatement->SetParamL(KContactFirstNamePrn, KSpace);
		iSelectStatement->SetParamL(KContactLastNamePrn, KSpace);
		iSelectStatement->SetParamL(KContactCompanyNamePrn, KSpace);
		iSelectStatement->SetConditionL(*searchCond);
		
		User::LeaveIfError(selectStatement.Prepare(iContactsFile.NamedDatabase(), iSelectStatement->SqlStringL()));
		
		while(PerformFindIterationL(idsFound, aText, selectStatement, findFlags, aSessionId)) 
			{
			// Keep performing find iteration until EFalse is returned.
			}
		
		CleanupStack::PopAndDestroy(2, searchCond); // searchCond, selectStatement
		}

	// Search through the email address fields
	if (findFlags & (EFindInAllFields | EFindInEmailField))
		{
		TCntSqlStatementType statementType(ESelect, KSqlContactCommAddrTableName);
		CCntSqlStatement* cntSelectStatement = TSqlProvider::GetSqlStatementL(statementType);
		CleanupStack::PushL(cntSelectStatement);
		
		_LIT(KWhereClause, "(%S = %d) AND (%S LIKE '%S')");
		HBufC* searchCond = HBufC::NewLC(KWhereClause().Length()+ 
			KCommAddrType().Length() + 
			KCommAddrValue().Length() + findText->Length());
		searchCond->Des().Format(KWhereClause, 
			&KCommAddrType, CPplCommAddrTable::EEmailAddress, 
			&KCommAddrValue, findText);
		
		RSqlStatement selectStatement;
		CleanupClosePushL(selectStatement);
		
		cntSelectStatement->SetParamL(KContactId, KSpace);
		cntSelectStatement->SetConditionL(*searchCond);

		User::LeaveIfError(selectStatement.Prepare(iContactsFile.NamedDatabase(), cntSelectStatement->SqlStringL()));
		
		while(PerformIdFindIterationL(idsFound, selectStatement))
			{
			// Keep performing find iteration until EFalse is returned.
			}
			
		CleanupStack::PopAndDestroy(3, cntSelectStatement); //selectStatement, searchCond, cntSelectStatement
		}

	// Search through the sip address fields
	if (findFlags & (EFindInAllFields | EFindInSIPField))
		{
		TCntSqlStatementType statementType(ESelect, KSqlContactCommAddrTableName);
		CCntSqlStatement* cntSelectStatement = TSqlProvider::GetSqlStatementL(statementType);
		CleanupStack::PushL(cntSelectStatement);
		
		_LIT(KWhereClause, "(%S = %d) AND (%S LIKE '%S')");
		HBufC* searchCond = HBufC::NewLC(KWhereClause().Length()+ 
			KCommAddrType().Length() + 
			KCommAddrValue().Length() + findText->Length());
		searchCond->Des().Format(KWhereClause, 
			&KCommAddrType, CPplCommAddrTable::ESipAddress, 
			&KCommAddrValue, findText);
		
		RSqlStatement selectStatement;
		CleanupClosePushL(selectStatement);
		
		cntSelectStatement->SetParamL(KContactId, KSpace);
		cntSelectStatement->SetConditionL(*searchCond);

		User::LeaveIfError(selectStatement.Prepare(iContactsFile.NamedDatabase(), cntSelectStatement->SqlStringL()));
		
		while(PerformIdFindIterationL(idsFound, selectStatement))
			{
			// Keep performing find iteration until EFalse is returned.
			}
			
		CleanupStack::PopAndDestroy(3, cntSelectStatement); //selectStatement, searchCond, cntSelectStatement
		}
		
	// Search through the searchable text for any other fields.
	if (findFlags & EFindInAllFields)
		{
		iSelectStatement->Reset();
		iSelectStatement->SetParamL(KContactId, KSpace);
		iSelectStatement->SetParamL(KContactTypeFlags, KSpace);
		iSelectStatement->SetParamL(KContactTextFieldHeader, KSpace);
		iSelectStatement->SetParamL(KContactTextFields, KSpace);
		
		RSqlStatement selectStatement;
		CleanupClosePushL(selectStatement);
	
		User::LeaveIfError(selectStatement.Prepare(iContactsFile.NamedDatabase(), iSelectStatement->SqlStringL()));

 		while(FindL(idsFound,*findText,aFieldDef, selectStatement, aSessionId))
			{
			// Keep performing find iteration until EFalse is returned.
			}
		
		CleanupStack::PopAndDestroy(&selectStatement);
		}

	CleanupStack::PopAndDestroy(findText);
	CleanupStack::Pop(idsFound);

	return(idsFound);
	}


/**
Get a collection of contact IDs which match the given filter.

@param aFilter Filter to match.

@return Contact ID array of those contacts which match the given filter.
*/
CContactIdArray* CPlCollection::FilterDatabaseL(CCntFilter& aFilter)
	{
	_LIT(KWhereCondition,"%S >= %LD");

	iSelectStatement->Reset();
	iSelectStatement->SetParamL(KContactId, KSpace);
	iSelectStatement->SetParamL(KContactTypeFlags, KSpace);

	if (aFilter.IncludeModifiedContacts() || aFilter.IncludeDeletedContacts())
		{
		// Create query to search for modified/deleted contacts.
		HBufC* searchCond=HBufC::NewLC(KWhereCondition().Length() + 
			KContactLastModified().Length() + 64); 
			
		TPtr ptr=searchCond->Des();
		ptr.Format(KWhereCondition, &KContactLastModified, aFilter.GetFilterDateTime().Int64());
	
		iSelectStatement->SetConditionL(*searchCond);
			
		CleanupStack::PopAndDestroy(searchCond);	
		}
	else if (aFilter.IncludeNewContacts())
		{
		// Create query to search for inserted contacts.
		HBufC* searchCond=HBufC::NewLC(KWhereCondition().Length() +
			KContactCreationDate().Length() + 64);

		TPtr ptr=searchCond->Des();
		ptr.Format(KWhereCondition, &KContactCreationDate, aFilter.GetFilterDateTime().Int64());
		
		iSelectStatement->SetConditionL(*searchCond);
		
		CleanupStack::PopAndDestroy(searchCond);
		}

	RSqlStatement selectStatement;
	CleanupClosePushL(selectStatement);
	
	const TInt idIndex = iSelectStatement->ParameterIndex(KContactId);
	const TInt typeFlagsIndex = iSelectStatement->ParameterIndex(KContactTypeFlags);

	TBool filterIncludesAllContacts = !(aFilter.IncludeDeletedContacts() ||
		aFilter.IncludeNewContacts() || aFilter.IncludeModifiedContacts());

	CContactIdArray* idArray = CContactIdArray::NewL();
	CleanupStack::PushL(idArray);

	User::LeaveIfError(selectStatement.Prepare(iContactsFile.NamedDatabase(), iSelectStatement->SqlStringL()));
		
	TInt err;
	while((err = selectStatement.Next()) == KSqlAtRow)
		{
		TInt typeFlags = selectStatement.ColumnInt(typeFlagsIndex);
		
		TUid type = TCntPersistenceUtility::TypeFlagsToContactTypeUid(typeFlags);
		TUint32 attribs = (typeFlags & EContactAttributes_Mask) >> EContactAttributes_Shift;

		if (((aFilter.IncludeDeletedContacts() && (attribs & CContactItem::EDeleted)) 
		||  (aFilter.IncludeNewContacts() 
		||   aFilter.IncludeModifiedContacts()) && (aFilter.TestContactFilterType(type)))
		|| (filterIncludesAllContacts && aFilter.TestContactFilterType(type))) 
			{
			TContactItemId id = selectStatement.ColumnInt(idIndex);
			idArray->AddL(id);
			}
		}
		
	if(err != KSqlAtEnd)
		{
		User::LeaveIfError(err);
		}

	CleanupStack::Pop(idArray);
	CleanupStack::PopAndDestroy(&selectStatement);
	
	return idArray;
	}


/**
Perform a find interation based on given sql statement for the given text.

@param aIdsFound On return the contact IDs found.
@param aText Text to find.
@param aStatement sql statement
@param aFieldsToSearch Fields within the view to search.
@param aSessionId Session ID (only relevant for ICC contacts).

@return ETrue if further iterations are required (i.e. view has not been fully
evaluated and searched), EFalse otherwise.
*/
TBool CPlCollection::PerformFindIterationL(CContactIdArray* aIdsFound,const TDesC& aText,RSqlStatement aStatement,  TInt aFieldsToSearch, TUint aSessionId)
	{
	TContactItemId contactId;

	const TInt idIndex = aStatement.ColumnIndex(KContactId);
	User::LeaveIfError(idIndex);
	const TInt typeFlagsIndex = aStatement.ColumnIndex(KContactTypeFlags);
	User::LeaveIfError(typeFlagsIndex);
	TInt err;
	
	const TInt firstNameIndex = aStatement.ColumnIndex(KContactFirstName);
	User::LeaveIfError(firstNameIndex);
	const TInt lastNameIndex = aStatement.ColumnIndex(KContactLastName);
	User::LeaveIfError(lastNameIndex);
	const TInt companyNameIndex = aStatement.ColumnIndex(KContactCompanyName);
	User::LeaveIfError(companyNameIndex);
	const TInt firstNamePrnIndex = aStatement.ColumnIndex(KContactFirstNamePrn);
	User::LeaveIfError(firstNamePrnIndex);
	const TInt lastNamePrnIndex = aStatement.ColumnIndex(KContactLastNamePrn);
	User::LeaveIfError(lastNamePrnIndex);
	const TInt companyNamePrnIndex = aStatement.ColumnIndex(KContactCompanyNamePrn);
	User::LeaveIfError(companyNamePrnIndex);
	
	while((err = aStatement.Next()) == KSqlAtRow)
		{
		// Do not include contact if it is not of the desired type.
		TInt typeFlags = aStatement.ColumnInt(typeFlagsIndex);
		TUid contactType = TCntPersistenceUtility::TypeFlagsToContactTypeUid(typeFlags);

		if(iContactsFile.ContactProperties().CheckType(contactType) == EFalse)
			{
			continue;
			}

		contactId = aStatement.ColumnInt(idIndex);

		// Check that the contact is not already present in the given contact
		// IDs before searching for the given text.
		if(aIdsFound->Find(contactId)==KErrNotFound)
			{
			TBool findTextInFields = ((aFieldsToSearch & EFindInAllFields) || (aFieldsToSearch & EFindFirstName)) ? (aStatement.ColumnTextL(firstNameIndex).FindC(aText.Ptr(), aText.Length(), KCollationLevel) != KErrNotFound ) : EFalse;
			findTextInFields = findTextInFields || (((aFieldsToSearch & EFindInAllFields) || (aFieldsToSearch & EFindLastName)) ? (aStatement.ColumnTextL(lastNameIndex).FindC(aText.Ptr(), aText.Length(), KCollationLevel) != KErrNotFound ) : EFalse);
			findTextInFields = findTextInFields || (((aFieldsToSearch & EFindInAllFields) || (aFieldsToSearch & EFindCompanyName)) ? (aStatement.ColumnTextL(companyNameIndex).FindC(aText.Ptr(), aText.Length(), KCollationLevel) != KErrNotFound ) : EFalse);
			findTextInFields = findTextInFields || (((aFieldsToSearch & EFindInAllFields) || (aFieldsToSearch & EFindFirstNamePronunciation)) ? (aStatement.ColumnTextL(firstNamePrnIndex).FindC(aText.Ptr(), aText.Length(), KCollationLevel) != KErrNotFound ) :EFalse);
			findTextInFields = findTextInFields || (((aFieldsToSearch & EFindInAllFields) || (aFieldsToSearch & EFindLastNamePronunciation)) ? (aStatement.ColumnTextL(lastNamePrnIndex).FindC(aText.Ptr(), aText.Length(), KCollationLevel) != KErrNotFound ) :EFalse);
			findTextInFields = findTextInFields || (((aFieldsToSearch & EFindInAllFields) || (aFieldsToSearch & EFindCompanyNamePronunciation)) ? (aStatement.ColumnTextL(companyNamePrnIndex).FindC(aText.Ptr(), aText.Length(), KCollationLevel) != KErrNotFound ) : EFalse);
			
			if(findTextInFields)
				{
				if (contactType == KUidContactICCEntry)
					{
					TInt error(iContactsFile.ContactProperties().ContactSynchroniserL(aSessionId).ValidateContact(MContactSynchroniser::ESearch,contactId));
					User::LeaveIfError(error);
					}
				aIdsFound->AddL(contactId);
				}
			}
		}
	User::LeaveIfError(err);	

	return (EFalse);
	}

/**
Perform a find interation based on the given sql statement for the given text.

@param aIdsFound On return the contact IDs found.
@param aStatement sql statement

@return ETrue if further iterations are required (i.e. view has not been fully
evaluated and searched), EFalse otherwise.
*/
TBool CPlCollection::PerformIdFindIterationL(CContactIdArray *aIdsFound, RSqlStatement aStatement)
	{
	const TInt idIndex = aStatement.ColumnIndex(KContactId);
	User::LeaveIfError(idIndex);	
	TInt err;
	while((err = aStatement.Next()) == KSqlAtRow)
		{
		TInt contactId = aStatement.ColumnInt(idIndex);
		
		// Check that the contact is not already present in the given contact
		// IDs before adding it to the array of contact IDs.
		if(aIdsFound->Find(contactId) == KErrNotFound)
			{
			aIdsFound->AddL(contactId);
			}
		}
	User::LeaveIfError(err);	

	return (EFalse);
	}

/**
Utility method used to build the search string
*/
void AppendSearchStringSegment(TDes16& aOrderFields,const TDesC& aColName,  
	const TPtrC aTxtPtr, TBool aAppendOr)
	{
	aOrderFields.Append(KLeftBracket);
	aOrderFields.Append(aColName);
	aOrderFields.Append(KLikeString);
	aOrderFields.Append(aTxtPtr);
	aOrderFields.Append(KLikeStringRight);
	if(aAppendOr)
		{
		aOrderFields.Append(KOR);
		}
	}

/**
Utility method used to build search string
*/
void ConstructSearchString(TInt aFieldDef,TInt aNoFields,TDes16& aOrderFields,TPtrC aTxtPtr)
	{
	const void* KColNamesFields[] =
		{
		&KContactFirstName, &KContactLastName, &KContactCompanyName,
		&KContactFirstNamePrn, &KContactLastNamePrn,
		&KContactLastNamePrn
		};
		
 	for(TInt ii=0;ii<aNoFields;ii++)
		{
		for(TInt name = 0; name < sizeof(KNameFlags)/sizeof(TFindFieldFlags); ++name)
			{
			if(aFieldDef&KNameFlags[name])
				{
				AppendSearchStringSegment(aOrderFields,*((TDesC*)KColNamesFields[name]),  aTxtPtr, (ii+1)<aNoFields);
				aFieldDef &= ~KNameFlags[name];
				break;
				}
			}
		}
	}


/**
This method is required for asynchronous finds which have an initialisation
method followed by one or more calls to the FindAsyncL() method.  The class
needs to be reset to a known state when a find is completed.
*/
void CPlCollection::Reset()
	{
	// Could introduce methods specific to the find operation such that this
	// method is called only if an asynchronous find has taken place. 
	delete iFieldDef;
	iFieldDef = NULL;
	delete iTextDef;
	iTextDef = NULL;
	delete iText;
	iText = NULL;
	delete iOriginalText;
	iOriginalText = NULL;
	delete iFindWords;
	iFindWords = NULL;
	delete iFindWords2;
	iFindWords2 = NULL;
	iNoIdentitySearchColumns = 0;
	iFindFlags = 0;
	iFindState = 0;	
	
	selectBlobStatement.Close();
	selectIdentityStatement.Close();
	selectEmailStatement.Close();
	selectSIPStatement.Close();
	selectIdFromIdentityStatement.Close();
	}


/**
Initialise the Persistence Layer collection class ready for iterative calls to
the FindAsyncL() method.  This form of initialisation is for an asynchronous
find which uses text and a field definition.

@param aText Find text.
@param aFieldDef Field definition to use in find.

@leave KErrNoMemory Out of memory.
*/
void CPlCollection::FindAsyncInitL(const TDesC& aText,CContactItemFieldDef* aFieldDef)
	{
	_LIT(KWhereEmailSIPClause, "(%S = %d) AND (%S LIKE '%S')");
	// Persistence Layer CPlCollection is not deleted but Reset()'s the member
	// variables for iterative FindAsyncL() calls.
	Reset();
	iText = CreateFindTextL(aText);
	iFieldDef = aFieldDef;
	iOriginalText = aText.AllocL();
	ConstructBitwiseFindFlags(iFindFlags,iNoIdentitySearchColumns,iFieldDef);
	// Construct RSqlStatements used for search
	
	HBufC *findText = CreateFindTextL(*iOriginalText);
	CleanupStack::PushL(findText);
	
	// first construct select statement for searching in identity fields
	if ( SearchIdentityFieldsRequired(iFindFlags) )
		{
		_LIT(KWhereClause,"(%S LIKE '%S') OR (%S LIKE '%S') OR (%S LIKE '%S') OR (%S LIKE '%S') OR (%S LIKE '%S') OR (%S LIKE '%S')");
		HBufC* searchCond=HBufC::NewLC(KWhereClause().Length() +
						KContactFirstName().Length() + findText->Length() +
						KContactLastName().Length() + findText->Length() +
						KContactCompanyName().Length() + findText->Length() +
						KContactFirstNamePrn().Length() + findText->Length() +
						KContactLastNamePrn().Length() + findText->Length() +
						KContactCompanyNamePrn().Length() + findText->Length());
		searchCond->Des().Format(KWhereClause, &KContactFirstName, findText,
						&KContactLastName, findText, &KContactCompanyName, findText,
						&KContactFirstNamePrn, findText, &KContactLastNamePrn, findText,
						&KContactCompanyNamePrn, findText);
					
		iSelectStatement->Reset();	
		iSelectStatement->SetParamL(KContactId, KSpace);
		iSelectStatement->SetParamL(KContactTypeFlags, KSpace);
		iSelectStatement->SetParamL(KContactFirstName, KSpace);
		iSelectStatement->SetParamL(KContactLastName, KSpace);
		iSelectStatement->SetParamL(KContactCompanyName, KSpace);
		iSelectStatement->SetParamL(KContactFirstNamePrn, KSpace);
		iSelectStatement->SetParamL(KContactLastNamePrn, KSpace);
		iSelectStatement->SetParamL(KContactCompanyNamePrn, KSpace);
		iSelectStatement->SetConditionL(*searchCond);
		
		User::LeaveIfError(selectIdentityStatement.Prepare(iContactsFile.NamedDatabase(), iSelectStatement->SqlStringL()));
		CleanupStack::PopAndDestroy(searchCond); 
		}
	//end construction for statement for searching in identity fields

	//construct statement for search email
	if (iFindFlags & (EFindInAllFields | EFindInEmailField))
		{
		TCntSqlStatementType statementType(ESelect, KSqlContactCommAddrTableName);
		CCntSqlStatement* cntSelectStatement = TSqlProvider::GetSqlStatementL(statementType);
		CleanupStack::PushL(cntSelectStatement);
	
		HBufC* searchCond = HBufC::NewLC(KWhereEmailSIPClause().Length()+ 
				KCommAddrType().Length() + 
				KCommAddrValue().Length() + findText->Length());
		searchCond->Des().Format(KWhereEmailSIPClause, 
				&KCommAddrType, CPplCommAddrTable::EEmailAddress, 
				&KCommAddrValue, findText);
	
		cntSelectStatement->SetParamL(KContactId, KSpace);
		cntSelectStatement->SetConditionL(*searchCond);

		User::LeaveIfError(selectEmailStatement.Prepare(iContactsFile.NamedDatabase(), cntSelectStatement->SqlStringL()));
		CleanupStack::PopAndDestroy(2, cntSelectStatement); // cntSelectStatement, searchCond
		}
	//end construction for statement for search email

	//construct statement for search sip values
	if (iFindFlags & (EFindInAllFields | EFindInSIPField))
		{
		TCntSqlStatementType statementType(ESelect, KSqlContactCommAddrTableName);
		CCntSqlStatement* cntSelectStatement = TSqlProvider::GetSqlStatementL(statementType);
		CleanupStack::PushL(cntSelectStatement);
	
		HBufC* searchCond = HBufC::NewLC(KWhereEmailSIPClause().Length()+ 
				KCommAddrType().Length() + 
				KCommAddrValue().Length() + findText->Length());
		searchCond->Des().Format(KWhereEmailSIPClause, 
				&KCommAddrType, CPplCommAddrTable::ESipAddress, 
				&KCommAddrValue, findText);
	
		cntSelectStatement->SetParamL(KContactId, KSpace);
		cntSelectStatement->SetConditionL(*searchCond);

		User::LeaveIfError(selectSIPStatement.Prepare(iContactsFile.NamedDatabase(), cntSelectStatement->SqlStringL()));
		CleanupStack::PopAndDestroy(2, cntSelectStatement); // cntSelectStatement, searchCond
		}
	//end construction for statement for search sip values

	//construct statement for search in blob
	if (iFindFlags & EFindInAllFields)
		{
		iSelectStatement->Reset();
		iSelectStatement->SetParamL(KContactId, KSpace);
		iSelectStatement->SetParamL(KContactTypeFlags, KSpace);
		iSelectStatement->SetParamL(KContactTextFieldHeader, KSpace);
		iSelectStatement->SetParamL(KContactTextFields, KSpace);
		
		User::LeaveIfError(selectBlobStatement.Prepare(iContactsFile.NamedDatabase(), iSelectStatement->SqlStringL()));
		}
	//end construction for statement for search in blob
	
	CleanupStack::PopAndDestroy(findText);
	}


/**
Add the required fields and search text for SQL query.  Used for asynchronous
find which uses a text definition.

@see CPlCollection::FindAsyncTextDefInitL()
	
@param aOrderFields On return contains the required fields and search text for
SQL query.
*/
void CPlCollection::doAppendFieldsToSearchString(HBufC* aOrderFields) const
  	{
  	TInt noOfFindWords = iFindWords2->Count();
  	TPtr16 orderFieldPtr = aOrderFields->Des();
  	for(TInt searchWord = 0;searchWord < noOfFindWords; ++searchWord)
  		{
  		ConstructSearchString(iFindFlags,iNoIdentitySearchColumns,orderFieldPtr,(*iFindWords2)[searchWord]);
  		if ((searchWord+1) < noOfFindWords)
  			{
  			orderFieldPtr.Append(KOR);
  			}
  		}		
  	}


/**
Initialise the Persistence Layer collection class ready for iterative calls to
the FindAsyncL() method.  This form of initialisation is for an asynchronous
find which uses a text definition and an array of "find words".

@param aWords "Find words" array.
@param aTextDef Text definition to use in find.
*/
void CPlCollection::FindAsyncTextDefInitL(const CDesCArray& aWords,CContactTextDef* aTextDef)
	{
	// Persistence Layer CPlCollection is not deleted but Reset()'s the member
	// variables for iterative FindAsyncL() calls.
	Reset();

	iFindWords = new(ELeave) CDesCArrayFlat(5);
	iFindWords2 = new(ELeave) CDesCArrayFlat(5);
	for(TInt loop = 0;loop < aWords.MdcaCount();++loop)
		{
		// Construct iFindWords2 which contains all the strings in the search
		// surrounded by a %.
		TPtrC findWord(aWords.MdcaPoint(loop));
		TKeyCmpTextLength key;
		HBufC* bufPtr = findWord.AllocLC();
		iFindWords->CArrayFixBase::InsertIsqAllowDuplicatesL(&bufPtr,key);
		CleanupStack::Pop(); // bufPtr
		HBufC *findText = CreateFindTextL(findWord);
		CleanupStack::PushL(findText);
		iFindWords2->AppendL(*findText);
		CleanupStack::PopAndDestroy(); // findText
		}

	iTextDef = aTextDef;
	ConstructBitwiseFlagsFromTextDef(iFindFlags,iNoIdentitySearchColumns,iTextDef);

	iSelectStatement->Reset();	
	iSelectStatement->SetParamL(KContactId, KSpace);
	iSelectStatement->SetParamL(KContactTypeFlags, KSpace);
	iSelectStatement->SetParamL(KContactFirstName, KSpace);
	iSelectStatement->SetParamL(KContactLastName, KSpace);
	iSelectStatement->SetParamL(KContactCompanyName, KSpace);
	iSelectStatement->SetParamL(KContactFirstNamePrn, KSpace);
	iSelectStatement->SetParamL(KContactLastNamePrn, KSpace);
	iSelectStatement->SetParamL(KContactCompanyNamePrn, KSpace);
		
	if(UsesIdentityFieldsOnly(iFindFlags))
		{
		TInt querySize = ApproximateSizeOfSearchString(); 
		HBufC* searchCond = HBufC::NewLC(querySize); 
		doAppendFieldsToSearchString(searchCond);
		iSelectStatement->SetConditionL(*searchCond);
		CleanupStack::PopAndDestroy(searchCond); 
		}
		
	User::LeaveIfError(selectIdFromIdentityStatement.Prepare(iContactsFile.NamedDatabase(), iSelectStatement->SqlStringL()));
	}


/**
This method does not do any searching.  It simply extracts the contact IDs from
the view and (ultimately) returns them to the CIdleFinder object on the client
side.

The client can then call CIdleFinder::CheckFindL() to perform the actual search.
Searching cannot be performed server-side because of the need to call a client
supplied callback method (the "find words parser" callback).

@param aIdArray Contact IDs extracted from the view.
@param aSessionId Session ID (only relevant for ICC contacts).

@return ETrue if further iterations are required (i.e. view has not been fully
evaluated), EFalse otherwise.
*/
TBool CPlCollection::GetContactIdsForTextDefFindL(CContactIdArray* aIdArray, TUint aSessionId)
	{
	TInt iterCount = 0;
	TInt err;
	
	const TInt idIndex = selectIdFromIdentityStatement.ColumnIndex(KContactId);
	User::LeaveIfError(idIndex);
	const TInt typeFlagsIndex = selectIdFromIdentityStatement.ColumnIndex(KContactTypeFlags);
	User::LeaveIfError(typeFlagsIndex);
	
	while(iterCount<KFindIterations)
		{
		err = selectIdFromIdentityStatement.Next();
		if(err != KSqlAtRow)
			{
			User::LeaveIfError(err);
			break;
			}
			
		TInt typeFlags = selectIdFromIdentityStatement.ColumnInt(typeFlagsIndex);
		TUid contactType = TCntPersistenceUtility::TypeFlagsToContactTypeUid(typeFlags);
		TContactItemId contactId = selectIdFromIdentityStatement.ColumnInt(idIndex);
		
		if (contactType == KUidContactICCEntry)
			{
			User::LeaveIfError(iContactsFile.ContactProperties().ContactSynchroniserL(aSessionId).ValidateContact(MContactSynchroniser::ESearch,contactId));
			User::LeaveIfError(iContactsFile.ContactProperties().ContactSynchroniserL(aSessionId).ValidateContact(MContactSynchroniser::ERead,contactId));
			}
		// Check that the contact is not already present in the given
		// contact IDs before adding to array.
		if(aIdArray->Find(contactId) == KErrNotFound)
			{
			aIdArray->AddL(contactId);		
			}
		}
	
	return(iterCount == KFindIterations);	
	}


/**
Perform an asynchronous find iteration.

@param aMoreToGo On return ETrue if further iterations are required to complete
the asynchronous find, EFalse otherwise.
@param aSessionId Session ID (only relevant for ICC contacts).

@return Array of contact ID found in this find iteration.
*/
CContactIdArray* CPlCollection::FindAsyncL(TBool& aMoreToGo, TUint aSessionId)
	{
	CContactIdArray* idArray = CContactIdArray::NewLC();
	TBool moreToGo = EFalse;

	if (iFindWords) // Text definition and "find words".
		{
		TRAPD(err, moreToGo=GetContactIdsForTextDefFindL(idArray, aSessionId));

		if (err != KErrNone)
			{
			moreToGo = EFalse;
			Reset();
			User::Leave(err);			
			}

		if(moreToGo == EFalse)
			{
			iFindState |= EFindInTextDefFinished;
			}
		}
	else // Search for a single string.
		{
		if(!(iFindState & EFindInBlobFinished) && (iFindFlags & EFindInAllFields))
			{
			// Search in searchable text (contents of BLOB).
			if(!FindL(idArray, *iText, iFieldDef, selectBlobStatement, aSessionId))
				{
				iFindState |= EFindInBlobFinished;
				}
			else
			    {
        		moreToGo = ETrue;
			    }	
			}

		if(!(iFindState & EFindInIdentityFinished))
			{
			if(SearchIdentityFieldsRequired(iFindFlags))
				{		
				// Search in identity fields.
				if(!PerformFindIterationL(idArray, *iOriginalText, selectIdentityStatement, iFindFlags, aSessionId))
					{
					iFindState |= EFindInIdentityFinished;
					}
    			else
    			    {
            		moreToGo = ETrue;
    			    }	
				}
			}

		if(!(iFindState & EFindInEmailFinished))
			{
			if(iFindFlags & (EFindInAllFields | EFindInEmailField))
				{
				// Search in email fields.		
				if(!PerformIdFindIterationL(idArray, selectEmailStatement))
					{
					iFindState |= EFindInEmailFinished;
					}
    			else
    			    {
            		moreToGo = ETrue;
    			    }	
				}
			}
		
		if(!(iFindState & EFindInSIPFinished))
			{
			if(iFindFlags & (EFindInAllFields | EFindInSIPField))
				{
				// Search in email fields.		
				if(!PerformIdFindIterationL(idArray, selectSIPStatement))
					{
					iFindState |= EFindInSIPFinished;
					}
    			else
    			    {
            		moreToGo = ETrue;
    			    }	
				}
			}
		}


    if(!moreToGo)
        {
		Reset();
        }

/*        
	// Check if we have finished searching.
	if ((iFindState&(EFindInBlobFinished|EFindInIdentityFinished|EFindInEmailFinished)) == (EFindInBlobFinished|EFindInIdentityFinished|EFindInEmailFinished)
		 || iFindState&EFindInTextDefFinished)
		{
		moreToGo = EFalse;
		Reset();
		}
*/
	aMoreToGo = moreToGo;

	// Pop this off the cleanup stack, as we are passing on ownership to the calling method
	CleanupStack::Pop(idArray);

	return idArray;
	}


/**
Perform a synchronous find for the given text and field definition.

@param aIdsFound Contacts IDs found.
@param aText Text to search for.
@param aFieldDef Field definition to use.
@param aSessionId Session ID (only relevant for ICC contacts).

@return ETrue if all iterations have been completed, EFalse otherwise.
*/
TBool CPlCollection::FindL(CContactIdArray *aIdsFound, const TDesC& aText,const CContactItemFieldDef *aFieldDef, RSqlStatement aStatement, TUint aSessionId)
	{
  	const CContactTemplate& systemTemplate = iContactsFile.ContactProperties().SystemTemplateManager().TemplateL();

	CContactItemField* field = CContactItemField::NewLC();	// construct just once

	TInt endOfQueryString = aText.Length()-2;
	TPtrC originalFindText = aText.Mid(1,endOfQueryString);

	TInt iterCount = 0;
	TInt err;
	
	const TInt idIndex = aStatement.ColumnIndex(KContactId);
	User::LeaveIfError(idIndex);
	const TInt typeFlagsIndex = aStatement.ColumnIndex(KContactTypeFlags);
	User::LeaveIfError(typeFlagsIndex);
	const TInt textHeaderIndex = aStatement.ColumnIndex(KContactTextFieldHeader);
	User::LeaveIfError(textHeaderIndex);
	const TInt textFieldsIndex = aStatement.ColumnIndex(KContactTextFields);
	User::LeaveIfError(textFieldsIndex);
	
	while(iterCount<KFindIterations)
		{
		err = aStatement.Next();
		if(err != KSqlAtRow)
			{
			User::LeaveIfError(err);
			break;
			}
			
		TInt typeFlags = aStatement.ColumnInt(typeFlagsIndex);
		TUid contactType = TCntPersistenceUtility::TypeFlagsToContactTypeUid(typeFlags);

		if (iContactsFile.ContactProperties().CheckType(contactType) )
			{
			TPtrC8 textHeader;
			aStatement.ColumnBinary(textHeaderIndex, textHeader);
			RDesReadStream textHeaderStream(textHeader);
			CleanupClosePushL(textHeaderStream);

			CEmbeddedStore* store = CEmbeddedStore::FromLC(textHeaderStream);
			
			TPtrC textFieldPtrC = aStatement.ColumnTextL(textFieldsIndex);
			HBufC *textBuf = HBufC::NewLC(textFieldPtrC.Size());
			textBuf->Des().Copy(textFieldPtrC);
			
			RStoreReadStream rootStream;
			rootStream.OpenLC(*store ,store->Root());
			
			TCardinality  count;
			rootStream >> count;

			for (TInt i = 0;i < count; ++i)
				{
				field->Reset();
				if (field->RestoreIfMatchL(rootStream,aFieldDef,&systemTemplate.CardFields(),textBuf,i))
					{
					if (field->StorageType() == KStorageTypeText && 
						field->TextStorage()->Text().FindC(originalFindText.Ptr(), 
						originalFindText.Length(), KCollationLevel) != KErrNotFound)
						{
						TContactItemId id = aStatement.ColumnInt(idIndex);
						if(aIdsFound->Find(id) == KErrNotFound)
							{
							if (contactType == KUidContactICCEntry)
								{
								TInt error(iContactsFile.ContactProperties().ContactSynchroniserL(aSessionId).ValidateContact(MContactSynchroniser::ESearch,id));
								User::LeaveIfError(error);
								}
							aIdsFound->AddL(id);
							}
						break;
						}
					}
				}
			CleanupStack::PopAndDestroy(4, &textHeaderStream); // rootStream, textBuf, textHeader, textHeaderStream
			}
		++iterCount;
		}

	CleanupStack::PopAndDestroy(field); 

	return(iterCount == KFindIterations);
	}

/**
Return an over-estimated string size for the amount of syntax required per field
per search string for the Identity table.

@return Maximum string size required for Identity table search syntax.
*/
TInt CPlCollection::MaximumSizeOfIdentitySearchSyntax()
	{
	TInt sqlLitSize;

	sqlLitSize = KLeftBracket().Length() + KLikeString().Length() +
		KLikeStringRight().Length() + KOR().Length();

	sqlLitSize *= iNoIdentitySearchColumns;

	sqlLitSize += KContactFirstName().Length() + KContactLastName().Length() +
		KContactCompanyName().Length();

	sqlLitSize += KContactFirstNamePrn().Length() +
		KContactLastNamePrn().Length() +
		KContactCompanyNamePrn().Length();

	return(sqlLitSize);
	}
	
/**
Determine the approximate size of the string required to construct the SQL query
on the Identity table when using the "find words".

@return Approximate size of search string.
*/
TInt CPlCollection::ApproximateSizeOfSearchString()
	{
	TInt sqlLitSize = MaximumSizeOfIdentitySearchSyntax();

	TInt noOfFindWords=iFindWords2->Count();
	TInt size=0;

	for(TInt searchWord=0;searchWord < noOfFindWords; ++searchWord)
		{
		size += (*iFindWords2)[searchWord].Length();
		}
	
	TInt totalSize = noOfFindWords*sqlLitSize + iNoIdentitySearchColumns*size + 256;
	
	return totalSize;
	}
	
/**
Utility method used to filter hint fields
*/	
TBool CPlCollection::HintFieldMatchesFilter(TInt aHintField, TInt aFilter)
	{
	TBool match=EFalse;

	if (aFilter==CContactDatabase::EUnfiltered)
		{
		return ETrue;
		}

	if (aFilter & ~aHintField & (CContactDatabase::EWork | CContactDatabase::EHome))
		{
		return EFalse;
		}

	match = aFilter & aHintField
		& (
			  CContactDatabase::ELandLine
			| CContactDatabase::ESmsable
			| CContactDatabase::EMailable
			| CContactDatabase::EFaxable
			| CContactDatabase::EPhonable
			| CContactDatabase::ERingTone
			| CContactDatabase::EVoiceDial
			| CContactDatabase::EIMAddress
			| CContactDatabase::EWirelessVillage
			| CContactDatabase::ECustomFilter1
			| CContactDatabase::ECustomFilter2
			| CContactDatabase::ECustomFilter3
			| CContactDatabase::ECustomFilter4
			);

	return match;
	}
