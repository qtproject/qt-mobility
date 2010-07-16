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
* This test code tests the contacts model and vCard import / export functionality 
*
*/


#include "t_secondname.h"
#include <vutil.h>

_LIT(KTestTitle,"T_SecondName - CNTVCARD tests for new contact fields");
RTest test(KTestTitle);

const TPtrC KDatabaseName=_L("C:secondname.cdb");

const TPtrC KTestFile1=_L("C:\\Test1.vcf");
const TPtrC KTestFile2=_L("C:\\Test2.vcf");

void CleanUpResetAndDestroy(TAny *aArray)
	{
	if (aArray)
		{
		CArrayPtr<CContactItem>* array=(CArrayPtr<CContactItem>*)aArray;
		array->ResetAndDestroy();
		delete array;
		}
	}


TTestData::TTestData(const TDesC8& aImportVCard,
	const TDesC8& aUpdateVCard,
	const TDesC8& aVersitToken,
	const TUid aFieldUid,
	const TDesC& aFieldContent,const TDesC& aUpdatedFieldContent,
	const TDesC& aImportFilename,
	const TDesC& aExportFilename)
	: iImportVCard(aImportVCard), iUpdateVCard(aUpdateVCard), iVersitToken(aVersitToken),
	iFieldUid(aFieldUid),iFieldContent(aFieldContent),iUpdatedFieldContent(aUpdatedFieldContent), iImportFilename(aImportFilename),
	iExportFilename(aExportFilename), iVCardMapping(TUid::Null())
	{
	}

void TTestData::SetVCardMapping(TUid aVCardMapping)
	{
	iVCardMapping=aVCardMapping;
	}

CNewFieldTestBase::CNewFieldTestBase(RTest& aTest, RFs& aFs) : iTest(aTest), iFs(aFs)
	{
	}

void CNewFieldTestBase::BaseConstructL()
	{
	iDb = CContactDatabase::ReplaceL(KDatabaseName);
	}

CNewFieldTestBase::~CNewFieldTestBase()
	{
	delete iDb;
    TRAP_IGNORE(CContactDatabase::DeleteDatabaseL(KDatabaseName));
    iFs.Delete(KTestFile1); //ignore return value
    iFs.Delete(KTestFile2); //ignore return value
	}


// ---------------- Utility Methods

// Using a text string, create a vCard. Return the pointer to the newly created instance
// The caller will take ownership of it.

CParserVCard* CNewFieldTestBase::CreateVCardLC(const TDesC8& aContents)
	{
	CParserVCard* vCardParser=CParserVCard::NewL();
	CleanupStack::PushL(vCardParser);

	RDesReadStream stream(aContents);
	stream.PushL();
	vCardParser->InternalizeL(stream);
	CleanupStack::PopAndDestroy(); // stream
	return vCardParser;
	}

// Externalise the VCard to the file. If the file does not exist it is created, if the
// file already exists it is overwritted.
void CNewFieldTestBase::WriteVCardL(const TDesC& aFileName, CParserVCard& aVCard)
	{
	RFileWriteStream vCardWrite;
	vCardWrite.PushL();
	User::LeaveIfError(vCardWrite.Replace(iFs,aFileName,EFileWrite));
	aVCard.ExternalizeL(vCardWrite);
	vCardWrite.CommitL();
	CleanupStack::PopAndDestroy(); //vCardWrite
	}

// Find a field value of type aFieldType and return true if it matches the aExpected value.
TBool CNewFieldTestBase::CheckSingleFieldValue(CContactItemFieldSet& aFieldSet, TTestData& aTd, const TDesC& aExpectedValue)
	{
	TBool correctValue = EFalse;

	TInt pos = aFieldSet.Find(aTd.iFieldUid);
	while ((pos != KErrNotFound) && !correctValue)
		{
		// Verify
		CContactItemField& field = aFieldSet[pos];
		const CContentType& type = field.ContentType();

		if (type.ContainsFieldType(aTd.iVCardMapping))
			{
			CContactTextField* storage = field.TextStorage();
			correctValue = (storage->Text().CompareF(aExpectedValue) == 0);
			}
		pos = aFieldSet.FindNext(aTd.iFieldUid, pos+1);
		}
	//
	return correctValue;
	}

