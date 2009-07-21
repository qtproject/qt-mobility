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
#include "qgeopositioninfosourcesubclasstest_p.h"
#include "qlocationtestutils_p.h"

#include <QTest>
#include <QSignalSpy>
#include <QMetaType>
#include <QDebug>

Q_DECLARE_METATYPE(QGeoPositionInfo)
Q_DECLARE_METATYPE(QList<int>)
Q_DECLARE_METATYPE(QList<QDateTime>)

class QUpdateIntervalLogger : public QObject
{
    Q_OBJECT
public:
    QUpdateIntervalLogger(QGeoPositionInfoSource *source, QObject *parent = 0)
        : QObject(parent),
          m_source(source)
    {
    }

    ~QUpdateIntervalLogger() {}

    int updateCount() const { return m_updateCount; }

    void compareIntervals(int expectedInterval, int errorMarginMsec, bool compareFirstInterval = true)
    {
        QList<int> intervals = listUpdateIntervals();

        for (int i=0; i<intervals.count(); i++) {
            if (i==0 && !compareFirstInterval)
                continue;
            //qDebug() << "Actual vs expected time:"
            //        << qAbs(expectedInterval - intervals[i]) << "ms";
            QString debug;
            QTextStream stream(&debug);
            stream << intervals[i] << " not within " << errorMarginMsec << "ms"
                    << " of " <<  expectedInterval;
            QVERIFY2( (intervals[i] >= expectedInterval - errorMarginMsec)
                    && (intervals[i] <= expectedInterval + errorMarginMsec),
                    debug.toLatin1().constData());
        }
    }

public slots:
    void startLogging()
    {
        connect(m_source, SIGNAL(positionUpdated(QGeoPositionInfo)),
                SLOT(positionUpdated(QGeoPositionInfo)));

        // insert starting timestamp to measure first interval
        m_updateCount = 0;
        m_timestamps << QDateTime::currentDateTime();
    }

private slots:
    void positionUpdated(const QGeoPositionInfo &update)
    {
        Q_UNUSED(update);
        m_timestamps << QDateTime::currentDateTime();
        m_updateCount++;
    }

private:
    QList<int> listUpdateIntervals() const
    {
        QList<int> intervals;
        for (int i=0; i<m_timestamps.count(); i++) {
            if (i+1 == m_timestamps.count())
                break;
            intervals << m_timestamps[i].time().msecsTo(m_timestamps[i+1].time());
        }
        return intervals;
    }

    QGeoPositionInfoSource *m_source;
    QList<QDateTime> m_timestamps;
    int m_updateCount;
    QTime m_last;
};



QGeoPositionInfoSourceSubclassTest::QGeoPositionInfoSourceSubclassTest(QGeoPositionInfoSourceProxyFactory *factory, QObject *parent)
    : QObject(parent),
      m_factory(factory),
      m_helper(0),
      m_source(0)
{
}

QList<QDateTime> QGeoPositionInfoSourceSubclassTest::createDateTimes(int count) const
{
    QList<QDateTime> dateTimes;
    QDateTime dt = QDateTime::currentDateTime().toUTC();
    int interval = 100;
    for (int i=0; i<count; i++) {
        dateTimes << dt.addMSecs(interval);
        interval += 100;
    }
    return dateTimes;
}

void QGeoPositionInfoSourceSubclassTest::initTestCase()
{
    qRegisterMetaType<QGeoPositionInfo>();
}

void QGeoPositionInfoSourceSubclassTest::init()
{
    m_helper = m_factory->createProxy();
    m_source = m_helper->source();
}

void QGeoPositionInfoSourceSubclassTest::cleanup()
{
    delete m_helper;
    m_source = 0;
}

void QGeoPositionInfoSourceSubclassTest::setUpdateInterval()
{
    QFETCH(int, interval);
    QFETCH(bool, setIntervalBeforeStart);
    QFETCH(QList<QDateTime>, dateTimes);

    QUpdateIntervalLogger *logger = new QUpdateIntervalLogger(m_source);

    // Should not make a difference whether setUpdateInterval() is called
    // before startUpdates() or after it.
    if (setIntervalBeforeStart)
        m_source->setUpdateInterval(interval);
    m_source->startUpdates();
    if (!setIntervalBeforeStart)
        m_source->setUpdateInterval(interval);

    logger->startLogging();

    QTest::qWait(m_source->updateInterval()/2);   // delay first update to ensure it is received
    for (int i=0; i<dateTimes.count(); i++) {
        m_helper->feedUpdate(dateTimes[i]);
        QTest::qWait(m_source->updateInterval());
    }

    QTRY_COMPARE(logger->updateCount(), dateTimes.count());
    logger->compareIntervals(m_source->updateInterval(), m_helper->updateIntervalErrorMargin());
}

