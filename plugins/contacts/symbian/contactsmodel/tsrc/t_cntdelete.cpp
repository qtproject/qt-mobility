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


#include <e32test.h>
#include "t_utils.h"

CCntTest* CntTest=NULL;
LOCAL_D RTest test(_L("T_CntDelete"));

const TPtrC KDatabaseFileName=_L("C:T_CNTDELETE");

const TPtrC KTestName=_L("Test Name No%d");
const TPtrC KTestAddress=_L("Address %d");

const TInt KTotalNumRecordsFirstUnknown  = 10;
const TInt KTotalNumRecordsSecondUnknown = 1;
const TInt KTotalNumRecordsSingle  = 1;

const TInt KTotalUnknownEvents = 2;
const TInt KTotalDeleteEvents  = KTotalNumRecordsSingle;
const TInt KTotalAddedEvents = KTotalNumRecordsFirstUnknown + KTotalNumRecordsSecondUnknown + KTotalNumRecordsSingle;

static const TInt KTimeout = 3000000; 

TInt gUnknownChangesEvent = 0;
TInt gAddedContactsEvent = 0;
TInt gDeleteContactsEvent = 0;
TInt gInvalidEvents = 0;

class CMyObserver :  public CActive, public MContactDbObserver
		{
	public:
		CMyObserver() : CActive(EPriorityIdle)
			{
			iStart.UniversalTime();
			}
		void Activate();
				
	private: // From CActive.
		void RunL();
		void DoCancel();
		TInt RunError(TInt aError);

	private: // From MContactDbObserver
		void HandleDatabaseEventL(TContactDbObserverEvent aEvent);
		
	private:
		TTime 	iStart;
		TTime 	iEnd;
		};

void CMyObserver::RunL()
	{
	iEnd.UniversalTime();
	// Timeout has been excedeed, stop waiting
	if( iEnd.MicroSecondsFrom( iStart.Int64() ) > KTimeout )
		{		
		CActiveScheduler::Stop();
		}
	else // Waiting for next event
		{
		Activate();
		}
	}

void CMyObserver::DoCancel()
	{
	}

TInt CMyObserver::RunError(TInt aError)
	{
	test.Printf( _L("CMyObserver:: Error in runL: %d"), aError );
	return aError;
	}

void CMyObserver::Activate()
	{
	if(!IsActive())
		{
		TRequestStatus *pS=&iStatus;
		User::RequestComplete(pS,KErrNone);
		SetActive();
		}
	}


void CMyObserver::HandleDatabaseEventL(TContactDbObserverEvent aEvent)
	{  
	switch( aEvent.iType )
		{	
		case EContactDbObserverEventUnknownChanges:
			{
			gUnknownChangesEvent++;
			test.Printf( _L("Unknown Changes Event: id= %d\n"), aEvent.iContactId); 
			Activate();	
			break;
			}		
		case EContactDbObserverEventContactAdded:
			{
			gAddedContactsEvent++;
			test.Printf( _L("Added: id= %d\n"), aEvent.iContactId );
			Activate();			
			break;
			}
		case EContactDbObserverEventContactDeleted:
		    {
		    gDeleteContactsEvent++;
		    test.Printf( _L("Deleted: id= %d\n"), aEvent.iContactId );
		    Activate();	
		    break;
		    }
		default:
			{
			gInvalidEvents++;
			test.Printf( _L("Invalid event: %d, contact id: %d"), aEvent.iType, aEvent.iContactId );
			Activate();
			break;
			}
		}
	}


LOCAL_C void SetNameL(CContactItem& aItem,TUid aType,const TDesC& aName, TBool aAddField)
	{
	CContactItemFieldSet& fieldSet=aItem.CardFields();
	const TInt pos=fieldSet.Find(aType);
	if (!aAddField && pos!=KErrNotFound)
		fieldSet[pos].TextStorage()->SetTextL(aName);
	else
		{
		CContactItemField* field=CContactItemField::NewLC(KStorageTypeText,aType);
   		field->SetMapping(KUidContactFieldVCardMapUnusedN);
		field->TextStorage()->SetTextL(aName);
		aItem.AddFieldL(*field);
		CleanupStack::Pop(); // field
		}
	}


