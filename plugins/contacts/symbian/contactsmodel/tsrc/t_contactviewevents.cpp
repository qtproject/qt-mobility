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


#include <e32test.h>
#include <cntdef.h>
#include <cntdb.h>
#include <cntitem.h>
#include <cntfield.h>
#include <cntfldst.h>
#include <cntviewbase.h>

#include "ccontactvieweventqueue.h"
#include "ccontactdbeventqueue.h"

_LIT(KTestName, "T_GroupViewEvents");

_LIT(KTestDbName, "c:T_GroupViewEvents.cdb");

_LIT(KTestGrpName, "T_Group");

_LIT(KRemoteView, "Test_UnderlyingRemoteView");

LOCAL_D RTest test(KTestName);

class CTestResources : public CBase
    {
    public:
        static CTestResources* NewLC(TBool aShowContactInGrp);
        static CTestResources* NewLC(TBool aShowContactInGrp, TBool aUnderlyingRemoteView);
        void ConstructL(TBool aUnderlinedRemoteView = EFalse);
        TContactItemId CreateTestContactL(const TDesC& aFamilyName);
        ~CTestResources();

        CContactDatabase* iDb;
        CContactDbEventQueue* iDbEventQueue;
        CContactViewEventQueue* iLocalViewEventQueue;
        CContactViewEventQueue* iRemoteViewEventQueue;
        RContactViewSortOrder iViewSortOrder;
        CContactLocalView* iLocalView;
        CContactNamedRemoteView* iRemoteView;
        
        CContactViewEventQueue* iOldAPIGroupViewEventQueue;
        CContactViewEventQueue* iOldAPIFindViewEventQueue;   
        CContactViewEventQueue* iOldAPISubViewEventQueue;
        
        CContactViewEventQueue* iNewAPIGroupViewEventQueue;
        CContactViewEventQueue* iNewAPIFindViewEventQueue;
        CContactViewEventQueue* iNewAPISubViewEventQueue;
        
        CContactGroupView* iGroupViewOldAPI;
        CContactFindView* iFindViewOldAPI;
        CContactSubView* iSubViewOldAPI;
        
        CContactGroupView* iGroupViewNewAPI;
        CContactFindView* iFindViewNewAPI;
        CContactSubView* iSubViewNewAPI;

        CContactIdArray* iContactIdArray;
        TContactItemId iTestContactId;
        TContactItemId iGrpId;
        
        TBool		   iShowContactInGrp;
    private:
  	    CTestResources(TBool aShowContactInGrp);
    
    };

CTestResources::CTestResources(TBool aShowContactInGrp)
	: iShowContactInGrp(aShowContactInGrp)
	{
	}

CTestResources* CTestResources::NewLC(TBool aShowContactInGrp)
    {
    CTestResources* self = new(ELeave) CTestResources(aShowContactInGrp);
    
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }	
    
CTestResources* CTestResources::NewLC(TBool aShowContactInGrp, TBool aUnderlyingRemoteView)
	{
	if(aUnderlyingRemoteView == EFalse)
		{
		return NewLC(aShowContactInGrp);
		}
	
	CTestResources* self = new(ELeave) CTestResources(aShowContactInGrp);
	CleanupStack::PushL(self);
	self->ConstructL(ETrue);
	return self;
	}        

