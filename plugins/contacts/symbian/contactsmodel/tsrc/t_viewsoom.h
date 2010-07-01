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
class CViewExerciser;
class CSortOrderChanger;
class CContactDbEventQueue;

enum TOomFailureModes
	{
	KOomFailNormal,
	KOomFailFileServerGeneral,
	KOomFailFileServerOom,
	KOomFailContactServer,
	KOomNumFailModes,
	};

const TInt KNumContactsInGroupOne=27;
_LIT(KGroupOneName,"GroupOne");

typedef TBuf<0x40> TOomTestName;

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
	enum TTest
		{
		ECreateLocalView,
		EAddInitialContacts,
		EAddGroup,
		ECreateRemoteView,
		EDoOOMTests,
		ENumTests
		};
public:
	RTest iTest;
	RFs iFs;
	CLog* iLog;
	CContactDatabase* iDb;
	CSortChecker* iSortChecker;
	RContactViewSortOrder iSortOrder_1;
	RContactViewSortOrder iSortOrder_2;
	CContactLocalView* iLocalView;
	CContactRemoteView* iRemoteView_1;
private:
	CViewCreator* iViewCreator;
	CContactAdder* iContactAdder;
	CContactRemover* iContactRemover;
	CViewExerciser* iViewExerciser;
	CSortOrderChanger* iSortOrderChanger;
	TInt iCurrentTestNumber;
	CTestBase* iCurrentTestObject;
	TBool iGnoreNotifications;
	CContactGroup* iGroupOne;
	TContactItemId iGroupId;
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
	CRandomContactGenerator* iRandomGenerator;
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


//
// CViewExerciser.
//

class CViewExerciser : public CTestBase
	{
public:
	static CViewExerciser* NewL(CTestConductor& aConductor);
	//void ProfileViewL(CContactViewBase& aView);
	void ExerciseViewL(CSortChecker::TSortOrder aSortOrder,CContactViewBase& aView);
	void ExerciseSubViewL(CSortChecker::TSortOrder aSortOrder,CContactViewBase& aView,const TDesC& aLowBoundary,const TDesC& aHighBoundary);
	void ExerciseLowSubViewL(CSortChecker::TSortOrder aSortOrder,CContactViewBase& aView,const TDesC& aLowBoundary);
	void ExerciseHighSubViewL(CSortChecker::TSortOrder aSortOrder,CContactViewBase& aView,const TDesC& aHighBoundary);
	void QueueNextTestWhenComplete(TBool aQueue);
	~CViewExerciser();
	virtual void HandleNotificationL(const CContactViewBase& aView,const TContactViewEvent& aEvent);
private:
	CViewExerciser(CTestConductor& aConductor);
	void ConstructL();
private:
	CContactTextDef* iTextDef_FirstLast;
	CContactTextDef* iTextDef_LastFirst;
	TBool iQueue;
	};


//
// CSortOrderChanger.
//

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

//////////////////
///OOM TEST CODE
//////////////////
class COomTest;
class COomFailBase : public CBase, public MContactViewObserver
	{
public:
	~COomFailBase();
	COomFailBase(COomTest *aOomTest,CContactDatabase* aDb,CContactViewBase* aUnderlyingView);
	virtual void ConstructL();
	virtual void PreFailL();
	virtual void ClearUp();
	virtual void FailL()=0;
	virtual TOomTestName TestName()=0;
	virtual TBool Skip(TInt aCount, TOomFailureModes aMode) const;
	void ClearEventQueueL();
private: // From MContactViewObserver.
	virtual void HandleContactViewEvent(const CContactViewBase& aView,const TContactViewEvent& aEvent)=0;
protected:
	COomTest *iOomTest;
	CContactDatabase* iDb;
	CContactViewBase* iUnderlyingView;
	};


class COomTest : public CBase
	{
public:
	COomTest(CContactDatabase* aDb,CContactLocalView* aLocalView,CContactRemoteView* aRemoteView,CContactGroup* aGroup,CTestConductor& aConductor);
	void ConstructL();
	~COomTest();
	TBool DoTestL();
	void DoOomTestL(COomFailBase *aOomTest, TOomFailureModes aMode);
	void Test(TBool aMustBeTrue);
	TBool TestsL();
	CTestConductor& Conductor() const;
private:
	CContactDatabase* iDb;
	CContactLocalView* iLocalView;
	CContactRemoteView* iRemoteView;
	CContactGroup* iGroupOne;//not owned
	CTestConductor& iConductor;
	};


