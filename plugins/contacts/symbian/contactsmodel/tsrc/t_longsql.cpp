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


#include "t_longsql.h"

#include <e32test.h>
#include <cntfield.h>

RTest TheTest(_L("Building a long SQL query"));

////////////////////////////////
// CSqlSearch class definition
////////////////////////////////
CSqlSearcher::CSqlSearcher()
	{
	}

CSqlSearcher::~CSqlSearcher()
	{
	delete iDb;
	}

void CSqlSearcher::ConstructL()
	{
	iDb = CContactDatabase::ReplaceL();
	}

CSqlSearcher* CSqlSearcher::NewLC()
	{
	CSqlSearcher* self = new(ELeave) CSqlSearcher;
	CleanupStack::PushL(self);
	self->ConstructL();
	return(self);
	}

void CSqlSearcher::IdleFindCallback()
	{
	CActiveScheduler::Stop();
	}

void CSqlSearcher::DoSearchL(const TDesC& aString, TUid aFieldId)
	{
    CContactItemFieldDef* fieldsToSearch = new (ELeave) CContactItemFieldDef;
    CleanupStack::PushL(fieldsToSearch);

    fieldsToSearch->AppendL(aFieldId);

    CIdleFinder* idleFinder = iDb->FindAsyncL(aString, fieldsToSearch, this);
    CleanupStack::PushL(idleFinder);

    CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(idleFinder);

    CleanupStack::PopAndDestroy(fieldsToSearch);
	}

////////////////////////////////
// Main test:
////////////////////////////////

static void doMainL()
	{
	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	CSqlSearcher* searcher = CSqlSearcher::NewLC();
	HBufC* buff = HBufC::NewLC(1024);
	TPtr ptr = buff->Des();

	TheTest.Next(_L("A very short search string"));

	ptr.Zero();
	ptr.AppendFill(TChar('x'), 3);
	searcher->DoSearchL(*buff, KUidContactFieldFamilyName);

	TheTest.Next(_L("A fairly long search string"));

	ptr.Zero();
	ptr.AppendFill(TChar('x'), 233);
	searcher->DoSearchL(*buff, KUidContactFieldFamilyName);

	TheTest.Next(_L("A very slightly longer search string"));

	ptr.Zero();
	ptr.AppendFill(TChar('x'), 234);
	searcher->DoSearchL(*buff, KUidContactFieldFamilyName);

	TheTest.Next(_L("A very long search string"));

	ptr.Zero();
	ptr.AppendFill(TChar('x'), 600);
	searcher->DoSearchL(*buff, KUidContactFieldFamilyName);

	CleanupStack::PopAndDestroy(buff);
	CleanupStack::PopAndDestroy(searcher);
	CleanupStack::PopAndDestroy(scheduler);
	}

/**

@SYMTestCaseID     PIM-T-LONGSQL-0001

*/

GLDEF_C TInt E32Main()
    {
	__UHEAP_MARK;
	TheTest.Start(_L("@SYMTESTCaseID:PIM-T-LONGSQL-0001 Initialising"));


	CTrapCleanup* cleanup = CTrapCleanup::New();

	TRAPD(error, doMainL());
	TheTest(error == KErrNone);

	// Cleanup & close
	delete cleanup;

	TheTest.End();
	TheTest.Close();

	__UHEAP_MARKEND;
	return KErrNone;
    }
