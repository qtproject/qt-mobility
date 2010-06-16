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


#include "testexportcontactstep.h"
#include "testimpexvcardsuiteserver.h"

TInt CTestExportContactStep::iStorage[]=
	{
	KStorageTypeText, KStorageTypeText, KStorageTypeDateTime, KStorageTypeText,KStorageTypeText,KStorageTypeText,KStorageTypeText, KStorageTypeText 		
	};

TUid CTestExportContactStep::iFieldUid[]=
	{
	KUidContactFieldAssistant, KUidContactFieldPhoneNumber, KUidContactFieldAnniversary, KUidContactFieldSpouse,KUidContactFieldChildren,KUidContactFieldClass, KUidContactFieldCompanyName, KUidContactFieldDepartmentName 		
	};

TUid CTestExportContactStep::iVcardUid[]=
	{
	KUidContactFieldVCardMapAssistant, KUidContactFieldVCardMapAssistantTel, KUidContactFieldVCardMapAnniversary, KUidContactFieldVCardMapSpouse,KUidContactFieldVCardMapChildren,KUidContactFieldVCardMapClass, KUidContactFieldVCardMapORG, KUidContactFieldVCardMapDepartment,		
	};

CTestExportContactStep::~CTestExportContactStep()
/**
 * Destructor
 */
	{
	}

CTestExportContactStep::CTestExportContactStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KTestExportContactStep);
	}

