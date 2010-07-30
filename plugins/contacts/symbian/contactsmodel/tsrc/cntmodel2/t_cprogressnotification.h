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



#ifndef __T_PROGRESS_NOTIFY_TEST_H__
#define __T_PROGRESS_NOTIFY_TEST_H__

#include "nbcnttestlib.h"
#include <cntitem.h>
#include <cntdbobs.h>


const TInt KMaxArraySize = 1000;

class CProgressNotificationTest : public CBase, public MContactUiCompactObserver
{
public:		
	void RunRecoverNotifyTestL();
	void RunCompressNotifyTestL();
	void RunRecoverNotifyCancelTestL();  // Negative test
	void RunCompressNotifyCancelTestL(); // Negative test
	
	static CProgressNotificationTest* NewLC();
	~CProgressNotificationTest();
	
	// From MContactUiCompactObserver
	void Step(TInt aStep);
	void HandleError(TInt aError);
	
	void AddContactsL(TInt aNoOfCnts = 1000);
	void DeleteContactsL(TInt aNoOfCnts = 500);
	
private:
	CProgressNotificationTest();
	const CContactTemplate&  GetSysTemplateL();
	void ConstructL();
	
private:
	TContactItemId    iIDarray [1000];
	CContactDatabase* iCntDb;
	CCntItemBuilder*  iCntItemBldr;
	CContactTemplate* iGoldenTemplate;
	RTest* iTest;
	CContactActiveCompress* iActiveCompress;
	CContactActiveRecover*  iActiveRecover;
	TInt iSteps;
};


#endif // __T_PROGRESS_NOTIFY_TEST_H__
