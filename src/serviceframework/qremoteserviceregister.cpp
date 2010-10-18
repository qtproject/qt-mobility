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

    This class registers and publishes IPC based service objects. It owns each created service object instance and
    ensures that the platform specific IPC mechanism publishes the required service
    object to other processes in the system.
*/

/*! \typedef QRemoteServiceRegister::SecurityFilter
    QRemoteServiceRegister -style synonym for bool (*SecurityFilter)(const void *message)
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
    Publishes every service entry that has been registered using
    \l registerService(). \a ident is the service specific
    IPC address under which the service can be reached. This address must match
    the address provided in the services xml descriptor (see <filepath> tag).
*/
void QRemoteServiceRegister::publishEntries( const QString& ident)
{
    d->publishServices(ident);
}

/*!
    \property QRemoteServiceRegister::quitOnLastInstanceClosed
*/
bool QRemoteServiceRegister::quitOnLastInstanceClosed() const
{
    return d->quitOnLastInstanceClosed();
}

void QRemoteServiceRegister::setQuitOnLastInstanceClosed(bool quit)
{
    d->setQuitOnLastInstanceClosed(quit);
}

QRemoteServiceRegister::SecurityFilter QRemoteServiceRegister::setSecurityFilter(QRemoteServiceRegister::SecurityFilter filter)
{
    return d->setSecurityFilter(filter);
}

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
