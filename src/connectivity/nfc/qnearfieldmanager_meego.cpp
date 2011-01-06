/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qnearfieldmanager_meego_p.h"
#include "qnearfieldtarget_meego_p.h"
#include "meego/manager_interface_p.h"
#include "meego/adapter_interface_p.h"
#include "meego/target_interface_p.h"
#include "meego/tag_interface_p.h"
#include "meego/ndefhandler_adaptor_p.h"
#include "meego/accessrequestor_adaptor_p.h"

#include <qnearfieldtagtype1.h>

#include <QtDBus/QDBusConnection>

using namespace com::nokia::nfc;

QTM_BEGIN_NAMESPACE

static const char * const ndefAgentPath = "/com/nokia/nfc/ndefAgent/";

static inline bool matchesTarget(QNearFieldTarget::Type type,
                                 const QList<QNearFieldTarget::Type> &types)
{
    return types.contains(type) || types.contains(QNearFieldTarget::AnyTarget);
}

NdefHandler::NdefHandler(QNearFieldManagerPrivateImpl *manager, NDEFHandlerAdaptor *adaptor,
                         QObject *object, const QMetaMethod &method)
:   m_manager(manager), m_adaptor(adaptor), m_object(object), m_method(method)
{
}

NdefHandler::~NdefHandler()
{
    delete m_adaptor;
}

void NdefHandler::NDEFDetected(const QDBusObjectPath &target, const QByteArray &message)
{
    m_method.invoke(m_object, Q_ARG(QNdefMessage, QNdefMessage::fromByteArray(message)),
                    Q_ARG(QNearFieldTarget *, m_manager->targetForPath(target.path())));
}

QNearFieldManagerPrivateImpl::QNearFieldManagerPrivateImpl()
:   m_connection(QDBusConnection::connectToBus(QDBusConnection::SystemBus, QUuid::createUuid())),
    m_accessAgent(0)
{
    qDebug() << Q_FUNC_INFO;

    qDBusRegisterMetaType<QList<QByteArray> >();

    m_manager = new Manager(QLatin1String("com.nokia.nfc"), QLatin1String("/"), m_connection);

    QDBusObjectPath defaultAdapterPath = m_manager->DefaultAdapter();

    qDebug() << "Default NFC adapter:" << defaultAdapterPath.path();

    m_adapter = new Adapter(QLatin1String("com.nokia.nfc"), defaultAdapterPath.path(),
                            m_connection);

    if (!m_adapter->isValid()) {
        qDebug() << "adapter is invalid, DBus error?";
        return;
    }

    qDebug() << "Adapter properties:";
    qDebug() << "State:" << m_adapter->state();
    qDebug() << "Discovering:" << m_adapter->discovering();
    qDebug() << "Tag Technologies:" << m_adapter->tagTechnologies();

    m_adapter->setState(QLatin1String("on"));

    qDebug() << "State:" << m_adapter->state();
}

QNearFieldManagerPrivateImpl::~QNearFieldManagerPrivateImpl()
{
    qDebug() << Q_FUNC_INFO;
    qDebug() << "***************** DESTROY *******************";

    delete m_manager;
    delete m_adapter;
}

void QNearFieldManagerPrivateImpl::startTargetDetection(const QList<QNearFieldTarget::Type> &targetTypes)
{
    qDebug() << Q_FUNC_INFO;

    m_detectTargetTypes = targetTypes;

    connect(m_adapter, SIGNAL(TargetDetected(QDBusObjectPath)),
            this, SLOT(_q_targetDetected(QDBusObjectPath)));
    connect(m_adapter, SIGNAL(TargetLost(QDBusObjectPath)),
            this, SLOT(_q_targetLost(QDBusObjectPath)));
}

