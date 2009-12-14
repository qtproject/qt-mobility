#ifndef __ETELMM_SERVER_H__
#define __ETELMM_SERVER_H__

#include <e32std.h>
#include "servercommon.h"

#if !defined(__ETEL_H__)
/** @internalComponent */
#define __ETEL_H__
#endif

#if !defined(__E32BASE_H__)
#include <E32BASE.H>
#endif

#if !defined (__C32COMM_H__)
#include <C32COMM.H>
#endif

#if !defined(__D32COMM_H__)
#include <D32COMM.H>
#endif

/*
#if !defined(__FAXDEFN_H)
#include <FAXDEFN.H>
#endif
*/
class RWriteStream;
class RReadStream;
_LIT( KETelMMPanicText, "ETelMM.h (stubbed)" );

_LIT(KETelMeAdnPhoneBook,"S1");
_LIT(KETelMeDialledPhoneBook,"S2");
_LIT(KETelMeMissedPhoneBook,"S3");
_LIT(KETelMeReceivedPhoneBook,"S4");
_LIT(KETelCombinedAdnPhoneBook,"S5");
_LIT(KETelTaAdnPhoneBook,"S6");
_LIT(KETelIccAdnPhoneBook,"S7");
_LIT(KETelIccFdnPhoneBook,"S8");
_LIT(KETelIccSdnPhoneBook,"S9");
_LIT(KETelIccBdnPhoneBook,"S10");
_LIT(KETelIccLndPhoneBook,"S11");
_LIT(KETelIccVoiceMailBox,"S12");

_LIT(KETelMeSmsStore,"S13");
_LIT(KETelIccSmsStore,"S14");
_LIT(KETelCombinedSmsStore,"S15");

_LIT(KETelNamStore,"S16");
_LIT(KETelOwnNumberStore,"S17");
_LIT(KETelEmergencyNumberStore,"S18");

_LIT(KETelSmsMessaging,"S19");
_LIT(KETelBroadcastMessaging,"S20");
_LIT(KETelUssdMessaging,"S21");

_LIT(KETelConferenceCall,"S22");

enum
	{
	// RMobilePhoneStore
	EMobilePhoneStoreGetInfo = 1,
	EMobilePhoneStoreRead,
	EMobilePhoneStoreWrite,
	EMobilePhoneStoreDelete,
	EMobilePhoneStoreNotifyStoreEvent,
	// RMobilePhone
	EMobilePhoneGetServiceTable,
    EMobilePhoneNotifyFdnStatusChange
	};


/***********************************************************************************/
//
//  Global Multimode constants and types
//
/***********************************************************************************/

/**
Unique API identifier and Functional Unit constants
Modes: Common
*/
enum TMultimodeETelV1Api
	{
	/** TSY supports the Multimode ETel v1.0 API.

	Modes: Common */
	KETelExtMultimodeV1=3000,  // 3000 is unique reference for Multimode Etel v1.0 API
	/** TSY supports the MobileSimAccess functional unit.

	Modes: Common */
	KETelFuncMobileIccAccess,
	/** TSY supports the MobileNetwork functional unit.

	Modes: Common */
	KETelFuncMobileNetwork,
	/** TSY supports the MobileIdentity functional unit.

	Modes: Common */
	KETelFuncMobileIdentity,
	/** TSY supports the MobilePower functional unit.

	Modes: Common */
	KETelFuncMobilePower,
	/** TSY supports the MobileSignal functional unit.

	Modes: GSM/WCDMA */
	KETelFuncMobileSignal,
	/** TSY supports the MobileIndicator functional unit.

	Modes: Common */
	KETelFuncMobileIndicator,
	/** TSY supports the MobileDTMF functional unit.

	Modes: Common */
	KETelFuncMobileDTMF,
	/** TSY supports the MobileUserNetworkAccess functional unit.

	Modes: Common */
	KETelFuncMobileUserNetworkAccess,
	/** TSY supports the MobileIdentityService functional unit.

	Modes: Common */
	KETelFuncMobileIdentityService,
	/** TSY supports the MobileCallForwarding functional unit.

	Modes: Common */
	KETelFuncMobileCallForwarding,
	/** TSY supports the MobileCallBarring functional unit.

	Modes: GSM/WCDMA */
	KETelFuncMobileCallBarring,
	/** TSY supports the MobileCallWaiting functional unit.

	Modes: Common */
	KETelFuncMobileCallWaiting,
	/** TSY supports the MobileCallCompletion functional unit. */
	KETelFuncMobileCallCompletion,
	/** TSY supports the MobileAlternatingCall functional unit.

	Modes: GSM/WCDMA */
	KETelFuncMobileAlternatingCall,
	/** TSY supports the MobileCost functional unit.

	Modes: GSM/WCDMA */
	KETelFuncMobileCost,
	/** TSY supports the MobileSecurity functional unit.

	Modes: Common */
	KETelFuncMobileSecurity,
	/** TSY supports the MobileAlternateLineService functional unit.

	Modes: GSM/WCDMA */
	KETelFuncMobileAlternateLineService,
	/** TSY supports the MobileMessageWaiting functional unit.

	Modes: Common */
	KETelFuncMobileMessageWaiting,
	/** TSY supports the MobileFixedDiallingNumbers functional unit.

	Modes: GSM/WCDMA */
	KETelFuncMobileFixedDiallingNumbers,
	/** TSY supports the MobileDataCall functional unit.

	Modes: Common */
	KETelFuncMobileDataCall,
	/** TSY supports the MobilePrivacy functional unit.

	Modes: CDMA */
	KETelFuncMobilePrivacy,
	/** TSY supports the MobileEmergencyCall functional unit.

	Modes: Common */
	KETelFuncMobileEmergencyCall,
	/** TSY supports the MobileSmsMessaging functional unit.

	Modes: Common */
	KETelFuncMobileSmsMessaging,
	/** TSY supports the MobileBroadcastMessaging functional unit.

	Modes: Common */
	KETelFuncMobileBroadcastMessaging,
	/** TSY supports the MobileUssdMessaging functional unit.

	Modes: GSM/WCDMA */
	KETelFuncMobileUssdMessaging,
	/** TSY supports the MobileConferenceCall functional unit.

	Modes: Common */
	KETelFuncMobileConferenceCall,
	/** TSY supports the MobilePhonebookStore functional unit.

	Modes: Common */
	KETelFuncMobilePhonebook,
	/** TSY supports the MobileSmsStore functional unit.

	Modes: Common */
	KETelFuncMobileSmsStore,
	/** TSY supports the MobileNamStore functional unit.

	Modes: CDMA */
	KETelFuncMobileNamStore,
	/** TSY supports the MobileONStore functional unit.

	Modes: Common */
	KETelFuncMobileOwnNumberStore,
	/** TSY supports the MobileENStore functional unit.

	Modes: Common */
	KETelFuncMobileEmergencyNumberStore,
	/** TSY supports the MobileMultimedia functional unit.

	Modes: GSM/WCDMA */
	KETelFuncMobileMulticall,
	/** TSY supports the MobileNextIncomingCall functional unit.

	Modes: GSM/WCDMA */
	KETelFuncMobileNextIncomingCall,
	/** TSY supports the MobileMultimediaCall functional unit.

	Modes: GSM/WCDMA */
	KETelFuncMobileMultimediaCall,
	/** TSY supports the MobileUserSignalling functional unit.

	Modes: GSM/WCDMA */
	KETelFuncMobileUserSignalling
	};

