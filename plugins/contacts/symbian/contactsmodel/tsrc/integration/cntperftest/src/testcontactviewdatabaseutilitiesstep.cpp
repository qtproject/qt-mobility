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


#include "testcontactviewdatabaseutilitiesstep.h"

/**
 * Test Step Destructor
 */
CTestContactViewDatabaseUtilitiesStep::~CTestContactViewDatabaseUtilitiesStep()
	{
	if(iContactUtility)
		{
		delete iContactUtility;
		}
	if(iContactViewCollection)
		{
		delete iContactViewCollection;
		}
	if(iDb)
		{
		delete iDb;
		}
	if(iScheduler)
		{
		delete iScheduler;
		}
	}

/**
 * Test Step preamble function, setups the environment required for the test step
 */
TVerdict CTestContactViewDatabaseUtilitiesStep::doTestStepPreambleL()
	{
	iPimTestServer.Connect();
	CopyDatabaseL();
	InstallActiveSchedularL();
	OpenDataBaseL();
	SetupUtilityL();
	return TestStepResult();
	}

/**
 * Test Step postamble function, part of cleanup activity for the test step
 */
TVerdict CTestContactViewDatabaseUtilitiesStep::doTestStepPostambleL()
	{
	iPimTestServer.Close();
	CloseDatabaseL();
	UnInstallActiveSchedularL();
	return TestStepResult();
	}


/**
 * Installs active schedular in the current thread
 */
void CTestContactViewDatabaseUtilitiesStep::InstallActiveSchedularL()
	{
	iScheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(iScheduler);
	}

/**
 * Allocates necessary utility objects
 */
void CTestContactViewDatabaseUtilitiesStep::SetupUtilityL()
	{
	iContactViewCollection = CContactViewCollection::NewL();
	CTestStep* self = static_cast<CTestStep*>(this);
	iContactUtility = CContactUtilitiesCollection::NewL(*self, DatabaseReference(), ViewCollectionReference());
	}

/**
 * Opens the desired database file based on ini data mentioned in the ini file
 */
void CTestContactViewDatabaseUtilitiesStep::OpenDataBaseL()
	{
	TBool	createDataBase = EFalse;
	TPtrC dbName;
	GetBoolFromConfig(ConfigSection(),   SharedConstants::KCreateDataBase, createDataBase);
	GetStringFromConfig(ConfigSection(), SharedConstants::KDbName, dbName);
	iDb = CreateAndOpenDataBaseL(dbName, createDataBase);
	}

/**
 * Create or open the contact database
 * @param aDbName database to be created/opened
 * @return CContactDatabase*
 */
CContactDatabase* CTestContactViewDatabaseUtilitiesStep::CreateAndOpenDataBaseL(const TPtrC& aDbName, TBool aCreateDataBase)
	{
	HBufC*	dbName = HBufC::NewLC(aDbName.Length());
	dbName->Des().Copy(aDbName);
	CContactDatabase*	cntDb = NULL;
	if( aCreateDataBase )
		{
		TInt	err = 0;
		// Replace the existing database and opens it
		if( aDbName != KNullDesC() )
			{
			TRAP(err, cntDb = CContactDatabase::ReplaceL(dbName->Des()));
			}
		else //if database name is not given then create and open the default DB
			{
			TRAP(err, cntDb = CContactDatabase::ReplaceL());
			}

		if( err != KErrNone )
			{
			ERR_PRINTF2(KErrInCreateDataBase, err);
			SetTestStepResult(EFail);
			SetTestStepError(err);
			}
		}
	else
		{
		TRAPD(err, cntDb = CContactDatabase::OpenL(dbName->Des()));
		if( err != KErrNone )
			{
			ERR_PRINTF2(KErrInOpen, err);
			SetTestStepResult(EFail);
			SetTestStepError(err);
			}
		}
	CleanupStack::PopAndDestroy(dbName);
	return	cntDb;
	}

/**
 * Constructs the desired contact views based on the data specified in the ini file
 */
