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

//TESTED_COMPONENT=src/location

#include <QTest>
#include <QMetaType>
#include <QSignalSpy>
#include <QDebug>
#include <QTimer>

#include <limits.h>

#include <qnumeric.h>
#include <qgeopositioninfosource.h>
#include <qgeopositioninfo.h>

#include "testqgeopositioninfosource_p.h"
#include "../qgeocoordinate/qlocationtestutils_p.h"

#ifdef TST_GEOCLUEMOCK_ENABLED
#include <geoclue/geoclue-master.h>
#include <geoclue/geoclue-master-client.h>
#include <geoclue/geoclue-velocity.h>
#include <QDateTime>
#endif

Q_DECLARE_METATYPE(QGeoPositionInfoSource::PositioningMethod)
Q_DECLARE_METATYPE(QGeoPositionInfoSource::PositioningMethods)
Q_DECLARE_METATYPE(QGeoPositionInfo)

#define MAX_WAITING_TIME 50000

// Must provide a valid source, unless testing the source
// returned by QGeoPositionInfoSource::createDefaultSource() on a system
// that has no default source
#define CHECK_SOURCE_VALID { \
        if (!m_source) { \
            if (m_testingDefaultSource && QGeoPositionInfoSource::createDefaultSource(0) == 0) \
                QSKIP("No default position source on this system", SkipAll); \
            else \
                QFAIL("createTestSource() must return a valid source!"); \
        } \
    }

class MyPositionSource : public QGeoPositionInfoSource
{
    Q_OBJECT
public:
    MyPositionSource(QObject *parent = 0)
            : QGeoPositionInfoSource(parent) {
    }

    QGeoPositionInfo lastKnownPosition(bool /*fromSatellitePositioningMethodsOnly = false*/) const {
        return QGeoPositionInfo();
    }

    void setSupportedPositioningMethods(PositioningMethods methods) {
        m_methods = methods;
    }

    virtual PositioningMethods supportedPositioningMethods() const {
        return m_methods;
    }
    virtual int minimumUpdateInterval() const {
        return 0;
    }

    virtual void startUpdates() {}
    virtual void stopUpdates() {}

    virtual void requestUpdate(int) {}

private:
    PositioningMethods m_methods;
};

class DefaultSourceTest : public TestQGeoPositionInfoSource
{
    Q_OBJECT
protected:
    QGeoPositionInfoSource *createTestSource() {
        return QGeoPositionInfoSource::createDefaultSource(0);
    }
};


TestQGeoPositionInfoSource::TestQGeoPositionInfoSource(QObject *parent)
        : QObject(parent)
{
#ifdef TST_GEOCLUEMOCK_ENABLED
    m_geoclueMock = 0;
#endif
    m_testingDefaultSource = false;
}

TestQGeoPositionInfoSource *TestQGeoPositionInfoSource::createDefaultSourceTest()
{
    DefaultSourceTest *test = new DefaultSourceTest;
    test->m_testingDefaultSource = true;
    return test;
}

void TestQGeoPositionInfoSource::test_slot1()
{
}

void TestQGeoPositionInfoSource::test_slot2()
{
    m_testSlot2Called = true;
}

void TestQGeoPositionInfoSource::base_initTestCase()
{
    qRegisterMetaType<QGeoPositionInfo>();
}

void TestQGeoPositionInfoSource::base_init()
{
    m_source = createTestSource();
    m_testSlot2Called = false;
}

void TestQGeoPositionInfoSource::base_cleanup()
{
    delete m_source;
    m_source = 0;
}

void TestQGeoPositionInfoSource::base_cleanupTestCase()
{
}

void TestQGeoPositionInfoSource::initTestCase()
{
    base_initTestCase();
#ifdef Q_WS_MEEGO
    qDebug ("Note: if running on MeeGo with real libraries, make sure GPS is configured.");
    qDebug ("Use gconftool-2 to set it, e.g.: ");
    qDebug ("gconftool-2 -t string -s /apps/geoclue/master/org.freedesktop.Geoclue.GPSDevice /dev/ttyUSB0");
#endif
#ifdef TST_GEOCLUEMOCK_ENABLED
    m_threadGeoclueMock.start();
#endif
}

void TestQGeoPositionInfoSource::init()
{
#ifdef TST_GEOCLUEMOCK_ENABLED
    initGeoclueMock();
#endif
    base_init();
}

void TestQGeoPositionInfoSource::cleanup()
{
#ifdef TST_GEOCLUEMOCK_ENABLED
    if (m_geoclueMock)
        delete m_geoclueMock;
    m_geoclueMock =0;
#endif
    base_cleanup();
}

void TestQGeoPositionInfoSource::cleanupTestCase()
{
#ifdef TST_GEOCLUEMOCK_ENABLED
    m_threadGeoclueMock.quit();
    m_threadGeoclueMock.wait();
#endif
    base_cleanupTestCase();
}

#ifdef TST_GEOCLUEMOCK_ENABLED
void TestQGeoPositionInfoSource::initGeoclueMock()
{
    if (m_geoclueMock)
        delete m_geoclueMock;
    m_geoclueMock = new GeoclueMock;
    geocluemock_setjournal(":/data/gcmock_basic_vel_pos.journal");
    m_geoclueMock->moveToThread(&m_threadGeoclueMock);
    QMetaObject::invokeMethod(m_geoclueMock, "start", Qt::BlockingQueuedConnection);
}
#endif

// TC_ID_3_x_1
void TestQGeoPositionInfoSource::constructor_withParent()
{
    //QLocationTestUtils::uheap_mark();
    QObject *parent = new QObject();
    new MyPositionSource(parent);
    delete parent;
    //QLocationTestUtils::uheap_mark_end();
}

// TC_ID_3_x_2
void TestQGeoPositionInfoSource::constructor_noParent()
{
    //QLocationTestUtils::uheap_mark();
    MyPositionSource *obj = new MyPositionSource();
    delete obj;
    //QLocationTestUtils::uheap_mark_end();
}

