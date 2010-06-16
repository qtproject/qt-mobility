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
* Description: Class to hold the character to key mappings for a virtual
*   12 key keyboard.
*/

#ifndef __C12KEYKEYMAP_H__
#define __C12KEYKEYMAP_H__

// INCLUDE FILES
#include "cpcskeymap.h"

// FORWARD DECLARATIONS
class QTextCodec;	// if Thai-recog is needed also in qwerty, move QTextCodecs to CPcsKeyMap

// CLASS DECLARATION
NONSHARABLE_CLASS(C12keyKeyMap) : public CPcsKeyMap
	{
    public: // Constructors and destructor
        /**
         * Two phase construction
         */
        static C12keyKeyMap* NewL();
        
        /**
         * Destructor
         */
		virtual ~C12keyKeyMap();

	public: // From CPcsKeyMap
		virtual const QChar ArrayIndexToMappedChar(TInt aArrayIndex) const;
#if !defined(USE_ORBIT_KEYMAP)
		virtual const QChar UseHardcodedKeyMap(const QChar input) const;
#endif

	private: // From CPcsKeyMap
		virtual TInt ComputeValue(QString aString,
								  TBool aUpperLimit,
								  QString& aValue) const;
#if defined(USE_ORBIT_KEYMAP)
		virtual QList<HbInputLanguage> SelectLanguages();

		virtual void SetHardcodedCharacters();
#endif

		/**
         * Returns ETrue if characters that are mapped to * and # keys, should
		 * be skipped.
         */
		virtual TBool DetermineSpecialCharBehaviour(QString aSource) const;
		virtual TBool ShouldSkipChar(QChar aChar, TBool aSkipHashStar) const;

	private: // Constructors
        /**
         * Constructor
         */
		C12keyKeyMap();
		
		/**
		 * Second phase constructor
		 */
		void ConstructL();

	private: // New functions
		void GetTextCodecs();

    private: // Data
		// Not owned
		QTextCodec* iLatinCodec;
		QTextCodec* iThaiCodec;

		// For unit testing
		friend class UT_CPcsKeyMap;
	};

#endif // __C12KEYKEYMAP_H__

// End of file
