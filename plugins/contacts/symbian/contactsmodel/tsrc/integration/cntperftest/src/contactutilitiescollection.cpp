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
*
*/

#include "contactutilitiescollection.h"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "cntdb_internal.h"
#endif

/**
 * Destructor
 */
CContactUtilitiesCollection::~CContactUtilitiesCollection()
	{
	}


/**
 * Constructor
 */
CContactUtilitiesCollection* CContactUtilitiesCollection::NewL(CTestStep& aTestStep, CContactDatabase& aDb, CContactViewCollection& aViewCollection)
	{
	CContactUtilitiesCollection* self = new(ELeave) CContactUtilitiesCollection(aTestStep, aDb, aViewCollection);
    return self;
	}

/**
 * Return database reference
 * @return CContactDatabase&
 */
CContactDatabase& CContactUtilitiesCollection::DatabaseReference()
	{
	return iDb;
	}

/**
 * Return viewcollection reference
 * @return CContactViewCollection&
 */
CContactViewCollection& CContactUtilitiesCollection::ViewCollectionReference()
	{
	return iViewCollection;
	}


/**
 * Constructor
 */
CContactUtilitiesCollection::CContactUtilitiesCollection(CTestStep& aTestStep, CContactDatabase& aDb, CContactViewCollection& aViewCollection)
: iBaseTestStep(aTestStep), iDb(aDb), iViewCollection(aViewCollection)
	{
	}

/**
 * Gets a reference to the base test step. This is needed to read from the ini file
 * @return CTestStep&
 */
CTestStep& CContactUtilitiesCollection::BaseTestStepReference()
	{
	return iBaseTestStep;
	}

/**
* Get the data required for the construction of the local view from the ini file and call the function
* ConstructLocalViewL to construct a local view Object.
* @param aSection ini section
*/
void CContactUtilitiesCollection::GetDataAndConstructLocalViewL(const TDesC& aSection)
	{
	TPtrC	viewSortOrder;
	TPtrC	viewPreferences;
	TPtrC	localViewSortPlugin;
	TBool 	viewNotReady = EFalse;
	BaseTestStepReference().GetBoolFromConfig(aSection, KViewNotReady, viewNotReady);
	BaseTestStepReference().GetStringFromConfig(aSection, KViewSortOrder, viewSortOrder);
	BaseTestStepReference().GetStringFromConfig(aSection, KViewPreferences, viewPreferences);
	BaseTestStepReference().GetStringFromConfig(aSection, KLocalViewSortPlugin, localViewSortPlugin);

	// Construct the local view by passing the input data required for the construction
	ConstructLocalViewL(viewSortOrder, viewPreferences, localViewSortPlugin, viewNotReady);
	}

/*
* Allocate and construct a local view object based on the sortorder, view preferences, sortplug-in given from the
* ini file
* @param aSortOrder specifies the fields to use to sort the items in the view
* @param aViewPreferences specifies the type of the contact item to be included in the view
* @param aSortPlugin specifies a plug-in that will be used to compare view contacts when the view is sorted
*/
void CContactUtilitiesCollection::ConstructLocalViewL(const TDesC& aSortOrder, const TDesC& aViewPreferences, const TDesC& aSortPlugin,
													TBool aViewNotReady)
	{
	// Get Db and view collection reference
	CContactDatabase& cDb = DatabaseReference();
	CContactViewCollection& cViewCollection = ViewCollectionReference();
	// Construct a view event queue object
	CContactViewEventQueue*	viewEventQueue = CContactViewEventQueue::NewL();
	CleanupStack::PushL(viewEventQueue);

	TContactViewPreferences	viewPreferences = ConvertStringToContactViewPreferencesL(aViewPreferences);
	RContactViewSortOrder	sortOrder = ConvertStringToSortOrderL(aSortOrder);
	CContactLocalView*	localView = NULL;
	// if sortplugin is specified in the ini, then create a local view object which contains sort plugin details.
	if ( aSortPlugin != KNullDesC() )
		{
		HBufC8*	sortPlugin = HBufC8::NewLC(aSortPlugin.Length());
		sortPlugin->Des().Copy(aSortPlugin);
		TRAPD( err, localView = CContactLocalView::NewL(*viewEventQueue, cDb, sortOrder, viewPreferences, sortPlugin->Des()));
		if( err != KErrNone )
			{
			BaseTestStepReference().ERR_PRINTF2(KErrSortPlugin, err);
			BaseTestStepReference().SetTestStepError(err);
			aViewNotReady = TRUE;
			}
		CleanupStack::Pop(sortPlugin);
		}
	else
		{
		localView = CContactLocalView::NewL(*viewEventQueue, cDb, sortOrder, viewPreferences);
		}

	CleanupStack::PushL(localView);
	if(!aViewNotReady)
		{
		WaitForViewReady(viewEventQueue);
		}
	if(localView!=NULL)
		{
		(cViewCollection.iArrayOfLocalViews).AppendL(localView);
		(cViewCollection.iArrayOfLocalViewEvents).AppendL(viewEventQueue);
		}
	CleanupStack::Pop(localView);
	CleanupStack::Pop(viewEventQueue);
	}

/*
* Get the data required for the construction of the remote view from the ini file and call the function
* ConstructRemoteViewL to construct a remote view Object.
* @param aSection ini section name
*/
void CContactUtilitiesCollection::GetDataAndConstructRemoteViewL(const TDesC& aSection)
	{
	TPtrC	viewSortOrder;
	TPtrC	viewPreferences;
	TPtrC	remoteViewSortPlugin;
	TBool 	viewNotReady = EFalse;
	BaseTestStepReference().GetBoolFromConfig(aSection, KViewNotReady, viewNotReady);
	BaseTestStepReference().GetStringFromConfig(aSection, KViewSortOrder, viewSortOrder);
	BaseTestStepReference().GetStringFromConfig(aSection, KViewPreferences, viewPreferences);
	BaseTestStepReference().GetStringFromConfig(aSection, KRemoteViewSortPlugin, remoteViewSortPlugin);

	// Construct remote view by passing the data required for the construction
	ConstructRemoteViewL(viewSortOrder, viewPreferences, remoteViewSortPlugin, viewNotReady);
	}

/*
* Allocate and construct a remote view object based on the sortorder, view preferences, sortplug-in given from the
* ini file
* @param aSortOrder Specifies the fields to use to sort the items in the view
* aram aViewPreferences Specifies the type of the contact item to be included in the view
* aram aSortPlugin Specifies a plug-in that will be used to compare view contacts when the view is sorted
*/
void CContactUtilitiesCollection::ConstructRemoteViewL(const TDesC& aSortOrder, const TDesC& aViewPreferences, const TDesC& aSortPlugin,
								TBool aViewNotReady)
	{
	TContactViewPreferences	viewPreferences = ConvertStringToContactViewPreferencesL(aViewPreferences);
	RContactViewSortOrder	sortOrder = ConvertStringToSortOrderL(aSortOrder);
	CContactViewEventQueue*	viewEventQueue = CContactViewEventQueue::NewL();
	CleanupStack::PushL(viewEventQueue);

	CContactRemoteView*	remoteView = NULL;

	CContactDatabase& cDb = DatabaseReference();
	CContactViewCollection& cViewCollection = ViewCollectionReference();
	if ( aSortPlugin != KNullDesC() )
		{
		HBufC8*	sortPlugin = HBufC8::NewLC(aSortPlugin.Length());
		sortPlugin->Des().Copy(aSortPlugin);
		// Construct a remote view which contains sort plugin details.
		TRAPD( err, remoteView = CContactRemoteView::NewL(*viewEventQueue, cDb, sortOrder, viewPreferences, sortPlugin->Des()));
		CleanupStack::Pop(sortPlugin);
		if( err != KErrNone )
			{
			BaseTestStepReference().ERR_PRINTF2(KErrSortPlugin, err);
			BaseTestStepReference().SetTestStepError(err);
			aViewNotReady = TRUE;
			}
		}
	else
		{
		remoteView = CContactRemoteView::NewL(*viewEventQueue, cDb, sortOrder, viewPreferences);
		}
	CleanupStack::PushL(remoteView);

	if(!aViewNotReady)
		{
		WaitForViewReady(viewEventQueue);
		}

	// Append the remote view and remote view events in the array which will be used for testing
	(cViewCollection.iArrayOfRemoteViewEvents).AppendL(viewEventQueue);
	(cViewCollection.iArrayOfRemoteViews).AppendL(remoteView);
	CleanupStack::Pop(remoteView);
	CleanupStack::Pop(viewEventQueue);
	}

/*
* Get the data required for the construction of the named remote view from the ini file and call the function
* ConstructNamedRemoteView to construct a named remote view Object.
* @param aSection ini section name
*/
void CContactUtilitiesCollection::GetDataAndConstructNamedRemoteViewL(const TDesC& aSection)
	{
	TPtrC	name;
	TPtrC	viewSortOrder;
	TPtrC	viewPreferences;
	TPtrC	namedViewSortPlugin;
	TBool 	viewNotReady = EFalse;
	BaseTestStepReference().GetBoolFromConfig(aSection, KViewNotReady, viewNotReady);
	BaseTestStepReference().GetStringFromConfig(aSection, KNamedRemoteViewName, name);
	BaseTestStepReference().GetStringFromConfig(aSection, KViewSortOrder, viewSortOrder);
	BaseTestStepReference().GetStringFromConfig(aSection, KViewPreferences, viewPreferences);
	BaseTestStepReference().GetStringFromConfig(aSection, KNamedViewSortPlugin, namedViewSortPlugin);

	// Construct a named remote view by passing the data required for the construction
	ConstructNamedRemoteViewL(name, viewSortOrder, viewPreferences, namedViewSortPlugin, viewNotReady);
	}

/*
* Allocate and construct a named remote view object based on the name, sortorder, view preferences, sortplug-in given from the
* ini file
* @param aName The name of the view object in the server
* @param aSortOrder specifies the fields to use to sort the items in the view
* @param aViewPreferences specifies the type of the contact item to be included in the view
* @param aSortPlugin specifies a plug-in that will be used to compare view contacts when the view is sorted
*/
void CContactUtilitiesCollection::ConstructNamedRemoteViewL(const TDesC& aName, const TDesC& aSortOrder, const TDesC& aViewPreferences,
														const TDesC& aSortPlugin, TBool aViewNotReady)
	{
	TContactViewPreferences	viewPreferences = ConvertStringToContactViewPreferencesL(aViewPreferences);
	RContactViewSortOrder	sortOrder = ConvertStringToSortOrderL(aSortOrder);
	CContactViewEventQueue*	viewEventQueue = CContactViewEventQueue::NewL();
	CleanupStack::PushL(viewEventQueue);
	HBufC*	name = HBufC::NewLC(aName.Length());
	name->Des().Copy(aName);
	CContactNamedRemoteView* namedRemoteView = NULL;

	CContactDatabase& cDb = DatabaseReference();
	CContactViewCollection& cViewCollection = ViewCollectionReference();

	if ( aSortPlugin != KNullDesC() )
		{
		HBufC8*	sortPlugin = HBufC8::NewLC(aSortPlugin.Length());
		sortPlugin->Des().Copy(aSortPlugin);
		// Construct a named remote view which contains sort plug-in details
		TRAPD( err, namedRemoteView = CContactNamedRemoteView::NewL(*viewEventQueue, name->Des(), cDb, sortOrder, viewPreferences, sortPlugin->Des()));
		CleanupStack::PopAndDestroy(sortPlugin);
		if( err != KErrNone )
			{
			BaseTestStepReference().ERR_PRINTF2(KErrSortPlugin, err);
			BaseTestStepReference().SetTestStepError(err);
			aViewNotReady = TRUE;
			}
		}
	else
		{
		namedRemoteView = CContactNamedRemoteView::NewL(*viewEventQueue, name->Des(), cDb, sortOrder, viewPreferences);
		}
	CleanupStack::PushL(namedRemoteView);

	// If aViewNotReady is set to true then the view is not ready
	if(!aViewNotReady)
		{
		WaitForViewReady(viewEventQueue);
		}
	if(namedRemoteView != NULL)
		{
		(cViewCollection.iArrayOfNamedRemoteViewEvents).AppendL(viewEventQueue);
		// append the named remote view and named remote view events to the array which will be used for testing
		(cViewCollection.iArrayOfNamedRemoteViews).AppendL(namedRemoteView);
		}
	CleanupStack::Pop(namedRemoteView);
	CleanupStack::PopAndDestroy(name);
	CleanupStack::Pop(viewEventQueue);

	}

