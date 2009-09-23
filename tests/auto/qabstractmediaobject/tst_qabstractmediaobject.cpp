/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
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
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QtTest>

#include <QtCore/qtimer.h>

#include <multimedia/qabstractmediaobject.h>
#include <multimedia/qabstractmediaservice.h>
#include <multimedia/qmetadataprovidercontrol.h>

class tst_QAbstractMediaObject : public QObject
{
    Q_OBJECT

private slots:
    void propertyWatch();
    void notifySignals_data();
    void notifySignals();
    void notifyInterval_data();
    void notifyInterval();

    void isMetaDataAvailable();
    void isWritable();
    void metaDataChanged();
    void metaData_data();
    void metaData();
    void setMetaData_data();
    void setMetaData();
    void extendedMetaData_data() { metaData_data(); }
    void extendedMetaData();
    void setExtendedMetaData_data() { extendedMetaData_data(); }
    void setExtendedMetaData();

private:
    void setupNotifyTests();
};

class QtTestMetaDataProvider : public QMetaDataProviderControl
{
    Q_OBJECT
public:
    QtTestMetaDataProvider(QObject *parent = 0)
        : QMetaDataProviderControl(parent)
        , m_available(false)
        , m_writable(false)
    {
    }

    bool isMetaDataAvailable() const { return m_available; }
    void setMetaDataAvailable(bool available) {
        emit metaDataAvailableChanged(m_available = available); }

    bool isWritable() const { return m_writable; }
    void setWritable(bool writable) { emit writableChanged(m_writable = writable); }

    QVariant metaData(QAbstractMediaObject::MetaData key) const { return m_data.value(key); }
    void setMetaData(QAbstractMediaObject::MetaData key, const QVariant &value) {
        m_data.insert(key, value); }

    QVariant extendedMetaData(const QString &key) const { return m_extendedData.value(key); }
    void setExtendedMetaData(const QString &key, const QVariant &value) {
        m_extendedData.insert(key, value); }

    using QMetaDataProviderControl::metaDataChanged;

    void populateMetaData()
    {
        m_available = true;

    }

    bool m_available;
    bool m_writable;
    QMap<QAbstractMediaObject::MetaData, QVariant> m_data;
    QMap<QString, QVariant> m_extendedData;
};

class QtTestMetaDataService : public QAbstractMediaService
{
    Q_OBJECT
public:
    QtTestMetaDataService(QObject *parent = 0)
        : QAbstractMediaService(parent)
    {
    }

    QAbstractMediaControl *control(const char *iid) const
    {
        if (qstrcmp(iid, QMetaDataProviderControl_iid) == 0)
            return const_cast<QtTestMetaDataProvider *>(&metaData);
        else
            return 0;
    }

    QtTestMetaDataProvider metaData;
};


class QtTestMediaObject : public QAbstractMediaObject
{
    Q_OBJECT
    Q_PROPERTY(int a READ a WRITE setA NOTIFY aChanged)
    Q_PROPERTY(int b READ b WRITE setB NOTIFY bChanged)
    Q_PROPERTY(int c READ c WRITE setC NOTIFY cChanged)
public:
    QtTestMediaObject(): QAbstractMediaObject(0, 0), m_a(0), m_b(0), m_c(0) {}

    bool isValid() const { return true; }

    using QAbstractMediaObject::addPropertyWatch;
    using QAbstractMediaObject::removePropertyWatch;

    int a() const { return m_a; }
    void setA(int a) { m_a = a; }

    int b() const { return m_b; }
    void setB(int b) { m_b = b; }

    int c() const { return m_c; }
    void setC(int c) { m_c = c; }

Q_SIGNALS:
    void aChanged(int a);
    void bChanged(int b);
    void cChanged(int c);

private:
    int m_a;
    int m_b;
    int m_c;
};

