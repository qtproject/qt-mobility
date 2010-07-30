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
#include <collate.h>
#include <cntdb.h>
#include <cntitem.h>
#include <cntfldst.h>
#include "t_utils2.h"
#include "t_utils.h"
#include "t_viewsoom.h"
#include <badesca.h>
#include "ccontactdbeventqueue.h"

_LIT(KTestName,"T_ViewsOOM");
LOCAL_D RFs TheFs;
//
// Constants.
//

_LIT8(KSortPlugin, "application.vnd.symbian.com/contacts-reverse-sort"); //SimpleSortPlugin DLL Unique name

_LIT(KLogFileName,"t_viewsoom.log");

_LIT(KDbFileName,"c:t_view2.cdb");

_LIT(KTextDefSeparator,"");
_LIT8(KUnicodeLineEnd,"\x0D\x00\x0a\x00");

_LIT(KContactsFile, "z:\\t_view2\\t_view2_initial_contacts_small.txt");

const TInt KMaxNumRequests(1000);

//
// CTestConductor.
//

CTestConductor* CTestConductor::NewL()
	{
	CTestConductor* self=new(ELeave) CTestConductor();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CTestConductor::~CTestConductor()
	{
	delete iGroupOne;
	iLocalView->Close(*this);
	iRemoteView_1->Close(*this);
	iSortOrder_1.Close();
	iSortOrder_2.Close();

	delete iSortChecker;
	delete iViewCreator;
	delete iContactAdder;
	delete iContactRemover;
	delete iViewExerciser;
	delete iSortOrderChanger;
	delete iLog;
    delete iDb;
	iTest.Close();
    TRAP_IGNORE(CContactDatabase::DeleteDatabaseL(KDbFileName));
	iFs.Close();
	}

CTestConductor::CTestConductor() : CActive(EPriorityIdle),iTest(KTestName),iCurrentTestNumber(-1)
	{
	CActiveScheduler::Add(this);
	}

/**

@SYMTestCaseID     PIM-T-VIEWSOOM-0001

*/

void CTestConductor::ConstructL()
	{
	iGroupId=-1;
	iGnoreNotifications=EFalse;
	iSortOrder_1.AppendL(KUidContactFieldGivenName);
	iSortOrder_1.AppendL(KUidContactFieldFamilyName);
	iSortOrder_1.AppendL(KUidContactFieldCompanyName);

	iSortOrder_2.AppendL(KUidContactFieldFamilyName);
	iSortOrder_2.AppendL(KUidContactFieldGivenName);
	iSortOrder_2.AppendL(KUidContactFieldCompanyName);

	User::LeaveIfError(iFs.Connect());
	iLog=CLog::NewL(iTest,KLogFileName);
	iDb=CContactDatabase::ReplaceL(KDbFileName);

	iSortChecker=CSortChecker::NewL(*this);
	iViewCreator=CViewCreator::NewL(*this);
	iContactAdder=CContactAdder::NewL(*this);
	iContactRemover=CContactRemover::NewL(*this);
	iViewExerciser=CViewExerciser::NewL(*this);
	iSortOrderChanger=CSortOrderChanger::NewL(*this);

	iTestError = KErrNone;

	NextTest();
	iTest.Start(_L("@SYMTESTCaseID:PIM-T-VIEWSOOM-0001 --- Contact Views OOM tests---"));

	CActiveScheduler::Start();

	User::LeaveIfError(iTestError);
	iTest.End();
	}

void CTestConductor::NextTest()
	{
	++iCurrentTestNumber;
	TRequestStatus *pS=&iStatus;
	User::RequestComplete(pS,KErrNone);
	SetActive();
	}

void CTestConductor::RunL()
	{
	switch (iCurrentTestNumber)
		{
		case ECreateLocalView:
			iLog->LogLine(_L("=== Create local view"));
			iCurrentTestObject=iViewCreator;
			iLocalView=iViewCreator->CreateLocalViewL(iSortOrder_1);
			break;
		case EAddInitialContacts:
			{
			iLog->LogLine(_L("=== Add initial contacts"));
			iCurrentTestObject=iContactAdder;
//			iContactAdder->AddContactsL(_L("c:\\t_view2\\t_view2_initial_contacts.txt"),1);
			iContactAdder->AddContactsL(KContactsFile,1);
			}
			break;
		case EAddGroup:
			{
			//Create Test Group
			iGroupOne = STATIC_CAST(CContactGroup*,iDb->CreateContactGroupL(KGroupOneName));
			for (TInt ii=1;ii<=KNumContactsInGroupOne;++ii)
				{
				iDb->AddContactToGroupL(ii,iGroupOne->Id());
				}
			iGroupId = iGroupOne->Id();
			NextTest();
			}
			break;
		case ECreateRemoteView:
			iLog->LogLine(_L("=== Create remote view"));
			iCurrentTestObject=iViewCreator;
			iRemoteView_1=iViewCreator->CreateRemoteViewL(iSortOrder_1);
			break;
		case EDoOOMTests:
			{
			iGnoreNotifications=ETrue;
			COomTest *oomTest=new(ELeave) COomTest(iDb,iLocalView,iRemoteView_1,iGroupOne,*this);
			CleanupStack::PushL(oomTest);
			oomTest->TestsL();
			CleanupStack::PopAndDestroy();	// oomTest
			NextTest();
			}
			break;
		case ENumTests:
			{
			RThread thread;
			iTest(thread.RequestCount()==0);
			CActiveScheduler::Stop();
			}
			break;
		default:
			ASSERT(EFalse);
			break;
		}
	}


TInt CTestConductor::RunError(TInt aError)
	{
	// quick code to report the test step and leave error
	// allows test to exit neatly with message about where the failure occurred
	iTestError = aError;

	switch (iCurrentTestNumber)
		{
		case ECreateLocalView: iTest.Printf(_L("RunL left in step CreateLocalView (%i) with error %i"), iCurrentTestNumber, aError); break;
		case EAddInitialContacts: iTest.Printf(_L("RunL left in step AddInitialContacts (%i) with error %i"), iCurrentTestNumber, aError); break;
		case EAddGroup: iTest.Printf(_L("RunL left in step AddGroup (%i) with error %i"), iCurrentTestNumber, aError); break;
		case ECreateRemoteView: iTest.Printf(_L("RunL left in step CreateRemoteView (%i) with error %i"), iCurrentTestNumber, aError); break;
		case EDoOOMTests: iTest.Printf(_L("RunL left in step DoOOMTestL (%i) with error %i"), iCurrentTestNumber, aError); break;

		case ENumTests: iTest.Printf(_L("RunL left in step NumTests (%i) with error %i"), iCurrentTestNumber, aError); break;

		default: iTest.Printf(_L("RunL left in step %i with error %i"), iCurrentTestNumber, aError); break;
		}

	// stop test
	CActiveScheduler::Stop();
	return KErrNone;
	}


void CTestConductor::HandleContactViewEvent(const CContactViewBase& aView, const TContactViewEvent& aEvent)
	{
	if(iGnoreNotifications)
		{
		return;
		}

	if(aEvent.iContactId ==  iGroupId )
		{
		return;//ignore
		}

	TRAPD(err, iCurrentTestObject->HandleNotificationL(aView, aEvent) );
	iTest(err == KErrNone);
	}

void CTestConductor::DoCancel()
	{
	}


//
// CSortChecker.
//

CSortChecker* CSortChecker::NewL(CTestConductor& aConductor)
	{
	CSortChecker* self=new(ELeave) CSortChecker(aConductor);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self.
	return self;
	}

CSortChecker::CSortChecker(CTestConductor& aConductor) : iConductor(aConductor)
	{
	}

void CSortChecker::ConstructL()
	{
	iCollateMethod=new(ELeave) TCollationMethod;
	*iCollateMethod=*Mem::CollationMethodByIndex(0);
	iCollateMethod->iFlags|=TCollationMethod::EIgnoreNone;
	}

CSortChecker::~CSortChecker()
	{
	iSortedFirstLast.ResetAndDestroy();
	iSortedLastFirst.ResetAndDestroy();
	delete iCollateMethod;
	}

void CSortChecker::AddContactL(const TDesC& aFirstName,const TDesC& aLastName)
	{
	HBufC* buf=HBufC::NewLC(aFirstName.Length()+aLastName.Length());
	TPtr bufPtr(buf->Des());
	bufPtr.Append(aFirstName);
	bufPtr.Append(aLastName);
	User::LeaveIfError(iSortedFirstLast.Append(buf));
	CleanupStack::Pop(); // buf.

	buf=HBufC::NewLC(aFirstName.Length()+aLastName.Length());
	TPtr bufPtr2(buf->Des());
	bufPtr2.Append(aLastName);
	bufPtr2.Append(aFirstName);
	User::LeaveIfError(iSortedLastFirst.Append(buf));
	CleanupStack::Pop(); // buf.
	}

void CSortChecker::RemoveContactL(const TDesC& aFirstName,const TDesC& aLastName)
	{
	HBufC* buf=HBufC::NewLC(aFirstName.Length()+aLastName.Length());
	TPtr bufPtr(buf->Des());
	bufPtr.Append(aFirstName);
	bufPtr.Append(aLastName);
	TInt index=iSortedFirstLast.Find(buf,TIdentityRelation<HBufC>(Match));
	ASSERT(index!=KErrNotFound);
	delete iSortedFirstLast[index];
	iSortedFirstLast.Remove(index);
	CleanupStack::PopAndDestroy(); // buf.

	buf=HBufC::NewLC(aFirstName.Length()+aLastName.Length());
	TPtr bufPtr2(buf->Des());
	bufPtr2.Append(aLastName);
	bufPtr2.Append(aFirstName);
	index=iSortedLastFirst.Find(buf,TIdentityRelation<HBufC>(Match));
	ASSERT(index!=KErrNotFound);
	delete iSortedLastFirst[index];
	iSortedLastFirst.Remove(index);
	CleanupStack::PopAndDestroy(); // buf.
	}

TBool CSortChecker::Match(const HBufC& aFirst,const HBufC& aSecond)
	{
	if (aFirst.Compare(aSecond)==0)
		{
		return ETrue;
		}
	return EFalse;
	}

TPtrC CSortChecker::ContactAt(TSortOrder aOrder,TInt aIndex) const
	{
	const RPointerArray<HBufC>& sortedList=(aOrder==ELastNameFirstName) ? iSortedLastFirst : iSortedFirstLast;
	return TPtrC(*sortedList[aIndex]);
	}

TInt CSortChecker::Count() const
	{
	return iSortedFirstLast.Count();
	}

TInt CSortChecker::Count(TSortOrder aOrder,const TDesC& aLowBoundary,const TDesC& aHighBoundary) const
	{
	const TInt low=FindIndex(aOrder,aLowBoundary,CSortChecker::EGreaterThanOrEqualTo);
	const TInt high=FindIndex(aOrder,aHighBoundary,CSortChecker::EGreaterThanOrEqualTo);
	if (low==KErrNotFound)
		{
		iConductor.iTest(high==KErrNotFound);
		return 0;
		}
	return high-low;
	}

TInt CSortChecker::CountLow(TSortOrder aOrder,const TDesC& aHighBoundary) const
	{
	const TInt high=FindIndex(aOrder,aHighBoundary,CSortChecker::EGreaterThanOrEqualTo);
	if (high==KErrNotFound)
		{
		return 0;
		}
	return high;
	}

TInt CSortChecker::CountHigh(TSortOrder aOrder,const TDesC& aLowBoundary) const
	{
	const TInt low=FindIndex(aOrder,aLowBoundary,CSortChecker::EGreaterThanOrEqualTo);
	if (low==KErrNotFound)
		{
		return 0;
		}
	return Count()-low;
	}

void CSortChecker::Sort()
	{
	iSortedFirstLast.Sort(TLinearOrder<HBufC>(Compare));
	iSortedLastFirst.Sort(TLinearOrder<HBufC>(Compare));

	const TInt numContacts=iSortedFirstLast.Count();
	__ASSERT_ALWAYS(numContacts==iSortedLastFirst.Count(),User::Invariant());
	}

void CSortChecker::CheckSort(TSortOrder aOrder,const TDesC& aName,TInt aIndex) const
	{
	const RPointerArray<HBufC>& sortedList=(aOrder==ELastNameFirstName) ? iSortedLastFirst : iSortedFirstLast;
	iConductor.iLog->LogLineNoEcho(_L("Checking - %S against %S"),&aName,sortedList[aIndex]);
	iConductor.iTest(sortedList[aIndex]->Compare(aName)==0);
	}

void CSortChecker::CheckSortLowSubView(TSortOrder aOrder,const TDesC& aName,TInt aIndex,const TDesC& aHighBoundary) const
	{
	const RPointerArray<HBufC>& sortedList=(aOrder==ELastNameFirstName) ? iSortedLastFirst : iSortedFirstLast;
	const HBufC* name=sortedList[aIndex];
	iConductor.iLog->LogLineNoEcho(_L("Checking - %S against %S"),&aName,name);
	iConductor.iTest(name->CompareC(aHighBoundary,3,iCollateMethod)<0);
	iConductor.iTest(name->Compare(aName)==0);
	}

void CSortChecker::CheckSortHighSubView(TSortOrder aOrder,const TDesC& aName,TInt aIndex,const TDesC& aLowBoundary) const
	{
	const RPointerArray<HBufC>& sortedList=(aOrder==ELastNameFirstName) ? iSortedLastFirst : iSortedFirstLast;
	const TInt lowIndex=FindIndex(aOrder,aLowBoundary,CSortChecker::EGreaterThanOrEqualTo);
	if (lowIndex!=KErrNotFound)
		{
		iConductor.iLog->LogLineNoEcho(_L("Checking - %S against %S"),&aName,sortedList[lowIndex+aIndex]);
		iConductor.iTest(sortedList[lowIndex+aIndex]->Compare(aName)==0);
		}
	}

void CSortChecker::CheckSortSubView(TSortOrder aOrder,const TDesC& aName,TInt aIndex,const TDesC& aLowBoundary,const TDesC& aHighBoundary) const
	{
	const RPointerArray<HBufC>& sortedList=(aOrder==ELastNameFirstName) ? iSortedLastFirst : iSortedFirstLast;
	const TInt lowIndex=FindIndex(aOrder,aLowBoundary,CSortChecker::EGreaterThanOrEqualTo);
	if (lowIndex!=KErrNotFound)
		{
		const HBufC* name=sortedList[lowIndex+aIndex];
		iConductor.iLog->LogLineNoEcho(_L("Checking - %S against %S"),&aName,name);
		iConductor.iTest(name->CompareC(aHighBoundary,3,iCollateMethod)<0);
		iConductor.iTest(name->Compare(aName)==0);
		}
	}

TInt CSortChecker::FindIndex(TSortOrder aOrder,const TDesC& aBoundary,TCriteria aCriteria) const
	{
	const RPointerArray<HBufC>& sortedList=(aOrder==ELastNameFirstName) ? iSortedLastFirst : iSortedFirstLast;
	const TInt numContacts=sortedList.Count();
	for (TInt ii=0;ii<numContacts;++ii)
		{
		switch (aCriteria)
			{
			case ELessThan:
				if (sortedList[ii]->CompareC(aBoundary,3,iCollateMethod)<0)
					{
					return ii;
					}
				break;
			case ELessThanOrEqualTo:
				if (sortedList[ii]->CompareC(aBoundary,3,iCollateMethod)<=0)
					{
					return ii;
					}
				break;
			case EGreaterThan:
				if (sortedList[ii]->CompareC(aBoundary,3,iCollateMethod)>0)
					{
					return ii;
					}
				break;
			case EGreaterThanOrEqualTo:
				if (sortedList[ii]->CompareC(aBoundary,3,iCollateMethod)>=0)
					{
					return ii;
					}
				break;
			default:
				ASSERT(EFalse);
			}
		}
	return KErrNotFound;
	}

TInt CSortChecker::Compare(const HBufC& aFirst, const HBufC& aSecond)
	{
	TCollationMethod collateMethod;
	collateMethod = *Mem::CollationMethodByIndex(0);
	collateMethod.iFlags|=TCollationMethod::EIgnoreNone;
	return aFirst.CompareC(aSecond,3,&collateMethod);
	}


//
// CTestBase.
//

CTestBase::CTestBase(CTestConductor& aConductor) : iConductor(aConductor)
	{
	}


//
// CViewCreator.
//

CViewCreator* CViewCreator::NewL(CTestConductor& aConductor)
	{
	CViewCreator* self=new(ELeave) CViewCreator(aConductor);
	return self;
	}

CContactLocalView* CViewCreator::CreateLocalViewL(const RContactViewSortOrder& aSortOrder)
	{
	TInt32 viewPrefs=0;
	viewPrefs|=EContactAndGroups;
	iView=CContactLocalView::NewL(iConductor,*iConductor.iDb,aSortOrder,TContactViewPreferences(viewPrefs));
	return STATIC_CAST(CContactLocalView*,iView);
	}

CContactNamedLocalView* CViewCreator::CreateNamedLocalViewL(const TDesC& aName,const RContactViewSortOrder& aSortOrder)
	{
	TInt32 viewPrefs=0;
	viewPrefs|=EContactAndGroups;
	iView=CContactNamedLocalView::NewL(iConductor,aName,*iConductor.iDb,aSortOrder,TContactViewPreferences(viewPrefs));
	return STATIC_CAST(CContactNamedLocalView*,iView);
	}

CContactRemoteView* CViewCreator::CreateRemoteViewL(const RContactViewSortOrder& aSortOrder)
	{
	TInt32 viewPrefs=0;
	viewPrefs|=EContactAndGroups;
	iView=CContactRemoteView::NewL(iConductor,*iConductor.iDb,aSortOrder,TContactViewPreferences(viewPrefs));
	return STATIC_CAST(CContactRemoteView*,iView);
	}

CContactNamedRemoteView* CViewCreator::CreateNamedRemoteViewL(const TDesC& aName,const RContactViewSortOrder& aSortOrder)
	{
	TInt32 viewPrefs=0;
	viewPrefs|=EContactAndGroups;
	iView=CContactNamedRemoteView::NewL(iConductor,aName,*iConductor.iDb,aSortOrder,TContactViewPreferences(viewPrefs));
	return STATIC_CAST(CContactNamedRemoteView*,iView);
	}

CContactSubView* CViewCreator::CreateSubViewL(CContactViewBase& aUnderlyingView,const TDesC& aBoundary)
	{
	iView=CContactSubView::NewL(iConductor,*iConductor.iDb,aUnderlyingView,aBoundary);
	return STATIC_CAST(CContactSubView*,iView);
	}

CContactSubView* CViewCreator::CreateSubViewL(CContactViewBase& aUnderlyingView,const TDesC& aLowBoundary,const TDesC& aHighBoundary)
	{
	iView=CContactSubView::NewL(iConductor,*iConductor.iDb,aUnderlyingView,aLowBoundary,aHighBoundary);
	return STATIC_CAST(CContactSubView*,iView);
	}

CContactConcatenatedView* CViewCreator::CreateConcatenatedViewL(RPointerArray<CContactViewBase>& aComponentViews)
	{
	iView=CContactConcatenatedView::NewL(iConductor,*iConductor.iDb,aComponentViews);
	return STATIC_CAST(CContactConcatenatedView*,iView);
	}

void CViewCreator::HandleNotificationL(const CContactViewBase& aView,const TContactViewEvent& aEvent)
	{
 	if(&aView==iView)
	 		{
	 		iConductor.iTest(aEvent.iEventType==TContactViewEvent::EReady);
	 		iConductor.NextTest();
	 		}
	}

CViewCreator::CViewCreator(CTestConductor& aConductor) : CTestBase(aConductor)
	{
	}


//
// CContactAdder.
//

CContactAdder* CContactAdder::NewL(CTestConductor& aConductor)
	{
	CContactAdder* self=new(ELeave) CContactAdder(aConductor);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self.
	return self;
	}

CContactAdder::~CContactAdder()
	{
	delete iRandomGenerator;
	delete iTemplate;
	iFile.Close();
	iFs.Close();
	}

void CContactAdder::AddContactsL(const TDesC& aFileName,TInt aNumViews)
	{
	User::LeaveIfError(iFile.Open(iFs,aFileName,EFileShareAny|EFileStreamText));
	iNumViews=aNumViews;
	TInt pos=2;
	iFile.Seek(ESeekStart,pos); // Ignore Unicode header.
	iConductor.iTest.Printf(_L("Adding contact "));
	AddOneContactL();
	}

void CContactAdder::HandleNotificationL(const CContactViewBase&,const TContactViewEvent& aEvent)
	{
	if (++iNumNotifications==iNumViews)
		{
		if (iFinished)
			{
			iConductor.NextTest();
			}
		else
			{
			AddOneContactL();
			}
		}
	else
		{
		iConductor.iTest(aEvent.iEventType==TContactViewEvent::EItemAdded);
		}
	}

CContactAdder::CContactAdder(CTestConductor& aConductor) : CTestBase(aConductor)
	{
	}

void CContactAdder::ConstructL()
	{
	User::LeaveIfError(iFs.Connect());
	iTemplate=STATIC_CAST(CContactTemplate*,iConductor.iDb->ReadContactL(iConductor.iDb->TemplateId()));
	iRandomGenerator=CRandomContactGenerator::NewL();
	iRandomGenerator->SetDbL(*iConductor.iDb);
	}

void CContactAdder::AddOneContactL()
	{
	iRandomGenerator->AddTypicalRandomContactL();
	iConductor.iLog->IncVisualCounter();
	iNumNotifications=0;
	TBuf<64> firstName;
	TBuf<64> lastName;
	iConductor.iLog->IncVisualCounter();
	iFinished=!ReadNamePairL(iFile,firstName,lastName);
	iConductor.iLog->LogLineNoEcho(_L("Adding %S %S"),&firstName,&lastName);
	CTestContact* contact=CTestContact::NewLC(*iTemplate);
	contact->SetFirstNameL(firstName);
	contact->SetLastNameL(lastName);
	iConductor.iDb->AddNewContactL(contact->ContactItem());
	CleanupStack::PopAndDestroy(); // contact.
	iConductor.iDb->CompactL();
	iConductor.iSortChecker->AddContactL(firstName,lastName);
	if (iFinished)
		{
		iConductor.iLog->EndVisualCounter();
		iConductor.iSortChecker->Sort();
		}
	}

TBool CContactAdder::ReadNamePairL(RFile aFile,TDes& aFirstName,TDes& aLastName)
	{
	TBuf8<2> character=_L8("  ");
	TBuf8<4> lineEndTestBuf=_L8("    ");

	aFirstName.SetLength(0);
	aLastName.SetLength(0);
	TBool readingLastName=ETrue;
	while (lineEndTestBuf!=KUnicodeLineEnd)
		{
		User::LeaveIfError(aFile.Read(character,2));
		if (character.Length()>0)
			{
			lineEndTestBuf[0]=lineEndTestBuf[2];
			lineEndTestBuf[1]=lineEndTestBuf[3];
			lineEndTestBuf[2]=character[0];
			lineEndTestBuf[3]=character[1];

			const TPtrC16 widePtr((TUint16*)character.Ptr(),1);
			if (widePtr[0]==';')
				{
				readingLastName=EFalse;
				}
			else if (readingLastName)
				{
				aLastName.Append(widePtr);
				}
			else
				{
				aFirstName.Append(widePtr);
				}
			}
		else
			{
			return EFalse;
			}
		}

	aFirstName.Delete(aFirstName.Length()-2,2); // Strip off \n\r.
	return ETrue;
	}


//
// CContactRemover.
//

CContactRemover* CContactRemover::NewL(CTestConductor& aConductor)
	{
	CContactRemover* self=new(ELeave) CContactRemover(aConductor);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self.
	return self;
	}

CContactRemover::~CContactRemover()
	{
	iFile.Close();
	iFs.Close();
	}

void CContactRemover::RemoveContactsL(const TDesC& aFileName,TInt aNumViews)
	{
	User::LeaveIfError(iFile.Open(iFs,aFileName,EFileShareAny|EFileStreamText));
	iNumViews=aNumViews;
	TInt pos=2;
	iFile.Seek(ESeekStart,pos); // Ignore Unicode header.
	iConductor.iTest.Printf(_L("Removing contact "));
	RemoveOneContactL();
	}

void CContactRemover::HandleNotificationL(const CContactViewBase&,const TContactViewEvent& aEvent)
	{
	if (++iNumNotifications==iNumViews)
		{
		if (iFinished)
			{
			iConductor.NextTest();
			}
		else
			{
			RemoveOneContactL();
			}
		}
	else
		{
		iConductor.iTest(aEvent.iEventType==TContactViewEvent::EItemRemoved);
		}
	}

CContactRemover::CContactRemover(CTestConductor& aConductor) : CTestBase(aConductor)
	{
	}

void CContactRemover::ConstructL()
	{
	User::LeaveIfError(iFs.Connect());
	}

void CContactRemover::RemoveOneContactL()
	{
	iNumNotifications=0;
	TBuf<64> firstName;
	TBuf<64> lastName;
	iConductor.iLog->IncVisualCounter();
	iFinished=!CContactAdder::ReadNamePairL(iFile,firstName,lastName);
	iConductor.iLog->LogLineNoEcho(_L("Removing %S %S"),&firstName,&lastName);
	CContactItemFieldDef* def=new(ELeave) CContactItemFieldDef();
	CleanupStack::PushL(def);
	def->AppendL(KUidContactFieldGivenName);
	def->AppendL(KUidContactFieldFamilyName);
	CContactIdArray* matchList1=iConductor.iDb->FindLC(lastName,def);
	CContactIdArray* matchList2=iConductor.iDb->FindLC(firstName,def);

	for (TInt ii=matchList1->Count()-1;ii>=0;--ii)
		{
		TContactItemId thisId=(*matchList1)[ii];
		for (TInt jj=matchList2->Count()-1;jj>=0;--jj)
			{
			if (thisId==(*matchList2)[jj])
				{
				iConductor.iDb->DeleteContactL(thisId);
				goto carry_on;
				}
			}
		}
	ASSERT(EFalse);

carry_on:
	CleanupStack::PopAndDestroy(3); // matchList2, matchList1, def.
	iConductor.iDb->CompactL();
	iConductor.iSortChecker->RemoveContactL(firstName,lastName);
	if (iFinished)
		{
		iConductor.iLog->EndVisualCounter();
		iConductor.iSortChecker->Sort();
		}
	}


//
// CViewExerciser.
//

CViewExerciser* CViewExerciser::NewL(CTestConductor& aConductor)
	{
	CViewExerciser* self=new(ELeave) CViewExerciser(aConductor);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self.
	return self;
	}

/*void CViewExerciser::ProfileViewL(CContactViewBase& aView)
	{
	TInt viewCount = aView.CountL();
	RDebug::Print(_L("Profile view Count: %d"),viewCount);
	CCntTest::ProfileReset(0,1);
	CCntTest::ProfileStart(0);
	for (TInt ii=0;ii<viewCount;++ii)
		{
		aView.ContactAtL(ii);
		}
	CCntTest::ProfileEnd(0);
	TCntProfile profile[1];
	CCntTest::ProfileResult(profile,0,1);
	TReal result = profile[0].iTime;///1000000.0;
	RDebug::Print(_L("Profile ContactAt Time Taken: %g"),result);
	}*/

void CViewExerciser::ExerciseViewL(CSortChecker::TSortOrder aSortOrder,CContactViewBase& aView)
	{
	TBuf<128> scratchBuf;
	TContactItemId lastId=0;
	const TInt numItems=aView.CountL();
	iConductor.iTest(iConductor.iSortChecker->Count()==numItems);
	CContactTextDef* textDef=(aSortOrder==CSortChecker::EFirstNameLastName) ? iTextDef_FirstLast : iTextDef_LastFirst;

	for (TInt ii=0;ii<numItems;++ii)
		{
		TContactItemId thisId=aView.AtL(ii);
		if (ii==numItems-1)
			{
			lastId=thisId;
			}

		iConductor.iDb->ReadContactTextDefL(thisId,scratchBuf,textDef);
		iConductor.iSortChecker->CheckSort(aSortOrder,scratchBuf,ii);
		scratchBuf.SetLength(0);
		}

	iConductor.iTest(aView.FindL(lastId)==numItems-1);

	if (iQueue)
		{
		iConductor.NextTest();
		}
	}

void CViewExerciser::ExerciseSubViewL(CSortChecker::TSortOrder aSortOrder,CContactViewBase& aView,const TDesC& aLowBoundary,const TDesC& aHighBoundary)
	{
	TBuf<128> scratchBuf;
	TContactItemId lastId=0;
	const TInt numItems=aView.CountL();
	iConductor.iTest(iConductor.iSortChecker->Count(aSortOrder,aLowBoundary,aHighBoundary)==numItems);

	for (TInt ii=0;ii<numItems;++ii)
		{
		TContactItemId thisId=aView.AtL(ii);
		if (ii==numItems-1)
			{
			lastId=thisId;
			}
		CContactTextDef* textDef=(aSortOrder==CSortChecker::EFirstNameLastName) ? iTextDef_FirstLast : iTextDef_LastFirst;
		iConductor.iDb->ReadContactTextDefL(thisId,scratchBuf,textDef);
		iConductor.iSortChecker->CheckSortSubView(aSortOrder,scratchBuf,ii,aLowBoundary,aHighBoundary);
		scratchBuf.SetLength(0);
		}

	iConductor.iTest(aView.FindL(lastId)==numItems-1);

	if (iQueue)
		{
		iConductor.NextTest();
		}
	}

void CViewExerciser::ExerciseLowSubViewL(CSortChecker::TSortOrder aSortOrder,CContactViewBase& aView,const TDesC& aLowBoundary)
	{
	TBuf<128> scratchBuf;
	TContactItemId lastId=0;
	const TInt numItems=aView.CountL();
	iConductor.iTest(iConductor.iSortChecker->CountLow(aSortOrder,aLowBoundary)==numItems);

	for (TInt ii=0;ii<numItems;++ii)
		{
		TContactItemId thisId=aView.AtL(ii);
		if (ii==numItems-1)
			{
			lastId=thisId;
			}
		CContactTextDef* textDef=(aSortOrder==CSortChecker::EFirstNameLastName) ? iTextDef_FirstLast : iTextDef_LastFirst;
		iConductor.iDb->ReadContactTextDefL(thisId,scratchBuf,textDef);
		iConductor.iSortChecker->CheckSortLowSubView(aSortOrder,scratchBuf,ii,aLowBoundary);
		scratchBuf.SetLength(0);
		}

	iConductor.iTest(aView.FindL(lastId)==numItems-1);

	if (iQueue)
		{
		iConductor.NextTest();
		}
	}

void CViewExerciser::ExerciseHighSubViewL(CSortChecker::TSortOrder aSortOrder,CContactViewBase& aView,const TDesC& aHighBoundary)
	{
	TBuf<128> scratchBuf;
	TContactItemId lastId=0;
	const TInt numItems=aView.CountL();
	iConductor.iTest(iConductor.iSortChecker->CountHigh(aSortOrder,aHighBoundary)==numItems);

	for (TInt ii=0;ii<numItems;++ii)
		{
		TContactItemId thisId=aView.AtL(ii);
		if (ii==numItems-1)
			{
			lastId=thisId;
			}
		CContactTextDef* textDef=(aSortOrder==CSortChecker::EFirstNameLastName) ? iTextDef_FirstLast : iTextDef_LastFirst;
		iConductor.iDb->ReadContactTextDefL(thisId,scratchBuf,textDef);
		iConductor.iSortChecker->CheckSortHighSubView(aSortOrder,scratchBuf,ii,aHighBoundary);
		scratchBuf.SetLength(0);
		}

	iConductor.iTest(aView.FindL(lastId)==numItems-1);

	if (iQueue)
		{
		iConductor.NextTest();
		}
	}

CViewExerciser::~CViewExerciser()
	{
	delete iTextDef_FirstLast;
	delete iTextDef_LastFirst;
	}

void CViewExerciser::HandleNotificationL(const CContactViewBase&,const TContactViewEvent&)
	{
	iConductor.iTest(EFalse);
	}

CViewExerciser::CViewExerciser(CTestConductor& aConductor) : CTestBase(aConductor),iQueue(ETrue)
	{
	}

void CViewExerciser::ConstructL()
	{
	iTextDef_FirstLast=CContactTextDef::NewL();
	iTextDef_FirstLast->AppendL(TContactTextDefItem(KUidContactFieldGivenName,KTextDefSeparator));
	iTextDef_FirstLast->AppendL(TContactTextDefItem(KUidContactFieldFamilyName,KTextDefSeparator));
	iTextDef_FirstLast->AppendL(TContactTextDefItem(KUidContactFieldCompanyName,KTextDefSeparator));

	iTextDef_LastFirst=CContactTextDef::NewL();
	iTextDef_LastFirst->AppendL(TContactTextDefItem(KUidContactFieldFamilyName,KTextDefSeparator));
	iTextDef_LastFirst->AppendL(TContactTextDefItem(KUidContactFieldGivenName,KTextDefSeparator));
	iTextDef_LastFirst->AppendL(TContactTextDefItem(KUidContactFieldCompanyName,KTextDefSeparator));
	}

void CViewExerciser::QueueNextTestWhenComplete(TBool aQueue)
	{
	iQueue=aQueue;
	}


//
// CSortOrderChanger.
//

CSortOrderChanger* CSortOrderChanger::NewL(CTestConductor& aConductor)
	{
	CSortOrderChanger* self=new(ELeave) CSortOrderChanger(aConductor);
	return self;
	}

void CSortOrderChanger::ChangeSortOrderL(RContactViewSortOrder& aSortOrder,CContactNamedLocalView& aView,TInt aNumViews)
	{
	aView.ChangeSortOrderL(aSortOrder);
	iNumViews=aNumViews;
	iNumNotifications=0;
	}

void CSortOrderChanger::ChangeSortOrderL(RContactViewSortOrder& aSortOrder,CContactNamedRemoteView& aView,TInt aNumViews)
	{
	aView.ChangeSortOrderL(aSortOrder);
	iNumViews=aNumViews;
	iNumNotifications=0;
	}

CSortOrderChanger::~CSortOrderChanger()
	{
	}

void CSortOrderChanger::HandleNotificationL(const CContactViewBase&,const TContactViewEvent& aEvent)
	{
	iConductor.iTest(aEvent.iEventType==TContactViewEvent::EUnavailable || aEvent.iEventType==TContactViewEvent::ESortOrderChanged || aEvent.iEventType==TContactViewEvent::EReady);
//	if (++iNumNotifications==iNumViews*3) // 3 because there will be EUnavailable, ESortOrderChanged, and EReady notifications for each view.
	if (++iNumNotifications==iNumViews*2) // 3 because there will be EUnavailable, ESortOrderChanged, and EReady notifications for each view.		
		{
		iConductor.NextTest();
		}
	}

CSortOrderChanger::CSortOrderChanger(CTestConductor& aConductor) : CTestBase(aConductor)
	{
	}



///////////////////////////////////////////////////////////////////	
//
// COomFailBase
//

COomFailBase::COomFailBase(COomTest *aOomTest,CContactDatabase* aDb,CContactViewBase* aUnderlyingView)
 : iOomTest(aOomTest),iDb(aDb),iUnderlyingView(aUnderlyingView)
	{}

COomFailBase::~COomFailBase()
	{
	}

void COomFailBase::ConstructL()
	{}

void COomFailBase::PreFailL()
	{}

void COomFailBase::ClearUp()
	{}

TBool COomFailBase::Skip(TInt , TOomFailureModes ) const
	{
	return(EFalse);
	}

void COomFailBase::ClearEventQueueL()
	{
	_LIT(KLoggingFormat, "(%d) Database Event Consumed. Type: %d\n");
	
	CContactDbEventQueue* eventQueue = CContactDbEventQueue::NewL(iDb, KMaxNumRequests);
	CleanupStack::PushL(eventQueue);
	
	// Clear the view of all outstanding events before continuing
	TContactDbObserverEvent  event;
	TBool expectingEvent = ETrue;
	const TInt KTimeOutInSeconds = 1;
	for(TInt i = 0; expectingEvent; ++i)
		{
		expectingEvent = eventQueue->ListenForEvent(KTimeOutInSeconds, event);
		if (expectingEvent) // meaning if we've just received one
			{
			iOomTest->Conductor().iTest.Printf(KLoggingFormat, i, event.iType);
			}
		}
	
	CleanupStack::PopAndDestroy(eventQueue);
	}

//
// COomTest
//

COomTest::COomTest(CContactDatabase* aDb,CContactLocalView* aLocalView,CContactRemoteView* aRemoteView,CContactGroup* aGroup,CTestConductor& aConductor)
 : iDb (aDb),iLocalView(aLocalView),iRemoteView (aRemoteView),iGroupOne(aGroup),iConductor(aConductor)
	{}
	
void COomTest::Test(TBool aMustBeTrue)
	{
	iConductor.iTest(aMustBeTrue);
	}
	
CTestConductor& COomTest::Conductor() const
	{
	return iConductor;
	}

void COomTest::DoOomTestL(COomFailBase *aOomTest, TOomFailureModes aMode)
	{
	Test(aOomTest!=NULL);
	iConductor.iTest.Next(_L(""));


	__UHEAP_MARK;
	TInt oldFsCount=TheFs.ResourceCount();
//
	CleanupStack::PushL(aOomTest);
	TRAPD(ret1,aOomTest->ConstructL());
	Test(ret1==KErrNone);
	CContactDatabase *lockDb=NULL;
	if (aMode==KOomFailContactServer)	// contact server failure tests
		{
		lockDb=CContactDatabase::OpenL(KDbFileName);
		CleanupStack::PushL(lockDb);
		}
	TInt successCount=0;
	TInt skipCount=0;
	for(TInt count=0;;count++)
		{//
		TRAPD(preFailErr,aOomTest->PreFailL());
		iConductor.iTest(preFailErr==KErrNone);
		switch(aMode)
			{
			case KOomFailNormal:
				__UHEAP_SETFAIL(RHeap::EDeterministic,count);
				break;
			case KOomFailContactServer:
				lockDb->SetCntServerHeapFailure(RHeap::EDeterministic,count);
				break;
			case KOomFailFileServerOom:
				TheFs.SetErrorCondition(KErrNoMemory,count);
				break;
			case KOomFailFileServerGeneral:
				TheFs.SetErrorCondition(KErrGeneral,count);
				break;
			default:;
			}
		TInt ret=KErrNone;
		if (aOomTest->Skip(count,aMode))
			skipCount++;
		else
			{
			__UHEAP_MARK;
			TRAP(ret,aOomTest->FailL());
			__UHEAP_MARKEND;
			}
		switch(aMode)
			{
			case KOomFailNormal:
				__UHEAP_RESET;
				break;
			case KOomFailContactServer:
				lockDb->SetCntServerHeapFailure(RHeap::ENone,0);
				break;
			case KOomFailFileServerOom:
			case KOomFailFileServerGeneral:
				TheFs.SetErrorCondition(KErrNone);
				break;
			default:;
			}
		TRAPD(clearUpErr,aOomTest->ClearUp());
		iConductor.iTest(clearUpErr==KErrNone);
		if (ret==KErrNone)
			{
			if (successCount==8)
				{
				iConductor.iTest.Printf(_L("Count=%d"),count);
				if (skipCount>0)
					iConductor.iTest.Printf(_L(", skipped=%d"),skipCount);
				iConductor.iTest.Printf(_L("\n"));
				break;
				}
			successCount++;
			}
		else
			{
			successCount=0;
			if (aMode==KOomFailFileServerGeneral)
				Test(ret==KErrGeneral || ret==KErrNotFound);
			else if (aMode==KOomFailFileServerOom)
				Test(ret==KErrNoMemory || ret==KErrNotFound);
			else
				Test(ret==KErrNoMemory);
			}
		}//

	if (aMode==KOomFailContactServer)
		CleanupStack::PopAndDestroy();	// lockDb
	CleanupStack::PopAndDestroy();	// aOomTest
	Test(oldFsCount==TheFs.ResourceCount());
	__UHEAP_MARKEND;

	}

COomTest::~COomTest()
	{}

TBool COomTest::TestsL()
	{
    //__UHEAP_MARK;
	for(TInt loop=0;loop<KOomNumFailModes;loop++)
		{
		if (loop > 0)
			{ // increment the RTest number XXX
			iConductor.iTest.Next(_L("--- Next Error case ---"));

			}

		TOomFailureModes mode=(TOomFailureModes)loop;
		switch(mode)
			{
			case KOomFailNormal: // RTest 001.01
				iConductor.iTest.Start(_L("===Test Out Of Memory==="));

				break;
			case KOomFailFileServerGeneral: // RTest 002.01
				iConductor.iTest.Start(_L("===Test File server general failure==="));

				break;
			case KOomFailFileServerOom: // RTest 003.01
				iConductor.iTest.Start(_L("===Test File server Oom failure==="));

				break;
			case KOomFailContactServer: // RTest 004.01
				iConductor.iTest.Start(_L("===Test Contact Server Oom failure==="));

				break;
			default:;
			}
		User::LeaveIfError(TheFs.Connect());

		//OOM TEST VIEWS/////////////
		DoOomTestL(new(ELeave) COomLocalView(this, iDb, NULL), mode); //Pass
		DoOomTestL(new(ELeave) COomLocalView(this, iDb, NULL, KSortPlugin), mode); //Pass
		DoOomTestL(new(ELeave) COomRemoteView(this, iDb, NULL), mode); //Pass
		DoOomTestL(new(ELeave) COomRemoteView(this, iDb, NULL, KSortPlugin), mode); //Pass
		
		if(iLocalView)
		    {
		    for(TInt ii = 0; ii < iLocalView->CountL(); ++ii)
		        {
		        const CViewContact& temp = iLocalView->ContactAtL(ii);
		        }
		    }
		
		DoOomTestL(new(ELeave) COomFilteredView(this, iDb, iLocalView), mode);//Pass
		DoOomTestL(new(ELeave) COomSubView(this, iDb, iLocalView), mode); //Pass
		DoOomTestL(new(ELeave) COomSubView(this, iDb, iRemoteView), mode); //Pass
		DoOomTestL(new(ELeave) COomGroupView(this, iDb, iLocalView, iGroupOne), mode); //Pass
		DoOomTestL(new(ELeave) COomMatchingCriteria(this, iDb, iLocalView), mode);
		DoOomTestL(new(ELeave) COomMatchingCriteria(this, iDb, iRemoteView), mode);
		//////////////////////////////
		iConductor.iTest(TheFs.ResourceCount()==0);
		TheFs.Close();
		iConductor.iTest.End();
		}
    //__UHEAP_MARKEND;
	return(ETrue);
	}


//
// COomLocalView
//

void COomLocalView::HandleContactViewEvent(const CContactViewBase& aView, const TContactViewEvent& /*aEvent*/)
	{
	if(&aView == iLocalView)
		{
		CActiveScheduler::Stop();
		}
	}


void COomLocalView::PreFailL()
	{
	iSortOrder.AppendL(KUidContactFieldGivenName);
	iSortOrder.AppendL(KUidContactFieldFamilyName);
	iSortOrder.AppendL(KUidContactFieldCompanyName);
	}

void COomLocalView::FailL()
	{
	TInt32 viewPrefs=0; 
	viewPrefs|=EIgnoreUnSorted;
	if(iSortPluginName.Length() == 0)
		{
		iLocalView=CContactLocalView::NewL(*this,*iDb,iSortOrder,TContactViewPreferences(viewPrefs));
		}
	else
		{
		iLocalView=CContactLocalView::NewL(*this,*iDb,iSortOrder,TContactViewPreferences(viewPrefs),iSortPluginName);
		}
	CActiveScheduler::Start();
	iLocalView->Close(*this);
	ClearEventQueueL();
	}

void COomLocalView::ClearUp()
	{
	iSortOrder.Close();
	}



//
// COomRemoteView
//

void COomRemoteView::HandleContactViewEvent(const CContactViewBase& /*aView*/,const TContactViewEvent& /*aEvent*/)
	{
	CActiveScheduler::Stop();
	}	

void COomRemoteView::PreFailL()
	{
	iSortOrder.AppendL(KUidContactFieldGivenName);
	iSortOrder.AppendL(KUidContactFieldFamilyName);
	iSortOrder.AppendL(KUidContactFieldCompanyName);
	}

void COomRemoteView::FailL()
	{
	TInt32 viewPrefs=0;
	viewPrefs|=EIgnoreUnSorted;
	if(iSortPluginName.Length() == 0)
		{
		iRemoteView=CContactRemoteView::NewL(*this,*iDb,iSortOrder,TContactViewPreferences(viewPrefs));
		}
	else
		{
		iRemoteView=CContactRemoteView::NewL(*this,*iDb,iSortOrder,TContactViewPreferences(viewPrefs),iSortPluginName);
		}
	CActiveScheduler::Start();
	iOomTest->Conductor().iTest(iRemoteView->Close(*this));
	ClearEventQueueL();
	}

void COomRemoteView::ClearUp()
	{
	iSortOrder.Close();
	}

//
// COomSubView
//
void COomSubView::HandleContactViewEvent(const CContactViewBase& /*aView*/, const TContactViewEvent& /*aEvent*/)
	{	
	CActiveScheduler::Stop();
	}	

void COomSubView::PreFailL()
	{
	}

void COomSubView::FailL()
	{
	iSubViewAbc=CContactSubView::NewL(*this,*iDb,*iUnderlyingView,_L(">=a"),_L("<d"));
	CActiveScheduler::Start();
	iSubViewAbc->Close(*this);
	ClearEventQueueL();
	}

TBool COomSubView::Skip(TInt /*aCount*/, TOomFailureModes /*aMode*/) const
	{
	return EFalse;
	}

void COomSubView::ClearUp()
	{}

// COomFilteredView
//
void COomFilteredView::HandleContactViewEvent(const CContactViewBase& /*aView*/,const TContactViewEvent& /*aEvent*/)
	{
	CActiveScheduler::Stop();
	}	

void COomFilteredView::PreFailL()
	{
	}

void COomFilteredView::FailL()
	{
	TInt filter=0;
	filter|=CContactDatabase::EPhonable;
	iPhonableFilterView=CContactFilteredView::NewL(*this,*iDb,*iUnderlyingView,filter);
	CActiveScheduler::Start();
	iPhonableFilterView->Close(*this);
	ClearEventQueueL();
	}

void COomFilteredView::ClearUp()
	{}

// COomGroupView
//

void COomGroupView::HandleContactViewEvent(const CContactViewBase& /*aView*/,const TContactViewEvent& /*aEvent*/)
	{
	if(iAsStarted)
		{
		CActiveScheduler::Stop();
		iAsStarted=EFalse;
		}
	}

void COomGroupView::PreFailL()
	{
	}

void COomGroupView::FailL()
	{
	iGroupView=CContactGroupView::NewL(*iDb,*iUnderlyingView,*this,iGroupOne->Id(),CContactGroupView::EShowContactsInGroup);
	iAsStarted=ETrue;
	CActiveScheduler::Start();
	iGroupView->Close(*this);
	ClearEventQueueL();
	}

void COomGroupView::ClearUp()
	{
	if(iAsStarted)
		{
		CActiveScheduler::Stop();
		iAsStarted=EFalse;
		}
	}

TBool COomGroupView::Skip(TInt aCount, TOomFailureModes aMode) const
	{
	if(aMode==KOomFailNormal)return aCount == 5;
	return EFalse;
	}


void COomMatchingCriteria::CleanupContactArray(TAny* aArray)	
	{
	RPointerArray<CViewContact>* array=REINTERPRET_CAST(RPointerArray<CViewContact>*,aArray);
	if (array)
		array->ResetAndDestroy();
	array->Close();
	}

void COomMatchingCriteria::FailL()
	{
	_LIT(KFindString1,"C");
	CPtrCArray* array =new(ELeave)CPtrC16Array(1);
	CleanupStack::PushL(array);
	array->AppendL(KFindString1());
	
	RPointerArray<CViewContact> contactArray;
	CleanupStack::PushL(TCleanupItem(COomMatchingCriteria::CleanupContactArray,&contactArray));

	iUnderlyingView->ContactsMatchingCriteriaL(*array, contactArray);
	
	CleanupStack::PopAndDestroy(2);
	}

void COomMatchingCriteria::HandleContactViewEvent(const CContactViewBase& /*aView*/,const TContactViewEvent& /*aEvent*/)
	{
	CActiveScheduler::Stop();
	}



//
// Main.
//

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	RProcess().SetPriority(EPriorityBackground);
	CActiveScheduler* scheduler=new CActiveScheduler;
	if (scheduler)
		{
		CActiveScheduler::Install(scheduler);
		CTrapCleanup* cleanup=CTrapCleanup::New();
		if (cleanup)
			{
			CTestConductor* tester=NULL;
			TRAPD(err,tester=CTestConductor::NewL());
			__ASSERT_ALWAYS(err==KErrNone,User::Invariant());
			delete tester;
			delete cleanup;
			}
		delete scheduler;
		}
	__UHEAP_MARKEND;
	return KErrNone;
    }
