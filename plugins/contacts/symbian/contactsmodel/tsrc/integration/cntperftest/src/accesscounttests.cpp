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


/**
 @file
 @publishedAll
 @released
*/

#include <testexecutelog.h>
#include <cntfldst.h>
#include "clientserver.h"
#include "accesscounttests.h"

_LIT(KRun1,"DeleteAccessed");
_LIT(KRun2,"ForwardDeletion");
_LIT(KRun3,"ReverseDeletion");
_LIT(KRun4,"AgentGetSet");
_LIT(KRun5,"AgentAccess");
_LIT(KRun6,"AgentImportExport");

_LIT(KTest1,"Testing deletion of contacts with access count > 0...");
_LIT(KTest2,"Testing agent fields and forward deletion of items with those fields...");
_LIT(KTest3,"Testing agent fields and reverse deletion of items with those fields...");
_LIT(KTest4,"Agent retrieve and set test...");
_LIT(KTest5,"Agent access count test...");
_LIT(KTest6,"Agent import/export access count test...");

_LIT(KVcardFile,"c:\\ExportedContacts.vcf");
_LIT(KAgentValue,"Agent value: %d");

//Number of contacts
CAccessCountTests::CAccessCountTests(CCntPerfServer &aParent):CCntBaseStep(30, aParent)
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(SharedConstants::KAccessCountTests);
	}

void CAccessCountTests::PreTestL()
	{
	User::LeaveIfError( iParent->Fs().Connect() );
	}

TVerdict CAccessCountTests::doTestStepL()
	{
	
	__UHEAP_MARK;

	InitializeL();
	iIterate->Reset();
	
	const TDesC &run = ConfigSection();
	
	if( run == KRun1 )
		{
		INFO_PRINTF1(KTest1);
		AccessCountTestL( iIterate->NextL() );
		}
	else if( run == KRun2 )
		{
		INFO_PRINTF1(KTest2);
		ForwardAgentDeletionTestL( 10 );
		}
	else if( run == KRun3 )
		{
		INFO_PRINTF1(KTest3);
		ReverseAgentDeletionTestL( 10 );
		}
	else if( run == KRun4 )
		{
		INFO_PRINTF1(KTest4);
		AddGetAgentTestL(iIterate->NextL());
		}
	else if( run == KRun5 )
		{
		INFO_PRINTF1(KTest5);
		AgentAccessCountTestL();
		}
	else if( run == KRun6 )
		{
		INFO_PRINTF1(KTest6);
		AgentExportImportAccessTestL();
		}
	else
		{
		MissngTestPanic();
		}
	Cleanup();
	__UHEAP_MARKEND;

	
	return TestStepResult();
	}

