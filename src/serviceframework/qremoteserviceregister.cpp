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

#if defined(Q_OS_SYMBIAN)
    #include "qremoteserviceregister_s60_p.h"
#elif defined(QT_NO_DBUS)
    #include "qremoteserviceregister_p.h"
#else
    #include "qremoteserviceregister_dbus_p.h"
#endif

QTM_BEGIN_NAMESPACE

QRemoteServiceRegister::Entry::Entry()
    : meta(0), cptr(0), instanceType(QRemoteServiceRegister::PrivateInstance)
{
}

QRemoteServiceRegister::Entry::Entry(const Entry& other)
    : iface(other.iface), service(other.service), ifaceVersion(other.ifaceVersion),
      meta(other.meta), cptr(other.cptr), instanceType(other.instanceType)
{
}

bool QRemoteServiceRegister::Entry::isValid() const
{
    if (!iface.isEmpty() && !service.isEmpty() 
            && !ifaceVersion.isEmpty() && cptr!=0 && meta!=0)
        return true;
    return false;
}

bool QRemoteServiceRegister::Entry::operator==(const Entry& other) const
{
    return service == other.service &&
           iface == other.iface &&
           ifaceVersion == other.ifaceVersion;
}

bool QRemoteServiceRegister::Entry::operator!=(const Entry& other) const
{
    return !(other == *this);
}

QRemoteServiceRegister::Entry &QRemoteServiceRegister::Entry::operator=(const Entry& other)
{
    service = other.service;
    iface = other.iface;
    ifaceVersion = other.ifaceVersion;
    meta = other.meta;
    cptr = other.cptr;
    instanceType = other.instanceType;
    return *this;
}

QString QRemoteServiceRegister::Entry::interfaceName() const
{
    return iface;
}

QString QRemoteServiceRegister::Entry::serviceName() const
{
    return service;
}

QString QRemoteServiceRegister::Entry::version() const
{
    return ifaceVersion;
}

const QMetaObject * QRemoteServiceRegister::Entry::metaObject() const
{
    return meta;
}

void QRemoteServiceRegister::Entry::setInstantiationType(QRemoteServiceRegister::InstanceType t)
{
    instanceType = t;
}

QRemoteServiceRegister::InstanceType QRemoteServiceRegister::Entry::instantiationType() const
{
    return instanceType;
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

/*! \typedef QRemoteServiceRegister::securityFilter
    QRemoteServiceRegister -style synonym for bool (*securityFilter)(const void *message)
*/

/*!
    Creates a service register instance with the given \a parent.
*/
QRemoteServiceRegister::QRemoteServiceRegister(QObject* parent)
    : QObject(parent)
{
    d = QRemoteServiceRegisterPrivate::constructPrivateObject(this);
    connect(d, SIGNAL(lastInstanceClosed()), this, SIGNAL(lastInstanceClosed()));
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

void QRemoteServiceRegister::registerEntry(const Entry& e)
{
    Q_ASSERT(InstanceManager::instance());
    InstanceManager::instance()->addType(e);
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

QRemoteServiceRegister::securityFilter QRemoteServiceRegister::setSecurityFilter(QRemoteServiceRegister::securityFilter filter)
{
    return d->setSecurityFilter(filter);
}


#include "moc_qremoteserviceregister.cpp"

QTM_END_NAMESPACE
