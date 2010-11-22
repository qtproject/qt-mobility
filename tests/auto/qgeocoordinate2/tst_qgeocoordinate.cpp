#include "tst_qgeocoordinate.h"

QTM_USE_NAMESPACE

Coordinate::Coordinate()
{
}

void Coordinate::initTestCase()
{

}

void Coordinate::cleanupTestCase()
{

}

void Coordinate::init()
{

    qgeocoordinate = new QGeoCoordinate();
}

void Coordinate::cleanup()
{
    delete qgeocoordinate;
}

void Coordinate::t_qgc_constructor1()
{
    QCOMPARE(typeid(*qgeocoordinate).name(),"N10QtMobility14QGeoCoordinateE");
    QVERIFY(!qgeocoordinate->isValid());
    QCOMPARE(qgeocoordinate->type(),QGeoCoordinate::InvalidCoordinate);
}

void Coordinate::t_qgc_constructor2()
{
    double latitude=200;
    double longitude=200;

    QGeoCoordinate *qgeocoordinatecopy = new QGeoCoordinate(latitude, longitude);

    QCOMPARE(typeid(*qgeocoordinate).name(),typeid(*qgeocoordinatecopy).name());
    QVERIFY(!qgeocoordinatecopy->isValid());
    QCOMPARE(qgeocoordinatecopy->type(),QGeoCoordinate::InvalidCoordinate);

    delete qgeocoordinatecopy;

    latitude=0;
    longitude=0;

    qgeocoordinatecopy = new QGeoCoordinate(latitude, longitude);
    QCOMPARE(typeid(*qgeocoordinate).name(),typeid(*qgeocoordinatecopy).name());
    QVERIFY(qgeocoordinatecopy->isValid());
    QCOMPARE(qgeocoordinatecopy->type(),QGeoCoordinate::Coordinate2D);

    delete qgeocoordinatecopy;

}

void Coordinate::t_qgc_constructor3()
{

    double latitude=200;
    double longitude=200;
    double altitude= -100;

    QGeoCoordinate *qgeocoordinatecopy = new QGeoCoordinate(latitude, longitude,altitude);

    QCOMPARE(typeid(*qgeocoordinate).name(),typeid(*qgeocoordinatecopy).name());
    QVERIFY(!qgeocoordinatecopy->isValid());
    QCOMPARE(qgeocoordinatecopy->type(),QGeoCoordinate::InvalidCoordinate);

    delete qgeocoordinatecopy;

    latitude=0;
    longitude=0;
    altitude=-100;

    qgeocoordinatecopy = new QGeoCoordinate(latitude, longitude,altitude);
    QCOMPARE(typeid(*qgeocoordinate).name(),typeid(*qgeocoordinatecopy).name());
    QVERIFY(qgeocoordinatecopy->isValid());
    QCOMPARE(qgeocoordinatecopy->type(),QGeoCoordinate::Coordinate3D);

    delete qgeocoordinatecopy;
}

void Coordinate::t_qgc_constructorCopy()
{
    QGeoCoordinate *qgeocoordinatecopy = new QGeoCoordinate (*qgeocoordinate);
    QCOMPARE(typeid(*qgeocoordinate).name(),typeid(*qgeocoordinatecopy).name());
    delete qgeocoordinatecopy;
}

void Coordinate::t_qgc_altitude()
{
    double altitude = 100;

    qgeocoordinate->setAltitude(altitude);

    QCOMPARE(qgeocoordinate->altitude(),altitude);
    QVERIFY(!qgeocoordinate->isValid());
    QCOMPARE(qgeocoordinate->type(),QGeoCoordinate::InvalidCoordinate);
}

void Coordinate::t_qgc_latitude()
{
    double latitude = 100;

    qgeocoordinate->setLatitude(latitude);

    QCOMPARE(qgeocoordinate->latitude(),latitude);
    QVERIFY(!qgeocoordinate->isValid());
    QCOMPARE(qgeocoordinate->type(),QGeoCoordinate::InvalidCoordinate);
}

void Coordinate::t_qgc_longitude()
{
    double longitude = 100;

    qgeocoordinate->setLongitude(longitude);

    QCOMPARE(qgeocoordinate->longitude(),longitude);
    QVERIFY(!qgeocoordinate->isValid());
    QCOMPARE(qgeocoordinate->type(),QGeoCoordinate::InvalidCoordinate);
}

