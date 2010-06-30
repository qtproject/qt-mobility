/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __T_DBNAMETEST_H__
#define __T_DBNAMETEST_H__

#include <e32base.h>
#include <e32test.h>

class CDbNameTest : public CActive
	{
public:
	~CDbNameTest();
	static CDbNameTest* NewLC();
	void RunTestsL();
	
	void DoCancel()             {}
	TInt RunError(TInt /* aError */)  { return KErrNone; }
	void RunL();
	enum TFileNameType
	{
		EMoreThanMaxLength =0,
		EMaxLength,
		ELessThanMaxLength,
		ENoDriveName,
		EInvalidFileName,
		ENullFileName
		
	};
	
private:
    CDbNameTest();
	void ConstructL();
    void AsyncOpenDatabaseTest();
    void AsyncOpenDatabaseNullTestL();
    void AsyncOpenDatabaseInvalidTest();
    void SetAndTestFileNameL(TFileNameType aType,  RArray<TInt>& aErrArray);
		
private: 
    RBuf iLongDbName;
    RBuf iInvalidDbName;	
    RTest* iTest;
    TInt iFailedTests;
	CContactOpenOperation* iOpenOp;     
	TPtrC iCurrName;
	CActiveSchedulerWait* iActiveSchedulerWait;
	};	
	
#endif // __T_DBNAMETEST_H__
