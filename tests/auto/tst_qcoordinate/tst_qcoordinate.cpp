/****************************************************************************
**
** This file is part of the $PACKAGE_NAME$.
**
** Copyright (C) $THISYEAR$ $COMPANY_NAME$.
**
** $QT_EXTENDED_DUAL_LICENSE$
**
****************************************************************************/

#include <qcoordinate.h>
#include <qtest.h>

#include <QMetaType>
#include <QDebug>

Q_DECLARE_METATYPE(QCoordinate)
Q_DECLARE_METATYPE(QCoordinate::CoordinateFormat)

static const QCoordinate BRISBANE(-27.46758, 153.027892);
static const QCoordinate MELBOURNE(-37.814251, 144.963169);
static const QCoordinate LONDON(51.500152, -0.126236);
static const QCoordinate NEW_YORK(40.71453, -74.00713);
static const QCoordinate NORTH_POLE(90, 0);
static const QCoordinate SOUTH_POLE(-90, 0);

static const QChar DEGREES_SYMB(0x00B0);


class tst_QCoordinate : public QObject
{
    Q_OBJECT

private slots:
    void type()
    {
        QCoordinate c;
        QVERIFY(!c.isValid());
        QVERIFY(c.type() == QCoordinate::InvalidCoordinate);

        c.setLatitude(1);
        QVERIFY(!c.isValid());
        QVERIFY(c.type() == QCoordinate::InvalidCoordinate);

        c.setLongitude(1);
        QVERIFY(c.isValid());
        QVERIFY(c.type() == QCoordinate::Coordinate2D);

        c.setAltitude(1);
        QVERIFY(c.isValid());
        QVERIFY(c.type() == QCoordinate::Coordinate3D);
    }

    // TODO fix tests to test range (-90 -> 90 for lat, -180 -> 180 for long)
    void addDataValues()
    {
        QTest::addColumn<double>("value");
        QTest::newRow("negative") << -1.0;
        QTest::newRow("zero") << 0.0;
        QTest::newRow("positive") << 1.0;
    }

    void latitude_data() { addDataValues(); }
    void latitude()
    {
        QFETCH(double, value);
        QCoordinate c;
        c.setLatitude(value);
        QCOMPARE(QString::number(c.latitude()), QString::number(value));

        QCoordinate c2 = c;
        QCOMPARE(QString::number(c2.latitude()), QString::number(value));
    }

    void longitude_data() { addDataValues(); }
    void longitude()
    {
        QFETCH(double, value);
        QCoordinate c;
        c.setLongitude(value);
        QCOMPARE(QString::number(c.longitude()), QString::number(value));

        QCoordinate c2 = c;
        QCOMPARE(QString::number(c2.longitude()), QString::number(value));
    }

    void altitude_data() { addDataValues(); }
    void altitude()
    {
        QFETCH(double, value);
        QCoordinate c;
        c.setAltitude(value);
        QCOMPARE(QString::number(c.altitude()), QString::number(value));

        QCoordinate c2 = c;
        QCOMPARE(QString::number(c2.altitude()), QString::number(value));
    }

    void distanceTo_data()
    {
        QTest::addColumn<QCoordinate>("c1");
        QTest::addColumn<QCoordinate>("c2");
        QTest::addColumn<double>("distance");

        QTest::newRow("invalid coord 1")
                << QCoordinate() << BRISBANE << 0.0;
        QTest::newRow("invalid coord 2")
                << BRISBANE << QCoordinate() << 0.0;
        QTest::newRow("brisbane -> melbourne")
                << BRISBANE << MELBOURNE << 1374820.0;
        QTest::newRow("london -> new york")
                << LONDON << NEW_YORK << 5570540.0;
        QTest::newRow("north pole -> south pole")
                << NORTH_POLE << SOUTH_POLE << 20015100.0;
    }

    void distanceTo()
    {
        QFETCH(QCoordinate, c1);
        QFETCH(QCoordinate, c2);
        QFETCH(double, distance);

        QCOMPARE(QString::number(c1.distanceTo(c2)), QString::number(distance));
    }

    void azimuthTo_data()
    {
        QTest::addColumn<QCoordinate>("c1");
        QTest::addColumn<QCoordinate>("c2");
        QTest::addColumn<double>("azimuth");

        QTest::newRow("invalid coord 1")
                << QCoordinate() << BRISBANE << 0.0;
        QTest::newRow("invalid coord 2")
                << BRISBANE << QCoordinate() << 0.0;
        QTest::newRow("brisbane -> melbourne")
                << BRISBANE << MELBOURNE << 211.1717;
        QTest::newRow("london -> new york")
                << LONDON << NEW_YORK << 288.3389;
        QTest::newRow("north pole -> south pole")
                << NORTH_POLE << SOUTH_POLE << 180.0;
    }

