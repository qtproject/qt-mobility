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
#include "t_utils.h"
#include "ccontactdbeventqueue.h"
#include <coreappstest/testserver.h>

// Type definitions 
LOCAL_D RTest test(_L("T_ERROR"));
LOCAL_D RFs TheFs;
LOCAL_D CContactIdArray* IdList;

enum TOomFailureModes
	{	
	KOomFailNormal,
	KOomFailFileServerGeneral,
	KOomFailFileServerOom,
	KOomFailContactServer,
	KOomFailDbs,
	KOomNumFailModes,
	};	

const TPtrC KDatabaseFileName = _L("C:T_ERROR");
const TPtrC KDatabaseCreateFileName = _L("C:T_ERRORC");
const TPtrC KFakeFDrive=_L("C:FAKEFDRIVE");

typedef TBuf<0x40> TOomTestName;

class COomTest;

class COomFailBase : public CBase
	{
public:
	~COomFailBase();
	COomFailBase(COomTest* aOomTest);
	virtual void ConstructL();
	virtual void PreFailL();
	virtual void ClearUpL();
	virtual void FailL() = 0;
	virtual TBool ErrorOk(TOomFailureModes aMode, TInt aIteration, TInt aError);
	virtual TOomTestName TestName() = 0;
	virtual TBool Skip(TInt aCount, TOomFailureModes aMode) const;
protected:
	void RecoverL(TBool waitForEvent = EFalse); 
	void WaitForEvent(TContactDbObserverEventType anEventType);

protected:
	COomTest* iOomTest;
	CContactDatabase* iDb;
	CContactDbEventQueue* iDbEventQueue;
	TInt iCounter;
	};

class COomOpen : public COomFailBase
	{
public:
	inline COomOpen(COomTest* aOomTest) : COomFailBase(aOomTest) {};
	void FailL();
	void ClearUpL();
	inline TOomTestName TestName() {return(_L("Open"));};
private:
	CContactDatabase* iDb2;
	CContactDatabase* iDb;
	};

class COomCreate : public COomFailBase
	{
public:
	inline COomCreate(COomTest* aOomTest) : COomFailBase(aOomTest) {};
	inline TOomTestName TestName() {return(_L("Create"));};
	void PreFailL();
	void FailL();
	void ClearUpL();
private:
	CContactDatabase* iDb2;
	};

class COomSetDatabaseDrive : public COomFailBase
	{
public:
	inline COomSetDatabaseDrive(COomTest* aOomTest, TDriveUnit aMmcDrive) : COomFailBase(aOomTest), iMmcDrive(aMmcDrive) {};
	inline TOomTestName TestName() {return(_L("SetDatabaseDrive"));};
	void PreFailL();
	void ConstructL();
	void FailL();
	void ClearUpL();
	TBool ErrorOk(TOomFailureModes aMode, TInt aIteration, TInt aError);
private:
	// writeable drive found for testing with
	TDriveUnit iMmcDrive;
	};

class COomGetDefaultName : public COomFailBase
	{
public:
	inline COomGetDefaultName(COomTest* aOomTest) : COomFailBase(aOomTest) {};
	inline TOomTestName TestName() {return(_L("GetDefaultName"));};
	void FailL();
	};

class COomViewDefinition : public COomFailBase
	{
public:
	inline COomViewDefinition(COomTest* aOomTest) : COomFailBase(aOomTest) {};
	inline TOomTestName TestName() {return(_L("ViewDefinition"));};
	void FailL();
	};

class COomTextDefinition : public COomFailBase
	{
public:
	inline COomTextDefinition(COomTest* aOomTest) : COomFailBase(aOomTest) {};
	inline TOomTestName TestName() {return(_L("TextDefinition"));};
	void FailL();
	};

class COomReadContact : public COomFailBase
	{
public:
	inline COomReadContact(COomTest* aOomTest) : COomFailBase(aOomTest) {};
	inline TOomTestName TestName() {return(_L("ReadContact"));};
	void FailL();
	};

class COomAddNewContact : public COomFailBase
	{
public:
	inline COomAddNewContact(COomTest* aOomTest) : COomFailBase(aOomTest) {};
	inline TOomTestName TestName() {return(_L("AddNewContact"));};
	void PreFailL();
	void FailL();
	void ClearUpL();
	TBool Skip(TInt aCount, TOomFailureModes aMode) const;
protected:
	TContactItemId iNewContactId;
	};

class COomReadContactTextDef : public COomFailBase
	{
public:
	inline COomReadContactTextDef(COomTest* aOomTest) : COomFailBase(aOomTest) {};
	inline TOomTestName TestName() {return(_L("ReadContactTextDef"));};
	void FailL();
	};

class COomOpenAndCloseContact : public COomFailBase
	{
public:
	inline COomOpenAndCloseContact(COomTest* aOomTest) : COomFailBase(aOomTest) {};
	inline TOomTestName TestName() {return(_L("OpenAndCloseContact"));};
	void FailL();
	};

class COomOpenAndCloseContactX : public COomFailBase
	{
public:
	inline COomOpenAndCloseContactX(COomTest* aOomTest) : COomFailBase(aOomTest) {};
	inline TOomTestName TestName() {return(_L("OpenAndCloseContactX"));};
	void FailL();
	};

class COomOpenModifyAndCommitContact : public COomFailBase
	{
public:
	inline COomOpenModifyAndCommitContact(COomTest* aOomTest) : COomFailBase(aOomTest) {};
	inline TOomTestName TestName() {return(_L("OpenModifyAndCommitContact"));};
	void PreFailL();
	void FailL();
	void ClearUpL();
	TBool Skip(TInt aCount, TOomFailureModes aMode) const;
private:
	CContactItem* iCard;
	};

class COomDeleteContact : public COomFailBase
	{
public:
	inline COomDeleteContact(COomTest* aOomTest) : COomFailBase(aOomTest) {};
	inline TOomTestName TestName() {return(_L("DeleteContact"));};
	void PreFailL();
	void FailL();
	void ClearUpL();
	TBool Skip(TInt aCount, TOomFailureModes aMode) const;
private:
	TContactItemId iNewContactId;
	TInt iStartCount;
	};

class COomDeleteContacts : public COomFailBase
	{
public:
	inline COomDeleteContacts(COomTest* aOomTest) : COomFailBase(aOomTest) {};
	inline TOomTestName TestName() {return(_L("DeleteContacts"));};
	void PreFailL();
	void FailL();
	void ClearUpL();
	TBool Skip(TInt aCount, TOomFailureModes aMode) const;
private:
	CContactIdArray* iNewContactIds;
	TInt iStartCount;
	};

class COomCompact : public COomFailBase
	{
public:
	inline COomCompact(COomTest* aOomTest) : COomFailBase(aOomTest) {};
	inline TOomTestName TestName() {return(_L("Compact"));};
	void FailL();
	void ClearUpL();
	};

class COomImportContacts : public COomFailBase
	{
public:
	~COomImportContacts();
	void ConstructL();
	inline COomImportContacts(COomTest* aOomTest) : COomFailBase(aOomTest) {};
	inline TOomTestName TestName() {return(_L("ImportContacts"));};
	void PreFailL();
	void FailL();
	void ClearUpL();
	TBool Skip(TInt aCount, TOomFailureModes aMode) const;
private:
	CFileStore* iStore;
	RStoreReadStream iInStream;
	TStreamId iStreamId;
	CArrayPtr<CContactItem>* iImportedContacts;
	TInt iOldRecordCount;
	TContactItemId iDeleteMeId;
	};

class COomExportSelectedContacts : public COomFailBase
	{
public:
	~COomExportSelectedContacts();
	void ConstructL();
	inline COomExportSelectedContacts(COomTest* aOomTest) : COomFailBase(aOomTest) {};
	inline TOomTestName TestName() {return(_L("ExportSelectedContacts"));};
	void PreFailL();
	void FailL();
	void ClearUpL();
	TBool ErrorOk(TOomFailureModes aMode, TInt aIteration, TInt aError);
private:
	CContactIdArray* iIdList;
	CFileStore* iStore;
	RStoreWriteStream iOutStream;
	TStreamId iStreamId;
	};

class COomFind : public COomFailBase
	{
public:
	inline COomFind(COomTest* aOomTest) : COomFailBase(aOomTest) {};
	inline TOomTestName TestName() {return(_L("Find"));};
	void FailL();
	};

class COomFindInTextDef : public COomFailBase
	{
public:
	inline COomFindInTextDef(COomTest* aOomTest) : COomFailBase(aOomTest) {};
	~COomFindInTextDef();
	void ConstructL();
	inline TOomTestName TestName() {return(_L("Find In TextDef"));};
	void FailL();
private:
	CDesCArrayFlat* iFindWords;
	CContactTextDef* iTextDef;
	};

class COomFindAsyncInTextDef : public COomFailBase
	{
public:
	inline COomFindAsyncInTextDef(COomTest* aOomTest) : COomFailBase(aOomTest) {};
	~COomFindAsyncInTextDef();
	void ConstructL();
	inline TOomTestName TestName() {return(_L("Find In TextDef ASync"));};
	void FailL();
private:
	CDesCArrayFlat* iFindWords;
	CContactTextDef* iTextDef;
	};

class COomFindAsync : public COomFailBase
	{
public:
	inline COomFindAsync(COomTest* aOomTest) : COomFailBase(aOomTest) {};
	inline TOomTestName TestName() {return(_L("FindAsync"));};
	void FailL();
	};

class COomSort : public COomFailBase
	{
public:
	inline COomSort(COomTest* aOomTest) : COomFailBase(aOomTest) {};
	inline TOomTestName TestName() {return(_L("Sort"));};
	void FailL();
	TBool Skip(TInt aCount, TOomFailureModes aMode) const;
private:
	TInt iPrevCount;
	};