void Coordinate::t_qgc_isValid()
{
    QFETCH(double, latitude);
    QFETCH(double, longitude);
    QFETCH(double, altitude);
    QFETCH(bool, result);

    qgeocoordinate->setAltitude(altitude);
    QVERIFY(!qgeocoordinate->isValid());
    qgeocoordinate->setLatitude(latitude);
    QVERIFY(!qgeocoordinate->isValid());
    qgeocoordinate->setLongitude(longitude);

    QCOMPARE(qgeocoordinate->isValid(),result);

}

void Coordinate::t_qgc_isValid_data()
{
    QTest::addColumn<double>("latitude");
    QTest::addColumn<double>("longitude");
    QTest::addColumn<double>("altitude");
    QTest::addColumn<bool>("result");


   QTest::newRow("all_correct") << 0.0 << 0.0 << 0.0 << true;

   QTest::newRow("lat1") << -90.001 << 0.0 << 0.0 << false;
   QTest::newRow("lat2") << -90.0 << 0.0 << 0.0 << true;
   QTest::newRow("lat3") << -89.999 << 0.0 << 0.0 << true;
   QTest::newRow("lat4") << 90.001 << 0.0 << 0.0 << false;
   QTest::newRow("lat5") << 90.0 << 0.0 << 0.0 << true;
   QTest::newRow("lat6") << 89.999 << 0.0 << 0.0 << true;
   QTest::newRow("lat7") << -200.0 << 0.0 << 0.0 << false;
   QTest::newRow("lat8") << 200.0 << 0.0 << 0.0 << false;
   QTest::newRow("lat9") << -46.8475 << 0.0 << 0.0 << true;
   QTest::newRow("lat10") << 28.63643 << 0.0 << 0.0 << true;

   QTest::newRow("long1") << 0.0 << -180.001 << 0.0 << false;
   QTest::newRow("long2") << 0.0 << -180.00 << 0.0 << true;
   QTest::newRow("long3") << 0.0 << -179.999 << 0.0 << true;
   QTest::newRow("long4") << 0.0 << 180.001 << 0.0 << false;
   QTest::newRow("long5") << 0.0 << 180.000 << 0.0 << true;
   QTest::newRow("long6") << 0.0 << 179.9999 << 0.0 << true;
   QTest::newRow("long7") << 0.0 << -300.675 << 0.0 << false;
   QTest::newRow("long8") << 0.0 << 50494.393 << 0.0 << false;
   QTest::newRow("long9") << 0.0 << -129.9484 << 0.0 << true;
   QTest::newRow("long10") << 0.0 << 34.3939 << 0.0 << true;

    QTest::newRow("alt1") << 0.0 << 0.0 << -2000.034 << true;
    QTest::newRow("alt1") << 0.0 << 0.0 << 233455.0443 << true;
}

void Coordinate::t_qgc_type()
{
    QFETCH(double, latitude);
    QFETCH(double, longitude);
    QFETCH(double, altitude);
    QFETCH(QGeoCoordinate::CoordinateType, result);

    QCOMPARE(qgeocoordinate->type(),QGeoCoordinate::InvalidCoordinate);
    qgeocoordinate->setLatitude(latitude);
    QCOMPARE(qgeocoordinate->type(),QGeoCoordinate::InvalidCoordinate);
    qgeocoordinate->setLongitude(longitude);
    if(result==QGeoCoordinate::Coordinate3D)
    {
        QCOMPARE(qgeocoordinate->type(),QGeoCoordinate::Coordinate2D);
        qgeocoordinate->setAltitude(altitude);
        QCOMPARE(qgeocoordinate->type(),result);
    }
    else
    {
        QCOMPARE(qgeocoordinate->type(),result);
        qgeocoordinate->setAltitude(altitude);
        QCOMPARE(qgeocoordinate->type(),result);
    }

}

