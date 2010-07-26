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
* GroupViewPanic Test module
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

_LIT(KTestName, "T_GroupViewPanic");

_LIT(KTestDbName, "c:T_GroupViewPanic.cdb");


LOCAL_D RTest test(KTestName);


class CRemoteViewTestResources : public CBase
    {
    public:
        static CRemoteViewTestResources* NewLC();
        void ConstructL();
        ~CRemoteViewTestResources();
        void CreateTestGroupsL();

        CContactDatabase* iDb;
        CContactIdArray* iContacts;
        CContactViewEventQueue* iViewEventQueue;
        RContactViewSortOrder iViewSortOrder;
        CContactRemoteView* iRemoteView;
    };
   
CRemoteViewTestResources* CRemoteViewTestResources::NewLC()
    {
    CRemoteViewTestResources* self = new(ELeave) CRemoteViewTestResources;
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

void CRemoteViewTestResources::ConstructL()
    {          
    iDb = CContactDatabase::ReplaceL(KTestDbName);
	iContacts = CContactIdArray::NewL();
    iViewEventQueue = CContactViewEventQueue::NewL();
    iViewSortOrder.AppendL(KUidContactFieldTemplateLabel);

    const TContactViewPreferences prefs = static_cast<TContactViewPreferences>
    			(EGroupsOnly |ESingleWhiteSpaceIsEmptyField | EUnSortedAtEnd);       
    iRemoteView = CContactRemoteView::NewL(*iViewEventQueue, *iDb, iViewSortOrder, prefs);
    
    // Wait for view to get ready
    TContactViewEvent event;
	
	TBool eventReady = iViewEventQueue->ListenForEvent(10,event);
    __ASSERT_ALWAYS(eventReady,User::Invariant());
    ASSERT(event.iEventType == TContactViewEvent::EReady);
    }

CRemoteViewTestResources::~CRemoteViewTestResources()
    {
    if (iRemoteView)
    	{
    	iRemoteView->Close(*iViewEventQueue);	
    	delete iViewEventQueue;
    	}
    iViewSortOrder.Close();    
    delete iContacts;
    delete iDb;
    TRAP_IGNORE(CContactDatabase::DeleteDatabaseL(KTestDbName));
    }
    
class CTestResources : public CBase
    {
    public:
        static CTestResources* NewLC();
        void ConstructL();
        void CreateTestContactsL();
        void CreateTestGroupsL();
        ~CTestResources();

        CContactDatabase* iDb;
        CContactIdArray* iContacts;
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
    iContacts = CContactIdArray::NewL();

    // Create some contacts into the database (don't create any groups)
    CreateTestContactsL();

    // Close and reopen the DB (this does not help with the bug)
    /*
    delete iDb;
    iDb = NULL;
    iDb = CContactDatabase::OpenL(KTestDbName);
    */

    iViewEventQueue = CContactViewEventQueue::NewL();

    iViewSortOrder.AppendL(KUidContactFieldTemplateLabel);
    
    // Create a groups only contact view
    const TContactViewPreferences prefs = 
        static_cast<TContactViewPreferences>(EGroupsOnly);
    iLocalView = CContactLocalView::NewL
        (*iViewEventQueue, *iDb, iViewSortOrder, prefs);
#if defined (_DEBUG)
    // Wait for view to get ready
    TContactViewEvent event;
#endif
    ASSERT(iViewEventQueue->ListenForEvent(10,event));
    ASSERT(event.iEventType == TContactViewEvent::EReady);
    }

LOCAL_C void AddFieldToContactL
        (CContactItem& aContact, TFieldType aFieldType, const TDesC& aText)
    {
    CContactItemField* field = CContactItemField::NewLC(KStorageTypeText, aFieldType);
    field->TextStorage()->SetTextL(aText);
    aContact.AddFieldL(*field);
    CleanupStack::Pop(field);
    }

LOCAL_C TContactItemId CreateTestContactL
        (CContactDatabase& aDb,
        const TDesC& aFamilyName, 
        const TDesC& aGivenName, 
        const TDesC& aCompanyName)
    {
    CContactCard* card = CContactCard::NewLC();
    AddFieldToContactL(*card, KUidContactFieldFamilyName, aFamilyName);
    AddFieldToContactL(*card, KUidContactFieldGivenName, aGivenName);
    AddFieldToContactL(*card, KUidContactFieldCompanyName, aCompanyName);
    const TContactItemId id = aDb.AddNewContactL(*card);
    CleanupStack::PopAndDestroy(card);
    return id;
    }

// Creates a few test contacts.
void CTestResources::CreateTestContactsL()
    {
    TContactItemId id;
    id = CreateTestContactL(*iDb, _L("Hakkinen"), _L("Mika"), _L("McLaren"));
    iContacts->AddL(id);
    id = CreateTestContactL(*iDb, _L("Coulthard"), _L("David"), _L("McLaren"));
    iContacts->AddL(id);
    id = CreateTestContactL(*iDb, _L("Schumacher"), _L("Michael"), _L("Ferrari"));
    iContacts->AddL(id);
    id = CreateTestContactL(*iDb, _L("Raikkonen"), _L("Kimi"), _L("Sauber"));
    iContacts->AddL(id);
    }

CTestResources::~CTestResources()
    {
    if (iLocalView)
    	{
    	iLocalView->Close(*iViewEventQueue);	
    	delete iViewEventQueue;
    	}
    iViewSortOrder.Close();    
    delete iContacts;
    delete iDb;
    TRAP_IGNORE(CContactDatabase::DeleteDatabaseL(KTestDbName));
    }

LOCAL_C void TestDeleteContactsL()
    {
    // Create test resources
    CTestResources* res = CTestResources::NewLC();

    // Delete all contacts in the DB
    for (TInt i=0; i < res->iContacts->Count(); ++i)
        {
        res->iDb->DeleteContactL((*res->iContacts)[i]);
        }

    CleanupStack::PopAndDestroy(res);
    }

void CheckEvent(const TContactViewEvent& event, 
				TContactViewEvent::TEventType expectedEventType,
				TInt expectedUnderlyingIndex,
				TContactItemId expectedItemId)
	{	
	test(event.iEventType == expectedEventType);
	test(event.iInt == expectedUnderlyingIndex);
	test(event.iContactId == expectedItemId);
	}


void CRemoteViewTestResources::CreateTestGroupsL()
    {
	test.Next(_L("Test for creating a remote view and validating that queued events are received in the correct order."));

	TContactViewEvent event;

	// Create A then "unnanmed", then rename "unnamed" to B
	test.Next(_L("Test for Create group A, then create group B"));

	CContactItem* group1 = iDb->CreateContactGroupLC(_L("A"));	
	iViewEventQueue->ListenForEvent(2,event);
	CheckEvent(event, TContactViewEvent::EItemAdded, 0, group1->Id());

	CContactItem* group0 = iDb->CreateContactGroupLC(_L(""));
	iViewEventQueue->ListenForEvent(2,event);
	CheckEvent(event, TContactViewEvent::EItemAdded, 1, group0->Id());
	 	
	CContactItem *group00 = iDb->OpenContactL(group0->Id());
	static_cast<CContactGroup*> (group00)->SetGroupLabelL(_L("B"));
	iDb->CommitContactL(*group00);
		
	// Received 3 events, grp changed, removed and added
	iViewEventQueue->ListenForEvent(2,event);
	CheckEvent(event, TContactViewEvent::EGroupChanged, 0, group00->Id());
	iViewEventQueue->ListenForEvent(2,event);
	CheckEvent(event, TContactViewEvent::EItemRemoved, 1, group00->Id());
	iViewEventQueue->ListenForEvent(2,event);
	CheckEvent(event, TContactViewEvent::EItemAdded, 1, group00->Id()); 	

	// Create D then "unnamed", then rename "unnamed" to C
	test.Next(_L("Test for Create group D, then create group C"));

	CContactItem* group2 = iDb->CreateContactGroupLC(_L("D"));	
	iViewEventQueue->ListenForEvent(2,event);
	CheckEvent(event, TContactViewEvent::EItemAdded, 2, group2->Id());

	CContactItem* group3 = iDb->CreateContactGroupLC(_L(""));
	iViewEventQueue->ListenForEvent(2,event);
	CheckEvent(event, TContactViewEvent::EItemAdded, 3, group3->Id());
	 	
	CContactItem *group4 = iDb->OpenContactL(group3->Id());
	static_cast<CContactGroup*> (group4)->SetGroupLabelL(_L("C"));
	iDb->CommitContactL(*group4);
		
	// Received 3 events, grp changed, removed and added
	iViewEventQueue->ListenForEvent(2,event);
	CheckEvent(event, TContactViewEvent::EGroupChanged, 0, group4->Id());
	iViewEventQueue->ListenForEvent(2,event);
	CheckEvent(event, TContactViewEvent::EItemRemoved, 3, group4->Id());
	iViewEventQueue->ListenForEvent(2,event);
	CheckEvent(event, TContactViewEvent::EItemAdded, 2, group4->Id()); 

	// More complex set of add and deletes.
	test.Next(_L("Test-Addgroup E, Addgroup F, Addgroup G , RemoveGroup G Addgroup H RenameGroup H as EA"));

	CContactItem* group5 = iDb->CreateContactGroupLC(_L("E"));	   
	CContactItem* group6 = iDb->CreateContactGroupLC(_L("F"));	    
	CContactItem* group7 = iDb->CreateContactGroupLC(_L("G"));    
	iDb->DeleteContactL(group7->Id());	    
	CContactItem* group8 = iDb->CreateContactGroupLC(_L("H"));	

	CContactItem *group01 = iDb->OpenContactL(group8->Id());
	static_cast<CContactGroup*> (group01)->SetGroupLabelL(_L("EA"));
	iDb->CommitContactL(*group01);

	// Should receive events as add for E F H 
	// (none for G as add, then remove) 
	// Similarly for H, the add and rename (ie remove + add ) is shrunk to a single add.
	iViewEventQueue->ListenForEvent(2,event);
	CheckEvent(event, TContactViewEvent::EItemAdded, 4, group5->Id());
	iViewEventQueue->ListenForEvent(2,event);
	CheckEvent(event, TContactViewEvent::EItemAdded, 5, group8->Id());
	iViewEventQueue->ListenForEvent(2,event);
	CheckEvent(event, TContactViewEvent::EItemAdded, 6, group6->Id());
	iViewEventQueue->ListenForEvent(2,event);
	CheckEvent(event, TContactViewEvent::EGroupChanged, 0, group8->Id());

	// Cleanup
	delete group01;
	delete group00;
	delete group4;    
	CleanupStack::PopAndDestroy(group8);
	CleanupStack::PopAndDestroy(group7);
	CleanupStack::PopAndDestroy(group6);
	CleanupStack::PopAndDestroy(group5);
	CleanupStack::PopAndDestroy(group3);
	CleanupStack::PopAndDestroy(group2);     
	CleanupStack::PopAndDestroy(group0);
	CleanupStack::PopAndDestroy(group1);
    }    

/**
@SYMTestCaseID PIM-T-GROUPVIEWPANIC-0001
@SYMTestType UT
@SYMTestPriority Urgent
@SYMDEF INC097928
@SYMTestCaseDesc Check the sequence of events when groups are added, removed or edited
@SYMTestActions
@SYMTestExpectedResults The sequence of events should be proper.
*/    
LOCAL_C void TestEventsForGroupCreationL()
    {
    // Create test resources
    CRemoteViewTestResources * pRes = CRemoteViewTestResources::NewLC();
    pRes->CreateTestGroupsL();  
    CleanupStack::PopAndDestroy(pRes);  
    }   

LOCAL_C void DoTestsL()
    {
	test.Start(_L("@SYMTESTCaseID:PIM-T-GROUPVIEWPANIC-0001 T_GroupViewPanic"));

    TestDeleteContactsL();
    TestEventsForGroupCreationL();    
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