void TestQGeoPositionInfoSource::updateInterval()
{
    MyPositionSource s;
    QCOMPARE(s.updateInterval(), 0);
}

void TestQGeoPositionInfoSource::setPreferredPositioningMethods()
{
    QFETCH(int, supported);
    QFETCH(int, preferred);
    QFETCH(int, resulting);

    MyPositionSource s;
    s.setSupportedPositioningMethods(
        static_cast<QGeoPositionInfoSource::PositioningMethods>(supported));
    s.setPreferredPositioningMethods(
        static_cast<QGeoPositionInfoSource::PositioningMethods>(preferred));
    QCOMPARE(s.preferredPositioningMethods(),
             static_cast<QGeoPositionInfoSource::PositioningMethods>(resulting));
}

void TestQGeoPositionInfoSource::setPreferredPositioningMethods_data()
{
    QTest::addColumn<int>("supported");
    QTest::addColumn<int>("preferred");
    QTest::addColumn<int>("resulting");

    QTest::newRow("Sat supported, Sat preferred")
    << int(QGeoPositionInfoSource::SatellitePositioningMethods)
    << int(QGeoPositionInfoSource::SatellitePositioningMethods)
    << int(QGeoPositionInfoSource::SatellitePositioningMethods);
    QTest::newRow("Sat supported, Non-Sat preferred")
    << int(QGeoPositionInfoSource::SatellitePositioningMethods)
    << int(QGeoPositionInfoSource::NonSatellitePositioningMethods)
    << int(QGeoPositionInfoSource::SatellitePositioningMethods);
    QTest::newRow("Sat supported, All preferred")
    << int(QGeoPositionInfoSource::SatellitePositioningMethods)
    << int(QGeoPositionInfoSource::AllPositioningMethods)
    << int(QGeoPositionInfoSource::SatellitePositioningMethods);

    QTest::newRow("Non-Sat supported, Sat preferred")
    << int(QGeoPositionInfoSource::NonSatellitePositioningMethods)
    << int(QGeoPositionInfoSource::SatellitePositioningMethods)
    << int(QGeoPositionInfoSource::NonSatellitePositioningMethods);
    QTest::newRow("Non-Sat supported, Non-Sat preferred")
    << int(QGeoPositionInfoSource::NonSatellitePositioningMethods)
    << int(QGeoPositionInfoSource::NonSatellitePositioningMethods)
    << int(QGeoPositionInfoSource::NonSatellitePositioningMethods);
    QTest::newRow("Non-Sat supported, All preferred")
    << int(QGeoPositionInfoSource::NonSatellitePositioningMethods)
    << int(QGeoPositionInfoSource::AllPositioningMethods)
    << int(QGeoPositionInfoSource::NonSatellitePositioningMethods);

    QTest::newRow("All supported, Sat preferred")
    << int(QGeoPositionInfoSource::AllPositioningMethods)
    << int(QGeoPositionInfoSource::SatellitePositioningMethods)
    << int(QGeoPositionInfoSource::SatellitePositioningMethods);
    QTest::newRow("All supported, Non-Sat preferred")
    << int(QGeoPositionInfoSource::AllPositioningMethods)
    << int(QGeoPositionInfoSource::NonSatellitePositioningMethods)
    << int(QGeoPositionInfoSource::NonSatellitePositioningMethods);
    QTest::newRow("All supported, All preferred")
    << int(QGeoPositionInfoSource::AllPositioningMethods)
    << int(QGeoPositionInfoSource::AllPositioningMethods)
    << int(QGeoPositionInfoSource::AllPositioningMethods);
}

void TestQGeoPositionInfoSource::preferredPositioningMethods()
{
    MyPositionSource s;
    QCOMPARE(s.preferredPositioningMethods(), 0);
}

//TC_ID_3_x_1 : Create a position source with the given parent that reads from the system's default
// sources of location data
void TestQGeoPositionInfoSource::createDefaultSource()
{
    //QLocationTestUtils::uheap_mark();
    QObject *parent = new QObject;

    QGeoPositionInfoSource *source = QGeoPositionInfoSource::createDefaultSource(parent);
    // now all platforms have the dummy plugin at least
    QVERIFY(source != 0);
    delete parent;
    //QLocationTestUtils::uheap_mark_end();
}

void TestQGeoPositionInfoSource::setUpdateInterval()
{
    CHECK_SOURCE_VALID;

    QFETCH(int, interval);
    QFETCH(int, expectedInterval);

    m_source->setUpdateInterval(interval);
    QCOMPARE(m_source->updateInterval(), expectedInterval);
}

void TestQGeoPositionInfoSource::setUpdateInterval_data()
{
    QTest::addColumn<int>("interval");
    QTest::addColumn<int>("expectedInterval");
#ifdef TST_GEOCLUEMOCK_ENABLED
    initGeoclueMock();
#endif
    QGeoPositionInfoSource *source = createTestSource();
    int minUpdateInterval = source ? source->minimumUpdateInterval() : -1;
    if (source)
        delete source;

    QTest::newRow("0") << 0 << 0;
    
    if (minUpdateInterval > -1) {
        QTest::newRow("INT_MIN") << INT_MIN << minUpdateInterval;
        QTest::newRow("-1") << -1 << minUpdateInterval;
    }

    if (minUpdateInterval > 0) {
        QTest::newRow("more than minInterval") << minUpdateInterval + 1 << minUpdateInterval + 1;
        QTest::newRow("equal to minInterval") << minUpdateInterval << minUpdateInterval;
    }
    
    if (minUpdateInterval > 1) {
        QTest::newRow("less then minInterval") << minUpdateInterval - 1 << minUpdateInterval;
        QTest::newRow("in btw zero and minInterval") << 1 << minUpdateInterval;
    }

    // Fails on S60, should investigate
    //QTest::newRow("INT_MAX") << INT_MAX << INT_MAX;
}

