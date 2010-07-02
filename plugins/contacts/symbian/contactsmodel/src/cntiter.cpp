/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <cntdb.h>

EXPORT_C TContactIter::TContactIter(CContactDatabase &aDatabase) : iDatabase(aDatabase)
/** Constructs the object with a contact database.

@param aDatabase The contact database over which to iterate. */
	{
	Reset();
	}

EXPORT_C void TContactIter::Reset()
/** Resets the iterator to its initialised state, so that a subsequent call to 
NextL() goes to the first item. */
	{
	iCursorId=KNullContactId;
	}

EXPORT_C TContactItemId TContactIter::FirstL()
/** Moves to the first contact item in the database.

@return The ID of the first contact item. */
	{
	if (iDatabase.SortedItemsL()->Count()==0)
		iCursorId=KNullContactId;
	else
		GotoIndexL(0);
	return iCursorId;
	}

EXPORT_C TContactItemId TContactIter::NextL()
/** Moves to the next contact item in the database. 

On a newly initialised TContactIter, this function moves to the first item.

@return The ID of the next contact item. Has a value of KNullContactId if 
there are no more items. */
	{
	TInt pos=0;
	if (iCursorId!=KNullContactId)
		pos=iDatabase.ContactPosL(iCursorId)+1;
	if (pos==iDatabase.SortedItemsL()->Count())
		iCursorId=KNullContactId;
	else
		GotoIndexL(pos);
	return iCursorId;
	}

EXPORT_C TContactItemId TContactIter::PreviousL()
/** Moves to the previous contact item in the database.

Note: you must not call this function on a newly initialised database, otherwise 
the function raises a panic.

@return The ID of the previous contact item. Has a value of KNullContactId 
if there is no previous item. */
	{
	TInt pos=iDatabase.ContactPosL(iCursorId);
	if (pos==0)
		return KNullContactId;
	GotoIndexL(--pos);
	return iCursorId;
	}

EXPORT_C TContactItemId TContactIter::LastL()
/** Moves to the last contact item in the database.

@return The ID of the last contact item. */
	{
	GotoIndexL(iDatabase.SortedItemsL()->Count()-1);
	return iCursorId;
	}

EXPORT_C void TContactIter::GotoL(TContactItemId aContactId)
/** Moves to the specified contact item.

@param aContactId A contact item ID. Must not have a value of KNullContactId, 
or the function raises a panic. */
	{
	User::LeaveIfError(iDatabase.DoGotoL(aContactId));
	iCursorId=aContactId;
	}

void TContactIter::GotoIndexL(TInt aPos)
	{
	GotoL((*iDatabase.SortedItemsL())[aPos]);
	}
