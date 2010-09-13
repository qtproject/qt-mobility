/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

//TESTED_COMPONENT=src/telephony

#ifndef __ETELMM_H__
#define __ETELMM_H__

#include <qmobilityglobal.h>
#define private public

extern TUint mock_istatus;
extern TUint mock_linestatus;
extern TUint mock_calltype;
extern TPtrC16 mock_callname;
extern TPtrC16 mock_dialledparty;

class RTelServer;

class RPhone
{
public:   
class TPhoneInfo
    {
        public:
            TPhoneInfo();
            TName iName;
    };    
    
struct TLineInfo {
        TName iName;
    };
    
    RPhone(){
    }
    
    ~RPhone(){
    }
    
    TInt RPhone::Open(RTelServer& /*aSession*/,const TDesC& /*aName*/){
        return KErrNone;
    }

    TInt RPhone::EnumerateLines(TInt& aCount){
        aCount = 1;
        return KErrNone;
    }
    
    TInt RPhone::GetLineInfo(const TInt /*aIndex*/,TLineInfo& /*aLineInfo*/){
        return KErrNone;
    }
    
    TInt RPhone::Close(){
        return KErrNone;
    }  
};

class RTelServer
{
public:
    struct  TPhoneInfo{
        TName iName;
        TUint iNumberOfLines;
        TUint iExtensions;
        };
    
    enum { KDefaultMessageSlots=32 };
    RTelServer(){
    }
    
    ~RTelServer(){
    }
    
    TInt RTelServer::Connect(/*TInt aMessageSlots =KDefaultMessageSlots*/){
        return KErrNone;
    }

    TInt RTelServer::GetPhoneInfo(const TInt, TPhoneInfo& /*aInfo*/){
        //aInfo = TPhoneInfo();
        return KErrNone;
    }
    
    TInt RTelServer::GetTsyName(const TInt /*aIndexOfPhone*/, TDes& /*aTsyName*/){
        return KErrNone;   
    }
    
    TInt RTelServer::LoadPhoneModule(const TDesC& /*aFileName*/){
        return KErrNone;
    }
    
    TInt RTelServer::Close(){
        return KErrNone;
    }
    
};

class RMobilePhone : public RPhone
{
public:   
    RMobilePhone(){
    }
    
    ~RMobilePhone(){
    }
    
    class TMultimodeType
    /**
    Base class for all the V1 parameter types defined within the API.
    
    @publishedPartner
    @released
    */
        {
    public:
        IMPORT_C TInt ExtensionId() const;
    protected:
        TMultimodeType(){
        }
    protected:
        TInt iExtensionId;
        };
        
    /** A typedef'd packaged TMultimodeType for passing through a generic API 
    function member. */
    typedef TPckg<TMultimodeType> TMultimodeTypePckg;
    
    class TMobileAddress
        {
    public:
        TMobileAddress(){
        }
        TBuf<40> iTelNumber;
        };
    
    enum TMobileService
        {
        /** The call service has not been specified.
        
        Modes: Common */
        EServiceUnspecified,
        /** The API request applies to voice call services.

        Modes: Common */
        EVoiceService,
        /** The API request applies to auxiliary voice call services.

        Modes: GSM/WCDMA */
        EAuxVoiceService,
        /** The API request applies to circuit switched data call services.
        
        Modes: Common */
        ECircuitDataService,
        /** The API request applies to packet data services.
    
        Modes: Common */
        EPacketDataService,
        /** The API request applies to fax call services.
    
        Modes: Common */
        EFaxService,
        /** The API request applies to short message services.
    
        Modes: Common */
        EShortMessageService,
        /** The API request applies to all mobile services.
    
        Modes: Common */
        EAllServices
        };
};

class RLine
    {
public:
    RLine(){
    }
    void RLine::Open(RPhone& /*aPhone*/,const TDesC& /*aName*/){
    }
    
    void RLine::Close(){
    }
    
    struct TCallInfo
        {
        TName iCallName;
        };
};

class RMobileLine;

