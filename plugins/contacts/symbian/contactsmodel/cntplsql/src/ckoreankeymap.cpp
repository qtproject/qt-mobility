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
#include "ckoreankeymap.h"
#include <QString>
#include <QTextCodec>

#include "predictivesearchkeymapdefs.h"
// This macro suppresses log writes
// #define NO_PRED_SEARCH_LOGS
#include "predictivesearchlog.h"
#include "koreaninput.h"


const int KMibKorean = 36;

// These constants are from Hangul and Jamo unicode specifications
// UD7A4..UD7AF are unspecific
const int KHangulSyllableLowerLimit = 0xAC00;
const int KHangulSyllableUpperLimit = 0xD7A3;

const int KJamoLowerLimit = 0x1100;
const int KJamoUpperLimit = 0x11FF;

const int KJungCount = 21;
const int KJongCount = 28;

const int KChoBase  = KJamoLowerLimit;
const int KJungBase = 0x1161;
const int KJongBase = 0x11A7;



// ============================== MEMBER FUNCTIONS ============================

// ----------------------------------------------------------------------------
// CKoreanKeyMap::NewL
// ----------------------------------------------------------------------------
CKoreanKeyMap* CKoreanKeyMap::NewL()
    {
	CKoreanKeyMap* self = new (ELeave) CKoreanKeyMap;
	CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// CKoreanKeyMap::~CKoreanKeyMap
// ----------------------------------------------------------------------------
CKoreanKeyMap::~CKoreanKeyMap()
    {
	delete iTokenizer;
    }

// ----------------------------------------------------------------------------
// CKoreanKeyMap::IsLanguageSupported
// Korean QTextCodec recognizes Hangul and Hangul compatibility Jamo characters,
// but not the Hangul Jamo characters (U1100+). Hence the Jamo are handled here
// and treated as Korean text.
// ----------------------------------------------------------------------------
TBool CKoreanKeyMap::IsLanguageSupported(QString aSource) const
	{
	if (iLatinCodec && iLatinCodec->canEncode(aSource))
		{
		return EFalse;
		}

	int len = aSource.length();
	for (int i = 0; i < len; ++i)
		{
		int unicode = aSource[i].unicode();
		if (unicode >= KJamoLowerLimit && unicode <= KJamoUpperLimit)
			{
			// Replace the character in aSource local to this function, with (any)
			// Hangul syllable that QTextCodec recognizes are Korean text.
			// Does not modify the caller's version of aSource.
			aSource[i] = KHangulSyllableLowerLimit;
			}
		}

    return iKoreanCodec && iKoreanCodec->canEncode(aSource);
	}

// ----------------------------------------------------------------------------
// CKoreanKeyMap::GetMappedString
// Hangul syllable characters are split into Jamos.
// Jamos are mapped to sequence of keypresses.
// If aSource has spaces, they are treated as separator characters for the
// later tokenizing.
// ----------------------------------------------------------------------------
QString CKoreanKeyMap::GetMappedString(QString aSource) const
	{
	QString destination;
	TInt length = aSource.length();

    for (int i = 0; i < length; ++i)
        {
		QChar ch = aSource[i];
		if (IsSyllable(ch))
			{
			QString jamos = ExtractJamos(ch);
			destination.append(GetMappedString(jamos));
			destination.append(KSeparatorChar); // Syllable is one token
			}
		else
			{
			if (ch == KSpaceChar)
				{
				destination.append(KSeparatorChar);
				}
			else
				{
				QString keySequence = MapJamoToKeySequence(ch); // searches iKeyPressMap
				destination.append(keySequence);
				}

			// TODO: detect the syllable limits within stream of Jamos in this way:
			// syllable begins by C + V pair, which is never in the middle of a syllable.
			// Currently assumes aSource only contains grammatically valid Jamo stream

			// To tokenize Jamos, map them to keysequences, and split them with iTokenizer->Tokenize()
			}
		}
#if defined(WRITE_PRED_SEARCH_LOGS)
	const int KLogLength = 60;
	TBuf<KLogLength> log = destination.left(KLogLength).utf16();
	PRINT1(_L("End CPcsKeyMap::GetMappedString result '%S'"), &log);
#endif
    return destination;
	}

// ----------------------------------------------------------------------------
// CKoreanKeyMap::CKoreanKeyMap
// ----------------------------------------------------------------------------
CKoreanKeyMap::CKoreanKeyMap()
	{
	}

// ----------------------------------------------------------------------------
// CKoreanKeyMap::ConstructL
// ----------------------------------------------------------------------------
void CKoreanKeyMap::ConstructL()
	{
	PRINT(_L("Enter CKoreanKeyMap::ConstructL"));

	TInt err(KErrNone);
	QT_TRYCATCH_ERROR(err,
		{
		GetTextCodecs();
		FillKeyPressMap();
		});		
		
    if (err != KErrNone)
        {
        PRINT1(_L("CKoreanKeyMap::ConstructL exception, err=%d"), err);
        User::Leave(err);
        }

	iTokenizer = new (ELeave) KoreanInput;

	PRINT(_L("End CKoreanKeyMap::ConstructL"));
	}

// ----------------------------------------------------------------------------
// CKoreanKeyMap::GetTextCodecs
// ----------------------------------------------------------------------------
void CKoreanKeyMap::GetTextCodecs()
    {
    iLatinCodec = QTextCodec::codecForMib(KMibLatin1);
    iKoreanCodec = QTextCodec::codecForMib(KMibKorean);
    }

// ----------------------------------------------------------------------------
// CKoreanKeyMap::FillKeyPressMap
// Map Korean unicode character values <--> sequence of keypresses
// Key presses are identified like this:
//   key 1 -> "1"
//   key 2 -> "2" etc.
// Any keypresses of * and # are skipped, and also the 9 after 3, 3 and 39
// after 6 and 9 after 0 are skipped. This means that every character is mapped
// to one of the following sequences:
// 1, 2, 4, 5, 7, 8, 3, 33, 6, 66, 9 or 0.
//
// Note: multiple Korean chars can map to same key sequence
// e.g. U1100 and U3131 are the same char
// ----------------------------------------------------------------------------
void CKoreanKeyMap::FillKeyPressMap()
    {
	// When there are multiple keys(unicode chars) mapped to same value (keypresses)
	// then QMap::key(const T & value) returns the _first_ key with value.
	// but is there a need for this reverse search at all?

	MapCharacters("1", 0x1100, 0x3131, 0x11a8);
	MapCharacters("1", 0x110f, 0x314b, 0x11bf);
	MapCharacters("1", 0x1101, 0x3132, 0x11a9);

	MapCharacters("2", 0x1102, 0x3134, 0x11ab);
	MapCharacters("2", 0x1103, 0x3137, 0x11ae);
	MapCharacters("2", 0x1110, 0x314c, 0x11c0);
	MapCharacters("2", 0x1104, 0x3138);

	MapCharacters("3", 0x1161, 0x314f);
	MapCharacters("3", 0x1162, 0x3150);
	MapCharacters("3", 0x1163, 0x3151);
	MapCharacters("3", 0x1164, 0x3152);

	// Double-3 tap is stored as 33
	MapCharacters("33", 0x1165, 0x3153);
	MapCharacters("33", 0x1166, 0x3154);
	MapCharacters("33", 0x1167, 0x3155);
	MapCharacters("33", 0x1168, 0x3156);

	MapCharacters("4", 0x1105, 0x3139, 0x11af);

	MapCharacters("5", 0x1106, 0x3141, 0x11b7);
	MapCharacters("5", 0x1107, 0x3142, 0x11b8);
	MapCharacters("5", 0x1111, 0x314d, 0x11c1);
	MapCharacters("5", 0x1108, 0x3143);

	MapCharacters("6", 0x1169, 0x3157);
	MapCharacters("6", 0x116a, 0x3158);
	MapCharacters("6", 0x116b, 0x3159);
	MapCharacters("6", 0x116c, 0x315a);
	MapCharacters("6", 0x116d, 0x315b);

	// Double-6 tap is stored as 66
	MapCharacters("66", 0x116e, 0x315c);
	MapCharacters("66", 0x116f, 0x315d);
	MapCharacters("66", 0x1170, 0x315e);
	MapCharacters("66", 0x1171, 0x315f);
	MapCharacters("66", 0x1172, 0x3160);

	MapCharacters("7", 0x1109, 0x3145, 0x11ba);
	MapCharacters("7", 0x110c, 0x3148, 0x11bd);
	MapCharacters("7", 0x110e, 0x314a, 0x11be);
	MapCharacters("7", 0x110d, 0x3149);
	MapCharacters("7", 0x110a, 0x3146, 0x11bb);

	MapCharacters("8", 0x110b, 0x3147, 0x11bc);
	MapCharacters("8", 0x1112, 0x314e, 0x11c2);

	MapCharacters("9", 0x1175, 0x3163);

	MapCharacters("0", 0x1173, 0x3161);
	MapCharacters("0", 0x1174, 0x3162);


	// The 11 jong characters that consist of two consonants.
	// Map them to key sequence that produces the two consonants (TODO: chk if this is the correct way)
	MapCharacters("17", 0x11aa);
	MapCharacters("27a", 0x11ac);
	MapCharacters("28a", 0x11ad);
	MapCharacters("41", 0x11b0);
	MapCharacters("45", 0x11b1);
	MapCharacters("45a", 0x11b2);
	MapCharacters("47", 0x11b3);
	MapCharacters("42aa", 0x11b4);
	MapCharacters("45aa", 0x11b5);
	MapCharacters("48a", 0x11b6);
	MapCharacters("5a7", 0x11b9);
	}

// ----------------------------------------------------------------------------
// CKoreanKeyMap::MapCharacters
// ----------------------------------------------------------------------------
void CKoreanKeyMap::MapCharacters(QString aKeySequence,
								  QChar aJamoChar,
								  QChar aCompatibilityJamoChar,
								  QChar aOtherJamoChar)
	{
	iKeyPressMap.insert(aJamoChar, aKeySequence);
	if (aOtherJamoChar.unicode() > 0)
		{
		iKeyPressMap.insert(aOtherJamoChar, aKeySequence);
		}
	if (aCompatibilityJamoChar.unicode() > 0)
		{
		iKeyPressMap.insert(aCompatibilityJamoChar, aKeySequence);
		}
	}

// ----------------------------------------------------------------------------
// CKoreanKeyMap::IsSyllable
// ----------------------------------------------------------------------------
TBool CKoreanKeyMap::IsSyllable(QChar aChar) const
	{
	int codePoint = aChar.unicode();
	return (codePoint >= KHangulSyllableLowerLimit &&
			codePoint <= KHangulSyllableUpperLimit);
	}

// ----------------------------------------------------------------------------
// CKoreanKeyMap::ExtractJamos
// ----------------------------------------------------------------------------
const QString CKoreanKeyMap::ExtractJamos(QChar aChar) const
	{
	PRINT1(_L("Enter CKoreanKeyMap::ExtractJamos aChar=%X"), aChar.unicode());
	__ASSERT_DEBUG(IsSyllable(aChar),
				   User::Panic(_L("CKoreanKeyMap::ExtractJamos"), KErrArgument));

	int syllable  = aChar.unicode() - KHangulSyllableLowerLimit;
	int jongValue = syllable % KJongCount;
	int tmp       = (syllable - jongValue) / KJongCount;
	int jungValue = tmp % KJungCount;
	int choValue  = tmp / KJungCount;
	
	PRINT3(_L("cho=%d jung=%d jong=%d"), choValue, jungValue, jongValue);

	QChar jong = KJongBase + jongValue;
	QChar jung = KJungBase + jungValue;
	QChar cho  = KChoBase + choValue;

	QString result = cho;
	result += jung;
	// If jong is zero, it is missing. Such hangul has just cho and jung.
	if (jongValue > 0)
		{
		result += jong;
		}

	PRINT(_L("End CKoreanKeyMap::ExtractJamos"));
	return result;
	}

// ----------------------------------------------------------------------------
// CKoreanKeyMap::MapJamoToKeySequence
// ----------------------------------------------------------------------------
const QString CKoreanKeyMap::MapJamoToKeySequence(QChar aChar) const
	{
	return iKeyPressMap.value(aChar, "" /* default value */);
	}

// End of file
