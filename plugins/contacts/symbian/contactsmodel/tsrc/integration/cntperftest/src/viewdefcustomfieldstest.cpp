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
#include "viewdefcustomfieldstest.h"

_LIT(KRun1,"AllLabelUid");
_LIT(KRun2,"AllNewFields");
_LIT(KRun3,"AllOnlyNew");
_LIT(KRun4,"ExistingLabelUid");
_LIT(KRun5,"ExistingNewFields");
_LIT(KRun6,"ExistingOnlyNew");

_LIT(KTest1,"Testing KUidContactFieldMatchAll in viewdef with custom labels/uids...");
_LIT(KTest2,"Testing KUidContactFieldMatchAll in viewdef with new fields...");
_LIT(KTest3,"Testing KUidContactFieldMatchAll in viewdef with only new fields...");
_LIT(KTest4,"Testing Multiple Exisiting & Custom Uids in viewdef with custom labels/uids...");
_LIT(KTest5,"Testing Multiple Exisiting & Custom Uids in viewdef with new fields...");
_LIT(KTest6,"Testing Multiple Exisiting & Custom Uids in viewdef with only new fields...");

const static TInt KCustomUidCount = 100;

//Number of contacts
CViewDefCustomFieldsTest::CViewDefCustomFieldsTest():CViewDefBase(1)
/**
 * Constructor
 */
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(SharedConstants::KViewDefCustomFieldsTest);
	}

TVerdict CViewDefCustomFieldsTest::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
	
	__UHEAP_MARK;
		
	InitializeL();
	iIterate->Reset();
	
	const TDesC &run = ConfigSection();
	
	if( run == KRun1 )
		{
		INFO_PRINTF1(KTest1);
		MatchAllTestsL( iIterate->NextL() );
		}
	else if( run == KRun2 )
		{
		INFO_PRINTF1(KTest2);
		NewFieldsMATestsL( iIterate->NextL(), EFalse );
		}
	else if( run == KRun3 )
		{
		INFO_PRINTF1(KTest3);
		NewFieldsMATestsL( iIterate->NextL(), ETrue );
		}
	else if( run == KRun4 )
		{
		INFO_PRINTF1(KTest4);
		ManyUidsTestsL( iIterate->NextL() );
		}
	else if( run == KRun5 )
		{
		INFO_PRINTF1(KTest5);
		NewFieldsMUITestsL( iIterate->NextL(), EFalse );
		}
	else if( run == KRun6 )
		{
		INFO_PRINTF1(KTest6);
		NewFieldsMUITestsL( iIterate->NextL(), ETrue );
		}
	else
		{
		MissngTestPanic();
		}
	
	Cleanup();
	
	__UHEAP_MARKEND;
	
	return TestStepResult();
	}

void CViewDefCustomFieldsTest::MatchAllTestsL(const TContactItemId &aCid)
	{
	TInt fields = 0;
	
	OpenL(aCid,*iViewAll, EFalse);//ETrue);
	SetContactFieldsL(*iContactItem);
	SetContactLabelsL(*iContactItem);
	AddUIDsL(*iFields,TestConstants::KInitialUID);
	fields = iFields->Count();	
	CommitL();

	iViewII->AddL(KUidContactFieldMatchAll);
	iViewIM->AddL(KUidContactFieldMatchAll);
	iViewMM->AddL(KUidContactFieldMatchAll);
	
	SINGLECOMPARE( doAllMatchL( aCid, *iViewII, fields, EFalse, EFalse ), 0, 0 );
	SINGLECOMPARE( doAllMatchL( aCid, *iViewIM, fields, EFalse, ETrue ), 0, 0 );
	SINGLECOMPARE( doAllMatchL( aCid, *iViewMM, fields, ETrue, ETrue ), 0, 0 );
	
	iViewII->Reset();
	iViewIM->Reset();
	iViewMM->Reset();
	}

