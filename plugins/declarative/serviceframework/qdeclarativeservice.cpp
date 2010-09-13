/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdeclarativeservice_p.h"

QTM_BEGIN_NAMESPACE

/*!
    \qmlclass Service
    
    \brief The Service element holds an instance of a service object.
    \inherits QObject

    \ingroup qml-serviceframework

    The Service element is part of the \bold{QtMobility.serviceframework 1.1} module and
    provides a client instance of the service object by specifying the Service::interfaceName, 
    Service::serviceName and Service::versionNumber properties.

    \sa ServiceList
*/
QDeclarativeService::QDeclarativeService()
: serviceInstance(0)
{
    serviceManager = new QServiceManager();
}

QDeclarativeService::~QDeclarativeService()
{
    delete serviceInstance;
}

/*!
    \qmlproperty bool Service::valid

    This property holds whether a default service was found at the
    interface name and corresponds to QServiceInterfaceDescriptor::isValid(). 
*/
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

/*!
    \qmlproperty QString Service::interfaceName

    This property holds the interface name of the service that
    corresponds to QServiceInterfaceDescriptor::interfaceName(). 
*/
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

/*!
    \qmlproperty QString Service::serviceName

    This property holds the service name of the service that
    corresponds to QServiceInterfaceDescriptor::serviceName(). 
*/
QString QDeclarativeService::serviceName() const
{
    if (isValid())
        return m_descriptor.serviceName();
    else
        return "No Service";
}

/*!
    \qmlproperty QString Service::versionNumber

    This property holds the version number of the service that
    represents \i major.minor corresponding to QServiceInterfaceDescriptor::majorVersion() 
    and QServiceInterfaceDescriptor::minorVersion(). 
*/
QString QDeclarativeService::versionNumber() const
{
    if (isValid())
        return (QString::number(m_descriptor.majorVersion())+"."+QString::number(m_descriptor.minorVersion()));
    else
        return "0.0";
}

/*!
    \qmlproperty QObject* Service::serviceObject

    This property holds an instance of the service object which
    can be used to make metaobject calls to the service.  This
    corresponds to QServiceManager::loadInterface().
*/
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

/*!
    \qmlclass ServiceList
    
    \brief The ServiceList element holds a list of \l Service elements.
    \inherits QObject

    \ingroup qml-serviceframework

    The ServiceList element is part of the \bold{QtMobility.serviceframework 1.1} module and
    provides a list of Service elements at the interface ServiceList::interfaceName with
    minimum version match ServiceList::minVersion properties. This list can be used to 
    select the desired service and instantiate a service object for access via the QMetaObject.

    \sa Service
*/
QDeclarativeServiceList::QDeclarativeServiceList()
{
    serviceManager = new QServiceManager();
}

QDeclarativeServiceList::~QDeclarativeServiceList()
{
}

/*!
    \qmlproperty QString ServiceList::interfaceName

    This property holds the interface name of the services that
    corresponds to setting QServiceFilter::setInterface(). 
*/
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

/*!
    \qmlproperty QString ServiceList::minVersion

    This property holds the minimum version for matching service interface
    descriptors with QServiceFilter::MinimumVersionMatch.
*/
void QDeclarativeServiceList::setMinVersion(const QString &version)
{
    m_version = version;
}

QString QDeclarativeServiceList::minVersion() const
{
    return m_version;
}

/*!
    \qmlproperty QDeclarativeListProperty ServiceList::services

    This property holds the list of \l Service elements that match
    the Service::interfaceName and minimum Service::versionNumber properties. 
*/
QDeclarativeListProperty<QDeclarativeService> QDeclarativeServiceList::services()
{
    return QDeclarativeListProperty<QDeclarativeService>(this, m_services);
}

#include "moc_qdeclarativeservice_p.cpp"

QTM_END_NAMESPACE
