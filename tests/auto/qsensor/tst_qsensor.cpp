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

//TESTED_COMPONENT=src/sensors

#include <QObject>
#include <QTest>
#include <QDebug>
#include <QSettings>
#include <QFile>

#include "qsensor.h"
#include "test_sensor.h"
#include "test_sensorimpl.h"
#include "qambientlightsensor.h"
#include "qorientationsensor.h"
#include "qtapsensor.h"

// The unit test needs to change the behaviour of the library. It does this
// through an exported but undocumented function.
QTM_BEGIN_NAMESPACE
Q_SENSORS_EXPORT void sensors_unit_test_hook(int index);
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class MyFilter : public TestSensorFilter
{
    bool filter(TestSensorReading *reading)
    {
        return reading->test();
    }
};

/*
    Unit test for QSensor class.
*/
class tst_QSensor : public QObject
{
    Q_OBJECT

public:
    tst_QSensor()
    {
        sensors_unit_test_hook(0); // change some flags the library uses
    }

private slots:
    void initTestCase()
    {
        QSettings settings(QLatin1String("Nokia"), QLatin1String("Sensors"));
        settings.clear();
    }

    void cleanupTestCase()
    {
        QSettings settings(QLatin1String("Nokia"), QLatin1String("Sensors"));
        settings.clear();

#ifdef WAIT_AT_END
        QFile _stdin;
        _stdin.open(1, QIODevice::ReadOnly);
        _stdin.readLine();
#endif
    }


    void testTypeRegistered()
    {
        QList<QByteArray> expected;
        expected << "QAccelerometer" << TestSensor::type;
        QList<QByteArray> actual = QSensor::sensorTypes();
        QCOMPARE(actual, expected);
    }

    void testSensorRegistered()
    {
        QList<QByteArray> expected;
        expected << testsensorimpl::id << "test sensor 2";
        QList<QByteArray> actual = QSensor::sensorsForType(TestSensor::type);
        QCOMPARE(actual, expected);
    }

    void testSensorDefault()
    {
        QByteArray expected = testsensorimpl::id;
        QByteArray actual = QSensor::defaultSensorForType(TestSensor::type);
        QCOMPARE(actual, expected);
    }

    void testBadDefaultFromConfig()
    {
        QSettings settings(QLatin1String("Nokia"), QLatin1String("Sensors"));
        settings.setValue(QString(QLatin1String("Default/%1")).arg(QString::fromLatin1(TestSensor::type)), QByteArray("bogus id"));
        settings.sync();

        QByteArray expected = testsensorimpl::id;
        QByteArray actual = QSensor::defaultSensorForType(TestSensor::type);
        QCOMPARE(actual, expected);
    }

    void testGoodDefaultFromConfig()
    {
        QSettings settings(QLatin1String("Nokia"), QLatin1String("Sensors"));
        settings.setValue(QString(QLatin1String("Default/%1")).arg(QString::fromLatin1(TestSensor::type)), QByteArray(testsensorimpl::id));
        settings.sync();

        QByteArray expected = testsensorimpl::id;
        QByteArray actual = QSensor::defaultSensorForType(TestSensor::type);
        QCOMPARE(actual, expected);
    }

    void testNoSensorsForType()
    {
        QList<QByteArray> expected;
        QList<QByteArray> actual = QSensor::sensorsForType("bogus type");
        QCOMPARE(actual, expected);
    }

    void testNoDefaultForType()
    {
        QByteArray expected;
        QByteArray actual = QSensor::defaultSensorForType("bogus type");
        QCOMPARE(actual, expected);
    }

    void testCreation()
    {
        TestSensor sensor;
        sensor.connectToBackend();
        QByteArray expected = testsensorimpl::id;
        QByteArray actual = sensor.identifier();
        QCOMPARE(actual, expected);
    }

    void testBadDefaultCreation()
    {
        QSettings settings(QLatin1String("Nokia"), QLatin1String("Sensors"));
        settings.setValue(QString(QLatin1String("Default/%1")).arg(QString::fromLatin1(TestSensor::type)), QByteArray("test sensor 2"));
        settings.sync();

        TestSensor sensor;
        QTest::ignoreMessage(QtWarningMsg, "Can't create backend \"test sensor 2\" ");
        sensor.connectToBackend();
        QByteArray expected = testsensorimpl::id;
        QByteArray actual = sensor.identifier();
        QCOMPARE(actual, expected);
    }

    void testBadCreation()
    {
        QSensor sensor("bogus type");
        sensor.connectToBackend();
        QByteArray expected; // should be null
        QByteArray actual = sensor.identifier();
        QCOMPARE(actual, expected);
    }

    void resetSettings()
    {
        QSettings settings(QLatin1String("Nokia"), QLatin1String("Sensors"));
        settings.setValue(QString(QLatin1String("Default/%1")).arg(QString::fromLatin1(TestSensor::type)), QByteArray(testsensorimpl::id));
        settings.sync();
    }

    void testTimestamp()
    {
        TestSensor sensor;
        sensor.connectToBackend();
        QVERIFY(sensor.reading() != 0);
        quint64 timestamp = sensor.reading()->timestamp();
        QVERIFY(timestamp == qtimestamp());
    }

    void testStart()
    {
        TestSensor sensor;
        sensor.start();
        QVERIFY(sensor.isActive());
        sensor.start();
        QVERIFY(sensor.isActive());
    }