// Create a contact entry in the Contact Database and return the ID

TContactItemId CNewFieldTestBase::CreateContactL()
	{
	const TContactItemId templateId = iDb->TemplateId();
	CContactItem* templateCard = iDb->ReadContactLC(templateId);
	CContactCard* card = CContactCard::NewL(templateCard);
	CleanupStack::PushL(card);
	//
	CContactItemFieldSet& cardFields=card->CardFields();
	// Set the telephone number
	TInt pos=cardFields.Find(KUidContactFieldPhoneNumber);
	if (pos!=KErrNotFound)
		{
		cardFields[pos].SetMapping(KUidContactFieldVCardMapTEL);
		cardFields[pos].TextStorage()->SetTextL(_L("1234"));
		}
	// Set the label
	pos=cardFields.Find(KUidContactFieldNote);
	if (pos!=KErrNotFound)
		{
		cardFields[pos].SetMapping(KUidContactFieldVCardMapLABEL);
		cardFields[pos].TextStorage()->SetTextL(_L(" "));
		}
	//
	const TContactItemId id = iDb->AddNewContactL(*card);
	CleanupStack::PopAndDestroy(2, templateCard);
	return id;
	}


// Read in the file and parse it using Versit
CParserVCard* CNewFieldTestBase::ParseVCardLC(const TDesC& aFilename)
	{
	CParserVCard* vCardParser=CParserVCard::NewL();
	CleanupStack::PushL(vCardParser);
	//
	RFile readFile;
	const TInt error = readFile.Open(iFs, aFilename, EFileRead);
	User::LeaveIfError(error);
	CleanupClosePushL(readFile);
	//
	RFileReadStream stream(readFile);
	stream.PushL();
	vCardParser->InternalizeL(stream);
	CleanupStack::PopAndDestroy(); // stream
	CleanupStack::PopAndDestroy(); // readFile
	return vCardParser;
	}


CArrayPtr<CContactItem>* CNewFieldTestBase::ImportVCardLC(const TDesC& aFilename, TBool aConnectWhitespaceOption)
	{
	CArrayPtr<CContactItem>* contactItems;
	RFileReadStream vCardRead;
	TBool importOK;

	vCardRead.PushL();
	User::LeaveIfError(vCardRead.Open(iFs,aFilename,EFileRead));
	// Just a precaution. This array must be empty before we go and add more data
	if (aConnectWhitespaceOption)
		{
		contactItems = iDb->ImportContactsL(TUid::Uid(KUidVCardConvDefaultImpl),vCardRead,importOK,(CContactVCardConverter::EImportSingleContact | CContactVCardConverter::EConnectWhitespace));
		}
	else
		{
		contactItems = iDb->ImportContactsL(TUid::Uid(KUidVCardConvDefaultImpl),vCardRead,importOK,(CContactVCardConverter::EImportSingleContact));
		}
	CleanupStack::PopAndDestroy(); // vCardRead
	CleanupStack::PushL(contactItems);
	return contactItems;
	}

void CNewFieldTestBase::ExportVCardL(const TDesC& aFilename, CArrayPtr<CContactItem>* aContactArray)
	{
	RFileWriteStream vCardWrite;
	CContactIdArray* idArray = CContactIdArray::NewLC();

	for (TInt i = 0; i < aContactArray->Count(); i++)
		{
		idArray->AddL(aContactArray->At(i)->Id());
		}

	vCardWrite.PushL();
	User::LeaveIfError(vCardWrite.Replace(iFs,aFilename,EFileWrite));
	iDb->ExportSelectedContactsL(TUid::Uid(KUidVCardConvDefaultImpl),*idArray,vCardWrite,0);
	vCardWrite.CommitL();
	CleanupStack::PopAndDestroy(); //vCardWrite
	CleanupStack::PopAndDestroy(idArray);
	}

