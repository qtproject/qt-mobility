/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in Technology Preview License Agreement accompanying
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QHALSERVICE_H
#define QHALSERVICE_H


#include <QtDBus>
#include <QDBusConnection>
#include <QDBusError>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDBusReply>


#include <QDBusPendingCallWatcher>

#define HAL_DBUS_SERVICE "org.freedesktop.Hal"

#define HAL_DBUS_PATH "/org/freedesktop/Hal"
#define HAL_DBUS_INTERFACE "org.freedesktop.Hal"

#define HAL_DBUS_MANAGER_PATH "/org/freedesktop/Hal/Manager"
#define HAL_DBUS_MANAGER_INTERFACE "org.freedesktop.Hal.Manager"

#define HAL_DEVICE_PATH "/org/freedesktop/Hal/Device"
#define HAL_DEVICE_INTERFACE "org.freedesktop.Hal.Device"

#define HAL_DEVICES_PATH "/org/freedesktop/Hal/devices"

#define HAL_DEVICES_LAPTOPPANEL_INTERFACE "org.freedesktop.Hal.Device.LaptopPanel"

class QHalInterfacePrivate;
class QHalInterface : public QObject
{
    Q_OBJECT

public:

    QHalInterface(QObject *parent = 0);
    ~QHalInterface();

    bool isValid();
    QDBusInterface *connectionInterface() const;
    QStringList findDeviceByCapability(const QString &cap);
    QStringList getAllDevices();

Q_SIGNALS:

private Q_SLOTS:
private:
        QHalInterfacePrivate *d;

};

class QHalDeviceInterfacePrivate;
class QHalDeviceInterface : public QObject
{
    Q_OBJECT
    
public:
    QHalDeviceInterface(const QString &devicePathName, QObject *parent = 0);
    ~QHalDeviceInterface();

    bool isValid();
    bool getPropertyBool(const QString &prop);
    QString getPropertyString(const QString &prop);
    QStringList getPropertyStringList(const QString &prop);
    qint32 getPropertyInt(const QString &prop);

    bool queryCapability(const QString &cap);

private:
        QHalDeviceInterfacePrivate *d;

};

class QHalDeviceLaptopPanelInterfacePrivate;
class QHalDeviceLaptopPanelInterface : public QObject
{
    Q_OBJECT

public:
    QHalDeviceLaptopPanelInterface(const QString &devicePathName, QObject *parent = 0);
    ~QHalDeviceLaptopPanelInterface();
    bool isValid();
public:
    quint32 getBrightness();
    void setBrightness(quint32);

private:
        QHalDeviceLaptopPanelInterfacePrivate *d;
};

#endif //
