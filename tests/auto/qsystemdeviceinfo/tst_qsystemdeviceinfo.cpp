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
#include <QtTest/QtTest>
#include "qsysteminfo.h"

#define QTRY_VERIFY(__expr) \
    do { \
        const int __step = 50; \
        const int __timeout = 5000; \
        if (!(__expr)) { \
            QTest::qWait(0); \
        } \
        for (int __i = 0; __i < __timeout && !(__expr); __i+=__step) { \
            QTest::qWait(__step); \
        } \
        QVERIFY(__expr); \
    } while(0)

Q_DECLARE_METATYPE(QSystemDeviceInfo::BatteryLevel);
Q_DECLARE_METATYPE(QSystemDeviceInfo::PowerState);
Q_DECLARE_METATYPE(QSystemDeviceInfo::InputMethods);
Q_DECLARE_METATYPE(QSystemDeviceInfo::Profile);
Q_DECLARE_METATYPE(QSystemDeviceInfo::SimStatus);


class tst_QSystemDeviceInfo : public QObject
{
    Q_OBJECT

private slots:

    void initTestCase();
    void tst_getInputMethodType();

    void tst_imei();
    void tst_imsi();
    void tst_manufacturer();
    void tst_model();
    void tst_productName();

    void tst_isBatteryCharging();
    void tst_batteryLevel();

    void tst_getCurrentProfile();

    void tst_getSimStatus();

    void tst_isDeviceLocked();

    //    void tst_powerState_data();
//    void tst_powerState();

};
/*
signal todo:
//    void profileChanged(QSystemDeviceInfo::Profile);
    void batteryLevelChanged(QSystemDeviceInfo::BatteryLevel);
    void batteryLevelCritical(qint32);
    void powerStateChanged(QSystemDeviceInfo::PowerState);
  */

void tst_QSystemDeviceInfo::initTestCase()
{
    qRegisterMetaType<QSystemDeviceInfo::BatteryLevel>("QSystemDeviceInfo::BatteryLevel");
    qRegisterMetaType<QSystemDeviceInfo::PowerState>("QSystemDeviceInfo::PowerState");

    qRegisterMetaType<QSystemDeviceInfo::InputMethods>("QSystemDeviceInfo::InputMethods");
    qRegisterMetaType<QSystemDeviceInfo::Profile>("QSystemDeviceInfo::Profile");

    qRegisterMetaType<QSystemDeviceInfo::SimStatus>("QSystemDeviceInfo::SimStatus");
}

void tst_QSystemDeviceInfo::tst_getInputMethodType()
{
    QSystemDeviceInfo di;
    QVERIFY( di.getInputMethodType() != 0);

}

void tst_QSystemDeviceInfo::tst_imei()
{
    QSystemDeviceInfo di;
    QVERIFY(!di.imei().isEmpty());

}

void tst_QSystemDeviceInfo::tst_imsi()
{
    QSystemDeviceInfo di;
    QVERIFY(!di.imsi().isEmpty());

}

void tst_QSystemDeviceInfo::tst_manufacturer()
{
    QSystemDeviceInfo di;
    QVERIFY(!di.manufacturer().isEmpty());

}

void tst_QSystemDeviceInfo::tst_model()
{
    QSystemDeviceInfo di;
    QVERIFY(!di.model().isEmpty());

}

void tst_QSystemDeviceInfo::tst_productName()
{
    QSystemDeviceInfo di;
    QVERIFY(!di.productName().isEmpty());

}

void tst_QSystemDeviceInfo::tst_isBatteryCharging()
{
    QSystemDeviceInfo di;
    QVERIFY(di.isBatteryCharging() == true
            || di.isBatteryCharging() == false);

}

void tst_QSystemDeviceInfo::tst_batteryLevel()
{
    QSystemDeviceInfo di;
    QVERIFY(di.batteryLevel() > -1);
}

void tst_QSystemDeviceInfo::tst_getCurrentProfile()
{
    QSystemDeviceInfo di;
    QSystemDeviceInfo::Profile profile = di.getCurrentProfile();
    QVERIFY( profile == QSystemDeviceInfo::UnknownProfile
             || profile == QSystemDeviceInfo::SilentProfile
             || profile == QSystemDeviceInfo::NormalProfile
             || profile == QSystemDeviceInfo::LoudProfile
             || profile == QSystemDeviceInfo::VibProfile
             || profile == QSystemDeviceInfo::OfflineProfile
             || profile == QSystemDeviceInfo::PowersaveProfile
             || profile == QSystemDeviceInfo::CustomProfile);
}

void tst_QSystemDeviceInfo::tst_getSimStatus()
{
    QSystemDeviceInfo di;
    bool simStat = di.getSimStatus();
    QVERIFY(simStat == true || simStat == false);

}

void tst_QSystemDeviceInfo::tst_isDeviceLocked()
{
    QSystemDeviceInfo di;
    bool devLock = di.isDeviceLocked();
    QVERIFY(devLock == true || devLock == false);
}



QTEST_MAIN(tst_QSystemDeviceInfo)
#include "tst_qsystemdeviceinfo.moc"
