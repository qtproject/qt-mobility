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

#include <e32panic.h>
#include <testexecutelog.h>
#include <cntfldst.h>
#include "clientserver.h"
#include "simpleviewdeftests.h"

_LIT(KTest1,"Testing Inverse behaviour of MI viewdef...");
_LIT(KTest2,"Testing Omission of primary mapping in viewdefs...");
_LIT(KTest3,"Semi-Complex Testing to demonstrate that the primary mapping for all uids are ignored...");

_LIT(KMedium, "MSajkldjKJIlk30aksdjjfeiME");
_LIT(KTimeS,"22000101:235959.999999");// time to use for datetime storage
static const TContactItemId KId = 1;//contact id to use for agent storage
static const TTime KTime(KTimeS);


//Number of contacts
CSimpleViewDefTest::CSimpleViewDefTest():CViewDefBase(1)
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(SharedConstants::KSimpleViewDefTest);
	}

TVerdict CSimpleViewDefTest::doTestStepL()
	{
	__UHEAP_MARK;
	static const TInt KManyContacts = 3;
	
	CContactItemViewDef *viewAll = CContactItemViewDef::NewLC(CContactItemViewDef::EIncludeFields,
	CContactItemViewDef::EIncludeHiddenFields);
	viewAll->AddL(KUidContactFieldMatchAll);

	CContactDatabase *contactsDatabase = CContactDatabase::ReplaceL();
	CleanupStack::PushL(contactsDatabase);		
			
	CContactTemplate* ctemplate = STATIC_CAST( CContactTemplate*, 
	contactsDatabase->ReadContactLC( contactsDatabase->TemplateId(), *viewAll ) );
	
	CContactCard* contact = CContactCard::NewLC(ctemplate);
	for(TInt i = 0; i< KManyContacts; ++i)
		{
		iIterate->AddL( contactsDatabase->AddNewContactL(*contact) );
		}
	CleanupStack::PopAndDestroy(contact);
	iIterate->Reset();
	INFO_PRINTF1(KTest1);
	MIInverseTestL( iIterate->NextL(), *contactsDatabase, *viewAll );
	INFO_PRINTF1(KTest2);
	PrimayMappingsTestL( iIterate->NextL(), *contactsDatabase, *viewAll );
	//this is not really a simple test, using it to confirm that behaviour applies acroos the board
	INFO_PRINTF1(KTest3);
	AllPrimayMappingsTestL( iIterate->NextL(), *contactsDatabase, *viewAll );
	
	CleanupStack::PopAndDestroy(ctemplate);
	CleanupStack::PopAndDestroy(contactsDatabase);
	CleanupStack::PopAndDestroy(viewAll);
	iIterate->Close();
	__UHEAP_MARKEND;
	
	return TestStepResult();
	}

/*this is for developers to test, doesnt use any memeber variable or such
for the sake of simplicity and demonstrability.... This adds one contact to the
database and sets all of its fiedls
*/
void CSimpleViewDefTest::SetupTestL(	const TContactItemId aCid, 
										CContactDatabase &aContactsDatabase,
										const CContactItemViewDef &aViewAll)
	{
	TInt length =0;
	
	CContactItem *contactItem = NULL;
	
	contactItem = aContactsDatabase.OpenContactLX(aCid,aViewAll);
	CleanupStack::PushL(contactItem);
	
	CContactItemFieldSet *fields = &( contactItem->CardFields() );
	CContactItemField *field = NULL;
	length = fields->Count();
	
	for(TInt i = 0; i<length;i++)
		{
		field = &((*fields)[i]);
		switch(field->StorageType())
			{
			case KStorageTypeText:
				field->TextStorage()->SetTextL(KMedium);
			break;
			case KStorageTypeStore:
				{
				const TDesC &mstring = KMedium;
				HBufC8 *buf = HBufC8::NewLC(mstring.Size());
				TPtr8 tptr8 = buf->Des();
				tptr8.Copy(KMedium);
				field->StoreStorage()->SetThingL(*buf);
				CleanupStack::PopAndDestroy(buf);
				}
			break;
			case KStorageTypeContactItemId:
				field->AgentStorage()->SetAgentId(KId);
			break;
			case KStorageTypeDateTime:
				field->DateTimeStorage()->SetTime(KTime);
			break;
			default:
				User::Panic(KMedium,EInvariantFalse);
			break;
			}
		}

	
	aContactsDatabase.CommitContactL(*contactItem);
	CleanupStack::PopAndDestroy(contactItem);
	CleanupStack::Pop();//lock
	//CleanupStack::Pop(viewMI);
	contactItem = NULL;
	fields = NULL;
	
	}
