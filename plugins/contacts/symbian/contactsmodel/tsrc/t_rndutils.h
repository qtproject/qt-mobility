/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/


#ifndef __T_RNDUTILS_H__
#define __T_RNDUTILS_H__


#include "t_utils.h"
#include "t_utils2.h"

const TInt KDigramNumberOfNonLetters =  3;	// space, -, null
const TInt KDigramNumberOfLetters =  26 ;
const TInt KDigramNumberOfChars =  KDigramNumberOfLetters*2+ KDigramNumberOfNonLetters;

/** Used to create randomly generated words using n-grams found in the specified data file.
	Using n-grams lead to much more realistic words and thus makes it obvious a descriptor
	is data and not noise.
	This implementation only supports digrams */
	
class CWordNgrams : public CBase, public MRandomWordGenerator
	{
	public:
		HBufC* WordLC();
		
	protected: 
		
		TBool AddLetter(TDes& aWord);
		void ConstructL(const TDesC& aFilename, TInt aNgramSize = 2, TBool aUseNarrowChar = ETrue);
		virtual TChar NextLetter(const TDesC& aWord) = 0;
		virtual void SetNgram(const TDesC8& aNgram, TUint aNumber) = 0;
		virtual void SetNgram(const TDesC16& aNgram, TUint aNumber) = 0;

	};
	

/** Implementation of CWordNgrams which uses digram frequencies (letter pairs)
	to generate random words */
class CWordDigrams : public CWordNgrams
	{
	public:
		static CWordDigrams* NewLC(const TDesC& aFilename, TInt64& aRandSeed );
		virtual ~CWordDigrams();
		
	protected: 
		void ConstructL(const TDesC& aFilename);
		void SetNgram(const TDesC8& aNgram, TUint aNumber);
		void SetNgram(const TDesC16& aNgram, TUint aNumber); // empty function
		TChar NextLetter(const TDesC& aWord);

	private:
		CWordDigrams(TInt64& aRandSeed);
		static TInt Index(const TChar c);
		static TChar Char(TInt i);
		
	private:
		TInt64& iRandSeed;
		TUint8 iDigrams[KDigramNumberOfChars][KDigramNumberOfChars];
		TUint16 iSizes[KDigramNumberOfChars];
	};



#endif
