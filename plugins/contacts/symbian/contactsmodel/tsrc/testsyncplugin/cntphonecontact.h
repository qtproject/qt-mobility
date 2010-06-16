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



#ifndef CNTPHONECONTACT_H
#define CNTPHONECONTACT_H


#include <centralrepository.h>  // crepository.
#include <e32property.h>
#include <phbksync.h>

class CContactSyncStaticRepository 
	{
public:
	static void WriteIntToCentralRepositoryL(TUint32& aKey, const TInt aValue, CRepository& iRepository);
	static void ReadIntFromCentralRepositoryL(TUint32& aKey, TInt& aValue, CRepository& iRepository);
	static void WriteStringToCentralRepositoryL(TUint32& aKey, TPtr aString, CRepository& iRepository);
	static void ReadStringFromCentralRepositoryL(TUint32& aKey, TPtr& aString, CRepository& iRepository);
	};

class CPhonebookContact : public CBase
	{

public:
	static CPhonebookContact* NewLC(const TDesC& aName, const TDesC& aNumber, CRepository* aRepository);
	static CPhonebookContact* NewL(TUint32& aCRKey, CRepository* aRepository);
	~CPhonebookContact();
	const HBufC* Name();
	const HBufC* Number();
	void WriteToCRL(TUint32& aCRKey) const;	
	CPhonebookContact(CRepository* aRepository);
private:
	void ConstructL(const TDesC& aName, const TDesC& aNumber);
	void ConstructL(TUint32& aCRKey);

private:
	HBufC*	iName;
	HBufC*	iNumber;
	CRepository* iRepository;
	};

#endif

