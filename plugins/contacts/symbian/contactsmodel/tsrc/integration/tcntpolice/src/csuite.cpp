/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* for (WINS && !EKA2) versions will be xxxServer.Dll and require a thread to be started
* in the process of the client. The client initialises the server by calling the
* one and only ordinal.
*
*/


#include <e32base.h>
#include <e32cmn.h> 

#if (!defined TS_TE_Cntsrv_API_Policing_SERVER_H_)
	#include "csuite.h"
#endif


#include "ecntcloseview1_cstep.h"
#include "ecntsendpluginuidtoserver1_cstep.h"
#include "cntcntctmatchingcriteriaexternalizedsize1_cstep.h"
#include "ecntviewsortorderexternalizedsize1_cstep.h"
#include "ecntviewat1_cstep.h"
#include "ecntviewcount1_cstep.h"
#include "ecntviewfind1_cstep.h"
#include "ecntallfieldslength1_cstep.h"
#include "ecntgetviewsortorder1_cstep.h"
#include "ecntrequestviewevent1_cstep.h"
#include "ecntcancelrequestviewevent1_cstep.h"
#include "ecntgetincludedtypes1_cstep.h"
#include "ecntgetcontactids1_cstep.h"
#include "ecntgetsortpluginuidfromserver1_cstep.h"
#include "ecntgetcurrentitem1_cstep.h"
#include "ecntsetheapfailure1_cstep.h"
#include "ecntresourcecount1_cstep.h"
#include "ecntdbcontactcount1_cstep.h"
#include "ecntsetdbviewcontacttype1_cstep.h"
#include "ecntviewcontactatlength1_cstep.h"
#include "ecntrequestevent1_cstep.h"
#include "ecntcanceleventrequest1_cstep.h"
#include "ecntconnectionid1_cstep.h"
#include "ecntmachineid1_cstep.h"
#include "ecntoverridemachineid1_cstep.h"
#include "ecntgetpreftemplateid1_cstep.h"
#include "ecnticctemplateid1_cstep.h"
#include "ecntphonebookgroupid1_cstep.h"
#include "ecntclosedatabase1_cstep.h"
#include "ecntclosedbtables1_cstep.h"
#include "ecntcancelasyncopendatabase1_cstep.h"
#include "ecntfileuniqueid1_cstep.h"
#include "ecntopstimeout1_cstep.h"
#include "ecntfilessize1_cstep.h"
#include "ecntwastedspaceinbytes1_cstep.h"
#include "ecntmatcheshintfield1_cstep.h"
#include "ecntgetdefaultdatabasename1_cstep.h"
#include "ecntgetcurrentdb1_cstep.h"
#include "ecntdatabasedrive1_cstep.h"
#include "ecntdatabaseexists1_cstep.h"
#include "ecntcompressrequired1_cstep.h"
#include "ecntisdamaged1_cstep.h"
#include "ecntsetsortprefs1_cstep.h"
#include "ecntgetowncard1_cstep.h"
#include "ecntfetchtemplateids1_cstep.h"
#include "ecntfetchgroupidlists1_cstep.h"
#include "ecntseekcontact1_cstep.h"
#include "ecntseekidentcontact1_cstep.h"
#include "ecntchangeviewdef1_cstep.h"
#include "ecntgetsortprefs1_cstep.h"
#include "ecntitemclose1_cstep.h"
#include "ecntgetdefinitionsforexistingview1_cstep.h"
#include "ecntopendatabase1_cstep.h"
#include "ecntreopendbtables1_cstep.h"
#include "ecntcreateview1_cstep.h"
#include "ecntcreatenamedview1_cstep.h"
#include "ecntviewcontactat1_cstep.h"
#include "ecntallfieldstext1_cstep.h"
#include "ecntgetcontactsmatchingfilter1_cstep.h"
#include "ecntgetcontactmatchingcriteria1_cstep.h"
#include "ecntgetspeeddialcontactidandphonenumber1_cstep.h"
#include "ecntitemread1_cstep.h"
#include "ecntreadcontacttextdef1_cstep.h"
#include "ecntfilterdatabase1_cstep.h"
#include "ecntfind1_cstep.h"
#include "ecntfindasyncinit1_cstep.h"
#include "ecntfindasynctextdefinit1_cstep.h"
#include "ecntfindasync1_cstep.h"
#include "ecntitematl1_cstep.h"
#include "ecntgetcollection1_cstep.h"
#include "ecnttextfield1_cstep.h"
#include "ecntlistdatabases1_cstep.h"
#include "ecntcreatedatabase1_cstep.h"
#include "ecntsetpreftemplateid1_cstep.h"
#include "ecntsetowncard1_cstep.h"
#include "ecntrecreatetemplate1_cstep.h"
#include "ecntsetcurrentitem1_cstep.h"
#include "ecntremovecurrentitem1_cstep.h"
#include "ecntsetcurrentdb1_cstep.h"
#include "ecntsetspeeddialidforposition1_cstep.h"
#include "ecntchangeviewsortorder1_cstep.h"
#include "ecntrecoverdb1_cstep.h"
#include "ecntrecoverdbstep1_cstep.h"
#include "ecntcancelrecoverstep1_cstep.h"
#include "ecntitemopen1_cstep.h"
#include "ecntsetdatabasedrive1_cstep.h"
#include "ecntreplacedatabase1_cstep.h"
#include "ecntdeletedatabase1_cstep.h"
#include "ecntitemupdate1_cstep.h"
#include "ecntitemcommit1_cstep.h"
#include "ecntitemdelete1_cstep.h"
#include "ecntcompress1_cstep.h"
#include "ecntcompressstep1_cstep.h"
#include "ecntcancelcompressstep1_cstep.h"
#include "ecntdamagedb1_cstep.h"
#include "ecntitemcreate1_cstep.h"
#include "ebegindbtransaction1_cstep.h"
#include "eenddbtransaction1_cstep.h"
#include "erollbackdbtransaction1_cstep.h"
#include "elocksrvnotsupported1_cstep.h"

