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
* This test code tests the vCard import / export functionality that was introduced as a result of the 
* defect DEF023159. This defect requires the implementation of a flag which tells the import/export
* routines to ignore white-space only valued properties within a Contact Entry / vCard file.
*
*/


#include "t_timeis.h"
#include <vutil.h>
#include <coreappstest/testserver.h>
// include templates for CleanupResetAndDestroyPushL(T)
#include "cntviewprivate.h"


RTest test(_L("T_TimeIS - exp/importing using TimeIS protocol"));

const TPtrC KDatabaseName=_L("C:TimeISdb.cdb");

const TPtrC KTestFile4_1=_L("Z:\\t_TimeIS\\Connectivity.vcf");

const TPtrC KTestFile1_1=_L("C:\\Test1_1.vcf");
const TPtrC KTestFile1_2=_L("C:\\Test1_2.vcf");
const TPtrC KTestFile1_3=_L("C:\\Test1_3.vcf");
const TPtrC KTestFile3_1=_L("C:\\Test3_1.vcf");
const TPtrC KTestFile5_1=_L("C:\\Test5_1.vcf");
const TPtrC KTestFile5_2=_L("C:\\Test5_2.vcf");
const TPtrC KTestFile5_3=_L("C:\\Test5_3.vcf");
const TPtrC KTestFile6_1=_L("C:\\Test6_1.vcf");
const TPtrC KTestFile6_2=_L("C:\\Test6_2.vcf");
const TPtrC KTestFile7_1=_L("C:\\Test7_1.vcf");
const TPtrC KTestFile7_2=_L("C:\\Test7_2.vcf");
const TPtrC KTestFile7_3=_L("C:\\Test7_3.vcf");
const TPtrC KTestFile8_1=_L("C:\\Test8_1.vcf");
const TPtrC KTestFile8_2=_L("C:\\Test8_2.vcf");
const TPtrC KTestFile8_3=_L("C:\\Test8_3.vcf");


CTimeISTest::CTimeISTest()
	{

	} // Contructor


CTimeISTest::~CTimeISTest()
	{
	delete iDb;
	CActiveScheduler* scheduler=CActiveScheduler::Current(); //get current
	CActiveScheduler::Install(NULL); //uninstall
	delete scheduler; // delete uninstalled scheduler
	iFs.Close();
	} // Destructor

CTimeISTest* CTimeISTest::NewLC(RTest& aTest)
	{
	CTimeISTest* self=new(ELeave) CTimeISTest();
	CleanupStack::PushL(self);
	self->ConstructL(aTest);
	return(self);
	}  //NewLC

void CTimeISTest::CloseDb()
	{
	delete iDb;
	iDb = 0;
	}

void CTimeISTest::ConstructL(RTest& aTest)
	{
	User::LeaveIfError(iFs.Connect());
	CActiveScheduler::Install(new(ELeave) CActiveScheduler);
	iTest = &aTest;
	iDb = CContactDatabase::ReplaceL(KDatabaseName);
	}; // ConstructL

// ---------------- Utility Methods

// Using a text string, create a vCard. Return the pointer to the newly created instance
// The caller will take ownership of it.

CParserVCard* CTimeISTest::CreateVCardLC(const TDesC8& aContents)
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
void CTimeISTest::WriteVCardL(const TDesC& aFileName, CParserVCard& aVCard)
	{
	RFileWriteStream vCardWrite;
	vCardWrite.PushL();
	User::LeaveIfError(vCardWrite.Replace(iFs,aFileName,EFileWrite));
	aVCard.ExternalizeL(vCardWrite);
	vCardWrite.CommitL();
	CleanupStack::PopAndDestroy(); //vCardWrite
	}

// Find a field value of type aFieldType and return true if it matches the aExpected value.
TBool CTimeISTest::CheckSingleFieldValue(CContactItemFieldSet& aFieldSet, TFieldType aFieldType, const TDesC& aExpectedValue)
	{
	TBool correctValue = EFalse;

	TInt pos = aFieldSet.Find(aFieldType);
	if	(pos != KErrNotFound)
		{
		// Verify
		CContactItemField& field = aFieldSet[pos];
		CContactTextField* storage = field.TextStorage();
		correctValue = (storage->Text().CompareF(aExpectedValue) == 0);
		}
	//
	return correctValue;
	}
	
// Find a parameter and field value of type aFieldType and return true if it matches the aExpected value.
TBool CTimeISTest::CheckSingleParameterFieldValues(CContactItemFieldSet& aFieldSet, TFieldType aFieldType, TUid aMapping, const TDesC& aExpectedValue, TBool aFieldPresent)
	{
	TBool correctValue = EFalse;
	TInt pos = 0;
	
	TBool fieldFound = EFalse;
	TInt startPos = KContactFieldSetSearchAll;
	
	while(!fieldFound && !(pos == KErrNotFound))
		{
		pos = aFieldSet.FindNext(aFieldType, startPos);
		startPos = pos + 1;
		if (pos != KErrNotFound )
			{
			CContactItemField& tempField = aFieldSet[pos];
			const CContentType& tempContentType = tempField.ContentType();
			
			if (aMapping == KNullUid)
				{
				TBool additionalMapFound = EFalse;
				additionalMapFound |= tempContentType.ContainsFieldType(KUidContactFieldVCardMapHOME);
				additionalMapFound |= tempContentType.ContainsFieldType(KUidContactFieldVCardMapWORK);
				additionalMapFound |= tempContentType.ContainsFieldType(KUidContactFieldVCardMapPREF);
				if (!additionalMapFound)
					{
					fieldFound = ETrue;
					}
				}
			else
				{
				fieldFound = tempContentType.ContainsFieldType(aMapping);
				}
			}
		}
	
	if (fieldFound)
		{
		CContactItemField& aItemField = aFieldSet[pos];

		// Verify
		CContactTextField* storage = aItemField.TextStorage();
		correctValue = (storage->Text().CompareF(aExpectedValue) == 0);
		}
	if (!aFieldPresent && !fieldFound)
		{
		correctValue = ETrue;
		}
		
	return correctValue;
	}

