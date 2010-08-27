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

#include <QTest>
#include <QMetaType>
#include <QSignalSpy>
#include <QDebug>
#include <QTimer>

#include <qgeosatelliteinfosource.h>
#include <qgeosatelliteinfo.h>

#include "testqgeosatelliteinfosource_p.h"
#include "qlocationtestutils_p.h"

Q_DECLARE_METATYPE(QList<QGeoSatelliteInfo>)

#define MAX_WAITING_TIME 50000

// Must provide a valid source, unless testing the source
// returned by QGeoSatelliteInfoSource::createDefaultSource() on a system
// that has no default source
#define CHECK_SOURCE_VALID { \
        if (!m_source) { \
            if (m_testingDefaultSource && QGeoSatelliteInfoSource::createDefaultSource(0) == 0) \
                QSKIP("No default satellite source on this system", SkipAll); \
            else \
                QFAIL("createTestSource() must return a valid source!"); \
        } \
    }

class MySatelliteSource : public QGeoSatelliteInfoSource
{
    Q_OBJECT
public:
    MySatelliteSource(QObject *parent = 0)
            : QGeoSatelliteInfoSource(parent) {
    }

    virtual void startUpdates() {}
    virtual void stopUpdates() {}

    virtual void requestUpdate(int) {}
};

class DefaultSourceTest : public TestQGeoSatelliteInfoSource
{
    Q_OBJECT
protected:
    QGeoSatelliteInfoSource *createTestSource() {
        return QGeoSatelliteInfoSource::createDefaultSource(0);
    }
};

TestQGeoSatelliteInfoSource::TestQGeoSatelliteInfoSource(QObject *parent)
        : QObject(parent)
{
    m_testingDefaultSource = false;
}

TestQGeoSatelliteInfoSource *TestQGeoSatelliteInfoSource::createDefaultSourceTest()
{
    DefaultSourceTest *test = new DefaultSourceTest;
    test->m_testingDefaultSource = true;
    return test;
}

void TestQGeoSatelliteInfoSource::base_initTestCase()
{
    qRegisterMetaType<QList<QGeoSatelliteInfo> >();
}

void TestQGeoSatelliteInfoSource::base_init()
{
    m_source = createTestSource();
    m_testSlot2Called = false;
}

void TestQGeoSatelliteInfoSource::base_cleanup()
{
    delete m_source;
    m_source = 0;
}

void TestQGeoSatelliteInfoSource::base_cleanupTestCase()
{
}

void TestQGeoSatelliteInfoSource::initTestCase()
{
    base_initTestCase();
}

void TestQGeoSatelliteInfoSource::init()
{
    base_init();
}

void TestQGeoSatelliteInfoSource::cleanup()
{
    base_cleanup();
}

void TestQGeoSatelliteInfoSource::cleanupTestCase()
{
    base_cleanupTestCase();
}

void TestQGeoSatelliteInfoSource::test_slot1()
{
}

void TestQGeoSatelliteInfoSource::test_slot2()
{
    m_testSlot2Called = true;
}

void TestQGeoSatelliteInfoSource::constructor_withParent()
{
    //QLocationTestUtils::uheap_mark();
    QObject *parent = new QObject();
    new MySatelliteSource(parent);
    delete parent;
    //QLocationTestUtils::uheap_mark_end();
}

void TestQGeoSatelliteInfoSource::constructor_noParent()
{
    //QLocationTestUtils::uheap_mark();
    MySatelliteSource *obj = new MySatelliteSource();
    delete obj;
    //QLocationTestUtils::uheap_mark_end();
}

void TestQGeoSatelliteInfoSource::createDefaultSource()
{
    //QLocationTestUtils::uheap_mark();
    QObject *parent = new QObject;

    QGeoSatelliteInfoSource *source = QGeoSatelliteInfoSource::createDefaultSource(parent);
#if defined(Q_OS_SYMBIAN)
    QVERIFY(source != 0);
#elif defined(Q_OS_WINCE)
    QVERIFY(source != 0);
#elif defined(Q_WS_MAEMO_5)
    QVERIFY(source != 0);
#else    
    QVERIFY(source == 0);
#endif
    delete parent;
    //QLocationTestUtils::uheap_mark_end();
}

