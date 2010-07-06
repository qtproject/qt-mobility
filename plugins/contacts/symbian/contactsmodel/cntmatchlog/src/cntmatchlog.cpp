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


#include "cntmatchlog.h"
#include <e32def.h>
#include <ecom/implementationproxy.h>

CCntMatchLog* CCntMatchLog::NewL()
	/** Creates a CCntMatchLog object for accessing the contacts model. 
	@capability None */
	{
   	CCntMatchLog* self = new (ELeave) CCntMatchLog;
   	return self;
	}

void CCntMatchLog::OpenContactsL()
	/** Opens the contacts DB and stores a pointer to the DB in iContactDb.	
	@capability ReadUserData WriteUserData*/
	{
	if( iContactDb)
		{
		return;
		}
		
	TRAPD(err, iContactDb = CContactDatabase::OpenL());
	if(err == KErrNotFound)
		{
		iContactDb = CContactDatabase::CreateL();
		}
	else
		{
		User::LeaveIfError(err);
		}
	}
	
void CCntMatchLog::CloseContacts()
	/** Closes the contacts DB and deletes the pointer to the DB. 
	@capability None */
	{
	if( iContactDb)
		{	
		delete iContactDb;
		iContactDb = NULL;
		}
	}
	
CCntMatchLog::~CCntMatchLog()
	/** Closes the contacts DB */
	{
	this->CloseContacts();
	}
	
TLogContactItemId CCntMatchLog::MatchPhoneNumberL(const TDesC& aNumber, TInt aMatchLengthFromRight)
	/** Attempts to find a contact item ID for the contact items which contains
	the specified telephone number in a telephone, fax or SMS type field.
	If more than one contact item contains the telephone number this should be 
	treated the same as no contact found.
	
	@capability ReadUserData
	@param aNumber Phone number string
	@param aMatchLengthFromRight Number of digits from the right of the phone number to use
	@return The contact Id found that contains the phone number. KLogNullContactId if none or more than one is found. */
	{
	CContactIdArray* array = NULL;
	TLogContactItemId contactId=KLogNullContactId;
	
	array = iContactDb->MatchPhoneNumberL(aNumber, aMatchLengthFromRight);
				
	// Only set contactId if we have exactly one match
	if (array->Count() == 1)
		{
		// we have only one match so set the contact id
		contactId = static_cast<TLogContactItemId>((*array)[0]);
		}
	delete array;
	return contactId;
	}
	
void CCntMatchLog::ReadContactNameL(TLogContactItemId aContactId, TDes &aName, TLogContactNameFormat aNameFormat)
	/** Gets the text data for the family and given name fields of a given contact Id.
	
	@capability ReadUserData
	@param aContactId Contact Id to find data for
	@param aName On return contains the family and given name in the desired format if found, a 0 length string otherwise.
	@param aNameFormat Desired format of returned string - Chinese or Western format */
	{
		
	// Specify what fields to fetch and concatenate
	CContactTextDef* textDef = CContactTextDef::NewLC();
	_LIT(KRemotePartyNameSeparator, " ");	
	
	if(ELogChineseFormat == aNameFormat)
		{
		textDef->AppendL(TContactTextDefItem(KUidContactFieldFamilyName, KRemotePartyNameSeparator));
		textDef->AppendL(TContactTextDefItem(KUidContactFieldGivenName));
		}
	else //ELogWesternFormat == iContactNameFormat
		{
		textDef->AppendL(TContactTextDefItem(KUidContactFieldGivenName, KRemotePartyNameSeparator));
		textDef->AppendL(TContactTextDefItem(KUidContactFieldFamilyName));
		}
	textDef->SetExactMatchOnly(ETrue);
	iContactDb->ReadContactTextDefL(aContactId, aName, textDef);
	CleanupStack::PopAndDestroy(textDef);
	}
	

CCntMatchLog::CCntMatchLog()
	{
	}
		
const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(0x2000862C, CCntMatchLog::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }

