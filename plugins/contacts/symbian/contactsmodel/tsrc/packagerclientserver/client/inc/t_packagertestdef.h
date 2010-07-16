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


#ifndef __PACKAGER_TEST_DEF_H__
#define __PACKAGER_TEST_DEF_H__

#include <e32std.h>

const TUid KServerUid3={0x101F8225};
_LIT(KPackagerServerTestName,"T_PackagerTestServer");
_LIT(KPackagerServerTestExe,"T_PackagerTestServer.exe");

// Contact Item test fields
_LIT(KContactItemGivenName,"AGivenName");
_LIT(KContactItemFamilyName,"AFamilyName");
_LIT(KContactItemFirstPhoneNumber,"mo1 39849342");
_LIT(KContactItemSecondPhoneNumber,"mo2 34530903495803948503984545");
_LIT(KContactItemFirstEmail,"home@email");
_LIT(KContactItemFirstAddress,"Home addresssssss");
_LIT(KContactItemFirstLocality,"Glazgo");
_LIT(KContactItemIMAddress,"wireless@village");
_LIT(KContactItemSecondName,"ASecondName");
_LIT(KContactItemCompanyName,"Symbian Software Ltd.");
_LIT(KContactItemJobTitle,"Software Engineer");
_LIT(KContactItemThirdPhoneNumber,"work Tel 2342");
_LIT(KContactItemSecondEmail,"work@email");
_LIT(KContactItemURL,"brouhaha1");
_LIT(KContactItemSecondAddress,"brouhaha2");
_LIT(KContactItemSecondLocality,"brouhaha3");
_LIT(KContactItemCustom1,"Filterable 1");
_LIT(KContactItemCustom2,"Filterable 2");
_LIT(KContactItemGivenNamePronunciation,"A pronounciation");
_LIT(KContactItemFamilyNamePronunciation,"Another pronounciation");
_LIT(KContactItemFirstSIPID,"SIP POC");
_LIT(KContactItemSecondSIPID,"SIP SWIS");
_LIT(KContactItemThirdSIPID,"SIP VOIP");
#define KUidContactFieldCustom1Value		0x101FD209
#define KUidContactFieldCustom2Value		0x101FD20A
const TUid KUidContactFieldCustom1={KUidContactFieldCustom1Value};
const TUid KUidContactFieldCustom2={KUidContactFieldCustom2Value};

// CContactIdArray test values
const TContactItemId KId1 = 1;
const TContactItemId KId2 = 23;
const TContactItemId KId3 = 456;
const TContactItemId KId4 = 78910;
const TContactItemId KId5 = 3;

// CCntFilter time test value
const TInt KInterval = 20;

enum TCntClientSendCodes
	{
	EProcessCContactItem = 0,
	EProcessCContentType,
	EProcessCContactTextDef,	
	EProcessCContactItemViewDef,
	EProcessCContactViewDef,
	EProcessCContactIdArray,
	EProcessCCntFilter,
	EProcessCViewContact,
	EProcessCDesCArray
	};
	
enum TCntClientReceiveCodes
	{
	EReceiveCContactItem = 9,
	EReceiveCContentType,
	EReceiveCContactTextDef,	
	EReceiveCContactItemViewDef,
	EReceiveCContactViewDef,
	EReceiveCContactIdArray,
	EReceiveCCntFilter,
	EReceiveCViewContact,
	EReceiveCDesCArray
	};	

enum TCntIPCParameters
	{
	EIPCPackagerTransmitBuffer = 0,
	EIPCPackagerReceiveBuffer
	};
	
	
	
	
#endif // __PACKAGER_TEST_DEF_H__
