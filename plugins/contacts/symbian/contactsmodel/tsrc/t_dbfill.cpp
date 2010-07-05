/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <e32panic.h>
#include <cntfldst.h>
#include <cntdb.h>
#include <cntitem.h>
#include <cntdef.h>

#include <cntviewbase.h>
#include <cntview.h>


static RTest test(_L("T_DBFill")) ;


_LIT(KTestName,"T_DBFill");
_LIT(KDBFilled,"Database Filled");
_LIT(KViewUpdated,"View Updated");
_LIT(KTestFailed,"RTEST: FAILED : T_DBFill test completed Failed!");


class CTestConductor : public CActive, public MContactViewObserver
	{

public:
	CTestConductor();
	void ConstructL();
	void RunTestsL();
	static CTestConductor* NewL() ;
	~CTestConductor() ;
	void HandleContactViewEvent(const CContactViewBase &aView, 
						const TContactViewEvent &aEvent) ;	
protected:
	void RunL();
	void DoCancel();
	void RunError();

private:
	CContactDatabase* iContactsDatabase;				
	TContactItemId iGroupID ;
	CContactNamedRemoteView *irview ;
	CContactGroupView *igview ;
	CContactIdArray* imemberArray ;
	RTimer iTimer;
	TBool iFailed;
	};


void CTestConductor::HandleContactViewEvent(const CContactViewBase &aView, const TContactViewEvent &aEvent)
	{
	if ((aEvent.iEventType == TContactViewEvent::EItemRemoved)
		&& (aEvent.iContactId == iGroupID) && (&aView == igview))
		{
		// Check whether items from imemberArray are in the view
		// checking one item is good enough for this test
		TContactItemId id = (*imemberArray)[0]; 			
		TRAPD(err, igview->FindL(id))				
		if (err == KErrNone)				
			{
				// the view has been updated				
				test.Printf(KViewUpdated);
				CActiveScheduler::Stop(); 	
			}
		}
	
	}

CTestConductor* CTestConductor::NewL()
	{
	CTestConductor* self=new(ELeave) CTestConductor();
	CleanupStack::PushL(self);
	self->ConstructL();
	self->RunTestsL();
	CleanupStack::Pop();
	return self;
	}

CTestConductor::~CTestConductor()
	{

	Cancel();
	iTimer.Close();

	delete iContactsDatabase;


	}

CTestConductor::CTestConductor() : CActive(0), iFailed(EFalse)
	{

	CActiveScheduler::Add(this);
	iTimer.CreateLocal();

	}

void CTestConductor::ConstructL()
	{
	
	iContactsDatabase=CContactDatabase::ReplaceL();

	}

void CTestConductor::RunL()
	{
	
	iFailed = ETrue;
	CActiveScheduler::Stop() ; 	//timeout expired - stop executing 

	}

void CTestConductor::RunError()
	{
	
	}

void CTestConductor::DoCancel()
	{
	iTimer.Cancel();
	}
	
void CTestConductor::RunTestsL()
	{
	
	//DBFill
	_LIT(KText, "Group1") ;
	TBufC<30> g1(KText) ;
	CContactGroup *group1 = static_cast<CContactGroup*> (iContactsDatabase->CreateContactGroupL(g1)) ;
	CleanupStack::PushL(group1);		

	TContactItemId id1 = group1->Id();	

	CContactCard* contact1 = CContactCard::NewL();
	CleanupStack::PushL(contact1);		
	
	TContactItemId cid1 = iContactsDatabase->AddNewContactL(*contact1);	
	iContactsDatabase->AddContactToGroupL(cid1, id1) ;
	
	CleanupStack::PopAndDestroy(contact1);
	CleanupStack::PopAndDestroy(group1);
	
	iContactsDatabase->CloseContactL(id1);
	iContactsDatabase->CloseTables();
	
	test.Printf(KDBFilled);//Database is filled

	delete iContactsDatabase;
	iContactsDatabase = 0;
	
	//ViewUpdate
	iContactsDatabase=CContactDatabase::OpenL();

	RContactViewSortOrder sortOrder ;	
	sortOrder.AppendL(KUidContactFieldGivenName);
	
	_LIT(KText1, "Remote View") ;
	TBufC<30> name(KText1) ;
	
	irview = CContactNamedRemoteView::NewL(*this, name, *iContactsDatabase, 
			sortOrder, EContactAndGroups) ;
	
	CleanupStack::PushL(irview) ;
		 
	igview = CContactGroupView::NewL(*iContactsDatabase, 
		*irview, *this, id1, CContactGroupView::EShowContactsNotInAnyGroup) ;
	
	CleanupStack::PushL(igview) ;

	
	CContactIdArray *arrayOfContacts = iContactsDatabase->FindLC(_L("Group1"), NULL) ;

	TInt count = arrayOfContacts->Count();	
	
	TContactItemId id = 0;
	TInt i ;
	for(i = 0; i < count; i++)
		{
		id = (*arrayOfContacts)[i] ;
		CContactItem *item = iContactsDatabase->ReadContactL(id) ;
		CleanupStack::PushL(item) ;
		
		if (item->Type() == KUidContactGroup)
			{			
			CleanupStack::PopAndDestroy(item) ;
			iGroupID = id ;
			break ;  // This group should be deleted			
			}
		CleanupStack::PopAndDestroy(item) ;
		}
	
	CleanupStack::PopAndDestroy(arrayOfContacts) ;
	
	if (i == count)
		{
		sortOrder.Close() ;
		User::Leave(KErrNotFound) ;
		}
		
	
	CContactGroup* group = (static_cast<CContactGroup*> (iContactsDatabase->ReadContactL(iGroupID)));
	CleanupStack::PushL(group) ;
	
	imemberArray = (const_cast<CContactIdArray*> (group->ItemsContained()));
		
	iContactsDatabase->DeleteContactL(id) ;
	
	iTimer.After(iStatus,3000000);//timeout after 3 seconds
	SetActive();

	CActiveScheduler::Start() ;	
	
	CleanupStack::PopAndDestroy(group) ;
	
	CleanupStack::PopAndDestroy(igview) ;
	CleanupStack::PopAndDestroy(irview) ;	
	
	sortOrder.Close() ;

	if (!iFailed)
		{

		test.End();

		}
	else
		{
		
		test.Printf(KTestFailed);
		
		}
	
	}


LOCAL_C void doMainL()
	{
	__UHEAP_MARK;
	CTestConductor* testConductor = NULL;
	testConductor = CTestConductor::NewL();
		
	CleanupStack::PushL( testConductor );
	CleanupStack::PopAndDestroy( testConductor );
	__UHEAP_MARKEND;
	}

/**

@SYMTestCaseID     PIM-T-DBFILL-0001

*/

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	test.Start(_L("@SYMTESTCaseID:PIM-T-DBFILL-0001 "));

	test.Title();
	CActiveScheduler* scheduler=new CActiveScheduler;
	if (scheduler)
		{
		CActiveScheduler::Install(scheduler);
		CTrapCleanup* cleanup=CTrapCleanup::New();
		if (cleanup)
			{
			TRAPD(err,doMainL());
			test(err == KErrNone);
			delete cleanup;
			}
		delete scheduler;
		}

	test.Close();
	__UHEAP_MARKEND;
	return KErrNone;
	}