/**
open acid, increase access count to 1, attempt to delete contact, check that it has
not been deleted, set access count to 0, delete, and check that it has been deleted.
*/
void CAccessCountTests::AccessCountTestL(const TContactItemId aCid)
	{
	TInt err = KErrNone;
	static const TInt KAccessCount = 1;//increase access count of contact by 1, 
										
	TInt i = 0;
	
	OpenL(aCid,*iViewAll);
	SetContactFieldsL(*iContactItem);
	for(i = 0; i < KAccessCount; ++i )
		{
		iContactItem->IncAccessCount();
		}
	CommitL();
	
	ReadL(aCid, *iViewAll);
	const TInt KInitialAccessCount = iContactItem->AccessCount();//facount
	const TInt KInitialFieldCount = iFields->Count(); //fcount
	const TInt KInitialEmptyFields = CountEmpty(*iFields);//fhcount
	Close();
	
	INTCOMPARE(  KInitialEmptyFields , ==,  0 , 0, 0 );
	INTCOMPARE(  KInitialFieldCount , >,  0 , 0, 0 );
	
	for(i = 0; i < KAccessCount; ++i )
		{
		iContactsDatabase->DeleteContactL(aCid);
		}
		
	ReadL(aCid,*iViewAll);
	const TInt KModifiedAccessCount = iContactItem->AccessCount();//dacount
	const TInt KModifiedFieldCount 	= iFields->Count();//dcount
	const TInt KModifiedEmptyFields = CountEmpty(*iFields);//dHcount
	Close();
	
	/*
	check that deleting a contact doesnt modify its access count
	check that deleting a contact with access count > 1, 
	deletes all fields associated with contact but not the contact item itself
	*/
	INTCOMPARE(  KModifiedAccessCount , ==,  KInitialAccessCount , 0, 0 ); 
	INTCOMPARE(  KInitialFieldCount , ==,  KModifiedFieldCount , 0, 0 );
	INTCOMPARE(  KModifiedEmptyFields , ==,  KInitialFieldCount , 0, 0 ); 
	INTCOMPARE(  KModifiedFieldCount , ==,  KModifiedEmptyFields , 0, 0 );
	
	OpenL(aCid,*iViewAll);
	for(i = 0; i < KAccessCount; ++i )
		{
		iContactItem->DecAccessCount();
		}
	const TInt lAcount = iContactItem->AccessCount();
	CommitL();
	
	INTCOMPARE( lAcount , ==,  0, 0, 0 );
	
	TRAP( err, ReadL(aCid, *iViewAll) );
	Close();
	
	INTCOMPARE(  err , ==,  KErrNotFound , 0, 0 );
	}

/*
test that contactitems can be delete successfull irregardless of the contents of their agent value
*/
void CAccessCountTests::ReverseAgentDeletionTestL(const TInt aNumContacts)
	{
	TContactItemId PreviousContactId = -1;
	TContactItemId CurrentContactId = iIterate->NextL();
	TInt i = 0;
	TInt err = KErrNone;
	
	for(; i < aNumContacts; ++i)
		{
		PreviousContactId = CurrentContactId;
		CurrentContactId = iIterate->NextL();
		//open contact item
		OpenL(CurrentContactId, *iViewAll);
		//add agent field and set to previous contact id
		AddAgentFieldL(PreviousContactId);
		CommitL();
		//print agent values useful for debugging
		INFO_PRINTF2(KAgentValue, GetAgentFieldL(CurrentContactId) );
		}

	//attempt to delete contacts starting from last contact to frist contact
	TBool DeleteSuccess = EFalse;
	for(i = aNumContacts ; i > 0 ; --i)
		{
		//attempt to delete contact
		iContactsDatabase->DeleteContactL(CurrentContactId);
		//remove contact id from iterator
		iIterate->RemoveL(CurrentContactId);
		//attempt to read deleted contact
		TRAP( err, ReadL(CurrentContactId, *iViewAll) );
		Close();
		//fail test if contact is still present
		DeleteSuccess = INTCOMPARE( err, ==, KErrNotFound, i, 0 );
		CurrentContactId = iIterate->PreviousL();
		if( ! DeleteSuccess )
			{
			break;
			}
		}
	//if test fails
	if( ! DeleteSuccess )
		{
		//remove rest of contacts from iterator
		for(; i > 0 ; --i)
			{
			iIterate->RemoveL(CurrentContactId);
			CurrentContactId = iIterate->PreviousL();
			}
		}
	}

