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
* This tests the various options for the ORG property
* This includes
* 1) ORG:company;department
* 2) ORG:Company;
* 3) ORG:;Department
*
*/


#include <e32test.h>
#include <e32debug.h>
#include <f32file.h>
#include <s32file.h>
#include "t_utils.h"

LOCAL_D CCntTest* CntTest=NULL;
LOCAL_D const TPtrC KDatabaseFileName=_L("C:VOrgDb.cdb");
LOCAL_D RTest test(_L("T_VExportORG"));

_LIT8(KOrg1, "ORG:Symbian;PIM\r\n");
//_LIT8(KOrg2, "ORG\r\n");
_LIT8(KOrg3, "ORG:Symbian;\r\n");
_LIT8(KOrg4, "ORG:;PIM\r\n");

_LIT(KVImportFileName, "C:\\vOrgImport.vcf");
_LIT(KVExportFileName, "C:\\vOrgExport.vcf");
_LIT8(KBegin,"BEGIN:VCARD\r\n");
_LIT8(KEnd,"END:VCARD\r\n");
_LIT8(KVersion, "VERSION:2.1\r\n");
_LIT8(KName, "N:Surname;FirstName;;;\r\n");

void createVcardFileL(const TDesC8& aOrgLine, RFs& aFileSession)
{
    RFile file;
    User::LeaveIfError(file.Replace(aFileSession, KVImportFileName, EFileWrite|EFileShareAny));
    CleanupClosePushL(file);
    
    file.Write(KBegin);
    file.Write(KVersion);
    file.Write(KName);
    file.Write(aOrgLine);
    file.Write(KEnd);
    
    file.Flush();
    CleanupStack::PopAndDestroy(); //file.Close()
    
}

void versitDeleteVcardFilesL(RFs& aFileSession)
{
	aFileSession.Delete(KVImportFileName);
	aFileSession.Delete(KVExportFileName);
}

void EmptyDatabase()
	{
	TContactItemId theid;
	TContactIter iter(*CntTest->Db());
	theid=iter.FirstL();
	while(theid!=KNullContactId )
		{
		CntTest->Db()->DeleteContactL(theid);
		theid=iter.NextL();
		}
	}
	
void versitImportContactL(RFs& aFileSession)
{
    RFileReadStream readStream;
		
	//Import vOrgImport.vcf
	User::LeaveIfError(readStream.Open(aFileSession,KVImportFileName, EFileRead));
	CleanupClosePushL(readStream);
	
	TBool success=EFalse;
	CArrayPtr<CContactItem>* contactItems=CntTest->Db()->ImportContactsL(TUid::Uid(KUidVCardConvDefaultImpl),
	                                                                     readStream, success,
	                                                                     CContactDatabase::EImportSingleContact+CContactDatabase::ETTFormat);
	                                                                     
	                                                                     
    //Should be the only contact in the database at this point
	TInt count = CntTest->Db()->CountL();
	test(count == 1);
	
	//Store the id of the new contact - we need this later
	TContactItemId contactId = (*contactItems)[0]->Id();
	CntTest->Db()->SetCurrentItem(contactId);
	
	readStream.Close();                                                                
	CleanupStack::PopAndDestroy(); //readStream.Close()
    contactItems->ResetAndDestroy();
    delete contactItems;
	
}

void versitExportContactL(RFs& aFileSession)
{
    RFileWriteStream writeStream;
    User::LeaveIfError(writeStream.Replace(aFileSession,KVExportFileName,EFileWrite));
    CleanupClosePushL(writeStream);
    
    CContactIdArray* idArray = CContactIdArray::NewLC();
    TInt count = CntTest->Db()->CountL();
	test(count == 1);
    TContactItemId contactId = CntTest->Db()->GetCurrentItem();
    
    idArray->InsertL(0, contactId);
    
    CntTest->Db()->ExportSelectedContactsL(TUid::Uid(KUidVCardConvDefaultImpl), *idArray, writeStream,0);
    writeStream.Close();
    
	CleanupStack::PopAndDestroy(2); //idArray, writeStream
	                                                                     
    	
}

TBool versitCheckExportFilesL(const TDesC8& aOrgLine, RFs& aFileSession)
{
	RFile fileHandle;
	TBool patternFound = EFalse;
	fileHandle.Open(aFileSession, KVExportFileName, EFileRead|EFileStreamText);
	CleanupClosePushL(fileHandle);
	
//	TFileText exportedfile; //Bug in TFileText so not used at the moment
//	exportedfile.Set(fileHandle);
//	exportedfile.Seek(ESeekStart);
	TBuf8<256> line;
	
	while(fileHandle.Read(line) == KErrNone && line.Length() != 0)
	{
		if (line.Find(aOrgLine) != KErrNotFound)
		{
		    patternFound = ETrue;
		    break;	
		}	
	}
	
	CleanupStack::PopAndDestroy(&fileHandle);
	return patternFound;
}


void testExportOrgL(const TDesC8& aOrgLine)
{
    RFs fsSession;
    TBool success = EFalse;
    User::LeaveIfError(fsSession.Connect());
    CleanupClosePushL(fsSession);
    
    createVcardFileL(aOrgLine,fsSession);
    versitImportContactL(fsSession);
    versitExportContactL(fsSession);
    EmptyDatabase();
    
    success = versitCheckExportFilesL(aOrgLine,fsSession);
    
    versitDeleteVcardFilesL(fsSession);
    fsSession.Close();
    CleanupStack::PopAndDestroy(&fsSession); //fsSession.Close() 
    RDebug::RawPrint(aOrgLine);
    if(success == EFalse)
    {   RDebug::RawPrint(aOrgLine);   
    	User::Leave(KErrNotFound);
    }
}

/**

@SYMTestCaseID     PIM-T-VEXPORTORG-0001

*/

void testOrgL()
{
	test.Start(_L("@SYMTESTCaseID:PIM-T-VEXPORTORG-0001 Test Versit export of ORG line"));

	
	CntTest->CreateDatabaseL();
	
	test.Next(_L("ORG:Company;Dept"));

	testExportOrgL(KOrg1);
	
	//test.Next(_L("ORG"));

	//testExportOrgL(KOrg2); 
	
	test.Next(_L("ORG:Company"));

	testExportOrgL(KOrg3);
	
	test.Next(_L("ORG:;Dept"));

	testExportOrgL(KOrg4);
	
		
	CntTest->CloseDatabase();
	
	CntTest->DeleteDatabaseL();
}

GLDEF_C TInt E32Main()
{
	__UHEAP_MARK;
	CntTest = new CCntTest;
	if (!CntTest)
		{
		return KErrNoMemory;
		}

	TRAPD(err, CntTest->ConstructL(test,KDatabaseFileName) );
	test(err == KErrNone);
	
	TRAP(err,testOrgL() );
	test(err == KErrNone);
	
	CntTest->EndTestLib(err);
	
	__UHEAP_MARKEND;
	return KErrNone;
}
