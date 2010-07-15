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


#include <cntview.h>
#include "cntviewprivate.h"
#include "t_utils2.h"


//
// Forward declarations.
//

class CSortChecker;
class CTestBase;
class CViewCreator;
class CContactAdder;
class CContactRemover;
class CSortOrderChanger;
class CUnsortableContactAdder;
//
// CTestConductor.
//

class CTestConductor : public CActive, public MContactViewObserver
	{
public:
	static CTestConductor* NewL();
	~CTestConductor();
	void NextTest();
private:
	CTestConductor();
	void ConstructL();
private: // From CActive.
	void RunL();
	TInt RunError(TInt aError);
	void DoCancel();
private: // From MContactViewObserver.
	virtual void HandleContactViewEvent(const CContactViewBase& aView,const TContactViewEvent& aEvent);
private:
	void ExerciseViewPreferencesL(const CContactViewBase* aViewToTest,TContactViewPreferences aPrefsToTest);
private:
	enum TTest
		{
		EAsyncOpen,
		EAsyncOpened,
		ECreateLocalView,
		EAddInitialContacts,
		ECreateLocalViewWithPlugin,		
		ECreateRemoteView,
		ECreateRemoteViewWithPlugin,
		ECriteriaSearchForStringInLocal, 
		ECriteriaSearchForStringInLocalWithPlugin,
		ECriteriaSearchForStringInRemote, 
		ECriteriaSearchForStringInRemoteWithPlugin,
		EPrefixSearchForStringInLocal, 
		EPrefixSearchForStringInLocalWithPlugin,
		EPrefixSearchForStringInRemote, 
		EPrefixSearchForStringInRemoteWithPlugin,
		ENumTests
		};
public:
	CContactOpenOperation* iContactOpenOperation;
	RFs iFs;
	CLog* iLog;
	CContactDatabase* iDb;
	CSortChecker* iSortChecker;
	RContactViewSortOrder iSortOrder;
	CContactLocalView* iLocalView;
	CContactLocalView* iLocalViewWithPlugin;
	CContactRemoteView* iRemoteView;
	CContactRemoteView* iRemoteViewWithPlugin;
	CRandomContactGenerator* iRandomGenerator;
	RArray<TContactItemId> iTestIds;
private:
	CViewCreator* iViewCreator;
	CContactAdder* iContactAdder;
	CContactRemover* iContactRemover;
	CSortOrderChanger* iSortOrderChanger;
	TInt iCurrentTestNumber;
	CTestBase* iCurrentTestObject;
	CPtrC16Array* iFindDesArray;
	CPtrC16Array* iFindDesArray2;
	CUnsortableContactAdder* iUnsortableAdder;
	TInt iTestError;
	};


//
// CSortChecker.
//

class CSortChecker : public CBase
	{
public:
	enum TSortOrder
		{
		EFirstNameLastName,
		ELastNameFirstName
		};
	enum TCriteria
		{
		ELessThan,
		ELessThanOrEqualTo,
		EGreaterThan,
		EGreaterThanOrEqualTo
		};
public:
	static CSortChecker* NewL(CTestConductor& aConductor);
	~CSortChecker();
	void AddContactL(const TDesC& aFirstName,const TDesC& aLastName);
	void RemoveContactL(const TDesC& aFirstName,const TDesC& aLastName);
	TPtrC ContactAt(TSortOrder aOrder,TInt aIndex) const;
	TInt Count() const;
	TInt Count(TSortOrder aOrder,const TDesC& aLowBoundary,const TDesC& aHighBoundary) const;
	TInt CountLow(TSortOrder aOrder,const TDesC& aHighBoundary) const;
	TInt CountHigh(TSortOrder aOrder,const TDesC& aLowBoundary) const;
	void Sort();	
	void CheckSort(TSortOrder aOrder,const TDesC& aName,TInt aIndex) const;
	void CheckSortLowSubView(TSortOrder aOrder,const TDesC& aName,TInt aIndex,const TDesC& aHighBoundary) const;
	void CheckSortHighSubView(TSortOrder aOrder,const TDesC& aName,TInt aIndex,const TDesC& aLowBoundary) const;
	void CheckSortSubView(TSortOrder aOrder,const TDesC& aName,TInt aIndex,const TDesC& aLowBoundary,const TDesC& aHighBoundary) const;
private:
	CSortChecker(CTestConductor& aConductor);
	void ConstructL();
	TInt FindIndex(TSortOrder aOrder,const TDesC& aBoundary,TCriteria aCriteria) const;
	static TInt Compare(const HBufC& aFirst, const HBufC& aSecond);
 	static TInt CompareWithoutIgnoringSpacesL(const HBufC& aFirst, const HBufC& aSecond);
 	static void RemoveCharacterFromDescriptor(const TChar aChar, TDes& aDes);
	static TBool Match(const HBufC& aFirst,const HBufC& aSecond);
private:
	CTestConductor& iConductor;
	TCollationMethod* iCollateMethod;
	RPointerArray<HBufC> iSortedFirstLast;
	RPointerArray<HBufC> iSortedLastFirst;
	};


