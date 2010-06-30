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


#include <e32std.h>
#include <e32test.h>
#include <s32file.h>
#include <s32mem.h>
#include <cntdef.h>
#include <cntdb.h>
#include <cntitem.h>
#include <cntfldst.h>
#include <vcard.h>

#include "t_cntpbapsupport.h"
#include "t_testdata.h"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "cntdb_internal.h"
#endif

_LIT(KTestName,"T_CntPBAPSupport");
// Configure database location
_LIT(KDbFileName,"C:T_CntPBAPSupport.cdb");
//Temporary Export file
_LIT(KTempExportFile,"c:\\TempExport.vcf");

RTest test( KTestName );

/**
Cleanup function
*/
void CleanUpResetAndDestroy(TAny* aArray)
	{
	if (aArray)
		{
		CArrayPtr<CContactItem>* array = (CArrayPtr<CContactItem>*)aArray;
		array->ResetAndDestroy();
		delete array;
		}
	}
	
/**
NewL	
*/
CCntPBAPSupport* CCntPBAPSupport::NewL()
	{
	CCntPBAPSupport* self = new(ELeave) CCntPBAPSupport();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	} 
 
/**
Constructor
*/
CCntPBAPSupport::CCntPBAPSupport()
 	{
 	} 

/**
Destructor:deletes the temporary file created during test execution
*/	
CCntPBAPSupport::~CCntPBAPSupport()
 	{
	if (iSessionConnected)
		{
		iFsSession.Delete(KTempExportFile());
		iFsSession.Close();
		}
	delete iDb;
	TRAP_IGNORE(CContactDatabase::DeleteDatabaseL(KDbFileName));
 	delete iIdArray;
 	} 
 	
/**
ConstructL
*/
void CCntPBAPSupport::ConstructL()
 	{
 	User::LeaveIfError(iFsSession.Connect());
	iSessionConnected = ETrue;
	iDb = CContactDatabase::ReplaceL(KDbFileName);
	iIdArray = CContactIdArray::NewL();
	iAddIntraProperties = EFalse;
 	}
 	
/**
Enables or Disables export of Intra-Contact property 	
*/
void CCntPBAPSupport::SetIntraExport(TBool aState)
	{
	iAddIntraProperties = aState;
	}
	
/**
Creates stream from literal containing vCard and imports contacts.
Ids are stored for exporting the contacts later.
*/	
void CCntPBAPSupport::ImportContactsL(const TDesC8& aVCard)
	{
	RDesReadStream vcard(aVCard);
	CleanupClosePushL(vcard);
	CArrayPtr<CContactItem>* contactItems;
	TBool success = EFalse;
	contactItems = iDb->ImportContactsL(TUid::Uid(KUidVCardConvDefaultImpl), vcard, success, CContactDatabase::ETTFormat);
	CleanupStack::PushL(TCleanupItem(CleanUpResetAndDestroy,contactItems));
	iIdArray->Reset();
	TContactItemId contactId;
	for(TInt loop = 0;loop < contactItems->Count();++loop)
		{
		contactId = (*contactItems)[loop]->Id();
		iIdArray->AddL(contactId);		
		}
	CleanupStack::PopAndDestroy(contactItems);
	CleanupStack::PopAndDestroy(&vcard);
	}

/**
Calls new contacts Export API to export contacts.
*/
void CCntPBAPSupport::ExportContactsL(TInt64 aFilter, TVCardVersion aVer, TBool aExportTel)
	{
	RFile outfile;
	outfile.Replace(iFsSession, KTempExportFile(), EFileWrite);
	CleanupClosePushL(outfile);
	RFileWriteStream writeStream(outfile);
	CleanupClosePushL(writeStream);
	
	TUid pluginUid;
	if(aVer == EPBAPVCard21 || aVer == EPBAPVCard30)
		{
		pluginUid.iUid = KUidPBAPVCardConvImpl;	
		}
	else
		{
		pluginUid.iUid = KUidVCardConvDefaultImpl;				
		}
	iDb->ExportSelectedContactsL(pluginUid, *iIdArray, writeStream, CContactDatabase::EDefault, aFilter, this, aVer, aExportTel);
	writeStream.CommitL();
	CleanupStack::PopAndDestroy(2);
	}


