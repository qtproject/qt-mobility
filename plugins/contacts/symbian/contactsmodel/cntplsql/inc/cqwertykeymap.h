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
			// Max amount of mapped keys. Currently 6 bits are used to identify
			// a key, and the last key is reserved, so it leaves 63 keys.
			EMaxAmountOfKeysInQwertyKeypad = 63,

			// Characters that are not recognized by the keymap, are mapped to
			// this key id.
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

	private: // From CPcsKeyMap
		virtual TInt ComputeValue(QString aString,
								  TBool aUpperLimit,
								  QString& aValue) const;
		virtual TInt ReadExtraCharacters(const HbInputLanguage& aLanguage);

	public:
		bool IsValidChar(const QChar aChar) const;

		TInt MapKeyNameToValue(const QChar aKeyName) const;

		/**
         * Returns the amount of mapped keys (not characters).
         */
		TInt MappedKeyCount() const;

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

		int FindNextFreeKey() const;

		void SetActualNumberOfMappedKeys(int aAmountOfKeys);

    private: // Data
		// Mapping between the virtual keys of the keyboard, and the base
		// character of that key (QChar).
		//
		// Place QMap's first argument the one that used more,
		// since reverse lookup (Qmap::key()) is slow
		QMap<int, QChar> iKeyNames;

		// List of the valid base characters of the keyboard. Obtained from
		// iKeyNames.
		QList<QChar> iKeyValues;

		// For unit testing
		friend class UT_CQwertyKeyMap;
	};

#endif // __CQWERTYKEYMAP_H__

// End of file
