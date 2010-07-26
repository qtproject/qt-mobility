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
#include <bautils.h>
#include <barsc.h>
#include <barsread.h>
#include <cntmodel.rsg>
#include "t_utils.h"

CCntTest* CntTest=NULL;
LOCAL_D RTest test(_L("T_OWNCAR"));

const TPtrC KDatabaseFileName=_L("C:T_OWNCAR");

const TPtrC KTestName=_L("Test Name No%d");
const TPtrC KTestAddress=_L("Address %d");
const TInt KTotalNumRecords=50;
const TInt KTotalNumOwnCards=1;


#define KCardGivenName _L("Given Name #%d")
#define KCardFamilyName _L("Family Name #%d")
#define KCardFieldText _L("Card id %d, text field #%d")
#define KCardFieldEditedText _L("UPDATED Card id %d, text field #%d")

static const TInt KTimeout = 20000000;//20 seconds timeout

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

	private: // from MContactDbObserver
		void HandleDatabaseEventL(TContactDbObserverEvent aEvent);
		
	private:
		TTime 	iStart;
		TTime 	iEnd;
		};

void CMyObserver::RunL()
	{
	iEnd.UniversalTime();
	//Timeout has been excedeed, stop waiting
	if( iEnd.MicroSecondsFrom( iStart.Int64() ) > KTimeout )
		{
		test.Printf( _L("Observer has timed out, EContactDbObserverEventOwnCardChanged notification not received"));
		CActiveScheduler::Stop();
		}
	else //waiting for next event
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
		case EContactDbObserverEventOwnCardChanged:
			{
			test.Printf( _L("Own card changed event id: %d, contact id: %d"), aEvent.iType, aEvent.iContactId );
			Cancel();
			CActiveScheduler::Stop();
			break;
			}
		default:
			{
			test.Printf( _L("Other event id: %d, contact id: %d"), aEvent.iType, aEvent.iContactId );
			Activate();
			break;
			}
		}
	}

GLDEF_C void Panic(TInt aPanic)
//
// Panic the thread with CNTMODEL as the category
//
	{
	User::Panic(_L("T_OWNCAR"),aPanic);
	}

LOCAL_C void SetNameL(CContactItem& aItem,TUid aType,const TDesC& aName, TBool aAddField)
//
// Set the contents of a text field, creating the field if required
//
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

