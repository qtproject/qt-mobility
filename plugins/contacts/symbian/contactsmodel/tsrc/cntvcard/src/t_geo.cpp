/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* CR 1320 - REQ 8766
* Test harness for testing the import and export behaviour for the vCard GEO property.
* 1) Check that data can be added programatically
* 2) Check that the exported format is as expected
* 3) Check that vCard can be imported and the data is as expected.
* GEO property has the form of 
* GEO:[lat],[lon]
* The latitude and longitude can have upto 6 decimal places, there is also a range from -90 to 90 for the latitude 
* and -180 to 180 for the longitude, however there isn't any validation done. Invalid information is imported and 
* exported as it is.
* Include Files  
*
*/


#include <e32base.h>
#include <e32std.h>
#include <e32test.h>
#include "t_utils.h"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "cntdb_internal.h"
#endif


//  Global Variables
LOCAL_D CCntTest* CntTest=NULL;
LOCAL_D const TPtrC KDatabaseFileName=_L("C:vgeo.cdb");
LOCAL_D RTest test(_L("T_VGEO"));
LOCAL_D RFs fileSession;
const TInt KNumOfGEOTest = 6;

_LIT(KGEOVCardPath,"c:\\ExportGEO.vcf");

_LIT(KGEOStandard,"-45.0,122.0");
_LIT(KGEOMaxDecimalPlaces,"-45.123456,-122.123456");
_LIT(KGEONonRealNum,"ABCDEFG,HIJKLMNOP");
_LIT(KGEOOutOfRange,"1222.000,3253.0000");
_LIT(KGEOSingleLonValue,",23.00");
_LIT(KGEOSingleLatValue,"-86.00");

const TPtrC KGeoValues[KNumOfGEOTest] =
	{
		KGEOStandard (),
		KGEOMaxDecimalPlaces (),
		KGEONonRealNum (),
		KGEOOutOfRange (),
		KGEOSingleLonValue (),
		KGEOSingleLatValue ()
	};

//Verify the vCard. Check that the pattern is present and the number of time the pattern should be present.
TBool CheckExportedFileL(const TDesC& aVCardPath, const TDesC8& aPattern, TInt aCount = 1)
	{
	TInt counter = 0;
	RFileReadStream fileReadStream;
	CleanupClosePushL(fileReadStream);
    User::LeaveIfError(fileReadStream.Open(fileSession, aVCardPath, EFileRead));
    
    TBuf8<0x80> bufO;
    TInt err = KErrNone;
    do {
	    TRAP(err, fileReadStream.ReadL(bufO, TChar(0xa)));
	    if (err != KErrNone && err != KErrEof)
		    {
		    User::Leave(err);	
		    }
	    if (bufO.Find(aPattern)!= KErrNotFound)
	    	{
	    	++counter;
	    	}
	    } while (err != KErrEof);
    CleanupStack::PopAndDestroy(&fileReadStream);
    return (counter == aCount);
	}

//  Local Functions
LOCAL_C TContactItemId AddContactToDbL(const TDesC& aGeoValue)
	{
	_LIT(KForename,"John"); 
    _LIT(KSurname,"Smith"); 
    _LIT(KPhoneNumber,"+441617779700"); 
    
    // Create a  contact card to contain the data
    CContactCard* newCard = CContactCard::NewLC();
    
    // Create the firstName field and add the data to it
    CContactItemField* firstName = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldGivenName);
    firstName->TextStorage()->SetTextL(KForename);
    newCard->AddFieldL(*firstName);
    CleanupStack::Pop(firstName);
      
    // Create the lastName field and add the data to it
    CContactItemField* lastName= CContactItemField::NewLC(KStorageTypeText, KUidContactFieldFamilyName);
    lastName->TextStorage()->SetTextL(KSurname);
    newCard->AddFieldL(*lastName);
    CleanupStack::Pop(lastName);
      
    // Create a Geo field and add the geo value to it
    CContactItemField* geoLoc = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldGEO);
    geoLoc->SetMapping(KUidContactFieldVCardMapGEO);
    geoLoc->TextStorage()->SetTextL(aGeoValue);
    newCard->AddFieldL(*geoLoc);
    CleanupStack::Pop(geoLoc);
        
    // Create the phoneNo field and add the data to it
    CContactItemField* phoneNumber = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldPhoneNumber);
    phoneNumber->SetMapping(KUidContactFieldVCardMapTEL);
    phoneNumber->TextStorage()->SetTextL(KPhoneNumber);
    newCard->AddFieldL(*phoneNumber);
    CleanupStack::Pop(phoneNumber);
    
    // Add newCard to the database
    const TContactItemId contactId = CntTest->Db()->AddNewContactL(*newCard);
    CleanupStack::PopAndDestroy(newCard);
    return contactId;
	}

