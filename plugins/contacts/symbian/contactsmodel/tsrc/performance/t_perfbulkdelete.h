/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __TPERFBULKDELETE__
#define __TPERFBULKDELETE__

#include <e32std.h>
#include <e32test.h>
#include <cntdb.h>
#include <s32strm.h>


class CDeleteMany : public CBase
	{
public:
	~CDeleteMany();
	static CDeleteMany* NewLC(RTest& aTest);
	void DelDataL(TInt aEntryCount, TInt aNumEntriesInDb, TInt aNumMsToGet20ItemsFromView);
		
private:
	CDeleteMany();
	void ConstructL(RTest& aTest);
	static void CleanupPtrArray(TAny* aCArrayPtr);
	void CreateRandomIdArrayL(CContactIdArray& aUids, TInt aNumIds, TInt aNumEntriesInDb);
		
private:

	RTest* iTest; //not owned
	CContactDatabase* iDb;
	};


#endif //__TPERFBULKDELETE__
