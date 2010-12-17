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

#include "qbluetoothlocaldevice.h"
#include "utils_symbian_p.h"
#ifndef S60_50_PLATFORM
#include <btengsettings.h>
#endif
#include <bttypes.h>
#include <bt_subscribe.h>

#include <QtCore/QString>

QTM_BEGIN_NAMESPACE

class QBluetoothLocalDevicePrivate
{
    ~QBluetoothLocalDevicePrivate();
public:
#ifndef S60_50_PLATFORM
    CBTEngSettings *m_settings;
#endif
};

QBluetoothLocalDevicePrivate::~QBluetoothLocalDevicePrivate()
{
#ifndef S60_50_PLATFORM
    delete m_settings;
#endif
}


/*!
    Constructs a QBluetoothLocalDevice.
*/
QBluetoothLocalDevice::QBluetoothLocalDevice(QObject *parent)
:   QObject(parent)
{
    this->d_ptr = new QBluetoothLocalDevicePrivate;
#ifndef S60_50_PLATFORM
    CBTEngSettings *settings = NULL;
    TRAPD(err, settings = CBTEngSettings::NewL());
    if (!err) {
        this->d_ptr->m_settings = settings;
    }
#endif
}

QBluetoothLocalDevice::QBluetoothLocalDevice(const QBluetoothAddress &address, QObject *parent)
: QObject(parent)
{
    this->d_ptr = new QBluetoothLocalDevicePrivate;
#ifndef S60_50_PLATFORM
    CBTEngSettings *settings = NULL;
    TRAPD(err, settings = CBTEngSettings::NewL());
    if (!err) {
        TBuf<20> bluetoothAddress;
        TPckgBuf<TBTDevAddr> addressPackage;

        RProperty::Get(KUidSystemCategory, KPropertyKeyBluetoothGetLocalDeviceAddress, addressPackage);

        addressPackage().GetReadable(bluetoothAddress, KNullDesC, _L(":"), KNullDesC);

        if (address == QBluetoothAddress(s60DescToQString(bluetoothAddress)))
            this->d_ptr->m_settings = settings;
    }
#endif
}

QString QBluetoothLocalDevice::name() const
{
    if (!d_ptr)
        return QString();
#ifndef S60_50_PLATFORM
    QString name = QString();
    TBuf<256> localName;
    TInt errorCode = this->d_ptr->m_settings->GetLocalName(localName);
    if (errorCode == KErrNone) {
        name = s60DescToQString(localName);
    }
    return name;
#endif
}

QBluetoothAddress QBluetoothLocalDevice::address() const
{
    TBuf<20> bluetoothAddress;
    TPckgBuf<TBTDevAddr> addressPackage;

    RProperty::Get(KUidSystemCategory, KPropertyKeyBluetoothGetLocalDeviceAddress, addressPackage);

    addressPackage().GetReadable(bluetoothAddress, KNullDesC, _L(":"), KNullDesC);

    return QBluetoothAddress(s60DescToQString(bluetoothAddress));
}

void QBluetoothLocalDevice::powerOn()
{
    if (!d_ptr)
        return;
#ifndef S60_50_PLATFORM
    TBTPowerStateValue powerState;
    this->d_ptr->m_settings->GetPowerState(powerState);
    if (powerState == EBTPowerOff)
        this->d_ptr->m_settings->SetPowerState(EBTPowerOn);
#endif
}

void QBluetoothLocalDevice::setHostMode(QBluetoothLocalDevice::HostMode mode)
{
    if (!d_ptr)
        return;
#ifndef S60_50_PLATFORM
    switch (mode) {
        case HostPoweredOff:
            this->d_ptr->m_settings->SetPowerState(EBTPowerOff);
            break;
        case HostConnectable:
            TBTPowerStateValue powerState;
            this->d_ptr->m_settings->GetPowerState(powerState);
            if (powerState == EBTPowerOff)
                this->d_ptr->m_settings->SetPowerState(EBTPowerOn);

            this->d_ptr->m_settings->SetVisibilityMode(EBTVisibilityModeHidden);
            break;
        case HostDiscoverable:
            TBTPowerStateValue powerState;
            this->d_ptr->m_settings->GetPowerState(powerState);
            if (powerState == EBTPowerOff)
                this->d_ptr->m_settings->SetPowerState(EBTPowerOn);

            this->d_ptr->m_settings->SetVisibilityMode(EBTVisibilityModeGeneral);
            break;
    }
#endif
}

QBluetoothLocalDevice::HostMode QBluetoothLocalDevice::hostMode() const
{
    if (!d_ptr)
        return HostPoweredOff;
#ifndef S60_50_PLATFORM
    TBTVisibilityMode visibilityMode;
    this->d_ptr->m_settings->GetVisibilityMode(visibilityMode);

    switch (visibilityMode) {
        case EBTVisibilityModeHidden:
            return HostConnectable;
        case EBTVisibilityModeGeneral:
            return HostDiscoverable;
        case EBTVisibilityModeNoScans:
        case EBTVisibilityModeTemporary:
        case EBTVisibilityModeInquiryScanOnly:
            return HostPoweredOff;
    }
#endif
}

QList<QBluetoothHostInfo> QBluetoothLocalDevice::allDevices()
{
    QList<QBluetoothHostInfo> localDevices;
    QBluetoothHostInfo hostInfo;
#ifndef S60_50_PLATFORM
    CBTEngSettings *settings = NULL;
    TRAPD(err, settings = CBTEngSettings::NewL());
    if (!err) {
        TBuf<256> localName;
        settings->GetLocalName(localName);
        hostInfo.setName(s60DescToQString(localName));
        delete settings;
    }

    TBuf<20> bluetoothAddress;
    TPckgBuf<TBTDevAddr> addressPackage;
    RProperty::Get(KUidSystemCategory, KPropertyKeyBluetoothGetLocalDeviceAddress, addressPackage);
    addressPackage().GetReadable(bluetoothAddress, KNullDesC, _L(":"), KNullDesC);
    hostInfo.setAddress(QBluetoothAddress(s60DescToQString(bluetoothAddress)));

    localDevices.append(hostInfo);
#endif
    return localDevices;
}

void QBluetoothLocalDevice::requestPairing(const QBluetoothAddress &address, Pairing pairing)
{
    Q_UNUSED(address);
    Q_UNUSED(pairing);
}
QBluetoothLocalDevice::Pairing QBluetoothLocalDevice::pairingStatus(const QBluetoothAddress &address) const
{
    Q_UNUSED(address);

    return Unpaired;
}



QTM_END_NAMESPACE
