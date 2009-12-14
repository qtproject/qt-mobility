#ifndef RMMCUSTOMAPI_H
#define RMMCUSTOMAPI_H

#include <etelmm_etel_test_server.h>

// CONSTANTS
const TInt KIpcCustomExt        = 10000;
const TInt KCallIpcCustomExt    = ( KIpcCustomExt		  + 15 ); // 10015
const TInt KSimLockIpcCustomExt = ( KCallIpcCustomExt     + 15 ); // 10030
const TInt KViagIpcCustomExt    = ( KSimLockIpcCustomExt  + 5 );  // 10035
const TInt KNetIpcCustomExt     = ( KViagIpcCustomExt     + 10 ); // 10045
const TInt KMiscIpcCustomExt    = ( KNetIpcCustomExt      + 15 ); // 10060
const TInt KSecIpcCustomExt     = ( KMiscIpcCustomExt     + 10 ); // 10070
const TInt KPacketIpcCustomExt  = ( KSecIpcCustomExt      + 10 ); // 10080

enum TCustomIPC
    {
    ECustomNotifyPndCacheReadyIPC = KMiscIpcCustomExt + 1,
    ECustomGetPndCacheStatusIPC,
    EGet3GPBInfoIPC
    };

class RMmCustomAPI : public RTelSubSessionBase
    {
    public:
        // Caching status
        enum TPndCacheStatus
            {
            ECacheNotReady,
            ECacheReady,
            ECacheNotUsed,
            ECacheFailed
            };

        RMmCustomAPI();

        /**
        * Opens a subsession.
        * @param aPhone reference to a Mobile Phone object
        * @return KErrNone if successful, else...
        */
        TInt Open(RMobilePhone& aPhone);

        /**
        * Closes the subsession.
        * @param None
        * @return None
        */
        void Close();

        /**
        * Notify a client when phonebook caching is ready
        * @param aStatus Request status
        * @param aPndName Returns the name of phonebook having ready cache
        * @return None
        */
        void NotifyPndCacheReady
            (TRequestStatus& aStatus, TName& aPndName);

        /**
        * Gets current cache status
        * @param aPndStatus the status of the cache
        * @param aPndName The name of the phonebook
        *
        */
        void GetPndCacheStatus( TRequestStatus& aStatus,
            RMmCustomAPI::TPndCacheStatus& aPndStatus,
            const TName& aPndName ) const;

        /**
        * A struct for extended information about USIM phonebook
        */
        struct T3GPBInfo
	        {
            // Max length of email
            TInt16 iMaxLenEmail;
            // Max length of Second Name
            TInt16 iMaxLenSne;
            // Max length of Additional Number
            TInt16 iMaxLenAnr;
            // Max length of Group Name
            TInt16 iMaxLenGroupName;
            // Max number of email fileds per entry
            TInt8 iMaxNumEmail;
            // Max number of Second Name fields per entry
            TInt8 iMaxNumSne;
            // Max number of Additional Number fields per entry
            TInt8 iMaxNumAnr;
            // Max number of Group Name fields per entry
	        TInt8 iMaxNumGroupName;
        	};

        /**
        * An asynchronous API for getting extended USIM phonebook info
        * @param aStatus the request status
        * @param aInfo the struct to be filled
        */
        void Get3GPBInfo( TRequestStatus& aStatus, T3GPBInfo& aInfo );


    private: // Data
        TPtr8 iPbkName1;
        mutable TPtrC iPbkName2;
        mutable TPtr8 iCacheStatus;
        TPtr8 i3GPBInfo;
    };

#endif // RMMCUSTOMAPI_H
