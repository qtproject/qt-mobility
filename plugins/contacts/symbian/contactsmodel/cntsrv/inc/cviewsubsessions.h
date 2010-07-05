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




/**
 @file
 @internalComponent
 @released
*/


#ifndef CVIEWSUBSESSIONS_H
#define CVIEWSUBSESSIONS_H


#include <e32base.h>

#include "persistencelayer.h"


class CViewSubSessionBase;
class CContactViewBase;
class CViewContact;
class RContactViewSortOrder;
class CContactLocalView;
class CContactNamedLocalView;
class CPersistenceLayer;
class CCntDbManager;
class CContactDefaultViewDefinition;


/**
The CViewSubSessionQueue class is used to queue view events on the Server for
later consumption by Client-side objects which make IPC calls to read events
from the queue.  An instance of this class is owned by each CViewSubSessionBase
object.
*/
class CViewSubSessionQueue : public CBase
	{
public:
	CViewSubSessionQueue();
	~CViewSubSessionQueue();
	void QueueEvent(const TContactViewEvent& aEvent);
	void RequestEvent(const RMessage2& aMessage);
	void CancelEventRequest();

private:
	void SendEventL(const TContactViewEvent& aEvent);

private:
	RArray<TContactViewEvent> iEvents;
	RMessage2 iMessage;
	TBool iRequestPending;
	TInt iQueueError;
	};


/**
The CViewManager class is responsible for managing Server-side local view
objects which are associated with Client-side remote view objects.

Each CCntSession object creates a CViewManager instance.

The CViewManager class constructs and manages one or more CContactLocalView or
CContactNamedLocalView instances and supplies them to CViewSubSession.
*/
class CViewManager : public CBase
	{
public:
	static CViewManager* NewL(MLplPersistenceLayerFactory& aFactory,CCntDbManager& aDbManager);
	CContactLocalView& OpenViewL(const RContactViewSortOrder& aSortOrder,MContactViewObserver& aObserver,TContactViewPreferences aContactTypeToInclude, const TUid aSortPluginImplUid, const TDesC8& aSortPluginName);
	CContactNamedLocalView& OpenNamedViewL(const TDesC& aName,const RContactViewSortOrder& aSortOrder,MContactViewObserver& aObserver,TContactViewPreferences aContactTypeToInclude, const TUid aSortPluginImplUid, const TDesC8& aSortPluginName);
	void CloseView(const CContactLocalView& aView,MContactViewObserver& aObserver);
	void CloseNamedView(const CContactNamedLocalView& aView,MContactViewObserver& aObserver);
	~CViewManager();
	void GetDefinitionsOfExistingViewsL(RPointerArray<CContactDefaultViewDefinition>& aViewDefs);

private:
	CViewManager(MLplPersistenceLayerFactory& aFactory,CCntDbManager& aManager);
	void ConstructL();

private:
	MLplPersistenceLayerFactory& iFactory;
	CCntDbManager&			iManager;
	struct TViewHandle
		{
		CContactLocalView*	iLocalView;
		TUid				iSortPluginImplUid;
		};
	RArray<TViewHandle>		iLocalViews;
	RPointerArray<CContactNamedLocalView> iNamedLocalViews;
	};


/**
The CViewSubSessionBase class is a base class for anonymous view subsessions.
Its main purpose is to provide methods for accessing CViewContact objects held
in the local views managed by CViewManager.

Instances of CViewSubSessionBase are constructed and managed by a CCntSession
instance using the subsession object container mechanism.  CCntSession calls the
derived CViewSubSessionBase::ServiceL() method after extracting the
CViewSubSessionBase handle from RMessage2 Int3()
*/
class CViewSubSessionBase : public CObject, public MContactViewObserver
	{
public:
	virtual TInt ServiceL(const RMessage2& aMessage);

protected:
	~CViewSubSessionBase();
	CViewSubSessionBase(CViewManager& aParent);
	void ConstructL();

private:
	virtual void HandleContactViewEvent(const CContactViewBase& aView,const TContactViewEvent& aEvent);
	void CountL(const RMessage2& aMessage) const;
	TInt AtL(const RMessage2& aMessage) const;
	void ContactAtL(const RMessage2& aMessage) const;
	TInt ContactAtLengthL(const RMessage2& aMessage);
	void FindL(const RMessage2& aMessage) const;
	TInt GetAllFieldsLengthL(const RMessage2& aMessage);
	void GetAllFieldsTextL(const RMessage2& aMessage);
	void ContactMatchingCriteriaExternalizedSizeL(const RMessage2& aMessage);
	void GetContactMatchingCriteriaL(const RMessage2& aMessage);
	void GetIncludedTypesL(const RMessage2& aMessage);
	void DeleteFindContacts();
	void RequestViewEvent(const RMessage2& aMessage);
	void CancelRequestViewEvent();
	void GetContactIdsL(const RMessage2& aMessage);
	void SendPluginUidToServer(const RMessage2& aMessage);
	void GetContactsMatchingFilterL(const RMessage2& aMessage);
	void GetSortPluginUidFromServerL(const RMessage2& aMessage);

protected:
	CViewManager& iViewManager;
	CContactViewBase* iView;
	HBufC* iSortableText;
	RPointerArray<CViewContact> iContacts;
	CViewContact* iContact;
	CViewSubSessionQueue* iQueue;
	};


/**
The CViewSubSession class implements anonymous view subsessions.
*/
class CViewSubSession : public CViewSubSessionBase
	{
public:
	static CViewSubSession* NewL(CViewManager& aParent,const RMessage2& aMessage);

protected:
	~CViewSubSession();
	CViewSubSession(CViewManager& aParent);
	void UnpackageSortOrderL(const RMessage2& aMessage,RContactViewSortOrder& aSortOrder,TContactViewPreferences& aContactTypes) const;
	HBufC8* UnpackageSortOrderAndPluginDetailsLC(const RMessage2& aMessage,RContactViewSortOrder& aSortOrder,TContactViewPreferences& aContactTypes,TUid& aSortPluginImplUid) const;

protected: 
	// From CViewSubSessionBase.
	TInt ServiceL(const RMessage2& aMessage);

private:
	void ConstructL(const RMessage2& aMessage);
	inline CContactLocalView& View() const;
	void ExternalizedSortOrderSizeL(const RMessage2& aMessage) const;
	void GetSortOrderL(const RMessage2& aMessage) const;
	};


/**
The CNamedViewSubSession class implements named view subsessions.
*/
class CNamedViewSubSession : public CViewSubSession
	{
public:
	static CNamedViewSubSession* NewL(CViewManager& aParent,const RMessage2& aMessage);

private:
	// From CViewSubSessionBase.
	TInt ServiceL(const RMessage2& aMessage);

private:
	~CNamedViewSubSession();
	CNamedViewSubSession(CViewManager& aParent);
	void ConstructL(const RMessage2& aMessage);
	void ChangeSortOrderL(const RMessage2& aMessage);
	inline CContactNamedLocalView& View() const;
	};

	
#endif
