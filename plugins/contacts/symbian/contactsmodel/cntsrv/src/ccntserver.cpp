/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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


/**
 @file
 @internalComponent
 @released
*/


#include <bacline.h>
#include "ccntserver.h"
#include "ccntsession.h"
#include "ccntdbmanagercontroller.h"
#include "ccntpackager.h"
#include "ccntipccodes.h"
#include "ccntlogger.h"
#include "ccntmsghandler.h"
#include "ccntitemmsghandler.h"
#include "ccnteventmsghandler.h"
#include "ccnttransactionmsghandler.h"
#include "ccntfilemanagermsghandler.h"
#include "ccntpropertiesmsghandler.h"
#include "ccntviewmsghandler.h"

// Contacts server name.
_LIT(KCntServerName,"CNTSRV");


#define KPolicyElementReadUserData 0
#define KPolicyElementWriteUserData 1


const TUint KRangeCount = 4; 


// Categorized opcodes for checking.
const TInt KOpCodeRanges[KRangeCount] = 
	{	
	KCapabilityNone,
	KCapabilityReadUserData,
	KCapabilityWriteUserData,
	ELockSrvNotSupported
	};


// Define the policy that is to be performed on opcode in the ranges defined
// above.
const TUint8 KElementsIndex[KRangeCount] =
	{
	CPolicyServer::EAlwaysPass, 
	KPolicyElementReadUserData, 
	KPolicyElementWriteUserData,
	CPolicyServer::ENotSupported
	};


// Define the action to take for specific capabilites if defined policy check
// fails.
const CPolicyServer::TPolicyElement KPolicyElements[] = 
	{
	{_INIT_SECURITY_POLICY_C1(ECapabilityReadUserData), CPolicyServer::EFailClient}, // KPolicyElementReadUserData
	{_INIT_SECURITY_POLICY_C1(ECapabilityWriteUserData), CPolicyServer::EFailClient} // KPolicyElementWriteUserData
	};


// Define the TPolicy object that is used by CPolicyServer.
const CPolicyServer::TPolicy KCntServerPolicy =
	{
	CPolicyServer::EAlwaysPass, // Specifies all connect attempts should pass.
	KRangeCount,
	KOpCodeRanges,
	KElementsIndex,				// What each range is compared to.
	KPolicyElements,			// What policies range is compared to.
	};

 	
CServerShutdown* CServerShutdown::NewL()
	{
	CServerShutdown* self = new(ELeave) CServerShutdown;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}


CServerShutdown::CServerShutdown()
	:CTimer(CActive::EPriorityIdle)
	{
	CActiveScheduler::Add(this);
	}


void CServerShutdown::Start()
	{
	After(KServerShutdownDelay);
	}


void CServerShutdown::ConstructL()
	{
	CTimer::ConstructL();
	}


void CServerShutdown::RunL()
	{
	CActiveScheduler::Stop();
	}


CCntServer* CCntServer::NewL()
	{
	CCntServer * server = new (ELeave) CCntServer();
	CleanupStack::PushL(server);
	server->ConstructL();

#ifdef __VERBOSE_DEBUG__
 #ifdef _DEBUG
    DEBUG_PRINT1(__VERBOSE_DEBUG__,_L("[CNTMODEL] Starting server in UDEB mode"));
 #else
	DEBUG_PRINT1(__VERBOSE_DEBUG__,_L("[CNTMODEL] Starting server in UREL mode"));
 #endif
#endif
	CleanupStack::Pop(server);
	return server;
	}


CCntServer::CCntServer()
	: CPolicyServer(KContactsServerPriority, KCntServerPolicy, ESharableSessions)
	{
	}


CCntServer::~CCntServer()
	{
	delete iController;
	delete iPackager;
	delete iContainerIndex;
	delete iServerShutdown;
	
    // Cleanup RHashMap
	iMsgLut.Close();
	}


/**
CSession2 calls this method to create subsession object container.
*/
CObjectCon* CCntServer::NewContainerL()
	{
	return iContainerIndex->CreateL();
	}


/**
CSession2 calls this method to remove subsession object container.
*/
void CCntServer::RemoveObjectContainer(CObjectCon& aContainer)
	{
	iContainerIndex->Remove(&aContainer);
	}


