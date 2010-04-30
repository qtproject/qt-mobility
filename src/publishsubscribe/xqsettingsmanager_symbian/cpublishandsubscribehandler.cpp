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

#include "cpublishandsubscribehandler.h"
#include <e32property.h>

#include "xqsettingskey.h"

CPublishAndSubscribeHandler* CPublishAndSubscribeHandler::NewL(TUid aUid)
{
    CPublishAndSubscribeHandler* self = new (ELeave) CPublishAndSubscribeHandler(aUid); 
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

void CPublishAndSubscribeHandler::ConstructL()
{
}

CPublishAndSubscribeHandler::CPublishAndSubscribeHandler(TUid aUid)
    : m_uid(aUid)
{
}

CPublishAndSubscribeHandler::~CPublishAndSubscribeHandler()
{
}

void CPublishAndSubscribeHandler::setObserver(MSettingsHandlerObserver* observer)
{
    m_observer = observer;
}

TInt CPublishAndSubscribeHandler::getValue(unsigned long key, TInt& value)
{
    return RProperty::Get(m_uid, key, value);
}

TInt CPublishAndSubscribeHandler::getValue(unsigned long /*key*/, TReal& /*value*/)
{
    return KErrArgument;
}

void CPublishAndSubscribeHandler::getValueL(unsigned long key, RBuf8& value)
{
    TInt err = RProperty::Get(m_uid, key, value);
    if (err == KErrOverflow)
    {
        value.ReAllocL(RProperty::KMaxPropertySize);
        err = RProperty::Get(m_uid, key, value);
        if (err == KErrOverflow)
        {
            value.ReAllocL(RProperty::KMaxLargePropertySize);
            err = RProperty::Get(m_uid, key, value);
        }
    }
    User::LeaveIfError(err);
}

void CPublishAndSubscribeHandler::getValueL(unsigned long key, RBuf16& value)
{
    TInt err = RProperty::Get(m_uid, key, value);
    if (err == KErrOverflow)
    {
        value.ReAllocL(RProperty::KMaxPropertySize);
        err = RProperty::Get(m_uid, key, value);
        if (err == KErrOverflow)
        {
            value.ReAllocL(RProperty::KMaxLargePropertySize);
            err = RProperty::Get(m_uid, key, value);
        }
    }
    User::LeaveIfError(err);
}

TInt CPublishAndSubscribeHandler::setValue(unsigned long key, const TInt& value)
{
    return RProperty::Set(m_uid, key, value);
}

TInt CPublishAndSubscribeHandler::setValue(unsigned long /*key*/, const TReal& /*value*/)
{
    return KErrArgument;
}

TInt CPublishAndSubscribeHandler::setValue(unsigned long key, const TDesC8& value)
{
    return RProperty::Set(m_uid, key, value);
}

TInt CPublishAndSubscribeHandler::setValue(unsigned long key, const TDesC16& value)
{
    return RProperty::Set(m_uid, key, value);
}

TInt CPublishAndSubscribeHandler::defineProperty(unsigned long key, XQSettingsManager::Type type)
{
    switch (type)
    {
        case XQSettingsManager::TypeInt:
        {
            return RProperty::Define(m_uid, key, RProperty::EInt);
        }
        case XQSettingsManager::TypeString:
        {
            return RProperty::Define(m_uid, key, RProperty::EText);
        }
        case XQSettingsManager::TypeByteArray:
        {
            return RProperty::Define(m_uid, key, RProperty::EByteArray);
        }
        case XQSettingsManager::TypeDouble:
        default:
        {
            return KErrArgument;
        }
    }
}

TInt CPublishAndSubscribeHandler::defineProperty(unsigned long key, XQSettingsManager::Type type,
    const XQPublishAndSubscribeSecurityPolicy& readPolicy, const XQPublishAndSubscribeSecurityPolicy& writePolicy)
{
    switch (type)
    {
        case XQSettingsManager::TypeInt:
        {
            return RProperty::Define(m_uid, key, RProperty::EInt, symbianPolicy(readPolicy), symbianPolicy(writePolicy));
        }
        case XQSettingsManager::TypeString:
        {
            return RProperty::Define(m_uid, key, RProperty::EText, symbianPolicy(readPolicy), symbianPolicy(writePolicy));
        }
        case XQSettingsManager::TypeByteArray:
        {
            return RProperty::Define(m_uid, key, RProperty::EByteArray, symbianPolicy(readPolicy), symbianPolicy(writePolicy));
        }
        case XQSettingsManager::TypeDouble:
        default:
        {
            return KErrArgument;
        }
    }
}

TSecurityPolicy CPublishAndSubscribeHandler::symbianPolicy(const XQPublishAndSubscribeSecurityPolicy& policy)
{
    //Use constructor for EAlwaysFail or EAlwaysPass
    switch (policy.secPolicyType())
    {
        case XQPublishAndSubscribeSecurityPolicy::SecPolicyAlwaysFail:
        {
            return TSecurityPolicy(TSecurityPolicy::EAlwaysFail);
        }
        case XQPublishAndSubscribeSecurityPolicy::SecPolicyAlwaysPass:
        {
            return TSecurityPolicy(TSecurityPolicy::EAlwaysPass);
        }
        default:
        {
            break;
        }
    }
    
    TCapability capability1 = ECapability_None;
    TCapability capability2 = ECapability_None;
    TCapability capability3 = ECapability_None;
    TCapability capability4 = ECapability_None;
    TCapability capability5 = ECapability_None;
    TCapability capability6 = ECapability_None;
    TCapability capability7 = ECapability_None;
    
    const QList<XQPublishAndSubscribeSecurityPolicy::Capability>& capabilities = policy.capabilities();
    
    if (capabilities.count() > 0) capability1 = symbianCapability(capabilities[0]);
    if (capabilities.count() > 1) capability2 = symbianCapability(capabilities[1]);
    if (capabilities.count() > 2) capability3 = symbianCapability(capabilities[2]);
    if (capabilities.count() > 3) capability4 = symbianCapability(capabilities[3]);
    if (capabilities.count() > 4) capability5 = symbianCapability(capabilities[4]);
    if (capabilities.count() > 5) capability6 = symbianCapability(capabilities[5]);
    if (capabilities.count() > 6) capability7 = symbianCapability(capabilities[6]);
    
    long int secureId = policy.secureId().uid();
    if (secureId != -1)
    {
        //Use constructor for TSecureId + max 3 capabilities
        return TSecurityPolicy(TSecureId(secureId), capability1, capability2, capability3);
    }
    
    long int vendorId = policy.vendorId().uid();
    if (vendorId != -1)
    {
        //Use constructor for TVendorId + max 3 capabilities
        return TSecurityPolicy(TVendorId(vendorId), capability1, capability2, capability3);
    }
    
    if (capabilities.count() < 4)
    {
        //Use constructor for max 3 capabilities
        return TSecurityPolicy(capability1, capability2, capability3);
    }
    else
    {
        //Use constructor for max 7 capabilities
        return TSecurityPolicy(capability1, capability2, capability3, capability4, capability5, capability6, capability7);
    }
}

TCapability CPublishAndSubscribeHandler::symbianCapability(const XQPublishAndSubscribeSecurityPolicy::Capability& capability)
{
    switch (capability)
    {
        case XQPublishAndSubscribeSecurityPolicy::CapabilityTCB:                return ECapabilityTCB;
        case XQPublishAndSubscribeSecurityPolicy::CapabilityCommDD:             return ECapabilityCommDD;
        case XQPublishAndSubscribeSecurityPolicy::CapabilityPowerMgmt:          return ECapabilityPowerMgmt;
        case XQPublishAndSubscribeSecurityPolicy::CapabilityMultimediaDD:       return ECapabilityMultimediaDD;
        case XQPublishAndSubscribeSecurityPolicy::CapabilityReadDeviceData:     return ECapabilityReadDeviceData;
        case XQPublishAndSubscribeSecurityPolicy::CapabilityWriteDeviceData:    return ECapabilityWriteDeviceData;
        case XQPublishAndSubscribeSecurityPolicy::CapabilityDRM:                return ECapabilityDRM;
        case XQPublishAndSubscribeSecurityPolicy::CapabilityTrustedUI:          return ECapabilityTrustedUI;
        case XQPublishAndSubscribeSecurityPolicy::CapabilityProtServ:           return ECapabilityProtServ;
        case XQPublishAndSubscribeSecurityPolicy::CapabilityDiskAdmin:          return ECapabilityDiskAdmin;
        case XQPublishAndSubscribeSecurityPolicy::CapabilityNetworkControl:     return ECapabilityNetworkControl;
        case XQPublishAndSubscribeSecurityPolicy::CapabilityAllFiles:           return ECapabilityAllFiles;
        case XQPublishAndSubscribeSecurityPolicy::CapabilitySwEvent:            return ECapabilitySwEvent;
        case XQPublishAndSubscribeSecurityPolicy::CapabilityNetworkServices:    return ECapabilityNetworkServices;
        case XQPublishAndSubscribeSecurityPolicy::CapabilityLocalServices:      return ECapabilityLocalServices;
        case XQPublishAndSubscribeSecurityPolicy::CapabilityReadUserData:       return ECapabilityReadUserData;
        case XQPublishAndSubscribeSecurityPolicy::CapabilityWriteUserData:      return ECapabilityWriteUserData;
        case XQPublishAndSubscribeSecurityPolicy::CapabilityLocation:           return ECapabilityLocation;
        case XQPublishAndSubscribeSecurityPolicy::CapabilitySurroundingsDD:     return ECapabilitySurroundingsDD;
        case XQPublishAndSubscribeSecurityPolicy::CapabilityUserEnvironment:    return ECapabilityUserEnvironment;
        default:
        {
            break;
        }
    }
    return TCapability();
}

TInt CPublishAndSubscribeHandler::deleteProperty(unsigned long key)
{
    return RProperty::Delete(m_uid, key);
}

bool CPublishAndSubscribeHandler::handleStartMonitoring(const XQSettingsKey& key, XQSettingsManager::Type type, MSettingsHandlerObserver& observer, TInt& error)
{
    if (m_monitors.contains(key.key()))
    {
        error = KErrAlreadyExists;
        return false;
    }
    CPubSubMonitor* newMonitor = new CPubSubMonitor(key, type, observer);
    if (newMonitor)
    {
        m_monitors[key.key()] = newMonitor;
        error = newMonitor->StartMonitoring();
        return error == KErrNone;
    }
    error = KErrNoMemory;
    return false;
}

bool CPublishAndSubscribeHandler::handleStopMonitoring(const XQSettingsKey& key, TInt& error)
{
    if (!m_monitors.contains(key.key()))
    {
        error = KErrNotFound;
        return false;
    }
    const long int itemKey = key.key();
    CPubSubMonitor* monitor = m_monitors[itemKey];
    m_monitors.remove(itemKey);
    delete monitor;
    
    return error == KErrNone;
}
