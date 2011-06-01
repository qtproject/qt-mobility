/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/


#include <e32property.h>
#include <AknUtils.h>
#include <cnwsession.h>
#include <networkhandlingproxy.h>
/*#include <avkondomainpskeys.h>
#include <MProfileEngine.h>
#include <MProfileName.h>
#include <MProfile.h>
#include <featmgr.h>*/
#include "trace.h"
#include <QString>
#include <Qlist>

#include "networkoperatornamelistener_s60.h"


// The following code in this file is based on CAiOperatorNamePublisher and
// CAiNetworkInfoListener class' codes. There are few minor modifications to
// fit systeminfo purposes.


const TInt KBitShiftByFour = 4;
const TInt KIsDigitLowLimit = 0;
const TInt KIsDigitHighLimit = 10;
const TInt KAiMessageCacheGranularity = 4;
//const TInt KOfflineProfileId =  5;

enum TAiNWIdPrioritities
    {
    EAiInvalidPriority      = -1,
    EAiOperatorNetInfoName  = 0,
    EAiMCC_MCN,
    EAiCountryMNC,
    EAiLatinOperatorName,
    EAiUnicodeOperatorName,
    EAiNITZ,
    EAiNetworkOperatorName,
    EAiServiceProviderName,
    EAiFlexiblePLMN,
    EAiProgOperatorLogo,
    EAiOTAOperatorLogo,
    EAiOfflineProfile,
    EAiBTSAP
    };


LOCAL_C void AppendDigit( TDes& aCode, TInt aValue )
    {
    // add a digit if valid value.
    if ( aValue >= KIsDigitLowLimit && aValue < KIsDigitHighLimit )
        {
        aCode.AppendNumUC( static_cast<TUint8>( aValue ) );
        }
    }


//Convert string to integer.
LOCAL_C TInt StrToInt( const TDesC& aDesc )
    {
    TLex lex( aDesc );

    TInt ret;
    TInt err = lex.Val( ret );

    if ( err != KErrNone )
        {
        ret = KErrNotFound;
        }
    return ret;
    }


CNetworkOperatorNameListener::CNetworkOperatorNameListener()
: iKeyProperties( 0, ECmpTInt ), iOperatorPriority( EAiInvalidPriority ),
  iFfEePnnSpnFeatSupported( EFalse ), iNWSesssionstatus(true)
    {
    }


void CNetworkOperatorNameListener::ConstructL()
    {
     TRACES(qDebug() << "CNetworkOperatorNameListener::ConstructL++");
    //Create message cache
    iMessageCache = new( ELeave )CArrayFixFlat
        <MNWMessageObserver::TNWMessages>( KAiMessageCacheGranularity );

    //Create network handling engine session. Needs networkControl Caps
    TRAP_IGNORE( iNWSession = CreateL( *this, iNWInfo ) );
    if (iNWSession == NULL ) {
     //Creation of session has failed due to lack of capabilities
     //All apps need NetworkControl caps to use this
     TRACES(qDebug() << "Warning:NWSession create failed...");
     iNWSesssionstatus = false;
    }
    iOperatorName = HBufC::NewL( KMaxOperatorNameLength );

    // KFeatureIdFfEePnnSpn feature check
    /* Disabled currently
    TRAPD( fmerr, FeatureManager::InitializeLibL() );
    if ( fmerr == KErrNone )
        {
        iFfEePnnSpnFeatSupported =
            FeatureManager::FeatureSupported( KFeatureIdFfEePnnSpn );
        TRACES(qDebug() << "KFeatureIdFfEePnnSpn feature Supported...");
        FeatureManager::UnInitializeLib();
        }*/

    TRACES(qDebug() << "CNetworkOperatorNameListener::ConstructL--");
    }


CNetworkOperatorNameListener* CNetworkOperatorNameListener::NewL()
    {
    TRACES( qDebug() << "CNetworkOperatorNameListener::NewL++" );
    CNetworkOperatorNameListener* self =
        new( ELeave )CNetworkOperatorNameListener();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    TRACES( qDebug() << "CNetworkOperatorNameListener::NewL--" );
    return self;
    }


