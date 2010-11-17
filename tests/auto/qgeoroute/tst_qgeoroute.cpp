#include "tst_qgeoroute.h"

QTM_USE_NAMESPACE

Route::Route()
{
}

void Route::initTestCase()
{

}

void Route::cleanupTestCase()
{

}

void Route::init()
{
    qgeoroute = new QGeoRoute();
    qgeocoordinate = new QGeoCoordinate ();
}

void Route::cleanup()
{
    delete qgeoroute;
    delete qgeocoordinate;
}

void Route::t_qgr_constructor()
{
    qgeoroute = new QGeoRoute ();
    QCOMPARE(typeid(*qgeoroute).name(),"N10QtMobility9QGeoRouteE");

}

void Route::t_qgr_constructorCopy()
{
    QGeoRoute *qgeoroutecopy = new QGeoRoute (*qgeoroute);
    QCOMPARE(typeid(*qgeoroute).name(),typeid(*qgeoroutecopy).name());
    delete qgeoroutecopy;

}

void Route::t_qgr_bounds()
{
    qgeocoordinate->setLatitude(13.3851);
    qgeocoordinate->setLongitude(52.5312);

    QGeoBoundingBox *qgeoboundingbox= new QGeoBoundingBox(*qgeocoordinate,0.4,0.4);

    qgeoroute->setBounds(*qgeoboundingbox);

    QCOMPARE(qgeoroute->bounds(),*qgeoboundingbox);

    qgeoboundingbox->setHeight(23.1);

    QEXPECT_FAIL("", "Expected Fail", Continue);
    QCOMPARE(qgeoroute->bounds(),*qgeoboundingbox);

    delete qgeoboundingbox;


}

void Route::t_qgr_distance()
{
    double distance = 0.0;

    qgeoroute->setDistance(distance);
    QCOMPARE(qgeoroute->distance(),distance);

    distance = 34.4324;
    QEXPECT_FAIL("", "Expected Fail", Continue);
    QCOMPARE(qgeoroute->distance(),distance);

    qgeoroute->setDistance(distance);
    QCOMPARE(qgeoroute->distance(),distance);
}

void Route::t_qgr_path()
{
    QFETCH(QList<double>, coordinates);

    QList<QGeoCoordinate> path;

    for (int i = 0; i<coordinates.size(); i=i+2)
    {

        path.append(*(new QGeoCoordinate(coordinates.at(i),coordinates.at(i+1))));
    }

   qgeoroute->setPath(path);

    QList<QGeoCoordinate> pathretrieved= qgeoroute->path();
    QCOMPARE(pathretrieved,path);

    for (int i=0; i<pathretrieved.size();i++)
    {
        QCOMPARE(pathretrieved.at(i),path.at(i));
    }
}

void Route::t_qgr_path_data()
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
void Route::t_qgr_request()
{
    qgeocoordinate->setLatitude(65.654);
    qgeocoordinate->setLongitude(0.4324);

    QGeoCoordinate *qgeocoordinatecopy = new QGeoCoordinate (34.54 , -21.32);

    QList<QGeoCoordinate> path;
    path.append(*qgeocoordinate);
    path.append(*qgeocoordinatecopy);

    qgeorouterequest = new QGeoRouteRequest(path);

    qgeoroute->setRequest(*qgeorouterequest);

    QCOMPARE(qgeoroute->request(),*qgeorouterequest);

    QGeoCoordinate *qgeocoordinatecopy2 = new QGeoCoordinate (4.7854 , -121.32);
    path.append(*qgeocoordinatecopy2);

    QGeoRouteRequest *qgeorouterequestcopy = new QGeoRouteRequest(path);

    QEXPECT_FAIL("", "Expected Fail", Continue);
    QCOMPARE(qgeoroute->request(),*qgeorouterequestcopy);

    delete qgeocoordinatecopy;
    delete qgeocoordinatecopy2;
    delete qgeorouterequest;
    delete qgeorouterequestcopy;
}

void Route::t_qgr_routeId()
{
    QString text = "routeId 4504";

    qgeoroute->setRouteId(text);

    QCOMPARE (qgeoroute->routeId(),text);

    text="routeId 1111";
    QEXPECT_FAIL("", "Expected Fail", Continue);
    QCOMPARE (qgeoroute->routeId(),text);

}

void Route::t_qgr_firstrouteSegments()
{
    qgeoroutesegment = new QGeoRouteSegment ();
    qgeoroutesegment->setDistance(35.453);
    qgeoroutesegment->setTravelTime(56);

    qgeoroute->setFirstRouteSegment(*qgeoroutesegment);

    QCOMPARE(qgeoroute->firstRouteSegment(),*qgeoroutesegment);

    QGeoRouteSegment *qgeoroutesegmentcopy = new QGeoRouteSegment ();
    qgeoroutesegmentcopy->setDistance(435.432);
    qgeoroutesegmentcopy->setTravelTime(786);

    QEXPECT_FAIL("", "Expected Fail", Continue);
    QCOMPARE(qgeoroute->firstRouteSegment(),*qgeoroutesegmentcopy);


    delete qgeoroutesegment;
    delete qgeoroutesegmentcopy;

}

void Route::t_qgr_travelMode()
{
    QFETCH(QGeoRouteRequest::TravelMode,mode);

    qgeoroute->setTravelMode(mode);
    QCOMPARE(qgeoroute->travelMode(),mode);
}
void Route::t_qgr_travelMode_data()
{
    QTest::addColumn<QGeoRouteRequest::TravelMode>("mode");

    QTest::newRow("mode1") << QGeoRouteRequest::CarTravel;
    QTest::newRow("mode2") << QGeoRouteRequest::PedestrianTravel;
    QTest::newRow("mode3") << QGeoRouteRequest::BicycleTravel;
    QTest::newRow("mode4") << QGeoRouteRequest::PublicTransitTravel;
    QTest::newRow("mode5") << QGeoRouteRequest::TruckTravel;
}

void Route::t_qgr_travelTime()
{
    int time = 0;
    qgeoroute->setTravelTime(time);

    QCOMPARE (qgeoroute->travelTime(),time);

    time = 35;

    QEXPECT_FAIL("", "Expected Fail", Continue);
    QCOMPARE (qgeoroute->travelTime(),time);

    qgeoroute->setTravelTime(time);
    QCOMPARE (qgeoroute->travelTime(),time);
}

void Route::t_qgr_operators()
{
    QGeoRoute *qgeoroutecopy = new QGeoRoute (*qgeoroute);

    QVERIFY(qgeoroute->operator ==(*qgeoroutecopy));
    QVERIFY(!qgeoroute->operator !=(*qgeoroutecopy));

    qgeoroute->setDistance(543.324);
    qgeoroute->setRouteId("RouteId 111");
    qgeoroute->setTravelMode(QGeoRouteRequest::PedestrianTravel);
    qgeoroute->setTravelTime(10);

    qgeoroutecopy->setDistance(12.21);
    qgeoroutecopy->setRouteId("RouteId 666");
    qgeoroutecopy->setTravelMode(QGeoRouteRequest::BicycleTravel);
    qgeoroutecopy->setTravelTime(99);

    QVERIFY(!(qgeoroute->operator ==(*qgeoroutecopy)));
    QVERIFY(qgeoroute->operator !=(*qgeoroutecopy));

    *qgeoroutecopy = qgeoroutecopy->operator =(*qgeoroute);
    QVERIFY(qgeoroute->operator ==(*qgeoroutecopy));
    QVERIFY(!qgeoroute->operator !=(*qgeoroutecopy));

    delete qgeoroutecopy;
}



QTEST_MAIN(Route);
