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




#include <e32test.h>
#include <f32file.h>
#include "t_utils.h"

_LIT(KTestName,"T_CntMatch");
#include "testhelpers.h"

RFs fs;

_LIT(KDatabaseFileName,"c:T_CntMatch");

#if defined(__EABI__)
 _LIT(KTestFileName, "z:\\CntmodelTest\\%S");
#else
 _LIT(KTestFileName, "z:\\system\\programs\\cntmodelTest\\%S");
#endif

_LIT(KCntName,"Firstname");
_LIT(KCntSurname,"Surname");

GLDEF_D CCntTest* CntTest=NULL;
TInt PhoneNumberFieldIndex = -1;

TBool FileExists(const TDesC& aFilename)
	{
	TUint val;
	TBool res = fs.Att(aFilename, val) == KErrNone;
	return res;
	}


// Delete the current contacts database and create a new one
//
LOCAL_C void ResetDatabaseL()
	{
	CntTest->CloseDatabase();
	CntTest->DeleteDatabaseL();
	CntTest->CreateDatabaseL();
	CntTest->OpenDatabaseL();
	// Reset all server side speed dials ;-)))
	CntTest->Db()->ResetServerSpeedDialsL();
	}


LOCAL_C TContactItemId CreateContactL(const TDesC& aFirstName, const TDesC& aFamilyName, const TDesC& aPhoneNo, const TDesC& aMobileNo)
	{
	CContactItem* item=CContactCard::NewLC();
	SetNameL(*item,KUidContactFieldGivenName,KUidContactFieldVCardMapUnusedN,aFirstName,ETrue);
	SetNameL(*item,KUidContactFieldFamilyName,KUidContactFieldVCardMapUnusedN,aFamilyName,ETrue);
	SetNameL(*item,KUidContactFieldPhoneNumber,KUidContactFieldVCardMapTEL,aPhoneNo,ETrue); //KUidContactFieldVCardMapTELFAX

	if (aMobileNo.Length() > 0)
		SetNameL(*item,KUidContactFieldPhoneNumber,KUidContactFieldVCardMapCELL,aMobileNo,ETrue);

	item->SetTemplateRefId(KGoldenTemplateId);
	TContactItemId id=CntTest->Db()->AddNewContactL(*item);
	CleanupStack::PopAndDestroy(item);
	return id;
	}


LOCAL_C TInt CheckPhoneMatchL(const TDesC& aPhoneNumberToMatch, TInt aNumberOfMatchDigits) //, TInt aExpectedNumOfMatches)
	{
	TBuf<256> testCase;
	_LIT(KFormat,"Checking matches to %S, matching %d digits");
	testCase.Format(KFormat, &aPhoneNumberToMatch, aNumberOfMatchDigits);
	//test.Next(_L(" "));

	CContactDatabase& db = *(CntTest->Db());
	CContactIdArray* array = db.MatchPhoneNumberL(aPhoneNumberToMatch,aNumberOfMatchDigits);
	CleanupStack::PushL(array);
	const TInt numberOfMatches = array->Count();
	CleanupStack::PopAndDestroy(array);
	return numberOfMatches;
	}


