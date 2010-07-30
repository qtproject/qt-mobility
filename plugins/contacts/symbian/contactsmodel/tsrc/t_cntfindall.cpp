/*
* Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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


/**
@SYMTestCaseID PIM-T-CNTFINDALL-INC049017-0001
@SYMTestPriority High
@SYMDEF INC049017
@SYMTestCaseDependencies cnttutil
@SYMTestCaseDesc Tests that Identity fields are searched when FindAsyncL and FindLC are used with KUidContactFieldMatchAll.
@SYMTestActions Create a contact database and add four contacts.
Search the database by using identity fields: GivenName, FamilyName and CompanyName.
Search for contacts by providing a name and using KUidContactFieldMatchAll.
Perform the search synchronously and asynchronously.
@SYMTestExpectedResults Contacts are found by given name, family name or company name when using KUidContactFieldMatchAll.
*/
#include <e32std.h>
#include <e32test.h>
#include <e32math.h>
#include <cntdb.h>
#include <cntitem.h>
#include <cntfldst.h>

#include "t_utils2.h"
#include "t_cntfindall.h"


// Constants.
_LIT(KTestName,"t_CntFindAll");
_LIT(KDbFileName,"c:contacts.cdb");
_LIT(KLogFileName,"t_cntFindAll.log");
_LIT(KDummyData, "Some dummy data");

// CFindTest.
CFindTest::CFindTest()
 : iTest(KTestName),
 iLogFileName(KLogFileName),
 iDbFileName(KDbFileName)
	{
	}

CFindTest::~CFindTest()
	{
	delete iTemplate;
	delete iDb;
	delete iLog;
	iTest.Close();
	}

void CFindTest::ConstructL()
	{
	iLog=CLog::NewL(iTest,iLogFileName);
	}


