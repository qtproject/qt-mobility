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

#ifndef __UT_CKOREANKEYMAP_H__
#define __UT_CKOREANKEYMAP_H__

//  EXTERNAL INCLUDES
#include <digia/eunit/ceunittestsuiteclass.h>
#include <digia/eunit/eunitmacros.h>

//  FORWARD DECLARATIONS
class CKoreanKeyMap;

//  CONSTANTS

//  CLASS DEFINITION
/**
 * Tester class for CQwertyKeyMap.
 */
NONSHARABLE_CLASS(UT_CKoreanKeyMap): public CEUnitTestSuiteClass
    {
    public: // Constructors and destructors

        /**
         * Two phase construction
         */
        static UT_CKoreanKeyMap* NewL();
        static UT_CKoreanKeyMap* NewLC();
        
        /**
         * Destructor
         */
        ~UT_CKoreanKeyMap();

    private: // Constructors

        UT_CKoreanKeyMap();
        void ConstructL();
        
    private: // Test case setup and teardown

        void SetupL();
        void Teardown();

    private: // Test methods
    
        void UT_GetMappedString_JamoL();
        void UT_GetMappedString_HangulL();
        void UT_GetMappedString_MixedL();
        void UT_GetMappedString_TokenizeHangulL();
        void UT_IsLanguageSupported_LatinTextL();
        void UT_IsLanguageSupported_KoreanTextL();
        void UT_IsLanguageSupported_MixedTextL();
        void UT_IsLanguageSupported_TextWithJamoL();

    private:    // Data

        CKoreanKeyMap* iKeyMap;

        EUNIT_DECLARE_TEST_TABLE;
    };

#endif      //  __UT_CKOREANKEYMAP_H__

// End of file