void Coordinate::t_qgc_type_data()
{


    QTest::addColumn<double>("latitude");
    QTest::addColumn<double>("longitude");
    QTest::addColumn<double>("altitude");
    QTest::addColumn<QGeoCoordinate::CoordinateType>("result");

    QTest::newRow("all_correct") << 0.0 << 0.0 << 0.0 << QGeoCoordinate::Coordinate3D;
    QTest::newRow("lat1") << -90.001 << 0.0 << 0.0 << QGeoCoordinate::InvalidCoordinate;
    QTest::newRow("lat2") << -90.0 << 0.0 << 0.0 << QGeoCoordinate::Coordinate3D;
    QTest::newRow("long1") << 0.0 << -180.001 << 0.0 << QGeoCoordinate::InvalidCoordinate;
    QTest::newRow("long2") << 0.0 << -180.00 << 0.0 << QGeoCoordinate::Coordinate3D;
    QTest::newRow("alt1") << 0.0 << 0.0 << -2000.034 << QGeoCoordinate::Coordinate3D;
    QTest::newRow("alt1") << 0.0 << 0.0 << 233455.0443 << QGeoCoordinate::Coordinate3D;
}

void Coordinate::t_qgc_azimuthTo(){

    QFETCH(double, latitude);
    QFETCH(double, longitude);
    QFETCH(double, latitude2);
    QFETCH(double, longitude2);
    QFETCH(double, result);

    qgeocoordinate->setLatitude(latitude);
    qgeocoordinate->setLongitude(longitude);

    QGeoCoordinate *qgeocoordinate2 = new QGeoCoordinate(latitude2,longitude2);
    QCOMPARE(qgeocoordinate->azimuthTo(*qgeocoordinate2),result);
}

void Coordinate::t_qgc_azimuthTo_data()
{
    QTest::addColumn<double>("latitude");
    QTest::addColumn<double>("longitude");
    QTest::addColumn<double>("latitude2");
    QTest::addColumn<double>("longitude2");
    QTest::addColumn<double>("result");

    QTest::newRow("invalid0") << -12220.0 << 0.0 << 0.0 << 0.0 << 0.0;
    QTest::newRow("invalid1") << 0.0 << 181.0 << 0.0 << 0.0 << 0.0;
    QTest::newRow("invalid2") << 0.0 << 0.0 << 91.0 << 0.0 << 0.0;
    QTest::newRow("invalid3") << 0.0 << 0.0 << 0.0 << -190.0 << 0.0;

    QTest::newRow("correct0") << 0.0 << 0.0 << 0.0 << 0.0 << 0.0;
    QTest::newRow("correct1") << 90.0 << 0.0 << 0.0 << 0.0 << 180.0;
    QTest::newRow("correct2") << 0.0 << 180.0 << 0.0 << 0.0 << 270.0;
    QTest::newRow("correct3") << 0.0 << 0.0 << 90.0 << 0.0 << 0.0;
    QTest::newRow("correct4") << 0.0 << 0.0 << 0.0 << 180.0 << 90.0;
    QTest::newRow("correct5") << -90.0 << 0.0 << 0.0 << 0.0 << 0.0;
    QTest::newRow("correct6") << 0.0 << -180.0 << 0.0 << 0.0 << 90.0;
    QTest::newRow("correct7") << 0.0 << 0.0 << -90.0 << 0.0 << 180.0;
    QTest::newRow("correct8") << 0.0 << 0.0 << 0.0 << -180.0 << 270.0;
    QTest::newRow("correct9") << 45.0 << 90.0 << -45.0 << -90.0 << 270.0;

}

void Coordinate::t_qgc_distanceTo(){

    QFETCH(double, latitude);
    QFETCH(double, longitude);
    QFETCH(double, latitude2);
    QFETCH(double, longitude2);
    QFETCH(bool, result);

    qgeocoordinate->setLatitude(latitude);
    qgeocoordinate->setLongitude(longitude);

    QGeoCoordinate *qgeocoordinate2 = new QGeoCoordinate(latitude2,longitude2);

    if (result)
    {
        QVERIFY(qgeocoordinate->distanceTo(*qgeocoordinate2)!=0.0);
    }
    else
    {
        QCOMPARE(qgeocoordinate->azimuthTo(*qgeocoordinate2),0.0);
    }
}

