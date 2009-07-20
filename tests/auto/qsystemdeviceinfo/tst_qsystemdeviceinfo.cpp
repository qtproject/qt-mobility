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

//    void tst_batteryLevel_data();
    void tst_batteryLevel();

//    void tst_profileChanged_data();
//    void tst_profileChanged();

    void tst_getSimStatus();

    void tst_isDeviceLocked();

//    void tst_powerState_data();
//    void tst_powerState();

};

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
    qWarning() << di.getInputMethodType();
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

void tst_QSystemDeviceInfo::tst_batteryLevel()
{
    QSystemDeviceInfo di;
// /   QVERIFY(di.batteryLevel() > 0);
}

void tst_QSystemDeviceInfo::tst_getSimStatus()
{
    QSystemDeviceInfo di;

}

void tst_QSystemDeviceInfo::tst_isDeviceLocked()
{
    QSystemDeviceInfo di;

}



QTEST_MAIN(tst_QSystemDeviceInfo)
#include "tst_qsystemdeviceinfo.moc"
