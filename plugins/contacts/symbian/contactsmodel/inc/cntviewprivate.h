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


#ifndef __CNTVIEWPRIVATE_H__
#define __CNTVIEWPRIVATE_H__

#include <cntviewbase.h>
#include "cntphbksyncwatcher.h"

/** 
 * This class adds the ability to change sort orders to the functionality
 * provided by CContactLocalView. If the sort order is changed, a notification
 * will be sent to all observers. This class is only intended for use by the 
 * contact server. Clients should use CContactNamedRemoteView.
 */
class CContactNamedLocalView : public CContactLocalView
	{
public:
	IMPORT_C static CContactNamedLocalView* NewL(MContactViewObserver& aObserver,const TDesC& aName,const CContactDatabase& aDb,const RContactViewSortOrder& aSortOrder,TContactViewPreferences aContactTypeToInclude);
	IMPORT_C static CContactNamedLocalView* NewL(MContactViewObserver& aObserver,const TDesC& aName,const CContactDatabase& aDb,const RContactViewSortOrder& aSortOrder,TContactViewPreferences aContactTypeToInclude, const TDesC8& aSortPluginName);
	IMPORT_C static CContactNamedLocalView* NewL(MContactViewObserver& aObserver,const TDesC& aName,const CContactDatabase& aDb,const RContactViewSortOrder& aSortOrder,TContactViewPreferences aContactTypeToInclude, MLplPersistenceLayerFactory* aFactory, const TDesC8& aSortPluginName);
	IMPORT_C const TDesC& Name();
	IMPORT_C void ChangeSortOrderL(RContactViewSortOrder& aSortOrder);
private:
	~CContactNamedLocalView();
	CContactNamedLocalView(const CContactDatabase& aDb,TContactViewPreferences aContactTypes,MLplPersistenceLayerFactory* aFactory);
	CContactNamedLocalView(const CContactDatabase& aDb,TContactViewPreferences aContactTypeToInclude);
	void ConstructL(MContactViewObserver& aObserver,const TDesC& aName,const RContactViewSortOrder& aSortOrder,
										TBool aUseNamedPlugin, const TDesC8& aSortPluginName);
private:
	HBufC* iName;
	};

/** 
 * This class is owned by CContactRemoteViewBase and is used to 
 * receive view events
 */
NONSHARABLE_CLASS (CContactRemoteViewNotifier) : public CActive
	{
public:
	CContactRemoteViewNotifier(RContactRemoteView& aView,TCallBack aCallBack);
	~CContactRemoteViewNotifier();
	void Start();
	const TContactViewEvent& Event() const;
private: // From CActive.
	void RunL();
	void DoCancel();
private:
	RContactRemoteView& iView;
	TPckgBuf<TContactViewEvent> iEvent;
	TCallBack iCallBack;
	};

	
class MContactViewSortObserver;

/** 
 * This class extends CAsyncOneShot to allow an initial notification request 
 * to be made to the phonebook synchroniser plug-in for ICC views.
 * 
 * This allows delaying the construction of the view until the ICC 
 * is synchronised and the phonebook synchroniser is in a cache-valid
 * state.
 */
NONSHARABLE_CLASS (CIdleContactSorter) : public CAsyncOneShot, public MContactPhbkSyncObserver
	{
private:
	enum TSorterState
		{
		EInsertSortFinal,
		EInsertContactsOnlyIccLocked,
		EWaitingForInitialICCReady,
		EContactsReadyWaitICCUnlock,
		ESortAllDone
		};

public:
	static CIdleContactSorter* NewL(MContactViewSortObserver& aObserver, MLplPersistenceLayerFactory& aFactory, TContactViewPreferences aRequestedViewPrefs);
	~CIdleContactSorter();
	TBool IsICCSynchronised() const;
	TContactViewPreferences SortViewPreferences() const;
	TBool InsertViewPreferences(TContactViewPreferences &aInsertView) const;
	void Start();
	void Stop();
	
private: // From CAsyncOneShot
	CIdleContactSorter(MContactViewSortObserver& aObserver, TContactViewPreferences aRequestedViewPrefs);
	void ConstructL(MLplPersistenceLayerFactory& aFactory);
	
	void DoCancel();
	void RunL();
	TInt RunError(TInt aError);

	void ChangeSortState(TSorterState aNewSortState);
	
	// From MContactPhbkSyncObserver
	void ContactPhbkSyncEventHandler(TPhonebookState aPhbkState);

private:
	MContactViewSortObserver&	iSortObserver;
	
	// requested view
	TContactViewPreferences iRequestedView;
	// non-NULL if ICC contacts are included in view, & therefore a sync is expected
	CContactPhbkSyncWatcher* iPhbkSyncWatcher;
	
	// subset of view we are actually insert sorting now
	TContactViewPreferences iSortView;
	TSorterState iSortState;
	};

	
/**
 * Template classes to call ResetAndDestroy on array classes
 * automatically, when ::PopAndDestroy'ing from the cleanup stack.
 */
template <class T>
class CleanupResetAndDestroy
	{
public:
	inline static void PushL(T& aRef);
private:
	static void ResetAndDestroy(TAny *aPtr);
	};
template <class T>
inline void CleanupResetAndDestroyL(T& aRef);

// Template class CleanupClose
template <class T>
inline void CleanupResetAndDestroy<T>::PushL(T& aRef)
	{CleanupStack::PushL(TCleanupItem(&ResetAndDestroy, &aRef));}
template <class T>
void CleanupResetAndDestroy<T>::ResetAndDestroy(TAny *aPtr)
	{static_cast<T*>(aPtr)->ResetAndDestroy();}
template <class T>
inline void CleanupResetAndDestroyPushL(T& aRef)
	{CleanupResetAndDestroy<T>::PushL(aRef);}

#endif //__CNTVIEWPRIVATE_H__
