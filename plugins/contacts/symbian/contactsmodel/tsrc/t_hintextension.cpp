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
*
*/


#include <e32std.h>
#include <e32test.h>
#include <cntdb.h>

#include <f32file.h>
#include "t_hintextension.h"
#include <coreappstest/testserver.h>



_LIT(KTestName, "T_HintExtension");

_LIT(KSrcDatabaseFileName, "Z:\\t_HintExtension_original.cdb");

_LIT(KDatabaseSecureName, "C:t_HintExtension_in_use.cdb");
_LIT(KDatabaseSecurePhysicalName, "C:\\private\\10003A73\\t_HintExtension_in_use.cdb");

_LIT(KDatabaseFileName, "C:\\t_HintExtension_in_use.cdb");

_LIT(KCntDbSecureFormat,"secure[100065FF]");

_LIT(KViewName,"RemoteView");

//from cntstd.h
_LIT(KIdentityTableName, "IDENTITYTABLE");
_LIT(KContactExtHintField, "CM_ExtHintField");



//////////// Logging macros ///////////////
GLDEF_C RTest test(KTestName);
int __ttp_err;

_LIT(KTestSmile, "\n{°±°}");
_LIT(KTestNoErrMask, "%s(%d) : runtime error: Function '%s' returned %d\n");
_LIT(KTestTrapMask,  "%s(%d) : runtime error: Function '%s' left with code %d\n");
_LIT(KTestTrueMask,  "%s(%d) : runtime error: Statement '%s' equals %d\n");



#define __ttr(x, c, l, f)  \
	if (x == EFalse) \
	_TestNoErrL(KTestTrueMask, x, _S(c), l,_S(f))

#define TestTrue(x) __ttr(x, #x, __LINE__,__FILE__)


#define __ttt(x, c, l, f)  \
	{ const TInt res = x; if (res != KErrNone)   \
	  _TestNoErrL(KTestNoErrMask, res, _S(c), l,_S(f));  }

#define TestNoErrL(x) __ttt(x, #x, __LINE__,__FILE__)



#define __ttp(x, c, l, f)		\
	TRAP(__ttp_err, x);		\
	if (__ttp_err != KErrNone)	\
	  _TestNoErrL(KTestTrapMask, __ttp_err, _S(c), l,_S(f))

#define TestTrap(x) __ttp(x, #x, __LINE__,__FILE__)


void _TestNoErrL(const TDesC& aMask, TInt aResult, const TText* aCommand, TInt aLineNum,const TText* aFileName)
	{
	TBuf<0x400> str;
	str.Format(aMask, aFileName, aLineNum, aCommand, aResult);
	test.Printf(KTestSmile);
	test.Printf(str);
	User::Leave(aResult < 0 ? aResult : KErrUnknown);
	}
///////////////////////////////////////////////

RPIMTestServer serv;


CViewTester* CViewTester::NewLC()
	{
	CViewTester* self=new(ELeave) CViewTester();
	CleanupStack::PushL(self);
	//self->ConstructL();
	return self;
	}

CViewTester::~CViewTester()
	{
	iRingToneView->Close(*this);
	iVoiceDialView->Close(*this);
	iCNRView->Close(*this);

	delete iDatabase;
	}


/**

@SYMTestCaseID     PIM-T-HINTEXTENSION-0001

*/

void CViewTester::CreateFilterViewsL()
	{
	test.Start(_L("@SYMTESTCaseID:PIM-T-HINTEXTENSION-0001 Checking that ERingTone and EVoiceDial views work and empty"));

	//Creating Database
	TestTrap(iDatabase = CContactDatabase::OpenL(KDatabaseSecureName));

	//Creating Sort Order
	TAutoClose<RContactViewSortOrder> theSortOrder;
	theSortOrder.iObj.AppendL(KUidContactFieldGivenName);

	//Creating Views
	TestTrap(iCNRView=CContactNamedRemoteView::NewL(*this,KViewName,*iDatabase,theSortOrder.iObj,EContactsOnly));
	TestTrap(iRingToneView = CContactFilteredView::NewL( *this, *iDatabase, *iCNRView, CContactDatabase::ERingTone));
	TestTrap(iVoiceDialView = CContactFilteredView::NewL( *this, *iDatabase, *iCNRView, CContactDatabase::EVoiceDial));

	CActiveScheduler::Start();
	test.End();
	}



