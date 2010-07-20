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


#include "testcontactviewcrudoperationsstep.h"

/**
 * Test Step Constructor
 */
CTestContactViewCRUDOperationsStep::CTestContactViewCRUDOperationsStep()
:CTestContactViewDatabaseUtilitiesStep()
	{
	// Mandatory call to base class method to set up the human readable name for logging.
	SetTestStepName(KTestContactViewCRUDOperationsStep);
	}

/**
 * Test Step Destructor
 */
CTestContactViewCRUDOperationsStep::~CTestContactViewCRUDOperationsStep()
	{
	iChunk.Close();
	iThread.Close();
	iSemaphore.Close();
	}

/**
 * Test Step preamble function, setups the environment required for the test step
 */
TVerdict CTestContactViewCRUDOperationsStep::doTestStepPreambleL()
	{
	CTestContactViewDatabaseUtilitiesStep::doTestStepPreambleL();
	iChunk.CreateGlobal(KChunkName(), sizeof(TBool) + sizeof(TPtrC), sizeof(TBool) + sizeof(TPtrC));
	iChunk.OpenGlobal(KChunkName(), ETrue);
	iExecutionCompleted = reinterpret_cast<TBool*>(iChunk.Base());
	iCurrentOperation = sizeof(TBool) + reinterpret_cast<TPtrC*>(iChunk.Base());
	*iExecutionCompleted = EFalse;
	return TestStepResult();
	}

/**
 * Test Step postamble function, part of cleanup activity for the test step
 */
TVerdict CTestContactViewCRUDOperationsStep::doTestStepPostambleL()
	{
	CTestContactViewDatabaseUtilitiesStep::doTestStepPostambleL();
	return TestStepResult();
	}

/**
* Base class pure virtual.
* @return EPass or EFail indicating the result of the test step.
*/
TVerdict CTestContactViewCRUDOperationsStep::doTestStepL()
	{
	// Share the current teststep logger across threads
	Logger().ShareAuto();

	// Starts a separate contact views thread and reumes the same
	LaunchContactViewsThreadL();
	ResumeContactViewsThreadL();

	CreateContactItemsL();
	UpdateContactItemsL();
	RemoveContactItemsL();

	// Mark the execution completed flag to true and thereby stop the contact views thread
	SetExecutionCompleted();
	ResumeContactViewsThreadL();
	return TestStepResult();
	}


/**
* Adds contact cards and groups in the database
*/
void CTestContactViewCRUDOperationsStep::CreateContactItemsL()
	{
	_LIT(KAddContacts, "add");
	TBool addContacts = EFalse;
	GetBoolFromConfig(ConfigSection(), KAddContacts, addContacts);

	if(!addContacts)
		{
		return;
		}

	// Add contacts and groups in the main thread and validate the view count and view notifications
	// in the contact views thread
	TPtrC currentOperation(KAddContactOperations);
	SetCurrentOperation(currentOperation);
	AddContactEntriesL();
	CloseDatabaseL();
	OpenDataBaseL();
	ResumeContactViewsThreadL();
	}

/**
* Updates contact cards and groups in the database
*/
void CTestContactViewCRUDOperationsStep::UpdateContactItemsL()
	{
	_LIT(KUpdateContacts, "update");
	TBool updateContacts = EFalse;
	GetBoolFromConfig(ConfigSection(), KUpdateContacts, updateContacts);

	if(!updateContacts)
		{
		return;
		}


	// Update contacts in the main thread and validate the view count and view notifications
	// in the contact views thread
	TPtrC currentOperation(KUpdateContactOperations);
	SetCurrentOperation(currentOperation);
	UpdateContactEntriesL();
	CloseDatabaseL();
	OpenDataBaseL();
	ResumeContactViewsThreadL();
	}

/**
* Deletes contact items in the database
*/
void CTestContactViewCRUDOperationsStep::RemoveContactItemsL()
	{
	_LIT(KDeleteContacts, "delete");
	TBool deleteContacts = EFalse;
	GetBoolFromConfig(ConfigSection(), KDeleteContacts, deleteContacts);

	if(!deleteContacts)
		{
		return;
		}

	// Delete Contacts in the main thread and validate the view count and view notifications
	// in the contact views thread
	TPtrC currentOperation(KRemoveContactsOperations);
	SetCurrentOperation(currentOperation);
	DeleteContactCardsL();
	CloseDatabaseL();
	OpenDataBaseL();
	ResumeContactViewsThreadL();
	}


