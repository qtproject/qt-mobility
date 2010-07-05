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


#include "testupdatefieldsstep.h"
#include "testcontactsuiteserver.h"
#include <cntitem.h>
#include <cntfield.h>
#include <cntfldst.h>
#include <cntdef.h>
#include <cntdb.h>

TInt CTestUpdateFieldsStep::iStorage[]=
{
KStorageTypeText, KStorageTypeText, KStorageTypeDateTime, KStorageTypeText,KStorageTypeText,KStorageTypeText,KStorageTypeText 		
};

TUid CTestUpdateFieldsStep::iFieldUid[]=
{
KUidContactFieldAssistant, KUidContactFieldPhoneNumber, KUidContactFieldAnniversary, KUidContactFieldSpouse,KUidContactFieldChildren,KUidContactFieldClass,KUidContactFieldDepartmentName 		
};

TUid CTestUpdateFieldsStep::iVcardUid[]=
{
KUidContactFieldVCardMapAssistant, KUidContactFieldVCardMapAssistantTel, KUidContactFieldVCardMapAnniversary, KUidContactFieldVCardMapSpouse,KUidContactFieldVCardMapChildren,KUidContactFieldVCardMapClass,KUidContactFieldVCardMapDepartment 		
};


CTestUpdateFieldsStep::~CTestUpdateFieldsStep()
/**
 * Destructor
 */
	{

	}

CTestUpdateFieldsStep::CTestUpdateFieldsStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KTestUpdateFieldsStep);
	}

TVerdict CTestUpdateFieldsStep::doTestStepPreambleL()
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

