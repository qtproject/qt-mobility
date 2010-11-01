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
    
    
    // Doing this here removes a resource leak from test case that uses Korean text
    QChar koreanChar[] = {0x3131};
    QString koreanText(koreanChar, sizeof(koreanChar) / sizeof(QChar));
    iKeyMap->IsLanguageSupported(koreanText);
    }

// -----------------------------------------------------------------------------
// UT_CKoreanKeyMap::SetupL
// -----------------------------------------------------------------------------
//
void UT_CKoreanKeyMap::SetupL()
    {
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

	EUNIT_ASSERT(result == "112");
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

    EUNIT_ASSERT(result == "13 131 ");
    
    
    
    
    QString koreanInput2;
    koreanInput2 += QChar(0xD604);

    QString result2 = iKeyMap->GetMappedString(koreanInput2);

    EUNIT_ASSERT(result2 == "8332 ");
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

    EUNIT_ASSERT(result == "13 10131 332");
    }

// -----------------------------------------------------------------------------
// UT_CKoreanKeyMap::UT_GetMappedString_TokenizeHangulL
// -----------------------------------------------------------------------------
//
void UT_CKoreanKeyMap::UT_GetMappedString_TokenizeHangulL()
    {
    QString koreanInput;
    koreanInput += QChar(0xAC50); // cho:key-1  jung:key-3 key-*  jong:key-1 key-*
    koreanInput += QChar(0xB070); // cho:key-1 key-#  jung:key-0 key-9  jong:key-5
    koreanInput += QChar(0xB204); // cho:key-2  jung:key-6 key-6  jong:none

    QString result = iKeyMap->GetMappedString(koreanInput);

    EUNIT_ASSERT(result == "131 105 266 ");
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
    // Note: korean text codec does not recognize Hangul Jamo characters (U1100+)
    
    QChar someKoreanChars[] = {0x3131, 0x315a, 0x315b, 0x3155, 0x3155, 0x3155};
    QString koreanText(someKoreanChars, sizeof(someKoreanChars) / sizeof(QChar));
    
    EUNIT_ASSERT_EQUALS(ETrue, iKeyMap->IsLanguageSupported(koreanText));
    
    QChar someKoreanChars2[] = {0xac00, 0xb500, 0x3157, 0xd102, 0xc14f, 0x3163,
            0xd10a, 0xbbcc, 0xac00, 0x314d, 0x3150, 0xd7a3, 0xc995 }; 
    QString koreanText2(someKoreanChars2, sizeof(someKoreanChars2) / sizeof(QChar));
    
    EUNIT_ASSERT_EQUALS(ETrue, iKeyMap->IsLanguageSupported(koreanText2));
    }

// -----------------------------------------------------------------------------
// UT_CKoreanKeyMap::UT_IsLanguageSupported_MixedTextL
// Both Korean and Latin characters. Looks like korean text codec understands
// also latin characters.
// -----------------------------------------------------------------------------
//
void UT_CKoreanKeyMap::UT_IsLanguageSupported_MixedTextL()
    {
    QChar mixedText[] =
        {0xac00, 0xb500, 'a', 0xd102, 0xc14f, 'H', 0xd10a, 0xbbcc, 0xac00,
         0xd7a3, 0xc995, 'B', 'n'};
    QString mixed(mixedText, sizeof(mixedText) / sizeof(QChar));
    
    EUNIT_ASSERT_EQUALS(ETrue, iKeyMap->IsLanguageSupported(mixed));
    
    
    QChar mixedText2[] = {'n', 0xacdf};
    QString mixed2(mixedText2, sizeof(mixedText2) / sizeof(QChar));
    
    EUNIT_ASSERT_EQUALS(ETrue, iKeyMap->IsLanguageSupported(mixed2));
    }


// -----------------------------------------------------------------------------
// UT_CKoreanKeyMap::UT_IsLanguageSupported_TextWithJamoL
// Text includes Jamo characters.
// -----------------------------------------------------------------------------
//
void UT_CKoreanKeyMap::UT_IsLanguageSupported_TextWithJamoL()
    {
    QChar textWithJamo[] = {0xac00, 0x1100, 0x11ff, 0xc14f, 0x1170};
    QString jamoText(textWithJamo, sizeof(textWithJamo) / sizeof(QChar));
    
    EUNIT_ASSERT_EQUALS(ETrue, iKeyMap->IsLanguageSupported(jamoText));
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
    "GetMappedString - test tokenizing Hangul",
    "UT_CKoreanKeyMap",
    "GetMappedStringL",
    "FUNCTIONALITY",
    SetupL, UT_GetMappedString_TokenizeHangulL, Teardown )
    
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

EUNIT_TEST(
    "IsLanguageSupported - test mixed text",
    "UT_CKoreanKeyMap",
    "IsLanguageSupported",
    "FUNCTIONALITY",
    SetupL, UT_IsLanguageSupported_MixedTextL, Teardown )

EUNIT_TEST(
    "IsLanguageSupported - test Jamo chars",
    "UT_CKoreanKeyMap",
    "IsLanguageSupported",
    "FUNCTIONALITY",
    SetupL, UT_IsLanguageSupported_TextWithJamoL, Teardown )
    
EUNIT_END_TEST_TABLE

//  END OF FILE
