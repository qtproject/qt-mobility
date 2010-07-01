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
*
*/


/**
 @file 
 @publishedAll
 @released
*/
#include "requestmixstep.h"
#include "performancefunctionalitydefs.h"

_LIT(KRun1,"SingleSession");
_LIT(KRun2,"MultipleSession");
_LIT(KRun3,"MultipleDestroy");
_LIT(KRun4,"SingleTransaction");
_LIT(KRun5,"MultipleTransaction");
_LIT(KRun6,"IndefiniteLock");
_LIT(KRun7,"OddSession");

_LIT(KTest1, "KSingle session tests");
_LIT(KTest2, "Multiple session tests");
_LIT(KTest3, "multiple destroy tests");
_LIT(KTest4, "KSingle session Transaction tests");
_LIT(KTest5, "Multiple session Transaction tests");
_LIT(KTest6, "Indefinite Locking test");
_LIT(KTest7, "Transaction Odd Session Commit test");

#define KNumberOfContacts 10

CRequestMixStep::~CRequestMixStep()
	{
	}

CRequestMixStep::CRequestMixStep() : CPerformanceFunctionalityBase(KNumberOfContacts)
	{
	SetTestStepName(KRequestMixStep);
	}


TVerdict CRequestMixStep::doTestStepL()
	{
	__UHEAP_MARK;
	InitializeL();
	_LIT(KStartTest,"CRequestMixStep::doTestStepL()");
	INFO_PRINTF1(KStartTest);  //Block start 
	CContactDatabase *contactsDatabase2 = CContactDatabase::OpenL();//open another seesion to cntsrv
																	//used by tests that require 2 sessions
	CleanupStack::PushL( contactsDatabase2 );
	iIterate->Reset();

	const TDesC &KRun = ConfigSection();
	
	if( KRun == KRun1 )
		{
		INFO_PRINTF1(KTest1);
		SingularTestsL();
		}
	else if( KRun == KRun2 )
		{
		INFO_PRINTF1(KTest2);
		SingularTestsL(contactsDatabase2);
		}
	else if( KRun == KRun3 )
		{
		INFO_PRINTF1(KTest3);
		MultipleDestroyTestL();
		}
	else if( KRun == KRun4 )
		{
		INFO_PRINTF1(KTest4);
		TransactionTestsL();
		}
	else if( KRun == KRun5 )
		{
		INFO_PRINTF1(KTest5);
		TransactionTestsL(contactsDatabase2);
		}
	else if( KRun == KRun6 )
		{
		INFO_PRINTF1(KTest6);
		EndlessLockTestL();
		}
	else if( KRun == KRun7 )
		{
		INFO_PRINTF1(KTest7);
		TransactionOddSessionCommitTestL(contactsDatabase2);
		}
	else
		{
		MissingTestPanic();
		}
	
	CleanupStack::PopAndDestroy(contactsDatabase2);
	Cleanup();
	__UHEAP_MARKEND;

	return TestStepResult();
	}

void CRequestMixStep::SingularTestsL(CContactDatabase *aDb2)
	{
	UpdateTestL(aDb2);
	CloseTestL(aDb2);
	OpenCloseTestL(aDb2);
	ReadOpenCloseTestL(aDb2);
	MultipleOpenTestL(aDb2);
	}

void CRequestMixStep::TransactionTestsL(CContactDatabase *aDb2)
	{
	TransactionCommitTestL(aDb2);
	TransactionCloseTestL(aDb2);
	TransactionReadCloseTestL(aDb2);
	TransactionDeleteTestL(aDb2);
	}

