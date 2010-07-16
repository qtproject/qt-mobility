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


#include "testcomparecntfiles.h"

// Class CTestCompareCntFiles - Opens import/export vcf and carries out the verification of the properties
CTestCompareCntFiles::CTestCompareCntFiles()
	{
	SetTestStepName(KTestCompareCntFiles);
	}

CTestCompareCntFiles::~CTestCompareCntFiles()
	{
	delete iExportObj;
	}


TVerdict CTestCompareCntFiles::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Start verifcation of import/export vcf files"));

	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTestCompareCntFiles::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
	// connect to file system
	User::LeaveIfError(iFsSession.Connect());

	TESTL(TestStepResult() == EPass);
	INFO_PRINTF1(_L("CTestCompareCntFiles::doTestStepL() start"));

	TESTL(GetStringFromConfig(ConfigSection(), KExportFile, iExportFilePath));
	
	GetStringFromConfig(ConfigSection(), KExpectedExportFile, iExpectedExportFilePath);
	GetStringFromConfig(ConfigSection(), KFieldToVerify, iFieldToVerify);
	GetStringFromConfig(ConfigSection(), KNoField, iNoField);
	
	// Create Utility class object, to use TokenizeStringL
	CTestStep* self = static_cast<CTestStep*>(this);
	iExportObj = new(ELeave) CContactsPBAPExportUtilityClass(self);
	
	TokenizeAndCompareL();
	TokenizeAndCheckNoFieldL();
	
	// Comparing two files
	TBool compareFile = EFalse;
	GetBoolFromConfig(ConfigSection(), KCompareFile, compareFile);
	
	if(compareFile)
		{
		TBool fileCompare = EFalse;
		// Call CompareWholeFileL to use the normal comparison of two files, ignoring the REV property
		fileCompare = CompareWholeFileL(iExpectedExportFilePath, iExportFilePath);
		if(!fileCompare)
			{
			SetTestStepResult(EFail);
			}
		}
	
	iFsSession.Close();
    
	INFO_PRINTF1(_L("CTestCompareCntFiles::doTestStepL() finish"));
	
	return TestStepResult();
	
	}

/** Compares 2 vcf fie
@param	aExpectedFile The expected vcf file
@param	aFile This is the vcf file produced, and will be compared against aExpectedFile
@param	aProperty This is the property we are interested in 
*/
void CTestCompareCntFiles::CompareFileL(const TDesC& aExpectedFile, const TDesC& aFile, TDes8& aProperty)
	{
	RFile fileExpected;
	RFile fileOutput;
	CleanupClosePushL(fileExpected);
	CleanupClosePushL(fileOutput);
	
	TInt err = KErrNone;
	err = fileExpected.Open(iFsSession, aExpectedFile, EFileRead);

	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Unable to open file: %S"), &aExpectedFile);
		SetTestStepResult(EFail);
		SetTestStepError(err);
		}
	err = fileOutput.Open(iFsSession, aFile, EFileRead);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Unable to open file: %S"), &aFile);
		SetTestStepResult(EFail);
		SetTestStepError(err);
		}

	RFileReadStream streamExpected(fileExpected);
	RFileReadStream streamOutput(fileOutput);
	CleanupClosePushL(streamExpected);
	CleanupClosePushL(streamOutput);

	TBuf8<0x80> bufExpected,bufOutput;
	TBool isProperty = EFalse;
	TBool isPropertyExpected = EFalse;
	TBool foundBothProperty = EFalse;
	TBool flag = EFalse;
	
	TInt err1 = KErrNone;
	
	do  // This do loop iterates through both files, until the end of file is found
		{
		do // This do loop iterates thorugh both files until both properties are found and compares them
			{
			foundBothProperty = EFalse;
			if (!isProperty)
				{
				TRAP(err, streamExpected.ReadL(bufExpected, KLinefeedChar));
				}

			if (!isPropertyExpected)
				{
				TRAP(err1, streamOutput.ReadL(bufOutput, KLinefeedChar));
				}

			if (err != KErrEof || err1 != KErrEof)
				{
				isProperty = IsPropertyPresent(bufExpected, aProperty); // checks if aProperty
				if(isProperty && err1 == KErrEof)
					{
					break;	
					}
				isPropertyExpected = IsPropertyPresent(bufOutput, aProperty); // checks if aProperty
				}
			else
				{
				break;
				}
			if ((isProperty) && (isPropertyExpected))
				{
				foundBothProperty = ETrue;
				flag = ETrue;
				}
			} while (!foundBothProperty); // exit 2nd do loop when both properties found

		// Exists 2nd do loop, so both properties are found
		if (err != KErrEof && err1 != KErrEof)
			{
			if(aProperty != KRevision)
				{
				CompareLinesL(bufExpected, bufOutput); // compares both properties	
				}
			else 
				{
				break;
				}
			}

		// After comparing, gets the next property parameters in vcs file, if we have not reached end of file
		TRAP(err, streamExpected.ReadL(bufExpected, KLinefeedChar));
		TRAP(err1, streamOutput.ReadL(bufOutput, KLinefeedChar));
		} while (err != KErrEof || err1 != KErrEof); // exits 2nd do loop when its reached the end of either file

	TBuf<KMaxLengthField> buf;
	buf.Copy(aProperty);
	TPtrC ptr(buf);
	
	if(flag)
		{
		INFO_PRINTF2(_L("Property: %S Exported properly"), &ptr);	
		}
	else
		{
		INFO_PRINTF2(_L("Property: %S not exported"), &ptr);
		SetTestStepResult(EFail);
		}
	CleanupStack::PopAndDestroy(4, &fileExpected);
	
	}
	
