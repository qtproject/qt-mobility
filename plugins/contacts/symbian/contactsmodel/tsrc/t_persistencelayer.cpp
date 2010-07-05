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
* Persistence Layer Test module
*
*/



#include <e32test.h>
#include <coreappstest/testserver.h>
#include "persistencelayer.h"
#include "cnttestsynchroniser.h"

_LIT(KTestName, "T_PersistenceLayer"); // Used by Testhelper

#include "testhelpers.h"

#include "nbcnttestlib/nbcnttestlib.h" 

#include "t_persistencelayer.h"
#include "t_cnttestimpldefs.h"


#define RUN_PL
#define RUN_CNT

#define KUidContactFieldCustom1Value		0x101FD209
#define KUidContactFieldCustom2Value		0x101FD20A
const TUid KUidContactFieldCustom1={KUidContactFieldCustom1Value};
const TUid KUidContactFieldCustom2={KUidContactFieldCustom2Value};
_LIT(KCrudTestDbFile, "C:CRUD_TEST");

////////////////////////////////////////////////////////////////////
// This test used to spit out a lot of html files which display
// the contents of contact items. There are a lot of files
// generated in the process which, as this is normally run on the
// overnight build, are not read and just take up disk space.
// Therefore, the call to the contact dumping method has been
// configured out by #ifdefs and needs to be manually re-activated.
//
// So, uncomment the following line to turn on contact dumping.
//#define __T_PERSISTENCELAYER_DUMP_CONTACT__
////////////////////////////////////////////////////////////////////


void CPersistenceTests::AllTestsL()
	{
	FileCreationL();

	IccL();
	DuplicatedFieldUidsL();

	BasicCRUDL();
	ViewDefsInclL();
	ViewDefsMaskL();

	TDriveUnit drv(EDriveC);
	CDesCArray* arr = iCntTestImpl.ListDatabasesL(drv);
	delete arr;

	OomTestL();
	AccessCountL();
	TemplateAccessCountL();
	GroupsL();
	iCntTestImpl.CloseDatabase();
	}
	

void CPersistenceTests::IccL()
	{
	CContactSyncChecker::ResetL();
	CContactItem* iccEntry = CContactICCEntry::NewL(iCntTestImpl.GetSysTemplate());
	PUSH(iccEntry);
	
	TCnt theCnt(iccEntry);
	theCnt[KUidContactFieldGivenName] = _L("Johny");
	theCnt[KUidContactFieldFamilyName] = _L("Herbert");

	iCntTestImpl.CreateL(*iccEntry);

	POPD(iccEntry);
	}


void CPersistenceTests::OomTestL()
	{
	test.Next(_L("ReadLC OOM test"));
	CContactItem* cnt = iCntTestImpl.ReadLC(1, *iMatchAll);
	POPD(cnt);

	TInt ret = 0;
	for(TInt i=1; i < 2000; ++i)
		{
		__UHEAP_SETFAIL(RHeap::EDeterministic, i);
		__UHEAP_MARK;
		TRAP(ret, CContactItem* cnt = iCntTestImpl.ReadLC(1, *iMatchAll);	POPD(cnt); );
		__UHEAP_MARKEND;
		__UHEAP_RESET;

		if (ret == KErrNone)
			{
			break;
			}
		else if (ret != KErrNoMemory)
			{
			TESTVALUE(ret, KErrNoMemory);
			break;
			}
		if (!(i % 100))
			{
			test.Printf(_L("."), i/100);
			}
		}
	test.Printf(_L("\r\n"));
	}



