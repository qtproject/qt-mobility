/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <QtTest/QtTest>

#include <qaudiodevicecontrol.h>
#include <qvideodevicecontrol.h>
#include <qmediacontrol.h>
#include <qmediaservice.h>

#include <QtGui/qapplication.h>
#include <QtGui/qstyle.h>

class QtTestMediaService;

class tst_QMediaService : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();

    void control();
    void nullAudioDeviceControl();
    void audioEndpoints();
};

Q_DECLARE_METATYPE(QMediaService::MediaEndpoint);

class QtTestMediaControlA : public QMediaControl
{
    Q_OBJECT
};

#define QtTestMediaControlA_iid "com.nokia.QtTestMediaControlA"
Q_MEDIA_DECLARE_CONTROL(QtTestMediaControlA, QtTestMediaControlA_iid)

class QtTestMediaControlB : public QMediaControl
{
    Q_OBJECT
};

#define QtTestMediaControlB_iid "com.nokia.QtTestMediaControlB"
Q_MEDIA_DECLARE_CONTROL(QtTestMediaControlB, QtTestMediaControlB_iid)

class QtTestMediaControlC : public QMediaControl
{
    Q_OBJECT
};

#define QtTestMediaControlC_iid "com.nokia.QtTestMediaControlC"
Q_MEDIA_DECLARE_CONTROL(QtTestMediaControlC, QtTestMediaControlA_iid) // Yes A.

class QtTestMediaControlD : public QMediaControl
{
    Q_OBJECT
};

#define QtTestMediaControlD_iid "com.nokia.QtTestMediaControlD"
Q_MEDIA_DECLARE_CONTROL(QtTestMediaControlD, QtTestMediaControlD_iid)

struct QtTestDevice
{
    QtTestDevice() {}
    QtTestDevice(const QString &name, const QString &description, const QIcon &icon)
        : name(name), description(description), icon(icon)
    {
    }

    QString name;
    QString description;
    QIcon icon;
};

class QtTestAudioDeviceControl : public QAudioDeviceControl
{
public:
    QtTestAudioDeviceControl(QObject *parent = 0)
        : QAudioDeviceControl(parent)
        , m_selectedDevice(-1)
        , m_defaultDevice(-1)
    {
    }

    int deviceCount() const { return devices.count(); }

    QString name(int index) const { return devices.value(index).name; }
    QString description(int index) const { return devices.value(index).description; }
    QIcon icon(int index) const { return devices.value(index).icon; }

    int defaultDevice() const { return m_defaultDevice; }
    void setDefaultDevice(int index) { m_defaultDevice = index; }

    int selectedDevice() const { return m_selectedDevice; }
    void setSelectedDevice(int index)
    {
        emit selectedDeviceChanged(m_selectedDevice = index);
        emit selectedDeviceChanged(devices.value(index).name);
    }

    QList<QtTestDevice> devices;

private:
    int m_selectedDevice;
    int m_defaultDevice;
};

class QtTestVideoDeviceControl : public QVideoDeviceControl
{
public:
    QtTestVideoDeviceControl(QObject *parent = 0)
        : QVideoDeviceControl(parent)
        , m_selectedDevice(-1)
        , m_defaultDevice(-1)
    {
    }

    int deviceCount() const { return devices.count(); }

    QString name(int index) const { return devices.value(index).name; }
    QString description(int index) const { return devices.value(index).description; }
    QIcon icon(int index) const { return devices.value(index).icon; }

    int defaultDevice() const { return m_defaultDevice; }
    void setDefaultDevice(int index) { m_defaultDevice = index; }

    int selectedDevice() const { return m_selectedDevice; }
    void setSelectedDevice(int index)
    {
        emit selectedDeviceChanged(m_selectedDevice = index);
        emit selectedDeviceChanged(devices.value(index).name);
    }

    QList<QtTestDevice> devices;

private:
    int m_selectedDevice;
    int m_defaultDevice;
};

class QtTestMediaService : public QMediaService
{
    Q_OBJECT
public:
    QtTestMediaService()
        : QMediaService(0)
        , hasDeviceControls(false)
    {
    }

    QMediaControl* control(const char *name) const
    {
        if (strcmp(name, QtTestMediaControlA_iid) == 0)
            return const_cast<QtTestMediaControlA *>(&controlA);
        else if (strcmp(name, QtTestMediaControlB_iid) == 0)
            return const_cast<QtTestMediaControlB *>(&controlB);
        else if (strcmp(name, QtTestMediaControlC_iid) == 0)
            return const_cast<QtTestMediaControlC *>(&controlC);
        else if (hasDeviceControls && strcmp(name, QAudioDeviceControl_iid) == 0)
            return const_cast<QtTestAudioDeviceControl *>(&audioDeviceControl);
        else if (hasDeviceControls && strcmp(name, QVideoDeviceControl_iid) == 0)
            return const_cast<QtTestVideoDeviceControl *>(&videoDeviceControl);
        else
            return 0;
    }

