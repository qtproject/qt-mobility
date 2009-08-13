/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtMultimedia module of the Qt Toolkit.
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
** Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QtTest>

#include <QtCore/qtimer.h>

#include "multimedia/qabstractmediaobject.h"

class tst_QAbstractMediaObject : public QObject
{
    Q_OBJECT
private slots:
    void propertyWatch();
    void notifyInterval_data();
    void notifyInterval();
};

class QtTestMediaObject : public QAbstractMediaObject
{
    Q_OBJECT
    Q_PROPERTY(int a READ a WRITE setA NOTIFY aChanged)
    Q_PROPERTY(int b READ b WRITE setB NOTIFY bChanged)
    Q_PROPERTY(int c READ c WRITE setC NOTIFY cChanged)
public:
    QtTestMediaObject() : m_a(0), m_b(0), m_c(0) {}

    QAbstractMediaService* service() const { return 0; }
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

    QSignalSpy aSpy(&object, SIGNAL(aChanged(int)));
    QSignalSpy bSpy(&object, SIGNAL(bChanged(int)));
    QSignalSpy cSpy(&object, SIGNAL(cChanged(int)));

    QCoreApplication::processEvents(QEventLoop::AllEvents, 1);

    QCOMPARE(aSpy.count(), 0);
    QCOMPARE(bSpy.count(), 0);
    QCOMPARE(cSpy.count(), 0);

    int aCount = 0;
    int bCount = 0;
    int cCount = 0;

    object.addPropertyWatch("a");
    QCoreApplication::processEvents(QEventLoop::AllEvents, 1);

    QVERIFY(aSpy.count() > aCount);
    QCOMPARE(bSpy.count(), 0);
    QCOMPARE(cSpy.count(), 0);
    QCOMPARE(aSpy.last().value(0).toInt(), 0);

    aCount = aSpy.count();

    object.setA(54);
    object.setB(342);
    object.setC(233);
    QCoreApplication::processEvents(QEventLoop::AllEvents, 1);

    QVERIFY(aSpy.count() > aCount);
    QCOMPARE(bSpy.count(), 0);
    QCOMPARE(cSpy.count(), 0);
    QCOMPARE(aSpy.last().value(0).toInt(), 54);

    aCount = aSpy.count();

    object.addPropertyWatch("b");
    object.setA(43);
    object.setB(235);
    object.setC(90);
    QCoreApplication::processEvents(QEventLoop::AllEvents, 1);

    QVERIFY(aSpy.count() > aCount);
    QVERIFY(bSpy.count() > bCount);
    QCOMPARE(cSpy.count(), 0);
    QCOMPARE(aSpy.last().value(0).toInt(), 43);
    QCOMPARE(bSpy.last().value(0).toInt(), 235);

    aCount = aSpy.count();
    bCount = bSpy.count();

    object.removePropertyWatch("a");
    object.addPropertyWatch("c");
    QCoreApplication::processEvents(QEventLoop::AllEvents, 1);

    QCOMPARE(aSpy.count(), aCount);
    QVERIFY(bSpy.count() > bCount);
    QVERIFY(cSpy.count() > cCount);
    QCOMPARE(bSpy.last().value(0).toInt(), 235);
    QCOMPARE(cSpy.last().value(0).toInt(), 90);

    bCount = bSpy.count();
    cCount = cSpy.count();

    object.setA(435);
    object.setC(9845);
    QCoreApplication::processEvents(QEventLoop::AllEvents, 1);

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
    QCoreApplication::processEvents(QEventLoop::AllEvents, 1);

    QCOMPARE(aSpy.count(), aCount);
    QVERIFY(bSpy.count() > bCount);
    QCOMPARE(cSpy.count(), cCount);
    QCOMPARE(bSpy.last().value(0).toInt(), 324);
    QCOMPARE(cSpy.last().value(0).toInt(), 9845);

    bCount = bSpy.count();

    object.removePropertyWatch("b");
    QCoreApplication::processEvents(QEventLoop::AllEvents, 1);

    QCOMPARE(aSpy.count(), aCount);
    QCOMPARE(bSpy.count(), bCount);
    QCOMPARE(cSpy.count(), cCount);
}

void tst_QAbstractMediaObject::notifyInterval_data()
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

void tst_QAbstractMediaObject::notifyInterval()
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

QTEST_MAIN(tst_QAbstractMediaObject)

#include "tst_qabstractmediaobject.moc"
