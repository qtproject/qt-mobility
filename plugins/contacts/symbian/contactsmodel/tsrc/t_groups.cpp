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
#include <bautils.h>
#include <barsc.h>
#include <barsread.h>
#include <cntmodel.rsg>
#include "t_utils.h"
#include <hal.h>
#include "cntclient.h"

#pragma warning(disable:4239) // group label warning

CCntTest* CntTest=NULL;
LOCAL_D RTest test(_L("T_GROUPS"));
TInt CCntClient::iObservers = 0;

const TPtrC KDatabaseFileName=_L("C:T_GROUP");
const TPtrC KTestName=_L("Test Name No %d");
const TPtrC KTestAddress=_L("Test Address No %d");
const TPtrC KTestFamilyName=_L("Test Family Name No %d");
const TPtrC KLongLabel = _L("New Long Label");
const TPtrC KGroupLabel = _L("Group Label No%d");
const TPtrC KTestEmail = _L("Test@Email.com No %d");
const TPtrC KTestCountry = _L("Test Country No %d");
const TPtrC KTestCompany = _L("Test Company No %d");
const TInt KLargeSizeRecords=50;
const TInt KFamilySizeRecords=20;


#define KCardGivenName _L("Given Name #%d")
#define KCardFamilyName _L("Family Name #%d")
#define KCardFieldText _L("Card id %d, text field #%d")
#define KCardFieldEditedText _L("UPDATED Card id %d, text field #%d")


//////////////////////////////////////////////////////////////////////////
//
//	SUPPORT MODULES 
//
//////////////////////////////////////////////////////////////////////////

GLDEF_C void Panic(TInt aPanic)
//
// Panic the thread with CNTMODEL as the category
//
	{
	User::Panic(_L("T_GROUP"),aPanic);
	}

LOCAL_C void SetNameL(CContactItem& aItem,TUid aType,const TDesC& aName)
//
// Set the contents of a text field, creating the field if required
//
	{
	CContactItemFieldSet& fieldSet=aItem.CardFields();
	const TInt pos=fieldSet.Find(aType);
	if (pos!=KErrNotFound)
		fieldSet[pos].TextStorage()->SetTextL(aName);
	else
		Panic(KErrGeneral);
	}

LOCAL_C void PopulateDatabaseL(TInt aNumberToPopulate, TBool aPhoneNumbers)
//
// Create and populate the database
//
	{
	TTime before;
	before.UniversalTime();
	CContactItem* templ = CntTest->Db()->ReadContactL(0);
	CleanupStack::PushL(templ);
	for (TInt ii=0;ii<aNumberToPopulate;ii++)
		{
		CContactItem* item=CContactCard::NewLC(templ);
		TBuf<16> name;
		name.Format(KTestName,ii);
   		SetNameL(*item,KUidContactFieldGivenName,name);
		if (aPhoneNumbers)
			{
			TBuf<20> number;
			switch(ii%3)
				{
				case 0:
					number.Format(_L("0171-%03d %04d"),(ii*9)%1000,((ii+11)*23)%10000);
					break;
				case 1:
					number.Format(_L("%04d:%04d:%04d:%04d"),(ii*123)%10000,(ii*666)%10000,(ii*234)%10000);
					break;
				case 2:
					number.Format(_L("+00%d-%03d %04d"),(ii*123)%100,(ii*13)%1000,((ii+13)*17)%10000);
					break;
				}
   			SetNameL(*item,KUidContactFieldPhoneNumber,number);
			if (!(ii%2))
				{
				number.Format(_L("0181-%03d %04d"),(ii*8)%1000,((ii+11)*22)%10000);
	   			SetNameL(*item,KUidContactFieldPhoneNumber,number);
				number.Format(_L("01734-%06d"),(ii*123456)%1000000);
	   			SetNameL(*item,KUidContactFieldPhoneNumber,number);
				}
			}
		TBuf<32> address;
		address.Format(KTestAddress,ii);
		SetNameL(*item,KUidContactFieldAddress,address);
//
		TBuf<32> familyName;
		familyName.Format(KTestFamilyName,ii);
		SetNameL(*item,KUidContactFieldFamilyName,familyName);
//
		TBuf<32> email;
		email.Format(KTestEmail,ii);
		SetNameL(*item,KUidContactFieldEMail,email);
//
		TBuf<32> country;
		country.Format(KTestCountry,ii);
		SetNameL(*item,KUidContactFieldCountry,country);
//	
		TBuf<32> company;
		company.Format(KTestCompany,ii);
		SetNameL(*item,KUidContactFieldCompanyName,company);
//	
		CntTest->Db()->AddNewContactL(*item); //templ
		CleanupStack::PopAndDestroy(); // item
//	
		if (ii%100==0)
			{
			test.Printf(_L("."));	// Just to show some life
			CntTest->Db()->CompactL();
			}

		}
	CleanupStack::PopAndDestroy();
	CntTest->Db()->SetDateFormatTextL(_L("%E%D%X%N%Y %1 %2 %3"));
	CntTest->Db()->CompactL();
	TTime after;
	after.UniversalTime();
	TTimeIntervalSeconds secondsTaken;
	after.SecondsFrom(before,secondsTaken);
	test.Printf(_L(" TIME: %d  Secs"),secondsTaken.Int());
	test.Printf(_L("\n"));	
	//test.Getch();
	}