class RMobileCall
{
public:
    /** Describes the possible call states. */
        enum TMobileCallStatus
            {
        /** Indicates that the call status is unknown. */
            EStatusUnknown,         // same as RCall::EStatusUnknown
        /** Indicates that the call is idle. */
            EStatusIdle,            // same as RCall::EStatusIdle
            EStatusDialling,        // same as RCall::EStatusDialling
        /** Indicates that the MT call is ringing but not answered yet by the local user. */
            EStatusRinging,         // same as RCall::EStatusRinging
        /** Indicates that the local user has answered the MT call but the network has not 
        acknowledged the call connection yet. */
            EStatusAnswering,       // same as RCall::EStatusAnswering
        /** MO Call: the network notifies to the MS that the remote party is now ringing. */
            EStatusConnecting,      // same as RCall::EStatusConnecting
        /** Indicates that call is connected and active. */
            EStatusConnected,       // same as RCall::EStatusConnected
        /** Indicates that call is disconnecting. */
            EStatusDisconnecting,   // same as RCall::EStatusHangingUp
        /** Indicates that the call is disconnecting with inband data (to enable the network 
        to send an audio tone), signifying that the call is not to be released until 
        the user terminates the call. */
            EStatusDisconnectingWithInband,
        /** Indicates that call is undergoing temporary channel loss and it may or may not 
        be reconnected. */
            EStatusReconnectPending,
        /** Indicates that the call is connected but on hold. */
            EStatusHold,
        /** Indicates that the call is the non-active half of an alternating call. This 
        call is waiting for its active half or the remote end to switch alternating 
        call mode. */
            EStatusWaitingAlternatingCallSwitch,
            EStatusTransferring,
            EStatusTransferAlerting
            };
    
    RMobileCall(){
    }
    
    ~RMobileCall(){
    }
    
    TInt OpenExistingCall(RMobileLine& /*aLine*/,const TDesC& /*aName*/)
    {
        return KErrNone;    
    }
    
    TInt RMobileCall::Close(){
        return KErrNone;
    } 
    
    enum TMobileCallRemoteIdentityStatus
        {
        ERemoteIdentityUnknown,
        ERemoteIdentityAvailable,
        };

    enum TMobileCallDirection
        {
    /** The direction of the call is unknown. */
        EDirectionUnknown,
    /** The call was originated by this phone, i.e. it is an outgoing call. */
        EMobileOriginated,
    /** The call was terminated by this phone, i.e. it is an incoming call. */
        EMobileTerminated
        };
    
    class TMobileCallRemotePartyInfoV1 : public RMobilePhone::TMultimodeType
/** Defines information about the remote party of a mobile call. 
*/
        {
    public:
        TMobileCallRemotePartyInfoV1(){
        }
    public:
    /** Indicates whether the remote party information in the rest of this structure 
    is valid or not.
    
    Modes: Common
    
    @see TMobileCallRemoteIdentityStatus */
        TMobileCallRemoteIdentityStatus iRemoteIdStatus;
    /** The direction of the call and hence the role of the remote party, i.e. if the 
    call is mobile originated then the remote party is the called party whereas 
    if the call is mobile terminated then the remote party is the calling party.
    
    Modes: Common
    
    @see TMobileCallDirection */
        TMobileCallDirection iDirection;
    /** The phone number of the remote party if available.
    
    Modes: Common
    
    @see RMobilePhone::TMobileAddress */
        RMobilePhone::TMobileAddress iRemoteNumber;
    /** Calling party name available through the CNAP supplementary service (if provisioned). 
    
    
    Modes: Common */
        TBuf<40> iCallingName;
        };

/** A typedef'd packaged TMobileCallRemotePartyInfoV1 for passing through a generic 
API function member. */
    typedef TPckg<TMobileCallRemotePartyInfoV1> TMobileCallRemotePartyInfoV1Pckg;
    
