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

  
#include "iteratorproxy.h"
#include "rcntmodel.h"
#include <ecom/ecom.h>
#include <cntsync.h>
#include "cntviewprivate.h"


// The name of the test phone book sync plugin.  This plugin will be used in the
// first instance (i.e. if it exists then use it rather than any other plugin).
_LIT(KTestPluginName,"Test phone book synchronizer Implementation");


/**
CContactSynchroniser object factory method.

This class is copied from the Persistence Layer.  It should be refactored into
a shared library if possible.

@return CContactSynchroniser object.
*/
CContactSynchroniser* CContactSynchroniser::NewL()
	{
	RImplInfoPtrArray implInfoArray;
	CleanupResetAndDestroyPushL(implInfoArray);
	REComSession::ListImplementationsL(KUidEcomCntPhBkSyncInterface, implInfoArray);
	
	const TInt count = implInfoArray.Count(); 
	__ASSERT_ALWAYS(count > 0, User::Leave(KErrNotSupported));

	TInt pluginToBeUsed = 0;
	CImplementationInformation *info;

	// Scan the list of plugins for the test plugin.
	for (TInt plugIn = 0 ; plugIn < count ; ++plugIn)
	{
		info = implInfoArray[plugIn];
		if(info->DisplayName() == KTestPluginName)
		{
			// We have found the test plugin so use it rather than any other
			// plugin.
			pluginToBeUsed = plugIn;
			break;
		}
	}

	// Create the implementation found for KUidEcomCntPhBkSyncInterface.
	const TUid theImplementationID = implInfoArray[pluginToBeUsed]->ImplementationUid();
	TAny* ptr = REComSession::CreateImplementationL(theImplementationID,
		_FOFF(CContactSynchroniser,iDtor_ID_Key));

#ifdef __VERBOSE_DEBUG__
	RDebug::Print(_L("[CNTMODEL] CContactSynchroniser::NewL(): Create sync plugin: %S\n"), &info->DisplayName());
#endif

	CleanupStack::PopAndDestroy(&implInfoArray);

	return reinterpret_cast<CContactSynchroniser*>(ptr);
	}


