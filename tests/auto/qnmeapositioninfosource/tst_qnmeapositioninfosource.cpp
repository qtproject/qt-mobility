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
** contact Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/
#include "../qnmeapositioninfosourceproxyfactory.h"
#include "../qgeopositioninfosourcesubclasstest_p.h"
#include "../qlocationtestutils_p.h"

#include <qnmeapositioninfosource.h>

#include <QTest>
#include <QDebug>
#include <QBuffer>
#include <QSignalSpy>
#include <QMetaType>
#include <QFile>
#include <QTemporaryFile>
#include <QHash>

Q_DECLARE_METATYPE(QNmeaPositionInfoSource::UpdateMode)
Q_DECLARE_METATYPE(QGeoPositionInfo)
Q_DECLARE_METATYPE(QList<QDateTime>)

class tst_QNmeaPositionInfoSource : public QObject
{
    Q_OBJECT

public:
    enum UpdateTriggerMethod
    {
        StartUpdatesMethod,
        RequestUpdatesMethod
    };

    tst_QNmeaPositionInfoSource(QNmeaPositionInfoSource::UpdateMode mode, QObject *parent = 0);

private slots:
    void initTestCase();

    void constructor();

    void supportedPositioningMethods();

    void minimumUpdateInterval();

    void testWithBufferedData();
    void testWithBufferedData_data();

    void startUpdates_waitForValidDateTime();
    void startUpdates_waitForValidDateTime_data();

    void requestUpdate_waitForValidDateTime();
    void requestUpdate_waitForValidDateTime_data();

    void testWithBadNmea();
    void testWithBadNmea_data();

private:
    QNmeaPositionInfoSource::UpdateMode m_mode;
};

Q_DECLARE_METATYPE(tst_QNmeaPositionInfoSource::UpdateTriggerMethod)


tst_QNmeaPositionInfoSource::tst_QNmeaPositionInfoSource(QNmeaPositionInfoSource::UpdateMode mode, QObject *parent)
    : QObject(parent),
      m_mode(mode)
{
}

void tst_QNmeaPositionInfoSource::initTestCase()
{
    qRegisterMetaType<QGeoPositionInfo>();
    qRegisterMetaType<QNmeaPositionInfoSource::UpdateMode>();
}

void tst_QNmeaPositionInfoSource::constructor()
{
    QObject o;
    QNmeaPositionInfoSource source(m_mode, &o);
    QCOMPARE(source.updateMode(), m_mode);
    QCOMPARE(source.parent(), &o);
}

void tst_QNmeaPositionInfoSource::supportedPositioningMethods()
{
    QNmeaPositionInfoSource source(m_mode);
    QCOMPARE(source.supportedPositioningMethods(), QNmeaPositionInfoSource::SatellitePositioningMethods);
}

void tst_QNmeaPositionInfoSource::minimumUpdateInterval()
{
    QNmeaPositionInfoSource source(m_mode);
    QCOMPARE(source.minimumUpdateInterval(), 100);
}

void tst_QNmeaPositionInfoSource::testWithBufferedData()
{
    // In SimulationMode, data stored in the QIODevice is read when
    // startUpdates() or requestUpdate() is called.
    // In RealTimeMode, all existing data in the QIODevice is ignored -
    // only new data will be read.

    QFETCH(QList<QDateTime>, dateTimes);
    QFETCH(UpdateTriggerMethod, trigger);

    QByteArray bytes;
    for (int i=0; i<dateTimes.count(); i++)
        bytes += QLocationTestUtils::createRmcSentence(dateTimes[i]).toLatin1();
    QBuffer buffer;
    buffer.setData(bytes);

    QNmeaPositionInfoSource source(m_mode);
    QSignalSpy spy(&source, SIGNAL(positionUpdated(QGeoPositionInfo)));
    source.setDevice(&buffer);

    if (trigger == StartUpdatesMethod)
        source.startUpdates();
    else if (trigger == RequestUpdatesMethod)
        source.requestUpdate();

    if (m_mode == QNmeaPositionInfoSource::RealTimeMode) {
        QTest::qWait(300);
        QCOMPARE(spy.count(), 0);
    } else {
        if (trigger == StartUpdatesMethod) {
            QTRY_COMPARE(spy.count(), dateTimes.count());
            for (int i=0; i<dateTimes.count(); i++)
                QCOMPARE(spy.at(i).at(0).value<QGeoPositionInfo>().dateTime(), dateTimes[i]);
        } else if (trigger == RequestUpdatesMethod) {
            QTRY_COMPARE(spy.count(), 1);
            QCOMPARE(spy.at(0).at(0).value<QGeoPositionInfo>().dateTime(), dateTimes.first());
        }
    }
}