/*
* Get the data required for the construction of the find view from the ini file and call the function
* ConstructFindView to construct a find view Object.
* @param aSection ini section name
*/
void CContactUtilitiesCollection::GetDataAndConstructFindViewL(const TDesC& aSection)
	{
	TPtrC	searchWords;
	TPtrC	searchType;
	TBool 	viewNotReady = EFalse;
	BaseTestStepReference().GetBoolFromConfig(aSection, KViewNotReady, viewNotReady);
	BaseTestStepReference().GetStringFromConfig(aSection, KSearchWords, searchWords);
	BaseTestStepReference().GetStringFromConfig(aSection, KSearchType, searchType);

	// Get the underlying required view for the construction of find view
	CContactViewBase*	viewBase = GetDesiredView(aSection);

	// Construct a find view by passing the data required for the construction
	ConstructFindViewL(searchWords, searchType, viewBase, viewNotReady);
	}

/*
* Allocate and constructs a find view object based on the search words and the search type given from the
* ini file
* @param aSearchWords The search terms
* @param aSortPlugin specifies a plug-in that will be used to compare view contacts when the view is sorted
* @param aSearchType The search type
* @param aViewBase underlying view
*/
void CContactUtilitiesCollection::ConstructFindViewL(const TDesC& aSearchWords, const TDesC& aSearchType,
													CContactViewBase* aViewBase, TBool aViewNotReady)
	{
	RArray<TPtrC>	searchList;
	CleanupClosePushL(searchList);
	TokenizeStringL(aSearchWords, searchList);

	// allocate an array to store the search words
	CPtrCArray*	findWords = new(ELeave) CPtrCArray(3);
	CleanupStack::PushL(findWords);
	for ( TInt i = 0; i < searchList.Count(); ++i )
		{
		findWords->AppendL(searchList[i]);
		}

	CContactViewBase::TSearchType	searchType = GetSearchType(aSearchType);
	// Construct an view event queue object
	CContactViewEventQueue*	viewEventQueue = CContactViewEventQueue::NewL();
	CleanupStack::PushL(viewEventQueue);
	CContactFindView*	findView;
	CContactDatabase& cDb = DatabaseReference();
	CContactViewCollection& cViewCollection = ViewCollectionReference();
	if ( aSearchType != KNullDesC() )
		{
		// Construct a find view which contains search type
		findView = CContactFindView::NewL(cDb, *aViewBase, *viewEventQueue, findWords, searchType);
		}
	else
		{
		findView = CContactFindView::NewL(cDb, *aViewBase, *viewEventQueue, findWords);
		}
	CleanupStack::PushL(findView);

	// If aViewNotReady is set to true then the view is not ready
	if(!aViewNotReady)
		{
		WaitForViewReady(viewEventQueue);
		}

	// appends the find view and find view events to the array which will be used for testing
	if(findView != NULL)
		{
		(cViewCollection.iArrayOfFindViewEvents).AppendL(viewEventQueue);
		(cViewCollection.iArrayOfFindViews).AppendL(findView);
		}
	CleanupStack::Pop(findView);
	CleanupStack::Pop(viewEventQueue);
	CleanupStack::PopAndDestroy(findWords);
	CleanupStack::PopAndDestroy(&searchList);
	}

/*
* Get the data required for the construction of the local view from the ini file and call the function
* ConstructGroupView to construct a group view Object.
* @param aSection ini section name
*/
void CContactUtilitiesCollection::GetDataAndConstructGroupViewL(const TDesC& aSection)
	{
	TPtrC	groupType;
	TPtrC	groupName;
	TInt	groupId = -1;
	TBool 	viewNotReady = EFalse;
	TInt    groupViewVersion = 0;
	BaseTestStepReference().GetBoolFromConfig(aSection, KViewNotReady, viewNotReady);
	BaseTestStepReference().GetStringFromConfig(aSection, KGroupType, groupType);
	BaseTestStepReference().GetStringFromConfig(aSection, KGroupName, groupName);
	BaseTestStepReference().GetIntFromConfig(aSection, KGroupId, groupId);
	BaseTestStepReference().GetIntFromConfig(aSection, KGroupVersion, groupViewVersion);

	// Get the underlying view for the construction of group view
	CContactViewBase*	viewBase = GetDesiredView(aSection);

	// Construct the group view by passing the data required for the construction
	ConstructGroupViewL(groupType, groupName, groupId, viewBase, viewNotReady, groupViewVersion);
	}

/*
* Allocate and construct a group view object based on the grouptype, groupid and groupname given from the
* ini file
* @param aGroupType The group Type
* @param aGroupName the group name
* @param aGroupId The Id of the contact group
* @param aViewBase underlying view
*/
void CContactUtilitiesCollection::ConstructGroupViewL(const TDesC& aGroupType, const TDesC& aGroupName, const TInt& aGroupId,
													CContactViewBase* aViewBase, TBool aViewNotReady, TInt& aGroupViewVersion)
	{
	// Get the group type required for the construction of group view
	CContactGroupView::TGroupType	groupType = GetGroupType(aGroupType);
	CContactViewEventQueue*	viewEventQueue = CContactViewEventQueue::NewL();
	CleanupStack::PushL(viewEventQueue);
	CContactGroupView*	groupView = NULL;
	CContactDatabase& cDb = DatabaseReference();
	CContactViewCollection& cViewCollection = ViewCollectionReference();
	//Construct a group view using version 1
	if(aGroupViewVersion == 1)
		{
		if( aGroupName != KNullDesC() )
			{
			// Construct a group view which contains the details of group name
			TRAPD(err, groupView = CContactGroupView::NewL(cDb, *aViewBase, *viewEventQueue, aGroupName, groupType));
			if( err != KErrNone)
				{
				BaseTestStepReference().SetTestStepError(err);
				BaseTestStepReference().SetTestStepResult(EFail);
				aViewNotReady = ETrue;
				}
			}
		if(aGroupId > 0)
			{
			TRAPD(err, groupView = CContactGroupView::NewL(cDb, *aViewBase, *viewEventQueue, aGroupId, groupType));
			if( err != KErrNone)
				{
				BaseTestStepReference().SetTestStepError(err);
				BaseTestStepReference().SetTestStepResult(EFail);
				aViewNotReady = ETrue;
				}
			}
		}
	else
		{//Construct a group view using version 2
		if( aGroupName != KNullDesC() )
			{
			// Construct a group view which contains the details of group name
			TRAPD(err, groupView =CContactGroupView::NewL(*aViewBase, cDb, *viewEventQueue, aGroupName, groupType));
			if( err != KErrNone)
				{
				BaseTestStepReference().SetTestStepError(err);
				BaseTestStepReference().SetTestStepResult(EFail);
				aViewNotReady = ETrue;
				}
			}
		if(aGroupId > 0)
			{
			TRAPD(err, groupView = CContactGroupView::NewL(*aViewBase, cDb, *viewEventQueue, aGroupId, groupType));
			if( err != KErrNone)
				{
				BaseTestStepReference().SetTestStepError(err);
				BaseTestStepReference().SetTestStepResult(EFail);
				aViewNotReady = ETrue;
				}
			}

		}

	CleanupStack::PushL(groupView);

    if(!aViewNotReady)
		{
		WaitForViewReady(viewEventQueue);
		}
	// append the group view to the array which will be used for multiple view access tests
	if( groupView != NULL )
		{
		(cViewCollection.iArrayOfGroupViews).AppendL(groupView);
		(cViewCollection.iArrayOfGroupViewEvents).AppendL(viewEventQueue);
		}
	CleanupStack::Pop(groupView);
	CleanupStack::Pop(viewEventQueue);
	}

/*
* Get the data required for the construction of the local view from the ini file and call the function
* ConstructFilteredView to construct a filtered view Object.
* @param aSection ini section name
*/
void CContactUtilitiesCollection::GetDataAndConstructFilteredViewL(const TDesC& aSection)
	{
	TPtrC	viewFilter;
	TBool 	viewNotReady = EFalse;
	BaseTestStepReference().GetBoolFromConfig(aSection, KViewNotReady, viewNotReady);
	BaseTestStepReference().GetStringFromConfig(aSection, KViewFilter, viewFilter);
	// Get the underlying view required for the construction of filtered view
	CContactViewBase*	viewBase = GetDesiredView(aSection);

	// construct the filtered view by passing the data required for the construction
	ConstructFilteredViewL(viewFilter, viewBase, viewNotReady);
	}

/*
* Allocate and constructs a filter view object based on the filter type given from the
* ini file
* @param aViewFilter The filter to use.
* @param aViewBase underlying view
*/
void CContactUtilitiesCollection::ConstructFilteredViewL(const TDesC& aViewFilter, CContactViewBase* aViewBase, TBool aViewNotReady)
	{
	// Get the filter type required for the construction of filtered view
	CContactDatabase::TContactViewFilter	viewFilter = GetContactViewFilterL(aViewFilter);
	CContactViewEventQueue*	viewEventQueue = CContactViewEventQueue::NewL();
	CleanupStack::PushL(viewEventQueue);
	CContactDatabase& cDb = DatabaseReference();
	CContactViewCollection& cViewCollection = ViewCollectionReference();
	CContactFilteredView*	filteredView = CContactFilteredView::NewL(*viewEventQueue, cDb, *aViewBase, viewFilter);
	CleanupStack::PushL(filteredView);

	if(!aViewNotReady)
		{
		WaitForViewReady(viewEventQueue);
		}
	// append the filtered view and its events to the array which will be used for testing
	if(filteredView !=NULL)
		{
		(cViewCollection.iArrayOfFilteredViews).AppendL(filteredView);
		(cViewCollection.iArrayOfFilteredViewEvents).AppendL(viewEventQueue);
		}
	CleanupStack::Pop(filteredView);
	CleanupStack::Pop(viewEventQueue);
	}

/*
* Get the data required for the construction of the local view from the ini file and call the function
* ConstructConcatenatedView to construct a concatenated view Object.
* @param aSection ini section
*/
void CContactUtilitiesCollection::ConstructConcatenatedViewL(const TDesC& aSection)
	{
	// Get the component views required for the construction of concatenated views
	RPointerArray<CContactViewBase>& arrayOfViews = GetComponentViewsL(aSection);
	CleanupClosePushL(arrayOfViews);
	CContactViewEventQueue*	viewEventQueue = CContactViewEventQueue::NewL();
	CleanupStack::PushL(viewEventQueue);

	CContactDatabase& cDb = DatabaseReference();
	CContactViewCollection& cViewCollection = ViewCollectionReference();

	CContactConcatenatedView* concatenatedView = CContactConcatenatedView::NewL(*viewEventQueue, cDb, arrayOfViews);
	CleanupStack::PushL(concatenatedView);

	TBool 	viewNotReady = EFalse;
	BaseTestStepReference().GetBoolFromConfig(aSection, KViewNotReady, viewNotReady);

	if(!viewNotReady)
		{
		WaitForViewReady(viewEventQueue);
		}
	if(concatenatedView != NULL)
		{
		// append the concatenated view to the array which will be used for multiple view access tests
		(cViewCollection.iArrayOfConcatenatedViews).AppendL(concatenatedView);
		(cViewCollection.iArrayOfConcatenatedViewEvents).AppendL(viewEventQueue);
		}
	CleanupStack::Pop(concatenatedView);
	CleanupStack::Pop(viewEventQueue);
	CleanupStack::PopAndDestroy(&arrayOfViews);

	}

/*
* Get the data required for the construction of the local view from the ini file and call the function
* ConstructSubView to construct a sub view Object.
* @param aSection ini section name
*/
void CContactUtilitiesCollection::GetDataAndConstructSubViewL(const TDesC& aSection)
	{
	TPtrC	lowerBoundary;
	TPtrC	higherBoundary;
	TPtrC	boundary;
	TBool 	viewNotReady = EFalse;
	BaseTestStepReference().GetBoolFromConfig(aSection, KViewNotReady, viewNotReady);
	BaseTestStepReference().GetStringFromConfig(aSection, KLowerBoundary, lowerBoundary);
	BaseTestStepReference().GetStringFromConfig(aSection, KHigherBoundary, higherBoundary);
	BaseTestStepReference().GetStringFromConfig(aSection, KBoundary, boundary);

	// Get the underlying view required for the construction of the sub view
	CContactViewBase*	viewBase = GetDesiredView(aSection);
	// Construct the sub view by passing the data from the ini file
	ConstructSubViewL(lowerBoundary, higherBoundary, boundary, viewBase, viewNotReady);
	}

