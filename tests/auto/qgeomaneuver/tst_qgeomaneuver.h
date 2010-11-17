#ifndef TST_Maneuver_H
#define TST_Maneuver_H

#endif // TST_Maneuver_H

#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QtCore/QCoreApplication>
#include <QMetaType>
#include <QSignalSpy>

#include <typeinfo>

#include <QGeoManeuver>
#include <QGeoCoordinate>

QTM_USE_NAMESPACE

class Maneuver : public QObject
{
    Q_OBJECT

public:
    Maneuver();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    //Start unit test for QGeoRouteManeuver
    void t_qgm_constructor();
    void t_qgm_constructorCopy();
    void t_qgm_direction();
    void t_qgm_direction_data();
    void t_qgm_distanceToNextInstruction();
    void t_qgm_instructionText();
    void t_qgm_position();
    void t_qgm_position_data();
    void t_qgm_timeToNextInstruction();
    void t_qgm_waypoint();
    void t_qgm_waypoint_data();
    void t_qgm_isValid();
    void t_qgm_operators();
    //End Unit Test for QGeoRouteManeuver

private:
    QGeoManeuver *qgeomaneuver;
    QGeoCoordinate *qgeocoordinate;

};

Q_DECLARE_METATYPE( QList<double>);
Q_DECLARE_METATYPE (QGeoManeuver::InstructionDirection);