void CCntServer::ConstructL()
	{
	// Check the command line arguments to see if the server
	// should be started in non-transient mode.
	iServerType = ETransientServer;
	_LIT(KNonTransientServerArg, "-nontransient");
	CCommandLineArguments* args = CCommandLineArguments::NewL();
	CleanupStack::PushL(args);
	// The first arg is always the .exe name, so start at the next item.
	const TInt count = args->Count();
	for (TInt i = 1; i < count; i++)
		{
		if (args->Arg(i).CompareF(KNonTransientServerArg) == 0)
			{
			iServerType = ENonTransientServer;
			}
		}
	CleanupStack::PopAndDestroy(args);
		
	// Initiate the session ID counter for used to identify each client session.
	iNextSessionId = 1;
	
	/** Create the lookup table which will map from message functions (op codes) to 
	message handling methods. This will be a single instance of lookup table 
	used across all sessions.
	*/
	CreateMessageMapL();

	// Create the packager object used to unpack/pack messages from/to the
	// client.
	iPackager = CCntPackager::NewL();

	// Create container for subsession objects.
	iContainerIndex=CObjectConIx::NewL();

	// Create the one and only instance of the CCntDbManagerController used to
	// manage all open databases.
	iController = CCntDbManagerController::NewLC(iServerType);
	CleanupStack::Pop(iController);
	
	// Create Active Object which will close the server if no clients are
	// connected for 5 seconds.
	iServerShutdown = CServerShutdown::NewL();
	
	// Base class call registers the server in the system.
	StartL(KCntServerName);
	}


/**
Create a new session on the server.  Maps to the client-side Connect() method.
The client can perform general database file control operations once Connect()
has completed.
*/
CSession2* CCntServer::NewSessionL(const TVersion& /*aVersion*/,const RMessage2& /*aMessage*/) const
	{
	CCntSession* session = CCntSession::NewL(*iPackager, iNextSessionId);
	
	// New client session so increment session ID.
	iNextSessionId++;
	
	// At least one client connected so cancel server shutdown timer.
	iServerShutdown->Cancel();
	
	return session;
	}
	
	
/**
For session access to public CCntDbManagerController methods.
*/
CCntDbManagerController& CCntServer::Controller()
	{
	return *iController;
	}


/**
If last client session is closing the server may close if no clients connect
within 5 seconds however only start server shutdown time if the server is
transient.
*/
void CCntServer::SessionClosing()
	{
	if (SessionCount() == 1 && (iServerType == ETransientServer))
		{
		iServerShutdown->Start();
		}

	// Freshen cntmodel.ini file if it was requested
	TRAP_IGNORE( iController->IniFileManager().SaveIniFileSettingsIfRequestedL());
	}


/**
Returns the number of connected sessions to the server.
*/
TInt CCntServer::SessionCount()
	{
	iSessionIter.SetToFirst();
	TInt count=0;
	while(iSessionIter++)
		{
		count++;
		}
	return(count);
	}

/**
Returns lookup table.
*/
RHashMap<TInt,MsgHandlerFptr>& CCntServer::MsgLut()
	{
		return iMsgLut;
	}