/*
* Allocate and constructs a sub view object based on the lower and higher boundary specified in the
* ini file
* @param aLowerBoundary A string containing the sub view's lower boundary criteria
* @param aHigherBoundary A string containing the sub view's higher boundary criteria
* @param aBoundary A string containing the sub-view criteria
* @param aViewBase underlying view
*/
void CContactUtilitiesCollection::ConstructSubViewL(const TDesC& aLowerBoundary, const TDesC& aHigherBoundary, const TDesC& aBoundary,
													CContactViewBase* aViewBase, TBool aViewNotReady)
	{
	CContactViewEventQueue*	viewEventQueue = CContactViewEventQueue::NewL();
	CleanupStack::PushL(viewEventQueue);
	CContactSubView*	subView;
	CContactDatabase& cDb = DatabaseReference();
	CContactViewCollection& cViewCollection = ViewCollectionReference();

	if ( aLowerBoundary != KNullDesC() || aHigherBoundary != KNullDesC() )
		{
		// Construct a sub view which contains both the higher and lower boundary
		subView = CContactSubView::NewL(*viewEventQueue, cDb, *aViewBase, aLowerBoundary, aHigherBoundary);
		}
	else
		{
		subView = CContactSubView::NewL(*viewEventQueue, cDb, *aViewBase, aBoundary);
		}
	CleanupStack::PushL(subView);

	if(!aViewNotReady)
		{
		WaitForViewReady(viewEventQueue);
		}
	if(subView != NULL)
		{
		// append the sub view to the array which will be used for multiple view access tests
		(cViewCollection.iArrayOfSubViews).AppendL(subView);
		(cViewCollection.iArrayOfSubViewEvents).AppendL(viewEventQueue);
		}
	CleanupStack::Pop(subView);
	CleanupStack::Pop(viewEventQueue);
	}


/*
* Get the underlying view type and index required for the views like find view, filtered view, group view,
* and sub view from the ini file and get the view of the specified type for that particular index
* @param aSection ini section
* @return CContactViewBase*
*/
CContactViewBase* CContactUtilitiesCollection::GetDesiredView(const TDesC& aSection)
	{
	TPtrC	viewBase;
	TInt	index = -1;
	BaseTestStepReference().GetStringFromConfig(aSection, KViewBase, viewBase);
	BaseTestStepReference().GetIntFromConfig(aSection, KIndexOfViews, index);

	CContactViewCollection& cViewCollection = ViewCollectionReference();
	CContactViewBase*	contactViewBase = NULL;
	// Based on the view type specified in the ini file, get the underlying view for the specified index
	if ( viewBase.Compare(KLocalView) == 0 )
		{
		contactViewBase = cViewCollection.GetDesiredLocalView(index);
		}
	else if ( viewBase.Compare(KRemoteView) == 0 )
		{
		contactViewBase = cViewCollection.GetDesiredRemoteView(index);
		}
	else if ( viewBase.Compare(KNamedRemoteView) == 0 )
		{
		contactViewBase = cViewCollection.GetDesiredNamedRemoteView(index);
		}
	else if ( viewBase.Compare(KFindView) == 0 )
		{
		contactViewBase = cViewCollection.GetDesiredFindView(index);
		}
	else if ( viewBase.Compare(KFilteredView) == 0 )
		{
		contactViewBase = cViewCollection.GetDesiredFilteredView(index);
		}
	else if ( viewBase.Compare(KGroupView) == 0 )
		{
		contactViewBase = cViewCollection.GetDesiredGroupView(index);
		}
	else if ( viewBase.Compare(KConcatenatedView) == 0 )
		{
		contactViewBase= cViewCollection.GetDesiredConcatenatedView(index);
		}
	else if ( viewBase.Compare(KSubView) == 0 )
		{
		contactViewBase = cViewCollection.GetDesiredSubView(index);
		}
	return contactViewBase;
	}

/*
* Get the component views required for construction of the concatenated view. The component views can be of any view
* type like local view, remote view, named remote view, find view, filtered view, group view and sub view
* @param aSection ini section
* @return RPointerArray<CContactViewBase>&
*/
RPointerArray<CContactViewBase>& CContactUtilitiesCollection::GetComponentViewsL(const TDesC& aSection)
	{
	TPtrC	componentViews;
	BaseTestStepReference().GetStringFromConfig(aSection, KComponentViews, componentViews);
	CContactViewCollection& cViewCollection = ViewCollectionReference();
	TBuf<KMaxLengthString>	field;
	field.Zero();
	field.Append(KIndexOfView);
	TInt	indexOfView = -1;

	RArray<TPtrC>	views;
	CleanupClosePushL(views);
	TokenizeStringL(componentViews, views);
	// Iterate through the list of component views given in the ini file
	for ( TInt i = 0; i < views.Count(); ++i )
		{
		field.AppendNum(i);
		BaseTestStepReference().GetIntFromConfig(aSection, field, indexOfView);
		CContactViewBase*	viewBase;
		// Append the view in the array iArrayofViewBase which is of type CContactViewBase
		if ( views[i].Compare(KLocalView) == 0 )
			{
			viewBase = cViewCollection.GetDesiredLocalView(indexOfView);
			(cViewCollection.iArrayOfViewBase).AppendL(viewBase);
			}
		else if ( views[i].Compare(KRemoteView) == 0 )
			{
			viewBase = cViewCollection.GetDesiredRemoteView(indexOfView);
			(cViewCollection.iArrayOfViewBase).AppendL(viewBase);
			}
		else if ( views[i].Compare(KNamedRemoteView) == 0 )
			{
			viewBase = cViewCollection.GetDesiredNamedRemoteView(indexOfView);
			(cViewCollection.iArrayOfViewBase).AppendL(viewBase);
			}
		else if ( views[i].Compare(KFindView) == 0 )
			{
			viewBase = cViewCollection.GetDesiredFindView(indexOfView);
			(cViewCollection.iArrayOfViewBase).AppendL(viewBase);
			}
		else if ( views[i].Compare(KFilteredView) == 0 )
			{
			viewBase = cViewCollection.GetDesiredFilteredView(indexOfView);
			(cViewCollection.iArrayOfViewBase).AppendL(viewBase);
			}
		else if ( views[i].Compare(KGroupView) == 0 )
			{
			viewBase = cViewCollection.GetDesiredGroupView(indexOfView);
			(cViewCollection.iArrayOfViewBase).AppendL(viewBase);
			}
		else if ( views[i].Compare(KConcatenatedView) == 0 )
			{
			viewBase = cViewCollection.GetDesiredConcatenatedView(indexOfView);
			(cViewCollection.iArrayOfViewBase).AppendL(viewBase);
			}
		else if ( views[i].Compare(KSubView) == 0 )
			{
			viewBase = cViewCollection.GetDesiredSubView(indexOfView);
			(cViewCollection.iArrayOfViewBase).AppendL(viewBase);
			}
		}
	CleanupStack::PopAndDestroy(&views);
	return (cViewCollection.iArrayOfViewBase);
	}

/*
* Delete the database based on the boolean variable set in the ini file
* @param aDeleteDataBase Boolean value that decides to delete the database
* @param aDbName database name
*/
void CContactUtilitiesCollection::DeleteDataBase(TBool aDeleteDataBase, const TDesC& aDbName)
	{
	// If aDeleteDataBase is set to true, then delete the database
	if ( aDeleteDataBase )
		{
		TRAPD(err, CContactDatabase::DeleteDatabaseL(aDbName) );
		if ( err != KErrNone )
			{
			BaseTestStepReference().ERR_PRINTF1(KErrInDeleteDataBase);
			BaseTestStepReference().SetTestStepError(err);
			BaseTestStepReference().SetTestStepResult(EFail);
			}
		}
	}

/*
* Convert the string of type uid which is used for the construction of the views. Commonly used sort fields are only supported
* if more is required, can be added in the loop.
* @param aSortOrder sort order passed in the ini file
* @return RContactViewSortOrder
*/
RContactViewSortOrder CContactUtilitiesCollection::ConvertStringToSortOrderL(const TDesC& aSortOrderString)
	{
	RContactViewSortOrder sortOrder;
	RArray<TPtrC>	sortOrderList;
	CleanupClosePushL(sortOrderList);
	TokenizeStringL(aSortOrderString, sortOrderList);

	for ( TInt i = 0; i < sortOrderList.Count(); ++i )
		{
		TUid uid = GetContactFieldType(sortOrderList[i]);
		sortOrder.AppendL(uid);
		}

	CleanupStack::PopAndDestroy(&sortOrderList);
	return sortOrder;
	}

/**
* Retrieve the contact field uid information based on the contact field information specified in string format
* @param aContactFieldString - Contact field information in String format
* @return TUid - The contact field information in uid format
*/