void CTestResources::ConstructL(TBool aUnderlinedRemoteView)
    {
    iDb = CContactDatabase::ReplaceL(KTestDbName);
    iDbEventQueue = CContactDbEventQueue::NewL(iDb);
   
    iViewSortOrder.AppendL(KUidContactFieldFamilyName);
    iViewSortOrder.AppendL(KUidContactFieldGivenName);
    iViewSortOrder.AppendL(KUidContactFieldCompanyName);
    
    iContactIdArray = CContactIdArray::NewL();
    
    CContactItem* grpItem = iDb->CreateContactGroupL(KTestGrpName);
    iGrpId = grpItem->Id();
    delete grpItem;
    
	if(aUnderlinedRemoteView == EFalse)
	{
		iLocalViewEventQueue = CContactViewEventQueue::NewL();
		iLocalView = CContactLocalView::NewL
			(*iLocalViewEventQueue, *iDb, iViewSortOrder, EContactsOnly);
		}
	else
		{
		iRemoteViewEventQueue = CContactViewEventQueue::NewL();
		iRemoteView = CContactNamedRemoteView::NewL
			(*iRemoteViewEventQueue, KRemoteView, *iDb, iViewSortOrder, EContactsOnly); 
		}

    iOldAPIGroupViewEventQueue = CContactViewEventQueue::NewL();
    
    CContactGroupView::TGroupType grpType = iShowContactInGrp ? 
    										CContactGroupView::EShowContactsInGroup : 
    										CContactGroupView::EShowContactsNotInAnyGroup;
    
    CContactViewBase* underlyingView = aUnderlinedRemoteView?(static_cast<CContactViewBase*>(iRemoteView)):(static_cast<CContactViewBase*>(iLocalView)); //do not take the ownership
    // Create a group view to test old functionality/API
    iGroupViewOldAPI = CContactGroupView::NewL(*iDb, *underlyingView, *iOldAPIGroupViewEventQueue, KTestGrpName, grpType);
        
	iOldAPIFindViewEventQueue = CContactViewEventQueue::NewL();
	CPtrCArray* findWordArray = new (ELeave) CPtrCArray(1);
	findWordArray->AppendL(_L(""));
	// Create a find view to test old functionality/API
	iFindViewOldAPI = CContactFindView::NewL(*iDb, *underlyingView, *iOldAPIFindViewEventQueue, findWordArray);

	iOldAPISubViewEventQueue = CContactViewEventQueue::NewL();
	// Create a sub view to test old functionality/API
	iSubViewOldAPI = CContactSubView::NewL(*iOldAPISubViewEventQueue,*iDb, *underlyingView,_L(">a"), _L("<z"));	
		
	iNewAPIGroupViewEventQueue = CContactViewEventQueue::NewL();
    // Create a group view to test new functionality/API
    iGroupViewNewAPI = CContactGroupView::NewL(*underlyingView, *iDb, *iNewAPIGroupViewEventQueue, KTestGrpName, grpType);

	iNewAPIFindViewEventQueue = CContactViewEventQueue::NewL();
	// Create a find view to test the new functionality/API 
	iFindViewNewAPI = CContactFindView::NewL(*underlyingView, *iDb, *iNewAPIFindViewEventQueue, findWordArray);
		
	iNewAPISubViewEventQueue = CContactViewEventQueue::NewL();
	// Create a sub view to test old functionality/API
	iSubViewNewAPI = CContactSubView::NewL(*underlyingView, *iDb, *iNewAPISubViewEventQueue, _L(">a"), _L("<z"));

	
	findWordArray->Reset();
	delete findWordArray;

    // Wait for contact views derived classes to get ready
    TContactViewEvent event;
    if(aUnderlinedRemoteView == EFalse)
    	{
    	__ASSERT_ALWAYS(iOldAPIGroupViewEventQueue->ListenForEvent(10,event), User::Invariant());
    	__ASSERT_ALWAYS(event.iEventType == TContactViewEvent::EReady, User::Invariant());
    	__ASSERT_ALWAYS(iOldAPIFindViewEventQueue->ListenForEvent(10,event), User::Invariant());
    	__ASSERT_ALWAYS(event.iEventType == TContactViewEvent::EReady, User::Invariant());
    	__ASSERT_ALWAYS(iOldAPISubViewEventQueue->ListenForEvent(10,event), User::Invariant());
    	__ASSERT_ALWAYS(event.iEventType == TContactViewEvent::EReady, User::Invariant());

    	__ASSERT_ALWAYS(iNewAPIGroupViewEventQueue->ListenForEvent(10,event), User::Invariant());
    	__ASSERT_ALWAYS(event.iEventType == TContactViewEvent::EReady, User::Invariant());
    	__ASSERT_ALWAYS(iNewAPIFindViewEventQueue->ListenForEvent(10,event), User::Invariant());
    	__ASSERT_ALWAYS(event.iEventType == TContactViewEvent::EReady, User::Invariant());
    	__ASSERT_ALWAYS(iNewAPISubViewEventQueue->ListenForEvent(10,event), User::Invariant());
    	__ASSERT_ALWAYS(event.iEventType == TContactViewEvent::EReady, User::Invariant());
    	}

    // Flush all other events
    iDbEventQueue->Flush();
    if(aUnderlinedRemoteView == EFalse)
    	{
    	iLocalViewEventQueue->Flush();
    	}
    else
    	{
    	iRemoteViewEventQueue->Flush();
    	}
    
    iOldAPIGroupViewEventQueue->Flush();
    iOldAPIFindViewEventQueue->Flush();
    iOldAPISubViewEventQueue->Flush();
    
    iNewAPIGroupViewEventQueue->Flush();
    iNewAPIFindViewEventQueue->Flush();
    iNewAPISubViewEventQueue->Flush();
    }

TContactItemId CTestResources::CreateTestContactL(const TDesC& aFamilyName)
    {
    // Create a contact card
    CContactCard* card = CContactCard::NewLC();
    // Create a name field
    CContactItemField* field = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldFamilyName);
    field->TextStorage()->SetTextL(aFamilyName);
    card->AddFieldL(*field);
    CleanupStack::Pop(field);
    // Create a phone number field
    CContactItemField* phoneField = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldPhoneNumber);
    phoneField->TextStorage()->SetTextL(_L("555"));
    card->AddFieldL(*phoneField);
    CleanupStack::Pop(phoneField);
    
    // Add the contact to the DB
    const TContactItemId contactId = iDb->AddNewContactL(*card);
    CleanupStack::PopAndDestroy(card);
	    
    // Eat away contact db events
    TContactDbObserverEvent event;
    while 
        (iDbEventQueue->ListenForEvent(10,event) && 
        (event.iType != EContactDbObserverEventContactAdded ||
        event.iContactId != contactId))
        {
        }
    iTestContactId = contactId;
    
    if(iShowContactInGrp)
    	{
	   	iDb->AddContactToGroupL(contactId, iGrpId);
    	}

    return contactId;
    }

