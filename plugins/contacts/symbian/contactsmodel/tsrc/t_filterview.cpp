/*
* Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <cntitem.h>
#include <cntfldst.h>
#include "t_utils2.h"
#include "t_filterview.h"
#include "t_utils.h"

//
// Constants.
//

_LIT(KTestName,"t_filterview");
_LIT(KLogFileName,"t_filterview.log");

_LIT(KDbFileName,"c:t_filterview.cdb");

_LIT(KRemoteViewName,"RemoteView");
_LIT(KTextDefSeparator,"");

LOCAL_D RTest test(KTestName);

const TInt KNumSmsContacts = 111;
const TInt KNumWorkEmailContacts = 121;
const TInt KNumHomeEmailContacts = 131;
const TInt KNumLandlineContacts = 141;
const TInt KNumFaxContacts = 151;
const TInt KNumRingTone = 161;
const TInt KNumVoiceDial = 171;

const TInt KNumIMProtocolWV = 53; 
const TInt KNumIMAddress= KNumIMProtocolWV;

const TInt KNumPhonable = KNumSmsContacts + KNumLandlineContacts + KNumFaxContacts;
const TInt KNumUnfiltered =	KNumSmsContacts
							+ KNumWorkEmailContacts
							+ KNumHomeEmailContacts
							+ KNumLandlineContacts
							+ KNumFaxContacts
							+ KNumRingTone
							+ KNumVoiceDial
							+ KNumIMAddress;


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
	CCntTest::ProfileReset(0,1);	// ensure the profiling 'TLS' used in EKA2 is freed
	}

CTestConductor::CTestConductor() {}

void CTestConductor::ConstructL()
	{
	User::LeaveIfError(iFs.Connect());
	iLog=CLog::NewL(test,KLogFileName);
	iDb=CContactDatabase::ReplaceL(KDbFileName);
	iRandomGenerator=CRandomContactGenerator::NewL();
	iRandomGenerator->SetDbL(*iDb);
	AddContactsL();
	}

void CTestConductor::AddContactL(TInt aBitwiseFilterType)
	{
	test.Printf(_L("Adding Contact"));
	iRandomGenerator->AddTypicalContactForFilterL(aBitwiseFilterType);
	iTotalContacts++;
	}

void CTestConductor::AddContactsThatMatchFilterL(TInt aNumberOfContacts, TInt aFilter)
	{
	test.Printf(_L("Adding %d contacts which match filter 0x%X : "), aNumberOfContacts, aFilter);
	TInt counter = 0;
	for (counter = 0; counter < aNumberOfContacts; counter++)
		{
		if(aFilter & CContactDatabase::EWirelessVillage)
			{
			iRandomGenerator->AddTypicalContactForFilterL(aFilter, KUidContactFieldVCardMapWV);
			}
		else
			{
			iRandomGenerator->AddTypicalContactForFilterL(aFilter);
			}
		iTotalContacts++;
		}
	test.Printf(_L("Done\n"));
	}

void CTestConductor::AddContactsL()
	{
	iTotalContacts=0;
	test.Printf(_L("Adding \n"));

	AddContactsThatMatchFilterL(KNumSmsContacts, CContactDatabase::ESmsable);
	AddContactsThatMatchFilterL(KNumWorkEmailContacts, CContactDatabase::EMailable);
	AddContactsThatMatchFilterL(KNumHomeEmailContacts, CContactDatabase::EMailable | CContactDatabase::EHome);
	AddContactsThatMatchFilterL(KNumLandlineContacts, CContactDatabase::ELandLine);
	AddContactsThatMatchFilterL(KNumFaxContacts, CContactDatabase::EFaxable);
	AddContactsThatMatchFilterL(KNumRingTone, CContactDatabase::ERingTone);
	AddContactsThatMatchFilterL(KNumVoiceDial, CContactDatabase::EVoiceDial);
	AddContactsThatMatchFilterL(KNumIMProtocolWV, /*CContactDatabase::EIMAddress |*/ CContactDatabase::EWirelessVillage);	
	}

void CTestConductor::RunTestsL()
	{
	CViewTester* tester=CViewTester::NewL(*iLog,*iDb,this);
	CleanupStack::PushL(tester);
	CActiveScheduler::Start();
	CleanupStack::Pop(tester);

	User::LeaveIfError(iTestError);
	}


void CTestConductor::SetTestError(TInt aTestError)
	{
	iTestError = aTestError;
	}


//
// CViewTester.
//