TUid CContactUtilitiesCollection::GetContactFieldType(const TDesC& aContactFieldString)
	{

	_LIT(KAddress, 						"Address");
	_LIT(KPostOffice,					"PostOffice");
	_LIT(KExtendedAddress, 				"ExtendedAddress");
	_LIT(KLocality,	 					"Locality");
	_LIT(KRegion,	 					"Region");
	_LIT(KPostCode,	 					"PostCode");
	_LIT(KCountry,	 					"Country");
	_LIT(KCompanyName, 					"CompanyName");
	_LIT(KCompanyNamePronunciation, 	"CompanyNamePronunciation");
	_LIT(KPhoneNumber,     				"PhoneNumber");
	_LIT(KGivenName,       		        "GivenName");
	_LIT(KFamilyName,                	"FamilyName");
	_LIT(KGivenNamePronunciation,     	"GivenNamePronunciation");
	_LIT(KFamilyNamePronunciation,    	"FamilyNamePronunciation");
	_LIT(KAdditionalName,             	"AdditionalName");
	_LIT(KSuffixName,                	"SuffixName");
	_LIT(KPrefixName,                	"PrefixName");
	_LIT(KHidden,                		"Hidden");
	_LIT(KEMail,                		"EMail");
	_LIT(KMsg,                			"Msg");
	_LIT(KSms,                			"Sms");
	_LIT(KFax,                			"Fax");
	_LIT(KNote,			  				"Note");
	_LIT(KBirthday,		          		"Birthday");
	_LIT(KUrl,			  				"Url");
	_LIT(KPicture,		          		"Picture");
	_LIT(KRingTone,		          		"RingTone");
	_LIT(KDTMF,			  				"DTMF");
	_LIT(KVoiceDialField,		  		"VoiceDialField");
	_LIT(KJobTitle,		          		"JobTitle");
	_LIT(KICCSlot,		          		"ICCSlot");
	_LIT(KICCPhonebook,	          		"ICCPhonebook");
	_LIT(KICCGroup,		          		"ICCGroup");
	_LIT(KIMAddress,		  			"IMAddress");
	_LIT(KSecondName,		  			"SecondName");
	_LIT(KSIPID,		          		"SIPID");
	_LIT(KAssistant,		  			"Assistant");
	_LIT(KAnniversary,	          		"Anniversary");
	_LIT(KSpouse,		          		"Spouse");
	_LIT(KChildren,		          		"Children");
	_LIT(KClass,		          		"Class");
	_LIT(KDepartmentName,	          	"DepartmentName");

	TUid	uid;

	if ( aContactFieldString.Compare(KAddress) == 0)
		{
		uid = KUidContactFieldAddress;
		}
	else if ( aContactFieldString.Compare(KPostOffice) == 0)
		{
		uid = KUidContactFieldPostOffice;
		}
	else if ( aContactFieldString.Compare(KExtendedAddress) == 0)
		{
		uid = KUidContactFieldExtendedAddress;
		}
	else if ( aContactFieldString.Compare(KLocality) == 0)
		{
		uid = KUidContactFieldLocality;
		}
	else if ( aContactFieldString.Compare(KRegion) == 0)
		{
		uid = KUidContactFieldRegion;
		}
	else if ( aContactFieldString.Compare(KPostCode) == 0)
		{
		uid = KUidContactFieldPostcode;
		}
	else if ( aContactFieldString.Compare(KCountry) == 0)
		{
		uid = KUidContactFieldCountry;
		}
	else if ( aContactFieldString.Compare(KCompanyName) == 0)
		{
		uid = KUidContactFieldCompanyName;
		}
	else if ( aContactFieldString.Compare(KCompanyNamePronunciation) == 0)
		{
		uid = KUidContactFieldCompanyNamePronunciation;
		}
	else if ( aContactFieldString.Compare(KPhoneNumber) == 0)
		{
		uid = KUidContactFieldPhoneNumber;
		}
	else if ( aContactFieldString.Compare(KGivenName) == 0)
		{
		uid = KUidContactFieldGivenName;
		}
	else if ( aContactFieldString.Compare(KFamilyName) == 0)
		{
		uid = KUidContactFieldFamilyName;
		}
	else if ( aContactFieldString.Compare(KGivenNamePronunciation) == 0)
		{
		uid = KUidContactFieldGivenNamePronunciation;
		}
	else if ( aContactFieldString.Compare(KFamilyNamePronunciation) == 0)
		{
		uid = KUidContactFieldFamilyNamePronunciation;
		}
	else if ( aContactFieldString.Compare(KAdditionalName) == 0)
		{
		uid = KUidContactFieldAdditionalName;
		}
	else if ( aContactFieldString.Compare(KSuffixName) == 0)
		{
		uid = KUidContactFieldSuffixName;
		}
	else if ( aContactFieldString.Compare(KPrefixName) == 0)
		{
		uid = KUidContactFieldPrefixName;
		}
	else if ( aContactFieldString.Compare(KHidden) == 0)
		{
		uid = KUidContactFieldHidden;
		}
	else if ( aContactFieldString.Compare(KEMail) == 0)
		{
		uid = KUidContactFieldEMail;
		}
	else if ( aContactFieldString.Compare(KMsg) == 0)
		{
		uid = KUidContactFieldMsg;
		}
	else if ( aContactFieldString.Compare(KSms) == 0)
		{
		uid = KUidContactFieldSms;
		}
	else if ( aContactFieldString.Compare(KFax) == 0)
		{
		uid = KUidContactFieldFax;
		}
	else if ( aContactFieldString.Compare(KNote) == 0)
		{
		uid = KUidContactFieldNote;
		}
	else if ( aContactFieldString.Compare(KBirthday) == 0)
		{
		uid = KUidContactFieldBirthday;
		}
	else if ( aContactFieldString.Compare(KUrl) == 0)
		{
		uid = KUidContactFieldUrl;
		}
	else if ( aContactFieldString.Compare(KPicture) == 0)
		{
		uid = KUidContactFieldPicture;
		}
	else if ( aContactFieldString.Compare(KRingTone) == 0)
		{
		uid = KUidContactFieldRingTone;
		}
	else if ( aContactFieldString.Compare(KDTMF) == 0)
		{
		uid = KUidContactFieldDTMF;
		}
	else if ( aContactFieldString.Compare(KVoiceDialField) == 0)
		{
		uid = KUidContactsVoiceDialField;
		}
	else if ( aContactFieldString.Compare(KJobTitle) == 0)
		{
		uid = KUidContactFieldJobTitle;
		}
	else if ( aContactFieldString.Compare(KICCSlot) == 0)
		{
		uid = KUidContactFieldICCSlot;
		}
	else if ( aContactFieldString.Compare(KICCPhonebook) == 0)
		{
		uid = KUidContactFieldICCPhonebook;
		}
	else if ( aContactFieldString.Compare(KICCGroup) == 0)
		{
		uid = KUidContactFieldICCGroup;
		}
	else if ( aContactFieldString.Compare(KIMAddress) == 0)
		{
		uid = KUidContactFieldIMAddress;
		}
	else if ( aContactFieldString.Compare(KSecondName) == 0)
		{
		uid = KUidContactFieldSecondName;
		}
	else if ( aContactFieldString.Compare(KSIPID) == 0)
		{
		uid = KUidContactFieldSIPID;
		}
	else if ( aContactFieldString.Compare(KAssistant) == 0)
		{
		uid = KUidContactFieldAssistant;
		}
	else if ( aContactFieldString.Compare(KAnniversary) == 0)
		{
		uid = KUidContactFieldAnniversary;
		}
	else if ( aContactFieldString.Compare(KSpouse) == 0)
		{
		uid = KUidContactFieldSpouse;
		}
	else if ( aContactFieldString.Compare(KChildren) == 0)
		{
		uid = KUidContactFieldChildren;
		}
	else if ( aContactFieldString.Compare(KClass) == 0)
		{
		uid = KUidContactFieldClass;
		}
	else if ( aContactFieldString.Compare(KDepartmentName) == 0)
		{
		uid = KUidContactFieldDepartmentName;
		}
	else
		{
		uid = KUidContactFieldNone;
		}

	return uid;
	}

/*
* Get the view preferences from the ini and convert it of type TContactViewPreferences
* @param aViewPreferences viewPreferences passed from the ini file
* @return TContactViewPreferences
*/
TContactViewPreferences CContactUtilitiesCollection::ConvertStringToContactViewPreferencesL(const TDesC& aViewPreferences)
	{
	_LIT(KContactsOnly, 				"contactsonly");
	_LIT(KGroupsOnly, 					"groupsonly");
	_LIT(KContactsAndGroups, 			"contactsandgroups");
	_LIT(KIgnoreUnsorted, 				"ignoreunsorted");
	_LIT(KUnsortedAtbeginning, 			"EUnsortedAtBeginning");
	_LIT(KUnsortedAtEnd, 				"EUnsortedAtEnd");
	_LIT(KSingleWhiteSpace, 			"ESingleWhiteSpaceIsEmptyField");
	_LIT(KICCEntries, 					"EICCEntriesOnly");
	_LIT(KICCEntriesAndContacts, 		"EICCEntriesAndContacts");

	TUint viewPreferences = 0x00000000;

	RArray<TPtrC> list;
	CleanupClosePushL(list);
	TokenizeStringL(aViewPreferences, list);

	for(TInt i = 0; i < list.Count(); ++i)
		{
		if(	list[i] == KContactsOnly)
			{
			viewPreferences |= EContactsOnly;
			}
		else if(list[i] == KGroupsOnly)
			{
			viewPreferences |= EGroupsOnly;
			}
		else if(list[i] == KContactsAndGroups)
			{
			viewPreferences |= EContactAndGroups;
			}
		else if(list[i] == KIgnoreUnsorted)
			{
			viewPreferences |= EIgnoreUnSorted;
			}
		else if(list[i] == KUnsortedAtbeginning)
			{
			viewPreferences |= EUnSortedAtBeginning;
			}
		else if(list[i] == KUnsortedAtEnd)
			{
			viewPreferences |= EUnSortedAtEnd;
			}
		else if(list[i] == KSingleWhiteSpace)
			{
			viewPreferences |= ESingleWhiteSpaceIsEmptyField;
			}
		else if(list[i] == KICCEntries)
			{
			viewPreferences |= EICCEntriesOnly;
			}
		else if(list[i] == KICCEntriesAndContacts)
			{
			viewPreferences |= EICCEntriesAndContacts;
			}
		}
	CleanupStack::PopAndDestroy(&list);
	return (TContactViewPreferences)viewPreferences;
	}

/*
* Converts the string of type TSearchType which is used for view construction
* @param aSearchType The search type
* @return CContactViewBase::TSearchType
*/
CContactViewBase::TSearchType CContactUtilitiesCollection::GetSearchType(const TDesC& aSearchType)
	{
	_LIT(KPrefixSearch,			"EPrefixSearch");

	CContactViewBase::TSearchType	searchType = CContactViewBase::EFullSearch;

	if(aSearchType.Compare(KPrefixSearch) == 0 )
		{
		searchType = CContactViewBase::EPrefixSearch;
		}

	return searchType;
	}

/*
* Converts the string of type TGroupType which is used for view construction
* @param aGroupType The group type
* @return ContactViewBase::TGroupType
*/
CContactGroupView::TGroupType CContactUtilitiesCollection::GetGroupType(const TDesC& aGroupType)
	{
	_LIT(KShowContactsInGroup,		"EShowContactsInGroup");
	_LIT(KShowContactsNotInGroup,	"EShowContactsNotInGroup");

	CContactGroupView::TGroupType	groupType = CContactGroupView::EShowContactsNotInAnyGroup;

	if ( aGroupType.Compare(KShowContactsInGroup) == 0 )
		{
		groupType = CContactGroupView::EShowContactsInGroup;
		}
	else if ( aGroupType.Compare(KShowContactsNotInGroup) == 0 )
		{
		groupType = CContactGroupView::EShowContactsNotInGroup;
		}

	return groupType;
	}

/*
* converts the string of type TContactViewFilter which is used for construction of the views
* @param aFilter The filter to use
* @return CContactDataBase::TContactViewFilter
*/
CContactDatabase::TContactViewFilter CContactUtilitiesCollection::GetContactViewFilterL(const TDesC& aFilter)
	{
	_LIT(KUnFiltered, 				"UnFiltered");
	_LIT(KMailable, 				"Mailable");
	_LIT(KSmsable, 					"Smsable");
	_LIT(KLandLine, 				"LandLine");
	_LIT(KFaxable, 					"Faxable");
	_LIT(KPhonable, 				"Phonable");
	_LIT(KWork, 					"Work");
	_LIT(KHome, 					"Home");
	_LIT(KRingTone, 				"RingTone");
	_LIT(KVoiceDial, 				"VoiceDial");
	_LIT(KIMAddress, 				"IMAddress");
	_LIT(KWirelessVillage, 			"WirelessVillage");
	_LIT(KCustomFilter1, 			"CustomFilter1");
	_LIT(KCustomFilter2, 			"CustomFilter2");
	_LIT(KCustomFilter3, 			"CustomFilter3");
	_LIT(KCustomFilter4, 			"CustomFilter4");

	TUint viewFilter = 0x0;

	RArray<TPtrC> list;
	CleanupClosePushL(list);
	TokenizeStringL(aFilter, list);

	for(TInt i = 0; i < list.Count(); ++i)
		{
		if(	list[i] == KUnFiltered)
			{
			viewFilter 	|= CContactDatabase::EUnfiltered;
			}
		else if(list[i] == KMailable)
			{
			viewFilter 	|= CContactDatabase::EMailable;
			}
		else if(list[i] == KSmsable)
			{
			viewFilter 	|= CContactDatabase::ESmsable;
			}
		else if(list[i] == KLandLine)
			{
			viewFilter 	|= CContactDatabase::ELandLine;
			}
		else if(list[i] == KFaxable)
			{
			viewFilter 	|= CContactDatabase::EFaxable;
			}
		else if(list[i] == KPhonable)
			{
			viewFilter 	|= CContactDatabase::EPhonable;
			}
		else if(list[i] == KWork)
			{
			viewFilter 	|= CContactDatabase::EWork;
			}
		else if(list[i] == KHome)
			{
			viewFilter 	|= CContactDatabase::EHome;
			}
		else if(list[i] == KRingTone)
			{
			viewFilter 	|= CContactDatabase::ERingTone;
			}
		else if(list[i] == KVoiceDial)
			{
			viewFilter 	|= CContactDatabase::EVoiceDial;
			}
		else if(list[i] == KIMAddress)
			{
			viewFilter 	|= CContactDatabase::EIMAddress;
			}
		else if(list[i] == KWirelessVillage)
			{
			viewFilter 	|= CContactDatabase::EWirelessVillage;
			}
		else if(list[i] == KCustomFilter1)
			{
			viewFilter 	|= CContactDatabase::ECustomFilter1;
			}
		else if(list[i] == KCustomFilter2)
			{
			viewFilter 	|= CContactDatabase::ECustomFilter2;
			}
		else if(list[i] == KCustomFilter3)
			{
			viewFilter 	|= CContactDatabase::ECustomFilter3;
			}
		else if(list[i] == KCustomFilter4)
			{
			viewFilter 	|= CContactDatabase::ECustomFilter4;
			}
		}

	CleanupStack::PopAndDestroy(); //list
	return (CContactDatabase::TContactViewFilter)viewFilter;
	}

