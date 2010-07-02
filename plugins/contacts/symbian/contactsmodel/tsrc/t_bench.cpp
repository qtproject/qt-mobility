/*
* Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32std.h>
#include <e32test.h>
#include <e32math.h>
#include <cntdb.h>
#include <cntitem.h>
#include <cntfldst.h>
#include "t_utils2.h"
#include "t_bench.h"
#include "t_utils.h"


//
// Configuration.
//

#define __TxEST
#define __VxERBOSE


//
// Constants.
//

_LIT(KTestName,"t_bench");


_LIT(KDbFileName,"c:contacts.cdb");

_LIT(KLogFileName,"t_bench.log");
_LIT(KBackSpace,"\x08");
_LIT(KVisualCounterFormat,"%S%d");
_LIT(KTimeProfileFormat,"Time taken (secs)");
_LIT(KSystemMemoryFormat,"System memory used (Kb)");
_LIT(KThreadMemoryFormat,"This thread memory used (Kb)");
_LIT(KFileSizeText,"File size (bytes)");
_LIT(KAverageTimeText,"Average time taken (secs)");
_LIT(KResultFormatTInt,"Result #%d.%d %S [%d]");
_LIT(KResultFormatTReal,"Result #%d.%d %S [%f]");
_LIT(KThreadNameFormat,"T_BenchCli%2d");
_LIT(KPhoneMatchFail,"6666666666666666");
_LIT(KNameMatchFail,"xxxxxxxxxxxxxxxx");
_LIT(KAsyncFindMatch,"a");
_LIT(KNewLine,"\n");
_LIT(KTextDefSeparator,"\t");

#ifdef __VERBOSE
_LIT(KContactSummaryFormat,"%d\t%S\t%S\t%S");
_LIT(KAscendingSortOrder,"ascending");
_LIT(KDescendingSortOrder,"descending");
_LIT(KProfileResult,"Took %d\tmicrosecs for %d iteration(s)");
#endif

const TInt KNumSortClients=10;
const TInt KClientStackSize=KDefaultStackSize;
const TInt KClientHeapSize=0x20000;

#ifdef __TEST
const TInt KNumTypicalContacts=10;
const TInt KNumAtypicalContacts=2;
const TInt KNumTypicalContactsCompact=0;
const TInt KNumAtypicalContactsCompact=0;
#else
const TInt KNumTypicalContacts=100;
const TInt KNumAtypicalContacts=50;
const TInt KNumTypicalContactsCompact=150;
const TInt KNumAtypicalContactsCompact=50;
#endif // __TEST


//
// CBenchMarker.
//

CBenchMarker::CBenchMarker() : iTest(KTestName),iVisualCounter(-1),iNumTypicalContacts(-1),iNumAtypicalContacts(-1),iNumTypicalContactsCompact(-1),iNumAtypicalContactsCompact(-1)
	{
	}

CBenchMarker::~CBenchMarker()
	{
	delete iGenerator;
	delete iDb;
	delete iTextDef;
	delete iLog;
	iTest.Close();
	iFs.Close();
	}

void CBenchMarker::ConstructL(const TDesC& aCommandLine)
	{
	User::LeaveIfError(iFs.Connect());
	DecodeCommandLineL(aCommandLine);
	iLog=CLog::NewL(iTest,iLogFileName);
	iGenerator=CRandomContactGenerator::NewL();
	}

void CBenchMarker::DecodeCommandLineL(const TDesC& aCommandLine)
	{
	_LIT(KArgContactsToAdd,"-a");
	_LIT(KArgContactsToCompact,"-c");
	_LIT(KArgDatabaseFileName,"-d");
	_LIT(KArgNoDataGenerationTests,"-g");
	_LIT(KArgLogFile,"-l");
	_LIT(KArgHelp,"-h");
	
	TLex cl(aCommandLine);
	while (cl.Remainder().Length()>0)
		{
		TPtrC token=cl.NextToken();
		
		if (token.CompareF(KArgContactsToAdd)==0)
			{
			cl.SkipSpace();
			User::LeaveIfError(cl.Val(iNumTypicalContacts));
			cl.SkipSpace();
			User::LeaveIfError(cl.Val(iNumAtypicalContacts));
			}
		else if (token.CompareF(KArgContactsToCompact)==0)
			{
			cl.SkipSpace();
			User::LeaveIfError(cl.Val(iNumTypicalContactsCompact));
			cl.SkipSpace();
			User::LeaveIfError(cl.Val(iNumAtypicalContactsCompact));
			}
		else if (token.CompareF(KArgDatabaseFileName)==0)
			{
			iDbFileName=cl.NextToken();
			}
		else if (token.CompareF(KArgNoDataGenerationTests)==0)
			{
			iNoDataGenerationTests=ETrue;
			}
		else if (token.CompareF(KArgLogFile)==0)
			{
			iLogFileName=cl.NextToken();
			}
		else if (token.CompareF(KArgHelp)==0)
			{
			iTest.Printf(_L("Usage t_bench [options]\n\n"));
			iTest.Printf(_L("-a <num_typical> <num_atypical> (default: %d %d)\n"),KNumTypicalContacts,KNumAtypicalContacts);
			iTest.Printf(_L("-c <num_typical_comp> <num_atypical_comp> (default: %d %d)\n"),KNumTypicalContactsCompact,KNumAtypicalContactsCompact);
			iTest.Printf(_L("-d <db_name> (default: %S)\n"),&KDbFileName);
			iTest.Printf(_L("-l <log_name> (default: %S)\n"),&KLogFileName);
			iTest.Printf(_L("-g skip data generation tests\n"));
			iTest.Printf(_L("\n<hit any key>"));
			iTest.Getch();
			User::Leave(KErrNone);
			}
		}

	if (iDbFileName.Length()==0)
		{
		iDbFileName=KDbFileName;
		}

	if (iLogFileName.Length()==0)
		{
		iLogFileName=KLogFileName;
		}

	if (iNoDataGenerationTests)
		{
		TUint att;
		if (iFs.Att(iDbFileName,att)==KErrNotFound)
			{
			iTest.Printf(_L("-g switch ignored, because %S not found\n"),&iDbFileName);
			iNoDataGenerationTests=EFalse;
			}
		}

	if (iNumTypicalContacts<0)
		{
		iNumTypicalContacts=KNumTypicalContacts;
		}

	if (iNumAtypicalContacts<0)
		{
		iNumAtypicalContacts=KNumAtypicalContacts;
		}

	if (iNumTypicalContactsCompact<0)
		{
		iNumTypicalContactsCompact=KNumTypicalContactsCompact;
		}

	if (iNumAtypicalContactsCompact<0)
		{
		iNumAtypicalContactsCompact=KNumAtypicalContactsCompact;
		}
	}

void CBenchMarker::RunL()
	{
	iTest.Printf(_L("Starting CntModel benchmarks...\n")); // This forces the console to get loaded under WINS - would otherwise scew profiles.

	for (TInt i=0;i<ENumTests;++i)
		{
		StartProfile();
		TInt timeMinorNumber=0;
		TRAPD(err,timeMinorNumber=DoTestL(TTest(i)));
		if (err)
			{
			iLog->LogLine(_L("Test %d left with %d"),i,err);
			break;
			}

		LogResult(i,timeMinorNumber,KTimeProfileFormat,EndProfile());

		User::CompressAllHeaps();
		TMemoryInfoV1Buf memoryBuf;
		UserHal::MemoryInfo(memoryBuf);
		TMemoryInfoV1 memory(memoryBuf());
		const TInt totalK=memory.iTotalRamInBytes>>10;
		const TInt freeK=memory.iFreeRamInBytes>>10;
		const TInt usedK=totalK-freeK;
		LogResult(i,timeMinorNumber+1,KSystemMemoryFormat,usedK);
		
		TInt allocSize;
		User::Heap().AllocSize(allocSize);;
		allocSize>>=10;
		LogResult(i,timeMinorNumber+2,KThreadMemoryFormat,allocSize);
		}

	iTest.Printf(_L("Dump log to comm port? (y/n) "));
	TKeyCode key=iTest.Getch();
	if (key=='y' || key=='Y')
		{
writelog:
		iLog->WriteLogToCommPortL();
		}
	iTest.Printf(_L("\nAgain? (y/n)"));
	key=iTest.Getch();
	if (key=='y' || key=='Y')
		{
		goto writelog;
		}
	}

TInt CBenchMarker::DoTestL(TTest aTest)
	{
	TInt timeMinorNumber=0;
	iLog->Log(_L("Test   #%d   "),aTest);
	TPtrC match;

	switch (aTest)
		{
		case EOpenNewDb:
			iLog->LogLine(_L("Open new database"));
			if (iNoDataGenerationTests)
				{
				iDb=CContactDatabase::OpenL(iDbFileName);
				}
			else
				{
				iDb=CContactDatabase::ReplaceL(iDbFileName);
				LogResult(aTest,0,KFileSizeText,DbFileSize());
				timeMinorNumber=1;
				}
			iGenerator->SetDbL(*iDb);
			break;
		case ECreateSecondaryClients:
			iLog->LogLine(_L("Create %d secondary client threads"),KNumSecondaryClients);
			CreateSecondaryClients();
			break;
		case EAddContacts:
			iLog->LogLine(_L("Add %d typical and %d atypical contacts"),iNumTypicalContacts,iNumAtypicalContacts);
			if (!iNoDataGenerationTests)
				{
				LogResult(aTest,0,KAverageTimeText,AddContactsL());
				LogResult(aTest,1,KFileSizeText,DbFileSize());
				timeMinorNumber=2;
				}
			break;
		case ECompactDb:
			iLog->LogLine(_L("Compact database"));
			if (!iNoDataGenerationTests)
				{
				iDb->CompactL();
				LogResult(aTest,0,KFileSizeText,DbFileSize());
				timeMinorNumber=1;
				}
			break;
		case EAddAndCompact:
			iLog->LogLine(_L("Add %d typical and %d atypical contacts (compact after each)"),iNumTypicalContactsCompact,iNumAtypicalContactsCompact);
			if (!iNoDataGenerationTests)
				{
				LogResult(aTest,0,KAverageTimeText,AddContactsCompactL());
				LogResult(aTest,1,KFileSizeText,DbFileSize());
				timeMinorNumber=2;
				}
			break;
		case EGetSortedItems:
			iLog->LogLine(_L("Get sorted id array by firstname, lastname, company name"));
			DoSortL();
			break;
		case ELogContactSummary:
			iLog->LogLine(_L("Log contact summary"));
			LogContactSummaryL(EFalse);
			break;
		case ELogContactSummaryFast:
			iLog->LogLine(_L("Log contact summary using TContactTextDefItem"));
			LogContactSummaryL(ETrue);
			break;
		case EOpenRContactView:
			iLog->LogLine(_L("Open RContactView"));
			OpenRContactViewL();
			break;
		case ESetSortOrderOfRContactView:
			iLog->LogLine(_L("Set order of RContactView"));
			SetSortOrderOfRContactViewL();
			break;
		case EGetSortOrderOfRContactView:
			iLog->LogLine(_L("Get order of RContactView"));
			GetSortOrderOfRContactViewL();
			break;
		case ELogContactSummaryWithRContactView:
			iLog->LogLine(_L("Log contact summary using RContactView"));
			LogContactSummaryFromRContactViewL(EFalse);
			break;
		case ELogContactSummaryFastWithRContactView:
			iLog->LogLine(_L("Log contact summary using RContactView and TContactTextDefItem"));
			LogContactSummaryFromRContactViewL(ETrue);
			break;
		case EFindWithRContactView:
			iLog->LogLine(_L("Find TContactItemId from RContactView"));
			FindInRContactViewL();
			break;
		case ECloseRContactView:
			iLog->LogLine(_L("Close RContactView"));
#ifdef __USE_NEW_INTERFACES
			iSortedIdList.Close();
#endif
			break;
		case EFindFromLargeFieldSetSuceed:
			match.Set(iGenerator->NameMatch());
			iLog->LogLine(_L("FindLC %S using large field set (will suceed)"),&match);
			FindFromLargeFieldSetL(match);
			break;
		case EFindFromLargeFieldSetFail:
			iLog->LogLine(_L("FindLC %S using large field set (will fail)"),&KNameMatchFail);
			FindFromLargeFieldSetL(KNameMatchFail);
			break;
		case EFindFromSmallFieldSetSuceed:
			match.Set(iGenerator->NameMatch());
			iLog->LogLine(_L("FindLC %S using small field set (will suceed)"),&match);
			FindFromSmallFieldSetL(match);
			break;
		case EFindFromSmallFieldSetFail:
			iLog->LogLine(_L("FindLC %S using small field set (will fail)"),&KNameMatchFail);
			FindFromSmallFieldSetL(KNameMatchFail);
			break;
		case EFindPhoneNumberSuceed:
			match.Set(iGenerator->PhoneMatch());
			iLog->LogLine(_L("FindLC %S using just phone field (will suceed)"),&match);
			FindPhoneNumberL(match);
			break;
		case EFindPhoneNumberFail:
			iLog->LogLine(_L("FindLC %S using just phone field (will fail)"),&KPhoneMatchFail);
			FindPhoneNumberL(KPhoneMatchFail);
			break;
		case EFindEmailAddressSuceed:
			match.Set(iGenerator->EmailMatch());
			iLog->LogLine(_L("FindLC %S using just email field (will suceed)"),&match);
			FindEmailAddressL(match);
			break;
		case EFindEmailAddressFail:
			iLog->LogLine(_L("FindLC %S using just email field (will fail)"),&KNameMatchFail);
			FindEmailAddressL(KNameMatchFail);
			break;
		case EFindAsyncFromLargeFieldSet:
			iLog->LogLine(_L("FindAsyncL %S using large field set"),&KAsyncFindMatch);
			FindAsyncFromLargeFieldSetL(KAsyncFindMatch);
			break;
		case EFindAsyncFromSmallFieldSet:
			iLog->LogLine(_L("FindAsyncL %S using small field set"),&KAsyncFindMatch);
			FindAsyncFromSmallFieldSetL(KAsyncFindMatch);
			break;
		case ECloseDb:
			iLog->LogLine(_L("Close database"));
			delete iDb;
			iDb=NULL;
			break;
		case ECloseSecondaryClients:
			iLog->LogLine(_L("Close secondary clients"));
			CloseSecondaryClients();
			break;
		case EOpenExistingDb:
			iLog->LogLine(_L("Open existing database"));
			iDb=CContactDatabase::OpenL(iDbFileName);
			break;
		case EMultiClientSort:
			iLog->LogLine(_L("%d client simultaneous sort"),KNumSortClients);
			CreateSortClients();
			break;
		case ENumTests:
			ASSERT(EFalse);
			break;
		}

	return timeMinorNumber;
	}

void CBenchMarker::StartProfile()
	{
	CCntTest::ProfileReset(0,1); // reset profiles 0-1
	CCntTest::ProfileStart(0);
	}

TReal CBenchMarker::EndProfile()
	{
	CCntTest::ProfileEnd(0);
	TCntProfile profile[1];
	CCntTest::ProfileResult(profile,0,1);
	return TReal(profile[0].iTime/1000000.0);
	}

void CBenchMarker::StartAverageProfile()
	{
	iAverageProfileCounter=0;
	iNumAverageProfiles=0;
	CCntTest::ProfileReset(1,1);
	CCntTest::ProfileStart(1);
	}

void CBenchMarker::UpdateAverageProfile(TInt aNumIterations)
	{
	CCntTest::ProfileEnd(1);
	TCntProfile profile[1];
	CCntTest::ProfileResult(profile,1,1);
	iAverageProfileCounter+=profile[0].iTime;
	iNumAverageProfiles+=aNumIterations;
	CCntTest::ProfileReset(1,1);
	CCntTest::ProfileStart(1);
#ifdef __VERBOSE
	iLog->LogLineNoEcho(KProfileResult,profile[0].iTime,aNumIterations);	
#endif
	}

TReal CBenchMarker::EndAverageProfile()
	{
	CCntTest::ProfileEnd(1);
	if (iNumAverageProfiles>0)
		{
		return TReal((iAverageProfileCounter/iNumAverageProfiles)/1000000.0);
		}

	return TReal(0.0);
	}

TReal CBenchMarker::AddContactsL()
	{
	iTest.Printf(_L("Adding "));
	StartAverageProfile();

	TInt ii;
	for (ii=0;ii<iNumTypicalContacts;++ii)
		{
		IncVisualCounter();
		iGenerator->AddTypicalRandomContactL();
		UpdateAverageProfile(1);
		}

	for (ii=0;ii<iNumAtypicalContacts;++ii)
		{
		IncVisualCounter();
		iGenerator->AddAtypicalRandomContactL();
		UpdateAverageProfile(1);
		}

	EndVisualCounter();
	return EndAverageProfile();
	}

TReal CBenchMarker::AddContactsCompactL()
	{
	iTest.Printf(_L("Adding "));
	StartAverageProfile();

	TInt ii;
	for (ii=0;ii<iNumTypicalContactsCompact;++ii)
		{
		IncVisualCounter();
		iGenerator->AddTypicalRandomContactL();
		iDb->CompactL();
		UpdateAverageProfile(1);
		}

	for (ii=0;ii<iNumAtypicalContactsCompact;++ii)
		{
		IncVisualCounter();
		iGenerator->AddAtypicalRandomContactL();
		iDb->CompactL();
		UpdateAverageProfile(1);
		}

	EndVisualCounter();
	return EndAverageProfile();
	}

#ifdef __USE_NEW_INTERFACES
void CBenchMarker::LogContactSummaryFromRContactViewL(TBool aFaster)
	{
	iTest.Printf(_L("Logging "));
	const TInt numContacts(iSortedIdList.Count());
	for (TInt ii=0;ii<numContacts;++ii)
		{
		IncVisualCounter();
		LogContactSummaryL(aFaster,iSortedIdList.GetL(ii));
		}
	EndVisualCounter();
	}
#else
void CBenchMarker::LogContactSummaryFromRContactViewL(TBool)
	{
	}
#endif

void CBenchMarker::OpenRContactViewL()
	{
#ifdef __USE_NEW_INTERFACES
	User::LeaveIfError(iSortedIdList.Open(*iDb));
#endif
	}

void CBenchMarker::SetSortOrderOfRContactViewL()
	{
#ifdef __USE_NEW_INTERFACES
	CArrayFix<CContactDatabase::TSortPref>* sortOrder=new(ELeave)CArrayFixFlat<CContactDatabase::TSortPref>(2);
	CleanupStack::PushL(sortOrder);
	sortOrder->AppendL(CContactDatabase::TSortPref(KUidContactFieldGivenName));
	sortOrder->AppendL(CContactDatabase::TSortPref(KUidContactFieldFamilyName));
	sortOrder->AppendL(CContactDatabase::TSortPref(KUidContactFieldCompanyName));
	iSortedIdList.ChangeSortOrderL(*sortOrder);
	CleanupStack::PopAndDestroy(); // sortOrder
#endif
	}

void CBenchMarker::GetSortOrderOfRContactViewL()
	{
#ifdef __USE_NEW_INTERFACES
	const CArrayFix<CContactDatabase::TSortPref>& sortOrder=iSortedIdList.SortOrderLC();
#ifdef __VERBOSE
	const TInt numSortPrefs=sortOrder.Count();
	for (TInt ii=0;ii<numSortPrefs;++ii)
		{
		iLog->LogLineNoEcho(_L("Sort pref %d field type: %x order: %S"),ii,sortOrder[ii].iFieldType,(sortOrder[ii].iOrder==CContactDatabase::TSortPref::EAsc) ? &KAscendingSortOrder : &KDescendingSortOrder);
		}
#else
	sortOrder.Count();
#endif
	CleanupStack::PopAndDestroy(); // sortOrder
#endif
	}

void CBenchMarker::FindInRContactViewL()
	{
#ifdef __USE_NEW_INTERFACES
	const TContactItemId id=iSortedIdList.GetL(iSortedIdList.Count()-1);
	const TInt index=iSortedIdList.Find(id);
	ASSERT(index==iSortedIdList.Count()-1);
	iLog->LogLine(_L("Found TContactItemId %d for index %d"),id,index);
#endif
	}

void CBenchMarker::LogContactSummaryL(TBool aFaster)
	{
	iTest.Printf(_L("Logging "));
	const TInt numContacts(iSortedItems->Count());
	for (TInt ii=0;ii<numContacts;++ii)
		{
		IncVisualCounter();
		LogContactSummaryL(aFaster,(*iSortedItems)[ii]);
		}
	EndVisualCounter();
	}

void CBenchMarker::LogContactSummaryL(TBool aFaster,TContactItemId aContactItemId)
	{
	if (aFaster)
		{
		if (iTextDef==NULL)
			{
			iTextDef=CContactTextDef::NewL();
			iTextDef->AppendL(TContactTextDefItem(KUidContactFieldGivenName,KTextDefSeparator));
			iTextDef->AppendL(TContactTextDefItem(KUidContactFieldFamilyName,KTextDefSeparator));
			iTextDef->AppendL(TContactTextDefItem(KUidContactFieldCompanyName,KTextDefSeparator));
			}

		TBuf<128> buf;
		iDb->ReadContactTextDefL(aContactItemId,buf,iTextDef);
#ifdef __VERBOSE
		iLog->LogLineNoEcho(buf);
#endif
		}
	else
		{
		CContactItem* thisContactItem=iDb->ReadContactLC(aContactItemId);
		CTestContact* thisContact=CTestContact::NewLC(*thisContactItem);

#ifdef __VERBOSE
		const TPtrC firstName(thisContact->FirstNameL());
		const TPtrC lastName(thisContact->LastNameL());
		const TPtrC companyName(thisContact->CompanyNameL());
		iLog->LogLineNoEcho(KContactSummaryFormat,thisContactItem->Id(),&firstName,&lastName,&companyName);
#else
		thisContact->FirstNameL();
		thisContact->LastNameL();
		thisContact->CompanyNameL();
#endif
		CleanupStack::PopAndDestroy(2); // thisContact, thisContactItem.
		}
	}


void CBenchMarker::DoSortL()
	{
	CContactTextDef* textDef=CContactTextDef::NewLC();
	textDef->AppendL(TContactTextDefItem(KUidContactFieldGivenName));
	textDef->AppendL(TContactTextDefItem(KUidContactFieldFamilyName));
	textDef->AppendL(TContactTextDefItem(KUidContactFieldCompanyName));
	iDb->SetTextDefinitionL(textDef); // Takes ownership.
	CleanupStack::Pop(); // textDef.
	CArrayFix<CContactDatabase::TSortPref>* sortOrder=new(ELeave)CArrayFixFlat<CContactDatabase::TSortPref>(2);
	CleanupStack::PushL(sortOrder);
	sortOrder->AppendL(CContactDatabase::TSortPref(KUidContactFieldDefinedText));
	iDb->SortL(sortOrder); // Takes ownership.
	CleanupStack::Pop(); // sortOrder
	iSortedItems=iDb->SortedItemsL();
	}

void CBenchMarker::FindFromLargeFieldSetL(const TDesC& aTextToFind)
	{
	CContactItemFieldDef* def=new(ELeave) CContactItemFieldDef();
	CleanupStack::PushL(def);
	def->AppendL(KUidContactFieldGivenName);
	def->AppendL(KUidContactFieldFamilyName);
	def->AppendL(KUidContactFieldCompanyName);
	def->AppendL(KUidContactFieldPhoneNumber);
	def->AppendL(KUidContactFieldFax);
	def->AppendL(KUidContactFieldEMail);
	def->AppendL(KUidContactFieldUrl);
	def->AppendL(KUidContactFieldAddress);
	def->AppendL(KUidContactFieldLocality);
	def->AppendL(KUidContactFieldRegion);
	def->AppendL(KUidContactFieldPostcode);
	def->AppendL(KUidContactFieldCountry);
	def->AppendL(KUidContactFieldNote);
	DoFindL(aTextToFind,*def);
	CleanupStack::PopAndDestroy(); // def.
	}

void CBenchMarker::FindFromSmallFieldSetL(const TDesC& aTextToFind)
	{
	CContactItemFieldDef* def=new(ELeave) CContactItemFieldDef();
	CleanupStack::PushL(def);
	def->AppendL(KUidContactFieldGivenName);
	def->AppendL(KUidContactFieldFamilyName);
	def->AppendL(KUidContactFieldCompanyName);
	DoFindL(aTextToFind,*def);
	CleanupStack::PopAndDestroy(); // def.
	}

void CBenchMarker::FindPhoneNumberL(const TDesC& aTextToFind)
	{
	CContactItemFieldDef* def=new(ELeave) CContactItemFieldDef();
	CleanupStack::PushL(def);
	def->AppendL(KUidContactFieldPhoneNumber);
	DoFindL(aTextToFind,*def);
	CleanupStack::PopAndDestroy(); // def.
	}

void CBenchMarker::FindEmailAddressL(const TDesC& aTextToFind)
	{
	CContactItemFieldDef* def=new(ELeave) CContactItemFieldDef();
	CleanupStack::PushL(def);
	def->AppendL(KUidContactFieldEMail);
	DoFindL(aTextToFind,*def);
	CleanupStack::PopAndDestroy(); // def.
	}

TReal CBenchMarker::FindAsyncFromLargeFieldSetL(const TDesC& aTextToFind)
	{
	CContactItemFieldDef* def=new(ELeave) CContactItemFieldDef();
	CleanupStack::PushL(def);
	def->AppendL(KUidContactFieldGivenName);
	def->AppendL(KUidContactFieldFamilyName);
	def->AppendL(KUidContactFieldCompanyName);
	def->AppendL(KUidContactFieldPhoneNumber);
	def->AppendL(KUidContactFieldFax);
	def->AppendL(KUidContactFieldEMail);
	def->AppendL(KUidContactFieldUrl);
	def->AppendL(KUidContactFieldAddress);
	def->AppendL(KUidContactFieldLocality);
	def->AppendL(KUidContactFieldRegion);
	def->AppendL(KUidContactFieldPostcode);
	def->AppendL(KUidContactFieldCountry);
	def->AppendL(KUidContactFieldNote);
	TReal averageTime=DoFindAsyncL(aTextToFind,*def);
	CleanupStack::PopAndDestroy(); // def.
	return averageTime;
	}

TReal CBenchMarker::FindAsyncFromSmallFieldSetL(const TDesC& aTextToFind)
	{
	CContactItemFieldDef* def=new(ELeave) CContactItemFieldDef();
	CleanupStack::PushL(def);
	def->AppendL(KUidContactFieldGivenName);
	def->AppendL(KUidContactFieldFamilyName);
	def->AppendL(KUidContactFieldCompanyName);
	TReal averageTime=DoFindAsyncL(aTextToFind,*def);
	CleanupStack::PopAndDestroy(); // def.
	return averageTime;
	}

void CBenchMarker::DoFindL(const TDesC& aTextToFind,const CContactItemFieldDef& aFieldDef)
	{
	CContactIdArray* matchList=iDb->FindLC(aTextToFind,&aFieldDef);

#ifdef __VERBOSE
	const TInt numIds=matchList->Count();
	iLog->LogLine(_L("Matched %d contact(s)"),numIds);
	for (TInt ii=0;ii<numIds;++ii)
		{
		LogContactSummaryL(ETrue,(*matchList)[ii]);
		}
#else
	matchList->Count();
#endif

	CleanupStack::PopAndDestroy(); // matchList.
	}

TReal CBenchMarker::DoFindAsyncL(const TDesC& aTextToFind,const CContactItemFieldDef& aFieldDef)
	{
	iTest.Printf(_L("Found "));
	StartAverageProfile();
	iIdleFinder=iDb->FindAsyncL(aTextToFind,&aFieldDef,this);
	CleanupStack::PushL(iIdleFinder); // Handle on cleanup stack because object is very temporary.
	CActiveScheduler::Start();

	EndVisualCounter();
#ifdef __VERBOSE
	CContactIdArray* matchList=iIdleFinder->TakeContactIds();
	CleanupStack::PushL(matchList);
	const TInt numIds=matchList->Count();
	iLog->LogLine(_L("Matched %d contact(s)"),numIds);
	for (TInt ii=0;ii<numIds;++ii)
		{
		LogContactSummaryL(ETrue,(*matchList)[ii]);
		}
	CleanupStack::PopAndDestroy(); // matchList.
#endif
	
	CleanupStack::PopAndDestroy(); // iIdleFinder.
	return EndAverageProfile();
	}

void CBenchMarker::IdleFindCallback()
	{
	IncVisualCounter();
	UpdateAverageProfile(1);
	if (iIdleFinder->IsComplete())
		{
		CActiveScheduler::Stop();
		}
	}

#ifndef __USE_NEW_INTERFACES
GLDEF_C void DoSortL(CContactDatabase& aDb)
	{
	CContactTextDef* textDef=CContactTextDef::NewLC();
	textDef->AppendL(TContactTextDefItem(KUidContactFieldGivenName));
	textDef->AppendL(TContactTextDefItem(KUidContactFieldFamilyName));
	textDef->AppendL(TContactTextDefItem(KUidContactFieldCompanyName));
	aDb.SetTextDefinitionL(textDef); // Takes ownership.
	CleanupStack::Pop(); // textDef.
	CArrayFix<CContactDatabase::TSortPref>* sortOrder=new(ELeave)CArrayFixFlat<CContactDatabase::TSortPref>(2);
	CleanupStack::PushL(sortOrder);
	sortOrder->AppendL(CContactDatabase::TSortPref(KUidContactFieldDefinedText));
	aDb.SortL(sortOrder); // Takes ownership.
	CleanupStack::Pop(); // sortOrder
	}
#endif

GLDEF_C TInt T_BenchClientThreadStart(TAny* aPtr)
	{
	TThreadCommandLine* comLine=STATIC_CAST(TThreadCommandLine*, aPtr);
	RThread owningThread=comLine->iOwningThread;
	TRequestStatus* initialStatus=comLine->iInitialStatus;
	TRequestStatus* completionStatus=comLine->iCompletionStatus;
	TFileName dbFileName=comLine->iDbFileName;
	if (comLine->iExitAfterSort)
		{
		owningThread.RequestComplete(initialStatus,KErrNone);
		}

	__UHEAP_MARK;
	TInt err;
	CActiveScheduler* scheduler=new CActiveScheduler;
	if (scheduler)
		{
		CActiveScheduler::Install(scheduler);
		CTrapCleanup* cleanup=CTrapCleanup::New();
		if (cleanup)
			{
			CContactDatabase* db=NULL;
#ifdef __USE_NEW_INTERFACES
			RContactView sortedIdList;
			TRAP(err,
				db=CContactDatabase::OpenL(dbFileName);
				User::LeaveIfError(sortedIdList.Open(*db));
				);
#else
			TRAP(err,
				db=CContactDatabase::OpenL(dbFileName);
				DoSortL(*db);
				);
#endif
			if (!comLine->iExitAfterSort)
				{
				owningThread.RequestComplete(initialStatus,KErrNone);
				CActiveScheduler::Start();
				}
#ifdef __USE_NEW_INTERFACES
			sortedIdList.Close();
#endif
			delete db;
			delete cleanup;
			}
		else
			{
			err=KErrNoMemory;
			}
		delete scheduler;
		}
	else
		{
		err=KErrNoMemory;
		}
	__UHEAP_MARKEND;

	if (comLine->iExitAfterSort)
		{
		owningThread.RequestComplete(completionStatus,err);
		}
	owningThread.Close();
	return KErrNone;
	}

void CBenchMarker::CreateSecondaryClients()
	{
	// Create a set of client threads.
	for (TInt ii=0;ii<KNumSecondaryClients;++ii)
		{
		TRequestStatus initialStatus(KRequestPending);
		TThreadCommandLine comLine(&initialStatus,NULL,iDbFileName,EFalse);
		TBuf<32> threadName;
		threadName.Format(KThreadNameFormat,ii);
		secondaryClient[ii].Create(threadName,T_BenchClientThreadStart,KClientStackSize,KClientHeapSize,KClientHeapSize,&comLine);
		comLine.iOwningThread.Duplicate(secondaryClient[ii]);
		secondaryClient[ii].Resume();
		User::WaitForRequest(initialStatus);
		}
	}

void CBenchMarker::CloseSecondaryClients()
	{
	for (TInt ii=0;ii<KNumSecondaryClients;++ii)
		{
		secondaryClient[ii].Kill(KErrNone);
		secondaryClient[ii].Close();
		}
	}

void CBenchMarker::CreateSortClients()
	{
	TRequestStatus completionStatus[KNumSortClients];

	// Create a set of client threads.
	for (TInt ii=0;ii<KNumSortClients;++ii)
		{
		completionStatus[ii]=KRequestPending;
		TRequestStatus initialStatus(KRequestPending);
		TThreadCommandLine comLine(&initialStatus,&(completionStatus[ii]),iDbFileName,ETrue);
		RThread thread;
		TBuf<32> threadName;
		threadName.Format(KThreadNameFormat,ii);
		thread.Create(threadName,T_BenchClientThreadStart,KClientStackSize,KClientHeapSize,KClientHeapSize,&comLine);
		comLine.iOwningThread.Duplicate(thread);
		thread.Resume();
		thread.Close();
		User::WaitForRequest(initialStatus);
		}

	// Wait for all the clients to finish.
	TBool moreClientsToComplete;
	TInt numWaits=-1;
	do
		{
		++numWaits;
		User::WaitForAnyRequest();
		moreClientsToComplete=EFalse;
		for (TInt ii=0;ii<KNumSortClients;++ii)
			{
			if (completionStatus[ii].Int()==KRequestPending)
				{
				moreClientsToComplete=ETrue;
				break;
				}
			}
		}
		while (moreClientsToComplete);

	while (numWaits<(KNumSortClients-1))
		{
		++numWaits;
		User::WaitForAnyRequest();
		}
	}

void CBenchMarker::IncVisualCounter()
	{
	TBuf<8> backSpaceBuf;

	if (iVisualCounter>=0)
		{
		TInt numDigits=1;
		TInt divisor=10;

		FOREVER
			{
			if (iVisualCounter/divisor>0)
				{
				divisor*=10;
				++numDigits;
				}
			else
				{
				break;
				}
			}

		for (TInt ii=0;ii<numDigits;++ii)
			{
			backSpaceBuf.Append(KBackSpace);
			}
		}

	iTest.Printf(KVisualCounterFormat,&backSpaceBuf,++iVisualCounter);
	}

void CBenchMarker::EndVisualCounter()
	{
	iTest.Printf(KNewLine);
	iVisualCounter=-1;
	}

TInt CBenchMarker::DbFileSize()
	{
	TEntry dbEntry;
	TInt err=iFs.Entry(iDbFileName,dbEntry);
	if (err==KErrNone)
		{
		return dbEntry.iSize;
		}

	return err;
	}

void CBenchMarker::LogResult(TInt aMajorTestNum,TInt aMinorTestNum,const TDesC& aDescription,TInt aResult)
	{
	iLog->LogLine(KResultFormatTInt,aMajorTestNum,aMinorTestNum,&aDescription,aResult);
	}

void CBenchMarker::LogResult(TInt aMajorTestNum,TInt aMinorTestNum,const TDesC& aDescription,TReal aResult)
	{
	iLog->LogLine(KResultFormatTReal,aMajorTestNum,aMinorTestNum,&aDescription,aResult);
	}


//
// Main.
//

GLDEF_C TInt E32Main()
	{
	RDebug::Print(_L("t_bench started"));
	__UHEAP_MARK;
	TBuf<256> commandLine;
	User::CommandLine(commandLine);
	
	CActiveScheduler* scheduler=new CActiveScheduler;
	if (scheduler)
		{
		CActiveScheduler::Install(scheduler);
		CTrapCleanup* cleanup=CTrapCleanup::New();
		if (cleanup)
			{
			CBenchMarker* benchMarker=new CBenchMarker();
			if (benchMarker)
				{
				TRAPD(err,
					benchMarker->ConstructL(commandLine);
					benchMarker->RunL();
					);
				RDebug::Print(_L("t_bench finish with %d error"),err);
				delete benchMarker;
				}
			delete cleanup;
			}
		delete scheduler;
		}
	__UHEAP_MARKEND;
	return KErrNone;
    }
