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
* Description: Retrieves the character map for each of the numeric keys.
*/

// INCLUDE FILES
#include "c12keykeymap.h"
#include "predictivesearchlog.h"

// Detection for Thai language is needed for both hardcoded and Orbit keymaps
#include <QTextCodec>
#include <hbinputkeymapfactory.h>
//#include <hbinputsettingproxy.h>

// How many keys have mappings in ITU-T keypad (keys 0..9, * and # have mappings)
const TInt KAmountOfKeys = 12;

// Largest amount of keypresses that can be stored in 12-key keyboard's
// predictive search tables.
// SQL BIGINT is a 64-bit signed integer, one bit is reserved for sign.
// 12-key keyboard's keys are identified by TKeyId that needs 4 bits.
// 63 / 4 = 15
const TInt KMaxKeysStoredInDb = 15;


// The first key of the keyboard has value zero ('1' in the 12-key virtual keypad) 
enum TKeyId
    {
    EKey1 = 0,
    EKey2,
    EKey3,
    EKey4,
    EKey5,
    EKey6,
    EKey7,
    EKey8,
    EKey9,
	EKey0,
	EKeyStar,
	EKeyHash
    };

#if defined(USE_ORBIT_KEYMAP)
const QChar KStar = '*';
const QChar KPlus = '+';
const QChar KHash = '#';
#endif // #if defined(USE_ORBIT_KEYMAP)


// * key is mapped to this
const QChar KMappedCharForStar = 'a';
// # key is mapped to this
const QChar KMappedCharForHash = 'b';
// Unmapped (unknown) characters are replaced with this
const QChar KPadChar		   = 'f';

const QChar KLowerLimitPadding = '0';
const QChar KUpperLimitPadding = 'f';

const int KLatin1 = 4; // ISO_8859-1
const int KThaiLanguage = 2259; // Thai Industrial Standards Institute


// ============================== MEMBER FUNCTIONS ============================

// ----------------------------------------------------------------------------
// C12keyKeyMap::NewL
// ----------------------------------------------------------------------------
C12keyKeyMap* C12keyKeyMap::NewL()
	{
    PRINT(_L("Enter C12keyKeyMap::NewL"));
    
    C12keyKeyMap* self = new (ELeave) C12keyKeyMap();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    PRINT(_L("End C12keyKeyMap::NewL"));
    return self;
	}

// ----------------------------------------------------------------------------
// C12keyKeyMap::~C12keyKeyMap
// QTextCodec's destructor is protected, so it is not intended to be deleted.
// ----------------------------------------------------------------------------
C12keyKeyMap::~C12keyKeyMap()
    {
    PRINT(_L("Enter C12keyKeyMap::~C12keyKeyMap"));
    PRINT(_L("End C12keyKeyMap::~C12keyKeyMap"));
    }

// ----------------------------------------------------------------------------
// C12keyKeyMap::ArrayIndexToMappedChar
// Map index of iKeyMapping list, to the number char that the mapping is for.
// ----------------------------------------------------------------------------
const QChar C12keyKeyMap::ArrayIndexToMappedChar(TInt aArrayIndex) const
    {
    __ASSERT_DEBUG(aArrayIndex < KAmountOfKeys,
                   User::Panic(_L("C12keyKeyMap::ArrayIndexToMappedChar"),
                   KErrOverflow));
    switch (aArrayIndex)
        {
        case EKey0:
            return '0';
        case EKeyStar:
            return KMappedCharForStar;
        case EKeyHash:
            return KMappedCharForHash;
        default:
            return aArrayIndex + '1';
        }
    }

