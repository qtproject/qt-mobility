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
* Run as part T_DBASE2, tests find
*
*/


#include <e32test.h>
#include <f32file.h>
#include <s32file.h>
#include <s32mem.h>
#include <cntdb.h>
#include <cntitem.h>
#include <cntfield.h>
#include <cntfldst.h>
#include "t_utils.h"
#include "t_dbase2.h"

class CAsyncFinder : public CBase, public MIdleFindObserver
	{
public:
	~CAsyncFinder();
	void ConstructL(CContactDatabase *aContactDatabase, const TDesC& aText,const CContactItemFieldDef *aFieldDef);
	void ConstructL(CContactDatabase *aContactDatabase, const CDesCArray& aFindWords,const CContactTextDef *aTextDef,const TCallBack &aCallBack);
	inline CContactIdArray *TakeContactIds() {return(iFinder->TakeContactIds());};
	inline TInt Error() const {return(iFinder->Error());};
protected:
// from MIdleFindObserver
	void IdleFindCallback();
private:
	CIdleFinder *iFinder;
	};

CAsyncFinder::~CAsyncFinder()
	{
	delete iFinder;
	}

void CAsyncFinder::ConstructL(CContactDatabase *aContactDatabase, const TDesC& aText,const CContactItemFieldDef *aFieldDef)
	{
	iFinder=aContactDatabase->FindAsyncL(aText, aFieldDef, this);
	}

void CAsyncFinder::ConstructL(CContactDatabase *aContactDatabase, const CDesCArray& aFindWords,const CContactTextDef *aTextDef,const TCallBack &aCallBack)
	{
	if (aTextDef)
		iFinder=aContactDatabase->FindInTextDefAsyncL(aFindWords, aTextDef, this, aCallBack);
	else
		iFinder=aContactDatabase->FindInTextDefAsyncL(aFindWords, this, aCallBack);
	}

void CAsyncFinder::IdleFindCallback()
	{
	if (iFinder->IsComplete())
		CActiveScheduler::Stop();
	}

GLDEF_C TBool CompareIdArrays(CContactIdArray* aIds1,CContactIdArray* aIds2)
	{
	if (aIds1->Count()!=aIds2->Count())
		return(EFalse);
	for(TInt loop=0;loop<aIds1->Count();loop++)
		{
		if ((*aIds1)[loop]!=(*aIds2)[loop])
			return(EFalse);
		}
	return(ETrue);
	}

GLDEF_C CContactIdArray* CreateIdArrayLC(TContactItemId *aId1, TContactItemId *aId2=NULL, TContactItemId *aId3=NULL, TContactItemId *aId4=NULL, TContactItemId *aId5=NULL)
	{
	CContactIdArray* ids=CContactIdArray::NewLC();
	if (aId1)
		ids->AddL(*aId1);
	if (aId2)
		ids->AddL(*aId2);
	if (aId3)
		ids->AddL(*aId3);
	if (aId4)
		ids->AddL(*aId4);
	if (aId5)
		ids->AddL(*aId5);
	return(ids);
	}

LOCAL_C CDesCArray *CreateFindWordsL(const TDesC &aFind1,const TDesC &aFind2,const TDesC &aFind3)
	{
	CDesCArray *desCA=new(ELeave) CDesCArrayFlat(5);
	CleanupStack::PushL(desCA);
	if (aFind1.Length()>0)
		desCA->AppendL(aFind1);
	if (aFind2.Length()>0)
		desCA->AppendL(aFind2);
	if (aFind3.Length()>0)
		desCA->AppendL(aFind3);
	CleanupStack::Pop();	// desCA
	return(desCA);
	}

LOCAL_C void CheckResult(CContactIdArray* aIds,CContactIdArray* aResultIds)
	{
	if (aResultIds)
		test(CompareIdArrays(aIds,aResultIds));
	else
		test(aIds->Count()==0);
	}

LOCAL_C void runAsyncFindL(CAsyncFinder *aFinder, CContactIdArray* aResultIds)
	{
	CActiveScheduler::Start();
	CContactIdArray* ids=aFinder->TakeContactIds();
	CheckResult(ids,aResultIds);
	delete ids;
	}

LOCAL_C TInt findWordSplitterL(TAny *aParams)
	{
	SFindInTextDefWordParser *parser=(SFindInTextDefWordParser *)aParams;
	const TText *ptr=parser->iSearchString->Ptr();
	const TText *end=ptr+parser->iSearchString->Length();
	const TText *startOfWord=NULL;
	FOREVER
		{
		if (ptr==end || !TChar(*ptr).IsAlphaDigit())
			{
			if (startOfWord)
				{
				TPtrC addWord(startOfWord,ptr-startOfWord);
				parser->iWordArray->AppendL(addWord);
				startOfWord=NULL;
				}
			if (ptr==end)
				break;
			}
		else if (!startOfWord)
			startOfWord=ptr;
		ptr++;
		}
	return(KErrNone);
	}

