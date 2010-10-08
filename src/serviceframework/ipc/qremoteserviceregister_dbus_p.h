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

#ifndef QREMOTESERVICEREGISTER_DBUS_P_H
#define QREMOTESERVICEREGISTER_DBUS_P_H

#include "qremoteserviceregister.h"
#include "instancemanager_p.h"
#include "qserviceinterfacedescriptor.h"
#include <QUuid>
#include <QtDBus/QtDBus>

QTM_BEGIN_NAMESPACE

class ObjectEndPoint;

class DBusSession: public QObject, protected QDBusContext
{
    Q_OBJECT
public:
    DBusSession() {} 
    ~DBusSession() {}

public slots:
    QByteArray writePackage(const QByteArray &package, int type, const QString &id) {
        emit packageReceived(package, type, id); 
        return package; 
    }

    bool processIncoming() { 
        int pid = connection().interface()->servicePid(message().service());
        int uid = connection().interface()->serviceUid(message().service());
        emit newConnection(pid, uid);
        return m_accept;
    }

    void acceptIncoming(bool accept) {
        m_accept = accept;
    }

    void closeIncoming() {
        emit closeConnection();
    }

Q_SIGNALS:
    void packageReceived(const QByteArray &package, int type, const QString &id);
    void newConnection(int pid, int uid);
    void closeConnection();

private:
    bool m_accept;
};


class QRemoteServiceRegisterDBusPrivate: public QRemoteServiceRegisterPrivate
{
    Q_OBJECT
public:
    QRemoteServiceRegisterDBusPrivate(QObject* parent);
    ~QRemoteServiceRegisterDBusPrivate();
    void publishServices(const QString& ident );

public slots:
    void processIncoming(int pid, int uid);
    void processClosing();

private:
    bool createServiceEndPoint(const QString& ident);

    QList<ObjectEndPoint*> pendingConnections;
    QDBusInterface *iface;
    DBusSession *session;
    int instanceCount;
};

QTM_END_NAMESPACE

#endif
