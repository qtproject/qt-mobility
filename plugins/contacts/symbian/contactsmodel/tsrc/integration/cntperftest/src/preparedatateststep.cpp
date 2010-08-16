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


#include "preparedatateststep.h"

/**
 * Constructor
 */
CPrepareDataTestStep::CPrepareDataTestStep()
	{
	// Mandatory call to base class method to set up the human readable name for logging.
	SetTestStepName(KPrepareDataTestStep);
	}

/**
 * Destructor
 */
CPrepareDataTestStep::~CPrepareDataTestStep()
	{
	iListOfContactItemsInDatabase.Close();
	}

/**
 * Setup the environment required for test, install schedular in the current thread
 * Opens the relevant contacts database file
 */
TVerdict CPrepareDataTestStep::doTestStepPreambleL()
	{
 	CTestContactViewDatabaseUtilitiesStep::doTestStepPreambleL();
	return TestStepResult();
	}

/**
 * Cleanup function for the test step
 */
TVerdict CPrepareDataTestStep::doTestStepPostambleL()
	{
 	CTestContactViewDatabaseUtilitiesStep::doTestStepPostambleL();
	return TestStepResult();
	}


/**
Base class pure virtual.
* @return EPass or EFail indicating the result of the test step.
*/
TVerdict CPrepareDataTestStep::doTestStepL()
	{
	ImportContactsL();
	AddContactsInDatabaseL();
	IterateThroAllSortOrderSectionsAndUpdateContactsInDatabaseL();
	IterateThroAllFilterViewsSectionsAndUpdateL();
	IterateThroAllMatchingStringSectionsAndUpdateL();
	AddGroupsInDatabaseL();
	IterateThroAllGroupSectionsAndUpdateL();
	UpdateContactToSpecificStringL();
	CloseDatabaseL();
	CopyDatabaseL();
	return TestStepResult();
	}

/**
* Import Contact entries
*/
void CPrepareDataTestStep::ImportContactsL()
	{
	ViewUtilityReference().ImportContactsL();
	}

/**
 * Here we are trying add blank contact items in the database
 * Say some 2000 thousand contacts are added to the database
 */
void CPrepareDataTestStep::AddContactsInDatabaseL()
	{
	_LIT(KNumOfContacts, "numofcontacts");
	TInt numOfContacts = 0;
	GetIntFromConfig(ConfigSection(), KNumOfContacts, numOfContacts);

	for(TInt i = 0; i < numOfContacts; ++i)
		{
		CContactCard* contactCard = CContactCard::NewL();
		CleanupStack::PushL(contactCard);
		TContactItemId contactId = DatabaseReference().AddNewContactL(*contactCard);
		CleanupStack::PopAndDestroy(contactCard);
		iListOfContactItemsInDatabase.AppendL(contactId);
		DatabaseReference().CloseContactL(contactId);
		}

	TInt count = DatabaseReference().CountL();
	_LIT(KDatabaseCount, "Total number of contact entries is %d :");
	INFO_PRINTF2(KDatabaseCount, count);
	}


/* As mentioned earlier there will be some 1000 blank contacts in the database
 * Now any view will have it's own criteria like
 * -- view sort order i.e. the order in which the contact items in the view must be sorted
 * -- view preferences i.e. whether only contact cards or contact groups or both must be included in the view
 *    whether the unsorted contacts must be included in the view or not, if included, whether they must be included in the
 *    start of view or end  of view
 * It becomes necessary to supply the necessary envirnoment that meets the view criteia
 * Each view can supply its desired criteria. Based on this, the contacts in the database will be updated to meet the view specificattions
 */
void CPrepareDataTestStep::IterateThroAllSortOrderSectionsAndUpdateContactsInDatabaseL()
	{
	_LIT(KListOfSortOrderSections, "listofsortorder");
	TPtrC listOfSortOrderSectionsString;
	GetStringFromConfig(ConfigSection(), KListOfSortOrderSections, listOfSortOrderSectionsString);

	RArray<TPtrC>	listOfSortOrderSections;
	CleanupClosePushL(listOfSortOrderSections);

	ViewUtilityReference().TokenizeStringL(listOfSortOrderSectionsString, listOfSortOrderSections);

	for ( TInt i = 0; i < listOfSortOrderSections.Count(); ++i )
		{
		UpdateContactFieldsSpecificToSortOrderL(listOfSortOrderSections[i]);
		}

	CleanupStack::PopAndDestroy(&listOfSortOrderSections);
	}

