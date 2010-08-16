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


#ifndef __TPERFRESORTVIEW__
#define __TPERFRESORTVIEW__

#include "../t_utils.h"
#include <e32std.h>
#include <e32test.h>
#include <cntview.h>

class CViewResort : public CActive, public MContactViewObserver
	{
public:
	enum TViewType
		{
		ERemoteContactsView = 0,
		ERemoteGroupsView
		};
	enum TViewState
		{
		ECreatedView = 0,
		EResortedViewByName,
		EResortedViewByNumber
		};
public:
	static CViewResort* NewLC(RTest& aTest);
	void RunL();
	void DoCancel();
	void HandleContactViewEvent(const CContactViewBase& aView, const TContactViewEvent& aEvent);
	void DoTestL(const TDesC& aDbName);
	
private:
	~CViewResort();
	CViewResort(RTest& aTest);
	void ConstructL();
	void CreateRemoteViewL();
	void Cleanup();
	void ResortL();
		
private:
	RTest& iTest; // not owned
	CContactDatabase* iContactsDb;
	CContactNamedRemoteView* iNamedRemoteView;
	TViewState iViewState;
	TCntPerfTimer iTimer;
	};

#endif //__TPERFRESORTVIEW__