void TestQGeoSatelliteInfoSource::createDefaultSource_noParent()
{
    //QLocationTestUtils::uheap_mark();

    QGeoSatelliteInfoSource *source = QGeoSatelliteInfoSource::createDefaultSource(0);
#if defined(Q_OS_SYMBIAN)
    QVERIFY(source != 0);
#elif defined(Q_OS_WINCE)
    QVERIFY(source != 0);
#elif defined(Q_WS_MAEMO_5)
    QVERIFY(source != 0);
#else
    QVERIFY(source == 0);
#endif
    delete source;
    //QLocationTestUtils::uheap_mark_end();
}

void TestQGeoSatelliteInfoSource::startUpdates()
{

    CHECK_SOURCE_VALID;

    QSignalSpy spyView(m_source,
                       SIGNAL(satellitesInViewUpdated(const QList<QGeoSatelliteInfo> &)));
    QSignalSpy spyUse(m_source,
                      SIGNAL(satellitesInUseUpdated(const QList<QGeoSatelliteInfo> &)));

    m_source->startUpdates();

    for (int i = 0; i < 3; i++) {

        EXPECT_FAIL_WINCE_SEE_MOBILITY_337;

        QTRY_VERIFY_WITH_TIMEOUT((spyView.count() > 0) && (spyUse.count() > 0), 12000);
        spyView.clear();
        spyUse.clear();
    }

    m_source->stopUpdates();
}

void TestQGeoSatelliteInfoSource::startUpdates_moreThanOnce()
{
    CHECK_SOURCE_VALID;

    QSignalSpy spyView(m_source,
                       SIGNAL(satellitesInViewUpdated(const QList<QGeoSatelliteInfo> &)));
    QSignalSpy spyUse(m_source,
                      SIGNAL(satellitesInUseUpdated(const QList<QGeoSatelliteInfo> &)));

    m_source->startUpdates();

    m_source->startUpdates(); // check there is no crash

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337;

    QTRY_VERIFY_WITH_TIMEOUT((spyView.count() > 0) && (spyUse.count() > 0), MAX_WAITING_TIME);

    m_source->startUpdates(); // check there is no crash

    m_source->stopUpdates();
}

void TestQGeoSatelliteInfoSource::stopUpdates()
{

    CHECK_SOURCE_VALID;

    QSignalSpy spyView(m_source,
                       SIGNAL(satellitesInViewUpdated(const QList<QGeoSatelliteInfo> &)));
    QSignalSpy spyUse(m_source,
                      SIGNAL(satellitesInUseUpdated(const QList<QGeoSatelliteInfo> &)));

    m_source->startUpdates();

    for (int i = 0; i < 2; i++) {

        EXPECT_FAIL_WINCE_SEE_MOBILITY_337;

        QTRY_VERIFY_WITH_TIMEOUT((spyView.count() == 1) && (spyUse.count() == 1), 12000);
        spyView.clear();
        spyUse.clear();
    }

    m_source->stopUpdates();

    QTRY_VERIFY_WITH_TIMEOUT((spyView.count() == 0) && (spyUse.count() == 0), 12000);
}

void TestQGeoSatelliteInfoSource::stopUpdates_withoutStart()
{
    CHECK_SOURCE_VALID;

    m_source->stopUpdates(); // check there is no crash
}

void TestQGeoSatelliteInfoSource::requestUpdate()
{
    CHECK_SOURCE_VALID;

    QFETCH(int, timeout);
    QSignalSpy spy(m_source, SIGNAL(requestTimeout()));
    m_source->requestUpdate(timeout);
    QTRY_COMPARE(spy.count(), 1);
}

void TestQGeoSatelliteInfoSource::requestUpdate_data()
{
    QTest::addColumn<int>("timeout");
    QTest::newRow("less than zero") << -1;
    QTest::newRow("very small timeout") << 1;
}

