/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __T_CASESENSITIVEDB_H__
#define __T_CASESENSITIVEDB_H__

// System includes
#include <cntdb.h>

// Classes referenced
class RTest;

class CTestBase : public CActive, public MContactDbObserver
	{
public:							// ENUMERATIONS SHARED BETWEEN DRIVER & TEST
	enum
		{
		EStartTests = 0,
		ETest1 = EStartTests,
		EEndTests
		};
	
public:							// STATIC CONSTRUCT / DESTRUCT
	~CTestBase();

public:							// ACCESS
	void						Complete(TInt aReason);
	inline TBool				TestFinished() const			{ return iTestFinished; }
	inline RThread&				Thread()						{ return iThread; }
	inline TBool				Waiting() const					{ return iWaiting; }

protected:						// INTERNAL CONSTRUCTION
	CTestBase(TInt aClientNumber, TThreadPriority aThreadPriority, TInt aPriority);
	virtual void				ConstructL(const TDesC& aName);
	//
	static TInt					ThreadFunction(TAny *aParam);
	void						RunTestsL();

protected:						// TEST FRAMEWORK
	virtual void				OpenDatabaseL() = 0;
	
protected:						// FROM MContactDbObserver
	void						HandleDatabaseEventL(TContactDbObserverEvent aEvent);

private:						// FROM CActive
	void						RunL();
	void						DoCancel();

protected:						// TEST RELATED
	void						SetRequestStatus();
	void						WaitForContactsEvent(TContactDbObserverEventType aEvent, TInt aTimeInSecondsToWaitFor = 5);

	virtual void				DoTestL() = 0;

protected:						// MEMBER DATA
	// Owned by this class after the thread has been created
	CContactDatabase			*iDb;
	RTest						*iTest;
	
	TInt						iClientNumber;
	RThread						iThread;
	TThreadPriority				iThreadPriority;
	
	TBool						iWaiting;
	TBool						iTestFinished;
	TContactDbObserverEventType	iEvent;
	RTimer						iWaitTimer;
	};

class CTestThreadOne : public CTestBase
	{
public:							// FROM CTestBase
	static CTestThreadOne*		NewL(TInt aClientNumber, const TDesC& aName, TThreadPriority aThreadPriority = EPriorityNormal, TInt aPriority = EPriorityStandard);
	void						DoTestL();
	void						OpenDatabaseL();

private:						// INTERNAL
	CTestThreadOne(TInt aClientNumber, TThreadPriority aThreadPriority, TInt aPriority);
	};

class CTestThreadTwo : public CTestBase
	{
public:							// FROM CTestBase
	static CTestThreadTwo*		NewL(TInt aClientNumber, const TDesC& aName, TThreadPriority aThreadPriority, TInt aPriority = EPriorityStandard);
	void						DoTestL();
	void						OpenDatabaseL();

private:						// INTERNAL
	CTestThreadTwo(TInt aClientNumber, TThreadPriority aThreadPriority, TInt aPriority);
	};

class CTestManager : public CTimer
	{
public:							// STATIC CONSTRUCT
	static CTestManager*		NewLC();
	static CTestManager*		NewL();
	~CTestManager();

private:						// INTERNAL CONSTRUCT
	CTestManager(TInt aPriority = EPriorityStandard);
	void						ConstructL();

private:						// FROM CActive
	void						RunL();

private:						// NEW FUNCTIONS
	void						IssueTimerRequest();

private:						// MEMBER DATA
    //TInt						iTestNumber;
	//
	CTestThreadOne*				iClient1;
	CTestThreadTwo*				iClient2;
	};

#endif
