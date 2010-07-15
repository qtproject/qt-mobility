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


#ifndef __CNTSERVERCURRENTITEMMAP_H__
#define __CNTSERVERCURRENTITEMMAP_H__

#include <e32base.h>
#include <cntdef.h>

const TUid KUidCntCurrentItemStream = { 0x10009EF8 };

class CDictionaryFileStore;

/** 
 * A collection of associations between database filenames and
 * current contact item id 
 */
class CCntServerCurrentItemMap : public CBase
	{
public:
	CCntServerCurrentItemMap();
	~CCntServerCurrentItemMap();

	// ACCESS - GETTERS
	TContactItemId CurrentItem(const TDesC& aDatabaseFile) const;
	TBool EntryAvailable(const TDesC& aDatabaseFile, TInt& aIndex) const;
	void UpdateEntryL(TInt aIndex, TContactItemId aId);
	void AddEntryL(const TDesC& aDatabaseFile, TContactItemId aId);

	// INTERNALIZE / EXTERNALIZE
	void StoreL(CDictionaryFileStore& aStore) const;
	void RestoreL(CDictionaryFileStore& aStore);
private:
	/** 
	 * An individual association between a database filename and 
	 * the current item id for that database
	 */
	class CCntCurrentItemMapEntry : public CBase
		{
	public:
		CCntCurrentItemMapEntry();
		~CCntCurrentItemMapEntry();
		void ConstructL(const TDesC& aFileName, TContactItemId aContactId);

		inline const TDesC& DatabaseFile() const;
		inline TContactItemId ContactId() const;
		inline void SetContactId(TContactItemId aContactId);

		void InternalizeL(RReadStream& aStream);
		void ExternalizeL(RWriteStream& aStream) const;
	private:
		HBufC* iDatabaseFile;
		TContactItemId iContactId;
		};
private:
	RPointerArray<CCntCurrentItemMapEntry> iCurrentItemMap;
	};

inline const TDesC& CCntServerCurrentItemMap::CCntCurrentItemMapEntry::DatabaseFile() const { return *iDatabaseFile; }
inline TContactItemId CCntServerCurrentItemMap::CCntCurrentItemMapEntry::ContactId() const { return iContactId; }
inline void CCntServerCurrentItemMap::CCntCurrentItemMapEntry::SetContactId(TContactItemId aContactId) { iContactId = aContactId; }



#endif
