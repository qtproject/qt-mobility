/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdeclarativeservice.h"

QServiceWrapper::QServiceWrapper()
: serviceInstance(0)
{
    serviceManager = new QServiceManager();
}

QServiceWrapper::~QServiceWrapper()
{
    delete serviceInstance;
}

bool QServiceWrapper::isValid() const
{
    return m_descriptor.isValid();
}

void QServiceWrapper::setInterfaceDesc(const QServiceInterfaceDescriptor &desc)
{
    if (desc == m_descriptor)
        return;

    m_descriptor = desc;

    if (serviceInstance)
        delete serviceInstance;

    serviceInstance = 0;
}

QServiceInterfaceDescriptor QServiceWrapper::interfaceDesc() const
{
    return m_descriptor;
}

void QServiceWrapper::setInterfaceName(const QString &interface)
{
    m_descriptor = serviceManager->interfaceDefault(interface);

    if (!isValid())
        qWarning() << "WARNING: No default service found for interface name: " << interface;
}

QString QServiceWrapper::interfaceName() const
{
    if (isValid())
        return m_descriptor.interfaceName();
    else
        return "No Interface";
}

QString QServiceWrapper::serviceName() const
{
    if (isValid())
        return m_descriptor.serviceName();
    else
        return "No Service";
}

QString QServiceWrapper::versionNumber() const
{
    if (isValid())
        return (QString::number(m_descriptor.majorVersion())+"."+QString::number(m_descriptor.minorVersion()));
    else
        return "0.0";
}

QObject* QServiceWrapper::serviceObject()
{
    if (serviceInstance) {
        return serviceInstance;
    }

    if (isValid()) {
        QServiceManager manager;
        serviceInstance = manager.loadInterface(m_descriptor);
        return serviceInstance;
    } else {
        return 0;
    }
}

QServiceListWrapper::QServiceListWrapper()
{
    serviceManager = new QServiceManager();
}

QServiceListWrapper::~QServiceListWrapper()
{
}

void QServiceListWrapper::setInterfaceName(const QString &interface)
{
    m_interface = interface;

    QServiceWrapper *service;
    QServiceFilter filter(m_interface, m_version);
    QList<QServiceInterfaceDescriptor> list = serviceManager->findInterfaces(filter);
    for (int i = 0; i < list.size(); i++) {
        service = new QServiceWrapper();
        service->setInterfaceDesc(list.at(i));
        m_services.append(service);
    }
}

QString QServiceListWrapper::interfaceName() const
{
    return m_interface;
}

void QServiceListWrapper::setMinVersion(const QString &version)
{
    m_version = version;
}

QString QServiceListWrapper::minVersion() const
{
    return m_version;
}

QDeclarativeListProperty<QServiceWrapper> QServiceListWrapper::services()
{
    return QDeclarativeListProperty<QServiceWrapper>(this, m_services);
}
