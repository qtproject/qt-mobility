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
#include "cntpplviewmanager.h"
#include "dbsqlconstants.h"
#include "cntpersistenceutility.h"
#include <cntfldst.h>

const TInt KCachedPrepareTimeOut = 1000000; //1 second

/**
Object factory method.
*/
CCntPplViewSession* CCntPplViewSession::NewL(CPplContactsFile& aContactsFile, const CLplContactProperties& aContactProperties, CCntSqlStatement& aSelectAllFields, TInt aViewId, const CContactTextDef& aTextDef, const TContactViewPreferences aViewPrefs)
	{
	CCntPplViewSession* viewSession = new (ELeave) CCntPplViewSession(aContactsFile, aContactProperties, aSelectAllFields, aViewId, aViewPrefs);
	CleanupStack::PushL(viewSession);
	viewSession->ConstructL(aTextDef);
	CleanupStack::Pop(viewSession);
	return viewSession;
	}

	
/**
CCntPplViewSession first phase constructor.
*/
CCntPplViewSession::CCntPplViewSession(CPplContactsFile& aContactsFile, const CLplContactProperties& aContactProperties, CCntSqlStatement& aSelectAllFields, TInt aViewId, TContactViewPreferences aViewPrefs)
:  	CTimer(CActive::EPriorityIdle),
    iViewId(aViewId),
	iContactProperties(aContactProperties),
    iContactsFile(aContactsFile),
	iSqlSmtSelectAllFieldsById(aSelectAllFields),
	iViewPrefs(aViewPrefs)
	{
	CActiveScheduler::Add(this);
	}

	
/**
CCntPplViewSession destructor.
*/
CCntPplViewSession::~CCntPplViewSession()
	{
	Deque();
	
	if(iRSqlStatement)
		{
		iRSqlStatement->Close();
		delete iRSqlStatement;
		}
	
	if(iCachedSqlStatement)
		{
		iCachedSqlStatement->Close();
		delete iCachedSqlStatement;
		}
	
	delete iCntSqlStatement;
	delete iTextDef;
	}

	
/**
CCntPplViewSession second phase constructor.
*/
void CCntPplViewSession::ConstructL(const CContactTextDef& aTextDef)
	{
	//Constructing iCntSqlStatement, iTextDef, and iIsFastAccessFieldsOnly
	//by simply call ChangeSortOrderL
	CTimer::ConstructL();
	ChangeSortOrderL(aTextDef);
	}	

	
/**
Change the sorting order/text definition. It should be always called when 
there is not an iteration started in persistence layer.

@param aTextDef the new text definition to be used in the view session.
*/
void CCntPplViewSession::ChangeSortOrderL(const CContactTextDef& aTextDef)
	{
	//Cleanup the cached Prepare statement as the sort order will be changed
	Cancel();
    CleanupCachedPrepareStatement();
	
	//Copy the text definition
	CContactTextDef* textDef = CContactTextDef::NewLC();
	const TInt KTextDefCount = aTextDef.Count();
	for (TInt index = 0; index < KTextDefCount; ++index)
		{
		textDef->AppendL(TContactTextDefItem(aTextDef.At(index).iFieldType));
		}
	
	// Create select statement on contact table
	TCntSqlStatementType statementType(ESelect, KSqlContactTableName());
	CCntSqlStatement* sqlSmt = TSqlProvider::GetSqlStatementL(statementType);
	CleanupStack::PushL(sqlSmt);
	
	// Always select id, type flags.	
	sqlSmt->SetParamL(KContactId(), KSpace());
	sqlSmt->SetParamL(KContactTypeFlags(), KSpace());
	
	//Go through text definition to construct select statement.
	TBool isFastAccessOnly = ETrue;
	for(TInt ii = 0; ii < KTextDefCount; ++ii)
		{
		const TDesC& KColunmName = TCntPersistenceUtility::GetFastAccessColumnNameById(aTextDef.At(ii).iFieldType.iUid);
		if(KColunmName.Length() > 0) 
			{
			sqlSmt->SetParamL(KColunmName, KSpace());
			}
		else
			{
			isFastAccessOnly = EFalse;		
			}
		}
	
	if(!isFastAccessOnly)
		{
		//Fields in text blob are needed, add text fields header and
        //text blob columns in the select statement.		
		sqlSmt->SetParamL(KContactTextFieldHeader(), KSpace());
		sqlSmt->SetParamL(KContactTextFields(), KSpace());
		}

	CleanupStack::Pop(2, textDef); // sqlSmt, textDef.
	
	delete iCntSqlStatement;
	iCntSqlStatement = sqlSmt;
	
	delete iTextDef;
	iTextDef = textDef;
	
	iIsFastAccessFieldsOnly = isFastAccessOnly;
	}
	

