/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#define __PROFILING__

#include <e32test.h>
#include <cntviewbase.h>
#include <cntitem.h>
#include <cntfldst.h>
#include "t_utils2.h"
#include "t_utils.h"

#include "ccontactvieweventqueue.h"

// Type definitions
_LIT(KTestName, "T_Cnt_ViewSortProfiling");

_LIT(KTestDbName, "c:T_Cnt_ViewSortProfiling.cdb");

LOCAL_D RTest test(KTestName);


class CTestResources : public CBase
    {
public:
    static CTestResources* NewLC(TInt *profileTime, TInt aNumberOfContactsToAddToExistingDatabase, TBool aCreateNewDatabase=ETrue);
    void ConstructL(TInt *profileTime, TInt aNumberOfContactsToAddToExistingDatabase, TBool aCreateNewDatabase);
    ~CTestResources();

	void CreateContactsL(TInt aNumberOfContacts);

    CContactDatabase* iDb;
    CContactViewEventQueue* iViewEventQueue;
    RContactViewSortOrder iViewSortOrder;
    CContactLocalView* iLocalView;
	RArray<TContactItemId> iIdsInViewSortOrder;
	CRandomContactGenerator* iRandomGenerator;
    };

CTestResources* CTestResources::NewLC(TInt *profileTime, TInt aNumberOfContactsToAddToExistingDatabase, TBool aCreateNewDatabase)
    {
    CTestResources* self = new(ELeave) CTestResources;
    CleanupStack::PushL(self);
    self->ConstructL(profileTime, aNumberOfContactsToAddToExistingDatabase, aCreateNewDatabase);
    return self;
    }

void CTestResources::ConstructL(TInt *profileTime, TInt aNumberOfContactsToAddToExistingDatabase, TBool aCreateNewDatabase)
    {
	if (aCreateNewDatabase)
		iDb = CContactDatabase::ReplaceL(KTestDbName);
	else
		iDb = CContactDatabase::OpenL(KTestDbName);

	iRandomGenerator=CRandomContactGenerator::NewL();
	iRandomGenerator->SetDbL(*iDb);
	CreateContactsL(aNumberOfContactsToAddToExistingDatabase);
	TInt numberOfContactsInDb=iDb->CountL();

    iViewEventQueue = CContactViewEventQueue::NewL(NULL, numberOfContactsInDb*2);

    iViewSortOrder.AppendL(KUidContactFieldGivenName);
    iViewSortOrder.AppendL(KUidContactFieldFamilyName);
    iViewSortOrder.AppendL(KUidContactFieldCompanyName);
    
    iLocalView = CContactLocalView::NewL(*iViewEventQueue, *iDb, iViewSortOrder, 
		static_cast<TContactViewPreferences>(EContactsOnly | EIgnoreUnSorted));

    // Wait 100 seconds for the view to get ready
    TContactViewEvent event;

	CCntTest::ProfileReset(0,1);
	TCntProfile profile[4];
	CCntTest::ProfileStart(0);
	test(iViewEventQueue->ListenForEvent(100,event));
	CCntTest::ProfileEnd(0);
	CCntTest::ProfileResult(&profile[0],0,4);
	test(event.iEventType == TContactViewEvent::EReady);

	// pass back time taken, for later logging
	profileTime[0] = profile[0].iTime;
	// times measured within LocalView.cpp
	profileTime[1] = profile[1].iTime;
	profileTime[2] = profile[2].iTime;
	// CompareFieldsL
	profileTime[3] = profile[3].iTime;
	profileTime[4] = profile[3].iCount;

	TBuf<128> result;
	result.Format(_L("* TIME TAKEN FOR VIEW TO GET READY WITH %d CONTACTS -  %d ms\n"),numberOfContactsInDb,profile[0].iTime/1000);
	RDebug::Print(result);
    }

CTestResources::~CTestResources()
    {
    if (iLocalView) iLocalView->Close(*iViewEventQueue);
    iViewSortOrder.Close();
    delete iViewEventQueue;
	delete iRandomGenerator;
	TRAP_IGNORE(CContactDatabase::DeleteDatabaseL(KTestDbName));
	iIdsInViewSortOrder.Close();
	delete iDb;
    }

void CTestResources::CreateContactsL(TInt aNumberOfContacts)
	{
	for (TInt i=0; i<aNumberOfContacts; ++i)
		iRandomGenerator->AddTypicalRandomContactL();
	}

/**

@SYMTestCaseID     PIM-T-CNT-VIEWSORTPROFILING-0001

*/

LOCAL_C void DoTestsL()
    {
	test.Start(_L("@SYMTESTCaseID:PIM-T-CNT-VIEWSORTPROFILING-0001 T_ViewSortProfiling"));

	// Delete the database
    TRAPD(err,CContactDatabase::DeleteDatabaseL(KTestDbName));
	if (err!=KErrNone && err!=KErrNotFound)
		{
		User::Leave(err);
		}
	const TInt numberOfTests=10;
	const TInt numberOfContactsToAddEachTest=100;
	TInt profileTime[numberOfTests][5];
	TInt i;

	for (i = 1; i <= numberOfTests; ++i)
		{
		CTestResources* res=NULL;
		if (i==1)
			res = CTestResources::NewLC(&profileTime[i-1][0], numberOfContactsToAddEachTest);
		else
		    res = CTestResources::NewLC(&profileTime[i-1][0], numberOfContactsToAddEachTest, EFalse);
		CleanupStack::PopAndDestroy(res);
		}

    test.End();

	// suumarise profile data in CSV format
	TInt number;
	RDebug::Print(_L("\"Num Contacts\",\"Ready time\",\"Read time\",\"Sort time\",\"Compare time\",\"Compare count\"\r"));
	for (i = 1, number=numberOfContactsToAddEachTest; i<=numberOfTests; ++i, number+=numberOfContactsToAddEachTest)
		{
		RDebug::Print(_L("%d,%d,%d,%d,%d,%d\r"), number, profileTime[i-1][0]/1000, profileTime[i-1][1]/1000, profileTime[i-1][2]/1000,
			profileTime[i-1][3]/1000, profileTime[i-1][4]);
		}
    }

GLDEF_C TInt E32Main()
	{
    // Init
    CTrapCleanup* cleanupStack = CTrapCleanup::New();
    if (!cleanupStack)
        {
        return KErrNoMemory;
        }

    CActiveScheduler* activeScheduler = new CActiveScheduler;
    if (!activeScheduler)
        {
        return KErrNoMemory;
        }
    CActiveScheduler::Install(activeScheduler);

	//Get rid of the warnings.
	//T_utils2 uses efsrv.dll, but we don't use this functions in this test code.
	//So we pretend to use efsrv to keep T_utils2.obj and linker happy
	RFs fs;
	fs.Connect();
	fs.Close();

    // Run the tests
    TRAPD(err, DoTestsL());

    // Cleanup
    delete activeScheduler;
    delete cleanupStack;
	return err;
    }
