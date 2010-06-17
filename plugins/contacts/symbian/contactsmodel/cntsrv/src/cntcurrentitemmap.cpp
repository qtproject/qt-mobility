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


#include "cntcurrentitemmap.h"
#include <s32stor.h>
#include <s32file.h>

const TInt KCntCurrentItemMapGranularity = 3;


CCntServerCurrentItemMap::CCntServerCurrentItemMap() : iCurrentItemMap(KCntCurrentItemMapGranularity)
	{
	}

CCntServerCurrentItemMap::~CCntServerCurrentItemMap()
	{
	iCurrentItemMap.ResetAndDestroy();
	iCurrentItemMap.Close();
	}

TContactItemId CCntServerCurrentItemMap::CurrentItem(const TDesC& aDatabaseFile) const
	{
	TInt index = KErrNotFound;
	if	(EntryAvailable(aDatabaseFile, index))
		return iCurrentItemMap[index]->ContactId();
	return KNullContactId;
	}

TBool CCntServerCurrentItemMap::EntryAvailable(const TDesC& aDatabaseFile, TInt& aIndex) const
	{
	const TInt count = iCurrentItemMap.Count();
	for(aIndex = 0; aIndex < count; aIndex++)
		{
		if (FileNamesIdentical(iCurrentItemMap[aIndex]->DatabaseFile(),aDatabaseFile))
			return ETrue;
		}
	aIndex = KErrNotFound;
	return EFalse;
	}


void CCntServerCurrentItemMap::UpdateEntryL(TInt aIndex, TContactItemId aId)
	{
	__ASSERT_ALWAYS(aIndex >= 0 && aIndex < iCurrentItemMap.Count(), User::Leave(KErrOverflow));
	iCurrentItemMap[aIndex]->SetContactId(aId);
	}

	
void CCntServerCurrentItemMap::AddEntryL(const TDesC& aDatabaseFile, TContactItemId aId)
	{
	CCntCurrentItemMapEntry* entry = new(ELeave) CCntCurrentItemMapEntry();
	CleanupStack::PushL(entry);
	entry->ConstructL(aDatabaseFile, aId);
	User::LeaveIfError(iCurrentItemMap.Append(entry));
	CleanupStack::Pop(); // entry
	}



void CCntServerCurrentItemMap::StoreL(CDictionaryFileStore& aStore) const
	{
	RDictionaryWriteStream stream;
	stream.AssignLC(aStore, KUidCntCurrentItemStream);

	const TInt count = iCurrentItemMap.Count();
	stream.WriteInt32L(count);
	for(TInt i=0; i<count; i++)
		{
		const CCntCurrentItemMapEntry* entry = iCurrentItemMap[i];
		stream << *entry;
		}

	stream.CommitL();
	CleanupStack::PopAndDestroy(); // stream
	}


void CCntServerCurrentItemMap::RestoreL(CDictionaryFileStore& aStore)
	{
	if	(aStore.IsPresentL(KUidCntCurrentItemStream))
		{
		RDictionaryReadStream stream;
		stream.OpenLC(aStore, KUidCntCurrentItemStream);

		const TInt count = stream.ReadInt32L();
		for(TInt i=0; i<count; i++)
			{
			CCntCurrentItemMapEntry* entry = new(ELeave) CCntCurrentItemMapEntry();
			CleanupStack::PushL(entry);
			stream >> *entry;
			User::LeaveIfError(iCurrentItemMap.Append(entry));
			CleanupStack::Pop(); // entry
			}

		CleanupStack::PopAndDestroy(); // stream
		}
	}







CCntServerCurrentItemMap::CCntCurrentItemMapEntry::CCntCurrentItemMapEntry()
	{
	}


CCntServerCurrentItemMap::CCntCurrentItemMapEntry::~CCntCurrentItemMapEntry()
	{
	delete iDatabaseFile;
	}


void CCntServerCurrentItemMap::CCntCurrentItemMapEntry::ConstructL(const TDesC& aFileName, TContactItemId aContactId)
	{
	iDatabaseFile = aFileName.AllocL();
	iContactId = aContactId;
	}


void CCntServerCurrentItemMap::CCntCurrentItemMapEntry::InternalizeL(RReadStream& aStream)
	{
	iDatabaseFile = HBufC::NewL(aStream, KMaxTInt);
	iContactId = aStream.ReadInt32L();
	}


void CCntServerCurrentItemMap::CCntCurrentItemMapEntry::ExternalizeL(RWriteStream& aStream) const
	{
	aStream << *iDatabaseFile;
	aStream.WriteInt32L(iContactId);
	}