void CRequestMixStep::UpdateTestL(CContactDatabase *aDb2)
	{
	if(!aDb2)
		{
		aDb2 = iContactsDatabase;
		}
	TInt err = KErrNone;
	CContactItem *contactItem1 = NULL;
	CContactItem *contactItem2 = NULL;
	CContactItem *contactItem3 = NULL;
	
	iIterate->Reset();
	TContactItemId con1 = iIterate->NextL();
	TContactItemId con2 = iIterate->NextL();
	//Lock contact A using session 1
	contactItem1 = iContactsDatabase->OpenContactLX(con1);
	CleanupStack::PushL(contactItem1);
	
	//Attemp to open contact a with session 1/2
	TRAP(err, contactItem2 = aDb2->OpenContactL(con1) );
	
	TESTPRINT( err == KErrInUse );
	CLEAR( contactItem2 );
	
	err = KErrNone;
	//Attemp to read contact a with session 1/2
	TRAP(err, contactItem2 = aDb2->ReadContactL(con1) );
	
	TESTPRINT(KErrNone == err) ;
	CLEAR( contactItem2 );
	
	
	err = KErrNone;
	//read contact b with session 1/2
	contactItem2 = aDb2->ReadContactLC(con2);
	//attempt to update contact 1 with contact 2 using session 1/2
	TRAP(err, contactItem3 = aDb2->UpdateContactLC(con1, contactItem2); 
			  CleanupStack::PopAndDestroy(contactItem3);
		);
	TESTPRINT(KErrInUse == err) ;
	contactItem3 = NULL;
	CleanupStack::PopAndDestroy(contactItem2);
	contactItem2 = NULL;
	
	
	err = KErrNone;
	//attempt to delete contact 1 with session 1/2
	TRAP(err, aDb2->DeleteContactL(con1) );
	TESTPRINT(KErrInUse == err) ;
	
	CleanupStack::PopAndDestroy(contactItem1);
	CleanupStack::PopAndDestroy();//unlock contact 1
	contactItem1 = NULL;
	}

void CRequestMixStep::CloseTestL(CContactDatabase *aDb2)
	{
	if(!aDb2)
		{
		aDb2 = iContactsDatabase;
		}
	
	TInt err = KErrNone;
	CContactItem *contactItem1 = NULL;
	
	iIterate->Reset();
	TContactItemId con1 = iIterate->NextL();
	contactItem1 = iContactsDatabase->ReadContactL(con1);
	
	TRAP(err, aDb2->CommitContactL(*contactItem1) );
	CLEAR( contactItem1 );
	TESTPRINT(KErrAccessDenied == err);
	
	contactItem1 = iContactsDatabase->ReadContactL(con1);
	
	err = KErrNone;
	TRAP(err, aDb2->CloseContactL(con1) );
	CLEAR( contactItem1 );
	TESTPRINT(KErrNone == err);
	}

void CRequestMixStep::OpenCloseTestL(CContactDatabase *aDb2)
	{
	const TBool KSingle = !aDb2;
	if(KSingle)
		{
		aDb2 = iContactsDatabase;
		}
	
	TInt err = KErrNone;
	CContactItem *contactItem1 = NULL;
	
	iIterate->Reset();
	TContactItemId con1 = iIterate->NextL();
	contactItem1 = iContactsDatabase->OpenContactLX(con1);
	CleanupStack::PushL(contactItem1);
	
	TRAP(err, aDb2->CommitContactL(*contactItem1) );
	if(KSingle)
		{
		TESTPRINT(KErrNone == err);
		}
	else
		{
		TESTPRINT(KErrInUse == err);
		}
	
	err = KErrNone;
	TRAP(err, iContactsDatabase->CommitContactL(*contactItem1) );
	CleanupStack::PopAndDestroy( contactItem1 );
	CleanupStack::PopAndDestroy();
	contactItem1 = NULL;
	if(KSingle)
		{
		TESTPRINT(KErrAccessDenied == err);
		}
	else
		{
		TESTPRINT(KErrNone == err);
		}
	
	contactItem1 = iContactsDatabase->OpenContactLX(con1);
	CleanupStack::PushL(contactItem1);
	
	TRAP(err, aDb2->CloseContactL(con1) );
	TESTPRINT(KErrNone == err);
	
	err = KErrNone;
	TRAP(err, aDb2->CloseContactL(con1) );
	CleanupStack::PopAndDestroy( contactItem1 );
	CleanupStack::PopAndDestroy();
	contactItem1 = NULL;
	TESTPRINT(KErrNone == err);
	}

