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
#ifdef TESTR
#include "qsysteminfo_simulator_p.h"
#endif

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


class ChangeBatteryThread : public QThread
{
public:
    void run()
    {
        QMutexLocker locker(&mutex);
        SystemInfoConnection si;
        QSystemBatteryInfoPrivate *d = si.batteryInfoPrivate();

        d->setBatteryStatus(currentBatStatus);
        d->setChargerType(curChargeType);
        d->setChargingState(curChargeState);

        d->setNominalCapacity(capacity);
        d->setRemainingCapacityPercent(currentBatLevelPercent);
        d->setRemainingCapacity(remainingCapacity);

        d->setVoltage(currentVoltage);
        d->setRemainingChargingTime(remainingEnergy);
        d->setCurrentFlow(dischargeRate);
//        d->setRemainingCapacityBars(int v);
//        d->setMaxBars(int v);


        this->exit();
      }
    QMutex mutex;
    QSystemBatteryInfo::BatteryStatus currentBatStatus;
    QSystemBatteryInfo::ChargingState curChargeState;
    QSystemBatteryInfo::ChargerType curChargeType;
    QVariantMap pMap;

    int currentBatLevelPercent;
    int currentVoltage;
    int dischargeRate;
    int capacity;
    int timeToFull;
    int remainingEnergy;
    int remainingCapacity;
//    int capBars;
};

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


#ifdef TESTR
    //signal tests
    void tst_batteryStatusChanged_data();
    void tst_batteryStatusChanged();

    void tst_chargingStateChanged_data();
    void tst_chargingStateChanged();

    void tst_chargerTypeChanged_data();
    void tst_chargerTypeChanged();

    void tst_nominalCapacityChanged_data();
    void tst_nominalCapacityChanged();

    void tst_remainingCapacityPercentChanged_data();
    void tst_remainingCapacityPercentChanged();

    void tst_remainingCapacityChanged_data();
    void tst_remainingCapacityChanged();

    void tst_currentFlowChanged_data();
    void tst_currentFlowChanged();

    void tst_remainingCapacityBarsChanged();
    void tst_remainingChargingTimeChanged();


    //signals
    void batteryStatusChanged(QSystemBatteryInfo::BatteryStatus batteryStatus);


    void chargingStateChanged(QSystemBatteryInfo::ChargingState chargingState);
    void chargerTypeChanged(QSystemBatteryInfo::ChargerType chargerType);

    void nominalCapacityChanged(int);
    void remainingCapacityPercentChanged(int);
    void remainingCapacityChanged(int);

    void currentFlowChanged(int);
//    void remainingCapacityBarsChanged(int);
//    void remainingChargingTimeChanged(int);
#endif

private:

#ifdef TESTR
    QSystemBatteryInfo::BatteryStatus currentBatStatus;
    QSystemBatteryInfo::ChargingState curChargeState;
    QSystemBatteryInfo::ChargerType curChargeType;

    int currentBatLevelPercent;
    int currentVoltage;
    int dischargeRate;
    int capacity;
    int timeToFull;
    int remainingEnergy;
    int remainingCapacity;
//    int capBars;
#endif
};

tst_QDeclarativeBatteryInfo::tst_QDeclarativeBatteryInfo()
{
}