LOCAL_C void PopulateDatabaseL(TBool aPhoneNumbers, TInt aCount)
	{
	for (TInt ii=0;ii<aCount;ii++)
		{
		CContactItem* item=CContactCard::NewLC();
		TBuf<16> name;
		name.Format(KTestName,ii);
   		SetNameL(*item,KUidContactFieldFamilyName,name,ETrue);
		if (aPhoneNumbers)
			{
			TBuf<20> number;
			switch(ii%3)
				{
				case 0:
					number.Format(_L("0171-%03d %04d"),(ii*9)%1000,((ii+11)*23)%10000);
					break;
				case 1:
					number.Format(_L("%04d:%04d:%04d:%04d"),(ii*123)%10000,(ii*666)%10000,(ii*234)%10000);
					break;
				case 2:
					number.Format(_L("+00%d-%03d %04d"),(ii*123)%100,(ii*13)%1000,((ii+13)*17)%10000);
					break;
				}
   			SetNameL(*item,KUidContactFieldPhoneNumber,number,ETrue);
			if (!(ii%2))
				{
				number.Format(_L("0181-%03d %04d"),(ii*8)%1000,((ii+11)*22)%10000);
	   			SetNameL(*item,KUidContactFieldPhoneNumber,number,ETrue);
				number.Format(_L("01734-%06d"),(ii*123456)%1000000);
	   			SetNameL(*item,KUidContactFieldPhoneNumber,number,ETrue);
				}
			}
		if (ii%2)
			{
			TBuf<16> address;
			address.Format(KTestAddress,ii);
			CContactItemField* field=CContactItemField::NewLC(KStorageTypeText,KUidContactFieldAddress);
			field->SetMapping(KUidContactFieldVCardMapADR);
			field->TextStorage()->SetText(address.AllocL());
			field->SetTemplateField(ETrue);
			item->AddFieldL(*field);
			CleanupStack::Pop(); // field
			}
		if (ii%3)
			{
			CContactItemField* field=CContactItemField::NewLC(KStorageTypeDateTime,KUidContactFieldBirthday);
			field->DateTimeStorage()->SetTime(TDateTime(1990+ii,(TMonth)(ii%12),(ii*3)%28,1,2,3,4));
			field->SetTemplateField(ETrue);
			item->AddFieldL(*field);
			CleanupStack::Pop(); // field
			}
		if (ii%4)
			{
			CContactItemField* field=CContactItemField::NewLC(KStorageTypeStore,KUidContactFieldVCardMapAGENT);
    		field->SetMapping(KUidContactFieldVCardMapAGENT);
			field->SetTemplateField(ETrue);
			item->AddFieldL(*field);
			CleanupStack::Pop(); // field
			}
		if (ii%6)
			{
			CContactItemField* field=CContactItemField::NewLC(KStorageTypeContactItemId);
			field->SetMapping(KUidContactFieldVCardMapAGENT);
			field->SetTemplateField(ETrue);
			item->AddFieldL(*field);
			CleanupStack::Pop(); // field
			}
		CntTest->Db()->AddNewContactL(*item);
		CleanupStack::PopAndDestroy(); // item
		}
	CntTest->Db()->SetDateFormatTextL(_L("%E%D%X%N%Y %1 %2 %3"));
	}


LOCAL_C void WaitForNotificationsL(CContactDatabase &aDb)
	{
	CMyObserver *observe = new (ELeave) CMyObserver();
	CleanupStack::PushL( observe );
	CContactChangeNotifier *notifier = CContactChangeNotifier::NewL(aDb, observe);
	CleanupStack::PushL( notifier );
	CActiveScheduler::Add(observe);
	observe->Activate();
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(notifier);
	CleanupStack::PopAndDestroy(observe);
	}
		
	
LOCAL_C void DeleteMultipleContactsL()
	{
	CContactIdArray* idArray = CntTest->Db()->ContactsChangedSinceL(TTime(0));
	CleanupStack::PushL(idArray);
	CntTest->Db()->DeleteContactsL(*idArray);		
	CleanupStack::PopAndDestroy(idArray);
	}
	
	
LOCAL_C void DeleteOneContactL()
    {
    CContactIdArray* idArray = CntTest->Db()->ContactsChangedSinceL(TTime(0));
	CleanupStack::PushL(idArray);
	if (idArray->Count() == 1)
	    {	    
	    CntTest->Db()->DeleteContactL((*idArray)[0]);		
	    }
	CleanupStack::PopAndDestroy(idArray);
    }
	

/**
@SYMTestCaseID PIM-T-CNTDELETE-0001
@SYMTestType UT
@SYMTestPriority Medium
@SYMDEF DEF103603
@SYMTestCaseDependencies CntModel CoreAppsTest
@SYMTestCaseDesc Check that deleting multiple contacts in one go will raise a EContactDbObserverEventUnknownChanges 
@SYMTestActions Create many contacts. Delete all and wait for the event. Create one contact
@SYMTestExpectedResults EContactDbObserverEventUnknownChanges should be the only 
event that will be sent to the client when deleting the contacts.
*/
	
void DoTestsL()
    {  
	test.Start(_L("@SYMTESTCaseID:PIM-T-CNTDELETE-0001 Create new database"));

	CntTest->CreateDatabaseL();	

    // Create many contacts, delete all, wait for Unknown Changes Event
	TRAPD(err,PopulateDatabaseL(ETrue, KTotalNumRecordsFirstUnknown));
	test(err==KErrNone);
	WaitForNotificationsL(*CntTest->Db());				
	DeleteMultipleContactsL();
	WaitForNotificationsL(*CntTest->Db());	
		
	// Create one contact, delete it, wait for Unknown Changes Event
	TRAP(err,PopulateDatabaseL(ETrue, KTotalNumRecordsSecondUnknown));
	test(err==KErrNone);
	WaitForNotificationsL(*CntTest->Db());				
	DeleteMultipleContactsL();
	WaitForNotificationsL(*CntTest->Db());		
	
	// Create one contact, delete it, wait for Contact Deleted Event
	TRAP(err,PopulateDatabaseL(ETrue, KTotalNumRecordsSingle));
	test(err==KErrNone);
	WaitForNotificationsL(*CntTest->Db());				
	DeleteOneContactL();
	WaitForNotificationsL(*CntTest->Db());		
	
	CntTest->CloseDatabase();
	test.Next(_L("Delete database"));

	CntTest->DeleteDatabaseL();
	
	test(
	gUnknownChangesEvent == KTotalUnknownEvents && 
	gAddedContactsEvent  == KTotalAddedEvents   &&
	gDeleteContactsEvent == KTotalDeleteEvents  &&
	gInvalidEvents       == 0
	);
	
	test.Printf( _L("Test complete!\n") );
    }
    

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
    CntTest=new(ELeave) CCntTest;
	CntTest->ConstructL(test,KDatabaseFileName);
    TRAPD(err,DoTestsL());
	CntTest->EndTestLib(err);	
	__UHEAP_MARKEND;
	return KErrNone;
    }
