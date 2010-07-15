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


#ifndef ITERATORPROXY_H
#define ITERATORPROXY_H

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
	CViewContact* ItemAtL(const TItemAtLParams& aParams, const CContactTextDef& aDef, TUint aSessionId);
	void ReadContactTextDefL(TContactItemId aContactId, TDes &aResult,const CContactTextDef& aTextDef, TUint aSessionId);
	TBool SeekContactL(TContactItemId aReqId,TContactItemId& aId,TUid& aContactType, TBool& aDeleted);
	TBool SeekIdentContactL(TContactItemId aId);
	void TextFieldL(TInt aCntItemId,TFieldType aFieldType, TDes& aText);

	~CViewIteratorProxyManager();

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
	
	TBool UsesIdentityTableOnly(TInt aFindFlags);
	void ConstructBitwiseFlagsFromTextDef(TInt& aFindFlags,TInt& aIdentityColumnsCount,const CContactTextDef* aTextDef);

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


#endif // ITERATORPROXY_H
