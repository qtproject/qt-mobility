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
#include <f32file.h>
#include <s32file.h>
#include <cntdb.h>
#include <cntitem.h>
#include <cntfield.h>
#include <cntfldst.h>
#include <cntviewbase.h>
#include "t_utils.h"
#include <coreappstest/testserver.h>

CCntTest* CntTest=NULL;

LOCAL_D RTest test(_L("T_VIEW"));
LOCAL_D CContactDatabase* TheDb;
LOCAL_D TContactItemId TheId;

const TPtrC KDatabaseFileName=_L("C:T_VIEW");

const TPtrC KDateFormatText1=_L("%1/%2/%3");
const TPtrC KDateFormatText2=_L("AbcdefghijklmNopqrstuvwxyz%1/%2/%3AbcdefghijklmNopqrstuvwxyz1234567890");	// Should be long enough to overflow a TTextFieldMinimal buf

LOCAL_C void CreateView1L()
	{
	CContactItemViewDef* viewDef=CContactItemViewDef::NewLC(CContactItemViewDef::EIncludeFields,CContactItemViewDef::EMaskHiddenFields);
	viewDef->AddL(KUidContactFieldGivenName);
	viewDef->AddL(KUidContactFieldFamilyName);
	viewDef->AddL(KUidContactFieldPhoneNumber);
	viewDef->AddL(KUidContactFieldCompanyName);
	viewDef->AddL(KUidContactFieldAddress);
	test(viewDef->Count()==5);
	test((*viewDef)[0]==KUidContactFieldGivenName);
	test((*viewDef)[1]==KUidContactFieldFamilyName);
	test((*viewDef)[2]==KUidContactFieldPhoneNumber);
	test((*viewDef)[3]==KUidContactFieldCompanyName);
	test((*viewDef)[4]==KUidContactFieldAddress);
	TInt index=viewDef->Find(KUidContactFieldCompanyName);
	test(index!=KErrNotFound);
	viewDef->Remove(index);
	test(viewDef->Find(KUidContactFieldCompanyName)==KErrNotFound);
	test(viewDef->Count()==4);
	viewDef->Remove(KUidContactFieldPhoneNumber);
	test(viewDef->Find(KUidContactFieldPhoneNumber)==KErrNotFound);
	test(viewDef->Count()==3);
	viewDef->Reset();
	test(viewDef->Count()==0);
	CleanupStack::PopAndDestroy(); // viewDef
	}

LOCAL_C void CreateView2L()
	{
	CContactItemViewDef* viewDef=CContactItemViewDef::NewL(CContactItemViewDef::EIncludeFields,CContactItemViewDef::EMaskHiddenFields);
	CleanupStack::PushL(viewDef);
	viewDef->AddL(KUidContactFieldPhoneNumber);
 	viewDef->AddL(KUidContactFieldPrefixName);
 	viewDef->AddL(KUidContactFieldCompanyName);
 	viewDef->AddL(KUidContactFieldGivenName);
 	test(viewDef->Count()==4);
 	test(viewDef->Find(KUidContactFieldPhoneNumber)!=KErrNotFound);
	test(viewDef->Find(KUidContactFieldPhoneNumber)!=KErrNotFound);
 	test(viewDef->Find(KUidContactFieldMatchAll)!=KErrNotFound);
 	test(viewDef->Find(KUidContactFieldSuffixName)==KErrNotFound);
 	test(viewDef->Find(KUidContactFieldPrefixName)!=KErrNotFound);
 	test(viewDef->Find(KUidContactFieldCompanyName)!=KErrNotFound);
	test(viewDef->Use()==CContactItemViewDef::EIncludeFields);
	viewDef->SetUse(CContactItemViewDef::EMaskFields);
	test(viewDef->Use()==CContactItemViewDef::EMaskFields);
	CleanupStack::PopAndDestroy(); // viewDef
	}

