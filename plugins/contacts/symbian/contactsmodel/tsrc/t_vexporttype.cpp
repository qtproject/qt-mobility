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
* This tests the versit export when the TYPE parameter name is missing.
* E.g PHOTO;GIF;BASE64: is present in the vcf file instead of PHOTO;TYPEGIF;BASE64:
* which the vcard specifications states is acceptable
*
*/


#include <e32test.h>
#include <e32debug.h>
#include <f32file.h>
#include <s32file.h>
#include "t_utils.h"

LOCAL_D CCntTest* CntTest=NULL;
LOCAL_D const TPtrC KDatabaseFileName=_L("C:VTypeDb.cdb");
LOCAL_D RTest test(_L("T_VExportType"));
LOCAL_D RFs fsSession;

_LIT8(KTypePattern, "TYPE=GIF");


_LIT(KVImportFileName, "Z:\\t_cntvcard\\T_VExportTypeWithGif.vcf");
_LIT(KVExportFileName, "C:\\vTypeExport.vcf");
_LIT(KVReExportedFileName, "C:\\vTypeRe-export.vcf");



void DeleteVcardFilesL()
    {
    User::LeaveIfError(fsSession.Connect());
	CleanupClosePushL(fsSession);
	
	fsSession.Delete(KVExportFileName);
	fsSession.Delete(KVReExportedFileName);
	
	fsSession.Close();
    CleanupStack::PopAndDestroy(&fsSession);
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
	
void VersitImportContactL(const TDesC& aImportFileName)
{
    RFileReadStream readStream;
		
	//Import aImportFileName
	User::LeaveIfError(readStream.Open(fsSession, aImportFileName, EFileRead));
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

void VersitExportContactL(const TDesC& aExportFileName)
{
    RFileWriteStream writeStream;
    User::LeaveIfError(writeStream.Replace(fsSession, aExportFileName, EFileWrite));
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

TBool VersitCheckPatternL(const TDesC& aExportFileName, const TDesC8& aExportedPattern)
{
	RFile fileHandle;
	TBool patternFound = EFalse;
	fileHandle.Open(fsSession, aExportFileName, EFileRead|EFileStreamText);
	CleanupClosePushL(fileHandle);
	
//	TFileText exportedfile; //Bug in TFileText so not used at the moment
//	exportedfile.Set(fileHandle);
//	exportedfile.Seek(ESeekStart);
	TBuf8<256> line;
	
	while(fileHandle.Read(line) == KErrNone && line.Length() != 0)
	{
		if (line.Find(aExportedPattern) != KErrNotFound)
		{
		    patternFound = ETrue;
		    break;	
		}	
	}
	
	CleanupStack::PopAndDestroy(&fileHandle);
	return patternFound;
}



void TestImportExportTypeL(const TDesC& aImportFileName,const TDesC& aExportFileName, const TDesC8& aExportedPattern)
    {
    VersitImportContactL(aImportFileName);
    
    VersitExportContactL(aExportFileName);
    
    test(VersitCheckPatternL(aExportFileName, aExportedPattern));
    }

/**

@SYMTestCaseID     PIM-T-VEXPORTTYPE-0001

*/

void TestTypeL()
    {
	test.Start(_L("@SYMTESTCaseID:PIM-T-VEXPORTTYPE-0001 Test Versit export of Type line"));

	
	User::LeaveIfError(fsSession.Connect());
	CleanupClosePushL(fsSession);
	
	CntTest->CreateDatabaseL();
	
	test.Next(_L("PHOTO;GIF;BASE64:"));

	
	TestImportExportTypeL(KVImportFileName, KVExportFileName, KTypePattern);
	
	EmptyDatabase();
	
	test.Next(_L("PHOTO;TYPE=GIF;BASE64:"));

	
	TestImportExportTypeL(KVExportFileName, KVReExportedFileName, KTypePattern);
	
	EmptyDatabase();
	
	fsSession.Close();
    CleanupStack::PopAndDestroy(&fsSession);
	
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
		
	TRAP(err, TestTypeL() );
	test(err == KErrNone);
	
	TRAP_IGNORE(DeleteVcardFilesL()); //Tidy up the exported files
	
	CntTest->EndTestLib(err);
	
	__UHEAP_MARKEND;
	return KErrNone;
    }
