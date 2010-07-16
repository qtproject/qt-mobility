/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

// INCLUDE FILES
#include <e32std.h>
#include <e32test.h>
#include <cntdb.h>
#include <cntdef.h>
#include <cntitem.h>
#include <cntfield.h>

#include <cntdef.hrh>
#include <cntview.h>
#include <cntfldst.h>

#include "cntmodeltesterasynch.h" 
#include "cntmodeltestersettonetocontacts.h"

_LIT(KTestName,"asynaccess");
_LIT(KLogFileName,"asynaccess.log");
_LIT(KThreadOpenAsync,"OpenAsync");
_LIT(KThreadOpenSync,"OpenSync");

LOCAL_D RTest test(KTestName);

CCntModelTesterAsynch* gAsync;
//
// CTestConductor.
//

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
	delete iLog;
	delete iDb;
	delete iRandomGenerator;
    TRAP_IGNORE(CContactDatabase::DeleteDatabaseL(KDbFileName));
	iFs.Close();
	}

CTestConductor::CTestConductor() 
	{
	}

void CTestConductor::ConstructL()
	{
	User::LeaveIfError(iFs.Connect());
	iLog=CLog::NewL(test,KLogFileName);
	iDb=CContactDatabase::ReplaceL(KDbFileName);
	iRandomGenerator=CRandomContactGenerator::NewL();
	iRandomGenerator->SetDbL(*iDb);
	}


void CTestConductor::RunTestsL()
	{
	iAsync = CCntModelTesterAsynch:: NewL(*iLog);
	gAsync = iAsync;
	CleanupStack::PushL(iAsync);
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(); // tester.
	}

void CTestConductor::SetTestError(TInt aTestError)
	{
	// error from tester
	iTestError = aTestError;
	}
	

//
//Class CCntModelTesterAsynch
//	
const TInt KContactCount = 500;

// ================= MEMBER FUNCTIONS =======================

CCntModelTesterAsynch::CCntModelTesterAsynch(CLog& aLog):
            CActive(CActive::EPriorityIdle),iLog(aLog)
    {    
    CActiveScheduler::Add(this);
    }

CCntModelTesterAsynch* CCntModelTesterAsynch::NewL(CLog& aLog)
    {
    CCntModelTesterAsynch* self = new(ELeave) CCntModelTesterAsynch(aLog);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }


void CCntModelTesterAsynch::ConstructL()
    {
    iDb= CContactDatabase::OpenL(KDbFileName);
    
    //Create a group
    iGroup = iDb->CreateContactGroupLC(_L("My Group"));
          
    iArray = CContactIdArray::NewL();
    CreateContactsL(); 
    CleanupStack::Pop(iGroup);   
    iOperation = CCntModelTesterSetToneToContacts::NewL(*iArray, _L("c:\\testdata\\Ring.wav"));
    
    IssueRequest();
    }

CCntModelTesterAsynch::~CCntModelTesterAsynch()
    { 
    delete iGroup;       
    delete iDb;
    Cancel();
    delete iArray;
    delete iOperation;
    }

void CCntModelTesterAsynch::DoCancel()
    {    
    }

void CCntModelTesterAsynch::RunL()
    {  
    if (!iOperation->IsProcessDone())
        {
        iLog.LogLine(_L("Operation continue, count=%d\n"),iOperation->iSetCount);
        TRAPD(err, iOperation->StepL());
		if (err != KErrNone )
			{
			iLog.LogLine(_L("Operation failed, error=%d\n"),err);
			if (err != KErrLocked )
				{
				User::Leave(err);
				}
			}
        IssueRequest();
        
        if (iOperation->iSetCount == KContactCount/2)
			{
			// Launch Thread to Open Db Asynchronously
			CDbOpenLauncher::Create(KThreadOpenAsync, KThreadOpenAsync);		
			}
			
        }
    else
        {
        iLog.LogLine(_L("Operation ready. Deleting contacts..\n"));
        // delete contacts:
        //TRAP_IGNORE(iDb->DeleteContactsL(*iArray));  
		for(TInt i=0;i<iArray->Count();i++)
			{
	       	TRAPD(err, iDb->DeleteContactL((*iArray)[i]));
			if (err != KErrNone )
				{
				iLog.LogLine(_L("Operation failed, error=%d\n"),err);
				if (err != KErrLocked )
					{
					User::Leave(err);
					}
				}				         
        	} 
        CActiveScheduler::Stop();  
        }

    }

void CCntModelTesterAsynch::Error(TInt aError)
    {    
    iLog.LogLine(_L("elieli: CCntModelTesterAsynch::RunError(%d)"), aError);
    return;
    }
    