//
// CTestBase.
//

class CTestBase : public CBase
	{
public:
	virtual void HandleNotificationL(const CContactViewBase& aView,const TContactViewEvent& aEvent)=0;
protected:
	CTestBase(CTestConductor& aConductor);
protected:
	CTestConductor& iConductor;
	};


//
// CViewCreator.
//

class CViewCreator : public CTestBase
	{
public:
	static CViewCreator* NewL(CTestConductor& aConductor);
	CContactLocalView* CreateLocalViewL(const RContactViewSortOrder& aSortOrder);
	CContactLocalView* CreateLocalViewL(const RContactViewSortOrder& aSortOrder,TContactViewPreferences aViewPrefs);
	CContactNamedLocalView* CreateNamedLocalViewL(const TDesC& aName,const RContactViewSortOrder& aSortOrder);
	CContactRemoteView* CreateRemoteViewL(const RContactViewSortOrder& aSortOrder);
	CContactNamedRemoteView* CreateNamedRemoteViewL(const TDesC& aName,const RContactViewSortOrder& aSortOrder);
	CContactSubView* CreateSubViewL(CContactViewBase& aUnderlyingView,const TDesC& aBoundary);
	CContactSubView* CreateSubViewL(CContactViewBase& aUnderlyingView,const TDesC& aLowBoundary,const TDesC& aHighBoundary);
	CContactConcatenatedView* CreateConcatenatedViewL(RPointerArray<CContactViewBase>& aComponentViews);
	virtual void HandleNotificationL(const CContactViewBase& aView,const TContactViewEvent& aEvent);
private:
	CViewCreator(CTestConductor& aConductor);
	CContactViewBase* iView;
	};


//
// CContactAdder.
//

class CContactAdder : public CTestBase
	{
public:
	static CContactAdder* NewL(CTestConductor& aConductor);
	~CContactAdder();
	void AddContactsL(const TDesC& aFileName,TInt aNumViews);
	virtual void HandleNotificationL(const CContactViewBase& aView,const TContactViewEvent& aEvent);
	static TBool ReadNamePairL(RFile aFile,TDes& aFirstName,TDes& aLastName);
private:
	CContactAdder(CTestConductor& aConductor);
	void ConstructL();
	void AddOneContactL();
private:
	RFs iFs;
	RFile iFile;
	CContactTemplate* iTemplate;
	TBool iFinished;
	TInt iNumViews;
	TInt iNumNotifications;
	};


//
// CContactRemover.
//

class CContactRemover : public CTestBase
	{
public:
	static CContactRemover* NewL(CTestConductor& aConductor);
	~CContactRemover();
	void RemoveContactsL(const TDesC& aFileName,TInt aNumViews);
	virtual void HandleNotificationL(const CContactViewBase& aView,const TContactViewEvent& aEvent);
private:
	CContactRemover(CTestConductor& aConductor);
	void ConstructL();
	void RemoveOneContactL();
private:
	RFs iFs;
	RFile iFile;
	TBool iFinished;
	TInt iNumViews;
	TInt iNumNotifications;
	};

class CSortOrderChanger : public CTestBase
	{
public:
	static CSortOrderChanger* NewL(CTestConductor& aConductor);
	void ChangeSortOrderL(RContactViewSortOrder& aSortOrder,CContactNamedLocalView& aView,TInt aNumViews);
	void ChangeSortOrderL(RContactViewSortOrder& aSortOrder,CContactNamedRemoteView& aView,TInt aNumViews);
	~CSortOrderChanger();
	virtual void HandleNotificationL(const CContactViewBase& aView,const TContactViewEvent& aEvent);
private:
	CSortOrderChanger(CTestConductor& aConductor);
private:
	CContactTextDef* iTextDef;
	TInt iNumViews;
	TInt iNumNotifications;
	};

//
// CContactAdder.
//

class CUnsortableContactAdder : public CTestBase
	{
public:
	static CUnsortableContactAdder* NewL(CTestConductor& aConductor,const CContactViewBase& aView);
	~CUnsortableContactAdder();
	void AddContactsL(TInt aNumberOfContact,TContactViewPreferences aContactTypes);
	virtual void HandleNotificationL(const CContactViewBase& aView,const TContactViewEvent& aEvent);
	inline void SetNumContactExpected(TInt aNoContact) {iNumContacts=aNoContact;}
	inline TInt NumContactsExpected() {return iNumContacts;}
private:
	CUnsortableContactAdder(CTestConductor& aConductor,const CContactViewBase& aView);
	void ConstructL();
private:
	TBool iFinished;
	TInt iNumContacts;
	TInt iNumNotifications;
	const CContactViewBase* iView;
	};
