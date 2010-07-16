/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32property.h>
#include <app-services/private/cbackuprestorenotification.h>

//
// Forward declarations.
//
class CContactsClient;
class MBackupRestoreObserver;
class CBackupHelper;


//
// MBackupRestoreObserver.
//
class MBackupRestoreObserver
	{
public:
	virtual void ContactsBackupRestoreObserver(CContactsClient& aContactClient, const TBool aRestore, const TBool aStart) = 0;
	};


/*
 CContactsClient
 This class models, through its public methods, various Contacts client behaviours.
 It is derived from CActive so that it can safely respond to Backup & Restore notifications
 - e.g. closing a database in the notification handler causes a panic. (The active object
 delivering the notification is deleted whilst it is running, and cannot recover.)
 Instead the active object must be set to run - and the RunL method can safely close the database.
 If called, the RunL also closes any contact or view that is open.
 */
class CContactsClient : public CActive, public MContactDbObserver, public MContactUiCompactObserver, public MIdleFindObserver
	{
public:
	static CContactsClient* NewLC(const TDesC& aDatabaseName, MBackupRestoreObserver* aObserver, MContactViewObserver& aContactViewObserver, TBool aCloseForBackup);
	~CContactsClient();

	void StartActiveClose();
	void CloseDatabase();
	void OpenDatabaseL();
	
	const TDesC& DatabaseName();
	TBool IsOpen();

	void OpenLocalViewL();
	void OpenRemoteViewL();
	void CloseViews();

	void OpenContactL(TContactItemId aContactId);
	void CommitContactL(TContactItemId aContactId);
	void CloseContacts();
	void ReadContactL();
	void FindPhoneNumberL();

	void StartAsyncFindL();
	void StartActiveCompactL();
	void StartActiveRecoverL();
	void StartIdleSorterL(TRequestStatus& aStatus);

	void CompleteAsyncFindL();
	void CompleteActiveCompactL();
	void CompleteActiveRecoverL();


	// Speed Dial Check, Remove and Set - for testing cntmodel.ini is Backed up and Restored correctly
	void CheckSpeedDialSetL(TContactItemId aContactId, TInt aSpeedDialPosition);
	void RemoveSpeedDialL(TContactItemId aContactId, TInt aSpeedDialPosition);
	void SetSpeedDialL(TContactItemId aContactId, TInt aSpeedDialPosition);
	
	TBool IsCloseForBackup();

	// class MContactUiCompactObserver
public:
	void Step(TInt aStep);
	void HandleError(TInt aError);

	// class MIdleFindObserver
	void IdleFindCallback();

private:
	CContactsClient(MBackupRestoreObserver* aObserver, MContactViewObserver& aContactViewObserver, TBool aCloseForBackup);
	void ConstructL(const TDesC& aDatabaseName);

	// from MContactDbObserver - observed for Backup & Restore events
	void HandleDatabaseEventL(TContactDbObserverEvent aEvent);

	// CActive
	void RunL();
	void DoCancel();

private:
	MBackupRestoreObserver* iObserver;
	MContactViewObserver& iContactViewObserver;
	HBufC* iDbName;
	CContactDatabase* iDb;
	CContactChangeNotifier* iNotifier;
	TBool iDatabaseIsOpen;
	TBool iCloseForBackup;
	RPointerArray<CContactItem> iOpenContacts;
	TBool iRestore;
	CContactLocalView* iLocalView;
	CContactRemoteView* iRemoteView;
	// resources for Async CContactDatabase operations
	CIdleFinder* iIdleFinder;
	CContactActiveRecover* iActiveRecover;
	CContactActiveCompress* iActiveCompress;
	CContactItemFieldDef* iItemFieldDef;
	};


/*
 CBackupTestConductor.
 This class controls the whole unit test framework:
 + Creating and populating test databases
 + Checking backup registration policy file is present on drize Z:
 + Checking Secure Backup has processed the policy file as intended
 + exercising through CBackupTestFramework a variety of Backup & Restore with a variety of client behaviours
 + Cleaning up after the test
 */
class CBackupTestConductor: public CBase
	{
public:
	static CBackupTestConductor* NewL();
	void RunTestsL();
	~CBackupTestConductor();
	static void DeleteDatabases();

private:
	CBackupTestConductor();
	void ConstructL();

	// database creation / checking / cleanup
	void CreateDatabasesL();
	void PopulateDatabaseL(CContactDatabase* aDb,const TDesC& aFileName);
	void ShowFieldsL(CContactDatabase* aDb);

	// check that the database backup policy is on the C: drive
	TBool CheckXMLBackupRegistrationExistsL();

private:
	// first and last contact ids added to each database
	TContactItemId iFirstContactId;
	TContactItemId iLastContactId;
	};




