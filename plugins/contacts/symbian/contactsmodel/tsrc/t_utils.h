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


#ifndef __T_UTILS_H__
#define __T_UTILS_H__

#include <cntdb.h>
#include <cntitem.h>
#include <cntfield.h>
#include <cntfldst.h>
#include <s32mem.h>
#include <hal.h>

class RTest;

enum TVCardStoreType
	{
	KVCardStoreTypeBuf,
	KVCardStoreTypeFile,
	};

class CVCardTestStore : public CBase
	{
public:
	IMPORT_C ~CVCardTestStore();
	IMPORT_C static CVCardTestStore* NewLC(TVCardStoreType aType, RFs *aFs, TInt aFileNum);
	IMPORT_C static CVCardTestStore* NewL(TVCardStoreType aType, RFs *aFs, TInt aFileNum);
	inline void SetStreamId(TStreamId aStreamId) {iStreamId=aStreamId;};
	inline TStreamId StreamId() const {return(iStreamId);};
	IMPORT_C CStreamStore *Store() const;
#ifdef __EABI__
       /** Cause vtable & typeinfo to be exported */
       CVCardTestStore() {}
#endif

private:
	IMPORT_C void ConstructL(TVCardStoreType aType, RFs *aFs, TInt aFileNum);
private:
	TStreamId iStreamId;
	TVCardStoreType iType;
	CBufStore *iBufStore;
	CFileStore *iFileStore;
	};
	
	
/** Holds results from performance profiling.
Same data members as deprecated TProfile class in e32debug.h */
class TCntProfile
    {
public:
    TInt iTime;
    TInt iCount;
    };


class CCntTest : public CBase
	{
public:
	IMPORT_C void ConstructL(RTest &aTest, const TDesC &aDatabaseName);
	IMPORT_C void EndTestLib(TInt aErr);
	inline RFs &Fs() {return(iFs);};
	CContactDatabase* Db() {return(iDb);};
	IMPORT_C CContactDatabase* CreateDatabaseL();
	IMPORT_C CContactDatabase* OpenDatabaseL();
	IMPORT_C void SelectDriveL();
	IMPORT_C void CloseDatabase();
	IMPORT_C void DeleteDatabaseL();
	IMPORT_C void DeleteAllTemplateFieldsL();
	IMPORT_C void TestField(CContactItemField &aField,TStorageType aType,TFieldType aFieldType,TUid aMapping);
	IMPORT_C void AdjustContactAccessCountL(TContactItemId aItemId, TInt aCount);
	IMPORT_C void TestAccessCount(TContactItemId aItemId, TInt aCount);
	IMPORT_C TBool CompareFields(CContactItemField &aField1, CContactItemField &aField2);
	IMPORT_C void CheckDeletedContact(TContactItemId aDeleteId);
	IMPORT_C void CheckContactDoesNotExist(TContactItemId aNotExistId);
	IMPORT_C void DeleteContact(TContactItemId aDeleteMe);
	IMPORT_C HBufC *ContactUidLC(TContactItemId aId, TInt64 aMachineId);
	IMPORT_C CVCardTestStore* ExportContactsLC(const CContactIdArray *aIds, TUint aFormatFlags, TVCardStoreType aType=KVCardStoreTypeBuf, TInt aFileNum=0);
	IMPORT_C CVCardTestStore* ExportContactLC(TContactItemId aId, TUint aFormatFlags, TVCardStoreType aType=KVCardStoreTypeBuf, TInt aFileNum=0);
	IMPORT_C void ImportContacts1by1L(CVCardTestStore* aStore, TUint aFormatFlags);
	IMPORT_C CArrayPtr<CContactItem>* ImportContactsL(CVCardTestStore* aStore, TUint aFormatFlags);
	IMPORT_C CArrayPtr<CContactItem>* ImportContactsLC(CVCardTestStore* aStore, TUint aFormatFlags);
	IMPORT_C void PrintfNewline() const;
	IMPORT_C TBool LockServerSessionExists(TBool aPause=EFalse) const;
	IMPORT_C TInt LockServerProcessCount(TBool aPause=EFalse, TBool aDisplayCount=EFalse, TBool aDisplayList=EFalse) const;
	IMPORT_C void TestLockServerExists() const;
	IMPORT_C void TestNoLockServer() const;
	IMPORT_C void TestLockServerCloses() const;
	inline const TFileName& DatabaseName() const {return(iDatabaseName);};
	/**	Profiling APIs */
    IMPORT_C static TInt ProfileResult(TCntProfile* aProfile,TInt aStart,TInt aCount);
    IMPORT_C static TInt ProfileReset(TInt aStart,TInt aCount);
    IMPORT_C static TInt ProfileStart(TInt aProfile);
    IMPORT_C static TInt ProfileEnd(TInt aProfile);

#ifdef __EABI__
       /** Cause vtable & typeinfo to be exported */
    IMPORT_C CCntTest() {}
#endif
private:

	IMPORT_C ~CCntTest();
private:
	CContactDatabase* iDb;
	TFileName iDatabaseName;
	RFs iFs;
    CTrapCleanup* iCleanup;
	RTest *iTest;
	TBool iIsTopLevel;

	};

