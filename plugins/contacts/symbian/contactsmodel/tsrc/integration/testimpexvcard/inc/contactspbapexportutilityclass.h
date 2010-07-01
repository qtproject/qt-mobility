/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Utility class, which exports the contact items with PBAP version 2.1 or 3.0. Also implements AddIntraContactPropertiesL
* 
*
*/


 
#ifndef __CONTACTSEXPORTUTILITYCLASS_H__
#define __CONTACTSEXPORTUTILITYCLASS_H__

// System includes
#include <e32test.h>
#include <s32file.h>
#include <e32std.h>
#include <f32file.h>
#include <cntdb.h>
#include <vcard.h>
#include <cntitem.h>
#include <cntfldst.h>
#include <cntdef.h>
#include <cntdef.hrh>
#include <cntfield.h>
#include <test/testexecutestepbase.h>

// User includes
#include "cntvcardutils.h"
#include "testimpexvcardsuitedefs.h"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "cntconvertercallback.h"
#endif

class CContactsPBAPExportUtilityClass : public CBase, public MConverterCallBack
	{
public:
	CContactsPBAPExportUtilityClass(CTestStep* atestStep);
	~CContactsPBAPExportUtilityClass();
	void AddIntraContactPropertiesL(const TContactItemId& /*aContactId*/, CArrayPtr<CParserProperty>* aPropertyList);
	void ExportvCardL(CContactDatabase* adBase, const TDesC& aStandard, CContactIdArray* aIdArray, RWriteStream& aWriteStream, TInt64 aContactFilter);
	void TokenizeStringL(const TDesC& aString, RArray<TPtrC>& aList, TChar aSeparator = ',');
		
private:
	CParserPropertyValue* CreateCDesCArrayL();
	CParserPropertyValueDate* CreateDatePropertyL(const TTime& aDateTime);
	void AddPropertyL(const TDesC& aPropertyName, CParserPropertyValue* aPropertyValue, CParserParam* aPropParameter, CArrayPtr<CParserProperty>* aPropertyList);
	CParserParam* CreateParameterL(const TDesC& aParamName);	
	CTestStep& BaseTestStepReference();
	
private:	
	CTestStep* iBaseTestStep;
		
	};

_LIT(KAddr1,								"1st Floor");
_LIT(KAddr2,								"Some Park");
_LIT(KAddr3,								"Tower No.1");					
_LIT(KEmailVal,								"dtest@symbianfoundation.com");
_LIT8(KPhotoVal,							"PHOTO");

_LIT(KDATETIME,								"DateTime");
_LIT(KDateTimeVal,							"20060921:");

_LIT(KIntraPropName,						"IntraPropName");
_LIT(KIntraPropDatatype,					"IntraPropDatatype");
_LIT(KCDesCArray,							"CDesCArray");
_LIT(KHBufC,								"HBufC");
_LIT(KBinary,								"Binary");
_LIT(KInt,									"Int");

_LIT(KLeave,								"Leave");
_LIT(KTelProp,								"TelProp");

_LIT(KVersion30,							"3.0");
_LIT(KVersion21,							"2.1");
_LIT(KVersionStd21,							"std21");
_LIT(KEmpid,								"EMPID");
_LIT(KIntraPropParName,						"IntraPropParName");

#endif
