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

#ifndef QT_NO_DBUS

#define	UDISKS_SERVICE        "org.freedesktop.UDisks"
#define	UDISKS_PATH           "/org/freedesktop/UDisks"
#define	UDISKS_DEVICE_SERVICE "org.freedesktop.UDisks.Device"
#define	UDISKS_DEVICE_PATH    "/org/freedesktop/UDisks/Device"
#define	UPOWER_SERVICE        "org.freedesktop.UPower"
#define	UPOWER_PATH           "/org/freedesktop/UPower"
#define	UPOWER_DEVICE_SERVICE "org.freedesktop.UPower.Device"
#define	UPOWER_DEVICE_PATH    "/org/freedesktop/UPower/Device"

QTM_BEGIN_NAMESPACE

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

private:
    QVariant getProperty(const QString &property);
    QString path;
};

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

QTM_END_NAMESPACE

#endif // QT_NO_DBUS

#endif // QDEVICEKITSERVICE_H
