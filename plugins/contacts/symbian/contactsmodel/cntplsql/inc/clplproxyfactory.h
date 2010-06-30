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




/**
 @file
 @internalComponent
 @released
*/


#ifndef CLPLPERSISTENCELAYERFACTORY_H
#define CLPLPERSISTENCELAYERFACTORY_H

#include "persistencelayer.h"


/**
The CViewIteratorProxyManager class is a proxy for the MLplViewIteratorManager
class.  The proxy is used for client-side access to the required view functions
in the Persistence Layer on the server.

Internally the proxy makes the relevant IPC calls using the Contacts Model
session handle from the Contacts database used to create the proxy object.
*/
NONSHARABLE_CLASS(CViewIteratorProxyManager) : public CBase , public MLplViewIteratorManager
	{
public:
	static CViewIteratorProxyManager* NewL(const CContactDatabase& aDb);
	~CViewIteratorProxyManager();

	TInt OpenViewL(const CContactTextDef& aTextDef, TContactViewPreferences aViewPrefs);
	void CloseView(TInt aViewId);
	
	void ChangeSortOrderL(TInt aViewId, const CContactTextDef& aTextDef);
	
	//Iterator Methods
	void BeginIterateL(TInt aViewId);
	void EndIterateL(TInt aViewId);
	CViewContact* NextItemL(TInt aViewId, TContactViewPreferences aViewPrefs);
	CViewContact* ItemAtL(TContactItemId aContactId, TInt aViewId);

    TUid ReadContactTextDefL(TContactItemId aContactId, TDes &aResult,const CContactTextDef& aTextDef);
	void TextFieldL(TInt aCntItemId,TFieldType aFieldType, TDes& aText);
    	
private:
	void ConstructL();
	CViewIteratorProxyManager(const CContactDatabase& aDb);

private:
	const CContactDatabase& iDb;
	};


/**
The CCollectionProxy class is a proxy for the MLplCollection class.  The proxy
is used for client-side access to the required collection functions in the
Persistence Layer on the server.

Internally the proxy makes the relevant IPC calls using the Contacts Model
session handle from the Contacts database used to create the proxy object.
*/
NONSHARABLE_CLASS(CCollectionProxy) : public CBase , public MLplCollection
	{
public:
	CCollectionProxy(const CContactDatabase& aDb);

	CContactIdArray* CollectionL(TLplViewType aViewType,TTime aTime = 0,const TDesC& aGuid = KNullDesC);

	TInt ContactCountL();

	void Reset();

	void FindAsyncInitL(const TDesC& aText,CContactItemFieldDef* aFieldDef);
	void FindAsyncTextDefInitL(const CDesCArray& aWords,CContactTextDef* aTextDef);
	CContactIdArray* FindAsyncL(TBool& aMoreToGo, TUint aSessionId);
	CContactIdArray* FindL(const TDesC& aText, const CContactItemFieldDef* aFieldDef, TUint aSessionId);

	TBool ContactMatchesHintFieldL (TInt aBitWiseFilter, TContactItemId aContactId);
	CContactIdArray* MatchPhoneNumberL(const TDesC& aNumber, const TInt aMatchLengthFromRight);

	CContactIdArray* FilterDatabaseL(CCntFilter& aFilter);
	
	TBool UsesIdentityFieldsOnly(TInt aFindFlags);
	void ConstructBitwiseFlagsFromTextDef(TInt& aFindFlags,TInt& aIdentityColumnsCount,const CContactTextDef* aTextDef);
	TBool SeekContactL(TContactItemId aReqId,TContactItemId& aId,TUid& aContactType, TBool& aDeleted);
private:
	const CContactDatabase& iDb;
	};


/**
The CProxyFactory class is a proxy for the MLplPersistenceLayerFactory class.
It provides methods for accessing several interfaces (which at this level are
themselves implemented as proxies).
*/
NONSHARABLE_CLASS(CProxyFactory) : public CBase, public MLplPersistenceLayerFactory
	{
public:
	static CProxyFactory* NewL(const CContactDatabase& aDb);
	~CProxyFactory();

	MLplViewIteratorManager& GetViewIteratorManagerL();
	MContactSynchroniser& GetContactSynchroniserL(TUint aSessionId);
	MLplCollection& GetCollectorL();

private:
	void ConstructL();
	CProxyFactory(const CContactDatabase& aDb);

private:
	const CContactDatabase& iDb;
	CViewIteratorProxyManager* iManager;
	CContactSynchroniser* iSynchroniser;
	CCollectionProxy* iCollection;
	};

#endif // CLPLPERSISTENCELAYERFACTORY_H
