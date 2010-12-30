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
* Description: Class to hold the character to key mappings and provide
*   conversion of strings to their numerical mappings. 
*
*/

#ifndef __CKOREANKEYMAP_H__
#define __CKOREANKEYMAP_H__

// INCLUDES
#include "mlanguagespecifickeymap.h"
#include <QList>
#include <QMap.h>
#include <QChar>
#include <e32base.h>

// FORWARD DECLARATIONS
class QString;
class QTextCodec;
class KoreanInput;


// CLASS DECLARATION
NONSHARABLE_CLASS(CKoreanKeyMap) : public CBase, public MLanguageSpecificKeymap
	{
    public: // Constructors and destructor
		static CKoreanKeyMap* NewL();

        /**
         * Destructor
         */
		virtual ~CKoreanKeyMap();

	public: // From MLanguageSpecificKeymap
		QString GetMappedString(QString aSource) const;

    public: // New functions
		/**
         * Returns ETrue if aSource contains one or more Korean characters.
         */
		TBool IsLanguageSupported(QString aSource) const;

	protected: // Constructors
		/**
         * Constructor
         */
		CKoreanKeyMap();

		/**
		 * Second phase constructor
		 */
		void ConstructL();

	private: // New functions
		void GetTextCodecs();

		void FillKeyPressMap();

		void MapCharacters(QString aKeySequence,
						   QChar aJamoChar,
						   QChar aCompatibilityJamoChar = 0,
						   QChar aOtherJamoChar = 0);

		TBool IsSyllable(QChar aChar) const;

        /**
         * Extracts the Jamos from the complete Korean character (Hangeul).
		 *
		 * Returned string contains the Jamos
         */
		const QString ExtractJamos(QChar aChar) const;

		const QString MapJamoToKeySequence(QChar aChar) const;

	private: // Data
		// Mapping between those Korean unicode characters (Naratgeul logic) and
		// the keypresses used to produce the character.
		QMap<QChar, QString> iKeyPressMap;

		QTextCodec* iLatinCodec;  // Not owned
		QTextCodec* iKoreanCodec; // Not owned

		KoreanInput* iTokenizer; // Owned

		// For unit testing
		friend class UT_CKoreanKeyMap;
	};

#endif // __CKOREANKEYMAP_H__

// End of file
