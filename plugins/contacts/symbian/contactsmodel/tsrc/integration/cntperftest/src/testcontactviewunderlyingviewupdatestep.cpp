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

 
#include "testcontactviewunderlyingviewupdatestep.h"

/**
 * Test Step Constructor
 */
CTestContactViewUnderlyingViewUpdateStep::CTestContactViewUnderlyingViewUpdateStep()
:CTestContactViewDatabaseUtilitiesStep()
	{
	// Mandatory call to base class method to set up the human readable name for logging.		
	SetTestStepName(KTestContactViewUnderLyingViewUpdateStep);	
	}			

/**
 * Base class pure virtual.
 * This function sets up the work envirnoment required by the test step
 * @return EPass or EFail indicating the result of the test step preamble operation.
 */
TVerdict CTestContactViewUnderlyingViewUpdateStep::doTestStepPreambleL()
	{
	CTestContactViewDatabaseUtilitiesStep::doTestStepPreambleL();
	return TestStepResult();
	}

/**
 *Base class pure virtual.
 *@return EPass or EFail indicating the result of the test step.
 */
TVerdict CTestContactViewUnderlyingViewUpdateStep::doTestStepL()
	{
	ConstructViewsL();
	ReadIniAndConstructViewUpdateObjectsL();	
	ReadIniAndConstructViewValidationObjectsL();	
	RemoveContactsFromTheUnderLyingViewsL();
	ListenForViewEventsAndValidateL();
 	ValidateViewCountL();
	return TestStepResult();
	}	

/**
 * Reads data specified in the ini file and constructs all relevant view update objects
 * These view update objects serve as source for performing underlying view update operations
 */
void CTestContactViewUnderlyingViewUpdateStep::ReadIniAndConstructViewUpdateObjectsL()
	{
	_LIT(KListOfViewsToBeRemoved, "ListOfViewsToBeUpdated");
	TPtrC listOfViewsToBeUpdated;
	TESTL(GetStringFromConfig(ConfigSection(), KListOfViewsToBeRemoved, listOfViewsToBeUpdated));

	RArray<TPtrC>	sections;
	CleanupClosePushL(sections);
	ViewUtilityReference().TokenizeStringL(listOfViewsToBeUpdated, sections);

	for ( TInt i = 0; i < sections.Count(); ++i )
		{
		TViewModificationData viewModificationData;

		// Here data is read from the ini file about various contact view update operations
		// to be performed. This information comprises of view to be updated and number of contacts
		// to be removed in the specified view
		_LIT(KContactViewType, "ViewType");
		TPtrC contactViewTypeString;
		GetStringFromConfig(sections[i], KContactViewType, contactViewTypeString);
		viewModificationData.SetViewType(contactViewTypeString);

		_LIT(KContactViewIndex, "ViewIndex");
		TInt contactViewIndex;
		GetIntFromConfig(sections[i], KContactViewIndex, contactViewIndex);
		viewModificationData.SetViewIndex(contactViewIndex);

		_LIT(KNumberOfContactToBeRemoved, "NumberOfContactsToBeRemoved");
		TInt numberOfContactsToBeRemoved;
		GetIntFromConfig(sections[i], KNumberOfContactToBeRemoved, numberOfContactsToBeRemoved);
		viewModificationData.SetNumberOfContactsToBeRemoved(numberOfContactsToBeRemoved);

		iViewUpdateArray.AppendL(viewModificationData);
		}

	CleanupStack::PopAndDestroy(&sections); 
	}
	
	
/**
 * Reads data specified in the ini file and constructs all view validation objects
 */
void CTestContactViewUnderlyingViewUpdateStep::ReadIniAndConstructViewValidationObjectsL()
	{
	_LIT(KListOfViewsToBeValidated, "ListOfViewsToBeValidated");
	TPtrC listOfViewsToBeRemoved;
	TESTL(GetStringFromConfig(ConfigSection(), KListOfViewsToBeValidated, listOfViewsToBeRemoved));

	RArray<TPtrC>	sections;
	CleanupClosePushL(sections);
	ViewUtilityReference().TokenizeStringL(listOfViewsToBeRemoved, sections);

	for ( TInt i = 0; i < sections.Count(); ++i )
		{
		TViewValidationData ViewValidationData;

		_LIT(KContactViewType, "ViewType");
		TPtrC contactViewTypeString;
		GetStringFromConfig(sections[i], KContactViewType, contactViewTypeString);
		ViewValidationData.SetViewType(contactViewTypeString);

		_LIT(KContactViewIndex, "ViewIndex");
		TInt contactViewIndex;
		GetIntFromConfig(sections[i], KContactViewIndex, contactViewIndex);
		ViewValidationData.SetViewIndex(contactViewIndex);

		_LIT(KUpdatedViewCount, "UpdatedViewCount");
		TInt updatedViewCount = 0;
		GetIntFromConfig(sections[i], KUpdatedViewCount, updatedViewCount);
		ViewValidationData.SetExpectedViewCount(updatedViewCount);

		iViewValidationArray.AppendL(ViewValidationData);
		}

	CleanupStack::PopAndDestroy(&sections); 
	}



/**
 * Removes contacts from the underlying view based on the data available with contact view update objects
 */ 