// Create a contact entry in the Contact Database and return the ID

TContactItemId CTimeISTest::CreateContactL()
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
CParserVCard* CTimeISTest::ParseVCardLC(const TDesC& aFilename)
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


// ---------------- Test Methods

CArrayPtr<CContactItem>* CTimeISTest::ImportVCardLC(const TDesC& aFilename, TBool aConnectWhitespaceOption)
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

void CTimeISTest::TestOneL()
	{
	// Create a vCard with actual telephone number
	iTest->Printf(_L("Creating test vCard Files\n"));
	_LIT8(KRealTel, "BEGIN:VCARD\r\nVERSION:2.1\r\nUID:1\r\nNOTE:text\r\nEND:VCARD\r\n");
	CParserVCard* vCard = CreateVCardLC(KRealTel);
	WriteVCardL(KTestFile1_1,*vCard);
	CleanupStack::PopAndDestroy(vCard);
	// Create a vCard with spaced telephone number
	_LIT8(KSpaceTel, "BEGIN:VCARD\r\nVERSION:2.1\r\nUID:1\r\nNOTE: \r\nTEL:1\r\nEND:VCARD\r\n");
	vCard = CreateVCardLC(KSpaceTel);
	WriteVCardL(KTestFile1_2,*vCard);
	CleanupStack::PopAndDestroy(vCard);
	// Create a vCard with null telephone number
	_LIT8(KNULLTel, "BEGIN:VCARD\r\nVERSION:2.1\r\nUID:1\r\nNOTE:\r\nTEL:1\r\nEND:VCARD\r\n");
	vCard = CreateVCardLC(KNULLTel);
	WriteVCardL(KTestFile1_3,*vCard);
	CleanupStack::PopAndDestroy(vCard);


	// Now import the files and check the behaviour
	iTest->Printf(_L("Importing vCard files to contacts\n"));
	iTest->Printf(_L("New File\n"));
	CArrayPtr<CContactItem>* contactItems = ImportVCardLC(KTestFile1_1);
	(*iTest)(contactItems->Count() == 1);
	CContactItem* cItem = iDb->ReadContactLC(contactItems->At(0)->Id());
	(*iTest)(CheckSingleFieldValue(cItem->CardFields(),KUidContactFieldNote,_L("text")));
	contactItems->ResetAndDestroy();
	CleanupStack::PopAndDestroy(cItem);
	CleanupStack::PopAndDestroy(contactItems);


	iTest->Printf(_L("Spaced value field\n"));
	contactItems = ImportVCardLC(KTestFile1_2);
	(*iTest)(contactItems->Count() == 1);
	cItem = iDb->ReadContactLC(contactItems->At(0)->Id());
	(*iTest)(CheckSingleFieldValue(cItem->CardFields(),KUidContactFieldNote,_L("text")));
	contactItems->ResetAndDestroy();
	CleanupStack::PopAndDestroy(cItem);
	CleanupStack::PopAndDestroy(contactItems);

	iTest->Printf(_L("NULL value field\n"));
	contactItems = ImportVCardLC(KTestFile1_3);
	(*iTest)(contactItems->Count() == 1);
	cItem = iDb->ReadContactLC(contactItems->At(0)->Id());
	(*iTest)(CheckSingleFieldValue(cItem->CardFields(),KUidContactFieldNote,_L("")));
	contactItems->ResetAndDestroy();
	CleanupStack::PopAndDestroy(cItem);
	CleanupStack::PopAndDestroy(contactItems);

	iTest->Printf(_L("Behaviour correct\n"));
	}

