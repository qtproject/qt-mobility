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
#include "t_utils.h"

CCntTest* CntTest=NULL;
LOCAL_D RTest test(_L("T_TIME"));

LOCAL_D TTimeIntervalMicroSeconds TheTestTime;
LOCAL_D TTime TheClock;

enum
	{
	KTimeTestExportImportDifferentMachineIds,
	KTimeTestVCardExport,
	KTimeTestVCardImport,
	KTimeTestVCardExportToFile1by1,
	KTimeTestVCardImportFromFile1by1,
	KTimeTestReadContacts,
	KTimeTestUpdateContacts,
	KTimeTestAddContacts,
	KTimeTestFind,
	KTimeTestFindInTextDef,
	KTimeTestTextDef,
	KTimeTestGroupLoad,
	KTimeTestReadContactsWithGroups,
	KTimeTestCreateDeleteGroups,
	};
	
const TPtrC KDatabaseFileName=_L("C:T_TIME");

const TInt KProfileBase=15;
const TInt KProfileCount=6;
const TInt KProfileBase2=23;
const TInt KProfileCount2=6;
const TInt KProfileCountMax=6;
const TInt KMaxFieldTypes=8;

void TestErr(TInt aErr)
    {
	if (aErr!=KErrNone)
		{
		test.Printf(_L("Error %d\n"),aErr);
		test(EFalse);
		}
	}

LOCAL_C void DisplayProfile(TInt aNumber, TCntProfile aProfile)
	{
	TInt seconds=aProfile.iTime/1000000;
	aProfile.iTime-=seconds*1000000;
	TInt hundredths=aProfile.iTime/1000;
	test.Printf(_L("[%d:%d]=%03d.%03d "),aNumber,aProfile.iCount,seconds,hundredths);
	}

LOCAL_C void ResetProfiles(TInt aSet)
	{
	
	if (aSet==1)
		{
		CCntTest::ProfileReset(KProfileBase,KProfileCount); // reset profiles KProfileBase-KProfileCount
		}
	else if (aSet==2)
		{
		CCntTest::ProfileReset(KProfileBase2,KProfileCount2); 
		}
	}

LOCAL_C void DisplayProfiles(TInt aSet)
	{
	TCntProfile profiles[KProfileCountMax];
	TInt base=KProfileBase;
	TInt count=KProfileCount;
	if (aSet==2)
		{
		base=KProfileBase2;
		count=KProfileCount2;
		}
	CCntTest::ProfileResult(&profiles[0],0,10);
	for(TInt loop=0;loop<count;loop++)
		DisplayProfile(base+loop,profiles[loop]);
	test.Printf(_L("\n"));
	}

LOCAL_C void StartTheClock()
	{
	test.Printf(_L("Starting the clock...\n"));
	ResetProfiles(1);
	ResetProfiles(2);
	TheClock.UniversalTime();
	}

LOCAL_C void StopTheClock()
	{
	TTime endTime;
	endTime.UniversalTime();
	TheTestTime=endTime.MicroSecondsFrom(TheClock);
	TInt64 totalTime=TheTestTime.Int64();
	TInt64 minutes=totalTime/60000000;
	totalTime-=minutes*60000000;
	TInt64 seconds=totalTime/1000000;
	totalTime-=seconds*1000000;
	TInt64 hundredths=totalTime/10000;
	DisplayProfiles(1);
	DisplayProfiles(2);
	test.Printf(_L("Result=%d:%02d.%02d\n"),I64LOW(minutes),I64LOW(seconds),I64LOW(hundredths));
	}

