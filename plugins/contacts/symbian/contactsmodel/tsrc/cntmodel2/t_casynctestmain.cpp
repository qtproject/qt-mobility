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



#include "t_casynctest.h"

void RunTestL()
	{
	CAsyncTest* asyncTest = CAsyncTest::NewLC();
	CleanupStack::PopAndDestroy(asyncTest);
	asyncTest = NULL;
	}


GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CActiveScheduler* scheduler=new CActiveScheduler;
	if (scheduler)
		{
		CActiveScheduler::Install(scheduler);
		CTrapCleanup* cleanup = CTrapCleanup::New();
		if (cleanup)
			{
			TRAPD(err,RunTestL());	
			__ASSERT_ALWAYS(err == KErrNone, User::Panic(_L("DBDumper Failed"),err) ); 
			delete cleanup;
			}
		delete scheduler;
		}
	__UHEAP_MARKEND;
	return KErrNone;
    }


