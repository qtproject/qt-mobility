/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "rpplicccontactstore.h"
#include "clplcontactproperties.h"
#include <phbksync.h>

/**
RPplIccContactStore constructor.
*/
RPplIccContactStore::RPplIccContactStore(CLplContactProperties& aContactProperties)
	:
	iProperties(aContactProperties)	
	{
	}


/**
Close the ICC contacts store.
*/
void RPplIccContactStore::Close()
	{
	iTransactionOps.Close();
	}


/**
Marks the start of a transaction.  If a transaction is already in progress
leaves with KErrAlreadyExists.

This method is part of the MLplTransactionManager interface.
*/
void RPplIccContactStore::StartTransactionL()
	{
	// If transaction already active then leave.
	if (iTransactionActive)
		{
		User::Leave(KErrAlreadyExists);
		}

	iTransactionActive = ETrue;
	}


/**
Commits the current transaction.  If no transaction is in progress leaves with
KErrNotFound.

This method is part of the MLplTransactionManager interface.

@param aSessionId The ID of the session that issued the request.  Used to
prevent Phonebook Synchroniser deadlock.
*/
void RPplIccContactStore::CommitCurrentTransactionL(TUint aSessionId)
	{
	// If no transaction active then leave.
	if (!iTransactionActive)
		{
		User::Leave(KErrNotFound);
		}

	ExecuteTransactionL(aSessionId);
	// Clear operation cache.
	iTransactionOps.Reset();

	iTransactionActive = EFalse;
	}


/**
Rolls back the current transaction.  If no transaction is in progress then
returns.

This method is part of the MLplTransactionManager interface.

@param aSessionId The ID of the session that issued the request.  Used to
prevent Phonebook Synchroniser deadlock.
*/
void RPplIccContactStore::RollbackCurrentTransaction(TUint aSessionId)
	{
	if (!iTransactionActive)
		{
		return;
		}

	RollbackTransactionL(aSessionId);
	// Clear operation cache.
	iTransactionOps.Reset();

	iTransactionActive = EFalse;
	}


TBool RPplIccContactStore::IsTransactionActive() const 
	{
	return iTransactionActive;
	}


/**
Creates an ICC entry for the given contact item.

Transactions are handled differently when creating ICC contacts.  We need to
actually write the ICC contact before the transaction is commited.  Update and
Delete operations can be done at commit time.

@param aItem Contact item for which to create ICC entry.
@param aSessionId The ID of the session that issued the request.  Used to
prevent Phonebook Synchroniser deadlock.
*/
TContactItemId RPplIccContactStore::CreateInDbL(CContactItem& aItem, TUint aSessionId)
	{
	#if defined(__PROFILE_DEBUG__)
		RDebug::Print(_L("[CNTMODEL] MTD: RPplIccContactStore::CreateInDbL"));
	#endif 
		
	// Make sure the contact is a CContactICCEntry.  If it isn't then don't do
	// anything.
	if (aItem.Type() != KUidContactICCEntry)
		{
		return KNullContactId;
		}

	// If in transaction then add to cache.
	if (iTransactionActive)
		{
		TIccContactOperation op(aItem, EIccContactCreate);
		iTransactionOps.AppendL(op);
		}
	
	// Perform operation.
	CreateIccContactL(aItem, aSessionId);
	
	// Obtain the ID of the ICC group.
	TContactItemId groupId = KNullContactId;
	const CContactItemFieldSet& fieldset = aItem.CardFields();
	const TInt n = fieldset.Find(KUidContactFieldICCPhonebook);
	if(n!=KErrNotFound) // Is KUidContactFieldICCPhonebook field supplied?
		{
		TPtrC phonebookField = fieldset[n].TextStorage()->Text();
		TLex lex(phonebookField);
		TUid phonebookUid;
		lex.Val(phonebookUid.iUid);
		groupId = iProperties.ContactSynchroniserL(aSessionId).GroupIdL(phonebookUid);
		}
	else
		{
		// Client hasn't supplied the KUidContactFieldICCPhonebook field.
		// The Phonebook Synchroniser will provide the group ID for the
		// Global/GSM ADN phonebook group as a default.
		groupId = iProperties.ContactSynchroniserL(aSessionId).GroupIdL(KUidIccGlobalAdnPhonebook);
		}

    return groupId;	
	}


/**
Validate the entry to make sure it can be read from the ICC.

@param aItem Contact item to be validated for reading from the ICC.
@param aSessionId The ID of the session that issued the request.  Used to
prevent Phonebook Synchroniser deadlock.
@param aIccOpenCheck ETrue if validation for open for edit should also be done.
*/
void RPplIccContactStore::ReadL(CContactItem& aItem, TInt /*aInfoToRead*/, TUint aSessionId, TBool aIccOpenCheck) const
	{
	// Make sure the contact is a CContactICCEntry.  If it isn't then don't do
	// anything.
	if (aItem.Type() != KUidContactICCEntry)
		{
		return;
		}
	
	ValidateIccContactL(MContactSynchroniser::ERead, aItem, aSessionId);

	if(aIccOpenCheck)
		{
		ValidateIccContactL(MContactSynchroniser::EEdit, aItem, aSessionId);	
		}
	}