class CBackupTestFramework : public CTimer, public MBackupRestoreObserver, public MContactViewObserver
	{
public:
	static CBackupTestFramework* NewLC();

	void DoBackupRestoreTestsL();
	void BackupRestoreTestL();


	enum TClientActivity
		{
		// activities that do not block backup / restore
		EIdleClient,
        ERestoreCorruptDbTest,
		EOpenLocalView, 
		EOpenRemoteView,
		// activities that can block backup / restore
		EOpenOneContactForEdit,
		EOpenTwoContactsForEdit,
		EContactsAsyncFind,
		EContactsActiveCompact,
		EContactsActiveRecover,
		EContactsIdleSorter,
		EContactsIniFileTest,
		EOpenRemoteViewOneContactForEdit,
		EMultipleOpenOneContactForEdit
		};
		
		enum TClientCloseForBackup
		{
		EAllOpenForBackup,
		EAllCloseForBackup,
		EHalfCloseForBackup
		};	

	struct TBackupRestoreTestCase
		{
		const TText*	iTestTitle;
		TInt			iNumDatabases;
		TInt			iClientsPerDatabase;
		TClientActivity	iActivity;
		TBool			iExpectedBackupSuccess;
		TClientCloseForBackup iCloseForBackup;
		conn::TBackupIncType  iBackupIncType;
		};


	CContactsClient* NewDatabaseClientL(const TDesC& aDatabaseName, TBool aCloseForBackup);

	// These methods implement the Use Case under test
	TBool PrepareBackupClientTestCaseL(TRequestStatus& aStatus);
	TBool PrepareTestCaseL(const TDesC& aDatabaseName, TRequestStatus& aStatus);

	void PreBackupNotificationActionsL();
	void PostBackupActionsL();
	void PreRestoreNotificationActionsL();
	void PostRestoreActionsL();
	
	void ReconnectBackupClientTestCaseL();
	void CheckConnectionAfterBackupForTestCaseL();

	
	~CBackupTestFramework();

	// from MBackupRestoreObserver
	void ContactsBackupRestoreObserver(CContactsClient& aContactClient, const TBool aRestore, const TBool aStart);

	// from MContactViewObserver - observed for View Unavailable and Ready events
	void HandleContactViewEvent(const CContactViewBase& aView,const TContactViewEvent& aEvent);
	
private:
	// all steps in the Backup and Restore test cycle
	enum TBackupRestoreSteps
		{
		// Backup test steps
		EPreBackup,
		ENotifyBackupStart,
		ECheckBackupStartNotifications,
		EDoBackup,
		ENotifyBackupEnd,
		ECheckBackupEndNotifications,
		EPostBackup,
		EFinishedBackup,
		// Restore test steps
		EPreRestore,
		ENotifyRestoreStart,
		ECheckRestoreStartNotifications,
		EDoRestore,
		ENotifyRestoreEnd,
		ECheckRestoreEndNotifications,
		EPostRestore,
		EFinishedRestore,
		};
private:
	CBackupTestFramework();
	void ConstructL();

	// From CActive.
	void RunL();
	TInt RunError(TInt aError);


private:
	const CBackupTestFramework::TBackupRestoreTestCase* iTestCase;
	TInt iTestError;
	TInt iClientNotifications;
	TInt iViewReadyCount;
	TInt iViewUnavailableCount;
	TBackupRestoreSteps iTestStep;
	CBackupHelper* iBackupHelper;
    RPIMTestServer iTestServ;

	RPointerArray<CContactsClient>	iContactClientSession;
	};


class CBackupHelper : CBase
	{
public:
	static CBackupHelper* NewL(RPIMTestServer& aTestServer);
	
	~CBackupHelper();

	void StartBackupL(conn::TBURPartType aBURPartType, conn::TBackupIncType aBackupIncType);
	void EndBackupL();

	void StartRestoreL();
	void EndRestoreL();

	TBool SimulateIniBackup();
	TBool SimulateDbBackup();
	TBool SimulateIniRestore();
	TBool SimulateDbRestore();
    TBool SimulateDamagedDbRestore();

private:
	CBackupHelper(RPIMTestServer& aTestServer);
	void ConstructL();

private:
	TDriveList iDriveList;
	conn::CSBEClient* iBackupClient;
    RPIMTestServer& iTestServ;
	};

