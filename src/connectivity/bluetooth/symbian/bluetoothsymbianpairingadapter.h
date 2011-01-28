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

    BluetoothSymbianPairingAdapter(const QBluetoothAddress &address, QObject *parent = 0);
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
