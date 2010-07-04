/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Summary:
* A class for creating a CContactDatabase object with random data.
* The contact items take the form of the following example:
* [KUidContactFieldFamilyName]	Fasiokljno	<5-10 chars>
* [KUidContactFieldGivenName]		Liasidwq	<5-10 chars>
* [KUidContactFieldCompanyName]	Xwjweiohj	<5-10 chars>	
* [KUidContactFieldPhoneNumber]	9874234891	<10 digits>
* Usage example:
*
*/


#include "dbcreator.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
// DbCreator methods
//////////////////////////////////////////////////////////////////////////////////////////////////

CContactDatabase* DbCreator::CreateDbL(const TDesC &aFileName, TInt aNumContacts)
	{
	CContactDatabase* db = CContactDatabase::ReplaceL(aFileName);
	CleanupStack::PushL(db);
	PopulateDatabaseL(*db, aNumContacts);
	CleanupStack::Pop();
	return db;
	}


TUint DbCreator::RandomNum(TUint aMin, TUint aMax)
    {
	TInt64 seed(Math::Random());
	return (static_cast<TUint>(Math::Rand(seed)) % ((aMax - aMin) + 1)) + aMin;
	}


TChar DbCreator::RandomAlphaChar(TBool aIsUpper = EFalse)
    {
	const TInt KUppercaseA(65);
	const TInt KLowercaseA(97);
	const TInt KZOffset(25);
	TInt min = aIsUpper ? KUppercaseA : KLowercaseA;
	TInt max = min + KZOffset;
	return TChar(RandomNum(min, max));
	}


HBufC* DbCreator::RandomNameL()
    {
	const TInt KNameLen( RandomNum(5, 10) );
	HBufC* name = HBufC::NewLC(KNameLen);
	TPtr namePtr = name->Des();
	for (TInt i = 0; i < KNameLen; ++i)
		{
		namePtr.Append(RandomAlphaChar(i == 0));		
		}
	CleanupStack::Pop(name);
	return name;
	}


HBufC* DbCreator::RandomPhoneNumL()
    {
	const TInt KAsciiZero(48);
	const TInt KTenDigits(10);
	HBufC* num = HBufC::NewLC(KTenDigits);
	TPtr numPtr = num->Des();
	for (TInt i = 0; i < KTenDigits; ++i)
		{
		numPtr.Append( TChar(RandomNum(KAsciiZero, KAsciiZero + 9) ) );		
		}
	CleanupStack::Pop(num);
	return num;
	}


void DbCreator::PopulateDatabaseL(CContactDatabase& aDb, TInt aNumContacts)
    {
	for(TInt i = 0; i < aNumContacts; ++i)
		{
		CContactCard* contact = CContactCard::NewLC();

		// first name
		HBufC* fnameBuf = RandomNameL();
		CleanupStack::PushL(fnameBuf);
		CContactItemField* fname = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldGivenName);
		TPtr fnamePtr = fnameBuf->Des();
		fname->TextStorage()->SetTextL(fnamePtr);
		contact->AddFieldL(*fname);
		CleanupStack::Pop(fname);
		CleanupStack::PopAndDestroy(fnameBuf);
		
		// last name
		HBufC* lnameBuf = RandomNameL();
		CleanupStack::PushL(lnameBuf);
		CContactItemField* lname = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldFamilyName);
		TPtr lnamePtr = lnameBuf->Des();
		lname->TextStorage()->SetTextL(lnamePtr);
		contact->AddFieldL(*lname);
		CleanupStack::Pop(lname);
		CleanupStack::PopAndDestroy(lnameBuf);

		// company name
		HBufC* cnameBuf = RandomNameL();
		CleanupStack::PushL(cnameBuf);
		CContactItemField* cname = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldCompanyName);
		TPtr cnamePtr = cnameBuf->Des();
		cname->TextStorage()->SetTextL(cnamePtr);
		contact->AddFieldL(*cname);
		CleanupStack::Pop(cname);
		CleanupStack::PopAndDestroy(cnameBuf);

		// phone number
		HBufC* numBuf = RandomPhoneNumL();
		CleanupStack::PushL(numBuf);
		CContactItemField* num = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldPhoneNumber);
		TPtr numPtr = numBuf->Des();
		num->TextStorage()->SetTextL(numPtr);
		contact->AddFieldL(*num);
		CleanupStack::Pop(num);
		CleanupStack::PopAndDestroy(numBuf);

		aDb.AddNewContactL(*contact);
		CleanupStack::PopAndDestroy(contact);
		}
	}

