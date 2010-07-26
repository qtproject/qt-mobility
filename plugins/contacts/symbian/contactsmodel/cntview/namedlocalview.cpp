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


#include <cntviewbase.h>
#include "cntviewprivate.h"


//
// CContactNamedLocalView.
//

CContactNamedLocalView::~CContactNamedLocalView()
	{
	delete iName;
	}

CContactNamedLocalView::CContactNamedLocalView(const CContactDatabase& aDb,TContactViewPreferences aContactTypes) : CContactLocalView(aDb,aContactTypes)
	{
	}

CContactNamedLocalView::CContactNamedLocalView(const CContactDatabase& aDb,TContactViewPreferences aContactTypes,MLplPersistenceLayerFactory* aFactory) 
: CContactLocalView(aDb,aContactTypes,aFactory)
	{
	}

EXPORT_C CContactNamedLocalView* CContactNamedLocalView::NewL(MContactViewObserver& aObserver,const TDesC& aName,const CContactDatabase& aDb,const RContactViewSortOrder& aSortOrder,TContactViewPreferences aContactTypes)
	{
	CContactNamedLocalView* self=new(ELeave) CContactNamedLocalView(aDb,aContactTypes);
	CleanupStack::PushL(self);
	self->ConstructL(aObserver, aName, aSortOrder, EFalse, KNullDesC8);
	CleanupStack::Pop(self);
	return self;
	}

void CContactNamedLocalView::ConstructL(MContactViewObserver& aObserver,const TDesC& aName,const RContactViewSortOrder& aSortOrder,
										TBool aUseNamedPlugin, const TDesC8& aSortPluginName)
	{
	iName=aName.AllocL();
	if(aSortPluginName.Length())
		{
		CContactLocalView::ConstructL(aObserver, aSortOrder, aUseNamedPlugin, aSortPluginName);
		}
		else
		{
		CContactLocalView::ConstructL(aObserver, aSortOrder);
		}

	}


EXPORT_C CContactNamedLocalView* CContactNamedLocalView::NewL(MContactViewObserver& aObserver,const TDesC& aName,const CContactDatabase& aDb,const RContactViewSortOrder& aSortOrder,TContactViewPreferences aContactTypes, const TDesC8& aSortPluginName)
	{
	CContactNamedLocalView* self=new(ELeave) CContactNamedLocalView(aDb,aContactTypes);
	CleanupStack::PushL(self);
	self->ConstructL(aObserver, aName, aSortOrder, ETrue, aSortPluginName);
	CleanupStack::Pop(self);
	return self;
	}

/** CContactNamedLocalView contructor, used in the server
@internalTechnology 
 */
EXPORT_C CContactNamedLocalView* CContactNamedLocalView::NewL(MContactViewObserver& aObserver,const TDesC& aName,const CContactDatabase& aDb,const RContactViewSortOrder& aSortOrder,TContactViewPreferences aContactTypes, MLplPersistenceLayerFactory* aFactory, const TDesC8& aSortPluginName)
	{
	CContactNamedLocalView* self=new(ELeave) CContactNamedLocalView(aDb,aContactTypes,aFactory);
	CleanupStack::PushL(self);
	self->ConstructL(aObserver, aName, aSortOrder, ETrue, aSortPluginName);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C const TDesC& CContactNamedLocalView::Name()
	{
	return *iName;
	}

EXPORT_C void CContactNamedLocalView::ChangeSortOrderL(RContactViewSortOrder& aSortOrder)
	{
	SetState(ENotReady);
	TRAPD(err,SortL(aSortOrder));
	if (err)
		{
		NotifyObservers(TContactViewEvent(TContactViewEvent::ESortError,err));
		}
	else
		{
		iSortOrder.CopyL(aSortOrder);
		}
	}

