// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#ifndef __CNTPHONENUMBERPARSER_H__
#define __CNTPHONENUMBERPARSER_H__

#include <e32base.h>
#include <ecom/ecom.h>

/** The UID of the phone number parser ECom plugin interface. 
@released
@internalTechnology
*/
const TUid KUidEcomCntPhoneNumberParserInterface = {0x102035FA};

class CContactPhoneNumberParser : public CBase
/** Provides functionality to extract the real phone number from a contact's phone number field.
@publishedPartner
@released
*/
	{
public:
	virtual void ExtractRawNumber(const TDesC& aTextualNumber, TDes& aRawNumber)=0;
	};


class CContactEcomPhoneNumberParser : public CContactPhoneNumberParser
/** It provides Ecom Framework based facilities to resolve and load the appropriate implementations at run-time.
The framework supplies a default resolver for selecting appropriate implementations.
@publishedPartner
@released
*/
	{
public:
	static CContactEcomPhoneNumberParser* NewL(TUid aImplementationUid);
	inline virtual ~CContactEcomPhoneNumberParser();
private:
	TUid iDtor_ID_Key;
	};

inline CContactEcomPhoneNumberParser* CContactEcomPhoneNumberParser::NewL(TUid aImplementationUid)
/** Allocates and Instantiates an interface implementation to satisfy the specified interface. 
@param aImplementationUid denotes the type of implemetation that need to be loaded.
@return A pointer to a newly allocated and initialised object of type CContactEcomPhoneNumberParser.   */
	{
	TAny* ptr = REComSession::CreateImplementationL(aImplementationUid,
													_FOFF(CContactEcomPhoneNumberParser,
														  iDtor_ID_Key));
	return reinterpret_cast<CContactEcomPhoneNumberParser*>(ptr);
	}

inline CContactEcomPhoneNumberParser::~CContactEcomPhoneNumberParser()
/** The destruction of the interface implementation referred to by iDtor_ID_Key */
	{
	REComSession::DestroyedImplementation(iDtor_ID_Key);
	}
	
#endif //__CNTPHONENUMBERPARSER_H__
