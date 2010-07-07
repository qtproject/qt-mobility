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



#ifndef __SERVER_ACTIVE_OBJECT_H__
#define __SERVER_ACTIVE_OBJECT_H__

#include <e32base.h>
#include "t_packagertestserver.h"

class CPackagerTestSession;
class CServerActiveObject : CActive
{
public: 	
	~CServerActiveObject();
	static CServerActiveObject* NewL(const CPackagerTestSession& aSession);
	static CServerActiveObject* NewLC(const CPackagerTestSession& aSession);

	void DoCancel();
	void RunL();
	// TInt RunError(TInt anError); //-Might not be needed
	void StartActiveObject();
	
private: 
	CServerActiveObject(const CPackagerTestSession& aSession);
	//void ConstructL();

private:
	TInt iAOStep;
	const CPackagerTestSession& iSessionHandle;
	
};

#endif //__SERVER_ACTIVE_OBJECT_H__
