/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Timeout tests involve:
* Locking a contact then attempting to perform a crud operation on the contact
* check the timeout and error returned are as expected.
*
*/


/**
 @file 
 @publishedAll
 @released
*/
#include "ccontactprivate.h"
#include "timeoutstep.h"
#include "performancefunctionalitydefs.h"

_LIT(KRun1,"ContactLock");
_LIT(KRun2,"TransactionLock");
_LIT(KRun3,"TemplateLock");
_LIT(KRun4,"TimeoutRange");
_LIT(KRun5,"MultipleSessionContactLock");
_LIT(KRun6,"MultipleSessionTransactionLock");
_LIT(KRun7,"MultipleSessionTemplateLock");
_LIT(KRun8,"MultipleSessionTimeoutRange");

_LIT(KTest1, "Contact locking timeout tests");
_LIT(KTest2, "Transaction locking timeout tests");
_LIT(KTest3, "Transaction internal locking timeout tests");
_LIT(KTest4, "Template locking timeout tests");
_LIT(KTest5, "Value range timeout tests");

_LIT(KErrOnIteration,"On iteration number %d :");

const TInt KIterations = 5;

#define KNumberOfContacts 50
	
CTimeoutStep::CTimeoutStep() 
		: 	CTimeoutBaseStep( KNumberOfContacts, KDeviation )
	{
	SetTestStepName(KTimeoutStep);
	}

TVerdict CTimeoutStep::doTestStepL()
	{
	__UHEAP_MARK;
	InitializeL();
	_LIT(KDoStepPrint,"CTimeoutStep::doTestStepL()");
	INFO_PRINTF1(KDoStepPrint);  //Block start 
	iIterate->Reset();
	CContactDatabase *contactsDatabase = CContactDatabase::OpenL();
	CleanupStack::PushL( contactsDatabase );
	
	const TDesC &run = ConfigSection();
	
	if( run == KRun1 )
		{
		TEST1(ContactLockingTestL(iContactsDatabase), ETrue);
		}
	else if( run == KRun2 )
		{
		TEST1(TransactionInternalLockingTestL(), ETrue);
		}
	else if( run == KRun3 )
		{
		TEST1(TemplateLockingTestL(iContactsDatabase), ETrue);
		}
	else if( run == KRun4 )
		{
		TimeoutValuesTestL(iContactsDatabase);
		}
	else if( run == KRun5 )
		{
		TEST1(ContactLockingTestL(contactsDatabase), ETrue);
		}
	else if( run == KRun6 )
		{
		TEST1(TransactionLockingTestL(contactsDatabase), ETrue);
		}
	else if( run == KRun7 )
		{
		TEST1(TemplateLockingTestL(contactsDatabase), ETrue);
		}
	else if( run == KRun8 )
		{
		TimeoutValuesTestL(contactsDatabase);
		}
	else
		{
		MissingTestPanic();
		}

	
	CleanupStack::PopAndDestroy(contactsDatabase);
	Cleanup();
	__UHEAP_MARKEND;

	return TestStepResult();
	}
	
TBool CTimeoutStep::ContactLockingTestL(CContactDatabase *aContactsDatabase)
	{
	INFO_PRINTF1(KTest1);
	TBool ret = EFalse;
	TContactItemId cid = 0;
	TInt i = 0;
	for(; i < KIterations ; ++i, iTimeOut += KDefaultTime )
		{
		cid = iIterate->NextL();
		OpenL(cid);//locks contact with id == cid
		ret = CheckTimeOutL(&CTimeoutStep::CheckOpenL, aContactsDatabase, cid, KErrInUse, iTimeOut);
		ret = ret && CheckTimeOutL(&CTimeoutStep::CheckUpdateL, aContactsDatabase, cid, KErrInUse, iTimeOut, EFalse);
		ret = ret && CheckTimeOutL(&CTimeoutStep::CheckRead, aContactsDatabase, cid, KErrNone, 0, EFalse);
		ret = ret && CheckTimeOutL(&CTimeoutStep::CheckDelete, aContactsDatabase, cid, KErrInUse, iTimeOut, EFalse);
		CloseL( ETrue );
		if(!ret)
			{			
			ERR_PRINTF2(KErrOnIteration,i+1);
			break;
			}
		}
	iTimeOut = KStartTime;
	return ret;
	}