void CPersistenceTests::TemplateAccessCountL()
	{
	test.Next(_L("Template Access Count test"));

	//Create a custom template
	CContactItem* templ = iCntTestImpl.CreateContactCardTemplateLC(_L("UserTemplate"));
	TContactItemId templId = templ->Id();
	POPD(templ); templ = NULL;

	//Read the template from the database
	templ = iCntTestImpl.ReadLC(templId, *iMatchAll);
	TESTVALUE(templ->IsDeleted(), EFalse);


	//Create a contact based on the template
	CContactItem* card = CContactCard::NewLC(templ);
	TCnt theCnt(card);
	theCnt[KUidContactFieldGivenName] += _L("Contact");
	theCnt[KUidContactFieldFamilyName] += _L("Based on");
	theCnt[KUidContactFieldEMail] & KHome += _L("User Template");
	iCntTestImpl.CreateL(*card);

	const TInt createdEntryId =  card->Id();
	POPD(card); card = NULL;
	POPD(templ); templ = NULL;

	//Try to delete the template (it should only be marked and deleted)
	TESTTRAP(iCntTestImpl.DeleteL(templId));

	//Read the template from the database
	//It should work and the template must be marked as deleted
	templ = iCntTestImpl.ReadLC(templId, *iMatchAll);
	TESTTRUE(templ->IsDeleted());
	POPD(templ); templ = NULL;

	//Delete the only contact, based on the user template
	TESTTRAP(iCntTestImpl.DeleteL(createdEntryId));

	//Because the contact was the only thing dependant on the template
	//Template was deleted also
	TRAPD(err, iCntTestImpl.ReadLC(templId, *iMatchAll); POPD(templ););
	TESTVALUE(err, KErrNotFound);
	}

void CPersistenceTests::AccessCountL()
	{
	test.Next(_L("Access Count test"));
	static const TInt KAccessReps = 10;
	TInt i = 0;
	const CContactTemplate& goldenTemplate = iCntTestImpl.GetSysTemplate();

	//Create a contact and set Acces Count to 10.
	CContactItem* card = CContactCard::NewLC(&goldenTemplate);
	TCnt theCnt(card);

	theCnt[KUidContactFieldGivenName] = _L("Giancarlo");
	theCnt[KUidContactFieldFamilyName] = _L("Fisichella");
	theCnt[KUidContactFieldPhoneNumber] & KHome & KVoice & KCell = _L("mo1 39849342");
	theCnt[KUidContactFieldEMail] & KHome = _L("home@email");
	theCnt[KUidContactFieldAddress] & KHome = _L("Home addresssssss");
//	theCnt[KUidContactFieldLocality] & KHome = _L("Rome");

	for(i = 0; i < KAccessReps; ++i )
		{
		card->IncAccessCount();
		}
	TESTTRUE(card->AccessCount() == KAccessReps);;
	iCntTestImpl.CreateL(*card);
	const TInt createdEntryId =  card->Id();
	POPD(card);

	//Check is Access cound was saved
	card = iCntTestImpl.ReadLC(createdEntryId, *iMatchAll);
	TESTTRUE(card->AccessCount() == KAccessReps);;
	POPD(card);	

	//Try to delete entry 10 times
	for(i = 0; i < KAccessReps; ++i )
		{
		iCntTestImpl.DeleteL(createdEntryId);
		}
	//Delete doesn't physically delete an entry, is simply marks is as deleted.

	//Check is Access count was saved, set it back to 0 and update
	card = iCntTestImpl.OpenLC(createdEntryId, *iMatchAll);
	TESTTRUE(card->IsDeleted());
	TESTTRUE(card->AccessCount() == KAccessReps);;
	for(i = 0; i < KAccessReps; ++i )
		{
		card->DecAccessCount();
		}

	iCntTestImpl.UpdateL(*card); //The contact is actually deleted here
	POPD(card);					 //because the access count is now 0

	//The contact should not be there anymore
	TRAPD(err, card = iCntTestImpl.ReadLC(createdEntryId, *iMatchAll));
	TESTTRUE(err == KErrNotFound);
	}


void CPersistenceTests::DuplicatedFieldUidsL()
	{
	CContactItem* cntItem = iCntTestImpl.OpenLC(1, *iMatchAll);
	TCnt theCnt(cntItem);
#ifdef __T_PERSISTENCELAYER_DUMP_CONTACT__
	TESTTRAP(iCntTestImpl.DumpContactL(*cntItem, _L("Before UID field")));
#endif

	theCnt[KUidContactFieldPhoneNumber] & KWork & KPager = _L("New work pager");
	
	iCntTestImpl.UpdateL(*cntItem);

#ifdef __T_PERSISTENCELAYER_DUMP_CONTACT__
	TESTTRAP(iCntTestImpl.DumpContactL(*cntItem, _L("After UID field")));
#endif
	
	TPtrC phone = theCnt[KUidContactFieldPhoneNumber] & KWork & KPager;

	POPD(cntItem);
	}


