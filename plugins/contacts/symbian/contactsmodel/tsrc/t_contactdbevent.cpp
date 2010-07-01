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
#include <cntdb.h>
#include <cntitem.h>
#include <cntfield.h>
#include <cntfilt.h>
#include <cntfldst.h>

RTest test(_L("t_contactdbevent"));

_LIT(KEventDbFileName,"c:t_contactdbevent.cdb");
const TInt KTimeMicroSec = 1000000;


class CPbObserver : public CTimer, public MContactDbObserver 
	{
public:
	static CPbObserver* NewL(TUint aConnectionId);
	void ConstructL();
	~CPbObserver(){}
public:
	//From MContactDbObserver
	void HandleDatabaseEventL(TContactDbObserverEvent aEvent);
protected:
	CPbObserver(TUint aConnectionId);
	void RunL();
protected:
	TUint iPbConnectionId;	
	};

class CPbTester : public CBase 
	{
public:
	static CPbTester* NewL();
	~CPbTester();
	void StartTestL();
protected:
	void ConstructL();
	TContactItemId AddEntryL();
	void EditEntryL(TContactItemId itemId);

protected:
	CPbObserver* ipbObSvr;
	CContactDatabase* ipbDb;
	CContactChangeNotifier* ipbNotifier;	
	};

CPbObserver* CPbObserver::NewL(TUint aConnectionId)
	{
	CPbObserver* self = new (ELeave) CPbObserver(aConnectionId);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

void CPbObserver::ConstructL()
	{
	CTimer::ConstructL();
	}

void CPbObserver::HandleDatabaseEventL(TContactDbObserverEvent aEvent)
	{
	if (aEvent.iType == EContactDbObserverEventContactChanged)
		{
		test.Printf(_L("Received a Contact changed event"));
		}
	if (aEvent.iType == EContactDbObserverEventContactDeleted)
		{
		test.Printf(_L("Received a Contact delete event"));
		}
	if (aEvent.iType == EContactDbObserverEventContactAdded) 
		{
		test.Printf(_L("Received a Contact added event"));
		}
	test (aEvent.iConnectionId == iPbConnectionId);
	}


CPbObserver::CPbObserver(TUint aConnectionId): CTimer(EPriorityStandard), 
                                               iPbConnectionId(aConnectionId)
	{
	CActiveScheduler::Add(this);
	}

void CPbObserver::RunL()
	{
	CActiveScheduler::Stop();
	}

CPbTester* CPbTester::NewL(void)
	{
	CPbTester *self = new(ELeave)CPbTester;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

void CPbTester::ConstructL(void)
	{
	ipbDb=CContactDatabase::ReplaceL(KEventDbFileName);
	ipbObSvr = CPbObserver::NewL(ipbDb->ConnectionId());
	ipbNotifier = CContactChangeNotifier::NewL(*ipbDb, ipbObSvr);
	}

CPbTester::~CPbTester(void)
	{
	TRAP_IGNORE(ipbDb->DeleteDatabaseL(KEventDbFileName));
	delete ipbNotifier;
	delete ipbObSvr;
	delete ipbDb;
	}

void CPbTester::StartTestL()
	{
	TContactItemId id = AddEntryL();
	EditEntryL(id);
	ipbObSvr->After(KTimeMicroSec); //Allow CContactChangeNotifier to run
	CActiveScheduler::Start();
	}

TContactItemId CPbTester::AddEntryL()
	{
	_LIT(KForename,"John"); 
	_LIT(KSurname,"Smith"); 
	_LIT(KPhoneNumber,"+441617779700"); 

	// Create a  contact card to contain the data
	CContactCard* newCard = CContactCard::NewLC();

	// Create the firstName field and add the data to it
	CContactItemField* firstName = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldGivenName);
	firstName->TextStorage()->SetTextL(KForename);
	newCard->AddFieldL(*firstName);
	CleanupStack::Pop(firstName);

	// Create the lastName field and add the data to it
	CContactItemField* lastName= CContactItemField::NewLC(KStorageTypeText, KUidContactFieldFamilyName);
	lastName ->TextStorage()->SetTextL(KSurname);
	newCard->AddFieldL(*lastName);
	CleanupStack::Pop(lastName);

	// Create the phoneNo field and add the data to it
	CContactItemField* phoneNumber = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldPhoneNumber);
	phoneNumber->SetMapping(KUidContactFieldVCardMapTEL);
	phoneNumber ->TextStorage()->SetTextL(KPhoneNumber);
	newCard->AddFieldL(*phoneNumber);
	CleanupStack::Pop(phoneNumber);

	// Add newCard to the database
	const TContactItemId contactId = ipbDb->AddNewContactL(*newCard);

	CleanupStack::PopAndDestroy(newCard);

	return contactId;
	}

void CPbTester::EditEntryL(TContactItemId itemId)
	{
	_LIT(KEmailAddress,"john.smith@symbian.com"); 
	CContactItem *item = ipbDb->OpenContactL(itemId);
	CContactCard* card = NULL;
	CleanupStack::PushL(item);

	card = (CContactCard*)item;
	// Create the emailAddress field and add the data to it
	CContactItemField* emailAddr = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldEMail);
	emailAddr->SetMapping(KUidContactFieldVCardMapEMAILINTERNET);
	emailAddr ->TextStorage()->SetTextL(KEmailAddress);
	card->AddFieldL(*emailAddr);
	CleanupStack::Pop(emailAddr);

	ipbDb->CommitContactL(*item);
	CleanupStack::PopAndDestroy(item);
	}

void RunTestL()
	{
	CPbTester* tester = CPbTester::NewL();
	CleanupStack::PushL(tester);
	tester->StartTestL();
	CleanupStack::PopAndDestroy(tester);
	}

/**

@SYMTestCaseID     PIM-T-CONTACTDBEVENT-0001

*/

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CActiveScheduler* scheduler=new CActiveScheduler;
	CActiveScheduler::Install(scheduler);
	CTrapCleanup* cleanup = CTrapCleanup::New();
	test.Start(_L("@SYMTESTCaseID:PIM-T-CONTACTDBEVENT-0001 Testing Database event"));

	TRAPD(err,RunTestL());
	test(err==KErrNone);
	test.End();
	test.Close();
	delete cleanup;
    delete scheduler;
	__UHEAP_MARKEND;
	return KErrNone;
	}
