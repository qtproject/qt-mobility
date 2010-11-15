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
#include "qsystemnetworkinfo.h"
#include "qsysteminfo_simulator_p.h"

QTM_USE_NAMESPACE


Q_DECLARE_METATYPE(QSystemNetworkInfo::NetworkStatus);
Q_DECLARE_METATYPE(QSystemNetworkInfo::NetworkMode);

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

class ChangeNetworkThread : public QThread
{
public:
    void run()
    {
        QMutexLocker locker(&mutex);
        SystemInfoConnection si;
        QSystemNetworkInfoPrivate *s = si.networkInfoPrivate();
        s->setNetworkStatus(mode, QSystemNetworkInfo::Denied);

        s->setNetworkSignalStrength(mode, 74);
        s->setCurrentMobileCountryCode("111");
        s->setCurrentMobileNetworkCode("123");
        s->setNetworkName(mode,"qsysteminfo test network");
        s->setCurrentMode(mode);
        s->setCellId(206555390);
        this->exit();

    }
    QSystemNetworkInfo::NetworkMode mode;
    QString str;
    QMutex mutex;
};

class tst_QSystemNetworkInfo : public QObject
{
    Q_OBJECT
//public:
//    tst_QSystemNetworkInfo();
//    virtual ~tst_QSystemNetworkInfo();

//    friend class QSystemNetworkInfo;

private slots:

    void initTestCase();

    void tst_mode_data();

    void tst_networkStatus_data();
    void tst_networkStatus();


    void tst_networkSignalStrength_data();
    void tst_networkSignalStrength();
    void tst_cellId();
    void tst_locationAreaCode();

    void tst_currentMobileCountryCode();
    void tst_currentMobileNetworkCode();

    void tst_homeMobileCountryCode();
    void tst_homeMobileNetworkCode();

    void tst_networkName_data();
    void tst_networkName();

    void tst_macAddress_data();
    void tst_macAddress();

    void tst_interfaceForMode_data();
    void tst_interfaceForMode();

    void tst_currentMode();

    void tst_networkStatusChanged_data();
    void tst_networkStatusChanged();

    void tst_networkSignalStrengthChanged_data();
    void tst_networkSignalStrengthChanged();

    void tst_currentMobileCountryCodeChanged();
    void tst_currentMobileNetworkCodeChanged();

    void tst_networkNameChanged_data();
    void tst_networkNameChanged();

    void tst_networkModeChanged_data();
    void tst_networkModeChanged();

    void tst_cellIdChanged();

    void networkStatusChanged(QSystemNetworkInfo::NetworkMode, QSystemNetworkInfo::NetworkStatus);
    void networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode, int);
    void currentMobileCountryCodeChanged(const QString &);
    void currentMobileNetworkCodeChanged(const QString &);
    void networkNameChanged(QSystemNetworkInfo::NetworkMode,const QString &);
    void networkModeChanged(QSystemNetworkInfo::NetworkMode);
    void cellIdChanged(int);


private:
    QSystemNetworkInfo::NetworkMode testMode;
};

//tst_QSystemNetworkInfo::tst_QSystemNetworkInfo()
//{
//}

//tst_QSystemNetworkInfo::~tst_QSystemNetworkInfo()
//{

//}

void tst_QSystemNetworkInfo::initTestCase()
{
    qRegisterMetaType<QSystemNetworkInfo::NetworkStatus>("QSystemNetworkInfo::NetworkStatus");
    qRegisterMetaType<QSystemNetworkInfo::NetworkMode>("QSystemNetworkInfo::NetworkMode");
}

