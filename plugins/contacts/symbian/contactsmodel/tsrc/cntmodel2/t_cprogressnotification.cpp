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
#include "cntdb.h" 

#include "t_cprogressnotification.h"

/* 
3 steps to create test
======================
1	Derive from MContactUiCompactObserver
2	Get an instance fo the activebase class via - CContactActiveCompress* CContactDatabase::CreateCompressorLC()
3	Call CContactActiveCompress::SetObserver to recieve notifications
*/	

_LIT(KTestDbName, "c:T_CProgressNotification.cdb");
	
void CProgressNotificationTest::AddContactsL(TInt aNoOfCnts)
	{
	TInt max = KMaxArraySize < aNoOfCnts ? KMaxArraySize : aNoOfCnts;
	// Add contacts
	CContactItem* cntItem = NULL;
	for (TInt ii = 0; ii < max; ++ii)
		{
		cntItem = iCntItemBldr->GetCntItemLC();
		iIDarray[ii] = iCntDb->AddNewContactL(*cntItem);
		CleanupStack::PopAndDestroy(cntItem);
		}
	}
	
void CProgressNotificationTest::DeleteContactsL(TInt aNoOfCnts)
	{
	TInt max = KMaxArraySize < aNoOfCnts ? KMaxArraySize : aNoOfCnts;
	//Remove the contacts
	for (TInt ii = 0; ii < max; ++ii)
		{
		iCntDb->DeleteContactL(iIDarray[ii]);
		iIDarray[ii] = 0;
		}
	}
	
	
void CProgressNotificationTest::HandleError(TInt aError)
	{
	iTest->Printf(_L(" - Received Compress Recover ERROR code:  %d \r\n"), aError);
	(*iTest)(EFalse); //Panic here
	}

	

void CProgressNotificationTest::Step(TInt aStep)
	{
	iTest->Printf(_L(" - Recieved Compress/Recover Event - Step: %d \r\n"), aStep);
	iSteps = aStep;

	if(aStep > 0 && iActiveCompress != NULL) 
		{
		if(!(iActiveCompress->StepsTogo() > 0 ))
			{
			iTest->Printf(_L(" ---- *****ERROR**** Compress Test Error - StepsTogo should be greater than zero \r\n"));
			(*iTest)(EFalse); //Panic here
			}
		}
	}
	

void CProgressNotificationTest::RunCompressNotifyTestL()
	{
  	iTest->Start(_L("Compress Test"));

	if (iActiveCompress)
		delete iActiveCompress;
	iActiveCompress = iCntDb->CreateCompressorLC();
	CleanupStack::Pop(iActiveCompress);
	
	// check if first step is greater than 0; CreateCompressorLC should have started the compression
	if(!(iActiveCompress->StepsTogo() > 0 ))
		{
		iTest->Printf(_L(" ---- *****ERROR**** Compress Test Error - StepsTogo should be greater than zero \r\n"));
		(*iTest)(EFalse); //Panic here
		}

	iActiveCompress->SetObserver(static_cast<MContactUiCompactObserver*>(this));
	while(iActiveCompress->Step()) {};
	
	iTest->End(); //If we are here test completed with success
	}
	
	
void CProgressNotificationTest::RunCompressNotifyCancelTestL()	
	{
  	iTest->Start(_L("Compress CANCEL Test"));

	if (iActiveCompress)
		delete iActiveCompress;
	iActiveCompress = iCntDb->CreateCompressorLC();
	CleanupStack::Pop(iActiveCompress);
	
	iActiveCompress->SetObserver(static_cast<MContactUiCompactObserver*>(this));
	iActiveCompress->Step();
	TInt halfSteps = iSteps/2;
	
	while (iSteps > halfSteps)
		{
		iActiveCompress->Step();
		}

	delete iActiveCompress;
	iActiveCompress = NULL;
	
	iTest->End(); //If we are here test completed with success
	}
	
	
