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
#include "t_apac.h"
#include <badesca.h>

//
// Constants.
//

_LIT(KTestName,"t_apac");
_LIT(KLogFileName,"t_apac.log");

_LIT(KDbFileName,"c:t_apac.cdb");

_LIT8(KUnicodeLineEnd,"\x0D\x00\x0a\x00");
const TInt KNumberOfUnsortableContacts=10;
//Sample uid for the TestPlugin.
const TUid KFindTestUid = {0x101F8012};


LOCAL_D RTest test(KTestName);


LOCAL_C void CleanupContactArray(TAny* aArray)	
	{
	RPointerArray<CViewContact>* array=REINTERPRET_CAST(RPointerArray<CViewContact>*,aArray);
	if (array)
		array->ResetAndDestroy();
	array->Close();
	}

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
	if (iLocalView != NULL)
		{
		iLocalView->Close(*this);
		}
	if (iLocalViewWithPlugin != NULL)
		{
		iLocalViewWithPlugin->Close(*this);
		}
	if (iRemoteView != NULL)
		{
		iRemoteView->Close(*this);
		}
	if (iRemoteViewWithPlugin)
		{
		iRemoteViewWithPlugin->Close(*this);
		}

	if(iFindDesArray)
		{
		iFindDesArray->Reset();
		delete iFindDesArray;
		}
	
	iSortOrder.Close();
	delete iContactOpenOperation;
	delete iRandomGenerator;
	delete iSortChecker;
	delete iViewCreator;
	delete iContactAdder;
	delete iContactRemover;
	delete iLog;
	delete iDb;
	iTestIds.Reset();
	iTestIds.Close();

	TRAP_IGNORE(CContactDatabase::DeleteDatabaseL(KDbFileName));

	iFs.Close();
	}

CTestConductor::CTestConductor() : CActive(EPriorityStandard),iCurrentTestNumber(-1)
	{
	CActiveScheduler::Add(this);
	}