GLDEF_C void doDoFindInTextDefTestL(CDesCArray *aFindWords, CContactTextDef *aTextDef, CContactIdArray* aResultIds)
	{
	TCallBack callBack(findWordSplitterL);
	CAsyncFinder *finder=new(ELeave) CAsyncFinder;
	CleanupStack::PushL(finder);
	finder->ConstructL(CntTest->Db(),*aFindWords,aTextDef,callBack);
	runAsyncFindL(finder, aResultIds);
	CleanupStack::PopAndDestroy(); // finder
//
	CContactIdArray* ids=NULL;
	if (aTextDef)
		ids=CntTest->Db()->FindInTextDefLC(*aFindWords,aTextDef,callBack);
	else
		ids=CntTest->Db()->FindInTextDefLC(*aFindWords,callBack);
	CheckResult(ids,aResultIds);
	CleanupStack::PopAndDestroy(); // ids
	}

GLDEF_C void DoFindInTextDefTestL(CDesCArray *aFindWords, CContactTextDef *aTextDef, CContactIdArray* aResultIds)
	{
	CleanupStack::PushL(aFindWords);
	doDoFindInTextDefTestL(aFindWords, aTextDef, aResultIds);
	CContactTextDef* textDef=CContactTextDef::NewLC();
	if (aTextDef)
		{
		for(TInt loop=0;loop<aTextDef->Count();loop++)
			textDef->AppendL((*aTextDef)[loop]);
		textDef->SetFallbackField(aTextDef->FallbackField());
		}
	CntTest->Db()->SetTextDefinitionL(textDef);
	CleanupStack::Pop();	// textDef
	doDoFindInTextDefTestL(aFindWords, NULL, aResultIds);
	CntTest->Db()->SetTextDefinitionL(NULL);
	CleanupStack::Pop(); // aFindWords
	}

GLDEF_C void DoFindInTextDefTestLD(CDesCArray *aFindWords, CContactTextDef *aTextDef, CContactIdArray* aResultIds)
	{
	DoFindInTextDefTestL(aFindWords, aTextDef, aResultIds);
	if (aFindWords->Count()>0)
		{
		CDesCArray *reverseArray=new(ELeave) CDesCArrayFlat(10);
		CleanupStack::PushL(reverseArray);
		for(TInt loop=aFindWords->Count()-1;loop>=0;loop--)
			reverseArray->AppendL((*aFindWords)[loop]);
		DoFindInTextDefTestL(reverseArray, aTextDef, aResultIds);
		CleanupStack::PopAndDestroy();
		}
	delete aFindWords;
	}

GLDEF_C void DoAsyncFindL(const TDesC &aText, CContactItemFieldDef *aFieldDef,CContactIdArray* aResultIds)
	{
	CAsyncFinder *finder=new(ELeave) CAsyncFinder;
	CleanupStack::PushL(finder);
	finder->ConstructL(CntTest->Db(),aText,aFieldDef);
	runAsyncFindL(finder, aResultIds);
	CleanupStack::PopAndDestroy(); // finder
	}

GLDEF_C void DoAsyncFindL(const TDesC &aText, CContactItemFieldDef *aFieldDef, TInt aCount)
	{
	CAsyncFinder *finder=new(ELeave) CAsyncFinder;
	CleanupStack::PushL(finder);
	finder->ConstructL(CntTest->Db(),aText,aFieldDef);
	CActiveScheduler::Start();
	CContactIdArray* ids=finder->TakeContactIds();
	test(ids->Count()==aCount);
	delete ids;
	CleanupStack::PopAndDestroy(); // finder
	}

/**

@SYMTestCaseID     PIM-T-FIND-0001

*/

GLDEF_C void FindAsyncL()
	{
	test.Next(_L("@SYMTestCaseID:PIM-T-FIND-0001 Async search 1"));
//
	TBuf<32> name;
	name.Format(KTestName,7);
	DoAsyncFindL(name,NULL,1);
//
	test.Next(_L("Async search 2"));
	CContactItemFieldDef *fieldDef=new(ELeave) CContactItemFieldDef;
	CleanupStack::PushL(fieldDef);
	fieldDef->AppendL(KUidContactFieldFamilyName);
	DoAsyncFindL(_L("Test Name No"),fieldDef,CntTest->Db()->CountL()-3); // ..empty groups & own card
	CleanupStack::PopAndDestroy(); // fieldDef
//
	test.Next(_L("Async search 3"));
	CContactItemFieldDef *fieldDef2=new(ELeave) CContactItemFieldDef;
	CleanupStack::PushL(fieldDef2);
	fieldDef2->AppendL(KUidContactFieldAddress);
	DoAsyncFindL(_L("Test Name No"),fieldDef2,0);
	CleanupStack::PopAndDestroy(); // fieldDef2
//
#if defined(_DEBUG)
	CAsyncFinder *finder=new(ELeave) CAsyncFinder;
	CleanupStack::PushL(finder);
	TBuf<32> name2;
	name2.Format(KTestName,7);
	finder->ConstructL(CntTest->Db(),name2,NULL);
	__UHEAP_SETFAIL(RHeap::EDeterministic,10);
	CActiveScheduler::Start();
	__UHEAP_RESET;
//
	CContactIdArray* ids=finder->TakeContactIds();
	test(finder->Error()==KErrNoMemory || finder->Error()==KErrNone);
	delete ids;
	CleanupStack::PopAndDestroy(); // finder
#endif
//
	CAsyncFinder *asyncFinder=new(ELeave) CAsyncFinder;
	asyncFinder->ConstructL(CntTest->Db(), _L("a"), NULL);
	delete asyncFinder;	// Checks ids get deleted if not claimed with TakeContactIds()
	}

