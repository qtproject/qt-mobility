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

#include "tst_qgeoaddress.h"

QTM_USE_NAMESPACE

tst_QGeoAddress::tst_QGeoAddress()
{
}

void tst_QGeoAddress::initTestCase()
{

}

void tst_QGeoAddress::cleanupTestCase()
{

}

void tst_QGeoAddress::init()
{

    qgeoaddress = new QGeoAddress();



}

void tst_QGeoAddress::cleanup()
{
    delete qgeoaddress;

}

void tst_QGeoAddress::t_qga_constructor()
{
//    QCOMPARE(typeid(*qgeoaddress).name(),"N10QtMobility11QGeoAddressE");
}

void tst_QGeoAddress::t_qga_constructorCopy()
{
    QGeoAddress *qgeoaddresscopy = new QGeoAddress (*qgeoaddress);
//    QCOMPARE(typeid(*qgeoaddress).name(),typeid(*qgeoaddresscopy).name());
    delete qgeoaddresscopy;
}

void tst_QGeoAddress::t_qga_city()
{
    QString city;
    qgeoaddress->setCity(city);
    QCOMPARE(qgeoaddress->city(),city);

    city = "";
    qgeoaddress->setCity(city);
    QCOMPARE(qgeoaddress->city(),city);

    city = "New York";
    qgeoaddress->setCity(city);
    QCOMPARE(qgeoaddress->city(),city);


}

void tst_QGeoAddress::t_qga_country()
{
    QString country;
    qgeoaddress->setCountry(country);
    QCOMPARE(qgeoaddress->country(),country);

    country = "";
    qgeoaddress->setCountry(country);
    QCOMPARE(qgeoaddress->country(),country);

    country = "Spain";
    qgeoaddress->setCountry(country);
    QCOMPARE(qgeoaddress->country(),country);

}

void tst_QGeoAddress::t_qga_countryCode()
{
    QString countryCode;
    qgeoaddress->setCountryCode(countryCode);
    QCOMPARE(qgeoaddress->countryCode(),countryCode);

    countryCode = "";
    qgeoaddress->setCountryCode(countryCode);
    QCOMPARE(qgeoaddress->countryCode(),countryCode);

    countryCode = "SGP";
    qgeoaddress->setCountryCode(countryCode);
    QCOMPARE(qgeoaddress->countryCode(),countryCode);
}

void tst_QGeoAddress::t_qga_county()
{
    QString county;
    qgeoaddress->setCounty(county);
    QCOMPARE(qgeoaddress->county(),county);

    county = "";
    qgeoaddress->setCounty(county);
    QCOMPARE(qgeoaddress->county(),county);

    county = "Kent";
    qgeoaddress->setCounty(county);
    QCOMPARE(qgeoaddress->county(),county);
}

void tst_QGeoAddress::t_qga_district()
{
    QString district;
    qgeoaddress->setDistrict(district);
    QCOMPARE(qgeoaddress->district(),district);

    district = "";
    qgeoaddress->setDistrict(district);
    QCOMPARE(qgeoaddress->district(),district);

    district = "Mitte";
    qgeoaddress->setDistrict(district);
    QCOMPARE(qgeoaddress->district(),district);
}

void tst_QGeoAddress::t_qga_postcode()
{
    QString postcode;
    qgeoaddress->setPostcode(postcode);
    QCOMPARE(qgeoaddress->postcode(),postcode);

    postcode = "";
    qgeoaddress->setPostcode(postcode);
    QCOMPARE(qgeoaddress->postcode(),postcode);

    postcode = "80080";
    qgeoaddress->setPostcode(postcode);
    QCOMPARE(qgeoaddress->postcode(),postcode);
}

void tst_QGeoAddress::t_qga_state()
{
    QString state;
    qgeoaddress->setState(state);
    QCOMPARE(qgeoaddress->state(),state);

    state = "";
    qgeoaddress->setState(state);
    QCOMPARE(qgeoaddress->state(),state);

    state = "Illinois";
    qgeoaddress->setState(state);
    QCOMPARE(qgeoaddress->state(),state);
}

void tst_QGeoAddress::t_qga_street()
{
    QString street;
    qgeoaddress->setStreet(street);
    QCOMPARE(qgeoaddress->street(),street);

    street = "";
    qgeoaddress->setStreet(street);
    QCOMPARE(qgeoaddress->street(),street);

    street = "Marathonas Avenue";
    qgeoaddress->setStreet(street);
    QCOMPARE(qgeoaddress->street(),street);
}

//Street Number is not define in the API!!!!
/*void tst_QGeoAddress::t_qga_streetNumber()
{
    QString streetNumber;
    qgeoaddress->setStreetNumber(streetNumber);
    QCOMPARE(qgeoaddress->street(),street);

    streetNumber = "";
    qgeoaddress->setStreetNumber(streetNumber);
    QCOMPARE(qgeoaddress->streetNumber(),streetNumber);

    streetNumber = "45";
    qgeoaddress->setStreetNumber(streetNumber);
    QCOMPARE(qgeoaddress->streetNumber(),streetNumber);
}*/