void Coordinate::t_qgc_distanceTo_data()
{
    QTest::addColumn<double>("latitude");
    QTest::addColumn<double>("longitude");
    QTest::addColumn<double>("latitude2");
    QTest::addColumn<double>("longitude2");
    QTest::addColumn<bool>("result");

    QTest::newRow("invalid0") << -12220.0 << 0.0 << 0.0 << 0.0 << false;
    QTest::newRow("invalid1") << 0.0 << 181.0 << 0.0 << 0.0 << false;
    QTest::newRow("invalid2") << 0.0 << 0.0 << 91.0 << 0.0 << false;
    QTest::newRow("invalid3") << 0.0 << 0.0 << 0.0 << -190.0 << false;

    QTest::newRow("correct1") << 90.0 << 0.0 << 0.0 << 0.0 << true;
    QTest::newRow("correct2") << 0.0 << 180.0 << 0.0 << 0.0 << true;
    QTest::newRow("correct3") << 0.0 << 0.0 << 90.0 << 0.0 << true;
    QTest::newRow("correct4") << 0.0 << 0.0 << 0.0 << 180.0 << true;
    QTest::newRow("correct5") << -90.0 << 0.0 << 0.0 << 0.0 << true;
    QTest::newRow("correct6") << 0.0 << -180.0 << 0.0 << 0.0 << true;
    QTest::newRow("correct7") << 0.0 << 0.0 << -90.0 << 0.0 << true;
    QTest::newRow("correct8") << 0.0 << 0.0 << 0.0 << -180.0 << true;
    QTest::newRow("correct9") << 45.0 << 90.0 << -45.0 << -90.0 << true;
}

void Coordinate::t_qgc_toString()
{
    QFETCH(double, latitude);
    QFETCH(double, longitude);
    QFETCH(double, altitude);
    QFETCH(QGeoCoordinate::CoordinateFormat, format);
    QFETCH(QString, result);

    qgeocoordinate->setLatitude(latitude);
    qgeocoordinate->setLongitude(longitude);
    qgeocoordinate->setAltitude(altitude);

    QCOMPARE(qgeocoordinate->toString(format),result);

}