void CProgressNotificationTest::RunRecoverNotifyCancelTestL()
	{
	iTest->Start(_L("Recover CANCEL Test"));
	
	iCntDb->DamageDatabaseL(0x666);
	
	if (iCntDb->IsDamaged() == EFalse)
		{
		iTest->Printf(_L(" ---- DATABASE IS NOT DAMAGED For Test ---- \r\n"));	
#ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__		
		(*iTest)(EFalse); //Panic here
#else
		return;  // test has no meaning with SQLite
#endif		
		}
	
	if (iActiveRecover)
		delete iActiveRecover;
	iActiveRecover = iCntDb->CreateRecoverLC();
	CleanupStack::Pop(iActiveRecover);
	
	iActiveRecover->SetObserver(static_cast<MContactUiCompactObserver*>(this));
	
	//do only one step
	iActiveRecover->Step();
	
	if (iCntDb->IsDamaged() == EFalse)
		{
		iTest->Printf(_L(" ---- DATABASE IS NOT DAMAGED For Test ---- \r\n"));	
		(*iTest)(EFalse); //Panic here
		}
		
	TRAPD(err, AddContactsL(1));
	if(err == KErrNone)
		{
		iTest->Printf(_L(" ---- *****ERROR**** recover cancellation does not works ---- \r\n"));	
		(*iTest)(EFalse); //Panic here
		}

	//recover stop in destructor
	delete iActiveRecover;
	iActiveRecover = NULL;	
	
	iCntDb->RecoverL();
	
	iTest->End(); //If we are here test completed with success
	}

	
void CProgressNotificationTest::RunRecoverNotifyTestL()
	{
	iTest->Start(_L("Recover Test"));
	
	iCntDb->DamageDatabaseL(0x666);
	if (iCntDb->IsDamaged())
		{
		iTest->Printf(_L(" ---- DATABASE IS DAMAGED ---- \r\n"));	
		}
	
  	if (iActiveRecover)
		delete iActiveRecover;
	iActiveRecover = iCntDb->CreateRecoverLC();
	CleanupStack::Pop(iActiveRecover);
	
	iActiveRecover->SetObserver(static_cast<MContactUiCompactObserver*>(this));
	while(iActiveRecover->Step()) {};	
	
	if (iCntDb->IsDamaged())
		{
		iTest->Printf(_L(" ----*****ERROR**** DATABASE IS STILL DAMAGED ---- \r\n"));	
		(*iTest)(EFalse); //Panic here
		}
		
	iTest->End(); //If we are here test completed with success
	}

	
CProgressNotificationTest* CProgressNotificationTest::NewLC()
	{
	CProgressNotificationTest* self = new (ELeave) CProgressNotificationTest();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;		
	}
	
CProgressNotificationTest::CProgressNotificationTest()
	{}
	

void CProgressNotificationTest::ConstructL()
	{
	// For multi-threaded test harness.
	iTest = new(ELeave) RTest(_L(""));

	iCntDb = CContactDatabase::ReplaceL(KTestDbName); // Default Database.
	const CContactTemplate& sysTempl(GetSysTemplateL() );
	iCntItemBldr = CCntItemBuilder::NewLC(sysTempl);
	CleanupStack::Pop(iCntItemBldr);
	}

CProgressNotificationTest::~CProgressNotificationTest()
	{
	iTest->Close();
	delete iTest;
	
	delete iCntDb;
	delete iGoldenTemplate;
	delete iCntItemBldr;

	delete iActiveCompress;
	delete iActiveRecover;
	TRAP_IGNORE(CContactDatabase::DeleteDatabaseL(KTestDbName));
	}

const CContactTemplate& CProgressNotificationTest::GetSysTemplateL()
	{
	if (!iGoldenTemplate)
		{
		CContactItemViewDef* matchAll = CContactItemViewDef::NewLC(CContactItemViewDef::EIncludeFields,CContactItemViewDef::EIncludeHiddenFields);
		matchAll->AddL(KUidContactFieldMatchAll);

		iGoldenTemplate = static_cast<CContactTemplate*>(iCntDb->ReadContactL(iCntDb->TemplateId(), *matchAll));
		CleanupStack::PopAndDestroy(matchAll);
		}
	return *iGoldenTemplate;
	} 
  	
		
