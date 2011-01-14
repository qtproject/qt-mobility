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


#include <QDBusContext>

#include "qbluetoothlocaldevice.h"
#include "qbluetoothaddress.h"

#include "bluez/manager_p.h"
#include "bluez/adapter_p.h"
#include "bluez/agent_p.h"
#include "bluez/device_p.h"

QTM_BEGIN_NAMESPACE

#define AGENT_PATH "/qt/agent"

class QBluetoothLocalDevicePrivate : public QObject,
                                     protected QDBusContext

{
    Q_OBJECT

public:
    OrgBluezAdapterInterface *adapter;
    OrgBluezAgentAdaptor *agent;
    QBluetoothLocalDevice *q;
    QString agent_path;
    QBluetoothAddress address;
    QBluetoothLocalDevice::Pairing pairing;       

public Q_SLOTS: // METHODS
    void Authorize(const QDBusObjectPath &in0, const QString &in1);
    void Cancel();
    void ConfirmModeChange(const QString &in0);
    void DisplayPasskey(const QDBusObjectPath &in0, uint in1, uchar in2);
    void Release();
    void RequestConfirmation(const QDBusObjectPath &in0, uint in1);
    uint RequestPasskey(const QDBusObjectPath &in0);
    QString RequestPinCode(const QDBusObjectPath &in0);

    void pairingCompleted(QDBusPendingCallWatcher*);

    void sendPinCode(QString pin);
    void sendPasskey(quint32 key);

private:
    QDBusMessage passKeyMsg;
    QDBusMessage pinCodeMsg;

//signals:
//    void pairingFinished(const QBluetoothAddress &address, QBluetoothLocalDevice::Pairing pairing);
//    void pairingDisplayPinCode(const QBluetoothAddress &address, QString pin);

};


/*!
    Constructs a QBluetoothLocalDevice.
*/
QBluetoothLocalDevice::QBluetoothLocalDevice(QObject *parent)
:   QObject(parent)
{
    OrgBluezManagerInterface manager(QLatin1String("org.bluez"), QLatin1String("/"),
                                     QDBusConnection::systemBus());

    QDBusPendingReply<QDBusObjectPath> reply = manager.DefaultAdapter();
    reply.waitForFinished();
    if (reply.isError())
        return;

    OrgBluezAdapterInterface *adapter = new OrgBluezAdapterInterface(QLatin1String("org.bluez"),
                                                                     reply.value().path(),
                                                                     QDBusConnection::systemBus());
    qRegisterMetaType<QBluetoothAddress>("QBluetoothAddress");

    this->d_ptr = new QBluetoothLocalDevicePrivate;
    this->d_ptr->adapter = adapter;    
    this->d_ptr->q = this;
    this->d_ptr->agent = 0x0;

//    connect(d_ptr, SIGNAL(pairingFinished(const QBluetoothAddress&,QBluetoothLocalDevice::Pairing)), this, SIGNAL(pairingFinished(const QBluetoothAddress&,QBluetoothLocalDevice::Pairing)));
//    connect(this->d_ptr, SIGNAL(pairingDisplayPinCode(const QBluetoothAddress &,QString)),
//            this, SIGNAL(pairingDisplayPinCode(const QBluetoothAddress &,QString)));

    qsrand(QTime::currentTime().msec());
    this->d_ptr->agent_path = AGENT_PATH;
    this->d_ptr->agent_path.append(QString("/%1").arg(qrand()));

    qRegisterMetaType<QBluetoothLocalDevice::Pairing>("QBluetoothLocalDevice::Pairing");
}

QBluetoothLocalDevice::QBluetoothLocalDevice(const QBluetoothAddress &address, QObject *parent)
: QObject(parent)
{
    OrgBluezManagerInterface manager(QLatin1String("org.bluez"), QLatin1String("/"),
                                     QDBusConnection::systemBus());

    QDBusPendingReply<QList<QDBusObjectPath> > reply = manager.ListAdapters();
    reply.waitForFinished();
    if (reply.isError())
        return;


    foreach (const QDBusObjectPath &path, reply.value()) {
        OrgBluezAdapterInterface *adapter = new OrgBluezAdapterInterface(QLatin1String("org.bluez"),
                                                                         path.path(),
                                                                         QDBusConnection::systemBus());

        QDBusPendingReply<QVariantMap> reply = adapter->GetProperties();
        reply.waitForFinished();
        if (reply.isError())
            continue;

        QBluetoothAddress path_address(reply.value().value(QLatin1String("Address")).toString());

        if(path_address == address){
            this->d_ptr = new QBluetoothLocalDevicePrivate;
            this->d_ptr->adapter = adapter;
            break;
        }
        else {
            delete adapter;
        }
    }
}