void CCntPBAPSupport::CompareExportsL(const TDesC8& aExpected)
    {
    
	RFile exportedFile;	
	TInt err = exportedFile.Open(iFsSession, KTempExportFile(), EFileRead);
	if (err != KErrNone)
		{
		ReportFileErrorL(err, KTempExportFile());	
		}
	CleanupClosePushL(exportedFile);
	RFileReadStream stream1(exportedFile);
	CleanupClosePushL(stream1);

	TBuf8<0x80> bufO;
	
	TInt line = 1;
	do {
		TRAP(err, stream1.ReadL(bufO, TChar(0xa)));
		if (err != KErrNone && err != KErrEof)
			{
			User::Leave(err);	
			}
        			
    	_LIT8(KPropREV,"REV:");
    	_LIT8(KPropUID,"UID:");
		TBufC8<4> first4char(bufO.Left(4));
		if(!first4char.Compare(KPropREV()) || !first4char.Compare(KPropUID()))
		    {
		    continue;
		    }
		
        if (KErrNotFound == aExpected.Find(bufO))
        	{
        	TBuf<256> info;
            info.Copy(bufO);
            TRefByValue<const TDesC> format(info);
            test.Printf(format);
            test (EFalse);
        	}
		++line;
		} while (err != KErrEof);
	CleanupStack::PopAndDestroy(2,&exportedFile);
    }

/**
Opens the exported file and compares it with expected vCard file passed as argument.
*/
void CCntPBAPSupport::CompareFilesL(RReadStream& aCompareStream)
	{
	RFile exportedFile;	
	TInt err = exportedFile.Open(iFsSession, KTempExportFile(), EFileRead);
	if (err != KErrNone)
		{
		ReportFileErrorL(err, KTempExportFile());	
		}
	CleanupClosePushL(exportedFile);
	RFileReadStream stream1(exportedFile);
	CleanupClosePushL(stream1);

	TBuf8<0x80> bufO,bufC;
	
	TInt line = 1;
	do {
		TRAP(err, stream1.ReadL(bufO, TChar(0xa)));
		if (err == KErrNone || err == KErrEof)
			{
			TRAP(err, aCompareStream.ReadL(bufC, TChar(0xa)));	
			}
		if (err != KErrNone && err != KErrEof)
			{
			User::Leave(err);	
			}
		test(CompareLines(bufO, bufC));
		++line;
		} while (err != KErrEof);
	CleanupStack::PopAndDestroy(2,&exportedFile);
	}
	
/**
Compares each line, REV and UID are ignored as they can be different
*/
TBool CCntPBAPSupport::CompareLines(const TDesC8& aLine1, const TDesC8& aLine2)
	{
	_LIT8(KPropREV,"REV:");
	_LIT8(KPropUID,"UID:");

	TBool ret =	!aLine1.Compare(aLine2);
	if (!ret)
		{
		TBufC8<4> first4char(aLine1.Left(4));
		if(!first4char.Compare(KPropREV()) || !first4char.Compare(KPropUID()))
			{
			return ETrue;
			}
		}
	return ret;
	}

/**
Reports any errors occured while opening a file.
*/
void CCntPBAPSupport::ReportFileErrorL(TInt aErr, const TDesC& aFileName)
	{
	if (aErr == KErrNotFound)
		{
		_LIT(KMissingFile,"\r\n** FILE NOT FOUND: %S\r\n\r\n");
		test.Printf(KMissingFile, &aFileName);
		}
	else if (aErr == KErrPathNotFound)
		{	//You will get this error if you have not exported the test data files.
		_LIT(KMissingPath,"\r\n** PATH NOT FOUND: %S\r\n\r\n");
		test.Printf(KMissingPath, &aFileName);
		}
	else
		{
		_LIT(KError,"Error %d opening file %S\r\n");
		test.Printf(KError, aErr, &aFileName);
		}
	__DEBUGGER();
	User::Leave(aErr);
	}
	
