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


#include "testimportcontactmergemodestep.h"
#include "testimpexvcardsuiteserver.h"
CContactDatabase *dBase = NULL;

TInt CTestImportContactMergeModeStep::iStorage[]=
{
KStorageTypeText, KStorageTypeText, KStorageTypeDateTime, KStorageTypeText,KStorageTypeText,KStorageTypeText,KStorageTypeText, KStorageTypeText
};

TUid CTestImportContactMergeModeStep::iFieldUid[]=
{
KUidContactFieldAssistant, KUidContactFieldPhoneNumber, KUidContactFieldAnniversary, KUidContactFieldSpouse,KUidContactFieldChildren,KUidContactFieldClass, KUidContactFieldCompanyName, KUidContactFieldDepartmentName
};

TUid CTestImportContactMergeModeStep::iVcardUid[]=
{
KUidContactFieldVCardMapAssistant, KUidContactFieldVCardMapAssistantTel, KUidContactFieldVCardMapAnniversary, KUidContactFieldVCardMapSpouse,KUidContactFieldVCardMapChildren,KUidContactFieldVCardMapClass, KUidContactFieldVCardMapORG, KUidContactFieldVCardMapDepartment,
};

CTestImportContactMergeModeStep::~CTestImportContactMergeModeStep()
/**
 * Destructor
 */
	{
	}

CTestImportContactMergeModeStep::CTestImportContactMergeModeStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KTestImportContactMergeModeStep);
	}

TVerdict CTestImportContactMergeModeStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{

	INFO_PRINTF1(_L("Start import from vCardsm in Merge Mode"));

	iScheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(iScheduler);

	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTestImportContactMergeModeStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
	TPtrC fieldName, fieldVal;
	TInt merge;
	 if(!GetStringFromConfig(ConfigSection(), KIniFieldName, fieldName))
		{
		ERR_PRINTF1(_L("Unable to read fieldname from ini file"));
		SetTestStepResult(EFail);
		}
	 else
		{
		TBuf<50> existingFile(KNullDesC);
		TBuf<50> newFile;
        TBuf<2> drive;
        
#ifdef __WINS__
        drive.Append(_L("c:"));
#else
	TFileName processFileName = RProcess().FileName();
	TParsePtrC parse(processFileName);
	drive.Append(parse.Drive());
#endif
	
	
		if(KErrNone == fieldName.Compare(KAssistance))
			{
			GetIntFromConfig(ConfigSection(), KIniFlagvalue, merge);
			if(merge==1)
				{
				// field present with data
				existingFile = KExportAsstWithDataFile;
				// new file with field present used for vCard
				newFile = KExportAsstWithNewFile;	
				}
			 if(merge==2)
				{
				//field absent
				existingFile.Append(drive);
				existingFile.Append(KImportAsstMergeMode2File);
				// new file with field present used for vCard
				newFile = KExportAsstWithNewFile; 
				}
			if(merge==3)
				{
				// field present
				existingFile = KExportAsstWithDataFile;
				// new file with field present but does not have value, used for vCard
				newFile.Append(drive);
				newFile.Append(KImportAsstMergeMode3File);
				}
			ImportFieldsInMergeModeL(existingFile, newFile, iStorage[0],iFieldUid[0], iVcardUid[0]);
			}
		else if(KErrNone == fieldName.Compare(KAssistanceTel))
			{
			GetIntFromConfig(ConfigSection(), KIniFlagvalue, merge);
			if(merge==1)
				{
				// field present with data
				existingFile = KExportAsstTelWithDataFile;
				// new file with field present used for vCard
				newFile = KExportAsstTelWithNewFile;
				}
			if(merge==2)
				{
				//field absent
				existingFile.Append(drive);
				existingFile.Append(KImportAsstTelMergeMode2File); 
				// new file with field present used for vCard
				newFile.Append(KExportAsstTelWithNewFile);
				}
			if(merge==3)
				{
				// field present
				existingFile = KExportAsstTelWithDataFile;
				// new file with field present but does not have value, used for vCard
				newFile.Append(drive);
				newFile.Append(KImportAsstTelMergeMode3File);
				}
			ImportFieldsInMergeModeL(existingFile, newFile, iStorage[1],iFieldUid[1], iVcardUid[1]);
			}
		else if(KErrNone == fieldName.Compare(KAnniversary))
			{
			GetIntFromConfig(ConfigSection(), KIniFlagvalue, merge);
			if(merge==1)
				{
				// field present with data
				existingFile = KExportAnniWithDataFile;
				// new file with field present used for vCard
				newFile = KExportAnniWithNewFile;
				}
			 if(merge==2)
				{
				//field absent
				existingFile.Append(drive);
				existingFile.Append(KImportAnniMergeMode2File); 
				// new file with field present used for vCard
				newFile = KExportAnniWithNewFile;
				}
			if(merge==3)	
				{
				// field present
				existingFile = KExportAnniWithDataFile;
				// new file with field present but does not have value, used for vCard
				newFile.Append(drive);
				newFile.Append(KImportAnniMergeMode3File);
				}
			ImportFieldsInMergeModeL(existingFile, newFile, iStorage[2],iFieldUid[2], iVcardUid[2]);
			}
		else if(KErrNone == fieldName.Compare(KSpouse))
			{
			GetIntFromConfig(ConfigSection(), KIniFlagvalue, merge);
			if(merge==1)
				{
				// field present with data	
				existingFile = KExportSpouseWithDataFile;
				//new file with field present used for vCard
				newFile = KExportSpouseWithNewFile;
				}
			 if(merge==2)
				{
				//field absent
				existingFile.Append(drive);
				existingFile.Append(KImportSpouseMergeMode2File); 
				//new file with field present used for vCard
				newFile = KExportSpouseWithNewFile;
				}
			if(merge==3)	
				{
				// field present	
				existingFile = KExportSpouseWithDataFile;
				//new file with field present but does not have value, used for vCard
				newFile.Append(drive);
				newFile.Append(KImportSpouseMergeMode3File);
				}
			ImportFieldsInMergeModeL(existingFile, newFile, iStorage[3],iFieldUid[3], iVcardUid[3]);
			}
		else if(KErrNone == fieldName.Compare(KChildren))
			{
			GetIntFromConfig(ConfigSection(), KIniFlagvalue, merge);
			if(merge==1)
				{
				// field present with data
				existingFile = KExportChildrenWithDataFile;
				//new file with field present used for vCard
				newFile = KExportChildrenWithNewFile; 
				}
			if(merge==2)
				{
				//field absent
				existingFile.Append(drive);
				existingFile.Append(KImportChildrenMergeMode2File);
				//new file with field present used for vCard
				newFile = KExportChildrenWithNewFile; 
				}
			if(merge==3)	
				{
				// field present
				existingFile = KExportChildrenWithDataFile;
				//new file with field present but does not have value, used for vCard
				newFile.Append(drive);
				newFile.Append(KImportChildrenMergeMode3File); 
				}
			ImportFieldsInMergeModeL(existingFile, newFile,  iStorage[4],iFieldUid[4], iVcardUid[4]);
			}
		else if(KErrNone == fieldName.Compare(KClass))
			{
			GetIntFromConfig(ConfigSection(), KIniFlagvalue, merge);
			if(merge==1)
				{
				// field present with data
				existingFile = KExportClassWithDataFile;
				//new file with field present used for vCard	
				newFile = KExportClassWithNewFile;
				}
			if(merge==2)
				{
				//field absent
				existingFile.Append(drive);
				existingFile.Append(KImportClassMergeMode2File); 
				//new file with field present used for vCard	
				newFile = KExportClassWithNewFile;
				}
			if(merge==3)	
				{
				// field present with data
				existingFile = KExportClassWithDataFile;
				//new file with field present but does not have value, used for vCard
				newFile.Append(drive);
				newFile.Append(KImportClassMergeMode3File);
				}
			ImportFieldsInMergeModeL(existingFile, newFile, iStorage[5],iFieldUid[5], iVcardUid[5]);
			}

		else if(KErrNone == fieldName.Compare(KCompanyName))
			{
			GetIntFromConfig(ConfigSection(), KIniFlagvalue, merge);
			if(merge==1)
				{
				// field present
				existingFile = KExportCompanyWithDataFile;
				//new file with field present used for vCard		
				newFile = KExportCompanyWithNewFile;
				}
			if(merge==2)
				{
				//field absent
				existingFile.Append(drive);
				existingFile.Append(KImportCompanyMergeMode2File); 
				//new file with field present used for vCard		
				newFile = KExportCompanyWithNewFile;
				}
			if(merge==3)
				{
				// field present
				existingFile = KExportCompanyWithDataFile;
				//new file with field present used for vCard
				newFile.Append(drive);
				newFile.Append(KImportCompanyMergeMode3File);
				}
			ImportFieldsInMergeModeL(existingFile, newFile, iStorage[6],iFieldUid[6], iVcardUid[6]);
			}
		else if(KErrNone == fieldName.Compare(KDepartment))
			{
			GetIntFromConfig(ConfigSection(), KIniFlagvalue, merge);
			if(merge==1)
				{
				// field present
				existingFile = KExportDeptWithDataFile;
				//new file with field present used for vCard	
				newFile = KExportDeptWithNewFile;
				}
			if(merge==2)
				{
				//field absent
				existingFile.Append(drive);
				existingFile.Append(KImportDeptMergeMode2File); 
				//new file with field present used for vCard	
				newFile = KExportDeptWithNewFile;
				}	
			if(merge==3)	
				{
				// field present
				existingFile = KExportDeptWithDataFile;
				//new file with field present used for vCard
				newFile.Append(drive);
				newFile.Append(KImportDeptMergeMode3File);
				}
			ImportFieldsInMergeModeL(existingFile, newFile, iStorage[7],iFieldUid[7], iVcardUid[7]);
			}
		else
			{
			ERR_PRINTF1(_L("Invalid fieldname"));
			SetTestStepResult(EFail);
			}
		}
	return TestStepResult();
	}
	