void QNearFieldManagerPrivateImpl::stopTargetDetection()
{
    qDebug() << Q_FUNC_INFO;

    m_detectTargetTypes.clear();

    disconnect(m_adapter, SIGNAL(TargetDetected(QDBusObjectPath)),
               this, SLOT(_q_targetDetected(QDBusObjectPath)));
    disconnect(m_adapter, SIGNAL(TargetLost(QDBusObjectPath)),
               this, SLOT(_q_targetLost(QDBusObjectPath)));
}

QNearFieldTarget *QNearFieldManagerPrivateImpl::targetForPath(const QString &path)
{
    qDebug() << Q_FUNC_INFO;

    QNearFieldTarget *nearFieldTarget = m_targets.value(path);
    if (!nearFieldTarget) {
        Target *target = new Target(QLatin1String("com.nokia.nfc"), path, m_connection);

        const QString type = target->type();

        qDebug() << "target type is:" << type;

        if (type == QLatin1String("tag")) {
            Tag *tag = new Tag(QLatin1String("com.nokia.nfc"), path, m_connection);

            const QString tagTechnology = tag->technology();
            if (tagTechnology == QLatin1String("TagType1"))
                nearFieldTarget = new TagType1(this, target, tag);
            else
                nearFieldTarget = new NearFieldTarget<QNearFieldTarget>(this, target, tag);
        } else if (type == QLatin1String("device")) {
            nearFieldTarget = new NearFieldTarget<QNearFieldTarget>(this, target, 0);
        }

        if (nearFieldTarget)
            m_targets.insert(path, nearFieldTarget);
    }

    return nearFieldTarget;
}

int QNearFieldManagerPrivateImpl::registerTargetDetectedHandler(const QString &filter,
                                                                QObject *object,
                                                                const QMetaMethod &method)
{
    qDebug() << Q_FUNC_INFO;

    int id = getFreeId();

    const QString agentPath = QLatin1String(ndefAgentPath) + QString::number(id);

    NDEFHandlerAdaptor *ndefAgent = new NDEFHandlerAdaptor(this);

    if (!QDBusConnection::sessionBus().registerObject(agentPath, this)) {
        delete ndefAgent;
        m_freeIds.append(id);
        return -1;
    }

    QDBusPendingReply<> reply = m_manager->RegisterNDEFHandler(QLatin1String("session"),
                                                               QLatin1String("test name"),
                                                               QDBusObjectPath(agentPath),
                                                               QLatin1String("any"),
                                                               filter);
    if (reply.isError()) {
        qDebug() << "RegisterNDEFAgent error";
        qDebug() << reply.error().name() << reply.error().message();
        QDBusConnection::sessionBus().unregisterObject(agentPath);
        m_freeIds.append(id);
        return -1;
    }

    m_registeredHandlers[id] = new NdefHandler(this, ndefAgent, object, method);

    return id;
}

int QNearFieldManagerPrivateImpl::registerTargetDetectedHandler(QObject *object,
                                                                const QMetaMethod &method)
{
    qDebug() << Q_FUNC_INFO;

    return registerTargetDetectedHandler(QLatin1String("'*';"), object, method);
}

int QNearFieldManagerPrivateImpl::registerTargetDetectedHandler(const QNdefFilter &filter,
                                                                QObject *object,
                                                                const QMetaMethod &method)
{
    qDebug() << Q_FUNC_INFO;

    Q_UNUSED(filter);
    Q_UNUSED(object);
    Q_UNUSED(method);

    return -1;
}

bool QNearFieldManagerPrivateImpl::unregisterTargetDetectedHandler(int handlerId)
{
    qDebug() << Q_FUNC_INFO;

    if (handlerId < 0 || handlerId >= m_registeredHandlers.count())
        return false;

    const QString agentPath = QLatin1String(ndefAgentPath) + QString::number(handlerId);
    QDBusConnection::sessionBus().unregisterObject(agentPath);

    delete m_registeredHandlers[handlerId];
    m_registeredHandlers[handlerId] = 0;

    m_freeIds.append(handlerId);

    while (m_freeIds.contains(m_registeredHandlers.count() - 1)) {
        m_freeIds.removeAll(m_registeredHandlers.count() - 1);
        m_registeredHandlers.removeLast();
    }

    return true;
}