// Implementation of CPersistenceTests member functions

void CPersistenceTests::FileCreationL()
	{
	test.Next(_L("File creation test"));

#ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__	
	TRAPD(err,iCntTestImpl.CreateDatabaseL(_L("JustAFilename.ext"), ETrue));
	TESTVALUE(err, KErrBadName); //The filename MUST contain a drive name
#endif

	iCntTestImpl.CreateDatabaseL(KCrudTestDbFile, ETrue);
	const CContactTemplate& goldenTemplate = iCntTestImpl.GetSysTemplate();

#ifdef __T_PERSISTENCELAYER_DUMP_CONTACT__
	TESTTRAP(iCntTestImpl.DumpContactL(goldenTemplate, _L("Template")));
#endif

	CContactItem* card = CContactCard::NewLC(&goldenTemplate);
	TBuf<10> uidStr(_L("UidString"));
	card->SetUidStringL(uidStr);
	TCnt theCnt(card);

	theCnt[KUidContactFieldGivenName] = _L("David");
	theCnt[KUidContactFieldFamilyName] = _L("Coulthard");

	theCnt[KUidContactFieldPhoneNumber] & KHome & KVoice & KCell = _L("mo1 39849342");
	theCnt[KUidContactFieldPhoneNumber] & KHome & KVoice & KCell += _L("mo2 34530903495803948503984545");

	theCnt[KUidContactFieldEMail] & KHome = _L("home@email");
	theCnt[KUidContactFieldAddress] & KHome = _L("Home addresssssss");
	theCnt[KUidContactFieldLocality] & KHome = _L("Glazgo");
	theCnt[KUidContactFieldIMAddress] & KHome & KWv = _L("wireless@village");

	//If the code fails on this line, you most likely haven't built the cnttutil
	//test library that create the test version of the system template cntmodel.rsc
	theCnt[KUidContactFieldSecondName] = _L("Second name");

	theCnt[KUidContactFieldCompanyName] = _L("McLaren");
	theCnt[KUidContactFieldJobTitle]	= _L("Driver");
	theCnt[KUidContactFieldPhoneNumber] & KWork & KVoice = _L("work Tel 2342");

	theCnt[KUidContactFieldEMail] & KWork = _L("work@email");
	theCnt[KUidContactFieldUrl] & KWork = _L("sdfsd");
	theCnt[KUidContactFieldAddress] & KWork = _L("afas df asd");
	theCnt[KUidContactFieldLocality] & KWork = _L("sdfsd");

	theCnt[KUidContactFieldCustom1] = _L("Filterable 1");
	theCnt[KUidContactFieldCustom2] = _L("Filterable 2");

	theCnt[KUidContactFieldGivenNamePronunciation] = _L("first name re");
	theCnt[KUidContactFieldFamilyNamePronunciation] = _L("Last name re");
	
	theCnt[KUidContactFieldSIPID] & KPoc  = _L("SIP POC");
	theCnt[KUidContactFieldSIPID] & KSwis = _L("SIP SWIS");
	theCnt[KUidContactFieldSIPID] & KVoip = _L("SIP VOIP");

#ifdef __T_PERSISTENCELAYER_DUMP_CONTACT__
	TESTTRAP(iCntTestImpl.DumpContactL(*card, _L("Before Creation")));
#endif
	iCntTestImpl.CreateL(*card);
	POPD(card);

	card = CContactCard::NewLC(&goldenTemplate);
	theCnt[KUidContactFieldGivenName] = _L("Elton");
	theCnt[KUidContactFieldFamilyName] = _L("John");
	theCnt[KUidContactFieldCompanyName] = _L("EMI");
	theCnt[KUidContactFieldJobTitle]	= _L("Singer");
	theCnt[KUidContactFieldPhoneNumber] & KHome & KVoice & KCell = _L("tel. 39849342");
	iCntTestImpl.CreateL(*card);
	POPD(card);
	
	//Create a group
	CContactGroup* group1 = iCntTestImpl.CreateGroupLC(_L("MyGroup1"));
	POPD(group1);
	}

