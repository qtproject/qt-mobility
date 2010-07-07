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




#ifndef CACTIVELOOP_H
#define CACTIVELOOP_H

#include <e32base.h>

class MActiveLoopCallBack 
	{
public:
	virtual TBool DoStepL() = 0;
	virtual void  DoError(TInt aError) = 0;
	};


/**
CActiveLoop class should be used in the server for all long-running operations.
Since the server is single threaded any long running operation will effectively
block the server for all other clients. In order to avoid it, the long running
operations should be done with incremental steps. Each step should be called
from the active object and controll should be passed to the active scheduler
afterwards. 
CActiveLoop is based on CTimer in order to give the Server active object
priority on the loop operations. It is particularly necessary for Cancel of
asynchronous operation use case, where ServiceL must be called between the
steps of an incremental operation.
*/
class CActiveLoop : public CTimer
	{
public:
	void 	Register(MActiveLoopCallBack & aCallBack, TBool aUseTimer = EFalse);
	void 	RunL();
	static 	CActiveLoop* NewL();
	void 	DoCancel();
		   ~CActiveLoop();
	TInt 	RunError(TInt aError);	  
	
private:
	CActiveLoop();
	
private:	
	MActiveLoopCallBack* iCallBack;
	}; 


#endif
