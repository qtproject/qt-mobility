/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/
#include <qgeocoordinate.h>
#include <qtest.h>

#include <QMetaType>
#include <QDebug>

Q_DECLARE_METATYPE(QGeoCoordinate)
Q_DECLARE_METATYPE(QGeoCoordinate::CoordinateFormat)

static const QGeoCoordinate BRISBANE(-27.46758, 153.027892);
static const QGeoCoordinate MELBOURNE(-37.814251, 144.963169);
static const QGeoCoordinate LONDON(51.500152, -0.126236);
static const QGeoCoordinate NEW_YORK(40.71453, -74.00713);
static const QGeoCoordinate NORTH_POLE(90, 0);
static const QGeoCoordinate SOUTH_POLE(-90, 0);

static const QChar DEGREES_SYMB(0x00B0);


class tst_QGeoCoordinate : public QObject
{
    Q_OBJECT

private:
    enum TestDataType {
        Latitude,
        Longitude,
        Altitude
    };

private slots:
    void constructor()
    {
        QGeoCoordinate c;
        QVERIFY(!c.isValid());
        QCOMPARE(c, QGeoCoordinate());
    }

    void type()
    {
        QGeoCoordinate c;
        QVERIFY(!c.isValid());
        QVERIFY(c.type() == QGeoCoordinate::InvalidCoordinate);

        c.setLatitude(1);
        QVERIFY(!c.isValid());
        QVERIFY(c.type() == QGeoCoordinate::InvalidCoordinate);

        c.setLongitude(1);
        QVERIFY(c.isValid());
        QVERIFY(c.type() == QGeoCoordinate::Coordinate2D);

        c.setAltitude(1);
        QVERIFY(c.isValid());
        QVERIFY(c.type() == QGeoCoordinate::Coordinate3D);
    }

    void addDataValues(TestDataType type)
    {
        QTest::addColumn<double>("value");
        QTest::addColumn<bool>("valid");

        QTest::newRow("negative") << -1.0 << true;
        QTest::newRow("zero") << 0.0 << true;
        QTest::newRow("positive") << 1.0 << true;

        switch (type) {
            case Latitude:
                QTest::newRow("too low") << -90.1 << false;
                QTest::newRow("not too low") << -90.0 << true;
                QTest::newRow("not too hight") << 90.0 << true;
                QTest::newRow("too high") << 90.1;
                break;
            case Longitude:
                QTest::newRow("too low") << -180.1 << false;
                QTest::newRow("not too low") << -180.0 << true;
                QTest::newRow("not too hight") << 180.0 << true;
                QTest::newRow("too high") << 180.1;
                break;
            case Altitude:
                break;
        }
    }

    void latitude_data() { addDataValues(Latitude); }
    void latitude()
    {
        QFETCH(double, value);
        QGeoCoordinate c;
        c.setLatitude(value);
        QCOMPARE(QString::number(c.latitude()), QString::number(value));

        QGeoCoordinate c2 = c;
        QCOMPARE(QString::number(c2.latitude()), QString::number(value));
        QCOMPARE(c2, c);
    }

    void longitude_data() { addDataValues(Longitude); }
    void longitude()
    {
        QFETCH(double, value);
        QGeoCoordinate c;
        c.setLongitude(value);
        QCOMPARE(QString::number(c.longitude()), QString::number(value));

        QGeoCoordinate c2 = c;
        QCOMPARE(QString::number(c2.longitude()), QString::number(value));
        QCOMPARE(c2, c);
    }

    void altitude_data() { addDataValues(Altitude); }
    void altitude()
    {
        QFETCH(double, value);
        QGeoCoordinate c;
        c.setAltitude(value);
        QCOMPARE(QString::number(c.altitude()), QString::number(value));

        QGeoCoordinate c2 = c;
        QCOMPARE(QString::number(c2.altitude()), QString::number(value));
        QCOMPARE(c2, c);
    }

