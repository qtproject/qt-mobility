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


#include "testimportcontactinsertmodestep.h"
#include "testimpexvcardsuiteserver.h"


TInt CTestImportContactInsertModeStep::iStorage[]=
{
KStorageTypeText, KStorageTypeText, KStorageTypeDateTime, KStorageTypeText,KStorageTypeText,KStorageTypeText,KStorageTypeText, KStorageTypeText
};

TUid CTestImportContactInsertModeStep::iFieldUid[]=
{
KUidContactFieldAssistant, KUidContactFieldPhoneNumber, KUidContactFieldAnniversary, KUidContactFieldSpouse,KUidContactFieldChildren,KUidContactFieldClass, KUidContactFieldCompanyName, KUidContactFieldDepartmentName
};

TUid CTestImportContactInsertModeStep::iVcardUid[]=
{
KUidContactFieldVCardMapAssistant, KUidContactFieldVCardMapAssistantTel, KUidContactFieldVCardMapAnniversary, KUidContactFieldVCardMapSpouse,KUidContactFieldVCardMapChildren,KUidContactFieldVCardMapClass, KUidContactFieldVCardMapORG, KUidContactFieldVCardMapDepartment,
};

CTestImportContactInsertModeStep::~CTestImportContactInsertModeStep()
/**
 * Destructor
 */
	{
	}

CTestImportContactInsertModeStep::CTestImportContactInsertModeStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KTestImportContactInsertModeStep);
	}

TVerdict CTestImportContactInsertModeStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{

	INFO_PRINTF1(_L("Start import from vCards in Insert Mode"));

	iScheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(iScheduler);

	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTestImportContactInsertModeStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
	TPtrC fieldName, fieldVal;
	_LIT(KAll,		"all");

	GetStringFromConfig(ConfigSection(), KIniFlagvalue, fieldVal);
	if(KErrNone == fieldVal.Compare(KAll))
		{
		ImportAllFieldsL();
		}
	else if(!GetStringFromConfig(ConfigSection(), KIniFieldName, fieldName))
		{
		ERR_PRINTF1(_L("Unable to read fieldname from ini file"));
		SetTestStepResult(EFail);
		}
	else
		{
		/**
		  *	Based on the field name, pass the path, fieldUid, vcardUid 
		  */
		TFileName fileNameExport;
		TFileName fileNameImport;
		#ifdef __WINS__
		    fileNameImport.Append(_L("c:"));
		#else
			TFileName processFileName = RProcess().FileName();
			TParsePtrC parse(processFileName);
			fileNameImport.Append(parse.Drive());
		#endif
		TInt corrupt = 0;
		if(KErrNone == fieldName.Compare(KAssistance))
			{
			GetIntFromConfig(ConfigSection(), KIniFlagvalue, corrupt);
			if(corrupt == 1)
				{
				fileNameImport.Append(KImportAsstCorruptData);
				ImportFieldsL(fileNameImport, iStorage[0],iFieldUid[0], iVcardUid[0]);
				}
			else
				{
				fileNameExport.Append(KExportAsstWithDataFile);
				ImportFieldsL(fileNameExport, iStorage[0],iFieldUid[0], iVcardUid[0]);
				}
			}
		else if(KErrNone == fieldName.Compare(KAssistanceTel))
			{
			GetIntFromConfig(ConfigSection(), KIniFlagvalue, corrupt);
			if(corrupt == 1)
				{
				fileNameImport.Append(KImportAsstTelCorruptData);
				ImportFieldsL(fileNameImport, iStorage[1],iFieldUid[1], iVcardUid[1]);
				}
			else
				{
				fileNameExport.Append(KExportAsstTelWithDataFile);
				ImportFieldsL(fileNameExport, iStorage[1],iFieldUid[1], iVcardUid[1]);
				}
			}
		else if(KErrNone == fieldName.Compare(KAnniversary))
			{
			GetIntFromConfig(ConfigSection(), KIniFlagvalue, corrupt);
			if(corrupt == 1)
				{
				fileNameImport.Append(KImportAnniCorruptData);
				ImportFieldsL(fileNameImport, iStorage[2],iFieldUid[2], iVcardUid[2]);
				}
				else
				{
				fileNameExport.Append(KExportAnniWithDataFile);
				ImportFieldsL(fileNameExport, iStorage[2],iFieldUid[2], iVcardUid[2]);
				}
			
			}
		else if(KErrNone == fieldName.Compare(KSpouse))
			{
			GetIntFromConfig(ConfigSection(), KIniFlagvalue, corrupt);
			if(corrupt == 1)
				{
				fileNameImport.Append(KImportSpouseCorruptData);
				ImportFieldsL(fileNameImport, iStorage[3],iFieldUid[3], iVcardUid[3]);
				}
			else
				{
				fileNameExport.Append(KExportSpouseWithDataFile);
				ImportFieldsL(fileNameExport, iStorage[3],iFieldUid[3], iVcardUid[3]);
				}
			}
		else if(KErrNone == fieldName.Compare(KChildren))
			{
			GetIntFromConfig(ConfigSection(), KIniFlagvalue, corrupt);
			if(corrupt == 1)
				{
				fileNameImport.Append(KImportChildrenCorruptData);
				ImportFieldsL(fileNameImport, iStorage[4],iFieldUid[4], iVcardUid[4]);
				}
			else
				{
				fileNameExport.Append(KExportChildrenWithDataFile);
				ImportFieldsL(fileNameExport, iStorage[4],iFieldUid[4], iVcardUid[4]);
				}
			}
		else if(KErrNone == fieldName.Compare(KClass))
			{
			GetIntFromConfig(ConfigSection(), KIniFlagvalue, corrupt);
			if(corrupt == 1)
				{
				fileNameImport.Append(KImportClassCorruptData);
				ImportFieldsL(fileNameImport, iStorage[5],iFieldUid[5], iVcardUid[5]);
				}
			else
				{
				fileNameExport.Append(KExportClassWithDataFile);
				ImportFieldsL(fileNameExport, iStorage[5],iFieldUid[5], iVcardUid[5]);
				}
			}
		else if(KErrNone == fieldName.Compare(KCompanyName))
			{
			GetIntFromConfig(ConfigSection(), KIniFlagvalue, corrupt);
			if(corrupt == 1)
				{
				fileNameImport.Append(KImportCompanyCorruptData);
				ImportFieldsL(fileNameImport, iStorage[6],iFieldUid[6], iVcardUid[6]);
				}
			else
				{
				fileNameExport.Append(KExportCompanyWithDataFile);
				ImportFieldsL(fileNameExport, iStorage[6],iFieldUid[6], iVcardUid[6]);
				}
			}
		else if(KErrNone == fieldName.Compare(KDepartment))
			{
			GetIntFromConfig(ConfigSection(), KIniFlagvalue, corrupt);
			if(corrupt == 1)
				{
				fileNameImport.Append(KImportDeptCorruptData);
				ImportFieldsL(fileNameImport, iStorage[7],iFieldUid[7], iVcardUid[7]);
				}
			else
				{
				fileNameExport.Append(KExportDeptWithDataFile);
				ImportFieldsL(fileNameExport, iStorage[7],iFieldUid[7], iVcardUid[7]);
				}	
			}
		else
			{
			ERR_PRINTF1(_L("Invalid fieldname"));
			SetTestStepResult(EFail);
			}
		}
	return TestStepResult();
	}

TVerdict CTestImportContactInsertModeStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CActiveScheduler::Install(NULL);
	delete iScheduler;
	INFO_PRINTF1(_L("Completed import from vCards in Insert Mode"));
	return TestStepResult();
	}