/*
* Construct different types of views like local view, remote view, named remote view, find view, filtered view,
* group view, concatenated view and sub view based on the list of views specified in the ini file and append the views
* to their corresponding view array
* @param aSections ini sections
*/
void CContactUtilitiesCollection::ConstructViewsL(RArray<TPtrC> aSections)
	{
	// Iterating through the sections listed in the field listOfViews.
	for ( TInt i = 0; i < aSections.Count(); ++i )
		{
		TPtrC	iniSection = aSections[i];
		TPtrC	viewType;
		BaseTestStepReference().GetStringFromConfig(iniSection, KViewType, viewType);
		TPtrC	allocTest;
		BaseTestStepReference().GetStringFromConfig(iniSection, KAllocTest, allocTest);


		if ( allocTest != KNullDesC() )
			{
			OOMTestForViewsL(iniSection, viewType, allocTest);
			}
		else
			{
			if ( viewType.Compare(KLocalView) == 0 )
				{
				// Construct Local view based on the data defined in the ini section i.e aSections[i]
				GetDataAndConstructLocalViewL(iniSection);
				}
			if ( viewType.Compare(KRemoteView) == 0 )
				{
				// Construct remote view based on the data defined in the ini section i.e aSections[i]
				GetDataAndConstructRemoteViewL(iniSection);
				}
			if ( viewType.Compare(KNamedRemoteView) == 0 )
				{
				// Construct named remote view based on the data defined in the ini section i.e aSections[i]
				GetDataAndConstructNamedRemoteViewL(iniSection);
				}
			if ( viewType.Compare(KGroupView) == 0 )
				{
				// Construct group view based on the data defined in the ini section i.e aSections[i]
				GetDataAndConstructGroupViewL(iniSection);
				}
			if ( viewType.Compare(KFindView) == 0 )
				{
				// Construct find view based on the data defined in the ini section i.e aSections[i]
				GetDataAndConstructFindViewL(iniSection);
				}
			if ( viewType.Compare(KFilteredView) == 0 )
				{
				// Construct filtered view based on the data defined in the ini section i.e aSections[i]
				GetDataAndConstructFilteredViewL(iniSection);
				}
			if ( viewType.Compare(KConcatenatedView) == 0 )
				{
				// Construct concatenated view based on the data defined in the ini section i.e aSections[i]
				ConstructConcatenatedViewL(iniSection);
				}
			if ( viewType.Compare(KSubView) == 0 )
				{
				// Construct sub view based on the data defined in the ini section i.e aSections[i]
				GetDataAndConstructSubViewL(iniSection);
				}
			}
		}
	}

/**
* Parses a comma separated string and constructs a list out of the values
* @param aString input string
* @param aList Array of seperated strings
* @param aSeparator ,
*/
void CContactUtilitiesCollection::TokenizeStringL(const TDesC& aString, RArray<TPtrC>& aList, TChar aSeparator)
	{
	TLex lexer(aString);

	while(!lexer.Eos())
		{
		lexer.SkipSpaceAndMark();

		while(!lexer.Eos() && lexer.Get() != aSeparator)
			{
			}

		if(!lexer.Eos())
			{
			lexer.UnGet(); // Do not include trailing ','
			}
		aList.AppendL(lexer.MarkedToken());
		lexer.Inc();
		}
	}

/*
* OOM testing of views
* @param aSection ini section
* @param aViewType view type
* @param aAllocTest OOM test
*/
void CContactUtilitiesCollection::CreateViewUnderOOMConditionL(const TDesC& aSection, const TDesC& aViewType, const TDesC& aAllocTest)
	{
	if ( aViewType.Compare(KGroupView) == 0 && aAllocTest.Compare(KGroupView) == 0 )
		{
		// Construct group view based on the data defined in the ini section i.e aSections[i]
		GetDataAndConstructGroupViewL(aSection);
		}
	else if ( aViewType.Compare(KFindView) == 0 && aAllocTest.Compare(KFindView) == 0 )
		{
		// Construct find view based on the data defined in the ini section i.e aSections[i]
		GetDataAndConstructFindViewL(aSection);
		}
	else if ( aViewType.Compare(KFilteredView) == 0 && aAllocTest.Compare(KFilteredView) == 0 )
		{
		// Construct filtered view based on the data defined in the ini section i.e aSections[i]
		GetDataAndConstructFilteredViewL(aSection);
		}
	else if ( aViewType.Compare(KSubView) == 0 && aAllocTest.Compare(KSubView) == 0 )
		{
		// Construct sub view based on the data defined in the ini section i.e aSections[i]
		GetDataAndConstructSubViewL(aSection);
		}
	}

/*
* Construct the views in the OOM loop
* @param aSortOrder specifies the fields to use to sort the items in the view
* @param aViewPreferences specifies the type of the contact item to be included in the view
* @param aSortPlugin specifies a plug-in that will be used to compare view contacts when the view is sorted
*/
void CContactUtilitiesCollection::OOMTestForViewsL(const TDesC& aSection, const TDesC& aViewType, const TDesC& aAllocTest)
	{
	TInt tryCount = 1;
	for ( ;; )
		{
		__UHEAP_SETFAIL(RHeap::EFailNext, tryCount);

		TRAPD(err, CreateViewUnderOOMConditionL(aSection, aViewType, aAllocTest));

		if ( err == KErrNone )
			{
			__UHEAP_RESET;
			BaseTestStepReference().INFO_PRINTF1(_L("Memory allocation testing for View is done"));
			break;
			}
		if ( err != KErrNoMemory )
			BaseTestStepReference().SetTestStepResult(EFail);
		__UHEAP_SETFAIL(RHeap::ENone, 0);
		tryCount++;
		}
	BaseTestStepReference().INFO_PRINTF2(_L("TRYCOUNT %d"), tryCount);
	}


// CContactViewCollection

CContactViewCollection* CContactViewCollection::NewL()
	{
	CContactViewCollection* self = new(ELeave) CContactViewCollection();
	return self;
	}

CContactViewCollection::CContactViewCollection()
	{
	}

CContactViewCollection::~CContactViewCollection()
	{
	CloseLocalViewAndEventQueue();
	CloseRemoteViewAndEventQueue();
	CloseNamedRemoteViewAndEventQueue();
	CloseFindViewAndEventQueue();
	CloseFilteredViewAndEventQueue();
	CloseGroupViewAndEventQueue();
	CloseConcatenatedViewAndEventQueue();
	CloseSubViewAndEventQueue();
	iArrayOfViewBase.Close();
	iArrayViewEvents.Close();
	}

/*
* Close the localvieweventqueue and localviewarray
*/
void CContactViewCollection::CloseLocalViewAndEventQueue()
	{
	for ( TInt i = 0; i < iArrayOfLocalViews.Count(); ++i )
		{
		if (iArrayOfLocalViews.Count() > 0  && iArrayOfLocalViewEvents.Count() > 0)
			{
			iArrayOfLocalViews[i]->Close(*iArrayOfLocalViewEvents[i]);
			delete iArrayOfLocalViewEvents[i];
			}
		}
	iArrayOfLocalViews.Close();
	}

/*
* Close the RemoteView And its EventQueue
*/
void CContactViewCollection::CloseRemoteViewAndEventQueue()
	{
	for ( TInt i = 0; i < iArrayOfRemoteViews.Count(); ++i )
		{
		if (iArrayOfRemoteViews.Count() > 0 && iArrayOfRemoteViewEvents.Count() > 0)
		iArrayOfRemoteViews[i]->Close(*iArrayOfRemoteViewEvents[i]);
		delete iArrayOfRemoteViewEvents[i];
		}
	iArrayOfRemoteViews.Close();
	}

/*
* Close the NamedRemoteView And its EventQueue
*/
void CContactViewCollection::CloseNamedRemoteViewAndEventQueue()
	{
	for ( TInt i = 0; i < iArrayOfNamedRemoteViews.Count(); ++i )
		{
		if (iArrayOfNamedRemoteViews.Count() > 0 && iArrayOfNamedRemoteViewEvents.Count() > 0)
		iArrayOfNamedRemoteViews[i]->Close(*iArrayOfNamedRemoteViewEvents[i]);
		delete iArrayOfNamedRemoteViewEvents[i];
		}
	iArrayOfNamedRemoteViews.Close();
	}

/*
* Close the FindView And its EventQueue
*/
void CContactViewCollection::CloseFindViewAndEventQueue()
	{
	for ( TInt i = 0; i < iArrayOfFindViews.Count(); ++i )
		{
		if (iArrayOfFindViews.Count() > 0 && iArrayOfFindViewEvents.Count() > 0)
		iArrayOfFindViews[i]->Close(*iArrayOfFindViewEvents[i]);
		delete iArrayOfFindViewEvents[i];
		}
	iArrayOfFindViews.Close();
	}

/*
* Close the FilteredView And its EventQueue
*/
void CContactViewCollection::CloseFilteredViewAndEventQueue()
	{
	for ( TInt i = 0; i < iArrayOfFilteredViews.Count(); ++i )
		{
		if (iArrayOfFilteredViews.Count() > 0 && iArrayOfFilteredViewEvents.Count() > 0)
		iArrayOfFilteredViews[i]->Close(*iArrayOfFilteredViewEvents[i]);
		delete iArrayOfFilteredViewEvents[i];
		}
	iArrayOfFilteredViews.Close();
	}

/*
* Close the GroupView And its EventQueue
*/
void CContactViewCollection::CloseGroupViewAndEventQueue()
	{
	for ( TInt i = 0; i < iArrayOfGroupViews.Count(); ++i )
		{
		if (iArrayOfGroupViews[i])
		iArrayOfGroupViews[i]->Close(*iArrayOfGroupViewEvents[i]);
		delete iArrayOfGroupViewEvents[i];
		}
	iArrayOfGroupViews.Close();
	}

/*
* Close the ConcatenatedView And its EventQueue
*/
void CContactViewCollection::CloseConcatenatedViewAndEventQueue()
	{
	for ( TInt i = 0; i < iArrayOfConcatenatedViews.Count(); ++i )
		{
		if (iArrayOfConcatenatedViews.Count() > 0 && iArrayOfConcatenatedViewEvents.Count() > 0)
		iArrayOfConcatenatedViews[i]->Close(*iArrayOfConcatenatedViewEvents[i]);
		delete iArrayOfConcatenatedViewEvents[i];
		}
	iArrayOfConcatenatedViews.Close();
	}

/*
* Close the SubView And its EventQueue
*/
void CContactViewCollection::CloseSubViewAndEventQueue()
	{
	for ( TInt i = 0; i < iArrayOfSubViews.Count(); ++i )
		{
		if (iArrayOfSubViews.Count() > 0 && iArrayOfSubViewEvents.Count() > 0)
		iArrayOfSubViews[i]->Close(*iArrayOfSubViewEvents[i]);
		delete iArrayOfSubViewEvents[i];
		}
	iArrayOfSubViews.Close();
	}
/*
* retrieve the local view array
* @return RPointerArray<CContactLocalView>&
*/
RPointerArray<CContactLocalView>& CContactViewCollection::GetLocalViewArray()
	{
	return iArrayOfLocalViews;
	}

/*
* retrieve the remote view array
* @return RPointerArray<CContactRemoteView>&
*/
RPointerArray<CContactRemoteView>& CContactViewCollection::GetRemoteViewArray()
	{
	return iArrayOfRemoteViews;
	}

/*
* retrieve the named remote view array
* @return RPointerArray<CContactNamedRemoteView>&
*/
RPointerArray<CContactNamedRemoteView>&	CContactViewCollection::GetNamedRemoteViewArray()
	{
	return iArrayOfNamedRemoteViews;
	}

/*
* retrieve the find view array
* @return RPointerArray<CContactFindView>&
*/
RPointerArray<CContactFindView>& CContactViewCollection::GetFindViewArray()
	{
	return iArrayOfFindViews;
	}

/*
* retrieve the group view array
* @return RPointerArray<CContactGroupView>&
*/
RPointerArray<CContactGroupView>& CContactViewCollection::GetGroupViewArray()
	{
	return iArrayOfGroupViews;
	}

/*
* retrieve the filtered view array
* @return RPointerArray<CContactFilteredView>&
*/
RPointerArray<CContactFilteredView>& CContactViewCollection::GetFilteredViewArray()
	{
	return iArrayOfFilteredViews;
	}

/*
* retrieve the concatenated view array
* @return RPointerArray<CContactConcatenatedView>&
*/
RPointerArray<CContactConcatenatedView>& CContactViewCollection::GetConcatenatedViewArray()
	{
	return iArrayOfConcatenatedViews;
	}

/*
* retrieve the sub view array
* @return RPointerArray<CContactSubView>&
*/
RPointerArray<CContactSubView>& CContactViewCollection::GetSubViewArray()
	{
	return iArrayOfSubViews;
	}

/*
* retrieve the component view array
* @return RPointerArray<CContactViewBase>&
*/
RPointerArray<CContactViewBase>& CContactViewCollection::GetComponentViews()
	{
	return iArrayOfViewBase;
	}