/*
* Creates a Contact Views Thread. This thread constructs the desired contact views
* After every CRUD operation performed on the database, the contact views thread
* validates the updated view count and view notifications
*/
void CTestContactViewCRUDOperationsStep::LaunchContactViewsThreadL()
	{

	TInt error = iSemaphore.CreateGlobal(KSemaphoreName, 0);

	if (error == KErrAlreadyExists || error == KErrNone)
		{
		iSemaphore.OpenGlobal(KSemaphoreName);
		}
	else
		{
		User::Leave(error);
		}

	HBufC* contactViewsThread = HBufC::NewLC(SharedConstants::KMaxBufferLength);
	TPtr contactViewsThreadHandle = contactViewsThread->Des();

	SetRandomAlphaString(contactViewsThreadHandle, SharedConstants::KMaxBufferLength);
    User::LeaveIfError( iThread.Create(contactViewsThreadHandle, RunContactViewsThreadL, KDefaultStackSize, NULL, this, EOwnerProcess));
    _LIT(KInfoContactsViewThread, "The contact views thread was created successfully");
   	INFO_PRINTF1(KInfoContactsViewThread);

	CleanupStack::PopAndDestroy(contactViewsThread);

	}

/**
* Resumes the contact views thread
*/
void CTestContactViewCRUDOperationsStep::ResumeContactViewsThreadL()
	{
	iThread.Resume();
	iSemaphore.Wait();
	}

/**
* New contact entries are added in the database. The newly added contacts can have fields that meet the sort order
* of some existing views, With the addition of such contact fields, the existing views needs resorting. The newly added
* fields may also meet the filter criteria of some existing views. This is useful while testing filtered views
* The new added contacts can have fields with some predefined string content, this is useful in case of Find Views and Sub Views.
*/
void CTestContactViewCRUDOperationsStep::AddContactEntriesL()
	{
	_LIT(KViewSortOrder, "SortOrder");
	TPtrC viewSortOrderString;
	GetStringFromConfig(ConfigSection(), KViewSortOrder, viewSortOrderString);

	RContactViewSortOrder sortOrder = ViewUtilityReference().ConvertStringToSortOrderL(viewSortOrderString);
	CleanupClosePushL(sortOrder);

	_LIT(KNumOfContactsToBeAdded, "NumOfContactsToBeAdded");
	TInt numOfContactsToBeAdded;
	GetIntFromConfig(ConfigSection(), KNumOfContactsToBeAdded, numOfContactsToBeAdded);


	for(TInt i = 0; i < numOfContactsToBeAdded; ++i)
		{
		CContactCard* contactCard = CContactCard::NewL();
		CleanupStack::PushL(contactCard);
		AddContactFieldL(*contactCard, sortOrder);

		TBool filterBasedFields;
		_LIT(KFilterBasedFields, "FilterBasedFields");
		GetBoolFromConfig(ConfigSection(), KFilterBasedFields, filterBasedFields);
		if(filterBasedFields)
			{
			AddFieldsSpecificToFilterL(*contactCard);
			}

		TBool contactsWithDesiredString;
		_LIT(KContactsWithDesiredString, "ContactsWithDesiredString");
		GetBoolFromConfig(ConfigSection(), KContactsWithDesiredString, contactsWithDesiredString);
		if(contactsWithDesiredString)
			{
			AddMatchingStringToContactL(*contactCard);
			}


		TContactItemId contactId = DatabaseReference().AddNewContactL(*contactCard);

		TPtrC addContactToGroup;
		_LIT(KAddContactToGroup, "grouplist");
		GetStringFromConfig(ConfigSection(), KAddContactToGroup, addContactToGroup);
		if(addContactToGroup != KNullDesC)
			{
			IterateThroAllGroupSectionsAndUpdateContactL(addContactToGroup, *contactCard);
			}

		CleanupStack::PopAndDestroy();// contactCard
		DatabaseReference().CloseContactL(contactId);
		}
	CleanupStack::PopAndDestroy(); // sortOrder
	}