/*
test that contactitems can be deleted successfully irregardless of the contents of their agent value
*/
void CAccessCountTests::ForwardAgentDeletionTestL(const TInt aNumContacts)
	{
	TBool DeleteSuccess = EFalse;
	TContactItemId PreviousContactId = -1;
	TContactItemId CurrentContactId = iIterate->NextL();
	TInt err = KErrNone;
	TInt i = 0;
	
	for(; i < aNumContacts; ++i)
		{
		PreviousContactId = CurrentContactId;
		CurrentContactId = iIterate->NextL();
		//open contact item
		OpenL(CurrentContactId, *iViewAll);
		//add agent field and set to previous contact id
		AddAgentFieldL(PreviousContactId);
		CommitL();
		//print agent values useful for debugging
		INFO_PRINTF2(KAgentValue, GetAgentFieldL(CurrentContactId) );
		}
	
	//move iterator to start first contact in list
	for( i = 0 ; i < aNumContacts; ++i )
		{
		CurrentContactId = iIterate->PreviousL();
		}
	
	//attempt to delete contacts starting from first contact to last contact
	for( i = 0 ; i < aNumContacts ; ++i )
		{
		//attempt to delete contact
		iContactsDatabase->DeleteContactL(CurrentContactId);
		//attempt to read deleted contact
		TRAP( err, ReadL(CurrentContactId, *iViewAll) );
		Close();
		//fail test if contact is still present
		DeleteSuccess = INTCOMPARE( err, ==, KErrNotFound , i, 0 );
		CurrentContactId = iIterate->NextL();
		if( ! DeleteSuccess )
			{
			break;
			}
		}
	//if test fails
	if(!DeleteSuccess)
		{
		//remove rest of contacts from iterator
		const TInt end = i + 1;
		for(i = 0 ; i < end ; ++i)
			{
			iIterate->RemoveL(CurrentContactId);
			CurrentContactId = iIterate->PreviousL();
			}
		CurrentContactId = iIterate->NextL();
		for(i = end ; i < aNumContacts ; ++i)
			{
			iIterate->RemoveL(CurrentContactId);
			CurrentContactId = iIterate->CurrentL();
			}
		}
	}

/**
* Open aCid, add agent field, set value of agent field, and get value of agent.
* Check this is correct, repeat this for minint, 0, 1, Maxint. Setting the 
* agent id should only accept values > 0, all other values are ignored.
*/
void CAccessCountTests::AddGetAgentTestL(const TContactItemId aCid)
	{
	TContactItemId retrievedAgentId = -1;
	const TInt KInitialAgentId = 5;
	
	OpenL(aCid,*iViewAll);
	AddAgentFieldL( KInitialAgentId );
	CommitL();
	
	TRAPD( error, SetAgentFieldL( aCid, KMinTInt ) );
	INTCOMPARE(  error , ==,  KErrNone , 0, 0 );
	retrievedAgentId = GetAgentFieldL( aCid );
	INTCOMPARE(  retrievedAgentId , ==,  KInitialAgentId , 0, 0 );
	
	TRAP( error, SetAgentFieldL( aCid, -1 ) );
	INTCOMPARE(  error , ==,  KErrNone , 0, 0 );
	retrievedAgentId = GetAgentFieldL( aCid );
	INTCOMPARE(  retrievedAgentId , ==,  KInitialAgentId , 0, 0 );
	
	TRAP( error, SetAgentFieldL( aCid, 0 ) );
	INTCOMPARE(  error , ==,  KErrNone , 0, 0 );
	retrievedAgentId = GetAgentFieldL( aCid );
	INTCOMPARE(  retrievedAgentId , ==,  KInitialAgentId , 0, 0 );

	TRAP( error, SetAgentFieldL( aCid, 1 ) );
	INTCOMPARE(  error , ==,  KErrNone , 0, 0 );
	retrievedAgentId = GetAgentFieldL( aCid );
	INTCOMPARE(  retrievedAgentId , ==,  1 , 0, 0 );
	
	TRAP( error, SetAgentFieldL( aCid, KMaxTInt ) );
	INTCOMPARE(  error , ==,  KErrNone , 0, 0 );
	retrievedAgentId = GetAgentFieldL( aCid );
	INTCOMPARE(  retrievedAgentId , ==,  KMaxTInt , 0, 0 );
	}