void CFindTest::DoTestsL()
	{
	iTest.Title(); 
	iTest.Start(_L("@SYMTestCaseID:PIM-T-CNTFINDALL-INC049017-0001 CREATING THE DATABASE"));
	iDb=CContactDatabase::ReplaceL(iDbFileName);
	iTemplate=STATIC_CAST(CContactTemplate*,iDb->ReadContactL(iDb->TemplateId()));

	iTest.Next(_L("ADDING COMPACT CONTACTS"));
		{
		iLog->LogLine(_L("Adding The Contacts."));
	//	AddContactCompactL(aFirstName,aLastName,aCompanyName,aWorkPhone, 
	//												aHomePhone,& aWorkMobile,aHomeMobile,aWorkEmail)											
		AddContactCompactL(_L("Robert"),_L("Jordan"),_L("Software Solutions"),_L("015672354658"), 
												_L("01617246900"), _L("077962345100"),_L("07785674300"),_L("rj2@software_solutions.com"));									
		AddContactCompactL(_L("Amanda"),_L("Jordan"),_L("Software Solutions"),_L("015672354658"), 
												_L("01617246900"), _L("077962345101"),_L("07785674301"),_L("aj1@software_solutions.com"));									
		AddContactCompactL(_L("Ben"),_L("Turner"),_L("High Res Solutions"),_L("017652354658"), 
												_L("01717246901"), _L("07896543210"),_L("07987654321"),_L("bturn@highres.com"));									
		AddContactCompactL(_L("Chris"),_L("Andrews"),_L("Soft Kicks"),_L("1234567890"), 
												_L("01712345678"), _L("07234567890"),_L("07234567891"),_L("c_andrew@softkicks.com"));
		}

	//start of tests for defect where identity fields were not searched when using KUidMatchInAllFields
	iTest.Next(_L("FINDING CONTACTS BY NAME FROM GIVEN NAME FIELD."));
		{
		iLog->LogLine(_L("\r\nFINDING CONTACTS BY NAME FROM GIVEN NAME FIELD."));
		FindNameInFirstNameFieldL(_L("Chris"),1);
		FindNameInFirstNameFieldL(_L("Turner"),0);
		FindNameInFirstNameFieldL(_L("e"),2);
		}

	iTest.Next(_L("FINDING CONTACTS BY NAME FROM FAMILY NAME FIELD"));
		{
		iLog->LogLine(_L("\r\nFINDING CONTACTS BY NAME FROM FAMILY NAME FIELD."));
		FindNameInLastNameFieldL(_L("Turner"),1);
		FindNameInLastNameFieldL(_L("Jordan"),2);
		FindNameInLastNameFieldL(_L("r"),4);
		FindNameInLastNameFieldL(_L("Chris"),0);
		}

	iTest.Next(_L("FINDING CONTACTS BY NAME FROM ALL FIELDS"));
		{
		iLog->LogLine(_L("\r\nFINDING CONTACTS BY NAME FROM ALL FIELDS"));
		FindInAllFieldsL(_L("Ben"),1);
		FindInAllFieldsL(_L("Jordan"),2);
		FindInAllFieldsL(_L("r"),4);
		FindInAllFieldsL(_L("xxxxxxxxx"),0);
		}

	iTest.Next(_L("FINDING CONTACTS BY COMPANY FROM COMPANY NAME FIELD"));
		{
		iLog->LogLine(_L("\r\nFINDING CONTACTS COMPANY FROM COMPANY NAME FIELD"));
		FindInCompanyFieldL(_L("Ben"),0);
		FindInCompanyFieldL(_L("High Res Solutions"),1);
		FindInCompanyFieldL(_L("Software Solutions"),2);
		FindInCompanyFieldL(_L("s"),4);
		}

	iTest.Next(_L("FINDING CONTACTS BY COMPANY FROM  ALL FIELDS"));
		{
		iLog->LogLine(_L("\r\nFINDING CONTACTS BY COMPANY FROM ALL FIELDS"));
		FindInAllFieldsL(_L("Kick"),1);
		FindInAllFieldsL(_L("High Res Solutions"),1);
		FindInAllFieldsL(_L("Software Solutions"),2);
		FindInAllFieldsL(_L("s"),4);
		}
	//end of tests for defect where identity fields were not searched when using KUidMatchInAllFields

	//start of Async tests for defect where identity fields were not searched when using KUidMatchInAllFields
	iTest.Next(_L("FINDING CONTACTS BY NAME FROM GIVEN NAME FIELD ASYNCHRONOUSLY"));
		{
		iLog->LogLine(_L("\r\nFINDING CONTACTS BY NAME FROM GIVEN NAME FIELD ASYNCHRONOUSLY."));
		FindNameInFirstNameFieldL(_L("Chris"),1,ETrue);
		FindNameInFirstNameFieldL(_L("Turner"),0,ETrue);
		FindNameInFirstNameFieldL(_L("e"),2,ETrue);
		}

	iTest.Next(_L("FINDING CONTACTS BY NAME FROM FAMILY NAME FIELD ASYNCHRONOUSLY"));
		{
		iLog->LogLine(_L("\r\nFINDING CONTACTS BY NAME FROM FAMILY NAME FIELD ASYNCHRONOUSLY."));
		FindNameInLastNameFieldL(_L("Turner"),1,ETrue);
		FindNameInLastNameFieldL(_L("Jordan"),2,ETrue);
		FindNameInLastNameFieldL(_L("r"),4,ETrue);
		FindNameInLastNameFieldL(_L("Chris"),0,ETrue);
		}

	iTest.Next(_L("FINDING CONTACTS BY NAME FROM ALL FIELDS ASYNCHRONOUSLY"));
		{
		iLog->LogLine(_L("\r\nFINDING CONTACTS BY NAME FROM ALL FIELDS ASYNCHRONOUSLY"));
		FindInAllFieldsL(_L("Ben"),1,ETrue);
		FindInAllFieldsL(_L("Jordan"),2,ETrue);
		FindInAllFieldsL(_L("r"),4,ETrue);
		FindInAllFieldsL(_L("xxxxxxxxx"),0,ETrue);
		}

	iTest.Next(_L("FINDING CONTACTS BY COMPANY FROM COMPANY NAME FIELD"));
		{
		iLog->LogLine(_L("\r\nFINDING CONTACTS COMPANY FROM COMPANY NAME FIELD ASYNCHRONOUSLY"));
		FindInCompanyFieldL(_L("Ben"),0,ETrue);
		FindInCompanyFieldL(_L("High Res Solutions"),1,ETrue);
		FindInCompanyFieldL(_L("Software Solutions"),2,ETrue);
		FindInCompanyFieldL(_L("s"),4,ETrue);
		}

	iTest.Next(_L("FINDING CONTACTS BY COMPANY FROM  ALL FIELDS ASYNCHRONOUSLY"));
		{
		iLog->LogLine(_L("\r\nFINDING CONTACTS BY COMPANY FROM ALL FIELDS ASYNCHRONOUSLY"));
		FindInAllFieldsL(_L("Kick"),1,ETrue);
		FindInAllFieldsL(_L("High Res Solutions"),1,ETrue);
		FindInAllFieldsL(_L("Software Solutions"),2,ETrue);
		FindInAllFieldsL(_L("s"),4,ETrue);
		}
	//end of Async tests for defect where identity fields were not searched when using KUidMatchInAllFields

	//the following tests were not part of the defect but included as regression testing
	iTest.Next(_L("FINDING CONTACTS BY WORK PHONE NUMBER FROM   PHONE FIELD"));
		{ 
		iLog->LogLine(_L("\r\nFINDING CONTACTS BY WORK PHONE NUMBER FROM   PHONE FIELD"));
		FindPhoneNumberL(_L("015672354658"),2);
		FindPhoneNumberL(_L("017652354658"),1);
		FindPhoneNumberL(_L("999999999999"),0);
		}
	iTest.Next(_L("FINDING CONTACTS BY  HOME PHONE NUMBER FROM PHONE FIELD"));
		{
		iLog->LogLine(_L("\r\nFINDING CONTACTS BY HOME PHONE NUMBER FROM   PHONE FIELD"));
		FindPhoneNumberL(_L("01617246900"),2);
		FindPhoneNumberL(_L("01712345678"),1);
		FindPhoneNumberL(_L("999999999999"),0);
		}
	iTest.Next(_L("FINDING CONTACTS BY  WORK MOBILE  NUMBER FROM PHONE FIELD"));
		{
		iLog->LogLine(_L("\r\nFINDING CONTACTS BY WORK MOBILE NUMBER FROM   PHONE FIELD"));
		FindPhoneNumberL(_L("077962345100"),1);
		FindPhoneNumberL(_L("077962345101"),1);
		FindPhoneNumberL(_L("07896543210"),1);
		FindPhoneNumberL(_L("999999999999"),0);
		}
	iTest.Next(_L("FINDING CONTACTS BY HOME MOBILE  NUMBER FROM  PHONE FIELD"));
		{
		iLog->LogLine(_L("\r\nFINDING CONTACTS BY HOME MOBILE NUMBER FROM   PHONE FIELD"));
		FindPhoneNumberL(_L("07234567891"),1);
		FindPhoneNumberL(_L("07785674301"),1);
		FindPhoneNumberL(_L("07785674300"),1);
		FindPhoneNumberL(_L("999999999999"),0);
		}
	iTest.Next(_L("FINDING CONTACTS BY PHONE NUMBER FROM  ALL FIELDS"));
		{
		iLog->LogLine(_L("\r\nFINDING CONTACTS BY PHONE NUMBER FROM  ALL FIELDS"));
		FindInAllFieldsL(_L("015672354658"),2);
		FindInAllFieldsL(_L("01617246900"),2);
		FindInAllFieldsL(_L("017652354658"),1);
		FindInAllFieldsL(_L("01712345678"),1);
		FindInAllFieldsL(_L("999999999999"),0);
		}
	iTest.Next(_L("FINDING CONTACTS BY EMAIL FROM  EMAIL FIELD"));
		{
		iLog->LogLine(_L("\r\nFINDING CONTACTS BY EMAIL FROM  EMAIL FIELD"));
		FindEmailAddressL(_L("@@"),0);
		FindEmailAddressL(_L("aj1@software_solutions.com"),1);
		FindEmailAddressL(_L("software_solutions.com"),2);
		FindEmailAddressL(_L("soft"),3);
		FindEmailAddressL(_L("@"),4);
		}
	iTest.Next(_L("FINDING CONTACTS BY EMAIL FROM  ALL FIELDS"));
		{
		iLog->LogLine(_L("\r\nFINDING CONTACTS BY EMAIL FROM  ALL FIELDS"));
		FindInAllFieldsL(_L("@@"),0);
		FindInAllFieldsL(_L("aj1@software_solutions.com"),1);
		FindInAllFieldsL(_L("software_solutions.com"),2);
		FindInAllFieldsL(_L("soft"),3);
		FindInAllFieldsL(_L("@"),4);
		}
	iTest.Next(_L("FINDING CONTACTS BY NOTES FROM NOTES FIELDS"));
		{
		iLog->LogLine(_L("\r\nFINDING CONTACTS BY NOTES FROM  NOTES FIELDS"));
		FindNotesL(_L("dummy"),4);
		FindNotesL(_L("dummy"),4, ETrue);
		FindNotesL(_L("nonexistant"),0);
		FindNotesL(_L("nonexistant"),0, ETrue);
		}
	iTest.Next(_L("FINDING NOTES FROM  ALL FIELDS"));
		{
		iLog->LogLine(_L("\r\nFINDING NOTES FROM  ALL FIELDS"));
		FindInAllFieldsL(_L("dummy"),4);
		FindInAllFieldsL(_L("dummy"),4, ETrue);
		FindInAllFieldsL(_L("nonexistant"),0);
		FindInAllFieldsL(_L("nonexistant"),0, ETrue);
		}
	iTest.End();
	}