CNetworkOperatorNameListener::~CNetworkOperatorNameListener()
    {
    TRACES( qDebug() <<  "CNetworkOperatorNameListener::~CNetworkOperatorNameListener++");
    delete iNWSession;
    delete iMessageCache;
    delete iOperatorName;
    }

void CNetworkOperatorNameListener::addObserver(MNetworkOperatorNameObserver *observer)
{
    m_observers.append(observer);
}

void CNetworkOperatorNameListener::removeObserver(MNetworkOperatorNameObserver *observer)
{
    m_observers.removeOne(observer);
}


// ---------------------------------------------------------------------------
// From class MNWMessageObserver.
// Offers message interface to the client.
// ---------------------------------------------------------------------------
//
void CNetworkOperatorNameListener::HandleNetworkMessage(
    const TNWMessages aMessage )
    {
    TRACES ( qDebug() << "CNetworkOperatorNameListener::HandleNetworkMessage++" );
    if ( iNWSesssionstatus == false ) return;
    TRAPD( leaveErr, iMessageCache->InsertIsqL( aMessage, iKeyProperties ) );
    TRACES ( qDebug() << "MessageCache check:" << leaveErr );
    if ( leaveErr == KErrAlreadyExists )
        {
        leaveErr = KErrNone;
        }

    if ( leaveErr != KErrNone )
        {
        TRACES ( qDebug() << "CNetworkOperatorNameListener::HandleNetworkMessage - Error:" << leaveErr );
        return;
        }
    TRACES (qDebug() << "Phase 1: Message check done");

    const TBool showOp( !NotAllowedToDisplayOperatorIndicator( aMessage ) );
    TRACES ( qDebug() << "NotAllowedToDisplayOpearatorIndicator returned: " << showOp );

    if ( !HasNetworkInfoChanged( aMessage ) )
        {
        TRACES( qDebug() << "HasNetworkInfoChanged -- False, so return" );
        return;
        }

    iOperatorPriority = EAiInvalidPriority;
    TRACES ( qDebug() << "Phase 2: OpIndicator check, NetworInfo Changed check" );

    /*TBool isOffline( EFalse );
    MProfileEngine* profileEngine = NULL;
    TRAP( leaveErr,
        profileEngine = CreateProfileEngineL();
        CleanupReleasePushL( *profileEngine );
        isOffline = ( profileEngine->ActiveProfileId() == KOfflineProfileId );
        CleanupStack::PopAndDestroy( profileEngine );
        )

    if ( leaveErr )
        {
        TRACES( qDebug() << "CNetworkOperatorNameListener::HandleNetworkMessage - Profile engine error:" << leaveErr );
        return;
        }*/

    if ( iNWInfo.iRegistrationStatus == ENWNotRegisteredNoService
        || iNWInfo.iRegistrationStatus == ENWRegistrationUnknown
        || !showOp )//|| isOffline
        {
        TBool changed( iOperatorName->Length() != 0 );
        iOperatorName->Des().SetLength( 0 );
        if ( changed )
            {
            //iObserver.OperatorNameChanged();
            foreach (MNetworkOperatorNameObserver *observer, m_observers) {
                    observer->OperatorNameChanged();
              }
            }
        TRACES( qDebug() << "CNetworkOperatorNameListener::HandleNetworkMessage - Not registered- return" );
        return;
        }
    TRACES ( qDebug() << "Phase 3: Network Registration Info check Done" );

    //const TBool isKeyLockEnabled( IsKeyLockEnabled() );
    const TBool isRoaming( IsRoaming() );
    const TInt mcc( StrToInt( iNWInfo.iCountryCode ) );
    const TInt mnc( StrToInt( iNWInfo.iNetworkId ) );

    //Check if PLMN  must be shown.

    const TBool showPLMN = iNWInfo.iServiceProviderNameDisplayReq & ENWDisplayPLMNRequired;

    TBool isInSPDI( EFalse );
    TBool showSPN( EFalse );

    // Check if we are in SPDI and SPN (Service Provider Name) must be shown.
    CheckServiceProviderDisplayListStatus( mcc, mnc, showSPN, isInSPDI );
    TRACES ( qDebug() << "CheckServiceProviderDisplayListStatus Done" );

    //SPN must be shown if it is defined in PLMNField or if it is set in service provider name
    //display requirements.
    showSPN =
        showSPN ||
        ( iNWInfo.iServiceProviderNameDisplayReq & ENWDisplaySPNRequired );

    const TPtrC serviceProviderName = iNWInfo.iSPName;

    // 1. Current network is one of those which preffer plmn.
    // 2. OR current network found in spdi.
    // 3. Isn't roaming (for situations when Orange/TMO network is being used as roaming network)
    if ( //( iFfEePnnSpnFeatSupported ) &&
        ( DoesOperatorPreferPlmnName( mcc, mnc ) || isInSPDI ) && ( !isRoaming ) )
        {
        // CR : Everything Everywhere - PNN shown instead of SPN [#1238048]
        ShowNetworkIdentityNameL( ETrue );
        }
    else if ( ( !isRoaming || isInSPDI ) && serviceProviderName.Length() )
        {
        if ( showPLMN )
            {
            /*if ( isKeyLockEnabled )
                {
                //only plmn
                TRAP_IGNORE( ShowNetworkIdentityNameL( ETrue ) );
                }
            else*/
                {
                //spn & plmn (SPN.And.EONS.No)
                TInt priority( EAiInvalidPriority );
                if ( !OperatorNamePriority( priority ) )
                    {
                    priority = EAiServiceProviderName;
                    }
                SetOperatorName( serviceProviderName, priority );
                }
            }
        else
            {
            //only spn
            SetOperatorName( serviceProviderName, EAiServiceProviderName );
            }
        }
    else
        {
        if ( showSPN && serviceProviderName.Length() )
            {
            /*if ( isKeyLockEnabled )
                {
                //only plmn
                TRAP_IGNORE( ShowNetworkIdentityNameL( ETrue ) );
                }
            else*/
                {
                //spn & plmn (SPN.And.EONS.No)
                TInt priority( EAiInvalidPriority );
                if ( !OperatorNamePriority( priority ) )
                    {
                    priority = EAiServiceProviderName;
                    }
                SetOperatorName( serviceProviderName, priority );
                }
            }
        else
            {
            //only plmn
            TRAP_IGNORE( ShowNetworkIdentityNameL( ETrue ) );
            }
        }
    TRACES ( qDebug() << "CNetworkOperatorNameListener::HandleNetworkMessage--" );
    }


