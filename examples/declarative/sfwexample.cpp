/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in Technology Preview License Agreement accompanying
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "sfwexample.h"

QML_DEFINE_TYPE(0,0,0,0,Service,ServiceWrapper);

ServiceWrapper::ServiceWrapper() : serviceInstance(0)
{
}

ServiceWrapper::~ServiceWrapper() 
{
    if ( serviceInstance)
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

    QString output("No Service");
    if (isValid()) {
        return m_descriptor.serviceName();
        /*output = "%1 %2 %3"; 
        output = output.arg(m_descriptor.serviceName()).arg(m_descriptor.interfaceName());
        output = output.arg(QString::number(m_descriptor.majorVersion())+"."+QString::number(m_descriptor.minorVersion()));*/
    }
    //qDebug() << output;
    return output;
}

QString ServiceWrapper::version() const
{
    if (isValid())
        return QString(QString::number(m_descriptor.majorVersion())+"."+QString::number(m_descriptor.minorVersion()));
    else
        return QString("0.0");
}

QVariant ServiceWrapper::descriptor() const 
{ 
    return qVariantFromValue(m_descriptor); 
}

void ServiceWrapper::setNativeDescriptor(const QServiceInterfaceDescriptor& d)
{
    if (d == m_descriptor)
        return;
    
    m_descriptor = d;
    emit descriptorChanged();
    emit nameChanged();
    emit versionChanged();
    if (serviceInstance)
        delete serviceInstance;

    serviceInstance = 0;
}
void ServiceWrapper::setDescriptor(QVariant& newDescriptor)
{
    QServiceInterfaceDescriptor d = newDescriptor.value<QServiceInterfaceDescriptor>();
    setNativeDescriptor(d);
}


QObject* ServiceWrapper::serviceObject()
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



ServiceRegister::ServiceRegister() {
    
    serviceManager = new QServiceManager();
    registerExampleServices();

    ServiceWrapper *service;
    QList<QServiceInterfaceDescriptor> allImpl = serviceManager->findInterfaces();
    for (int i = 0; i<allImpl.count(); i++) {
        service = new ServiceWrapper();
        service->setNativeDescriptor(allImpl.at(i));
        //m_services.append(service);
        m_services.append(service);
    }
}

ServiceRegister::~ServiceRegister() {
    unregisterExampleServices();
}

void ServiceRegister::registerExampleServices()
{
    QStringList exampleXmlFiles;
    exampleXmlFiles << "filemanagerservice.xml" << "bluetoothtransferservice.xml";
    foreach (const QString &fileName, exampleXmlFiles) {
        QString path = QCoreApplication::applicationDirPath() + "/xmldata/" + fileName;            serviceManager->addService(path);
    }
}

void ServiceRegister::unregisterExampleServices()
{
    serviceManager->removeService("FileManagerService");
    serviceManager->removeService("BluetoothTransferService");
}


