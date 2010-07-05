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


#include "contactviewsthread.h"

/**
 * The Contact Views thread entry point
 * @param aArgs - Command line parameters for the contact views thread
 * @return TInt - exit reason
 */
GLDEF_C TInt RunContactViewsThreadL(TAny* aArgs)
    {
	TInt err;
	CContactViews* contactViews = NULL;
	CTrapCleanup* trapCleanup = CTrapCleanup::New();
	CTestStep* testStep = static_cast<CTestStep*>(aArgs);
	CActiveScheduler*	scheduler = new(ELeave) CActiveScheduler;

	if (scheduler)
		{
		CActiveScheduler::Install(scheduler);
    	TRAP(err, contactViews = CContactViews::NewL(*testStep));

    	if (err == KErrNone)
			{
			TRAP(err, contactViews->RunTestL());
			}

		RSemaphore semaphoreStop;
		semaphoreStop.OpenGlobal(KSemaphoreName);

		delete contactViews;
		contactViews = NULL;
		delete scheduler;
		scheduler = NULL;
		delete trapCleanup;
		trapCleanup = NULL;

		semaphoreStop.Signal();
		semaphoreStop.Close();
		}

	return err;
	}

/**
 * Class CContactViews constructor
 * @param aTestStep - Reference to current test step, used for retrieveing data from the ini file
 *                  and logging of results
 */
CContactViews::CContactViews(CTestStep& aTestStep)
:iTestStep(aTestStep)
	{
	}

/**
 * Class CContactViews destructor
 */
CContactViews::~CContactViews()
	{
	iExpectedData.Close();
	iActualData.Close();
	delete iContactViewCollection;
	delete iContactUtility;
	delete iDb;
	delete iScheduler;
	iChunk.Close();
	iSemaphore.Close();
	}

/**
 * Allocates CContactViews object on heap
 * @param aTestStep - Test Step Reference, used for reading data from ini and logging purpose
 * @return pointer to newly constructed CContactViews object
 */
