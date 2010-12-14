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
#include "qdeclarativedeviceinfo_p.h"
#include <QDebug>

QTM_USE_NAMESPACE
Q_DECLARE_METATYPE(QSystemDeviceInfo::BatteryStatus);
Q_DECLARE_METATYPE(QSystemDeviceInfo::PowerState);
Q_DECLARE_METATYPE(QSystemDeviceInfo::InputMethodFlags);
Q_DECLARE_METATYPE(QSystemDeviceInfo::Profile);
Q_DECLARE_METATYPE(QSystemDeviceInfo::SimStatus);

Q_DECLARE_METATYPE(QSystemDeviceInfo::KeyboardTypeFlags);

class tst_QDeclarativeDeviceInfo : public QObject
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

};
/*
signal todo:
//    void profileChanged(QSystemDeviceInfo::Profile);
    void batteryLevelChanged(QSystemDeviceInfo::BatteryLevel);
    void batteryLevelCritical(qint32);
    void powerStateChanged(QSystemDeviceInfo::PowerState);
  */

void tst_QDeclarativeDeviceInfo::initTestCase()
{
    qRegisterMetaType<QSystemDeviceInfo::BatteryStatus>("QSystemDeviceInfo::BatteryStatus");
    qRegisterMetaType<QSystemDeviceInfo::PowerState>("QSystemDeviceInfo::PowerState");

    qRegisterMetaType<QSystemDeviceInfo::InputMethodFlags>("QSystemDeviceInfo::InputMethodFlags");
    qRegisterMetaType<QSystemDeviceInfo::Profile>("QSystemDeviceInfo::Profile");

    qRegisterMetaType<QSystemDeviceInfo::SimStatus>("QSystemDeviceInfo::SimStatus");

    qRegisterMetaType<QSystemDeviceInfo::KeyboardTypeFlags>("QSystemDeviceInfo::KeyboardTypeFlags");
    qRegisterMetaType<QSystemDeviceInfo::LockType>("QSystemDeviceInfo::LockType");

}

void tst_QDeclarativeDeviceInfo::tst_inputMethodType()
{
    QDeclarativeDeviceInfo di;
    QVERIFY( di.inputMethodType() != 0
        || di.inputMethodType() == 0);
}

void tst_QDeclarativeDeviceInfo::tst_imei()
{
    QDeclarativeDeviceInfo di;
    QString imeiStr =di.imei();
    QVERIFY(!imeiStr.isEmpty() || imeiStr.isEmpty());

}

void tst_QDeclarativeDeviceInfo::tst_imsi()
{
    QDeclarativeDeviceInfo di;
    QString imsiStr = di.imsi();
    QVERIFY(!imsiStr.isEmpty() || imsiStr.isEmpty());

}

void tst_QDeclarativeDeviceInfo::tst_manufacturer()
{
    QDeclarativeDeviceInfo di;
    QString manu = di.manufacturer();
    QVERIFY(!manu.isEmpty() || manu.isEmpty());

}

void tst_QDeclarativeDeviceInfo::tst_model()
{
    QDeclarativeDeviceInfo di;
    QString model = di.model();
    QVERIFY(!model.isEmpty() || model.isEmpty());

}

void tst_QDeclarativeDeviceInfo::tst_productName()
{
    QDeclarativeDeviceInfo di;
    QString product = di.productName();
    QVERIFY(!product.isEmpty() | product.isEmpty());

}

void tst_QDeclarativeDeviceInfo::tst_batteryLevel()
{
    QDeclarativeDeviceInfo di;
    QVERIFY(di.batteryLevel() > -1);

// until we simulate this, or wait the signalspy to olong, this will always fail
//    if(di.currentPowerState() == QSystemDeviceInfo::WallPowerChargingBattery) {
//        QSignalSpy batSpy(&di, SIGNAL(batteryLevelChanged(int)));
//        QVERIFY(!batSpy.isEmpty());
//        int level = batSpy.first().at(0).toInt();
//        QVERIFY( level > -1 || level < 101);
//    }
}

void tst_QDeclarativeDeviceInfo::tst_batteryStatus()
{
    QDeclarativeDeviceInfo di;
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

    // until we simulate this, or wait the signalspy to olong, this will always fail
//    if(di.currentPowerState() == QSystemDeviceInfo::WallPowerChargingBattery) {
//        QSignalSpy batSpy(&di, SIGNAL(batteryStatusChanged(QSystemDeviceInfo::BatteryStatus)));
//        QVERIFY(!batSpy.isEmpty());
//        QSystemDeviceInfo::BatteryStatus status = qvariant_cast<QSystemDeviceInfo::BatteryStatus>(batSpy.first().at(0));
//        QVERIFY( status == QSystemDeviceInfo::NoBatteryLevel
//                 || status == QSystemDeviceInfo::BatteryCritical
//                 || status == QSystemDeviceInfo::BatteryVeryLow
//                 || status == QSystemDeviceInfo::BatteryLow
//                 || status == QSystemDeviceInfo::BatteryNormal);
//    }
}

