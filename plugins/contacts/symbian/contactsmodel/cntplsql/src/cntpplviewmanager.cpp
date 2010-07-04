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
#include "cntsqlprovider.h"
#include "dbsqlconstants.h"
#include "cntpersistenceutility.h"

const TInt KViewSessionsGranularity = 4;


/**
Object factory method.
*/
CCntPplViewManager* CCntPplViewManager::NewL(CPplContactsFile& aContactsFile, const CLplContactProperties& aContactProperties)
	{
	CCntPplViewManager* manager = new (ELeave) CCntPplViewManager(aContactsFile, aContactProperties);
	CleanupStack::PushL(manager);
	manager->ConstructL();
	CleanupStack::Pop(manager);
	return manager;
	}

	
/**
CCntPplViewManager first phase constructor.
*/
CCntPplViewManager::CCntPplViewManager(CPplContactsFile& aContactsFile, const CLplContactProperties& aContactProperties)
:  	iContactsFile(aContactsFile),
	iContactProperties(aContactProperties),
	iNextViewId(KPLViewSessionIdNull + 1),
	iViewSessions(KViewSessionsGranularity)
	{
	}

	
/**
CCntPplViewManager destructor.
*/
CCntPplViewManager::~CCntPplViewManager()
	{
	iViewSessions.ResetAndDestroy();
	delete iSelectFullFieldsStmt;
	}


/**
CCntPplViewManager second phase constructor.
*/
void CCntPplViewManager::ConstructL()
	{
	/* 
	Construct select statement:
	"SELECT contact_id, type_flags, first_name, last_name, company_name, firstname_prn, 
			lastname_prn, companyname_prn, text_fields_header, text_fields 
     FROM contact WHERE contact_id == :SeekId"
	*/
	TCntSqlStatementType statementType(ESelect, KSqlContactTableName);
	iSelectFullFieldsStmt = TSqlProvider::GetSqlStatementL(statementType);

	// To select id, type flags, and all fast access fields.	
	iSelectFullFieldsStmt->SetParamL(KContactId(), KSpace());
	iSelectFullFieldsStmt->SetParamL(KContactTypeFlags(), KSpace());
	iSelectFullFieldsStmt->SetParamL(KContactFirstName(), KSpace());
	iSelectFullFieldsStmt->SetParamL(KContactLastName(), KSpace());
	iSelectFullFieldsStmt->SetParamL(KContactCompanyName(), KSpace());
	iSelectFullFieldsStmt->SetParamL(KContactFirstNamePrn(), KSpace());
	iSelectFullFieldsStmt->SetParamL(KContactLastNamePrn(), KSpace());
	iSelectFullFieldsStmt->SetParamL(KContactCompanyNamePrn(), KSpace());
	
	// Also select text header, and text blob
	iSelectFullFieldsStmt->SetParamL(KContactTextFieldHeader(), KSpace());
	iSelectFullFieldsStmt->SetParamL(KContactTextFields(), KSpace());
	
	// Add selecting condition
	HBufC* condition = HBufC::NewLC(KSelectCondition_SearchForEqual().Length() + KContactId().Length());
	TPtr ptrCondition = condition->Des();
	ptrCondition.Format(KSelectCondition_SearchForEqual(), &KContactId);
		
	iSelectFullFieldsStmt->SetConditionL(ptrCondition);
	
	CleanupStack::PopAndDestroy(condition);
	}

	
/**
Create a new view session which is mapping to a local view.

@param aTextDef the text definition for the view
@param aViewPrefs the view preferences for the view.
@return the view session id created for the local view.
*/
TInt CCntPplViewManager::OpenViewL(const CContactTextDef& aTextDef, const TContactViewPreferences aViewPrefs)
	{
	CCntPplViewSession* viewSession = CCntPplViewSession::NewL(iContactsFile, 
			iContactProperties, *iSelectFullFieldsStmt, iNextViewId, aTextDef, aViewPrefs);	
	CleanupStack::PushL(viewSession);
	iViewSessions.AppendL(viewSession);
	CleanupStack::Pop(viewSession);

	//there is no relationship between the iViewSessions index (subscript) and iNextViewId
	return iNextViewId++;
	}

	
/**
Close a view session.

@param aViewId the id of the view session to be closed
*/
void CCntPplViewManager::CloseView(TInt aViewId)
	{
	TInt index = FindViewSessionIndexById(aViewId);
	if(index != KErrNotFound)
		{
		CCntPplViewSession* viewSession = iViewSessions[index];
		iViewSessions.Remove(index);
		delete viewSession;	
		}
	}


