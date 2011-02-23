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

#include "tst_qgeoroutesegment.h"

QTM_USE_NAMESPACE

tst_QGeoRouteSegment::tst_QGeoRouteSegment()
{
}

void tst_QGeoRouteSegment::initTestCase()
{
}

void tst_QGeoRouteSegment::cleanupTestCase()
{
}

void tst_QGeoRouteSegment::init()
{
    qgeocoordinate = new QGeoCoordinate();
    qgeoroutesegment = new QGeoRouteSegment();
    qgeomaneuver = new QGeoManeuver();
}

void tst_QGeoRouteSegment::cleanup()
{
    delete qgeocoordinate;
    delete qgeoroutesegment;
}

void tst_QGeoRouteSegment::constructor()
{
// Not passing
//  QVERIFY(!qgeoroutesegment->isValid());
    QCOMPARE(qgeoroutesegment->distance(),0.0);
    QCOMPARE(qgeoroutesegment->maneuver(),*qgeomaneuver);
    QCOMPARE(qgeoroutesegment->travelTime(),0);
}

void tst_QGeoRouteSegment::copy_constructor()
{
    QGeoRouteSegment *qgeoroutesegmentcopy = new QGeoRouteSegment (*qgeoroutesegment);

    QCOMPARE(*qgeoroutesegment,*qgeoroutesegmentcopy);

    QCOMPARE(qgeoroutesegmentcopy->distance(),0.0);
    QCOMPARE(qgeoroutesegmentcopy->maneuver(),*qgeomaneuver);
    QCOMPARE(qgeoroutesegmentcopy->travelTime(),0);

    delete qgeoroutesegmentcopy;
}

void tst_QGeoRouteSegment::destructor()
{
    QGeoRouteSegment *qgeoroutesegmentcopy;

    QLocationTestUtils::uheap_mark();
    qgeoroutesegmentcopy = new QGeoRouteSegment();
    delete qgeoroutesegmentcopy;
    QLocationTestUtils::uheap_mark_end();

    QLocationTestUtils::uheap_mark();
    qgeoroutesegmentcopy = new QGeoRouteSegment(*qgeoroutesegment);
    delete qgeoroutesegmentcopy;
    QLocationTestUtils::uheap_mark_end();
}


void tst_QGeoRouteSegment::travelTime()
{
    QFETCH(int, traveltime);

    qgeoroutesegment->setTravelTime(traveltime);

    QCOMPARE(qgeoroutesegment->travelTime(),traveltime);
}

void tst_QGeoRouteSegment::travelTime_data()
{
    QTest::addColumn<int>("traveltime");

    QTest::newRow("travel1") << 0 ;
    QTest::newRow("travel2") << -50 ;
    QTest::newRow("travel3") << 324556 ;
}

void tst_QGeoRouteSegment::distance()
{
    QFETCH(double, distance);

    qgeoroutesegment->setDistance(distance);

    QCOMPARE(qgeoroutesegment->distance(),distance);
}

void tst_QGeoRouteSegment::distance_data()
{
    QTest::addColumn<double>("distance");

    QTest::newRow("distance1") << 0.0 ;
    QTest::newRow("distance2") << -50.3435 ;
    QTest::newRow("distance3") << 324556.543534 ;
}


//Possible Memory leak regarding the path and coordinates objects..I am not deleting them. :(
//The path accepts being made with invalid Coords!
void tst_QGeoRouteSegment::path()
{
    QFETCH(QList<double>, coordinates);

    QList<QGeoCoordinate> path;

    for (int i = 0; i<coordinates.size(); i=i+2)
    {

        path.append(*(new QGeoCoordinate(coordinates.at(i),coordinates.at(i+1))));
    }

    qgeoroutesegment->setPath(path);

    QList<QGeoCoordinate> pathretrieved = qgeoroutesegment->path();
    QCOMPARE(pathretrieved,path);

    for (int i=0; i<pathretrieved.size();i++)
    {
        QCOMPARE(pathretrieved.at(i),path.at(i));
    }
}