void TestQGeoPositionInfoSource::lastKnownPosition()
{
    CHECK_SOURCE_VALID;
    QFETCH(int, positioningMethod);
    QFETCH(bool, lastKnownPositionArgument);
    QFETCH(bool, positionValid);

    QGeoPositionInfoSource::PositioningMethods method
            = static_cast<QGeoPositionInfoSource::PositioningMethods>(positioningMethod);

    if ((m_source->supportedPositioningMethods() & method) == 0)
        QSKIP("Not a supported positioning method for this position source", SkipSingle);

    m_source->setPreferredPositioningMethods(method);

    QSignalSpy spy(m_source, SIGNAL(positionUpdated(const QGeoPositionInfo&)));
    QSignalSpy timeout(m_source, SIGNAL(updateTimeout()));
    int time_out = 7000;
    m_source->setUpdateInterval(time_out);
    m_source->startUpdates();

    // Use QEventLoop instead of qWait() to ensure we stop as soon as a
    // position is emitted (otherwise the lastKnownPosition() may have
    // changed by the time it is checked)
    QEventLoop loop;
    QTimer timer;
    timer.setInterval(9500);
    connect(m_source, SIGNAL(positionUpdated(const QGeoPositionInfo&)),
            &loop, SLOT(quit()));
    connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
    timer.start();
    loop.exec();

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337_ABORT;

    QVERIFY((spy.count() > 0) && (timeout.count() == 0));

    QList<QVariant> list = spy.takeFirst();
    QGeoPositionInfo info;
    info = list.at(0).value<QGeoPositionInfo>();
    QGeoPositionInfo lastPositioninfo;
    lastPositioninfo = m_source->lastKnownPosition(lastKnownPositionArgument);
 
    QCOMPARE(lastPositioninfo.isValid(), positionValid);

    if (positionValid) {
        QCOMPARE(info.coordinate(), lastPositioninfo.coordinate());
        QCOMPARE(info.timestamp(), lastPositioninfo.timestamp());

        QCOMPARE(info.hasAttribute(QGeoPositionInfo::HorizontalAccuracy),
                 lastPositioninfo.hasAttribute(QGeoPositionInfo::HorizontalAccuracy));

        if (info.hasAttribute(QGeoPositionInfo::HorizontalAccuracy)) {
            bool isNaN1 =  qIsNaN(info.attribute(QGeoPositionInfo::HorizontalAccuracy));
            bool isNaN2 =  qIsNaN(lastPositioninfo.attribute(QGeoPositionInfo::HorizontalAccuracy));
            QCOMPARE(isNaN1, isNaN2);
            if (!isNaN1) {
                QCOMPARE(qFuzzyCompare(info.attribute(QGeoPositionInfo::HorizontalAccuracy),
                                       lastPositioninfo.attribute(QGeoPositionInfo::HorizontalAccuracy)), TRUE);
            }
        }

        QCOMPARE(info.hasAttribute(QGeoPositionInfo::VerticalAccuracy),
                 lastPositioninfo.hasAttribute(QGeoPositionInfo::VerticalAccuracy));

        if (info.hasAttribute(QGeoPositionInfo::VerticalAccuracy)) {
            bool isNaN1 =  qIsNaN(info.attribute(QGeoPositionInfo::VerticalAccuracy));
            bool isNaN2 =  qIsNaN(lastPositioninfo.attribute(QGeoPositionInfo::VerticalAccuracy));
            QCOMPARE(isNaN1, isNaN2);
            if (!isNaN1) {
                QCOMPARE(qFuzzyCompare(info.attribute(QGeoPositionInfo::VerticalAccuracy),
                                       lastPositioninfo.attribute(QGeoPositionInfo::VerticalAccuracy)), TRUE);
            }
        }
    }

    m_source->stopUpdates();
}

void TestQGeoPositionInfoSource::lastKnownPosition_data()
{
    QTest::addColumn<int>("positioningMethod");
    QTest::addColumn<bool>("lastKnownPositionArgument");
    QTest::addColumn<bool>("positionValid");

#ifndef Q_WS_MEEGO
    // no good way to determine on MeeGo what are supported. If we ask for all or non-satellites, we
    // typically get geoclue-example provider, which is not suitable for this test.
    QTest::newRow("all - false") << int(QGeoPositionInfoSource::AllPositioningMethods) << false << true;
    QTest::newRow("all - true") << int(QGeoPositionInfoSource::AllPositioningMethods) << true << true;
#endif
    QTest::newRow("satellite - false") << int(QGeoPositionInfoSource::SatellitePositioningMethods) << false << true;
    QTest::newRow("satellite - true") << int(QGeoPositionInfoSource::SatellitePositioningMethods) << true << true;
}

void TestQGeoPositionInfoSource::minimumUpdateInterval()
{
    CHECK_SOURCE_VALID;

    QVERIFY(m_source->minimumUpdateInterval() > 0);
}

//TC_ID_3_x_1
void TestQGeoPositionInfoSource::startUpdates_testIntervals()
{
    CHECK_SOURCE_VALID;

    QSignalSpy spy(m_source, SIGNAL(positionUpdated(const QGeoPositionInfo&)));
    QSignalSpy timeout(m_source, SIGNAL(updateTimeout()));
    m_source->setUpdateInterval(7000);
    int interval = m_source->updateInterval();

    m_source->startUpdates();

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337;

    QTRY_COMPARE_WITH_TIMEOUT(spy.count(), 1, 9500);
    for (int i = 0; i < 6; i++) {
        EXPECT_FAIL_WINCE_SEE_MOBILITY_337;

        QTRY_VERIFY_WITH_TIMEOUT((spy.count() == 1) && (timeout.count() == 0), (interval*2));
        spy.clear();
    }

    m_source->stopUpdates();
}