/** Tests vCard import for the new field.
This test creates a vCard from the supplied test data and imports it.
The contact field content is checked against the test data and the
contact is exported ready for the next test.
*/
void CNewFieldTestBase::TestImportL(TTestData& aTd)
	{
	iTest.Printf(_L("Creating test vCard Files\n"));
	CParserVCard* vCard = CreateVCardLC(aTd.iImportVCard);
	WriteVCardL(aTd.iImportFilename,*vCard);
	CleanupStack::PopAndDestroy(vCard);

	iTest.Printf(_L("Importing vCard files to contacts\n"));
	iTest.Printf(_L("New File\n"));
	CArrayPtr<CContactItem>* contactItems = ImportVCardLC(aTd.iImportFilename);
	iTest(contactItems->Count() == 1);
	CContactItem* cItem = iDb->ReadContactLC(contactItems->At(0)->Id());
	iTest(CheckSingleFieldValue(cItem->CardFields(),aTd,aTd.iFieldContent));

	ExportVCardL(aTd.iExportFilename,contactItems);
	contactItems->ResetAndDestroy();
	CleanupStack::PopAndDestroy(cItem);
	CleanupStack::PopAndDestroy(contactItems);
	iTest.Printf(_L("Behaviour correct\n"));
	}

/** Tests vCard export for the new field.
This test checks that the exported vCard contains the correct
field and field content.
*/
void CNewFieldTestBase::TestExportL(TTestData& aTd)
	{
	CParserVCard* cardParser = CParserVCard::NewL();
	CleanupStack::PushL(cardParser);
	RFileReadStream vCardRead;
	vCardRead.PushL();
	User::LeaveIfError(vCardRead.Open(iFs,aTd.iImportFilename,EFileRead));
	cardParser->InternalizeL(vCardRead);
	CArrayPtr<CParserProperty>* arrayOfProperties = cardParser->ArrayOfProperties();
	CleanupStack::PushL(TCleanupItem(CleanUpResetAndDestroy, arrayOfProperties));
	TBool FieldFound = EFalse;
	TInt Counter = 0;
	TInt Max = arrayOfProperties->Count();
	CParserProperty* aProperty = NULL;
	while (!FieldFound && (Counter < Max))
		{
		aProperty = (*arrayOfProperties)[Counter];
		if (aProperty->Name() == aTd.iVersitToken)
			{
			FieldFound = ETrue;
			}
		++Counter;
		}
	iTest.Printf(_L("Checking Field is present in the vCard\n"));
	iTest(FieldFound);
	iTest.Printf(_L("Present. Checking Value\n"));
	CParserPropertyValueHBufC* propertyValueWrapper = static_cast<CParserPropertyValueHBufC*>(aProperty->Value());
	const TPtrC propertyValue(propertyValueWrapper->Value());
	iTest(propertyValue.CompareF(aTd.iFieldContent) == 0);
	iTest.Printf(_L("Value Correct\n"));
	arrayOfProperties->ResetAndDestroy();
	CleanupStack::PopAndDestroy(arrayOfProperties);
	CleanupStack::PopAndDestroy(); // vCardRead
	CleanupStack::PopAndDestroy(cardParser);
	}

/** Tests vCard update for the new field.
This test modifies the contact by importing new vCard test data
and checking the field is correctly updated.
*/
void CNewFieldTestBase::TestUpdateL(TTestData& aTd)
	{
	iTest.Printf(_L("Updating vCard\n"));
	CParserVCard* vCard = CreateVCardLC(aTd.iUpdateVCard);
	WriteVCardL(KTestFile1,*vCard);
	CleanupStack::PopAndDestroy(vCard);
	iTest.Printf(_L("Importing vCard files to contacts\n"));
	CArrayPtr<CContactItem>* contactItems = ImportVCardLC(aTd.iImportFilename);
	iTest(contactItems->Count() == 1);
	CContactItem* cItem = iDb->ReadContactLC(contactItems->At(0)->Id());
	iTest(CheckSingleFieldValue(cItem->CardFields(),aTd,aTd.iUpdatedFieldContent));
	contactItems->ResetAndDestroy();
	CleanupStack::PopAndDestroy(cItem);
	CleanupStack::PopAndDestroy(contactItems);
	iTest.Printf(_L("Behaviour correct\n"));
	}


