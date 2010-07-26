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

#include <testexecutelog.h>
#include <cntfldst.h>
#include "clientserver.h"
#include "viewdefanyfieldstest.h"
#include "cntstd.h"

_LIT(KRun1,"SingleConfig");
_LIT(KRun2,"UniqueConfig");
_LIT(KRun3,"RepeatConfig");

_LIT(KTest1,"Testing all uids in viewdef in single configurations...");
_LIT(KTest2,"Testing all uids in viewdef in multiple unique configurations...");
_LIT(KTest3,"Testing all uids in viewdef in multiple repeat configurations...");

//Number of contacts
CViewDefAnyFieldsTest::CViewDefAnyFieldsTest() 
						: CViewDefBase(1)
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(SharedConstants::KViewDefAnyFieldsTest);
	}

TVerdict CViewDefAnyFieldsTest::doTestStepL()
	{
	
	__UHEAP_MARK;
	
	
	InitializeL();
	iIterate->Reset();
	
	const TDesC &run = ConfigSection();
	
	if( run == KRun1 )
		{
		INFO_PRINTF1(KTest1);
		doAnyFieldsTestL(iIterate->NextL());
		}
	else if( run == KRun2 )
		{
		INFO_PRINTF1(KTest2);
		doUniqueUidsTestL(iIterate->NextL());
		}
	else if( run == KRun3 )
		{
		//this panics as only one instance of a uid is allowed within a viewdef
		//only panics in debug mode
		#ifdef _DEBUG
		
		INFO_PRINTF1(KTest3);
		doIdenticalUidsTestL(iIterate->NextL());
		
		#else
		
		INFO_PRINTF1(KTest3);
		_LIT(KTestPanic,"TestPanic");
		User::Panic(KTestPanic, ECntPanicDuplicateViewFields);
		
		#endif
		}
	else
		{
		MissngTestPanic();
		}
	
	Cleanup();
	
	__UHEAP_MARKEND;
	
	return TestStepResult();
	}
	
/**
tests loading aCid with different viewdefs, it is tested with all possible uids for each viewdef,
check to see that expected fields are excluded/included
*/
void CViewDefAnyFieldsTest::doAnyFieldsTestL(const TContactItemId &aCid)
	{
	OpenL(aCid,*iViewAll, EFalse);//ETrue);
	SetContactFieldsL(*iContactItem);	
	CommitL();
	/*viewdefs ignore primary mappings stored as mapping in contenttype rather
	than within the array*/
	SINGLECOMPARE( FieldCountCompL(aCid, *iViewII, EFalse, EFalse), 0, 0);
	//hidden is reversed
	SINGLECOMPARE( FieldCountCompL(aCid, *iViewMI, ETrue, ETrue), 0, 0 );
	//"all" hidden fields are excluded
	SINGLECOMPARE( FieldCountCompL(aCid, *iViewIM, EFalse, ETrue, ETrue), 0, 0 );
	//"all" hidden fields are excluded
	SINGLECOMPARE( FieldCountCompL(aCid, *iViewMM, ETrue, ETrue, ETrue), 0, 0 );
	}
	
/**
attempt to load aCid with viewdef containing multiple(2) unique(existing) uids, 
check to see that expected fields are excluded/included
*/
void CViewDefAnyFieldsTest::doUniqueUidsTestL(const TContactItemId &aCid)
	{
	OpenL(aCid,*iViewAll, EFalse);//ETrue);
	SetContactFieldsL(*iContactItem);	
	CommitL();
	//multiple uniques uids
	SINGLECOMPARE( MultipleUidCountCompL(aCid, *iViewII, EFalse, EFalse),0 ,0 );
	}
/**
attempt to load aCid with viewdef containing multiple identical uids, this test
will panic in debug mode as CContactItemViewDef does not allow multiple identical uids
*/
void CViewDefAnyFieldsTest::doIdenticalUidsTestL(const TContactItemId &aCid)
	{
	OpenL(aCid,*iViewAll, EFalse);// ETrue);
	SetContactFieldsL(*iContactItem);	
	CommitL();
	//multiple identical uids
	SINGLECOMPARE( FieldCountCompL(aCid, *iViewII, EFalse, EFalse, EFalse, ETrue), 0 , 0 );
	}

