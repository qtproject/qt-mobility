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




#if (!defined __TESTIMPEXVCARD_SERVER_H__)
#define __TESTIMPEXVCARD_SERVER_H__
#include <testexecuteserverbase.h>

// Literal constants for INI file field-names
_LIT(KIniFieldName, 			"fieldname");
_LIT(KIniFieldVal,				"fieldval");
_LIT(KIniFlagvalue,				"flag");
_LIT(KIniYear,					"year");
_LIT(KIniMonth,					"month");
_LIT(KIniDay,					"day");
_LIT(KFlagExport,				"flagexport");
_LIT(KExpectedData,				"expecteddata");

// Field Name
_LIT(KAssistance, 		"Assistant");
_LIT(KAssistanceTel, 	"Assistant-Tel");
_LIT(KAnniversary, 		"Anniversary");
_LIT(KSpouse, 			"Spouse");
_LIT(KChildren, 		"Children");
_LIT(KClass, 			"Class");
_LIT(KDepartment, 		"Department");
_LIT(KCompanyName, 		"CompanyName");

// all fields value
_LIT(KIniFieldAsstVal,					"asstfieldval");
_LIT(KIniFieldAsstTelVal,				"assttelfieldval");
_LIT(KIniFieldSpouseVal,				"spousefieldval");
_LIT(KIniFieldChildVel,					"childrenfieldval");
_LIT(KIniFieldClassVal,					"classfieldval");
_LIT(KIniFieldDeptVal,					"deptfieldval");
_LIT(KIniFieldCompanyVal,				"companyfieldval");


class CTestImpExvCardSuite : public CTestServer
	{
public:
	static CTestImpExvCardSuite* NewL();
	// Base class pure virtual override
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	
private:
	};

#endif