TVerdict CTestExportContactStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Start export of vCards"));
	iScheduler = new (ELeave) CActiveScheduler;
	
	CActiveScheduler::Install(iScheduler);
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTestExportContactStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
	TPtrC fieldName, fieldVal; 
	_LIT(KWithout, "without");
	_LIT(KAll,		"all");
	_LIT(KNew,		"new");
	
	GetStringFromConfig(ConfigSection(), KIniFlagvalue, fieldVal);
	if(KErrNone == fieldVal.Compare(KAll))	
		{
		TRAPD(ret,ExportAllFieldsL());
		if(ret!=KErrNone)
			{
			SetTestStepResult(EFail);
			}
		}
	else if(!GetStringFromConfig(ConfigSection(), KIniFieldName, fieldName))
		{
		ERR_PRINTF1(_L("Unable to read fieldname from ini file"));
		SetTestStepResult(EFail);
		}
	else
		{
		TBuf<50> path(KNullDesC);
		if(KErrNone == fieldName.Compare(KAssistance))
			{
			//Get the Flag value form Ini file and based on that assign the file path.
			GetStringFromConfig(ConfigSection(), KIniFlagvalue, fieldVal);
			if(KErrNone == fieldVal.Compare(KWithout))
				{
				path=KExportAsstWithoutDataFile;
				}
			else if(KErrNone == fieldVal.Compare(KNew))	
				{
				path=KExportAsstWithNewFile;
				}
			else
				{
				path = KExportAsstWithDataFile;	
				}	
			// Get the Field Value	
			GetStringFromConfig(ConfigSection(), KIniFieldVal, fieldVal);
			TRAPD(ret, ExportFieldsL(fieldVal, iStorage[0],iFieldUid[0], iVcardUid[0], path));
			if(ret!=KErrNone)
				{
				SetTestStepResult(EFail);
				}
			}
		else if(KErrNone == fieldName.Compare(KAssistanceTel))
			{
			//Get the Flag value form Ini file and based on that assign the file path.
			GetStringFromConfig(ConfigSection(), KIniFlagvalue, fieldVal);
			if(KErrNone == fieldVal.Compare(KWithout))
				{
				path=KExportAsstTelWithoutDataFile;
				}
			else if(KErrNone == fieldVal.Compare(KNew))	
				{
				path = KExportAsstTelWithNewFile;
				}
			else
				{
				path = KExportAsstTelWithDataFile;	
				}
			// Get the Field Value		
			GetStringFromConfig(ConfigSection(), KIniFieldVal, fieldVal);
			TRAPD(ret, ExportFieldsL(fieldVal, iStorage[1],iFieldUid[1], iVcardUid[1], path));
			if(ret!=KErrNone)
				{
				SetTestStepResult(EFail);
				}
			}
		else if(KErrNone == fieldName.Compare(KAnniversary))
			{
			//Get the Flag value form Ini file and based on that assign the file path.
			GetStringFromConfig(ConfigSection(), KIniFlagvalue, fieldVal);
			if(KErrNone == fieldVal.Compare(KWithout))
				{
				path=KExportAnniWithoutDataFile;
				}
			else if(KErrNone == fieldVal.Compare(KNew))	
				{
				path=KExportAnniWithNewFile;
				}
			else
				{
				path = KExportAnniWithDataFile;
				}
			GetStringFromConfig(ConfigSection(), KIniFieldVal, fieldVal);
			TRAPD(ret, ExportFieldsL(fieldVal, iStorage[2],iFieldUid[2], iVcardUid[2], path));
			if(ret!=KErrNone)
				{
				SetTestStepResult(EFail);
				}
			}
		else if(KErrNone == fieldName.Compare(KSpouse))
			{
			//Get the Flag value form Ini file and based on that assign the file path.
			GetStringFromConfig(ConfigSection(), KIniFlagvalue, fieldVal);
			if(KErrNone == fieldVal.Compare(KWithout))
				{
				path=KExportSpouseWithoutDataFile;
				}
			else if(KErrNone == fieldVal.Compare(KNew))	
				{
				path = KExportSpouseWithNewFile;
				}
			else	
				{
				path = KExportSpouseWithDataFile;
				}
			GetStringFromConfig(ConfigSection(), KIniFieldVal, fieldVal);
			TRAPD(ret, ExportFieldsL(fieldVal, iStorage[3],iFieldUid[3], iVcardUid[3], path));
			if(ret!=KErrNone)
				{
				SetTestStepResult(EFail);
				}
			}
		else if(KErrNone == fieldName.Compare(KChildren))
			{
			//Get the Flag value form Ini file and based on that assign the file path.
			GetStringFromConfig(ConfigSection(), KIniFlagvalue, fieldVal);
			if(KErrNone == fieldVal.Compare(KWithout))
				{
				path=KExportChildrenWithoutDataFile;
				}
			else if(KErrNone == fieldVal.Compare(KNew))	
				{
				path = KExportChildrenWithNewFile;
				}
			else
				{
				path = KExportChildrenWithDataFile;
				}
			// Get the Field Value	
			GetStringFromConfig(ConfigSection(), KIniFieldVal, fieldVal);
			TRAPD(ret, ExportFieldsL(fieldVal, iStorage[4],iFieldUid[4], iVcardUid[4], path));
			if(ret!=KErrNone)
				{
				SetTestStepResult(EFail);
				}
			}
		else if(KErrNone == fieldName.Compare(KClass))
			{
			//Get the Flag value form Ini file and based on that assign the file path.
			GetStringFromConfig(ConfigSection(), KIniFlagvalue, fieldVal);
			if(KErrNone == fieldVal.Compare(KWithout))
				{
				path=KExportClassWithoutDataFile;
				}
			else if(KErrNone == fieldVal.Compare(KNew))	
				{
				path=KExportClassWithNewFile;
				}
			else
				{
				path = KExportClassWithDataFile;
				}
			// Get the Field Value	
			GetStringFromConfig(ConfigSection(), KIniFieldVal, fieldVal);
			TRAPD( ret, ExportFieldsL(fieldVal, iStorage[5],iFieldUid[5], iVcardUid[5], path));
			if(ret!=KErrNone)
				{
				SetTestStepResult(EFail);
				}
			}
	
		else if(KErrNone == fieldName.Compare(KCompanyName))
			{
			//Get the Flag value form Ini file and based on that assign the file path.
			GetStringFromConfig(ConfigSection(), KIniFlagvalue, fieldVal);
			if(KErrNone == fieldVal.Compare(KWithout))
				{
				path=KExportCompanyWithoutDataFile;
				}
			else if(KErrNone == fieldVal.Compare(KNew))	
				{
				path = KExportCompanyWithNewFile;
				}
			else
				{
				path = KExportCompanyWithDataFile;
				}
			// Get the Field Value	
			GetStringFromConfig(ConfigSection(), KIniFieldVal, fieldVal);
			TRAPD(ret, ExportFieldsL(fieldVal, iStorage[6],iFieldUid[6], iVcardUid[6], path));
			if(ret!=KErrNone)
				{
				SetTestStepResult(EFail);
				}
			}
		else if(KErrNone == fieldName.Compare(KDepartment))
			{
			//Get the Flag value form Ini file and based on that assign the file path.
			GetStringFromConfig(ConfigSection(), KIniFlagvalue, fieldVal);
			if(KErrNone == fieldVal.Compare(KWithout))
				{
				path=KExportDeptWithoutDataFile;
				}
			else if(KErrNone == fieldVal.Compare(KNew))	
				{
				path = KExportDeptWithNewFile;
				}
			else
				{
				path = KExportDeptWithDataFile;
				}
			// Get the Field Value	
			GetStringFromConfig(ConfigSection(), KIniFieldVal, fieldVal);
			TRAPD(ret, ExportFieldsL(fieldVal, iStorage[7],iFieldUid[7], iVcardUid[7], path));
			if(ret!=KErrNone)
				{
				SetTestStepResult(EFail);
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

TVerdict CTestExportContactStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CActiveScheduler::Install(NULL);
	delete iScheduler;
	INFO_PRINTF1(_L("Completed export of vCards "));
	return TestStepResult();
	}
/** Create Contact Item and Add field to it.Add the Contact Item
 *	to Contact Database and Export the Contact Item into vCard format.
 *	@param aValue - Value for the field
 *  @param aStorageType - Storage type of each field
 *	@param aContFieldUid - Uid of Contact field
 *	@param avCardMapUid - Uid of vCard 
 *  @param aPath - Ptah for VCF file required to export the contact item.
 *	@return - void
*/
void CTestExportContactStep::ExportFieldsL(TPtrC aValue, TInt aStorageType,TUid aContFieldUid, TUid avCardMapUid, TBuf<80> aPath)
	{
	CContactDatabase *dBase = NULL;
   _LIT(KTestDbName, "c:exportcontact.cdb");
   	TInt year, month, day;	
   	TContactItemId itemId;
   	CContactIdArray* idArray = NULL;
   	RFs fsSession;
	RFileWriteStream fileStream;
	
	//replace existing database 
	dBase = CContactDatabase::ReplaceL(KTestDbName);
	CleanupStack::PushL(dBase);
	
	//connect to file system
	User::LeaveIfError(fsSession.Connect());
	CleanupClosePushL(fsSession);
   	idArray = CContactIdArray::NewLC();
   	
   	//Makes one or more directories.
   	fsSession.MkDirAll(aPath);
   	
	//Replaces a single file with another
	User::LeaveIfError(fileStream.Replace(fsSession, aPath, EFileWrite));
	
    // Create contact item and add field to it
    CContactItem* item = CContactCard::NewLC();
	CContactItemField* field = CContactItemField::NewL(aStorageType, aContFieldUid);
	CleanupStack::PushL(field); 
	field->SetMapping(avCardMapUid);
	if(aStorageType == KStorageTypeDateTime)
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
		TDateTime date(year,(TMonth)month,day,0,0,0,0);
		field->DateTimeStorage()->SetTime(date);
		}
	else
		{
		field->TextStorage()->SetTextL(aValue);	
		}	
		item->AddFieldL(*field);  
	CleanupStack::Pop(field);
			
	// add contact item to database  
	INFO_PRINTF1(_L("Adding contact to Database....."));
	itemId = dBase->AddNewContactL(*item);
	
	INFO_PRINTF1(_L("Contact Added to Database"));

	CleanupStack::PopAndDestroy(item);
	
	//insert CI ids
	idArray->InsertL(0, itemId);
	
	INFO_PRINTF1(_L("Exporting Contact....."));
	
	// Exporting contact item into vCard format
	dBase->ExportSelectedContactsL(TUid::Uid(KUidVCardConvDefaultImpl), *idArray, fileStream, 0);
	INFO_PRINTF1(_L("Exported Contact"));
	fileStream.CommitL();
	fileStream.Close();	
	
	// Close the contacts
    dBase->CloseContactL(itemId);
    
    //Cleanup
    CleanupStack::PopAndDestroy(idArray);
    CleanupStack::PopAndDestroy(&fsSession);
    fsSession.Close();
    CleanupStack::PopAndDestroy(dBase);	
	}
/**	Create Contact Item and Add All fields to it then Add the Contact Item
 *	to Contact Database and Export the Contact Item into vCard format.
 */

void CTestExportContactStep::ExportAllFieldsL()
	{
	CContactDatabase *dBase = NULL;
	TPtrC temp,flagVal;
   _LIT(KTestDbName, 	"c:exportcontactall.cdb");
   	TInt year, month,exportflag, day,readPos=0;	
   	TContactItemId itemId;
   	CContactIdArray* idArray = NULL;
   	RFs fsSession;
	RFileWriteStream fileStream;
	
	//replace existing database 
	dBase = CContactDatabase::ReplaceL(KTestDbName); 
	CleanupStack::PushL(dBase); 
	
	RArray<TPtrC> readValues;
	// Push into stack
	CleanupClosePushL(readValues);
	
	// specify the size of path
	TBuf<50> path(KNullDesC);
	
	// Get the value from ini file
	GetIntFromConfig(ConfigSection(), KFlagExport, exportflag);
	if(exportflag == 1)
		{
		path=KExportAllWithoutDataFile;
		}
	else if(exportflag == 2)
		{
		path = KExportAllFieldWithEmptyDeptFile;
		}
	else if(exportflag == 3)
		{
		path = KExportAllFieldWithEmptyCompanyFile;
		}
	else
		{
		path=KExportAllFieldFile;	
		}
	//connect to file system
	User::LeaveIfError(fsSession.Connect());
	
	// Push the session into stack
	CleanupClosePushL(fsSession);
	
	//Allocates and constructs a new contact ID array.
   	idArray = CContactIdArray::NewLC();
   	
   	//Makes one or more directories.
   	fsSession.MkDirAll(path);
   	
	//Replaces a single file with another
	User::LeaveIfError(fileStream.Replace(fsSession, path, EFileWrite));
	
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
			
    // Create contact item and add field to it
    CContactItem* item = CContactCard::NewLC();
    for(TInt pos=0; pos<8; pos++)
		{
		CContactItemField* field = CContactItemField::NewL(iStorage[pos], iFieldUid[pos]);
		CleanupStack::PushL(field); 
		field->SetMapping(iVcardUid[pos]);
		if(iStorage[pos]==KStorageTypeDateTime)
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
			if(!res )
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
			TDateTime date(year,(TMonth)month,day,0,0,0,0);
			field->DateTimeStorage()->SetTime(date);
			}
		else
			{
			field->TextStorage()->SetTextL(readValues[readPos++]);	
			}	
		item->AddFieldL(*field);  
		CleanupStack::Pop(field);
		}
			
	// add contact item to database  
	INFO_PRINTF1(_L("Adding contact to Database....."));
	itemId = dBase->AddNewContactL(*item);
	INFO_PRINTF1(_L("Contact Added to Database"));
	CleanupStack::PopAndDestroy(item);
	
	//insert CI ids
	idArray->InsertL(0, itemId);
	
	INFO_PRINTF1(_L("Exporting Contact....."));
	
	// Exporting contact item into vCard format
	dBase->ExportSelectedContactsL(TUid::Uid(KUidVCardConvDefaultImpl), *idArray, fileStream, EFalse);
	
	INFO_PRINTF1(_L("Exported Contact"));
	fileStream.CommitL();
	fileStream.Close();	
	
	// Close the Contact
    dBase->CloseContactL(itemId);
    
    //Cleanup
    CleanupStack::PopAndDestroy(idArray);
    CleanupStack::PopAndDestroy(&fsSession);
    fsSession.Close();
    CleanupStack::PopAndDestroy(&readValues);
    CleanupStack::PopAndDestroy(dBase);	
	}
