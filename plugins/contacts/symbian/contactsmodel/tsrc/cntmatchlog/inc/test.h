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


#ifndef __TEST_H__
#define __TEST_H__

#define TEST(arg) theTest(arg, __LINE__)
#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__)

//-----------------------------------------------------------------------------
//
// Timing macros.
//
//-----------------------------------------------------------------------------
#define TIMERINSTALL \
	TTime start(static_cast<TInt64>(0)); \
	TTime end(static_cast<TInt64>(0));   \
	TUint delta = 0; \
	TUint cummulative = 0; \
	TUint iterations = 0;


#define TIMERRESET \
	delta = 0, cummulative = 0, iterations = 0;


#define TIMERSTART \
	start.UniversalTime();


#define TIMERSTOP \
	end.UniversalTime(); \
	delta = I64LOW(end.MicroSecondsFrom(start).Int64()); \
	iterations++; \
	cummulative += delta;
	

#ifdef INCLUDE_VERBOSE
_LIT(KExecutionTime, "Iteration %3d, execution Time: %dus\n");
_LIT(KTimerResult,   "Average time = %dus over %d iterations\n");
_LIT(KTimingError,   "Error - no timing results to display\n");

# define TIMERPRINT \
	theTest.Printf(KExecutionTime, iterations, delta);


# define TIMERRESULT \
	if (iterations) \
		{ \
		theTest.Printf(KTimerResult, cummulative/iterations, iterations); \
		} \
	else \
		{ \
		theTest.Printf(KTimingError); \
		}

#else
# define TIMERPRINT
# define TIMERRESULT
#endif
	

#define TIMERAVERAGE \
		(iterations ? cummulative/iterations : 0xFFFFFFFF)


#endif//__TEST_H__
