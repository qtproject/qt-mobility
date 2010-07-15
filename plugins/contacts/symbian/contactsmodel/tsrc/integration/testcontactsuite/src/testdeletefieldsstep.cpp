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


#include "testdeletefieldsstep.h"
#include "testcontactsuiteserver.h"
#include <cntitem.h>
#include <cntfield.h>
#include <cntfldst.h>
#include <cntdef.h>
#include <cntdb.h>

TInt CTestDeleteFieldsStep::iStorage[]=
{
KStorageTypeText, KStorageTypeText, KStorageTypeDateTime, KStorageTypeText,KStorageTypeText,KStorageTypeText,KStorageTypeText 		
};

TUid CTestDeleteFieldsStep::iFieldUid[]=
{
KUidContactFieldAssistant, KUidContactFieldPhoneNumber, KUidContactFieldAnniversary,KUidContactFieldSpouse,KUidContactFieldChildren,KUidContactFieldClass,KUidContactFieldDepartmentName 		
};

TUid CTestDeleteFieldsStep::iVcardUid[]=
{
KUidContactFieldVCardMapAssistant, KUidContactFieldVCardMapAssistantTel, KUidContactFieldVCardMapAnniversary,KUidContactFieldVCardMapSpouse,KUidContactFieldVCardMapChildren,KUidContactFieldVCardMapClass,KUidContactFieldVCardMapDepartment 		
};

CTestDeleteFieldsStep::~CTestDeleteFieldsStep()
/**
 * Destructor
 */
	{

	}

CTestDeleteFieldsStep::CTestDeleteFieldsStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KTestDeleteFieldsStep);
	}

TVerdict CTestDeleteFieldsStep::doTestStepPreambleL()
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