void CFindTest::AddContactL(const TDesC& aFirstName,const TDesC& aLastName,const TDesC& aCompanyName,
													const TDesC& aWorkPhone,const TDesC& aHomePhone,const TDesC& aWorkMobile,
													const TDesC& aHomeMobile,const TDesC& aWorkEmail)
	{
	iLog->LogLine(_L("\r\nAdding Contact "));
	iLog->LogLine(_L("Name: %S %S"),&aFirstName,&aLastName);
	iLog->LogLine(_L("Company: %S "),&aCompanyName);
	iLog->LogLine(_L("WorkPhone: %S "),&aWorkPhone);
	iLog->LogLine(_L("WorkMob: %S"),&aWorkMobile);
	iLog->LogLine(_L("HomePhone: %S"),&aHomePhone);
	iLog->LogLine(_L("HomeMob: %S"),&aHomeMobile);
	iLog->LogLine(_L("Email: %S "),&aWorkEmail);

	CTestContact* contact=CTestContact::NewLC(*iTemplate);
	
	contact->SetFirstNameL(aFirstName);
	contact->SetLastNameL(aLastName);
	contact->SetCompanyNameL(aCompanyName);
	contact->SetWorkPhoneL(aWorkPhone);
	contact->SetHomePhoneL(aHomePhone);
	contact->SetWorkMobileL(aWorkMobile);
	contact->SetHomeMobileL(aHomeMobile);
	contact->SetWorkEmailAddressL(aWorkEmail);
	contact->SetNotesL(KDummyData);	
	iDb->AddNewContactL(contact->ContactItem());
	CleanupStack::PopAndDestroy(1);  //contact.

	}

