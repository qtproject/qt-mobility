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

#ifndef QBLUETOOTHSERVICEDISCOVERYAGENT_P_H
#define QBLUETOOTHSERVICEDISCOVERYAGENT_P_H

#include <private/qobject_p.h>

#include "qbluetoothaddress.h"
#include "qbluetoothdeviceinfo.h"
#include "qbluetoothserviceinfo.h"
#include "qbluetoothservicediscoveryagent.h"

#include <QStack>

#ifdef Q_OS_SYMBIAN
#include <btsdp.h>
#endif

#ifndef QT_NO_DBUS
class OrgBluezManagerInterface;
class OrgBluezAdapterInterface;
class OrgBluezDeviceInterface;
class QDBusPendingCallWatcher;
class QXmlStreamReader;
#endif

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

class QBluetoothDeviceDiscoveryAgent;

class QBluetoothServiceDiscoveryAgentPrivate : public QObjectPrivate
#ifdef Q_OS_SYMBIAN
, public MSdpAgentNotifier, public MSdpAttributeValueVisitor
#endif
{
    Q_DECLARE_PUBLIC(QBluetoothServiceDiscoveryAgent)

public:
    enum DiscoveryState {
        Inactive,
        DeviceDiscovery,
        ServiceDiscovery,
    };

    QBluetoothServiceDiscoveryAgentPrivate(const QBluetoothAddress &address);
    ~QBluetoothServiceDiscoveryAgentPrivate();

    void startDeviceDiscovery();
    void stopDeviceDiscovery();
    void startServiceDiscovery();
    void stopServiceDiscovery();

    void setDiscoveryState(DiscoveryState s) { state = s; }
    DiscoveryState discoveryState() { return state; }

    // private slots
    void _q_deviceDiscoveryFinished();
    void _q_serviceDiscoveryFinished();
#ifndef QT_NO_DBUS
    void _q_discoveredServices(QDBusPendingCallWatcher *watcher);
#endif

#ifdef Q_OS_SYMBIAN
    /* MSdpAgentNotifier virtual functions */
    void NextRecordRequestComplete(TInt aError, TSdpServRecordHandle aHandle, TInt aTotalRecordsCount);
    void AttributeRequestResult(TSdpServRecordHandle aHandle, TSdpAttributeID aAttrID, CSdpAttrValue *aAttrValue);
    void AttributeRequestComplete(TSdpServRecordHandle, TInt aError);

    /* MSdpAttributeValueVisitor virtual functions */
    void VisitAttributeValueL(CSdpAttrValue &aValue, TSdpElementType aType);
    void StartListL(CSdpAttrValueList &);
    void EndListL();
#endif

private:
    void start(const QBluetoothAddress &address);
    void stop();

#ifdef Q_OS_SYMBIAN
    void initAgent(const QBluetoothAddress &address);
#elif !defined(QT_NO_DBUS)
    QVariant readAttributeValue(QXmlStreamReader &xml);
#endif

public:
    QBluetoothServiceDiscoveryAgent::Error error;

    QList<QBluetoothServiceInfo> discoveredServices;
    QList<QBluetoothDeviceInfo> discoveredDevices;

private:
    DiscoveryState state;
    QBluetoothAddress deviceAddress;
    QList<QBluetoothUuid> uuidFilter;

    QBluetoothDeviceDiscoveryAgent *deviceDiscoveryAgent;

#ifdef Q_OS_SYMBIAN
    CSdpAgent *sdpAgent;
    CSdpSearchPattern *filter;
    CSdpAttrIdMatchList *attributes;
    QBluetoothServiceInfo serviceInfo;
    TSdpAttributeID currentAttributeId;

    QStack<QVariant> stack;
#elif !defined(QT_NO_DBUS)
    OrgBluezManagerInterface *manager;
    OrgBluezAdapterInterface *adapter;
    OrgBluezDeviceInterface *device;
#endif
};

QTM_END_NAMESPACE

QT_END_HEADER

#endif