void TestQGeoSatelliteInfoSource::requestUpdate_validTimeout()
{
    CHECK_SOURCE_VALID;

    QSignalSpy spyView(m_source,
                       SIGNAL(satellitesInViewUpdated(const QList<QGeoSatelliteInfo> &)));
    QSignalSpy spyUse(m_source,
                      SIGNAL(satellitesInUseUpdated(const QList<QGeoSatelliteInfo> &)));
    QSignalSpy spyTimeout(m_source, SIGNAL(requestTimeout()));

    m_source->requestUpdate(7000);

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337;

    QTRY_VERIFY_WITH_TIMEOUT(
            (spyView.count() == 1) && (spyUse.count() == 1 && (spyTimeout.count()) == 0), 7000);
}

void TestQGeoSatelliteInfoSource::requestUpdate_defaultTimeout()
{
    CHECK_SOURCE_VALID;

    QSignalSpy spyView(m_source,
                       SIGNAL(satellitesInViewUpdated(const QList<QGeoSatelliteInfo> &)));
    QSignalSpy spyUse(m_source,
                      SIGNAL(satellitesInUseUpdated(const QList<QGeoSatelliteInfo> &)));
    QSignalSpy spyTimeout(m_source, SIGNAL(requestTimeout()));

    m_source->requestUpdate(0);

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337;

    QTRY_VERIFY_WITH_TIMEOUT(
            (spyView.count() == 1) && (spyUse.count() == 1 && (spyTimeout.count()) == 0), 
            MAX_WAITING_TIME);
}

void TestQGeoSatelliteInfoSource::requestUpdate_repeatedCalls()
{
    CHECK_SOURCE_VALID;

    QSignalSpy spyView(m_source,
                       SIGNAL(satellitesInViewUpdated(const QList<QGeoSatelliteInfo> &)));
    QSignalSpy spyUse(m_source,
                      SIGNAL(satellitesInUseUpdated(const QList<QGeoSatelliteInfo> &)));

    m_source->requestUpdate(7000);

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337;

    QTRY_VERIFY_WITH_TIMEOUT((spyView.count() == 1) && (spyUse.count() == 1), 7000);
    spyView.clear();
    spyUse.clear();

    m_source->requestUpdate(7000);

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337;

    QTRY_VERIFY_WITH_TIMEOUT((spyView.count() == 1) && (spyUse.count() == 1), 7000);
}

void TestQGeoSatelliteInfoSource::requestUpdate_overlappingCalls()
{
    CHECK_SOURCE_VALID;

    QSignalSpy spyView(m_source,
                       SIGNAL(satellitesInViewUpdated(const QList<QGeoSatelliteInfo> &)));
    QSignalSpy spyUse(m_source,
                      SIGNAL(satellitesInUseUpdated(const QList<QGeoSatelliteInfo> &)));

    m_source->requestUpdate(7000);
    m_source->requestUpdate(7000);

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337;

    QTRY_VERIFY_WITH_TIMEOUT((spyView.count() == 1) && (spyUse.count() == 1), 7000);
}

void TestQGeoSatelliteInfoSource::requestUpdate_overlappingCallsWithTimeout()
{
    CHECK_SOURCE_VALID;

    QSignalSpy spyView(m_source,
                       SIGNAL(satellitesInViewUpdated(const QList<QGeoSatelliteInfo> &)));
    QSignalSpy spyUse(m_source,
                      SIGNAL(satellitesInUseUpdated(const QList<QGeoSatelliteInfo> &)));
	QSignalSpy spyTimeout(m_source,
                      SIGNAL(requestTimeout()));

    m_source->requestUpdate(0);
    m_source->requestUpdate(1);
	
    QTRY_COMPARE_WITH_TIMEOUT(spyTimeout.count(), 0, 7000);

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337;

    QTRY_VERIFY_WITH_TIMEOUT((spyView.count() == 1) && (spyUse.count() == 1) && (spyTimeout.count() == 0), 7000);
}

