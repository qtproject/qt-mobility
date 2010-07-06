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


#include "cntspeeddials.h"
#include <s32stor.h>
#include <s32file.h>
#include <collate.h>


const TInt KCntSpeedDialGranularity = 1;


CCntServerSpeedDialManager::CCntServerSpeedDialManager() : iSpeedDials(KCntSpeedDialGranularity)
	{
	}

CCntServerSpeedDialManager::~CCntServerSpeedDialManager()
	{
	iSpeedDials.ResetAndDestroy();
	iSpeedDials.Close();
	}

// returns the speeddial table for the database or if it doesn't exist creates a table
CCntServerSpeedDialTable& CCntServerSpeedDialManager::TableL(const TDesC& aDatabase)
	{
	TInt count = iSpeedDials.Count();
	TInt index = KErrNotFound;
	for (TInt i=0; i<count; i++)
		{
		CCntServerSpeedDialTable* table = iSpeedDials[i]; 
		if (FileNamesIdentical(aDatabase,table->Database()))
			{
			index = i;
			break;
			}
		}
	if (index==KErrNotFound) // the table doesn't exist yet
		{
		CCntServerSpeedDialTable* newTable = CCntServerSpeedDialTable::NewL(aDatabase);
		CleanupStack::PushL(newTable);
		User::LeaveIfError(iSpeedDials.Append(newTable));
		CleanupStack::Pop(newTable);
		index = count;
		}
	return *(iSpeedDials[index]);
	}

TBool CCntServerSpeedDialManager::DeleteTable(const TDesC& aDatabase)
	{
	const TInt count = iSpeedDials.Count();
	for (TInt i = 0; i < count; i++)
		{
		CCntServerSpeedDialTable* table = iSpeedDials[i]; 
		if (FileNamesIdentical(aDatabase,table->Database()))
			{
			iSpeedDials.Remove(i);
			delete table;
			// table has been deleted, cntmodel.ini file needs saving
			return ETrue;
			}
		}

	// no change
	return EFalse;
	}
void CCntServerSpeedDialManager::StoreL(CDictionaryFileStore& aStore) const
	{
	const TInt count = iSpeedDials.Count();
	if (count)
		{
		RDictionaryWriteStream stream;
		stream.AssignLC(aStore, KUidCntSpeedDialStream);
		stream.WriteInt32L(iSpeedDials.Count()); 
		for(TInt i=0; i<count; i++)
			{
			stream << *iSpeedDials[i];
			}
		stream.CommitL();
		CleanupStack::PopAndDestroy(); // stream
		}
	}

void CCntServerSpeedDialManager::RestoreL(CDictionaryFileStore& aStore)
	{
	if	(aStore.IsPresentL(KUidCntSpeedDialStream))
		{
		RDictionaryReadStream stream;
		stream.OpenLC(aStore, KUidCntSpeedDialStream);
		TInt count = stream.ReadInt32L();
		if (count <0)
			{
			User::LeaveIfError(KErrCorrupt);
			}
		iSpeedDials.ResetAndDestroy();
		TInt error(KErrNone) ;
		for(TInt i=0; i<count && (error == KErrNone); i++)
			{
			CCntServerSpeedDialTable* table = CCntServerSpeedDialTable::NewL(KNullDesC);
			CleanupStack::PushL(table);
			stream >> *table;
			error = iSpeedDials.Append(table);
			CleanupStack::Pop(table);
			}
		User::LeaveIfError(error);
		CleanupStack::PopAndDestroy(); // stream
		}
	}
	

CCntServerSpeedDialTable::CSpeedDial::CSpeedDial()
	{ 
	Reset();
	}
	
CCntServerSpeedDialTable::CSpeedDial::CSpeedDial(TContactItemId aContactId, const TSpeedDialPhoneNumber& aPhoneNumber) 
: iContactId(aContactId)
	{
	TPtr ptrNum = iPhoneNumber->Des();
	ptrNum = aPhoneNumber;
	}


CCntServerSpeedDialTable::CSpeedDial* CCntServerSpeedDialTable::CSpeedDial::NewL(TContactItemId aContactId, const TSpeedDialPhoneNumber& aPhoneNumber) 
	{
	CCntServerSpeedDialTable::CSpeedDial* self = new (ELeave) CSpeedDial(aContactId, aPhoneNumber);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}


void CCntServerSpeedDialTable::CSpeedDial::ConstructL()
	{
	iPhoneNumber = HBufC::NewL(KSpeedDialPhoneLength);
	}


void CCntServerSpeedDialTable::CSpeedDial::PhoneNumber(TSpeedDialPhoneNumber& aSpeedNumber) const 
	{
	if (iPhoneNumber == NULL)
		{
		aSpeedNumber.Zero();
		return;
		}
	TPtrC ptrNum = iPhoneNumber->Des();
	aSpeedNumber = ptrNum; 
	}

	
CCntServerSpeedDialTable::CSpeedDial::~CSpeedDial()
	{
	if (iPhoneNumber != NULL)
		{
		delete iPhoneNumber;
		}
	}

void CCntServerSpeedDialTable::CSpeedDial::InternalizeL(RReadStream& aStream)
	{
	iContactId = aStream.ReadInt32L();
	if (iPhoneNumber != NULL)
		{
		delete iPhoneNumber;
		iPhoneNumber = NULL;
		}
	iPhoneNumber = HBufC::NewL(aStream, KSpeedDialPhoneLength);
	}

void CCntServerSpeedDialTable::CSpeedDial::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteInt32L(iContactId);
	if (iPhoneNumber != NULL)
		{
		aStream << *iPhoneNumber;
		}
	else
		{
		aStream << KNullDesC;
		}
	}

