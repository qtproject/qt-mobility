/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Test for Wrong Field module
*
*/

#include <e32test.h>
#include <cntdef.h>
#include <cntfield.h>
#include <e32svr.h>
#include "t_utils.h"

_LIT(KTestName, "T_WRONGFIELDMATCH");
LOCAL_D RTest test(KTestName);

/*
class CBenchMarker
	{
public:
	static void StartProfile();
	static void EndProfile();
	};

	//User::LeaveIfError(iFs.Connect());
	//iLog=CLog::NewL(iTest,iFs,KLogFileName);

void CBenchMarker::StartProfile()
	{
	CCntTest::ProfileReset(0,1);
	CCntTest::ProfileStart(0);
	}

void CBenchMarker::EndProfile()
	{
	CCntTest::ProfileEnd(0);
	TCntProfile profile[1];
	CCntTest::ProfileResult(profile,0,1);
	_LIT(KProfileFormat,"Total %d.%03d sec (%d hits)\n");
	RDebug::Print(KProfileFormat,profile[0].iTime/1000000, profile[0].iTime%1000000, profile[0].iCount);
	//iLog->LogLine(KProfileFormat,profile[0].iTime/1000000, profile[0].iTime%1000000, profile[0].iCount);
	}*/


//KUidContactFieldPhoneNumber
//KUidContactFieldVCardMapTEL
//KUidContactFieldVCardMapVOICE

LOCAL_C void DoTestVoiceDialIgnoreL()
	{
	//Create a calypso general phone number content type
	CContentType* phoneNumber=CContentType::NewL(KUidContactFieldPhoneNumber,KUidContactFieldVCardMapTEL);
	CleanupStack::PushL(phoneNumber);
	phoneNumber->AddFieldTypeL(KUidContactFieldVCardMapVOICE);

	//Create a calypso general phone number content type
	CContentType* voiceDial=CContentType::NewL(KUidContactFieldPhoneNumber,KUidContactFieldVCardMapTEL);
	CleanupStack::PushL(voiceDial);
	voiceDial->AddFieldTypeL(KUidContactFieldVCardMapVOICE);
	voiceDial->AddFieldTypeL(KUidContactsVoiceDialField);
	
	if (!voiceDial->IsEqualForSyncUpdate(*phoneNumber))
		{
		User::Leave(KErrGeneral); 
		}
	
	//Behavior due to algorithm optimisation
	if (phoneNumber->IsEqualForSyncUpdate(*voiceDial))
		{
		User::Leave(KErrGeneral); 
		}

	CleanupStack::PopAndDestroy(2,phoneNumber);
	}