/**
 * Here we are trying add contacts fields in the existing contacts such that they meet the contact views
 * Sort Order. Intentionally some of the contacts are updated with the contact fields of Null data to meet
 * the unsorted contacts criteria
 */
void CPrepareDataTestStep::UpdateContactFieldsSpecificToSortOrderL(const TPtrC& aSortOrderSection)
	{
	// Based on the sort order, the desired number of contacts will be updated i.e contact fields will be
	// added to the contacts in the database. The new fields added will be as per the sort order
	// at the end of this operation, if we constuct a contact view with this sort order
	// then the desired number of contacts will be included in the view

	_LIT(KViewSortOrder, "sortorder");
	TPtrC viewSortOrderString;
	GetStringFromConfig(aSortOrderSection, KViewSortOrder, viewSortOrderString);

	RContactViewSortOrder sortOrder = ViewUtilityReference().ConvertStringToSortOrderL(viewSortOrderString) ;
	CleanupClosePushL(sortOrder);

	_LIT(KNumOfSortableContacts, "numofsortablecontacts");
	TInt numOfSortableContacts;
	GetIntFromConfig(aSortOrderSection, KNumOfSortableContacts, numOfSortableContacts);

	// Here we are trying to update the desired number of contacts in the database
	// with Null contact fields in order to meet the unsortable contacts requirement

	_LIT(KNumUnSortableContacts, "numunsortedcontacts");
	TInt numUnSortableContacts;
	GetIntFromConfig(aSortOrderSection, KNumUnSortableContacts, numUnSortableContacts);

	const TInt KTotalNumOfContactsSpecificToThisSortOrder =  numOfSortableContacts + numUnSortableContacts;

	TInt numberOfReadySortableContacts = 0;
	TInt numberOfReadyUnSortableContacts = 0;

	// There may be contact items in the database that meet the sort order
	// We need be sure about the contact items that we should update

	RArray <TContactItemId> ListOfContactItemsThatCanUpdatedToMeetSortOrder;
	CleanupClosePushL(ListOfContactItemsThatCanUpdatedToMeetSortOrder);


	for(TInt i = 0; i < iListOfContactItemsInDatabase.Count(); ++i)
		{
		TContactItemId desiredContactItemId = iListOfContactItemsInDatabase[i];
		CContactItem* contact = DatabaseReference().OpenContactL(desiredContactItemId);
		CleanupStack::PushL(contact);
		CContactItemFieldSet& fieldSet = contact->CardFields();

		TInt firstContactFieldPosition = fieldSet.Find(sortOrder[0]);
		TInt secondContactFieldPosition = fieldSet.Find(sortOrder[1]);
		TInt thirdContactFieldPosition = fieldSet.Find(sortOrder[2]);

		if ((firstContactFieldPosition != KErrNotFound) &&  (secondContactFieldPosition != KErrNotFound) && (thirdContactFieldPosition!= KErrNotFound))
			{
			CContactItemField& firstContactItemField = fieldSet[firstContactFieldPosition];
			CContactItemField& secondContactItemField = fieldSet[secondContactFieldPosition];
			CContactItemField& thirdContactItemField  = fieldSet[thirdContactFieldPosition];

			if((firstContactItemField.Storage() == NULL) && (secondContactItemField.Storage() == NULL) && (thirdContactItemField.Storage() == NULL))
				{
		  		++numberOfReadyUnSortableContacts;
				}
			else
				{
				++numberOfReadySortableContacts;
				}
			}
		else
			{
			ListOfContactItemsThatCanUpdatedToMeetSortOrder.AppendL(desiredContactItemId);
			}

		CleanupStack::PopAndDestroy(contact);
		DatabaseReference().CloseContactL(desiredContactItemId);
		}

	if((numberOfReadyUnSortableContacts + numberOfReadySortableContacts) < KTotalNumOfContactsSpecificToThisSortOrder)
		{
		TInt j = 0;
		for(; j < (numOfSortableContacts-numberOfReadySortableContacts); ++j)
			{
			TContactItemId contactItemIdToBeUpdated = ListOfContactItemsThatCanUpdatedToMeetSortOrder[j];
			CContactItem* contact = DatabaseReference().OpenContactL(contactItemIdToBeUpdated);
			CleanupStack::PushL(contact);
			AddContactFieldL(*contact, sortOrder, ETrue);
			DatabaseReference().CommitContactL(*contact);
			DatabaseReference().CloseContactL(contactItemIdToBeUpdated);
			CleanupStack::PopAndDestroy(contact);
			}

		for(TInt k = j; k < (j + (numUnSortableContacts-numberOfReadyUnSortableContacts)); ++k)
			{
			TContactItemId contactItemIdToBeUpdated = ListOfContactItemsThatCanUpdatedToMeetSortOrder[k];
			CContactItem* contact = DatabaseReference().OpenContactL(contactItemIdToBeUpdated);
			CleanupStack::PushL(contact);
			AddContactFieldL(*contact, sortOrder);
			DatabaseReference().CommitContactL(*contact);
			DatabaseReference().CloseContactL(contactItemIdToBeUpdated);
			CleanupStack::PopAndDestroy(contact);
			}
		}

	CleanupStack::PopAndDestroy(2,&sortOrder);
	}

