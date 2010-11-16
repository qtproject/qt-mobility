#ifndef TST_Address_H
#define TST_Address_H

#endif // TST_Address_H

#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QtCore/QCoreApplication>
#include <QMetaType>
#include <QSignalSpy>

#include <typeinfo>

#include "qgeoaddress.h"

QTM_USE_NAMESPACE

class Address : public QObject
{
    Q_OBJECT

public:
    Address();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    //Start Unit Test for qgeoaddress.h
    void t_qga_constructor();
    void t_qga_constructorCopy();
    void t_qga_city();
    void t_qga_country();
    void t_qga_countryCode();
    void t_qga_county();
    void t_qga_district();
    void t_qga_postcode();
    void t_qga_state();
    void t_qga_street();
//    void t_qga_streetNumber();
    void t_qga_emptyClear();
    void t_qga_emptyClear_data();
    void t_qga_operators();
    void t_qga_operators_data();
    //End Unit Tests for qgeoaddress.h

private:
    QGeoAddress *qgeoaddress;





};

