/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32std.h>
#include <e32test.h>
#include <cntdb.h>

#ifndef __TADDBENCH_H__
#define __TADDBENCH_H__

// This class is designed to imitate the calls on the Contacts Model made by 
// CSmlContactsDba::ImportContactL v2.
// Specifically it mimics the SyncML Add operation to import new contacts to the 
// Contacts model. 
class CAddBench : public CBase
	{
public:
	~CAddBench();
	static CAddBench* NewLC(RTest& aTest);
	void ImportFileL(const TDesC& aFilename, TInt aNumEntries);
	void DoSyncL(TInt aEntryCount, const TDesC& aVCardLoc );
private:
	CAddBench();
	static void CleanupPtrArray(TAny* aCArrayPtr);
	void ConstructL(RTest& aTest);
	void CreateEmptyDBL();
	
private:
	RFs iFs;
	RTest* iTest; //not owned
	CContactDatabase* iDb;
	TTime iTime; // for SML mimic calls
	TInt iNumContacts;
	};



#endif // __TADDBENCH_H__