    void testBadStart()
    {
        QSensor sensor("bogus type");
        sensor.start();
        QVERIFY(!sensor.isActive());
    }

    void testStop()
    {
        TestSensor sensor;
        sensor.stop();
        QVERIFY(!sensor.isActive());
        sensor.start();
        QVERIFY(sensor.isActive());
        sensor.stop();
        QVERIFY(!sensor.isActive());
    }

    void testMetaData()
    {
        TestSensor sensor;
        sensor.connectToBackend();

        QString actual = sensor.description();
        QString expected = "sensor description";
        QCOMPARE(actual, expected);
        sensor.outputRange();
        sensor.setOutputRange(1);
        sensor.outputRanges();
        sensor.availableDataRates();
        sensor.setDataRate(100);
        sensor.dataRate();
        sensor.isBusy();
        sensor.error();
        sensor.isConnectedToBackend();

        TestSensorReading *reading = sensor.reading();
        reading->test();
        QCOMPARE(reading->valueCount(), 1);
        reading->value(0).toBool();
    }

    void testMetaData2()
    {
        TestSensor sensor;
        sensor.setProperty("doThis", "rates(0)");
        QTest::ignoreMessage(QtWarningMsg, "ERROR: Cannot call QSensorBackend::setDataRates with 0 ");
        sensor.connectToBackend();
    }

    void testMetaData3()
    {
        TestSensor sensor;
        sensor.setProperty("doThis", "rates");
        sensor.connectToBackend();

        sensor.availableDataRates();
    }

    void testFilter()
    {
        TestSensor sensor;
        sensor.connectToBackend();

        MyFilter filter;
        sensor.addFilter(&filter);
        sensor.removeFilter(&filter);
        sensor.addFilter(&filter);
    }

    void testStart2()
    {
        TestSensor sensor;
        sensor.connectToBackend();

        sensor.setProperty("doThis", "busy");
        sensor.start();
        QVERIFY(!sensor.isActive());

        sensor.setProperty("doThis", "stop");
        sensor.start();
        QVERIFY(!sensor.isActive());

        sensor.setProperty("doThis", "error");
        sensor.start();
        QVERIFY(sensor.error() == 1);
        QVERIFY(sensor.isActive());

        MyFilter filter;
        sensor.addFilter(&filter);
        sensor.setProperty("doThis", "setFalse");
        sensor.start();
        QVERIFY(sensor.isActive());

        sensor.setProperty("doThis", "setTrue");
        sensor.start();
        QVERIFY(sensor.isActive());

    }

    void testSetBadRate()
    {
        TestSensor sensor;
        sensor.connectToBackend();

        QTest::ignoreMessage(QtWarningMsg, "setDataRate: rate 300 is not supported by the sensor. ");
        sensor.setDataRate(300);
    }

    void testEnumHandling()
    {
        {
            QAmbientLightReading reading;
            for (int i = 0; i <= 6; i++) {
                QAmbientLightReading::LightLevel setting = static_cast<QAmbientLightReading::LightLevel>(i);
                QAmbientLightReading::LightLevel expected = setting;
                if (i == 6)
                    expected = QAmbientLightReading::Undefined;
                reading.setLightLevel(setting);
                QCOMPARE(reading.lightLevel(), expected);
            }
        }

        {
            QOrientationReading reading;
            for (int i = 0; i <= 7; i++) {
                QOrientationReading::Orientation setting = static_cast<QOrientationReading::Orientation>(i);
                QOrientationReading::Orientation expected = setting;
                if (i == 7)
                    expected = QOrientationReading::Undefined;
                reading.setOrientation(setting);
                QCOMPARE(reading.orientation(), expected);
            }
        }

        {
            QTapReading reading;
            reading.setTapDirection(QTapReading::Undefined);
            QCOMPARE(reading.tapDirection(), QTapReading::Undefined);
            reading.setTapDirection(QTapReading::X_Pos);
            QCOMPARE(reading.tapDirection(), QTapReading::X_Pos);
            reading.setTapDirection(QTapReading::X_Neg);
            QCOMPARE(reading.tapDirection(), QTapReading::X_Neg);
            reading.setTapDirection(QTapReading::Y_Pos);
            QCOMPARE(reading.tapDirection(), QTapReading::Y_Pos);
            reading.setTapDirection(QTapReading::Y_Neg);
            QCOMPARE(reading.tapDirection(), QTapReading::Y_Neg);
            reading.setTapDirection(QTapReading::Z_Pos);
            QCOMPARE(reading.tapDirection(), QTapReading::Z_Pos);
            reading.setTapDirection(QTapReading::Z_Neg);
            QCOMPARE(reading.tapDirection(), QTapReading::Z_Neg);
            reading.setTapDirection(static_cast<QTapReading::TapDirection>(1000));
            QCOMPARE(reading.tapDirection(), QTapReading::Undefined);
        }
    }

    // This test must be LAST or it will interfere with the other tests
    void testLoadingPlugins()
    {
        // Go ahead and load the actual plugins (as a test that plugin loading works)
        sensors_unit_test_hook(1);

        // Hmm... There's no real way to tell if this worked or not.
        // If it doesn't work the unit test will probably crash.
        // That's what it did on Symbian before plugin loading was fixed.
    }
};

QTEST_MAIN(tst_QSensor)

#include "tst_qsensor.moc"