CContactViews* CContactViews::NewL(CTestStep& aTestStep)
	{
	CContactViews* self = new(ELeave) CContactViews(aTestStep);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
 * Second level CContactViews constructors
 * Obtains handle to the global semaphore used for synchronizations with main thread
 * Obtains handle to the Global chunk used for exchanging data across threads
 * Allocates CContactUtilitiesCollection object used for contact views operations
 * Allocates CContactViewCollection used for holding the newly constructed contact views
 */
void CContactViews::ConstructL()
	{
	iSemaphore.OpenGlobal(KSemaphoreName);
	SetupChunk();
	OpenDatabaseL();
	iContactViewCollection = CContactViewCollection::NewL();
	iContactUtility = CContactUtilitiesCollection::NewL(iTestStep, DatabaseReference(), ViewCollectionReference());
    iContactUtility->ConstructViewsL();
	ReadDataFromIniAndConstructValidationObjectsL();
    SetViewOfInterest();    
	}

/**
 * Helps to obtain reference to current database handle
 * @return Reference to current database handle
 */
CContactDatabase& CContactViews::DatabaseReference()
	{
	return *iDb;
	}

/**
 * Helps to obtain reference to current view collection
 * @return Reference to current view collection
 */
CContactViewCollection& CContactViews::ViewCollectionReference()
	{
	return *iContactViewCollection;
	}

/**
 * Helps to obtain reference to current test step used for reading data from ini and logging the results
 * @return Reference to current view collection
 */

CTestStep& CContactViews::TestStepReference()
	{
 	return iTestStep;
	}

/**
 * Setups global chunk used for exchanging data across threads
 */
void CContactViews::SetupChunk()
	{
	iChunk.OpenGlobal(KChunkName(), EFalse);
	iExecutionCompleted = reinterpret_cast<TBool*>(iChunk.Base());
	iCurrentOperation = sizeof(TBool) + reinterpret_cast<TPtrC*>(iChunk.Base());
	}

/**
 * Installs Active Schedular for the current thread
 */
void CContactViews::InstallActiveSchedularL()
	{
	iScheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(iScheduler);
	}


/**
 * Opens an existing contacts database file based on the data mentioned in the ini file
 */
void CContactViews::OpenDatabaseL()
	{
	TPtrC databaseFileName;
	_LIT(KDatabaseName, "dbname");
	TestStepReference().GetStringFromConfig(TestStepReference().ConfigSection(), KDatabaseName, databaseFileName);
	TRAPD(err, iDb = CContactDatabase::OpenL(databaseFileName));
	User::LeaveIfError(err);
	}


/**
 * Set the desired view to be validated for its content
 * There can be n number of view constructed. It is a good idea to concentrate on a specific view
 * and validate the view content at the end of each contact view operation
 */
void CContactViews::SetViewOfInterest()
	{
	TInt desiredViewIndex = 0;
	_LIT(KDesiredViewIndex, "DesiredViewIndex");
	TestStepReference().GetIntFromConfig(TestStepReference().ConfigSection(), KDesiredViewIndex, desiredViewIndex);

	TPtrC desiredViewType;
	_LIT(KDesiredViewType, "DesiredViewType");
	TestStepReference().GetStringFromConfig(TestStepReference().ConfigSection(), KDesiredViewType, desiredViewType);

	iContactView = iContactViewCollection->GetDesiredView(desiredViewType, desiredViewIndex);	
	}

/**
 * Returns indication whether the execution is completed or not
 * @return TBool - Information whether execution is completed or not
 */
TBool CContactViews::ExecutionCompleted()
	{
	return *iExecutionCompleted;
	}

/**
 * Loop Function which validates the contact views after each CRUD operation is performed in the main thread
 */
void CContactViews::RunTestL()
	{
	// Suspend the current thread, will be resumed at a later stage by the main thread at the
	// end of each CRUD Operation
	iSemaphore.Signal();
	RThread().Suspend();
	

	while (!ExecutionCompleted())
		{
		TContactViewValidationData actualValidationData;
		SetCurrentOperation(actualValidationData);
		CContactViewEventQueue& desiredViewObserver = ViewCollectionReference().RetrieveDesiredViewObserverL(TestStepReference());
		ListenForContactViewEvents(desiredViewObserver, actualValidationData);
		TInt viewCount = iContactView->CountL();
		actualValidationData.SetViewCount(viewCount);		
		iActualData.AppendL(actualValidationData);
		iSemaphore.Signal();
		RThread().Suspend();
		}

	for(TInt i = 0; i < iExpectedData.Count(); ++i)
		{
		TContactViewValidationData::TContactViewCurrentOperation currentOperation = iExpectedData[i].CurrentOperation();
		TContactViewValidationData actualResult = RetrieveValidationData(currentOperation);
		TBool result = Compare(iExpectedData[i], actualResult);
		if(!result)
			{ 
			PrintDetailsL(iExpectedData[i], actualResult);									
			const TInt KNullCount = 0;
			if(actualResult.ViewCount() == KNullCount)
				{
				return;
				}
			_LIT(KValidationFailed, "The actual data doesnt match with the expected data");
			TestStepReference().ERR_PRINTF1(KValidationFailed);
			TestStepReference().SetTestStepResult(EFail);
			} 
		}
	}

/**
 * Initally view validation objects are constructed based on data specified in the ini file.
 * They are retrieved from the collection abd further used for validation against actual results
 * @param - aCurrentOperation Current Operation to be validated
 * @return TContactViewValidationData - Object which contains expected results for the current Operation
 */
TContactViewValidationData CContactViews::RetrieveValidationData
							(TContactViewValidationData::TContactViewCurrentOperation aCurrentOperation)
	{
	TContactViewValidationData viewValidationData;
	for(TInt i = 0; i < iActualData.Count(); ++i)
		{
		TContactViewValidationData::TContactViewCurrentOperation currentOperation =	iActualData[i].CurrentOperation();
		if(aCurrentOperation == currentOperation)
			{
			viewValidationData = iActualData[i];
			}
		}
	return viewValidationData;
	}

/**
 * Sets the current CRUD Operation being performed in the main thread
 * @param - aValidationData on return contains information about current CRUD operation being performed in the main thread
 */
void CContactViews::SetCurrentOperation(TContactViewValidationData& aValidationData)
	{
	aValidationData.SetCurrentOperation(*iCurrentOperation);
	}

/**
 * Reads data from the ini file and constructs all the view validation objects
 * and uploads them in a collection
 */
void CContactViews::ReadDataFromIniAndConstructValidationObjectsL()
	{
	_LIT(KListOfValidationSectionsString, "ListOfValidationSections");
	TPtrC listOfValidationSectionsString;
	TestStepReference().GetStringFromConfig(TestStepReference().ConfigSection(), KListOfValidationSectionsString, listOfValidationSectionsString);

	RArray<TPtrC>	listOfValidationSections;
	CleanupClosePushL(listOfValidationSections);
	iContactUtility->TokenizeStringL(listOfValidationSectionsString, listOfValidationSections);

	for(TInt i = 0; i < listOfValidationSections.Count(); ++i)
		{
		TContactViewValidationData viewValidationData;
		IterateThroValidationSectionAndUpdateValidationData(listOfValidationSections[i], viewValidationData);
		iExpectedData.AppendL(viewValidationData);
		}

	CleanupStack::PopAndDestroy(&listOfValidationSections);
	}

/**
 * Reads a specific section in the ini file and constructs all the view validation objects
 * @param aValidationSection  desired section in the ini file
 * @param aValidationData contact view validation object to be updated
 */
void CContactViews::IterateThroValidationSectionAndUpdateValidationData
									(const TPtrC& aValidationSection, TContactViewValidationData& aValidationData)
	{
	_LIT(KUnAvailableNotifications,			"UnAvailableNotifications");
	_LIT(KReadyNotifications,				"ReadyNotifications");
	_LIT(KSortOrderChangedNotifications, 	"SortOrderChangedNotifications");
	_LIT(KSortErrorNotifications,			"SortErrorNotifications");
	_LIT(KServerErrorNotifications,			"ServerErrorNotifications");
	_LIT(KIndexingErrorNotifications,		"IndexingErrorNotifications");
	_LIT(KItemAddedNotifications, 			"ItemAddedNotifications");
	_LIT(KItemRemovedNotifications,			"ItemRemovedNotifications");
	_LIT(KGroupChangedNotifications,		"GroupChangedNotifications");


	TInt unAvailableNotifications = 0;
	TestStepReference().GetIntFromConfig(aValidationSection, KUnAvailableNotifications, unAvailableNotifications);
	aValidationData.SetNotificationsCount(TContactViewValidationData::EUnAvailableNotifications, unAvailableNotifications);

	TInt viewReadyNotifications = 0;
	TestStepReference().GetIntFromConfig(aValidationSection, KReadyNotifications, viewReadyNotifications);
	aValidationData.SetNotificationsCount(TContactViewValidationData::EViewReadyNotifications, viewReadyNotifications);

	TInt sortOrderChangedNotifications = 0;
	TestStepReference().GetIntFromConfig(aValidationSection, KSortOrderChangedNotifications, sortOrderChangedNotifications);
	aValidationData.SetNotificationsCount(TContactViewValidationData::EViewSortOrderChangedNotifications, sortOrderChangedNotifications);

	TInt sortErrorNotifications = 0;
	TestStepReference().GetIntFromConfig(aValidationSection, KSortErrorNotifications, sortErrorNotifications);
	aValidationData.SetNotificationsCount(TContactViewValidationData::EViewSortErrorNotifications, sortErrorNotifications);

	TInt serverErrorNotifications = 0;
	TestStepReference().GetIntFromConfig(aValidationSection, KServerErrorNotifications, serverErrorNotifications);
	aValidationData.SetNotificationsCount(TContactViewValidationData::EContactServerErrorNotifications, serverErrorNotifications);

	TInt indexingErrorNotifications = 0;
	TestStepReference().GetIntFromConfig(aValidationSection, KIndexingErrorNotifications, indexingErrorNotifications);
	aValidationData.SetNotificationsCount(TContactViewValidationData::EViewIndexingErrorNotifications, indexingErrorNotifications);

	TInt itemAddedNotifications = 0;
	TestStepReference().GetIntFromConfig(aValidationSection, KItemAddedNotifications, itemAddedNotifications);
	aValidationData.SetNotificationsCount(TContactViewValidationData::EItemAddedNotifications, itemAddedNotifications);

	TInt itemRemovedNotifications = 0;
	TestStepReference().GetIntFromConfig(aValidationSection, KItemRemovedNotifications, itemRemovedNotifications);
	aValidationData.SetNotificationsCount(TContactViewValidationData::EItemRemovedNotifications, itemRemovedNotifications);

	TInt groupChangedNotifications = 0;
	TestStepReference().GetIntFromConfig(aValidationSection, KGroupChangedNotifications, groupChangedNotifications);
	aValidationData.SetNotificationsCount(TContactViewValidationData::EGroupChangedNotifications, groupChangedNotifications);

	TPtrC currentOperationTypeString;
	_LIT(KCurrentOperationType, "currentOperationType");
	TestStepReference().GetStringFromConfig(aValidationSection, KCurrentOperationType, currentOperationTypeString);
	aValidationData.SetCurrentOperation(currentOperationTypeString);

	TInt viewCount = 0;
	_LIT(KViewCount, "viewcount");
	TestStepReference().GetIntFromConfig(aValidationSection, KViewCount, viewCount);
	aValidationData.SetViewCount(viewCount);
	}


/**
 * Listen for contact view notifications and update the actual view validation information
 * @param aViewObserver  desired contact view observer
 * @param aActualData contact view validation object that will be updated with actual results
 */
void CContactViews::ListenForContactViewEvents(CContactViewEventQueue& aViewObserver,
 														TContactViewValidationData& aActualData)
	{
	const TInt KViewEventTimeOut = 20; // ms
	TContactViewEvent viewEvent;
	while(aViewObserver.ListenForEvent(KViewEventTimeOut,viewEvent))
         {
		 TContactViewValidationData::TContactViewNotificationType notificationType;
		 aActualData.NotificationType(viewEvent, notificationType);
		 aActualData.IncrementNotificationCount(notificationType);
         }
	}

/**
 * Compares the desired results against actual view update results
 * @param aDesiredData  desired view update information
 * @param aActualData actual view update information
 */
TBool CContactViews::Compare(TContactViewValidationData& aDesiredData, TContactViewValidationData& aActualData)
	{
	if(((aDesiredData.NotificationsCount(TContactViewValidationData::EUnAvailableNotifications)) != (aActualData.NotificationsCount(TContactViewValidationData::EUnAvailableNotifications)))
	|| ((aDesiredData.NotificationsCount(TContactViewValidationData::EViewReadyNotifications)) != (aActualData.NotificationsCount(TContactViewValidationData::EViewReadyNotifications)))
	|| ((aDesiredData.NotificationsCount(TContactViewValidationData::EViewSortOrderChangedNotifications))	!= (aActualData.NotificationsCount(TContactViewValidationData::EViewSortOrderChangedNotifications)))
	|| ((aDesiredData.NotificationsCount(TContactViewValidationData::EViewSortErrorNotifications))		!= (aActualData.NotificationsCount(TContactViewValidationData::EViewSortErrorNotifications)))
	|| ((aDesiredData.NotificationsCount(TContactViewValidationData::EContactServerErrorNotifications))	!= (aActualData.NotificationsCount(TContactViewValidationData::EContactServerErrorNotifications)))
	|| ((aDesiredData.NotificationsCount(TContactViewValidationData::EViewIndexingErrorNotifications))	!= (aActualData.NotificationsCount(TContactViewValidationData::EViewIndexingErrorNotifications)))
	|| ((aDesiredData.NotificationsCount(TContactViewValidationData::EItemAddedNotifications))		!= (aActualData.NotificationsCount(TContactViewValidationData::EItemAddedNotifications)))
	|| ((aDesiredData.NotificationsCount(TContactViewValidationData::EItemRemovedNotifications))		!= (aActualData.NotificationsCount(TContactViewValidationData::EItemRemovedNotifications)))
	|| ((aDesiredData.NotificationsCount(TContactViewValidationData::EGroupChangedNotifications))    != (aActualData.NotificationsCount(TContactViewValidationData::EGroupChangedNotifications)))
	||  (aDesiredData.ViewCount() != aActualData.ViewCount()))
		{
		return EFalse;
		}
	return ETrue;
	}

/**
 * Prints the desired results and actual view update results
 * @param aDesiredData  desired view update information
 * @param aActualData actual view update information
 */
void CContactViews::PrintDetailsL(TContactViewValidationData& aDesiredData, TContactViewValidationData& aActualData)
	{
	_LIT(KCurrentOperation, "The current CRUD operations being performed is ::::: %S \n");
	_LIT(KDesiredViewCount, "The desired view count %d \n");
	_LIT(KActualViewCount, "The actual view count is %d \n");
	_LIT(KDesiredViewUnAvailableNotifications, "The total number of desired view unavailable notifications are %d \n");
	_LIT(KActualViewUnAvailableNotifications,  "The total number of actual view unavailable notifications are %d \n");
	_LIT(KDesiredViewReadyNotifications, "The total number of desired view ready notifications are %d \n");
	_LIT(KActualViewReadyNotifications,  "The total number of actual view ready notifications are %d \n");
	_LIT(KDesiredViewSortOrderChangedNotifications, "The total number of desired view sort order changed notifications are %d \n");
	_LIT(KActualViewSortOrderChangedNotifications,  "The total number of actual view sort order changed notifications are %d \n");
	_LIT(KDesiredViewSortErrorNotifications, "The total number of desired View Sort ErrorNotifications are %d \n");
	_LIT(KActualViewSortErrorNotifications, "The total number of actual View Sort Error Notifications are %d \n");
	_LIT(KDesiredContactServerErrorNotifications, "The total number of desired Contact Server Error Notificationsare %d \n");
	_LIT(KActualContactServerErrorNotifications, "The total number of actual Contact Server Error Notifications are %d \n");
	_LIT(KDesiredViewIndexingErrorNotifications, "The total number of desired view indexing notifications are %d \n");
	_LIT(KActualViewIndexingErrorNotifications, "The total number of actual view indexing notifications are %d \n");
	_LIT(KDesiredItemAddedNotifications, "The total number of desired item added notifications are %d \n");
	_LIT(KActualItemAddedNotifications, "The total number of actual item added notifications are %d \n");
	_LIT(KDesiredItemRemovedNotifications, "The total number of desired item removed notifications are %d \n");
	_LIT(KActualItemRemovedNotifications, "The total number of actual item removed notifications are %d \n");
	_LIT(KDesiredGroupChangedNotifications, "The total number of desired group changed notifications are %d \n");
	_LIT(KActualGroupChangedNotifications, "The total number of actual group changed notifications are %d \n");

	TPtrC currentOperation(aDesiredData.CurrentOperationInfoInStringFormatL()); 
	TestStepReference().INFO_PRINTF2(KCurrentOperation, &currentOperation);

	TestStepReference().INFO_PRINTF2(KDesiredViewCount, aDesiredData.ViewCount());
	TestStepReference().INFO_PRINTF2(KActualViewCount, aActualData.ViewCount());

	TestStepReference().INFO_PRINTF2(KDesiredViewUnAvailableNotifications, aDesiredData.NotificationsCount(TContactViewValidationData::EUnAvailableNotifications));
	TestStepReference().INFO_PRINTF2(KActualViewUnAvailableNotifications, aActualData.NotificationsCount(TContactViewValidationData::EUnAvailableNotifications));

	TestStepReference().INFO_PRINTF2(KDesiredViewReadyNotifications, aDesiredData.NotificationsCount(TContactViewValidationData::EViewReadyNotifications));
	TestStepReference().INFO_PRINTF2(KActualViewReadyNotifications, aActualData.NotificationsCount(TContactViewValidationData::EViewReadyNotifications));

	TestStepReference().INFO_PRINTF2(KDesiredViewSortOrderChangedNotifications, aDesiredData.NotificationsCount(TContactViewValidationData::EViewSortOrderChangedNotifications));
	TestStepReference().INFO_PRINTF2(KActualViewSortOrderChangedNotifications, aActualData.NotificationsCount(TContactViewValidationData::EViewSortOrderChangedNotifications));

	TestStepReference().INFO_PRINTF2(KDesiredViewSortErrorNotifications, aDesiredData.NotificationsCount(TContactViewValidationData::EViewSortErrorNotifications));
	TestStepReference().INFO_PRINTF2(KActualViewSortErrorNotifications, aActualData.NotificationsCount(TContactViewValidationData::EViewSortErrorNotifications));

	TestStepReference().INFO_PRINTF2(KDesiredContactServerErrorNotifications, aDesiredData.NotificationsCount(TContactViewValidationData::EContactServerErrorNotifications));
	TestStepReference().INFO_PRINTF2(KActualContactServerErrorNotifications, aActualData.NotificationsCount(TContactViewValidationData::EContactServerErrorNotifications));

	TestStepReference().INFO_PRINTF2(KDesiredViewIndexingErrorNotifications, aDesiredData.NotificationsCount(TContactViewValidationData::EViewIndexingErrorNotifications));
	TestStepReference().INFO_PRINTF2(KActualViewIndexingErrorNotifications, aActualData.NotificationsCount(TContactViewValidationData::EViewIndexingErrorNotifications));


	TestStepReference().INFO_PRINTF2(KDesiredItemAddedNotifications, aDesiredData.NotificationsCount(TContactViewValidationData::EItemAddedNotifications));
	TestStepReference().INFO_PRINTF2(KActualItemAddedNotifications, aActualData.NotificationsCount(TContactViewValidationData::EItemAddedNotifications));

	TestStepReference().INFO_PRINTF2(KDesiredItemRemovedNotifications, aDesiredData.NotificationsCount(TContactViewValidationData::EItemRemovedNotifications));
	TestStepReference().INFO_PRINTF2(KActualItemRemovedNotifications, aActualData.NotificationsCount(TContactViewValidationData::EItemRemovedNotifications));

	TestStepReference().INFO_PRINTF2(KDesiredGroupChangedNotifications, aDesiredData.NotificationsCount(TContactViewValidationData::EGroupChangedNotifications));
	TestStepReference().INFO_PRINTF2(KActualGroupChangedNotifications, aActualData.NotificationsCount(TContactViewValidationData::EGroupChangedNotifications));
	}


/**
 * Class TContactViewValidationData constructor
 * Initializes all member variables
 */
TContactViewValidationData::TContactViewValidationData()
:iViewCount(0),
 iNumOfUnAvailableNotifications(0),
 iNumOfReadyNotifications(0),
 iNumOfSortOrderChangedNotifications(0),
 iNumOfSortErrorNotifications(0),
 iNumOfServerErrorNotifications(0),
 iNumOfIndexingErrorNotifications(0),
 iNumOfItemAddedNotifications(0),
 iNumOfItemRemovedNotifications(0),
 iNumOfGroupChangedNotifications(0),
 iCurrentOperation(TContactViewValidationData::ENone)
	{
	}

/**
 * The following set of functions belong to class TContactViewValidationData
 * Constructor is available to initialize the member variables
 * Get/Set functions are available to retrieve and update the TContactViewValidationData member
 * variables Functions are available to increment the relevant member variables
 * Necessary Conversion functions are also available
 */

/**
 * Set the desired view contact item count data
 */
void TContactViewValidationData::SetViewCount(TInt aViewCount)
	{
	iViewCount = aViewCount;
	}

/**
 * Get the view contact item count information
 */
TInt TContactViewValidationData::ViewCount()
	{
	return iViewCount;
	}

/**
 * Set the desired contact view operation based on information available in string format
 */
void TContactViewValidationData::SetCurrentOperation(const TPtrC& aCurrentOperation)
	{
	iCurrentOperationInStringFormat.Copy(aCurrentOperation);
	ConvertStringToTContactViewValidationDataFormat(aCurrentOperation);
	}

/**
 * Get the contact view operation information in string format
 */
TDesC& TContactViewValidationData::CurrentOperationInfoInStringFormatL()
	{
	return iCurrentOperationInStringFormat;
	}

/**
 * Set the desired contact view operation based on information available in TContactViewValidationData::TContactViewCurrentOperation format
 */
void TContactViewValidationData::SetCurrentOperation(TContactViewValidationData::TContactViewCurrentOperation aCurrentOperation)
	{
	iCurrentOperation = aCurrentOperation;
	}

/**
 * Get the contact view operation information in TContactViewValidationData::TContactViewCurrentOperation format
 */
TContactViewValidationData::TContactViewCurrentOperation TContactViewValidationData::CurrentOperation()
	{
	return iCurrentOperation;
	}

/**
 * Converts String to View Validation Internal Format
 * @param aCurrentOperation String containing information about current operation being performed
 */
void TContactViewValidationData::ConvertStringToTContactViewValidationDataFormat(const TPtrC& aCurrentOperation)
	{
	_LIT(KViewConstructionOperations,	"ViewConstructionOperations");
	_LIT(KAddContactOperations,			"AddContactOperations");
	_LIT(KRemoveContactsOperations,		"RemoveContactsOperations");
	_LIT(KUpdateContactOperations,		"UpdateContactOperations");
	_LIT(KSortOrderChangeOperations,	"SortOrderChangeOperations");
	_LIT(KServerRelatedOperations,		"ServerRelatedOperations");
	_LIT(KGroupChangedOperations,		"GroupChangedOperations");


	if ( aCurrentOperation.Compare(KViewConstructionOperations) == 0)
		{
		iCurrentOperation = TContactViewValidationData::EViewConstructionOperations;
		}
	else if ( aCurrentOperation.Compare(KAddContactOperations) == 0)
		{
		iCurrentOperation = TContactViewValidationData::EAddContactOperations;
		}
	else if ( aCurrentOperation.Compare(KRemoveContactsOperations) == 0)
		{
		iCurrentOperation = TContactViewValidationData::ERemoveContactsOperations;
		}
	else if ( aCurrentOperation.Compare(KUpdateContactOperations) == 0)
		{
		iCurrentOperation = TContactViewValidationData::EUpdateContactOperations;
		}
	else if ( aCurrentOperation.Compare(KSortOrderChangeOperations) == 0)
		{
		iCurrentOperation = TContactViewValidationData::ESortOrderChangeOperations;
		}
	else if ( aCurrentOperation.Compare(KServerRelatedOperations) == 0)
		{
		iCurrentOperation = TContactViewValidationData::EServerRelatedOperations;
		}
	else if ( aCurrentOperation.Compare(KGroupChangedOperations) == 0)
		{
		iCurrentOperation = TContactViewValidationData::EGroupChangedOperations;
		}
	}

/**
 * Convert TContactViewEvent to TContactViewValidationData::TContactViewNotificationType format
 * @param aEvent - Contact View Event information in TContactViewEvent format
 * @param aNotificationType - Contact View Event information in TContactViewValidationData::TContactViewNotificationType format
 */
 void TContactViewValidationData::NotificationType (TContactViewEvent aEvent, TContactViewValidationData::TContactViewNotificationType& aNotificationType)
	{
	if(aEvent.iEventType == TContactViewEvent::EUnavailable)
		{
		aNotificationType = 	TContactViewValidationData::EUnAvailableNotifications;
		}
	else if(aEvent.iEventType ==  TContactViewEvent::EReady)
		{
		aNotificationType = 	TContactViewValidationData::EViewReadyNotifications;
		}
	else if(aEvent.iEventType ==  TContactViewEvent::ESortOrderChanged )
		{
		aNotificationType = 	TContactViewValidationData::EViewSortOrderChangedNotifications;
		}
	else if(aEvent.iEventType ==  TContactViewEvent::ESortError)
		{
		aNotificationType = 	TContactViewValidationData::EViewSortErrorNotifications;
		}
	else if(aEvent.iEventType == TContactViewEvent::EServerError )
		{
		aNotificationType = 	TContactViewValidationData::EContactServerErrorNotifications;
		}
	else if(aEvent.iEventType == TContactViewEvent::EIndexingError)
		{
		aNotificationType = 	TContactViewValidationData::EViewIndexingErrorNotifications;
		}
	else if(aEvent.iEventType == TContactViewEvent::EItemAdded)
		{
		aNotificationType = 	TContactViewValidationData::EItemAddedNotifications;
		}
	else if(aEvent.iEventType == TContactViewEvent::EItemRemoved)
		{
		aNotificationType = 	TContactViewValidationData::EItemRemovedNotifications;
		}
	else if(aEvent.iEventType == TContactViewEvent::EGroupChanged)
		{
		aNotificationType= 	TContactViewValidationData::EGroupChangedNotifications;
		}
	}

 /**
  * Sets the total number of expected notifications of desired type
  * @param aNotificationType - Desired type of Contact View Event Notification
  * @param aNumOfExpectedNotifications - Total number of notifications expected
  */
  void TContactViewValidationData::SetNotificationsCount( TContactViewNotificationType aNotificationType,
 																				TInt aNumOfExpectedNotifications)
 	{
 	if(aNotificationType ==	TContactViewValidationData::EUnAvailableNotifications)
 		{
 		iNumOfUnAvailableNotifications = aNumOfExpectedNotifications;
 		}
 	else if(aNotificationType ==  TContactViewValidationData::EViewReadyNotifications)
 		{
 		iNumOfReadyNotifications = aNumOfExpectedNotifications;
 		}
 	else if(aNotificationType ==  TContactViewValidationData::EViewSortOrderChangedNotifications)
 		{
 		iNumOfSortOrderChangedNotifications = aNumOfExpectedNotifications;
 		}
 	else if(aNotificationType ==  TContactViewValidationData::EContactServerErrorNotifications)
 		{
 		iNumOfServerErrorNotifications = aNumOfExpectedNotifications;
 		}
 	else if(aNotificationType == TContactViewValidationData::EViewIndexingErrorNotifications)
 		{
 		iNumOfIndexingErrorNotifications = aNumOfExpectedNotifications;
 		}
 	else if(aNotificationType == TContactViewValidationData::EItemAddedNotifications)
 		{
 		iNumOfItemAddedNotifications = aNumOfExpectedNotifications;
 		}
 	else if(aNotificationType == TContactViewValidationData::EItemRemovedNotifications)
 		{
 		iNumOfItemRemovedNotifications = aNumOfExpectedNotifications;
 		}
 	else if(aNotificationType == TContactViewValidationData::EGroupChangedNotifications)
 		{
 		iNumOfGroupChangedNotifications = aNumOfExpectedNotifications;
 		}
 	}

 /**
  * Retrieve the expected notifications count
  * @param aNotificationType - Desired type of Contact View Event Notification
  * @return TInt - Count of number of notifications
  */
  TInt TContactViewValidationData::NotificationsCount(TContactViewNotificationType aNotificationType)
 	{
 	TInt numOfNotifications = 0;
 	if(aNotificationType ==	TContactViewValidationData::EUnAvailableNotifications)
 		{
 		numOfNotifications = iNumOfUnAvailableNotifications;
 		}
 	else if(aNotificationType ==  TContactViewValidationData::EViewReadyNotifications)
 		{
 		numOfNotifications = iNumOfReadyNotifications;
 		}
 	else if(aNotificationType ==  TContactViewValidationData::EViewSortOrderChangedNotifications)
 		{
 		numOfNotifications = iNumOfSortOrderChangedNotifications;
 		}
 	else if(aNotificationType ==  TContactViewValidationData::EContactServerErrorNotifications)
 		{
 		numOfNotifications = iNumOfServerErrorNotifications;
 		}
 	else if(aNotificationType == TContactViewValidationData::EViewIndexingErrorNotifications)
 		{
 		numOfNotifications = iNumOfIndexingErrorNotifications;
 		}
 	else if(aNotificationType == TContactViewValidationData::EItemAddedNotifications)
 		{
 		numOfNotifications = iNumOfItemAddedNotifications;
 		}
 	else if(aNotificationType == TContactViewValidationData::EItemRemovedNotifications)
 		{
 		numOfNotifications = iNumOfItemRemovedNotifications;
 		}
 	else if(aNotificationType == TContactViewValidationData::EGroupChangedNotifications)
 		{
 		numOfNotifications = iNumOfGroupChangedNotifications;
 		}
 	return numOfNotifications;
 	}

/**
 * Increment the expected notifications count
 * @param aNotificationType - Desired type of Contact View Event Notification
 */
 void TContactViewValidationData::IncrementNotificationCount
				(TContactViewValidationData::TContactViewNotificationType aNotificationType)
	{
	if(aNotificationType ==	TContactViewValidationData::EUnAvailableNotifications)
		{
		++iNumOfUnAvailableNotifications;
		}
	else if(aNotificationType ==  TContactViewValidationData::EViewReadyNotifications)
		{
		++iNumOfReadyNotifications;
		}
	else if(aNotificationType ==  TContactViewValidationData::EViewSortOrderChangedNotifications)
		{
		++iNumOfSortOrderChangedNotifications;
		}
	else if(aNotificationType ==  TContactViewValidationData::EContactServerErrorNotifications)
		{
		++iNumOfServerErrorNotifications;
		}
	else if(aNotificationType == TContactViewValidationData::EViewIndexingErrorNotifications)
		{
		++iNumOfIndexingErrorNotifications;
		}
	else if(aNotificationType == TContactViewValidationData::EItemAddedNotifications)
		{
		++iNumOfItemAddedNotifications;
		}
	else if(aNotificationType == TContactViewValidationData::EItemRemovedNotifications)
		{
		++iNumOfItemRemovedNotifications;
		}
	else if(aNotificationType == TContactViewValidationData::EGroupChangedNotifications)
		{
		++iNumOfGroupChangedNotifications;
		}
	}


