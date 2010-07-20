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




/**
 @file 
 @publishedAll
 @released
*/
#if (!defined __TIMEOUT_BASE_STEP_H__)
#define __TIMEOUT_BASE_STEP_H__
#include <testexecutestepbase.h>
#include "performancefunctionalitybase.h"

class CTimeoutBaseStep : public CPerformanceFunctionalityBase
	{
protected:
	CTimeoutBaseStep(const TInt aContacts, const TInt aDeviation);
	void SetTimeOutL(CContactDatabase *aContactsDatabase = NULL);	
	TBool CheckTimeOutL(TInt (CTimeoutBaseStep::*aFunc)(CContactDatabase *, const TContactItemId),
						CContactDatabase *aContactsDatabase, 
						const TContactItemId aCid,
						const TInt aLockErr,
						const TInt aTimeout,
						const TBool aSetTime=ETrue);
						
//methods to perform and cleanup a single CRUD operation						
protected:
	TInt CheckOpenL(CContactDatabase *aContactsDatabase, const TContactItemId aCid);
	TInt CheckUpdateL(CContactDatabase *aContactsDatabase, const TContactItemId aCid);
	TInt CheckRead(CContactDatabase *aContactsDatabase, const TContactItemId aCid);
	TInt CheckAddL(CContactDatabase *aContactsDatabase, const TContactItemId aCid);
	TInt CheckDelete(CContactDatabase *aContactsDatabase, const TContactItemId aCid);
						
protected:
	TInt iTimeOut;	
	TTime iStart;
	TTime iEnd;
	const TInt iDeviation;
	TBool iTransaction;
	};

#endif