LOCAL_C void PopulateDatabaseL()
	{
	CContactItem* item=CContactCard::NewLC();
	CContactItemField* field=CContactItemField::NewLC(KStorageTypeText,KUidContactFieldCompanyName);
	field->SetMapping(KUidContactFieldVCardMapORG);
	field->TextStorage()->SetTextL(_L("x"));
	item->AddFieldL(*field);
	CleanupStack::Pop(); // field
	field=CContactItemField::NewLC(KStorageTypeText,KUidContactFieldPhoneNumber);
	field->SetMapping(KUidContactFieldVCardMapTEL);
	field->TextStorage()->SetTextL(_L("x"));
	item->AddFieldL(*field);
	CleanupStack::Pop(); // field
	field=CContactItemField::NewLC(KStorageTypeText,KUidContactFieldPhoneNumber);
 	field->SetMapping(KUidContactFieldVCardMapTEL);
	field->TextStorage()->SetTextL(_L("x"));
	item->AddFieldL(*field);
	CleanupStack::Pop(); // field
	field=CContactItemField::NewLC(KStorageTypeText,KUidContactFieldPhoneNumber);
 	field->SetMapping(KUidContactFieldVCardMapTEL);
	field->TextStorage()->SetTextL(_L("x"));
	item->AddFieldL(*field);
	CleanupStack::Pop(); // field
	field=CContactItemField::NewLC(KStorageTypeText,KUidContactFieldPhoneNumber);
	field->SetMapping(KUidContactFieldVCardMapTEL);
 	field->SetHidden(ETrue);
	field->TextStorage()->SetTextL(_L("x"));
	item->AddFieldL(*field);
	CleanupStack::Pop(); // field
	field=CContactItemField::NewLC(KStorageTypeText,KUidContactFieldSuffixName);
	field->SetMapping(KUidContactFieldVCardMapUnusedN);
	field->TextStorage()->SetTextL(_L("x"));
 	item->AddFieldL(*field);
	CleanupStack::Pop(); // field
	field=CContactItemField::NewLC(KStorageTypeText,KUidContactFieldPrefixName);
	field->SetMapping(KUidContactFieldVCardMapUnusedN);
	field->TextStorage()->SetTextL(_L("x"));
 	item->AddFieldL(*field);
	CleanupStack::Pop(); // field

	TheId=TheDb->AddNewContactL(*item);
	CleanupStack::PopAndDestroy(); // item
	}

LOCAL_C void SetViewsL()
	{
	CContactItem* item=TheDb->ReadContactLC(TheId);
	test(item->CardFields().Count()==6);
	CleanupStack::PopAndDestroy(); // item
	CContactItemViewDef* viewDef=CContactItemViewDef::NewLC(CContactItemViewDef::EIncludeFields,CContactItemViewDef::EMaskHiddenFields);
	viewDef->AddL(KUidContactFieldPhoneNumber);
	viewDef->AddL(KUidContactFieldCompanyName);
	CContactViewDef* def=CContactViewDef::NewLC(viewDef);
	CleanupStack::Pop(); // viewDef

    // Try setting to null. Should do nothing but shouldn't crash.
	TheDb->SetViewDefinitionL( NULL );

	TheDb->SetViewDefinitionL(def);
	CleanupStack::Pop(); // def
	item=TheDb->ReadContactLC(TheId);
	test(item->CardFields().Count()==4);
	CleanupStack::PopAndDestroy(); // item

	viewDef=CContactItemViewDef::NewLC(CContactItemViewDef::EIncludeFields,CContactItemViewDef::EIncludeHiddenFields);
	viewDef->AddL(KUidContactFieldPhoneNumber);
	viewDef->AddL(KUidContactFieldCompanyName);
	def=CContactViewDef::NewLC(viewDef);
	CleanupStack::Pop(); // viewDef
	TheDb->SetViewDefinitionL(def);
	CleanupStack::Pop(); // def
	item=TheDb->ReadContactLC(TheId);
	CleanupStack::PopAndDestroy(); // item
	
	viewDef=CContactItemViewDef::NewLC(CContactItemViewDef::EIncludeFields,CContactItemViewDef::EIncludeHiddenFields);
	viewDef->AddL(KUidContactFieldPhoneNumber);
	def=CContactViewDef::NewLC(viewDef);
	CleanupStack::Pop(); // viewDef
	TheDb->SetViewDefinitionL(def);
	CleanupStack::Pop(); // def
	item=TheDb->ReadContactLC(TheId);
	test(item->CardFields().Count()==4);
	CleanupStack::PopAndDestroy(); // item

	viewDef=CContactItemViewDef::NewLC(CContactItemViewDef::EIncludeFields,CContactItemViewDef::EMaskHiddenFields);
  	viewDef->AddL(KUidContactFieldPhoneNumber);
  	viewDef->AddL(KUidContactFieldCompanyName);
	def=CContactViewDef::NewLC(viewDef);
	CleanupStack::Pop(); // viewDef
	TheDb->SetViewDefinitionL(def);
	CleanupStack::Pop(); // def
	item=TheDb->ReadContactLC(TheId);
  	test(item->CardFields().Count()==4);
	CleanupStack::PopAndDestroy(); // item

	viewDef=CContactItemViewDef::NewLC(CContactItemViewDef::EIncludeFields,CContactItemViewDef::EMaskHiddenFields);
	viewDef->AddL(KUidContactFieldFamilyName);
	def=CContactViewDef::NewLC(viewDef);
	CleanupStack::Pop(); // viewDef
	TheDb->SetViewDefinitionL(def);
	CleanupStack::Pop(); // def
	item=TheDb->ReadContactLC(TheId);
	test(item->CardFields().Count()==0);
	CleanupStack::PopAndDestroy(); // item
	}

