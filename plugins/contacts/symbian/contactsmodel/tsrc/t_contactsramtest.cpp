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
#include <cntviewbase.h>
#include <cntfldst.h>
#include <e32panic.h>

_LIT(KTestName, "Ram consumption Test");

LOCAL_D RTest test(KTestName);

#define DELETE_SET_NULL(x) { delete x; x = NULL; }

static const TInt KMaxContacts = 2000;
static const TInt KManyFields = 8;
static const TInt KAgentId = 2;
_LIT(KTime,"22000101:235959.999999");// time to use for datetime storage
_LIT(KFieldValue,"Standard length field value");
TInt manycontacts = 0;

_LIT(KContactsIniFileFullPath, "c:\\private\\10003A73\\Cntmodel.ini");
_LIT(KContactsDbFileFullPath, "c:\\private\\10003A73\\Contacts.cdb");
_LIT(KContactsBackupRegistrationIniFileFullPath, "c:\\private\\10003A73\\backup_registration.xml");

static RFs TheFsSession;

class CContactsRamTest : public CActive, public MContactViewObserver
	{
public:
	static CContactsRamTest* NewLC();
	~CContactsRamTest();
	void SetView(const CContactViewBase *aLocalView);
	void RamTestL();
	void RamTest2L();
	void Activate();
	void CloseView();
	
private:
	CContactsRamTest();
	void ConstructL();
	void AddContactsL(const TInt aNumber);
	void SetFieldL(const CContactItemField &aField, const TDesC& aText);
	void SetManyFieldsL(const TDesC& aText, const TInt aMany, CContactItemFieldSet &fields);
	void PrintHeap();
	
private: // From CActive.
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);
private: // From MContactViewObserver.
	virtual void HandleContactViewEvent(const CContactViewBase& aView, 
										const TContactViewEvent& aEvent);

private:
	CContactDatabase* iContactsDatabase;
	CContactRemoteView* iRemoteView;
	RContactViewSortOrder iViewSortOrder;
	TBool iEnd;
	};