/**
* Add fields that meet some desired sort order
* @param aContact - Contact to be updated woth desired fields
* @param aSortOrder - specifies information about the desired fields
*/
void CTestContactViewCRUDOperationsStep::AddContactFieldL(CContactItem& aContact,
															 const RContactViewSortOrder& aSortOrder)
	{
	const TInt KMaxSortOrderCount = 3;
	for(TInt i = 0; i < KMaxSortOrderCount; ++i)
		{
		TFieldType fieldType = aSortOrder[i];
		TStorageType storageType = GetStorageType(fieldType);
		CContentType* contentType = CContentType::NewL();
		CleanupStack::PushL(contentType);
		contentType->AddFieldTypeL(fieldType);
		CContactItemField* field = 	CContactItemField::NewL(storageType, *contentType);
		CleanupStack::PushL(field);
		SetDataL(fieldType, *field);
		aContact.AddFieldL(*field);
		CleanupStack::Pop(field);
		CleanupStack::PopAndDestroy(contentType);
		}
	}

/**
 * Adds contact fields to the contact, as per the desired contact view filter
 */
void CTestContactViewCRUDOperationsStep::AddFieldsSpecificToFilterL(CContactItem& aContact)
	{
	_LIT(KDesiredFilterString, "desiredFilter");
	TPtrC desiredFilterString;
	GetStringFromConfig(ConfigSection(), KDesiredFilterString, desiredFilterString);
	CContactDatabase::TContactViewFilter	viewFilter = ViewUtilityReference().GetContactViewFilterL(desiredFilterString);
	AddFilterBasedFieldsL(aContact, viewFilter);
	}

/**
 * Updates the specified contact items with desired matching string
 */

void CTestContactViewCRUDOperationsStep::AddMatchingStringToContactL(CContactItem& aContact)
	{
	_LIT(KDesiredMatchingString, "matchingstring");
	TPtrC desiredMatchingString;
	GetStringFromConfig(ConfigSection(), KDesiredMatchingString, desiredMatchingString);

	_LIT(KDesiredContactField, "contactfield");
	TPtrC desiredContactField;
	GetStringFromConfig(ConfigSection(), KDesiredContactField, desiredContactField);

	TUid uidInfo = GetContactFieldType(desiredContactField);

	CContactItemFieldSet& fieldSet = aContact.CardFields();
	TInt pos = fieldSet.Find(uidInfo);
	if(pos == KErrNotFound)
		{
		TFieldType fieldType = uidInfo;
		TStorageType storageType = GetStorageType(fieldType);
		CContentType* contentType = CContentType::NewL();
		CleanupStack::PushL(contentType);

		contentType->AddFieldTypeL(fieldType);
		CContactItemField* field = 	CContactItemField::NewL(storageType, *contentType);
		CleanupStack::PushL(field);
		field->TextStorage()->SetTextL(desiredMatchingString);
		aContact.AddFieldL(*field);
		CleanupStack::Pop(field);

		CleanupStack::PopAndDestroy(contentType);
		}
	else
		{
		CContactItemField& field = fieldSet[pos];
		field.TextStorage()->SetTextL(desiredMatchingString);
		}
	}

/**
 * Updates the specified number of contact items from the database
 */
void CTestContactViewCRUDOperationsStep::UpdateContactEntriesL()
	{
	_LIT(KNumOfContactsToBeUpdated, "NumOfContactsToBeUpdated");
	TInt numOfContactToBeUpdated;
	GetIntFromConfig(ConfigSection(), KNumOfContactsToBeUpdated, numOfContactToBeUpdated);

	TPtrC deleteContactInGroups;
	_LIT(KDeleteContactInGroups, "grouplist");
	GetStringFromConfig(ConfigSection(), KDeleteContactInGroups, deleteContactInGroups);


	if(deleteContactInGroups != KNullDesC)
		{
		UpdateContactsInGroupL(numOfContactToBeUpdated, deleteContactInGroups);
		}
	else
		{
		CCntFilter* filter = CCntFilter::NewL();
		CleanupStack::PushL(filter);
		filter->SetContactFilterTypeCard(ETrue);
		DatabaseReference().FilterDatabaseL(*filter);

		for(TInt i = 0; i < numOfContactToBeUpdated; ++i)
			{
			TContactItemId contactItemId = (*filter->iIds)[i];
			CContactItem* contactItem = DatabaseReference().ReadContactL(contactItemId);
			CleanupStack::PushL(contactItem);
			UpdateFieldsL(*contactItem);
			CContactItem* updatedContactItem = DatabaseReference().UpdateContactLC(contactItemId, contactItem);
			CleanupStack::PopAndDestroy(updatedContactItem);
			CleanupStack::PopAndDestroy(contactItem);
			}

		CleanupStack::PopAndDestroy(filter);
		}
	}