LOCAL_C void SetMaskedViewsL()
	{
	CContactItemViewDef* viewDef=CContactItemViewDef::NewLC(CContactItemViewDef::EMaskFields,CContactItemViewDef::EMaskHiddenFields);
	viewDef->AddL(KUidContactFieldPhoneNumber);
	viewDef->AddL(KUidContactFieldCompanyName);
	CContactViewDef* def=CContactViewDef::NewLC(viewDef);
	CleanupStack::Pop(); // viewDef
	TheDb->SetViewDefinitionL(def);
	CleanupStack::Pop(); // def
	CContactItem* item=TheDb->ReadContactLC(TheId);
  	test(item->CardFields().Count()==2);
	CleanupStack::PopAndDestroy(); // item

	viewDef=CContactItemViewDef::NewLC(CContactItemViewDef::EMaskFields,CContactItemViewDef::EMaskHiddenFields);
	viewDef->AddL(KUidContactFieldPhoneNumber);
	viewDef->AddL(KUidContactFieldCompanyName);
	def=CContactViewDef::NewLC(viewDef);
	CleanupStack::Pop(); // viewDef
	TheDb->SetViewDefinitionL(def);
	CleanupStack::Pop(); // def
	item=TheDb->ReadContactLC(TheId);
  	test(item->CardFields().Count()==2);
	CleanupStack::PopAndDestroy(); // item

	viewDef=CContactItemViewDef::NewLC(CContactItemViewDef::EMaskFields,CContactItemViewDef::EMaskHiddenFields);
	viewDef->AddL(KUidContactFieldPhoneNumber);
	viewDef->AddL(KUidContactFieldCompanyName);
	def=CContactViewDef::NewLC(viewDef);
	CleanupStack::Pop(); // viewDef
	TheDb->SetViewDefinitionL(def);
	CleanupStack::Pop(); // def
	item=TheDb->ReadContactLC(TheId);
  	test(item->CardFields().Count()==2);
	CleanupStack::PopAndDestroy(); // item

	viewDef=CContactItemViewDef::NewLC(CContactItemViewDef::EMaskFields,CContactItemViewDef::EMaskHiddenFields);
	viewDef->AddL(KUidContactFieldNone);
	def=CContactViewDef::NewLC(viewDef);
	CleanupStack::Pop(); // viewDef
	TheDb->SetViewDefinitionL(def);
	CleanupStack::Pop(); // def
	item=TheDb->ReadContactLC(TheId);
  	test(item->CardFields().Count()==6);
	CleanupStack::PopAndDestroy(); // item

	viewDef=CContactItemViewDef::NewLC(CContactItemViewDef::EMaskFields,CContactItemViewDef::EMaskHiddenFields);
	viewDef->AddL(KUidContactFieldNone);
	def=CContactViewDef::NewLC(viewDef);
	CleanupStack::Pop(); // viewDef
	TheDb->SetViewDefinitionL(def);
	CleanupStack::Pop(); // def
	item=TheDb->ReadContactLC(TheId);
	test(item->CardFields().Count()==6);
	CleanupStack::PopAndDestroy(); // item

	viewDef=CContactItemViewDef::NewLC(CContactItemViewDef::EMaskFields,CContactItemViewDef::EMaskHiddenFields);
	viewDef->AddL(KUidContactFieldNone);
	def=CContactViewDef::NewLC(viewDef);
	CleanupStack::Pop(); // viewDef
	TheDb->SetViewDefinitionL(def);
	CleanupStack::Pop(); // def
	item=TheDb->ReadContactLC(TheId);
  	test(item->CardFields().Count()==6);
	CleanupStack::PopAndDestroy(); // item

	viewDef=CContactItemViewDef::NewLC(CContactItemViewDef::EMaskFields,CContactItemViewDef::EMaskHiddenFields);
	viewDef->AddL(KUidContactFieldMatchAll);
	def=CContactViewDef::NewLC(viewDef);
	CleanupStack::Pop(); // viewDef
	TheDb->SetViewDefinitionL(def);
	CleanupStack::Pop(); // def
	item=TheDb->ReadContactLC(TheId);
	test(item->CardFields().Count()==0);
	CleanupStack::PopAndDestroy(); // item
	}

