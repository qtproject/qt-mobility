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




#ifndef __CONTACTVIEWS_THREAD_H__
#define __CONTACTVIEWS_THREAD_H__

// System Includes
#include <testexecutestepbase.h>
#include <cntdb.h>
#include <cntitem.h>
#include <cntviewbase.h>
#include <cntfldst.h>
#include <cntfilt.h>
#include <e32math.h>

// User Includes
#include "contactutilitiescollection.h"
#include "clientserver.h"
#include "testcontactviewdatabaseutilitiesstep.h"

// Literals used
_LIT(KChunkName, 	 "ContactViewsChunk");
_LIT(KSemaphoreName, "ContactViewsSemaphore");

// Global Methods
GLDEF_C TInt RunContactViewsThreadL(TAny* aArgs);

/**
 * This class facilitates construction of contact view validation objects, used for verifying the
 * behaviour of contact views when CRUD operations are performed on the contacts database. Basically
 * information regarding different contact CRUD Operations performed and expected view behaviour in terms of
 * view count, view notifications are specified in the ini file. These details are validated against
 * actual results.
 */
class TContactViewValidationData
	{
	public:

		enum TContactViewNotificationType
			{
 			EUnAvailableNotifications,
			EViewReadyNotifications,
			EViewSortOrderChangedNotifications,
			EViewSortErrorNotifications,
			EContactServerErrorNotifications,
			EViewIndexingErrorNotifications,
			EItemAddedNotifications,
			EItemRemovedNotifications,
			EGroupChangedNotifications,
			};

		enum TContactViewCurrentOperation
			{
			EViewConstructionOperations,
			EAddContactOperations,
			ERemoveContactsOperations,
			EUpdateContactOperations,
			ESortOrderChangeOperations,
			EServerRelatedOperations,
			EGroupChangedOperations,
			ENone,
			};

		TContactViewValidationData();

		// Increment view notification functions are useful while incrementing the
		// TContactViewNotificationGroup member varibales i.e TContactViewNotificationGroup object
		// the member variables as and when notifications are recieved by the view observers
		void IncrementNotificationCount(TContactViewNotificationType aContactViewNotificationType);

		// Get View Notification functions are useful while comparing the notifications received against expected
		// notification information specified in the ini file
		TInt NotificationsCount(TContactViewNotificationType aContactViewNotificationType);

		// Set View Notification functions are useful while updating TContactViewNotificationGroup objects
		// with data read from the ini file
		void SetNotificationsCount(TContactViewNotificationType aContactViewNotificationType,
																 TInt aNumOfExpectedNotifications);																
		void NotificationType (TContactViewEvent aEvent,
										 TContactViewValidationData::TContactViewNotificationType& aNotificationType);																
		void SetCurrentOperation(const TPtrC& aCurrentOperation);
		void SetCurrentOperation(TContactViewValidationData::TContactViewCurrentOperation aCurrentOperation);				
		TDesC& TContactViewValidationData::CurrentOperationInfoInStringFormatL();
		TContactViewValidationData::TContactViewCurrentOperation CurrentOperation();				
		void ConvertStringToTContactViewValidationDataFormat(const TPtrC& aCurrentOperation);		
		void SetViewCount(TInt aViewCount);
		TInt ViewCount();

		

	private:
		TInt iViewCount;
 		TInt iNumOfUnAvailableNotifications;
 		TInt iNumOfReadyNotifications;
 		TInt iNumOfSortOrderChangedNotifications;
 		TInt iNumOfSortErrorNotifications;
 		TInt iNumOfServerErrorNotifications;
 		TInt iNumOfIndexingErrorNotifications;
 		TInt iNumOfItemAddedNotifications;
 		TInt iNumOfItemRemovedNotifications;
 		TInt iNumOfGroupChangedNotifications;
		TContactViewValidationData::TContactViewCurrentOperation iCurrentOperation;
		TBuf<256>  	iCurrentOperationInStringFormat;		

	};


/**
 * This class facilitates construction of desired contact views based on the ini data. Listen for contact view
 * notifications, validate the contact view count at the end of each CRUD Operation. 
 */
class CContactViews : public CBase
	{
	public:
		static 		CContactViews*  NewL(CTestStep& aTestStep);
		void 		RunTestL();
					~CContactViews();

	private:
		CContactViews(CTestStep& aTestStep);
		void 		ConstructL();		
		void 		InstallActiveSchedularL();
		void 		UnInstallActiveSchedularL();
		void 		OpenDatabaseL();
		void 		CloseDatabaseL();
		void 		SetupChunk();
		void 		SetCurrentOperation(TContactViewValidationData& aValidationData);		
		void 		ReadDataFromIniAndConstructValidationObjectsL();
		void 		IterateThroValidationSectionAndUpdateValidationData(const TPtrC& aValidationSection, TContactViewValidationData& aValidationData);
		void 		ConstructViewsL();		
		void 		SetViewOfInterest();
 		void		ListenForContactViewEvents(CContactViewEventQueue& aViewObserver, TContactViewValidationData& aActualData);
		void 		UpdateActualNotificationCollectionObjectL (TContactViewEvent aEvent, TContactViewValidationData& aActualData);
		TBool 		Compare(TContactViewValidationData& aDesiredData, TContactViewValidationData& aActualData);
		TPtrC& 		SemaphoreName();
		CTestStep&  TestStepReference();
		TBool 		ExecutionCompleted();
		void 		PrintDetailsL(TContactViewValidationData& aDesiredData, TContactViewValidationData& aActualData);		
		CContactDatabase& DatabaseReference();
		CContactViewCollection& ViewCollectionReference();
		CContactViewEventQueue& RetrieveDesiredViewObserverL();
		TContactViewValidationData RetrieveValidationData(TContactViewValidationData::TContactViewCurrentOperation aCurrentOperation);		
			
	private:
		CTestStep& 													iTestStep;
		TBool*														iExecutionCompleted;
		CContactUtilitiesCollection*								iContactUtility;
		CContactDatabase* 											iDb;
		CContactViewCollection* 									iContactViewCollection;
		CActiveScheduler* 											iScheduler;
		CContactViewBase* 											iContactView;
		RSemaphore													iSemaphore;
		RArray<TContactViewValidationData> 							iExpectedData;
		RArray<TContactViewValidationData> 							iActualData;
		RChunk														iChunk;		
		TPtrC*														iCurrentOperation;
	};
	
#endif