LOCAL_C void DoTestSpeedDialIgnoreL()
	{
	//Create a calypso general phone number content type
	CContentType* phoneNumber=CContentType::NewL(KUidContactFieldPhoneNumber,KUidContactFieldVCardMapTEL);
	CleanupStack::PushL(phoneNumber);
	phoneNumber->AddFieldTypeL(KUidContactFieldVCardMapVOICE);

	//Create a calypso general phone number content type
	CContentType* speedDial=CContentType::NewL(KUidContactFieldPhoneNumber,KUidContactFieldVCardMapTEL);
	CleanupStack::PushL(speedDial);
	speedDial->AddFieldTypeL(KUidContactFieldVCardMapVOICE);
	speedDial->AddFieldTypeL(KUidSpeedDialOne);

	///////////////////////////////////////////

	if (!speedDial->IsEqualForSyncUpdate(*phoneNumber))
		{
		User::Leave(KErrGeneral); 
		}
	//Behavior due to algorithm optimisation
	if (phoneNumber->IsEqualForSyncUpdate(*speedDial))
		{
		User::Leave(KErrGeneral); 
		}

	speedDial->RemoveFieldType(KUidSpeedDialOne);

	////////////////////////////////////////////

	speedDial->AddFieldTypeL(KUidSpeedDialTwo);

	if (!speedDial->IsEqualForSyncUpdate(*phoneNumber))
		{
		User::Leave(KErrGeneral); 
		}
	//Behavior due to algorithm optimisation
	if (phoneNumber->IsEqualForSyncUpdate(*speedDial))
		{
		User::Leave(KErrGeneral); 
		}

	speedDial->RemoveFieldType(KUidSpeedDialTwo);

	////////////////////////////////////////////

	speedDial->AddFieldTypeL(KUidSpeedDialThree);

	if (!speedDial->IsEqualForSyncUpdate(*phoneNumber))
		{
		User::Leave(KErrGeneral); 
		}
	//Behavior due to algorithm optimisation
	if (phoneNumber->IsEqualForSyncUpdate(*speedDial))
		{
		User::Leave(KErrGeneral); 
		}

	speedDial->RemoveFieldType(KUidSpeedDialThree);

	////////////////////////////////////////////

	speedDial->AddFieldTypeL(KUidSpeedDialFour);
	//Behavior due to algorithm optimisation
	if (!speedDial->IsEqualForSyncUpdate(*phoneNumber))
		{
		User::Leave(KErrGeneral); 
		}

	if (phoneNumber->IsEqualForSyncUpdate(*speedDial))
		{
		User::Leave(KErrGeneral); 
		}

	speedDial->RemoveFieldType(KUidSpeedDialFour);

	////////////////////////////////////////////

	speedDial->AddFieldTypeL(KUidSpeedDialFive);

	if (!speedDial->IsEqualForSyncUpdate(*phoneNumber))
		{
		User::Leave(KErrGeneral); 
		}
	//Behavior due to algorithm optimisation
	if (phoneNumber->IsEqualForSyncUpdate(*speedDial))
		{
		User::Leave(KErrGeneral); 
		}

	speedDial->RemoveFieldType(KUidSpeedDialFive);

	////////////////////////////////////////////
	
	speedDial->AddFieldTypeL(KUidSpeedDialSix);

	if (!speedDial->IsEqualForSyncUpdate(*phoneNumber))
		{
		User::Leave(KErrGeneral); 
		}
	//Behavior due to algorithm optimisation
	if (phoneNumber->IsEqualForSyncUpdate(*speedDial))
		{
		User::Leave(KErrGeneral); 
		}

	speedDial->RemoveFieldType(KUidSpeedDialSix);

	////////////////////////////////////////////
	
	speedDial->AddFieldTypeL(KUidSpeedDialSeven);

	if (!speedDial->IsEqualForSyncUpdate(*phoneNumber))
		{
		User::Leave(KErrGeneral); 
		}
	//Behavior due to algorithm optimisation
	if (phoneNumber->IsEqualForSyncUpdate(*speedDial))
		{
		User::Leave(KErrGeneral); 
		}

	speedDial->RemoveFieldType(KUidSpeedDialSeven);

	////////////////////////////////////////////

	speedDial->AddFieldTypeL(KUidSpeedDialEight);

	if (!speedDial->IsEqualForSyncUpdate(*phoneNumber))
		{
		User::Leave(KErrGeneral); 
		}
	//Behavior due to algorithm optimisation
	if (phoneNumber->IsEqualForSyncUpdate(*speedDial))
		{
		User::Leave(KErrGeneral); 
		}

	speedDial->RemoveFieldType(KUidSpeedDialEight);

	////////////////////////////////////////////

	speedDial->AddFieldTypeL(KUidSpeedDialNine);

	if (!speedDial->IsEqualForSyncUpdate(*phoneNumber))
		{
		User::Leave(KErrGeneral); 
		}
	//Behavior due to algorithm optimisation
	if (phoneNumber->IsEqualForSyncUpdate(*speedDial))
		{
		User::Leave(KErrGeneral); 
		}

	speedDial->RemoveFieldType(KUidSpeedDialNine);

	////////////////////////////////////////////

	CleanupStack::PopAndDestroy(2,phoneNumber); //type1, type2
	}