CTestResources::~CTestResources()
    {
    if (iGroupViewOldAPI) 
    	iGroupViewOldAPI->Close(*iOldAPIGroupViewEventQueue);
    delete iOldAPIGroupViewEventQueue;

    if (iFindViewOldAPI)
    	iFindViewOldAPI->Close(*iOldAPIFindViewEventQueue);
    delete iOldAPIFindViewEventQueue;
    
    if (iSubViewOldAPI)
    	iSubViewOldAPI->Close(*iOldAPISubViewEventQueue);
    delete iOldAPISubViewEventQueue;
    
    if (iGroupViewNewAPI) 
    	iGroupViewNewAPI->Close(*iNewAPIGroupViewEventQueue);
    delete iNewAPIGroupViewEventQueue;

    if (iFindViewNewAPI)
    	iFindViewNewAPI->Close(*iNewAPIFindViewEventQueue);
    delete iNewAPIFindViewEventQueue;

    if (iSubViewNewAPI)
    	iSubViewNewAPI->Close(*iNewAPISubViewEventQueue);
    delete iNewAPISubViewEventQueue;
    
    if (iLocalView) 
    	iLocalView->Close(*iLocalViewEventQueue);
    delete iLocalViewEventQueue;
     
    if (iRemoteView)	
    	iRemoteView->Close(*iRemoteViewEventQueue);
    delete iRemoteViewEventQueue;	
    
    iContactIdArray->Reset();
    delete iContactIdArray;
    
    iViewSortOrder.Close(); 
    delete iDbEventQueue;
    delete iDb;
    TRAP_IGNORE(CContactDatabase::DeleteDatabaseL(KTestDbName));
    }

LOCAL_C CTestResources* DoTestAddContactAPILC(TBool aShowContactInGrp)
    {
    //add 2 contacts and check if localview send events
    CTestResources* res = CTestResources::NewLC(aShowContactInGrp);
    
    const TContactItemId contactId = res->CreateTestContactL(_L("Joplin"));
    res->iContactIdArray->AddL(contactId);

    // Check that iLocalView sends an event
    TContactViewEvent event;
    test(res->iLocalViewEventQueue->ListenForEvent(10,event));
    test(event.iEventType == TContactViewEvent::EItemAdded);
    test(event.iContactId == contactId);
    test(event.iInt == 0 && event.iInt < res->iLocalView->CountL());
    test(res->iLocalView->AtL(event.iInt) == res->iTestContactId);
	
	if(aShowContactInGrp)
		{
	    test(res->iLocalViewEventQueue->ListenForEvent(10,event));
	    test(event.iEventType == TContactViewEvent::EGroupChanged);
	    test(event.iContactId == res->iGrpId);
		}

    const TContactItemId contactId1 = res->CreateTestContactL(_L("Vidin"));
    res->iContactIdArray->AddL(contactId1);

    //get the EItemAdd event
    test(res->iLocalViewEventQueue->ListenForEvent(10,event));
    test(event.iEventType == TContactViewEvent::EItemAdded);
    test(event.iContactId == contactId1);
    test(event.iInt == 1);
    test(res->iLocalView->AtL(event.iInt) == res->iTestContactId);

	if(aShowContactInGrp)
		{
	    test(res->iLocalViewEventQueue->ListenForEvent(10,event));
	    test(event.iEventType == TContactViewEvent::EGroupChanged);
	    test(event.iContactId == res->iGrpId);
		}
		
    return res;
    }


LOCAL_C void TestAddContactOldAPIL(TBool aShowContactInGrp)
    {
    test.Next(_L("Add a contact with phone number - old api"));

    CTestResources* res = DoTestAddContactAPILC(aShowContactInGrp);

    TContactViewEvent event;

	for(int i = 0; i < 2; i++)
		{
	    // There should come EItemAdded event from group view - check if iInt is the right one
	    test(res->iOldAPIGroupViewEventQueue->ListenForEvent(10,event));
	    test(event.iEventType == TContactViewEvent::EItemAdded);
	    test(event.iInt == 0);
	    
	    if(aShowContactInGrp)
	    	{
		    //Consume add to group event
		    test(res->iOldAPIGroupViewEventQueue->ListenForEvent(10,event));
	    	}
					
		// There should come EItemAdded event from find view - check if iInt is the right one
	    test(res->iOldAPIFindViewEventQueue->ListenForEvent(10,event));
	    test(event.iEventType == TContactViewEvent::EItemAdded);
	    test(event.iInt == 0);
	    
	    if(aShowContactInGrp)
	    	{
		    //Consume add to group event
		    test(res->iOldAPIFindViewEventQueue->ListenForEvent(10,event));
	    	}
	    	
		// There should come EItemAdded event from sub view - check if iInt is the right one
	    test(res->iOldAPISubViewEventQueue->ListenForEvent(10,event));
	    test(event.iEventType == TContactViewEvent::EItemAdded);
	    test(event.iInt == 0);

	    if(aShowContactInGrp)
	    	{
		    //Consume add to group event
		    test(res->iOldAPISubViewEventQueue->ListenForEvent(10,event));
	    	}
		}
    CleanupStack::PopAndDestroy(res);
    }


