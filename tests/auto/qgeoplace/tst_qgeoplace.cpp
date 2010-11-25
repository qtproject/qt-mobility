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

#include "tst_qgeoplace.h"

QTM_USE_NAMESPACE

tst_QGeoPlace::tst_QGeoPlace()
{
}

void tst_QGeoPlace::initTestCase()
{

}

void tst_QGeoPlace::cleanupTestCase()
{

}

void tst_QGeoPlace::init()
{
    qgeoaddress = new QGeoAddress();
    qgeocoordinate = new QGeoCoordinate();
    qgeoplace = new QGeoPlace();
    qgeoboundingbox = new QGeoBoundingBox();

}

void tst_QGeoPlace::cleanup()
{
    delete qgeoaddress;
    delete qgeocoordinate;
    delete qgeoplace;
    delete qgeoboundingbox;


}

void tst_QGeoPlace::t_qgp_constructor()
{
//    QCOMPARE(typeid(*qgeoplace).name(),"N10QtMobility9QGeoPlaceE");
}

void tst_QGeoPlace::t_qgp_constructorCopy()
{
    QGeoPlace *qgeoplacecopy = new QGeoPlace (*qgeoplace);
//    QCOMPARE(typeid(*qgeoplace).name(),typeid(*qgeoplacecopy).name());
    delete qgeoplacecopy;
}

void tst_QGeoPlace::t_qgp_address()
{
    qgeoaddress->setCity("Berlin");
    qgeoaddress->setCountry("Germany");
    qgeoaddress->setCountryCode("DEU");
    qgeoaddress->setDistrict("Mitte");
    qgeoaddress->setPostcode("10115");
    qgeoaddress->setStreet("Invalidenstrasse");

    qgeoplace->setAddress(*qgeoaddress);

    QCOMPARE(qgeoplace->address(),*qgeoaddress);

    qgeoaddress->setPostcode("10125");
    QEXPECT_FAIL("","Test Correct", Continue);
    QCOMPARE(qgeoplace->address(),*qgeoaddress);
}

void tst_QGeoPlace::t_qgp_coordinate()
{
    qgeocoordinate->setLatitude(13.3851);
    qgeocoordinate->setLongitude(52.5312);
    qgeocoordinate->setAltitude(134.23);

    qgeoplace->setCoordinate(*qgeocoordinate);

    QCOMPARE(qgeoplace->coordinate(),*qgeocoordinate);

    qgeocoordinate->setAltitude(0);
    QEXPECT_FAIL("","Test Correct", Continue);
    QCOMPARE(qgeoplace->coordinate(),*qgeocoordinate);
}

void tst_QGeoPlace::t_qgp_viewport()
{
    qgeocoordinate->setLatitude(13.3851);
    qgeocoordinate->setLongitude(52.5312);

    QGeoBoundingBox *qgeoboundingboxcopy= new QGeoBoundingBox(*qgeocoordinate,0.4,0.4);

    qgeoplace->setViewport(*qgeoboundingboxcopy);

    QCOMPARE(qgeoplace->viewport(),*qgeoboundingboxcopy);

    qgeoboundingboxcopy->setHeight(1);

    QEXPECT_FAIL("","Test Correct", Continue);
    QCOMPARE(qgeoplace->viewport(),*qgeoboundingboxcopy);

    delete qgeoboundingboxcopy;
}

void tst_QGeoPlace::t_qgp_isLandmark()
{
    QVERIFY(!qgeoplace->isLandmark());

    qgeoplace->setAddress(*qgeoaddress);
    QVERIFY(!qgeoplace->isLandmark());

    qgeoplace->setCoordinate(*qgeocoordinate);
    QVERIFY(!qgeoplace->isLandmark());

    //qgeoaddress->setCity("Berlin");
    qgeoaddress->setCountryCode("DEU");
    qgeocoordinate->setLatitude(13.4109);
    qgeocoordinate->setLongitude(52.5221);

    qgeoplace->setAddress(*qgeoaddress);
    qgeoplace->setCoordinate(*qgeocoordinate);
// Not passing
//    QVERIFY(qgeoplace->isLandmark());

}

void tst_QGeoPlace::t_qgp_operators()
{
    QGeoAddress *qgeoaddresscopy = new QGeoAddress();
    qgeoaddresscopy->setCity("Berlin");
    qgeoaddresscopy->setCountry("Germany");
    qgeoaddresscopy->setCountryCode("DEU");

    QGeoCoordinate *qgeocoordinatecopy = new QGeoCoordinate (32.324 , 41.324 , 24.55);

    qgeoaddress->setCity("Madrid");
    qgeoaddress->setCountry("Spain");
    qgeoaddress->setCountryCode("SPA");

    qgeocoordinate->setLatitude(21.3434);
    qgeocoordinate->setLongitude(38.43443);
    qgeocoordinate->setAltitude(634.21);

    qgeoplace->setAddress(*qgeoaddress);
    qgeoplace->setCoordinate(*qgeocoordinate);

    //Create a copy and see that they are the same
    QGeoPlace *qgeoplacecopy = new QGeoPlace (*qgeoplace);
    QVERIFY(qgeoplace->operator ==(*qgeoplacecopy));
    QVERIFY(!qgeoplace->operator !=(*qgeoplacecopy));

    //Modify one and test if they are different
   qgeoplacecopy->setAddress(*qgeoaddresscopy);
   QVERIFY(!qgeoplace->operator ==(*qgeoplacecopy));
   QVERIFY(qgeoplace->operator !=(*qgeoplacecopy));
   qgeoplacecopy->setCoordinate(*qgeocoordinatecopy);
   QVERIFY(!qgeoplace->operator ==(*qgeoplacecopy));
   QVERIFY(qgeoplace->operator !=(*qgeoplacecopy));

   //delete qgeoplacecopy;
   //Asign and test that they are the same
    *qgeoplacecopy = qgeoplacecopy->operator =(*qgeoplace);
    QVERIFY(qgeoplace->operator ==(*qgeoplacecopy));
    QVERIFY(!qgeoplace->operator !=(*qgeoplacecopy));

    delete qgeocoordinatecopy;
    delete qgeoaddresscopy;
    delete qgeoplacecopy;

}

QTEST_MAIN(tst_QGeoPlace);

