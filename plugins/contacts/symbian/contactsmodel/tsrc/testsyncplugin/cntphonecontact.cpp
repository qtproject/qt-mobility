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


#include "cntphonecontact.h"
#include <cntdef.h>
#include <cntdb.h>

#include <centralrepository.h>  // crepository.
#include <e32property.h>
#include <phbksync.h>

// class CPhonebookContact - a rather simple data wrapper

void CContactSyncStaticRepository::WriteIntToCentralRepositoryL(TUint32& aKey, const TInt aValue, CRepository& iRepository)
{
	TRAPD(err, iRepository.Set(aKey++, aValue));
	if (err != KErrNone)
		User::Leave(err);	
}

void CContactSyncStaticRepository::ReadIntFromCentralRepositoryL(TUint32& aKey, TInt& aValue, CRepository& iRepository)
{
	TRAPD(err, iRepository.Get(aKey++, aValue));
	if (err != KErrNone)
		User::Leave(err);	
}

void CContactSyncStaticRepository::WriteStringToCentralRepositoryL(TUint32& aKey, TPtr aString, CRepository& iRepository)
{
	TRAPD(err, iRepository.Set(aKey++, aString));
	if (err != KErrNone)
		User::Leave(err);	
}

void CContactSyncStaticRepository::ReadStringFromCentralRepositoryL(TUint32& aKey, TPtr& aString, CRepository& iRepository)
{
	TRAPD(err, iRepository.Get(aKey++, aString));
	if (err != KErrNone)
		User::Leave(err);	
}

CPhonebookContact* CPhonebookContact::NewLC(const TDesC& aName, const TDesC& aNumber, CRepository* aRepository)
	{
	CPhonebookContact* self = new(ELeave) CPhonebookContact(aRepository);
	CleanupStack::PushL(self);
	self->ConstructL(aName, aNumber);
	return self;
	}

CPhonebookContact* CPhonebookContact::NewL(TUint32& aCRKey, CRepository* aRepository)
	{
	CPhonebookContact* self = new(ELeave) CPhonebookContact(aRepository);
	CleanupStack::PushL(self);
	self->ConstructL(aCRKey);
	CleanupStack::Pop(self);
	return self;
	}


CPhonebookContact::~CPhonebookContact()
	{
	delete iName;
	delete iNumber;
	}


CPhonebookContact::CPhonebookContact(CRepository* aRepository) : iRepository(aRepository)
	{
	}
	
	
void CPhonebookContact::ConstructL(const TDesC& aName, const TDesC& aNumber)
	{
	iName = aName.AllocL();
	iNumber = aNumber.AllocL();
	}
	
void CPhonebookContact::ConstructL(TUint32& aCRKey) 
	{
	//	How to read text from CR, probably the size and then the text
	TInt nameLength;
	CContactSyncStaticRepository::ReadIntFromCentralRepositoryL(aCRKey, nameLength, *iRepository);
	iName = HBufC::NewL(nameLength);
	TBuf<100> tempBuf1;
	TPtr tempPtr1 = iName->Des();
	CContactSyncStaticRepository::ReadStringFromCentralRepositoryL(aCRKey, tempPtr1, *iRepository);
	tempPtr1.Append(tempBuf1);
	TInt numberLength;
	CContactSyncStaticRepository::ReadIntFromCentralRepositoryL(aCRKey, numberLength, *iRepository);
	iNumber = HBufC::NewL(numberLength);
	TBuf<100> tempBuf2;
	TPtr tempPtr2 = iNumber->Des();
	CContactSyncStaticRepository::ReadStringFromCentralRepositoryL(aCRKey, tempPtr2, *iRepository);
	tempPtr2.Append(tempBuf2);	
	}
	
void CPhonebookContact::WriteToCRL(TUint32& aCRKey) const
	{
	CContactSyncStaticRepository::WriteIntToCentralRepositoryL(aCRKey, iName->Des().Length(), *iRepository);
	CContactSyncStaticRepository::WriteStringToCentralRepositoryL(aCRKey, iName->Des(), *iRepository);
 	CContactSyncStaticRepository::WriteIntToCentralRepositoryL(aCRKey, iNumber->Des().Length(), *iRepository);
	CContactSyncStaticRepository::WriteStringToCentralRepositoryL(aCRKey, iNumber->Des(), *iRepository);
	}



const HBufC* CPhonebookContact::Name()
	{
	return iName;
	}

const HBufC* CPhonebookContact::Number()
	{
	return iNumber;
	}


