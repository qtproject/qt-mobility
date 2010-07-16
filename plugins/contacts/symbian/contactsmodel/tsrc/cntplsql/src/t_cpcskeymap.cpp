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
* Description:  Unit test class for testing C12keyKeyMap
*
*/

// INTERNAL INCLUDES
#include "t_cpcskeymap.h"
#include "c12keykeymap.h"
#include <QString>
#include <QTextCodec>

// SYSTEM INCLUDES
#include <digia/eunit/eunitmacros.h>
#if defined(USE_ORBIT_KEYMAP)
#include <hbinputkeymapfactory.h>
#endif


// 10.1 wk12 SDK's Thai keymap does not map anything to *,# keys
// so must use either a hardcoded keymap, or replace the SDK's keymap
// with a newer one by copying the keymap file 120.txt to
// \epoc32\release\winscw\udeb\z\resource\keymaps

/* New Thai keymap, not yet present in the wk12 SDK
1   x0E01\x0E02\x0E03\x0E04\x0E05\x0031\t\x0E01\x0E02\x0E03\x0E04\x0E05\x0031"
2   x0E06\x0E07\x0E08\x0E09\x0032\t\x0E06\x0E07\x0E08\x0E09\x0032"
3   x0E0A\x0E0B\x0E0C\x0E0D\x0033\t\x0E0A\x0E0B\x0E0C\x0E0D\x0033"
4   x0E0E\x0E0F\x0E10\x0E11\x0E12\x0E13\x0034\t\x0E0E\x0E0F\x0E10\x0E11\x0E12\x0E13\x0034"
5   x0E14\x0E15\x0E16\x0E17\x0E18\x0035\t\x0E14\x0E15\x0E16\x0E17\x0E18\x0035"
6   x0E19\x0E1A\x0E1B\x0E1C\x0E1D\x0036\t\x0E19\x0E1A\x0E1B\x0E1C\x0E1D\x0036"
7   x0E1E\x0E1F\x0E20\x0E21\x0E22\x0037\t\x0E1E\x0E1F\x0E20\x0E21\x0E22\x0037"
8   x0E23\x0E24\x0E25\x0E26\x0E27\x0E28\x0E29\x0038\t\x0E23\x0E24\x0E25\x0E26\x0E27\x0E28\x0E29\x0038"
9   x0E2A\x0E2B\x0E2C\x0E2D\x0E2E\x0039\t\x0E2A\x0E2B\x0E2C\x0E2D\x0E2E\x0039"
0   0\x21B2\t 0\x21B2"
*   x0E2F\x0E3F\x0E31\x0E34\x0E35\x0E36\x0E37\x0E38\x0E39\x0E46\x0E47\x0E48\x0E49\x0E4A\x0E4B\x0E4C\t\x0E2F\x0E3F\x0E31\x0E34\x0E35\x0E36\x0E37\x0E38\x0E39\x0E46\x0E47\x0E48\x0E49\x0E4A\x0E4B\x0E4C"
#   x0E30\x0E32\x0E33\x0E40\x0E41\x0E42\x0E43\x0E44\t\x0E30\x0E32\x0E33\x0E40\x0E41\x0E42\x0E43\x0E44" */
const TChar KThaiChar0 = 0x21B2; // char from key-0
const TChar KThaiChar1 = 0x0E05; // char from key-1
const TChar KThaiChar2 = 0x0E06; // char from key-2
const TChar KThaiChar3 = 0x0E0A; // char from key-3
const TChar KThaiChar4 = 0x0E0E; // char from key-4
const TChar KThaiChar5 = 0x0E14; // char from key-5
const TChar KThaiChar6 = 0x0E19; // char from key-6
const TChar KThaiChar7 = 0x0E1E; // char from key-7
const TChar KThaiChar8 = 0x0E23; // char from key-8
const TChar KThaiChar9 = 0x0E2A; // char from key-9
const TChar KThaiChar10 = 0x0E34; // char from key-*
const TChar KThaiChar11 = 0x0E41; // char from key-#

    
// -----------------------------------------------------------------------------
// UT_CPcsKeyMap::NewL
// -----------------------------------------------------------------------------
//
UT_CPcsKeyMap* UT_CPcsKeyMap::NewL()
    {
    UT_CPcsKeyMap* self = UT_CPcsKeyMap::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// UT_CPcsKeyMap::NewLC
// -----------------------------------------------------------------------------
//
UT_CPcsKeyMap* UT_CPcsKeyMap::NewLC()
    {
    UT_CPcsKeyMap* self = new( ELeave ) UT_CPcsKeyMap();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// UT_CPcsKeyMap::~UT_CPcsKeyMap
// -----------------------------------------------------------------------------
//
UT_CPcsKeyMap::~UT_CPcsKeyMap()
    {
    delete iKeyMap;
    iKeyMap = NULL;
    }

// -----------------------------------------------------------------------------
// UT_CPcsKeyMap::UT_CPcsKeyMap
// -----------------------------------------------------------------------------
//
UT_CPcsKeyMap::UT_CPcsKeyMap()
    {
    }

// -----------------------------------------------------------------------------
// UT_CPcsKeyMap::ConstructL
// -----------------------------------------------------------------------------
//
void UT_CPcsKeyMap::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    
    
    // When instantiating keymap was moved to here, it removed a resource leak.
    iKeyMap = C12keyKeyMap::NewL();
    
    
    
    // EUnit shows that the first call to Thai text codec's canEncode() leaks
    // 10 cells. That's why we call canEncode() here, so when the actual test
    // calls canEncode(), the leak no longer happens.
    const int KThaiLanguage = 2259; // Thai Industrial Standards Institute
    QTextCodec* thaiCodec = QTextCodec::codecForMib(KThaiLanguage);
    if (thaiCodec)
        {
        thaiCodec->canEncode('a');
        }
    }

// -----------------------------------------------------------------------------
// UT_CPcsKeyMap::SetupL
// -----------------------------------------------------------------------------
//
void UT_CPcsKeyMap::SetupL()
    {
#if defined(USE_ORBIT_KEYMAP)
    // Create singleton outside actual test cases so that it is not treated as
    // resource leak, since it can't be deleted.
    HbKeymapFactory::instance();
#endif
    }

// -----------------------------------------------------------------------------
// UT_CPcsKeyMap::Teardown
// -----------------------------------------------------------------------------
//
void UT_CPcsKeyMap::Teardown()
    {
    }


// TEST CASES

// -----------------------------------------------------------------------------
// UT_CPcsKeyMap::UT_NewLL
// -----------------------------------------------------------------------------
//
void UT_CPcsKeyMap::UT_NewLL()
    {
#if defined(USE_ORBIT_KEYMAP)
	// Must be same as in c12keykeymap.cpp
	const TInt KAmountOfKeys = 12;

    // Each numeric key has been mapped
    EUNIT_ASSERT_EQUALS( KAmountOfKeys, iKeyMap->iKeyMapping.count() );
    for (TInt i = 0; i < KAmountOfKeys; ++i)
        {
        EUNIT_ASSERT( iKeyMap->iKeyMapping.at(i).length() > 0 );
        }
#endif
    }

// -----------------------------------------------------------------------------
// UT_CPcsKeyMap::UT_GetMappedStringL
// -----------------------------------------------------------------------------
//
void UT_CPcsKeyMap::UT_GetMappedStringL()
	{
	_LIT( KAlpha, "abjqmca" );
	_LIT( KNumeric, "2257622" );
	HBufC* numericBuf = iKeyMap->GetMappedStringL( KAlpha );
	CleanupStack::PushL( numericBuf );
	EUNIT_ASSERT_EQUALS( *numericBuf, KNumeric );
	CleanupStack::PopAndDestroy( numericBuf );
	numericBuf = NULL;

	
	_LIT( KAlphaOneChar, "m" );
	_LIT( KNumericOneChar, "6" );
	numericBuf = iKeyMap->GetMappedStringL( KAlphaOneChar );
	CleanupStack::PushL( numericBuf );
	EUNIT_ASSERT_EQUALS( *numericBuf, KNumericOneChar );
	CleanupStack::PopAndDestroy( numericBuf );
	numericBuf = NULL;
	

	_LIT( KAlphaWithSpaces, "kjhgfdsa qwert " );
	_LIT( KNumericWithSpaces, "55443372 79378 " );
	numericBuf = iKeyMap->GetMappedStringL( KAlphaWithSpaces );
    CleanupStack::PushL( numericBuf );
    EUNIT_ASSERT_EQUALS( *numericBuf, KNumericWithSpaces );
    CleanupStack::PopAndDestroy( numericBuf );
    
    
    _LIT( KAlphaLongString, "adg   1230 0zbzb0 0 e56e101at 00  " );
    _LIT( KNumericLongString, "234   1230 092920 0 356310128 00  " );
    numericBuf = iKeyMap->GetMappedStringL( KAlphaLongString );
    CleanupStack::PushL( numericBuf );
    EUNIT_ASSERT_EQUALS( *numericBuf, KNumericLongString );
    CleanupStack::PopAndDestroy( numericBuf );
	}

// -----------------------------------------------------------------------------
// UT_CPcsKeyMap::UT_GetMappedStringSeparatorL
// -----------------------------------------------------------------------------
//
void UT_CPcsKeyMap::UT_GetMappedStringSeparatorL()
    {
    _LIT( KAlpha, "abjqmca" );
    _LIT( KNumeric, "2257622" );
    HBufC* numericBuf = iKeyMap->GetMappedStringL( KAlpha );
    CleanupStack::PushL( numericBuf );
    EUNIT_ASSERT_EQUALS( *numericBuf, KNumeric );
    CleanupStack::PopAndDestroy( numericBuf );
    numericBuf = NULL;


    _LIT( KAlphaOneChar, "m" );
    _LIT( KNumericOneChar, "6" );
    numericBuf = iKeyMap->GetMappedStringL( KAlphaOneChar );
    CleanupStack::PushL( numericBuf );
    EUNIT_ASSERT_EQUALS( *numericBuf, KNumericOneChar );
    CleanupStack::PopAndDestroy( numericBuf );
    numericBuf = NULL;
    

    _LIT( KAlphaWithSpaces, " kjh gfdsa qwert " );
    _LIT( KNumericWithSpaces, " 554 43372 79378 " ); // The leading space remains
    numericBuf = iKeyMap->GetMappedStringL( KAlphaWithSpaces );
    CleanupStack::PushL( numericBuf );
    EUNIT_ASSERT_EQUALS( *numericBuf, KNumericWithSpaces );
    CleanupStack::PopAndDestroy( numericBuf );
    

    _LIT( KAlphaLongString, "adg   1230 0zbzb0 0 e56e101at 00  " );
    _LIT( KNumericLongString, "234   1230 092920 0 356310128 00  " );  
    numericBuf = iKeyMap->GetMappedStringL( KAlphaLongString );
    CleanupStack::PushL( numericBuf );
    EUNIT_ASSERT_EQUALS( *numericBuf, KNumericLongString );
    CleanupStack::PopAndDestroy( numericBuf );
    }

// -----------------------------------------------------------------------------
// UT_CPcsKeyMap::UT_GetMappedStringWithNumbersL
// Source string contains numbers
// -----------------------------------------------------------------------------
//
void UT_CPcsKeyMap::UT_GetMappedStringWithNumbersL()
    {
    _LIT( KAlpha, "11098" );
    _LIT( KNumeric, "11098" );
    HBufC* numericBuf = iKeyMap->GetMappedStringL( KAlpha );
    CleanupStack::PushL( numericBuf );
    EUNIT_ASSERT_EQUALS( *numericBuf, KNumeric );
    CleanupStack::PopAndDestroy(numericBuf);
    }

// -----------------------------------------------------------------------------
// UT_CPcsKeyMap::UT_GetMappedStringWithSpecialCharactersL
// Source string contains numbers
// -----------------------------------------------------------------------------
//
void UT_CPcsKeyMap::UT_GetMappedStringWithSpecialCharactersL()
    {
	_LIT( KAlpha, " +g-*[#5]?t" );
	_LIT( KNumeric, " a41afb5f18" );
    HBufC* numericBuf = iKeyMap->GetMappedStringL( KAlpha );
    CleanupStack::PushL( numericBuf );
    EUNIT_ASSERT_EQUALS( *numericBuf, KNumeric );
    CleanupStack::PopAndDestroy(numericBuf);
    }

// -----------------------------------------------------------------------------
// UT_CPcsKeyMap::UT_GetMappedStringThaiLanguageOneCharL
// Source string contains one Thai character.
// -----------------------------------------------------------------------------
//
void UT_CPcsKeyMap::UT_GetMappedStringThaiLanguageOneCharL()
    {
    HBufC* thaiText = HBufC::NewLC(1);
    TPtr16 p = thaiText->Des();
    p.Append(KThaiChar2);
    _LIT(KNumeric, "2");
    HBufC* numericBuf = iKeyMap->GetMappedStringL(*thaiText);
    CleanupStack::PopAndDestroy(thaiText);
    CleanupStack::PushL(numericBuf);
    EUNIT_ASSERT_EQUALS(*numericBuf, KNumeric);
    CleanupStack::PopAndDestroy(numericBuf);
    
    
    QString source;
    QChar thaiChar3(KThaiChar3);
    QChar thaiChar5(KThaiChar5);
    source.append(thaiChar3);
    source.append(thaiChar5);
    QString result = iKeyMap->GetMappedString(source);
    EUNIT_ASSERT(result == "35");
    }


// -----------------------------------------------------------------------------
// UT_CPcsKeyMap::UT_GetMappedStringThaiLanguageL
// Source string is Thai.
// -----------------------------------------------------------------------------
//
void UT_CPcsKeyMap::UT_GetMappedStringThaiLanguageL()
    {
    HBufC* thaiText = HBufC::NewLC(20);
    TPtr16 p = thaiText->Des();
    p.Append(KThaiChar10);
    p.Append(KThaiChar1);
    p.Append(KThaiChar2);
    p.Append(KThaiChar3);
    p.Append(KThaiChar11);
    p.Append(KThaiChar4);
    p.Append(KThaiChar5);
    p.Append(KThaiChar6);
    p.Append(KThaiChar11);
    p.Append(KThaiChar10);
    p.Append(KThaiChar7);
    p.Append(KThaiChar8);
    p.Append(KThaiChar9);
    p.Append(KThaiChar11);
    // For some reason, the Thai text codec indicates KThaiChar0 as invalid
    // character, so that's why it is not used here.

    _LIT(KNumeric, "123456789"); // keys * and # are skipped from result
    HBufC* numericBuf = iKeyMap->GetMappedStringL(*thaiText);
    CleanupStack::PopAndDestroy(thaiText);
    CleanupStack::PushL(numericBuf);

    EUNIT_ASSERT_EQUALS(*numericBuf, KNumeric);
    CleanupStack::PopAndDestroy(numericBuf);
    }

// -----------------------------------------------------------------------------
// UT_CPcsKeyMap::UT_GetMappedStringThaiLanguageEmptyResultL
// Input string only contains characters that map to * and # keys.
// -----------------------------------------------------------------------------
//
void UT_CPcsKeyMap::UT_GetMappedStringThaiLanguageEmptyResultL()
    {
    HBufC* thaiText = HBufC::NewLC(20);
    TPtr16 p = thaiText->Des();
    p.Append(KThaiChar11);
    p.Append(KThaiChar10);
    p.Append(KThaiChar10);
    p.Append(KThaiChar11);
    p.Append(KThaiChar11);
    p.Append(KThaiChar11);
    p.Append(KThaiChar10);
    p.Append(KThaiChar11);
    p.Append(KThaiChar10);
    p.Append(KThaiChar10);
    p.Append(KThaiChar11);
    p.Append(KThaiChar10);

    HBufC* numericBuf = iKeyMap->GetMappedStringL(*thaiText);
    CleanupStack::PopAndDestroy(thaiText);
    CleanupStack::PushL(numericBuf);

    EUNIT_ASSERT_EQUALS(*numericBuf, KNullDesC);
    CleanupStack::PopAndDestroy(numericBuf);
    }

// -----------------------------------------------------------------------------
// UT_CPcsKeyMap::UT_GetNumericLimitsL
// -----------------------------------------------------------------------------
//
void UT_CPcsKeyMap::UT_GetNumericLimitsL()
	{
	QString lowerLimit;
	QString upperLimit;
	TInt err = iKeyMap->GetNumericLimits("123", lowerLimit, upperLimit);
	QString expectedLowerLimit("81909218222800895");
	QString expectedUpperLimit("82190693199511552");
	EUNIT_ASSERT(lowerLimit == expectedLowerLimit);
	EUNIT_ASSERT(upperLimit == expectedUpperLimit);
	}

// -----------------------------------------------------------------------------
// UT_CPcsKeyMap::UT_GetNumericLimitsLongInputL
// -----------------------------------------------------------------------------
//
void UT_CPcsKeyMap::UT_GetNumericLimitsLongInputL()
    {
    QString lowerLimit;
    QString upperLimit;
    TInt err = iKeyMap->GetNumericLimits("1234567890ab123456789", lowerLimit, upperLimit);
    QString expectedLowerLimit("81985529205928226");
    QString expectedUpperLimit("81985529205928228");
    EUNIT_ASSERT(lowerLimit == expectedLowerLimit);
    EUNIT_ASSERT(upperLimit == expectedUpperLimit);
    }


//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE(
    UT_CPcsKeyMap,
    "UT_CPcsKeyMap",
    "UNIT" )