/*
* retrieve the view events array
* @param aTypeOfView type of the view events array to be retrieved
* @return RPointerArray<CContactViewEventQueue>&
*/
RPointerArray<CContactViewEventQueue>& CContactViewCollection::GetContactViewEventsQueueL(const TDesC& aTypeOfView)
	{
	if ( aTypeOfView.Compare(KLocalView) == 0 )
		{
		for ( TInt i = 0; i < iArrayOfLocalViewEvents.Count(); ++i )
			{
			iArrayViewEvents.AppendL(iArrayOfLocalViewEvents[i]);
			}
		}
	else if ( aTypeOfView.Compare(KRemoteView) == 0 )
		{
		for ( TInt i = 0; i < iArrayOfRemoteViewEvents.Count(); ++i )
			{
			iArrayViewEvents.AppendL(iArrayOfRemoteViewEvents[i]);
			}
		}
	else if ( aTypeOfView.Compare(KNamedRemoteView) == 0 )
		{
		for ( TInt i = 0; i < iArrayOfNamedRemoteViewEvents.Count(); ++i )
			{
			iArrayViewEvents.AppendL(iArrayOfNamedRemoteViewEvents[i]);
			}
		}
	else if ( aTypeOfView.Compare(KFindView) == 0 )
		{
		for ( TInt i = 0; i < iArrayOfFindViewEvents.Count(); ++i )
			{
			iArrayViewEvents.AppendL(iArrayOfFindViewEvents[i]);
			}
		}
	else if ( aTypeOfView.Compare(KFilteredView) == 0 )
		{
		for ( TInt i = 0; i < iArrayOfFilteredViewEvents.Count(); ++i )
			{
			iArrayViewEvents.AppendL(iArrayOfFilteredViewEvents[i]);
			}
		}
	else if ( aTypeOfView.Compare(KGroupView) == 0 )
		{
		for ( TInt i = 0; i < iArrayOfGroupViewEvents.Count(); ++i )
			{
			iArrayViewEvents.AppendL(iArrayOfGroupViewEvents[i]);
			}
		}
	else if ( aTypeOfView.Compare(KConcatenatedView) == 0 )
		{
		for ( TInt i = 0; i < iArrayOfConcatenatedViewEvents.Count(); ++i )
			{
			iArrayViewEvents.AppendL(iArrayOfConcatenatedViewEvents[i]);
			}
		}
	else if ( aTypeOfView.Compare(KSubView) == 0 )
		{
		for ( TInt i = 0; i < iArrayOfSubViewEvents.Count(); ++i )
			{
			iArrayViewEvents.AppendL(iArrayOfSubViewEvents[i]);
			}
		}
	return iArrayViewEvents;
	}

/*
* Get the desired local view located at the specified index in the array iArrayOfLocalViews
* @param aIndexOfView index of the view
* @return CContactLocalView
*/
CContactLocalView* CContactViewCollection::GetDesiredLocalView(TInt aIndexOfView)
	{
	RPointerArray<CContactLocalView>& localViewArray = GetLocalViewArray();
	CContactLocalView*	localView = NULL;
	for ( TInt i = 0; i < localViewArray.Count(); ++i )
		{
		if ( i == (aIndexOfView - 1) )
			{
			localView = localViewArray[i];
			}
		}
	return localView;
	}

/*
* Get the desired remote view located at the specified index in the array iArrayOfRemoteViews
* @param aIndexOfView index of the view
* @return CContactRemoteView
*/
CContactRemoteView* CContactViewCollection::GetDesiredRemoteView(TInt aIndexOfView)
	{
	RPointerArray<CContactRemoteView>& remoteViewArray = GetRemoteViewArray();
	CContactRemoteView*	remoteView = NULL;
	for ( TInt i = 0; i < remoteViewArray.Count(); ++i )
		{
		if ( i == (aIndexOfView - 1) )
			{
			remoteView = remoteViewArray[i];
			}
		}
	return remoteView;
	}

/*
* Get the desired named remote view located at the specified index in the array iArrayOfNamedRemoteViews
* @param aIndexOfView index of the view
* @return CContactNamedRemoteView
*/
CContactNamedRemoteView* CContactViewCollection::GetDesiredNamedRemoteView(TInt aIndexOfView)
	{
	RPointerArray<CContactNamedRemoteView>& namedRemoteViewArray = GetNamedRemoteViewArray();
	CContactNamedRemoteView*	namedRemoteView = NULL;
	for ( TInt i = 0; i < namedRemoteViewArray.Count(); ++i )
		{
		if ( i == (aIndexOfView - 1) )
			{
			namedRemoteView = namedRemoteViewArray[i];
			}
		}
	return namedRemoteView;
	}

/*
* Get the desired find view located at the specified index in the array iArrayOfFindViews
* @param aIndexOfView index of the view
* @return CContactFindView
*/
CContactFindView* CContactViewCollection::GetDesiredFindView(TInt aIndexOfView)
	{
	RPointerArray<CContactFindView>& findViewArray = GetFindViewArray();
	CContactFindView*	findView = NULL;
	for ( TInt i = 0; i < findViewArray.Count(); ++i )
		{
		if ( i == (aIndexOfView - 1) )
			{
			findView = findViewArray[i];
			}
		}
	return findView;
	}

/*
* Get the desired filtered view located at the specified index in the array iArrayOfFilteredViews
* @param aIndexOfView index of the view
* @return CContactFilteredView
*/
CContactFilteredView* CContactViewCollection::GetDesiredFilteredView(TInt aIndexOfView)
	{
	RPointerArray<CContactFilteredView>& filteredViewArray = GetFilteredViewArray();
	CContactFilteredView*	filteredView = NULL;
	for ( TInt i = 0; i < filteredViewArray.Count(); ++i )
		{
		if ( i == (aIndexOfView - 1) )
			{
			filteredView = filteredViewArray[i];
			}
		}
	return filteredView;
	}

/*
* Get the desired group view located at the specified index in the array iArrayOfGroupViews
* @param aIndexOfView index of the view
* @return CContactGroupView
*/
CContactGroupView* CContactViewCollection::GetDesiredGroupView(TInt aIndexOfView)
	{
	RPointerArray<CContactGroupView>& groupViewArray = GetGroupViewArray();
	CContactGroupView*	groupView = NULL;
	for ( TInt i = 0; i < groupViewArray.Count(); ++i )
		{
		if ( i == (aIndexOfView - 1) )
			{
			groupView = groupViewArray[i];
			}
		}
	return groupView;
	}

/*
* Get the desired concatenated view located at the specified index in the array iArrayOfConcatenatedViews
* @param aIndexOfView index of the view
* @return CContactConcatenatedView
*/
CContactConcatenatedView* CContactViewCollection::GetDesiredConcatenatedView(TInt aIndexOfView)
	{
	RPointerArray<CContactConcatenatedView>& concatenatedViewArray = GetConcatenatedViewArray();
	CContactConcatenatedView*	concatenatedView = NULL;
	for ( TInt i = 0; i < concatenatedViewArray.Count(); ++i )
		{
		if ( i == (aIndexOfView - 1) )
			{
			concatenatedView = concatenatedViewArray[i];
			}
		}
	return concatenatedView;
	}

/*
* Get the desired sub view located at the specified index in the array iArrayOfSubViews
* @param aIndexOfView index of the view
* @return CContactSubView
*/
CContactSubView* CContactViewCollection::GetDesiredSubView(TInt aIndexOfView)
	{
	RPointerArray<CContactSubView>& subViewArray = GetSubViewArray();
	CContactSubView*	subView = NULL;
	for ( TInt i = 0; i < subViewArray.Count(); ++i )
		{
		if ( i == (aIndexOfView - 1) )
			{
			subView = subViewArray[i];
			}
		}
	return subView;
	}

/*
* Get the desired sub view located at the specified index in the array iArrayOfSubViews
* @param indexOfView index of the view
* @param aTypeOfView type of the view
* @return CContactViewEventQueue*
*/
CContactViewEventQueue* CContactViewCollection::GetDesiredViewEventL(TInt aIndexOfView, const TDesC& aTypeOfView)
	{
	RPointerArray<CContactViewEventQueue>& viewEventsQueue = GetContactViewEventsQueueL(aTypeOfView);
	CContactViewEventQueue*	viewEvent = NULL;
	for ( TInt i = 0; i < viewEventsQueue.Count(); ++i )
		{
		if ( i == (aIndexOfView - 1) )
			{
			viewEvent = viewEventsQueue[i];
			}
		}
	return viewEvent;
	}


// CContactViewTest

CContactViewApiTest::CContactViewApiTest(CTestStep* aTestStep)
: iTestStep(aTestStep)
	{
	}

/**
 * Constructor
 */
CContactViewApiTest* CContactViewApiTest::NewL(CTestStep* aTestStep)
	{
	CContactViewApiTest* self = new(ELeave) CContactViewApiTest(aTestStep);
	return self;
	}

/**
 * Destructor
 */
CContactViewApiTest::~CContactViewApiTest()
	{
	iFsSession.Close();
	}


CTestStep& CContactViewApiTest::TestStepReference()
	{
	return *iTestStep;
	}

/*
* Get the data required to test the AtL() API
* @param aSection ini section
* @return index
*/
TInt CContactViewApiTest::GetAtLApiDataFromIni(const TDesC& aSection)
	{
	TInt	index = -1;
	TestStepReference().GetIntFromConfig(aSection, KAtLIndex, index);
	return index;
	}

/*
* Pass the index of the contact item and get the contact item id
* @param aIndex index of the contact item id
* @param aView desired view
* @param aErrorCondition diskfull or not ready
* @return TContactItemId
*/
TContactItemId CContactViewApiTest::AtL(TInt aIndex, CContactViewBase* aView, const TDesC& aErrorCondition)
	{
	TContactItemId contactItemId = 0;
	// Accessing the view when it is in disk full condition
	if ( aErrorCondition != KNullDesC() )
		{
		#ifdef __DEBUG
		SetFsErrorConditionL(aErrorCondition, KFsAccessCount);
		#endif
		TRAPD(err, contactItemId = aView->AtL(aIndex) );
		TPtrC errorCondition;
		errorCondition.Set(_L("ErrorNone"));
		#ifdef __DEBUG
		SetFsErrorConditionL(errorCondition);
		#endif
		ValidateErrorCondition(err, aErrorCondition);
		}
	else
		{
		TRAPD(err, contactItemId = aView->AtL(aIndex) );
		if ( err != KErrNone )
			{
			contactItemId = -1;
			TestStepReference().SetTestStepError(err);
			TestStepReference().SetTestStepResult(EFail);
			}
		}
	return contactItemId;
	}

/*
* Change the sortorder of the view
* @return index
*/
void CContactViewApiTest::TestChangeSortOrderL(const TDesC& aSection, CContactUtilitiesCollection* aContactUtility)
	{
	CContactNamedRemoteView* namedRemoteview = NULL;
	TInt index = -1;
	TestStepReference().GetIntFromConfig(aSection, KIndexOfViews, index);
	CContactViewCollection& viewCollection = aContactUtility->ViewCollectionReference();
	namedRemoteview = viewCollection.GetDesiredNamedRemoteView(index);
	TPtrC	viewSortOrder;
	TestStepReference().GetStringFromConfig(aSection, KViewSortOrder, viewSortOrder);
	RContactViewSortOrder sortOrder = aContactUtility->ConvertStringToSortOrderL(viewSortOrder);
	namedRemoteview->ChangeSortOrderL(sortOrder);
	User::After(10 * 1000000);
	}



/*
* Get the data required to test the ContactAtL() API
* @param aSection ini section
* @return index
*/
TInt CContactViewApiTest::GetContactAtLApiDataFromIni(const TDesC& aSection)
	{
	TInt	index = -1;
	TestStepReference().GetIntFromConfig(aSection, KAtLIndex, index);
	return index;
	}

/*
* Pass the index and get the contact item
* @param aIndex index of the contact item in the view
* @param aView desired view
* @return CViewContact
*/
CViewContact* CContactViewApiTest::ContactAtL(TInt aIndex, CContactViewBase* aView)
	{
	CViewContact* viewContact = NULL;
	TRAPD(err, viewContact = CViewContact::NewL(aView->ContactAtL(aIndex)));
	if(err != KErrNone)
		{
		viewContact = NULL;
		TestStepReference().SetTestStepError(err);
		TestStepReference().SetTestStepResult(EFail);
		}
	return viewContact;
	}

