/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Unit tests for functionality delivered by the CNTVCARD.DLL library.
* This test contains the tests originally in T_EXPDEL
*
*/


#include <e32test.h>
#include <f32file.h>
#include <s32file.h>
#include <s32mem.h>
#include <cntdb.h>
#include <cntvcard.h>
#include <cntitem.h>
#include <cntfldst.h>
#include <e32def.h>
#include "t_utils2.h"
#include "t_rndutils.h"
#include <coreappstest/testserver.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "cntdb_internal.h"
#endif

_LIT(KTestName,"T_CNTVCARD");



_LIT(KDatabaseFileName,"C:T_CNTVCARD");

_LIT(KVcard1, "Z:\\t_cntvcard\\email1.vcf");
_LIT(KVcard2, "Z:\\t_cntvcard\\email2.vcf");


_LIT(KOutputFileName, "C:\\Test_Output.txt");
_LIT(KExpDelAFileName, "c:\\expdela");
_LIT(KTestVcardFile, "Z:\\cntvcard\\testvcard.vcf");

_LIT(KImpTypeVcardName, "Z:\\t_cntvcard\\testTypeVcard.vcf");
_LIT(KImpNoTypeVcardName, "Z:\\t_cntvcard\\testNoTypeVcard.vcf");

_LIT(KTestVcardFile1, "Z:\\cntvcard\\testvcard1.vcf");
//UpdateVCardL
_LIT(KVCardUpdate1,"Z:\\System\\Programs\\CntmodelTest\\vcardupdate1.vcf");
_LIT(KVCardUpdate2,"C:\\vcardupdate2.vcf");
_LIT(KVCardUpdate3,"Z:\\System\\Programs\\CntmodelTest\\vcardupdate3.vcf");

// digram data files
_LIT(KFirstNameFileName,"z:\\cntvcard\\names.first.2gm");
_LIT(KLastNameFileName,"z:\\cntvcard\\names.last.2gm");

// vcard terminated by LF
_LIT(KVCardFileLF,"z:\\t_cntvcard\\cntvcardLF.vcf");
// vcard containing UID property
_LIT(KVCardFileUID,"z:\\t_cntvcard\\cntvcardUID.vcf");

_LIT8(KVCardBinaryKeyImportFileDes,"BEGIN:VCARD\r\n"
							"VERSION:2.1\r\n"
							"REV:20061120T111808Z\r\n"
							"UID:0000000000000000-00e1005eee8a27d8-44\r\n"
							"N:Name;;;;\r\n"
							"TEL;HOME;VOICE;CELL:1111\r\n"
							"KEY;ENCODING=BASE64:MIICajCCAdOgAwIBAgICBEUwDQYJKoZIhvcNAQEEBQA\r\n"
							"wdzELMAkGA1UEBhMCVVMxLDAqBgNVBAoTI05ldHNjYXBlIENbW11bmljYX\r\n"
							"Rpb25zIENvcnBvcmF0aW9uMRwwGgYDVQQLExNJbmZvcm1hdGlvbiBTeXN0\r\n"
							"ZW1zMRwwGgYDVQQDExNyb290Y2EubmV0c2NhcGUuY29tMB4XDTk3MDYwNj\r\n"
							"E5NDc1OVoXDTk3MTIwMzE5NDc1OVowgYkxCzAJBgNVBAYTAlVTMSYwJAYD\r\n"							 
							"VQQKEx1OZXRzY2FwZSBDb21tdW5pY2F0aW9ucyBDb3JwLjEYMBYGA1UEAx\r\n"
							"MPVGltb3RoeSBBIEhvd2VzMSEwHwYJKoZIhvcNAQkBFhJob3dlc0BuZXRz\r\n"
							"Y2FwZS5jb20xFTATBgoJkiaJk/IsZAEBEwVob3dlczBcMA0GCSqGSIb3DQ\r\n"
							"EBAQUAA0sAMEgCQQC0JZf6wkg8pLMXHHCUvMfL5H6zjSk4vTTXZpYyrdN2\r\n"
							"dXcoX49LKiOmgeJSzoiFKHtLOIboyludF90CgqcxtwKnAgMBAAGjNjA0MB\r\n"
							"EGCWCGSAGG+EIBAQQEAwIAoDAfBgNVHSMEGDAWgBT84FToB/GV3jr3mcau\r\n"
							"+hUMbsQukjANBgkqhkiG9w0BAQQFAAOBgQBexv7o7mi3PLXadkmNP9LcIP\r\n"
							"mx93HGp0Kgyx1jIVMyNgsemeAwBM+MSlhMfcpbTrONwNjZYW8vJDSoi//y\r\n"
							"rZlVt9bJbs7MNYZVsyF1unsqaln4/vy6Uawfg8VUMk1U7jt8LYpo4YULU7\r\n"
							"UZHPYVUaSgVttImOHZIKi4hlPXBOhcUQ==\r\n"
							"\r\n"
							"PHOTO;ENCODING=BASE64:\r\n"
							"   ZmlsZTovLy9qcXB1YmxpYy5naWY=\r\n"
							"\r\n"
							"END:VCARD\r\n"
							);
							
_LIT8(KVCardTextKeyImportFileDes,"BEGIN:VCARD\r\n"
							"VERSION:2.1\r\n"
							"REV:20061120T111808Z\r\n"
							"UID:0000000000000000-00e1005eee8a27d8-44\r\n"
							"N:Name;;;;\r\n"
							"TEL;HOME;VOICE;CELL:1111\r\n"
							"KEY:X509\r\n"
							"PHOTO;ENCODING=BASE64:\r\n"
							"   ZmlsZTovLy9qcXB1YmxpYy5naWY=\r\n"
							"\r\n"
							"END:VCARD\r\n"
							);

_LIT8(KMultiParam,"BEGIN:VCARD\r\n"
				"VERSION:2.1\r\n"
				"N:;Neo;;Mr.;\r\n"
				"FN:Mr. TestName\r\n"
				"TEL;TYPE=HOME;TYPE=VOICE;TYPE=CELL:123\r\n"
				"END:VCARD\r\n"
				);
_LIT8(KTelExport, "TEL;HOME;VOICE;CELL:123\r\n");

_LIT8(KVCardBeforeChange,"BEGIN:VCARD\r\n"
 						"VERSION:2.1\r\n"
 						"N:Jm;Jg;;;\r\n"
 						"TEL;HOME:11111\r\n"
 						"TEL;HOME:67890\r\n"
 						"TEL;CELL:12345\r\n"
 						"UID:4c2d2b19ef07adea-00e115956542fd28-1\r\n"
 						"END:VCARD\r\n"
 						);
 						
 _LIT8(KVCardAfterChange,"BEGIN:VCARD\r\n"
 						"VERSION:2.1\r\n"
 						"N:Jm;Jg;;;\r\n"
 						"TEL;HOME:11112\r\n"
 						"TEL;HOME:22222\r\n"
 						"TEL;HOME:22223\r\n"
 						"TEL;HOME:22224\r\n"
 						"UID:4c2d2b19ef07adea-00e115956542fd28-1\r\n"
 						"END:VCARD\r\n"
 						);				
 
 _LIT(KGivenName, "Jg");
 _LIT(KFamilyName, "Jm");
 _LIT(KTelHome1,"11111");
 _LIT(KTelHome2,"67890");
 _LIT(KTelCell,"12345");
 _LIT(KTelHome1Modified,"11112");
 _LIT(KTelHome2Modified,"22222");
 _LIT(KTelHome3Modified,"22223");
 _LIT(KTelHome4Modified,"22224");

//vCard containing CompanyField
_LIT(KVCardWithCompany, "z:\\t_cntvcard\\withcompany.vcf");
//vCard without CompanyField
_LIT(KVCardWithoutCompany, "z:\\t_cntvcard\\withoutcompany.vcf");

_LIT(KVCardLargePhoto, "Z:\\t_cntvcard\\vcardwithlargephoto.vcf");


CCntTest* CntTest=NULL;
LOCAL_D RTest test(KTestName);

LOCAL_C void TestCondition(TBool aCondition, TInt aLineNumber);
#define TEST_CONDITION(x) TestCondition(x, __LINE__)

const TInt KNumAccessCountContacts= 5;
const TInt KMachineUniqueId=0;
const TInt KNumberOfRandomContacts = 40;

_LIT(KName,"Name");
_LIT(KPhoneNumber,"1111");
_LIT8(KModifiedNumberProperty,"TEL;HOME;CELL:11112222\r\n"); //no VOICE parameter!
_LIT8(KModifiedNumberProperty2,"tel;work;cell;2:4444\r\ntel;home;Voice;CELL:\r\n"); //empty property to delete
_LIT8(KModifiedNumberProperty3,"TEL;HOME;CELL:\r\nTEL;HOME;CELL:3333\r\n"); 

_LIT(KVCardFile1,"c:\\cntvcard1.vcf");
_LIT(KVCardFile2,"c:\\cntvcard2.vcf");
_LIT(KVCardFile3,"c:\\cntvcard3.vcf");

// data we expect to see in KTestVcardFile
_LIT(KFirstName, "first");
_LIT(KFirstNamePrn, "f-i-r-s-t");
_LIT(KSurName, "last");
_LIT(KSurNamePrn, "l-a-s-t");
_LIT(KOrgName, "I work here");
_LIT(KOrgNamePrn, "Eye w-o-r-k h-e-r-e");

// labels we expect from the conacts resource file
_LIT(KFirstNameLabel, "First name");
_LIT(KFirstNamePrnLabel, "First name reading");
_LIT(KSurNameLabel, "Last name");
_LIT(KSurNamePrnLabel, "Last name reading");
_LIT(KOrgNameLabel, "Company");
_LIT(KOrgNamePrnLabel, "Company reading");

_LIT(KVCardFile5, "c:\\cntvcard5.vcf");
_LIT(KVCardFile6, "c:\\cntvcard6.vcf");
_LIT8(KMasterVcard, "BEGIN:VCARD\r\n"
					"VERSION:2.1\r\n"
					"REV:20060821T131829Z\r\n"
					"UID:f356da00bfd5320f-00e0f93a015e3ec0-18\r\n"
					"N:Aaaa;Pekka;;;\r\n"
					"END:VCARD\r\n"
					"BEGIN:VCARD\r\n"
					"VERSION:2.1\r\n"
					"REV:20060821T131916Z\r\n"
					"UID:f356da00bfd5320f-00e0f93a015e8128-19\r\n"
					"N:Bbb;Pekka;;;\r\n"
					"END:VCARD\r\n"
					"BEGIN:VCARD\r\n"
					"VERSION:2.1\r\n"
					"REV:20060821T131829Z\r\n"
					"UID:f356da00bfd5320f-00e0f93a015eb46d-20\r\n"
					"N:Cccc;Pekka;;;\r\n"
					"END:VCARD\r\n"
);
_LIT8(KModifierVcard,"BEGIN:VCARD\r\n"
					"VERSION:2.1\r\n"
					"REV:20060821T131829Z\r\n"
					"UID:f356da00bfd5320f-00e0f93a015e3ec0-18\r\n"
					"N:Aaaa;Pekka;;;\r\n"
					"END:VCARD\r\n"
					"BEGIN:VCARD\r\n"
					"VERSION:2.1\r\n"
					"REV:20060821T131916Z\r\n"
					"UID:f356da00bfd5320f-00e0f93a015e8128-19\r\n"
					"N:;Pekka;;;\r\n"
					"END:VCARD\r\n"
					"BEGIN:VCARD\r\n"
					"VERSION:2.1\r\n"
					"REV:20060821T131829Z\r\n"
					"UID:f356da00bfd5320f-00e0f93a015eb46d-20\r\n"
					"N:Cccc;Pekka;;;\r\n"
					"END:VCARD\r\n"
);
 
