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


#if !defined(__T_SECONDNAME_H__)
    #define __T_SECONDNAME_H__

 
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

/** Parameter object to encapsulate test data. */
class TTestData
	{
public:
	TTestData(const TDesC8& aImportVCard, const TDesC8& aUpdateVCard, 
		const TDesC8& aVersitToken, const TUid aFieldUid, 
		const TDesC& aFieldContent, const TDesC& aUpdatedFieldContent,
		const TDesC& aImportFilename, const TDesC& aExportFilename);
	void SetVCardMapping(TUid aVCardMapping);
public:
	const TDesC8& iImportVCard;
	const TDesC8& iUpdateVCard;
	const TDesC8& iVersitToken;
	const TUid iFieldUid;
	const TDesC& iFieldContent;
	const TDesC& iUpdatedFieldContent;
	const TDesC& iImportFilename;
	const TDesC& iExportFilename;
	TUid iVCardMapping;
	};

/** Base class for new contact field vCard tests. */
class CNewFieldTestBase : public CBase
	{
public:
	~CNewFieldTestBase();
protected:
	CParserVCard* CreateVCardLC(const TDesC8& aContents);
	void WriteVCardL(const TDesC& aFileName, CParserVCard& aVCard);
	TBool CheckSingleFieldValue(CContactItemFieldSet& aFieldSet, TTestData& aTd, const TDesC& aExpectedValue);
	TContactItemId CreateContactL();
	CParserVCard* ParseVCardLC(const TDesC& aFilename);
	CArrayPtr<CContactItem>* ImportVCardLC(const TDesC& aFilename, TBool aConnectWhitespaceOption = ETrue);
	void ExportVCardL(const TDesC& aFilename, CArrayPtr<CContactItem>* aContactArray);
protected:
	void TestImportL(TTestData& aTd);
	void TestExportL(TTestData& aTd);
	void TestUpdateL(TTestData& aTd);
protected:
	CNewFieldTestBase(RTest& aTest, RFs& aFs);
	CNewFieldTestBase();
	void BaseConstructL();
protected:	
	CContactDatabase* iDb;
	RTest& iTest;
	RFs& iFs;
	};

/** Tests for second name field (X-EPOCSECONDNAME). */
class CSecondNameTest : public CNewFieldTestBase
	{
public:
	static CSecondNameTest* NewLC(RTest& aTest, RFs& aFs);
	void RunTestsL();
	~CSecondNameTest();
private:
	CSecondNameTest(RTest& aTest, RFs& aFs);
	void ConstructL();
	};

/** Tests for SIP identity field (X-SIP). */
class CSipIdTest : public CNewFieldTestBase
	{
public:
	static CSipIdTest* NewLC(RTest& aTest, RFs& aFs);
	void RunTestsL();
	~CSipIdTest();
private:
	CSipIdTest(RTest& aTest, RFs& aFs);
	void ConstructL();
	void RunTestCaseL(const TDesC8& aImportVCard, const TDesC8& aUpdateVCard, TUid aVCardMapping);
	};

/** Tests for Wireless Village identity field (X-WV-ID). */
class CWvIdTest : public CNewFieldTestBase
	{
public:
	static CWvIdTest* NewLC(RTest& aTest, RFs& aFs);
	void RunTestsL();
	~CWvIdTest();
private:
	CWvIdTest(RTest& aTest, RFs& aFs);
	void ConstructL();
	};

/** Test for fields limitation at import time - related to defect INC077129 */
class CFieldLimitationTest : public CNewFieldTestBase
	{
public:
	static CFieldLimitationTest* NewLC(RTest& aTest, RFs& aFs);
	void RunTestsL();
	~CFieldLimitationTest();
private:
	CFieldLimitationTest(RTest& aTest, RFs& aFs);
	void ConstructL();
	void RunTestCaseL(const TDesC8& aImportVCard, const TDesC8& aVersitToken, TUid aCardMapping, const TDesC16& aExpectedValue);
 	};

#endif