/**
Change a sort order for a view session.

@param aViewId the id of the view session
@param aTextDef the new sort order/text definition.
*/
void CCntPplViewManager::ChangeSortOrderL(TInt aViewId, const CContactTextDef& aTextDef)
	{
	FindViewSessionByIdL(aViewId)->ChangeSortOrderL(aTextDef);
	}
	
	
/**
Start iteration in a view session.

@param aViewId the id of the view session
*/
void CCntPplViewManager::BeginIterateL(TInt aViewId)
	{
	FindViewSessionByIdL(aViewId)->BeginIterateL();
	}

	
/**
End iteration in a view session.

@param aViewId the id of the view session
*/
void CCntPplViewManager::EndIterateL(TInt aViewId)
	{
	FindViewSessionByIdL(aViewId)->EndIterate();
	}

	
/**
Get next view item in a view session.

@param aViewId the id of the view session
@return the view item got from the database
@leave KErrNotFound if iterator reachs the end of rows.
*/
CViewContact* CCntPplViewManager::NextItemL(TInt aViewId, TContactViewPreferences aViewPrefs)
	{
	return FindViewSessionByIdL(aViewId)->NextItemL(aViewPrefs);
	}

	
/**
Get view item by given contact id in a view session.

@param aViewId the id of the view session
@return the view item got from the database
@leave KErrNotFound if there is not the row.
*/
CViewContact* CCntPplViewManager::ItemAtL(TContactItemId aContactId, TInt aViewId)
	{
	return FindViewSessionByIdL(aViewId)->ItemAtL(aContactId);
	}

	
/**
Find a view session by given view session id.

@param aViewId the id of the view session
@return the view session
@leave KErrArgument if there is not such a view session.
*/
CCntPplViewSession* CCntPplViewManager::FindViewSessionByIdL(TInt aViewId) const
	{
	ASSERT(aViewId >= 0);
	TInt index = FindViewSessionIndexById(aViewId);
	if(index == KErrNotFound)
	    {
		User::Leave(KErrNotFound);
	    }
	    
	return iViewSessions[index];
	}	

	
/**
Find a view session by given view session id.

@param aViewId the id of the view session
@return the index of view session stored in view session array
        KNotFoundIndex if there is not such a view session.
*/
TInt CCntPplViewManager::FindViewSessionIndexById(TInt aViewId) const 
	{
	TInt index = KErrNotFound;
	TInt count = iViewSessions.Count();
	for(TInt i = 0; i < count; ++i)
		{
		ASSERT(iViewSessions[i] != NULL);
		if(aViewId == iViewSessions[i]->ViewId())
			{		
			index = i;
			break;
			}
		}
	return index;	
	}	


