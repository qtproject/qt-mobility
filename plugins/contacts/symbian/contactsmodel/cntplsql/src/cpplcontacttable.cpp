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
#include "cntpersistenceutility.h"
#include <cntdef.h>

// forward declaration to allow this to compile. 
// Which header file is this declared in and do we actually still need the properties here?
class CLplContactProperties;

/**
NewL

@param aDatabase A handle to the database.
@param aProperties A contact properties object.

@return A pointer to a new CPplContactTable object.
*/
CPplContactTable* CPplContactTable::NewL(RSqlDatabase& aDatabase, CLplContactProperties& aProperties)
	{
	CPplContactTable* self = CPplContactTable::NewLC(aDatabase, aProperties);
	CleanupStack::Pop(self);
	return self;
	}


/**
NewLC

@param aDatabase A handle to the database.
@param aProperties A contact properties object.

@return A pointer to a new CPplContactTable object.
*/
CPplContactTable* CPplContactTable::NewLC(RSqlDatabase& aDatabase, CLplContactProperties& aProperties)
	{
	CPplContactTable* self = new (ELeave) CPplContactTable(aDatabase, aProperties);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}


/**
Set up the CCntSqlStatement objects held by the class.
*/
void CPplContactTable::ConstructL()
	{	
	iCardTemplateIds = CContactIdArray::NewL();
	
	// Statement types
	TCntSqlStatementType insertType(EInsert, KSqlContactTableName);
	TCntSqlStatementType selectType(ESelect, KSqlContactTableName);
	TCntSqlStatementType updateType(EUpdate, KSqlContactTableName);
	TCntSqlStatementType deleteType(EDelete, KSqlContactTableName);

	// Where clause

	// sizes of the clause
	const TInt KWhereClauseBufSize(KContactId().Size() + 
		KWhereStringEqualsStringFormatText().Size() + KContactIdParam().Size() );

	// for WHERE contact_id = [contact id value]
	HBufC* whereIdClause = HBufC::NewLC(KWhereClauseBufSize);
	whereIdClause->Des().AppendFormat(KWhereStringEqualsStringFormatText, &KContactId, 
		&KContactIdParam );

	// INSERT

	// insert record into contact table
	// For a statement in the following format:
	// 	INSERT INTO contact 
	//		(contact_id, template_id...[etc...], binary_fields) 
	//		VALUES (NULL, 37, .......);
	//
	// Actual parameters are added when inserting a contact based
	// in available fields
	iInsertStmnt = TSqlProvider::GetSqlStatementL(insertType);

	// SELECT

	// delete select
	// For a statement in the following format:
	// 	SELECT type_flags, access_count, template_id FROM contact 
	//		WHERE contact_id = [contact id value];
	//
	iDeleteSelectStmnt = TSqlProvider::GetSqlStatementL(selectType);	
	iDeleteSelectStmnt->SetParamL(KContactTypeFlags, KNullDesC() );
	iDeleteSelectStmnt->SetParamL(KContactAccessCount, KNullDesC() );
	iDeleteSelectStmnt->SetParamL(KContactTemplateId, KNullDesC() );
	iDeleteSelectStmnt->SetConditionL(*whereIdClause);

	// UPDATE
	
	// update record from contact table
	// For a statement in the following format:
	// 	UPDATE contact SET template_id = [new template id value]
	//      ...,
	//      binary_fields = [new binary fields value]
	//		WHERE contact_id = [contact id value];
	//
	iUpdateStmnt = TSqlProvider::GetSqlStatementL(updateType);
	iUpdateStmnt->SetConditionL(*whereIdClause);

	// type_flags update
	// For a statement in the following format:
	// 	UPDATE contact SET type_flags = [new type flags value]
	//		WHERE contact_id = [contact id value];
	//
	iUpdateFlagsStmnt = TSqlProvider::GetSqlStatementL(updateType);	
	iUpdateFlagsStmnt->SetParamL(KContactTypeFlags, KContactTypeFlagsParam);
	iUpdateFlagsStmnt->SetConditionL(*whereIdClause);

	// AccessCount update
	// For a statement in the following format:
	// 	UPDATE contact SET access_count = [new access count value]
	//		WHERE contact_id = [contact id value];
	//
	iAccessCountUpdateStmnt = TSqlProvider::GetSqlStatementL(updateType);	
	iAccessCountUpdateStmnt->SetParamL(KContactAccessCount, KContactAccessCountParam);
	iAccessCountUpdateStmnt->SetConditionL(*whereIdClause);

	// DELETE

	// delete record from contact table
	// For a statement in the following format:
	// 	DELETE FROM contact WHERE contact_id = [contact id value];
	//
	iDeleteStmnt = TSqlProvider::GetSqlStatementL(deleteType);	
	iDeleteStmnt->SetConditionL(*whereIdClause);

	// Set up the field lookup table hash map
	iFieldMap.InsertL(KUidContactFieldGivenNameValue, KContactFirstNameParam() );
	iFieldMap.InsertL(KUidContactFieldGivenNamePronunciationValue, KContactFirstNamePrnParam() );
	iFieldMap.InsertL(KUidContactFieldFamilyNameValue, KContactLastNameParam() );
	iFieldMap.InsertL(KUidContactFieldFamilyNamePronunciationValue, KContactLastNamePrnParam() );
	iFieldMap.InsertL(KUidContactFieldCompanyNameValue, KContactCompanyNameParam() );
	iFieldMap.InsertL(KUidContactFieldCompanyNamePronunciationValue, KContactCompanyNamePrnParam() );

	CleanupStack::PopAndDestroy(); //whereIdClause
	}

