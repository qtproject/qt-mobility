/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "testcontactsdbstep.h"
#include "cntviewstoreprivate.h"
#include <e32panic.h>


CTestContactsDbStep::~CTestContactsDbStep()
/**
 * Destructor
 */
	{
	delete iDb;
	iDb = NULL;
	}

CTestContactsDbStep::CTestContactsDbStep() 
/**
 * Constructor
 */
	{ 
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KTestContactsDbStep);
	}

TVerdict CTestContactsDbStep::doTestStepPreambleL()
/**Step
 * @return - TVerdict code
 * Override of base class virtual
 */
 	{
 	SetTestStepResult(EPass);
 	
 	GetStringFromConfig(ConfigSection(),KCntDbActionTag,iCntsDbAction); 
 	GetIntFromConfig(ConfigSection(),KNoOfCntsTag,iNumberOfContacts);
 	
 	CActiveScheduler* iSched = NULL;
    iSched = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(iSched);
 		
 	return TestStepResult();
 	}
 	

TVerdict CTestContactsDbStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
    {
    SetTestStepResult(EPass);
	
    if (iCntsDbAction.Compare(KCreateDb) == KErrNone)
        {
    	CreateDefaultCntsDb();
        }
        
    else if (iCntsDbAction.Compare(KOpenDb) == KErrNone)
        {
    	OpenDefaultCntsDb();
        }
        
    else if (iCntsDbAction.Compare(KReplaceDb) == KErrNone)
        {
    	ReplaceDefaultCntsDb();
        }
        
    else if (iCntsDbAction.Compare(KAddItemsDb) == KErrNone)
        {
    	TRAPD(err,AddContactsL(iNumberOfContacts));
    	if (err != KErrNone)
    	    {
    		_LIT(KAddCntLeft,"AddContactsL Left, err = %d");
    		INFO_PRINTF2(KAddCntLeft, err);
    		SetTestStepResult(EFail);
    	    }
        }
    
    else if (iCntsDbAction.Compare(KDeleteDb) == KErrNone)
        {
    	DeleteDefaultCntsDb();
        }
        
    else if (iCntsDbAction.Compare(KCheckCntViewDefs) == KErrNone)
        {
        
#ifdef _DEBUG

    	TBool res = CheckExistingViewDefsL();
    	if (!res)
    	    {
    		SetTestStepResult(EFail);
    	    }
#endif
        }
        
    else
        {
    	//illegal contacts db action
    	SetTestStepResult(EFail);
        }
        
    return TestStepResult();
    }
    

TVerdict CTestContactsDbStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
    {
    delete iSched;
	iSched = NULL;
	CActiveScheduler::Install(NULL);
	
	return TestStepResult();
    }


/**
 * @return - err 
 * Implements a method to create a default contacts database.
 */    
void CTestContactsDbStep::CreateDefaultCntsDb()   
    {
    TRAPD(err,iDb = CContactDatabase::CreateL());
    if ((err != KErrNone) && (err != KErrAlreadyExists))
        {
        _LIT(KNoDbCreate,"Could not create cnts default db, err = %d");
        ERR_PRINTF2(KNoDbCreate,err);  
        SetTestStepResult(EFail);
        }
    }


/**
 * @return - err
 * Implements a method to open a default contacts database.
 */
void CTestContactsDbStep::OpenDefaultCntsDb()   
    {
    TRAPD(err,iDb = CContactDatabase::OpenL());
    if (err != KErrNone)
        {
        _LIT(KNoDbOpen,"Could not open cnts default db, err = %d");
        ERR_PRINTF2(KNoDbOpen,err);  
        SetTestStepResult(EFail);
        }
    }
    

/**
 * @return err
 * Implements a method to delete and a default contacts database.
 */
void CTestContactsDbStep::DeleteDefaultCntsDb()   
    {
    delete iDb;
    iDb = NULL;
    
    TRAPD(err,CContactDatabase::DeleteDefaultFileL());
    if(err != KErrNone)
        {
    	_LIT(KNoDbDel,"Could not delete cnts default db, err = %d");
        ERR_PRINTF2(KNoDbDel,err);  
        SetTestStepResult(EFail);
        }
    }
    
    
/**
 * Implements a method to replace the default contacts database
 */   
void CTestContactsDbStep::ReplaceDefaultCntsDb()
    {
	TRAPD(err,iDb = CContactDatabase::ReplaceL())
	if (err != KErrNone)
	    {
		_LIT(KNoRep,"Could not replace default contacts db, err = %d");
		ERR_PRINTF2(KNoRep,err);
		SetTestStepResult(EFail);
	    }  
    }


/**
 * @return TBool
 * Implements a method to check the remote view definitions used
 * by the cnts server when it is running. They are not necessarily
 * the same as the view definitions stored in central repository.
 * The views are fetched using a debug mode only, test API, and compared
 * against the data in the ini file.
 */
