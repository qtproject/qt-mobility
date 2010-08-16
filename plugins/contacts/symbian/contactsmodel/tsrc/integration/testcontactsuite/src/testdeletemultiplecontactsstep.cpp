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


#include "testdeletemultiplecontactsstep.h"
#include "testcontactsuiteserver.h"
#include <cntitem.h>
#include <cntfield.h>
#include <cntfldst.h>
#include <cntdef.h>
#include <cntdb.h>
#include <commdb.h>


TInt CTestDeleteMultipleContactsStep::iStorage[]=
{
KStorageTypeText, KStorageTypeText, KStorageTypeDateTime, KStorageTypeText,KStorageTypeText,KStorageTypeText,KStorageTypeText
};

TUid CTestDeleteMultipleContactsStep::iFieldUid[]=
{
KUidContactFieldAssistant, KUidContactFieldPhoneNumber, KUidContactFieldAnniversary,KUidContactFieldSpouse,KUidContactFieldChildren,KUidContactFieldClass,KUidContactFieldDepartmentName
};

TUid CTestDeleteMultipleContactsStep::iVcardUid[]=
{
KUidContactFieldVCardMapAssistant, KUidContactFieldVCardMapAssistantTel, KUidContactFieldVCardMapAnniversary, KUidContactFieldVCardMapSpouse,KUidContactFieldVCardMapChildren,KUidContactFieldVCardMapClass,KUidContactFieldVCardMapDepartment
};

CTestDeleteMultipleContactsStep::~CTestDeleteMultipleContactsStep()
/**
 * Destructor
 */
	{

	}

CTestDeleteMultipleContactsStep::CTestDeleteMultipleContactsStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KTestDeleteMultipleContactsStep);
	}

TVerdict CTestDeleteMultipleContactsStep::doTestStepPreambleL()
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

TVerdict CTestDeleteMultipleContactsStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
	TPtrC fieldName,fieldVal, fieldMultiple;
 if (!GetStringFromConfig(ConfigSection(), KIniFieldName, fieldName))
		{
		ERR_PRINTF1(_L("Unable to read fieldname from ini file"));
		SetTestStepResult(EFail);
		}
	else
		{
		if(KErrNone == fieldName.Compare(KAssistance))
			{
			//Get the Field value
			GetStringFromConfig(ConfigSection(), KIniFieldVal, fieldVal);
			CreateAndDeleteMultipleContactsL(fieldVal, iStorage[0],iFieldUid[0], iVcardUid[0]);
			}
		else if(KErrNone == fieldName.Compare(KAssistanceTel))
			{
			//Get the Field value
			GetStringFromConfig(ConfigSection(), KIniFieldVal, fieldVal);
			CreateAndDeleteMultipleContactsL(fieldVal, iStorage[1],iFieldUid[1], iVcardUid[1]);
			}
		else if(KErrNone == fieldName.Compare(KAnniversary))
			{
			//Get the Field value
			GetStringFromConfig(ConfigSection(), KIniFieldVal, fieldVal);
			CreateAndDeleteMultipleContactsL(fieldVal, iStorage[2],iFieldUid[2], iVcardUid[2]);
			}
		else if(KErrNone == fieldName.Compare(KSpouse))
			{
			//Get the Field value
			GetStringFromConfig(ConfigSection(), KIniFieldVal, fieldVal);
			CreateAndDeleteMultipleContactsL(fieldVal, iStorage[3],iFieldUid[3], iVcardUid[3]);
			}
		else if(KErrNone == fieldName.Compare(KChildren))
			{
			//Get the Field value
			GetStringFromConfig(ConfigSection(), KIniFieldVal, fieldVal);
			CreateAndDeleteMultipleContactsL(fieldVal, iStorage[4],iFieldUid[4], iVcardUid[4]);
			}
		else if(KErrNone == fieldName.Compare(KClass))
			{
			//Get the Field value
			GetStringFromConfig(ConfigSection(), KIniFieldVal, fieldVal);
			CreateAndDeleteMultipleContactsL(fieldVal, iStorage[5],iFieldUid[5], iVcardUid[5]);
			}
		else if(KErrNone == fieldName.Compare(KDepartment))
			{
			//Get the Field value
			GetStringFromConfig(ConfigSection(), KIniFieldVal, fieldVal);
			CreateAndDeleteMultipleContactsL(fieldVal, iStorage[6],iFieldUid[6], iVcardUid[6]);
			}
		else
			{
			ERR_PRINTF1(_L("Invalid fieldname"));
			SetTestStepResult(EFail);
			}
		}
		return TestStepResult();

	}
/** Create Multiple Contact Item with field and Add all contact
 *	Item to Contact database then Delete the multiple contact item field. 
 *	aValue - field value
 *  @param aStorageType - Storage type of each field
 *	@param aContFieldUid - Uid of Contact field
 *	@param avCardMapUid - Uid of vCard 
 *	@return - void
 */
void CTestDeleteMultipleContactsStep::CreateAndDeleteMultipleContactsL(TPtrC aValue, TInt aStorageType,TUid aContFieldUid, TUid avCardMap)
	{
	TPtrC value, updateVal, getUpdateValue;
	CContactDatabase *dBase;
   _LIT(KTestDbName, "c:contactmultiple.cdb");
   	TInt year, month, day,noOfCI;
   	TContactItemId id = 0;
   	RArray <TInt> idArray;
	CleanupClosePushL(idArray);

	//replace existing database name
	dBase = CContactDatabase::ReplaceL(KTestDbName);

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
		id = dBase->AddNewContactL(*item);
		idArray.Append(id);
		CleanupStack::PopAndDestroy(item);
    	}
    dBase->CloseContactL(id);

    // delete multiple contact item field 
    CContactItem *readItem = NULL;
    for(TInt readContact=0;readContact<noOfCI;readContact++)
    	{
    	readItem = dBase->ReadContactLC(idArray[readContact]);
    	CContactItemFieldSet& ContactFieldSet = readItem->CardFields();
		TInt pos = ContactFieldSet.Find(aContFieldUid, avCardMap);
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
		CleanupStack::PopAndDestroy(readItem);	
    	}
    INFO_PRINTF1(_L("Field deleted successfully"));	
    
 	//cleanup
    CleanupStack::PopAndDestroy(&idArray);
  	delete dBase;
	}
TVerdict CTestDeleteMultipleContactsStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CActiveScheduler::Install(NULL);
	delete iScheduler;
	return TestStepResult();
	}