void TestQGeoPositionInfoSource::startUpdates_testIntervalChangesWhileRunning()
{
    // There are two ways of dealing with an interval change, and we have left it system dependent.
    // The interval can be changed will running or after the next update.
    // WinCE uses the first method, S60 uses the second method.

    // The minimum interval on the symbian emulator is 5000 msecs, which is why the times in
    // this test are as high as they are.

    CHECK_SOURCE_VALID;

    QSignalSpy spy(m_source, SIGNAL(positionUpdated(const QGeoPositionInfo&)));
    QSignalSpy timeout(m_source, SIGNAL(updateTimeout()));
    m_source->setUpdateInterval(0);
    m_source->startUpdates();
    m_source->setUpdateInterval(0);

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337;

    QTRY_VERIFY_WITH_TIMEOUT(spy.count() > 0, 7000);
    QCOMPARE(timeout.count(), 0);
    spy.clear();

    m_source->setUpdateInterval(5000);

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337;
    QTRY_VERIFY_WITH_TIMEOUT((spy.count() == 2) && (timeout.count() == 0), 15000);
    spy.clear();

    m_source->setUpdateInterval(10000);

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337;
    QTRY_VERIFY_WITH_TIMEOUT((spy.count() == 2) && (timeout.count() == 0), 30000);
    spy.clear();

    m_source->setUpdateInterval(5000);

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337;
    QTRY_VERIFY_WITH_TIMEOUT((spy.count() == 2) && (timeout.count() == 0), 15000);
    spy.clear();

    m_source->setUpdateInterval(5000);

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337;
    QTRY_VERIFY_WITH_TIMEOUT((spy.count() == 2) && (timeout.count() == 0), 15000);
    spy.clear();

    m_source->setUpdateInterval(0);

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337;
    QTRY_VERIFY_WITH_TIMEOUT((spy.count() == 1) && (timeout.count() == 0), 7000);
    spy.clear();

    m_source->setUpdateInterval(0);

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337;
    QTRY_VERIFY_WITH_TIMEOUT((spy.count() == 1) && (timeout.count() == 0), 7000);
    spy.clear();

    m_source->stopUpdates();
}

//TC_ID_3_x_2
void TestQGeoPositionInfoSource::startUpdates_testDefaultInterval()
{
    CHECK_SOURCE_VALID;

    QSignalSpy spy(m_source, SIGNAL(positionUpdated(const QGeoPositionInfo&)));
    QSignalSpy timeout(m_source, SIGNAL(updateTimeout()));
    m_source->startUpdates();
    for (int i = 0; i < 3; i++) {
        EXPECT_FAIL_WINCE_SEE_MOBILITY_337;

        QTRY_VERIFY_WITH_TIMEOUT((spy.count() > 0) && (timeout.count() == 0), 7000);
        spy.clear();
    }
    m_source->stopUpdates();
}

//TC_ID_3_x_3
void TestQGeoPositionInfoSource::startUpdates_testZeroInterval()
{
    CHECK_SOURCE_VALID;

    QSignalSpy spy(m_source, SIGNAL(positionUpdated(const QGeoPositionInfo&)));
    QSignalSpy timeout(m_source, SIGNAL(updateTimeout()));
    m_source->setUpdateInterval(0);
    m_source->startUpdates();
    for (int i = 0; i < 3; i++) {
        EXPECT_FAIL_WINCE_SEE_MOBILITY_337;

        QTRY_VERIFY_WITH_TIMEOUT((spy.count() > 0) && (timeout.count() == 0), 7000);
        spy.clear();
    }
    m_source->stopUpdates();
}

void TestQGeoPositionInfoSource::startUpdates_moreThanOnce()
{
    CHECK_SOURCE_VALID;

    QSignalSpy spy(m_source, SIGNAL(positionUpdated(const QGeoPositionInfo&)));
    QSignalSpy timeout(m_source, SIGNAL(updateTimeout()));
    m_source->setUpdateInterval(0);
    m_source->startUpdates();

    m_source->startUpdates(); // check there is no crash

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337;

    QTRY_VERIFY_WITH_TIMEOUT((spy.count() > 0) && (timeout.count() == 0), 7000);

    m_source->startUpdates(); // check there is no crash

    m_source->stopUpdates();
}

//TC_ID_3_x_1
void TestQGeoPositionInfoSource::stopUpdates()
{
    CHECK_SOURCE_VALID;

    QSignalSpy spy(m_source, SIGNAL(positionUpdated(const QGeoPositionInfo&)));
    QSignalSpy timeout(m_source, SIGNAL(updateTimeout()));
    m_source->setUpdateInterval(7000);
    m_source->startUpdates();
    for (int i = 0; i < 2; i++) {
        EXPECT_FAIL_WINCE_SEE_MOBILITY_337;

        QTRY_VERIFY_WITH_TIMEOUT((spy.count() > 0) && (timeout.count() == 0), 9500);
        spy.clear();
    }
    m_source->stopUpdates();
    QTest::qWait(9500);
    QCOMPARE(spy.count(), 0);
    spy.clear();

    m_source->setUpdateInterval(0);
    m_source->startUpdates();
    m_source->stopUpdates();
    QTRY_COMPARE_WITH_TIMEOUT(spy.count(), 0, 9500);
}

//TC_ID_3_x_2
void TestQGeoPositionInfoSource::stopUpdates_withoutStart()
{
    CHECK_SOURCE_VALID;
    m_source->stopUpdates(); // check there is no crash
}

void TestQGeoPositionInfoSource::requestUpdate()
{
    CHECK_SOURCE_VALID;
    QFETCH(int, timeout);
    QSignalSpy spy(m_source, SIGNAL(updateTimeout()));
    m_source->requestUpdate(timeout);
    QTRY_COMPARE(spy.count(), 1);
}

void TestQGeoPositionInfoSource::requestUpdate_data()
{
    QTest::addColumn<int>("timeout");
    QTest::newRow("less than zero") << -1;          //TC_ID_3_x_7
}

// TC_ID_3_x_1 : Create position source and call requestUpdate with valid timeout value
void TestQGeoPositionInfoSource::requestUpdate_validTimeout()
{
    CHECK_SOURCE_VALID;

    QSignalSpy spyUpdate(m_source, SIGNAL(positionUpdated(const QGeoPositionInfo&)));
    QSignalSpy spyTimeout(m_source, SIGNAL(updateTimeout()));

    m_source->requestUpdate(7000);

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337;

    QTRY_VERIFY_WITH_TIMEOUT((spyUpdate.count() > 0) && (spyTimeout.count() == 0), 7000);
}

