#ifndef __CETELSERVERSESSION_H__
#define __CETELSERVERSESSION_H__

#include <e32base.h>
#include "CETelServer.h"
#include "CSimStoreEmulator.h"

class CETelServerSession;

class CETelServerSession : public CSession2,
                           public MSatRefreshEmulationObserver
	{
	public:
		static CETelServerSession* NewLC( CETelServer& aServer );
		static CETelServerSession* NewL( CETelServer& aServer );

		~CETelServerSession();

		void PanicClient( TInt aPanic, const RMessage2& aMessage ) const;

		void DeleteSubSession( TUint aHandle, const RMessage2& aMessage );
		
		inline CETelServer& Server();
		
	public: 
        /**
        * From CSession2
        */
		void ServiceL(const RMessage2& aMessage);

        /**
        * From MSatRefreshEmulationObserver
        */
        void SatRefreshL( TDes16& aFiles, TInt aType );

	private:
		CETelServerSession( CETelServer& aServer );
		void CreateL();
		void NewSubSessionL( TInt aSessionType, const RMessage2& aMessage );

		CETelServerSubSession* SubSessionFromHandle( TInt aHandle );

		void EnumeratePhonesL( const RMessage2& aMessage );
		void GetPhoneInfoL( const RMessage2& aMessage );

        void DoServiceL(const RMessage2& aMessage);
        void HandleNotifySatRefresh( const RMessage2& aMessage );
        void HandleSpecialNotifySatRefreshL( const RMessage2& aMessage );
        void CancelNotifySatRefresh( const RMessage2& aMessage );
        void MakeSatRefreshL( const RMessage2& aMessage );

	private: // data
		CETelServer& iServer;
		
		CObjectCon* iContainer;
		CObjectIx* iSubSessionIx;

		TInt iResourceCount;
        RMessage2 iSatMessage;
        /// Own: the files from the client that it's interested in
        /// when SAT refresh happens
        HBufC* iInterestedSATFiles;
	};

inline CETelServer& CETelServerSession::Server()
    {
    return iServer;
    }
#endif // __CETELSERVERSESSION_H__