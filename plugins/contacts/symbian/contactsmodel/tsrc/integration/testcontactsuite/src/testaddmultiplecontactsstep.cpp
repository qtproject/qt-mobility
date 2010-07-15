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


#include "testaddmultiplecontactsstep.h"
#include "testcontactsuiteserver.h"
#include <cntitem.h>
#include <cntfield.h>
#include <cntfldst.h>
#include <cntdef.h>
#include <cntdb.h>
#include <commdb.h>


TInt CTestAddMultipleContactsStep::iStorage[]=
{
KStorageTypeText, KStorageTypeText, KStorageTypeDateTime, KStorageTypeText,KStorageTypeText,KStorageTypeText,KStorageTypeText
};

TUid CTestAddMultipleContactsStep::iFieldUid[]=
{
KUidContactFieldAssistant, KUidContactFieldPhoneNumber, KUidContactFieldAnniversary,KUidContactFieldSpouse,KUidContactFieldChildren,KUidContactFieldClass,KUidContactFieldDepartmentName
};

TUid CTestAddMultipleContactsStep::iVcardUid[]=
{
KUidContactFieldVCardMapAssistant, KUidContactFieldVCardMapAssistantTel, KUidContactFieldVCardMapAnniversary, KUidContactFieldVCardMapSpouse,KUidContactFieldVCardMapChildren,KUidContactFieldVCardMapClass,KUidContactFieldVCardMapDepartment
};

CContactDatabase *database = NULL;
TContactItemId id = 0;

CTestAddMultipleContactsStep::~CTestAddMultipleContactsStep()
/**
 * Destructor
 */
	{

	}

CTestAddMultipleContactsStep::CTestAddMultipleContactsStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KTestAddMultipleContactsStep);
	}

TVerdict CTestAddMultipleContactsStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	iScheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(iScheduler);
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTestAddMultipleContactsStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
	TPtrC fieldName,fieldVal, fieldMultiple;
	_LIT(KMultiple,	"multiple");
 	if (!GetStringFromConfig(ConfigSection(), KIniFieldName, fieldName))
		{
		ERR_PRINTF1(_L("Unable to read fieldname from ini file"));
		SetTestStepResult(EFail);
		}
	else
		{
		if(KErrNone == fieldName.Compare(KAssistance))
			{
			// Get the values from ini file
			GetStringFromConfig(ConfigSection(), KIniFieldVal, fieldVal);
			GetStringFromConfig(ConfigSection(), KIniFlagvalue, fieldMultiple);
			if(fieldMultiple.Compare(KMultiple)==0)
				{
				AddMultipleContactsL(fieldVal, iStorage[0],iFieldUid[0], iVcardUid[0]);
				}
			else
				{		
				OpenExistingContactL(fieldVal, iStorage[0],iFieldUid[0], iVcardUid[0]);
				}
			}
		else if(KErrNone == fieldName.Compare(KAssistanceTel))
			{
			// Get the values from ini file
			GetStringFromConfig(ConfigSection(), KIniFieldVal, fieldVal);
			GetStringFromConfig(ConfigSection(), KIniFlagvalue, fieldMultiple);
			if(fieldMultiple.Compare(KMultiple)==0)
				{
				AddMultipleContactsL(fieldVal, iStorage[1],iFieldUid[1], iVcardUid[1]);
				}
			else
				{
				OpenExistingContactL(fieldVal, iStorage[1],iFieldUid[1], iVcardUid[1]);
				}
			}
		else if(KErrNone == fieldName.Compare(KAnniversary))
			{
			// Get the values from ini file
			GetStringFromConfig(ConfigSection(), KIniFieldVal, fieldVal);
			GetStringFromConfig(ConfigSection(), KIniFlagvalue, fieldMultiple);
			if(fieldMultiple.Compare(KMultiple)==0)
				{
				AddMultipleContactsL(fieldVal, iStorage[2],iFieldUid[2], iVcardUid[2]);
				}
			else
				{
				OpenExistingContactL(fieldVal, iStorage[2],iFieldUid[2], iVcardUid[2]);
				}
			}
		else if(KErrNone == fieldName.Compare(KSpouse))
			{
			// Get the values from ini file
			GetStringFromConfig(ConfigSection(), KIniFieldVal, fieldVal);
			GetStringFromConfig(ConfigSection(), KIniFlagvalue, fieldMultiple);
			if(fieldMultiple.Compare(KMultiple)==0)
				{
				AddMultipleContactsL(fieldVal, iStorage[3],iFieldUid[3], iVcardUid[3]);
				}
			else
				{
				OpenExistingContactL(fieldVal, iStorage[3],iFieldUid[3], iVcardUid[3]);
				}
			}
		else if(KErrNone == fieldName.Compare(KChildren))
			{
			// Get the values from ini file
			GetStringFromConfig(ConfigSection(), KIniFieldVal, fieldVal);
			GetStringFromConfig(ConfigSection(), KIniFlagvalue, fieldMultiple);
			if(fieldMultiple.Compare(KMultiple)==0)
				{
				AddMultipleContactsL(fieldVal, iStorage[4],iFieldUid[4], iVcardUid[4]);
				}
			else
				{
				OpenExistingContactL(fieldVal, iStorage[4],iFieldUid[4], iVcardUid[4]);
				}
			}
		else if(KErrNone == fieldName.Compare(KClass))
			{
			// Get the values from ini file
			GetStringFromConfig(ConfigSection(), KIniFieldVal, fieldVal);
			GetStringFromConfig(ConfigSection(), KIniFlagvalue, fieldMultiple);
			if(fieldMultiple.Compare(KMultiple)==0)
				{
				AddMultipleContactsL(fieldVal, iStorage[5],iFieldUid[5], iVcardUid[5]);
				}
			else
				{
				OpenExistingContactL(fieldVal, iStorage[5],iFieldUid[5], iVcardUid[5]);
				}
			}
		else if(KErrNone == fieldName.Compare(KDepartment))
			{
			// Get the values from ini file
			GetStringFromConfig(ConfigSection(), KIniFieldVal, fieldVal);
			GetStringFromConfig(ConfigSection(), KIniFlagvalue, fieldMultiple);
			if(fieldMultiple.Compare(KMultiple)==0)
				{
				AddMultipleContactsL(fieldVal, iStorage[6],iFieldUid[6], iVcardUid[6]);
				}
			else
				{
				OpenExistingContactL(fieldVal, iStorage[6],iFieldUid[6], iVcardUid[6]);
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
/** Create Contact Item with field and add contact Item
 *	Contact database.Reopen the contact item and add field to it. 
 *	aValue - field value
 *  @param aStorageType - Storage type of each field
 *	@param aContFieldUid - Uid of Contact field
 *	@param avCardMapUid - Uid of vCard 
 *	@return - void
*/		

void CTestAddMultipleContactsStep::OpenExistingContactL(TPtrC aValue, TInt aStorageType,TUid aContFieldUid, TUid avCardMap)
	{
	TPtrC value,fielddel;
   _LIT(KTestDbName, "c:contactexist.cdb");
   _LIT(KExistDelete, "exist");
   	TInt year, month, day;	
   	
    // Create contact item and add field to it
	CContactItem* item = CContactCard::NewLC();
    CContactItemField* field = CContactItemField::NewL(aStorageType, aContFieldUid);
    CleanupStack::PushL(field); 
    field->SetMapping(avCardMap);
    if(aStorageType==KStorageTypeDateTime)
    	{
    	GetIntFromConfig(ConfigSection(), KIniYear, year);
    	GetIntFromConfig(ConfigSection(), KIniMonth, month);
    	GetIntFromConfig(ConfigSection(), KIniDay, day);
		TDateTime date(year,(TMonth)month,day,0,0,0,0);
   		field->DateTimeStorage()->SetTime(date);
    	}
    else
    	{
    	field->TextStorage()->SetTextL(aValue);	
    	}	
   	item->AddFieldL(*field);  
   	CleanupStack::Pop(field);
   
   	//replace existing database name
   	database = CContactDatabase::ReplaceL(KTestDbName);  
   	
   	// add contact item to database  
  	id = database->AddNewContactL(*item);
  	CleanupStack::PopAndDestroy(item);
  
  	//reopen the contact
  	CContactItem *openItem = database->OpenContactL(id);
  	CleanupStack::PushL(openItem);
  	
  	//create contactitem field and add to it.
  	CContactItemField* newField = CContactItemField::NewL(aStorageType, aContFieldUid);
    CleanupStack::PushL(newField); 
    newField->SetMapping(avCardMap);
    if(aStorageType==KStorageTypeDateTime)
    	{
    	GetIntFromConfig(ConfigSection(), KIniYear, year);
    	GetIntFromConfig(ConfigSection(), KIniMonth, month);
    	GetIntFromConfig(ConfigSection(), KIniDay, day);
		TDateTime date(year,(TMonth)month,day,0,0,0,0);
   		newField->DateTimeStorage()->SetTime(date);
    	}
    else
    	{
    	newField->TextStorage()->SetTextL(aValue);	
    	}	
   	openItem->AddFieldL(*newField);  
   	CleanupStack::Pop(newField);
  
   	// verify the field is being added   
   	CContactItemFieldSet& ContactFieldSetOpen = openItem->CardFields();
   	TInt startPos=0;
   	for(TInt check=0; check<2; check++)
   		{
		TInt pos = ContactFieldSetOpen.FindNext(aContFieldUid, avCardMap, startPos);
		TInt noOfFields = ContactFieldSetOpen.Count();	
		if(pos != KErrNotFound)	
			{
			if(aStorageType==KStorageTypeDateTime)
				{
				TDateTime result=ContactFieldSetOpen[pos].DateTimeStorage()->Time().DateTime();
				startPos=startPos+1;
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
				value.Set(ContactFieldSetOpen[pos].TextStorage()->Text());
				startPos=startPos+1;
				//compare the results
				if(aValue.Compare(value)==0 )
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
	GetStringFromConfig(ConfigSection(), KIniFlagDelete, fielddel);
  	if(KErrNone == fielddel.Compare(KExistDelete))
		{
		TRAPD(ret, DeleteExistContactL(aContFieldUid, avCardMap));
		if(ret!=KErrNone)
			{
			database->CloseContactL(id);
  			delete database;
  			return;
			}
		}
	database->CloseContactL(id);
	CleanupStack::PopAndDestroy(openItem);
  	delete database;		
	}
/** Delete existing contact item field.
 *	Contact database.Reopen the contact item and add field to it. 
 *	@param aContFieldUid - Uid of Contact field
 *	@param avCardMapUid - Uid of vCard 
 *	@return - void
*/	
void CTestAddMultipleContactsStep::DeleteExistContactL(TUid aContFieldUid, TUid avCardMap)	
	{
	CContactItem *readItem = NULL;
    readItem = database->ReadContactLC(id);
    CContactItemFieldSet& ContactFieldSet = readItem->CardFields();
	TInt pos = ContactFieldSet.Find(aContFieldUid, avCardMap);
	if(pos != KErrNotFound)
		{
		ContactFieldSet.Remove(pos);
		SetTestStepResult(EPass);
		INFO_PRINTF1(_L("field deleted"));
		}
	else
		{
		INFO_PRINTF1(_L("field not deleted"));
		SetTestStepResult(EFail);
		}
	CleanupStack::PopAndDestroy(readItem);	
	}	
/** Create Multiple Contact Item with field and Add all contact
 *	Item to Contact database. verify field is 
 *	being added 
 *	aValue - field value
 *  @param aStorageType - Storage type of each field
 *	@param aContFieldUid - Uid of Contact field
 *	@param avCardMapUid - Uid of vCard 
 *	@return - void
*/		
void CTestAddMultipleContactsStep::AddMultipleContactsL(TPtrC aValue, TInt aStorageType,TUid aContFieldUid, TUid avCardMap)	
	{
	TPtrC value;
   _LIT(KTestDbName, "c:contactmultiple.cdb");
   	TInt year, month, day,noOfCI;	
   	RArray <TInt> idArray;
	CleanupClosePushL(idArray); 
	
	//replace existing database name
	database = CContactDatabase::ReplaceL(KTestDbName); 
	 
	//Get the number of contact item from ini file
	GetIntFromConfig(ConfigSection(), KNoOfContactItem, noOfCI);
	
	//Get the int value
	GetIntFromConfig(ConfigSection(), KIniYear, year);
	GetIntFromConfig(ConfigSection(), KIniMonth, month);
	GetIntFromConfig(ConfigSection(), KIniDay, day);
	
    // Create contact item and add field to it
    for(TInt range=0;range<noOfCI;range++)
    	{
		CContactItem* item = CContactCard::NewLC();
		CContactItemField* field = CContactItemField::NewL(aStorageType, aContFieldUid);
		CleanupStack::PushL(field); 
		field->SetMapping(avCardMap);
		if(aStorageType==KStorageTypeDateTime)
			{
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
		id = database->AddNewContactL(*item);
		
		//append the id in array
		idArray.Append(id);
		
		//Cleanup
		CleanupStack::PopAndDestroy(item);	
    	}
    database->CloseContactL(id);
    
    // verify for multiple contact item with field is being created
    CContactItem *readItem = NULL;
    for(TInt readContact=0;readContact<noOfCI;readContact++)
    	{
    	readItem = database->ReadContactL(idArray[readContact]);
    	CleanupStack::PushL(readItem);
    	CContactItemFieldSet& ContactFieldSet = readItem->CardFields();
		TInt pos = ContactFieldSet.Find(aContFieldUid, avCardMap);	
		if(pos != KErrNotFound)	
			{
			if(aStorageType==KStorageTypeDateTime)
				{
				TDateTime result=ContactFieldSet[pos].DateTimeStorage()->Time().DateTime();
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
				if(aValue.Compare(value)==0 )
					{
					SetTestStepResult(EPass);
					}
				else
					{
					SetTestStepResult(EFail);
					}
				}
			}
		CleanupStack::PopAndDestroy(readItem);		
    	}
    // Cleanup
    CleanupStack::PopAndDestroy(&idArray);
  	delete database;	
	}

TVerdict CTestAddMultipleContactsStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CActiveScheduler::Install(NULL);
	delete iScheduler;
	return TestStepResult();
	}