class COomSortArray : public COomFailBase
	{
public:
	~COomSortArray();
	inline COomSortArray(COomTest* aOomTest) : COomFailBase(aOomTest) {};
	inline TOomTestName TestName() {return(_L("Sort Array"));};
	void ConstructL();
	void FailL();
private:
	CContactIdArray* iIdArray;
	TInt iPrevCount;
	CArrayFix<CContactDatabase::TSortPref>* iSortOrder;
	};

class COomCompress : public COomFailBase, public MContactUiCompactObserver
	{
public:
	inline COomCompress(COomTest* aOomTest) : COomFailBase(aOomTest) {};
	inline TOomTestName TestName() {return(_L("Compress"));};
	void PreFailL();
	void FailL();
	TBool Skip(TInt aCount, TOomFailureModes aMode) const;
	void ClearUpL();
// from MContactUiCompactObserver
	void Step(TInt aStep);
	void HandleError(TInt aError);
private:
	TInt iRecievedError;
	};

class COomRecover : public COomFailBase, public MContactUiCompactObserver
	{
public:
	inline COomRecover(COomTest* aOomTest) : COomFailBase(aOomTest) {};
	inline TOomTestName TestName() {return(_L("Recover"));};
	void PreFailL();
	void FailL();
	TBool Skip(TInt aCount, TOomFailureModes aMode) const;
private:
	void doRecoverL();
// from MContactUiCompactObserver
	void Step(TInt aStep);
	void HandleError(TInt aError);
private:
	TInt iRecievedError;
	};

class COomCloseOpenTables : public COomFailBase
	{
public:
	inline COomCloseOpenTables(COomTest* aOomTest) : COomFailBase(aOomTest) {};
	inline TOomTestName TestName() {return(_L("CloseOpenTables"));};
	void FailL();
	};

class COomViewDefInternalize : public COomFailBase
	{
public:
	~COomViewDefInternalize();
	inline COomViewDefInternalize(COomTest* aOomTest) : COomFailBase(aOomTest) {};
	inline TOomTestName TestName() {return(_L("ViewDefInternalize"));};
	void ConstructL();
	void PreFailL();
	void FailL();
	void ClearUpL();
protected:
	CBufStore *iBufStore;
	RStoreReadStream iReadStream;
	TStreamId iStreamId;
	};

class COomViewDefExternalize : public COomFailBase
	{
public:
	~COomViewDefExternalize();
	inline COomViewDefExternalize(COomTest* aOomTest) : COomFailBase(aOomTest) {};
	inline TOomTestName TestName() {return(_L("ViewDefExternalize"));};
	void ConstructL();
	void PreFailL();
	void FailL();
	void ClearUpL();
protected:
	CBufStore* iBufStore;
	RStoreWriteStream iWriteStream;
	TStreamId iStreamId;
	TBool iFailed;
	};

class COomContactChangeNotifier : public COomFailBase
	{
public:
	class COomNotificationRec : public CBase, public MContactDbObserver
		{
	public:
		static COomNotificationRec* NewL();
		~COomNotificationRec();
		void Wait(TBool aUnknownErrAllowed);
		static TInt TimerCallBackL(TAny* aSelf);
		inline TBool UnknownChange() const {return(iUnknownChange);};
	public: // from MContactDbObserver
		void HandleDatabaseEventL(TContactDbObserverEvent aEvent);
	private:
		void ConstructL();
	private:
		CPeriodic* iTimer;
		TBool iWaiting;
		TBool iGotMessage;
		TBool iUnknownErrAllowed;
		TBool iUnknownChange;
		};
public:
	~COomContactChangeNotifier();
	inline COomContactChangeNotifier(COomTest* aOomTest) : COomFailBase(aOomTest) {};
	inline TOomTestName TestName() {return(_L("ContactChangeNotifier"));};
	void ConstructL();
	void PreFailL();
	void FailL();
	void ClearUpL();
	TBool Skip(TInt aCount, TOomFailureModes aMode) const;
protected:
	COomNotificationRec* iRec;
	CContactItem* iItem1;
	CContactItem* iItem2;
	CContactItem* iItem3;
	};

class COomFieldSetStore : public COomFailBase
	{
public:
	~COomFieldSetStore();
	inline COomFieldSetStore(COomTest* aOomTest) : COomFailBase(aOomTest) {};
	inline TOomTestName TestName() {return(_L("FieldSetStore"));};
	void ConstructL();
	void FailL();
	void ClearUpL();
protected:
	CBufStore* iBufStore;
	CBufStore* iBlobStore;
	CBufStore* iTextStore;
	CContactItemFieldSet* iItemFieldSet;
	TStreamId iStreamId;
	};

class COomFieldSetRestore : public COomFailBase
	{
public:
	~COomFieldSetRestore();
	inline COomFieldSetRestore(COomTest* aOomTest) : COomFailBase(aOomTest) {};
	inline TOomTestName TestName() {return(_L("FieldSetRestore"));};
	void ConstructL();
	void PreFailL();
	void FailL();
	void ClearUpL();
protected:
	CBufStore* iBufStore;
	CBufStore* iTextStore;
	CBufStore* iBlobStore;
	TStreamId iStreamId;
	TStreamId iTextStreamId;
	CContactViewDef* iViewDef;
	};

class COomContactCardClone : public COomFailBase
	{
public:
	~COomContactCardClone();
	inline COomContactCardClone(COomTest* aOomTest) : COomFailBase(aOomTest) {};
	inline TOomTestName TestName() {return(_L("ContactCardClone"));};
	void ConstructL();
	void FailL();
private:
	CContactCard* iSourceCard;
	};

class COomPhoneNumberLookup : public COomFailBase
	{
public:
	~COomPhoneNumberLookup();
	inline COomPhoneNumberLookup(COomTest* aOomTest) : COomFailBase(aOomTest) {};
	inline TOomTestName TestName() {return(_L("Phone Number Lookup"));};
	void ConstructL();
	void PreFailL();
	void FailL();
	void ClearUpL();
private:
	TInt FileSizeL();
private:
	TContactItemId iAddId;
	CContactCard* iAddCard;
	};

class COomContactsChangedSince : public COomFailBase
	{
public:
	~COomContactsChangedSince();
	inline COomContactsChangedSince(COomTest* aOomTest) : COomFailBase(aOomTest) {};
	inline TOomTestName TestName() {return(_L("ContactsChangedSince"));};
	void ConstructL();
	void FailL();
private:
	TContactItemId iNewContactId1;
	TContactItemId iNewContactId2;
	TTime iBeforeTime;
	};

class COomGroups : public COomFailBase
	{
public:
	~COomGroups();
	inline COomGroups(COomTest* aOomTest) : COomFailBase(aOomTest) {};
	inline TOomTestName TestName() {return(_L("Groups"));};
	void ConstructL();
	void FailL();
	void ClearUpL();
	TBool Skip(TInt aCount, TOomFailureModes aMode) const;
private:
	TContactItemId iGroupId;
	CContactCard* iItem1;
	};

class COomGroups2 : public COomFailBase
	{
public:
	~COomGroups2();
	inline COomGroups2(COomTest* aOomTest) : COomFailBase(aOomTest) {};
	inline TOomTestName TestName() {return(_L("Groups 2"));};
	void ConstructL();
	void FailL();
	void ClearUpL();
private:
	TContactItemId iGroupId1;
	TContactItemId iGroupId2;
	};

///////////////////

class COomTest : public CBase
	{
public:
	COomTest(TDriveUnit aMmcDrive);
	void ConstructL();
	~COomTest();
	TBool DoTestL();
	void DoOomTestL(COomFailBase *aOomTest, TOomFailureModes aMode);
	void Test(TBool aMustBeTrue);
	TBool TestsL();
private:
	TInt GetRecordCountL();
private:
	RDbs iDbs;
	TDriveUnit iMmcDrive;
	};


LOCAL_C void AddNewContactsL()
//
// Add 2 new contacts with varying numbers of text fields
//
	{
	CContactDatabase* db=CContactDatabase::OpenL(KDatabaseFileName);
	CleanupStack::PushL(db);
	CContactCard* card=CContactCard::NewL();
	CleanupStack::PushL(card);
	TContactItemId id=KNullContactId;
	TContactItemId newId=KNullContactId;
	for (TInt ii=0;ii<5;ii++)
		{
		TBuf<16> name;
		name.Format(_L("NAME #%d"),ii);
		SetNameL(*card,KUidContactFieldFamilyName,KUidContactFieldVCardMapUnusedN,name,EFalse);
		name.Format(_L("GivenName #%d"),ii);
		SetNameL(*card,KUidContactFieldGivenName,KUidContactFieldVCardMapUnusedN,name,EFalse);
		CContactItemField* field=CContactItemField::NewLC(KStorageTypeText);
		card->AddFieldL(*field);
		CleanupStack::Pop(); // field
		newId=db->AddNewContactL(*card);
		id=newId;
		IdList->AddL(id);
		}
	CleanupStack::PopAndDestroy(2); // card, db
	}

LOCAL_C void CreateDatabaseL()
//
// Create a database in a store
//
	{
	CContactDatabase* db=CContactDatabase::ReplaceL(KDatabaseFileName);
	delete db;
	}

///////////////////////////////////////////////////////////////////	
//
// COomFailBase
//

COomFailBase::COomFailBase(COomTest* aOomTest) : iOomTest(aOomTest), iCounter(0)
	{}

COomFailBase::~COomFailBase()
	{
	delete iDbEventQueue;
	delete iDb;
	}

void COomFailBase::ConstructL()
	{
	iDb=CContactDatabase::OpenL(KDatabaseFileName);
	iDbEventQueue = CContactDbEventQueue::NewL(iDb);
	}

void COomFailBase::WaitForEvent(TContactDbObserverEventType anEventType)
	{
	TContactDbObserverEvent event;
	++iCounter;
	FOREVER
		{
		event.iType = EContactDbObserverEventNull;
		iDbEventQueue->ListenForEvent(1,event);  //1 second timeout
		//RDebug::Print(_L("%d Message: %d"), iCounter, event.iType);
		
		if (event.iType == anEventType || event.iType == EContactDbObserverEventNull)
			break;
		}
	}