LOCAL_C void AddFieldL(CContactItem& aItem, TInt aRecordNum, TInt aFieldNum)
	{
	TBuf<128> buf;
	buf.Format(_L("Record number=%d, Field number=%d"),aRecordNum,aFieldNum);
	TFieldType fieldType=TUid::Null();
	TUid mapping=KUidContactFieldVCardMapUnusedN;
	switch(aFieldNum%KMaxFieldTypes)
		{
		case 0:
			fieldType=KUidContactFieldGivenName;
			break;
		case 1:
			fieldType=KUidContactFieldAdditionalName;
			break;
		case 2:
			fieldType=KUidContactFieldFamilyName;
			break;
		case 3:
			mapping=KUidContactFieldVCardMapADR;
			fieldType=KUidContactFieldAddress;
			break;
		case 4:
    		mapping=KUidContactFieldVCardMapORG;
			fieldType=KUidContactFieldCompanyName;
			break;
		case 5:
    		mapping=KUidContactFieldVCardMapEMAILINTERNET;
    		fieldType=KUidContactFieldEMail;
			break;
		case 6:
    		mapping=KUidContactFieldVCardMapNOTE;
			fieldType=KUidContactFieldNote;
			break;
		case 7:
    		mapping=KUidContactFieldVCardMapURL;
			fieldType=KUidContactFieldUrl;
			break;
		}
	SetNameL(aItem,fieldType,mapping, buf, ETrue);
	}

LOCAL_C CContactIdArray *AddRecordsLC(TInt aNumRecords, TInt aNumFields, TInt aBlobSize)
	{
	CContactIdArray *ids=CContactIdArray::NewLC();
	TInt numFields=aNumFields;
	HBufC8 *blobData=NULL;
	if (aBlobSize>0)
		{
		numFields--;
		blobData=HBufC8::NewLC(aBlobSize);
		TUint8 *pData=(TUint8 *)blobData->Ptr();
		for(TInt loop1=0;loop1<aBlobSize;loop1++)
			*pData++=(TUint8)(loop1&0xFF);
		blobData->Des().SetLength(aBlobSize);
		}
	for(TInt loop=0;loop<aNumRecords;loop++)
		{
		CContactItem* item=CContactCard::NewLC();
		for(TInt fieldNum=0;fieldNum<numFields;fieldNum++)
			AddFieldL(*item,loop,fieldNum);
		if (aBlobSize>0)
			{
			CContactItemField* field=CContactItemField::NewLC(KStorageTypeStore,KUidContactFieldVCardMapPHOTO);
			field->SetMapping(KUidContactFieldVCardMapUnusedN);
			field->StoreStorage()->SetThingL(*blobData);
			item->AddFieldL(*field);
			CleanupStack::Pop(); // item
			}
		TContactItemId addId=CntTest->Db()->AddNewContactL(*item);
		CleanupStack::PopAndDestroy();	// item
		ids->AddL(addId);
		if ((loop%100)==0)
			CntTest->Db()->CompactL();
		}
	if (blobData)
		CleanupStack::PopAndDestroy(); // blobData
	CntTest->Db()->CompactL();
	test(CntTest->Db()->CountL()==aNumRecords);
	return(ids);
	}

LOCAL_C void VCardExportL(const CContactIdArray &aIds)
	{
	StartTheClock();
	CntTest->ExportContactsLC(&aIds,CContactDatabase::ETTFormat);
	StopTheClock();
	CleanupStack::PopAndDestroy();	// store (from ExportContactsLC)
	}

LOCAL_C void VCardExportToFile1by1L(const CContactIdArray &aIds)
	{
	StartTheClock();
	for(TInt loop=0;loop<aIds.Count();loop++)
		{
		CntTest->ExportContactLC(aIds[loop],CContactDatabase::ETTFormat,KVCardStoreTypeFile);
		CleanupStack::PopAndDestroy();	// store
		}
	StopTheClock();
	}

LOCAL_C void VCardImportL(const CContactIdArray &aIds)
	{
	CVCardTestStore* store=CntTest->ExportContactsLC(&aIds,CContactDatabase::ETTFormat);
	StartTheClock();
	CArrayPtr<CContactItem>* items=CntTest->ImportContactsL(store,CContactDatabase::ETTFormat);
	StopTheClock();
	items->ResetAndDestroy();
	CleanupStack::PopAndDestroy(); // store
	delete items;
	}

