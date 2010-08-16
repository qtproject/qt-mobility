/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent
 @released
*/


#ifndef RCNTMODEL_H
#define RCNTMODEL_H

#include <cntdb.h>
#include "ccntipccodes.h"

#include <e32base.h>
#include <f32file.h>
#include <badesca.h>


class CContactTemplate;
class CViewContact;
class CCntDbNotifyMonitor;
class CCntPackager;
class MContactDbObserver;
class CContactDefaultViewDefinition;
class CContactTextDef;

const TInt KUidBufferSize = 400;

/**
Client-side Contacts Model session handle.
*/
class RCntModel : public RSessionBase
	{
public:
	RCntModel();

	void ConnectL();
	void Close();

	TInt OpenDatabase(const TDesC& aCntFile = KNullDesC) const;
	void OpenDatabaseAsyncL(TRequestStatus& aStatus ,const TDesC& aCntFile = KNullDesC);
	void CancelAsyncOpen();

	void OpenTablesL();
	void CloseTablesL();

	void CloseDatabase() const;
	TInt ReplaceDatabase(const TDesC& aCntFile = KNullDesC) const;
	TInt CreateDatabase(const TDesC& aCntFile = KNullDesC) const;
	TInt DeleteDatabase(const TDesC& aCntFile = KNullDesC) const;
	
	TInt DefaultDatabase(TDes& aCntFile) const;
	TInt DatabaseDrive(TDriveUnit& aDriveUnit) const;
	void SetDatabaseDriveL(TDriveUnit aDriveUnit, TBool aCopy = ETrue);
	TBool DatabaseExistsL(const TDesC& aCntFile = KNullDesC) const;
	CDesCArray* ListDatabasesL(TDriveUnit* aDriveUnit = NULL) const;
	TPtrC FileUidL() const;
	TBool DatabaseReadyL() const;
	
	// Contact Item CRUD methods.
	TContactItemId CreateContactL(CContactItem& aContact) const;
	void DeleteContactL(const TContactItemId aCntId, TCntSendEventAction aCntEventType, TBool aDecAccessCount = EFalse) const;	
	void CommitContactL(const CContactItem& aContact, TBool aSendChangedEvent = ETrue) const;
	CContactItem* ReadContactL(const CContactItemViewDef* aCntItemVDef, TContactItemId aCntId) const;
	CContactItem* OpenContactLX(const CContactItemViewDef* aCntItemVDef, TContactItemId aCntId) const;
	TBool CloseContact(TContactItemId aContactId);
	void SetViewDefinitionL(const CContactViewDef& aView);

	TInt64 MachineId() const;
	void SetOperationTimeOutL(const TInt aMicroSeconds) const;
	TInt OpenCrud() const;
	TInt OpenIterator() const;
	TInt OpenProperties() const;
	TInt CloseCrud() const;
	TInt CloseIterator() const;
	TInt CloseProperties() const;
	void CrudAsyncOp(TInt aParam,TRequestStatus& aStatus) const;
	TInt CrudSyncOp(TInt aParam) const;
	void IteratorAsyncOp(TInt aParam,TRequestStatus& aStatus) const;
	TInt IteratorSyncOp(TInt aParam) const;
	void PropertiesAsyncOp(TInt aParam,TRequestStatus& aStatus) const;
	TInt PropertiesSyncOp(TInt aParam) const;
	
    TInt FileSize() const;
	TInt ReCreateTemplate() const;	    
    
	TInt BeginDbTransaction() const;
	TInt CommitDbTransaction() const;
	TInt RollbackDbTransaction() const;
	
	// Observer methods for database event notifications.
	void AddObserverL(MContactDbObserver& aObserver);
	void RemoveObserver(const MContactDbObserver& aObserver);
	void AddObserverV2L(MContactDbObserverV2& aObserver);
	void RemoveObserverV2(const MContactDbObserverV2& aObserver);
	
	void SetCurrentItem(TContactItemId aContactId) const;
	TContactItemId CurrentItem() const;
	TInt RemoveCurrentItem();	
	TInt GetCurrentDatabase(TDes& aDatabase) const;
	TInt SetCurrentDatabase(const TDesC& aDatabase) const;
	void ResetServerSpeedDialsL();
	TContactItemId GetSpeedDialFieldL(TInt aSpeedDialPosition, TDes& aPhoneNumber);
	void SetFieldAsSpeedDialL(TContactItemId aContactId, TInt aFieldIndex, TInt aSpeedDialPosition);
	void RemoveSpeedDialFieldL(TContactItemId aContactId, TInt aSpeedDialPosition);
	void SetOwnCardL(const CContactItem& aContact);

	TContactItemId OwnCard() const;
	TInt ConnectionId() const;
	void OverrideMachineUniqueId(TInt64 aMachineUniqueId);
	void SetHeapFailure(RHeap::TAllocFail aType, TInt aRate);
	TInt ResourceCount();
	TContactItemId PrefTemplateId() const;
	void SetPrefTemplateL(const TContactItemId aContactId);
	
	CContactIdArray* FetchTemplateListIdsL();
	CContactIdArray* FetchGroupListIdsL();
	
	CContactIdArray* CollectionL(TInt aCollectionType, TTime aTime=0,const TDesC& aGuid = KNullDesC);

	void SetSortPreferenceL(const CArrayFix<CContactDatabase::TSortPref>& aSortOrder);
	CArrayFix<CContactDatabase::TSortPref>* GetSortPreferenceL() const;
	CContactIdArray* MatchPhoneNumberL(const TDesC& aNumber, const TInt aMatchLengthFromRight);
	
	TInt ContactCountL() const;

	TBool SeekContactL(TContactItemId aContactId,TContactItemId& aId,TUid& aContactType, TBool& aDeleted);
	void ReadContactTextDefL(TContactItemId aContactId, TDes &aResult,const CContactTextDef& aTextDef);
	
	void TextFieldL(TInt aCntItemId,TFieldType aFieldType, TDes& aText);
	
	TContactItemId ICCTemplateIdL(TUid aPhonebookUid = KNullUid);
	TContactItemId PhonebookGroupIdL();

	CContactIdArray* FindL(const TDesC& aText,const CContactItemFieldDef* aFieldDef);

	TBool ContactMatchesHintFieldL(TInt aBitWiseFilter,TContactItemId aContactId);
	
	void SetDbViewContactType(const TUid aUid);
	TUid GetDbViewContactType() const;

	void FilterDatabaseL(CCntFilter& aFilter);

	void FindAsyncTextDefInitL(const CDesCArray& aWords,CContactTextDef* aTextDef);
	void FindAsyncInitL(const TDesC& aText,CContactItemFieldDef* aFieldDef);
	TBool FindAsyncL(CContactIdArray*& aIdArray);

	void SetAsyncActivityL(TBool aAsyncActivity);
	
	// Used to request a database event.  See CCntDbNotifyMonitor::Start().
	void StartNotificationTransfer(TRequestStatus& aStatus, TDes8& aEvent);
	void EndNotificationTransfer();

	// See CCntDbNotifyMonitor::RunL().
	void HandlePrematureServerTerminationL();

	// Used to close/unlock the last locked contact items from the cleanup
	// stack.  Called when popping the cleanup stack.
	void UnlockLastLockedContact();

	// Used for test purposes (only works in _DEBUG builds).
	void GetDefinitionsOfExistingViewsL(const TDesC& aDbName, RPointerArray<CContactDefaultViewDefinition>& aViewDefs);

	TInt OpenViewL(const CContactTextDef& aTextDef, const TInt aViewPrefs);
	void CloseView(TInt aViewId);
	void ChangeSortOrderL(TInt aViewId, const CContactTextDef& aTextDef);
	void BeginIterateL(TInt aViewId);
	void EndIterateL(TInt aViewId);
	CViewContact* NextItemL(TInt aViewId, const TInt aViewPrefs);
	CViewContact* ItemAtL(TContactItemId aContactId, TInt aViewId);
	
private:
	TVersion Version() const;
	CContactItem* UnPackContactLC(TInt aBufferSize, TIpcArgs& aArgs) const;
	
	CContactIdArray* DoGetCollectionChangedSinceL(TTime aTime);
	CContactIdArray* DoGetCollectionL(TInt aCollectionType);
	CContactIdArray* DoGetCollectionGuidL(const TDesC& aGuid);
	void PushUnlockL() const;

	// Save the filename.  Used to re-open database if the server is terminated
	// prematurely.
	TInt SetFileName(const TDesC& aCntFile) const;

private:
	CCntDbNotifyMonitor* 	  iDbNotifyMonitor;	
	mutable CCntPackager* 	  iPackager;

	mutable TBuf<KUidBufferSize> iFileUid;
	mutable TInt iConnectionId;
	mutable TBuf<KCntMaxFilePath> iFileName;

	// Number of attempts to restart the server after an unexpected process
	// termination.
	TInt iNoOfSvrStartAttempts;
	};
	
#endif // RCNTMODEL_H