LOCAL_C void DoTestDifferentPhoneNumberL()
	{
	//Create a calypso general phone number content type
	CContentType* calypsoGeneralPhoneNumber=CContentType::NewL(KUidContactFieldPhoneNumber,KUidContactFieldVCardMapTEL);
	CleanupStack::PushL(calypsoGeneralPhoneNumber);
	calypsoGeneralPhoneNumber->AddFieldTypeL(KUidContactFieldVCardMapVOICE);
	
	//Create a calypso general mobile number content type
	CContentType* calypsoGeneralMobileNumber=CContentType::NewL(KUidContactFieldPhoneNumber,KUidContactFieldVCardMapTEL);
	CleanupStack::PushL(calypsoGeneralMobileNumber);
	calypsoGeneralMobileNumber->AddFieldTypeL(KUidContactFieldVCardMapVOICE);
	calypsoGeneralMobileNumber->AddFieldTypeL(KUidContactFieldVCardMapCELL);

	if (calypsoGeneralPhoneNumber->IsEqualForSyncUpdate(*calypsoGeneralMobileNumber))
		{
		User::Leave(KErrGeneral); //Leave if equal 
		}

	if (calypsoGeneralMobileNumber->IsEqualForSyncUpdate(*calypsoGeneralPhoneNumber))
		{
		User::Leave(KErrGeneral); //Leave if equal	
		}

	//Must be equal to itself
	if (!calypsoGeneralMobileNumber->IsEqualForSyncUpdate(*calypsoGeneralMobileNumber))
		{
		User::Leave(KErrGeneral); //Leave if not equal	
		}

	//Must be equal to itself
	if (!calypsoGeneralPhoneNumber->IsEqualForSyncUpdate(*calypsoGeneralPhoneNumber))
		{
		User::Leave(KErrGeneral); //Leave if not equal	
		}

	/////////Now test if the voice property exception does not create any side effect //////////

	calypsoGeneralMobileNumber->RemoveFieldType(KUidContactFieldVCardMapVOICE);

	if (calypsoGeneralPhoneNumber->IsEqualForSyncUpdate(*calypsoGeneralMobileNumber))
		{
		User::Leave(KErrGeneral); //Leave if equal 
		}

	if (calypsoGeneralMobileNumber->IsEqualForSyncUpdate(*calypsoGeneralPhoneNumber))
		{
		User::Leave(KErrGeneral); //Leave if equal	
		}

	//Must be equal to itself
	if (!calypsoGeneralMobileNumber->IsEqualForSyncUpdate(*calypsoGeneralMobileNumber))
		{
		User::Leave(KErrGeneral); //Leave if not equal	
		}

	////////////////////////////////////////////////////////////////////////////////
	
	/////////Now test if the pref property exception does not create any side effect //////////

	calypsoGeneralMobileNumber->AddFieldTypeL(KUidContactFieldVCardMapPREF);

	if (calypsoGeneralPhoneNumber->IsEqualForSyncUpdate(*calypsoGeneralMobileNumber))
		{
		User::Leave(KErrGeneral); //Leave if equal 
		}

	if (calypsoGeneralMobileNumber->IsEqualForSyncUpdate(*calypsoGeneralPhoneNumber))
		{
		User::Leave(KErrGeneral); //Leave if equal	
		}

	//Must be equal to itself
	if (!calypsoGeneralMobileNumber->IsEqualForSyncUpdate(*calypsoGeneralMobileNumber))
		{
		User::Leave(KErrGeneral); //Leave if not equal	
		}

	calypsoGeneralPhoneNumber->AddFieldTypeL(KUidContactFieldVCardMapPREF);

	if (calypsoGeneralPhoneNumber->IsEqualForSyncUpdate(*calypsoGeneralMobileNumber))
		{
		User::Leave(KErrGeneral); //Leave if equal 
		}

	if (calypsoGeneralMobileNumber->IsEqualForSyncUpdate(*calypsoGeneralPhoneNumber))
		{
		User::Leave(KErrGeneral); //Leave if equal	
		}

	//Must be equal to itself
	if (!calypsoGeneralPhoneNumber->IsEqualForSyncUpdate(*calypsoGeneralPhoneNumber))
		{
		User::Leave(KErrGeneral); //Leave if not equal	
		}

	////////////////////////////////////////////////////////////////////////////////
	
	CleanupStack::PopAndDestroy(2); //type1, type2
	}


