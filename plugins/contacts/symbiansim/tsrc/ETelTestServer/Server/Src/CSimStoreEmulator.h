/*
* ==============================================================================
*  Name        : CSimStoreEmulator.h
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


#ifndef CSIMSTOREEMULATOR_H
#define CSIMSTOREEMULATOR_H

//  INCLUDES
#include <e32base.h>
#include <f32file.h>
#include <s32file.h>
#include <etelmm.h>
#include <rmmcustomapi.h>
#include <msatrefreshobserver.h>

// FORWARD DECLARATIONS
class CETelServerSubSession;
class CEntryBuffer;
class CSimStoreEmulator;

class MStoreNotifyObserver
	{
	public:
		virtual void ReceiveNotificationL( TUint32 aEvent, TInt aIndex ) = 0;
	};

class MTSYCacheReadyObserver
    {
    public:
        virtual void TSYCacheReadyCallback(
                CSimStoreEmulator& aStoreEmulator ) = 0;
    };

// CLASS DECLARATION
class CSimStoreEmulator : public CBase
	{
	public:
		static CSimStoreEmulator* NewL( const TDesC& aStoreName );
		~CSimStoreEmulator();

	public: // functions

		/*
		 * Reads client parameter, creates server side buffer and copies that
		 * to client buffer. If buffer too small returns epmty buffer.
		 *
		 * @param aMessage Contains client side parameter and buffer pointer.
		 * @return KErrNone,...
		 */
		TInt ReadL( const RMessage2& aMessage );

		TInt MPSReadL( const RMessage2& aMessage );
		TInt MPSWriteL( const RMessage2& aMessage );

		TInt WriteL( const RMessage2& aMessage );

		TInt DeleteL( const RMessage2& aMessage );

		void FillInfoL( const RMessage2& aMessage);

        void FillCustomPhoneBookInfoL( const RMessage2& aMessage );

		void AddNotifyObserverL( MStoreNotifyObserver* aObserver );
		void RemoveNotifyObserver( MStoreNotifyObserver* aObserver );

		void SaveEntries();

		const TDesC& Name() const;

		/// TSY Cache emulation
		TBool HasTSYCache() const;
		TBool IsTSYCacheReady() const;
		void AddTSYCacheReadyObserverL(MTSYCacheReadyObserver& aObserver );
		void RemoveTSYCacheReadyObserver(MTSYCacheReadyObserver& aObserver );

        /// SAT refresh support
        const TSatRefreshFiles& ElementaryFiles() const;
        void RefreshL();

	private: // Construction
		CSimStoreEmulator();
		void ConstructL( const TDesC& aStoreName );

	private: // Functions
		void NotifyObserversL( TUint32 aEvent, TInt aIndex );

		void SaveEntriesL( RFileWriteStream& aWriteStream );
		void LoadEntriesL();
		void FillPhoneBookStoreInfoL( const RMessage2& aMessage );
		void FillOwnNumberStoreInfoL( const RMessage2& aMessage );
        void InitializeEmulatorConfigurationL();
        void InitializeEmulatorDataStructuresL();
        TInt GetFirstFreeIndex();
        TInt CheckWritePreCondition( TInt aIndex );
        void AddOrReplaceEntryL( HBufC8* aEntry, TInt aArrayIndex );
        static TInt TSYCacheBuildTimerCallback( TAny* aThis );
        static void Panic( TInt aReason );

	private: // Data

        CEntryBuffer* iEntryBuffer;

		TBool* iReservedEntries;
		CArrayPtrFlat<HBufC8> iEntries;

		TInt64 iRandomSeed;

		RPointerArray<MStoreNotifyObserver> iNotifyObservers;

		RFs iFs;

		HBufC* iPbkName;

        RMobilePhoneBookStore::TMobilePhoneBookInfoV5 iInfo;
        RMmCustomAPI::T3GPBInfo i3GPInfo;

        RArray<TInt> iEntriesWithEmail;
        RArray<TInt> iEntriesWithAnr;
        RArray<TInt> iEntriesWithScndName;
        /// ETrue if the real TSY uses a cache for this store
        TTimeIntervalMicroSeconds32 iTSYCacheBuildTime;
        CPeriodic* iTSYCacheReadyTimer;
        /// Ref: the observer for emulated cache building
        RPointerArray<MTSYCacheReadyObserver> iCacheReadyObservers;
        // Own: SAT Refresh support, contains EFs that are related to
        // this store
        TSatRefreshFiles iSatFilesBuf;
    };

#endif // CSIMSTOREEMULATOR_H
