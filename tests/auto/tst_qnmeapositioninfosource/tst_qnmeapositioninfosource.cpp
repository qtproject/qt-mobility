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
#include "../qgeopositioninfosourcesubclasstest_p.h"
#include "../qlocationtestutils_p.h"

#include <qnmeapositioninfosource.h>

#include <QTest>
#include <QDebug>
#include <QMetaType>
#include <QTcpServer>
#include <QTcpSocket>
#include <QBuffer>
#include <QSignalSpy>
#include <QFile>
#include <QTemporaryFile>

Q_DECLARE_METATYPE(QNmeaPositionInfoSource::UpdateMode)
Q_DECLARE_METATYPE(QGeoPositionInfo)
Q_DECLARE_METATYPE(QList<QDateTime>)

static QString tst_qnmeapositioninfosource_createRmcSentence(const QDateTime &dt)
{
    QString time = dt.toString("hhmmss.zzz");
    QString date = dt.toString("ddMMyy");
    QString nmea = QString("$GPRMC,%1,A,2730.83609,S,15301.87844,E,0.7,9.0,%2,11.2,W,A*")
        .arg(time).arg(date);
    return QLocationTestUtils::addNmeaChecksumAndBreaks(nmea);
}

static QString tst_qnmeapositioninfosource_createGgaSentence(const QTime &time)
{
    QString nmea = QString("$GPGGA,%1,2734.76859,S,15305.99361,E,1,04,3.5,49.4,M,39.2,M,,*")
            .arg(time.toString("hhmmss.zzz"));
    return QLocationTestUtils::addNmeaChecksumAndBreaks(nmea);
}

static QString tst_qnmeapositioninfosource_createZdaSentence(const QDateTime &dt)
{
    QString time = dt.toString("hhmmss.zzz");
    QString nmea = QString("$GPZDA,%1,%2,%3,%4,,*")
        .arg(time).arg(dt.toString("dd")).arg(dt.toString("MM")).arg(dt.toString("yyyy"));
    return QLocationTestUtils::addNmeaChecksumAndBreaks(nmea);
}


//-------------------------------------------------------

class QNmeaPositionInfoSourceProxy : public QGeoPositionInfoSourceProxy
{
public:
    QNmeaPositionInfoSourceProxy(QNmeaPositionInfoSource *source, QIODevice *outDevice)
        : m_source(source),
          m_outDevice(outDevice)
    {
    }

    ~QNmeaPositionInfoSourceProxy()
    {
        m_source->device()->close();
        m_outDevice->close();
        delete m_source;
        delete m_outDevice;
    }

    QGeoPositionInfoSource *source() const
    {
        return m_source;
    }

    void feedUpdate(const QDateTime &dt)
    {
        m_outDevice->write(tst_qnmeapositioninfosource_createRmcSentence(dt).toAscii());
    }

    void feedBytes(const QByteArray &bytes)
    {
        m_outDevice->write(bytes);
    }

private:
    QNmeaPositionInfoSource *m_source;
    QIODevice *m_outDevice;
};


class QNmeaPositionInfoSourceProxyFactory : public QObject, public QGeoPositionInfoSourceProxyFactory
{
    Q_OBJECT
public:
    QNmeaPositionInfoSourceProxyFactory(QNmeaPositionInfoSource::UpdateMode mode)
        : m_server(new QTcpServer(this)),
          m_mode(mode)
    {
        m_server->listen();
    }

    QGeoPositionInfoSourceProxy *createProxy()
    {
        QTcpSocket *client = new QTcpSocket;
        client->connectToHost(m_server->serverAddress(), m_server->serverPort());
        bool b = m_server->waitForNewConnection();
        Q_ASSERT(b);
        b = client->waitForConnected();
        Q_ASSERT(b);

        QNmeaPositionInfoSource *source = new QNmeaPositionInfoSource(m_mode);
        source->setDevice(m_server->nextPendingConnection());
        Q_ASSERT(source->device() != 0);
        return new QNmeaPositionInfoSourceProxy(source, client);
    }

private:
    QTcpServer *m_server;
    QNmeaPositionInfoSource::UpdateMode m_mode;
};

//-------------------------------------------------------

class tst_QNmeaPositionInfoSource : public QObject
{
    Q_OBJECT

public:
    enum UpdateTriggerMethod
    {
        StartUpdatesMethod,
        RequestUpdatesMethod
    };

    tst_QNmeaPositionInfoSource(QNmeaPositionInfoSource::UpdateMode mode, QObject *parent = 0)
        : QObject(parent)
    {
        m_mode = mode;
    }

private:
    QNmeaPositionInfoSource::UpdateMode m_mode;

private slots:
    void initTestCase()
    {
        qRegisterMetaType<QGeoPositionInfo>();
    }

