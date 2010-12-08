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

//TESTED_COMPONENT=src/systeminfo

#include <QtTest/QtTest>
#include "qsysteminfo.h"
#include "../qsysteminfotestcommon.h"
#include "qsysteminfo_simulator_p.h"

QTM_USE_NAMESPACE
Q_DECLARE_METATYPE(QSystemDeviceInfo::BatteryStatus);
Q_DECLARE_METATYPE(QSystemDeviceInfo::PowerState);
Q_DECLARE_METATYPE(QSystemDeviceInfo::InputMethodFlags);
Q_DECLARE_METATYPE(QSystemDeviceInfo::Profile);
Q_DECLARE_METATYPE(QSystemDeviceInfo::SimStatus);

Q_DECLARE_METATYPE(QSystemDeviceInfo::KeyboardTypeFlags);
Q_DECLARE_METATYPE(QSystemDeviceInfo::LockType);

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

class ChangeDeviceThread : public QThread
{
public:
    void run()
    {
        QMutexLocker locker(&mutex);
        SystemInfoConnection si;
        QSystemDeviceInfoPrivate *d = si.deviceInfoPrivate();
        d->setBatteryLevel(batteryLevel);
        d->setCurrentPowerState(powerState);
        d->setCurrentProfile(profile);
        d->setBluetoothPower(btPower);
        d->setWirelessKeyboardConnected(keyboardConnected);
        d->setKeyboardFlipOpen(flip);
        d->setDeviceLocked(locked);
        d->setTypeOfLock(lockType);

        d->setMessageRingtoneVolume(messageRingtoneVolume);
        d->setVoiceRingtoneVolume(voiceRingtoneVolume);
        d->setVibrationActive(vibrationActive);

        this->exit();
    }

    QMutex mutex;
    QSystemDeviceInfo::PowerState powerState;
    QSystemDeviceInfo::Profile profile;
    int batteryLevel;
    bool btPower;
    bool keyboardConnected;
    bool flip;
    bool locked;
    QSystemDeviceInfo::LockType lockType;
    int messageRingtoneVolume;
    int voiceRingtoneVolume;
    int vibrationActive;
};

class tst_QSystemDeviceInfo : public QObject
{
    Q_OBJECT

private slots:

    void initTestCase();
    void tst_inputMethodType();

    void tst_imei();
    void tst_imsi();
    void tst_manufacturer();
    void tst_model();
    void tst_productName();

    void tst_batteryLevel();
    void tst_batteryStatus();

    void tst_currentProfile();

    void tst_simStatus();

    void tst_isDeviceLocked();

    void tst_currentPowerState();

    void tst_currentBluetoothPowerState();

    void tst_keyboardType();
    void tst_isWirelessKeyboardConnected();
    void tst_isKeyboardFlipOpen();
    void tst_keypadLightOn();
    void tst_uniqueID();
    void tst_lockStatus();
    void tst_getActiveProfileDetails();


    void tst_batteryLevelChanged_data();
    void tst_batteryLevelChanged();

    void tst_batteryStatusChanged_data();
    void tst_batteryStatusChanged();

    void tst_powerStateChanged_data();
    void tst_powerStateChanged();

    void tst_currentProfileChanged_data();
    void tst_currentProfileChanged();

    void tst_bluetoothStateChanged();

    void tst_wirelessKeyboardConnected();
    void tst_keyboardFlip();
    void tst_deviceLocked();

    void tst_lockTypeChanged_data();
    void tst_lockTypeChanged();


    void batteryLevelChanged(int level);
    void batteryStatusChanged(QSystemDeviceInfo::BatteryStatus batteryStatus);
    void powerStateChanged(QSystemDeviceInfo::PowerState powerState);
    void currentProfileChanged(QSystemDeviceInfo::Profile currentProfile);
    void bluetoothStateChanged(bool on);

    void wirelessKeyboardConnected(bool connected);
    void keyboardFlip(bool open);
    void deviceLocked(bool isLocked);
    void lockStatusChanged(QSystemDeviceInfo::LockType);

private:
    int curBatLevel;
    QSystemDeviceInfo::PowerState curPowerState;
    QSystemDeviceInfo::Profile curProfile;
    bool btpower;
    bool keyboardConnect;
    bool keyFLip;
    bool deviceLock;
    QSystemDeviceInfo::LockType lockType;
    bool lockTypeOn;
};

