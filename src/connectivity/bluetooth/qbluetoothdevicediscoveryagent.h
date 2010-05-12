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

#ifndef QBLUETOOTHDEVICEDISCOVERYAGENT_H
#define QBLUETOOTHDEVICEDISCOVERYAGENT_H

#include "qmobilityglobal.h"

#include <QObject>

#include "qbluetoothdeviceinfo.h"

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

class QBluetoothDeviceDiscoveryAgentPrivate;

class Q_CONNECTIVITY_EXPORT QBluetoothDeviceDiscoveryAgent : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QBluetoothDeviceDiscoveryAgent)
    Q_PROPERTY(QBluetoothDeviceDiscoveryAgent::InquiryType inquiryType READ inquiryType WRITE setInquiryType)

public:
    enum Error {
        NoError,
        Canceled,
        UnknownError = 100
    };

    enum InquiryType {
        GeneralUnlimitedInquiry,
        LimitedInquiry,
    };

    QBluetoothDeviceDiscoveryAgent(QObject *parent = 0);

    QBluetoothDeviceDiscoveryAgent::InquiryType inquiryType() const;
    void setInquiryType(QBluetoothDeviceDiscoveryAgent::InquiryType type);

    bool isActive() const;

    Error error() const;
    QString errorString() const;

    QList<QBluetoothDeviceInfo> discoveredDevices() const;

public slots:
    void start();
    void stop();

signals:
    void deviceDiscovered(const QBluetoothDeviceInfo &info);
    void finished();
    void error(QBluetoothDeviceDiscoveryAgent::Error error);

private:
    Q_PRIVATE_SLOT(d_func(), void _q_deviceFound(const QString &address, const QVariantMap &dict))
    Q_PRIVATE_SLOT(d_func(), void _q_propertyChanged(const QString &name, const QDBusVariant &value))
};

QTM_END_NAMESPACE

QT_END_HEADER

#endif
