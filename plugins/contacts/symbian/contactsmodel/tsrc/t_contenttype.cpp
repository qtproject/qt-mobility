/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32test.h>
#include <cntfield.h>

_LIT(KTestName, "CContentType Tests");
_LIT(KConpareContentTest, "ContentType compare");

LOCAL_D RTest test(KTestName);

/**

@SYMTestCaseID     PIM-T-CONTENTTYPE-0001

*/

void CompareContentTypeL()
	{
	test.Start(_L("@SYMTESTCaseID:PIM-T-CONTENTTYPE-0001 ContentType compare"));

	CContentType* leftContentType=CContentType::NewL();
	CleanupStack::PushL(leftContentType);
	CContentType* rightContentType=CContentType::NewL();
	CleanupStack::PushL(rightContentType);
	
	leftContentType->SetMapping(KUidContactFieldVCardMapSIPID);
	leftContentType->AddFieldTypeL(KUidContactFieldSIPID);
	leftContentType->AddFieldTypeL(KUidContactFieldVCardMapVOIP);
	
	rightContentType->SetMapping(KUidContactFieldVCardMapSIPID);
	rightContentType->AddFieldTypeL(KUidContactFieldSIPID);
	rightContentType->AddFieldTypeL(KUidContactFieldVCardMapSIPID);
	
	test(!(*leftContentType == *rightContentType));
	test(!(*rightContentType == *leftContentType));
	test(!leftContentType->IsEqual(*rightContentType));
	test(!rightContentType->IsEqual(*leftContentType));
	test(leftContentType->IsEqual(*leftContentType));
	test(rightContentType->IsEqual(*rightContentType));
	
	CleanupStack::PopAndDestroy(2); //leftContentType & rightContentType
	test.End();
	}


void DoTestsL()
    {
    test.Start(_L("CContentType Tests"));

	CompareContentTypeL();	
	test.Close();
    }

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
    CTrapCleanup* cleanupStack = CTrapCleanup::New();
    if (!cleanupStack)
        {
        return KErrNoMemory;
        }

    // Run the tests
    TRAPD(err, DoTestsL());

    // Cleanup
    delete cleanupStack;
	__UHEAP_MARKEND;
	return err;
    }