TVerdict CTestDeleteFieldsStep::doTestStepL()
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
			// Get the asst value from ini file
			if(!GetStringFromConfig(ConfigSection(), KIniFieldVal, fieldVal))
				{
				ERR_PRINTF1(_L("Unable to read Asstname val from ini file"));
				SetTestStepResult(EFail);
				}
			else
				{
				GetStringFromConfig(ConfigSection(), KIniFlagvalue, fieldTemplate);
				if(KErrNone == fieldTemplate.Compare(KTemplate))
					{
					DeleteFieldsNewTemplateL(fieldVal, iStorage[0],iFieldUid[0], iVcardUid[0]);
					}
				else
					{
					DeleteFieldsL(fieldVal, iStorage[0],iFieldUid[0], iVcardUid[0]);
					}	
				}	
			}
		else if(KErrNone == fieldName.Compare(KAssistanceTel))
			{
			// Get the asst-tel value from ini file
			if(!GetStringFromConfig(ConfigSection(), KIniFieldVal, fieldVal))
				{
				ERR_PRINTF1(_L("Unable to read AsstTel val from ini file"));
				SetTestStepResult(EFail);
				}
			else
				{
				GetStringFromConfig(ConfigSection(), KIniFlagvalue, fieldTemplate);
				if(KErrNone == fieldTemplate.Compare(KTemplate))
					{
					DeleteFieldsNewTemplateL(fieldVal, iStorage[1],iFieldUid[1], iVcardUid[1]);
					}
				else
					{
					DeleteFieldsL(fieldVal, iStorage[1],iFieldUid[1], iVcardUid[1]);
					}	
				}	
			}
		else if(KErrNone == fieldName.Compare(KAnniversary))
			{
			// Get the anniversary value from ini file
			if(!GetStringFromConfig(ConfigSection(), KIniFieldVal, fieldVal))
				{
				ERR_PRINTF1(_L("Unable to read Anni val from ini file"));
				SetTestStepResult(EFail);
				}
			else
				{
				GetStringFromConfig(ConfigSection(), KIniFlagvalue, fieldTemplate);
				if(KErrNone == fieldTemplate.Compare(KTemplate))
					{
					DeleteFieldsNewTemplateL(fieldVal, iStorage[2],iFieldUid[2], iVcardUid[2]);
					}
				else
					{
					DeleteFieldsL(fieldVal, iStorage[2],iFieldUid[2], iVcardUid[2]);
					}	
				}	
			}
		else if(KErrNone == fieldName.Compare(KSpouse))
			{
			// Get the spouse value from ini file
			if(!GetStringFromConfig(ConfigSection(), KIniFieldVal, fieldVal))
				{
				ERR_PRINTF1(_L("Unable to read Spouse val from ini file"));
				SetTestStepResult(EFail);
				}
			else
				{
				GetStringFromConfig(ConfigSection(), KIniFlagvalue, fieldTemplate);
				if(KErrNone == fieldTemplate.Compare(KTemplate))
					{
					DeleteFieldsNewTemplateL(fieldVal, iStorage[3],iFieldUid[3], iVcardUid[3]);
					}
				else
					{
					DeleteFieldsL(fieldVal, iStorage[3],iFieldUid[3], iVcardUid[3]);
					}	
				}	
			}
		else if(KErrNone == fieldName.Compare(KChildren))
			{
			// Get the children value from ini file
			if(!GetStringFromConfig(ConfigSection(), KIniFieldVal, fieldVal))
				{
				ERR_PRINTF1(_L("Unable to read Children val from ini file"));
				SetTestStepResult(EFail);
				}
			else	
				{
				GetStringFromConfig(ConfigSection(), KIniFlagvalue, fieldTemplate);
				if(KErrNone == fieldTemplate.Compare(KTemplate))
					{
					DeleteFieldsNewTemplateL(fieldVal, iStorage[4],iFieldUid[4], iVcardUid[4]);
					}
				else
					{
					DeleteFieldsL(fieldVal, iStorage[4],iFieldUid[4], iVcardUid[4]);
					}	
				}	
			}
		else if(KErrNone == fieldName.Compare(KClass))
			{
			// Get the class value from ini file
			if(!GetStringFromConfig(ConfigSection(), KIniFieldVal, fieldVal))
				{
				ERR_PRINTF1(_L("Unable to read Class val from ini file"));
				SetTestStepResult(EFail);
				}
			else	
				{
				GetStringFromConfig(ConfigSection(), KIniFlagvalue, fieldTemplate);
				if(KErrNone == fieldTemplate.Compare(KTemplate))
					{
					DeleteFieldsNewTemplateL(fieldVal, iStorage[5],iFieldUid[5], iVcardUid[5]);
					}
				else
					{
					DeleteFieldsL(fieldVal, iStorage[5],iFieldUid[5], iVcardUid[5]);
					}	
				}	
			}
		else if(KErrNone == fieldName.Compare(KDepartment))
			{
			// Get the dept value from ini file
			if(!GetStringFromConfig(ConfigSection(), KIniFieldVal, fieldVal))
				{
				ERR_PRINTF1(_L("Unable to read Dept val from ini file"));
				SetTestStepResult(EFail);
				}
			else
				{
				GetStringFromConfig(ConfigSection(), KIniFlagvalue, fieldTemplate);
				if(KErrNone == fieldTemplate.Compare(KTemplate))
					{
					DeleteFieldsNewTemplateL(fieldVal, iStorage[6],iFieldUid[6], iVcardUid[6]);
					}
				else
					{
					DeleteFieldsL(fieldVal, iStorage[6],iFieldUid[6], iVcardUid[6]);
					}	
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
/** Create Contact Item, add field to it and Delete the field.
 *	aValue - field value
 *  @param aStorageType - Storage type of each field
 *	@param aContFieldUid - Uid of Contact field
 *	@param avCardMapUid - Uid of vCard 
 *	@return - void
*/			
void CTestDeleteFieldsStep::DeleteFieldsL(TPtrC aValue, TInt aStorageType,TUid aContFieldUid, TUid avCardMap)
	{
	TPtrC value;
   	TInt year, month, day;
	// Create contact Item and add field to it
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

   	// Delete the field from the contact item
	CContactItemFieldSet& ContactFieldSet = item->CardFields();
	TInt pos = ContactFieldSet.Find(aContFieldUid, avCardMap);
	if(pos != KErrNotFound)
		{
		ContactFieldSet.Remove(pos);
		INFO_PRINTF1(_L("Field deleted successfully"));
		SetTestStepResult(EPass);
		}
	else
		{
		ERR_PRINTF1(_L("field not deleted"));
		SetTestStepResult(EFail);
		}
		
	// pop the item and field
	CleanupStack::Pop(field);
	CleanupStack::PopAndDestroy(item);

	}
void CTestDeleteFieldsStep::DeleteFieldsNewTemplateL(TPtrC aValue, TInt aStorageType,TUid aContFieldUid, TUid avCardMap)	
	{
	TPtrC value;
	TInt year, month, day;
	CContactDatabase *base = NULL;
	_LIT(KTestDbName, "c:mytemplate.cdb");
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
   	
   	// Delete the field from the contact item
	CContactItemFieldSet& ContactFieldSet = item->CardFields();
	TInt pos = ContactFieldSet.Find(aContFieldUid, avCardMap);
	if(pos != KErrNotFound)
		{
		ContactFieldSet.Remove(pos);
		INFO_PRINTF1(_L("Field deleted successfully"));
		SetTestStepResult(EPass);
		}
	else
		{
		ERR_PRINTF1(_L("field not deleted"));
		SetTestStepResult(EFail);
		}
   	CleanupStack::Pop(field);
	CleanupStack::PopAndDestroy(item);
	CleanupStack::PopAndDestroy(contactTemplate);
	CleanupStack::PopAndDestroy(base);
	}

TVerdict CTestDeleteFieldsStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CActiveScheduler::Install(NULL);
	delete iScheduler;
	return TestStepResult();
	}


