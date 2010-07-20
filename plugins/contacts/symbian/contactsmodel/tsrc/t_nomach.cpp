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


#include <e32test.h>
#include <f32file.h>
#include "t_utils.h"
#include <coreappstest/testserver.h>

_LIT(KCntTestTitle,"T_NOMACH");
_LIT(KDatabaseFileName,"c:T_NOMACH");

#ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__ 
_LIT(KSrcDatabaseName, "z:\\t_old_contacts_original.cdb");
_LIT(KDatabaseName, "C:\\private\\10003A73\\t_old_contacts_in_use.cdb");
_LIT(KDatabaseOpenName, "C:t_old_contacts_in_use.cdb");
_LIT(KTestDbUpdate, "-- Test database update\n");
#endif //__SYMBIAN_CNTMODEL_USE_SQLITE__

_LIT(KTestName,"Test Name No %d");
_LIT(KTestAddress,"Test Address No %d");
_LIT(KTestFamilyName,"Test Family Name No %d");
_LIT(KTestEmail,"Test@Email.com No %d");
_LIT(KTestCountry,"Test Country No %d");
_LIT(KTestCompany,"Test Company No %d");
_LIT(KCntName,"Firstname");
_LIT(KCntSurname,"Surname");

_LIT(KTestNoPhoneMatchLibrary,"-- Regression testcode for DUG-4ZVNBW - MatchPhoneNumberL doesn't match correctly without CNTPHONE.DLL\n");
_LIT(KTestNewMatching,"-- Test matching; all following tests should match 1234567\n");
_LIT(KTestFaxSMSMatching,"-- Test adding fax and SMS numbers to the phone number match list, regression for COY-567J99\n");
_LIT(KSpeedDialSimpleTest,"-- Basic speed-dial tests.\n");
_LIT(KTestRemovingSpeedDials,"\n-- Test removing speed-dials; add five contacts, Set two speed dials - remove one & check the other is still there.\n");
_LIT(KTest11Digitmatching, "-- Test 11 digit matching\n");
_LIT(KTest15Digitmatching, "-- Test 15 digit matching\n");
_LIT(KTestEmbeddedZeroes, "-- Test Embedded Zeroes\n");

const TInt KMatch6Digits = 6;
const TInt KMatch7Digits = 7;
const TInt KMatch8Digits = 8;
const TInt KMatch10Digits = 10;
const TInt KMatch11Digits = 11;
const TInt KMatch12Digits = 12;
const TInt KMatch13Digits = 13;
const TInt KMatch14Digits = 14;
const TInt KMatch15Digits = 15;
const TInt KMatch16Digits = 16;

const TInt KTestContactsNum=10;

GLDEF_D CCntTest* CntTest=NULL;
GLDEF_D RTest test(KCntTestTitle);


LOCAL_C void SetNameL(CContactItem& aItem,TUid aType,const TDesC& aName, TBool aAddField)
//
// Set the contents of a text field, creating the field if required
//
	{
	CContactItemFieldSet& fieldSet=aItem.CardFields();
	const TInt pos=fieldSet.Find(aType);
	if (!aAddField && pos!=KErrNotFound)
		fieldSet[pos].TextStorage()->SetTextL(aName);
	else
		{
		CContactItemField* field=CContactItemField::NewLC(KStorageTypeText,aType);
   		field->SetMapping(KUidContactFieldVCardMapUnusedN);
		field->TextStorage()->SetTextL(aName);
		aItem.AddFieldL(*field);
		CleanupStack::Pop(field); 
		}
	}

