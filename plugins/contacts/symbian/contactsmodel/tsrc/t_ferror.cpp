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
#include <e32math.h>
#include <f32file.h>
#include <s32file.h>
#include <cntdb.h>
#include <cntitem.h>
#include <cntfield.h>
#include <cntfldst.h>
#include "t_utils.h"

// Type definitions
CCntTest* CntTest=NULL;
LOCAL_D RTest test(_L("T_FERROR"));
LOCAL_D RFile logFile;
LOCAL_D RFile threadLogFile;

const TPtrC KDatabaseFileName=_L("C:T_FERROR");
const TPtrC KDatabaseRemoveableFileName=_L("D:T_FERROR2");

const TPtrC KLogFileName=_L("C:\\T_FERROR.LOG");
const TPtrC KThreadLogFileName=_L("C:\\T_FERROR2.LOG");
const TInt KNumTestContacts=5;

class CFred2 : public CBase
	{
public:
	~CFred2();
	void ConstructL();
	void WaitForLogon();
	void Kill();
	void DoDamageL();
protected:
	RThread iThread;
	TRequestStatus iLogonStatus;
	};

class CContactReadWriter : public CIdle, public MContactDbObserver
	{
public:
	static CContactReadWriter *NewLC();
	static TInt CallbackL(TAny *xThis);
private:
	TInt DoWorkL();
	CContactReadWriter();
	~CContactReadWriter();
	void SetNameL(CContactItem& aItem,TUid aVcardType,const TDesC& aName);
	TPtrC Name(CContactItem& aItem);
	TBool AddNewContactsL();
	TBool EditContactsL();
	TBool DeleteContactsL();
	void PrintRecovering() const;
	void PrintErr(TInt aErr) const;
public: // from MContactDbObserver
	void HandleDatabaseEventL(TContactDbObserverEvent aEvent);
private:
	TBool iSuspendWork;
	TInt iSubState;
	TInt iState;
	CContactDatabase* iDb;
	CArrayFix<TContactItemId>* iIdList;
	CContactChangeNotifier *iNotify;
	};

LOCAL_C void doWriteToLogL(RFile &aFile, const TDesC &aLog, TInt aParam, TInt aParam2)
	{
	TBuf<128> buf;
	TTime time;
	time.UniversalTime();
	time.FormatL(buf,_L("%T:%S.%*C3-"));
	TFileText textFile;
	textFile.Set(aFile);
	User::LeaveIfError(textFile.Write(buf));
	buf.Format(aLog,aParam,aParam2);
	User::LeaveIfError(textFile.Write(buf));
	aFile.Flush();
	}

LOCAL_C void WriteToLogL(const TDesC &aLog, TInt aParam=0, TInt aParam2=0)
	{
	doWriteToLogL(logFile,aLog,aParam,aParam2);
	}

LOCAL_C void WriteToThreadLogL(const TDesC &aLog, TInt aParam=0, TInt aParam2=0)
	{
	doWriteToLogL(threadLogFile,aLog,aParam,aParam2);
	}

//
// CContactReadWriter
//

void CContactReadWriter::HandleDatabaseEventL(TContactDbObserverEvent aEvent)
	{
	if (aEvent.iType==EContactDbObserverEventTablesClosed)
		iSuspendWork=ETrue;
	else if (aEvent.iType==EContactDbObserverEventTablesOpened)
		iSuspendWork=EFalse;
	else if (aEvent.iType==EContactDbObserverEventRollback)
		{
		PrintRecovering();
		TRAP_IGNORE(iDb->RecoverL());
		}
	WriteToLogL(_L("Event(%d,%d)"),aEvent.iType,aEvent.iContactId);
	}

void CContactReadWriter::SetNameL(CContactItem& aItem,TUid aVcardType,const TDesC& aName)
//
// Set the contents of a text field, creating the field if required
//
	{
	CContactItemFieldSet& fieldSet=aItem.CardFields();
	const TInt pos=fieldSet.Find(KUidContactFieldFamilyName);
	if (pos!=KErrNotFound)
		fieldSet[pos].TextStorage()->SetTextL(aName);
	else
		{
		CContactItemField* field=CContactItemField::NewLC(KStorageTypeText,KUidContactFieldFamilyName);
		field->SetMapping(aVcardType);
		field->TextStorage()->SetTextL(aName);
		aItem.AddFieldL(*field);
		CleanupStack::Pop(); // item
		}
	}