struct TCntProfileTls
	{
	TTime iTime;
	TCntProfile iProfile;
	};


IMPORT_C TPtrC FieldText(CContactItem& aItem,TUid aType);
IMPORT_C void SetNameL(CContactItem& aItem,TUid aFieldType,TUid aMapping,const TDesC& aName, TBool aAddEntry);
IMPORT_C void AddFieldL(CContactItemFieldSet* aFieldSet, TStorageType aType,TFieldType aFieldType,TUid aMapping);
IMPORT_C void AddFieldL(CContactItem* aItem, TStorageType aType,TFieldType aFieldType,TUid aMapping);
IMPORT_C void InsertFieldL(CContactItem& aItem, TInt aPos, TStorageType aType,TFieldType aFieldType,TUid aMapping);
IMPORT_C void InsertFieldL(CContactItem& aItem, TInt aPos, TStorageType aType,TFieldType aFieldType,TUid aMapping, const TDesC& aText);
IMPORT_C TContactItemId AddContactL(CContactDatabase* aDb, TFieldType aFieldType,TUid aMapping, const TDesC &aText);
IMPORT_C void SetUserFlags(CContactItemFieldSet& aFieldSet, TUint aFlags, TInt aIndex);
IMPORT_C void SetUserFlags(CContactItemFieldSet& aFieldSet, TUint aFlags);
IMPORT_C void SetUserFlags(CContactItem* aItem, TUint aFlags, TInt aIndex);
IMPORT_C void SetUserFlags(CContactItem* aItem, TUint aFlags);
IMPORT_C CVCardTestStore* ExportContactsL(CContactDatabase* aDb, const CContactIdArray *aIds, TUint aFormatFlags, TVCardStoreType aType, RFs *aFs, TInt aFileNum);
IMPORT_C CArrayPtr<CContactItem>* ImportContactsL(CContactDatabase* aDb, CVCardTestStore* aStore, TUint aFormatFlags);
IMPORT_C void CleanUpResetAndDestroy(TAny *aArray);
IMPORT_C TBool CompareItemField(const CContactItemField &aItemField1, const CContactItemField &aItemField2, TBool aCheckStorage);
IMPORT_C TBool CompareItemFields(CContactItem *aItem1, CContactItem *aItem2, TBool aCheckStorage=ETrue, TBool aTemplateLabelRemoved=EFalse);
IMPORT_C TBool TestGroupStateL(CContactDatabase* aDb, TInt aGroupCount, TInt aGroupEntryCount);
IMPORT_C HBufC* GetFullDbName(const TDesC& aDbName);
IMPORT_C TBool DbShouldBeInDamagedState(CContactDatabase* aDb); 
IMPORT_C CContactItem* ReadContactFromDamagedDbLC(CContactDatabase* aDb, TContactItemId aContactId);

/** A simple timer primarily for use in the T_PERFORMANCE test.
    Used as an alternative for the CntProfile timing stuff.
    Uses User::NTickCount as opposed to TTime.
*/
class TCntPerfTimer
	{
public:
	IMPORT_C TCntPerfTimer();
	IMPORT_C void StartTimer();
	IMPORT_C void StopTimer();
	IMPORT_C void ResetTimer();
	IMPORT_C TUint Result();
private:
	TUint iTickCount1;
	TUint iTickCount2;
	};

#endif