// ---------------------------------------------------------------------------
// Show network indicator.
// ---------------------------------------------------------------------------
//
void CNetworkOperatorNameListener::ShowNetworkIdentityNameL( TBool aTryToPublish )
    {
    TRACES ( qDebug() << "CNetworkOperatorNameListener::ShowNetworkIdentityNameL++" );
    iNetworkIdentityName.Set( KNullDesC );

    TInt priority( EAiInvalidPriority );

    // *** Network operator name (CPHS-ONS) ***
    if ( iNWInfo.iNPName.Length() > 0 &&
        iNWInfo.iOperatorNameInfo.iType != RMmCustomAPI::EOperatorNameFlexiblePlmn &&
        iNWInfo.iRegistrationStatus == ENWRegisteredOnHomeNetwork )
        {
        //priority
        priority = EAiNetworkOperatorName;
        //name
        iNetworkIdentityName.Set( iNWInfo.iNPName );
        //publish network identity name
        if ( aTryToPublish )
            {
            SetOperatorName( iNetworkIdentityName, priority );
            }

        return;
        }

    // *** Operator name ***
    if ( iNWInfo.iOperatorNameInfo.iName.Length() > 0 )
        {
        //priority
        OperatorNamePriority( priority );
        //name
        iNetworkIdentityName.Set( iNWInfo.iOperatorNameInfo.iName );
        //converted name
        HBufC* convertedOperatorName = NULL;

        if ( iNWInfo.iOperatorNameInfo.iType == RMmCustomAPI::EOperatorNameFlexiblePlmn )
            {
            // Long & short name may be in the same buffer.
            const TInt separatorPos = iNetworkIdentityName.Locate( KFlexibleNameSeparator );
            if ( separatorPos != KErrNotFound )
                {
                convertedOperatorName = iNWInfo.iOperatorNameInfo.iName.Left( separatorPos ).AllocLC();
                }
            }
        else if ( iNWInfo.iOperatorNameInfo.iType ==
                      RMmCustomAPI::EOperatorNameMccMnc ||
                  iNWInfo.iOperatorNameInfo.iType ==
                      RMmCustomAPI::EOperatorNameCountryMcn )
            {
            // Perform display language specific conversion.
            convertedOperatorName = iNWInfo.iOperatorNameInfo.iName.AllocLC();
            TPtr ptr = convertedOperatorName->Des();
            AknTextUtils::DisplayTextLanguageSpecificNumberConversion( ptr );
            }
        else
            {
            // No changes needed.
            }

        if ( convertedOperatorName )
            {
            iNetworkIdentityName.Set( *convertedOperatorName );
            }

        if ( priority != EAiInvalidPriority )
            {
            // Publish network identity name
            if ( aTryToPublish )
                {
                SetOperatorName( iNetworkIdentityName, priority );
                }
            }

        if ( convertedOperatorName )
            {
            CleanupStack::PopAndDestroy( convertedOperatorName );
            convertedOperatorName = NULL;
            }

        if ( priority != EAiInvalidPriority )
            {
            return;
            }
        }

    // *** Network info name ***
    if ( priority == EAiInvalidPriority )
        {
        //priority
        priority = EAiOperatorNetInfoName;

        if ( iNWInfo.iLongName.Length() > 0 )
            {
            iNetworkIdentityName.Set( iNWInfo.iLongName );
            }
        else if ( iNWInfo.iShortName.Length() > 0 )
            {
            iNetworkIdentityName.Set( iNWInfo.iShortName );
            }
        else if ( iNWInfo.iDisplayTag.Length() > 0 )
            {
            iNetworkIdentityName.Set( iNWInfo.iDisplayTag );
            }
        else
            {
            priority = EAiInvalidPriority;
            }

        //Publish network identity name
        if ( aTryToPublish )
            {
            SetOperatorName( iNetworkIdentityName, priority );
            }
        return;
        }
    TRACES ( qDebug() << "CNetworkOperatorNameListener::ShowNetworkIdentityNameL--" );
    }


