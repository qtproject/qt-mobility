#include "tst_qgeopositioninfo.h"

QTM_USE_NAMESPACE

PositionInfo::PositionInfo()
{
}

void PositionInfo::initTestCase()
{

}

void PositionInfo::cleanupTestCase()
{

}

void PositionInfo::init()
{
    qgeocoordinate = new QGeoCoordinate();
    qgeoposinfo = new QGeoPositionInfo();
}

void PositionInfo::cleanup()
{
    delete qgeocoordinate;
    delete qgeoposinfo;
}

void PositionInfo::t_qgpi_constructor()
{
    QCOMPARE(typeid(*qgeoposinfo).name(),"N10QtMobility16QGeoPositionInfoE");
    QVERIFY(!qgeoposinfo->isValid());
}

void PositionInfo::t_qgpi_constructor2()
{
    QDate *qd = new QDate (2010,2,16);
    QDateTime *qdt_valid = new QDateTime (*qd);
    QDateTime *qdt_invalid = new QDateTime ();

    QGeoCoordinate *qgeocoordinate_valid = new QGeoCoordinate(34.3 , 26.5);

    QGeoPositionInfo *qgeoposinfocopy = new QGeoPositionInfo (*qgeocoordinate,*qdt_invalid);

    QCOMPARE(typeid(*qgeoposinfo).name(),typeid(*qgeoposinfocopy).name());
    QVERIFY(!qgeoposinfocopy->isValid());

    delete qgeoposinfocopy;
    qgeoposinfocopy = new QGeoPositionInfo (*qgeocoordinate_valid,*qdt_valid);

    QCOMPARE(typeid(*qgeoposinfo).name(),typeid(*qgeoposinfocopy).name());
    QVERIFY(qgeoposinfocopy->isValid());

    delete qd;
    delete qdt_valid;
    delete qdt_invalid;
    delete qgeocoordinate_valid;
    delete qgeoposinfocopy;
}

void PositionInfo::t_qgpi_constructorcopy()
{
    QGeoPositionInfo *qgeoposinfocopy = new QGeoPositionInfo (*qgeoposinfo);
    QCOMPARE(typeid(*qgeoposinfo).name(),typeid(*qgeoposinfocopy).name());

    delete qgeoposinfocopy;
}

void PositionInfo::t_qgpi_attribute()
{
    QFETCH(QGeoPositionInfo::Attribute, attribute);
    QFETCH(double, value);

    QCOMPARE(qgeoposinfo->attribute(attribute),-1.0);
    qgeoposinfo->setAttribute(attribute, value);
    QCOMPARE(qgeoposinfo->attribute(attribute),value);
}

void PositionInfo::t_qgpi_attribute_data()
{
    QTest::addColumn<QGeoPositionInfo::Attribute>("attribute");
    QTest::addColumn<double>("value");

    QTest::newRow("attribute1") << QGeoPositionInfo::Direction << 20.2238 ;
    QTest::newRow("attribute2") << QGeoPositionInfo::GroundSpeed << 9.34 ;
    QTest::newRow("attribute3") << QGeoPositionInfo::VerticalSpeed << 0.456 ;
    QTest::newRow("attribute4") << QGeoPositionInfo::MagneticVariation << 0.001 ;
    QTest::newRow("attribute5") << QGeoPositionInfo::HorizontalAccuracy << 21.1 ;
    QTest::newRow("attribute6") << QGeoPositionInfo::VerticalAccuracy << 21.1 ;
}

void PositionInfo::t_qgpi_coordinate()
{
    qgeocoordinate->setLatitude(27.54);
    qgeocoordinate->setLongitude(-32.432432);
    qgeoposinfo->setCoordinate(*qgeocoordinate);

    QCOMPARE(qgeoposinfo->coordinate(),*qgeocoordinate);
}

void PositionInfo::t_qgpi_timestamp()
{
    QDate *qd = new QDate (2010,2,16);
    QDateTime *qdt = new QDateTime (*qd);

    qgeoposinfo->setTimestamp(*qdt);

    QCOMPARE(qgeoposinfo->timestamp(),*qdt);

    delete qd;
    delete qdt;
}