LOCAL_C void SetNameL(CContactItem& aItem,TUid aFieldType,const TDesC& aName)
	{
	CContactItemField* field=CContactItemField::NewLC(KStorageTypeText,aFieldType);
	field->SetMapping(KUidContactFieldVCardMapUnusedN);
	field->TextStorage()->SetTextL(aName);
	aItem.AddFieldL(*field);
	CleanupStack::Pop(); // field
	}

LOCAL_C void SetTimeDateL(CContactItem& aItem,TUid aFieldType,TTime aTime)
	{
	CContactItemField* field=CContactItemField::NewLC(KStorageTypeDateTime,aFieldType);
	field->SetMapping(KUidContactFieldVCardMapUnusedN);
	field->DateTimeStorage()->SetTime(aTime);
	aItem.AddFieldL(*field);
	CleanupStack::Pop(); // field
	}

LOCAL_C void doTestTextDefinitionL(TBool aPreLoadTest, TContactItemId aTextDefItem, TContactTextDefItem aItem1, TContactTextDefItem aItem2, TContactTextDefItem aItem3, TContactTextDefItem aItem4, TFieldType aFieldType, const TDesC &aTestText)
	{
	CContactItem *item=NULL;
	if (aPreLoadTest)
		item=TheDb->ReadContactLC(aTextDefItem);
	CContactTextDef *textDef=CContactTextDef::NewLC();
	if (aItem1.iFieldType!=KNullUid)
		textDef->AppendL(aItem1);
	if (aItem2.iFieldType!=KNullUid)
		textDef->AppendL(aItem2);
	if (aItem3.iFieldType!=KNullUid)
		textDef->AppendL(aItem3);
	if (aItem4.iFieldType!=KNullUid)
		textDef->AppendL(aItem4);
	if (aFieldType!=KUidContactFieldNone)
		textDef->SetFallbackField(aFieldType);
	if (aItem1.iFieldType!=KNullUid)
		{
		test((*textDef)[0].iFieldType==aItem1.iFieldType);
		test((*textDef)[0].iSeperator==aItem1.iSeperator);
		}
	if (aItem2.iFieldType!=KNullUid)
		{
		test((*textDef)[1].iFieldType==aItem2.iFieldType);
		test((*textDef)[1].iSeperator==aItem2.iSeperator);
		}
	if (aItem3.iFieldType!=KNullUid)
		{
		test((*textDef)[2].iFieldType==aItem3.iFieldType);
		test((*textDef)[2].iSeperator==aItem3.iSeperator);
		}
	if (aItem4.iFieldType!=KNullUid)
		{
		test((*textDef)[3].iFieldType==aItem4.iFieldType);
		test((*textDef)[3].iSeperator==aItem4.iSeperator);
		}
//
	TText chars[0x100];
	for(TInt length=0;length<=aTestText.Length();length++)
		{
		Mem::FillZ(&chars[0],sizeof(chars));
		TPtr ptr(&chars[0],length);
		if (item)
			TheDb->ReadContactTextDefL(*item,ptr,textDef);
		else
			TheDb->ReadContactTextDefL(aTextDefItem,ptr,textDef);
		test(ptr==aTestText.Left(length));
		}
//
	TheDb->SetTextDefinitionL(textDef);
	CleanupStack::Pop();	// textDef
	TBuf<0x100> buf1;
	if (item)
		TheDb->ReadContactTextDefL(*item,buf1);
	else
		TheDb->ReadContactTextDefL(aTextDefItem,buf1);
	test(buf1==aTestText);
	TheDb->SetTextDefinitionL(NULL);
	if (aPreLoadTest)
		CleanupStack::PopAndDestroy();	// item
	}

