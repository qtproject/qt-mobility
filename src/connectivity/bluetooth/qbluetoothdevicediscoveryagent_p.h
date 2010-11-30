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

#ifndef QBLUETOOTHDEVICEDISCOVERYAGENT_P_H
#define QBLUETOOTHDEVICEDISCOVERYAGENT_P_H

#include "qbluetoothdevicediscoveryagent.h"

QTM_BEGIN_NAMESPACE

class QBluetoothDeviceDiscoveryAgentPrivate : public QObject
{
Q_OBJECT
public:
    QBluetoothDeviceDiscoveryAgentPrivate(QObject *parent) : QObject(parent)
    {
      connect(this, SIGNAL(deviceDiscovered(const QBluetoothDeviceInfo)), parent, SIGNAL(deviceDiscovered(const QBluetoothDeviceInfo)));
      connect(this, SIGNAL(finished()), parent, SIGNAL(finished()));
      connect(this, SIGNAL(error(QBluetoothDeviceDiscoveryAgent::Error)), parent, SIGNAL(error(QBluetoothDeviceDiscoveryAgent::Error)));      
      inquiryType = QBluetoothDeviceDiscoveryAgent::GeneralUnlimitedInquiry;
    }
    virtual ~QBluetoothDeviceDiscoveryAgentPrivate() { };

    virtual void start() = 0;
    virtual void stop() = 0;
    virtual bool isActive() const = 0;
    
    QList<QBluetoothDeviceInfo> discoveredDevices;
    QBluetoothDeviceDiscoveryAgent::InquiryType inquiryType;
    QBluetoothDeviceDiscoveryAgent *q;   
    
signals:
    void deviceDiscovered(const QBluetoothDeviceInfo &info);
    void finished();
    void error(QBluetoothDeviceDiscoveryAgent::Error error);

public:
    static QBluetoothDeviceDiscoveryAgentPrivate* constructPrivateObject(QBluetoothDeviceDiscoveryAgent *parent);

};

QTM_END_NAMESPACE

#endif
