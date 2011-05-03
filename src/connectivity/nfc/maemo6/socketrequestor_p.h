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

/*
    The private API defined in this file is temporary. It should be removed when Qt can handle
    passing unix file descriptors over DBus. Most likely in Qt 4.8.
*/

#ifndef SOCKETREQUESTOR_P_H
#define SOCKETREQUESTOR_P_H

#include <qmobilityglobal.h>

#include <QtCore/QObject>
#include <QtCore/QVariantMap>

QT_FORWARD_DECLARE_CLASS(QDBusObjectPath)
QT_FORWARD_DECLARE_CLASS(QDBusVariant)

class DBusConnection;

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

class SocketRequestor : public QObject
{
    Q_OBJECT

public:
    explicit SocketRequestor(const QString &adaptor, QObject *parent = 0);
    ~SocketRequestor();

    void requestAccess(const QString &path, const QString &kind);
    void cancelAccessRequest(const QString &path, const QString &kind);

    bool waitForDBusSignal(int msecs);

signals:
    void accessFailed(const QDBusObjectPath &targetPath, const QString &kind,
                      const QString &error);
    void accessGranted(const QDBusObjectPath &targetPath, const QString &accessKind);

    void accept(const QDBusVariant &lsap, const QDBusVariant &rsap, int fd, const QVariantMap &properties);
    void connect(const QDBusVariant &lsap, const QDBusVariant &rsap, int fd, const QVariantMap &properties);
    void socket(const QDBusVariant &lsap, const QDBusVariant &rsap, int fd, const QVariantMap &properties);

private:
    const QString m_adaptor;
};

QTM_END_NAMESPACE

QT_END_HEADER

#endif // SOCKETREQUESTOR_P_H