/**
Destructor

Tidy up CCntSqlStatement objects
*/
CPplContactTable::~CPplContactTable()
	{
	delete iInsertStmnt;
	delete iDeleteSelectStmnt;
	delete iUpdateFlagsStmnt;
	delete iUpdateStmnt;
	delete iAccessCountUpdateStmnt;
	delete iDeleteStmnt;
	iFieldMap.Close();
	delete iCardTemplateIds;
	}


/**
CPplContactTable constructor
*/
CPplContactTable::CPplContactTable(RSqlDatabase& aDatabase, CLplContactProperties& aProperties):
	iProperties(aProperties),
	iDatabase(aDatabase)
	{
	}


/**
Update the access count for the given template ID. If the template is marked for deletion and 
no one is referencing it any longer, it is deleted.

@param aTemplateRefId
@param aIncrement If ETrue increase the reference count otherwise decrease the reference count.
*/
void CPplContactTable::UpdateTemplateAccessCounterL(TContactItemId aTemplateRefId, TBool aIncrement)
	{
	if (aTemplateRefId == KGoldenTemplateId || aTemplateRefId == KNullContactId)
		{
		return;
		}

	// Get the access count and type flags for the template
	RSqlStatement selectStmnt;
	CleanupClosePushL(selectStmnt);
	selectStmnt.PrepareL(iDatabase, iDeleteSelectStmnt->SqlStringL() );
	const TInt KContactIdParamIndex(KFirstIndex); // first and only parameter in the query
	User::LeaveIfError(selectStmnt.BindInt(KContactIdParamIndex, aTemplateRefId) );

	TInt err(selectStmnt.Next() );
	if (err != KSqlAtRow)
		{
		User::Leave(err);
		}

	TInt oldAccessCount(selectStmnt.ColumnInt(iDeleteSelectStmnt->ParameterIndex(KContactAccessCount() ) ) );
	TInt newAccessCount = oldAccessCount + (aIncrement ? 1 : -1);
	TInt typeFlags(selectStmnt.ColumnInt(iDeleteSelectStmnt->ParameterIndex(KContactTypeFlags() ) ) );
	TInt attributes((typeFlags & EContactAttributes_Mask) >> EContactAttributes_Shift); 
	CleanupStack::PopAndDestroy(&selectStmnt);

	// set the new access count for the template
	RSqlStatement updateStmnt;
	CleanupClosePushL(updateStmnt);
	updateStmnt.PrepareL(iDatabase, iAccessCountUpdateStmnt->SqlStringL() );
	
	const TInt KAccessCountParamIndex(KFirstParam);					// first parameter in query...
	const TInt KTemplateIdParamIndex(KAccessCountParamIndex + 1);	// ...and the second.

	User::LeaveIfError(updateStmnt.BindInt(KAccessCountParamIndex, newAccessCount) );
	User::LeaveIfError(updateStmnt.BindInt(KTemplateIdParamIndex, aTemplateRefId) );
	User::LeaveIfError(updateStmnt.Exec() );
	CleanupStack::PopAndDestroy(&updateStmnt);

	// should the template be deleted  -- i.e. nobody accessing it and marked as deleted?
	if (newAccessCount <= 0 && (attributes & CContactItem::EDeleted) )
		{
		TBool lowDiskErr(EFalse);
		CContactItem* templ = DeleteLC(aTemplateRefId, lowDiskErr);
		CleanupStack::PopAndDestroy(templ);
		if (lowDiskErr)
			{
			User::Leave(KErrDiskFull);
			}
		}
	}

void CPplContactTable::CreateInDbL(CContactItem& aItem)
	{
	CreateInDbL(aItem, 0);
	}

/**
Add the given contact item to the database.

@param aItem The contact item to be added to the database. 
@param aSessionId The ID of the session that issued the request.  Used to
prevent Phonebook Synchroniser deadlock.

@return Contact item ID of the contact added to the database.
*/
void CPplContactTable::CreateInDbL(CContactItem& aItem, TUint aSessionId)
	{
	if (aItem.Type() != KUidContactTemplate) // Not creating system template?
		{
		// Make sure the System template is loaded.
		iProperties.SystemTemplateL();
		}
		
	if (aItem.Type() == KUidContactICCEntry) 
		{
		TInt ret = iProperties.ContactSynchroniserL(aSessionId).ValidateWriteContact(static_cast<CContactICCEntry&>(aItem) );
		User::LeaveIfError(ret);	
		}

	// Mark fields as template fields if the contact item is a contact card template.
	const TBool KIsTemplateCard = (aItem.Type() == KUidContactCardTemplate);
	CContactItemFieldSet& fieldset = aItem.CardFields();
	for (TInt i = fieldset.Count() - 1; i >= 0; --i)
		{
		fieldset[i].SetTemplateField(KIsTemplateCard);
		}

	// Set iLastModified and iCreationDate to current time.
	TTime lastModified = aItem.LastModified();
   	lastModified.UniversalTime();
   	aItem.SetLastModified(lastModified);
   	aItem.SetCreationDate(lastModified); 

	// ID of newly created item obtained from autoincrementing ID column.
	// -- get an id from the database	
	RSqlStatement selectIdStatement;
	CleanupClosePushL(selectIdStatement);
		
	User::LeaveIfError(selectIdStatement.Prepare(iDatabase, KSelectLastIdSqlStmnt()));

	TInt err;
	TInt lastId((-1));
	if((err = selectIdStatement.Next()) == KSqlAtRow)
		{
		lastId = selectIdStatement.ColumnInt(0);
		}

	if(err == KSqlAtEnd)
		{
		lastId = -1;
		}
	
	aItem.SetId(lastId + 1);
	
	CleanupStack::PopAndDestroy(&selectIdStatement);

	// Increment Template reference counter.
	UpdateTemplateAccessCounterL(aItem.TemplateRefId(), ETrue);

	// Write contact data to the database.
	WriteContactItemL(aItem, EInsert);
	}