// ---------------------------------------------------------------------------
// Check SPN status from PLMNField. Also check if current country and network
// codes are in PLMNField.
// ---------------------------------------------------------------------------
//
void CNetworkOperatorNameListener::CheckServiceProviderDisplayListStatus(
    TInt aMCC,
    TInt aMNC,
    TBool& aShowSPN,
    TBool& aIsInSPDI )
    {
    TRACES( qDebug() << "CNetworkOperatorNameListener::CheckServiceProviderDisplayListStatus++" );
    // Not in list by default
    aIsInSPDI = EFalse;
    aShowSPN = EFalse;

    if ( !CurrentNetworkOk() )
        {
        return;
        }

    TInt bufferLength = iNWInfo.iPLMNField.Length();
     if ( !bufferLength )
        {
        return;
        }

    const TUint8* field =
        reinterpret_cast< const TUint8* >( iNWInfo.iPLMNField.Ptr() );

    TInt octetIndex = 0;

    // Update SPN showing. (in 3GPP a tag).
    aShowSPN = static_cast<TUint8>( field[ octetIndex ] ) ? EFalse : ETrue ;
    octetIndex++;

    // Number of PLMN pairs. (in 3GPP number of octets).
    const TUint8 length = static_cast<TUint8>( ( field[ octetIndex ] ) );
    octetIndex++;

    // Empty list (always with SIM and sometimes with UICC)
    if ( !( length > 0 ) )
        {
        return;
        }

    // Compare MCC&MNC pairs
    TNWCountryCode mcc;
    TNWIdentity    mnc;

    const TUint numberOfPairs = length;      // each pair has 3 octets

    for ( TUint pair = 0; pair < numberOfPairs; pair++ )
        {
        // 1st octet
        AppendDigit( mcc, field[ octetIndex ] & 0x0F );
        AppendDigit( mcc, ( field[ octetIndex ] & 0xF0 ) >> KBitShiftByFour );
        octetIndex++;

        // 2nd octet
        AppendDigit( mcc, field[ octetIndex ] & 0x0F );
        TInt tmp = ( field[ octetIndex ] & 0xF0 ) >> KBitShiftByFour;
        octetIndex++;

        // 3rd octet
        AppendDigit( mnc, field[ octetIndex ] & 0x0F );
        AppendDigit( mnc, ( field[ octetIndex ] & 0xF0 ) >> KBitShiftByFour );
        octetIndex++;

        AppendDigit( mnc, tmp ); // mnc 3rd digit.

        TInt imcc = StrToInt( mcc );
        TInt imnc = StrToInt( mnc );

        if ( ( aMNC == imnc ) && ( aMCC == imcc ) )
            {
            // match.
            aIsInSPDI = ETrue;
            break;
            }

        // Reset
        mnc.Zero();
        mcc.Zero();
        }
    TRACES( qDebug() << "CNetworkOperatorNameListener::CheckServiceProviderDisplayListStatus--" );
    }