EUNIT_TEST(
    "NewL - test ",
    "UT_CPcsKeyMap",
    "NewL",
    "FUNCTIONALITY",
    SetupL, UT_NewLL, Teardown )

EUNIT_TEST(
    "GetMappedString - test ",
    "UT_CPcsKeyMap",
    "GetMappedStringL",
    "FUNCTIONALITY",
    SetupL, UT_GetMappedStringL, Teardown )

EUNIT_TEST(
    "GetMappedString - test separator",
    "UT_CPcsKeyMap",
    "GetMappedStringL",
    "FUNCTIONALITY",
    SetupL, UT_GetMappedStringSeparatorL, Teardown )

EUNIT_TEST(
    "GetMappedString - test numbers",
    "UT_CPcsKeyMap",
    "GetMappedStringL",
    "FUNCTIONALITY",
    SetupL, UT_GetMappedStringWithNumbersL, Teardown )

EUNIT_TEST(
    "GetMappedString - test special characters",
    "UT_CPcsKeyMap",
    "GetMappedStringL",
    "FUNCTIONALITY",
    SetupL, UT_GetMappedStringWithSpecialCharactersL, Teardown )

EUNIT_TEST(
    "GetMappedString - Thai, test one character input",
    "UT_CPcsKeyMap",
    "GetMappedStringL",
    "FUNCTIONALITY",
    SetupL, UT_GetMappedStringThaiLanguageOneCharL, Teardown )

EUNIT_TEST(
    "GetMappedString - test Thai language",
    "UT_CPcsKeyMap",
    "GetMappedStringL",
    "FUNCTIONALITY",
    SetupL, UT_GetMappedStringThaiLanguageL, Teardown )

EUNIT_TEST(
    "GetMappedString - Thai, empty result",
    "UT_CPcsKeyMap",
    "GetMappedStringL",
    "FUNCTIONALITY",
    SetupL, UT_GetMappedStringThaiLanguageEmptyResultL, Teardown )

EUNIT_TEST(
    "GetNumericLimits",
    "UT_CPcsKeyMap",
    "GetNumericLimits",
    "FUNCTIONALITY",
    SetupL, UT_GetNumericLimitsL, Teardown )

EUNIT_TEST(
    "GetNumericLimits - too long input",
    "UT_CPcsKeyMap",
    "GetNumericLimits",
    "FUNCTIONALITY",
    SetupL, UT_GetNumericLimitsLongInputL, Teardown )

EUNIT_END_TEST_TABLE

//  END OF FILE
