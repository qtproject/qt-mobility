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


#include "t_rndutils.h"

const TInt KMaxWordLength =  64 ;
const TInt KMaxNgramSize = 8;

void CWordNgrams::ConstructL(const TDesC& aFilename, const TInt  aN, TBool aUseNarrowChar)
	{
	ASSERT(aN<KMaxNgramSize && aN>0);
	
	RFs fsSession;
	RFileReadStream fileStream;
	User::LeaveIfError(fsSession.Connect());
	CleanupClosePushL(fsSession);
	User::LeaveIfError(fileStream.Open(fsSession, aFilename, EFileRead));
	CleanupClosePushL(fileStream);
	TUint number = 0;
	TBuf8<KMaxNgramSize> ngram8;
#ifdef _UNICODE
	TBuf<KMaxNgramSize> ngram;
#else
#define  ngram ngram8;
#endif
	TInt err=KErrNone; 
	while(err==KErrNone)
		{
		TRAP(err, number = fileStream.ReadUint8L()); 
		if(err==KErrNone)
			{
			if(aUseNarrowChar)
				{
				fileStream.ReadL(ngram8,aN);
				SetNgram(ngram8, number);
				}
			else
				{
				fileStream.ReadL(ngram,aN);
				SetNgram(ngram, number);
				}
			}
		}
	CleanupStack::PopAndDestroy(2);
	}

	
HBufC*  CWordNgrams::WordLC()
	{
	HBufC* buffer = HBufC::NewLC(KMaxWordLength);
	TPtr word = buffer->Des();
	while(AddLetter(word))
		{}
	return buffer;
	}


/** Return false if done*/
TBool CWordNgrams::AddLetter(TDes& aWord)
	{
	if(aWord.Length()==aWord.MaxLength())
		{
		return EFalse;
		}
	TChar c = NextLetter(aWord);
	if(c==0)
		{
		return EFalse;
		}
	aWord.Append(c);
	return ETrue;
	}

//
// CWordDigrams.
//


CWordDigrams::CWordDigrams(TInt64& aRandSeed) : iRandSeed(aRandSeed)
	{}


CWordDigrams* CWordDigrams::NewLC(const TDesC& aFilename, TInt64& aRandSeed)
	{
	CWordDigrams* self = new(ELeave) CWordDigrams(aRandSeed);
	CleanupStack::PushL(self);
	self->ConstructL(aFilename);
	return self;
	}

void CWordDigrams::SetNgram(const TDesC8& aNgram, TUint aNumber)
	{
	iDigrams[Index(aNgram[0])][Index(aNgram[1])] = (TUint8) aNumber;
	}

void CWordDigrams::SetNgram(const TDesC16& /*aNgram*/, TUint /*aNumber*/)
	{}

void CWordDigrams::ConstructL(const TDesC& aFilename)
	{
	CWordNgrams::ConstructL(aFilename);
	for(TInt i=0;i<KDigramNumberOfChars;i++)
		{
		TUint max =0;
		for(TInt j=0;j<KDigramNumberOfChars;j++)
			{
			max += iDigrams[i][j];
			}
		iSizes[i] = (TUint16)max;
		}
	}



TChar CWordDigrams::NextLetter(const TDesC& aWord)
	{
	TInt len = aWord.Length();
	TInt index = 0;
	if(len!=0)
		{
		index = Index(aWord[len-1]);
		}

	if(iSizes[index]==0)
		{
		return 0;
		}
	TInt x = Math::Rand(iRandSeed) % iSizes[index];
	for(TInt i=0;i<KDigramNumberOfChars;i++)
		{
		if(x<iDigrams[index][i])
			{
			return Char(i);
			}
		x-=iDigrams[index][i];
		if(x<=0) 
			{
			x=1;
			}
		}
	return 0;
	}



TInt CWordDigrams::Index(TChar c)
	{
	
	if(c.IsSpace())
		return 1;
	if(c=='-')
		return 2;
	if(c.IsUpper())
		return ((TUint)c)-'A'+KDigramNumberOfNonLetters;
	if(c.IsLower())
		return ((TUint) c)-'a'+KDigramNumberOfLetters+KDigramNumberOfNonLetters;
	return 0;
	}


TChar CWordDigrams::Char(TInt i)
	{
	if(i==0)
		return 0;
	if(i==1)
		return ' ';
	if(i==2)
		return '-';
	if(i<KDigramNumberOfNonLetters+KDigramNumberOfLetters)
		return TChar('A'+i-KDigramNumberOfNonLetters);
	return TChar('a'+i-KDigramNumberOfNonLetters-KDigramNumberOfLetters);
	}


CWordDigrams::~CWordDigrams()
	{}