void TestQGeoSatelliteInfoSource::requestUpdateAfterStartUpdates()
{
    CHECK_SOURCE_VALID;

    QSignalSpy spyView(m_source,
                       SIGNAL(satellitesInViewUpdated(const QList<QGeoSatelliteInfo> &)));
    QSignalSpy spyUse(m_source,
                      SIGNAL(satellitesInUseUpdated(const QList<QGeoSatelliteInfo> &)));
    QSignalSpy spyTimeout(m_source, SIGNAL(requestTimeout()));

    m_source->startUpdates();

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337;

    QTRY_VERIFY_WITH_TIMEOUT((spyView.count() == 1) && (spyUse.count() == 1), MAX_WAITING_TIME);
    spyView.clear();
    spyUse.clear();

    m_source->requestUpdate(7000);

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337;

    QTRY_VERIFY_WITH_TIMEOUT((spyView.count() == 1) && (spyUse.count() == 1)
                             && (spyTimeout.count() == 0), 7000);

    spyView.clear();
    spyUse.clear();

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337;

    QTRY_VERIFY_WITH_TIMEOUT((spyView.count() == 1) && (spyUse.count() == 1), 12000);

    m_source->stopUpdates();
}

void TestQGeoSatelliteInfoSource::requestUpdateBeforeStartUpdates()
{
    CHECK_SOURCE_VALID;

    QSignalSpy spyView(m_source,
                       SIGNAL(satellitesInViewUpdated(const QList<QGeoSatelliteInfo> &)));
    QSignalSpy spyUse(m_source,
                      SIGNAL(satellitesInUseUpdated(const QList<QGeoSatelliteInfo> &)));
    QSignalSpy spyTimeout(m_source, SIGNAL(requestTimeout()));

    m_source->requestUpdate(7000);

    m_source->startUpdates();

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337;

    QTRY_VERIFY_WITH_TIMEOUT((spyView.count() > 0) && (spyUse.count() > 0)
                             && (spyTimeout.count() == 0), 7000);

    spyView.clear();
    spyUse.clear();

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337;

    QTRY_VERIFY_WITH_TIMEOUT((spyView.count() == 1) && (spyUse.count() == 1), 12000);

    m_source->stopUpdates();
}

void TestQGeoSatelliteInfoSource::removeSlotForRequestTimeout()
{
	CHECK_SOURCE_VALID;
	
	bool i = connect(m_source, SIGNAL(requestTimeout()), this, SLOT(test_slot1()));
	QVERIFY(i==true);
	i = connect(m_source, SIGNAL(requestTimeout()), this, SLOT(test_slot2()));
	QVERIFY(i==true);
	i = disconnect(m_source, SIGNAL(requestTimeout()), this, SLOT(test_slot1()));
	QVERIFY(i==true);
	
	m_source->requestUpdate(-1);
    QTRY_VERIFY_WITH_TIMEOUT((m_testSlot2Called == true), 1000);
}

void TestQGeoSatelliteInfoSource::removeSlotForSatellitesInUseUpdated()
{
    CHECK_SOURCE_VALID;

    bool i = connect(m_source, SIGNAL(satellitesInUseUpdated(const QList<QGeoSatelliteInfo> &)), this, SLOT(test_slot1()));
    QVERIFY(i == true);
    i = connect(m_source, SIGNAL(satellitesInUseUpdated(const QList<QGeoSatelliteInfo> &)), this, SLOT(test_slot2()));
    QVERIFY(i == true);
    i = disconnect(m_source, SIGNAL(satellitesInUseUpdated(const QList<QGeoSatelliteInfo> &)), this, SLOT(test_slot1()));
    QVERIFY(i == true);

    m_source->requestUpdate(7000);

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337;

    QTRY_VERIFY_WITH_TIMEOUT((m_testSlot2Called == true), 7000);
}

void TestQGeoSatelliteInfoSource::removeSlotForSatellitesInViewUpdated()
{
    CHECK_SOURCE_VALID;

    bool i = connect(m_source, SIGNAL(satellitesInViewUpdated(const QList<QGeoSatelliteInfo> &)), this, SLOT(test_slot1()));
    QVERIFY(i == true);
    i = connect(m_source, SIGNAL(satellitesInViewUpdated(const QList<QGeoSatelliteInfo> &)), this, SLOT(test_slot2()));
    QVERIFY(i == true);
    i = disconnect(m_source, SIGNAL(satellitesInViewUpdated(const QList<QGeoSatelliteInfo> &)), this, SLOT(test_slot1()));
    QVERIFY(i == true);

    m_source->requestUpdate(7000);

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337;

    QTRY_VERIFY_WITH_TIMEOUT((m_testSlot2Called == true), 7000);
}

#include "testqgeosatelliteinfosource.moc"