LOCAL_C void VCardImportFromFile1by1L(const CContactIdArray &aIds)
	{
	CArrayPtrFlat<CVCardTestStore> *stores=new(ELeave) CArrayPtrFlat<CVCardTestStore>(aIds.Count());
	TInt loop=0;
	for(;loop<aIds.Count();loop++)
		{
		stores->AppendL(CntTest->ExportContactLC(aIds[loop],CContactDatabase::ETTFormat,KVCardStoreTypeFile,loop));
		CleanupStack::Pop();	// store
		}
	StartTheClock();
	for(loop=0;loop<aIds.Count();loop++)
		{
		CArrayPtr<CContactItem>* items=CntTest->ImportContactsL((*stores)[loop],CContactDatabase::ETTFormat);
		items->ResetAndDestroy();
		delete items;
		delete (*stores)[loop];
		}
	delete stores;
	StopTheClock();
	}

LOCAL_C void ExportImportDifferentMachineIds(TInt aNumRecords, TInt aNumFields)
	{
	CntTest->Db()->OverrideMachineUniqueId(0x1);
	CContactIdArray *ids=AddRecordsLC(aNumRecords, aNumFields, 0);
//
	CVCardTestStore* store=CntTest->ExportContactsLC(ids,CContactDatabase::ETTFormat);
	CntTest->CloseDatabase();
	CntTest->CreateDatabaseL();
	CntTest->Db()->OverrideMachineUniqueId(0x2);
	StartTheClock();
	CntTest->ImportContacts1by1L(store,CContactDatabase::ETTFormat|CContactDatabase::EImportSingleContact);
	StopTheClock();
//
	test(CntTest->Db()->CountL()==aNumRecords);
	CntTest->Db()->CompactL();
//
	StartTheClock();
	CntTest->ImportContacts1by1L(store,CContactDatabase::ETTFormat|CContactDatabase::EImportSingleContact);
	StopTheClock();
//
	CleanupStack::PopAndDestroy();	// store
	test(CntTest->Db()->CountL()==aNumRecords);
//
	CleanupStack::PopAndDestroy();	// ids
	}

/*
LOCAL_C void DeleteFiles()
	{
	for(TInt loop=0;loop<200;loop++)
		{
		TFileName fileName;
		fileName.Format(_L("VCard%04d"),loop);
		TInt err=CntTest->Fs().Delete(fileName);
		test.Printf(_L("Del %04d err=%d\n"),loop,err);
		User::After(100000);
		}
	}*/

void CreateDeleteGroupsL(const CContactIdArray &aIds)
	{
	StartTheClock();
	for(TInt loop=0;loop<20;loop++)
		{
// Create groups
		CContactItem* group1=CntTest->Db()->CreateContactGroupLC();
		TContactItemId gr1id=group1->Id();
		CContactItem* group2=CntTest->Db()->CreateContactGroupLC();
		TContactItemId gr2id=group2->Id();
		CContactItem* group3=CntTest->Db()->CreateContactGroupLC();
		TContactItemId gr3id=group3->Id();
		CleanupStack::PopAndDestroy(3); // group1,2,3
// add contacts to groups
		for(TInt loop1=0;loop1<aIds.Count();loop1++)
			{
			if ((loop%4)==0)
				CntTest->Db()->AddContactToGroupL(aIds[loop1],gr1id);
			if ((loop%8)==0)
				CntTest->Db()->AddContactToGroupL(aIds[loop1],gr2id);
			if ((loop%16)==0)
				CntTest->Db()->AddContactToGroupL(aIds[loop1],gr3id);
			}
// delete groups
		CntTest->Db()->DeleteContactL(gr1id);
		CntTest->Db()->DeleteContactL(gr2id);
		CntTest->Db()->DeleteContactL(gr3id);
		}
	StopTheClock();
	}

