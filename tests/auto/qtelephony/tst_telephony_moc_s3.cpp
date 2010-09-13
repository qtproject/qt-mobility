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

#include "qtelephonycallinfo_symbian_p.h"
#include <qmobilityglobal.h>
#define private public
#include <mcallinformationobserver.h>
#include <ccallinformation.h>
#include <ccallinfoiter.h>
#include <ccallremotepartyinformation.h>
#include <mcallremotepartyinfo.h>
#include <spsettings.h>
#include <spentry.h>



class CCall : 
    public MCall, public MCallRemotePartyInfo
{
public:
    CCall(QString number, int index, CCPCall::TCallType type) :
        m_number(number),
        m_index(index),
        m_type(type)
        {}
    
    ~CCall() {}

// From MCall
    TInt CallIndex() const { return m_index; } 
    CCPCall::TCallType CallType() const { return m_type; }
    CCPCall::TCallState CallState() const { return m_status; }
    TUint32 ServiceId() const { return 0; }
    CCPCall::TCallDirection CallDirection() const { return CCPCall::EMobileOriginated; }
    TBool IsEmergency() const { return EFalse;}

    // From MCallRemotePartyInfo
    const TDesC& Address() const { return TPtrC(m_number.utf16()); }
    const TDesC& MatchedName() const { return KNullDesC(); }
    const TDesC& NameFromNetwork() const { return KNullDesC(); }
    const TDesC& CompanyName() const { return KNullDesC(); }
    //TInt CallIndex() const {}
    TRemoteIdentityStatus RemoteIdentity() const { return ERemoteIdentityAvailable; }
    TBool SATCall() const { return false; }
    TBool operator==(const MCallRemotePartyInfo& ) const { return EFalse; }
    
    CCPCall::TCallState m_status;
    CCPCall::TCallType m_type;
    QString m_number;
    int m_index;
};

MCallInformationObserver *g_observer = 0;
QList<CCall> g_callList;
int g_currentCall = 0;

CCallInfoIter::CCallInfoIter(CCallInfos &i) : iCallInfos(i)
{
    
}

CCallInfoIter::~CCallInfoIter()
{
    
}

CCallInfoIter& CCallInformation::GetCallsL()
{
    static CCallInfoIter ret(*(CCallInfos*)NULL);
    return ret;
}

void CCallInfoIter::First()
{
    g_currentCall = 0;
}

class MCall const & CCallInfoIter::Current() const
{
    return g_callList[g_currentCall];
}

void CCallInfoIter::Next()
{
    g_currentCall++;
}

TBool CCallInfoIter::IsDone() const
{
    
    return (g_currentCall == (g_callList.count()));
}

CCallRemotePartyInformation::CCallRemotePartyInformation() {}
CCallRemotePartyInformation::~CCallRemotePartyInformation() {}
CCallRemotePartyInformation * CCallRemotePartyInformation::NewL()
{
    return new (ELeave) CCallRemotePartyInformation;
}

MCallRemotePartyInfo const * CCallRemotePartyInformation::GetRemotePartyInformationL(TInt call)
{
    for (int i=0; i < g_callList.count(); i++) {
        if(g_callList[i].m_index == call)
            return &g_callList[i];
    }
    
    return NULL;
}

CSPSettings::CSPSettings() {}
CSPSettings::~CSPSettings() {}
CSPSettings * CSPSettings::NewLC()
{
    //User::Leave(KErrNotSupported);
    CSPSettings *self = new (ELeave) CSPSettings;
    CleanupStack::PushL(self);
    return self;
}

CSPEntry::CSPEntry() {}
CSPEntry::~CSPEntry() {}
CSPEntry * CSPEntry::NewLC()
{
    //User::Leave(KErrNotSupported);
    CSPEntry *self = new (ELeave) CSPEntry;
    CleanupStack::PushL(self);
    return self;
}

int CSPSettings::FindEntryL(TUint, class CSPEntry &)
{
    return KErrNone;
}

_LIT(KServiceName, "Test service");
class TDesC16 const & CSPEntry::GetServiceName() const
{
    return KServiceName;
}

CCallInformation * CCallInformation::NewL()
{
    return new (ELeave) CCallInformation;
}

CCallInformation::CCallInformation() {}
CCallInformation::~CCallInformation() {}

void CCallInformation::NotifyCallInformationChanges(class MCallInformationObserver &o)
{
    g_observer = &o;
}

QTM_USE_NAMESPACE
QTM_BEGIN_NAMESPACE

CCPCall::TCallState mapStatus(QTelephony::CallStatus status)
{
    CCPCall::TCallState newStatus = CCPCall::EStateIdle;
    switch (status) {
    case QTelephony::Idle: newStatus = CCPCall::EStateIdle; break;
    case QTelephony::Dialing: newStatus = CCPCall::EStateDialling; break;
    case QTelephony::Alerting: newStatus = CCPCall::EStateConnecting; break;
    case QTelephony::Connected: newStatus = CCPCall::EStateConnected; break;
    case QTelephony::Disconnecting: newStatus = CCPCall::EStateDisconnecting; break;
    case QTelephony::Incoming: newStatus = CCPCall::EStateRinging; break;
    case QTelephony::OnHold: newStatus = CCPCall::EStateHold; break;
    default: Q_ASSERT(false); break;
    }
    
    return newStatus;
}

CCPCall::TCallType mapType(QTelephony::CallType type)
{
    CCPCall::TCallType newType = CCPCall::ECallTypeCSVoice;
    switch (type) {
    case QTelephony::Any: Q_ASSERT(false); break;
    case QTelephony::Data: Q_ASSERT(false); break;
    case QTelephony::Other: Q_ASSERT(false); break;
    case QTelephony::Text: Q_ASSERT(false); break;
    case QTelephony::Video: newType = CCPCall::ECallTypeVideo; break;
    case QTelephony::Voice: newType = CCPCall::ECallTypeCSVoice; break;
    default: Q_ASSERT(false); break;
    }
    
    return newType;
}

int addCall(QString number, QTelephony::CallType type)
{
    int index = 0;
    if(g_callList.count())
        index = g_callList.last().m_index + 1;
    
    g_callList.append(CCall(number, index, mapType(type)));
    g_observer->CallInformationChanged();
    return index;
}

void removeCall(int call)
{
    for (int i=0; i < g_callList.count(); i++) {
        if(g_callList[i].m_index == call)
            g_callList.removeAt(i);
    }

    g_observer->CallInformationChanged();
}

void changeCall(int call, QTelephony::CallStatus status)
{
    for (int i=0; i < g_callList.count(); i++) {
        if(g_callList[i].m_index == call)
            g_callList[i].m_status = mapStatus(status);
    }
    
    g_observer->CallInformationChanged();
}

QTM_END_NAMESPACE