void CTimeISTest::TestTwoL()
	{
	// Create a vCard with actual telephone number, Home Work Preferred and default addresses.
	iTest->Printf(_L("Creating test vCard Files\n"));
	_LIT8(KNewN, "BEGIN:VCARD\r\nVERSION:2.1\r\nUID:1\r\nN:A;B;C\r\nEND:VCARD\r\n");
	CParserVCard* vCard = CreateVCardLC(KNewN);
	WriteVCardL(KTestFile1_1,*vCard);
	CleanupStack::PopAndDestroy(vCard);
	// Create a vCard with spaced telephone number, null address regions.
	_LIT8(KModN, "BEGIN:VCARD\r\nVERSION:2.1\r\nUID:1\r\nN:A;;C\r\nTEL:1\r\nEND:VCARD\r\n");
	vCard = CreateVCardLC(KModN);
	WriteVCardL(KTestFile1_2,*vCard);
	CleanupStack::PopAndDestroy(vCard);
	// Create a vCard with null telephone number, spaced address localities.
	_LIT8(KDelN, "BEGIN:VCARD\r\nVERSION:2.1\r\nUID:1\r\nN: ;;C\r\nTEL:2\r\nEND:VCARD\r\n");
	vCard = CreateVCardLC(KDelN);
	WriteVCardL(KTestFile1_3,*vCard);
	CleanupStack::PopAndDestroy(vCard);

	// Now import the files and check the behaviour
	iTest->Printf(_L("Importing vCard files to contacts\n"));
	iTest->Printf(_L("New File\n"));
	CArrayPtr<CContactItem>* contactItems = ImportVCardLC(KTestFile1_1);
	(*iTest)(contactItems->Count() == 1);
	CContactItem* cItem = iDb->ReadContactLC(contactItems->At(0)->Id());
	
	(*iTest)(CheckSingleFieldValue(cItem->CardFields(),KUidContactFieldFamilyName,_L("A")));
	(*iTest)(CheckSingleFieldValue(cItem->CardFields(),KUidContactFieldGivenName,_L("B")));
	(*iTest)(CheckSingleFieldValue(cItem->CardFields(),KUidContactFieldAdditionalName,_L("C")));
	
	contactItems->ResetAndDestroy();
	CleanupStack::PopAndDestroy(cItem);
	CleanupStack::PopAndDestroy(contactItems);

		
	iTest->Printf(_L("A non supported field\n"));
	contactItems = ImportVCardLC(KTestFile1_2);
	(*iTest)(contactItems->Count() == 1);
	cItem = iDb->ReadContactLC(contactItems->At(0)->Id());
	(*iTest)(CheckSingleFieldValue(cItem->CardFields(),KUidContactFieldFamilyName,_L("A")));
	(*iTest)(CheckSingleFieldValue(cItem->CardFields(),KUidContactFieldGivenName,_L("B")));
	(*iTest)(CheckSingleFieldValue(cItem->CardFields(),KUidContactFieldAdditionalName,_L("C")));
	contactItems->ResetAndDestroy();
	CleanupStack::PopAndDestroy(cItem);
	CleanupStack::PopAndDestroy(contactItems);
	

	iTest->Printf(_L("Empty field\n"));
	contactItems = ImportVCardLC(KTestFile1_3);
	(*iTest)(contactItems->Count() == 1);
	cItem = iDb->ReadContactLC(contactItems->At(0)->Id());
	(*iTest)(CheckSingleFieldValue(cItem->CardFields(),KUidContactFieldFamilyName,_L("")));
	(*iTest)(CheckSingleFieldValue(cItem->CardFields(),KUidContactFieldGivenName,_L("B")));
	(*iTest)(CheckSingleFieldValue(cItem->CardFields(),KUidContactFieldAdditionalName,_L("C")));
	contactItems->ResetAndDestroy();
	CleanupStack::PopAndDestroy(cItem);
	CleanupStack::PopAndDestroy(contactItems);
	}


void CTimeISTest::TestThree()
	{	
	iTest->Printf(_L("Testing Export\n"));
	// Create a contacts entry and export it via CntVCard
	iTest->Printf(_L("Creating the Contact and exporting it\n"));
	CContactIdArray* idArray = CContactIdArray::NewLC();
	TContactItemId contactID = CreateContactL();
	idArray->AddL(contactID);
	RFileWriteStream vCardWrite;
	vCardWrite.PushL();
	User::LeaveIfError(vCardWrite.Replace(iFs,KTestFile3_1,EFileWrite));
	iDb->ExportSelectedContactsL(TUid::Uid(KUidVCardConvDefaultImpl),*idArray,vCardWrite,(CContactVCardConverter::EImportSingleContact | CContactVCardConverter::EConnectWhitespace));
	vCardWrite.CommitL();
	CleanupStack::PopAndDestroy(); //vCardWrite
	CleanupStack::PopAndDestroy(idArray);
	// Verify that the card contains a telephone field and NOT a LABEL field.
	iTest->Printf(_L("Checking its contents\n"));
	const TUid KVersitTELPropertyValueType = { KVersitPropertyHBufCUid };
	const TUid KVersitLABELPropertyValueType = { KVersitPropertyHBufCUid };
	CParserVCard* parser = ParseVCardLC(KTestFile3_1);
	CArrayPtr<CParserProperty>* properties = parser->PropertyL(KVersitTokenTEL,KVersitTELPropertyValueType, EFalse);
	// If properties were found this will have a value
	(*iTest)(!(properties == NULL));
	delete properties;
	properties = parser->PropertyL(KVersitTokenLABEL,KVersitLABELPropertyValueType, EFalse);
	// If properties were found this will have a value
	(*iTest)(properties == NULL);
	CleanupStack::PopAndDestroy(parser);
	}

void CTimeISTest::TestFour()
	{
	// Create a badly formatted vCard.
	iTest->Printf(_L("Importing bad vCard to contacts\n"));
	CArrayPtr<CContactItem>* contactItems = ImportVCardLC(KTestFile4_1);
	
	(*iTest)(contactItems->Count() == 1);
	contactItems->ResetAndDestroy();
	CleanupStack::PopAndDestroy(contactItems);
	}
	