//Simple test cases
LOCAL_C void PerfectMatchL()
	{
	//Database contains the only phone number.
	//We search for exactly the same number.
	//The matching must return one for all the rage of digits.

	ResetDatabaseL();

	_LIT(KTestNumber2,"4362 5120 7000 0001");
	CreateContactL(KCntName,KCntSurname,KTestNumber2,KNullDesC);

	TESTTRUE(CheckPhoneMatchL(KTestNumber2,1) == 1);
	TESTTRUE(CheckPhoneMatchL(KTestNumber2,2) == 1);
	TESTTRUE(CheckPhoneMatchL(KTestNumber2,3) == 1);
	TESTTRUE(CheckPhoneMatchL(KTestNumber2,4) == 1);
	TESTTRUE(CheckPhoneMatchL(KTestNumber2,5) == 1);
	TESTTRUE(CheckPhoneMatchL(KTestNumber2,6) == 1);
	TESTTRUE(CheckPhoneMatchL(KTestNumber2,7) == 1);
	TESTTRUE(CheckPhoneMatchL(KTestNumber2,8) == 1);
	TESTTRUE(CheckPhoneMatchL(KTestNumber2,9) == 1);
	TESTTRUE(CheckPhoneMatchL(KTestNumber2,10) == 1);
	TESTTRUE(CheckPhoneMatchL(KTestNumber2,11) == 1);
	TESTTRUE(CheckPhoneMatchL(KTestNumber2,12) == 1);
	TESTTRUE(CheckPhoneMatchL(KTestNumber2,13) == 1);
	TESTTRUE(CheckPhoneMatchL(KTestNumber2,14) == 1);
	TESTTRUE(CheckPhoneMatchL(KTestNumber2,15) == 1);
	TESTTRUE(CheckPhoneMatchL(KTestNumber2,16) == 0);	// AB - only 15 digits..
	//Here it works...


	//The same story - match on itself must happen for all the rage of digits.
	_LIT(KTestNumber,"789543625 120 7582 2381");
	CreateContactL(KCntName,KCntSurname,KTestNumber,KNullDesC);

	TESTTRUE(CheckPhoneMatchL(KTestNumber,1) == 1);
	TESTTRUE(CheckPhoneMatchL(KTestNumber,2) == 0);
	TESTTRUE(CheckPhoneMatchL(KTestNumber,3) == 0);
	TESTTRUE(CheckPhoneMatchL(KTestNumber,4) == 0);
	TESTTRUE(CheckPhoneMatchL(KTestNumber,5) == 0);
	TESTTRUE(CheckPhoneMatchL(KTestNumber,6) == 0);
	TESTTRUE(CheckPhoneMatchL(KTestNumber,7) == 1);
	TESTTRUE(CheckPhoneMatchL(KTestNumber,8) == 1);
	TESTTRUE(CheckPhoneMatchL(KTestNumber,9) == 1);
	TESTTRUE(CheckPhoneMatchL(KTestNumber,10) == 1);
	TESTTRUE(CheckPhoneMatchL(KTestNumber,11) == 1);
	TESTTRUE(CheckPhoneMatchL(KTestNumber,12) == 1);
	TESTTRUE(CheckPhoneMatchL(KTestNumber,13) == 1);
	TESTTRUE(CheckPhoneMatchL(KTestNumber,14) == 1);
	TESTTRUE(CheckPhoneMatchL(KTestNumber,15) == 1);
	TESTTRUE(CheckPhoneMatchL(KTestNumber,16) == 0);	// AB - not 16 digits, 15 is max.
	//Here it doesn't

	/*  At the moment, the function simply returns "no matches" if the
		number of match digits is incorrect. At no point does it leave with
		KErrArgument; if it did, it is possible that client code could be
		broken because it expects the API to return zero, not that it should
		leave. Whilst leaving with KErrArgument is a laudable aspiration, it
		should not be changed here without going through the appropriate
		assessment process. Suggest raising a CR.

	//We shouldn't be able to match on less than 1 digit...
	TRAPD(err,CheckPhoneMatchL(KTestNumber,0));
	TESTVALUE(err, KErrArgument);

	TRAP(err,CheckPhoneMatchL(KTestNumber,-15));
	TESTVALUE(err, KErrArgument);

	//We do not provide matching for more than 16 digits.
	TRAP(err, CheckPhoneMatchL(KTestNumber,200));
	TESTVALUE(err, KErrArgument);

	*/
	}

LOCAL_C void DontUnderestimateTheMeaningOfZeroL()
	{
	ResetDatabaseL();

	//Database contains only this number 228 876 5324;
	CreateContactL(KCntName,KCntSurname,_L("228 876 5324"),KNullDesC);
	//Trying to search on that exact number by 6 digits...
		TESTVALUE(CheckPhoneMatchL(		_L("228 876 5324"), 6),0);
	//No matches, well I kind of expected it... :-(

	//Now we add a number that differs from the first one by one digit..
	CreateContactL(KCntName,KCntSurname,_L("228 076 5324"),KNullDesC);

	//Repeat our search
		TESTVALUE(CheckPhoneMatchL(		_L("228 876 5324"), 6),1);
	//It matches!!... Please don't laugh! There is logic in it!
	//If the number doesn't match to itself - then it should match on different one! ;-)
	}


LOCAL_C void Test2L()
	{
	ResetDatabaseL();

	//if 5324 doesn't match to  228 100 5324...
	CreateContactL(KCntName,KCntSurname,_L("228 100 5324"),KNullDesC);
	TESTVALUE(CheckPhoneMatchL(					_L("5324"),11),0);
	TESTVALUE(CheckPhoneMatchL(					_L("5324"), 4),0);

	//it shouldn't match to 228 000 5324 as well, but... it does
	CreateContactL(KCntName,KCntSurname,_L("228 000 5324"),KNullDesC);
	TESTVALUE(CheckPhoneMatchL(					_L("5324"),11),1);	// AB
	TESTVALUE(CheckPhoneMatchL(					_L("5324"), 4),1);
	}


