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
* This profiles the time it takes for a view to become ready and the 
* time it takes to find all contacts matching a set of criteria.
* This also tests to make sure the numbers found match the
* numbers which should be found (this will fail for cntmodel builds
* without the CR PHAR-5U6KGG changes)
*
*/


#include <e32test.h>
#include <cntviewbase.h>
#include <cntitem.h>
#include <cntfldst.h>
#include "t_utils2.h"
#include "t_utils.h"

#include "ccontactvieweventqueue.h"

_LIT(KTestName, "T_ViewSortProfiling");

_LIT(KTestDbName, "c:T_ViewSortProfiling.cdb");

_LIT(KStringToFind, "a");

LOCAL_D RTest test(KTestName);


const TInt KNumberOfTests=7;

const TInt KNumberOfNormalFieldNames=3;
const TInt KNumberOfExtendedFieldNames=6;

const TInt KNormalFieldNames[KNumberOfNormalFieldNames] = 
	{
	KUidContactFieldGivenNameValue, 
	KUidContactFieldFamilyNameValue,
	KUidContactFieldCompanyNameValue, 
	};
const TInt KExtendedFieldNames[KNumberOfExtendedFieldNames] = 
	{
	KUidContactFieldGivenNamePronunciationValue, 
	KUidContactFieldGivenNameValue, 
	KUidContactFieldFamilyNamePronunciationValue,
	KUidContactFieldFamilyNameValue,
	KUidContactFieldCompanyNamePronunciationValue, 
	KUidContactFieldCompanyNameValue, 	
	};

// various globals for testing
TChar GChar;
TInt GNumberOfFieldNames;
TInt GNumberOfFieldsToSort;
const TInt* GFieldNames;
const TInt* GFieldsToSort;
TInt GNumberOfFinds[KNumberOfExtendedFieldNames+1];

class CTestResources : public CBase
    {
public:
    static CTestResources* NewLC(TInt aNumberOfContactsToAddToExistingDatabase, CRandomContactGenerator* aGenerator = NULL, TBool aCreateNewDatabase=ETrue);
    void ConstructL(TInt aNumberOfContactsToAddToExistingDatabase, TBool aCreateNewDatabase);
    ~CTestResources();
	void PrintCardL(const TContactItemId& aId);
	void AddMatchesL(const TContactItemId& aId);
	void CreateContactsL(TInt aNumberOfContacts);
	void ProfileFindsL();
	
    CContactDatabase* iDb;
    CContactViewEventQueue* iViewEventQueue;
    RContactViewSortOrder iViewSortOrder;
    CContactLocalView* iLocalView;
	RArray<TContactItemId> iIdsInViewSortOrder;
	CRandomContactGenerator* iRandomGenerator;
	CContactItemFieldDef *iFindFields;
private:
    	CTestResources(CRandomContactGenerator* aGenerator);
	void  CheckFoundL(TInt i);

    };

/** DOES NOT take ownership of aGenerator*/
CTestResources* CTestResources::NewLC(TInt aNumberOfContactsToAddToExistingDatabase, CRandomContactGenerator* aGenerator, TBool aCreateNewDatabase)
    {
    CTestResources* self = new(ELeave) CTestResources(aGenerator);
    CleanupStack::PushL(self);
    self->ConstructL(aNumberOfContactsToAddToExistingDatabase,aCreateNewDatabase);
    return self;
    }
    
 CTestResources::CTestResources(CRandomContactGenerator* aGenerator) : iRandomGenerator(aGenerator)
 	{}
 