/*
* Get the data required for the FindL() API
* @param aSection ini section
* @return TInt contactItemId
*/
TInt CContactViewApiTest::GetFindLApiDataFromIni(const TPtrC& aSection)
	{
	TInt contactItemId = -1;
	TestStepReference().GetIntFromConfig(aSection, KContactItemId, contactItemId);
	return contactItemId;
	}

/*
* Pass the contact item id and find the index of the contact item id
* @param aSection ini section
* @param aView desired view
* @param aErrorCondition diskfull or not ready
* @return index of the contact item id
*/
TInt CContactViewApiTest::FindL(TContactItemId aId, CContactViewBase* aView, const TDesC& aErrorCondition)
	{
	TInt index(-1);
	// Accessing the view when it is in disk full condition
	if ( aErrorCondition != KNullDesC() )
		{
		#ifdef __DEBUG
		SetFsErrorConditionL(aErrorCondition, KFsAccessCount);
		#endif
		TRAPD( err, index = aView->FindL(aId));
		TPtrC errorCondition;
		errorCondition.Set(_L("ErrorNone"));
		#ifdef __DEBUG
		SetFsErrorConditionL(errorCondition);
		#endif
		ValidateErrorCondition(err, aErrorCondition);
		}
	else
		{
		TRAPD( err, index = aView->FindL(aId));
		if ( err != KErrNone )
			{
			TestStepReference().SetTestStepError(err);
			TestStepReference().SetTestStepResult(EFail);
			}
		}
	return index;
	}

/*
* Get the fields required for testing AllFieldsLC() from the ini file
* @param aSection ini section
* @param aView desired view
* @return HBufC* contact item fields
*/
HBufC* CContactViewApiTest::GetAllFieldsLDataFromIniL(const TDesC& aSection, CContactViewBase* aView)
	{
	TInt	index = -1;
	TPtrC	fieldSeparator;
	TestStepReference().GetIntFromConfig(aSection, KIndex, index);
	TestStepReference().GetStringFromConfig(aSection, KFieldSeparator, fieldSeparator);

	return (GetAllFieldsL(index, fieldSeparator, aView));
	}

/*
* Get all the fields of the contact item located at the specified index
* @param aIndex retrieved index
* @param aSeparator separator to separate the fields
* @param aView desired view
* @return HBufC* contact item fields
*/
HBufC* CContactViewApiTest::GetAllFieldsL(TInt aIndex, const TDesC &aSeparator, CContactViewBase* aView)
	{
	HBufC* contactFields = NULL;
	contactFields = aView->AllFieldsLC(aIndex, aSeparator);
	if(contactFields!= NULL)
		{
		CleanupStack::Pop(contactFields);
		}
	return (contactFields);
	}

/*
* Get the sort order of the contact view
* @param aView desired view
* @param aErrorCondition diskfull or not ready
* @return RContactViewSortOrder sortOrder
*/
RContactViewSortOrder CContactViewApiTest::GetSortOrderL(CContactViewBase* aView, const TDesC& aErrorCondition)
	{
	RContactViewSortOrder viewSortOrder;
	// Accessing the view when it is in disk full condition
	if ( aErrorCondition != KNullDesC() )
		{
		#ifdef __DEBUG
		SetFsErrorConditionL(aErrorCondition, KFsAccessCount);
		#endif
		TRAPD( err, viewSortOrder = aView->SortOrderL());
		TPtrC errorCondition;
		errorCondition.Set(_L("ErrorNone"));
		#ifdef __DEBUG
		SetFsErrorConditionL(errorCondition);
		#endif
		ValidateErrorCondition(err, aErrorCondition);
		}
	else
		{
		TRAPD( err, viewSortOrder = aView->SortOrderL());
		if ( err != KErrNone )
			{
			TestStepReference().SetTestStepError(err);
			TestStepReference().SetTestStepResult(EFail);
			}
		}
	return viewSortOrder;
	}

/*
* Get the contact view preferences of the current view
* @param aView desired view
* @return TContactViewPreferences
*/
TContactViewPreferences CContactViewApiTest::ContactViewPreferences(CContactViewBase* aView)
	{
	return (aView->ContactViewPreferences());
	}

/*
* Refine the find view by passing new set of search words or append characters to the existing search words
* @param aSearchWords refined search words
* @param aView desired view
* @param aContactUtility utility pointer
*/
void CContactViewApiTest::RefineFindViewL(const TDesC& aSearchWords, CContactFindView* aView, CContactUtilitiesCollection* aContactUtility)
	{
	RArray<TPtrC>	searchStrings;
	CleanupClosePushL(searchStrings);
	aContactUtility->TokenizeStringL(aSearchWords, searchStrings);

	// Search words to refine the find view
	CPtrCArray*	findWords = new(ELeave) CPtrCArray(3);
	CleanupStack::PushL(findWords);
	for ( TInt i = 0; i < searchStrings.Count(); ++i )
		{
		findWords->AppendL(searchStrings[i]);
		}

	// Refine the find view with the new find words
	TRAPD( err, aView->RefineFindViewL(findWords) );
	if ( err != KErrNone )
		{
		TestStepReference().ERR_PRINTF1(KErrInRefineFindView);
		TestStepReference().SetTestStepError(err);
		TestStepReference().SetTestStepResult(EFail);
		}
	CleanupStack::PopAndDestroy(findWords);
	CleanupStack::PopAndDestroy(&searchStrings);
	}

/*
* Get the contacts matching the specified criteria
* @param aSearchWords Criteria to search
* @param aView desired view
* @param aContactUtility utility pointer
* @return RPointerArray<CViewContact>
*/
RPointerArray<CViewContact> CContactViewApiTest::GetContactsMatchingCriteriaL(const TDesC& aSearchWords, CContactViewBase* aView,
 													CContactUtilitiesCollection* aContactUtility, const TDesC& aErrorCondition)
	{
	RArray<TPtrC>	searchStrings;
	CleanupClosePushL(searchStrings);
	aContactUtility->TokenizeStringL(aSearchWords, searchStrings);

	// Append the search strings in the array which is required to pass in the API
	CPtrCArray*	findWords = new(ELeave) CPtrCArray(3);
	CleanupStack::PushL(findWords);
	for ( TInt i = 0; i < searchStrings.Count(); ++i )
		{
		findWords->AppendL(searchStrings[i]);
		}

	// get the contacts matching the findwords and return the matched contacts in the array
	RPointerArray<CViewContact>	matchedContacts;
	if ( aErrorCondition != KNullDesC() )
		{
		#ifdef __DEBUG
		SetFsErrorConditionL(aErrorCondition, KFsAccessCount);
		#endif
		TRAPD( err, aView->ContactsMatchingCriteriaL(*findWords, matchedContacts) );
		TPtrC errorCondition;
		errorCondition.Set(_L("ErrorNone"));
		#ifdef __DEBUG
		SetFsErrorConditionL(errorCondition);
		#endif
		ValidateErrorCondition(err, aErrorCondition);
		}
	else
		{
		TRAPD( err, aView->ContactsMatchingCriteriaL(*findWords, matchedContacts) );
		if ( err != KErrNone )
			{
			TestStepReference().SetTestStepError(err);
			TestStepReference().SetTestStepResult(EFail);
			}
		}
	CleanupStack::PopAndDestroy(findWords);
	CleanupStack::PopAndDestroy(&searchStrings);
	return matchedContacts;
	}

/*
* Get the contacts where the search words matches the prefix of the contacts
* @param aSearchWords Criteria to search
* @param aView desired view
* @param aContactUtility utility pointer
* @return RPointerArray<CViewContact>
*/
RPointerArray<CViewContact>  CContactViewApiTest::GetContactsMatchingPrefixL(const TDesC& aSearchWords, CContactViewBase* aView,
																CContactUtilitiesCollection* aContactUtility, const TDesC& aErrorCondition)
	{
	RArray<TPtrC>	searchStrings;
	CleanupClosePushL(searchStrings);
	aContactUtility->TokenizeStringL(aSearchWords, searchStrings);
	// Append the search strings in the array which is required to pass in the ini file
	CPtrCArray*	findWords = new(ELeave) CPtrCArray(3);
	CleanupStack::PushL(findWords);
	for ( TInt i = 0; i < searchStrings.Count(); ++i )
		{
		findWords->AppendL(searchStrings[i]);
		}
	// Get the contacts where the search words matches the prefix and return the matched contacts in the array
	RPointerArray<CViewContact>	matchedContacts;
	if ( aErrorCondition != KNullDesC() )
		{
		#ifdef __DEBUG
		SetFsErrorConditionL(aErrorCondition, KFsAccessCount);
		#endif
		TRAPD( err, aView->ContactsMatchingPrefixL(*findWords, matchedContacts) );
		TPtrC errorCondition;
		errorCondition.Set(_L("ErrorNone"));
		#ifdef __DEBUG
		SetFsErrorConditionL(errorCondition);
		#endif
		ValidateErrorCondition(err, aErrorCondition);
		}
	else
		{
		TRAPD( err, aView->ContactsMatchingPrefixL(*findWords, matchedContacts) );
			if ( err != KErrNone )
			{
			TestStepReference().SetTestStepError(err);
			TestStepReference().SetTestStepResult(EFail);
			}
		}
	CleanupStack::PopAndDestroy(findWords);
	CleanupStack::PopAndDestroy(&searchStrings);
	return matchedContacts;
	}


/** Based on the error condition, call the respective validate function
* @param aErr actual error code -- KErrDiskFull/KErrNotReady
* @param aErrorCondition expected error --- KErrDiskFull/KErrNotReady
*/
void CContactViewApiTest::ValidateErrorCondition(TInt aErr, const TDesC& aErrorCondition)
	{
	if ( aErrorCondition.Compare(KDiskFull) == 0 )
		{
		ValidateResult(aErr, KErrDiskFull);
		}
	else
		{
		if ( aErrorCondition.Compare(KDiskNotReady) == 0 )
			{
			ValidateResult(aErr, KErrNotReady);
			}
		else
			{
			ValidateResult(aErr, KErrNone);
			}
		}
	}

/** Validate and set the TestStep result to EFail, if required
* @param aActualResult - actual result
* @param aDesiredResult - desired result
*/
void CContactViewApiTest::ValidateResult(TInt aActualResult, TInt aDesiredResult)
	{
	if(aActualResult != aDesiredResult)
		{

		TestStepReference().ERR_PRINTF3(KError, aActualResult, aDesiredResult);
		TestStepReference().SetTestStepResult(EFail);
		}
	}

#ifdef _DEBUG
/**
* Set the error condition to the file system
* @param aErrorCondition KDiskFull or KDiskNotReady
* @param aCount count
*/
void CContactViewApiTest::SetFsErrorConditionL(const TDesC& aErrorCondition, TInt aCount)
	{
	RFs& fs = GetFsSessionL();
	TInt res;
	if ( aErrorCondition.Compare(KDiskFull) == 0 )
		{
		res = fs.SetErrorCondition(KErrDiskFull, aCount);

		}
	else if ( aErrorCondition.Compare(KDiskNotReady) == 0 )
		{
		res = fs.SetErrorCondition(KErrNotReady, aCount);
		}
	else if ( aErrorCondition.Compare(KErrorNone) == 0 )
		{
		res = fs.SetErrorCondition(0, aCount);
		}
	if(res != KErrNone)
		{
		TestStepReference().SetTestStepError(res);
		TestStepReference().SetTestStepResult(EFail);
		}
	}
#endif

/*
* Create a session with the file server
* @return RFs& file session
*/
RFs& CContactViewApiTest::GetFsSessionL()
	{
	User::LeaveIfError(iFsSession.Connect());
	return iFsSession;
	}


/**
* CContactViewValidation
*/

CContactViewValidation::CContactViewValidation(CTestStep* aTestStep)
: iTestStep(aTestStep)
	{
	}

/*
* Destructor
*/
CContactViewValidation::~CContactViewValidation()
	{
	}

/*
* Returns the test step reference
* @return CTestStep&
*/
CTestStep& CContactViewValidation::TestStepReference()
	{
	return *iTestStep;
	}

/*
* Validate the Contact item id returned by the AtL() API with the expected contact item id
* @param aContactItemId retrieved contact item id from the API
* @param aSection ini section
*/
void CContactViewValidation::ValidateAtLApi(TContactItemId aContactItemId, const TDesC& aSection)
	{
	TInt	desiredContactItemId = -1;
	TestStepReference().GetIntFromConfig(aSection, KDesiredContactItemId, desiredContactItemId);
	if ( desiredContactItemId != aContactItemId )
		{
		TestStepReference().ERR_PRINTF3(KErrExpectedAndRetrivedContactItemId, desiredContactItemId, aContactItemId);
		TestStepReference().SetTestStepResult(EFail);
		}
	}