LOCAL_C void TestRemoveContactOldAPIL(TBool aShowContactInGrp)
    {
    test.Next(_L("Remove a contact with phone number - old api"));

    CTestResources* res = DoTestAddContactAPILC(aShowContactInGrp);
	
    TContactViewEvent viewEvent;
    
    int factor = aShowContactInGrp? 2 : 1;
    
    // first comsume the add events from group view, find view and sub view
    for(int i = 0; i < 2 * factor; i++)
    	{
	    test(res->iOldAPIGroupViewEventQueue->ListenForEvent(10,viewEvent));
	    test(res->iOldAPIFindViewEventQueue->ListenForEvent(10,viewEvent));
	    test(res->iOldAPISubViewEventQueue->ListenForEvent(10,viewEvent));
    	}
	
    // Delete the last added contact
    res->iDb->DeleteContactL(res->iTestContactId);

    // Wait for a DB event
    TContactDbObserverEvent dbEvent;
    if(aShowContactInGrp)
    	{
	    test(res->iDbEventQueue->ListenForEvent(10,dbEvent));
	    test(dbEvent.iType == EContactDbObserverEventGroupChanged);
    	}
    
    test(res->iDbEventQueue->ListenForEvent(10,dbEvent));
    test(dbEvent.iType == EContactDbObserverEventContactDeleted);
    test(dbEvent.iContactId == res->iTestContactId);

    // Check that iLocalView sends an event
    test(res->iLocalViewEventQueue->ListenForEvent(10,viewEvent));
    test(viewEvent.iEventType == TContactViewEvent::EItemRemoved);
    test(viewEvent.iContactId == res->iTestContactId);
    test(viewEvent.iInt == 1);
    test(res->iLocalView->FindL(viewEvent.iContactId) == KErrNotFound);

    // There should come events from group view - check if iInt is the right one
    test(res->iOldAPIGroupViewEventQueue->ListenForEvent(10,viewEvent));
    test(viewEvent.iEventType == TContactViewEvent::EItemRemoved);
    test(viewEvent.iContactId == res->iTestContactId);
    test(viewEvent.iInt == 1);
    test(res->iGroupViewOldAPI->FindL(viewEvent.iContactId) == KErrNotFound);
    
    // There should come events from find view - check if iInt is the right one
    test(res->iOldAPIFindViewEventQueue->ListenForEvent(10,viewEvent));
    test(viewEvent.iEventType == TContactViewEvent::EItemRemoved);
    test(viewEvent.iContactId == res->iTestContactId);
    test(viewEvent.iInt == 0);
    test(res->iGroupViewOldAPI->FindL(viewEvent.iContactId) == KErrNotFound);
    
    // There should come events from sub view - check if iInt is the right one
    test(res->iOldAPISubViewEventQueue->ListenForEvent(10,viewEvent));
    test(viewEvent.iEventType == TContactViewEvent::EItemRemoved);
    test(viewEvent.iContactId == res->iTestContactId);
    test(viewEvent.iInt == 0);
    test(res->iGroupViewOldAPI->FindL(viewEvent.iContactId) == KErrNotFound);
    
    CleanupStack::PopAndDestroy(res);
    }

LOCAL_C void TestAddContactNewAPIL(TBool aShowContactInGrp)
    {
    test.Next(_L("Add a contact with phone number - new api"));

    CTestResources* res = DoTestAddContactAPILC(aShowContactInGrp);

    TContactViewEvent event;
    
	for(int i = 0; i < 2; i++)
		{
	    // There should come EItemAdded event from group view - check if iInt is the right one
	    if(aShowContactInGrp)
	    	{
		    //Consume add to group event
		    test(res->iNewAPIGroupViewEventQueue->ListenForEvent(10,event));
		    test(event.iEventType == TContactViewEvent::ESortOrderChanged);
	    	}
	    else //TO_FIX
	    	{
		    test(res->iNewAPIGroupViewEventQueue->ListenForEvent(10,event));
		    test(event.iEventType == TContactViewEvent::EItemAdded);
		    test(event.iInt == i); 
	    	}	
	    
					
		// There should come EItemAdded event from find view - check if iInt is the right one
	    test(res->iNewAPIFindViewEventQueue->ListenForEvent(10,event));
	    test(event.iEventType == TContactViewEvent::EItemAdded);
	    test(event.iInt == i); 
	    
	    if(aShowContactInGrp)
	    	{
		    //Consume add to group event
		    test(res->iNewAPIFindViewEventQueue->ListenForEvent(10,event));
		    test(event.iEventType == TContactViewEvent::EGroupChanged);
	    	}
	    
	    	
		// There should come EItemAdded event from sub view - check if iInt is the right one
	    test(res->iNewAPISubViewEventQueue->ListenForEvent(10,event));
	    test(event.iEventType == TContactViewEvent::EItemAdded);
	    test(event.iInt == i); 
	    if(aShowContactInGrp)
	    	{
		    //Consume add to group event
		    test(res->iNewAPISubViewEventQueue->ListenForEvent(10,event));
		    test(event.iEventType == TContactViewEvent::EGroupChanged);
	    	}

		}
    CleanupStack::PopAndDestroy(res);
    }