TBool CViewDefAnyFieldsTest::FieldCountCompL(	const TContactItemId &aCid,
												CContactItemViewDef &aView, 
												const TBool aExcludeN, 
												const TBool aExcludeH,
												const TBool aZeroH,
												const TBool aMultiple)
	{
	TBool SingleTestResult = EFalse;
	TBool OverallTestResult = EFalse;
	
	TInt countM = 0;
	TInt countR = 0;
	TInt fieldsIV = 0;
	TInt emptyfsM = 0;
	TInt emptyfsR = 0;
	TInt hcountM = 0;
	TInt hcountR = 0;
	TInt hfieldsIV = 0;
	
	TInt length = 0;
	
	length = iExistingUidsArray->Count();
	
	for(TInt i = 0, j = 0; i < length;)
		{
		OverallTestResult = EFalse;
		if( (*iExistingUidsArray)[i] == 0 )
			{
			++i;
			continue;
			}
		aView.AddL(TUid::Uid((*iExistingUidsArray)[i]));
		
		ReadL(aCid,*iViewAll,EFalse);
		FieldsInView(aView, fieldsIV, hfieldsIV);
		countR = iFields->Count();
		hcountR = CountHiddenFields(*iFields);
		emptyfsR = CountEmpty(*iFields);
		Close();
		
		ReadMinimalL(aCid, aView, *iDefView,EFalse);
		countM = iFields->Count();
		hcountM = CountHiddenFields(*iFields);
		emptyfsM = CountEmpty(*iFields);
		Close();
		
		SingleTestResult = INTCOMPARE( 0 , ==,  emptyfsM, i, 0 );
		OverallTestResult = SingleTestResult;
		
		SingleTestResult = INTCOMPARE( 0 , ==,  emptyfsR, i, 0 );
		OverallTestResult = OverallTestResult && SingleTestResult;
		
		SingleTestResult = INTCOMPARE( hcountR , >,  0 , i, 0 );
		OverallTestResult = OverallTestResult && SingleTestResult;
		
		
		if(aExcludeN)
			{
			SingleTestResult = INTCOMPARE( ( (countR - fieldsIV) - hcountR ) , ==,  (countM - hcountM) , i, 0 );
			OverallTestResult = OverallTestResult && SingleTestResult;
			}
		else
			{
			SingleTestResult = INTCOMPARE( fieldsIV  , ==,  (countM - hcountM) , i, 0 );
			OverallTestResult = OverallTestResult && SingleTestResult;
			}
		if(aExcludeH)
			{
			if( aZeroH)
				{
				SingleTestResult = INTCOMPARE( 0 , ==,  hcountM, i, 0 );
				OverallTestResult = OverallTestResult && SingleTestResult;	
				}
			else
				{
				SingleTestResult = INTCOMPARE( ( hcountR - hfieldsIV) , ==,  hcountM , i, 0 );
				OverallTestResult = OverallTestResult && SingleTestResult;
				}
			}
		else
			{
			SingleTestResult = INTCOMPARE( hcountM , ==,  hfieldsIV , i, 0 );
			OverallTestResult = OverallTestResult && SingleTestResult;	
			}
		
		
		if(!OverallTestResult)
			{
			break;
			}
			
		if(aMultiple)
			{
			++j;
			if(4 == j)
				{
				j = 0;
				++i;
				aView.Reset();
				}
			}
		else
			{
			++i;
			aView.Reset();
			}
			
		countM = 0;
		countR = 0;
		fieldsIV = 0;
		emptyfsM = 0;
		emptyfsR = 0;
		hcountM = 0;
		hcountR = 0;
		hfieldsIV = 0;
		
		}
	return OverallTestResult;
	}

TBool CViewDefAnyFieldsTest::MultipleUidCountCompL(	const TContactItemId &aCid,
													CContactItemViewDef &aView, 
													const TBool aExcludeN, 
													const TBool aExcludeH
													)
	{
	TBool SingleTestResult = EFalse;
	TBool OverallTestResult = EFalse;
	
	const TInt KManyUids = 1;//number of additional uids
	TInt countM = 0;
	TInt countR = 0;
	TInt fieldsIV = 0;
	TInt emptyfsM = 0;
	TInt emptyfsR = 0;
	TInt hcountM = 0;
	TInt hcountR = 0;
	TInt hfieldsIV = 0;
	
	const TInt length = iExistingUidsArray->Count()-1;
	
	for(TInt i = 0; i < length;)
		{
		OverallTestResult = EFalse;
		AddMultipleUidsL(aView, i, KManyUids);
		
		ReadL(aCid,*iViewAll,EFalse);
		
		FieldsInView(aView, fieldsIV, hfieldsIV);
				
		countR = iFields->Count();
		hcountR = CountHiddenFields(*iFields);
		emptyfsR = CountEmpty(*iFields);
		Close();
		
		ReadMinimalL(aCid, aView, *iDefView, EFalse);
		countM = iFields->Count();
		hcountM = CountHiddenFields(*iFields);
		emptyfsM = CountEmpty(*iFields);
		Close();
		
		SingleTestResult = INTCOMPARE( 0 , ==,  emptyfsM, i, 0 ); 
		OverallTestResult = SingleTestResult;
		
		SingleTestResult = INTCOMPARE( 0 , ==,  emptyfsR, i, 0 ); 
		OverallTestResult = OverallTestResult && SingleTestResult;
		
		SingleTestResult = INTCOMPARE( hcountR , >,  0 , i, 0 ); 
		OverallTestResult = OverallTestResult && SingleTestResult;
		
		if(aExcludeN)
			{
			SingleTestResult = INTCOMPARE( ( (countR - fieldsIV) - hcountR ) , ==,  (countM - hcountM) , i, 0 );
			OverallTestResult = OverallTestResult && SingleTestResult;
			}
		else
			{
			SingleTestResult = INTCOMPARE( fieldsIV  , ==,  (countM - hcountM) , i, 0 );
			OverallTestResult = OverallTestResult && SingleTestResult;
			}
		if(aExcludeH)
			{
			SingleTestResult = INTCOMPARE( ( hcountR - hfieldsIV) , ==,  hcountM , i, 0 );
			OverallTestResult = OverallTestResult && SingleTestResult;
			}
		else
			{
			SingleTestResult = INTCOMPARE( hcountM , ==,  hfieldsIV , i, 0 );
			OverallTestResult = OverallTestResult && SingleTestResult;
			}
		
		if(!OverallTestResult)
			{
			break;
			}
			
		countM = 0;
		countR = 0;
		fieldsIV = 0;
		emptyfsM = 0;
		emptyfsR = 0;
		hcountM = 0;
		hcountR = 0;
		hfieldsIV = 0;
		
		}
	return OverallTestResult;
	}

