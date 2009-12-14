/*
* ==============================================================================
*  Name        : MSatRefreshObserver.h
*  Part of     : SatClient / SatServer
*  Interface   : Internal, MSatRefreshObserver
*  Description : Observer for refresh events.
*  Version     : 1.0
*
*  Copyright © 2002-2005 Nokia. All rights reserved.
*  This material, including documentation and any related
*  computer programs, is protected by copyright controlled by
*  Nokia. All rights are reserved. Copying, including
*  reproducing, storing, adapting or translating, any
*  or all of this material requires the prior written consent of
*  Nokia. This material also contains confidential
*  information which may not be disclosed to others without the
*  prior written consent of Nokia.
* ==============================================================================
*/


#ifndef MSATREFRESHOBSERVER_H
#define MSATREFRESHOBSERVER_H

//  INCLUDES
#include <e32std.h>
//#include <etelsat.h>

// for simulation copied from etelsat to avoid etel.h include
// DATA TYPES
	/// The enum values of TElementaryFiles are derived directly from ETSI 11.14
	enum TSatElementaryFiles
		{
		KUnknownEf			=0x2000,	///< Unknown EF type
		KDirEf				=0x2F00,	///< Application directory EF
		KElpEf				=0x2F05,	///< Extended Language Preference EF
		KArrEf				=0x2F06,	///< Access Rule Reference EF at the MF level.			
		KImgEf				=0x4F20,	///< Image EF
		KPscEf				=0x4F22,	///< Phonebook synchronisation counter EF
		KCcEf				=0x4F23,	///< Change counter EF
		KPuidEf				=0x4F24,	///< Previous unique identifier EF
		KPbr				=0x4F30,	///< Phonebook reference file EF
		KInvScan			=0x4F64,	///< Investigation File EF
		KImpiEf				=0x6F02,	///< IMS private user identity EF	
		KImpuEf				=0x6F03,	///< IMS public user identity EF
		KDomainEf			=0x6F04,	///< Home Network Domain Name EF
		KLpEf				=0x6F05,	///< Language Preference EF
		KArradEf			=0x6F06,	///< Access rule reference (under ADFUSIM and DFTELECOM) EF
		KImsiEf				=0x6F07,	///< IMSI EF
		KCsp1Ef				=0x6F15,	///< CSP 1 EF (7F20/21 6F15)
		KDckEf				=0x6F2C,	///< De-personalization Control Keys EF
		KPlmnSelEf			=0x6F30,	///< PLMN Selection EF
		KHplmnEf			=0x6F31,	///< Home PLMN EF
		KCnlEf				=0x6F32,	///< Co-operative Network List EF
		KAcmMaxEf			=0x6F37,	///< ACM max value EF
		KSstEf				=0x6F38,	///< SIM Service Table EF
		KAcmEf				=0x6F39,	///< Accumulated Call Meter EF
		KAdnEf				=0x6F3A,	///< Abbreviated Dialling Numbers EF
		KFdnEf				=0x6F3B,	///< Fixed Diallind Numbers EF
		KSmsEf				=0x6F3C,	///< Short Messages EF
		KCcpEf				=0x6F3D,	///< Capability Configuration Parameters EF
		KGid1Ef				=0x6F3E,	///< Group Identifier Level 1 EF
		KGid2Ef				=0x6F3F,	///< Group Identifier Level 2 EF
		KMsisdnEf			=0x6F40,	///< MS ISDN EF
		KPuctEf				=0x6F41,	///< Price per Unit and Currency Table EF
		KSmspEf				=0x6F42,	///< SMS Parameters EF
		KSmssEf				=0x6F43,	///< SMS Status EF
		KLndEf				=0x6F44,	///< Last Number Dialled EF
		KCbmiEf				=0x6F45,	///< Cell Broadcast Message Identifier
		KSpnEf				=0x6F46,	///< Service Provider Name EF
		KSmsrEf				=0x6F47,	///< SMS reports EF
		KCbmidEf			=0x6F48,	///< CBMI for Data Download EF
		KSdnEf				=0x6F49,	///< Service Dialling Numbers EF 
		KExt1Ef				=0x6F4A,	///< Extension 1 EF
		KExt2Ef				=0x6F4B,	///< Extension 2 EF
		KExt3Ef				=0x6F4C,	///< Extension 3 EF
		KBdnEf				=0x6F4D,	///< Barred Dialling Numbers EF
		KExt5Ef				=0x6F4E,	///< Extension 5 EF
		KCcp2Ef				=0x6F4F,	///< Capability configuration parameters 2 EF
		KCbmirEf			=0x6F50,	///< CBMI Range Selection EF
		KNiaEf				=0x6F51,	///< Network's Indication of Alerting EF
		KLociGprsEf			=0x6F53,	///< GPRS Location Information EF
		KSumeEf				=0x6F54,	///< SetUpMenu Elements EF
		KExt4Ef				=0x6F55,	///< Extension 4 EF
		KEstEf				=0x6F56,	///< Enabled services table EF
		KAclEf				=0x6F57,	///< Access point name control list EF
		KCmiEf				=0x6F58,	///< Comparison method information EF
		KStartHfnEf			=0x6F5B,	///< Initialisation value for Hyperframe number EF
		KThresholdEf		=0x6F5C,	///< Maximum value of START EF
		KOPlmnwAcTEf		=0x6F61,	///< Operator controlled PLMN selector with Access Technology EF
		KHPlmnwAcTEf		=0x6F62,	///< HPLMN selector with Access Technology EF
		KRplmnActEf			=0x6F65,	///< RPLMN Last used Access Technology EF
		KPsLociEf			=0x6F73,	///< Packet switched location information EF
		KAccEf				=0x6F78,	///< Access Control Class EF
		KFPlmnEf			=0x6F7B,	///< Forbidden PLMN EF
		KIciEf				=0x6F80,	///< Incoming call information EF
		KOciEf				=0x6F81,	///< Outgoing call information EF
		KIctEf				=0x6F82,	///< Incoming call timer EF
		KOctEf				=0x6F83,	///< Outgoing call timer EF
		KCsp2Ef				=0x6F98,	///< CSP 2 EF (7F40 6F98)
		KAdEf				=0x6FAD,	///< Administrative Data EF
		KPhaseEf			=0x6FAE,	///< Phase Identification EF
		KVcgsEf				=0x6FB1,	///< Voice Group Call Service EF
		KVgcssEf			=0x6FB2,	///< Voice Group Call Service Status EF
		KVbsEf				=0x6FB3,	///< Voice Broadcast Service EF
		KVbssEf				=0x6FB4,	///< Voice Broadcast Service Status EF
		KeMlppEf			=0x6FB5,	///< enhanced Multi Level Pre-emption and Priority EF
		KAaemef				=0x6FB6,	///< Automatic Answer for eMLLP Service EF
		KEccEf				=0x6FB7,	///< Emergency Call Codes EF
		KGmsi				=0x6FC2,	///< Group Identity EF
		KHiddenKeyEf        =0x6FC3,	///< Key for hidden phone book entries EF
		KPnnEf				=0x6fC5,	///< PLMN Network Name EF
		KOplEf				=0x6FC6,	///< Operator Network List EF
		KMbdnEf				=0x6FC7,	///< Mailbox Dialling Numbers EF
		KExt6Ef				=0x6FC8,	///< Extension 6 EF
		KMbiEf				=0x6FC9,	///< Mailbox Identifier EF
		KMwisEf				=0x6FCA,	///< Message Waiting Indication Status EF
		KCfisEf				=0x6FCB,	///< Call Forwarding Indication Status EF
		KExt7Ef				=0x6FCC,	///< Extension 7 EF
		KSpdiEf				=0x6FCD,	///< Service provider display information EF
		KMmsnEF				=0x6FCE,	///< MMS notification EF
		KExt8Ef				=0x6FCF,	///< Extension 8 EF
		KMmsicpEf			=0x6FD0,	///< MMS issuer connectivity parameters EF
		KMmsupEf			=0x6FD1,	///< MMS user preferences EF
		KMmsucpEf			=0x6FD2 	///< MMS user connectivity parameters EF
		};




