/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* PREQ 1775 - REQ 8135 and REQ 8136
* Test harness for testing the import and export behaviour for the vCard TEL and EMAIL properties.
* 1) Check that data can be added programatically
* 2) Check that the exported format is as expected
* 3) Check that vCard can be imported and the data is as expected.
* EMail property TYPE parameter values MUST be supported
* TYPE value	    Semantic Description
* EMAIL;INTERNET	Default
* EMAIL;CELL	    MMS mobile email address
* EMAIL;HOME	    personal email address
* EMAIL;WORK	    business email address
* TEl property TYPE parameter values MUST be supported
* TYPE value	    Semantic Description
* TEL;VOICE	    personal wired phone number
* TEL;CELL	        personal mobile phone number
* TEL;WORK;VOICE	business wired phone number
* TEL;WORK;FAX	    Business fax number
* TEL;WORK;CELL	Business mobile phone number
* The TEL property supports the following parameter sequence
* TEL;["PREF;"]["WORK;"/"HOME;"]"VOICE;"/"FAX;"/"MSG;"/"CELL;"/PAGER;"/"BBS;"/"MODEM;"/"CAR;"/"ISDN;"/"VIDEO;"
* Include Files  
*
*/


#include <e32test.h>
#include <e32debug.h>
#include <e32cmn.h>
#include <f32file.h>
#include <s32file.h>
#include "t_utils.h"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "cntdb_internal.h"
#endif


//  Global Variables
LOCAL_D CCntTest* CntTest=NULL;
LOCAL_D const TPtrC KDatabaseFileName=_L("C:VTelEMailDb.cdb");
LOCAL_D RTest test(_L("T_VTel_EMail"));
LOCAL_D RFs fileSession;


//  Constants
_LIT(KVCardPath, "c:\\ExportTelEMail.vcf");
_LIT(KEMailAddress, "email1@symbian.com");
_LIT(KEMailAddress2, "email2@symbian.com");
_LIT(KEMailAddress3, "email3@symbian.com");
_LIT8(KEMailProperty, "EMAIL;");
_LIT8(KTELProperty, "TEL;");
_LIT8(KPrefParameter, "PREF;");
_LIT8(KTypeEqualParam, "TYPE=");
_LIT(KCellNumber, "07987563475692");
_LIT(KCellNumber2, "07987563472965");
_LIT(KCellNumber3, "07987555572965");
_LIT8(KSemicolon, ";");

//  Local Functions
void TestPrint(const TDesC8& aInfo)
	{
	//Print out the 8 bit aInfo by converting to 16 bit and then using RTest::Printf()
    TBuf<256> buffer16;//16 bit buffer
    buffer16.Copy(aInfo);//convert to 16 bit from a 8 bit
    TRefByValue<const TDesC> format(buffer16);
    test.Printf(format);
	}

void  AddNewTextFieldL(CContactItem& aContactItem, TUid aMapping, const RArray<TFieldType>& aFieldTypeArray, const TDesC& aText)
    {
    CContentType* contentType = CContentType::NewL();
    CleanupStack::PushL(contentType);
    contentType->SetMapping(aMapping);
    TInt fieldTypeCount = aFieldTypeArray.Count();
    for (TInt i = 0; i < fieldTypeCount; ++i)
    	{
    	contentType->AddFieldTypeL(aFieldTypeArray[i]);
    	}
    
    CContactItemField* contactItemField = CContactItemField::NewLC(KStorageTypeText, *contentType);
    contactItemField->TextStorage()->SetTextL(aText);
    
    aContactItem.AddFieldL(*contactItemField);
    
    CleanupStack::Pop(contactItemField);
    CleanupStack::PopAndDestroy(contentType);
    }

TContactItemId AddNewContactToDBL(const RArray<TFieldType>& aFieldTypeArray, TUid aMapping, const TDesC& aPropertyValue)
	{
	CContactItem* contactCard = CContactCard::NewLC();
	AddNewTextFieldL(*contactCard, aMapping, aFieldTypeArray, aPropertyValue);
    TContactItemId contactId = CntTest->Db()->AddNewContactL(*contactCard);
    CleanupStack::PopAndDestroy(contactCard); //contactCard
    return contactId;
	}

