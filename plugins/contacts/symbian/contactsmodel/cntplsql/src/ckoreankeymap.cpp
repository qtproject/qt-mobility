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
#include <QChar>
#include <QString>
#include <QTextCodec>

#include "predictivesearchkeymapdefs.h"
// This macro suppresses log writes
// #define NO_PRED_SEARCH_LOGS
#include "predictivesearchlog.h"


const int KMibKorean = 36;

// These constants are from Hangul and Jamo unicode specifications
const int KHangulSyllableLowerLimit = 0xAC00;
const int KHangulSyllableUpperLimit = 0xD7AF;

const int KChoCount  = 19;
const int KJungCount = 21;
const int KJongCount = 28;

const int KChoBase  = 0x1100;
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
    }

// ----------------------------------------------------------------------------
// CKoreanKeyMap::IsKoreanString
// TODO: how to handle strings with both latin and Korean chars?
// ----------------------------------------------------------------------------
TBool CKoreanKeyMap::IsKoreanString(QString aSource) const
	{
	if (iLatinCodec && iLatinCodec->canEncode(aSource))
		{
		return EFalse;
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
	// TODO: in a later sprint: put a KSeparatorChar between syllables
	
	QString destination;
	TInt length = aSource.length();

    for (int i = 0; i < length; ++i)
        {
		QChar ch = aSource[i];
		if (IsSyllable(ch))
			{
			QString jamos = ExtractJamos(ch);
			destination.append(GetMappedString(jamos));
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
//   key * -> "a"
//   key # -> "b"
//
// Note: multiple Korean chars can map to same key sequence
// e.g. U1100 and U3131 are the same char
// ----------------------------------------------------------------------------
void CKoreanKeyMap::FillKeyPressMap()
    {
	// When there are multiple keys(unicode chars) mapped to same value (keypresses)
	// then QMap::key(const T & value) returns the _first_ key with value.
	// but is there a need for this reverse search at all?

// TODO: the aJamoChar's of jong (U11A8..U11C2) are not yet mapped
// those 19 that are same as cho chars can be mapped similarly, but what about the
// other 8 that look like they are formed from two cho characters?
// should they be mapped as two individual chos?

	MapCharacters(0x1100, 0x3131, "1");
	MapCharacters(0x110f, 0x314b, "1a");
	MapCharacters(0x1101, 0x3132, "1b");

	MapCharacters(0x1102, 0x3134, "2");
	MapCharacters(0x1103, 0x3137, "2a");
	MapCharacters(0x1110, 0x314c, "2aa");
	MapCharacters(0x1104, 0x3138, "2ab");

	MapCharacters(0x1161, 0x314f, "3");
	MapCharacters(0x1162, 0x3150, "39");
	MapCharacters(0x1163, 0x3151, "3a");
	MapCharacters(0x1164, 0x3152, "3a9");

	// Double-3 tap is stored as 33
	MapCharacters(0x1165, 0x3153, "33");
	MapCharacters(0x1166, 0x3154, "339");
	MapCharacters(0x1167, 0x3155, "33a");
	MapCharacters(0x1168, 0x3156, "33a9");

	MapCharacters(0x1105, 0x3139, "4");

	MapCharacters(0x1106, 0x3141, "5");
	MapCharacters(0x1107, 0x3142, "5a");
	MapCharacters(0x1111, 0x314d, "5aa");
	MapCharacters(0x1108, 0x3143, "5ab");

	MapCharacters(0x1169, 0x3157, "6");
	MapCharacters(0x116a, 0x3158, "63");
	MapCharacters(0x116b, 0x3159, "639");
	MapCharacters(0x116c, 0x315a, "69");
	MapCharacters(0x116d, 0x315b, "6a");

	// Double-6 tap is stored as 66
	MapCharacters(0x116e, 0x315c, "66");
	MapCharacters(0x116f, 0x315d, "663");
	MapCharacters(0x1170, 0x315e, "6639");
	MapCharacters(0x1171, 0x315f, "669");
	MapCharacters(0x1172, 0x3160, "66a");

	MapCharacters(0x1109, 0x3145, "7");
	MapCharacters(0x110c, 0x3148, "7a");
	MapCharacters(0x110e, 0x314a, "7aa");
	MapCharacters(0x110d, 0x3149, "7ab");
	MapCharacters(0x110a, 0x3146, "7b");

	MapCharacters(0x110b, 0x3147, "8");
	MapCharacters(0x1112, 0x314e, "8a");

	MapCharacters(0x1175, 0x3163, "9");

	MapCharacters(0x1173, 0x3161, "0");
	MapCharacters(0x1174, 0x3162, "09");
	}

// ----------------------------------------------------------------------------
// CKoreanKeyMap::MapCharacters
// ----------------------------------------------------------------------------
void CKoreanKeyMap::MapCharacters(QChar aJamoChar,
								  QChar aCompatibilityJamoChar,
								  QString aKeySequence)
	{
	iKeyPressMap.insert(aJamoChar, aKeySequence);
	iKeyPressMap.insert(aCompatibilityJamoChar, aKeySequence);
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

	//QString result = cho + jung + jong; // won't compile
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