//////////////////////////////////////////////////////////////////////////
//
//	TEST MODULES 
//
//////////////////////////////////////////////////////////////////////////


LOCAL_C void CreateBasicContactGroups()
	{
	test.Next(_L("Create Basic Groups"));
	__ASSERT_ALWAYS(CntTest->Db()->GroupCount()==0,Panic(KErrGeneral));
	CContactItem* newGroup = CntTest->Db()->CreateContactGroupL(_L("New Group"));
	CContactItem* newGroup2 = CntTest->Db()->CreateContactGroupL();
	CleanupStack::PushL(newGroup);
	CleanupStack::PushL(newGroup2);
	CntTest->CloseDatabase();
	CntTest->OpenDatabaseL();
	test(CntTest->Db()->GroupCount()==2);
	CleanupStack::PopAndDestroy(2); // newGroup newGroup2
	}

LOCAL_C void CreateGroupFromAClientL(CCntClient& aCntClient)
	{
	test.Next(_L("Create a basic group"));
	CContactItem* tempGroup = aCntClient.Db()->CreateContactGroupL(_L("New Group"));
	delete tempGroup ;
	CActiveScheduler::Start();
	}

LOCAL_C void DeleteBasicContactGroups()
	{
	test.Next(_L("Delete Basic Groups"));
	TInt groupCount = CntTest->Db()->GroupCount();
	__ASSERT_ALWAYS(groupCount==2,Panic(KErrGeneral));
	CContactIdArray* groupIdList = CntTest->Db()->GetGroupIdListL();
	CleanupStack::PushL(groupIdList);
	test(groupIdList->Count()==2);
//
	for (TInt ii=0; ii< groupIdList->Count();ii++)
		CntTest->Db()->DeleteContactL((*groupIdList)[ii]);
	test(CntTest->Db()->CountL()==0); //own card
	CContactIdArray* newGroupIdList = CntTest->Db()->GetGroupIdListL();
	test(newGroupIdList->Count()==0);
	delete newGroupIdList;
	CleanupStack::PopAndDestroy(); //groupIdList;
	}	

LOCAL_C void CheckBasicPopulatedGroup()
	{
	test.Next(_L("Populate Basic Group"));
	CntTest->CloseDatabase();
	CntTest->DeleteDatabaseL();
	CntTest->CreateDatabaseL();
	TRAP_IGNORE(PopulateDatabaseL(KFamilySizeRecords,ETrue));
	
	CContactItem* newGroup = CntTest->Db()->CreateContactGroupLC(_L("Family Group"));
	TContactItemId groupId = newGroup->Id();
	for(TInt ii=2;ii<12;ii++)
		{
		CntTest->Db()->AddContactToGroupL(ii,groupId);
		}
	CntTest->CloseDatabase();
	CntTest->OpenDatabaseL();
//
	CContactItem* familyGroup = CntTest->Db()->ReadContactLC(groupId);
	CContactIdArray* memberArray = STATIC_CAST(CContactGroup*,familyGroup)->ItemsContainedLC();
	test(memberArray->Count()==10);
//
	CContactItem* familyMember = CntTest->Db()->ReadContactLC(2);
	CContactIdArray* groups = STATIC_CAST(CContactCard*, familyMember)->GroupsJoinedLC();
	test(groups->Count()==1);
	TContactItemId memberShip = (*groups)[0];
	test(memberShip==groupId);
	CleanupStack::PopAndDestroy(5); 
	// newGroup familyGroup memberArray familyMember groups
	}


LOCAL_C void CheckRemoveMember()
	{
	test.Next(_L("Remove Group Member a check Persist"));
//
	CContactItem* familyMember = CntTest->Db()->ReadContactLC(2);
	CContactIdArray* groups = STATIC_CAST(CContactCard*, familyMember)->GroupsJoinedLC();
	test(groups->Count()>=1);
	TContactItemId memberShip = (*groups)[0];
	CContactItem* group = CntTest->Db()->ReadContactLC(memberShip);
	CContactIdArray* memberArray = STATIC_CAST(CContactGroup*,group)->ItemsContainedLC();
	TInt memberCount1 = memberArray->Count();
//
	CntTest->Db()->RemoveContactFromGroupL(*familyMember, *group);
	CleanupStack::PopAndDestroy(); // group
//
	CContactItem* changedGroup = CntTest->Db()->ReadContactLC(memberShip);
	CContactIdArray* newMemberArray = STATIC_CAST(CContactGroup*,changedGroup)->ItemsContainedLC();
	test(newMemberArray->Count()==memberCount1-1);
	CleanupStack::PopAndDestroy(5); // familyMember groups changedGroup newMemberArray memberarray
	}