/** Unique API identifier and Functional Unit constants.
Modes: WCDMA

*/
enum TMultimodeETelV2Api
	{
	/** TSY supports the Multimode ETel v2.0 API.

	Modes: WCDMA */
	KETelExtMultimodeV2=7000,  // 7000 is unique reference for Multimode Etel v2.0 API
	/** TSY supports the MobileMultimediaCallSettings functional unit.

	Modes: WCDMA */
	KEtelFuncMobileMultimediaCallSettings,
	/** TSY supports the MobileNetworkSecurity functional unit.

	Modes: WCDMA */
	KEtelFuncMobileNetworkSecurity,
	KEtelFuncMobileUSIMApplications
	};

/**
Enum to define the v3.0 API additions

*/
enum TMultimodeETelV3Api
	{
	/**
	Indicator that the TSY supports the Multimode ETel v3.0 API.
	*/
	KETelExtMultimodeV3 = 10000,
	/**
	TSY supports the MobileMMSInformation functional unit.
	*/
	KEtelFuncMobileMMSInformation,       // 10001
	/**
	TSY supports the KEtelFuncMobileAirTime functional unit.
	*/
	KEtelFuncMobileAirTime,              // 10002
	/**
	TSY supports the KEtelFuncMobileAutoRedial functional unit.
	*/
	KEtelFuncMobileAutoRedial,           // 10003
	/**
	TSY supports the KEtelFuncMobilePersonalisation functional unit.
	*/
	KEtelFuncMobilePersonalisation,      // 10004
	/**
	TSY supports the KEtelMobileMailboxNumbers functional unit.
	*/
	KEtelFuncMobileMailboxNumbers,       // 10005
	/**
	TSY supports the Mobile APN Control List functional unit.
	*/
	KEtelFuncMobileAPNControlList        // 10006
	};

/**
Enum to define the v4.0 API additions

*/
enum TMultimodeETelV4Api
	{
	/**
	Indicator that the TSY supports the Multimode ETel v4.0 API.
	*/
	KETelExtMultimodeV4	= 15000
	};


/** Enum to define Release 5 enabling API and data structures
*/
enum TMultimodeEtelV5Api
	{
	/**
	Indicator that the TSY supports the Multimode ETel v5.0 API.
	*/
	KEtelExtMultimodeV5=20000,
	/**
	Indicator that the TSY supports the IMS functional unit.
	*/
	KEtelFuncMobileIMS,                  // 20001
	/**
	TSY supports the KEtelFuncMobileSmartCardApplications functional unit.
	*/
	KEtelFuncMobileSmartCardApplications // 20002
	};


/**
Enum to define the v6.0 API additions.
*/
enum TMultimodeEtelV6Api
	{
	/**
	Indicator that the TSY supports the Multimode ETel v6.0 API.
	*/
	KEtelExtMultimodeV6=25000,
	/**
	Indicator that the TSY supports the EAP supporting functional unit.
	*/
	KEtelSmartCardEap                    // 25001
	};
