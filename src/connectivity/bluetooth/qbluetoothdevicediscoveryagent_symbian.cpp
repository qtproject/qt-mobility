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

#include "qbluetoothdevicediscoveryagent_p.h"
#include "qbluetoothaddress.h"
#include "utils_symbian_p.h"

#include <QDebug>

QTM_BEGIN_NAMESPACE

_LIT(KBTLinkManagerTxt,"BTLinkManager");

QBluetoothDeviceDiscoveryAgentPrivate::QBluetoothDeviceDiscoveryAgentPrivate()
: CActive(EPriorityStandard)
{
    TInt result;

    /* connect to socker server */
    result = socketServer.Connect();
    if (result != KErrNone) {
        qWarning("RSocketServ.Connect() failed with error %d", result);
        return;
    }

    /* find Bluetooth protocol */
    TProtocolDesc protocol;
    TRAPD(error, socketServer.FindProtocol(KBTLinkManagerTxt(), protocol));
    if (error) {
        qWarning("RSocketServ.FindProtocol() failed with error %d", error);
        return;
    }

    /* open host resolver */
    TRAP(error, result = hostResolver.Open(socketServer, protocol.iAddrFamily, protocol.iProtocol));
    if (error || result != KErrNone) {
        qWarning("RHostResolver.Open() failed with errors %d %d", error, result);
        return;
    }

    /* add this active object to scheduler */
    CActiveScheduler::Add(this);
}

QBluetoothDeviceDiscoveryAgentPrivate::~QBluetoothDeviceDiscoveryAgentPrivate()
{
    /* cancel active object */
    Cancel();

    /* close resolver */
    hostResolver.Close();
}

void QBluetoothDeviceDiscoveryAgentPrivate::start()
{
    if (!IsActive()) {
        TInquirySockAddr address;

        if (inquiryType == QBluetoothDeviceDiscoveryAgent::LimitedInquiry)
            address.SetIAC(KLIAC);
        else
            address.SetIAC(KGIAC);

        address.SetAction(KHostResInquiry|KHostResName|KHostResIgnoreCache);

        hostResolver.GetByAddress(address, entry, iStatus);
        SetActive();
    }
}

void QBluetoothDeviceDiscoveryAgentPrivate::stop()
{
    DoCancel();
}

bool QBluetoothDeviceDiscoveryAgent::isActive() const
{
    Q_D(const QBluetoothDeviceDiscoveryAgent);

    return d->IsActive();
}

void QBluetoothDeviceDiscoveryAgentPrivate::RunL()
{
    Q_Q(QBluetoothDeviceDiscoveryAgent);
    if (iStatus == KErrNone) {
        QString deviceName = QString::fromUtf16(entry().iName.Ptr(), entry().iName.Length());

        QBluetoothDeviceInfo device(qTBTDevAddrToQBluetoothAddress(static_cast<TBTSockAddr>(entry().iAddr).BTAddr()), deviceName, 0);
        discoveredDevices.append(device);

        emit q->deviceDiscovered(device);

        hostResolver.Next(entry, iStatus);

        SetActive();
    } else if (iStatus == KErrEof) {
        emit q->finished(false);
    } else {
        qWarning("Bluetooth device scan error %d", iStatus.Int());
        emit q->finished(true);
    }
}

void QBluetoothDeviceDiscoveryAgentPrivate::DoCancel()
{
    hostResolver.Cancel();
}

QTM_END_NAMESPACE
