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


#ifndef __CNTVIEWSORTPLUGIN_H__
#define __CNTVIEWSORTPLUGIN_H__

#include <e32base.h>
#include "ecom/ecom.h"		// For REComSession
#include <cntviewbase.h>



/** The UID of the ECOM contact view sort interface. 
@publishedAll
@released
*/
const TUid KCntSortPluginInterfaceUid = {0x10200FBD};

/** The name of the default ECOM contact view sort plug-in. 
@publishedAll
@released
*/
_LIT8(KViewSortPluginDefaultName, "/default");

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS

//********************************************************************************************
//
// Parameter blocks passed to Sort Plug-in's NewL, where they should be copied.
//

/** The UID for the sort plug-in parameters (TSortPluginParams and 
TSortPluginViewParamsRev1) that are used to pass data to the ECOM plug-in DLL. 
@publishedPartner
@released
*/
const TUid KCntSortPluginViewParamsRev1Uid = {0x10201325};

/** Information block holding the sort plug-in's parameters.
This is passed to the plug-in instance from the contact view via 
TSortPluginParams::iViewSortParams.

It has an inline constructor to initialise the data members.

@publishedPartner
@released */
class TSortPluginViewParamsRev1
	{
public:	// constructors
	/** Empty default constructor. */
	TSortPluginViewParamsRev1()
			{}
	/** Inline constructor, initialises all data members. 
	
	@param aViewPreferences The view's view preferences.
	@param aCompareViewContactsL A pointer to a default function that is used 
	to compare two contact items. The plug-in may call this function from its 
	implementation of SortCompareViewContactsL().
	@param aIsSortable A pointer to a default function that is used to test 
	whether a CViewContact is sortable. The plug-in may call this function 
	from its implementation of ViewContactIsSortable(). */
	TSortPluginViewParamsRev1(TContactViewPreferences aViewPreferences,
		TInt(*aCompareViewContactsL)(const CViewContact& aFirst, const CViewContact& aSecond),
		TBool(*aIsSortable)(const CViewContact& aViewContact)) :
		iViewPreferences(aViewPreferences), iCompareViewContactsL(aCompareViewContactsL), iIsSortable(aIsSortable)
			{}
public:
	// Fields in all versions
	/** The view's view preferences.*/
	TContactViewPreferences		iViewPreferences;		// preferences of view (at creation)

	// Default implementations of Compare and IsSortable methods
	// that the sort Plugin can call.
	// Must not be NULL !
	/** A pointer to a default function that is used to compare two contact 
	items. The plug-in may call this function from its implementation of 
	SortCompareViewContactsL(). */
	TInt (*iCompareViewContactsL)(const CViewContact& aFirst, const CViewContact& aSecond);
	/** A pointer to a default function that is used to test whether a contact item 
	is sortable. The plug-in may call this function from its implementation of 
	ViewContactIsSortable(). */
	TBool(*iIsSortable)(const CViewContact& aViewContact);
	};


/** A data class used to pass initialization information to 
CViewContactSortPlugin::NewL().

@publishedPartner
@released */
class TSortPluginParams
	{
public:	// constructors
	/** Empty default constructor. */
	TSortPluginParams() {}
	
	/** Inline constructor, initialises all data members.

	The parameter revision UID is initialised to KCntSortPluginViewParamsRev1Uid.
	@param aInterfaceUid Interface UID as specified in the INTERFACE_INFO resource struct, 
	required by ECOM.
	@param aImplementationUid Implementation UID, as specified in the IMPLEMENTATION_INFO 
	resource struct, required by ECOM.
	@param aViewSortParams View sort parameters pointer. This object should match the 
	parameter revision UID value. */
	TSortPluginParams(TUid aInterfaceUid, TUid aImplementationUid, TSortPluginViewParamsRev1* aViewSortParams) : 
		iParametersRevision(KCntSortPluginViewParamsRev1Uid), iInterfaceUid(aInterfaceUid), 
		iImplementationUid(aImplementationUid), iViewSortParams(aViewSortParams)
	{}
public:
	/** A UID that identifies the revision of the structure holding 
	the view's sort parameters. A value of KCntSortPluginViewParamsRev1Uid 
	indicates TSortPluginViewParamsRev1. */
	TUid	iParametersRevision;	

	// Information for ECOM to load the correct plug-in
	/** Interface UID as specified in the INTERFACE_INFO resource struct, 
	required by ECOM.*/
	TUid	iInterfaceUid;
	/** Implementation UID, as specified in the IMPLEMENTATION_INFO 
	resource struct, required by ECOM.*/
	TUid	iImplementationUid;

	// information block from Contact View
	/** View sort parameters pointer. This object should match the 
	parameter revision UID value.*/
	TAny*	iViewSortParams;
	};