/**
Enum to define the v7.0 API additions.
*/
enum TMultimodeEtelV7Api
	{
	/**
	Indicator that the TSY supports the Multimode ETel v7.0 API.
	*/
	KEtelExtMultimodeV7=30000,
	/**
	Indicator that the TSY supports the LocationServices supporting functional unit.
	*/
	KEtelLocationControlServices                    // 30001
	};
/**
Enum to define the Etel 3rdParty V1 API additions

*/
enum TMultimodeETel3rdPartyV1Api
	{
	/**
	Indicator that the TSY supports the Etel 3rd Party v1.0 API
	*/
	KETelExt3rdPartyV1 = 40000
	};


//##ModelId=3DB3FDA90109
class RTelSubSessionBase : public RSubSessionBase
	{
	public:
	//##ModelId=3DB3FDA90114
		RTelSubSessionBase();
	//##ModelId=3DB3FDA9011D
		void CancelAsyncRequest(TInt aReqToCancel) const;

	//##ModelId=3DB3FDA90127
		TInt SubSessionHandle() { return(RSubSessionBase::SubSessionHandle()); }
	//##ModelId=3DB3FDA90128
		RSessionBase& SessionHandle() const { return *iTelSession; }
	//##ModelId=3DB3FDA90131
		void SetSessionHandle(RSessionBase& aTelSession) { iTelSession = &aTelSession; }
	private:
	//##ModelId=3DB3FDA9013B
		TInt MapCancelRequest( TInt aRequest ) const;
	private: // Data
	//##ModelId=3DB3FDA90146
		RSessionBase* iTelSession;
	};

