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
#include "t_cqwertykeymap.h"
#include "cqwertykeymap.h"
#include <QString>

// SYSTEM INCLUDES
#include <digia/eunit/eunitmacros.h>
#include <hbinputkeymapfactory.h>

    
// -----------------------------------------------------------------------------
// UT_CQwertyKeyMap::NewL
// -----------------------------------------------------------------------------
//
UT_CQwertyKeyMap* UT_CQwertyKeyMap::NewL()
    {
    UT_CQwertyKeyMap* self = UT_CQwertyKeyMap::NewLC();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// UT_CQwertyKeyMap::NewLC
// -----------------------------------------------------------------------------
//
UT_CQwertyKeyMap* UT_CQwertyKeyMap::NewLC()
    {
    UT_CQwertyKeyMap* self = new (ELeave) UT_CQwertyKeyMap();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// UT_CQwertyKeyMap::~UT_CQwertyKeyMap
// -----------------------------------------------------------------------------
//
UT_CQwertyKeyMap::~UT_CQwertyKeyMap()
    {
    delete iKeyMap;
    iKeyMap = NULL;
    }

// -----------------------------------------------------------------------------
// UT_CQwertyKeyMap::UT_CQwertyKeyMap
// -----------------------------------------------------------------------------
//
UT_CQwertyKeyMap::UT_CQwertyKeyMap()
    {
    }

// -----------------------------------------------------------------------------
// UT_CQwertyKeyMap::ConstructL
// -----------------------------------------------------------------------------
//
void UT_CQwertyKeyMap::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    
    
    // When instantiating keymap was moved to here, it removed a resource leak.
    iKeyMap = CQwertyKeyMap::NewL();
    }

// -----------------------------------------------------------------------------
// UT_CQwertyKeyMap::SetupL
// -----------------------------------------------------------------------------
//
void UT_CQwertyKeyMap::SetupL()
    {
    // Create singleton outside actual test cases so that it is not treated as
    // resource leak, since it can't be deleted.
    HbKeymapFactory::instance();
    }
    
// -----------------------------------------------------------------------------
// UT_CQwertyKeyMap::Teardown
// -----------------------------------------------------------------------------
//
void UT_CQwertyKeyMap::Teardown()
    {
    }


// TEST CASES

// -----------------------------------------------------------------------------
// UT_CQwertyKeyMap::UT_NewLL
// -----------------------------------------------------------------------------
//
void UT_CQwertyKeyMap::UT_NewLL()
    {
    // Note: depending on the language, the keymaps have variable amount of keys.
    // Check that at least minimum amount of keys has been mapped.
    const TInt KMinimumAmountOfKeysInUse = 32;
    
    EUNIT_ASSERT(iKeyMap->iAmountOfKeys >= KMinimumAmountOfKeysInUse);

    TInt keys = iKeyMap->iKeyMapping.count();
	EUNIT_ASSERT(keys >= KMinimumAmountOfKeysInUse);
	
    for (TInt i = 0; i < KMinimumAmountOfKeysInUse; ++i)
        {
        EUNIT_ASSERT(iKeyMap->iKeyMapping.at(i).length() > 0);
        }
    }

// -----------------------------------------------------------------------------
// UT_CQwertyKeyMap::UT_GetMappedStringL
// -----------------------------------------------------------------------------
//
void UT_CQwertyKeyMap::UT_GetMappedStringL()
	{
	_LIT(KAlpha, "AbCQWe");
	_LIT(KNumeric, "abcqwe");
	HBufC* numericBuf = iKeyMap->GetMappedStringL( KAlpha );
	CleanupStack::PushL( numericBuf );
	EUNIT_ASSERT_EQUALS( *numericBuf, KNumeric );
	CleanupStack::PopAndDestroy( numericBuf );
	numericBuf = NULL;
	}

// -----------------------------------------------------------------------------
// UT_CQwertyKeyMap::UT_GetNumericLimitsL
// -----------------------------------------------------------------------------
//
void UT_CQwertyKeyMap::UT_GetNumericLimitsL()
    {
    QString lowerLimit;
    QString upperLimit; 
    TInt err = iKeyMap->GetNumericLimits("we", lowerLimit, upperLimit);
    QString expectedLowerLimit("18577348462903295");
    QString expectedUpperLimit("18858823439613952");
    EUNIT_ASSERT(lowerLimit == expectedLowerLimit);
    EUNIT_ASSERT(upperLimit == expectedUpperLimit);
    }

// -----------------------------------------------------------------------------
// UT_CQwertyKeyMap::UT_GetNumericLimitsBeginsByFirstKeyL
// Input string begins by the first key (key Q)
// -----------------------------------------------------------------------------
//
void UT_CQwertyKeyMap::UT_GetNumericLimitsBeginsByFirstKeyL()
    {
    QString lowerLimit;
    QString upperLimit;
    TInt err = iKeyMap->GetNumericLimits("qwe", lowerLimit, upperLimit);
    QString expectedLowerLimit("290271069732863");
    QString expectedUpperLimit("294669116243968");
    EUNIT_ASSERT(lowerLimit == expectedLowerLimit);
    EUNIT_ASSERT(upperLimit == expectedUpperLimit);
    }

// -----------------------------------------------------------------------------
// UT_CQwertyKeyMap::UT_GetNumericLimitsLongValueL
// Input string has more than maximum characters
// -----------------------------------------------------------------------------
//
void UT_CQwertyKeyMap::UT_GetNumericLimitsLongValueL()
    {
    QString lowerLimit;
    QString upperLimit;
    // "aaqqsswwddee" => "ee" is ignored, rest are as binary:
    // 001010 001010 000000 000000 001011 001011 000001 000001 001100 001100
    // =
    // 0010 1000 1010 0000 0000 0000 0010 1100 1011 0000 0100 0001 0011 0000 1100
    // as hex = 28A0002CB04130C, as decimal = 182958746857902860
    TInt err = iKeyMap->GetNumericLimits("aaqqsswwddee", lowerLimit, upperLimit);
    QString expectedLowerLimit("182958746857902859");
    QString expectedUpperLimit("182958746857902861");
    EUNIT_ASSERT(lowerLimit == expectedLowerLimit);
    EUNIT_ASSERT(upperLimit == expectedUpperLimit);
    }

// -----------------------------------------------------------------------------
// UT_CQwertyKeyMap::UT_CheckDigitsAreMappedL
// Note: this test case works only for languages using 0,1,2..9 as decimal
// numbers.
// -----------------------------------------------------------------------------
//
void UT_CQwertyKeyMap::UT_CheckDigitsAreMappedL()
    {
    TInt keys = iKeyMap->iKeyMapping.count();
    const TInt KAmountOfDecimalDigits = 10;
    TBool hasDigitBeenMapped[ KAmountOfDecimalDigits ] = { 0 };
    
    for (TInt i = 0; i < keys; ++i)
        {
        for (TInt digit = 0; digit < KAmountOfDecimalDigits; ++digit)
            {
            QChar ch(digit + '0');
            if (iKeyMap->iKeyMapping[i].contains(ch))
                {
                hasDigitBeenMapped[digit] = ETrue;
                }
            }
        }
    
    // Check every decimal digit was mapped
    for (TInt digit = 0; digit < KAmountOfDecimalDigits; ++digit)
        {
        QChar ch(digit + '0');
        EUNIT_ASSERT(hasDigitBeenMapped[digit]);
        }
    
    // TODO: handle all languages present
    }

// -----------------------------------------------------------------------------
// UT_CQwertyKeyMap::UT_CheckAmountOfMappedKeysL
// Check the internal qwerty keymap does not have unnecessary (empty) entries.
// -----------------------------------------------------------------------------
//
void UT_CQwertyKeyMap::UT_CheckAmountOfMappedKeysL()
    {
    TInt amountOfMappedKeys = iKeyMap->iAmountOfKeys;
    
    EUNIT_ASSERT_EQUALS(amountOfMappedKeys, iKeyMap->iKeyNames.count());
    EUNIT_ASSERT_EQUALS(amountOfMappedKeys, iKeyMap->iKeyValues.count());
    //EUNIT_ASSERT_EQUALS(amountOfMappedKeys, iKeyMap->iKeyMapping.count());
    }

// -----------------------------------------------------------------------------
// UT_CQwertyKeyMap::UT_CheckSpecialCharactersL
// Check that the special characters from HbKeyboardSctLandscape are mapped.
// -----------------------------------------------------------------------------
//
void UT_CQwertyKeyMap::UT_CheckSpecialCharactersL()
    {
    // NOTE: these test cases pass in english, but not necesary with other
    // languages, as each language has its own keymap with some differences.

    _LIT(KInputWithSpecialChars, "<A1>C#W*2-($)&");
    _LIT(KMappedResult, "#a1#c#w#2-###&");
    HBufC* mapped = iKeyMap->GetMappedStringL(KInputWithSpecialChars);
    CleanupStack::PushL(mapped);
    EUNIT_ASSERT_EQUALS(*mapped, KMappedResult);
    CleanupStack::PopAndDestroy(mapped);
    mapped = NULL;
    
    
    // Those special characters (e.g '.') that are present in virtual
    // QWERTY keymap, have been mapped to individual keys.
    // That depends on the used language.
    
    _LIT(KInputWithSpecialChars2, ".8Hg %01 kK£");
    _LIT(KMappedResult2, ".8hg #01 kk#");
    mapped = iKeyMap->GetMappedStringL(KInputWithSpecialChars2);
    CleanupStack::PushL(mapped);
    EUNIT_ASSERT_EQUALS(*mapped, KMappedResult2);
    CleanupStack::PopAndDestroy(mapped);
    mapped = NULL;
    }

//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE(
    UT_CQwertyKeyMap,
    "UT_CQwertyKeyMap",
    "UNIT" )