LOCAL_C void PopulateDatabaseL(TInt aNumberToPopulate)
//
// Create and populate the database
//
	{
	CContactItem* templ = CntTest->Db()->ReadContactL(0);
	CleanupStack::PushL(templ);
	
	for (TInt ii=0;ii<aNumberToPopulate;ii++)
		{
		CContactItem* item=CContactCard::NewLC(templ);
		TBuf<20> name;
		name.Format(KTestName,ii);
   		SetNameL(*item,KUidContactFieldGivenName,name,EFalse);
		TBuf<20> number;
		switch(ii%3)
			{
			case 0:
				number.Format(_L("0171-%03d %04d"),(ii*9)%1000,((ii+11)*23)%10000);
				break;
			case 1:
				number.Format(_L("%04d:%04d:%04d:%04d"),(ii*123)%10000,(ii*666)%10000,(ii*234)%10000);
				break;
			case 2:
				number.Format(_L("+00%d-%03d %04d"),(ii*123)%100,(ii*13)%1000,((ii+13)*17)%10000);
				break;
			}
   		SetNameL(*item,KUidContactFieldPhoneNumber,_L("11111"),EFalse);
		if (!(ii%2))
			{
			number.Format(_L("0181-%03d %04d"),(ii*8)%1000,((ii+11)*22)%10000);
	   		SetNameL(*item,KUidContactFieldPhoneNumber,_L("111"),EFalse); 
			number.Format(_L("01734-%06d"),(ii*123456)%1000000);
	   		SetNameL(*item,KUidContactFieldPhoneNumber,_L("1111"),EFalse);
			CContactItemFieldSet& itemFields = item->CardFields();
			for (TInt ii=0;ii<itemFields.Count();ii++)
				{
				CContactItemField& field = (itemFields)[ii];
				if (!itemFields[ii].Storage()->IsFull() && itemFields[ii].StorageType()==KStorageTypeText && 
					field.ContentType().ContainsFieldType(KUidContactFieldPhoneNumber))
					itemFields[ii].TextStorage()->SetTextL(_L("111111"));
				}
			}
		TBuf<32> address;
		address.Format(KTestAddress,ii);
		SetNameL(*item,KUidContactFieldAddress,address,EFalse);
//
		TBuf<32> familyName;
		familyName.Format(KTestFamilyName,ii);
		SetNameL(*item,KUidContactFieldFamilyName,familyName,EFalse);
//
		TBuf<32> email;
		email.Format(KTestEmail,ii);
		SetNameL(*item,KUidContactFieldEMail,email,EFalse);
//
		TBuf<32> country;
		country.Format(KTestCountry,ii);
		SetNameL(*item,KUidContactFieldCountry,country,EFalse);
//	
		TBuf<32> company;
		company.Format(KTestCompany,ii);
		SetNameL(*item,KUidContactFieldCompanyName,company,EFalse);
//	
		CntTest->Db()->AddNewContactL(*item); 
		CleanupStack::PopAndDestroy(item); 

		test.Printf(_L("."));	// Just to show some life
		}
	CleanupStack::PopAndDestroy(templ);
	CntTest->Db()->CompactL();
	}






// Delete the current contacts database and create a new one
// 
LOCAL_C void ResetDatabaseL()
	{
	// Reset all server side speed dials ;-))) 
	CntTest->Db()->ResetServerSpeedDialsL();

	CntTest->CloseDatabase();
	CntTest->DeleteDatabaseL();
	CntTest->CreateDatabaseL();
	CntTest->OpenDatabaseL();	
	}


LOCAL_C TContactItemId CreateContactL(const TDesC& aFirstName, const TDesC& aFamilyName, const TDesC& aPhoneNo, const TDesC& aMobileNo)
	{
	test.Printf(_L("\tCreating contact %S %S, phone: %S mobile: %S\n"), &aFirstName, &aFamilyName, &aPhoneNo, &aMobileNo);
	CContactItem* item=CContactCard::NewLC();
	SetNameL(*item,KUidContactFieldGivenName,KUidContactFieldVCardMapUnusedN,aFirstName,ETrue);
	SetNameL(*item,KUidContactFieldFamilyName,KUidContactFieldVCardMapUnusedN,aFamilyName,ETrue);
	SetNameL(*item,KUidContactFieldPhoneNumber,KUidContactFieldVCardMapTEL,aPhoneNo,ETrue);
	SetNameL(*item,KUidContactFieldPhoneNumber,KUidContactFieldVCardMapCELL,aMobileNo,ETrue);
	item->SetTemplateRefId(KGoldenTemplateId);
	TContactItemId id=CntTest->Db()->AddNewContactL(*item);
	CleanupStack::PopAndDestroy(item); 
	return id;
	}

LOCAL_C void CheckPhoneMatchL(const TDesC& aPhoneNumberToMatch, TInt aNumberOfMatchDigits, TInt aExpectedNumOfMatches)
	{
	TBuf<256> testCase;
	_LIT(KFormat,"Checking %d matches to %S, matching %d digits");
	testCase.Format(KFormat, aExpectedNumOfMatches, &aPhoneNumberToMatch, aNumberOfMatchDigits);
	test.Next(_L(" "));

	CContactDatabase& db = *(CntTest->Db());
	CContactIdArray* array = db.MatchPhoneNumberL(aPhoneNumberToMatch,aNumberOfMatchDigits);
	CleanupStack::PushL(array);
	const TInt numberOfMatches = array->Count();
	CleanupStack::PopAndDestroy(array);
	
	
	TInt sfd = db.MachineId();
	// OverrideMachineUniqueId
	
	
	test(numberOfMatches==aExpectedNumOfMatches);
	}

