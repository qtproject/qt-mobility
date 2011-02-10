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

#ifndef QNEARFIELDMANAGER_MEEGO_P_H
#define QNEARFIELDMANAGER_MEEGO_P_H

#include "qnearfieldmanager_p.h"
#include "qnearfieldmanager.h"
#include "qnearfieldtarget.h"

#include <QtCore/QPointer>
#include <QtCore/QMap>
#include <QtCore/QPair>
#include <QtCore/QMetaMethod>
#include <QtCore/QBasicTimer>

#include <QtDBus/QDBusConnection>

QT_FORWARD_DECLARE_CLASS(QDBusObjectPath)

class ComNokiaNfcManagerInterface;
class ComNokiaNfcAdapterInterface;
class NDEFHandlerAdaptor;
class AccessRequestorAdaptor;

QTM_BEGIN_NAMESPACE

class QNearFieldManagerPrivateImpl;

class NdefHandler : public QObject
{
    Q_OBJECT

public:
    NdefHandler(QNearFieldManagerPrivateImpl *manager, const QString &serviceName,
                const QString &path, QObject *object, const QMetaMethod &method);
    ~NdefHandler();

    bool isValid() const;

    QString serviceName() const;
    QString path() const;

private:
    Q_INVOKABLE void NDEFData(const QDBusObjectPath &target, const QByteArray &message);

    QNearFieldManagerPrivateImpl *m_manager;
    NDEFHandlerAdaptor *m_adaptor;
    QObject *m_object;
    QMetaMethod m_method;
    QString m_serviceName;
    QString m_path;
};

class QNearFieldManagerPrivateImpl : public QNearFieldManagerPrivate
{
    Q_OBJECT

public:
    QNearFieldManagerPrivateImpl();
    ~QNearFieldManagerPrivateImpl();

    bool startTargetDetection(const QList<QNearFieldTarget::Type> &targetTypes);
    void stopTargetDetection();

    QNearFieldTarget *targetForPath(const QString &path);

    int registerTargetDetectedHandler(const QString &filter,
                                      QObject *object, const QMetaMethod &method);
    int registerTargetDetectedHandler(QObject *object, const QMetaMethod &method);
    int registerTargetDetectedHandler(const QNdefFilter &filter,
                                      QObject *object, const QMetaMethod &method);
    bool unregisterTargetDetectedHandler(int handlerId);

    void requestAccess(QNearFieldManager::TargetAccessModes accessModes);
    void releaseAccess(QNearFieldManager::TargetAccessModes accessModes);

    // Access Agent Adaptor
    Q_INVOKABLE void AccessFailed(const QDBusObjectPath &target, const QString &error);
    Q_INVOKABLE void AccessGranted(const QDBusObjectPath &target, const QString &kind);

protected:
    void timerEvent(QTimerEvent *event);

private slots:
    void emitTargetDetected(const QString &targetPath);
    void _q_targetDetected(const QString &targetPath);
    void _q_targetLost(const QString &targetPath);

private:
    int getFreeId();

    QDBusConnection m_connection;
    ComNokiaNfcManagerInterface *m_manager;
    ComNokiaNfcAdapterInterface *m_adapter;

    QList<QNearFieldTarget::Type> m_detectTargetTypes;
    QMap<QString, QPointer<QNearFieldTarget> > m_targets;

    AccessRequestorAdaptor *m_accessAgent;

    QMap<int, NdefHandler *> m_registeredHandlers;

    QMap<QString, QBasicTimer> m_pendingDetectedTargets;
};

QTM_END_NAMESPACE

#endif // QNEARFIELDMANAGER_MEEGO_P_H
