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
* PreferencesBug Test module
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

_LIT(KTestName, "T_PreferencesBug");

_LIT(KTestDbName, "c:T_PreferencesBug.cdb");

const TInt KEventWaitTime = 2;

LOCAL_D RTest test(KTestName);

enum TPanicCode
    {
    EPanicNoReadyEventFromView = 1
    };

static void Panic(TInt aReason)
    {
    User::Panic(KTestName, aReason);
    }

class CTestResources : public CBase
    {
    public: // Interface
        static CTestResources* NewLC();
        void ConstructL();
        void CreateTestViewsL
            (TContactViewPreferences aAddViewPreferences=static_cast<TContactViewPreferences>(0));
        ~CTestResources();

        enum TTestViews
            {
            ETestViewContactsOnly, 
            ETestViewGroupsOnly, 
            ETestViewContactsAndGroups
            };

        CContactDatabase& Db() const { return *iDb; }
        CContactDbEventQueue& DbEvents() const { return *iDbEvents; }

        CContactViewBase& TestView(TTestViews aViewIndex) const
            {
            return *iTestViews[aViewIndex].iView;
            }

        CContactViewEventQueue& TestViewEvents(TTestViews aViewIndex) const
            {
            return *iTestViews[aViewIndex].iViewEvents;
            }

    private: // Implementation
        struct TTestViewInit
            {
            enum { KMaxSortOrderLen = 5 };
            TContactViewPreferences iViewPrefs;
            TInt iSortOrder[KMaxSortOrderLen];
            };
        
        class TTestView
            {
            public:
                TTestView();
                void InitLC
                    (CContactDatabase& aDb, 
                    TContactViewPreferences aAddViewPreferences,
                    const TTestViewInit& aInit);
                void Destroy();
                RContactViewSortOrder iSortOrder;
                CContactViewEventQueue* iViewEvents;
                CContactLocalView* iView;
            private:
                static void Cleanup(TAny* aPtr);
            };
            
    private:  // Data
        static const TTestViewInit iTestViewInit[];
        CContactDatabase* iDb;
        CContactDbEventQueue* iDbEvents;
        RArray<TTestView> iTestViews;
    };

// Constant data for creating the test views
const CTestResources::TTestViewInit CTestResources::iTestViewInit[] =
    {
        { EContactsOnly, 
        { KUidContactFieldFamilyNameValue, KUidContactFieldGivenNameValue, KUidContactFieldCompanyNameValue, KNullUidValue } },
        { EGroupsOnly, 
        { KUidContactFieldTemplateLabelValue, KNullUidValue } },
        { EContactAndGroups, 
        { KUidContactFieldFamilyNameValue, KUidContactFieldGivenNameValue, KUidContactFieldCompanyNameValue, KUidContactFieldTemplateLabelValue, KNullUidValue } },
        { static_cast<TContactViewPreferences>(0), { KNullUidValue } }
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
    iDbEvents = CContactDbEventQueue::NewL(iDb);
    }

void CTestResources::CreateTestViewsL(TContactViewPreferences aAddViewPreferences)
    {
    for (const TTestViewInit* initData = iTestViewInit; 
        initData->iSortOrder[0]!=KNullUidValue;
        ++initData)
        {
        TTestView testView;
        testView.InitLC(*iDb,aAddViewPreferences,*initData);
        User::LeaveIfError(iTestViews.Append(testView));
        CleanupStack::Pop();  // testView
        }

    // Wait for the views to get ready
    for (TInt i=0; i<iTestViews.Count(); ++i)
        {
        TContactViewEvent event;
        __ASSERT_ALWAYS(iTestViews[i].iViewEvents->ListenForEvent(10,event),
            Panic(EPanicNoReadyEventFromView));
        __ASSERT_ALWAYS(event.iEventType==TContactViewEvent::EReady,
            Panic(EPanicNoReadyEventFromView));
        }
    }

CTestResources::~CTestResources()
    {
    for (TInt i=iTestViews.Count()-1; i>=0; --i)
        {
        iTestViews[i].Destroy();
        }
    iTestViews.Close();
    delete iDbEvents;
    delete iDb;
    TRAP_IGNORE(CContactDatabase::DeleteDatabaseL(KTestDbName));
    }

CTestResources::TTestView::TTestView()
    : iViewEvents(NULL), iView(NULL)
    {
    }

void CTestResources::TTestView::InitLC
        (CContactDatabase& aDb, 
        TContactViewPreferences aAddViewPreferences,
        const TTestViewInit& aInit)
    {
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    for (const TInt* fieldType=aInit.iSortOrder; *fieldType!=KNullUidValue; ++fieldType)
        {
        iSortOrder.AppendL(TUid::Uid(*fieldType));
        }
    iViewEvents = CContactViewEventQueue::NewL();
    iView = CContactLocalView::NewL
        (*iViewEvents, 
        aDb, 
        iSortOrder, 
        static_cast<TContactViewPreferences>(aInit.iViewPrefs|aAddViewPreferences));
    }

void CTestResources::TTestView::Destroy()
    {
    if (iView) iView->Close(*iViewEvents);
    delete iViewEvents;
    iSortOrder.Close();
    }

