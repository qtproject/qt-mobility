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
* ViewObserversBug Test module
*
*/



#include <e32test.h>
#include <cntdef.h>
#include <cntdb.h>
#include <cntitem.h>
#include <cntfield.h>
#include <cntfldst.h>
#include <cntviewbase.h>

#include "ccontactvieweventqueue.h"

_LIT(KTestName, "T_ViewObserversBug");

_LIT(KTestDbName, "c:T_ViewObserversBug.cdb");

LOCAL_D RTest test(KTestName);


class CTestResources : public CBase
    {
    public:
        static CTestResources* NewLC();
        void ConstructL();
        ~CTestResources();

        CContactDatabase* iDb;
        CContactViewEventQueue* iViewEventQueue;
        RContactViewSortOrder iViewSortOrder;
        CContactLocalView* iLocalView;
    };

CTestResources* CTestResources::NewLC()
    {
    CTestResources* self = new(ELeave) CTestResources;
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

void CTestResources::ConstructL()
    {
    iDb = CContactDatabase::ReplaceL(KTestDbName);

    iViewEventQueue = CContactViewEventQueue::NewL();

    iViewSortOrder.AppendL(KUidContactFieldFamilyName);
    iViewSortOrder.AppendL(KUidContactFieldGivenName);
    iViewSortOrder.AppendL(KUidContactFieldCompanyName);
    
    const TContactViewPreferences prefs = 
        static_cast<TContactViewPreferences>(EContactsOnly | EUnSortedAtEnd);
    iLocalView = CContactLocalView::NewL
        (*iViewEventQueue, *iDb, iViewSortOrder, prefs);

    // Wait for view to get ready
    TContactViewEvent event;
    __ASSERT_ALWAYS(iViewEventQueue->ListenForEvent(10,event),User::Invariant());
    __ASSERT_ALWAYS(event.iEventType == TContactViewEvent::EReady,User::Invariant());
    }

CTestResources::~CTestResources()
    {
    if (iLocalView) iLocalView->Close(*iViewEventQueue);
    iViewSortOrder.Close();
    delete iViewEventQueue;
    delete iDb;
    TRAP_IGNORE(CContactDatabase::DeleteDatabaseL(KTestDbName));
    }

LOCAL_C void TestRegistersObserversL()
    {
    // Create test resources
    CTestResources* res = CTestResources::NewLC();

    // Create observer1
    CContactViewEventQueue* observer1 = CContactViewEventQueue::NewL();
    CleanupStack::PushL(observer1);
    
    // Connect observer1 to iLocalView
    res->iLocalView->OpenL(*observer1);
    // Wait for EReady event to observer1
    TContactViewEvent event;
    test(observer1->ListenForEvent(10,event));
    test(event.iEventType==TContactViewEvent::EReady);

    // Create observer2
    CContactViewEventQueue* observer2 = CContactViewEventQueue::NewL();
    CleanupStack::PushL(observer2);

    // Connect observer2 to iLocalView
    res->iLocalView->OpenL(*observer2);

    // Disconnect observer1 from iLocalView
    res->iLocalView->Close(*observer1);

    // Wait for EReady event to observer2
    test(observer2->ListenForEvent(10,event));
    test(event.iEventType==TContactViewEvent::EReady);

    // Disconnect observer2 from iLocalView
    res->iLocalView->Close(*observer2);

    // Cleanup
    CleanupStack::PopAndDestroy(3);  // observer2, observer1, res
    }

/**

@SYMTestCaseID     PIM-T-VIEWOBSERVERSBUG-0001

*/

LOCAL_C void DoTestsL()
    {
	test.Start(_L("@SYMTESTCaseID:PIM-T-VIEWOBSERVERSBUG-0001 T_ViewObserversBug"));

    TestRegistersObserversL();
    test.End();
    test.Close();
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

    // Run the tests
    __UHEAP_MARK;
    TRAPD(err, DoTestsL());
    __UHEAP_MARKEND;

    // Cleanup
    delete activeScheduler;
    delete cleanupStack;

	return err;
    }