void tst_QSystemDeviceInfo::initTestCase()
{
    qRegisterMetaType<QSystemDeviceInfo::BatteryStatus>("QSystemDeviceInfo::BatteryStatus");
    qRegisterMetaType<QSystemDeviceInfo::PowerState>("QSystemDeviceInfo::PowerState");

    qRegisterMetaType<QSystemDeviceInfo::InputMethodFlags>("QSystemDeviceInfo::InputMethodFlags");
    qRegisterMetaType<QSystemDeviceInfo::Profile>("QSystemDeviceInfo::Profile");

    qRegisterMetaType<QSystemDeviceInfo::SimStatus>("QSystemDeviceInfo::SimStatus");

    qRegisterMetaType<QSystemDeviceInfo::KeyboardTypeFlags>("QSystemDeviceInfo::KeyboardTypeFlags");
    qRegisterMetaType<QSystemDeviceInfo::LockType>("QSystemDeviceInfo::LockType");

}

void tst_QSystemDeviceInfo::tst_inputMethodType()
{
    QSystemDeviceInfo di;
    QVERIFY( di.inputMethodType() != 0
        || di.inputMethodType() == 0);
}

void tst_QSystemDeviceInfo::tst_imei()
{
    QSystemDeviceInfo di;
    QString imeiStr =di.imei();
    QVERIFY(!imeiStr.isEmpty() || imeiStr.isEmpty());
#ifdef TESTR
    QVERIFY(imeiStr == "12-345678-901234-5");
#endif
}

void tst_QSystemDeviceInfo::tst_imsi()
{
    QSystemDeviceInfo di;
    QString imsiStr = di.imsi();
    QVERIFY(!imsiStr.isEmpty() || imsiStr.isEmpty());

#ifdef TESTR
    QVERIFY(imsiStr == "12345679012345");
#endif
}

void tst_QSystemDeviceInfo::tst_manufacturer()
{
    QSystemDeviceInfo di;
    QString manu = di.manufacturer();
    QVERIFY(!manu.isEmpty() || manu.isEmpty());

#ifdef TESTR
    QVERIFY(manu == "simulator manufacturer");
#endif
}

void tst_QSystemDeviceInfo::tst_model()
{
    QSystemDeviceInfo di;
    QString model = di.model();
    QVERIFY(!model.isEmpty() || model.isEmpty());

#ifdef TESTR
    QVERIFY(model == "simulator model");
#endif
}

void tst_QSystemDeviceInfo::tst_productName()
{
    QSystemDeviceInfo di;
    QString product = di.productName();
    QVERIFY(!product.isEmpty() | product.isEmpty());

#ifdef TESTR
    QVERIFY(product == "simulator product name");
#endif
}

void tst_QSystemDeviceInfo::tst_batteryLevel()
{
    QSystemDeviceInfo di;
    QVERIFY(di.batteryLevel() > -1);
#ifdef TESTR
    QVERIFY(di.batteryLevel() == 84);
#endif
}

void tst_QSystemDeviceInfo::tst_batteryStatus()
{
    QSystemDeviceInfo di;
    int level = di.batteryLevel();
    if(level < 4) {
        QVERIFY(di.batteryStatus() ==QSystemDeviceInfo::BatteryCritical );
    }   else if(level < 11) {
        QVERIFY(di.batteryStatus() == QSystemDeviceInfo::BatteryVeryLow);
    }  else if(level < 41) {
        QVERIFY(di.batteryStatus() == QSystemDeviceInfo::BatteryLow);
    }   else if(level > 40) {
        QVERIFY(di.batteryStatus() == QSystemDeviceInfo::BatteryNormal);
    }
}

void tst_QSystemDeviceInfo::tst_currentProfile()
{
    QSystemDeviceInfo di;
    QSystemDeviceInfo::Profile profile = di.currentProfile();
    QVERIFY( profile == QSystemDeviceInfo::UnknownProfile
             || profile == QSystemDeviceInfo::SilentProfile
             || profile == QSystemDeviceInfo::NormalProfile
             || profile == QSystemDeviceInfo::LoudProfile
             || profile == QSystemDeviceInfo::VibProfile
             || profile == QSystemDeviceInfo::OfflineProfile
             || profile == QSystemDeviceInfo::PowersaveProfile
             || profile == QSystemDeviceInfo::CustomProfile);
#ifdef TESTR
    QVERIFY(di.currentProfile() == QSystemDeviceInfo::NormalProfile);
#endif
}

