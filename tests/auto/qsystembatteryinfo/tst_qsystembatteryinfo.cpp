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
#include "qsystembatteryinfo.h"
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
//        QSystemBatteryInfo bi;
//        QSystemBatteryInfoPrivate *bip = bi.priv;
//      }

//};

class tst_QSystemBatteryInfo : public QObject
{
    Q_OBJECT

public:
    tst_QSystemBatteryInfo();
    virtual ~tst_QSystemBatteryInfo();

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

tst_QSystemBatteryInfo::tst_QSystemBatteryInfo()
{
  //  changeBatteryThread = new ChangeBatteryThread();
}

tst_QSystemBatteryInfo::~tst_QSystemBatteryInfo()
{
    //delete changeBatteryThread, changeBatteryThread = 0;
}
/*
    SystemInfoConnection si;
    QSystemBatteryInfoPrivate *bip = si.batteryInfoPrivate();
*/

void tst_QSystemBatteryInfo::initTestCase()
{
    qRegisterMetaType<QSystemBatteryInfo::BatteryStatus>("QSystemBatteryInfo::BatteryStatus");
    qRegisterMetaType<QSystemBatteryInfo::ChargingState>("QSystemBatteryInfo::ChargingState");
    qRegisterMetaType<QSystemBatteryInfo::ChargerType>("QSystemBatteryInfo::ChargerType");
    qRegisterMetaType<QSystemBatteryInfo::EnergyUnit>("QSystemBatteryInfo::EnergyUnit");
}

void tst_QSystemBatteryInfo::tst_chargerType()
{
    QSystemBatteryInfo bi;
    QSystemBatteryInfo::ChargerType cType = bi.chargerType();
    if (bi.batteryStatus() == QSystemBatteryInfo::BatteryUnknown) {
     QVERIFY(cType == QSystemBatteryInfo::NoCharger
             || cType == QSystemBatteryInfo::UnknownCharger);
    } else {
        if(bi.chargingState() == QSystemBatteryInfo::Charging) {
            QVERIFY(cType == QSystemBatteryInfo::WallCharger
                    || cType == QSystemBatteryInfo::USBCharger
                    || cType == QSystemBatteryInfo::USB_500mACharger
                    || cType == QSystemBatteryInfo::USB_100mACharger
                    || cType == QSystemBatteryInfo::VariableCurrentCharger);
        } else {
            QVERIFY(cType == QSystemBatteryInfo::NoCharger
                    || cType == QSystemBatteryInfo::UnknownCharger);
        }
    }
}

void tst_QSystemBatteryInfo::tst_chargingState()
{
    QSystemBatteryInfo bi;
    QSystemBatteryInfo::ChargingState cState = bi.chargingState();

    if (bi.batteryStatus() == QSystemBatteryInfo::BatteryUnknown) {
        QVERIFY(cState == QSystemBatteryInfo::NotCharging
                || cState == QSystemBatteryInfo::ChargingError);
    } else {
        QVERIFY(cState == QSystemBatteryInfo::Charging
                || cState == QSystemBatteryInfo::NotCharging);
    }
}

