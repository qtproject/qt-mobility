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


#include "testaddfieldsstep.h"
#include "testcontactsuiteserver.h"
#include <cntitem.h>
#include <cntfield.h>
#include <cntfldst.h> 
#include <cntdef.h> 
#include <cntdb.h> 

TInt CTestAddFieldsStep::iStorage[]=
{
KStorageTypeText, KStorageTypeText, KStorageTypeDateTime, KStorageTypeText,KStorageTypeText,KStorageTypeText,KStorageTypeText 		
};

TUid CTestAddFieldsStep::iFieldUid[]=
{
KUidContactFieldAssistant, KUidContactFieldPhoneNumber, KUidContactFieldAnniversary,KUidContactFieldSpouse,KUidContactFieldChildren,KUidContactFieldClass,KUidContactFieldDepartmentName 		
};

TUid CTestAddFieldsStep::iVcardUid[]=
{
KUidContactFieldVCardMapAssistant, KUidContactFieldVCardMapAssistantTel, KUidContactFieldVCardMapAnniversary, KUidContactFieldVCardMapSpouse,KUidContactFieldVCardMapChildren,KUidContactFieldVCardMapClass,KUidContactFieldVCardMapDepartment 		
};

CTestAddFieldsStep::~CTestAddFieldsStep()
/**
 * Destructor
 */
	{

	}

CTestAddFieldsStep::CTestAddFieldsStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KTestAddFieldsStep);
	}

TVerdict CTestAddFieldsStep::doTestStepPreambleL()
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