LOCAL_C void CheckRemoveGroup()
	{
	test.Next(_L("Remove Group check Persist"));
//
	CContactIdArray* groupIdList = CntTest->Db()->GetGroupIdListL();
	CleanupStack::PushL(groupIdList);
	CContactItem* group = CntTest->Db()->ReadContactLC((*groupIdList)[0]);
	CContactIdArray* memberArray = STATIC_CAST(CContactGroup*,group)->ItemsContainedLC();
//
	CContactItem* member = CntTest->Db()->ReadContactLC((*memberArray)[0]);
	CContactIdArray* memberShipArray = STATIC_CAST(CContactCard*, member)->GroupsJoinedLC();
	__ASSERT_ALWAYS(memberShipArray->Count()==1,Panic(KErrGeneral));
	test((*memberShipArray)[0]==(*groupIdList)[0]);
//
	CntTest->Db()->DeleteContactL((*groupIdList)[0]);
//
	CContactItem* newMember = CntTest->Db()->ReadContactLC((*memberArray)[0]);
	CContactIdArray* newMemberShipArray = STATIC_CAST(CContactCard*, newMember)->GroupsJoinedLC();
	test(newMemberShipArray->Count()==0);
//
	CleanupStack::PopAndDestroy(7); // groupidList group memberArray member memberShipArray newMember newMemberShipArray
	}

LOCAL_C void CheckLargePopulatedGroup()
	{
	test.Next(_L("Create MANY Contacts"));
	CntTest->CloseDatabase();
	CntTest->DeleteDatabaseL();
	CntTest->CreateDatabaseL();
	TRAP_IGNORE(PopulateDatabaseL(KLargeSizeRecords,ETrue));
	CContactItem* newGroup = CntTest->Db()->CreateContactGroupL(_L("Large Group"));
	CleanupStack::PushL(newGroup);
	TContactItemId groupId = newGroup->Id();
	test.Next(_L("Add MANY Contacts to a Group"));
	TTime before;
	before.UniversalTime();
	for(TInt ii=1;ii<KLargeSizeRecords+1;ii++)
		{
		CntTest->Db()->AddContactToGroupL(ii, groupId); //*tempContact,*newGroup);
		if (ii%100==0)
			test.Printf(_L("."));	// Just to show some life
		}
	test(TestGroupStateL(CntTest->Db(),1,KLargeSizeRecords));
	TTime after;
	after.UniversalTime();
	TTimeIntervalSeconds secondsTaken;
	after.SecondsFrom(before,secondsTaken);
	test.Printf(_L(" TIME: %d  Secs"),secondsTaken.Int());
	test.Printf(_L("\n"));	
	//test.Getch();
	CntTest->CloseDatabase();
	CntTest->OpenDatabaseL();
//
	CContactItem* familyGroup = CntTest->Db()->ReadContactL(groupId);
	CContactIdArray* memberArray = STATIC_CAST(CContactGroup*,familyGroup)->ItemsContainedLC();
	test(memberArray->Count()==KLargeSizeRecords);
	delete familyGroup;
//
	CContactItem* familyMember = CntTest->Db()->ReadContactL(2);
	CContactIdArray* groups = STATIC_CAST(CContactCard*, familyMember)->GroupsJoinedLC();
	test(groups->Count()==1);
	TContactItemId memberShip = (*groups)[0];
	test(memberShip==groupId);
	delete familyMember;
	CleanupStack::PopAndDestroy(3); // newGroup groups memberArray
	}

LOCAL_C void DeleteGroupMembersCheckPersist()
	{
	test.Next(_L("Delete Group Members Check Persist"));
//
	TTime before;
	before.UniversalTime();
	for(TInt ii=1;ii<KLargeSizeRecords+1;ii++)
		{
		CntTest->Db()->DeleteContactL(ii);
		if (ii%100==0)
			test.Printf(_L("."));	// Just to show some life
		}
	TTime after;
	after.UniversalTime();
	TTimeIntervalSeconds secondsTaken;
	after.SecondsFrom(before,secondsTaken);
	test.Printf(_L(" TIME: %d  Secs"),secondsTaken.Int());
	test.Printf(_L("\n"));	
	//test.Getch();
//
	CContactIdArray* groups = CntTest->Db()->GetGroupIdListL();
	CleanupStack::PushL(groups);
	CContactItem* group = CntTest->Db()->ReadContactL((*groups)[0]);
	CContactIdArray* memberArray = STATIC_CAST(CContactGroup*,group)->ItemsContainedLC();
	test(memberArray->Count()==0);
	delete group;
	CleanupStack::PopAndDestroy(2); // groups memberArray
	}