void CTimeISTest::TestFiveL()
	{
	// Create a vCard with actual telephone number, Home Work Preferred and default addresses.
	iTest->Printf(_L("Creating test vCard Files\n"));
	_LIT8(KNewN, "BEGIN:VCARD\r\nVERSION:2.1\r\nUID:3\r\nADR;HOME:;;;HLocal;HRegion;HPost;\r\nADR;WORK:;;;WLocal;WRegion;WPost;\r\nADR:;;;Local;Region;Post;\r\nADR;PREF:;;;PLocal;PRegion;PPost;\r\nEND:VCARD\r\n");
	CParserVCard* vCard = CreateVCardLC(KNewN);
	WriteVCardL(KTestFile5_1,*vCard);
	CleanupStack::PopAndDestroy(vCard);
	// Create a vCard with spaced telephone number, null address regions.
	_LIT8(KModN, "BEGIN:VCARD\r\nVERSION:2.1\r\nUID:3\r\nADR;HOME:;;;HLocal;;HPost;\r\nADR;WORK:;;;WLocal;;WPost;\r\nADR:;;;Local;;Post;\r\nADR;PREF:;;;PLocal;;PPost;\r\nEND:VCARD\r\n");
	vCard = CreateVCardLC(KModN);
	WriteVCardL(KTestFile5_2,*vCard);
	CleanupStack::PopAndDestroy(vCard);
	// Create a vCard with null telephone number, spaced address localities.
	_LIT8(KDelN, "BEGIN:VCARD\r\nVERSION:2.1\r\nUID:3\r\nADR;HOME:;;; ;;HPost;\r\nADR;WORK:;;; ;;WPost;\r\nADR:;;; ;;Post;\r\nADR;PREF:;;; ;;PPost;\r\nEND:VCARD\r\n");
	vCard = CreateVCardLC(KDelN);
	WriteVCardL(KTestFile5_3,*vCard);
	CleanupStack::PopAndDestroy(vCard);

	// Now import the files and check the behaviour
	iTest->Printf(_L("Importing vCard files to contacts -Test 5\n"));
	iTest->Printf(_L("New File -Test 5\n\r"));
	CArrayPtr<CContactItem>* contactItems = ImportVCardLC(KTestFile5_1);
	(*iTest)(contactItems->Count() == 1);
	CContactItem* cItem = iDb->ReadContactLC(contactItems->At(0)->Id());
	
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldLocality,KUidContactFieldVCardMapHOME,_L("HLocal")));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldLocality,KUidContactFieldVCardMapWORK,_L("WLocal")));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldLocality,KNullUid,_L("Local")));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldLocality,KUidContactFieldVCardMapPREF,_L("PLocal")));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldRegion,KUidContactFieldVCardMapHOME,_L("HRegion")));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldRegion,KUidContactFieldVCardMapWORK,_L("WRegion")));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldRegion,KNullUid,_L("Region")));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldRegion,KUidContactFieldVCardMapPREF,_L("PRegion")));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldPostcode,KUidContactFieldVCardMapHOME,_L("HPost")));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldPostcode,KUidContactFieldVCardMapWORK,_L("WPost")));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldPostcode,KNullUid,_L("Post")));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldPostcode,KUidContactFieldVCardMapPREF,_L("PPost")));
			
	contactItems->ResetAndDestroy();
	CleanupStack::PopAndDestroy(cItem);
	CleanupStack::PopAndDestroy(contactItems);

	iTest->Printf(_L("A non supported field -Test 5\n\r"));
	contactItems = ImportVCardLC(KTestFile5_2);
	(*iTest)(contactItems->Count() == 1);
	cItem = iDb->ReadContactLC(contactItems->At(0)->Id());
	
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldLocality,KUidContactFieldVCardMapHOME,_L("HLocal")));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldLocality,KUidContactFieldVCardMapWORK,_L("WLocal")));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldLocality,KNullUid,_L("Local")));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldLocality,KUidContactFieldVCardMapPREF,_L("PLocal")));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldRegion,KUidContactFieldVCardMapHOME,_L("HRegion")));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldRegion,KUidContactFieldVCardMapWORK,_L("WRegion")));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldRegion,KNullUid,_L("Region")));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldRegion,KUidContactFieldVCardMapPREF,_L("PRegion")));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldPostcode,KUidContactFieldVCardMapHOME,_L("HPost")));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldPostcode,KUidContactFieldVCardMapWORK,_L("WPost")));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldPostcode,KNullUid,_L("Post")));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldPostcode,KUidContactFieldVCardMapPREF,_L("PPost")));
		
	contactItems->ResetAndDestroy();
	CleanupStack::PopAndDestroy(cItem);
	CleanupStack::PopAndDestroy(contactItems);
	

	iTest->Printf(_L("Empty field -Test 5\n\r"));
	contactItems = ImportVCardLC(KTestFile5_3);
	(*iTest)(contactItems->Count() == 1);
	cItem = iDb->ReadContactLC(contactItems->At(0)->Id());
	
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldLocality,KUidContactFieldVCardMapHOME,_L("")));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldLocality,KUidContactFieldVCardMapWORK,_L("")));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldLocality,KNullUid,_L(""), EFalse));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldLocality,KUidContactFieldVCardMapPREF,_L(""), EFalse));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldRegion,KUidContactFieldVCardMapHOME,_L("HRegion")));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldRegion,KUidContactFieldVCardMapWORK,_L("WRegion")));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldRegion,KNullUid,_L("Region")));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldRegion,KUidContactFieldVCardMapPREF,_L("PRegion")));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldPostcode,KUidContactFieldVCardMapHOME,_L("HPost")));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldPostcode,KUidContactFieldVCardMapWORK,_L("WPost")));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldPostcode,KNullUid,_L("Post")));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldPostcode,KUidContactFieldVCardMapPREF,_L("PPost")));
		
	contactItems->ResetAndDestroy();
	CleanupStack::PopAndDestroy(cItem);
	CleanupStack::PopAndDestroy(contactItems);
	}

	
