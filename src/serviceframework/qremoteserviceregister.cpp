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

#include "qremoteserviceregister.h"
#include "qremoteserviceregisterentry_p.h"
#include "ipc/instancemanager_p.h"
#include "qremoteserviceregister_p.h"

QTM_BEGIN_NAMESPACE

QRemoteServiceRegister::Entry::Entry()
{
    d = new QRemoteServiceRegisterEntryPrivate;
}

QRemoteServiceRegister::Entry::Entry(const Entry& other)
    : d(other.d)
{
}

QRemoteServiceRegister::Entry::~Entry()
{
}

bool QRemoteServiceRegister::Entry::isValid() const
{
    if (!d->iface.isEmpty() && !d->service.isEmpty()
            && !d->ifaceVersion.isEmpty() && d->cptr!=0 && d->meta!=0)
        return true;
    return false;
}

bool QRemoteServiceRegister::Entry::operator==(const Entry& other) const
{
    return d->service == other.d->service &&
           d->iface == other.d->iface &&
           d->ifaceVersion == other.d->ifaceVersion;
}

bool QRemoteServiceRegister::Entry::operator!=(const Entry& other) const
{
    return !(other == *this);
}

QRemoteServiceRegister::Entry &QRemoteServiceRegister::Entry::operator=(const Entry& other)
{
    d = other.d;
    return *this;
}

QString QRemoteServiceRegister::Entry::interfaceName() const
{
    return d->iface;
}

QString QRemoteServiceRegister::Entry::serviceName() const
{
    return d->service;
}

QString QRemoteServiceRegister::Entry::version() const
{
    return d->ifaceVersion;
}

const QMetaObject * QRemoteServiceRegister::Entry::metaObject() const
{
    return d->meta;
}

/*!
    Sets the instance type of the QRemoteServiceRegister::Entry Entry
*/
void QRemoteServiceRegister::Entry::setInstantiationType(QRemoteServiceRegister::InstanceType t)
{
    d->instanceType = t;
}

QRemoteServiceRegister::InstanceType QRemoteServiceRegister::Entry::instantiationType() const
{
    return d->instanceType;
}

/*!
    \class QRemoteServiceRegister
    \inmodule QtServiceFramework
    \ingroup servicefw
    \brief The QRemoteServiceRegister class manages instances of remote service objects.

    This class registers and publishes IPC based service objects. It owns each created service
    object instance and ensures that the platform specific IPC mechanism publishes the required
    service object to other processes in the system. 

    Note that in order for the remote services to be discoverable by QServiceManager each \l Entry
    must be registered with the information reflecting the service XML description.
    
    The following XML descriptor is used for subsequent examples. 

    \code
    <SFW version="1.1">
    <service>
        <name>MyService</name>
        <ipcaddress>my_service</ipcaddress>
        <description>My service example</description>
        <interface>
            <name>com.nokia.qt.example.myService</name>
            <version>1.0</version>
            <description>My private service</description>
            <capabilities></capabilities>
        </interface>
    </service>
    </SFW>
    \endcode

    The snippet belows demonstrates how an application can register the class \a MyClass 
    as a remote service, which is published and accessible to clients who wish to load 
    service object instances.

    \code
    int main(int argc, char** argv)
    {
        QCoreApplication app(argc, argv);

        QRemoteServiceRegister* serviceRegister = new QRemoteServiceRegister();

        QRemoteServiceRegister::Entry myService;
        myService = serviceRegister->createEntry<MyClass>(
            "MyService", "com.nokia.qt.example.myservice", "1.0");
        
        serviceRegister->publishEntries("my_service");

        delete serviceRegister;
        return app.exec();
    }
    \endcode

    By default all entries are created as \l GlobalInstance types but this can be set 
    by \l Entry. Once service entries are published to instance manager
    the register is no longer needed and can be removed.

    \sa Entry
*/

/*!
    \enum QRemoteServiceRegister::InstanceType
    Defines the two types of instances for a registration entry
    \value GlobalInstance     New requests for a service gets the same service instance
    \value PrivateInstance    New requests for a service gets a new service instance
*/

/*!
    \fn void QRemoteServiceRegister::instanceClosed(const QRemoteServiceRegister::Entry&)

    This signal is emitted whenever a created instance has been closed. This indicates
    that a connected client has either shutdown or released the loaded service object.
    
    The \l Entry is supplied to identify which registered service entry the closed 
    instance belonged to.

    \sa allInstancesClosed()
*/

/*!
    \fn void QRemoteServiceRegister::allInstancesClosed()

    This signal is emitted whenever all service instances have been closed. This indicates
    that the last connected client has either shutdown or released the loaded service object.
    
    \sa instanceClosed()
*/

/*! 
    \typedef QRemoteServiceRegister::CreateServiceFunc
    TODO: QObject* (*CreateServiceFunc)()
*/

