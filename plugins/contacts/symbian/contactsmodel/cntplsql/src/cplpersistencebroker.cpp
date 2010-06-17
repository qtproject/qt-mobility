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


/**
 @file
 @internalComponent
 @released
*/

#include "persistencelayerimpl.h"
#include <cntfldst.h> // for ccontactfieldstorage.
#include "clplcontactproperties.h"
#include "clplanalyserproxy.h"
#include "cntpplviewmanager.h"

/**
CLplTester constructor.
*/
CLplTester::CLplTester(CLplContactProperties& aProps, CPplContactsFile& aFile) 
	:
	iProps(aProps),
	iFile(aFile)
	{
	}

/**
Merging pure (minimal) contact with the template should be out of scope of the
Persistence Layer.  The only reason to have this functions in the test interface
is that CContactItem::RestoreTemplateFieldsL() is not exported.
*/
void CLplTester::MergeContactWithTemplateL(CContactItem& aContact, const CContactItem& aTempl, const CContactItemViewDef& aView) const
	{	
	if (aContact.TemplateRefId()!=KNullContactId &&  aContact.Type() != KUidContactCardTemplate)
		{
		aContact.RestoreTemplateFieldsL(iProps.SystemTemplateL().CardFields(), aTempl.CardFields(), aView);
		}
	}

/**
CPersistenceLayer object factory.
*/
EXPORT_C CPersistenceLayer* CPersistenceLayer::NewLC(RFs& aFs, MIniFileManager* aIniFileManager, MContactDbObserverV2* aObserver) 
 	{
	CPersistenceLayer* self = new (ELeave) CPersistenceLayer(aFs, aIniFileManager, aObserver);
	CleanupStack::PushL(self);
	return self;
	}


/**
CPersistenceLayer first phase constructor.
*/
CPersistenceLayer::CPersistenceLayer(RFs& aFs, MIniFileManager* aIniFileManager, MContactDbObserverV2* aObserver)
	:
	iFs(aFs), 
	iCntDbObserver(aObserver), 
	iIniManager(aIniFileManager)				   			   
	{
	}



/**
CPersistenceLayer destructor.
*/
CPersistenceLayer::~CPersistenceLayer()
	{
	if (iFactory)
		{
		delete iFactory;		
		}
	if (iContactProperties)
		{
		delete iContactProperties;		
		}
	if (iAnalyserProxy)
		{
		delete iAnalyserProxy;		
		}
	if (iTester)	
		{
		delete iTester;		
		}
	
	// May not be a file opened by Persistence Layer.
	if (iContactsFile)
		{
		// Don't call iContactsFile.Close() since the destructor does it.
		delete iContactsFile;
		}
	}


/**
Set the database observer.
*/
EXPORT_C void CPersistenceLayer::RegisterDbObserver(MContactDbObserverV2& aDbObserver)
	{
	// Note the callback interface supplied.
	iCntDbObserver = &aDbObserver;
	
	// Contacts file should always exist.
	iContactsFile->RegisterDbObserver(aDbObserver);

	// If the ini manager exists and the analyser proxy does not yet exist then
	// create the proxy.
	if (iIniManager && !iAnalyserProxy)
		{
		// This used to call new (ELeave). We don't check for NULL afterwards as we 
		// can't do anything about it. If an allocation failure occurs and the users 
		// of iAnalyserProxy do not check for NULL, cntsrv.exe will now panic.
		iAnalyserProxy = new CLplAnalyserProxy(*iIniManager,
			*iCntDbObserver, iContactsFile->PersistenceBroker(), 
			iContactsFile->TransactionManager(), *iContactProperties);
		}
	}