void CTestContactViewUnderlyingViewUpdateStep::RemoveContactsFromTheUnderLyingViewsL()
	{
	for(TInt i = 0; i < iViewUpdateArray.Count(); ++i)
		{
		TViewModificationData contactViewValidationObject = iViewUpdateArray[i];		
		CContactViewBase* contactView = ViewCollectionReference().GetDesiredView
							(contactViewValidationObject.ViewTypeInStringFormat(), contactViewValidationObject.ViewIndex());
	

		RArray<TContactItemId> contactItemIdArray;
		CleanupClosePushL(contactItemIdArray);
		
		for(TInt j = 0; j < contactViewValidationObject.NumberOfContactsToBeRemoved(); ++j)
			{
			TContactItemId contactItemId = contactView->AtL(j);
			contactItemIdArray.AppendL(contactItemId);
			}

		for(TInt j = 0; j < contactItemIdArray.Count(); ++j)
			{
			DatabaseReference().DeleteContactL(contactItemIdArray[j]);
			}
			
		CleanupStack::PopAndDestroy(&contactItemIdArray); 
		}
	}

/** 
 * Listens for contact view notifications and validates that only remove contact item events are received
 */
void CTestContactViewUnderlyingViewUpdateStep::ListenForViewEventsAndValidateL()
	{
	TContactViewEvent contactViewEvent;
	while(CTestContactViewDatabaseUtilitiesStep::ListenForViewEventsL(contactViewEvent))
		{
		if(contactViewEvent.iEventType != TContactViewEvent::EItemRemoved)
			{
			_LIT(KUndesiredContactViewEvent, " An undesired contact view event i.e. a contact view event other than remove item contact event was received was received : %d");
			ERR_PRINTF2(KUndesiredContactViewEvent, contactViewEvent.iEventType);
			SetTestStepResult(EFail);
			}
		}		
	}

/**
 * Validates the contact view count against expected data specified in the contact view validation objects
 */
void CTestContactViewUnderlyingViewUpdateStep::ValidateViewCountL()
	{
	for(TInt i = 0; i < iViewValidationArray.Count(); ++i)
		{
		TViewValidationData validationInfo =	iViewValidationArray[i];
		CContactViewBase* contactView = ViewCollectionReference().GetDesiredView(validationInfo.ViewTypeInStringFormat(), validationInfo.ViewIndex());

		if(contactView->CountL() != validationInfo.ExpectedViewCount())
			{
			_LIT(KErrContactViewMisMatch, "The contact view doesnt reflect the desired view count \n");
			ERR_PRINTF1(KErrContactViewMisMatch);
			
			_LIT(KInfoViewCountData, "The expected view count is %d while the actual view count is %d \n");
			INFO_PRINTF3(KInfoViewCountData, validationInfo.ExpectedViewCount(), contactView->CountL());
			
			SetTestStepResult(EFail);
			}
		}
	}

/**
 * Converts the view type information in TViewData::TContactViewType to string format 
 */
void TViewData::ConvertViewTypeToString(TViewData::TContactViewType aViewType, TDes& aViewTypeInString)
	{
	if ( aViewType == TViewData::ELocalView )
		{
		aViewTypeInString.Copy(KLocalView);
		}
	else if ( aViewType == TViewData::ERemoteView)
		{
		aViewTypeInString.Copy(KRemoteView);
		}
	else if ( aViewType == TViewData::ENamedRemoteView)
		{
		aViewTypeInString.Copy(KNamedRemoteView);
		}
	else if ( aViewType == TViewData::EGroupView)
		{
		aViewTypeInString.Copy(KGroupView);
		}
	else if ( aViewType == TViewData::EFindView)
		{
		aViewTypeInString.Copy(KFindView);
		}
	else if ( aViewType == TViewData::EFilteredView)
		{
		aViewTypeInString.Copy(KFilteredView);
		}
	else if ( aViewType == TViewData::EConcatenatedView)
		{
		aViewTypeInString.Copy(KConcatenatedView);
		}
	else if ( aViewType == TViewData::ESubView)
		{
		aViewTypeInString.Copy(KSubView);
		}
	}

/**
 * Converts the view type information available in string format to  TViewData::TContactViewType
 * @param aViewTypeString view type information in string format
 * @return TViewData::TContactViewType - view type information in TViewData::TContactViewType format
 */
TViewData::TContactViewType TViewData::ConvertViewType(const TPtrC& aViewTypeString)
	{
	TViewData::TContactViewType viewType = TViewData::ELocalView;

	if ( aViewTypeString.Compare(KLocalView) == 0 )
		{
		viewType = TViewData::ELocalView;
		}
	else if ( aViewTypeString.Compare(KRemoteView) == 0 )
		{
		viewType = TViewData::ERemoteView;
		}
	else if ( aViewTypeString.Compare(KNamedRemoteView) == 0 )
		{
		viewType = TViewData::ENamedRemoteView;
		}
	else if ( aViewTypeString.Compare(KGroupView) == 0 )
		{
		viewType = TViewData::EGroupView;
		}
	else if ( aViewTypeString.Compare(KFindView) == 0 )
		{
		viewType = TViewData::EFindView;
		}
	else if ( aViewTypeString.Compare(KFilteredView) == 0 )
		{
		viewType = TViewData::EFilteredView;
		}
	else if ( aViewTypeString.Compare(KConcatenatedView) == 0 )
		{
		viewType = TViewData::EConcatenatedView;
		}
	else if ( aViewTypeString.Compare(KSubView) == 0 )
		{
		viewType = TViewData::ESubView;
		}

	return 	viewType;
	}	