    void constructor()
    {
        QObject o;
        QNmeaPositionInfoSource source(m_mode, &o);
        QCOMPARE(source.updateMode(), m_mode);
        QCOMPARE(source.parent(), &o);
    }

    void startUpdates_withBufferedData()
    {
        // In SimulationMode, data stored in the QIODevice is read when
        // startUpdates() or requestUpdate() is called.
        // In RealTimeMode, all existing data in the QIODevice is ignored -
        // only new data will be read.

        QFETCH(QList<QDateTime>, dateTimes);
        QFETCH(UpdateTriggerMethod, trigger);

        QByteArray bytes;
        for (int i=0; i<dateTimes.count(); i++)
            bytes += tst_qnmeapositioninfosource_createRmcSentence(dateTimes[i]).toLatin1();
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

    void testWithBufferedData_data()
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

    void startUpdates_waitForValidDateTime()
    {
        // Tests that the class does not emit an update until it receives a
        // sentences with a valid date *and* time. All sentences before this
        // should be ignored, and any sentences received after this that do
        // not have a date should use the known date.

        QFETCH(QByteArray, bytes);
        QFETCH(QList<QDateTime>, dateTimes);

        QNmeaPositionInfoSourceProxyFactory factory(QNmeaPositionInfoSource::SimulationMode);
        QNmeaPositionInfoSourceProxy *proxy = static_cast<QNmeaPositionInfoSourceProxy*>(factory.createProxy());

        QSignalSpy spy(proxy->source(), SIGNAL(positionUpdated(QGeoPositionInfo)));
        proxy->source()->startUpdates();

        proxy->feedBytes(bytes);
        QTRY_COMPARE(spy.count(), dateTimes.count());

        for (int i=0; i<spy.count(); i++)
            QCOMPARE(spy[i][0].value<QGeoPositionInfo>().dateTime(), dateTimes[i]);
    }

    void startUpdates_waitForValidDateTime_data()
    {
        QTest::addColumn<QByteArray>("bytes");
        QTest::addColumn<QList<QDateTime> >("dateTimes");

        QDateTime dt = QDateTime::currentDateTime().toUTC();
        QByteArray bytes;

        // should only receive RMC sentence and the GGA sentence *after* it
        bytes += tst_qnmeapositioninfosource_createGgaSentence(dt.addSecs(1).time()).toLatin1();
        bytes += tst_qnmeapositioninfosource_createRmcSentence(dt.addSecs(2)).toLatin1();
        bytes += tst_qnmeapositioninfosource_createGgaSentence(dt.addSecs(3).time()).toLatin1();
        QTest::newRow("Feed GGA,RMC,GGA; expect RMC, second GGA only")
                << bytes << (QList<QDateTime>() << dt.addSecs(2) << dt.addSecs(3));

        // should not receive ZDA (has no coordinates) but should get the GGA
        // sentence after it since it got the date/time from ZDA
        bytes.clear();
        bytes += tst_qnmeapositioninfosource_createGgaSentence(dt.addSecs(1).time()).toLatin1();
        bytes += tst_qnmeapositioninfosource_createZdaSentence(dt.addSecs(2)).toLatin1();
        bytes += tst_qnmeapositioninfosource_createGgaSentence(dt.addSecs(3).time()).toLatin1();
        QTest::newRow("Feed GGA,ZDA,GGA; expect second GGA only")
                << bytes << (QList<QDateTime>() << dt.addSecs(3));

        // should ignore sentence with a date/time before the known date/time
        bytes.clear();
        bytes += tst_qnmeapositioninfosource_createRmcSentence(dt.addSecs(1)).toLatin1();
        bytes += tst_qnmeapositioninfosource_createRmcSentence(dt.addSecs(-2)).toLatin1();
        bytes += tst_qnmeapositioninfosource_createRmcSentence(dt.addSecs(2)).toLatin1();
        QTest::newRow("Feed good RMC, RMC with bad date/time, good RMC; expect first and third RMC only")
                << bytes << (QList<QDateTime>() << dt.addSecs(1) << dt.addSecs(2));
    }

    void requestUpdate_waitForValidDateTime()
    {
        QFETCH(QByteArray, bytes);
        QFETCH(QList<QDateTime>, dateTimes);

        QNmeaPositionInfoSourceProxyFactory factory(QNmeaPositionInfoSource::SimulationMode);
        QNmeaPositionInfoSourceProxy *proxy = static_cast<QNmeaPositionInfoSourceProxy*>(factory.createProxy());

        QSignalSpy spy(proxy->source(), SIGNAL(positionUpdated(QGeoPositionInfo)));
        proxy->source()->requestUpdate();

        proxy->feedBytes(bytes);
        QTRY_COMPARE(spy.count(), 1);
        QCOMPARE(spy[0][0].value<QGeoPositionInfo>().dateTime(), dateTimes[0]);
    }

    void requestUpdate_waitForValidDateTime_data()
    {
        startUpdates_waitForValidDateTime_data();
    }

    void testWithBadNmea()
    {
        QFETCH(QByteArray, bytes);
        QFETCH(QList<QDateTime>, dateTimes);
        QFETCH(UpdateTriggerMethod, trigger);

        QNmeaPositionInfoSourceProxyFactory factory(QNmeaPositionInfoSource::SimulationMode);
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

    void testWithBadNmea_data()
    {
        QTest::addColumn<QByteArray>("bytes");
        QTest::addColumn<QList<QDateTime> >("dateTimes");
        QTest::addColumn<UpdateTriggerMethod>("trigger");

        QDateTime firstDateTime = QDateTime::currentDateTime().toUTC();
        QByteArray bad = tst_qnmeapositioninfosource_createRmcSentence(firstDateTime.addSecs(1)).toLatin1();
        bad = bad.mid(bad.length()/2);
        QDateTime lastDateTime = firstDateTime.addSecs(2);

        QByteArray bytes;
        bytes += tst_qnmeapositioninfosource_createRmcSentence(firstDateTime).toLatin1();
        bytes += bad;
        bytes += tst_qnmeapositioninfosource_createRmcSentence(lastDateTime).toLatin1();
        QTest::newRow("requestUpdate(), bad second sentence") << bytes
                << (QList<QDateTime>() << firstDateTime) << RequestUpdatesMethod;
        QTest::newRow("startUpdates(), bad second sentence") << bytes
                << (QList<QDateTime>() << firstDateTime << lastDateTime) << StartUpdatesMethod;
    }
};

Q_DECLARE_METATYPE(tst_QNmeaPositionInfoSource::UpdateTriggerMethod)

//------------------------------------------------

// Making subclasses of tst_QNmeaPositionInfoSource and QGeoPositionInfoSourceSubclassTest
// based on real-time vs simulation mode (rather than just creating instances with
// different arguments) helps to get more informative output from QTest::qExec().
// Otherwise the output makes it look like it is running the same test twice,
// rather than the same test with different parameters.

class tst_QNmeaPositionInfoSource_RealTimeMode : public tst_QNmeaPositionInfoSource
{
    Q_OBJECT
public:
    tst_QNmeaPositionInfoSource_RealTimeMode(QObject *parent = 0)
        : tst_QNmeaPositionInfoSource(QNmeaPositionInfoSource::RealTimeMode, parent)
    {
    }
};

class tst_QNmeaPositionInfoSource_SimulationMode : public tst_QNmeaPositionInfoSource
{
    Q_OBJECT

public:
    tst_QNmeaPositionInfoSource_SimulationMode(QObject *parent = 0)
        : tst_QNmeaPositionInfoSource(QNmeaPositionInfoSource::SimulationMode, parent)
    {
    }
};

class tst_QNmeaPositionInfoSource_SubclassTest_RealTimeMode : public QGeoPositionInfoSourceSubclassTest
{
    Q_OBJECT
public:
    tst_QNmeaPositionInfoSource_SubclassTest_RealTimeMode(QObject *parent = 0)
        : QGeoPositionInfoSourceSubclassTest(new QNmeaPositionInfoSourceProxyFactory(QNmeaPositionInfoSource::RealTimeMode))
    {
    }

    ~tst_QNmeaPositionInfoSource_SubclassTest_RealTimeMode()
    {
        delete factory();
    }
};

class tst_QNmeaPositionInfoSource_SubclassTest_SimulationMode : public QGeoPositionInfoSourceSubclassTest
{
    Q_OBJECT
public:
    tst_QNmeaPositionInfoSource_SubclassTest_SimulationMode(QObject *parent = 0)
        : QGeoPositionInfoSourceSubclassTest(new QNmeaPositionInfoSourceProxyFactory(QNmeaPositionInfoSource::SimulationMode))
    {
    }

    ~tst_QNmeaPositionInfoSource_SubclassTest_SimulationMode()
    {
        delete factory();
    }
};


//------------------------------------------------


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    int r;

    tst_QNmeaPositionInfoSource_RealTimeMode realTime;
    r = QTest::qExec(&realTime, argc, argv);
    if (r < 0)
        return r;

    tst_QNmeaPositionInfoSource_SimulationMode sim;
    r = QTest::qExec(&sim, argc, argv);
    if (r < 0)
        return r;

    tst_QNmeaPositionInfoSource_SubclassTest_RealTimeMode realTimeSubclassTest;
    r = QTest::qExec(&realTimeSubclassTest, argc, argv);
    if (r < 0)
        return r;

    tst_QNmeaPositionInfoSource_SubclassTest_SimulationMode simSubclassTest;
    r = QTest::qExec(&simSubclassTest, argc, argv);
    if (r < 0)
        return r;

    return 0;
}

//QTEST_MAIN(tst_QNmeaPositionInfoSource)
#include "tst_qnmeapositioninfosource.moc"
