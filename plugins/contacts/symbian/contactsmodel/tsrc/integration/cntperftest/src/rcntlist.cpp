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
* This iterator is intended as a replacement for tcontactiter
* only to be used by tests. It is not update automatically
* and allows explicit changes to its contents.
* When a contact is added to the db, its id should be added to the iterator
* and similarly when it is deleted.
* It is possible to access the current item pointed to by the iterator, in addition
* to the next and previous items.
*
*/


/**
 @file
 @publishedAll
 @released
*/

#include "rcntlist.h"

RCntList::RCntList():iDatabase(), iPos(-1), iLength(0)
	{
	}

void RCntList::CopyL(const RCntList &aList)
	{
	iDatabase.Reset();
	TInt i = 0;
	for(; i < aList.iLength; ++i)
		{
		iDatabase.AppendL(aList.iDatabase[i]);
		}
	iPos = aList.iPos;
	iLength = aList.iLength;
	}

RCntList::~RCntList()
	{
	Close();
	}

void RCntList::AddL(const TContactItemId &aItem)
	{
	iDatabase.AppendL(aItem);
	++iLength;
	}

void RCntList::RemoveL(const TInt &aItem)
	{
	__ASSERT_ALWAYS(aItem >= 0 && aItem < iLength, User::Leave(KNullContactId));
	iDatabase.Remove(aItem);
	--iLength;
	}

void RCntList::RemoveL(const TContactItemId &aItem)
	{
	const TInt pos = iDatabase.Find(aItem);
	__ASSERT_ALWAYS(pos >= 0 && pos < iLength, User::Leave(KNullContactId));
	iDatabase.Remove(pos);
	--iLength;
	}
	
TContactItemId RCntList::NextL()
	{
	__ASSERT_ALWAYS(iLength > 0,User::Leave(KNullContactId));
	if( ++iPos >= iLength )
		{
		iPos = 0;
		}
	return iDatabase[iPos];
	}
	
TContactItemId RCntList::PreviousL()
	{
	__ASSERT_ALWAYS(iLength > 0,User::Leave(KNullContactId));
	if( --iPos < 0 )
		{
		iPos = iLength - 1;
		}
	return iDatabase[iPos];
	}
	
TContactItemId RCntList::CurrentL()
	{
	__ASSERT_ALWAYS(iLength > 0,User::Leave(KNullContactId));
	if( iPos >= iLength)
		{
		iPos = 0;
		}
	else if( iPos < 0 )
		{
		iPos = iLength - 1;
		}
	return iDatabase[iPos];
	}

TInt RCntList::Count()
	{
	return iDatabase.Count();
	}

void RCntList::Reset()
	{
	iPos = -1;
	}


void RCntList::Clear()
	{
	iPos = -1;
	iLength = 0;
	iDatabase.Reset();
	}

void RCntList::Close()
	{
	iLength = 0;
	iPos = -1;
	iDatabase.Close();
	}