void CFindTest::AddContactCompactL(const TDesC& aFirstName,const TDesC& aLastName,const TDesC& aCompanyName,
																	  const TDesC& aWorkPhone,const TDesC& aHomePhone,const TDesC& aWorkMobile,
																	  const TDesC& aHomeMobile,const TDesC& aWorkEmail)
	{
	AddContactL(aFirstName,aLastName,aCompanyName,aWorkPhone,aHomePhone,aWorkMobile,aHomeMobile,aWorkEmail);
	iDb->CompactL();
	}

void CFindTest::FindNameInFirstNameFieldL(const TDesC& aTextToFind, const TInt aNumExpectedFinds,TBool aAsync)
	{
	iLog->LogLine(_L("Find %S in Given name field"),&aTextToFind);	
	CContactItemFieldDef* def=new(ELeave) CContactItemFieldDef();
	CleanupStack::PushL(def);
	def->AppendL(KUidContactFieldGivenName);
	if(!aAsync)
		{
		DoFindL(aTextToFind,*def,aNumExpectedFinds);
		}
	else
		{
		DoFindAsyncL(aTextToFind,*def,aNumExpectedFinds);
		}
	CleanupStack::PopAndDestroy(); // def.
	}

void CFindTest::FindNameInLastNameFieldL(const TDesC& aTextToFind, const TInt aNumExpectedFinds,TBool aAsync)
	{
	iLog->LogLine(_L("Find %S in Family name field"),&aTextToFind);	
	CContactItemFieldDef* def=new(ELeave) CContactItemFieldDef();
	CleanupStack::PushL(def);
	def->AppendL(KUidContactFieldFamilyName);
	if(!aAsync)
		{
		DoFindL(aTextToFind,*def,aNumExpectedFinds);
		}	
	else
		{
		DoFindAsyncL(aTextToFind,*def,aNumExpectedFinds);
		}
	CleanupStack::PopAndDestroy(); // def.	
	}

