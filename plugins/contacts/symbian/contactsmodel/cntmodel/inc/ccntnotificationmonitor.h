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




/**
 @file
 @internalComponent
 @released
*/


#ifndef CCNT_NOTIFICATIONMONITOR_H
#define CCNT_NOTIFICATIONMONITOR_H

#include <e32base.h>
#include <f32file.h>
#include <badesca.h>

#include <cntdbobs.h> // for tcontactdbobserverevent.


/**
The CCntNotificationMonitor class provides the base class for monitoring of
event notifications from the Server.
*/
NONSHARABLE_CLASS(CCntNotificationMonitor) : public CActive
{
protected:
	CCntNotificationMonitor(RCntModel& aSession);
	~CCntNotificationMonitor();	
	void DoCancel();

protected:
	RCntModel& iSession;
};


/**
The CCntDbNotifyMonitor class is an Active Object which makes requests for
Contacts database events from the Server.  Each Contacts database event
received from the Server is sent to all registered observers.  Each observer
implements the MContactDbObserver interface which consists of the
HandleDatabaseEventL() method.
*/
NONSHARABLE_CLASS(CCntDbNotifyMonitor) : private CCntNotificationMonitor
{
public:
	static  CCntDbNotifyMonitor* NewL(RCntModel& aSession);
	void 	AddObserverL(MContactDbObserver& aObserver);
	void 	RemoveObserver(const MContactDbObserver& aObserver);
	void    AddObserverV2L(MContactDbObserverV2& aObserver);
	void    RemoveObserverV2(const MContactDbObserverV2& aObserver);
	~CCntDbNotifyMonitor();
	TInt    NumberOfListeners() const;

private:
	CCntDbNotifyMonitor(RCntModel& aSession);
	void 	RunL();
	void 	Start();
	TInt	RunError(TInt aError);

private:	
	RPointerArray<MContactDbObserver> iObserverArray;
	RPointerArray<MContactDbObserverV2> iObserverV2Array;
	TPckgBuf<TContactDbObserverEventV2> iEventMsg;
	TInt iCurrentProcessedObserver;
};


#endif // CCNT_NOTIFICATIONMONITOR_H