LOCAL_C void TestCallingAmericaL()
	{
	ResetDatabaseL();

	//here are some additional test numbers for testing the acceptance of the '(+' combination
	CreateContactL(KCntName,KCntSurname,_L("(+358) 1234567"),KNullDesC);
	CreateContactL(KCntName,KCntSurname,_L("(358) 1234567"),KNullDesC);
	CreateContactL(KCntName,KCntSurname,_L("+358 1234567"),KNullDesC);

	//when searching for any of the above numbers 3 instances should be found
	TESTVALUE(CheckPhoneMatchL(_L("(+358) 1234567"),10),3);
	TESTVALUE(CheckPhoneMatchL(_L("(358) 1234567"),10),3);
	TESTVALUE(CheckPhoneMatchL(_L("+358 1234567"),10),3);

	// Note that the phone number parser does generate a hash of 0 for all
	//these now that the '(+'combination is recognised
	CreateContactL(KCntName,KCntSurname,_L("(+1) 0755 345 3644 644"),KNullDesC);
	CreateContactL(KCntName,KCntSurname,_L("(+1) 0789 37737 39173-1"),KNullDesC);
	CreateContactL(KCntName,KCntSurname,_L("(+1) 0755 789 445 557"),KNullDesC);

	//these numbers should no longer be found
	TESTVALUE(CheckPhoneMatchL(_L("(650)365-0000"),4),0);
	TESTVALUE(CheckPhoneMatchL(_L("20 00000 00000"),11),0);

	//the american number should now be found with out a problem
	TESTVALUE(CheckPhoneMatchL(_L("(+1) 0755 345 3644 644"), 7),1);
	TESTVALUE(CheckPhoneMatchL(_L("(+1) 0755 345 3644 644"), 8),1);
	TESTVALUE(CheckPhoneMatchL(_L("(+1) 0755 345 3644 644"), 9),1);
	TESTVALUE(CheckPhoneMatchL(_L("(+1) 0755 345 3644 644"),10),1);
	TESTVALUE(CheckPhoneMatchL(_L("(+1) 0755 345 3644 644"),11),1);
	TESTVALUE(CheckPhoneMatchL(_L("(+1) 0755 345 3644 644"),12),1);
	TESTVALUE(CheckPhoneMatchL(_L("(+1) 0755 345 3644 644"),13),1);
	TESTVALUE(CheckPhoneMatchL(_L("(+1) 0755 345 3644 644"),14),1);
	TESTVALUE(CheckPhoneMatchL(_L("(+1) 0755 345 3644 644"),15),1);
	TESTVALUE(CheckPhoneMatchL(_L("(+1) 0755 345 3644 644"),16),1);
	TESTVALUE(CheckPhoneMatchL(_L("(+1) 0755 345 3644 644"), KBestMatchingPhoneNumbers),1);
	}


LOCAL_C void Test3L()
	{
	ResetDatabaseL();

	CreateContactL(KCntName,KCntSurname,_L(		   "123 4567"),KNullDesC);
	CreateContactL(KCntName,KCntSurname,_L("9876543 123 4567"),KNullDesC);

	//it definetly should match on itself....
	TESTVALUE(CheckPhoneMatchL(_L("123 4567"), 7),2); //Both numbers match by 7 digits..
	TESTVALUE(CheckPhoneMatchL(_L("123 4567"), 8),2); //But they must differ for more digits
	TESTVALUE(CheckPhoneMatchL(_L("123 4567"), 9),2); // AB - all these should match both...
	TESTVALUE(CheckPhoneMatchL(_L("123 4567"),10),2);
	TESTVALUE(CheckPhoneMatchL(_L("123 4567"),11),2);
	TESTVALUE(CheckPhoneMatchL(_L("123 4567"),12),2);
	TESTVALUE(CheckPhoneMatchL(_L("123 4567"),13),2);
	TESTVALUE(CheckPhoneMatchL(_L("123 4567"),14),2);
	TESTVALUE(CheckPhoneMatchL(_L("123 4567"),15),2);
	TESTVALUE(CheckPhoneMatchL(_L("123 4567"), KBestMatchingPhoneNumbers),2);
	}

LOCAL_C void Test4L()
	{
	ResetDatabaseL();

	CreateContactL(KCntName,KCntSurname,_L(	  "6543 123 4567 00"),KNullDesC);


	TESTVALUE(CheckPhoneMatchL(_L("6543 123 456700"), 7),1); //Both numbers match by 7 digits..
	TESTVALUE(CheckPhoneMatchL(_L("6543 123 456700"), 8),1); //But they must differ for more digits
	TESTVALUE(CheckPhoneMatchL(_L("6543 123 456700"), 9),1);
	TESTVALUE(CheckPhoneMatchL(_L("6543 123 456700"),10),1);
	TESTVALUE(CheckPhoneMatchL(_L("6543 123 456700"),11),1);
	TESTVALUE(CheckPhoneMatchL(_L("6543 123 456700"),12),1);
	TESTVALUE(CheckPhoneMatchL(_L("6543 123 456700"),13),1);
	TESTVALUE(CheckPhoneMatchL(_L("6543 123 456700"),14),1);
	TESTVALUE(CheckPhoneMatchL(_L("6543 123 456700"),15),1);
	TESTVALUE(CheckPhoneMatchL(_L("6543 123 456700"), KBestMatchingPhoneNumbers),1);
	}