/**
Updates the given contact item in the database.

@param aItem The contact item to be updated
*/
void CPplContactTable::UpdateL(const CContactItem& aItem)
	{
	// If contact is marked as deleted and access count is zero then delete the
	// contact otherwise update it.
	if (aItem.IsDeleted() && (aItem.AccessCount() == 0) )
		{
		//update access field in contact table
		RSqlStatement updateStmnt;
		CleanupClosePushL(updateStmnt);
		updateStmnt.PrepareL(iDatabase, iAccessCountUpdateStmnt->SqlStringL() );
		
		const TInt KAccessCountParamIndex(KFirstParam);					// first parameter in query...
		const TInt KContactIdParamIndex(KAccessCountParamIndex + 1);	// ...and the second.

		User::LeaveIfError(updateStmnt.BindInt(KAccessCountParamIndex, 0) );
		User::LeaveIfError(updateStmnt.BindInt(KContactIdParamIndex, aItem.Id()) );
		User::LeaveIfError(updateStmnt.Exec() );
		CleanupStack::PopAndDestroy(&updateStmnt);

		TBool lowDiskErr(EFalse);
		CContactItem* templ = DeleteLC(aItem.Id(), lowDiskErr);
		CleanupStack::PopAndDestroy(templ);
		if (lowDiskErr)
			{
			User::Leave(KErrDiskFull);
			}
		}
	else
		{
		WriteContactItemL(aItem, EUpdate);
		}
	}