EUNIT_TEST(
    "NewL - test ",
    "UT_CQwertyKeyMap",
    "NewL",
    "FUNCTIONALITY",
    SetupL, UT_NewLL, Teardown )

EUNIT_TEST(
    "GetMappedString - test ",
    "UT_CQwertyKeyMap",
    "GetMappedStringL",
    "FUNCTIONALITY",
    SetupL, UT_GetMappedStringL, Teardown )

EUNIT_TEST(
    "GetNumericLimits",
    "UT_CQwertyKeyMap",
    "GetNumericLimits",
    "FUNCTIONALITY",
    SetupL, UT_GetNumericLimitsL, Teardown )

EUNIT_TEST(
    "GetNumericLimits - begins by first key",
    "UT_CQwertyKeyMap",
    "GetNumericLimits",
    "FUNCTIONALITY",
    SetupL, UT_GetNumericLimitsBeginsByFirstKeyL, Teardown )
    
EUNIT_TEST(
    "GetNumericLimits - long input",
    "UT_CQwertyKeyMap",
    "GetNumericLimits",
    "FUNCTIONALITY",
    SetupL, UT_GetNumericLimitsLongValueL, Teardown )

EUNIT_TEST(
    "Check decimal digits are mapped",
    "UT_CQwertyKeyMap",
    "",
    "FUNCTIONALITY",
    SetupL, UT_CheckDigitsAreMappedL, Teardown )

EUNIT_TEST(
    "Check QWERTY keymap does not contain empty entries",
    "UT_CQwertyKeyMap",
    "",
    "FUNCTIONALITY",
    SetupL, UT_CheckAmountOfMappedKeysL, Teardown )

EUNIT_TEST(
    "Check special characters from SCT keymap have been mapped",
    "UT_CQwertyKeyMap",
    "",
    "FUNCTIONALITY",
    SetupL, UT_CheckSpecialCharactersL, Teardown )

EUNIT_END_TEST_TABLE

//  END OF FILE
