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


#include <e32base.h>
#include <centralrepository.h>
#include <cntdb.h>
#include "cntstd.h"
#include "rcntmodel.h"
#include <cntviewbase.h>
#include <cntviewstore.h>
#include "cntviewstoreprivate.h"


/*
  Constant values used to access the view definition data 
  in the central repository.
*/
//const TUint32 KTableMask = 0x00FF0000;
const TUint32 KRowMask = 0x0000FF00;
const TUint32 KColumnMask = 0x000000FF;

const TUint32 KViewDefinitionsTable = 0x00010000;
const TUint32 KViewSortOrderTable = 0x00020000;
const TUint32 KTypeCol = 0x00000000;
const TUint32 KNameCol = 0x00000001;
const TUint32 KSortPluginCol = 0x00000002;
const TUint32 KPreferencesCol = 0x00000003;
const TUint32 KSortOrderIdxCol = 0x00000004;

const TUid KContactsRepositoryUid = { 0x10003A73 };

/** 
	Stores a list of default remote view definitions in the contacts servers' 
    persistent data store. The view definitions are assumed to relate to 
    the default contacts database although this is not enforced.

@param aDefaultViews Array of view definitions to store

@publishedPartner
@released
*/
EXPORT_C void TContactViewStore::SetDefaultViewsL(const RPointerArray<CContactDefaultViewDefinition> &aDefaultViews)
	{
	CRepository* rep = CRepository::NewLC(KContactsRepositoryUid);
	
	// Attempt to store in one transaction; if it fails, pass
	// the error on as a leave.
	rep->StartTransaction(CRepository::EConcurrentReadWriteTransaction);

	// clear out any existing entries
	TUint32 errorKey;
	rep->Delete(KViewDefinitionsTable, ~(KRowMask | KColumnMask), errorKey);
	rep->Delete(KViewSortOrderTable, ~(KRowMask | KColumnMask), errorKey);
	
	// for each view definition
	TInt count = aDefaultViews.Count();
	for (TInt i = 0; i < count; i++)
		{
		CContactDefaultViewDefinition* def = aDefaultViews[i];

		// View Type
		TUint32 key = (KViewDefinitionsTable | (i << 8) | KTypeCol);
		rep->Set(key, def->ViewType());
				
		// View Name
		key = (KViewDefinitionsTable | (i << 8) | KNameCol);
		rep->Set(key, def->ViewNameL());
		
		// Sort Plugin Name
		key = (KViewDefinitionsTable | (i << 8) | KSortPluginCol);
		rep->Set(key, def->SortPluginNameL());

		// View Preferences
		key = (KViewDefinitionsTable | (i << 8) | KPreferencesCol);
		rep->Set(key, def->ViewPreferences());
		
		// Sort Order Row Index
		key = (KViewDefinitionsTable | (i << 8) | KSortOrderIdxCol);
		rep->Set(key, i);
		
		// Write the entries for the sort order in the sort order table
		// Need Assert that Count() <= 256 (i.e. 0xFF)
		TInt fieldCount = def->SortOrder().Count();
		for (TInt j = 0; j < fieldCount; j++)
			{
			key = (KViewSortOrderTable | (i << 8) | j);
			rep->Set(key, static_cast<TInt>(def->SortOrder()[j].iUid));
			}
		}

	User::LeaveIfError(rep->CommitTransaction(errorKey));
		
	CleanupStack::PopAndDestroy(rep);	
	}