void CViewDefCustomFieldsTest::NewFieldsMATestsL(const TContactItemId &aCid, const TBool aOnlyNew)
	{
	TInt fields = 0;
	const static TInt newFields = 10;
	
	OpenL(aCid,*iViewAll, EFalse);//ETrue);
	if(aOnlyNew)
		{
		iFields->Reset();
		}
	for(TInt i = 0, uids = TestConstants::KAltUID; i < newFields; i++, uids++)
		{
		AddNewFieldL( *iFields, TestConstants::KShortString, TFieldType::Uid(uids) );
		}
	SetContactFieldsL(*iContactItem);
	fields = iFields->Count();	
	CommitL();

	iViewII->AddL(KUidContactFieldMatchAll);
	iViewIM->AddL(KUidContactFieldMatchAll);
	iViewMM->AddL(KUidContactFieldMatchAll);
	
	SINGLECOMPARE( doAllMatchL( aCid, *iViewII, fields, EFalse, EFalse, aOnlyNew ), 0, 0  );
	SINGLECOMPARE( doAllMatchL( aCid, *iViewIM, fields, EFalse, ETrue, aOnlyNew ), 0, 0  );
	SINGLECOMPARE( doAllMatchL( aCid, *iViewMM, fields, ETrue, ETrue, aOnlyNew ), 0, 0  );
	
	iViewII->Reset();
	iViewIM->Reset();
	iViewMM->Reset();
	}


TBool CViewDefCustomFieldsTest::doAllMatchL(const TContactItemId &aCid, const CContactItemViewDef &aView,
											const TInt aFieldCount, const TBool aZeroN, 
											const TBool aZeroH, const TBool aNew )
	{
	TBool SingleTestResult = EFalse;
	TBool OverallTestResult = ETrue;
	
	TInt countM = 0;
	TInt countR = 0;
	TInt emptyfsM = 0;
	TInt emptyfsR = 0;
	TInt hcountM = 0;
	TInt hcountR = 0;
	
	ReadL(aCid,*iViewAll,EFalse);
	countR = iFields->Count();
	hcountR = CountHiddenFields(*iFields);
	emptyfsR = CountEmpty(*iFields);
	Close();
	
	ReadMinimalL(aCid, aView, *iDefView, EFalse);
	countM = iFields->Count();
	hcountM = CountHiddenFields(*iFields);
	emptyfsM = CountEmpty(*iFields);
	Close();
	
	if(aNew)
		{
		countR -= emptyfsR;
		emptyfsR = 0;
		hcountR = 0;
		}
	
	SingleTestResult = INTCOMPARE(countR , ==,  aFieldCount, 0, 0 ); 
	OverallTestResult = OverallTestResult && SingleTestResult;
	
	SingleTestResult = INTCOMPARE(emptyfsM , ==,  0, 0, 0 ); 
	OverallTestResult = OverallTestResult && SingleTestResult;
	
	SingleTestResult = INTCOMPARE(emptyfsR , ==,  0, 0, 0 );
	
	if(aZeroN)
		{
		SingleTestResult = INTCOMPARE( (countM - hcountM), <=, 0, 0, 0 );
		OverallTestResult = OverallTestResult && SingleTestResult;
		}
	else
		{
		SingleTestResult = INTCOMPARE( (countM - hcountM) , ==,  (countR - hcountR) , 0, 0 );
		OverallTestResult = OverallTestResult && SingleTestResult;
		}
		
	if(aZeroH)
		{
		SingleTestResult = INTCOMPARE(hcountM , ==,  0, 0, 0 );
		OverallTestResult = OverallTestResult && SingleTestResult;
		}
	else
		{
		SingleTestResult = INTCOMPARE(hcountR , ==,  hcountM, 0, 0 );
		OverallTestResult = OverallTestResult && SingleTestResult;
		}
		
	return OverallTestResult;
	
	}

void CViewDefCustomFieldsTest::ManyUidsTestsL(const TContactItemId &aCid)
	{
	TInt fields = 0;
	
	OpenL(aCid,*iViewAll, EFalse);//ETrue);
	SetContactFieldsL(*iContactItem);
	SetContactLabelsL(*iContactItem);
	AddUIDsL(*iFields,TestConstants::KInitialUID);
	fields = iFields->Count();	
	CommitL();

	const TInt length = iExistingUidsArray->Count() - 1;
	TInt i(0);
	for(; i < length;)
		{	
		AddMultipleUidsL(*iViewII, i, 1);
		SINGLECOMPARE( doManyUidsL( aCid, *iViewII, fields, EFalse, EFalse ), i, 0 );
		iViewII->Reset();
		}
		
	const TInt flength = KCustomUidCount - 1;
	for(i = 0; i < flength;)
		{	
		AddMultipleCustomUidsL(*iViewII, i, 1);
		SINGLECOMPARE( doManyUidsL( aCid, *iViewII, fields, EFalse, EFalse ), i, i );
		iViewII->Reset();
		}
	}

