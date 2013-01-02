/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef BLUETOOTHSYMBIANPAIRINGADAPTER_H
#define BLUETOOTHSYMBIANPAIRINGADAPTER_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API. It exists purely as an
// implementation detail. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <qmobilityglobal.h>
#include <QtCore/QObject>

#include <e32base.h>
#include <btdevice.h>
#include <bt_sock.h>
#include <btengconnman.h>
#include "qbluetoothlocaldevice.h"

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

class QBluetoothAddress;

class BluetoothSymbianPairingAdapter : public QObject, public MBTEngConnObserver
{
    Q_OBJECT
public:

    explicit BluetoothSymbianPairingAdapter(const QBluetoothAddress &address, QObject *parent = 0);
    ~BluetoothSymbianPairingAdapter();

    void startPairing(QBluetoothLocalDevice::Pairing pairing);

    int errorCode() const;
    QString pairingErrorString() const;

public: //from MBTEngConnObserver
    virtual void ConnectComplete( TBTDevAddr& aAddr, TInt aErr,
                                       RBTDevAddrArray* aConflicts = NULL );
    virtual void DisconnectComplete( TBTDevAddr& aAddr, TInt aErr );

    virtual void PairingComplete( TBTDevAddr& aAddr, TInt aErr );

Q_SIGNALS: // SIGNALS
    void pairingFinished(const QBluetoothAddress &address, QBluetoothLocalDevice::Pairing pairing);
    void pairingDisplayPinCode(const QBluetoothAddress &address, QString pin);
    void pairingError(int errorCode);
private:

    //  socket server handle
    CBTEngConnMan *m_pairingEngine;
    const QBluetoothAddress &m_address;
    bool m_pairingOngoing;
    int m_errorCode;
    QString m_pairingErrorString;
};

QTM_END_NAMESPACE

QT_END_HEADER

#endif //BLUETOOTHSYMBIANPAIRINGADAPTER_H