void CPersistenceTests::ViewDefsMaskL()
	{
	test.Next(_L("View Definition with Masks"));

#ifdef __T_PERSISTENCELAYER_DUMP_CONTACT__
	TESTTRAP(iCntTestImpl.PrintCntForViewL(_L("Full"), *iMatchAll));
#endif

	//ViewDefinition with masking of normal fields
	CContactItemViewDef* viewDef0 = CContactItemViewDef::NewLC( CContactItemViewDef::EMaskFields,
																CContactItemViewDef::EIncludeHiddenFields);	

#ifdef __T_PERSISTENCELAYER_DUMP_CONTACT__
	TESTTRAP(iCntTestImpl.PrintCntForViewL(_L("MaskFields_nothing"), *viewDef0));
#endif

	viewDef0->AddL(KUidContactFieldGivenName);
#ifdef __T_PERSISTENCELAYER_DUMP_CONTACT__
	TESTTRAP(iCntTestImpl.PrintCntForViewL(_L("Mask_GivenName"), *viewDef0));
#endif

	  viewDef0->AddL(KUidContactFieldPhoneNumber);
#ifdef __T_PERSISTENCELAYER_DUMP_CONTACT__
	  TESTTRAP(iCntTestImpl.PrintCntForViewL(_L("Mask_GNameAndPhones"), *viewDef0));
#endif

		viewDef0->AddL(KUidContactFieldEMail);
#ifdef __T_PERSISTENCELAYER_DUMP_CONTACT__
		TESTTRAP(iCntTestImpl.PrintCntForViewL(_L("Mask_GNamePhonesAndEmails"), *viewDef0));
#endif

		viewDef0->Remove(2);
	  viewDef0->Remove(1);
	viewDef0->Remove(0);

	viewDef0->AddL(KHome);
#ifdef __T_PERSISTENCELAYER_DUMP_CONTACT__
	TESTTRAP(iCntTestImpl.PrintCntForViewL(_L("Mask_Home"), *viewDef0));
#endif
	viewDef0->Remove(0);

	POPD(viewDef0);

		
	//ViewDefinition with masking of hidden fields
	CContactItemViewDef* viewDef1 = CContactItemViewDef::NewLC( CContactItemViewDef::EIncludeFields,
																CContactItemViewDef::EMaskHiddenFields);

	viewDef1->AddL(KUidContactFieldGivenName);
#ifdef __T_PERSISTENCELAYER_DUMP_CONTACT__
	TESTTRAP(iCntTestImpl.PrintCntForViewL(_L("OnlyGivenName_noH"), *viewDef1));
#endif
	viewDef1->Remove(0);

	viewDef1->AddL(KUidContactFieldPicture);
#ifdef __T_PERSISTENCELAYER_DUMP_CONTACT__
	TESTTRAP(iCntTestImpl.PrintCntForViewL(_L("noH_Photo"), *viewDef1));
#endif
	viewDef1->Remove(0);
	POPD(viewDef1);

	//todo: add more testing for hidden fields.
	}