LOCAL_C void Test5L()
	{
	ResetDatabaseL();

	CreateContactL(KCntName,KCntSurname,_L("01234 56789"),KNullDesC);

	//it definetly should match on itself....
	TESTVALUE(CheckPhoneMatchL(_L("00 44 1234 56789"), 7),1); //Both numbers match by 7 digits..
	TESTVALUE(CheckPhoneMatchL(_L("00 44 1234 56789"), 8),1); //But they must differ for more digits
	TESTVALUE(CheckPhoneMatchL(_L("00 44 1234 56789"), 9),1);
	TESTVALUE(CheckPhoneMatchL(_L("00 44 1234 56789"),10),0);
	TESTVALUE(CheckPhoneMatchL(_L("00 44 1234 56789"),11),0);
	TESTVALUE(CheckPhoneMatchL(_L("00 44 1234 56789"),12),0);
	TESTVALUE(CheckPhoneMatchL(_L("00 44 1234 56789"),13),0);
	TESTVALUE(CheckPhoneMatchL(_L("00 44 1234 56789"),14),0);
	TESTVALUE(CheckPhoneMatchL(_L("00 44 1234 56789"),15),0);
	TESTVALUE(CheckPhoneMatchL(_L("00 44 1234 56789"), KBestMatchingPhoneNumbers),1);
	}

/**
+@SYMTestType UT
+@SYMTestPriority Critical
+@SYMDEF PDEF097460
+@SYMTestCaseDesc Tests that attempting to match a phone number consisting of
+only "(" does not cause a panic.

+@SYMTestActions
+1. Add a number consisting only of "(".
+2. Check that no panic or error occurs when we attempt to match.

+@SYMTestExpectedResults No panic occurs.
+*/


void TestPDEF097460()
	{
	test.Next(_L("Testing fix for PDEF097460."));


	//
	// 1. Add a number consisting only of "(".
	//
	_LIT(KTestNumber3,"(");
	CreateContactL(KCntName,KCntSurname,KTestNumber3,KNullDesC);
	//
	// 2. Check that no panic or error occurs when we attempt to match.
	//
	TRAPD( err, CheckPhoneMatchL(KTestNumber3,1) );
	test( err == KErrNone );
	}
/////// CCsvReader  class //////////////////////////

class CCsvReader : CBase
	{
public:
	static CCsvReader* NewLC(const TDesC& filename);
	~CCsvReader();

	CDesCArray* ReadNextLineL();

private:
	CCsvReader() {};

	RFs iFs;
	RFile iSrcFile;
	};



CCsvReader::~CCsvReader()
	{
	iSrcFile.Close();
	}

CCsvReader* CCsvReader::NewLC(const TDesC& filename)
	{
	CCsvReader* self = new (ELeave) CCsvReader();
	CleanupStack::PushL(self);
	User::LeaveIfError(self->iFs.Connect());
	User::LeaveIfError(self->iSrcFile.Open(self->iFs,filename,EFileRead));
	return self;
	}


CDesCArray* CCsvReader::ReadNextLineL()
	{
	TBool goOn = ETrue;
	TBuf8<1> symbol;
	TFileName buf;
	CDesCArraySeg* array = new (ELeave) CDesCArraySeg(3);
	CleanupStack::PushL(array);

	while (goOn)
		{
		User::LeaveIfError(iSrcFile.Read(symbol, 1));
		if(symbol.Length() == 0)
			{
			if (buf.Length() > 0) //If there is something in the buffer - save in array
				array->AppendL(buf);

			if (array->Count() > 0)
				{
				CleanupStack::Pop(array);
				return array; //If array is not empty - return what we've got
				}
			else
				{
				CleanupStack::PopAndDestroy(array);
				return NULL;  //If array is empty - return zero to indicate EOF
				}
			}

		TChar chr = symbol[0];
		switch(chr)
			{
			case '\r': 	goOn = EFalse;
			case ';': 	array->AppendL(buf);
						buf.Zero();
			case '\n':  //Ignore
			break;

			default:
				buf.Append(chr);
			}
		}

	CleanupStack::Pop(array);
	return array;
	}


LOCAL_C void FillInDatabaseL(const TDesC& file)
	{
	test.Printf(_L("Creating Database:\n"));
	CntTest->CreateDatabaseL();

	CCsvReader* reader = CCsvReader::NewLC(file);
	FOREVER
		{
		CDesCArray* arr = reader->ReadNextLineL();
		if (!arr) //EOF reached
			break;
		PUSH(arr);
		const TPtrC surname	= (*arr)[0];
		const TPtrC name	= (*arr)[1];
		const TPtrC phone	= (*arr)[2];
		CreateContactL(name, surname, phone, KNullDesC);

		test.Printf(surname.Mid(0,1));
		POPD(arr);
		}
	POPD(reader);

	CntTest->Db()->CompactL();
	}

