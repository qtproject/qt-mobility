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


#ifndef __CNTSERVERSPEEDDIALS_H__
#define __CNTSERVERSPEEDDIALS_H__

#include <e32base.h>
#include <cntdef.h>

const TUid KUidCntSpeedDialStream = { 0x10009EF7 };

class CDictionaryFileStore;
class CCntServerSpeedDialTable;

/** Manages a collection of speed-dial tables. */
class CCntServerSpeedDialManager : public CBase
	{
public:
	CCntServerSpeedDialManager();
	~CCntServerSpeedDialManager();
	CCntServerSpeedDialTable& TableL(const TDesC& aDatabase);
	TBool DeleteTable(const TDesC& aDatabase);

	void StoreL(CDictionaryFileStore& aStore) const;
	void RestoreL(CDictionaryFileStore& aStore);
	static TUid SpeedDialFieldUidFromSpeedDialPosition(TInt aSpeedDialPosition);
private: 
	RPointerArray<CCntServerSpeedDialTable> iSpeedDials;
	};


/** A speed-dial table. */
class CCntServerSpeedDialTable : public CBase
	{
public:
	static CCntServerSpeedDialTable* NewL(const TDesC& aDatabase);
	~CCntServerSpeedDialTable();
	inline const TDesC& Database() const;
	TContactItemId SpeedDialContactItem(TInt aIndex, TDes& aPhoneNumber) const;
	CArrayFix<TInt>* SpeedDialIndicesForContactIdLC(TContactItemId aContactId);
	void SetSpeedDialL(TInt aIndex, TContactItemId aContactId, const TSpeedDialPhoneNumber& aPhoneNumber);
		
	void InternalizeL(RReadStream& aStream);
	void ExternalizeL(RWriteStream& aStream) const;
private:
	CCntServerSpeedDialTable();
	void ConstructL(const TDesC& aDatabase);
private: 
	/** An individual speed-dial entry. */
	class CSpeedDial : public CBase
		{
	public:
		CSpeedDial();
		static CSpeedDial* NewL(TContactItemId aContactId, const TSpeedDialPhoneNumber& aPhoneNumber);
		~CSpeedDial();
		inline TContactItemId ContactId() const;
		void PhoneNumber(TSpeedDialPhoneNumber& aSpeedNumber) const;
		void InternalizeL(RReadStream& aStream);
		void ExternalizeL(RWriteStream& aStream) const;
		void Reset();
		void SetL(TContactItemId aContactId, const TSpeedDialPhoneNumber& aPhoneNumber);
	private: 
		CSpeedDial(TContactItemId aContactId, const TSpeedDialPhoneNumber& aPhoneNumber);
		void ConstructL();
	private: 
		TContactItemId iContactId;
		HBufC* iPhoneNumber;
		};
private:
	TFixedArray<CSpeedDial,KCntMaxSpeedDialIndex> iSpeedDialTable;
	HBufC* iDatabaseFile;
	};


inline const TDesC& CCntServerSpeedDialTable::Database() const { return *iDatabaseFile; }
inline TContactItemId CCntServerSpeedDialTable::CSpeedDial::ContactId() const { return iContactId; }



#endif
