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

#include "sfwexample.h"

ServiceWrapper::ServiceWrapper() 
: serviceInstance(0)
{
}

ServiceWrapper::~ServiceWrapper()
{
    delete serviceInstance;
}

bool ServiceWrapper::isValid() const
{
    return m_descriptor.isValid();
}

QString ServiceWrapper::interfaceName() const
{
    if (isValid())
        return m_descriptor.interfaceName();
    else
        return "No Interface";
}

QString ServiceWrapper::serviceName() const
{
    if (isValid()) {
        return m_descriptor.serviceName();
    } else
        return "No Service";
}

QString ServiceWrapper::version() const
{
    if (isValid())
        return QString(QString::number(m_descriptor.majorVersion())+"."+QString::number(m_descriptor.minorVersion()));
    else
        return QString("0.0");
}

void ServiceWrapper::setNativeDescriptor(const QServiceInterfaceDescriptor &d)
{
    if (d == m_descriptor)
        return;

    m_descriptor = d;
    
    if (serviceInstance)
        delete serviceInstance;

    serviceInstance = 0;
}

void ServiceWrapper::setDescriptor(QVariant &newDescriptor)
{
    QServiceInterfaceDescriptor d = newDescriptor.value<QServiceInterfaceDescriptor>();
    setNativeDescriptor(d);
}


QObject* ServiceWrapper::serviceObject()
{
    qDebug() << "called serviceObject";
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

ServiceRegister::ServiceRegister()
{
    serviceManager = new QServiceManager();
    unregisterExampleServices();
    registerExampleServices();

    //! [1]
    ServiceWrapper *service;
    QServiceFilter filter("com.nokia.qt.examples.Dialer");
    QList<QServiceInterfaceDescriptor> allImpl = serviceManager->findInterfaces(filter);
    for (int i = 0; i < allImpl.count(); i++) {
        qDebug() << "Found service:" << allImpl.at(i).serviceName() << "(" << allImpl.at(i).interfaceName() << ")";
        service = new ServiceWrapper();
        service->setNativeDescriptor(allImpl.at(i));
        m_services.append(service);
    }
    //! [1]
}

ServiceRegister::~ServiceRegister() 
{
    unregisterExampleServices();
}

void ServiceRegister::registerExampleServices()
{
    //! [0]
    QStringList exampleXmlFiles;
    exampleXmlFiles << "voipdialerservice.xml" << "landlinedialerservice.xml";
    foreach (const QString &fileName, exampleXmlFiles) {
        QString path = QCoreApplication::applicationDirPath() + "/xmldata/" + fileName;
        serviceManager->addService(path);
    }
    //! [0]
}

void ServiceRegister::unregisterExampleServices()
{
    serviceManager->removeService("VoipDialer");
    serviceManager->removeService("LandlineDialer");
}



