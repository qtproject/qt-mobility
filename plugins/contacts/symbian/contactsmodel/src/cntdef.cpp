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


#include <cntdef.h>
#include "cntstd.h"

//
// class CContactIdArray
//

EXPORT_C CContactIdArray* CContactIdArray::NewL()
/** Allocates and constructs a new contact ID array. 

@return Pointer to the newly created contact ID array. */
	{ // static 
	CContactIdArray* self=CContactIdArray::NewLC();
	CleanupStack::Pop(); // self
	return self;
	}

EXPORT_C CContactIdArray* CContactIdArray::NewLC()
/** Allocates and constructs a new contact ID array. 

The pointer to the object is left on the cleanup stack.

@return Pointer to the newly created contact ID array. */
	{ // static 
	CContactIdArray* self=new(ELeave) CContactIdArray;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

EXPORT_C CContactIdArray* CContactIdArray::NewL(const CContactIdArray* aArray)
/** Allocates and constructs a new contact ID array and then copies the contents of 
aArray into it. 

@param aArray Pointer to the contact ID array to copy.
@return Pointer to the newly created contact ID array. */
	{
	CContactIdArray* self=CContactIdArray::NewLC(aArray);
	CleanupStack::Pop(); // self
	return self;
	}

EXPORT_C CContactIdArray* CContactIdArray::NewLC(const CContactIdArray* aArray)
/** Allocates and constructs a new contact ID array and then copies the contents of 
aArray into it. 

The pointer to the object is left on the cleanup stack.

@param aArray Pointer to the contact ID array to copy.
@return Pointer to the newly created contact ID array. */
	{
	CContactIdArray* self=CContactIdArray::NewLC();
	self->CloneL(aArray);
	return self;
	}
	
/** Allocates and constructs a new contact ID array based on a RReadStream. 

The pointer to the object is left on the cleanup stack.

@param aStream RReadStream containing object to internalize.
@return Pointer to the newly created contact ID array 
@internalTechnology 
*/
CContactIdArray* CContactIdArray::NewLC(RReadStream& aStream)
	{
	CContactIdArray* self=CContactIdArray::NewLC();
	self->InternalizeL(aStream);	
	return self;
	}	

void CContactIdArray::CloneL(const CContactIdArray* aArray)
	{
	const TInt arrayCount = aArray->Count();
	for(TInt loop=0;loop<arrayCount;loop++)
		AddL((*aArray)[loop]);
	}

CContactIdArray::CContactIdArray()
	{}

void CContactIdArray::ConstructL()
	{
	iIds=new(ELeave) CArrayFixFlat<TContactItemId>(4);
	}

EXPORT_C CContactIdArray::~CContactIdArray()
/** The destructor frees all resources owned by the array, prior to its destruction. */
	{
	delete iIds;
	}

EXPORT_C TInt CContactIdArray::Find(TContactItemId aId) const
/** Finds the index of the specified contact ID within the array.

@param aId The contact ID to find.
@return If the contact ID is found, its index within the array. KErrNotFound 
if the ID is not found. */
	{
	TInt pos;
	TKeyArrayFix key(0,ECmpTInt);
	if (iIds->Find(aId,key,pos)==KErrNone)
		return pos;
	return KErrNotFound;
	}

EXPORT_C void CContactIdArray::AddL(TContactItemId aId)
/** Appends a contact ID to the array.

@param aId The contact ID to append to the array. */
	{
//	__ASSERT_DEBUG(Find(aId)<=KErrNotFound,Panic(ECntPanicDuplicateIds));
	iIds->AppendL(aId);
	}


EXPORT_C void CContactIdArray::MoveL(TInt aOldIndex,TInt aNewIndex)
/** Moves a contact ID within the array.

Note: both indexes must be valid or a panic occurs.

@param aOldIndex The index of the ID to move.
@param aNewIndex The new index for the contact ID. */
	{
	/*
	This function has unexpected behaviour. From the function prototype
	one would expect this to provide simple swap functionality. However,
	in fact, this function performs a slightly more complicated task: 
	it actually deletes the item at aOldIndex and moves it *after* the 
	item at aNewIndex. This behaviour is subtley different from a 'swap'
	(see the example at the end of this comment).
	
	This means that the only time this kind of code could be used:-

	const TContactItemId id = (*iIds)[aOldIndex];
	(*iIds)[aOldIndex] = (*iIds)[aNewIndex];
	(*iIds)[aNewIndex] = id;

	is when Abs(aNewIndex - aOldIndex) = 1, which is rare in the simple
	testing that has been performed.

	An example of the function pre and post execution is below

	CContactIdArray::MoveL(aOld = 2, aNew = 7)
	CContactIdArray::MoveL - Before operation...
	iIds[0] = 5
	iIds[1] = 6
	iIds[2] = 7
	iIds[3] = 4
	iIds[4] = 1
	iIds[5] = 2
	iIds[6] = 3
	iIds[7] = 8

	CContactIdArray::MoveL - After operation...
	iIds[0] = 5
	iIds[1] = 6
	iIds[2] = 4
	iIds[3] = 1
	iIds[4] = 2
	iIds[5] = 3
	iIds[6] = 8
	iIds[7] = 7
	*/

	if	(aOldIndex==aNewIndex)
		return;

	if	(aNewIndex>aOldIndex && aNewIndex<Count())
		++aNewIndex;
	TContactItemId id=(*iIds)[aOldIndex];
	iIds->InsertL(aNewIndex,id);
	if	(aOldIndex>aNewIndex)
		++aOldIndex;
	Remove(aOldIndex);
	}

EXPORT_C void CContactIdArray::ReverseOrder()
/** Reverses the order of the whole array. */
	{
	TInt count=Count();
	TInt end=count/2;
	TInt index2=count-1;
	for(TInt index=0;index<end;index++)
		{
		TContactItemId id=(*iIds)[index];
		(*iIds)[index]=(*iIds)[index2];
		(*iIds)[index2]=id;
		index2--;
		}
	}

EXPORT_C void CContactIdArray::InternalizeL(RReadStream& aStream)
/** Internalises a CContactIdArray object from a read stream. The presence of this 
function means that the standard templated operator>>() (defined in s32strm.h) 
is available to internalise objects of this class.

@param aStream Stream from which the object should be internalised. */
	{
	TContactItemId anitem;
	TInt count;
	count=aStream.ReadInt32L();
	for (TInt i=0;i<count;i++)
		{
		aStream >> anitem;
		iIds->AppendL(anitem);
		}
	}

EXPORT_C void CContactIdArray::ExternalizeL(RWriteStream& aStream) const 
/** Externalises a CContactIdArray object to a write stream. The presence of this 
function means that the standard templated operator<<() (defined in s32strm.h) 
is available to externalise objects of this class. 

@param aStream Stream to which the object should be externalised. */
	{
	TInt count;
	TContactItemId anitem;
	count=iIds->Count();
	aStream.WriteInt32L(count);
	for (TInt i=0;i<count;i++)
		{
		anitem=(*iIds)[i];
		aStream << anitem;
		}
	}
	
/* Sorts the array into ascending order.  
This function is intended for internal use only.*/	
void CContactIdArray::Sort()
	{
	TKeyArrayFix key(0,ECmpTInt);
	iIds->Sort(key);     
	}	