/**
Preparing sql statement for the coming iteration requests from upper layer.
Caller has the responsibilty of controling the lifecycle of the iteration 
session -- when it starts and stop.  
*/
void CCntPplViewSession::BeginIterateL()
	{
	if (! iContactsFile.IsOpened())
		{
		User::Leave(KErrInUse);
		}
	
	if(iRSqlStatement)
		{
		iRSqlStatement->Close();
		delete iRSqlStatement;
		iRSqlStatement = NULL;
		}
	
	//Create a new sqlstatment to start a iteration	
	iRSqlStatement = new (ELeave) RSqlStatement();
	
	// we don't need condition for iterating database.
	iCntSqlStatement->ClearCondition();
	
	iRSqlStatement->PrepareL(iContactsFile.NamedDatabase(), iCntSqlStatement->SqlStringL());
	}

	
/**
Stop iteration by releasing iteration sql statement.
*/
void CCntPplViewSession::EndIterate()
	{
	if(iRSqlStatement)
		{
		iRSqlStatement->Close();
		delete iRSqlStatement;
		iRSqlStatement = NULL;
		}
	}

	
/**
Get next view contact object from iterating sql statement.

@return CViewContact object or NULL if iteration reachs end of rows.
*/
CViewContact* CCntPplViewSession::NextItemL(TContactViewPreferences aViewPrefs)
	{
	if(!iRSqlStatement)
		{
		//Iteration has not started
		BeginIterateL();	
		}
	
	return CreateViewItemL(*iRSqlStatement, *iCntSqlStatement, aViewPrefs);
	}

	
/**
Get view contact object based on given contact id.

@return CViewContact object or NULL if there is not such a contact.
*/
CViewContact* CCntPplViewSession::ItemAtL(TContactItemId aContactId)
	{
	//Cancel the timer so that the cached Prepare statement would not be deleted.
	Cancel();
	
	CViewContact* viewContact = NULL;
	
	TRAPD(err, viewContact = doItemAtL(aContactId));
    if(err != KErrNone)
        {
        CleanupCachedPrepareStatement();
    	User::Leave(err);	
        }
    	
	//Reset the timer
	After(KCachedPrepareTimeOut);
	return viewContact;
	}


/**
The method to do the real work of get view contact object based on given contact id.

@return CViewContact object or NULL if there is not such a contact.
*/
CViewContact* CCntPplViewSession::doItemAtL(TContactItemId aContactId)
    {
    if (! iContactsFile.IsOpened())
    	{
    	User::Leave(KErrInUse);
    	}
    
	if(!iCachedSqlStatement)
		{
		//iCacheSqlStatement is just for ItemAt while iRSqlStatement is used for iteration 
		//and maintained by BeginIterateL and EndIterateL.
    	iCntSqlStatement->SetConditionL(KSelectCondition_SearchForContactId()); 
		
		//Create a new sqlstatment and cache its Prepare statement.	
		iCachedSqlStatement = new (ELeave) RSqlStatement();
		iCachedSqlStatement->PrepareL(iContactsFile.NamedDatabase(),  iCntSqlStatement->SqlStringL());
		}
		
	User::LeaveIfError(iCachedSqlStatement->BindInt(KFirstIndex, aContactId)); //Bind item id into the condition.	

	CViewContact* viewContact = CreateViewItemL(*iCachedSqlStatement, *iCntSqlStatement, iViewPrefs);
	
	// Adding this to the cleanup stack, because coverity recons that "iCachedSqlStatement->Reset();" can leave, and it's probably right
	CleanupStack::PushL(viewContact);
	iCachedSqlStatement->Reset();

	// Pop it back off the cleanup stack, and hand over ownership
	CleanupStack::Pop(viewContact);
	return viewContact;
    }


/**
Clean up the cached prepare statement.
*/
void CCntPplViewSession::CleanupCachedPrepareStatement()
    {
	if(iCachedSqlStatement)
		{
		iCachedSqlStatement->Close();
		delete iCachedSqlStatement;
		iCachedSqlStatement = NULL;
		}
    }