CSecondNameTest::CSecondNameTest(RTest& aTest, RFs& aFs) : CNewFieldTestBase(aTest,aFs)
	{
	}

CSecondNameTest::~CSecondNameTest()
	{
	}

CSecondNameTest* CSecondNameTest::NewLC(RTest& aTest, RFs& aFs)
	{
	CSecondNameTest* self=new(ELeave) CSecondNameTest(aTest, aFs);
	CleanupStack::PushL(self);
	self->ConstructL();
	return(self);
	}

void CSecondNameTest::ConstructL()
	{
	BaseConstructL();
	};

void CSecondNameTest::RunTestsL()
	{
	_LIT(KSecondNameTestDescription,"Second Name Tests Starting\n");
	iTest.Next(_L("Second Name Tests Starting"));

	_LIT8(KImportVCard, "BEGIN:VCARD\r\nVERSION:2.1\r\nUID:1\r\nX-EPOCSECONDNAME:test\r\nEND:VCARD\r\n");
	_LIT8(KUpdateVCard, "BEGIN:VCARD\r\nVERSION:2.1\r\nUID:1\r\nX-EPOCSECONDNAME:change\r\nEND:VCARD\r\n");
	_LIT(KFieldContent, "test");
	_LIT(KUpdatedFieldContent,"change");
	TTestData t(KImportVCard,KUpdateVCard,KVersitTokenSECONDNAME,KUidContactFieldSecondName,KFieldContent,KUpdatedFieldContent,KTestFile1,KTestFile2);
	t.SetVCardMapping(KUidContactFieldVCardMapSECONDNAME);
	TestImportL(t);
	TestExportL(t);
	TestUpdateL(t);
	}


CSipIdTest::CSipIdTest(RTest& aTest, RFs& aFs) : CNewFieldTestBase(aTest,aFs)
	{
	}

CSipIdTest::~CSipIdTest()
	{
	}

CSipIdTest* CSipIdTest::NewLC(RTest& aTest, RFs& aFs)
	{
	CSipIdTest* self=new(ELeave) CSipIdTest(aTest, aFs);
	CleanupStack::PushL(self);
	self->ConstructL();
	return(self);
	}

void CSipIdTest::ConstructL()
	{
	BaseConstructL();
	};