void CTimeISTest::TestSixL()
	{
	// Create a vCard with actual telephone number, Home Work Preferred and default addresses.
	iTest->Printf(_L("Creating test vCard Files\n"));
	_LIT8(KNewN, "BEGIN:VCARD\r\nVERSION:2.1\r\nUID:5\r\nADR;HOME:;;;HLocal;HRegion;HPost;\r\nEND:VCARD\r\n");
	CParserVCard* vCard = CreateVCardLC(KNewN);
	WriteVCardL(KTestFile6_1,*vCard);
	CleanupStack::PopAndDestroy(vCard);
	// Create a vCard with spaced telephone number, null address regions.
	_LIT8(KModN, "BEGIN:VCARD\r\nVERSION:2.1\r\nUID:5\r\nADR;WORK:;;;WLocal;WRegion;WPost;\r\nEND:VCARD\r\n");
	vCard = CreateVCardLC(KModN);
	WriteVCardL(KTestFile6_2,*vCard);
	CleanupStack::PopAndDestroy(vCard);
	
	// Now import the files and check the behaviour
	iTest->Printf(_L("Importing vCard files to contacts -Test 6\n"));
	iTest->Printf(_L("New File -Test 6\n\r"));
	CArrayPtr<CContactItem>* contactItems = ImportVCardLC(KTestFile6_1);
	(*iTest)(contactItems->Count() == 1);
	CContactItem* cItem = iDb->ReadContactLC(contactItems->At(0)->Id());
	
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldLocality,KUidContactFieldVCardMapHOME,_L("HLocal")));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldRegion,KUidContactFieldVCardMapHOME,_L("HRegion")));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldPostcode,KUidContactFieldVCardMapHOME,_L("HPost")));
			
	contactItems->ResetAndDestroy();
	CleanupStack::PopAndDestroy(cItem);
	CleanupStack::PopAndDestroy(contactItems);

	iTest->Printf(_L("A non supported field -Test 6\n\r"));
	contactItems = ImportVCardLC(KTestFile6_2);
	(*iTest)(contactItems->Count() == 1);
	cItem = iDb->ReadContactLC(contactItems->At(0)->Id());
	
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldLocality,KUidContactFieldVCardMapHOME,_L("HLocal")));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldRegion,KUidContactFieldVCardMapHOME,_L("HRegion")));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldPostcode,KUidContactFieldVCardMapHOME,_L("HPost")));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldLocality,KUidContactFieldVCardMapWORK,_L("WLocal")));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldRegion,KUidContactFieldVCardMapWORK,_L("WRegion")));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldPostcode,KUidContactFieldVCardMapWORK,_L("WPost")));
		
	contactItems->ResetAndDestroy();
	CleanupStack::PopAndDestroy(cItem);
	CleanupStack::PopAndDestroy(contactItems);
	}
	