TVerdict CTestUpdateFieldsStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
	TPtrC fieldName, fieldVal, fieldTemplate;
	_LIT(KTemplate,	"template");
	
	if (!GetStringFromConfig(ConfigSection(), KIniFieldName, fieldName))
		{
		ERR_PRINTF1(_L("Unable to read fieldname from ini file"));
		SetTestStepResult(EFail);
		}
	else
		{
		if(KErrNone == fieldName.Compare(KAssistance))
			{
			//Get the field value
			GetStringFromConfig(ConfigSection(), KIniFieldVal, fieldVal);
			GetStringFromConfig(ConfigSection(), KIniFlagvalue, fieldTemplate);
			
			if(KErrNone == fieldTemplate.Compare(KTemplate))
				{
				UpdateFieldsNewTemplateL(fieldVal, iStorage[0],iFieldUid[0], iVcardUid[0]);
				}
			else
				{
				UpdateFieldsL(fieldVal, iStorage[0],iFieldUid[0], iVcardUid[0]);	
				}	
			}
		else if(KErrNone == fieldName.Compare(KAssistanceTel))
			{
			//Get the field value
			GetStringFromConfig(ConfigSection(), KIniFieldVal, fieldVal);
			GetStringFromConfig(ConfigSection(), KIniFlagvalue, fieldTemplate);
			
			if(KErrNone == fieldTemplate.Compare(KTemplate))
				{
				UpdateFieldsNewTemplateL(fieldVal, iStorage[1],iFieldUid[1], iVcardUid[1]);
				}
			else
				{
				UpdateFieldsL(fieldVal, iStorage[1],iFieldUid[1], iVcardUid[1]);
				}
			}
		else if(KErrNone == fieldName.Compare(KAnniversary))
			{
			//Get the field value 
			GetStringFromConfig(ConfigSection(), KIniFieldVal, fieldVal);
			GetStringFromConfig(ConfigSection(), KIniFlagvalue, fieldTemplate);
			
			if(KErrNone == fieldTemplate.Compare(KTemplate))
				{
				UpdateFieldsNewTemplateL(fieldVal, iStorage[2],iFieldUid[2], iVcardUid[2]);
				}
			else
				{
				UpdateFieldsL(fieldVal, iStorage[2],iFieldUid[2], iVcardUid[2]);	
				}	
			}
		else if(KErrNone == fieldName.Compare(KSpouse))
			{
			//Get the field value
			GetStringFromConfig(ConfigSection(), KIniFieldVal, fieldVal);
			GetStringFromConfig(ConfigSection(), KIniFlagvalue, fieldTemplate);
			
			if(KErrNone == fieldTemplate.Compare(KTemplate))
				{
				UpdateFieldsNewTemplateL(fieldVal, iStorage[3],iFieldUid[3], iVcardUid[3]);
				}
			else
				{
				UpdateFieldsL(fieldVal, iStorage[3],iFieldUid[3], iVcardUid[3]);	
				}	
			}
		else if(KErrNone == fieldName.Compare(KChildren))
			{
			//Get the field value
			GetStringFromConfig(ConfigSection(), KIniFieldVal, fieldVal);
			GetStringFromConfig(ConfigSection(), KIniFlagvalue, fieldTemplate);
			
			if(KErrNone == fieldTemplate.Compare(KTemplate))
				{
				UpdateFieldsNewTemplateL(fieldVal, iStorage[4],iFieldUid[4], iVcardUid[4]);
				}
			else
				{
				UpdateFieldsL(fieldVal, iStorage[4],iFieldUid[4], iVcardUid[4]);	
				}
			}
		else if(KErrNone == fieldName.Compare(KClass))
			{
			//Get the field value
			GetStringFromConfig(ConfigSection(), KIniFieldVal, fieldVal);
			GetStringFromConfig(ConfigSection(), KIniFlagvalue, fieldTemplate);
			
			if(KErrNone == fieldTemplate.Compare(KTemplate))
				{
				UpdateFieldsNewTemplateL(fieldVal, iStorage[5],iFieldUid[5], iVcardUid[5]);
				}
			else
				{
				UpdateFieldsL(fieldVal, iStorage[5],iFieldUid[5], iVcardUid[5]);	
				}	
			}
		else if(KErrNone == fieldName.Compare(KDepartment))
			{
			//Get the field value
			GetStringFromConfig(ConfigSection(), KIniFieldVal, fieldVal);
			GetStringFromConfig(ConfigSection(), KIniFlagvalue, fieldTemplate);
			
			if(KErrNone == fieldTemplate.Compare(KTemplate))
				{
				UpdateFieldsNewTemplateL(fieldVal, iStorage[6],iFieldUid[6], iVcardUid[6]);
				}
			else
				{
				UpdateFieldsL(fieldVal, iStorage[6],iFieldUid[6], iVcardUid[6]);	
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
/** Create Contact Item and add field to it.Update the field value and 
 *	verify the field is being updated
 *	aValue - field value
 *  @param aStorageType - Storage type of each field
 *	@param aContFieldUid - Uid of Contact field
 *	@param avCardMapUid - Uid of vCard 
 *	@return - void
*/		
void CTestUpdateFieldsStep::UpdateFieldsL(TPtrC aValue, TInt aStorageType,TUid aContFieldUid, TUid avCardMap)
	{
	TPtrC value, updateVal, getUpdateValue;
	TInt year, month, day;
	
	// get the value from ini file
	GetIntFromConfig(ConfigSection(), KIniYear, year);
    GetIntFromConfig(ConfigSection(), KIniMonth, month);
    GetIntFromConfig(ConfigSection(), KIniDay, day);
    
	//Create contact Item and add field to it.
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

   	// Get the value from ini file for update
   	GetStringFromConfig(ConfigSection(), KIniUpdatedvalue, updateVal);
	GetIntFromConfig(ConfigSection(), KIniUpdateYear, year);
    GetIntFromConfig(ConfigSection(), KIniUpdateMonth, month);
    GetIntFromConfig(ConfigSection(), KIniUpdateDay, day);
    
   	
	CContactItemFieldSet& ContactFieldSet = item->CardFields();
	TInt pos = ContactFieldSet.Find(aContFieldUid, avCardMap);
	
	if(aStorageType==KStorageTypeDateTime)
    	{
    	// update the field value
		TDateTime date(year,(TMonth)month,day,0,0,0,0);
   		field->DateTimeStorage()->SetTime(date);
   		ContactFieldSet.UpdateFieldL(ContactFieldSet[pos],pos);
   		
   		// verify the updated field value
   		TInt upadtePos = ContactFieldSet.Find(aContFieldUid, avCardMap);
   		TDateTime result=ContactFieldSet[upadtePos].DateTimeStorage()->Time().DateTime();
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
    	// Update the field value
		ContactFieldSet[pos].TextStorage()->SetTextL(updateVal);
		ContactFieldSet.UpdateFieldL(ContactFieldSet[pos],pos);
		
		// verify the field is being updated 
		TInt upadtePos = ContactFieldSet.Find(aContFieldUid, avCardMap);
		getUpdateValue.Set(ContactFieldSet[upadtePos].TextStorage()->Text());
	
		//compair the results
		if(getUpdateValue.Compare(updateVal)==0 )
			{
			SetTestStepResult(EPass);
			}
		else
			{
			SetTestStepResult(EFail);
			}
    	}

	// Cleanup
	CleanupStack::Pop(field);
	CleanupStack::PopAndDestroy(item);

	}

TVerdict CTestUpdateFieldsStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CActiveScheduler::Install(NULL);
	delete iScheduler;	
	return TestStepResult();
	}
/** Create Contact user defuned Template add add field to contact item  
 *	through template. update the contact item and verify the field is being updated
 *	aValue - field value
 *  @param aStorageType - Storage type of each field
 *	@param aContFieldUid - Uid of Contact field
 *	@param avCardMapUid - Uid of vCard 
 *	@return - void
*/	
void CTestUpdateFieldsStep::UpdateFieldsNewTemplateL(TPtrC aValue, TInt aStorageType,TUid aContFieldUid, TUid avCardMap)
	{
	TPtrC value, updateVal, getUpdateValue;
	TInt year, month, day;
	CContactDatabase *base = NULL;
	_LIT(KTestDbName, "c:mytemplate1.cdb");
	_LIT(KUserDefinedTemplate,"MyTemplate");
	
	// replace existing database name
	base = CContactDatabase::ReplaceL(KTestDbName);
	CleanupStack::PushL(base);
		
	TContactItemId TempId ;
	CContactItemField* field = NULL;
	
	//Create User defined Template
	CContactItem* contactTemplate = base->CreateContactCardTemplateLC(KUserDefinedTemplate);
	
	// Get the ID of Template
	TempId = contactTemplate->Id();
	
	//Add fields to it
    field = CContactItemField::NewL(aStorageType, aContFieldUid);
    CleanupStack::PushL(field); 
    field->SetMapping(avCardMap);
    contactTemplate->AddFieldL(*field);  
   	CleanupStack::Pop(field);
   	
	//Allocates and constructs a new contact card.
	CContactItem* item = CContactCard::NewLC();
	
	//Sets the ID of the template on which this contact item is based
	item->SetTemplateRefId(TempId);
	
	//create contact item and add field value to it
    field = CContactItemField::NewL(aStorageType, aContFieldUid);
    CleanupStack::PushL(field); 
    field->SetMapping(avCardMap);
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
		TDateTime date(year,(TMonth)month,day,0,0,0,0);
   		field->DateTimeStorage()->SetTime(date);
    	}
    else
    	{
    	field->TextStorage()->SetTextL(aValue);	
    	}	
   	item->AddFieldL(*field); 
   	
   	// Get the value from ini file for update
   	GetStringFromConfig(ConfigSection(), KIniUpdatedvalue, updateVal);
	GetIntFromConfig(ConfigSection(), KIniUpdateYear, year);
    GetIntFromConfig(ConfigSection(), KIniUpdateMonth, month);
    GetIntFromConfig(ConfigSection(), KIniUpdateDay, day);
   
	CContactItemFieldSet& ContactFieldSet = item->CardFields();
	TInt pos = ContactFieldSet.Find(aContFieldUid, avCardMap);
	
	if(aStorageType==KStorageTypeDateTime)
    	{
    	// update the field value
		TDateTime date(year,(TMonth)month,day,0,0,0,0);
   		field->DateTimeStorage()->SetTime(date);
   		ContactFieldSet.UpdateFieldL(ContactFieldSet[pos],pos);
   		
   		// verify the updated field value
   		TInt upadtePos = ContactFieldSet.Find(aContFieldUid, avCardMap);
   		TDateTime result=ContactFieldSet[upadtePos].DateTimeStorage()->Time().DateTime();
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
    	// Update the field value
		ContactFieldSet[pos].TextStorage()->SetTextL(updateVal);
		ContactFieldSet.UpdateFieldL(ContactFieldSet[pos],pos);
		
		// verify the field is being updated 
		TInt upadtePos = ContactFieldSet.Find(aContFieldUid, avCardMap);
		getUpdateValue.Set(ContactFieldSet[upadtePos].TextStorage()->Text());
	
		//compair the results
		if(getUpdateValue.Compare(updateVal)==0 )
			{
			SetTestStepResult(EPass);
			}
		else
			{
			SetTestStepResult(EFail);
			}
    	}
   	CleanupStack::Pop(field);
	CleanupStack::PopAndDestroy(item);
	CleanupStack::PopAndDestroy(contactTemplate);
	CleanupStack::PopAndDestroy(base);
	}
