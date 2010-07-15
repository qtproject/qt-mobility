/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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

#include <QtTest/QtTest>
#include <QtTest/QSignalSpy>
#include <QMetaObject>
#include <QDateTime>
#include <QtDeclarative/qdeclarativeengine.h>
#include <QtDeclarative/qdeclarativecomponent.h>
// #include "qdeclarativeposition_p.h"
#include <QString>

// Eventually these will make it into qtestcase.h
// but we might need to tweak the timeout values here.
#ifndef QTRY_COMPARE
#define QTRY_COMPARE(__expr, __expected) \
    do { \
        const int __step = 50; \
        const int __timeout = 10000; \
        if ((__expr) != (__expected)) { \
            QTest::qWait(0); \
        } \
        for (int __i = 0; __i < __timeout && ((__expr) != (__expected)); __i+=__step) { \
            QTest::qWait(__step); \
        } \
        QCOMPARE(__expr, __expected); \
    } while(0)
#endif

#ifndef QTRY_VERIFY
#define QTRY_VERIFY(__expr) \
        do { \
        const int __step = 50; \
        const int __timeout = 10000; \
        if (!(__expr)) { \
            QTest::qWait(0); \
        } \
        for (int __i = 0; __i < __timeout && !(__expr); __i+=__step) { \
            QTest::qWait(__step); \
        } \
        QVERIFY(__expr); \
    } while(0)
#endif

#ifndef QTRY_WAIT
#define QTRY_WAIT(code, __expr) \
        do { \
        const int __step = 50; \
        const int __timeout = 10000; \
        if (!(__expr)) { \
            QTest::qWait(0); \
        } \
        for (int __i = 0; __i < __timeout && !(__expr); __i+=__step) { \
            do { code } while(0); \
            QTest::qWait(__step); \
        } \
    } while(0)
#endif

#ifdef Q_OS_SYMBIAN
// In Symbian OS test data is located in applications private dir
#define SRCDIR "."
#endif

// Helper functions to avoid repetitive signal spy usage
QList<QSignalSpy*> createSpies(QObject* source_obj);
QList<QSignalSpy*> createSourceSpies(QObject* source_obj);
QList<QSignalSpy*> createPositionSpies(QObject* position_obj);
bool spiesAreEmpty(QList<QSignalSpy*> spies);
bool spiesAreNotEmpty(QList<QSignalSpy*> spies);
void clearSpies(QList<QSignalSpy*> spies);

//QTM_USE_NAMESPACE

class tst_QDeclarativePosition : public QObject
{
Q_OBJECT

public:
    tst_QDeclarativePosition();
    virtual ~tst_QDeclarativePosition();

public slots:
    void initTestCase();
    void cleanupTestCase();
    
private slots:

    void construction();
    void construction_data();

    void defaultProperties();

    void basicNmeaSource();
    void basicNmeaSource_data();
    
private:
    // Engine is needed for instantiating declarative components
    QDeclarativeEngine engine;
};

tst_QDeclarativePosition::tst_QDeclarativePosition()
{
}

tst_QDeclarativePosition::~tst_QDeclarativePosition()
{
}

void tst_QDeclarativePosition::initTestCase()
{ 
}

void tst_QDeclarativePosition::cleanupTestCase()
{
}


/*
  Tests Position element instantiating from plugin.
*/


void tst_QDeclarativePosition::construction()
{
    QFETCH(QString, componentString);
    QFETCH(QString, expectedClassName);
    QFETCH(bool, shouldSucceed);
    // Component encapsulates one component description
    QDeclarativeComponent component(&engine);
    component.setData(componentString.toLatin1(), QUrl::fromLocalFile(""));
    QObject* obj = component.create();

    if (shouldSucceed) {
        if (obj == 0) {
            if (component.isError()) {
                qDebug() << "Error(s) occured when trying to instantiate. QDeclarativeComponent::errors(): ";
                qDebug() << component.errors();
            }
            qWarning("--------- ------------- ------------- ---------------------- ------------ ");
            qWarning("--------- could not instantiate components from location   -------------- ");
            qWarning("--------- declarative plugin. make sure it is built and found ------------");
            qWarning(" ---------under {QTDIR}/imports, or c:/sys/bin on Symbian     ----------- ");
            qWarning("--------- ------------- ------------- ---------------------- ------------ ");
        }
        QVERIFY(obj != 0);
        QCOMPARE(obj->metaObject()->className(), expectedClassName.toAscii().constData());
    } else {
        QVERIFY(obj == 0);
    }
    // The above check is a bit clumsy, but testing with qobject_cast<> will not work,
    // because the obj is instantiated from a aplugin _but_ also the header has been compiled in this
    // test binary --> static meta object addresses do not match. Test below indicates why the 
    // qobject_cast would fail, although the types are actually same. (This could probably be worked around by
    // declaring the interface of QDeclarativePosition into the meta-system).
    // qDebug() << "Address of the plugin-instantiated metaobject of the object: " << QString::number(uint(obj->metaObject()));
    // qDebug() << "Whereas the address of the metaobject compiled in this binary is: " << QString::number(uint(&((QDeclarativePosition*)0)->staticMetaObject));
    // QDeclarativePosition* position = qobject_cast<QDeclarativePosition*>(obj);
    // QVERIFY(position != 0);
    delete obj;
}