LOCAL_C TContactItemId AddFindRecordL(const TDesC &aFirstName, const TDesC &aLastName, const TDesC &aCompanyName, const TDesC &aAddress)
	{
	CContactItem *item=CContactCard::NewLC();
	if (aFirstName.Length()>0)
		SetNameL(*item,KUidContactFieldGivenName,KUidContactFieldVCardMapUnusedN,aFirstName,ETrue);
	if (aLastName.Length()>0)
		SetNameL(*item,KUidContactFieldFamilyName,KUidContactFieldVCardMapUnusedN,aLastName,ETrue);
	if (aCompanyName.Length()>0)
		SetNameL(*item,KUidContactFieldCompanyName,KUidContactFieldVCardMapORG,aCompanyName,ETrue);
	if (aAddress.Length()>0)
		SetNameL(*item,KUidContactFieldAddress,KUidContactFieldVCardMapADR,aAddress,ETrue);
	TContactItemId id=CntTest->Db()->AddNewContactL(*item);
	CleanupStack::PopAndDestroy();	// item
	return(id);
	}

GLDEF_C void FindAsyncInTextDefL()
	{
	CContactTextDef* textDef1=CContactTextDef::NewLC();
	textDef1->AppendL(TContactTextDefItem(KUidContactFieldGivenName));
	textDef1->AppendL(TContactTextDefItem(KUidContactFieldFamilyName));
	textDef1->SetFallbackField(KUidContactFieldCompanyName);
	CContactTextDef* textDef2=CContactTextDef::NewLC();
	textDef2->AppendL(TContactTextDefItem(KUidContactFieldGivenName));
	textDef2->AppendL(TContactTextDefItem(KUidContactFieldFamilyName));
	textDef2->AppendL(TContactTextDefItem(KUidContactFieldCompanyName));
	textDef2->SetFallbackField(KUidContactFieldAddress);
//
	TContactItemId wally=AddFindRecordL(_L("Wally"),_L("Wombat"),_L("Wombat Soup Inc"),_L(""));
	TContactItemId vincent=AddFindRecordL(_L("Vincent"),_L("Jones"),_L(""),_L(""));
	TContactItemId wilbert=AddFindRecordL(_L("Wilbert"),_L("Wombat"),_L("Great Ape Software"),_L(""));
	TContactItemId vincent2=AddFindRecordL(_L("Vincent"),_L("Vincent"),_L(""),_L(""));
	TContactItemId death=AddFindRecordL(_L("Black"),_L("De'ath"),_L("Hell"),_L("South of Heaven"));
//
	CContactIdArray* wallyResults=CreateIdArrayLC(&wally);
	CContactIdArray* wombatResults=CreateIdArrayLC(&wally,&wilbert);
	CContactIdArray* vjResults=CreateIdArrayLC(&vincent);
	CContactIdArray* v2Results=CreateIdArrayLC(&vincent2);
	CContactIdArray* vjv2Results=CreateIdArrayLC(&vincent,&vincent2);
	CContactIdArray* deathResults=CreateIdArrayLC(&death);
//	
	CDesCArray *findWords=CreateFindWordsL(_L("Wally"),_L(""),_L(""));
//chrisd: the behaviour of the new code has changed because if you pass a null 
//chrisd: textdef to ER5/ER6 or 6.1 it doesn't find anything
	DoFindInTextDefTestL(findWords, NULL, NULL);	// No text def defined
//
	DoFindInTextDefTestLD(findWords, textDef1, wallyResults);
	findWords=CreateFindWordsL(_L("Wally"),_L("NotFound"),_L(""));
	DoFindInTextDefTestLD(findWords, textDef1, NULL);
	findWords=CreateFindWordsL(_L("Wally"),_L("Wombat"),_L(""));
	DoFindInTextDefTestLD(findWords, textDef1, wallyResults);
	findWords=CreateFindWordsL(_L("Wally"),_L("Wombat"),_L("xxx"));
	DoFindInTextDefTestLD(findWords, textDef1, NULL);
	findWords=CreateFindWordsL(_L("W"),_L("Wombat"),_L(""));
	DoFindInTextDefTestLD(findWords, textDef1, wombatResults);
	findWords=CreateFindWordsL(_L("Wally"),_L("Wally"),_L(""));
	DoFindInTextDefTestLD(findWords, textDef1, NULL);
	findWords=CreateFindWordsL(_L("Wombat"),_L("Wombat"),_L(""));
	DoFindInTextDefTestLD(findWords, textDef1, wallyResults);
	findWords=CreateFindWordsL(_L("Wombat"),_L("Wombat"),_L("Wombat"));
	DoFindInTextDefTestLD(findWords, textDef1, NULL);
	findWords=CreateFindWordsL(_L("Wombat"),_L("Soup"),_L("Inc"));
//chrisd: new code doesn't find anything here and should have - this is
//chrisd: all words are contained in the company field
DoFindInTextDefTestLD(findWords, textDef1, wallyResults);
//
	findWords=CreateFindWordsL(_L("ombat"),_L("oup"),_L("lly"));
	DoFindInTextDefTestLD(findWords, textDef1, NULL);
	findWords=CreateFindWordsL(_L("Wombat"),_L("Soup"),_L("Inc"));
//chrisd: new code doesn't find anything here and should have - this is
//chrisd: all words are contained in the company field
DoFindInTextDefTestLD(findWords, textDef1, wallyResults);
//
	findWords=CreateFindWordsL(_L("Vincent"),_L("J"),_L(""));
	DoFindInTextDefTestLD(findWords, textDef1, vjResults);
	findWords=CreateFindWordsL(_L("Vincent"),_L("V"),_L(""));
	DoFindInTextDefTestLD(findWords, textDef1, v2Results);
	findWords=CreateFindWordsL(_L("Vincent"),_L(""),_L(""));
	DoFindInTextDefTestLD(findWords, textDef1, vjv2Results);

	findWords=CreateFindWordsL(_L("De"),_L("Ath"),_L(""));
//chrisd: new code fails here
//chrisd: last name contains both first & last names (PC PIM syncs both to one field)
DoFindInTextDefTestLD(findWords, textDef1, deathResults);
	findWords=CreateFindWordsL(_L("De"),_L("Ath"),_L("Heaven"));
	DoFindInTextDefTestLD(findWords, textDef1, NULL);
//chrisd: new code fails here
//chrisd: last name contains first and last
findWords=CreateFindWordsL(_L("De"),_L("Ath"),_L("Heaven"));
DoFindInTextDefTestLD(findWords, textDef2, deathResults);
	findWords=CreateFindWordsL(_L("Heaven"),_L(""),_L(""));
//chrisd:one word at the end - partial match - so doesn't need to match
DoFindInTextDefTestLD(findWords, textDef1, NULL);
	findWords=CreateFindWordsL(_L("Heaven"),_L(""),_L(""));
//chrisd:one word at the end - partial match - so doesn't need to match
DoFindInTextDefTestLD(findWords, textDef2, deathResults);
findWords=CreateFindWordsL(_L("of"),_L("South"),_L("Heaven"));
//chrisd:all words are in one field - this doesn't need to match
DoFindInTextDefTestLD(findWords, textDef2, deathResults);
//
	CntTest->DeleteContact(wally);
	CntTest->DeleteContact(vincent);
	CntTest->DeleteContact(wilbert);
	CntTest->DeleteContact(vincent2);
	CntTest->DeleteContact(death);
	CleanupStack::PopAndDestroy(6);	// wallyResults,wombatResults,vjResults,v2Results,deathResults
	CleanupStack::PopAndDestroy(2);	// textDef1,textDef2
	}

