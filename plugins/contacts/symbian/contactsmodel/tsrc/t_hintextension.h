/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __T_HINTEXTENSION__
#define __T_HINTEXTENSION__

#include <cntview.h>



class CViewTester : public CActive, public MContactViewObserver
	{
public:
	static CViewTester* NewLC();
	~CViewTester();

	//MContactViewObserver implementation
	void HandleContactViewEvent(const CContactViewBase& aView,const TContactViewEvent& aEvent);

	void CreateFilterViewsL();
private: 
	CViewTester() : CActive(EPriorityStandard) {};

	// From CActive.
	void RunL() {};
	void DoCancel() {};

	CContactDatabase* iDatabase; 

	CContactNamedRemoteView* iCNRView;
	CContactFilteredView* iRingToneView;
	CContactFilteredView* iVoiceDialView;
	TInt iEventsReceived;
	};

#endif