/**
Performs database write operations for the given contact item.

@param aItem the contact item to be written
@param aType the type of operation (insert or update) to 
*/
void CPplContactTable::WriteContactItemL(const CContactItem& aItem, TCntSqlStatement aType)
	{
	// check the correct type of statement/operation has been supplied
	if (aType != EInsert && aType != EUpdate)
		{
		User::Leave(KErrArgument);
		}

	// temporary reference to the CCntSqlStatements member variables to take advantage 
	// of the commonality between update and insert operations.
	CCntSqlStatement* tempCntStmnt;
	if (aType == EInsert)
		{
        iInsertStmnt->ClearParams();
        iInsertStmnt->SetParamL(KContactId, KContactIdParam);
        iInsertStmnt->SetParamL(KContactTemplateId, KContactTemplateIdParam);
        iInsertStmnt->SetParamL(KContactTypeFlags, KContactTypeFlagsParam);
        iInsertStmnt->SetParamL(KContactAccessCount, KContactAccessCountParam);
        iInsertStmnt->SetParamL(KContactCreationDate, KContactCreationDateParam);
        iInsertStmnt->SetParamL(KContactLastModified, KContactLastModifiedParam);
        iInsertStmnt->SetParamL(KContactGuidString, KContactGuidStringParam);
        iInsertStmnt->SetParamL(KContactFirstNamePrn, KContactFirstNamePrnParam);
        iInsertStmnt->SetParamL(KContactLastNamePrn, KContactLastNamePrnParam);
        iInsertStmnt->SetParamL(KContactCompanyNamePrn, KContactCompanyNamePrnParam);
        iInsertStmnt->SetParamL(KContactTextFieldHeader, KContactTextFieldHeaderParam);
        iInsertStmnt->SetParamL(KContactBinaryFieldHeader, KContactBinaryFieldHeaderParam);
        iInsertStmnt->SetParamL(KContactTextFields, KContactTextFieldsParam);
        iInsertStmnt->SetParamL(KContactBinaryFields, KContactBinaryFieldsParam);
        
        for (TInt fieldNum = aItem.CardFields().Count() - 1; fieldNum >= 0; --fieldNum) 
            {
            const CContactItemField& field = aItem.CardFields()[fieldNum];
            const TInt nameFieldUid = NameFieldUid(field);
            
            if (nameFieldUid != KErrNotFound && field.StorageType() == KStorageTypeText)
                {
                TInt length = field.TextStorage()->Text().Length();
                // check if field should be stored in the Identity table.
                if (nameFieldUid == TInt(KUidContactFieldGivenNameValue) && length)
                    {
                    iInsertStmnt->SetParamL(KContactFirstName, KContactFirstNameParam);
                    }
                if (nameFieldUid == TInt(KUidContactFieldFamilyNameValue) && length)
                    {
                    iInsertStmnt->SetParamL(KContactLastName, KContactLastNameParam);
                    }
                if (nameFieldUid == TInt(KUidContactFieldCompanyNameValue) && length)
                    {
                    iInsertStmnt->SetParamL(KContactCompanyName, KContactCompanyNameParam);
                    }         
                }
            }
		tempCntStmnt = iInsertStmnt;
		}
	else
	    {
        iUpdateStmnt->ClearParams();
        iUpdateStmnt->SetParamL(KContactTemplateId, KContactTemplateIdParam);
	    
        _LIT(KOwnCardInvariant, "((((%S>>%d==%d)*%d) | ((NOT(%S>>%d==%d))*%S))<<%d)| %S");
        
	    HBufC* typeFlagsParameter = HBufC::NewLC(KOwnCardInvariant().Size() + KContactTypeFlags().Size() + KContactTypeFlags().Size() + 
	            KContactTypeParam().Size() + KAttributesAndHintParam().Size() + 6*sizeof(TInt));
	    typeFlagsParameter->Des().AppendFormat(KOwnCardInvariant, &KContactTypeFlags, EContactType_Shift, EContactTypeFlags_OwnCard, EContactTypeFlags_OwnCard,
	            &KContactTypeFlags, EContactType_Shift, EContactTypeFlags_OwnCard, &KContactTypeParam, EContactType_Shift, &KAttributesAndHintParam);
	    
	    iUpdateStmnt->SetParamL(KContactTypeFlags, *typeFlagsParameter);
	    
	    iUpdateStmnt->SetParamL(KContactAccessCount, KContactAccessCountParam);
	    iUpdateStmnt->SetParamL(KContactLastModified, KContactLastModifiedParam);
	    iUpdateStmnt->SetParamL(KContactGuidString, KContactGuidStringParam);
	    iUpdateStmnt->SetParamL(KContactFirstNamePrn, KContactFirstNamePrnParam);
	    iUpdateStmnt->SetParamL(KContactLastNamePrn, KContactLastNamePrnParam);
	    iUpdateStmnt->SetParamL(KContactCompanyNamePrn, KContactCompanyNamePrnParam);
	    iUpdateStmnt->SetParamL(KContactTextFieldHeader, KContactTextFieldHeaderParam);
	    iUpdateStmnt->SetParamL(KContactBinaryFieldHeader, KContactBinaryFieldHeaderParam);
	    iUpdateStmnt->SetParamL(KContactTextFields, KContactTextFieldsParam);
	    iUpdateStmnt->SetParamL(KContactBinaryFields, KContactBinaryFieldsParam);
	    
        for (TInt fieldNum = aItem.CardFields().Count() - 1; fieldNum >= 0; --fieldNum) 
            {
            const CContactItemField& field = aItem.CardFields()[fieldNum];
            const TInt nameFieldUid = NameFieldUid(field);
            
            if (nameFieldUid != KErrNotFound && field.StorageType() == KStorageTypeText)
                {
                TInt length = field.TextStorage()->Text().Length();
                // check if field should be stored in the Identity table.
                if (nameFieldUid == TInt(KUidContactFieldGivenNameValue) && length)
                    {
                    iUpdateStmnt->SetParamL(KContactFirstName, KContactFirstNameParam);
                    }
                if (nameFieldUid == TInt(KUidContactFieldFamilyNameValue) && length)
                    {
                    iUpdateStmnt->SetParamL(KContactLastName, KContactLastNameParam);
                    }
                if (nameFieldUid == TInt(KUidContactFieldCompanyNameValue) && length)
                    {
                    iUpdateStmnt->SetParamL(KContactCompanyName, KContactCompanyNameParam);
                    }         
                }
            }
        
        CleanupStack::PopAndDestroy(); //typeFlagsParameter
	    tempCntStmnt = iUpdateStmnt;
	    }
	
	RSqlStatement stmnt;
	CleanupClosePushL(stmnt);
 	stmnt.PrepareL(iDatabase, tempCntStmnt->SqlStringL() );

	// get the identity-type fields and build the hint fields
	THintField hint;
	const RArray<TUid>* custFiltFields = NULL;

	if (aItem.Type() != KUidContactTemplate)
		{
		custFiltFields = &iProperties.CustomFilterableFieldsL(); // doesn't take ownership
		}

	for (TInt fieldNum = aItem.CardFields().Count() - 1; fieldNum >= 0; --fieldNum) 
		{
		const CContactItemField& field = aItem.CardFields()[fieldNum];
		const TInt nameFieldUid = CPplContactTable::NameFieldUid(field);

		// check if field should be stored in the Identity table.
		if (nameFieldUid != KErrNotFound)
			{
			TPtrC textToSet = field.TextStorage()->Text();
			if(textToSet.Length() > 0)
			    {
			    const TInt KParamIndex(User::LeaveIfError(stmnt.ParameterIndex(iFieldMap.FindL(nameFieldUid) ) ) );
    			User::LeaveIfError(stmnt.BindText(KParamIndex, textToSet));
			    }			}
		else if (field.StorageType() == KStorageTypeText &&  // the field is textual
				 field.TextStorage()->Text().Length() &&     // ignore empty fields
				 custFiltFields != NULL)
			{
			// the field is not stored in contact table but potentially maps to a hint
			hint.UpdateHintL(field, *custFiltFields);
			}
		}

	
	
	// bind other values to statement
	if(aType == EInsert) 
		{
		TInt typeFlags(GenerateTypeFlags(aItem.Type(), aItem.Attributes(), hint.iValue) );
		User::LeaveIfError(stmnt.BindInt(
				User::LeaveIfError(stmnt.ParameterIndex(KContactTypeFlagsParam() ) ), typeFlags) );
		}
	else
		{
		User::LeaveIfError(stmnt.BindInt(
				User::LeaveIfError(stmnt.ParameterIndex(KContactTypeParam() ) ), TCntPersistenceUtility::ContactTypeUidToTypeFlags(aItem.Type()) >> EContactType_Shift ) );
		
		TInt attrHint = (aItem.Attributes() << EContactAttributes_Shift) & EContactAttributes_Mask;
		attrHint |= hint.iValue & EContactHintFlags_Mask;
		User::LeaveIfError(stmnt.BindInt(
				User::LeaveIfError(stmnt.ParameterIndex(KAttributesAndHintParam() ) ), attrHint ) );		
		}
	User::LeaveIfError(stmnt.BindInt(
		User::LeaveIfError(stmnt.ParameterIndex(KContactTemplateIdParam() ) ), aItem.TemplateRefId()) );
	User::LeaveIfError(stmnt.BindInt(
		User::LeaveIfError(stmnt.ParameterIndex(KContactAccessCountParam() ) ), aItem.AccessCount()) );
	if (aType == EInsert)
		{
		User::LeaveIfError(stmnt.BindInt64(
			User::LeaveIfError(stmnt.ParameterIndex(KContactCreationDateParam() ) ), aItem.LastModified().Int64() ) );
		}
	TTime time;
	time.UniversalTime();	
	User::LeaveIfError(stmnt.BindInt64(
		User::LeaveIfError(stmnt.ParameterIndex(KContactLastModifiedParam() ) ), time.Int64() ) );
	User::LeaveIfError(stmnt.BindText(
		User::LeaveIfError(stmnt.ParameterIndex(KContactGuidStringParam() ) ), const_cast<CContactItem&>(aItem).Guid() ) );
	User::LeaveIfError(stmnt.BindInt(
		User::LeaveIfError(stmnt.ParameterIndex(KContactIdParam() ) ), aItem.Id() ) );

	// build the clob/blob parts of the update statement
	RSqlParamWriteStream textHeader;
	User::LeaveIfError(textHeader.BindBinary(stmnt, 
		User::LeaveIfError(stmnt.ParameterIndex(KContactTextFieldHeaderParam() ) ) ) );	
	CEmbeddedStore* textEmbeddedStore = CEmbeddedStore::NewLC(textHeader);

	RSqlParamWriteStream binHeader;
	User::LeaveIfError(binHeader.BindBinary(stmnt, 
		User::LeaveIfError(stmnt.ParameterIndex(KContactBinaryFieldHeaderParam() ) ) ) );	
	CEmbeddedStore* binaryEmbeddedStore = CEmbeddedStore::NewLC(binHeader);
	
	RSqlParamWriteStream textFields;
	User::LeaveIfError(textFields.BindText(stmnt, 
		User::LeaveIfError(stmnt.ParameterIndex(KContactTextFieldsParam() ) ) ) );
	CleanupClosePushL(textFields);
	
	RSqlParamWriteStream binFields;
	User::LeaveIfError(binFields.BindBinary(stmnt, 
		User::LeaveIfError(stmnt.ParameterIndex(KContactBinaryFieldsParam() ) ) ) );
	CEmbeddedStore* binaryEmbeddedBlobStore=CEmbeddedStore::NewLC(binFields);
	
	const CContactTemplate* sysTemplate = NULL;
	if (aItem.Type() != KUidContactTemplate && aItem.Type() != KUidContactGroup)
		{
		// System template is needed unless we are creating a template.
		sysTemplate = &iProperties.SystemTemplateL();
		}

	TCntPersistenceUtility::WriteBlobL(*textEmbeddedStore, textFields, *binaryEmbeddedStore, *binaryEmbeddedBlobStore, aItem, sysTemplate);

	textHeader.CommitL();
	textFields.CommitL();
	binHeader.CommitL();
	binFields.CommitL();

	// execute the statement
	User::LeaveIfError(stmnt.Exec() );

	textHeader.Close();
	binHeader.Close();
	textFields.Close();
	binFields.Close();
	
	CleanupStack::PopAndDestroy(5, &stmnt); //binaryEmbeddedBlobStore, textFields, binaryEmbeddedStore, textEmbeddedStore, stmnt
	}

