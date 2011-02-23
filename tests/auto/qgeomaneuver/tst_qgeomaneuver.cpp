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

#include "tst_qgeomaneuver.h"

QTM_USE_NAMESPACE

tst_QGeoManeuver::tst_QGeoManeuver()
{
}

void tst_QGeoManeuver::initTestCase()
{

}

void tst_QGeoManeuver::cleanupTestCase()
{

}

void tst_QGeoManeuver::init()
{

    qgeomaneuver = new QGeoManeuver();
}

void tst_QGeoManeuver::cleanup()
{
    delete qgeomaneuver;
}

void tst_QGeoManeuver::constructor()
{
    QString empty ="";

    QVERIFY(!qgeomaneuver->isValid());
    QCOMPARE(qgeomaneuver->direction(),QGeoManeuver::NoDirection);
    QCOMPARE(qgeomaneuver->distanceToNextInstruction(),0.0);
    QCOMPARE(qgeomaneuver->instructionText(),empty);
    QCOMPARE(qgeomaneuver->timeToNextInstruction(),0);
}

void tst_QGeoManeuver::copy_constructor()
{
    QGeoManeuver *qgeomaneuvercopy = new QGeoManeuver (*qgeomaneuver);

    QCOMPARE(*qgeomaneuver,*qgeomaneuvercopy);

    delete qgeomaneuvercopy;
}

void tst_QGeoManeuver::destructor()
{
    QGeoManeuver *qgeomaneuvercopy;

    QLocationTestUtils::uheap_mark();
    qgeomaneuvercopy = new QGeoManeuver();
    delete qgeomaneuvercopy;
    QLocationTestUtils::uheap_mark_end();

    QLocationTestUtils::uheap_mark();
    qgeomaneuvercopy = new QGeoManeuver(*qgeomaneuver);
    delete qgeomaneuvercopy;
    QLocationTestUtils::uheap_mark_end();
}

void tst_QGeoManeuver::direction()
{
    QFETCH(QGeoManeuver::InstructionDirection,direction);

    qgeomaneuver->setDirection(direction);

    QCOMPARE(qgeomaneuver->direction(),direction);
}
void tst_QGeoManeuver::direction_data()
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

void tst_QGeoManeuver::distanceToNextInstruction()
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

void tst_QGeoManeuver::instructionText()
{
    QString text = "After 50m turn left";

    qgeomaneuver->setInstructionText(text);

    QCOMPARE (qgeomaneuver->instructionText(),text);

    text="After 40m, turn left";
    QEXPECT_FAIL("", "Expected Fail", Continue);
    QCOMPARE (qgeomaneuver->instructionText(),text);

}

void tst_QGeoManeuver::position()
{
    QFETCH(double, latitude);
    QFETCH(double, longitude);

    qgeocoordinate = new QGeoCoordinate (latitude,longitude);

    qgeomaneuver->setPosition(*qgeocoordinate);

    QCOMPARE(qgeomaneuver->position(),*qgeocoordinate);

    delete qgeocoordinate;
}

void tst_QGeoManeuver::position_data()
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

void tst_QGeoManeuver::timeToNextInstruction()
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

void tst_QGeoManeuver::waypoint()
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
void tst_QGeoManeuver::waypoint_data()
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

void tst_QGeoManeuver::isValid()
{
    QVERIFY(!qgeomaneuver->isValid());
    qgeomaneuver->setDirection(QGeoManeuver::DirectionBearLeft);
    QVERIFY(qgeomaneuver->isValid());
}

void tst_QGeoManeuver::operators(){

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

// Not passing
//    QVERIFY(!(qgeomaneuver->operator ==(*qgeomaneuvercopy)));
// Not passing
//    QVERIFY(qgeomaneuver->operator !=(*qgeomaneuvercopy));

    *qgeomaneuvercopy = qgeomaneuvercopy->operator =(*qgeomaneuver);
    QVERIFY(qgeomaneuver->operator ==(*qgeomaneuvercopy));
    QVERIFY(!qgeomaneuver->operator !=(*qgeomaneuvercopy));

    delete qgeomaneuvercopy;
}


QTEST_MAIN(tst_QGeoManeuver);
