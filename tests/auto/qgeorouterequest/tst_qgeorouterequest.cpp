#include "tst_qgeorouterequest.h"

QTM_USE_NAMESPACE

RouteRequest::RouteRequest()
{
}

void RouteRequest::initTestCase()
{

}

void RouteRequest::cleanupTestCase()
{

}

void RouteRequest::init()
{
    qgeocoordinate = new QGeoCoordinate();
    qgeoboundingbox = new QGeoBoundingBox();
    qgeorouterequest = new QGeoRouteRequest();
}

void RouteRequest::cleanup()
{
    delete qgeocoordinate;
    delete qgeoboundingbox;
    delete qgeorouterequest;

}

void RouteRequest::t_qgrreq_constructor1()
{
    QCOMPARE(typeid(*qgeorouterequest).name(),"N10QtMobility16QGeoRouteRequestE");

    QGeoCoordinate *qgeocoord1 = new QGeoCoordinate (43.5435 , 76.342);
    QGeoCoordinate *qgeocoord2 = new QGeoCoordinate (-43.5435 , 176.342);
    QGeoCoordinate *qgeocoord3 = new QGeoCoordinate (-13.5435 , +76.342);

    QList<QGeoCoordinate> waypoints;
    waypoints.append(*qgeocoord1);
    waypoints.append(*qgeocoord2);
    waypoints.append(*qgeocoord3);

    QGeoRouteRequest *qgeorouterequestcopy = new QGeoRouteRequest (waypoints);

    QCOMPARE(typeid(*qgeorouterequest).name(),typeid(*qgeorouterequestcopy).name());

    delete qgeocoord1;
    delete qgeocoord2;
    delete qgeocoord3;
    delete qgeorouterequestcopy;

}

void RouteRequest::t_qgrreq_constructor2()
{
    QGeoCoordinate *qgeocoord1 = new QGeoCoordinate (43.5435 , 76.342);
    QGeoCoordinate *qgeocoord2 = new QGeoCoordinate (-43.5435 , 176.342);

    QGeoRouteRequest *qgeorouterequestcopy = new QGeoRouteRequest (*qgeocoord1, *qgeocoord2);
    QCOMPARE(typeid(*qgeorouterequest).name(),typeid(*qgeorouterequestcopy).name());

    delete qgeocoord1;
    delete qgeocoord2;

    delete qgeorouterequestcopy;
}

void RouteRequest::t_qgrreq_constructorcopy()
{

    QGeoRouteRequest *qgeorouterequestcopy = new QGeoRouteRequest (*qgeorouterequest);
    QCOMPARE(typeid(*qgeorouterequest).name(),typeid(*qgeorouterequestcopy).name());
}

void RouteRequest::t_qgrreq_excludeAreas()
{
    qgeocoordinate->setLatitude(13.3851);
    qgeocoordinate->setLongitude(52.5312);

    QGeoCoordinate *qgeocoordinatecopy = new QGeoCoordinate(34.324 , -110.32);

    QGeoBoundingBox *qgeoboundingboxcopy= new QGeoBoundingBox(*qgeocoordinate,0.4,0.4);
    QGeoBoundingBox *qgeoboundingboxcopy2 = new QGeoBoundingBox(*qgeocoordinatecopy, 1.2, 0.9);
    QList<QGeoBoundingBox> areas;
    areas.append(*qgeoboundingboxcopy);
    areas.append(*qgeoboundingboxcopy2);

    qgeorouterequest->setExcludeAreas(areas);

    QCOMPARE(qgeorouterequest->excludeAreas(),areas);

    delete qgeoboundingboxcopy;
    delete qgeoboundingboxcopy2;
    delete qgeocoordinatecopy;
}

void RouteRequest::t_qgrreq_numberAlternativeRoutes()
{
    qgeorouterequest->setNumberAlternativeRoutes(0);
    QCOMPARE(qgeorouterequest->numberAlternativeRoutes(),0);

    qgeorouterequest->setNumberAlternativeRoutes(24);
    QCOMPARE(qgeorouterequest->numberAlternativeRoutes(),24);

    qgeorouterequest->setNumberAlternativeRoutes(-12);
    QCOMPARE(qgeorouterequest->numberAlternativeRoutes(),0);
}

void RouteRequest::t_qgrreq_routeOptimization()
{
    QFETCH(QGeoRouteRequest::RouteOptimization,optimization);

    QCOMPARE(qgeorouterequest->routeOptimization(),QGeoRouteRequest::FastestRoute);

    qgeorouterequest->setRouteOptimization(optimization);
    QCOMPARE(qgeorouterequest->routeOptimization(),optimization);
}

