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
#include <s32strm.h>

#ifndef __TPERFSYNCDELETE__
#define __TPERFSYNCDELETE__

class CSyncDelete : public CBase
	{
public:
	static CSyncDelete* NewLC(RTest& aTest,TInt64 aSeed);
	void DoSyncL(TInt aEntryCount, TInt aNumEntriesInDb);
    ~CSyncDelete();
    
private:
	CSyncDelete(TInt64 seed);
	void ConstructL(RTest& aTest);
	TBool ExistsInArray(const CArrayFix<TInt>& aUids, TInt aSize, TInt aNum);
	void DeleteItemL(TInt aUid);
	static void CleanupPtrArray(TAny* aCArrayPtr);
	void CreateRandomIdArray(CArrayFix<TInt>& aUids, TInt aNumIds, TInt aNumEntriesInDb);

private:
	RTest* iTest;
	TTime iCreated;
	CContactDatabase* iDb;
	TInt64 iRandSeed;	
	RFs iFs;
	};
	
#endif // __TPERFSYNCDELETE__
