#ifndef __CETELSERVER_H__
#define __CETELSERVER_H__

// INCLUDES
#include <e32base.h>
#include "servercommon.h"

class CSimPhoneEmulator;

// CONSTANTS
const TTimeIntervalMicroSeconds32 KETelShutDownAfter =5000000; // 5 s

class MSatRefreshEmulationObserver
    {
    public:
        virtual void SatRefreshL( TDes16& aFiles, TInt aType ) = 0;
    };

// FORWARD DECLARATIONS
class CSimStoreEmulator;

class CETelServer : public CServer2
	{
public:
	static CETelServer* NewLC();
	static CETelServer* NewL();

	static TInt ThreadFunction();

	static void ThreadFunctionL();

	void IncreaseNumClients();
	void DecreaseNumClients();

	~CETelServer();

	CObjectCon* NewContainerL();
    void RemoveObjectContainer( CObjectCon& aContainer );

	/**
	 * Creates a new emulator object if needed.
	 *
	 * @param aStoreName The name of the phonebook.
	 * @return Corresponding SIM phonebook emulator object.
	 */
	CSimStoreEmulator& CreateSIMPbkEmulatorL( const TDesC& aStoreName );
	
	TArray<CSimStoreEmulator*> SIMPbkEmulators() const;
	
	/**
	* @return SIM phone emulator
	*/
	CSimPhoneEmulator& SimPhoneEmulator() const;

    void AddSatRefreshObserverL( MSatRefreshEmulationObserver& aObserver );
    void RemoveSatRefreshObserver( MSatRefreshEmulationObserver& aObserver );
    void MakeSatRefreshL( TDes16& aFiles, TInt aType );

protected:  // From CActive
	TInt RunError(TInt aError);

private: // From CServer2
    CSession2* NewSessionL( const TVersion& aVersion,
        const RMessage2& aMessage) const;
        
private:
	CETelServer(TInt aPriority);
	void ConstructL();

    void InitializeSupportedStoresL();
	static void PanicServer(TETelServerPanic aPanic);
	static TInt IdleDesctruction( TAny* self );
	void StopServer();
	
private: // Data
	TInt iNumClients;

	CObjectConIx* iContainerIndex;

	// Array for different SIM phonebook emulators.
	CArrayPtrFlat<CSimStoreEmulator> iSIMPbkEmulators;
	/// Own: sim phone emulator
	CSimPhoneEmulator* iSimPhoneEmulator;
    /// Own: an asyncrnous destruction of the server
	CPeriodic* iAsyncShutDown;
    RPointerArray<MSatRefreshEmulationObserver> iSatObservers;
	};

#endif // __CETELSERVER_H__