tst_QDeclarativeBatteryInfo::~tst_QDeclarativeBatteryInfo()
{
}
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
        if(bi.chargingState() == QSystemBatteryInfo::Charging) {
            QVERIFY(cType == QSystemBatteryInfo::WallCharger
                    || cType == QSystemBatteryInfo::USBCharger
                    || cType == QSystemBatteryInfo::USB_500mACharger
                    || cType == QSystemBatteryInfo::USB_100mACharger
                    || cType == QSystemBatteryInfo::VariableCurrentCharger);
        } else {
            if(bi.batteryStatus() == QSystemBatteryInfo::BatteryUnknown) {
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
    if (bi.batteryStatus() == QSystemBatteryInfo::BatteryUnknown) {
        QVERIFY(nom == -1);
    } else {
        QVERIFY(nom != -1);

    }
}

void tst_QDeclarativeBatteryInfo::tst_remainingCapacityPercent()
{
    QDeclarativeBatteryInfo bi;
    int rem = bi.remainingCapacityPercent();
    if (bi.batteryStatus() == QSystemBatteryInfo::BatteryUnknown) {
        QVERIFY(rem == -1);
    } else {
        QVERIFY(rem > -1 && rem < 101);
    }
}

void tst_QDeclarativeBatteryInfo::tst_remainingCapacity()
{
    QDeclarativeBatteryInfo bi;
    int rem = bi.remainingCapacity();
    if (bi.batteryStatus() == QSystemBatteryInfo::BatteryUnknown) {
        QVERIFY(rem == -1);
    } else {
        QVERIFY(rem != -1);
    }
}

void tst_QDeclarativeBatteryInfo::tst_voltage()
{
    QDeclarativeBatteryInfo bi;
    int volt = bi.voltage();
    if (bi.batteryStatus() == QSystemBatteryInfo::BatteryUnknown) {
        QVERIFY(volt == -1);
    } else {
        QVERIFY(volt != -1);
    }
}

void tst_QDeclarativeBatteryInfo::tst_remainingChargingTime()
{
    QDeclarativeBatteryInfo bi;
    int rem = bi.remainingChargingTime();
    if (bi.batteryStatus() == QSystemBatteryInfo::BatteryUnknown) {
        QVERIFY(rem == -1);
    } else {
        if(bi.batteryStatus() == QSystemBatteryInfo::BatteryFull) {
            QVERIFY(rem == 0);
        } else {
            if (bi.chargingState() == QSystemBatteryInfo::NotCharging) {
                QVERIFY(rem == 0);
            } else {
                QVERIFY(rem != -1);
            }
        }
    }
}

void tst_QDeclarativeBatteryInfo::tst_currentFlow()
{
    QDeclarativeBatteryInfo bi;
    int flow = bi.currentFlow();
    if (bi.batteryStatus() == QSystemBatteryInfo::BatteryUnknown) {
        QVERIFY(flow == 0);
    } else {
        QVERIFY(flow != -1);
    }
}

void tst_QDeclarativeBatteryInfo::tst_remainingCapacityBars()
{
    QDeclarativeBatteryInfo bi;
    bi.remainingCapacityBars();
    if (bi.batteryStatus() == QSystemBatteryInfo::BatteryUnknown) {
   //     QVERIFY(rem == 0);
    } else {
     //   QVERIFY(rem != -1);
    }
}

void tst_QDeclarativeBatteryInfo::tst_maxBars()
{
    QDeclarativeBatteryInfo bi;
    bi.maxBars();
    if (bi.batteryStatus() == QSystemBatteryInfo::BatteryUnknown) {
    } else {

    }
}

void tst_QDeclarativeBatteryInfo::tst_batteryStatus()
{
    QDeclarativeBatteryInfo bi;
    QSystemBatteryInfo::BatteryStatus bstatus = bi.batteryStatus();
    if (bstatus == QSystemBatteryInfo::BatteryUnknown) {
    } else {
        QVERIFY(bstatus != -1);
    }
}

void tst_QDeclarativeBatteryInfo::tst_energyMeasurementUnit()
{
    QDeclarativeBatteryInfo bi;
    if (bi.batteryStatus() == QSystemBatteryInfo::BatteryUnknown) {
        QVERIFY(bi.energyMeasurementUnit() == QSystemBatteryInfo::UnitUnknown);
    } else {

    }
}


#ifdef TESTR
//signal tests
void tst_QDeclarativeBatteryInfo::tst_batteryStatusChanged_data()
{
    QTest::addColumn<QSystemBatteryInfo::BatteryStatus>("batterystatus");

    QTest::newRow("BatteryUnknown") << QSystemBatteryInfo::BatteryUnknown;
    QTest::newRow("BatteryEmpty") << QSystemBatteryInfo::BatteryEmpty;
    QTest::newRow("BatteryCritical") << QSystemBatteryInfo::BatteryCritical;
    QTest::newRow("BatteryVeryLow") << QSystemBatteryInfo::BatteryVeryLow;

    QTest::newRow("BatteryLow") << QSystemBatteryInfo::BatteryLow;
    QTest::newRow("BatteryOk") << QSystemBatteryInfo::BatteryOk;
    QTest::newRow("BatteryFull") << QSystemBatteryInfo::BatteryFull;
}

void tst_QDeclarativeBatteryInfo::tst_batteryStatusChanged()
{
    QDeclarativeBatteryInfo bi;

    ChangeBatteryThread *changeBatThread = new ChangeBatteryThread();

    connect(&bi,SIGNAL(batteryStatusChanged(QSystemBatteryInfo::BatteryStatus)),
            this,SLOT(batteryStatusChanged(QSystemBatteryInfo::BatteryStatus)));

    QFETCH(QSystemBatteryInfo::BatteryStatus, batterystatus);

    changeBatThread->currentBatStatus = currentBatStatus = batterystatus;
    changeBatThread->start();

//    QSignalSpy errorSpy(&bi, SIGNAL(batteryStatusChanged(QSystemBatteryInfo::BatteryStatus)));
//    QVERIFY(::waitForSignal(&bi, SIGNAL(batteryStatusChanged(QSystemBatteryInfo::BatteryStatus)), 10 * 1000));

//    QVERIFY(errorSpy.count() == 1);
}

void tst_QDeclarativeBatteryInfo::tst_chargingStateChanged_data()
{
    QTest::addColumn<QSystemBatteryInfo::ChargingState>("chargingstate");

    QTest::newRow("ChargingError") << QSystemBatteryInfo::ChargingError;
    QTest::newRow("NotCharging") << QSystemBatteryInfo::NotCharging;
    QTest::newRow("Charging") << QSystemBatteryInfo::Charging;
}

void tst_QDeclarativeBatteryInfo::tst_chargingStateChanged()
{
    QDeclarativeBatteryInfo bi;
    ChangeBatteryThread *changeBatThread = new ChangeBatteryThread();

    connect(&bi,SIGNAL(chargingStateChanged(QSystemBatteryInfo::ChargingState)),
            this,SLOT(chargingStateChanged(QSystemBatteryInfo::ChargingState)));

    QFETCH(QSystemBatteryInfo::ChargingState, chargingstate);

    changeBatThread->curChargeState = curChargeState = chargingstate;
    changeBatThread->start();

    QSignalSpy errorSpy(&bi, SIGNAL(chargingStateChanged(QSystemBatteryInfo::ChargingState)));
    QVERIFY(::waitForSignal(&bi, SIGNAL(chargingStateChanged(QSystemBatteryInfo::ChargingState)), 10 * 1000));

    QVERIFY(errorSpy.count() == 1);
}

void tst_QDeclarativeBatteryInfo::tst_chargerTypeChanged_data()
{
    QTest::addColumn<QSystemBatteryInfo::ChargerType>("chargertype");

    QTest::newRow("UnknownCharger") << QSystemBatteryInfo::UnknownCharger;
    QTest::newRow("NoCharger") << QSystemBatteryInfo::NoCharger;
    QTest::newRow("WallCharger") << QSystemBatteryInfo::WallCharger;

    QTest::newRow("USBCharger") << QSystemBatteryInfo::USBCharger;
    QTest::newRow("USB_500mACharger") << QSystemBatteryInfo::USB_500mACharger;
    QTest::newRow("USB_100mACharger") << QSystemBatteryInfo::USB_100mACharger;

    QTest::newRow("VariableCurrentCharger") << QSystemBatteryInfo::VariableCurrentCharger;
}

void tst_QDeclarativeBatteryInfo::tst_chargerTypeChanged()
{
    QDeclarativeBatteryInfo bi;
    ChangeBatteryThread *changeBatThread = new ChangeBatteryThread();

    connect(&bi,SIGNAL(chargerTypeChanged(QSystemBatteryInfo::ChargerType)),
            this,SLOT(chargerTypeChanged(QSystemBatteryInfo::ChargerType)));

    QFETCH(QSystemBatteryInfo::ChargerType, chargertype);

    changeBatThread->curChargeType = curChargeType = chargertype;
    changeBatThread->start();

    QSignalSpy errorSpy(&bi, SIGNAL(chargerTypeChanged(QSystemBatteryInfo::ChargerType)));
    QVERIFY(::waitForSignal(&bi, SIGNAL(chargerTypeChanged(QSystemBatteryInfo::ChargerType)), 10 * 1000));

    QVERIFY(errorSpy.count() == 1);
}

void tst_QDeclarativeBatteryInfo::tst_nominalCapacityChanged_data()
{
    QTest::addColumn<int>("intdata");

    QTest::newRow("1") << 1;
    QTest::newRow("2") << 1234;
    QTest::newRow("3") << 4321;
    QTest::newRow("4") << 9990;
    QTest::newRow("5") << -1;
}

void tst_QDeclarativeBatteryInfo::tst_nominalCapacityChanged()
{
    QDeclarativeBatteryInfo bi;

    ChangeBatteryThread *changeBatThread = new ChangeBatteryThread();

    connect(&bi,SIGNAL(nominalCapacityChanged(int)),
            this,SLOT(nominalCapacityChanged(int)));

    QFETCH(int, intdata);

    changeBatThread->capacity = capacity = intdata;
    changeBatThread->start();

    QSignalSpy errorSpy(&bi, SIGNAL(nominalCapacityChanged(int)));
    QVERIFY(::waitForSignal(&bi, SIGNAL(nominalCapacityChanged(int)), 10 * 1000));

    QVERIFY(errorSpy.count() == 1);
}

void tst_QDeclarativeBatteryInfo::tst_remainingCapacityPercentChanged_data()
{
    QTest::addColumn<int>("batPercent");

    QTest::newRow("1") << 1;
    QTest::newRow("2") << 3;
    QTest::newRow("3") << 5;
    QTest::newRow("4") << 10;
    QTest::newRow("5") << 12;
    QTest::newRow("6") << 40;

    QTest::newRow("4") << 41;
    QTest::newRow("5") << 98;
    QTest::newRow("6") << 99;
    QTest::newRow("7") << 100;

}
void tst_QDeclarativeBatteryInfo::tst_remainingCapacityPercentChanged()
{
    QDeclarativeBatteryInfo bi;

    ChangeBatteryThread *changeBatThread = new ChangeBatteryThread();

    connect(&bi,SIGNAL(remainingCapacityPercentChanged(int)),
            this,SLOT(remainingCapacityPercentChanged(int)));

    QFETCH(int, batPercent);

    changeBatThread->currentBatLevelPercent = currentBatLevelPercent = batPercent;
    changeBatThread->start();

    QSignalSpy errorSpy(&bi, SIGNAL(remainingCapacityPercentChanged(int)));
    QVERIFY(::waitForSignal(&bi, SIGNAL(remainingCapacityPercentChanged(int)), 10 * 1000));

    QVERIFY(errorSpy.count() == 1);
}

void tst_QDeclarativeBatteryInfo::tst_remainingCapacityChanged_data()
{
 tst_nominalCapacityChanged_data();
}

void tst_QDeclarativeBatteryInfo::tst_remainingCapacityChanged()
{
    QDeclarativeBatteryInfo bi;

    ChangeBatteryThread *changeBatThread = new ChangeBatteryThread();

    connect(&bi,SIGNAL(remainingCapacityChanged(int)),
            this,SLOT(remainingCapacityChanged(int)));

    QFETCH(int, intdata);

    changeBatThread->remainingCapacity = remainingCapacity = intdata;
    changeBatThread->start();

    QSignalSpy errorSpy(&bi, SIGNAL(remainingCapacityChanged(int)));
    QVERIFY(::waitForSignal(&bi, SIGNAL(remainingCapacityChanged(int)), 10 * 1000));

    QVERIFY(errorSpy.count() == 1);

}

void tst_QDeclarativeBatteryInfo::tst_currentFlowChanged_data()
{
 tst_nominalCapacityChanged_data();
}

void tst_QDeclarativeBatteryInfo::tst_currentFlowChanged()
{
    QDeclarativeBatteryInfo bi;

    ChangeBatteryThread *changeBatThread = new ChangeBatteryThread();

    connect(&bi,SIGNAL(currentFlowChanged(int)),
            this,SLOT(currentFlowChanged(int)));

    QFETCH(int, intdata);

    changeBatThread->dischargeRate = dischargeRate = intdata;
    changeBatThread->start();

    QSignalSpy errorSpy(&bi, SIGNAL(currentFlowChanged(int)));
    QVERIFY(::waitForSignal(&bi, SIGNAL(currentFlowChanged(int)), 10 * 1000));

    QVERIFY(errorSpy.count() == 1);
}

void tst_QDeclarativeBatteryInfo::tst_remainingCapacityBarsChanged()
{
  //  QDeclarativeBatteryInfo bi;

}

void tst_QDeclarativeBatteryInfo::tst_remainingChargingTimeChanged()
{
  //  QDeclarativeBatteryInfo bi;

}

//signals
void tst_QDeclarativeBatteryInfo::batteryStatusChanged(QSystemBatteryInfo::BatteryStatus batStatus)
{
    QVERIFY(batStatus == currentBatStatus);
}

void tst_QDeclarativeBatteryInfo::chargingStateChanged(QSystemBatteryInfo::ChargingState chargState)
{
    QVERIFY(chargState == curChargeState);

}

void tst_QDeclarativeBatteryInfo::chargerTypeChanged(QSystemBatteryInfo::ChargerType chargType)
{
    QVERIFY(chargType == curChargeType);
}

void tst_QDeclarativeBatteryInfo::nominalCapacityChanged(int cap)
{
    QVERIFY(cap == capacity);
}

void tst_QDeclarativeBatteryInfo::remainingCapacityPercentChanged(int cap)
{
    QVERIFY(cap == currentBatLevelPercent);
}

void tst_QDeclarativeBatteryInfo::remainingCapacityChanged(int cap)
{
    QVERIFY(cap == remainingCapacity);
}

void tst_QDeclarativeBatteryInfo::currentFlowChanged(int flow)
{
    QVERIFY(flow == dischargeRate);
}

//void tst_QDeclarativeBatteryInfo::remainingCapacityBarsChanged(int cap)
//{
//    QVERIFY(cap == currentBatStatus);

//}

//void tst_QDeclarativeBatteryInfo::remainingChargingTimeChanged(int time)
//{
//    QVERIFY(time == currentBatStatus);

//}
#endif


QTEST_MAIN(tst_QDeclarativeBatteryInfo)
#include "tst_qdeclarativebatteryinfo.moc"