/*when using mi viewdef to load data no hidden fields have their values restored,
i.e. that action of include hidden fields is reveresed to act as exclude*/	
void CSimpleViewDefTest::MIInverseTestL(	const TContactItemId aCid, 
											CContactDatabase &aContactsDatabase,
											const CContactItemViewDef &aViewAll)
	{
	SetupTestL(aCid,aContactsDatabase,aViewAll);
	CContactItem *contactItem = NULL;
	
	CContactItemViewDef *viewMI = NULL;
	viewMI = CContactItemViewDef::NewLC(CContactItemViewDef::EMaskFields,
	CContactItemViewDef::EIncludeHiddenFields);
	viewMI->AddL(KUidContactFieldMatchAll);
	
	aContactsDatabase.SetViewDefinitionL(CContactViewDef::NewL(viewMI));
	CleanupStack::Pop(viewMI);
	contactItem = aContactsDatabase.ReadMinimalContactLC(aCid);
	
	CContactItemFieldSet &fields = contactItem->CardFields();
	TInt hcount = fields.Count();
	
	INTCOMPARE(hcount, ==, 0, 0, 0);/*there should be three fields in readminimal, all hidden
						as in instead if including all hidden, they are all excluded*/

	CleanupStack::PopAndDestroy(contactItem);
	contactItem = NULL;
	}
	
/*when using any viewdef that contains a primary mapping uid to load data,
then those fields which use that uid are not excluded/included as necessary*/	
void CSimpleViewDefTest::PrimayMappingsTestL(	const TContactItemId aCid, 
												CContactDatabase &aContactsDatabase,
												const CContactItemViewDef &aViewAll)
	{
	SetupTestL(aCid,aContactsDatabase,aViewAll);
	doPrimayMappingsTestL(aCid, aContactsDatabase, aViewAll, KIntContactFieldVCardMapADR);
	
	}
	
/*when using any viewdef that contains a primary mapping uid to load data,
then those fields which use that uid are not excluded/included as necessary*/	
void CSimpleViewDefTest::AllPrimayMappingsTestL(	const TContactItemId aCid, 
													CContactDatabase &aContactsDatabase,
													const CContactItemViewDef &aViewAll)
	{
	SetupTestL(aCid,aContactsDatabase,aViewAll);
	TInt length = iExistingUidsArray->Count();
	for(TInt i = 0; i < length; ++i)
		{
		doPrimayMappingsTestL(aCid, aContactsDatabase, aViewAll, (*iExistingUidsArray)[i]);
		}
	}

void CSimpleViewDefTest::doPrimayMappingsTestL(	const TContactItemId aCid, 
													CContactDatabase &aContactsDatabase,
													const CContactItemViewDef &aViewAll,
													const TInt32 aUid)
	{
	CContactItem *contactItem = NULL;
	TInt fieldsIV = 0;//fields in viewdef
	TInt primaryFieldsIV = 0;
		
	contactItem = aContactsDatabase.ReadContactLC(aCid,aViewAll);
	CContactItemFieldSet *fields = &( contactItem->CardFields() );
	TInt fcount = fields->Count();
	TInt ccount = 0;
	TBool uInArray = EFalse;
	const CContentType *ctype = NULL;
	
	for(TInt i = 0; i < fcount; ++i)
		{
		ctype = &( (*fields)[i].ContentType() );
		ccount = ctype->FieldTypeCount();
		uInArray = EFalse;
		for(TInt j = 0; j < ccount; ++j)
			{
			if( ctype->FieldType(j).iUid == aUid ) //uid stored in standard fieldtype array
				{
				++fieldsIV;
				uInArray = ETrue;
				break;
				}
			}
		if( !uInArray && ( ctype->Mapping().iUid == aUid ) )//uid only stored in primary mapping
			{
			++primaryFieldsIV;
			}
		}
		
	CleanupStack::PopAndDestroy(contactItem);
	contactItem = NULL;
	fields = NULL;
	
	CContactItemViewDef *viewII = NULL;
	viewII = CContactItemViewDef::NewLC(CContactItemViewDef::EIncludeFields,
	CContactItemViewDef::EIncludeHiddenFields);
	viewII->AddL( TUid::Uid(aUid) );
	
	aContactsDatabase.SetViewDefinitionL( CContactViewDef::NewL(viewII) );
	CleanupStack::Pop(viewII);
	contactItem = aContactsDatabase.ReadMinimalContactLC(aCid);
	fields = &( contactItem->CardFields() );
	
	TInt resCount = 0;
	resCount = fields->Count();
	
	INTCOMPARE( fcount, >, 0, aUid, 0 );
	//primaryFieldsIV is ignored
	INTCOMPARE(  resCount, ==, fieldsIV , aUid, 0 );
		
	CleanupStack::PopAndDestroy(contactItem);
	contactItem = NULL;
	fields = NULL;
	}

