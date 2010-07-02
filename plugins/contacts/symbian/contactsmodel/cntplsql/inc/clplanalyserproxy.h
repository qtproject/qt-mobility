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


#ifndef __CLPLANALYSERPROXY_H__
#define __CLPLANALYSERPROXY_H__

#include <e32base.h>
#include "persistencelayer.h"
#include "clplgenericproxy.h"


class MIniFileManager;


/**
The CLplAnalyserProxy proxy class sits between the State Machine and the
Persistence Layer.

This proxy has several functions.  It provides contacts server speed dial 
and initialisation file managers with dynamic information on the changes in the
database.

It also creates database event notification messages on the changes and sends
them to the CCntDbManager owning this object which in turn queues the events for
consumption by the client-side of the Contacts Model.
*/
class NONSHARED CLplAnalyserProxy : public CLplGenericProxy
	{
public:
	CLplAnalyserProxy(MIniFileManager& aIniManager,MContactDbObserverV2& aCntDbObserver, MLplPersistenceBroker& aBroker, MLplTransactionManager& aTranMan, MLplContactProperties& aProperties);

	/** Store a new Contact. */
	TContactItemId CreateL(CContactItem& aItem, TUint aSessionId);
	/** Update an existing Contact (or parts of it as specified by the view
	definition). */
	void UpdateL(CContactItem& aItem, TUint aSessionId, TBool aSpeeDailUpdate = EFalse);
	/** Delete a Contact. */
	CContactItem* DeleteLC(TContactItemId  aItemId, TUint aSessionId, TCntSendEventAction aEventType);
	/** Change the type of an existing Contact. */	
	void ChangeTypeL(TContactItemId aItemId, TUid aNewType);

	void SetConnectionId(TInt aConnectionId);

	void RollbackCurrentTransactionL(TUint aSessionId);

private:
	TUid SpeedDialFieldUidFromSpeedDialPosition(TInt aSpeedDialPosition);
	TBool CheckSpeedDialUpdatesL(CContactItem& aItem);
	TBool CheckSpeedDialDeletesL(TContactItemId aItemId);

	void CheckPrefTemplateDeleteL(TContactItemId aItemId);

	void NotifyObserverL(const TContactDbObserverEventType aType,
                         const TContactItemId aContactId,
                         const TUint aConnectionId = 0,
                         const TContactDbObserverEventTypeV2 aTypeV2 = EContactDbObserverEventV2Null,
                         const TContactItemId aAdditionalContactId = 0);

	MContactDbObserverV2& iCntDbObserver;
	MIniFileManager& iIniManager;
	MLplContactProperties& iProperties;

	TInt iConnectionId;
	};


#endif // __CLPLANALYSERPROXY_H__
