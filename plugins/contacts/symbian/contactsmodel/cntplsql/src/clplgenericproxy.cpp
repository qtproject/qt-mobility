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


#include "clplgenericproxy.h"


CLplGenericProxy::CLplGenericProxy(MLplPersistenceBroker& aBroker, MLplTransactionManager& aTranMan)
	:
	iBroker(aBroker),
	iTranMan(aTranMan)
	{
	}


/**
Store a new Contact.
*/
TContactItemId CLplGenericProxy::CreateL(CContactItem& aItem, TUint aSessionId)
	{
	#if defined(__PROFILE_DEBUG__)
		RDebug::Print(_L("[CNTMODEL] MTD: CLplGenericProxy::CreateL"));
	#endif 
	
	return iBroker.CreateL(aItem, aSessionId);
	}


/**
Read an existing Contact (or parts of it as specified by the view definition).
*/
CContactItem* CLplGenericProxy::ReadLC(TContactItemId aItemId, const CContactItemViewDef& aView, TInt aInfoToRead, TUint aSessionId, TBool aIccOpenCheck) const
	{
	return iBroker.ReadLC(aItemId, aView, aInfoToRead, aSessionId, aIccOpenCheck);
	}


/**
Update an existing Contact (or parts of it as specified by the view definition).
*/
void CLplGenericProxy::UpdateL(CContactItem& aItem, TUint aSessionId)
	{
	iBroker.UpdateL(aItem, aSessionId);
	}


/**
Delete a Contact.
*/
CContactItem* CLplGenericProxy::DeleteLC(TContactItemId  aItemId, TUint aSessionId, TCntSendEventAction aEventType)
	{
	return iBroker.DeleteLC(aItemId, aSessionId, aEventType);
	}


/**
Change the type of an existing Contact.
*/
void CLplGenericProxy::ChangeTypeL(TContactItemId aItemId, TUid aNewType)
	{
	iBroker.ChangeTypeL(aItemId, aNewType);
	}


/**
Start a new transaction.
*/
void CLplGenericProxy::StartTransactionL()
	{
	iTranMan.StartTransactionL();
	}


/**
Commit the current transaction.
*/
void CLplGenericProxy::CommitCurrentTransactionL(TUint aSessionId)
	{
	iTranMan.CommitCurrentTransactionL(aSessionId);
	}


/**
Rollback the current transaction.
*/
void CLplGenericProxy::RollbackCurrentTransactionL(TUint aSessionId)
	{
	iTranMan.RollbackCurrentTransactionL(aSessionId);
	}


/**
Determine if a transaction is currently in progress.
*/
TBool CLplGenericProxy::IsTransactionActive() const
	{
	return iTranMan.IsTransactionActive();
	}


/** Does nothing since the analyser proxy sets the connection ID.
*/
void CLplGenericProxy::SetConnectionId(TInt /*aConnectionId*/)
	{
	}
