/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Unit test class for QWERTY tables
*
*/

#ifndef __UT_CPREDICTIVESEARCHSETTINGSTABLE_H__
#define __UT_CPREDICTIVESEARCHSETTINGSTABLE_H__

//  EXTERNAL INCLUDES
#include <digia/eunit/ceunittestsuiteclass.h>
#include <digia/eunit/eunitmacros.h>
#include <sqldb.h>

//  FORWARD DECLARATIONS
class CPredictiveSearchSettingsTable;

//  CONSTANTS

//  CLASS DEFINITION
/**
 * Tester class for CPredictiveSearchSettingsTable.
 */
NONSHARABLE_CLASS( UT_CPredictiveSearchSettingsTable ): public CEUnitTestSuiteClass
    {
    public: // Constructors and destructors

        /**
         * Two phase construction
         */
        static UT_CPredictiveSearchSettingsTable* NewL();
        static UT_CPredictiveSearchSettingsTable* NewLC();
        
        /**
         * Destructor
         */
        ~UT_CPredictiveSearchSettingsTable();

    private: // Constructors

        UT_CPredictiveSearchSettingsTable();
        void ConstructL();
        
    private: // Test case setup and teardown

        void SetupL();
        void Teardown();

    private: // Test functions

        void UT_CreateTableLL();

    private:    // Data

		CPredictiveSearchSettingsTable* iTable;
        
        RSqlDatabase iDB;

        EUNIT_DECLARE_TEST_TABLE;
    };

#endif      //  __UT_CPREDICTIVESEARCHSETTINGSTABLE_H__

// End of file