LOCAL_C void TestRemoveContactNewAPIL(TBool aShowContactInGrp)
    {
    test.Next(_L("Remove a contact with phone number - new api"));

    CTestResources* res = DoTestAddContactAPILC(aShowContactInGrp);
    
    TContactViewEvent viewEvent;
    
    // first comsume the add events from group view, find view and sub view
	res->iNewAPIGroupViewEventQueue->Flush();
	res->iNewAPIFindViewEventQueue->Flush();
	res->iNewAPISubViewEventQueue->Flush();
	
    // Delete the last added contact
    res->iDb->DeleteContactL(res->iTestContactId);

    // Wait for a DB event
    TContactDbObserverEvent dbEvent;
    if(aShowContactInGrp)
    	{
	    test(res->iDbEventQueue->ListenForEvent(10,dbEvent));
	    test(dbEvent.iType == EContactDbObserverEventGroupChanged);
    	}
    	
    test(res->iDbEventQueue->ListenForEvent(10,dbEvent));
    test(dbEvent.iType == EContactDbObserverEventContactDeleted);
    test(dbEvent.iContactId == res->iTestContactId);

    // Check that iLocalView sends an event
    test(res->iLocalViewEventQueue->ListenForEvent(10,viewEvent));
    test(viewEvent.iEventType == TContactViewEvent::EItemRemoved);
    test(viewEvent.iContactId == res->iTestContactId);
    test(viewEvent.iInt == 1);
    test(res->iLocalView->FindL(viewEvent.iContactId) == KErrNotFound);

    // There should come events from group view - check if iInt is the right one
    test(res->iNewAPIGroupViewEventQueue->ListenForEvent(10,viewEvent));
    test(viewEvent.iEventType == TContactViewEvent::EItemRemoved);
    test(viewEvent.iContactId == res->iTestContactId);
    test(viewEvent.iInt == 1);
    test(res->iGroupViewOldAPI->FindL(viewEvent.iContactId) == KErrNotFound);
    
    // There should come events from find view - check if iInt is the right one
    test(res->iNewAPIFindViewEventQueue->ListenForEvent(10,viewEvent));
    test(viewEvent.iEventType == TContactViewEvent::EItemRemoved);
    test(viewEvent.iContactId == res->iTestContactId);
    test(viewEvent.iInt == 1);
    test(res->iGroupViewOldAPI->FindL(viewEvent.iContactId) == KErrNotFound);
    
    // There should come events from sub view - check if iInt is the right one
    test(res->iNewAPISubViewEventQueue->ListenForEvent(10,viewEvent));
    test(viewEvent.iEventType == TContactViewEvent::EItemRemoved);
    test(viewEvent.iContactId == res->iTestContactId);
    test(viewEvent.iInt == 1);
    test(res->iGroupViewOldAPI->FindL(viewEvent.iContactId) == KErrNotFound);

    CleanupStack::PopAndDestroy(res);
    }
    
/*
This function checks that for multiple deletes using the DeleteContactsL api, this generates
an EUnavailable event before sending EItemRemoved events to signify that the views are 
not synchronised yet. When the views are synchronised then a final EReady event is sent
and all the views should then be consistent. All the subviews also receive the EUnavailable
and EReady events.
*/    


LOCAL_C void TestRemoveContactsNewAPIL(TBool aShowContactInGrp)
    {
	test.Next(_L("Remove multiple contacts with phone number - new api"));

    CTestResources* res = DoTestAddContactAPILC(aShowContactInGrp);
   
    TContactViewEvent viewEvent;
    
    // first consume the add events from group view, find view and sub view
	res->iNewAPIGroupViewEventQueue->Flush();
	res->iNewAPIFindViewEventQueue->Flush();
	res->iNewAPISubViewEventQueue->Flush();
	
    // Delete multiple contacts, there are two in the array
    res->iDb->DeleteContactsL(*(res->iContactIdArray));
    
    
    //Check that the view has not been updated yet after the delete
    TInt groupViewCount = res->iGroupViewNewAPI->CountL();
    test(groupViewCount == 2);
    TInt contactViewBaseContactId = res->iGroupViewNewAPI->AtL(0);
    const CViewContact &vcontact = res->iGroupViewNewAPI->ContactAtL(0);
    

    // Wait for a DB event
    TContactDbObserverEvent dbEvent;
    
	test(res->iDbEventQueue->ListenForEvent(10,dbEvent));
	test(res->iDbEventQueue->ListenForEvent(10,dbEvent));
	test(dbEvent.iType == EContactDbObserverEventUnknownChanges); 	

    // Check that iLocalView sends an event
	test(res->iLocalViewEventQueue->ListenForEvent(10,viewEvent));
	test(viewEvent.iEventType == TContactViewEvent::EUnavailable);
	test(res->iLocalViewEventQueue->ListenForEvent(10,viewEvent));
	test(viewEvent.iEventType == TContactViewEvent::EReady);
    

    // There should be events from the group view - check if iInt is the right one
    test(res->iNewAPIGroupViewEventQueue->ListenForEvent(10,viewEvent));
    test(viewEvent.iEventType == TContactViewEvent::EUnavailable);
    test(res->iNewAPIGroupViewEventQueue->ListenForEvent(10,viewEvent));
    test(viewEvent.iEventType == TContactViewEvent::EItemRemoved);
    test(res->iNewAPIGroupViewEventQueue->ListenForEvent(10,viewEvent));
    test(viewEvent.iEventType == TContactViewEvent::EItemRemoved);
    test(viewEvent.iContactId == res->iTestContactId);
    test(viewEvent.iInt == 0);
   
	test(res->iNewAPIGroupViewEventQueue->ListenForEvent(10,viewEvent));
	test(viewEvent.iEventType == TContactViewEvent::EReady);
   
    
    // The group view should be consistent, there should be no contacts in this view
    groupViewCount = res->iGroupViewNewAPI->CountL();
    test(groupViewCount == 0);
    // should not be able to find anything 
    TRAPD(err, contactViewBaseContactId = res->iGroupViewNewAPI->AtL(0));
    test(err == KErrNotFound);
    
    
    
    // Check that the find view receives its events
    if(aShowContactInGrp)
    	{
	    test(res->iNewAPIFindViewEventQueue->ListenForEvent(10,viewEvent));
	    test(viewEvent.iEventType == TContactViewEvent::EUnavailable);
	    test(res->iNewAPIFindViewEventQueue->ListenForEvent(10,viewEvent));
	    test(viewEvent.iEventType == TContactViewEvent::EReady);
    	}
    	 
  
    // Check that the subview event queue receives events correctly
    if(aShowContactInGrp)
    	{
	    test(res->iNewAPISubViewEventQueue->ListenForEvent(10,viewEvent));
	    test(viewEvent.iEventType == TContactViewEvent::EUnavailable);
	    test(res->iNewAPISubViewEventQueue->ListenForEvent(10,viewEvent));
	    test(viewEvent.iEventType == TContactViewEvent::EReady);
    	}
    test.Next(_L("Remove multiple contacts with phone number - new api Successfull"));
	CleanupStack::PopAndDestroy(res);
	}