/**
Gets the variables for the three contact item fields represented by the type flags database field.

@param aTypeFlags the type flags field to be decoded
@param aType the type variable to be got
@param aAttributes the attribute variable to be got
@param aHintFields the hint fields variable to be got
*/
void CPplContactTable::GetTypeFlagFields(TInt aTypeFlags, TUid& aType, TUint& aAttributes, TUint& aHintFields)
	{
	aType 		=  TCntPersistenceUtility::TypeFlagsToContactTypeUid(aTypeFlags); 
	aAttributes = (aTypeFlags & EContactAttributes_Mask) >> EContactAttributes_Shift; 
	aHintFields = TCntPersistenceUtility::TypeFlagsToHint(aTypeFlags);
	}

/**
Creates a TInt of type_flags for insertion into the database.

@return A bitmap representing all three parameter values ready to be inserted 
into the database as a type_flags column value.

@param aType
@param aAttributes
@param aHintFields
*/
TInt CPplContactTable::GenerateTypeFlags(TUid aType, TUint aAttributes, TUint aHintFields)
	{
	TInt tmpVal = TCntPersistenceUtility::ContactTypeUidToTypeFlags(aType);
	tmpVal |= (aAttributes << EContactAttributes_Shift) & EContactAttributes_Mask;
	tmpVal |= aHintFields & EContactHintFlags_Mask;
	return tmpVal;
	}

