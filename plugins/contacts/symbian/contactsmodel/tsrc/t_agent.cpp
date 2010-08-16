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
*
*/


#include <e32test.h>
#include "t_rndutils.h"
#include "t_agent.h"
#include <cntitem.h>
#include <cntfldst.h>
#include <cntdef.h>
_LIT(KAgentExported, "C:\\Agentcontact.vcf");

_LIT(KTestName,"T_AGENT");


_LIT(KDatabaseFileName,"C:T_AGENT");
_LIT(KMyfile,"Z:\\ImportContacts.vcf"); 
_LIT(KAgentTemp,"C:\\ExportedContact.vcf");
_LIT(KAgentexpected,"Z:\\Expectedcontact.vcf");


CCntTest* CntTest=NULL;
LOCAL_D RTest test(KTestName);

CAgentTest* CAgentTest::NewL(CCntTest& aTestLibrary)
	{
	CAgentTest* self = new(ELeave) CAgentTest(aTestLibrary);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CAgentTest::~CAgentTest()
	{
	iTest.CloseDatabase();
	delete iIdArray;
	}


void CAgentTest::RunTestsL()
	{
    AgentExportL();
	
	test.Next(_L("Check if Agent cards are properly imported,single and multiple agents"));

	ImportAgentToEmptyDatabaseL();
	
	test.Next(_L("Check if Agent cards are merged with main contacts in database."));

	ImportAgentForMergeL();
	
	test.Next(_L("MainItems should be updated by incoming vCard having Agents."));

	ImportAgentMiscL();

	test.Next(_L("MainItems should be exported even if Agents are deleted."));

	ExportWithDeletedAgentL();
	
	test.Next(_L("Multiple  agents should be exported."));

	MultipleAgentExportL();

 	}

CAgentTest::CAgentTest(CCntTest& aTestLibrary) : iTest(aTestLibrary)
	{
	}

void CAgentTest::ConstructL()
	{
	iDb=iTest.CreateDatabaseL();
	iIdArray = CContactIdArray::NewL();
	}

/** Import contacts from the vCard file specified by aFileName */
void CAgentTest::ImportContactsL(const TDesC& aFileName)
	{
	RFileReadStream vcard;
	User::LeaveIfError(vcard.Open(iTest.Fs(), aFileName, EFileRead));
	CleanupClosePushL(vcard);

	TBool success=EFalse;
	CArrayPtr<CContactItem>* contactItems=iDb->ImportContactsL(TUid::Uid(KUidVCardConvDefaultImpl), vcard, success, CContactDatabase::ETTFormat);
	CleanupStack::PopAndDestroy(&vcard);
	       
	CleanupStack::PushL(TCleanupItem(CleanUpResetAndDestroy,contactItems));
	iIdArray->Reset();
	TContactItemId contactId;
 	for(TInt loop = 0;loop < contactItems->Count();++loop)
		{
		contactId = (*contactItems)[loop]->Id();
		iIdArray->AddL(contactId);		
		}
	CleanupStack::PopAndDestroy(contactItems);	
	}

/** Export contact items specified by aIds to aFileName */
void CAgentTest::ExportContactsL(const TDesC& aFileName)
	{
	RFile outfile;
	outfile.Replace(iTest.Fs(),aFileName,EFileWrite);
	CleanupClosePushL(outfile);
	RFileWriteStream writeStream(outfile);
	CleanupClosePushL(writeStream);
	
	TUid uid;
	uid.iUid=KUidVCardConvDefaultImpl;
	iDb->ExportSelectedContactsL(uid,*iIdArray,writeStream,CContactDatabase::EDefault);

	writeStream.CommitL();
	CleanupStack::PopAndDestroy(2); //writeStream.Close(), outfile.Close()
	}

/** Imports contacts from a vCard present in descriptor*/
void CAgentTest::ImportContactsFromDescriptorL(const TDesC8& aVCard)
	{
	RDesReadStream vcard(aVCard);
	CleanupClosePushL(vcard);
	CArrayPtr<CContactItem>* contactItems;
	TBool success = EFalse;
	contactItems = iDb->ImportContactsL(TUid::Uid(KUidVCardConvDefaultImpl), vcard, success, CContactDatabase::ETTFormat);
	CleanupStack::PushL(TCleanupItem(CleanUpResetAndDestroy, contactItems));
	iIdArray->Reset();
	TContactItemId contactId;
	for(TInt loop = 0;loop < contactItems->Count();++loop)
		{
		contactId = (*contactItems)[loop]->Id();
		iIdArray->AddL(contactId);		
		}
	CleanupStack::PopAndDestroy(contactItems);
	CleanupStack::PopAndDestroy(&vcard);
	}

	
void CAgentTest::CompareFilesL(const TDesC& aOutputFile,const TDesC& aCompareFile)
	{
	RFs fsSession;
	User::LeaveIfError(fsSession.Connect());
	RFile file1,file2;
	TInt err=file1.Open(fsSession, aOutputFile, EFileRead);
	if (err!=KErrNone)
		{
		User::Leave(err);	
		}
	
	CleanupClosePushL(file1);
	err=file2.Open(fsSession, aCompareFile, EFileRead);
	
	if (err!=KErrNone)
		{
		User::Leave(err);	
		}

	
	CleanupClosePushL(file2);
	RFileReadStream stream1(file1);
	RFileReadStream stream2(file2);
	CleanupClosePushL(stream1);
	CleanupClosePushL(stream2);
	TBuf8<0x80> bufO,bufC;
	TInt line=1;
	do {
		TRAP(err,stream1.ReadL(bufO,TChar(0xa)));
		if (err==KErrNone || err==KErrEof)
		{
		TRAP(err,stream2.ReadL(bufC,TChar(0xa)));	
		}
		if (err!=KErrNone && err!=KErrEof)
		{
		User::Leave(err);	
		}
		
		CompareLines(bufO, bufC);
		++line;
		} while (err!=KErrEof);
	CleanupStack::PopAndDestroy(4,&file1);
	}
	
TBool CAgentTest::CompareLines(const TDesC8& aLine1, const TDesC8& aLine2)
	{
	_LIT8(Krev,"REV");
	
	TBool ret=!aLine1.Compare(aLine2);
	if (!ret)
		{
		TBufC8<2> firstchar(aLine1.Left(2));
		if(!firstchar.Compare(Krev()))
			{
			return ETrue;
			}
		}
	return ret;
	}	
	
void CleanUpResetAndDestroy(TAny *aArray)
	{
	if (aArray)
		{
		CArrayPtr<CContactItem>* array=(CArrayPtr<CContactItem>*)aArray;
		array->ResetAndDestroy();
		delete array;
		}
	}


void CAgentTest::AgentExportL()	
	{  
     test.Next(_L("First it Imports the contacts from the vcard"));

    
    ImportContactsL(KMyfile);
    
    test.Next(_L("It Exports the main contact which conatains the agent contact"));

    
    ExportContactsL(KAgentTemp);
    
    test.Next(_L("It compares the exported conatcts with expected vcf file"));

 	 		   
    CompareFilesL(KAgentTemp,KAgentexpected);
 
	}

/**
@SYMTestCaseID PIM-AGENT-0001
@SYMDEF DEF098709
@SYMTestType UT
@SYMTestPriority High
@SYMTestCaseDesc Check if Agent cards are properly imported.Single and Multiple.

@SYMTestActions
1.Import vCards having Main Contacts and Agent cards
2.Import vCards having main contact having multiple agent cards.

@SYMTestExpectedResults For the above tests:
1.AgentCard should merge with the main contact item in database.
2.All Agents should be properly imported, creating new agents and adding them to Main contact items.
3.No leaves or panics should occur
*/	
void CAgentTest::ImportAgentToEmptyDatabaseL()
	{
	_LIT(KAgentFirstName,"AgentSmith");	
	iDb = iTest.CreateDatabaseL();
	//vCard has contact A, contact B. contact A included in contact B as Agent.
	//Agent Contact in B has different Name property. so after merge, contact A should
	//have the name present in Agent A. (look at the vCard in header file).
	ImportContactsFromDescriptorL(KAgentvCard());
	//Only 2 contacts should have been imported, as Agent will be merged.
	test(iIdArray->Count() == 2);
	
	//check if name has changed for contact A
	CContactItem* mainItem = iDb->ReadContactLC((*iIdArray)[0]);
	test(FieldText(*mainItem, KUidContactFieldFamilyName) == KAgentFirstName);
	CleanupStack::PopAndDestroy();

	//Import vCard has 2 Main Contacts and 3 Agents. No contact is occurring twice,
	//so all should be added as new contacts, 5 in total.
	iDb = iTest.CreateDatabaseL();
	ImportContactsFromDescriptorL(KMultipleAgentvCard());
	test(iIdArray->Count() == 5);

	//check that 1st Main contact has 1 agent.
	//extracting 2nd one in array as agents would be added prior to the main contact.
	test(GetAgentCountL((*iIdArray)[1]) == 1);

	//2nd Main contact should have 2 agents.
	//extracting 5th one in array as agents would be added prior to the main contact.
	test(GetAgentCountL((*iIdArray)[4]) == 2);
	}

/**
@SYMTestCaseID PIM-AGENT-0002 
@SYMDEF DEF098709
@SYMTestType UT
@SYMTestPriority High
@SYMTestCaseDesc Check if Agent cards are merged with main contacts in database.

@SYMTestActions
1.Create contact items and add contact agents to them.
2.Export Main contact items.
3.Change properties of Agent contact items in database.
4.Import the exported vCard.

@SYMTestExpectedResults For the above tests:
1.vCard being imported should properly update the existing contact/Agent items
2.Updated contacts should be restored with old values.
3.No leaves or panics should occur
*/	
void CAgentTest::ImportAgentForMergeL()
	{
	iDb = iTest.CreateDatabaseL();
	//Adding 4 contacts (A,B,C,D)to database
	AddContactItemsL(4);
	//Make A an agent of B 
	MakeAgentL((*iIdArray)[1], (*iIdArray)[0]);

	//make C an Agent of D
	MakeAgentL((*iIdArray)[3], (*iIdArray)[2]);
	
	//storing Id's of agents A and C and removing them from array of Ids to be exported.
	TContactItemId idAgentA = (*iIdArray)[0];
	iIdArray->Remove(0);
	TContactItemId idAgentC = (*iIdArray)[1];
	iIdArray->Remove(1);

	//export Maincontacts B & D.
	ExportContactsL(KAgentTemp);
	
	_LIT(Kfn,"NewName");
	CContactItem* agentItem = NULL;

	//change familyname property of Agent contact A
	TBuf<20> agentAText;
	agentItem = iDb->OpenContactL(idAgentA);
	CleanupStack::PushL(agentItem);
	//storing name to cross check after import
	agentAText.Append(FieldText(*agentItem, KUidContactFieldFamilyName));
	TUid aid = {0};
	SetNameL(*agentItem, KUidContactFieldFamilyName, aid, Kfn, 0);
	iDb->CommitContactL(*agentItem);
	CleanupStack::PopAndDestroy();
	
	//change familyname property of Agent contact C
	TBuf<20> agentCText;
	agentItem = iDb->OpenContactL(idAgentC);
	CleanupStack::PushL(agentItem);
	//storing name to cross check after import
	agentCText.Append(FieldText(*agentItem, KUidContactFieldFamilyName));
	SetNameL(*agentItem, KUidContactFieldFamilyName, aid, Kfn, 0);
	iDb->CommitContactL(*agentItem);
	CleanupStack::PopAndDestroy();

	//All four contacts exist in database, 2 main and 2 agents so iIdArray will have only 2 Ids.
	ImportContactsL(KAgentTemp);
	
	//now check that names should have been restored to what it was before export
	agentItem = iDb->ReadContactLC(idAgentA);
	test(FieldText(*agentItem, KUidContactFieldFamilyName) == agentAText);
	CleanupStack::PopAndDestroy();

	agentItem = iDb->ReadContactLC(idAgentC);
	test(FieldText(*agentItem, KUidContactFieldFamilyName) == agentCText);
	CleanupStack::PopAndDestroy();	
	}

/**
@SYMTestCaseID PIM-AGENT-0003
@SYMDEF DEF098709
@SYMTestType UT
@SYMTestPriority High
@SYMTestCaseDesc MainItems should be updated by incoming vCard having Agents.

@SYMTestActions
1.Create contact items and add contact agents to them.
2.Export Main contact items.
3.Delete agent fields from Main contact items.
4.Import the exported vCard.

@SYMTestExpectedResults For the above tests:
1.vCard being imported should properly update the existing contact items
2.Updated contacts should be restored with old values.
3.Agents fields should be added to main contact items.
4.No leaves or panics should occur
*/	

void CAgentTest::ImportAgentMiscL()
	{
	iDb = iTest.CreateDatabaseL();
	iIdArray->Reset();
	
	//Adding 4 contacts (A,B,C,D)to database
	AddContactItemsL(4);
	//Make A an agent of B 
	MakeAgentL((*iIdArray)[1], (*iIdArray)[0]);
	//make C an Agent of D
	MakeAgentL((*iIdArray)[3], (*iIdArray)[2]);

	//storing Id's of agents A and C and removing them from array of Ids to be exported.
	TContactItemId idAgentA = (*iIdArray)[0];
	iIdArray->Remove(0);
	TContactItemId idAgentC = (*iIdArray)[1];
	iIdArray->Remove(1);

	//export Maincontacts B & D.
	ExportContactsL(KAgentTemp);

	//remove Agent fields from main contact items B and D.
	RemoveFieldFromContactL((*iIdArray)[0], KUidContactFieldVCardMapAGENT);
	test(GetAgentCountL((*iIdArray)[0]) == 0);
	
	RemoveFieldFromContactL((*iIdArray)[1], KUidContactFieldVCardMapAGENT);	
	test(GetAgentCountL((*iIdArray)[1]) == 0);

	//All four contacts exist in database,Merge will be done for all of them.
	//2 main and 2 agents so iIdArray will have only 2 Ids.
	//and main contacts should have Agent fields.
	ImportContactsL(KAgentTemp);
	
	test(GetAgentCountL((*iIdArray)[0]) == 1);	
	test(GetAgentCountL((*iIdArray)[1]) == 1);	
			
	}

/**
@SYMTestCaseID PIM-AGENT-0004
@SYMDEF DEF102255
@SYMTestType UT
@SYMTestPriority High
@SYMTestCaseDesc MainItems should be exported even if Agents are deleted.

@SYMTestActions
1.Create contact items and add contact agents to them.
2.Delete agent contacts.
3.Export Main contact items.

@SYMTestExpectedResults For the above tests:
1.Contact items and agents should be created.
2.Agent deletion should be successful.
3.Main contact items without agents should be exported.
4.No leaves or panics should occur.
*/	
void CAgentTest::ExportWithDeletedAgentL()
{
	iDb = iTest.CreateDatabaseL();
	//Adding 4 contacts (A,B,C,D)to database
	AddContactItemsL(4);
	
	//Make A an agent of B 
	MakeAgentL((*iIdArray)[1], (*iIdArray)[0]);
	
	//Make C an Agent of D
	MakeAgentL((*iIdArray)[3], (*iIdArray)[2]);
	
	//Delete agents A & C
	iDb->DeleteContactL((*iIdArray)[0]);
	iDb->DeleteContactL((*iIdArray)[2]);
	
	//Removing Id's of agents A & C from array of Ids to be exported
	iIdArray->Remove(0);
	iIdArray->Remove(1);	
	
	//Export main contacts B & D
	ExportContactsL(KAgentTemp);
}

/**
@SYMTestCaseID PIM-AGENT-0005
@SYMDEF PDEF102693
@SYMTestType UT
@SYMTestPriority High
@SYMTestCaseDesc Multiple agents must be Exported properly.

@SYMTestActions
1.Create contact item with multiple agents.
2.Export contact items..
3.Import  contact items.
4.Check the agents.

@SYMTestExpectedResults For the above tests:
1. agents should be exported properly.
*/	
void CAgentTest::MultipleAgentExportL()
{
	CTestRegister* TempFiles = CTestRegister::NewLC();
	TempFiles->RegisterL(KAgentExported);

	iDb = iTest.CreateDatabaseL();
	ImportContactsFromDescriptorL(KMultipleAgentvCard());
	_LIT(KAgent1FirstName,"Oracle");
	_LIT(KAgent2FirstName,"Architect");

	//remove the other contact item as the imported vacrd contains two contact items.
	iIdArray->Remove(0);
	iIdArray->Remove(0);
	iIdArray->Remove(0);
	iIdArray->Remove(0);
	
	ExportContactsL(KAgentExported);

	//clean the database 
	iDb = iTest.CreateDatabaseL();
	//import contacts from the exported file.	
	ImportContactsL(KAgentExported);
	//check  the first names of the 
	CContactItem* mainItem = iDb->ReadContactLC((*iIdArray)[0]);
	test(FieldText(*mainItem, KUidContactFieldFamilyName) == KAgent1FirstName);


	mainItem = iDb->ReadContactLC((*iIdArray)[1]);
	test(FieldText(*mainItem, KUidContactFieldFamilyName) == KAgent2FirstName);
	CleanupStack::PopAndDestroy(3);

}

/**Utility function to remove any specified contact field from a specified contact item*/
TBool CAgentTest::RemoveFieldFromContactL(TContactItemId aContactId, TUid aContactField)
	{
	CContactItem* mainItem = NULL;
	CContactItemFieldSet* fields = NULL;
	mainItem = iDb->OpenContactL(aContactId);
	CleanupStack::PushL(mainItem);
	fields = &mainItem->CardFields();
	TInt pos = 0;
	pos = fields->Find(aContactField);
	if (pos != KErrNotFound)
		{
		fields->Remove(pos);	
		}
	iDb->CommitContactL(*mainItem);
	CleanupStack::PopAndDestroy();	
	return pos;	
	}

/**Utility function for adding agent to a contact item*/
void CAgentTest::MakeAgentL(TContactItemId aMainContactId, TContactItemId aAgentId)
	{
	CContactItem* mainItem = NULL;
	CContactItemField* field = NULL;

	mainItem = iDb->OpenContactL(aMainContactId);
	CleanupStack::PushL(mainItem);
	field = CContactItemField::NewLC(KStorageTypeContactItemId, KUidContactFieldVCardMapAGENT );
	field->SetMapping(KUidContactFieldVCardMapAGENT);
	field->AgentStorage()->SetAgentId(aAgentId);
	mainItem->AddFieldL(*field);
	CleanupStack::Pop();

	iDb->CommitContactL(*mainItem);
	CleanupStack::PopAndDestroy();
	}

/**Utility function for finding number of agents in a contact item*/
TInt CAgentTest::GetAgentCountL(TContactItemId aContactId)
	{
	CContactItem* mainItem = NULL;
	mainItem = iDb->ReadContactL(aContactId);
	CleanupStack::PushL(mainItem);
	CContactItemFieldSet* fields = &mainItem->CardFields();
	TInt count = fields->Count();	
	TInt agentCount = 0;
	TInt i = 0;
	for(; i < count; ++i)
		{
		if( (*fields)[i].StorageType() == KStorageTypeContactItemId )
			{
			++agentCount;
			}
		}
	CleanupStack::PopAndDestroy();
	return agentCount;
	}

/**Utility function for Adding any number of contact items
   with name and telephone fields.
*/
void CAgentTest::AddContactItemsL(TInt aCount)
	{
	iIdArray->Reset();
	CContactItem* templateCard = NULL;
	CContactCard* card = NULL;
	CContactItemFieldSet* fieldSet = NULL;
	
	for(TInt loop = 0; loop < aCount; ++loop)
		{
		TInt pos = KErrNotFound;
		TBuf<16> name;
		name.Format(_L("NAME%d"),loop);
		TBuf<16> tel;
		tel.Format(_L("123456%d"),loop);
		
		templateCard = iDb->ReadContactLC(iDb->TemplateId());
		card = CContactCard::NewLC(templateCard); 
	
		fieldSet = &card->CardFields();
		pos = fieldSet->Find(KUidContactFieldFamilyName);
		if (pos != KErrNotFound)
			{
			(*fieldSet)[pos].TextStorage()->SetTextL(name);		
			}
		pos = fieldSet->Find(KUidContactFieldPhoneNumber);
		
		if (pos != KErrNotFound)
			{
			(*fieldSet)[pos].TextStorage()->SetTextL(tel);	
			}
		
    	TContactItemId contactId = iDb->AddNewContactL(*card);
		iIdArray->AddL(contactId);
		CleanupStack::PopAndDestroy(2);
		}
	}

/**

@SYMTestCaseID     PIM-T-AGENT-0001

*/

LOCAL_C void DoTestsL()
	{
	test.Start(_L("@SYMTESTCaseID:PIM-T-AGENT-0001 Preparing tests"));

	CAgentTest* agentTest = CAgentTest::NewL(*CntTest);
	CleanupStack::PushL(agentTest);
	agentTest->RunTestsL();	
	CleanupStack::PopAndDestroy(agentTest);
	}

GLDEF_C TInt E32Main()
	{
    __UHEAP_MARK; 
    CntTest = new CCntTest();
	CntTest->ConstructL(test,KDatabaseFileName);
    TRAPD(err,DoTestsL());
	CntTest->EndTestLib(err);
	__UHEAP_MARKEND;	
	return KErrNone;
    }