/*
This function checks that for multiple deletes using the DeleteContactL api, EItemRemoved 
events are sent in the right order and with the right info. A remote view is used
*/


LOCAL_C void TestRemoveContactRemoteViewNewAPIL(TBool aShowContactInGrp)
{
	test.Next(_L("Remote view - delete multiple contacts - new api"));

    CTestResources* res = CTestResources::NewLC(aShowContactInGrp, ETrue);
    TContactViewEvent viewEvent;
    
    CContactIdArray* idArray = CContactIdArray::NewLC();
    // Create 4 contacts
    for(TInt loop = 0; loop < 4; ++loop)
    	{
    	idArray->AddL(res->CreateTestContactL(_L("a")));
    	}
    
    //just consume events
    while(res->iRemoteViewEventQueue->ListenForEvent(1, viewEvent))
    	{
    	}
    
    // how many contacts do we have in remote view?
	TInt remoteViewCount = res->iRemoteView->CountL();
    test(remoteViewCount == 4);
    
    //delete all contacts now
    for(TInt loop = 0; loop < idArray->Count(); ++loop)
    	{
    	res->iDb->DeleteContactL((*idArray)[loop]);
    	}
    
    int currentEvent = 0;
    // check incoming events
    while(res->iRemoteViewEventQueue->ListenForEvent(1, viewEvent))
    	{
    	test(viewEvent.iEventType == TContactViewEvent::EItemRemoved);
    	test(viewEvent.iInt == 0);
    	test(viewEvent.iContactId == (*idArray)[currentEvent++]);
    	}
    
    while(idArray->Count() > 0) 
    	{
    	idArray->Remove(0);
    	}
    
    CleanupStack::PopAndDestroy(2, res);		
	}

LOCAL_C void SetTextFieldL(CContactItem& aItem, TUid aType, const TDesC& aValue)
	{
	CContactItemFieldSet& fieldSet = aItem.CardFields();
	const TInt pos = fieldSet.Find(aType);
	if(pos == KErrNotFound)
		{
		// if does not exist add this field
		CContactItemField* field = CContactItemField::NewLC(KStorageTypeText, aType);
		field->TextStorage()->SetTextL(aValue);
		aItem.AddFieldL(*field);
		CleanupStack::Pop(field);
		}
	else
		{
		fieldSet[pos].TextStorage()->SetTextL(aValue);
		}
	}


