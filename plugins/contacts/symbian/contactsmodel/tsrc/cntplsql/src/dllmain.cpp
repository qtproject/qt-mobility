/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

//  EXTERNAL INCLUDES
#include <digia/eunit/ceunittestsuite.h>
//  INTERNAL INCLUDES
#include "t_cpcskeymap.h"
#include "t_cqwertykeymap.h"
#include "t_cpplpredictivesearchtable.h"
#include "t_cqwertypredictivesearchtable.h"
#include "t_cpredictivesearchsettingstable.h"

/**
 * Test suite factory function.
 */
EXPORT_C MEUnitTest* CreateTestSuiteL()
    {
    CEUnitTestSuite* rootSuite = CEUnitTestSuite::NewLC(_L("cntplsql unit tests"));

    rootSuite->AddL(UT_CPcsKeyMap::NewLC());
    CleanupStack::Pop(); // UT_CPcsKeyMap instance

	rootSuite->AddL(UT_CQwertyKeyMap::NewLC());
    CleanupStack::Pop(); // UT_CQwertyKeyMap instance
 
    rootSuite->AddL(UT_CPplPredictiveSearchTable::NewLC());
    CleanupStack::Pop(); // UT_CPplPredictiveSearchTable instance
    
    rootSuite->AddL(UT_CQwertyPredictiveSearchTable::NewLC());
    CleanupStack::Pop(); // UT_CQwertyPredictiveSearchTable instance
    
    rootSuite->AddL(UT_CPredictiveSearchSettingsTable::NewLC());
    CleanupStack::Pop(); // UT_CPredictiveSearchSettingsTable instance

    CleanupStack::Pop(rootSuite);
    return rootSuite;
    }