void QGeoPositionInfoSourceSubclassTest::setUpdateInterval_data()
{
    QTest::addColumn<int>("interval");
    QTest::addColumn<bool>("setIntervalBeforeStart");
    QTest::addColumn< QList<QDateTime> >("dateTimes");

    QList<QDateTime> dateTimes;
    QList<int> testIntervals;

    dateTimes = createDateTimes(3);
    QTest::newRow("300ms, regular test intervals, set before start") << 300 << true << dateTimes;
    QTest::newRow("300ms, regular test intervals, set after start") << 300 << false << dateTimes;
}

void QGeoPositionInfoSourceSubclassTest::setUpdateInterval_delayedUpdate()
{
    // If an update interval is set, and an update is not available at a
    // particular interval, the source should emit the next update as soon
    // as it becomes available

    QSignalSpy spyUpdate(m_source, SIGNAL(positionUpdated(QGeoPositionInfo)));
    m_source->setUpdateInterval(500);
    m_source->startUpdates();

    QTest::qWait(600);
    QDateTime now = QDateTime::currentDateTime();
    m_helper->feedUpdate(now);
    QTRY_COMPARE(spyUpdate.count(), 1);

    // should have gotten the update immediately, and not have needed to
    // wait until the next interval
    QVERIFY(now.time().msecsTo(QDateTime::currentDateTime().time()) < 200);
}

void QGeoPositionInfoSourceSubclassTest::setUpdateInterval_expectLatestUpdateOnly()
{
    // If multiple updates are in the buffer at a particular update interval,
    // only the latest update should be emitted

    QSignalSpy spyUpdate(m_source, SIGNAL(positionUpdated(QGeoPositionInfo)));
    m_source->setUpdateInterval(500);
    m_source->startUpdates();

    QList<QDateTime> dateTimes = createDateTimes(3);
    for (int i=0; i<dateTimes.count(); i++)
        m_helper->feedUpdate(dateTimes[i]);

    QTRY_COMPARE(spyUpdate.count(), 1);
    QCOMPARE(spyUpdate[0][0].value<QGeoPositionInfo>().dateTime(), dateTimes.last());
}

void QGeoPositionInfoSourceSubclassTest::lastKnownPosition()
{
    QCOMPARE(m_source->lastKnownPosition(), QGeoPositionInfo());

    // source may need requestUpdate() or startUpdates() to be called to
    // trigger reading of data channel
    QSignalSpy spyTimeout(m_source, SIGNAL(requestTimeout()));
    m_source->requestUpdate(0);
    QTRY_COMPARE(spyTimeout.count(), 1);

    // If an update is received and startUpdates() or requestUpdate() hasn't
    // been called, it should still be available through lastKnownPosition()
    QDateTime dt = QDateTime::currentDateTime().toUTC();
    m_helper->feedUpdate(dt);
    QTRY_COMPARE(m_source->lastKnownPosition().dateTime(), dt);

    QList<QDateTime> dateTimes = createDateTimes(5);
    for (int i=0; i<dateTimes.count(); i++) {
        m_source->requestUpdate();
        m_helper->feedUpdate(dateTimes[i]);
        QTRY_COMPARE(m_source->lastKnownPosition().dateTime(), dateTimes[i]);
    }

    m_source->startUpdates();
    dateTimes = createDateTimes(5);
    for (int i=0; i<dateTimes.count(); i++) {
        m_helper->feedUpdate(dateTimes[i]);
        QTRY_COMPARE(m_source->lastKnownPosition().dateTime(), dateTimes[i]);
    }

    m_source->stopUpdates();
    m_source->setUpdateInterval(3000);
    m_source->startUpdates();
    dateTimes = createDateTimes(5);
    for (int i=0; i<dateTimes.count(); i++) {
        m_helper->feedUpdate(dateTimes[i]);
        QTRY_COMPARE(m_source->lastKnownPosition().dateTime(), dateTimes[i]);
    }

    // calling start, stop, request shouldn't affect the last known position
    m_source->startUpdates();
    m_source->stopUpdates();
    m_source->requestUpdate(100);

    QCOMPARE(m_source->lastKnownPosition().dateTime(), dateTimes.last());
}

