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




#ifndef __TEST_CONTACTVIEW_UNDERLYING_VIEW_UPDATE_H__
#define __TEST_CONTACTVIEW_UNDERLYING_VIEW_UPDATE_H__

// System Includes
#include <testexecutestepbase.h>
#include <cntdb.h>
#include <cntitem.h>
#include <cntviewbase.h>
#include <cntfldst.h>
#include <cntfilt.h>
#include <e32math.h>

// User Includes
#include "contactutilitiescollection.h"
#include "clientserver.h"
#include "testcontactviewdatabaseutilitiesstep.h"

// Literals used
_LIT(KTestContactViewUnderLyingViewUpdateStep,  "TestContactViewUnderLyingViewUpdateStep");
_LIT(KLocalViewString,						    "Local View");
_LIT(KRemoteViewString,						    "Remote View");
_LIT(KNamedRemoteViewString,					"Named Remote View");
_LIT(KGroupViewString,							"Group View");
_LIT(KFindViewString,							"Find View");
_LIT(KFilteredViewString,						"Filtered View");
_LIT(KConcatenatedViewString,					"ConcatenatedV iew");
_LIT(KSubViewString,							"Sub View");


/**
 * This class facilitates construction of contact view validation objects, used for verifying the
 * behaviour of contact views when the underlying database is updated.
 */
class TViewData
	{
	public:
		enum TContactViewType
			{	
			ELocalView,
			ERemoteView,
			ENamedRemoteView,
			EFilteredView,
			EFindView,
			EGroupView,
			ESubView,
			EConcatenatedView,
			};
	
		void 								SetViewType(const TPtrC& aViewTypeString);		
		void 								SetViewType(TViewData::TContactViewType aViewType);
		void 								SetViewIndex(TInt aViewIndex);
		TInt 								ViewIndex();
		TPtrC 								ViewTypeInStringFormat();
		TViewData::TContactViewType 		ViewType();
		TViewData::TContactViewType 		ConvertViewType(const TPtrC& aViewTypeString);
		void 	ConvertViewTypeToString(TViewData::TContactViewType aViewType, TDes& aViewTypeInString);
		
	private:
		TViewData::TContactViewType 		iViewType;
		TBuf<SharedConstants::KMaxViewTypeLength>  		 	iViewTypeInStringFormat;		
		TInt			 					iViewIndex;
	};

/**
 * Sets the view type information based on data available in string format
 * @param aViewTypeString - view type information in string format
 */
inline void TViewData::SetViewType(const TPtrC& aViewTypeString)
	{
	iViewTypeInStringFormat.Copy(aViewTypeString);
	iViewType = ConvertViewType(aViewTypeString);
	}

/**
 * Sets the view type information based on data available in TViewData::TContactViewType format	
 * @param aViewType - view type in TViewData::TContactViewType format	
 */ 
inline void TViewData::SetViewType(TViewData::TContactViewType aViewType)
	{
	iViewType = aViewType;
	ConvertViewTypeToString(aViewType, iViewTypeInStringFormat);
	}

/**
 * Sets the view index of the TViewData object
 * @param aViewIndex - view index
 */ 
inline void TViewData::SetViewIndex(TInt aViewIndex)
	{
	iViewIndex = aViewIndex;	
	}
	
/**
 * returns the view index of the TViewData object
 * @return TInt - view index
 */ 
inline TInt TViewData::ViewIndex()
	{
	return iViewIndex;
	}

/**
 * returns the view type information of the TViewData Object in String format
 * @return TPtrC - pointer to view type information of the TViewData Object
 */ 
inline TPtrC TViewData::ViewTypeInStringFormat()
	{
	return iViewTypeInStringFormat;		
	}

/**
 * returns the view type information of the TViewData Object in TViewData::TContactViewType format
 * @return TViewData::TContactViewType - Object of type TViewData::TContactViewType
 */ 
inline TViewData::TContactViewType TViewData::ViewType()
	{
	return 	iViewType;	
	}	

/**
 * This class extends TViewData, adds support by storing information about number of contacts to be removed.
 */
class TViewModificationData : public TViewData
	{
	public:
		void SetNumberOfContactsToBeRemoved(const TInt aCount);
		TInt NumberOfContactsToBeRemoved();
	
	private:
		TInt	iNumOfContactsToBeRemoved;	
		
	};
	
	
// Sets the desired number of contacts to be removed 

/**
 * Sets the desired number of contacts to be removed 
 * @param TInt - number of contacts to be removed  
 */ 
inline void TViewModificationData::SetNumberOfContactsToBeRemoved(const TInt aCount)
		{
		iNumOfContactsToBeRemoved = aCount;			
		}

/**
 * Returns the number of contacts to be removed 
 * @return TInt - number of contacts to be removed  
 */
inline TInt TViewModificationData::NumberOfContactsToBeRemoved()
		{
		return	iNumOfContactsToBeRemoved;		
		}	

/**
 * This class extends TViewData, adds support by storing information about expected view count after view update.
 */
class TViewValidationData : public TViewData
	{
	public:
		void SetExpectedViewCount(const TInt aCount);
		TInt ExpectedViewCount();

	private:
		TInt	iNumOfContactsAfterViewUpdate;
	};



/**
 * Sets the desired number expected contacts 
 * @param TInt - number of contacts to be removed  
 */ 
inline void TViewValidationData::SetExpectedViewCount(const TInt aCount)
		{
		iNumOfContactsAfterViewUpdate = aCount;			
		}

/**
 * Returns the number of contacts expected after view update
 * @return TInt - number of contacts expected after view update  
 */inline TInt TViewValidationData::ExpectedViewCount()
		{
		return	iNumOfContactsAfterViewUpdate;		
		}


/**
 * This test step should Update of base views and validating the view update in
 * the dependant views and view notifications
 */
class CTestContactViewUnderlyingViewUpdateStep : public CTestContactViewDatabaseUtilitiesStep
	{
	public:
		CTestContactViewUnderlyingViewUpdateStep();
		
		TVerdict 	doTestStepPreambleL();
		TVerdict 	doTestStepL();		
		
		void 		ReadIniAndConstructViewUpdateObjectsL();
		void 		ReadIniAndConstructViewValidationObjectsL();		
		void 		RemoveContactsFromTheUnderLyingViewsL();
		void 		ValidateViewCountL();
		void 		ListenForViewEventsAndValidateL();

	private:
		CActiveScheduler* 					iScheduler;
		RArray<TViewModificationData> 		iViewUpdateArray;
		RArray<TViewValidationData> 		iViewValidationArray;
	};

#endif