void ReadContactsWithGroupsL(const CContactIdArray &aIds)
	{
/*	CContactItem* group1=CntTest->Db()->CreateContactGroupLC();
	TContactItemId gr1id=group1->Id();
	CContactItem* group2=CntTest->Db()->CreateContactGroupLC();
	TContactItemId gr2id=group2->Id();
	CleanupStack::PopAndDestroy(2); // group1,2
	for(TInt loop1=0;loop1<aIds.Count();loop1++)
		{
		if ((loop1%2)==0)
			CntTest->Db()->AddContactToGroupL(aIds[loop1],gr1id);
		else
			CntTest->Db()->AddContactToGroupL(aIds[loop1],gr2id);
		}
*/
	TContactItemId grid=0;
	for(TInt loop1=0;loop1<aIds.Count();loop1++)
		{
		if ((loop1%2)==0)
			{
			CContactItem* group=CntTest->Db()->CreateContactGroupLC();
			grid=group->Id();
			CleanupStack::PopAndDestroy(); // group
			CntTest->Db()->AddContactToGroupL(aIds[loop1],grid);
			}
		else
			CntTest->Db()->AddContactToGroupL(aIds[loop1],grid);
		}
//
	StartTheClock();
	for(TInt loop=0;loop<aIds.Count();loop++)
		delete CntTest->Db()->ReadContactL(aIds[loop]);
	StopTheClock();
	}

void GroupLoadL(const CContactIdArray &aIds)
	{
	CContactItem* group1=CntTest->Db()->CreateContactGroupLC();
	TContactItemId gr1id=group1->Id();
	CContactItem* group2=CntTest->Db()->CreateContactGroupLC();
	TContactItemId gr2id=group2->Id();
	CContactItem* group3=CntTest->Db()->CreateContactGroupLC();
	TContactItemId gr3id=group3->Id();
	CleanupStack::PopAndDestroy(3); // group1,2,3
	for(TInt loop=0;loop<aIds.Count();loop++)
		{
		if ((loop%2)==0)
			CntTest->Db()->AddContactToGroupL(aIds[loop],gr1id);
		else
			CntTest->Db()->AddContactToGroupL(aIds[loop],gr2id);
		if ((loop%100)==0)
			CntTest->Db()->AddContactToGroupL(aIds[loop],gr3id);
		}
	StartTheClock();
	for(TInt times=0;times<200;times++)
		delete CntTest->Db()->ReadContactL(gr1id);
	StopTheClock();
	}

void ReadTextDefL(const CContactIdArray &aIds)
	{
	CContactTextDef* textDef=CContactTextDef::NewLC();
	textDef->AppendL(TContactTextDefItem(KUidContactFieldGivenName,_L(",")));
	textDef->AppendL(TContactTextDefItem(KUidContactFieldAdditionalName));
	TBuf<256> result;
	StartTheClock();
	for(TInt times=0;times<10;times++)
		for(TInt loop=0;loop<aIds.Count();loop++)
			CntTest->Db()->ReadContactTextDefL(aIds[loop],result,textDef);
	StopTheClock();
	CleanupStack::PopAndDestroy(); // textDef
	}

void ReadContactsL(const CContactIdArray &aIds)
	{
	StartTheClock();
	for(TInt loop=0;loop<aIds.Count();loop++)
		delete CntTest->Db()->ReadContactL(aIds[loop]);
	StopTheClock();
	}

void UpdateContactsL(const CContactIdArray &aIds)
	{
	StartTheClock();
	for(TInt loop=0;loop<aIds.Count();loop++)
		{
		CContactItem *item=CntTest->Db()->OpenContactLX(aIds[loop]);
		CleanupStack::PushL(item);
		CntTest->Db()->CommitContactL(*item);
		CleanupStack::PopAndDestroy(2);	// item,item->close
		}
	StopTheClock();
	}

