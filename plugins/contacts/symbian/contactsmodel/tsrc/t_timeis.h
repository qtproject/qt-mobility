/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#if !defined(__T_TIMEIS_H__)
    #define __T_TTIMEIS_H__

 
#include <e32base.h>
#include <e32def.h>
#include <e32test.h>
#include <f32file.h>
#include <s32file.h>
#include <s32mem.h>
#include <cntdb.h>
#include <cntdef.h>
#include <cntitem.h>
#include <cntfield.h>
#include <cntfldst.h>
#include <cntvcard.h>
#include <vtoken.h>


class CTimeISTest : public CBase
	{
public:
	CTimeISTest();
	~CTimeISTest();

	static CTimeISTest* NewLC(RTest& aTest);
	void CloseDb();
	void RunTestsL();

	RFs & Fs() { return(iFs); }
private:
	void ConstructL(RTest& aTest);
	// Utility Methods
	CParserVCard* CreateVCardLC(const TDesC8& aContents);
	void WriteVCardL(const TDesC& aFileName, CParserVCard& aVCard);
	TBool CheckSingleFieldValue(CContactItemFieldSet& aFieldSet, TFieldType aFieldType, const TDesC& aExpectedValue);
	TBool CheckSingleParameterFieldValues(CContactItemFieldSet& aFieldSet, TFieldType aFieldType, TUid aMapping, const TDesC& aExpectedValue, TBool aFieldPresent = ETrue);
	TContactItemId CreateContactL();
	CParserVCard* ParseVCardLC(const TDesC& aFilename);

	// Test Methods
	CArrayPtr<CContactItem>* ImportVCardLC(const TDesC& aFilename, TBool aConnectWhitespaceOption = ETrue);
	void TestOneL();
	void TestTwoL();
	void TestThree();
	void TestFour();
	void TestFiveL();
	void TestSixL();
	void TestSevenL();
	void TestEightL();
	void TestBirthdayDeletionL(TBool aAddNameField);
	
private:
	RFs iFs;
	RTest* iTest;
	CContactDatabase* iDb;

	}; // CTimeISTest


#endif