/** 
	Retrieves the list of default view definitions stored in the contacts
    servers' persistent data storage.
    
@publishedPartner
@released
*/	
EXPORT_C void TContactViewStore::GetDefaultViewsL(RPointerArray<CContactDefaultViewDefinition> &aDefaultViews)
	{
	__ASSERT_ALWAYS(aDefaultViews.Count() == 0, User::Leave(KErrArgument));
	
	CRepository* rep = CRepository::NewLC(KContactsRepositoryUid);
	
	TInt type(1);
	TInt prefs(0);
	TInt sortOrderRow;
	RContactViewSortOrder viewSortOrder;
	CleanupClosePushL(viewSortOrder);
	TInt actualLength;
	RBuf name;
	name.CleanupClosePushL();
	RBuf8 pluginName;
	pluginName.CleanupClosePushL();	
	
	// Attempt to store in one transaction; if it fails, pass
	// the error on as a leave.
	rep->StartTransaction(CRepository::EConcurrentReadWriteTransaction);

	TInt row(0);
	const TUint32 KMask = ~KColumnMask;
	TUint32 partialKey = (KViewDefinitionsTable | (row << 8));
	RArray<TUint32> foundKeys;
	CleanupClosePushL(foundKeys);
	CContactDefaultViewDefinition* viewDef;
		
	TInt err = rep->FindL(partialKey, KMask, foundKeys);
	while (err != KErrNotFound)
		{
		TInt count = foundKeys.Count();		
		for (TInt col = 0; col < count; col++)
			{
			switch (foundKeys[col] & KColumnMask)
				{
				case KTypeCol:
					{
					err = rep->Get(foundKeys[col], type);
					break;
					}				
				case KNameCol:
					{
					err = rep->Get(foundKeys[col], name, actualLength);
					if (err == KErrOverflow)
						{
						name.ReAllocL(actualLength);
						rep->Get(foundKeys[col], name);
						}
					break;
					}
				case KSortPluginCol:
					{
					err = rep->Get(foundKeys[col], pluginName, actualLength);
					if (err == KErrOverflow)
						{
						pluginName.ReAllocL(actualLength);
						rep->Get(foundKeys[col], pluginName);
						}							
					break;
					}
				case KPreferencesCol:
					{
					rep->Get(foundKeys[col], prefs);
					break;
					}
				case KSortOrderIdxCol:
					{
					rep->Get(foundKeys[col], sortOrderRow);
					
					// read in the sort order from the sort order table
					RArray<TUint32> foundFieldKeys;
					partialKey = (KViewSortOrderTable | (sortOrderRow << 8));
					err = rep->FindL(partialKey, KMask, foundFieldKeys);
					CleanupClosePushL(foundFieldKeys);
					for (TInt field = 0; field < foundFieldKeys.Count(); field++)
						{
						TInt fieldUid;
						rep->Get(foundFieldKeys[field], fieldUid);
						viewSortOrder.AppendL(TUid().Uid(fieldUid));
						}
					CleanupStack::PopAndDestroy(&foundFieldKeys);
					break;
					}
				default:
					{
					break;
					}
				}
			}
													   
		viewDef = CContactDefaultViewDefinition::NewLC(static_cast<CContactDefaultViewDefinition::TViewType>(type),
													   name, viewSortOrder, 
													   static_cast<TContactViewPreferences>(prefs), 
													   pluginName);													   
			
		aDefaultViews.AppendL(viewDef);
		CleanupStack::Pop(viewDef);	
		viewSortOrder.Close();

		row++;
		partialKey = (KViewDefinitionsTable | (row << 8));
		err = rep->FindL(partialKey, KMask, foundKeys);
		}

	TUint32 errorKey;
	User::LeaveIfError(rep->CommitTransaction(errorKey));

	CleanupStack::PopAndDestroy(5, rep); // foundKeys, pluginName, name, viewSortOrder, rep
	}
	
/**
  Returns an array of definitions describing the remote views that exist 
  inside the server for the given contact database.
  Note: The order of views in the array is not guaranteed to be the order 
  that they were created in.

@param aDbName Name of the contacts database to query for open views.
@param aViewDefs An empty array that will be filled with any exisiting
                 open views for the given contact database.

@internalTechnology
@test
*/
#if defined(_DEBUG)
EXPORT_C void TContactViewStorePrivate::GetDefinitionsOfExistingViewsL(const TDesC& aDbName, RPointerArray<CContactDefaultViewDefinition>& aViewDefs)
	{
	RCntModel session;
	CleanupClosePushL(session);
	session.ConnectL();
	session.GetDefinitionsOfExistingViewsL(aDbName, aViewDefs);
	CleanupStack::PopAndDestroy(); // session
	}
#else
EXPORT_C void TContactViewStorePrivate::GetDefinitionsOfExistingViewsL(const TDesC& , RPointerArray<CContactDefaultViewDefinition>& )
	{
	}
#endif // _DEBUG
	
/**  Static constructor. Returns a default remote view definition.

@param	aViewType	Type of the view. 
@param	aViewName	The name of the view. Ignored if aViewType = ERemoteView; 
@param 	aSortOrder	Specifies the fields to use to sort the items in the view.
@param 	aContactTypes	Specifies which types of contact items should be included in the
view and the behaviour for items that do not have content in any of the fields specified in the sort order.
@param 	aSortPluginName	Specifies a plug-in that will be used to compare view contacts
when the the view is sorted. This name is used by ECOM to select the plugin, and is matched
with the "default_data" of all ECOM plugins that support the required interface.
@return The newly constructed object.

@publishedPartner
@released
*/
EXPORT_C  CContactDefaultViewDefinition* CContactDefaultViewDefinition::NewLC(TViewType aViewType, const TDesC &aViewName, const RContactViewSortOrder &aSortOrder, TContactViewPreferences aContactTypes, const TDesC8 &aSortPluginName)
	{
	CContactDefaultViewDefinition* self = new (ELeave) CContactDefaultViewDefinition(aViewType, aContactTypes);
	CleanupStack::PushL(self);
	self->ConstructL(aViewName, aSortOrder, aSortPluginName);
	return self;	
	}

