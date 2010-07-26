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
* RecoverView Test module
*
*/



#include <e32test.h>
#include <cntviewbase.h>
#include <cntitem.h>
#include <cntfldst.h>

#include "ccontactvieweventqueue.h"

_LIT(KTestName, "T_RecoverView");

_LIT(KTestDbName, "c:T_RecoverView.cdb");

LOCAL_D RTest test(KTestName);


class CTestResources : public CBase
    {
    public:
        static CTestResources* NewLC(TBool aDamaged);
        void ConstructL(TBool aDamaged);
        ~CTestResources();

        CContactDatabase* iDb;
        CContactViewEventQueue* iViewEventQueue;
        RContactViewSortOrder iViewSortOrder;
        CContactLocalView* iLocalView;
    };

/**
 * Creates new test resources object.
 *
 * @param aDamaged  if true creates the database as initially damaged.
 */
CTestResources* CTestResources::NewLC(TBool aDamaged)
    {
    CTestResources* self = new(ELeave) CTestResources;
    CleanupStack::PushL(self);
    self->ConstructL(aDamaged);
    return self;
    }

/**
 * Creates a new, nondamaged DB.
 */
void CTestResources::ConstructL(TBool aDamaged)
    {
    if (aDamaged)
        {
        // Create an empty DB...
        CContactDatabase* db = CContactDatabase::ReplaceL(KTestDbName);
        CleanupStack::PushL(db);
        // Damage the DB...
        db->DamageDatabaseL(0x666);
        // Close the DB.
        CleanupStack::PopAndDestroy(db);
        // Open the damaged database. CContactDatabase::OpenL() does recovery
        // by calling RecoverL().
		iDb = CContactDatabase::OpenL(KTestDbName);
        }
    else
        {
        // Just create an empty DB
        iDb = CContactDatabase::ReplaceL(KTestDbName);
        }

    iViewEventQueue = CContactViewEventQueue::NewL();

    iViewSortOrder.AppendL(KUidContactFieldFamilyName);
    iViewSortOrder.AppendL(KUidContactFieldGivenName);
    iViewSortOrder.AppendL(KUidContactFieldCompanyName);
    
    iLocalView = CContactLocalView::NewL
        (*iViewEventQueue, *iDb, iViewSortOrder, EContactsOnly);
    }

CTestResources::~CTestResources()
    {
    if (iLocalView) iLocalView->Close(*iViewEventQueue);
    iViewSortOrder.Close();
    delete iViewEventQueue;
    delete iDb;
    TRAP_IGNORE(CContactDatabase::DeleteDatabaseL(KTestDbName));
    }

#if defined(_DEBUG)

LOCAL_C void TestRecoverWhenViewReadyL()
    {
	test.Next(_L("Recover database when view is ready"));

    CTestResources* res = CTestResources::NewLC(EFalse);
	
	//Creating a contact to test recovery
	_LIT(KPhoneNumLabel,"Phone Number");
	_LIT(KPhoneNum,		"+440000000000");

	CContactCard* contact		= CContactCard::NewLC();
	CContactItemField* field	= CContactItemField::NewLC(KStorageTypeText,KUidContactFieldPhoneNumber);
	
	field->SetLabelL(KPhoneNumLabel());
	field->TextStorage()->SetTextL(KPhoneNum());
	
	contact->AddFieldL(*field);
	
	TContactItemId aContactId = res->iDb->AddNewContactL(*contact);
	
	CleanupStack::Pop(field);

    // Wait 5 seconds for the view to get ready
    TContactViewEvent event;
    test(res->iViewEventQueue->ListenForEvent(5,event));
    test(event.iEventType == TContactViewEvent::EReady);

    // Damage the DB
	test.Next(_L("Damage the open database"));

    res->iDb->DamageDatabaseL(0x666);
    test(res->iDb->IsDamaged());
   
    // Damaging should not send view events
    test(!res->iViewEventQueue->ListenForEvent(5,event));

	//Attempt to read contact from the damaged database - this should fail with KErrNotReady
	test.Next(_L("Attempting to read from damaged database"));

	CContactItem* testCard = NULL;
	TRAPD(ret,testCard = res->iDb->ReadContactLC(aContactId));
	test(ret == KErrNotReady);
		
	// Recover the DB
	test.Next(_L("Recover the damaged database"));

    res->iDb->RecoverL();
		
    // View should be unavailable here
    test(res->iViewEventQueue->ListenForEvent(5,event));
    test(event.iEventType == TContactViewEvent::EUnavailable);

	//View should now be ready for use again
	test(res->iViewEventQueue->ListenForEvent(5,event));
	test(event.iEventType == TContactViewEvent::EReady);

	//Line below included to fix ARMV5 minor build warnings.
	testCard = res->iDb->ReadContactLC(aContactId);
	test(testCard != NULL);
	
	CleanupStack::PopAndDestroy(testCard);
	CleanupStack::PopAndDestroy(contact);
    CleanupStack::PopAndDestroy(res);
    }

LOCAL_C void TestOpenDamagedDbL()
    {
    test.Next(_L("Open damaged database"));

    CTestResources* res = CTestResources::NewLC(ETrue);

    // Wait 10 seconds for the view to get ready
    TContactViewEvent event;
    test(res->iViewEventQueue->ListenForEvent(10,event));
    test(event.iEventType == TContactViewEvent::EReady);
    CleanupStack::PopAndDestroy(res);
    }

LOCAL_C void TestOpenAndCloseTablesL()
    {
    test.Next(_L("Open and close tables"));

    CTestResources* res = CTestResources::NewLC(EFalse);

    // Wait 10 seconds for the view to get ready
    TContactViewEvent event;
    test(res->iViewEventQueue->ListenForEvent(5,event));
    test(event.iEventType == TContactViewEvent::EReady);

    // Close tables
    res->iDb->CloseTables();
   
    // View should get unavailable
    test(res->iViewEventQueue->ListenForEvent(5,event));
    test(event.iEventType == TContactViewEvent::EUnavailable);

    // Reopen tables
    res->iDb->OpenTablesL();

    // Wait 10 seconds for the view to get ready
    test(res->iViewEventQueue->ListenForEvent(10,event));
    test(event.iEventType == TContactViewEvent::EReady);
    CleanupStack::PopAndDestroy(res);
    }

#endif

/**

@SYMTestCaseID     PIM-T-RECOVERVIEW-0001

*/

LOCAL_C void DoTestsL()
    {
	test.Start(_L("@SYMTESTCaseID:PIM-T-RECOVERVIEW-0001 T_RecoverView"));


#if defined(_DEBUG)
	TestRecoverWhenViewReadyL();
	TestOpenDamagedDbL();
	TestOpenAndCloseTablesL();
#else
	test.Printf(_L("T_RecoverView only perform functionality testing in UDEB build"));

	// reference CntModel DLL to remove linker warning
    TRAP_IGNORE(CContactDatabase::DefaultContactDatabaseExistsL());
#endif

    test.End();
	test.Close();
    }

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
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
    TRAPD(err, DoTestsL());
	User::LeaveIfError(err);

    // Cleanup
    delete activeScheduler;
    delete cleanupStack;
	__UHEAP_MARKEND;
	return err;
    }
