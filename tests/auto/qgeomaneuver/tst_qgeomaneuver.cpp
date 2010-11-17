#include "tst_qgeomaneuver.h"

QTM_USE_NAMESPACE

Maneuver::Maneuver()
{
}

void Maneuver::initTestCase()
{

}

void Maneuver::cleanupTestCase()
{

}

void Maneuver::init()
{

    qgeomaneuver = new QGeoManeuver();
}

void Maneuver::cleanup()
{
    delete qgeomaneuver;
}

void Maneuver::t_qgm_constructor()
{
    qgeomaneuver = new QGeoManeuver ();
    QCOMPARE(typeid(*qgeomaneuver).name(),"N10QtMobility12QGeoManeuverE");

}

void Maneuver::t_qgm_constructorCopy()
{
    QGeoManeuver *qgeomaneuvercopy = new QGeoManeuver (*qgeomaneuver);
    QCOMPARE(typeid(*qgeomaneuver).name(),typeid(*qgeomaneuvercopy).name());
    delete qgeomaneuvercopy;

}

void Maneuver::t_qgm_direction()
{
    QFETCH(QGeoManeuver::InstructionDirection,direction);

    qgeomaneuver->setDirection(direction);

    QCOMPARE(qgeomaneuver->direction(),direction);
}
void Maneuver::t_qgm_direction_data()
{
    QTest::addColumn<QGeoManeuver::InstructionDirection>("direction");

    QTest::newRow("instruction1") << QGeoManeuver::NoDirection;
    QTest::newRow("instruction2") << QGeoManeuver::DirectionForward;
    QTest::newRow("instruction3") << QGeoManeuver::DirectionBearRight;
    QTest::newRow("instruction4") << QGeoManeuver::DirectionLightRight;
    QTest::newRow("instruction5") << QGeoManeuver::DirectionRight;
    QTest::newRow("instruction6") << QGeoManeuver::DirectionHardRight;
    QTest::newRow("instruction7") << QGeoManeuver::DirectionUTurnRight;
    QTest::newRow("instruction8") << QGeoManeuver::DirectionUTurnLeft;
    QTest::newRow("instruction9") << QGeoManeuver::DirectionHardLeft;
    QTest::newRow("instruction10") << QGeoManeuver::DirectionLeft;
    QTest::newRow("instruction11") << QGeoManeuver::DirectionLightLeft;
    QTest::newRow("instruction12") << QGeoManeuver::DirectionBearLeft;
}

void Maneuver::t_qgm_distanceToNextInstruction()
{
    double distance = 0.0;
    qgeomaneuver->setDistanceToNextInstruction(distance);

    QCOMPARE (qgeomaneuver->distanceToNextInstruction(),distance);

    distance = -3423.4324;

    QEXPECT_FAIL("", "Expected Fail", Continue);
    QCOMPARE (qgeomaneuver->distanceToNextInstruction(),distance);

    qgeomaneuver->setDistanceToNextInstruction(distance);
    QCOMPARE (qgeomaneuver->distanceToNextInstruction(),distance);
}

void Maneuver::t_qgm_instructionText()
{
    QString text = "After 50m turn left";

    qgeomaneuver->setInstructionText(text);

    QCOMPARE (qgeomaneuver->instructionText(),text);

    text="After 40m, turn left";
    QEXPECT_FAIL("", "Expected Fail", Continue);
    QCOMPARE (qgeomaneuver->instructionText(),text);

}

void Maneuver::t_qgm_position()
{
    QFETCH(double, latitude);
    QFETCH(double, longitude);

    qgeocoordinate = new QGeoCoordinate (latitude,longitude);

    qgeomaneuver->setPosition(*qgeocoordinate);

    QCOMPARE(qgeomaneuver->position(),*qgeocoordinate);

    delete qgeocoordinate;
}

