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


/**
 @file
 @internalComponent
 @released
*/

#include "ccntmsghandler.h"
#include "ccntmsghandlerfptr.h"
#include "ccntpropertiesmsghandler.h"

#include "ccntserver.h"
#include "ccntipccodes.h"
#include "ccntrequest.h"
#include "ccntdbmanager.h"
#include "ccntstatemachine.h"
#include "ccntpackager.h"

const TInt KCntPropertiesIpcCodes[] =
	{
	ECntChangeViewDef,
	ECntMachineID,
	ECntFileUniqueId,
	ECntOverrideMachineID,
	ECntReCreateTemplate,
	ECntGetPrefTemplateId,
	ECntSetPrefTemplateId,
	ECntOpsTimeOut,
	ECntICCTemplateId,
	ECntPhonebookGroupId
	};
	
CCntPropertiesMsgHandler* CCntPropertiesMsgHandler::NewLC(CCntSession& aSession)
	{
	CCntPropertiesMsgHandler* CntPropertiesMsgHandler = new (ELeave) CCntPropertiesMsgHandler(aSession);
	CleanupStack::PushL(CntPropertiesMsgHandler);
	return CntPropertiesMsgHandler;
	}
	

CCntPropertiesMsgHandler::CCntPropertiesMsgHandler(CCntSession& aSession)
:CCntMsgHandler(aSession)
	{	
	}
	
CCntPropertiesMsgHandler::~CCntPropertiesMsgHandler()
	{
	}
	
/**
Delegates the incoming op code to a message handling method.

First checks if this class services the op code, it then uses the lookup table and finds 
function pointer(to message handling method) mapped to the incoming message function (op code)
and finally delegates the message to handling method.

It returns KErrNotFound if op code not handled.
*/
TInt CCntPropertiesMsgHandler::HandleMessageL(const RMessage2& aMessage)
	{
	MsgHandlerFptr func_ptr = LookupHandlerMethodL(aMessage.Function(), KCntPropertiesIpcCodes, sizeof(KCntPropertiesIpcCodes)/sizeof(TInt));
	
	if(func_ptr)
		{
		PropertiesMsgHandlerFptr mem_func_ptr = static_cast<PropertiesMsgHandlerFptr>(func_ptr);
		(this->*mem_func_ptr)(aMessage);
		return (KErrNone);
		}
	
	return (KErrNotFound);
	}
	
/** 
Sets the default view definition.  The default view definition is used in calls
to ReadContactL(), ReadContactLC() and ReadContactAndAgentL() when no view
definition is explicitly specified.

@param aMessage.Ptr0() View definition (from client).
*/
void CCntPropertiesMsgHandler::ChangeCntViewDefL(const RMessage2& aMessage)
	{
	// Destroy any existing view definition.
	delete iView;
	iView = NULL;
	
	iPackager.SetBufferFromMessageL(aMessage);
	iView = iPackager.UnpackCntViewDefLC();
	CleanupStack::Pop(iView);

	aMessage.Complete(KErrNone);
	}

/**
Message handling methods.

Properties contains all logic to handle client-side get/set operation on
server-side objects and variables.  These objects and variables are used
throughout the server and Persistence Layer.
*/
void CCntPropertiesMsgHandler::ChangeViewDefL(const RMessage2& aMessage)
	{
	ChangeCntViewDefL(aMessage);
	}

void CCntPropertiesMsgHandler::MachineIdL(const RMessage2& aMessage)
	{
	// Maps to RCntModel::MachineId().
	
	CheckForManagerL();
	TPckg<TInt64> pckg(iManager->GetPersistenceLayer().ContactProperties().MachineIdL());
	aMessage.WriteL(0,pckg);
	aMessage.Complete(KErrNone);
	}
	
void CCntPropertiesMsgHandler::FileUniqueIdL(const RMessage2& aMessage)
	{
	// Maps to RCntModel::FileUidL().
	
	CheckForManagerL();
	aMessage.WriteL(0, iManager->GetPersistenceLayer().ContactProperties().UniqueIdL());
	aMessage.Complete(KErrNone);
	}
	
void CCntPropertiesMsgHandler::OverrideMachineIdL(const RMessage2& aMessage)
	{
	// Maps to RCntModel::OverrideMachineUniqueId().
	
	CheckForManagerL();
	#if defined(_DEBUG)
	TInt64 machineId;
	TPckg<TInt64> package(machineId);
	aMessage.ReadL(0, package);
	iManager->GetPersistenceLayer().ContactProperties().SetMachineIdL(machineId);
	#endif			
	aMessage.Complete(KErrNone);
	}
	
void CCntPropertiesMsgHandler::ReCreateTemplateL(const RMessage2& aMessage)
	{
	// Maps to RCntModel::ReCreateTemplate().
	CheckForManagerL();
	iManager->RecreateSystemTemplateL();
	aMessage.Complete(KErrNone);
	}
	
void CCntPropertiesMsgHandler::GetPrefTemplateIdL(const RMessage2& aMessage)
	{
	CheckForManagerL();
	aMessage.Complete(iManager->GetPersistenceLayer().ContactProperties().CardTemplatePrefIdL());
	}
	
void CCntPropertiesMsgHandler::SetPrefTemplateIdL(const RMessage2& aMessage)
	{
	CheckForManagerL();
	TContactItemId newCardTemplatePrefId = aMessage.Int0();
	iManager->SetCardTemplatePrefIdL(newCardTemplatePrefId, iSessionId);
	aMessage.Complete(KErrNone);
	}
	
void CCntPropertiesMsgHandler::OpsTimeOut(const RMessage2& aMessage)
	{
	iTimeOut = aMessage.Int0();
	aMessage.Complete(KErrNone);
	}
	
void CCntPropertiesMsgHandler::ICCTemplateIdL(const RMessage2& aMessage)
	{
	CheckForManagerL();
	const TUid phonebookUid(TUid::Uid(aMessage.Int0()));
	aMessage.Complete(iManager->GetPersistenceLayer().ContactProperties().ICCTemplateIdL(phonebookUid, iSessionId));
	}
	
void CCntPropertiesMsgHandler::PhonebookGroupIdL(const RMessage2& aMessage)
	{
	CheckForManagerL();
	aMessage.Complete(iManager->GetPersistenceLayer().ContactProperties().PhonebookGroupIdL(iSessionId));
	}
	