void  CTestImportContactMergeModeStep::ExternaliseFileL(CVersitParser& aParser,const TDesC& aFileName, RFs& fsSession)
     {
     RFile file;
     User::LeaveIfError(file.Replace(fsSession, aFileName, EFileWrite));
     CleanupClosePushL(file);
     aParser.ExternalizeL(file);
     CleanupStack::PopAndDestroy(&file);
     }


void  CTestImportContactMergeModeStep::InternalizeFileL(CVersitParser& aParser,const TDesC& aFileName, RFs& fsSession)
     {
     RFile file;
     User::LeaveIfError(file.Open(fsSession, aFileName, EFileRead));
     CleanupClosePushL(file);
     TInt start = 0;
     aParser.InternalizeL(file,start);
     CleanupStack::PopAndDestroy(&file);
     }
	

CParserPropertyValueDateTime* CTestImportContactMergeModeStep::CreateDateTimePropertyL(const TTime& aDateTime, TVersitDateTime::TRelativeTime aRelativeTime)
     {
     TVersitDateTime* dateTime= new(ELeave)TVersitDateTime(aDateTime.DateTime(), aRelativeTime);
     CleanupStack::PushL(dateTime);
     CParserPropertyValueDateTime* dateTimeValue=new(ELeave) CParserPropertyValueDateTime(dateTime);
     CleanupStack::Pop(dateTime);
     return dateTimeValue;
     }

/** Add UID and REV properties to the input vCard files
 *  This helps in importing the vCards in Merge Mode
 *	@param aContactItem - existing contact item
 *	@param fsSession - File Session
 *  @param aSource - new file
 *	@param aDestination - File contains Uid of existing Contact item and field value from new file 
 *	@return - void
*/
void CTestImportContactMergeModeStep::AddUidRevPropertyL(const CContactItem& aContactItem, RFs& fsSession, const TDesC& aSource, const TDesC& aDestination)
    {
     CParserVCard* vCardParser=CParserVCard::NewL();
     CleanupStack::PushL(vCardParser);
     InternalizeFileL(*vCardParser,aSource,fsSession);

     CParserPropertyValueDateTime* valueDateTime = CreateDateTimePropertyL(aContactItem.LastModified(),TVersitDateTime::EIsMachineLocal);
     CleanupStack::PushL(valueDateTime);
     CParserProperty* property=CParserGroupedProperty::NewL(*valueDateTime,KVersitTokenREV,NULL,NULL);
     CleanupStack::Pop(valueDateTime);
     CleanupStack::PushL(property);
     vCardParser->AddPropertyL(property); //takes ownership
     CleanupStack::Pop(property);

     TInt64  machineUniqueId = dBase->MachineId();
     TPtrC guid=aContactItem.UidStringL(machineUniqueId);
     CParserPropertyValueHBufC* uidValue=CParserPropertyValueHBufC::NewL(guid);
     CleanupStack::PushL(uidValue);
     CParserProperty* uidproperty=CParserGroupedProperty::NewL(*uidValue,KVersitTokenUID,NULL,NULL);
     CleanupStack::Pop(uidValue);
     CleanupStack::PushL(uidproperty);
     vCardParser->AddPropertyL(uidproperty); //takes ownership
     CleanupStack::Pop(uidproperty);

     ExternaliseFileL(*vCardParser,aDestination, fsSession);
     CleanupStack::PopAndDestroy(vCardParser);
     }


TVerdict CTestImportContactMergeModeStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CActiveScheduler::Install(NULL);
	delete iScheduler;
	INFO_PRINTF1(_L("Completed import from vCards in Merge Mode "));
	return TestStepResult();
	}