void tst_QDeclarativePosition::construction_data()
{
    QTest::addColumn<QString>("expectedClassName");
    QTest::addColumn<QString>("componentString");
    QTest::addColumn<bool>("shouldSucceed");
    // PositionSource
    QTest::newRow("Source: No properties") <<  "QDeclarativePositionSource" << "import Qt 4.7 \n import QtMobility.location 1.0 \n PositionSource {}" << true;
    QTest::newRow("Source: Only id property") << "QDeclarativePositionSource" << "import Qt 4.7 \n import QtMobility.location 1.0 \n PositionSource {id: positionId}" << true;
    QTest::newRow("Source: All write properties") << "QDeclarativePositionSource" << "import Qt 4.7 \n import QtMobility.location 1.0 \n PositionSource {id: positionId; updateInterval: 1000; nmeaSource: \"nonexistentfile.txt\"}" << true;
    QTest::newRow("Source: Nonexistent property") << "QDeclarativePositionSource" << "import Qt 4.7 \n import QtMobility.location 1.0 \n PositionSource {id: positionId; nonExistentProperty: 1980}" << false;
    // Position
    QTest::newRow("Position: No properties") << "QDeclarativePosition" << "import Qt 4.7 \n import QtMobility.location 1.0 \n Position {}" << true;
    QTest::newRow("Position: Only id property") << "QDeclarativePosition" << "import Qt 4.7 \n import QtMobility.location 1.0 \n Position {id: position}" << true;
    QTest::newRow("Position: All write properties") << "QDeclarativePosition" << "import Qt 4.7 \n import QtMobility.location 1.0 \n Position {id: position; timestamp: \"2010-07-09\"; longtitude: 61.373459; latitude: 21.611216; altitude: 325; speed: 15}" << true;
}

/*
    Tests the default values of properties
*/
void tst_QDeclarativePosition::defaultProperties()
{
    QString componentString("import Qt 4.7 \n import QtMobility.location 1.0 \n PositionSource {id: positionId}");
    QDeclarativeComponent component(&engine);
    component.setData(componentString.toLatin1(), QUrl::fromLocalFile(""));
    QObject* source_obj = component.create();
    QVERIFY(source_obj != 0);
    // Verify position source default data
    QCOMPARE(source_obj->property("nmeaSource").toUrl(), QUrl());
    QCOMPARE(source_obj->property("updateInterval").toInt(), 0);
    // Get the position member variable
    QObject* position_obj = source_obj->property("position").value<QObject*>();
    QVERIFY(position_obj != 0);
    // Verify position default data
    QCOMPARE(position_obj->property("timestamp").toDateTime(), QDateTime());
    QCOMPARE(position_obj->property("latitude").toDouble(), static_cast<double>(0));
    QCOMPARE(position_obj->property("longtitude").toDouble(), static_cast<double>(0));
    QCOMPARE(position_obj->property("altitude").toDouble(), static_cast<double>(0));
    QCOMPARE(position_obj->property("speed").toDouble(), static_cast<double>(0));
    QCOMPARE(position_obj->property("latitudeValid").toBool(), false);
    QCOMPARE(position_obj->property("longtitudeValid").toBool(), false);
    QCOMPARE(position_obj->property("altitudeValid").toBool(), false);
    QCOMPARE(position_obj->property("speedValid").toBool(), false);

    delete source_obj;
}

/*
    Tests the nmea source basics
*/