TVerdict CTestAddFieldsStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
	TPtrC fieldName, fieldVal, fieldCouple, allFields, fieldTemplate;  
	_LIT(KCouple,	"couple");
	_LIT(KAll,		"all");
	_LIT(KTemplate,	"template");
	
	GetStringFromConfig(ConfigSection(), KIniFlagvalue, allFields);
	if(KErrNone==allFields.Compare(KAll))	
		{
		AddAllFieldsL();
		}
	else if (!GetStringFromConfig(ConfigSection(), KIniFieldName, fieldName))
		{
		ERR_PRINTF1(_L("Unable to read fieldname from ini file"));
		SetTestStepResult(EFail);
		}
		
	else
		{
		if(KErrNone == fieldName.Compare(KAssistance))
			{
			// Get the value from ini file
			GetStringFromConfig(ConfigSection(), KIniFieldVal, fieldVal);
			GetStringFromConfig(ConfigSection(), KIniFlagvalue, fieldCouple);
			GetStringFromConfig(ConfigSection(), KIniFlagvalue, fieldTemplate);
			
			if(fieldCouple.Compare(KCouple)==0)
				{
				AddCoupleFieldsL(fieldVal, iStorage[0],iFieldUid[0], iVcardUid[0]);
				}
			else if(fieldTemplate.Compare(KTemplate)==0)
				{
				AddFieldsNewTemplateL(fieldVal, iStorage[0],iFieldUid[0], iVcardUid[0]);	
				}	
			else
				{
				AddFieldsL(fieldVal, iStorage[0],iFieldUid[0], iVcardUid[0]);
				}	
			}
		else if(KErrNone == fieldName.Compare(KAssistanceTel))	
			{
			// Get the value from ini file
			GetStringFromConfig(ConfigSection(), KIniFieldVal, fieldVal);
			GetStringFromConfig(ConfigSection(), KIniFlagvalue, fieldCouple);
			GetStringFromConfig(ConfigSection(), KIniFlagvalue, fieldTemplate);
			
			if(fieldCouple.Compare(KCouple) == 0)
				{
				AddCoupleFieldsL(fieldVal, iStorage[1],iFieldUid[1], iVcardUid[1]);
				}
			else if(fieldTemplate.Compare(KTemplate) == 0)
				{
				AddFieldsNewTemplateL(fieldVal, iStorage[1],iFieldUid[1], iVcardUid[1]);	
				}	
			else
				{
				AddFieldsL(fieldVal, iStorage[1],iFieldUid[1], iVcardUid[1]);
				}	
			}
		else if(KErrNone == fieldName.Compare(KAnniversary))	
			{
			// Get the value from ini file
			GetStringFromConfig(ConfigSection(), KIniFieldVal, fieldVal);
			GetStringFromConfig(ConfigSection(), KIniFlagvalue, fieldCouple);
			GetStringFromConfig(ConfigSection(), KIniFlagvalue, fieldTemplate);
			
			if(fieldCouple.Compare(KCouple)==0)
				{
				AddCoupleFieldsL(fieldVal, iStorage[2],iFieldUid[2], iVcardUid[2]);
				}
			else if(fieldTemplate.Compare(KTemplate)==0)
				{
				AddFieldsNewTemplateL(fieldVal, iStorage[2],iFieldUid[2], iVcardUid[2]);	
				}	
			else
				{
				AddFieldsL(fieldVal, iStorage[2],iFieldUid[2], iVcardUid[2]);
				}
			
			}
		else if(KErrNone == fieldName.Compare(KSpouse))	
			{
			// Get the value from ini file
			GetStringFromConfig(ConfigSection(), KIniFieldVal, fieldVal);
			GetStringFromConfig(ConfigSection(), KIniFlagvalue, fieldCouple);
			GetStringFromConfig(ConfigSection(), KIniFlagvalue, fieldTemplate);
			
			if(fieldCouple.Compare(KCouple)==0)
				{
				AddCoupleFieldsL(fieldVal, iStorage[3],iFieldUid[3], iVcardUid[3]);
				}
			else if(fieldTemplate.Compare(KTemplate)==0)
				{
				AddFieldsNewTemplateL(fieldVal, iStorage[3],iFieldUid[3], iVcardUid[3]);	
				}	
			else
				{
				AddFieldsL(fieldVal, iStorage[3],iFieldUid[3], iVcardUid[3]);
				}
			}
		else if(KErrNone == fieldName.Compare(KChildren))	
			{
			// Get the value from ini file
			GetStringFromConfig(ConfigSection(), KIniFieldVal, fieldVal);
			GetStringFromConfig(ConfigSection(), KIniFlagvalue, fieldCouple);
			GetStringFromConfig(ConfigSection(), KIniFlagvalue, fieldTemplate);
			
			if(fieldCouple.Compare(KCouple)==0)
				{
				AddCoupleFieldsL(fieldVal, iStorage[4],iFieldUid[4], iVcardUid[4]);
				}
			else if(fieldTemplate.Compare(KTemplate)==0)
				{
				AddFieldsNewTemplateL(fieldVal, iStorage[4],iFieldUid[4], iVcardUid[4]);	
				}	
			else
				{
				AddFieldsL(fieldVal, iStorage[4],iFieldUid[4], iVcardUid[4]);
				}
			}
		else if(KErrNone == fieldName.Compare(KClass))	
			{
			// Get the value from ini file
			GetStringFromConfig(ConfigSection(), KIniFieldVal, fieldVal);
			GetStringFromConfig(ConfigSection(), KIniFlagvalue, fieldCouple);
			GetStringFromConfig(ConfigSection(), KIniFlagvalue, fieldTemplate);
			
			if(fieldCouple.Compare(KCouple)==0)
				{
				AddCoupleFieldsL(fieldVal, iStorage[5],iFieldUid[5], iVcardUid[5]);
				}
			else if(fieldTemplate.Compare(KTemplate)==0)
				{
				AddFieldsNewTemplateL(fieldVal, iStorage[5],iFieldUid[5], iVcardUid[5]);	
				}	
			else
				{
				AddFieldsL(fieldVal, iStorage[5],iFieldUid[5], iVcardUid[5]);
				}
			}	
		else if(KErrNone == fieldName.Compare(KDepartment))	
			{
			// Get the value from ini file
			GetStringFromConfig(ConfigSection(), KIniFieldVal, fieldVal);
			GetStringFromConfig(ConfigSection(), KIniFlagvalue, fieldCouple);
			GetStringFromConfig(ConfigSection(), KIniFlagvalue, fieldTemplate);
			
			if(fieldCouple.Compare(KCouple)==0)
				{
				AddCoupleFieldsL(fieldVal, iStorage[6],iFieldUid[6], iVcardUid[6]);
				}
			else if(fieldTemplate.Compare(KTemplate)==0)
				{
				AddFieldsNewTemplateL(fieldVal, iStorage[6],iFieldUid[6], iVcardUid[6]);	
				}	
			else
				{
				AddFieldsL(fieldVal, iStorage[6],iFieldUid[6], iVcardUid[6]);
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
/** Create Contact Item and add field to it and 
 *	verify the field is being added
 *	aValue - field value
 *  @param aStorageType - Storage type of each field
 *	@param aContFieldUid - Uid of Contact field
 *	@param avCardMapUid - Uid of vCard 
 *	@return - void
*/	
void CTestAddFieldsStep::AddFieldsL(TPtrC aValue, TInt aStorageType,TUid aContFieldUid, TUid avCardMap)	
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
  
   	// verify the field is being added    	
	CContactItemFieldSet& ContactFieldSet = item->CardFields();
	TInt pos = ContactFieldSet.Find(aContFieldUid, avCardMap);	
	if(pos != KErrNotFound)	
		{
		if(aStorageType==KStorageTypeDateTime)
			{
			TDateTime result=ContactFieldSet[pos].DateTimeStorage()->Time().DateTime();
			//compair the results
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
			//compair the results
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
	CleanupStack::Pop(field);
	CleanupStack::PopAndDestroy(item);
	}
/** Create Contact Item and add couple of same fields to it and 
 *	verify the field is being added
 *	aValue - field value
 *  @param aStorageType - Storage type of each field
 *	@param aContFieldUid - Uid of Contact field
 *	@param avCardMapUid - Uid of vCard 
 *	@return - void
*/		
void CTestAddFieldsStep::AddCoupleFieldsL(TPtrC aValue, TInt aStorageType,TUid aContFieldUid, TUid avCardMap)	
	{
	TPtrC value;
	TInt year, month, day;
	CContactItemField* field;
	
	// Get the value form ini file
	GetIntFromConfig(ConfigSection(), KIniYear, year);
    GetIntFromConfig(ConfigSection(), KIniMonth, month);
    GetIntFromConfig(ConfigSection(), KIniDay, day);
    
	// Create contact Item and add same field twice to it
	CContactItem* item = CContactCard::NewLC();
	for(TInt x=0; x<2; x++)
		{
    	field = CContactItemField::NewL(aStorageType, aContFieldUid);
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
		}  
	CContactItemFieldSet& ContactFieldSetOpen = item->CardFields();	
	
   	// verify the field is being added 
   	TInt startPos=0;
   	for(TInt check=0; check<2; check++)
   		{
		TInt pos = ContactFieldSetOpen.FindNext(aContFieldUid, avCardMap,startPos);	
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
				//compair the results
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
	CleanupStack::PopAndDestroy(item);	
	}
/** Create Contact Item and add all fields to it and 
 *	verify the field is being added
 *	@return - void
*/	
void CTestAddFieldsStep::AddAllFieldsL()	
	{
	RArray<TPtrC> readValues;
	CleanupClosePushL(readValues);
	TPtrC temp, value;
	TInt year, month, day,readPos=0;
	
	// get string value from ini file.
	if(!GetStringFromConfig(ConfigSection(), KIniFieldAsstVal, temp))
		{
		ERR_PRINTF1(_L("Unable to read val from ini file"));
		SetTestStepResult(EFail);
		}
	else
		{
		readValues.Append(temp);
		}	
	if(!GetStringFromConfig(ConfigSection(), KIniFieldAsstTelVal, temp))
		{
		ERR_PRINTF1(_L("Unable to read val from ini file"));
		SetTestStepResult(EFail);
		}
	else
		{
		readValues.Append(temp);
		}
	if(!GetStringFromConfig(ConfigSection(), KIniFieldSpouseVal, temp))
		{
		ERR_PRINTF1(_L("Unable to read val from ini file"));
		SetTestStepResult(EFail);
		}
	else
		{
		readValues.Append(temp);
		}	
	if(!GetStringFromConfig(ConfigSection(), KIniFieldChildVel, temp))
		{
		ERR_PRINTF1(_L("Unable to read val from ini file"));
		SetTestStepResult(EFail);
		}
	else
		{
		readValues.Append(temp);	
		}	
	if(!GetStringFromConfig(ConfigSection(), KIniFieldClassVal, temp))
		{
		ERR_PRINTF1(_L("Unable to read val from ini file"));
		SetTestStepResult(EFail);
		}
	else
		{
		readValues.Append(temp);	
		}
	if(!GetStringFromConfig(ConfigSection(), KIniFieldDeptVal, temp))
		{
		ERR_PRINTF1(_L("Unable to read val from ini file"));
		SetTestStepResult(EFail);
		}
	else
		{
		readValues.Append(temp);	
		}	
	
	// get int val from ini file
	GetIntFromConfig(ConfigSection(), KIniYear, year);
    GetIntFromConfig(ConfigSection(), KIniMonth, month);
    GetIntFromConfig(ConfigSection(), KIniDay, day);
    	
	CContactItemField* field = NULL;
	// Create contact Item and add all fields to it
	CContactItem* item = CContactCard::NewLC();
	for(TInt x=0; x<7; x++)
		{
    	field = CContactItemField::NewL(iStorage[x], iFieldUid[x]);
    	CleanupStack::PushL(field); 
    	field->SetMapping(iVcardUid[x]);
    	if(iStorage[x]==KStorageTypeDateTime)
    		{
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
	readPos=0;
	// verify the field is being added    
	for(TInt x=0; x<7; x++)	
	{	
		CContactItemFieldSet& ContactFieldSet = item->CardFields();
		TInt pos = ContactFieldSet.Find(iFieldUid[x], iVcardUid[x]);	
		if(pos != KErrNotFound)	
			{
			if(iStorage[x]==KStorageTypeDateTime)
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
				if(value.Compare(readValues[readPos++])==0 )
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
	
	//Cleanup stack
	CleanupStack::PopAndDestroy(item);
	CleanupStack::PopAndDestroy(&readValues);
	}
TVerdict CTestAddFieldsStep::doTestStepPostambleL()
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
 *	through template and verify the field is being added
 *	aValue - field value
 *  @param aStorageType - Storage type of each field
 *	@param aContFieldUid - Uid of Contact field
 *	@param avCardMapUid - Uid of vCard 
 *	@return - void
*/	
void CTestAddFieldsStep::AddFieldsNewTemplateL(TPtrC aValue, TInt aStorageType,TUid aContFieldUid, TUid avCardMap)	
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

   	// verify the field is being added to the user defined template    	
	CContactItemFieldSet& ContactFieldSet = item->CardFields();
	TInt pos = ContactFieldSet.Find(aContFieldUid, avCardMap);	
	if(pos != KErrNotFound)	
		{
		if(aStorageType==KStorageTypeDateTime)
			{
			TDateTime result=ContactFieldSet[pos].DateTimeStorage()->Time().DateTime();
			//compair the results
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
			//compair the results
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
	CleanupStack::Pop(field);
	CleanupStack::PopAndDestroy(item);
	CleanupStack::PopAndDestroy(contactTemplate);
	CleanupStack::PopAndDestroy(base);
	}
