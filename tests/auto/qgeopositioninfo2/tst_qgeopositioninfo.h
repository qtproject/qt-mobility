#ifndef TST_PositionInfo_H
#define TST_PositionInfo_H

#endif // TST_PositionInfo_H

#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QtCore/QCoreApplication>
#include <QMetaType>
#include <QSignalSpy>

#include <typeinfo>

#include <QGeoCoordinate>
#include <QGeoPositionInfo>

QTM_USE_NAMESPACE

class PositionInfo : public QObject
{
    Q_OBJECT

public:
    PositionInfo();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    //Start Unit Test for qgeopositioninfo.h
    void t_qgpi_constructor();
    void t_qgpi_constructor2();
    void t_qgpi_constructorcopy();
    void t_qgpi_attribute();
    void t_qgpi_attribute_data();
    void t_qgpi_coordinate();
    void t_qgpi_timestamp();
    void t_qgpi_hasRemoveAttribute();
    void t_qgpi_hasRemoveAttribute_data();
    void t_qgpi_isValid();
    void t_qgpi_isValid_data();
    void t_qgpi_operators();
    void t_qgpi_operators_data();
    //End Unit Test for qgeopositioninfo.h


private:

    QGeoCoordinate *qgeocoordinate;
    QGeoPositionInfo *qgeoposinfo;

};

Q_DECLARE_METATYPE( QGeoCoordinate::CoordinateFormat);
Q_DECLARE_METATYPE( QGeoCoordinate::CoordinateType);
Q_DECLARE_METATYPE( QGeoPositionInfo::Attribute);
Q_DECLARE_METATYPE( QList<double>);
