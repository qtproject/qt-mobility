/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

_LIT(KTestName, "CContactRemoteView::ContactAtL Tests");

_LIT(KTestDbName, "c:contactat.cdb");

LOCAL_D RTest test(KTestName);


class CTestResources : public CBase , public MContactViewObserver
    {
public:
	static CTestResources* NewLC();
	void ConstructL();
	void CreateTestContactsL();
	~CTestResources();
	void HandleContactViewEvent(const CContactViewBase& aView,const TContactViewEvent& aEvent);
	CContactDatabase* iDb;
	TContactItemId iAddedContact;
	RContactViewSortOrder iViewSortOrder;
	CContactRemoteView* iRemoteView;
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

    iViewSortOrder.AppendL(KUidContactFieldFamilyName);
    iViewSortOrder.AppendL(KUidContactFieldGivenName);
    iViewSortOrder.AppendL(KUidContactFieldCompanyName);

    iRemoteView = CContactRemoteView::NewL(*this, *iDb, iViewSortOrder, EContactsOnly);
    CreateTestContactsL();
    }

/** 
 * Create a test contact with a Euro character name field.
 * This bug can be reproduced with most of the cyrillic characters also.
 */
void CTestResources::CreateTestContactsL()
    {
    const TInt KContacts = 1;
    TInt i;
    for (i=1; i <= KContacts; ++i)
        {
        CContactCard* card = CContactCard::NewLC();
        CContactItemField* field = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldFamilyName);
        _LIT(KName, "\x20a0"); //Euro character
        field->TextStorage()->SetTextL(KName);
        card->AddFieldL(*field);
        CleanupStack::Pop(field);
        iAddedContact = iDb->AddNewContactL(*card);
        CleanupStack::PopAndDestroy(card);
        }
    }

CTestResources::~CTestResources()
    {
    if (iRemoteView)
        {
        iRemoteView->Close(*this);
        }
    iViewSortOrder.Close();
    delete iDb;
    TRAP_IGNORE(CContactDatabase::DeleteDatabaseL(KTestDbName));
    }

void CTestResources::HandleContactViewEvent(const CContactViewBase& aView,const TContactViewEvent& aEvent)
    {
    if (&aView == iRemoteView && aEvent.iEventType == TContactViewEvent::EItemAdded)
        {
        TInt index(0);
		TRAPD(error, index = iRemoteView->FindL(iAddedContact) );
		test(error == KErrNone);
        TRAP(error, iRemoteView->ContactAtL(index));
		test(error == KErrNone);
     
		__ASSERT_ALWAYS(error==KErrNone,User::Invariant());
		CActiveScheduler::Stop();
        }
    }

/**

@SYMTestCaseID     PIM-T-CONTACTAT-0001

*/

void DoTestsL()
    {
	test.Start(_L("@SYMTESTCaseID:PIM-T-CONTACTAT-0001 Test CContactRemoteView::ContactAtL"));

    test.Next(_L("Add a contact with euro sign to db"));

	CTestResources* res = CTestResources::NewLC();
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(res);
    test.End();
	test.Close();
    }

GLDEF_C TInt E32Main()
	{
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

