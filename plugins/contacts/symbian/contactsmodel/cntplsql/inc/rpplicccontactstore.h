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


#ifndef __RPPLICCCONTACTSTORE_H__
#define __RPPLICCCONTACTSTORE_H__

#include <cntsync.h>
#include "persistencelayer.h"

/**
The RPplIccContactStore class works as an adaptor between the Persistence Layer
and the phonebook synchroniser server.

From one side it behaves like a Persistence Layer table and is registered as a
child of the Contacts table.  This is achieved by implementing the
MPplContactItemPersistor interface.

From the other side it uses the contact synchroniser plugin to communicate with
the phonebook synchroniser.

Since DBMS and all the tables in the Persistence Layer support transactions and
the phonebook synchroniser does not, the RPplIccContactStore class takes this
responsibility.  The class implements the MLplTransactionManager interface and
uses an internal array to keep all the actions required for transaction rollback
during the transaction.
*/
class NONSHARED RPplIccContactStore // : public MLplTransactionManager//,  public MPplContactItemPersistor
	{
public:
	enum TIccContactOperationType
		{
		EIccContactCreate,
		// Reading is not supported.
		EIccContactUpdate,
		EIccContactDelete
		};
	
	struct TIccContactOperation
		{
		TIccContactOperation(CContactItem& aItem, TIccContactOperationType aType)
			: iContact(aItem), iType(aType) {}
		CContactItem& iContact;
		TIccContactOperationType iType;
		};
	
	RPplIccContactStore(CLplContactProperties& aContactProperties);
	void Close();
	
	// From MLplTransactionManager.
	void StartTransactionL();
	void CommitCurrentTransactionL(TUint aSessionId);
	void RollbackCurrentTransaction(TUint aSessionId);
	TBool IsTransactionActive() const;
	
	TContactItemId CreateInDbL (CContactItem& aItem, TUint aSessionId);
	void ReadL(CContactItem& aItem, TInt aInfoToRead, TUint aSessionId, TBool aIccOpenCheck=EFalse) const;	
	void UpdateL(CContactItem& aNewItem, TUint aSessionId);
	void DeleteL(const CContactItem& aItem, TUint aSessionId);
	
	void PersistorChangeTypeL(TContactItemId aItemId, TUid aNewType);
	void ExecuteTransaction();
	
private:
	void ConstructL();
	void ExecuteTransactionL(TUint aSessionId);
	void RollbackTransactionL(TUint aSessionId);
	void CreateIccContactL(CContactItem& aItem, TUint aSessionId);
	void UpdateIccContactL(CContactItem& aItem, TUint aSessionId);
	void DeleteIccContactL(const CContactItem& aItem, TUint aSessionId);
	void ValidateIccContactL(MContactSynchroniser::TValidateOperation aOp,
							 CContactItem& aItem, TUint aSessionId) const;
	
private:
	TBool iTransactionActive;
	RArray<TIccContactOperation> iTransactionOps;
	CLplContactProperties& iProperties;
	};


#endif // __RPPLICCCONTACTSTORE_H__