LOCAL_C void TestSimulateSyncSessionL()
    {
	test.Next(_L("Simulate a sync session - 2 contacts changed - new api"));

    CTestResources* res = CTestResources::NewLC(ETrue, ETrue);
    TContactViewEvent viewEvent;
    
    // Create 2 contacts
    TContactItemId firstContactId = res->CreateTestContactL(_L("Apan"));
    TContactItemId secondContactId = res->CreateTestContactL(_L("Baname"));
        
	//we should have 5 events here
	test(res->iNewAPIGroupViewEventQueue->ListenForEvent(10,viewEvent));
	test(viewEvent.iEventType == TContactViewEvent::EReady);
	test(res->iNewAPIGroupViewEventQueue->ListenForEvent(10,viewEvent));
	test(viewEvent.iEventType == TContactViewEvent::EItemAdded);
	test(res->iNewAPIGroupViewEventQueue->ListenForEvent(10,viewEvent));
	test(viewEvent.iEventType == TContactViewEvent::ESortOrderChanged);
	test(res->iNewAPIGroupViewEventQueue->ListenForEvent(10,viewEvent));
	test(viewEvent.iEventType == TContactViewEvent::EItemAdded);
	test(res->iNewAPIGroupViewEventQueue->ListenForEvent(10,viewEvent));
	test(viewEvent.iEventType == TContactViewEvent::ESortOrderChanged);

	TInt groupViewCount = res->iGroupViewNewAPI->CountL();
    test(groupViewCount == 2);
            
    // change existing contact items
    CContactItem* firstContact = res->iDb->OpenContactL(firstContactId);
    CleanupStack::PushL(firstContact);
    CContactItem* secondContact = res->iDb->OpenContactL(secondContactId);
    CleanupStack::PushL(secondContact);
    
    // change name
    SetTextFieldL(*firstContact, KUidContactFieldFamilyName, _L("Apan1"));
    SetTextFieldL(*secondContact, KUidContactFieldFamilyName, _L("1Baname"));

	res->iNewAPIGroupViewEventQueue->Flush();
	res->iNewAPIFindViewEventQueue->Flush();
	res->iNewAPISubViewEventQueue->Flush();
	
    res->iDb->CommitContactL(*firstContact);	
    res->iDb->CommitContactL(*secondContact);
   
    // check events and count in group view after these changes
    test(res->iNewAPIGroupViewEventQueue->ListenForEvent(10, viewEvent)); 
    test(viewEvent.iEventType == TContactViewEvent::EItemRemoved); //first we should have a removed
    test(res->iGroupViewNewAPI->CountL() == 1);
    test(res->iNewAPIGroupViewEventQueue->ListenForEvent(10, viewEvent)); 
    test(viewEvent.iEventType == TContactViewEvent::EItemAdded); //then we should have an add
    test(res->iGroupViewNewAPI->CountL() == 2);
    test(res->iNewAPIGroupViewEventQueue->ListenForEvent(10, viewEvent)); 
    test(viewEvent.iEventType == TContactViewEvent::EItemRemoved); //then we should have a removed
    test(res->iGroupViewNewAPI->CountL() == 1);
    test(res->iNewAPIGroupViewEventQueue->ListenForEvent(10, viewEvent)); 
    test(viewEvent.iEventType == TContactViewEvent::EItemAdded); //then we should have an add
    test(res->iGroupViewNewAPI->CountL() == 2);
    
    // check now the status of the group view - we should have 1Baname ; Apan1 in the group view
    test(res->iGroupViewNewAPI->ContactAtL(0).Id() == secondContactId);
    test(res->iGroupViewNewAPI->ContactAtL(1).Id() == firstContactId);
    CleanupStack::PopAndDestroy(2, firstContact);
    
    // change back
    firstContact = res->iDb->OpenContactL(firstContactId);
    CleanupStack::PushL(firstContact);
    secondContact = res->iDb->OpenContactL(secondContactId);
    CleanupStack::PushL(secondContact);
    // change back contacts
    SetTextFieldL(*firstContact, KUidContactFieldFamilyName, _L("Apan"));
    SetTextFieldL(*secondContact, KUidContactFieldFamilyName, _L("Baname"));
    res->iDb->CommitContactL(*firstContact);	
    res->iDb->CommitContactL(*secondContact);
    //just consume events
    while(res->iNewAPIGroupViewEventQueue->ListenForEvent(1, viewEvent))
    	{
    	}
    // check now the status of the group view - we should have Apan ; Baname in the group view
    test(res->iGroupViewNewAPI->ContactAtL(0).Id() == firstContactId);
    test(res->iGroupViewNewAPI->ContactAtL(1).Id() == secondContactId);    
    CleanupStack::PopAndDestroy(2, firstContact);
    
    // change again contacts but commit changes in different order
    firstContact = res->iDb->OpenContactL(firstContactId);
    CleanupStack::PushL(firstContact);
    secondContact = res->iDb->OpenContactL(secondContactId);
    CleanupStack::PushL(secondContact);
    // change back contacts
    SetTextFieldL(*firstContact, KUidContactFieldFamilyName, _L("Apan1"));
    SetTextFieldL(*secondContact, KUidContactFieldFamilyName, _L("1Baname"));
    res->iDb->CommitContactL(*firstContact);	
    res->iDb->CommitContactL(*secondContact);
    
    // check events and count in group view after these changes
    test(res->iNewAPIGroupViewEventQueue->ListenForEvent(10, viewEvent)); 
    test(viewEvent.iEventType == TContactViewEvent::EItemRemoved); //first we should have a removed
    test(res->iGroupViewNewAPI->CountL() == 1);
    test(res->iNewAPIGroupViewEventQueue->ListenForEvent(10, viewEvent)); 
    test(viewEvent.iEventType == TContactViewEvent::EItemAdded); //then we should have an add
    test(res->iGroupViewNewAPI->CountL() == 2);
    test(res->iNewAPIGroupViewEventQueue->ListenForEvent(10, viewEvent)); 
    test(viewEvent.iEventType == TContactViewEvent::EItemRemoved); //then we should have a removed
    test(res->iGroupViewNewAPI->CountL() == 1);
    test(res->iNewAPIGroupViewEventQueue->ListenForEvent(10, viewEvent)); 
    test(viewEvent.iEventType == TContactViewEvent::EItemAdded); //then we should have an add
    test(res->iGroupViewNewAPI->CountL() == 2);
    // check now the status of the group view - we should have 1Baname ; Apan1 in the group view
    test(res->iGroupViewNewAPI->ContactAtL(0).Id() == secondContactId);
    test(res->iGroupViewNewAPI->ContactAtL(1).Id() == firstContactId);
    
    CleanupStack::PopAndDestroy(3, res);
    }  