    void azimuthTo()
    {
        QFETCH(QCoordinate, c1);
        QFETCH(QCoordinate, c2);
        QFETCH(double, azimuth);

        QCOMPARE(QString::number(c1.azimuthTo(c2)), QString::number(azimuth));
    }

    void degreesToString_data()
    {
        QTest::addColumn<QCoordinate>("coord");
        QTest::addColumn<QCoordinate::CoordinateFormat>("format");
        QTest::addColumn<QString>("string");

        QCoordinate northEast(27.46758, 153.027892);
        QCoordinate southEast(-27.46758, 153.027892);
        QCoordinate northWest(27.46758, -153.027892);
        QCoordinate southWest(-27.46758, -153.027892);

        QTest::newRow("NE, dd, no hemisphere")
                << northEast << QCoordinate::DecimalDegrees
                << QString("27.46758%1, 153.02789%1").arg(DEGREES_SYMB);
        QTest::newRow("NE, dd, hemisphere")
                << northEast << QCoordinate::DecimalDegreesWithHemisphere
                << QString("27.46758%1 N, 153.02789%1 E").arg(DEGREES_SYMB);
        QTest::newRow("NE, dm, no hemisphere")
                << northEast << QCoordinate::DegreesMinutes
                << QString("27%1 28.055', 153%1 1.674'").arg(DEGREES_SYMB);
        QTest::newRow("NE, dm, hemisphere")
                << northEast << QCoordinate::DegreesMinutesWithHemisphere
                << QString("27%1 28.055' N, 153%1 1.674' E").arg(DEGREES_SYMB);
        QTest::newRow("NE, dms, no hemisphere")
                << northEast << QCoordinate::DegreesMinutesSeconds
                << QString("27%1 28' 3.3\", 153%1 1' 40.4\"").arg(DEGREES_SYMB);
        QTest::newRow("NE, dms, hemisphere")
                << northEast << QCoordinate::DegreesMinutesSecondsWithHemisphere
                << QString("27%1 28' 3.3\" N, 153%1 1' 40.4\" E").arg(DEGREES_SYMB);

        QTest::newRow("SE, dd, no hemisphere")
                << southEast << QCoordinate::DecimalDegrees
                << QString("-27.46758%1, 153.02789%1").arg(DEGREES_SYMB);
        QTest::newRow("SE, dd, hemisphere")
                << southEast << QCoordinate::DecimalDegreesWithHemisphere
                << QString("27.46758%1 S, 153.02789%1 E").arg(DEGREES_SYMB);
        QTest::newRow("SE, dm, no hemisphere")
                << southEast << QCoordinate::DegreesMinutes
                << QString("-27%1 28.055', 153%1 1.674'").arg(DEGREES_SYMB);
        QTest::newRow("SE, dm, hemisphere")
                << southEast << QCoordinate::DegreesMinutesWithHemisphere
                << QString("27%1 28.055' S, 153%1 1.674' E").arg(DEGREES_SYMB);
        QTest::newRow("SE, dms, no hemisphere")
                << southEast << QCoordinate::DegreesMinutesSeconds
                << QString("-27%1 28' 3.3\", 153%1 1' 40.4\"").arg(DEGREES_SYMB);
        QTest::newRow("SE, dms, hemisphere")
                << southEast << QCoordinate::DegreesMinutesSecondsWithHemisphere
                << QString("27%1 28' 3.3\" S, 153%1 1' 40.4\" E").arg(DEGREES_SYMB);

        QTest::newRow("NE, dd, no hemisphere")
                << northEast << QCoordinate::DecimalDegrees
                << QString("27.46758%1, 153.02789%1").arg(DEGREES_SYMB);
        QTest::newRow("NE, dd, hemisphere")
                << northEast << QCoordinate::DecimalDegreesWithHemisphere
                << QString("27.46758%1 N, 153.02789%1 E").arg(DEGREES_SYMB);
        QTest::newRow("NE, dm, no hemisphere")
                << northEast << QCoordinate::DegreesMinutes
                << QString("27%1 28.055', 153%1 1.674'").arg(DEGREES_SYMB);
        QTest::newRow("NE, dm, hemisphere")
                << northEast << QCoordinate::DegreesMinutesWithHemisphere
                << QString("27%1 28.055' N, 153%1 1.674' E").arg(DEGREES_SYMB);
        QTest::newRow("NE, dms, no hemisphere")
                << northEast << QCoordinate::DegreesMinutesSeconds
                << QString("27%1 28' 3.3\", 153%1 1' 40.4\"").arg(DEGREES_SYMB);
        QTest::newRow("NE, dms, hemisphere")
                << northEast << QCoordinate::DegreesMinutesSecondsWithHemisphere
                << QString("27%1 28' 3.3\" N, 153%1 1' 40.4\" E").arg(DEGREES_SYMB);

        QTest::newRow("SE, dd, no hemisphere")
                << southEast << QCoordinate::DecimalDegrees
                << QString("-27.46758%1, 153.02789%1").arg(DEGREES_SYMB);
        QTest::newRow("SE, dd, hemisphere")
                << southEast << QCoordinate::DecimalDegreesWithHemisphere
                << QString("27.46758%1 S, 153.02789%1 E").arg(DEGREES_SYMB);
        QTest::newRow("SE, dm, no hemisphere")
                << southEast << QCoordinate::DegreesMinutes
                << QString("-27%1 28.055', 153%1 1.674'").arg(DEGREES_SYMB);
        QTest::newRow("SE, dm, hemisphere")
                << southEast << QCoordinate::DegreesMinutesWithHemisphere
                << QString("27%1 28.055' S, 153%1 1.674' E").arg(DEGREES_SYMB);
        QTest::newRow("SE, dms, no hemisphere")
                << southEast << QCoordinate::DegreesMinutesSeconds
                << QString("-27%1 28' 3.3\", 153%1 1' 40.4\"").arg(DEGREES_SYMB);
        QTest::newRow("SE, dms, hemisphere")
                << southEast << QCoordinate::DegreesMinutesSecondsWithHemisphere
                << QString("27%1 28' 3.3\" S, 153%1 1' 40.4\" E").arg(DEGREES_SYMB);

        QTest::newRow("NW, dd, no hemisphere")
                << northWest << QCoordinate::DecimalDegrees
                << QString("27.46758%1, -153.02789%1").arg(DEGREES_SYMB);
        QTest::newRow("NW, dd, hemisphere")
                << northWest << QCoordinate::DecimalDegreesWithHemisphere
                << QString("27.46758%1 N, 153.02789%1 W").arg(DEGREES_SYMB);
        QTest::newRow("NW, dm, no hemisphere")
                << northWest << QCoordinate::DegreesMinutes
                << QString("27%1 28.055', -153%1 1.674'").arg(DEGREES_SYMB);
        QTest::newRow("NW, dm, hemisphere")
                << northWest << QCoordinate::DegreesMinutesWithHemisphere
                << QString("27%1 28.055' N, 153%1 1.674' W").arg(DEGREES_SYMB);
        QTest::newRow("NW, dms, no hemisphere")
                << northWest << QCoordinate::DegreesMinutesSeconds
                << QString("27%1 28' 3.3\", -153%1 1' 40.4\"").arg(DEGREES_SYMB);
        QTest::newRow("NW, dms, hemisphere")
                << northWest << QCoordinate::DegreesMinutesSecondsWithHemisphere
                << QString("27%1 28' 3.3\" N, 153%1 1' 40.4\" W").arg(DEGREES_SYMB);

        QTest::newRow("SW, dd, no hemisphere")
                << southWest << QCoordinate::DecimalDegrees
                << QString("-27.46758%1, -153.02789%1").arg(DEGREES_SYMB);
        QTest::newRow("SW, dd, hemisphere")
                << southWest << QCoordinate::DecimalDegreesWithHemisphere
                << QString("27.46758%1 S, 153.02789%1 W").arg(DEGREES_SYMB);
        QTest::newRow("SW, dm, no hemisphere")
                << southWest << QCoordinate::DegreesMinutes
                << QString("-27%1 28.055', -153%1 1.674'").arg(DEGREES_SYMB);
        QTest::newRow("SW, dm, hemisphere")
                << southWest << QCoordinate::DegreesMinutesWithHemisphere
                << QString("27%1 28.055' S, 153%1 1.674' W").arg(DEGREES_SYMB);
        QTest::newRow("SW, dms, no hemisphere")
                << southWest << QCoordinate::DegreesMinutesSeconds
                << QString("-27%1 28' 3.3\", -153%1 1' 40.4\"").arg(DEGREES_SYMB);
        QTest::newRow("SW, dms, hemisphere")
                << southWest << QCoordinate::DegreesMinutesSecondsWithHemisphere
                << QString("27%1 28' 3.3\" S, 153%1 1' 40.4\" W").arg(DEGREES_SYMB);
    }

    void degreesToString()
    {
        QFETCH(QCoordinate, coord);
        QFETCH(QCoordinate::CoordinateFormat, format);
        QFETCH(QString, string);

        QCOMPARE(coord.toString(format), string);
    }

};

QTEST_MAIN(tst_QCoordinate)
#include "tst_qcoordinate.moc"
