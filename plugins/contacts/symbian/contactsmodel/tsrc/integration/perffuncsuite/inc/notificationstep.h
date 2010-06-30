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
#ifndef __NOTIFICATION_STEP_H__
#define __NOTIFICATION_STEP_H__
#include <testexecutestepbase.h>
#include "performancefunctionalitybase.h"

class CNotificationStep : public CPerformanceFunctionalityBase
	{	
class CNotificationActive;
friend class CNotificationActive;		
public:
	CNotificationStep();
	~CNotificationStep();
	virtual TVerdict doTestStepL();

private:
	void PreTestL();
	void InitializeL();
	void Cleanup();
	void ClearNotifications();
	void ResetArrays();
	void AddEventL(const TInt aEvent);
	void AddContactL(const TInt aCid);
	void EndTransactionL(const TBool aRevert);
	void UnkownChangeTestL();
	TBool UnkownChangeCheck();
	void TransactionUnkownChangeTestL(const TBool aRevert);
	TBool TransactionUnkownChangeCheck();
	void PlusTransactionUnkownChangeTestL(const TBool aRevert);
	
private: 
	TTime 							iStart;
	TTime 							iEnd;
	TInt 							iMessageCount;
	CContactChangeNotifier* 		iNotifier;
	RArray<TInt>* 					iSharedContacts;
	RArray<TInt>* 					iSharedEvents;
	RArray<TInt>* 					iDBContacts;
	RArray<TInt>* 					iDBEvents;
	CNotificationStep::CNotificationActive* 	iMyActive;

private:
	class CNotificationActive : public CActive, MContactDbObserver
		{
	friend class CNotificationStep;
	public:
		CNotificationActive(CNotificationStep* aStep) : CActive(EPriorityIdle),
													iStep(aStep){}
	private:
		void Activate();		
	private: // From CActive.
		void RunL();
		void DoCancel();
		TInt RunError(TInt aError);

	private: // from MContactDbObserver
		void HandleDatabaseEventL(TContactDbObserverEvent aEvent);
	private:
		CNotificationStep* iStep;
		TBool (CNotificationStep::* iCheck)();
	private:
		TBool iClear;//If true expect to receive non test related notificationa for adding contacts in preamble / constructor
		};
	};

_LIT(KNotificationStep,"NotificationStep");

#endif
