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
#if defined(USE_ORBIT_KEYMAP)
#include <hbinputkeymapfactory.h>
#endif

    
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
#if defined(USE_ORBIT_KEYMAP)
    // Create singleton outside actual test cases so that it is not treated as
    // resource leak, since it can't be deleted.
    HbKeymapFactory::instance();
#endif
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
#if defined(USE_ORBIT_KEYMAP)
    // Check each key has been mapped
	EUNIT_ASSERT_EQUALS(CQwertyKeyMap::EAmountOfKeysInQwertyKeypad,
					    iKeyMap->iKeyMapping.count());

	const TInt KAmountOfKeysInUse = 32;
	// Note: depending on the language, the keymaps have 32..36 keys.
	// So far no language has the full 44 keys.
    for (TInt i = 0; i < KAmountOfKeysInUse; ++i)
        {
        EUNIT_ASSERT(iKeyMap->iKeyMapping.at(i).length() > 0);
        }
#endif
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
    
EUNIT_END_TEST_TABLE

//  END OF FILE