class COomLocalView : public COomFailBase
	{
public:
	inline COomLocalView(COomTest *aOomTest,CContactDatabase* aDb,CContactViewBase* aUnderlyingView, const TDesC8& aSortPluginName = KNullDesC8) :
		 COomFailBase(aOomTest,aDb,aUnderlyingView),  iSortPluginName(aSortPluginName){};
	void FailL();
	void PreFailL();
	void ClearUp();
	inline TOomTestName TestName() {return(_L("CContactLocalView : NewL"));};
private:
	virtual void HandleContactViewEvent(const CContactViewBase& aView,const TContactViewEvent& aEvent);
private:
	RContactViewSortOrder iSortOrder;
	CContactLocalView* iLocalView;
	TPtrC8 iSortPluginName;
	};

/////////////
class COomRemoteView : public COomFailBase
	{
public:
	inline COomRemoteView(COomTest *aOomTest,CContactDatabase* aDb,CContactViewBase* aUnderlyingView, const TDesC8& aSortPluginName = KNullDesC8) : 
		COomFailBase(aOomTest,aDb,aUnderlyingView),  iSortPluginName(aSortPluginName){};
	void FailL();
	void PreFailL();
	void ClearUp();
	inline TOomTestName TestName() {return(_L("CContactRemoteView : NewL"));};
private:
	virtual void HandleContactViewEvent(const CContactViewBase& aView,const TContactViewEvent& aEvent);
private:
	RContactViewSortOrder iSortOrder;
	CContactRemoteView* iRemoteView;
	TPtrC8 iSortPluginName;
	};

///////////////////
class COomSubView : public COomFailBase
	{
public:
	inline COomSubView(COomTest *aOomTest,CContactDatabase* aDb,CContactViewBase* aUnderlyingView) : COomFailBase(aOomTest,aDb,aUnderlyingView) {};
	void FailL();
	void PreFailL();
	void ClearUp();
	inline TOomTestName TestName() {return(_L("CContactSubView : NewL"));};
	TBool Skip(TInt aCount, TOomFailureModes aMode) const;
private:
	virtual void HandleContactViewEvent(const CContactViewBase& aView,const TContactViewEvent& aEvent);
private:
	CContactSubView* iSubViewAbc;
	};

///////////////////
class COomFilteredView : public COomFailBase
	{
public:
	inline COomFilteredView(COomTest *aOomTest,CContactDatabase* aDb,CContactViewBase* aUnderlyingView) : COomFailBase(aOomTest,aDb,aUnderlyingView) {};
	void FailL();
	void PreFailL();
	void ClearUp();
	inline TOomTestName TestName() {return(_L("COomFilteredView : NewL"));};
private:
	virtual void HandleContactViewEvent(const CContactViewBase& aView,const TContactViewEvent& aEvent);
private:
	CContactFilteredView* iPhonableFilterView;
	};


///////////////////
class COomGroupView : public COomFailBase
	{
public:
	inline COomGroupView(COomTest *aOomTest,CContactDatabase* aDb,CContactViewBase* aUnderlyingView,CContactGroup* aGroup) : COomFailBase(aOomTest,aDb,aUnderlyingView),iGroupOne(aGroup) {};
	void FailL();
	void PreFailL();
	void ClearUp();
	inline TOomTestName TestName() {return(_L("COomGroupView"));};
	TBool Skip(TInt aCount, TOomFailureModes aMode) const;
private:
	virtual void HandleContactViewEvent(const CContactViewBase& aView,const TContactViewEvent& aEvent);
private:
	CContactGroupView* iGroupView;
	TBool iAsStarted;
	CContactGroup* iGroupOne;//not owned
	};

/** Tests construction of contacts matching criteria method in OOM situations */
class COomMatchingCriteria: public COomFailBase
	{
public:
	inline COomMatchingCriteria(COomTest *aOomTest,CContactDatabase* aDb,CContactViewBase* aUnderlyingView) : COomFailBase(aOomTest,aDb,aUnderlyingView) {};
	inline TOomTestName TestName() {return(_L("ContactsMatchingCriteriaL"));};
	void FailL();
	static void CleanupContactArray(TAny* aArray);
private:
	virtual void HandleContactViewEvent(const CContactViewBase& aView,const TContactViewEvent& aEvent);
	};

