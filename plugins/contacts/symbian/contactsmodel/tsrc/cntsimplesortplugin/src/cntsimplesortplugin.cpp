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


#include <cntviewsortplugin.h>
#include <ecom/implementationproxy.h>
#include "cntsimplesortplugin.hrh"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <cntviewsortpluginbase.h>
#endif




// For simplicity Class definition and declaration in the same file.


/**
	@class CViewSortPluginExample
	Intended usage: This class implements the functionality promised by 
	the CSortUtilInterface defintion class. It does little apart from 
	implementing a simple sort,as a demonstration of using ECOM to load a test plugin.
 */
class CViewSortPluginExample : public CViewContactSortPlugin
{
public:
	/** Factory function
	*/
	static CViewSortPluginExample* NewL(TSortPluginParams* aParams);


	// Implementation of CView
	void SetSortOrderL(const RContactViewSortOrder& aViewSortOrder);
	TInt SortStart(TSortStartTypes aSortStartType, TInt aToCount);
	void SortCompleted();

	TInt SortCompareViewContactsL(const CViewContact& aLhs, const CViewContact& aRhs);
	TBool ViewContactIsSortable(const CViewContact& aViewContact);


	// functions not in VTable:
public:
	CViewSortPluginExample();					
	void ConstructL(TSortPluginParams* aParams);
private:
	~CViewSortPluginExample();

protected:
	RContactViewSortOrder		iViewSortOrder;
	TSortStartTypes				iCurrentSort;
	TInt						iToCount;
	// Parameter block given to NewL
	TSortPluginParams			iPluginParams;
	// Parameters from View
	TSortPluginViewParamsRev1	iPluginViewParams;
	};
	


// __________________________________________________________________________
// Implementation



CViewSortPluginExample::CViewSortPluginExample() : CViewContactSortPlugin()
	{
	}


CViewSortPluginExample* CViewSortPluginExample::NewL(TSortPluginParams* aParams)
	{
	CViewSortPluginExample* self = new (ELeave) CViewSortPluginExample();  // calls c'tor
	CleanupStack::PushL(self);
	self->ConstructL(aParams);
	CleanupStack::Pop();	// self
	return(self);
	}



void	CViewSortPluginExample::ConstructL(TSortPluginParams* aParams)
	{
	// parameters 
	__ASSERT_DEBUG(aParams, User::Invariant());

	// Check parameters UID
	if (!aParams || (aParams->iParametersRevision != KCntSortPluginViewParamsRev1Uid))
		User::Leave(KErrArgument);


	// Parameters are valid, copy
	iPluginParams = *aParams;

	// View Parameters pointer must not be NULL
	__ASSERT_DEBUG(aParams->iViewSortParams, User::Invariant());

	// COPY View parameters
	iPluginViewParams = *(reinterpret_cast<TSortPluginViewParamsRev1*> (aParams->iViewSortParams));

	// Function pointers must not be NULL
	__ASSERT_DEBUG(iPluginViewParams.iCompareViewContactsL, User::Invariant());
	__ASSERT_DEBUG(iPluginViewParams.iIsSortable, User::Invariant());
	RDebug::Print(_L("-->Instantiated example sort plugin\n"));

	}



CViewSortPluginExample::~CViewSortPluginExample()
	{
	// Class must free all memory
	iViewSortOrder.Close();
	RDebug::Print(_L("-->Deleted example sort plugin\n"));
	}



/** Used by the Contacts View to tell the plugin the required sort order.
Any processing of the order information is done here. */
void CViewSortPluginExample::SetSortOrderL(const RContactViewSortOrder& aViewSortOrder)
	{ 
	iViewSortOrder.CopyL(aViewSortOrder); 
	}



/* Notify Start of sort;
indicating type of operation and approx number of contacts to be processed
*/
TInt CViewSortPluginExample::SortStart(TSortStartTypes aSortStartType, TInt aToCount) 
	{
	// Sort should have NOT have started
	__ASSERT_DEBUG( iCurrentSort == ESortNull, User::Invariant());

	// sort is starting
	iCurrentSort = aSortStartType;
	iToCount = aToCount;

	// Can allocate buffers here if required ...

	return KErrNone;
	}



// Notify completion of sort
void CViewSortPluginExample::SortCompleted()
	{
	// Sort should have started previously
	__ASSERT_DEBUG( iCurrentSort != ESortNull, User::Invariant());

	// sort has finished
	iCurrentSort = ESortNull;

	// release buffers here
	// ...
	}



/** Compare 2 contacts for Contacts View Sort, Insert or Update
 */
TInt CViewSortPluginExample::SortCompareViewContactsL(const CViewContact& aFirst, const CViewContact& aSecond)
	{
	// must be notified before a sort starts
	__ASSERT_DEBUG( iCurrentSort != ESortNull, User::Invariant());

	// must have a Sort Order
	__ASSERT_DEBUG( iViewSortOrder.Count() > 0, User::Invariant());


	// result of comparison: Zero = fields are identical

	// pass to default compare function
	TInt comparison = iPluginViewParams.iCompareViewContactsL(aFirst, aSecond);


	// REVERSE normal sort order
	return -comparison;
	}



TBool CViewSortPluginExample::ViewContactIsSortable(const CViewContact& aContact)
	{
	// pass to default Sortable Contact test
	return iPluginViewParams.iIsSortable(aContact);
	};



//***********************************************************************************
//
// DLL Information for ECOM

/** Exported proxy for instantiation method resolution
Define the interface UIDs */
const TImplementationProxy ImplementationTable[] = 
	{
		// old style: {{KCntSortPluginImplement1Uid},	CViewSortPluginExample::NewL},
		// Required for EABI compiler:
		IMPLEMENTATION_PROXY_ENTRY(KCntSortPluginImplement1UidInt, CViewSortPluginExample::NewL),
	};



EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}
