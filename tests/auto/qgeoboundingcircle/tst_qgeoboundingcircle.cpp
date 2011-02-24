/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <QtTest/QtTest>
#include <qgeoboundingcircle.h>
#include <qgeocoordinate.h>

QTM_USE_NAMESPACE

Q_DECLARE_METATYPE(QGeoCoordinate)

class tst_QGeoBoundingCircle : public QObject
{
    Q_OBJECT

private slots:
    void defaultConstructor();
    void centerRadiusConstructor();

    void comparison();
    void type();

    void radius();
    void center();

    void translate_data();
    void translate();

    void valid_data();
    void valid();

    void empty_data();
    void empty();

    void contains_data();
    void contains();
};

void tst_QGeoBoundingCircle::defaultConstructor()
{
    QGeoBoundingCircle c;
    QVERIFY(!c.center().isValid());
    QCOMPARE(c.radius(), -1.0);
}

void tst_QGeoBoundingCircle::centerRadiusConstructor()
{
    QGeoBoundingCircle c(QGeoCoordinate(1,1), 50);
    QCOMPARE(c.center(), QGeoCoordinate(1,1));
    QCOMPARE(c.radius(), 50.0);
}

void tst_QGeoBoundingCircle::comparison()
{
    QGeoBoundingCircle c1(QGeoCoordinate(1,1), 50);
    QGeoBoundingCircle c2(QGeoCoordinate(1,1), 50);
    QGeoBoundingCircle c3(QGeoCoordinate(1,1), 35);
    QGeoBoundingCircle c4(QGeoCoordinate(1,2), 50);

    QVERIFY(c1 == c2);
    QVERIFY(c1 != c3);
    QVERIFY(c1 != c4);
    QVERIFY(c2 != c3);
}

void tst_QGeoBoundingCircle::type()
{
    QGeoBoundingCircle c;
    QCOMPARE(c.type(), QGeoBoundingArea::CircleType);
}

void tst_QGeoBoundingCircle::radius()
{
    QGeoBoundingCircle c;
    c.setRadius(1.0);
    QCOMPARE(c.radius(), 1.0);
    c.setRadius(5.0);
    QCOMPARE(c.radius(), 5.0);
}

void tst_QGeoBoundingCircle::center()
{
    QGeoBoundingCircle c;
    c.setCenter(QGeoCoordinate(1,1));
    QCOMPARE(c.center(), QGeoCoordinate(1,1));
    c.setCenter(QGeoCoordinate(5,10));
    QCOMPARE(c.center(), QGeoCoordinate(5,10));
}

void tst_QGeoBoundingCircle::translate_data()
{
    QTest::addColumn<QGeoCoordinate>("center");
    QTest::addColumn<qreal>("radius");
    QTest::addColumn<qreal>("lat");
    QTest::addColumn<qreal>("lon");
    QTest::addColumn<QGeoCoordinate>("newCenter");

    QTest::newRow("from 0,0") << QGeoCoordinate(0,0) << 10.0 <<
                                 5.0 << 5.0 << QGeoCoordinate(5.0, 5.0);
    QTest::newRow("across 0,0") << QGeoCoordinate(-2, -2) << 20.0 <<
                                   5.0 << 5.0 << QGeoCoordinate(3.0, 3.0);
    QTest::newRow("backwards across 0,0") << QGeoCoordinate(5,5) << 50.0
                                              << -13.0 << 5.0
                                              << QGeoCoordinate(-8.0, 10.0);
}

void tst_QGeoBoundingCircle::translate()
{
    QFETCH(QGeoCoordinate, center);
    QFETCH(qreal, radius);
    QFETCH(qreal, lat);
    QFETCH(qreal, lon);
    QFETCH(QGeoCoordinate, newCenter);

    QGeoBoundingCircle c(center, radius);
    QGeoBoundingCircle d = c;

    c.translate(lat, lon);

    QCOMPARE(c.radius(), radius);
    QCOMPARE(c.center(), newCenter);

    c = d.translated(lat, lon);
    d.setRadius(1.0);

    QCOMPARE(c.radius(), radius);
    QCOMPARE(d.center(), center);
    QCOMPARE(c.center(), newCenter);
}

