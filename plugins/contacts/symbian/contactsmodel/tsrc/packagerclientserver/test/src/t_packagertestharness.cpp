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
#include "t_packagertestharness.h"


_LIT(KTest,"T_PackagerTestHarness test");
	
static RTest test(KTest);


CPackagerTestHarness::CPackagerTestHarness() :CActive(EPriorityStandard), iTestStep(0), iNextStep(0)
/** Constructor. */ 
 	{
 	CActiveScheduler::Add(this);
 	}
 	
CPackagerTestHarness::~CPackagerTestHarness()
	{
/** The destructor frees all resources owned by CPackagerTestHarness, 
prior to its destruction. */	
	delete iFactory;
	delete iComparator;
	delete iContactItem;
	delete iContentType;
	delete iContactTextDef;
	delete iContactItemViewDef;
	delete iContactViewDef;
	delete iContactIdArray;
	delete iContactFilter;
	delete iViewContact;
	delete iDesCArray;	
	
	iClient.Close();
	}


CPackagerTestHarness* CPackagerTestHarness::NewL()
/** Allocates and constructs a new default CPackagerTestHarness. 

@return Pointer to the new CPackagerTestHarness object. */
	{
	CPackagerTestHarness* self = CPackagerTestHarness::NewLC();
	CleanupStack::Pop();
	return self;
	}	

CPackagerTestHarness* CPackagerTestHarness::NewLC()
/** Allocates and constructs a new default CPackagerTestHarness. 

@return Pointer to the new CPackagerTestHarness object. */
	{
	CPackagerTestHarness* self = new(ELeave) CPackagerTestHarness;
	CleanupStack::PushL(self);
	self->ConstructL();
	
	return self;
	}

void CPackagerTestHarness::ConstructL()
/** Second-phase construction.
Initializes the Factory, Comparator and Client. */
	{
	FactoryL();
	ComparatorL();
	iClient.ConnectL();
	}
	
void CPackagerTestHarness::StartActiveObject()
/** Sets this active object to active. Does a request complete
so that RunL gets called right away. */	
	{
	TRequestStatus *pS = &iStatus;
	User::RequestComplete(pS, KErrNone);
	SetActive();
	}	

void CPackagerTestHarness::SendCContactItemL()
/** Sends a CContactItem object to the server to be tested. Asynchronous
request, so SetActive is called. */
	{
	iClient.PackAndSendL(iFactory->CreateDefaultCContactItemL(), iStatus);
	SetActive();
	}
	
void CPackagerTestHarness::SendCContentTypeL()
/** Sends a CContentType object to the server to be tested. Asynchronous
request, so SetActive is called. */
	{
	iClient.PackAndSendL(iFactory->CreateDefaultCContentTypeL(), iStatus);
	SetActive();
	}
	
void CPackagerTestHarness::SendCContactTextDefL()
/** Sends a CContactTextDef object to the server to be tested. Asynchronous
request, so SetActive is called. */
	{
	iClient.PackAndSendL(iFactory->CreateDefaultCContactTextDefL(), iStatus);
	SetActive();	
	}
	
void CPackagerTestHarness::SendCContactItemViewDefL()
/** Sends a CContactItemViewDef object to the server to be tested. Asynchronous
request, so SetActive is called. */
	{
	iClient.PackAndSendL(iFactory->CreateDefaultCContactItemViewDefL(), iStatus);
	SetActive();
	}
	
void CPackagerTestHarness::SendCContactViewDefL()
/** Sends a CContactViewDef object to the server to be tested. Asynchronous
request, so SetActive is called. */
	{
	iClient.PackAndSendL(iFactory->CreateDefaultCContactViewDefL(), iStatus);
	SetActive();
	}
	
void CPackagerTestHarness::SendCContactIdArrayL()
/** Sends a CContactIdArray object to the server to be tested. Asynchronous
request, so SetActive is called. */
	{
	iClient.PackAndSendL(iFactory->CreateDefaultCContactIdArrayL(), iStatus);
	SetActive();
	}
	
void CPackagerTestHarness::SendCCntFilterL()
/** Sends a CCntFilter object to the server to be tested. Asynchronous
request, so SetActive is called. */
	{
	iClient.PackAndSendL(iFactory->CreateDefaultCCntFilterL(), iStatus);
	SetActive();
	}
	
void CPackagerTestHarness::SendCViewContactL()
/** Sends a CViewContact object to the server to be tested. Asynchronous
request, so SetActive is called. */
	{
	iClient.PackAndSendL(iFactory->CreateDefaultCViewContactL(), iStatus);
	SetActive();
	}
	
