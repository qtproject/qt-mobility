/*********************f*******************************************************
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

#include <qgeosatelliteinfosource.h>
#include <qgeosatelliteinfo.h>

#include "testqgeosatelliteinfosource_p.h"
#include "qlocationtestutils_p.h"
#ifdef TST_GYPSYMOCK_ENABLED
#include "qgeosatelliteinfosource_gypsy_p.h"
#include <QProcessEnvironment>
#include "../gypsymock/gypsymock.h"
#endif

Q_DECLARE_METATYPE(QList<QGeoSatelliteInfo>)

#define MAX_WAITING_TIME 50000

// Must provide a valid source, unless testing the source
// returned by QGeoSatelliteInfoSource::createDefaultSource() on a system
// that has no default source
// todo is a mock check useful here?
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

#ifdef TST_GYPSYMOCK_ENABLED
// Mocked platform calls. Depending on environment variable,
// either calls the mocked version or the real one (parentclass).
class TestSatelliteGypsyEngine : public SatelliteGypsyEngine
{
public:
    TestSatelliteGypsyEngine(QGeoSatelliteInfoSource *parent = 0):
        SatelliteGypsyEngine(parent), m_gypsymock(0)
    {
    }
    ~TestSatelliteGypsyEngine()
    {
        m_thread.quit();
        m_thread.wait();
        if (m_gypsymock) {
            delete m_gypsymock;
            m_gypsymock = 0;
        }
    }

    gulong eng_g_signal_connect(gpointer instance,
                                    const gchar* detailed_signal,
                                    GCallback c_handler,
                                    gpointer data)
    {
        Q_UNUSED(instance);
        //TRACE1("============= blocking call to connectSignal");
        QMetaObject::invokeMethod(m_gypsymock,
                                  "connectSignal",
                                  Qt::BlockingQueuedConnection,
                                  //Qt::QueuedConnection,
                                  Q_ARG(int, (int)detailed_signal),
                                  Q_ARG(int, (int)c_handler),
                                  Q_ARG(int, (int)data));
        //TRACE1("============= nonblocking call to connectSignal DONE");
        return 1; // dummy
    }
    guint eng_g_signal_handlers_disconnect_by_func (gpointer instance,
                                                        gpointer func,
                                                        gpointer data)
    {
        Q_UNUSED(instance);
        //TRACE1("============= blocking call to disconnectSignal");
        QMetaObject::invokeMethod(m_gypsymock,
                                  "disconnectSignal",
                                  //Qt::BlockingQueuedConnection,
                                  Qt::QueuedConnection,
                                  Q_ARG(int, (int)func),
                                  Q_ARG(int, (int)data));
        //TRACE1("============= blocking call to disconnectSignal DONE");
        return 1; // dummy
    }

    void eng_g_free(gpointer mem)
    {
        // todo crashes
        //free((char*)mem);
    }

    GypsyControl* eng_gypsy_control_get_default (void)
    {
        return (GypsyControl*)g_object_new (G_TYPE_OBJECT, NULL); // dummy
    }
    char *eng_gypsy_control_create (GypsyControl *control, const char* device_name, GError **error)
    {
        // use root '/' here because it usually exists (sat source makes
        // existense checks)
        if (!control || QString::compare(QString::fromAscii(device_name), "/")) {
            if (error) {
                *error = g_error_new ((GQuark)1, // dummy
                                      2,         // dummy
                                      "=mock= No control or wrong device_name.");
            }
            return NULL;
        }
        return (char*)"/my/little/gypsycontrol";
    }

    GypsyDevice *eng_gypsy_device_new (const char* object_path)
    {
        // Create new mockend.
        if (!object_path || QString::compare(QString::fromAscii(object_path), "/my/little/gypsycontrol")) {
            TRACE1("No correctobject path given or mockend already existed.");
            return NULL;
        }

        if (m_gypsymock)
            delete m_gypsymock;
        m_gypsymock = new GypsyMock;
        m_gypsymock->moveToThread(&m_thread);
        m_thread.start();
        return (GypsyDevice*)g_object_new (G_TYPE_OBJECT, NULL); // dummy
    }
    GypsySatellite *eng_gypsy_satellite_new (const char *object_path)
    {
        if (!object_path || QString::compare(QString::fromAscii(object_path), "/my/little/gypsycontrol")) {
            return NULL;
        }
        return (GypsySatellite*)g_object_new (G_TYPE_OBJECT, NULL); // dummy
    }
    gboolean eng_gypsy_device_start (GypsyDevice *device, GError **error)
    {
        if (!device || !m_gypsymock) {
            if (error) {
                *error = g_error_new ((GQuark)1, // dummy
                                      2,         // dummy
                                      "=mock= No device provided or mockend not created at gypsy_device_start");
            }
            return false;
        }
        //TRACE1("============= blocking call to start");
        QMetaObject::invokeMethod(m_gypsymock,
                                  "start",
                                  Qt::BlockingQueuedConnection
                                  //Qt::QueuedConnection
                                  );
        //TRACE1("============= blocking call to start DONE");
        return true;
    }
    gboolean eng_gypsy_device_stop (GypsyDevice *device, GError **error)
    {
        if (!device || !m_gypsymock) {
            if (error) {
                *error = g_error_new ((GQuark)1, // dummy
                                      2,         // dummy
                                      "=mock= No device provided or mockend not created at gypsy_device_stop");
            }
            return false;
        }
        //TRACE1("============= blocking call to stop");
        QMetaObject::invokeMethod(m_gypsymock,
                                  "stop",
                                  Qt::BlockingQueuedConnection
                                  //Qt::QueuedConnection
                                  );
        //TRACE1("============= blocking call to DONE");
        return true;
    }
    GypsyDeviceFixStatus eng_gypsy_device_get_fix_status (GypsyDevice *device, GError **error)
    {
        if (!device) {
            if (error) {
                *error = g_error_new ((GQuark)1, // dummy
                                      2,         // dummy
                                      "=mock= No device provided for gypsy_device_get_fix_status");
            }
            return GYPSY_DEVICE_FIX_STATUS_INVALID;
        }
        return GYPSY_DEVICE_FIX_STATUS_NONE;
    }
    GPtrArray *eng_gypsy_satellite_get_satellites (GypsySatellite *satellite, GError **error)
    {
        TRACE1("===== NOT YET IMPLEMENTED IN MOCK! =====");
        if (!satellite) {
            if (error) {
                *error = g_error_new ((GQuark)1, // dummy
                                      2,         // dummy
                                      "=mock= No satellite provided for gypsy_satellite_get_satellites");
            }
            return NULL;
        }
        return (GPtrArray*)1; // todo
    }
    void eng_gypsy_satellite_free_satellite_array (GPtrArray *satellites)
    {
        for (uint i = 0; i < satellites->len; i++) {
            delete (GypsySatelliteDetails*)g_ptr_array_index(satellites, i);
        }
        g_ptr_array_free(satellites, true);
    }
    // GConf symbols (mockability due to gconf X11 requirement)
    GConfClient* eng_gconf_client_get_default(void)
    {
        return (GConfClient*)g_object_new (G_TYPE_OBJECT, NULL); // dummy
    }
    gchar* eng_gconf_client_get_string(GConfClient* client, const gchar* key, GError** err)
    {
        // QString::compare return value is a bit 'unintuitive'
        TRACE1(key);
        if (!client || QString::compare(QString::fromAscii(key),"/apps/geoclue/master/org.freedesktop.Geoclue.GPSDevice")) {
            if (err) {
                *err = g_error_new ((GQuark)1, // dummy
                                    2,       // dummy
                                    "=mock= No client or wrong key in gconf_client_get_string");
            }
            return 0;
        }  else {
            return (gchar*)"/";
        }
    }
private:
    GypsyMock* m_gypsymock;
    QThread m_thread;
};

class TestQGeoSatelliteInfoSourceGypsy : public QGeoSatelliteInfoSourceGypsy
{
public:
    TestQGeoSatelliteInfoSourceGypsy(QObject *parent = 0) :
        QGeoSatelliteInfoSourceGypsy(parent) {}
protected:
    // override to mock platform calls
    void createEngine() { m_engine = new TestSatelliteGypsyEngine(this); }
};

#endif // TST_GYPSYMOCK_ENABLED

class DefaultSourceTest : public TestQGeoSatelliteInfoSource
{
    Q_OBJECT
protected:
    QGeoSatelliteInfoSource *createTestSource() {
#ifdef TST_GYPSYMOCK_ENABLED
        TestQGeoSatelliteInfoSourceGypsy *source = new TestQGeoSatelliteInfoSourceGypsy(0);
        int status = source->init();
        if (status == -1) {
            delete source;
            return 0;
        }
        return source;
#else
        return QGeoSatelliteInfoSource::createDefaultSource(0);

#endif
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
#ifdef TST_GYPSYMOCK_ENABLED
    QGeoSatelliteInfoSourceGypsy* source = new TestQGeoSatelliteInfoSourceGypsy(parent);
    int status = source->init();
    if (status == -1) {
        delete source;
        source = 0;
    }
#else
    QGeoSatelliteInfoSource *source = QGeoSatelliteInfoSource::createDefaultSource(parent);
#endif
#if defined(Q_OS_SYMBIAN)
    QVERIFY(source != 0);
#elif defined(Q_OS_WINCE)
    QVERIFY(source != 0);
#elif defined(Q_WS_MAEMO_5)
    QVERIFY(source != 0);
#elif defined(Q_WS_MAEMO_6)
    QVERIFY(source != 0);
#elif defined(TST_GYPSYMOCK_ENABLED)
    QVERIFY(source != 0);
#elif defined(Q_WS_MEEGO)
    Q_UNUSED(source);
    // with meego the source may or may not be present,
    // as it may be a usb/bt device
#else
    QVERIFY(source == 0);
#endif
    delete parent;
    //QLocationTestUtils::uheap_mark_end();
}

void TestQGeoSatelliteInfoSource::createDefaultSource_noParent()
{
    //QLocationTestUtils::uheap_mark();
#ifdef TST_GYPSYMOCK_ENABLED
    QGeoSatelliteInfoSourceGypsy *source = new TestQGeoSatelliteInfoSourceGypsy(0);
    int status = source->init();
    if (status == -1) {
        delete source;
        source = 0;
    }
#else
    QGeoSatelliteInfoSource *source = QGeoSatelliteInfoSource::createDefaultSource(0);
#endif
#if defined(Q_OS_SYMBIAN)
    QVERIFY(source != 0);
#elif defined(Q_OS_WINCE)
    QVERIFY(source != 0);
#elif defined(Q_WS_MAEMO_5)
    QVERIFY(source != 0);
#elif defined(Q_WS_MAEMO_6)
    QVERIFY(source != 0);
#elif defined(TST_GYPSYMOCK_ENABLED)
    QVERIFY(source != 0);
#elif defined(Q_WS_MEEGO)
    // with meego the source may or may not be present,
    // as it may be a usb/bt device
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
    QSignalSpy spyView(m_source,
                       SIGNAL(satellitesInViewUpdated(const QList<QGeoSatelliteInfo> &)));
    m_source->requestUpdate(timeout);
    // Geoclue may deliver update instantly if there is a satellite fix
    QTRY_VERIFY_WITH_TIMEOUT(!spy.isEmpty() || !spyView.isEmpty(), 10);
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

#if defined(Q_WS_MEEGO) && !defined(TST_GYPSYMOCK_ENABLED)
    // With Gypsy we migh actually get two updates instantly.
    // Not with mocked backend though.
    QTRY_VERIFY_WITH_TIMEOUT((spyView.count() == 2) && (spyUse.count() == 2), 7000);
#else
    QTRY_VERIFY_WITH_TIMEOUT((spyView.count() == 1) && (spyUse.count() == 1), 7000);
#endif
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

#if defined(Q_WS_MEEGO) && !defined(TST_GYPSYMOCK_ENABLED)
    // With Gypsy we migh actually get two updates instantly.
    // Not with mocked backend though.
    QTRY_VERIFY_WITH_TIMEOUT((spyView.count() == 2) && (spyUse.count() == 2), 7000);
#else
    QTRY_VERIFY_WITH_TIMEOUT((spyView.count() == 1) && (spyUse.count() == 1), 7000);
#endif
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
