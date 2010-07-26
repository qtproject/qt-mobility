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

#ifndef __PERSISTENCELAYERIMPL_H__
#define __PERSISTENCELAYERIMPL_H__

#include "persistencelayer.h" // for interface definitions.
#include "pplcontactitemmanager.h"
#include "pltables.h"
#include <sqldb.h>
class CContactItemManger;
class CCntSqlStatement;

#define KIgnorePhbkSync 0

/**
The CPplContactsFile class represents a single contacts database file and
provides all common database file operations such as open, close, create and
delete. 
*/
class NONSHARED CPplContactsFile:  public CBase, public MLplContactsFile, public MLplTransactionManager
	{
public: // MLplContactsFile interface.

	static CPplContactsFile* NewL(CLplContactProperties& aProps, MContactDbObserverV2* aObserver);
	~CPplContactsFile();

	IMPORT_C void CreateL(const TDesC& aFileName, TPlCreateMode aMode = EPlLeaveIfExist);
	void OpenL(const TDesC& aFileName, TBool aNotify = EFalse);  
	void Close(TBool aNotify = EFalse);
	void DeleteL(const TDesC& aFileName);
	CDesCArray* ListL(TDriveUnit* aDriveUnit = NULL);
	inline TBool IsOpened() const;
	virtual void CloseTablesL(TBool aNotify);
	virtual void OpenTablesL(TBool aNotify = EFalse);	

	inline MLplPersistenceBroker& PersistenceBroker();
	inline MLplTransactionManager& TransactionManager();
	inline MLplFieldMatcher& FieldMatcher();
	
	void DatabaseDrive(TDriveUnit& aDriveUnit);
	TBool DatabaseExistsL(const TDesC& aFileName);
	TInt FileSize() const;
	
	// Non-interface methods.
	void RegisterDbObserver(MContactDbObserverV2& aDbObserver);
	inline RSqlDatabase& NamedDatabase();
	inline CLplContactProperties& ContactProperties();

	void StartTransactionL();
	void CommitCurrentTransactionL(TUint aSessionId);
	void RollbackCurrentTransactionL(TUint aSessionId);
	inline TBool IsTransactionActive() const;

	void NotifyObserver() const;

private: // General methods used to implement all interfaces.
	CPplContactsFile(CLplContactProperties& aProps, MContactDbObserverV2* aObserver);
	void ConstructL();

	void GenerateNotificationEventL(TBool aNotify = EFalse);

	void GetPhysicalPathL(TDes& aPhysicalFileName, const TDesC& aSecuredFileName);
	void GetPhysicalFileNameL(TDes& aPhysicalFileName, const TDesC& aSecuredFileName);

	inline void LocalFsL();

private: // Member variables used by several interfaces.
	TBool iActive;  // Member variables used by MLplTransactionManager interface.

	RFs iLocalFs;
	
	CPplContactItemManager* iItemManager;
  	MContactDbObserverV2* iDbObserver;  
  	CLplContactProperties& iContactProperties;

	RSqlDatabase iDatabase;     // owned
	TDriveUnit iDatabaseDrive;
	TBool iFileIsOpen;
	
	RPplIccContactStore iIccContactStore; //owned
	HBufC8* iConfigureStr;
	};


inline void CPplContactsFile::LocalFsL()
  	{
  	if(iLocalFs.Handle() == 0)
  		{
  		User::LeaveIfError(iLocalFs.Connect());
  		}
  	}


inline RSqlDatabase& CPplContactsFile::NamedDatabase()
	{
	return iDatabase;
	}


inline CLplContactProperties& CPplContactsFile::ContactProperties()
	{
	return iContactProperties;
	}


inline TBool CPplContactsFile::IsTransactionActive() const 
	{
	return iActive;
	}


inline TBool CPplContactsFile::IsOpened() const
	{
	return iFileIsOpen;
	}


inline MLplPersistenceBroker& CPplContactsFile::PersistenceBroker()
	{
	return *(static_cast<MLplPersistenceBroker*>(iItemManager));
	}

inline MLplTransactionManager& CPplContactsFile::TransactionManager()
	{
	return *this;
	}

inline MLplFieldMatcher& CPplContactsFile::FieldMatcher()
	{
	return *(static_cast<MLplFieldMatcher*>(iItemManager));
	}

// Forward class reference.
class CPrivFindViewColSet;


