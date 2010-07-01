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
#include "cqwertykeymap.h"

// This macro suppresses log writes
//#define NO_PRED_SEARCH_LOGS
#include "predictivesearchlog.h"

#include <QChar>
#include <QString>


// Largest amount of keypresses that can be stored in QWERTY keyboard's
// predictive search tables.
// SQL BIGINT is a 64-bit signed integer and one bit is reserved for sign.
// QWERTY's keys are identified by TKeyId that needs 6 bits.
// 63 / 6 = 10
const TInt KMaxKeysStoredInDb = 10;

// How many bits are needed to represent TKeyId
const TInt KBitsInKeyId = 6;


const QChar KEY_Q_NAME = 'q';
const QChar KEY_W_NAME = 'w';
const QChar KEY_E_NAME = 'e';
const QChar KEY_R_NAME = 'r';
const QChar KEY_T_NAME = 't';
const QChar KEY_Y_NAME = 'y';
const QChar KEY_U_NAME = 'u';
const QChar KEY_I_NAME = 'i';
const QChar KEY_O_NAME = 'o';
const QChar KEY_P_NAME = 'p';

const QChar KEY_A_NAME = 'a';
const QChar KEY_S_NAME = 's';
const QChar KEY_D_NAME = 'd';
const QChar KEY_F_NAME = 'f';
const QChar KEY_G_NAME = 'g';
const QChar KEY_H_NAME = 'h';
const QChar KEY_J_NAME = 'j';
const QChar KEY_K_NAME = 'k';
const QChar KEY_L_NAME = 'l';

const QChar KEY_Z_NAME = 'z';
const QChar KEY_X_NAME = 'x';
const QChar KEY_C_NAME = 'c';
const QChar KEY_V_NAME = 'v';
const QChar KEY_B_NAME = 'b';
const QChar KEY_N_NAME = 'n';
const QChar KEY_M_NAME = 'm';

const QChar KEY_COLON_NAME = ',';
const QChar KEY_DOT_NAME   = '.';
const QChar KEY_DASH_NAME  = '-';
const QChar KEY_AT_NAME	   = '@';
const QChar KEY_QUOTE_NAME = '\'';
const QChar KEY_QUESTION_MARK_NAME = '?';

const QChar KEY_32_NAME = '1';
const QChar KEY_33_NAME = '2';
const QChar KEY_34_NAME = '3';
const QChar KEY_35_NAME = '4';
const QChar KEY_36_NAME = '5';
const QChar KEY_37_NAME = '6';
const QChar KEY_38_NAME = '7';
const QChar KEY_39_NAME = '8';
const QChar KEY_40_NAME = '9';
const QChar KEY_41_NAME = '0';
const QChar KEY_42_NAME = '+';
const QChar KEY_43_NAME = '#';

// Unmapped (unknown) characters are replaced with this
const QChar PAD_CHAR = '!';

// Must be the first key (EKeyQ) that has internal value 0
const QChar KLowerLimitPadding = KEY_Q_NAME;
const QChar KUpperLimitPadding = PAD_CHAR;


// ============================== MEMBER FUNCTIONS ============================

// ----------------------------------------------------------------------------
// CQwertyKeyMap::NewL
// ----------------------------------------------------------------------------
CQwertyKeyMap* CQwertyKeyMap::NewL()
	{
    PRINT(_L("Enter CQwertyKeyMap::NewL"));
    
    CQwertyKeyMap* self = new (ELeave) CQwertyKeyMap();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    PRINT(_L("End CQwertyKeyMap::NewL"));
    return self;
	}

// ----------------------------------------------------------------------------
// CQwertyKeyMap::~CQwertyKeyMap
// ----------------------------------------------------------------------------
CQwertyKeyMap::~CQwertyKeyMap()
    {
    PRINT(_L("Enter CQwertyKeyMap::~CQwertyKeyMap"));    
    PRINT(_L("End CQwertyKeyMap::~CQwertyKeyMap"));
    }

