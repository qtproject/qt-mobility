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

#ifndef QBLUETOOTHLOCALDEVICE_P_H
#define QBLUETOOTHLOCALDEVICE_P_H

#include <qmobilityglobal.h>

#include "qbluetoothlocaldevice.h"
#include <btengsettings.h>

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

class QBluetoothAddress;

class QBluetoothLocalDevicePrivate: public MBTEngSettingsObserver
{
    Q_DECLARE_PUBLIC(QBluetoothLocalDevice)
public:
    QBluetoothLocalDevicePrivate();
    ~QBluetoothLocalDevicePrivate();

    static QString name();
    static QBluetoothAddress address();

    void powerOn();
    void powerOff();
    void setHostMode(QBluetoothLocalDevice::HostMode mode);
    QBluetoothLocalDevice::HostMode hostMode() const;

    void requestPairing(const QBluetoothAddress &address, QBluetoothLocalDevice::Pairing pairing);
    QBluetoothLocalDevice::Pairing pairingStatus(const QBluetoothAddress &address) const;

    void pairingConfirmation(bool confirmation);
    //slot exposed for the public api.
    void _q_pairingFinished(const QBluetoothAddress &address, QBluetoothLocalDevice::Pairing pairing);

private:
    //From MBTEngSettingsObserver
    void PowerStateChanged(TBTPowerStateValue aState);
    void VisibilityModeChanged(TBTVisibilityMode aState);

private:
    CBTEngSettings *m_settings;
    bool m_settingVisibility;

protected:
    QBluetoothLocalDevice *q_ptr;

};

QTM_END_NAMESPACE

QT_END_HEADER

#endif // QBLUETOOTHLOCALDEVICE_P_H
