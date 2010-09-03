/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
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
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qtelephonycalllist_symbian_p.h"
#include "qtelephonycallinfo_p.h"

QTM_BEGIN_NAMESPACE

CLineStatusObserver::CLineStatusObserver(QTelephonyCallListPrivate *aList, TInt aLineIndex)
    : CActive(EPriorityStandard),
      m_callStatus(RMobileCall::EStatusIdle),
      m_list(aList),
      m_lineIndex(aLineIndex),
      m_setupPhase(true)
{
    TRAP_IGNORE ( 
        User::LeaveIfError(m_etel.Connect());
        TBuf<40> tsyName;
        User::LeaveIfError(m_etel.GetTsyName(0,tsyName));
        User::LeaveIfError(m_etel.LoadPhoneModule(tsyName));
        RTelServer::TPhoneInfo phoneInfo;
        TInt phoneIndex = 0;
        User::LeaveIfError(m_etel.GetPhoneInfo(phoneIndex, phoneInfo));
        User::LeaveIfError(m_phone.Open(m_etel, phoneInfo.iName));           
        RPhone::TLineInfo lineInfo;
        User::LeaveIfError(m_phone.GetLineInfo(m_lineIndex, lineInfo));
        User::LeaveIfError(m_line.Open(m_phone, lineInfo.iName));    
        CActiveScheduler::Add(this);
    )
}

CLineStatusObserver::~CLineStatusObserver()
{
    Cancel();
    m_line.Close();
    m_phone.Close();
    m_etel.Close();
}

void CLineStatusObserver::startMonitoring()
{
    updateCallList();
    m_setupPhase = false;
    issueNotifyStatusChange();
}

void CLineStatusObserver::issueNotifyStatusChange()
{   
    m_line.NotifyMobileLineStatusChange(iStatus, m_callStatus);
    SetActive();    
}

void CLineStatusObserver::RunL()
{
    updateCallList();
    issueNotifyStatusChange();
}

void CLineStatusObserver::DoCancel()
{
    m_line.CancelAsyncRequest(EMobileLineNotifyMobileLineStatusChange);
}

QTelephony::CallStatus CLineStatusObserver::mapStatus(TInt aStatus)
{
    // RMobileCall::TMobileCallStatus -- QTelephony::CallStatus mapping
    switch(aStatus) {
    case RMobileCall::EStatusUnknown: 
        return QTelephony::Idle;
    case RMobileCall::EStatusIdle: 
        return QTelephony::Idle;
    case RMobileCall::EStatusDialling: 
        return QTelephony::Dialing;
    case RMobileCall::EStatusRinging: 
        return QTelephony::Incoming;
    case RMobileCall::EStatusAnswering: 
        return QTelephony::Alerting;
    case RMobileCall::EStatusConnecting: 
        return QTelephony::Connected;
    case RMobileCall::EStatusConnected: 
        return QTelephony::Connected;
    case RMobileCall::EStatusReconnectPending:            
        return QTelephony::Connected;
    case RMobileCall::EStatusDisconnecting: 
        return QTelephony::Disconnecting;    
    case RMobileCall::EStatusHold: 
        return QTelephony::OnHold;    
    case RMobileCall::EStatusTransferring: 
        return QTelephony::Connected;
    case RMobileCall::EStatusTransferAlerting: 
        return QTelephony::Alerting;   
    default:
        return QTelephony::Idle;
    };    
}

QTelephony::CallType CLineStatusObserver::mapType(TInt aType)
{
    // RMobilePhone::TMobileService -- QTelephony::CallType mapping
    switch(aType) {
    case RMobilePhone::EVoiceService: 
        return QTelephony::Voice;
    case RMobilePhone::EAuxVoiceService:
        return QTelephony::Voice; 
    case RMobilePhone::ECircuitDataService:
        return QTelephony::Data;
    case RMobilePhone::EPacketDataService:
        return QTelephony::Data;    
    case RMobilePhone::EFaxService: 
        return QTelephony::Data;
    default:
        return QTelephony::Voice;
    };    
}

