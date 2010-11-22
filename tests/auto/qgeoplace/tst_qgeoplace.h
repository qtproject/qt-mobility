#ifndef TST_Place_H
#define TST_Place_H

#endif // TST_Place_H

#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QtCore/QCoreApplication>
#include <QMetaType>
#include <QSignalSpy>

#include <typeinfo>

#include <QGeoAddress>
#include <QGeoCoordinate>
#include <QGeoPlace>
#include <QGeoBoundingBox>


QTM_USE_NAMESPACE

class Place : public QObject
{
    Q_OBJECT

public:
    Place();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

   //Start Unit Tests for qgeoplace.h
    void t_qgp_constructor();
    void t_qgp_constructorCopy();
    void t_qgp_address();
    void t_qgp_coordinate();
    void t_qgp_viewport();
    void t_qgp_isLandmark();
    void t_qgp_operators();
    //End Unit Tests for qgeoplace.h


private:

    QGeoAddress *qgeoaddress;
    QGeoCoordinate *qgeocoordinate;
    QGeoPlace *qgeoplace;
    QGeoBoundingBox *qgeoboundingbox;

};

Q_DECLARE_METATYPE( QGeoCoordinate::CoordinateFormat);
Q_DECLARE_METATYPE( QGeoCoordinate::CoordinateType);
Q_DECLARE_METATYPE( QList<double>);

