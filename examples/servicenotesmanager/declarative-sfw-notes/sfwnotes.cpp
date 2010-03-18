/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "sfwnotes.h"

ServiceWrapper::ServiceWrapper()
: serviceInstance(0)
{
    serviceManager = new QServiceManager(this);

    registerExampleServices();
}

ServiceWrapper::~ServiceWrapper()
{
    delete serviceInstance;
    unregisterExampleServices();
}

QString ServiceWrapper::interfaceName() const
{
    return m_interface;
}

void ServiceWrapper::setInterfaceName(const QString &interface)
{
    m_interface = interface;
}

QString ServiceWrapper::serviceName() const
{
    return m_service;
}

void ServiceWrapper::setServiceName(const QString &service)
{
    m_service = service;
}

QString ServiceWrapper::version() const
{
    return m_version;
}

void ServiceWrapper::setVersion(const QString &version)
{
    m_version = version;
}

bool ServiceWrapper::existsInterface()
{
    return (m_interface != "");
}

bool ServiceWrapper::existsService()
{
    return (m_service != "");
}

bool ServiceWrapper::existsVersion()
{
    return (m_version != "");
}

void ServiceWrapper::findServiceInterface()
{
    QServiceFilter filter;
    filter.setServiceName(m_service);
    
    if (existsInterface())
        filter.setInterface(m_interface, m_version, QServiceFilter::ExactVersionMatch);

    QList<QServiceInterfaceDescriptor> list = serviceManager->findInterfaces(filter);

    if (list.size() == 0) return;

    if (existsInterface()) {
        if (existsVersion()) {
            m_descriptor = list[0];
        } else {
            m_descriptor = serviceManager->interfaceDefault(m_interface);
        }

    } else if (existsService()) {
        if (existsVersion()) {
            for (int i=0; i<list.count(); i++) {
                QString currVersion = QString::number(list[i].majorVersion()) + "." + 
                                      QString::number(list[i].minorVersion());

                if ( currVersion == m_version) {
                    m_descriptor = list[i];
                    return;
                }
            }
        } else {
            int major = -1;
            int minor = -1;

            for (int i=0; i<list.count(); i++) {
                int currMajor = list[i].majorVersion();
                int currMinor = list[i].minorVersion();
                
                if (currMajor > major) {
                    major = currMajor;
                    minor = -1;
                }
                    
                if (currMinor > minor) {
                    minor = currMinor;
                    m_descriptor = list[i];
                }
            }
        }
    }
}

QObject* ServiceWrapper::serviceObject()
{
    findServiceInterface();

    if (serviceInstance) {
        return serviceInstance;
    }

    if (m_descriptor.isValid()) {
        QServiceManager manager;
        serviceInstance = manager.loadInterface(m_descriptor);
        return serviceInstance;
    } else {
        return 0;
    }
}

void ServiceWrapper::registerExampleServices()
{
    QStringList exampleXmlFiles;
    exampleXmlFiles << "notesmanagerservice.xml";
    foreach (const QString &fileName, exampleXmlFiles) {
        QString path = QCoreApplication::applicationDirPath() + "/xmldata/" + fileName;
        serviceManager->addService(path);
    }
}

void ServiceWrapper::unregisterExampleServices()
{
    serviceManager->removeService("NotesManagerService");
}