void TestQGeoPositionInfoSource::requestUpdate_defaultTimeout()
{
    CHECK_SOURCE_VALID;

    QSignalSpy spyUpdate(m_source, SIGNAL(positionUpdated(const QGeoPositionInfo&)));
    QSignalSpy spyTimeout(m_source, SIGNAL(updateTimeout()));

    m_source->requestUpdate(0);

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337;

    // S60 emulator fail
    QTRY_VERIFY_WITH_TIMEOUT((spyUpdate.count() > 0) && (spyTimeout.count() == 0), 7000);
}

// TC_ID_3_x_2 : Create position source and call requestUpdate with a timeout less than
// minimumupdateInterval
void TestQGeoPositionInfoSource::requestUpdate_timeoutLessThanMinimumInterval()
{
    CHECK_SOURCE_VALID;

    QSignalSpy spyTimeout(m_source, SIGNAL(updateTimeout()));
    m_source->requestUpdate(1);

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337;

    QTRY_COMPARE_WITH_TIMEOUT(spyTimeout.count(), 1, 1000);
}

// TC_ID_3_x_3 : Call requestUpdate() with same value repeatedly
void TestQGeoPositionInfoSource::requestUpdate_repeatedCalls()
{
    CHECK_SOURCE_VALID;

    QSignalSpy spyUpdate(m_source, SIGNAL(positionUpdated(const QGeoPositionInfo&)));
    QSignalSpy spyTimeout(m_source, SIGNAL(updateTimeout()));

    m_source->requestUpdate(7000);

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337;

    QTRY_VERIFY_WITH_TIMEOUT((spyUpdate.count() > 0) && (spyTimeout.count() == 0), 7000);
    spyUpdate.clear();
    m_source->requestUpdate(7000);

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337;

    QTRY_VERIFY_WITH_TIMEOUT((spyUpdate.count() > 0) && (spyTimeout.count() == 0), 7000);
}

void TestQGeoPositionInfoSource::requestUpdate_overlappingCalls()
{
    CHECK_SOURCE_VALID;

    QSignalSpy spyUpdate(m_source, SIGNAL(positionUpdated(const QGeoPositionInfo&)));
    QSignalSpy spyTimeout(m_source, SIGNAL(updateTimeout()));

    m_source->requestUpdate(7000);
    m_source->requestUpdate(7000);

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337;

    QTRY_VERIFY_WITH_TIMEOUT((spyUpdate.count() > 0) && (spyTimeout.count() == 0), 7000);
}

//TC_ID_3_x_4
void TestQGeoPositionInfoSource::requestUpdateAfterStartUpdates_ZeroInterval()
{
    CHECK_SOURCE_VALID;

    QSignalSpy spyUpdate(m_source, SIGNAL(positionUpdated(const QGeoPositionInfo&)));
    QSignalSpy spyTimeout(m_source, SIGNAL(updateTimeout()));

    m_source->setUpdateInterval(0);
    m_source->startUpdates();

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337;

    QTRY_VERIFY_WITH_TIMEOUT((spyUpdate.count() > 0) && (spyTimeout.count() == 0), 7000);
    spyUpdate.clear();

    m_source->requestUpdate(7000);
    QTest::qWait(7000);

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337;

    QVERIFY((spyUpdate.count() > 0) && (spyTimeout.count() == 0));
    spyUpdate.clear();

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337;

    QTRY_VERIFY_WITH_TIMEOUT((spyUpdate.count() > 0) && (spyTimeout.count() == 0), MAX_WAITING_TIME);

    m_source->stopUpdates();
}

void TestQGeoPositionInfoSource::requestUpdateAfterStartUpdates_SmallInterval()
{
    CHECK_SOURCE_VALID;

    QSignalSpy spyUpdate(m_source, SIGNAL(positionUpdated(const QGeoPositionInfo&)));
    QSignalSpy spyTimeout(m_source, SIGNAL(updateTimeout()));

    m_source->setUpdateInterval(10000);
    m_source->startUpdates();

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337;

    QTRY_VERIFY_WITH_TIMEOUT((spyUpdate.count() == 1) && (spyTimeout.count() == 0), 20000);
    spyUpdate.clear();

    m_source->requestUpdate(7000);

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337;

    QTRY_VERIFY_WITH_TIMEOUT((spyUpdate.count() == 1) && (spyTimeout.count() == 0), 7000);
    spyUpdate.clear();

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337;

    QTRY_VERIFY_WITH_TIMEOUT((spyUpdate.count() == 1) && (spyTimeout.count() == 0), 20000);

    m_source->stopUpdates();
}

void TestQGeoPositionInfoSource::requestUpdateBeforeStartUpdates_ZeroInterval()
{
    CHECK_SOURCE_VALID;

    QSignalSpy spyUpdate(m_source, SIGNAL(positionUpdated(const QGeoPositionInfo&)));
    QSignalSpy spyTimeout(m_source, SIGNAL(updateTimeout()));

    m_source->requestUpdate(7000);

    m_source->setUpdateInterval(0);
    m_source->startUpdates();

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337;

    QTRY_VERIFY_WITH_TIMEOUT((spyUpdate.count() >= 2) && (spyTimeout.count() == 0), 14000);
    spyUpdate.clear();

    QTest::qWait(7000);

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337;

    QCOMPARE(spyTimeout.count(), 0);

    m_source->stopUpdates();
}