void AddContactsL(TInt aNumRecords, TInt aNumFields)
	{
	if (aNumFields>8)
		{
		test.Printf(_L("Error max fields=8"));
		test.Getch();
		return;
		}
	CContactItem* item=CContactCard::NewLC();
	for(TInt fieldNum=0;fieldNum<aNumFields;fieldNum++)
		AddFieldL(*item,0,fieldNum);
	StartTheClock();
	for(TInt loop=0;loop<aNumRecords;loop++)
		CntTest->Db()->AddNewContactL(*item);
	StopTheClock();
	CleanupStack::PopAndDestroy();	// item
	}

void FindL(TInt , TInt )
	{
	StartTheClock();
	for(TInt loop=0;loop<200;loop++)
		{
		TBuf<32> find;
		find.Format(_L("rd number=%d"),loop%10);
		CContactIdArray* ids=CntTest->Db()->FindLC(find,NULL);
		ids->Count();
		CleanupStack::PopAndDestroy(); // ids
		}
	StopTheClock();
	}

LOCAL_C TInt findWordSplitterL(TAny *aParams)
	{
	SFindInTextDefWordParser *parser=(SFindInTextDefWordParser *)aParams;
	parser->iWordArray->AppendL(*parser->iSearchString);
	return(KErrNone);
	}

void FindInTextDefL(TInt , TInt )
	{
	TCallBack callBack(findWordSplitterL);
	CContactTextDef* textDef=CContactTextDef::NewLC();
	textDef->AppendL(TContactTextDefItem(KUidContactFieldGivenName));
	CDesCArray *desCA=new(ELeave) CDesCArrayFlat(5);
	CleanupStack::PushL(desCA);
	StartTheClock();
	for(TInt loop=0;loop<200;loop++)
		{
		TBuf<32> find;
		find.Format(_L("Record number=%d"),loop%10);
		desCA->AppendL(find);
		CContactIdArray* ids=CntTest->Db()->FindInTextDefLC(*desCA,textDef,callBack);
		ids->Count();
		desCA->Delete(0);
		CleanupStack::PopAndDestroy(); // ids
		}
	StopTheClock();
	CleanupStack::PopAndDestroy(2); // textDef,desCA
	}

TKeyCode RunTimeTestL(TInt aTestNum, TInt aParam1, TInt aParam2, TBool aAddStandardFields, TInt aBlobSize)
	{
	CContactIdArray *ids=NULL;
	test.Printf(_L("Initialising...%d,%d Blob=%d\n"),aParam1,aParam2,aBlobSize);
	CntTest->CreateDatabaseL();
	if (aAddStandardFields)
		ids=AddRecordsLC(aParam1,aParam2,aBlobSize);
	switch(aTestNum)
		{
		case KTimeTestExportImportDifferentMachineIds:
			ExportImportDifferentMachineIds(aParam1, aParam2);
			break;
		case KTimeTestVCardExport:
			VCardExportL(*ids);
			break;
		case KTimeTestVCardImport:
			VCardImportL(*ids);
			break;
		case KTimeTestVCardExportToFile1by1:
			VCardExportToFile1by1L(*ids);
			break;
		case KTimeTestVCardImportFromFile1by1:
			VCardImportFromFile1by1L(*ids);
			break;
		case KTimeTestReadContacts:
			ReadContactsL(*ids);
			break;
		case KTimeTestReadContactsWithGroups:
			ReadContactsWithGroupsL(*ids);
			break;
		case KTimeTestCreateDeleteGroups:
			CreateDeleteGroupsL(*ids);
			break;
		case KTimeTestTextDef:
			ReadTextDefL(*ids);
			break;
		case KTimeTestGroupLoad:
			GroupLoadL(*ids);
			break;
		case KTimeTestUpdateContacts:
			UpdateContactsL(*ids);
			break;
		case KTimeTestAddContacts:
			AddContactsL(aParam1,aParam2);
			break;
		case KTimeTestFind:
			FindL(aParam1,aParam2);
			break;
		case KTimeTestFindInTextDef:
			FindInTextDefL(aParam1,aParam2);
			break;
		}
	if (aAddStandardFields)
		CleanupStack::PopAndDestroy();	// ids
	CntTest->CloseDatabase();
	CntTest->DeleteDatabaseL();
	return(test.Getch());
	}