// ---------------------------------------------------------------------------
// Check if current network is ready. Returns ETrue if all needed messages are
// received from network info listener.
// ---------------------------------------------------------------------------
//
TBool CNetworkOperatorNameListener::CurrentNetworkOk()
    {
    TRACES( qDebug() << "CNetworkOperatorNameListener::CurrentNetworkOk++" );
    TBool ok = MessageReceived(
            MNWMessageObserver::ENWMessageNetworkInfoChange ) ||
        MessageReceived(
            MNWMessageObserver::ENWMessageNetworkRegistrationStatusChange );

    ok = ok & ( iNWInfo.iStatus == ENWStatusCurrent );

    TRACERD( _L( "CNetworkOperatorNameListener::CurrentNetworkOk - %d" ), ok );
    TRACES( qDebug() << "CNetworkOperatorNameListener::CurrentNetworkOk--" );
    return ok;
    }


// ---------------------------------------------------------------------------
// Returns ETrue if phone is currently roaming.
// ---------------------------------------------------------------------------
//
TBool CNetworkOperatorNameListener::IsRoaming()
    {
    return MessageReceived(
        MNWMessageObserver::ENWMessageNetworkRegistrationStatusChange ) &&
        ( iNWInfo.iRegistrationStatus == ENWRegisteredRoaming );
    }


// ---------------------------------------------------------------------------
// Check priority of operator name.
// ---------------------------------------------------------------------------
//
TBool CNetworkOperatorNameListener::OperatorNamePriority( TInt& aPriority )
    {
    TBool succeeded = ETrue;

    switch ( iNWInfo.iOperatorNameInfo.iType )
        {
        case RMmCustomAPI::EOperatorNameFlexiblePlmn:
            aPriority = EAiFlexiblePLMN;
            break;

        case RMmCustomAPI::EOperatorNameNitzFull:
            aPriority = EAiNITZ;
            break;

        case RMmCustomAPI::EOperatorNameNitzShort:
            aPriority = EAiNITZ;
            break;

        case RMmCustomAPI::EOperatorNameProgrammableUcs2:
            aPriority = EAiUnicodeOperatorName;
            break;

        case RMmCustomAPI::EOperatorNameProgrammableLatin:
            aPriority = EAiLatinOperatorName;
            break;

        case RMmCustomAPI::EOperatorNameHardcodedUcs2:
            aPriority = EAiUnicodeOperatorName;
            break;

        case RMmCustomAPI::EOperatorNameHardcodedLatin:
            aPriority = EAiLatinOperatorName;
            break;

        case RMmCustomAPI::EOperatorNameCountryMcn:
            aPriority = EAiCountryMNC;
            break;

        case RMmCustomAPI::EOperatorNameMccMnc:
            aPriority = EAiMCC_MCN;
            break;

        default:
            aPriority = EAiInvalidPriority;
            succeeded = EFalse;
            break;
        }
    return succeeded;
    }