/**
Add intra contact properties when called by Cntmodel/Cntvcard, clients can use aContactId to 
collect any info about that contact and create a property for export.
All property parameters should be in form of name=value pair if exported as vCard3.0	
*/
void  CCntPBAPSupport::AddIntraContactPropertiesL(const TContactItemId& /*aContactId*/, CArrayPtr<CParserProperty>* aPropertyList)
	{
	_LIT8(KVersitIntraProp,"X-IRMC-CALL-DATETIME");
	_LIT8(KParam,"TYPE");
	_LIT8(KParamVal,"MISSED");

	if(iAddIntraProperties)
		{
	    CArrayPtr<CParserParam>* arrayOfParams = new(ELeave) CArrayPtrFlat<CParserParam>(5);
		CleanupStack::PushL(TCleanupItem(CleanUpResetAndDestroy, arrayOfParams));

		CParserParam* param = CParserParam::NewL(KParam, KParamVal);
		CleanupStack::PushL(param);
		arrayOfParams->AppendL(param);
		CleanupStack::Pop(param);

		TDateTime missedTime(2006,EOctober,10,10,30,0,0);
		TVersitDateTime* dateTime = new(ELeave)TVersitDateTime(missedTime, TVersitDateTime::EIsUTC);
		CleanupStack::PushL(dateTime);
		CParserPropertyValueDateTime* dateTimeValue = new(ELeave) CParserPropertyValueDateTime(dateTime);
		CleanupStack::Pop(dateTime);

		CleanupStack::PushL(dateTimeValue);
		CParserProperty* property = CParserGroupedProperty::NewL(*dateTimeValue, KVersitIntraProp, NULL, arrayOfParams);
		CleanupStack::Pop(dateTimeValue);
		CleanupStack::PushL(property);	
		aPropertyList->AppendL(property);
		CleanupStack::Pop(property);
		CleanupStack::Pop(arrayOfParams);
		}
	}

/**
@SYMPREQ 1375
@SYMTestType UT
@SYMTestPriority Critical
@SYMTestCaseDesc Export all supported fields mentioned in PBAP spec.

@SYMTestActions
1.Import contact item having all supported fields
2.Export contact as vCard2.1, include TEL export, no addition of Intra-Contact properties
3.Export contact as vCard30, include TEL export, no addition of Intra-Contact properties
4.Compare exported file with expected output.

@SYMTestExpectedResults For the above tests:
1.Only supported fields should be exported.
2.Export of CLASS,NICKNAME,CATEGORIES,PRODID,SORT-STRING,AGENT is not supported.
4.Comparison of expected vCard should pass.
5.No leaves or panics should occur

*/
void CCntPBAPSupport::TestExportOfAllSupportedFieldsL()
	{
	ImportContactsL(KAllSupportedFields());
	SetIntraExport(EFalse);
	TInt64 filter = 0; //All supported fields should be exported
	//Test for vCard2.1
	ExportContactsL(filter,EPBAPVCard21,ETrue);
	//RDesReadStream compareStream(KAllExpectedExport21());
	//CleanupClosePushL(compareStream);
	//CompareFilesL(compareStream);
	CompareExportsL(KAllExpectedExport21());
	test.Printf(_L("Expected export of PBAP supported fields for vCard2.1 verified\n"));
	//CleanupStack::PopAndDestroy(&compareStream);
	//Test for vCard3.0
	ExportContactsL(filter,EPBAPVCard30,ETrue);
	CompareExportsL(KAllExpectedExport30());
	//RDesReadStream compareStream2(KAllExpectedExport30());
	//CleanupClosePushL(compareStream2);
	//CompareFilesL(compareStream2);
	test.Printf(_L("Expected export of PBAP supported fields for vCard3.0 verified\n"));
	//CleanupStack::PopAndDestroy(&compareStream2);
	}
	
