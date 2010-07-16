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


#ifndef __TPERFSTARTUP__
#define __TPERFSTARTUP__

#include "../t_utils.h"
#include <e32std.h>
#include <e32test.h>
#include <cntview.h>

const TInt KNumTimings = 4; // number enums in TTimings and timing slots required in iTimes

class CStartUp : public CActive, public MContactViewObserver
	{
public:
	enum TViewType
		{
		ERemoteContactsView,
		ERemoteGroupsView
		};
private:
	enum TTimings
		{
		EOpenDatabase,
		ECreateView,
		EGetFirstTwenty,
		EGetTwentyFirst
		};

public:
	static CStartUp* NewLC(RTest& aTest);
	
	void RunL();
	void DoCancel();
	
	void HandleContactViewEvent(const CContactViewBase& aView, const TContactViewEvent& aEvent);
	
	void DoTestL(const TDesC& aDbName, TViewType aViewType, TInt& aNumMsToGet20ItemsFromView);
	
private:
	~CStartUp();
	CStartUp(RTest& aTest);
	void ConstructL();
	void CompleteSelf();

	void OpenDatabaseL(const TDesC& aDBName);
	void CreateRemoteViewL();
	void Cleanup();
	void RetrieveContactsL();
			
private:

	RTest& iTest;  //not owned
	CContactDatabase* iContactsDb;
	CContactRemoteView * iContactRemoteView;
	TViewType iViewType;
	RArray<TInt> iTimes;
	TCntPerfTimer iTimer;
	TInt* iNumMsToGet20ItemsFromViewPtr;
	};


#endif //__TPERFSTARTUP__
