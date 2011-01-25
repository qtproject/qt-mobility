/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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
#include "symbian/utils_symbian_p.h"
#include <btengsettings.h>
#include <bttypes.h>
#include <bt_subscribe.h>

#include <QtCore/QString>

QTM_BEGIN_NAMESPACE

class QBluetoothLocalDevicePrivate: public MBTEngSettingsObserver
{
    Q_DECLARE_PUBLIC(QBluetoothLocalDevice)
public:
    QBluetoothLocalDevicePrivate();
    ~QBluetoothLocalDevicePrivate();

    static QString name();
    static QBluetoothAddress address();

    void powerOn();
    void setHostMode(QBluetoothLocalDevice::HostMode mode);
    QBluetoothLocalDevice::HostMode hostMode() const;

private:
    //From MBTEngSettingsObserver
    void PowerStateChanged(TBTPowerStateValue aState);
    void VisibilityModeChanged(TBTVisibilityMode aState);

public:
    CBTEngSettings *m_settings;

protected:
    QBluetoothLocalDevice *q_ptr;
};

QBluetoothLocalDevicePrivate::QBluetoothLocalDevicePrivate()
:m_settings(NULL)
{
    TRAPD(err, m_settings = CBTEngSettings::NewL(this));
    if (err != KErrNone)
        m_settings = NULL;
}

QBluetoothLocalDevicePrivate::~QBluetoothLocalDevicePrivate()
{
    delete m_settings;
}

QString QBluetoothLocalDevicePrivate::name()
{
    CBTEngSettings *settings = NULL;
    TRAPD(err, settings = CBTEngSettings::NewL());
    if (err != KErrNone)
        return QString();
    QString name = QString();
    HBufC *localName = NULL;
    TRAPD(error, localName = HBufC::NewL(256));
    if (error != KErrNone) {
        delete settings;
        return QString();
    }
    TPtr localPtr = localName->Des();
    TInt errorCode = settings->GetLocalName(localPtr);
    if (errorCode == KErrNone) {
        name = s60DescToQString(localPtr);
    }
    delete localName;
    delete settings;
    return name;
}

QBluetoothAddress QBluetoothLocalDevicePrivate::address()
{
    TBuf<20> bluetoothAddress;
    TPckgBuf<TBTDevAddr> addressPackage;

    TInt error = RProperty::Get(KUidSystemCategory, KPropertyKeyBluetoothGetLocalDeviceAddress, addressPackage);

    if (error != KErrNone)
        return QBluetoothAddress();

    addressPackage().GetReadable(bluetoothAddress, KNullDesC, _L(":"), KNullDesC);

    return QBluetoothAddress(s60DescToQString(bluetoothAddress));
}

void QBluetoothLocalDevicePrivate::powerOn()
{
    if (!m_settings)
        return;
    TBTPowerStateValue powerState;
    TInt error = m_settings->GetPowerState(powerState);
    if (error == KErrNone && powerState == EBTPowerOff)
        m_settings->SetPowerState(EBTPowerOn);
}

void QBluetoothLocalDevicePrivate::setHostMode(QBluetoothLocalDevice::HostMode mode)
{
    if (!m_settings)
        return;
    switch (mode) {
        case QBluetoothLocalDevice::HostPoweredOff:
            m_settings->SetPowerState(EBTPowerOff);
            break;
        case QBluetoothLocalDevice::HostConnectable: {
            TBTPowerStateValue powerState;
            TInt error = m_settings->GetPowerState(powerState);
            if (error == KErrNone) {
                if (powerState == EBTPowerOff)
                    m_settings->SetPowerState(EBTPowerOn);
                m_settings->SetVisibilityMode(EBTVisibilityModeHidden);
            }
            break;
        }
        case QBluetoothLocalDevice::HostDiscoverable: {
            TBTPowerStateValue powerState;
            TInt error = m_settings->GetPowerState(powerState);
            if (error == KErrNone) {
                if (powerState == EBTPowerOff)
                    m_settings->SetPowerState(EBTPowerOn);
                m_settings->SetVisibilityMode(EBTVisibilityModeGeneral);
            }
            break;
        }
    }
}


