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

#ifndef QSERVICE_INSTANCE_MANAGER
#define QSERVICE_INSTANCE_MANAGER

#include <qmobilityglobal.h>
#include "qremoteserviceclassregister.h"
#include <QHash>
#include <QMutexLocker>
#include <QMetaObject>
#include <QMetaClassInfo>
#include <QUuid>
#include <QDebug>

QTM_BEGIN_NAMESPACE

struct ServiceIdentDescriptor
{
    ServiceIdentDescriptor() : sharedInstance(0), sharedRefCount(0)
    {
    }

    const QMetaObject* meta;
    QRemoteServiceClassRegister::CreateServiceFunc create;
    QRemoteServiceClassRegister::InstanceType instanceType;
    QHash<QUuid, QObject*> individualInstances;
    QObject* sharedInstance;
    QUuid sharedId;
    int sharedRefCount;
};

class Q_AUTOTEST_EXPORT InstanceManager 
{
public:
    InstanceManager();
    ~InstanceManager();

    bool addType(const QMetaObject* meta,
            QRemoteServiceClassRegister::CreateServiceFunc func,
            QRemoteServiceClassRegister::TypeIdentFunc typeFunc,
            QRemoteServiceClassRegister::InstanceType type);
    const QMetaObject* metaObject(const QRemoteServiceIdentifier& ident) const;
    QList<QRemoteServiceIdentifier> allIdents() const;

    QObject* createObjectInstance(const QRemoteServiceIdentifier& ident, QUuid& instanceId);
    void removeObjectInstance(const QRemoteServiceIdentifier& ident, const QUuid& instanceId);

    static InstanceManager* instance();

private:
    mutable QMutex lock;
    QHash<QRemoteServiceIdentifier, ServiceIdentDescriptor> metaMap;
};



QTM_END_NAMESPACE


#endif //QSERVICE_INSTANCE_MANAGER