void COomFailBase::PreFailL()
	{}

void COomFailBase::ClearUpL()
	{}

TBool COomFailBase::ErrorOk(TOomFailureModes aMode, TInt aIteration, TInt aError)
	{
	if(aMode == KOomFailFileServerGeneral)
		{
		if (aError != KErrGeneral)
			{
			test.Printf(_L("ErrorOk test failed - Mode %i, iteration %i, error %i"), aMode, aIteration, aError);
			return EFalse;
			}
		return ETrue;
		}
	else
		{
		if (aError != KErrNoMemory)
			{
			test.Printf(_L("ErrorOk test failed - Mode %i, iteration %i, error %i"), aMode, aIteration, aError);
			return EFalse;
			}
		return ETrue;
		}
	}

void COomFailBase::RecoverL(TBool waitForEvent)
	{
	iDb->RecoverL();
	if (waitForEvent)
		{
		WaitForEvent(EContactDbObserverEventRecover);
		}
	else
		{
		iDbEventQueue->Clear();
		}
		
	}

TBool COomFailBase::Skip(TInt , TOomFailureModes ) const
	{
	// When trying to figure out why we need this, please see DEF037382 and DEF037234
	// Basically, certain conditions cause the DBMS system to corrupt the database
	// and we hop over those particular cases.
	//
	// Generally, one particular case, perhaps when count==71 or whatever, will be
	// the only problem. However, new builds and commenting out other test cases
	// can both change this number, so in a lot of cases every test is skipped instead.
	return(EFalse);
	}

//
// COomOpen
//

void COomOpen::FailL()
	{
	iDb2=CContactDatabase::OpenL(KDatabaseFileName);
	iDb2->CountL();
	}

void COomOpen::ClearUpL()
	{
	RecoverL();
	delete iDb2;
	iDb2=NULL;
	}

//
// COomCreate
//

void COomCreate::PreFailL()
	{
	if(CContactDatabase::ContactDatabaseExistsL(KDatabaseCreateFileName))
    	CContactDatabase::DeleteDatabaseL(KDatabaseCreateFileName);
	}

void COomCreate::FailL()
	{
	iDb2=CContactDatabase::CreateL(KDatabaseCreateFileName);
	}

void COomCreate::ClearUpL()
	{
	RecoverL();
	delete iDb2;
	iDb2=NULL;
    TRAPD(err,CContactDatabase::DeleteDatabaseL(KDatabaseCreateFileName));
	if (err != KErrNotFound)
		{
		User::LeaveIfError(err);
		}
	}

//
// COomSetDatabaseDrive
//

void COomSetDatabaseDrive::ConstructL()
	{
	COomFailBase::ConstructL();
	CContactDatabase::SetDatabaseDriveL(TDriveUnit(EDriveC),EFalse);
	}

void COomSetDatabaseDrive::PreFailL()
	{
	// select test drive
	CContactDatabase::SetDatabaseDriveL(iMmcDrive,EFalse);
	// delete default database, if present
	TRAP_IGNORE(CContactDatabase::DeleteDefaultFileL());
	// switch back to C: drive
	CContactDatabase::SetDatabaseDriveL(TDriveUnit(EDriveC),EFalse);
	// ensure a default database file exists on C:
	if (!CContactDatabase::DefaultContactDatabaseExistsL())
		{
		CContactDatabase* newDb = CContactDatabase::ReplaceL();
		delete newDb;
		}
	}

void COomSetDatabaseDrive::FailL()
	{
	CContactDatabase::SetDatabaseDriveL(iMmcDrive);
	}

void COomSetDatabaseDrive::ClearUpL()
	{
// Below needed because of problem with CDictionaryStore reporting error when it has successfully
// committed changes to the DatabaseDrive ini variable
	RecoverL();
	
	TRAPD(error,CContactDatabase::SetDatabaseDriveL(TDriveUnit(EDriveC),ETrue));
	if (error==KErrNotFound || error==KErrAlreadyExists)
		{
		CContactDatabase::SetDatabaseDriveL(TDriveUnit(EDriveC),EFalse);
		}
	else 
		{
		User::LeaveIfError(error);
		}
	}

TBool COomSetDatabaseDrive::ErrorOk(TOomFailureModes aMode, TInt aIteration, TInt aError)
	{
	if(aMode != KOomFailFileServerGeneral)
		{
		if (aError != KErrNoMemory)
			{
			TPtrC drvPtr(iMmcDrive.Name());

			test.Printf(_L("ErrorOk test failed for SetDatabaseDrive(%S) - Mode %i, iteration %i, error %i"), 
				&drvPtr, aMode, aIteration, aError);

#ifdef __WINS__
			if (aError == KErrNotReady)
				{
				test.Printf(_L("Writeable directory must map to an extra drive (e.g. F: or D:). In epoc.ini file add _EPOC_DRIVE_F \\epoc32\\winscw\\f"));
				}
			else if (aError == KErrPathNotFound)
				{
				test.Printf(_L("Drive %S mapped in epoc.ini file, but probably the mapped directory does not exist."), &drvPtr);
				}
#else
			// hardware build
			if (aError == KErrNotReady)
				{
				test.Printf(_L("Drive %s not ready."), &drvPtr);
				}
#endif
			return EFalse;
			}
		return ETrue;
		}
	else
		{
		return(COomFailBase::ErrorOk(aMode, aIteration, aError));
		}
	}

//
// COomGetDefaultName
//

void COomGetDefaultName::FailL()
	{
	TFileName fileName;
	CContactDatabase::GetDefaultNameL(fileName);
	}

//
// COomViewDefinition
//

void COomViewDefinition::FailL()
	{
	CContactItemViewDef* itemViewDef=CContactItemViewDef::NewLC(CContactItemViewDef::EIncludeFields,CContactItemViewDef::EIncludeHiddenFields);
	itemViewDef->AddL(KUidContactFieldCompanyName);
	CContactViewDef* viewDef=CContactViewDef::NewL(itemViewDef);
	CleanupStack::Pop();	// itemViewDef
	delete viewDef;
	}

//
// COomTextDefinition
//

void COomTextDefinition::FailL()
	{
	CContactTextDef *textDef=CContactTextDef::NewLC();
	textDef->AppendL(TContactTextDefItem(KUidContactFieldPhoneNumber));
	CleanupStack::PopAndDestroy();	// textDef
	}

//
// COomReadContact
//

void COomReadContact::FailL()
	{
	iDb->ReadContactLC((*IdList)[0]);
	CContactItem* card2=iDb->ReadContactL((*IdList)[1]);
	CleanupStack::PopAndDestroy();	// card1
	delete card2;
	}

//
// COomAddNewContact
//
void COomAddNewContact::PreFailL()
	{
	iDbEventQueue->Flush();
	iNewContactId=KNullContactId;
	}

void COomAddNewContact::FailL()
	{
	CContactCard* card=CContactCard::NewL();
	CleanupStack::PushL(card);
	SetNameL(*card,KUidContactFieldFamilyName,KUidContactFieldVCardMapUnusedN,_L("OOM"),EFalse);
	iNewContactId=iDb->AddNewContactL(*card);
	CleanupStack::PopAndDestroy(); // card
	}

void COomAddNewContact::ClearUpL()
	{
	RecoverL(ETrue); //Wait for recover complete

	if (iNewContactId!=KNullContactId)
		iDb->DeleteContactL(iNewContactId);
	}

TBool COomAddNewContact::Skip(TInt aCount, TOomFailureModes aMode) const
	{
	// Skipped because KErrCorrupt when recovering.
	if (aMode==KOomFailFileServerGeneral) return aCount==10;
	if (aMode==KOomFailFileServerOom) return aCount==10;
	return EFalse;
	}

//
// COomReadContactTextDef
//

void COomReadContactTextDef::FailL()
	{
	CContactTextDef *textDef=CContactTextDef::NewLC();
	textDef->AppendL(TContactTextDefItem(KUidContactFieldGivenName,_L("==")));
	textDef->AppendL(TContactTextDefItem(KUidContactFieldFamilyName,_L(", ")));
	TBuf<0x100> buf;
	iDb->ReadContactTextDefL((*IdList)[1],buf,textDef);
	iDb->ReadContactTextDefL((*IdList)[3],buf,textDef);
	CleanupStack::PopAndDestroy();	// textDef
	}

//
// COomOpenAndCloseContact
//

void COomOpenAndCloseContact::FailL()
	{
	CContactItem* card=iDb->OpenContactL((*IdList)[0]);
	delete card;
	iDb->CloseContactL((*IdList)[0]);
	}

//
// COomOpenAndCloseContactX
//

void COomOpenAndCloseContactX::FailL()
	{
	CContactItem* card=iDb->OpenContactLX((*IdList)[0]);
	CleanupStack::PushL(card);
	CleanupStack::PushL(HBufC::NewL(10));	// any old alloc to trigger a fail between the open and close
	iDb->CloseContactL((*IdList)[0]);
	CleanupStack::PopAndDestroy(2); // HBufC & card
	CleanupStack::Pop(); // card close
	}

//
// COomOpenModifyAndCommitContact
//

void COomOpenModifyAndCommitContact::PreFailL()
	{
	iCard=iDb->OpenContactL((*IdList)[1]);
	iDbEventQueue->Flush();
	}

void COomOpenModifyAndCommitContact::FailL()
	{
	SetNameL(*iCard,KUidContactFieldGivenName,KUidContactFieldVCardMapUnusedN,_L("Hacked name"),EFalse);
	iDb->CommitContactL(*iCard);
	}

void COomOpenModifyAndCommitContact::ClearUpL()
	{
	iDb->CloseContactL((*IdList)[1]);
	delete iCard;
	RecoverL(ETrue); //wait for event
	}

TBool COomOpenModifyAndCommitContact::Skip(TInt aCount, TOomFailureModes aMode) const
	{
	// KErrCorrupt when recovering
	if (aMode==KOomFailFileServerGeneral) return aCount==11;
	if (aMode==KOomFailFileServerOom) return aCount==11;
	return(EFalse);
	}

