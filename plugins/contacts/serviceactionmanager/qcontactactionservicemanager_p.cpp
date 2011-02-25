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
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
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
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/


#include "qcontactactionservicemanager_p.h"

#include "qcontactaction.h"
#include "qcontactactiondescriptor.h"
#include "qcontactactionfactory.h"

#include "qservicemanager.h"

#include <QMutexLocker>

QTM_BEGIN_NAMESPACE

Q_GLOBAL_STATIC(QContactActionServiceManager, contactActionServiceManagerInstance)
Q_EXPORT_PLUGIN2(qtcontacts_serviceactionmanager, QContactActionServiceManager);

/*!
  \internal
  \class QContactActionServiceManager
  This class uses the service framework to discover contact actions which are
  provided by third parties.  It is an implementation detail of QContactAction.
 */

QContactActionServiceManager* QContactActionServiceManager::instance()
{
    return contactActionServiceManagerInstance();
}

QContactActionServiceManager::QContactActionServiceManager()
    : QObject(), initLock(false)
{
}

QContactActionServiceManager::~QContactActionServiceManager()
{
    // we don't use qDeleteAll() because some factories produce more than one action descriptor.
    QList<QContactActionDescriptor> keys = m_actionFactoryHash.keys();
    QSet<QContactActionFactory*> deletedFactories;
    foreach (const QContactActionDescriptor& key, keys) {
        QContactActionFactory *curr = m_actionFactoryHash.value(key);
        if (!deletedFactories.contains(curr)) {
            deletedFactories.insert(curr);
            delete curr;
        }
    }
}

void QContactActionServiceManager::init()
{
    // XXX NOTE: should already be locked PRIOR to entering this function.
    if (!initLock) {
        initLock = true;
        // fill up our hashes
        QList<QServiceInterfaceDescriptor> sids = m_serviceManager.findInterfaces(); // all services, all interfaces.
        foreach (const QServiceInterfaceDescriptor& sid, sids) {
            if (sid.interfaceName() == QContactActionFactory::InterfaceName) {
                if (static_cast<QService::Type>(sid.attribute(QServiceInterfaceDescriptor::ServiceType).toInt()) != QService::Plugin) {
                    continue; // we don't allow IPC contact action factories.
                }
                QContactActionFactory* actionFactory = qobject_cast<QContactActionFactory*>(m_serviceManager.loadInterface(sid));
                if (actionFactory) {
                    // if we cannot get the action factory from the service manager, then we don't add it to our hash.
                    QList<QContactActionDescriptor> descriptors = actionFactory->actionDescriptors();
                    foreach (const QContactActionDescriptor& ad, descriptors) {
                        m_descriptorHash.insert(ad.actionName(), ad); // multihash insert.
                        m_actionFactoryHash.insert(ad, actionFactory);
                    }
                }
            }
        }

        // and listen for signals.
        connect(&m_serviceManager, SIGNAL(serviceAdded(QString, QService::Scope)), this, SLOT(serviceAdded(QString)));
        connect(&m_serviceManager, SIGNAL(serviceRemoved(QString, QService::Scope)), this, SLOT(serviceRemoved(QString)));
    }
}

QHash<QContactActionDescriptor, QContactActionFactory*> QContactActionServiceManager::actionFactoryHash()
{
    QMutexLocker locker(&m_instanceMutex);
    init();
    return m_actionFactoryHash;
}

QMultiHash<QString, QContactActionDescriptor> QContactActionServiceManager::descriptorHash()
{
    QMutexLocker locker(&m_instanceMutex);
    init();
    return m_descriptorHash;
}

void QContactActionServiceManager::serviceAdded(const QString& serviceName)
{
    QMutexLocker locker(&m_instanceMutex);
    QList<QServiceInterfaceDescriptor> sids = m_serviceManager.findInterfaces(serviceName);
    foreach (const QServiceInterfaceDescriptor& sid, sids) {
        if (sid.interfaceName() == QContactActionFactory::InterfaceName) {
            if (static_cast<QService::Type>(sid.attribute(QServiceInterfaceDescriptor::ServiceType).toInt()) != QService::Plugin) {
                continue; // we don't allow IPC contact action factories.
            }
            QContactActionFactory* actionFactory = qobject_cast<QContactActionFactory*>(m_serviceManager.loadInterface(sid));
            if (actionFactory) {
                // if we cannot get the action factory from the service manager, then we don't add it to our hash.
                QList<QContactActionDescriptor> descriptors = actionFactory->actionDescriptors();
                foreach (const QContactActionDescriptor& ad, descriptors) {
                    m_descriptorHash.insert(ad.actionName(), ad); // multihash insert.
                    m_actionFactoryHash.insert(ad, actionFactory);
                }
            }
        }
    }
}

void QContactActionServiceManager::serviceRemoved(const QString& serviceName)
{
    QMutexLocker locker(&m_instanceMutex);
    QList<QServiceInterfaceDescriptor> sids = m_serviceManager.findInterfaces(serviceName);
    foreach (const QServiceInterfaceDescriptor& sid, sids) {
        if (sid.interfaceName() == QContactActionFactory::InterfaceName) {
            if (static_cast<QService::Type>(sid.attribute(QServiceInterfaceDescriptor::ServiceType).toInt()) != QService::Plugin) {
                continue; // we don't allow IPC contact action factories.
            }
            QList<QContactActionDescriptor> cads = m_actionFactoryHash.keys();
            foreach (const QContactActionDescriptor& cad, cads) {
                if (cad.serviceName() != serviceName)
                    continue;
                delete m_actionFactoryHash.value(cad);
                m_actionFactoryHash.remove(cad);
                m_descriptorHash.remove(cad.actionName(), cad);
            }
        }
    }
}

#include "moc_qcontactactionservicemanager_p.cpp"

QTM_END_NAMESPACE