LOCAL_C void CreateManyGroups()
	{
	test.Next(_L("Populate Database"));
	CntTest->CloseDatabase();
	CntTest->DeleteDatabaseL();
	CntTest->CreateDatabaseL();
	// add 5 new contacts
	TRAP_IGNORE(PopulateDatabaseL(5,ETrue));
//
	test.Next(_L("Create MANY Groups"));
	TTime before;
	before.UniversalTime();
	for(TInt ii=0;ii<KLargeSizeRecords;ii++)
		{
		TBuf<32> label;
		label.Format(KGroupLabel,ii);
		CContactItem* newGroup = CntTest->Db()->CreateContactGroupL(label);
		if (ii%100==0)
				test.Printf(_L("."));	// Just to show some life
		delete newGroup;
		}
	TTime after;
	after.UniversalTime();
	TTimeIntervalSeconds secondsTaken;
	after.SecondsFrom(before,secondsTaken);
	test.Printf(_L(" TIME: %d  Secs"),secondsTaken.Int());
	test.Printf(_L("\n"));	
	//test.Getch();
	test(CntTest->Db()->GroupCount()==KLargeSizeRecords);
//
	test.Next(_L("Add 5 Contacts to MANY Groups"));
	before.UniversalTime();
	for(TInt jj=1;jj<6;jj++)
	// add cards to all groups
		{
		CContactItem* contact = CntTest->Db()->ReadContactL(jj);
		CleanupStack::PushL(contact);
		for(TInt gg=6;gg<KLargeSizeRecords+6;gg++)	// assume knowledge of group ids..
			{
			CntTest->Db()->AddContactToGroupL(jj,gg);	//*contact,*group);
			if (gg%100==0)
				test.Printf(_L("."));	// Just to show some life
			}
		CleanupStack::PopAndDestroy(); // contact
		}
	after.UniversalTime();
	after.SecondsFrom(before,secondsTaken);
	test.Printf(_L(" TIME: %d  Secs"),secondsTaken.Int());
	test.Printf(_L("\n"));	
	test(TestGroupStateL(CntTest->Db(),KLargeSizeRecords,5*KLargeSizeRecords));
	//test.Getch();

//
	test.Next(_L("Check Contacts Group membership"));
	before.UniversalTime();
	for(TInt gg=1;gg<6;gg++)
	//	check cards membership
		{
		CContactItem* contact = CntTest->Db()->ReadContactLC(gg);
		CContactIdArray* groups = STATIC_CAST(CContactCard*, contact)->GroupsJoinedLC();
		test(groups->Count()==KLargeSizeRecords);
		CleanupStack::PopAndDestroy(2); // contact groups
		}
	after.UniversalTime();
	after.SecondsFrom(before,secondsTaken);
	test.Printf(_L(" TIME: %d  Secs"),secondsTaken.Int());
	test.Printf(_L("\n"));	
	//test.Getch();

	}


LOCAL_C void DeleteManyGroups()
	{
//
	test.Next(_L("Delete MANY Groups"));
	TTime before;
	before.UniversalTime();
	for(TInt ii=6;ii<KLargeSizeRecords+6;ii++)
		{
		CntTest->Db()->DeleteContactL(ii);
		if (ii%100==0)
				test.Printf(_L("."));	// Just to show some life
		}
	TTime after;
	after.UniversalTime();
	TTimeIntervalSeconds secondsTaken;
	after.SecondsFrom(before,secondsTaken);
	test.Printf(_L(" TIME: %d  Secs"),secondsTaken.Int());
	test.Printf(_L("\n"));	
	//test.Getch();

//
	test.Next(_L("Test Card Membership Update"));
	test(CntTest->Db()->CountL()==5); // 5 contacts + own card
	for(TInt jj=1;jj<6;jj++)
	// add cards to all groups
		{
		CContactItem* contact = CntTest->Db()->ReadContactLC(jj);
		CContactIdArray* groups = STATIC_CAST(CContactCard*, contact)->GroupsJoinedLC();
		test(groups->Count()==0);
		CleanupStack::PopAndDestroy(2); // contact groups
		}
	CntTest->Db()->CompactL();
	}


LOCAL_C void AddCardToCard()
	{
//
	test.Next(_L("Add Card to Card"));
	// assumes Prior Knowldege of contacts 2 & 3 (both are cards)
	CntTest->Db()->AddContactToGroupL(2,3);	
//
	}	
	

LOCAL_C void AddGroupToCard()
	{
//
	test.Next(_L("Group to Card"));
	CContactItem* newGroup = CntTest->Db()->CreateContactGroupL(_L("New Group"));
	TContactItemId tempId = newGroup->Id();
	delete newGroup;
	// assumes Prior Knowldege of contact 3 (is a card)
	CntTest->Db()->AddContactToGroupL(tempId,3);	
//
	}


LOCAL_C void AddBadCardToGroup()
	{
//
	test.Next(_L("Add unknown Card to group"));
	// where id 19 does not exist (assumes knowledge of KLargeSizeRecords+6 as a group)
	CntTest->Db()->AddContactToGroupL(KLargeSizeRecords+9,KLargeSizeRecords+6);	
//
	}

LOCAL_C void AddCardToBadGroup()
	{
//
	test.Next(_L("Add Card to Unknown group"));
	// where id 19 does not exist
	CntTest->Db()->AddContactToGroupL(3,KLargeSizeRecords+9);	
//
	}


LOCAL_C void AddCardToGroupTwice()
	{
//
	test.Next(_L("Add Card to group Twice"));
	// assumes Prior Knowldege of KLargeSizeRecords+6 is a group
	CntTest->Db()->AddContactToGroupL(3,KLargeSizeRecords+6);	
	CntTest->Db()->AddContactToGroupL(3,KLargeSizeRecords+6);	
//
	}