///////////////////////////////////////////////////////////////////////////////
//
// class RMobilePhone
//
///////////////////////////////////////////////////////////////////////////////
class RTelServer;
//##ModelId=3DB3FDA9018C
class RMobilePhone : public RTelSubSessionBase
	{
	public:
		RMobilePhone();

	//##ModelId=3DB3FDA90197
		TInt Open(RTelServer& aSession,const TDesC& aName);
	//##ModelId=3DB3FDA901AA
		void Close();

	//##ModelId=3DB3FDA901C8
		class TMultimodeType
			{
			public:
		//##ModelId=3DB3FDA901D2
			    TInt ExtensionId() const { return 0; }
			protected:
				TMultimodeType() {}
				void InternalizeL(RReadStream& /* aStream */) { User::Panic( KETelMMPanicText, KErrNotSupported ); }
				void ExternalizeL(RWriteStream& /* aStream */) const { User::Panic( KETelMMPanicText, KErrNotSupported ); }
			protected:
		//##ModelId=3DB3FDA901DD
				TInt iExtensionId;
			};

	//##ModelId=3DB3FDA9020E
		enum TMobilePhoneNetworkMode
			{
		//##ModelId=3DB3FDA90222
			ENetworkModeUnknown,
		//##ModelId=3DB3FDA90223
			ENetworkModeUnregistered,
		//##ModelId=3DB3FDA9022C
			ENetworkModeGsm,
		//##ModelId=3DB3FDA90236
			ENetworkModeAmps,
		//##ModelId=3DB3FDA90240
			ENetworkModeCdma95,
		//##ModelId=3DB3FDA90241
			ENetworkModeCdma2000,
		//##ModelId=3DB3FDA9024A
			ENetworkModeWcdma
			};

	//##ModelId=3DB3FDA90268
		enum TMobilePhoneServiceTable
			{
		//##ModelId=3DB3FDA9027C
			ESIMServiceTable,
		//##ModelId=3DB3FDA9027D
			EUSIMServiceTable,
		//##ModelId=3DB3FDA90286
			ECDMAServiceTable,
		//##ModelId=3DB3FDA90290
			EUSIMEnabledServiceTable
			};

	//##ModelId=3DB3FDA902C2
		enum TMobileService
			{
		//##ModelId=3DB3FDA902CD
			EServiceUnspecified,
		//##ModelId=3DB3FDA902D6
			EVoiceService,
		//##ModelId=3DB3FDA902E0
			EAuxVoiceService,
		//##ModelId=3DB3FDA902E1
			ECircuitDataService,
		//##ModelId=3DB3FDA902EA
			EPacketDataService,
		//##ModelId=3DB3FDA902F4
			EFaxService,
		//##ModelId=3DB3FDA902F5
			EShortMessageService,
		//##ModelId=3DB3FDA902FE
			EAllServices
			};

	//##ModelId=3DB3FDA90330
		enum TMobileTON
			{
		//##ModelId=3DB3FDA9033B
			EUnknownNumber,			// 0
		//##ModelId=3DB3FDA90344
			EInternationalNumber,	// 1
		//##ModelId=3DB3FDA9034E
			ENationalNumber,		// 2
		//##ModelId=3DB3FDA9034F
			ENetworkSpecificNumber, // 3
		//##ModelId=3DB3FDA90358
			ESubscriberNumber,		// 4 - Also defined as "dedicated, short code" in GSM 04.08
		//##ModelId=3DB3FDA90362
			EAlphanumericNumber,	// 5
		//##ModelId=3DB3FDA90363
			EAbbreviatedNumber		// 6
			};

	//##ModelId=3DB3FDA9039E
		enum TMobileNPI
			{
		//##ModelId=3DB3FDA903B2
			EUnknownNumberingPlan =0,
		//##ModelId=3DB3FDA903B3
			EIsdnNumberPlan=1,
		//##ModelId=3DB3FDA903BC
			EDataNumberPlan=3,
		//##ModelId=3DB3FDA903C6
			ETelexNumberPlan=4,
		//##ModelId=3DB3FDA903C7
			EServiceCentreSpecificPlan1=5,
		//##ModelId=3DB3FDA903D0
			EServiceCentreSpecificPlan2=6,
		//##ModelId=3DB3FDA903DA
			ENationalNumberPlan=8,
		//##ModelId=3DB3FDA903DB
			EPrivateNumberPlan=9,
		//##ModelId=3DB3FDA903E4
			EERMESNumberPlan=10
			};

		enum
			{
			KMaxMobilePasswordSize=10,
			KMaxMobileNameSize=32,
			KMaxMobileTelNumberSize=100
			};

		enum TMobilePhoneModeCaps
		{
		KCapsGsmSupported=0x00000001,
		KCapsGprsSupported=0x00000002,
		KCapsAmpsSupported=0x00000004,
		KCapsCdma95Supported=0x00000008,
		KCapsCdma2000Supported=0x00000010,
		KCapsWcdmaSupported=0x00000020
		};

	//##ModelId=3DB3FDAA001A
		class TMobileAddress
			{
			public:
				TMobileAddress() {}

				void InternalizeL(RReadStream& /* aStream */) { User::Panic( KETelMMPanicText, KErrNotSupported ); }
				void ExternalizeL(RWriteStream& /* aStream */ ) const { User::Panic( KETelMMPanicText, KErrNotSupported ); }
			public:
		//##ModelId=3DB3FDAA002F
				TMobileTON iTypeOfNumber;
		//##ModelId=3DB3FDAA0043
				TMobileNPI iNumberPlan;
		//##ModelId=3DB3FDAA006C
				TBuf<KMaxMobileTelNumberSize> iTelNumber;
			};

    enum TSSTServices1To8
		{
		KSstADN=0x02,
		KSstFDN=0x04,
		};

	enum TSSTServices9To16
		{
		KSstMSISDN=0x01
		};

	enum TSSTServices17To24
		{
		KSstSDN=0x02,
		};

    enum TUSTServices1To8
		{
	    /** Indicates that the local phonebook is supported by the USIM.
        Local phonebook means a phonebook that is specific to a USIM
        Application, as opposed to the global phonebook which is located
        at the DF Telecom level on the UICC and not attached to
        a USIM Application. */
		KUstLocalPhBk=0x01,
	    /** Indicates that the Fixed Dialling Numbers (FDN) service and
        the storage of FDN numbers within this USIM is supported. */
		KUstFDN=0x02,
	    /** Indicates that the Service Dialling Numbers are supported
        by the USIM. */
		KUstSDN=0x08,
		};

    enum TUSTServices17To24
		{
	    /** Indicates that MSISDN is supported by the USIM. */
		KUstMSISDN=0x10,
		};

	//##ModelId=3DB3FDAA00A7
		class TMobilePhoneServiceTableV1 : public TMultimodeType
			{
			public:
		//##ModelId=3DB3FDAA00BC
				TUint8 iServices1To8;
		//##ModelId=3DB3FDAA00D0
				TUint8 iServices9To16;
		//##ModelId=3DB3FDAA00E4
				TUint8 iServices17To24;
		//##ModelId=3DB3FDAA00F8
				TUint8 iServices25To32;
		//##ModelId=3DB3FDAA010C
				TUint8 iServices33To40;
		//##ModelId=3DB3FDAA0120
				TUint8 iServices41To48;
		//##ModelId=3DB3FDAA0134
				TUint8 iServices49To56;
			};

	//##ModelId=3DB3FDAA0151
		typedef TPckg<TMobilePhoneServiceTableV1> TMobilePhoneServiceTableV1Pckg;

	//##ModelId=3DB3FDA901AB
		void GetServiceTable(TRequestStatus& aReqStatus, TMobilePhoneServiceTable aTable, TDes8& aTableData) const;

	//##ModelId=3DB3FDAA0165
		typedef TBuf<KMaxMobileNameSize> TMobileName;

    /***********************************************************************************/
	//
	// Mobile Fixed Dialling Numbers Functional Unit
	//
	/***********************************************************************************/

	enum TMobilePhoneFdnStatus
		{
		EFdnNotActive,
		EFdnActive,
		EFdnPermanentlyActive,
		EFdnNotSupported,
		EFdnUnknown
		};

    TInt GetFdnStatus(TMobilePhoneFdnStatus& aFdnStatus) const;

    void NotifyFdnStatusChange(TRequestStatus& aReqStatus, TMobilePhoneFdnStatus& aFdnStatus) const;

    // !!! THIS IS NOT SYMBIAN API FUNCTION BUT ONLY FOR TESTING !!!
    // Sets FDN status and sends a notification if changed
    // Returns the result of the request.
    TInt SetFdnStatus(TMobilePhoneFdnStatus& aFdnStatus);


    // ICC access capabilities.
	enum TMobilePhoneIccCaps
		{
	    /** Phone has a SIM and it currently supports SIM access by clients.
	    Modes: GSM
        */
		KCapsSimAccessSupported=0x00000001,
	    /** Phone has a R-UIM and it currently supports R-UIM access by clients.
	    Modes: CDMA
        */
		KCapsRUimAccessSupported=0x00000002,
	    /** Phone has a USIM and it currently supports USIM access by clients.
	    Modes: WCDMA
        */
		KCapsUSimAccessSupported=0x00000004
		};

	TInt GetIccAccessCaps(TUint32& aCaps) const;

	private:
		mutable TPtr8 iTableData;
        mutable TPtr8 iFdnStatus;
        mutable TPtr8 iNotifyFdnStatus;
	};