GLDEF_C void FindL()
	{
	CContactItemFieldDef* fieldDef=new(ELeave) CContactItemFieldDef;
	CleanupStack::PushL(fieldDef);
	fieldDef->AppendL(KUidContactFieldAddress);
	fieldDef->AppendL(KUidContactFieldFamilyName);
	fieldDef->AppendL(KUidContactFieldGivenName);
	fieldDef->AppendL(KUidContactFieldCompanyName);
	fieldDef->AppendL(KUidContactFieldBirthday);
	fieldDef->AppendL(KUidContactFieldTemplateLabel);
//
	test.Next(_L("Search for full given name"));
	TBuf<16> name;
	name.Format(KTestName,7);
	CContactIdArray* ids=CntTest->Db()->FindLC(name,fieldDef);
	test(ids->Count()==1);
	CleanupStack::PopAndDestroy(); // ids

	test.Next(_L("Search for full given name in only contact groups"));
	// same test but only looking in groups so shouldn't find anything
	name.Format(KTestName,7);
	CntTest->Db()->SetDbViewContactType(KUidContactGroup);
	ids=CntTest->Db()->FindLC(name,fieldDef);
	test(ids->Count()==0); // nothing found.
	CleanupStack::PopAndDestroy(); // ids

	CntTest->Db()->SetDbViewContactType(KUidContactItem); //reset view
	test.Next(_L("Search for partial name"));
	TBuf<8> shortName(name.Left(8));
	ids=CntTest->Db()->FindLC(shortName,fieldDef);
	test(ids->Count()==KTotalNumRecords);
	CleanupStack::PopAndDestroy(); // ids

	shortName=name.Right(8);
	ids=CntTest->Db()->FindLC(shortName,fieldDef);
	test(ids->Count()==1);
	CleanupStack::PopAndDestroy(); // ids
	shortName=name.Mid(3,8);
	ids=CntTest->Db()->FindLC(shortName,fieldDef);
	test(ids->Count()>1);
	CleanupStack::PopAndDestroy(); // ids

	test.Next(_L("Search for full address"));
	TBuf<16> address;
	address.Format(KTestAddress,11);
	ids=CntTest->Db()->FindLC(address,fieldDef);
	test(ids->Count()==1);
	CleanupStack::PopAndDestroy(); // ids

	test.Next(_L("NULL search string"));
	ids=CntTest->Db()->FindLC(_L(""),fieldDef);
	test(ids->Count()==KTotalNumRecords + 3); // 2 groups + 1 owncard
	CleanupStack::PopAndDestroy(); // ids

	// look in just the groups
	test.Next(_L("NULL search string in GROUPS"));
	TInt groupCount = CntTest->Db()->GroupCount();
	CntTest->Db()->SetDbViewContactType(KUidContactGroup);
	ids=CntTest->Db()->FindLC(_L(""),fieldDef);
	test(ids->Count()==groupCount);
	CleanupStack::PopAndDestroy(); // ids
//
	ids=CntTest->Db()->FindLC(_L("New Group"),fieldDef);
	test(ids->Count()==groupCount);
	CleanupStack::PopAndDestroy(); // ids
//
	CntTest->Db()->SetDbViewContactType(KUidContactItem);

	// look in just the OWN CARD
	test.Next(_L("NULL search string in OWN CARD"));
	CntTest->Db()->SetDbViewContactType(KUidContactOwnCard);
	ids=CntTest->Db()->FindLC(_L(""),fieldDef);
	test(ids->Count()==1);
	CleanupStack::PopAndDestroy(); // ids
//
	CntTest->Db()->SetDbViewContactType(KUidContactItem);

	test.Next(_L("Search for partial address"));
	TBuf<8> shortAddress(address.Left(6));
	ids=CntTest->Db()->FindLC(shortAddress,fieldDef);
	test(ids->Count()==KTotalNumRecords/2);
	CleanupStack::PopAndDestroy(); // ids
	shortAddress=address.Right(6);
	ids=CntTest->Db()->FindLC(shortAddress,fieldDef);
	test(ids->Count()==1);
	CleanupStack::PopAndDestroy(); // ids
	shortAddress=address.Mid(3,6);
	ids=CntTest->Db()->FindLC(shortAddress,fieldDef);
	test(ids->Count()>1);
	TContactItemId deleteItem=(*ids)[0];
	CleanupStack::PopAndDestroy(); // ids
//
	CntTest->AdjustContactAccessCountL(deleteItem,1);
	CntTest->Db()->DeleteContactL(deleteItem);
//
	ids=CntTest->Db()->FindLC(shortAddress,NULL);
	test(ids->Count()>1);
	CleanupStack::PopAndDestroy(); // ids
//
	CntTest->AdjustContactAccessCountL(deleteItem,-1);
	ids=CntTest->Db()->FindLC(shortAddress,NULL);
	test(ids->Count()>1);
	CleanupStack::PopAndDestroy(); // ids
//
	ids=CntTest->Db()->FindLC(_L("*"),NULL);
	test(ids->Count()==0); // literal matching now and no asterisks in this file!
	CleanupStack::PopAndDestroy(); // ids
//
	ids=CntTest->Db()->FindLC(shortAddress,fieldDef);
	test(ids->Count()>1);
	CleanupStack::PopAndDestroy(); // ids
//
	CleanupStack::PopAndDestroy(); // fieldDef
	}