LOCAL_C void AddFieldToGroup()
	{
//
	test.Next(_L("Add Field to Group"));
	// assumes Prior Knowldege of KLargeSizeRecords+6 is a group
	CContactItem* group = CntTest->Db()->OpenContactL(KLargeSizeRecords+6);	
	CleanupStack::PushL(group);
	CContactItemField* field=CContactItemField::NewLC(KStorageTypeText);
	field->SetMapping(KUidContactFieldVCardMapUnusedN);
	field->TextStorage()->SetTextL(_L("Test Field"));
	group->AddFieldL(*field);
//
	CntTest->Db()->CommitContactL(*group);
	CntTest->CloseDatabase();
	CntTest->OpenDatabaseL();
	CContactItem* group2 = CntTest->Db()->OpenContactL(KLargeSizeRecords+6);	
	CContactItemFieldSet& tempFieldSet = group2->CardFields();
	test(tempFieldSet.Count()==2);
	CntTest->Db()->CloseContactL(KLargeSizeRecords+6);
	delete group2;
	CleanupStack::Pop(); // field
	CleanupStack::PopAndDestroy();  // group
	}


LOCAL_C void SetGroupLabel()
	{
//
	test.Next(_L("Set Group Label"));
	// assumes Prior Knowldege of KLargeSizeRecords+6 is a group
	CContactIdArray* groupIds = CntTest->Db()->GetGroupIdListL();
	CleanupStack::PushL(groupIds);
	CContactItem* group = CntTest->Db()->OpenContactL((*groupIds)[0]);	
	CleanupStack::PushL(group);
	TBuf<32> label;
		label.Format(KGroupLabel,1);
	STATIC_CAST(CContactGroup*,group)->SetGroupLabelL(label);
//
	CntTest->Db()->CommitContactL(*group);
	CntTest->CloseDatabase();
	CntTest->OpenDatabaseL();
	CContactItem* group2 = CntTest->Db()->ReadContactLC(KLargeSizeRecords+6,*CntTest->Db()->AllFieldsView());
	TPtrC testLabel = STATIC_CAST(CContactGroup*,group2)->GetGroupLabelL();
	test(testLabel==label);
	//delete group2;
	CleanupStack::PopAndDestroy(3);  // group groupIds group2
	}

LOCAL_C void ChangeGroupLabel()
	{
	test.Next(_L("Change Group Label"));
	// assumes Prior Knowldege of KLargeSizeRecords+6 is a group
	CContactIdArray* groupIds = CntTest->Db()->GetGroupIdListL();
	CleanupStack::PushL(groupIds);
	CContactItem* group = CntTest->Db()->OpenContactL((*groupIds)[0]);	
	CleanupStack::PushL(group);
	TBuf<32> label;
		label.Format(KGroupLabel,1);
	STATIC_CAST(CContactGroup*,group)->SetGroupLabelL(_L("Glenns Group"));
//
	CntTest->Db()->CommitContactL(*group);
	CntTest->CloseDatabase();
	CntTest->OpenDatabaseL();
	CContactItem* group2 = CntTest->Db()->ReadContactLC(KLargeSizeRecords+6,*CntTest->Db()->AllFieldsView());
	TPtrC testLabel = STATIC_CAST(CContactGroup*,group2)->GetGroupLabelL();
	test(testLabel==_L("Glenns Group"));
	//delete group2;
	CleanupStack::PopAndDestroy(3);  // group groupIds group2
	}

// Extended to provide coverage for defect 
// EDNBWHE-4HXCXM "group retains link to a nested group after it has been deleted"
//
LOCAL_C void CheckNestedGroup()
	{	
	test.Next(_L("Check Nested Group"));
	CntTest->CloseDatabase();
	CntTest->DeleteDatabaseL();
	CntTest->CreateDatabaseL();
	CContactItem* newGroup = CntTest->Db()->CreateContactGroupLC(_L("Family Group"));
	TContactItemId groupId = newGroup->Id();
	
	CContactItem* nestedGroup = CntTest->Db()->CreateContactGroupLC(_L("nested Group"));
	TContactItemId nestedGroupId = nestedGroup->Id();

	CntTest->Db()->AddContactToGroupL(nestedGroupId,groupId);

	CntTest->CloseDatabase();
	CntTest->OpenDatabaseL();

	CContactItem* familyGroup = CntTest->Db()->ReadContactLC(groupId);
	CContactIdArray* memberArray = STATIC_CAST(CContactGroup*,familyGroup)->ItemsContainedLC();
	test(memberArray->Count()==1);

	CContactItem* newNestedGroup = CntTest->Db()->ReadContactLC(nestedGroupId);
	CContactIdArray* nestedMemberArray = STATIC_CAST(CContactGroup*,newNestedGroup)->ItemsContainedLC();
	test(nestedMemberArray->Count()==0); //fails as count = 0

	CContactIdArray* groups = STATIC_CAST(CContactGroup*, newNestedGroup)->GroupsJoinedLC();
	test(groups->Count()==1); 
	
	TContactItemId memberShip = (*groups)[0];
	test(memberShip==groupId);
	
	// If delete a group member, then retrieve the group from the database, 
	// group.contactCount() is the same and group.getContacts() includes a 
	// null entry where the deleted group member was.
	CntTest->Db()->DeleteContactL(nestedGroupId);
	CContactItem* newFamilyGroup  = CntTest->Db()->ReadContactLC(groupId);
	CContactIdArray* newMemberArray = STATIC_CAST(CContactGroup*,newFamilyGroup )->ItemsContainedLC();
	test(newMemberArray->Count()==0);		//fails
	
	CleanupStack::PopAndDestroy(9); 
	// newGroup, nestedGroup, familyGroup memberArray newMemberArray newNestedGroup, newFamilyGroup  nestedMemberArray, groups
	}




