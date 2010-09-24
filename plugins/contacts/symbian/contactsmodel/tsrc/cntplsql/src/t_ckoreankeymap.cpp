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
* Description:  Unit test class for testing CQwertyKeyMap
*
*/

// INTERNAL INCLUDES
#include "t_ckoreankeymap.h"
#include "ckoreankeymap.h"
#include <QString>

// SYSTEM INCLUDES
#include <digia/eunit/eunitmacros.h>
#include <hbinputkeymapfactory.h>


// -----------------------------------------------------------------------------
// UT_CKoreanKeyMap::NewL
// -----------------------------------------------------------------------------
//
UT_CKoreanKeyMap* UT_CKoreanKeyMap::NewL()
    {
    UT_CKoreanKeyMap* self = UT_CKoreanKeyMap::NewLC();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// UT_CKoreanKeyMap::NewLC
// -----------------------------------------------------------------------------
//
UT_CKoreanKeyMap* UT_CKoreanKeyMap::NewLC()
    {
    UT_CKoreanKeyMap* self = new (ELeave) UT_CKoreanKeyMap();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// UT_CKoreanKeyMap::~UT_CKoreanKeyMap
// -----------------------------------------------------------------------------
//
UT_CKoreanKeyMap::~UT_CKoreanKeyMap()
    {
    delete iKeyMap;
    iKeyMap = NULL;
    }

// -----------------------------------------------------------------------------
// UT_CKoreanKeyMap::UT_CKoreanKeyMap
// -----------------------------------------------------------------------------
//
UT_CKoreanKeyMap::UT_CKoreanKeyMap()
    {
    }

// -----------------------------------------------------------------------------
// UT_CKoreanKeyMap::ConstructL
// -----------------------------------------------------------------------------
//
void UT_CKoreanKeyMap::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();

    iKeyMap = CKoreanKeyMap::NewL();
    }

// -----------------------------------------------------------------------------
// UT_CKoreanKeyMap::SetupL
// -----------------------------------------------------------------------------
//
void UT_CKoreanKeyMap::SetupL()
    {
    // Create singleton outside actual test cases so that it is not treated as
    // resource leak, since it can't be deleted.
//    HbKeymapFactory::instance();
    }
    
// -----------------------------------------------------------------------------
// UT_CKoreanKeyMap::Teardown
// -----------------------------------------------------------------------------
//
void UT_CKoreanKeyMap::Teardown()
    {
    }


// TEST CASES

// -----------------------------------------------------------------------------
// UT_CKoreanKeyMap::UT_GetMappedString_JamoL
// -----------------------------------------------------------------------------
//
void UT_CKoreanKeyMap::UT_GetMappedString_JamoL()
	{
	QString koreanInput;
	koreanInput += QChar(0x1100); // key-1
	koreanInput += QChar(0x1101); // key-1 key-#
	koreanInput += QChar(0x1102); // key-2

	QString result = iKeyMap->GetMappedString(koreanInput);

	EUNIT_ASSERT(result == "11b2");
	}

// -----------------------------------------------------------------------------
// UT_CKoreanKeyMap::UT_GetMappedString_HangulL
// -----------------------------------------------------------------------------
//
void UT_CKoreanKeyMap::UT_GetMappedString_HangulL()
    {
    QString koreanInput;
    koreanInput += QChar(0xAC00); // cho:key-1  jung:key-3  jong:none
    koreanInput += QChar(0xAC50); // cho:key-1  jung:key-3 key-*  jong:key-1 key-*

    QString result = iKeyMap->GetMappedString(koreanInput);

    EUNIT_ASSERT(result == "1313a1a");
    
    
    
    
    QString koreanInput2;
    koreanInput2 += QChar(0xD604);

    QString result2 = iKeyMap->GetMappedString(koreanInput2);

    EUNIT_ASSERT(result2 == "8a33a2");
    }

// -----------------------------------------------------------------------------
// UT_CKoreanKeyMap::UT_GetMappedString_MixedL
// -----------------------------------------------------------------------------
//
void UT_CKoreanKeyMap::UT_GetMappedString_MixedL()
    {
    QString koreanInput;
    koreanInput += QChar(0xAC00); // cho:key-1  jung:key-3  jong:none
    koreanInput += QChar(0x1101); // key-1 key-#
    koreanInput += QChar(0x3161); // key-0
    koreanInput += QChar(0xAC50); // cho:key-1  jung:key-3 key-*  jong:key-1 key-*
    koreanInput += QChar(0x1168); // key-3 key-3 key-* key-9
    koreanInput += QChar(0x3138); // key-2 key-* key-#

    QString result = iKeyMap->GetMappedString(koreanInput);

    EUNIT_ASSERT(result == "131b013a1a33a92ab");
    }

// -----------------------------------------------------------------------------
// UT_CKoreanKeyMap::UT_IsLanguageSupported_LatinTextL
// -----------------------------------------------------------------------------
//
void UT_CKoreanKeyMap::UT_IsLanguageSupported_LatinTextL()
    {
    EUNIT_ASSERT_EQUALS(EFalse, iKeyMap->IsLanguageSupported("abcdef ghijk"));
    EUNIT_ASSERT_EQUALS(EFalse, iKeyMap->IsLanguageSupported("123"));
    EUNIT_ASSERT_EQUALS(EFalse, iKeyMap->IsLanguageSupported("-a (h % *"));
    
    // Default behaviour is latin
    EUNIT_ASSERT_EQUALS(EFalse, iKeyMap->IsLanguageSupported(""));
    }

// -----------------------------------------------------------------------------
// UT_CKoreanKeyMap::UT_IsLanguageSupported_KoreanTextL
// -----------------------------------------------------------------------------
//
void UT_CKoreanKeyMap::UT_IsLanguageSupported_KoreanTextL()
    {
    QChar someKoreanChars[] =
        {0x1100, 0x3131, 0x1169, 0x314f, 0x11aa, 0x11b6, 0x3147, 0xac00,
         0xd7af, 0x3162};
    QString koreanText(someKoreanChars, sizeof(someKoreanChars) / sizeof(QChar));
    
    EUNIT_ASSERT_EQUALS(EFalse, iKeyMap->IsLanguageSupported(koreanText));
    }

//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE(
    UT_CKoreanKeyMap,
    "UT_CKoreanKeyMap",
    "UNIT" )

EUNIT_TEST(
    "GetMappedString - test Jamo chars",
    "UT_CKoreanKeyMap",
    "GetMappedStringL",
    "FUNCTIONALITY",
    SetupL, UT_GetMappedString_JamoL, Teardown )

EUNIT_TEST(
    "GetMappedString - test Hangul chars",
    "UT_CKoreanKeyMap",
    "GetMappedStringL",
    "FUNCTIONALITY",
    SetupL, UT_GetMappedString_HangulL, Teardown )

EUNIT_TEST(
    "GetMappedString - test mixed chars",
    "UT_CKoreanKeyMap",
    "GetMappedStringL",
    "FUNCTIONALITY",
    SetupL, UT_GetMappedString_MixedL, Teardown )

EUNIT_TEST(
    "IsLanguageSupported - test latin text",
    "UT_CKoreanKeyMap",
    "IsLanguageSupported",
    "FUNCTIONALITY",
    SetupL, UT_IsLanguageSupported_LatinTextL, Teardown )

EUNIT_TEST(
    "IsLanguageSupported - test korean text",
    "UT_CKoreanKeyMap",
    "IsLanguageSupported",
    "FUNCTIONALITY",
    SetupL, UT_IsLanguageSupported_KoreanTextL, Teardown )

EUNIT_END_TEST_TABLE

//  END OF FILE
