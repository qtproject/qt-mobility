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
*
*/


#ifndef __TPERFCRUDOPERATIONS__
#define __TPERFCRUDOPERATIONS__

#include <e32std.h>
#include <e32test.h>
#include <cntdb.h>
#include <s32strm.h>


class CCrudOperations : public CBase
	{
public:
	~CCrudOperations();
	static CCrudOperations* NewLC(RTest& aTest);
	void DoTestsL(TInt aNumMsToGet20ItemsFromView);
		
private:
	CCrudOperations();
	void ConstructL(RTest& aTest);
	void MatchContactsByPhoneNumL();
	void MatchContactsByEmailAndSipL();
	void DoCrudTestsL();

private:

	RTest* iTest; //not owned
	CContactDatabase* iDb;
	TInt iNumMsToGet20ItemsFromView;
	};


#endif //__TPERFCRUDOPERATIONS__
