/*
* ==============================================================================
*  Name        : CSimPhoneEmulator.h
*  Part of     : ETelTestServer
*  Interface   : ?Interface_category, ?Interface_name
*  Description : ?Description
*  Version     : ?Version
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


#ifndef CSIMPHONEEMULATOR_H
#define CSIMPHONEEMULATOR_H

#include <etelmm.h>

class MPhoneNotifyObserver
	{
	public:
		virtual void FdnChangedNotificationL( 
		    RMobilePhone::TMobilePhoneFdnStatus aFdnStatus ) = 0;
	};
	
class CSimPhoneEmulator : public CBase
	{
	public:
		static CSimPhoneEmulator* NewL();
		~CSimPhoneEmulator();

	public: // functions
	    
	    /**
	    * Fills the service table and completes
	    */
        void FillServiceTableL( const RMessage2& aMessage );
        
        /**
        * @return USIM access caps, defined in RMobilePhone TMobilePhoneIccCaps
        */
        TUint32 USimAccessCaps();
        
        /**
        * @return Fixed Dialling status
        */
        RMobilePhone::TMobilePhoneFdnStatus FdnStatus() const;
        
        /**
        * Sets the FDN status and sends notification event
        */
        void SetFdnStatusL( RMobilePhone::TMobilePhoneFdnStatus aFdnStatus);
        
        /**
        * Add FDN notification observer
        */
        void NotifyFdnStatusChangeL( MPhoneNotifyObserver& aObserver );
        
        /**
        * Remove FDN notification observer
        */
        void RemoveFdnStatusChangeL( MPhoneNotifyObserver& aObserver );
        
    private: // Construction
		CSimPhoneEmulator();
		void ConstructL();
    
    private: // Data
        RMobilePhone::TMobilePhoneFdnStatus iFdnStatus;
        RPointerArray<MPhoneNotifyObserver> iFdnObservers;
    };

#endif // CSIMPHONEEMULATOR_H