void CTestResources::TTestView::Cleanup(TAny* aPtr)
    {
    static_cast<TTestView*>(aPtr)->Destroy();
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

LOCAL_C TContactItemId CreateTestGroupL
        (CContactDatabase& aDb,
        const TDesC& aLabel) 
    {
    CContactItem* group = aDb.CreateContactGroupLC(aLabel);
    const TContactItemId id = group->Id();
    CleanupStack::PopAndDestroy(group);
    return id;
    }

/**
 * Returns ETrue if aView contains all the contacts in aContacts.
 */
LOCAL_C TBool ContainsAllContactsL
        (const CContactViewBase& aView, const CContactIdArray& aContacts)
    {
    for (TInt i=0; i < aContacts.Count(); ++i)
        {
        if (aView.FindL(aContacts[i])==KErrNotFound)
            {
            return EFalse;
            }
        }
    return ETrue;
    }

/**
 * Returns ETrue if aView doesn't contain _any_ contact of aContacts.
 */
LOCAL_C TBool DoesNotContainsContactsL
        (const CContactViewBase& aView, const CContactIdArray& aContacts)
    {
    for (TInt i=0; i < aContacts.Count(); ++i)
        {
        if (aView.FindL(aContacts[i])!=KErrNotFound)
            {
            return EFalse;
            }
        }
    return ETrue;
    }



// TEST CODE

LOCAL_C void TestOpenViewsWithExistingDataL
        (TContactViewPreferences aAddPrefs)
    {
    CTestResources* res = CTestResources::NewLC();

    // Create test contacts and a group
    CContactIdArray* groups = CContactIdArray::NewLC();
    TContactItemId groupId = CreateTestGroupL(res->Db(), _L("IBM"));
    groups->AddL(groupId);

    CContactIdArray* unsortedGroups = CContactIdArray::NewLC();
    groupId = CreateTestGroupL(res->Db(), _L(""));
    unsortedGroups->AddL(groupId);

    CContactIdArray* contacts = CContactIdArray::NewLC();
    TContactItemId id = CreateTestContactL(res->Db(), _L("Foo"), _L("John"), _L("IBM"));
    contacts->AddL(id);
    res->Db().AddContactToGroupL(id,(*groups)[0]);
    id = CreateTestContactL(res->Db(), _L("Smith"), _L("Peter"), _L("IBM"));
    contacts->AddL(id);
    res->Db().AddContactToGroupL(id,(*groups)[0]);
    id = CreateTestContactL(res->Db(), _L("Paul"), _L("Linus"), _L("Bell Labs"));
    contacts->AddL(id);

    CContactIdArray* unsortedContacts = CContactIdArray::NewLC();
    id = CreateTestContactL(res->Db(), _L(""), _L(""), _L(""));
    unsortedContacts->AddL(id);

    id = CreateTestContactL(res->Db(), _L(" "), _L(" "), _L(" "));
    if (aAddPrefs & ESingleWhiteSpaceIsEmptyField)
        {
        unsortedContacts->AddL(id);
        }
    else
        {
        contacts->AddL(id);
        }

    // Eat all DB events
    TContactDbObserverEvent dbEvent;
    while (res->DbEvents().ListenForEvent(KEventWaitTime,dbEvent))
        {
        }

    // Create the test contact views
    res->CreateTestViewsL(aAddPrefs);

    // Check the test view states
    test.Next(_L("Check EContactsOnly view contains all the contacts"));
    test(ContainsAllContactsL(res->TestView(CTestResources::ETestViewContactsOnly), *contacts));
    if (res->TestView(CTestResources::ETestViewContactsOnly).ContactViewPreferences() & EIgnoreUnSorted)
        {
        test(DoesNotContainsContactsL(res->TestView(CTestResources::ETestViewContactsOnly), *unsortedContacts));
        }
    else
        {
        test(ContainsAllContactsL(res->TestView(CTestResources::ETestViewContactsOnly), *unsortedContacts));
        }

    test.Next(_L("Check EGroupsOnly view contains all the groups"));
    test(ContainsAllContactsL(res->TestView(CTestResources::ETestViewGroupsOnly), *groups));
    if (res->TestView(CTestResources::ETestViewGroupsOnly).ContactViewPreferences() & EIgnoreUnSorted)
        {
        test(DoesNotContainsContactsL(res->TestView(CTestResources::ETestViewGroupsOnly), *unsortedGroups));
        }
    else
        {
        test(ContainsAllContactsL(res->TestView(CTestResources::ETestViewGroupsOnly), *unsortedGroups));
        }

    test.Next(_L("Check EContactAndGroups view contains all contacts and groups"));
    test(ContainsAllContactsL(res->TestView(CTestResources::ETestViewContactsAndGroups), *groups));
    test(ContainsAllContactsL(res->TestView(CTestResources::ETestViewContactsAndGroups), *contacts));
    if (res->TestView(CTestResources::ETestViewContactsAndGroups).ContactViewPreferences() & EIgnoreUnSorted)
        {
        test(DoesNotContainsContactsL(res->TestView(CTestResources::ETestViewContactsAndGroups), *unsortedContacts));
        test(DoesNotContainsContactsL(res->TestView(CTestResources::ETestViewContactsAndGroups), *unsortedGroups));
        }
    else
        {
        test(ContainsAllContactsL(res->TestView(CTestResources::ETestViewContactsAndGroups), *unsortedContacts));
        test(ContainsAllContactsL(res->TestView(CTestResources::ETestViewContactsAndGroups), *unsortedGroups));
        }

    test.Next(_L("Check EContactsOnly view does not contain any groups"));
    test(DoesNotContainsContactsL(res->TestView(CTestResources::ETestViewContactsOnly), *groups));
    test(DoesNotContainsContactsL(res->TestView(CTestResources::ETestViewContactsOnly), *unsortedGroups));

    test.Next(_L("Check EGroupsOnly view does not contain any contacts"));
    test(DoesNotContainsContactsL(res->TestView(CTestResources::ETestViewGroupsOnly), *contacts));
    test(DoesNotContainsContactsL(res->TestView(CTestResources::ETestViewGroupsOnly), *unsortedContacts));

    test.Next(_L("Check EContactsOnly view count"));
    if (res->TestView(CTestResources::ETestViewContactsOnly).ContactViewPreferences() & EIgnoreUnSorted)
        {
        test(res->TestView(CTestResources::ETestViewContactsOnly).CountL() == contacts->Count());
        }
    else
        {
        test(res->TestView(CTestResources::ETestViewContactsOnly).CountL() == contacts->Count()+unsortedContacts->Count());
        }

    test.Next(_L("Check EGroupsOnly view count"));
    if (res->TestView(CTestResources::ETestViewGroupsOnly).ContactViewPreferences() & EIgnoreUnSorted)
        {
        test(res->TestView(CTestResources::ETestViewGroupsOnly).CountL() == groups->Count());
        }
    else
        {
        test(res->TestView(CTestResources::ETestViewGroupsOnly).CountL() == groups->Count()+unsortedGroups->Count());
        }

    test.Next(_L("Check EContactAndGroups view count"));
    if (res->TestView(CTestResources::ETestViewContactsAndGroups).ContactViewPreferences() & EIgnoreUnSorted)
        {
        const TInt count = contacts->Count()+groups->Count();
        test(res->TestView(CTestResources::ETestViewContactsAndGroups).CountL() == count);
        }
    else
        {
        const TInt count = contacts->Count()+unsortedContacts->Count()+groups->Count()+unsortedGroups->Count();
        test(res->TestView(CTestResources::ETestViewContactsAndGroups).CountL() == count);
        }
    
    CleanupStack::PopAndDestroy(5,res);
    }

LOCAL_C void TestOpenViewsWithExistingDataL()
    {
    test.Start(_L("Open views with exisiting data (no special preferences)"));
    TestOpenViewsWithExistingDataL(static_cast<TContactViewPreferences>(0));

    // Calypso Phonebook uses these view preferences
    test.Next(_L("Open views with exisiting data (EUnSortedAtEnd | ESingleWhiteSpaceIsEmptyField)"));
    TestOpenViewsWithExistingDataL
        (static_cast<TContactViewPreferences>(EUnSortedAtEnd|ESingleWhiteSpaceIsEmptyField));

    test.Next(_L("Open views with exisiting data (EIgnoreUnSorted)"));
    TestOpenViewsWithExistingDataL(EIgnoreUnSorted);

    test.Next(_L("Open views with exisiting data (EIgnoreUnSorted|ESingleWhiteSpaceIsEmptyField)"));
    TestOpenViewsWithExistingDataL
        (static_cast<TContactViewPreferences>(EIgnoreUnSorted|ESingleWhiteSpaceIsEmptyField));

    test.Next(_L("Open views with exisiting data (EUnSortedAtBeginning)"));
    TestOpenViewsWithExistingDataL(EUnSortedAtBeginning);

    test.Next(_L("Open views with exisiting data (EUnSortedAtBeginning|ESingleWhiteSpaceIsEmptyField)"));
    TestOpenViewsWithExistingDataL
        (static_cast<TContactViewPreferences>(EUnSortedAtBeginning|ESingleWhiteSpaceIsEmptyField));

    test.Next(_L("Open views with exisiting data (EUnSortedAtEnd)"));
    TestOpenViewsWithExistingDataL(EUnSortedAtEnd);

    test.Next(_L("Open views with exisiting data (EUnSortedAtEnd|ESingleWhiteSpaceIsEmptyField)"));
    TestOpenViewsWithExistingDataL
        (static_cast<TContactViewPreferences>(EUnSortedAtEnd|ESingleWhiteSpaceIsEmptyField));

    test.Next(_L("Open views with exisiting data (ESingleWhiteSpaceIsEmptyField)"));
    TestOpenViewsWithExistingDataL(ESingleWhiteSpaceIsEmptyField);

    test.End();
    }

LOCAL_C void TestModifyContactL(CTestResources& aRes, TContactItemId aContactId)
    {
    CContactItem* contact = aRes.Db().OpenContactLX(aContactId);
    CleanupStack::PushL(contact);
    const TInt fieldIndex = contact->CardFields().Find(KUidContactFieldFamilyName);
    contact->CardFields()[fieldIndex].TextStorage()->SetTextL(_L("Modified"));
    aRes.Db().CommitContactL(*contact);
    CleanupStack::PopAndDestroy(2);

    TContactDbObserverEvent dbEvent;
    test(aRes.DbEvents().ListenForEvent(KEventWaitTime,dbEvent));
    test(dbEvent.iType==EContactDbObserverEventContactChanged && dbEvent.iContactId==aContactId);

    TContactViewEvent event;
    test(aRes.TestViewEvents(CTestResources::ETestViewContactsOnly).ListenForEvent(KEventWaitTime,event));
    test(event.iEventType==TContactViewEvent::EItemRemoved && event.iContactId==aContactId);
    test(aRes.TestViewEvents(CTestResources::ETestViewContactsOnly).ListenForEvent(KEventWaitTime,event));
    test(event.iEventType==TContactViewEvent::EItemAdded && event.iContactId==aContactId);
    test(aRes.TestView(CTestResources::ETestViewContactsOnly).FindL(aContactId)!=KErrNotFound);
    test(!aRes.TestViewEvents(CTestResources::ETestViewContactsOnly).ListenForEvent(KEventWaitTime,event));

    test(!aRes.TestViewEvents(CTestResources::ETestViewGroupsOnly).ListenForEvent(KEventWaitTime,event));
    test(aRes.TestView(CTestResources::ETestViewGroupsOnly).FindL(aContactId)==KErrNotFound);

    test(aRes.TestViewEvents(CTestResources::ETestViewContactsAndGroups).ListenForEvent(KEventWaitTime,event));
    test(event.iEventType==TContactViewEvent::EItemRemoved && event.iContactId==aContactId);
    test(aRes.TestViewEvents(CTestResources::ETestViewContactsAndGroups).ListenForEvent(KEventWaitTime,event));
    test(event.iEventType==TContactViewEvent::EItemAdded && event.iContactId==aContactId);
    test(aRes.TestView(CTestResources::ETestViewContactsAndGroups).FindL(aContactId)!=KErrNotFound);
    test(!aRes.TestViewEvents(CTestResources::ETestViewContactsAndGroups).ListenForEvent(KEventWaitTime,event));
    }

LOCAL_C void TestModifyUnsortedContactL(CTestResources& aRes, TContactItemId aContactId)
    {
    CContactItem* contact = aRes.Db().OpenContactLX(aContactId);
    CleanupStack::PushL(contact);
    AddFieldToContactL(*contact, KUidContactFieldEMail, _L("foo@bar.com"));
    aRes.Db().CommitContactL(*contact);
    CleanupStack::PopAndDestroy(2);

    TContactDbObserverEvent dbEvent;
    test(aRes.DbEvents().ListenForEvent(KEventWaitTime,dbEvent));
    test(dbEvent.iType==EContactDbObserverEventContactChanged && dbEvent.iContactId==aContactId);

    TContactViewEvent event;
    if (aRes.TestView(CTestResources::ETestViewContactsOnly).ContactViewPreferences() & EIgnoreUnSorted)
        {
        test(!aRes.TestViewEvents(CTestResources::ETestViewContactsOnly).ListenForEvent(KEventWaitTime,event));
        }
    else
        {
        test(aRes.TestViewEvents(CTestResources::ETestViewContactsOnly).ListenForEvent(KEventWaitTime,event));
        test(event.iEventType==TContactViewEvent::EItemRemoved && event.iContactId==aContactId);
        test(aRes.TestViewEvents(CTestResources::ETestViewContactsOnly).ListenForEvent(KEventWaitTime,event));
        test(event.iEventType==TContactViewEvent::EItemAdded && event.iContactId==aContactId);
        test(aRes.TestView(CTestResources::ETestViewContactsOnly).FindL(aContactId)!=KErrNotFound);
        test(!aRes.TestViewEvents(CTestResources::ETestViewContactsOnly).ListenForEvent(KEventWaitTime,event));
        }

    test(!aRes.TestViewEvents(CTestResources::ETestViewGroupsOnly).ListenForEvent(KEventWaitTime,event));
    test(aRes.TestView(CTestResources::ETestViewGroupsOnly).FindL(aContactId)==KErrNotFound);

    if (aRes.TestView(CTestResources::ETestViewContactsAndGroups).ContactViewPreferences() & EIgnoreUnSorted)
        {
        test(!aRes.TestViewEvents(CTestResources::ETestViewContactsAndGroups).ListenForEvent(KEventWaitTime,event));
        }
    else
        {
        test(aRes.TestViewEvents(CTestResources::ETestViewContactsAndGroups).ListenForEvent(KEventWaitTime,event));
        test(event.iEventType==TContactViewEvent::EItemRemoved && event.iContactId==aContactId);
        test(aRes.TestViewEvents(CTestResources::ETestViewContactsAndGroups).ListenForEvent(KEventWaitTime,event));
        test(event.iEventType==TContactViewEvent::EItemAdded && event.iContactId==aContactId);
        test(aRes.TestView(CTestResources::ETestViewContactsAndGroups).FindL(aContactId)!=KErrNotFound);
        test(!aRes.TestViewEvents(CTestResources::ETestViewContactsAndGroups).ListenForEvent(KEventWaitTime,event));
        }
    }

LOCAL_C void TestDeleteGroupL(CTestResources& aRes, TContactItemId aGroupId)
    {
    aRes.Db().DeleteContactL(aGroupId);
    TContactDbObserverEvent dbEvent;
    test(aRes.DbEvents().ListenForEvent(KEventWaitTime,dbEvent));
    test(dbEvent.iType==EContactDbObserverEventGroupDeleted && dbEvent.iContactId==aGroupId);

    TContactViewEvent event;
    test(!aRes.TestViewEvents(CTestResources::ETestViewContactsOnly).ListenForEvent(KEventWaitTime,event));
    test(aRes.TestView(CTestResources::ETestViewContactsOnly).FindL(aGroupId)==KErrNotFound);

    test(aRes.TestViewEvents(CTestResources::ETestViewGroupsOnly).ListenForEvent(KEventWaitTime,event));
    test(event.iEventType==TContactViewEvent::EItemRemoved && event.iContactId==aGroupId);
    test(aRes.TestView(CTestResources::ETestViewGroupsOnly).FindL(aGroupId)==KErrNotFound);
    test(!aRes.TestViewEvents(CTestResources::ETestViewGroupsOnly).ListenForEvent(KEventWaitTime,event));

    test(aRes.TestViewEvents(CTestResources::ETestViewContactsAndGroups).ListenForEvent(KEventWaitTime,event));
    test(event.iEventType==TContactViewEvent::EItemRemoved && event.iContactId==aGroupId);
    test(aRes.TestView(CTestResources::ETestViewContactsAndGroups).FindL(aGroupId)==KErrNotFound);
    test(!aRes.TestViewEvents(CTestResources::ETestViewContactsAndGroups).ListenForEvent(KEventWaitTime,event));
    }

LOCAL_C void TestDeleteContactL(CTestResources& aRes, TContactItemId aContactId)
    {
    aRes.Db().DeleteContactL(aContactId);
    TContactDbObserverEvent dbEvent;
    test(aRes.DbEvents().ListenForEvent(KEventWaitTime,dbEvent));
    test(dbEvent.iType==EContactDbObserverEventContactDeleted && dbEvent.iContactId==aContactId);

    TContactViewEvent event;
    test(aRes.TestViewEvents(CTestResources::ETestViewContactsOnly).ListenForEvent(KEventWaitTime,event));
    test(event.iEventType==TContactViewEvent::EItemRemoved && event.iContactId==aContactId);
    test(aRes.TestView(CTestResources::ETestViewContactsOnly).FindL(aContactId)==KErrNotFound);
    test(!aRes.TestViewEvents(CTestResources::ETestViewContactsOnly).ListenForEvent(KEventWaitTime,event));

    test(!aRes.TestViewEvents(CTestResources::ETestViewGroupsOnly).ListenForEvent(KEventWaitTime,event));
    test(aRes.TestView(CTestResources::ETestViewGroupsOnly).FindL(aContactId)==KErrNotFound);

    test(aRes.TestViewEvents(CTestResources::ETestViewContactsAndGroups).ListenForEvent(KEventWaitTime,event));
    test(event.iEventType==TContactViewEvent::EItemRemoved && event.iContactId==aContactId);
    test(aRes.TestView(CTestResources::ETestViewContactsAndGroups).FindL(aContactId)==KErrNotFound);
    test(!aRes.TestViewEvents(CTestResources::ETestViewContactsAndGroups).ListenForEvent(KEventWaitTime,event));
    }

LOCAL_C void TestDeleteUnsortedContactL(CTestResources& aRes, TContactItemId aContactId)
    {
    aRes.Db().DeleteContactL(aContactId);
    TContactDbObserverEvent dbEvent;
    test(aRes.DbEvents().ListenForEvent(KEventWaitTime,dbEvent));
    test(dbEvent.iType==EContactDbObserverEventContactDeleted && dbEvent.iContactId==aContactId);

    TContactViewEvent event;
    if (aRes.TestView(CTestResources::ETestViewContactsOnly).ContactViewPreferences() & EIgnoreUnSorted)
        {
        test(!aRes.TestViewEvents(CTestResources::ETestViewContactsOnly).ListenForEvent(KEventWaitTime,event));
        test(aRes.TestView(CTestResources::ETestViewContactsOnly).FindL(aContactId)==KErrNotFound);
        }
    else
        {
        test(aRes.TestViewEvents(CTestResources::ETestViewContactsOnly).ListenForEvent(KEventWaitTime,event));
        test(event.iEventType==TContactViewEvent::EItemRemoved && event.iContactId==aContactId);
        test(aRes.TestView(CTestResources::ETestViewContactsOnly).FindL(aContactId)==KErrNotFound);
        test(!aRes.TestViewEvents(CTestResources::ETestViewContactsOnly).ListenForEvent(KEventWaitTime,event));
        }

    test(!aRes.TestViewEvents(CTestResources::ETestViewGroupsOnly).ListenForEvent(KEventWaitTime,event));
    test(aRes.TestView(CTestResources::ETestViewGroupsOnly).FindL(aContactId)==KErrNotFound);

    if (aRes.TestView(CTestResources::ETestViewContactsAndGroups).ContactViewPreferences() & EIgnoreUnSorted)
        {
        test(!aRes.TestViewEvents(CTestResources::ETestViewContactsAndGroups).ListenForEvent(KEventWaitTime,event));
        test(aRes.TestView(CTestResources::ETestViewContactsAndGroups).FindL(aContactId)==KErrNotFound);
        }
    else
        {
        test(aRes.TestViewEvents(CTestResources::ETestViewContactsAndGroups).ListenForEvent(KEventWaitTime,event));
        test(event.iEventType==TContactViewEvent::EItemRemoved && event.iContactId==aContactId);
        test(aRes.TestView(CTestResources::ETestViewContactsAndGroups).FindL(aContactId)==KErrNotFound);
        test(!aRes.TestViewEvents(CTestResources::ETestViewContactsAndGroups).ListenForEvent(KEventWaitTime,event));
        }
    }

LOCAL_C void TestViewEventsL
        (TContactViewPreferences aAddPrefs)
    {
    CTestResources* res = CTestResources::NewLC();
    // Create the test contact views
    res->CreateTestViewsL(aAddPrefs);

    // Add a contact
    test.Next(_L("Add a contact"));
    CContactIdArray* contacts = CContactIdArray::NewLC();
    TContactItemId id = CreateTestContactL(res->Db(), _L("Foo"), _L("John"), _L("IBM"));
    contacts->AddL(id);

    TContactDbObserverEvent dbEvent;
    test(res->DbEvents().ListenForEvent(KEventWaitTime,dbEvent));
    test(dbEvent.iType==EContactDbObserverEventContactAdded && dbEvent.iContactId==id);

    TContactViewEvent event;
    test(res->TestViewEvents(CTestResources::ETestViewContactsOnly).ListenForEvent(KEventWaitTime,event));
    test(event.iEventType==TContactViewEvent::EItemAdded && event.iContactId==id);
    test(res->TestView(CTestResources::ETestViewContactsOnly).FindL(id)!=KErrNotFound);
    test(!res->TestViewEvents(CTestResources::ETestViewContactsOnly).ListenForEvent(KEventWaitTime,event));

    test(!res->TestViewEvents(CTestResources::ETestViewGroupsOnly).ListenForEvent(KEventWaitTime,event));

    test(res->TestViewEvents(CTestResources::ETestViewContactsAndGroups).ListenForEvent(KEventWaitTime,event));
    test(event.iEventType==TContactViewEvent::EItemAdded && event.iContactId==id);
    test(res->TestView(CTestResources::ETestViewContactsAndGroups).FindL(id)!=KErrNotFound);
    test(!res->TestViewEvents(CTestResources::ETestViewContactsAndGroups).ListenForEvent(KEventWaitTime,event));

    // Add a group
    test.Next(_L("Add a group"));
    CContactIdArray* groups = CContactIdArray::NewLC();
    id = CreateTestGroupL(res->Db(), _L("IBM"));
    groups->AddL(id);

    test(res->DbEvents().ListenForEvent(KEventWaitTime,dbEvent));
    test(dbEvent.iType==EContactDbObserverEventGroupAdded && dbEvent.iContactId==id);

    test(!res->TestViewEvents(CTestResources::ETestViewContactsOnly).ListenForEvent(KEventWaitTime,event));

    test(res->TestViewEvents(CTestResources::ETestViewGroupsOnly).ListenForEvent(KEventWaitTime,event));
    test(event.iEventType==TContactViewEvent::EItemAdded && event.iContactId==id);
    test(res->TestView(CTestResources::ETestViewGroupsOnly).FindL(id)!=KErrNotFound);
    test(!res->TestViewEvents(CTestResources::ETestViewGroupsOnly).ListenForEvent(KEventWaitTime,event));

    test(res->TestViewEvents(CTestResources::ETestViewContactsAndGroups).ListenForEvent(KEventWaitTime,event));
    test(event.iEventType==TContactViewEvent::EItemAdded && event.iContactId==id);
    test(res->TestView(CTestResources::ETestViewContactsAndGroups).FindL(id)!=KErrNotFound);
    test(!res->TestViewEvents(CTestResources::ETestViewContactsAndGroups).ListenForEvent(KEventWaitTime,event));

    // Add an unsorted contact
    CContactIdArray* unsortedContacts = CContactIdArray::NewLC();
    test.Next(_L("Add an unsorted contact"));
    id = CreateTestContactL(res->Db(), _L(""), _L(""), _L(""));
    unsortedContacts->AddL(id);

    test(res->DbEvents().ListenForEvent(KEventWaitTime,dbEvent));
    test(dbEvent.iType==EContactDbObserverEventContactAdded && dbEvent.iContactId==id);

    if (aAddPrefs & EIgnoreUnSorted)
        {
        TContactViewEvent event;
        test(!res->TestViewEvents(CTestResources::ETestViewContactsOnly).ListenForEvent(KEventWaitTime,event));
        test(!res->TestViewEvents(CTestResources::ETestViewGroupsOnly).ListenForEvent(KEventWaitTime,event));
        test(!res->TestViewEvents(CTestResources::ETestViewContactsAndGroups).ListenForEvent(KEventWaitTime,event));
        }
    else
        {
        TContactViewEvent event;
        test(res->TestViewEvents(CTestResources::ETestViewContactsOnly).ListenForEvent(KEventWaitTime,event));
        test(event.iEventType==TContactViewEvent::EItemAdded && event.iContactId==id);
        test(res->TestView(CTestResources::ETestViewContactsOnly).FindL(id)!=KErrNotFound);
        test(!res->TestViewEvents(CTestResources::ETestViewContactsOnly).ListenForEvent(KEventWaitTime,event));

        test(!res->TestViewEvents(CTestResources::ETestViewGroupsOnly).ListenForEvent(KEventWaitTime,event));

        test(res->TestViewEvents(CTestResources::ETestViewContactsAndGroups).ListenForEvent(KEventWaitTime,event));
        test(event.iEventType==TContactViewEvent::EItemAdded && event.iContactId==id);
        test(res->TestView(CTestResources::ETestViewContactsAndGroups).FindL(id)!=KErrNotFound);
        test(!res->TestViewEvents(CTestResources::ETestViewContactsAndGroups).ListenForEvent(KEventWaitTime,event));
        }

    // Add an unsorted contact (fields contain spaces)
    test.Next(_L("Add an unsorted contact (fields contain spaces)"));
    id = CreateTestContactL(res->Db(), _L(" "), _L(" "), _L(" "));
    if (aAddPrefs & ESingleWhiteSpaceIsEmptyField)
        {
        unsortedContacts->AddL(id);
        }

    test(res->DbEvents().ListenForEvent(KEventWaitTime,dbEvent));
    test(dbEvent.iType==EContactDbObserverEventContactAdded && dbEvent.iContactId==id);

    if ((aAddPrefs & ESingleWhiteSpaceIsEmptyField) && (aAddPrefs & EIgnoreUnSorted))
        {
        TContactViewEvent event;
        test(!res->TestViewEvents(CTestResources::ETestViewContactsOnly).ListenForEvent(KEventWaitTime,event));
        test(!res->TestViewEvents(CTestResources::ETestViewGroupsOnly).ListenForEvent(KEventWaitTime,event));
        test(!res->TestViewEvents(CTestResources::ETestViewContactsAndGroups).ListenForEvent(KEventWaitTime,event));
        }
    else
        {
        TContactViewEvent event;
        test(res->TestViewEvents(CTestResources::ETestViewContactsOnly).ListenForEvent(KEventWaitTime,event));
        test(event.iEventType==TContactViewEvent::EItemAdded && event.iContactId==id);
        test(res->TestView(CTestResources::ETestViewContactsOnly).FindL(id)!=KErrNotFound);
        test(!res->TestViewEvents(CTestResources::ETestViewContactsOnly).ListenForEvent(KEventWaitTime,event));

        test(!res->TestViewEvents(CTestResources::ETestViewGroupsOnly).ListenForEvent(KEventWaitTime,event));

        test(res->TestViewEvents(CTestResources::ETestViewContactsAndGroups).ListenForEvent(KEventWaitTime,event));
        test(event.iEventType==TContactViewEvent::EItemAdded && event.iContactId==id);
        test(res->TestView(CTestResources::ETestViewContactsAndGroups).FindL(id)!=KErrNotFound);
        test(!res->TestViewEvents(CTestResources::ETestViewContactsAndGroups).ListenForEvent(KEventWaitTime,event));
        }

    // Modify contacts
    TInt i;
    test.Next(_L("Modify contacts"));
    for (i=0; i<contacts->Count(); ++i)
        {
        TestModifyContactL(*res,(*contacts)[i]);
        }    

    // Modify unsorted contacts
    test.Next(_L("Modify unsorted contacts"));
    for (i=0; i<unsortedContacts->Count(); ++i)
        {
        TestModifyUnsortedContactL(*res,(*unsortedContacts)[i]);
        }

    // Modify a group
    test.Next(_L("Modify a group"));
    res->Db().AddContactToGroupL((*contacts)[0], (*groups)[0]);
    test(res->DbEvents().ListenForEvent(KEventWaitTime,dbEvent));
    test(dbEvent.iType==EContactDbObserverEventGroupChanged && dbEvent.iContactId==(*groups)[0]);

    // Here the design is really weird: why an EContactsOnly view sends an event when 
    // a _group_ changes???
    test(res->TestViewEvents(CTestResources::ETestViewContactsOnly).ListenForEvent(KEventWaitTime,event));
    test(event.iEventType==TContactViewEvent::EGroupChanged && event.iContactId==(*groups)[0]);
    test(!res->TestViewEvents(CTestResources::ETestViewContactsOnly).ListenForEvent(KEventWaitTime,event));

    test(res->TestViewEvents(CTestResources::ETestViewGroupsOnly).ListenForEvent(KEventWaitTime,event));
    test(event.iEventType==TContactViewEvent::EGroupChanged && event.iContactId==(*groups)[0]);
    test(res->TestViewEvents(CTestResources::ETestViewGroupsOnly).ListenForEvent(KEventWaitTime,event));
    test(event.iEventType==TContactViewEvent::EItemRemoved && event.iContactId==(*groups)[0]);
    test(res->TestViewEvents(CTestResources::ETestViewGroupsOnly).ListenForEvent(KEventWaitTime,event));
    test(event.iEventType==TContactViewEvent::EItemAdded && event.iContactId==(*groups)[0]);
    test(!res->TestViewEvents(CTestResources::ETestViewGroupsOnly).ListenForEvent(KEventWaitTime,event));

    // Same weirdness here
    test(res->TestViewEvents(CTestResources::ETestViewContactsAndGroups).ListenForEvent(KEventWaitTime,event));
        // This seems to be the event ordering
    test(event.iEventType==TContactViewEvent::EGroupChanged && event.iContactId==(*groups)[0]);
    test(res->TestViewEvents(CTestResources::ETestViewContactsAndGroups).ListenForEvent(KEventWaitTime,event));
    test(event.iEventType==TContactViewEvent::EItemRemoved && event.iContactId==(*groups)[0]);
    test(res->TestViewEvents(CTestResources::ETestViewContactsAndGroups).ListenForEvent(KEventWaitTime,event));
    test(event.iEventType==TContactViewEvent::EItemAdded && event.iContactId==(*groups)[0]);
    test(!res->TestViewEvents(CTestResources::ETestViewContactsAndGroups).ListenForEvent(KEventWaitTime,event));

    // Delete groups
    test.Next(_L("Delete groups"));
    for (i=0; i<groups->Count(); ++i)
        {
        TestDeleteGroupL(*res, (*groups)[i]);
        }

    // Delete contacts
    test.Next(_L("Delete contacts"));
    for (i=0; i<contacts->Count(); ++i)
        {
        TestDeleteContactL(*res,(*contacts)[i]);
        }    

    // Delete unsorted contacts
    test.Next(_L("Delete unsorted contacts"));
    for (i=0; i<unsortedContacts->Count(); ++i)
        {
        TestDeleteUnsortedContactL(*res,(*unsortedContacts)[i]);
        }    

    CleanupStack::PopAndDestroy(4,res);
    }


LOCAL_C void TestViewEventsL()
    {
    test.Start(_L("Test view events (no special preferences)"));
    TestViewEventsL(static_cast<TContactViewPreferences>(0));

    // Calypso Phonebook uses these view preferences
    test.Next(_L("Test view events (EUnSortedAtEnd | ESingleWhiteSpaceIsEmptyField)"));
    TestViewEventsL
        (static_cast<TContactViewPreferences>(EUnSortedAtEnd|ESingleWhiteSpaceIsEmptyField));

    test.Next(_L("Test view events (EIgnoreUnSorted)"));
    TestViewEventsL(static_cast<TContactViewPreferences>(EIgnoreUnSorted));

    test.Next(_L("Test view events (EIgnoreUnSorted|ESingleWhiteSpaceIsEmptyField)"));
    TestViewEventsL(static_cast<TContactViewPreferences>(EIgnoreUnSorted|ESingleWhiteSpaceIsEmptyField));

    test.Next(_L("Test view events (EUnSortedAtBeginning)"));
    TestViewEventsL(EUnSortedAtBeginning);

    test.Next(_L("Test view events (EUnSortedAtBeginning|ESingleWhiteSpaceIsEmptyField)"));
    TestViewEventsL
        (static_cast<TContactViewPreferences>(EUnSortedAtBeginning|ESingleWhiteSpaceIsEmptyField));

    test.Next(_L("Test view events  (EUnSortedAtEnd)"));
    TestViewEventsL(EUnSortedAtEnd);

    test.Next(_L("Test view events  (EUnSortedAtEnd|ESingleWhiteSpaceIsEmptyField)"));
    TestViewEventsL
        (static_cast<TContactViewPreferences>(EUnSortedAtEnd|ESingleWhiteSpaceIsEmptyField));

    test.Next(_L("Test view events (ESingleWhiteSpaceIsEmptyField)"));
    TestViewEventsL(ESingleWhiteSpaceIsEmptyField);

    test.End();
    }
/**

@SYMTestCaseID     PIM-T-PREFERENCESBUG-0001

*/
LOCAL_C void DoTestsL()
    {
    CleanupClosePushL(test);
	test.Start(_L("@SYMTestCaseID:PIM-T-PREFERENCESBUG-0001 T_PreferencesBug"));

    TestOpenViewsWithExistingDataL();
    
    test.Next(KNullDesC);
    TestViewEventsL();

    test.End();
    CleanupStack::PopAndDestroy();  // test.Close()
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
