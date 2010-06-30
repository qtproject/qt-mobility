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

 
#include "contactspbapexportutilityclass.h"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "cntdb_internal.h"
#endif

CContactsPBAPExportUtilityClass::~CContactsPBAPExportUtilityClass()
/**
 * Destructor
 */
	{
	}

CContactsPBAPExportUtilityClass::CContactsPBAPExportUtilityClass(CTestStep* atestStep)
:iBaseTestStep(atestStep)
/**
 * Constructor
 */
	{
	}

// Gets a reference to the base test step. This is needed to read from the ini file
CTestStep& CContactsPBAPExportUtilityClass::BaseTestStepReference()
	{
	return *iBaseTestStep;
	}
	
/** Exports a vCard with version 2.1 or 3.0
@param	adBase The database to export from
@param	aStandard The PBAP version to export 
@param aIdArray The contact item ID array to export from database
@param aWriteStream The write stream to write to
@param aContactFilter Filter
*/	
void CContactsPBAPExportUtilityClass::ExportvCardL(CContactDatabase* adBase, const TDesC& aStandard, CContactIdArray* aIdArray, RWriteStream& aWriteStream, TInt64 aContactFilter)
	{
	TVCardVersion version(EVCardUDEF);
	
	TBool enableTel = EFalse;
	BaseTestStepReference().GetBoolFromConfig(BaseTestStepReference().ConfigSection(), KTelProp, enableTel);			
	
	if(aStandard.Compare(KVersion21) == 0)
		{
		version = EPBAPVCard21;	
		}
	if(aStandard.Compare(KVersion30) == 0)
		{
		version = EPBAPVCard30;	
		}
	else
		{
		if(aStandard.Compare(KVersionStd21) == 0)
			{
			version = EVCard21;	
			adBase->ExportSelectedContactsL(TUid::Uid(KUidVCardConvDefaultImpl), *aIdArray, aWriteStream, 0, aContactFilter, this, version ,enableTel);		
			return;
			}
		}	
	adBase->ExportSelectedContactsL(TUid::Uid(KUidPBAPVCardConvImpl), *aIdArray, aWriteStream, 0, aContactFilter, this, version ,enableTel);
	}
	
/** Definition for the callback function from the interface
@param	aContactId The Contact item id, supplied by the production code
@param	aPropertyList The array of intra-properties to be added to the exported vCard 
*/	
void CContactsPBAPExportUtilityClass::AddIntraContactPropertiesL(const TContactItemId& /*aContactId*/, CArrayPtr<CParserProperty>* aPropertyList)
	{
	TPtrC intraPropName;
	TPtrC intraPropParamName;
	TPtrC intraPropDatatype;
	BaseTestStepReference().GetStringFromConfig(BaseTestStepReference().ConfigSection(), KIntraPropName, intraPropName);			
	BaseTestStepReference().GetStringFromConfig(BaseTestStepReference().ConfigSection(), KIntraPropDatatype, intraPropDatatype);			
	BaseTestStepReference().GetStringFromConfig(BaseTestStepReference().ConfigSection(), KIntraPropParName, intraPropParamName);			
	
	RArray<TPtrC>	intraPropNameList;
	CleanupClosePushL(intraPropNameList);
	TokenizeStringL(intraPropName, intraPropNameList);

	RArray<TPtrC> intraPropDatatypeList;
	CleanupClosePushL(intraPropDatatypeList);
	TokenizeStringL(intraPropDatatype, intraPropDatatypeList);
	
	RArray<TPtrC> intraPropParamNameList;
	CleanupClosePushL(intraPropParamNameList);
	TokenizeStringL(intraPropParamName, intraPropParamNameList);
	
	const TInt dataTypeCount = intraPropDatatypeList.Count();
	
	CParserPropertyValue* propertyValue = NULL;
	
	for(TInt i=0;i < dataTypeCount;i++)
		{
		if(intraPropDatatypeList[i].Compare(KCDesCArray) == 0)
			{
			propertyValue = CreateCDesCArrayL();
			}
		
		if(intraPropDatatypeList[i].Compare(KDATETIME) == 0)
			{
			TTime date(KDateTimeVal);
			propertyValue = CreateDatePropertyL(date);   
			}
		
		if(intraPropDatatypeList[i].Compare(KHBufC) == 0)
			{
			propertyValue = CParserPropertyValueHBufC::NewL(KEmailVal);
			}
		
		if(intraPropDatatypeList[i].Compare(KBinary) == 0)
			{
			propertyValue = CParserPropertyValueBinary::NewL(KPhotoVal);
			}
	
		if(intraPropDatatypeList[i].Compare(KInt) == 0)
			{
			CParserPropertyValueInt* intVal = new(ELeave) CParserPropertyValueInt(143);
			propertyValue = intVal;
			}
		
		CParserParam* propParameter = CreateParameterL(intraPropParamNameList[i]);
		
		CleanupStack::PushL(propertyValue);
		AddPropertyL(intraPropNameList[i], propertyValue, propParameter, aPropertyList);
		CleanupStack::Pop();	// propertyValue
		}
		
	TBool leave = EFalse;
	BaseTestStepReference().GetBoolFromConfig(BaseTestStepReference().ConfigSection(), KLeave, leave);			
	
	if(leave)
		{
		User::Leave(KErrGeneral);
		}
			
	CleanupStack::PopAndDestroy(3, &intraPropNameList);
	
	}
	