/**
@SYMPREQ 1375
@SYMTestType UT
@SYMTestPriority Critical
@SYMTestCaseDesc Tests export of the Mandatory properties,populated and empty(N,FN
@SYMTestCaseDesc and TEL).Export of TEL is required by PBAP but client has to specify it explictly.

@SYMTestActions
1.Contact item having all populated supported fields present in database.
2.Export contact as vCard2.1 when filter has not included any of the mandatory properties.
3.Export contact as vCard3.0 when filter has not included any of the mandatory properties.
4.Import vCard which is not having any mandatory properties.
5.Export contact as vCard3.0 when filter has not included any of the mandatory properties.
6.Export contact as vCard2.1 when filter has not included any of the mandatory properties.
7.Export contact when export of TEL has been disabled by providing EFalse to aExportTEL.
8.Compare exported file with expected output.

@SYMTestExpectedResults 
1.Mandatory properties N,TEL should be exported.
2.Mandatory properties N,FN,TEL should be exported..
3.Contact created without mandatory properties.
4.Empty mandatory properties should be exported.
5.Empty mandatory properties should be exported.
6.All mandatory properties except TEL should be exported.
7.Comparison of expected vCard should pass.
8.No leaves or panics should occur

*/
void CCntPBAPSupport::TestMandatoryAndTELPropertiesL()
	{
	SetIntraExport(EFalse);
	//filter needs to be non-Zero,This field is not present, so only mandatory ones shall be exported
	TInt64 filter = EPropertySORTSTRING;
	ExportContactsL(filter,EPBAPVCard21,ETrue);
	CompareExportsL(KMandatoryPopulatedProperties21());
	test.Printf(_L("Expected export of populated mandatory properties for vCard2.1 verified\n"));
	/*
	RDesReadStream compareStream(KMandatoryPopulatedProperties21());
	CleanupClosePushL(compareStream);
	CompareFilesL(compareStream);
	CleanupStack::PopAndDestroy(&compareStream);
	*/
	//Test vCard3.0
	ExportContactsL(filter,EPBAPVCard30,ETrue);
	CompareExportsL(KMandatoryPopulatedProperties30());
	test.Printf(_L("Expected export of populated mandatory properties for vCard3.0 verified\n"));
	/*
	RDesReadStream compareStream2(KMandatoryPopulatedProperties30());
	CleanupClosePushL(compareStream2);
	CompareFilesL(compareStream2);
	test.Printf(_L("Expected export of populated mandatory properties for vCard3.0 verified\n"));
	CleanupStack::PopAndDestroy(&compareStream2);
	*/
	//Empty fields
	ImportContactsL(KvCardWithoutMandatoryProperties());
	ExportContactsL(filter,EPBAPVCard21,ETrue);
	CompareExportsL(KEmptyMandatoryProperties21());
	test.Printf(_L("Expected export of Empty mandatory properties for vCard2.1 verified\n"));
	/*
	RDesReadStream compareStream3(KEmptyMandatoryProperties21());
	CleanupClosePushL(compareStream3);
	CompareFilesL(compareStream3);
	test.Printf(_L("Expected export of Empty mandatory properties for vCard2.1 verified\n"));
	CleanupStack::PopAndDestroy(&compareStream3);
	*/
	//Test vCard3.0
	ExportContactsL(filter,EPBAPVCard30,ETrue);
	CompareExportsL(KEmptyMandatoryProperties30());
	test.Printf(_L("Expected export of Empty mandatory properties for vCard3.0 verified\n"));
	/*
	RDesReadStream compareStream4(KEmptyMandatoryProperties30());
	CleanupClosePushL(compareStream4);
	CompareFilesL(compareStream4);
	test.Printf(_L("Expected export of Empty mandatory properties for vCard3.0 verified\n"));
	CleanupStack::PopAndDestroy(&compareStream4);
	*/
	//Test for export of TEL property
	ExportContactsL(filter,EPBAPVCard30,EFalse);
	CompareExportsL(KTELPropertyDisabled30());
	test.Printf(_L("Disabling of TEL export verified\n"));
	/*
	RDesReadStream compareStream5(KTELPropertyDisabled30());
	CleanupClosePushL(compareStream5);
	CompareFilesL(compareStream5);
	test.Printf(_L("Disabling of TEL export verified\n"));
	CleanupStack::PopAndDestroy(&compareStream5);
	*/
	}

