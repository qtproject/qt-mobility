/*
* ==============================================================================
*  Name        : SimEmulatorConfig.h
*  Part of     : ETel test server / ETelTestServer.dll
*  Interface   :
*  Description : Configuration of SIM emulator
*  Version     : 1.0
*
*  Copyright © 2002 Nokia Corporation.
*  This material, including documentation and any related
*  computer programs, is protected by copyright controlled by
*  Nokia Corporation. All rights are reserved. Copying,
*  including reproducing, storing, adapting or translating, any
*  or all of this material requires the prior written consent of
*  Nokia Corporation. This material also contains confidential
*  information which may not be disclosed to others without the
*  prior written consent of Nokia Corporation.
* ==============================================================================
*/


#ifndef SIMEMULATORCONFIG_H
#define SIMEMULATORCONFIG_H

//  INCLUDES
#include <etelmm.h>

// CONSTANTS

namespace SimEmulator
    {
    // Database filename
    // %S is ETel name ( ADN=S7,SDN=S9,Own number=S17)
    _LIT( KDatabaseName, "c:\\System\\Data\\SIMEmulator%S.db" );

    // SIM or USIM
    const TBool KUSimAccessSupported = ETrue;

    // ADN configuration
    const TInt KMaxADNEntries = 50;
    const TInt KMaxADNTextLength = 20;
    const TInt KMaxADNNumberLength = 48;
    const TTimeIntervalMicroSeconds32 KTSYCacheBuildingTimeForADN = 2000000;
    // USIM capabilities that are used if KUSimAccessSupported is ETrue
    const TUint32 KUSimADNCaps = RMobilePhoneBookStore::KCapsReadAccess  |
					         RMobilePhoneBookStore::KCapsWriteAccess |
					         RMobilePhoneBookStore::KCapsNotifyEvent |
                             RMobilePhoneBookStore::KCapsSecondNameUsed |
                             RMobilePhoneBookStore::KCapsAdditionalNumUsed |
                             RMobilePhoneBookStore::KCapsEmailAddressUsed;
    // SIM capabilities that are used if KUSimAccessSupported is EFalse
    const TUint32 KSimADNCaps = RMobilePhoneBookStore::KCapsReadAccess  |
						         RMobilePhoneBookStore::KCapsWriteAccess |
						         RMobilePhoneBookStore::KCapsNotifyEvent;
    const TInt16 KMaxEmailLength = 20;
    const TInt16 KMaxScndNameLength = 20;
    const TInt16 KMaxAnrLength = 20;
    const TInt16 KMaxGroupNameLength = 15;
    const TInt8 KMaxEMailFields = 1;
    const TInt8 KMaxScndNameFields = 1;
    const TInt8 KMaxAnrFields = 1;
	const TInt8 KMaxGroupNameFields = 1;
    const TInt16 KMaxEmailEntries = 10;
    const TInt16 KMaxAnrEntries = 20;
    const TInt16 KMaxScndNameEntries = 25;

    // SDN configuration
    const TInt KMaxSDNEntries = 1;
    const TInt KMaxSDNTextLength = 20;
    const TInt KMaxSDNNumberLength = 48;
    // The real TSY doesn't use cache, set to zero.
    const TTimeIntervalMicroSeconds32 KTSYCacheBuildingTimeForSDN = 0;
    const TUint32 KSDNCaps = RMobilePhoneStore::KCapsReadAccess;

    // FDN configuration
    const TInt KMaxFDNEntries = 20;
    const TInt KMaxFDNTextLength = 20;
    const TInt KMaxFDNNumberLength = 48;
    const TTimeIntervalMicroSeconds32 KTSYCacheBuildingTimeForFDN = 1000000;
    const TUint32 KFDNCaps = RMobilePhoneStore::KCapsReadAccess |
                             RMobilePhoneBookStore::KCapsWriteAccess |
					         RMobilePhoneBookStore::KCapsNotifyEvent;
	// FDN status
    const RMobilePhone::TMobilePhoneFdnStatus KFdnStatus =
        RMobilePhone::EFdnNotActive;

    // Own number configuration
    const TInt KMaxOwnNumberEntries = 5;
    const TInt KMaxOwnNumberTextLength = 20;
    const TInt KMaxOwnNumberNumberLength = 48;
    // The real TSY doesn't use cache, set to zero.
    const TTimeIntervalMicroSeconds32 KTSYCacheBuildingTimeForMSISDN = 0;
    const TUint32 KOwnNumberCaps = RMobilePhoneStore::KCapsReadAccess |
    							   RMobilePhoneBookStore::KCapsWriteAccess;

    // SIM service table configuration
    const TUint8 KSimServices1To8 = RMobilePhone::KSstADN | RMobilePhone::KSstFDN;
    const TUint8 KSimServices9To16 = RMobilePhone::KSstMSISDN;
    const TUint8 KSimServices17To24 = RMobilePhone::KSstSDN;
    const TUint8 KSimServices25To32 = 0;
    const TUint8 KSimServices33To40 = 0;
    const TUint8 KSimServices41To48 = 0;
    const TUint8 KSimServices49To56 = 0;

    // USIM service table configuration
    const TUint8 KUSimServices1To8 = RMobilePhone::KUstSDN | RMobilePhone::KUstFDN;
    const TUint8 KUSimServices9To16 = 0;
    const TUint8 KUSimServices17To24 = RMobilePhone::KUstMSISDN;
    const TUint8 KUSimServices25To32 = 0;
    const TUint8 KUSimServices33To40 = 0;
    const TUint8 KUSimServices41To48 = 0;
    const TUint8 KUSimServices49To56 = 0;
    
    // USIM AAS and Group, etel simulator does not support these yet
    // Max length of Additional Alpha String
    const TInt16 KMaxLenAas = 0;   
    // Max number of group alpha strings
    const TInt8 KMaxNumOfGas = 0;
    // Max number of Alpha Strings
    const TInt KMaxNumOfAas = 0;
    }

#endif      // SIMEMULATORCONFIG_H

// End of File
