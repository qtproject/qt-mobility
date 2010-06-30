/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32math.h>
#include <e32cmn.h>
#include "ipcfuzztest.h"

RIpcFuzzTest::RIpcFuzzTest(RTest& aTest)
	:iTest(aTest)
	{
	// No implementation required
	}


RIpcFuzzTest::~RIpcFuzzTest()
	{
	Close();	
	}

/**

@SYMTestCaseID     PIM-IPCFUZZTEST-0001

*/

void RIpcFuzzTest::RunTestL(const TDesC& aTargetSrvName, TInt aMinFuncRange, TInt aMaxFuncRange)
	{
	TVersion version(0,1,1);

	TInt err = CreateSession(aTargetSrvName, version);
	if(err == KErrNotFound) // Server not running?
		{
		HBufC* buf = HBufC::NewLC(aTargetSrvName.Length() + 16);
		TPtr ptr = buf->Des();
		ptr.Copy(aTargetSrvName);
		ptr.Append(_L(".exe"));
		
		// Use the RProcess API to start the server.
		RProcess server;
		User::LeaveIfError(server.Create(*buf,KNullDesC));
		CleanupStack::PopAndDestroy(buf);
		
		server.SetPriority(EPriorityHigh);
		
		// Synchronise with the server.
		TRequestStatus reqStatus;
		server.Rendezvous(reqStatus);
		server.Resume();
		
		// Server will call the reciprocal static synchronisation call.
		User::WaitForRequest(reqStatus);
		server.Close();
		User::LeaveIfError(reqStatus.Int());
		
		// Create the server session.
		User::LeaveIfError(CreateSession(aTargetSrvName,version));
		}
	else
		{
		User::LeaveIfError(err);
		}
	
	itest.Next(_L("@SYMTESTCaseID:PIM-IPCFUZZTEST-0001 Start IPC Fuzz Test"));

	iTest.Printf(_L("IPC Fuzz Test against %S"), &aTargetSrvName);
	
	for(TInt func = aMinFuncRange; func < aMaxFuncRange; ++func)
		{
		iTest.Printf(_L("Fuzz Test on function number %d"), func);
		Fuzz(func, 0);
		FuzzL(func, _L("Random"));
		}
		
	}

TInt RIpcFuzzTest::Fuzz(TInt aMsg, TInt /*aNotUsed*/)
	{
	TIpcArgs args(Math::Random(), Math::Random(), Math::Random(), Math::Random());
	return SendReceive(aMsg, args);
	}

TInt RIpcFuzzTest::FuzzL(TInt aMsg, const TDesC&)
	{
	HBufC* buf = HBufC::NewLC(255);
	TPtr ptr = buf->Des();
	ptr.Fill(Math::Random(),255);

	TIpcArgs args(&ptr, &ptr, &ptr, &ptr);
	TInt ret = SendReceive(aMsg, args);

	CleanupStack::PopAndDestroy(buf);
	return ret;
	}
