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
#ifndef __RCNT_LIST_H__
#define __RCNT_LIST_H__

#include <cntdef.h>

class RCntList
	{
public:
	 RCntList();
	 void CopyL(const RCntList &aList);
	 ~RCntList();
	 void AddL(const TContactItemId &aItem);
	 TContactItemId NextL();
	 TContactItemId PreviousL();
	 void Reset();
	 void Close();
	 void RemoveL(const TInt &aItem);
	 void RemoveL(const TContactItemId &aItem);
	 TContactItemId CurrentL();
	 void Clear();
	 TInt Count();
	 
private:
	RArray<TContactItemId> iDatabase;
	TInt iPos;
	TInt iLength;
	};

#endif