void tst_QSystemNetworkInfo::tst_networkStatus_data()
{
    tst_mode_data();
}
void tst_QSystemNetworkInfo::tst_networkStatus()
{
    QFETCH(QSystemNetworkInfo::NetworkMode, mode);
    QSystemNetworkInfo ni;
    QSystemNetworkInfo::NetworkStatus status = ni.networkStatus(mode);

    QVERIFY( status == QSystemNetworkInfo::UndefinedStatus
            || status == QSystemNetworkInfo::NoNetworkAvailable
            || status == QSystemNetworkInfo::EmergencyOnly
            || status == QSystemNetworkInfo::Searching
            || status == QSystemNetworkInfo::Busy
            || status == QSystemNetworkInfo::Connected
            || status == QSystemNetworkInfo::HomeNetwork
            || status == QSystemNetworkInfo::Denied
            || status == QSystemNetworkInfo::Roaming);
}

void tst_QSystemNetworkInfo::tst_mode_data()
{
    QTest::addColumn<QSystemNetworkInfo::NetworkMode>("mode");

    QTest::newRow("GsmMode") << QSystemNetworkInfo::GsmMode;
    QTest::newRow("CdmaMode") << QSystemNetworkInfo::CdmaMode;
    QTest::newRow("WcdmaMode") << QSystemNetworkInfo::WcdmaMode;
    QTest::newRow("WlanMode") << QSystemNetworkInfo::WlanMode;
    QTest::newRow("EthernetMode") << QSystemNetworkInfo::EthernetMode;
    QTest::newRow("BluetoothMode") << QSystemNetworkInfo::BluetoothMode;
    QTest::newRow("WimaxMode") << QSystemNetworkInfo::WimaxMode;

    QTest::newRow("GprsMode") << QSystemNetworkInfo::GprsMode;
    QTest::newRow("EdgeMode") << QSystemNetworkInfo::EdgeMode;
    QTest::newRow("HspaMode") << QSystemNetworkInfo::HspaMode;
    QTest::newRow("LteMode") << QSystemNetworkInfo::LteMode;
}

void tst_QSystemNetworkInfo::tst_networkSignalStrength_data()
{
    tst_mode_data();
}

void tst_QSystemNetworkInfo::tst_networkSignalStrength()
{
    QFETCH(QSystemNetworkInfo::NetworkMode, mode);
    QSystemNetworkInfo ni;
    qint32 strength = ni.networkSignalStrength(mode);
    QVERIFY( strength > -2
            && strength < 101);
}

void  tst_QSystemNetworkInfo::tst_cellId()
{
    QSystemNetworkInfo ni;
    qint32 id = ni.cellId();
    QVERIFY(id > -2);
}

void  tst_QSystemNetworkInfo::tst_locationAreaCode()
{
    QSystemNetworkInfo ni;
    qint32 lac = ni.locationAreaCode();
    QVERIFY(lac > -2);
}


void  tst_QSystemNetworkInfo::tst_currentMobileCountryCode()
{
    QSystemNetworkInfo ni;
    QSystemNetworkInfo::NetworkMode mode = ni.currentMode();
    QSystemNetworkInfo::NetworkStatus status = ni.networkStatus(mode);

    if ((mode == QSystemNetworkInfo::GsmMode
        || mode == QSystemNetworkInfo::CdmaMode
        || mode == QSystemNetworkInfo::GprsMode
        || mode == QSystemNetworkInfo::EdgeMode
        || mode == QSystemNetworkInfo::HspaMode
        || mode == QSystemNetworkInfo::LteMode)
            && (QSystemNetworkInfo::HomeNetwork == status
                || QSystemNetworkInfo::Connected == status
                || QSystemNetworkInfo::Roaming == status)) {
        QVERIFY(!ni.currentMobileCountryCode().isEmpty());
        bool ok;
        int cc = ni.currentMobileCountryCode().toInt(&ok);
        QVERIFY(ok);
        QVERIFY(ni.currentMobileCountryCode().count() == 3);
    } else {
        QVERIFY(ni.currentMobileCountryCode().isEmpty());
    }
}