/** 
Testcode for Graham's new API

These all match 1234567:
*#42# 0401234567 p123
*#42# +358401234567 p123
*61 0401234567
*61 +358401234567
+358401234567 +3
+358401234567 p123

*/
LOCAL_C void TestNewMatchingL()
	{
	ResetDatabaseL();

	const TInt KLastThreeDigits =3;
	//These numbers match 1234567
	_LIT(KCntDTMFNumber1,"*#42# 0401234567 p123");
	_LIT(KCntDTMFNumber2,"*#42#+358401234567 p123");
	_LIT(KCntDTMFNumber3,"*61 0401234567");
	_LIT(KCntDTMFNumber4,"*61 +358401234567");
	_LIT(KCntDTMFNumber5,"+358401234567 +3");
	_LIT(KCntDTMFNumber6,"+358401234567 p123");
	_LIT(KCntMatch1to7ExtraCharacters,"mobile 12 345-67");
	_LIT(KCntMatch1to7Brackets,"(020) 0123-4567");
	_LIT(KCntMatch1to7TooManyNumbers,"123456789012345678901234567");
	//These shouldn't match 1234567
	_LIT(KCntDTMFNumber7,"*#42# 0401234567#p123");//empty
	_LIT(KCntDTMFNumber8,"*12345+0401234567");    //matches 12345
	_LIT(KCntDTMFNumber9,"*+123+456+++++++++++"); //matches 123
	//Graham's possible defects
	_LIT(KCntPossibleDefect,"70000");
	_LIT(KCntZeroAtEnd,"020 5632700");
	//Match numbers
	_LIT(KCntMatch1to7,"1234567");
	_LIT(KCntMatch1to3,"123");
	_LIT(KCntMatch700,"700");
	//Spaces at the start
	_LIT(KCntSpaceInNumber,"   0 2 0      7 1 2 0  5 6 7 8    ");
	_LIT(KCntSpaceMatch,"02071205678");
	_LIT(KCntSpaceInNumberIntl,"   +44 7447 065 472");
	_LIT(KCntSpaceInNumberIntlMatch,"+447747065472");
	_LIT(KCntSpaceWorstCase,"              ");
	_LIT(KCntNoRealDigitsInNumber,"Lycos SMS");

	_LIT(KCntShortNumber1,"789");
	_LIT(KCntShortNumber2,"456");

	_LIT(KCntEndingByShortNumber1,"+44(0)32465764789");
	_LIT(KCntEndingByShortNumber2,"+443216568731456");

	_LIT(KCntMatchShortNumber1,"789"); // must not match other number ending with 789
	_LIT(KCntMatchShortNumber2,"456"); // must not match other number ending with 456

	CreateContactL(KCntName,KCntSurname,KCntShortNumber1,KNullDesC);
	CreateContactL(KCntName,KCntSurname,KCntShortNumber2,KNullDesC);
	CreateContactL(KCntName,KCntSurname,KCntEndingByShortNumber1,KNullDesC);
	CreateContactL(KCntName,KCntSurname,KCntEndingByShortNumber2,KNullDesC);
	

	CreateContactL(KCntName,KCntSurname,KCntDTMFNumber1,KNullDesC);
	CreateContactL(KCntName,KCntSurname,KCntDTMFNumber2,KNullDesC);
	CreateContactL(KCntName,KCntSurname,KCntDTMFNumber3,KNullDesC);
	CreateContactL(KCntName,KCntSurname,KCntDTMFNumber4,KNullDesC);
	CreateContactL(KCntName,KCntSurname,KCntDTMFNumber5,KNullDesC);
	CreateContactL(KCntName,KCntSurname,KCntDTMFNumber6,KNullDesC);
	CreateContactL(KCntName,KCntSurname,KCntDTMFNumber7,KNullDesC);
	CreateContactL(KCntName,KCntSurname,KCntDTMFNumber8,KNullDesC);
	CreateContactL(KCntName,KCntSurname,KCntDTMFNumber9,KNullDesC);
	CreateContactL(KCntName,KCntSurname,KCntPossibleDefect,KNullDesC);
	CreateContactL(KCntName,KCntSurname,KCntMatch1to7ExtraCharacters,KNullDesC);
	CreateContactL(KCntName,KCntSurname,KCntMatch1to7Brackets,KNullDesC);
	CreateContactL(KCntName,KCntSurname,KCntMatch1to7TooManyNumbers,KNullDesC);
	TContactItemId id = CreateContactL(KCntName,KCntSurname,KCntZeroAtEnd,KNullDesC);

	CheckPhoneMatchL(KCntMatch1to7,KMatch7Digits,9);
	CheckPhoneMatchL(KCntMatch1to7,KLastThreeDigits,0);	// no contact with 567 as phone number  
	CheckPhoneMatchL(KCntMatch1to7,KMatch7Digits,9);
	CheckPhoneMatchL(KCntMatch1to7,KLastThreeDigits,0); // no contact with 567 as phone number  
	
	CheckPhoneMatchL(KCntMatch1to3,KMatch7Digits,1);
	CheckPhoneMatchL(KCntMatch1to3,KMatch7Digits,1);

	CheckPhoneMatchL(KCntPossibleDefect,KMatch7Digits,1);
	CheckPhoneMatchL(KCntZeroAtEnd,KMatch7Digits,1);
	CheckPhoneMatchL(KCntMatch700,KLastThreeDigits,0);  // no contact with 700 as phone number at this point 
	CheckPhoneMatchL(KCntZeroAtEnd,KMatch7Digits,1);
		
	CheckPhoneMatchL(KCntNoRealDigitsInNumber,KMatch7Digits,0);
	
	CheckPhoneMatchL(KCntMatchShortNumber1,KLastThreeDigits,1); //
	CheckPhoneMatchL(KCntMatchShortNumber2,KLastThreeDigits,1); //

	// Simulate a synchronisation (increment the access count)
	CContactItem* item = CntTest->Db()->OpenContactLX(id);
	item->IncAccessCount();
	CntTest->Db()->CommitContactL(*item);
	CleanupStack::PopAndDestroy(); //lock record
	delete item;
	
	// Delete the contact and verify that it has been removed from the phone match table 
	CntTest->Db()->DeleteContactL(id);
	CheckPhoneMatchL(KCntZeroAtEnd,KMatch7Digits,0);

	CreateContactL(KCntName,KCntSurname,KCntSpaceInNumber,KNullDesC);
	CheckPhoneMatchL(KCntSpaceMatch,KMatch7Digits,1);	
	CreateContactL(KCntName,KCntSurname,KCntSpaceInNumberIntl,KNullDesC);
	CheckPhoneMatchL(KCntSpaceInNumberIntlMatch,KMatch7Digits,1);	
	CreateContactL(KCntName,KCntSurname,KCntSpaceWorstCase,KNullDesC);
	}