TBool CTimeoutStep::TransactionLockingTestL(CContactDatabase *aContactsDatabase)
	{
	INFO_PRINTF1(KTest2);
	iTransaction = ETrue;
	TBool ret = EFalse;
	TContactItemId cid = 0;
	TInt i = 0;
	aContactsDatabase->DatabaseBeginLC( EFalse ); // locks the entire database
	for(; i < KIterations ; ++i, iTimeOut += KDefaultTime )
		{
		cid = iIterate->NextL();
		ret = CheckTimeOutL(&CTimeoutStep::CheckOpenL, iContactsDatabase, cid, KErrLocked, iTimeOut);
		ret = ret && CheckTimeOutL(&CTimeoutStep::CheckUpdateL, iContactsDatabase, cid, KErrLocked, iTimeOut, EFalse);
		ret = ret && CheckTimeOutL(&CTimeoutStep::CheckRead, iContactsDatabase, cid, KErrNone, 0, EFalse);
		ret = ret && CheckTimeOutL(&CTimeoutStep::CheckAddL, iContactsDatabase, cid, KErrLocked, iTimeOut, EFalse);
		ret = ret && CheckTimeOutL(&CTimeoutStep::CheckDelete, iContactsDatabase, cid, KErrLocked, iTimeOut, EFalse);
		if(!ret)
			{
			ERR_PRINTF2(KErrOnIteration,i+1);
			break;
			}

		}
	aContactsDatabase->DatabaseCommitLP( EFalse );
	iTimeOut = KStartTime;
	iTransaction = EFalse;
	return ret;
	}

TBool CTimeoutStep::TransactionInternalLockingTestL()
	{
	INFO_PRINTF1(KTest3);
	iTransaction = ETrue;
	TBool ret = EFalse;
	TContactItemId cid = 0;
	TInt i = 0;
	iContactsDatabase->DatabaseBeginLC( EFalse );
	for(; i < KIterations ; ++i, iTimeOut += KDefaultTime )
		{
		cid = iIterate->NextL();
		OpenL(cid);//locks contact with id == cid
		ret = CheckTimeOutL(&CTimeoutStep::CheckOpenL, iContactsDatabase, cid, KErrInUse, iTimeOut);
		ret = ret && CheckTimeOutL(&CTimeoutStep::CheckUpdateL, iContactsDatabase, cid, KErrInUse, iTimeOut, EFalse);
		ret = ret && CheckTimeOutL(&CTimeoutStep::CheckRead, iContactsDatabase, cid, KErrNone, 0, EFalse);
		ret = ret && CheckTimeOutL(&CTimeoutStep::CheckAddL, iContactsDatabase, cid, KErrNone, 0, EFalse);
		ret = ret && CheckTimeOutL(&CTimeoutStep::CheckDelete, iContactsDatabase, cid, KErrInUse, iTimeOut, EFalse);
		CloseL( ETrue );
		if(!ret)
			{
			ERR_PRINTF2(KErrOnIteration,i+1);
			break;
			}

		}
	iContactsDatabase->DatabaseCommitLP( EFalse );
	iTimeOut = KStartTime;
	iTransaction = EFalse;
	return ret;
	}
	
