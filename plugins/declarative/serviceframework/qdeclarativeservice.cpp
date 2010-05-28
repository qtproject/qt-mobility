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

#include "qdeclarativeservice.h"

QDeclarativeService::QDeclarativeService()
: serviceInstance(0)
{
    serviceManager = new QServiceManager();
}

QDeclarativeService::~QDeclarativeService()
{
    delete serviceInstance;
}

bool QDeclarativeService::isValid() const
{
    return m_descriptor.isValid();
}

void QDeclarativeService::setInterfaceDesc(const QServiceInterfaceDescriptor &desc)
{
    if (desc == m_descriptor)
        return;

    m_descriptor = desc;

    if (serviceInstance)
        delete serviceInstance;

    serviceInstance = 0;
}

QServiceInterfaceDescriptor QDeclarativeService::interfaceDesc() const
{
    return m_descriptor;
}

void QDeclarativeService::setInterfaceName(const QString &interface)
{
    m_descriptor = serviceManager->interfaceDefault(interface);

    if (!isValid())
        qWarning() << "WARNING: No default service found for interface name: " << interface;
}

QString QDeclarativeService::interfaceName() const
{
    if (isValid())
        return m_descriptor.interfaceName();
    else
        return "No Interface";
}

QString QDeclarativeService::serviceName() const
{
    if (isValid())
        return m_descriptor.serviceName();
    else
        return "No Service";
}

QString QDeclarativeService::versionNumber() const
{
    if (isValid())
        return (QString::number(m_descriptor.majorVersion())+"."+QString::number(m_descriptor.minorVersion()));
    else
        return "0.0";
}

QObject* QDeclarativeService::serviceObject()
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

QDeclarativeServiceList::QDeclarativeServiceList()
{
    serviceManager = new QServiceManager();
}

QDeclarativeServiceList::~QDeclarativeServiceList()
{
}

void QDeclarativeServiceList::setInterfaceName(const QString &interface)
{
    m_interface = interface;

    // ![0]
    QDeclarativeService *service;
    QServiceFilter filter(m_interface, m_version);
    QList<QServiceInterfaceDescriptor> list = serviceManager->findInterfaces(filter);
    for (int i = 0; i < list.size(); i++) {
        service = new QDeclarativeService();
        service->setInterfaceDesc(list.at(i));
        m_services.append(service);
    }
    // ![0]
}

QString QDeclarativeServiceList::interfaceName() const
{
    return m_interface;
}

void QDeclarativeServiceList::setMinVersion(const QString &version)
{
    m_version = version;
}

QString QDeclarativeServiceList::minVersion() const
{
    return m_version;
}

QDeclarativeListProperty<QDeclarativeService> QDeclarativeServiceList::services()
{
    return QDeclarativeListProperty<QDeclarativeService>(this, m_services);
}