/*! 
    \typedef QRemoteServiceRegister::SecurityFilter
    TODO: bool (*SecurityFilter)(const void *message)
*/

/*!
    Creates a service register instance with the given \a parent.
*/
QRemoteServiceRegister::QRemoteServiceRegister(QObject* parent)
    : QObject(parent)
{
    d = QRemoteServiceRegisterPrivate::constructPrivateObject(this);

    connect(InstanceManager::instance(), SIGNAL(allInstancesClosed()),
            this, SIGNAL(allInstancesClosed()));
    connect(InstanceManager::instance(), SIGNAL(instanceClosed(QRemoteServiceRegister::Entry)),
            this, SIGNAL(instanceClosed(QRemoteServiceRegister::Entry)));
}

/*!
    Destroys the service register instance
*/
QRemoteServiceRegister::~QRemoteServiceRegister()
{
}

/*!
    Publishes every service entry that has been created using \l createEntry(). The \a ident 
    is the service specific IPC address under which the service can be reached. 
    
    This address must match the address provided in the services XML descriptor, otherwise 
    the service will not be discoverable.

    \sa createEntry()
*/
void QRemoteServiceRegister::publishEntries(const QString& ident)
{
    d->publishServices(ident);
}

/*!
    \property QRemoteServiceRegister::quitOnLastInstanceClosed

    Holds the value for automatically exiting services if connected clients have closed their
    instances. By default this value is set to true.
*/
bool QRemoteServiceRegister::quitOnLastInstanceClosed() const
{
    return d->quitOnLastInstanceClosed();
}

void QRemoteServiceRegister::setQuitOnLastInstanceClosed(bool quit)
{
    d->setQuitOnLastInstanceClosed(quit);
}

/*! 
    Allows a security filter to be set which can access \l QRemoteServiceRegisterCredentials.
    The \l SecurityFilter takes a function pointer where the function code implements possible
    permission checks and returns true or false. If a connecting client fails the security
    filter it will be denied access and unable to obtain a valid service instance. 
    
    The following snippet is an example of how to use the security filter feature.
    
    \code
    bool myFunction(const void *p)
    {
        const QRemoteServiceRegisterCredentials *cred = 
            (const struct QRemoteServiceRegisterCredentials *)p;

        // deny the superuser
        if (cred->uid == 0)
            return false;

        return true;
    }
        
    int main(int argc, char** argv)
    {
        ...

        QRemoteServiceRegister* serviceRegister = new QRemoteServiceRegister();
        service->setSecurityFilter(myFunction);

        ...
    }
    \endcode
*/
QRemoteServiceRegister::SecurityFilter QRemoteServiceRegister::setSecurityFilter(QRemoteServiceRegister::SecurityFilter filter)
{
    return d->setSecurityFilter(filter);
}

/*!
    \fn QRemoteServiceRegister::createEntry(const QString& serviceName, const QString& interfaceName, const QString& version)

    Creates an entry on our remote instance manager. The \a serviceName, \a interfaceName and 
    \a version must match the service XML descriptor in order for the remote service to be 
    discoverable.

    \sa publishEntries()
*/
QRemoteServiceRegister::Entry QRemoteServiceRegister::createEntry(const QString& serviceName, const QString& interfaceName, const QString& version, QRemoteServiceRegister::CreateServiceFunc cptr, const QMetaObject* meta)
{
    if (serviceName.isEmpty()
            || interfaceName.isEmpty()
            || version.isEmpty() ) {
        qWarning() << "QRemoteServiceRegister::registerService: service name, interface name and version must be specified";
        return Entry();
    }

    Entry e;
    e.d->service = serviceName;
    e.d->iface = interfaceName;
    e.d->ifaceVersion = version;
    e.d->cptr = cptr;
    e.d->meta = meta;

    Q_ASSERT(InstanceManager::instance());
    InstanceManager::instance()->addType(e);

    return e;
}


#ifndef QT_NO_DATASTREAM
QDataStream& operator>>(QDataStream& s, QRemoteServiceRegister::Entry& entry) {
    //for now we only serialize version, iface and service name
    //neds to sync with qHash and operator==
    s >> entry.d->service >> entry.d->iface >> entry.d->ifaceVersion;
    return s;
}

QDataStream& operator<<(QDataStream& s, const QRemoteServiceRegister::Entry& entry) {
    //for now we only serialize version, iface and service name
    //neds to sync with qHash and operator==
    s << entry.d->service << entry.d->iface << entry.d->ifaceVersion;
    return s;
}
#endif

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug dbg, const QRemoteServiceRegister::Entry& entry) {
    dbg.nospace() << "QRemoteServiceRegister::Entry("
                  << entry.serviceName() << ", "
                  << entry.interfaceName() << ", "
                  << entry.version() << ")";
    return dbg.space();
}
#endif

#include "moc_qremoteserviceregister.cpp"

QTM_END_NAMESPACE
