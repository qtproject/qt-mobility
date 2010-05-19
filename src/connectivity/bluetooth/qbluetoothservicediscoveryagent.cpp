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

#include "qbluetoothservicediscoveryagent.h"
#include "qbluetoothservicediscoveryagent_p.h"

#include "qbluetoothdevicediscoveryagent.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QBluetoothServiceDiscoveryAgent
    \brief The QBluetoothServiceDiscoveryAgent class provides an API for querying the services
           provided by a Bluetooth device.

    \ingroup connectivity-bluetooth
    \inmodule QtConnectivity

    To query the services provided by all contactable Bluetooth devices create an instance of
    QBluetoothServiceDiscoveryAgent, connect to either the serviceDiscovered() or finished()
    signals and call start().

    \snippet snippets/connectivity/servicediscovery.cpp Service discovery

    By default a minimal service discovery is performed. In this mode the QBluetotohServiceInfo
    objects returned are guaranteed to contain only device and service UUID information. Depending
    on platform and device capabilities other service information may also be available. For most
    use cases this is adequate as QBluetoothSocket::connectToService() will perform additional
    discovery if required.  If full service information is required pass \l FullDiscovery as the
    discoveryMode parameter to start().
*/

/*!
    \enum QBluetoothServiceDiscoveryAgent::Error

    This enum describes errors that can occur during service discovery.

    \value NoError              No error.
    \value Canceled             Service discovery was canceled.
    \value DeviceDiscoveryError Error occurred during device discovery.
    \value UnknownError         An unidentified error occurred.
*/

/*!
    \enum QBluetoothServiceDiscoveryAgent::DiscoveryMode

    This enum describes the service discovery mode.

    \value MinimalDiscovery     Performs a minimal service discovery. The QBluetoothServiceInfo
                                objects returned may be incomplete and are only guaranteed to
                                contain device and service UUID information.
    \value FullDiscovery        Performs a full service discovery.
*/

/*!
    \fn QBluetoothServiceDiscoveryAgent::serviceDiscovered(const QBluetoothServiceInfo &info)

    This signal is emitted when the Bluetooth service described by \a info is discovered.
*/

/*!
    \fn QBluetoothServiceDiscoveryAgent::finished()

    This signal is emitted when Bluetooth service discovery completes.
*/

/*!
    \fn void QBluetoothServiceDiscoveryAgent::error(QBluetoothServiceDiscoveryAgent::Error error)

    This signal is emitted when an error occurs. The \a error parameter describes the error that
    occurred.
*/

/*!
    Constructs a new QBluetoothServiceDiscoveryAgent with \a parent.
*/
QBluetoothServiceDiscoveryAgent::QBluetoothServiceDiscoveryAgent(QObject *parent)
: QObject(*new QBluetoothServiceDiscoveryAgentPrivate(QBluetoothAddress()), parent)
{
}

/*!
    Constructs a new QBluetoothServiceDiscoveryAgent for \a remoteAddress and with \a parent.

    If \a remoteAddress is invalid the agent will discover services on all contactable Bluetooth
    devices.
*/
QBluetoothServiceDiscoveryAgent::QBluetoothServiceDiscoveryAgent(const QBluetoothAddress &remoteAddress, QObject *parent)
: QObject(*new QBluetoothServiceDiscoveryAgentPrivate(remoteAddress), parent)
{
}

/*!
    Returns the list of all discovered services.
*/
QList<QBluetoothServiceInfo> QBluetoothServiceDiscoveryAgent::discoveredServices() const
{
    Q_D(const QBluetoothServiceDiscoveryAgent);

    return d->discoveredServices;
}
/*!
    Sets the UUID filter to \a uuids.  Only services matching the UUIDs in \a uuids will be
    returned.

    An empty UUID list is equivalent to a list containing only QBluetoothUuid::PublicBrowseGroup.

    \sa uuidFilter()
*/
void QBluetoothServiceDiscoveryAgent::setUuidFilter(const QList<QBluetoothUuid> &uuids)
{
    Q_D(QBluetoothServiceDiscoveryAgent);

    d->uuidFilter = uuids;
}

/*!
    This is an overloaded member function, provided for convenience.

    Sets the UUID filter to a list containing the single element \a uuid.

    \sa uuidFilter()
*/
void QBluetoothServiceDiscoveryAgent::setUuidFilter(const QBluetoothUuid &uuid)
{
    Q_D(QBluetoothServiceDiscoveryAgent);

    d->uuidFilter.clear();
    d->uuidFilter.append(uuid);
}

/*!
    Returns the UUID filter.

    \sa setUuidFilter()
*/
QList<QBluetoothUuid> QBluetoothServiceDiscoveryAgent::uuidFilter() const
{
    Q_D(const QBluetoothServiceDiscoveryAgent);

    return d->uuidFilter;
}