void PositionInfo::t_qgpi_hasRemoveAttribute()
{
    QFETCH(QGeoPositionInfo::Attribute, attribute);
    QFETCH(double, value);

    QVERIFY(!qgeoposinfo->hasAttribute(attribute));
    QCOMPARE(qgeoposinfo->attribute(attribute),-1.0);
    qgeoposinfo->removeAttribute(attribute);

    qgeoposinfo->setAttribute(attribute, value);
    QVERIFY(qgeoposinfo->hasAttribute(attribute));
    QCOMPARE(qgeoposinfo->attribute(attribute),value);

    qgeoposinfo->removeAttribute(attribute);
    QVERIFY(!qgeoposinfo->hasAttribute(attribute));
    QCOMPARE(qgeoposinfo->attribute(attribute),-1.0);
}

void PositionInfo::t_qgpi_hasRemoveAttribute_data()
{
    QTest::addColumn<QGeoPositionInfo::Attribute>("attribute");
    QTest::addColumn<double>("value");

    QTest::newRow("attribute1") << QGeoPositionInfo::Direction << 20.2238 ;
    QTest::newRow("attribute2") << QGeoPositionInfo::GroundSpeed << 9.34 ;
    QTest::newRow("attribute3") << QGeoPositionInfo::VerticalSpeed << 0.456 ;
    QTest::newRow("attribute4") << QGeoPositionInfo::MagneticVariation << 0.001 ;
    QTest::newRow("attribute5") << QGeoPositionInfo::HorizontalAccuracy << 21.1 ;
    QTest::newRow("attribute6") << QGeoPositionInfo::VerticalAccuracy << 21.1 ;
}

void PositionInfo::t_qgpi_isValid()
{
    QFETCH(double, latitude);
    QFETCH(double, longitude);
    QFETCH(int, year);
    QFETCH(int, month);
    QFETCH(int, day);
    QFETCH(bool, result);

    QDate *qd = new QDate (year,month,day);
    QDateTime *qdt = new QDateTime (*qd);

    qgeocoordinate->setLatitude(latitude);
    qgeocoordinate->setLongitude(longitude);

    QVERIFY(!qgeoposinfo->isValid());
    qgeoposinfo->setCoordinate(*qgeocoordinate);
    qgeoposinfo->setTimestamp(*qdt);
    QCOMPARE(qgeoposinfo->isValid(),result);

    delete qd;
    delete qdt;
}

void PositionInfo::t_qgpi_isValid_data()
{
    QTest::addColumn<double>("latitude");
    QTest::addColumn<double>("longitude");
    QTest::addColumn<int>("year");
    QTest::addColumn<int>("month");
    QTest::addColumn<int>("day");
    QTest::addColumn<bool>("result");

    QTest::newRow("valid1") << -90.1 << 0.0 << 2010 << 12 << 12 << false;
    QTest::newRow("valid2") << 90.1 << 0.0 << 2010 << 12 << 12 << false;
    QTest::newRow("valid3") << -89.1 << -180.10 << 2010 << 12 << 12 << false;
    QTest::newRow("valid4") << -89.1 << 180.10 << 2010 << 12 << 12 << false;
    QTest::newRow("valid5") << -89.1 << -160.10 << 2010 << 15 << 12 << false;
    QTest::newRow("valid6") << -89.1 << -160.10 << 2010 << 11 << 43 << false;
    QTest::newRow("valid7") << -89.1 << -160.10 << 2010 << 10 << 12 << true;
    QTest::newRow("valid8") << -90.0 << -180.0 << 2010 << 2 << 28 << true;
    QTest::newRow("valid1") << -892.1 << -160.10 << 2010 << 15 << 12 << false;
    QTest::newRow("valid1") << -892.1 << -11260.10 << 23010 << 315 << 132 << false;
}

