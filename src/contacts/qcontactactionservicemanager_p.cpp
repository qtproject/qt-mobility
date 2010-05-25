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

#include "qservicemanager.h"

#include <QMutexLocker>

QTM_BEGIN_NAMESPACE

Q_GLOBAL_STATIC(QMutex, contactActionServiceManagerGlobalMutex)
QMutex* QContactActionServiceManager::m_instanceMutex = contactActionServiceManagerGlobalMutex();
QContactActionServiceManager* QContactActionServiceManager::m_instance = 0;

/*!
  \internal
  \class QContactActionServiceManager
  This class uses the service framework to discover contact actions which are
  provided by third parties.  It is an implementation detail of QContactAction.
 */

QContactActionServiceManager* QContactActionServiceManager::instance()
{
    QMutexLocker locker(m_instanceMutex);
    m_instance = new QContactActionServiceManager;
    return m_instance;
}

QContactActionServiceManager::QContactActionServiceManager()
    : QObject()
{
    m_serviceManager = new QServiceManager;
    connect(m_serviceManager, SIGNAL(serviceAdded(QString, QService::Scope)), this, SLOT(serviceAdded(QString)));
    connect(m_serviceManager, SIGNAL(serviceRemoved(QString, QService::Scope)), this, SLOT(serviceRemoved(QString)));
}

QContactActionServiceManager::~QContactActionServiceManager()
{
    // XXX TODO: when does m_instanceMutex get deleted?
    QMutexLocker locker(QContactActionServiceManager::m_instanceMutex);
    qDeleteAll(m_actionHash);
    delete m_serviceManager;
    if (m_instance) {
        delete QContactActionServiceManager::m_instance;
        QContactActionServiceManager::m_instance = 0;
    }
}

/* the following copy ctor and assignment operator should not work (or ever be called)! */
QContactActionServiceManager::QContactActionServiceManager(const QContactActionServiceManager &) : QObject() {}
QContactActionServiceManager& QContactActionServiceManager::operator=(const QContactActionServiceManager &) {return *this;}

QList<QContactActionDescriptor> QContactActionServiceManager::actionDescriptors(const QString& actionName)
{
    if (actionName.isEmpty())
        return m_descriptorHash.values();
    return m_descriptorHash.values(actionName);
}

/*! The QContactActionServiceManager retains ownership of the returned action pointer.  Do not delete it, or undefined behaviour will occur. */
QContactAction* QContactActionServiceManager::action(const QContactActionDescriptor& descriptor)
{
    return m_actionHash.value(descriptor);
}

void QContactActionServiceManager::serviceAdded(const QString& serviceName)
{
    QList<QServiceInterfaceDescriptor> sids = m_serviceManager->findInterfaces(serviceName);
    foreach (const QServiceInterfaceDescriptor& sid, sids) {
        if (sid.interfaceName().startsWith("com.nokia.qt.mobility.contacts")) {
            QContactAction* action = qobject_cast<QContactAction*>(m_serviceManager->loadInterface(sid));
            QContactActionDescriptor ad = action->actionDescriptor();
            m_descriptorHash.insert(ad.actionName(), ad);
            m_actionHash.insert(ad, action); // multimap insert.
        }
    }
}

void QContactActionServiceManager::serviceRemoved(const QString& serviceName)
{
    QList<QServiceInterfaceDescriptor> sids = m_serviceManager->findInterfaces(serviceName);
    foreach (const QServiceInterfaceDescriptor& sid, sids) {
        if (sid.interfaceName().startsWith("com.nokia.qt.mobility.contacts")) {
            QList<QContactActionDescriptor> cads = m_actionHash.keys();
            foreach (const QContactActionDescriptor& cad, cads) {
                if (cad.serviceName() != serviceName)
                    continue;
                delete m_actionHash.value(cad);
                m_actionHash.remove(cad);
                m_descriptorHash.remove(cad.actionName(), cad);
            }
        }
    }
}

#include "moc_qcontactactionservicemanager_p.cpp"

QTM_END_NAMESPACE