void CSipIdTest::RunTestsL()
	{
	_LIT(KSipTestDescription,"SIP Identity Field Tests starting\n");
	iTest.Next(_L("SIP Identity Field Tests starting"));


	// Test X-SIP:
	_LIT8(KImportVCard, "BEGIN:VCARD\r\nVERSION:2.1\r\nUID:1\r\nX-SIP:sip:alice@atlanta.com\r\nEND:VCARD\r\n");
	_LIT8(KUpdateVCard, "BEGIN:VCARD\r\nVERSION:2.1\r\nUID:1\r\nX-SIP:sip:bob@biloxi.com\r\nEND:VCARD\r\n");
	RunTestCaseL(KImportVCard,KUpdateVCard,KUidContactFieldVCardMapSIPID);

	// Test X-SIP;POC:
	_LIT8(KImportVCardPOC, "BEGIN:VCARD\r\nVERSION:2.1\r\nUID:2\r\nX-SIP;POC:sip:alice@atlanta.com\r\nEND:VCARD\r\n");
	_LIT8(KUpdateVCardPOC, "BEGIN:VCARD\r\nVERSION:2.1\r\nUID:2\r\nX-SIP;POC:sip:bob@biloxi.com\r\nEND:VCARD\r\n");
	RunTestCaseL(KImportVCardPOC,KUpdateVCardPOC,KUidContactFieldVCardMapPOC);

	// Test X-SIP;SWIS:
	_LIT8(KImportVCardSWIS, "BEGIN:VCARD\r\nVERSION:2.1\r\nUID:3\r\nX-SIP;SWIS:sip:alice@atlanta.com\r\nEND:VCARD\r\n");
	_LIT8(KUpdateVCardSWIS, "BEGIN:VCARD\r\nVERSION:2.1\r\nUID:3\r\nX-SIP;SWIS:sip:bob@biloxi.com\r\nEND:VCARD\r\n");
	RunTestCaseL(KImportVCardSWIS,KUpdateVCardSWIS,KUidContactFieldVCardMapSWIS);

	// Test X-SIP;VOIP:
	_LIT8(KImportVCardVOIP, "BEGIN:VCARD\r\nVERSION:2.1\r\nUID:4\r\nX-SIP;VOIP:sip:alice@atlanta.com\r\nEND:VCARD\r\n");
	_LIT8(KUpdateVCardVOIP, "BEGIN:VCARD\r\nVERSION:2.1\r\nUID:4\r\nX-SIP;VOIP:sip:bob@biloxi.com\r\nEND:VCARD\r\n");
	RunTestCaseL(KImportVCardVOIP,KUpdateVCardVOIP,KUidContactFieldVCardMapVOIP);

	// Test alternative representation - X-SIP;TYPE=POC:
	_LIT8(KImportVCardAltPOCImport, "BEGIN:VCARD\r\nVERSION:2.1\r\nUID:5\r\nX-SIP;TYPE=POC:sip:alice@atlanta.com\r\nEND:VCARD\r\n");
	_LIT8(KUpdateVCardAltPOCUpdate, "BEGIN:VCARD\r\nVERSION:2.1\r\nUID:5\r\nX-SIP;TYPE=POC:sip:bob@biloxi.com\r\nEND:VCARD\r\n");
	RunTestCaseL(KImportVCardAltPOCImport,KUpdateVCardAltPOCUpdate,KUidContactFieldVCardMapPOC);

	// Test alternative representation - X-SIP;TYPE=SWIS:
	_LIT8(KImportVCardAltSWISImport, "BEGIN:VCARD\r\nVERSION:2.1\r\nUID:6\r\nX-SIP;TYPE=SWIS:sip:alice@atlanta.com\r\nEND:VCARD\r\n");
	_LIT8(KUpdateVCardAltSWISUpdate, "BEGIN:VCARD\r\nVERSION:2.1\r\nUID:6\r\nX-SIP;TYPE=SWIS:sip:bob@biloxi.com\r\nEND:VCARD\r\n");
	RunTestCaseL(KImportVCardAltSWISImport,KUpdateVCardAltSWISUpdate,KUidContactFieldVCardMapSWIS);

	// Test alternative representation - X-SIP;TYPE=VOIP:
	_LIT8(KImportVCardAltVOIPImport, "BEGIN:VCARD\r\nVERSION:2.1\r\nUID:7\r\nX-SIP;TYPE=VOIP:sip:alice@atlanta.com\r\nEND:VCARD\r\n");
	_LIT8(KUpdateVCardAltVOIPUpdate, "BEGIN:VCARD\r\nVERSION:2.1\r\nUID:7\r\nX-SIP;TYPE=VOIP:sip:bob@biloxi.com\r\nEND:VCARD\r\n");
	RunTestCaseL(KImportVCardAltVOIPImport,KUpdateVCardAltVOIPUpdate,KUidContactFieldVCardMapVOIP);
	}

void CSipIdTest::RunTestCaseL(const TDesC8& aImportVCard, const TDesC8& aUpdateVCard, TUid aVCardMapping)
	{
	_LIT(KFieldContent, "sip:alice@atlanta.com");
	_LIT(KUpdatedFieldContent, "sip:bob@biloxi.com");
	_LIT8(KVersitTokenSIPID,"X-SIP");

	TTestData t(aImportVCard,aUpdateVCard,KVersitTokenSIPID,KUidContactFieldSIPID,KFieldContent,KUpdatedFieldContent,KTestFile1,KTestFile2);
	t.SetVCardMapping(aVCardMapping);
	TestImportL(t);
	TestExportL(t);
	TestUpdateL(t);
	}