// Regression testcode for 
// DUG-4ZVNBW "MatchPhoneNumberL doesn't match correctly without CNTPHONE.DLL"
//
// Remove CNTPHONE.DLL before running this test to verify
//
LOCAL_C void TestNoPhoneMatchLibraryL()
	{
	ResetDatabaseL();
	_LIT(KCntNumberWithSpaces,"+44  020 7563 2000");
	_LIT(KCntNumberWithoutSpaces,"02075632000");
	CreateContactL(KCntName,KCntSurname,KCntNumberWithSpaces,KNullDesC);
	CheckPhoneMatchL(KCntNumberWithoutSpaces,KMatch7Digits,1);
	}


LOCAL_C TContactItemId CreateFaxSMSContactL(const TDesC& aFirstName, const TDesC& aFamilyName, const TDesC& aFaxNo, const TDesC& aSMSNo)
	{
	CContactItem* item=CContactCard::NewLC();
	SetNameL(*item,KUidContactFieldGivenName,KUidContactFieldVCardMapUnusedN,aFirstName,ETrue);
	SetNameL(*item,KUidContactFieldFamilyName,KUidContactFieldVCardMapUnusedN,aFamilyName,ETrue);
	SetNameL(*item,KUidContactFieldFax,KUidContactFieldVCardMapFAX,aFaxNo,ETrue);
	SetNameL(*item,KUidContactFieldSms,KUidContactFieldVCardMapTEL,aSMSNo,ETrue);
	item->SetTemplateRefId(KGoldenTemplateId);
	TContactItemId id=CntTest->Db()->AddNewContactL(*item);
	CleanupStack::PopAndDestroy(item); 
	return id;
	}


/** 
Regression testcode for defect COY-567J99 "Phone number matching does not match Fax field"
Testcode for adding fax and SMS numbers to the phone number match list
*/
LOCAL_C void TestFaxSMSMatchingL()
	{
	ResetDatabaseL();

	_LIT(KCntFaxNumber1,"020 85858585");
	_LIT(KCntFaxNumber2,"020 85858432");
	_LIT(KCntSMSNumber1,"+442085858432");
	_LIT(KCntSMSNumber2,"+442085858585");
	
	_LIT(KCntDuplicateNumber,"0208 5858585");
	_LIT(KCntNewSMSNumber,"020 85969696");
	
	TContactItemId one = CreateFaxSMSContactL(KCntName,KCntSurname,KCntFaxNumber1,KCntSMSNumber1);
	TContactItemId two = CreateFaxSMSContactL(KCntName,KCntSurname,KCntFaxNumber2,KCntSMSNumber2);
	
	//check that there are only 2 matches
	CheckPhoneMatchL(KCntDuplicateNumber,9,2);
	
	// delete contact one 
	CntTest->Db()->DeleteContactL(one);
	
	//check that there is only 1 match now
	CheckPhoneMatchL(KCntDuplicateNumber,9,1);

	//edit item two
	CContactItem* item = CntTest->Db()->OpenContactL(two);
	CleanupStack::PushL(item);
	CContactItemFieldSet& fieldSet = item->CardFields();
	TInt fieldId=fieldSet.Find(KUidContactFieldSms);
	CContactItemField& field = fieldSet[fieldId];
	field.TextStorage()->SetTextL(KCntNewSMSNumber);
	CntTest->Db()->CommitContactL(*item);
	CleanupStack::PopAndDestroy(item);
	
	//check that there is no matches now
	CheckPhoneMatchL(KCntDuplicateNumber,9,0);
	}