void PositionInfo::t_qgpi_operators()
{
    QFETCH(double, latitude);
    QFETCH(double, longitude);
    QFETCH(int, year);
    QFETCH(int, month);
    QFETCH(int, day);
    QFETCH(double, latitude2);
    QFETCH(double, longitude2);
    QFETCH(int, year2);
    QFETCH(int, month2);
    QFETCH(int, day2);
    QFETCH(bool, result);

    QDate *qd = new QDate (year,month,day);
    QDateTime *qdt = new QDateTime (*qd);

    qgeocoordinate->setLatitude(latitude);
    qgeocoordinate->setLongitude(longitude);

    qgeoposinfo->setCoordinate(*qgeocoordinate);
    qgeoposinfo->setTimestamp(*qdt);

    QDate *qd2 = new QDate (year2,month2,day2);
    QDateTime *qdt2 = new QDateTime (*qd2);

    QGeoCoordinate *qgeocoordinate2 = new QGeoCoordinate (latitude2,longitude2);
    QGeoPositionInfo *qgeoposinfo2 = new QGeoPositionInfo (*qgeocoordinate2,*qdt2);

    QCOMPARE(qgeoposinfo->operator ==(*qgeoposinfo2),result);
    QCOMPARE(!qgeoposinfo->operator !=(*qgeoposinfo2),result);

    qgeoposinfo->setAttribute(QGeoPositionInfo::Direction, 23);
    QVERIFY(!qgeoposinfo->operator ==(*qgeoposinfo2));

    *qgeoposinfo2 = qgeoposinfo2->operator =(*qgeoposinfo);
    QVERIFY(qgeoposinfo->operator ==(*qgeoposinfo2));
    QVERIFY(!qgeoposinfo->operator !=(*qgeoposinfo2));

    delete qd;
    delete qdt;
    delete qd2;
    delete qdt2;
    delete qgeocoordinate2;
    delete qgeoposinfo2;

}

void PositionInfo::t_qgpi_operators_data()
{
    QTest::addColumn<double>("latitude");
    QTest::addColumn<double>("longitude");
    QTest::addColumn<int>("year");
    QTest::addColumn<int>("month");
    QTest::addColumn<int>("day");
    QTest::addColumn<double>("latitude2");
    QTest::addColumn<double>("longitude2");
    QTest::addColumn<int>("year2");
    QTest::addColumn<int>("month2");
    QTest::addColumn<int>("day2");
    QTest::addColumn<bool>("result");

    QTest::newRow("operators1") << -80.1 << 0.0 << 2010 << 12 << 12 << -80.1 << 0.0 << 2010 << 12 << 12 << true;
    QTest::newRow("operators2") << -80.1 << 0.0 << 2010 << 13 << 12 << -80.1 << 0.0 << 2010 << 13 << 12 << true;
    QTest::newRow("operators3") << -2380.1 << 0.0 << 2010 << 1 << 12 << -820.1 << 0.0 << 2010 << 1 << 12 << false;
    QTest::newRow("operators4") << -10.13 << 0.0 << 2010 << 12 << 12 << -80.1 << 0.0 << 2010 << 12 << 12 << false;
    QTest::newRow("operators5") << -80.1 << 21.320 << 2010 << 12 << 12 << -80.1 << 0.0 << 2010 << 12 << 12 << false;
    QTest::newRow("operators6") << -80.1 << 0.0 << 21010 << 12 << 12 << -80.1 << 0.0 << 2010 << 12 << 12 << false;
    QTest::newRow("operators7") << -80.1 << 0.0 << 2010 << 11 << 12 << -80.1 << 0.0 << 2010 << 12 << 12 << false;
    QTest::newRow("operators8") << -80.1 << 0.0 << 2010 << 12 << 27 << -80.1 << 0.0 << 2010 << 12 << 12 << false;
    QTest::newRow("operators9") << -80.1 << 0.0 << 2010 << 12 << 12 << 14.3432 << 0.0 << 2010 << 12 << 12 << false;
    QTest::newRow("operators10") << -80.1 << 0.0 << 2010 << 12 << 12 << -80.1 << -34.545 << 2010 << 12 << 12 << false;
    QTest::newRow("operators11") << -80.1 << 0.0 << 2010 << 12 << 12 << -80.1 << 0.0 << 1900 << 12 << 12 << false;
    QTest::newRow("operators12") << -80.1 << 0.0 << 2010 << 12 << 12 << -80.1 << 0.0 << 2010 << 2 << 12 << false;
    QTest::newRow("operators13") << -80.1 << 0.0 << 2010 << 12 << 12 << -80.1 << 0.0 << 2010 << 12 << 23 << false;
}

QTEST_MAIN(PositionInfo);