CArrayPtr<CContactItem>* ImportVCardL(const TDesC& aVCardPath, CContactDatabase* aContactDB)
	{
	TBool success=EFalse;
	RFileReadStream fileReadStream;
	CleanupClosePushL(fileReadStream);
	User::LeaveIfError(fileReadStream.Open(fileSession, aVCardPath, EFileRead));
	CArrayPtr<CContactItem>* contactItems = aContactDB->ImportContactsL(TUid::Uid(KUidVCardConvDefaultImpl),
            fileReadStream, success, CContactDatabase::EImportSingleContact+CContactDatabase::ETTFormat);
	CleanupStack::PopAndDestroy(&fileReadStream);
	test (success);
    return contactItems;
	}

void ExportVCardL(const TDesC& aVCardPath, CContactDatabase* aContactDB, const CContactIdArray& aContactIdArray, TVCardVersion aVer = EVCard21)
	{
	RFileWriteStream writeStream;
	CleanupClosePushL(writeStream);
    User::LeaveIfError(writeStream.Replace(fileSession, aVCardPath, EFileWrite));
    if (aVer == EVCard21)
    	{
        aContactDB->ExportSelectedContactsL(TUid::Uid(KUidVCardConvDefaultImpl), aContactIdArray, writeStream, 0);
    	}
    else
    	{
    	aContactDB->ExportSelectedContactsL(TUid::Uid(KUidPBAPVCardConvImpl), aContactIdArray, writeStream, CContactDatabase::EDefault, 0, NULL, aVer, ETrue);
    	}
    CleanupStack::PopAndDestroy(&writeStream);
	}

//Verify the vCard. Check that the pattern is present and the number of time the pattern should be present.
TBool CheckExportedFileL(const TDesC& aVCardPath, const TDesC8& aPattern, TInt aCount = 1)
	{
	TInt counter = 0;
	RFileReadStream fileReadStream;
	CleanupClosePushL(fileReadStream);
    User::LeaveIfError(fileReadStream.Open(fileSession, aVCardPath, EFileRead));
    
    TBuf8<0x80> bufO;
    TInt err = KErrNone;
    do {
	    TRAP(err, fileReadStream.ReadL(bufO, TChar(0xa)));
	    if (err != KErrNone && err != KErrEof)
		    {
		    User::Leave(err);	
		    }
	    if (bufO.Find(aPattern)!= KErrNotFound)
	    	{
	    	++counter;
	    	}
	    } while (err != KErrEof);
    CleanupStack::PopAndDestroy(&fileReadStream);
    return (counter == aCount);
	}

// Create and add a contact to the database 
// export the contact and parse it to check if relevant string is present.
//
TBool DoExportTestL(const RArray<TFieldType>& aFieldTypeArray, TUid aMapping, const TDesC8& aExpectedExportProperty, const TDesC& aPropertyValue, TVCardVersion aVer = EVCard21)
	{
    CntTest->CreateDatabaseL();
    //Create and add a contact to the database
    TContactItemId contactId = AddNewContactToDBL(aFieldTypeArray, aMapping, aPropertyValue);
    
    CContactIdArray* contactIdArray =  CContactIdArray::NewLC();
    contactIdArray->AddL(contactId);
    ExportVCardL(KVCardPath, CntTest->Db(), *contactIdArray, aVer);//Exporting the contact
    	
    CleanupStack::PopAndDestroy(contactIdArray);
    
    TBool success = CheckExportedFileL(KVCardPath, aExpectedExportProperty); //Check if the relevant string is present
    CntTest->CloseDatabase();
    return success;
	}

