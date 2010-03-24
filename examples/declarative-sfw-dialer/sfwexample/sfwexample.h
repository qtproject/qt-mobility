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
#include "../../src/serviceframework/qserviceinterfacedescriptor.h"
#include "../../src/serviceframework/qservicemanager.h"
#include <qdeclarative.h>

QTM_USE_NAMESPACE

Q_DECLARE_METATYPE(QServiceInterfaceDescriptor)

//! [0]
class ServiceWrapper : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool isValid READ isValid);
    Q_PROPERTY(QString serviceName READ serviceName CONSTANT);
    Q_PROPERTY(QString interfaceName READ interfaceName CONSTANT);
    Q_PROPERTY(QString version READ version NOTIFY versionChanged);
//! [0]

public:
    ServiceWrapper();
    
    ~ServiceWrapper() ;

    bool isValid() const;

    QString serviceName() const;

    QString interfaceName() const;

    void setNativeDescriptor(const QServiceInterfaceDescriptor& desc);

    void setDescriptor(QVariant& newDescriptor);

    QString version() const;

    Q_INVOKABLE QObject* serviceObject();

private:
    QServiceInterfaceDescriptor m_descriptor;
    QObject* serviceInstance;
};

QML_DECLARE_TYPE(ServiceWrapper)

//! [1]
class ServiceRegister : public QObject {
    Q_OBJECT
    Q_PROPERTY(QList<ServiceWrapper*>* registeredservices READ registeredservices NOTIFY modelChanged CONSTANT);
//! [1]

public:
    ServiceRegister();
    ~ServiceRegister();
    
    QList<ServiceWrapper*> *registeredservices() {return &m_services; }

    void registerExampleServices();

    void unregisterExampleServices();

    void serviceStateChange(int state);

private:
    QServiceManager* serviceManager;
    QList<ServiceWrapper *> m_services;

signals:
    void modelChanged();
};

QML_DECLARE_TYPE(ServiceRegister)

