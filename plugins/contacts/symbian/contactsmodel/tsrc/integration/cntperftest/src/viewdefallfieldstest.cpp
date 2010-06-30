/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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


/**
 @file
 @publishedAll
 @released
*/

#include <e32panic.h>
#include <testexecutelog.h>
#include <cntfldst.h>
#include "clientserver.h"
#include "viewdefallfieldstest.h"

_LIT(KTest1,"Testing ...");

const LOCAL_D TInt KFields = 67;
const LOCAL_D TInt KHfields = 3;

//Number of contacts
CViewDefAllFieldsTest::CViewDefAllFieldsTest():CViewDefBase(5)
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(SharedConstants::KViewDefAllFieldsTest);
	}

TVerdict CViewDefAllFieldsTest::doTestStepL()
	{
	
	__UHEAP_MARK;

	InitializeL();
	iIterate->Reset();
	INFO_PRINTF1(KTest1);
	FieldCountTestL();
	Cleanup();
	__UHEAP_MARKEND;
	
	return TestStepResult();
	}
/**viewdef tests where contact item is opened/read using empty viewdefs or one containng,
KUidContactFieldMatchAll only*/
void CViewDefAllFieldsTest::FieldCountTestL()
	{		
	iViewII->AddL(KUidContactFieldMatchAll);
	iViewIM->AddL(KUidContactFieldMatchAll);
	iViewMI->AddL(KUidContactFieldMatchAll);
	iViewMM->AddL(KUidContactFieldMatchAll);
	
	//commented out tests that fail
	SINGLECOMPARE( FieldCountCompL(iIterate->NextL(),*iViewII,EFalse,EFalse,EFalse),0,0 );
	SINGLECOMPARE( FieldCountCompL(iIterate->NextL(),*iViewIM,EFalse,EFalse,ETrue),0,0 );
	//the behaviour of this test is inverted with regards to hidden fields, they are excluded.
	SINGLECOMPARE( FieldCountCompL(iIterate->NextL(),*iViewMI,EFalse,ETrue,ETrue),0,0 );
	SINGLECOMPARE( FieldCountCompL(iIterate->NextL(),*iViewMM,EFalse,ETrue,ETrue),0,0 );
	//using the default viewdefs used in open and read causes a discrepency
	SINGLECOMPARE( FieldCountCompL(iIterate->NextL(),*iViewAll,ETrue,EFalse,ETrue),0,0 );
	
	iViewII->Reset();
	iViewIM->Reset();
	iViewMI->Reset();
	iViewMM->Reset();
	}
/**opens and read contct aCid using the default viewdefs. 
then it reads and minimally reads aCid using aView, it stores the number relevant fields after
each read/open, then checks that the values match the expected resluts as constrained by aZeroN/H,
aZeroN == all normal fields should be excluded, aZeroH == all hidden fields should be excluded*/
TBool CViewDefAllFieldsTest::FieldCountCompL(	const TContactItemId aCid,
										const CContactItemViewDef &aView, 
										const TBool aDefault, 
										const TBool aZeroN, 
										const TBool aZeroH)
	{
	TBool SingleCheck = EFalse;
	TBool OverallResult = ETrue;
	
	TInt counti = 0;
	TInt countr = 0;
	TInt countrv = 0;
	TInt countrAll = 0;
	TInt countrAllv = 0;
	TInt countrm = 0;
	
	TInt hcounti = 0;
	TInt hcountr = 0;
	TInt hcountrv = 0;
	TInt hcountrm = 0;
	
	OpenL(aCid,aView, ETrue);
	SetContactFieldsL(*iContactItem);
	counti = iFields->Count();
	hcounti = CountHiddenFields(*iFields);
	CommitL();
	
	ReadL(aCid,*iViewAll,EFalse);
	countrv = CountEmpty(*iFields);
	countrAllv = iFields->Count();
	hcountrv = CountHiddenFields(*iFields);
	Close();
	
	ReadL(aCid,aView,aDefault);
	countr = CountEmpty(*iFields);
	countrAll = iFields->Count();
	hcountr = CountHiddenFields(*iFields);
	Close();
	
	ReadMinimalL(aCid, aView, *iDefView, aDefault);
	countrm = iFields->Count();
	hcountrm = CountHiddenFields(*iFields);
	Close();
		
	SingleCheck = INTCOMPARE( countrAllv, ==, counti, 0, 0 );
	OverallResult = OverallResult && SingleCheck;
	
	SingleCheck = INTCOMPARE(hcounti , ==,  hcountrv, 0, 0 );
	OverallResult = OverallResult && SingleCheck;
	
	SingleCheck = INTCOMPARE(countrv , ==,  0, 0, 0 );
	OverallResult = OverallResult && SingleCheck;
	
	SingleCheck = INTCOMPARE( ( countrAll - countr) , ==,  countrm , 0, 0 ); 
	OverallResult = OverallResult && SingleCheck;
	
	
	if(aZeroN)
		{
		SingleCheck = INTCOMPARE( ( countr - hcountr ) , ==,  ( counti - hcounti ) , 0, 0 );
		OverallResult = OverallResult && SingleCheck;
	
		SingleCheck = INTCOMPARE( ( countrm - hcountrm ) , ==,  0 , 0, 0 );
		OverallResult = OverallResult && SingleCheck;
	
		}
	else
		{
		SingleCheck = INTCOMPARE( countr , ==,  0 , 0, 0 );
		OverallResult = OverallResult && SingleCheck;
	
		SingleCheck = INTCOMPARE( (countrm - hcountrm) , ==,  KFields , 0, 0 );
		OverallResult = OverallResult && SingleCheck;
	
		SingleCheck = INTCOMPARE( (countrm - hcountrm) , ==,  (counti - hcounti), 0, 0 );
		OverallResult = OverallResult && SingleCheck;
	
		}
		
	if(aZeroH)
		{
		SingleCheck = INTCOMPARE(hcountrm , ==,  0, 0, 0 );
		OverallResult = OverallResult && SingleCheck;
	
		SingleCheck = SINGLECOMPARE( ( (hcountr == 0) || ( (hcountr == hcountrv) && (countr == countrAll) ) ), 0, 0 );
		OverallResult = OverallResult && SingleCheck;	
		}
	else
		{
		SingleCheck = INTCOMPARE(hcountrm , ==,  KHfields, 0, 0 );
		OverallResult = OverallResult && SingleCheck;
			
		SingleCheck = INTCOMPARE(hcountr , ==,  hcountrm, 0, 0 );
		OverallResult = OverallResult && SingleCheck;
	
		SingleCheck = INTCOMPARE(hcountrm , ==,  hcounti, 0, 0 );
		OverallResult = OverallResult && SingleCheck;	
		}
		
	return OverallResult;
	}