TBool CTestContactsDbStep::CheckExistingViewDefsL()
    {        
 	RPointerArray<CContactDefaultViewDefinition> existingViewDefs;
 	RContactViewSortOrder viewSortOrder;
 	 
 	TInt noOfViewsFromIni;
 	GetIntFromConfig(ConfigSection(),KNoOfViewsTag,noOfViewsFromIni);	 
	
	TRAPD(err,TContactViewStorePrivate::GetDefinitionsOfExistingViewsL(KNullDesC, existingViewDefs));
	if ((err == KErrNone) && (existingViewDefs.Count() == noOfViewsFromIni))
	     {
	 	 for (TInt n = 0; n < noOfViewsFromIni; n++)
	         {
	         CContactDefaultViewDefinition* existingDefs = existingViewDefs[n];
		     TBuf<KMaxTagLen>viewNameTag;
		     TBuf<KMaxTagLen>pluginNameTag;
		     TBuf<KMaxTagLen>viewPreferanceTag;
		     TBuf<KMaxTagLen>viewTypeTag;
		     TBuf<KMaxTagLen>sortOrderTag;
		
		     TPtrC viewNameFromIni;
		     TPtrC pluginNameFromIni;
		     TInt  viewPreferanceFromIni;
		     TInt  viewTypeFromIni =0;
		     TInt  sortOrderFromIni =0;
		
	         viewNameTag       = KViewNameTag;
		     pluginNameTag     = KPluginNameTag;
		     viewPreferanceTag = KViewPreferanceTag;
		     viewTypeTag       = KViewTypeTag;
		
		     viewNameTag.AppendNum(n+1);
		     pluginNameTag.AppendNum(n+1);
		     viewPreferanceTag.AppendNum(n+1);
		     viewTypeTag.AppendNum(n+1);
		
		     GetStringFromConfig(ConfigSection(),viewNameTag,viewNameFromIni);
		     GetStringFromConfig(ConfigSection(),pluginNameTag,pluginNameFromIni);
		     GetIntFromConfig(ConfigSection(),viewPreferanceTag,viewPreferanceFromIni);
		     GetIntFromConfig(ConfigSection(),viewTypeTag,viewTypeFromIni);
             
             if ((existingDefs->ViewNameL().Compare(viewNameFromIni)) != KErrNone)
		         {
		         _LIT(KWrongName, "View Name does not match expected name in INI file");
		         ERR_PRINTF1(KWrongName);
		         return (EFalse);
		         }
		         
		     //Truncate plugin name
		     HBufC8 *buf = NULL;
		     TRAPD(err1, buf = HBufC8::NewL(pluginNameFromIni.Size()));
		     CleanupStack::PushL(buf);
		
		     if (err1 != KErrNone)
		        {
		        CleanupStack::PopAndDestroy(buf);
			    return (EFalse);
	    	    }
	    	
             TPtr8 tptr8pluginNameFromIni = buf->Des();
             tptr8pluginNameFromIni.Copy(pluginNameFromIni);
             if ((existingDefs->SortPluginNameL().Compare(tptr8pluginNameFromIni)) != KErrNone)
                 {
             	 _LIT(KNoMatch,"Plugin names dont match");
             	 ERR_PRINTF1(KNoMatch);
             	 return (EFalse);
                 }
             CleanupStack::PopAndDestroy(buf);
		         
		     if (existingDefs->ViewPreferences() != viewPreferanceFromIni)
		         {
		         _LIT(KVprefNoMatch,"ViewPreferances do not match");
		         ERR_PRINTF1(KVprefNoMatch);
		         return (EFalse);
		         }
		     
		     CContactDefaultViewDefinition::TViewType viewType;
	         if (viewTypeFromIni == 0)
		         {			
			     viewType = CContactDefaultViewDefinition::ERemoteView;
		         }
		     else if (viewTypeFromIni == 1)
		         {
			     viewType = CContactDefaultViewDefinition::ENamedRemoteView;
		         }
		     else
		         {
		         viewType = (CContactDefaultViewDefinition::TViewType)viewTypeFromIni;
		         }
		       
		     if (existingDefs->ViewType() != viewType)
		         {
			     _LIT(KViewTypeNoMatch,"View Types do not match");
		         ERR_PRINTF1(KViewTypeNoMatch);
		         return (EFalse);
		         }
		         
	     	viewSortOrder = existingDefs->SortOrder();
		    TUid sortOrderUid = TUid::Null();
								    
		    sortOrderTag = KFirstSortOrderTag;
	        sortOrderTag.AppendNum(n+1);
	    	    
		    if (GetIntFromConfig(ConfigSection(),sortOrderTag,sortOrderFromIni))						
		        {
			    sortOrderUid = TUid::Uid(sortOrderFromIni);
					
		        if (viewSortOrder[0] != sortOrderUid)
	                {
			        _LIT(KFirstSOWrong,"Incorrect first sort order received = %d");
			        ERR_PRINTF2(KFirstSOWrong,viewSortOrder[0]);
			        return (EFalse);
			        }
		        }
			
		    sortOrderTag = KSecondSortOrderTag;
		    sortOrderTag.AppendNum(n+1);
	
		    if (GetIntFromConfig(ConfigSection(),sortOrderTag,sortOrderFromIni))
		        {
			    sortOrderUid = TUid::Uid(sortOrderFromIni);
			
		        if (viewSortOrder[1] != sortOrderUid)
	                {
			        _LIT(KSecondSOWrong,"Incorrect second sort order received = %d");
			        ERR_PRINTF2(KSecondSOWrong,viewSortOrder[1]);
			        return (EFalse);
			        }
		        }
				
		    sortOrderTag = KThirdSortOrderTag;
		    sortOrderTag.AppendNum(n+1);
		
		    if (GetIntFromConfig(ConfigSection(),sortOrderTag,sortOrderFromIni))
		        {
			    sortOrderUid = TUid::Uid(sortOrderFromIni);
			
		        if (viewSortOrder[2] != sortOrderUid)
	                {
		            _LIT(KThirdSOWrong,"Incorrect third sort order received = %d");
			        ERR_PRINTF2(KThirdSOWrong,viewSortOrder[2]);
			        return (EFalse);
			        }
		        }
					    
	         }
	     viewSortOrder.Close();
	     }
	 else
	     {
	 	 _LIT(KErrExistingView,"Problem with GetDefinitionsOfExistingViewsL(existingViewDefs)");
	 	 ERR_PRINTF1(KErrExistingView);
	 	 return (EFalse);
	     }
	     
	 return (ETrue);
    }