/**
CTimer callback funtion, the cached Prepare statement would be de-cached if ItemAtL is not called
during the time interval defined by KCachedPrepareTimeOut.
*/
void CCntPplViewSession::RunL()
	{
	CleanupCachedPrepareStatement();
	}
	
	
/**
Create view contact object based on given sql statement.

@return CViewContact object or NULL the contact is not found.
*/
CViewContact* CCntPplViewSession::CreateViewItemL(RSqlStatement& aSqlStmt, const CCntSqlStatement& aCntSqlStmt, const TContactViewPreferences& aViewPrefs)
	{
	if (! iContactsFile.IsOpened())
		{
		User::Leave(KErrInUse);
		}
	
	CViewContact* viewContact = InitialiseViewItemL(aSqlStmt, aCntSqlStmt, aViewPrefs);
	if(!viewContact)
		{
		//Couldn't find the contact.
		return NULL;
		}
	
	CleanupStack::PushL(viewContact);
	if(viewContact->ContactType() == CViewContact::EContactItem)
		{
		FillViewItemL(*viewContact, aSqlStmt, aViewPrefs);
		}
	
	TUid typeUid = GetContactFieldMatchUid(*viewContact, aViewPrefs);	
	if(typeUid != KUidContactFieldNone)
		{
		RSqlStatement contactSqlStmt;
		CleanupClosePushL(contactSqlStmt);
		
		/* we have to reload the row from SQL database to cache in text header and text blob */ 	
		contactSqlStmt.PrepareL(iContactsFile.NamedDatabase(),  iSqlSmtSelectAllFieldsById.SqlStringL());
		User::LeaveIfError(contactSqlStmt.BindInt(KFirstIndex, viewContact->Id())); //Bind item id into the condition.	
		
		TInt err = contactSqlStmt.Next();
		if(err == KSqlAtEnd)
			{
			//Should never be here, we found it in InitialiseViewItemL 
			//but couldn't find the same contact in same database.
			CleanupStack::PopAndDestroy(viewContact);
			return NULL;
			}			
			
		User::LeaveIfError(err);
		
		/* set first field with possible content for group or unsorted contact */	
		CContactDatabase::TTextFieldMinimal buf;
		TextFieldL(contactSqlStmt, iSqlSmtSelectAllFieldsById, iContactProperties.SystemTemplateL(), typeUid, buf);
		viewContact->SetFirstFieldForBlankContactL(buf);
		CleanupStack::PopAndDestroy(&contactSqlStmt);
		} //if(typeUid != 0)
	
	CleanupStack::Pop(viewContact);
	return viewContact;
	}

	
