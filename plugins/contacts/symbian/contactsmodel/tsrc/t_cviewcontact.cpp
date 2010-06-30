/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <s32mem.h>
#include <cntdb.h>
#include <cntviewbase.h>
#include "t_utils.h"

_LIT(KTestName,"T_CVIEWCONTACT");

#ifdef SYMIAN_SECURE_DBMS
_LIT(KDatabaseFileName,"C:\\T_CVIEWCONTACT");
#else
_LIT(KDatabaseFileName,"C:\\T_CVIEWCONTACT");
#endif

CCntTest* CntTest=NULL;
LOCAL_D RTest test(KTestName);

const TContactItemId KContactId = 5;
const TContactItemId KModifiedContactId = 6;
_LIT(KContactName,"ContactName");
_LIT(KContactSurname,"ContactSurname");

/** 
 * Test different construction methods.
 */
LOCAL_C void TestConstructionL()
	{
	CViewContact* contact = CViewContact::NewLC(KContactId);

	CViewContact* contact2 = CViewContact::NewL(*contact);
	delete contact2;
	CleanupStack::PopAndDestroy(contact);
	}

/** 
 * Test other CViewContact methods.
 */
LOCAL_C void TestMiscMethodsL()
	{
	CViewContact* contact = CViewContact::NewLC(KContactId);
	test(contact->Id()==KContactId);
	contact->SetId(KModifiedContactId);
	test(contact->Id()==KModifiedContactId);
	contact->SetContactType(CViewContact::EContactItem);
	test(contact->ContactType()==CViewContact::EContactItem);
	contact->SetContactType(CViewContact::EGroup);
	test(contact->ContactType()==CViewContact::EGroup);
	CleanupStack::PopAndDestroy(contact);
	}

/** 
 * Test case 1
 * Add an empty Field when the existing iFieldTextBuf is empty
 * Action : Add an empty Field (i.e. length = 0) when the existing iFieldTextBuf is empty
 * Result : Empty field added
 */
LOCAL_C void AddEmptyFieldL()
	{
	CViewContact* contact = CViewContact::NewLC(KContactId);
	contact->AddFieldL(KNullDesC);
	test(contact->FieldCount()==1);
	test(contact->Field(0)==KNullDesC);
	CleanupStack::PopAndDestroy(contact);
	}

/** 
 * Test case 2
 * Add an empty Field when the existing iFieldTextBuf is not empty 
 * Action : Add an empty Field (i.e. length = 0) when the existing iFieldTextBuf contains some fields already
 * Result : Empty field added
 */
LOCAL_C void AddFieldL()
	{
	CViewContact* contact = CViewContact::NewLC(KContactId);
	contact->AddFieldL(KContactName);
	contact->AddFieldL(KContactSurname);
	contact->AddFieldL(KNullDesC);
	test(contact->FieldCount()==3);
	test(contact->Field(0)==KContactName);
	test(contact->Field(1)==KContactSurname);
	test(contact->Field(2)==KNullDesC);
	CleanupStack::PopAndDestroy(contact);
	}

/** 
 * Test case 3
 * Add an empty Field when OOM 
 * Action : Add an empty Field (i.e. length = 0) when OOM  
 * Result : Empty field added, iFieldTextBuf is unchanged
 */
LOCAL_C void OomAddEmptyFieldL()
	{
	TInt ret=KErrNoMemory;
	TInt failAt=0;	  
	while (ret!=KErrNone)
		{
		failAt++;
		__UHEAP_SETFAIL(RHeap::EDeterministic,failAt);
		__UHEAP_MARK;			
		TRAP(ret, AddEmptyFieldL());
		if (ret!=KErrNone)
			{
			__UHEAP_MARKEND;
			}
		__UHEAP_RESET;
		if (ret!=KErrNoMemory&&ret!=KErrNone)
			{
			test.Printf(_L("Non standard error: %d\n"),ret);
			test.Getch();
			}
		test(ret==KErrNoMemory||ret==KErrNone);
		}
	}

/**

@SYMTestCaseID     PIM-T-CVIEWCONTACT-0001

*/

LOCAL_C void DoTestsL()
    {
	test.Start(_L("@SYMTESTCaseID:PIM-T-CVIEWCONTACT-0001 Tests for CViewContact"));

	TestConstructionL();
	TestMiscMethodsL();
	AddFieldL();
	AddEmptyFieldL();
	OomAddEmptyFieldL();
	}

GLDEF_C TInt E32Main()
	{
    CntTest=new(ELeave) CCntTest;
	CntTest->ConstructL(test,KDatabaseFileName);
    TRAPD(err,DoTestsL());
	CntTest->EndTestLib(err);
	return KErrNone;
    }
