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
#include <QList>
#include <QMap.h>
#include <e32base.h>

// FORWARD DECLARATIONS
class QString;
class QChar;
class QTextCodec;


// CLASS DECLARATION
NONSHARABLE_CLASS(CKoreanKeyMap) : public CBase
	{
    public: // Constructors and destructor
		static CKoreanKeyMap* NewL();

        /**
         * Destructor
         */
		virtual ~CKoreanKeyMap();

    public: // New functions
		/**
         * Returns ETrue if aSource contains one or more Korean characters.
         */
		TBool IsKoreanString(QString aSource) const;

// TODO: might append space between boxed-chars (if needed)
		/**
		 * Maps the given string using a built-in Korean key map.
		 * aSource String to be converted
		 * returns Conversion result
		 */
		QString GetMappedString(QString aSource) const;

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

		void MapCharacters(QChar aJamoChar,
						   QChar aCompatibilityJamoChar,
						   QString aKeySequence);

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


		// Not owned
		QTextCodec* iLatinCodec;
		QTextCodec* iKoreanCodec;


		// For unit testing
		friend class UT_CKoreanKeyMap;
	};

#endif // __CKOREANKEYMAP_H__

// End of file