void SetPhoneNumberFieldIndex(const TContactItemId& id)
	{
	CContactItem* cnt = CntTest->Db()->ReadContactL(id);
	PUSH(cnt);
	TInt fldInd = 0;
	FOREVER  //Find a field with phone number
		{
		fldInd = cnt->CardFields().FindNext(KUidContactFieldPhoneNumber,KUidContactFieldVCardMapTEL, fldInd+1);
		ASSERT(fldInd != KErrNotFound);
		CContactItemField& phoneFld = cnt->CardFields().operator[](fldInd); //fldInd
		const TPtrC& phoneNum = phoneFld.TextStorage()->Text();
		if (phoneNum.Length())
			{
			PhoneNumberFieldIndex = fldInd;
			break;
			}
		}
	POPD(cnt);
	}



LOCAL_C CDesCArraySeg* MatchL(const TDesC& phone, TInt matchOn)
	{
	CDesCArraySeg* retArray = new (ELeave) CDesCArraySeg(3);
	PUSH(retArray);

	CContactIdArray* array = CntTest->Db()->MatchPhoneNumberL(phone,matchOn);
	CleanupStack::PushL(array);
	const TInt numberOfMatches = array->Count();

	for(TInt j=0; j<numberOfMatches; ++j)  //Cycle on results of matching
		{
		const TContactItemId& id = (*array)[j];
		if (PhoneNumberFieldIndex == -1) //Find an index for the field with the phone (depends on template)
			SetPhoneNumberFieldIndex(id);

		CContactItem* cnt = CntTest->Db()->ReadContactL(id);
		CleanupStack::PushL(cnt);
		CContactItemField& phoneFld = cnt->CardFields().operator[](PhoneNumberFieldIndex); //todo: 27-magic number
		const TPtrC& phoneStr = phoneFld.TextStorage()->Text();

		retArray->AppendL(phoneStr);

		CleanupStack::PopAndDestroy(cnt);
		}
	CleanupStack::PopAndDestroy(array);

	POP(retArray);
	return retArray;
	}

TBool FilesAreIdenticaL(const TDesC& file1, const TDesC& file2)
	{
	RFile aFile;
	RFile bFile;
	TBuf8<256> aBuf;
	TBuf8<256> bBuf;
	TBool retVal = ETrue;

	TESTNOERRL(aFile.Open(fs,file1,EFileRead));
	TESTNOERRL(bFile.Open(fs,file2,EFileRead));

	do  {
		TESTNOERRL(aFile.Read(aBuf));
		TESTNOERRL(bFile.Read(bBuf));
		if (aBuf != bBuf)
			{
			retVal =  EFalse;
			break;
			}
		}
	while (aBuf.Length() > 0 || bBuf.Length() >0);

	aFile.Close();
	bFile.Close();

	return retVal;
	}

LOCAL_C void CheckMatchingL(const TDesC& aDatabase, const TDesC& aMatchtable, const TDesC& aResultTable)
	{
	_LIT(KTempResult, "c:\\result.csv");
	TFileName fileName;

	//Open existing database or create if it was not found
	//The code reads comma-seprated file and puts contacts to the database
	if (FileExists(KDatabaseFileName))
		{
		CntTest->OpenDatabaseL();
		}
	else
		{
		fileName.Format(KTestFileName, &aDatabase);
		FillInDatabaseL(fileName);
		}

	RFile outFile;
	TESTNOERRL(outFile.Replace(fs,KTempResult,EFileWrite));

	TBuf<64>  str;
	TBuf8<64> str8;

	//The reader reads comma separated phonenumbers
	fileName.Format(KTestFileName, &aMatchtable);
 	CCsvReader* reader = CCsvReader::NewLC(fileName);

	//This cycle scans through the file with all the phonenumber we want to match with
	FOREVER
		{
		CDesCArray* arr = reader->ReadNextLineL();
		if (!arr) //EOF reached
			break;
		CleanupStack::PushL(arr);

		const TPtrC& phoneNumber = (*arr)[0];

		str.Format(_L("\r\n%S;\r\n"),&phoneNumber);
		str8.Copy(str);
		test.Printf(_L("%S"),&str);
		outFile.Write(str8);

		for (TInt i = 1; i<=16; ++i) //Cycle on number of digits to match
			{
			//Match of cntmodel
			CDesCArray* matches = MatchL(phoneNumber,i);
			TESTTRUE(matches != NULL);
			if (!matches)
				break;
			PUSH(matches);

			//Matches from model
			str.Format(_L("%d;%d"),i,matches->Count());
			str8.Copy(str);
			//test.Printf(str);
			outFile.Write(str8);

			for(TInt j=0; j<matches->Count(); ++j)  //Cycle on results of matching
				{									//Each match returns an array on contacts - so we print them all
				const TPtrC& phoneStr = matches->operator[](j);
				str.Format(_L(";%S"),&phoneStr);
				str8.Copy(str);
				//test.Printf(_L("%S"),&str);
				outFile.Write(str8);
				}
			//test.Printf(_L("\r\n"));
			outFile.Write(_L8("\r\n"));
			POPD(matches);
			}
		POPD(arr);
		}
	outFile.Close();
	POPD(reader);
	CntTest->CloseDatabase();
	CntTest->DeleteDatabaseL();

	fileName.Format(KTestFileName, &aResultTable);
	TESTTRUEL(FilesAreIdenticaL(fileName,KTempResult));
	fs.Delete(KTempResult);
	}