void RouteRequest::t_qgrreq_routeOptimization_data()
{
    QTest::addColumn<QGeoRouteRequest::RouteOptimization>("optimization");

    QTest::newRow("optimization1") << QGeoRouteRequest::FastestRoute;
    QTest::newRow("optimization2") << QGeoRouteRequest::ShortestRoute;
    QTest::newRow("optimization3") << QGeoRouteRequest::MostEconomicRoute;
    QTest::newRow("optimization4") << QGeoRouteRequest::MostScenicRoute;

}

void RouteRequest::t_qgrreq_segmentDetail()
{
    QFETCH(QGeoRouteRequest::SegmentDetail,detail);

    QCOMPARE(qgeorouterequest->segmentDetail(),QGeoRouteRequest::BasicSegmentData);

    qgeorouterequest->setSegmentDetail(detail);
    QCOMPARE(qgeorouterequest->segmentDetail(),detail);
}

void RouteRequest::t_qgrreq_segmentDetail_data()
{
    QTest::addColumn<QGeoRouteRequest::SegmentDetail>("detail");

    QTest::newRow("detail1") << QGeoRouteRequest::NoSegmentData;
    QTest::newRow("detail2") << QGeoRouteRequest::BasicSegmentData;
}

void RouteRequest::t_qgrreq_travelModes()
{
    QFETCH(QGeoRouteRequest::TravelMode,mode);

    QCOMPARE(qgeorouterequest->travelModes(),QGeoRouteRequest::CarTravel);

    qgeorouterequest->setTravelModes(mode);
    QCOMPARE(qgeorouterequest->travelModes(),mode);
}

void RouteRequest::t_qgrreq_travelModes_data()
{
    QTest::addColumn<QGeoRouteRequest::TravelMode>("mode");

    QTest::newRow("mode1") << QGeoRouteRequest::CarTravel;
    QTest::newRow("mode2") << QGeoRouteRequest::PedestrianTravel;
    QTest::newRow("mode3") << QGeoRouteRequest::BicycleTravel;
    QTest::newRow("mode4") << QGeoRouteRequest::PublicTransitTravel;
    QTest::newRow("mode5") << QGeoRouteRequest::TruckTravel;
}

void RouteRequest::t_qgrreq_waypoints()
{
    QFETCH(QList<double>, coordinates);

    QList<QGeoCoordinate> waypoints;

    for (int i = 0; i<coordinates.size(); i=i+2)
    {

        waypoints.append(*(new QGeoCoordinate(coordinates.at(i),coordinates.at(i+1))));
    }

   qgeorouterequest->setWaypoints(waypoints);

    QList<QGeoCoordinate> waypointsretrieved= qgeorouterequest->waypoints();
    QCOMPARE(waypointsretrieved,waypoints);

    for (int i=0; i<waypointsretrieved.size();i++)
    {
        QCOMPARE(waypointsretrieved.at(i),waypoints.at(i));
    }


}

void RouteRequest::t_qgrreq_waypoints_data()
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

void RouteRequest::t_qgrreq_maneuverDetail()
{
    QFETCH(QGeoRouteRequest::ManeuverDetail,maneuver);

    QCOMPARE(qgeorouterequest->maneuverDetail(),QGeoRouteRequest::BasicManeuvers);

    qgeorouterequest->setManeuverDetail(maneuver);
    QCOMPARE(qgeorouterequest->maneuverDetail(),maneuver);
}

void RouteRequest::t_qgrreq_maneuverDetail_data()
{
    QTest::addColumn<QGeoRouteRequest::ManeuverDetail>("maneuver");

    QTest::newRow("maneuver1") << QGeoRouteRequest::NoManeuvers;
    QTest::newRow("maneuver2") << QGeoRouteRequest::BasicManeuvers;

}