void CTimeISTest::TestSevenL()
	{
	// Create a vCard with actual telephone number, Home Work Preferred and default addresses.
	iTest->Printf(_L("Creating test vCard Files -Test 7\n"));
	_LIT8(KNewN, "BEGIN:VCARD\r\nVERSION:2.1\r\nUID:7\r\nN:A;B;C\r\nEND:VCARD\r\n");
	CParserVCard* vCard = CreateVCardLC(KNewN);
	WriteVCardL(KTestFile7_1,*vCard);
	CleanupStack::PopAndDestroy(vCard);
	// Create a vCard with spaced telephone number, null address regions.
	_LIT8(KModN, "BEGIN:VCARD\r\nVERSION:2.1\r\nUID:7\r\nN:A;;C\r\nTEL:1\r\nEND:VCARD\r\n");
	vCard = CreateVCardLC(KModN);
	WriteVCardL(KTestFile7_2,*vCard);
	CleanupStack::PopAndDestroy(vCard);
	// Create a vCard with null telephone number, spaced address localities.
	_LIT8(KDelN, "BEGIN:VCARD\r\nVERSION:2.1\r\nUID:7\r\nN: ;;C\r\nTEL:2\r\nEND:VCARD\r\n");
	vCard = CreateVCardLC(KDelN);
	WriteVCardL(KTestFile7_3,*vCard);
	CleanupStack::PopAndDestroy(vCard);

	// Now import the files and check the behaviour
	iTest->Printf(_L("Importing vCard files to contacts -Test 7\n"));
	iTest->Printf(_L("New File -Test 7\n"));
	CArrayPtr<CContactItem>* contactItems = ImportVCardLC(KTestFile7_1, 0);
	(*iTest)(contactItems->Count() == 1);
	CContactItem* cItem = iDb->ReadContactLC(contactItems->At(0)->Id());
	
	(*iTest)(CheckSingleFieldValue(cItem->CardFields(),KUidContactFieldFamilyName,_L("A")));
	(*iTest)(CheckSingleFieldValue(cItem->CardFields(),KUidContactFieldGivenName,_L("B")));
	(*iTest)(CheckSingleFieldValue(cItem->CardFields(),KUidContactFieldAdditionalName,_L("C")));
	
	contactItems->ResetAndDestroy();
	CleanupStack::PopAndDestroy(cItem);
	CleanupStack::PopAndDestroy(contactItems);

		
	iTest->Printf(_L("A non supported field -Test 7\n"));
	contactItems = ImportVCardLC(KTestFile7_2, 0);
	(*iTest)(contactItems->Count() == 1);
	cItem = iDb->ReadContactLC(contactItems->At(0)->Id());
	
	(*iTest)(CheckSingleFieldValue(cItem->CardFields(),KUidContactFieldFamilyName,_L("A")));
	(*iTest)(CheckSingleFieldValue(cItem->CardFields(),KUidContactFieldGivenName,_L("")));
	(*iTest)(CheckSingleFieldValue(cItem->CardFields(),KUidContactFieldAdditionalName,_L("C")));
	contactItems->ResetAndDestroy();
	CleanupStack::PopAndDestroy(cItem);
	CleanupStack::PopAndDestroy(contactItems);
	

	iTest->Printf(_L("Empty field -Test 7\n"));
	contactItems = ImportVCardLC(KTestFile7_3, 0);
	(*iTest)(contactItems->Count() == 1);
	cItem = iDb->ReadContactLC(contactItems->At(0)->Id());
	
	(*iTest)(CheckSingleFieldValue(cItem->CardFields(),KUidContactFieldFamilyName,_L(" ")));
	(*iTest)(CheckSingleFieldValue(cItem->CardFields(),KUidContactFieldGivenName,_L("")));
	(*iTest)(CheckSingleFieldValue(cItem->CardFields(),KUidContactFieldAdditionalName,_L("C")));
	contactItems->ResetAndDestroy();
	CleanupStack::PopAndDestroy(cItem);
	CleanupStack::PopAndDestroy(contactItems);
	}