/**
@SYMTestCaseID PIM-T-CNTMATCH-0002
@SYMTestType UT
@SYMTestPriority Normal
@SYMDEF PDEF104729/INC104452
@SYMTestCaseDesc Tests that attempting to match a phone number that exceeds
KCntMaxTextFieldLength does not cause a panic.

@SYMTestActions
1. Run a match on a number that exceeds KCntMaxTextFieldLength.
2. Check that no panic or error occurs when we attempt to match.

@SYMTestExpectedResults No panic occurs.
*/

void TestMatchWithNumberGreaterThanKCntMaxTextFieldLength()
	{
	test.Next(_L("@SYMTESTCaseID:PIM-T-CNTMATCH-0002 Testing fix for PDEF104729."));


	test.Printf(_L("Running a match with a number greater than KCntMaxTextFieldLength\n"));

	// 265 characters long.
	_LIT(KTestNumber,"333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333");

	CContactDatabase& db = *(CntTest->Db());
	CContactIdArray* array = NULL;

	TRAPD(err , array = db.MatchPhoneNumberL(KTestNumber, 5));

	CleanupStack::PushL(array);
	CleanupStack::PopAndDestroy(array);

	test( err == KErrNone );
	test.Printf(_L("Handled match with a number greater than KCntMaxTextFieldLength OK\n"));
	}

void TestMatchWithContactWithMultipleNumbersL()
	{
	/*
	 * Create contact Y with phone number 0501111111
	   Add another phone number to Y 0441111111
	   phone match 0441111111
	   Result: phone should match to the second number
	 *
	 */
	_LIT(KTelNo,"0501111111");
	_LIT(KMobNo, "0441111111");

	ResetDatabaseL();
	CreateContactL(KCntName(), KCntSurname(), KTelNo(), KMobNo());
	TESTVALUE(CheckPhoneMatchL(	KMobNo(), 10),1);
    TESTVALUE(CheckPhoneMatchL(	KTelNo(), 10),1);
		}

void TestMatchWithContactAfterEditL()
	{
	/*
	 * Create contact with number 1111111111
	 * edit the phone number to 2221111111
	 * try to do a phone match with 1111111111
	 * This should not match since the number has been edited.
	 */
	_LIT(KOrigTelNo,"1111111111");
	_LIT(KEditedTelNo,"2221111111");

	ResetDatabaseL();
	TContactItemId contactId = CreateContactL(KCntName(), KCntSurname(), KOrigTelNo(), KNullDesC());
    //	Edit the contact
	CContactItem* contactItem = CntTest->Db()->OpenContactL(contactId);
	CleanupStack::PushL(contactItem);
	CContactItemFieldSet& fieldSet = contactItem->CardFields();
	TInt pos = fieldSet.Find(KUidContactFieldVCardMapTEL);
	while (pos!=KErrNotFound)
		{
		if (fieldSet[pos].TextStorage()->Text() == KOrigTelNo())
			{
			fieldSet[pos].TextStorage()->SetTextL(KEditedTelNo());
			break;
			}
		pos = fieldSet.FindNext(KUidContactFieldVCardMapTEL, pos+1);
		}

	CntTest->Db()->CommitContactL(*contactItem);

	test (fieldSet[pos].TextStorage()->Text() == KEditedTelNo());
	CleanupStack::PopAndDestroy(contactItem);

	TESTVALUE(CheckPhoneMatchL(	KOrigTelNo(), 10),0);
	TESTVALUE(CheckPhoneMatchL(	KEditedTelNo(), 10),1);
	}

