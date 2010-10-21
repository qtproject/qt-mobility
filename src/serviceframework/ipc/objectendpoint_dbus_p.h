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

#ifndef OBJECT_ENDPOINT_DBUS_H
#define OBJECT_ENDPOINT_DBUS_H

#include "qmobilityglobal.h"
#include "ipcendpoint_p.h"
#include "qremoteserviceregister.h"
#include "qservice.h"
#include <QPointer>
#include <QHash>
#include <QtDBus>
#include "qservicemetaobject_dbus_p.h"

QTM_BEGIN_NAMESPACE

class QServiceMetaObjectDBus;
class ObjectEndPointPrivate;
class ObjectEndPoint : public QObject
{
    Q_OBJECT
public:
    enum Type {
        Service = 0,
        Client
    };

    ObjectEndPoint(Type type, QServiceIpcEndPoint* comm, QObject* parent = 0);
    ~ObjectEndPoint();

    QObject* constructProxy(const QRemoteServiceRegister::Entry& entry);
    
    void objectRequest(const QServicePackage& p);
    void methodCall(const QServicePackage& p);
    void propertyCall(const QServicePackage& p);

    QString getInstanceId() const;

    QVariant invokeRemote(int metaIndex, const QVariantList& args, int returnType);
    QVariant invokeRemoteProperty(int metaIndex, const QVariant& arg, int returnType, QMetaObject::Call c);
    
Q_SIGNALS:
    void pendingRequestFinished();

public Q_SLOTS:
    void newPackageReady();
    void disconnected(const QString& clientId, const QString & instanceId);
    void unregisterObjectDBus(const QRemoteServiceRegister::Entry& entry, const QUuid& id);

private:
    void waitForResponse(const QUuid& requestId);
    QVariant toDBusVariant(const QByteArray& type, const QVariant& arg);

    QServiceIpcEndPoint* dispatch;
    QPointer<QObject> service;
    ObjectEndPointPrivate* d;
    
    QDBusConnection *connection;
    QDBusInterface *iface;
    QServiceMetaObjectDBus *signalsObject;
};

QTM_END_NAMESPACE

#endif //OBJECT_ENDPOINT_DBUS_H
