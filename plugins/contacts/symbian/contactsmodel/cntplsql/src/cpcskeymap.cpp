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
#include "cpcskeymap.h"
#include <QChar>
#include <QString>
#include <hbinputkeymap.h>
#include <hbinputkeymapfactory.h>

// This macro suppresses log writes
//#define NO_PRED_SEARCH_LOGS
#include "predictivesearchlog.h"

const QChar KSpaceChar = ' ';

// Separator character stored in predictive search table columns
const QChar KSeparatorChar = ' ';

// Code using the new API (wk32 onwards) is put here. Remove old API code
// when wk30 is no longer used.
// #define NEW_KEYMAP_FACTORY_API


// ============================== MEMBER FUNCTIONS ============================

// ----------------------------------------------------------------------------
// CPcsKeyMap::~CPcsKeyMap
// ----------------------------------------------------------------------------
CPcsKeyMap::~CPcsKeyMap()
    {
    PRINT(_L("Enter CPcsKeyMap::~CPcsKeyMap"));    
    PRINT(_L("End CPcsKeyMap::~CPcsKeyMap"));
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::GetMappedStringL
// ----------------------------------------------------------------------------
HBufC* CPcsKeyMap::GetMappedStringL(const TDesC& aSource) const
    {
    PRINT1(_L("Enter CPcsKeyMap::GetMappedStringL input '%S'"), &aSource);

	QString source((QChar*)aSource.Ptr(), aSource.Length());
	QString result;
	TInt err(KErrNone);
	QT_TRYCATCH_ERROR(err, result = GetMappedString(source));
	User::LeaveIfError(err);

    HBufC* destination = HBufC::NewL(result.length());
	destination->Des().Copy(result.utf16());

    PRINT1(_L("End CPcsKeyMap::GetMappedStringL result '%S'"), destination);
    return destination;
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::GetMappedString
// ----------------------------------------------------------------------------
QString CPcsKeyMap::GetMappedString(QString aSource) const
	{
#if defined(WRITE_PRED_SEARCH_LOGS)
	const int KLogLength = 30;
	TBuf<KLogLength> log(aSource.left(KLogLength).utf16());
	PRINT1(_L("Enter CPcsKeyMap::GetMappedString input '%S'"), &log);
#endif

	QString destination;
	TBool skipHashStar = DetermineSpecialCharBehaviour(aSource);
	TInt length = aSource.length();

    for (int i = 0; i < length; ++i)
        {
        if (aSource[i] == KSpaceChar)
            {
            destination.append(KSeparatorChar);
            }
        else
			{
			QChar ch(0);
            ch = MappedKeyForChar(aSource[i]);
			if (!ShouldSkipChar(ch, skipHashStar))
				{
				destination.append(ch);
				}
			}
		}
#if defined(WRITE_PRED_SEARCH_LOGS)
	log = destination.left(KLogLength).utf16();
	PRINT1(_L("End CPcsKeyMap::GetMappedString result '%S'"), &log);
#endif
    return destination;
	}

// ----------------------------------------------------------------------------
// CPcsKeyMap::GetNumericLimitsL
// In order to speed up the execution, caller should convert search pattern
// with a one call to CPcsKeyMap::GetMappedStringL() and then pass the tokens
// to CPcsKeyMap::GetNumericLimitsL().
// So it is expected that aString contains only certain characters.
// ----------------------------------------------------------------------------
TInt CPcsKeyMap::GetNumericLimits(QString aString,
								  QString& aLowerLimit,
								  QString& aUpperLimit) const
	{
	PRINT(_L("CPcsKeyMap::GetNumericLimits"));
	if (aString.length() > iMaxKeysStoredInDb)
		{
		QString truncated = aString.left(iMaxKeysStoredInDb);
		aString = truncated;
		}

	TInt err = ComputeValue(aString, EFalse, aLowerLimit);
	if (err == KErrNone)
		{
		err = ComputeValue(aString, ETrue, aUpperLimit);
		}
	PRINT1(_L("End CPcsKeyMap::GetNumericLimits ret=%d"), err);
	return err;
	}

// ----------------------------------------------------------------------------
// CPcsKeyMap::Separator
// ----------------------------------------------------------------------------
QChar CPcsKeyMap::Separator() const
    {
    return KSeparatorChar;
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::SetHardcodedCharacters
// Default implementation selects only the current default language.
// ----------------------------------------------------------------------------
QList<HbInputLanguage> CPcsKeyMap::SelectLanguages()
	{
	QList<HbInputLanguage> languages;
	HbInputLanguage inputLanguage(QLocale::system().language()); 
	languages << inputLanguage;
	return languages;
	}

// ----------------------------------------------------------------------------
// CPcsKeyMap::SetHardcodedCharacters
// Default implementation does nothing
// ----------------------------------------------------------------------------
void CPcsKeyMap::SetHardcodedCharacters()
	{
	}

// ----------------------------------------------------------------------------
// CPcsKeyMap::DetermineSpecialCharBehaviour
// Default implementation
// ----------------------------------------------------------------------------
TBool CPcsKeyMap::DetermineSpecialCharBehaviour(QString /*aSource*/) const
	{
	return EFalse;
	}

// ----------------------------------------------------------------------------
// CPcsKeyMap::ShouldSkipChar
// Default implementation
// ----------------------------------------------------------------------------
TBool CPcsKeyMap::ShouldSkipChar(QChar /*aChar*/, TBool /*aSkipHashStar*/) const
	{
	return EFalse;
	}

// ----------------------------------------------------------------------------
// CPcsKeyMap::ConstructL
// ----------------------------------------------------------------------------
void CPcsKeyMap::ConstructL(HbKeyboardType aKeyboardType)
	{
	PRINT(_L("Enter CPcsKeyMap::ConstructL"));

	TInt err(KErrNone);
	QT_TRYCATCH_ERROR(err,
		{
		InitKeyMappings();
		SetHardcodedCharacters();
		ConstructLanguageMappings(aKeyboardType);
		});
	if (err != KErrNone)
        {
        PRINT1(_L("CPcsKeyMap::ConstructL exception, err=%d"), err);
        User::Leave(err);
        }

	PRINT(_L("End CPcsKeyMap::ConstructL"));
	}

// ----------------------------------------------------------------------------
// CPcsKeyMap::CPcsKeyMap
// ----------------------------------------------------------------------------
CPcsKeyMap::CPcsKeyMap(TInt aAmountOfKeys,
					   QChar aPadChar,
					   TInt aMaxKeysStoredInDb) :
	iKeyMapping(),
	iAmountOfKeys(aAmountOfKeys),
	iPadChar(aPadChar),
	iMaxKeysStoredInDb(aMaxKeysStoredInDb)
	{
	}

// ----------------------------------------------------------------------------
// CPcsKeyMap::InitKeyMappings
// Put string for each key into iKeyMapping.
// ----------------------------------------------------------------------------
void CPcsKeyMap::InitKeyMappings()
	{
    PRINT(_L("Enter CPcsKeyMap::InitKeyMappings"));

	for (TInt i = 0; i < iAmountOfKeys; ++i)
        {
        iKeyMapping << QString("");
        }
	}

// ----------------------------------------------------------------------------
// CPcsKeyMap::ConstructLanguageMappings
// Fetch keymap for selected languages.
// Currently QWERTY keymaps do not map digits ('0'..'9') to any key, even with
// HbModifierChrPressed and HbModifierFnPressed modifiers.
// ----------------------------------------------------------------------------
void CPcsKeyMap::ConstructLanguageMappings(HbKeyboardType aKeyboardType)
	{
    PRINT(_L("Enter CPcsKeyMap::ConstructLanguageMappings"));

#if defined(WRITE_PRED_SEARCH_LOGS)
    TInt count(0);
#endif

	QList<HbInputLanguage> languages = SelectLanguages();
    PRINT1(_L("build keymap from %d language(s)"), languages.count());

	TInt languageCount = languages.size();
#if !defined(NEW_KEYMAP_FACTORY_API)
	// Latest SDKs have so many keymaps contact server runs out of stack.
	// So limit the amount of keymaps.
	const TInt KMaxKeymapCount = 10;
	if (languageCount > KMaxKeymapCount)
	    {
        languageCount = KMaxKeymapCount;
	    }
#endif

	for (TInt lang = 0; lang < languageCount; ++lang)
		{
        PRINT2(_L("(%d) handle language %d"), lang, languages[lang].language());
		if (IsLanguageSupported(languages[lang].language()))
			{
			PRINT2(_L("Constructing keymap for lang=%d,var=%d"),
				   languages[lang].language(),
				   languages[lang].variant());
#if defined(NEW_KEYMAP_FACTORY_API)
			// Gets ownership of keymap
			const HbKeymap* keymap =
			    HbKeymapFactory::instance()->keymap(languages[lang],  
                                                    HbKeymapFactory::NoCaching);
#else
			// Does not get ownership of keymap
			const HbKeymap* keymap =
				HbKeymapFactory::instance()->keymap(languages[lang].language(),
                                                    languages[lang].variant());
#endif
			if (keymap)
			    {
#if defined(WRITE_PRED_SEARCH_LOGS)
                count +=
#endif
                ReadKeymapCharacters(aKeyboardType, *keymap);
				
#if defined(NEW_KEYMAP_FACTORY_API)
				delete keymap;
#endif
			    }
			else
                {
                PRINT(_L("CPcsKeyMap::ContructKeyboardMapping keymap not found"));
                }
			}
		}

#if defined(WRITE_PRED_SEARCH_LOGS)
    PRINT1(_L("End CPcsKeyMap::ConstructLanguageMappings keymap has %d chars"), count);
#endif
	}

// ----------------------------------------------------------------------------
// CPcsKeyMap::IsLanguageSupported
// ----------------------------------------------------------------------------
TBool CPcsKeyMap::IsLanguageSupported(QLocale::Language aLanguage) const
	{
	return (aLanguage != QLocale::Japanese && aLanguage != QLocale::Chinese);
	}

// ----------------------------------------------------------------------------
// CPcsKeyMap::MappedKeyForChar
// Loop all QStrings of iKeyMapping to find one containing the character.
// If the character is not mapped, use pad character.
// ----------------------------------------------------------------------------
const QChar CPcsKeyMap::MappedKeyForChar(const QChar aChar) const
	{
    for (TInt index = 0; index < iAmountOfKeys; ++index) 
        {
        if (iKeyMapping[index].contains(aChar))
            {
			return ArrayIndexToMappedChar(index);
            }
        }

#if _DEBUG
	TUint ch = aChar.unicode();
	PRINT2(_L("CPcsKeyMap::MappedKeyForChar no mapping for char '%c' (0x%x)"),
		   ch, ch);
#endif
	return iPadChar;
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::ReadKeymapCharacters
// ----------------------------------------------------------------------------
TInt CPcsKeyMap::ReadKeymapCharacters(HbKeyboardType aKeyboardType,
                                      const HbKeymap& aKeymap)
    {
    PRINT(_L("Enter CPcsKeyMap::ReadKeymapCharacters"));

    TInt count(0);

    for (TInt key = 0; key < iAmountOfKeys; ++key)
        {
        PRINT1(_L("handle key(enum value %d)"), key); // test
        const HbMappedKey* mappedKey = aKeymap.keyForIndex(aKeyboardType, key);
        // 12-key: Most languages don't have mapping for EKeyStar, EKeyHash.
        // QWERTY: Different languages have different amount of keys,
        // so mappedKey can be NULL.
        if (mappedKey)
            {
            const QString lowerCase = mappedKey->characters(HbModifierNone); // "abc2.."
            const QString upperCase = mappedKey->characters(HbModifierShiftPressed); // "ABC2.."                        
            const QString charsForKey = lowerCase + upperCase; 

            // Filter out duplicate characters
            for (TInt i = charsForKey.length() - 1; i >= 0 ; --i) 
                {
                QChar ch = charsForKey[i];
                if (!iKeyMapping[key].contains(ch) &&
                    !iHardcodedChars.contains(ch))
                    {
#if defined(WRITE_PRED_SEARCH_LOGS)
                    char ascChar = ch.toAscii();
                    TChar logChar(ArrayIndexToMappedChar(key).unicode());

                    if (ascChar == 0) // ch can't be represented in ASCII
                        {
                        PRINT2(_L("CPcsKeyMap: map key(%c) <-> char=0x%x"),
                               logChar, ch);
                        }
                    else
                        {
                        PRINT3(_L("CPcsKeyMap: map key(%c) <-> char='%c'(0x%x)"),
                               logChar,
                               ascChar,
                               ascChar);
                        }
                    ++count;
#endif // #if defined(WRITE_PRED_SEARCH_LOGS)
                    iKeyMapping[key] += ch;
                    }
                }
            }
        }
    
    PRINT(_L("End CPcsKeyMap::ReadKeymapCharacters"));
    return count;
    }

// End of file