#if !defined(USE_ORBIT_KEYMAP)
// ----------------------------------------------------------------------------
// C12keyKeyMap::UseHardcodedKeyMap
// ----------------------------------------------------------------------------
QChar C12keyKeyMap::UseHardcodedKeyMap(QChar input) const
    {
	QChar ret = '0';
    switch (input.unicode())
        {
#if defined(THAI_KEYMAP)
		case 0x21B2:
			ret = '0';
			break;

		case 0x0E01:
		case 0x0E02:
		case 0x0E03:
		case 0x0E04:
		case 0x0E05:	
#endif
		case '-':
		case '?':
		    ret = '1';
		    break;

        case 'a':
        case 'A':
        case 'b':
        case 'B':
        case 'c':
        case 'C':
#if defined(THAI_KEYMAP)
		case 0x0E06:
		case 0x0E07:
		case 0x0E08:
		case 0x0E09:
#endif
            ret = '2';
            break;

		case 'd':
        case 'D':
        case 'e':
        case 'E':
        case 'f':
        case 'F':
#if defined(THAI_KEYMAP)
		case 0x0E0A:
		case 0x0E0B:
		case 0x0E0C:
		case 0x0E0D:
#endif
            ret = '3';
            break;
            
		case 'g':
        case 'G':
        case 'h':
        case 'H':
        case 'i':
        case 'I':
#if defined(THAI_KEYMAP)
		case 0x0E0E:
		case 0x0E0F:
		case 0x0E10:
		case 0x0E11:
		case 0x0E12:
		case 0x0E13:
#endif
            ret = '4';
            break;

		case 'j':
        case 'J':
        case 'k':
        case 'K':
        case 'l':
        case 'L':
#if defined(THAI_KEYMAP)
		case 0x0E14:
		case 0x0E15:
		case 0x0E16:
		case 0x0E17:
		case 0x0E18:
#endif
            ret = '5';
            break;

		case 'm':
        case 'M':
        case 'n':
        case 'N':
        case 'o':
        case 'O':
#if defined(THAI_KEYMAP)
		case 0x0E19:
		case 0x0E1A:
		case 0x0E1B:
		case 0x0E1C:
		case 0x0E1D:
#endif
            ret = '6';
            break;
      
		case 'p':
        case 'P':
        case 'q':
        case 'Q':
        case 'r':
        case 'R':
        case 's':
        case 'S':
#if defined(THAI_KEYMAP)
		case 0x0E1E:
		case 0x0E1F:
		case 0x0E20:
		case 0x0E21:
		case 0x0E22:
#endif
            ret = '7';
            break;

		case 't':
        case 'T':
        case 'u':
        case 'U':
        case 'v':
        case 'V':
#if defined(THAI_KEYMAP)
		case 0x0E23:
		case 0x0E24:
		case 0x0E25:
		case 0x0E26:
		case 0x0E27:
		case 0x0E28:
		case 0x0E29:
#endif
            ret = '8';
            break;

        case 'w':
        case 'W':
        case 'X':
        case 'x':
        case 'Y':
        case 'y':
        case 'z':
        case 'Z':
#if defined(THAI_KEYMAP)
		case 0x0E2A:
		case 0x0E2B:
		case 0x0E2C:
		case 0x0E2D:
		case 0x0E2E:
#endif
            ret = '9';
            break;

		case '*':
		case '+':
#if defined(THAI_KEYMAP)
		case 0x0E2F:
		case 0x0E3F:
		case 0x0E31:
		case 0x0E34:
		case 0x0E35:
		case 0x0E36:
		case 0x0E37:
		case 0x0E38:
		case 0x0E39:
		case 0x0E46:
		case 0x0E47:
		case 0x0E48:
		case 0x0E49:
		case 0x0E4A:
		case 0x0E4B:
		case 0x0E4C:
#endif
		    ret = KMappedCharForStar;
		    break;
		    
		case '#':
#if defined(THAI_KEYMAP)
		case 0x0E30:
		case 0x0E32:
		case 0x0E33:
		case 0x0E40:
		case 0x0E41:
		case 0x0E42:
		case 0x0E43:
		case 0x0E44:
#endif
		    ret = KMappedCharForHash;
		    break;

		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case ' ':
            ret = input; // Numbers and space
            break;

		default: // Other (unknown) chars
		    ret = KPadChar;
        }
    return ret;
    }
#endif // #if !defined(USE_ORBIT_KEYMAP)

// ----------------------------------------------------------------------------
// C12keyKeyMap::ComputeValue
// ----------------------------------------------------------------------------
TInt C12keyKeyMap::ComputeValue(QString aString,
							    TBool aUpperLimit,
							    QString& aValue) const
	{
	QString hexString;
	if (aString.length() < KMaxKeysStoredInDb)
		{
		QChar pad = KLowerLimitPadding;
		if (aUpperLimit)
			{
			pad = KUpperLimitPadding;
			}
		hexString = aString.leftJustified(KMaxKeysStoredInDb, pad);
		}
	else
		{
		hexString = aString;
		}
    
    const TInt KHexadecimalBase = 16;
    bool ok(true);
    // Use signed int to prevent underflow when padded is "00...00"
    qint64 value = hexString.toLongLong(&ok, KHexadecimalBase); 
    if (!ok)
        {
		PRINT(_L("CPcsKeyMap::ComputeValue fails"));
		return KErrArgument;
        }

	// In order to write queries using '>' and '<' instead of '>=' and '<=',
	// expand the limit by one.
	if (aUpperLimit)
		{
		++value;
		}
	else
		{
		--value;
		}

    aValue = QString::number(value); // Convert to decimal value
	return KErrNone;
	}

