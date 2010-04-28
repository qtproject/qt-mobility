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

#include "qbluetoothdevicediscoveryagent.h"
#include "qbluetoothdevicediscoveryagent_p.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QBluetoothDeviceDiscoveryAgent
    \brief The QBluetoothDeviceDiscoveryAgent class provides an API for discovering nearby
           Bluetooth devices.

    \ingroup connectivity-bluetooth
    \inmodule QtConnectivity

    To discovery nearby Bluetooth devices create an instance of QBluetoothDeviceDiscoveryAgent,
    connect to either the deviceDiscovered() or finished() signals and call start().

    \code
        QBluetoothDeviceDiscoveryAgent *discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
        connect(discoveryAgent, SIGNAL(deviceDiscovered(const QBluetoothDeviceInfo&)),
                this, SLOT(deviceDiscovered(const QBluetoothDeviceInfo&)));
        discoveryAgent->start();
    \endcode
*/

/*!
    \enum QBluetoothDeviceDiscoveryAgent::InquiryType

    This enum describes the inquiry type used when discovering Bluetooth devices.

    \value GeneralUnlimitedInquiry  A general unlimited inquiry. Discovers all visible Bluetooth
                                    devices in the local vicinity.
    \value LimitedInquiry           A limited inquiry. Only discovers devices that are in limited
                                    inquiry mode. Not all platforms support limited inquiry. If
                                    limited inquiry is requested on a platform that does not
                                    support it general unlimited inquiry we be used instead.
*/

/*!
    \fn void QBluetoothDeviceDiscoveryAgent::deviceDiscovered(const QBluetoothDeviceInfo &info)

    This signal is emitted when the Bluetooth device described by \a info is discovered.
*/

/*!
    \fn void QBluetoothDeviceDiscoveryAgent::finished(bool error)

    This signal is emitted when Bluetooth device discovery completes. \a error is true if an
    error caused Bluetooth device discovery to abort.
*/

/*!
    \fn bool QBluetoothDeviceDiscoveryAgent::isActive() const

    Returns true if the agent is currently discovering Bluetooth devices, other returns false.
*/


/*!
    Constructs a new Bluetooth device discovery agent with parent \a parent.
*/
QBluetoothDeviceDiscoveryAgent::QBluetoothDeviceDiscoveryAgent(QObject *parent)
: QObject(*new QBluetoothDeviceDiscoveryAgentPrivate, parent)
{
}

/*!
    \property QBluetoothDeviceDiscoveryAgent::inquiryType
    \brief type of inquiry scan to use when discovering devices

    This property affects the type of inquiry scan which is performed when discovering devices.

    By default, this property is set to GeneralUnlimitedInquiry.

    Not all platforms support LimitedInquiry.

    \sa InquiryType
*/
QBluetoothDeviceDiscoveryAgent::InquiryType QBluetoothDeviceDiscoveryAgent::inquiryType() const
{
    Q_D(const QBluetoothDeviceDiscoveryAgent);

    return d->inquiryType;
}

void QBluetoothDeviceDiscoveryAgent::setInquiryType(QBluetoothDeviceDiscoveryAgent::InquiryType type)
{
    Q_D(QBluetoothDeviceDiscoveryAgent);

    d->inquiryType = type;
}

/*!
    Returns a list of all discovered Bluetooth devices.
*/
QList<QBluetoothDeviceInfo> QBluetoothDeviceDiscoveryAgent::discoveredDevices() const
{
    Q_D(const QBluetoothDeviceDiscoveryAgent);

    return d->discoveredDevices;
}

/*!
    Starts Bluetooth device discovery, if it is not already started.

    The deviceDiscovered() signal is emitted as each device is discovered. The finished() signal
    is emitted once device discovery is complete.
*/
void QBluetoothDeviceDiscoveryAgent::start()
{
    Q_D(QBluetoothDeviceDiscoveryAgent);

    if (!isActive())
        d->start();
}

/*!
    Stops Bluetooth device discovery.
*/
void QBluetoothDeviceDiscoveryAgent::stop()
{
    Q_D(QBluetoothDeviceDiscoveryAgent);

    if (isActive())
        d->stop();
}

#include "moc_qbluetoothdevicediscoveryagent.cpp"

QTM_END_NAMESPACE
