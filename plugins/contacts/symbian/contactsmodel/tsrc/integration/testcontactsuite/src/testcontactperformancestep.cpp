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


#include "testcontactperformancestep.h"
#include "testcontactsuiteserver.h"
#include <cntitem.h>
#include <cntfield.h>
#include <cntfldst.h>
#include <cntdef.h>
#include <cntdb.h>
#include <commdb.h>


TInt CTestContactPerformanceStep::iStorage[]=
{
KStorageTypeText, KStorageTypeText, KStorageTypeDateTime, KStorageTypeText,KStorageTypeText,KStorageTypeText,KStorageTypeText
};

TUid CTestContactPerformanceStep::iFieldUid[]=
{
KUidContactFieldAssistant, KUidContactFieldPhoneNumber, KUidContactFieldAnniversary,KUidContactFieldSpouse,KUidContactFieldChildren,KUidContactFieldClass,KUidContactFieldDepartmentName
};

TUid CTestContactPerformanceStep::iVcardUid[]=
{
KUidContactFieldVCardMapAssistant, KUidContactFieldVCardMapAssistantTel, KUidContactFieldVCardMapAnniversary, KUidContactFieldVCardMapSpouse,KUidContactFieldVCardMapChildren,KUidContactFieldVCardMapClass,KUidContactFieldVCardMapDepartment
};

CContactDatabase *dBase = NULL;
RArray <TInt> idArray;

CTestContactPerformanceStep::~CTestContactPerformanceStep()
/**
 * Destructor
 */
	{

	}

CTestContactPerformanceStep::CTestContactPerformanceStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KTestContactPerformanceStep);
	}

TVerdict CTestContactPerformanceStep::doTestStepPreambleL()
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

TVerdict CTestContactPerformanceStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{

	FieldPerformanceL();
	return TestStepResult();
	}

/**
 * Creating 1000 contact item with all new fields
 * @return - void
 */