void tst_QAbstractMediaObject::propertyWatch()
{
    QtTestMediaObject object;
    object.setNotifyInterval(0);

    QEventLoop loop;
    QTimer timer;
    connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
    connect(&object, SIGNAL(aChanged(int)), &loop, SLOT(quit()));
    connect(&object, SIGNAL(bChanged(int)), &loop, SLOT(quit()));
    connect(&object, SIGNAL(cChanged(int)), &loop, SLOT(quit()));

    QSignalSpy aSpy(&object, SIGNAL(aChanged(int)));
    QSignalSpy bSpy(&object, SIGNAL(bChanged(int)));
    QSignalSpy cSpy(&object, SIGNAL(cChanged(int)));

    timer.start(20);
    loop.exec();
    timer.stop();

    QCOMPARE(aSpy.count(), 0);
    QCOMPARE(bSpy.count(), 0);
    QCOMPARE(cSpy.count(), 0);

    int aCount = 0;
    int bCount = 0;
    int cCount = 0;

    object.addPropertyWatch("a");

    timer.start(20);
    loop.exec();
    timer.stop();

    QVERIFY(aSpy.count() > aCount);
    QCOMPARE(bSpy.count(), 0);
    QCOMPARE(cSpy.count(), 0);
    QCOMPARE(aSpy.last().value(0).toInt(), 0);

    aCount = aSpy.count();

    object.setA(54);
    object.setB(342);
    object.setC(233);

    timer.start(20);
    loop.exec();
    timer.stop();

    QVERIFY(aSpy.count() > aCount);
    QCOMPARE(bSpy.count(), 0);
    QCOMPARE(cSpy.count(), 0);
    QCOMPARE(aSpy.last().value(0).toInt(), 54);

    aCount = aSpy.count();

    object.addPropertyWatch("b");
    object.setA(43);
    object.setB(235);
    object.setC(90);

    timer.start(20);
    loop.exec();
    timer.stop();

    QVERIFY(aSpy.count() > aCount);
    QVERIFY(bSpy.count() > bCount);
    QCOMPARE(cSpy.count(), 0);
    QCOMPARE(aSpy.last().value(0).toInt(), 43);
    QCOMPARE(bSpy.last().value(0).toInt(), 235);

    aCount = aSpy.count();
    bCount = bSpy.count();

    object.removePropertyWatch("a");
    object.addPropertyWatch("c");

    timer.start(20);
    loop.exec();
    timer.stop();

    QCOMPARE(aSpy.count(), aCount);
    QVERIFY(bSpy.count() > bCount);
    QVERIFY(cSpy.count() > cCount);
    QCOMPARE(bSpy.last().value(0).toInt(), 235);
    QCOMPARE(cSpy.last().value(0).toInt(), 90);

    bCount = bSpy.count();
    cCount = cSpy.count();

    object.setA(435);
    object.setC(9845);

    timer.start(20);
    loop.exec();
    timer.stop();

    QCOMPARE(aSpy.count(), aCount);
    QVERIFY(bSpy.count() > bCount);
    QVERIFY(cSpy.count() > cCount);
    QCOMPARE(bSpy.last().value(0).toInt(), 235);
    QCOMPARE(cSpy.last().value(0).toInt(), 9845);

    bCount = bSpy.count();
    cCount = cSpy.count();

    object.setA(8432);
    object.setB(324);
    object.setC(443);
    object.removePropertyWatch("c");

    timer.start(20);
    loop.exec();
    timer.stop();

    QCOMPARE(aSpy.count(), aCount);
    QVERIFY(bSpy.count() > bCount);
    QCOMPARE(cSpy.count(), cCount);
    QCOMPARE(bSpy.last().value(0).toInt(), 324);
    QCOMPARE(cSpy.last().value(0).toInt(), 9845);

    bCount = bSpy.count();

    object.removePropertyWatch("b");

    timer.start(20);
    loop.exec();
    timer.stop();

    QCOMPARE(aSpy.count(), aCount);
    QCOMPARE(bSpy.count(), bCount);
    QCOMPARE(cSpy.count(), cCount);
}

void tst_QAbstractMediaObject::setupNotifyTests()
{
    QTest::addColumn<int>("interval");
    QTest::addColumn<int>("count");
    QTest::addColumn<int>("wait");

    QTest::newRow("single 50ms")
            << 50
            << 1
            << 100;
    QTest::newRow("single 100ms")
            << 100
            << 1
            << 200;
    QTest::newRow("x3 50ms")
            << 50
            << 3
            << 200;
    QTest::newRow("x3 100ms")
            << 100
            << 3
            << 400;
}

void tst_QAbstractMediaObject::notifySignals_data()
{
    setupNotifyTests();
}

void tst_QAbstractMediaObject::notifySignals()
{
    QFETCH(int, interval);
    QFETCH(int, count);
    QFETCH(int, wait);

    QtTestMediaObject object;
    object.setNotifyInterval(interval);
    object.addPropertyWatch("a");

    QSignalSpy spy(&object, SIGNAL(aChanged(int)));

    QEventLoop loop;
    QTimer timer;
    connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));

    timer.start(wait);
    loop.exec();

    QVERIFY(spy.count() >= count);
    QVERIFY(spy.count() < count + 2);
}

void tst_QAbstractMediaObject::notifyInterval_data()
{
    setupNotifyTests();
}

void tst_QAbstractMediaObject::notifyInterval()
{
    QFETCH(int, interval);

    QtTestMediaObject object;
    object.setNotifyInterval(interval);

    QCOMPARE(object.notifyInterval(), interval);
}