/** Checks if the line of data contains the property (aProperty) parameter
@param	aLine The line of data to check
@return	True or False indicating if property found in the line of data
*/
TBool CTestCompareCntFiles::IsPropertyPresent(const TDesC8& aLine, TDes8& aProperty)
	{
	TInt propertyLength = aProperty.Length();
	TBuf8<KMaxLengthField> propertyFromLine(aLine.Left(propertyLength));

	if (!propertyFromLine.Compare(aProperty))
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}
	}


/** Compares two lines of data
@param	aBufExpected The line of data to compare with aBufOutput
@param	aBufOutput The second line of data to compare to aBufExpected
*/
void CTestCompareCntFiles::CompareLinesL(const TDesC8& aBufExpected, const TDesC8& aBufOutput)
	{
	TESTL(!aBufExpected.Compare(aBufOutput));	
	}

/** Checks for any field that should have not been exported
@param	aFile The vcs file to open
*/
TBool CTestCompareCntFiles::CheckForNoFieldL(const TDesC& aFile, const TDesC& aProperty)
	{
	RFile fileOutput;
	CleanupClosePushL(fileOutput);

	TInt err = fileOutput.Open(iFsSession, aFile, EFileRead);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Unable to open file: %S"), &aFile);
		SetTestStepResult(EFail);
		SetTestStepError(err);
		}

	RFileReadStream streamOutput(fileOutput);
	CleanupClosePushL(streamOutput);

	TBuf8<0x80> bufOutput;
	
	TBool result;
	TBuf8<KMaxLengthField> noField;
	noField.Copy(aProperty);

	do
		{
		TRAP(err, streamOutput.ReadL(bufOutput, TChar(KLinefeedChar)));
		
		result = IsPropertyPresent(bufOutput, noField);
		
		if(result)
			{
			break;	
			}
		} while (err != KErrEof);

	CleanupStack::PopAndDestroy(2, &fileOutput);
	
	if(result)
		{
		return EFalse;	
		}
	else
		{
		return ETrue;
		}
	}

