#include "tst_qgeoroutesegment.h"

QTM_USE_NAMESPACE

RouteSegment::RouteSegment()
{
}

void RouteSegment::initTestCase()
{

}

void RouteSegment::cleanupTestCase()
{

}

void RouteSegment::init()
{
    qgeocoordinate = new QGeoCoordinate();
    qgeoroutesegment = new QGeoRouteSegment();
    qgeomaneuver = new QGeoManeuver();
}

void RouteSegment::cleanup()
{
    delete qgeocoordinate;
    delete qgeoroutesegment;

}


void RouteSegment::t_qgrs_constructor()
{
    QCOMPARE(typeid(*qgeoroutesegment).name(),"N10QtMobility16QGeoRouteSegmentE");
}

void RouteSegment::t_qgrs_constructorCopy()
{
    QGeoRouteSegment *qgeoroutesegmentcopy = new QGeoRouteSegment (*qgeoroutesegment);
    QCOMPARE(typeid(*qgeoroutesegment).name(),typeid(*qgeoroutesegmentcopy).name());
    delete qgeoroutesegmentcopy;
}

void RouteSegment::t_qgrs_travelTime()
{
    QFETCH(int, traveltime);

    qgeoroutesegment->setTravelTime(traveltime);

    QCOMPARE(qgeoroutesegment->travelTime(),traveltime);
}

void RouteSegment::t_qgrs_travelTime_data()
{
    QTest::addColumn<int>("traveltime");

    QTest::newRow("travel1") << 0 ;
    QTest::newRow("travel2") << -50 ;
    QTest::newRow("travel3") << 324556 ;
}

void RouteSegment::t_qgrs_distance()
{
    QFETCH(double, distance);

    qgeoroutesegment->setDistance(distance);

    QCOMPARE(qgeoroutesegment->distance(),distance);
}

void RouteSegment::t_qgrs_distance_data()
{
    QTest::addColumn<double>("distance");

    QTest::newRow("distance1") << 0.0 ;
    QTest::newRow("distance2") << -50.3435 ;
    QTest::newRow("distance3") << 324556.543534 ;
}


//Possible Memory leak regarding the path and coordinates objects..I am not deleting them. :(
//The path accepts being made with invalid Coords!
void RouteSegment::t_qgrs_path()
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

void RouteSegment::t_qgrs_path_data()
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

void RouteSegment::t_qgrs_nextroutesegment()
{
    QGeoRouteSegment *qgeoroutesegmentcopy = new QGeoRouteSegment();
    qgeoroutesegmentcopy->setDistance(45.34);
    qgeoroutesegment->setNextRouteSegment(*qgeoroutesegmentcopy);

    QCOMPARE(qgeoroutesegment->nextRouteSegment(),*qgeoroutesegmentcopy);

    QCOMPARE((qgeoroutesegment->nextRouteSegment()).distance(),qgeoroutesegmentcopy->distance());
    delete qgeoroutesegmentcopy;

}

void RouteSegment::t_qgrs_maneuver()
{
    qgeoroutesegment->setManeuver(*qgeomaneuver);
    QCOMPARE(qgeoroutesegment->maneuver(),*qgeomaneuver);
}

void RouteSegment::t_qgrs_isValid()
{
    QVERIFY(qgeoroutesegment->isValid());

}


void RouteSegment::t_qgrs_operators()
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

    QVERIFY(!qgeoroutesegment->operator ==(*qgeoroutesegmentcopy));
    QVERIFY(qgeoroutesegment->operator !=(*qgeoroutesegmentcopy));

    //Assign one address to the other and test that they are the same again
    *qgeoroutesegmentcopy = qgeoroutesegmentcopy->operator =(*qgeoroutesegment);
    QVERIFY(qgeoroutesegment->operator ==(*qgeoroutesegmentcopy));
    QVERIFY(!qgeoroutesegment->operator !=(*qgeoroutesegmentcopy));

    delete qgeoroutesegmentcopy;
}

void RouteSegment::t_qgrs_operators_data()
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

QTEST_MAIN(RouteSegment);
