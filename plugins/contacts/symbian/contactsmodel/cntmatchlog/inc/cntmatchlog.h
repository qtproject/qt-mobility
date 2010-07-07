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


#ifndef __CNTMATCHLOG_H__
#define __CNTMATCHLOG_H__

#include <e32base.h>
#include <logcntmodel.h>
#include <cntdb.h>

class CCntMatchLog : public CLogCntModel
/** Plugin implementation that provide contacts matching functionality. 
 
@internalAll */
	{
public:
	static CCntMatchLog* NewL();
	virtual void OpenContactsL();
	virtual	void CloseContacts();
	virtual TLogContactItemId MatchPhoneNumberL(const TDesC& aNumber, TInt aMatchLengthFromRight);
	virtual void ReadContactNameL(TLogContactItemId aContactId, TDes &aName, TLogContactNameFormat aNameFormat);

private:
	~CCntMatchLog();
	CCntMatchLog();
	CContactDatabase* iContactDb;
	};
	
#endif