/**
add agent field to contact b, set value of agent in b to id of contact a.
check that agent field is retrieved/stored correctly, check that access count is
not modified for either contact
*/	
void CAccessCountTests::AgentAccessCountTestL()
	{
	TContactItemId RetrievedAgentId = -1;
	TContactItemId PreviousContactId = iIterate->NextL();//contact a
	TContactItemId CurrentContactId = iIterate->NextL();//contact b
	
	//open current contact
	OpenL(CurrentContactId,*iViewAll);
	//add new agent field, set to previous contact id
	AddAgentFieldL(PreviousContactId);
	CommitL();
	
	//get agent id store for current contact
	RetrievedAgentId = GetAgentFieldL(CurrentContactId);
	
	//get access count of current contact
	ReadL(CurrentContactId, *iViewAll);
	const TInt KCurrentContactAccessCount = iContactItem->AccessCount();
	Close();
	
	//get access count of previous contact
	ReadL(PreviousContactId, *iViewAll);
	const TInt KPreviousContactAccessCount = iContactItem->AccessCount();
	Close();
	
	//check that retrieved agent id equals previous contact id
	INTCOMPARE( RetrievedAgentId , ==,  PreviousContactId, 0, 0 );
	//check that changing the agent field values doesnt not affect the access count
	//of involved contacts
	INTCOMPARE( KCurrentContactAccessCount , ==,  0, 0, 0 ); 
	INTCOMPARE( KPreviousContactAccessCount , ==,  0, 0, 0 );
	}

/**
add agent field to contact b. set value of agent to contact a.
export contacts a & b. check access count for a & b(should be 0), 
check value of agent(should equal b) open contact b, delete all fields.
commit. import contacts a and b. check that agent field is not imported.
*/
void CAccessCountTests::AgentExportImportAccessTestL()
	{
	const TContactItemId KFirstContactId = iIterate->NextL();//contact a
	const TContactItemId KSecondContactId = iIterate->NextL();//contact b
	
	//open contact a and populate all fields
	OpenL(KFirstContactId, *iViewAll);
	SetContactFieldsL(*iContactItem);
	CommitL();
	
	//open contact b
	OpenL(KSecondContactId,*iViewAll);
	//populate all fields
	SetContactFieldsL(*iContactItem);
	//add contact a id as an agent field value
	AddAgentFieldL(KFirstContactId);
	CommitL();
	
	CContactIdArray *ArrayOfExportedContactIds = CContactIdArray::NewLC();
	//set contact a and b to be export
	ArrayOfExportedContactIds->AddL(KFirstContactId);
	ArrayOfExportedContactIds->AddL(KSecondContactId);
	
	//export contact a & b
	ExportVcardL(*ArrayOfExportedContactIds);
	
	const TInt KFirstContactAccessCountAfterExport = GetAccessCountL(KFirstContactId);
	const TInt KSecondContactAccessCountAfterExport = GetAccessCountL(KSecondContactId);
	const TInt KSecondContactAgentFieldAfterExport = GetAgentFieldL(KSecondContactId);
	CleanupStack::PopAndDestroy(ArrayOfExportedContactIds);
	
	//delete all fields from contact b
	OpenL(KSecondContactId,*iViewAll);
	iFields->Reset();
	CommitL();
	
	//import exported contacts
	ImportVcardL();
	
	const TInt KFirstContactAccessCountAfterImport = GetAccessCountL(KFirstContactId);
	const TInt KSecondContactAccessCountAfterImport = GetAccessCountL(KSecondContactId);
	const TContactItemId KSecondContactAgentFieldAfterImport = GetAgentFieldL(KSecondContactId);
	
	//importing & exporting contacts should not affect the access count
	INTCOMPARE( KFirstContactAccessCountAfterExport , ==,  0, 0, 0 ); 
	INTCOMPARE( KSecondContactAccessCountAfterExport , ==,  0, 0, 0 ); 
	INTCOMPARE( KFirstContactAccessCountAfterExport , ==,  KFirstContactAccessCountAfterImport, 0, 0 );
	INTCOMPARE( KSecondContactAccessCountAfterExport , ==,  KSecondContactAccessCountAfterImport, 0, 0 );
	
	//agent field values should not be exported/imported
	INTCOMPARE( KFirstContactId , ==,  KSecondContactAgentFieldAfterImport, 0, 0 ); 
	//exporting contacts should not affect the internal state of the contacts agent field
	INTCOMPARE( KSecondContactAgentFieldAfterExport , ==,  KFirstContactId, 0, 0 );
	
	iParent->Fs().Delete(KVcardFile);//cleanup of exported vcard file
	}
	
