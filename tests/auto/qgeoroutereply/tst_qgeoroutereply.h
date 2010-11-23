#ifndef TST_RouteReply_H
#define TST_RouteReply_H

#endif // TST_RouteReply_H

#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QtCore/QCoreApplication>
#include <QMetaType>
#include <QSignalSpy>

#include <typeinfo>

#include <QGeoCoordinate>
#include <QGeoRouteRequest>
#include <QGeoRouteReply>
#include <QGeoRoute>

QTM_USE_NAMESPACE
class SubRouteReply :public QGeoRouteReply
{
    Q_OBJECT
public:
    SubRouteReply(QGeoRouteRequest request):QGeoRouteReply(request) {}
    void callSetError(QGeoRouteReply::Error error, QString msg) {setError(error,msg);}
    void callSetFinished(bool finished) {setFinished(finished);}
    void callSetRoutes(const QList<QGeoRoute> & routes ) {setRoutes(routes);}
};

class RouteReply :public QObject
{
    Q_OBJECT

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    //Start Unit Test for QGeoRouteReply
private slots:
    void t_qgrrep_constructor1();
    void t_qgrrep_constructor2();
    void t_qgrrep_constructor2_data();
    void t_qgrrep_routes();
    void t_qgrrep_finished();
    void t_qgrrep_abort();
    void t_qgrrep_error();
    void t_qgrrep_error_data();
    void t_qgrrep_request();
    //End Unit Test for QGeoRouteReply


private:
    QGeoCoordinate *qgeocoordinate1;
    QGeoCoordinate *qgeocoordinate2;
    QGeoCoordinate *qgeocoordinate3;
    QGeoRouteRequest *qgeorouterequest;
    QSignalSpy *signalerror;
    QSignalSpy *signalfinished;
    QList<QGeoCoordinate> waypoints;
    SubRouteReply* reply;
};

Q_DECLARE_METATYPE( QList<QGeoRoute>);
Q_DECLARE_METATYPE( QList<QGeoCoordinate>);
Q_DECLARE_METATYPE( QList<double>);
Q_DECLARE_METATYPE( QGeoRouteReply::Error);