void CRequestMixStep::ReadOpenCloseTestL(CContactDatabase *aDb2)
	{
	const TBool KSingle = !aDb2;
	if(!aDb2)
		{
		aDb2 = iContactsDatabase;
		}
	
	TInt err = KErrNone;
	CContactItem *contactItem1 = NULL;
	CContactItem *contactItem2 = NULL;
	
	iIterate->Reset();
	TContactItemId con1 = iIterate->NextL();
	TContactItemId con2 = iIterate->NextL();

	contactItem1 = iContactsDatabase->OpenContactLX(con1);
	CleanupStack::PushL(contactItem1);
	contactItem2 = aDb2->ReadContactL(con1);
	
	TRAP(err, aDb2->CommitContactL(*contactItem2) );
	if(KSingle)
		{
		TESTPRINT(KErrNone == err);
		}
	else
		{
		TESTPRINT(KErrInUse == err);
		}
	
	CleanupStack::PopAndDestroy( contactItem1 );
	CleanupStack::PopAndDestroy();//destroy lock (unlock)
	contactItem1 = NULL;
	CLEAR(contactItem2);
	
	contactItem1 = iContactsDatabase->OpenContactLX(con1);
	CleanupStack::PushL(contactItem1);
	contactItem2 = aDb2->ReadContactL(con1);
	
	TRAP(err, aDb2->CloseContactL(con1) );
	TESTPRINT(KErrNone == err);
	
	CleanupStack::PopAndDestroy( contactItem1 );
	CleanupStack::PopAndDestroy();//destroy lock (unlock)
	contactItem1 = NULL;
	CLEAR(contactItem2);
	
	contactItem1 = iContactsDatabase->OpenContactLX(con1);
	CleanupStack::PushL(contactItem1);
	contactItem2 = aDb2->ReadContactL(con2);
	
	TRAP(err, aDb2->CommitContactL(*contactItem2) );
	TESTPRINT(KErrAccessDenied == err);
	
	CleanupStack::PopAndDestroy( contactItem1 );
	CleanupStack::PopAndDestroy();//destroy lock (unlock)
	contactItem1 = NULL;
	CLEAR(contactItem2);
	}
/**
for single session:
open x many contact then commit them all
open x many contacts, close them, then destroy the locks
for multiple sessions:
open x many contacts then commit them using different session
open x many contacts, check that they are still locked, then close them
*/
void CRequestMixStep::MultipleOpenTestL(CContactDatabase *aDb2)
	{
	const TBool KSingle = !aDb2;
	if(KSingle)
		{
		aDb2 = iContactsDatabase;
		}
	
	const TInt KMany = 5;
	TInt i = 0;
	TInt err = KErrNone;
	CContactItem *contactItems[KMany];
	TContactItemId con1 = 0;
	
	iIterate->Reset();
	
	for(i = 0; i < KMany; ++i)
		{
		contactItems[i] = iContactsDatabase->OpenContactLX( iIterate->NextL() );
		CleanupStack::PushL( contactItems[i] );
		}
		
	for(i = 0; i < KMany; ++i)
		{
		TRAP(err, aDb2->CommitContactL( *contactItems[i] ));
		if(KSingle)
			{
			TESTPRINTI(KErrNone == err, i);
			}
		else
			{
			TESTPRINTI(KErrInUse == err, i);
			}
		}
		
	for(i = KMany - 1; i >= 0; --i)
		{
		CleanupStack::PopAndDestroy( contactItems[i] );
		CleanupStack::Pop();//pop lock
		contactItems[i] = NULL;
		}
		
	iIterate->Reset();
	err = KErrNone;
	if(KSingle)
		{
		for(i = 0; i < KMany; ++i)
			{
			contactItems[i] = iContactsDatabase->OpenContactLX( iIterate->NextL() );
			CleanupStack::PushL( contactItems[i] );
			}
			
		for(i = 0; i < KMany; ++i)
			{
			TRAP(err, iContactsDatabase->CloseContactL( contactItems[i]->Id() ));
			TESTPRINTI(KErrNone == err, i);
			}
			
		for(i = KMany - 1; i >= 0; --i)
			{
			CleanupStack::PopAndDestroy( contactItems[i] );
			CleanupStack::PopAndDestroy();
			contactItems[i] = NULL;
			}
		}
	else
		{	
		for(i = 0; i < KMany; ++i)
			{
			con1 = iIterate->NextL();
			TRAP(err, contactItems[0] = iContactsDatabase->OpenContactL( con1 ) );
			TESTPRINTI(KErrInUse == err, i);
			iContactsDatabase->CloseContactL( con1 );
			CLEAR(contactItems[0]);
			}
		}
	}