//
// COomDeleteContact
//

void COomDeleteContact::PreFailL()
	{
	iStartCount=iDb->CountL();
	CContactCard* card=CContactCard::NewL();
	CleanupStack::PushL(card);
	SetNameL(*card,KUidContactFieldFamilyName,KUidContactFieldVCardMapUnusedN,_L("DeleteMe"),EFalse);
	iNewContactId=iDb->AddNewContactL(*card);
	CleanupStack::PopAndDestroy(); // card
	}

void COomDeleteContact::FailL()
	{
	iDb->DeleteContactL(iNewContactId);
	}

void COomDeleteContact::ClearUpL()
	{
	RecoverL();
	TRAP_IGNORE(iDb->DeleteContactL(iNewContactId));
	test(iStartCount==iDb->CountL());
	}

TBool COomDeleteContact::Skip(TInt aCount, TOomFailureModes aMode) const
	{
	// KErrCorrupt when recovering.
	if (aMode==KOomFailFileServerGeneral) return aCount==11;
	if (aMode==KOomFailFileServerOom) return aCount==11;
	return(EFalse);
	}

//
// COomDeleteContacts
//

void COomDeleteContacts::PreFailL()
	{
	iStartCount=iDb->CountL();
	CContactCard* card=CContactCard::NewL();
	CleanupStack::PushL(card);
	SetNameL(*card,KUidContactFieldFamilyName,KUidContactFieldVCardMapUnusedN,_L("DeleteMe"),EFalse);
	iNewContactIds=CContactIdArray::NewL();
	iNewContactIds->AddL(iDb->AddNewContactL(*card));
	iNewContactIds->AddL(iDb->AddNewContactL(*card));
	iNewContactIds->AddL(iDb->AddNewContactL(*card));
	CleanupStack::PopAndDestroy(); // card
	}

void COomDeleteContacts::FailL()
	{
	iDb->DeleteContactsL(*iNewContactIds);
	}

void COomDeleteContacts::ClearUpL()
	{
	RecoverL();
	for(TInt loop=0;loop<iNewContactIds->Count();loop++)
		{
		TRAP_IGNORE(iDb->DeleteContactL((*iNewContactIds)[loop]));
		}
	delete iNewContactIds;
	test(iStartCount==iDb->CountL());
	}

TBool COomDeleteContacts::Skip(TInt /*aCount*/, TOomFailureModes aMode) const
	{
	// KErrCorrupt when recovering.
	// This fails on one test case (usually around 60 - 70) but which
	// one changes between builds.
	if (aMode==KOomFailFileServerGeneral) return ETrue;
	if (aMode==KOomFailFileServerOom) return ETrue;
	return EFalse;
	}

//
// COomCompact
//

void COomCompact::FailL()
	{
	iDb->CompactL();
	}

void COomCompact::ClearUpL()
	{
	RecoverL();
	}

//
// COomExportSelectedContacts
//

COomExportSelectedContacts::~COomExportSelectedContacts()
	{
	delete iIdList;
	}

void COomExportSelectedContacts::ConstructL()
	{
	COomFailBase::ConstructL();
//
	CArrayFix<CContactDatabase::TSortPref>* sortOrder=new(ELeave) CArrayFixFlat<CContactDatabase::TSortPref>(3);
	CleanupStack::PushL(sortOrder);
	sortOrder->AppendL(CContactDatabase::TSortPref(KUidContactFieldFamilyName));
	iDb->SortL(sortOrder);
	CleanupStack::Pop(); // sortOrder
//
	iIdList=CContactIdArray::NewL(iDb->SortedItemsL());
	while(iIdList->Count()>2)
		iIdList->Remove(0);	// only leave 2 in the array
	}

void COomExportSelectedContacts::PreFailL()
	{
	iStore=CDirectFileStore::ReplaceL(TheFs,_L("c:\\TErrorExport"),EFileWrite);
   	iStore->SetTypeL(KDirectFileStoreLayoutUid);
	iStreamId=iOutStream.CreateL(*iStore);
	}

void COomExportSelectedContacts::FailL()
	{
   	iDb->ExportSelectedContactsL(TUid::Uid(KUidVCardConvDefaultImpl),*iIdList,iOutStream,CContactDatabase::ETTFormat);
	}

void COomExportSelectedContacts::ClearUpL()
	{
	RecoverL();
	iOutStream.CommitL();
	iStore->SetRootL(iStreamId);
	iStore->CommitL();  	
	iOutStream.Close();
	delete iStore;
	}

TBool COomExportSelectedContacts::ErrorOk(TOomFailureModes aMode, TInt aIteration, TInt aError)
	{
	// LoadPluginL interprets all file server errors as KErrNotFound, except KErrNoMemory.
	if(aMode == KOomFailFileServerGeneral)
		{
		if (aError == KErrGeneral || aError == KErrNotFound)
			return ETrue;
		
		test.Printf(_L("ErrorOk test - Mode %i, iteration %i, error %i"), aMode, aIteration, aError);
		return EFalse;
		}
	else if(aMode == KOomFailFileServerOom)
		{
		if (aError == KErrNoMemory || aError == KErrNotFound)
			return ETrue;

		test.Printf(_L("ErrorOk test - Mode %i, iteration %i, error %i"), aMode, aIteration, aError);
		return EFalse;
		}
	else
		{
		return(COomFailBase::ErrorOk(aMode, aIteration, aError));
		}
	}

//
// COoomImportContacts
//

COomImportContacts::~COomImportContacts()
	{
	TRAP_IGNORE(iDb->DeleteContactL(iDeleteMeId));
	delete iStore;
	}

void COomImportContacts::ConstructL()
	{
//zzz Need a ttformat version of this
	COomFailBase::ConstructL();
//
	CArrayFix<CContactDatabase::TSortPref>* sortOrder=new(ELeave) CArrayFixFlat<CContactDatabase::TSortPref>(3);
	CleanupStack::PushL(sortOrder);
	sortOrder->AppendL(CContactDatabase::TSortPref(KUidContactFieldFamilyName));
	iDb->SortL(sortOrder);
	CleanupStack::Pop(); // sortOrder
//
	iStore=CDirectFileStore::ReplaceL(TheFs,_L("c:\\TErrorExport"),EFileWrite);
  	iStore->SetTypeL(KDirectFileStoreLayoutUid);
	RStoreWriteStream outStream;
	iStreamId=outStream.CreateLC(*iStore);
//
	CContactIdArray *idList=CContactIdArray::NewLC();
	CContactCard *card=CContactCard::NewLC();
	SetNameL(*card,KUidContactFieldFamilyName,KUidContactFieldVCardMapUnusedN,_L("ABCDE"),EFalse);
	SetNameL(*card,KUidContactFieldGivenName,KUidContactFieldVCardMapUnusedN,_L("123"),EFalse);
	idList->AddL(iDb->AddNewContactL(*card));
	SetNameL(*card,KUidContactFieldFamilyName,KUidContactFieldVCardMapUnusedN,_L("ZYX"),EFalse);
	SetNameL(*card,KUidContactFieldGivenName,KUidContactFieldVCardMapUnusedN,_L("987"),EFalse);
	idList->AddL(iDb->AddNewContactL(*card));
	SetNameL(*card,KUidContactFieldFamilyName,KUidContactFieldVCardMapUnusedN,_L("QQQQ"),EFalse);
	SetNameL(*card,KUidContactFieldGivenName,KUidContactFieldVCardMapUnusedN,_L("666"),EFalse);
	idList->AddL(iDb->AddNewContactL(*card));
	CleanupStack::PopAndDestroy();	// card
   	iDb->ExportSelectedContactsL(TUid::Uid(KUidVCardConvDefaultImpl),*idList,outStream,CContactDatabase::EExcludeUid);
	iDb->DeleteContactL((*idList)[0]);
	iDb->DeleteContactL((*idList)[1]);
	iDeleteMeId=(*idList)[2];	// Leave this one in the DB so it is imported as an updated entry
//
	outStream.CommitL();
	iStore->SetRootL(iStreamId);
	iStore->CommitL();  	
	CleanupStack::PopAndDestroy(2);	// outStream,idList
	iOldRecordCount=iDb->CountL();
	}

void COomImportContacts::PreFailL()
	{
	iInStream.OpenL(*iStore,iStreamId);
	}

void COomImportContacts::FailL()
	{
	TBool success=EFalse;
	iImportedContacts=iDb->ImportContactsL(TUid::Uid(KUidVCardConvDefaultImpl),iInStream,success,0);
	test(iImportedContacts->Count()==3);
	test(success);
	}

void COomImportContacts::ClearUpL()
	{
	RecoverL();
	if (iImportedContacts)
		{
		while(iImportedContacts->Count()>0)
			{
			iDb->DeleteContactL((*iImportedContacts)[0]->Id());
			delete (*iImportedContacts)[0];
			iImportedContacts->Delete(0);
			}
		delete iImportedContacts;
		iImportedContacts=NULL;
		}
	test(iDb->CountL()==iOldRecordCount);
	iInStream.Close();
	}

TBool COomImportContacts::Skip(TInt, TOomFailureModes aMode) const
	{
	// KErrCorrupt when recovering
	// This test only fails on one or two counts in each loop through the harness,
	// but the particular counts on which it fails seem to vary with each execution.
	if (aMode==KOomFailFileServerGeneral) return ETrue;
	if (aMode==KOomFailFileServerOom) return ETrue;
	return(EFalse);
	}

//
// COomFind
//

void COomFind::FailL()
	{
	CContactItemFieldDef *fieldDef=new(ELeave) CContactItemFieldDef;
	CleanupStack::PushL(fieldDef);
	fieldDef->AppendL(KUidContactFieldFamilyName);
	CContactIdArray* ids=iDb->FindLC(_L("NAME"),fieldDef);
	test(ids->Count()==5);
	CleanupStack::PopAndDestroy(2);	// ids & fieldDef
	}

//
// COomFindInTextDef
//