// Tokenize the property list taken from ini file and compare each of them with the expected one
void CTestCompareCntFiles::TokenizeAndCompareL()
	{
	RArray<TPtrC>	fieldList;
	CleanupClosePushL(fieldList);
	iExportObj->TokenizeStringL(iFieldToVerify, fieldList);
	
	TInt count = fieldList.Count();
	TBuf8<KMaxLengthField> property;
	
	for(TInt i = 0;i < count;i++)
		{	
		property.Copy(fieldList[i]);
		CompareFileL(iExpectedExportFilePath, iExportFilePath, property);
		}
	CleanupStack::PopAndDestroy();
	fieldList.Close();
	}

// Tokenize the property list taken from ini file and check for non-existence of these properties in the exported file.
void CTestCompareCntFiles::TokenizeAndCheckNoFieldL()
	{
	TBool result = EFalse;
	RArray<TPtrC>	fieldList;
	CleanupClosePushL(fieldList);
	iExportObj->TokenizeStringL(iNoField, fieldList);
	TInt count = fieldList.Count();
	
	for(TInt i = 0;i < count;i++)
		{	
		result = CheckForNoFieldL(iExportFilePath, fieldList[i]);
		
		if(result)
			{
			INFO_PRINTF1(_L("Success: Property not present in vCard"));
			}
		else
			{
			INFO_PRINTF1(_L("Error: Property present in vCard"));
			SetTestStepResult(EFail);
			break;
			}
		}
	CleanupStack::PopAndDestroy();
	fieldList.Close();	
	}

/** Compares two files, excluding the REV property, as it doesn't remain the same always
@param	aExpectedFile The expected vcf file
@param	aFile This is the vcf file produced, and will be compared against aExpectedFile
*/
TBool CTestCompareCntFiles::CompareWholeFileL(const TDesC& aExpectedFile, const TDesC& aFile)
	{
	User::LeaveIfError(iFsSession.Connect());
	CleanupClosePushL(iFsSession);
	
	RFile fileExpected;
	RFile fileOutput;
	CleanupClosePushL(fileExpected);
	CleanupClosePushL(fileOutput);
	
	TInt err;
	err = fileExpected.Open(iFsSession, aExpectedFile, EFileRead);

	if (err != KErrNone)
		{
		User::Leave(err);
		}

	err = fileOutput.Open(iFsSession, aFile, EFileWrite);
	if (err != KErrNone)
		{
		User::Leave(err);
		}

	RFileReadStream stream1(fileExpected);
	RFileReadStream stream2(fileOutput);
	CleanupClosePushL(stream1);
	CleanupClosePushL(stream2);
	TBuf8<0x80> bufO,bufC;
	TBool flag = ETrue;
	TInt line = 1;
	do
		{
		TRAP(err, stream1.ReadL(bufO, KLinefeedChar));
		if (err == KErrNone || err == KErrEof)
			TRAP(err, stream2.ReadL(bufC, KLinefeedChar));
		if (err != KErrNone && err != KErrEof)
			User::Leave(err);
		if (CompareLine(bufO, bufC) == EFalse)
			{
			flag = EFalse;
			break;
			}
		++line;
		}
	while (err != KErrEof);

	CleanupStack::PopAndDestroy(4, &fileExpected);
	CleanupStack::PopAndDestroy(&iFsSession);
    iFsSession.Close();
	
	if(flag)
		{
		return ETrue;		
		}
	else
		{
		return EFalse;
		}		
	}

/* Compare the corresponding lines of two files
@param	aLine1 Complete line of a file to compare
@param	aLine2 Corresponding line to compare in the second file
*/
TBool CTestCompareCntFiles::CompareLine(const TDesC8& aLine1, const TDesC8& aLine2)
	{
	_LIT8(KREV,"REV:");
	TBool ret = !aLine1.Compare(aLine2);
	if (!ret)
		{
		TBufC8<4> propertyRev(aLine1.Left(4));
		if(!propertyRev.Compare(KREV()))
			{
			return ETrue;	
			}
		}
	return ret;
	}


TVerdict CTestCompareCntFiles::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Completed verifcation of import/export vcf files"));
	return TestStepResult();
	}