/**
CProxyFactory object factory method.

@param aDb Contacts database used by the proxies this factory provides.

@return CProxyFactory object.
*/
CProxyFactory* CProxyFactory::NewL(const CContactDatabase& aDb)
	{
	CProxyFactory*	self = new (ELeave) CProxyFactory(aDb);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

	
/**
CProxyFactory second phase constructor.
*/
void CProxyFactory::ConstructL()
	{
	}


/**
CProxyFactory first phase constructor.

@param aDb Contacts database used to access Contacts Model session handle.
*/
CProxyFactory::CProxyFactory(const CContactDatabase& aDb)
	:
	iDb(aDb)
	{
	}


/**
CProxyFactory destructor.
*/
CProxyFactory::~CProxyFactory()
	{
	if (iSynchroniser)
		{
		iSynchroniser->Release();
		iSynchroniser = NULL;
		}	
	delete iManager;
	delete iCollection;
	}


/**
Get the MLplViewIteratorManager proxy interface.  If the
CViewIteratorProxyManager proxy object does not yet exist then create it (lazy
initialisation).

@return MLplViewIteratorManager proxy interface instance.
*/
MLplViewIteratorManager& CProxyFactory::GetViewIteratorManagerL()
	{
	if(iManager == NULL)
		{
		iManager = CViewIteratorProxyManager::NewL(iDb);
		}
	return *iManager;
	}


/**
Get the MContactSynchroniser interface.  If the CContactSynchroniser object does
not yet exist then create it (lazy initialisation).

@return MContactSynchroniser interface instance.
*/
MContactSynchroniser& CProxyFactory::GetContactSynchroniserL(TUint)
	{
	if(iSynchroniser == NULL)
		{
		iSynchroniser = CContactSynchroniser::NewL();
		}
	return *iSynchroniser;
	}


/**
Get the MLplCollection proxy interface.  If the CCollectionProxy proxy object
does not yet exist then create it (lazy initialisation).

@return MLplCollection proxy interface instance.
*/
MLplCollection& CProxyFactory::GetCollectorL()
	{
	if(iCollection == NULL)
		{
		iCollection = new (ELeave) CCollectionProxy(iDb);
		}
	return *iCollection;
	}


/**
CViewIteratorProxyManager object factory method.

@param aDb Contacts database used to access Contacts Model session handle.

@return CViewIteratorProxyManager object.
*/
CViewIteratorProxyManager* CViewIteratorProxyManager::NewL(const CContactDatabase& aDb)
	{
	CViewIteratorProxyManager* self = new (ELeave) CViewIteratorProxyManager(aDb);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}


/**
CViewIteratorProxyManager second phase constructor.
*/
void CViewIteratorProxyManager::ConstructL()
	{
	}


/**
CViewIteratorProxyManager first phase constructor.

@param aDb Contacts database used to access Contacts Model session handle.
*/
CViewIteratorProxyManager::CViewIteratorProxyManager(const CContactDatabase& aDb)
	:
	iDb(aDb)
	{
	}


/**
CViewIteratorProxyManager destructor.
*/
CViewIteratorProxyManager::~CViewIteratorProxyManager()
	{
	}


/**
Proxy implementation of pure virtual method in MLplViewIteratorManager.

The proxy does not access the Persistence Layer directly.  Instead it uses the
Contacts Model session handle to access the Persistence Layer via the server.
*/
CViewContact* CViewIteratorProxyManager::ItemAtL(const TItemAtLParams& aParams, const CContactTextDef& aTextDef, TUint /*aSessionId*/)
	{
	return iDb.iCntSvr->ItemAtL(aParams,aTextDef);
	}


/**
Proxy implementation of pure virtual method in MLplViewIteratorManager.

The proxy does not access the Persistence Layer directly.  Instead it uses the
Contacts Model session handle to access the Persistence Layer via the server.
*/
void CViewIteratorProxyManager::ReadContactTextDefL(TContactItemId aContactId, TDes &aResult,const CContactTextDef& aTextDef, TUint /*aSessionId*/)
	{
	iDb.iCntSvr->ReadContactTextDefL(aContactId,aResult,aTextDef);
	}


/**
Proxy implementation of pure virtual method in MLplViewIteratorManager.

The proxy does not access the Persistence Layer directly.  Instead it uses the
Contacts Model session handle to access the Persistence Layer via the server.
*/
TBool CViewIteratorProxyManager::SeekContactL(TContactItemId aReqId,TContactItemId& aId,TUid& aContactType,TBool& aDeleted)
	{
	return iDb.iCntSvr->SeekContactL(aReqId,aId,aContactType,aDeleted);
	}


/**
Proxy implementation of pure virtual method in MLplViewIteratorManager.

The proxy does not access the Persistence Layer directly.  Instead it uses the
Contacts Model session handle to access the Persistence Layer via the server.
*/
TBool CViewIteratorProxyManager::SeekIdentContactL(TContactItemId aId)
	{
	return iDb.iCntSvr->SeekIdentContactL(aId);
	}


/**
Proxy implementation of pure virtual method in MLplViewIteratorManager.

The proxy does not access the Persistence Layer directly.  Instead it uses the
Contacts Model session handle to access the Persistence Layer via the server.
*/
void CViewIteratorProxyManager::TextFieldL(TInt aCntItemId,TFieldType aFieldType, TDes& aText)
	{
	iDb.iCntSvr->TextFieldL(aCntItemId,aFieldType,aText);
	}


/**
CCollectionProxy first phase constructor.

@param aDb Contacts database used to access Contacts Model session handle.
*/
CCollectionProxy::CCollectionProxy(const CContactDatabase& aDb)
	:
	iDb(aDb)
	{
	}


/**
Proxy implementation of pure virtual method in MLplCollection.

The proxy does not access the Persistence Layer directly.  Instead it uses the
Contacts Model session handle to access the Persistence Layer via the server.
*/
CContactIdArray* CCollectionProxy::CollectionL(TLplViewType aViewType,TTime aTime,const TDesC& aGuid)
	{
	return iDb.iCntSvr->CollectionL(aViewType,aTime,aGuid);
	}


/**
Proxy implementation of pure virtual method in MLplCollection.

The proxy does not access the Persistence Layer directly.  Instead it uses the
Contacts Model session handle to access the Persistence Layer via the server.
*/
TInt CCollectionProxy::ContactCountL()
	{
	return iDb.iCntSvr->ContactCountL();
	}


/**
Dummy method - implemented in the Persistence Layer.
*/
TBool CCollectionProxy::ContactMatchesHintFieldL (TInt /*aBitWiseFilter*/, TContactItemId /*aContactId*/)
	{
	return EFalse;		
	}


/**
Dummy method - implemented in the Persistence Layer.
*/
CContactIdArray* CCollectionProxy::MatchPhoneNumberL(const TDesC& /*aNumber*/, const TInt /*aMatchLengthFromRight*/)
	{
	return NULL;	
	}


/**
Proxy implementation of pure virtual method in MLplCollection.

The proxy does not access the Persistence Layer directly.  Instead it uses the
Contacts Model session handle to access the Persistence Layer via the server.

This method could be a dummy implementation since only CContactDatabase calls
the client session FindL() method.
*/
CContactIdArray* CCollectionProxy::FindL(const TDesC& aText, const CContactItemFieldDef* aFieldDef, TUint /*aSessionId*/)
	{
	return iDb.iCntSvr->FindL(aText,aFieldDef);
	}

	
/**
Dummy method - implemented in the Persistence Layer.
*/
CContactIdArray* CCollectionProxy::FilterDatabaseL(CCntFilter& /*aFilter*/)
	{
	return NULL;
	}


/**
Dummy method - implemented in the Persistence Layer.
*/
void CCollectionProxy::Reset()
	{
	}


/**
Dummy method - implemented in the Persistence Layer.
*/
void CCollectionProxy::FindAsyncInitL(const TDesC& /*aText*/,CContactItemFieldDef* /*aFieldDef*/)
	{
	}


/**
Dummy method - implemented in the Persistence Layer.
*/
void CCollectionProxy::FindAsyncTextDefInitL(const CDesCArray& /*aWords*/,CContactTextDef* /*aTextDef*/)
	{
	}


/**
Dummy method - implemented in the Persistence Layer.
*/
CContactIdArray* CCollectionProxy::FindAsyncL(TBool& /*aMoreToGo*/, TUint /*aSessionId*/)
	{
	return NULL;
	}


/**
Dummy method - implemented in the Persistence Layer.
*/
TBool CCollectionProxy::UsesIdentityTableOnly(TInt /*aFindFlags*/)
	{
	return EFalse;		
	}


/**
Dummy method - implemented in the Persistence Layer.
*/
void CCollectionProxy::ConstructBitwiseFlagsFromTextDef(TInt& /*aFindFlags*/, TInt& /*aIdentityColumnsCount*/, const CContactTextDef* /*aTextDef*/)	
	{
	}