void tst_QDeclarativePosition::basicNmeaSource()
{
    QFETCH(QString, updateMethod);
    QFETCH(int, repeats);

    qDebug() << "1. ----- Create PositionSource";
    QString componentString("import Qt 4.7 \n import QtMobility.location 1.0 \n PositionSource {id: positionId;}");
    QDeclarativeComponent component(&engine);
    component.setData(componentString.toLatin1(), QUrl::fromLocalFile(""));
    QObject* source_obj = component.create();

    QVERIFY(source_obj != 0);
    QObject* position_obj = source_obj->property("position").value<QObject*>();
    QVERIFY(position_obj != 0);

    qDebug() << "2. ----- Create spies and set NMEA source";
    QList<QSignalSpy*> spies = createSpies(source_obj);
    source_obj->setProperty("nmeaSource", ":/data/nmealog.txt");

    source_obj->setProperty("updateInterval", 500);
    QVERIFY(source_obj->property("nmeaSource").toUrl().isValid());

    qDebug() << "3. ----- Start updates and verify that relevant signals are received.";
    QSignalSpy positionChangedSpy(source_obj, SIGNAL(positionChanged()));
    if (updateMethod == "start") {
        source_obj->metaObject()->invokeMethod(source_obj, updateMethod.toLatin1().constData(), Qt::DirectConnection);
        QTRY_VERIFY(positionChangedSpy.count() == repeats);
    }  else  {
        for (int i = 0; i < repeats; i++) {
            source_obj->metaObject()->invokeMethod(source_obj, updateMethod.toLatin1().constData(), Qt::DirectConnection);
            QTRY_VERIFY(!positionChangedSpy.isEmpty());
            positionChangedSpy.clear();
        }
    }
    QVERIFY(spiesAreNotEmpty(spies));

    qDebug() << "4. ----- Stop updates and verify that signals are not received anymore.";
    source_obj->metaObject()->invokeMethod(source_obj, "stop", Qt::DirectConnection);
    clearSpies(spies);
    QVERIFY(spiesAreEmpty(spies));
    delete source_obj;
}

void tst_QDeclarativePosition::basicNmeaSource_data()
{
    QTest::addColumn<QString>("updateMethod");
    QTest::addColumn<int>("repeats");
    QTest::newRow("periodic updates") << "start" << 10;
    QTest::newRow("single shot update") << "update" << 10;
}

QList<QSignalSpy*> createSpies(QObject* source_obj)
{
    return createSourceSpies(source_obj) + createPositionSpies(source_obj->property("position").value<QObject*>());
}

QList<QSignalSpy*> createSourceSpies(QObject* source_obj)
{
    QList<QSignalSpy*> spies;
    spies.append(new QSignalSpy(source_obj, SIGNAL(positionChanged()))); // 0
    spies.append(new QSignalSpy(source_obj, SIGNAL(activeChanged(bool))));
    spies.append(new QSignalSpy(source_obj, SIGNAL(updateIntervalChanged(int))));
    spies.append(new QSignalSpy(source_obj, SIGNAL(nmeaSourceChanged(QUrl))));
    return spies;
}

QList<QSignalSpy*> createPositionSpies(QObject* position_obj)
{
    QList<QSignalSpy*> spies;
    spies.append(new QSignalSpy(position_obj, SIGNAL(latitudeChanged(double)))); //4
    spies.append(new QSignalSpy(position_obj, SIGNAL(latitudeValidChanged(bool))));
    spies.append(new QSignalSpy(position_obj, SIGNAL(longtitudeChanged(double))));
    spies.append(new QSignalSpy(position_obj, SIGNAL(longtitudeValidChanged(bool))));
    spies.append(new QSignalSpy(position_obj, SIGNAL(altitudeChanged(double))));
    spies.append(new QSignalSpy(position_obj, SIGNAL(altitudeValidChanged(bool))));
    spies.append(new QSignalSpy(position_obj, SIGNAL(speedChanged(double)))); // 10
    spies.append(new QSignalSpy(position_obj, SIGNAL(speedValidChanged(bool))));
    spies.append(new QSignalSpy(position_obj, SIGNAL(timestampChanged(QDateTime))));
    return spies;
}

bool spiesAreEmpty(QList<QSignalSpy*> spies)
{
    for (int i = 0; i < spies.size(); i++) {
        // qDebug() << "Spies are empty: " << i;
        if (!spies.at(i)->isEmpty())
            return false;
    }
    return true;
}

bool spiesAreNotEmpty(QList<QSignalSpy*> spies)
{
    for (int i = 0; i < spies.size(); i++) {
        // qDebug() << "Spies are not empty: " << i;
        if (spies.at(i)->isEmpty())
            return false;
    }
    return true;
}

void clearSpies(QList<QSignalSpy*> spies)
{
    for (int i = 0; i < spies.size(); i++) {
        spies.at(i)->clear();
    }
}

QTEST_MAIN(tst_QDeclarativePosition)
#include "tst_qdeclarativeposition.moc"