GLDEF_C TContactItemId AddContactForFindL(const TDesC& aName)
	{
	CContactItem* item=CContactCard::NewLC();
	SetNameL(*item,KUidContactFieldGivenName,KUidContactFieldVCardMapUnusedN,aName,ETrue);	
	item->SetTemplateRefId(KGoldenTemplateId);
	TContactItemId id=CntTest->Db()->AddNewContactL(*item);
	CleanupStack::PopAndDestroy(item); 
	return id;
	}

GLDEF_C TContactItemId AddContactWithAddressL(const TDesC& aName, const TDesC& anAddress)
	{
	CContactItem* item=CContactCard::NewLC();
	item->SetTemplateRefId(KGoldenTemplateId);
	SetNameL(*item,KUidContactFieldGivenName,KUidContactFieldVCardMapUnusedN,aName,ETrue);	
	SetNameL(*item,KUidContactFieldAddress,  KUidContactFieldVCardMapUnusedN,anAddress,ETrue);	
	TContactItemId id=CntTest->Db()->AddNewContactL(*item);
	CleanupStack::PopAndDestroy(item); 
	return id;
	}

//
// Testcode for find defect 
// EDNAPIN-4L4EN9 "Wildcards should be treated as normal charactersin search"
//
// Test case:
// a contacts database containing contacts with names
// *,?,chris,c?ris,c*s,??,????
// 
// '*' matches contacts with fields containing '*' 
// '?' matches contacts with fields containing '?'
// 'chris' matches contacts with fields containing 'chris' but not 'c*s' or 'c?ris'
// 'c?ris' matches contacts with fields containing 'c?ris' but not 'c*s' or 'chris'
// 'c*' matches contacts with fields containing 'c*' but not 'chris' or 'c?ris'
// '??' matches contacts with fields containing '??' but not '?', 'chris', 'c*s' etc.
//
GLDEF_C void FindDefectL()
	{
	test.Next(_L("Find defect"));
	CntTest->CreateDatabaseL();
	_LIT(KAsterisk,"*");
	_LIT(KQuestionMark,"?");
	_LIT(KChris,"chris");
	_LIT(KChrisQuestion,"c?ris");
	_LIT(KChrisAsterisk,"c*s");
	_LIT(KTwoQuestionMarks,"??");
	_LIT(KFourQuestionMarks,"????");
	TContactItemId asterisk = AddContactForFindL(KAsterisk());
	AddContactForFindL(KQuestionMark());
	TContactItemId chris = AddContactForFindL(KChris());
	TContactItemId chrisquestion = AddContactForFindL(KChrisQuestion());
	TContactItemId chrisasterisk = AddContactForFindL(KChrisAsterisk());
	AddContactForFindL(KTwoQuestionMarks());
	AddContactForFindL(KFourQuestionMarks());


	CContactItemFieldDef* fieldDef=new(ELeave) CContactItemFieldDef;
	CleanupStack::PushL(fieldDef);
	fieldDef->AppendL(KUidContactFieldGivenName);

	// *	
	CContactIdArray* ids=CntTest->Db()->FindLC(KAsterisk,fieldDef);
	test(ids->Count()==2);
	test((*ids)[0]==asterisk);
	CleanupStack::PopAndDestroy(); // ids

	// ?
	ids=CntTest->Db()->FindLC(KQuestionMark,fieldDef);
	test(ids->Count()==4); // should be 4
	CleanupStack::PopAndDestroy(); // ids

	// chris
	ids=CntTest->Db()->FindLC(KChris,fieldDef);
	test(ids->Count()==1); // should be 1
	test((*ids)[0]==chris);
	CleanupStack::PopAndDestroy(); // ids

	// c?ris
	ids=CntTest->Db()->FindLC(KChrisQuestion,fieldDef);
	test(ids->Count()==1); // should be 1
	test((*ids)[0]==chrisquestion);
	CleanupStack::PopAndDestroy(); // ids

	// c*
	ids=CntTest->Db()->FindLC(KChrisAsterisk,fieldDef);
	test(ids->Count()==1); // should be 1
	test((*ids)[0]==chrisasterisk);
	CleanupStack::PopAndDestroy(); // ids

	// ??
	ids=CntTest->Db()->FindLC(KTwoQuestionMarks,fieldDef);
	test(ids->Count()==2); // should be 2
	CleanupStack::PopAndDestroy(); // ids

	CleanupStack::PopAndDestroy(fieldDef);
	CntTest->CloseDatabase();
	CntTest->DeleteDatabaseL();
	};

