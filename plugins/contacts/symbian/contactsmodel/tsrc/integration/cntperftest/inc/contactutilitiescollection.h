/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Utility Class which construct different type of views like local view, remote view,
* named remote view, find view, filtered view, group view, concatenated view and sub view.
* It is a collection of different classes like CContactViewCollection, CContactViewApiTest and
* CContactViewValidation
* CContactViewCollection - It is the collection of the arrays which contains pointers to the
* constructed views. Depending upon the requirement, the views can be retrieved.
* CContactViewApiTest - It is set of test functions which calls the View API's and return
* the value.
* CContactViewValidation - It is set of validation functions which validates the result
* returned by the functions in the CContactViewApiTest and the expected results from ini.
*
*/




#ifndef CONTACT_UTILITIES_COLLECTION_H
#define CONTACT_UTILITIES_COLLECTION_H

// User Includes
#include "cntperfserver.h"
#include "ccontactvieweventqueue.h"
#include "clientserver.h"

// System Includes
#include <e32base.h>
#include <e32cmn.h>
#include <cntview.h>
#include <cntdb.h>
#include <cntitem.h>
#include <cntviewbase.h>
#include <testexecutestepbase.h>

// Constant Literals

// CContactUtilitiesCollection
_LIT(KLocalView,					"localview");
_LIT(KRemoteView,					"remoteview");
_LIT(KNamedRemoteView,				"namedremoteview");
_LIT(KFindView,						"findview");
_LIT(KFilteredView,					"filteredview");
_LIT(KGroupView,					"groupview");
_LIT(KConcatenatedView,				"concatenatedview");
_LIT(KSubView,						"subview");
_LIT(KViewSortOrder,				"viewsortorder");
_LIT(KViewPreferences,				"viewpreferences");
_LIT(KLocalViewSortPlugin,			"localviewsortplugin");
_LIT(KViewNotReady,					"viewnotready");
_LIT(KAllocTestForLocalView,		"alloctestforlocalview");
_LIT(KRemoteViewSortPlugin,			"remoteviewsortplugin");
_LIT(KAllocTestForRemoteView,		"alloctestforremoteview");
_LIT(KNamedRemoteViewName,			"name");
_LIT(KNamedViewSortPlugin,			"namedviewsortplugin");
_LIT(KAllocTestForNamedRemoteView,	"alloctestfornamedremoteview");
_LIT(KSearchWords,					"searchwords");
_LIT(KFindViewSortPlugin,			"findviewsortplugin");
_LIT(KSearchType,					"searchtype");
_LIT(KAllocTestForFindView,			"alloctestfornamedremoteview");
_LIT(KGroupType,					"grouptype");
_LIT(KGroupName,					"groupname");
_LIT(KGroupId,						"groupid");
_LIT(KGroupVersion,					"groupviewversion");
_LIT(KAllocTestForGroupView,		"alloctestforgroupview");
_LIT(KViewFilter,					"viewfilter");
_LIT(KAllocTestForFilteredView,		"alloctestforfilteredview");
_LIT(KLowerBoundary,				"lowerboundary");
_LIT(KHigherBoundary,				"higherboundary");
_LIT(KBoundary,						"boundary");
_LIT(KAllocTestForSubView,			"alloctestforsubview");
_LIT(KViewBase,						"viewbase");
_LIT(KIndexOfViews,					"indexofview");
_LIT(KComponentViews,				"componentviews");
_LIT(KViewType,						"viewtype");
_LIT(KAllocTest,					"alloctest");
_LIT(KIndexOfView,					"indexofview");

_LIT(KErrInCreateDataBase,			"Error in creating database");
_LIT(KErrInOpen,					"Error in opening database");
_LIT(KErrInDeleteDataBase,			"Error in deleting database");
_LIT(KErrSortPlugin,				"Error No Sort Plugin available");
_LIT(KContactNotFound,				"No of contact found : %d");
_LIT(KErrorNone,					"ErrNone");



// CContactViewCollection
_LIT(KAtLIndex,						"atlindex");
_LIT(KContactAtLIndex,				"contactatlindex");
_LIT(KContactItemId,				"contactitemid");
_LIT(KAllFieldsIndex,				"allfieldsindex");
_LIT(KFieldSeparator,				"fieldseparator");
_LIT(KErrInRefineFindView,			"Error while refining the find view");