void CTestResources::ConstructL(TInt aNumberOfContactsToAddToExistingDatabase, TBool aCreateNewDatabase)
    {
	if (aCreateNewDatabase)
		iDb = CContactDatabase::ReplaceL(KTestDbName);
	else
		iDb = CContactDatabase::OpenL(KTestDbName);

	if(!iRandomGenerator)
		{
		iRandomGenerator=CRandomContactGenerator::NewL();
		}
	iRandomGenerator->SetDbL(*iDb);
		
	CreateContactsL(aNumberOfContactsToAddToExistingDatabase);

	// done adding everything to the database
	TCntProfile profile[1];

	CCntTest::ProfileReset(0,1);
	CCntTest::ProfileStart(0);
	iDb->CompactL();
	CCntTest::ProfileEnd(0);
	CCntTest::ProfileResult(&profile[0],0,1);
	TInt timeToCompact = profile[0].iTime/1000;
	
	CCntTest::ProfileReset(0,1);
	CCntTest::ProfileStart(0);
	delete iDb;
	iDb=NULL;
	CCntTest::ProfileEnd(0);
	CCntTest::ProfileResult(&profile[0],0,1);
	TInt timeToClose = profile[0].iTime; // in microseconds

	CCntTest::ProfileReset(0,1);
	CCntTest::ProfileStart(0);
	iDb = CContactDatabase::OpenL(KTestDbName);
	CCntTest::ProfileEnd(0);
	CCntTest::ProfileResult(&profile[0],0,1);

	iRandomGenerator->SetDbL(*iDb);
	TInt timeToOpen = profile[0].iTime/1000;
	TInt numberOfContactsInDb=iDb->CountL();
	

	iViewEventQueue = CContactViewEventQueue::NewL(NULL, numberOfContactsInDb*2);
	TInt i;
	for(i = 0; i< GNumberOfFieldsToSort; i++)
		{
		iViewSortOrder.AppendL(TUid::Uid(GFieldsToSort[i]));
		}
    
    iLocalView = CContactLocalView::NewL(*iViewEventQueue, *iDb, iViewSortOrder, EContactsOnly);

    // Wait 100 seconds for the view to get ready
    TContactViewEvent event;

	CCntTest::ProfileReset(0,1);
	CCntTest::ProfileStart(0);
	test(iViewEventQueue->ListenForEvent(100,event));
	CCntTest::ProfileEnd(0);
	CCntTest::ProfileResult(&profile[0],0,1);
	test(event.iEventType == TContactViewEvent::EReady);

	TBuf<128> result;

	result.Format(_L("*%C Contacts: %d\t"),static_cast<TUint>(GChar),numberOfContactsInDb);
	RDebug::Print(result);
	
	result.Format(_L("\tView ready: %d ms\t\tOpen: %d ms\t\tClose: %d us\t\tCompact: %d ms\t"),
		profile[0].iTime/1000, timeToOpen, timeToClose, timeToCompact);
	RDebug::Print(result);
	result.Format(_L("\tSize: %d bytes\n"),iDb->FileSize());
	RDebug::Print(result);
	
	iFindFields = new(ELeave)CContactItemFieldDef;
    }
    
   
void  CTestResources::ProfileFindsL()
	{
	TCntProfile profile[KNumberOfExtendedFieldNames+1];
	TBuf<128> result;
	
	for(TInt i = 0; i<= GNumberOfFieldNames; i++)
		{
		CCntTest::ProfileReset(0,1);
		CCntTest::ProfileStart(0);
		CheckFoundL(i);
		CCntTest::ProfileEnd(0);
		CCntTest::ProfileResult(&profile[i],0,1);
		}

	_LIT(KFindSingle,"@%C N = %d\t Find Single: %d ms\t%d ms\t%d ms\t\tAll: %d \n");
	_LIT(KFindAll,"@%C N = %d\t Find Single: %d ms\t%d ms\t%d ms\t%d ms\t%d ms\t%d ms\t\tAll: %d \n");

	if(GNumberOfFieldNames==KNumberOfNormalFieldNames)
		result.Format(KFindSingle,static_cast<TUint>(GChar),iDb->CountL(),profile[0].iTime/1000, profile[1].iTime/1000, profile[2].iTime/1000, profile[3].iTime/1000);
	else
		result.Format(KFindAll,static_cast<TUint>(GChar),iDb->CountL(), profile[0].iTime/1000, profile[1].iTime/1000, profile[2].iTime/1000, profile[3].iTime/1000, profile[4].iTime/1000, profile[5].iTime/1000, profile[6].iTime/1000);
	RDebug::Print(result);
	}
	
void  CTestResources::CheckFoundL(TInt i)
	{
	if(i<0 || i>GNumberOfFieldNames)
		{
		User::Leave(KErrNotSupported);
		}
	iFindFields->Reset();
	if(i==GNumberOfFieldNames)
		{
		for(TInt k =0; k<i; k++)
			{
			iFindFields->AppendL(TUid::Uid(GFieldNames[k]));
			}
		}
	else
		{
		iFindFields->AppendL(TUid::Uid(GFieldNames[i]));
		}
	CContactIdArray* ids = iDb->FindLC(KStringToFind, iFindFields);
	TInt count = ids->Count();
	test(count == GNumberOfFinds[i]);
	CleanupStack::PopAndDestroy(ids);
	 }

CTestResources::~CTestResources()
	{
	if (iLocalView) iLocalView->Close(*iViewEventQueue);
	iViewSortOrder.Close();
	delete iViewEventQueue;
	delete iDb;
	iIdsInViewSortOrder.Close();
	delete iFindFields;
	}

	
void CTestResources::PrintCardL(const TContactItemId& aId)
	{
	_LIT(KFormatCo,"%S\n");
	_LIT(KFormat1st,"%S ");
	_LIT(KFormatLast,"%S from ");
	_LIT(KFormatPronunciation,"<%S>");
const void* formats[] = {&KFormat1st, &KFormatLast, &KFormatCo};
	CContactCard* card=STATIC_CAST(CContactCard*,iDb->OpenContactL(aId));
	CleanupStack::PushL(card);
	CContactItemFieldSet& fieldSet=card->CardFields();

	
	for(TInt i=0;i<GNumberOfFieldNames;i++)// alternate printing pronunciation then name
		{
		const TInt delta = (i%2) ? 0: GNumberOfFieldNames/2;
		const TInt index = fieldSet.Find( TUid::Uid(GFieldNames[i/2+delta]));
		TPtrC text(fieldSet[index].TextStorage()->Text());
		TPtrC format(KFormatPronunciation);
		if(i%2) format.Set(* ((const TDesC*) formats[i/2]));
		test.Printf(format ,&text);	// print this just to make sure
		}
	CleanupStack::PopAndDestroy(card); 
}