// ---------------------------------------------------------------------------
// Check status of keylock.
// ---------------------------------------------------------------------------
//
/*TBool CNetworkOperatorNameListener::IsKeyLockEnabled()
    {
    TInt value;
    TInt err = RProperty::Get(KPSUidAvkonDomain, KAknKeyguardStatus, value);
    if ( err != KErrNone )
        {
        return EFalse;
        }
    switch ( value )
        {
        case EKeyguardLocked:
        case EKeyguardAutolockEmulation:
            return ETrue;
        case EKeyguardNotActive:
        default:
            return EFalse;
        }
    }*/


TBool CNetworkOperatorNameListener::DoesOperatorPreferPlmnName( TInt aMCC, TInt aMNC ) const
    {
    TBool result( EFalse );
    const TInt KMccUK( 234 );
    const TInt KMncOrange1( 33 );
    const TInt KMncOrange2( 34 );
    const TInt KMncTmo( 30 );
    result = ( ( aMCC == KMccUK ) && ( aMNC == KMncOrange1 ||
        aMNC == KMncOrange2 || aMNC == KMncTmo ) );

    return result;
    }


// ---------------------------------------------------------------------------
// Client can use this method to check if certain network message has been
// already received.
// ---------------------------------------------------------------------------
//
TBool CNetworkOperatorNameListener::MessageReceived(
    MNWMessageObserver::TNWMessages aMessage )
    {
    //check if the message is in message cache.
    TInt index( KErrNotFound );
    TBool found(
        iMessageCache->FindIsq( aMessage, iKeyProperties, index ) == 0 );
    return found;
    }


// ---------------------------------------------------------------------------
// From class MNWMessageObserver.
// Offers error message interface to the client.
// ---------------------------------------------------------------------------
//
void CNetworkOperatorNameListener::HandleNetworkError(
    const TNWOperation aOperation, TInt aErrorCode )
    {
    TRACERD( _L( "CNetworkOperatorNameListener::HandleNetworkError - operation:%d error:%d" ), aOperation, aErrorCode );
    if ( iNWSesssionstatus == false ) return;
    TNWMessages errorCode = TNWMessages( KErrGeneral );

    switch ( aOperation )
        {
        case MNWMessageObserver::ENWGetNetworkProviderName:
            iReceivedMessageFlags |= ENetworkProviderNameReceived;
            iReceivedMessageFlags &= ~ENetworkProviderNameOk;
            iNWInfo.iNPName.Zero();
            break;
        case MNWMessageObserver::ENWGetProgrammableOperatorName:
            iReceivedMessageFlags |= EProgrammableOperatorInfoReceived;
            iReceivedMessageFlags &= ~EProgrammableOperatorInfoReceivedOk;
            iNWInfo.iOperatorNameInfo.iName.Zero();
            break;
        case MNWMessageObserver::ENWGetServiceProviderName:
            iReceivedMessageFlags |= EServiceProviderNameReceived;
            iReceivedMessageFlags &= ~EServiceProviderNameOk;
            iNWInfo.iServiceProviderNameDisplayReq =
                RMobilePhone::KDisplaySPNNotRequired;
            iNWInfo.iSPName.Zero();
            iNWInfo.iPLMNField.Zero();
            break;
        case MNWMessageObserver::ENWNotifyNetworkRegistrationStatusChange:
            /*if ( FeatureManager::FeatureSupported( KFeatureIdFfManualSelectionPopulatedPlmnList )
                 && ( KErrGsmMMNetworkFailure == aErrorCode ) )
                {
                errorCode = static_cast<TNWMessages>( aErrorCode );
                }
            TRACERD( _L( "CNetworkOperatorNameListener::HandleNetworkError - ENWNotifyNetworkRegistrationStatusChange error received" ) );
            */
            break;
        default:
            break;
        }

    HandleNetworkMessage( errorCode );
    }