/**
@SYMPREQ 1375
@SYMTestType UT
@SYMTestPriority Critical
@SYMTestCaseDesc Test the functionality of adding intra-contact property by callback function.
This function is called by PBAP export Plug-in during export.

@SYMTestActions
1.Import contact item having all supported fields
2.Prepare filter to enable export of required populated contact fields.
3.Export contact as vCard30 and compare with expected vcard,include TEL export.
4.Add one Intra-contact property when callback function is called.
5.Compare exported file with expected output.

@SYMTestExpectedResults For the above tests:
1.Export should contain mandatory properties + TEL and properties mentioned in filter.
2.Exported vCard should contain Intra-Contact property provided by callback function.
3.Comparison of expected vCard should pass.
4.No leaves or panics should occur
*/
void CCntPBAPSupport::TestIntraContactPropertiesL()
	{
	SetIntraExport(ETrue);
	ImportContactsL(KAllSupportedFields());
	TInt64 filter = EPropertyLABEL;
	filter |= EPropertyTITLE;
	filter |= EPropertyNOTE;
	ExportContactsL(filter, EPBAPVCard30,ETrue);
	CompareExportsL(KIntraContactExport());
	test.Printf(_L("Expected export of IntraContact Property verified\n"));
	/*
	RDesReadStream compareStream(KIntraContactExport());
	CleanupClosePushL(compareStream);
	CompareFilesL(compareStream);
	test.Printf(_L("Expected export of IntraContact Property verified\n"));
	CleanupStack::PopAndDestroy(&compareStream);
	*/
	}
	
/**
@SYMPREQ 1375
@SYMTestType UT
@SYMTestPriority Critical
@SYMTestCaseDesc Export contact item having unnamed parameters as vCard following vCard3.0 specification,
all such parameters should be prefixed with "TYPE=".

@SYMTestActions
1.Import contact item having fields with unnamed parameter values.
2.Create an empty filter.
3.Export contact as vCard30,export of TEL enabled.
4.Compare exported file with expected output.

@SYMTestExpectedResults For the above tests:
1.All supported and populated fields will be exported if empty filter is provided.
2.All unnamed parameters should be prefixed with "TYPE=".
4.Comparison of expected vCard should pass.
5.No leaves or panics should occur
*/
void CCntPBAPSupport::TestTypePrefixingL()
	{
	SetIntraExport(EFalse);
	ImportContactsL(KTestTypePrefix());
	TInt64 filter = 0;
	ExportContactsL(filter, EPBAPVCard30,ETrue);
	CompareExportsL(KExpectedTypePrefix30());
	test.Printf(_L("Type= prefix for vCard3.0 verified\n"));
	/*
	RDesReadStream compareStream(KExpectedTypePrefix30());
	CleanupClosePushL(compareStream);
	CompareFilesL(compareStream);
	test.Printf(_L("Type= prefix for vCard3.0 verified\n"));
	CleanupStack::PopAndDestroy(&compareStream);
	*/
	}