void CCntServerSpeedDialTable::CSpeedDial::Reset()
	{
	iContactId = KNullContactId;
	if (iPhoneNumber != NULL)
		{
		delete iPhoneNumber;
		iPhoneNumber = NULL;
		}
	}

void CCntServerSpeedDialTable::CSpeedDial::SetL(TContactItemId aContactId, const TSpeedDialPhoneNumber& aPhoneNumber)
	{
	iContactId = aContactId;
	if (iPhoneNumber != NULL)
		{
		delete iPhoneNumber;
		iPhoneNumber = NULL;
		}
	iPhoneNumber = HBufC::NewL(KSpeedDialPhoneLength);
	TPtr phoneNum = iPhoneNumber->Des();
	phoneNum = aPhoneNumber;
	}

CCntServerSpeedDialTable* CCntServerSpeedDialTable::NewL(const TDesC& aDatabase)
	{
	CCntServerSpeedDialTable* self = new (ELeave) CCntServerSpeedDialTable();
	CleanupStack::PushL(self);
	self->ConstructL(aDatabase);
	CleanupStack::Pop(self);
	return self;
	}

CCntServerSpeedDialTable::~CCntServerSpeedDialTable()
	{
	delete iDatabaseFile;
	}

CCntServerSpeedDialTable::CCntServerSpeedDialTable()
	{
	}

void CCntServerSpeedDialTable::ConstructL(const TDesC& aDatabase)
	{
	iDatabaseFile = aDatabase.AllocL();
	}

TContactItemId CCntServerSpeedDialTable::SpeedDialContactItem(TInt aIndex, TDes& aPhoneNumber) const
	{
//	__ASSERT_ALWAYS(aIndex >= KCntMinSpeedDialIndex && aIndex <= KCntMaxSpeedDialIndex, User::Panic(KErrArgument));
	const CSpeedDial& item = iSpeedDialTable.At(aIndex - 1);
	TSpeedDialPhoneNumber speedNumber;
	item.PhoneNumber(speedNumber);
	aPhoneNumber = speedNumber;
	return item.ContactId();
	}

CArrayFix<TInt>* CCntServerSpeedDialTable::SpeedDialIndicesForContactIdLC(TContactItemId aContactId)
	{
	CArrayFixFlat<TInt>* indices = new(ELeave) CArrayFixFlat<TInt>(KCntMaxSpeedDialIndex);
	CleanupStack::PushL(indices);

	const TInt count = iSpeedDialTable.Count();
	for(TInt i=0; i<count; i++)
		{
		const CSpeedDial& item = iSpeedDialTable.At(i);
		if	(item.ContactId() == aContactId)
			indices->AppendL(i+1); // +1 to map onto the contacts speed dial indices of 1-9
		}
	return indices;
	}

void CCntServerSpeedDialTable::SetSpeedDialL(TInt aIndex, TContactItemId aContactId, const TSpeedDialPhoneNumber& aPhoneNumber)
	{
//	__ASSERT_ALWAYS(aIndex >= KCntMinSpeedDialIndex && aIndex <= KCntMaxSpeedDialIndex, CntServerUtils::Panic(ECntServerPanicSpeedDialIndexOutOfRange));
	CSpeedDial& item = iSpeedDialTable.At(aIndex - 1);
	item.SetL(aContactId, aPhoneNumber);	
	}

void CCntServerSpeedDialTable::InternalizeL(RReadStream& aStream)
	{
	HBufC* file = HBufC::NewLC(aStream, KMaxTInt);
	delete iDatabaseFile;
	iDatabaseFile = file;
	CleanupStack::Pop(file); 
	const TInt count = iSpeedDialTable.Count();
	for(TInt i=0; i<count; i++)
		{
		CSpeedDial& tableItem = iSpeedDialTable.At(i);
		tableItem.InternalizeL(aStream);
		}
	}

void CCntServerSpeedDialTable::ExternalizeL(RWriteStream& aStream) const
	{
	aStream << *iDatabaseFile;
	const TInt count = iSpeedDialTable.Count();
	for(TInt i=0; i<count; i++)
		{
		aStream << iSpeedDialTable.At(i);
		}
	}

TUid CCntServerSpeedDialManager::SpeedDialFieldUidFromSpeedDialPosition(TInt aSpeedDialPosition)
//
//	Static utility method to map a speed dial index onto a field uid.
//
	{
//	__ASSERT_ALWAYS(aSpeedDialPosition >= KCntMinSpeedDialIndex && aSpeedDialPosition <= KCntMaxSpeedDialIndex, Panic(ECntPanicInvalidSpeedDialIndex));

	TUid fieldTypeUid = KNullUid;
	switch (aSpeedDialPosition)
		{
	case 1:
		fieldTypeUid = KUidSpeedDialOne;
		break;
	case 2:
		fieldTypeUid = KUidSpeedDialTwo;
		break;
	case 3:
		fieldTypeUid = KUidSpeedDialThree;
		break;
	case 4:
		fieldTypeUid = KUidSpeedDialFour;
		break;
	case 5:
		fieldTypeUid = KUidSpeedDialFive;
		break;
	case 6:
		fieldTypeUid = KUidSpeedDialSix;
		break;
	case 7:
		fieldTypeUid = KUidSpeedDialSeven;
		break;
	case 8:
		fieldTypeUid = KUidSpeedDialEight;
		break;
	case 9:
		fieldTypeUid = KUidSpeedDialNine;
		break;
		}

	return fieldTypeUid; 
	}