CWvIdTest::CWvIdTest(RTest& aTest, RFs& aFs) : CNewFieldTestBase(aTest,aFs)
	{
	}

CWvIdTest::~CWvIdTest()
	{
	}

CWvIdTest* CWvIdTest::NewLC(RTest& aTest, RFs& aFs)
	{
	CWvIdTest* self=new(ELeave) CWvIdTest(aTest, aFs);
	CleanupStack::PushL(self);
	self->ConstructL();
	return(self);
	}

void CWvIdTest::ConstructL()
	{
	BaseConstructL();
	};

/**
@SYMTestCaseID     PIM-T-SECONDNAME-0002
@SYMTestType UT
@SYMTestPriority Urgent
@SYMDEF INC063619
@SYMTestCaseDesc To verify that the vCard "X-WV-ID" property added to address
the problem described in INC063619 works as expected.
@SYMTestActions The vCard KImportVCard is imported and tested as per the
description of the method CNewFieldTestBase::TestImportL().  Next, the vCard
exported by this method is tested as per the description of the method
CNewFieldTestBase::TestExportL().  Finally, the vCard KUpdateVCard is imported
and tested as per description of the method CNewFieldTestBase::TestUpdateL().
@SYMTestExpectedResults See the descriptions of the methods mentioned above.
*/

void CWvIdTest::RunTestsL()
	{
	_LIT(KWvidTestDescription,"Wireless Village Identity Field Tests starting\n");
	iTest.Next(_L("@SYMTESTCaseID:PIM-T-SECONDNAME-0002 Wireless Village Identity Field Tests starting"));


	_LIT8(KImportVCard,"BEGIN:VCARD\r\nVERSION:2.1\r\nUID:1\r\nX-WV-ID:test\r\nEND:VCARD\r\n");
	_LIT8(KUpdateVCard,"BEGIN:VCARD\r\nVERSION:2.1\r\nUID:1\r\nX-WV-ID:change\r\nEND:VCARD\r\n");
	_LIT(KFieldContent,"test");
	_LIT(KUpdatedFieldContent,"change");
	_LIT8(KVersitTokenWVID,"X-WV-ID");

	TTestData t(KImportVCard,KUpdateVCard,KVersitTokenWVID,KUidContactFieldIMAddress,KFieldContent,KUpdatedFieldContent,KTestFile1,KTestFile2);

	t.SetVCardMapping(KUidContactFieldVCardMapWV);

	TestImportL(t);
	TestExportL(t);
	TestUpdateL(t);
	}

CFieldLimitationTest::CFieldLimitationTest(RTest& aTest, RFs& aFs) : CNewFieldTestBase(aTest,aFs)
	{
	}

CFieldLimitationTest::~CFieldLimitationTest()
	{
	}

CFieldLimitationTest* CFieldLimitationTest::NewLC(RTest& aTest, RFs& aFs)
	{
	CFieldLimitationTest* self=new(ELeave) CFieldLimitationTest(aTest, aFs);
	CleanupStack::PushL(self);
	self->ConstructL();
	return(self);
	}

void CFieldLimitationTest::ConstructL()
	{
	BaseConstructL();
	};

