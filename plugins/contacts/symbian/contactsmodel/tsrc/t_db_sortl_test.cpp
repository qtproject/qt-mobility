/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Summary:
* A regression test to check changes to the CContactDatabase::SortL() API
* that provide backward compatibility with <v9.2 for some 3rd party apps 
* that misuse the API.
* Full description:
* In cntmodel <v9.2, sorting on the database was done with the now deprecated
* method:
* CContactDatabase::SortL(CArrayFix<TSortPref>* aSortOrder)
* As is implied by the pass-by-pointer semantics, this has always taken
* ownership of aSortOrder. In the past, aSortOrder was held as a data member
* of CContactDatabase and so was held for the lifetime of the database object.
* Some 3rd party apps called SortL(), mistakenly thinking that ownership
* of aSortOrder had not been passed. Consequently, they used aSortOrder after 
* the call. They were able to get away with this because aSortOrder had not 
* been deleted yet at that point.
* In cntmodel >v9.3, the way sort preferences are stored changed and so
* aSortOrder was deleted at the end of SortL() as it was no longer needed. 
* The impact of this change was that aSortOrder was no longer available for
* use after a call to SortL() and so the 3rd party apps panicked.
* The fix for INC119319 provides a work-around that re-introduces the 
* CContactDatabase member variable to hold aSortOrder for the lifetime of the
* database object, as in pre-v9.3 versions. This extended lifetime allows the
* 3rd party apps to continue to use aSortOrder as they had done before. 
* Actions:
* -- Create a new database;
* -- Create and insert 2 contact items into the database;
* -- Call SortL() to sort the contact items;
* -- Test whether aSortOrder can still be used, failing if not;
* -- Clean up the database.
* Associated defect:
* INC119319 Break in CContactDatabase::SortL
* [Not actually a defect but a work-around was provided for compatibility.]
* Written by:
* James Clarke
*
*/


#include <e32test.h>
#include <cntdb.h>
#include <cntitem.h>
#include <cntfield.h>
#include <cntfldst.h>

_LIT(KDbName, "C:T_DB_SortL_test.cdb");
_LIT(KTestName, "T_DB_SortL_test");
RTest test(KTestName);

/**

@SYMTestCaseID     PIM-T-DB-SORTL-TEST-0001

*/

LOCAL_C void DoTestsL()
    {
	test.Start(_L("@SYMTESTCaseID:PIM-T-DB-SORTL-TEST-0001 T_DB_SortL_test"));


	// create default, empty database
	CContactDatabase* db = CContactDatabase::ReplaceL(KDbName);
	CleanupStack::PushL(db);
	
	// create contact 1 and add it to the db
	CContactCard* contact1 = CContactCard::NewLC();
	CContactItemField* fname1 = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldGivenName);
	_LIT(KFname1, "Aaron");
	fname1->TextStorage()->SetTextL(KFname1() );
	contact1->AddFieldL(*fname1);
	CleanupStack::Pop(fname1);
	CContactItemField* lname1 = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldFamilyName);
	_LIT(KLname1, "Zimmerman");
	lname1->TextStorage()->SetTextL(KLname1() );
	contact1->AddFieldL(*lname1);
	CleanupStack::Pop(lname1);
	db->AddNewContactL(*contact1);
	
	// create contact 2 and add it to the db
	CContactCard* contact2 = CContactCard::NewLC();
	CContactItemField* fname2 = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldGivenName);
	_LIT(KFname2, "Zachary");
	fname2->TextStorage()->SetTextL(KFname2() );
	contact2->AddFieldL(*fname2);
	CleanupStack::Pop(fname2);
	CContactItemField* lname2 = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldFamilyName);
	_LIT(KLname2, "Abrahams");
	lname2->TextStorage()->SetTextL(KLname2() );
	contact2->AddFieldL(*lname2);
	CleanupStack::Pop(lname2);
	db->AddNewContactL(*contact2);

	// fetch and print contact IDs. Expected order: 1, 2
	const CContactIdArray* items = db->SortedItemsL(); // doesn't take ownership
	_LIT(KFormattedIdList1, "Contact IDs *before* sorting: %d, %d\n");
	test.Printf(KFormattedIdList1, (*items)[0], (*items)[1]);
	TInt item1BeforeSort = (*items)[0];
	TInt item2BeforeSort = (*items)[1];
	
	// create sort order array
	CArrayFix<CContactDatabase::TSortPref>* sortOrder = new (ELeave) CArrayFixFlat<CContactDatabase::TSortPref>(2);
	CleanupStack::PushL(sortOrder);
	sortOrder->AppendL(CContactDatabase::TSortPref(KUidContactFieldFamilyName) );
	sortOrder->AppendL(CContactDatabase::TSortPref(KUidContactFieldGivenName) );

	// sort db and measure differences in heap allocations as a proxy for 
	// whether sortOrder has been deleted by SortL() or not.
	TInt totalAllocSize(0);
	TInt heapCellsDifference = User::AllocSize(totalAllocSize);
	db->SortL(sortOrder);
	heapCellsDifference -= User::AllocSize(totalAllocSize);
	CleanupStack::Pop(sortOrder);
	
	// fetch and print contact IDs. Expected order: 2, 1
	items = db->SortedItemsL(); // doesn't take ownership
	_LIT(KFormattedIdList2, "Contact IDs *after* sorting:  %d, %d\n");
	test.Printf(KFormattedIdList2, (*items)[0], (*items)[1]);
	TInt item1AfterSort = (*items)[0];
	TInt item2AfterSort = (*items)[1];
	
	//check the sort order to make sure
	test(item1BeforeSort == item2AfterSort);
	test(item2BeforeSort == item1AfterSort);
	
	// check to see if sortOrder is still usable
	if (heapCellsDifference == 0)
		{
		// attempt to re-use sortOrder -- should be allowed after workaround
		CContactDatabase::TSortPref sortPref( (*sortOrder)[0]);
		_LIT(KSortOrderInfo, "The first sort order preference's field type is: %d.\n");
		test.Printf(KSortOrderInfo, sortPref.iFieldType);
		_LIT(KYesSame, "Yes, have successfully re-used sortOrder.\n");
		test.Printf(KYesSame);
		}
	else
		{
		// looks like sortOrder has been deleted so don't try to use it
		_LIT(KNoDifferent, "No, have not reused sortOrder as it has probably been deleted.\n");
		test.Printf(KNoDifferent);
		}
	
	// cleanup
	CleanupStack::PopAndDestroy(3, db);	// and contact1, contact2
	CContactDatabase::DeleteDatabaseL(KDbName);
	
    test.End();
    test.Close();
    }

GLDEF_C TInt E32Main()
	{
    // Init
    __UHEAP_MARK; 
    CTrapCleanup* cleanupStack = CTrapCleanup::New();
    if (!cleanupStack)
        {
        return KErrNoMemory;
        }

    CActiveScheduler* activeScheduler = new CActiveScheduler;
    if (!activeScheduler)
        {
        return KErrNoMemory;
        }
    CActiveScheduler::Install(activeScheduler);

    // Run the tests
    TRAPD(err, DoTestsL());
    if (err)
    	{
    	TRAP_IGNORE(CContactDatabase::DeleteDatabaseL(KDbName) );
    	test(EFalse);
    	}
    
    // Cleanup
    delete activeScheduler;
    delete cleanupStack;
    __UHEAP_MARKEND;
	return err;
    }
