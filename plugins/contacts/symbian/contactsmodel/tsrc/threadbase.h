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


#ifndef __ThreadBase_H__
#define __ThreadBase_H__

// System includes
#if !defined(__E32MATH_H__)
#include <e32math.h> 
#endif
#if !defined(__E32TEST_H__)
#include <e32test.h>
#endif

// Constant

const TInt KMinDelay = 10000;
const TInt KMaxDelay = 50000;
const TInt KStandardTimeOut = 50000;

///////////////////////////////////////////////////////////////////////////////
/* CThreadBase - Specification                                               */
class CThreadBase : public CBase
	{
public: // user API
	CThreadBase();
	virtual ~CThreadBase();
	void Go();
	void WaitForCompetion();

public: // can be used by derived class.
	void SyncronizeSignal();
	void SyncronizeWait();
	void RandomDelay(TInt aMax, TInt aMin) const;

protected: // to be overridden by the user
	virtual void ThreadL() = 0;
	virtual const TDesC& ThreadName() const = 0;

private: // don't touch
	void DoRunThreadL();
	void SetUpTestL( const TDesC& aThreadName );
	void TakeDownTest();
	void OpenSemaphore();
	void CloseSemaphore();
	TInt RunThread();
	static TInt ThreadFunction(TAny* aMe);

public: // data
	RThread					iThread;
protected:
	RTest*					iTest;
private: // data
	RSemaphore				iSemaphoreSignal;
	RSemaphore				iSemaphoreWait;
	TBool					iSemaphoreOpen;
	CTrapCleanup* 			iCleanup;
	TRequestStatus			iThreadStatus;
	};

#endif // __ThreadBase_H__