typedef TBuf16<242> TSatRefreshFiles;

// Refresh types.
enum TSatRefreshType
    {
    ERefreshTypeNotSet,
    ESimInitFullFileChangeNotification,
    EFileChangeNotification,
    ESimInitFileChangeNotification,
    ESimInit,
    ESimReset,
    EUsimApplicationReset,
    E3GSessionReset
    };

// CLASS DECLARATION

/**
*  Observer for refresh events. Client must not call any RSatRefresh
*  methods from the observer methods.
*
*  @lib SatClient
*/
class MSatRefreshObserver
    {
    protected:

        /**
        * C++ constructor.
        */
        MSatRefreshObserver() {};

        /**
        * Destructor.
        */
        virtual ~MSatRefreshObserver() {};

    public: // New functions

        /**
        * Refresh query. Client should determine whether it allow the
        * refresh to happen.
        * @param aType Refresh type.
        * @param aFiles Elementary files which are to be changed.
        * May be zero length.
        * @return ETrue to allow refresh, EFalse to decline refresh.
        */
        virtual TBool AllowRefresh(
            TSatRefreshType aType, const TSatRefreshFiles& aFiles ) = 0;

         /**
        * Notification of refresh.
        * @param aType Type of refresh which has happened.
        * @param aFiles List of elementary files which has been changed.
        * May be zero length.
        */
        virtual void Refresh(
            TSatRefreshType aType, const TSatRefreshFiles& aFiles ) = 0;

    private:

        // Prohibit copy constructor if not deriving from CBase.
        MSatRefreshObserver( const MSatRefreshObserver& );

        // Prohibit assigment operator if not deriving from CBase.
        MSatRefreshObserver& operator=( const MSatRefreshObserver& );

    };

#endif      // MSATREFRESHOBSERVER_H

// End of File
