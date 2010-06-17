/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __PERSISTENCELAYER_H__
#define __PERSISTENCELAYER_H__

#include <cntdef.h>
#include <cntitem.h>
#include <cntviewbase.h>


#ifdef __EABI__
  #define NONSHARED __declspec(notshared)
#else
  #define NONSHARED
#endif


enum TPlReadMode 
	{
	EPlBlobOnly	  = 0,
	EPlIdentityInfo = 1,
	EPlEmailInfo =	2,
	EPlGroupMembershipInfo =	4,
	EPlAllInfo = EPlIdentityInfo | EPlEmailInfo | EPlGroupMembershipInfo 
	};


enum TOpenStates 
	{
	EPlClosed = 0,
	EPlFileOpen,
	EPlRecover,
	EPlUpgrade,
	EPlOpened
	};


/**
This interface defines five basic operations on CContactItem objects in the
Persistence Layer:

- Create
- Read
- Update
- Delete
- Change type

The methods in this interface provide the only mechanism to change data in the
Contacts database.

The SetConnectionId() method is a helper function and it is used in the event
notification mechanism.
*/
class MLplPersistenceBroker
	{
public:
	/** Store a new contact. */
	virtual TContactItemId CreateL(CContactItem& aItem, TUint aSessionId)=0;

	/** Read an existing contact (or parts of it as specified by the view definition). */
	virtual CContactItem* ReadLC(TContactItemId aItemId, const CContactItemViewDef& aView, TInt aInfoToRead, TUint aSessionId, TBool aIccOpenCheck = EFalse) const =0;

	/** Update an existing contact (or parts of it as specified by the view definition). */
	virtual void UpdateL(CContactItem& aItem, TUint aSessionId, TBool aSpeeDailUpdate = EFalse) = 0;

	/** Delete a contact.  The deleted contact is read first before the deletion 
	and the object is returned.  This is useful for the analysis of the deleted contact. */
	virtual CContactItem* DeleteLC(TContactItemId  aItemId, TUint aSessionId, TCntSendEventAction aEventType) = 0;

	/** Change type for the current contact item. */
	virtual void ChangeTypeL(TContactItemId aItemId, TUid aNewType) = 0;

	/** Set connection ID for the notification mechanism. */
	virtual void SetConnectionId(TInt aConnectionId) = 0;
	};


/**
The MLplTransactionManager class is part of the Logical sub-layer of the
Persistence Layer. 

The interface offers Logical transactions and it ensures that the write and read
operations are atomic, consistent, isolated and durable.  

The MLplTransactionManager class only provides support for a single transaction. 
*/
class MLplTransactionManager
	{
public:
	/** Start a new transaction. */
	virtual void StartTransactionL() = 0;
	/** Commit the current transaction. */
	virtual void CommitCurrentTransactionL(TUint aSessionId) = 0;
	/** Rollback the current transaction. */
	virtual void RollbackCurrentTransactionL(TUint aSessionId) = 0;
	/** Returns if a transaction is currently in progress. */
	virtual TBool IsTransactionActive() const = 0;
	};


/**
The MLplContactsFile interface defines a set of operation on the Contacts
database file, such as opening and closing, compression, recovery, deletion
and Contacts Model private directory listing.

Note that some operations do not require the Contacts database file to be
opened.
*/
class MLplContactsFile
	{
public:
	enum TPlCreateMode {EPlLeaveIfExist, EPlOverwrite};
	 
	virtual void CreateL(const TDesC& aFileName, TPlCreateMode aMode = EPlLeaveIfExist)=0;
	virtual void Close(TBool aNotify = ETrue)=0;
	virtual void DeleteL(const TDesC& aFileName)=0;
	virtual CDesCArray* ListL(TDriveUnit* aDriveUnit = NULL)=0;
	virtual TBool DatabaseExistsL(const TDesC& aFileName) = 0;
	virtual void DatabaseDrive(TDriveUnit& aDriveUnit) = 0;
	virtual TInt FileSize() const = 0;
  	virtual void CloseTablesL(TBool aNotify) = 0;
  	virtual void OpenTablesL(TBool aNotify = EFalse) = 0;  	

    virtual void OpenL(const TDesC& aFileName, TBool aNotify = EFalse)=0; 
	};


// Forward class references.
class CContactTemplate;
class CContactPhoneParser;