void CPplContactTable::DeleteL(const CContactItem& aItem, TBool& aLowDiskErrorOccurred)
	{
	CContactItem* item = DeleteLC(aItem.Id(), aLowDiskErrorOccurred);
	CleanupStack::PopAndDestroy(item);
	}

/**
Delete a contact item from the contact table

@param aItemId contact item id
@param aLowDiskErrorOccurred outparameter; will be set to ETrue if an attempt to delete in
		low disk condition occured
*/
CContactItem* CPplContactTable::DeleteLC(TContactItemId  aItemId, TBool& aLowDiskErrorOccurred)
	{
	// You can't delete the system template, because you couldn't read any cards otherwise.
	__ASSERT_ALWAYS(aItemId != KGoldenTemplateId, User::Leave(KErrNotSupported) );

    // the contact assigned to own card is being deleted, so set cached own card id to "not found"
    if (iOwnCardId == aItemId) {
        iOwnCardId = KErrNotFound;
    }

	// select the relevant bits from the contact table for the contact item 
	// and put them in a new contact item
	RSqlStatement selectStmnt;
	CleanupClosePushL(selectStmnt);
	selectStmnt.PrepareL(iDatabase, iDeleteSelectStmnt->SqlStringL() );
	const TInt KContactIdParamIndex(KFirstIndex); // first and only parameter in the query	
	User::LeaveIfError(selectStmnt.BindInt(KContactIdParamIndex, aItemId ) );

	// execute query
	TInt err(selectStmnt.Next() );
	if (err != KSqlAtRow)
		{
		if (err == KSqlAtEnd)
			{
			User::Leave(KErrNotFound);
			}
		User::LeaveIfError(err);
		}
	
	// Obtain the contact type and create a new contact item of this type.
	TInt typeFlags = selectStmnt.ColumnInt(iDeleteSelectStmnt->ParameterIndex(KContactTypeFlags() ) ) ;
	TInt accessCount =  selectStmnt.ColumnInt(iDeleteSelectStmnt->ParameterIndex(KContactAccessCount() ) ) ;
	TContactItemId templateId = selectStmnt.ColumnInt(iDeleteSelectStmnt->ParameterIndex(KContactTemplateId() ) ) ;

	CleanupStack::PopAndDestroy(&selectStmnt);

	TUid type = TCntPersistenceUtility::TypeFlagsToContactTypeUid(typeFlags);
	CContactItem* item = CContactItem::NewLC(type);
	item->SetId(aItemId);
	item->SetAttributes((typeFlags & EContactAttributes_Mask) >> EContactAttributes_Shift); 
	item->SetAccessCount(accessCount);
	item->SetTemplateRefId(templateId);
	
	if (item->IsDeletable() )
		{
		// delete it here
		RSqlStatement deleteStmnt;
		CleanupClosePushL(deleteStmnt);
		deleteStmnt.PrepareL(iDatabase, iDeleteStmnt->SqlStringL() );
		User::LeaveIfError(deleteStmnt.BindInt(KContactIdParamIndex, aItemId) );
		TInt err = deleteStmnt.Exec();
		CleanupStack::PopAndDestroy(&deleteStmnt);

		if (err == KErrDiskFull)
			{
			aLowDiskErrorOccurred = ETrue;
			}
		else
			{
			User::LeaveIfError(err);
			}
		}
	else // Not deletable because of access count > 0.
		{
		if (!item->IsDeleted() )
			{
			// Set and persist the deleted flag.
			item->SetDeleted(ETrue);

			// update attributes in the contact table
			typeFlags |= (item->Attributes()) << EContactAttributes_Shift;
			RSqlStatement updateStmnt;
			CleanupClosePushL(updateStmnt);
			updateStmnt.PrepareL(iDatabase, iUpdateFlagsStmnt->SqlStringL() );
			const TInt KTypeFlagsParamIndex(KFirstParam);				// first parameter in query...
			const TInt KContactIdParamIndex(KTypeFlagsParamIndex + 1);	// ...and the second.
			User::LeaveIfError(updateStmnt.BindInt(KTypeFlagsParamIndex, typeFlags) );
			User::LeaveIfError(updateStmnt.BindInt(KContactIdParamIndex, item->Id()) );
			User::LeaveIfError(updateStmnt.Exec() );
			CleanupStack::PopAndDestroy(&updateStmnt);
			}
		}

	if (!item->IsDeleted() && !aLowDiskErrorOccurred)
		{
		// Decrement Template reference counter.
		UpdateTemplateAccessCounterL(item->TemplateRefId(), EFalse);
		}

	// Ownership is passed to the caller, who can do additional analysis of the item.
	return item;
	}


/**
Create the contact table in the database.
*/
void CPplContactTable::CreateTableL()
	{
	User::LeaveIfError(iDatabase.Exec(KContactCreateStmnt) );
	}


