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
#include "qdeclarativebatteryinfo_p.h"
#include <QDebug>

QTM_USE_NAMESPACE
Q_DECLARE_METATYPE(QSystemBatteryInfo::BatteryStatus);
Q_DECLARE_METATYPE(QSystemBatteryInfo::ChargingState);
Q_DECLARE_METATYPE(QSystemBatteryInfo::ChargerType);
Q_DECLARE_METATYPE(QSystemBatteryInfo::EnergyUnit);

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

//class ChangeBatteryThread : public QThread
//{
//public:
//    void run()
//    {
//        QDeclarativeBatteryInfo bi;
//        QSystemBatteryInfoPrivate *bip = bi.priv;
//      }

//};

class tst_QDeclarativeBatteryInfo : public QObject
{
    Q_OBJECT

public:
    tst_QDeclarativeBatteryInfo();
    virtual ~tst_QDeclarativeBatteryInfo();

    friend class QSystemBatteryInfo;

private slots:
    void initTestCase();

    void tst_chargerType();
    void tst_chargingState();

    void tst_nominalCapacity();
    void tst_remainingCapacityPercent();
    void tst_remainingCapacity();

    void tst_voltage();
    void tst_remainingChargingTime();
    void tst_currentFlow();
    void tst_remainingCapacityBars();
    void tst_maxBars();
    void tst_batteryStatus();
    void tst_energyMeasurementUnit();


    //signal tests
//    void tst_batteryLevelChanged();
//    void tst_batteryStatusChanged();


//    void tst_chargingStateChanged();
//    void tst_chargerTypeChanged();

//    void tst_nominalCapacityChanged();
//    void tst_remainingCapacityPercentChanged();
//    void tst_remainingCapacityChanged();
//    void tst_voltageChanged();

//    void tst_currentFlowChanged();
//    void tst_cumulativeCurrentFlowChanged();
//    void tst_remainingCapacityBarsChanged();
//    void tst_remainingChargingTimeChanged();


//    //signals
//    void batteryLevelChanged(int level);
//    void batteryStatusChanged(QSystemBatteryInfo::BatteryStatus batteryStatus);


//    void chargingStateChanged(QSystemBatteryInfo::ChargingState chargingState);
//    void chargerTypeChanged(QSystemBatteryInfo::ChargerType chargerType);

//    void nominalCapacityChanged(int);
//    void remainingCapacityPercentChanged(int);
//    void remainingCapacityChanged(int);
//    void voltageChanged(int);

//    void currentFlowChanged(int);
//    void cumulativeCurrentFlowChanged(int);
//    void remainingCapacityBarsChanged(int);
//    void remainingChargingTimeChanged(int);


//    ChangeBatteryThread *changeBatteryThread;
};

tst_QDeclarativeBatteryInfo::tst_QDeclarativeBatteryInfo()
{
  //  changeBatteryThread = new ChangeBatteryThread();
}

tst_QDeclarativeBatteryInfo::~tst_QDeclarativeBatteryInfo()
{
    //delete changeBatteryThread, changeBatteryThread = 0;
}
/*
    SystemInfoConnection si;
    QSystemBatteryInfoPrivate *bip = si.batteryInfoPrivate();
*/

void tst_QDeclarativeBatteryInfo::initTestCase()
{
    qRegisterMetaType<QSystemBatteryInfo::BatteryStatus>("QSystemBatteryInfo::BatteryStatus");
    qRegisterMetaType<QSystemBatteryInfo::ChargingState>("QSystemBatteryInfo::ChargingState");
    qRegisterMetaType<QSystemBatteryInfo::ChargerType>("QSystemBatteryInfo::ChargerType");
    qRegisterMetaType<QSystemBatteryInfo::EnergyUnit>("QSystemBatteryInfo::EnergyUnit");
}