/** Import the field in the vCard format into Contact Item and check for
 * 	field in the vCard format is being imported inot contact item  
 *	@param aPath - Ptah for existing VCF file required to import into contact item.
 *  @param aStorageType - Storage type of each field
 *	@param aContFieldUid - Uid of Contact field
 *	@param avCardMapUid - Uid of vCard 
 *	@return - void
*/
void CTestImportContactInsertModeStep::ImportFieldsL(TBuf<50> aPath, TInt aStorageType,TUid aContFieldUid, TUid avCardMapUid)
	{
	CContactDatabase *dBase = NULL;
	TPtrC compareVal, value;
	TInt year, month, day;
   _LIT(KTestDbName, "c:importcontact.cdb");
   	RFs fsSession;
	RFileReadStream readStream;
	TBool success=EFalse;
	
	//replace existing database
	dBase = CContactDatabase::ReplaceL(KTestDbName);
	CleanupStack::PushL(dBase);
	
	//connect to file system
	User::LeaveIfError(fsSession.Connect());
	CleanupClosePushL(fsSession);
   	
	//Opens a file containing a stream and prepares the stream for reading
	User::LeaveIfError(readStream.Open(fsSession,aPath, EFileRead));
    CleanupClosePushL(readStream);
	INFO_PRINTF1(_L("Importing Contact in Insert Mode....."));

	// importing vCard format into contact item
	CArrayPtr<CContactItem>* item = dBase->ImportContactsL(TUid::Uid(KUidVCardConvDefaultImpl), readStream, success, CContactDatabase::EImportSingleContact|CContactDatabase::ETTFormat);
	if(!success)
		{
		INFO_PRINTF1(_L("Contact Imported Failed"));
		SetTestStepResult(EFail);
		return;
		}
	INFO_PRINTF1(_L("Contact Imported in Insert Mode"));
		
	CContactItemFieldSet& ContactFieldSet = (*item)[0]->CardFields();
	
	//get the string value
	GetStringFromConfig(ConfigSection(), KExpectedData, compareVal);
	
	//  verify the field is being imported
	TInt pos = ContactFieldSet.Find(aContFieldUid, avCardMapUid);	
	if(pos != KErrNotFound)	
		{
		if(aStorageType==KStorageTypeDateTime)
			{
			//Get the int value
			TBool res;
			res = GetIntFromConfig(ConfigSection(), KIniYear, year);
			if(!res)
				{
				ERR_PRINTF1(_L("Unable to read year val from ini file"));
				SetTestStepResult(EFail);
				}
			res = GetIntFromConfig(ConfigSection(), KIniMonth, month);
			if(!res)
				{
				ERR_PRINTF1(_L("Unable to read month val from ini file"));
				SetTestStepResult(EFail);
				}
			res = GetIntFromConfig(ConfigSection(), KIniDay, day);
			if(!res)
				{
				ERR_PRINTF1(_L("Unable to read day val from ini file"));
				SetTestStepResult(EFail);
				}	
			TDateTime result=ContactFieldSet[pos].DateTimeStorage()->Time().DateTime();
			//check the result
			if((result.Year() == year) && (result.Month() == (TMonth)month) && (result.Day() == day)) 
				{
				SetTestStepResult(EPass);	
				}
			else
				{
				SetTestStepResult(EFail);	
				}
			}
		else
			{	
			value.Set(ContactFieldSet[pos].TextStorage()->Text());
			//Check the results
			if(compareVal.Compare(value) == 0)
				{
				SetTestStepResult(EPass);
				}
			else
				{
				SetTestStepResult(EFail);
				}
			}	
		}
	readStream.Close();
	
	// Cleanup
	item->ResetAndDestroy();
	delete item;
	CleanupStack::PopAndDestroy(&readStream);
	fsSession.Close();
    CleanupStack::PopAndDestroy(&fsSession);
    CleanupStack::PopAndDestroy(dBase);
	}
/**	 Import all fields present in the vCard format into
  *	 the Contact Item.check for all fields are being imported. 
 */