/**  Static constructor. Returns a default remote view definition created from the contents
     of the given stream.

@param aStream Stream which the parmameters for the view definition are read from

@return The newly constructed object.

@publishedPartner
@released
*/
EXPORT_C  CContactDefaultViewDefinition* CContactDefaultViewDefinition::NewLC(RReadStream& aStream)
	{
	CContactDefaultViewDefinition* self = new (ELeave) CContactDefaultViewDefinition;
	CleanupStack::PushL(self);
	aStream >> *self;
	return self;	
	}
	
CContactDefaultViewDefinition::CContactDefaultViewDefinition(TViewType aViewType, TContactViewPreferences aContactTypes):
	iContactTypes(aContactTypes),
	iViewType(aViewType)
	{
	}
	
CContactDefaultViewDefinition::CContactDefaultViewDefinition()
	{
	}
	
void CContactDefaultViewDefinition::ConstructL(const TDesC &aViewName, const RContactViewSortOrder &aSortOrder, const TDesC8 &aSortPluginName)
	{
	iViewName = aViewName.AllocL();
	iSortPluginName = aSortPluginName.AllocL();
	iSortOrder.CopyL(aSortOrder);
	}	

/**
@publishedPartner
@released
*/
EXPORT_C CContactDefaultViewDefinition::~CContactDefaultViewDefinition()
	{
	Close();
	}
	
void CContactDefaultViewDefinition::Close()
	{
	iSortOrder.Close();
	delete iSortPluginName;
	iSortPluginName = NULL;
	delete iViewName;	
	iViewName = NULL;
	}

/** Gets the view name.
@return The view name.

@publishedPartner
@released
*/
EXPORT_C const TDesC& CContactDefaultViewDefinition::ViewNameL() const
	{ return *iViewName; }

/** Gets the sort plugin name.
@return The sort plugin name.

@publishedPartner
@released
*/
EXPORT_C const TDesC8& CContactDefaultViewDefinition::SortPluginNameL() const
	{ return *iSortPluginName; }

/** Gets the sort order.
@return The sort order.

@publishedPartner
@released
*/
EXPORT_C const RContactViewSortOrder& CContactDefaultViewDefinition::SortOrder() const
	{ return iSortOrder; }

/** Gets the view preferences.
@return The view preferences.

@publishedPartner
@released
*/
EXPORT_C TContactViewPreferences CContactDefaultViewDefinition::ViewPreferences() const
	{ return iContactTypes; }

/** Gets the view type.
@return The view type.

@publishedPartner
@released
*/
EXPORT_C CContactDefaultViewDefinition::TViewType CContactDefaultViewDefinition::ViewType() const
	{ return iViewType; }

/** Standard object internalization function.
@param aStream Stream from which the object is internalized.

@publishedPartner
@released
*/
EXPORT_C void CContactDefaultViewDefinition::InternalizeL(RReadStream &aStream)
	{
	// free any existing data
	Close();
	
	// read in the new data
	const TInt KMaxDescriptorLength(KMaxTInt);
	iViewName = HBufC::NewL(aStream, KMaxDescriptorLength);
	iSortPluginName = HBufC8::NewL(aStream, KMaxDescriptorLength);
	aStream >> iSortOrder;
	iContactTypes = static_cast<TContactViewPreferences>(aStream.ReadUint32L());
	iViewType = static_cast<TViewType>(aStream.ReadUint32L());
	}

/** Standard object externalization function.
@param aStream Stream that the object is externalized to.

@publishedPartner
@released
*/
EXPORT_C void CContactDefaultViewDefinition::ExternalizeL(RWriteStream &aStream) const
	{
	aStream << *iViewName;
	aStream << *iSortPluginName;
	aStream << iSortOrder;
	aStream.WriteUint32L(static_cast<TUint32>(iContactTypes));
	aStream.WriteUint32L(static_cast<TUint32>(iViewType));
	}