// Test the contact item has the correct contentType mapping and contains the correct fieldTypes
TBool TestImportedContactL(const RArray<TFieldType>& aFieldTypeArray, TUid aMapping, const CContactItem& aContactItem)
	{
	TBool result = EFalse;
    const CContactItemFieldSet& cardFieldSet  = aContactItem.CardFields();
	TInt fieldCount  = cardFieldSet.Count();
	TInt fieldTypeArrayCount = aFieldTypeArray.Count();
	
	for (TInt j =0; j < fieldCount; ++j)
		{
		const CContactItemField& contactItemField =  cardFieldSet[j];
		const CContentType& contentType = contactItemField.ContentType();
		TInt fieldTypeCount = contentType.FieldTypeCount();
		if (contentType.Mapping() == aMapping && fieldTypeCount == fieldTypeArrayCount)
			{
			for (TInt k=0; k < fieldTypeArrayCount; ++k)
				{
				TFieldType fieldType = aFieldTypeArray[k];
				if (aFieldTypeArray[k] != KUidContactFieldNone)
					{
					test (contentType.ContainsFieldType(aFieldTypeArray[k]));
					}
				}
			result = ETrue;
			break;
			}
		}
	return result;
	}

// Import the file and test the imported data
TBool DoImportTestL(const RArray<TFieldType>& aFieldTypeArray, TUid aMapping)
	{
	/* Reset the database 
	 * import file
	 * get the contact item and email field
	 * test for mapping and field types
	 */
	CntTest->DeleteDatabaseL();
	CntTest->CreateDatabaseL();
	CArrayPtr<CContactItem>* contactItems = ImportVCardL(KVCardPath, CntTest->Db());
	CleanupStack::PushL(TCleanupItem(CleanUpResetAndDestroy,contactItems));
	test (contactItems->Count() == 1);
	CContactItem* contactItem = contactItems->At(0);
	TBool success =  TestImportedContactL(aFieldTypeArray, aMapping, *contactItem);
	
	CleanupStack::PopAndDestroy();
	return success;
	}

void CheckMultiplePrefRuleL(const RArray<TFieldType>& aFieldTypeArray, TUid aMapping, const TDesC8& aProperty, const TDesC& aPropertyValue, TUint numOfInstance = 3)
	{
	//Test the the exported file for three properties
    TPtrC8 tptr = aProperty.Left(';');
    test (CheckExportedFileL(KVCardPath, tptr, numOfInstance));
    //import the vCard and test for contacts DB contents
    test (DoImportTestL(aFieldTypeArray, aMapping));
    
    //The last import/parser should have made sure there is only a single PREF in the contacts DB
    // Re export the file and check the file that there is a single instance, and it is the first property to verify this
    CContactIdArray* contactIdArray  = CntTest->Db()->UnfiledContactsL();
    CleanupStack::PushL(contactIdArray);
    if (contactIdArray)
    	{
    	test(contactIdArray->Count() == 1);
    	ExportVCardL(KVCardPath, CntTest->Db(), *contactIdArray);
    	}
    CleanupStack::PopAndDestroy(contactIdArray);
    CntTest->CloseDatabase();
    //Check there is only a single instance of aProperty, 
    test ( CheckExportedFileL(KVCardPath, aProperty, 1));
    TBuf8<256> propertyAndValue(aProperty);
    propertyAndValue.Append(aPropertyValue);
    TestPrint(propertyAndValue);//Check it is the instance of the property that we expect
    test ( CheckExportedFileL(KVCardPath, propertyAndValue, 1));
	}

/**
@SYMPREQ 1775
@SYMTestType CIT
@SYMTestPriority Critical
@SYMTestCaseDesc To check that the TEL property supports the following parameters and sequence
                 TEL;["PREF;"]["WORK;"/"HOME;"]"VOICE;"/"FAX;"/"MSG;"/"CELL;"/PAGER;"/"BBS;"/"MODEM;"/"CAR;"/"ISDN;"/"VIDEO;"
                 This tests OMA-MMS1.3-15 and OMA-MMS1.3-16 testcases from the test specification.
@SYMTestActions Create a contact item and export the item to a vCard, the exported item is verified.
                 The vCard is imported and the contact item is verified.

@SYMTestExpectedResults For the above tests:
The vCard exported should have the following line, if the parameter types are WORK and CELL. Other parameter types will give other results.

TEL;PREF;WORK;CELL:07987563475692

*/