LOCAL_C void TestBestMatchingStrategyL()
    {
    ResetDatabaseL();
    CreateContactL(KCntName,KCntSurname,_L("34567"),KNullDesC);
    TESTVALUE(CheckPhoneMatchL(_L("358401234567"),KBestMatchingPhoneNumbers),0);
    TESTVALUE(CheckPhoneMatchL(_L("34567"), KBestMatchingPhoneNumbers),1);
    
    ResetDatabaseL();
    CreateContactL(KCntName,KCntSurname,_L("358401234567"),KNullDesC);
    TESTVALUE(CheckPhoneMatchL(_L("358401234567"),KBestMatchingPhoneNumbers),1);
    TESTVALUE(CheckPhoneMatchL(_L("34567"), KBestMatchingPhoneNumbers),0);
    
    ResetDatabaseL();
    CreateContactL(KCntName,KCntSurname,_L("3560 0123456"),KNullDesC);
    TESTVALUE(CheckPhoneMatchL(_L("0000 0123456"),KBestMatchingPhoneNumbers),1);
    // false positive?
    TESTVALUE(CheckPhoneMatchL(_L("123456"), KBestMatchingPhoneNumbers),1);
    
    ResetDatabaseL();
    CreateContactL(KCntName,KCntSurname,_L("1234567"),KNullDesC);
    TESTVALUE(CheckPhoneMatchL(_L("358401234567"),KBestMatchingPhoneNumbers),1);
    TESTVALUE(CheckPhoneMatchL(_L("34567"), KBestMatchingPhoneNumbers),0);
    
    ResetDatabaseL();
    CreateContactL(KCntName,KCntSurname,_L("0000 0123456"),KNullDesC);
    TESTVALUE(CheckPhoneMatchL(_L("0123456"),KBestMatchingPhoneNumbers),1);
    TESTVALUE(CheckPhoneMatchL(_L("123456"), KBestMatchingPhoneNumbers),1);
    TESTVALUE(CheckPhoneMatchL(_L("3456"), KBestMatchingPhoneNumbers),0);
    
    ResetDatabaseL();
    CreateContactL(KCntName,KCntSurname,_L("020 7700 9001"),KNullDesC);
    CreateContactL(KCntName,KCntSurname,_L("20 7700 90012"),KNullDesC);
    CreateContactL(KCntName,KCntSurname,_L("020 7700 9081"),KNullDesC);
    CreateContactL(KCntName,KCntSurname,_L("120 7700 9081"),KNullDesC);
    CreateContactL(KCntName,KCntSurname,_L("9120 7700 9081"),KNullDesC);
    TESTVALUE(CheckPhoneMatchL(_L("020 7700 9001"), KBestMatchingPhoneNumbers),1);
    TESTVALUE(CheckPhoneMatchL(_L("20 7700 90012"), KBestMatchingPhoneNumbers),1);
    TESTVALUE(CheckPhoneMatchL(_L("020 7700 9081"), KBestMatchingPhoneNumbers),3);
    TESTVALUE(CheckPhoneMatchL(_L("120 7700 9081"), KBestMatchingPhoneNumbers),3);
    TESTVALUE(CheckPhoneMatchL(_L("9120 7700 9081"), KBestMatchingPhoneNumbers),3);
    TESTVALUE(CheckPhoneMatchL(_L("20 7700 9081"), KBestMatchingPhoneNumbers),3);

    ResetDatabaseL();
    CreateContactL(KCntName,KCntSurname,_L("9999 9990 0999 999"),KNullDesC);
    CreateContactL(KCntName,KCntSurname,_L("9000 0000 0000 000"),KNullDesC);
    CreateContactL(KCntName,KCntSurname,_L("0000 0000 0000 000"),KNullDesC);
    CreateContactL(KCntName,KCntSurname,_L("0000 0000 0000 009"),KNullDesC);
    CreateContactL(KCntName,KCntSurname,_L("9 9000 000"),KNullDesC);
    CreateContactL(KCntName,KCntSurname,_L("9000 000"),KNullDesC);
    TESTVALUE(CheckPhoneMatchL(_L("9999 9990 0999 999"), KBestMatchingPhoneNumbers),1);
    TESTVALUE(CheckPhoneMatchL(_L("9000 0000 0000 000"), KBestMatchingPhoneNumbers),2);
    TESTVALUE(CheckPhoneMatchL(_L("0000 0000 0000 000"), KBestMatchingPhoneNumbers),2);
    TESTVALUE(CheckPhoneMatchL(_L("0000 0000 0000 009"), KBestMatchingPhoneNumbers),1);
    TESTVALUE(CheckPhoneMatchL(_L("9 9000 000"), KBestMatchingPhoneNumbers),2);
    TESTVALUE(CheckPhoneMatchL(_L("9000 000"), KBestMatchingPhoneNumbers),2);
    TESTVALUE(CheckPhoneMatchL(_L("0000 000"), KBestMatchingPhoneNumbers),2);
    
    ResetDatabaseL();
    CreateContactL(KCntName,KCntSurname,_L("443049607"),KNullDesC);
    TESTVALUE(CheckPhoneMatchL(_L("358443049607"), KBestMatchingPhoneNumbers),1);
    
    ResetDatabaseL();
    CreateContactL(KCntName,KCntSurname,_L("0443049607"),KNullDesC);
    TESTVALUE(CheckPhoneMatchL(_L("358443049607"), KBestMatchingPhoneNumbers),1);

    ResetDatabaseL();
    CreateContactL(KCntName,KCntSurname,_L("358443049607"),KNullDesC);
    TESTVALUE(CheckPhoneMatchL(_L("443049607"), KBestMatchingPhoneNumbers),1);
    
    ResetDatabaseL();
    CreateContactL(KCntName,KCntSurname,_L("358443049607"),KNullDesC);
    TESTVALUE(CheckPhoneMatchL(_L("0443049607"), KBestMatchingPhoneNumbers),1);
    
    ResetDatabaseL();
    CreateContactL(KCntName,KCntSurname,_L("358443049607"),KNullDesC);
    TESTVALUE(CheckPhoneMatchL(_L("3049607"), KBestMatchingPhoneNumbers),1);
    
    ResetDatabaseL();
    CreateContactL(KCntName,KCntSurname,_L("358443049607"),KNullDesC);
    TESTVALUE(CheckPhoneMatchL(_L("03049607"), KBestMatchingPhoneNumbers),1);
    
    ResetDatabaseL();
    CreateContactL(KCntName,KCntSurname,_L("443049607"),KNullDesC);
    TESTVALUE(CheckPhoneMatchL(_L("0443049607"), KBestMatchingPhoneNumbers),1);
    
    ResetDatabaseL();
    CreateContactL(KCntName,KCntSurname,_L("0443049607"),KNullDesC);
    TESTVALUE(CheckPhoneMatchL(_L("443049607"), KBestMatchingPhoneNumbers),1);
    
    ResetDatabaseL();
    CreateContactL(KCntName,KCntSurname,_L("3049607"),KNullDesC);
    TESTVALUE(CheckPhoneMatchL(_L("358443049607"), KBestMatchingPhoneNumbers),1);
    
    ResetDatabaseL();
    CreateContactL(KCntName,KCntSurname,_L("03049607"),KNullDesC);
    TESTVALUE(CheckPhoneMatchL(_L("358443049607"), KBestMatchingPhoneNumbers),1);
    
    ResetDatabaseL();
    CreateContactL(KCntName,KCntSurname,_L("03049607"),KNullDesC);
    TESTVALUE(CheckPhoneMatchL(_L("0358443049607"), KBestMatchingPhoneNumbers),1);
    
    ResetDatabaseL();
    CreateContactL(KCntName,KCntSurname,_L("4443049607"),KNullDesC);
    TESTVALUE(CheckPhoneMatchL(_L("3584443049607"), KBestMatchingPhoneNumbers),1);
    
    ResetDatabaseL();
    CreateContactL(KCntName,KCntSurname,_L("584443049607"),KNullDesC);
    TESTVALUE(CheckPhoneMatchL(_L("4443049607"), KBestMatchingPhoneNumbers),1);
    
    CreateContactL(KCntName,KCntSurname,_L("401234567"),KNullDesC);
    TESTVALUE(CheckPhoneMatchL(_L("2041234567"), KBestMatchingPhoneNumbers),0);
    
    }

