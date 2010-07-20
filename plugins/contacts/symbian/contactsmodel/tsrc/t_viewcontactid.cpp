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
#include <cntview.h>

#include <coreappstest/testserver.h>


#include "ccontactvieweventqueue.h"
#include "cfindtext.h"
#include "t_utils2.h"

////////////////////////////////////////////////////////////////////////
// Uncomment the following macro to run the large view creation test.
// This takes a long time to run. Also, it requires the database file
// SQLite__T_ViewContactId_10000_Contacts.cdb. This is *NOT* listed in
// the bld.inf file and so is not available unless manually moved into
// the emulator's z:\core-apps\app-engines\cntmodel\databases folder
// in the epoc32 tree.
// 
// Also, this test cannot easily be run on hardware as the db file is 
// too large to fit on the ROM. It is not included in the IBY file.
//
//
//#define __DO_CREATE_LARGE_VIEW_TEST__
//
////////////////////////////////////////////////////////////////////////

_LIT(KTestName, "T_ViewContactId");

_LIT(KTestDbName, "c:T_ViewContactId.cdb");

const TInt KOneHundredContacts  =   100;
#if (defined __SYMBIAN_CNTMODEL_USE_SQLITE__ && defined __DO_CREATE_LARGE_VIEW_TEST__ )
const TInt KTenThousandContacts = 10000;
#endif

LOCAL_D RTest test(KTestName);


class CTestResources : public CBase
    {
public:
	static CTestResources* NewLC(TInt aNumContactsToCreate);
	static CTestResources* NewLC(const TDesC& aDatabaseName);
	~CTestResources();
	void GetLocalContactIdsLC(const CArrayFixFlat<TInt>& aIndexes, TInt aExpectedCount);
	void GetRemoteContactIdsLC(const CArrayFixFlat<TInt>& aIndexes, TInt aExpectedCount);
	inline CContactLocalView* LocalView() { return iLocalView; }
	inline CContactRemoteView* RemoteView() { return iRemoteView; }
	void CreateViewL();
private:
	void ConstructL(TInt aNumContactsToCreate);
	void ConstructL(const TDesC& aDatabaseName);
	void CreateTestContactsL(TInt aNumContactsToCreate);
private:
	CContactDatabase* iDb;
	CContactViewEventQueue* iViewEventQueue;
	RContactViewSortOrder iViewSortOrder;
	CContactLocalView* iLocalView;
	CContactRemoteView* iRemoteView;
	};

CTestResources* CTestResources::NewLC(TInt aNumContactsToCreate)
    {
    CTestResources* self = new(ELeave) CTestResources;
    CleanupStack::PushL(self);
    self->ConstructL(aNumContactsToCreate);
    return self;
    }

CTestResources* CTestResources::NewLC(const TDesC& aDatabaseName)
    {
    CTestResources* self = new(ELeave) CTestResources;
    CleanupStack::PushL(self);
    self->ConstructL(aDatabaseName);
    return self;
    }

void CTestResources::ConstructL(TInt aNumContactsToCreate)
    {
    iDb = CContactDatabase::ReplaceL(KTestDbName);
	CreateTestContactsL(aNumContactsToCreate);
    }

void CTestResources::ConstructL(const TDesC& aDatabaseName)
    {
    iDb = CContactDatabase::OpenL(aDatabaseName);
    TInt count(iDb->CountL());
    }

void CTestResources::CreateViewL()
	{
	iViewEventQueue = CContactViewEventQueue::NewL(NULL,4000);

    iViewSortOrder.AppendL(KUidContactFieldFamilyName);
    iViewSortOrder.AppendL(KUidContactFieldGivenName);
    iViewSortOrder.AppendL(KUidContactFieldCompanyName);
    
    const TContactViewPreferences prefs = static_cast<TContactViewPreferences>(EContactsOnly);
    iLocalView = CContactLocalView::NewL(*iViewEventQueue, *iDb, iViewSortOrder, prefs);

#ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__
	const TInt KTimeOutInSeconds(10);
#else
	const TInt KTimeOutInSeconds(420);  // 7 minutes.
#endif

   // Wait for view to get ready
    TContactViewEvent event;
    __ASSERT_ALWAYS(iViewEventQueue->ListenForEvent(KTimeOutInSeconds, event), User::Invariant());
    __ASSERT_ALWAYS(event.iEventType == TContactViewEvent::EReady, User::Invariant());

	iViewEventQueue->Flush();
	iRemoteView = CContactRemoteView::NewL(*iViewEventQueue, *iDb, iViewSortOrder, prefs);
	__ASSERT_ALWAYS(iViewEventQueue->ListenForEvent(KTimeOutInSeconds, event), User::Invariant());
	__ASSERT_ALWAYS(event.iEventType == TContactViewEvent::EReady, User::Invariant());
    }

