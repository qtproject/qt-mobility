#ifndef TST_RouteRequest_H
#define TST_RouteRequest_H

#endif // TST_RouteRequest_H

#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QtCore/QCoreApplication>
#include <QMetaType>
#include <QSignalSpy>

#include <typeinfo>

#include <QGeoCoordinate>
#include <QGeoBoundingBox>
#include <QGeoRouteRequest>


QTM_USE_NAMESPACE

class RouteRequest : public QObject
{
    Q_OBJECT

public:
    RouteRequest();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    //Start Unit Test for QGeoRouteRequest
    void t_qgrreq_constructor1();
    void t_qgrreq_constructor2();
    void t_qgrreq_constructorcopy();
    void t_qgrreq_excludeAreas();
    void t_qgrreq_numberAlternativeRoutes();
    void t_qgrreq_routeOptimization();
    void t_qgrreq_routeOptimization_data();
    void t_qgrreq_segmentDetail();
    void t_qgrreq_segmentDetail_data();
    void t_qgrreq_travelModes();
    void t_qgrreq_travelModes_data();
    void t_qgrreq_waypoints();
    void t_qgrreq_waypoints_data();
    void t_qgrreq_maneuverDetail();
    void t_qgrreq_maneuverDetail_data();
    void t_qgrreq_featureWeight();
    void t_qgrreq_featureWeight_data();
    //End Unit Test for QGeoRouteRequest

private:
    QGeoCoordinate *qgeocoordinate;
    QGeoBoundingBox *qgeoboundingbox;
    QGeoRouteRequest *qgeorouterequest;

};

Q_DECLARE_METATYPE( QList<double>);
Q_DECLARE_METATYPE( QGeoRouteRequest::RouteOptimization );
Q_DECLARE_METATYPE( QGeoRouteRequest::SegmentDetail );
Q_DECLARE_METATYPE( QGeoRouteRequest::TravelMode );
Q_DECLARE_METATYPE( QGeoRouteRequest::FeatureType);
Q_DECLARE_METATYPE( QGeoRouteRequest::FeatureWeight);
Q_DECLARE_METATYPE( QGeoRouteRequest::ManeuverDetail );