void tst_QGeoBoundingCircle::valid_data()
{
    QTest::addColumn<QGeoCoordinate>("center");
    QTest::addColumn<qreal>("radius");
    QTest::addColumn<bool>("valid");

    QTest::newRow("default") << QGeoCoordinate() << -1.0 << false;
    QTest::newRow("empty coord") << QGeoCoordinate() << 5.0 << false;
    QTest::newRow("NaN coord") << QGeoCoordinate(500, 500) << 5.0 << false;
    QTest::newRow("bad radius") << QGeoCoordinate(10, 10) << -5.0 << false;
    QTest::newRow("NaN radius") << QGeoCoordinate(10, 10) << qQNaN() << false;
    QTest::newRow("zero radius") << QGeoCoordinate(10, 10) << 0.0 << true;
    QTest::newRow("good") << QGeoCoordinate(10, 10) << 5.0 << true;
}

void tst_QGeoBoundingCircle::valid()
{
    QFETCH(QGeoCoordinate, center);
    QFETCH(qreal, radius);
    QFETCH(bool, valid);

    QGeoBoundingCircle c(center, radius);
    QCOMPARE(c.isValid(), valid);
}

void tst_QGeoBoundingCircle::empty_data()
{
    QTest::addColumn<QGeoCoordinate>("center");
    QTest::addColumn<qreal>("radius");
    QTest::addColumn<bool>("empty");

    QTest::newRow("default") << QGeoCoordinate() << -1.0 << true;
    QTest::newRow("empty coord") << QGeoCoordinate() << 5.0 << true;
    QTest::newRow("NaN coord") << QGeoCoordinate(500, 500) << 5.0 << true;
    QTest::newRow("bad radius") << QGeoCoordinate(10, 10) << -5.0 << true;
    QTest::newRow("NaN radius") << QGeoCoordinate(10, 10) << qQNaN() << true;
    QTest::newRow("zero radius") << QGeoCoordinate(10, 10) << 0.0 << true;
    QTest::newRow("good") << QGeoCoordinate(10, 10) << 5.0 << false;
}

void tst_QGeoBoundingCircle::empty()
{
    QFETCH(QGeoCoordinate, center);
    QFETCH(qreal, radius);
    QFETCH(bool, empty);

    QGeoBoundingCircle c(center, radius);
    QCOMPARE(c.isEmpty(), empty);
}

void tst_QGeoBoundingCircle::contains_data()
{
    QTest::addColumn<QGeoCoordinate>("center");
    QTest::addColumn<qreal>("radius");
    QTest::addColumn<QGeoCoordinate>("probe");
    QTest::addColumn<bool>("result");

    QTest::newRow("own centre") << QGeoCoordinate(1,1) << 100.0 <<
                                   QGeoCoordinate(1,1) << true;
    QTest::newRow("over the hills") << QGeoCoordinate(1,1) << 100.0 <<
                                       QGeoCoordinate(30, 40) << false;
    QTest::newRow("at 0.5*radius") << QGeoCoordinate(1,1) << 100.0 <<
                                      QGeoCoordinate(1.00015374,1.00015274) << true;
    QTest::newRow("at 0.99*radius") << QGeoCoordinate(1,1) << 100.0 <<
                                       QGeoCoordinate(1.00077538, 0.99955527) << true;
    QTest::newRow("at 1.01*radius") << QGeoCoordinate(1,1) << 100.0 <<
                                       QGeoCoordinate(1.00071413, 0.99943423) << false;
}

void tst_QGeoBoundingCircle::contains()
{
    QFETCH(QGeoCoordinate, center);
    QFETCH(qreal, radius);
    QFETCH(QGeoCoordinate, probe);
    QFETCH(bool, result);

    QGeoBoundingCircle c(center, radius);
    QCOMPARE(c.contains(probe), result);
}

QTEST_MAIN(tst_QGeoBoundingCircle)
#include "tst_qgeoboundingcircle.moc"