void CLineStatusObserver::updateCallList()
{
    TRAP_IGNORE
    (
        RLine::TCallInfo callInfo;
        TInt callIndex;
        TInt callCount = 0;
        m_line.EnumerateCall(callCount);            
        for (callIndex = 0; callIndex < callCount; callIndex++) {
            User::LeaveIfError(m_line.GetCallInfo(callIndex, callInfo));
            RMobileCall call;
            User::LeaveIfError(call.OpenExistingCall(m_line, callInfo.iCallName));
            CleanupClosePushL(call);
            RMobileCall::TMobileCallInfoV1 mobileCallInfo;
            RMobileCall::TMobileCallInfoV1Pckg mobileCallInfoPckg(mobileCallInfo);   
            TInt error;
            error = call.GetMobileCallInfo(mobileCallInfoPckg);
            if (error != KErrNotFound) {
                QString callName = toQString(mobileCallInfoPckg().iCallName);
                TInt listIndex = m_list->findCallInfo(callName);
                if (listIndex != -1) {
                    if (mobileCallInfoPckg().iStatus == RMobileCall::EStatusIdle) { 
                        // CALL REMOVED
                        QExplicitlySharedDataPointer<QTelephonyCallInfoPrivate> callinfo_p =
                            QExplicitlySharedDataPointer<QTelephonyCallInfoPrivate>(new QTelephonyCallInfoPrivate());    
                        callinfo_p = m_list->callInfoList.at(listIndex);
                        callinfo_p->status = QTelephony::Idle;
                        m_list->callInfoList.replace(listIndex,QExplicitlySharedDataPointer<QTelephonyCallInfoPrivate>(callinfo_p));
                        m_list->emitActiveCallStatusChanged(*(callinfo_p));
                        m_list->callInfoList.removeAt(listIndex);
                        m_list->emitActiveCallRemoved(*callinfo_p);
                    } else if (m_list->callInfoList.at(listIndex)->status != mapStatus(mobileCallInfoPckg().iStatus)) {
                        // CALL STATUS CHANGED
                        QExplicitlySharedDataPointer<QTelephonyCallInfoPrivate> callinfo_p =
                            QExplicitlySharedDataPointer<QTelephonyCallInfoPrivate>(new QTelephonyCallInfoPrivate());
                        callinfo_p = m_list->callInfoList.at(listIndex);
                        callinfo_p->status = mapStatus(mobileCallInfoPckg().iStatus);
                        m_list->callInfoList.replace(listIndex,QExplicitlySharedDataPointer<QTelephonyCallInfoPrivate>(callinfo_p));
                        m_list->emitActiveCallStatusChanged(*callinfo_p);
                    }
                } else {
                    if (mapStatus(mobileCallInfoPckg().iStatus) == QTelephony::Idle) {
                        // Nothing done for an idle call here.   
                    } else {
                        // CALL ADDED
                        QExplicitlySharedDataPointer<QTelephonyCallInfoPrivate> callinfo_p =
                            QExplicitlySharedDataPointer<QTelephonyCallInfoPrivate>(new QTelephonyCallInfoPrivate());
                        callinfo_p->status = mapStatus(mobileCallInfoPckg().iStatus);
                        callinfo_p->type = mapType(mobileCallInfoPckg().iService);
                        // Video call specific checking
                        if (callinfo_p->type == QTelephony::Data) {
                            RMobileCall::TMobileCallDataCapsV1 caps;
                            RMobileCall::TMobileCallDataCapsV1Pckg capsPckg(caps);
                            error = call.GetMobileDataCallCaps(capsPckg);
                            if (error == KErrNone) {
                                if (caps.iProtocolCaps == RMobileCall::KCapsIsdnMultimedia)    
                                    callinfo_p->type = QTelephony::Video;
                            }
                        }
                        if (mobileCallInfoPckg().iRemoteParty.iDirection == RMobileCall::EMobileOriginated) {
                            callinfo_p->remotePartyIdentifier = toQString(mobileCallInfoPckg().iDialledParty.iTelNumber);
                        } else {
                            if (mobileCallInfoPckg().iRemoteParty.iRemoteIdStatus == RMobileCall::ERemoteIdentityAvailable) {
                                callinfo_p->remotePartyIdentifier = toQString(mobileCallInfoPckg().iRemoteParty.iRemoteNumber.iTelNumber);
                            } else {
                                callinfo_p->remotePartyIdentifier = QString();
                            }       
                        }
                        callinfo_p->values.insert(QString("Name"), callName);
                        m_list->callInfoList.append(callinfo_p);
                        m_list->emitActiveCallAdded(*callinfo_p);
                    }
                }
            }
            CleanupStack::PopAndDestroy(1); // call
        }
        if (!m_setupPhase)
            m_list->checkMobileLines();
    ) // TRAP_IGNORE
}

QString CLineStatusObserver::toQString(const TDesC& aText) 
{
    return QString::fromUtf16(aText.Ptr(), aText.Length());
}