/**
Filling content for the given view contact object.

@param aViewContact reference to the view contact object to be filled
@param aSqlStmt the sql statement which contains the retrieved content for the view object. 
*/
void CCntPplViewSession::FillViewItemL(CViewContact& aViewContact, RSqlStatement& aSqlStmt, const TContactViewPreferences& aViewPrefs)
	{
	if(iIsFastAccessFieldsOnly)
		{
		//The view gets fields only from fast access columns	
		const TInt KTextDefCount = iTextDef->Count();
		for(TInt index = 0; index < KTextDefCount; ++index)
			{
			const TDesC& KColumnName = TCntPersistenceUtility::GetFastAccessColumnNameById(iTextDef->At(index).iFieldType.iUid);
			ASSERT(KColumnName.Length() > 0);
			
			TPtrC fieldPtrC = aSqlStmt.ColumnTextL(iCntSqlStatement->ParameterIndex(KColumnName));
   			AddFieldInViewContactL(aViewContact, fieldPtrC, aViewPrefs);
			} //for
		}
	else
		{
		TBool searchFastAccessFields = EFalse;
		
		// iTextDef contains the fields that should be included in the view.
		// The array of all the field objects in a contact item is returned from
		// the Contacts table.  
		RPointerArray<CContactItemField> fields;
		CleanupStack::PushL(TCleanupItem(TCntPersistenceUtility::ResetAndDestroyRPointerArray, &fields));
		
		TPtrC8 textHeader;
		aSqlStmt.ColumnBinary(iCntSqlStatement->ParameterIndex(KContactTextFieldHeader()), textHeader);
		RDesReadStream textHeaderStream(textHeader);
		CleanupClosePushL(textHeaderStream);
        CEmbeddedStore* textHeaderStore = CEmbeddedStore::FromLC(textHeaderStream);
        
    	RStoreReadStream textHeaderStoreStream;
    	textHeaderStoreStream.OpenLC(*textHeaderStore,textHeaderStore->Root());
        
		TPtrC textFieldPtrC = aSqlStmt.ColumnTextL(iCntSqlStatement->ParameterIndex(KContactTextFields()));
		HBufC* textFieldsBuf = textFieldPtrC.AllocLC();
			
		TCntPersistenceUtility::ReadTextBlobL(textHeaderStoreStream, textFieldsBuf, *iTextDef, iContactProperties.SystemTemplateL(), fields, searchFastAccessFields);
		CleanupStack::PopAndDestroy(4, &textHeaderStream); //textHeaderStore, textHeaderStream, textHeaderStoreStream, textFieldsBuf
		
		// Loop through fields, checking for fields from fast access fields, and add 
		// the fields to the view contact object.
		const TInt KFieldsNumMax = fields.Count();
		for(TInt fieldsIndex = 0; fieldsIndex < KFieldsNumMax; ++fieldsIndex)
			{
			// this doesn't own the instance stored in fields array.
			CContactItemField* itemField = fields[fieldsIndex];

			if(itemField == NULL)
				{
				aViewContact.AddFieldL(KNullDesC);
				continue;
				}

			// The array of fields retrieved from the text fields blob does not
			// contain the text data Fast Access fields. If the searchFastAccessFields 
			// flags have been set then get the data from Fast Access columns
			// before adding the data to the view.
			TPtrC fieldText;
			if(searchFastAccessFields && itemField->ContentType().FieldTypeCount() > 0)
				{
				//Check the field name of the first field type(key field type) 
				//in the field's field types array
    			const TDesC& KColumnName = TCntPersistenceUtility::GetFastAccessColumnNameById(itemField->ContentType().FieldType(0).iUid);
				if(KColumnName.Length() > 0)
					{
					//this is a fast access field.
					fieldText.Set(aSqlStmt.ColumnTextL(iCntSqlStatement->ParameterIndex(KColumnName)));
					}
				else
					{
					fieldText.Set(itemField->TextStorage()->Text());
					}					
				}
			else
				{
				fieldText.Set(itemField->TextStorage()->Text());
				}
			
			AddFieldInViewContactL(aViewContact, fieldText, aViewPrefs);
			}
		
		CleanupStack::PopAndDestroy(&fields);
		}
	}

	
/**
Initialise requested view contact object based on given parameters:
 1. search the contact in contact table.
 2. fill in contact type and hint to the view contact object.

@param aSqlStmt      reference to a RSqlStatement to be read in data 
@return initialized CViewContact object or NULL if reached end of rows.
*/
CViewContact* CCntPplViewSession::InitialiseViewItemL(RSqlStatement& aSqlStmt, const CCntSqlStatement& aCntSqlStmt, const TContactViewPreferences& aViewPrefs)
	{
	CViewContact* viewContact = NULL;
	
    while(viewContact == NULL)
	    {
    	TInt err = aSqlStmt.Next();
    	 
    	if(err == KSqlAtEnd)
    	    {
    	    return NULL;
    	    }
    	    
       	User::LeaveIfError(err);
       	
    	TInt typeFlags = aSqlStmt.ColumnInt(aCntSqlStmt.ParameterIndex(KContactTypeFlags()));
    	TUid contactTypeUid = TCntPersistenceUtility::TypeFlagsToContactTypeUid(typeFlags);
    	
        if(!ContactCorrectType(contactTypeUid, aViewPrefs))
            {
            //The contact type doesn't match the view prefreference
            continue;
            }
    	
    	viewContact = CViewContact::NewLC(KNullContactId);
    	
    	/* Got a row and add common fields for both Groups and Contact view items. */
    	viewContact->SetId(aSqlStmt.ColumnInt(aCntSqlStmt.ParameterIndex(KContactId())));
    	viewContact->SetContactHint(TCntPersistenceUtility::TypeFlagsToHint(typeFlags));
    	viewContact->SetContactTypeUid(contactTypeUid);
    	
    	
    	if(contactTypeUid == KUidContactGroup)
    		{
    		// Groups don't require all the field types to be set.
    		viewContact->SetContactType(CViewContact::EGroup);
    		}
    	else
    		{
    		// All other contact types do require field types to be set.
    		viewContact->SetContactType(CViewContact::EContactItem);
    		}

    	CleanupStack::Pop(viewContact);		
	    }
   	
	return viewContact;    
	}	

	