// ---------------------------------------------------------------------------
// Check if it's allowed to show operator indicator.
// Number of rules control operator indicator visibility. Parameter aMessage
// is used for fulfilling these rules - it can have various values so this
// method is called many times. The sum of the calls (and thus sum of
// different aMessage values) defines whether rules are fulfilled and
// indicator is shown.
// ---------------------------------------------------------------------------
//
TBool CNetworkOperatorNameListener::NotAllowedToDisplayOperatorIndicator(
    const TNWMessages aMessage )
    {
    // Service provider name must have been fetched.
    // Network provider name must have been fetched.
    // Registration status and network information must have been received.
    // Operator name information must have been received.
    // Device must be camped to a network.
    // CS registration should be completed (only valid in AT&T NW)
    TRACES (qDebug() << "CNetworkOperatorNameListener::NotAllowedToDisplayOperatorIndicator++");
    switch ( aMessage )
        {
        case MNWMessageObserver::ENWMessageNetworkInfoChange:
            iReceivedMessageFlags |= ENetworkInfoChangeReceived;
            break;
        case MNWMessageObserver::ENWMessageNetworkRegistrationStatusChange:
            iReceivedMessageFlags |= ERegistrationStatusReceived;
            break;
        case MNWMessageObserver::ENWMessageNetworkProviderNameChange:
            iReceivedMessageFlags |=
                ( ENetworkProviderNameReceived + ENetworkProviderNameOk );
            break;
        case MNWMessageObserver::ENWMessageServiceProviderNameChange:
            iReceivedMessageFlags |=
                ( EServiceProviderNameReceived + EServiceProviderNameOk );
            break;
        case MNWMessageObserver::ENWMessageProgrammableOperatorInfoChange:
            iReceivedMessageFlags |=
                ( EProgrammableOperatorInfoReceived +
                  EProgrammableOperatorInfoReceivedOk );
            break;
        case MNWMessageObserver::ENWMessageNetworkProviderNameUpdating:
            iReceivedMessageFlags &=
                ~( ENetworkProviderNameReceived + ENetworkProviderNameOk );
            break;
        case MNWMessageObserver::ENWMessageServiceProviderNameUpdating:
            iReceivedMessageFlags &=
                ~( EServiceProviderNameReceived + EServiceProviderNameOk );
            break;
        case MNWMessageObserver::ENWMessageProgrammableOperatorInfoUpdating:
            iReceivedMessageFlags &=
                ~( EProgrammableOperatorInfoReceived +
                    EProgrammableOperatorInfoReceivedOk );
            break;
        case MNWMessageObserver::ENWMessageDynamicCapsChange:
            /*TRAPD(fmerr, FeatureManager::InitializeLibL());
            if ( fmerr == KErrNone )
                {
                if ( FeatureManager::FeatureSupported(
                    KFeatureIdFfDisplayNetworkNameAfterCsRegistration ) )
                    {
                    // CS flag is EFalse, alpha tag should not be shown.
                    if ( !( RPacketService::KCapsRxCSCall &
                            iNWInfo.iDynamicCapsFlags ) )
                        {
                        TRACES( qDebug() << "CNetworkOperatorNameListener::NotAllowedToDisplayOperatorIndicator - CS registration failed" );
                        iReceivedMessageFlags |= ECSRegistrationNotOk;
                        }
                    else
                        {
                        TRACES( qDebug() << "CNetworkOperatorNameListener::NotAllowedToDisplayOperatorIndicator - CS registration ok" );
                        iReceivedMessageFlags &= ~ECSRegistrationNotOk;
                        }
                    }
                FeatureManager::UnInitializeLib();
                }*/
            break;
        default:
            break;
        }

    TBool serviceProviderNameFetched(
        EServiceProviderNameReceived & iReceivedMessageFlags );
    TBool networkProviderNameFetched(
        ENetworkProviderNameReceived & iReceivedMessageFlags );
    TBool registrationStatusReceived(
        ERegistrationStatusReceived & iReceivedMessageFlags );
    TBool networkInformationReceived(
        ENetworkInfoChangeReceived & iReceivedMessageFlags );
    TBool operatorNameInformationReceived(
         EProgrammableOperatorInfoReceived & iReceivedMessageFlags );
    TBool currentNetworkOk(
        ( ENetworkInfoChangeReceived  & iReceivedMessageFlags ) &&
        ( ERegistrationStatusReceived & iReceivedMessageFlags ) &&
        ( iNWInfo.iStatus == ENWStatusCurrent ) );
    TBool csAlphaFlag( ECSRegistrationNotOk & iReceivedMessageFlags );

    return
        !serviceProviderNameFetched ||
        !networkProviderNameFetched ||
        !( registrationStatusReceived && networkInformationReceived
            && operatorNameInformationReceived ) ||
        !currentNetworkOk || csAlphaFlag;
    }


