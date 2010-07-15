/*
* Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __CNTPHONE_H__
#define __CNTPHONE_H__

#include <e32std.h>
#include <cntdb.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <cntphonenumparser.h>
#endif

/**
 * Default implementation of a phone number parser
 * Conforms to ETSI DTMF specifications.
 */

class CContactDefaultPhoneNumberParser : public CContactEcomPhoneNumberParser
	{
public:
	//from CContactPhoneNumberParser
	void ExtractRawNumber(const TDesC& aTextualNumber, TDes& aRawNumber);
	static CContactDefaultPhoneNumberParser* NewL();
	};


#endif