QString QBluetoothLocalDevice::name() const
{    
    if (!d_ptr)
        return QString();

    QDBusPendingReply<QVariantMap> reply = d_ptr->adapter->GetProperties();
    reply.waitForFinished();
    if (reply.isError())
        return QString();

    return reply.value().value(QLatin1String("Name")).toString();
}

QBluetoothAddress QBluetoothLocalDevice::address() const
{
    if (!d_ptr)
        return QBluetoothAddress();

    QDBusPendingReply<QVariantMap> reply = d_ptr->adapter->GetProperties();
    reply.waitForFinished();
    if (reply.isError())
        return QBluetoothAddress();

    return QBluetoothAddress(reply.value().value(QLatin1String("Address")).toString());
}

void QBluetoothLocalDevice::powerOn()
{
    if (!d_ptr)
        return;

    d_ptr->adapter->SetProperty(QLatin1String("Powered"), QDBusVariant(QVariant::fromValue(true)));
}

void QBluetoothLocalDevice::setHostMode(QBluetoothLocalDevice::HostMode mode)
{
    if (!d_ptr)
        return;

    switch (mode) {
    case HostDiscoverable:
        d_ptr->adapter->SetProperty(QLatin1String("Powered"), QDBusVariant(QVariant::fromValue(true)));
        d_ptr->adapter->SetProperty(QLatin1String("Discoverable"),
                                QDBusVariant(QVariant::fromValue(true)));
        break;
    case HostConnectable:
        d_ptr->adapter->SetProperty(QLatin1String("Powered"), QDBusVariant(QVariant::fromValue(true)));
        d_ptr->adapter->SetProperty(QLatin1String("Discoverable"),
                                QDBusVariant(QVariant::fromValue(false)));
        break;
    case HostPoweredOff:
        d_ptr->adapter->SetProperty(QLatin1String("Powered"),
                                QDBusVariant(QVariant::fromValue(false)));
//        d->adapter->SetProperty(QLatin1String("Discoverable"),
//                                QDBusVariant(QVariant::fromValue(false)));
        break;
    }
}

QBluetoothLocalDevice::HostMode QBluetoothLocalDevice::hostMode() const
{
    if (!d_ptr)
        return HostPoweredOff;

    QDBusPendingReply<QVariantMap> reply = d_ptr->adapter->GetProperties();
    reply.waitForFinished();
    if (reply.isError())
        return HostPoweredOff;

    if (!reply.value().value(QLatin1String("Powered")).toBool())
        return HostPoweredOff;
    else if (reply.value().value(QLatin1String("Discoverable")).toBool())
        return HostDiscoverable;
    else if (reply.value().value(QLatin1String("Powered")).toBool())
        return HostConnectable;

    return HostPoweredOff;
}

QList<QBluetoothHostInfo> QBluetoothLocalDevice::allDevices()
{
    QList<QBluetoothHostInfo> localDevices;

    OrgBluezManagerInterface manager(QLatin1String("org.bluez"), QLatin1String("/"),
                                     QDBusConnection::systemBus());

    QDBusPendingReply<QList<QDBusObjectPath> > reply = manager.ListAdapters();
    reply.waitForFinished();
    if (reply.isError())
        return localDevices;


    foreach (const QDBusObjectPath &path, reply.value()) {
        QBluetoothHostInfo hostinfo;
        OrgBluezAdapterInterface adapter(QLatin1String("org.bluez"), path.path(),
                                         QDBusConnection::systemBus());

        QDBusPendingReply<QVariantMap> reply = adapter.GetProperties();
        reply.waitForFinished();
        if (reply.isError())
            continue;

        hostinfo.setAddress(QBluetoothAddress(reply.value().value(QLatin1String("Address")).toString()));
        hostinfo.setName(reply.value().value(QLatin1String("Name")).toString());

        localDevices.append(hostinfo);
    }

    return localDevices;
}