GLDEF_C void FindAndCheckL(const TDesC& aStr, const TInt aNum, const CContactItemFieldDef* const aFieldDef)
	{
	CContactIdArray* const ids = CntTest->Db()->FindLC(aStr,aFieldDef);
	test(aNum == ids->Count());
	CleanupStack::PopAndDestroy(ids);
	}

GLDEF_C void FindScandinavianLettersL()
	{
	test.Next(_L("Scandinavian Letters defect"));
	CntTest->CreateDatabaseL();

	//Scandinavian words with their conterparts without accents.
	_LIT(KMalmoS,"Malm\xF6"); //Malmö
	_LIT(KMalmo,"Malmo");
	_LIT(KAraS,"\xC5ra");  //Åra
	_LIT(KAra,"Ara");
	_LIT(KAtaS,"\xC4ta");  //Äta
	_LIT(KAta,"Ata");

	//Adding some contacts with Scandinavian letter in them.
	//Note that Name is stored in separate field while address is in the BLOB
	AddContactWithAddressL(KAraS, KMalmoS);
	AddContactWithAddressL(KAtaS, KAtaS);
	AddContactWithAddressL(KAtaS, KAraS);
	AddContactWithAddressL(KMalmoS, KMalmoS);

	//Search only by name column
	CContactItemFieldDef* fieldDef=new(ELeave) CContactItemFieldDef;
	CleanupStack::PushL(fieldDef);
	fieldDef->AppendL(KUidContactFieldGivenName);
	
	FindAndCheckL(KAta,  2, fieldDef);
	FindAndCheckL(KAra,  1, fieldDef);
	FindAndCheckL(KMalmo,1, fieldDef);

	//Search only by address column
	fieldDef->Delete(0);
	fieldDef->AppendL(KUidContactFieldAddress);
	test(fieldDef->Count() == 1);

//  Uncomment when DBMS collation problem will be sorted out.
	FindAndCheckL(KMalmo,2, fieldDef);
	FindAndCheckL(KAra,  1, fieldDef);
	FindAndCheckL(KAta,1, fieldDef);

	CleanupStack::PopAndDestroy(fieldDef);
	CntTest->CloseDatabase();
	CntTest->DeleteDatabaseL();
	}