void CCntModelTesterAsynch::IssueRequest()
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    SetActive();
    } 

void CCntModelTesterAsynch::CreateContactsL()
    {
    iLog.LogLine(_L("Creating contacts synchronously.\n"));
    
    CContactDatabase* db = CContactDatabase::OpenL(KDbFileName);
    CleanupStack::PushL(db);
    
    TContactItemId groupId = iGroup->Id();
    TBuf16<30> name;    
    for (TInt i=0; i<KContactCount; i++)
        {
        name.Zero();
        name.Append(_L("test "));
        name.AppendNum(i+1);        
        iArray->AddL(CreateContactL(db, name, _L("12456789")));
        iLog.LogLine(_L("contact ID of index %d is %d.\n"), i, (*iArray)[i]);
        //Add the contact item to the group
        db->AddContactToGroupL((*iArray)[i],groupId);                
        }    
    
    CleanupStack::PopAndDestroy(db);
    test.Printf(_L("Contacts ready. Setting Ringingtones started.\n"));
    iLog.LogLine(_L("Contacts ready. Setting Ringingtones started.\n"));
    }
    
TContactItemId CCntModelTesterAsynch::CreateContactL(CContactDatabase* aDb, 
                                                    const TDesC& aName, 
                                                    const TDesC& aNumber)
    {
    CContactCard* card = CContactCard::NewLC();       
  
    card->AddFieldL(*CreateFieldLC(KUidContactFieldVCardMapUnusedN,
                                   KUidContactFieldFamilyName, 
                                   aName));
    CleanupStack::Pop();
    card->AddFieldL(*CreateFieldLC(KUidContactFieldVCardMapTEL,
                                   KUidContactFieldPhoneNumber, 
                                   aNumber));
    CleanupStack::Pop();    
    
    TContactItemId id = aDb->AddNewContactL(*card);
    CleanupStack::PopAndDestroy(card);    
    return id;
    }
    
CContactItemField* CCntModelTesterAsynch::CreateFieldLC(const TUid aMappingUId,
                                                        TFieldType aFieldType, 
                                                        const TDesC &aText)
    {
    CContactItemField* field = CContactItemField::NewLC(KStorageTypeText, aFieldType);    
    field->SetMapping(aMappingUId);
    field->TextStorage()->SetTextL(aText);
    return field;
    }
    
//
// Separate Thread to Open Database
//
CDbOpenLauncher::CViewer* CDbOpenLauncher::CViewer::NewL(const TDesC& aViewName)
	{
	CViewer* self = new(ELeave) CViewer;
	CleanupStack::PushL(self);
	self->ConstructL(aViewName);
	CleanupStack::Pop(self);
	return(self);
	}

CDbOpenLauncher::CViewer::~CViewer()
	{
	delete iDatabase;
	}

void CDbOpenLauncher::CViewer::ConstructL(const TDesC& aViewName)
	{
	if(aViewName.Compare(KThreadOpenAsync) == 0)
		{	
		// Create Async Open 
		tester=CTester::NewL();
		}
	else if (aViewName.Compare(KThreadOpenSync) == 0)
		{
		//Open the database Synchronously
		//Here we do nothing
		;
		}
		
	}

void CDbOpenLauncher::DoMainL(const TDesC& aName)
	{
	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(scheduler);	
	CViewer* view = CViewer::NewL(aName);  
	CleanupStack::PushL(view);

	CActiveScheduler::Start();
		
	CleanupStack::PopAndDestroy(view);
	}
		
// This is the only thing one needs to call in order to set the view in motion. 
void CDbOpenLauncher::Create(const TDesC& aName, const TDesC& aViewName)
	{
	RThread thread;
	thread.Create( aName, CDbOpenLauncher::LaunchThread, KDefaultStackSize, 0x2000, 0x20000, (void*) &aViewName, EOwnerThread );
	thread.Resume();
	thread.SetPriority(EPriorityNormal);
	thread.Close();
	}


TInt CDbOpenLauncher::LaunchThread(TAny* aAny) 
	{
	__UHEAP_MARK;
	
	CTrapCleanup* cleanUpStack=CTrapCleanup::New();
	if (!cleanUpStack)
		{
		return KErrNoMemory;
		}			
					
	TRAPD(r,DoMainL(*((TDesC*) aAny)));

	delete CActiveScheduler::Current();
	delete cleanUpStack;

	__UHEAP_MARKEND;
	return r;
	}
	
	
//
// CTester Active Object to test Async Open in thread whilst async open is going on in other thread
//	
CTester* CTester::NewL()
	{
	CTester* self = new(ELeave) CTester();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}
	
void CTester::DoCancel()	
	{
	}

