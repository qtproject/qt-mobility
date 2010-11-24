#ifndef TST_Coordinate_H
#define TST_Coordinate_H

#endif // TST_Coordinate_H

#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QtCore/QCoreApplication>
#include <QMetaType>
#include <QSignalSpy>

#include <typeinfo>

#include <QGeoCoordinate>

QTM_USE_NAMESPACE

class Coordinate : public QObject
{
    Q_OBJECT

public:
    Coordinate();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    //Start Unit Tests for qgeocoordinate.h
    void t_qgc_constructor1();
    void t_qgc_constructor2();
    void t_qgc_constructor3();
    void t_qgc_constructorCopy();
    void t_qgc_altitude();
    void t_qgc_latitude();
    void t_qgc_longitude();
    void t_qgc_isValid();
    void t_qgc_isValid_data();
    void t_qgc_type();
    void t_qgc_type_data();
    void t_qgc_azimuthTo();
    void t_qgc_azimuthTo_data();
    void t_qgc_distanceTo();
    void t_qgc_distanceTo_data();
    void t_qgc_toString();
    void t_qgc_toString_data();
    void t_qgc_operators();
    void t_qgc_operators_data();
    //End Unit Tests for qgeocoordinate.h


private:
    QGeoCoordinate *qgeocoordinate;
};

Q_DECLARE_METATYPE( QGeoCoordinate::CoordinateFormat);
Q_DECLARE_METATYPE( QGeoCoordinate::CoordinateType);