static inline OrgBluezDeviceInterface *getDevice(const QBluetoothAddress &address, QBluetoothLocalDevicePrivate *d_ptr){
    QDBusPendingReply<QDBusObjectPath> reply = d_ptr->adapter->FindDevice(address.toString());
    reply.waitForFinished();
    if(reply.isError()){
        qDebug() << Q_FUNC_INFO << "reply failed" << reply.error();
        return 0;
    }

    QDBusObjectPath path = reply.value();

    return new OrgBluezDeviceInterface(QLatin1String("org.bluez"), path.path(),
                                   QDBusConnection::systemBus());
}

void QBluetoothLocalDevice::requestPairing(const QBluetoothAddress &address, Pairing pairing)
{

    if(pairing == Paired || pairing == AuthorizedPaired) {

        d_ptr->address = address;
        d_ptr->pairing = pairing;

        if(!d_ptr->agent){
            d_ptr->agent = new OrgBluezAgentAdaptor(d_ptr);
            bool res = QDBusConnection::systemBus().registerObject(d_ptr->agent_path, d_ptr);
            if(!res){
                qDebug() << "Failed to register agent";
                return;
            }
        }

        Pairing current_pairing = pairingStatus(address);
        if(current_pairing == Paired && pairing == AuthorizedPaired){
            OrgBluezDeviceInterface *device = getDevice(address, d_ptr);
            if(!device)
                return;
            QDBusPendingReply<> deviceReply = device->SetProperty(QLatin1String("Trusted"), QDBusVariant(true));
            deviceReply.waitForFinished();
            if(deviceReply.isError()){
                qDebug() << Q_FUNC_INFO << "reply failed" << deviceReply.error();
                return;
            }
            delete device;
            QMetaObject::invokeMethod(this, "pairingFinished", Qt::QueuedConnection, Q_ARG(QBluetoothAddress, address),
                                      Q_ARG(QBluetoothLocalDevice::Pairing, QBluetoothLocalDevice::AuthorizedPaired));
        }
        else if(current_pairing == AuthorizedPaired && pairing == Paired){
            OrgBluezDeviceInterface *device = getDevice(address, d_ptr);
            if(!device)
                return;
            QDBusPendingReply<> deviceReply = device->SetProperty(QLatin1String("Trusted"), QDBusVariant(false));
            deviceReply.waitForFinished();
            if(deviceReply.isError()){
                qDebug() << Q_FUNC_INFO << "reply failed" << deviceReply.error();
                return;
            }
            delete device;
            QMetaObject::invokeMethod(this, "pairingFinished", Qt::QueuedConnection, Q_ARG(QBluetoothAddress, address),
                                      Q_ARG(QBluetoothLocalDevice::Pairing, QBluetoothLocalDevice::Paired));
        }
        else {
            QDBusPendingReply<QDBusObjectPath> reply =
                    d_ptr->adapter->CreatePairedDevice(address.toString(),
                                                       QDBusObjectPath(d_ptr->agent_path),
                                                       QString("NoInputNoOutput"));

            QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
            connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)), d_ptr, SLOT(pairingCompleted(QDBusPendingCallWatcher*)));

            if(reply.isError())
                qDebug() << Q_FUNC_INFO << reply.error() << d_ptr->agent_path;
        }
    }
    else if(pairing == Unpaired) {
        QDBusPendingReply<QDBusObjectPath> reply = this->d_ptr->adapter->FindDevice(address.toString());
        reply.waitForFinished();
        if(reply.isError()) {
            qDebug() << Q_FUNC_INFO << "failed to find device" << reply.error();
            return;
        }
        QDBusPendingReply<> removeReply = this->d_ptr->adapter->RemoveDevice(reply.value());
        removeReply.waitForFinished();
        if(removeReply.isError()){
            qDebug() << Q_FUNC_INFO << "failed to remove device" << removeReply.error();
        }
        return;
    }
    return;
}

QBluetoothLocalDevice::Pairing QBluetoothLocalDevice::pairingStatus(const QBluetoothAddress &address) const
{
    OrgBluezDeviceInterface *device = getDevice(address, d_ptr);

    if(!device)
        return Unpaired;

    QDBusPendingReply<QVariantMap> deviceReply = device->GetProperties();
    deviceReply.waitForFinished();
    if (deviceReply.isError())
        return Unpaired;

    QVariantMap map = deviceReply.value();

//    qDebug() << "Paired: " << map.value("Paired");


    if(map.value("Trusted").toBool() && map.value("Paired").toBool())
        return AuthorizedPaired;
    else if(map.value("Paired").toBool())
            return Paired;
    else
            return Unpaired;

}