void CTestResources::CreateTestContactsL(TInt aNumContactsToCreate)
	{
	CRandomContactGenerator* generator = CRandomContactGenerator::NewL();
	CleanupStack::PushL(generator);
	generator->SetDbL(*iDb);
	for (TInt i = 0; i < aNumContactsToCreate; ++i)
		{
		generator->AddTypicalRandomContactL();
		}
	iDb->CompactL();
	CleanupStack::PopAndDestroy(generator);

	}

CTestResources::~CTestResources()
    {
	if (iRemoteView) iRemoteView->Close(*iViewEventQueue);
    if (iLocalView) iLocalView->Close(*iViewEventQueue);
    iViewSortOrder.Close();
    delete iViewEventQueue;
    delete iDb;
	TRAP_IGNORE(CContactDatabase::DeleteDatabaseL(KTestDbName));
    }

void CTestResources::GetLocalContactIdsLC(const CArrayFixFlat<TInt>& aIndexes, TInt aExpectedCount)
	{
	CContactIdArray* array = CContactIdArray::NewLC();
	//Pack the parameters to GetContactIdsL in the class TVirtualFunction1Params 
	//and pass it to CContactViewBase_Reserved_1.
	CContactViewBase::TVirtualFunction1Params structOfParams(&aIndexes,array);
	//Make a call to CContactViewBase_Reserved_1() instead of GetContactIdsL().
	iLocalView->CContactViewBase_Reserved_1(CContactViewBase::ECContactViewBaseVirtualFunction1, 
		&structOfParams);

	__ASSERT_ALWAYS(array->Count()==aExpectedCount, User::Invariant());
	CleanupStack::PopAndDestroy(array);
	}


void CTestResources::GetRemoteContactIdsLC(const CArrayFixFlat<TInt>& aIndexes, TInt aExpectedCount)
	{
	CContactIdArray* array = CContactIdArray::NewLC();
	//Pack the parameters to GetContactIdsL in the class TVirtualFunction1Params 
	//and pass it to CContactViewBase_Reserved_1.
	CContactViewBase::TVirtualFunction1Params structOfParams(&aIndexes,array);
	//Make a call to CContactViewBase_Reserved_1() instead of GetContactIdsL().
	iRemoteView->CContactViewBase_Reserved_1(CContactViewBase::ECContactViewBaseVirtualFunction1, 
		&structOfParams);

	__ASSERT_ALWAYS(array->Count()==aExpectedCount, User::Invariant());
	CleanupStack::PopAndDestroy(array);
	}


LOCAL_C void TestGetContactIdsL()
	{
	CTestResources* res = CTestResources::NewLC(KOneHundredContacts);
	res->CreateViewL();

	//Empty index array
	CArrayFixFlat<TInt>* emptyIndexes = new(ELeave) CArrayFixFlat<TInt>(8);	
	CleanupStack::PushL(emptyIndexes);
	res->GetLocalContactIdsLC(*emptyIndexes,0);
	res->GetRemoteContactIdsLC(*emptyIndexes,0);
	CleanupStack::PopAndDestroy(emptyIndexes);

	//Valid index array
	CArrayFixFlat<TInt>* indexes = new(ELeave) CArrayFixFlat<TInt>(8);	
	CleanupStack::PushL(indexes);
	indexes->AppendL(1);
	indexes->AppendL(3);
	indexes->AppendL(5);
	indexes->AppendL(7);
	res->GetLocalContactIdsLC(*indexes,4);
	res->GetRemoteContactIdsLC(*indexes,4);
	CleanupStack::PopAndDestroy(indexes);
	
	//Invalid index array
	CArrayFixFlat<TInt>* invalid = new(ELeave) CArrayFixFlat<TInt>(8);	
	CleanupStack::PushL(indexes);
	indexes->AppendL(1003);
	indexes->AppendL(666);
	res->GetLocalContactIdsLC(*invalid,0);
	res->GetRemoteContactIdsLC(*invalid,0);
	CleanupStack::PopAndDestroy(invalid);

	//Partially valid array
	CArrayFixFlat<TInt>* partial = new(ELeave) CArrayFixFlat<TInt>(8);	
	CleanupStack::PushL(partial);
	partial->AppendL(0);
	partial->AppendL(1000);
	partial->AppendL(1);
	partial->AppendL(666);
	res->GetLocalContactIdsLC(*partial,2);
	res->GetRemoteContactIdsLC(*partial,2);
	CleanupStack::PopAndDestroy(partial);

	//Whole View
	CArrayFixFlat<TInt>* allIndexes = new(ELeave) CArrayFixFlat<TInt>(8);	
	CleanupStack::PushL(allIndexes);
	const TInt count = res->LocalView()->CountL();
	for (TInt i=0; i<count; ++i)
		{
		allIndexes->AppendL(i);
		}
	res->GetLocalContactIdsLC(*allIndexes,count);
	res->GetRemoteContactIdsLC(*allIndexes,count);

	//Check data returned matches view
	CContactIdArray* array = CContactIdArray::NewLC();
	//Pack the parameters to GetContactIdsL in the class TVirtualFunction1Params 
	//and pass it to CContactViewBase_Reserved_1.
	CContactViewBase::TVirtualFunction1Params structOfParams(allIndexes,array);
	//Make a call to CContactViewBase_Reserved_1() instead of GetContactIdsL().
	res->LocalView()->CContactViewBase_Reserved_1(CContactViewBase::ECContactViewBaseVirtualFunction1, 
		&structOfParams);

	const TInt arrayCount = array->Count();
	__ASSERT_ALWAYS(arrayCount==count, User::Invariant());
	
	for (TInt j=0; j<arrayCount; ++j)
		{
		test(res->LocalView()->AtL(j)==(*array)[j]);
		}
	CleanupStack::PopAndDestroy(array);
	CleanupStack::PopAndDestroy(allIndexes);

	CleanupStack::PopAndDestroy(res);
	}