void CTimeISTest::TestEightL()
	{
	// Create a vCard with actual telephone number, Home Work Preferred and default addresses.
	iTest->Printf(_L("Creating test vCard Files - Test 8\n"));
	_LIT8(KNewN, "BEGIN:VCARD\r\nVERSION:2.1\r\nUID:8\r\nADR;HOME:;;;HLocal;HRegion;HPost;\r\nADR;WORK:;;;WLocal;WRegion;WPost;\r\nADR:;;;Local;Region;Post;\r\nADR;PREF:;;;PLocal;PRegion;PPost;\r\nEND:VCARD\r\n");
	CParserVCard* vCard = CreateVCardLC(KNewN);
	WriteVCardL(KTestFile8_1,*vCard);
	CleanupStack::PopAndDestroy(vCard);
	// Create a vCard with spaced telephone number, null address regions.
	_LIT8(KModN, "BEGIN:VCARD\r\nVERSION:2.1\r\nUID:8\r\nADR;HOME:;;;HLocal;;HPost;\r\nADR;WORK:;;;WLocal;;WPost;\r\nADR:;;;Local;;Post;\r\nADR;PREF:;;;PLocal;;PPost;\r\nEND:VCARD\r\n");
	vCard = CreateVCardLC(KModN);
	WriteVCardL(KTestFile8_2,*vCard);
	CleanupStack::PopAndDestroy(vCard);
	// Create a vCard with null telephone number, spaced address localities.
	_LIT8(KDelN, "BEGIN:VCARD\r\nVERSION:2.1\r\nUID:8\r\nADR;HOME:;;; ;;HPost;\r\nADR;WORK:;;; ;;WPost;\r\nADR:;;; ;;Post;\r\nADR;PREF:;;; ;;PPost;\r\nEND:VCARD\r\n");
	vCard = CreateVCardLC(KDelN);
	WriteVCardL(KTestFile8_3,*vCard);
	CleanupStack::PopAndDestroy(vCard);

	// Now import the files and check the behaviour
	iTest->Printf(_L("Importing vCard files to contacts -Test 8\n"));
	iTest->Printf(_L("New File -Test 8\n\r"));
	CArrayPtr<CContactItem>* contactItems = ImportVCardLC(KTestFile8_1, 0);
	(*iTest)(contactItems->Count() == 1);
	CContactItem* cItem = iDb->ReadContactLC(contactItems->At(0)->Id());
	
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldLocality,KUidContactFieldVCardMapHOME,_L("HLocal")));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldLocality,KUidContactFieldVCardMapWORK,_L("WLocal")));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldLocality,KNullUid,_L("Local")));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldLocality,KUidContactFieldVCardMapPREF,_L("PLocal")));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldRegion,KUidContactFieldVCardMapHOME,_L("HRegion")));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldRegion,KUidContactFieldVCardMapWORK,_L("WRegion")));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldRegion,KNullUid,_L("Region")));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldRegion,KUidContactFieldVCardMapPREF,_L("PRegion")));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldPostcode,KUidContactFieldVCardMapHOME,_L("HPost")));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldPostcode,KUidContactFieldVCardMapWORK,_L("WPost")));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldPostcode,KNullUid,_L("Post")));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldPostcode,KUidContactFieldVCardMapPREF,_L("PPost")));
			
	contactItems->ResetAndDestroy();
	CleanupStack::PopAndDestroy(cItem);
	CleanupStack::PopAndDestroy(contactItems);

	iTest->Printf(_L("A non supported field -Test 8\n\r"));
	contactItems = ImportVCardLC(KTestFile8_2, 0);
	(*iTest)(contactItems->Count() == 1);
	cItem = iDb->ReadContactLC(contactItems->At(0)->Id());
	
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldLocality,KUidContactFieldVCardMapHOME,_L("HLocal")));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldLocality,KUidContactFieldVCardMapWORK,_L("WLocal")));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldLocality,KNullUid,_L("Local")));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldLocality,KUidContactFieldVCardMapPREF,_L("PLocal")));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldRegion,KUidContactFieldVCardMapHOME,_L("")));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldRegion,KUidContactFieldVCardMapWORK,_L("")));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldRegion,KNullUid,_L(""), EFalse));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldRegion,KUidContactFieldVCardMapPREF,_L(""), EFalse));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldPostcode,KUidContactFieldVCardMapHOME,_L("HPost")));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldPostcode,KUidContactFieldVCardMapWORK,_L("WPost")));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldPostcode,KNullUid,_L("Post")));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldPostcode,KUidContactFieldVCardMapPREF,_L("PPost")));
		
	contactItems->ResetAndDestroy();
	CleanupStack::PopAndDestroy(cItem);
	CleanupStack::PopAndDestroy(contactItems);
	

	iTest->Printf(_L("Empty field -Test 8\n\r"));
	contactItems = ImportVCardLC(KTestFile8_3, 0);
	(*iTest)(contactItems->Count() == 1);
	cItem = iDb->ReadContactLC(contactItems->At(0)->Id());
	
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldLocality,KUidContactFieldVCardMapHOME,_L(" ")));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldLocality,KUidContactFieldVCardMapWORK,_L(" ")));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldLocality,KNullUid,_L(" ")));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldLocality,KUidContactFieldVCardMapPREF,_L(" ")));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldRegion,KUidContactFieldVCardMapHOME,_L("")));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldRegion,KUidContactFieldVCardMapWORK,_L("")));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldRegion,KNullUid,_L(""), EFalse));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldRegion,KUidContactFieldVCardMapPREF,_L(""), EFalse));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldPostcode,KUidContactFieldVCardMapHOME,_L("HPost")));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldPostcode,KUidContactFieldVCardMapWORK,_L("WPost")));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldPostcode,KNullUid,_L("Post")));
	(*iTest)(CheckSingleParameterFieldValues(cItem->CardFields(),KUidContactFieldPostcode,KUidContactFieldVCardMapPREF,_L("PPost")));
		
	contactItems->ResetAndDestroy();
	CleanupStack::PopAndDestroy(cItem);
	CleanupStack::PopAndDestroy(contactItems);
	}