void CViewDefCustomFieldsTest::NewFieldsMUITestsL(const TContactItemId &aCid, const TBool aOnlyNew)
	{
	TInt fields = 0;
	const static TInt newFields = 10;
	
	OpenL(aCid,*iViewAll, EFalse);//ETrue);
	if(aOnlyNew)
		{
		iFields->Reset();
		}
	TInt i(0);
	for(TInt uids = TestConstants::KInitialUID; i < newFields; i++, uids++)
		{
		AddNewFieldL( *iFields, TestConstants::KShortString, TFieldType::Uid(uids) );
		}
	SetContactFieldsL(*iContactItem);
	fields = iFields->Count();	
	CommitL();

	const TInt length = iExistingUidsArray->Count()-1;
	for(i = 0; i < length;)
		{	
		AddMultipleUidsL(*iViewII, i, 1);
		SINGLECOMPARE( doManyUidsL( aCid, *iViewII, fields, EFalse, EFalse ) , i, 0 );
		iViewII->Reset();
		}
		
	const TInt flength = KCustomUidCount - 1;
	for(i = 0; i < flength;)
		{	
		AddMultipleCustomUidsL(*iViewII, i, 1);
		SINGLECOMPARE( doManyUidsL( aCid, *iViewII, fields, EFalse, EFalse ), i, 0 );
		iViewII->Reset();
		}
	}
	
TBool CViewDefCustomFieldsTest::doManyUidsL(const TContactItemId &aCid, const CContactItemViewDef &aView,
											const TInt aFieldCount, const TBool aExcludeN, 
											const TBool aExcludeH )
	{
	TBool SingleTestResult = EFalse;
	TBool OverallTestResult = ETrue;
	
	TInt countM = 0;
	TInt countR = 0;
	TInt fieldsIV = 0;

	TInt emptyfsR = 0;
	TInt hcountM = 0;
	TInt hcountR = 0;
	TInt hfieldsIV = 0;
	
	ReadL(aCid,*iViewAll,EFalse);
	FieldsInView(aView, fieldsIV, hfieldsIV, ETrue);
	countR = iFields->Count();
	hcountR = CountHiddenFields(*iFields);
	emptyfsR = CountEmpty(*iFields);
	Close();
	
	ReadMinimalL(aCid, aView, *iDefView, EFalse);
	countM = iFields->Count();
	hcountM = CountHiddenFields(*iFields);

	Close();
	
	SingleTestResult = INTCOMPARE( (countR - emptyfsR) , ==,  aFieldCount, 0, 0 ); 
	OverallTestResult = OverallTestResult && SingleTestResult;
	
	if(aExcludeN)
		{
		SingleTestResult = INTCOMPARE( ( (countR - fieldsIV) - hcountR ) , ==,  (countM - hcountM) , 0, 0 ); 
		OverallTestResult = OverallTestResult && SingleTestResult;
		}
	else
		{
		SingleTestResult = INTCOMPARE(  fieldsIV  , ==,  (countM - hcountM) , 0, 0 );
		OverallTestResult = OverallTestResult && SingleTestResult;
		}
	if(aExcludeH)
		{
		SingleTestResult = INTCOMPARE(  (hcountR - hfieldsIV) , ==,  hcountM , 0, 0 );
		OverallTestResult = OverallTestResult && SingleTestResult;
		}
	else
		{
		SingleTestResult = INTCOMPARE(  hcountM , ==,  hfieldsIV , 0, 0 );
		OverallTestResult = OverallTestResult && SingleTestResult;
		}
		
	return OverallTestResult;
	}

void CViewDefCustomFieldsTest::AddMultipleCustomUidsL( CContactItemViewDef &aView, TInt &aPos, const TInt &aMany )
	{
	
	static TInt lastPlace = 0;	
	aView.Reset();
	aView.AddL( TUid::Uid( TestConstants::KInitialUID + aPos ) );
	
	for(TInt i = 0; (lastPlace < KCustomUidCount) && (i < aMany); ++lastPlace, ++i)
		{
		if(lastPlace == aPos)
			{
			--i;
			continue;
			}
		aView.AddL( TUid::Uid( TestConstants::KInitialUID + lastPlace ) );
		}
	if( lastPlace == KCustomUidCount )
		{
		lastPlace = 0;
		++aPos;//this should never equal count
		_LIT(KNextCustom,"nextCustom %d");
		INFO_PRINTF2(KNextCustom, aPos);
		}
		
	}

