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


#include "testgetsetviewsettingstep.h"
#include <cntviewstore.h>

//For default actions
_LIT8(KSortPluginName, "");
_LIT(KTempViewName,"TempViewName");


CTestGetSetViewSettingStep::~CTestGetSetViewSettingStep()
/**
 * Destructor
 */
	{
	}


CTestGetSetViewSettingStep::CTestGetSetViewSettingStep()
/**
 * Constructor
 */
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KTestGetSetViewSettingStep); 
	}


TVerdict CTestGetSetViewSettingStep::doTestStepPreambleL()
/**Step
 * @return - TVerdict code
 * Override of base class virtual
 */
 	{
 	GetStringFromConfig(ConfigSection(),KApiActionTag,iIniApiAction);
 	GetIntFromConfig(ConfigSection(),KNoOfViewsTag,iIniNumberOfViews);
 	return TestStepResult();
 	}
 	

TVerdict CTestGetSetViewSettingStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class virtual. Implements the main control function
 * which calls a number of actions that this test step is responsible for
 */
	{
	SetTestStepResult(EPass);
	
	if (iIniApiAction.Compare(KApiGet) == KErrNone)
	    {
	    RPointerArray<CContactDefaultViewDefinition> storedViewDefs;
	    CleanupClosePushL(storedViewDefs);
		TRAPD(err, TContactViewStore::GetDefaultViewsL(storedViewDefs));
		if (err != KErrNone || storedViewDefs.Count() != iIniNumberOfViews)
		    {
			SetTestStepResult(EFail);
			User::Leave(err);
		    }		    
		else
	  	    {	    
	        TBool res = TestStoredDefaultViewsL(storedViewDefs);	 
	        if (!res)
	            {
	    	    SetTestStepResult(EFail);
	            }	    
	        }
	    CleanupStack::PopAndDestroy();
	    }
	    
	else if (iIniApiAction.Compare(KApiSet) == KErrNone)
	    {
	    TRAPD(err,SetNewDefaultViewsL())
	    if (err != KErrNone)
	        {
	    	SetTestStepResult(EFail);
	        }
	    }
	    
	else if (iIniApiAction.Compare(KApiAppend) == KErrNone)
	    {
		TRAPD(err,AppendToDefaultViewListL());
		if (err != KErrNone)
		    {
			SetTestStepResult(EFail);
		    }
	    }
	    
	else if (iIniApiAction.Compare(KApiDelete) == KErrNone)
	    {
		TRAPD(err,DeleteAViewDefinitionL());
		if (err != KErrNone)
		    {
			SetTestStepResult(EFail);
		    }
	    }
	    
	else if (iIniApiAction.Compare(KApiOOMGet) == KErrNone)
	    {
		TRAPD(err,OOMTestGetDefaultViewsL());
		if (err != KErrNone)
		    {
			SetTestStepResult(EFail);
		    }
	    }
	    
	else if (iIniApiAction.Compare(KApiOOMSet) == KErrNone)
	    {
		TRAPD(err,OOMTestSetDefaultViewsL());
		if (err != KErrNone)
		    {
			SetTestStepResult(EFail);
		    }
	    }
	else
	    {
		//illegal api action
		SetTestStepResult(EFail);
	    }    
	   
	return TestStepResult();    
	}

	
TVerdict CTestGetSetViewSettingStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}
	

/**
 * return TBool
 * @param aStoredViewDefs  an array of pointers to CContactDefaultViewDefinition objects
 * Implements a method to test the view definitions retrieved from
 * central repository against whats expected in the ini file
 */	
