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




#ifndef __TEST_CONTACTVIEW_DATABASE_UTILITIES_H__
#define __TEST_CONTACTVIEW_DATABASE_UTILITIES_H__

// System Includes
#include <testexecutestepbase.h>
#include <cntdb.h>
#include <cntitem.h>
#include <cntviewbase.h>
#include <cntfldst.h>
#include <cntfilt.h>
#include <e32math.h>
#include <coreappstest/testserver.h>

// User Includes
#include "contactutilitiescollection.h"
#include "clientserver.h"

// Literals used
_LIT(KTestContactViewDatabaseUtilitiesStep, "TestContactViewDatabaseUtilitiesStep");

/**
 * This class groups all commonlu used functions across test steps
 * supports contact views creation, Installing of Active Schedular
 * Handles for current database, view collections etc..
 */
class CTestContactViewDatabaseUtilitiesStep : public CTestStep
	{
	public:
		~CTestContactViewDatabaseUtilitiesStep();
		TVerdict 		  	 		 doTestStepPreambleL();
		TVerdict 					 doTestStepPostambleL();
		void 						 InstallActiveSchedularL();
		void 						 OpenDataBaseL();
		void 					 	 SetupUtilityL();
		CContactDatabase* 		 	 CreateAndOpenDataBaseL(const TPtrC& aDbName, TBool aCreateDataBase = EFalse);
		CContactDatabase& 			 DatabaseReference();
		CContactViewCollection& 	 ViewCollectionReference();
		CContactUtilitiesCollection& ViewUtilityReference();
		RPIMTestServer& 			 PimTestServerReference();
		void 					 	 ConstructViewsL();
		TBool 						 ListenForViewEventsL(TContactViewEvent& aEvent);
		void 						 UnInstallActiveSchedularL();
		void 						 CloseDatabaseL();
		void 						 AddGroupsInDatabaseL();
		void 						 IterateThroAllGroupSectionsAndUpdateL();
		void 						 UpdateGroupsL(const TPtrC& aGroupSection, CContactGroup& aGroup);
		void 						 CopyDatabaseL();


	private:
		CActiveScheduler* 					iScheduler;
		CContactUtilitiesCollection* 		iContactUtility;
		CContactViewCollection*    			iContactViewCollection;
		CContactDatabase* 					iDb;
		RPIMTestServer						iPimTestServer;

	};

#endif