/**
The CPlCollection class provides the implementation for APIs making simple
queries to the database as well as the functions for legacy finding and sorting
APIs.
*/
class NONSHARED CPlCollection : public CBase , public MLplCollection
	{
public:
	static CPlCollection* NewL(CPplContactsFile& aContactsFile);
	~CPlCollection();

	CContactIdArray* CollectionL(TLplViewType aViewType,TTime aTime = 0, const TDesC& aGuid = KNullDesC);

	TInt ContactCountL();
	TBool ContactMatchesHintFieldL(TInt aBitWiseFilter, TContactItemId aContactId);	
	CContactIdArray* MatchPhoneNumberL(const TDesC& aNumber, const TInt aMatchLengthFromRight);

	CContactIdArray* FindL(const TDesC& aText, const CContactItemFieldDef* aFieldDef, TUint aSessionId);	
	void FindAsyncInitL(const TDesC& aText,CContactItemFieldDef* aFieldDef);
	void FindAsyncTextDefInitL(const CDesCArray& aWords,CContactTextDef* aTextDef);
	CContactIdArray* FindAsyncL(TBool& aMoreToGo, TUint aSessionId);

	TBool UsesIdentityFieldsOnly(TInt aFindFlags);
	void ConstructBitwiseFlagsFromTextDef(TInt& aFindFlags,TInt& aIdentityColumnsCount,const CContactTextDef* aTextDef);
	TBool SeekContactL(TContactItemId aReqId,TContactItemId& aId,TUid& aContactType, TBool& aDeleted);	
	
private:
    CPlCollection(CPplContactsFile& aContactsFile);
	void ConstructL();
	CContactIdArray* GuidL(const TDesC& aGuid);
	CContactIdArray* ChangedSinceL(TTime aTime);
	CContactIdArray* UnfiledL();
	CContactIdArray* DeletedL();
	TBool PerformFindIterationL(CContactIdArray *aIdsFound, const TDesC& aText, RSqlStatement aStatement, TInt aFieldsToSearch, TUint aSessionId);
	TBool PerformIdFindIterationL(CContactIdArray *aIdsFound, RSqlStatement aStatement);
	TBool FindL(CContactIdArray *aIdsFound, const TDesC& aText,const CContactItemFieldDef *aFieldDef, RSqlStatement aStatement, TUint aSessionId);
	CContactIdArray* FilterDatabaseL(CCntFilter& aFilter);
	void Reset();
	TInt MaximumSizeOfIdentitySearchSyntax();
	TInt ApproximateSizeOfSearchString();
	void doAppendFieldsToSearchString(HBufC* aOrderFields) const;
	TBool GetContactIdsForTextDefFindL(CContactIdArray* aIdArray, TUint aSessionId);
	TBool PerformTextDefFindIterationL(CContactIdArray* aIdArray); 
	TBool HintFieldMatchesFilter(TInt aHintField, TInt aFilter);
	
private:
	enum TAsyncFindState
		{
		EFindInBlobFinished				=0x00000001,
		EFindInIdentityFinished			=0x00000002,
		EFindInEmailFinished			=0x00000004,
		EFindInTextDefFinished			=0x00000008,
		EFindInSIPFinished				=0x00000010
		};

private:
	CPplContactsFile&		iContactsFile; // doesn't own
	CContactItemFieldDef*	iFieldDef;
	CContactTextDef*		iTextDef;
	HBufC					*iText;
	HBufC					*iOriginalText;
	CDesCArray* 			iFindWords;
	CDesCArray*				iFindWords2;
	TInt					iFindFlags;
	TInt					iNoIdentitySearchColumns;
	TInt 					iFindState;
	// The column number of the parent ID in iEmailFindView.
	
	CCntSqlStatement*		iSelectStatement;	
	
	//RSqlstatements for async find
	RSqlStatement			selectBlobStatement;
	RSqlStatement			selectIdentityStatement;
	RSqlStatement			selectEmailStatement;
	RSqlStatement			selectSIPStatement;
	RSqlStatement			selectIdFromIdentityStatement;
	};

// Forward class reference.
class CCntPplViewManager;

/**
This is the server-side implementation of the MLplPersistenceLayerFactory
interface.  It is the single access point for all data services required for
Local View functionality.
*/
class NONSHARED CLplPersistenceLayerFactory : public CBase, public MLplPersistenceLayerFactory
	{
public:
	CLplPersistenceLayerFactory(CPplContactsFile& aContactsFile,CLplContactProperties& aContactProperties);
	~CLplPersistenceLayerFactory();
	
	MLplViewIteratorManager& GetViewIteratorManagerL();
	MContactSynchroniser& GetContactSynchroniserL(TUint aSessionId);
	MLplCollection&			GetCollectorL();	

private:
	CPplContactsFile&		iContactsFile;
	CLplContactProperties&  iContactProperties;
	CCntPplViewManager*	    iManager;
	CPlCollection*			iCollection;
	};

/**
Support class useful for testing of the Contacts Model.
*/
class NONSHARED CLplTester : public MLplPersistenceLayerTest
	{
public:
	CLplTester(CLplContactProperties& aProps, CPplContactsFile& aFile);

	void MergeContactWithTemplateL(CContactItem& aContact, const CContactItem& aTempl, const CContactItemViewDef& aView) const; 
	void DamageDatabaseL(TInt aSecretCode);

private:
	CLplContactProperties& iProps;
	CPplContactsFile&	   iFile;
	};

#endif // __PERSISTENCELAYERIMPL_H__