void QGeoPositionInfoSourceSubclassTest::requestUpdate()
{
    QSignalSpy spyUpdate(m_source, SIGNAL(positionUpdated(QGeoPositionInfo)));
    QSignalSpy spyTimeout(m_source, SIGNAL(requestTimeout()));
    QDateTime dt;

    m_source->requestUpdate(100);
    QTRY_COMPARE(spyTimeout.count(), 1);
    spyTimeout.clear();

    dt = QDateTime::currentDateTime().toUTC();
    m_helper->feedUpdate(dt);
    m_source->requestUpdate();
    QTRY_COMPARE(spyUpdate.count(), 1);
    QCOMPARE(spyUpdate[0][0].value<QGeoPositionInfo>().dateTime(), dt);
    QCOMPARE(spyTimeout.count(), 0);
    spyUpdate.clear();

    // delay the update and expect it to be emitted after 300ms
    dt = QDateTime::currentDateTime().toUTC();
    m_source->requestUpdate(1000);
    QTest::qWait(300);
    m_helper->feedUpdate(dt);
    QTRY_COMPARE(spyUpdate.count(), 1);
    QCOMPARE(spyUpdate[0][0].value<QGeoPositionInfo>().dateTime(), dt);
    QCOMPARE(spyTimeout.count(), 0);
    spyUpdate.clear();

    // delay the update and expect requestTimeout() to be emitted
    dt = QDateTime::currentDateTime().toUTC();
    m_source->requestUpdate(500);
    QTest::qWait(1000);
    m_helper->feedUpdate(dt);
    QCOMPARE(spyTimeout.count(), 1);
    QCOMPARE(spyUpdate.count(), 0);
    spyUpdate.clear();
}

void QGeoPositionInfoSourceSubclassTest::requestUpdate_after_start()
{
    QSignalSpy spyUpdate(m_source, SIGNAL(positionUpdated(QGeoPositionInfo)));
    QSignalSpy spyTimeout(m_source, SIGNAL(requestTimeout()));

    // Start updates with 500ms interval and requestUpdate() with 100ms
    // timeout. Feed an update, and it should be emitted immediately due to
    // the requestUpdate(). The update should not be emitted again after that
    // (i.e. the startUpdates() interval should not cause it to be re-emitted).

    QDateTime dt = QDateTime::currentDateTime().toUTC();
    m_source->setUpdateInterval(500);
    m_source->startUpdates();
    m_source->requestUpdate(100);
    m_helper->feedUpdate(dt);
    QTRY_COMPARE(spyUpdate.count(), 1);
    QCOMPARE(spyUpdate[0][0].value<QGeoPositionInfo>().dateTime(), dt);
    QCOMPARE(spyTimeout.count(), 0);
    spyUpdate.clear();

    // Update has been emitted for requestUpdate(), shouldn't be emitted for startUpdates()
    QTest::qWait(1000);
    QCOMPARE(spyUpdate.count(), 0);
}

void QGeoPositionInfoSourceSubclassTest::startUpdates()
{
    QFETCH(QList<QDateTime>, dateTimes);

    QSignalSpy spyUpdate(m_source, SIGNAL(positionUpdated(QGeoPositionInfo)));
    m_source->startUpdates();

    for (int i=0; i<dateTimes.count(); i++)
        m_helper->feedUpdate(dateTimes[i]);
    QTRY_COMPARE(spyUpdate.count(), dateTimes.count());
}

void QGeoPositionInfoSourceSubclassTest::startUpdates_data()
{
    QTest::addColumn<QList<QDateTime> >("dateTimes");

    QTest::newRow("1 update") << createDateTimes(1);
    QTest::newRow("2 updates") << createDateTimes(2);
    QTest::newRow("10 updates") << createDateTimes(10);
}

void QGeoPositionInfoSourceSubclassTest::startUpdates_startNotCalled()
{
    QSignalSpy spyUpdate(m_source, SIGNAL(positionUpdated(QGeoPositionInfo)));
    m_helper->feedUpdate(QDateTime::currentDateTime().toUTC());

    // signal not emitted if start() not called
    QTest::qWait(1000);
    QCOMPARE(spyUpdate.count(), 0);
}

void QGeoPositionInfoSourceSubclassTest::stopUpdates()
{
    QSignalSpy spyUpdate(m_source, SIGNAL(positionUpdated(QGeoPositionInfo)));

    m_source->startUpdates();

    QList<QDateTime> dateTimes = createDateTimes(5);
    for (int i=0; i<dateTimes.count(); i++)
        m_helper->feedUpdate(dateTimes[i]);
    QTRY_COMPARE(spyUpdate.count(), dateTimes.count());

    spyUpdate.clear();
    m_source->stopUpdates();
    for (int i=0; i<dateTimes.count(); i++)
        m_helper->feedUpdate(dateTimes[i]);
    // Shouldn't get updates after stop()
    QTest::qWait(1000);
    QCOMPARE(spyUpdate.count(), 0);
}

void QGeoPositionInfoSourceSubclassTest::stopUpdates_restart()
{
    QSignalSpy spyUpdate(m_source, SIGNAL(positionUpdated(QGeoPositionInfo)));

    m_source->startUpdates();
    m_source->stopUpdates();

    m_source->startUpdates();
    QList<QDateTime> dateTimes = createDateTimes(5);
    for (int i=0; i<dateTimes.count(); i++)
        m_helper->feedUpdate(dateTimes[i]);
    QTRY_COMPARE(spyUpdate.count(), dateTimes.count());
}


#include "qgeopositioninfosourcesubclasstest.moc"