void Maneuver::t_qgm_position_data()
{
    QTest::addColumn<double>("latitude");
    QTest::addColumn<double>("longitude");

    QTest::newRow("invalid0") << -12220.0 << 0.0;
    QTest::newRow("invalid1") << 0.0 << 181.0;

    QTest::newRow("correct0") << 0.0 << 0.0;
    QTest::newRow("correct1") << 90.0 << 0.0;
    QTest::newRow("correct2") << 0.0 << 180.0;
    QTest::newRow("correct3") << -90.0 << 0.0;
    QTest::newRow("correct4") << 0.0 << -180.0;
    QTest::newRow("correct5") << 45.0 << 90.0;
}

void Maneuver::t_qgm_timeToNextInstruction()
{
    int time = 0;
    qgeomaneuver->setTimeToNextInstruction(time);

    QCOMPARE (qgeomaneuver->timeToNextInstruction(),time);

    time = 35;

    QEXPECT_FAIL("", "Expected Fail", Continue);
    QCOMPARE (qgeomaneuver->timeToNextInstruction(),time);

    qgeomaneuver->setTimeToNextInstruction(time);
    QCOMPARE (qgeomaneuver->timeToNextInstruction(),time);
}

void Maneuver::t_qgm_waypoint()
{
    QFETCH(double, latitude);
    QFETCH(double, longitude);

    qgeocoordinate = new QGeoCoordinate (latitude,longitude);

    qgeomaneuver->setWaypoint(*qgeocoordinate);

    QCOMPARE(qgeomaneuver->waypoint(),*qgeocoordinate);

    qgeocoordinate->setLatitude(30.3);
    QEXPECT_FAIL("", "Expected Fail", Continue);
    QCOMPARE(qgeomaneuver->waypoint(),*qgeocoordinate);


    delete qgeocoordinate;
}
void Maneuver::t_qgm_waypoint_data()
{
    QTest::addColumn<double>("latitude");
    QTest::addColumn<double>("longitude");

    QTest::newRow("invalid0") << -12220.0 << 0.0;
    QTest::newRow("invalid1") << 0.0 << 181.0;

    QTest::newRow("correct0") << 0.0 << 0.0;
    QTest::newRow("correct1") << 90.0 << 0.0;
    QTest::newRow("correct2") << 0.0 << 180.0;
    QTest::newRow("correct3") << -90.0 << 0.0;
    QTest::newRow("correct4") << 0.0 << -180.0;
    QTest::newRow("correct5") << 45.0 << 90.0;
}

void Maneuver::t_qgm_isValid()
{
    QVERIFY(!qgeomaneuver->isValid());
    qgeomaneuver->setDirection(QGeoManeuver::DirectionBearLeft);
    QVERIFY(qgeomaneuver->isValid());
}

void Maneuver::t_qgm_operators(){

    QGeoManeuver *qgeomaneuvercopy = new QGeoManeuver(*qgeomaneuver);

    QVERIFY(qgeomaneuver->operator ==(*qgeomaneuvercopy));
    QVERIFY(!qgeomaneuver->operator !=(*qgeomaneuvercopy));

    qgeomaneuver->setDirection(QGeoManeuver::DirectionBearLeft);
    qgeomaneuver->setInstructionText("Turn left in 50m");
    qgeomaneuver->setTimeToNextInstruction(60);
    qgeomaneuver->setDistanceToNextInstruction(560.45);

    qgeomaneuvercopy->setDirection(QGeoManeuver::DirectionForward);
    qgeomaneuvercopy->setInstructionText("Turn left in 80m");
    qgeomaneuvercopy->setTimeToNextInstruction(70);
    qgeomaneuvercopy->setDistanceToNextInstruction(56065.45);

    QVERIFY(!(qgeomaneuver->operator ==(*qgeomaneuvercopy)));
    QVERIFY(qgeomaneuver->operator !=(*qgeomaneuvercopy));

    *qgeomaneuvercopy = qgeomaneuvercopy->operator =(*qgeomaneuver);
    QVERIFY(qgeomaneuver->operator ==(*qgeomaneuvercopy));
    QVERIFY(!qgeomaneuver->operator !=(*qgeomaneuvercopy));

    delete qgeomaneuvercopy;
}


QTEST_MAIN(Maneuver);