void tst_QSystemDeviceInfo::tst_simStatus()
{
    QSystemDeviceInfo di;
    bool simStat = di.simStatus();
    QVERIFY(simStat == true || simStat == false);
#ifdef TESTR
    QVERIFY(di.simStatus() == QSystemDeviceInfo::SimNotAvailable);
#endif
}

void tst_QSystemDeviceInfo::tst_isDeviceLocked()
{
    QSystemDeviceInfo di;
    bool devLock = di.isDeviceLocked();
    QVERIFY(devLock == true || devLock == false);
#ifdef TESTR
    QVERIFY(!di.isDeviceLocked());
#endif
}

void tst_QSystemDeviceInfo::tst_currentPowerState()
{
    QSystemDeviceInfo di;
    QSystemDeviceInfo::PowerState state = di.currentPowerState();
    QVERIFY( state == QSystemDeviceInfo::UnknownPower
             || state == QSystemDeviceInfo::BatteryPower
             || state == QSystemDeviceInfo::WallPower
             || state == QSystemDeviceInfo::WallPowerChargingBattery);
#ifdef TESTR
    QVERIFY(di.currentPowerState() == QSystemDeviceInfo::WallPower);
#endif
}

void tst_QSystemDeviceInfo::tst_currentBluetoothPowerState()
{
    QSystemDeviceInfo di;
    bool state = di.currentBluetoothPowerState();
    QVERIFY(state || !state);
#ifdef TESTR
    QVERIFY(!di.currentBluetoothPowerState());
#endif
}


void tst_QSystemDeviceInfo::tst_keyboardType()
{
    QSystemDeviceInfo di;
    QSystemDeviceInfo::KeyboardTypeFlags flags = di.keyboardType();

    QVERIFY( (flags && QSystemDeviceInfo::UnknownKeyboard == QSystemDeviceInfo::UnknownKeyboard)
             || (flags && QSystemDeviceInfo::SoftwareKeyboard ==  QSystemDeviceInfo::SoftwareKeyboard)
             || (flags && QSystemDeviceInfo::ITUKeypad ==  QSystemDeviceInfo::ITUKeypad)
             || (flags && QSystemDeviceInfo::HalfQwertyKeyboard == QSystemDeviceInfo::HalfQwertyKeyboard)
             || (flags && QSystemDeviceInfo::FullQwertyKeyboard == QSystemDeviceInfo::FullQwertyKeyboard)
             || (flags && QSystemDeviceInfo::WirelessKeyboard ==  QSystemDeviceInfo::WirelessKeyboard));
}

void tst_QSystemDeviceInfo::tst_isWirelessKeyboardConnected()
{
    QSystemDeviceInfo di;
    bool on = di.isWirelessKeyboardConnected();
    QVERIFY(on || !on);
}

void tst_QSystemDeviceInfo::tst_isKeyboardFlipOpen()
{
    QSystemDeviceInfo di;
    bool on = di.isKeyboardFlipOpen();
    QVERIFY(on || !on);
}

void tst_QSystemDeviceInfo::tst_keypadLightOn()

{
    QSystemDeviceInfo di;
    bool on = di.keypadLightOn(QSystemDeviceInfo::PrimaryKeypad);
    QVERIFY(on || !on);
    on = di.keypadLightOn(QSystemDeviceInfo::SecondaryKeypad);
    QVERIFY(on || !on);

}

void tst_QSystemDeviceInfo::tst_uniqueID()
{
    QSystemDeviceInfo di;
    QUuid id = di.uniqueID();
    QVERIFY(id.isNull()|| !id.isNull());
}

void tst_QSystemDeviceInfo::tst_lockStatus()
{
    QSystemDeviceInfo di;
    QSystemDeviceInfo::LockType lock = di.lockStatus();
    if (di.isDeviceLocked()) {
        QVERIFY((lock == QSystemDeviceInfo::DeviceLocked)
                || (lock == QSystemDeviceInfo::TouchAndKeyboardLocked));
    } else {
        QVERIFY(lock == QSystemDeviceInfo::DeviceUnlocked);
    }
}