_LIT8(KPartialVCard,"BEGIN:VCARD\r\n"
        "\r"
);

const TAny* GNames[]  = {&KFirstName, &KFirstNamePrn, &KSurName, &KSurNamePrn, &KOrgName, &KOrgNamePrn};
const TAny* GLabels[]  = {&KFirstNameLabel, &KFirstNamePrnLabel, &KSurNameLabel, &KSurNamePrnLabel, &KOrgNameLabel, &KOrgNamePrnLabel};

const TInt KNumberOfExtendedFieldNames=6;

const TInt KExtendedFieldNames[KNumberOfExtendedFieldNames] = 
	{
	KUidContactFieldGivenNameValue, 
	KUidContactFieldGivenNamePronunciationValue,
	KUidContactFieldFamilyNameValue, 
	KUidContactFieldFamilyNamePronunciationValue,
	KUidContactFieldCompanyNameValue, 	
	KUidContactFieldCompanyNamePronunciationValue, 
	};

		
/** Tests for default VOICE property parameter handling (tests1-4), and the X-IRMC-x extentions for the SOUND property (tests 5-6) */
class CDefaultVoiceParamTests : public CBase
	{
public :
	enum THowToCheck {ECheckValue = 1, ECheckLabel = 2, EAppendValue = 4, EAppendLabel = 8};

public:
	static CDefaultVoiceParamTests* NewLC(CCntTest& aTestLibrary);
	~CDefaultVoiceParamTests();
	void RunTestsL();
private:
	CDefaultVoiceParamTests(CCntTest& aTestLibrary);
	void ConstructL();
	void TestOneL();
	void TestTwoL();
	void TestThreeL();
	void TestFourL();
	void TestFiveL();
	void TestSixL();
	void ImportContactsL(const TDesC& aFileName);
	void ExportContactsL(const TDesC& aFileName, const CContactIdArray* aIds);
	void CreateTestDataL(TContactItemId aId, const TDesC& aFileName, const TDesC8& aVCardData);
	TInt CountPhoneNumberFieldsL(TContactItemId aId);
	TContactItemId CreateItemL(const TDesC& aName, const TDesC& aNumber);
	static void CheckCard(CContactCard* aCard,  CDesCArray* aFields = NULL, CDesCArray* aLabels =NULL, TUint aFlags = ECheckValue|ECheckLabel);
	void CheckCardL(TContactItemId aId,  CDesCArray* aFields = NULL, CDesCArray* aLabels =NULL, TUint aFlags = ECheckValue|ECheckLabel) const;
	
private:
	CCntTest& iTest;
	CContactDatabase* iDb;
	};

