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
***************************************************************************/

#include <QtCore>
#include <qserviceinterfacedescriptor.h>
#include <qservicemanager.h>
#include <qml.h>


Q_DECLARE_METATYPE(QServiceInterfaceDescriptor)


class ServiceWrapper : public QObject {
    Q_OBJECT
public:
    ServiceWrapper();
    ~ServiceWrapper() ;

    Q_PROPERTY(bool isValid READ isValid);
    bool isValid() const;

    Q_PROPERTY(QString serviceName READ serviceName CONSTANT);
    QString serviceName() const;

    Q_PROPERTY(QString interfaceName READ interfaceName CONSTANT);
    QString interfaceName() const;

    void setNativeDescriptor(const QServiceInterfaceDescriptor& desc)

    Q_PROPERTY(QVariant descriptor READ descriptor WRITE setDescriptor NOTIFY descriptorChanged);
    QVariant descriptor() const;
    void setDescriptor(QVariant& newDescriptor);

    Q_PROPERTY(QString version READ version NOTIFY versionChanged);
    QString version() const;


    Q_INVOKABLE QObject* serviceObject();
signals:
    void descriptorChanged();
    void nameChanged();
    void versionChanged();

private:
    QServiceInterfaceDescriptor m_descriptor;
    QObject* serviceInstance;
};

QML_DECLARE_TYPE(ServiceWrapper)

class ServiceRegister : public QObject{
    Q_OBJECT
public:
    ServiceRegister();
    ~ServiceRegister();
    
    Q_PROPERTY(QList<ServiceWrapper *> *services READ services CONSTANT);
    QList<ServiceWrapper *> *services() { return &m_services; }

    void registerExampleServices();

    void unregisterExampleServices();
private:
    QServiceManager* serviceManager;
    QList<ServiceWrapper *> m_services;
};