void TestTelPropertyL(const TDesC8& aOptionalParameter, TFieldType aFieldType, const TDesC8& aParameterType, TBool aPrefPresent, TVCardVersion aVer = EVCard21)
	{
	test.Next(_L("Testing TEL property"));

	RArray<TFieldType> fieldTypeArray;
    CleanupClosePushL(fieldTypeArray);

    if (aFieldType == KUidContactFieldVCardMapFAX)
        {
    	fieldTypeArray.AppendL(KUidContactFieldFax);
        }
    else
        {
        fieldTypeArray.AppendL(KUidContactFieldPhoneNumber);	
        }
    
    TBuf8<256> exportPattern(KTELProperty); // This string is modified according the the parameter list, it is built up to what 
                                            //should appear in the vCard e.g. TEL;HOME;CELL;
    if (aOptionalParameter.Length())
        {
        if (aVer != EVCard21)
	    	{
	    	exportPattern.Append(KTypeEqualParam); //if vCard 3.0 then parameters names are TYPE i.e. TEL;TYPE=WORK;....
	    	}
        exportPattern.Append(aOptionalParameter);
        exportPattern.Append(KSemicolon());
        if (aOptionalParameter.Find(KVersitParam8Work()) != KErrNotFound)
            {
        	fieldTypeArray.AppendL(KUidContactFieldVCardMapWORK);
            }
        else if (aOptionalParameter.Find(KVersitParam8Home()) != KErrNotFound)
            {
        	fieldTypeArray.AppendL(KUidContactFieldVCardMapHOME);
            }
        }
    
    if (aPrefPresent)
	    {
	    if (aVer != EVCard21)
	    	{
	    	exportPattern.Insert(KTELProperty().Length(), KTypeEqualParam());//TEL;TYPE=.....
	    	exportPattern.Insert(KTELProperty().Length() + KTypeEqualParam().Length(), KPrefParameter());//TEL;TYPE=PREF;...
	    	}
	    else
	    	{
	        exportPattern.Insert(KTELProperty().Length(), KPrefParameter()); //TEL;PREF;....
	    	}
		fieldTypeArray.AppendL(KUidContactFieldVCardMapPREF);//PREF is added after HOME or WORK just to test the exporting PREF parameter rule. 
		                                                     //I.e. It should be the first parameter in the parameter list.
		}
    
    fieldTypeArray.AppendL(aFieldType);
    if (aVer != EVCard21)
    	{
    	exportPattern.Append(KTypeEqualParam);
    	}
    exportPattern.Append(aParameterType);
    
    //Print out the expected export string that should be present in the vCard
    TestPrint(exportPattern);
    
	test (DoExportTestL(fieldTypeArray, KUidContactFieldVCardMapTEL, exportPattern, KCellNumber(), aVer));
	test (DoImportTestL(fieldTypeArray, KUidContactFieldVCardMapTEL));
	
	CleanupStack::PopAndDestroy(&fieldTypeArray);
	}

/**
@SYMPREQ 1775
@SYMTestType CIT
@SYMTestPriority Critical
@SYMTestCaseDesc To check that multiple TEL properties can be supported. Multiple TEL properties with PREF parameters, when imported
                 will only store the first TEL property as the PREF'd one.
                 This function tests the export functionality and also the importing functionality.
                 This function provides the testing for OMA-MMS1.3-17 from the test specification
@SYMTestActions Create a contact item with multiple TEL property with PREF parameter types
                and export the item to a vCard, the exported item is verified.
                 The vCard is imported and the contact item is verified.

@SYMTestExpectedResults For the above tests:
The vCard exported should have the following lines.

TEL;PREF;WORK;CELL:07987563475692
TEL;PREF;WORK;CELL:07987563472965
TEL;PREF;WORK;CELL:07987555572965

Importing the vCard will have only the first EMAIL field with the PREF parameter

Exporting that contact item will have the following lines
TEL;PREF;WORK;CELL:07987563475692
TEL;WORK;CELL:07987563472965
TEL;WORK;CELL:07987555572965
*/