void tst_QAbstractMediaObject::isMetaDataAvailable()
{
    QtTestMetaDataService service;
    service.metaData.setMetaDataAvailable(false);

    QtTestMediaObject object;

    QSignalSpy spy(&object, SIGNAL(metaDataAvailableChanged(bool)));

    QCOMPARE(object.isMetaDataAvailable(), false);

    service.metaData.setMetaDataAvailable(true);

    QCOMPARE(object.isMetaDataAvailable(), true);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toBool(), true);

    service.metaData.setMetaDataAvailable(false);

    QCOMPARE(object.isMetaDataAvailable(), false);
    QCOMPARE(spy.count(), 2);
    QCOMPARE(spy.at(1).at(0).toBool(), false);
}

void tst_QAbstractMediaObject::isWritable()
{
    QtTestMetaDataService service;
    service.metaData.setWritable(false);

    QtTestMediaObject object;

    QSignalSpy spy(&object, SIGNAL(metaDataWritableChanged(bool)));

    QCOMPARE(object.isMetaDataWritable(), false);

    service.metaData.setWritable(true);

    QCOMPARE(object.isMetaDataWritable(), true);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toBool(), true);

    service.metaData.setWritable(false);

    QCOMPARE(object.isMetaDataWritable(), false);
    QCOMPARE(spy.count(), 2);
    QCOMPARE(spy.at(1).at(0).toBool(), false);
}

void tst_QAbstractMediaObject::metaDataChanged()
{
    QtTestMetaDataService service;
    QtTestMediaObject object;

    QSignalSpy spy(&object, SIGNAL(metaDataChanged()));

    service.metaData.metaDataChanged();
    QCOMPARE(spy.count(), 1);

    service.metaData.metaDataChanged();
    QCOMPARE(spy.count(), 2);
}

void tst_QAbstractMediaObject::metaData_data()
{
    QTest::addColumn<QString>("artist");
    QTest::addColumn<QString>("title");
    QTest::addColumn<QString>("genre");

    QTest::newRow("")
            << QString::fromLatin1("Dead Can Dance")
            << QString::fromLatin1("Host of Seraphim")
            << QString::fromLatin1("Awesome");
}

void tst_QAbstractMediaObject::metaData()
{
    QFETCH(QString, artist);
    QFETCH(QString, title);
    QFETCH(QString, genre);

    QtTestMetaDataService service;
    service.metaData.populateMetaData();

    service.metaData.m_data.insert(QAbstractMediaObject::AlbumArtist, artist);
    service.metaData.m_data.insert(QAbstractMediaObject::Title, title);
    service.metaData.m_data.insert(QAbstractMediaObject::Genre, genre);

    QtTestMediaObject object;

    QCOMPARE(object.metaData(QAbstractMediaObject::AlbumArtist).toString(), artist);
    QCOMPARE(object.metaData(QAbstractMediaObject::Title).toString(), title);
}

void tst_QAbstractMediaObject::setMetaData_data()
{
    QTest::addColumn<QString>("title");

    QTest::newRow("")
            << QString::fromLatin1("In the Kingdom of the Blind the One eyed are Kings");
}

void tst_QAbstractMediaObject::setMetaData()
{
    QFETCH(QString, title);

    QtTestMetaDataService service;
    service.metaData.populateMetaData();

    QtTestMediaObject object;

    object.setMetaData(QAbstractMediaObject::Title, title);
    QCOMPARE(object.metaData(QAbstractMediaObject::Title).toString(), title);
    QCOMPARE(service.metaData.m_data.value(QAbstractMediaObject::Title).toString(), title);
}

void tst_QAbstractMediaObject::extendedMetaData()
{
    QFETCH(QString, artist);
    QFETCH(QString, title);
    QFETCH(QString, genre);

    QtTestMetaDataService service;
    service.metaData.m_extendedData.insert(QLatin1String("Artist"), artist);
    service.metaData.m_extendedData.insert(QLatin1String("Title"), title);
    service.metaData.m_extendedData.insert(QLatin1String("Genre"), genre);

    QtTestMediaObject object;

    QCOMPARE(object.extendedMetaData(QLatin1String("Artist")).toString(), artist);
    QCOMPARE(object.extendedMetaData(QLatin1String("Title")).toString(), title);
}

void tst_QAbstractMediaObject::setExtendedMetaData()
{
    QtTestMetaDataService service;
    service.metaData.populateMetaData();

    QtTestMediaObject object;

    QString title(QLatin1String("In the Kingdom of the Blind the One eyed are Kings"));

    object.setExtendedMetaData(QLatin1String("Title"), title);
    QCOMPARE(object.extendedMetaData(QLatin1String("Title")).toString(), title);
    QCOMPARE(service.metaData.m_extendedData.value(QLatin1String("Title")).toString(), title);
}

QTEST_MAIN(tst_QAbstractMediaObject)

#include "tst_qabstractmediaobject.moc"