#if defined(USE_ORBIT_KEYMAP)

// ----------------------------------------------------------------------------
// C12keyKeyMap::SetHardcodedCharacters
// In emulator (except in unit tests), select just the default language, as new
// SDKs have over 40 languages, causing out of memory error.
// In hardware and when compiling unit tests in emulator, select all languages
// available in the device. Reason: it is though that HW does not come with
// dozens of languages and some unit test cases require Thai keymap.
// ----------------------------------------------------------------------------
QList<HbInputLanguage> C12keyKeyMap::SelectLanguages()
	{
#if defined(__WINSCW__) && !defined(EUNIT_UNIT_TEST_COVERAGE)
	return CPcsKeyMap::SelectLanguages();
#else
	return HbKeymapFactory::availableLanguages();
#endif
	}

// ----------------------------------------------------------------------------
// C12keyKeyMap::SetHardcodedCharacters
// Even though most languages map *, + and # to 1-key, they are here mapped to
// the distinct *-key or #-key of the 12-key ITU-T keypad.
// ----------------------------------------------------------------------------
void C12keyKeyMap::SetHardcodedCharacters()
	{
	iKeyMapping[EKeyStar].append(KStar);
	iKeyMapping[EKeyStar].append(KPlus);
	iKeyMapping[EKeyHash].append(KHash);
	iHardcodedChars.append(KStar);
	iHardcodedChars.append(KPlus);
	iHardcodedChars.append(KHash);
	}
#endif // #if defined(USE_ORBIT_KEYMAP)

// ----------------------------------------------------------------------------
// C12keyKeyMap::DetermineSpecialCharBehaviour
// If input string is Thai language, skip chars that map to star or hash keys.
// ----------------------------------------------------------------------------
TBool C12keyKeyMap::DetermineSpecialCharBehaviour(QString aSource) const
	{
	/* this code would use the current input language to determine if Thai is in use
	HbInputLanguage lang = HbInputSettingProxy::instance()->globalInputLanguage();
    TBool skipHashStar = lang.language() == QLocale::Thai; */

	if (iLatinCodec && iLatinCodec->canEncode(aSource))
		{
		return EFalse;
		}
    return iThaiCodec && iThaiCodec->canEncode(aSource);
	}

// ----------------------------------------------------------------------------
// C12keyKeyMap::ShouldSkipChar
// Characters mapped to certain keys (*,#) are skipped for certain languages.
// ----------------------------------------------------------------------------
TBool C12keyKeyMap::ShouldSkipChar(QChar aChar, TBool aSkipHashStar) const
	{
	return aSkipHashStar &&
		   (aChar == KMappedCharForStar || aChar == KMappedCharForHash);
	}

// ----------------------------------------------------------------------------
// C12keyKeyMap::C12keyKeyMap
// Fill QList with empty strings
// ----------------------------------------------------------------------------
C12keyKeyMap::C12keyKeyMap() :
	CPcsKeyMap(KAmountOfKeys, KPadChar, KMaxKeysStoredInDb)
	{
	}

// ----------------------------------------------------------------------------
// C12keyKeyMap::ConstructL
// ----------------------------------------------------------------------------
void C12keyKeyMap::ConstructL()
	{
	PRINT(_L("Enter C12keyKeyMap::ConstructL"));

#if defined(USE_ORBIT_KEYMAP)
	CPcsKeyMap::ConstructL(HbKeyboardVirtual12Key);
#else
	CPcsKeyMap::ConstructL();
#endif

	TInt err(KErrNone);
	QT_TRYCATCH_ERROR(err, GetTextCodecs());
    if (err != KErrNone)
        {
        PRINT1(_L("C12keyKeyMap::ConstructL exception, err=%d"), err);
        User::Leave(err);
        }

	PRINT(_L("End C12keyKeyMap::ConstructL"));
	}

// ----------------------------------------------------------------------------
// C12keyKeyMap::GetTextCodecs
// ----------------------------------------------------------------------------
void C12keyKeyMap::GetTextCodecs()
    {
    iLatinCodec = QTextCodec::codecForMib(KLatin1);
    iThaiCodec = QTextCodec::codecForMib(KThaiLanguage);
    }

// End of file