/**
Update an ICC entry using the given contact item.  If in a transaction then just
add the operation to the cache, otherwise do the update directly.

@param aNewItem The contact to be updated.
@param aSessionId The ID of the session that issued the request.  Used to
prevent Phonebook Synchroniser deadlock.
*/
void RPplIccContactStore::UpdateL(CContactItem& aNewItem, TUint aSessionId)
	{
	// Make sure the contact is a CContactICCEntry.  If it isn't then don't do
	// anything.
	if (aNewItem.Type() != KUidContactICCEntry)
		{
		return;
		}

	// If in transaction then add to cache.
	// Otherwise perform operation.
	if (iTransactionActive)
		{
		TIccContactOperation op(aNewItem, EIccContactUpdate);
		iTransactionOps.AppendL(op);
		}
	else
	    {
        if (aNewItem.IsDeleted() && (aNewItem.AccessCount() == 0))
            {
            DeleteL(aNewItem, aSessionId);
            }
    	else
    		{
    		UpdateIccContactL(aNewItem, aSessionId);
    		}
	    }	
	}


/**
Delete the ICC entry associated with the given contact item.

@param aItem Contact item whose associated ICC entry is to be deleted.
@param aSessionId The ID of the session that issued the request.  Used to
prevent Phonebook Synchroniser deadlock.
*/
void RPplIccContactStore::DeleteL(const CContactItem& aItem, TUint aSessionId)
	{
	// Make sure the contact is a CContactICCEntry and that it can be deleted.
	// If not then don't do anything.
	if (aItem.Type() != KUidContactICCEntry ||
		!const_cast<CContactItem&>(aItem).IsDeletable())
		{
		return;
		}
	DeleteIccContactL(aItem, aSessionId);
	}


void RPplIccContactStore::PersistorChangeTypeL(TContactItemId /*aItemId*/, TUid /*aNewType*/)
	{
	// Do nothing.
	}


/**
Perform the operations stored in the cache.

@param aSessionId The ID of the session that issued the request.  Used to
prevent Phonebook Synchroniser deadlock.
*/
void RPplIccContactStore::ExecuteTransactionL(TUint aSessionId)
	{
	for (TInt i = 0; i < iTransactionOps.Count(); i++)
		{
		TIccContactOperation op = iTransactionOps[i];
		switch (op.iType)
			{
			case EIccContactCreate:
				// Nothing to do since create operations are executed
				// immediately, even in transactions.
				break;
			case EIccContactUpdate:
				UpdateIccContactL(op.iContact, aSessionId);
				break;
			case EIccContactDelete:
				DeleteIccContactL(op.iContact, aSessionId);
				break;
			default:
				User::Leave(KErrNotSupported);
				break;
			}
		}
	}

	
/**
Rollback the cached operations.  Only Create operations need special attention
since we need to remove those ICC entries that were created during the
transaction.

@param aSessionId The ID of the session that issued the request.  Used to
prevent Phonebook Synchroniser deadlock.

@leave KErrNoMemory Out of memory.
@leave KErrGeneral
@leave [other system-wide leave codes]
*/
void RPplIccContactStore::RollbackTransactionL(TUint aSessionId)
	{
	for (TInt i = 0; i < iTransactionOps.Count(); i++)
		{
		TIccContactOperation op = iTransactionOps[i];
		switch (op.iType)
			{
			case EIccContactCreate:
				// Delete the ICC entry.
				DeleteIccContactL(op.iContact, aSessionId);
				break;
			case EIccContactUpdate:
			case EIccContactDelete:
				// Nothing to do.
				break;
			default:
				// Give up.
				return;
			}
		}
	}


/**
Write a contact using the Phonebook Synchroniser.  We can call the
UpdatePostWriteL() method right after ValidateWriteContact() because we make the
assumption that by the time this function is called the contact item has already
had a UID assigned to it.

@param aSessionId The ID of the session that issued the request.  Used to
prevent Phonebook Synchroniser deadlock.
*/
void RPplIccContactStore::CreateIccContactL(CContactItem& aItem, TUint aSessionId)
	{
	iProperties.ContactSynchroniserL(aSessionId).UpdatePostWriteL(static_cast<CContactICCEntry&>(aItem));
	}


/**
Update a contact using the Phonebook Synchroniser.

@param aItem Contact item whose associated ICC entry is to be updated.
@param aSessionId The ID of the session that issued the request.  Used to
prevent Phonebook Synchroniser deadlock.
*/
void RPplIccContactStore::UpdateIccContactL(CContactItem& aCurrItem, TUint aSessionId)
	{
	TInt ret = iProperties.ContactSynchroniserL(aSessionId).ValidateWriteContact(static_cast<CContactICCEntry&>(aCurrItem));
	User::LeaveIfError(ret);			
	}


/**
Delete a contact using the Phonebook Synchroniser.

@param aItem Contact item whose associated ICC entry is to be deleted.
@param aSessionId The ID of the session that issued the request.  Used to
prevent Phonebook Synchroniser deadlock.

@leave KErrNoMemory Out of memory.
@leave KErrGeneral
*/
void RPplIccContactStore::DeleteIccContactL(const CContactItem& aItem, TUint aSessionId)
	{
	TInt ret = iProperties.ContactSynchroniserL(aSessionId).DeleteContact(aItem.Id());
	User::LeaveIfError(ret);	
	}


/**
Validate a contact using the Phonebook Synchroniser.

@param aOp Validate operation.
@param aItem Contact item for which the operation is to be validated.
@param aSessionId The ID of the session that issued the request.  Used to
prevent Phonebook Synchroniser deadlock.
*/
void RPplIccContactStore::ValidateIccContactL(MContactSynchroniser::TValidateOperation aOp,
	CContactItem& aItem, TUint aSessionId) const
	{
	TInt ret = iProperties.ContactSynchroniserL(aSessionId).ValidateContact(aOp, aItem.Id());
	User::LeaveIfError(ret);
	}