/**
@SYMPREQ 1375
@SYMTestType UT
@SYMTestPriority Critical
@SYMTestCaseDesc Tests if properties mentioned in filter are exported.

@SYMTestActions
1.Import vCard to create contact with populated fields.
2.Create 3 filters, each one enabling export of few fields.
3.Enable export of TEL for last two test.
4.Export contact and compare the expected output.

@SYMTestExpectedResults For the above tests:
1.Resulting vCards should contain only mandatory + populated properties present in the filter.
2.Comparison of expected vCard should pass.
3.No leaves or panics should occur
*/
void CCntPBAPSupport::TestFilterL()
	{
	SetIntraExport(EFalse);
	ImportContactsL(KAllSupportedFields());
	//Try with three filters, each enable export of few fields.	
	//Filter1, export LABEL,TITLE,NOTE,ADR apart from mandatory + TEL
	TInt64 filter = EPropertyLABEL;
	filter |= EPropertyTITLE;
	filter |= EPropertyNOTE;
	filter |= EPropertyADR;
	ExportContactsL(filter, EPBAPVCard30, ETrue);
	CompareExportsL(KExpected1FilteredProp30());
	test.Printf(_L("First Filter for vCard3.0 verified\n"));
	/*
 	RDesReadStream compareStream(KExpected1FilteredProp30());
	CleanupClosePushL(compareStream);
	CompareFilesL(compareStream);
	test.Printf(_L("First Filter for vCard3.0 verified\n"));
	CleanupStack::PopAndDestroy(&compareStream);
	*/
	//Filter2, export EMAIL,ORG,ROLE,URL + mandatory and no TEL.
	filter = 0;
	filter |= EPropertyROLE;
	filter |= EPropertyEMAIL;
	filter |= EPropertyORG;
	filter |= EPropertyURL;
	ExportContactsL(filter, EPBAPVCard30, EFalse);
	CompareExportsL(KExpected2FilteredProp30());
	test.Printf(_L("Second Filter for vCard3.0 without TEL verified\n"));
	/*
 	RDesReadStream compareStream2(KExpected2FilteredProp30());
	CleanupClosePushL(compareStream2);
	CompareFilesL(compareStream2);
	test.Printf(_L("Second Filter for vCard3.0 without TEL verified\n"));
	CleanupStack::PopAndDestroy(&compareStream2);
	*/
	//Filter3, export PHOTO,LOGO,BDAY,MAILER,SOUND,KEY + mandatory and no TEL to 2.1
	filter = 0;
	filter |= EPropertyPHOTO;
	filter |= EPropertyLOGO;
	filter |= EPropertyBDAY;
	filter |= EPropertyMAILER;
	filter |= EPropertySOUND;
	filter |= EPropertyKEY;
	ExportContactsL(filter, EPBAPVCard21, EFalse);
	CompareExportsL(KExpected3FilteredProp21());
	test.Printf(_L("Third Filter on vCard2.1 without TEL verified\n"));
	/*
 	RDesReadStream compareStream3(KExpected3FilteredProp21());
	CleanupClosePushL(compareStream3);
	CompareFilesL(compareStream3);
	test.Printf(_L("Third Filter on vCard2.1 without TEL verified\n"));
	CleanupStack::PopAndDestroy(&compareStream3);
	*/
	}

/**
@SYMPREQ 1375
@SYMTestType UT
@SYMTestPriority Critical
@SYMTestCaseDesc Tests if standard vCard2.1 support has been provided or not.

@SYMTestActions
1.Import vCard to create contact with populated fields.
2.Provide EVCard21 as format of export.
3.Export contact and compare the expected output.

@SYMTestExpectedResults For the above tests:
1.Resulting vCards should contain all populated properties supported by standard vCard2.1 export.
2.Comparison of expected vCard should pass.
3.No leaves or panics should occur
*/
void CCntPBAPSupport::TestVCard21SupportL()
	{
	ImportContactsL(KAllSupportedFields());
	TInt64 filter = 0;
	ExportContactsL(filter, EVCard21, ETrue);
	CompareExportsL(KExpectedStandard21vCard());
	test.Printf(_L("Expected export of standard vCard2.1 verified\n"));
	/*
	RDesReadStream compareStream(KExpectedStandard21vCard());
	CleanupClosePushL(compareStream);
	CompareFilesL(compareStream);
	test.Printf(_L("Expected export of standard vCard2.1 verified\n"));
	CleanupStack::PopAndDestroy(&compareStream);
	*/
	}
/**
Initiates test functions.
*/	
void DoTaskL()
	{	
	CCntPBAPSupport* pbapTest = CCntPBAPSupport::NewL();
	CleanupStack::PushL(pbapTest);
	pbapTest->TestExportOfAllSupportedFieldsL();
	pbapTest->TestMandatoryAndTELPropertiesL();
	pbapTest->TestIntraContactPropertiesL();
	pbapTest->TestTypePrefixingL();
	pbapTest->TestFilterL();
	pbapTest->TestVCard21SupportL();
	CleanupStack::PopAndDestroy(pbapTest);	
	test.End();
	}
	
//
// Main.
//

//@SYMTestCaseID PIM-TCNTPBAPSUPPORT-0001
	
GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	test.Start(_L("@SYMTestCaseID PIM-TCNTPBAPSUPPORT-0001 CntPBAPSupport Test"));

	CActiveScheduler* scheduler = new CActiveScheduler;
	if (scheduler)
		{
		CActiveScheduler::Install(scheduler);
		CTrapCleanup* cleanup = CTrapCleanup::New();
		if (cleanup)
			{
			TRAPD(err,DoTaskL() );
			test(err == KErrNone);
			delete cleanup;
			}
		delete scheduler;
		}
	test.Close();
	__UHEAP_MARKEND;
	return KErrNone;
    }