/**
Creates entries in message handler lookup tables.
*/
void CCntServer::CreateMessageMapL()
	{
	// CCntItemMsgHandler member function pointers
	iMsgLut.InsertL(ECntItemCreate, static_cast<MsgHandlerFptr>(&CCntItemMsgHandler::ItemCreateL));
	iMsgLut.InsertL(ECntItemUpdate, static_cast<MsgHandlerFptr>(&CCntItemMsgHandler::ItemUpdateL));
	iMsgLut.InsertL(ECntItemDelete, static_cast<MsgHandlerFptr>(&CCntItemMsgHandler::ItemDeleteL));
	iMsgLut.InsertL(ECntItemRead,   static_cast<MsgHandlerFptr>(&CCntItemMsgHandler::ItemReadL));
	iMsgLut.InsertL(ECntItemCommit, static_cast<MsgHandlerFptr>(&CCntItemMsgHandler::ItemCommitL));
	iMsgLut.InsertL(ECntItemOpen,   static_cast<MsgHandlerFptr>(&CCntItemMsgHandler::ItemOpenL));
	iMsgLut.InsertL(ECntItemClose,  static_cast<MsgHandlerFptr>(&CCntItemMsgHandler::ItemCloseL));
	
	iMsgLut.InsertL(ECntConnectionId,      static_cast<MsgHandlerFptr>(&CCntItemMsgHandler::ConnectionId));
	iMsgLut.InsertL(ECntGetCurrentItem,    static_cast<MsgHandlerFptr>(&CCntItemMsgHandler::GetCurrentItemL));
	iMsgLut.InsertL(ECntRemoveCurrentItem, static_cast<MsgHandlerFptr>(&CCntItemMsgHandler::RemoveCurrentItemL));
	iMsgLut.InsertL(ECntSetCurrentItem,    static_cast<MsgHandlerFptr>(&CCntItemMsgHandler::SetCurrentItemL));
	
	iMsgLut.InsertL(ECntGetCurrentDb,                        static_cast<MsgHandlerFptr>(&CCntItemMsgHandler::GetCurrentDb));
	iMsgLut.InsertL(ECntSetCurrentDb,                        static_cast<MsgHandlerFptr>(&CCntItemMsgHandler::SetCurrentDbL));
	iMsgLut.InsertL(ECntGetSpeedDialContactIdAndPhoneNumber, static_cast<MsgHandlerFptr>(&CCntItemMsgHandler::GetSpeedDialContactIdAndPhoneNumberL));
	iMsgLut.InsertL(ECntSetSpeedDialIdForPosition,           static_cast<MsgHandlerFptr>(&CCntItemMsgHandler::SetSpeedDialIdForPositionL));
	iMsgLut.InsertL(ECntSetOwnCard,                          static_cast<MsgHandlerFptr>(&CCntItemMsgHandler::SetOwnCardL));
	iMsgLut.InsertL(ECntGetOwnCard,                          static_cast<MsgHandlerFptr>(&CCntItemMsgHandler::GetOwnCardL));
	iMsgLut.InsertL(ECntGetCollection,                       static_cast<MsgHandlerFptr>(&CCntItemMsgHandler::GetCollectionL));
	
	iMsgLut.InsertL(ECntSetSortPrefs,         static_cast<MsgHandlerFptr>(&CCntItemMsgHandler::SetSortPrefsL));
	iMsgLut.InsertL(ECntGetSortPrefs,         static_cast<MsgHandlerFptr>(&CCntItemMsgHandler::GetSortPrefsL));
	iMsgLut.InsertL(ECntSetDbViewContactType, static_cast<MsgHandlerFptr>(&CCntItemMsgHandler::SetDbViewContactTypeL));
	iMsgLut.InsertL(ECntGetDbViewContactType, static_cast<MsgHandlerFptr>(&CCntItemMsgHandler::GetDbViewContactTypeL));
	iMsgLut.InsertL(ECntDbContactCount,       static_cast<MsgHandlerFptr>(&CCntItemMsgHandler::DbContactCountL));
	iMsgLut.InsertL(ECntFindAsync,            static_cast<MsgHandlerFptr>(&CCntItemMsgHandler::FindAsyncL));
	iMsgLut.InsertL(ECntFindAsyncInit,        static_cast<MsgHandlerFptr>(&CCntItemMsgHandler::FindAsyncInitL));
	iMsgLut.InsertL(ECntFindAsyncTextDefInit, static_cast<MsgHandlerFptr>(&CCntItemMsgHandler::FindAsyncTextDefInitL));
	iMsgLut.InsertL(ECntFind,                 static_cast<MsgHandlerFptr>(&CCntItemMsgHandler::FindL));
	iMsgLut.InsertL(ECntFilterDatabase,       static_cast<MsgHandlerFptr>(&CCntItemMsgHandler::FilterDatabaseL));
	iMsgLut.InsertL(ECntSetAsyncActivity,     static_cast<MsgHandlerFptr>(&CCntItemMsgHandler::SetAsyncActivityL));
	iMsgLut.InsertL(ECntResourceCount,        static_cast<MsgHandlerFptr>(&CCntItemMsgHandler::ResourceCount));
	iMsgLut.InsertL(ECntSetHeapFailure,       static_cast<MsgHandlerFptr>(&CCntItemMsgHandler::SetHeapFailure));
	iMsgLut.InsertL(ECntSeekContactInCollection, static_cast<MsgHandlerFptr>(&CCntItemMsgHandler::SeekContactL));	

	// CCntEventMsgHandler member function pointers
	iMsgLut.InsertL(ECntRequestEvent,       static_cast<MsgHandlerFptr>(&CCntEventMsgHandler::RequestEvent));
	iMsgLut.InsertL(ECntCancelEventRequest, static_cast<MsgHandlerFptr>(&CCntEventMsgHandler::CancelEventRequest));
	
	// CCntTransactionMsgHandler member function pointers
	iMsgLut.InsertL(EBeginDbTransaction,    static_cast<MsgHandlerFptr>(&CCntTransactionMsgHandler::BeginDbTransactionL));
	iMsgLut.InsertL(EEndDbTransaction,      static_cast<MsgHandlerFptr>(&CCntTransactionMsgHandler::EndDbTransactionL));
	iMsgLut.InsertL(ERollbackDbTransaction, static_cast<MsgHandlerFptr>(&CCntTransactionMsgHandler::RollbackDbTransactionL));
	
	// CCntFileManagerMsgHandler member function pointers
	iMsgLut.InsertL(ECntOpenDataBase,            static_cast<MsgHandlerFptr>(&CCntFileManagerMsgHandler::OpenDataBaseL));
	iMsgLut.InsertL(ECntCreateDatabase,          static_cast<MsgHandlerFptr>(&CCntFileManagerMsgHandler::CreateDatabaseL));
	iMsgLut.InsertL(ECntReplaceDatabase,         static_cast<MsgHandlerFptr>(&CCntFileManagerMsgHandler::ReplaceDatabaseL));
	iMsgLut.InsertL(ECntCancelAsyncOpenDatabase, static_cast<MsgHandlerFptr>(&CCntFileManagerMsgHandler::CancelAsyncOpenDatabaseL));
	iMsgLut.InsertL(ECntCloseDataBase,           static_cast<MsgHandlerFptr>(&CCntFileManagerMsgHandler::CloseDataBaseL));
	iMsgLut.InsertL(ECntCloseDbTables,           static_cast<MsgHandlerFptr>(&CCntFileManagerMsgHandler::CloseDbTablesL));
	iMsgLut.InsertL(ECntReOpenDbTables,          static_cast<MsgHandlerFptr>(&CCntFileManagerMsgHandler::ReOpenDbTablesL));
	iMsgLut.InsertL(ECntDeleteDatabase,          static_cast<MsgHandlerFptr>(&CCntFileManagerMsgHandler::DeleteDatabaseL));
	
	iMsgLut.InsertL(ECntGetDefaultDatabaseName, static_cast<MsgHandlerFptr>(&CCntFileManagerMsgHandler::GetDefaultDatabaseNameL));
	iMsgLut.InsertL(ECntDatabaseDrive,          static_cast<MsgHandlerFptr>(&CCntFileManagerMsgHandler::DatabaseDrive));
	iMsgLut.InsertL(ECntSetDatabaseDrive,       static_cast<MsgHandlerFptr>(&CCntFileManagerMsgHandler::SetDatabaseDriveL));
	iMsgLut.InsertL(ECntDatabaseExists,         static_cast<MsgHandlerFptr>(&CCntFileManagerMsgHandler::DatabaseExistsL));
	iMsgLut.InsertL(ECntListDatabases,          static_cast<MsgHandlerFptr>(&CCntFileManagerMsgHandler::ListDatabasesL));
	iMsgLut.InsertL(ECntGetDatabaseReady,       static_cast<MsgHandlerFptr>(&CCntFileManagerMsgHandler::GetDatabaseReadyL));
	iMsgLut.InsertL(ECntFetchTemplateIds,       static_cast<MsgHandlerFptr>(&CCntFileManagerMsgHandler::FetchTemplateIdsL));
	iMsgLut.InsertL(ECntFetchGroupIdLists,      static_cast<MsgHandlerFptr>(&CCntFileManagerMsgHandler::FetchGroupIdListsL));
	iMsgLut.InsertL(ECntSearchResultList,  static_cast<MsgHandlerFptr>(&CCntFileManagerMsgHandler::FetchSearchResultsL));
	
	iMsgLut.InsertL(ECntPredictiveSearchList,  static_cast<MsgHandlerFptr>(&CCntFileManagerMsgHandler::FetchPredictiveSearchResultsL));
	iMsgLut.InsertL(ECntFilesSize,          static_cast<MsgHandlerFptr>(&CCntFileManagerMsgHandler::FilesSizeL));
	
	iMsgLut.InsertL(ECntGetDefinitionsForExistingView, static_cast<MsgHandlerFptr>(&CCntFileManagerMsgHandler::GetDefinitionsForExistingViewL));
	
	// CCntPropertiesMsgHandler member function pointers
	iMsgLut.InsertL(ECntChangeViewDef,     static_cast<MsgHandlerFptr>(&CCntPropertiesMsgHandler::ChangeViewDefL));
	iMsgLut.InsertL(ECntMachineID,         static_cast<MsgHandlerFptr>(&CCntPropertiesMsgHandler::MachineIdL));
	iMsgLut.InsertL(ECntFileUniqueId,      static_cast<MsgHandlerFptr>(&CCntPropertiesMsgHandler::FileUniqueIdL));
	iMsgLut.InsertL(ECntOverrideMachineID, static_cast<MsgHandlerFptr>(&CCntPropertiesMsgHandler::OverrideMachineIdL));
	iMsgLut.InsertL(ECntReCreateTemplate,  static_cast<MsgHandlerFptr>(&CCntPropertiesMsgHandler::ReCreateTemplateL));
	iMsgLut.InsertL(ECntGetPrefTemplateId, static_cast<MsgHandlerFptr>(&CCntPropertiesMsgHandler::GetPrefTemplateIdL));
	iMsgLut.InsertL(ECntSetPrefTemplateId, static_cast<MsgHandlerFptr>(&CCntPropertiesMsgHandler::SetPrefTemplateIdL));
	iMsgLut.InsertL(ECntOpsTimeOut,        static_cast<MsgHandlerFptr>(&CCntPropertiesMsgHandler::OpsTimeOut));
	iMsgLut.InsertL(ECntICCTemplateId,     static_cast<MsgHandlerFptr>(&CCntPropertiesMsgHandler::ICCTemplateIdL));
	iMsgLut.InsertL(ECntPhonebookGroupId,  static_cast<MsgHandlerFptr>(&CCntPropertiesMsgHandler::PhonebookGroupIdL));

	// CCntViewMsgHandler member function pointers
	iMsgLut.InsertL(ECntItemAtL,              static_cast<MsgHandlerFptr>(&CCntViewMsgHandler::ItemAtL));
	iMsgLut.InsertL(ECntOpenViewSession,      static_cast<MsgHandlerFptr>(&CCntViewMsgHandler::OpenViewSessionL));
	iMsgLut.InsertL(ECntCloseViewSession,     static_cast<MsgHandlerFptr>(&CCntViewMsgHandler::CloseViewSessionL));
	iMsgLut.InsertL(ECntViewChangeSortOrderL, static_cast<MsgHandlerFptr>(&CCntViewMsgHandler::ChangeSortOrderL));
	iMsgLut.InsertL(ECntViewBeginIterate,     static_cast<MsgHandlerFptr>(&CCntViewMsgHandler::BeginIterateL));
	iMsgLut.InsertL(ECntViewEndIterate,       static_cast<MsgHandlerFptr>(&CCntViewMsgHandler::EndIterateL));
	iMsgLut.InsertL(ECntViewNextItemL,     	  static_cast<MsgHandlerFptr>(&CCntViewMsgHandler::NextItemL));	
	iMsgLut.InsertL(ECntTextField,            static_cast<MsgHandlerFptr>(&CCntViewMsgHandler::TextFieldL));
	iMsgLut.InsertL(ECntReadContactTextDef,   static_cast<MsgHandlerFptr>(&CCntViewMsgHandler::ReadContactTextDefL));
	iMsgLut.InsertL(ECntCreateView,           static_cast<MsgHandlerFptr>(&CCntViewMsgHandler::CreateViewL));
	iMsgLut.InsertL(ECntCreateNamedView,      static_cast<MsgHandlerFptr>(&CCntViewMsgHandler::CreateNamedViewL));
	iMsgLut.InsertL(ECntCloseView,            static_cast<MsgHandlerFptr>(&CCntViewMsgHandler::CloseViewL));
	iMsgLut.InsertL(ECntMatchesHintField,     static_cast<MsgHandlerFptr>(&CCntViewMsgHandler::MatchesHintFieldL));
	}