LOCAL_C TInt findWordSplitterL(TAny *aParams)
	{
	SFindInTextDefWordParser *parser=(SFindInTextDefWordParser *)aParams;
	const TText *ptr=parser->iSearchString->Ptr();
	const TText *end=ptr+parser->iSearchString->Length();
	const TText *startOfWord=NULL;
	FOREVER
		{
		if (ptr==end || !TChar(*ptr).IsAlphaDigit())
			{
			if (startOfWord)
				{
				TPtrC addWord(startOfWord,ptr-startOfWord);
				parser->iWordArray->AppendL(addWord);
				startOfWord=NULL;
				}
			if (ptr==end)
				break;
			}
		else if (!startOfWord)
			startOfWord=ptr;
		ptr++;
		}
	return(KErrNone);
	}

COomFindInTextDef::~COomFindInTextDef()
	{
	delete iTextDef;
	delete iFindWords;
	}

void COomFindInTextDef::ConstructL()
	{
	COomFailBase::ConstructL();
	iTextDef=CContactTextDef::NewL();
	iTextDef->AppendL(KUidContactFieldFamilyName);
	iTextDef->AppendL(KUidContactFieldGivenName);
	iFindWords=new(ELeave) CDesCArrayFlat(2);
	iFindWords->AppendL(_L("NAME"));
	iFindWords->AppendL(_L("GivenName"));
	}

void COomFindInTextDef::FailL()
	{
	TCallBack callBack(findWordSplitterL);
	CContactIdArray* ids=iDb->FindInTextDefLC(*iFindWords,iTextDef,callBack);
	test(ids->Count()==5);
	CleanupStack::PopAndDestroy();	// ids
	}

//
// COomFindAsync
//

class CAsyncFinder : public CBase, public MIdleFindObserver
	{
public:
	~CAsyncFinder();
	void ConstructL(CContactDatabase *aContactDatabase, const TDesC& aText,const CContactItemFieldDef *aFieldDef);
	void ConstructL(CContactDatabase *aContactDatabase, const CDesCArrayFlat &iFindWords,CContactTextDef* iTextDef, const TCallBack &aCallBack);
	inline CContactIdArray *TakeContactIds() {return(iFinder->TakeContactIds());};
	inline TInt Error() const {return(iFinder->Error());};
protected:
// from MIdleFindObserver
	void IdleFindCallback();
private:
	CIdleFinder *iFinder;
	};

CAsyncFinder::~CAsyncFinder()
	{
	delete iFinder;
	}

void CAsyncFinder::ConstructL(CContactDatabase *aContactDatabase, const TDesC& aText,const CContactItemFieldDef *aFieldDef)
	{
	iFinder=aContactDatabase->FindAsyncL(aText, aFieldDef, this);
	}

void CAsyncFinder::ConstructL(CContactDatabase *aContactDatabase, const CDesCArrayFlat &aFindWords,CContactTextDef* iTextDef, const TCallBack &aCallBack)
	{
	iFinder=aContactDatabase->FindInTextDefAsyncL(aFindWords, iTextDef, this, aCallBack);
	}

void CAsyncFinder::IdleFindCallback()
	{
	if (iFinder->IsComplete())
		CActiveScheduler::Stop();
	}

void COomFindAsync::FailL()
	{
	CAsyncFinder *finder=new(ELeave) CAsyncFinder;
	CleanupStack::PushL(finder);
	finder->ConstructL(iDb,_L("NAME"),NULL);
	CActiveScheduler::Start();
	User::LeaveIfError(finder->Error());
	CContactIdArray *ids=finder->TakeContactIds();
	CleanupStack::PushL(ids);
	test(ids->Count()==5);
	CleanupStack::PopAndDestroy(2);	// ids,finder
	}

//
// COomFindAsyncInTextDef
//

COomFindAsyncInTextDef::~COomFindAsyncInTextDef()
	{
	delete iTextDef;
	delete iFindWords;
	}

void COomFindAsyncInTextDef::ConstructL()
	{
	COomFailBase::ConstructL();
	iTextDef=CContactTextDef::NewL();
	iTextDef->AppendL(KUidContactFieldFamilyName);
	iTextDef->AppendL(KUidContactFieldGivenName);
	iFindWords=new(ELeave) CDesCArrayFlat(2);
	iFindWords->AppendL(_L("NAME"));
	iFindWords->AppendL(_L("GivenName"));
	}

void COomFindAsyncInTextDef::FailL()
	{
	TCallBack callBack(findWordSplitterL);
	CAsyncFinder *finder=new(ELeave) CAsyncFinder;
	CleanupStack::PushL(finder);
	finder->ConstructL(iDb,*iFindWords,NULL,callBack);
	CActiveScheduler::Start();
	User::LeaveIfError(finder->Error());
	CContactIdArray *ids=finder->TakeContactIds();
	CleanupStack::PushL(ids);
//	test(ids->Count()==5);
	CleanupStack::PopAndDestroy(2);	// ids,finder
	}

//
// COomSort
//

TBool COomSort::Skip(TInt /*aCount*/, TOomFailureModes aMode) const
	{
	if(aMode == KOomFailFileServerGeneral)
		return ETrue;
	return ETrue;
	}
void COomSort::FailL()
	{
	CArrayFix<CContactDatabase::TSortPref>* sortOrder=new(ELeave) CArrayFixFlat<CContactDatabase::TSortPref>(3);
	CleanupStack::PushL(sortOrder);
	sortOrder->AppendL(CContactDatabase::TSortPref(KUidContactFieldFamilyName));
	sortOrder->AppendL(CContactDatabase::TSortPref(KUidContactFieldGivenName));
	iDb->SortL(sortOrder);
	CleanupStack::Pop(); // sortOrder
	const CContactIdArray *ids=iDb->SortedItemsL();
	test(ids->Count()>0);
	if (iPrevCount>0)
		test(ids->Count()==iPrevCount);
	else
		iPrevCount=ids->Count();
	}

//
// COomSortArray
//

COomSortArray::~COomSortArray()
	{
	delete iSortOrder;
	delete iIdArray;
	}

void COomSortArray::ConstructL()
	{
	COomFailBase::ConstructL();
	iSortOrder=new(ELeave) CArrayFixFlat<CContactDatabase::TSortPref>(3);
	iSortOrder->AppendL(CContactDatabase::TSortPref(KUidContactFieldFamilyName));
	iSortOrder->AppendL(CContactDatabase::TSortPref(KUidContactFieldGivenName));
	iIdArray=CContactIdArray::NewL(iDb->SortedItemsL());
	}

void COomSortArray::FailL()
	{
	CContactIdArray *sortedIds=iDb->SortArrayL(iIdArray,iSortOrder);
	test(sortedIds->Count()==iIdArray->Count());
	delete sortedIds;
	}

//
// COomCompress
//

void COomCompress::Step(TInt )
	{
	}

void COomCompress::HandleError(TInt aError)
	{
	iRecievedError=aError;
	}

void COomCompress::PreFailL()
	{
	CContactCard* card=CContactCard::NewL();
	CleanupStack::PushL(card);
	SetNameL(*card,KUidContactFieldFamilyName,KUidContactFieldVCardMapUnusedN,_L("DeleteMe"),EFalse);
	iDb->DeleteContactL(iDb->AddNewContactL(*card));	// Make sure a compress is required
	iDb->DeleteContactL(iDb->AddNewContactL(*card));
	iDb->DeleteContactL(iDb->AddNewContactL(*card));
	iDb->DeleteContactL(iDb->AddNewContactL(*card));
	CleanupStack::PopAndDestroy(); // card
	iRecievedError=KErrNone;
	}

void COomCompress::FailL()
	{
	CContactActiveCompress* compressor=iDb->CreateCompressorLC();
	compressor->SetObserver(this);
	while(compressor->Step()) {};
	test(iRecievedError==compressor->Error());
	User::LeaveIfError(compressor->Error());
	CleanupStack::PopAndDestroy();	// compressor
	}

void COomCompress::ClearUpL()
	{
	RecoverL();
	}

TBool COomCompress::Skip(TInt /*aCount*/, TOomFailureModes aMode) const
	{
	// KErrCorrupt.
	// This fails on 3 or 4 tests in each run, but which ones varies between executions.
	if (aMode==KOomFailFileServerGeneral || aMode==KOomFailFileServerOom) return ETrue;
	return(EFalse);
	}

//
// COomRecover
//

void COomRecover::Step(TInt )
	{
	}

void COomRecover::HandleError(TInt aError)
	{
	iRecievedError=aError;
	}

void COomRecover::PreFailL()
	{
	iRecievedError = KErrNone;
	if (iDb->IsDamaged())
		{
		doRecoverL();
		}
	iDb->DamageDatabaseL(0x666);
	}

void COomRecover::doRecoverL()
	{
	CContactActiveRecover* recover=iDb->CreateRecoverLC();
	recover->SetObserver(this);
	while(recover->Step()) {};
	test(iRecievedError==recover->Error());
	User::LeaveIfError(recover->Error());
	CleanupStack::PopAndDestroy();	// recover
	}

void COomRecover::FailL()
	{
	test(DbShouldBeInDamagedState(iDb) );
	doRecoverL();
	}

TBool COomRecover::Skip(TInt, TOomFailureModes aMode) const
	{
	// KErrCorrupt
	// This test usually only fails on one value of count, but exactly which
	// one it will be varies between executions of the harness. Not good.
	if (aMode==KOomFailFileServerGeneral) return ETrue;
	if (aMode==KOomFailFileServerOom) return ETrue;
	if (aMode==KOomFailDbs) return ETrue; //fixes INC052869 - remove for DEF054579
	return(EFalse);
	}

//
// COomCloseOpenTables
//

void COomCloseOpenTables::FailL()
	{
	iDb->CloseTables();
	iDbEventQueue->Clear();
	}

//
// COomViewDefInternalize
//

COomViewDefInternalize::~COomViewDefInternalize()
	{
	delete iBufStore;
	}