/**
Change the type of the given contact item to the given type.

@param aItemId Contact item ID whose type is to be changed.
@param aNewType New type for contact item.
*/
void CPplContactTable::ChangeTypeL(TContactItemId aItemId, TUid aNewType)
	{
   	if(aItemId == KNullContactId)
   	    {
   	    return;
   	    }
	
	// get the type flags for the contact item
	const TInt bufSize(KSelectFlagsSqlStmntFormat().Size() + NumDigits(aItemId) );
	HBufC* buf = HBufC::NewLC(bufSize);
	TPtr selectTypeFlagsSqlStmnt(buf->Des() );
	selectTypeFlagsSqlStmnt.AppendFormat(KSelectFlagsSqlStmntFormat, aItemId);
	TSqlScalarFullSelectQuery selectTypeFlagsQuery(iDatabase);
	TInt typeFlags = selectTypeFlagsQuery.SelectIntL(selectTypeFlagsSqlStmnt);
	CleanupStack::PopAndDestroy(buf);

	// deconstruct the type flags into the different fields and then rebuild it with the new type
	TUid type;
	TUint attributes;
	TUint hintFields;
	GetTypeFlagFields(typeFlags, type, attributes, hintFields);
	typeFlags = GenerateTypeFlags(aNewType, attributes, hintFields); // use new type

	// update type flags in the contact table
	RSqlStatement updateStmnt;
	CleanupClosePushL(updateStmnt);
	updateStmnt.PrepareL(iDatabase, iUpdateFlagsStmnt->SqlStringL() );
	User::LeaveIfError(updateStmnt.BindInt(updateStmnt.ParameterIndex(KContactTypeFlagsParam() ), typeFlags) );
	User::LeaveIfError(updateStmnt.BindInt(updateStmnt.ParameterIndex(KContactIdParam()), aItemId) );
	User::LeaveIfError(updateStmnt.Exec() );
	CleanupStack::PopAndDestroy(&updateStmnt);
	}

/**
CPplContactTable::THintField constructor.
*/
CPplContactTable::THintField::THintField()
	: iValue(0)
	{
	}


/**
CPplContactTable::THintField constructor.
*/
CPplContactTable::THintField::THintField(TUint16 aExtHint, TUint8 aHint)
	{
	iValue = ((aExtHint << 8) | aHint);
	}


/** 
Update the hint value using the given contact field and custom filterable
fields.

@param aField Contact field.
@param aCustFiltFields Custom filterable fields.
*/
void CPplContactTable::THintField::UpdateHintL(const CContactItemField& aField, const RArray<TUid>& aCustFiltFields)
	{
	const CContentType& type = aField.ContentType();

	if (type.ContainsFieldType(KUidContactFieldVCardMapWORK) )
		{
		iValue |= CContactDatabase::EWork;
		}

	if (type.ContainsFieldType(KUidContactFieldVCardMapHOME) )
		{
		iValue |= CContactDatabase::EHome;
		}

	if (type.ContainsFieldType(KUidContactFieldPhoneNumber) )
		{
		iValue |= CContactDatabase::EPhonable;

		if(type.ContainsFieldType(KUidContactFieldVCardMapCELL) )
			{
			iValue |= CContactDatabase::ESmsable;
			}
		else if(!type.ContainsFieldType(KUidContactFieldVCardMapPAGER) )
			{
			iValue |= CContactDatabase::ELandLine;
			}
		}

	if (type.ContainsFieldType(KUidContactFieldRingTone) )
		{
		CContactFieldStorage* storage = aField.Storage();
		if ( storage && storage->IsFull() )
			{
			iValue |= CContactDatabase::ERingTone;
			}
		}

	if (type.ContainsFieldType( KUidContactsVoiceDialField ) )
		{
		CContactFieldStorage* storage = aField.Storage();
		if (storage && storage->IsFull() )
			{
			iValue |= CContactDatabase::EVoiceDial;
			}
		}

	if(type.ContainsFieldType(KUidContactFieldFax) )
		{
		iValue |= CContactDatabase::EFaxable;
		iValue |= CContactDatabase::EPhonable;
		}

	if(type.ContainsFieldType(KUidContactFieldIMAddress) )
		{
		CContactFieldStorage* storage = aField.Storage();
		if(storage && storage->IsFull() )
			{
			iValue |= CContactDatabase::EIMAddress;
			if(type.ContainsFieldType(KUidContactFieldVCardMapWV) )
				{
				iValue |= CContactDatabase::EWirelessVillage;
				}
			}
		}

	if (type.ContainsFieldType(KUidContactFieldEMail) )
		{
		iValue |= CContactDatabase::EMailable;
		}

	// Now check if given field type maps to one of the given (licensee) custom
	// filterable fields.
	
	TInt index(KErrNotFound);

	const TInt KCount = aCustFiltFields.Count();
	for (TInt i = 0; i < KCount; ++i)
		{
		if (type.ContainsFieldType(aCustFiltFields[i]) )
			{
			index = i;
			break;
			}
		}

	if (index != KErrNotFound)
		{
		switch (index)
			{
			case 0:
				iValue |= CContactDatabase::ECustomFilter1;
				break;
			case 1:
				iValue |= CContactDatabase::ECustomFilter2;
				break;
			case 2:
				iValue |= CContactDatabase::ECustomFilter3;
				break;
			case 3:
				iValue |= CContactDatabase::ECustomFilter4;
				break;
			default:
				__ASSERT_DEBUG(EFalse, User::Leave(KErrNotSupported) );
				break;
			}
		}
	}


/**
Get the hint extension value (upper 8 bits).

@return Hint extension value.
*/
TUint16 CPplContactTable::THintField::ExtHint()
	{
	return static_cast<TUint16>((iValue >> 8) & KMaxTUint16);
	}


/**
Get the non-extended hint value (lower 8 bits).

@return Non-extended hint value.
*/
TInt8 CPplContactTable::THintField::Hint()
	{
	return static_cast<TInt8>(iValue & KMaxTUint8);
	}