TPtrC CContactReadWriter::Name(CContactItem& aItem)
	{
	CContactItemFieldSet& fieldSet=aItem.CardFields();
	const TInt pos=fieldSet.Find(KUidContactFieldFamilyName);
	if (pos==KErrNotFound)
		return _L("");
	return fieldSet[pos].TextStorage()->Text();
	}

TBool CContactReadWriter::AddNewContactsL()
	{
	CContactCard* card=CContactCard::NewL();
	CleanupStack::PushL(card);
	TBuf<16> name;
	name.Format(_L("NAME #%d"),iSubState);
	SetNameL(*card,KUidContactFieldVCardMapUnusedN,name);
	CContactItemField* field=CContactItemField::NewLC(KStorageTypeText);
	card->AddFieldL(*field);
	CleanupStack::Pop(); // field
	iIdList->AppendL(iDb->AddNewContactL(*card));
	CleanupStack::PopAndDestroy(); // card
	iSubState++;
	return(iSubState==KNumTestContacts);
	}

TBool CContactReadWriter::EditContactsL()
//
// Check then edit contact names
//
	{
	if (iSubState<KNumTestContacts)
		{
		const TInt index=(*iIdList)[iSubState];
		CContactItem* item=iDb->OpenContactLX(index);
		CleanupStack::PushL(item);
		TBuf<16> name;
		name.Format(_L("NAME #%d"),iSubState);
		test(name==Name(*item));
		name.Format(_L("NEW NAME #%d"),index);
		SetNameL(*item,KUidContactFieldVCardMapUnusedN,name);
		iDb->CommitContactL(*item);
		CleanupStack::PopAndDestroy(); // item;
		CleanupStack::Pop(); // Close from OpenContactLX
		}
	else if (iSubState<(2*KNumTestContacts))
		{
		const TInt index=(*iIdList)[iSubState-KNumTestContacts];
		CContactItem* item=iDb->ReadContactL(index);
		TBuf<16> name;
		name.Format(_L("NEW NAME #%d"),index);
		test(name==Name(*item));
		delete item;
		}
	return(++iSubState==2*KNumTestContacts);
	}

TBool CContactReadWriter::DeleteContactsL()
//
// Delete all contacts
//
	{
	if (iDb->CountL()==0)
		return(ETrue);
	iDb->DeleteContactL((*iDb->SortedItemsL())[0]);
	return(EFalse);
	}

////////////////////////////////////////////////////////////

CContactReadWriter::CContactReadWriter() : CIdle(CActive::EPriorityIdle)
	{
	}

CContactReadWriter *CContactReadWriter::NewLC()
	{
	CContactReadWriter *rw=new(ELeave) CContactReadWriter();
	CleanupStack::PushL(rw);
	CActiveScheduler::Add(rw);
	rw->Start(TCallBack(CContactReadWriter::CallbackL,rw));
	return(rw);
	}

CContactReadWriter::~CContactReadWriter()
	{
	delete iNotify;
	delete iIdList;
	delete iDb;
	}

TInt CContactReadWriter::DoWorkL()
	{
	if (iSuspendWork)
		{
		WriteToLogL(_L("Suspended"));
		User::After(100000);
		return(ETrue);
		}
	WriteToLogL(_L("DoWork(%d,%d)"),iState,iSubState);
	TBool moveToNextState=ETrue;
	switch(iState)
		{
		case 0:
			test(iDb==NULL);
			User::After(200000);	// Increase the chance of a damage hit here
			iDb=CContactDatabase::OpenL(KDatabaseFileName);
			while(iDb->IsDamaged())
				{
				WriteToLogL(_L("Recovering"));
				TRAP_IGNORE(iDb->RecoverL());
				}
			iNotify=CContactChangeNotifier::NewL(*iDb,this);
			iIdList=new(ELeave) CArrayFixFlat<TContactItemId>(5);
			break;
		case 1:
			moveToNextState=AddNewContactsL();
			break;
		case 2:
			moveToNextState=EditContactsL();
			break;
		case 3:
			moveToNextState=DeleteContactsL();
			break;
		case 4:
			CActiveScheduler::Stop();
			return(EFalse);
		}
	if (moveToNextState)
		{
		iState++;
		iSubState=0;
		}
	return(ETrue);
	}