/**
 * @param aNumber of contact items
 * Implements a simple method to a number of contact items to the
 * the default database.
*/    
void  CTestContactsDbStep::AddContactsL(TInt aNumber)
    {
    OpenDefaultCntsDb();
    if (iDb == NULL)
        {
    	SetTestStepResult(EFail);
        }
    else
        {
    	CContactItemViewDef* viewAll = CContactItemViewDef::NewL(CContactItemViewDef::EIncludeFields,
                                                                 CContactItemViewDef::EIncludeHiddenFields);
                                       
        viewAll->AddL(KUidContactFieldMatchAll);
        CleanupStack::PushL(viewAll);
                                         
        CContactTemplate* cntTemplate = STATIC_CAST(CContactTemplate*,iDb->ReadContactL(iDb->TemplateId(),*viewAll));
        CleanupStack::PushL(cntTemplate);
       
	    CContactCard* contact = CContactCard::NewLC(cntTemplate);
	    for(TInt i = 0; i < aNumber; i++)
	        {		
	        iDb->AddNewContactL(*contact);
	        }
	    	
	    TContactIter hh(*iDb);
	    _LIT(KText,"someDummyTextInFields");	
	    CContactItem *contactItem = iDb->OpenContactL( hh.NextL(), *viewAll );
	    CleanupStack::PushL(contactItem);
	    SetManyFieldsL(contactItem->CardFields(),KText,20);
	    iDb->CommitContactL(*contactItem);
	    
	    CleanupStack::PopAndDestroy(contactItem);	    
	    CleanupStack::PopAndDestroy(contact);    
        CleanupStack::PopAndDestroy(cntTemplate);
        CleanupStack::PopAndDestroy(viewAll);
        }
    
    }
    
/*sets a specific contact field to a value as determined by its type.
field at aPos in aFields is set to aText*/
void CTestContactsDbStep::SetFieldL(CContactItemFieldSet &aFields, const TInt aPos, const TDesC& aText)
	{
	CContactItemField &field = aFields[aPos];
	if( field.IsTemplateLabelField() )
		{
		return;
		}
		
	if( 0 == aText.Size() )
		{
		return;
		}
	
	switch(field.StorageType())
		{
		case KStorageTypeText:
			field.TextStorage()->SetTextL(aText);
		break;
		case KStorageTypeStore:
			{
			HBufC8 *buf = HBufC8::NewLC(aText.Size());
			TPtr8 tptr8 = buf->Des();
			tptr8.Copy(aText);
			field.StoreStorage()->SetThingL(*buf);
			CleanupStack::PopAndDestroy(buf);
			}
		break;
		case KStorageTypeContactItemId:
			{
			field.AgentStorage()->SetAgentId(0);
			}
		break;
		case KStorageTypeDateTime:
			{			
			field.DateTimeStorage()->SetTime( TTime(0) );
			}
		break;
		default:
			User::Panic(aText,EInvariantFalse);
		break;
		}
	}

void CTestContactsDbStep::SetManyFieldsL(CContactItemFieldSet &aFields, const TDesC& aText, const TInt aMany)
	{
	TInt count = aFields.Count();
	if( count > aMany )
		{
		count = aMany;
		}
		
	TInt i = 0;
	for(; i < count; ++i)
        {
		SetFieldL(aFields, i,aText);
		}
	}