// ----------------------------------------------------------------------------
// CQwertyKeyMap::ArrayIndexToMappedChar
// Map index of iKeyMapping list, to the key that the mapping is for.
// Space is already handled in CPcsKeyMap::GetMappedString(), no need to handle
// it here.
// ----------------------------------------------------------------------------
const QChar CQwertyKeyMap::ArrayIndexToMappedChar(TInt aArrayIndex) const
	{
	__ASSERT_DEBUG(aArrayIndex < EAmountOfKeysInQwertyKeypad,
				   User::Panic(_L("CQwertyKeyMap::ArrayIndexToMappedChar"),
				   KErrOverflow));
	return iKeyNames.value(static_cast<TKeyId>(aArrayIndex), PAD_CHAR);
	}

#if !defined(USE_ORBIT_KEYMAP)
// ----------------------------------------------------------------------------
// CQwertyKeyMap::UseHardcodedKeyMap
// ----------------------------------------------------------------------------
const QChar CQwertyKeyMap::UseHardcodedKeyMap(const QChar input) const
    {
    if (input >= 'a' && input <= 'z')
        {
        return input;
        }
    if (input >= 'A' && input <= 'Z')
        {
        return input.toLower(); // Return lowercase letter
        }
    if (input == KEY_COLON_NAME ||
        input == KEY_DOT_NAME ||
        input == KEY_DASH_NAME ||
        input == KEY_AT_NAME ||
        input == KEY_QUOTE_NAME ||
        input == KEY_QUESTION_MARK_NAME)
        {
        return input;
        }

        // TODO: add rest of the keys

#if defined(THAI_KEYMAP)
    // TODO: add Thai key map
#endif

    // Other (unknown) chars
    return PAD_CHAR;
    }
#endif // #if !defined(USE_ORBIT_KEYMAP)