void CContactReadWriter::PrintRecovering() const
	{
	test.Printf(_L("Recovering:"));
	}

void CContactReadWriter::PrintErr(TInt aErr) const
	{
	TBuf<64> errText;
	errText.Format(_L("Err %d:"),aErr);
	test.Printf(errText);
	}

TInt CContactReadWriter::CallbackL(TAny *aThis)
	{
	CContactReadWriter *xthis=(CContactReadWriter *)aThis;
	TInt ret=ETrue;
	TRAPD(err,ret=xthis->DoWorkL());
	if (err!=KErrNone)
		{
		xthis->PrintErr(err);
/*		if (xthis->iDb)
			{
			TRAP_IGNORE(xthis->iDb->RecoverL());
			}*/
		}
	return(ret);
	}

////////////////////////////////////////////////////////////

LOCAL_C void CreateDatabaseL()
//
// Create a database in a store
//
	{
	CntTest->CreateDatabaseL();
	CntTest->DeleteAllTemplateFieldsL();
	CntTest->CloseDatabase();
	}
	
void TestLoop(TInt aNumLoops)
    {
	CFred2 *fred2=new(ELeave) CFred2;
	CleanupStack::PushL(fred2);
	fred2->ConstructL();
	for(TInt loop=0;loop<aNumLoops;loop++)
		{
		CContactReadWriter::NewLC();
		CActiveScheduler::Start();
		CleanupStack::PopAndDestroy();	// CContactReadWriter
		test.Printf(_L("."));
		}
	test.Printf(_L("\n"));
	CleanupStack::PopAndDestroy();	// fred2
	}

LOCAL_C void DriveErrMsg()
	{
	test.Printf(_L("Error: For this test to run properly "));
#if defined(__WINS__)
	test.Printf(_L("_EPOC_DRIVE_D must be set up as a removable drive for this test\n"));
#else
	test.Printf(_L("a removable/writable D: must exist\n"));
#endif
	test.Getch();
	}

void RemovePackTestL()
	{
	RFs fs;
	test(fs.Connect()==KErrNone);
//
	TDriveInfo driveInfo;
	if (fs.Drive(driveInfo,EDriveD)!=KErrNone || driveInfo.iType==EMediaNotPresent || driveInfo.iMediaAtt&KMediaAttWriteProtected)
		DriveErrMsg();
	else
		{
		TInt err=fs.MkDirAll(_L("D:\\___t___\\"));
		if (err!=KErrNone)
			DriveErrMsg();
		else
			{
			fs.RmDir(_L("D:\\___t___\\"));
			CContactDatabase *remDb=CContactDatabase::ReplaceL(KDatabaseRemoveableFileName);
			CleanupStack::PushL(remDb);
			test.Printf(_L("Remove pack and press a key\n"));
			test.Getch();
			TInt addErr=KErrNone;
			do
				{
				if (addErr!=KErrNone)
					{
					test.Printf(_L("Add error %d,"),addErr);
					TRAP(addErr,remDb->RecoverL());
					test.Printf(_L("Recover error %d"),addErr);
					TRAP(addErr,remDb->OpenTablesL());
					test.Printf(_L("Open tables error %d\n"),addErr);
					test.Printf(_L("Press a key to try again\n"));
					test.Getch();
					}
				if (addErr==KErrNone)
					{
					TRAP(addErr,AddContactL(remDb,KUidContactFieldFamilyName,KUidContactFieldVCardMapUnusedN,_L("x")));
					}
				} while(addErr!=KErrNone);
			CleanupStack::PopAndDestroy();	// remDb
			}
		}
	}