void Coordinate::t_qgc_toString_data()
{
    QTest::addColumn<double>("latitude");
    QTest::addColumn<double>("longitude");
    QTest::addColumn<double>("altitude");
    QTest::addColumn<QGeoCoordinate::CoordinateFormat>("format");
    QTest::addColumn<QString>("result");


    QTest::newRow("invalid1") << 300.0 << 0.0 << 230.0 << QGeoCoordinate::Degrees << "";
    QTest::newRow("invalid2") << 0.0 << 1880.0 << 340.0 << QGeoCoordinate::DegreesWithHemisphere << "";
    QTest::newRow("invalid3") << -2330.0 << 0.0 << -670.0 << QGeoCoordinate::DegreesMinutes << "";
    QTest::newRow("invalid4") << 0.0 << -321340.0 << 60.6546 << QGeoCoordinate::DegreesMinutesWithHemisphere << "";
    QTest::newRow("invallid5") << 3240.340 << 3230.0 << 0.2324 << QGeoCoordinate::DegreesMinutesSeconds << "";
    QTest::newRow("invallid6") << -3240.340 << -323330.0435 << 0.2324 << QGeoCoordinate::DegreesMinutesSecondsWithHemisphere << "";

    QTest::newRow("degrees1") << 0.0 << 0.0 << 0.0 << QGeoCoordinate::Degrees << "0.00000°, 0.00000°, 0m";
    QTest::newRow("degrees2") << 40.04569 << -130.54 << -200.0 << QGeoCoordinate::Degrees << "40.04569°, -130.54000°, -200m";
    QTest::newRow("degrees3") << -90.0 << 180.0 << 204.3 << QGeoCoordinate::Degrees << "-90.00000°, 180.00000°, 204.3m";
    QTest::newRow("degrees4") << 30.230 << -24.857834 << 45.32 << QGeoCoordinate::Degrees << "30.23000°, -24.85783°, 45.32m";
    QTest::newRow("degrees5") << -9.32302 << 139.43423 << -132.545 << QGeoCoordinate::Degrees << "-9.32302°, 139.43423°, -132.545m";

    QTest::newRow("degreesHemisphere1") << 0.0 << 0.0 << 0.0 << QGeoCoordinate::DegreesWithHemisphere << "0.00000°, 0.00000°, 0m";
    QTest::newRow("degreesHemisphere2") << 40.04569 << -130.54 << -200.0 << QGeoCoordinate::DegreesWithHemisphere << "40.04569° N, 130.54000° W, -200m";
    QTest::newRow("degreesHemisphere3") << -90.0 << 180.0 << 204.3 << QGeoCoordinate::DegreesWithHemisphere << "90.00000° S, 180.00000° E, 204.3m";
    QTest::newRow("degreesHemisphere4") << 30.230 << -24.857834 << 45.32 << QGeoCoordinate::DegreesWithHemisphere << "30.23000° N, 24.85783° W, 45.32m";
    QTest::newRow("degreesHemisphere5") << -9.32302 << 139.43423 << -132.545 << QGeoCoordinate::DegreesWithHemisphere << "9.32302° S, 139.43423° E, -132.545m";

    QTest::newRow("degreesMinutes1") << 0.0 << 0.0 << 0.0 << QGeoCoordinate::DegreesMinutes << "0° 0.000', 0° 0.000', 0m";
    QTest::newRow("degreesMinutes2") << 40.04569 << -130.54 << -200.0 << QGeoCoordinate::DegreesMinutes << "40° 2.741', -130° 32.400', -200m";
    QTest::newRow("degreesMinutes3") << -90.0 << 180.0 << 204.3 << QGeoCoordinate::DegreesMinutes << "-90° 0.000', 180° 0.000', 204.3m";
    QTest::newRow("degreesMinutes4") << 30.230 << -24.857834 << 45.32 << QGeoCoordinate::DegreesMinutes << "30° 13.800', -24° 51.470', 45.32m";
    QTest::newRow("degreesMinutes5") << -9.32302 << 139.43423 << -132.545 << QGeoCoordinate::DegreesMinutes << "-9° 19.381', 139° 26.054', -132.545m";

    QTest::newRow("degreesMinutesHemisphere1") << 0.0 << 0.0 << 0.0 << QGeoCoordinate::DegreesMinutesWithHemisphere << "0° 0.000', 0° 0.000', 0m";
    QTest::newRow("degreesMinutesHemisphere2") << 40.04569 << -130.54 << -200.0 << QGeoCoordinate::DegreesMinutesWithHemisphere << "40° 2.741' N, 130° 32.400' W, -200m";
    QTest::newRow("degreesMinutesHemisphere3") << -90.0 << 180.0 << 204.3 << QGeoCoordinate::DegreesMinutesWithHemisphere << "90° 0.000' S, 180° 0.000' E, 204.3m";
    QTest::newRow("degreesMinutesHemisphere4") << 30.230 << -24.857834 << 45.32 << QGeoCoordinate::DegreesMinutesWithHemisphere << "30° 13.800' N, 24° 51.470' W, 45.32m";
    QTest::newRow("degreesMinutesHemisphere5") << -9.32302 << 139.43423 << -132.545 << QGeoCoordinate::DegreesMinutesWithHemisphere << "9° 19.381' S, 139° 26.054' E, -132.545m";

    QTest::newRow("degreesMinutesSeconds1") << 0.0 << 0.0 << 0.0 << QGeoCoordinate::DegreesMinutesSeconds << "0° 0' 0.0\", 0° 0' 0.0\", 0m";
    QTest::newRow("degreesMinutesSeconds2") << 40.04569 << -130.54 << -200.0 << QGeoCoordinate::DegreesMinutesSeconds << "40° 2' 44.5\", -130° 32' 24.0\", -200m";
    QTest::newRow("degreesMinutesSeconds3") << -90.0 << 180.0 << 204.3 << QGeoCoordinate::DegreesMinutesSeconds << "-90° 0' 0.0\", 180° 0' 0.0\", 204.3m";
    QTest::newRow("degreesMinutesSeconds4") << 30.230 << -24.857834 << 45.32 << QGeoCoordinate::DegreesMinutesSeconds << "30° 13' 48.0\", -24° 51' 28.2\", 45.32m";
    QTest::newRow("degreesMinutesSeconds5") << -9.32302 << 139.43423 << -132.545 << QGeoCoordinate::DegreesMinutesSeconds << "-9° 19' 22.9\", 139° 26' 3.2\", -132.545m";

    QTest::newRow("degreesMinutesSecondsHemisphere1") << 0.0 << 0.0 << 0.0 << QGeoCoordinate::DegreesMinutesSecondsWithHemisphere << "0° 0' 0.0\", 0° 0' 0.0\", 0m";
    QTest::newRow("degreesMinutesSecondsHemisphere2") << 40.04569 << -130.54 << -200.0 << QGeoCoordinate::DegreesMinutesSecondsWithHemisphere << "40° 2' 44.5\" N, 130° 32' 24.0\" W, -200m";
    QTest::newRow("degreesMinutesSecondsHemisphere3") << -90.0 << 180.0 << 204.3 << QGeoCoordinate::DegreesMinutesSecondsWithHemisphere << "90° 0' 0.0\" S, 180° 0' 0.0\" E, 204.3m";
    QTest::newRow("degreesMinutesSecondsHemisphere4") << 30.230 << -24.857834 << 45.32 << QGeoCoordinate::DegreesMinutesSecondsWithHemisphere << "30° 13' 48.0\" N, 24° 51' 28.2\" W, 45.32m";
    QTest::newRow("degreesMinutesSecondsHemisphere5") << -9.32302 << 139.43423 << -132.545 << QGeoCoordinate::DegreesMinutesSecondsWithHemisphere << "9° 19' 22.9\" S, 139° 26' 3.2\" E, -132.545m";

}