TBool CTimeoutStep::TemplateLockingTestL(CContactDatabase *aContactsDatabase)
	{
	INFO_PRINTF1(KTest4);
	TBool ret = EFalse;
	TContactItemId cid = iIterate->NextL();
	
	ReadL( cid );
	_LIT(KTLabel,"UserDefined template");
	CContactItem *ctemplate = iContactsDatabase->CreateContactCardTemplateLC(iContactItem, KTLabel);
	cid = aContactsDatabase->AddNewContactL(*ctemplate);
	CloseL( EFalse );
	
	TInt i = 0;
	for(; i < KIterations ; ++i, iTimeOut += KDefaultTime )
		{
		OpenL( ctemplate->Id() );//locks contact/template with id == cid
		ret = CheckTimeOutL(&CTimeoutStep::CheckOpenL, aContactsDatabase, ctemplate->Id(), KErrInUse, iTimeOut);
		ret = ret && CheckTimeOutL(&CTimeoutStep::CheckUpdateL, aContactsDatabase, ctemplate->Id(), KErrInUse, iTimeOut, EFalse);
		ret = ret && CheckTimeOutL(&CTimeoutStep::CheckRead, aContactsDatabase, ctemplate->Id(), KErrNone, 0, EFalse);
		ret = ret && CheckTimeOutL(&CTimeoutStep::CheckDelete, aContactsDatabase, ctemplate->Id(), KErrInUse, iTimeOut, EFalse);
		ret = ret && CheckTimeOutL(&CTimeoutStep::CheckOpenL, aContactsDatabase, cid, KErrInUse, iTimeOut, EFalse);
		ret = ret && CheckTimeOutL(&CTimeoutStep::CheckUpdateL, aContactsDatabase, cid, KErrInUse, iTimeOut, EFalse);
		ret = ret && CheckTimeOutL(&CTimeoutStep::CheckRead, aContactsDatabase, cid, KErrNone, 0, EFalse);
		ret = ret && CheckTimeOutL(&CTimeoutStep::CheckDelete, aContactsDatabase, cid, KErrInUse, iTimeOut, EFalse);
		CloseL( ETrue );
		if(!ret)
			{
			ERR_PRINTF2(KErrOnIteration,i+1);
			break;
			}
		}
	iTimeOut = KStartTime;
	CleanupStack::PopAndDestroy( ctemplate );
	return ret;
	}
	
void CTimeoutStep::TimeoutValuesTestL(CContactDatabase *aContactsDatabase)
	{
	INFO_PRINTF1(KTest5);
	TContactItemId cid = iIterate->NextL();
	
	iTimeOut = KStartTime;
	TRAPD(err, SetTimeOutL( aContactsDatabase ) );
	TESTPRINT(KErrNone == err);
	
	
	iTimeOut = KMinTInt;
	TRAP(err, SetTimeOutL( aContactsDatabase ) );
	TESTPRINT(KErrArgument == err);
	OpenL( cid );
	TESTPRINT( CheckTimeOutL(&CTimeoutStep::CheckOpenL, aContactsDatabase, cid, KErrInUse, KStartTime, EFalse) );
	CloseL( ETrue );
	
	iTimeOut = -1;
	TRAP(err, SetTimeOutL( aContactsDatabase ) );
	TESTPRINT(KErrArgument == err);
	OpenL( cid );
	TESTPRINT(CheckTimeOutL(&CTimeoutStep::CheckOpenL, aContactsDatabase, cid, KErrInUse, KStartTime, EFalse) );
	CloseL( ETrue );
	
	iTimeOut = 0;
	TRAP(err, SetTimeOutL( aContactsDatabase ) );
	TESTPRINT( KErrNone == err );
	OpenL( cid );
	TESTPRINT( CheckTimeOutL(&CTimeoutStep::CheckOpenL, aContactsDatabase, cid, KErrInUse, iTimeOut, EFalse) );
	CloseL( ETrue );
	
	iTimeOut = 1;
	TRAP(err, SetTimeOutL( aContactsDatabase ) );
	TESTPRINT(KErrNone == err);
	OpenL( cid );
	TESTPRINT( CheckTimeOutL(&CTimeoutStep::CheckOpenL, aContactsDatabase, cid, KErrInUse, iTimeOut, EFalse) );
	CloseL( ETrue );
	
	iTimeOut = KFiveMins - 1;		
	TRAP(err, SetTimeOutL( aContactsDatabase ) );
	TESTPRINT(KErrNone == err);

	iTimeOut = KFiveMins;		
	TRAP(err, SetTimeOutL( aContactsDatabase ) );
	TESTPRINT( KErrNone == err );
	OpenL( cid );
	TESTPRINT( CheckTimeOutL(&CTimeoutStep::CheckOpenL, aContactsDatabase, cid, KErrInUse, iTimeOut, EFalse) );
	CloseL( ETrue );
	
	iTimeOut = KFiveMins + 1;		
	TRAP(err, SetTimeOutL( aContactsDatabase ) );
	TESTPRINT(KErrArgument == err);
	
	iTimeOut = KMaxTInt;		
	TRAP(err, SetTimeOutL( aContactsDatabase ) );
	TESTPRINT(KErrArgument == err);
	

	iTimeOut = KStartTime;
	SetTimeOutL();
	}