void TestQGeoPositionInfoSource::requestUpdateBeforeStartUpdates_SmallInterval()
{
    CHECK_SOURCE_VALID;

    QSignalSpy spyUpdate(m_source, SIGNAL(positionUpdated(const QGeoPositionInfo&)));
    QSignalSpy spyTimeout(m_source, SIGNAL(updateTimeout()));

    m_source->requestUpdate(7000);

    m_source->setUpdateInterval(10000);
    m_source->startUpdates();

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337;

    QTRY_VERIFY_WITH_TIMEOUT((spyUpdate.count() > 0) && (spyTimeout.count() == 0), 7000);
    spyUpdate.clear();

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337;

    QTRY_VERIFY_WITH_TIMEOUT((spyUpdate.count() > 0) && (spyTimeout.count() == 0), 20000);

    m_source->stopUpdates();
}

void TestQGeoPositionInfoSource::removeSlotForRequestTimeout()
{
    CHECK_SOURCE_VALID;

    bool i = connect(m_source, SIGNAL(updateTimeout()), this, SLOT(test_slot1()));
    QVERIFY(i == true);
    i = connect(m_source, SIGNAL(updateTimeout()), this, SLOT(test_slot2()));
    QVERIFY(i == true);
    i = disconnect(m_source, SIGNAL(updateTimeout()), this, SLOT(test_slot1()));
    QVERIFY(i == true);

    m_source->requestUpdate(-1);
    QTRY_VERIFY_WITH_TIMEOUT((m_testSlot2Called == true), 1000);
}

void TestQGeoPositionInfoSource::removeSlotForPositionUpdated()
{
    CHECK_SOURCE_VALID;

    bool i = connect(m_source, SIGNAL(positionUpdated(const QGeoPositionInfo &)), this, SLOT(test_slot1()));
    QVERIFY(i == true);
    i = connect(m_source, SIGNAL(positionUpdated(const QGeoPositionInfo &)), this, SLOT(test_slot2()));
    QVERIFY(i == true);
    i = disconnect(m_source, SIGNAL(positionUpdated(const QGeoPositionInfo &)), this, SLOT(test_slot1()));
    QVERIFY(i == true);

    m_source->requestUpdate(7000);

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337;

    QTRY_VERIFY_WITH_TIMEOUT((m_testSlot2Called == true), 7000);
}

#ifdef TST_GEOCLUEMOCK_ENABLED
// Testcase to check that values are passed correctly
void TestQGeoPositionInfoSource::updateValues()
{
    CHECK_SOURCE_VALID;

    QSignalSpy spy(m_source, SIGNAL(positionUpdated(const QGeoPositionInfo&)));
    m_source->startUpdates();
    QTRY_VERIFY_WITH_TIMEOUT(!spy.isEmpty(), 2000);
    QList<QVariant> list = spy.takeFirst();
    QGeoPositionInfo info;
    info = list.at(0).value<QGeoPositionInfo>();
    QCOMPARE(qFuzzyCompare(info.coordinate().latitude(), 21), TRUE );
    QCOMPARE(qFuzzyCompare(info.coordinate().longitude(), 31), TRUE );
    QCOMPARE(qFuzzyCompare(info.coordinate().altitude(), 5.1), TRUE );
    QCOMPARE(qFuzzyCompare(info.attribute(QGeoPositionInfo::HorizontalAccuracy), 8), TRUE );
    QCOMPARE(qFuzzyCompare(info.attribute(QGeoPositionInfo::VerticalAccuracy), 9), TRUE );
    QDateTime dateTime;
    dateTime.setTime_t(99998);
    // there is some rounding difference impacting at sec level
    // hence don't compare directly
    QCOMPARE(info.timestamp().date(),dateTime.date());
    QCOMPARE(info.timestamp().time().hour(),dateTime.time().hour());
}

void TestQGeoPositionInfoSource::changeSource()
{
    CHECK_SOURCE_VALID;

    QSignalSpy positionUpdatedSpy(m_source, SIGNAL(positionUpdated(const QGeoPositionInfo&)));
    // First try updates with non-satellite sources and check that
    // correct values are received.
    m_source->setPreferredPositioningMethods(QGeoPositionInfoSource::SatellitePositioningMethods);
    m_source->setUpdateInterval(2000);
    m_source->startUpdates();
    QTRY_COMPARE_WITH_TIMEOUT_RANGE(positionUpdatedSpy.count(), 1,  1900, 2100);
    // Change source and check that updates continue
    m_source->setPreferredPositioningMethods(QGeoPositionInfoSource::AllPositioningMethods);
    positionUpdatedSpy.clear();
    QTRY_COMPARE_WITH_TIMEOUT_RANGE(positionUpdatedSpy.count(), 1, 1900, 2100);
}

void TestQGeoPositionInfoSource::initGoneBad()
{
    // Test various error conditions during init.
    QGeoPositionInfoSource* bad_source = 0;

    geocluemock_set_gcmaster_get_default(false);
    bad_source = QGeoPositionInfoSource::createDefaultSource(this);
    QVERIFY(!bad_source);
    geocluemock_set_gcmaster_get_default(true);
    bad_source = QGeoPositionInfoSource::createDefaultSource(this);
    QVERIFY(bad_source);
    delete bad_source;

    geocluemock_set_gcmaster_create_client(false);
    bad_source = QGeoPositionInfoSource::createDefaultSource(this);
    QVERIFY(!bad_source);
    geocluemock_set_gcmaster_create_client(true);
    bad_source = QGeoPositionInfoSource::createDefaultSource(this);
    QVERIFY(bad_source);
    delete bad_source;

    geocluemock_set_gcmaster_set_requirements(false);
    bad_source = QGeoPositionInfoSource::createDefaultSource(this);
    QVERIFY(!bad_source);
    geocluemock_set_gcmaster_set_requirements(true);
    bad_source = QGeoPositionInfoSource::createDefaultSource(this);
    QVERIFY(bad_source);
    delete bad_source;

    geocluemock_set_gcmaster_create_position(false);
    bad_source = QGeoPositionInfoSource::createDefaultSource(this);
    QVERIFY(!bad_source);
    geocluemock_set_gcmaster_create_position(true);
    bad_source = QGeoPositionInfoSource::createDefaultSource(this);
    QVERIFY(bad_source);
    delete bad_source;

    geocluemock_set_geoclue_velocity_new(false);
    bad_source = QGeoPositionInfoSource::createDefaultSource(this);
    QVERIFY(bad_source); // velocity is not mandatory
    delete bad_source;
    geocluemock_set_geoclue_velocity_new(true); // cleanup
}