EXPORT_C MLplContactsFile& CPersistenceLayer::ContactsFileL()
	{
	if (!iContactsFile)
		{
		iContactProperties = new (ELeave) CLplContactProperties(iFs);
		CleanupStack::PushL(TCleanupItem(CleanupOperation, this));	
		iContactsFile = CPplContactsFile::NewL(*iContactProperties, iCntDbObserver);
		CleanupStack::PushL(TCleanupItem(CleanupOperation, this));	
		
		if (iIniManager && iCntDbObserver) // Callback interfaces supplied?
			{
			iAnalyserProxy = new (ELeave) CLplAnalyserProxy(*iIniManager,
				*iCntDbObserver, iContactsFile->PersistenceBroker(), 
				iContactsFile->TransactionManager(), *iContactProperties);
			}
		CleanupStack::Pop(2);
		}
	return *iContactsFile;
	}

void CPersistenceLayer::CleanupOperation(TAny* aObject)
	{
	CPersistenceLayer* persitenceLayer = static_cast<CPersistenceLayer*>(aObject);
	if (persitenceLayer->iContactProperties)
		{
		delete persitenceLayer->iContactProperties;		
		persitenceLayer->iContactProperties = NULL;
		}
	if (persitenceLayer->iContactsFile)
		{
		delete persitenceLayer->iContactsFile;
		persitenceLayer->iContactsFile = NULL;
		}
	}

EXPORT_C MLplPersistenceBroker& CPersistenceLayer::PersistenceBroker()
	{
	// Use analyser proxy if it exists.
	if (iAnalyserProxy)
		{
		return *iAnalyserProxy;
		}
	else
		{
		return iContactsFile->PersistenceBroker();
		}
	}


EXPORT_C MLplTransactionManager& CPersistenceLayer::TransactionManager()
	{
	// In order to use the Persistence Broker you must open the database first.
	__ASSERT_ALWAYS(iContactsFile && iContactsFile->IsOpened(), User::Leave(KErrNotReady));

	// Use analyser proxy if it exists.
	if (iAnalyserProxy) 
		{
		return *iAnalyserProxy;
		}
	else
		{
		return iContactsFile->TransactionManager();
		}	
	}


EXPORT_C MLplContactProperties& CPersistenceLayer::ContactProperties()
	{
	// In order to use Contact Properties you must open the database first.
	__ASSERT_ALWAYS(iContactProperties, User::Leave(KErrNotReady));

	// If contacts file is created iContactProperties is also created.
	return *iContactProperties;
	}


EXPORT_C MLplPersistenceLayerFactory& CPersistenceLayer::FactoryL()
	{
	if(iFactory == NULL)
		{
		iFactory = new (ELeave) CLplPersistenceLayerFactory(
			*iContactsFile,*iContactProperties);
		}
	return *iFactory;
	}


CLplPersistenceLayerFactory::~CLplPersistenceLayerFactory()
	{
	delete iCollection;
	delete iManager;
	}


CLplPersistenceLayerFactory::CLplPersistenceLayerFactory(CPplContactsFile& aContactsFile,CLplContactProperties& aContactProperties) :
	iContactsFile(aContactsFile),
	iContactProperties(aContactProperties)
	{
	}


MLplViewIteratorManager& CLplPersistenceLayerFactory::GetViewIteratorManagerL()
	{
	if(iManager == NULL)
		{
		iManager = CCntPplViewManager::NewL(iContactsFile, iContactProperties);
		}
	return *iManager;
	}

	
MContactSynchroniser& CLplPersistenceLayerFactory::GetContactSynchroniserL(TUint aSessionId)
	{
	return iContactProperties.ContactSynchroniserL(aSessionId);
	}


MLplCollection& CLplPersistenceLayerFactory::GetCollectorL()
	{
	if(iCollection == NULL)
		{
		iCollection = CPlCollection::NewL(iContactsFile);
		}
	return *iCollection;
	}


EXPORT_C MLplPersistenceLayerTest& CPersistenceLayer::PersistenceLayerTestL()
	{
	if (!iTester)
		{ 
		iTester = new (ELeave) CLplTester(*iContactProperties, *iContactsFile);
		}
	return *iTester;
	}