// Check that the speed dial at position aSpeedDialPosition matches the aExpectedId
// and the contents of the field with aFieldId is the same in the speed dial.
//
LOCAL_C void CheckSpeedDialL(TInt aSpeedDialPosition, CContactItem& aContact, TInt aFieldId)
	{
	HBufC* buf=HBufC::NewLC(100);	
	TPtr fieldContents=buf->Des();
	
	TContactItemId id = CntTest->Db()->GetSpeedDialFieldL(aSpeedDialPosition, fieldContents);
	TPtrC actualContents =(aContact.CardFields()[aFieldId].TextStorage()->Text());
	test(id==aContact.Id());
	test(actualContents==fieldContents);

	CleanupStack::PopAndDestroy(); //buf
	}

/**
 * Check that speed dials are working correctly;
 * Add first contact to speed dial 1, add second contact to speed dial 9.
 * Check speed dial 1 still matches contact 1 after adding speed dial
 * Check persistance of speed dial list
 * Check overwriting old speed dial with new contact id
 * Check deletion of contact removes all existing speed dials
 */
LOCAL_C void SpeedDialSimpleTest()
	{
	ResetDatabaseL();
	TRAP_IGNORE(PopulateDatabaseL(KTestContactsNum));
	
	const CContactIdArray* sortedItems = CntTest->Db()->SortedItemsL();

	// first item in position 1
	CContactItem* firstItem = CntTest->Db()->OpenContactL((*sortedItems)[0]);
	CleanupStack::PushL(firstItem);
	TInt fieldId=5;
	const TInt speedDialPositionOne=1;
	CntTest->Db()->SetFieldAsSpeedDialL(*firstItem, fieldId, speedDialPositionOne);
	CheckSpeedDialL(speedDialPositionOne,*firstItem,fieldId);

	// second item in position 2
	CContactItem* secondItem = CntTest->Db()->OpenContactL((*sortedItems)[1]);
	CleanupStack::PushL(secondItem);
	fieldId=5;
	const TInt speedDialPositionNine=9;
	CntTest->Db()->SetFieldAsSpeedDialL(*secondItem, fieldId, speedDialPositionNine);
	CheckSpeedDialL(speedDialPositionNine,*secondItem,fieldId);

	// wrong answers..... 
	CheckSpeedDialL(speedDialPositionOne,*firstItem,5);
	
	// check persistance
	CntTest->CloseDatabase();
	CntTest->OpenDatabaseL();
	CheckSpeedDialL(speedDialPositionOne,*firstItem,5);
	CheckSpeedDialL(speedDialPositionNine,*secondItem,5);

	// check overwriting old speed dial (third contact, speed dial 1)
	const CContactIdArray* newSortedItems = CntTest->Db()->SortedItemsL();
	CContactItem* thirdItem = CntTest->Db()->OpenContactL((*newSortedItems)[2]);
	CleanupStack::PushL(thirdItem);
	fieldId=5;
	CntTest->Db()->SetFieldAsSpeedDialL(*thirdItem, fieldId, speedDialPositionOne);
	CheckSpeedDialL(speedDialPositionOne,*thirdItem,5);

	// delete contact and check that speed dials have been removed
	CntTest->Db()->DeleteContactL((*newSortedItems)[2]);
	HBufC* temp8=HBufC::NewLC(100);	
	TPtr testText8=temp8->Des();
	TContactItemId eightId = CntTest->Db()->GetSpeedDialFieldL(speedDialPositionOne, testText8);
	test(eightId==KNullContactId);
	test(testText8==KNullDesC);

	// modify a contact with a speed dial - but do not change the relavent field.
	CContactItem* fourthItem = CntTest->Db()->OpenContactL((*newSortedItems)[2]);
	CleanupStack::PushL(fourthItem);
	fieldId=5;
	
	CntTest->Db()->SetFieldAsSpeedDialL(*fourthItem, fieldId, speedDialPositionOne);
	TInt fieldPosOfSpeedDial = fourthItem->CardFields().Find(KUidSpeedDialOne);
	test(fieldPosOfSpeedDial != KErrNotFound);
  	
	CContactItem* fifthItem = CntTest->Db()->OpenContactL(fourthItem->Id());
	CleanupStack::PushL(fifthItem);

	// remove the first field in the list.
	fieldPosOfSpeedDial = fifthItem->CardFields().Find(KUidSpeedDialOne);

	fifthItem->CardFields().Remove(1);
	CntTest->Db()->CommitContactL(*fifthItem);
	
	CleanupStack::PopAndDestroy(6); // firstItem, secondItem, thirdItem, fourthItem, fifthItem, temp8
	}