void TestMultipleTelPropertiesL()
	{
	test.Next(_L("Testing multiple TEL properties"));

	
    CntTest->CreateDatabaseL();
    
    RArray<TFieldType> fieldTypeArray;
    CleanupClosePushL(fieldTypeArray);
    fieldTypeArray.AppendL(KUidContactFieldPhoneNumber);
    fieldTypeArray.AppendL(KUidContactFieldVCardMapCELL);
    fieldTypeArray.AppendL(KUidContactFieldVCardMapWORK);
    fieldTypeArray.AppendL(KUidContactFieldVCardMapPREF);
    //Create a contact with multiple tel fields
    TContactItemId contactId = AddNewContactToDBL(fieldTypeArray, KUidContactFieldVCardMapTEL, KCellNumber());
    
    CContactItem* contactCard = CntTest->Db()->OpenContactL(contactId);
    CleanupStack::PushL(contactCard);
    //Add additional TEL fields with PREF
    AddNewTextFieldL(*contactCard, KUidContactFieldVCardMapTEL, fieldTypeArray, KCellNumber2());
    AddNewTextFieldL(*contactCard, KUidContactFieldVCardMapTEL, fieldTypeArray, KCellNumber3());
    
    CntTest->Db()->CommitContactL(*contactCard);
    CleanupStack::PopAndDestroy(contactCard);
    
    //export the contact
    CContactIdArray* contactIdArray =  CContactIdArray::NewLC();
    contactIdArray->AddL(contactId);
    ExportVCardL(KVCardPath, CntTest->Db(), *contactIdArray);
    	
    CleanupStack::PopAndDestroy(contactIdArray);
	CntTest->CloseDatabase();
	
	_LIT8(KTELPrefpattern, "TEL;PREF;WORK;CELL:");
	CheckMultiplePrefRuleL(fieldTypeArray, KUidContactFieldVCardMapTEL, KTELPrefpattern(), KCellNumber());
    CleanupStack::PopAndDestroy(&fieldTypeArray);
	}

/**
@SYMPREQ 1775
@SYMTestType CIT
@SYMTestPriority Critical
@SYMTestCaseDesc To check that the CELL/HOME/WORK parameter is supported with the EMAIL property without the INTERNET type.
                 This function tests the export functionality and also the importing functionality.
                 This function provides the testing for OMA-MMS1.3-1 to OMA-MMS1.3-12 from the test specification.
@SYMTestActions Create a contact item and export the item to a vCard, the exported item is verified.
                 The vCard is imported and the contact item is verified.

@SYMTestExpectedResults For the above tests:
The vCard exported should have the following line, if the test is for the CELL parameter type. Other parameter types will be 
substituted instead of CELL in the exported vCard.

EMAIL;CELL;ENCODING=QUOTED-PRINTABLE:email1@symbian.com and
EMAIL;PREF;CELL;ENCODING=QUOTED-PRINTABLE:email1@symbian.com if the PREF parameter is present
*/