LOCAL_C void ExportVCardL(const TDesC& aVCardPath, CContactDatabase* aContactDB, const CContactIdArray& aContactIdArray, TVCardVersion aVer = EVCard21)
	{
	RFileWriteStream writeStream;
	CleanupClosePushL(writeStream);
    User::LeaveIfError(writeStream.Replace(fileSession, aVCardPath, EFileWrite));
    if (aVer == EVCard21)
    	{
        aContactDB->ExportSelectedContactsL(TUid::Uid(KUidVCardConvDefaultImpl), aContactIdArray, writeStream, 0);
    	}
    else
    	{
    	aContactDB->ExportSelectedContactsL(TUid::Uid(KUidPBAPVCardConvImpl), aContactIdArray, writeStream, CContactDatabase::EDefault, 0, NULL, aVer, ETrue);
    	}
    CleanupStack::PopAndDestroy(&writeStream);
	}

LOCAL_C CArrayPtr<CContactItem>* ImportVCardL(const TDesC& aVCardPath, CContactDatabase* aContactDB)
	{
	TBool success=EFalse;
	RFileReadStream fileReadStream;
	CleanupClosePushL(fileReadStream);
	User::LeaveIfError(fileReadStream.Open(fileSession, aVCardPath, EFileRead));
	CArrayPtr<CContactItem>* contactItems = aContactDB->ImportContactsL(TUid::Uid(KUidVCardConvDefaultImpl),
			fileReadStream, success, CContactDatabase::EImportSingleContact+CContactDatabase::ETTFormat);
	CleanupStack::PopAndDestroy(&fileReadStream);
	test (success);
	return contactItems;
	}

LOCAL_C TBool DoExportTestL( const TDesC& aGeoValue)
	{
	TBuf8<255> geoValueBuf8;
	geoValueBuf8.Copy(aGeoValue);
	CntTest->CreateDatabaseL();
	
	TContactItemId contactId = AddContactToDbL(aGeoValue);
	CContactIdArray* contactIdArray =  CContactIdArray::NewLC();
    contactIdArray->AddL(contactId);
	ExportVCardL(KGEOVCardPath, CntTest->Db(), *contactIdArray);//Exporting the contact
	CleanupStack::PopAndDestroy(contactIdArray);
	
    TBool success = CheckExportedFileL(KGEOVCardPath, geoValueBuf8);
    test (success);
    CntTest->CloseDatabase();
    return success;
	}

LOCAL_C TBool TestImportedContact(const CContactItem& aContactItem, const TDesC& aGeoValue)
	{
	TBool success = EFalse;
	const CContactItemFieldSet& contactFieldSet = aContactItem.CardFields();
	TInt pos = contactFieldSet.Find(KUidContactFieldGEO, KUidContactFieldVCardMapGEO);
	if (pos != KErrNotFound)
		{
		const CContactTextField*  contactTextField = contactFieldSet[pos].TextStorage();
		success = (contactTextField->Text().Compare(aGeoValue) == 0);
		test (success);
		}
	 return success;
	}

LOCAL_C TBool DoImportTestL(const TDesC& aGeoValue)
	{
	//Reset the Db
	CntTest->DeleteDatabaseL();
	CntTest->CreateDatabaseL();
	
	CArrayPtr<CContactItem>* contactItems = ImportVCardL(KGEOVCardPath, CntTest->Db());
	CleanupStack::PushL(TCleanupItem(CleanUpResetAndDestroy,contactItems));
	test (contactItems->Count() == 1);
	CContactItem* contactItem = contactItems->At(0);
    TBool success =  TestImportedContact(*contactItem, aGeoValue);
	test (success);
	CleanupStack::PopAndDestroy();
	return success;
	}
/**
@SYMREQ 8766
@SYMTestType CIT
@SYMTestPriority Critical
@SYMTestCaseDesc This function verifies that the GEO field can be added and its text can contain the
                 GEO values of latitude and longitude. This tests all the import and export functionality of
                 the testcase ids of GEO-1 to GEO-12
@SYMTestActions Create a contact item and export the item to a vCard, the exported item is verified.
                 The vCard is imported and the contact item is verified.

@SYMTestExpectedResults 
The vCard exported should have the following line.
GEO:[lat],[lon]
Different [lat] and [lon] values are tested.
*/


LOCAL_C void TestGeoL(const TDesC& aGeoValue)
	{
	test.Next(_L(""));

	
    test(DoExportTestL(aGeoValue));
    
    test(DoImportTestL(aGeoValue));
	}

/**

@SYMTestCaseID     PIM-T-GEO-0001

*/

LOCAL_C void RunTestsL()
	{
	test.Start(_L("@SYMTESTCaseID:PIM-T-GEO-0001 T_VGEO"));

	CleanupClosePushL(fileSession);
    User::LeaveIfError(fileSession.Connect());
	
	for (TInt var = 0; var < KNumOfGEOTest; ++var)
		{
		TestGeoL(KGeoValues[var]);
		}
	fileSession.Delete(KGEOVCardPath()); 
	CleanupStack::PopAndDestroy(&fileSession); // fileSession
	}

//  Global Functions
GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CntTest = new CCntTest;
	if ( CntTest)
		{
		//ActiveScheduler is created within CntTest
		TRAPD(err, CntTest->ConstructL(test,KDatabaseFileName));
		test(err==KErrNone);
		// Run tests inside TRAP harness
		TRAPD(mainError, RunTestsL());
		test(mainError==KErrNone);
		CntTest->EndTestLib (mainError);
		}

	__UHEAP_MARKEND;
	return 0;
	}

