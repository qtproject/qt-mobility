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

    void testRecursiveLoadPlugins()
    {
        // The logic for the test is in test_sensorplugin.cpp (which warns and aborts if the test fails)
        (void)QSensor::sensorTypes();
    }

    void testTypeRegistered()
    {
        QList<QByteArray> expected;
        expected << "QAccelerometer" << TestSensor::type;
        QList<QByteArray> actual = QSensor::sensorTypes();
        qSort(actual); // The actual list is not in a defined order
        QCOMPARE(actual, expected);
    }

    void testSensorRegistered()
    {
        QList<QByteArray> expected;
        expected << "test sensor 2" << testsensorimpl::id;
        QList<QByteArray> actual = QSensor::sensorsForType(TestSensor::type);
        qSort(actual); // The actual list is not in a defined order
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

        settings.clear();
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

        settings.clear();
    }

    void testBadCreation()
    {
        QSensor sensor("bogus type");
        sensor.connectToBackend();
        QByteArray expected; // should be null
        QByteArray actual = sensor.identifier();
        QCOMPARE(actual, expected);
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

    void testSetBadDataRate()
    {
        TestSensor sensor;
        sensor.connectToBackend();

        QTest::ignoreMessage(QtWarningMsg, "setDataRate: 300 is not supported by the sensor. ");
        sensor.setDataRate(300);
        QCOMPARE(sensor.dataRate(), 0);
    }

    void testSetBadDataRateWhenNotConnected()
    {
        TestSensor sensor;
        sensor.setDataRate(300);
        QCOMPARE(sensor.dataRate(), 300);
        sensor.setDataRate(350);
        QTest::ignoreMessage(QtWarningMsg, "setDataRate: 350 is not supported by the sensor. ");
        sensor.connectToBackend();
        QCOMPARE(sensor.dataRate(), 0);
    }

    void testSetBadOutputRange()
    {
        TestSensor sensor;
        sensor.connectToBackend();

        QTest::ignoreMessage(QtWarningMsg, "setOutputRange: 300 is not supported by the sensor. ");
        sensor.setOutputRange(300);
        QCOMPARE(sensor.outputRange(), -1);
    }

    void testSetBadOutputRangeWhenNotConnected()
    {
        TestSensor sensor;
        sensor.setOutputRange(300);
        QCOMPARE(sensor.outputRange(), 300);
        sensor.setOutputRange(350);
        QTest::ignoreMessage(QtWarningMsg, "setOutputRange: 350 is not supported by the sensor. ");
        sensor.connectToBackend();
        QCOMPARE(sensor.outputRange(), -1);
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
            // Directions can be ORed together
            reading.setTapDirection(QTapReading::X_Both);
            QCOMPARE(reading.tapDirection(), QTapReading::X_Both);
            reading.setTapDirection(QTapReading::Y_Both);
            QCOMPARE(reading.tapDirection(), QTapReading::Y_Both);
            reading.setTapDirection(QTapReading::Z_Both);
            QCOMPARE(reading.tapDirection(), QTapReading::Z_Both);
            // You can't set just the Axis
            reading.setTapDirection(QTapReading::X);
            QCOMPARE(reading.tapDirection(), QTapReading::Undefined);
            reading.setTapDirection(QTapReading::Y);
            QCOMPARE(reading.tapDirection(), QTapReading::Undefined);
            reading.setTapDirection(QTapReading::Z);
            QCOMPARE(reading.tapDirection(), QTapReading::Undefined);
            reading.setTapDirection(static_cast<QTapReading::TapDirection>(0x1000));
            QCOMPARE(reading.tapDirection(), QTapReading::Undefined);
        }
    }

    void testDynamicDefaultsAndGenericHandling()
    {
        TestSensor sensor;
        QByteArray expected;
        QByteArray actual;

        // The default for this type is null
        expected = QByteArray();
        actual = QSensor::defaultSensorForType("random");
        QCOMPARE(expected, actual);

        // Register a bogus backend
        QSensorManager::registerBackend("random", "generic.random", 0);

        // The default for this type is the newly-registered backend
        expected = "generic.random";
        actual = QSensor::defaultSensorForType("random");
        QCOMPARE(expected, actual);

        // Register a non-generic bogus backend
        QSensorManager::registerBackend("random", "not.generic.random", 0);

        // The default for this type is the newly-registered backend
        expected = "not.generic.random";
        actual = QSensor::defaultSensorForType("random");
        QCOMPARE(expected, actual);

        // Unregister a non-generic bogus backend
        QSensorManager::unregisterBackend("random", "not.generic.random");

        // The default for this type is the generic backend
        expected = "generic.random";
        actual = QSensor::defaultSensorForType("random");
        QCOMPARE(expected, actual);

        // Unregister a bogus backend
        QSensorManager::unregisterBackend("random", "generic.random");

        // The default for this type is null again
        expected = QByteArray();
        actual = QSensor::defaultSensorForType("random");
        QCOMPARE(expected, actual);
    }

    void testSensorsChangedSignal()
    {
        TestSensor sensor;

        // Register a bogus backend
        sensor.sensorsChangedEmitted = false;
        QSensorManager::registerBackend("a random type", "a random id", 0);
        QVERIFY(sensor.sensorsChangedEmitted);

        // Register it again (creates a warning)
        sensor.sensorsChangedEmitted = false;
        QTest::ignoreMessage(QtWarningMsg, "A backend with type \"a random type\" and identifier \"a random id\" has already been registered! ");
        QSensorManager::registerBackend("a random type", "a random id", 0);
        QVERIFY(!sensor.sensorsChangedEmitted);

        // Unregister a bogus backend
        sensor.sensorsChangedEmitted = false;
        QSensorManager::unregisterBackend("a random type", "a random id");
        QVERIFY(sensor.sensorsChangedEmitted);

        // Unregister an unknown identifier
        sensor.sensorsChangedEmitted = false;
        QTest::ignoreMessage(QtWarningMsg, "Identifier \"a random id\" is not registered ");
        QSensorManager::unregisterBackend(TestSensor::type, "a random id");
        QVERIFY(!sensor.sensorsChangedEmitted);

        // Unregister for an unknown type
        sensor.sensorsChangedEmitted = false;
        QTest::ignoreMessage(QtWarningMsg, "No backends of type \"foo\" are registered ");
        QSensorManager::unregisterBackend("foo", "bar");
        QVERIFY(!sensor.sensorsChangedEmitted);

        // Make sure we've cleaned up the list of available types
        QList<QByteArray> expected;
        expected << "QAccelerometer" << TestSensor::type;
        QList<QByteArray> actual = QSensor::sensorTypes();
        QCOMPARE(actual, expected);
    }

    // This test must be LAST or it will interfere with the other tests
    void testLoadingPlugins()
    {
        // Unregister this one we did before... otherwise we may get a duplicate registration warning
        QSensorManager::unregisterBackend("QAccelerometer", "dummy.accelerometer");

        // Go ahead and load the actual plugins (as a test that plugin loading works)
        sensors_unit_test_hook(1);

        // Hmm... There's no real way to tell if this worked or not.
        // If it doesn't work the unit test will probably crash.
        // That's what it did on Symbian before plugin loading was fixed.
    }
};

QTEST_MAIN(tst_QSensor)

#include "tst_qsensor.moc"
