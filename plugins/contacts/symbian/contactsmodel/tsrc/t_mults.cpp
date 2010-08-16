/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <f32file.h>
#include <s32file.h>
#include <cntdb.h>
#include <cntitem.h>
#include <cntfield.h>
#include <cntfldst.h>
#include "t_utils.h"
#include "t_utils2.h"

RFs fs;
CCntTest* CntTest=NULL;
#define xtest(x) TestErr(x,__LINE__)

_LIT(KDataBaseName, "c:T_MultReq.cdb");

const TInt KDeleteGranularity=16;	// Must be kept in step with KDeleteTransactionGranularity in CNTDB.CPP

struct TUpdatingResults
	{
public:
	TUpdatingResults();
	void ProcessErrorL(TInt aError);
	void ProcessErrorL(TInt aError, TUpdatingResults &aMoreResults);
	void PrintResults(const TDesC &aTitle) const;
	void AppendResults(TDes &aBuf) const;
public:
	TInt iTotalCount;
	TInt iLockedCount;
	TInt iNotFoundCount;
	TInt iModifiedErr;
	};

struct SDbmsErrParams
	{
	inline SDbmsErrParams(TBool *aRemoteThreadFinished,TUpdatingResults *aResults) : iRemoteThreadFinished(aRemoteThreadFinished),iResults(aResults) {};
	TBool *iRemoteThreadFinished;
	TUpdatingResults *iResults;
	};

LOCAL_D RTest test(_L("T_MULTS"));
LOCAL_D CContactDatabase* TheDb1;
LOCAL_D CContactDatabase* TheDb2;
LOCAL_D CArrayFix<TContactItemId>* TheIds;
void DoDEF022709TestL();

const TPtrC KDatabaseFileName=_L("C:T_MULTS");

// Note long names needed to push record size of 255 bytes to trigger problems with DBMS locking
const TPtrC KTestName1=_L("Test Name No%d, plus some padding to make the data over 255 bytes, 0123456789, 0123456789, 0123456789, 0123456789");
const TPtrC KTestName2=_L("Test Name 2 No%d, plus some padding to make the data over 255 bytes, 0123456789, 0123456789, 0123456789, 0123456789");
const TPtrC KTestName3=_L("Test Name 3 No%d, plus some padding to make the data over 255 bytes, 0123456789, 0123456789, 0123456789, 0123456789");
const TInt KTotalNumRecords=20;

struct TOpenBaseParams
	{
	TFileName iDbName;
	TInt iParam1;
	TInt iParam2;
	};

enum TTestFuncs
	{
	EThreadFuncHoldOpenForAWhile,
	EThreadFuncHoldOpenForever,
	EThreadFuncHoldOpenRangeForAWhile,
	EThreadFuncHoldOpenRangeForever,
	EThreadFuncKeepUpdatingContacts,
	};

enum TThreadFuncs
	{
	EFuncOpenContact,
	EFuncDeleteContact,
	EFuncKeepReadingContacts,
	};

class CFred2 : public CBase
	{
public:
	~CFred2();
	void ConstructL(TInt aFunc, const TDesC8 &aParams);
	inline TInt Func() {return(iFunc);};
	inline const HBufC8 *Params() {return(iParams);};
	void WaitForLogon();
	void Kill();
// Other thread funcs
	void DoTestL();
protected:
	void OpenDbEtcL();
	void RunTestL();
	void CloseDbEtcL();
	void KeepUpdatingContactsL(TInt aNumTimesToLoop,SDbmsErrParams *aResults);
protected:
	CContactDatabase* iDb;
	TInt iFunc;
	HBufC8* iParams;
	RThread iThread;
	TRequestStatus iLogonStatus;
	};

class CNotificationRec : public CBase, public MContactDbObserver
	{
public:
	static CNotificationRec* NewLC();
	~CNotificationRec();
	void Wait(TContactItemId aTestId, TContactDbObserverEventType aEventType, const CContactDatabase* aDatabase);
	static TInt TimerCallBackL(TAny* aSelf);
	void Purge();
public: // from MContactDbObserver
	void HandleDatabaseEventL(TContactDbObserverEvent aEvent);
private:
	void ConstructL();
private:
	CPeriodic* iTimer;
	TBool iWaiting;
	TContactItemId iTestId;
	TUint iConnectionId;
	TContactDbObserverEventType iEventType;
	};

//
// TUpdatingResults
//

TUpdatingResults::TUpdatingResults()
	{
	iTotalCount=0;
	iLockedCount=0;
	iNotFoundCount=0;
	iModifiedErr=0;
	}

void TUpdatingResults::ProcessErrorL(TInt aError)
	{
	if (aError==KErrLocked)
		iLockedCount++;
	else if (aError==KErrNotFound)
		iNotFoundCount++;
	else if (aError==KErrCommsLineFail)
		iModifiedErr++;
	else
		User::LeaveIfError(aError);
	iTotalCount++;
	}

void TUpdatingResults::ProcessErrorL(TInt aError, TUpdatingResults &aMoreResults)
	{
	ProcessErrorL(aError);
	aMoreResults.ProcessErrorL(aError);
	}

void TUpdatingResults::PrintResults(const TDesC &aTitle) const
	{
	test.Printf(aTitle);
	test.Printf(_L("-Locked/Not Found/Debug/Total[1] %d/%d/%d/%d\n"),iLockedCount,iNotFoundCount,iModifiedErr,iTotalCount);
	}