    class TMobileCallInfoV1 : public RMobilePhone::TMultimodeType
/** Defines general information about a mobile call. 
*/
        {
    public:
    /** A bit-wise sum of TMobileCallInfoFlags indicating which of the following optional 
    fields contain valid data. */
        TUint32 iValid;
    /** The call service (voice, data, fax, aux. voice) that this call provides.
    
    @see RMobilePhone::TMobileService */
        RMobilePhone::TMobileService iService;
    /** The mobile call status.
    
    @see TMobileCallStatus */
        TMobileCallStatus iStatus;
    /** The call ID number assigned to an ongoing call. Will equal -1 if the call does 
    not have an allocated ID. */
        TInt iCallId;
    /** The reason for termination of a finished call. Will equal KErrNone if the call 
    ended normally and KErrNotFound if the call has not ended. */
        TInt iExitCode;
    /** ETrue if the call is an emergency call or EFalse if it is not. */
        TBool iEmergency;
    /** For a MT call, this attribute equals ETrue if the call has been forwarded to 
    this phone or EFalse if it has not. For a MO call, this attribute equals ETrue 
    if the call has been forwarded to another destination or EFalse if it has 
    not. If the phone can not determine whether the call has been forwarded or 
    not then the attribute will return EFalse. */
        TBool iForwarded;
    /** This attribute indicates whether privacy is on or off for this call.
    
    @see TMobileCallRemotePartyInfoV1 */
        TMobileCallRemotePartyInfoV1 iRemoteParty;
    /** The original number (including DTMF) dialled for an outgoing call.
    
    @see RMobilePhone::TMobileAddress */
        RMobilePhone::TMobileAddress iDialledParty;
    /** The time and date the call started.
    
    @see TDateTime */
        TDateTime iStartTime;
    /** The name of the call.
    
    @see TName */
        TName iCallName;
    /** The name of the line on which the call is open.
    
    @see TName */
        TName iLineName;
    
    public:
        TMobileCallInfoV1(){
        }  
    };

    typedef TPckg<TMobileCallInfoV1> TMobileCallInfoV1Pckg;

    class TMobileCallDataCapsV1
        {
    public:
        TMobileCallDataCapsV1(){
        }
    public:
        TUint32 iProtocolCaps;
        };

    typedef TPckg<TMobileCallDataCapsV1> TMobileCallDataCapsV1Pckg;

    TInt RMobileCall::GetMobileDataCallCaps(TDes8& aCaps){
        TMobileCallDataCapsV1 caps = TMobileCallDataCapsV1();
        caps.iProtocolCaps = RMobileCall::KCapsIsdnMultimedia;
        RMobileCall::TMobileCallDataCapsV1Pckg capsPckg(caps);
        aCaps.Copy(capsPckg); 
        return KErrNone;
    }
    
    enum TMobileCallDataProtocolCaps
        {
        KCapsIsdnMultimedia = 0x00000400,
        };
    
    TInt GetMobileCallInfo(TDes8& aCallInfo)
    {     
        TMobileCallInfoV1 callInfo = TMobileCallInfoV1();
        callInfo.iCallName.Copy(mock_callname);
        callInfo.iStatus = RMobileCall::TMobileCallStatus(mock_linestatus);
        callInfo.iRemoteParty = TMobileCallRemotePartyInfoV1();
        callInfo.iRemoteParty.iDirection = RMobileCall::EMobileOriginated;
        callInfo.iDialledParty = RMobilePhone::TMobileAddress();
        callInfo.iDialledParty.iTelNumber.Copy(mock_dialledparty);
        callInfo.iService = RMobilePhone::TMobileService(mock_calltype);   
        RMobileCall::TMobileCallInfoV1Pckg callInfoPckg(callInfo);
        aCallInfo.Copy(callInfoPckg);
        return KErrNone;
    }
};

class TCallInfo
{
public:
    TCallInfo();
    TName iCallName;
    TName iLineName;
};

enum {EMobileLineNotifyMobileLineStatusChange = 1};

class RMobileLine
{
public:
    RMobileLine(){
    }
    
    ~RMobileLine(){
    }
    
    TInt RMobileLine::OpenExistingCall(RMobileLine& /*aLine*/,const TDesC& /*aName*/){
        return KErrNone;
    }

    TInt RMobileLine::Open(RPhone& /*aPhone*/,const TDesC& /*aName*/){
        return KErrNone;
    }
    
    TInt RMobileLine::EnumerateCall(TInt& aCount){
        aCount = 1;
        return KErrNone;
    }
    
    TInt RMobileLine::GetCallInfo(const TInt /*aIndex*/,RLine::TCallInfo& /*aLineInfo*/){
        return KErrNone;
    }
    
    void RMobileLine::CancelAsyncRequest(TInt){
    }
    
    TInt RMobileLine::Close(){
        return KErrNone;
    }  
    
    void RMobileLine::NotifyMobileLineStatusChange(TRequestStatus& /*aReqStatus*/, RMobileCall::TMobileCallStatus& /*aStatus*/){
    }
};

#endif // __ETELMM_H__
