/*
* ==============================================================================
*  Name        : CSimPhoneEmulator.cpp
*  Part of     : ETelTestServer
*  Description : Emulates the SIM phone
*  Version     : 
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
#include "CSimPhoneEmulator.h"

#include "SimEmulatorConfig.h"
#include "ServerCommon.h"
#include "TestServerDebug.h"

CSimPhoneEmulator* CSimPhoneEmulator::NewL()
    {
    CSimPhoneEmulator* self = new(ELeave) CSimPhoneEmulator;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }
		
CSimPhoneEmulator::~CSimPhoneEmulator()
    {
    iFdnObservers.Close();
    }

CSimPhoneEmulator::CSimPhoneEmulator() : iFdnStatus(SimEmulator::KFdnStatus)
    {
    }

void CSimPhoneEmulator::ConstructL()
    {
    }
    
void CSimPhoneEmulator::FillServiceTableL( const RMessage2& aMessage)
	{
    ETELT_DEBUG_PRINT(ETELT_DEBUG_STRING(
    "[ETELT SERVER] CSimStoreEmulator::FillServiceTableL start"));

	RMobilePhone::TMobilePhoneServiceTableV1 serviceTable;
	RMobilePhone::TMobilePhoneServiceTableV1Pckg serviceTablePckg( serviceTable );

    if ( SimEmulator::KUSimAccessSupported )
        {
        serviceTable.iServices1To8 = SimEmulator::KUSimServices1To8;
        serviceTable.iServices9To16 = SimEmulator::KUSimServices9To16;
	    serviceTable.iServices17To24 = SimEmulator::KUSimServices17To24;
	    serviceTable.iServices25To32 = SimEmulator::KUSimServices25To32;
	    serviceTable.iServices33To40 = SimEmulator::KUSimServices33To40;
	    serviceTable.iServices41To48 = SimEmulator::KUSimServices41To48;
	    serviceTable.iServices49To56 = SimEmulator::KUSimServices49To56;
        }
    else
        {
        serviceTable.iServices1To8 = SimEmulator::KSimServices1To8;
        serviceTable.iServices9To16 = SimEmulator::KSimServices9To16;
	    serviceTable.iServices17To24 = SimEmulator::KSimServices17To24;
	    serviceTable.iServices25To32 = SimEmulator::KSimServices25To32;
	    serviceTable.iServices33To40 = SimEmulator::KSimServices33To40;
	    serviceTable.iServices41To48 = SimEmulator::KSimServices41To48;
	    serviceTable.iServices49To56 = SimEmulator::KSimServices49To56;
        }
	aMessage.WriteL( KSecondMsgSlot, serviceTablePckg );
    ETELT_DEBUG_PRINT(ETELT_DEBUG_STRING(
    "[ETELT SERVER] CSimPhoneEmulator::FillServiceTableL end"));
	}

TUint32 CSimPhoneEmulator::USimAccessCaps()
    {
    TUint32 caps = RMobilePhone::KCapsSimAccessSupported;
    if ( SimEmulator::KUSimAccessSupported )
        {
        caps |= RMobilePhone::KCapsUSimAccessSupported;
        }
    return caps;
    }

RMobilePhone::TMobilePhoneFdnStatus CSimPhoneEmulator::FdnStatus() const
    {
    return iFdnStatus;
    }
        
void CSimPhoneEmulator::SetFdnStatusL( 
        RMobilePhone::TMobilePhoneFdnStatus aFdnStatus )
    {
    RMobilePhone::TMobilePhoneFdnStatus oldStatus = iFdnStatus;
    iFdnStatus = aFdnStatus;
    
    if ( iFdnStatus != oldStatus )
        {
        const TInt count = iFdnObservers.Count();
        for (TInt i = 0; i < count; ++i)
            {
            iFdnObservers[i]->FdnChangedNotificationL(iFdnStatus);
            }
        iFdnObservers.Reset();
        }
    }

void CSimPhoneEmulator::NotifyFdnStatusChangeL( 
        MPhoneNotifyObserver& aObserver )
    {
    if (iFdnObservers.Find(&aObserver) != KErrNotFound)
        {
        User::Leave(KErrAlreadyExists);
        }
    iFdnObservers.AppendL(&aObserver);
    }

void CSimPhoneEmulator::RemoveFdnStatusChangeL( 
        MPhoneNotifyObserver& aObserver )
    {
    TInt pos = iFdnObservers.Find(&aObserver);
    if (pos != KErrNotFound)
        {
        iFdnObservers.Remove(pos);
        }
    } 
