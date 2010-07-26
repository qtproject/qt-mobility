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




#ifndef __TEST_CONTACTVIEW_CRUD_OPERATIONS_H__
#define __TEST_CONTACTVIEW_CRUD_OPERATIONS_H__

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
#include "addmodifydeletecontactfields.h"
#include "testcontactviewdatabaseutilitiesstep.h"
#include "contactviewsthread.h"


// Literals
	_LIT(KTestContactViewCRUDOperationsStep, "TestContactViewCRUDOperationsStep");
	_LIT(KViewConstructionOperations,	 	 "ViewConstructionOperations");
	_LIT(KAddContactOperations,		      	 "AddContactOperations");
	_LIT(KRemoveContactsOperations,		     "RemoveContactsOperations");
	_LIT(KUpdateContactOperations,		     "UpdateContactOperations");
	_LIT(KSortOrderChangeOperations,	     "SortOrderChangeOperations");
	_LIT(KServerRelatedOperations,		     "ServerRelatedOperations");
	_LIT(KGroupChangedOperations,		     "GroupChangedOperations");

/**
 * This Test Step supports testing of contact views behaviour when CRUD operations are performed on the
 * contacts database. A seperate contact views thread is launched and contact views are created in the bew thread
 * CRUD operations are perfomed in the main thread and the contact view count and notifications are
 * validated in the Contact Views Thread
 */
class CTestContactViewCRUDOperationsStep : public CTestContactViewDatabaseUtilitiesStep, public TAddModifyDeleteContactFields
	{
	public:
		CTestContactViewCRUDOperationsStep();
		~CTestContactViewCRUDOperationsStep();

		TVerdict 	doTestStepPreambleL();
		TVerdict 	doTestStepL();
		TVerdict 	doTestStepPostambleL();

		void 		LaunchContactViewsThreadL();
		void 		ResumeContactViewsThreadL();

		void 		CreateContactItemsL();
		void 		UpdateContactItemsL();
		void		RemoveContactItemsL();
		void 		DeleteContactCardsL();
		void 		DeleteContactGroupsL();



		void 		AddContactEntriesL();
		void 		AddContactFieldL(CContactItem& aContact, const RContactViewSortOrder& aSortOrder);
		void 		AddFieldsSpecificToFilterL(CContactItem& aContact);
		void    	AddMatchingStringToContactL(CContactItem& aContact);
		void 		UpdateContactEntriesL();
		void 		SetExecutionCompleted();
		void 		SetCurrentOperation	(TPtrC aCurrentOperation);
		CContactDatabase* CreateAndOpenDataBaseL(const TPtrC& aDbName, TBool aCreateDataBase = EFalse);
		void 		IterateThroAllGroupSectionsAndUpdateContactL(const TPtrC& aGroupSection, CContactCard& aContact);
		void 		DeleteContactsInGroupL(const TInt aNumberOfContactsToBeUpdated, const TPtrC& aGroupSection);
		void 		UpdateContactsInGroupL(const TInt aNumberOfContactsToBeUpdated, const TPtrC& aGroupSection);
		
		

	private:
		RThread 						iThread;
		TBool* 							iExecutionCompleted;
		RSemaphore						iSemaphore;
		TPtrC*							iCurrentOperation;
		RChunk							iChunk;
	};

#endif
