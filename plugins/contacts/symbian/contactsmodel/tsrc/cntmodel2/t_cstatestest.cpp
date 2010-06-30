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
*
*/



#include <e32test.h>
#include "cntdb.h" 

#include "t_cstatestest.h"


COpenCommitTest::COpenCommitTest()
	{}
	
COpenCommitTest* COpenCommitTest::NewLC()
	{
	COpenCommitTest* self = new (ELeave) COpenCommitTest();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;		
	}

void COpenCommitTest::ConstructL()
	{
	TPtrC name(RThread().Name());
	iTest = new(ELeave) RTest(name);

	iCntDb 		 = CContactDatabase::ReplaceL(); // Default Database.
	const CContactTemplate&  sysTempl(GetSysTemplateL()	);
	iCntItemBldr = CCntItemBuilder::NewLC(sysTempl);
	CleanupStack::Pop(iCntItemBldr);
	}

COpenCommitTest::~COpenCommitTest()
	{
	delete iCntDb;
	delete iCntItemBldr;
	delete iGoldenTemplate;
	iTest->Close();
	delete iTest;
	}

const CContactTemplate&  COpenCommitTest::GetSysTemplateL()
	{
	if (!iGoldenTemplate)
		{
		CContactItemViewDef* matchAll = CContactItemViewDef::NewLC(CContactItemViewDef::EIncludeFields,CContactItemViewDef::EIncludeHiddenFields);
		matchAll->AddL(KUidContactFieldMatchAll);

		iGoldenTemplate = static_cast<CContactTemplate*>(iCntDb->ReadContactL(iCntDb->TemplateId(), *matchAll));
		CleanupStack::PopAndDestroy(matchAll);
		}
	return *iGoldenTemplate;
	}
	
	




void COpenCommitTest::RunOpenCommitTestL()
	{
	iTest->Printf(_L(" ---- Open & Commit Test Started ---- \r\n"));
	// Create a new ContactItem & add to DB
	CContactItem* cntItem = iCntItemBldr->GetCntItemLC();
	TContactItemId cntId  = iCntDb->AddNewContactL(*cntItem);

	// Simple Open & Commit
	CContactItem* openCntItem = iCntDb->OpenContactLX(cntId);
	CleanupStack::PushL(openCntItem);
	iCntDb->CommitContactL(*openCntItem);
	CleanupStack::PopAndDestroy(openCntItem);
	CleanupStack::Pop(); // Pop the lock
		
	
	CContactItemViewDef* matchAll = CContactItemViewDef::NewLC(CContactItemViewDef::EIncludeFields,CContactItemViewDef::EIncludeHiddenFields);
	matchAll->AddL(KUidContactFieldMatchAll);
	openCntItem = iCntDb->OpenContactLX(cntId, *matchAll);
	CleanupStack::PushL(openCntItem);
	iCntDb->CommitContactL(*openCntItem);
	CleanupStack::PopAndDestroy(openCntItem);
	CleanupStack::Pop(); // Pop the lock

	// Commit without Open
	TRAPD(err, iCntDb->CommitContactL(*cntItem));
	if (err != KErrNotFound)
		User::Leave(err);

	CleanupStack::PopAndDestroy(matchAll);
	matchAll = NULL;
	CleanupStack::PopAndDestroy(cntItem);
	cntItem = NULL;
	iTest->Printf(_L(" ---- Open & Commit Test Completed OK ---- \r\n"));
	}




void COpenCommitTest::RunGroupTestL()
	{
  	iTest->Printf(_L(" ---- Group Test Started ---- \r\n"));
	CContactItem* cntItemGrp = iCntDb->CreateContactGroupLC();
	TContactItemId grpId     = iCntDb->AddNewContactL(*cntItemGrp);
	CleanupStack::PopAndDestroy(cntItemGrp);
	cntItemGrp = NULL;
	
	CContactItem*  cntItem      = iCntItemBldr->GetCntItemLC();
	iCntDb->AddNewContactL(*cntItem);
	
	// Open ContactItem - this will lock the contactItem. 
	// The AddContactToGroupL calls commit, which unlocks the item.
	
	CContactGroup* cntItemGroup = static_cast<CContactGroup*>(iCntDb->OpenContactLX(grpId));
	// Close the contact - AddContactToGroup opens it.
	iCntDb->CloseContactL(cntItemGroup->Id()); 
	CleanupStack::PushL(cntItemGroup);
	iCntDb->AddContactToGroupL(*cntItem, *cntItemGroup);
	iCntDb->RemoveContactFromGroupL(cntItem->Id(), cntItemGroup->Id());
	
	CleanupStack::PopAndDestroy(cntItemGroup);
	cntItemGroup = NULL;
	CleanupStack::Pop(); // Pop the lock

	CleanupStack::PopAndDestroy(cntItem);
	cntItem = NULL;
	
	iTest->Printf(_L(" ---- Group Test Completed OK ---- \r\n"));
	}