void tst_QNmeaPositionInfoSource::testWithBufferedData_data()
{
    QTest::addColumn<QList<QDateTime> >("dateTimes");
    QTest::addColumn<UpdateTriggerMethod>("trigger");

    QList<QDateTime> dateTimes;
    dateTimes << QDateTime::currentDateTime().toUTC();

    QTest::newRow("startUpdates(), 1 update in buffer") << dateTimes << StartUpdatesMethod;
    QTest::newRow("requestUpdate(), 1 update in buffer") << dateTimes << RequestUpdatesMethod;

    for (int i=1; i<3; i++)
        dateTimes << dateTimes[0].addDays(i);
    QTest::newRow("startUpdates(), multiple updates in buffer") << dateTimes << StartUpdatesMethod;
    QTest::newRow("requestUpdate(), multiple updates in buffer") << dateTimes << RequestUpdatesMethod;
}

void tst_QNmeaPositionInfoSource::startUpdates_waitForValidDateTime()
{
    // Tests that the class does not emit an update until it receives a
    // sentences with a valid date *and* time. All sentences before this
    // should be ignored, and any sentences received after this that do
    // not have a date should use the known date.

    QFETCH(QByteArray, bytes);
    QFETCH(QList<QDateTime>, dateTimes);

    QNmeaPositionInfoSourceProxyFactory factory(m_mode);
    QNmeaPositionInfoSourceProxy *proxy = static_cast<QNmeaPositionInfoSourceProxy*>(factory.createProxy());

    QSignalSpy spy(proxy->source(), SIGNAL(positionUpdated(QGeoPositionInfo)));
    proxy->source()->startUpdates();

    proxy->feedBytes(bytes);
    QTRY_COMPARE(spy.count(), dateTimes.count());

    for (int i=0; i<spy.count(); i++)
        QCOMPARE(spy[i][0].value<QGeoPositionInfo>().dateTime(), dateTimes[i]);
}

void tst_QNmeaPositionInfoSource::startUpdates_waitForValidDateTime_data()
{
    QTest::addColumn<QByteArray>("bytes");
    QTest::addColumn<QList<QDateTime> >("dateTimes");

    QDateTime dt = QDateTime::currentDateTime().toUTC();
    QByteArray bytes;

    // should only receive RMC sentence and the GGA sentence *after* it
    bytes += QLocationTestUtils::createGgaSentence(dt.addSecs(1).time()).toLatin1();
    bytes += QLocationTestUtils::createRmcSentence(dt.addSecs(2)).toLatin1();
    bytes += QLocationTestUtils::createGgaSentence(dt.addSecs(3).time()).toLatin1();
    QTest::newRow("Feed GGA,RMC,GGA; expect RMC, second GGA only")
            << bytes << (QList<QDateTime>() << dt.addSecs(2) << dt.addSecs(3));

    // should not receive ZDA (has no coordinates) but should get the GGA
    // sentence after it since it got the date/time from ZDA
    bytes.clear();
    bytes += QLocationTestUtils::createGgaSentence(dt.addSecs(1).time()).toLatin1();
    bytes += QLocationTestUtils::createZdaSentence(dt.addSecs(2)).toLatin1();
    bytes += QLocationTestUtils::createGgaSentence(dt.addSecs(3).time()).toLatin1();
    QTest::newRow("Feed GGA,ZDA,GGA; expect second GGA only")
            << bytes << (QList<QDateTime>() << dt.addSecs(3));

    if (m_mode == QNmeaPositionInfoSource::SimulationMode) {
        // In sim m_mode, should ignore sentence with a date/time before the known date/time
        // (in real time m_mode, everything is passed on regardless)
        bytes.clear();
        bytes += QLocationTestUtils::createRmcSentence(dt.addSecs(1)).toLatin1();
        bytes += QLocationTestUtils::createRmcSentence(dt.addSecs(-2)).toLatin1();
        bytes += QLocationTestUtils::createRmcSentence(dt.addSecs(2)).toLatin1();
        QTest::newRow("Feed good RMC, RMC with bad date/time, good RMC; expect first and third RMC only")
                << bytes << (QList<QDateTime>() << dt.addSecs(1) << dt.addSecs(2));
    }
}

void tst_QNmeaPositionInfoSource::requestUpdate_waitForValidDateTime()
{
    QFETCH(QByteArray, bytes);
    QFETCH(QList<QDateTime>, dateTimes);

    QNmeaPositionInfoSourceProxyFactory factory(m_mode);
    QNmeaPositionInfoSourceProxy *proxy = static_cast<QNmeaPositionInfoSourceProxy*>(factory.createProxy());

    QSignalSpy spy(proxy->source(), SIGNAL(positionUpdated(QGeoPositionInfo)));
    proxy->source()->requestUpdate();

    proxy->feedBytes(bytes);
    QTRY_COMPARE(spy.count(), 1);
    QCOMPARE(spy[0][0].value<QGeoPositionInfo>().dateTime(), dateTimes[0]);
}

