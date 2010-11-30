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


#ifndef QBLUETOOTHLOCALDEVICE_H
#define QBLUETOOTHLOCALDEVICE_H

#include <qmobilityglobal.h>

#include <QObject>
#include <QtCore/QList>
#include <QString>

#include "qbluetoothaddress.h"

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

class QBluetoothLocalDevicePrivate;

class Q_CONNECTIVITY_EXPORT QBluetoothHostInfo
{
public:
    QBluetoothHostInfo() { };
    QBluetoothHostInfo(const QBluetoothHostInfo &other) {
        m_address = other.m_address;
        m_name = other.m_name;
    };

    QBluetoothAddress getAddress() const { return m_address; }
    void setAddress(const QBluetoothAddress &address) { m_address = address; }

    QString getName() const { return m_name; }
    void setName(const QString &name){ m_name = name; }

private:
    QBluetoothAddress m_address;
    QString m_name;

};

class Q_CONNECTIVITY_EXPORT QBluetoothLocalDevice : public QObject
{
    Q_OBJECT

public:
    enum Pairing {
        Unpaired,
        Paired,
        AuthorizedPaired
    };

    enum HostMode {        
        HostPoweredOff,
        HostConnectable,
        HostDiscoverable
    };

    QBluetoothLocalDevice(QObject *parent = 0);
    QBluetoothLocalDevice(const QBluetoothAddress &address, QObject *parent = 0);
    virtual ~QBluetoothLocalDevice();

    bool isValid() const;

    void requestPairing(const QBluetoothAddress &address, Pairing pairing);
    Pairing pairingStatus(const QBluetoothAddress &address) const;

    void setHostMode(QBluetoothLocalDevice::HostMode mode);
    HostMode hostMode() const;

    void powerOn();

    QString name() const;
    QBluetoothAddress address() const;

    static QList<QBluetoothHostInfo> allDevices();

Q_SIGNALS:    
    void hostModeStateChanged(HostMode state);
    void pairingFinished(const QBluetoothAddress &address, Pairing pairing);

private:
    QBluetoothLocalDevicePrivate *d_ptr;
};

QTM_END_NAMESPACE

QT_END_HEADER

#endif // QBLUETOOTHLOCALDEVICE_H
