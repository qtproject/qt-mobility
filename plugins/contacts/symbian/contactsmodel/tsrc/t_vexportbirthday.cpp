/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* This harness Creates a Contact Item and adds a single field to it (a birthday)
* This is then exported to a vcard and the date from this export is checked.
* This is to prove that INC038574 that time is not exported with the birthday and therefore
* time differences from UTC do not affect the birthday date.
*
*/


#include <vutil.h>
#include <e32test.h>
#include <cntdb.h>
#include <cntvcard.h>
#include <cntitem.h>
#include <cntfield.h>
#include <cntfldst.h>
#include "t_utils.h"

LOCAL_D CCntTest* CntTest=NULL;
LOCAL_D const TPtrC KDatabaseFileName=_L("C:VBirthdayDb.cdb");

LOCAL_D RTest test(_L("T_VExportBirthday"));

LOCAL_C void MainTestsL()
/**
 *Testing the vcard export of a birthday field from the contact model.
 *
 */
	{
	CContactDatabase *db=CntTest->CreateDatabaseL();
	
	// Versit treats months and days as being numbers from 0, therefore EAugust is 7, and the 
	// day here (0) represents the first day.  Some seconds are added to this date,
	// but the birthday should only be exported as a date without time.
	TDateTime date(1976,EAugust,0,22,0,0,0);
	_LIT8(KExpectedExportDateStr,"19760801");
		
	//Create a contact with just a birthday date
	const TContactItemId templateId = db->TemplateId();
	CContactItem* templateCard = db->ReadContactLC(templateId);
	CContactCard* card = CContactCard::NewL(templateCard); 
	CleanupStack::PushL(card);
	//
	CContactItemFieldSet& cardFields=card->CardFields();
	// Set the birthday
	TInt pos=cardFields.Find(KUidContactFieldBirthday);
	if (pos!=KErrNotFound)
		{
		cardFields[pos].SetMapping(KUidContactFieldVCardMapBDAY);
		cardFields[pos].DateTimeStorage()->SetTime(date);
		}
	
	//Create contact now
	const TContactItemId id = db->AddNewContactL(*card);
	CleanupStack::PopAndDestroy(2, templateCard); 
	CContactIdArray* idArray = CContactIdArray::NewLC();
	idArray->AddL(id);
	
	//export contact to buf
	TBuf8<256> buf;
	RDesWriteStream vCardWriteStream(buf);
	vCardWriteStream.PushL();
	db->ExportSelectedContactsL(TUid::Uid(KUidVCardConvDefaultImpl),*idArray,vCardWriteStream,CContactVCardConverter::EDefault);
	vCardWriteStream.CommitL();
	CleanupStack::PopAndDestroy(&vCardWriteStream);
	CleanupStack::PopAndDestroy(idArray); 
	
	//check there is no date in export
	pos=buf.Find(KVersitTokenBDAY);
	test(pos>0);
	pos+=5;
	_LIT8(KEndOfLine,"\r\n");
	TInt datePos = buf.Mid(pos).Find(KEndOfLine);
	TPtrC8 extractedDate = buf.Mid(pos,datePos);
	test(TPtrC8(KExpectedExportDateStr)==extractedDate);
	
	CntTest->CloseDatabase();
	}

/**

@SYMTestCaseID     PIM-T-VEXPORTBIRTHDAY-0001

*/

void DoTestsL()
    {
	test.Start(_L("@SYMTESTCaseID:PIM-T-VEXPORTBIRTHDAY-0001 Test Versit Export of a Birthday Date"));

	TRAPD(ret,MainTestsL());
	test(ret==KErrNone);

	test.Next(_L("Delete database"));

	TRAPD(ret2,CntTest->DeleteDatabaseL());
	test(ret2==KErrNone);
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