GLDEF_C void DoFindL(const TDesC &aTextToFind, CContactItemFieldDef *aFieldDef, TInt aCount)
	{
	CContactIdArray* matchList=CntTest->Db()->FindLC(aTextToFind,aFieldDef);

	const TInt numIds=matchList->Count();
	RDebug::Print(_L("Matched %d contact(s) of %d expected"),numIds,aCount);
	if(numIds!=aCount)
		{
		RDebug::Print(_L("FAILED!"));
		}
	test(numIds==aCount);
	CleanupStack::PopAndDestroy(); // matchList.
	}

GLDEF_C void FindNameInFirstNameFieldL(const TDesC& aTextToFind, const TInt aNumExpectedFinds,TBool aAsync=EFalse)
	{
	RDebug::Print(_L("Find %S in Given name field"),&aTextToFind);	
	CContactItemFieldDef* def=new(ELeave) CContactItemFieldDef();
	CleanupStack::PushL(def);
	def->AppendL(KUidContactFieldGivenName);
	if(!aAsync)
		{
		DoFindL(aTextToFind,def,aNumExpectedFinds);
		}
	else
		{
		DoAsyncFindL(aTextToFind,def,aNumExpectedFinds);
		}
	CleanupStack::PopAndDestroy(); // def.
	}

GLDEF_C void FindNameInLastNameFieldL(const TDesC& aTextToFind, const TInt aNumExpectedFinds,TBool aAsync=EFalse)
	{
	RDebug::Print(_L("Find %S in Family name field"),&aTextToFind);	
	CContactItemFieldDef* def=new(ELeave) CContactItemFieldDef();
	CleanupStack::PushL(def);
	def->AppendL(KUidContactFieldFamilyName);
	if(!aAsync)
		{
		DoFindL(aTextToFind,def,aNumExpectedFinds);
		}	
	else
		{
		DoAsyncFindL(aTextToFind,def,aNumExpectedFinds);
		}
	CleanupStack::PopAndDestroy(); // def.	
	}

GLDEF_C void FindInAllFieldsL(const TDesC& aTextToFind, const TInt aNumExpectedFinds,TBool aAsync=EFalse)
	{
	RDebug::Print(_L("Find %S in All fields"),&aTextToFind);	
	CContactItemFieldDef* def=new(ELeave) CContactItemFieldDef();
	CleanupStack::PushL(def);
	def->AppendL(KUidContactFieldMatchAll);
	if(!aAsync)
		{
		DoFindL(aTextToFind,def,aNumExpectedFinds);
		}
	else
		{
		DoAsyncFindL(aTextToFind,def,aNumExpectedFinds);
		}
	CleanupStack::PopAndDestroy(); // def.
	}

GLDEF_C void FindInCompanyFieldL(const TDesC& aTextToFind, const TInt aNumExpectedFinds,TBool aAsync=EFalse)
	{
	RDebug::Print(_L("Find %S in Company name field"),&aTextToFind);	
	CContactItemFieldDef* def=new(ELeave) CContactItemFieldDef();
	CleanupStack::PushL(def);
	def->AppendL(KUidContactFieldCompanyName);
	if(!aAsync)
		{
		DoFindL(aTextToFind,def,aNumExpectedFinds);
		}
	else
		{
		DoAsyncFindL(aTextToFind,def,aNumExpectedFinds);
		}
	CleanupStack::PopAndDestroy(); // def.
	}