LOCAL_C void PopulateDatabaseL(TBool aPhoneNumbers)
//
// Create and populate the database
//
	{
	for (TInt ii=0;ii<KTotalNumRecords;ii++)
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



//////////////////////////////////////////////////////////////////////
//
//	main test modules
//
//////////////////////////////////////////////////////////////////////


LOCAL_C void CheckOwnCardCreatedL()
//
// make sure own card is the only card in db
//
	{
	test.Next(_L("Check Own Card Created"));

	CContactItem* newOwnCard = CntTest->Db()->CreateOwnCardL();
	delete newOwnCard;
	test(CntTest->Db()->CountL()==KTotalNumOwnCards);
	// check own card uid is set
	if (CntTest->Db()->OwnCardId()==NULL)
		User::Leave(KErrNotFound);
	CContactItem* tempOwnCard = CntTest->Db()->ReadContactL(CntTest->Db()->OwnCardId());
	delete tempOwnCard;
	}

LOCAL_C void SetOwnCardL(TInt aUid)
//
//  set an existing card to be new own card
//
	{
	test.Next(_L("Set Own Card"));

	CContactItem* tempCard = CntTest->Db()->ReadContactL(aUid);
	CleanupStack::PushL(tempCard);
	CntTest->Db()->SetOwnCardL(*tempCard);
	if(CntTest->Db()->OwnCardId()!=tempCard->Id())
		User::Leave(KErrNotFound);
	CleanupStack::PopAndDestroy(); // tempCard
	}


LOCAL_C void SetBadOwnCardL()
//
//	try to set a bad card as the new own card
//
	{
	test.Next(_L("Set Bad Own Card"));

	CContactItem* badOwnCard=CContactCard::NewLC();
	CntTest->Db()->SetOwnCardL(*badOwnCard);
	CleanupStack::PopAndDestroy(badOwnCard);
	}


LOCAL_C void GetOwnCardL(TInt aUid)
//
//	check own card persistence
//
	{
	test.Next(_L("Check Persistence"));

	if(CntTest->Db()->OwnCardId()!=aUid)
		User::Leave(KErrNotFound);
	CContactItem* tempCard = CntTest->Db()->ReadContactL(aUid);
	delete tempCard;
	}


LOCAL_C void DeleteOwnCardL()
//
//	delete the own card & check persistence
//
	{
	test.Next(_L("Delete Own Card"));

	CntTest->Db()->DeleteContactL(CntTest->Db()->OwnCardId());

	CntTest->CloseDatabase();
	CntTest->OpenDatabaseL();
	
	// ensure id is reset.
	if (CntTest->Db()->OwnCardId()!=KNullContactId)
		User::Leave(KErrNotFound);
	}


LOCAL_C void SetGoodOwnCardL(TInt aUid)
//
//	after deleting own card set a new one
//
	{
	test.Next(_L("Set Good Own Card"));

	// ownCard should be null.
	CContactItem* tempCard = CntTest->Db()->ReadContactL(aUid);
	CntTest->Db()->SetOwnCardL(*tempCard);
	delete tempCard;
	}

LOCAL_C void CreateOwnCardTwiceL()
//
//	Create own card twice and see ok
//
	{
	test.Next(_L("Create Own Card Twice"));


	CContactItem* newOwnCard = CntTest->Db()->CreateOwnCardL();
	CContactItem* newOwnCard2 = CntTest->Db()->CreateOwnCardL();
	delete newOwnCard;
	delete newOwnCard2;
	}

LOCAL_C void SetContactFieldsL(CContactDatabase &aDb, const TContactItemId aId)
	{
	static TInt i = 0;
	CContactItem *citem = aDb.OpenContactLX(aId);
	CleanupStack::PushL( citem );
	TBuf<16> name;
	name.Format(KTestName,++i);
	SetNameL(*citem,KUidContactFieldFamilyName,name,EFalse);
	SetNameL(*citem,KUidContactFieldPhoneNumber,name,EFalse);
	SetNameL(*citem,KUidContactFieldGivenName,name,EFalse);
	SetNameL(*citem,KUidContactFieldAdditionalName,name,EFalse);
	SetNameL(*citem,KUidContactFieldEMail,name,EFalse);
	SetNameL(*citem,KUidContactFieldCompanyName,name,EFalse);
	SetNameL(*citem,KUidContactFieldAddress,name,EFalse);
	aDb.CommitContactL(*citem);
	CleanupStack::PopAndDestroy(citem);
	CleanupStack::Pop();//lock
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
	
LOCAL_C void SetOwnCardL(CContactDatabase &aDb, const TContactItemId aId)
	{
	test.Printf(_L("Change owncard from: %d to: %d"), aDb.OwnCardId(), aId);
	CContactItem *citem = aDb.ReadContactLC(aId);
	aDb.SetOwnCardL( *citem );
	CleanupStack::PopAndDestroy(citem);
	}
 
/**
@SYMTestCaseID     PIM-T-OWNCAR-0002
@SYMTestType UT
@SYMTestPriority Medium
@SYMDEF DEF085699
@SYMTestCaseDesc 
Multiple Client Owncard Test
1) Create a contact and set it as the own card
2) Create another contact
3) Set the new contact to be the own card
4) Modify the first contact created (i.e not the one currently set to the own card)
5) View result of a CContactDatabase::OwnCardId() call in both the applications

@SYMTestActions 
Create Sessions A and B to the database
add two contacts using session A
Set contact 1 to own card using session A
Set contact 2 to own card using session A
Update Contact 1 using session A

@SYMTestExpectedResults 
Owncard should equal contact 2 in for both sessions
*/

LOCAL_C void MultipleClientOwncardTestL()
	{
	test.Next(_L("@SYMTESTCaseID:PIM-T-OWNCAR-0002 Multiple Client OwnCard Test"));

	CContactDatabase *db = CContactDatabase::ReplaceL( KDatabaseFileName );
	CContactDatabase *db2 = CContactDatabase::OpenL( KDatabaseFileName );
	CleanupStack::PushL( db );
	CleanupStack::PushL( db2 );
	CContactItemViewDef *viewAll = CContactItemViewDef::NewLC(CContactItemViewDef::EIncludeFields,CContactItemViewDef::EIncludeHiddenFields);
	viewAll->AddL(KUidContactFieldMatchAll);
	// Retrieve the system template
	CContactTemplate* ttemplate = static_cast< CContactTemplate* >
												( db->ReadContactLC( db->TemplateId(), *viewAll ) );
	// Create a new contact based on the system template
	CContactCard* contact = CContactCard::NewLC(ttemplate);
	
	// Add the new contact to the db
	TContactItemId id1 = db->AddNewContactL(*contact);
	SetContactFieldsL(*db,id1);
	
	test(db->OwnCardId() == db2->OwnCardId());
	SetOwnCardL(*db, id1);
	WaitForNotificationsL(*db2);

	TContactItemId oid1 = db->OwnCardId();
	TContactItemId oid2 = db2->OwnCardId();
	test( (oid1 == oid2) && (oid1 == id1) );
	
	test.Printf(_L("Add new contact; check that it is different from existing contacts"));
	TContactItemId id2 = db->AddNewContactL(*contact);
	SetContactFieldsL(*db,id2);
	test(id1 != id2);
	
	SetOwnCardL(*db, id2);
	WaitForNotificationsL(*db2);

	oid1 = db->OwnCardId();
	oid2 = db2->OwnCardId();
	test( (oid1 == oid2) && (oid1 == id2) );
	
	test.Printf(_L("modify contact 1, check that its not marked as an own card by either session"));
	SetContactFieldsL(*db,id1);
	WaitForNotificationsL(*db2);
	oid1 = db->OwnCardId();
	oid2 = db2->OwnCardId();
	test( (oid1 == oid2) && (oid1 == id2) );
	
	
	CleanupStack::PopAndDestroy(contact);
	CleanupStack::PopAndDestroy(ttemplate);
	CleanupStack::PopAndDestroy(viewAll);
	CleanupStack::PopAndDestroy(db2);
	CleanupStack::PopAndDestroy(db);
	}

/**

@SYMTestCaseID     PIM-T-OWNCAR-0001

*/

void DoTestsL()
    {
    MultipleClientOwncardTestL();
	test.Start(_L("@SYMTESTCaseID:PIM-T-OWNCAR-0001 Create new database"));

	TRAPD(err,CntTest->CreateDatabaseL());
	test(err==KErrNone);
	CntTest->Db()->OwnCardId();
	CntTest->CloseDatabase();
	CntTest->OpenDatabaseL();
	CntTest->Db()->OwnCardId();
	TRAP(err,CheckOwnCardCreatedL());
	test(err==KErrNone);
	CntTest->Db()->DeleteContactL(CntTest->Db()->OwnCardId());
	CntTest->CloseDatabase();
	CntTest->OpenDatabaseL();
	TRAP(err,PopulateDatabaseL(ETrue));
	test(err==KErrNone);
	TRAP(err,SetOwnCardL(CntTest->Db()->CountL()-2));
	test(err==KErrNone);
	TRAP(err,GetOwnCardL(CntTest->Db()->CountL()-2));
	test(err==KErrNone);
	TRAP(err,SetBadOwnCardL());
	test(err==KErrNotFound);
	CntTest->CloseDatabase();
	CntTest->OpenDatabaseL();
	TRAP(err,DeleteOwnCardL());
	test(err==KErrNone);
	TRAP(err,SetGoodOwnCardL(CntTest->Db()->CountL()-15));
	test(err==KErrNone);
	TRAP(err,CreateOwnCardTwiceL());
	test(err==KErrNone);
	CntTest->CloseDatabase();
	test.Next(_L("Delete database"));

	CntTest->DeleteDatabaseL();
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