void CPackagerTestHarness::SendCDesCArrayL()
/** Sends a CDesCArray object to the server to be tested. Asynchronous
request, so SetActive is called. */
	{
	iClient.PackAndSendL(iFactory->CreateDefaultCDesCArrayL(), iStatus);
	SetActive();
	}	

CPackagerCntFactory& CPackagerTestHarness::FactoryL() const
/** Retrieves the CPackagerCntFactory class member, constructing a new object 
if it has not been previously allocated. */
	{
	if(!iFactory)
		{
		iFactory = CPackagerCntFactory::NewL();	
		}
	return *iFactory;
	}
	
CPackagerCntComparator& CPackagerTestHarness::ComparatorL() const
/** Retrieves the CPackagerCntComparator class member, constructing a new object 
if it has not been previously allocated. */
	{
	if(!iComparator)
		{
		iComparator = CPackagerCntComparator::NewL();
		}
	return *iComparator;
	}	
	
void CPackagerTestHarness::GetCContactItemL() const
/** Requests a CContactItem object from the server to be compared with 
a local default object. */
	{
	CContactItem* theItem= iClient.GetAndUnpackCntItemLC();
	test.Printf(_L("GetCContactItemL, Comparison result:%d \n"),
		iComparator->Compare(iFactory->CreateDefaultCContactItemL(), *theItem));
	CleanupStack::PopAndDestroy(theItem);
	}
	
void CPackagerTestHarness::GetCContentTypeL() const
/** Requests a CContentType object from the server to be compared with 
a local default object. */
	{
	CContentType* theItem= iClient.GetAndUnpackCntContentTypeLC();
	test.Printf(_L("GetCContentTypeL, Comparison result:%d \n"),
		iComparator->Compare(iFactory->CreateDefaultCContentTypeL(), *theItem));
	CleanupStack::PopAndDestroy(theItem);
	}
	
void CPackagerTestHarness::GetCContactTextDefL() const
/** Requests a CContactTextDef object from the server to be compared with 
a local default object. */
	{
	CContactTextDef* theItem= iClient.GetAndUnpackCntTextDefLC();
	test.Printf(_L("GetCContactTextDefL, Comparison result:%d \n"),
		iComparator->Compare(iFactory->CreateDefaultCContactTextDefL(), *theItem));
	CleanupStack::PopAndDestroy(theItem);
	}
	
void CPackagerTestHarness::GetCContactItemViewDefL() const
/** Requests a CContactItemViewDef object from the server to be compared with 
a local default object. */
	{
	CContactItemViewDef* theItem= iClient.GetAndUnpackCntItemViewDefLC();
	test.Printf(_L("GetCContactItemViewDefL, Comparison result:%d \n"),
		iComparator->Compare(iFactory->CreateDefaultCContactItemViewDefL(), *theItem));
	CleanupStack::PopAndDestroy(theItem);
	}
	
void CPackagerTestHarness::GetCContactViewDefL() const
/** Requests a CContactViewDef object from the server to be compared with 
a local default object. */
	{
	CContactViewDef* theItem= iClient.GetAndUnpackCntViewDefLC();
	test.Printf(_L("GetCContactViewDefL, Comparison result:%d \n"),
		iComparator->Compare(iFactory->CreateDefaultCContactViewDefL(), *theItem));
	CleanupStack::PopAndDestroy(theItem);
	}
	
void CPackagerTestHarness::GetCContactIdArrayL() const
/** Requests a CContactIdArray object from the server to be compared with 
a local default object. */
	{
	CContactIdArray* theItem= iClient.GetAndUnpackCntIdArrayLC();
	test.Printf(_L("GetCContactIdArrayL, Comparison result:%d \n"),
		iComparator->Compare(iFactory->CreateDefaultCContactIdArrayL(), *theItem));
	CleanupStack::PopAndDestroy(theItem);
	}
	
void CPackagerTestHarness::GetCCntFilterL() const
/** Requests a CCntFilter object from the server to be compared with 
a local default object. */
	{
	CCntFilter* theItem= iClient.GetAndUnpackCntFilterLC();
	test.Printf(_L("GetCCntFilterL, Comparison result:%d \n"),
		iComparator->Compare(iFactory->CreateDefaultCCntFilterL(), *theItem));
	CleanupStack::PopAndDestroy(theItem);
	}
	
void CPackagerTestHarness::GetCViewContactL() const
/** Requests a CViewContact object from the server to be compared with 
a local default object. */
	{
	CViewContact* theItem= iClient.GetAndUnpackViewCntLC();
	test.Printf(_L("GetCViewContactL, Comparison result:%d \n"),
		iComparator->Compare(iFactory->CreateDefaultCViewContactL(), *theItem));
	CleanupStack::PopAndDestroy(theItem);
	}
	