void CAccessCountTests::AddAgentFieldL(const TContactItemId aCid)
	{
	_LIT(KAgentName, "ImAnAgent");
	CContactItemField * field = CContactItemField::NewLC(KStorageTypeContactItemId, KUidContactFieldVCardMapAGENT );
	field->SetMapping(KUidContactFieldVCardMapAGENT);
	field->SetLabelL(KAgentName);
	field->AgentStorage()->SetAgentId(aCid);
	iFields->AddL(*field);
	CleanupStack::Pop(field);
	field = NULL;
	}

TContactItemId CAccessCountTests::GetAgentFieldL(const TContactItemId aCid)
	{
	ReadL(aCid, *iViewAll);
	const TInt count = iFields->Count();
	TContactItemId ret = KErrNotFound;
	TInt i = 0;
	for(; i < count; ++i)
		{
		if( (*iFields)[i].StorageType() == KStorageTypeContactItemId )
			{
			ret = (*iFields)[i].AgentStorage()->Value();
			break;
			}
		}
	Close();
	return ret;
	}

void CAccessCountTests::SetAgentFieldL(const TContactItemId aCid, const TContactItemId aVal)
	{
	
	TBool FoundAgentField = EFalse;
	OpenL(aCid, *iViewAll);
	const TInt count = iFields->Count();
	TInt i = 0;
	
	for(; i < count; ++i)
		{
		if( (*iFields)[i].StorageType() == KStorageTypeContactItemId )
			{
			(*iFields)[i].AgentStorage()->SetAgentId(aVal);
			FoundAgentField = ETrue;
			break;
			}
		}
		
	if( !FoundAgentField )
		{
		AddAgentFieldL(aVal);
		}
		
	CommitL();
	}

void CAccessCountTests::ExportVcardL(const CContactIdArray &aConids)
	{	
	RFileWriteStream rfws;
	CleanupClosePushL(rfws);
	User::LeaveIfError( rfws.Replace( iParent->Fs(), KVcardFile, EFileWrite | EFileShareAny ) );
	
	iContactsDatabase->ExportSelectedContactsL(	TUid::Uid(KUidVCardConvDefaultImpl), aConids, rfws,
										CContactDatabase::EIncludeX | CContactDatabase::ETTFormat );
	
	CleanupStack::PopAndDestroy(&rfws);		
	}	
	
void CAccessCountTests::ImportVcardL()
	{
	TBool sucess = EFalse;
	RFileReadStream rfrs;
	CleanupClosePushL( rfrs );
	User::LeaveIfError( rfrs.Open( iParent->Fs(), KVcardFile, EFileRead | EFileShareAny ) );
	
	FOREVER
		{
		CArrayPtr<CContactItem>* imported= 	iContactsDatabase->ImportContactsL( 
											TUid::Uid( KUidVCardConvDefaultImpl ), rfrs, sucess, 
											CContactDatabase::EIncludeX | CContactDatabase::ETTFormat |
											CContactDatabase::EImportSingleContact
											);	
		if(sucess)
			{
			imported->ResetAndDestroy();
			CLEAR(imported);	
			}
		else
			{
			imported->ResetAndDestroy();
			CLEAR(imported);
			break;	
			}
		sucess = EFalse;
		}

	CleanupStack::PopAndDestroy(&rfrs);
	}

TInt CAccessCountTests::GetAccessCountL(const TContactItemId aCid)
	{
	ReadL( aCid, *iViewAll );
	TInt ret = iContactItem->AccessCount();
	Close();
	return ret;
	}