CDefaultVoiceParamTests* CDefaultVoiceParamTests::NewLC(CCntTest& aTestLibrary)
	{
	CDefaultVoiceParamTests* self = new(ELeave) CDefaultVoiceParamTests(aTestLibrary);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CDefaultVoiceParamTests::~CDefaultVoiceParamTests()
	{
	}

void CDefaultVoiceParamTests::RunTestsL()
	{
	TestFiveL(); //these two have to run 1st since tests 1-4 leave the db in an inconvenient state
	TestSixL();
	TestOneL();
#if defined(_DEBUG)
	//this test cannot be run in release mode due to UID mismatch.
	TestTwoL();
#endif
	TestThreeL();
	TestFourL();	
	}

CDefaultVoiceParamTests::CDefaultVoiceParamTests(CCntTest& aTestLibrary) : iTest(aTestLibrary)
	{
	}

void CDefaultVoiceParamTests::ConstructL()
	{
	iDb=iTest.CreateDatabaseL();
	iDb->OverrideMachineUniqueId(KMachineUniqueId); 
	}

/** Check all the fields specified by KExtendedFieldNames for correctness.
	The specific behaviour on what is checked is regulated by aFlags. 
	This also prints each field with its label.
	
	@param aFields 	If present and the ECheckValue bit of aFlags set, the nth field text value is compared against
					the nth descriptor in aFields. 
					If present and EAppendValue bit of aFlags set, each field's text value is appended to the end of aFields. 	
						
	@param aLabels 	If present and the ECheckLabel bit of aFlags set, the nth field's label is compared against
					the nth descriptor in ECheckLabel. 
					If present and EAppendLabel bit of aFlags set, each field's label is appended to the end of aLabels. 
	@param aFlags	Bitfields indicating how aFields and aLabels are used. Note that some values from THowToCheck
					 (like ECheckValue and EAppendValue) cannot be used at the same time.
 */
void CDefaultVoiceParamTests::CheckCard(CContactCard* aCard,  CDesCArray* aFields, CDesCArray* aLabels, TUint aFlags)
	{
	CContactItemFieldSet& fieldSet=aCard->CardFields();
	for(TInt i=0;i<KNumberOfExtendedFieldNames;i++)
		{
		TInt index = fieldSet.Find( TUid::Uid(KExtendedFieldNames[i]));
		TEST_CONDITION(index != KErrNotFound);
      	TPtrC text;
		if(aFields && (ECheckValue& aFlags))
			{
			TPtrC compare((*aFields)[i]);
			TBool succeed(EFalse);
			while(index != KErrNotFound)
                {
    			text.Set(fieldSet[index].TextStorage()->Text());
    			succeed = (text.Compare(compare)==0);
    			if(succeed)
    			    {
    			    break;
    			    }
    			index = fieldSet.FindNext( TUid::Uid(KExtendedFieldNames[i]), index + 1);    
                }
            TEST_CONDITION(succeed);    
			}
		else if (aFields && (aFlags & EAppendValue))
			{
			text.Set(fieldSet[index].TextStorage()->Text());
			TRAP_IGNORE(aFields->AppendL(text));
			}
		
		TPtrC label(fieldSet[index].Label());
		if(aLabels && (ECheckLabel& aFlags))
			{
			TPtrC comparelabel((*aLabels)[i]);
			TEST_CONDITION(label.Compare(comparelabel)==0);
			}
		else if (aLabels && (aFlags & EAppendLabel))
			{
			TRAP_IGNORE(aLabels->AppendL(label));
			}
		test.Printf(_L("%S: %S\n") ,&label, &text);	// print this just to make sure
		}
}

void CDefaultVoiceParamTests::CheckCardL(TContactItemId aId,  CDesCArray* aFields, CDesCArray* aLabels, TUint aFlags) const
	{
	CContactCard* card = STATIC_CAST(CContactCard*, iDb->OpenContactL(aId));
	CleanupStack::PushL(card);
	CheckCard(card, aFields, aLabels, aFlags);
	iDb->CloseContactL(aId);
	CleanupStack::PopAndDestroy(card);
	}

/** 
Default number is edited in PIM and synchronisation is started. 
Default number is not edited in phone but new number is inserted

1. Choose a phone number field of a phone contact as a default (For example Tel home: 1111)
2. Start synchronisation
3. Edit the contact's default number in PIM (For example Home: 1111 -> 11112222)
4. Start synchronisation
5. Default number wasn't amended in phone but new edited number detail (Tel Home 11112222) was created.
*/
void CDefaultVoiceParamTests::TestOneL()
	{
	TContactItemId id = CreateItemL(KName(),KPhoneNumber());
	
	CContactIdArray* array = CContactIdArray::NewLC();
	array->AddL(id);


	ExportContactsL(KVCardFile1,array);
	CreateTestDataL(id, KVCardFile2,KModifiedNumberProperty); 
	ImportContactsL(KVCardFile2);
	ExportContactsL(KVCardFile3,array); //for checking
	CleanupStack::PopAndDestroy(array);

	TEST_CONDITION(CountPhoneNumberFieldsL(id)==1); //no new field was created
	}

/**
Case2
Default number is deleted from PIM and new number is created in PIM. 
After synchronisation default number is not deleted from PIM and new number is not created to phone.
(If default number is deleted only and synch is started, default number is not deleted from phone)

	1. Choose a phone number field of a phone contact as a default
		(For example Tel home: 1111)
	2. Start Synchronisation
	3. Delete default number (Home:1111) in PIM and create a new number in PIM (Work2: 4444)
	4. Start synchronisation
	5. Default number wasn't deleted from phone and new number wasn't inserted to phone
*/
void CDefaultVoiceParamTests::TestTwoL()
	{
	TContactItemId id = CreateItemL(KName,KPhoneNumber);

	CContactIdArray* array = CContactIdArray::NewLC();
	array->AddL(id);
	ExportContactsL(KVCardFile1,array);
	CreateTestDataL(id,KVCardFile2,KModifiedNumberProperty2); 
	ImportContactsL(KVCardFile2);
	ExportContactsL(KVCardFile3,array); //for checking
	CleanupStack::PopAndDestroy(array);
	TEST_CONDITION(CountPhoneNumberFieldsL(id)==1); 

	CContactItem* item = iDb->ReadContactLC(id);
	CContactItemFieldSet& fieldSet = item->CardFields();
	TInt pos=fieldSet.Find(KUidContactFieldPhoneNumber);
	while (pos!=KErrNotFound)
		{
		const CContactItemField& field = (fieldSet)[pos];
		if (field.Storage()->IsFull())
			{
			CContactTextField* textfield = field.TextStorage();
			_LIT(KExpectedNumber,"4444");
			TEST_CONDITION(textfield->Text()==KExpectedNumber);
			}
		pos=fieldSet.FindNext(KUidContactFieldPhoneNumber,++pos);
		}
	CleanupStack::PopAndDestroy(item);
	}


/**
Case3
Default number is selected in phone, and same number is edited in PIM. 
After synchronisation edited number is not syncronised at all
	1. Choose a phone number field of a phone contact as a default
		(For example Tel home: 1111)
	2. Edit the same number (phone's default) for example
                         home 1111 to 11114444 in PIM
	3. Start Synchronisation
	4. Default number was not edited in phone. New number was inserted to phone 
	but number was same as the default's number. So, edited number wasn't 
	synchronised at all.

*/
void CDefaultVoiceParamTests::TestThreeL()
	{
	//Not sure how to simulate. I believe this is case is duplicated by TestOneL()
	}

/*
Case 4
Default number is selected in phone, same number is deleted from PIM and new 
number is created in PIM. 
After synchronisation default number is not deleted from PIM but it is duplicated. 
	1. Choose a phone number field of a phone contact as a default
		(For example Tel home: 1111)
	2. Delete the same number (phone's default) in PIM and create a new number in PIM 
	3. Start Synchronisation
	4. Default is not deleted from PIM. New number is inserted to phone but number is 
	same as default's. Number you created in PIM, is copied correctly to phone.

*/
void CDefaultVoiceParamTests::TestFourL()
	{
	TContactItemId id = CreateItemL(KName,KPhoneNumber);

	CContactIdArray* array = CContactIdArray::NewLC();
	array->AddL(id);
	ExportContactsL(KVCardFile1,array);
	CreateTestDataL(id, KVCardFile2,KModifiedNumberProperty3); 
	ImportContactsL(KVCardFile2);
	ExportContactsL(KVCardFile3,array); //for checking
	CleanupStack::PopAndDestroy(array);
	TEST_CONDITION(CountPhoneNumberFieldsL(id)==1); //no new field was created	
	}


/**This checks the contact indentity fields extracted from a known vCard (KTestVcardFile) to ensure
	the contact is exactly what we expect. This tests both the field value and label in the generated contact.
	*/
void CDefaultVoiceParamTests::TestFiveL()
	{
	CDesCArray* value = new(ELeave) CDesCArrayFlat(KNumberOfExtendedFieldNames);
	CleanupStack::PushL(value);
	CDesCArray* label = new(ELeave) CDesCArrayFlat(KNumberOfExtendedFieldNames);	
	CleanupStack::PushL(label);

	for(TInt i=0;i<KNumberOfExtendedFieldNames;i++)
		{
		value->AppendL(*((const TDesC*) GNames[i]));
		label->AppendL(*((const TDesC*) GLabels[i]));
		}
	ImportContactsL(KTestVcardFile);
	const CContactIdArray& list = *iDb->SortedItemsL();
	CheckCardL(list[0], value, label);
	CleanupStack::PopAndDestroy(2,value); // + label
	iDb->DeleteContactL(list[0]);
	}

/** This generates a whole bunch of conacts (KNumberOfRandomContacts) and exports them to a vCard file.
	The contacts are then read back in. This tests the merge functionality of vCard parsing.
	The retrieved cards' fields are checked against the original cards' fields to make sure they're the same */
void CDefaultVoiceParamTests::TestSixL()
	{
	CDesCArray* fields = new(ELeave) CDesC16ArrayFlat(KNumberOfExtendedFieldNames*KNumberOfRandomContacts);
	CleanupStack::PushL(fields);

	CDesCArray* labels = new(ELeave) CDesC16ArrayFlat(KNumberOfExtendedFieldNames);	
	CleanupStack::PushL(labels);
	TInt  i;	
	for( i=0;i<KNumberOfExtendedFieldNames;i++)
		{
		labels->AppendL(*((const TDesC*) GLabels[i]));
		}
	RArray<TInt> allIds;
	CleanupClosePushL(allIds);
			
	TInt64 randseed = 0;
	CWordDigrams* firstname = CWordDigrams::NewLC(KFirstNameFileName, randseed);
	CWordDigrams* lastname = CWordDigrams::NewLC(KLastNameFileName, randseed);
	CleanupStack::Pop(lastname);
	CleanupStack::Pop(firstname);
	CRandomContactGenerator* generator = CRandomContactGenerator::NewL(firstname,lastname );

	CleanupStack::PushL(generator);
	
	generator->SetDbL(*iDb);
	RArray<TInt> nameIds;
	CleanupClosePushL(nameIds);
	for( i = 0; i<KNumberOfExtendedFieldNames;i++)
		{
		nameIds.AppendL(KExtendedFieldNames[i]);
		}


	for ( i = 0;i<KNumberOfRandomContacts;i++)
		{
		TContactItemId id = generator->AddTypicalRandomContactWithNamesL(nameIds, ETrue);
		allIds.AppendL(id);
		CContactCard* card = STATIC_CAST(CContactCard*, iDb->OpenContactL(id));
		CleanupStack::PushL(card);
		CheckCard(card,fields, labels,ECheckLabel|EAppendValue );
		iDb->CommitContactL(*card);	
		CleanupStack::PopAndDestroy(card);
	    }
	    
	CleanupStack::PopAndDestroy( 2, generator );	// + nameIds
	
	ExportContactsL(KVCardFile1, iDb->SortedItemsL());
	ImportContactsL(KVCardFile1);

	for (i =0; i<allIds.Count();i++)
		{
		CheckCardL(allIds[i], fields);
		for(TInt j=0;j<KNumberOfExtendedFieldNames;j++)
			{
			fields->Delete(0);
			}
		}
	CleanupStack::PopAndDestroy(3, fields );	// +label

	}


/** Import contacts from the vCard file specified by aFileName */
void CDefaultVoiceParamTests::ImportContactsL(const TDesC& aFileName)
	{
	RFileReadStream vcard;
	User::LeaveIfError(vcard.Open(iTest.Fs(), aFileName, EFileRead));
	CleanupClosePushL(vcard);

	TBool success=EFalse;
	CArrayPtr<CContactItem>* contactItems=iDb->ImportContactsL(TUid::Uid(KUidVCardConvDefaultImpl), vcard, success, CContactDatabase::EImportSingleContact+CContactDatabase::ETTFormat);
	CleanupStack::PopAndDestroy(&vcard);
	contactItems->ResetAndDestroy();
	delete contactItems;
	}

/** Export contact items specified by aIds to aFileName */
void CDefaultVoiceParamTests::ExportContactsL(const TDesC& aFileName, const CContactIdArray* aIds)
	{
	RFile outfile;
	outfile.Replace(iTest.Fs(),aFileName,EFileWrite);
	CleanupClosePushL(outfile);
	RFileWriteStream writeStream(outfile);
	CleanupClosePushL(writeStream);
		
	TUid uid;
	uid.iUid=KUidVCardConvDefaultImpl;
	iDb->ExportSelectedContactsL(uid,*aIds,writeStream,CContactDatabase::EDefault);

	writeStream.CommitL();
	CleanupStack::PopAndDestroy(2); //writeStream.Close(), outfile.Close()
	}


/** Creates a vCard with the correct UID for the contact item specified by aId. */
void CDefaultVoiceParamTests::CreateTestDataL(TContactItemId aId, const TDesC& aFileName, const TDesC8& aVCardData)
	{
	RFile file;
	TInt err=file.Replace(iTest.Fs(), aFileName, EFileWrite+EFileShareAny+EFileStreamText);
	User::LeaveIfError(err);
	TPtrC8 header((const TText8*)	"BEGIN:VCARD\r\n"
									"VERSION:2.1\r\n"
									"REV:20020520T134824Z\r\n"
									"N:Name;;;;\r\n");
	TPtrC8 footer((const TText8*)	"END:VCARD\r\n");
	TPtrC8 uidProperty((const TText8*)	"UID:");
	TPtrC8 endProperty((const TText8*)	"\r\n");
	file.Write(header);
	file.Write(uidProperty);
	HBufC* buffer = iTest.ContactUidLC(aId,KMachineUniqueId);
	TBuf8<KMaxExternalizedTokenLength> des;
	des.Copy(*buffer);
	file.Write(des);
	CleanupStack::PopAndDestroy(buffer);
	file.Write(endProperty);
	file.Write(aVCardData);
	file.Write(footer);
	file.Close();
	}

/** Count the number of non-empty phone number fields */
TInt CDefaultVoiceParamTests::CountPhoneNumberFieldsL(TContactItemId aId)
	{
	CContactItem* item = iDb->ReadContactLC(aId);
	CContactItemFieldSet& fieldSet = item->CardFields();
	TInt numPhoneNumbers=0;
	TInt pos=fieldSet.Find(KUidContactFieldPhoneNumber);
	while (pos!=KErrNotFound)
		{
		const CContactItemField& field = (fieldSet)[pos];
		if (field.Storage()->IsFull())
			numPhoneNumbers++;
		pos=fieldSet.FindNext(KUidContactFieldPhoneNumber,++pos);
		}
	CleanupStack::PopAndDestroy(item);
	return numPhoneNumbers;
	}

/** Create a contact item */
TContactItemId CDefaultVoiceParamTests::CreateItemL(const TDesC& aName, const TDesC& aNumber)
	{
	TContactItemId templateId = iDb->TemplateId();
	CContactItem* templateCard = iDb->ReadContactLC(templateId);
	CContactCard* card=CContactCard::NewL(templateCard); 
	CleanupStack::PushL(card);
	SetNameL(*card, KUidContactFieldFamilyName,KUidContactFieldVCardMapUnusedN, aName, EFalse);
	SetNameL(*card, KUidContactFieldPhoneNumber,KUidContactFieldVCardMapTEL, aNumber, EFalse);
	TContactItemId id = iDb->AddNewContactL(*card);
	CleanupStack::PopAndDestroy(2,templateCard); 
	return id;
	}


/** 
 * Regression testcode for defect EXT-599EF6 "Problems appear when contact's 
 * default number is deleted / edited in PIM - Lotus Organizer 6.0"
 * 
 * The basic problem is:
 * SymbianOS uses TEL;VOICE in a vCard to represent a phone number
 * TimeIS PC Connectivity code sends TEL;VOICE to PIM
 * PIM removes VOICE property parameter
 * TimeIS sends TEL back 
 * Contacts model does not match TEL;VOICE and TEL, so a duplicate number is added
 *
 */
LOCAL_C void DefaultVoiceParamTestsL()
	{
	CDefaultVoiceParamTests* test = CDefaultVoiceParamTests::NewLC(*CntTest);
	test->RunTestsL();
	CleanupStack::PopAndDestroy(test);		
	}

/** Add KNumAccessCountContacts contacts **/
LOCAL_C void AddNewContactsL()
	{
	_LIT(KNameFormat,"NAME #%d");
	_LIT(KPhoneNumber,"123");
	for (TInt ii=0;ii<KNumAccessCountContacts;ii++)
		{
		CContactCard* card=CContactCard::NewL();
		CleanupStack::PushL(card);
		TBuf<16> name;
		name.Format(KNameFormat,ii);
		SetNameL(*card, KUidContactFieldFamilyName,KUidContactFieldVCardMapUnusedN, name, EFalse);
		SetNameL(*card, KUidContactFieldPhoneNumber,KUidContactFieldVCardMapTEL, KPhoneNumber, EFalse);
		CntTest->Db()->AddNewContactL(*card);
		CleanupStack::PopAndDestroy(card); 
		}
	}	


LOCAL_C void EmptyDatabase()
	{
	TContactItemId theid;
	TContactIter iter(*CntTest->Db());
	theid=iter.FirstL();
	while(theid!=KNullContactId )
		{
		CntTest->Db()->DeleteContactL(theid);
		theid=iter.NextL();
		}
	}
	

enum TAccessCountFlags {EIncAccessCount,EDecAccessCount};

LOCAL_C void UpdateAccessCountL(TAccessCountFlags aUpdateType)
	{
	TContactItemId theid;
	TContactIter iter(*CntTest->Db());
	theid=iter.FirstL();
	while(theid!=KNullContactId )
		{
		CContactItem* contactItem=CntTest->Db()->OpenContactL(theid);
		CleanupStack::PushL(contactItem);
		if (contactItem->IsDeleted())
			test.Next(_L("Synchronizer knows record 2 is deleted"));

		if(aUpdateType==EIncAccessCount)
			contactItem->IncAccessCount();
		else
			contactItem->DecAccessCount();
		
		theid=iter.NextL();
		CntTest->Db()->CommitContactL(*contactItem);
		CleanupStack::PopAndDestroy(contactItem);	
		}
	}

/** Tests from T_EXPDEL */
LOCAL_C void AccessCountTestsL()
	{
	CContactDatabase* db=CntTest->CreateDatabaseL();
	AddNewContactsL();
	TEST_CONDITION(db->CountL()==KNumAccessCountContacts);

	test.Next(_L("Increase Access Count"));
	UpdateAccessCountL(EIncAccessCount);
	
	test.Next(_L("Removing second contact"));
	TContactIter iter(*db);
	TContactItemId theid=iter.FirstL();
	theid=iter.NextL();
	db->DeleteContactL(theid);
	TEST_CONDITION(db->CountL()==KNumAccessCountContacts-1);
	
#if defined(_DEBUG) //test case only valid in debug mode, see CContactDatabase::DeletedContactsLC for detail.
	test.Next(_L("Checking DeletedContactsLC Deleted Count and Identity"));
	CContactIdArray* delArray = db->DeletedContactsLC();
	TEST_CONDITION(delArray->Count() == 1);  // as second contact is deleted but has access count of > 1
	TEST_CONDITION((*delArray)[0] == theid); // test the DeletedContact Id is the second Id (that was deleted above)	
	
	CleanupStack::PopAndDestroy(delArray);
#endif //defined(_DEBUG)
	
	test.Next(_L("Synchronizing"));
	UpdateAccessCountL(EDecAccessCount);
	TEST_CONDITION(db->CountL()==KNumAccessCountContacts-1);
	test.Next(_L("Exporting"));
	CContactIdArray* TheIds=CContactIdArray::NewLC();
	theid=iter.FirstL();
	while(theid!=KNullContactId )
		{
		TheIds->AddL(theid);
		theid=iter.NextL();
		}			
	CFileStore* store = CDirectFileStore::ReplaceLC(CntTest->Fs(),KExpDelAFileName(),EFileWrite);
	store->SetTypeL(KDirectFileStoreLayoutUid);
	RStoreWriteStream outstream;
	TStreamId id = outstream.CreateLC(*store);
	TUid uid;
	uid.iUid= KVersitEntityUidVCard; // KUidVCardConvDefaultImpl;
	db->ExportSelectedContactsL(uid,*TheIds,outstream,CContactDatabase::EIncludeX);
	outstream.CommitL();
	store->SetRootL(id);
	store->CommitL();  	
	CleanupStack::PopAndDestroy(2); // store+ oustream
	CleanupStack::PopAndDestroy(TheIds); 
	test.Next(_L("Emptying database"));
	EmptyDatabase();
	}

//
/* Test Function Implementations                                             */
//
TContactItemId AddContactL (CContactDatabase& aDatabase)
	{
	TInt bit = 0;
	TContactItemId retval;
	CRandomContactGenerator* generator = NULL;

	generator = CRandomContactGenerator::NewL();
	CleanupStack::PushL(generator);

	generator->SetDbL(aDatabase);

	bit |= CContactDatabase::ESmsable;
	retval = generator->AddTypicalContactForFilterL(bit);

	CleanupStack::PopAndDestroy( generator );
	
	return retval;
	}

void ExportContactTestL()
	{
	TContactItemId itemId;
	RFs fsSession;
	RFileWriteStream fileStream;
	CContactIdArray* idArray = NULL;
	CContactItem* contactAdded = NULL;
	TInt firstNameIndex = KErrNotFound;

	idArray = CContactIdArray::NewL();

	CleanupStack::PushL(idArray);

	User::LeaveIfError(fsSession.Connect());
	User::LeaveIfError(fileStream.Replace(fsSession, KOutputFileName, EFileWrite));


	itemId = AddContactL(*CntTest->Db());
	contactAdded = CntTest->Db()->OpenContactL(itemId);
	CleanupStack::PushL(contactAdded);

	CContactItemFieldSet& fieldSet = contactAdded->CardFields();
	
	firstNameIndex = fieldSet.Find(KUidContactFieldGivenName);
	TEST_CONDITION(firstNameIndex != KErrNotFound);

	CContactItemField& field = fieldSet[firstNameIndex];
	field.SetLabelL(_L(" New Label "));

	CntTest->Db()->CommitContactL(*contactAdded);
	CleanupStack::PopAndDestroy(contactAdded);
	contactAdded = NULL;


	idArray->InsertL(0, itemId);

	CntTest->Db()->ExportSelectedContactsL(TUid::Uid(KUidVCardConvDefaultImpl), *idArray, fileStream, 0 /*option*/);
	CntTest->Db()->ExportSelectedContactsL(TUid::Uid(KUidVCardConvDefaultImpl), *idArray, fileStream, 1 /*option*/);

	CleanupStack::PopAndDestroy(idArray);

	fileStream.Close();
	fsSession.Close();
	}

// Export a contact which has no name field,
// and check that the exported file doesn't contain a name field:
void ExportNamelessContactTestL()
	{
	TContactItemId itemId;
	RFs fsSession;
	RFileWriteStream fileStream;
	RFileReadStream readStream;
	CContactIdArray* idArray = NULL;
	CContactItem* contactAdded = NULL;
	TInt Index = KErrNotFound;

	idArray = CContactIdArray::NewL();

	CleanupStack::PushL(idArray);

	User::LeaveIfError(fsSession.Connect());
	User::LeaveIfError(fileStream.Replace(fsSession, KOutputFileName, EFileWrite));

	// Create the usual random contact:
	itemId = AddContactL(*CntTest->Db());
	contactAdded = CntTest->Db()->OpenContactL(itemId);
	CleanupStack::PushL(contactAdded);

	// Find the name fields and remove them:
	CContactItemFieldSet& fieldSet = contactAdded->CardFields();
	Index = fieldSet.Find(KUidContactFieldGivenName);
	if(Index != KErrNotFound)
		fieldSet.Remove(Index);
	Index = fieldSet.Find(KUidContactFieldFamilyName);
	if(Index != KErrNotFound)
		fieldSet.Remove(Index);
	Index = fieldSet.Find(KUidContactFieldAdditionalName);
	if(Index != KErrNotFound)
		fieldSet.Remove(Index);
	Index = fieldSet.Find(KUidContactFieldPrefixName);
	if(Index != KErrNotFound)
		fieldSet.Remove(Index);
	Index = fieldSet.Find(KUidContactFieldSuffixName);
	if(Index != KErrNotFound)
		fieldSet.Remove(Index);

	// Put the contact back:
	CntTest->Db()->CommitContactL(*contactAdded);
	CleanupStack::PopAndDestroy(contactAdded);
	contactAdded = NULL;

	// Export it:
	idArray->InsertL(0, itemId);
	CntTest->Db()->ExportSelectedContactsL(TUid::Uid(KUidVCardConvDefaultImpl), *idArray, fileStream, 0 /*option*/);
	CleanupStack::PopAndDestroy(idArray);
	fileStream.Close();

	// Verify the output by reading it in and searching it long hand:
	User::LeaveIfError(readStream.Open(fsSession, KOutputFileName, EFileRead));
	HBufC8 * buf = HBufC8::NewLC(100);
	TPtr8 ptr = buf->Des();
	TRAPD(ret, readStream.ReadL(ptr));
	TEST_CONDITION(ret == KErrNone || (ret == KErrEof && buf->Length() > 0));

	// This is the important test - we should not find a name entry:
	_LIT8(str, "\nN:");
	TEST_CONDITION(buf->Find(str) == KErrNotFound);

	// Clean up:
	CleanupStack::PopAndDestroy(buf);

	readStream.Close();
	fsSession.Close();
	}

//=======================================================================
//FindInFields
//Finds a given value in the field set, assumes that the fields are text.
//Used by CheckTypeSupport.
//=======================================================================
TBool FindInFields(CContactItemFieldSet& aFieldSet,TPtrC aExpectedValue)
	{
	for (TInt i = 0; i< aFieldSet.Count(); i++ )
		{
		CContactItemField& aFieldValue = aFieldSet[i];
		TPtrC value = aFieldValue.TextStorage()->Text();
		if (value == aExpectedValue)
			{
			return ETrue;
			}
		}
		
	// Value not found, return false.
	return EFalse;
	}

//=======================================================================
//FieldCheck
//Compares the value of the CContactItemField at aIndex of aFieldSet
//with the expected value.  Used by UpdateVCardTestL
//=======================================================================
TBool FieldCheck(CContactItemFieldSet& aFieldSet,TInt aIndex,TPtrC aExpectedValue)
	{
	CContactItemField& aPhoneNum = aFieldSet[aIndex];
	TPtrC value = aPhoneNum.TextStorage()->Text();
	return (value == aExpectedValue);
	}
//=======================================================================
//UpdateVCardTestL()
//Checks updating of a contact via a modified vcard.
//1: Contact is imported from vcardupdate1.vcf
//2: Contact is exported to vcardupdate2.vcf
//3: Contact is updated from vcardupdate3.vcf
//=======================================================================
void UpdateVCardTestL()
	{
	test.Next(_L("Update VCard Test"));
	//Check the database is empty before we start
	EmptyDatabase();
	TInt count = CntTest->Db()->CountL();
	TEST_CONDITION(count == 0);
	test.Next(_L("Database Empty"));
	
	RFs fsSession;
	User::LeaveIfError(fsSession.Connect());
	
	RFileReadStream readStream;
	RFileWriteStream writeStream;
	CContactIdArray* idArray = NULL;
	
	idArray = CContactIdArray::NewL();
	CleanupStack::PushL(idArray);
		
	//Import vcardupdate1.vcf
	test.Next(_L("Importing contact from vcardupdate1.vcf"));
	User::LeaveIfError(readStream.Open(fsSession,KVCardUpdate1, EFileRead));
	CleanupClosePushL(readStream);
	TBool success=EFalse;
	CArrayPtr<CContactItem>* contactItems=CntTest->Db()->ImportContactsL(TUid::Uid(KUidVCardConvDefaultImpl), readStream, success, CContactDatabase::EImportSingleContact+CContactDatabase::ETTFormat);
	//Store the id of the new contact - we need this later
	TContactItemId contactId = (*contactItems)[0]->Id();
	idArray->InsertL(0, contactId);
	readStream.Close();
	//Should be the only contact in the database at this point
	count = CntTest->Db()->CountL();
	TEST_CONDITION(count == 1);
	test.Next(_L("Imported"));
	
	//Retrieve the new contact from the database and check the fields
	CContactItem* importedContact = CntTest->Db()->ReadContactLC(contactId);
	CContactItemFieldSet& importedContactFieldSet = importedContact->CardFields();

	/*
	* For this particular test we are only interested in the following field indexes:
	* 5:	Mobile 1
	* 6:	Mobile 2
	* 29:	Work tel 1
	* 30:	Work tel 2
	* 31:	Work tel 3
	* 32:	Work tel 4
	* 33:	Work tel 5
	* You can get other field indexes by using:
 	*
	 _LIT(KFormat,"%d:\t");
	 for (TInt i = 0; i < importedContactFieldSet.Count();i++)
		{
		CContactItemField& aPhoneNum = importedContactFieldSet[i];
		TPtrC label = aPhoneNum.Label();
		test.Printf(KFormat,i);
		test.Printf(label);
		}
	*/
	
	test.Next(_L("Check Mobile Number 1"));
	TEST_CONDITION(FieldCheck(importedContactFieldSet,5,_L("07905111")));
		
	test.Next(_L("Check Mobile Number 2"));
	TEST_CONDITION(FieldCheck(importedContactFieldSet,6,_L("07906222")));

	test.Next(_L("Check Work Number 1"));
	TEST_CONDITION(FieldCheck(importedContactFieldSet,29,_L("1111111111")));

	test.Next(_L("Check Work Number 2"));
	TEST_CONDITION(FieldCheck(importedContactFieldSet,30,_L("2222222222")));

	test.Next(_L("Check Work Number 3"));
	TEST_CONDITION(FieldCheck(importedContactFieldSet,31,_L("3333333333")));

	test.Next(_L("Check Work Number 4"));
	TEST_CONDITION(FieldCheck(importedContactFieldSet,32,_L("4444444444")));

	test.Next(_L("Check Work Number 5"));
	TEST_CONDITION(FieldCheck(importedContactFieldSet,33,_L("5555555555")));
	
	CleanupStack::PopAndDestroy(importedContact);

	//Export the contact to vcardupdate2.vcf
	test.Next(_L("Exporting contact to vcardupdate2.vcf"));
	User::LeaveIfError(writeStream.Replace(fsSession,KVCardUpdate2,EFileWrite));
	CleanupClosePushL(writeStream);
	CntTest->Db()->ExportSelectedContactsL(TUid::Uid(KUidVCardConvDefaultImpl), *idArray, writeStream,0);
	CleanupStack::PopAndDestroy(&writeStream);

	//Import vcardupdate3.vcf - this should have the same UID as vcardupdate2.vcf
	test.Next(_L("Updating contact from vcardupdate3.vcf"));
	User::LeaveIfError(readStream.Open(fsSession,KVCardUpdate3, EFileRead));
	success=EFalse;
	contactItems->ResetAndDestroy();
	delete contactItems;
	contactItems = CntTest->Db()->ImportContactsL(TUid::Uid(KUidVCardConvDefaultImpl), readStream, success, CContactDatabase::EImportSingleContact+CContactDatabase::ETTFormat);
	
	//We are updating the previous contact so we should still only have
	//one database entry.  If count == 2 here check the uids of C:\\vcardupdate2.vcf and 
	//Z:\\System\\Programs\\CntmodelTest\\vcardupdate3.vcf - the last digits of vcardupdate3 should match vcardupdate2
	count = CntTest->Db()->CountL();
	//TEST_CONDITION(count == 1);
	test.Next(_L("Imported"));

	//Retrieve the updated contact from the database and check the fields
	//Use the same contact id as before
	CContactItem* updatedContact = CntTest->Db()->ReadContactLC(contactId);
	CContactItemFieldSet& updatedContactFieldSet = updatedContact->CardFields();

	test.Next(_L("Check Updated Mobile Number 1"));
	TEST_CONDITION(FieldCheck(updatedContactFieldSet,5,_L("7700900329")));
		
	test.Next(_L("Check Updated Mobile Number 2"));
	TEST_CONDITION(FieldCheck(updatedContactFieldSet,6,_L("07700900529")));

	test.Next(_L("Check Updated Work Number 1"));
	TEST_CONDITION(FieldCheck(updatedContactFieldSet,29,_L("7700900999")));

	test.Next(_L("Check Updated Work Number 2"));
	TEST_CONDITION(FieldCheck(updatedContactFieldSet,30,_L("7700900888")));

	test.Next(_L("Check Updated Work Number 3"));
	TEST_CONDITION(FieldCheck(updatedContactFieldSet,31,_L("7700900777")));

	test.Next(_L("Check Updated Work Number 4"));
	TEST_CONDITION(FieldCheck(updatedContactFieldSet,32,_L("7700900666")));

	test.Next(_L("Check Updated Work Number 5"));
	TEST_CONDITION(FieldCheck(updatedContactFieldSet,33,_L("7700900555")));
 
	//cleanup
	CleanupStack::PopAndDestroy(updatedContact);
	CleanupStack::PopAndDestroy(&readStream);
	CleanupStack::PopAndDestroy(idArray);
	fsSession.Close();
	contactItems->ResetAndDestroy();
	delete contactItems;
	}

/*
The following is the test for trailing space to test DEF051853.
*/

const TUint KSpaceChar = 0x20;

_LIT( KSearchText, "\x2029" );
_LIT( KIncommingVCard, "z:\\cntvcard\\address-with-trailing-white-space.vcf" );
_LIT( KOutgoingVCard, "c:\\address-with-trailing-white-space-out.vcf" );

/** Import contacts from the vCard file specified by aFileName */
CArrayPtr<CContactItem>* ImportContactsL(const TDesC& aFileName, CContactDatabase& aDatabase);
void CheckForWhiteSpaceL( const TDesC& aFieldToCheck );
void ImportAndCheckForNoTrailSpaceStrippingL(const TDesC& aFileName, CContactIdArray& aArray);
void TrailingSpaceExportContactsL(const TDesC& aFileName, CContactIdArray* aIds);
void DoTrailingSpaceImportAndExportTestL();

// Implementations
CArrayPtr<CContactItem>* ImportContactsL(const TDesC& aFileName, CContactDatabase& aDatabase)
	{
	CArrayPtr<CContactItem>* retval = NULL;
	RFileReadStream vcard;
	RFs fileSystem;
	User::LeaveIfError( fileSystem.Connect() );
	CleanupClosePushL( fileSystem );
	User::LeaveIfError(vcard.Open( fileSystem, aFileName, EFileRead));
	CleanupClosePushL(vcard);

	TBool success=EFalse;
	retval = aDatabase.ImportContactsL(TUid::Uid(KVersitEntityUidVCard), vcard, success, CContactDatabase::EImportSingleContact+CContactDatabase::ETTFormat);
	CleanupStack::PopAndDestroy(&vcard);
	CleanupStack::PopAndDestroy(&fileSystem);

	return retval;
	}

void CheckForWhiteSpaceL( const TDesC& aFieldToCheck )
	{
	TInt trailingSpaceLocation = KErrNotFound;
	TUint thisShouldBeASpaceChar = 0;

	trailingSpaceLocation = ( aFieldToCheck.Find(KSearchText) -1 );

	thisShouldBeASpaceChar = aFieldToCheck[ trailingSpaceLocation ];
	test ( thisShouldBeASpaceChar == KSpaceChar );

	}

void ImportAndCheckForNoTrailSpaceStrippingL(const TDesC& aFileName, CContactIdArray& aArray)
	{
	CArrayPtr<CContactItem>* contactList = NULL;
	CContactItem* item = NULL;
	TInt addressIndex = KErrNotFound;

	aArray.Reset();

	contactList = ImportContactsL( aFileName, *CntTest->Db() );
	CleanupStack::PushL( contactList );

	item = (*contactList)[0];
	aArray.AddL( item->Id() );
	CContactItemFieldSet& fieldSet = item->CardFields();

	addressIndex = fieldSet.Find( KUidContactFieldAddress );

	CContactItemField& addressField = fieldSet[ addressIndex ];


	TPtrC data = addressField.TextStorage()->Text();

	CheckForWhiteSpaceL( data );

	contactList->ResetAndDestroy();
	CleanupStack::PopAndDestroy( contactList );

	}

/** Export contact items specified by aIds to aFileName */
void TrailingSpaceExportContactsL(const TDesC& aFileName, CContactIdArray* aIds)
	{
	RFs fsSession;
	RFileWriteStream fileStream;
	
	User::LeaveIfError(fsSession.Connect());
	User::LeaveIfError(fileStream.Replace(fsSession, aFileName, EFileWrite));

	CntTest->Db()->ExportSelectedContactsL(TUid::Uid(KVersitEntityUidVCard), *aIds, fileStream, 0 /*option*/);

	fileStream.Close();
	fsSession.Close();

	}


void DoTrailingSpaceImportAndExportTestL()
	{
	CContactIdArray* idArray = NULL;

	idArray = CContactIdArray::NewL();
	CleanupStack::PushL( idArray );

	ImportAndCheckForNoTrailSpaceStrippingL(KIncommingVCard, *idArray);
	TrailingSpaceExportContactsL( KOutgoingVCard, idArray );

	ImportAndCheckForNoTrailSpaceStrippingL(KOutgoingVCard, *idArray);
	CleanupStack::PopAndDestroy( idArray );
	}


void ImportLargeVCardTestL()
	{
 	test.Printf(_L("Importing large VCards"));
 	
	EmptyDatabase();
			
	RFs fsSession;
	User::LeaveIfError(fsSession.Connect());
	
	RFileReadStream readStream;
		
 	// Import vcardupdate1.vcf.
 	test.Printf(_L("Importing contact from testvcard1.vcf"));
	User::LeaveIfError(readStream.Open(fsSession,KTestVcardFile1, EFileRead));
 
 	CleanupClosePushL(readStream);
 
 	TBool success=EFalse;
 	CArrayPtr<CContactItem>* contactItems=CntTest->Db()->ImportContactsL(
 		TUid::Uid(KUidVCardConvDefaultImpl), readStream, success, 
 		CContactDatabase::EImportSingleContact+CContactDatabase::ETTFormat);
 
 	// If the above function does not leave or panic test has been successful.
 	
 	TEST_CONDITION(contactItems->Count() == 1);
 	
 	test.Printf(_L("Importing large VCards successful\n"));
 
 	contactItems->ResetAndDestroy();
 	delete contactItems;
 
 	CleanupStack::PopAndDestroy(&readStream);	
 
 	fsSession.Close();			
 	}
 	

//return the number of email fields within a contact
TInt ContactEmailCountL(TContactItemId aContactId)
	{
	
	CContactItem *item = CntTest->Db()->ReadContactLC(aContactId);
	TInt pos = 0;
	TInt many = 0;
	while (pos < item->CardFields().Count() )
		{
		pos = item->CardFields().FindNext(KUidContactFieldEMail, pos);
		if( pos == KErrNotFound )
			{
			break;
			}
		else
			{
			++many;
			++pos;
			}
		}
	
	CleanupStack::PopAndDestroy(item);
	return many;
	
	}

//import a single contact item from vcard into db, return id of contact in db
TContactItemId ImportVCardL(const TDesC &aVcard, const TInt aOption)
	{
	RFs fsSession;
	User::LeaveIfError(fsSession.Connect());
	
	RFileReadStream readStream;
		
	//Import vcf
	User::LeaveIfError(readStream.Open(fsSession,aVcard, EFileRead));
	CleanupClosePushL(readStream);
	TBool success=EFalse;
	CArrayPtr<CContactItem>* contactItems = CntTest->Db()->ImportContactsL(
												TUid::Uid(KVersitEntityUidVCard), readStream, success, 
												aOption
											);
											
	//Store the id of the new contact - we need this later
	TContactItemId contactId = (*contactItems)[0]->Id();
	contactItems->ResetAndDestroy();
	delete contactItems;
	contactItems = NULL;
	//cleanup
	CleanupStack::PopAndDestroy(&readStream);
	fsSession.Close();
	return contactId;
	}
	
//returns the number of non empty fields in contact
TInt ContactFilledFields(TContactItemId aContactId)
	{
	CContactItem *item = CntTest->Db()->ReadMinimalContactLC(aContactId);
	TInt ret = item->CardFields().Count();
	CleanupStack::PopAndDestroy(item);
	return ret;
	}

/**
@SYMTestCaseID PIM-T-CNTVCARD-DEF084708-0001
@SYMTestType UT
@SYMTestPriority High
@SYMDEF DEF084708
@SYMTestCaseDesc Vcard with full parameter description of TYPE=VALUE for properties 
ADDR or TEL should be correctly imported.

@SYMTestActions
1) import vcard with home and tel properties with parameters of TYPE=HOME and TYPE=WORK
2) check that imported contact contains the correct home and work address 

@SYMTestExpectedResults For the above tests:
On importing, the type=home and type=work fields should not be deleted.

*/
void CheckTypeSupport(const TDesC &inputVcard)
{
	test.Next(_L("@SYMTestCaseID:PIM-T-CNTVCARD-DEF084708-0001 Checking if the type=value parameter is handled correctly."));
	
	//Check the database is empty before we start
	EmptyDatabase();
	TInt count = CntTest->Db()->CountL();
	TEST_CONDITION(count == 0);

	// First import the vcard file.
	RFs fsSession;
	RFileReadStream readStream;
	User::LeaveIfError(fsSession.Connect());
	test.Printf(_L("Importing contact "));
	User::LeaveIfError(readStream.Open(fsSession,inputVcard, EFileRead));
	CleanupClosePushL(readStream);
	TBool success=EFalse;
	CArrayPtr<CContactItem>* contactItems=CntTest->Db()->ImportContactsL(TUid::Uid(KUidVCardConvDefaultImpl), readStream, success, CContactDatabase::EImportSingleContact+CContactDatabase::ETTFormat);
	readStream.Close();
	
	//Should be the only contact in the database at this point
	count = CntTest->Db()->CountL();
	TEST_CONDITION(count == 1);
	test.Printf(_L("Imported the contact item"));

	// Check that the home address and work address has been imported correctly into the card.
	test.Printf(_L("Find whether all the address fields are present."));
	TContactItemId contactId = (*contactItems)[0]->Id();
	CContactItem* importedContact = CntTest->Db()->ReadContactLC(contactId);
	CContactItemFieldSet& fieldSet = importedContact->CardFields();
	
	TEST_CONDITION(FindInFields(fieldSet, _L("W211")));
	TEST_CONDITION(FindInFields(fieldSet, _L("W212")));
	TEST_CONDITION(FindInFields(fieldSet, _L("W213")));
	TEST_CONDITION(FindInFields(fieldSet, _L("W214")));
	TEST_CONDITION(FindInFields(fieldSet, _L("W215")));

	TEST_CONDITION(FindInFields(fieldSet, _L("H211")));
	TEST_CONDITION(FindInFields(fieldSet, _L("H212")));
	TEST_CONDITION(FindInFields(fieldSet, _L("H213")));
	TEST_CONDITION(FindInFields(fieldSet, _L("H214")));
	TEST_CONDITION(FindInFields(fieldSet, _L("H215")));
		
	// Check that the phone number has been correctly imported	
	TEST_CONDITION(FindInFields(fieldSet, _L("222222222222222222")));
	TEST_CONDITION(FindInFields(fieldSet, _L("333333333333333333")));

	//cleanup
	CleanupStack::PopAndDestroy(importedContact);
	CleanupStack::PopAndDestroy(&readStream);
	fsSession.Close();
	contactItems->ResetAndDestroy();
	delete contactItems;
}

/**
@SYMTestCaseID PIM-T-CNTVCARD-DEF081712-0001
@SYMTestType UT
@SYMTestPriority High
@SYMDEF DEF081712
@SYMTestCaseDesc Contact properties on device not deleted during synchronisation

@SYMTestActions
1) import vcard with 3 emails
2) check that imported contact contains 3 email fields
3) update uid of imported contact to 8
4) import vcard with no email fields and uid 8
5) check that all email fields are deleted
6) check that no other fields have been deleted

@SYMTestExpectedResults For the above tests:
It should be possible to delete multiple fields of the same type when a vcard is imported
*/

void VCardEmailTestL()
	{
	const TInt importOption =
		CContactDatabase::ELocalTime |
		CContactDatabase::EImportSingleContact | 
		CContactDatabase::ENullTemplateId;

	test.Next(_L("@SYMTestCaseID:PIM-T-CNTVCARD-DEF081712-0001 VCard Email Test"));
	//start with fresh/clean database
	CntTest->CreateDatabaseL();
	TInt count = CntTest->Db()->CountL();
	TEST_CONDITION(count == 0);
	
	test.Next(_L("Importing initial contact from 1.vcf"));
	TContactItemId contactId = ImportVCardL( KVcard1, importOption );
	TInt initialcount = ContactFilledFields(contactId);
	
	count = CntTest->Db()->CountL();
	TEST_CONDITION(count == 1);
	
	CContactItem *item = CntTest->Db()->OpenContactLX(contactId);
	CleanupStack::PushL( item );
	
	_LIT(guid, "8");
	item->SetUidStringL( const_cast<TDesC &>( guid() ) );
	CntTest->Db()->CommitContactL( *item );
	CleanupStack::PopAndDestroy(item);
	CleanupStack::Pop();//lock
	item = NULL;
	
	test.Next(_L("check number of email fields"));
 	TEST_CONDITION(ContactEmailCountL(contactId) == 3);
 	
 	test.Next(_L("reimport contact from 2.vcf"));
	contactId = ImportVCardL( KVcard2, importOption );
	count = CntTest->Db()->CountL();
	TEST_CONDITION(count == 1);
	
	TInt updatedcount = ContactFilledFields(contactId);
	
	test.Next(_L("check email fields are deleted"));
 	TEST_CONDITION(ContactEmailCountL(contactId) == 0);
 	
 	test.Next(_L("check no other fields are cleared or deleted"));
 	TEST_CONDITION( updatedcount == (initialcount - 3) );
	
	}
	
/**
@SYMTestCaseID PIM-T-CNTVCARD-DEF083613-0001
@SYMTestType UT
@SYMTestPriority High
@SYMDEF DEF083613
@SYMTestCaseDesc A VCard with lines terminated by LF(line-feed)s  only is not imported correctly.

@SYMTestActions
1) import a vcard containing only line-feed line terminators.
2) Check that the import was successful.

@SYMTestExpectedResults The vcard containing only LF line terminators is imported correctly.
*/

void VCardLineFeedOnlyTestL()
	{
	const TInt importOption =
		CContactDatabase::ELocalTime |
		CContactDatabase::EImportSingleContact | 
		CContactDatabase::ENullTemplateId;

	TRAPD(err, ImportVCardL( KVCardFileLF, importOption ));
	TEST_CONDITION(err==0);
	}
/**
@SYMTestCaseID PIM-T-CNTVCARD-DEF085873-0001
@SYMTestType UT
@SYMTestPriority High
@SYMDEF DEF085873
@SYMTestCaseDesc Check that use of the CContactVCardConverter::EIgnoreUid flag
behaves correctly.

@SYMTestActions
1) Empty database.
2) Import a vcard containing a UID without using the flag.
3) Check there is only one contact in the database.
4) Import the same vcard without using the flag.  The import should count as an
update of the existing contact item.
5) Check there is only one contact in the database.
6) Import the same vcard using the flag.  The import should count as an add of
a new item.
7) Check there are now 2 contacts in the database.

@SYMTestExpectedResults The vcard is added to the database a second time when
the CContactVCardConverter::EIgnoreUid flag is used.
*/
void VCardIgnoreUidTestL()
	{
	const TInt importOption1 = CContactDatabase::ELocalTime;

	const TInt importOption2 =
		CContactDatabase::ELocalTime |
		CContactVCardConverter::EIgnoreUid;

	test.Next(_L("@SYMTestCaseID:PIM-T-CNTVCARD-DEF085873-0001 Check that use of the CContactVCardConverter::EIgnoreUid flag behaves correctly."));
	
	// 1) Empty database.
	EmptyDatabase();
	TInt count = CntTest->Db()->CountL();
	TEST_CONDITION(count == 0);

	// 2) Import a vcard containing a UID without using the flag.
	TRAPD(err, ImportVCardL( KVCardFileUID, importOption1 ));	
	TEST_CONDITION(err==0);
		
	// 3) Check there is only one contact in the database.
	count = CntTest->Db()->CountL();
	TEST_CONDITION(count == 1);
	
	// 4) Import the same vcard without using the flag.  The import should count
	// as an update of the existing contact item.
	TRAP(err, ImportVCardL( KVCardFileUID, importOption1 ));	
	TEST_CONDITION(err==0);

	// 5) Check there is only one contact in the database.
	count = CntTest->Db()->CountL();
	TEST_CONDITION(count == 1);

	// 6) Import the same vcard using the flag.  The import should count as an
	// add of a new item.
	TRAP(err, ImportVCardL( KVCardFileUID, importOption2 ));	
	TEST_CONDITION(err==0);

	// 7) Check there are now 2 contacts in the database.
	count = CntTest->Db()->CountL();
	TEST_CONDITION(count == 2);
	} 
/*@SYMTestPriority High
@SYMDEF PDEF097999 
@SYMTestCaseDesc The identity table is not updated if we pass a null value for 
updation (as a result sorting goes wrong)
@SYMTestActions
1) Import a vcard containing firstname and lastname.
2) Import the same vcf file but without  having lastname for one entry
3) Check whether the old lastname is present for that entry.

@SYMTestExpectedResults The identity table must be updated even if we give a null value for lastname.
so if we search for the old lastname it should not be there .
*/
LOCAL_C void TestImportContactsL(const TDesC& aFileName1 , const TDesC& aFileName2 , const TDesC8& aVcfData1 , const TDesC8& aVcfData2)
	{
	test.Next(_L("checking  Updation of Identity table"));
	CContactDatabase *db=CntTest->CreateDatabaseL();	

	RFs fileSystem;
	User::LeaveIfError( fileSystem.Connect() );
	CleanupClosePushL( fileSystem );
	//Creating input Vcf files
	RFile file;
	file.Replace(fileSystem, aFileName1, EFileWrite+EFileShareAny+EFileStreamText);
	file.Write(aVcfData1);
	file.Close();
	
	file.Replace(fileSystem, aFileName2, EFileWrite+EFileShareAny+EFileStreamText);
	file.Write(aVcfData2);
	file.Close();
		
	RFileReadStream vcard;
	User::LeaveIfError(vcard.Open(fileSystem, aFileName1, EFileRead));
	CleanupClosePushL(vcard);
	TBool success=EFalse;
	CArrayPtr<CContactItem>* contactItems=db->ImportContactsL(TUid::Uid(KUidVCardConvDefaultImpl), vcard, success, CContactDatabase::ETTFormat);
	CleanupStack::PopAndDestroy(&vcard);
	contactItems->ResetAndDestroy();
	delete contactItems;
	
	User::LeaveIfError(vcard.Open(fileSystem, aFileName2, EFileRead));
	CleanupClosePushL(vcard);
	success=EFalse;
	contactItems=db->ImportContactsL(TUid::Uid(KUidVCardConvDefaultImpl), vcard, success, CContactDatabase::ETTFormat);
	CleanupStack::PopAndDestroy(&vcard);

	//Search for "Bbb" it must not be there
    CContactItemFieldDef* fieldDef = new(ELeave) CContactItemFieldDef;
	TFieldType fieldtype = KUidContactFieldFamilyName;
	fieldDef->AppendL(fieldtype);
	CContactIdArray* matchList = db->FindLC(_L("Bbb"), fieldDef);
	TInt matchCount = matchList->Count();
	//Count  zero means the vcard has been updated properly
	TEST_CONDITION(matchCount==0);
	test.Printf	(_L("Sucessfully modified identity table"));
	CleanupStack::PopAndDestroy(matchList);
	contactItems->ResetAndDestroy();
	delete contactItems;
	delete fieldDef;
	CntTest->CloseDatabase();
	CleanupStack::PopAndDestroy(&fileSystem);
	}

 CArrayPtr<CContactItem>* ImportContactsL(const TDesC8& aImportFileInputDescriptor, CContactDatabase* aDatabase)
 	{
 	CArrayPtr<CContactItem>* retval = NULL;
 	
 	// read the input contents 
 	RDesReadStream vCard(aImportFileInputDescriptor);
 	CleanupClosePushL(vCard);
 	
	//import
 	TBool success=EFalse;
	retval = aDatabase->ImportContactsL(TUid::Uid(KUidVCardConvDefaultImpl), vCard, success, CContactDatabase::EDefault);
 
 	CleanupStack::PopAndDestroy(&vCard);
 	return retval;
 	}

void TestStorageType( CContactItem *aContactItem, TStorageType aExpectedStorageType)
	{
	//verify test
 	TInt fieldIndex = aContactItem->CardFields().Find(KUidContactFieldVCardMapKEY);
 	TStorageType  storageType = aContactItem->CardFields()[fieldIndex].StorageType();
 	TEST_CONDITION(storageType == aExpectedStorageType);
 	test.Next(_L("ImportExportVCardWithKeyL passed"));
	}
	
 /**
 @SYMTestCaseID PIM-T-CNTVCARD-DEF097565-0001
 @SYMTestType UT
 @SYMTestPriority Medium
 @SYMDEF DEF097565
 @SYMTestCaseDesc Importr the vcf file and then check whether the BInary Key is imported 
 properly or not.

 @SYMTestActions
 1) Create an empty database.
 2) Import a vcard.
 3) Check there is Key field imported is having storage type KStorageTypeStore.
 
 @SYMTestExpectedResults The vcard is containing the Binary Key value.
 */	
 void ImportExportVCardWithKeyL(const TDesC8& aImportFileInputDescriptor, TStorageType aExpectedStorageType)
 	{
 	test.Next(_L("@SYMTestCaseID:PIM-T-CNTVCARD-DEF097565-0001 ImportExportVCardWithKeyL Begins"));
 
 	CntTest->CreateDatabaseL();
 	
 	//import
 	CArrayPtr <CContactItem>* contactItems = NULL;
 	contactItems = ImportContactsL(aImportFileInputDescriptor, CntTest->Db());
 
 	//verify test
 	TestStorageType((contactItems->At(0)), aExpectedStorageType);
 
 	// reclaim memory
 	contactItems->ResetAndDestroy() ;
 	delete contactItems;
 	
 	CntTest->CloseDatabase();
   	}
/**
@SYMTestCaseID PIM-T-CNTVCARD-INC096705-0001
@SYMTestType UT
@SYMTestPriority Medium
@SYMDEF INC096705
@SYMTestCaseDesc Check that use of the CContactVCardConverter::EReplaceIfExists flag
behaves correctly.

@SYMTestActions
1) Empty database.
2) Import a vcard containing a company field with EDefault flag.
3) Check of company field is imported
4) Import the same vcard without company field and using CContactVCardConverter::EReplaceIfExists option

@SYMTestExpectedResults The vcard is replaced with the contact in the database when
the CContactVCardConverter::EReplaceIfExists flag is used.
*/
void ImportVCardWithFlagReplaceIfExitstL()
{
	CContactDatabase *db=CntTest->CreateDatabaseL();
	test.Next(_L("@SYMTestCaseID:PIM-T-CNTVCARD-INC096705-0001 hecking  Replacing old contact with new one"));
	const TInt importOption1 = CContactDatabase::EDefault;
	const TInt importOption2 = CContactDatabase::EConverterReserved2;
	
	// 1) Empty database.
	EmptyDatabase();
	TInt count = db->CountL();
	TEST_CONDITION(count == 0);	

	// 2) Import a vcard containing company filed with CContactDatabase::EDefault flag.
	// and ensure company field gets imported
	test.Next(_L("Importing Contact with Company field"));
	TContactItemId aContactId = ImportVCardL( KVCardWithCompany, importOption1 );		
	CContactItem *item = db->ReadContactLC(aContactId);
	CContactItemFieldSet& fieldSet = item->CardFields();	
	TInt companyIndex = fieldSet.Find( KUidContactFieldCompanyName );
	TEST_CONDITION(companyIndex  != KErrNotFound );	
	const CContactItemField& field = (fieldSet)[companyIndex];
	TEST_CONDITION(field.Storage()->IsFull() != 0);
	CContactTextField* textfield = field.TextStorage();
	_LIT(KExpectedNumber,"XYZ ltd");
	TEST_CONDITION(textfield->Text()==KExpectedNumber);				
	CleanupStack::PopAndDestroy(item);	
	
	//3)Import a vcard without containing company filed with CContactDatabase::EDefault flag.
	// and ensure company field gets merged
	test.Next(_L("Importing Same Contact and merging the old contact without company field"));
	TContactItemId aContactId1 = ImportVCardL( KVCardWithoutCompany, importOption1 );
	
	//4) Ensure that company field should be there
	CContactItem *item1 = db->ReadContactLC(aContactId1);
	CContactItemFieldSet& fieldSet1 = item1->CardFields();
	TInt companyIndex1 = fieldSet1.Find( KUidContactFieldCompanyName );
	TEST_CONDITION(companyIndex1  != KErrNotFound );			
	const CContactItemField& field1 = (fieldSet1)[companyIndex1];
	TEST_CONDITION(field1.Storage()->IsFull() != 0);
	CContactTextField* textfield1 = field1.TextStorage();
	_LIT(KExpectedNumber1,"XYZ ltd");
	TEST_CONDITION(textfield1->Text()==KExpectedNumber1);			
	
	CleanupStack::PopAndDestroy(item1);
	
	//5) Import the same vcard without company field and using CContactVCardConverter::EReplaceIfExists option
	test.Next(_L("Importing Same Contact and replacing with the old contact without company field"));
	TContactItemId aContactId2 = ImportVCardL( KVCardWithoutCompany, importOption2 );	
	
	//6) Ensure that company field should not be there	
	CContactItem *item2 = db->ReadContactLC(aContactId2);
	CContactItemFieldSet& fieldSet2 = item2->CardFields();
	TInt companyIndex2 = fieldSet2.Find( KUidContactFieldCompanyName );		
	const CContactItemField& field2 = (fieldSet2)[companyIndex2];	
	TEST_CONDITION(field2.Storage()->IsFull() == 0);
	CleanupStack::PopAndDestroy(item2);
}

 /**
 @SYMTestCaseID PIM-T-CNTVCARD-INC110795-0001
 @SYMTestType UT
 @SYMTestPriority Medium
 @SYMDEF INC110795
 @SYMTestCaseDesc Check the use of import option with CContactVCardConverter::EReplaceIfExists flag
 behaves correctly with duplicate fields.
 
 @SYMTestActions
 1) Empty database.
 2) Import a vcard containing 2 Home Tel fields with EDefault flag.
 3) Check of all fields are imported
 4) Import the same vcard with 4 modified Home Tel fields  and using CContactVCardConverter::EReplaceIfExists option
 
 @SYMTestExpectedResults The vcard is replaced with the contact in the database when
 the CContactVCardConverter::EReplaceIfExists flag is used.
 */
 void ImportDuplicateFieldVCardWithReplaceIfExitstL()
 {
 
 	test.Next(_L("@SYMTestCaseID:PIM-T-CNTVCARD-INC110795-0001 checking Replacing existing contact with new one (has duplicate fields) using EReplaceIfExists"));
 	
 	const TInt importOption1 = CContactDatabase::EDefault;
 	const TInt importOption2 = CContactDatabase::EDefault|
 								CContactDatabase::EImportSingleContact|
 								CContactDatabase::ENullTemplateId |
 								CContactVCardConverter::EReplaceIfExists;  
 
 								
 	CContactDatabase *db=CntTest->CreateDatabaseL();
 
 	// 1) Empty database.
 	EmptyDatabase();
 	TInt count = db->CountL();
 	test(count == 0);
 	
 	// 2) Import a vcard containing 2 Home Tel fields with CContactDatabase::EDefault flag.
 	test.Next(_L("Importing Contact with 2 duplicate Home Tel fields"));
 	TContactItemId contactId;
 	
 	RDesReadStream vcard(KVCardBeforeChange());
 	CleanupClosePushL(vcard);
 
 	CArrayPtr<CContactItem>* contactItems;
 	TBool success = EFalse;
 	contactItems = db->ImportContactsL(TUid::Uid(KUidVCardConvDefaultImpl), vcard, success, importOption1);
 	CleanupStack::PushL(TCleanupItem(CleanUpResetAndDestroy,contactItems));
 	contactId = (*contactItems)[0]->Id();
 	CleanupStack::PopAndDestroy(contactItems);
 	CleanupStack::PopAndDestroy(&vcard);
 	
 	CContactItem *item = db->ReadContactLC(contactId);
 	CContactItemFieldSet& fieldSet = item->CardFields();	
 
 	// Ensure all fields get imported
 	test(FindInFields(fieldSet, TPtrC16(KGivenName())));
 	test(FindInFields(fieldSet, TPtrC16(KFamilyName())));
 	test(FindInFields(fieldSet, TPtrC16(KTelHome1())));
 	test(FindInFields(fieldSet, TPtrC16(KTelHome2())));
 	test(FindInFields(fieldSet, TPtrC16(KTelCell())));
 
 	CleanupStack::PopAndDestroy(item);	
 
 
 	//3) Import the same vcard with 4 modified Home tel fields and using 
 	//CContactVCardConverter::EReplaceIfExists option
 	test.Next(_L("Importing Same Contact and replacing the old contact with 4 modified Home Tel field using EReplaceIfExists"));
 
 	RDesReadStream vcard2(KVCardAfterChange());
 	CleanupClosePushL(vcard2);
 	
 	success = EFalse;
 	contactItems = db->ImportContactsL(TUid::Uid(KUidVCardConvDefaultImpl), vcard2, success, importOption2);
 	CleanupStack::PushL(TCleanupItem(CleanUpResetAndDestroy,contactItems));
 	contactId = (*contactItems)[0]->Id();
 	CleanupStack::PopAndDestroy(contactItems);
 	CleanupStack::PopAndDestroy(&vcard2);
 	
 	item = db->ReadContactLC(contactId);
 
 	CContactItemFieldSet& fieldSet2 = item->CardFields();
 
 	//4) Ensure that the all 4 Home TEL field have been modified 	
 	test(FindInFields(fieldSet2, TPtrC16(KGivenName())));
 	test(FindInFields(fieldSet2, TPtrC16(KFamilyName())));
 	test(FindInFields(fieldSet2, TPtrC16(KTelHome1Modified())));
 	test(FindInFields(fieldSet2, TPtrC16(KTelHome2Modified())));
 	test(FindInFields(fieldSet2, TPtrC16(KTelHome3Modified())));
 	test(FindInFields(fieldSet2, TPtrC16(KTelHome4Modified())));
 	
 	CleanupStack::PopAndDestroy(item);
 	}
   	
/**
Utility function to check if a specific line is present in a file
*/	
TBool CheckExportFilesL(const TDesC8& aString, const TDesC16& aFile)
	{
	RFile fileHandle;
	TBool patternFound = EFalse;
	fileHandle.Open(CntTest->Fs(), aFile, EFileRead|EFileStreamText);
	CleanupClosePushL(fileHandle);
	TBuf8<256> line;
	
	while(fileHandle.Read(line) == KErrNone && line.Length() != 0)
		{
		if (line.Find(aString) != KErrNotFound)
			{
		    patternFound = ETrue;
		    break;	
			}	
		}
	
	CleanupStack::PopAndDestroy(&fileHandle);
	return patternFound;
	}

/**
@SYMTestCaseID PIM-T-CNTVCARD-INC102410-0001
@SYMTestType UT
@SYMTestPriority Critical
@SYMTestCaseDesc Properties having multiple parameters should be imported properly.

@SYMTestActions
1.Import vCard having property with multiple parameter.
2.Open imported contact and check if parameter values have been added as fields.
3.Export imported contact and check if the property has been exported properly.

@SYMTestExpectedResults For the above tests:
1.vCard should be imported without losing data.
2.Necessary parameters should be added as fields in contact.
3.Exported file should contain all the parameters which were there while importing.
4.No leaves or panics should occur
*/	
void TestMultipleParamsInVCardPropertyL()
	{
	test.Next(_L("@SYMTestCaseID:PIM-T-CNTVCARD-INC102410-0001 Test Import of property having multiple parameters"));
	CntTest->CreateDatabaseL();
	TContactItemId contactId;
	
	RDesReadStream vcard(KMultiParam());
	CleanupClosePushL(vcard);

	CArrayPtr<CContactItem>* contactItems;
	TBool success = EFalse;
	contactItems = CntTest->Db()->ImportContactsL(TUid::Uid(KUidVCardConvDefaultImpl), vcard, success, CContactDatabase::ETTFormat);
	CleanupStack::PushL(TCleanupItem(CleanUpResetAndDestroy,contactItems));
	contactId = (*contactItems)[0]->Id();
	CleanupStack::PopAndDestroy(contactItems);
	CleanupStack::PopAndDestroy(&vcard);
	
	CContactItem* item = CntTest->Db()->ReadContactLC(contactId);
	
	CContactItemFieldSet& fields = item->CardFields();
	TInt fieldPos = fields.FindNext(KUidContactFieldVCardMapTEL, KContactFieldSetSearchAll);
	const CContentType& content = fields[fieldPos].ContentType();
	
	TBool result = 0;
	result = content.ContainsFieldType(KUidContactFieldVCardMapCELL);
	TEST_CONDITION(result > 0);
	
	result = content.ContainsFieldType(KUidContactFieldVCardMapVOICE);
	TEST_CONDITION(result > 0);
	
	result = content.ContainsFieldType(KUidContactFieldVCardMapHOME);
	TEST_CONDITION(result > 0);
	  
	CleanupStack::PopAndDestroy(item);	//destroying contact item
	
	CContactIdArray* idArray = CContactIdArray::NewL();
	CleanupStack::PushL(idArray);
	idArray->AddL(contactId);

	RFileWriteStream fileStream;
	User::LeaveIfError(fileStream.Replace(CntTest->Fs(), KOutputFileName, EFileWrite));
	CleanupClosePushL(fileStream);

	CntTest->Db()->ExportSelectedContactsL(TUid::Uid(KUidVCardConvDefaultImpl), *idArray, fileStream,0);
	fileStream.CommitL();
	CleanupStack::PopAndDestroy(&fileStream);
	
	result = CheckExportFilesL(KTelExport(), KOutputFileName());
	TEST_CONDITION(result > 0);
	
	
	CleanupStack::PopAndDestroy();	//destroying id array
	}
	
/**
@SYMTestCaseID PIM-T-CNTVCARD-PDEF103252-0001
@SYMTestType UT
@SYMTestPriority High
@SYMTestCaseDesc Contact item having a large binary field should be exported properly without any leaves.

@SYMTestActions
1.Import vCard having PHOTO propery with very large property value.
2.Export the imported contacts.
3.Import the exported contacts.
4.Compare the value of the PICTURE field present in the contact items resultin from two Import actions performed.

@SYMTestExpectedResults For the above tests:
1.vCard should be Exported without losing data or causing any low memory leaves.
2.The Imported PHOTO data should be same in both cases.
.No leaves or panics should occur
*/
void TestExportofContactHavingLargeBinaryFieldL()
	{
	test.Next(_L("@SYMTestCaseID:PIM-T-CNTVCARD-PDEF103252-0001 Test Export of contact having large binary field"));
	
	//	Empty the Database
	CntTest->CreateDatabaseL();
	CContactIdArray* idarray = CContactIdArray::NewL();
	CleanupStack::PushL(idarray);
	RFs fsSession;
	User::LeaveIfError(fsSession.Connect());
	RFileReadStream readStream;

 	//Import vcard with large PHOTO property
 	test.Printf(_L("Importing contacts from VCard"));
	User::LeaveIfError(readStream.Open(fsSession, KVCardLargePhoto, EFileRead));
 	CleanupClosePushL(readStream);
 	TBool success = EFalse;
 	CArrayPtr<CContactItem>* contactItems = CntTest->Db()->ImportContactsL(TUid::Uid(KUidVCardConvDefaultImpl),
 	 	 readStream, success, CContactDatabase::ETTFormat);
 	 
	CleanupStack::PushL( TCleanupItem( CleanUpResetAndDestroy, contactItems ) );

	TEST_CONDITION(contactItems->Count() == 1) ;

 	//adding imported contact Id's to idarray for export
	idarray->AddL((*contactItems)[0]->Id());

	CleanupStack::PopAndDestroy(contactItems);
 	CleanupStack::PopAndDestroy(&readStream);

	test.Printf(_L("Exporting the Imported contacts, Testing it for both vCard2.1 and vCard3.0 export."));
	RFileWriteStream writeStream;
	TUid uid;

	//vCard3.0
	User::LeaveIfError(writeStream.Replace(fsSession,KVCardFile2, EFileWrite));
 	CleanupClosePushL(writeStream);
	uid.iUid = KUidPBAPVCardConvImpl;
	TInt64 filter = 0;
	filter |= 0x08;
	//This call should not leave with a Low memory Error
	CntTest->Db()->ExportSelectedContactsL(uid, *idarray, writeStream, CContactDatabase::EDefault, filter, NULL, EPBAPVCard30,ETrue);
 	writeStream.CommitL();
	CleanupStack::PopAndDestroy(&writeStream);
	
	//vCard2.1
	User::LeaveIfError(writeStream.Replace(fsSession, KVCardFile3, EFileWrite));
 	CleanupClosePushL(writeStream);
	uid.iUid = KUidVCardConvDefaultImpl;
	//This call should not leave with a Low memory Error
	CntTest->Db()->ExportSelectedContactsL(uid, *idarray, writeStream, CContactDatabase::EDefault);
 	writeStream.CommitL();
	CleanupStack::PopAndDestroy(&writeStream);
	CleanupStack::PopAndDestroy(idarray);
	
	test.Printf(_L("Export of large binary property completed successfully"));		

}

/**
@SYMTestCaseID PIM-T-CNTVCARD-PDEF140328-0001
@SYMTestType UT
@SYMTestPriority High
@SYMTestCaseDesc Partial vCard's should be processed without any panics

@SYMTestActions
1.Import a partial vCard.

@SYMTestExpectedResults For the above tests:
1.There should be no panics while import is happening.
*/
 void TestImportingPartialOrEmptyVCardsL()
{
    test.Next(_L("Test import of partial vCard"));
    CArrayPtr<CContactItem>* contactItems = NULL;
    RDesReadStream vcard(KPartialVCard());
    CleanupClosePushL(vcard);
    TInt success = EFalse;
    CContactDatabase* db = CntTest->CreateDatabaseL();
    contactItems = db->ImportContactsL(TUid::Uid(KUidVCardConvDefaultImpl), vcard, success, CContactDatabase::EImportSingleContact);
    CleanupStack::PushL( TCleanupItem( CleanUpResetAndDestroy, contactItems ) );
    CleanupStack::PopAndDestroy(contactItems);
    CleanupStack::PopAndDestroy(&vcard);
    test.Printf(_L("Import of partial vCard completed successfully"));
}


/**

@SYMTestCaseID     PIM-T-CNTVCARD-0001

*/

LOCAL_C void DoTestsL()
	{
	test.Start(_L("@SYMTestCaseID:PIM-T-CNTVCARD-0001 Preparing tests"));
	CTestRegister* TempFiles = CTestRegister::NewLC();
	TempFiles->RegisterL(KVCardFile1);
	TempFiles->RegisterL(KVCardFile2);
	TempFiles->RegisterL(KVCardFile3);
	TempFiles->RegisterL(KVCardUpdate2);
	TempFiles->RegisterL(KDatabaseFileName, EFileTypeCnt);
	TempFiles->RegisterL(KOutputFileName);
	TempFiles->RegisterL(KExpDelAFileName);
	TempFiles->RegisterL(KImpTypeVcardName);
	TempFiles->RegisterL(KImpNoTypeVcardName);	
	TempFiles->RegisterL(KVCardFile5);
	TempFiles->RegisterL(KVCardFile6);

	//without the fix this test will cause User 21 panic in versit 
	TestImportingPartialOrEmptyVCardsL();
	VCardEmailTestL();
	AccessCountTestsL();			
	DefaultVoiceParamTestsL();
	ExportContactTestL();
	ExportNamelessContactTestL();
#if defined(_DEBUG)
	//this test cannot be run in release mode due to UID mismatch
	//UpdateVCardTestL();
#endif			
	
 	ImportLargeVCardTestL() ;			

	DoTrailingSpaceImportAndExportTestL() ;
	VCardLineFeedOnlyTestL();
	VCardIgnoreUidTestL();
	CheckTypeSupport(KImpTypeVcardName);
	CheckTypeSupport(KImpNoTypeVcardName);	
	TestImportContactsL(KVCardFile5, KVCardFile6, KMasterVcard, KModifierVcard);
	// ImportExportVCardWithBinaryKeyL called with binary KEY value
	ImportExportVCardWithKeyL(KVCardBinaryKeyImportFileDes, KStorageTypeStore);
	// ImportExportVCardWithBinaryKeyL called with text KEY value
	ImportExportVCardWithKeyL(KVCardTextKeyImportFileDes, KStorageTypeText);
	ImportVCardWithFlagReplaceIfExitstL();
	ImportDuplicateFieldVCardWithReplaceIfExitstL();

	TestMultipleParamsInVCardPropertyL();
	TestExportofContactHavingLargeBinaryFieldL();

	CntTest->CloseDatabase();
	CleanupStack::PopAndDestroy(TempFiles);
	}


LOCAL_C void CleanupFilesL()
	{
    // delete the database file
	if (CContactDatabase::ContactDatabaseExistsL(KDatabaseFileName) )
		{
		CContactDatabase::DeleteDatabaseL(KDatabaseFileName);
		}
	}


LOCAL_C void TestCondition(TBool aCondition, TInt aLineNumber)
	{
	// if the test is about to fail, cleanup files first
	if (!aCondition)
		{
		TRAP_IGNORE(CleanupFilesL());
		}
	test.operator()(aCondition, aLineNumber);
	}


GLDEF_C TInt E32Main()
	{
    __UHEAP_MARK; 
    CntTest=new(ELeave) CCntTest();
	CntTest->ConstructL(test,KDatabaseFileName);
    TRAPD(err,DoTestsL());
    // ensure files are cleaned up even if DoTestsL() leaves
    TRAP_IGNORE(CleanupFilesL() ); 
    CntTest->EndTestLib(err);
    __UHEAP_MARKEND;	
	return KErrNone;
    }
