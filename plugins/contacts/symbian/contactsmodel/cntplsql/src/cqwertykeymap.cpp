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
// #define NO_PRED_SEARCH_LOGS
#include "predictivesearchlog.h"

#include <QChar>
#include <QString>
#include <QScopedPointer>
#include <hbinputkeymap.h>
#include <hbinputkeymapfactory.h>


// Largest amount of keypresses that can be stored in QWERTY keyboard's
// predictive search tables.
// SQL BIGINT is a 64-bit signed integer and one bit is reserved for sign.
// QWERTY's keys are identified by an ID that needs 6 bits.
// 63 / 6 = 10
const TInt KMaxKeysStoredInDb = 10;

// How many bits are needed to represent key's ID
const TInt KBitsInKeyId = 6;


// The internal names of the keys. String must contain a character for each key
// and one character (last character) for the unmapped keys.
const QString KKeyNames =
	"qwertyuiopasdfghjklzxcvbnm,.-@&!#1234567890QWERTYUIOPASDFGHJKLZ?";
const QChar KLowerLimitPadding = KKeyNames[0];
const QChar KUpperLimitPadding = KKeyNames[CQwertyKeyMap::KPadCharValue];


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
	__ASSERT_DEBUG(aArrayIndex < iAmountOfKeys,
				   User::Panic(_L("CQwertyKeyMap::ArrayIndexToMappedChar"),
				   KErrOverflow));
	return iKeyNames.value(aArrayIndex, KUpperLimitPadding);
	}

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

// ----------------------------------------------------------------------------
// CQwertyKeyMap::ReadExtraCharacters
// Read the SCT landscape keymap. Map decimal numbers to individual keys and
// the special characters that have not yet been read from virtual QWERTY keymap
// to a single key.
// ----------------------------------------------------------------------------
TInt CQwertyKeyMap::ReadExtraCharacters(const HbInputLanguage& aLanguage)
	{
	PRINT(_L("CQwertyKeyMap::ReadExtraCharacters"));

	TInt count(0); // How many new keys have been mapped
	int key = FindNextFreeKey();

#if defined(NEW_KEYMAP_FACTORY_API)
	// Takes ownership
	QScopedPointer<const HbKeymap> keymap(
		HbKeymapFactory::instance()->keymap(aLanguage, HbKeymapFactory::NoCaching));
#else
	const HbKeymap* keymap =
		HbKeymapFactory::instance()->keymap(aLanguage, HbKeymapFactory::Default);
#endif

	if (keymap)
		{
		TInt i(0);
		int keyForSpecialChars(key++);
		const HbMappedKey* mappedKey = keymap->keyForIndex(HbKeyboardSctLandscape, i);
		while (mappedKey)
			{
			const QString mappedCharacters = mappedKey->characters(HbModifierNone);
			if (mappedCharacters.length() > 0)
				{
				// Current HbKeyboardSctLandscape keymap has just one char per key
				QString mappedChar = mappedCharacters[0];
				if (mappedChar[0].isDigit())
					{
					if (AddNewKeyToMap(key, mappedChar, count))
						{
						++key; // Digit was added, reserve next key
						}
					}
				else
					{
					AddNewKeyToMap(keyForSpecialChars, mappedChar, count);
					}
				}
			mappedKey = keymap->keyForIndex(HbKeyboardSctLandscape, ++i);
			}
		}
	else
		{
		PRINT1(_L("no keymap for language %d"), aLanguage.language());
		}
	SetActualNumberOfMappedKeys(key);

    PRINT1(_L("End CQwertyKeyMap::ReadExtraCharacters added %d chars"), count);
	return count;
	}

// ----------------------------------------------------------------------------
// CQwertyKeyMap::IsValidChar
// ----------------------------------------------------------------------------
bool CQwertyKeyMap::IsValidChar(const QChar aChar) const
	{
	return iKeyValues.contains(aChar);
	}

// ----------------------------------------------------------------------------
// CQwertyKeyMap::MapKeyNameToValue
// Does the reverse of ArrayIndexToMappedChar()
// ----------------------------------------------------------------------------
TInt CQwertyKeyMap::MapKeyNameToValue(const QChar aKeyName) const
    {
    return iKeyNames.key(aKeyName, KPadCharValue);
    }

// ----------------------------------------------------------------------------
// CQwertyKeyMap::MappedKeyCount
// ----------------------------------------------------------------------------
TInt CQwertyKeyMap::MappedKeyCount() const
	{
	return iAmountOfKeys;
	}

// ----------------------------------------------------------------------------
// CQwertyKeyMap::CQwertyKeyMap
// Fill QList with empty strings
// ----------------------------------------------------------------------------
CQwertyKeyMap::CQwertyKeyMap() :
	CPcsKeyMap(EMaxAmountOfKeysInQwertyKeypad,
			   KUpperLimitPadding,
			   KMaxKeysStoredInDb)
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

	CPcsKeyMap::ConstructL(HbKeyboardVirtualQwerty);

	PRINT(_L("End CQwertyKeyMap::ConstructL"));
	}

// ----------------------------------------------------------------------------
// CQwertyKeyMap::ConstructKeyNameMap
// ----------------------------------------------------------------------------
void CQwertyKeyMap::ConstructKeyNameMap()
	{
	// The last char in KKeyNames (KUpperLimitPadding) is not inserted
	int len = KKeyNames.length();
	for (int keyId = 0; keyId < len; ++keyId)
		{
		iKeyNames.insert(keyId, KKeyNames[keyId]);
		}

	// Reverse lookup in QMap is slow, so collect all values to iKeyValues,
	// that can be searched.
	iKeyValues = iKeyNames.values();
	}

// ----------------------------------------------------------------------------
// CQwertyKeyMap::FindNextFreeKey
// ----------------------------------------------------------------------------
int CQwertyKeyMap::FindNextFreeKey() const
	{
	int key(iAmountOfKeys - 1);

	while (key >= 0 && iKeyMapping[key].isEmpty())
		{
		--key;
		}
	++key;
	PRINT1(_L("CQwertyKeyMap::FindNextFreeKey return %d"), key);
	return key;
	}

// ----------------------------------------------------------------------------
// CQwertyKeyMap::SetActualNumberOfMappedKeys
// Set the real amount of keys to iAmountOfKeys and remove extra entries to
// speed up searching.
// ----------------------------------------------------------------------------
void CQwertyKeyMap::SetActualNumberOfMappedKeys(int aAmountOfKeys)
	{
	PRINT1(_L("CQwertyKeyMap::SetActualNumberOfMappedKeys %d keys"), aAmountOfKeys);
	iAmountOfKeys = aAmountOfKeys; 

	// TODO: it is not necessary to remove excess entries also from iKeyMapping
	// but that could be done here too.

	for (int i = EMaxAmountOfKeysInQwertyKeypad; i >= aAmountOfKeys; --i)
		{
		iKeyNames.remove(i);
		}
	iKeyValues = iKeyNames.values();
	}
// End of file