/**
 * Deletes the specified number of contact cards from the database
 */
void CTestContactViewCRUDOperationsStep::DeleteContactCardsL()
	{
	_LIT(KNumOfContactsToBeDeleted, "NumOfContactsToBeDeleted");
	TInt numOfContactsToBeDeleted;
	GetIntFromConfig(ConfigSection(), KNumOfContactsToBeDeleted, numOfContactsToBeDeleted);


	TPtrC deleteContactInGroups;
	_LIT(KDeleteContactInGroups, "grouplist");
	GetStringFromConfig(ConfigSection(), KDeleteContactInGroups, deleteContactInGroups);

	if(deleteContactInGroups != KNullDesC)
		{
		DeleteContactsInGroupL(numOfContactsToBeDeleted, deleteContactInGroups);
		}
	else
		{
		CCntFilter* filter = CCntFilter::NewL();
		CleanupStack::PushL(filter);
		filter->SetContactFilterTypeCard(ETrue);
		DatabaseReference().FilterDatabaseL(*filter);

		for(TInt i = 0; i < numOfContactsToBeDeleted; ++i)
			{
			TContactItemId contactItemId = (*filter->iIds)[i];
			DatabaseReference().DeleteContactL(contactItemId);
			}

		CleanupStack::PopAndDestroy(filter);
		}
	}



/**
* Sets the program completion flag to True, thereby enables the
* contact views thread to release all its resources
*/
void CTestContactViewCRUDOperationsStep::SetExecutionCompleted()
	{
	*iExecutionCompleted = ETrue;
	}

/*
* Sets information about the current operation being performed
* This information is used by the contact views thread to validate the
* view count and view notifications at the end of each CRUD Operations
*/
void CTestContactViewCRUDOperationsStep::SetCurrentOperation
		(TPtrC aCurrentOperation)
	{
	(*iCurrentOperation).Set(aCurrentOperation);
	}


/* In this section, we edit the desired number of groups in the database
 * Set label for them and associate them with contacts
 @ param aGroupSection - Desired Group Name
 @ param aContact - Contact to be updated
 */
void CTestContactViewCRUDOperationsStep::IterateThroAllGroupSectionsAndUpdateContactL(const TPtrC& aGroupSection,
																									 CContactCard& aContact)
	{
	RArray<TPtrC>	listOfGroupsSections;
	CleanupClosePushL(listOfGroupsSections);
	ViewUtilityReference().TokenizeStringL(aGroupSection, listOfGroupsSections);

	for ( TInt i = 0; i < listOfGroupsSections.Count(); ++i )
		{
		CCntFilter* filter = CCntFilter::NewL();
		CleanupStack::PushL(filter);
		filter->SetContactFilterTypeGroup(ETrue);
		DatabaseReference().FilterDatabaseL(*filter);

		for ( TInt i = 0; i < filter->iIds->Count(); ++i )
			{
			TContactItemId groupId = (*filter->iIds)[i];
			CContactItem* group = DatabaseReference().OpenContactL(groupId);
			CContactGroup* newGroup = static_cast<CContactGroup*>(group);
			if(aGroupSection.Compare(newGroup->GetGroupLabelL())  == 0)
				{
				TRAP_IGNORE(DatabaseReference().RemoveContactFromGroupL(aContact, *newGroup));
				TRAP_IGNORE(DatabaseReference().AddContactToGroupL(aContact, *newGroup));
				DatabaseReference().CommitContactL(*group);
				const CContactIdArray* listOfContacts = newGroup->ItemsContained();
				const TInt count = listOfContacts->Count();
				delete listOfContacts;
				DatabaseReference().CloseContactL(groupId);
				break;
				}
			DatabaseReference().CloseContactL(groupId);
			}

    	CleanupStack::PopAndDestroy(filter);
		}

	CleanupStack::PopAndDestroy(&listOfGroupsSections);
	}