void TestEMailPropertyL(TFieldType aFieldType, const TDesC8& aParameterType, TBool aPrefPresent, TVCardVersion aVer = EVCard21)
	{
	test.Next(_L("Email property test"));

	RArray<TFieldType> fieldTypeArray;
    CleanupClosePushL(fieldTypeArray);
    fieldTypeArray.AppendL(KUidContactFieldEMail);
    fieldTypeArray.AppendL(aFieldType);
    
    TBuf8<256> exportPattern(KEMailProperty);//Create exportPattern string to verify that it appears when exporting to a vCard
	
	if (aVer != EVCard21)
    	{
    	exportPattern.Append(KTypeEqualParam);//EMAIL;TYPE=...
    	}	
    exportPattern.Append(aParameterType);
    
    if (aPrefPresent)
	    {
	    if (aVer != EVCard21)
	    	{
	    	exportPattern.Insert(KEMailProperty().Length(), KTypeEqualParam());//EMAIL;TYPE=......
	    	exportPattern.Insert(KEMailProperty().Length() + KTypeEqualParam().Length(), KPrefParameter());//EMAIL;TYPE=PREF;......
	    	}
	    else
	    	{
	    	exportPattern.Insert(KEMailProperty().Length(), KPrefParameter());
	    	}
		fieldTypeArray.AppendL(KUidContactFieldVCardMapPREF);
		}
    
    TestPrint(exportPattern);//Print out the expected export string that should be present in the vCard
    
	test(DoExportTestL(fieldTypeArray, KUidContactFieldVCardMapEMAILINTERNET, exportPattern, KEMailAddress(), aVer));
	test(DoImportTestL(fieldTypeArray, KUidContactFieldVCardMapEMAILINTERNET));

	CleanupStack::PopAndDestroy(&fieldTypeArray);
	}

/**
@SYMPREQ 1775
@SYMTestType CIT
@SYMTestPriority Critical
@SYMTestCaseDesc Check the vCard with EMAIL property and CELL  parameter as well as an INTERNET parameter can be imported. 
                 This is a test for a incorrectly  formatted EMAIL property..
                 This function tests the export functionality and also the importing functionality.
                 This function provides the testing for OMA-MMS1.3-13 from the test specification
@SYMTestActions Create a contact item and export the item to a vCard, the exported item is verified.
                 The vCard is imported and the contact item is verified.

@SYMTestExpectedResults For the above tests:
The vCard exported should have the following line, if the test is for the CELL parameter type. Other parameter types will be 
substituted instead of CELL in the exported vCard.

EMAIL;CELL;INTERNET;ENCODING=QUOTED-PRINTABLE:email1@symbian.com and
*/

void TestWrongFormatEmail()
	{
	test.Next(_L("Email - Wrong format"));

	RArray<TFieldType> fieldTypeArray;
    CleanupClosePushL(fieldTypeArray);
    fieldTypeArray.AppendL(KUidContactFieldEMail);
    fieldTypeArray.AppendL(KUidContactFieldVCardMapCELL);
    fieldTypeArray.AppendL(KUidContactFieldVCardMapINTERNET);
    
    _LIT8(KWrongFormat, "EMAIL;CELL;INTERNET;");
	TBuf8<256> exportPattern(KWrongFormat);//Create exportPattern string to verify that it appears when exporting to a vCard
    TestPrint(exportPattern);//Print out the expected export string that should be present in the vCard
    
	test (DoExportTestL(fieldTypeArray, KUidContactFieldVCardMapEMAILINTERNET, exportPattern, KEMailAddress()));
	test (DoImportTestL(fieldTypeArray, KUidContactFieldVCardMapEMAILINTERNET));
	
	CleanupStack::PopAndDestroy(&fieldTypeArray);
	}

/**
@SYMPREQ 1775
@SYMTestType CIT
@SYMTestPriority Critical
@SYMTestCaseDesc To check that multiple email properties can be supported. Multiple email properties with PREF parameters, when imported
                 will only store the first EMAIL property as the PREF'd one.
                 This function tests the export functionality and also the importing functionality.
                 This function provides the testing for OMA-MMS1.3-14  from the test specification
@SYMTestActions Create a contact item with multiple EMAIL property with PREF parameter types
                and export the item to a vCard, the exported item is verified.
                 The vCard is imported and the contact item is verified.

@SYMTestExpectedResults For the above tests:
The vCard exported should have the following lines.

EMAIL;PREF;CELL;ENCODING=QUOTED-PRINTABLE:email1@symbian.com
EMAIL;PREF;CELL;ENCODING=QUOTED-PRINTABLE:email2@symbian.com
EMAIL;PREF;CELL;ENCODING=QUOTED-PRINTABLE:email3@symbian.com

Importing the vCard will have only the first EMAIL field with the PREF parameter

Exporting that contact item will have the following lines
EMAIL;PREF;CELL;ENCODING=QUOTED-PRINTABLE:email1@symbian.com
EMAIL;CELL;ENCODING=QUOTED-PRINTABLE:email2@symbian.com
EMAIL;CELL;ENCODING=QUOTED-PRINTABLE:email3@symbian.com
*/