// CContactViewApiTest
_LIT(KError,			 			"The actual result is %d while the desired result is %d ");
_LIT(KDiskFull,						"diskfull");
_LIT(KDiskNotReady,					"disknotready");
_LIT(KNoError,						"errornone");

// CContactViewValidation
_LIT(KDesiredContactItemId,			"desiredcontactitemid");
_LIT(KDesiredContactId,				"desiredcontactid");
_LIT(KDesiredContactType,			"desiredcontacttype");
_LIT(KDesiredContactFilter,			"desiredcontactfilter");
_LIT(KDesiredCount,					"desiredcount");
_LIT(KDesiredIndex,					"desiredindex");
_LIT(KIndex,						"index");

_LIT(KErrInContactId,				"Error in Contact Item id");
_LIT(KErrInContactType,				"Error in Contact type");
_LIT(KErrExpectedAndRetrievedFields,"Expected and retrieved fields is %S, %S");
_LIT(KErrExpectedAndRetrievedViewPreferences,"Expected and retrieved viewpreferences is %d, %d");
_LIT(KErrExpectedAndRetrievedUid,	"Expected and retrieved uid is %d, %d");
_LIT(KErrExpectedAndRetrievedCount,	"Expected and retrieved count is %d, %d");
_LIT(KErrExpectedAndRetrievedIndex,	"Expected and retrieved index is %d, %d");
_LIT(KErrExpectedAndRetrivedContactItemId,"Expected and retrieved Contact Item Id is %d, %d");


const TUint KFsAccessCount = 1;
const TUint KEventWaitTime = 30;
const TInt KMaxLengthString = 100;

class CContactViewCollection;
class CContactUtilitiesCollection : public CBase
	{
public:
	static CContactUtilitiesCollection* NewL(CTestStep& aTestStep, CContactDatabase& aDb, CContactViewCollection& aViewCollection);
	~CContactUtilitiesCollection();

private:
	CContactUtilitiesCollection(CTestStep& aTestStep, CContactDatabase& aDb, CContactViewCollection& aViewCollection);

public:
	CTestStep& BaseTestStepReference();
	CContactDatabase& DatabaseReference();
	CContactViewCollection& ViewCollectionReference();
	void	GetDataAndConstructLocalViewL(const TDesC& aSection);
	void	GetDataAndConstructRemoteViewL(const TDesC& aSection);
	void	GetDataAndConstructNamedRemoteViewL(const TDesC& aSection);
	void	GetDataAndConstructFindViewL(const TDesC& aSection);
	void	GetDataAndConstructGroupViewL(const TDesC& aSection);
	void	GetDataAndConstructFilteredViewL(const TDesC& aSection);
	void	GetDataAndConstructSubViewL(const TDesC& aSection);
	void	ConstructLocalViewL(const TDesC& aSortOrder, const TDesC& aViewPreferences, const TDesC& aSortPlugin,
							TBool aViewNotReady);
	void	ConstructRemoteViewL(const TDesC& aSortOrder, const TDesC& aViewPreferences, const TDesC& aSortPlugin,
								TBool aViewNotReady);
	void	ConstructNamedRemoteViewL(const TDesC& aName, const TDesC& aSortOrder, const TDesC& aViewPreferences,
									const TDesC& aSortPlugin, TBool aViewNotReady);
	void	ConstructFindViewL(const TDesC& aSearchWords, const TDesC& aSearchType,
									CContactViewBase* aViewBase, TBool aViewNotReady);
	void	ConstructGroupViewL(const TDesC& aGroupType, const TDesC& aGroupName, const TInt& aGroupId,
									CContactViewBase* aViewBase, TBool aViewNotReady, TInt& aGroupVersion);
	void	ConstructFilteredViewL(const TDesC& aViewFilter, CContactViewBase* aViewBase, TBool aViewNotReady);
	void	ConstructConcatenatedViewL(const TDesC& aSection);
	void	ConstructSubViewL(const TDesC& aLowerBoundary, const TDesC& aHigherBoundary, const TDesC& aBoundary,
							CContactViewBase* aViewBase, TBool aViewNotReady);
	void	ConstructViewsL(RArray<TPtrC> aSections);
	void	TokenizeStringL(const TDesC& aString, RArray<TPtrC>& aList, TChar aSeparator = ',');
	void	DeleteDataBase(TBool aDeleteDataBase, const TDesC& aDbName);
	void	CreateViewUnderOOMConditionL(const TDesC& aSection, const TDesC& aViewType, const TDesC& aAllocTest );
	void	OOMTestForViewsL(const TDesC& aSection, const TDesC& aViewType, const TDesC& aAllocTest);
	void	ImportVcardContactsL(const TPtrC& aImportFileName, TBool aPbapBased);
	RPointerArray<CContactViewBase>&	GetComponentViewsL(const TDesC& aSection);
	CContactViewBase*	GetDesiredView(const TDesC& aSection);
	RContactViewSortOrder	ConvertStringToSortOrderL(const TDesC& aSortOrder);
	TContactViewPreferences	ConvertStringToContactViewPreferencesL(const TDesC& aViewPreferences);
	CContactViewBase::TSearchType	GetSearchType(const TDesC& aSearchType);
	CContactGroupView::TGroupType	GetGroupType(const TDesC& aGroupType);
	CContactDatabase::TContactViewFilter	GetContactViewFilterL(const TDesC& aFilter);
	void ConstructViewsL();
	TUid CContactUtilitiesCollection::GetContactFieldType(const TDesC& aContactFieldString);
	void WaitForViewReady(CContactViewEventQueue*&	aViewEventQueue);
	void ImportContactsL();	
	

private:
	CTestStep& iBaseTestStep;
	CContactDatabase& iDb;
	CContactViewCollection& iViewCollection;
	};