///////////////////////////////////////////////////////////////////////////////
//
// class RMobilePhoneStore
//
///////////////////////////////////////////////////////////////////////////////
//##ModelId=3DB3FDAA01C9
class RMobilePhoneStore : public RTelSubSessionBase
	{
	public:
		RMobilePhoneStore() :
				iEvent( NULL, 0, 0 ),
				iIndex( NULL, 0, 0 ),
				iInfo( NULL, 0, 0 ),
				iRead( NULL, 0, 0 ) {};
	//##ModelId=3DB3FDAA01D3
		void Read(TRequestStatus& aReqStatus, TDes8& aEntry) const;
	//##ModelId=3DB3FDAA01DE
		void GetInfo(TRequestStatus& aReqStatus, TDes8& aInfo) const;
	//##ModelId=3DB3FDAA01E9
		void NotifyStoreEvent(TRequestStatus& aReqStatus, TUint32& aEvent, TInt& aIndex) const;
	//##ModelId=3DB3FDAA01FB
		void Delete(TRequestStatus& aReqStatus, TInt aIndex) const;
	//##ModelId=3DB3FDAA0206
		void Write(TRequestStatus& aReqStatus, TDes8& aEntry) const;

	//##ModelId=3DB3FDAA0269
		enum TMobilePhoneStoreType
			{
		//##ModelId=3DB3FDAA0273
			EPhoneStoreTypeUnknown,
		//##ModelId=3DB3FDAA027D
			EShortMessageStore,
		//##ModelId=3DB3FDAA027E
			ENamStore,
		//##ModelId=3DB3FDAA0287
			EPhoneBookStore,
		//##ModelId=3DB3FDAA0291
			EEmergencyNumberStore,
		//##ModelId=3DB3FDAA0292
			EOwnNumberStore
			};

	//##ModelId=3DB3FDAA02F5
		enum TMobilePhoneStoreCaps
			{
		//##ModelId=3DB3FDAA0300
			KCapsWholeStore			= 0x80000000,
		//##ModelId=3DB3FDAA0309
			KCapsIndividualEntry	= 0x40000000,
		//##ModelId=3DB3FDAA030A
			KCapsReadAccess			= 0x20000000,
		//##ModelId=3DB3FDAA0314
			KCapsWriteAccess		= 0x10000000,
		//##ModelId=3DB3FDAA031E
			KCapsDeleteAll			= 0x08000000,
		//##ModelId=3DB3FDAA0328
			KCapsNotifyEvent		= 0x04000000
			};

	//##ModelId=3DB3FDAA0378
		class TMobilePhoneStoreInfoV1 : public RMobilePhone::TMultimodeType
			{
			public:
				TMobilePhoneStoreInfoV1() { iType = EPhoneStoreTypeUnknown; }

		//##ModelId=3DB3FDAA038D
				TMobilePhoneStoreType		iType;
		//##ModelId=3DB3FDAA03A1
				TInt iTotalEntries;
		//##ModelId=3DB3FDAA03B5
				TInt iUsedEntries;
		//##ModelId=3DB3FDAA03C9
				TUint32						iCaps;
		//##ModelId=3DB3FDAA03DD
				RMobilePhone::TMobileName	iName;
			};

	//##ModelId=3DB3FDAB0012
		typedef TPckg<TMobilePhoneStoreInfoV1> TMobilePhoneStoreInfoV1Pckg;

	//##ModelId=3DB3FDAB003A
		class TMobilePhoneStoreEntryV1 : public RMobilePhone::TMultimodeType
			{
			public:
				enum
					{
					KIndexNotUsed = -1
					};
			protected:
				TMobilePhoneStoreEntryV1() {}
			public:
				void InternalizeL(RReadStream& /* aStream */) { User::Panic( KETelMMPanicText, KErrNotSupported ); }
				void ExternalizeL(RWriteStream& /* aStream */) const { User::Panic( KETelMMPanicText, KErrNotSupported ); }
			public:
		//##ModelId=3DB3FDAB004F
				TInt  iIndex;
			};

	//##ModelId=3DB3FDAB00B2
		enum TMobileStoreEvent
			{
		//##ModelId=3DB3FDAB00C6
			KStoreFull =0x00000001,
		//##ModelId=3DB3FDAB00C7
			KStoreHasSpace =0x00000002,
		//##ModelId=3DB3FDAB00D0
			KStoreEmpty =0x00000004,
		//##ModelId=3DB3FDAB00DA
			KStoreEntryAdded =0x00000008,
		//##ModelId=3DB3FDAB00DB
			KStoreEntryDeleted =0x00000010,
		//##ModelId=3DB3FDAB00E4
			KStoreEntryChanged =0x00000020,
		//##ModelId=3DB3FDAB00EE
			KStoreDoRefresh =0x00000040
			};

    /** The type of store information. As defined in TMobilePhoneStoreInfoV1::iExtensionId

    Modes: Common

    @see TMobilePhoneStoreInfoV1
    @see RMobilePhone::TMultimodeType */
    	enum TMobilePhoneStoreInfoExtId
    		{
    	/** The iExtensionId contents indicate that the packaged class is a RMobilePhoneStore::TMobilePhoneStoreInfoV1.
    	Modes: Common */
    		KETelMobilePhoneStoreV1=KETelExtMultimodeV1,
    	/** The iExtensionId contents indicate that the packaged class is a RMobilePhoneBookStore::TMobilePhoneBookInfoV1.
    	Modes: Common */
    		KETelMobilePhonebookStoreV1,
    	/** Reserved for SMS store specific information class.
    	Modes: Common */
    		KETelMobileSmsStoreV1,
    	/** The iExtensionId contents indicate that the packaged class is a RMobileNamStore::TMobileNamStoreInfoV1.
    	Modes: CDMA */
    		KETelMobileNamStoreV1,
    	/** The iExtensionId contents indicate that the packaged class is a RMobileONStore::TMobileONStoreInfoV1.
    	Modes: Common */
    		KETelMobileONStoreV1,
    	/** Reserved for Emergency Number store specific infoprmation class.
    	Modes: Common */
    		KETelMobileENStoreV1,
    	/** The iExtensionId contents indicate that the packaged class is a TMobilePhoneBookInfoV2.
    	Modes: Common */
    		KETelMobilePhonebookStoreV2=KETelExtMultimodeV2,

    	/** The iExtensionId contents indicate that the packaged class is a TMobilePhoneBookInfoV2.
     	Modes: Common */
     		KETelMobilePhonebookStoreV5=KEtelExtMultimodeV5
    		};



	private: // Data for asynchronous service calls
		mutable TPtr8 iEvent;
		mutable TPtr8 iIndex;
		mutable TPtr8 iInfo;
		mutable TPtr8 iRead;
	};