/** Import the fields from vCard format into contact item in 3 scenarios.
 *  1) Field present in both vCard and the Contact Item
 *	2) Field present in vCard but absent in  the Contact Item
 *	3) Field present in vCard but does not contain any value
 *	@param aExistingFile - existing file for existing contact item
 *	@param aNewFile - New file for vCard format
 *  @param aStorageType - Storage type of each field
 *	@param aContFieldUid - Uid of Contact field
 *	@param avCardMapUid - Uid of vCard 
 *	@return - void
*/
void CTestImportContactMergeModeStep::ImportFieldsInMergeModeL(TBuf<50> aExistingFile, TBuf<50> aNewFile, TInt aStorageType,TUid aContFieldUid, TUid avCardMapUid)
{
	
	TPtrC compareVal, value;
   _LIT(KTestDbName, "c:importcontact.cdb");
   	TInt year, month, day;
   	RFs fsSession;
	RFileReadStream readStream, newReadStream;
	TBool success=EFalse;
	_LIT(KImportNewFileWithUid,			"C:\\ImportNewFileWithUid.vcf");
	
	//replace existing database
	dBase = CContactDatabase::ReplaceL(KTestDbName);

	//connect to file system
	User::LeaveIfError(fsSession.Connect());
	CleanupClosePushL(fsSession);

	//Opens a existing file in read mode
	User::LeaveIfError(readStream.Open(fsSession,aExistingFile, EFileRead));
    CleanupClosePushL(readStream);
	INFO_PRINTF1(_L("Importing Contact in Merge Mode....."));
	
	// importing contact item from vCard format
	CArrayPtr<CContactItem>* item = dBase->ImportContactsL(TUid::Uid(KUidVCardConvDefaultImpl), readStream, success, CContactDatabase::EImportSingleContact|CContactDatabase::ETTFormat);
	if(!success)
		{
		INFO_PRINTF1(_L("Contact Imported Failed"));
		SetTestStepResult(EFail);
		return;
		}
	INFO_PRINTF1(_L("Contact Imported in Merge Mode"));

	// Get the Contact Id
    TContactItemId contactId = (*item)[0]->Id();
    
    // Reads a contact item
    CContactItem* importedContact = dBase->ReadContactLC(contactId);

	AddUidRevPropertyL(*importedContact, fsSession, aNewFile, KImportNewFileWithUid);
	
	item->ResetAndDestroy();
	delete item;
	//Opens a file contains Uid of existing CI and field value from 
	// newly created contact item in read mode
   	User::LeaveIfError(newReadStream.Open(fsSession,KImportNewFileWithUid, EFileRead));
    CleanupClosePushL(newReadStream);
    
    // import the contact from vCard format.
    item = NULL;
	item = dBase->ImportContactsL(TUid::Uid(KUidVCardConvDefaultImpl), newReadStream, success, CContactDatabase::EImportSingleContact|CContactDatabase::ETTFormat);
	if(!success)
		{
		INFO_PRINTF1(_L("Contact Imported Failed"));
		SetTestStepResult(EFail);
		return;
		}
	INFO_PRINTF1(_L("Contact Imported in Merge Mode"));
	
	// Extract the merge mode value from ImportExportTestCases.ini.
	TInt flagValue;
	GetIntFromConfig(ConfigSection(), KIniFlagvalue, flagValue);

	//
	// Specifically for Mode 3 tests. Checks if the contact item has
	// been deleted. Passes the test if the contact item has been deleted.
	// Fails otherwise.
	//
	if( flagValue == 3 )
		{
		// Checks if any contact item has been returned.
		if( (*item).Count() == 0 )
			{
			SetTestStepResult(EPass);
			INFO_PRINTF1(_L("Contact Item Deleted"));
			}
		else
			{
			// Assuming that in mode 3, the item always has just 1 field
			// hence on merge, the item should always be deleted, and 
			// if it is not deleted the test will fail.
			SetTestStepResult(EFail);
			INFO_PRINTF1(_L("Contact Item Not Deleted"));
			}
		// Cleanup.
		item->ResetAndDestroy();
		delete item;	
		CleanupStack::PopAndDestroy(4,&fsSession);
		delete dBase;
		return;
		}

    // verify the field is being added	
	CContactItemFieldSet& ContactFieldSet = (*item)[0]->CardFields();
	//get the string value
	GetStringFromConfig(ConfigSection(), KExpectedData, compareVal);
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
			//compare the results
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
			if(compareVal.Compare(value)==0 )
				{
				SetTestStepResult(EPass);
				}
			else
				{
				SetTestStepResult(EFail);
				}
			}
		}
	else
		{
		TInt deleted;
		// get the flag value from ini file
		GetIntFromConfig(ConfigSection(), KIniFlagvalue, deleted);
		if(deleted == 3)
			{
			SetTestStepResult(EPass);
			INFO_PRINTF1(_L("Field deleted"));
			}
		else
			{
			INFO_PRINTF1(_L("Field not found"));
			}	
		
		}
	// Cleanup	
	item->ResetAndDestroy();
	delete item;	
    CleanupStack::PopAndDestroy(4,&fsSession);
    delete dBase;
}