void tst_QDeclarativeDeviceInfo::tst_currentProfile()
{
    QDeclarativeDeviceInfo di;
    QSystemDeviceInfo::Profile profile = di.currentProfile();
    QVERIFY( profile == QSystemDeviceInfo::UnknownProfile
             || profile == QSystemDeviceInfo::SilentProfile
             || profile == QSystemDeviceInfo::NormalProfile
             || profile == QSystemDeviceInfo::LoudProfile
             || profile == QSystemDeviceInfo::VibProfile
             || profile == QSystemDeviceInfo::OfflineProfile
             || profile == QSystemDeviceInfo::PowersaveProfile
             || profile == QSystemDeviceInfo::CustomProfile);
}

void tst_QDeclarativeDeviceInfo::tst_simStatus()
{
    QDeclarativeDeviceInfo di;
    bool simStat = di.simStatus();
    QVERIFY(simStat == true || simStat == false);

}

void tst_QDeclarativeDeviceInfo::tst_isDeviceLocked()
{
    QDeclarativeDeviceInfo di;
    bool devLock = di.isDeviceLocked();
    QVERIFY(devLock == true || devLock == false);
}

void tst_QDeclarativeDeviceInfo::tst_currentPowerState()
{
    QDeclarativeDeviceInfo di;
    QSystemDeviceInfo::PowerState state = di.currentPowerState();
    QVERIFY( state == QSystemDeviceInfo::UnknownPower
             || state == QSystemDeviceInfo::BatteryPower
             || state == QSystemDeviceInfo::WallPower
             || state == QSystemDeviceInfo::WallPowerChargingBattery);
}

void tst_QDeclarativeDeviceInfo::tst_currentBluetoothPowerState()
{
    QDeclarativeDeviceInfo di;
    bool state = di.currentPowerState();
    QVERIFY(state || !state);
}


void tst_QDeclarativeDeviceInfo::tst_keyboardType()
{
    QDeclarativeDeviceInfo di;
    QSystemDeviceInfo::KeyboardTypeFlags  flags = di.keyboardTypes();

    QVERIFY( (flags && QSystemDeviceInfo::UnknownKeyboard == QSystemDeviceInfo::UnknownKeyboard)
             || (flags && QSystemDeviceInfo::SoftwareKeyboard ==  QSystemDeviceInfo::SoftwareKeyboard)
             || (flags && QSystemDeviceInfo::ITUKeypad ==  QSystemDeviceInfo::ITUKeypad)
             || (flags && QSystemDeviceInfo::HalfQwertyKeyboard == QSystemDeviceInfo::HalfQwertyKeyboard)
             || (flags && QSystemDeviceInfo::FullQwertyKeyboard == QSystemDeviceInfo::FullQwertyKeyboard)
             || (flags && QSystemDeviceInfo::WirelessKeyboard ==  QSystemDeviceInfo::WirelessKeyboard));
}

void tst_QDeclarativeDeviceInfo::tst_isWirelessKeyboardConnected()
{
    QDeclarativeDeviceInfo di;
   bool on = di.isWirelessKeyboardConnected();
   QVERIFY(on || !on);
}

void tst_QDeclarativeDeviceInfo::tst_isKeyboardFlipOpen()
{
    QDeclarativeDeviceInfo di;
    bool on = di.isKeyboardFlippedOpen();
    QVERIFY(on || !on);
}

void tst_QDeclarativeDeviceInfo::tst_keypadLightOn()

{
    QDeclarativeDeviceInfo di;
    bool on = di.keypadLightOn(QSystemDeviceInfo::PrimaryKeypad);
    QVERIFY(on || !on);
    on = di.keypadLightOn(QSystemDeviceInfo::SecondaryKeypad);
    QVERIFY(on || !on);

}

void tst_QDeclarativeDeviceInfo::tst_uniqueID()
{
    QDeclarativeDeviceInfo di;
    QUuid id = di.uniqueDeviceID();
    QVERIFY(id.isNull()|| !id.isNull());
}

void tst_QDeclarativeDeviceInfo::tst_lockStatus()
{
    qDebug() << Q_FUNC_INFO;
    QDeclarativeDeviceInfo di;
    QSystemDeviceInfo::LockType lock = di.lockStatus();
    if (di.isDeviceLocked()) {
        QVERIFY((lock == QSystemDeviceInfo::DeviceLocked)
                || (lock == QSystemDeviceInfo::TouchAndKeyboardLocked));
    } else {
        QVERIFY( lock == QSystemDeviceInfo::UnknownLock);
    }
}

void tst_QDeclarativeDeviceInfo::tst_getActiveProfileDetails()
{
    QDeclarativeDeviceInfo di;
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

QTEST_MAIN(tst_QDeclarativeDeviceInfo)
#include "tst_qdeclarativedeviceinfo.moc"