void RouteRequest::t_qgrreq_featureWeight()
{
    QFETCH(QGeoRouteRequest::FeatureType,type);

    //Compare that there are not features with weight
    QList<QGeoRouteRequest::FeatureType> types;
    QCOMPARE(qgeorouterequest->featureTypes(),types);

    //Set a weight to the feature
    switch(type)
    {
    case QGeoRouteRequest::NoFeature :
        QCOMPARE(qgeorouterequest->featureWeight(type),QGeoRouteRequest::NeutralFeatureWeight);
        qgeorouterequest->setFeatureWeight(type,QGeoRouteRequest::PreferFeatureWeight);
        QCOMPARE(qgeorouterequest->featureWeight(type),QGeoRouteRequest::NeutralFeatureWeight);

        break;

    case QGeoRouteRequest::TollFeature :
        QCOMPARE(qgeorouterequest->featureWeight(type),QGeoRouteRequest::NeutralFeatureWeight);
        qgeorouterequest->setFeatureWeight(type,QGeoRouteRequest::PreferFeatureWeight);
        QCOMPARE(qgeorouterequest->featureWeight(type),QGeoRouteRequest::PreferFeatureWeight);

        types.append(type);
        break;

    case QGeoRouteRequest::HighwayFeature :
        QCOMPARE(qgeorouterequest->featureWeight(type),QGeoRouteRequest::NeutralFeatureWeight);
        qgeorouterequest->setFeatureWeight(type,QGeoRouteRequest::RequireFeatureWeight);
        QCOMPARE(qgeorouterequest->featureWeight(type),QGeoRouteRequest::RequireFeatureWeight);

        types.append(type);

        break;

    case QGeoRouteRequest::PublicTransitFeature :
        QCOMPARE(qgeorouterequest->featureWeight(type),QGeoRouteRequest::NeutralFeatureWeight);
        qgeorouterequest->setFeatureWeight(type,QGeoRouteRequest::AvoidFeatureWeight);
        QCOMPARE(qgeorouterequest->featureWeight(type),QGeoRouteRequest::AvoidFeatureWeight);

        types.append(type);

        break;

    case QGeoRouteRequest::FerryFeature :
        QCOMPARE(qgeorouterequest->featureWeight(type),QGeoRouteRequest::NeutralFeatureWeight);
        qgeorouterequest->setFeatureWeight(type,QGeoRouteRequest::DisallowFeatureWeight);
        QCOMPARE(qgeorouterequest->featureWeight(type),QGeoRouteRequest::DisallowFeatureWeight);

        types.append(type);

        break;

    case QGeoRouteRequest::TunnelFeature :
        QCOMPARE(qgeorouterequest->featureWeight(type),QGeoRouteRequest::NeutralFeatureWeight);
        qgeorouterequest->setFeatureWeight(type,QGeoRouteRequest::NeutralFeatureWeight);
        QCOMPARE(qgeorouterequest->featureWeight(type),QGeoRouteRequest::NeutralFeatureWeight);


        break;

    case QGeoRouteRequest::DirtRoadFeature :
        QCOMPARE(qgeorouterequest->featureWeight(type),QGeoRouteRequest::NeutralFeatureWeight);
        qgeorouterequest->setFeatureWeight(type,QGeoRouteRequest::PreferFeatureWeight);
        QCOMPARE(qgeorouterequest->featureWeight(type),QGeoRouteRequest::PreferFeatureWeight);

        types.append(type);

        break;

    case QGeoRouteRequest::ParksFeature :
        QCOMPARE(qgeorouterequest->featureWeight(type),QGeoRouteRequest::NeutralFeatureWeight);
        qgeorouterequest->setFeatureWeight(type,QGeoRouteRequest::AvoidFeatureWeight);
        QCOMPARE(qgeorouterequest->featureWeight(type),QGeoRouteRequest::AvoidFeatureWeight);

        types.append(type);

        break;

    case QGeoRouteRequest::MotorPoolLaneFeature :
        QCOMPARE(qgeorouterequest->featureWeight(type),QGeoRouteRequest::NeutralFeatureWeight);
        qgeorouterequest->setFeatureWeight(type,QGeoRouteRequest::DisallowFeatureWeight);
        QCOMPARE(qgeorouterequest->featureWeight(type),QGeoRouteRequest::DisallowFeatureWeight);

        types.append(type);

        break;

    }
    QCOMPARE(qgeorouterequest->featureTypes(),types);

}

void RouteRequest::t_qgrreq_featureWeight_data()
{
    QTest::addColumn<QGeoRouteRequest::FeatureType>("type");

    QTest::newRow("type1") << QGeoRouteRequest::NoFeature;
    QTest::newRow("type1") << QGeoRouteRequest::TollFeature;
    QTest::newRow("type2") << QGeoRouteRequest::HighwayFeature;
    QTest::newRow("type3") << QGeoRouteRequest::PublicTransitFeature;
    QTest::newRow("type4") << QGeoRouteRequest::FerryFeature;
    QTest::newRow("type5") << QGeoRouteRequest::TunnelFeature;
    QTest::newRow("type6") << QGeoRouteRequest::DirtRoadFeature;
    QTest::newRow("type7") << QGeoRouteRequest::ParksFeature;
    QTest::newRow("type8") << QGeoRouteRequest::MotorPoolLaneFeature;


}

QTEST_MAIN(RouteRequest);