    void distanceTo_data()
    {
        QTest::addColumn<QGeoCoordinate>("c1");
        QTest::addColumn<QGeoCoordinate>("c2");
        QTest::addColumn<double>("distance");

        QTest::newRow("invalid coord 1")
                << QGeoCoordinate() << BRISBANE << 0.0;
        QTest::newRow("invalid coord 2")
                << BRISBANE << QGeoCoordinate() << 0.0;
        QTest::newRow("brisbane -> melbourne")
                << BRISBANE << MELBOURNE << 1374820.0;
        QTest::newRow("london -> new york")
                << LONDON << NEW_YORK << 5570540.0;
        QTest::newRow("north pole -> south pole")
                << NORTH_POLE << SOUTH_POLE << 20015100.0;
    }

    void distanceTo()
    {
        QFETCH(QGeoCoordinate, c1);
        QFETCH(QGeoCoordinate, c2);
        QFETCH(double, distance);

        QCOMPARE(QString::number(c1.distanceTo(c2)), QString::number(distance));
    }

    void azimuthTo_data()
    {
        QTest::addColumn<QGeoCoordinate>("c1");
        QTest::addColumn<QGeoCoordinate>("c2");
        QTest::addColumn<double>("azimuth");

        QTest::newRow("invalid coord 1")
                << QGeoCoordinate() << BRISBANE << 0.0;
        QTest::newRow("invalid coord 2")
                << BRISBANE << QGeoCoordinate() << 0.0;
        QTest::newRow("brisbane -> melbourne")
                << BRISBANE << MELBOURNE << 211.1717;
        QTest::newRow("london -> new york")
                << LONDON << NEW_YORK << 288.3389;
        QTest::newRow("north pole -> south pole")
                << NORTH_POLE << SOUTH_POLE << 180.0;
    }

    void azimuthTo()
    {
        QFETCH(QGeoCoordinate, c1);
        QFETCH(QGeoCoordinate, c2);
        QFETCH(double, azimuth);

        QCOMPARE(QString::number(c1.azimuthTo(c2)), QString::number(azimuth));
    }