// ---------------------------------------------------------------------------
// Check if network info has been changed.
// ---------------------------------------------------------------------------
//
TBool CNetworkOperatorNameListener::HasNetworkInfoChanged(
    const TNWMessages aMessage )
    {
    TBool result( ETrue );

    // pass through
    if ( aMessage == MNWMessageObserver::ENWMessageCurrentHomeZoneMessage ||
        aMessage == MNWMessageObserver::ENWMessageNetworkConnectionFailure ||
        aMessage == MNWMessageObserver::ENWMessageCurrentCellInfoMessage ||
        aMessage == static_cast<TNWMessages>( KErrGsmMMNetworkFailure )
       )
        {
        return result;
        }

    result = ( iReceivedMessageFlags != iOldReceivedMessageFlags );

    if ( !result )
        {
        // Check if contents of iInfo has changed. Most probable to the
        // beginning of the expression.
        result =
            iNWInfo.iRegistrationStatus != iOldInfo.iRegistrationStatus ||
            iNWInfo.iStatus != iOldInfo.iStatus ||
            iNWInfo.iCountryCode != iOldInfo.iCountryCode ||
            iNWInfo.iNetworkId != iOldInfo.iNetworkId ||
            iNWInfo.iOperatorNameInfo.iType !=
                iOldInfo.iOperatorNameInfo.iType ||
            iNWInfo.iOperatorNameInfo.iName !=
                iOldInfo.iOperatorNameInfo.iName ||
            iNWInfo.iDisplayTag != iOldInfo.iDisplayTag ||
            iNWInfo.iShortName != iOldInfo.iShortName ||
            iNWInfo.iLongName != iOldInfo.iLongName ||
            iNWInfo.iSPName != iOldInfo.iSPName ||
            iNWInfo.iServiceProviderNameDisplayReq !=
                iOldInfo.iServiceProviderNameDisplayReq ||
            iNWInfo.iNPName != iOldInfo.iNPName ||
            iNWInfo.iPLMNField != iOldInfo.iPLMNField;
        /*TRAPD( fmerr, FeatureManager::InitializeLibL() );
        if ( fmerr == KErrNone )
            {
            if ( FeatureManager::FeatureSupported(
                KFeatureIdFfDisplayNetworkNameAfterCsRegistration ) )
                {
                result = result ||
                    iNWInfo.iDynamicCapsFlags != iOldInfo.iDynamicCapsFlags;
                }
            FeatureManager::UnInitializeLib();
            }*/
        }

    iOldReceivedMessageFlags = iReceivedMessageFlags;
    iOldInfo = iNWInfo;

    return result;
    }


// ---------------------------------------------------------------------------
// Set current operator name string based on the set priority.
// ---------------------------------------------------------------------------
//
void CNetworkOperatorNameListener::SetOperatorName(
    const TPtrC& aOperatorName,
    TInt aPriority )
    {
    TRACES( qDebug() << "CNetworkOperatorNameListener::SetOperatorName++" );
    if ( iOperatorPriority <= aPriority )
        {
        TRACERD( aOperatorName );
        TBool changed( iOperatorName->Compare( aOperatorName ) );
        *iOperatorName = aOperatorName;
        iOperatorPriority = aPriority;
        if ( changed )
            {
             foreach (MNetworkOperatorNameObserver *observer, m_observers) {
                    observer->OperatorNameChanged();
             }
            }
        }
    }


// ---------------------------------------------------------------------------
// Pass the current operator name.
// ---------------------------------------------------------------------------
//
/*void CNetworkOperatorNameListener::GetOperatorName( TDes& aOperatorName )
    {
    aOperatorName.Copy( iOperatorName->Left( aOperatorName.MaxLength() ) );
    }*/

QString CNetworkOperatorNameListener::networkName()
{
 TRACES( qDebug() << "CNetworkOperatorNameListener::networkName++" );
 if (iOperatorName->Length() != 0 ) {
    m_operatorName = QString::fromUtf16(iOperatorName->Ptr(), iOperatorName->Length());
    return m_operatorName;
   }
 return QString();
}

