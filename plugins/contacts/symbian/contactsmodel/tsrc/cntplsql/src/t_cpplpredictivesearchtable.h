/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Unit test class for testing 12-key predictive search table
*
*/

#ifndef __UT_CPPLPREDICTIVESEARCHTABLE_H__
#define __UT_CPPLPREDICTIVESEARCHTABLE_H__

//  EXTERNAL INCLUDES
#include <digia/eunit/ceunittestsuiteclass.h>
#include <digia/eunit/eunitmacros.h>
#include <sqldb.h>
#include "cntdef.h" // TContactItemId

//  FORWARD DECLARATIONS
class CPplPredictiveSearchTableBase;
class C12keyPredictiveSearchTable;
class CQwertyPredictiveSearchTable;
class CPredictiveSearchSettingsTable;
class CPredictiveSearchSynchronizer;
class CContactItem;


//  CONSTANTS

//  CLASS DEFINITION
/**
 * Tester class for C12keyPredictiveSearchTable.
 */
NONSHARABLE_CLASS( UT_CPplPredictiveSearchTable ): public CEUnitTestSuiteClass
    {
    public: // Constructors and destructors

        /**
         * Two phase construction
         */
        static UT_CPplPredictiveSearchTable* NewL();
        static UT_CPplPredictiveSearchTable* NewLC();
        
        /**
         * Destructor
         */
        ~UT_CPplPredictiveSearchTable();

    private: // Constructors

        UT_CPplPredictiveSearchTable();
        void ConstructL();
        
    private: // Test case setup and teardown

        void SetupL();
        void SetupSyncL();
		void SetupSyncJust12keyExistsL();
		void SetupLanguageChangesL();
		void UseSpecificDbL(const TDesC& aDbFile);
        void Teardown();

    private: // Test functions

		void UT_DummyL();
        void UT_CreateInDbLL();
        void UT_CreateInDbManyContactsL();
		void UT_CreateInDbWithHashAndStarL();
        void UT_UpdateLL();
        void UT_UpdateLBothFieldsL();
        void UT_SearchL();
        void UT_SearchWithSpacesL();
        void UT_DeleteLL();
        void UT_DeleteContactsL();
        void UT_DeleteNonexistingContactL();
        void UT_CheckIfTableExistsL();
        void UT_CheckIfTableExists2L();
        void UT_SynchronizeTableL();
		void UT_SynchronizeTableJust12keyExistsL();
		void UT_DeleteTablesL();
		void UT_LanguageChangesL();
        void UT_TokenizeNamesL();
		void UT_WriteToDbL();
		void UT_ConvertToHexL();
        
    private: // New functions

        void CheckItemCountL(
            TInt aCountInTable0 = 0,
            TInt aCountInTable1 = 0,
            TInt aCountInTable2 = 0,
            TInt aCountInTable3 = 0,
            TInt aCountInTable4 = 0,
            TInt aCountInTable5 = 0,
            TInt aCountInTable6 = 0,
            TInt aCountInTable7 = 0,
            TInt aCountInTable8 = 0,
            TInt aCountInTable9 = 0,
			TInt aCountInTable10 = 0,
			TInt aCountInTable11 = 0);
        
        /**
         * Adds a new contact to table.
         */
        void AddContactL(const TDesC& aFirstName,
                         const TDesC& aLastName,
                         TContactItemId aContactId);
        
        /**
         * Searches the table for matches, and returns an array containing
         * matching contact's ids.
         */
        RArray<TContactItemId> DoPredictiveSearchL(const TDesC& aSearchString);

        const TDesC& DetermineTableName(const TDesC& aSearchString);

		TInt64 LowerLimitL(const TDesC& aString) const;
		TInt64 UpperLimitL(const TDesC& aString) const;
		TInt64 ConvertToNbrL(const TDesC& aString, TChar aPadChar) const;

    private:    // Data

		C12keyPredictiveSearchTable* iTable; // Owned
		CQwertyPredictiveSearchTable* iPredSearchQwertyTable; // Owned 
		CPredictiveSearchSettingsTable* iPredSearchSettingsTable; // Owned
		CPredictiveSearchSynchronizer* iPredictiveSearchSynchronizer; // Owned
        
        RSqlDatabase iDB;

        EUNIT_DECLARE_TEST_TABLE;
    };

#endif      //  __UT_CPPLPREDICTIVESEARCHTABLE_H__

// End of file