void TestQGeoPositionInfoSource::updateInferno()
{
    // Tests different position update failure scenarios
    // Successful, normal update
    m_source->setUpdateInterval(1000);
    QSignalSpy updateSpy(m_source, SIGNAL(positionUpdated(const QGeoPositionInfo&)));
    QSignalSpy timeoutSpy(m_source, SIGNAL(updateTimeout()));
    m_source->startUpdates();
    QTRY_VERIFY_WITH_TIMEOUT(!updateSpy.isEmpty(), 2100);
    QVERIFY(timeoutSpy.isEmpty());

    // Suppress the regular updates and check that timeouts are received
    // Then resume the updates and see it goes ok.
    geocluemock_set_suppress_regular_updates(true);
    updateSpy.clear(); timeoutSpy.clear();
    QTest::qWait(2000);
    // there is no updateTimeout because there is no error
    QTRY_VERIFY_WITH_TIMEOUT(timeoutSpy.isEmpty(), 2000);
    QVERIFY(updateSpy.isEmpty());
    geocluemock_set_suppress_regular_updates(false);
    QTRY_VERIFY_WITH_TIMEOUT(!updateSpy.isEmpty(), 4000);
    QVERIFY(timeoutSpy.isEmpty());

    // Restart updates and check velocity is received when expected
    m_source->stopUpdates();
    updateSpy.clear(); timeoutSpy.clear();
    // start updates without velocity
    geocluemock_set_suppress_velocity_update(true);
    m_source->startUpdates();
    QTRY_VERIFY_WITH_TIMEOUT(!updateSpy.isEmpty(), 4000);
    QGeoPositionInfo info = updateSpy.takeFirst().at(0).value<QGeoPositionInfo>();
    QVERIFY(info.isValid());
    QVERIFY(!info.hasAttribute(QGeoPositionInfo::GroundSpeed));
    // enable velocity updates again
    geocluemock_set_suppress_velocity_update(false);
    updateSpy.clear(); timeoutSpy.clear();
    QTest::qWait(3000);
    QTRY_VERIFY_WITH_TIMEOUT(!updateSpy.isEmpty(), 4000);
    info = updateSpy.takeLast().at(0).value<QGeoPositionInfo>();
    QVERIFY(info.isValid());
    QVERIFY(info.hasAttribute(QGeoPositionInfo::GroundSpeed));
    m_source->stopUpdates();

    // Test that bad updatedata produces timeout both on regular updates
    // and single updates
    geocluemock_set_position_fields(0);
    updateSpy.clear(); timeoutSpy.clear();
    m_source->startUpdates();
    QTRY_VERIFY_WITH_TIMEOUT(!timeoutSpy.isEmpty(), 2000);
    QVERIFY(updateSpy.isEmpty());
    updateSpy.clear(); timeoutSpy.clear();
    m_source->requestUpdate(1000);
    QTRY_VERIFY_WITH_TIMEOUT(!timeoutSpy.isEmpty(), 2000);
    QVERIFY(updateSpy.isEmpty());
    geocluemock_unset_position_fields(); // clear

    // Test that lib works ok with instane lat/lon values
    geocluemock_set_position_latitude(1000);
    updateSpy.clear(); timeoutSpy.clear();
    QTRY_VERIFY_WITH_TIMEOUT(!updateSpy.isEmpty(), 2000);
    QVERIFY(timeoutSpy.isEmpty());
    info = updateSpy.takeLast().at(0).value<QGeoPositionInfo>();
    QVERIFY(!info.isValid());
    geocluemock_unset_position_latitude(); // clear

    // Test that invalid velocity updates are not emitted
    geocluemock_set_velocity_fields(0);
    updateSpy.clear(); timeoutSpy.clear();
    QTest::qWait(1500);
    QTRY_VERIFY_WITH_TIMEOUT(!updateSpy.isEmpty(), 2000);
    QVERIFY(timeoutSpy.isEmpty());
    info = updateSpy.takeLast().at(0).value<QGeoPositionInfo>();
    QVERIFY(info.isValid());
    QVERIFY(!info.hasAttribute(QGeoPositionInfo::GroundSpeed));
    geocluemock_unset_velocity_fields();

    // Test that requestUpdate timeouts correctly
    geocluemock_set_suppress_single_update(true);
    geocluemock_set_suppress_regular_updates(true);
    m_source->stopUpdates();
    updateSpy.clear(); timeoutSpy.clear();
    m_source->requestUpdate(2000);
    QTRY_COMPARE_WITH_TIMEOUT(timeoutSpy.count(), 1, 2100);
    QVERIFY(updateSpy.isEmpty());
    geocluemock_set_suppress_single_update(false);
    geocluemock_set_suppress_regular_updates(false);

    // Test changing interval during active updates works ok
    m_source->setUpdateInterval(1000);
    updateSpy.clear(); timeoutSpy.clear();
    m_source->startUpdates();
    QTRY_COMPARE_WITH_TIMEOUT_RANGE(updateSpy.count(), 1, 950, 1050);
    updateSpy.clear();
    m_source->setUpdateInterval(2000);
    QTRY_COMPARE_WITH_TIMEOUT_RANGE(updateSpy.count(), 1, 1900, 2100);
    // Check that unusual
}