void QBluetoothLocalDevice::sendPinCode(QString pin)
{
    d_ptr->sendPinCode(pin);
}

void QBluetoothLocalDevice::sendPasskey(quint32 key)
{
    d_ptr->sendPasskey(key);
}

void QBluetoothLocalDevicePrivate::Authorize(const QDBusObjectPath &in0, const QString &in1)
{
    qDebug() << "Got authorize for" << in0.path() << in1;
}

void QBluetoothLocalDevicePrivate::Cancel()
{
    qDebug() << "Got cancel";
    emit q->pairingCancelDialog(q->address());
}

void QBluetoothLocalDevicePrivate::ConfirmModeChange(const QString &in0)
{
    qDebug() << Q_FUNC_INFO << in0;
}

void QBluetoothLocalDevicePrivate::DisplayPasskey(const QDBusObjectPath &in0, uint in1, uchar in2)
{
    qDebug() << Q_FUNC_INFO << in0.path() << in1 << in2;
}

void QBluetoothLocalDevicePrivate::Release()
{
    qDebug() << Q_FUNC_INFO;
}

void QBluetoothLocalDevicePrivate::RequestConfirmation(const QDBusObjectPath &in0, uint in1)
{
    qDebug() << Q_FUNC_INFO << in0.path() << in1;
    emit q->pairingDisplayConfirmation(address, QString("%1").arg(in1));
    return;
}

uint QBluetoothLocalDevicePrivate::RequestPasskey(const QDBusObjectPath &in0)
{
    setDelayedReply(true);
    passKeyMsg = message();
    emit q->pairingRequestPasskey(address);
    return 0;
}

QString QBluetoothLocalDevicePrivate::RequestPinCode(const QDBusObjectPath &in0)
{
    qDebug() << Q_FUNC_INFO << in0.path();
    // seeded in constructor, 6 digit pin
    QString pin = QString("%1").arg(qrand()&1000000);
    pin = QString("%1").arg(pin, 6, '0');

    emit q->pairingDisplayPinCode(address, pin);
    return pin;
}

void QBluetoothLocalDevicePrivate::pairingCompleted(QDBusPendingCallWatcher *watcher)
{
    QDBusPendingReply<> reply = *watcher;

    if(reply.isError()) {
        qDebug() << Q_FUNC_INFO << "failed to create pairing" << reply.error();
        emit q->pairingFinished(address, QBluetoothLocalDevice::Unpaired);
        delete watcher;
        return;
    }

    QDBusPendingReply<QDBusObjectPath> findReply = adapter->FindDevice(address.toString());
    findReply.waitForFinished();
    if(findReply.isError()) {
        qDebug() << Q_FUNC_INFO << "failed to find device" << findReply.error();
        emit q->pairingFinished(address, QBluetoothLocalDevice::Unpaired);
        delete watcher;
        return;
    }

    OrgBluezDeviceInterface device(QLatin1String("org.bluez"), findReply.value().path(),
                                   QDBusConnection::systemBus());

    if(pairing == QBluetoothLocalDevice::AuthorizedPaired) {
        device.SetProperty(QLatin1String("Trusted"), QDBusVariant(QVariant(true)));
        emit q->pairingFinished(address, QBluetoothLocalDevice::AuthorizedPaired);
    }
    else {
        device.SetProperty(QLatin1String("Trusted"), QDBusVariant(QVariant(false)));
        emit q->pairingFinished(address, QBluetoothLocalDevice::Paired);
    }
    delete watcher;

}

void QBluetoothLocalDevicePrivate::sendPinCode(QString pin)
{
    QDBusMessage reply = pinCodeMsg.createReply(QVariant(pin));
    connection().send(reply);
}

void QBluetoothLocalDevicePrivate::sendPasskey(quint32 key)
{
    QDBusMessage reply = passKeyMsg.createReply(QVariant(key));
    connection().send(reply);
}

#include "qbluetoothlocaldevice_bluez.moc"

QTM_END_NAMESPACE
