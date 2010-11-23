#include "tst_qgeoroutereply.h"

QTM_USE_NAMESPACE

void RouteReply::initTestCase()
{
    qgeocoordinate1 = new QGeoCoordinate (43.5435 , 76.342);
    qgeocoordinate2 = new QGeoCoordinate (-43.5435 , 176.342);
    qgeocoordinate3 = new QGeoCoordinate (-13.5435 , +76.342);

    waypoints.append(*qgeocoordinate1);
    waypoints.append(*qgeocoordinate2);
    waypoints.append(*qgeocoordinate3);

    qgeorouterequest = new QGeoRouteRequest (waypoints);
    reply = new SubRouteReply(*qgeorouterequest);
}

void RouteReply::cleanupTestCase()
{
    delete qgeocoordinate1;
    delete qgeocoordinate2;
    delete qgeocoordinate3;
    delete qgeorouterequest;
    delete reply;
}

void RouteReply::init()
{
    qRegisterMetaType<QGeoRouteReply::Error>("Error");
    signalerror = new QSignalSpy(reply, SIGNAL(error(QGeoRouteReply::Error,const QString)));
    signalfinished = new QSignalSpy(reply, SIGNAL(finished()));
}

void RouteReply::cleanup()
{
    delete signalerror;
    delete signalfinished;
}

void RouteReply::t_qgrrep_constructor1()
{
    QVERIFY(!reply->isFinished());
    QVERIFY( signalerror->isValid() );
    QVERIFY( signalfinished->isValid() );

    QCOMPARE(signalerror->count(),0);
    QCOMPARE(signalfinished->count(),0);



}

void RouteReply::t_qgrrep_constructor2()
{
    QFETCH(QGeoRouteReply::Error,error);
    QFETCH(QString,msg);

    QVERIFY( signalerror->isValid() );
    QVERIFY( signalfinished->isValid() );

    QGeoRouteReply *qgeoroutereplycopy = new QGeoRouteReply (error,msg,0);

    QCOMPARE(typeid(*qgeoroutereplycopy).name(),"N10QtMobility14QGeoRouteReplyE");
    QCOMPARE(signalerror->count(),0);
    QCOMPARE(signalfinished->count(),0);
    QCOMPARE (qgeoroutereplycopy->errorString(),msg);
    delete qgeoroutereplycopy;
}

void RouteReply::t_qgrrep_constructor2_data()
{
    QTest::addColumn<QGeoRouteReply::Error>("error");
    QTest::addColumn<QString>("msg");

    QTest::newRow("error1") << QGeoRouteReply::NoError << "No error.";
    QTest::newRow("error2") << QGeoRouteReply::EngineNotSetError << "Engine Not Set Error.";
    QTest::newRow("error3") << QGeoRouteReply::CommunicationError << "Comunication Error.";
    QTest::newRow("error4") << QGeoRouteReply::ParseError << "Parse Error.";
    QTest::newRow("error5") << QGeoRouteReply::UnsupportedOptionError << "Unsupported Option Error.";
    QTest::newRow("error6") << QGeoRouteReply::UnknownError << "Unknown Error.";

}

void RouteReply::t_qgrrep_routes()
{
    QList<QGeoRoute> routes;
    QGeoRoute *route1 = new QGeoRoute ();
    QGeoRoute *route2 = new QGeoRoute ();

    route1->setDistance(15.12);
    route2->setDistance(20.12);

    routes.append(*route1);
    routes.append(*route2);

    reply->callSetRoutes(routes);

    QList<QGeoRoute> routescopy;
    routescopy = reply->routes();
    QCOMPARE(routescopy,routes);

    QCOMPARE(routescopy.at(0).distance(),route1->distance());
    QCOMPARE(routescopy.at(1).distance(),route2->distance());

    delete route1;
    delete route2;
}

void RouteReply::t_qgrrep_finished()
{
    QVERIFY( signalerror->isValid() );
    QVERIFY( signalfinished->isValid() );

    QCOMPARE(signalerror->count(),0);
    QCOMPARE (signalfinished->count(),0);

    reply->callSetFinished(true);

    QCOMPARE(signalerror->count(),0);
    QCOMPARE (signalfinished->count(),1);

    reply->callSetFinished(false);

    QCOMPARE(signalerror->count(),0);
    QCOMPARE (signalfinished->count(),1);

    reply->callSetFinished(true);

    QCOMPARE(signalerror->count(),0);
    QCOMPARE (signalfinished->count(),2);
}

void RouteReply::t_qgrrep_abort()
{
    QVERIFY( signalerror->isValid() );
    QVERIFY( signalfinished->isValid() );

    QCOMPARE(signalerror->count(),0);
    QCOMPARE (signalfinished->count(),0);

    reply->abort();

    QCOMPARE(signalerror->count(),0);
    QCOMPARE (signalfinished->count(),0);

    reply->abort();
    reply->callSetFinished(false);
    reply->abort();

    QCOMPARE(signalerror->count(),0);
    QCOMPARE (signalfinished->count(),1);
}

void RouteReply::t_qgrrep_error()
{
    QFETCH(QGeoRouteReply::Error,error);
    QFETCH(QString,msg);

    QVERIFY( signalerror->isValid() );
    QVERIFY( signalfinished->isValid() );
    QCOMPARE(signalerror->count(),0);

    reply->callSetError(error,msg);

   QCOMPARE(signalerror->count(),1);
   QCOMPARE(signalfinished->count(),1);
   QCOMPARE(reply->errorString(),msg);
   QCOMPARE(reply->error(),error);


}

void RouteReply::t_qgrrep_error_data()
{
    QTest::addColumn<QGeoRouteReply::Error>("error");
    QTest::addColumn<QString>("msg");

    QTest::newRow("error1") << QGeoRouteReply::NoError << "No error.";
    QTest::newRow("error2") << QGeoRouteReply::EngineNotSetError << "Engine Not Set Error.";
    QTest::newRow("error3") << QGeoRouteReply::CommunicationError << "Comunication Error.";
    QTest::newRow("error4") << QGeoRouteReply::ParseError << "Parse Error.";
    QTest::newRow("error5") << QGeoRouteReply::UnsupportedOptionError << "Unsupported Option Error.";
    QTest::newRow("error6") << QGeoRouteReply::UnknownError << "Unknown Error.";
}

void RouteReply::t_qgrrep_request()
{
    SubRouteReply *rr= new SubRouteReply(*qgeorouterequest);

    QCOMPARE(rr->request(),*qgeorouterequest);

    delete rr;
}

QTEST_MAIN(RouteReply);