#if (defined __SYMBIAN_CNTMODEL_USE_SQLITE__ && defined __DO_CREATE_LARGE_VIEW_TEST__ )
LOCAL_C void TestBigDatabaseL()
	{
	_LIT(K1000ContactDbSourcePath, 
		"z:\\core-apps\\app-engines\\cntmodel\\databases\\SQLite__T_ViewContactId_10000_Contacts.cdb");
	_LIT(K1000ContactDbDestinationPath, 
		"c:\\private\\10003a73\\SQLite__T_ViewContactId_10000_Contacts.cdb");
	_LIT(K1000ContactDbUserFriendlyName, 
		"c:T_ViewContactId_10000_Contacts.cdb");

    RPIMTestServer serv;
	CleanupClosePushL(serv);	
    User::LeaveIfError(serv.Connect());
    serv.DeleteFileL(K1000ContactDbDestinationPath());
    serv.CopyFileL(K1000ContactDbSourcePath, K1000ContactDbDestinationPath());
    CleanupStack::PopAndDestroy(&serv);
	
	CTestResources* res = CTestResources::NewLC(K1000ContactDbUserFriendlyName());
	res->CreateViewL();
	const TInt KNumContactsInLocalView = res->LocalView()->CountL();
	const TInt KNumContactsInRemoteView = res->RemoteView()->CountL();
	test(KNumContactsInLocalView == KTenThousandContacts && 
		KNumContactsInRemoteView == KTenThousandContacts);
	test.Printf(_L("Number contacts in views -- local: %d; remote: %d.\n"), 
		KNumContactsInLocalView, KNumContactsInRemoteView);

	// Access the penultimate contact view item
	const TInt KPenultimateContactViewIndex(KNumContactsInRemoteView - 2);
	const CViewContact& viewContact = res->RemoteView()->ContactAtL(KPenultimateContactViewIndex);
	const TInt KFieldCount = viewContact.FieldCount();
	for (TInt i = 0; i < KFieldCount; ++i)
		{
		TPtrC fieldText = viewContact.Field(i);
		test.Printf(_L("Remote view contact item %d; field %d of %d: %S\n"), 
			KPenultimateContactViewIndex + 1, i + 1, KFieldCount, &fieldText);
		}
	CleanupStack::PopAndDestroy(res);
	}
#endif

/**

@SYMTestCaseID     PIM-T-VIEWCONTACTID-0001

*/

void DoTestsL()
    {
	_LIT(KTestTitle, "T_ViewContactId: test contact view item IDs");
	test.Start(_L("@SYMTESTCaseID:PIM-T-VIEWCONTACTID-0001 T_ViewContactId: test contact view item IDs"));

	TestGetContactIdsL();

#if (defined __SYMBIAN_CNTMODEL_USE_SQLITE__ && defined __DO_CREATE_LARGE_VIEW_TEST__ )
	// This will only work after the migration to SQLite and the re-architected views 
	// in v9.5 and then only if the __DO_CREATE_LARGE_VIEW_TEST__ macro is turned on.
	_LIT(KBigDbTestTitle, "Test creating a view from a big database (10,000 contacts)");
	test.Next(_L("Test creating a view from a big database (10,000 contacts)"));

	TestBigDatabaseL();
#endif

    test.End();
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
	test.Close();
	__UHEAP_MARKEND;
	return err;
    }
