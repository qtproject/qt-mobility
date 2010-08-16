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
* Description: Class to hold the character to key mappings for a virtual
*   qwerty keyboard.
*/

#ifndef __CQWERTYKEYMAP_H__
#define __CQWERTYKEYMAP_H__

// INCLUDE FILES
#include "cpcskeymap.h"
#include <QMap.h>


// FORWARD DECLARATIONS

// CLASS DECLARATION
NONSHARABLE_CLASS(CQwertyKeyMap) : public CPcsKeyMap
	{
	public: // Enums
		enum TQwertyKeyboard
			{
			// How many keys have mappings in the virtual QWERTY keypad.
			// Most languages list 32 keys, some have 36 keys.
			// 4x11 keyboard could have 44 keys.
			EAmountOfKeysInQwertyKeypad = 44
			};

		// These values are used to identify each key in the keyboard.
		enum TKeyId
			{
			// first key at index 0 
			EKeyQ = 0,
			EKeyW,
			EKeyE,
			EKeyR,
			EKeyT,
			EKeyY,
			EKeyU,
			EKeyI,
			EKeyO,
			EKeyP,

			EKeyA, // 10
			EKeyS,
			EKeyD,
			EKeyF,
			EKeyG,
			EKeyH,
			EKeyJ,
			EKeyK,
			EKeyL,

			EKeyZ, // 19
			EKeyX,
			EKeyC,
			EKeyV,
			EKeyB,
			EKeyN,
			EKeyM,
			EKeyColon,
			EKeyDot,
			EKeyDash,	// 28
			EKeyAt,
			EKeyQuote,
			EKeyQuestionMark,

			EKey32,
			EKey33,
			EKey34,
			EKey35,
			EKey36,
			EKey37,
			EKey38,
			EKey39,
			EKey40,
			EKey41,
			EKey42,
			EKey43,

			// Value is KBitsInKeyId amount of 1-bits (i.e. 111111 (binary))
			KPadCharValue = 0x3f
			};

    public: // Constructors and destructor
        /**
         * Two phase construction
         */
        static CQwertyKeyMap* NewL();
        
        /**
         * Destructor
         */
		virtual ~CQwertyKeyMap();

	public: // From CPcsKeyMap
		virtual const QChar ArrayIndexToMappedChar(TInt aArrayIndex) const;
#if !defined(USE_ORBIT_KEYMAP)
		virtual const QChar UseHardcodedKeyMap(const QChar input) const;
#endif

	private: // From CPcsKeyMap
		virtual TInt ComputeValue(QString aString,
								  TBool aUpperLimit,
								  QString& aValue) const;

	public:
#if defined(USE_ORBIT_KEYMAP)
		bool IsValidChar(const QChar aChar) const;
#endif
		TInt MapKeyNameToValue(const QChar aKeyName) const;

	private: // Constructors
        /**
         * Constructor
         */
		CQwertyKeyMap();
		
		/**
		 * Second phase constructor
		 */
		void ConstructL();

	private: // New functions
		void ConstructKeyNameMap();

    private: // Data
		// Mapping between the virtual keys (TKeyId) of the keyboard, and the
		// base character of that key (QChar).
		//
		// TODO: must place QMap's 1st arg the one that used more,
		// since reverse lookup (Qmap::key()) is slow
		QMap<TKeyId, QChar> iKeyNames;

		// List of the valid base characters of the keyboard. Obtained from
		// iKeyNames.
		QList<QChar> iKeyValues;

		// For unit testing
		friend class UT_CQwertyKeyMap;
	};

#endif // __CQWERTYKEYMAP_H__

// End of file