CViewTester* CViewTester::NewL(CLog& aLog,CContactDatabase& aDb,CTestConductor* aTestConductor)
	{
	CViewTester* self=new(ELeave) CViewTester(aLog,aDb,aTestConductor);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CViewTester::~CViewTester()
	{
	iVoiceDialView->Close(*this);
	iRingToneView->Close(*this);
	iLandlineFilterView->Close(*this);
	iLandlineFilterView2Fields->Close(*this);
	iSmsFilterView->Close(*this);
	iEmailFilterView->Close(*this);
	iFaxFilterView->Close(*this);
	iPhonableFilterView->Close(*this);
	iWorkFilterView->Close(*this);
	iHomeFilterView->Close(*this);
	iUnfilteredView->Close(*this);
	iWirelessVillageView->Close(*this);
	iIMAddressView->Close(*this);
	iOOMIMAddressView->Close(*this);
	iNamedRemoteView->Close(*this);
	iSortOrder_1.Close();
	iSortOrder_2.Close();
	delete iTextDef;
	}

CViewTester::CViewTester(CLog& aLog,CContactDatabase& aDb,CTestConductor* aTestConductor)
	: CActive(EPriorityStandard),iLog(aLog),iDb(aDb),iCurrentTest(-1),iTestConductor(aTestConductor)
	{
	CActiveScheduler::Add(this);
	}

void CViewTester::ConstructL()
	{
	iSortOrder_1.AppendL(KUidContactFieldGivenName);
	iSortOrder_1.AppendL(KUidContactFieldFamilyName);
	iSortOrder_1.AppendL(KUidContactFieldCompanyName);

	iTextDef=CContactTextDef::NewL();
	iTextDef->AppendL(TContactTextDefItem(KUidContactFieldGivenName,KTextDefSeparator));
	iTextDef->AppendL(TContactTextDefItem(KUidContactFieldFamilyName,KTextDefSeparator));
	iTextDef->AppendL(TContactTextDefItem(KUidContactFieldCompanyName,KTextDefSeparator));

	iSortOrder_2.AppendL(KUidContactFieldCompanyName);
	iSortOrder_2.AppendL(KUidContactFieldFamilyName);
	iSortOrder_2.AppendL(KUidContactFieldGivenName);

	NextTest();
	}

void CViewTester::PrintTimeTaken(TInt aTime)
	{
	iScratchBuf.Format(_L("**  Test Took [%ds%dus]  **\n"), aTime/1000000, aTime%1000000);
	iLog.LogLine(iScratchBuf);
	//TBuf<100> buffer;
	//buffer.Format(_L("  Test Took [%ds%dus]\n"), aProfile.iTime/1000000, aProfile.iTime%1000000);
	//iLog.LogLine(buffer);
	}


void CViewTester::PrintTotalTimeTaken()
	{
	TInt counter;
	TInt totalTime = 0;

	for (counter = 0; counter < ENumTests; counter++)
		{
		totalTime += iTimeForTestsToComplete[counter];
		}

	iScratchBuf.Format(_L("**  Total amount of time taken [%ds%dus]  **"), totalTime/1000000, totalTime%1000000);
	iLog.LogLine(iScratchBuf);

	}

CContactFilteredView* CViewTester::CreateFilterViewL()
	{
	CCntTest::ProfileReset(0,1);
	CCntTest::ProfileStart(0);
	return CContactFilteredView::NewL(*this,iDb,*iNamedRemoteView, iFilter);
	}

void CViewTester::RunL()
	{
	switch (iCurrentTest)
		{
		case ECreateLocalView:
			iLog.LogLine(_L("=== Create Name Remote local view"));
			CCntTest::ProfileReset(0,1);
			CCntTest::ProfileStart(0);
			iNamedRemoteView=CContactNamedRemoteView::NewL(*this,KRemoteViewName,iDb,iSortOrder_1,EContactsOnly);
			break;
		case EExerciseLocalView:
			iLog.LogLine(_L("=== Exercise local view"));
			ExceriseViewL(*iNamedRemoteView);
			NextTest();
			break;
		case ECreateLandlineFilterView:
			{
			iLog.LogLine(_L("=== Create Land line Filter View {CContactFilteredView}"));
			iFilter = CContactDatabase::ELandLine;
			iLandlineFilterView = CreateFilterViewL();
			}
			break;
		case ETestLandlineFilterView:
			{
			iLog.LogLine(_L("==== Exercise ETestLandlineFilterView"));
			test(iLandlineFilterView->CountL()==KNumLandlineContacts);
			TestViewIndiciesL(*iLandlineFilterView);
			NextTest();
			}
			break;
		case ECreateSmsFilterView:
			{
			iLog.LogLine(_L("==== Create SMS Filter View"));
			iFilter=CContactDatabase::ESmsable;
			iSmsFilterView=CreateFilterViewL();
			}
			break;
		case ETestSmsFilterView:
			{
			iLog.LogLine(_L("==== Exercise ETestSmsFilterView"));
			test(iSmsFilterView->CountL()==KNumSmsContacts);
			TestViewIndiciesL(*iSmsFilterView);
			NextTest();
			}
			break;
		case ECreateEmailFilterView:
			{
			iLog.LogLine(_L("==== Create Email Filter View"));
			iFilter=CContactDatabase::EMailable;
			iEmailFilterView=CreateFilterViewL();
			}
			break;
		case ETestEmailFilterView:
			{
			iLog.LogLine(_L("==== Exercise ETestEmailFilterView"));
			CCntTest::ProfileReset(0,1);
			CCntTest::ProfileStart(0);
			test(iEmailFilterView->CountL()==KNumWorkEmailContacts+KNumHomeEmailContacts);
			TestViewIndiciesL(*iEmailFilterView);
			NextTest();
			}
			break;
		case ECreateFaxFilterView:
			{
			iLog.LogLine(_L("==== Create Fax Filter View"));
			iFilter=CContactDatabase::EFaxable;
			iFaxFilterView=CreateFilterViewL();
			}
			break;
		case ETestFaxFilterView:
			{
			iLog.LogLine(_L("==== Exercise ETestFaxFilterView"));
			test(iFaxFilterView->CountL()==KNumFaxContacts);
			TestViewIndiciesL(*iFaxFilterView);
			NextTest();
			}
			break;
		case ECreatePhonableFilterView:
			{
			iLog.LogLine(_L("==== Create Phonable Filter View"));
			iFilter=CContactDatabase::EPhonable;
			iPhonableFilterView=CreateFilterViewL();
			}
			break;
		case ECreateWorkFilterView:
			{
			iLog.LogLine(_L("==== Create Work Filter View"));
			iFilter=CContactDatabase::EWork;
			iFilter|=CContactDatabase::EMailable;
			iWorkFilterView=CreateFilterViewL();
			}
			break;
		case ECreateHomeFilterView:
			{
			iLog.LogLine(_L("==== Create Home Filter View"));
			iFilter=CContactDatabase::EHome;
			iFilter|=CContactDatabase::EMailable;
			iHomeFilterView=CreateFilterViewL();
			}
			break;
		case ECreateUnfilteredView:
			{
			iLog.LogLine(_L("==== Create Unfiltered Filter View"));
			iFilter=0;
			iUnfilteredView=CreateFilterViewL();
			}
			break;

		case ECreateRingToneView:
			iLog.LogLine(_L("==== Create Unfiltered Filter View"));
			iFilter = CContactDatabase::ERingTone;
			CCntTest::ProfileReset(0,1);
			CCntTest::ProfileStart(0);
			iRingToneView = CreateFilterViewL();
			break;

		case ECreateVoiceDialView:
			iLog.LogLine(_L("==== Create Unfiltered Filter View"));
			iFilter = CContactDatabase::EVoiceDial;
			iVoiceDialView = CreateFilterViewL();
			break;

		case ECreateWirelessVillageAddressView:
			{
			iLog.LogLine(_L("==== Create WirelessVillage address view"));
			iFilter = CContactDatabase::EWirelessVillage;
			iWirelessVillageView = CreateFilterViewL();
			}
			break;

		case ECreateIMAddressView:
			{
			iLog.LogLine(_L("==== Create IM address view"));
			iFilter = CContactDatabase::EIMAddress;
			iIMAddressView = CreateFilterViewL();
			}
			break;

		case ETestPhonableFilterView:
			{
			iLog.LogLine(_L("==== Exercise ETestPhonableFilterView"));
			test(iPhonableFilterView->CountL()==KNumPhonable);
			TestViewIndiciesL(*iPhonableFilterView);
			NextTest();
			}
			break;
		case ETestWorkFilterView:
			{
			iLog.LogLine(_L("==== Exercise ETestWorkFilterView"));
			test(iWorkFilterView->CountL()==KNumWorkEmailContacts);
			TestViewIndiciesL(*iWorkFilterView);
			NextTest();
			}
			break;
		case ETestHomeFilterView:
			{
			iLog.LogLine(_L("==== Exercise ETestHomeFilterView"));
			test(iHomeFilterView->CountL()==KNumHomeEmailContacts);
			TestViewIndiciesL(*iHomeFilterView);
			NextTest();
			}
			break;
		case ETestUnfilteredView:
			{
			iLog.LogLine(_L("==== Exercise EUnfilteredView"));
			test(iUnfilteredView->CountL()==KNumUnfiltered);
			TestViewIndiciesL(*iUnfilteredView);
			NextTest();
			}
			break;

		case ETestRingToneView:
			iLog.LogLine(_L("==== Exercise ETestRingToneView"));
			test(iRingToneView->CountL()==KNumRingTone);
			TestViewIndiciesL(*iRingToneView);
			NextTest();

			break;

		case ETestVoiceDialView:
			iLog.LogLine(_L("==== Exercise ETestVoiceDialView"));
			test( iVoiceDialView->CountL() == KNumVoiceDial );
			TestViewIndiciesL(*iVoiceDialView);
			NextTest();

			break;

		case ETestWirelessVillageAddressView:
		// this should also test the different protocols assigned to the IMAddress for each contact
			{
			iLog.LogLine(_L("==== Exercise WirelessVillageAddressView"));
			TInt numberContacts = iWirelessVillageView->CountL();
			test(numberContacts == KNumIMAddress);
			
			TInt numberWV = 0;

			for(TInt a = 0; a < numberContacts; a++)
				{
				TContactItemId contactID = iWirelessVillageView->AtL(a);
				CContactItem* contact = iDb.ReadContactLC(contactID);	// PUSH
				CContactItemFieldSet& fieldSet = contact->CardFields();
				CContactItemField& field = fieldSet[contact->CardFields().Find(KUidContactFieldIMAddress)];
				TFieldType imProtocol = field.ContentType().FieldType(1);

				if(imProtocol == KUidContactFieldVCardMapWV)
					numberWV++;
				CleanupStack::PopAndDestroy();							// contact
				}
			test(numberWV == KNumIMProtocolWV);

			TestViewIndiciesL(*iWirelessVillageView);
			NextTest();
			}
			break;

		case ETestIMAddressView:
			{
			iLog.LogLine(_L("==== Exercise IMAddressView"));
			test(iIMAddressView->CountL()==KNumIMAddress);
			TestViewIndiciesL(*iIMAddressView);
			NextTest();
			}
			break;

		case ETestRemoveContacts:
			{
			iLog.LogLine(_L("==== Remove Some Contacts"));
			TContactItemId deletedId = iPhonableFilterView->AtL(10);
			iDb.DeleteContactL(deletedId);
			 RDebug::Print(_L("DELETED ID : %d"),deletedId);
			deletedId = iEmailFilterView->AtL(3);
			iDb.DeleteContactL(deletedId);
			RDebug::Print(_L("DELETED ID : %d"),deletedId);
			deletedId = iFaxFilterView->AtL(6);
			iDb.DeleteContactL(deletedId);
			RDebug::Print(_L("DELETED ID : %d"),deletedId);
			iItemEventsExpected=3;
			iItemsEventsRecieved=0;
			}
			break;
		case ETestViewIndiciesAfterRemoval:
			{
			iLog.LogLine(_L("==== Exercise ETestPhonableFilterView after removal"));
			TestViewIndiciesL(*iPhonableFilterView);
			TestViewIndiciesL(*iLandlineFilterView);
			TestViewIndiciesL(*iSmsFilterView);
			TestViewIndiciesL(*iFaxFilterView);
			TestViewIndiciesL(*iEmailFilterView);
			NextTest();
			}
			break;
		case ETestAddContacts:
			{
			iLog.LogLine(_L("==== Add some Contacts"));
			iTestConductor->AddContactL(CContactDatabase::ELandLine);
			iTestConductor->AddContactL(CContactDatabase::EMailable);
			iTestConductor->AddContactL(CContactDatabase::ESmsable);
			iTestConductor->AddContactL(CContactDatabase::EFaxable);
			iItemEventsExpected=4;
			iItemsEventsRecieved=0;
	//		NextTest(); //wait until notitifications has gone through the system
			}
			break;
		case ETestViewIndiciesAfterAddition:
			{
			iLog.LogLine(_L("==== Exercise ETestPhonableFilterView after addition"));
			TestViewIndiciesL(*iPhonableFilterView);
			TestViewIndiciesL(*iLandlineFilterView);
			TestViewIndiciesL(*iSmsFilterView);
			TestViewIndiciesL(*iFaxFilterView);
			TestViewIndiciesL(*iEmailFilterView);
			NextTest();
			}
			break;
		case EAllViewsOutOfBoundsAccess:
			{
			//Views depend on their underlying views being in a good state, however
			//as some base views are potentially in other processes they must be resistant
			//to out of date views accessesing out of bound members, views, should not
			//panic but should leave with KErrNotFound;
			//local view
			TInt err=0;
			iLog.LogLine(_L("=== Test views for out of bounds access"));
			TInt outCount = iPhonableFilterView->CountL();
			TRAP(err,iPhonableFilterView->AtL(outCount));
			test(err==KErrNotFound);
			TRAP(err,iPhonableFilterView->ContactAtL(outCount));
			test(err==KErrNotFound);
			NextTest();
			}
			break;
		case EOOMIMAddressView:
			{
			++iOOMCounter += iOOMCounter/25; //Increasing step as we go further
			iFilter = CContactDatabase::EIMAddress;

			__UHEAP_SETFAIL(RHeap::EDeterministic,iOOMCounter);
			TRAPD(ret, iOOMIMAddressView = CContactFilteredView::NewL(*this,iDb,*iNamedRemoteView, iFilter));

			if (ret==KErrNone) 
				{
				break; //Wait for asynchronous notification in HandleContactViewEvent
				}

			if (ret != KErrNoMemory)
				{
				_LIT(KOMMFail, "OOM Test failed: Leave reason is not KErrNoMemory: %d");
				test.Printf(KOMMFail, ret);
				User::Leave(KErrAbort);
				}
			__UHEAP_RESET;
		
			test.Printf(_L("KErrNoMemory at step %5d"), iOOMCounter);
			TRequestStatus *pS=&iStatus;
			User::RequestComplete(pS,KErrNone);
			SetActive();
			}
			break;

		case EOOMDeleteIMAddressView:
			{
			iOOMIMAddressView->Close(*this);
			iCurrentTest = EOOMIMAddressView; //Back to OOM 
			TRequestStatus *pS=&iStatus;
			User::RequestComplete(pS,KErrNone);
			SetActive();
			}
			break;
			
 		case ECreateLandLineFilterView2Fields:
 			{
 			// For DEF077467. Propagated from DEF074316.
 			// Ensure hint field is set to landline when there's < 3 fields but one of them is voice
 			// One contact should appear in the filtered view 
 			// Clear the database first
 			while(iDb.CountL() !=0)
 				{
 				iDb.DeleteContactL((*iDb.SortedItemsL())[0]);
 				}
 			iFilterViewSavedId = KErrNotFound;
 			// Loop adds 2 contacts with 2 fields, Hint field should be set with ELandLine
 			// first time through loop and hence should appear in the filtered view
 			// Second time through the loop still 2 fields but not voice and hence will not
 			// appear in the filtered view
 			// Verify the correct one is in the view with the contact id
 			for(TInt i=0;i<2;++i)
 				{
 				CContactCard* card=CContactCard::NewLC();
 				CContactItemField* field=CContactItemField::NewLC(KStorageTypeText);
 				field->AddFieldTypeL(KUidContactFieldPhoneNumber);
 				if(i==0)
 					{
 					field->AddFieldTypeL(KUidContactFieldVCardMapVOICE);
 					}
 				else
 					{
 					field->AddFieldTypeL(KUidContactFieldVCardMapPAGER);
 					}
 				field->TextStorage()->SetTextL(_L("02071541222"));
 				card->AddFieldL(*field);
 				CleanupStack::Pop(field);
 				if(i==0)
 					{
 					iFilterViewSavedId = iDb.AddNewContactL(*card);
 					}
 				else
 					{
 					(void)iDb.AddNewContactL(*card);
 					}
 				CleanupStack::PopAndDestroy(card); // card
 				}
 			// Create the filtered view
 			iFilter = CContactDatabase::ESmsable|CContactDatabase::ELandLine;
 			iLandlineFilterView2Fields = CreateFilterViewL();
 			// Event handler kicks to next state where we check result
 			}
 			break;
 			
 		case ETestLandLineFilterView2Fields:
 			{
 			// Should only be one contact in the view even though there's 2 in the database
 			test(iLandlineFilterView2Fields->CountL()==1);
 			TContactItemId id = iLandlineFilterView2Fields->AtL(0);
 			// Make sure it's the one we're expecting
 			test(iFilterViewSavedId == id && iFilterViewSavedId != KErrNotFound);
 			NextTest();
 			}
 			break;
		
		case ENumTests:
			iLog.LogLine(_L("==== Filter View Finished"));
			PrintTotalTimeTaken();
			CActiveScheduler::Stop();
			delete this;
			break;
		default:
			ASSERT(EFalse);
			break;
		}
	}


TInt CViewTester::RunError(TInt aError)
	{
	iTestConductor->SetTestError(aError);

	switch (iCurrentTest)
		{
		case ECreateLocalView: test.Printf(_L("Test failed at step CreateLocalView (%i) with error %i"), iCurrentTest, aError); break;
		case EExerciseLocalView: test.Printf(_L("Test failed at step ExerciseLocalView (%i) with error %i"), iCurrentTest, aError); break;
		case ECreateLandlineFilterView: test.Printf(_L("Test failed at step CreateLandlineFilterView (%i) with error %i"), iCurrentTest, aError); break;
		case ETestLandlineFilterView: test.Printf(_L("Test failed at step TestLandlineFilterView (%i) with error %i"), iCurrentTest, aError); break;
		case ECreateSmsFilterView: test.Printf(_L("Test failed at step CreateSmsFilterView (%i) with error %i"), iCurrentTest, aError); break;
		case ETestSmsFilterView: test.Printf(_L("Test failed at step TestSmsFilterView (%i) with error %i"), iCurrentTest, aError); break;
		case ECreateEmailFilterView: test.Printf(_L("Test failed at step CreateEmailFilterView (%i) with error %i"), iCurrentTest, aError); break;
		case ETestEmailFilterView: test.Printf(_L("Test failed at step TestEmailFilterView (%i) with error %i"), iCurrentTest, aError); break;
		case ECreateFaxFilterView: test.Printf(_L("Test failed at step CreateFaxFilterView (%i) with error %i"), iCurrentTest, aError); break;
		case ETestFaxFilterView: test.Printf(_L("Test failed at step TestFaxFilterView (%i) with error %i"), iCurrentTest, aError); break;
		case ECreatePhonableFilterView: test.Printf(_L("Test failed at step CreatePhonableFilterView (%i) with error %i"), iCurrentTest, aError); break;
		case ECreateWorkFilterView: test.Printf(_L("Test failed at step CreateWorkFilterView (%i) with error %i"), iCurrentTest, aError); break;
		case ECreateHomeFilterView: test.Printf(_L("Test failed at step CreateHomeFilterView (%i) with error %i"), iCurrentTest, aError); break;
		case ECreateUnfilteredView: test.Printf(_L("Test failed at step CreateUnfilteredView (%i) with error %i"), iCurrentTest, aError); break;
		case ECreateRingToneView: test.Printf(_L("Test failed at step CreateRingToneView (%i) with error %i"), iCurrentTest, aError); break;
		case ECreateVoiceDialView: test.Printf(_L("Test failed at step CreateVoiceDialView (%i) with error %i"), iCurrentTest, aError); break;
		case ECreateWirelessVillageAddressView: test.Printf(_L("Test failed at step CreateWirelessVillageAddressView (%i) with error %i"), iCurrentTest, aError); break;
		case ECreateIMAddressView: test.Printf(_L("Test failed at step CreateIMAddressView (%i) with error %i"), iCurrentTest, aError); break;
		case ETestPhonableFilterView: test.Printf(_L("Test failed at step TestPhonableFilterView (%i) with error %i"), iCurrentTest, aError); break;
		case ETestWorkFilterView: test.Printf(_L("Test failed at step TestWorkFilterView (%i) with error %i"), iCurrentTest, aError); break;
		case ETestHomeFilterView: test.Printf(_L("Test failed at step TestHomeFilterView (%i) with error %i"), iCurrentTest, aError); break;
		case ETestUnfilteredView: test.Printf(_L("Test failed at step TestUnfilteredView (%i) with error %i"), iCurrentTest, aError); break;
		case ETestRingToneView: test.Printf(_L("Test failed at step TestRingToneView (%i) with error %i"), iCurrentTest, aError); break;
		case ETestVoiceDialView: test.Printf(_L("Test failed at step TestVoiceDialView (%i) with error %i"), iCurrentTest, aError); break;
		case ETestWirelessVillageAddressView: test.Printf(_L("Test failed at step TestWirelessVillageAddressView (%i) with error %i"), iCurrentTest, aError); break;
		case ETestIMAddressView: test.Printf(_L("Test failed at step TestIMAddressView (%i) with error %i"), iCurrentTest, aError); break;
		case ETestRemoveContacts: test.Printf(_L("Test failed at step TestRemoveContacts (%i) with error %i"), iCurrentTest, aError); break;
		case ETestViewIndiciesAfterRemoval: test.Printf(_L("Test failed at step TestViewIndiciesAfterRemoval (%i) with error %i"), iCurrentTest, aError); break;
		case ETestAddContacts: test.Printf(_L("Test failed at step TestAddContacts (%i) with error %i"), iCurrentTest, aError); break;
		case ETestViewIndiciesAfterAddition: test.Printf(_L("Test failed at step TestViewIndiciesAfterAddition (%i) with error %i"), iCurrentTest, aError); break;
		case EAllViewsOutOfBoundsAccess: test.Printf(_L("Test failed at step AllViewsOutOfBoundsAccess (%i) with error %i"), iCurrentTest, aError); break;
		case EOOMIMAddressView: test.Printf(_L("Test failed at step OOMIMAddressView (%i) with error %i"), iCurrentTest, aError); break;
		case EOOMDeleteIMAddressView: test.Printf(_L("Test failed at step OOMDeleteIMAddressView (%i) with error %i"), iCurrentTest, aError); break;
 		case ECreateLandLineFilterView2Fields: test.Printf(_L("Test failed at step CreateLandlineFilterView (%i) with error %i"), iCurrentTest, aError); break;
 		case ETestLandLineFilterView2Fields: test.Printf(_L("Test failed at step TestLandlineFilterView (%i) with error %i"), iCurrentTest, aError); break;

		case ENumTests: test.Printf(_L("Test failed at step NumTests (%i) with error %i"), iCurrentTest, aError); break;

		default: test.Printf(_L("Test failed at step %i with error %i"), iCurrentTest, aError); break;
		}

	CActiveScheduler::Stop();
	return KErrNone;
	}


// Should always return true for a correct implementation of
// CContactViewBase
TBool CViewTester::ContactIdsEqualL(const CContactViewBase& aView, TInt aIndex)
  {
  TContactItemId viewId = aView.AtL(aIndex);
  //RDebug::Print(_L("UnderLyingView viewId : %d"),viewId);
  TContactItemId contactId = aView.ContactAtL(aIndex).Id();
  //RDebug::Print(_L("Sub View Id contactId : %d"),contactId);
  return (contactId == viewId);
  }

void CViewTester::TestViewIndiciesL(const CContactViewBase& aView)
	{
	for (TInt i=0; i < aView.CountL(); ++i)
	  {
	  test(ContactIdsEqualL(aView,i)!=EFalse);
	  }
	}
/*
CContactFilteredView* filteredView = ...;
for (TInt i=0; i < filteredView->CountL(); ++i)
  {
  ASSERT(ContactIdsEqual(*filteredView, i);
  }*/

void CViewTester::HandleViewCreation(const CContactViewBase& aView, const TContactViewEvent& aEvent, CContactViewBase* aPtrView)
	{
	test(aPtrView==&aView);
	test(aEvent.iEventType==TContactViewEvent::EReady);
	CCntTest::ProfileEnd(0);
	CCntTest::ProfileResult(&iProfile,0,1);
	iTimeForTestsToComplete[iCurrentTest] = iProfile.iTime;
	PrintTimeTaken(iProfile.iTime);
	}


void CViewTester::HandleContactViewEvent(const CContactViewBase& aView,const TContactViewEvent& aEvent)
	{
	switch (iCurrentTest)
		{
		case ECreateLocalView:
			HandleViewCreation( aView, aEvent, iNamedRemoteView );
			break;
		case EExerciseLocalView:
			test(ETrue);
			break;
		case ECreateLandlineFilterView:
			HandleViewCreation( aView, aEvent, iLandlineFilterView );
			break;
		case ETestLandlineFilterView:
			test(EFalse);
			break;
		case ECreateSmsFilterView:
			HandleViewCreation( aView, aEvent, iSmsFilterView );
			break;
		case ETestSmsFilterView:
			test(EFalse);
			break;
		case ECreateEmailFilterView:
			HandleViewCreation( aView, aEvent, iEmailFilterView );
			break;
		case ETestEmailFilterView:
			test(EFalse);
			break;
		case ECreateFaxFilterView:
			HandleViewCreation( aView, aEvent, iFaxFilterView );
			break;
		case ETestFaxFilterView:
			test(EFalse);
			break;
		case ECreatePhonableFilterView:
			HandleViewCreation( aView, aEvent, iPhonableFilterView );
			break;
		case ECreateWorkFilterView:
			HandleViewCreation( aView, aEvent, iWorkFilterView );
			break;
		case ECreateHomeFilterView:
			HandleViewCreation( aView, aEvent, iHomeFilterView );
			break;
		case ECreateUnfilteredView:
			HandleViewCreation( aView, aEvent, iUnfilteredView );
			break;

		case ECreateRingToneView:
			HandleViewCreation( aView, aEvent, iRingToneView );
			break;

		case ECreateVoiceDialView:
			HandleViewCreation( aView, aEvent, iVoiceDialView );
			break;

		case ECreateWirelessVillageAddressView:
			HandleViewCreation( aView, aEvent, iWirelessVillageView );
			break;

		case ECreateIMAddressView:
			HandleViewCreation( aView, aEvent, iIMAddressView );
			break;

		case ETestPhonableFilterView:
			test(EFalse);
			break;
		case ETestRemoveContacts:
			{
			if(aEvent.iEventType==TContactViewEvent::EItemRemoved)
				{
				iItemsEventsRecieved++;
				if(iItemEventsExpected==iItemsEventsRecieved)
					{
					NextTest();
					}
				}
			return;
			}
		case ETestViewIndiciesAfterRemoval:
			test(aEvent.iEventType==TContactViewEvent::EItemRemoved);
			return;
		case ETestAddContacts:
			{
			if(aEvent.iEventType==TContactViewEvent::EItemAdded)
				{
				iItemsEventsRecieved++;
				if(iItemEventsExpected==iItemsEventsRecieved)
					{
					NextTest();
					}
				}
			return;
			}
		case ETestViewIndiciesAfterAddition:
			test(aEvent.iEventType==TContactViewEvent::EItemAdded);
			return;
		case EAllViewsOutOfBoundsAccess:
			{
			test(EFalse);
			break;
			}
		case EOOMIMAddressView:
			{
			__UHEAP_RESET;
			test(iOOMIMAddressView==&aView);
			switch (aEvent.iEventType)
				{
				case TContactViewEvent::EReady:
					test.Printf(_L("OOM: EReady at step %5d\n"), iOOMCounter);
					++iCurrentTest; //Exit OOM loop - goto next test
					break;
				case TContactViewEvent::ESortError:
					test.Printf(_L("OOM: ESortError at step %5d\n"), iOOMCounter);
					break; //Go to EOOMDeleteIMAddressView - we cannot delete view in its callback.
				default:
					test.Printf(_L("OOM: Unexpected notification at step %5d\n"), iOOMCounter);
					test(EFalse); //Unsupported event - fail
				}
			}
			break;

 		case ECreateLandLineFilterView2Fields:
 			{
 			HandleViewCreation( aView, aEvent, iLandlineFilterView2Fields);
 			}
 			break;
			
		case EOOMDeleteIMAddressView: //there should not be any call backs after deletion
		case ENumTests:
		default:
			test(EFalse);
			break;
		}
	if (--iNumNotificationExpected<=0)
		{
		NextTest();
		iNumNotificationExpected=0;
		}
	}

void CViewTester::TestViewNotificationsL(CContactViewBase& /*aView*/)
	{
	//Test just add some contacts for now...
/*	CRandomContactGenerator* generator=CRandomContactGenerator::NewL();
	CleanupStack::PushL(generator);
	generator->SetDbL(iDb);
	for (TInt ii=0;ii<KInitialNumContacts;++ii)
		{
		generator->AddTypicalRandomContactL();
		}
	CleanupStack::PopAndDestroy(); // generator.
*/	}

void CViewTester::NextTest()
	{
	++iCurrentTest;
	TRequestStatus *pS=&iStatus;
	User::RequestComplete(pS,KErrNone);
	SetActive();
	}

void CViewTester::ExceriseViewL(CContactViewBase& aView)
	{
	TContactItemId lastId=0;
	const TInt numItems=aView.CountL();
	for (TInt ii=0;ii<numItems;++ii)
		{
		if (ii==numItems-1)
			{
			lastId=aView.AtL(ii);
			}
		iDb.ReadContactTextDefL(aView.AtL(ii),iScratchBuf,iTextDef);
		iLog.LogLineNoEcho(iScratchBuf);
		iScratchBuf.SetLength(0);
		}

	test(aView.FindL(lastId)==numItems-1);
	}

void CViewTester::DoCancel()
	{
	}

void doMainL()
	{
	__UHEAP_MARK;
	CTestConductor* testConductor = NULL;
	testConductor = CTestConductor::NewL();
	CleanupStack::PushL( testConductor );
	CleanupStack::PopAndDestroy( testConductor );
	__UHEAP_MARKEND;
	}
//
// Main.
//


/**

@SYMTestCaseID     PIM-T-FILTERVIEW-0001

*/

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	test.Start(_L("@SYMTESTCaseID:PIM-T-FILTERVIEW-0001 "));

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
	test.End();
	test.Close();
	__UHEAP_MARKEND;
	return KErrNone;
    }
