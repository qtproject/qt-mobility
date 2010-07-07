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
* Description: Unit test class for character to key mappings
*
*/

#ifndef __UT_CQWERTYKEYMAP_H__
#define __UT_CQWERTYKEYMAP_H__

//  EXTERNAL INCLUDES
#include <digia/eunit/ceunittestsuiteclass.h>
#include <digia/eunit/eunitmacros.h>

//  FORWARD DECLARATIONS
class CQwertyKeyMap;

//  CONSTANTS

//  CLASS DEFINITION
/**
 * Tester class for CQwertyKeyMap.
 */
NONSHARABLE_CLASS(UT_CQwertyKeyMap): public CEUnitTestSuiteClass
    {
    public: // Constructors and destructors

        /**
         * Two phase construction
         */
        static UT_CQwertyKeyMap* NewL();
        static UT_CQwertyKeyMap* NewLC();
        
        /**
         * Destructor
         */
        ~UT_CQwertyKeyMap();

    private: // Constructors

        UT_CQwertyKeyMap();
        void ConstructL();
        
    private: // Test case setup and teardown

        void SetupL();
        void Teardown();

    private: // Test methods
    
        void UT_NewLL();
        void UT_GetMappedStringL();
		void UT_GetNumericLimitsL();
		void UT_GetNumericLimitsBeginsByFirstKeyL();
		void UT_GetNumericLimitsLongValueL();

    private:    // Data

        CQwertyKeyMap* iKeyMap;

        EUNIT_DECLARE_TEST_TABLE;
    };

#endif      //  __UT_CQWERTYKEYMAP_H__

// End of file