void CTestImportContactInsertModeStep::ImportAllFieldsL()
	{
	CContactDatabase *dBase = NULL;
	TPtrC compareVal, value, temp;
   _LIT(KTestDbName, "c:importcontact.cdb");
   	TInt year, month, day, readPos=0,importflag;
   	RFs fsSession;
	RFileReadStream readStream;
	TBool success=EFalse;
	TBuf<50> path(KNullDesC);
	
	//replace existing database
	dBase = CContactDatabase::ReplaceL(KTestDbName);
	CleanupStack::PushL(dBase);
	
	RArray<TPtrC> readValues;
	CleanupClosePushL(readValues);
	
	// Get the value from ini file
	GetIntFromConfig(ConfigSection(), KFlagExport, importflag);
	if(importflag==2)
		{
		path = KExportAllFieldWithEmptyDeptFile;
		}
	else if(importflag==3)
		{
		path = KExportAllFieldWithEmptyCompanyFile;
		}
	else
		{
		path=KExportAllFieldFile;	
		}
	//connect to file system
	User::LeaveIfError(fsSession.Connect());
	CleanupClosePushL(fsSession);
   	
	//Opens a file containing a stream and prepares the stream for reading
	User::LeaveIfError(readStream.Open(fsSession,path, EFileRead));
    CleanupClosePushL(readStream);
	INFO_PRINTF1(_L("Importing all Contact....."));

	// importing fields in vCard format into Contact Item.
	CArrayPtr<CContactItem>* item = dBase->ImportContactsL(TUid::Uid(KUidVCardConvDefaultImpl), readStream, success, CContactDatabase::EImportSingleContact|CContactDatabase::ETTFormat);
	if(!success)
		{
		INFO_PRINTF1(_L("Contact Imported Failed"));
		SetTestStepResult(EFail);
		return;
		}
	INFO_PRINTF1(_L("All Contact Imported"));
  
	// get string value from ini file.
	if(!GetStringFromConfig(ConfigSection(), KIniFieldAsstVal, temp))
		{
		ERR_PRINTF1(_L("Unable to read fieldval from ini file"));
		SetTestStepResult(EFail);
		}
	else
		{
		readValues.Append(temp);	
		}	
	if(!GetStringFromConfig(ConfigSection(), KIniFieldAsstTelVal, temp))
		{
		ERR_PRINTF1(_L("Unable to read fieldval from ini file"));
		SetTestStepResult(EFail);
		}
	else
		{
		readValues.Append(temp);
		}	
	if(!GetStringFromConfig(ConfigSection(), KIniFieldSpouseVal, temp))
		{
		ERR_PRINTF1(_L("Unable to read fieldval from ini file"));
		SetTestStepResult(EFail);
		}
	else	
		{
		readValues.Append(temp);	
		}	
	if(!GetStringFromConfig(ConfigSection(), KIniFieldChildVel, temp))
		{
		ERR_PRINTF1(_L("Unable to read fieldval from ini file"));
		SetTestStepResult(EFail);
		}
	else
		{
		readValues.Append(temp);	
		}
	if(!GetStringFromConfig(ConfigSection(), KIniFieldClassVal, temp))
		{
		ERR_PRINTF1(_L("Unable to read fieldval from ini file"));
		SetTestStepResult(EFail);
		}
	else
		{
		readValues.Append(temp);	
		}
	if(!GetStringFromConfig(ConfigSection(), KIniFieldCompanyVal, temp))
		{
		ERR_PRINTF1(_L("Unable to read fieldval from ini file"));
		SetTestStepResult(EFail);
		}
	else
		{
		readValues.Append(temp);	
		}
	if(!GetStringFromConfig(ConfigSection(), KIniFieldDeptVal, temp))
		{
		ERR_PRINTF1(_L("Unable to read fieldval from ini file"));
		SetTestStepResult(EFail);
		}
	else
		{
		readValues.Append(temp);	
		}
	
	// verify the field is being imported from vCard 	
	CContactItemFieldSet& ContactFieldSet = (*item)[0]->CardFields();
	for(TInt read=0;read<8;read++)
		{
		TInt pos = ContactFieldSet.Find(iFieldUid[read], iVcardUid[read]);	
		if(pos != KErrNotFound)	
			{
			if(iStorage[read]==KStorageTypeDateTime)
				{
				//Get the int value
				TBool res;
				res = GetIntFromConfig(ConfigSection(), KIniYear, year);
				if(!res)
					{
					ERR_PRINTF1(_L("Unable to read year val from ini file"));
					SetTestStepResult(EFail);
					}
				res = GetIntFromConfig(ConfigSection(), KIniMonth, month);
				if(!res)
					{
					ERR_PRINTF1(_L("Unable to read month val from ini file"));
					SetTestStepResult(EFail);
					}
				res = GetIntFromConfig(ConfigSection(), KIniDay, day);
				if(!res)
					{
					ERR_PRINTF1(_L("Unable to read day val from ini file"));
					SetTestStepResult(EFail);
					}
				TDateTime result=ContactFieldSet[pos].DateTimeStorage()->Time().DateTime();
				//Compare the Results
				if((result.Year() == year) && (result.Month() == (TMonth)month) && (result.Day() == day)) 
					{
					SetTestStepResult(EPass);	
					}
				else
					{
					SetTestStepResult(EFail);	
					}
				}
			else
				{	
				value.Set(ContactFieldSet[pos].TextStorage()->Text());
				//compare the results
				if(readValues[readPos++].Compare(value)==0 )
					{
					SetTestStepResult(EPass);
					}
				else
					{
					SetTestStepResult(EFail);
					}
				}
			}
		}
    
	readStream.Close();
	
	//Cleanup
	item->ResetAndDestroy();
	delete item;
	CleanupStack::PopAndDestroy(&readStream);
	fsSession.Close();
    CleanupStack::PopAndDestroy(&fsSession);
    CleanupStack::PopAndDestroy(&readValues);
    CleanupStack::PopAndDestroy(dBase);
	}