LOCAL_C void TestTextDefinitionL(TContactItemId aTextDefItem, TContactTextDefItem aItem1, TContactTextDefItem aItem2, TContactTextDefItem aItem3, TContactTextDefItem aItem4, TFieldType aFieldType, const TDesC &aTestText)
	{
	doTestTextDefinitionL(EFalse, aTextDefItem, aItem1, aItem2, aItem3, aItem4, aFieldType, aTestText);
	doTestTextDefinitionL(ETrue, aTextDefItem, aItem1, aItem2, aItem3, aItem4, aFieldType, aTestText);
	}

LOCAL_C void TestContactTextDefL()
	{
	CContactTextDef* textDef1=CContactTextDef::NewL();
	delete textDef1;
	CContactTextDef* textDef2=CContactTextDef::NewLC();
	CleanupStack::PopAndDestroy();	// textDef2
	textDef2=NULL;	// to shut unused warning up
//
	CContactItem* item=CContactCard::NewLC();
	SetNameL(*item,KUidContactFieldGivenName,_L("Given"));
	SetNameL(*item,KUidContactFieldFamilyName,_L("Family"));
	SetNameL(*item,KUidContactFieldSuffixName,_L("Suffix"));
	SetNameL(*item,KUidContactFieldCompanyName,_L("Company"));
	SetNameL(*item,KUidContactFieldNote,_L("Two line\nnote"));
	TTime testTime(TDateTime(2001,EJuly,15,18,0,0,0));
	SetTimeDateL(*item,KUidContactFieldBirthday,testTime);
	TheDb->SetDateFormatTextL(KDateFormatText1);
	TContactItemId textDefItem=TheDb->AddNewContactL(*item);
	CleanupStack::PopAndDestroy(); // item
//
	CContactItemViewDef *itemDef=CContactItemViewDef::NewLC(CContactItemViewDef::EIncludeFields,CContactItemViewDef::EMaskHiddenFields);
	itemDef->AddL(KUidContactFieldMatchAll);
	CContactViewDef *def=CContactViewDef::NewL(itemDef);
	CleanupStack::Pop(); // itemDef
	CleanupStack::PushL(def);
	TheDb->SetViewDefinitionL(def);
	CleanupStack::Pop(); // def
//
	TestTextDefinitionL(textDefItem,
						TContactTextDefItem(KUidContactFieldAdditionalName,_L(",")),
						TContactTextDefItem(KUidContactFieldCompanyName),
						TContactTextDefItem(),
						TContactTextDefItem(),
						KUidContactFieldNone,
						_L("Company"));
//
	TestTextDefinitionL(textDefItem,
						TContactTextDefItem(KUidContactFieldGivenName,_L("<-->")),
						TContactTextDefItem(KUidContactFieldFamilyName),
						TContactTextDefItem(),
						TContactTextDefItem(),
						KUidContactFieldNone,
						_L("Given<-->Family"));
//
	TestTextDefinitionL(textDefItem,
						TContactTextDefItem(KUidContactFieldFamilyName,_L(", ")),
						TContactTextDefItem(KUidContactFieldGivenName,_L("")),
						TContactTextDefItem(KUidContactFieldCompanyName),
						TContactTextDefItem(),
						KUidContactFieldNone,
						_L("Family, GivenCompany"));
//
	TestTextDefinitionL(textDefItem,
						TContactTextDefItem(KUidContactFieldNote,_L(", ")),
						TContactTextDefItem(KUidContactFieldGivenName,_L("")),
						TContactTextDefItem(KUidContactFieldCompanyName),
						TContactTextDefItem(),
						KUidContactFieldSuffixName,
						_L("Two line, GivenCompany"));
//
	TestTextDefinitionL(textDefItem,	// None of these fields exist in the DB
						TContactTextDefItem(KUidContactFieldAdditionalName,_L("")),
						TContactTextDefItem(KUidContactFieldAddress,_L("----")),
						TContactTextDefItem(KUidContactFieldPrefixName),
						TContactTextDefItem(),
						KUidContactFieldNone,
						_L("Given"));
	TestTextDefinitionL(textDefItem,	// None of these fields exist in the DB, fall back to suffix
						TContactTextDefItem(KUidContactFieldAdditionalName,_L("")),
						TContactTextDefItem(KUidContactFieldAddress,_L("----")),
						TContactTextDefItem(KUidContactFieldPrefixName),
						TContactTextDefItem(),
						KUidContactFieldSuffixName,
						_L("Suffix"));
//
	TestTextDefinitionL(textDefItem,	// None of these fields exist in the DB, and no fallback field Ivor.
						TContactTextDefItem(KUidContactFieldAdditionalName,_L("")),
						TContactTextDefItem(KUidContactFieldAddress,_L("----")),
						TContactTextDefItem(KUidContactFieldPrefixName),
						TContactTextDefItem(),
						KUidContactFieldNone,
						_L("Given"));
//
	TestTextDefinitionL(textDefItem,	// None of these fields exist in the DB, including the fallback field.
						TContactTextDefItem(KUidContactFieldAdditionalName,_L("")),
						TContactTextDefItem(KUidContactFieldAddress,_L("----")),
						TContactTextDefItem(KUidContactFieldPrefixName),
						TContactTextDefItem(),
						KUidContactFieldSms,
						_L("Given"));
						
//
//
	TheDb->SetTextDefinitionL(NULL);
	}
	