    using QMediaService::control;

    QtTestMediaControlA controlA;
    QtTestMediaControlB controlB;
    QtTestMediaControlC controlC;
    QtTestAudioDeviceControl audioDeviceControl;
    QtTestVideoDeviceControl videoDeviceControl;
    bool hasDeviceControls;
};

void tst_QMediaService::initTestCase()
{
    qRegisterMetaType<QMediaService::MediaEndpoint>();
}

void tst_QMediaService::control()
{
    QtTestMediaService service;

    QCOMPARE(service.control<QtTestMediaControlA *>(), &service.controlA);
    QCOMPARE(service.control<QtTestMediaControlB *>(), &service.controlB);
    QVERIFY(!service.control<QtTestMediaControlC *>());  // Faulty implementation returns A.
    QVERIFY(!service.control<QtTestMediaControlD *>());  // No control of that type.
}

void tst_QMediaService::nullAudioDeviceControl()
{
    const QString deviceName(QLatin1String("test"));

    QtTestMediaService service;

    QSignalSpy spy(&service, SIGNAL(activeEndpointChanged(QMediaService::MediaEndpoint, QString)));

    QCOMPARE(service.isEndpointSupported(QMediaService::AudioDevice), false);
    QCOMPARE(service.supportedEndpoints(QMediaService::AudioDevice), QStringList());
    QCOMPARE(service.endpointDescription(QMediaService::AudioDevice, deviceName), QString());
    QCOMPARE(service.activeEndpoint(QMediaService::AudioDevice), QString());

    QCOMPARE(service.setActiveEndpoint(QMediaService::AudioDevice, deviceName), false);
    QCOMPARE(service.activeEndpoint(QMediaService::AudioDevice), QString());
}

void tst_QMediaService::audioEndpoints()
{
    const QtTestDevice device1(
            QLatin1String("mic"),
            QLatin1String("Microphone"),
            QApplication::style()->standardIcon(QStyle::SP_ArrowLeft));
    const QtTestDevice device2(
            QLatin1String("headset"),
            QLatin1String("Head-set"),
            QApplication::style()->standardIcon(QStyle::SP_ArrowDown));
    const QtTestDevice device3(
            QLatin1String("handset"),
            QLatin1String("Hand-set"),
            QApplication::style()->standardIcon(QStyle::SP_ArrowUp));
    const QtTestDevice device4(
            QLatin1String("lineout"),
            QLatin1String("Speakers"),
            QApplication::style()->standardIcon(QStyle::SP_ArrowRight));

    const QStringList deviceNames = QStringList()
            << device1.name
            << device2.name
            << device3.name;

    QtTestMediaService service;
    service.hasDeviceControls = true;

    QCOMPARE(service.isEndpointSupported(QMediaService::AudioDevice), true);
    QCOMPARE(service.supportedEndpoints(QMediaService::AudioDevice), QStringList());
    QCOMPARE(service.endpointDescription(QMediaService::AudioDevice, device1.name), QString());
    QCOMPARE(service.endpointDescription(QMediaService::AudioDevice, device2.name), QString());
    QCOMPARE(service.endpointDescription(QMediaService::AudioDevice, device4.name), QString());

    QCOMPARE(service.activeEndpoint(QMediaService::AudioDevice), QString());
    QCOMPARE(service.setActiveEndpoint(QMediaService::AudioDevice, device1.name), false);
    QCOMPARE(service.activeEndpoint(QMediaService::AudioDevice), QString());

    service.audioDeviceControl.devices.append(device1);
    service.audioDeviceControl.devices.append(device2);
    service.audioDeviceControl.devices.append(device3);

    QCOMPARE(service.supportedEndpoints(QMediaService::AudioDevice), deviceNames);
    QCOMPARE(service.endpointDescription(QMediaService::AudioDevice, device1.name),
             device1.description);
    QCOMPARE(service.endpointDescription(QMediaService::AudioDevice, device2.name),
             device2.description);
    QCOMPARE(service.endpointDescription(QMediaService::AudioDevice, device4.name), QString());
    QCOMPARE(service.activeEndpoint(QMediaService::AudioDevice), QString());

    QCOMPARE(service.setActiveEndpoint(QMediaService::AudioDevice, device1.name), true);
    QCOMPARE(service.activeEndpoint(QMediaService::AudioDevice), device1.name);

    QCOMPARE(service.setActiveEndpoint(QMediaService::AudioDevice, device1.name), true);
    QCOMPARE(service.activeEndpoint(QMediaService::AudioDevice), device1.name);
    QCOMPARE(service.setActiveEndpoint(QMediaService::AudioDevice, QString()), true);
    QCOMPARE(service.activeEndpoint(QMediaService::AudioDevice), QString());
}

QTEST_MAIN(tst_QMediaService)

#include "tst_qmediaservice.moc"
