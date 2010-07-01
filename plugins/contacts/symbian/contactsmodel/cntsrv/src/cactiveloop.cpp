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


#include "cactiveloop.h"

void CActiveLoop::Register(MActiveLoopCallBack & aCallBack, TBool aUseTimer)
	{
	iCallBack = &aCallBack;
	if (aUseTimer)
		{ //Timer required for Cancel Asynchronous operation use case.
		After(0);
		}
	else
		{
		CActive::SetActive();
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status, KErrNone);
		}
	}

	
	
void CActiveLoop::RunL()
	{
	if (iCallBack->DoStepL())
		{	
  		CActive::SetActive();
  		TRequestStatus* status = &iStatus;
  		User::RequestComplete(status, KErrNone);
		}
	}
	
CActiveLoop* CActiveLoop::NewL()
	{
	CActiveLoop* active = new (ELeave) CActiveLoop();
	CleanupStack::PushL(active);
	active->ConstructL();
	CleanupStack::Pop(active);
	return active;
	}
	
void CActiveLoop::DoCancel() 
	{
	}	

CActiveLoop::CActiveLoop() :CTimer(EPriorityStandard)
	{
	CActiveScheduler::Add(this);
	}

	
CActiveLoop::~CActiveLoop() 
	{
	Cancel();
	}
	
TInt CActiveLoop::RunError(TInt aError)
	{
	iCallBack->DoError(aError);
	return KErrNone;
	}