void TestMultipleEMailPropertiesL()
	{
	test.Next(_L("Testing multiple EMail properties"));

	//Create a contact with multiple email fields
    CntTest->CreateDatabaseL();
    
    RArray<TFieldType> fieldTypeArray;
    CleanupClosePushL(fieldTypeArray);
    fieldTypeArray.AppendL(KUidContactFieldEMail);
    fieldTypeArray.AppendL(KUidContactFieldVCardMapCELL);
    fieldTypeArray.AppendL(KUidContactFieldVCardMapPREF);
    
    TContactItemId contactId = AddNewContactToDBL(fieldTypeArray, KUidContactFieldVCardMapEMAILINTERNET, KEMailAddress());
    CContactItem* contactCard = CntTest->Db()->OpenContactL(contactId);
    CleanupStack::PushL(contactCard);
    //Add additional email fields with PREF.
    AddNewTextFieldL(*contactCard, KUidContactFieldVCardMapEMAILINTERNET, fieldTypeArray, KEMailAddress2());
    AddNewTextFieldL(*contactCard, KUidContactFieldVCardMapEMAILINTERNET, fieldTypeArray, KEMailAddress3());
    CntTest->Db()->CommitContactL(*contactCard);
    CleanupStack::PopAndDestroy(contactCard);
    
    CContactIdArray* contactIdArray =  CContactIdArray::NewLC();
    contactIdArray->AddL(contactId);
    ExportVCardL(KVCardPath, CntTest->Db(), *contactIdArray);
    	
    CleanupStack::PopAndDestroy(contactIdArray);
	CntTest->CloseDatabase();
	
	_LIT8(KEmailPrefPattern, "EMAIL;PREF;CELL;ENCODING=QUOTED-PRINTABLE:");
	CheckMultiplePrefRuleL(fieldTypeArray, KUidContactFieldVCardMapEMAILINTERNET, KEmailPrefPattern(), KEMailAddress().Left(KEMailAddress().Locate('@')));
	
    CleanupStack::PopAndDestroy(&fieldTypeArray);
	}

/**

@SYMTestCaseID     PIM-T-VTEL-EMAIL-0001

*/