void tst_QSystemDeviceInfo::tst_batteryLevelChanged_data()
{
    QTest::addColumn<int>("batLevel");

    QTest::newRow("1c") << 1;//critical
    QTest::newRow("4c") << 4;//very low
    QTest::newRow("11c") << 11;//low
    QTest::newRow("41c") << 41;//normal
    QTest::newRow("40d") << 40;//low
    QTest::newRow("10d") << 10;//very low
    QTest::newRow("3d") << 3;//critical
}

void tst_QSystemDeviceInfo::tst_batteryLevelChanged()
{
    QSystemDeviceInfo di;
    QFETCH(int, batLevel);

    connect(&di,SIGNAL(batteryLevelChanged(int)),
            this,SLOT(batteryLevelChanged(int)));

    ChangeDeviceThread *changeDevThread = new ChangeDeviceThread();
    changeDevThread->batteryLevel = batLevel;
    changeDevThread->start();

    QSignalSpy errorSpy(&di, SIGNAL(batteryLevelChanged(int)));
    QVERIFY(::waitForSignal(&di, SIGNAL(batteryLevelChanged(int)), 10 * 1000));

    QVERIFY(errorSpy.count() == 1);

    curBatLevel = batLevel;
}

void tst_QSystemDeviceInfo::tst_batteryStatusChanged_data()
{
    SystemInfoConnection si;
    QSystemDeviceInfoPrivate *d = si.deviceInfoPrivate();
    d->setInitialData();
    tst_batteryLevelChanged_data();
}

void tst_QSystemDeviceInfo::tst_batteryStatusChanged()
{
    QSystemDeviceInfo di;

    SystemInfoConnection si;

    QFETCH(int, batLevel);

    connect(&di,SIGNAL(batteryStatusChanged(QSystemDeviceInfo::BatteryStatus)),
            this,SLOT(batteryStatusChanged(QSystemDeviceInfo::BatteryStatus)));

    ChangeDeviceThread *changeDevThread = new ChangeDeviceThread();
    changeDevThread->batteryLevel = batLevel;
    changeDevThread->start();

    QSignalSpy errorSpy(&di, SIGNAL(batteryStatusChanged(QSystemDeviceInfo::BatteryStatus)));
    QVERIFY(::waitForSignal(&di, SIGNAL(batteryStatusChanged(QSystemDeviceInfo::BatteryStatus)), 10 * 1000));

    QVERIFY(errorSpy.count() == 1);

    curBatLevel = batLevel;
}

void tst_QSystemDeviceInfo::tst_powerStateChanged_data()
{
    QTest::addColumn<QSystemDeviceInfo::PowerState>("powerstate");

    QTest::newRow("BatteryPower") << QSystemDeviceInfo::BatteryPower;
    QTest::newRow("WallPower") << QSystemDeviceInfo::WallPower;
    QTest::newRow("WallPowerChargingBattery") << QSystemDeviceInfo::WallPowerChargingBattery;
    QTest::newRow("UnknownPower") << QSystemDeviceInfo::UnknownPower;
}

void tst_QSystemDeviceInfo::tst_powerStateChanged()
{
    QSystemDeviceInfo di;

    SystemInfoConnection si;

    QFETCH(QSystemDeviceInfo::PowerState, powerstate);

    connect(&di,SIGNAL(powerStateChanged(QSystemDeviceInfo::PowerState)),
            this,SLOT(powerStateChanged(QSystemDeviceInfo::PowerState)));

    ChangeDeviceThread *changeDevThread = new ChangeDeviceThread();
    changeDevThread->powerState = curPowerState = powerstate;
    changeDevThread->start();

    QSignalSpy errorSpy(&di, SIGNAL(powerStateChanged(QSystemDeviceInfo::PowerState)));
    QVERIFY(::waitForSignal(&di, SIGNAL(powerStateChanged(QSystemDeviceInfo::PowerState)), 10 * 1000));

    QVERIFY(errorSpy.count() == 1);

}

void tst_QSystemDeviceInfo::tst_currentProfileChanged_data()
{
    QTest::addColumn<QSystemDeviceInfo::Profile>("profile");

    QTest::newRow("SilentProfile") << QSystemDeviceInfo::SilentProfile;
    QTest::newRow("NormalProfile") << QSystemDeviceInfo::NormalProfile;
    QTest::newRow("LoudProfile") << QSystemDeviceInfo::LoudProfile;
    QTest::newRow("VibProfile") << QSystemDeviceInfo::VibProfile;
    QTest::newRow("OfflineProfile") << QSystemDeviceInfo::OfflineProfile;
    QTest::newRow("PowersaveProfile") << QSystemDeviceInfo::PowersaveProfile;
    QTest::newRow("CustomProfile") << QSystemDeviceInfo::CustomProfile;
    QTest::newRow("UnknownProfile") << QSystemDeviceInfo::UnknownProfile;

}