/** Adds contact fields specific to sort order,
 * Sort Order. Intentionally some of the contacts are updated with the contact fields of Null data to meet
 * the unsorted contacts criteria
 */
void CPrepareDataTestStep::AddContactFieldL(CContactItem& aContact, const RContactViewSortOrder& aSortOrder,
																								TBool aDataRequired)
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

		if(aDataRequired)
			{
			SetDataL(fieldType, *field);
			}

		aContact.AddFieldL(*field);
		CleanupStack::Pop(field);
		CleanupStack::PopAndDestroy(contentType);
		}
	}

/**
 * Reads all the sections in the ini file relevant for updating the contacts database
 * in order to meet filter view requirements
 */
void CPrepareDataTestStep::IterateThroAllFilterViewsSectionsAndUpdateL()
	{
	_LIT(KListOfFilterViewSectionsString, "listoffilterviewsections");
	TPtrC listOfFilterViewSectionsString;
	GetStringFromConfig(ConfigSection(), KListOfFilterViewSectionsString, listOfFilterViewSectionsString);

	RArray<TPtrC>	listOfFilterViewSections;
	CleanupClosePushL(listOfFilterViewSections);
	ViewUtilityReference().TokenizeStringL(listOfFilterViewSectionsString, listOfFilterViewSections);

	for(TInt i = 0; i < listOfFilterViewSections.Count(); ++i)
		{
		UpdateContactsInDatabaseAsPerFilteredViewRequirementsL(listOfFilterViewSections[i]);
		}

	CleanupStack::PopAndDestroy(&listOfFilterViewSections);
	}

/**
 * Updates contacts in database as per filter view requirements specified in the ini section
 */
void CPrepareDataTestStep::UpdateContactsInDatabaseAsPerFilteredViewRequirementsL(const TPtrC& aFilteredViewSection)
	{
	_LIT(KViewSortOrder, "sortorder");
	TPtrC viewSortOrderString;
	GetStringFromConfig(aFilteredViewSection, KViewSortOrder, viewSortOrderString);

	RContactViewSortOrder sortOrder = 	ViewUtilityReference().ConvertStringToSortOrderL(viewSortOrderString);
	CleanupClosePushL(sortOrder);

	_LIT(KNumOfContactsWithFilter, "numofcontactswiththisfilter");
	TInt numOfContactsWithFilter;
	GetIntFromConfig(aFilteredViewSection, KNumOfContactsWithFilter, numOfContactsWithFilter);

	_LIT(KDesiredFilterString, "desiredFilter");
	TPtrC desiredFilterString;
	GetStringFromConfig(aFilteredViewSection, KDesiredFilterString, desiredFilterString);

	CContactDatabase::TContactViewFilter	viewFilter = ViewUtilityReference().GetContactViewFilterL(desiredFilterString);

	RArray<TContactItemId> contactSpecificToSortOrder;
	CleanupClosePushL(contactSpecificToSortOrder);

	RetrieveContactsSpecificToSortOrderL(sortOrder, contactSpecificToSortOrder);

	for(TInt i = 0; i < numOfContactsWithFilter; ++i)
		{
		TContactItemId id = contactSpecificToSortOrder[i];
		CContactItem* contactItem = DatabaseReference().OpenContactL(id);
		CleanupStack::PushL(contactItem);
		AddFilterBasedFieldsL(*contactItem, viewFilter);
		DatabaseReference().CommitContactL(*contactItem);
		DatabaseReference().CloseContactL(id);
		CleanupStack::PopAndDestroy(contactItem);
		}

	CleanupStack::PopAndDestroy(2, &sortOrder);
	}


/**
 * Retrieves contacts specific to desired sort order from the database
 */