LOCAL_C void DoTestEqualPhoneNumberL()
	{
	//Create a calypso general phone number content type
	CContentType* samePhoneNumber=CContentType::NewL(KUidContactFieldPhoneNumber,KUidContactFieldVCardMapTEL);
	CleanupStack::PushL(samePhoneNumber);
	samePhoneNumber->AddFieldTypeL(KUidContactFieldVCardMapVOICE);
	
	//Create a calypso general phone number content type
	CContentType* samePhoneNumberBis=CContentType::NewL(KUidContactFieldPhoneNumber,KUidContactFieldVCardMapTEL);
	CleanupStack::PushL(samePhoneNumberBis);
	samePhoneNumberBis->AddFieldTypeL(KUidContactFieldVCardMapVOICE);
	
	
	if (!samePhoneNumber->IsEqualForSyncUpdate(*samePhoneNumberBis))
		{
		User::Leave(KErrGeneral); //Leave if not equal	
		}


	if (!samePhoneNumberBis->IsEqualForSyncUpdate(*samePhoneNumber))
		{
		User::Leave(KErrGeneral); //Leave if not equal	
		}

	//////////////////Check voice property exception////////////////////

	samePhoneNumber->AddFieldTypeL(KUidContactFieldVCardMapPREF);

	if (!samePhoneNumber->IsEqualForSyncUpdate(*samePhoneNumberBis))
		{
		User::Leave(KErrGeneral); //Leave if not equal	
		}


	if (!samePhoneNumberBis->IsEqualForSyncUpdate(*samePhoneNumber))
		{
		User::Leave(KErrGeneral); //Leave if not equal	
		}

	samePhoneNumber->RemoveFieldType(KUidContactFieldVCardMapPREF);


	//////////////////Check pref property exception////////////////////

	samePhoneNumber->RemoveFieldType(KUidContactFieldVCardMapVOICE);

	if (!samePhoneNumber->IsEqualForSyncUpdate(*samePhoneNumberBis))
		{
		User::Leave(KErrGeneral); //Leave if not equal	
		}

	if (!samePhoneNumberBis->IsEqualForSyncUpdate(*samePhoneNumber))
		{
		User::Leave(KErrGeneral); //Leave if not equal	
		}

	samePhoneNumber->AddFieldTypeL(KUidContactFieldVCardMapVOICE);

	//////////////////Check pref and voice property exception together////////////////////
	
	samePhoneNumber->AddFieldTypeL(KUidContactFieldVCardMapPREF);
	samePhoneNumber->RemoveFieldType(KUidContactFieldVCardMapVOICE);


	if (!samePhoneNumber->IsEqualForSyncUpdate(*samePhoneNumberBis))
		{
		User::Leave(KErrGeneral); //Leave if not equal	
		}


	if (!samePhoneNumberBis->IsEqualForSyncUpdate(*samePhoneNumber))
		{
		User::Leave(KErrGeneral); //Leave if not equal	
		}

	samePhoneNumber->RemoveFieldType(KUidContactFieldVCardMapPREF);
	samePhoneNumber->AddFieldTypeL(KUidContactFieldVCardMapVOICE);


	//////////////////Check pref and voice property exception together when set in both content type////////////////////
	
	samePhoneNumber->AddFieldTypeL(KUidContactFieldVCardMapPREF);
	samePhoneNumber->RemoveFieldType(KUidContactFieldVCardMapVOICE);
	samePhoneNumberBis->AddFieldTypeL(KUidContactFieldVCardMapPREF);
	samePhoneNumberBis->RemoveFieldType(KUidContactFieldVCardMapVOICE);

	if (!samePhoneNumber->IsEqualForSyncUpdate(*samePhoneNumberBis))
		{
		User::Leave(KErrGeneral); //Leave if not equal	
		}


	if (!samePhoneNumberBis->IsEqualForSyncUpdate(*samePhoneNumber))
		{
		User::Leave(KErrGeneral); //Leave if not equal	
		}

	/////////////////////////////////////////////////////////////////////////////////////////


	CleanupStack::PopAndDestroy(2); //type1, type2
	}