/**

@SYMTestCaseID     PIM-T-CNTMATCH-0001

*/

LOCAL_C void DoTestsL()
	{
	test.Start(_L("@SYMTESTCaseID:PIM-T-CNTMATCH-0001 T_CntMatch"));


	CheckMatchingL(_L("cnt2.csv"), _L("mat2.csv"), _L("res2.csv"));

	CntTest->CreateDatabaseL();
	TestPDEF097460();
	PerfectMatchL();
	DontUnderestimateTheMeaningOfZeroL();
	Test2L();
	TestCallingAmericaL();
	Test3L();
	Test4L();
	Test5L();
	TestMatchWithNumberGreaterThanKCntMaxTextFieldLength();
	TestMatchWithContactWithMultipleNumbersL();
	TestMatchWithContactAfterEditL();
	TestBestMatchingStrategyL();

	CntTest->CloseDatabase();
	CntTest->DeleteDatabaseL();
    }



GLDEF_C TInt E32Main()
	{
    CntTest=new(ELeave) CCntTest;
	CntTest->ConstructL(test,KDatabaseFileName);
	User::LeaveIfError(fs.Connect());
    TRAPD(err,DoTestsL());
	test(__NB_failures == 0);
	CntTest->EndTestLib(err);
	fs.Close();

	return KErrNone;
    }
