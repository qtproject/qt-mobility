/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "qtelephonycallinfo_symbian_p.h"
#include <qdebug.h>
#include <qcoreevent.h>
#include <qtimer.h>
#include <ccallinformation.h>
#include <ccallinfoiter.h>
#include <ccallremotepartyinformation.h>
#include <mcallremotepartyinfo.h>
#include <spsettings.h>
#include <spentry.h>

QTM_BEGIN_NAMESPACE

void QTelephonyCallListPrivate::updateCallInformation()
{
    qDebug() << "QTelephonyCallListPrivate::updateCallInformation";
    QList<int> allOngoingCalls; // For checking ended calls

    TRAP_IGNORE( IterateCallInformationListL(allOngoingCalls) );
        
    // Check if calls has been removed
    foreach (QExplicitlySharedDataPointer<QTelephonyCallInfoPrivate> existingCall, callInfoList) {
        if (!allOngoingCalls.contains(existingCall->m_index)) {
            // Make sure that status is changed to idle
            existingCall->status = QTelephony::Idle;
            callInfoList.removeOne(existingCall);
            emitActiveCallRemoved(*existingCall);
            // Call should be deleted after there are no references to it
        }
    
    }
}

void QTelephonyCallListPrivate::CallInformationChanged()
{
    qDebug() << "QTelephonyCallListPrivate::CallInformationChanged";
    // Syncronous ( remote party infomation ) data fetching crashes, do it async
    QMetaObject::invokeMethod(this, "updateCallInformation", Qt::QueuedConnection);
}

void QTelephonyCallListPrivate::IterateCallInformationListL(QList<int> &existingCalls)
{
    CCallInfoIter &iter = m_CallInformation->GetCallsL();
    for (iter.First(); !iter.IsDone(); iter.Next()) {
        bool existingCallFound = false;
        const MCall& call = iter.Current();
        existingCalls.append(call.CallIndex());
        
        // Search if there is already created call
        foreach (QExplicitlySharedDataPointer<QTelephonyCallInfoPrivate> existingCall, callInfoList) {
            // Existing call found
            if (existingCall->m_index == call.CallIndex()) {
                existingCallFound = true;
                
                // Notify if state has been changed
                if (existingCall->status != mapStatus(call.CallState())) {
                    fillCallInfo(*existingCall, call);
                    emitActiveCallStatusChanged(*existingCall);
                }
            }
                
        }
    
        if (!existingCallFound) {
            // No existing call found, create new
            QExplicitlySharedDataPointer<QTelephonyCallInfoPrivate> callinfo =
                    QExplicitlySharedDataPointer<QTelephonyCallInfoPrivate>(
                            new QTelephonyCallInfoPrivate());
        
            fillCallInfo(*callinfo, call);
            fillCallRemoteInfo(*callinfo, call); // Update this only once
            callInfoList.append(callinfo);
            emitActiveCallAdded(*callinfo);
        }
    }
}

void QTelephonyCallListPrivate::fillCallInfo(
        QTelephonyCallInfoPrivate &callInfo, const MCall& fromCall)
{
    callInfo.m_index = fromCall.CallIndex();
    callInfo.status = mapStatus(fromCall.CallState());
    callInfo.type = mapType(fromCall.CallType());
    callInfo.subType = mapSubType(fromCall.ServiceId());
}

void QTelephonyCallListPrivate::fillCallRemoteInfo(
        QTelephonyCallInfoPrivate &callInfo, const MCall& fromCall)
{
    TRAP_IGNORE(
        CCallRemotePartyInformation *remoteInfo = CCallRemotePartyInformation::NewL();
        CleanupStack::PushL(remoteInfo);
        const MCallRemotePartyInfo *info = 
                remoteInfo->GetRemotePartyInformationL(fromCall.CallIndex());
        
        if (info) { // No leave if NULL
            callInfo.remotePartyIdentifier = QString::fromUtf16(
                    info->Address().Ptr(), info->Address().Length());
        }
        CleanupStack::PopAndDestroy(remoteInfo);
    );
}

QTelephony::CallStatus QTelephonyCallListPrivate::mapStatus(
        CCPCall::TCallState aStatus)
{
    switch(aStatus) {
        case CCPCall::EStateIdle: 
            return QTelephony::Idle;
        case CCPCall::EStateDialling: 
            return QTelephony::Dialing;
        case CCPCall::EStateRinging: 
            return QTelephony::Incomming;
        case CCPCall::EStateQueued:
            return QTelephony::Incomming;
        case CCPCall::EStateAnswering: 
            return QTelephony::Incomming;
        case CCPCall::EStateConnecting: 
            return QTelephony::Alerting;
        case CCPCall::EStateConnected: 
            return QTelephony::Connected;
        case CCPCall::EStateDisconnecting: 
            return QTelephony::Disconnecting;    
        case CCPCall::EStateHold: 
            return QTelephony::OnHold;    
        case CCPCall::EStateForwarding: 
            return QTelephony::Alerting;
        case CCPCall::EStateTransferring: 
            return QTelephony::Alerting;   
        default:
            return QTelephony::Idle;
    };    
}

QTelephony::CallType QTelephonyCallListPrivate::mapType(
        CCPCall::TCallType aType)
{
    switch (aType) {
        case CCPCall::ECallTypeCSVoice:
            return QTelephony::Voice;
        case CCPCall::ECallTypePS:
            return QTelephony::Voice;
        case CCPCall::ECallTypeVideo:
            return QTelephony::Video;
        default:
            return QTelephony::Any;
    };
}

QString QTelephonyCallListPrivate::mapSubType(ulong aServiceId)
{
    QString ret("");
    TRAP_IGNORE(
        CSPSettings *settings = CSPSettings::NewLC();
        CSPEntry *entry = CSPEntry::NewLC();
        User::LeaveIfError(settings->FindEntryL(aServiceId, *entry));
        ret = QString::fromUtf16(
                entry->GetServiceName().Ptr(), entry->GetServiceName().Length());
        CleanupStack::PopAndDestroy(entry);
        CleanupStack::PopAndDestroy(settings);
        );
    
    return ret;
}

////////
QTelephonyCallListPrivate::QTelephonyCallListPrivate(QTelephonyCallList *parent)
    : QObject(parent), p(parent), m_CallInformation(0)
{
    TRAP_IGNORE( 
        m_CallInformation = CCallInformation::NewL();
        m_CallInformation->NotifyCallInformationChanges(*this);
        
        // Check if there is existing calls
        QList<int> tmp;
        IterateCallInformationListL(tmp);
    );
    
}

QTelephonyCallListPrivate::~QTelephonyCallListPrivate()
{
    delete m_CallInformation;
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

    //call copy constructor so the caller has to delete the QTelephony pointers
    for( int i = 0; i < callInfoList.count(); i++){
        if(callInfoList[i]->type == QTelephony::Any
            || QTelephony::Any == calltype
            || callInfoList[i]->type == calltype)
        {
            QTelephonyCallInfo callinfo;
            callinfo.d = callInfoList.at(i);
            ret.push_back(callinfo);
        }
    }
    return ret;
}

#include "moc_qtelephonycalllist_symbian_p.cpp"

QTM_END_NAMESPACE