void CPersistenceTests::ViewDefsInclL()
	{
	test.Next(_L("View Definition with Inclusions"));

	CContactItem* cntItem = NULL;

	//ViewDefinition with inclusion
	CContactItemViewDef* viewDef1 = CContactItemViewDef::NewLC(CContactItemViewDef::EIncludeFields,CContactItemViewDef::EIncludeHiddenFields);

	cntItem = iCntTestImpl.ReadMinimalLC(1);
	
#ifdef __T_PERSISTENCELAYER_DUMP_CONTACT__
	TESTTRAP(iCntTestImpl.DumpContactL(*cntItem, _L("Minimal")));
#endif

	POPD(cntItem);

#ifdef __T_PERSISTENCELAYER_DUMP_CONTACT__
	TESTTRAP(iCntTestImpl.PrintCntForViewL(_L("Full"), *iMatchAll));
#endif

	viewDef1->AddL(KUidContactFieldPhoneNumber);
#ifdef __T_PERSISTENCELAYER_DUMP_CONTACT__
	TESTTRAP(iCntTestImpl.PrintCntForViewL(_L("OnlyPhones"), *viewDef1));
#endif
	viewDef1->Remove(0);

	viewDef1->AddL(KUidContactFieldAddress);
#ifdef __T_PERSISTENCELAYER_DUMP_CONTACT__
	TESTTRAP(iCntTestImpl.PrintCntForViewL(_L("OnlyAddress"), *viewDef1));
#endif
	viewDef1->Remove(0);

	viewDef1->AddL(KUidContactFieldGivenName);
#ifdef __T_PERSISTENCELAYER_DUMP_CONTACT__
	TESTTRAP(iCntTestImpl.PrintCntForViewL(_L("OnlyGivenName"), *viewDef1));
#endif
	viewDef1->Remove(0);

	viewDef1->AddL(KUidContactFieldEMail);
#ifdef __T_PERSISTENCELAYER_DUMP_CONTACT__
	TESTTRAP(iCntTestImpl.PrintCntForViewL(_L("OnlyEmails"), *viewDef1));
#endif
	  viewDef1->AddL(KUidContactFieldPhoneNumber);
#ifdef __T_PERSISTENCELAYER_DUMP_CONTACT__
	  TESTTRAP(iCntTestImpl.PrintCntForViewL(_L("Emails_and_Phones"), *viewDef1));
#endif
		viewDef1->AddL(KUidContactFieldFamilyName);
#ifdef __T_PERSISTENCELAYER_DUMP_CONTACT__
		TESTTRAP(iCntTestImpl.PrintCntForViewL(_L("EmailsPhonesAndFamilyName"), *viewDef1));
#endif
		viewDef1->Remove(2);
	  viewDef1->Remove(1);
	viewDef1->Remove(0);

	viewDef1->AddL(KUidContactFieldBirthday);
#ifdef __T_PERSISTENCELAYER_DUMP_CONTACT__
	TESTTRAP(iCntTestImpl.PrintCntForViewL(_L("OnlyBirthday"), *viewDef1));
#endif
	viewDef1->Remove(0);

	viewDef1->AddL(KUidContactFieldPicture);
#ifdef __T_PERSISTENCELAYER_DUMP_CONTACT__
	TESTTRAP(iCntTestImpl.PrintCntForViewL(_L("OnlyPicture"), *viewDef1));
#endif
	viewDef1->Remove(0);


	POPD(viewDef1);
	}