void CTestConductor::ConstructL()
	{
	iSortOrder.AppendL(KUidContactFieldGivenName);
	iSortOrder.AppendL(KUidContactFieldFamilyName);
	iSortOrder.AppendL(KUidContactFieldCompanyName);

	User::LeaveIfError(iFs.Connect());
	// ensure database exists
	iDb=CContactDatabase::ReplaceL(KDbFileName);
	// close it
	delete iDb;
	iDb = NULL;

	iRandomGenerator=CRandomContactGenerator::NewL();

	iLog=CLog::NewL(test,KLogFileName);

	NextTest();
	CActiveScheduler::Start();
	// error from active object?
	User::LeaveIfError(iTestError);
	test.Printf(_L("\n"));
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
		case EAsyncOpen:
			iLog->LogLine(_L("=== Async Open"));
			SetActive();
			iContactOpenOperation = CContactDatabase::Open(KDbFileName,iStatus);
			++iCurrentTestNumber;
			break;
		case EAsyncOpened:
			// Open failed
			if (iStatus.Int() != KErrNone)
				{
				iLog->LogLine(_L(" * Asynchronous open failed"));
				User::Leave(iStatus.Int());
				}
			iLog->LogLine(_L("=== Opened Async"));
			iDb = iContactOpenOperation->TakeDatabase();
			iRandomGenerator->SetDbL(*iDb);
			iSortChecker=CSortChecker::NewL(*this);
			iViewCreator=CViewCreator::NewL(*this);
			iContactAdder=CContactAdder::NewL(*this);
			iContactRemover=CContactRemover::NewL(*this);
			NextTest();
			break;

		case ECreateLocalView:
			iLog->LogLine(_L("=== Create local view"));
			iCurrentTestObject=iViewCreator;
			iLocalView=iViewCreator->CreateLocalViewL(iSortOrder);
			break;
		case EAddInitialContacts:
			iLog->LogLine(_L("=== Add initial contacts"));
			iCurrentTestObject=iContactAdder;
			iContactAdder->AddContactsL(_L("z:\\t_apac\\t_apac_initial_contacts.txt"),1);
			break;
		
		case ECreateLocalViewWithPlugin:
			iLog->LogLine(_L("=== Create local view with my find plugin"));
			iCurrentTestObject=iViewCreator;
			iLocalViewWithPlugin=iViewCreator->CreateLocalViewL(iSortOrder);
			iLocalViewWithPlugin->SetViewFindConfigPlugin(KFindTestUid);
			break;
		case ECreateRemoteView:
			iLog->LogLine(_L("=== Create remote view"));
			iCurrentTestObject=iViewCreator;
			iRemoteView=iViewCreator->CreateRemoteViewL(iSortOrder);
			break;
		case ECreateRemoteViewWithPlugin:
			iLog->LogLine(_L("=== Create remote view with my find plugin"));
			iCurrentTestObject=iViewCreator;
			iRemoteViewWithPlugin=iViewCreator->CreateRemoteViewL(iSortOrder);
			iRemoteViewWithPlugin->SetViewFindConfigPlugin(KFindTestUid);
			break;
		case ECriteriaSearchForStringInLocal:
			{
			iLog->LogLine(_L("=== Criteria Search for string in Local View"));
			_LIT(KFindString1,"Mar");
			CPtrCArray* array =new(ELeave)CPtrC16Array(1);
			CleanupStack::PushL(array);
			array->AppendL(KFindString1());
			RPointerArray<CViewContact> contactArray;
			CleanupStack::PushL(TCleanupItem(CleanupContactArray,&contactArray));
			iLocalView->ContactsMatchingCriteriaL(*array, contactArray);
			if(contactArray.Count() > 0)
			{
				iLog->LogLine(_L("=== Matches found!!Criteria Search Local View!"));
			}
			CleanupStack::PopAndDestroy(2, array);
			NextTest();
			}
			break;
		case ECriteriaSearchForStringInLocalWithPlugin:
			{
			iLog->LogLine(_L("=== Criteria Search for string in Local View with my plugin"));
			_LIT(KFindString1,"Mar");
			CPtrCArray* array =new(ELeave)CPtrC16Array(1);
			CleanupStack::PushL(array);
			array->AppendL(KFindString1());
			RPointerArray<CViewContact> contactArray;
			CleanupStack::PushL(TCleanupItem(CleanupContactArray,&contactArray));
			iLocalViewWithPlugin->ContactsMatchingCriteriaL(*array, contactArray);
			if(contactArray.Count() > 0)
			{
				iLog->LogLine(_L("=== Matches found!!Criteria Search Local View with plugin"));
			}
			CleanupStack::PopAndDestroy(2, array);
			NextTest();
			}
			break;
		case ECriteriaSearchForStringInRemote:
			{
			iLog->LogLine(_L("=== Criteria Search for string in Remote View"));
			_LIT(KFindString1,"Mar");
			CPtrCArray* array =new(ELeave)CPtrC16Array(1);
			CleanupStack::PushL(array);
			array->AppendL(KFindString1());
			RPointerArray<CViewContact> contactArray;
			CleanupStack::PushL(TCleanupItem(CleanupContactArray,&contactArray));
			iRemoteView->ContactsMatchingCriteriaL(*array, contactArray);
			if(contactArray.Count() > 0)
			{
				iLog->LogLine(_L("=== Matches found!!Criteria Search Remote View "));
			}
			CleanupStack::PopAndDestroy(2, array);
			NextTest();
			}
			break;
		case ECriteriaSearchForStringInRemoteWithPlugin:
			{
			iLog->LogLine(_L("=== Criteria Search for string in Remote View with plugin"));
			_LIT(KFindString1,"Mar");
			CPtrCArray* array =new(ELeave)CPtrC16Array(1);
			CleanupStack::PushL(array);
			array->AppendL(KFindString1());
			RPointerArray<CViewContact> contactArray;
			CleanupStack::PushL(TCleanupItem(CleanupContactArray,&contactArray));
			iRemoteViewWithPlugin->ContactsMatchingCriteriaL(*array, contactArray);
			if(contactArray.Count() > 0)
			{
				iLog->LogLine(_L("=== Matches found!!Criteria Search Remote View with plugin"));
			}
			CleanupStack::PopAndDestroy(2, array);
			NextTest();
			}
			break;
		case EPrefixSearchForStringInLocal:
			{
			iLog->LogLine(_L("=== Prefix Search for string in Local View"));
			_LIT(KFindString1,"Mar");
			CPtrCArray* array =new(ELeave)CPtrC16Array(1);
			CleanupStack::PushL(array);
			array->AppendL(KFindString1());
			RPointerArray<CViewContact> contactArray;
			CleanupStack::PushL(TCleanupItem(CleanupContactArray,&contactArray));
			iLocalView->ContactsMatchingCriteriaL(*array, contactArray);
			if(contactArray.Count() > 0)
			{
				iLog->LogLine(_L("=== Matches were indeed found!!Prefix Search Local View!"));
			}
			CleanupStack::PopAndDestroy(2, array);
			NextTest();
			}
			break;
		case EPrefixSearchForStringInLocalWithPlugin:
			{
			iLog->LogLine(_L("=== Prefix Search for string in Local View with my plugin"));
			_LIT(KFindString1,"Mar");
			CPtrCArray* array =new(ELeave)CPtrC16Array(1);
			CleanupStack::PushL(array);
			array->AppendL(KFindString1());
			RPointerArray<CViewContact> contactArray;
			CleanupStack::PushL(TCleanupItem(CleanupContactArray,&contactArray));
			iLocalViewWithPlugin->ContactsMatchingPrefixL(*array, contactArray);
			if(contactArray.Count() > 0)
			{
				iLog->LogLine(_L("=== Matches were found!!Prefix Search Local View with plugin"));
			}
			CleanupStack::PopAndDestroy(2, array);
			NextTest();
			}
			break;
		case EPrefixSearchForStringInRemote:
			{
			iLog->LogLine(_L("=== Prefix Search for string in Remote View"));
			_LIT(KFindString1,"Mar");
			CPtrCArray* array =new(ELeave)CPtrC16Array(1);
			CleanupStack::PushL(array);
			array->AppendL(KFindString1());
			RPointerArray<CViewContact> contactArray;
			CleanupStack::PushL(TCleanupItem(CleanupContactArray,&contactArray));
			iRemoteView->ContactsMatchingPrefixL(*array, contactArray);
			if(contactArray.Count() > 0)
			{
				iLog->LogLine(_L("=== Matches were found!!Prefix Search Remote View "));
			}
			CleanupStack::PopAndDestroy(2, array);
			NextTest();
			}
			break;
		case EPrefixSearchForStringInRemoteWithPlugin:
			{
			iLog->LogLine(_L("=== Prefix Search for string in Remote View with plugin"));
			_LIT(KFindString1,"Mar");
			CPtrCArray* array =new(ELeave)CPtrC16Array(1);
			CleanupStack::PushL(array);
			array->AppendL(KFindString1());
			RPointerArray<CViewContact> contactArray;
			CleanupStack::PushL(TCleanupItem(CleanupContactArray,&contactArray));
			iRemoteViewWithPlugin->ContactsMatchingPrefixL(*array, contactArray);
			if(contactArray.Count() > 0)
			{
				iLog->LogLine(_L("=== Matches were found!!Prefix Search Remote View with plugin"));
			}
			CleanupStack::PopAndDestroy(2, array);
			NextTest();
			}
			break;

		case ENumTests:
			{
			RThread thread;
			test(thread.RequestCount()==0);
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
	// propagate error
	iTestError = aError;

	switch (iCurrentTestNumber)
		{
		case EAsyncOpen: test.Printf(_L("test failed at step AsyncOpen (%i) with error %i"), iCurrentTestNumber, aError); break;
		case EAsyncOpened: test.Printf(_L("test failed at step AsyncOpened (%i) with error %i"), iCurrentTestNumber, aError); break;
		case ECreateLocalView: test.Printf(_L("test failed at step CreateLocalView (%i) with error %i"), iCurrentTestNumber, aError); break;
		case EAddInitialContacts: test.Printf(_L("test failed at step AddInitialContacts (%i) with error %i"), iCurrentTestNumber, aError); break;
		case ECreateLocalViewWithPlugin: test.Printf(_L("test failed at step CreateLocalViewWithPlugin (%i) with error %i"), iCurrentTestNumber, aError); break;
		case ECreateRemoteView: test.Printf(_L("test failed at step CreateRemoteView (%i) with error %i"), iCurrentTestNumber, aError); break;
		case ECreateRemoteViewWithPlugin: test.Printf(_L("test failed at step CreateRemoteViewWithPlugin (%i) with error %i"), iCurrentTestNumber, aError); break;
		case ECriteriaSearchForStringInLocal: test.Printf(_L("test failed at step CriteriaSearchForStringInLocal (%i) with error %i"), iCurrentTestNumber, aError); break;
		case ECriteriaSearchForStringInLocalWithPlugin: test.Printf(_L("test failed at step CriteriaSearchForStringInLocalWithPlugin (%i) with error %i"), iCurrentTestNumber, aError); break;
		case ECriteriaSearchForStringInRemote: test.Printf(_L("test failed at step CriteriaSearchForStringInRemote (%i) with error %i"), iCurrentTestNumber, aError); break;
		case ECriteriaSearchForStringInRemoteWithPlugin: test.Printf(_L("test failed at step CriteriaSearchForStringInRemoteWithPlugin (%i) with error %i"), iCurrentTestNumber, aError); break;
		case EPrefixSearchForStringInLocal: test.Printf(_L("test failed at step PrefixSearchForStringInLocal (%i) with error %i"), iCurrentTestNumber, aError); break;
		case EPrefixSearchForStringInLocalWithPlugin: test.Printf(_L("test failed at step PrefixSearchForStringInLocalWithPlugin (%i) with error %i"), iCurrentTestNumber, aError); break;
		case EPrefixSearchForStringInRemote: test.Printf(_L("test failed at step PrefixSearchForStringInRemote (%i) with error %i"), iCurrentTestNumber, aError); break;
		case EPrefixSearchForStringInRemoteWithPlugin: test.Printf(_L("test failed at step PrefixSearchForStringInRemoteWithPlugin (%i) with error %i"), iCurrentTestNumber, aError); break;

		case ENumTests: test.Printf(_L("test failed at step NumTests (%i) with error %i"), iCurrentTestNumber, aError); break;

		default: test.Printf(_L("test failed at step %i with error %i"), iCurrentTestNumber, aError); break;
		}

	CActiveScheduler::Stop();
	return KErrNone;
	}


void CTestConductor::HandleContactViewEvent(const CContactViewBase& aView,const TContactViewEvent& aEvent)
	{
	TRAPD(err, iCurrentTestObject->HandleNotificationL(aView,aEvent) );
	test(err == KErrNone);
	}

void CTestConductor::DoCancel()
	{
	}

void CTestConductor::ExerciseViewPreferencesL(const CContactViewBase* aViewToTest,TContactViewPreferences aPrefsToTest)
	{
	//test the viewpreferences of a view. All views with EunsortedatBegining for example should place, any contact
	//without the field specified in a sort to the begining of the main sorted list of contact
	TInt normalContacts = aViewToTest->CountL()-iTestIds.Count();
	test(normalContacts>0);
	TContactItemId id=KErrNotFound;
	TInt index =KErrNotFound;
	for(TInt ii=0;ii<iTestIds.Count();ii++)
		{
		index = aViewToTest->FindL(iTestIds[ii]);
		test(index>=0);//no error code
		if(aPrefsToTest&EUnSortedAtBeginning)
			{
			//shpould be before 'normal contacts
			test(index<iTestIds.Count());//in the right place
			}
		else if(aPrefsToTest&EUnSortedAtEnd)
			{
			//should be after the last index of the 'normal' contacts
			test(index>=normalContacts);//in the right place
			}
		//Integrity check
		id=aViewToTest->AtL(index);
		const CViewContact& contact=aViewToTest->ContactAtL(index);
		test(contact.Id()==id);
		if(aPrefsToTest&ESingleWhiteSpaceIsEmptyField)
			{
			//Contact Fields Should not have any white space as a field, _L(" ")
			for(TInt zz=0;zz<contact.FieldCount();zz++)
				{
				test((contact.Field(zz).CompareC(_L(" ")))!=0);
				}
			}
		}
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
	HBufC* buf=HBufC::NewLC(aFirstName.Length()+aLastName.Length()+1);
	TPtr bufPtr(buf->Des());
	bufPtr.Append(aFirstName);
 	if (aFirstName.Length())
 		bufPtr.Append(_L(" "));
	bufPtr.Append(aLastName);
	User::LeaveIfError(iSortedFirstLast.Append(buf));
	CleanupStack::Pop(); // buf.

	buf=HBufC::NewLC(aFirstName.Length()+aLastName.Length()+1);
	TPtr bufPtr2(buf->Des());
	bufPtr2.Append(aLastName);
 	if (aLastName.Length())
 		bufPtr2.Append(_L(" "));
	bufPtr2.Append(aFirstName);
	User::LeaveIfError(iSortedLastFirst.Append(buf));
	CleanupStack::Pop(); // buf.
	}

void CSortChecker::RemoveContactL(const TDesC& aFirstName,const TDesC& aLastName)
	{
	HBufC* buf=HBufC::NewLC(aFirstName.Length()+aLastName.Length()+1);
	TPtr bufPtr(buf->Des());
	bufPtr.Append(aFirstName);
 	if (aFirstName.Length())
 		bufPtr.Append(_L(" "));
	bufPtr.Append(aLastName);
	TInt index=iSortedFirstLast.Find(buf,TIdentityRelation<HBufC>(Match));
	ASSERT(index!=KErrNotFound);
	delete iSortedFirstLast[index];
	iSortedFirstLast.Remove(index);
	CleanupStack::PopAndDestroy(); // buf.

	buf=HBufC::NewLC(aFirstName.Length()+aLastName.Length()+1);
	TPtr bufPtr2(buf->Des());
	bufPtr2.Append(aLastName);
 	if (aLastName.Length())
 		bufPtr2.Append(_L(" "));
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
		test(high==KErrNotFound);
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
 	iSortedFirstLast.Sort(TLinearOrder<HBufC>(CompareWithoutIgnoringSpacesL));
 	iSortedLastFirst.Sort(TLinearOrder<HBufC>(CompareWithoutIgnoringSpacesL));

	const TInt numContacts=iSortedFirstLast.Count();
	__ASSERT_ALWAYS(numContacts==iSortedLastFirst.Count(),User::Invariant());
	}

void CSortChecker::CheckSort(TSortOrder aOrder,const TDesC& aName,TInt aIndex) const
	{
	const RPointerArray<HBufC>& sortedList=(aOrder==ELastNameFirstName) ? iSortedLastFirst : iSortedFirstLast;
	iConductor.iLog->LogLineNoEcho(_L("Checking - %S against %S"),&aName,sortedList[aIndex]);
	test(sortedList[aIndex]->Compare(aName)==0);
	}

void CSortChecker::CheckSortLowSubView(TSortOrder aOrder,const TDesC& aName,TInt aIndex,const TDesC& aHighBoundary) const
	{
	const RPointerArray<HBufC>& sortedList=(aOrder==ELastNameFirstName) ? iSortedLastFirst : iSortedFirstLast;
	const HBufC* name=sortedList[aIndex];
	iConductor.iLog->LogLineNoEcho(_L("Checking - %S against %S"),&aName,name);
	test(name->CompareC(aHighBoundary,3,iCollateMethod)<0);
	test(name->Compare(aName)==0);
	}

void CSortChecker::CheckSortHighSubView(TSortOrder aOrder,const TDesC& aName,TInt aIndex,const TDesC& aLowBoundary) const
	{
	const RPointerArray<HBufC>& sortedList=(aOrder==ELastNameFirstName) ? iSortedLastFirst : iSortedFirstLast;
	const TInt lowIndex=FindIndex(aOrder,aLowBoundary,CSortChecker::EGreaterThanOrEqualTo);
	if (lowIndex!=KErrNotFound)
		{
		iConductor.iLog->LogLineNoEcho(_L("Checking - %S against %S"),&aName,sortedList[lowIndex+aIndex]);
		test(sortedList[lowIndex+aIndex]->Compare(aName)==0);
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
		test(name->CompareC(aHighBoundary,3,iCollateMethod)<0);
		test(name->Compare(aName)==0);
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
	return aFirst.CompareC(aSecond);
	}

TInt CSortChecker::CompareWithoutIgnoringSpacesL(const HBufC& aFirst, const HBufC& aSecond)
 	{
 	// In order to sort correctly, we need to recognise spaces, but ignore other punctuation
 	HBufC* temp1=HBufC::NewLC(aFirst.Length());
 	TPtr first(temp1->Des());
 	first.Copy(aFirst);
 	HBufC* temp2=HBufC::NewLC(aSecond.Length());
 	TPtr second(temp2->Des());
 	second.Copy(aSecond);
 	TChar leftParenthesis('(');
 	TChar rightParenthesis(')');
 	RemoveCharacterFromDescriptor(leftParenthesis, first);
 	RemoveCharacterFromDescriptor(rightParenthesis, first);
 	RemoveCharacterFromDescriptor(leftParenthesis, second);
 	RemoveCharacterFromDescriptor(rightParenthesis, second);
 
 
   	TCollationMethod collateMethod;
   	collateMethod = *Mem::CollationMethodByIndex(0);
   	collateMethod.iFlags|=TCollationMethod::EIgnoreNone;
 	TInt compare=0;
 	TInt collationLevel=0;
 
 	while (!compare && collationLevel<4)
 		{
 		compare=first.CompareC(second,collationLevel,&collateMethod);
 		++collationLevel;
 		}
 
 	CleanupStack::PopAndDestroy(2);
 	return compare;
 	}
 
 void CSortChecker::RemoveCharacterFromDescriptor(const TChar aChar, TDes& aDes)
 	{
 	TInt location=aDes.Locate(aChar);
 	while (location!=KErrNotFound)
 		{
 		aDes.Delete(location,1);
 		location=aDes.Locate(aChar);
 		}
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

CContactLocalView* CViewCreator::CreateLocalViewL(const RContactViewSortOrder& aSortOrder,TContactViewPreferences aViewPrefs)
	{
	iView=CContactLocalView::NewL(iConductor,*iConductor.iDb,aSortOrder,aViewPrefs);
	return STATIC_CAST(CContactLocalView*,iView);
	}

CContactLocalView* CViewCreator::CreateLocalViewL(const RContactViewSortOrder& aSortOrder)
	{
	TInt32 viewPrefs=0;
	viewPrefs|=EUnSortedAtBeginning;
	iView=CContactLocalView::NewL(iConductor,*iConductor.iDb,aSortOrder,TContactViewPreferences(viewPrefs));
	return STATIC_CAST(CContactLocalView*,iView);
	}
CContactNamedLocalView* CViewCreator::CreateNamedLocalViewL(const TDesC& aName,const RContactViewSortOrder& aSortOrder)
	{
	TInt32 viewPrefs=0;
	viewPrefs|=EUnSortedAtBeginning;
	iView=CContactNamedLocalView::NewL(iConductor,aName,*iConductor.iDb,aSortOrder,TContactViewPreferences(viewPrefs));
	return STATIC_CAST(CContactNamedLocalView*,iView);
	}

CContactRemoteView* CViewCreator::CreateRemoteViewL(const RContactViewSortOrder& aSortOrder)
	{
	TInt32 viewPrefs=0;
	viewPrefs|=EUnSortedAtBeginning;
	iView=CContactRemoteView::NewL(iConductor,*iConductor.iDb,aSortOrder,TContactViewPreferences(viewPrefs));
	return STATIC_CAST(CContactRemoteView*,iView);
	}

CContactNamedRemoteView* CViewCreator::CreateNamedRemoteViewL(const TDesC& aName,const RContactViewSortOrder& aSortOrder)
	{
	TInt32 viewPrefs=0;
	viewPrefs|=EUnSortedAtBeginning;
	iView=CContactNamedRemoteView::NewL(iConductor,aName,*iConductor.iDb,aSortOrder,TContactViewPreferences(viewPrefs));
	return STATIC_CAST(CContactNamedRemoteView*,iView);
	}

void CViewCreator::HandleNotificationL(const CContactViewBase& aView,const TContactViewEvent& aEvent)
	{
	test(&aView==iView);
	test(aEvent.iEventType==TContactViewEvent::EReady);
	iConductor.NextTest();
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
	test.Printf(_L("Adding contact "));
	AddOneContactL();
	}

void CContactAdder::HandleNotificationL(const CContactViewBase&,const TContactViewEvent& aEvent)
	{
	if (++iNumNotifications == iNumViews)
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
		test(aEvent.iEventType == TContactViewEvent::EItemAdded);
		}
	}

CContactAdder::CContactAdder(CTestConductor& aConductor) : CTestBase(aConductor)
	{
	}

void CContactAdder::ConstructL()
	{
	User::LeaveIfError(iFs.Connect());
	iTemplate=STATIC_CAST(CContactTemplate*,iConductor.iDb->ReadContactL(iConductor.iDb->TemplateId()));
	}

void CContactAdder::AddOneContactL()
	{
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
	test.Printf(_L("Removing contact "));
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
			TRAPD(err, RemoveOneContactL() );
			test(err == KErrNone);
			}
		}
	else
		{
		test(aEvent.iEventType==TContactViewEvent::EItemRemoved);
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
	test(aEvent.iEventType==TContactViewEvent::EUnavailable || aEvent.iEventType==TContactViewEvent::ESortOrderChanged || aEvent.iEventType==TContactViewEvent::EReady);
//	if (++iNumNotifications==iNumViews*3) // 3 because there will be EUnavailable, ESortOrderChanged, and EReady notifications for each view.
	if (++iNumNotifications==iNumViews*2) // 3 because there will be EUnavailable, ESortOrderChanged, and EReady notifications for each view.		
		{
		iConductor.NextTest();
		}
	}

CSortOrderChanger::CSortOrderChanger(CTestConductor& aConductor) : CTestBase(aConductor)
	{
	}


//
// CUnsortableContactAdder.
//

CUnsortableContactAdder::CUnsortableContactAdder(CTestConductor& aConductor,const CContactViewBase& aView)
	:CTestBase(aConductor),iView(&aView)
	{}

CUnsortableContactAdder* CUnsortableContactAdder::NewL(CTestConductor& aConductor,const CContactViewBase& aView)
	{
	CUnsortableContactAdder* self=new(ELeave) CUnsortableContactAdder(aConductor,aView);
	return self;
	}

CUnsortableContactAdder::~CUnsortableContactAdder()
		{
		}

void CUnsortableContactAdder::AddContactsL(TInt aNumberOfContact,TContactViewPreferences aContactTypes)
	{
	iNumContacts = aNumberOfContact;
	iNumNotifications=0;
	for(TInt ii=0;ii<KNumberOfUnsortableContacts;ii++)
		{
		TContactItemId id = iConductor.iRandomGenerator->AddTypicalContactForLocalViewPreferencesL(TContactViewPreferences(aContactTypes));
		iConductor.iTestIds.Append(id);
		}


	}
void CUnsortableContactAdder::HandleNotificationL(const CContactViewBase& aView,const TContactViewEvent& aEvent)
	{
	if(aEvent.iEventType==TContactViewEvent::EItemAdded && &aView == iView )
		{
		//only  start the next test when all the contact have been added to our view.
		if (++iNumNotifications==iNumContacts) 
			{
			iConductor.NextTest();
			}
		}
	}


//
// Main.
//

/**

@SYMTestCaseID     PIM-T-APAC-0001

*/

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
//	UserHal::SetAutoSwitchOffBehavior(ESwitchOffDisabled); 
	test.Start(_L("@SYMTESTCaseID:PIM-T-APAC-0001 t_apac"));

	CActiveScheduler* scheduler=new CActiveScheduler;
	if (scheduler)
		{
		CActiveScheduler::Install(scheduler);
		CTrapCleanup* cleanup=CTrapCleanup::New();
		if (cleanup)
			{
			CTestConductor* tester=NULL;
			TRAPD(err,tester=CTestConductor::NewL());
			test(err == KErrNone);
			delete tester;
			delete cleanup;
			}
		delete scheduler;
		}
	test.End();
	test.Close();
	__UHEAP_MARKEND;
	return KErrNone;
    }