/*!
    Starts service discovery. \a mode specifies the type of service discovery to perform.
*/
void QBluetoothServiceDiscoveryAgent::start(DiscoveryMode mode)
{
    Q_D(QBluetoothServiceDiscoveryAgent);

    if (d->discoveryState() == QBluetoothServiceDiscoveryAgentPrivate::Inactive) {
        if (d->deviceAddress.isValid()) {
            d->discoveredDevices << QBluetoothDeviceInfo(d->deviceAddress, QString(), 0);
            d->startServiceDiscovery();
        } else {
            d->startDeviceDiscovery();
        }
    }
}

/*!
    Stops service discovery.
*/
void QBluetoothServiceDiscoveryAgent::stop()
{
    Q_D(QBluetoothServiceDiscoveryAgent);

    switch (d->discoveryState()) {
    case QBluetoothServiceDiscoveryAgentPrivate::DeviceDiscovery:
        d->stopDeviceDiscovery();
        break;
    case QBluetoothServiceDiscoveryAgentPrivate::ServiceDiscovery:
        d->stopServiceDiscovery();
    default:
        ;
    }

    d->discoveredDevices.clear();
}

/*!
    Clears the results of a previous service discovery.
*/
void QBluetoothServiceDiscoveryAgent::clear()
{
    Q_D(QBluetoothServiceDiscoveryAgent);

    d->discoveredDevices.clear();
    d->discoveredServices.clear();
    d->uuidFilter.clear();
}

/*!
    Returns true if service discovery is currently active, otherwise returns false.
*/
bool QBluetoothServiceDiscoveryAgent::isActive() const
{
    Q_D(const QBluetoothServiceDiscoveryAgent);

    return d->state != QBluetoothServiceDiscoveryAgentPrivate::Inactive;
}

/*!
    Returns the type of error that last occurred.
*/
QBluetoothServiceDiscoveryAgent::Error QBluetoothServiceDiscoveryAgent::error() const
{
    Q_D(const QBluetoothServiceDiscoveryAgent);

    return d->error;
}

/*!
    Returns a human-readable description of the last error that occurred.
*/
QString QBluetoothServiceDiscoveryAgent::errorString() const
{
    return QString();
}

/*!
    Starts device discovery.
*/
void QBluetoothServiceDiscoveryAgentPrivate::startDeviceDiscovery()
{
    Q_Q(QBluetoothServiceDiscoveryAgent);

    if (!deviceDiscoveryAgent) {
        deviceDiscoveryAgent = new QBluetoothDeviceDiscoveryAgent;
        QObject::connect(deviceDiscoveryAgent, SIGNAL(finished(bool)),
                         q, SLOT(_q_deviceDiscoveryFinished(bool)));
    }

    setDiscoveryState(DeviceDiscovery);

    deviceDiscoveryAgent->start();
}

/*!
    Stops device discovery.
*/
void QBluetoothServiceDiscoveryAgentPrivate::stopDeviceDiscovery()
{
    deviceDiscoveryAgent->stop();
    delete deviceDiscoveryAgent;
    deviceDiscoveryAgent = 0;

    setDiscoveryState(Inactive);
}

/*!
    Called when device discovery finishes.
*/
void QBluetoothServiceDiscoveryAgentPrivate::_q_deviceDiscoveryFinished()
{
    if (deviceDiscoveryAgent->error() != QBluetoothDeviceDiscoveryAgent::NoError) {
        error = QBluetoothServiceDiscoveryAgent::DeviceDiscoveryError;

        setDiscoveryState(Inactive);

        return;
    }

    discoveredDevices = deviceDiscoveryAgent->discoveredDevices();

    delete deviceDiscoveryAgent;
    deviceDiscoveryAgent = 0;

    startServiceDiscovery();
}

/*!
    Starts service discovery for the next device.
*/
void QBluetoothServiceDiscoveryAgentPrivate::startServiceDiscovery()
{
    Q_Q(QBluetoothServiceDiscoveryAgent);

    setDiscoveryState(ServiceDiscovery);

    if (discoveredDevices.isEmpty()) {
        setDiscoveryState(Inactive);
        emit q->finished();
        return;
    }

    setDiscoveryState(ServiceDiscovery);
    start(discoveredDevices.at(0).address());
}

/*!
    Stops service discovery.
*/
void QBluetoothServiceDiscoveryAgentPrivate::stopServiceDiscovery()
{
    stop();

    setDiscoveryState(Inactive);
}

void QBluetoothServiceDiscoveryAgentPrivate::_q_serviceDiscoveryFinished()
{
    discoveredDevices.removeFirst();

    startServiceDiscovery();
}

#include "moc_qbluetoothservicediscoveryagent.cpp"

QTM_END_NAMESPACE