void CPersistenceTests::GroupsL()
	{
	test.Next(_L("Groups testing"));
	
	//Create a new group
	CContactGroup* group1 = iCntTestImpl.CreateGroupLC(_L("NewGroupLabel"));				//>>>
	const TContactItemId newGroupID = group1->Id();
#ifdef __T_PERSISTENCELAYER_DUMP_CONTACT__
	TESTTRAP(iCntTestImpl.DumpContactL(*group1, _L("NewGroup_created")));
#endif
	
	//Add existing contact to the group
	iCntTestImpl.GroupAddAndRemove(*group1, 1, -1);
#ifdef __T_PERSISTENCELAYER_DUMP_CONTACT__
	TESTTRAP(iCntTestImpl.DumpContactL(*group1, _L("GroupWithContAdded")));
#endif
	
	//Add another existing contact to the groups
	iCntTestImpl.GroupAddAndRemove(*group1, 2, -1);
#ifdef __T_PERSISTENCELAYER_DUMP_CONTACT__
	TESTTRAP(iCntTestImpl.DumpContactL(*group1, _L("GroupWithCont2Added")));
#endif
	
	//Add the created group to an existing group
	CContactGroup* group3 = static_cast<CContactGroup*>(iCntTestImpl.ReadLC(3, *iMatchAll));	//>>>
#ifdef __T_PERSISTENCELAYER_DUMP_CONTACT__
	TESTTRAP(iCntTestImpl.DumpContactL(*group3, _L("ExitstingGroup3")));
#endif
	iCntTestImpl.GroupAddAndRemove(*group3, group1->Id(), -1);
	POPD(group3);																	//<<<
	POPD(group1);																	//<<<

	//Print the existing group contents
	group3 = static_cast<CContactGroup*>(iCntTestImpl.ReadLC(3, *iMatchAll));					//>>>
#ifdef __T_PERSISTENCELAYER_DUMP_CONTACT__
	TESTTRAP(iCntTestImpl.DumpContactL(*group3, _L("Group3AddedGroup1")));
#endif
	POPD(group3);																	//<<<

	//Delete one of the contacts, belonging to the newly created group
	iCntTestImpl.DeleteL(2);
	group1 = static_cast<CContactGroup*>(iCntTestImpl.ReadLC(newGroupID, *iMatchAll));			//>>>
#ifdef __T_PERSISTENCELAYER_DUMP_CONTACT__
	TESTTRAP(iCntTestImpl.DumpContactL(*group1, _L("GroupWithContRemoved")));
#endif
	POPD(group1);																	//<<<

	//Delete new group
	iCntTestImpl.DeleteL(newGroupID);
	group3 = static_cast<CContactGroup*>(iCntTestImpl.ReadLC(3, *iMatchAll));					//>>>
#ifdef __T_PERSISTENCELAYER_DUMP_CONTACT__
	TESTTRAP(iCntTestImpl.DumpContactL(*group3, _L("Group3-Group1WasDeleted")));
#endif
	///////////////
	
	TRAPD(err, iCntTestImpl.GroupAddAndRemove(*group3, 238, -1));
	TESTVALUE(err, KErrNotFound);
	TRAP(err, iCntTestImpl.GroupAddAndRemove(*group3, -1, 1257));
	TESTVALUE(err, KErrNotFound);

	POPD(group3);																	//<<<
	}


void CPersistenceTests::BasicCRUDL()
	{
	test.Next(_L("Basic CRUD test"));


	test.Printf(_L("Reading Template\n"));
	CContactItem* cntItem = iCntTestImpl.ReadMinimalLC(0);
#ifdef __T_PERSISTENCELAYER_DUMP_CONTACT__
	TESTTRAP(iCntTestImpl.DumpContactL(*cntItem, _L("GoldenTempl")));
#endif
	POPD(cntItem);

	test.Printf(_L("Reading First contact\n"));
	cntItem = iCntTestImpl.ReadMinimalLC(1);
#ifdef __T_PERSISTENCELAYER_DUMP_CONTACT__
	TESTTRAP(iCntTestImpl.DumpContactL(*cntItem, _L("Contact_1")));
#endif

	TCnt theCnt2(cntItem);

	theCnt2[KUidContactFieldPhoneNumber] & KHome & KVoice & KCell = _L("456456456");
	theCnt2[KUidContactFieldPhoneNumber] & KWork & KVoice = _L("456456456");
	
	//Note that we are trying to create a contact, based on another conact which
	//was read using ReadMinimal API. This API doest read template information
	//So the newly created contact will not be bases on any template 
	//(even not on Golden Template e.i.  TemplateRefId() == 0)
	test.Printf(_L("Create new contact\n"));
	TContactItemId newItemId = 0;
	TESTTRAP(newItemId = iCntTestImpl.CreateL(*cntItem)); 
#ifdef __T_PERSISTENCELAYER_DUMP_CONTACT__
	TESTTRAP(iCntTestImpl.DumpContactL(*cntItem, _L("InMemoryAfterCreation")));
#endif
	POPD(cntItem);

	test.Printf(_L("Reading Created contact\n"));
	cntItem = iCntTestImpl.OpenLC(newItemId, *iMatchAll);
#ifdef __T_PERSISTENCELAYER_DUMP_CONTACT__
	TESTTRAP(iCntTestImpl.DumpContactL(*cntItem, _L("Created_ReadFromDb")));
#endif
	
	TCnt theCnt3(cntItem);
	theCnt3[KUidContactFieldGivenName] = _L("new Given Name");
	theCnt3[KUidContactFieldPhoneNumber] & KWork & KVoice = _L("123123123");
	theCnt3[KUidContactFieldPhoneNumber] & KWork & KVoice & KCell += _L("123123123");
	theCnt3[KUidContactFieldEMail] & KHome = _L("new@home");
	theCnt3[KUidContactFieldAddress] & KHome = _L("new home address");

	test.Printf(_L("Update contact\n"));
#ifdef __T_PERSISTENCELAYER_DUMP_CONTACT__
	TESTTRAP(iCntTestImpl.DumpContactL(*cntItem, _L("BeforeUpdate")));
#endif
	TESTTRAP(iCntTestImpl.UpdateL(*cntItem));
#ifdef __T_PERSISTENCELAYER_DUMP_CONTACT__
	TESTTRAP(iCntTestImpl.DumpContactL(*cntItem, _L("AfterUpdate")));
#endif
	POPD(cntItem);

	test.Printf(_L("Reading Updated contact\n"));
	cntItem = iCntTestImpl.ReadMinimalLC(newItemId);
#ifdef __T_PERSISTENCELAYER_DUMP_CONTACT__
	TESTTRAP(iCntTestImpl.DumpContactL(*cntItem, _L("Updated_ReadFromDb")));
#endif
	
	iCntTestImpl.DeleteL(cntItem->Id()); 
	
	POPD(cntItem);
	}