/**
open x many contacts and push lock on stack
destroy all contacts and locks
check all contacts are now unlocked
*/
void CRequestMixStep::MultipleDestroyTestL()
	{
	const TInt KMany = 5;
	TInt i = 0;
	TInt err = KErrNone;
	CContactItem *contactItems[KMany];
	
	iIterate->Reset();
	
	for(i = 0; i < KMany; ++i)
		{
		contactItems[i] = iContactsDatabase->OpenContactLX( iIterate->NextL() );
		CleanupStack::PushL( contactItems[i] );
		}		
		
	for(i = KMany - 1; i >= 0; --i)
		{
		CleanupStack::PopAndDestroy( contactItems[i] );
		CleanupStack::PopAndDestroy();
		contactItems[i] = NULL;
		}	
		
	iIterate->Reset();	
	for(i = 0; i < KMany; ++i)
		{
		TRAP(err, contactItems[0] = iContactsDatabase->OpenContactL( iIterate->NextL() ) );
		TESTPRINTI(KErrNone == err, i);//checks the contact is no longer locked
		CLEAR( contactItems[0] );
		}
		
	iIterate->Reset();
	for(i = 0; i < KMany; ++i)
		{
		iContactsDatabase->CloseContactL( iIterate->NextL() );
		}	
	}

void CRequestMixStep::TransactionCommitTestL( CContactDatabase *aDb2 )
	{
	if(!aDb2)
		{
		aDb2 = iContactsDatabase;
		}
	
	TInt err = KErrNone;
	CContactItem *contactItem1 = NULL;
	CContactItem *contactItem2 = NULL;
	
	iIterate->Reset();
	TContactItemId con1 = iIterate->NextL();

	contactItem1 = aDb2->OpenContactLX(con1);
	CleanupStack::PushL(contactItem1);
	
	iContactsDatabase->DatabaseBeginLC(EFalse);
	
	TRAP(err, contactItem2 = iContactsDatabase->OpenContactL( con1 ) );
	TESTPRINT( KErrInUse == err );
	CLEAR( contactItem2 );
	TRAP(err, iContactsDatabase->doCommitContactL(*contactItem1,ETrue,ETrue) );
	TESTPRINT( KErrNone == err );
	
	iContactsDatabase->DatabaseCommitLP(EFalse);
	
	CleanupStack::PopAndDestroy( contactItem1 );
	CleanupStack::PopAndDestroy();
	contactItem1 = NULL;
	CLEAR(contactItem2);
	}

void CRequestMixStep::TransactionCloseTestL( CContactDatabase *aDb2 )
	{
	if(!aDb2)
		{
		aDb2 = iContactsDatabase;
		}
	
	TInt err = KErrNone;
	CContactItem *contactItem1 = NULL;
	CContactItem *contactItem2 = NULL;
	
	iIterate->Reset();
	TContactItemId con1 = iIterate->NextL();

	contactItem1 = aDb2->OpenContactLX(con1);
	CleanupStack::PushL(contactItem1);
	
	iContactsDatabase->DatabaseBeginLC(EFalse);
	
	TRAP(err, iContactsDatabase->CloseContactL( con1 ) );
	TESTPRINT( KErrNone == err );
	
	iContactsDatabase->DatabaseCommitLP(EFalse);
	
	CleanupStack::PopAndDestroy( contactItem1 );
	CleanupStack::PopAndDestroy();
	contactItem1 = NULL;
	CLEAR(contactItem2);
	}