CContactsRamTest* CContactsRamTest::NewLC()
	{
	CContactsRamTest* self = new(ELeave) CContactsRamTest();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
	

void CContactsRamTest::ConstructL()
	{
	CActiveScheduler::Add(this);
	iViewSortOrder.AppendL(KUidContactFieldGivenName);	
	iViewSortOrder.AppendL(KUidContactFieldFamilyName);
	}
	

CContactsRamTest::~CContactsRamTest()
	{
	CloseView();
	iViewSortOrder.Close();
	delete iContactsDatabase;
	}

CContactsRamTest::CContactsRamTest() : CActive(EPriorityStandard)
	{
	}
	
	
void CContactsRamTest::RunL()
	{
	if( iEnd )
		{
		iEnd = EFalse;
		RamTest2L();
		CActiveScheduler::Stop();
		}
	else
		{
		RamTestL();
		}
	}
	
void CContactsRamTest::DoCancel()
	{
	}

TInt CContactsRamTest::RunError(TInt aError)
	{
	test.Printf( _L("ViewObserver:: Error in doTest runL: %d"), aError );
	return aError;
	}

void CContactsRamTest::Activate()
	{
	TRequestStatus *pS=&iStatus;
	User::RequestComplete(pS,KErrNone);
	SetActive();
	}

void CContactsRamTest::RamTestL()
	{
	test.Next(_L("Initial ram test"));

	test.Printf( _L("Ram test for %d contacts\n"), manycontacts);
	DELETE_SET_NULL( iContactsDatabase );
	test.Printf(_L("Empty Database is opened\n"));
	iContactsDatabase = CContactDatabase::ReplaceL();
	PrintHeap();
	AddContactsL( manycontacts );
	test.Printf(_L("Database is opened and contacts added\n"));
	PrintHeap();
	test.Printf(_L("RemoteView with first/surname order is created\n"));
	iRemoteView = CContactRemoteView::NewL(	*this, *iContactsDatabase, iViewSortOrder, EContactsOnly);
	PrintHeap();
	
	}	

void CContactsRamTest::RamTest2L()
	{
	test.Printf(_L("RemoteView is populated\n"));
	PrintHeap();
	CloseView();
	DELETE_SET_NULL( iContactsDatabase );
	test.Printf(_L("Full Database is re-opened\n"));
	iContactsDatabase = CContactDatabase::OpenL();
	PrintHeap();
	DELETE_SET_NULL( iContactsDatabase );
	}	
	
void CContactsRamTest::HandleContactViewEvent(	const CContactViewBase& aView, 
											const TContactViewEvent& aEvent)
	{
	if (&aView==iRemoteView)
		{
		switch(aEvent.iEventType)
			{
			case TContactViewEvent::EReady:
				{				
				iEnd = ETrue;
				Activate();
				break;
				}
			}
		}
	}
	
void CContactsRamTest::AddContactsL(const TInt aNumber)
/**
Add number of contacts to db, using the system template.
@return None
@param aNumber number of contacts to be added
@pre None
@post database now contains aNumber of empty contacts
*/
	{
	

	//viewdef that loads all fields 	
	CContactItemViewDef *viewAll = CContactItemViewDef::NewLC(CContactItemViewDef::EIncludeFields,CContactItemViewDef::EIncludeHiddenFields);
	viewAll->AddL(KUidContactFieldMatchAll);
	CContactTemplate *ttemplate = static_cast< CContactTemplate* >(
	iContactsDatabase->ReadContactLC( iContactsDatabase->TemplateId(), *viewAll ) );

	CContactCard* contact = CContactCard::NewLC(ttemplate);
	SetManyFieldsL(KFieldValue, KManyFields, contact->CardFields());
	for(TInt k = 0; k<aNumber; k++)
		{		
		iContactsDatabase->AddNewContactL(*contact);
		}	
	CleanupStack::PopAndDestroy(contact);
	CleanupStack::PopAndDestroy(ttemplate);
	CleanupStack::PopAndDestroy(viewAll);
	}

/*sets a specific contact field to a value as determined by its type.
field at aPos in aFields is set to aText*/
void CContactsRamTest::SetFieldL(const CContactItemField &aField, const TDesC& aText)
	{
	if( aField.IsTemplateLabelField() )
		{
		return;
		}
		
	if( 0 == aText.Size() )
		{
		return;
		}
	
	switch(aField.StorageType())
		{
		case KStorageTypeText:
			aField.TextStorage()->SetTextL(aText);
		break;
		case KStorageTypeStore:
			{
			HBufC8 *buf = HBufC8::NewLC(aText.Size());
			TPtr8 tptr8 = buf->Des();
			tptr8.Copy(aText);
			aField.StoreStorage()->SetThingL(*buf);
			CleanupStack::PopAndDestroy(buf);
			}
		break;
		case KStorageTypeContactItemId:
			{
			aField.AgentStorage()->SetAgentId( KAgentId );
			}
		break;
		case KStorageTypeDateTime:
			{
			if( aText.Size() > 0 ) 
				{
				aField.DateTimeStorage()->SetTime( TTime(KTime) );
				}
			else
				{
				aField.DateTimeStorage()->SetTime( TTime(0) );
				}
			}
		break;
		default:
			User::Panic(aText,EInvariantFalse);
		break;
		}
	}

/**
sets aMany fields of a currently read / open item to aText
*/
void CContactsRamTest::SetManyFieldsL(const TDesC& aText, const TInt aMany, CContactItemFieldSet &fields)
	{
	TInt count = fields.Count();
	if( count > aMany )
		{
		count = aMany;
		}
	TInt i = 0;
	for(; i < count; ++i)
		{
		SetFieldL(fields[i],aText);
		}
	}
	
void CContactsRamTest::PrintHeap()
	{
	TInt totalAllocSize(0);
	User::Heap().AllocSize(totalAllocSize);
	
	test.Printf(_L("Client heap size: %d bytes\n"), totalAllocSize);
	iContactsDatabase->CntServerResourceCount();
	}

void CContactsRamTest::CloseView()
	{
	if(iRemoteView)
		{
		iRemoteView->Close(*this);
		iRemoteView = NULL;
		}
	}

void CleanupFiles()
	{
	TheFsSession.Delete(KContactsIniFileFullPath);
	TheFsSession.Delete(KContactsDbFileFullPath);
	TheFsSession.Delete(KContactsBackupRegistrationIniFileFullPath);
	}

/**

@SYMTestCaseID     PIM-T-CONTACTSRAMTEST-0001

*/
	
void DoTestsL()
    {
    test.SetLogged( ETrue );
	test.Start(_L("@SYMTESTCaseID:PIM-T-CONTACTSRAMTEST-0001 Test Ram usage for contacts model"));

	
	test.Printf(_L("Uninitilized Client Heap count: %d bytes, Heap Size: %d bytes\n"), User::Heap().Count(), User::Heap().Size());

	TInt error =TheFsSession.Connect();
	User::LeaveIfError(error);

	CleanupFiles();
	for ( manycontacts = 500; manycontacts <= KMaxContacts; manycontacts+=500 )
		{
		CContactsRamTest* res = CContactsRamTest::NewLC();	
		res->Activate();
		CActiveScheduler::Start();
		CleanupStack::PopAndDestroy(res);
		}
    test.End();
	test.Close();
	CleanupFiles();
	//cleanup
	CContactDatabase::DeleteDefaultFileL();
    }

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
    CTrapCleanup* cleanupStack = CTrapCleanup::New();
    if (!cleanupStack)
        {
        return KErrNoMemory;
        }

    CActiveScheduler* activeScheduler = new CActiveScheduler;
    if (!activeScheduler)
        {
        return KErrNoMemory;
        }
    CActiveScheduler::Install(activeScheduler);

    // Run the tests
    TRAPD(err, DoTestsL());

    // Cleanup
    delete activeScheduler;
    delete cleanupStack;
	__UHEAP_MARKEND;
	return err;
    }

