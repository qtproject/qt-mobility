/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __T_CNTTEST_H__
#define __T_CNTTEST_H__


#include "t_cnttestimplementor.h"


// CCntTest - the base class defining for the Contact Test abstraction. 
// This class is the base implementation of the left hand side of the
// the bridge pattern. It aggregates the test implementation and 
// defines (test procedures) virtual methods.

class CCntTest : public CBase
	{
public:
	
	~CCntTest ()
		{
		iCntTestImpl.CloseDatabase();
		};

protected:

	inline CCntTest (CCntTestImplementor& aCntTestImpl);
	//Virtual methods - Test procedures
	

protected:
	CCntTestImplementor& iCntTestImpl;
	};
	
inline CCntTest::CCntTest(CCntTestImplementor& aCntTestImpl):iCntTestImpl(aCntTestImpl){};

	
	
#endif //__T_CNTTEST_H__	