void CTestContactPerformanceStep::FieldPerformanceL()
	{
	TPtrC value, temp;
   _LIT(KTestDbName, "c:contactmultiple.cdb");
   	TInt readPos=0;
   	TContactItemId id = 0;
   	_LIT(KUpdate,	"update");
   	_LIT(KDelete,	"delete");
	CleanupClosePushL(idArray);
	RArray<TPtrC> readValues;
	CleanupClosePushL(readValues);
	
	// get string value from ini file.
	GetStringFromConfig(ConfigSection(), KIniFieldAsstVal, temp);
	readValues.Append(temp);
	GetStringFromConfig(ConfigSection(), KIniFieldAsstTelVal, temp);
	readValues.Append(temp);
	GetStringFromConfig(ConfigSection(), KIniFieldSpouseVal, temp);
	readValues.Append(temp);
	GetStringFromConfig(ConfigSection(), KIniFieldChildVel, temp);
	readValues.Append(temp);
	GetStringFromConfig(ConfigSection(), KIniFieldClassVal, temp);
	readValues.Append(temp);
	GetStringFromConfig(ConfigSection(), KIniFieldDeptVal, temp);
	readValues.Append(temp);
	
	// get int val from ini file
	GetIntFromConfig(ConfigSection(), KIniYear, iYear);
    GetIntFromConfig(ConfigSection(), KIniMonth, iMonth);
    GetIntFromConfig(ConfigSection(), KIniDay, iDay);
    	
	//replace existing database name
	dBase = CContactDatabase::ReplaceL(KTestDbName);

	//Get the number of contact item from ini file
	GetIntFromConfig(ConfigSection(), KNoOfContactItem, iNoOfCI);

    // Create contact item and add all fields to it
    for(TInt range=0;range<iNoOfCI;range++)
    	{
		CContactItem* item = CContactCard::NewLC();
		for(TInt noOfFields=0; noOfFields<7; noOfFields++)
			{
			CContactItemField* field = CContactItemField::NewL(iStorage[noOfFields], iFieldUid[noOfFields]);
			CleanupStack::PushL(field);
			field->SetMapping(iVcardUid[noOfFields]);
			if(iStorage[noOfFields]==KStorageTypeDateTime)
				{
				TDateTime date(iYear,(TMonth)iMonth,iDay,0,0,0,0);
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
		id = dBase->AddNewContactL(*item);
		
		//Append the id into array
		idArray.Append(id);
		readPos=0;
		
		// Cleanup
		CleanupStack::PopAndDestroy(item);
    	}
    dBase->CloseContactL(id);

    // verify for multiple contact item with all fields is being added
    readPos=0;
    CContactItem *readItem = NULL;
    for(TInt readContact=0;readContact<iNoOfCI;readContact++)
    	{
    	readItem = dBase->ReadContactLC(idArray[readContact]);
    	for(TInt check=0;check<7;check++)
    		{
    		CContactItemFieldSet& ContactFieldSet = readItem->CardFields();
			TInt pos = ContactFieldSet.Find(iFieldUid[check], iVcardUid[check]);
			if(pos != KErrNotFound)
				{
				if(iStorage[check]==KStorageTypeDateTime)
					{
					TDateTime result=ContactFieldSet[pos].DateTimeStorage()->Time().DateTime();
					//compare the results
					if((result.Year() == iYear) && (result.Month() == (TMonth)iMonth) && (result.Day() == iDay))
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
    	readPos=0;
    	CleanupStack::PopAndDestroy(readItem);	
    	}
 	CleanupStack::PopAndDestroy(&readValues);
    GetStringFromConfig(ConfigSection(), KIniFlagvalue, temp);
    if(KErrNone == temp.Compare(KUpdate))	
		{
    	 UpdatePerformanceL();
		}
	else if(KErrNone == temp.Compare(KDelete))
		{
		DeletePerformanceL();
		}	
	CleanupStack::PopAndDestroy(&idArray);	
  	delete dBase;
	}
TVerdict CTestContactPerformanceStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CActiveScheduler::Install(NULL);
	delete iScheduler;
	return TestStepResult();
	}
/**
 * updating the 1000 contact item 
 * with all new fields
 * @return - void
 */
void CTestContactPerformanceStep::UpdatePerformanceL()
	{
	TPtrC value, temp, updateVal, getUpdateValue;
	
	// Get the string value from ini file for update
   	GetStringFromConfig(ConfigSection(), KIniUpdatedvalue, updateVal);
   	
   	//Get the int value from ini file for update
	GetIntFromConfig(ConfigSection(), KIniUpdateYear, iYear);
    GetIntFromConfig(ConfigSection(), KIniUpdateMonth, iMonth);
    GetIntFromConfig(ConfigSection(), KIniUpdateDay, iDay);
    
    //Get the number of contact item from ini file
	GetIntFromConfig(ConfigSection(), KNoOfContactItem, iNoOfCI);
       	
    // update multiple contact item field value

    CContactItem *readItem = NULL;
    for(TInt readContact=0;readContact<iNoOfCI;readContact++)
    	{
    	readItem = dBase->ReadContactLC(idArray[readContact]);
    	CContactItemFieldSet& ContactFieldSet = readItem->CardFields();
    	for(TInt update=0;update<7;update++)
    		{
			TInt pos = ContactFieldSet.Find(iFieldUid[update], iVcardUid[update]);
			if(pos != KErrNotFound)
				{
				if(iStorage[update]==KStorageTypeDateTime)
					{
					// update the field value
					TDateTime date(iYear,(TMonth)iMonth,iDay,0,0,0,0);
   					ContactFieldSet[pos].DateTimeStorage()->SetTime(date);
   					ContactFieldSet.UpdateFieldL(ContactFieldSet[pos],0);
   				
					// verify the updated field value
   					TInt upadtePos = ContactFieldSet.Find(iFieldUid[update], iVcardUid[update]);
   					TDateTime result=ContactFieldSet[upadtePos].DateTimeStorage()->Time().DateTime();
   					if((result.Year() == iYear) && (result.Month() == (TMonth)iMonth) && (result.Day() == iDay)) 
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
					ContactFieldSet.UpdateFieldL(ContactFieldSet[pos],0);
		
					// verify the field is being updated 
					TInt upadtePos = ContactFieldSet.Find(iFieldUid[update], iVcardUid[update]);
					getUpdateValue.Set(ContactFieldSet[upadtePos].TextStorage()->Text());
	
					//set the results
					if(getUpdateValue.Compare(updateVal)==0 )
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
    	CleanupStack::PopAndDestroy(readItem);	
    	}	
	}
/**
 * Deleting the 1000 contact item 
 * with all new fields
 * @return - void
 */	
void CTestContactPerformanceStep::DeletePerformanceL()
	{
	// delete multiple contact item field 
    CContactItem *readItem = NULL;
   
    //Get the number of contact item from ini file
	GetIntFromConfig(ConfigSection(), KNoOfContactItem, iNoOfCI);
	
    for(TInt readContact=0;readContact<iNoOfCI;readContact++)
    	{
    	readItem = dBase->ReadContactLC(idArray[readContact]);
    	CContactItemFieldSet& ContactFieldSet = readItem->CardFields();
    	for(TInt del=0;del<7;del++)
    		{
			TInt pos = ContactFieldSet.Find(iFieldUid[del], iVcardUid[del]);
			if(pos != KErrNotFound)
				{
				ContactFieldSet.Remove(pos);
				SetTestStepResult(EPass);
				}
			else
				{
				ERR_PRINTF1(_L("field not deleted"));
				SetTestStepResult(EFail);
				}
    		}
    	CleanupStack::PopAndDestroy(readItem);	
    	}
    INFO_PRINTF1(_L("Fields deleted successfully"));	
	}