/////////////////////////////////////////////////////////////////////////////

TInt GetNum()
	{
	TInt num=0;
	TKeyCode key;
	TBuf<8> buf;
	FOREVER
		{
		key=test.Getch();
		if (key==EKeyEnter)
			break;
		if (buf.Length()<buf.MaxLength())
			buf.Append(key);
		TBuf<1> akey;
		akey.Append(key);
		test.Printf(akey);
		}
	for(TInt loop=0;loop<buf.Length();loop++)
		{
		TText chr=buf[loop];
		if (TChar(chr).IsDigit())
			{
			num*=10;
			num+=chr-'0';
			}
		else
			{
			num=0;
			break;
			}
		}
	return(num);
	}

void SetParams(TInt &aParam1,TInt &aParam2)
	{
	test.Printf(_L("Param1="));
	aParam1=GetNum();
	test.Printf(_L("\nParam2="));
	aParam2=GetNum();
	test.Printf(_L("\nParam1=%d, Param2=%d\n"),aParam1,aParam2);
	}

void SetBlobSize(TInt &aBlobSize)
	{
	test.Printf(_L("\nBlob Size="));
	aBlobSize=GetNum();
	test.Printf(_L("\nBlobSize=%d\n"),aBlobSize);
	}

void ListDeletedContactsL()
    {
	CContactDatabase *db=CContactDatabase::OpenL();
	CleanupStack::PushL(db);
//
	CContactIdArray *deletedIds=db->DeletedContactsLC();
	for(TInt loop=0;loop<deletedIds->Count();loop++)
		{
		CContactItem *item=db->ReadContactLC((*deletedIds)[loop]);
		test(item->IsDeleted());
		test.Printf(_L("Item[%d]),count=%d\n"),(*deletedIds)[loop],item->AccessCount());
		CleanupStack::PopAndDestroy();	// item
		}
	CleanupStack::PopAndDestroy();	// deletedIds
//
	CleanupStack::PopAndDestroy();	// db
	test.Printf(_L("\n\nPress Any Key to continue....\n"));
	test.Getch();
	}

/**

@SYMTestCaseID     PIM-T-TIME-0001

*/