/* In this section, we edit the desired number of groups in the database
 * Set label for them and associate them with contacts
 @ param aGroupSection - Desired Group Name
 @ param aContact - Contact to be updated
 */
void CTestContactViewCRUDOperationsStep::DeleteContactsInGroupL(const TInt aNumberOfContactsToBeUpdated, const TPtrC& aGroupSection)
	{
	RArray<TPtrC>	listOfGroupsSections;
	CleanupClosePushL(listOfGroupsSections);
	ViewUtilityReference().TokenizeStringL(aGroupSection, listOfGroupsSections);

	for ( TInt i = 0; i < listOfGroupsSections.Count(); ++i )
		{
		CCntFilter* filter = CCntFilter::NewL();
		CleanupStack::PushL(filter);
		filter->SetContactFilterTypeGroup(ETrue);
		DatabaseReference().FilterDatabaseL(*filter);

		for ( TInt i = 0; i < filter->iIds->Count(); ++i )
			{
			TContactItemId groupId = (*filter->iIds)[i];
			CContactItem* group = DatabaseReference().OpenContactL(groupId);
			CContactGroup* newGroup = static_cast<CContactGroup*>(group);


			if(aGroupSection.Compare(newGroup->GetGroupLabelL())  == 0)
				{
				const CContactIdArray* listOfContacts = newGroup->ItemsContained();
				for(TInt i(0); i < aNumberOfContactsToBeUpdated; i++)
					{
					TRAP_IGNORE(DatabaseReference().DeleteContactL((*listOfContacts)[i]));
					}
				delete listOfContacts;
				DatabaseReference().CloseContactL(groupId);
				break;
				}
			DatabaseReference().CloseContactL(groupId);
			}

    	CleanupStack::PopAndDestroy(filter);
		}

	CleanupStack::PopAndDestroy(&listOfGroupsSections);
	}

/* In this section, we edit the desired number of groups in the database
 * Set label for them and associate them with contacts
 @ param aGroupSection - Desired Group Name
 @ param aContact - Contact to be updated
 */
void CTestContactViewCRUDOperationsStep::UpdateContactsInGroupL(const TInt aNumberOfContactsToBeUpdated, const TPtrC& aGroupSection)
	{
	RArray<TPtrC>	listOfGroupsSections;
	CleanupClosePushL(listOfGroupsSections);
	ViewUtilityReference().TokenizeStringL(aGroupSection, listOfGroupsSections);

	for ( TInt i = 0; i < listOfGroupsSections.Count(); ++i )
		{
		CCntFilter* filter = CCntFilter::NewL();
		CleanupStack::PushL(filter);
		filter->SetContactFilterTypeGroup(ETrue);
		DatabaseReference().FilterDatabaseL(*filter);

		for ( TInt i = 0; i < filter->iIds->Count(); ++i )
			{
			TContactItemId groupId = (*filter->iIds)[i];
			CContactItem* group = DatabaseReference().OpenContactL(groupId);
			CContactGroup* newGroup = static_cast<CContactGroup*>(group);


			if(aGroupSection.Compare(newGroup->GetGroupLabelL())  == 0)
				{
				const CContactIdArray* listOfContacts = newGroup->ItemsContained();
				for(TInt i(0); i < aNumberOfContactsToBeUpdated; i++)
					{
					TContactItemId contactItemId = (*listOfContacts)[i];
					CContactItem* contactItem = DatabaseReference().ReadContactL(contactItemId);
					CleanupStack::PushL(contactItem);
					UpdateFieldsL(*contactItem);
					CContactItem* updatedContactItem = DatabaseReference().UpdateContactLC(contactItemId, contactItem);
					CleanupStack::PopAndDestroy(updatedContactItem);
					CleanupStack::PopAndDestroy(contactItem);
					}
				delete listOfContacts;
				DatabaseReference().CloseContactL(groupId);
				break;
				}
			DatabaseReference().CloseContactL(groupId);
			}

    	CleanupStack::PopAndDestroy(filter);
		}

	CleanupStack::PopAndDestroy(&listOfGroupsSections);
	}