void tst_QSystemDeviceInfo::tst_currentProfileChanged()
{
    QSystemDeviceInfo di;

    SystemInfoConnection si;
    QFETCH(QSystemDeviceInfo::Profile, profile);

    connect(&di,SIGNAL(currentProfileChanged(QSystemDeviceInfo::Profile)),
            this,SLOT(currentProfileChanged(QSystemDeviceInfo::Profile)));

    ChangeDeviceThread *changeDevThread = new ChangeDeviceThread();
    changeDevThread->profile = curProfile = profile;
    changeDevThread->start();

    QSignalSpy errorSpy(&di, SIGNAL(currentProfileChanged(QSystemDeviceInfo::Profile)));
    QVERIFY(::waitForSignal(&di, SIGNAL(currentProfileChanged(QSystemDeviceInfo::Profile)), 10 * 1000));

    QVERIFY(errorSpy.count() == 1);
}

void tst_QSystemDeviceInfo::tst_bluetoothStateChanged()
{
    QSystemDeviceInfo di;

    SystemInfoConnection si;
    ChangeDeviceThread *changeDevThread = new ChangeDeviceThread();

    connect(&di,SIGNAL(bluetoothStateChanged(bool)),
            this,SLOT(bluetoothStateChanged(bool)));

    changeDevThread->btPower = btpower = true;
    changeDevThread->start();

    QSignalSpy errorSpy(&di, SIGNAL(bluetoothStateChanged(bool)));
    QVERIFY(::waitForSignal(&di, SIGNAL(bluetoothStateChanged(bool)), 10 * 1000));

    QVERIFY(errorSpy.count() == 1);

    changeDevThread->btPower = btpower = false;
    changeDevThread->start();

}


void tst_QSystemDeviceInfo::tst_wirelessKeyboardConnected()
{
    QSystemDeviceInfo di;

    SystemInfoConnection si;
    ChangeDeviceThread *changeDevThread = new ChangeDeviceThread();

    connect(&di,SIGNAL(wirelessKeyboardConnected(bool)),
            this,SLOT(wirelessKeyboardConnected(bool)));

    changeDevThread->keyboardConnected = keyboardConnect = true;
    changeDevThread->start();

    QSignalSpy errorSpy(&di, SIGNAL(wirelessKeyboardConnected(bool)));
    QVERIFY(::waitForSignal(&di, SIGNAL(wirelessKeyboardConnected(bool)), 10 * 1000));

    QVERIFY(errorSpy.count() == 1);

    changeDevThread->keyboardConnected = keyboardConnect = false;
    changeDevThread->start();

}

void tst_QSystemDeviceInfo::tst_keyboardFlip()
{
    QSystemDeviceInfo di;

    SystemInfoConnection si;
    ChangeDeviceThread *changeDevThread = new ChangeDeviceThread();

    connect(&di,SIGNAL(keyboardFlip(bool)),
            this,SLOT(keyboardFlip(bool)));

    changeDevThread->flip = keyFLip = true;
    changeDevThread->start();

    QSignalSpy errorSpy(&di, SIGNAL(keyboardFlip(bool)));
    QVERIFY(::waitForSignal(&di, SIGNAL(keyboardFlip(bool)), 10 * 1000));

    QVERIFY(errorSpy.count() == 1);

    changeDevThread->flip = keyFLip = false;
    changeDevThread->start();


}

void tst_QSystemDeviceInfo::tst_deviceLocked()
{
    QSystemDeviceInfo di;

    SystemInfoConnection si;
    ChangeDeviceThread *changeDevThread = new ChangeDeviceThread();

    connect(&di,SIGNAL(deviceLocked(bool)),
            this,SLOT(deviceLocked(bool)));

    changeDevThread->locked = deviceLock = true;
    changeDevThread->start();

    QSignalSpy errorSpy(&di, SIGNAL(deviceLocked(bool)));
    QVERIFY(::waitForSignal(&di, SIGNAL(deviceLocked(bool)), 10 * 1000));

    QVERIFY(errorSpy.count() == 1);

    changeDevThread->locked = deviceLock = false;
    changeDevThread->start();


}

