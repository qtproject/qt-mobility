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




/**
 @file
 @publishedAll
 @released
*/
 
#ifndef __VIEW_OBSERVER_H__
#define __VIEW_OBSERVER_H__

#include <cntviewbase.h>
#include "contactviewtest.h"

class CViewObserver : public CActive, public MContactViewObserver
	{
public:
	static CViewObserver* NewLC(const CContactViewTest &aTest, void (CContactViewTest::*aFunc)() );
	~CViewObserver();
	void SetView(const CContactViewBase *aLocalView);
	void Activate();
	void CleanView();
private:
	CViewObserver(	const CContactViewTest &aTest, void (CContactViewTest::*aFunc)());
	
private: // From CActive.
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);
private: // From MContactViewObserver.
	virtual void HandleContactViewEvent(const CContactViewBase& aView, 
										const TContactViewEvent& aEvent);
public:
	void (CContactViewTest::*iMainFunc)(); 
	void (CContactViewTest::*iDoFunc)(const CContactViewBase & ); 
	
public:
	TContactViewEvent iEvent;

private:
	CContactViewTest 	&iTest;
	CContactViewBase	*iLocalView;

	};
	
#endif
