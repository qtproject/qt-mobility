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
* TestHelpers Test module
*
*/



#ifndef __TEST_HELPERS__
#define __TEST_HELPERS__

#include <e32test.h>


#define PUSH(x) CleanupStack::PushL(x)
#define POP(x)  CleanupStack::Pop(x)
#define POPD(x) CleanupStack::PopAndDestroy(x); x = NULL;

//////////// Logging macros ///////////////
GLDEF_C RTest test(KTestName);
//int __NB_ERR;
int __NB_failures = 0;

_LIT(KNBTestSmile, "\n{°±°}");
_LIT(KNBTestNoErrMask, "%s(%d) : runtime error: Function '%s' returned %d\n");
_LIT(KNBTestTrapMask,  "%s(%d) : runtime error: Function '%s' left with code %d\n");
_LIT(KNBTestTrueMask,  "%s(%d) : runtime error: Statement '%s' equals %d\n");


#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif


#define __ttv(x, c, l, f, val, leave)  \
	{const TInt __NB_ERR = x;	 \
	if (__NB_ERR != val) \
		__NB_PRINT_ERROR(KNBTestTrueMask, __NB_ERR, _S(c), l,_S(f), leave);}

#define TESTVALUE(x, val)  __ttv(x, #x, __LINE__,__FILE__,val,EFalse)
#define TESTVALUEL(x, val) __ttv(x, #x, __LINE__,__FILE__,val,ETrue)


#define __ttr(x, c, l, f, leave)  \
	{TBool __NB_ERR = x;	 \
	if (__NB_ERR == EFalse) \
	{__NB_PRINT_ERROR(KNBTestTrueMask, __NB_ERR, _S(c), l,_S(f), leave);} }

#define TESTTRUE(x)  __ttr(x, #x, __LINE__,__FILE__,EFalse)
#define TESTTRUEL(x) __ttr(x, #x, __LINE__,__FILE__,ETrue)

#define __ttt(x, c, l, f, leave)  \
	{const TInt __NB_ERR = x;     \
	if (__NB_ERR != KErrNone)	  \
		__NB_PRINT_ERROR(KNBTestNoErrMask, __NB_ERR, _S(c), l,_S(f), leave);}

#define TESTNOERR(x)  __ttt(x, #x, __LINE__,__FILE__, EFalse)
#define TESTNOERRL(x) __ttt(x, #x, __LINE__,__FILE__, ETrue) 		

#define __ttp(x, c, l, f, leave)		\
	{TRAPD(__NB_ERR, x);		\
	if (__NB_ERR != KErrNone)	\
		__NB_PRINT_ERROR(KNBTestTrapMask, __NB_ERR, _S(c), l,_S(f), leave);}

#define TESTTRAP(x)  __ttp(x, #x, __LINE__,__FILE__, EFalse)
#define TESTTRAPL(x) __ttp(x, #x, __LINE__,__FILE__, ETrue)

GLDEF_C void __NB_PRINT_ERROR(const TDesC& aMask, TInt aResult, const TText* aCommand, TInt aLineNum,const TText* aFileName, TBool leave = EFalse)
	{
	TBuf<0x400> str;
	str.Format(aMask, aFileName, aLineNum, aCommand, aResult);
	test.Printf(KNBTestSmile);
	test.Printf(str);
	if (leave)
		User::Leave(aResult < 0 ? aResult : KErrUnknown);
	else
		++__NB_failures;
	}
///////////////////////////////////////////////






#endif //__TEST_HELPERS__