void  tst_QSystemNetworkInfo::tst_currentMobileNetworkCode()
{
    QSystemNetworkInfo ni;
    QSystemNetworkInfo::NetworkMode mode = ni.currentMode();
    QSystemNetworkInfo::NetworkStatus status = ni.networkStatus(mode);

    if ((mode == QSystemNetworkInfo::GsmMode
        || mode == QSystemNetworkInfo::CdmaMode
        || mode == QSystemNetworkInfo::GprsMode
        || mode == QSystemNetworkInfo::EdgeMode
        || mode == QSystemNetworkInfo::HspaMode
        || mode == QSystemNetworkInfo::LteMode)
            && (QSystemNetworkInfo::HomeNetwork == status
                || QSystemNetworkInfo::Connected == status
                || QSystemNetworkInfo::Roaming == status)) {
        QVERIFY(!ni.currentMobileNetworkCode().isEmpty());
        bool ok;
        int cc = ni.currentMobileCountryCode().toInt(&ok);
        QVERIFY(ok);
        QVERIFY((ni.currentMobileCountryCode().count() == 3)
                || (ni.currentMobileCountryCode().count() == 2));
    } else {
        QVERIFY(ni.currentMobileNetworkCode().isEmpty());
    }
}


void  tst_QSystemNetworkInfo::tst_homeMobileCountryCode()
{
    {
        QSystemNetworkInfo ni;
        QSystemNetworkInfo::NetworkMode mode = ni.currentMode();
        QSystemNetworkInfo::NetworkStatus status = ni.networkStatus(mode);

        if ((mode == QSystemNetworkInfo::GsmMode
            || mode == QSystemNetworkInfo::CdmaMode
            || mode == QSystemNetworkInfo::GprsMode
            || mode == QSystemNetworkInfo::EdgeMode
            || mode == QSystemNetworkInfo::HspaMode
            || mode == QSystemNetworkInfo::LteMode)
                && (QSystemNetworkInfo::HomeNetwork == status
                    || QSystemNetworkInfo::Connected == status
                    || QSystemNetworkInfo::Roaming == status)) {
            QVERIFY(!ni.homeMobileCountryCode().isEmpty());
        } else {
            QVERIFY(ni.homeMobileCountryCode().isEmpty());
        }
    }
}

void  tst_QSystemNetworkInfo::tst_homeMobileNetworkCode()
{
    QSystemNetworkInfo ni;
    QSystemNetworkInfo::NetworkMode mode = ni.currentMode();
    QSystemNetworkInfo::NetworkStatus status = ni.networkStatus(mode);

    if ((mode == QSystemNetworkInfo::GsmMode
        || mode == QSystemNetworkInfo::CdmaMode
        || mode == QSystemNetworkInfo::GprsMode
        || mode == QSystemNetworkInfo::EdgeMode
        || mode == QSystemNetworkInfo::HspaMode
        || mode == QSystemNetworkInfo::LteMode)
            && (QSystemNetworkInfo::HomeNetwork == status
                || QSystemNetworkInfo::Connected == status
                || QSystemNetworkInfo::Roaming == status)) {
        QVERIFY(!ni.homeMobileNetworkCode().isEmpty());
        bool ok;
        int cc = ni.currentMobileCountryCode().toInt(&ok);
        QVERIFY(ok);
        QVERIFY((ni.currentMobileCountryCode().count() == 3)
                || (ni.currentMobileCountryCode().count() == 2));
    } else {
        QVERIFY(ni.homeMobileNetworkCode().isEmpty());
    }
}

void  tst_QSystemNetworkInfo::tst_networkName_data()
{
    tst_mode_data();
}

void  tst_QSystemNetworkInfo::tst_networkName()
{
    {
        QFETCH(QSystemNetworkInfo::NetworkMode, mode);
        QSystemNetworkInfo ni;
        QSystemNetworkInfo::NetworkStatus status = ni.networkStatus(mode);
        if (QSystemNetworkInfo::HomeNetwork == status
                || QSystemNetworkInfo::Connected == status
                || QSystemNetworkInfo::Roaming == status) {
            QVERIFY(!ni.networkName(mode).isEmpty());
        } else {
            ni.networkName(mode).isEmpty();
        }
    }
}