void COomViewDefInternalize::ConstructL()
	{
	COomFailBase::ConstructL();
	CContactItemViewDef* itemViewDef=CContactItemViewDef::NewLC(CContactItemViewDef::EIncludeFields,CContactItemViewDef::EIncludeHiddenFields);
	itemViewDef->AddL(KUidContactFieldCompanyName);
	itemViewDef->AddL(KUidContactFieldGivenName);
	CContactViewDef* viewDef=CContactViewDef::NewL(itemViewDef);
	CleanupStack::Pop();	// itemViewDef
	CleanupStack::PushL(viewDef);	// viewDef
//
	iBufStore=CBufStore::NewL(0x400);
	RStoreWriteStream writeStream;
	iStreamId=writeStream.CreateLC(*iBufStore);
	viewDef->ExternalizeL(writeStream);
	CleanupStack::PopAndDestroy(2);	// writeStream,viewDef
	}

void COomViewDefInternalize::PreFailL()
	{
	iReadStream.OpenL(*iBufStore,iStreamId);
	}

void COomViewDefInternalize::FailL()
	{
	CContactViewDef* viewDef=CContactViewDef::NewLC();
	viewDef->InternalizeL(iReadStream);
	CleanupStack::PopAndDestroy();	// viewDef
	}

void COomViewDefInternalize::ClearUpL()
	{
	RecoverL();
	iReadStream.Close();
	}

//
// COomViewDefExternalize
//

COomViewDefExternalize::~COomViewDefExternalize()
	{
	delete iBufStore;
	}

void COomViewDefExternalize::ConstructL()
	{
	COomFailBase::ConstructL();
	iBufStore=CBufStore::NewL(0x400);
	}

void COomViewDefExternalize::PreFailL()
	{
	iStreamId=iWriteStream.CreateL(*iBufStore);
	iFailed=ETrue;
	}

void COomViewDefExternalize::FailL()
	{
	CContactItemViewDef* itemViewDef=CContactItemViewDef::NewLC(CContactItemViewDef::EIncludeFields,CContactItemViewDef::EIncludeHiddenFields);
	itemViewDef->AddL(KUidContactFieldCompanyName);
	itemViewDef->AddL(KUidContactFieldGivenName);
	CContactViewDef* viewDef=CContactViewDef::NewL(itemViewDef);
	CleanupStack::Pop();	// itemViewDef
	CleanupStack::PushL(viewDef);	// viewDef
	viewDef->ExternalizeL(iWriteStream);
	CleanupStack::PopAndDestroy();	// viewDef
	iFailed=EFalse;
	}

void COomViewDefExternalize::ClearUpL()
	{
	RecoverL();
	iWriteStream.Close();
	if (!iFailed)
		{
		RStoreReadStream readStream;
		readStream.OpenL(*iBufStore,iStreamId);
		CContactViewDef* viewDef=CContactViewDef::NewLC();
		viewDef->InternalizeL(readStream);
		CleanupStack::PopAndDestroy();	// viewDef
		readStream.Close();
		}
	iBufStore->Delete(iStreamId);
	}

//
// COomFieldSetStore
//

COomFieldSetStore::~COomFieldSetStore()
	{
	delete iItemFieldSet;
	delete iBufStore;
	delete iBlobStore;
	delete iTextStore;
	}

void COomFieldSetStore::ConstructL()
	{
	COomFailBase::ConstructL();
	iBufStore=CBufStore::NewL(0x400);
	iBlobStore=CBufStore::NewL(0x400);
	iTextStore=CBufStore::NewL(0x400);
	CContactItemField* itemField=CContactItemField::NewLC(KStorageTypeText, KUidContactFieldAddress);
	iItemFieldSet=CContactItemFieldSet::NewL();
	iItemFieldSet->AddL(*itemField);
	CleanupStack::Pop();	// itemField
	}

void COomFieldSetStore::FailL()
	{
	iStreamId=KNullStreamId;
	RStoreWriteStream stream;
	stream.CreateLC(*iTextStore);
	iStreamId=iItemFieldSet->StoreL(*iBufStore,stream,*iBlobStore);
	CleanupStack::PopAndDestroy();
	}

void COomFieldSetStore::ClearUpL()
	{
	RecoverL();
	iBufStore->Delete(iStreamId);
	}

//
// COomFieldSetRestore
//

COomFieldSetRestore::~COomFieldSetRestore()
	{
	delete iBufStore;
	delete iTextStore;
	delete iBlobStore;
	delete iViewDef;
	}

void COomFieldSetRestore::ConstructL()
	{
	COomFailBase::ConstructL();
	iBufStore=CBufStore::NewL(0x400);
	iTextStore=CBufStore::NewL(0x400);
	iBlobStore=CBufStore::NewL(0x400);
	CContactItemViewDef* itemViewDef=CContactItemViewDef::NewLC(CContactItemViewDef::EIncludeFields,CContactItemViewDef::EIncludeHiddenFields);
	itemViewDef->AddL(KUidContactFieldAddress);
	iViewDef=CContactViewDef::NewL(itemViewDef);
	CleanupStack::Pop();	// itemViewDef
	}

void COomFieldSetRestore::PreFailL()
	{
	CContactItemFieldSet *itemFieldSet=CContactItemFieldSet::NewLC();
	CContactItemField* itemField=CContactItemField::NewLC(KStorageTypeText, KUidContactFieldAddress);
	itemFieldSet->AddL(*itemField);
	RStoreWriteStream stream;
	iTextStreamId=stream.CreateLC(*iTextStore);
	iStreamId=itemFieldSet->StoreL(*iBufStore,stream,*iBlobStore);
	CleanupStack::PopAndDestroy();	// stream
	CleanupStack::Pop();	// itemField
	CleanupStack::PopAndDestroy();	// itemFieldSet
	}

void COomFieldSetRestore::FailL()
	{
	CContactItemFieldSet *itemFieldSet=CContactItemFieldSet::NewLC();
	RStoreReadStream stream;
	stream.OpenLC(*iTextStore,iTextStreamId);
	itemFieldSet->RestoreL(*iBufStore,iStreamId,iBlobStore,iViewDef->ItemDef(),stream);
	CleanupStack::PopAndDestroy(2);	// itemFieldSet
	}

void COomFieldSetRestore::ClearUpL()
	{
	RecoverL();
	iBufStore->Delete(iStreamId);
	}

//
// COomContactCardClone
//

COomContactCardClone::~COomContactCardClone()
	{
	delete iSourceCard;
	}

void COomContactCardClone::ConstructL()
	{
	COomFailBase::ConstructL();
	iSourceCard=CContactCard::NewL();
	CContactItemField *field=CContactItemField::NewLC(KStorageTypeText,KUidContactFieldAddress);
	iSourceCard->AddFieldL(*field);
	CleanupStack::Pop();	// field
	field=CContactItemField::NewLC(KStorageTypeDateTime,TUid::Uid(0x123));
	iSourceCard->AddFieldL(*field);
	CleanupStack::Pop();	// field
	}

void COomContactCardClone::FailL()
	{
	CContactCard::NewLC(iSourceCard);
	CleanupStack::PopAndDestroy();	// card
	}

//
// COomContactChangeNotifier
//

COomContactChangeNotifier::COomNotificationRec* COomContactChangeNotifier::COomNotificationRec::NewL()
	{ // static
	COomContactChangeNotifier::COomNotificationRec* self=new(ELeave) COomContactChangeNotifier::COomNotificationRec;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();	// self
	return self;
	}

COomContactChangeNotifier::COomNotificationRec::~COomNotificationRec()
	{
	delete iTimer;
	}

void COomContactChangeNotifier::COomNotificationRec::ConstructL()
	{
	iTimer=CPeriodic::NewL(0);
	}

void COomContactChangeNotifier::COomNotificationRec::Wait(TBool aUnknownErrAllowed)
	{
	iWaiting=ETrue;
	iGotMessage=EFalse;
	iUnknownErrAllowed=aUnknownErrAllowed;
	iUnknownChange=EFalse;
	iTimer->Cancel();
	iTimer->Start(500000,500000,TCallBack(TimerCallBackL,this)); // wait for 0.5 second for notification
	CActiveScheduler::Start();
	}

TInt COomContactChangeNotifier::COomNotificationRec::TimerCallBackL(TAny* aSelf)
	{ // static
	// if this gets called, notification hasn't happened
	if (((COomNotificationRec *)aSelf)->iGotMessage)
		CActiveScheduler::Stop();
	else
		User::Leave(KErrGeneral);
	return 0;
	}

void COomContactChangeNotifier::COomNotificationRec::HandleDatabaseEventL(TContactDbObserverEvent aEvent)
	{
	if (aEvent.iType==EContactDbObserverEventContactChanged || (iUnknownErrAllowed && aEvent.iType==EContactDbObserverEventUnknownChanges))
		{
		iGotMessage=ETrue;
		iTimer->Cancel();
		if (!iWaiting)
			return;
		if (aEvent.iType==EContactDbObserverEventUnknownChanges)
			iUnknownChange=ETrue;
		CActiveScheduler::Stop();
		delete HBufC::NewL(10);	// Dummy alloc so we can fail inside here
		}
	}

COomContactChangeNotifier::~COomContactChangeNotifier()
	{
	delete iRec;
	}

void COomContactChangeNotifier::ConstructL()
	{
	COomFailBase::ConstructL();
	iRec=COomContactChangeNotifier::COomNotificationRec::NewL();
	}

void COomContactChangeNotifier::PreFailL()
	{
	iItem1=iDb->OpenContactL((*IdList)[0]);
	iItem2=iDb->OpenContactL((*IdList)[1]);
	iItem3=iDb->OpenContactL((*IdList)[2]);
	}

void COomContactChangeNotifier::FailL()
	{
	CContactChangeNotifier *notify1=CContactChangeNotifier::NewL(*iDb,iRec);
	CleanupStack::PushL(notify1);
	iDb->CommitContactL(*iItem1);
	iDb->CommitContactL(*iItem2);
	iDb->CommitContactL(*iItem3);
	for(TInt loop=0;loop<3;loop++)
		{
		iRec->Wait(ETrue);
		if (iRec->UnknownChange())
			User::Leave(KErrNoMemory);
		}
	CleanupStack::PopAndDestroy(); // notify1
	}