/**
Return the Identity field uid for the given contact field.

@param Contact field.

@return Identity field index corresponding to the given contact field.
*/
TInt CPplContactTable::NameFieldUid(const CContactItemField& nameField)
	{
	// For all the name fields stored in the Identity table...
	for (TInt nameFieldNum = 0; nameFieldNum < sizeof(KFastAccessFieldUids) / sizeof(TInt); ++nameFieldNum)
		{
		if (nameField.ContentType().ContainsFieldType(TUid::Uid(KFastAccessFieldUids[nameFieldNum]) ) )
			{
			return KFastAccessFieldUids[nameFieldNum];
			}
		}
	return KErrNotFound;
	}

/**
  Utility function to calculate the number of digits in an integer.
  If a negative number, counts the - sign as an extra digit.
  
  @param aNum an integer
  
  @return TUint the number of digits
*/
TUint CPplContactTable::NumDigits(TInt aNum)
	{
  	TInt numDig(1);
  	if (aNum < 0)
  		{
  		++numDig;
  		aNum = -aNum;
  		}
  	while( (aNum /= 10) > 0)
  		{
  		++numDig;
  		}
  	return numDig;
  	}
  	
/**	
Utility method used to check if contact table is empty or not

@return ETrue is the contact table is empty and EFalse otherwise
*/
TBool CPplContactTable::IsTableEmptyL()
	{
	TInt count = 0;

	HBufC* selectString = HBufC::NewLC(KCountSelect().Length() + 
						KSqlContactTableName().Length()); 
	TPtr ptrSelectString = selectString->Des();
	ptrSelectString.Format(KCountSelect, &KSqlContactTableName);

	TSqlScalarFullSelectQuery scalarQuery(iDatabase);
	
	count = scalarQuery.SelectIntL(ptrSelectString);
	CleanupStack::PopAndDestroy(selectString);

	return (count == 0);	
	}

/**
Utility method used to retrieve card template ids
*/
CContactIdArray& CPplContactTable::CardTemplateIdsL()
	{
	iCardTemplateIds->Reset();
	
	HBufC* selectString = HBufC::NewLC(KTwoTypeField().Length() + KContactId().Length() + KContactTypeFlags().Length() + 
						KSqlContactTableName().Length() + KContactTypeFlags().Length() + 3); 
	TPtr ptrSelectString = selectString->Des();
	ptrSelectString.Format(KTwoTypeField, &KContactId, &KContactTypeFlags, &KSqlContactTableName, &KContactTypeFlags, EContactType_Shift, EContactTypeFlags_CardTemplate);
	
	RSqlStatement selectStmnt;
	CleanupClosePushL(selectStmnt);
	selectStmnt.PrepareL(iDatabase, ptrSelectString );
	
	const TInt KIdIndex = selectStmnt.ColumnIndex(KContactId);
	const TInt KTypeFlagsIndex = selectStmnt.ColumnIndex(KContactTypeFlags);
	
	TInt err;
	while((err = selectStmnt.Next()) == KSqlAtRow)
		{
		TInt typeFlags = selectStmnt.ColumnInt(KTypeFlagsIndex);
		TInt attr = (typeFlags & EContactAttributes_Mask) >> EContactAttributes_Shift; 
		if((attr & EContactAttrsFlags_Deleted) == 0)
		    {
		    //Only add templates are not marked as deleted.
    		iCardTemplateIds->AddL(selectStmnt.ColumnInt(KIdIndex));	
		    }
		}
	
	if (err != KSqlAtEnd)
		{
		User::Leave(err);
		}

	CleanupStack::PopAndDestroy(2,selectString);
	
	return *iCardTemplateIds;
	}

/**
Utility method used to retrieve own card id
*/
TContactItemId CPplContactTable::OwnCardIdL()
	{
	if (iOwnCardId != 0)
	    {
    	RDebug::Print(_L("CPplContactTable::OwnCardIdL() exit %d"), iOwnCardId);
	    return iOwnCardId;
	    }
	    
	HBufC* selectString = HBufC::NewLC(KOneTypeField().Length() + KContactId().Length() + 
						KSqlContactTableName().Length() + KContactTypeFlags().Length() + 3); 
	TPtr ptrSelectString = selectString->Des();
	ptrSelectString.Format(KOneTypeField, &KContactId, &KSqlContactTableName, &KContactTypeFlags, EContactType_Shift, EContactTypeFlags_OwnCard);
	
	RSqlStatement selectStmnt;
	CleanupClosePushL(selectStmnt);
	selectStmnt.PrepareL(iDatabase, ptrSelectString );
	
	const TInt idIndex = selectStmnt.ColumnIndex(KContactId);
	
	TInt err;
	if((err = selectStmnt.Next()) == KSqlAtRow)
		{
		iOwnCardId = selectStmnt.ColumnInt(idIndex);	
		}
	else
		{
		User::LeaveIfError(err);
		iOwnCardId = KErrNotFound;
		}
	
	CleanupStack::PopAndDestroy(2, selectString);
	
	return iOwnCardId;
	}
	
/**
Utility method used to set own card id
*/	
void CPplContactTable::SetOwnCardIdL(TContactItemId aId, TBool aPersist)
	{
	if (iOwnCardId != aId)
	    {
	    if (aPersist)
	        {
	        //Change old own card to be contact card.
            ChangeTypeL(iOwnCardId, KUidContactCard);
			iOwnCardId = KErrNotFound;
        
	        //set given card as own card.
    	    ChangeTypeL(aId, KUidContactOwnCard);
    	    }

	    //update cached own card id
    	iOwnCardId = aId;
	    }
	}
