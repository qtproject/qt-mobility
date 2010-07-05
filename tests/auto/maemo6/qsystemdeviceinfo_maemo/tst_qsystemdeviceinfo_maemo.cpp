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
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
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
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/
#include <QtTest/QtTest>
#include <QDebug>
#include <QDBusInterface>
#include <QDBusConnection>
#include <QDBusReply>
#include "qsysteminfo.h"

QTM_USE_NAMESPACE

const static QString PROFILE_GENERAL = "general";
const static QString PROFILE_SILENT  = "silent";

static QDBusInterface profiledConnectionInterface("com.nokia.profiled",
                                                  "/com/nokia/profiled",
                                                  "com.nokia.profiled",
                                                  QDBusConnection::sessionBus());

static QDBusInterface *bluezAdapter = 0;

/**
 * Starts an event loop that runs until the given signal is received.
 * Optionally the event loop can return earlier on a timeout.
 *
 * \return \p true if the requested signal was received
 *         \p false on timeout
 */
static bool waitForSignal(QObject *obj, const char *signal, int timeout = 0)
{
    QEventLoop loop;
    QObject::connect(obj, signal, &loop, SLOT(quit()));
    QTimer timer;
    QSignalSpy timeoutSpy(&timer, SIGNAL(timeout()));
    if (timeout > 0) {
        QObject::connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
        timer.setSingleShot(true);
        timer.start(timeout);
    }
    loop.exec();
    return timeoutSpy.isEmpty();
}

static void log(QString message)
{
    qDebug() << QDateTime::currentDateTime().toTime_t() << " " << message;
}

static void setProfile(QString profile)
{
    profiledConnectionInterface.call("set_profile", profile);
}

static void setBluetoothPowered(bool powered)
{
    bluezAdapter->call("SetProperty", "Powered", qVariantFromValue(QDBusVariant(powered)));
}

class ChangeProfileToSilentThread : public QThread
{
public:
    void run()
    {
        log("ChangeProfileToSilentThread: Waiting 2s before changing profile to silent");
        sleep(2);
        setProfile(PROFILE_SILENT);
        log("ChangeProfileToSilentThread: profile changed to silent");
    }
};

class SwitchBluetoothOnThread : public QThread
{
public:
    void run()
    {
        log("SwitchBluetoothOnThread: Waiting 2s before switching bluetooth on.");
        sleep(2);
        setBluetoothPowered(true);
        log("SwitchBluetoothOnThread: Bluetooth switched on.");
    }
};

class tst_QSystemDeviceInfo : public QObject
{
    Q_OBJECT

public:
    tst_QSystemDeviceInfo();
    virtual ~tst_QSystemDeviceInfo();

public slots:
    void init();
    void cleanup();

private slots:
    void currentProfileChanged();
    void bluetoothStateChanged();

private:
    QSystemDeviceInfo *m_systemDeviceInfo;
    ChangeProfileToSilentThread *m_changeProfileToSilentThread;
    SwitchBluetoothOnThread *m_switchBluetoothOnThread;
};

tst_QSystemDeviceInfo::tst_QSystemDeviceInfo()
{
    // Query the default adapter from Bluez Manager
    QDBusInterface bluezManager("org.bluez", "/", "org.bluez.Manager", QDBusConnection::systemBus());
    QDBusReply<QDBusObjectPath> reply = bluezManager.call("DefaultAdapter");
    QVERIFY(reply.isValid());
    QString defaultAdapter = reply.value().path();
    QVERIFY(defaultAdapter.length() > 0);

    bluezAdapter = new QDBusInterface("org.bluez", defaultAdapter, "org.bluez.Adapter", QDBusConnection::systemBus());

    m_systemDeviceInfo = new QSystemDeviceInfo();
    m_changeProfileToSilentThread = new ChangeProfileToSilentThread();
    m_switchBluetoothOnThread = new SwitchBluetoothOnThread();
}

tst_QSystemDeviceInfo::~tst_QSystemDeviceInfo()
{
    delete m_changeProfileToSilentThread, m_changeProfileToSilentThread = 0;
    delete m_switchBluetoothOnThread, m_switchBluetoothOnThread = 0;
    delete m_systemDeviceInfo, m_systemDeviceInfo = 0;

    delete bluezAdapter, bluezAdapter = 0;
}

/*
 * Called before each test case
 */
void tst_QSystemDeviceInfo::init()
{
    log("init: setProfile: general, bluetooth powered = false");
    setProfile(PROFILE_GENERAL);
    setBluetoothPowered(false);
    QTest::qWait(2000);
    log("init done");
}

/*
 * Called after each test case
 */
void tst_QSystemDeviceInfo::cleanup()
{
    QTest::qWait(2000);
    log("cleanup: setProfile: general, bluetooth powered = false");
    setProfile(PROFILE_GENERAL);
    setBluetoothPowered(false);
    QTest::qWait(2000);
    log("cleanup done");
}

/*
 * The test cases
 */
void tst_QSystemDeviceInfo::currentProfileChanged()
{
    m_changeProfileToSilentThread->start();

    log("Waiting 10s for the current profile to change");
    QVERIFY(::waitForSignal(m_systemDeviceInfo, SIGNAL(currentProfileChanged(QSystemDeviceInfo::Profile)), 10 * 1000));
    log("currentProfileChanged");
}

void tst_QSystemDeviceInfo::bluetoothStateChanged()
{
    m_switchBluetoothOnThread->start();

    log("Waiting 10s to get a bluetooth state change");
    QVERIFY(::waitForSignal(m_systemDeviceInfo, SIGNAL(bluetoothStateChanged(bool)), 10 * 1000));
    log("bluetoothStateChanged");
}

QTEST_MAIN(tst_QSystemDeviceInfo)
#include "tst_qsystemdeviceinfo_maemo.moc"