void COomContactChangeNotifier::ClearUpL()
	{
	RecoverL();
	iDb->CloseContactL(iItem1->Id());
	iDb->CloseContactL(iItem2->Id());
	iDb->CloseContactL(iItem3->Id());
	delete iItem1;
	delete iItem2;
	delete iItem3;
//
	CContactChangeNotifier *notify=CContactChangeNotifier::NewL(*iDb,iRec);
	CleanupStack::PushL(notify);
	CContactItem *item=iDb->OpenContactLX((*IdList)[0]);
	CleanupStack::PushL(item);
	iDb->CommitContactL(*item);
	iRec->Wait(EFalse);	// Check q working again
	CleanupStack::PopAndDestroy(3);	// item, close record, notify
	}

TBool COomContactChangeNotifier::Skip(TInt aCount, TOomFailureModes aMode) const
	{
	// KErrCorrupt:
	if (aMode==KOomFailFileServerGeneral) return aCount == 5 || aCount == 11 || aCount == 17;
	if (aMode==KOomFailFileServerOom) return aCount == 5 || aCount == 11 || aCount == 17;
	return EFalse;
	}

//
// COomPhoneNumberLookup
//

COomPhoneNumberLookup::~COomPhoneNumberLookup()
	{
	delete iAddCard;
	}

void COomPhoneNumberLookup::ConstructL()
	{
	iAddId=KNullContactId;
	iDb=CContactDatabase::ReplaceL(KDatabaseCreateFileName);
	iDbEventQueue = CContactDbEventQueue::NewL(iDb);
	iAddCard=CContactCard::NewL();
	SetNameL(*iAddCard,KUidContactFieldPhoneNumber,KUidContactFieldVCardMapTEL,_L("11223344"),ETrue);
	SetNameL(*iAddCard,KUidContactFieldPhoneNumber,KUidContactFieldVCardMapTEL,_L("22334455"),ETrue);
	}

void COomPhoneNumberLookup::PreFailL()
	{
	iAddId=KNullContactId;
	iAddId=iDb->AddNewContactL(*iAddCard);
	}

void COomPhoneNumberLookup::FailL()
	{
/*	CContactIdArray *matchArray=iDb->PhoneMatchListL(_L("11223344"));
	CleanupStack::PushL(matchArray);
	test(matchArray->Count()==1);
	test((*matchArray)[0]==iAddId);
	CleanupStack::PopAndDestroy();	// matchArray
	matchArray=iDb->PhoneMatchListL(_L("22334455"));
	CleanupStack::PushL(matchArray);
	test(matchArray->Count()==1);
	test((*matchArray)[0]==iAddId);
	CleanupStack::PopAndDestroy();	// matchArray*/
	}

void COomPhoneNumberLookup::ClearUpL()
	{
	RecoverL();
	if (iAddId!=KNullContactId)
		iDb->DeleteContactL(iAddId);
	iAddId = KNullContactId;
	iDb->CompactL();
	}

// 
// COomContactsChangedSince
//

COomContactsChangedSince::~COomContactsChangedSince()
	{
	TRAP_IGNORE(iDb->DeleteContactL(iNewContactId1));
	TRAP_IGNORE(iDb->DeleteContactL(iNewContactId2));
	}

void COomContactsChangedSince::ConstructL()
	{
	COomFailBase::ConstructL();
	iBeforeTime.UniversalTime();
	CContactCard* card=CContactCard::NewL();
	CleanupStack::PushL(card);
	SetNameL(*card,KUidContactFieldFamilyName,KUidContactFieldVCardMapUnusedN,_L("add1"),EFalse);
	iNewContactId1=iDb->AddNewContactL(*card);
	SetNameL(*card,KUidContactFieldFamilyName,KUidContactFieldVCardMapUnusedN,_L("add2"),EFalse);
	iNewContactId2=iDb->AddNewContactL(*card);
	CleanupStack::PopAndDestroy(); // card
	}

void COomContactsChangedSince::FailL()
	{
	CContactIdArray* ids=iDb->ContactsChangedSinceL(iBeforeTime);
	test(ids->Count()==2);
	delete ids;
	}

// 
// COomGroups
//

COomGroups::~COomGroups()
	{
	delete iItem1;
	}

void COomGroups::ConstructL()
	{
	COomFailBase::ConstructL();
	iItem1=(CContactCard *)iDb->ReadContactL((*IdList)[0]);
	}

void COomGroups::FailL()
	{
	iGroupId=KNullContactId;
	CContactGroup* oomGroup=(CContactGroup*)iDb->CreateContactGroupLC(_L("Oom Group"));
	iGroupId=oomGroup->Id();
//
	iDb->AddContactToGroupL((*IdList)[1],iGroupId);
	iDb->AddContactToGroupL(*iItem1,*oomGroup);
//
	iDb->RemoveContactFromGroupL((*IdList)[0],iGroupId);
	iDb->RemoveContactFromGroupL((*IdList)[1],iGroupId);
//
	CleanupStack::PopAndDestroy(oomGroup);
	}

void COomGroups::ClearUpL()
	{
	RecoverL();
	if (iGroupId!=KNullContactId)
		{
		TRAP_IGNORE(iDb->DeleteContactL(iGroupId));
		}
//	test(TestGroupStateL(iDb,-1,-1));
	CContactIdArray* grpList=iDb->GetGroupIdListL(); // This is introduced from 9.3 onwards as deletion of GroupIds was not happening properly.
	TInt count = grpList->Count();					 // during a leave (in COomGroups::FailL() at CContactGroup* oomGroup=(CContactGroup*)iDb->CreateContactGroupLC(_L("Oom Group")); 
													 // a  group is created in db but the ccontactdatabase doesnt append to its list and so it is not cleaned by COomGroups::ClearUpL properly
	if( count > 0 )
	{
		TContactItemId id = (*grpList)[0];
		TRAP_IGNORE(iDb->DeleteContactL(id)); 
	}
	delete grpList;
	}

TBool COomGroups::Skip(TInt, TOomFailureModes aMode) const
	{
	// KErrCorrupt
	// Another one of those moving targets. The value of count at which failure occurs
	// varies between executions.
	if (aMode==KOomFailFileServerGeneral) return ETrue;
	if (aMode==KOomFailFileServerOom) return ETrue;
	return(EFalse);
	}

// 
// COomGroups
//

COomGroups2::~COomGroups2()
	{
	TRAP_IGNORE(iDb->DeleteContactL(iGroupId1));
	TRAP_IGNORE(iDb->DeleteContactL(iGroupId2));
	}

void COomGroups2::ConstructL()
	{
	COomFailBase::ConstructL();
	CContactGroup* oomGroup=(CContactGroup*)iDb->CreateContactGroupLC(_L("Oom Group"));
	iGroupId1=oomGroup->Id();
	CleanupStack::PopAndDestroy(oomGroup);
	iDb->AddContactToGroupL((*IdList)[0],iGroupId1);
	oomGroup=(CContactGroup*)iDb->CreateContactGroupLC(_L("Oom Group"));
	iGroupId2=oomGroup->Id();
	CleanupStack::PopAndDestroy(oomGroup);
	iDb->AddContactToGroupL((*IdList)[0],iGroupId2);
	iDb->AddContactToGroupL((*IdList)[1],iGroupId2);
	}

void COomGroups2::FailL()
	{
	CContactCard *card=(CContactCard *)iDb->ReadContactLC((*IdList)[0]);
//
	CContactIdArray* idArray=card->GroupsJoinedLC();
	test(idArray->Count()==2);
	test((*idArray)[0]==iGroupId1);
	test((*idArray)[1]==iGroupId2);
	CleanupStack::PopAndDestroy(idArray);
	CleanupStack::PopAndDestroy(card);
//
	CContactGroup *group=(CContactGroup *)iDb->ReadContactLC(iGroupId2);
	const CContactIdArray* itemsContained=group->ItemsContained();
	test(itemsContained->Count()==2);
	test((*itemsContained)[0]==(*IdList)[0]);
	test((*itemsContained)[1]==(*IdList)[1]);
	CleanupStack::PopAndDestroy(group);
//
	CContactIdArray* groupIdList=iDb->GetGroupIdListL();
	test(groupIdList->Count()==2);
	test((*groupIdList)[0]==iGroupId1);
	test((*groupIdList)[1]==iGroupId2);
	delete groupIdList;
	}

void COomGroups2::ClearUpL()
	{
	RecoverL();
	test(TestGroupStateL(iDb,-1,-1));
	}

//
// COomTest
//

COomTest::COomTest(TDriveUnit aMmcDrive) : iMmcDrive(aMmcDrive)
	{}

void COomTest::Test(TBool aMustBeTrue)
	{
	test(aMustBeTrue);
	}

TInt COomTest::GetRecordCountL()
	{
	CContactDatabase *db=CContactDatabase::OpenL(KDatabaseFileName);
	CleanupStack::PushL(db);
	TInt recordCount=db->CountL();
	CleanupStack::PopAndDestroy();	// db
	return(recordCount);
	}