// Test for Defect INC066425 ContactDatabase::SetViewDefinitionL panic  
// This tests that when a view is defined to exclude all fields in the empty array (count=0)
// that there is NO panic.
// The original test done in CContactDatabase::SetViewDefinitionL for the panic was not valid:
	
LOCAL_C void TestSetViewDefinitionPanicL()
	{
	CContactItemViewDef* contactItemViewDef = CContactItemViewDef::NewLC( CContactItemViewDef::EMaskFields, CContactItemViewDef::EIncludeHiddenFields ) ;
   	CContactViewDef* cvd = CContactViewDef::NewLC( contactItemViewDef ) ;
   	CleanupStack::Pop();	// contactItemViewDef
   	TheDb->SetViewDefinitionL( cvd ) ;
   	CleanupStack::Pop();	// cvd
	}	
	
// Contact view obverser class used by TestOOMOnServerSideL.
class CntViewObserver : public MContactViewObserver
	{
public:
	void HandleContactViewEvent(const CContactViewBase &aView, const TContactViewEvent &aEvent);
	};

void CntViewObserver::HandleContactViewEvent(const CContactViewBase& /*aView*/, const TContactViewEvent& /*aEvent*/)
	{
	CActiveScheduler::Stop();
	}
	
#if defined(_DEBUG)
// Wait for server exit which is used by TestOOMOnServerSideL.
LOCAL_C void WaitForServerToExit()
	{
	TInt findProcessResult(KErrNone);
	
	// The name of the CntSrv process includes its uid like this [10003a73]
	TFindProcess findProcess(_L("*[10003a73]*"));
	TFullName fullName;
	
	findProcessResult = findProcess.Next(fullName);
	while(findProcessResult == KErrNone)
		{
		// the CntSrv process is running so wait 
		RProcess process;
		if (process.Open(fullName) == KErrNone)
			{
			//process.Kill(0);
			TRequestStatus status;
			process.Logon(status); // ask for a callback when the process ends
			User::WaitForRequest(status);
			User::LeaveIfError(status.Int());
			test.Printf(_L("*** The CntSrv process has been stopped ***\n"));
			process.Close();
			}
			
		findProcessResult = findProcess.Next(fullName);
		}
		
	}
#endif

#define HEAPFAIL_RATE_START 560	//The rate just start to triger OOM fail in CViewIteratorManager::CreateViewFromMultipleTablesL
#define HEAPFAIL_RATE_END   570	//The rate just end of trigering OOM fail in CViewIteratorManager::CreateViewFromMultipleTablesL