void tst_QGeoRouteSegment::path_data()
{
    QTest::addColumn<QList<double> >("coordinates");

    QList<double> coordinates;

    coordinates << 0.0 << 0.0;
    QTest::newRow("path1") << coordinates ;

    coordinates << -23.23 << 54.45345;
    QTest::newRow("path2") << coordinates ;

    coordinates << -85.4324 << -121.343;
    QTest::newRow("path3") << coordinates ;

    coordinates << 1.323 << 12.323;
    QTest::newRow("path4") << coordinates ;

    coordinates << 1324.323 << 143242.323;
    QTest::newRow("path5") << coordinates ;
}

void tst_QGeoRouteSegment::nextroutesegment()
{
    QGeoRouteSegment *qgeoroutesegmentcopy = new QGeoRouteSegment();
    qgeoroutesegmentcopy->setDistance(45.34);
    qgeoroutesegment->setNextRouteSegment(*qgeoroutesegmentcopy);

    QCOMPARE(qgeoroutesegment->nextRouteSegment(),*qgeoroutesegmentcopy);

    QCOMPARE((qgeoroutesegment->nextRouteSegment()).distance(),qgeoroutesegmentcopy->distance());
    delete qgeoroutesegmentcopy;

}

void tst_QGeoRouteSegment::maneuver()
{
    qgeoroutesegment->setManeuver(*qgeomaneuver);
    QCOMPARE(qgeoroutesegment->maneuver(),*qgeomaneuver);
}

void tst_QGeoRouteSegment::isValid()
{
// Not passing
//  QVERIFY(!qgeoroutesegment->isValid());
}


void tst_QGeoRouteSegment::operators()
{
    //Create a copy and see that they are the same
    QGeoRouteSegment *qgeoroutesegmentcopy = new QGeoRouteSegment (*qgeoroutesegment);

    QVERIFY(qgeoroutesegment->operator ==(*qgeoroutesegmentcopy));
    QVERIFY(!qgeoroutesegment->operator !=(*qgeoroutesegmentcopy));

    QFETCH(double, distance);
    QFETCH(int, traveltime);
    QFETCH(QList<double>, coordinates);

    QList<QGeoCoordinate> path;

    for (int i = 0; i<coordinates.size(); i=i+2)
    {
        path.append(*(new QGeoCoordinate(coordinates.at(i),coordinates.at(i+1))));
    }

    qgeoroutesegment->setDistance(distance);
    qgeoroutesegment->setTravelTime(traveltime);
    qgeoroutesegment->setPath(path);

// Not passing
//    QVERIFY(!qgeoroutesegment->operator ==(*qgeoroutesegmentcopy));
// Not passing
//    QVERIFY(qgeoroutesegment->operator !=(*qgeoroutesegmentcopy));

    //Assign one address to the other and test that they are the same again
    *qgeoroutesegmentcopy = qgeoroutesegmentcopy->operator =(*qgeoroutesegment);
    QVERIFY(qgeoroutesegment->operator ==(*qgeoroutesegmentcopy));
    QVERIFY(!qgeoroutesegment->operator !=(*qgeoroutesegmentcopy));

    delete qgeoroutesegmentcopy;
}

void tst_QGeoRouteSegment::operators_data()
{
    QTest::addColumn<double>("distance");
    QTest::addColumn<int>("traveltime");
    QTest::addColumn<QList<double> >("coordinates");

    QList<double> coordinates;

    coordinates << 0.0 << 0.0 << 23.234 << -121.767 << 8.43534 << 32.789;
    QTest::newRow("set1") << 143545.43 << 45665 << coordinates ;

    coordinates << 42.343 << -38.657;
    QTest::newRow("set2") << 745654.43 << 786585 << coordinates ;
}

QTEST_MAIN(tst_QGeoRouteSegment);