void tst_QSystemNetworkInfo::tst_macAddress_data()
{
   tst_networkSignalStrength_data();
}

void tst_QSystemNetworkInfo::tst_macAddress()
{
   QFETCH(QSystemNetworkInfo::NetworkMode, mode);
   QSystemNetworkInfo ni;
   QString mac = ni.macAddress(mode);
   if (!mac.isEmpty()) {
      QVERIFY(mac.length() == 17);
      QVERIFY(mac.contains(":"));
   }
}

void tst_QSystemNetworkInfo::tst_interfaceForMode_data()
{
    tst_mode_data();
}

void tst_QSystemNetworkInfo::tst_interfaceForMode()
{
    QFETCH(QSystemNetworkInfo::NetworkMode, mode);
    QSystemNetworkInfo ni;
    QSystemNetworkInfo::NetworkStatus status = ni.networkStatus(mode);
    if ((mode == ni.currentMode()) && (mode == QSystemNetworkInfo::WlanMode
            || mode == QSystemNetworkInfo::EthernetMode
            || mode == QSystemNetworkInfo::BluetoothMode)) {
        QVERIFY(!ni.interfaceForMode(mode).name().isEmpty());
    } else {
        QVERIFY(!ni.interfaceForMode(mode).isValid());
    }
}

void tst_QSystemNetworkInfo::tst_currentMode()
{
    QSystemNetworkInfo ni;
    QSystemNetworkInfo::NetworkMode mode = ni.currentMode();

    QVERIFY( mode == QSystemNetworkInfo::UnknownMode
             || mode == QSystemNetworkInfo::GsmMode
             || mode == QSystemNetworkInfo::CdmaMode
             || mode == QSystemNetworkInfo::WcdmaMode
             || mode == QSystemNetworkInfo::WlanMode
             || mode == QSystemNetworkInfo::EthernetMode
             || mode == QSystemNetworkInfo::BluetoothMode
             || mode == QSystemNetworkInfo::WimaxMode

             || mode == QSystemNetworkInfo::GprsMode
             || mode == QSystemNetworkInfo::EdgeMode
             || mode == QSystemNetworkInfo::HspaMode
             || mode == QSystemNetworkInfo::LteMode
             );
}

void tst_QSystemNetworkInfo::tst_networkStatusChanged_data()
{
    tst_mode_data();
}

void tst_QSystemNetworkInfo::tst_networkStatusChanged()
{
    QSystemNetworkInfo ni;
    SystemInfoConnection si;
    QSystemNetworkInfoPrivate *s = si.networkInfoPrivate();
    s->setInitialData();
    QFETCH(QSystemNetworkInfo::NetworkMode, mode);
    s->setNetworkStatus(mode,QSystemNetworkInfo::UndefinedStatus);

    connect(&ni,SIGNAL(networkStatusChanged(QSystemNetworkInfo::NetworkMode,QSystemNetworkInfo::NetworkStatus)),
            this,SLOT(networkStatusChanged(QSystemNetworkInfo::NetworkMode,QSystemNetworkInfo::NetworkStatus)),Qt::UniqueConnection);
    ChangeNetworkThread *changeNetThread = new ChangeNetworkThread();
    changeNetThread->mode = testMode = mode;
    changeNetThread->start();
    QSignalSpy errorSpy(&ni, SIGNAL(networkStatusChanged(QSystemNetworkInfo::NetworkMode, QSystemNetworkInfo::NetworkStatus)));
    QVERIFY(::waitForSignal(&ni, SIGNAL(networkStatusChanged(QSystemNetworkInfo::NetworkMode, QSystemNetworkInfo::NetworkStatus)), /*10 * */1000));
    QVERIFY(errorSpy.count() == 1);
    delete changeNetThread, changeNetThread = 0;
}


void tst_QSystemNetworkInfo::tst_networkSignalStrengthChanged_data()
{
    tst_mode_data();
}