    void degreesToString_data()
    {
        QTest::addColumn<QGeoCoordinate>("coord");
        QTest::addColumn<QGeoCoordinate::CoordinateFormat>("format");
        QTest::addColumn<QString>("string");

        QGeoCoordinate northEast(27.46758, 153.027892);
        QGeoCoordinate southEast(-27.46758, 153.027892);
        QGeoCoordinate northWest(27.46758, -153.027892);
        QGeoCoordinate southWest(-27.46758, -153.027892);

        QTest::newRow("NE, dd, no hemisphere")
                << northEast << QGeoCoordinate::DecimalDegrees
                << QString("27.46758%1, 153.02789%1").arg(DEGREES_SYMB);
        QTest::newRow("NE, dd, hemisphere")
                << northEast << QGeoCoordinate::DecimalDegreesWithHemisphere
                << QString("27.46758%1 N, 153.02789%1 E").arg(DEGREES_SYMB);
        QTest::newRow("NE, dm, no hemisphere")
                << northEast << QGeoCoordinate::DegreesMinutes
                << QString("27%1 28.055', 153%1 1.674'").arg(DEGREES_SYMB);
        QTest::newRow("NE, dm, hemisphere")
                << northEast << QGeoCoordinate::DegreesMinutesWithHemisphere
                << QString("27%1 28.055' N, 153%1 1.674' E").arg(DEGREES_SYMB);
        QTest::newRow("NE, dms, no hemisphere")
                << northEast << QGeoCoordinate::DegreesMinutesSeconds
                << QString("27%1 28' 3.3\", 153%1 1' 40.4\"").arg(DEGREES_SYMB);
        QTest::newRow("NE, dms, hemisphere")
                << northEast << QGeoCoordinate::DegreesMinutesSecondsWithHemisphere
                << QString("27%1 28' 3.3\" N, 153%1 1' 40.4\" E").arg(DEGREES_SYMB);

        QTest::newRow("SE, dd, no hemisphere")
                << southEast << QGeoCoordinate::DecimalDegrees
                << QString("-27.46758%1, 153.02789%1").arg(DEGREES_SYMB);
        QTest::newRow("SE, dd, hemisphere")
                << southEast << QGeoCoordinate::DecimalDegreesWithHemisphere
                << QString("27.46758%1 S, 153.02789%1 E").arg(DEGREES_SYMB);
        QTest::newRow("SE, dm, no hemisphere")
                << southEast << QGeoCoordinate::DegreesMinutes
                << QString("-27%1 28.055', 153%1 1.674'").arg(DEGREES_SYMB);
        QTest::newRow("SE, dm, hemisphere")
                << southEast << QGeoCoordinate::DegreesMinutesWithHemisphere
                << QString("27%1 28.055' S, 153%1 1.674' E").arg(DEGREES_SYMB);
        QTest::newRow("SE, dms, no hemisphere")
                << southEast << QGeoCoordinate::DegreesMinutesSeconds
                << QString("-27%1 28' 3.3\", 153%1 1' 40.4\"").arg(DEGREES_SYMB);
        QTest::newRow("SE, dms, hemisphere")
                << southEast << QGeoCoordinate::DegreesMinutesSecondsWithHemisphere
                << QString("27%1 28' 3.3\" S, 153%1 1' 40.4\" E").arg(DEGREES_SYMB);

        QTest::newRow("NE, dd, no hemisphere")
                << northEast << QGeoCoordinate::DecimalDegrees
                << QString("27.46758%1, 153.02789%1").arg(DEGREES_SYMB);
        QTest::newRow("NE, dd, hemisphere")
                << northEast << QGeoCoordinate::DecimalDegreesWithHemisphere
                << QString("27.46758%1 N, 153.02789%1 E").arg(DEGREES_SYMB);
        QTest::newRow("NE, dm, no hemisphere")
                << northEast << QGeoCoordinate::DegreesMinutes
                << QString("27%1 28.055', 153%1 1.674'").arg(DEGREES_SYMB);
        QTest::newRow("NE, dm, hemisphere")
                << northEast << QGeoCoordinate::DegreesMinutesWithHemisphere
                << QString("27%1 28.055' N, 153%1 1.674' E").arg(DEGREES_SYMB);
        QTest::newRow("NE, dms, no hemisphere")
                << northEast << QGeoCoordinate::DegreesMinutesSeconds
                << QString("27%1 28' 3.3\", 153%1 1' 40.4\"").arg(DEGREES_SYMB);
        QTest::newRow("NE, dms, hemisphere")
                << northEast << QGeoCoordinate::DegreesMinutesSecondsWithHemisphere
                << QString("27%1 28' 3.3\" N, 153%1 1' 40.4\" E").arg(DEGREES_SYMB);

        QTest::newRow("SE, dd, no hemisphere")
                << southEast << QGeoCoordinate::DecimalDegrees
                << QString("-27.46758%1, 153.02789%1").arg(DEGREES_SYMB);
        QTest::newRow("SE, dd, hemisphere")
                << southEast << QGeoCoordinate::DecimalDegreesWithHemisphere
                << QString("27.46758%1 S, 153.02789%1 E").arg(DEGREES_SYMB);
        QTest::newRow("SE, dm, no hemisphere")
                << southEast << QGeoCoordinate::DegreesMinutes
                << QString("-27%1 28.055', 153%1 1.674'").arg(DEGREES_SYMB);
        QTest::newRow("SE, dm, hemisphere")
                << southEast << QGeoCoordinate::DegreesMinutesWithHemisphere
                << QString("27%1 28.055' S, 153%1 1.674' E").arg(DEGREES_SYMB);
        QTest::newRow("SE, dms, no hemisphere")
                << southEast << QGeoCoordinate::DegreesMinutesSeconds
                << QString("-27%1 28' 3.3\", 153%1 1' 40.4\"").arg(DEGREES_SYMB);
        QTest::newRow("SE, dms, hemisphere")
                << southEast << QGeoCoordinate::DegreesMinutesSecondsWithHemisphere
                << QString("27%1 28' 3.3\" S, 153%1 1' 40.4\" E").arg(DEGREES_SYMB);

        QTest::newRow("NW, dd, no hemisphere")
                << northWest << QGeoCoordinate::DecimalDegrees
                << QString("27.46758%1, -153.02789%1").arg(DEGREES_SYMB);
        QTest::newRow("NW, dd, hemisphere")
                << northWest << QGeoCoordinate::DecimalDegreesWithHemisphere
                << QString("27.46758%1 N, 153.02789%1 W").arg(DEGREES_SYMB);
        QTest::newRow("NW, dm, no hemisphere")
                << northWest << QGeoCoordinate::DegreesMinutes
                << QString("27%1 28.055', -153%1 1.674'").arg(DEGREES_SYMB);
        QTest::newRow("NW, dm, hemisphere")
                << northWest << QGeoCoordinate::DegreesMinutesWithHemisphere
                << QString("27%1 28.055' N, 153%1 1.674' W").arg(DEGREES_SYMB);
        QTest::newRow("NW, dms, no hemisphere")
                << northWest << QGeoCoordinate::DegreesMinutesSeconds
                << QString("27%1 28' 3.3\", -153%1 1' 40.4\"").arg(DEGREES_SYMB);
        QTest::newRow("NW, dms, hemisphere")
                << northWest << QGeoCoordinate::DegreesMinutesSecondsWithHemisphere
                << QString("27%1 28' 3.3\" N, 153%1 1' 40.4\" W").arg(DEGREES_SYMB);

        QTest::newRow("SW, dd, no hemisphere")
                << southWest << QGeoCoordinate::DecimalDegrees
                << QString("-27.46758%1, -153.02789%1").arg(DEGREES_SYMB);
        QTest::newRow("SW, dd, hemisphere")
                << southWest << QGeoCoordinate::DecimalDegreesWithHemisphere
                << QString("27.46758%1 S, 153.02789%1 W").arg(DEGREES_SYMB);
        QTest::newRow("SW, dm, no hemisphere")
                << southWest << QGeoCoordinate::DegreesMinutes
                << QString("-27%1 28.055', -153%1 1.674'").arg(DEGREES_SYMB);
        QTest::newRow("SW, dm, hemisphere")
                << southWest << QGeoCoordinate::DegreesMinutesWithHemisphere
                << QString("27%1 28.055' S, 153%1 1.674' W").arg(DEGREES_SYMB);
        QTest::newRow("SW, dms, no hemisphere")
                << southWest << QGeoCoordinate::DegreesMinutesSeconds
                << QString("-27%1 28' 3.3\", -153%1 1' 40.4\"").arg(DEGREES_SYMB);
        QTest::newRow("SW, dms, hemisphere")
                << southWest << QGeoCoordinate::DegreesMinutesSecondsWithHemisphere
                << QString("27%1 28' 3.3\" S, 153%1 1' 40.4\" W").arg(DEGREES_SYMB);
    }

    void degreesToString()
    {
        QFETCH(QGeoCoordinate, coord);
        QFETCH(QGeoCoordinate::CoordinateFormat, format);
        QFETCH(QString, string);

        QCOMPARE(coord.toString(format), string);
    }

    void datastream()
    {
        QFETCH(QGeoCoordinate, coord);
        
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out << coord;

        QDataStream in(&ba, QIODevice::ReadOnly);
        QGeoCoordinate inCoord;
        in >> inCoord;
        QCOMPARE(inCoord, coord);
    }

    void datastream_data()
    {
        QTest::addColumn<QGeoCoordinate>("coord");

        QTest::newRow("invalid") << QGeoCoordinate();
        QTest::newRow("valid lat, long") << BRISBANE;
        QTest::newRow("valid lat, long, alt") << QGeoCoordinate(-1, -1, -1);
        QTest::newRow("valid lat, long, alt again") << QGeoCoordinate(1, 1, 1);
    }
};

QTEST_MAIN(tst_QGeoCoordinate)
#include "tst_qgeocoordinate.moc"