    QSystemBatteryInfo bi;
void tst_QSystemBatteryInfo::tst_nominalCapacity()
{
    QSystemBatteryInfo bi;
    int nom = bi.nominalCapacity();
    if (bi.batteryStatus() == QSystemBatteryInfo::BatteryUnknown) {
        QVERIFY(nom == -1);
    } else {
        QVERIFY(nom != -1);

    }
}

void tst_QSystemBatteryInfo::tst_remainingCapacityPercent()
{
    QSystemBatteryInfo bi;
    int rem = bi.remainingCapacityPercent();
    if (bi.batteryStatus() == QSystemBatteryInfo::BatteryUnknown) {
        QVERIFY(rem == -1);
    } else {
        QVERIFY(rem > -1 && rem < 101);
    }
}

void tst_QSystemBatteryInfo::tst_remainingCapacity()
{
    QSystemBatteryInfo bi;
    int rem = bi.remainingCapacity();
    if (bi.batteryStatus() == QSystemBatteryInfo::BatteryUnknown) {
        QVERIFY(rem == -1);
    } else {
        QVERIFY(rem != -1);
    }
}

void tst_QSystemBatteryInfo::tst_voltage()
{
    QSystemBatteryInfo bi;
    int volt = bi.voltage();
    if (bi.batteryStatus() == QSystemBatteryInfo::BatteryUnknown) {
        QVERIFY(volt == -1);
    } else {
        QVERIFY(volt != -1);
    }
}

void tst_QSystemBatteryInfo::tst_remainingChargingTime()
{
    QSystemBatteryInfo bi;
    int rem = bi.remainingChargingTime();
    if (bi.batteryStatus() == QSystemBatteryInfo::BatteryUnknown) {
        QVERIFY(rem == -1);
    } else {
        if(bi.batteryStatus() == QSystemBatteryInfo::BatteryFull) {
            QVERIFY(rem == 0);
        } else {
            QVERIFY(rem != -1);
        }
    }
}

void tst_QSystemBatteryInfo::tst_currentFlow()
{
    QSystemBatteryInfo bi;
    int flow = bi.currentFlow();
    if (bi.batteryStatus() == QSystemBatteryInfo::BatteryUnknown) {
        QVERIFY(flow == 0);
    } else {
        QVERIFY(flow != -1);
    }
}

void tst_QSystemBatteryInfo::tst_remainingCapacityBars()
{
    QSystemBatteryInfo bi;
    int rem = bi.remainingCapacityBars();
    if (bi.batteryStatus() == QSystemBatteryInfo::BatteryUnknown) {
   //     QVERIFY(rem == 0);
    } else {
     //   QVERIFY(rem != -1);
    }
}

void tst_QSystemBatteryInfo::tst_maxBars()
{
    QSystemBatteryInfo bi;
    int max = bi.maxBars();
    if (bi.batteryStatus() == QSystemBatteryInfo::BatteryUnknown) {
    } else {

    }
}

void tst_QSystemBatteryInfo::tst_batteryStatus()
{
    QSystemBatteryInfo bi;
    QSystemBatteryInfo::BatteryStatus bstatus = bi.batteryStatus();
    if (bstatus == QSystemBatteryInfo::BatteryUnknown) {
    } else {
        QVERIFY(bstatus != -1);
    }
}

void tst_QSystemBatteryInfo::tst_energyMeasurementUnit()
{
    QSystemBatteryInfo bi;
    if (bi.batteryStatus() == QSystemBatteryInfo::BatteryUnknown) {
        QVERIFY(bi.energyMeasurementUnit() == QSystemBatteryInfo::UnitUnknown);
    } else {

    }
}

////signal tests
//void tst_QSystemBatteryInfo::tst_batteryLevelChanged()
//{
//    QSystemBatteryInfo bi;

//}

//void tst_QSystemBatteryInfo::tst_batteryStatusChanged()
//{
//    QSystemBatteryInfo bi;

//}

//void tst_QSystemBatteryInfo::tst_chargingStateChanged()
//{
//    QSystemBatteryInfo bi;

//}

//void tst_QSystemBatteryInfo::tst_chargerTypeChanged()
//{
//    QSystemBatteryInfo bi;

//}

//void tst_QSystemBatteryInfo::tst_nominalCapacityChanged()
//{
//    QSystemBatteryInfo bi;

//}

//void tst_QSystemBatteryInfo::tst_remainingCapacityPercentChanged()
//{
//    QSystemBatteryInfo bi;

//}

//void tst_QSystemBatteryInfo::tst_remainingCapacityChanged()
//{
//    QSystemBatteryInfo bi;

//}

//void tst_QSystemBatteryInfo::tst_voltageChanged()
//{
//    QSystemBatteryInfo bi;

//}

//void tst_QSystemBatteryInfo::tst_currentFlowChanged()
//{
//    QSystemBatteryInfo bi;

//}

//void tst_QSystemBatteryInfo::tst_cumulativeCurrentFlowChanged()
//{
//    QSystemBatteryInfo bi;

//}

//void tst_QSystemBatteryInfo::tst_remainingCapacityBarsChanged()
//{
//    QSystemBatteryInfo bi;

//}

//void tst_QSystemBatteryInfo::tst_remainingChargingTimeChanged()
//{
//    QSystemBatteryInfo bi;

//}

////signals
//void tst_QSystemBatteryInfo::batteryLevelChanged(int level)
//{

//}

//void tst_QSystemBatteryInfo::batteryStatusChanged(QSystemBatteryInfo::BatteryStatus batteryStatus)
//{

//}

//void tst_QSystemBatteryInfo::chargingStateChanged(QSystemBatteryInfo::ChargingState chargingState)
//{

//}

//void tst_QSystemBatteryInfo::chargerTypeChanged(QSystemBatteryInfo::ChargerType chargerType)
//{

//}

//void tst_QSystemBatteryInfo::nominalCapacityChanged(int)
//{

//}

//void tst_QSystemBatteryInfo::remainingCapacityPercentChanged(int)
//{

//}

//void tst_QSystemBatteryInfo::remainingCapacityChanged(int)
//{

//}

//void tst_QSystemBatteryInfo::voltageChanged(int)
//{

//}

//void tst_QSystemBatteryInfo::currentFlowChanged(int)
//{

//}

//void tst_QSystemBatteryInfo::cumulativeCurrentFlowChanged(int)
//{

//}

//void tst_QSystemBatteryInfo::remainingCapacityBarsChanged(int)
//{

//}

//void tst_QSystemBatteryInfo::remainingChargingTimeChanged(int)
//{

//}


QTEST_MAIN(tst_QSystemBatteryInfo)
#include "tst_qsystembatteryinfo.moc"