///////////////////////////////////////////////////////////////////////////////
//
// class RMobilePhoneBookStore
//
///////////////////////////////////////////////////////////////////////////////
//##ModelId=3DB3FDAB013E
class RMobilePhoneBookStore : public RMobilePhoneStore
	{
	public: // Construction
	    RMobilePhoneBookStore();

	public:
	//##ModelId=3DB3FDAB0148
		void Read(TRequestStatus& aReqStatus, TInt aIndex, TInt aNumSlots, TDes8& aPBData) const;
	//##ModelId=3DB3FDAB015C
		TInt Open(RMobilePhone& aPhone, const TDesC& aStore);
	//##ModelId=3DB3FDAB0167
		void Write(TRequestStatus& aReqStatus, const TDesC8& aPBData, TInt& aIndex) const;
	//##ModelId=3DB3FDAB0172
		void Close();

	//##ModelId=3DB3FDAB01C0
		enum TMobilePhoneBookLocation
			{
		//##ModelId=3DB3FDAB01D4
			ELocationUnknown,
		//##ModelId=3DB3FDAB01D5
			ELocationIccMemory,
		//##ModelId=3DB3FDAB01DE
			ELocationPhoneMemory,
		//##ModelId=3DB3FDAB01E9
			ELocationExternalMemory,
		//##ModelId=3DB3FDAB01EA
			ELocationCombinedMemory
			};

	//##ModelId=3DB3FDAB0261
		enum TMobilePhoneBookCaps
			{
		//##ModelId=3DB3FDAB026C
			KCapsRestrictedWriteAccess = 0x00000001,
		//##ModelId=3DB3FDAB0275
			KCapsSecondNameUsed        = 0x00000002,
		//##ModelId=3DB3FDAB027F
			KCapsAdditionalNumUsed     = 0x00000004,
		//##ModelId=3DB3FDAB0280
			KCapsGroupingUsed		   = 0x00000008,
		//##ModelId=3DB3FDAB0289
			KCapsEntryControlUsed      = 0x00000010,
		//##ModelId=3DB3FDAB0293
			KCapsEmailAddressUsed      = 0x00000020,
		//##ModelId=3DB3FDAB0294
			KCapsBearerCapUsed		   = 0x00000040,
		//##ModelId=3DB3FDAB029D
			KCapsSynchronisationUsed   = 0x00000080
			};

	//##ModelId=3DB3FDAB0351
		enum TMobilePBFieldTags
			{
		//##ModelId=3DB3FDAB035C
			ETagPBNewEntry		=0xA0,
		//##ModelId=3DB3FDAB0365
			ETagPBUniqueId		=0xB0,
		//##ModelId=3DB3FDAB036F
			ETagPBAdnIndex		=0xC0,
		//##ModelId=3DB3FDAB0379
			ETagPBText			=0xC1,
		//##ModelId=3DB3FDAB037A
			ETagPBNumber		=0xC2,
		//##ModelId=3DB3FDAB0383
			ETagPBTonNpi		=0xC3,
		//##ModelId=3DB3FDAB038D
			ETagPBBearerCap		=0xC4,
		//##ModelId=3DB3FDAB038E
			ETagPBAnrStart		=0xC5,
		//##ModelId=3DB3FDAB0397
			ETagPBSecondName	=0xC6,
		//##ModelId=3DB3FDAB03A1
			ETagPBGroupName		=0xC7,
		//##ModelId=3DB3FDAB03A2
			ETagPBEmailAddress	=0xC8,
		//##ModelId=3DB3FDAB03AB
			ETagPBEntryControl	=0xC9,
		//##ModelId=3DB3FDAB03B5
			ETagPBHiddenInfo	=0xCA
			};

		enum
			{
			KMaxPBIDSize=15
			};

	//##ModelId=3DB3FDAB03C9
		typedef TBuf8<KMaxPBIDSize> TMobilePhoneBookIdentity;

	//##ModelId=3DB3FDAC0045
		class TMobilePhoneBookInfoV1 : public RMobilePhoneStore::TMobilePhoneStoreInfoV1
			{
			public:
				TMobilePhoneBookInfoV1() { iType = EPhoneBookStore; };
		//##ModelId=3DB3FDAC005A
				TInt    iMaxNumLength;
		//##ModelId=3DB3FDAC006E
				TInt    iMaxTextLength;
		//##ModelId=3DB3FDAC0082
				TMobilePhoneBookLocation iLocation;
		//##ModelId=3DB3FDAC0096
				TUint16 iChangeCounter;
		//##ModelId=3DB3FDAC00AA
				TMobilePhoneBookIdentity iIdentity;
			};

	//##ModelId=3DB3FDAC00D2
		typedef	TPckg<TMobilePhoneBookInfoV1> TMobilePhoneBookInfoV1Pckg;

        class TMobilePhoneBookInfoV2 : public TMobilePhoneBookInfoV1
        /** Defines information about a Phonebook store for version v2.0 of the API.
        @publishedPartner
        @released */
            {
        public:
            IMPORT_C TMobilePhoneBookInfoV2() :
                TMobilePhoneBookInfoV1(),
                iPhBkMode(0)
                {};
        public:
        /** The type of the phonebook which relates to its location (ICC/UICC/CDMA).

        Mode: Common */
            TName iPhBkMode;
            };

        /** Packages the TMobilePhoneBookInfoV2 into a TMobilePhoneBookInfoV1Pckg. */
        typedef TPckg<TMobilePhoneBookInfoV2> TMobilePhoneBookInfoV2Pckg;

        class TMobilePhoneBookInfoV5 : public TMobilePhoneBookInfoV2
            {
        public:
            IMPORT_C TMobilePhoneBookInfoV5() :
                TMobilePhoneBookInfoV2(),
                    iMaxSecondNames(-1),
                    iMaxTextLengthSecondName(-1),
                    iMaxAdditionalNumbers(-1),
                    iMaxNumLengthAdditionalNumber(-1),
                    iMaxTextLengthAdditionalNumber(-1),
                    iMaxGroupNames(-1),
                    iMaxTextLengthGroupName(-1),
                    iMaxEmailAddr(-1),
                    iMaxTextLengthEmailAddr(-1)
                    {};
        public:
            /** The maximum number of Second Name fields that can be used. */
            TInt iMaxSecondNames;

            /** The maximum text length for Second Name fields. */
            TInt iMaxTextLengthSecondName;

            /** The maximum number of additional number fields that can be used. */
            TInt iMaxAdditionalNumbers;

            /** The maximum number length for Additional Number fields. */
            TInt iMaxNumLengthAdditionalNumber;

            /** The maximum text length for Additonal Number fields. */
            TInt iMaxTextLengthAdditionalNumber;

            /** The maximum number of additional Group Name fields that can be used. */
            TInt iMaxGroupNames;

            /** The maximum text length for Group Name fields. */
            TInt iMaxTextLengthGroupName;

            /** The maximum number of additional E-Mail address fields that can be used. */
            TInt iMaxEmailAddr;

            /** The maximum text length for Email Address fields. */
            TInt iMaxTextLengthEmailAddr;
            };

        /** Packages the TMobilePhoneBookInfoV5 into a TMobilePhoneBookInfoV5Pckg. */
        typedef TPckg<TMobilePhoneBookInfoV5> TMobilePhoneBookInfoV5Pckg;

		// Contains the index where the contact is written
		mutable TPtr8 iWriteIndex;
	};

