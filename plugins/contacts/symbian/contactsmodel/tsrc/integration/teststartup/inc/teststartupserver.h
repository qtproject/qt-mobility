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




#ifndef __TESTSTARTUPSERVER_H__
#define __TESTSTARTUPSERVER_H__

#include <testexecutestepbase.h>
#include <e32base.h>
#include <cntviewbase.h>
#include <cntviewstore.h>
#include <centralrepository.h>
#include <testexecuteserverbase.h>

#include <cntdb.h>
#include <cntitem.h>
#include <cntfield.h>
#include <cntfldst.h>

const TInt KMaxTagLen = 255;

//Test Step Names used in script files
_LIT(KTestGetSetViewSettingStep, "TestGetSetViewSettingStep");
_LIT(KTestStartUpExeStep,        "TestStartUpExeStep");
_LIT(KTestTermStartUpExeStep,    "TestTermStart");
_LIT(KTestContactsDbStep,        "TestCntsDbStep");

//Default View settings and API actions
_LIT(KApiActionTag,              "ApiAction");
_LIT(KApiGet,                    "GetDefaultViewSettings");
_LIT(KApiSet,                    "SetNewDefaultViewSettings");
_LIT(KApiAppend,                 "AppendNewViewSetting");
_LIT(KApiDelete,                 "DeleteViewSettings");
_LIT(KApiOOMGet,                 "OutOfMemoryTestsForGetAPI");
_LIT(KApiOOMSet,                 "OutOfMemoryTestsForSetAPI");
_LIT(KNoOfViewsTag,              "NumberofViews");
_LIT(KViewNameTag,               "ViewName");
_LIT(KViewPreferanceTag,         "ViewPreferance");
_LIT(KViewTypeTag,               "ViewType");
_LIT(KFirstSortOrderTag,         "FirstViewSortOrder");
_LIT(KSecondSortOrderTag,        "SecondViewSortOrder");
_LIT(KThirdSortOrderTag,         "ThirdViewSortOrder");
_LIT(KPluginNameTag,             "SortPluginName");

//Contacts server startup and process control
_LIT(KStartupActionTag,          "StartupAction");
_LIT(KLaunch,                    "LaunchProcess");
_LIT(KTerminate,                 "TerminateProcess");
_LIT(KCheck,                     "CheckProcess");
_LIT(KStartupnameTag,            "StartupExeName");
_LIT(KExitTypeTag,               "StartUpExitType");
_LIT(KExitReasonTag,             "StartUpExitReason");
_LIT(KModeTag,                   "Mode");

//Contacts database actions
_LIT(KCntDbActionTag,            "ContactsDbAction");
_LIT(KCreateDb,                  "CreateDefCntsDb");
_LIT(KOpenDb,                    "OpenDefCntsDb");
_LIT(KReplaceDb,                 "ReplaceDefCntsDb");
_LIT(KAddItemsDb,                "AddItemstoCntsDb");
_LIT(KDeleteDb,                  "DeleteDefCntsDb");
_LIT(KCheckCntViewDefs,          "CheckExistingViewDefsInCntsServer");
_LIT(KNoOfCntsTag,               "NumberOfContacts");

class CTestStartUpServer : public CTestServer
	{
public:
	static CTestStartUpServer* NewL();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
    inline RFs& Fs();
     
    TInt LaunchCntsStartUpExe(TPtrC aStartUpExeName, TPtrC aServerMode);
    void TermCntsStartUpExe();
    TBool CheckCntsStartUpExe(TExitType aExitTypeFromIni, TInt aExitReasonFromIni);

private:
	RFs iFs;
	RProcess iStartUpProcess;
	};
	

inline RFs& CTestStartUpServer::Fs()
    {
    return iFs;
    }

#endif