// CContactViewCollection
class CContactViewCollection : public CBase
	{
public:
	static CContactViewCollection* NewL();
	~CContactViewCollection();

private:
	CContactViewCollection();

public:
	RPointerArray<CContactLocalView>&	GetLocalViewArray();
	RPointerArray<CContactRemoteView>&	GetRemoteViewArray();
	RPointerArray<CContactNamedRemoteView>&	GetNamedRemoteViewArray();
	RPointerArray<CContactFindView>&	GetFindViewArray();
	RPointerArray<CContactFilteredView>&	GetFilteredViewArray();
	RPointerArray<CContactGroupView>&	GetGroupViewArray();
	RPointerArray<CContactConcatenatedView>&	GetConcatenatedViewArray();
	RPointerArray<CContactSubView>&	GetSubViewArray();
	RPointerArray<CContactViewBase>&	GetComponentViews();
	RPointerArray<CContactViewEventQueue>&	GetContactViewEventsQueueL(const TDesC& aTypeOfView);
	CContactLocalView*	GetDesiredLocalView(TInt indexOfView);
	CContactRemoteView*	GetDesiredRemoteView(TInt indexOfView);
	CContactNamedRemoteView*	GetDesiredNamedRemoteView(TInt indexOfView);
	CContactFindView*	GetDesiredFindView(TInt indexOfView);
	CContactFilteredView*	GetDesiredFilteredView(TInt indexOfView);
	CContactGroupView*	GetDesiredGroupView(TInt indexOfView);
	CContactConcatenatedView*	GetDesiredConcatenatedView(TInt indexOfView);
	CContactSubView*	GetDesiredSubView(TInt indexOfView);
	CContactViewEventQueue*	GetDesiredViewEventL(TInt indexOfView, const TDesC& aTypeOfView);
	void CloseLocalViewAndEventQueue();
	void CloseRemoteViewAndEventQueue();
	void CloseNamedRemoteViewAndEventQueue();
	void CloseFindViewAndEventQueue();
	void CloseFilteredViewAndEventQueue();
	void CloseGroupViewAndEventQueue();
	void CloseConcatenatedViewAndEventQueue();
	void CloseSubViewAndEventQueue();

	CContactViewEventQueue& GetDesiredViewObserver(const TDesC& aTypeOfView, const TInt aViewIndex);
	CContactViewBase* GetDesiredView(const TDesC& aTypeOfView, const TInt aViewIndex);
	CContactViewEventQueue& RetrieveDesiredViewObserverL(CTestStep& aTestStepReference);
	

public:
	RPointerArray<CContactLocalView>	iArrayOfLocalViews;
	RPointerArray<CContactRemoteView>	iArrayOfRemoteViews;
	RPointerArray<CContactNamedRemoteView>	iArrayOfNamedRemoteViews;
	RPointerArray<CContactFindView>	iArrayOfFindViews;
	RPointerArray<CContactFilteredView>	iArrayOfFilteredViews;
	RPointerArray<CContactGroupView>	iArrayOfGroupViews;
	RPointerArray<CContactConcatenatedView>	iArrayOfConcatenatedViews;
	RPointerArray<CContactSubView>	iArrayOfSubViews;
	RPointerArray<CContactViewBase>	iArrayOfViewBase;
	RPointerArray<CContactViewEventQueue>	iArrayOfLocalViewEvents;
	RPointerArray<CContactViewEventQueue>	iArrayOfRemoteViewEvents;
	RPointerArray<CContactViewEventQueue>	iArrayOfNamedRemoteViewEvents;
	RPointerArray<CContactViewEventQueue>	iArrayOfGroupViewEvents;
	RPointerArray<CContactViewEventQueue>	iArrayOfFindViewEvents;
	RPointerArray<CContactViewEventQueue>	iArrayOfFilteredViewEvents;
	RPointerArray<CContactViewEventQueue>	iArrayOfConcatenatedViewEvents;
	RPointerArray<CContactViewEventQueue>	iArrayOfSubViewEvents;
	RPointerArray<CContactViewEventQueue>	iArrayViewEvents;

	};