LOCAL_C void DoTestDifferentEmailL()
	{
	//Create a calypso email general 
	CContentType* emailGeneral=CContentType::NewL(KUidContactFieldEMail,KUidContactFieldVCardMapEMAILINTERNET);
	CleanupStack::PushL(emailGeneral);
		
	//Create a calypso email work
	CContentType* emailWork=CContentType::NewL(KUidContactFieldEMail,KUidContactFieldVCardMapEMAILINTERNET);
	CleanupStack::PushL(emailWork);
	emailWork->AddFieldTypeL(KUidContactFieldVCardMapWORK);
	
	///////// Check  ///////////

	if (emailGeneral->IsEqualForSyncUpdate(*emailWork))
		{
		User::Leave(KErrGeneral); //Leave if equal	
		}

	if (emailWork->IsEqualForSyncUpdate(*emailGeneral))
		{
		User::Leave(KErrGeneral); //Leave if equal	
		}

	//Check if equal to itself
	if (!emailGeneral->IsEqualForSyncUpdate(*emailGeneral))
		{
		User::Leave(KErrGeneral); //Leave if not equal	
		}
	
	//Check if equal to itself
	if (!emailWork->IsEqualForSyncUpdate(*emailWork))
		{
		User::Leave(KErrGeneral); //Leave if equal	
		}

	///////// Check pref not create side effect ///////////

	emailWork->AddFieldTypeL(KUidContactFieldVCardMapPREF);

	if (emailGeneral->IsEqualForSyncUpdate(*emailWork))
		{
		User::Leave(KErrGeneral); //Leave if equal	
		}

	if (emailWork->IsEqualForSyncUpdate(*emailGeneral))
		{
		User::Leave(KErrGeneral); //Leave if equal	
		}

	//Check if equal to itself
	if (!emailWork->IsEqualForSyncUpdate(*emailWork))
		{
		User::Leave(KErrGeneral); //Leave if equal	
		}
	
	emailWork->RemoveFieldType(KUidContactFieldVCardMapPREF);

	CleanupStack::PopAndDestroy(2); //type1, type2
	}
	
LOCAL_C void DoTestEqualEmailL()
	{
	//Create a calypso email general 
	CContentType* sameEmail=CContentType::NewL(KUidContactFieldEMail,KUidContactFieldVCardMapEMAILINTERNET);
	CleanupStack::PushL(sameEmail);
		
	//Create a calypso email work
	CContentType* sameEmailBis=CContentType::NewL(KUidContactFieldEMail,KUidContactFieldVCardMapEMAILINTERNET);
	CleanupStack::PushL(sameEmailBis);


	if (!sameEmailBis->IsEqualForSyncUpdate(*sameEmail))
		{
		User::Leave(KErrGeneral); //Leave if not equal	
		}

	if (!sameEmail->IsEqualForSyncUpdate(*sameEmailBis))
		{
		User::Leave(KErrGeneral); //Leave if not equal	
		}


	///////// Check pref is ignore ///////////
	sameEmail->AddFieldTypeL(KUidContactFieldVCardMapPREF);

	if (!sameEmailBis->IsEqualForSyncUpdate(*sameEmail))
		{
		User::Leave(KErrGeneral); //Leave if not equal	
		}

	if (!sameEmail->IsEqualForSyncUpdate(*sameEmailBis))
		{
		User::Leave(KErrGeneral); //Leave if not equal	
		}
	
	sameEmail->RemoveFieldType(KUidContactFieldVCardMapPREF);

	CleanupStack::PopAndDestroy(2); //type1, type2
	}



void DoBenchMarkL()
	{
	}

/**

@SYMTestCaseID     PIM-T-WRONGFIELDMATCH-0001

*/

LOCAL_C void DoTestsL()
    {
	DoBenchMarkL();
	

	test.Start(_L("@SYMTESTCaseID:PIM-T-WRONGFIELDMATCH-0001 T_WRONGFIELDMATCH"));

	
	DoTestDifferentEmailL();
	DoTestEqualEmailL();
	DoTestSpeedDialIgnoreL();
	DoTestVoiceDialIgnoreL();
	DoTestDifferentPhoneNumberL();
	DoTestEqualPhoneNumberL();
	test.End();
    }

GLDEF_C TInt E32Main()
	{
    // Init
    CTrapCleanup* cleanupStack = CTrapCleanup::New();
    if (!cleanupStack)
        {
        return KErrNoMemory;
        }
    
	// Run the tests
	__UHEAP_MARK;
    TRAPD(err, DoTestsL());
	test.Close(); //avoid memory leak
	__UHEAP_MARKEND;
    // Cleanup

	if (err)
		{
		User::Panic(_L("T_WrongFieldMatch  Failed"),err);
		}



    delete cleanupStack;
	return err;
    }