void tst_QGeoAddress::t_qga_emptyClear()
{
    QFETCH(QString, country);
    QFETCH(QString, countryCode);
    QFETCH(QString, state);
    QFETCH(QString, county);
    QFETCH(QString, city);
    QFETCH(QString, postcode);
    QFETCH(QString, district);
    QFETCH(QString, street);

    QVERIFY(qgeoaddress->isEmpty());
    qgeoaddress->clear();

    qgeoaddress->setCountry(country);
    QVERIFY(!qgeoaddress->isEmpty());
    qgeoaddress->clear();

    qgeoaddress->setCountryCode(countryCode);
    QVERIFY(!qgeoaddress->isEmpty());
    qgeoaddress->clear();

    qgeoaddress->setState(state);
    QVERIFY(!qgeoaddress->isEmpty());
    qgeoaddress->clear();

    qgeoaddress->setCounty(county);
    QVERIFY(!qgeoaddress->isEmpty());
    qgeoaddress->clear();

    qgeoaddress->setCity(city);
    QVERIFY(!qgeoaddress->isEmpty());
    qgeoaddress->clear();

    qgeoaddress->setPostcode(postcode);
    QVERIFY(!qgeoaddress->isEmpty());
    qgeoaddress->clear();

    qgeoaddress->setDistrict(district);
    QVERIFY(!qgeoaddress->isEmpty());
    qgeoaddress->clear();

    qgeoaddress->setStreet(street);
    QVERIFY(!qgeoaddress->isEmpty());
    qgeoaddress->clear();

    qgeoaddress->setCountryCode(countryCode);
    qgeoaddress->setState(state);
    qgeoaddress->setCounty(county);
    qgeoaddress->setCity(city);
    qgeoaddress->setPostcode(postcode);
    qgeoaddress->setDistrict(district);
    qgeoaddress->setStreet(street);
    QVERIFY(!qgeoaddress->isEmpty());
    qgeoaddress->clear();

}

void tst_QGeoAddress::t_qga_emptyClear_data()
{
    QTest::addColumn<QString>("country");
    QTest::addColumn<QString>("countryCode");
    QTest::addColumn<QString>("state");
    QTest::addColumn<QString>("county");
    QTest::addColumn<QString>("city");
    QTest::addColumn<QString>("postcode");
    QTest::addColumn<QString>("district");
    QTest::addColumn<QString>("street");

   QTest::newRow("adress1") << "Germany" << "DEU" << "Bavaria" << "XXX" << "Bayern" << "10873" << "North Side" << "Auguststrasse" ;

}

void tst_QGeoAddress::t_qga_operators()
{
    //Create a copy and see that they are the same
    QGeoAddress *qgeoaddresscopy = new QGeoAddress (*qgeoaddress);
    QVERIFY(qgeoaddress->operator ==(*qgeoaddresscopy));
    QVERIFY(!qgeoaddress->operator !=(*qgeoaddresscopy));

    QFETCH(QString, country);
    QFETCH(QString, countryCode);
    QFETCH(QString, state);
    QFETCH(QString, county);
    QFETCH(QString, city);
    QFETCH(QString, postcode);
    QFETCH(QString, district);
    QFETCH(QString, street);

    qgeoaddress->setCountryCode(countryCode);
    qgeoaddress->setState(state);
    qgeoaddress->setCounty(county);
    qgeoaddress->setCity(city);
    qgeoaddress->setPostcode(postcode);
    qgeoaddress->setDistrict(district);
    qgeoaddress->setStreet(street);

    //After modifiying an address, test that they are different
    QVERIFY(!qgeoaddress->operator ==(*qgeoaddresscopy));
    QVERIFY(qgeoaddress->operator !=(*qgeoaddresscopy));

   // delete qgeoaddresscopy;

    //Assign one address to the other and test that they are the same again
    *qgeoaddresscopy = qgeoaddresscopy->operator =(*qgeoaddress);
    QVERIFY(qgeoaddress->operator ==(*qgeoaddresscopy));
    QVERIFY(!qgeoaddress->operator !=(*qgeoaddresscopy));

    //Clear one address and test that they are different
    qgeoaddresscopy->clear();
    QVERIFY(!qgeoaddress->operator ==(*qgeoaddresscopy));
    QVERIFY(qgeoaddress->operator !=(*qgeoaddresscopy));

    delete qgeoaddresscopy;

}

void tst_QGeoAddress::t_qga_operators_data()
{
    QTest::addColumn<QString>("country");
    QTest::addColumn<QString>("countryCode");
    QTest::addColumn<QString>("state");
    QTest::addColumn<QString>("county");
    QTest::addColumn<QString>("city");
    QTest::addColumn<QString>("postcode");
    QTest::addColumn<QString>("district");
    QTest::addColumn<QString>("street");

   QTest::newRow("adress1") << "Germany" << "DEU" << "Bavaria" << "XXX" << "Bayern" << "10873" << "North Side" << "Auguststrasse" ;

}

QTEST_MAIN(tst_QGeoAddress);