void CViewTester::HandleContactViewEvent(const CContactViewBase& aView,const TContactViewEvent& aEvent)
	{

	if (aEvent.iEventType == TContactViewEvent::EReady)
		{
		TInt err(KErrNone);
		++iEventsReceived;
		if (&aView == iCNRView)
			{//Ignore main view
			test.Printf(_L("Main view ready\n"));
			}
		else if (&aView == iRingToneView)
			{
			test.Printf(_L("RingTone view ready"));
			TRAP(err, TestNoErrL(aView.CountL() ) );
			test(err == KErrNone);
			test.Printf(_L(" and empty!\n"));
			}
		else if (&aView == iVoiceDialView)
			{
			test.Printf(_L("VoiceDial view ready"));
			TRAP(err, TestNoErrL(aView.CountL() ) );
			test(err == KErrNone);
			test.Printf(_L(" and empty!\n"));
			}
		else
			{
			test.Printf(_L("Unknown view in CViewTester::HandleContactViewEvent"));
			test(EFalse);
			}

		if (iEventsReceived == 3) //events from, iCNRView, iRingToneView and iVoiceDialView
			CActiveScheduler::Stop();
		}
	}




TBool ExtHintFieldExistsL(RFs& theFS)
	{
	TBool theResult;
	RDbNamedDatabase theDatabase;
	
	TestNoErrL(theDatabase.Open(theFS, KDatabaseFileName,KCntDbSecureFormat));

	CDbColSet* theIdentitiTable = theDatabase.ColSetL(KIdentityTableName);
	CleanupStack::PushL(theIdentitiTable);
	const TDbCol* theExtHint = theIdentitiTable->Col(KContactExtHintField);
	theResult = (theExtHint != NULL); //The field shouldn't exits

	CleanupStack::PopAndDestroy(); //theIdentitiTable
	theDatabase.Close();

	return theResult;
	}

void TestExtHintFieldCreationL(RFs& theFS)
	{
	test.Start(_L("Ext Hint field creation"));


    serv.CopyFileL(KSrcDatabaseFileName, KDatabaseFileName);
	serv.CopyFileL(KSrcDatabaseFileName, KDatabaseSecurePhysicalName);
		
	test.Printf(_L("Checking ExtHintField before loading cntmodel\n"));
	TestTrue(!ExtHintFieldExistsL(theFS)); //The field shouldn't exist in original table

	//Contact model must open and upgrade database
	test.Printf(_L("Loading cntmodel\n"));
	CContactDatabase* theDatabase = NULL; 
	
	TestTrap(theDatabase = CContactDatabase::OpenL(KDatabaseSecureName));
	delete theDatabase;
	theDatabase = NULL;

	//Copy the file to the C drive, where it can be checked by the test code through DBMS
	serv.CopyFileL(KDatabaseSecurePhysicalName, KDatabaseFileName);

	test.Printf(_L("Checking ExtHintField after opening with cntmodel\n"));
	TestTrue(ExtHintFieldExistsL(theFS)); //Contacts model must have created the ExtHintField
	test.End();
	}



void DoTestL()
	{
	TAutoClose<RFs> theFS;
	TestNoErrL(theFS.iObj.Connect());

    User::LeaveIfError(serv.Connect());
	CleanupClosePushL(serv);

	TestExtHintFieldCreationL(theFS.iObj);

	CViewTester* viewTester = CViewTester::NewLC();
	viewTester->CreateFilterViewsL();
	CleanupStack::PopAndDestroy(viewTester);

    serv.DeleteFileL(KDatabaseFileName);
	serv.DeleteFileL(KDatabaseSecurePhysicalName);

	CleanupStack::PopAndDestroy(&serv);
	}


GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CActiveScheduler* scheduler=new CActiveScheduler;
	if (scheduler)
		{
		CActiveScheduler::Install(scheduler);
		CTrapCleanup* cleanup=CTrapCleanup::New();
		if (cleanup)
			{
			test.Title();
			test.Start(_L("T_HintExtension"));

			TRAPD(err, DoTestL()); 
			test(err == KErrNone);
			test.End();

			//test.Printf(_L("Exiting in one second...\n"));
			User::After(1000000);
			test.Close();
			delete cleanup;
			}
		delete scheduler;
		}
	__UHEAP_MARKEND;
	return KErrNone;
    }