void DoTestsL()
    {
	CntTest->SelectDriveL();
	TInt param1=100;
	TInt param2=4;
	TInt blobSize=0;
	test.Start(_L("@SYMTESTCaseID:PIM-T-TIME-0001 Time tests"));

	TInt key=-1;
	FOREVER
		{
		if (key==-1)
			{
			test.Printf(_L("\n\nSelect test:\n"));
			test.Printf(_L("0: VCard non-matching ID import export\n"));
			test.Printf(_L("1: VCard export (P1=Records, P2=fields)\n"));
			test.Printf(_L("2: VCard import (P1=Records, P2=fields)\n"));
			test.Printf(_L("3: VCard export file 1x1 (P1=Records, P2=fields)\n"));
			test.Printf(_L("4: VCard import file 1x1 (P1=Records, P2=fields)\n"));
			test.Printf(_L("5: Read Contacts (P1=Records, P2=fields)\n"));
			test.Printf(_L("6: Update Contacts (P1=Records, P2=fields)\n"));
			test.Printf(_L("7: Add Contacts (P1=Records, P2=fields)\n"));
			test.Printf(_L("8: Find (P1=Records, P2=fields)\n"));
			test.Printf(_L("9: Find in text def (P1=Records, P2=fields)\n"));
			test.Printf(_L("a: Read text def (P1=Records, P2=fields)\n"));
			test.Printf(_L("g: Load group (P1=Records, P2=fields)\n"));
			test.Printf(_L("h: Read contacts with groups (P1=Records, P2=fields)\n"));
			test.Printf(_L("D: List deleted contacts"));
			test.Printf(_L("P: Set params (%d,%d)\n"),param1,param2);
			test.Printf(_L("R: Show pRofile results\n"));
			test.Printf(_L("E: Exit\n"));
			key=test.Getch();
			}
//		TAutoSwitchOffBehavior switchOff;
//		UserHal::AutoSwitchOffBehavior(switchOff);
//		UserHal::SetAutoSwitchOffBehavior(ESwitchOffDisabled);
		test.Console()->ClearScreen();
		if (key=='e' || key=='E')
			break;
		TInt switchKey=key;
		key=-1;
		switch(switchKey)
			{
			case '0':
				key=RunTimeTestL(KTimeTestExportImportDifferentMachineIds,param1,param2,EFalse,blobSize);
				break;
			case '1':
				key=RunTimeTestL(KTimeTestVCardExport,param1,param2,ETrue,blobSize);
				break;
			case '2':
				key=RunTimeTestL(KTimeTestVCardImport,param1,param2,ETrue,blobSize);
				break;
			case '3':
				key=RunTimeTestL(KTimeTestVCardExportToFile1by1,param1,param2,ETrue,blobSize);
				break;
			case '4':
				key=RunTimeTestL(KTimeTestVCardImportFromFile1by1,param1,param2,ETrue,blobSize);
				break;
			case '5':
				key=RunTimeTestL(KTimeTestReadContacts,param1,param2,ETrue,blobSize);
				break;
			case '6':
				key=RunTimeTestL(KTimeTestUpdateContacts,param1,param2,ETrue,blobSize);
				break;
			case '7':
				key=RunTimeTestL(KTimeTestAddContacts,param1,param2,EFalse,blobSize);
				break;
			case '8':
				key=RunTimeTestL(KTimeTestFind,param1,param2,ETrue,blobSize);
				break;
			case '9':
				key=RunTimeTestL(KTimeTestFindInTextDef,param1,param2,ETrue,blobSize);
				break;
			case 'a':
			case 'A':
				key=RunTimeTestL(KTimeTestTextDef,param1,param2,ETrue,blobSize);
				break;
			case 'g':
			case 'G':
				key=RunTimeTestL(KTimeTestGroupLoad,param1,param2,ETrue,blobSize);
				break;
			case 'H':
			case 'h':
				key=RunTimeTestL(KTimeTestReadContactsWithGroups,param1,param2,ETrue,blobSize);
				break;
			case 'i':
				key=RunTimeTestL(KTimeTestCreateDeleteGroups,param1,param2,ETrue,blobSize);
				break;
//			case 'd':
//				DeleteFiles();
//				break;
			case 'p':
			case 'P':
				SetParams(param1,param2);
				break;
			case 'b':
			case 'B':
				SetBlobSize(blobSize);
				break;
			case 'r':
			case 'R':
				DisplayProfiles(1);
				DisplayProfiles(2);
				ResetProfiles(1);
				ResetProfiles(2);
				test.Getch();
				break;
			case 'd':
			case 'D':
				ListDeletedContactsL();
				break;
			case 'Q':
				test.Printf(_L("Press 'q' to confirm replace contacts file"));
				if (test.Getch()=='q')
					{
					CContactDatabase *db=CContactDatabase::ReplaceL();
					delete db;
					}
				break;
			}
//		UserHal::SetAutoSwitchOffBehavior(switchOff);
		}
	TInt ignore;
	if (CntTest->Db())
		TRAP(ignore,CntTest->DeleteDatabaseL());
    }

GLDEF_C TInt E32Main()
	{
    CntTest=new(ELeave) CCntTest;
	CntTest->ConstructL(test,KDatabaseFileName);
    TRAPD(err,DoTestsL());
	if (err!=KErrNone)
		{
		test.Printf(_L("Exit error=%d"),err);
		test.Getch();
		}
	CntTest->EndTestLib(err);
	return KErrNone;
    }