void TestQGeoPositionInfoSource::robustness()
{
    // Perform uneducated API calls and make sure it behaves
    QVERIFY(m_source);
    QGeoPositionInfo info = m_source->lastKnownPosition(true);
    QVERIFY(!info.isValid());
    info = m_source->lastKnownPosition(false);
    QVERIFY(!info.isValid());
    m_source->stopUpdates();

    // Have changing source fail and check interface works.
    geocluemock_set_gcmaster_get_default(false);
    m_source->setPreferredPositioningMethods(QGeoPositionInfoSource::NonSatellitePositioningMethods);
    m_source->setUpdateInterval(1000);
    m_source->startUpdates();
    m_source->requestUpdate(1000);
    m_source->stopUpdates();
    info = m_source->lastKnownPosition(false);
    QVERIFY(!info.isValid());
    geocluemock_set_gcmaster_get_default(true);
    // Check that the source is able to recover
    m_source->setPreferredPositioningMethods(QGeoPositionInfoSource::SatellitePositioningMethods);
    QSignalSpy updateSpy(m_source, SIGNAL(positionUpdated(const QGeoPositionInfo&)));
    QSignalSpy timeoutSpy(m_source, SIGNAL(updateTimeout()));
    m_source->startUpdates();
    QTRY_VERIFY_WITH_TIMEOUT(!updateSpy.isEmpty(), 3000);
    info = updateSpy.takeLast().at(0).value<QGeoPositionInfo>();
    QVERIFY(info.isValid());
    QVERIFY(timeoutSpy.isEmpty());

    // Successfully change source during active updates and check updates continue
    m_source->setPreferredPositioningMethods(QGeoPositionInfoSource::NonSatellitePositioningMethods);
    updateSpy.clear(); timeoutSpy.clear();
    QTRY_VERIFY_WITH_TIMEOUT(!updateSpy.isEmpty(), 3000);
    info = updateSpy.takeLast().at(0).value<QGeoPositionInfo>();
    QVERIFY(info.isValid());
    QVERIFY(timeoutSpy.isEmpty());
    // Change again successfully the source and check that the requestUpdate is delivered
    m_source->stopUpdates();
    m_source->requestUpdate(2000);
    updateSpy.clear(); timeoutSpy.clear();
    m_source->setPreferredPositioningMethods(QGeoPositionInfoSource::SatellitePositioningMethods);
    QTRY_VERIFY_WITH_TIMEOUT(!updateSpy.isEmpty(), 3000);
    info = updateSpy.takeLast().at(0).value<QGeoPositionInfo>();
    QVERIFY(info.isValid());
    QVERIFY(timeoutSpy.isEmpty());

    // Change the source unsuccessfully and expect timeout
    m_source->startUpdates();
    geocluemock_set_gcmaster_get_default(false);
    updateSpy.clear(); timeoutSpy.clear();
    m_source->setPreferredPositioningMethods(QGeoPositionInfoSource::NonSatellitePositioningMethods);
    QTRY_VERIFY_WITH_TIMEOUT(!timeoutSpy.isEmpty(), 3000);
    QVERIFY(updateSpy.isEmpty());
    geocluemock_set_gcmaster_get_default(true);

    // Issue a start to unsuccessfully changed source and expect timeout
    updateSpy.clear(); timeoutSpy.clear();
    m_source->requestUpdate(2000);
    QTRY_VERIFY_WITH_TIMEOUT(!timeoutSpy.isEmpty(), 3000);
    QVERIFY(updateSpy.isEmpty());

    // Set unknown preferred method and check it has no impact (first set successful method)
    m_source->setPreferredPositioningMethods(QGeoPositionInfoSource::SatellitePositioningMethods);
    updateSpy.clear(); timeoutSpy.clear();
    m_source->startUpdates();
    QTRY_VERIFY_WITH_TIMEOUT(!updateSpy.isEmpty(), 3000);
    m_source->setPreferredPositioningMethods((QGeoPositionInfoSource::PositioningMethods)555);
    updateSpy.clear(); timeoutSpy.clear();
    QTRY_VERIFY_WITH_TIMEOUT(!updateSpy.isEmpty(), 3000);

    // Successfully change preferred method but without velocity
    geocluemock_set_geoclue_velocity_new(false);
    geocluemock_set_suppress_velocity_update(true);
    m_source->setPreferredPositioningMethods(QGeoPositionInfoSource::NonSatellitePositioningMethods);
    updateSpy.clear(); timeoutSpy.clear();
    QTRY_VERIFY_WITH_TIMEOUT(!updateSpy.isEmpty(), 3000);
    info = updateSpy.takeLast().at(0).value<QGeoPositionInfo>();
    QVERIFY(info.isValid());
    QVERIFY(!info.hasAttribute(QGeoPositionInfo::GroundSpeed));
    geocluemock_set_geoclue_velocity_new(true);
    geocluemock_set_suppress_velocity_update(false);

    // Unsuccessfully change source during active requestUpdate()
    m_source->stopUpdates();
    geocluemock_set_suppress_single_update(true); // Justin Case
    m_source->requestUpdate(1000);
    geocluemock_set_gcmaster_get_default(false);
    updateSpy.clear(); timeoutSpy.clear();
    m_source->setPreferredPositioningMethods(QGeoPositionInfoSource::NonSatellitePositioningMethods);
    QTRY_VERIFY_WITH_TIMEOUT(!timeoutSpy.isEmpty(), 3000);
    QVERIFY(updateSpy.isEmpty());
    geocluemock_set_gcmaster_get_default(true);
    geocluemock_set_suppress_single_update(false);

    // Check that setting same positioning method does nothing.
    // First setup successful and check updates are received
    m_source->setPreferredPositioningMethods(QGeoPositionInfoSource::NonSatellitePositioningMethods);
    m_source->startUpdates();
    updateSpy.clear(); timeoutSpy.clear();
    QTRY_VERIFY_WITH_TIMEOUT(!updateSpy.isEmpty(), 3000);
    // then set creation to fail. however should not impact because
    // methods are the same
    geocluemock_set_gcmaster_get_default(false);
    m_source->setPreferredPositioningMethods(QGeoPositionInfoSource::NonSatellitePositioningMethods);
    updateSpy.clear(); timeoutSpy.clear();
    QTRY_VERIFY_WITH_TIMEOUT(!updateSpy.isEmpty(), 3000);
}

#endif // TST_GEOCLUEMOCK_ENABLED

#include "testqgeopositioninfosource.moc"
