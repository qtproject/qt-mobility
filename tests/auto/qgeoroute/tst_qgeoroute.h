#ifndef TST_Route_H
#define TST_Route_H

#endif // TST_Route_H

#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QtCore/QCoreApplication>
#include <QMetaType>
#include <QSignalSpy>

#include <typeinfo>

#include <QGeoRoute>
#include <QGeoBoundingBox>
#include <QGeoCoordinate>
#include <QGeoRouteRequest>
#include <QGeoRouteSegment>


QTM_USE_NAMESPACE

class Route : public QObject
{
    Q_OBJECT

public:
    Route();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    //Start unit test for QGeoRoute
    void t_qgr_constructor();
    void t_qgr_constructorCopy();
    void t_qgr_bounds();
    void t_qgr_distance();
    void t_qgr_path();
    void t_qgr_path_data();
    void t_qgr_request();
    void t_qgr_routeId();
    void t_qgr_firstrouteSegments();
    void t_qgr_travelMode();
    void t_qgr_travelMode_data();
    void t_qgr_travelTime();
    void t_qgr_operators();
    //End Unit Test for QGeoRoute

private:
    QGeoRoute *qgeoroute;
    QGeoBoundingBox *qgeoboundingbox;
    QGeoCoordinate *qgeocoordinate;
    QGeoRouteRequest *qgeorouterequest;
    QGeoRouteSegment *qgeoroutesegment;


};

Q_DECLARE_METATYPE( QList<double>);
Q_DECLARE_METATYPE( QList<QGeoCoordinate>);
Q_DECLARE_METATYPE( QGeoRouteRequest::TravelMode);