void RunTestsL()
	{
	CleanupClosePushL(fileSession);
    User::LeaveIfError(fileSession.Connect());
    test.Start(_L("@SYMTESTCaseID:PIM-T-VTEL-EMAIL-0001 Test the vCard TEL and EMail properties"));

    //  Email property tests
    
    // CELL parameter
    test.Start(_L("Starting EMail property tests"));

	TestEMailPropertyL(KUidContactFieldVCardMapCELL, KVersitParam8Cell(), EFalse);
	TestEMailPropertyL(KUidContactFieldVCardMapCELL, KVersitParam8Cell(), ETrue);
	// WORK parameter
	TestEMailPropertyL(KUidContactFieldVCardMapWORK, KVersitParam8Work(), EFalse);
	TestEMailPropertyL(KUidContactFieldVCardMapWORK, KVersitParam8Work(), ETrue);
	// HOME parameter
	TestEMailPropertyL(KUidContactFieldVCardMapHOME, KVersitParam8Home(), EFalse);
	TestEMailPropertyL(KUidContactFieldVCardMapHOME, KVersitParam8Home(), ETrue);
	// INTERNET parameter
	TestEMailPropertyL(KUidContactFieldVCardMapINTERNET, KVersitParam8Internet(), EFalse);
	TestEMailPropertyL(KUidContactFieldVCardMapINTERNET, KVersitParam8Internet(), ETrue);
    // default INTERNET parameter test
	TestEMailPropertyL(KUidContactFieldNone, KVersitParam8Internet(), EFalse);
	TestEMailPropertyL(KUidContactFieldNone, KVersitParam8Internet(), ETrue);
	// vCard 3.0 format test
	TestEMailPropertyL(KUidContactFieldVCardMapCELL, KVersitParam8Cell(), ETrue, EPBAPVCard30);
	TestEMailPropertyL(KUidContactFieldVCardMapCELL, KVersitParam8Cell(), EFalse, EPBAPVCard30);
	//Test for unexpected parameter list format
    TestWrongFormatEmail();
    
	test.End(); // End of email property tests

	test.Start(_L("Starting TEL property tests"));

	TestMultipleEMailPropertiesL();
    //	Telephone property tests
	//TEL;WORK;CELL test
    TestTelPropertyL(KVersitParam8Work(), KUidContactFieldVCardMapCELL, KVersitParam8Cell(), EFalse);
    TestTelPropertyL(KVersitParam8Work(), KUidContactFieldVCardMapCELL, KVersitParam8Cell(), ETrue);
    //TEL;WORK;VOICE test
	TestTelPropertyL(KVersitParam8Work(), KUidContactFieldVCardMapVOICE, KVersitParam8Voice(), EFalse);
    TestTelPropertyL(KVersitParam8Work(), KUidContactFieldVCardMapVOICE, KVersitParam8Voice(), ETrue);
    //TEL;WORK;FAX test
    TestTelPropertyL(KVersitParam8Work(), KUidContactFieldVCardMapFAX, KVersitParam8Fax(), EFalse);
    TestTelPropertyL(KVersitParam8Work(), KUidContactFieldVCardMapFAX, KVersitParam8Fax(), ETrue);
    //TEL;HOME;VOICE test
    TestTelPropertyL(KVersitParam8Home(), KUidContactFieldVCardMapVOICE, KVersitParam8Voice(), EFalse);
    TestTelPropertyL(KVersitParam8Home(), KUidContactFieldVCardMapVOICE, KVersitParam8Voice(), ETrue);
    //TEL;HOME;CELL test
    TestTelPropertyL(KVersitParam8Home(), KUidContactFieldVCardMapCELL, KVersitParam8Cell(), EFalse);
    TestTelPropertyL(KVersitParam8Home(), KUidContactFieldVCardMapCELL, KVersitParam8Cell(), ETrue);
    //TEL;CELL
	TestTelPropertyL(KNullDesC8(), KUidContactFieldVCardMapCELL, KVersitParam8Cell(), EFalse);
    TestTelPropertyL(KNullDesC8(), KUidContactFieldVCardMapCELL, KVersitParam8Cell(), ETrue);
    //TEL;VOICE
    TestTelPropertyL(KNullDesC8(), KUidContactFieldVCardMapVOICE, KVersitParam8Voice(), EFalse);
    TestTelPropertyL(KNullDesC8(), KUidContactFieldVCardMapVOICE, KVersitParam8Voice(), ETrue);
    //TEL;TYPE=WORK;TYPE=CELL vCard 3.0 test
    TestTelPropertyL(KVersitParam8Work(), KUidContactFieldVCardMapCELL, KVersitParam8Cell(), EFalse, EPBAPVCard30);
    TestTelPropertyL(KVersitParam8Work(), KUidContactFieldVCardMapCELL, KVersitParam8Cell(), ETrue, EPBAPVCard30);
    //Test multiple pref rule
    TestMultipleTelPropertiesL();
    test.End(); // end of tel property tests
    
    fileSession.Delete(KVCardPath()); 
	CleanupStack::PopAndDestroy(&fileSession); // fileSession
	}

//  Global Functions

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CntTest = new CCntTest;
	if (CntTest)
		{
		TRAPD(err, CntTest->ConstructL(test,KDatabaseFileName));
	    test(err==KErrNone);
	    // Run tests inside TRAP harness
	    TRAPD(mainError, RunTestsL());
	    test(mainError==KErrNone);
	     CntTest->EndTestLib(mainError);
		}
	
	__UHEAP_MARKEND;
	return 0;
	}