void tst_QSystemDeviceInfo::tst_lockTypeChanged_data()
{
    QTest::addColumn<QSystemDeviceInfo::LockType>("locktype");

    QTest::newRow("DeviceLocked") << QSystemDeviceInfo::DeviceLocked;
    QTest::newRow("TouchAndKeyboardLocked") << QSystemDeviceInfo::TouchAndKeyboardLocked;
    QTest::newRow("UnknownLock") << QSystemDeviceInfo::UnknownLock;
}

void tst_QSystemDeviceInfo::tst_lockTypeChanged()
{
    QSystemDeviceInfo di;

    SystemInfoConnection si;
    QFETCH(QSystemDeviceInfo::LockType, locktype);

    connect(&di,SIGNAL(lockStatusChanged(QSystemDeviceInfo::LockType)),
            this,SLOT(lockStatusChanged(QSystemDeviceInfo::LockType)));

    ChangeDeviceThread *changeDevThread = new ChangeDeviceThread();
    changeDevThread->lockType = lockType = locktype;
    changeDevThread->start();

    QSignalSpy errorSpy(&di, SIGNAL(lockStatusChanged(QSystemDeviceInfo::LockType)));
    QVERIFY(::waitForSignal(&di, SIGNAL(lockStatusChanged(QSystemDeviceInfo::LockType)), 10 * 1000));

    QVERIFY(errorSpy.count() == 1);

    changeDevThread->start();
}

void tst_QSystemDeviceInfo::batteryLevelChanged(int level)
{
    QVERIFY(level != curBatLevel);
}

void tst_QSystemDeviceInfo::batteryStatusChanged(QSystemDeviceInfo::BatteryStatus batteryStatus)
{
    QSystemDeviceInfo di;
    int level = di.batteryLevel();
    if (level == -1)
        QVERIFY(batteryStatus ==QSystemDeviceInfo::NoBatteryLevel);
    if (level < 4) {
        QVERIFY(batteryStatus == QSystemDeviceInfo::BatteryCritical);
    } else if (level < 11) {
        QVERIFY(batteryStatus == QSystemDeviceInfo::BatteryVeryLow);
    } else if (level < 41) {
        QVERIFY(batteryStatus == QSystemDeviceInfo::BatteryLow);
    } else if (level > 40) {
        QVERIFY(batteryStatus == QSystemDeviceInfo::BatteryNormal);
    }
}

void tst_QSystemDeviceInfo::powerStateChanged(QSystemDeviceInfo::PowerState powerState)
{
    QVERIFY(powerState == curPowerState);
}

void tst_QSystemDeviceInfo::currentProfileChanged(QSystemDeviceInfo::Profile currentProfile)
{
    QVERIFY(currentProfile == curProfile);
}

void tst_QSystemDeviceInfo::bluetoothStateChanged(bool on)
{
    QVERIFY(on == btpower);
}

void tst_QSystemDeviceInfo::wirelessKeyboardConnected(bool connected)
{
    QVERIFY(connected == keyboardConnect);
}

void tst_QSystemDeviceInfo::keyboardFlip(bool open)
{
    QVERIFY(open == keyFLip);
}

void tst_QSystemDeviceInfo::deviceLocked(bool isLocked)
{
    QVERIFY(isLocked == deviceLock);
}

void tst_QSystemDeviceInfo::lockStatusChanged(QSystemDeviceInfo::LockType type)
{
    QVERIFY(type == lockType);
}


void tst_QSystemDeviceInfo::tst_getActiveProfileDetails()
{
    QSystemDeviceInfo di;
    QSystemDeviceInfo::ActiveProfileDetails *details = di.getActiveProfileDetails();
    int vol = details->messageRingtoneVolume();
    int vol2 = details->voiceRingtoneVolume();
    bool vib = details->vibrationActive();
    if(di.currentProfile() != QSystemDeviceInfo::UnknownProfile) {
        QVERIFY(vol > -1 && vol < 101);
        QVERIFY(vol2 > -1 && vol2 < 101);
        QVERIFY(vib || !vib);
    }
}

QTEST_MAIN(tst_QSystemDeviceInfo)
#include "tst_qsystemdeviceinfo.moc"