/**
@SYMTestCaseID PIM-T-FIND-INC049017-0001
@SYMTestType CT
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

GLDEF_C void FindUsingKUidContactFieldMatchAllL()
	{
	//create the database
	CntTest->CreateDatabaseL();

	//add the contacts
	//AddFindRecordL(const TDesC &aFirstName, const TDesC &aLastName, const TDesC &aCompanyName, const TDesC &aAddress)
	AddFindRecordL(_L("Robert"),_L("Jordan"),_L("Software Solutions"),_L(" "));
	AddFindRecordL(_L("Amanda"),_L("Jordan"),_L("Software Solutions"),_L(" "));
	AddFindRecordL(_L("Ben"),_L("Turner"),_L("High Res Solutions"),_L(" "));
	AddFindRecordL(_L("Chris"),_L("Andrews"),_L("Soft Kicks"),_L(" "));

	//start of tests for defect where identity fields were not searched when using KUidMatchInAllFields
	test.Next(_L("@SYMTestCaseID:PIM-T-FIND-INC049017-0001 FINDING CONTACTS BY NAME FROM GIVEN NAME FIELD."));
		{
		FindNameInFirstNameFieldL(_L("Chris"),1);
		FindNameInFirstNameFieldL(_L("Turner"),0);
		FindNameInFirstNameFieldL(_L("e"),2);
		}

	test.Next(_L("FINDING CONTACTS BY NAME FROM FAMILY NAME FIELD"));
		{
		FindNameInLastNameFieldL(_L("Turner"),1);
		FindNameInLastNameFieldL(_L("Jordan"),2);
		FindNameInLastNameFieldL(_L("r"),4);
		FindNameInLastNameFieldL(_L("Chris"),0);
		}

	test.Next(_L("FINDING CONTACTS BY NAME FROM ALL FIELDS"));
		{
		FindInAllFieldsL(_L("Ben"),1);
		FindInAllFieldsL(_L("Jordan"),2);
		FindInAllFieldsL(_L("r"),4);
		FindInAllFieldsL(_L("xxxxxxxxx"),0);
		}

	test.Next(_L("FINDING CONTACTS BY COMPANY FROM COMPANY NAME FIELD"));
		{
		FindInCompanyFieldL(_L("Ben"),0);
		FindInCompanyFieldL(_L("High Res Solutions"),1);
		FindInCompanyFieldL(_L("Software Solutions"),2);
		FindInCompanyFieldL(_L("s"),4);
		}

	test.Next(_L("FINDING CONTACTS BY COMPANY FROM  ALL FIELDS"));
		{
		FindInAllFieldsL(_L("Kick"),1);
		FindInAllFieldsL(_L("High Res Solutions"),1);
		FindInAllFieldsL(_L("Software Solutions"),2);
		FindInAllFieldsL(_L("s"),4);
		}
	//end of tests for defect where identity fields were not searched when using KUidMatchInAllFields

	//start of Async tests for defect where identity fields were not searched when using KUidMatchInAllFields
	test.Next(_L("FINDING CONTACTS BY NAME FROM GIVEN NAME FIELD ASYNCHRONOUSLY"));
		{
		FindNameInFirstNameFieldL(_L("Chris"),1,ETrue);
		FindNameInFirstNameFieldL(_L("Turner"),0,ETrue);
		FindNameInFirstNameFieldL(_L("e"),2,ETrue);
		}

	test.Next(_L("FINDING CONTACTS BY NAME FROM FAMILY NAME FIELD ASYNCHRONOUSLY"));
		{
		FindNameInLastNameFieldL(_L("Turner"),1,ETrue);
		FindNameInLastNameFieldL(_L("Jordan"),2,ETrue);
		FindNameInLastNameFieldL(_L("r"),4,ETrue);
		FindNameInLastNameFieldL(_L("Chris"),0,ETrue);
		}

	test.Next(_L("FINDING CONTACTS BY NAME FROM ALL FIELDS ASYNCHRONOUSLY"));
		{
		FindInAllFieldsL(_L("Ben"),1,ETrue);
		FindInAllFieldsL(_L("Jordan"),2,ETrue);
		FindInAllFieldsL(_L("r"),4,ETrue);
		FindInAllFieldsL(_L("xxxxxxxxx"),0,ETrue);
		}

	test.Next(_L("FINDING CONTACTS BY COMPANY FROM COMPANY NAME FIELD ASYNCHRONOUSLY"));
		{
		FindInCompanyFieldL(_L("Ben"),0,ETrue);
		FindInCompanyFieldL(_L("High Res Solutions"),1,ETrue);
		FindInCompanyFieldL(_L("Software Solutions"),2,ETrue);
		FindInCompanyFieldL(_L("s"),4,ETrue);
		}

	test.Next(_L("FINDING CONTACTS BY COMPANY FROM  ALL FIELDS ASYNCHRONOUSLY"));
		{
		FindInAllFieldsL(_L("Kick"),1,ETrue);
		FindInAllFieldsL(_L("High Res Solutions"),1,ETrue);
		FindInAllFieldsL(_L("Software Solutions"),2,ETrue);
		FindInAllFieldsL(_L("s"),4,ETrue);
		}
	//end of Async tests for defect where identity fields were not searched when using KUidMatchInAllFields

	CntTest->CloseDatabase();
	CntTest->DeleteDatabaseL();
	}