/**
Originally MLplContactProperties was split into two classes.

One was concerned with looking up contact item attributes (type, hint and text)
by ID and the other was solely concerned with looking after the Preferences
table.

The latter was considered to be data that was 'static' to the whole model.  

Both were merged into MLplContactProperties.
*/
class MLplContactProperties
	{
public:
	virtual TInt CardTemplatePrefIdL() const = 0;
	virtual void SetCardTemplatePrefIdL(TInt aCardTemplatePrefId) = 0;
	virtual TInt64 MachineIdL() const = 0;
	virtual CContactIdArray& CardTemplateIdsL() = 0;
	virtual CContactIdArray& GroupIdListL() = 0;
    virtual CBufSeg* DetailsListL(const TDesC& aSearchQuery) const = 0;
	virtual void SetMachineIdL(TInt64 aMachineId) = 0;
	virtual TPtrC UniqueIdL() = 0; 
	
	// System template methods.
	virtual const CContactTemplate& SystemTemplateL() const =0;
	virtual void RecreateSystemTemplateL() = 0;

	virtual TContactItemId OwnCardIdL() = 0;
	virtual void SetOwnCardIdL(TContactItemId aId, TBool aPersist = ETrue) = 0;

	virtual const CArrayFix<CContactDatabase::TSortPref>& SortPrefsL() =0;
	virtual void SetSortPrefsL(CArrayFix<CContactDatabase::TSortPref>* aPrefs) =0;

	virtual void SetDbViewContactType(TUid aDbViewContactType) =0;
	virtual TUid GetDbViewContactType() const =0;

	virtual TContactItemId ICCTemplateIdL(TUid aPhonebookUid, TUint aSessionId) =0;
	virtual TContactItemId PhonebookGroupIdL(TUint aSessionId) =0;
	virtual void DisableSynchroniser(TUint aSessionId) =0;

	virtual void SessionDeleted(TUint aSessionId) =0;
	};

class CViewContact;

#define KPLViewSessionIdNull 0

/**
This interface defines a set of functions for retrieving view contacts from the 
contacts database.The view item manager have read-only access to the database 
and supply all the required data for the contact view framework.
*/
class MLplViewIteratorManager
	{
public:
	//View session create and close methods
	virtual TInt OpenViewL(const CContactTextDef& aTextDef, TContactViewPreferences aViewPrefs) =0;
	virtual void CloseView(TInt aViewId) =0;
	
	//Change sort order should be always called when there is not an iteration 
	//started in persistence layer.
	virtual void ChangeSortOrderL(TInt aViewId, const CContactTextDef& aTextDef) =0;
	
	//Iterator Methods
	virtual void BeginIterateL(TInt aViewId) =0;
	virtual void EndIterateL(TInt aViewId) =0;
	virtual CViewContact* NextItemL(TInt aViewId, TContactViewPreferences aViewPrefs) =0;
	virtual CViewContact* ItemAtL(TContactItemId aContactId, TInt aViewId) =0;
	
	//Read content of a contact into descriptor
	virtual TUid ReadContactTextDefL(TContactItemId aContactId, TDes &aResult,const CContactTextDef& aTextDef) =0;
	
	//Get a sortable text field based on field type.
	virtual void TextFieldL(TInt aCntItemId,TFieldType aFieldType, TDes& aText) =0;
	};


/**
This interface defines a function to Match Phone Numbers in the 
contacts database.
*/
class MLplFieldMatcher
	{
public:
     virtual CContactIdArray* MatchPhoneNumberL(const TDesC& aNumber, TInt aMatchLengthFromRight) = 0;
	};