void tst_QNmeaPositionInfoSource::requestUpdate_waitForValidDateTime_data()
{
    startUpdates_waitForValidDateTime_data();
}

void tst_QNmeaPositionInfoSource::testWithBadNmea()
{
    QFETCH(QByteArray, bytes);
    QFETCH(QList<QDateTime>, dateTimes);
    QFETCH(UpdateTriggerMethod, trigger);

    QNmeaPositionInfoSourceProxyFactory factory(m_mode);
    QNmeaPositionInfoSourceProxy *proxy = static_cast<QNmeaPositionInfoSourceProxy*>(factory.createProxy());

    QSignalSpy spy(proxy->source(), SIGNAL(positionUpdated(QGeoPositionInfo)));
    if (trigger == StartUpdatesMethod)
        proxy->source()->startUpdates();
    else
        proxy->source()->requestUpdate();

    proxy->feedBytes(bytes);
    QTRY_COMPARE(spy.count(), dateTimes.count());
    for (int i=0; i<dateTimes.count(); i++)
        QCOMPARE(spy[i][0].value<QGeoPositionInfo>().dateTime(), dateTimes[i]);
}

void tst_QNmeaPositionInfoSource::testWithBadNmea_data()
{
    QTest::addColumn<QByteArray>("bytes");
    QTest::addColumn<QList<QDateTime> >("dateTimes");
    QTest::addColumn<UpdateTriggerMethod>("trigger");

    QDateTime firstDateTime = QDateTime::currentDateTime().toUTC();
    QByteArray bad = QLocationTestUtils::createRmcSentence(firstDateTime.addSecs(1)).toLatin1();
    bad = bad.mid(bad.length()/2);
    QDateTime lastDateTime = firstDateTime.addSecs(2);

    QByteArray bytes;
    bytes += QLocationTestUtils::createRmcSentence(firstDateTime).toLatin1();
    bytes += bad;
    bytes += QLocationTestUtils::createRmcSentence(lastDateTime).toLatin1();
    QTest::newRow("requestUpdate(), bad second sentence") << bytes
            << (QList<QDateTime>() << firstDateTime) << RequestUpdatesMethod;
    QTest::newRow("startUpdates(), bad second sentence") << bytes
            << (QList<QDateTime>() << firstDateTime << lastDateTime) << StartUpdatesMethod;
}

//QTEST_MAIN(tst_QNmeaPositionInfoSource)

class tst_QNmeaPositionInfoSource_RealTime : public tst_QNmeaPositionInfoSource
{
    Q_OBJECT
public:
    tst_QNmeaPositionInfoSource_RealTime()
        : tst_QNmeaPositionInfoSource(QNmeaPositionInfoSource::RealTimeMode) {}
};

class tst_QNmeaPositionInfoSource_Simulation : public tst_QNmeaPositionInfoSource
{
    Q_OBJECT
public:
    tst_QNmeaPositionInfoSource_Simulation()
        : tst_QNmeaPositionInfoSource(QNmeaPositionInfoSource::SimulationMode) {}
};

class tst_QNmeaPositionInfoSource_RealTimeUpdates : public QGeoPositionInfoSourceSubclassTest
{
    Q_OBJECT
public:
    tst_QNmeaPositionInfoSource_RealTimeUpdates(QObject *parent = 0)
        : QGeoPositionInfoSourceSubclassTest(new QNmeaPositionInfoSourceProxyFactory(QNmeaPositionInfoSource::RealTimeMode), parent)
    {
    }

    ~tst_QNmeaPositionInfoSource_RealTimeUpdates()
    {
        delete factory();
    }
};

class tst_QNmeaPositionInfoSource_SimulationUpdates : public QGeoPositionInfoSourceSubclassTest
{
    Q_OBJECT
public:
    tst_QNmeaPositionInfoSource_SimulationUpdates(QObject *parent = 0)
        : QGeoPositionInfoSourceSubclassTest(new QNmeaPositionInfoSourceProxyFactory(QNmeaPositionInfoSource::SimulationMode), parent)
    {
    }

    ~tst_QNmeaPositionInfoSource_SimulationUpdates()
    {
        delete factory();
    }
};

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    int r;
    bool fail = false;

    tst_QNmeaPositionInfoSource_RealTime common_realTime;
    r = QTest::qExec(&common_realTime);
    if (r < 0) fail = true;

    tst_QNmeaPositionInfoSource_Simulation common_sim;
    r = QTest::qExec(&common_sim);
    if (r < 0) fail = true;

    tst_QNmeaPositionInfoSource_RealTimeUpdates updates_realTime;
    r = QTest::qExec(&updates_realTime);
    if (r < 0) fail = true;

    tst_QNmeaPositionInfoSource_SimulationUpdates updates_sim;
    r = QTest::qExec(&updates_sim);
    if (r < 0) fail = true;

    return fail ? -1 : 0;
}

#include "tst_qnmeapositioninfosource.moc"