void COomTest::DoOomTestL(COomFailBase *aOomTest, TOomFailureModes aMode)
	{
	test(aOomTest!=NULL);
	test.Next(_L(" "));

	__UHEAP_MARK;
	TInt oldDbCount=iDbs.ResourceCount();
	TInt oldFsCount=TheFs.ResourceCount();
//
	CleanupStack::PushL(aOomTest);
	TRAPD(ret1,aOomTest->ConstructL());
	test(ret1==KErrNone);
	CContactDatabase *lockDb=NULL;
	if (aMode==KOomFailContactServer)	// contact server failure tests
		{
		lockDb=CContactDatabase::OpenL(KDatabaseFileName);
		CleanupStack::PushL(lockDb);
		}
	TInt successCount=0;
	TInt skipCount=0;
	for(TInt count=0;;count++)
		{
		TInt ret=KErrNone;
		if (aOomTest->Skip(count,aMode))
			{
			skipCount++;
			}
		else
			{		
			// PREFAILL - Prepare for the test:
			TRAPD(preFailErr,aOomTest->PreFailL());
			test(preFailErr==KErrNone);
			// Set failure condition:
			switch(aMode)
				{
				case KOomFailNormal:
					__UHEAP_SETFAIL(RHeap::EDeterministic,count);
					break;
				case KOomFailDbs:
					iDbs.SetHeapFailure(RHeap::EDeterministic,count);
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
				
			// FAILL - Perform test:
			TRAP(ret,aOomTest->FailL());

			// Remove failure condition:
			switch(aMode)
				{
				case KOomFailNormal:
					__UHEAP_RESET;
					break;
				case KOomFailDbs:
					iDbs.SetHeapFailure(RHeap::ENone,0);
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

			// CLEARUPL - tidy up after the test:
			TRAPD(ClearUpErr,aOomTest->ClearUpL());
			test(ClearUpErr==KErrNone);
			}

		// Verify results of test:
		if (ret==KErrNone)
			{
			if (successCount==8)
				{
				test.Printf(_L("Count=%d"),count);
				if (skipCount>0)
					test.Printf(_L(", skipped=%d"),skipCount);
				test.Printf(_L("\n"));
				break;
				}
			successCount++;
			}
		else
			{
			successCount=0;
			test(aOomTest->ErrorOk(aMode, count, ret));
			}
		}
	if (aMode==KOomFailContactServer)
		CleanupStack::PopAndDestroy();	// lockDb
	CleanupStack::PopAndDestroy();	// aOomTest
	test(oldFsCount==TheFs.ResourceCount());
	test(oldDbCount==iDbs.ResourceCount());
	__UHEAP_MARKEND;
	}

COomTest::~COomTest()
	{
	iDbs.Close();
	}

/**

@SYMTestCaseID     PIM-T-ERROR-0001

*/

TBool COomTest::TestsL()
	{
	iDbs.Connect();
    __UHEAP_MARK;
	TInt oldDbCount=iDbs.ResourceCount();
	for(TInt loop=0;loop<KOomNumFailModes;loop++)
		{
		if (loop > 0)
			{ // increment the RTest number XXX
			test.Next(_L("--- Next Error case ---"));

			}

		TOomFailureModes mode=(TOomFailureModes)loop;
		switch(mode)
			{
			case KOomFailNormal: // RTest 001.01
				test.Start(_L("@SYMTESTCaseID:PIM-T-ERROR-0001 ===Test Out Of Memory==="));

				break;
			case KOomFailFileServerGeneral: // RTest 002.01
				test.Start(_L("===Test File server general failure==="));

				break;
			case KOomFailFileServerOom: // RTest 003.01
				test.Start(_L("===Test File server Oom failure==="));

				break;
			case KOomFailContactServer: // RTest 004.01
				test.Start(_L("===Test Contact Server Oom failure==="));

				break;
			case KOomFailDbs: // RTest 005.01
				test.Start(_L("===Test DBS Oom failure==="));

				break;
			default:;
			}
		// RTest X.01
		test.Next(_L("Create Database for OOM test"));

		TRAPD(ret,CreateDatabaseL());
		test(ret==KErrNone);
		IdList=CContactIdArray::NewLC();
		test.Next(_L("Create Contacts for OOM test"));

		TRAP(ret,AddNewContactsL());
		test(ret==KErrNone);

		DoOomTestL(new(ELeave) COomOpen(this),mode);
		DoOomTestL(new(ELeave) COomCreate(this),mode);	
		DoOomTestL(new(ELeave) COomFind(this),mode);
		DoOomTestL(new(ELeave) COomFindAsync(this),mode);
		DoOomTestL(new(ELeave) COomFindInTextDef(this),mode);
		DoOomTestL(new(ELeave) COomFindAsyncInTextDef(this),mode);
		DoOomTestL(new(ELeave) COomSort(this),mode);
		DoOomTestL(new(ELeave) COomSortArray(this),mode);  
		DoOomTestL(new(ELeave) COomSetDatabaseDrive(this, iMmcDrive),mode);	
		DoOomTestL(new(ELeave) COomGetDefaultName(this),mode);
		DoOomTestL(new(ELeave) COomViewDefinition(this),mode);
		DoOomTestL(new(ELeave) COomTextDefinition(this),mode);
		DoOomTestL(new(ELeave) COomReadContact(this),mode);
		DoOomTestL(new(ELeave) COomAddNewContact(this),mode);
		DoOomTestL(new(ELeave) COomReadContactTextDef(this),mode);
		DoOomTestL(new(ELeave) COomOpenAndCloseContactX(this),mode);
		DoOomTestL(new(ELeave) COomOpenAndCloseContact(this),mode);
		DoOomTestL(new(ELeave) COomOpenModifyAndCommitContact(this),mode);
		DoOomTestL(new(ELeave) COomDeleteContact(this),mode);
		DoOomTestL(new(ELeave) COomDeleteContacts(this),mode);
		DoOomTestL(new(ELeave) COomCompact(this),mode);
		DoOomTestL(new(ELeave) COomImportContacts(this),mode);
		DoOomTestL(new(ELeave) COomExportSelectedContacts(this),mode);
		DoOomTestL(new(ELeave) COomCompress(this),mode);
		// This test damages the database and attempts to recover it which
		// is not possible in the UREL build.		
#ifdef _DEBUG
		DoOomTestL(new(ELeave) COomRecover(this),mode);
#endif
		DoOomTestL(new(ELeave) COomCloseOpenTables(this),mode);
		DoOomTestL(new(ELeave) COomViewDefInternalize(this),mode);
		DoOomTestL(new(ELeave) COomViewDefExternalize(this),mode);
		DoOomTestL(new(ELeave) COomFieldSetStore(this),mode);
		DoOomTestL(new(ELeave) COomFieldSetRestore(this),mode);
		DoOomTestL(new(ELeave) COomContactCardClone(this),mode);
		DoOomTestL(new(ELeave) COomContactChangeNotifier(this),mode);  
		DoOomTestL(new(ELeave) COomPhoneNumberLookup(this),mode);
		User::After(1000000);	//	fixes defect DEF039417
		DoOomTestL(new(ELeave) COomContactsChangedSince(this),mode);
		DoOomTestL(new(ELeave) COomGroups2(this),mode);
		DoOomTestL(new(ELeave) COomGroups(this),mode);
		CleanupStack::PopAndDestroy(); // IdList
		test.End();
		}

	test(oldDbCount==iDbs.ResourceCount());
    __UHEAP_MARKEND;
	return(ETrue);
	}

/////////////////////////////////

// Code for DrivesOfMediaTypeL() and MMCDriveL() adapted from T_Sec_CntDbase

LOCAL_C TBool FirstDriveOfMediaTypeL(TMediaType aMedia, TInt& aDriveNumber)
	{
	TDriveInfo info;
	TInt error(KErrNone);
	TBool result(EFalse);

	TDriveList driveList;
	error = TheFs.DriveList(driveList);
	User::LeaveIfError(error);

	for(TInt drive=EDriveA; drive<=EDriveZ; drive++)
		{
		if( driveList[drive] )
			{
			error = TheFs.Drive(info, drive);
			if	(error != KErrNone)
				{
				continue;
				}
				
			if	(info.iType == aMedia)
				{
				aDriveNumber = drive;
				result = ETrue;
				
				// return if only the first drive of the required type.
				break;
				}
			}
		}

	return result;
	}

/**
	MMCDriveL
	
	Determines a list of drives of type EMediaHardDisk.
	The first drive in this list is returned and used
	by the rest of the test.

        NOTE: if this test is running on ARMv5, an MMC card
        must be present otherwise the test fails.

*/
LOCAL_C TDriveUnit MMCDriveL()
	{
	TInt driveNumber = 0;
	
	if( !FirstDriveOfMediaTypeL(EMediaHardDisk, driveNumber) )
		{
		test.Printf( _L("No drives found of type EMediaHardDisk\n"));
		User::LeaveIfError(KErrHardwareNotAvailable);
		}

	TDriveUnit mmcDrive(driveNumber);
	TPtrC drvPtr(mmcDrive.Name());
	test.Printf(_L("Selected drive of type EMediaHardDisk for test use is %S\n"), &drvPtr);

	// drive selected fortest to use
	return mmcDrive;
	}


LOCAL_C void OomTestsL()
	{
	// usable drive for SetDatabaseDriveL test
	TDriveUnit mmcDrive = MMCDriveL();

	COomTest *oomTest=new(ELeave) COomTest(mmcDrive);
	CleanupStack::PushL(oomTest);
//
	oomTest->TestsL();
//
	CleanupStack::PopAndDestroy(oomTest);
	}


void DoTests()
    {
	test.Start(_L("Error tests"));

	User::LeaveIfError(TheFs.Connect());

	TRAPD(ret,OomTestsL());
	test(ret==KErrNone);
	test.End();
	
	// cleanup disk space
   	TRAP_IGNORE(CContactDatabase::DeleteDatabaseL(KDatabaseFileName));
    TRAP_IGNORE(CContactDatabase::DeleteDatabaseL(KDatabaseCreateFileName));
	TRAP_IGNORE(CContactDatabase::DeleteDefaultFileL());

	test(TheFs.ResourceCount()==0);
	TheFs.Close();

	// propagate error
	User::LeaveIfError(ret);
    }

GLDEF_C TInt E32Main()
	{
    __UHEAP_MARK;
	CActiveScheduler::Install(new(ELeave) CActiveScheduler);
    CTrapCleanup* cleanup=CTrapCleanup::New();
	test.Title();
    DoTests();
    test.Close();
    delete cleanup;
	delete CActiveScheduler::Current();
    __UHEAP_MARKEND;
	return KErrNone;
    }
