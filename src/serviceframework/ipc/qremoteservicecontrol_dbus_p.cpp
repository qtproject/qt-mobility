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

#include "qremoteservicecontrol_dbus_p.h"
#include "ipcendpoint_p.h"
#include "objectendpoint_dbus_p.h"

#include <QDataStream>
#include <QTimer>


QTM_BEGIN_NAMESPACE

//IPC based on QDBus

#define SERVER 0
#define CLIENT 1

class DBusEndPoint : public QServiceIpcEndPoint
{
    Q_OBJECT

public:
    DBusEndPoint(QDBusInterface* iface, int type, QObject* parent = 0)
        : QServiceIpcEndPoint(parent), interface(iface), endType(type)
    {
        Q_ASSERT(interface);
        interface->setParent(this);
        connect(interface, SIGNAL(packageReceived(QByteArray,int,QString)), this, SLOT(readPackage(QByteArray,int,QString)));
    }

    ~DBusEndPoint() 
    {
    }

    void testThis()
    {
        qDebug() << "HELLO";
    }

protected:
    void flushPackage(const QServicePackage& package)
    {
        QDBusConnection *connection = new QDBusConnection(QDBusConnection::sessionBus());
        if (!connection->isConnected()) {
            qWarning() << "Cannot connect to DBus";
        }
       
        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << package;

        packageId = package.d->messageId;
        interface->asyncCall("writePackage", block, endType, packageId);
    }

protected slots:
    void readPackage(const QByteArray &package, int type, const QString &id) {
        
        // Check the message id
        QDataStream data(package);
        QServicePackage pack;
        data >> pack;
    
        // Check that its of a client-server nature
        if (endType != type) {
            // Client to Server
            if (type != 0) {
                readIncoming(package);
            } else {
            // Server to Client
                if (id == packageId)
                    readIncoming(package);
            }
        }
    }

    void readIncoming(const QByteArray &package)
    {
        QDataStream data(package);
        QServicePackage pack;
        data >> pack;
    
        incoming.enqueue(pack);
        emit readyRead();
    }

private:
    QDBusInterface* interface;
    int endType;
    QString packageId;
};

class DBusSession: public QObject
{
    Q_OBJECT
public:
    DBusSession(); 
    ~DBusSession();

public slots:
    QByteArray writePackage(const QByteArray &package, int type, const QString &id) {
        emit packageReceived(package, type, id); 
        return package; 
    }

signals:
    void packageReceived(const QByteArray &package, int type, const QString &id);
};

DBusSession::DBusSession() 
{
}

DBusSession::~DBusSession() 
{
}

class DBusSessionAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.qtmobility.DBus")

public:
    DBusSessionAdaptor(QObject *parent);
    ~DBusSessionAdaptor();

public slots:
    QByteArray writePackage(const QByteArray &package, int type, const QString &id) { 
        QByteArray ret;
        QMetaObject::invokeMethod(parent(), "writePackage",
                                  Q_RETURN_ARG(QByteArray, ret),
                                  Q_ARG(QByteArray, package),
                                  Q_ARG(int, type),
                                  Q_ARG(QString, id));
        return ret;
    }

signals:
    void packageReceived(const QByteArray &package, int type, const QString &id);
};

DBusSessionAdaptor::DBusSessionAdaptor(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    setAutoRelaySignals(true);
}

DBusSessionAdaptor::~DBusSessionAdaptor()
{
}

QRemoteServiceControlPrivate::QRemoteServiceControlPrivate(QObject* parent)
    : QObject(parent)
{
}

void QRemoteServiceControlPrivate::publishServices(const QString& ident)
{
    createServiceEndPoint(ident);
}

/*!
    Creates endpoint on service side.
*/
bool QRemoteServiceControlPrivate::createServiceEndPoint(const QString& ident)
{
    InstanceManager *iManager = InstanceManager::instance();
    QList<QRemoteServiceIdentifier> list = iManager->allIdents();
    
    if (list.size() > 0) {
        QDBusConnection *connection = new QDBusConnection(QDBusConnection::sessionBus());
        if (!connection->isConnected()) {
            qWarning() << "Cannot connect to DBus";
            return 0;
        }

        // MAYBE A FOR-LOOP FOR EACH SERVICENAME (ie DBUSExample, IPCExample)

        // TODO: do we want to always re-register services to dbus?
        QString serviceName = "com.nokia.qtmobility.sfw." + list[0].name;
        connection->unregisterService(serviceName);

        connection->registerService(serviceName);
        if (!connection->registerService(serviceName)) {
            qWarning() << "Cannot register service to DBus";
            return 0;
        }

        // Register our DBusSession server/client
        DBusSession *session = new DBusSession();
        new DBusSessionAdaptor(session);

        QString path = "/" + list[0].interface + "/DBusSession";
        path.replace(QString("."), QString("/"));
        connection->registerObject(path, session);

        QDBusInterface *iface = new QDBusInterface(serviceName, path, "", QDBusConnection::sessionBus());
        if (!iface->isValid()) {
            qWarning() << "Cannot connect to remote service" << serviceName << path;;
            return 0;
        }
    
        DBusEndPoint* ipcEndPoint = new DBusEndPoint(iface, 0);
        ObjectEndPoint* endpoint = new ObjectEndPoint(ObjectEndPoint::Service, ipcEndPoint, this);
    
        return true;
    }
}

/*!
    Creates endpoint on client side.
*/
QObject* QRemoteServiceControlPrivate::proxyForService(const QRemoteServiceIdentifier& typeIdent, const QString& location)
{
    QString serviceName = "com.nokia.qtmobility.sfw." + typeIdent.name;
    QString path = "/" + typeIdent.interface + "/DBusSession";
    path.replace(QString("."), QString("/"));

    QDBusConnection *connection = new QDBusConnection(QDBusConnection::sessionBus());
    if (!connection->isConnected()) {
        qWarning() << "Cannot connect to DBus";
        return 0;
    }
   
    QDBusInterface *inface = new QDBusInterface(serviceName, path, "", QDBusConnection::sessionBus());
    if (!inface->isValid()) {
        qWarning() << "Cannot connect to remote service" << serviceName << path;
        return 0;
    }
    
    DBusEndPoint* ipcEndPoint = new DBusEndPoint(inface, 1);
    ObjectEndPoint* endPoint = new ObjectEndPoint(ObjectEndPoint::Client, ipcEndPoint);
   
    //QObject* proxy 
    QObject *proxy = endPoint->constructProxy(typeIdent);
      
    QObject::connect(proxy, SIGNAL(destroyed()), endPoint, SLOT(deleteLater()));
    return proxy;
}

#include "moc_qremoteservicecontrol_dbus_p.cpp"
#include "qremoteservicecontrol_dbus_p.moc"
QTM_END_NAMESPACE