void Coordinate::t_qgc_operators()
{
    //Create a copy and see that they are the same
    QGeoCoordinate *qgeocoordinatecopy = new QGeoCoordinate (*qgeocoordinate);
    QVERIFY(qgeocoordinate->operator ==(*qgeocoordinatecopy));
    QVERIFY(!qgeocoordinate->operator !=(*qgeocoordinatecopy));

    QFETCH(double, latitude);
    QFETCH(double, longitude);
    QFETCH(double, altitude);
    QFETCH(double, latitude2);
    QFETCH(double, longitude2);
    QFETCH(double, altitude2);
    QFETCH(bool, result);

    qgeocoordinate->setLatitude(latitude);
    qgeocoordinate->setLongitude(longitude);
    qgeocoordinate->setAltitude(altitude);

    qgeocoordinatecopy->setLatitude(latitude2);
    qgeocoordinatecopy->setLongitude(longitude2);
    qgeocoordinatecopy->setAltitude(altitude2);

    QCOMPARE(qgeocoordinate->operator ==(*qgeocoordinatecopy),result);
    QCOMPARE(qgeocoordinate->operator !=(*qgeocoordinatecopy),!result);

    //Assign one address to the other and test that they are the same again
    *qgeocoordinatecopy = qgeocoordinatecopy->operator =(*qgeocoordinate);
    QVERIFY(qgeocoordinate->operator ==(*qgeocoordinatecopy));
    QVERIFY(!qgeocoordinate->operator !=(*qgeocoordinatecopy));

    delete qgeocoordinatecopy;

}

void Coordinate::t_qgc_operators_data()
{
    QTest::addColumn<double>("latitude");
    QTest::addColumn<double>("longitude");
    QTest::addColumn<double>("altitude");
    QTest::addColumn<double>("latitude2");
    QTest::addColumn<double>("longitude2");
    QTest::addColumn<double>("altitude2");
    QTest::addColumn<bool>("result");

   QTest::newRow("operators1") << 34.32 << 123.5656 << 23.53 << 54.9209 << -23.345 << 126.32 << false ;
   QTest::newRow("operators2") << 90.0 << 123.5656 << 54.9209 << 90.0 << -23.345 << 54.9209 << true ;
   QTest::newRow("operators3") << -90.0 << 123.5656 << 1456.65 << -90.0 << -123.345 << 1456.65 << true;
   QTest::newRow("operators4") << 34.32 << 123.5656 << 23.53 << 34.32 << 123.5656 << 23.53 << true;
   QTest::newRow("operators5") << 34.32 << 123.5656 << 23.53 << 34.32 << 123.5656 << 126.32 << false ;
   QTest::newRow("operators6") << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << true ;

}

QTEST_MAIN(Coordinate);
