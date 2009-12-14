#ifndef __CETELSERVERSUBSESSION_H__
#define __CETELSERVERSUBSESSION_H__

#include <e32base.h>
#include "CSimStoreEmulator.h"
#include "CSimPhoneEmulator.h"

class CETelServerSession;

class CETelServerSubSession 
        :   public CObject, 
            public MStoreNotifyObserver,
            public MPhoneNotifyObserver,
            public MTSYCacheReadyObserver
	{
	public:
		static CETelServerSubSession* NewL( CETelServerSession& aSession,
		    CSimPhoneEmulator& aSimPhoneEmulator);
		
		static CETelServerSubSession* NewL( CETelServerSession& aSession,
		    CSimPhoneEmulator& aSimPhoneEmulator, 
		    CSimStoreEmulator& aSIMEmulator );

		void MPAsyncGetServiceTableL(const RMessage2& aMessage);
        void MPSyncGetFdnStatus(const RMessage2& aMessage);
        void MPSyncSetFdnStatus(const RMessage2& aMessage);
        void MPAsyncNotifyFdnStatusChange(const RMessage2& aMessage);
        void MPSyncGetICCAccessCapsL(const RMessage2& aMessage);
		void MPSAsyncNotifyStoreEventL( const RMessage2& aMessage );
		void MPSAsyncReadL(const RMessage2& aMessage);
		void MPSAsyncGetInfoL(const RMessage2& aMessage);
		void MPSAsyncDeleteL(const RMessage2& aMessage);
		void MPSAsyncWriteL(const RMessage2& aMessage);

		void MPAsyncGetServiceTableCancel();
		void MPAsyncNotifyFdnStatusChangeCancel();
		void MPSAsyncNotifyStoreEventCancelL();
		void MPSAsyncReadCancel();
		void MPSAsyncGetInfoCancel();
		void MPSAsyncDeleteCancel();
		void MPSAsyncWriteCancel();

		void MPBSAsyncReadL( const RMessage2& aMessage );
		void MPBSAsyncWriteL( const RMessage2& aMessage );
        // for custom api
        void CustomApiCacheStatusL( const RMessage2& aMessage );
        void CustomApiCacheStatusCancel( const RMessage2& aMessage );
        void CustomApiNotifyCacheReadyL( const RMessage2& aMessage );
        void CustomApiNotifyCacheReadyCancel( const RMessage2& aMessage );
        void Get3GPBInfoL( const RMessage2& aMessage );

		~CETelServerSubSession();

		void CloseSubSession( TUint aHandle, const RMessage2& aMessage );
	public:
		TInt GetServiceTable();
		TInt NotifyStoreEvent();
		TInt GetInfo();
		TInt Delete();
		TInt Read();
		TInt Write();

	public: // From MNotifyObserver
		void ReceiveNotificationL( TUint32 aEvent, TInt aIndex );
		void FdnChangedNotificationL( 
		    RMobilePhone::TMobilePhoneFdnStatus aFdnStatus );
    private: // From MTSYCacheReadyObserver
        void TSYCacheReadyCallback( CSimStoreEmulator& aStoreEmulator );
        
	private:
		CETelServerSubSession( CETelServerSession& aSession, 
		    CSimPhoneEmulator& aSimPhoneEmulator,
		    CSimStoreEmulator* aSIMEmulator );
		void ConstructL();
	private: // Data
		CETelServerSession* iSession;
		
		// Ref: 
		// can be NULL
		CSimPhoneEmulator& iSimPhoneEmulator; 
		CSimStoreEmulator* iSimStoreEmulator;

		RMessage2 iMPGetServiceTableMsg;
		RMessage2 iMPSGetInfoMsg;
		RMessage2 iMPSNotifyStoreEventMsg;
		RMessage2 iMPSDeleteMsg;
		RMessage2 iMPSWriteMsg;
        RMessage2 iMPBSAsyncWriteMsg;
        RMessage2 iMPBSAsyncReadMsg;
        RMessage2 iMPNotifyFdnStatusChangeMsg;
        RMessage2 iTSYCacheReadyNotification;

		TBool iWaitingGetServiceTable;
		TBool iWaitingNotifyStoreEvent;
		TBool iWaitingRead;
		TBool iWaitingGetInfo;
		TBool iWaitingDelete;
		TBool iWaitingWrite;

		CPeriodic* iPeriodicGetServiceTable;
		CPeriodic* iPeriodicNotifyStoreEvent;
		CPeriodic* iPeriodicGetInfo;
		CPeriodic* iPeriodicDelete;
		CPeriodic* iPeriodicWrite;
        CPeriodic* iPeriodicRead;
        
		TCallBack iGetServiceTableCallBack;
		TCallBack iNotifyStoreEventCallBack;
		TCallBack iGetInfoCallBack;
		TCallBack iDeleteCallBack;
		TCallBack iWriteCallBack;
        TCallBack iReadCallBack;
        
        // Saves the result of the write request
        TInt iMPBSWriteResult;
        TInt iMPBSReadResult;
	};

#endif // __CETELSERVERSUBSESSION_H__

// End of File