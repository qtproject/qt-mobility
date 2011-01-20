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
#include <btengsettings.h>
#include <bttypes.h>
#include <bt_subscribe.h>
#include "bluetoothsymbianpairingadapter.h"

#include <QtCore/QString>

QTM_BEGIN_NAMESPACE

class QBluetoothLocalDevicePrivate
{
    ~QBluetoothLocalDevicePrivate();
public:
    CBTEngSettings *m_settings;
};

QBluetoothLocalDevicePrivate::~QBluetoothLocalDevicePrivate()
{
    delete m_settings;
}


/*!
    Constructs a QBluetoothLocalDevice.
*/
QBluetoothLocalDevice::QBluetoothLocalDevice(QObject *parent)
:   QObject(parent)
{
    this->d_ptr = new QBluetoothLocalDevicePrivate;
    CBTEngSettings *settings = NULL;
    TRAPD(err, settings = CBTEngSettings::NewL());
    if (!err) {
        this->d_ptr->m_settings = settings;
    }
}

QBluetoothLocalDevice::QBluetoothLocalDevice(const QBluetoothAddress &address, QObject *parent)
: QObject(parent)
{
    this->d_ptr = new QBluetoothLocalDevicePrivate;
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
}

QString QBluetoothLocalDevice::name() const
{
    if (!d_ptr)
        return QString();
    QString name = QString();
    TBuf<256> localName;
    TInt errorCode = this->d_ptr->m_settings->GetLocalName(localName);
    if (errorCode == KErrNone) {
        name = s60DescToQString(localName);
    }
    return name;
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
    TBTPowerStateValue powerState;
    this->d_ptr->m_settings->GetPowerState(powerState);
    if (powerState == EBTPowerOff)
        this->d_ptr->m_settings->SetPowerState(EBTPowerOn);
}

void QBluetoothLocalDevice::setHostMode(QBluetoothLocalDevice::HostMode mode)
{
    if (!d_ptr)
        return;
    switch (mode) {
        case HostPoweredOff:
            this->d_ptr->m_settings->SetPowerState(EBTPowerOff);
            break;
        case HostConnectable: {
            TBTPowerStateValue powerState;
            this->d_ptr->m_settings->GetPowerState(powerState);
            if (powerState == EBTPowerOff)
                this->d_ptr->m_settings->SetPowerState(EBTPowerOn);

            this->d_ptr->m_settings->SetVisibilityMode(EBTVisibilityModeHidden);
            break;
        }
        case HostDiscoverable: {
            TBTPowerStateValue powerState;
            this->d_ptr->m_settings->GetPowerState(powerState);
            if (powerState == EBTPowerOff)
                this->d_ptr->m_settings->SetPowerState(EBTPowerOn);

            this->d_ptr->m_settings->SetVisibilityMode(EBTVisibilityModeGeneral);
            break;
        }
    }
}

QBluetoothLocalDevice::HostMode QBluetoothLocalDevice::hostMode() const
{
    if (!d_ptr)
        return HostPoweredOff;

    TBTVisibilityMode visibilityMode;
    this->d_ptr->m_settings->GetVisibilityMode(visibilityMode);

    switch (visibilityMode) {
        case EBTVisibilityModeHidden:
            return HostConnectable;
        case EBTVisibilityModeGeneral:
            return HostDiscoverable;
        case EBTVisibilityModeTemporary:
            return HostPoweredOff;
        default:
            // default value, also includes these new values from Symbian^3 onwards
            // case EBTVisibilityModeNoScans:
            // case EBTVisibilityModeInquiryScanOnly:
        return HostPoweredOff;
    }
}

QList<QBluetoothHostInfo> QBluetoothLocalDevice::allDevices()
{
    QList<QBluetoothHostInfo> localDevices;
    QBluetoothHostInfo hostInfo;
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
    return localDevices;
}

void QBluetoothLocalDevice::requestPairing(const QBluetoothAddress &address, Pairing pairing)
{
    // pass this pointer so that adapter gets delete when this object is deleted. Not optimal though.
    BluetoothSymbianPairingAdapter *pairingAdapter = new BluetoothSymbianPairingAdapter(address,this);
    connect(pairingAdapter, SIGNAL(pairingFinished(const QBluetoothAddress&,QBluetoothLocalDevice::Pairing)),
        this, SIGNAL(pairingFinished(const QBluetoothAddress&,QBluetoothLocalDevice::Pairing)));

    switch (pairing) {
    case Unpaired:
        // this is async method
        pairingAdapter->removePairing();
        break;
    case Paired:
    case AuthorizedPaired:
        // this is async method
        pairingAdapter->startPairing(pairing);
        break;
    default:
        ASSERT(0);
        break;
    }
}
QBluetoothLocalDevice::Pairing QBluetoothLocalDevice::pairingStatus(const QBluetoothAddress &address) const
{
    QScopedPointer<BluetoothSymbianPairingAdapter> pairingAdapter (new BluetoothSymbianPairingAdapter(address));
    return pairingAdapter->pairingStatus();
}

QTM_END_NAMESPACE