QBluetoothLocalDevice::HostMode QBluetoothLocalDevicePrivate::hostMode() const
{
    if (m_settings)
        return QBluetoothLocalDevice::HostPoweredOff;

    TBTVisibilityMode visibilityMode;
    TInt error = m_settings->GetVisibilityMode(visibilityMode);

    if (error != KErrNone)
        return QBluetoothLocalDevice::HostPoweredOff;

    switch (visibilityMode) {
        case EBTVisibilityModeHidden:
            return QBluetoothLocalDevice::HostConnectable;
        case EBTVisibilityModeGeneral:
        case EBTVisibilityModeTemporary:
            return QBluetoothLocalDevice::HostDiscoverable;
        default:
            // default value, also includes these new values from Symbian^3 onwards
            // case EBTVisibilityModeNoScans:
            // case EBTVisibilityModeInquiryScanOnly:
            return QBluetoothLocalDevice::HostPoweredOff;
    }
}

void QBluetoothLocalDevicePrivate::PowerStateChanged(TBTPowerStateValue aState)
{
    Q_Q(QBluetoothLocalDevice);

    QBluetoothLocalDevice::HostMode hostMode;
    switch (aState) {
        case EBTPowerOn:
            hostMode = this->hostMode();
            break;
        case EBTPowerOff:
        default:
            hostMode = QBluetoothLocalDevice::HostPoweredOff;
            break;
    }
    emit q->hostModeStateChanged(hostMode);
}

void QBluetoothLocalDevicePrivate::VisibilityModeChanged(TBTVisibilityMode aState)
{
    Q_Q(QBluetoothLocalDevice);

    QBluetoothLocalDevice::HostMode hostMode;
    switch (aState) {
        case EBTVisibilityModeHidden:
            hostMode = QBluetoothLocalDevice::HostConnectable;
            break;
        case EBTVisibilityModeGeneral:
        case EBTVisibilityModeTemporary:
            hostMode = QBluetoothLocalDevice::HostDiscoverable;
            break;
        default:
            // default value, also includes these new values from Symbian^3 onwards
            // case EBTVisibilityModeNoScans:
            // case EBTVisibilityModeInquiryScanOnly:
            hostMode = QBluetoothLocalDevice::HostPoweredOff;
            break;
    }
    emit q->hostModeStateChanged(hostMode);
}


/*!
    Constructs a QBluetoothLocalDevice.
*/
QBluetoothLocalDevice::QBluetoothLocalDevice(QObject *parent)
:   QObject(parent)
{
    this->d_ptr = new QBluetoothLocalDevicePrivate;
    if (this->d_ptr->m_settings == NULL) {
        delete this->d_ptr;
        this->d_ptr = NULL;
    }
}

QBluetoothLocalDevice::QBluetoothLocalDevice(const QBluetoothAddress &address, QObject *parent)
: QObject(parent)
{
    this->d_ptr = new QBluetoothLocalDevicePrivate;
    if (this->d_ptr->m_settings == NULL || address != this->d_ptr->address()) {
        delete this->d_ptr;
        this->d_ptr = NULL;
    }
}

QString QBluetoothLocalDevice::name() const
{
    if (!d_ptr)
        return QString();
    return d_ptr->name();
}

QBluetoothAddress QBluetoothLocalDevice::address() const
{
    if (!d_ptr)
        return QBluetoothAddress();
    return d_ptr->address();
}

void QBluetoothLocalDevice::powerOn()
{
    if (!d_ptr)
        return;
    d_ptr->powerOn();
}

void QBluetoothLocalDevice::setHostMode(QBluetoothLocalDevice::HostMode mode)
{
    if (!d_ptr)
        return;
    d_ptr->setHostMode(mode);
}

QBluetoothLocalDevice::HostMode QBluetoothLocalDevice::hostMode() const
{
    if (!d_ptr)
        return HostPoweredOff;
    return d_ptr->hostMode();
}

QList<QBluetoothHostInfo> QBluetoothLocalDevice::allDevices()
{
    QList<QBluetoothHostInfo> localDevices;
    QBluetoothHostInfo hostInfo;
    hostInfo.setName(QBluetoothLocalDevicePrivate::name());
    hostInfo.setAddress(QBluetoothLocalDevicePrivate::address());
    localDevices.append(hostInfo);
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

void QBluetoothLocalDevice::pairingConfirmation(bool confirmation)
{

}

QTM_END_NAMESPACE