void tst_QSystemNetworkInfo::tst_networkSignalStrengthChanged()
{
    QSystemNetworkInfo ni;
    SystemInfoConnection si;
    QSystemNetworkInfoPrivate *s = si.networkInfoPrivate();
    s->setInitialData();

    QFETCH(QSystemNetworkInfo::NetworkMode, mode);
    s->setNetworkSignalStrength(mode,1);

    connect(&ni,SIGNAL(networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode,int)),
            this,SLOT(networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode,int)),Qt::UniqueConnection);

    ChangeNetworkThread *changeNetThread = new ChangeNetworkThread();
    changeNetThread->mode  = testMode= mode;
    changeNetThread->start();
    QSignalSpy errorSpy(&ni, SIGNAL(networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode, int)));
    QVERIFY(::waitForSignal(&ni, SIGNAL(networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode, int)), 10 * 1000));
    QVERIFY(errorSpy.count() == 1);
    delete changeNetThread, changeNetThread = 0;
}


void tst_QSystemNetworkInfo::tst_currentMobileCountryCodeChanged()
{
    QSystemNetworkInfo  ni;
    SystemInfoConnection si;
    QSystemNetworkInfoPrivate *s = si.networkInfoPrivate();
    s->setCurrentMobileCountryCode("000");

    connect(&ni,SIGNAL(currentMobileCountryCodeChanged(const QString &)),
            this,SLOT(currentMobileCountryCodeChanged(const QString &)));

    ChangeNetworkThread *changeNetThread = new ChangeNetworkThread();
    changeNetThread->mode = testMode = QSystemNetworkInfo::WcdmaMode;
    changeNetThread->start();

    QSignalSpy errorSpy(&ni, SIGNAL(currentMobileCountryCodeChanged(const QString &)));
    QVERIFY(::waitForSignal(&ni, SIGNAL(currentMobileCountryCodeChanged(const QString &)), 10 * 1000));
    QVERIFY(errorSpy.count() == 1);
    delete changeNetThread, changeNetThread = 0;

}


void tst_QSystemNetworkInfo::tst_currentMobileNetworkCodeChanged()
{
    QSystemNetworkInfo ni;
    SystemInfoConnection si;
    QSystemNetworkInfoPrivate *s = si.networkInfoPrivate();

    s->setCurrentMobileNetworkCode("000");

    connect(&ni,SIGNAL(currentMobileNetworkCodeChanged(const QString &)),
            this,SLOT(currentMobileNetworkCodeChanged(const QString &)));

    ChangeNetworkThread *changeNetThread = new ChangeNetworkThread();
    changeNetThread->mode = testMode = QSystemNetworkInfo::WcdmaMode;
    changeNetThread->start();

    QSignalSpy errorSpy(&ni, SIGNAL(currentMobileNetworkCodeChanged(const QString &)));
    QVERIFY(::waitForSignal(&ni, SIGNAL(currentMobileNetworkCodeChanged(const QString &)), 10 * 1000));
    QVERIFY(errorSpy.count() == 1);
    delete changeNetThread, changeNetThread = 0;

}

void  tst_QSystemNetworkInfo::tst_networkNameChanged_data()
{
    tst_mode_data();
}

void tst_QSystemNetworkInfo::tst_networkNameChanged()
{
    QSystemNetworkInfo ni;
    SystemInfoConnection si;
    QSystemNetworkInfoPrivate *s = si.networkInfoPrivate();

    QFETCH(QSystemNetworkInfo::NetworkMode, mode);
    s->setNetworkName(mode,"unknown");

    connect(&ni,SIGNAL(networkNameChanged(QSystemNetworkInfo::NetworkMode,const QString &)),
            this,SLOT(networkNameChanged(QSystemNetworkInfo::NetworkMode,const QString &)));

    ChangeNetworkThread *changeNetThread = new ChangeNetworkThread();
    changeNetThread->mode  = testMode = mode;
    changeNetThread->start();
    QSignalSpy errorSpy(&ni, SIGNAL(networkNameChanged(QSystemNetworkInfo::NetworkMode, const QString &)));
    QVERIFY(::waitForSignal(&ni, SIGNAL(networkNameChanged(QSystemNetworkInfo::NetworkMode, const QString &)), 10 * 1000));
    QVERIFY(errorSpy.count() == 1);
    delete changeNetThread, changeNetThread = 0;

}