/**
This interface provides a simplified way of querying the database.  The result
of the query is typically an array of the contact IDs satisfying the given
criteria.  For example, IDs of all the contacts changed since the given date.
*/
class MLplCollection
	{
public:
	enum TLplViewType
		{
		EFilter,
		EChangedSince,
		EFindInIdFields,
		EFindInEmail,
		EFindInAll,
		EFindType,
		EFindGuid,
		EPhoneMatch,
		EViewData,
		EUnfiled,
		EDeleted,
		ESortNoText,
		ESortWithText,
		EMatchPhoneNos
		};
public:
	virtual TBool ContactMatchesHintFieldL (TInt aBitWiseFilter, TContactItemId aContactId) = 0;
	virtual CContactIdArray* CollectionL(TLplViewType aViewType,TTime aTime = 0, const TDesC& aGuid = KNullDesC)=0;
	virtual TInt  ContactCountL () = 0;
	virtual CContactIdArray* MatchPhoneNumberL(const TDesC& aNumber, const TInt aMatchLengthFromRight) = 0;
	virtual CContactIdArray* FindL(const TDesC& aText, const CContactItemFieldDef* aFieldDef, TUint aSessionId) = 0;
	virtual CContactIdArray* FilterDatabaseL(CCntFilter& aFilter)=0;
	virtual void Reset()=0;
	virtual void FindAsyncInitL(const TDesC& aText,CContactItemFieldDef* aFieldDef)=0;
	virtual void FindAsyncTextDefInitL(const CDesCArray& aWords,CContactTextDef* aTextDef) =0;
	virtual CContactIdArray* FindAsyncL(TBool& aMoreToGo, TUint aSessionId)=0;
	virtual TBool UsesIdentityFieldsOnly(TInt aFindFlags) = 0;
	virtual void ConstructBitwiseFlagsFromTextDef(TInt& aFindFlags,TInt& aIdentityColumnsCount,const CContactTextDef* aTextDef) = 0;
	
	virtual TBool SeekContactL(TContactItemId aReqId,TContactItemId& aId,TUid& aContactType, TBool& aDeleted) = 0;
	};
	

/**
This interface provides a single point of access to the three other interfaces:
	
- View Iterator Manager
- Phone Book Synchroniser Plugin
- Collection Iterator.

This interface is implemented in the Contacts Model twice: once on the client 
and the once on the server.  This allows us to reuse the same code for Local
View on both sides of the Contacts Model.
*/
class MLplPersistenceLayerFactory
	{
public:
	virtual MLplViewIteratorManager& GetViewIteratorManagerL() = 0;
	virtual MContactSynchroniser& GetContactSynchroniserL(TUint aSessionId) = 0;
	virtual MLplCollection& GetCollectorL() = 0;
	};


class MLplPersistenceLayerTest
	{
public:
	virtual void MergeContactWithTemplateL(CContactItem& aContact, const CContactItem& aTempl, const CContactItemViewDef& aView) const =0; 
	};


class MIniFileManager
	{
public:
	virtual void DeleteNotifyL(TContactItemId aContactId) = 0;
	virtual TContactItemId SetSpeedDialIdForPositionL(const TInt aSpeedDialIndex, const TContactItemId aContactId,  const TSpeedDialPhoneNumber& aPhoneNumber, TUint aConnectionId, TBool aSendNotification) = 0;
	virtual CArrayFix<TInt>* SpeedDialIndicesForContactIdLC(TContactItemId aContactId) = 0; 
	};


// Forward class references.
class CPplContactsFile;
class CLplContactProperties;
class CLplTester;
class CLplPersistenceLayerFactory;
class CLplAnalyserProxy;


class NONSHARED CPersistenceLayer : public CBase
	{
public:
	IMPORT_C static CPersistenceLayer* NewLC(RFs& aFs, MIniFileManager* aIniFileManager = NULL, MContactDbObserverV2* aObserver = NULL); 

	virtual ~CPersistenceLayer();

	IMPORT_C MLplContactsFile& ContactsFileL();
	IMPORT_C MLplPersistenceBroker&  PersistenceBroker();
	IMPORT_C MLplTransactionManager&  TransactionManager();
	IMPORT_C MLplContactProperties& ContactProperties();
	IMPORT_C MLplPersistenceLayerFactory& FactoryL();
	IMPORT_C void RegisterDbObserver(MContactDbObserverV2& aDbObserver);
	IMPORT_C MLplPersistenceLayerTest& PersistenceLayerTestL();
			
private:
	CPersistenceLayer(RFs& aFs, MIniFileManager* aIniFileManager, MContactDbObserverV2* aObserver);
	static void CleanupOperation(TAny* aObject);

private:
	RFs& iFs;
	CPplContactsFile* 				iContactsFile;
	CLplContactProperties* 			iContactProperties;
	CLplTester*						iTester;
	CLplAnalyserProxy*				iAnalyserProxy;
	CLplPersistenceLayerFactory*	iFactory;
	MContactDbObserverV2* 			iCntDbObserver; //optional
	MIniFileManager*				iIniManager;	//optional
	};

#endif //__PERSISTENCELAYER_H__
