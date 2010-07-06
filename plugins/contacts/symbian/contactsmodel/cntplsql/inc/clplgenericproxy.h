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


#ifndef __CLPLGENERICPROXY_H__
#define __CLPLGENERICPROXY_H__

#include <e32base.h>
#include "persistencelayer.h"


/**
This CLplGenericProxy class provides the default behavior for proxy classes.

The default behaviour is to pass the calls on without any action to the
Persistence Broker or Transaction Manager.

The proxy controls all the basic operations on the database (such as Create,
Read, Update and Delete) and transaction management operations.

Future implementations of the proxy classes could extend the business rules of
the contacts model or even plugin alternative data sources.  All these
implementations should derive from this base class.
*/
class NONSHARED CLplGenericProxy : public CBase, public MLplPersistenceBroker, public MLplTransactionManager
	{
public:
	//
	// CRUD methods.
	//
	/** Store a new Contact. */
	virtual TContactItemId CreateL(CContactItem& aItem, TUint aSessionId);
	/** Read an existing Contact (or parts of it as specified by the view
	definition). */
	virtual CContactItem* ReadLC(TContactItemId aItemId, const CContactItemViewDef& aView, TInt aInfoToRead, TUint aSessionId, TBool aIccOpenCheck = EFalse) const;
	/** Update an existing Contact (or parts of it as specified by the view
	definition). */
	virtual void UpdateL(CContactItem& aItem, TUint aSessionId);
	/** Delete a Contact. */
	virtual CContactItem* DeleteLC(TContactItemId  aItemId, TUint aSessionId, TCntSendEventAction aEventType);
	/** Change the type of an existing contact. */
	virtual void ChangeTypeL(TContactItemId aItemId, TUid aNewType);
	/** Set connection ID for the notification mechanism. */
	virtual void SetConnectionId(TInt aConnectionId);

	//
	// Transaction methods.
	//
	/** Start a new transaction. */
	virtual void StartTransactionL();
	/** Commit the current transaction. */
	virtual void CommitCurrentTransactionL(TUint aSessionId);
	/** Rollback the current transaction. */
	virtual void RollbackCurrentTransactionL(TUint aSessionId);
	/** Determine if a transaction is currently in progress. */
	virtual TBool IsTransactionActive() const;
	
protected:
	CLplGenericProxy(MLplPersistenceBroker& aBroker, MLplTransactionManager& aTranMan);

private:
	MLplPersistenceBroker& iBroker; 
	MLplTransactionManager& iTranMan;
	};


#endif //__CLPLGENERICPROXY_H__