void CFieldLimitationTest::RunTestsL()
	{
	_LIT(KNameTestDescription,"Field limitation Tests Starting\n");
	iTest.Next(_L("Field limitation Tests Starting"));


	iTest.Printf(_L("Test if name is truncated or not at import"));
	_LIT8(KImportNameVCard, "BEGIN:VCARD\r\nVERSION:2.1\r\nN:12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890;;;\r\nFN:John Smith\r\nTEL;CELL:0123456789\r\nADR;HOME:;;;;;;\r\nADR;WORK:;;;;;;\r\nEND:VCARD");
	_LIT(KFieldNameExpectedContent, "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345");
	RunTestCaseL(KImportNameVCard, KVersitTokenFN, KUidContactFieldFamilyName, KFieldNameExpectedContent);

	iTest.Printf(_L("Test if company name is truncated or not at import"));
	_LIT8(KImportOrgVCard, "BEGIN:VCARD\r\nVERSION:2.1\r\nN:John Smith;;;\r\nFN:John Smith\r\nORG:12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890\r\nTEL;CELL:0123456789\r\nADR;HOME:;;;;;;\r\nADR;WORK:;;;;;;\r\nEND:VCARD");
	_LIT(KFieldOrgExpectedContent, "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345");
	RunTestCaseL(KImportOrgVCard, KVersitTokenORG, KUidContactFieldCompanyName, KFieldOrgExpectedContent);

	iTest.Printf(_L("Test if email is truncated or not at import"));
	_LIT8(KImportEmailVCard, "BEGIN:VCARD\r\nVERSION:2.1\r\nN:John Smith;;;\r\nFN:John Smith\r\nEMAIL:12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890\r\nTEL;CELL:0123456789\r\nADR;HOME:;;;;;;\r\nADR;WORK:;;;;;;\r\nEND:VCARD");
	_LIT(KFieldEmailExpectedContent, "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345");
	RunTestCaseL(KImportEmailVCard, KVersitTokenEMAIL, KUidContactFieldEMail, KFieldEmailExpectedContent);

	iTest.Printf(_L("Test if note is truncated or not at import"));
	_LIT8(KImportNoteVCard, "BEGIN:VCARD\r\nVERSION:2.1\r\nN:Smith;John;;;\r\nFN:John Smith\r\nNOTE:12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890\r\nTEL;CELL:0123456789\r\nADR;HOME:;;;;;;\r\nADR;WORK:;;;;;;\r\nEND:VCARD");
	_LIT(KFieldNoteExpectedContent, "12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890");
	RunTestCaseL(KImportNoteVCard, KVersitTokenNOTE, KUidContactFieldVCardMapNOTE, KFieldNoteExpectedContent);
	}

void CFieldLimitationTest::RunTestCaseL(const TDesC8& aImportVCard, const TDesC8& aVersitToken, TUid aCardMapping, const TDesC16& aExpectedValue)
	{
	_LIT8(KUpdateVCard, ""); //empty - we do not test the update
	_LIT(KUpdatedFieldContent,""); //empty - we do not test the update
	TTestData t(aImportVCard,KUpdateVCard,aVersitToken,aCardMapping,aExpectedValue,KUpdatedFieldContent,KTestFile1,KTestFile2);
	t.SetVCardMapping(aCardMapping);
	TestImportL(t);
	}

void DoTestsL()
	{
	CActiveScheduler::Install(new(ELeave) CActiveScheduler);
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	CSecondNameTest* secondNameTests = CSecondNameTest::NewLC(test,fs);
	secondNameTests->RunTestsL();
	CleanupStack::PopAndDestroy(secondNameTests);

	CSipIdTest* sipIdTests = CSipIdTest::NewLC(test,fs);
	sipIdTests->RunTestsL();
	CleanupStack::PopAndDestroy(sipIdTests);

	CWvIdTest* wvIdTests = CWvIdTest::NewLC(test,fs);
	wvIdTests->RunTestsL();
	CleanupStack::PopAndDestroy(wvIdTests);

	CFieldLimitationTest* fieldLimitationTest = CFieldLimitationTest::NewLC(test,fs);
	fieldLimitationTest->RunTestsL();
	CleanupStack::PopAndDestroy(fieldLimitationTest);

	CleanupStack::PopAndDestroy(&fs);
	delete CActiveScheduler::Current();
	}

/**

@SYMTestCaseID     PIM-T-SECONDNAME-0001

*/

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	test.Title();
	test.Start(_L("@SYMTESTCaseID:PIM-T-SECONDNAME-0001 T_SecondName - CNTVCARD tests for new contact fields"));

	CTrapCleanup* theCleanup = CTrapCleanup::New();
	if (!theCleanup)
		return KErrNoMemory;
	TRAPD(ret, DoTestsL());
	test(ret == KErrNone);
	delete theCleanup;
	test.End();
	test.Close();
	__UHEAP_MARKEND;
	return(0);
	}
