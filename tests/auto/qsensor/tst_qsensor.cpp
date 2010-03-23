/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <QObject>
#include <QTest>
#include <QDebug>
#include <QSettings>

#include "qsensor.h"
#include "test_sensor.h"
#include "test_sensorimpl.h"

QTM_USE_NAMESPACE

/*
    Unit test for QSensor class.
*/
class tst_QSensor : public QObject
{
    Q_OBJECT

public:
    tst_QSensor()
    {
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
    }


    void testTypeRegistered()
    {
        QList<QByteArray> expected;
        expected << TestSensor::type;
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
        sensor.connect();
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
        sensor.connect();
        QByteArray expected = testsensorimpl::id;
        QByteArray actual = sensor.identifier();
        QCOMPARE(actual, expected);
    }

    void testBadCreation()
    {
        QSensor sensor("bogus type");
        sensor.connect();
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
        sensor.connect();
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
        sensor.connect();

        QString actual = sensor.description();
        QString expected = "sensor description";
        QCOMPARE(actual, expected);
    }
};

QTEST_MAIN(tst_QSensor)

#include "tst_qsensor.moc"