CTester::~CTester()
	{	
    //state deleted during tests
	}

CTester::CTester() : CActive(EPriorityIdle),iCurrentTestNumber(0)
	{
	CActiveScheduler::Add(this);
	}

void CTester::ConstructL()
	{
	NextTest();
	CActiveScheduler::Start();
	}

void CTester::NextTest()
	{
	TRequestStatus *pS = &iStatus;
	User::RequestComplete(pS,KErrNone);
	SetActive();
	}

void CTester::RunL()
	{
	if (iCurrentTestNumber == EAsyncOpenNamed) 
		{
		//test.Printf(_L("Open asynchrously..\n"));
    	SetActive();
		iContactOpenOperation = CContactDatabase::Open(KDbFileName,iStatus);
		++iCurrentTestNumber;    		
		}
	else if(iCurrentTestNumber <= EAsyncOpenedNamed)
		{
		//test.Printf(_L("check opened result..\n"));
		if (iStatus.Int() == KErrLocked)
			{
			//test.Printf(_L("database is locked.\n"));
			iCurrentTestNumber = EAsyncOpenNamed;
			iStatus=KErrNone;
			NextTest();
			return;
			}
		test(iStatus.Int() == KErrNone);
		
		if (iDb == NULL)
			{
			iDb = iContactOpenOperation->TakeDatabase();
			if (iDb == NULL)
				{
				iCurrentTestNumber=EAsyncOpenNamed;
				iStatus=KErrNone;
				NextTest();
				return;
				}
			}
		
        const TContactItemId id = (*gAsync->iArray)[iCurrentTestNumber];
        CContactItem* item=iDb->OpenContactLX(id);
   	    CleanupStack::PushL(item);
        
        item->RemoveField(0);
        
   	    TRAPD(err, iDb->CommitContactL(*item));
		if (err != KErrNone && err != KErrLocked )
			{
			//test.Printf(_L("CTest::Run CommitContact failed, error=%d\n"),err);
			User::Leave(err);
			}	
						         
       	CleanupStack::PopAndDestroy(2); // item, lock			
		
		if (iCurrentTestNumber == EAsyncOpenedNamed)
			{
			delete iContactOpenOperation;
			delete iDb;
			}
	
		++iCurrentTestNumber;

		
		NextTest();
		}
	else if (iCurrentTestNumber <= ESyncOpenedNamed)
		{
		//Open synchronously
   	    TRAPD(err, iDb=CContactDatabase::OpenL(KDbFileName));
		if (err != KErrNone )
			{
			//test.Printf(_L("CTest::RunL Open failed, error=%d\n"),err);
			if (err != KErrLocked )
				{
				User::Leave(err);
				}
			else
				{
				iStatus=KErrNone;
				NextTest();
				return;		
				}
			}	
						
		CleanupStack::PushL(iDb);
		test(iDb != NULL);
		

       	const TContactItemId id = (*gAsync->iArray)[iCurrentTestNumber];
        CContactItem* item=iDb->OpenContactLX(id);
       	CleanupStack::PushL(item);
        
        item->RemoveField(0);
        
        TRAP(err, iDb->CommitContactL(*item));
  		if (err != KErrNone )
			{
			//test.Printf(_L("CTest::RunL CommitContact failed, error=%d\n"),err);
			if (err != KErrLocked )
				{
				User::Leave(err);
				}
			} 
				      	
       	CleanupStack::PopAndDestroy(2); // item, lock			
		CleanupStack::PopAndDestroy(iDb);   //iDb

		if (iCurrentTestNumber < ESyncOpenedNamed)
			{
			++iCurrentTestNumber;
			NextTest();
			}
		else
			{
			CActiveScheduler::Stop();
			}					

		}
		
	}   
	 
void CTester::Error(TInt aError)
    {    
    test.Printf(_L("elieli: CTester::RunError(%d)"), aError);
    return;
    }
//
// Main.
//

/**

@SYMTestCaseID     PIM-CNTMODELTESTERASYNCH-0001

*/

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CActiveScheduler* scheduler=new CActiveScheduler;
	test.Start(_L("@SYMTESTCaseID:PIM-CNTMODELTESTERASYNCH-0001 Testing Asynchronously Access"));

	if (scheduler)
		{
		CActiveScheduler::Install(scheduler);
		CTrapCleanup* cleanup=CTrapCleanup::New();
		if (cleanup)
			{
			CTestConductor* testConductor=NULL;
			TRAP_IGNORE(testConductor = CTestConductor::NewL());
			delete testConductor;
			delete cleanup;
			}
		delete scheduler;
		}
	test.End();
	test.Close();
	__UHEAP_MARKEND;
	return KErrNone;
    }
//  End of File  
