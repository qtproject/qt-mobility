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

#include "plplugins.h"
#include "cntviewprivate.h"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <cntphonenumparser.h>
#include "cntdb_internal.h"
#include "cntsyncecom.h"
#endif


// The name of the test phone book sync plugin.  This plugin will be used in the
// first instance (i.e. if it exists then use it rather than any other plugin).
_LIT(KTestPluginName,"Test phone book synchronizer Implementation");


CContactPlugin::CContactPlugin()
	{
	}


CContactPlugin::~CContactPlugin()
	{
	iDll.Close();
	}


CContactPhoneParser* CContactPhoneParser::NewL()
	{
	CContactPhoneParser* self=new(ELeave) CContactPhoneParser;
	CleanupStack::PushL(self);
	self->LoadEComPluginL();
	CleanupStack::Pop(self); 
	return self;
	}


CContactPhoneParser::~CContactPhoneParser()
	{
	delete iParser;
	}


void CContactPhoneParser::SetPluginL(TLibraryFunction aGateFunction)
	{
	iParser=REINTERPRET_CAST(CContactPhoneNumberParser*,(*aGateFunction)());
	User::LeaveIfNull(iParser);
	}


void CContactPhoneParser::LoadEComPluginL()
	{
#ifdef __PROFILE__
	RDebug::Print(_L("[CNTMODEL] Entering CContactPhoneParser::LoadEComPluginL\n"));
	TTime now;
	now.UniversalTime();
#endif
	
	RImplInfoPtrArray	implInfoArray;
	CleanupResetAndDestroyPushL(implInfoArray);

	// Use default resolver parameters.
	TEComResolverParams resolverParams;
	
	// List implementations, searching only in ROM for the plugin.
	REComSession::ListImplementationsL(KUidEcomCntPhoneNumberParserInterface,
		resolverParams,	KRomOnlyResolverUid, implInfoArray);

	const TInt count = implInfoArray.Count();
	__ASSERT_ALWAYS( count > 0, User::Leave(KErrNotFound));

	// Create the first implementation found for
	// KUidEcomCntPhoneNumberParserInterface.
	const TUid firstImplementationFound = implInfoArray[0]->ImplementationUid();
	iParser = CContactEcomPhoneNumberParser::NewL(firstImplementationFound);
	CleanupStack::PopAndDestroy(&implInfoArray);

#ifdef __PROFILE__
	TTime now2;
	now2.UniversalTime();
	RDebug::Print(_L("[CNTMODEL] Spend %d microseconds in CContactPhoneParser::LoadEComPluginL\n"), now2.MicroSecondsFrom(now).Int64());
#endif
	}


EXPORT_C CContactSynchroniser::~CContactSynchroniser()
	{
	REComSession::DestroyedImplementation(iDtor_ID_Key);
	}


CContactSynchroniser* CContactSynchroniser::NewL()
	{
#ifdef __PROFILE__
	RDebug::Print(_L("[CNTMODEL] Entering CContactSynchroniser::NewL\n"));
	TTime now;
	now.UniversalTime();
#endif

	RImplInfoPtrArray implInfoArray;
	CleanupResetAndDestroyPushL(implInfoArray);
	REComSession::ListImplementationsL(KUidEcomCntPhBkSyncInterface, implInfoArray);
	// Load the implementation found for KUidEcomCntPhBkSyncInterface, prefer the test plugin
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
#ifdef __VERBOSE_DEBUG__
	RDebug::Print(_L("[CNTMODEL] CntServer has loaded syncplugin: %S\n"), &info->DisplayName());
#endif

	// Create the implementation found for KUidEcomCntPhBkSyncInterface.
	const TUid theImplementationID = implInfoArray[pluginToBeUsed]->ImplementationUid();
	TAny* ptr = REComSession::CreateImplementationL(theImplementationID,
		_FOFF(CContactSynchroniser,iDtor_ID_Key));

	CleanupStack::PopAndDestroy(&implInfoArray);

#ifdef __PROFILE__
	TTime now2;
	now2.UniversalTime();
	RDebug::Print(_L("[CNTMODEL] Spend %d microseconds in CContactSynchroniser::NewL()\n"), now2.MicroSecondsFrom(now).Int64());
#endif

	return reinterpret_cast<CContactSynchroniser*>(ptr);
	}


/**
Reserved (private) virtual function to allow further extension of the
MContactSynchroniser interface without requiring changes to all derived classes.
This can be used to return a pointer to an additional extension class.
*/
EXPORT_C void MContactSynchroniser::MContactSynchroniser_Reserved_1()
	{
	}