void CPrepareDataTestStep::RetrieveContactsSpecificToSortOrderL(const RContactViewSortOrder& aSortOrder, RArray<TContactItemId>& aContactSpecificToSorder)
	{
	for(TInt i = 0; i < iListOfContactItemsInDatabase.Count(); ++i)
		{
		TContactItemId contactId = iListOfContactItemsInDatabase[i];
		CContactItem* contactItem = DatabaseReference().OpenContactL(contactId);
		CleanupStack::PushL(contactItem);
		TBool exists = CheckAllTheFieldsExists(*contactItem, aSortOrder);
		if(exists)
			{
			aContactSpecificToSorder.AppendL(contactId);
			}
		DatabaseReference().CloseContactL(contactId);
		CleanupStack::PopAndDestroy(contactItem);
		}

	}

/**
 * Checks whether all the three desired contact fields are present in the contact or not
 */
TBool CPrepareDataTestStep::CheckAllTheFieldsExists(const CContactItem& aContact, const RContactViewSortOrder& aSortOrder)
	{
	CContactItemFieldSet& fieldSet = aContact.CardFields();

	TInt firstContactFieldPosition = fieldSet.Find(aSortOrder[0]);
	TInt secondContactFieldPosition = fieldSet.Find(aSortOrder[1]);
	TInt thirdContactFieldPosition = fieldSet.Find(aSortOrder[2]);

	TInt error = KErrNotFound;

	if ((firstContactFieldPosition != error) &&  (secondContactFieldPosition != error) && (thirdContactFieldPosition!= error))
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}
	}


/**
 * Data is read from the ini file about the matching strings expected to be present in some of the
 * contact fields and desired number of contacts in the database are updated with this matching string info
 */
void CPrepareDataTestStep::IterateThroAllMatchingStringSectionsAndUpdateL()
	{
	_LIT(KListOfMatchingStringSectionsString, "listofMatchingString");
	TPtrC listOfMatchingStringSectionsString;
	GetStringFromConfig(ConfigSection(), KListOfMatchingStringSectionsString, listOfMatchingStringSectionsString);

	RArray<TPtrC>	listOfMatchingStringSections;
	CleanupClosePushL(listOfMatchingStringSections);
	ViewUtilityReference().TokenizeStringL(listOfMatchingStringSectionsString, listOfMatchingStringSections);

	for(TInt i = 0; i < listOfMatchingStringSections.Count(); ++i)
		{
		UpdateContactsInDatabaseAsPerMatchingStringsL(listOfMatchingStringSections[i]);
		}

	CleanupStack::PopAndDestroy(&listOfMatchingStringSections);
	}

/**
 * Updates the contacts in the database based on the data available in the ini section
 * @param aMatchingStringSection - Relevant section in the ini file
 */
void CPrepareDataTestStep::UpdateContactsInDatabaseAsPerMatchingStringsL(const TPtrC& aMatchingStringSection)
	{
	_LIT(KViewSortOrder, "sortorder");
	TPtrC viewSortOrderString;
	GetStringFromConfig(aMatchingStringSection, KViewSortOrder, viewSortOrderString);

	RContactViewSortOrder sortOrder = ViewUtilityReference().ConvertStringToSortOrderL(viewSortOrderString);
	CleanupClosePushL(sortOrder);


	_LIT(KNumOfContactsWithFilter, "numofcontactswiththisstring");
	TInt numOfContactsWithThisString;
	GetIntFromConfig(aMatchingStringSection, KNumOfContactsWithFilter, numOfContactsWithThisString);

	_LIT(KDesiredMatchingString, "matchingstring");
	TPtrC desiredMatchingString;
	GetStringFromConfig(aMatchingStringSection, KDesiredMatchingString, desiredMatchingString);

	_LIT(KDesiredContactField, "contactfield");
	TPtrC desiredContactField;
	GetStringFromConfig(aMatchingStringSection, KDesiredContactField, desiredContactField);

	TUid uidInfo = GetContactFieldType(desiredContactField);

	RArray<TContactItemId> contactSpecificToSortOrder;
	CleanupClosePushL(contactSpecificToSortOrder);

	RetrieveContactsSpecificToSortOrderL(sortOrder, contactSpecificToSortOrder);

	for(TInt i = 0; (i < (contactSpecificToSortOrder.Count()) && (i < numOfContactsWithThisString)); ++i)
		{
		TContactItemId id = contactSpecificToSortOrder[i];
		CContactItem* contactItem = DatabaseReference().OpenContactL(id);
		CleanupStack::PushL(contactItem);	// contactItem
		AddMatchingStringToContactL(*contactItem, uidInfo, desiredMatchingString);
		DatabaseReference().CommitContactL(*contactItem);
		DatabaseReference().CloseContactL(id);
		CleanupStack::PopAndDestroy(contactItem);
		}

	CleanupStack::PopAndDestroy(2, &sortOrder);

	}

