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
* Description: Unit test class for character to key mappings
*
*/

#ifndef __UT_CPCSKEYMAP_H__
#define __UT_CPCSKEYMAP_H__

//  EXTERNAL INCLUDES
#include <digia/eunit/ceunittestsuiteclass.h>
#include <digia/eunit/eunitmacros.h>

//  FORWARD DECLARATIONS
class C12keyKeyMap;

//  CONSTANTS

//  CLASS DEFINITION
/**
 * Tester class for C12keyKeyMap.
 */
NONSHARABLE_CLASS(UT_CPcsKeyMap): public CEUnitTestSuiteClass
    {
    public: // Constructors and destructors

        /**
         * Two phase construction
         */
        static UT_CPcsKeyMap* NewL();
        static UT_CPcsKeyMap* NewLC();
        
        /**
         * Destructor
         */
        ~UT_CPcsKeyMap();

    private: // Constructors

        UT_CPcsKeyMap();
        void ConstructL();
        
    private: // Test case setup and teardown

        void SetupL();
        void Teardown();

    private: // Test methods
    
        void UT_NewLL();
        void UT_GetMappedStringL();
        void UT_GetMappedStringSeparatorL();
        void UT_GetMappedStringWithNumbersL();
        void UT_GetMappedStringWithSpecialCharactersL();
		void UT_GetMappedStringThaiLanguageOneCharL();
        void UT_GetMappedStringThaiLanguageL();
        void UT_GetMappedStringThaiLanguageEmptyResultL();
        void UT_GetNumericLimitsL();
		void UT_GetNumericLimitsLongInputL();

    private:    // Data

        C12keyKeyMap* iKeyMap;

        EUNIT_DECLARE_TEST_TABLE;
    };

#endif      //  __UT_CPCSKEYMAP_H__

// End of file