/**

@SYMTestCaseID     PIM-T-FERROR-0001

*/

void DoTestsL()
    {
	test.Start(_L("@SYMTESTCaseID:PIM-T-FERROR-0001 Remove pack test"));

	RemovePackTestL();

	test.Next(_L("File error tests"));

	User::LeaveIfError(logFile.Replace(CntTest->Fs(),KLogFileName,EFileStreamText|EFileWrite));
	CleanupClosePushL(logFile);
//
	test.Next(_L("Create new database"));

	WriteToLogL(_L("Starting Logging"));
	TRAPD(ret,CreateDatabaseL());
	test(ret==KErrNone);
	test.Next(_L("Starting fail loop"));

	TestLoop(50);
	test.Next(_L("Delete database"));

	TInt retDel=KErrNone;
	do
		{
		TRAP(retDel,CntTest->DeleteDatabaseL());
		if (retDel==KErrInUse)
			{
			User::After(200000);
			}
		else
			{
			test(retDel==KErrNone);
			}
		} while(retDel!=KErrNone);
	WriteToLogL(_L("Close log"));
	CleanupStack::PopAndDestroy();	// logfile
    }

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

LOCAL_C TInt FredIIFunc(TAny *aParam);

CFred2::~CFred2()
	{
	Kill();
	iThread.Close();
	}

void CFred2::ConstructL()
	{
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
	test(iLogonStatus.Int()==KErrNone);
	}

//
// Fred II from here on
// 

LOCAL_C TInt FredIIFunc(TAny *aParam)
	{
	CActiveScheduler::Install(new(ELeave) CActiveScheduler);
    CTrapCleanup* cleanup=CTrapCleanup::New();
	TRAPD(err,((CFred2 *)aParam)->DoDamageL());
	test(EFalse);
    delete cleanup;
	delete CActiveScheduler::Current();
	return(err);
	}

#if defined(__WINS__)
 const TInt KMaxDamageWait=500000;	// up to a half a second
#else
# if defined(_UNICODE)
 const TInt KMaxDamageWait=1500000;	// up to a one second
# else
 const TInt KMaxDamageWait=1000000;	// up to one and a half a second
# endif
#endif

void CFred2::DoDamageL()
	{
	RFs fs;
	test(fs.Connect()==KErrNone);
	CleanupClosePushL(fs);
	User::LeaveIfError(threadLogFile.Replace(fs,KThreadLogFileName,EFileStreamText|EFileWrite));
	CleanupClosePushL(threadLogFile);
	WriteToThreadLogL(_L("test1"));
//
	WriteToThreadLogL(_L("test2"));
	CContactDatabase* db=CContactDatabase::OpenL(KDatabaseFileName);
	WriteToThreadLogL(_L("test3"));
	CleanupStack::PushL(db);
	TBool doRecover=EFalse;
	TInt64 seed=0;
	User::After(1000000);
	TInt damageCount=0;
	TInt error1=KErrNone;
	TInt error2=KErrNone;
	WriteToThreadLogL(_L("test4"));
	FOREVER
		{
		TInt after=(Math::Rand(seed)%KMaxDamageWait);
		User::After(after);
		WriteToThreadLogL(_L("Doing damage"));
		TRAP(error1,db->DamageDatabaseL(0x666));
		damageCount++;
		FOREVER
			{
			if (doRecover)
				{
				WriteToThreadLogL(_L("Start Recover"));
				TRAP(error2,db->RecoverL());
				WriteToThreadLogL(_L("Finished Recover"));
				}
			else
				{
				WriteToThreadLogL(_L("Close tables"));
				db->CloseTables();
				TRAP(error2,db->OpenTablesL());
				WriteToThreadLogL(_L("Open tables %d"),error2);
				}
			if (error2==KErrNone)
				break;
			User::After(100000);
			}
		doRecover=!doRecover;
		}
//unreachable at the mo'	CleanupStack::PopAndDestroy(3);	// db, log file, fs
	}