void tst_QDeclarativeBatteryInfo::tst_chargerType()
{
    QDeclarativeBatteryInfo bi;
    QSystemBatteryInfo::ChargerType cType = bi.chargerType();
    if (bi.batteryStatus() == QSystemBatteryInfo::BatteryUnknown) {
     QVERIFY(cType == QSystemBatteryInfo::NoCharger
             || cType == QSystemBatteryInfo::UnknownCharger);
    } else {
        QVERIFY(cType == QSystemBatteryInfo::WallCharger
                || cType == QSystemBatteryInfo::USBCharger
                || cType == QSystemBatteryInfo::USB_500mACharger
                || cType == QSystemBatteryInfo::USB_100mACharger
                || cType == QSystemBatteryInfo::VariableCurrentCharger);

    }
}

void tst_QDeclarativeBatteryInfo::tst_chargingState()
{
    QDeclarativeBatteryInfo bi;
    QSystemBatteryInfo::ChargingState cState = bi.chargingState();

    if (bi.batteryStatus() == QSystemBatteryInfo::BatteryUnknown) {
        QVERIFY(cState == QSystemBatteryInfo::NotCharging
                || cState == QSystemBatteryInfo::ChargingError);
    } else {
        QVERIFY(cState == QSystemBatteryInfo::Charging
                || cState == QSystemBatteryInfo::NotCharging);
    }
}

    QDeclarativeBatteryInfo bi;
void tst_QDeclarativeBatteryInfo::tst_nominalCapacity()
{
    QDeclarativeBatteryInfo bi;
    int nom = bi.nominalCapacity();
    QVERIFY(nom != -1);
    if (bi.batteryStatus() == QSystemBatteryInfo::BatteryUnknown) {
    } else {

    }
}

void tst_QDeclarativeBatteryInfo::tst_remainingCapacityPercent()
{
    QDeclarativeBatteryInfo bi;
    int rem = bi.remainingCapacityPercent();
    QVERIFY(rem != -1);
    if (bi.batteryStatus() == QSystemBatteryInfo::BatteryUnknown) {
    } else {

    }
}

void tst_QDeclarativeBatteryInfo::tst_remainingCapacity()
{
    QDeclarativeBatteryInfo bi;
    int rem = bi.remainingCapacity();
    QVERIFY(rem != -1);
    if (bi.batteryStatus() == QSystemBatteryInfo::BatteryUnknown) {
    } else {

    }
}

void tst_QDeclarativeBatteryInfo::tst_voltage()
{
    QDeclarativeBatteryInfo bi;
    int volt = bi.voltage();
    QVERIFY(volt != -1);
    if (bi.batteryStatus() == QSystemBatteryInfo::BatteryUnknown) {
    } else {

    }
}

void tst_QDeclarativeBatteryInfo::tst_remainingChargingTime()
{
    QDeclarativeBatteryInfo bi;
    int rem = bi.remainingChargingTime();
    QVERIFY(rem != -1);
    if (bi.batteryStatus() == QSystemBatteryInfo::BatteryUnknown) {
    } else {

    }
}

void tst_QDeclarativeBatteryInfo::tst_currentFlow()
{
    QDeclarativeBatteryInfo bi;
    int flow = bi.currentFlow();
    QVERIFY(flow != -1);
    if (bi.batteryStatus() == QSystemBatteryInfo::BatteryUnknown) {
    } else {

    }
}

void tst_QDeclarativeBatteryInfo::tst_remainingCapacityBars()
{
    QDeclarativeBatteryInfo bi;
    int rem = bi.remainingCapacityBars();
    QVERIFY(rem != -1);
    if (bi.batteryStatus() == QSystemBatteryInfo::BatteryUnknown) {
    } else {

    }
}

void tst_QDeclarativeBatteryInfo::tst_maxBars()
{
    QDeclarativeBatteryInfo bi;
    int max = bi.maxBars();
    QVERIFY(max != -1);
    if (bi.batteryStatus() == QSystemBatteryInfo::BatteryUnknown) {
    } else {

    }
}