static QStringList accessModesToKind(QNearFieldManager::TargetAccessModes accessModes)
{
    QStringList kind;

    if (accessModes & QNearFieldManager::NdefReadTargetAccess)
        kind.append(QLatin1String("tag.ndef.read"));
    else if (accessModes & QNearFieldManager::NdefWriteTargetAccess)
        kind.append(QLatin1String("tag.ndef.write"));
    else if (accessModes & QNearFieldManager::TagTypeSpecificTargetAccess)
        kind.append(QLatin1String("tag.raw"));

    return kind;
}

void QNearFieldManagerPrivateImpl::requestAccess(QNearFieldManager::TargetAccessModes accessModes)
{
    qDebug() << Q_FUNC_INFO;

    if (!m_accessAgent) {
        m_accessAgent = new AccessRequestorAdaptor(this);
        if (!m_connection.registerObject(QLatin1String("/test"), this)) {
            qDebug() << "failed to register agent";
            delete m_accessAgent;
            m_accessAgent = 0;
            return;
        }
    }

    qDebug() << "agent registered, calling RequestAccess";

    foreach (const QString &kind, accessModesToKind(accessModes))
        m_adapter->RequestAccess(QDBusObjectPath("/test"), kind);

    QNearFieldManagerPrivate::requestAccess(accessModes);
}

void QNearFieldManagerPrivateImpl::releaseAccess(QNearFieldManager::TargetAccessModes accessModes)
{
    qDebug() << Q_FUNC_INFO;

    foreach (const QString &kind, accessModesToKind(accessModes))
        m_adapter->CancelAccessRequest(QDBusObjectPath("/test"), kind);

    QNearFieldManagerPrivate::releaseAccess(accessModes);
}

void QNearFieldManagerPrivateImpl::AccessFailed(const QDBusObjectPath &target,
                                                const QString &error)
{
    qDebug() << Q_FUNC_INFO;

    qDebug() << "Access for" << target.path() << "failed with error:" << error;
}

void QNearFieldManagerPrivateImpl::AccessGranted(const QDBusObjectPath &target,
                                                 const QString &kind)
{
    qDebug() << Q_FUNC_INFO;

    qDebug() << "Access granted for" << target.path() << kind;

    _q_targetDetected(target);
}

void QNearFieldManagerPrivateImpl::_q_targetDetected(const QDBusObjectPath &targetPath)
{
    qDebug() << Q_FUNC_INFO;

    QNearFieldTarget *target = targetForPath(targetPath.path());
    if (target && matchesTarget(target->type(), m_detectTargetTypes))
        emit targetDetected(target);
}

void QNearFieldManagerPrivateImpl::_q_targetLost(const QDBusObjectPath &targetPath)
{
    qDebug() << Q_FUNC_INFO;

    QNearFieldTarget *nearFieldTarget = m_targets.value(targetPath.path());

    // haven't seen target so just drop this event
    if (!nearFieldTarget) {
        // We either haven't seen target (started after target was detected by system) or the
        // application deleted the target. Remove from map and dont emit anything.
        m_targets.remove(targetPath.path());
        return;
    }

    if (matchesTarget(nearFieldTarget->type(), m_detectTargetTypes))
        emit targetLost(nearFieldTarget);
}

int QNearFieldManagerPrivateImpl::getFreeId()
{
    qDebug() << Q_FUNC_INFO;

    if (!m_freeIds.isEmpty())
        return m_freeIds.takeFirst();

    m_registeredHandlers.append(0);
    return m_registeredHandlers.count() - 1;
}

#include "moc_qnearfieldmanager_meego_p.cpp"

QTM_END_NAMESPACE
