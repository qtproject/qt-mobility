#include "tst_qgeoplace.h"

QTM_USE_NAMESPACE

Place::Place()
{
}

void Place::initTestCase()
{

}

void Place::cleanupTestCase()
{

}

void Place::init()
{
    qgeoaddress = new QGeoAddress();
    qgeocoordinate = new QGeoCoordinate();
    qgeoplace = new QGeoPlace();
    qgeoboundingbox = new QGeoBoundingBox();

}

void Place::cleanup()
{
    delete qgeoaddress;
    delete qgeocoordinate;
    delete qgeoplace;
    delete qgeoboundingbox;


}

void Place::t_qgp_constructor()
{
    QCOMPARE(typeid(*qgeoplace).name(),"N10QtMobility9QGeoPlaceE");
}

void Place::t_qgp_constructorCopy()
{
    QGeoPlace *qgeoplacecopy = new QGeoPlace (*qgeoplace);
    QCOMPARE(typeid(*qgeoplace).name(),typeid(*qgeoplacecopy).name());
    delete qgeoplacecopy;
}

void Place::t_qgp_address()
{
    qgeoaddress->setCity("Berlin");
    qgeoaddress->setCountry("Germany");
    qgeoaddress->setCountryCode("DEU");
    qgeoaddress->setDistrict("Mitte");
    qgeoaddress->setPostcode("10115");
    qgeoaddress->setStreet("Invalidenstrasse");

    qgeoplace->setAddress(*qgeoaddress);

    QCOMPARE(qgeoplace->address(),*qgeoaddress);

    qgeoaddress->setPostcode("10125");
    QEXPECT_FAIL("","Test Correct", Continue);
    QCOMPARE(qgeoplace->address(),*qgeoaddress);
}

void Place::t_qgp_coordinate()
{
    qgeocoordinate->setLatitude(13.3851);
    qgeocoordinate->setLongitude(52.5312);
    qgeocoordinate->setAltitude(134.23);

    qgeoplace->setCoordinate(*qgeocoordinate);

    QCOMPARE(qgeoplace->coordinate(),*qgeocoordinate);

    qgeocoordinate->setAltitude(0);
    QEXPECT_FAIL("","Test Correct", Continue);
    QCOMPARE(qgeoplace->coordinate(),*qgeocoordinate);
}

void Place::t_qgp_viewport()
{
    qgeocoordinate->setLatitude(13.3851);
    qgeocoordinate->setLongitude(52.5312);

    QGeoBoundingBox *qgeoboundingboxcopy= new QGeoBoundingBox(*qgeocoordinate,0.4,0.4);

    qgeoplace->setViewport(*qgeoboundingboxcopy);

    QCOMPARE(qgeoplace->viewport(),*qgeoboundingboxcopy);

    qgeoboundingboxcopy->setHeight(1);

    QEXPECT_FAIL("","Test Correct", Continue);
    QCOMPARE(qgeoplace->viewport(),*qgeoboundingboxcopy);

    delete qgeoboundingboxcopy;
}

void Place::t_qgp_isLandmark()
{
    QVERIFY(!qgeoplace->isLandmark());

    qgeoplace->setAddress(*qgeoaddress);
    QVERIFY(!qgeoplace->isLandmark());

    qgeoplace->setCoordinate(*qgeocoordinate);
    QVERIFY(!qgeoplace->isLandmark());

    //qgeoaddress->setCity("Berlin");
    qgeoaddress->setCountryCode("DEU");
    qgeocoordinate->setLatitude(13.4109);
    qgeocoordinate->setLongitude(52.5221);

    qgeoplace->setAddress(*qgeoaddress);
    qgeoplace->setCoordinate(*qgeocoordinate);
    QVERIFY(qgeoplace->isLandmark());

}

void Place::t_qgp_operators()
{
    QGeoAddress *qgeoaddresscopy = new QGeoAddress();
    qgeoaddresscopy->setCity("Berlin");
    qgeoaddresscopy->setCountry("Germany");
    qgeoaddresscopy->setCountryCode("DEU");

    QGeoCoordinate *qgeocoordinatecopy = new QGeoCoordinate (32.324 , 41.324 , 24.55);

    qgeoaddress->setCity("Madrid");
    qgeoaddress->setCountry("Spain");
    qgeoaddress->setCountryCode("SPA");

    qgeocoordinate->setLatitude(21.3434);
    qgeocoordinate->setLongitude(38.43443);
    qgeocoordinate->setAltitude(634.21);

    qgeoplace->setAddress(*qgeoaddress);
    qgeoplace->setCoordinate(*qgeocoordinate);

    //Create a copy and see that they are the same
    QGeoPlace *qgeoplacecopy = new QGeoPlace (*qgeoplace);
    QVERIFY(qgeoplace->operator ==(*qgeoplacecopy));
    QVERIFY(!qgeoplace->operator !=(*qgeoplacecopy));

    //Modify one and test if they are different
   qgeoplacecopy->setAddress(*qgeoaddresscopy);
   QVERIFY(!qgeoplace->operator ==(*qgeoplacecopy));
   QVERIFY(qgeoplace->operator !=(*qgeoplacecopy));
   qgeoplacecopy->setCoordinate(*qgeocoordinatecopy);
   QVERIFY(!qgeoplace->operator ==(*qgeoplacecopy));
   QVERIFY(qgeoplace->operator !=(*qgeoplacecopy));

   //delete qgeoplacecopy;
   //Asign and test that they are the same
    *qgeoplacecopy = qgeoplacecopy->operator =(*qgeoplace);
    QVERIFY(qgeoplace->operator ==(*qgeoplacecopy));
    QVERIFY(!qgeoplace->operator !=(*qgeoplacecopy));

    delete qgeocoordinatecopy;
    delete qgeoaddresscopy;
    delete qgeoplacecopy;

}

QTEST_MAIN(Place);