QTelephonyCallListPrivate::QTelephonyCallListPrivate(QTelephonyCallList *parent)
    : p(parent)
{
    TRAP_IGNORE
    (
        User::LeaveIfError(m_etel.Connect());
        TBuf<40> tsyName;
        User::LeaveIfError(m_etel.GetTsyName(0,tsyName));
        User::LeaveIfError(m_etel.LoadPhoneModule(tsyName));
        RTelServer::TPhoneInfo phoneInfo;
        TInt phoneIndex = 0;
        User::LeaveIfError(m_etel.GetPhoneInfo(phoneIndex, phoneInfo));
        User::LeaveIfError(m_phone.Open(m_etel, phoneInfo.iName));       
        TInt lineIndex;
        TInt lineCount;
        m_phone.EnumerateLines(lineCount);            
        for (lineIndex = 0; lineIndex < lineCount; lineIndex++) {        
            CLineStatusObserver* observer = new CLineStatusObserver(this, lineIndex);
            m_observers.append(observer); 
        }
        foreach (CLineStatusObserver *observer, m_observers)
            observer->startMonitoring();
    )
}

QTelephonyCallListPrivate::~QTelephonyCallListPrivate()
{
    m_phone.Close();
    m_etel.Close();
    foreach (CLineStatusObserver *observer, m_observers)
        delete observer;
    if (!callInfoList.isEmpty())
        callInfoList.clear();
}

void QTelephonyCallListPrivate::checkMobileLines()
{
    RMobileCall::TMobileCallStatus status = RMobileCall::EStatusIdle;
    foreach (CLineStatusObserver *observer, m_observers) {
        if ( observer->m_callStatus != RMobileCall::EStatusIdle )
            status = observer->m_callStatus;    
    }           
    if ( status == RMobileCall::EStatusIdle ) { 
        // No calls, clear the call info list
        while (!callInfoList.isEmpty()) {
            QExplicitlySharedDataPointer<QTelephonyCallInfoPrivate> callinfo_p =
                QExplicitlySharedDataPointer<QTelephonyCallInfoPrivate>(new QTelephonyCallInfoPrivate());
            callinfo_p = callInfoList.at(0);
            callinfo_p->status = QTelephony::Idle;
            callInfoList.replace(0,callinfo_p);
            emitActiveCallStatusChanged(*callinfo_p);
            callInfoList.removeAt(0);
            emitActiveCallRemoved(*callinfo_p);
        }
    }
}
    
void QTelephonyCallListPrivate::emitActiveCallStatusChanged(QTelephonyCallInfoPrivate& call)
{
    QTelephonyCallInfo callinfo;
    callinfo.d = QExplicitlySharedDataPointer<QTelephonyCallInfoPrivate>(&call);
    emit p->activeCallStatusChanged(callinfo);
}

void QTelephonyCallListPrivate::emitActiveCallRemoved(QTelephonyCallInfoPrivate& call)
{
    QTelephonyCallInfo callinfo;
    callinfo.d = QExplicitlySharedDataPointer<QTelephonyCallInfoPrivate>(&call);
    emit p->activeCallRemoved(callinfo);
}

void QTelephonyCallListPrivate::emitActiveCallAdded(QTelephonyCallInfoPrivate& call)
{
    QTelephonyCallInfo callinfo;
    callinfo.d = QExplicitlySharedDataPointer<QTelephonyCallInfoPrivate>(&call);
    emit p->activeCallAdded(callinfo);
}

QList<QTelephonyCallInfo> QTelephonyCallListPrivate::activeCalls(const QTelephony::CallType& calltype) const
{
    QList<QTelephonyCallInfo> ret;

    //call copy constructor so the caller has to delete the QTelephonyCallInfo pointers
    for( int i = 0; i < callInfoList.count(); i++){
        if(callInfoList.at(i).data()->type == QTelephony::Any
            || callInfoList.at(i).data()->type == calltype)
        {
            QTelephonyCallInfo callinfo;
            callinfo.d = callInfoList.at(i);
            ret.push_back(callinfo);
        }
    }
    return ret;
}

TInt QTelephonyCallListPrivate::findCallInfo(QString aCallName)
{
    for (int i = 0; i < callInfoList.count(); i++) {
        if (!(callInfoList.at(i).data()->values.value(QString("Name")).toString().localeAwareCompare(aCallName))) {
            return i;
        }
    }
    return -1 ; // not found
}

int QTelephonyCallListPrivate::activeCallCount() const
{
    return callInfoList.count();
}

QTM_END_NAMESPACE
