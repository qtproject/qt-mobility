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
* This file define all the common values thoughout your test project
*
*/




/**
 @file 
 @publishedAll
 @released
*/

#ifndef __TE_PERFORMANCEFUNCTIONALITYTESTS_SUITEDEFS_H__
#define __TE_PERFORMANCEFUNCTIONALITYTESTS_SUITEDEFS_H__

_LIT(KStringFields, "StringFields");
_LIT(KField, "field");
_LIT(KSharedEvents,"Events");
_LIT(KSharedClients,"Clients");
_LIT(KSharedContact,"Contact");
_LIT(KSharedNextTest,"NextTest");
_LIT(KSharedMessageCount,"MessageCount");

static const TInt KConcurrentContacts = 10;
static const TInt KConcurrentTimeoutContacts = 50;
static const TInt KDefaultTime 	= 1000000;
static const TInt KStartTime 	= 0;
static const TInt KTimeDiff = 4;
static const TInt KNullEvent = -2;

// The following constant was set to "500000", representing 0.5 seconds. At 
// points in the test suite, something would take longer than this arbitrary 
// timeout limit and would thus fail. However, this does not reflect any of our
// performance targets. We do not use this test as a measure of our performance
// (only T_PERFORMANCE). So, as we want to keep the test for its functional test
// coverage, to stop it failing, we can just add a '0' on the end of the value.
// This is a short-term fix and this test should eventually be refactored or, 
// better still, replaced.
const TInt KDeviation = 5000000;	//the amount of time that an operations time
									//out may deviate from the defined time out 
									//without the test failing

// For test step panics
_LIT(KTe_PerformanceFunctionalityTestsSuitePanic,"Te_PerformanceFunctionalityTestsSuite");

#define ALLPRINT( x )	INFO_PRINTF1( x );\
						RDebug::Print( x );
						
#define ALLPRINT2( x, y )	INFO_PRINTF2( x, y );\
							RDebug::Print( x, y );
							
#define CLEAR(x) { delete x; x = NULL; }


//necessary, so that __FILE__ macro is executed
#define	FILE(file) _S(file)

//TP macro necessary to convert file name to string, otherwise compiler error.
#define TESTPRINT( compare )  	PrintTestComparison( compare,  _S(#compare), __LINE__, FILE(__FILE__) );

#define TESTPRINTI( compare, iteration ) PrintTestComparisonIteration( compare, iteration, _S(#compare), __LINE__, FILE(__FILE__) );

#define STRINGCOMPARE( compareLeft, compareOperation, compareRight, iteration, subiteration )\
	StringComparisonTestPrint(compareLeft, _S(#compareOperation), compareRight,\
	(compareLeft compareOperation compareRight), iteration, subiteration, __LINE__, FILE(__FILE__))

#define INTCOMPARE( compareLeft, compareOperation, compareRight, iteration, subiteration )\
	IntegerComparisonTestPrint(compareLeft, _S(#compareOperation), compareRight,\
	(compareLeft compareOperation compareRight), iteration, subiteration, __LINE__, FILE(__FILE__))

#endif