/*
* Validate the Contact item id returned by the AtL() API with the expected contact item id
* @param aViewContact retrieved contact item
* @param aSection ini section
*/
void CContactViewValidation::ValidateContactAtLApi(CViewContact* aViewContact, const TDesC& aSection)
	{
	TInt	desiredContactId = -1;
	TestStepReference().GetIntFromConfig(aSection, KDesiredContactId, desiredContactId);

	if ( aViewContact->Id() != desiredContactId )
		{
		TestStepReference().ERR_PRINTF1(KErrInContactType);
		TestStepReference().SetTestStepResult(EFail);
		}
	}

/*
* Validate count with the expectedCount
* @param aSection ini section
* @param aCount retrieved count from the API
*/
void CContactViewValidation::ValidateCountL(const TDesC& aSection, TInt aCount)
	{
	TInt	desiredCount = -1;
	TestStepReference().GetIntFromConfig(aSection, KDesiredCount, desiredCount);
	if ( aCount != desiredCount )
		{
		TestStepReference().ERR_PRINTF3(KErrExpectedAndRetrievedCount, desiredCount, aCount);
		TestStepReference().SetTestStepResult(EFail);
		}
	}

/*
* Validate the index returned by FindL() API with the expected Index
* @param aSection ini section
* @param aIndex retrieved index
*/
void CContactViewValidation::ValidateFindL(const TDesC& aSection, TInt aIndex)
	{
	TInt	desiredIndex = -1;
	TestStepReference().GetIntFromConfig(aSection, KDesiredIndex, desiredIndex);
	if ( aIndex != desiredIndex )
		{
		TestStepReference().ERR_PRINTF3(KErrExpectedAndRetrievedIndex, desiredIndex, aIndex);
		TestStepReference().SetTestStepResult(EFail);
		}
	}

/*
* Validate all the fields of the contact item and compare it the with the expected fields
* @param aSection ini section
* @param retrievedField fields retrieved by the API
* @param aView deisred view
*/
void CContactViewValidation::ValidateAllFieldsL(const TDesC& aSection, RArray<TPtrC> aRetrievedField, CContactViewBase* aView,
												CContactViewApiTest* aViewApiTest)
	{
	TInt	index = -1;
	TestStepReference().GetIntFromConfig(aSection, KIndex, index);
	// Get the contact item located at the specified index in the view
	CViewContact* viewContact = aViewApiTest->ContactAtL(index, aView);
	// Count the number of fields for the contact item
	TInt	count = viewContact->FieldCount();
	TPtrC	field;
	for ( TInt i = 0; i < count; ++i )
		{
		//retrieve the fields one by one by specifying the position
		field.Set(viewContact->Field(i));
		// Compare the fields retrived by AllFieldsLC() and ContactAtL() API's and log the result
		if ( aRetrievedField[i] != field )
			{
			TestStepReference().ERR_PRINTF3(KErrExpectedAndRetrievedFields, &field, &aRetrievedField[i]);
			TestStepReference().SetTestStepResult(EFail);
			}
		}
	}

/*
* Validate the view preferences with the expected view preferences
* @param aSection ini section
* @param viewPreferences retrieved view preferences
*/
void CContactViewValidation::ValidateViewPreferencesL(const TDesC& aSection, TContactViewPreferences& aViewPreferences,
													CContactUtilitiesCollection* aContactUtility)
	{
	TPtrC	viewPreferences1;
	TestStepReference().GetStringFromConfig(aSection, KViewPreferences, viewPreferences1);
	TContactViewPreferences	expectedViewPreferences = aContactUtility->ConvertStringToContactViewPreferencesL(viewPreferences1);
	if ( expectedViewPreferences != aViewPreferences)
		{
		TestStepReference().ERR_PRINTF3(KErrExpectedAndRetrievedViewPreferences, expectedViewPreferences, aViewPreferences);
		TestStepReference().SetTestStepResult(EFail);
		}
	}

/*
* Validate the uid's of the expected and retrieved sort order
* @param aUid retrieved Uid from the API
* @param aExpecteUid expectedUid passed from the ini file
*/
void CContactViewValidation::ValidateSortOrderL(const TUid& aUid, const TUid& aExpectedUid)
	{
	if ( aUid != aExpectedUid )
		{
		TestStepReference().ERR_PRINTF3(KErrExpectedAndRetrievedUid, aUid, aExpectedUid);
		TestStepReference().SetTestStepResult(EFail);
		}
	}

/*
* Imports contacts from a VCF file into specified database file
* @param aSection ini section
* @param aImportFileName file to be imported
* @param aDb reference to the pointed of CContactDatabase
*/
void CContactUtilitiesCollection::ImportVcardContactsL(const TPtrC& aImportFileName, TBool aPbapBased)
	{
	// Get the version of the VCard parser to be used
	TUid	pluginUid;
	pluginUid.iUid = KUidVCardConvDefaultImpl; // Supports version 2.1 and 3.0


	if( aPbapBased )
		{
		pluginUid.iUid = KUidPBAPVCardConvImpl; // used only when the vCard has pbap specific properties
		}

	// Connect to file system
   	RFs	fsSession;
	User::LeaveIfError(fsSession.Connect());
	CleanupClosePushL(fsSession);
	RFileReadStream	readStream;
	CleanupClosePushL(readStream);

	// Open the the file from which the contact data has to be imported
   	User::LeaveIfError(readStream.Open(fsSession, aImportFileName, EFileRead));

	CArrayPtr<CContactItem>*	item = NULL;
	TBool	success = EFalse;
	CContactDatabase& cDb = DatabaseReference();
	FOREVER
		{
		TRAPD( err, item = cDb.ImportContactsL(pluginUid, readStream, success, CContactDatabase::EImportSingleContact) );
		if( success == EFalse ) // success will set to false, if there are issues during import operation
			{                   // or all the contacts in the vcf file are imported
			break;
			}
		if ( err != KErrNone )
			{
			_LIT(KErrInImportContacts,	"Error in importcontacts");
			BaseTestStepReference().ERR_PRINTF1(KErrInImportContacts);
			}

		if( item->Count() )
			{
			item->ResetAndDestroy();
			}
		}

	CleanupStack::PopAndDestroy(); //  readStream
	CleanupStack::PopAndDestroy(); //  fsSession
	}

/*
* Get the desired view observer
* @param aTypeOfView Type of view like local, Remote, NamedRemote etc.
* @param aViewIndex Index of the view
* @return CContactViewEventQueue returns a reference to the desired view observer
*/
CContactViewEventQueue& CContactViewCollection::GetDesiredViewObserver(const TDesC& aTypeOfView, const TInt aViewIndex)
	{
	CContactViewEventQueue* viewEventQueue = NULL;

	if ( aTypeOfView.Compare(KLocalView) == 0 )
		{
		viewEventQueue = iArrayOfLocalViewEvents[aViewIndex];
		}
	else if ( aTypeOfView.Compare(KRemoteView) == 0 )
		{
		viewEventQueue = iArrayOfRemoteViewEvents[aViewIndex];
		}
	else if ( aTypeOfView.Compare(KNamedRemoteView) == 0 )
		{
		viewEventQueue  = iArrayOfNamedRemoteViewEvents[aViewIndex];
		}
	else if ( aTypeOfView.Compare(KFindView) == 0 )
		{
		viewEventQueue = iArrayOfFindViewEvents[aViewIndex];
		}
	else if ( aTypeOfView.Compare(KFilteredView) == 0 )
		{
		viewEventQueue = iArrayOfFilteredViewEvents[aViewIndex];
		}
	else if ( aTypeOfView.Compare(KGroupView) == 0 )
		{
		viewEventQueue = iArrayOfGroupViewEvents[aViewIndex];
		}
	else if ( aTypeOfView.Compare(KConcatenatedView) == 0 )
		{
		viewEventQueue = iArrayOfConcatenatedViewEvents[aViewIndex];
		}
	else if ( aTypeOfView.Compare(KSubView) == 0 )
		{
		viewEventQueue = iArrayOfSubViewEvents[aViewIndex];
		}
	return *viewEventQueue;
	}

/*
* Get the desired view
* @param aTypeOfView Type of view like local, Remote, NamedRemote etc.
* @param aViewIndex Index of the view
* @return CContactViewBase return the pointer of desired view
*/
CContactViewBase* CContactViewCollection::GetDesiredView(const TDesC& aTypeOfView, const TInt aViewIndex)
	{
	CContactViewBase* desiredView = NULL;

	if ( aTypeOfView.Compare(KLocalView) == 0 )
		{
		desiredView = iArrayOfLocalViews[aViewIndex];
		}
	else if ( aTypeOfView.Compare(KRemoteView) == 0 )
		{
		desiredView = iArrayOfRemoteViews[aViewIndex];
		}
	else if ( aTypeOfView.Compare(KNamedRemoteView) == 0 )
		{
		desiredView  = iArrayOfNamedRemoteViews[aViewIndex];
		}
	else if ( aTypeOfView.Compare(KFindView) == 0 )
		{
		desiredView = iArrayOfFindViews[aViewIndex];
		}
	else if ( aTypeOfView.Compare(KFilteredView) == 0 )
		{
		desiredView = iArrayOfFilteredViews[aViewIndex];
		}
	else if ( aTypeOfView.Compare(KGroupView) == 0 )
		{
		desiredView = iArrayOfGroupViews[aViewIndex];
		}
	else if ( aTypeOfView.Compare(KConcatenatedView) == 0 )
		{
		desiredView = iArrayOfConcatenatedViews[aViewIndex];
		}
	else if ( aTypeOfView.Compare(KSubView) == 0 )
		{
		desiredView = iArrayOfSubViews[aViewIndex];
		}
	return desiredView;
	}

/*
* Construct the desired views based on data from the ini file
*/
void CContactUtilitiesCollection::ConstructViewsL()
	{
	TPtrC	listOfViews;
	BaseTestStepReference().GetStringFromConfig(BaseTestStepReference().ConfigSection(), SharedConstants::KListOfViews, listOfViews);
	RArray<TPtrC>	sections;
	CleanupClosePushL(sections);
	TokenizeStringL(listOfViews, sections);
	ConstructViewsL(sections);
	CleanupStack::PopAndDestroy(&sections);
	}

/*
* Wait for view to ready
* @param aViewEventQueue used to queued the events.
*/
void CContactUtilitiesCollection::WaitForViewReady(CContactViewEventQueue*& aViewEventQueue)
	{
	TContactViewEvent	event;
	// wait for the event to get ready
	TBool eventReady = aViewEventQueue->ListenForEvent(KEventWaitTime, event);
    __ASSERT_ALWAYS(eventReady, User::Invariant());
    ASSERT(event.iEventType == TContactViewEvent::EReady);
	}

/*
* Retrieve the desired contact view observer from the view collection
* @param aTestStepReference - Reference to current test step, used to read data from the ini file
*/
CContactViewEventQueue& CContactViewCollection::RetrieveDesiredViewObserverL(
																			CTestStep& aTestStepReference)

	{
	TInt desiredViewIndex = 0;
	_LIT(KDesiredViewIndex, "DesiredViewIndex");
	aTestStepReference.GetIntFromConfig(aTestStepReference.ConfigSection(), KDesiredViewIndex, desiredViewIndex);

	TPtrC desiredViewType;
	_LIT(KDesiredViewType, "DesiredViewType");
	aTestStepReference.GetStringFromConfig(aTestStepReference.ConfigSection(), KDesiredViewType, desiredViewType);

 	return(GetDesiredViewObserver(desiredViewType, desiredViewIndex));
	}

/*
* Imports contacts into database
*/
void CContactUtilitiesCollection::ImportContactsL()
	{
	// File from where contacts must be imported
	TPtrC	importFileName;
	TBool	filePresent;
	filePresent = 	BaseTestStepReference().GetStringFromConfig(BaseTestStepReference().ConfigSection(), SharedConstants::KFileToBeImported, importFileName);

	TBool	pbapBased = EFalse;
	BaseTestStepReference().GetBoolFromConfig(BaseTestStepReference().ConfigSection(), SharedConstants::KVCardVersionPBAP, pbapBased);
	if(filePresent)
		{
		ImportVcardContactsL(importFileName, pbapBased);
		}
	}

