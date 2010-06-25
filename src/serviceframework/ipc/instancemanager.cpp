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

#include <qmobilityglobal.h>
#include "instancemanager_p.h"

QTM_BEGIN_NAMESPACE

Q_GLOBAL_STATIC(InstanceManager, typeRegister);

InstanceManager* InstanceManager::instance()
{
    return typeRegister();
}

InstanceManager::InstanceManager()
{
}

InstanceManager::~InstanceManager()
{
    QList<QRemoteServiceIdentifier> allIdents = metaMap.keys();
    while (!allIdents.isEmpty()) {
        ServiceIdentDescriptor descr = metaMap.take(allIdents.takeFirst());
        if (descr.instanceType == QRemoteServiceClassRegister::SharedInstance) {
            if (descr.sharedInstance)
               descr.sharedInstance->deleteLater();
            descr.sharedInstance = 0;
        } else {
            QList<QUuid> allUuids = descr.individualInstances.keys();
            while (!allUuids.isEmpty()) {
                descr.individualInstances.take(allUuids.takeFirst())->deleteLater();
            }
        }
    }

}

bool InstanceManager::addType(const QMetaObject* meta, 
        QRemoteServiceClassRegister::CreateServiceFunc func, 
        QRemoteServiceClassRegister::TypeIdentFunc typeFunc,
        QRemoteServiceClassRegister::InstanceType type)
{
    QRemoteServiceIdentifier ident = (*typeFunc)();

    QMutexLocker ml(&lock);

    if (metaMap.contains(ident)) {
        qWarning() << "Service" << ident.first << "(" << ident.second << ")"
            << "already registered";
    } else {
        ServiceIdentDescriptor d;
        d.meta = meta;
        d.instanceType = type;
        d.create = func;
        metaMap.insert(ident, d);
        return true;
    }
    return false;
}

const QMetaObject* InstanceManager::metaObject(const QRemoteServiceIdentifier& ident) const
{
    QMutexLocker ml(&lock);
    if (metaMap.contains(ident)) {
        return metaMap[ident].meta;
    } else {
        return 0;
    }
}

QList<QRemoteServiceIdentifier> InstanceManager::allIdents() const
{
    QMutexLocker ml(&lock);
    return metaMap.keys();
}

/*!
    Instance manager takes ownership of service instance. Returns a null pointer
    if \a ident cannot be mapped to a known meta object. \a instanceId will
    contain the id for the new service instance.
*/
QObject* InstanceManager::createObjectInstance(const QRemoteServiceIdentifier& ident, QUuid& instanceId)
{
    instanceId = QUuid();
    QMutexLocker ml(&lock);
    if (!metaMap.contains(ident))
        return 0;

    QObject* service = 0;
    ServiceIdentDescriptor& descr = metaMap[ident];

    if (descr.instanceType == QRemoteServiceClassRegister::SharedInstance) {
        if (descr.sharedInstance) {
            service = descr.sharedInstance;
            instanceId = descr.sharedId;
            descr.sharedRefCount++;
        } else {
            service = (*descr.create)();
            if (!service)
                return 0;

            descr.sharedInstance = service;
            descr.sharedId = instanceId = QUuid::createUuid();
            descr.sharedRefCount = 1;
        }
    } else {
        service = (*descr.create)();
        if (!service)
            return 0;
        instanceId = QUuid::createUuid();
        descr.individualInstances.insert(instanceId, service);
    }

    return service;
}

/*!
    The associated service object will be deleted in the process.
*/
void InstanceManager::removeObjectInstance(const QRemoteServiceIdentifier& ident, const QUuid& instanceId)
{
    QMutexLocker ml(&lock);
    if (!metaMap.contains(ident))
        return;
    
    ServiceIdentDescriptor& descr = metaMap[ident];
    if (descr.instanceType == QRemoteServiceClassRegister::SharedInstance) {
        if (descr.sharedRefCount < 1)
            return;

        if (descr.sharedRefCount == 1) {
            if (descr.sharedInstance)
                descr.sharedInstance->deleteLater();
            descr.sharedInstance = 0;
            descr.sharedId = QUuid();
            descr.sharedRefCount = 0;
        } else {
            descr.sharedRefCount--;
        }
    } else {
        QObject* service = descr.individualInstances.take(instanceId);
        if (service) {
            service->deleteLater();
        }
    }
}


QTM_END_NAMESPACE
