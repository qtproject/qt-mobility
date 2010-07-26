/*
* Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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


const TInt KNumSecondaryClients=3;


class CBenchMarker : public CBase, public MIdleFindObserver
	{
public:
	CBenchMarker();
	~CBenchMarker();
	void ConstructL(const TDesC& aCommandLine);
	void RunL();
private:
	enum TTest
		{
		EOpenNewDb,
		ECreateSecondaryClients,
		EAddContacts,
		ECompactDb,
		EAddAndCompact,
		EGetSortedItems,
		ELogContactSummary,
		ELogContactSummaryFast,
		EOpenRContactView,
		ESetSortOrderOfRContactView,
		EGetSortOrderOfRContactView,
		ELogContactSummaryWithRContactView,
		ELogContactSummaryFastWithRContactView,
		EFindWithRContactView,
		ECloseRContactView,
		EFindFromLargeFieldSetSuceed,
		EFindFromLargeFieldSetFail,
		EFindFromSmallFieldSetSuceed,
		EFindFromSmallFieldSetFail,
		EFindPhoneNumberSuceed,
		EFindPhoneNumberFail,
		EFindEmailAddressSuceed,
		EFindEmailAddressFail,
		EFindAsyncFromLargeFieldSet,
		EFindAsyncFromSmallFieldSet,
		ECloseDb,
		ECloseSecondaryClients,
		EOpenExistingDb,
		EMultiClientSort,
		ENumTests
		};
private: // From MIdleFindObserver.
	void IdleFindCallback();
private:
	void DecodeCommandLineL(const TDesC& aCommandLine);
	void PrepareLogL();
	TInt DoTestL(TTest aTest);
	void StartProfile();
	TReal EndProfile();
	void StartAverageProfile();
	void UpdateAverageProfile(TInt aNumIterations);
	TReal EndAverageProfile();
	TReal AddContactsL();
	TReal AddContactsCompactL();
	void LogContactSummaryL(TBool aFaster);
	void LogContactSummaryFromRContactViewL(TBool aFaster);
	void LogContactSummaryL(TBool aFaster,TContactItemId aContactItemId);
	void DoSortL();
	void OpenRContactViewL();
	void SetSortOrderOfRContactViewL();
	void GetSortOrderOfRContactViewL();
	void FindInRContactViewL();
	void FindFromLargeFieldSetL(const TDesC& aTextToFind);
	void FindFromSmallFieldSetL(const TDesC& aTextToFind);
	void FindPhoneNumberL(const TDesC& aTextToFind);
	void FindMobilePhoneNumberL(const TDesC& aTextToFind);
	void FindEmailAddressL(const TDesC& aTextToFind);
	TReal FindAsyncFromLargeFieldSetL(const TDesC& aTextToFind);
	TReal FindAsyncFromSmallFieldSetL(const TDesC& aTextToFind);
	void DoFindL(const TDesC& aTextToFind,const CContactItemFieldDef& aFieldDef);
	TReal DoFindAsyncL(const TDesC& aTextToFind,const CContactItemFieldDef& aFieldDef);
	void CreateSecondaryClients();
	void CloseSecondaryClients();
	void CreateSortClients();
	void IncVisualCounter();
	void EndVisualCounter();
	TInt DbFileSize();
	void LogResult(TInt aMajorTestNum,TInt aMinorTestNum,const TDesC& aDescription,TInt aResult);
	void LogResult(TInt aMajorTestNum,TInt aMinorTestNum,const TDesC& aDescription,TReal aResult);
private:
	RFs iFs;
	RTest iTest;
	TFileName iLogFileName;
	TFileName iDbFileName;
	CLog* iLog;
	CContactDatabase* iDb;
	CRandomContactGenerator* iGenerator;
	const CContactIdArray* iSortedItems;
	CContactTextDef* iTextDef;
	CIdleFinder* iIdleFinder;
	TInt iVisualCounter;
	TReal iAverageProfileCounter;
	TInt iNumAverageProfiles;
	TBool iNoDataGenerationTests;
	TInt iNumTypicalContacts;
	TInt iNumAtypicalContacts;
	TInt iNumTypicalContactsCompact;
	TInt iNumAtypicalContactsCompact;
	RThread secondaryClient[KNumSecondaryClients];
#ifdef __USE_NEW_INTERFACES
	RContactView iSortedIdList;
#endif
	};


class TThreadCommandLine
	{
public:
	TThreadCommandLine(TRequestStatus* aInitialStatus,TRequestStatus* aCompletionStatus,const TDesC& aDbFileName,TBool aExitAfterSort)
		: iInitialStatus(aInitialStatus),iCompletionStatus(aCompletionStatus),iDbFileName(aDbFileName),iExitAfterSort(aExitAfterSort) {}
	RThread iOwningThread;
	TRequestStatus* iInitialStatus;
	TRequestStatus* iCompletionStatus;
	const TDesC& iDbFileName;
	TBool iExitAfterSort;
	};