//##ModelId=3DB3FDAC010E
class RMobileONStore : public RMobilePhoneStore
	{
	public:
		RMobileONStore();
	//##ModelId=3DB3FDAC0118
		TInt Open(RMobilePhone& aPhone);
	//##ModelId=3DB3FDAC0122
		void Close();

	//##ModelId=3DB3FDAC014A
		class TMobileONStoreInfoV1 : public RMobilePhoneStore::TMobilePhoneStoreInfoV1
			{
			public:
				TMobileONStoreInfoV1() { iType = EOwnNumberStore; }
		//##ModelId=3DB3FDAC015F
				TInt iNumberLen;
		//##ModelId=3DB3FDAC0173
				TInt iTextLen;
			};

		typedef TPckg<TMobileONStoreInfoV1> TMobileONStoreInfoV1Pckg;

		enum
			{
			KOwnNumberTextSize = 20
			};

		class TMobileONEntryV1 : public RMobilePhoneStore::TMobilePhoneStoreEntryV1
			{
			public:
				TMobileONEntryV1() {}
			public:
				void InternalizeL(RReadStream& /* aStream */ ) { User::Panic( KETelMMPanicText, KErrNotSupported ); }
				void ExternalizeL(RWriteStream& /* aStream */ ) const { User::Panic( KETelMMPanicText, KErrNotSupported ); }
			public:
		//##ModelId=3DB3FDAC01D7
				RMobilePhone::TMobilePhoneNetworkMode iMode;
		//##ModelId=3DB3FDAC01EB
				RMobilePhone::TMobileService iService;
		//##ModelId=3DB3FDAC01FF
				RMobilePhone::TMobileAddress iNumber;
		//##ModelId=3DB3FDAC021E
				TBuf<KOwnNumberTextSize> iText;
			};

	//##ModelId=3DB3FDAC0244
		typedef TPckg<TMobileONEntryV1> TMobileONEntryV1Pckg;
	};