/** Creates a property parameter
@param aParamName The paramter name
@return Property parameter
*/
CParserParam* CContactsPBAPExportUtilityClass::CreateParameterL(const TDesC& aParamName)
	{
	TBuf8<KMaxLengthField> paramName;
	paramName.Copy(aParamName);
		
	CParserParam* propParameter = CParserParam::NewL(paramName, KNullDesC8);	
	return propParameter;
	}

/** Creates a CDesCArray property
@return CDesCArray parser Property value
*/
CParserPropertyValue* CContactsPBAPExportUtilityClass::CreateCDesCArrayL()
	{
	CDesCArrayFlat* desArray = new (ELeave)CDesCArrayFlat(4);
	CleanupStack::PushL(desArray);
	desArray->AppendL(KAddr1);
	desArray->AppendL(KAddr2);
	desArray->AppendL(KAddr3);
	CParserPropertyValue* propertyValue = new (ELeave) CParserPropertyValueCDesCArray(desArray);
	CleanupStack::Pop();	// desArray
	return propertyValue;
	}

/** Utility method to create a Versit date property.
@param aDateTime Date (and Time) in Local Time. The only property that uses this field is BDAY, which should continue to be stored as MachineLocal time.
@return Versit date property corresponding to aDateTime which does not contain a time
*/	
CParserPropertyValueDate* CContactsPBAPExportUtilityClass::CreateDatePropertyL(const TTime& aDateTime)	
	{
	TVersitDateTime* dateTime = new(ELeave)TVersitDateTime(aDateTime.DateTime(), TVersitDateTime::EIsMachineLocal); 
	CleanupStack::PushL(dateTime);
	CParserPropertyValueDate* dateValue = new(ELeave) CParserPropertyValueDate(dateTime);
	CleanupStack::Pop(dateTime);	
	return dateValue;
	}

/** Appends the property to the array in AddIntraContactPropertiesL
@param	aPropertyName The property name to append
@param	aPropertyValue Value of the property
@param  aPropertyList The property list array in AddIntraContactPropertiesL to append  
*/	
void CContactsPBAPExportUtilityClass::AddPropertyL(const TDesC& aPropertyName, CParserPropertyValue* aPropertyValue, CParserParam* aPropParameter, CArrayPtr<CParserProperty>* aPropertyList) 
	{
	TBuf8<KMaxLengthField> propertyName;
	propertyName.Copy(aPropertyName);
	CParserProperty* property = CParserProperty::NewL(*aPropertyValue,propertyName,NULL);
	CleanupStack::PushL(property);
	if(aPropParameter)
		{
		property->AddParamL(aPropParameter);
		}
	aPropertyList->AppendL(property);
	CleanupStack::Pop();
	}

/** Parses a comma separated string and constructs a list out of the values
@param	aString The string to be tokenized
@param	aList Output list containing the tokens
@param	aSeparator Separator to recognize the tokens
*/
void CContactsPBAPExportUtilityClass::TokenizeStringL(const TDesC& aString, RArray<TPtrC>& aList, TChar aSeparator)
	{
	TLex lexer(aString);

	while(!lexer.Eos())
		{
		lexer.SkipSpaceAndMark();

		while(!lexer.Eos() && lexer.Get() != aSeparator)
			{
			}

		if(!lexer.Eos())
			{
			lexer.UnGet(); // Do not include trailing ','
			}
		aList.AppendL(lexer.MarkedToken());
		lexer.Inc();
		}
	}
	