// CContactViewApiTest
class CContactViewApiTest : public CBase
	{
public:

	static CContactViewApiTest* CContactViewApiTest::NewL(CTestStep* aTestStep);
	~CContactViewApiTest();

private:
	CContactViewApiTest(CTestStep* aTestStep);

public:
	CTestStep&	TestStepReference();
	RFs&	GetFsSessionL();
	TInt	GetAtLApiDataFromIni(const TDesC& aSection);
	TInt	GetContactAtLApiDataFromIni(const TDesC& aSection);
	TInt	GetFindLApiDataFromIni(const TPtrC& aSection);
	TInt	FindL(TContactItemId aId, CContactViewBase* aView, const TDesC& aErrorCondition);
	void 	TestChangeSortOrderL(const TDesC& aSection, CContactUtilitiesCollection* aContactUtility);
	void	RefineFindViewL(const TDesC& aSearchWords, CContactFindView* aView,
								CContactUtilitiesCollection* aContactUtility);
	void	ValidateErrorCondition(TInt aErr, const TDesC& aErrorCondition);
	void	ValidateResult(TInt aActualResult, TInt aDesiredResult);
	#ifdef _DEBUG
	void	SetFsErrorConditionL(const TDesC& aErrorCondition, TInt aCount = 0);
	#endif
	HBufC*	GetAllFieldsLDataFromIniL(const TDesC& aSection, CContactViewBase* aView);
	HBufC*	GetAllFieldsL(TInt aIndex, const TDesC &aSeparator, CContactViewBase* aView);
	CViewContact* ContactAtL(TInt aIndex, CContactViewBase* aView);
	TContactItemId	AtL(TInt aIndex, CContactViewBase* aView, const TDesC& aErrorCondition);
	RContactViewSortOrder	GetSortOrderL(CContactViewBase* aView, const TDesC& aErrorCondition);
	TContactViewPreferences	ContactViewPreferences(CContactViewBase* aView);
	RPointerArray<CViewContact>	GetContactsMatchingCriteriaL(const TDesC& aSearchWords, CContactViewBase* aView,
															CContactUtilitiesCollection* aContactUtility, const TDesC& aErrorCondition);
	RPointerArray<CViewContact> GetContactsMatchingPrefixL(const TDesC& aSearchWords, CContactViewBase* aView,
															CContactUtilitiesCollection* aContactUtility, const TDesC& aErrorCondition);

private:
	CTestStep* iTestStep;
	RFs	iFsSession;
	};


// CContactViewValidation
class CContactViewValidation : public CBase
	{
public:
	CContactViewValidation(CTestStep* aTestStep);
	~CContactViewValidation();

public:
	CTestStep&	TestStepReference();
	void	ValidateAtLApi(TContactItemId aContactItemId, const TDesC& aSection);
	void	ValidateContactAtLApi(CViewContact* aViewContact, const TDesC& aSection);
	void	ValidateCountL(const TDesC& aSection, TInt aCount);
	void	ValidateFindL(const TDesC& aSection, TInt aIndex);
	void	ValidateAllFieldsL(const TDesC& aSection, RArray<TPtrC> aRetrievedField, CContactViewBase* aView,
								CContactViewApiTest* aViewApiTest);
	void	ValidateViewPreferencesL(const TDesC& aSection, TContactViewPreferences& viewPreferences,
									CContactUtilitiesCollection* aContactUtility);
	void	ValidateSortOrderL(const TUid& aUid, const TUid& aExpectedUid);

private:
	CTestStep* iTestStep;
	};

#endif