///////////////////////////////////////////////////////////////////////////////
//
// class RTelServer
//
///////////////////////////////////////////////////////////////////////////////
//##ModelId=3DB3FDAC02BC
class RTelServer : public RSessionBase
	{
	public:
		enum { KDefaultMessageSlots=32 };

		RTelServer() : iNoOfPhones( NULL, 0, 0 ), iInfo( NULL, 0, 0 ) {}

	//##ModelId=3DB3FDAC02C6
		TInt Connect(TInt aMessageSlots = KDefaultMessageSlots);
	//##ModelId=3DB3FDAC02D0
		TInt EnumeratePhones(TInt& aNoOfPhones) const;
	//##ModelId=3DB3FDAC02DA
		TInt LoadPhoneModule(const TDesC& aFileName) const;

	//##ModelId=3DB3FDAC0348
		enum TNetworkType
			{
		//##ModelId=3DB3FDAC0353
			ENetworkTypeWiredAnalog,
		//##ModelId=3DB3FDAC035C
			ENetworkTypeWiredDigital,
		//##ModelId=3DB3FDAC0366
			ENetworkTypeMobileAnalog,
		//##ModelId=3DB3FDAC0367
			ENetworkTypeMobileDigital,
		//##ModelId=3DB3FDAC0370
			ENetworkTypeUnknown
			};

	//##ModelId=3DB3FDAC03B7
		struct  TPhoneInfo
			{
		//##ModelId=3DB3FDAC03D6
			TNetworkType iNetworkType;
		//##ModelId=3DB3FDAD0002
			TName iName;
		//##ModelId=3DB3FDAD0016
			TUint iNumberOfLines;
		//##ModelId=3DB3FDAD002A
			TUint iExtensions;
			};

	//##ModelId=3DB3FDAC02E4
		TInt GetPhoneInfo(const TInt aIndex,TPhoneInfo& aInfo) const;

	//##ModelId=3DB3FDAD0065
		enum TErrorGranularity
			{
		//##ModelId=3DB3FDAD0070
			EErrorBasic,
		//##ModelId=3DB3FDAD0079
			EErrorExtended
			};

	//##ModelId=3DB3FDAC02EE
		TInt SetExtendedErrorGranularity(const TErrorGranularity aGranularity) const;

	//##ModelId=3DB3FDAC02F8
		TVersion Version() const;

        /** SAT refresh testing support. Older version.
         * @param aFiles the refreshed files that are updated by the server
         *               when request completes
         * @param aType the refresh type that is updated by the server
         *              when request completes
         */
        void NotifySatRefresh( TRequestStatus& aStatus, TDes16& aFiles, TDes8& aType ) const;

        /** SAT refresh testing support.
         * @param aFiles the refreshed files that are updated by the server
         *               when request completes
         * @param aType the refresh type that is updated by the server
         *              when request completes
         * @param aInterestedFiles files that client is interested in. Server doesn't
         *                         notify if the changed files are not in this list
         */
        void NotifySatRefresh( TRequestStatus& aStatus, TDes16& aFiles, TDes8& aType,
            const TDes16& aInterestedFiles ) const;

        void CancelSatRefreshNotification() const;
        void MakeSatRefresh( TDes16& aFiles, TInt aType ) const;

	private:
		mutable TPtr8 iNoOfPhones;
		mutable TPtr8 iInfo;
	};

#endif // __ETELMM_H__
