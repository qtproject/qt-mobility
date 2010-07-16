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


#include "t_serveractiveobject.h"

CServerActiveObject::CServerActiveObject(const CPackagerTestSession& aSession) 
:CActive(EPriorityStandard), iAOStep(0), iSessionHandle(aSession)
/** Constructor. 

@param aSession A pointer to the server session owner. */ 
 	{
 	CActiveScheduler::Add(this);
 	}
 	
CServerActiveObject::~CServerActiveObject()
/** The destructor frees all resources owned by the active object, 
prior to its destruction. */
	{
	}

CServerActiveObject* CServerActiveObject::NewL(const CPackagerTestSession& aSession)
/** Allocates and constructs a new default CServerActiveObject object. 

@param aSession A pointer to the server session owner
@return Pointer to the new CServerActiveObject object. */
	{
	CServerActiveObject* self = CServerActiveObject::NewLC(aSession);
	CleanupStack::Pop();
	return self;
	}	


CServerActiveObject* CServerActiveObject::NewLC(const CPackagerTestSession& aSession)
/** Allocates and constructs a new default CServerActiveObject object. 

@param aSession A pointer to the server session owner
@return Pointer to the new CServerActiveObject object. */
	{
	CServerActiveObject* self = new(ELeave) CServerActiveObject(aSession);
	CleanupStack::PushL(self);
	
	return self;
	}
	
void CServerActiveObject::StartActiveObject()
/** Sets this active object to active. Does a request complete
so that RunL gets called right away. */	
	{
	TRequestStatus *pS = &iStatus;
	User::RequestComplete(pS, KErrNone);
	SetActive();
	CActiveScheduler::Start();	
	}		
	
void CServerActiveObject::DoCancel()
/** Pure virtual method in parent CActive class. */
	{
	}	
	
void CServerActiveObject::RunL()
/** Active Object's RunL implementation. */
	{
	CActiveScheduler::Stop();	
	iSessionHandle.NextStepL();
	}
//