/**
 *Add five contacts.
 * Set two speed dials - remove one & check the other is still there.
 */
LOCAL_C void TestRemovingSpeedDialsL()
	{
	ResetDatabaseL();
	PopulateDatabaseL(KTestContactsNum);
	const CContactIdArray* sortedItems = CntTest->Db()->SortedItemsL(); //doesn't take ownership
	const TInt speedDialPositionOne=1;
	const TInt speedDialPositionNine=9;

	
	const TContactItemId firstId = (*sortedItems)[3];
	const TContactItemId secondId = (*sortedItems)[5];

	CContactItem* first = CntTest->Db()->OpenContactL(firstId);
	CleanupStack::PushL(first);
	CContactItemFieldSet& aFieldSet = first->CardFields();
	for(TInt fieldLoop=0;fieldLoop<aFieldSet.Count();fieldLoop++)
		{
		CContactItemField& field = aFieldSet[fieldLoop];
		if (field.ContentType().ContainsFieldType(KUidContactFieldPhoneNumber))
			{
			// change it...
			field.TextStorage()->SetTextL(_L("956431458"));
			}				
		}
	CntTest->Db()->CommitContactL(*first);
	CleanupStack::PopAndDestroy(); //first

	first = CntTest->Db()->OpenContactL(firstId);
	CleanupStack::PushL(first);	
	TInt fieldId=first->CardFields().Find(KUidContactFieldPhoneNumber);
	CntTest->Db()->SetFieldAsSpeedDialL(*first, fieldId, speedDialPositionOne);

	CContactItem* second = CntTest->Db()->OpenContactL(secondId);
	CleanupStack::PushL(second);	
	TInt fieldId2=first->CardFields().Find(KUidContactFieldPhoneNumber);
	CntTest->Db()->SetFieldAsSpeedDialL(*second, fieldId2, speedDialPositionNine);

	// check they are all established ok
	HBufC* buf=HBufC::NewLC(100);	
	TPtr fieldContents=buf->Des();
	TContactItemId	id = CntTest->Db()->GetSpeedDialFieldL(speedDialPositionOne, fieldContents);
	test(id==firstId);
	id = CntTest->Db()->GetSpeedDialFieldL(speedDialPositionNine, fieldContents);
	test(id==secondId);
	CleanupStack::PopAndDestroy(); //buf

	CntTest->Db()->RemoveSpeedDialFieldL(firstId, speedDialPositionOne);

	HBufC* buf2=HBufC::NewLC(100);	
	TPtr phoneNum=buf2->Des();
	TContactItemId idOne = CntTest->Db()->GetSpeedDialFieldL(speedDialPositionOne, phoneNum);
	test(idOne==KNullContactId);
	test(phoneNum==KNullDesC);
	CleanupStack::PopAndDestroy(); //buf2


	HBufC* buf3=HBufC::NewLC(100);	
	TPtr phoneNo2=buf3->Des();
	TContactItemId id2 = CntTest->Db()->GetSpeedDialFieldL(speedDialPositionNine, phoneNo2);
	test(id2==secondId);
	CleanupStack::PopAndDestroy(); //buf

	CntTest->Db()->RemoveSpeedDialFieldL(secondId, speedDialPositionNine);

	HBufC* buf4=HBufC::NewLC(100);	
	TPtr phoneNo3=buf4->Des();
	id = CntTest->Db()->GetSpeedDialFieldL(speedDialPositionNine, phoneNo3);
	test(id==KNullContactId);
	test(phoneNum==KNullDesC);
	CleanupStack::PopAndDestroy(); //buf4

	CleanupStack::PopAndDestroy(2); //first, second
	}

/**
 * Verifies that matching of 11-digit phone number matching works correctly
 */