void CRequestMixStep::TransactionReadCloseTestL( CContactDatabase *aDb2 )
	{
	if(!aDb2)
		{
		aDb2 = iContactsDatabase;
		}
	
	TInt err = KErrNone;
	CContactItem *contactItem1 = NULL;
	CContactItem *contactItem2 = NULL;
	
	iIterate->Reset();
	TContactItemId con1 = iIterate->NextL();
	TContactItemId con2 = iIterate->NextL();

	contactItem1 = aDb2->ReadContactLC(con1);
	contactItem2 = aDb2->ReadContactLC(con2);
	
	iContactsDatabase->DatabaseBeginLC(EFalse);
	
	TRAP(err, iContactsDatabase->doCommitContactL( *contactItem1,ETrue,ETrue ) );
	TESTPRINT( KErrAccessDenied == err );
	
	TRAP(err, iContactsDatabase->CloseContactL( con2 ) );
	TESTPRINT( KErrNone == err );
	
	iContactsDatabase->DatabaseCommitLP(EFalse);
	
	CleanupStack::PopAndDestroy( contactItem2 );
	CleanupStack::PopAndDestroy( contactItem1 );
	contactItem1 = NULL;
	contactItem2 = NULL;
	}

void CRequestMixStep::TransactionDeleteTestL( CContactDatabase *aDb2 )
	{

	TInt err = KErrNone;
	
	if( NULL == aDb2 )
		{
		TESTPRINT(NULL == aDb2);
		return;
		}
	
	iIterate->Reset();
	TContactItemId con1 = iIterate->NextL();
	CContactItem *contactItem1 = NULL;

	iContactsDatabase->DatabaseBeginLC(EFalse);
	
	TRAP(err, aDb2->doDeleteContactL( con1, ETrue, ETrue ) );
	TESTPRINT( KErrLocked == err );
	
	iContactsDatabase->DatabaseRollback();
	CleanupStack::Pop();
	if( iContactsDatabase->IsDamaged() )
		{
		iContactsDatabase->RecoverL();
		}
	
	TRAP(err, contactItem1 = iContactsDatabase->ReadContactL( con1 ) );
	CLEAR( contactItem1 );
	TESTPRINT( KErrNone == err );
	
	if( err == KErrNotFound )
		{
		iIterate->RemoveL( con1 );
		AddEmptyContactsL(1, EFalse);
		}
	}

void CRequestMixStep::TransactionOddSessionCommitTestL( CContactDatabase *aDb2 )
	{
	TInt err = KErrNone;
	
	if( NULL == aDb2 )
		{
		TESTPRINT( NULL == aDb2 );
		return;
		}
	iIterate->Reset();
	TContactItemId con1 = iIterate->NextL();
	CContactItem *contactItem1 = NULL;

	iContactsDatabase->DatabaseBeginLC(EFalse);
	
	TRAP(err, iContactsDatabase->doDeleteContactL( con1, ETrue, ETrue ) );
	TESTPRINT( KErrNone == err );
	
	TRAP( err, aDb2->DatabaseCommitLP(EFalse) );
	TESTPRINT( KErrLocked == err );
	if( err )
		{
		CleanupStack::Pop();
		}
	TRAP(err, contactItem1 = iContactsDatabase->ReadContactL( con1 ) );
	CLEAR( contactItem1 );
	TESTPRINT( KErrNotFound == err );
	
	if( err == KErrNotFound )
		{
		iIterate->RemoveL( con1 );
		AddEmptyContactsL(1, EFalse);
		}
	}

/**open contact, kill database session, 
reopen database, check that contact is not still locked*/
void CRequestMixStep::EndlessLockTestL( )
	{
	CContactItem *contactItem2 = NULL;
	TContactItemId con1 = iIterate->NextL();
	
	TInt err = KErrNone;
	iIterate->Reset();
	OpenL( con1 );
	
	CLEAR(iContactsDatabase);
	iContactsDatabase = CContactDatabase::OpenL();
	TRAP(err, contactItem2 = iContactsDatabase->OpenContactL( con1 ) );
	TESTPRINT( err == KErrNone );
	CLEAR(contactItem2);
	CloseL( ETrue );
	}