// __EDIT_ME__ - Substitute the name of your test server 
_LIT(KServerName,"Cap_TE_Cntsrv_API_Policing_sc");
// __EDIT_ME__ - Use your own server class name

CTestTE_Cntsrv_API_PolicingServer* CTestTE_Cntsrv_API_PolicingServer::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
// __EDIT_ME__ - Use your own server class name
	CTestTE_Cntsrv_API_PolicingServer * server = new (ELeave) CTestTE_Cntsrv_API_PolicingServer();
	CleanupStack::PushL(server);
	// CServer base class call
	server->StartL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}

CTestStep* CTestTE_Cntsrv_API_PolicingServer::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	 CTestStep* testStep = NULL;
	 TBool testStepFound(EFalse);
	// add test steps
		if (aStepName == _L("CECntCloseView1Step"))
			{
			testStep =  new CECntCloseView1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntSendPluginUidToServer1Step"))
			{
			testStep =  new CECntSendPluginUidToServer1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CCntCntctMatchingCriteriaExternalizedSize1Step"))
			{
			testStep =  new CCntCntctMatchingCriteriaExternalizedSize1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntViewSortOrderExternalizedSize1Step"))
			{
			testStep =  new CECntViewSortOrderExternalizedSize1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntViewAt1Step"))
			{
			testStep =  new CECntViewAt1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntViewCount1Step"))
			{
			testStep =  new CECntViewCount1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntViewFind1Step"))
			{
			testStep =  new CECntViewFind1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntAllFieldsLength1Step"))
			{
			testStep =  new CECntAllFieldsLength1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntGetViewSortOrder1Step"))
			{
			testStep =  new CECntGetViewSortOrder1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntRequestViewEvent1Step"))
			{
			testStep =  new CECntRequestViewEvent1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntCancelRequestViewEvent1Step"))
			{
			testStep =  new CECntCancelRequestViewEvent1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntGetIncludedTypes1Step"))
			{
			testStep =  new CECntGetIncludedTypes1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntGetContactIds1Step"))
			{
			testStep =  new CECntGetContactIds1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntGetSortPluginUidFromServer1Step"))
			{
			testStep =  new CECntGetSortPluginUidFromServer1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntGetCurrentItem1Step"))
			{
			testStep =  new CECntGetCurrentItem1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntSetHeapFailure1Step"))
			{
			testStep =  new CECntSetHeapFailure1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntResourceCount1Step"))
			{
			testStep =  new CECntResourceCount1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntDbContactCount1Step"))
			{
			testStep =  new CECntDbContactCount1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntSetDbViewContactType1Step"))
			{
			testStep =  new CECntSetDbViewContactType1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntViewContactAtLength1Step"))
			{
			testStep =  new CECntViewContactAtLength1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntRequestEvent1Step"))
			{
			testStep =  new CECntRequestEvent1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntCancelEventRequest1Step"))
			{
			testStep =  new CECntCancelEventRequest1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntConnectionId1Step"))
			{
			testStep =  new CECntConnectionId1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntMachineID1Step"))
			{
			testStep =  new CECntMachineID1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntOverrideMachineID1Step"))
			{
			testStep =  new CECntOverrideMachineID1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntGetPrefTemplateId1Step"))
			{
			testStep =  new CECntGetPrefTemplateId1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntICCTemplateId1Step"))
			{
			testStep =  new CECntICCTemplateId1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntPhonebookGroupId1Step"))
			{
			testStep =  new CECntPhonebookGroupId1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntCloseDataBase1Step"))
			{
			testStep =  new CECntCloseDataBase1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntCloseDbTables1Step"))
			{
			testStep =  new CECntCloseDbTables1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntCancelAsyncOpenDatabase1Step"))
			{
			testStep =  new CECntCancelAsyncOpenDatabase1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntFileUniqueId1Step"))
			{
			testStep =  new CECntFileUniqueId1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntOpsTimeOut1Step"))
			{
			testStep =  new CECntOpsTimeOut1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntFilesSize1Step"))
			{
			testStep =  new CECntFilesSize1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntWastedSpaceInBytes1Step"))
			{
			testStep =  new CECntWastedSpaceInBytes1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntMatchesHintField1Step"))
			{
			testStep =  new CECntMatchesHintField1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntGetDefaultDatabaseName1Step"))
			{
			testStep =  new CECntGetDefaultDatabaseName1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntGetCurrentDb1Step"))
			{
			testStep =  new CECntGetCurrentDb1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntDatabaseDrive1Step"))
			{
			testStep =  new CECntDatabaseDrive1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntDatabaseExists1Step"))
			{
			testStep =  new CECntDatabaseExists1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntCompressRequired1Step"))
			{
			testStep =  new CECntCompressRequired1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntIsDamaged1Step"))
			{
			testStep =  new CECntIsDamaged1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntSetSortPrefs1Step"))
			{
			testStep =  new CECntSetSortPrefs1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntGetOwnCard1Step"))
			{
			testStep =  new CECntGetOwnCard1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntFetchTemplateIds1Step"))
			{
			testStep =  new CECntFetchTemplateIds1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntFetchGroupIdLists1Step"))
			{
			testStep =  new CECntFetchGroupIdLists1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntSeekContact1Step"))
			{
			testStep =  new CECntSeekContact1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntSeekIdentContact1Step"))
			{
			testStep =  new CECntSeekIdentContact1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntChangeViewDef1Step"))
			{
			testStep =  new CECntChangeViewDef1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntGetSortPrefs1Step"))
			{
			testStep =  new CECntGetSortPrefs1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntItemClose1Step"))
			{
			testStep =  new CECntItemClose1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntGetDefinitionsForExistingView1Step"))
			{
			testStep =  new CECntGetDefinitionsForExistingView1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntOpenDataBase1Step"))
			{
			testStep =  new CECntOpenDataBase1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntReOpenDbTables1Step"))
			{
			testStep =  new CECntReOpenDbTables1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntCreateView1Step"))
			{
			testStep =  new CECntCreateView1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntCreateNamedView1Step"))
			{
			testStep =  new CECntCreateNamedView1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntViewContactAt1Step"))
			{
			testStep =  new CECntViewContactAt1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntAllFieldsText1Step"))
			{
			testStep =  new CECntAllFieldsText1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntGetContactsMatchingFilter1Step"))
			{
			testStep =  new CECntGetContactsMatchingFilter1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntGetContactMatchingCriteria1Step"))
			{
			testStep =  new CECntGetContactMatchingCriteria1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntGetSpeedDialContactIdAndPhoneNumber1Step"))
			{
			testStep =  new CECntGetSpeedDialContactIdAndPhoneNumber1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntItemRead1Step"))
			{
			testStep =  new CECntItemRead1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntReadContactTextDef1Step"))
			{
			testStep =  new CECntReadContactTextDef1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntFilterDatabase1Step"))
			{
			testStep =  new CECntFilterDatabase1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntFind1Step"))
			{
			testStep =  new CECntFind1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntFindAsyncInit1Step"))
			{
			testStep =  new CECntFindAsyncInit1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntFindAsyncTextDefInit1Step"))
			{
			testStep =  new CECntFindAsyncTextDefInit1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntFindAsync1Step"))
			{
			testStep =  new CECntFindAsync1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntItemAtL1Step"))
			{
			testStep =  new CECntItemAtL1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntGetCollection1Step"))
			{
			testStep =  new CECntGetCollection1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntTextField1Step"))
			{
			testStep =  new CECntTextField1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntListDatabases1Step"))
			{
			testStep =  new CECntListDatabases1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntCreateDatabase1Step"))
			{
			testStep =  new CECntCreateDatabase1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntSetPrefTemplateId1Step"))
			{
			testStep =  new CECntSetPrefTemplateId1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntSetOwnCard1Step"))
			{
			testStep =  new CECntSetOwnCard1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntReCreateTemplate1Step"))
			{
			testStep =  new CECntReCreateTemplate1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntSetCurrentItem1Step"))
			{
			testStep =  new CECntSetCurrentItem1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntRemoveCurrentItem1Step"))
			{
			testStep =  new CECntRemoveCurrentItem1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntSetCurrentDb1Step"))
			{
			testStep =  new CECntSetCurrentDb1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntSetSpeedDialIdForPosition1Step"))
			{
			testStep =  new CECntSetSpeedDialIdForPosition1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntChangeViewSortOrder1Step"))
			{
			testStep =  new CECntChangeViewSortOrder1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntRecoverDb1Step"))
			{
			testStep =  new CECntRecoverDb1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntRecoverDbStep1Step"))
			{
			testStep =  new CECntRecoverDbStep1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntCancelRecoverStep1Step"))
			{
			testStep =  new CECntCancelRecoverStep1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntItemOpen1Step"))
			{
			testStep =  new CECntItemOpen1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntSetDatabaseDrive1Step"))
			{
			testStep =  new CECntSetDatabaseDrive1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntReplaceDatabase1Step"))
			{
			testStep =  new CECntReplaceDatabase1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntDeleteDatabase1Step"))
			{
			testStep =  new CECntDeleteDatabase1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntItemUpdate1Step"))
			{
			testStep =  new CECntItemUpdate1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntItemCommit1Step"))
			{
			testStep =  new CECntItemCommit1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntItemDelete1Step"))
			{
			testStep =  new CECntItemDelete1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntCompress1Step"))
			{
			testStep =  new CECntCompress1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntCompressStep1Step"))
			{
			testStep =  new CECntCompressStep1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntCancelCompressStep1Step"))
			{
			testStep =  new CECntCancelCompressStep1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntDamageDb1Step"))
			{
			testStep =  new CECntDamageDb1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CECntItemCreate1Step"))
			{
			testStep =  new CECntItemCreate1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CEBeginDbTransaction1Step"))
			{
			testStep =  new CEBeginDbTransaction1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CEEndDbTransaction1Step"))
			{
			testStep =  new CEEndDbTransaction1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CERollbackDbTransaction1Step"))
			{
			testStep =  new CERollbackDbTransaction1Step  ;
			testStepFound = ETrue;
			}
			 
		else if (aStepName == _L("CELockSrvNotSupported1Step"))
			{
			testStep =  new CELockSrvNotSupported1Step  ;
			testStepFound = ETrue;
			}
		
		if (testStepFound && !testStep)
			{
			_LIT(KAllocationFailed, 
				"Could not allocate teststep in CTestTE_Cntsrv_API_PolicingServer::CreateTestStep()\n");
			ERR_PRINTF1(KAllocationFailed);
			User::Invariant();
			}

		return testStep;
	 }

	

// Secure variants much simpler
// Just an E32Main and a MainL()
LOCAL_C void MainL()
/**
 * Secure variant
 * Much simpler, uses the new Rendezvous() call to sync with the client
 */
	{
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().DataCaging(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
// __EDIT_ME__ - Use your own server class name
	CTestTE_Cntsrv_API_PolicingServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTestTE_Cntsrv_API_PolicingServer::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
	}


GLDEF_C TInt E32Main()
/**
 * @return - Standard Epoc error code on process exit
 * Secure variant only
 * Process entry point. Called by client using RProcess API
 */
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAPD(err,MainL());
	delete cleanup;
	__UHEAP_MARKEND;
	return err;
    }


TVerdict CCapabilityTestStep::doTestStepPreambleL( void )
	{
	//If Preamble is not required just pass a success value
	TVerdict testResult = CTestStep::doTestStepPreambleL();
	
	
	return TestStepResult();
	
	}
TVerdict CCapabilityTestStep::doTestStepPostambleL( void )
	{
	//If Postamble is not required just pass a success value
	TVerdict testResult = CTestStep::doTestStepPostambleL();

	return TestStepResult();
	
	}


// Moved from CStep.cpp

enum TVerdict CCapabilityTestStep::doTestStepL()
	{
	//DEF! INFO_PRINTF2(_L("%S - Starting ..."), &iTestStepName);

	//The MainThread()creates a separate thread that executes SendReceive
	TVerdict vResult = MainThread();
	
	SetTestStepResult(vResult);
	return TestStepResult();
	}

/*
ThreadStartFn:
Called by:	The Child thread
Function:	Calls the Exec_SendReceive
*/
static TInt ThreadStartFn( TAny * ptr )
	{
	return(((CCapabilityTestStep *)ptr)->Exec_SendReceive());
	}

/*
TVerdict GetVerdict(TInt aAPIretValue)

Called by: "MainThread" for returning verdict

Parameters(TInt aRetValue) :	0 if API call gets thru without any rejection
								1 if API call is rejected for capability error	
*/
enum TVerdict CCapabilityTestStep::GetVerdict(TInt aAPIretValue)
	{
	TVerdict vVerdict[] = {EPass, EFail};

	//please leave the following if/else block as the information printed by INFO_PRINTF1 is used bu CapTestSumm
	if(iExpect_Rejection)//[Inverse Test] EPass for 1 while EFail for 0
		{
		INFO_PRINTF1(_L("Test Expected to Fail due to lack of capabilities"));
		return vVerdict[(aAPIretValue)?0:1];
		
		}
	else //[Direct Test] EPass for 0 while EFail for 1
		{
		INFO_PRINTF1(_L("Test Expected to Pass with correct capabilities"));
		return vVerdict[(aAPIretValue)?1:0];
		}
	}


/*
TVerdict MainThread()

Called by: "doTestStepL"

Purpose:	Creates the child thread(which calls the respective function with regard
			to the server and also implements the Message Call). Then this fn.waits for the
			completion of the childthread( doesnt matter how the thread did die!)

Return Value(Verdict of the TestStep):

			A.Reporting PASS/FAIL
				Direct Test:
						When a message call gets thru. Please note that in such cases
						actually the implementation of the message has started. As we
						are passing "0" Parameters, server may panic, though our botheration
						stops once the call gets thru.
						NOTE:	The style is the same when CONNECTION capabilities
								are tested, the only diff is you dont have to expect a 
								panic from server
				Inverse Test:
						The call should be either failed or panicked with
						"KErrPermissionDenied" flag. 

				General Case:
						If a thread creation failed or if the server couldnt be connected
						apart from the above two cases, then a FAIL is reported
			
			B.Reporting INCONCLUSIVE
						Any panic say from unexpected source (eg:KERN-EXEC) will be
						reported INCONCLUSIVE														
*/
TVerdict CCapabilityTestStep::MainThread()
	{

	TBuf<100>	tExitCategory;
	TInt		tExitReason = 0;		
	TBuf<100>	TestStyle;

	iExpect_Rejection?TestStyle = _L("Inverse"):TestStyle = _L("Direct");
	TCapabilitySet theCaps =  TSecurityInfo(RProcess()).iCaps ;
	const TInt KMaxTestThreadHeapSize = 0x10000;

	//Initialize return values
	iResult_SR = iResult_Server = KErrNone;	
		

	// Create a child thread, with a new heap
	TInt nRes_Thread =	tChildThread.Create(
						ChildThread_SR,
						ThreadStartFn,
						KDefaultStackSize,
						KMinHeapSize,
						KMaxTestThreadHeapSize,
						this,			
						EOwnerProcess);


	if(nRes_Thread == KErrNone)//Thread Created
		{

		//Let me know when the thread is dead
		TRequestStatus ThreadStatus;
		tChildThread.Logon(ThreadStatus);
		tChildThread.Resume(); 
		//Is the thread dead?
		User::WaitForRequest( ThreadStatus );
		
		//yes, he is dead. RIP!  Now the Killer's profile
		tExitCategory	=	tChildThread.ExitCategory();
		tExitReason		=	tChildThread.ExitReason();
		
	
		//Somebody Please say what are we testing!!		
		if(iSessionCreated && (SR_MESSAGE_ID >=0))//Flag set by Child thread when connected to Server
		{
			//DEF INFO_PRINTF5(_L("Connected to Server(%S) for %S Test [MessageID: %d,Req.Cap: 0x%x,Present.Cap: 0x%x]"),&SR_ServerName,&TestStyle,SR_MESSAGE_ID,iStepCap,TSecurityInfo(RProcess()));			
		}
		else if(SR_MESSAGE_ID < 0)
		{
			//DEF INFO_PRINTF5(_L("Testing Connection capabilities[%S Test] for Server(%S)  [Req.Cap: 0x%x,Present.Cap: 0x%x]"),&TestStyle,
			//&SR_ServerName,TSecurityInfo(RProcess()));			
		}
		else if(!iSessionCreated)// NO Connection
			{
			INFO_PRINTF4(_L("Couldnt connect to the Server(%S) ErrorCode - ServerRet: %d C32ret: %d"),&SR_ServerName,iResult_Server,iResult_C32);
			//INFO_PRINTF3(_L("Child Thread: ExitCategory : %S ExitReason : %d"),&tExitCategory,tExitReason);
  			return EFail;
 			}



		switch(tChildThread.ExitType())
			{			
			case EExitPanic:
				//1.A Panic from the connected Server 
				//2.A CServer Panic normally for capability rejection
				//3.A kernel Panic (consider yourself doomed!)
				if((tExitReason == KErrPermissionDenied) ||
					//DEF ? 	it's old version (tExitReason == CServer::EClientDoesntHaveRequiredCaps))//Rejected for Insufficient Cap.
					// is it correct ?
					(tExitReason == CServer2::EClientDoesntHaveRequiredCaps))//Rejected for Insufficient Cap.
					{
					INFO_PRINTF2(_L("Rejected for insufficient capabilities [Return Value : %d] "),tExitReason);
					return(GetVerdict(API_RetValue_PermissionDenied));
					}
				else if(tExitCategory == iServer_Panic) //Panic from Server
					{
					INFO_PRINTF2(_L("Server(%S) Panic to child thread"),&tExitCategory);	
					INFO_PRINTF3(_L("Child Thread: ExitCategory : %S ExitReason : %d"),&tExitCategory,tExitReason);			
					return(GetVerdict(API_RetValue_ServerPanic));
					}
				else//A kernel Panic possibly
					{
					INFO_PRINTF3(_L("Child Thread: Panic from unexpected source (ExitCategory: %S ExitReason : %d)!"),&tExitCategory,tExitReason);
					return EInconclusive;
					}
			case EExitKill:
				if(iResult_SR != KErrPermissionDenied)
					{
					INFO_PRINTF2(_L("A Successfull call (Return Value : %d)"),((SR_MESSAGE_ID >=0)?iResult_SR:iResult_Server));
					return(GetVerdict(API_RetValue_NoCapError));
					}
				else 
					{
					INFO_PRINTF2(_L("Rejected for insufficient capabilities [Return Value : %d] "),((SR_MESSAGE_ID >=0)?iResult_SR:iResult_Server));
					return(GetVerdict(API_RetValue_PermissionDenied));			
					}
			default:					
					break;
			}
		}			
	else //Our thread couldnt start	:o(
		{
		INFO_PRINTF2(_L("ERROR: Failed to create Child thread,  ErrorCode:(%d)"),nRes_Thread);
		return EFail;
		}

	return EInconclusive;
	}

TInt CCapabilityTestStep::StartServer()
{

	TInt err =  KErrNone ;
	
	// EKA2 is simple No path required
	TBuf<32> serverFile;
	serverFile.Copy(_L("CNTSRV"));
	_LIT(KExe,".exe");
	serverFile.Append(KExe);
	RProcess server;
	err = server.Create(serverFile,_L(""));
	
	if(err != KErrNone)
		{
		return err;
		}
	
	// Synchronise with the server
	TRequestStatus reqStatus;
	server.Rendezvous(reqStatus);
	server.Resume();
	
	// Server will call the reciprocal static synchronise call
	User::WaitForRequest(reqStatus);
	
	if(reqStatus.Int() != KErrNone)
		{
		return reqStatus.Int();
		}
	return err;
}	
	
 	TInt CCapabilityTestStep::TestDebugHeap(TInt* iDbgIPCNo)
 	{
 	
 	//TDbgFns  {MarkHeapStart, MarkHeapEnd, CheckHeap, FailNext, ResetFailNext};
 	TInt aFnToTest= iDbgIPCNo[5];
 			
 	
 	TInt iResult_SR [6] ={0};
 	TInt i = 1;
 	TInt testedFn = 0;
 
 	TInt dbgTestSequence[5][6]	=	{	{MarkHeapStart	,2,0,1,-1,-1}, 
 										{MarkHeapEnd	,2,0,1,-1,-1},
 										{CheckHeap		,3,0,2, 1,-1},
 										{FailNext		,4,0,3, 4, 1},
 										{ResetFailNext	,4,0,3, 4, 1}
 										
 									};
 
 	
 	TInt aCount = dbgTestSequence[aFnToTest][i];
 
 	while(aCount--  )
 		{
 		testedFn =  dbgTestSequence[aFnToTest][(++i)];
 		
			iResult_SR[testedFn ]= SendReceive( iDbgIPCNo[testedFn],TIpcArgs(((iDbgIPCNo[testedFn]==3 )?4:0),0,0,0));
   		
 		
 		if( ((testedFn !=aFnToTest)?iResult_SR[testedFn]:KErrNone) == KErrPermissionDenied) 			
			
			User::Panic(_L("Failed at Initialization"),iResult_SR[testedFn]);		

		}

	return iResult_SR[aFnToTest];
	}
	
 
