#ifndef TST_RouteSegment_H
#define TST_RouteSegment_H

#endif // TST_RouteSegment_H

#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QtCore/QCoreApplication>
#include <QMetaType>
#include <QSignalSpy>

#include <typeinfo>

#include <QGeoCoordinate>
#include <QGeoRouteSegment>
#include <QGeoManeuver>

QTM_USE_NAMESPACE

class RouteSegment : public QObject
{
    Q_OBJECT

public:
    RouteSegment();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    //Start unit test for QGeoRouteSegment
    void t_qgrs_constructor();
    void t_qgrs_constructorCopy();
    void t_qgrs_travelTime();
    void t_qgrs_travelTime_data();
    void t_qgrs_distance();
    void t_qgrs_distance_data();
    void t_qgrs_path();
    void t_qgrs_path_data();
    void t_qgrs_maneuver();
    void t_qgrs_nextroutesegment();
    void t_qgrs_isValid();
    void t_qgrs_operators();
    void t_qgrs_operators_data();
    //End Unit Test for QGeoRouteSegment

private:

    QGeoCoordinate *qgeocoordinate;
    QGeoRouteSegment *qgeoroutesegment;
    QGeoManeuver *qgeomaneuver;

};

Q_DECLARE_METATYPE( QList<double>);