void CTestContactViewDatabaseUtilitiesStep::ConstructViewsL()
	{
	TPtrC	listOfViews;
	GetStringFromConfig(ConfigSection(), SharedConstants::KListOfViews, listOfViews);
	RArray<TPtrC>	sections;
	CleanupClosePushL(sections);
	iContactUtility->TokenizeStringL(listOfViews, sections);
	iContactUtility->ConstructViewsL(sections);
	CleanupStack::PopAndDestroy(&sections);
	}

/**
 * Retrieves the desired contact view observer and listens for contact view notifications
 * @param aEvent - A TContactViewEvent object that will be updated with the notification received
 * @return TBool - An indication whether notification was received or not
 */
TBool CTestContactViewDatabaseUtilitiesStep::ListenForViewEventsL(TContactViewEvent& aEvent)
	{
	const TInt KNotificationTimeout = 10; // ms
	TInt desiredViewIndex = 0;
	_LIT(KDesiredViewIndex, "DesiredViewIndex");
	GetIntFromConfig(ConfigSection(), KDesiredViewIndex, desiredViewIndex);

	TPtrC desiredViewType;
	_LIT(KDesiredViewType, "DesiredViewType");
	GetStringFromConfig(ConfigSection(), KDesiredViewType, desiredViewType);

	CContactViewEventQueue&	contactViewObserver =
		ViewCollectionReference().GetDesiredViewObserver(desiredViewType, desiredViewIndex);

    return (contactViewObserver.ListenForEvent(KNotificationTimeout,aEvent));
	}

/* Here we are trying add blank groups in the database
 * Say some 1000 thousand groups are added to the database
 */
void CTestContactViewDatabaseUtilitiesStep::AddGroupsInDatabaseL()
	{
	_LIT(KNumOfGroups, "numofgroups");
	TInt numOfGroups = 0;
	GetIntFromConfig(ConfigSection(), KNumOfGroups, numOfGroups);

	for(TInt i = 0; i < numOfGroups; ++i)
		{
		CContactItem* contactGroup = DatabaseReference().CreateContactGroupL();
		delete contactGroup;
		}
	}


/* In this section, we edit the desired number of groups in the database
 * Set label for them and associate them with desired number of groups
 * With this database updation, group views can be constructed and accessed
 */
void CTestContactViewDatabaseUtilitiesStep::IterateThroAllGroupSectionsAndUpdateL()
	{
	_LIT(KListOfGroupsSectionsString, "listofgroupsections");
	TPtrC listOfGroupsSectionsString;
	GetStringFromConfig(ConfigSection(), KListOfGroupsSectionsString, listOfGroupsSectionsString);

	RArray<TPtrC>	listOfGroupsSections;
	CleanupClosePushL(listOfGroupsSections);
	ViewUtilityReference().TokenizeStringL(listOfGroupsSectionsString, listOfGroupsSections);

	CCntFilter* filter = CCntFilter::NewL();
	CleanupStack::PushL(filter);
	filter->SetContactFilterTypeGroup(ETrue);
	DatabaseReference().FilterDatabaseL(*filter);

	for ( TInt i = 0; i < listOfGroupsSections.Count(); ++i )
		{
		CContactIdArray* idArray = filter->iIds;
		TContactItemId groupId = (*idArray)[i];
		CContactItem* group = DatabaseReference().OpenContactL(groupId);
		CleanupStack::PushL(group);
		CContactGroup* newGroup = static_cast<CContactGroup*>(group);
		UpdateGroupsL(listOfGroupsSections[i], *newGroup);
		DatabaseReference().CommitContactL(*group);
		DatabaseReference().CloseContactL(groupId);
		CleanupStack::PopAndDestroy(group);
		}

	CleanupStack::PopAndDestroy(2, &listOfGroupsSections);
	}


/**
 * Updates the specified group with details like group name, number of contacts in the group
 * @param aGroupSection - section in the ini file contains necessary details for group update
 * @param aGroup - Group to be updated
 */