/**
CContactDatabase::ReadContactTextDefL() uses a text definition - a way of 
grouping fields to form a string which represents a contact item - to
build up a string from a contact item.

When creating the string, the contact item is searched for fields whose type 
matches one of the fields specified in the text definition.

The contents of the first matching field of each type found in the
item are read into the string.

These strings may be separated using a field separator, which is also specified 
in the text definition.  A separator is not appended to the last field used to 
make up the string.  It is only inserted when there is a following non-empty
field.

@param aContactId The contact ID for which the text definition is required.
@param aResult The string representing the contact item. 
@param aTextDef The text definition which contains the fields that are read 
and returned in the aResult string.

@return the contact type uid for the specified contact.				  
*/
TUid CCntPplViewManager::ReadContactTextDefL(TContactItemId aContactId, TDes &aResult,const CContactTextDef& aTextDef)
    {
    if (! iContactsFile.IsOpened())
    	{
    	User::Leave(KErrInUse);
    	}
    
	RSqlStatement contactSqlStmt;
	CleanupClosePushL(contactSqlStmt);
	
	/* we have to reload the row from SQL database to cache in text header and text blob */ 	
	contactSqlStmt.PrepareL(iContactsFile.NamedDatabase(), iSelectFullFieldsStmt->SqlStringL());
	User::LeaveIfError(contactSqlStmt.BindInt(KFirstIndex, aContactId)); //Bind item id into the condition.	
	
	TInt err = contactSqlStmt.Next();
	if(err == KSqlAtEnd)
		{
		//couldn't find the contact
		err = KErrNotFound;
		}			
		
	User::LeaveIfError(err);
	
	TInt typeFlags = contactSqlStmt.ColumnInt(iSelectFullFieldsStmt->ParameterIndex(KContactTypeFlags()));
	TUid contactTypeUid = TCntPersistenceUtility::TypeFlagsToContactTypeUid(typeFlags);
	TInt attr = (typeFlags & EContactAttributes_Mask) >> EContactAttributes_Shift; 
	
	if((attr & EContactAttrsFlags_Deleted) == 0) 
		{
		//Only read text def for not deleted contact.
    	TBuf<KMaxContactTextSeperator> nextSeperator;
    	TContactTextDefItem textDefItem;
    	
    	// Mark the first iteration in the loop.
    	TBool firstText = ETrue;

    	TInt max = aTextDef.Count();
     	for(TInt i=0; i<max; ++i)
    		{
    		// Recreate the TTextFieldMinimal on each iteration.
    		CContactDatabase::TTextFieldMinimal textFieldMin;
    		textDefItem = aTextDef[i];
    		// Populate the TTextFieldMinimal.
    		CCntPplViewSession::TextFieldL(contactSqlStmt, *iSelectFullFieldsStmt, iContactProperties.SystemTemplateL(), textDefItem.iFieldType, textFieldMin);
    		// Append the returned text.
    		if (textFieldMin.Length() > 0)
    			{
    			// Only add a seperator after the first text has been appended.
    			if ((firstText == EFalse) && (aResult.MaxLength() > aResult.Length()))
    				{
    				aResult.Append(nextSeperator.Left(Min(aResult.MaxLength() - aResult.Length(), nextSeperator.Length())));	
    				}
    			aResult.Append(textFieldMin.Left(Min(aResult.MaxLength() - aResult.Length(),textFieldMin.Length())));
    			firstText = EFalse; 
    			}
    		nextSeperator = textDefItem.iSeperator;
    		}
    	
    	// Add the best match if it was requested.
    	if(aResult.Length() == 0 && const_cast<CContactTextDef&>(aTextDef).ExactMatchOnly() == EFalse)
    		{
    		CContactDatabase::TTextFieldMinimal textFieldMin;
    		if (aTextDef.FallbackField() != KUidContactFieldNone) 
    			{
    			CCntPplViewSession::TextFieldL(contactSqlStmt, *iSelectFullFieldsStmt, iContactProperties.SystemTemplateL(), aTextDef.FallbackField(), textFieldMin);
    			}
    		if(textFieldMin.Length() == 0) // No results from call to TextFieldL()?
    			{
    			CCntPplViewSession::TextFieldL(contactSqlStmt, *iSelectFullFieldsStmt, iContactProperties.SystemTemplateL(), KUidContactFieldMatchAll, textFieldMin);
    			}
    		aResult.Append(textFieldMin.Left(Min(aResult.MaxLength(),textFieldMin.Length())));
    		}
		} //if((attr & EContactAttrsFlags_Deleted) != 0) 
	
	CleanupStack::PopAndDestroy(&contactSqlStmt);
	
	return contactTypeUid;
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

@param aContactId The contact ID for which the field text is required.
@param aFieldType the field type to determine what kind of text client required. 
@param aText the reference to the buffer of final found field text.
*/
void CCntPplViewManager::TextFieldL(TInt aContactId, TFieldType aFieldType, TDes& aText)
    {
    if (! iContactsFile.IsOpened())
    	{
    	User::Leave(KErrInUse);
    	}
    
	RSqlStatement contactSqlStmt;
	CleanupClosePushL(contactSqlStmt);
	
	/* we have to reload the row from SQL database to cache in text header and text blob */ 	
	contactSqlStmt.PrepareL(iContactsFile.NamedDatabase(), iSelectFullFieldsStmt->SqlStringL());
	User::LeaveIfError(contactSqlStmt.BindInt(KFirstIndex, aContactId)); //Bind item id into the condition.	
	
	TInt err = contactSqlStmt.Next();
	if(err == KSqlAtEnd)
		{
		//couldn't find the contact
		err = KErrNotFound;
		}			
		
	User::LeaveIfError(err);
	
    CCntPplViewSession::TextFieldL(contactSqlStmt, *iSelectFullFieldsStmt, iContactProperties.SystemTemplateL(), aFieldType, aText);    
    
	CleanupStack::PopAndDestroy(&contactSqlStmt);
    }
	