LOCAL_C void Test11DigitMatchingL()
	{
	ResetDatabaseL();

	_LIT(K11DigitTestNumber1,"20 7582 24220");
	_LIT(K11DigitTestNumber2,"020 7582 2422");
	_LIT(K11DigitTestNumber3,"020 7582 2381");
	_LIT(K11DigitTestNumber4,"120 7582 2381");
	_LIT(K11DigitTestNumber5,"9120 7582 2381");

	CreateContactL(KCntName,KCntSurname,K11DigitTestNumber1,KNullDesC);
	CreateContactL(KCntName,KCntSurname,K11DigitTestNumber2,KNullDesC);
	CreateContactL(KCntName,KCntSurname,K11DigitTestNumber3,KNullDesC);
	CreateContactL(KCntName,KCntSurname,K11DigitTestNumber4,KNullDesC);
	CreateContactL(KCntName,KCntSurname,K11DigitTestNumber5,KNullDesC);

	//Match numbers
	_LIT(K11DigitMatch1,"020 7582 2422");
	_LIT(K11DigitMatch2,"20 7582 24220");
	_LIT(K11DigitMatch3,"020 7582 2381");
	_LIT(K11DigitMatch4,"120 7582 2381");
	_LIT(K11DigitMatch5,"9120 7582 2381");
	_LIT(K11DigitMatch6,"20 7582 2381");

	CheckPhoneMatchL(K11DigitMatch1,KMatch10Digits,1);
	CheckPhoneMatchL(K11DigitMatch1,KMatch11Digits,1);

	CheckPhoneMatchL(K11DigitMatch2,KMatch10Digits,1);
	CheckPhoneMatchL(K11DigitMatch2,KMatch11Digits,1);

	CheckPhoneMatchL(K11DigitMatch3,KMatch10Digits,3);
	CheckPhoneMatchL(K11DigitMatch3,KMatch11Digits,1);

	CheckPhoneMatchL(K11DigitMatch4,KMatch10Digits,3);
	CheckPhoneMatchL(K11DigitMatch4,KMatch11Digits,2);

	CheckPhoneMatchL(K11DigitMatch5,KMatch12Digits,1);
	CheckPhoneMatchL(K11DigitMatch5,KMatch13Digits,1);

	CheckPhoneMatchL(K11DigitMatch6,KMatch10Digits,3);
	CheckPhoneMatchL(K11DigitMatch6,KMatch11Digits,3);
	}

/**
 * Verifies that 15 digit phone numbers are matched correctly.
 */
LOCAL_C void Test15DigitMatchingL()
	{
	ResetDatabaseL();

	_LIT(K15DigitTestNumber1,"0086 207 3453 3454");
	_LIT(K15DigitTestNumber2,"8186 207 3453 3454");
	_LIT(K15DigitTestNumber3,"8620 734 5334 5400");

	CreateContactL(KCntName,KCntSurname,K15DigitTestNumber1,KNullDesC);
	CreateContactL(KCntName,KCntSurname,K15DigitTestNumber2,KNullDesC);
	CreateContactL(KCntName,KCntSurname,K15DigitTestNumber3,KNullDesC);

	//Match numbers
	_LIT(K15DigitMatch1,"0086 207 3453 3454");
	_LIT(K15DigitMatch2,"86 207 3453 3454");
	_LIT(K15DigitMatch3,"8186 207 3453 3454");
	_LIT(K15DigitMatch4,"08186 207 3453 3454");

	CheckPhoneMatchL(K15DigitMatch1,KMatch15Digits,1);
	CheckPhoneMatchL(K15DigitMatch1,KMatch14Digits,1);
	CheckPhoneMatchL(K15DigitMatch1,KMatch13Digits,2);

	CheckPhoneMatchL(K15DigitMatch2,KMatch13Digits,2);

	CheckPhoneMatchL(K15DigitMatch3,KMatch15Digits,1);

	CheckPhoneMatchL(K15DigitMatch4,KMatch16Digits,0);
	CheckPhoneMatchL(K15DigitMatch4,KMatch15Digits,1);
	}


/**
 * Verifies that phone numbers are matched correctly if they
 * have leading, trailing and embedded zeroes.
 */
LOCAL_C void TestEmbeddedZeroesL()
	{
	ResetDatabaseL();

	_LIT(KEmbeddedZeroesTestNumber1,"9999 9990 0999 999");
	_LIT(KEmbeddedZeroesTestNumber2,"9000 0000 0000 000");
	_LIT(KEmbeddedZeroesTestNumber3,"0000 0000 0000 000");
	_LIT(KEmbeddedZeroesTestNumber4,"0000 0000 0000 009");
	_LIT(KEmbeddedZeroesTestNumber5,"9 9000 000");
	_LIT(KEmbeddedZeroesTestNumber6,"9000 000");

	CreateContactL(KCntName,KCntSurname,KEmbeddedZeroesTestNumber1,KNullDesC);
	CreateContactL(KCntName,KCntSurname,KEmbeddedZeroesTestNumber2,KNullDesC);
	CreateContactL(KCntName,KCntSurname,KEmbeddedZeroesTestNumber3,KNullDesC);
	CreateContactL(KCntName,KCntSurname,KEmbeddedZeroesTestNumber4,KNullDesC);
	CreateContactL(KCntName,KCntSurname,KEmbeddedZeroesTestNumber5,KNullDesC);
	CreateContactL(KCntName,KCntSurname,KEmbeddedZeroesTestNumber6,KNullDesC);

	//Match numbers
	_LIT(KEmbeddedZeroMatch1,"9999 9990 0999 999");
	_LIT(KEmbeddedZeroMatch2,"9000 0000 0000 000");
	_LIT(KEmbeddedZeroMatch3,"0000 0000 0000 000");
	_LIT(KEmbeddedZeroMatch4,"0000 0000 0000 009");
	_LIT(KEmbeddedZeroMatch5,"9 9000 000");
	_LIT(KEmbeddedZeroMatch6,"9000 000");
	_LIT(KEmbeddedZeroMatch7,"0000 000");

	CheckPhoneMatchL(KEmbeddedZeroMatch1,KMatch15Digits,1);
	CheckPhoneMatchL(KEmbeddedZeroMatch2,KMatch15Digits,1);
	CheckPhoneMatchL(KEmbeddedZeroMatch3,KMatch15Digits,1);
	CheckPhoneMatchL(KEmbeddedZeroMatch4,KMatch15Digits,1);
	CheckPhoneMatchL(KEmbeddedZeroMatch5,KMatch15Digits,1);
	CheckPhoneMatchL(KEmbeddedZeroMatch5,KMatch8Digits,1);
	CheckPhoneMatchL(KEmbeddedZeroMatch6,KMatch7Digits,2);
	CheckPhoneMatchL(KEmbeddedZeroMatch7,KMatch7Digits,2);
	CheckPhoneMatchL(KEmbeddedZeroMatch6,KMatch6Digits,2);
	}


#ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__ 	//we mark this since the new contact database based on SQLite 
                         		//as the porting on SQLite database allows the thorough DC break and
                         		//doesn't support the feature of database update from old database on
                         		//Symbian DBMS.
void TestDbUpdateL()
	{

    RPIMTestServer serv;
    User::LeaveIfError(serv.Connect());
    serv.CopyFileL(KSrcDatabaseName, KDatabaseName);

    serv.Close();

	// Contact model must open and upgrade database
	test.Printf(_L("Opening and upgrading database\n"));
	CContactDatabase* db = NULL; 
	db = CContactDatabase::OpenL(KDatabaseOpenName);

	// Add a contact with a 15 digit phone number and match the same number
	_LIT(K15DigitTestNumber,"0086 207 3453 1212");
	_LIT(K15DigitMatch,"0086 207 3453 1212");
	CreateContactL(KCntName,KCntSurname,K15DigitTestNumber,KNullDesC);
	CheckPhoneMatchL(K15DigitMatch,KMatch15Digits,1);

	delete db;
	db = NULL;
	}
#endif // __SYMBIAN_CNTMODEL_USE_SQLITE__ 

/**

@SYMTestCaseID     PIM-T-NOMACH-0001

*/

LOCAL_C void DoTestsL()
    {
	test.Start(_L("@SYMTESTCaseID:PIM-T-NOMACH-0001 T_NOMACH"));


	CTestRegister* TempFiles = CTestRegister::NewLC();
	

#ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__
	TempFiles->RegisterL(KDatabaseOpenName, EFileTypeCnt);
#endif // __SYMBIAN_CNTMODEL_USE_SQLITE__
	
	TempFiles->RegisterL(KDatabaseFileName, EFileTypeCnt);

	CntTest->CreateDatabaseL();
    TInt err;
    
#ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__ //see comments above function TestDbUpdateL 
	test.Printf(KTestDbUpdate);
	TRAP(err,TestDbUpdateL());
	test(err==KErrNone);
#endif // __SYMBIAN_CNTMODEL_USE_SQLITE__



     


	test.Printf(KTest11Digitmatching);
	TRAP(err,Test11DigitMatchingL());
	test(err==KErrNone);

	test.Printf(KTest15Digitmatching);
	TRAP(err,Test15DigitMatchingL());
	test(err==KErrNone);

	test.Printf(KTestEmbeddedZeroes);
	TRAP(err,TestEmbeddedZeroesL());
	test(err==KErrNone);

	test.Printf(KTestNoPhoneMatchLibrary);
	TestNoPhoneMatchLibraryL();

	test.Printf(KTestNewMatching);
	TestNewMatchingL();


	test.Printf(KTestFaxSMSMatching);
	TestFaxSMSMatchingL();

	test.Printf(KSpeedDialSimpleTest);
	TRAP(err,SpeedDialSimpleTest());
	test(err==KErrNone);

	test.Printf(KTestRemovingSpeedDials);
	TRAP(err,TestRemovingSpeedDialsL());
	test(err==KErrNone);

	CntTest->CloseDatabase();
	CntTest->DeleteDatabaseL();

	CleanupStack::PopAndDestroy(TempFiles);
	
	// stop efsrv.lib warning on 8.1a wins
	TEntry dummy;
	(void)dummy.IsTypeValid();
    }


	
GLDEF_C TInt E32Main()
	{
    CntTest=new(ELeave) CCntTest;
	CntTest->ConstructL(test,KDatabaseFileName);
    TRAPD(err,DoTestsL());
	test.Printf(_L("\n"));
	CntTest->EndTestLib(err);

	return KErrNone;
    }