// Helper function for RenameDefectL() below.  Sets a sort ordering for the database.
//
LOCAL_C void SetSortOrderL(CContactDatabase& aDb, TFieldType aFieldType)
	{
	CArrayFix<CContactDatabase::TSortPref>* sortOrder=new(ELeave) CArrayFixFlat<CContactDatabase::TSortPref>(3);
	CleanupStack::PushL(sortOrder);
	sortOrder->AppendL(CContactDatabase::TSortPref(aFieldType,CContactDatabase::TSortPref::EAsc));
	aDb.SortL(sortOrder);
	CleanupStack::Pop();	// sortOrder
	}


// This tests tries to reproduce defect 
// EDNRTRN-4J5C5E "Groups in Cntmodel cannot be renamed.." 
//
// This defect was caused because groups weren't included in the view and
// MoveInSortArrayL doesn't check that the contact item will be in the 
// iSortedItems list (this list depends on the view chosen).
//
LOCAL_C void RenameDefectL()
	{
	test.Next(_L("Rename a group"));
	CntTest->CloseDatabase();
	CntTest->DeleteDatabaseL();

	CContactDatabase* db=CntTest->CreateDatabaseL();
	_LIT(KCntmodelOriginalName,"Original");
	_LIT(KCntmodelRevisedName,"Revised");
	CContactItem* newGroup = CntTest->Db()->CreateContactGroupLC(KCntmodelOriginalName);
	TContactItemId groupId = newGroup->Id();
	CleanupStack::PopAndDestroy(); //newGroup

	TContactItemId contactA=AddContactL(db,KUidContactFieldFamilyName,KUidContactFieldVCardMapUnusedN,_L("a"));
	TContactItemId contactB=AddContactL(db,KUidContactFieldFamilyName,KUidContactFieldVCardMapUnusedN,_L("b"));
	CntTest->Db()->AddContactToGroupL(contactA,groupId);
	CntTest->Db()->AddContactToGroupL(contactB,groupId);

	CntTest->Db()->SetDbViewContactType(KUidContactCard);
	CntTest->Db()->SortedItemsL(); // ensures theres a iSortedItems
	SetSortOrderL(*db,TUid::Uid(KUidContactFieldDefinedTextValue));		// required so that the comparision 

	CContactGroup* group = static_cast<CContactGroup*>(CntTest->Db()->OpenContactL(groupId));	
	CleanupStack::PushL(group);
	group->SetGroupLabelL(KCntmodelRevisedName);
	CntTest->Db()->CommitContactL(*group);
	CntTest->Db()->CloseContactL(groupId);
	CleanupStack::PopAndDestroy(); //group

	CContactItem* revisedGroup = CntTest->Db()->ReadContactLC(groupId,*CntTest->Db()->AllFieldsView());
	TPtrC testLabel = static_cast<CContactGroup*>(revisedGroup)->GetGroupLabelL();
	test(testLabel==KCntmodelRevisedName);
	CleanupStack::PopAndDestroy(); //revisedGroup
	}

// This tests reproduces defect 
// BET-4YDGB3 "Contacts crashes when creating two entries"
//
// This was caused because the CContactTables::ContactType() method uses the 
// iCurrentIdInIdentityTable to determine which contact should be used to read 
// the type information from. This member stores the last contact ID to be read 
// from the identity table. However, since AddContactToGroupL() doesn't read any 
// information from the identity table, the value set in iCurrentIdInIdentityTable 
// should be set to KErrNotFound so that the correct contact from main CONTACTS table 
// is used.
//
LOCAL_C void QuartzGroupDefectL()
	{
	test.Next(_L("Quartz defect"));
	CntTest->CloseDatabase();
	CntTest->DeleteDatabaseL();

	CContactDatabase* db=CntTest->CreateDatabaseL();

	_LIT(KCntmodelGroupAll,"All");
	CContactItem* group = db->CreateContactGroupLC(KCntmodelGroupAll);
	TContactItemId groupId = group->Id();
	CleanupStack::PopAndDestroy(group); 
	

	CntTest->Db()->SetDbViewContactType(KUidContactCard);
	CntTest->Db()->SortedItemsL(); // ensures theres a iSortedItems
	SetSortOrderL(*db,TUid::Uid(KUidContactFieldDefinedTextValue));		// required so that the comparision 
	
	TContactItemId contactB=AddContactL(db,KUidContactFieldFamilyName,KUidContactFieldVCardMapUnusedN,_L("B"));
	CntTest->Db()->AddContactToGroupL(contactB,groupId);
	
	TContactItemId contactA=AddContactL(db,KUidContactFieldFamilyName,KUidContactFieldVCardMapUnusedN,_L("A"));
	CntTest->Db()->AddContactToGroupL(contactA,groupId);
	}