/**
Check if the retrieved view contact is group or unsorted contact, if so return suitable field 
match uid in order to fill in a possible text for the contact: 
For group contact    -- find template label.
For unsorted contact -- find first not none field.

@param aViewContact     reference to the retrieved view contact object.
@param aViewPreferences the view preferences made by client.
 
@return suitable match uid or KUidContactFieldNone if it's not group or unsorted contact.
*/
TUid CCntPplViewSession::GetContactFieldMatchUid(const CViewContact& aViewContact, const TContactViewPreferences& aViewPreferences)	
	{
	TUid typeUid = KUidContactFieldNone;
	if(aViewContact.ContactType() == CViewContact::EGroup && (aViewPreferences & (EGroupsOnly | EContactAndGroups)))
		{
		// Check for group view contact type.  This type of view contact uses the
		// template label field to populate the first view contact field.
		typeUid = KUidContactFieldTemplateLabel;
		}
	else if(!aViewContact.IsSortable() && (aViewPreferences & (EIgnoreUnSorted | EUnSortedAtBeginning | EUnSortedAtEnd)) == 0)		
		{
		// None of the sort fields contained any data.  We want this contact to
		// be sorted by the first bit of text we find so we need to retrieve the
		// first text available from the contact and use this to populate the
		// first view contact field.
		typeUid = KUidContactFieldMatchAll;
		}
	return typeUid;	
	}

	
/**
Refine the field content and add the refined content to the given view contact object.
 
@param aViewContact     reference to the retrieved view contact object.
@param aFieldPtrC    	The actual field content read from database.
@param aViewPreferences The view preferences for how to format the content. 
*/
void CCntPplViewSession::AddFieldInViewContactL(CViewContact& aViewContact, TPtrC& aFieldPtrC, const TContactViewPreferences& aViewPreferences)
	{
	_LIT(KBlank," ");
	
	// Truncate to 64 if necessary.
	TPtr ptr(const_cast<TUint16*>(aFieldPtrC.Ptr()), KTextFieldMinimalLength);
	if(aFieldPtrC.Length() > KTextFieldMinimalLength) 
		{
		ptr.SetLength(KTextFieldMinimalLength); 
		}
	else
		{
		ptr.SetLength(aFieldPtrC.Length());
		}
		
	// Add the field to the view.
	if((ptr.Length() == 0) ||
	   (ptr.Length() == 1 && (aViewPreferences & ESingleWhiteSpaceIsEmptyField) && ptr.Compare(KBlank)==KErrNone))
		{
		/* empty field */
		aViewContact.AddFieldL(KNullDesC);
		} 
	else
		{
		aViewContact.AddFieldL(ptr);
		}
	}

/** 
Retrieve the field text for the given field type and contact item ID.

The behaviour differs when a specific field type is not given i.e. when
aFieldType is KUidContactFieldMatchAll:

- First tries to find an email for the given contact item ID.
- If there is no email then it retrieves the first entry in Fast Access fields
for the given contact item ID.
- If there is no Fast Access fields then it retrieves the first entry in the
text fields blob for the given contact item ID.

Text for all other field types are retrieved from the text fields blob.

The caller must determine that the given contact item ID exists before calling
this method.
*/
void CCntPplViewSession::TextFieldL(RSqlStatement& aSqlStatement, const CCntSqlStatement& aCntSqlStmt, const CContactTemplate& aSystemTemplate, TFieldType aFieldType, TDes& aText)
	{
	TPtrC8 textHeader;
	aSqlStatement.ColumnBinary(aCntSqlStmt.ParameterIndex(KContactTextFieldHeader()), textHeader);
	RDesReadStream textHeaderStream(textHeader);
	CleanupClosePushL(textHeaderStream);	
    CEmbeddedStore* textHeaderStore = CEmbeddedStore::FromLC(textHeaderStream);
    
	RStoreReadStream textHeaderStoreStream;
   	textHeaderStoreStream.OpenLC(*textHeaderStore,textHeaderStore->Root());

	TPtrC textFieldPtrC = aSqlStatement.ColumnTextL(aCntSqlStmt.ParameterIndex(KContactTextFields()));
	HBufC* textFieldsBuf = textFieldPtrC.AllocLC();
	
	if(aFieldType == KUidContactFieldMatchAll)
		{
		if (TCntPersistenceUtility::FindTxtFieldInTextBlobL(textHeaderStoreStream, textFieldsBuf, aSystemTemplate, aFieldType, aText) == EFalse)
			{
			CContactDatabase::TTextFieldMinimal	fastAccessText;
			if (HasTxtFieldInFastAccessFieldsL(aSqlStatement, aCntSqlStmt, fastAccessText))
				{
				aText.Copy(fastAccessText);		
				}
			}
		} 
	else
		{
		if (SpecificTxtFieldInFastAccessFieldsL(aSqlStatement, aCntSqlStmt, aFieldType, aText) == EFalse)
			{
			TCntPersistenceUtility::FindTxtFieldInTextBlobL(textHeaderStoreStream, textFieldsBuf, aSystemTemplate, aFieldType, aText);
			}
		}
		
	CleanupStack::PopAndDestroy(4, &textHeaderStream); //textHeaderStore, textHeaderStream, textHeaderStoreStream, textFieldsBuf
	}