TBool CTestGetSetViewSettingStep::TestStoredDefaultViewsL(RPointerArray<CContactDefaultViewDefinition>& aStoredViewDefs)
    {
    RContactViewSortOrder viewSortOrder;
	for (TInt n = 0; n < iIniNumberOfViews; n++)
	    {
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
		
	    CContactDefaultViewDefinition* gotDefs = aStoredViewDefs[n];
	    
	    if ((gotDefs->ViewNameL().Compare(viewNameFromIni)) != KErrNone)
		    {
		    _LIT(KWrongName, "View Name does not match expected name in INI file");
		    ERR_PRINTF1(KWrongName);
		    return (EFalse);
		    }
		else
		    {
			_LIT(KNameOk,"View Name mataches expected name in the INI file");
			INFO_PRINTF1(KNameOk);
		    }
		     
		//Truncate plugin name
		HBufC8 *buf = NULL;
		TRAPD(err, buf = HBufC8::NewL(pluginNameFromIni.Size()));
		CleanupStack::PushL(buf);
		
		if (err != KErrNone)
		    {
		    CleanupStack::PopAndDestroy(buf);
			return (EFalse);
	    	}
	    	
        TPtr8 tptr8pluginNameFromIni = buf->Des();
        tptr8pluginNameFromIni.Copy(pluginNameFromIni);
        		   
	    if (gotDefs->SortPluginNameL().Length() == 0 && tptr8pluginNameFromIni.Length() != 0)
	        {
	        _LIT(KPlugLenthZero,"PluginLength = 0 But Ini expected a valid name");
	        ERR_PRINTF1(KPlugLenthZero);
	        CleanupStack::PopAndDestroy(buf);
		    return (EFalse);
	        }
	    else if (gotDefs->SortPluginNameL().Length() != 0 && tptr8pluginNameFromIni.Length() == 0)
	        {
	   	    _LIT(KPlugIniZero,"Ini Plugin name missing, valid Plugin Name stored in CR");
	   	    ERR_PRINTF1(KPlugIniZero);
	   	    CleanupStack::PopAndDestroy(buf);
		    return (EFalse);
	        }
	    else if ((gotDefs->SortPluginNameL().Compare(tptr8pluginNameFromIni)) == KErrNone)
	        {
		    _LIT(KMatch,"Expected Plugin name received");
		    INFO_PRINTF1(KMatch);
		    CleanupStack::PopAndDestroy(buf);
		    }
		else
		    {
			 _LIT(KWrongName, "PlugIn Name Does Not Match Name in INI File");
		    ERR_PRINTF1(KWrongName);
		    CleanupStack::PopAndDestroy(buf);
		    return (EFalse);
		    }
		    
		if (gotDefs->ViewPreferences() != viewPreferanceFromIni)
		    {
		    _LIT(KVprefNoMatch,"ViewPreferances do not match");
		    ERR_PRINTF1(KVprefNoMatch);
		    return (EFalse);
		    }
		else
		    {
		    _LIT(KVprefMatch,"ViewPreferances match ok");
		    INFO_PRINTF1(KVprefMatch);	
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
		       
		if (gotDefs->ViewType() != viewType)
		    {
			_LIT(KViewTypeNoMatch,"View Types do not match");
		    ERR_PRINTF1(KViewTypeNoMatch);
		    return (EFalse);
		    }
		else
		    {
			_LIT(KViewTypeMatch,"View Types match ok");
		    INFO_PRINTF1(KViewTypeMatch);
		    }
		    
		viewSortOrder = gotDefs->SortOrder();
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
	    return (ETrue);
    }	   
		

/**
 * Implements a method to overwrite the view settings in central
 * repository by calling the SetDefaultViewsL() API. View settings
 * are read in from the ini file.
*/
void CTestGetSetViewSettingStep::SetNewDefaultViewsL()
    {
    RPointerArray<CContactDefaultViewDefinition> viewDefs;
    CContactDefaultViewDefinition* createdViewDef;
    CleanupClosePushL(viewDefs);
    
    RContactViewSortOrder viewSortOrder;
    CleanupClosePushL(viewSortOrder);
        
	for (TInt n = 0; n < iIniNumberOfViews; n++)
	    {		
		TBuf<KMaxTagLen>viewNameTag;
		TBuf<KMaxTagLen>pluginNameTag;
		TBuf<KMaxTagLen>viewPreferanceTag;
		TBuf<KMaxTagLen>viewTypeTag;
		TBuf<KMaxTagLen>sortOrderTag;
		
		TPtrC viewNameFromIni;
		TPtrC pluginNameFromIni;
		TInt  viewPreferanceFromIni;
		TInt  viewTypeFromIni;
		TInt  sortOrderFromIni;
		
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
				
		// change the view type
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
			_LIT(WrongViewType,"Incorrect View Type now being used from ini file");
			INFO_PRINTF1(WrongViewType);
			viewType = ((CContactDefaultViewDefinition::TViewType)viewTypeFromIni);
		    }
				
		TContactViewPreferences viewPref = static_cast<TContactViewPreferences> (viewPreferanceFromIni);
		
		viewSortOrder.Close();
		viewSortOrder = RContactViewSortOrder();
		
		TUid sortOrderUid = TUid::Null();   
	    sortOrderTag = KFirstSortOrderTag;
	    sortOrderTag.AppendNum(n+1);
	    if(GetIntFromConfig(ConfigSection(),sortOrderTag,sortOrderFromIni))
	        {
	    	sortOrderUid = TUid::Uid(sortOrderFromIni); 
		    viewSortOrder.AppendL(sortOrderUid);
	        }
		   
	    sortOrderTag = KSecondSortOrderTag;
	    sortOrderTag.AppendNum(n+1);
	    if(GetIntFromConfig(ConfigSection(),sortOrderTag,sortOrderFromIni))
	        {
	    	sortOrderUid = TUid::Uid(sortOrderFromIni); 
		    viewSortOrder.AppendL(sortOrderUid);
	        }
		  
	    sortOrderTag = KThirdSortOrderTag;
	    sortOrderTag.AppendNum(n+1);
	    if(GetIntFromConfig(ConfigSection(),sortOrderTag,sortOrderFromIni))
	        {
	    	sortOrderUid = TUid::Uid(sortOrderFromIni); 
	    	viewSortOrder.AppendL(sortOrderUid);
	        }
		
		//Read the plugin name from ini, truncate the name and set plugin
		//name with the set method
		HBufC8 *buf = NULL;
		TRAPD(err1, buf = HBufC8::NewL(pluginNameFromIni.Size()));
		CleanupStack::PushL(buf);
		if (err1 != KErrNone)
		    {
			User::Leave(err1);
	    	}		
		TPtr8 tptr8pluginNameFromIni = buf->Des();
        tptr8pluginNameFromIni.Copy(pluginNameFromIni);      
		
		//Create a CContactDefaultViewDefinition object with data from ini file
	    createdViewDef = NULL;		
		TRAPD(res,createdViewDef = CContactDefaultViewDefinition::NewLC(viewType,
		                                                                viewNameFromIni,
		                                                                viewSortOrder,
		                                                                viewPref,
		                                                                tptr8pluginNameFromIni);
		CleanupStack::Pop(createdViewDef););
		CleanupStack::PopAndDestroy(buf);
		CleanupStack::PushL(createdViewDef);
		if(res != KErrNone)
		    {
			User::Leave(res);
		    }                                     		    
		viewDefs.AppendL(createdViewDef);	
	    }	
	    	    
	    TRAPD(err2,TContactViewStore::SetDefaultViewsL(viewDefs));
	    if (err2 != KErrNone)
	        {
	    	_LIT(KSetFail,"SetDefaultViewsL(viewDefs) left with err2 = %d");
	    	ERR_PRINTF2(KSetFail,err2);
	    	User::Leave(err2);
	        }
	        
	    for(TInt i = viewDefs.Count()-1; i >= 0; --i)
		    {
		    CleanupStack::PopAndDestroy(viewDefs[i]);	
		    }
	    
	    CleanupStack::PopAndDestroy();
	    CleanupStack::PopAndDestroy();
    }
    

/**
 * Implements a method to fetch the existing view settings
 * from central repository, then creates a new view definition
 * object and proceeds to set the new object as an appended item
 * to the existing view definition.
*/
void CTestGetSetViewSettingStep::AppendToDefaultViewListL()
    {
    RPointerArray<CContactDefaultViewDefinition> viewDefs;
    RContactViewSortOrder viewSortOrder;
    CleanupClosePushL(viewSortOrder);
    CContactDefaultViewDefinition* createdViewDef = NULL;
    
    viewSortOrder.AppendL(KUidContactFieldFamilyName);
	viewSortOrder.AppendL(KUidContactFieldFamilyName);
	viewSortOrder.AppendL(KUidContactFieldFamilyName);
	
    TRAPD(err, TContactViewStore::GetDefaultViewsL(viewDefs));
    if (err != KErrNone)
        {
    	_LIT(KAppErr,"DefaultViewsL(viewDefs) left with err = %d");
    	ERR_PRINTF2(KAppErr,err);
    	User::Leave(err);
        }
			
	TRAPD(res,createdViewDef = CContactDefaultViewDefinition::NewLC(CContactDefaultViewDefinition::ENamedRemoteView,
		                                                            KTempViewName,
		                                                            viewSortOrder,
		                                                            EContactsOnly,
		                                                            KSortPluginName);
		                                                                
		                                                                
	CleanupStack::Pop(createdViewDef););
	CleanupStack::PushL(createdViewDef);
	
	if(res != KErrNone)
        {
		User::Leave(res);
		}
	
	viewDefs.AppendL(createdViewDef);
	
	TRAPD(err1,
				TContactViewStore::SetDefaultViewsL(viewDefs);
				viewDefs.Remove(viewDefs.Find(createdViewDef));
			);
	if (err1 != KErrNone)
        {
    	_LIT(KAppErr,"SetDefaultViewsL(viewDefs) left with err = %d");
    	ERR_PRINTF2(KAppErr,err1);
    	User::Leave(err1);
        }
        	        
    CleanupStack::PopAndDestroy(createdViewDef);	    
	CleanupStack::PopAndDestroy(&viewSortOrder);    
	viewDefs.ResetAndDestroy();
    }
    
    
/**
 * Implements a method to fetch the view settings from central
 * repository, it then performs a search on the view name and if
 * found deletes the view setting and restores with the
 * SetDefaultViewsL() API. 
 */
void CTestGetSetViewSettingStep::DeleteAViewDefinitionL()
    {
	RPointerArray<CContactDefaultViewDefinition> viewDefs;
    	
	TRAPD(err, TContactViewStore::GetDefaultViewsL(viewDefs));
    if (err != KErrNone)
        {
    	_LIT(KAppErr,"DefaultViewsL(viewDefs) left with err = %d");
    	ERR_PRINTF2(KAppErr,err);
    	User::Leave(err);
        }
    
    TPtrC viewNameFromIni;
    GetStringFromConfig(ConfigSection(),KViewNameTag,viewNameFromIni);
    	
    for (TInt n = 0; n < viewDefs.Count(); n++)
        {
    	CContactDefaultViewDefinition* gotDefs = viewDefs[n];
    	if (gotDefs->ViewNameL() == viewNameFromIni)
    	    {
    		viewDefs.Remove(n);
    	    }
    	else
    	    {
    		_LIT(KViewNotFound,"%s was not found");
    		ERR_PRINTF2(viewNameFromIni,KViewNotFound);
    	    }    	
        }
    
    if (viewDefs.Count() >= 0)
        {
    	TRAPD(err1,TContactViewStore::SetDefaultViewsL(viewDefs));
	    if (err1 != KErrNone)
            {
    	    _LIT(KAppErr,"SetDefaultViewsL(viewDefs) left with err1 = %d");
    	    ERR_PRINTF2(KAppErr,err1);
    	    User::Leave(err1);
            }
        }
       
    viewDefs.ResetAndDestroy();    
    }


/**
 * Implements a method to test the GetDefaultViewsL() API
 * under out of memory conditions.
 */
void CTestGetSetViewSettingStep::OOMTestGetDefaultViewsL()
    {
	RPointerArray<CContactDefaultViewDefinition> storedViewDefs;    
    for (TInt i=1 ;; i++)
        {  
    	__UHEAP_SETFAIL(RHeap::EFailNext,i);  
    	__UHEAP_MARK;     	
    	
    	TRAPD(err, TContactViewStore::GetDefaultViewsL(storedViewDefs)); 
    	storedViewDefs.ResetAndDestroy();
    	
    	__UHEAP_MARKEND;
    		    	
    	if (err != KErrNoMemory)
		    {
		    User::LeaveIfError(err);
		    break;		   
		    }
        }
        
    storedViewDefs.ResetAndDestroy();
    __UHEAP_RESET;    
    }

/**
 * Implements a method to test the SetDefaultViewsL() API
 * under out of memory conditions. A CContactDefaultViewDefinition
 * object is created within a for loop, the number of iterations being
 * controlled by a ini file parameter. The array of objects are passed to
 * the SetDefaultViewsL() API which is under test.
 */
void CTestGetSetViewSettingStep::OOMTestSetDefaultViewsL()
    {
    RPointerArray<CContactDefaultViewDefinition> viewDefs;
    CleanupClosePushL(viewDefs);    
    RContactViewSortOrder viewSortOrder;
    CleanupClosePushL(viewSortOrder);
    CContactDefaultViewDefinition* createdViewDef;
    
    viewSortOrder.AppendL(KUidContactFieldFamilyName);
	viewSortOrder.AppendL(KUidContactFieldFamilyName);
	viewSortOrder.AppendL(KUidContactFieldFamilyName);
    
	for (TInt n = 0; n < iIniNumberOfViews; n++)
	    {		
	    createdViewDef = NULL;		
		TRAPD(res,createdViewDef = CContactDefaultViewDefinition::NewLC(CContactDefaultViewDefinition::ENamedRemoteView,
		                                                                KTempViewName,
		                                                                viewSortOrder,
		                                                                EContactsOnly,
		                                                                KSortPluginName);
		CleanupStack::Pop(createdViewDef););
		CleanupStack::PushL(createdViewDef); 
		if(res != KErrNone)
		    {
			User::Leave(res);
		    }                                    		    
		viewDefs.AppendL(createdViewDef);	
	    }
	
	//Do the OOM test loop on the TContactViewStore::SetDefaultViewsL() API   	    	
	for (TInt i=1 ;; i++)
	    {
	    __UHEAP_MARK;
	    __UHEAP_SETFAIL(RHeap::EFailNext,i);	    
		TRAPD(err,TContactViewStore::SetDefaultViewsL(viewDefs));
		__UHEAP_MARKEND;
		
		if (err != KErrNoMemory)
		    {
			User::LeaveIfError(err);
			break;
		    }
	    }
	    
	__UHEAP_RESET; 	
	
	for(TInt i = viewDefs.Count()-1; i >= 0; --i)
        {
		CleanupStack::PopAndDestroy(viewDefs[i]);	
		}
		
    CleanupStack::PopAndDestroy();
    CleanupStack::PopAndDestroy();
    }