void TUpdatingResults::AppendResults(TDes &aBuf) const
	{
	aBuf.AppendFormat(_L("%d/%d/%d/%d"),iLockedCount,iNotFoundCount,iModifiedErr,iTotalCount);
	}

//////////////////

LOCAL_C CFred2 *LaunchFred2L(TInt aFunc, const TDesC8 &aParams)
	{
	CFred2 *fred2=new(ELeave) CFred2;
	fred2->ConstructL(aFunc, aParams);
	return(fred2);
	}

CNotificationRec* CNotificationRec::NewLC()
	{ // static
	CNotificationRec* self=new(ELeave) CNotificationRec;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CNotificationRec::~CNotificationRec()
	{
	delete iTimer;
	}

void CNotificationRec::ConstructL()
	{
	iTimer=CPeriodic::NewL(0);
	}

void CNotificationRec::Wait(TContactItemId aTestId, TContactDbObserverEventType aEventType, const CContactDatabase* aDatabase)
	{
	iWaiting=ETrue;
	iTestId=aTestId;
	iEventType=aEventType;
	iConnectionId=aDatabase->ConnectionId();
	iTimer->Start(5000000,5000000,TCallBack(TimerCallBackL,NULL)); // wait for 5 seconds for notification
	CActiveScheduler::Start();
	}

void CNotificationRec::Purge()
	{
	iWaiting=EFalse;
	iTimer->Start(2000000,2000000,TCallBack(TimerCallBackL,this)); // wait for 2 seconds for notification
	CActiveScheduler::Start();
	}

TInt CNotificationRec::TimerCallBackL(TAny* aSelf)
	{ // static
	// if this gets called, notification hasn't happened
	if (((CNotificationRec*)aSelf)->iWaiting)
		User::Leave(KErrGeneral);
	else
		{
		((CNotificationRec*)aSelf)->iTimer->Cancel();
		CActiveScheduler::Stop();
		}
	return 0;
	}

void CNotificationRec::HandleDatabaseEventL(TContactDbObserverEvent aEvent)
	{
	if (!iWaiting || aEvent.iType==EContactDbObserverEventRollback)
		return;
	if (iEventType!=EContactDbObserverEventUnknownChanges)
		test(aEvent.iContactId==iTestId);
	test(iEventType==aEvent.iType);
	switch(iEventType)
		{
		case EContactDbObserverEventRecover:
		case EContactDbObserverEventTablesClosed:
		case EContactDbObserverEventTablesOpened:
			test(aEvent.iConnectionId==0);
			break;
		default:
			test(iConnectionId==aEvent.iConnectionId);
			break;
		}
	iWaiting=EFalse;
	iTimer->Cancel();
	CActiveScheduler::Stop();
	}


LOCAL_C void PopulateDatabaseL()
//
// Create and populate the database
//
	{
	TheIds=new(ELeave) CArrayFixFlat<TContactItemId>(5);
	CContactDatabase* db=CntTest->CreateDatabaseL();
	CntTest->DeleteAllTemplateFieldsL();
	TContactItemId id;
	for (TInt ii=0;ii<KTotalNumRecords;ii++)
		{
		CContactItem* item=CContactCard::NewLC();
		TBuf<256> name;
		name.Format(KTestName1,ii);
		SetNameL(*item,KUidContactFieldCompanyName,KUidContactFieldVCardMapUnusedN,name,ETrue);
		name.Format(KTestName2,ii);
		SetNameL(*item,KUidContactFieldFamilyName,KUidContactFieldVCardMapUnusedN,name,ETrue);
		name.Format(KTestName3,ii);
		SetNameL(*item,KUidContactFieldGivenName,KUidContactFieldVCardMapUnusedN,name,ETrue);
		id=db->AddNewContactL(*item);
		TheIds->AppendL(id);
		CleanupStack::PopAndDestroy(); // item
		}
	CntTest->CloseDatabase();
	}
	
LOCAL_C TPtrC Name(CContactItem& aItem,TUid aType)
	{
	CContactItemFieldSet& fieldSet=aItem.CardFields();
	const TInt pos=fieldSet.Find(aType);
	if (pos==KErrNotFound)
		return _L("");
	return fieldSet[pos].TextStorage()->Text();
	}

LOCAL_C void ReadL()
//
// Access the database with two clients running in the same thread
//
	{
	CContactItem* item1=NULL;
	CContactItem* item2=NULL;
	for (TInt ii=0;ii<KTotalNumRecords;ii++)
		{
		TContactItemId id=(*TheIds)[ii];
		item1=TheDb1->ReadContactLC(id);
		item2=TheDb2->ReadContactLC(id);
		test(Name(*item1,KUidContactFieldCompanyName)==Name(*item2,KUidContactFieldCompanyName));
		TBuf<256> testName;
		testName.Format(KTestName1,ii);
		test(Name(*item1,KUidContactFieldCompanyName)==testName);
		CleanupStack::PopAndDestroy(2); // item2, item1
		}
	}

LOCAL_C void EditL()
	{
	CContactItem* item1=NULL;
	CContactItem* item2=NULL;
	for (TInt ii=0;ii<KTotalNumRecords;ii++)
		{
		TContactItemId id=(*TheIds)[ii];
		// edit items in db1
		item1=TheDb1->OpenContactL(id);
		CleanupStack::PushL(item1);
		SetNameL(*item1,KUidContactFieldFamilyName,KUidContactFieldVCardMapUnusedN,_L("Family"),EFalse);
		TheDb1->CommitContactL(*item1);
		CleanupStack::PopAndDestroy(); // item1
		// test db2 picks up changes and re-edit items
		item2=TheDb2->OpenContactL(id);
		CleanupStack::PushL(item2);
		test(Name(*item2,KUidContactFieldFamilyName)==_L("Family"));
		SetNameL(*item2,KUidContactFieldGivenName,KUidContactFieldVCardMapUnusedN,_L(""),EFalse);
		TheDb2->CommitContactL(*item2);
		CleanupStack::PopAndDestroy(); // item2
		// check db1 notices changes
		item1=TheDb1->ReadContactL(id);
		test(Name(*item1,KUidContactFieldGivenName)==_L(""));
		delete item1;
		}
	// test each one can lock records
	TContactItemId id=(*TheIds)[0];
	item1=TheDb1->OpenContactL(id);
	CleanupStack::PushL(item1);
	TRAPD(err,TheDb2->OpenContactL(id));
	test(err==KErrInUse);
	TheDb1->CloseContactL(id);
	CleanupStack::PopAndDestroy(); // item1

	item2=TheDb2->OpenContactL(id);
	CleanupStack::PushL(item2);
	TRAP(err,TheDb1->OpenContactL(id));
	test(err==KErrInUse);
	TheDb2->CloseContactL(id);
	CleanupStack::PopAndDestroy(); // item2
	}

LOCAL_C void AddL()
	{
	CContactItem* item=CContactCard::NewLC();
	TContactItemId id1=TheDb1->AddNewContactL(*item);
	TContactItemId id2=TheDb2->AddNewContactL(*item);
	CleanupStack::PopAndDestroy(); // item
	delete TheDb1->ReadContactL(id2);
	delete TheDb2->ReadContactL(id1);
	TheDb1->DeleteContactL(id2);
	TheDb2->DeleteContactL(id1);
	}

LOCAL_C void TouchContactL(CContactDatabase* aDb, TContactItemId aId)
	{
	CContactItem* item=aDb->OpenContactL(aId);
	SetNameL(*item,KUidContactFieldGivenName,KUidContactFieldVCardMapUnusedN,_L(""),EFalse);
	aDb->CommitContactL(*item);
	delete item;
	}

LOCAL_C void AdjustContactAccessCountL(CContactDatabase* aDb, TContactItemId aItemId, TInt aCount)
	{
	CContactItem *incItem=aDb->OpenContactLX(aItemId);
	CleanupStack::PushL(incItem);
	while(aCount>0)
		{
		incItem->IncAccessCount();
		aCount--;
		}
	while(aCount<0)
		{
		incItem->DecAccessCount();
		aCount++;
		}
	aDb->CommitContactL(*incItem);
	CleanupStack::PopAndDestroy(2);	// incItem, Close(incItem)
	}

LOCAL_C void WaitAndCheckEvent(CNotificationRec* aRec1,CNotificationRec* aRec2,TContactItemId aTestId, TContactDbObserverEventType aEventType, const CContactDatabase* aDatabase)
	{
	aRec1->Wait(aTestId,aEventType,aDatabase);
	if (aRec2)
		aRec2->Wait(aTestId,aEventType,aDatabase);
	}

LOCAL_C void SetSortOrderL(CContactDatabase* aDb, TFieldType aFieldType)
	{
	CArrayFix<CContactDatabase::TSortPref>* sortOrder=new(ELeave) CArrayFixFlat<CContactDatabase::TSortPref>(3);
	CleanupStack::PushL(sortOrder);
	sortOrder->AppendL(CContactDatabase::TSortPref(aFieldType,CContactDatabase::TSortPref::EAsc));
	aDb->SortL(sortOrder);
	CleanupStack::Pop();	// sortOrder
	}

LOCAL_C void TestDeleteContactsErrorL(CContactDatabase* aDb, CContactDatabase* aDb2, CNotificationRec* aRec, CNotificationRec* aRec2, TInt aErrorPos, TBool aChangedMessage)
	{
	aRec2->Purge();
	SetSortOrderL(aDb,KUidContactFieldGivenName);
	CContactIdArray *deleteIds=CContactIdArray::NewLC();
	CContactItem *openItem=NULL;
	TInt startCount=aDb->SortedItemsL()->Count();
	TInt startCount2=aDb2->SortedItemsL()->Count();
	for(TInt loop=0;loop<(aErrorPos+10);loop++)
		{
		TContactItemId add=AddContactL(aDb,KUidContactFieldFamilyName,KUidContactFieldVCardMapUnusedN,_L("Add1"));
		deleteIds->AddL(add);
		aRec->Wait(add,EContactDbObserverEventContactAdded,aDb);
		aRec2->Wait(add,EContactDbObserverEventContactAdded,aDb);
		if (loop==aErrorPos)
			{
			openItem=aDb2->OpenContactLX(add);
			CleanupStack::PushL(openItem);
			}
		TInt db2Count=aDb2->SortedItemsL()->Count();
		test(db2Count==(startCount2+loop+1));
		}
	test(openItem!=NULL);
	TRAPD(delErr,aDb->DeleteContactsL(*deleteIds));
	test(delErr==KErrInUse);
	User::After(500000);	// Let rollback event arrive
	if (aChangedMessage)
		aRec->Wait(0,EContactDbObserverEventUnknownChanges,aDb);
//
	CleanupStack::PopAndDestroy(3); // deleteIds, openItem, openItem->Close()
//
	aDb2->RecoverL();
//
	if (aChangedMessage)
		{
		aRec->Wait(0,EContactDbObserverEventTablesClosed,aDb);
//		aRec->Wait(0,EContactDbObserverEventUnknownChanges,aDb);
		}
//
	aDb->OpenTablesL();
	aRec->Purge();
	aRec2->Purge();
	SetSortOrderL(aDb,KUidContactFieldFamilyName);
	TInt db1Count=aDb->SortedItemsL()->Count();
	TInt db2Count=aDb2->SortedItemsL()->Count();
	test(db1Count==db2Count);
	if (aChangedMessage)
		test(db1Count==(startCount+10+aErrorPos-KDeleteGranularity));
	else
		test(db1Count==(startCount+10+aErrorPos));
//
	deleteIds=CContactIdArray::NewLC(aDb->SortedItemsL());
	aDb->DeleteContactsL(*deleteIds);
	aRec->Wait(0,EContactDbObserverEventUnknownChanges,aDb);
	CleanupStack::PopAndDestroy(); // deleteIds
	}

LOCAL_C void WaitForNotificationL()
	{
	CNotificationRec* rec1=CNotificationRec::NewLC();
	CNotificationRec* rec2=CNotificationRec::NewLC();
	CContactDatabase* db1=CContactDatabase::OpenL(CntTest->DatabaseName());
	CleanupStack::PushL(db1);
	CContactChangeNotifier *notify1=CContactChangeNotifier::NewL(*db1,rec1);
	CleanupStack::PushL(notify1);
	CContactDatabase* db2=CContactDatabase::OpenL(CntTest->DatabaseName());
	CleanupStack::PushL(db2);
	CContactChangeNotifier *notify2=CContactChangeNotifier::NewL(*db2,rec2);
	CleanupStack::PushL(notify2);
	TContactItemId touchId=(*TheIds)[0];
	TouchContactL(db1,touchId);
	WaitAndCheckEvent(rec1,rec2,touchId,EContactDbObserverEventContactChanged,db1); // shouldn't return until the change is notified
//
	touchId=(*TheIds)[1];
	TouchContactL(db2,touchId);
	WaitAndCheckEvent(rec1,rec2,touchId,EContactDbObserverEventContactChanged,db2);
//
	TouchContactL(db2,(*TheIds)[0]);
	TouchContactL(db2,(*TheIds)[1]);
	TouchContactL(db2,(*TheIds)[2]);
	db2->DeleteContactL((*TheIds)[1]);
	TouchContactL(db2,(*TheIds)[3]);
	TouchContactL(db2,(*TheIds)[0]);
// zzz Beef up test code to have a queue of expected events and read rec & rec2 in parallel
	WaitAndCheckEvent(rec1,NULL,(*TheIds)[0],EContactDbObserverEventContactChanged,db2);
	WaitAndCheckEvent(rec1,NULL,(*TheIds)[1],EContactDbObserverEventContactChanged,db2);
	WaitAndCheckEvent(rec1,NULL,(*TheIds)[2],EContactDbObserverEventContactChanged,db2);
	WaitAndCheckEvent(rec1,NULL,(*TheIds)[1],EContactDbObserverEventContactDeleted,db2);
	WaitAndCheckEvent(rec1,NULL,(*TheIds)[3],EContactDbObserverEventContactChanged,db2);
	WaitAndCheckEvent(rec1,NULL,(*TheIds)[0],EContactDbObserverEventContactChanged,db2);
//
	AdjustContactAccessCountL(db2,(*TheIds)[2],1);
	rec1->Wait((*TheIds)[2],EContactDbObserverEventContactChanged,db2);
	db2->DeleteContactL((*TheIds)[2]);
	rec1->Wait((*TheIds)[2],EContactDbObserverEventContactDeleted,db2);
	AdjustContactAccessCountL(db2,(*TheIds)[2],-1);
	TouchContactL(db2,(*TheIds)[0]);
	rec1->Wait((*TheIds)[0],EContactDbObserverEventContactChanged,db2);
//
	TContactItemId add1=AddContactL(db1,KUidContactFieldFamilyName,KUidContactFieldVCardMapUnusedN,_L("Add1"));
	TContactItemId add2=AddContactL(db2,KUidContactFieldFamilyName,KUidContactFieldVCardMapUnusedN,_L("Add2"));
	TContactItemId add3=AddContactL(db1,KUidContactFieldFamilyName,KUidContactFieldVCardMapUnusedN,_L("Add3"));
	rec1->Wait(add1,EContactDbObserverEventContactAdded,db1);
	rec1->Wait(add2,EContactDbObserverEventContactAdded,db2);
	rec1->Wait(add3,EContactDbObserverEventContactAdded,db1);
//
	CContactIdArray* deleteIds=CContactIdArray::NewLC();
	deleteIds->AddL(add2);
	deleteIds->AddL(add1);
	deleteIds->AddL(add3);
	db1->DeleteContactsL(*deleteIds);
	rec1->Wait(0,EContactDbObserverEventUnknownChanges,db1);
	CleanupStack::PopAndDestroy(); // deleteIds
//
	TestDeleteContactsErrorL(db1,db2,rec1,rec2,KDeleteGranularity-4,EFalse);	// Count lower than delete granularity
	TestDeleteContactsErrorL(db1,db2,rec1,rec2,KDeleteGranularity+4,ETrue);		// Count Higher than delete granularity
//
	CleanupStack::PopAndDestroy(6); // db2, db1, notify2, notify1, rec2, rec1
	}

void TestErr(TInt aErr,TInt aLineNum)
    {
	if (aErr!=KErrNone)
		{
		test.Printf(_L("Error %d, line %d\n"),aErr,aLineNum);
		test(EFalse);
		}
	}

LOCAL_C void KeepReadingContactsL(TInt ,SDbmsErrParams *aParams)
	{
	const CContactIdArray* idarray=CContactIdArray::NewLC(TheDb1->SortedItemsL());
	User::After(200000);	// Synch with other thread
	while(!(*aParams->iRemoteThreadFinished))
		{
		for(TInt idIndex=idarray->Count();idIndex>0;)
			{
			CContactItem *readItem=NULL;
			TContactItemId id=(*idarray)[--idIndex];
			TRAPD(err,readItem=TheDb1->ReadContactL(id));
			CleanupStack::PushL(readItem);
			aParams->iResults->ProcessErrorL(err);
			if (err==KErrLocked)
				test.Printf(_L("*"));
			else if (err==KErrNotFound)
				test.Printf(_L("#"));
			else if (err==KErrCommsLineFail)
				test.Printf(_L("%"));
			else
				test.Printf(_L("."));
			CleanupStack::PopAndDestroy();	// readItem
			}
		}
	test.Printf(_L("\n"));
	CleanupStack::PopAndDestroy();	// idArray
	}

LOCAL_C void MultiAccessTestL(TInt aFredFunc,TInt aFredParam1,TInt aFredParam2,TInt aFunc,TInt aParam1,TInt aParam2)
	{
	TOpenBaseParams params;
	params.iDbName.Copy(CntTest->DatabaseName());
	params.iParam1=aFredParam1;
	params.iParam2=aFredParam2;
	CFred2* fred2=LaunchFred2L(aFredFunc,TPtrC8((TUint8 *)&params,sizeof(params)));
	User::After(600000);	// Let Fred2 get going
//
	switch(aFunc)
		{
		case EFuncOpenContact:
			{
			CContactItem *item=NULL;
			TRAPD(openErr,item=TheDb1->OpenContactL((*TheIds)[aParam1]));
			CleanupStack::PushL(item);
			xtest(openErr);
			TheDb1->CommitContactL(*item);
			CleanupStack::PopAndDestroy();
			}
			break;
		case EFuncDeleteContact:
			TheDb1->DeleteContactL((*TheIds)[aParam1]);
			break;
		case EFuncKeepReadingContacts:
			KeepReadingContactsL(aParam1,(SDbmsErrParams *)aParam2);
			break;
		}
//
	if (aFredFunc==EThreadFuncHoldOpenForever || aFredFunc==EThreadFuncHoldOpenRangeForever)
		fred2->Kill();
	fred2->WaitForLogon();
	delete fred2;
	User::After(400000);	// Let Fred2 shutdown properly
	}

/**

@SYMTestCaseID     PIM-T-MULTS-0001

*/

LOCAL_C void MultiAccessTestL()
	{
	test.Start(_L("@SYMTestCaseID:PIM-T-MULTS-0001 Multi 1"));
//
	MultiAccessTestL(EThreadFuncHoldOpenForAWhile,1,0,EFuncOpenContact,1,0);
	test.Next(_L("Multi 2"));
	MultiAccessTestL(EThreadFuncHoldOpenForever,2,0,EFuncOpenContact,3,0);
//
	test.Next(_L("Multi 3"));
	MultiAccessTestL(EThreadFuncHoldOpenRangeForAWhile,8,9,EFuncOpenContact,5,0);
	test.Next(_L("Multi 4"));
	MultiAccessTestL(EThreadFuncHoldOpenRangeForever,2,16,EFuncOpenContact,18,0);
//
	test.Next(_L("Multi 5"));
	MultiAccessTestL(EThreadFuncHoldOpenForAWhile,1,0,EFuncDeleteContact,1,0);
	test.Next(_L("Multi 6"));
	MultiAccessTestL(EThreadFuncHoldOpenForever,0,0,EFuncDeleteContact,3,0);
//
	test.Next(_L("Multi 7"));
	TUpdatingResults updateResults1;
	TUpdatingResults updateResults2[4];
	TBool remoteThreadFinished=EFalse;
	SDbmsErrParams params1(&remoteThreadFinished,&updateResults1);
	SDbmsErrParams params2(&remoteThreadFinished,&updateResults2[0]);
	MultiAccessTestL(EThreadFuncKeepUpdatingContacts,5,(TInt)&params2,EFuncKeepReadingContacts,0,(TInt)&params1);
	updateResults1.PrintResults(_L("Reading"));
	updateResults2[0].PrintResults(_L("Open/Commit"));
	updateResults2[1].PrintResults(_L("Export"));
	updateResults2[2].PrintResults(_L("Import"));
	updateResults2[3].PrintResults(_L("Delete"));
//
	test.End();
	}

void InitialiseDatabaseEtcL()
	{
	TRAPD(err,PopulateDatabaseL());
	xtest(err);
	TheDb1=CContactDatabase::OpenL(CntTest->DatabaseName());
	TheDb2=CContactDatabase::OpenL(CntTest->DatabaseName());
	}

void DeleteDatabaseEtc()
	{
	delete TheDb1;
	delete TheDb2;
	delete TheIds;
	TheDb1=NULL;
	TheDb2=NULL;
	TheIds=NULL;
	}

class RTestSession : public RSessionBase
	{
public:
	TInt CreateSession();
	};

TInt RTestSession::CreateSession()
	{
	TVersion version;
	return(RSessionBase::CreateSession(_L("CntLockServer"),version,1));
	}

void TestServerGoneL()
	{
	test(CntTest->LockServerProcessCount(ETrue,EFalse,ETrue)==0);
	if (CntTest->LockServerSessionExists())
		{
		test.Printf(_L("Warning - Lock server still exists"));
		test.Getch();
		}
	}

void DoTestsL()
    {
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	CTestRegister* TempFiles = CTestRegister::NewLC();
	TempFiles->RegisterL(KDataBaseName, EFileTypeCnt);
	TempFiles->RegisterL(KDatabaseFileName, EFileTypeCnt);

	CntTest->SelectDriveL();
	test.Start(_L("Create new database"));
	TInt err;
//
	TRAP(err,InitialiseDatabaseEtcL());
	test.Next(_L("Wait for change notification"));
	TRAP(err,WaitForNotificationL());
	xtest(err);
	DeleteDatabaseEtc();
//
	TRAP(err,InitialiseDatabaseEtcL());
	xtest(err);
	test.Next(_L("Lock tests"));
	TRAP(err,MultiAccessTestL());
	xtest(err);
	DeleteDatabaseEtc();
//
	TRAP(err,InitialiseDatabaseEtcL());
	xtest(err);
	test.Next(_L("Read items"));
	TRAP(err,ReadL());
	xtest(err);
	test.Next(_L("Edit items"));
	TRAP(err,EditL());
	xtest(err);
	test.Next(_L("Add / delete items"));
	TRAP(err,AddL());
	xtest(err);
	test.Next(_L("Wait for change notification"));
	TRAP(err,WaitForNotificationL());
	xtest(err);
	DeleteDatabaseEtc();
	TestServerGoneL();

	// Now do test for DEF022709  (Propagated) Contact DB Monitoring Error Messages 
	TRAP(err,DoDEF022709TestL());
	xtest(err);

	CleanupStack::PopAndDestroy(2); // TempFiles, close fs
    }

LOCAL_C TInt FredIIFunc(TAny *aParam);

GLDEF_C TInt E32Main()
	{
    CntTest=new(ELeave) CCntTest;
	CntTest->ConstructL(test,KDatabaseFileName);
	TRAPD(err,DoTestsL());
	CntTest->EndTestLib(err);
	return KErrNone;
    }

//
// CFred2
//

CFred2::~CFred2()
	{
	iThread.Close();
	delete iParams;
	}

void CFred2::ConstructL(TInt aFunc, const TDesC8 &aParams)
	{
	iFunc=aFunc;
	iParams=aParams.AllocL();
	User::LeaveIfError(iThread.Create(_L("FredII"),FredIIFunc,KDefaultStackSize,0x2000,0x20000,this,EOwnerThread));
	iThread.Logon(iLogonStatus);
	iThread.Resume();
	}

void CFred2::Kill()
	{
	iThread.Kill(0);
	}

void CFred2::WaitForLogon()
	{
	User::WaitForRequest(iLogonStatus);
	xtest(iLogonStatus.Int());
	}

//
// Fred II from here on
// 

LOCAL_C TInt FredIIFunc(TAny *aParam)
	{
	CActiveScheduler::Install(new(ELeave) CActiveScheduler);
    CTrapCleanup* cleanup=CTrapCleanup::New();
	TRAPD(err,((CFred2 *)aParam)->DoTestL());
    delete cleanup;
	delete CActiveScheduler::Current();
	return(err);
	}

void CFred2::OpenDbEtcL()
	{
	switch(Func())
		{
		case EThreadFuncHoldOpenForAWhile:
		case EThreadFuncHoldOpenForever:
		case EThreadFuncHoldOpenRangeForAWhile:
		case EThreadFuncHoldOpenRangeForever:
		case EThreadFuncKeepUpdatingContacts:
			iDb=CContactDatabase::OpenL(((TOpenBaseParams *)Params()->Ptr())->iDbName);
			break;
		}
	}

void CFred2::CloseDbEtcL()
	{
	delete iDb;
	}

void CFred2::KeepUpdatingContactsL(TInt aNumTimesToLoop,SDbmsErrParams *aParams)
	{
	TUpdatingResults *openResults=aParams->iResults;
	TUpdatingResults *exportResults=aParams->iResults+1;
	TUpdatingResults *importResults=aParams->iResults+2;
	TUpdatingResults *deleteResults=aParams->iResults+3;
	TUpdatingResults totalResults;
	const CContactIdArray* idarray=CContactIdArray::NewLC(iDb->SortedItemsL());
	for(TInt mode=0;mode<2;mode++)
		{
		for(TInt loop=0;loop<aNumTimesToLoop;loop++)
			{
			TInt err=KErrNone;
			switch(mode)
				{
				case 0:
					{
					for(TInt idIndex=0;idIndex<idarray->Count();idIndex++)
						{
						CContactItem *openItem=NULL;
						TContactItemId id=(*idarray)[idIndex];
						TRAP(err,openItem=iDb->OpenContactL(id));
						CleanupStack::PushL(openItem);
						if (err==KErrNone)
							{
							TRAP(err,iDb->CommitContactL(*openItem));
							iDb->CloseContactL(id);
							}
						CleanupStack::PopAndDestroy();	// openItem
						openResults->ProcessErrorL(err,totalResults);
						}
					break;
					}
				case 1:
					{
					CVCardTestStore* store=NULL;
					TRAP(err,store=ExportContactsL(iDb,idarray,CContactDatabase::EExcludeUid,KVCardStoreTypeBuf,NULL,1000));
					exportResults->ProcessErrorL(err,totalResults);
					if (err==KErrNone)
						{
						CleanupStack::PushL(store);
						CArrayPtr<CContactItem>* items=NULL;
						TRAP(err,items=ImportContactsL(iDb,store,CContactDatabase::EExcludeUid));
						importResults->ProcessErrorL(err,totalResults);
						if (err==KErrNone)
							{
							CleanupStack::PushL(TCleanupItem(CleanUpResetAndDestroy,items));
							for(TInt delLoop=0;delLoop<items->Count();delLoop++)
								{
								TRAP(err,iDb->DeleteContactL((*items)[delLoop]->Id()));
								deleteResults->ProcessErrorL(err,totalResults);
								}
							CleanupStack::PopAndDestroy();	// items
							}
						CleanupStack::PopAndDestroy();	// store
						}
					}
					break;
				}
			TBuf<128> buf;
			buf.Format(_L("M:%d,C:%d "),mode,loop);
			totalResults.AppendResults(buf);
			User::InfoPrint(buf);
			}
		}
	CleanupStack::PopAndDestroy();	// idArray
	*aParams->iRemoteThreadFinished=ETrue;
	}

void CFred2::RunTestL()
	{
	switch(Func())
		{
		case EThreadFuncHoldOpenForever:
		case EThreadFuncHoldOpenForAWhile:
			{
			TContactItemId id=(*TheIds)[((TOpenBaseParams *)(Params()->Ptr()))->iParam1];
			CContactItem *item=iDb->OpenContactL(id);
			CleanupStack::PushL(item);
			if (Func()==EThreadFuncHoldOpenForAWhile)
				{
				User::After(1000000);
				iDb->CommitContactL(*item);
				CleanupStack::PopAndDestroy(); // item
				}
			else
				{
				TRequestStatus request;
				User::WaitForRequest(request);	// Wait forever
				}
			}
			break;
		case EThreadFuncHoldOpenRangeForAWhile:
		case EThreadFuncHoldOpenRangeForever:
			{
			TInt start=((TOpenBaseParams *)(Params()->Ptr()))->iParam1;
			TInt end=((TOpenBaseParams *)(Params()->Ptr()))->iParam2;
			CContactItem *items[KTotalNumRecords];
			for(TInt loop=start;loop<end;loop++)
				{
				TContactItemId id=(*TheIds)[loop];
				items[loop]=iDb->OpenContactL(id);
/*			Weird MARM Death
			TRAPD(x,items[loop]=iDb->OpenContactL(id));
			TBuf<32> buf;
			buf.Format(_L("oc-%d"),x);
			test.Printf(buf);*/
				CleanupStack::PushL(items[loop]);
				}
			if (Func()==EThreadFuncHoldOpenRangeForAWhile)
				{
				User::After(100000);
				for(TInt loop=start;loop<end;loop++)
					iDb->CommitContactL(*items[loop]);
				CleanupStack::PopAndDestroy(end-start);
				}
			else
				{
				TRequestStatus request;
				User::WaitForRequest(request);	// Wait forever
				}
			}
			break;
		case EThreadFuncKeepUpdatingContacts:
			{
			TOpenBaseParams *params=(TOpenBaseParams *)(Params()->Ptr());
			KeepUpdatingContactsL(params->iParam1,(SDbmsErrParams *)params->iParam2);
			}
			break;
		}
	}

void CFred2::DoTestL()
	{
	OpenDbEtcL();
	RunTestL();
	CloseDbEtcL();
	}

////////////////////////////////////////////////////////////////////////////////
// Test code for DEF022709  (Propagated) Contact DB Monitoring Error Messages //
////////////////////////////////////////////////////////////////////////////////

const TInt32	KNumContactsToAdd=50;
// If there are many contacts, it is worth using the #define below.
// This will compact the database when necessary.
#define __KEEP_DB_SMALL__

// CWaitForCompletion implementation
class CWaitForCompletion : public CActive
	{
	public:
		CWaitForCompletion() : CActive(EPriorityStandard)
			{
			CActiveScheduler::Add(this);
			}
		virtual ~CWaitForCompletion() {}

		void Activate() { SetActive(); }
		TRequestStatus& RequestStatus() { return iStatus; }
	protected:
		// From CActive
		virtual void DoCancel() { ; }
		virtual void RunL() { CActiveScheduler::Stop(); }
	};

class CTestMultipleRequests : public CBase
	{
    public:
        static CTestMultipleRequests* NewLC(TRequestStatus& aRequest);
        void ConstructL(TRequestStatus& aRequest);
        TContactItemId CreateTestContactL(const TDesC& aFamilyName, TBool aWithPhoneNumber);
		CTestMultipleRequests();
        ~CTestMultipleRequests();
	    TBool Synchronize();
		static TInt OnIdle(TAny* aObject);
		TInt DoIdle();
		void Complete();
	private:
//		CPbkContactEngine* iPbkEngine;
		CContactDatabase* iContactDatabase;
		CContactChangeNotifier* iContactChangeNotifier;
		TBool iRepeat;
		CContactIdArray* iContactDbIdArray;
		CIdle* iIdleTask;
		TInt iContactIdCount;
		TInt iCount;
		TInt iTimesThrough;
		TRequestStatus* iClientRequest;
		TBool iFinished;
	};

CTestMultipleRequests::CTestMultipleRequests()
	{
	}

CTestMultipleRequests::~CTestMultipleRequests()
	{
//	delete iPbkEngine;
    delete iContactChangeNotifier;
    delete iContactDbIdArray;
	delete iIdleTask;
	delete iContactDatabase;
	}

CTestMultipleRequests* CTestMultipleRequests::NewLC(TRequestStatus& aRequest)
	{
	CTestMultipleRequests* self=new(ELeave) CTestMultipleRequests();
	CleanupStack::PushL(self);
	self->ConstructL(aRequest);
	return self;
	}

void CTestMultipleRequests::ConstructL(TRequestStatus& aRequest)
	{
	iCount = 0;
    iTimesThrough = 0;
    iIdleTask = CIdle::NewL(-20);
    iRepeat = EFalse;
	iFinished=EFalse;
	iClientRequest=&aRequest;
	*iClientRequest=KRequestPending;
//    BaseConstructL();
//    iPbkEngine = CPbkContactEngine::NewL();

//    iContactDatabase = CContactDatabase::CreateL(KDatabaseFileName);
    iContactDatabase = CContactDatabase::ReplaceL(KDataBaseName);

	CRandomContactGenerator* randomGenerator=CRandomContactGenerator::NewL();
	CleanupStack::PushL(randomGenerator);
	randomGenerator->SetDbL(*iContactDatabase);
	for (TInt i=0; i<KNumContactsToAdd; i++)
		randomGenerator->AddTypicalRandomContactL();
	CleanupStack::PopAndDestroy(randomGenerator);
    
	const CContactIdArray* contactDbIdArray = iContactDatabase->SortedItemsL();
    iContactDbIdArray = CContactIdArray::NewL(contactDbIdArray);
    iContactIdCount = iContactDbIdArray->Count();

    User::After(15000000); // 15 seconds
    iIdleTask->Start(TCallBack(OnIdle,this));            // sync the VoCoS and Contact DB using CIdle
	}

TBool CTestMultipleRequests::Synchronize()
	{

	if (iTimesThrough == 5)
		return EFalse;
	if (iCount == iContactIdCount)
		{
		iCount = 0;
		iTimesThrough++;
		}
 	CContactItem* item = iContactDatabase->OpenContactLX((*iContactDbIdArray)[iCount]);
	CleanupStack::PushL(item);
	iContactDatabase->CommitContactL( *item ); 
	CleanupStack::PopAndDestroy(item);
	CleanupStack::PopAndDestroy(); // lock
#ifdef __KEEP_DB_SMALL__
	if (iContactDatabase->CompressRequired())
		iContactDatabase->CompactL(); // keep the database to a sensible size.
#endif
	iCount++;
	return ETrue;
	}


TInt CTestMultipleRequests::OnIdle(TAny* aObject)
	{
	TBool ret = ((CTestMultipleRequests*)aObject)->DoIdle();
	if (!ret)
		((CTestMultipleRequests*)aObject)->Complete();
	return ret;
	}

TInt CTestMultipleRequests::DoIdle()
	{
	TBool ret=EFalse;
	if (!iFinished)
		{
		ret = Synchronize();
		if (!ret)
			{
			iFinished=ETrue;
			}
		}
	return ret;
	}

void CTestMultipleRequests::Complete()
	{
    if (iIdleTask)
		{
        iIdleTask->Cancel();
		}
	User::RequestComplete(iClientRequest, KErrNone);
	}


void DoDEF022709TestL()
	{
	test.Start(_L("Test for defect DEF022709"));

	// Create test resources
	CWaitForCompletion* transactionWait=new (ELeave)CWaitForCompletion();
	CleanupStack::PushL(transactionWait);
	CTestMultipleRequests* res = CTestMultipleRequests::NewLC(transactionWait->RequestStatus());
	transactionWait->Activate();
	CActiveScheduler::Start();

	// Cleanup
    CleanupStack::PopAndDestroy(res);
	CleanupStack::PopAndDestroy(transactionWait);

    test.End();
	}