void CTimeISTest::TestBirthdayDeletionL(TBool aAddNameField)
/**
 *Testing the deletion of a birthday field in a contact card through the time IS protocol
 *@param TBool aAddNameField specify whether or not adding a name field to the contact card
 */
	{
	TDateTime date(1976,EAugust,28,22,0,0,0);
	TTime time(date);	

	//Create a contact with just a birthday date
	const TContactItemId templateId = iDb->TemplateId();
	CContactItem* templateCard = iDb->ReadContactLC(templateId);
	CContactCard* card = CContactCard::NewL(templateCard); 
	CleanupStack::PushL(card);
	//
	CContactItemFieldSet& cardFields=card->CardFields();
	// Set the birthday
	TInt pos=cardFields.Find(KUidContactFieldBirthday);
	if (pos!=KErrNotFound)
		{
		cardFields[pos].SetMapping(KUidContactFieldVCardMapBDAY);
		cardFields[pos].DateTimeStorage()->SetTime(date);
		}
	
	if (aAddNameField)
		{
		pos=cardFields.Find(KUidContactFieldFamilyName);
		if (pos!=KErrNotFound)
			{
			cardFields[pos].TextStorage()->SetTextL(_L("MyName"));
			}
		}

	//Create contact now
	const TContactItemId id = iDb->AddNewContactL(*card);
	CleanupStack::PopAndDestroy(2, templateCard); 
	CContactIdArray* idArray = CContactIdArray::NewLC();
	idArray->AddL(id);
	
	//export contact to buf
	TBuf8<256> buf;//256 bytes should be ok
	RDesWriteStream vCardWriteStream(buf);
	vCardWriteStream.PushL();
	iDb->ExportSelectedContactsL(TUid::Uid(KUidVCardConvDefaultImpl),*idArray,vCardWriteStream,CContactVCardConverter::EConnectWhitespace);
	vCardWriteStream.CommitL();
	CleanupStack::PopAndDestroy(&vCardWriteStream);
	CleanupStack::PopAndDestroy(idArray); 

	//modify the exported vcard to remove birthday date and add a white space
	pos=buf.Find(KVersitTokenBDAY);
	(*iTest)(pos>0);
	pos+=5; //jump over the token and colon	
	_LIT8(KEndOfLine,"\n");
	TInt charToDelete=buf.Mid(pos).Find(KEndOfLine); //search for the next 0A
	charToDelete--; // don't delete the 0A
	buf.Delete(pos,charToDelete); //delete now
	//Now add white space in vcard
	_LIT8(KWhiteSpace," ");
	buf.Insert(pos,KWhiteSpace); //insert now
	
	//now import back that vcard it should not change user data
	CArrayPtr<CContactItem>* contactItems;
	RDesReadStream vCardReadStream(buf);
	TBool importOK;
	vCardReadStream.PushL();
	contactItems = iDb->ImportContactsL(TUid::Uid(KUidVCardConvDefaultImpl),vCardReadStream,importOK,CContactVCardConverter::EConnectWhitespace);
	CleanupStack::PopAndDestroy(); // vCardReadStream
	CleanupStack::PushL(contactItems);
	CleanupResetAndDestroyPushL(*contactItems);
	//make sure contact is imported 
	(*iTest)(contactItems->Count()==1); 

	TInt fieldPos=contactItems->At(0)->CardFields().Find(KUidContactFieldBirthday);
	//make sure field has not been deleted 
	(*iTest)(fieldPos!=KErrNotFound); 
	//make sure birthday date as not changed
	(*iTest)(contactItems->At(0)->CardFields()[fieldPos].DateTimeStorage()->Time()==time);
		
	CleanupStack::PopAndDestroy(2,contactItems); // ResetAndDestroy
	
	//Now try to delete that field through the symbian connect timeIS protocol by importing a NULL field
	//First delete that space in vcard
	buf.Delete(pos,1); 
	//now import back that vcard it should delete the birthday field
	vCardReadStream.PushL();
	vCardReadStream.Open(buf);
	contactItems = iDb->ImportContactsL(TUid::Uid(KUidVCardConvDefaultImpl),vCardReadStream,importOK,CContactVCardConverter::EConnectWhitespace);
	CleanupStack::PopAndDestroy(); // vCardReadStream
	CleanupStack::PushL(contactItems);
	CleanupResetAndDestroyPushL(*contactItems);
	(*iTest)(contactItems->Count()==1); //make sure one contact is imported
	
	//Make sure birthday has been deleted
	/*TInt*/ fieldPos=contactItems->At(0)->CardFields().Find(KUidContactFieldBirthday);
	(*iTest)(fieldPos==KErrNotFound); //make sure field has been deleted 
		
	CleanupStack::PopAndDestroy(2,contactItems); // ResetAndDestroy
	}

void CTimeISTest::RunTestsL()
	{
	iTest->Printf(_L("Time IS Tests Starting\n"));
	
	TestBirthdayDeletionL(ETrue);
		
	TestOneL();
	TestTwoL();
	TestThree();
	TestFour();
	TestFiveL();
	TestSixL();
	TestSevenL();
	TestEightL();
	}


void DoTestsL()
	{
	CTimeISTest* theTest = CTimeISTest::NewLC(test);

	CTestRegister * TempFiles = CTestRegister::NewLC();

	TempFiles->RegisterL(KDatabaseName, EFileTypeCnt);
	TempFiles->RegisterL(KTestFile1_1);
	TempFiles->RegisterL(KTestFile1_2);
	TempFiles->RegisterL(KTestFile1_3);
	TempFiles->RegisterL(KTestFile3_1);
	TempFiles->RegisterL(KTestFile5_1);
	TempFiles->RegisterL(KTestFile5_2);
	TempFiles->RegisterL(KTestFile5_3);
	TempFiles->RegisterL(KTestFile6_1);
	TempFiles->RegisterL(KTestFile6_2);
	TempFiles->RegisterL(KTestFile7_1);
	TempFiles->RegisterL(KTestFile7_2);
	TempFiles->RegisterL(KTestFile7_3);
	TempFiles->RegisterL(KTestFile8_1);
	TempFiles->RegisterL(KTestFile8_2);
	TempFiles->RegisterL(KTestFile8_3);

	theTest->RunTestsL();

	theTest->CloseDb();
	CleanupStack::PopAndDestroy(2, theTest);
	} //DoTestsL


/**

@SYMTestCaseID     PIM-T-TIMEIS-0001

*/

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	test.Title();
	test.Start(_L("@SYMTESTCaseID:PIM-T-TIMEIS-0001 CNTVCard - Check TimeIS import / export support"));

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

 
