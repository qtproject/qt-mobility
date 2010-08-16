/*
* Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32test.h>
#include <cntdef.h>
#include <cntdb.h>
#include <cntitem.h>
#include <cntfield.h>
#include <cntfldst.h>
#include <cntviewbase.h>

#include "ccontactvieweventqueue.h"
#include "ccontactdbeventqueue.h"

_LIT(KTestName, "T_GroupViewUpdate");

_LIT(KTestDbName, "c:T_GroupViewUpdate.cdb");

LOCAL_D RTest test(KTestName);


class CTestResources : public CBase
    {
    public:
        static CTestResources* NewLC();
        void ConstructL();
        void CreateTestContactsL();
        ~CTestResources();

        CContactDatabase* iDb;
        CContactIdArray* iContacts;
        TContactItemId iGroupId;
        CContactDbEventQueue* iDbEventQueue;
        CContactViewEventQueue* iViewEventQueue;
        RContactViewSortOrder iViewSortOrder;
        CContactLocalView* iLocalView;
        CContactViewEventQueue* iGroupViewEventQueue;
        CContactGroupView* iGroupView;
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
    iDbEventQueue = CContactDbEventQueue::NewL(iDb);

    CreateTestContactsL();

    iViewEventQueue = CContactViewEventQueue::NewL();

    iViewSortOrder.AppendL(KUidContactFieldFamilyName);
    iViewSortOrder.AppendL(KUidContactFieldGivenName);
    iViewSortOrder.AppendL(KUidContactFieldCompanyName);
    
    iLocalView = CContactLocalView::NewL
        (*iViewEventQueue, *iDb, iViewSortOrder, EContactsOnly);

    iGroupViewEventQueue = CContactViewEventQueue::NewL();
    iGroupView = CContactGroupView::NewL
        (*iDb, *iLocalView, *iGroupViewEventQueue, iGroupId, 
        CContactGroupView::EShowContactsInGroup);

    // Wait for group view to get ready
    TContactViewEvent event;
	TBool eventReady = iGroupViewEventQueue->ListenForEvent(10,event);
    __ASSERT_ALWAYS(eventReady,User::Invariant());
    ASSERT(event.iEventType == TContactViewEvent::EReady);

    // Flush all other events
    iDbEventQueue->Flush();
    iViewEventQueue->Flush();
    iGroupViewEventQueue->Flush();
    }

void CTestResources::CreateTestContactsL()
    {
    const TInt KContacts = 4;
    TInt i;
    iContacts = CContactIdArray::NewL();

    // Create contacts
    for (i=1; i <= KContacts; ++i)
        {
        CContactCard* card = CContactCard::NewLC();
        CContactItemField* field = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldFamilyName);
        TBuf<30> name;
        name.Format(_L("Contact%02d"), i);
        field->TextStorage()->SetTextL(name);
        card->AddFieldL(*field);
        CleanupStack::Pop(field);
        const TContactItemId contactId = iDb->AddNewContactL(*card);
        iContacts->AddL(contactId);
        CleanupStack::PopAndDestroy(card);
        // Eat away contact db events
        TContactDbObserverEvent event;
		TBool eventReady = iDbEventQueue->ListenForEvent(10,event);
        __ASSERT_ALWAYS(eventReady,User::Invariant());        
		}

    // Create a group
    CContactItem* group = iDb->CreateContactGroupLC(_L("TestGroup"));
    iGroupId = group->Id();
    CleanupStack::PopAndDestroy(group);

    // Connect half of the contacts to the group
    for (i=0; i < iContacts->Count(); ++i)
        {
        if (i%2 == 0)
            {
            // This call doesn't send any event from CContactDatabase, which is 
            // pretty strange...
            iDb->AddContactToGroupL((*iContacts)[i], iGroupId);            
            }
        }
    }

CTestResources::~CTestResources()
    {
    if (iGroupView) iGroupView->Close(*iGroupViewEventQueue);
    delete iGroupViewEventQueue;
    if (iLocalView) iLocalView->Close(*iViewEventQueue);
    iViewSortOrder.Close();
    delete iViewEventQueue;
    delete iDbEventQueue;
    delete iContacts;
    delete iDb;
    TRAP_IGNORE(CContactDatabase::DeleteDatabaseL(KTestDbName));
    }

LOCAL_C void CheckConsistentL
        (CContactDatabase& aDb,
        const CContactGroupView& aGroupView, 
        const CContactViewBase& aBaseView,
        TContactItemId aGroupId)
    {
    const TInt groupViewCount = aGroupView.CountL();
    test(groupViewCount <= aBaseView.CountL());
    CContactGroup* group = static_cast<CContactGroup*>(aDb.ReadContactLC(aGroupId));

    const TInt count = aBaseView.CountL();
    TInt checked = 0;
    for (TInt i=0; i < count; ++i)
        {
        const TContactItemId id = aBaseView.AtL(i);
        if (group->ContainsItem(id))
            {
            test(aGroupView.FindL(id) != KErrNotFound);
            ++checked;
            }
        }
    test(checked == groupViewCount);
    CleanupStack::PopAndDestroy(group);
    }

LOCAL_C void CheckConsistentL(CTestResources& aRes)
    {
    CheckConsistentL(*aRes.iDb, *aRes.iGroupView, *aRes.iLocalView, aRes.iGroupId);
    }

LOCAL_C void TestAddContactToGroupL()
    {
    test.Next(_L("Add a contact to the group"));

    CTestResources* res = CTestResources::NewLC();

    CheckConsistentL(*res);

    // Need to open and commit the group to get any events from CContactDatabase!
    CContactGroup* group = static_cast<CContactGroup*>(res->iDb->OpenContactLX(res->iGroupId));
    CleanupStack::PushL(group);
    // Find a contact to add into group
    for (TInt i=0; i < res->iContacts->Count(); ++i)
        {
        const TContactItemId contactId = (*res->iContacts)[i];
        if (!group->ContainsItem(contactId))
            {
            // Item found, add it to the group
            res->iDb->AddContactToGroupL(contactId, group->Id());
            // Commit the group to get group changed event from Contact Db
            res->iDb->CommitContactL(*group);
            
            // Listen to the event from the DB
            TContactDbObserverEvent dbEvent;
            test(res->iDbEventQueue->ListenForEvent(2,dbEvent));
            test(dbEvent.iType == EContactDbObserverEventGroupChanged);
            test(dbEvent.iContactId == group->Id());

            // Listen to the event from the group view
            TContactViewEvent groupViewEvent;
            test(res->iGroupViewEventQueue->ListenForEvent(2,groupViewEvent));
            //test(groupViewEvent.iEventType == TContactViewEvent::EGroupChanged);
            //test(groupViewEvent.iContactId == group->Id());
            test(groupViewEvent.iEventType == TContactViewEvent::ESortOrderChanged);
            break;
            }
        }
    CleanupStack::PopAndDestroy(2);  // group, lock

    // Eat away all events from queue to give iGroupView a chance to update
    // itself
    // BUG: This loop receives a second EGroupChanged event from the group view!
    TContactViewEvent groupViewEvent;
    while (res->iGroupViewEventQueue->ListenForEvent(2,groupViewEvent))
        {
        }
    
    CheckConsistentL(*res);
    CleanupStack::PopAndDestroy(res);
    }


LOCAL_C void TestRemoveContactFromGroupL()
    {
    test.Next(_L("Remove a contact from the group"));

    CTestResources* res = CTestResources::NewLC();

    CheckConsistentL(*res);

    // Need to open and commit the group to get any events from CContactDatabase!
    CContactGroup* group = static_cast<CContactGroup*>(res->iDb->OpenContactLX(res->iGroupId));
    CleanupStack::PushL(group);

    const TContactItemId contactId = (*group->ItemsContained())[0];
    res->iDb->RemoveContactFromGroupL(contactId, group->Id());
    // Commit the group to get group changed event from Contact Db
    res->iDb->CommitContactL(*group);
            
    // Listen to the event from the DB
    TContactDbObserverEvent dbEvent;
    test(res->iDbEventQueue->ListenForEvent(2,dbEvent));
    test(dbEvent.iType == EContactDbObserverEventGroupChanged);
    test(dbEvent.iContactId == group->Id());

    // Listen to the event from the group view
    TContactViewEvent groupViewEvent;
    test(res->iGroupViewEventQueue->ListenForEvent(2,groupViewEvent));
    //test(groupViewEvent.iEventType == TContactViewEvent::EGroupChanged);
    //test(groupViewEvent.iContactId == group->Id());
    test(groupViewEvent.iEventType == TContactViewEvent::ESortOrderChanged);

    CleanupStack::PopAndDestroy(2);  // group, lock

    // Eat away all events from queue to give iGroupView a chance to update
    // itself
    // BUG: This loop receives a second EGroupChanged event from the group view!
    while (res->iGroupViewEventQueue->ListenForEvent(2,groupViewEvent))
        {
        }
    
    // This test fails: the group view still contains contactId
    CheckConsistentL(*res);
    CleanupStack::PopAndDestroy(res);
    }

/**

@SYMTestCaseID     PIM-T-GROUPVIEWUPDATE-0001

*/

void DoTestsL()
    {
	test.Start(_L("@SYMTESTCaseID:PIM-T-GROUPVIEWUPDATE-0001 T_GroupViewUpdate"));

    TRAPD(err,TestAddContactToGroupL());
    test(err == KErrNone);
    TRAP(err,TestRemoveContactFromGroupL());
    test(err == KErrNone);
    test.End();
    test.Close();   
  	}

GLDEF_C TInt E32Main()
	{
    // Init
    __UHEAP_MARK; 
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

    // Cleanup
    delete activeScheduler;
    delete cleanupStack;
    __UHEAP_MARKEND;
	return err;
    }
