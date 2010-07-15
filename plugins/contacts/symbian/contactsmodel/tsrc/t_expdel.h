/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#if !defined(__T_VERS_H__)
    #define __T_VERS_H__
   
#if !defined(__E32BASE_H__)
    #include <e32base.h>
 #endif
#include <versit.h>
#include <vcard.h>
#include <s32stor.h>
#include <s32file.h>

class CVersitTest : public CBase
	{
public:
	CVersitTest();
	~CVersitTest();
	void StreamInL();
	void StreamOutL();
	void StreamLC(RReadStream& aStream);
	void Stream2LC(RReadStream& aStream);
	virtual void LoadFileVersitL()=0;
	void SaveFileVersitL();
	virtual void GetSampleVersitL()=0;
	virtual void GetSampleVersit2L()=0;
protected:
	CVersitParser* iParser;
	CDirectFileStore* iStore; // Store to/Restore from this store
	TBuf8<10000> iVersit;
	};


class CVCardTest : public CVersitTest
	{
public: // framework
	void LoadFileVersitL();
	void SaveFileVersitL();
	void GetSampleVersitL();
	void GetSampleVersit2L();
	};



#endif