void CFindTest::FindInAllFieldsL(const TDesC& aTextToFind, const TInt aNumExpectedFinds,TBool aAsync)
	{
	iLog->LogLine(_L("Find %S in All fields"),&aTextToFind);	
	CContactItemFieldDef* def=new(ELeave) CContactItemFieldDef();
	CleanupStack::PushL(def);
	def->AppendL(KUidContactFieldMatchAll);
	if(!aAsync)
		{
		DoFindL(aTextToFind,*def,aNumExpectedFinds);
		}
	else
		{
		DoFindAsyncL(aTextToFind,*def,aNumExpectedFinds);
		}
	CleanupStack::PopAndDestroy(); // def.
	}

void CFindTest::FindInCompanyFieldL(const TDesC& aTextToFind, const TInt aNumExpectedFinds,TBool aAsync)
	{
	iLog->LogLine(_L("Find %S in Company name field"),&aTextToFind);	
	CContactItemFieldDef* def=new(ELeave) CContactItemFieldDef();
	CleanupStack::PushL(def);
	def->AppendL(KUidContactFieldCompanyName);
	if(!aAsync)
		{
		DoFindL(aTextToFind,*def,aNumExpectedFinds);
		}
	else
		{
		DoFindAsyncL(aTextToFind,*def,aNumExpectedFinds);
		}
	CleanupStack::PopAndDestroy(); // def.
	}

void CFindTest::FindPhoneNumberL(const TDesC& aTextToFind, const TInt aNumExpectedFinds,TBool aAsync)
	{
	iLog->LogLine(_L("Find %S in Phone fields"),&aTextToFind);	
	CContactItemFieldDef* def=new(ELeave) CContactItemFieldDef();
	CleanupStack::PushL(def);
	def->AppendL(KUidContactFieldPhoneNumber);
	if(!aAsync)
		{
		DoFindL(aTextToFind,*def,aNumExpectedFinds);
		}
	else
		{
		DoFindAsyncL(aTextToFind,*def,aNumExpectedFinds);
		}
	CleanupStack::PopAndDestroy(); // def.
	}