void CTestContactViewDatabaseUtilitiesStep::UpdateGroupsL(const TPtrC& aGroupSection, CContactGroup& aGroup)
	{
	_LIT(KGroupName, "groupname");
	TPtrC groupName;
	GetStringFromConfig(aGroupSection, KGroupName, groupName);

	aGroup.SetGroupLabelL(groupName);

	_LIT(KNumOfContacts, "numofcontacts");
	TInt numOfContacts;
	GetIntFromConfig(aGroupSection, KNumOfContacts, numOfContacts);

	CCntFilter* filter = CCntFilter::NewL();
	CleanupStack::PushL(filter);
	filter->SetContactFilterTypeCard(ETrue);
	DatabaseReference().FilterDatabaseL(*filter);

	for(TInt i = 0; i < numOfContacts; ++i)
		{
		CContactIdArray* idArray = filter->iIds;
		TContactItemId contactItemId = (*idArray)[i];
		CContactItem* contactItem = DatabaseReference().OpenContactL(contactItemId);
		CleanupStack::PushL(contactItem);
		TRAP_IGNORE(DatabaseReference().RemoveContactFromGroupL(*contactItem, aGroup));
		TRAP_IGNORE(DatabaseReference().AddContactToGroupL(*contactItem, aGroup));
		CleanupStack::PopAndDestroy(contactItem);
		DatabaseReference().CloseContactL(contactItemId);
		}

	CleanupStack::PopAndDestroy(filter);
	}


/**
 * Closes the relevant contacts database file
 */
void CTestContactViewDatabaseUtilitiesStep::CloseDatabaseL()
	{
	if(iContactUtility)
		{
		delete iContactUtility;
		iContactUtility = NULL;
		}
	if(iContactViewCollection)
		{
		delete iContactViewCollection;
		iContactViewCollection = NULL;
		}
	if(iDb)
		{
		delete iDb;
		iDb=NULL;
		}
	}

/**
 * Uninstall scheduler in the current thread
 */
void CTestContactViewDatabaseUtilitiesStep::UnInstallActiveSchedularL()
	{
	if(iScheduler)
		{
		delete iScheduler;
		iScheduler = NULL;
		}
	}

/**
 * Handle to the current database in operation
 * @return CContactDatabase& - Reference to current database in operation
 */
CContactDatabase& CTestContactViewDatabaseUtilitiesStep::DatabaseReference()
	{
	return *iDb;
	}

/**
 * Handle to the contact view collection
 * @return CContactViewCollection& - Reference to current contact view collection
 */
CContactViewCollection& CTestContactViewDatabaseUtilitiesStep::ViewCollectionReference()
	{
	return *iContactViewCollection;
	}

/**
 * Handle to the contact view utilities
 * @return CContactUtilitiesCollection& - Reference to current contact view utilities collection
 */
CContactUtilitiesCollection& CTestContactViewDatabaseUtilitiesStep::ViewUtilityReference()
	{
	return *iContactUtility;
	}

/**
 * Handle to the Pim Test Server
 * @return RPIMTestServer& - Reference to Pim Test Server
 */
RPIMTestServer& CTestContactViewDatabaseUtilitiesStep::PimTestServerReference()
	{
	return iPimTestServer;
	}

/**
 * Copies database files across folders
 */
void CTestContactViewDatabaseUtilitiesStep::CopyDatabaseL()
	{
	_LIT(KSourcePath, "sourcepath");
	TPtrC sourcePathString;
	GetStringFromConfig(ConfigSection(), KSourcePath, sourcePathString);

	_LIT(KDestinationPath, "destinationpath");
	TPtrC destinationPathString;
	GetStringFromConfig(ConfigSection(), KDestinationPath, destinationPathString);

	if(sourcePathString != KNullDesC && destinationPathString != KNullDesC)
		{
   		TRAP_IGNORE(PimTestServerReference().CopyFileL(sourcePathString, destinationPathString));
		}
	}