//Persistence test construction and destruction
CPersistenceTests* CPersistenceTests::NewLC (CCntTestImplementor& aCntTestImpl)
	{
	CPersistenceTests* self = new (ELeave) CPersistenceTests(aCntTestImpl);
	CleanupStack::PushL(self);

	self->ConstructL();

	return self;
	}

void CPersistenceTests::ConstructL()
	{
	iMatchAll = CContactItemViewDef::NewLC(CContactItemViewDef::EIncludeFields,CContactItemViewDef::EIncludeHiddenFields);
	iMatchAll->AddL(KUidContactFieldMatchAll);
	CleanupStack::Pop(iMatchAll);
	}

CPersistenceTests::~CPersistenceTests()
	{
	delete iMatchAll;
	iMatchAll = NULL;
	}

/**

@SYMTestCaseID PIM-T-PERSISTENCELAYER-0001

*/

LOCAL_C void DoTestsL()
	{
    CleanupClosePushL(test);
	CPersistenceTests* cntTester = NULL;
	test.Start(_L("@SYMTestCaseID:PIM-T-PERSISTENCELAYER-0001 Persistency tests"));
	
	RPIMTestServer serv;
	User::LeaveIfError(serv.Connect());

#ifdef RUN_PL

	test.Start(_L("Persistence Layer Tests"));
    __UHEAP_MARK;
	CPlTestImpl* plCntTestImpl = CPlTestImpl::NewLC();
  	cntTester = CPersistenceTests::NewLC(*plCntTestImpl);
	cntTester->AllTestsL();
	test(__NB_failures == 0);

	POPD(cntTester);
	CleanupStack::PopAndDestroy(plCntTestImpl);
	plCntTestImpl = NULL;
	test.End();
    __UHEAP_MARKEND;

#endif 

#ifdef RUN_CNT

	test.Start(_L("Contact model tests"));
    __UHEAP_MARK;
	CCntTestImpl* oldCntTestImpl = CCntTestImpl::NewLC();

	cntTester = CPersistenceTests::NewLC(*oldCntTestImpl);
	cntTester->AllTestsL();
	POPD(cntTester);
	test(__NB_failures == 0);

	CleanupStack::PopAndDestroy(oldCntTestImpl);
	oldCntTestImpl = NULL;
	test.End();
    __UHEAP_MARKEND;
	User::After(1500000);
#endif	
	
	serv.Close();	
	test.End();
    CleanupStack::PopAndDestroy(1);  // test.Close

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

    // Run the tests
    __UHEAP_MARK;
    TRAPD(err, DoTestsL());
	test(err == KErrNone);
	TRAP_IGNORE(CContactDatabase::DeleteDatabaseL(KCrudTestDbFile) );
	__UHEAP_MARKEND;

	
    // Cleanup
    delete activeScheduler;
    delete cleanupStack;
	activeScheduler = NULL;
	cleanupStack = NULL;
	
	return err;
    }