void CTestResources::AddMatchesL(const TContactItemId& aId)
	{
	CContactCard* card=STATIC_CAST(CContactCard*,iDb->OpenContactL(aId));
	CleanupStack::PushL(card);
	
	CContactItemFieldSet& fieldSet=card->CardFields();
	
	TBool found= EFalse;
	for(TInt i=0;i<GNumberOfFieldNames;i++)// alternate printing pronunciation then name
		{
		const TInt index = fieldSet.Find( TUid::Uid(GFieldNames[i]));
		if( fieldSet[index].TextStorage()->Text().FindF(KStringToFind) !=KErrNotFound) 
			{
			GNumberOfFinds[i]++;
			found=ETrue;
			}
		}
	if( found )
		{
		GNumberOfFinds[GNumberOfFieldNames]++;
		}
	CleanupStack::PopAndDestroy(card); 
}

void CTestResources::CreateContactsL(TInt aNumberOfContacts)
	{
	RArray<TInt> names;
	CleanupClosePushL(names);

	for(TInt j =0; j<GNumberOfFieldNames;j++)
		{
		names.AppendL(GFieldNames[j]);
		}
	
	for (TInt i=0; i<aNumberOfContacts; ++i)
		{
		TContactItemId id = 	iRandomGenerator->AddTypicalRandomContactWithNamesL(names);
//		PrintCardL(id);
		AddMatchesL(id);
		}
	CleanupStack::PopAndDestroy( &names );
	}

/**

@SYMTestCaseID     PIM-T-VIEWSORTPROFILING-0001

*/

LOCAL_C void DoTestsL()
    {
	test.Start(_L("@SYMTESTCaseID:PIM-T-VIEWSORTPROFILING-0001 T_ViewSortProfiling"));


	// Delete the database
	TRAPD(err,CContactDatabase::DeleteDatabaseL(KTestDbName));
  	if (err!=KErrNone && err!=KErrNotFound)
		{
		User::Leave(err);
		}
	
	for (TInt k=0;k<3;k++)
		{
		TInt numberOfContactsToAddEachTest=100;
		switch(k)
			{
			case 0: //1st test -- save 3 fields in each contact, sort all of them
				GNumberOfFieldNames=KNumberOfNormalFieldNames;
				GFieldNames=KNormalFieldNames;
				GNumberOfFieldsToSort = KNumberOfNormalFieldNames;
				GFieldsToSort = GFieldNames;
				GChar = 'C';
				break;
			
			case 1: //2nd test -- save 6 fields in each contact, sort 3 of them
				GNumberOfFieldNames=KNumberOfExtendedFieldNames;
				GFieldNames=KExtendedFieldNames;
				GChar = 'D';
				break;
			
			default: // 3rd test -- save 6 fields in each contact, sort all of them	
				GNumberOfFieldsToSort = GNumberOfFieldNames;
				GFieldsToSort = GFieldNames;
				GChar = 'E';
				break;
			}
		TInt i;
		for(i=0;i<=GNumberOfFieldNames; i++)
			{
			GNumberOfFinds[i]=0;
			}
		CRandomContactGenerator* randomGenerator=CRandomContactGenerator::NewL();
		CleanupStack::PushL(randomGenerator);

		for ( i=1; i<=KNumberOfTests; ++i)
			{
			CTestResources* res=NULL;
			if (i==1)
				res = CTestResources::NewLC(numberOfContactsToAddEachTest, randomGenerator);
			else
				res = CTestResources::NewLC(numberOfContactsToAddEachTest, randomGenerator, EFalse);
			res->ProfileFindsL();
			CleanupStack::PopAndDestroy(res);
			}
		CleanupStack::PopAndDestroy(randomGenerator);
		}

    test.End();
    }

GLDEF_C TInt E32Main()
	{
    // Init
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
	
	//Get rid of the warnings.
	//T_utils2 uses efsrv.dll, but we don't use this functions in this test code.
	//So we pretend to use efsrv to keep T_utils2.obj and linker happy
	RFs fs;
	fs.Connect();
	fs.Close();

    // Run the tests
    TRAPD(err, DoTestsL());

    // Cleanup
    TRAP_IGNORE(CContactDatabase::DeleteDatabaseL(KTestDbName));
    delete activeScheduler;
    delete cleanupStack;
	return err;
    }