// ----------------------------------------------------------------------------
// CQwertyKeyMap::ComputeValue
// ----------------------------------------------------------------------------
TInt CQwertyKeyMap::ComputeValue(QString aString,
								 TBool aUpperLimit,
								 QString& aValue) const
	{
	QString padded;
	if (aString.length() < KMaxKeysStoredInDb)
		{
		padded = aString.leftJustified(KMaxKeysStoredInDb,
			aUpperLimit ? KUpperLimitPadding : KLowerLimitPadding);
		}
	else
		{
		padded = aString;
		}	

#if defined(WRITE_PRED_SEARCH_LOGS)
	TBuf<KMaxKeysStoredInDb> log(padded.utf16());
	PRINT1(_L("CQwertyKeyMap::ComputeValue string = '%S'"), &log);
#endif

	qint64 value(0);
	for (TInt i = 0; i < KMaxKeysStoredInDb; ++i)
		{
        value <<= KBitsInKeyId;
		value += MapKeyNameToValue(padded[i]);
		PRINT1(_L("  value now 0x%lx"), value);
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
	PRINT2(_L("CQwertyKeyMap::ComputeValue result=0x%lx (%ld decimal)"), value, value);
	aValue = QString::number(value); // Convert to decimal value
	return KErrNone;
	}

#if defined(USE_ORBIT_KEYMAP)
// ----------------------------------------------------------------------------
// CQwertyKeyMap::MapKeyNameToValue
// ----------------------------------------------------------------------------
bool CQwertyKeyMap::IsValidChar(const QChar aChar) const
	{
	return iKeyValues.contains(aChar);
	}
#endif // #if defined(USE_ORBIT_KEYMAP)

// ----------------------------------------------------------------------------
// CQwertyKeyMap::MapKeyNameToValue
// Does the reverse of ArrayIndexToMappedChar()
// ----------------------------------------------------------------------------
TInt CQwertyKeyMap::MapKeyNameToValue(const QChar aKeyName) const
    {
    return iKeyNames.key(aKeyName, KPadCharValue);
    }

// ----------------------------------------------------------------------------
// CQwertyKeyMap::CQwertyKeyMap
// Fill QList with empty strings
// ----------------------------------------------------------------------------
CQwertyKeyMap::CQwertyKeyMap() :
	CPcsKeyMap(EAmountOfKeysInQwertyKeypad, PAD_CHAR, KMaxKeysStoredInDb)
	{
	}

// ----------------------------------------------------------------------------
// CQwertyKeyMap::ConstructL
// ----------------------------------------------------------------------------
void CQwertyKeyMap::ConstructL()
	{
	PRINT(_L("Enter CQwertyKeyMap::ConstructL"));

	// CPcsKeyMap::ConstructLanguageMappings() uses ArrayIndexToMappedChar()
	// that uses iKeyNames, so construct it first
	TInt err(KErrNone);
    QT_TRYCATCH_ERROR(err, ConstructKeyNameMap());
    User::LeaveIfError(err);

#if defined(USE_ORBIT_KEYMAP)
	CPcsKeyMap::ConstructL(HbKeyboardVirtualQwerty);
#else
	CPcsKeyMap::ConstructL();
#endif

	PRINT(_L("End CQwertyKeyMap::ConstructL"));
	}

// ----------------------------------------------------------------------------
// CQwertyKeyMap::ConstructKeyNameMap
// ----------------------------------------------------------------------------
void CQwertyKeyMap::ConstructKeyNameMap()
	{
	iKeyNames.insert(EKeyQ, KEY_Q_NAME);
	iKeyNames.insert(EKeyW, KEY_W_NAME);
	iKeyNames.insert(EKeyE, KEY_E_NAME);
	iKeyNames.insert(EKeyR, KEY_R_NAME);
	iKeyNames.insert(EKeyT, KEY_T_NAME);
	iKeyNames.insert(EKeyY, KEY_Y_NAME);
	iKeyNames.insert(EKeyU, KEY_U_NAME);
	iKeyNames.insert(EKeyI, KEY_I_NAME);
	iKeyNames.insert(EKeyO, KEY_O_NAME);
	iKeyNames.insert(EKeyP, KEY_P_NAME);
	iKeyNames.insert(EKeyA, KEY_A_NAME);
	iKeyNames.insert(EKeyS, KEY_S_NAME);
	iKeyNames.insert(EKeyD, KEY_D_NAME);
	iKeyNames.insert(EKeyF, KEY_F_NAME);
	iKeyNames.insert(EKeyG, KEY_G_NAME);
	iKeyNames.insert(EKeyH, KEY_H_NAME);
	iKeyNames.insert(EKeyJ, KEY_J_NAME);
	iKeyNames.insert(EKeyK, KEY_K_NAME);
	iKeyNames.insert(EKeyL, KEY_L_NAME);
	iKeyNames.insert(EKeyZ, KEY_Z_NAME);
	iKeyNames.insert(EKeyX, KEY_X_NAME);
	iKeyNames.insert(EKeyC, KEY_C_NAME);
	iKeyNames.insert(EKeyV, KEY_V_NAME);
	iKeyNames.insert(EKeyB, KEY_B_NAME);
	iKeyNames.insert(EKeyN, KEY_N_NAME);
	iKeyNames.insert(EKeyM, KEY_M_NAME);
	iKeyNames.insert(EKeyColon, KEY_COLON_NAME);
	iKeyNames.insert(EKeyDot,   KEY_DOT_NAME);
	iKeyNames.insert(EKeyDash,  KEY_DASH_NAME);
	iKeyNames.insert(EKeyAt,	KEY_AT_NAME);
	iKeyNames.insert(EKeyQuote, KEY_QUOTE_NAME);
	iKeyNames.insert(EKeyQuestionMark, KEY_QUESTION_MARK_NAME);
	iKeyNames.insert(EKey32, KEY_32_NAME);
	iKeyNames.insert(EKey33, KEY_33_NAME);
	iKeyNames.insert(EKey34, KEY_34_NAME);
	iKeyNames.insert(EKey35, KEY_35_NAME);
	iKeyNames.insert(EKey36, KEY_36_NAME);
	iKeyNames.insert(EKey37, KEY_37_NAME);
	iKeyNames.insert(EKey38, KEY_38_NAME);
	iKeyNames.insert(EKey39, KEY_39_NAME);
	iKeyNames.insert(EKey40, KEY_40_NAME);
	iKeyNames.insert(EKey41, KEY_41_NAME);
	iKeyNames.insert(EKey42, KEY_42_NAME);
	iKeyNames.insert(EKey43, KEY_43_NAME);


	// Since reverse lookup in QMap is slow, collect all values into iKeyValues
	// list, that can be searched.
	iKeyValues = iKeyNames.values();
	}

// End of file
