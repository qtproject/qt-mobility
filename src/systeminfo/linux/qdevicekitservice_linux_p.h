/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QDEVICEKITSERVICE_H
#define QDEVICEKITSERVICE_H

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
#include <QtDBus/QtDBus>

QTM_BEGIN_NAMESPACE

#if !defined(QT_NO_UDISKS)

class QUDisksInterface : public QDBusAbstractInterface
{
    Q_OBJECT

public:
    QUDisksInterface(QObject *parent = 0);
    ~QUDisksInterface();

    QList<QDBusObjectPath> enumerateDevices();

Q_SIGNALS:
    void deviceChanged(const QDBusObjectPath &path);

protected:
    void connectNotify(const char *signal);
    void disconnectNotify(const char *signal);
};

class QUDisksDeviceInterface : public QDBusAbstractInterface
{
    Q_OBJECT

public:
    QUDisksDeviceInterface(const QString &dbusPathName, QObject *parent = 0);
    ~QUDisksDeviceInterface();

    bool deviceIsMounted();
    bool deviceIsRemovable();
    bool deviceIsSystemInternal();
    bool deviceIsLinuxLvm2LV();
    bool deviceIsLinuxMd();
    bool deviceIsLinuxLvm2PV();
    bool driveIsRotational();

    QString uuid();
    QString deviceFilePresentation();
    QStringList deviceMountPaths();

    bool deviceIsDrive();
    QString driveMedia();
    bool driveCanDetach();
    bool driveIsMediaEjectable();

private:
    QVariant getProperty(const QString &property);
    QString path;
};

#endif // QT_NO_UDISKS

#if !defined(QT_NO_UPOWER)

class QUPowerInterface : public QDBusAbstractInterface
{
    Q_OBJECT

public:
    QUPowerInterface(QObject *parent = 0);
    ~QUPowerInterface();

    bool onBattery();

    QList<QDBusObjectPath> enumerateDevices();

Q_SIGNALS:
    void changed();

protected:
    void connectNotify(const char *signal);
    void disconnectNotify(const char *signal);

private:
    QVariant getProperty(const QString &property);
};

class QUPowerDeviceInterface : public  QDBusAbstractInterface
{
    Q_OBJECT
public:
    QUPowerDeviceInterface(const QString &dbusPathName,QObject *parent = 0);
    ~QUPowerDeviceInterface();


    bool isPowerSupply();
    bool isOnline();
    double currentEnergy();
    double energyWhenFull();
    double energyDischargeRate();
    double percentLeft();
    double voltage();
    quint16 getState();
    quint16 getType();
    qint64 timeToFull();

Q_SIGNALS:
    void changed();
    void propertyChanged(QString,QVariant);

protected:
    void connectNotify(const char *signal);
    void disconnectNotify(const char *signal);

private:
    QVariant getProperty(const QString &);
    QVariantMap getProperties();
    QDBusInterface *propertiesInterface;
    QString path;
    QVariantMap pMap;

private Q_SLOTS:
    void propChanged();
};

#endif // QT_NO_UPOWER

QTM_END_NAMESPACE

#endif // QDEVICEKITSERVICE_H