// Regression testcode for MAT-5B7JD4 "AddContactToGroupL()/RemoveContactFromGroupL() does not update 
// object passed as argument"
//
LOCAL_C void GroupAddRemoveTestL()
	{
	test.Next(_L("Group addition/removal defect"));
	CntTest->CloseDatabase();
	CntTest->DeleteDatabaseL();
	CContactDatabase* db=CntTest->CreateDatabaseL();

	_LIT(KCntmodelGroup,"MyGroup");
	CContactItem* group = db->CreateContactGroupLC(KCntmodelGroup);
	TContactItemId groupId = group->Id();
	
	TContactItemId itemId=AddContactL(db,KUidContactFieldFamilyName,KUidContactFieldVCardMapUnusedN,_L("C"));
	CContactItem* item = CntTest->Db()->ReadContactLC(itemId);

	CntTest->Db()->AddContactToGroupL(*item,*group);
	// write change to database
	const CContactIdArray* groupsJoined = STATIC_CAST(CContactItemPlusGroup*,item)->GroupsJoined();
	test(groupsJoined->Find(groupId)==0);
	const CContactIdArray* itemsContained = STATIC_CAST(CContactGroup*,group)->ItemsContained();
	test(itemsContained->Find(itemId)==0);
	
	CntTest->Db()->RemoveContactFromGroupL(*item,*group);
	test(STATIC_CAST(CContactItemPlusGroup*,item)->GroupsJoined()->Find(groupId)!=0);
	test(STATIC_CAST(CContactGroup*,group)->ItemsContained()->Find(itemId)!=0);

	CleanupStack::PopAndDestroy(item);
	CleanupStack::PopAndDestroy(group); 
	}

void CompareGroupListL(CCntClient& cntClient1, CCntClient& cntClient2)
	{
	CContactItem* group1 = NULL;
	CContactItem* group2 = NULL;
	CContactIdArray* ids1 = cntClient1.Db()->GetGroupIdListL();
	CleanupStack::PushL(ids1);
	CContactIdArray* ids2 = cntClient2.Db()->GetGroupIdListL();
	CleanupStack::PushL(ids2);
	TInt groupCount = ids1->Count();
	test(cntClient1.Db()->GroupCount()==cntClient1.Db()->GroupCount());
	for (TInt i=0;i<groupCount;i++)
		{
		group1 = cntClient1.Db()->ReadContactLC((*ids1)[groupCount-1]);
		group2 = cntClient2.Db()->ReadContactLC((*ids2)[groupCount-1]);
		test(group1->LastModified() == group2->LastModified());
		test(group1->Id() == group2->Id());
		test(group1->Type() == group2->Type());
		CleanupStack::PopAndDestroy(2,group1);// group2
		}
	CleanupStack::PopAndDestroy(2,ids1);//ids2
	}

LOCAL_C void TestGroupNotificationsAndGroupIdListUpdateL()
	{
	test.Next(_L("Test for group addition notification for multifle clients (defect INC018191) "));
	// Open DB from cntClient1
	CCntClient* cntClient1 = new(ELeave) CCntClient(test,KDatabaseFileName);
	CleanupStack::PushL(cntClient1);
	cntClient1->CreateDatabaseL();
		
	// Open DB from cntClient2
	CCntClient* cntClient2 = new(ELeave) CCntClient(test,KDatabaseFileName);
	CleanupStack::PushL(cntClient2);								
	cntClient2->OpenDatabaseL();
	
	// Check that no group is visible from both clients
	CContactIdArray * ids1 = cntClient1->Db()->GetGroupIdListL();
	CleanupStack::PushL(ids1);										
	CContactIdArray * ids2 = cntClient2->Db()->GetGroupIdListL();
	CleanupStack::PushL(ids2);	
	test(ids1->Count()==0);
	test(ids2->Count()==0);
	CleanupStack::PopAndDestroy(2,ids1);// ids2 
	ids1=NULL;
	ids2=NULL;

	// Create a group from the first client
	TRAPD(err,CreateGroupFromAClientL(*cntClient1));	//start the Active Scheduler and then stop it in the event handler
	test(err==KErrNone);

	// Check that the newly added group can be seen by both clients and it is the same object  
	test(cntClient1->Db()->GroupCount()==1);
	test(cntClient2->Db()->GroupCount()==1);
	CompareGroupListL(*cntClient1,*cntClient2);
	
	// Create a group from the second client
	TRAP(err,CreateGroupFromAClientL(*cntClient2));		//start the Active Scheduler and then stop it in the event handler
	test(err==KErrNone);

	// Check that now 2 groups can be seen from both clients ..
	CompareGroupListL(*cntClient1,*cntClient2);
	test(cntClient1->Db()->GroupCount()==2);
	test(cntClient2->Db()->GroupCount()==2);

	// Now close the database from the two clients and re-open it from a third client
	// and chech that there are still 2 groups 
	cntClient1->CloseDatabase();
	cntClient2->CloseDatabase();
	
	CleanupStack::PopAndDestroy(2,cntClient1);// cntClient1, cntClient2

	CCntClient* cntClient3 = new(ELeave) CCntClient(test,KDatabaseFileName);
	CleanupStack::PushL(cntClient3);								
	cntClient3->OpenDatabaseL();
	test(cntClient3->Db()->GroupCount()==2);
	CleanupStack::PopAndDestroy(cntClient3);
	cntClient3 = NULL;
	}

