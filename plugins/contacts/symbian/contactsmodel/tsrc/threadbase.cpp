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


#include "threadbase.h"
_LIT(KSemaphoreNameOne, "SemaphoneNameOne");
_LIT(KSemaphoreNameTwo, "SemaphoneNameTwo");
const TInt KMaxHeapSize=0x400; // 1024 

///////////////////////////////////////////////////////////////////////////////
/* CThreadBase - Implementation                                              */
CThreadBase::CThreadBase()
	{
	}

CThreadBase::~CThreadBase()
	{
	iThread.Close();
	CloseSemaphore();
	}

void CThreadBase::Go()
	{
	iThread.Create( ThreadName(), CThreadBase::ThreadFunction, KDefaultStackSize, 0x2000, 0x20000, this );
	iThread.Logon( iThreadStatus );
	iThread.Resume();
	}
	
void CThreadBase::WaitForCompetion()
	{
	User::WaitForRequest( iThreadStatus );
	}

void CThreadBase::SetUpTestL( const TDesC& aThreadName )
	{
	// Set's up extra resources.
	CConsoleBase* console = NULL;
	TSize size;

	// Create and name an RTest
	iTest = new(ELeave) RTest(aThreadName);
	iTest->Start(_L("Starting test"));

	// Position our console window
	size = iTest->Console()->ScreenSize();
	size.iWidth = size.iWidth - 4;
	size.iHeight = (size.iHeight / 2) - 3;

	console = Console::NewL(aThreadName, size);
	delete const_cast<RTest*>(iTest)->Console();
	
	iTest->SetConsole(console);
	console->ClearScreen();
	}

void CThreadBase::TakeDownTest()
	{
	iTest->Close();
	delete iTest;
	iTest = NULL;
	}


void CThreadBase::OpenSemaphore()
	{
	TInt success = KErrNone;

	success = iSemaphoreSignal.OpenGlobal( KSemaphoreNameOne );
	if ( success == KErrNotFound )
		{
		iSemaphoreSignal.CreateGlobal( KSemaphoreNameOne, 0 );
		success = KErrNone;
		}


	success = iSemaphoreWait.OpenGlobal( KSemaphoreNameTwo );
	if ( success == KErrNotFound )
		{
		iSemaphoreWait.CreateGlobal( KSemaphoreNameTwo, 0 );
		}

	iSemaphoreOpen = ETrue;
	}

void CThreadBase::CloseSemaphore()
	{
	if (iSemaphoreOpen)
		{
		iSemaphoreSignal.Close();
		iSemaphoreWait.Close();
		}

	iSemaphoreOpen = EFalse;
	}

void CThreadBase::SyncronizeSignal()
	{
	iSemaphoreSignal.Signal();
	iSemaphoreWait.Wait();
	iSemaphoreSignal.Signal();
	}

void CThreadBase::SyncronizeWait()
	{
	iSemaphoreSignal.Wait();
	iSemaphoreWait.Signal();
	iSemaphoreSignal.Wait();
	}


void CThreadBase::RandomDelay(TInt aMax, TInt aMin) const 
	{
	TInt delay;
	TTime timeNow;
	TInt64 seed;
	TReal randomNumber;

	timeNow.UniversalTime();
	seed = timeNow.Int64();
	randomNumber = Math::FRand( seed );
	delay = static_cast<TInt>( (randomNumber * static_cast<TReal>(aMax)) + aMin ) ;
	delay = delay % aMax;

	User::After( delay );
	}

void CThreadBase::DoRunThreadL()
	{
	CActiveScheduler*  scheduler = NULL;
	scheduler = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	OpenSemaphore();
	SetUpTestL( ThreadName() );
	
	ThreadL();
	
	TakeDownTest();
	CloseSemaphore();
	
	CleanupStack::PopAndDestroy(scheduler);
	}

TInt CThreadBase::RunThread()
	{
	
	TInt retval = KErrNone;
	
    iCleanup = CTrapCleanup::New();
    
	if	(iCleanup)
		{	
		TRAP(retval, DoRunThreadL());
		}
	else
		{
		retval = KErrNoMemory;
		}
		
    delete iCleanup;
    iCleanup = NULL;
	return retval;	
	}

TInt CThreadBase::ThreadFunction(TAny* aMe)
	{
	CThreadBase* me = static_cast<CThreadBase*>(aMe);
	return me->RunThread();
	}
