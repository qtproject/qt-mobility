/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the plugins of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QCONNMANSERVICE_H
#define QCONNMANSERVICE_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qmobilityglobal.h"
#include <QtDBus/qdbusabstractinterface.h>
#include <QtDBus/qdbuscontext.h>

#ifndef QT_NO_DBUS

QTM_BEGIN_NAMESPACE

class QConnmanManagerInterface : public QDBusAbstractInterface
{
    Q_OBJECT

public:
    QConnmanManagerInterface( QObject *parent = 0);
    ~QConnmanManagerInterface();

    QDBusObjectPath lookupService(const QString &service);
    QString getDefaultTechnology();
    QString getPathForTechnology(const QString &technology);
    QStringList getTechnologies();
    QStringList getServices();

Q_SIGNALS:
    void propertyChangedContext(const QString &path, const QString &item, const QDBusVariant &value);

protected:
    void connectNotify(const char *signal);
    void disconnectNotify(const char *signal);

private:
    QVariant getProperty(const QString &property);
};

class QConnmanServiceInterface : public QDBusAbstractInterface
{
    Q_OBJECT

public:
    QConnmanServiceInterface(const QString &dbusPathName,QObject *parent = 0);
    ~QConnmanServiceInterface();

    bool isRoaming();
    QString getAPN();
    QString getInterface();
    QString getMacAddress();
    QString getName();
    QString getState();
    QString getType();
    QStringList getDomains();
    QVariantMap getEthernet();
    quint8 getSignalStrength();

Q_SIGNALS:
    void propertyChangedContext(const QString &path, const QString &item, const QDBusVariant &value);

protected:
    void connectNotify(const char *signal);
    void disconnectNotify(const char *signal);

private:
    QVariant getProperty(const QString &);
};

class QConnmanTechnologyInterface : public QDBusAbstractInterface
{
    Q_OBJECT

public:
    QConnmanTechnologyInterface(const QString &dbusPathName,QObject *parent = 0);
    ~QConnmanTechnologyInterface();
};

class QConnmanDBusHelper: public QObject, protected QDBusContext
{
    Q_OBJECT

public:
    QConnmanDBusHelper(QObject *parent = 0);
    ~QConnmanDBusHelper();

public Q_SLOTS:
    void propertyChanged(const QString &item, const QDBusVariant &value);

Q_SIGNALS:
    void propertyChangedContext(const QString &path, const QString &item, const QDBusVariant &value);
};

QTM_END_NAMESPACE

#endif // QT_NO_DBUS

#endif //QCONNMANSERVICE_H