/** An interface class that enables implementers to configure the way in which 
contacts are sorted in a contact view.

This is an abstract base class that will be derived from.

@publishedPartner
@released */
class CViewContactSortPlugin : public CBase
	{
public:
	/** Identifies the type of sort operation to SortStart(). 
	
	@publishedPartner
	@released */
	enum TSortStartTypes {
		/** No sort is in progress. */
		ESortNull = 0,
		/** A full sort or re-sort. */
		ESortStartFull,	
		/** A single contact has been inserted or changed. */
		ESortStartInsertOne,
		/** Multiple contacts have been added. For example, ICC contacts arrived in a 
		mixed view (one that also includes contacts from the phone's memory). */
		ESortStartInsertMultiple,
		// other values are reserved
		};
	
	/* aImplementationUid is used by ECOM to select plugin.
	Contacts model can use REComSession::ListImplementationsL() to determine
	DLLs that match the interface UID for this class. */
	static CViewContactSortPlugin* NewL(TSortPluginParams* aParams);

	/* Class must free all memory, implementation should include:
	iViewSortOrder.Close();	*/
	inline virtual ~CViewContactSortPlugin();


	//pure virtual methods to be implemented by the plugin.

	/** Called by a contact view to pass the sort plug-in the required sort order.
	Any processing of the order information is done here.

    This function is called when the view is created and when a different sort order 
	is requested. 

	@param aViewSortOrder The requested sort order. */
	virtual void SetSortOrderL(const RContactViewSortOrder& aViewSortOrder) = 0;

	/** Called by a contact view to notify the sort plug-in that sorting is about to start.

	@param aSortStartType Indicates the type of sort operation required.
	@param aCount The approximate number of contacts to be processed.
	@return KErrNone if successful, otherwise another of the system-wide error codes. */
	virtual TInt SortStart(TSortStartTypes aSortStartType, TInt aCount) = 0;

	/** Called by a contact view to notify the sort plug-in that sorting is complete. */
	virtual void SortCompleted() = 0;

	/** Called by a contact view to compare two CViewContact items for sorting in the 
	view.

	If TDesC::CompareC() is used to implement this function, the return value from 
	TDesC::CompareC() can be used directly.

	@param aLhs The first item to compare.
	@param aRhs The second item to compare.
	@return Zero means that aLhs and aRhs have an equal sorting order.
	An arbitrary negative value means that aLhs is to be sorted before aRhs. 
	An arbitrary positive value means that aRhs is to be sorted before aLhs. */
	virtual TInt SortCompareViewContactsL(const CViewContact& aLhs, const CViewContact& aRhs) = 0;

	/** Tests whether a CViewContact is sortable. 
	
	For instance, a contact may be unsortable if none of the fields used to sort on 
	contain any data. 
	@param aViewContact The view contact to test.
	@return ETrue if the view contact is sortable, EFalse if not. */
	virtual TBool ViewContactIsSortable(const CViewContact& aViewContact) = 0;

private:
	// The uid is stored here, so that it can be used during destruction of the instance.
	TUid iDtor_ID_Key;
	};


//********************************************************************************************
//
// NewL() & destructor inline functions, placed in .h file for simplicity
//

// -----------------------CViewContactSortPlugin --------------------------------

/** Allocates and constructs a sort plug-in instance.

@param aParams The sort plug-in's parameters.
@return Pointer to the newly created sort plug-in implementation. */
inline CViewContactSortPlugin* CViewContactSortPlugin::NewL(TSortPluginParams* aParams)
	{
	TAny* ptr = REComSession::CreateImplementationL(aParams->iImplementationUid,
				_FOFF(CViewContactSortPlugin, iDtor_ID_Key),
				aParams);
	return reinterpret_cast<CViewContactSortPlugin*>(ptr);
	}

/** Virtual destructor. */
inline CViewContactSortPlugin::~CViewContactSortPlugin()
	{
	REComSession::DestroyedImplementation(iDtor_ID_Key);
	}
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS

#endif