void CFindTest::FindEmailAddressL(const TDesC& aTextToFind, const TInt aNumExpectedFinds,TBool aAsync)
	{
	iLog->LogLine(_L("Find %S in Email fields"),&aTextToFind);	
	CContactItemFieldDef* def=new(ELeave) CContactItemFieldDef();
	CleanupStack::PushL(def);
	def->AppendL(KUidContactFieldEMail);
	if(!aAsync)
		{
		DoFindL(aTextToFind,*def,aNumExpectedFinds);
		}
	else
		{
		DoFindAsyncL(aTextToFind,*def,aNumExpectedFinds);
		}
	CleanupStack::PopAndDestroy(); // def.
	}
	
void CFindTest::FindNotesL(const TDesC& aTextToFind, const TInt aNumExpectedFinds,TBool aAsync)
	{
	iLog->LogLine(_L("Find %S in Email fields"),&aTextToFind);	
	CContactItemFieldDef* def=new(ELeave) CContactItemFieldDef();
	CleanupStack::PushL(def);
	def->AppendL(KUidContactFieldNote);
	
	if(!aAsync)
		{	
	    DoFindL(aTextToFind,*def,aNumExpectedFinds);
		}
	else
		{
		DoFindAsyncL(aTextToFind,*def,aNumExpectedFinds);
		}
	CleanupStack::PopAndDestroy(); // def.
	}	

void CFindTest::DoFindL(const TDesC& aTextToFind,const CContactItemFieldDef& aFieldDef, const TInt aNumExpectedFinds)
	{
	CContactIdArray* matchList=iDb->FindLC(aTextToFind,&aFieldDef);

	const TInt numIds=matchList->Count();
	iLog->LogLine(_L("Matched %d contact(s) of %d expected"),numIds,aNumExpectedFinds);
	if(numIds!=aNumExpectedFinds)
		{
		iLog->LogLine(_L("%S FAILED!"),&KTestName);
		}
	iTest(numIds==aNumExpectedFinds);
	CleanupStack::PopAndDestroy(); // matchList.
	}

void CFindTest::DoFindAsyncL(const TDesC& aTextToFind,const CContactItemFieldDef& aFieldDef, const TInt aNumExpectedFinds)
	{
	iIdleFinder=iDb->FindAsyncL(aTextToFind,&aFieldDef,this);
	CleanupStack::PushL(iIdleFinder); // Handle on cleanup stack because object is very temporary.
	CActiveScheduler::Start();

	CContactIdArray* matchList=iIdleFinder->TakeContactIds();
	CleanupStack::PushL(matchList);
	const TInt numIds=matchList->Count();
	iLog->LogLine(_L("Matched %d contact(s) of %d expected"),numIds,aNumExpectedFinds);
	if(numIds!=aNumExpectedFinds)
		{
		iLog->LogLine(_L("%S FAILED!"),&KTestName);
		}
	iTest(numIds==aNumExpectedFinds);
	CleanupStack::PopAndDestroy(2); // matchList.  iIdleFinder.
	}

void CFindTest::IdleFindCallback()
	{
	if (iIdleFinder->IsComplete())
		{
		CActiveScheduler::Stop();
		}
	}

//
// Main.
//

GLDEF_C TInt E32Main()
	{
	RDebug::Print(_L("t_cntfindall started"));
	__UHEAP_MARK;
	
	CActiveScheduler* sheduler=new CActiveScheduler;
	if (sheduler)
		{
		CActiveScheduler::Install(sheduler);
		CTrapCleanup* cleanup=CTrapCleanup::New();
		if (cleanup)
			{
			CFindTest* findtest=new CFindTest();//::NewL();
			if (findtest)
				{
				TRAPD(err, findtest->ConstructL();	findtest->DoTestsL(););
				RDebug::Print(_L("t_cntFindAll finish with %d error"),err);
				delete findtest;
				}
			delete cleanup;
			}
		delete sheduler;
		}
	__UHEAP_MARKEND;
	return KErrNone;
    }