#if defined(_DEBUG)
// Test for Defect DEF086689  Remote view server-side memory leak   
// This tests is to check if there is any memory leak on server side when creating a remote view.
LOCAL_C void TestOOMOnServerSideL(TBool aRemoteView)
	{
	CntViewObserver observer;
	RContactViewSortOrder contactViewSortOrder;
	contactViewSortOrder.AppendL( KUidContactFieldAddress );
	contactViewSortOrder.AppendL( KUidContactFieldVCardMapTEL );
	contactViewSortOrder.AppendL( KUidContactFieldPhoneNumber );

	TInt count(HEAPFAIL_RATE_START);
	
	while(count++ < HEAPFAIL_RATE_END)
		{
		//Populate contact data;
		for(TInt i(0); i < 10; i++)
			{
			PopulateDatabaseL();
			}
		
		TInt err(KErrNone);
		CContactViewBase* contactView=NULL;
		
		//Enable server side OOM failure when client is creating remote view.
		TheDb->SetCntServerHeapFailure(RHeap::EDeterministic,count);
		if(aRemoteView)
			{
		    TRAP(err, contactView = CContactRemoteView::NewL(observer, *TheDb, contactViewSortOrder, EContactsOnly));
			}
		else
			{
		    TRAP(err, contactView = CContactLocalView::NewL(observer, *TheDb, contactViewSortOrder, EContactsOnly));
			}
		TheDb->SetCntServerHeapFailure(RHeap::ENone,0);
		
		if(err == KErrNone)	    
			{
			contactView->Close(observer);
			}
			
	    //delete the session and wait for the server to close  
	    CntTest->CloseDatabase();
	    WaitForServerToExit();
		
		// Re=open the database for future test cases
		TheDb = CntTest->OpenDatabaseL();
		}	
	}	
#endif

/**

@SYMTestCaseID     PIM-T-VIEW-0001

*/

LOCAL_C void DoTestsL()
	{
	CTestRegister * TempFiles = CTestRegister::NewLC();
	TempFiles->RegisterL(KDatabaseFileName, EFileTypeCnt);

	test.Start(_L("@SYMTESTCaseID:PIM-T-VIEW-0001 CContactItemViewDef with only iMajor typing info"));

	TRAPD(err,CreateView1L());
	test(err==KErrNone);

	test.Next(_L("CContactItemViewDef with full typing info"));

	TRAP(err,CreateView2L());
	test(err==KErrNone);
	
	// create test database
	TheDb=CntTest->CreateDatabaseL();
	CntTest->DeleteAllTemplateFieldsL();
	PopulateDatabaseL();
	test.Next(_L("Read item with view defs"));

	TRAP(err,SetViewsL());
	test(err==KErrNone);
	
	test.Next(_L("Read item with view masks"));

	TRAP(err,SetMaskedViewsL());
	test(err==KErrNone);

	test.Next(_L("CContactTextDef tests"));

	TRAP(err,TestContactTextDefL());
	test(err==KErrNone);

	test.Next(_L("INC066425 SetViewDefinitionL panic test"));

	TRAP(err,TestSetViewDefinitionPanicL());
	test(err==KErrNone);
	
	// create test database
	TheDb=CntTest->CreateDatabaseL();
	
#if defined(_DEBUG)	
// DEF100291:
// The macro is added because TestOOMOnServerSideL tests for Memory Leaks. 
// This is only possible in DEBUG mode. 
// In addition, this test function cannot be executed in the Techview environment, as
// the boot-up sequence includes the argument: "nontransient", which will
// cause this function to time out (hang).	
	test.Next(_L("DEF086689 Remote view OOM test on server side"));

	TRAP(err,TestOOMOnServerSideL(ETrue));
	test(err==KErrNone);
	
	test.Next(_L("DEF086713 Local view OOM test on server side"));

	TRAP(err,TestOOMOnServerSideL(EFalse));
	test(err==KErrNone);
#endif	
	
	CntTest->CloseDatabase();

	CleanupStack::PopAndDestroy(TempFiles);
	
	// stop efsrv.lib warning on 8.1a wins
	TEntry dummy;
	(void)dummy.IsTypeValid();
	}


LOCAL_C void CleanupFilesL()
	{
    // delete the database file
	if (CContactDatabase::ContactDatabaseExistsL(KDatabaseFileName) )
		{
		CContactDatabase::DeleteDatabaseL(KDatabaseFileName);
		}
	}


GLDEF_C TInt E32Main()
	{
    CntTest=new(ELeave) CCntTest;
	CntTest->ConstructL(test,KDatabaseFileName);
    TRAPD(err,DoTestsL());
	TRAP_IGNORE(CleanupFilesL() );
	CntTest->EndTestLib(err);
	return KErrNone;
    }