LOCAL_C void TestSimulateSyncSession1L()
    {
	test.Next(_L("Simulate a sync session - 3 contacts changed - new api - new api"));

    CTestResources* res = CTestResources::NewLC(ETrue, ETrue);
    TContactViewEvent viewEvent;
    
    // Create 3 contacts
    TContactItemId firstContactId = res->CreateTestContactL(_L("a"));
    TContactItemId secondContactId = res->CreateTestContactL(_L("b"));
    TContactItemId thirdContactId = res->CreateTestContactL(_L("c"));
    
    //just consume events
    while(res->iNewAPIGroupViewEventQueue->ListenForEvent(10, viewEvent))
    	{
    	}
    
	TInt groupViewCount = res->iGroupViewNewAPI->CountL();
    test(groupViewCount == 3);
            
    // change existing contact items
    CContactItem* firstContact = res->iDb->OpenContactL(firstContactId);
    CleanupStack::PushL(firstContact);
    CContactItem* secondContact = res->iDb->OpenContactL(secondContactId);
    CleanupStack::PushL(secondContact);
    CContactItem* thirdContact = res->iDb->OpenContactL(thirdContactId);
    CleanupStack::PushL(thirdContact);
    
    // change name
    SetTextFieldL(*firstContact, KUidContactFieldFamilyName, _L("3a"));
    SetTextFieldL(*secondContact, KUidContactFieldFamilyName, _L("2b"));
    SetTextFieldL(*thirdContact, KUidContactFieldFamilyName, _L("1c"));

	res->iNewAPIGroupViewEventQueue->Flush();
	res->iNewAPIFindViewEventQueue->Flush();
	res->iNewAPISubViewEventQueue->Flush();
	
    res->iDb->CommitContactL(*firstContact);	
    res->iDb->CommitContactL(*secondContact);
    res->iDb->CommitContactL(*thirdContact);

    //just consume events
    while(res->iNewAPIGroupViewEventQueue->ListenForEvent(1, viewEvent))
    	{
    	}

     // check now the status of the group view - we should have 1c ; 2b ; 3a in the group view
    test(res->iGroupViewNewAPI->ContactAtL(0).Id() == thirdContactId);
    test(res->iGroupViewNewAPI->ContactAtL(1).Id() == secondContactId);
    test(res->iGroupViewNewAPI->ContactAtL(2).Id() == firstContactId);
    CleanupStack::PopAndDestroy(3, firstContact);
    
    //change back
    firstContact = res->iDb->OpenContactL(firstContactId);
    CleanupStack::PushL(firstContact);
    secondContact = res->iDb->OpenContactL(secondContactId);
    CleanupStack::PushL(secondContact);
    thirdContact = res->iDb->OpenContactL(thirdContactId);
    CleanupStack::PushL(thirdContact);
    
    // change name
    SetTextFieldL(*firstContact, KUidContactFieldFamilyName, _L("a"));
    SetTextFieldL(*secondContact, KUidContactFieldFamilyName, _L("b"));
    SetTextFieldL(*thirdContact, KUidContactFieldFamilyName, _L("c"));

    res->iDb->CommitContactL(*firstContact);	
    res->iDb->CommitContactL(*secondContact);
    res->iDb->CommitContactL(*thirdContact);

    //just consume events
    while(res->iNewAPIGroupViewEventQueue->ListenForEvent(1, viewEvent))
    	{
    	}

     // check now the status of the group view - we should have a ; b ; c in the group view
    test(res->iGroupViewNewAPI->ContactAtL(0).Id() == firstContactId);
    test(res->iGroupViewNewAPI->ContactAtL(1).Id() == secondContactId);
    test(res->iGroupViewNewAPI->ContactAtL(2).Id() == thirdContactId);
    
    CleanupStack::PopAndDestroy(4, res);
    }

/**

@SYMTestCaseID     PIM-T-CONTACTVIEWEVENTS-0001

*/

void DoTestsL()
    {
	test.Start(_L("@SYMTESTCaseID:PIM-T-CONTACTVIEWEVENTS-0001 T_FilteredViewEvents"));

	
	//Test with contacts not in group
    TestAddContactOldAPIL(EFalse);
    TestRemoveContactOldAPIL(EFalse);
    TestAddContactNewAPIL(EFalse);
	TestRemoveContactNewAPIL(EFalse);
	TestRemoveContactRemoteViewNewAPIL(EFalse);	
	
	//Test with contacts in group
    TestAddContactOldAPIL(ETrue);
    TestRemoveContactOldAPIL(ETrue);
    TestAddContactNewAPIL(ETrue);
	TestRemoveContactNewAPIL(ETrue);
	TestRemoveContactRemoteViewNewAPIL(ETrue);	
	
	// remove multiple contacts
	TestRemoveContactsNewAPIL(ETrue);
	TestSimulateSyncSessionL();
	TestSimulateSyncSession1L();
	
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
