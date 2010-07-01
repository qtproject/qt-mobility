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


#ifndef __CASYNCTEST_H__
#define __CASYNCTEST_H__

#include <e32base.h>
#include <e32test.h>

class CContactOpenOperation;

class CAsyncTest : private CActive
	{
public:
	~CAsyncTest();
	static CAsyncTest* NewLC();
	
public:
	void OpenUnnamedDatabaseTestL();
	void OpenNamedDatabaseTestL();
private:
	void DoCancel();
	TInt RunError(TInt aError);
	void RunL();
	void CreateNamedDbL();
		 CAsyncTest();
	void TestResultL();
	void ConstructL();
	void NextTest();
	void CancelDatabaseTestL();
	void OpenAndCancelDatabaseTestL();
private:
	TInt iTestNo;
	CContactOpenOperation* iOpenOp; 
	RTest* iTest;
	};
	
#endif //__CASYNCTEST_H__