void  tst_QSystemNetworkInfo::tst_networkModeChanged_data()
{
    tst_mode_data();
}

void tst_QSystemNetworkInfo::tst_networkModeChanged()
{
    QSystemNetworkInfo ni;
    SystemInfoConnection si;
    QSystemNetworkInfoPrivate *s = si.networkInfoPrivate();

    s->setInitialData();
    QFETCH(QSystemNetworkInfo::NetworkMode, mode);
    s->setCurrentMode(QSystemNetworkInfo::UnknownMode);

    connect(&ni,SIGNAL(networkModeChanged(QSystemNetworkInfo::NetworkMode)),
            this,SLOT(networkModeChanged(QSystemNetworkInfo::NetworkMode)));

    ChangeNetworkThread *changeNetThread = new ChangeNetworkThread();
    changeNetThread->mode = testMode = mode;
    changeNetThread->start();
    QSignalSpy errorSpy(&ni, SIGNAL(networkModeChanged(QSystemNetworkInfo::NetworkMode)));
    QVERIFY(::waitForSignal(&ni, SIGNAL(networkModeChanged(QSystemNetworkInfo::NetworkMode)), 10 * 1000));
    QVERIFY(errorSpy.count() == 1);

    delete changeNetThread, changeNetThread = 0;
}

void tst_QSystemNetworkInfo::tst_cellIdChanged()
{
    QSystemNetworkInfo ni;
    SystemInfoConnection si;
    QSystemNetworkInfoPrivate *s = si.networkInfoPrivate();
    s->setInitialData();
    connect(&ni,SIGNAL(cellIdChanged(int)),
            this,SLOT(cellIdChanged(int)));

    ChangeNetworkThread *changeNetThread = new ChangeNetworkThread();
    changeNetThread->mode = testMode = QSystemNetworkInfo::WcdmaMode;
    changeNetThread->start();
    QSignalSpy errorSpy(&ni, SIGNAL(cellIdChanged(int)));
    QVERIFY(::waitForSignal(&ni, SIGNAL(cellIdChanged(int)), 10 * 1000));
    QVERIFY(errorSpy.count() == 1);
    delete changeNetThread, changeNetThread = 0;
}



void tst_QSystemNetworkInfo::networkStatusChanged(QSystemNetworkInfo::NetworkMode mode, QSystemNetworkInfo::NetworkStatus status)
{
    QVERIFY(mode == testMode);
    QVERIFY(status == QSystemNetworkInfo::Denied);
}

void tst_QSystemNetworkInfo::networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode mode, int strength)
{
    QVERIFY(mode == testMode);
    QVERIFY(strength == 74);
}

void tst_QSystemNetworkInfo::currentMobileCountryCodeChanged(const QString &str)
{
    QVERIFY(str == "111");
}

void tst_QSystemNetworkInfo::currentMobileNetworkCodeChanged(const QString &str)
{
    QVERIFY(str == "123");
}

void tst_QSystemNetworkInfo::networkNameChanged(QSystemNetworkInfo::NetworkMode mode,const QString &name)
{
    QVERIFY(mode == testMode);
    QVERIFY(name == "qsysteminfo test network");
}

void tst_QSystemNetworkInfo::networkModeChanged(QSystemNetworkInfo::NetworkMode mode)
{
    QVERIFY(mode == testMode);
}

void tst_QSystemNetworkInfo::cellIdChanged(int id)
{
    QVERIFY(id == 206555390);
}


QTEST_MAIN(tst_QSystemNetworkInfo)
#include "tst_qsystemnetworkinfo.moc"
