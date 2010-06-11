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


#include "qcontactactionservicemanager_p.h"

#include "qcontactaction.h"
#include "qcontactactiondescriptor.h"
#include "qcontactactionfactory.h"

#include "qservicemanager.h"

#include <QMutexLocker>

QTM_BEGIN_NAMESPACE

Q_GLOBAL_STATIC(QContactActionServiceManager, contactActionServiceManagerInstance)

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
    qDeleteAll(m_actionFactoryHash);
}

void QContactActionServiceManager::init()
{
    // XXX NOTE: should already be locked PRIOR to entering this function.
    if (!initLock) {
        initLock = true;
        // fill up our hashes
        QList<QServiceInterfaceDescriptor> sids = m_serviceManager.findInterfaces(); // all services, all interfaces.
        foreach (const QServiceInterfaceDescriptor& sid, sids) {
            if (sid.interfaceName().startsWith(QString(QLatin1String("com.nokia.qt.mobility.contacts")))) {
                QContactActionFactory* actionFactory = qobject_cast<QContactActionFactory*>(m_serviceManager.loadInterface(sid));
                QContactActionDescriptor ad = actionFactory->actionDescriptor();
                m_descriptorHash.insert(ad.actionName(), ad); // multihash insert.
                m_actionFactoryHash.insert(ad, actionFactory);
            }
        }

        // and listen for signals.
        connect(&m_serviceManager, SIGNAL(serviceAdded(QString, QService::Scope)), this, SLOT(serviceAdded(QString)));
        connect(&m_serviceManager, SIGNAL(serviceRemoved(QString, QService::Scope)), this, SLOT(serviceRemoved(QString)));
    }
}

QList<QContactActionDescriptor> QContactActionServiceManager::availableActions(const QContact &contact)
{
    QMutexLocker locker(&m_instanceMutex);
    init();
    QList<QContactActionDescriptor> ret;
    QList<QContactActionFactory*> factories = m_actionFactoryHash.values();
    for (int i = 0; i < factories.size(); ++i) {
        QContactActionFactory* curr = factories.at(i);
        if (curr->supportsContact(contact)) {
            ret.append(curr->actionDescriptor());
        }
    }

    return ret;
}

QList<QContactActionDescriptor> QContactActionServiceManager::actionDescriptors(const QString& actionName)
{
    QMutexLocker locker(&m_instanceMutex);
    init();
    if (actionName.isEmpty())
        return m_descriptorHash.values();
    return m_descriptorHash.values(actionName);
}

/*! The caller takes ownership of the returned action pointer, and must delete it to avoid leaking memory. */
QContactAction* QContactActionServiceManager::action(const QContactActionDescriptor& descriptor)
{
    QMutexLocker locker(&m_instanceMutex);
    init();
    return m_actionFactoryHash.value(descriptor)->instance();
}

void QContactActionServiceManager::serviceAdded(const QString& serviceName)
{
    QMutexLocker locker(&m_instanceMutex);
    QList<QServiceInterfaceDescriptor> sids = m_serviceManager.findInterfaces(serviceName);
    foreach (const QServiceInterfaceDescriptor& sid, sids) {
        if (sid.interfaceName().startsWith(QString(QLatin1String("com.nokia.qt.mobility.contacts")))) {
            QContactActionFactory* actionFactory = qobject_cast<QContactActionFactory*>(m_serviceManager.loadInterface(sid));
            QContactActionDescriptor ad = actionFactory->actionDescriptor();
            m_descriptorHash.insert(ad.actionName(), ad); // multimap insert.
            m_actionFactoryHash.insert(ad, actionFactory);
        }
    }
}

void QContactActionServiceManager::serviceRemoved(const QString& serviceName)
{
    QMutexLocker locker(&m_instanceMutex);
    QList<QServiceInterfaceDescriptor> sids = m_serviceManager.findInterfaces(serviceName);
    foreach (const QServiceInterfaceDescriptor& sid, sids) {
        if (sid.interfaceName().startsWith(QString(QLatin1String("com.nokia.qt.mobility.contacts")))) {
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