/**
 * Update contacts in database with desired string
 * This feature is useful while constructing Find Views, Sub Views etc..
 * @param aContact - Contact to be updated
 * @param aUid - uid of field in the contact item to be updated
 * @param aDesiredMatchingString - Matching string
 */
void CPrepareDataTestStep::AddMatchingStringToContactL(CContactItem& aContact, TUid aUid, const TPtrC& aDesiredMatchingString)
	{
	CContactItemFieldSet& fieldSet = aContact.CardFields();
	TInt pos = fieldSet.Find(aUid);
	if(pos == KErrNotFound)
		{
		TFieldType fieldType = aUid;
		TStorageType storageType = GetStorageType(fieldType);
		CContentType* contentType = CContentType::NewL();
		contentType->AddFieldTypeL(fieldType);
		CContactItemField* field = 	CContactItemField::NewL(storageType, *contentType);
		CleanupStack::PushL(field);
		field->TextStorage()->SetTextL(aDesiredMatchingString);
		aContact.AddFieldL(*field);
		CleanupStack::Pop();	// field
		}
	else
		{
		CContactItemField& field = fieldSet[pos];
		field.TextStorage()->SetTextL(aDesiredMatchingString);
		}
	}

void CPrepareDataTestStep::UpdateContactToSpecificStringL()
	{
		_LIT(KListOfSetDesiredStringSections, "listofcontacttoupdate");
		TPtrC listOfSections;
		GetStringFromConfig(ConfigSection(), KListOfSetDesiredStringSections, listOfSections);

		RArray<TPtrC>	listOfUpdateContact;
		CleanupClosePushL(listOfUpdateContact);

		ViewUtilityReference().TokenizeStringL(listOfSections, listOfUpdateContact);

		for ( TInt i = 0; i < listOfUpdateContact.Count(); ++i )
			{
			UpdateContactFieldsToDesireStingL(listOfUpdateContact[i]);
			}

		CleanupStack::PopAndDestroy(&listOfUpdateContact);

	}

void CPrepareDataTestStep::UpdateContactFieldsToDesireStingL(const TPtrC& aMatchingStringSection)
	{
	_LIT(KViewSortOrder, "sortorder");
	_LIT(KContactId, "contactid");
	_LIT(KDesiredString, "desiredstring");

	TPtrC viewSortOrderString;
	GetStringFromConfig(aMatchingStringSection, KViewSortOrder, viewSortOrderString);

	RContactViewSortOrder sortOrder = ViewUtilityReference().ConvertStringToSortOrderL(viewSortOrderString) ;
	CleanupClosePushL(sortOrder);

	TInt contactId;
	GetIntFromConfig(aMatchingStringSection, KContactId, contactId);

	TPtrC desiredSting;
	GetStringFromConfig(aMatchingStringSection, KDesiredString, desiredSting);

	RArray<TContactItemId> contactSpecificToSortOrder;
	CleanupClosePushL(contactSpecificToSortOrder);

	RetrieveContactsSpecificToSortOrderL(sortOrder, contactSpecificToSortOrder);
	CContactItem* contactItem = DatabaseReference().OpenContactL(contactId);
	CleanupStack::PushL(contactItem);

	CContactItemFieldSet& fieldSet = contactItem->CardFields();

	TInt firstContactFieldPosition = fieldSet.Find(sortOrder[0]);
	TInt secondContactFieldPosition = fieldSet.Find(sortOrder[1]);
	TInt thirdContactFieldPosition = fieldSet.Find(sortOrder[2]);

	// Check whether they have content.
	CContactItemField& field1 = fieldSet[firstContactFieldPosition];
	CContactFieldStorage* firstStorage = field1.Storage();

	CContactItemField& field2 = fieldSet[secondContactFieldPosition];
	CContactFieldStorage* secondStorage = field2.Storage();

	CContactItemField& field3 = fieldSet[thirdContactFieldPosition];
	CContactFieldStorage* thirdStorage = field3.Storage();

	field1.TextStorage()->SetTextL(desiredSting);
	field2.TextStorage()->SetTextL(desiredSting);
	field3.TextStorage()->SetTextL(desiredSting);

	DatabaseReference().CommitContactL(*contactItem);
	DatabaseReference().CloseContactL(contactId);

	CleanupStack::PopAndDestroy(contactItem);
	CleanupStack::PopAndDestroy(&contactSpecificToSortOrder);
	CleanupStack::PopAndDestroy(&sortOrder);
	}