/**
The first text field in the Fast Access fields is found and returned via the
parameter aText.

@param aCntItemId The contact ID for which the first text field is required.
@param aText On return contains the text for the field of the first Identity
text field.

@return ETrue if a Fast Access text field was found, EFalse otherwise.
*/
TBool CCntPplViewSession::HasTxtFieldInFastAccessFieldsL(RSqlStatement& aSelectStmt, const CCntSqlStatement& aCntSqlStmt, TDes& aText)
	{
	for(TInt ii = EGivenName; ii <= ECompanyNamePrn; ++ii)
		{
		const TDesC& KColumnName = TCntPersistenceUtility::GetFastAccessColumnNameById(KFastAccessFieldUids[ii]);
		TPtrC identText = aSelectStmt.ColumnTextL(aCntSqlStmt.ParameterIndex(KColumnName));
		if(identText.Length() > 0)
			{
			TCntPersistenceUtility::CopyMinFieldText(identText, aText);
			return ETrue;
			}
		}
	return EFalse;
	}

	
/**
Checks if the given field type is in the Fast Access fields and if so the field text
is returned via the parameter aText.

@param aCntItemId The contact ID for which the specific text field is required.
@param aText On return contains the field text for the first Fast Access fields
with field type aFieldType.
@param aFieldType The specific field requested.

@return ETrue if the specific fast access text field was found, EFalse otherwise.
*/
TBool CCntPplViewSession::SpecificTxtFieldInFastAccessFieldsL(RSqlStatement& aSelectStmt, const CCntSqlStatement& aCntSqlStmt, const TFieldType aFieldType, TDes& aText)
	{
    const TDesC& KColumnName = TCntPersistenceUtility::GetFastAccessColumnNameById(aFieldType.iUid);
	if(KColumnName.Length() > 0)
		{
		// This is a fast access field.
		TPtrC fastAccessText = aSelectStmt.ColumnTextL(aCntSqlStmt.ParameterIndex(KColumnName));
		TCntPersistenceUtility::CopyMinFieldText(fastAccessText, aText);
		return ETrue;
		}
	return EFalse;
	}

	
/**
Get view session id

@return the view session id assigned to the session.
*/
TInt CCntPplViewSession::ViewId() const
	{
	return iViewId;
	}


TBool CCntPplViewSession::ContactCorrectType(TUid aContactTypeUid, TContactViewPreferences aTypeToInclude)
	{
	TBool correctType(EFalse);
	if (aContactTypeUid==KUidContactCard || aContactTypeUid==KUidContactOwnCard)
		{
		// Ignore Unsorted Contacts flags & White Space flag
		// catch non- contact views
		// Should be EContactsOnly, EContactAndGroups & EICCEntriesAndContacts
		if (0 == ((aTypeToInclude & ~(ESingleWhiteSpaceIsEmptyField | EIgnoreUnSorted | EUnSortedAtBeginning | EUnSortedAtEnd))
				& (EGroupsOnly | EICCEntriesOnly))) // Ignore 'UnSorted' flags, exclude Groups Only & ICC Only 
			{
			correctType = ETrue;
			}
		}
	else if (aContactTypeUid == KUidContactGroup)
		{
		if (aTypeToInclude & (EGroupsOnly | EContactAndGroups))
			{
			correctType = ETrue;
			}
		}
	else if (aContactTypeUid == KUidContactICCEntry)
		{
		if (aTypeToInclude & (EICCEntriesOnly | EICCEntriesAndContacts))
			{
			correctType = ETrue;
			}
		}
	return correctType;
	}

	