void CPackagerTestHarness::GetCDesCArrayL() const
/** Requests a CDesCArray object from the server to be compared with 
a local default object. */
	{
	CDesCArray* theItem= iClient.GetAndUnpackDesCArrayLC();
	test.Printf(_L("GetCDesCArrayL, Comparison result:%d \n"),
		iComparator->Compare(iFactory->CreateDefaultCDesCArrayL(), *theItem));
	CleanupStack::PopAndDestroy(theItem);
	}
	
void CPackagerTestHarness::DoCancel()
/** Pure virtual method in parent CActive class. */
	{

	}	
	
void CPackagerTestHarness::RunL()
/** Active Object's RunL implementation. */
	{
	switch (iTestStep)
		{
		case EAsynchTests:
			NextAsynchStepL();
			++iTestStep;
			break;
			
		case ESynchTests:
			test.Printf(_L("Asynch Test %d: Server Comparison result:%d \n"),
			iNextStep, iStatus.Int());	
			NextSynchStepL();
			if(iNextStep==EReceiveCContactItem)
				{
				CActiveScheduler::Stop();
				}
				
			else
				{
				--iTestStep;
				StartActiveObject();
				}
			
			break;
					
		default:
			ASSERT(EFalse);
			break;	
		}
	}
	
void CPackagerTestHarness::NextAsynchStepL()
/** Asynchronous test methods. Packs and sends items to server-side to be
compared against a local copy. */
	{
	switch(iNextStep)
		{
		case EProcessCContactItem:
			SendCContactItemL();
			break;
			
		case EProcessCContentType:
			SendCContentTypeL();
			break;
				
		case EProcessCContactTextDef:
			SendCContactTextDefL();
			break;
				
		case EProcessCContactItemViewDef:
			SendCContactItemViewDefL();
			break;
				
		case EProcessCContactViewDef:
			SendCContactViewDefL();
			break;
				
		case EProcessCContactIdArray:
			SendCContactIdArrayL();
			break;
				
		case EProcessCCntFilter:
			SendCCntFilterL();
			break;
				
		case EProcessCViewContact:
			SendCViewContactL();
			break;
				
		case EProcessCDesCArray:
			SendCDesCArrayL();
			break;
		
		default:
			break;		
		}
	}
	
void CPackagerTestHarness::NextSynchStepL()
/** Synchronous test methods. Requests items from server-side to be
compared against a local copy. */
	{
	switch(iNextStep)
		{
		case EProcessCContactItem:
			GetCContactItemL();
			++iNextStep;
			break;
			
		case EProcessCContentType:
			GetCContentTypeL();
			++iNextStep;
			break;
				
		case EProcessCContactTextDef:
			GetCContactTextDefL();
			++iNextStep;
			break;
				
		case EProcessCContactItemViewDef:
			GetCContactItemViewDefL();
			++iNextStep;
			break;
				
		case EProcessCContactViewDef:
			GetCContactViewDefL();
			++iNextStep;
			break;
				
		case EProcessCContactIdArray:
			GetCContactIdArrayL();
			++iNextStep;
			break;
				
		case EProcessCCntFilter:
			GetCCntFilterL();
			++iNextStep;
			break;
				
		case EProcessCViewContact:
			GetCViewContactL();
			++iNextStep;
			break;
				
		case EProcessCDesCArray:
			GetCDesCArrayL();
			++iNextStep;
			break;
		
		default:
			break;		
		}
	}

//@SYMTestCaseID PIM-TPACKAGERTESTHARNESS-0001
	
static void MainL()
	{
	test.Start(_L("@SYMTestCaseID PIM-TPACKAGERTESTHARNESS-0001 T_PackagerTestHarness test"));
	CPackagerTestHarness* testSrvTest = CPackagerTestHarness::NewL();
	
	// Start tests 
	testSrvTest->StartActiveObject();
	CActiveScheduler::Start();
	
	delete testSrvTest;
	testSrvTest = NULL;

	test.End();
	test.Close();
	}
	

TInt E32Main()
	{
	__UHEAP_MARK;
	test.Title();
	CActiveScheduler* scheduler=new CActiveScheduler;
	if (scheduler)
		{
		CActiveScheduler::Install(scheduler);
		CTrapCleanup* cleanup = CTrapCleanup::New();
		if (cleanup)
			{
			TRAPD(err,MainL());	
			__ASSERT_ALWAYS(err == KErrNone, User::Panic(_L("DBDumper Failed"),err) ); 
			delete cleanup;
			}
		delete scheduler;
		}
	test.Close(); 
	__UHEAP_MARKEND;
	
	return KErrNone;
	}