void tst_QDeclarativeBatteryInfo::tst_batteryStatus()
{
    QDeclarativeBatteryInfo bi;
    QSystemBatteryInfo::BatteryStatus bstatus = bi.batteryStatus();
    QVERIFY(bstatus != -1);
    if (bi.batteryStatus() == QSystemBatteryInfo::BatteryUnknown) {
    } else {

    }
}

void tst_QDeclarativeBatteryInfo::tst_energyMeasurementUnit()
{
    QDeclarativeBatteryInfo bi;
    if (bi.batteryStatus() == QSystemBatteryInfo::BatteryUnknown) {
    } else {

    }
}

////signal tests
//void tst_QDeclarativeBatteryInfo::tst_batteryLevelChanged()
//{
//    QDeclarativeBatteryInfo bi;

//}

//void tst_QDeclarativeBatteryInfo::tst_batteryStatusChanged()
//{
//    QDeclarativeBatteryInfo bi;

//}

//void tst_QDeclarativeBatteryInfo::tst_chargingStateChanged()
//{
//    QDeclarativeBatteryInfo bi;

//}

//void tst_QDeclarativeBatteryInfo::tst_chargerTypeChanged()
//{
//    QDeclarativeBatteryInfo bi;

//}

//void tst_QDeclarativeBatteryInfo::tst_nominalCapacityChanged()
//{
//    QDeclarativeBatteryInfo bi;

//}

//void tst_QDeclarativeBatteryInfo::tst_remainingCapacityPercentChanged()
//{
//    QDeclarativeBatteryInfo bi;

//}

//void tst_QDeclarativeBatteryInfo::tst_remainingCapacityChanged()
//{
//    QDeclarativeBatteryInfo bi;

//}

//void tst_QDeclarativeBatteryInfo::tst_voltageChanged()
//{
//    QDeclarativeBatteryInfo bi;

//}

//void tst_QDeclarativeBatteryInfo::tst_currentFlowChanged()
//{
//    QDeclarativeBatteryInfo bi;

//}

//void tst_QDeclarativeBatteryInfo::tst_cumulativeCurrentFlowChanged()
//{
//    QDeclarativeBatteryInfo bi;

//}

//void tst_QDeclarativeBatteryInfo::tst_remainingCapacityBarsChanged()
//{
//    QDeclarativeBatteryInfo bi;

//}

//void tst_QDeclarativeBatteryInfo::tst_remainingChargingTimeChanged()
//{
//    QDeclarativeBatteryInfo bi;

//}

////signals
//void tst_QDeclarativeBatteryInfo::batteryLevelChanged(int level)
//{

//}

//void tst_QDeclarativeBatteryInfo::batteryStatusChanged(QSystemBatteryInfo::BatteryStatus batteryStatus)
//{

//}

//void tst_QDeclarativeBatteryInfo::chargingStateChanged(QSystemBatteryInfo::ChargingState chargingState)
//{

//}

//void tst_QDeclarativeBatteryInfo::chargerTypeChanged(QSystemBatteryInfo::ChargerType chargerType)
//{

//}

//void tst_QDeclarativeBatteryInfo::nominalCapacityChanged(int)
//{

//}

//void tst_QDeclarativeBatteryInfo::remainingCapacityPercentChanged(int)
//{

//}

//void tst_QDeclarativeBatteryInfo::remainingCapacityChanged(int)
//{

//}

//void tst_QDeclarativeBatteryInfo::voltageChanged(int)
//{

//}

//void tst_QDeclarativeBatteryInfo::currentFlowChanged(int)
//{

//}

//void tst_QDeclarativeBatteryInfo::cumulativeCurrentFlowChanged(int)
//{

//}

//void tst_QDeclarativeBatteryInfo::remainingCapacityBarsChanged(int)
//{

//}

//void tst_QDeclarativeBatteryInfo::remainingChargingTimeChanged(int)
//{

//}


QTEST_MAIN(tst_QDeclarativeBatteryInfo)
#include "tst_qdeclarativebatteryinfo.moc"