LOCAL_C void TestReadContactTextDefL() // Added to test changes made for DEF065476 and DEF065477
	{
	test.Next(_L("Testing ReadContactTextDef ( Defect DEF065476 and DEF065477 )"));
	CntTest->CloseDatabase();
	CntTest->DeleteDatabaseL();
	CContactDatabase* db=CntTest->CreateDatabaseL();

	_LIT(KContactGroup,"ContactGroup");
	CContactItem* group = db->CreateContactGroupLC(KContactGroup);
	TContactItemId groupId = group->Id();

	TBuf<128> scratchBuf;
	CntTest->Db()->ReadContactTextDefL(groupId, scratchBuf);
	test(scratchBuf==KContactGroup);

	test.Printf(_L("The Group name is %S\n"), &scratchBuf);

	CleanupStack::PopAndDestroy(group); 
	}
	
LOCAL_C void AsyncDbOpenFollowedByAddGroupL()
	{
	test.Next(_L("Async database open & group creating"));

		
	TRequestStatus requestStatus(0);
	CContactOpenOperation* op = CContactDatabase::Open(KDatabaseFileName, requestStatus); 
	// note: op doesn't have to be on CS
	User::WaitForRequest(requestStatus);
	CContactDatabase* database = op->TakeDatabase();
	delete op;
	CleanupStack::PushL(database);
	
	CContactItem* tempGroup = database->CreateContactGroupL(_L("New Group"));
	delete tempGroup;
	CleanupStack::PopAndDestroy(database);
	}	

//////////////////////////////////////////////////////////////////////////
//
//	MAIN DISPATCH MODULE
//
//////////////////////////////////////////////////////////////////////////

/**

@SYMTestCaseID     PIM-T-GROUPS-0001

*/

void DoTestsL()
    {
//	HAL::SetAutoSwitchOffBehavior(ESwitchOffDisabled);
	test.Start(_L("@SYMTESTCaseID:PIM-T-GROUPS-0001 Create new database"));
	TRAPD(err,CntTest->CreateDatabaseL());
	test(err==KErrNone);
	CntTest->CloseDatabase();
	TRAP(err,CntTest->OpenDatabaseL());
	test(err==KErrNone);
	TRAP(err,CreateBasicContactGroups());
	test(err==KErrNone);
	TRAP(err,DeleteBasicContactGroups());
	test(err==KErrNone);
	TRAP(err,CheckBasicPopulatedGroup());
	test(err==KErrNone);
	TRAP(err,CheckRemoveMember());
	test(err==KErrNone);
	TRAP(err,CheckRemoveGroup());
	test(err==KErrNone);
	TRAP(err,CheckLargePopulatedGroup());
	test(err==KErrNone);
	TRAP(err,DeleteGroupMembersCheckPersist());
	test(err==KErrNone);
	TRAP(err,CreateManyGroups());
	test(err==KErrNone);
	TRAP(err,DeleteManyGroups());
	test(err==KErrNone);
	// error checking
	TRAP(err,AddCardToCard());
	test(err==KErrNotSupported);
	TRAP(err,AddGroupToCard());
	test(err==KErrNotSupported);
	TRAP(err,AddBadCardToGroup());
	test(err==KErrNotFound);
	TRAP(err,AddCardToBadGroup());
	test(err==KErrNotFound);
	TRAP(err,AddCardToGroupTwice());
	test(err==KErrNone);
	TRAP(err,AddFieldToGroup());
	test(err==KErrNone);
	TRAP(err,SetGroupLabel());
	test(err==KErrNone);
	TRAP(err,ChangeGroupLabel());
	test(err==KErrNone);
	TRAP(err,CheckNestedGroup());
	test(err==KErrNone);
	TRAP(err,RenameDefectL());
	test(err==KErrNone);
	TRAP(err,QuartzGroupDefectL());
	test(err==KErrNone);
	TRAP(err,GroupAddRemoveTestL());
	test(err==KErrNone);
	TRAP(err, TestReadContactTextDefL());
	test(err==KErrNone);
	CntTest->CloseDatabase();
	TRAP(err,TestGroupNotificationsAndGroupIdListUpdateL());	//Test case for defect INC018191
	test(err==KErrNone);
	TRAP(err, AsyncDbOpenFollowedByAddGroupL());
	test(err==KErrNone);
	test.Next(_L("Delete database"));
	TRAP(err,CntTest->DeleteDatabaseL());
	test(err==KErrNone);
    }

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
    CntTest=new(ELeave) CCntTest;
	CntTest->ConstructL(test,KDatabaseFileName);
    TRAPD(err,DoTestsL());
	CntTest->EndTestLib(err);
	__UHEAP_MARKEND;
	return KErrNone;
    }
