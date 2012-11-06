/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "moduletest_configurability.h"
#include "testthread.h"

#include <QtTest/QtTest>
#include <QtDebug>
#include <QVariant>
#include <QValueSpaceSubscriber>
#include <QValueSpacePublisher>

#ifdef Q_OS_SYMBIAN
#include <centralrepository.h>
#include <e32property.h>
#include <featdiscovery.h>
#include <e32cmn.h>
#endif

const QString ownCrFullPath("/cr/0xE056F50B/1");
const QString ownCrFullPathString("/cr/0xE056F50B/5");
const QString ownCrPath("/cr/0xE056F50B");
const QString ownCrKey("1");
const QString ownPsFullPath("/ps/0xE056F50B/1");
const uint ownUid = 3763795211;
const int intProperty = 1;    
const int stringProperty = 3;

// ======== LOCAL FUNCTIONS ========

bool waitForSignal(QObject *sender, const char *signal, int timeout = 1000) {
    QEventLoop loop;
    QTimer timer;
    timer.setInterval(timeout);
    timer.setSingleShot(true);

    loop.connect(sender, signal, SLOT(quit()));
    loop.connect(&timer, SIGNAL(timeout()), SLOT(quit()));
    timer.start();
    loop.exec();

    return timer.isActive();
}

// Runs the event loop for a short while to allow all unhandled queued signals to be sent forward.
// This flushes the events, so that when we call waitForSignal we don't get any unhandled signals
// from previous test cases.
void flushEventLoop()
{
    QEventLoop loop;
    QTimer timer;
    QObject::connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
    timer.setSingleShot(true);
    timer.start(1000); // 1 second
    loop.exec();
}


// ======== MEMBER FUNCTIONS ========

void ModuletestConfigurability::initTestCase()
{
}

void ModuletestConfigurability::init()
{
    // set own cr key to default value
    QtMobility::QValueSpacePublisher publisher(ownCrPath, this);
    publisher.setValue(ownCrKey, QVariant(0));
    publisher.setValue(QString("5"), QString());
    
    // remove any defined ps properties:
    RProperty::Delete(intProperty);
    RProperty::Delete(stringProperty);
}

void ModuletestConfigurability::cleanup()
{
}

void ModuletestConfigurability::cleanupTestCase()
{
}

// TEST CASE: several different threads access the same cenrep value.
// In this test case the read and write operations are protected with a mutex
// in order to verify that the value was written successfully. That means
// that this test case doesn't test simultaneous read/write operations from
// multiple threads; it just tests that it is possible to read same value
// multiple threads.
void ModuletestConfigurability::threads()
{
    QSemaphore sem(0);
    QMutex mutex;
    ConfigurabilityTestThread *threads[10];
    for (int i=0; i<10; i++) {
        threads[i] = new ConfigurabilityTestThread(&sem, &mutex, 1);
        threads[i]->start();
    }
    sem.acquire(10); // wait for the threads to finish

    for (int i=0; i<10; i++) {
        if (threads[i]->error) {
            qDebug() << threads[i]->errorString;
            QVERIFY(false);
        }
    }
}

// TEST CASE: several different threads are accessing PS API simultaneously.
// Each thread is accessing a different cenrep or pubsub key.
void ModuletestConfigurability::threads2()
{
    QSemaphore sem(0);
    QMutex mutex;
    ConfigurabilityTestThread *threads[10];
 
    for (int i=0; i<10; i++) {
        threads[i] = new ConfigurabilityTestThread(&sem, &mutex, 2);
    }

    threads[0]->setPath(QString("/publishsubscribe_mt/cenrep"), QString("int"));
    threads[1]->setPath(QString("/publishsubscribe_mt/cenrep"), QString("int2"));
    threads[2]->setPath(QString("/publishsubscribe_mt/cenrep"), QString("int3"));
    threads[3]->setPath(QString("/publishsubscribe_mt/cenrep"), QString("int4"));
    threads[4]->setPath(QString("/publishsubscribe_mt/cenrep"), QString("int5"));
    threads[5]->setPath(QString("/publishsubscribe_mt/pubsub"), QString("int"));
    threads[6]->setPath(QString("/ps/0xE056F50B"), QString("0x50"));
    threads[7]->setPath(QString("/ps/0xE056F50B"), QString("0x51"));
    threads[8]->setPath(QString("/ps/0xE056F50B"), QString("0x52"));
    threads[9]->setPath(QString("/ps/0xE056F50B"), QString("0x53"));

    for (int i=0; i<10; i++) {
        threads[i]->start();
    }

    sem.acquire(10); // wait for the threads to finish
    
    bool fail = false;
    for (int i=0; i<10; i++) {
        if (threads[i]->error) {
            qDebug() << threads[i]->errorString;
            fail = true;
        }
    }
    QVERIFY(!fail);
}

// TEST CASE: basic read and and write operations to Symbian Central Repository
//            through Symbian Settings Layer
void ModuletestConfigurability::readAndSetCenrep_data()
{
    QTest::addColumn<QString>("writePath");
    QTest::addColumn<QString>("writeKey");
    QTest::addColumn<QString>("readPath");
    QTest::addColumn<QString>("readKey");
    QTest::addColumn<QVariant>("value");

    QTest::newRow("numeric cenrep path, hex and decimal notation point to same key")
            << QString("/cr/0xE056F50B/0x1") << QString()
            << QString("/cr/3763795211/1") << QString() << QVariant(123);
    QTest::newRow("numeric cenrep path and qcrml path point to same key")
            << QString("/cr/0xE056F50B/0x1") << QString()
            << QString("/publishsubscribe_mt/cenrep/int") << QString() << QVariant(124);
    QTest::newRow("same as above but change the read and write keys around")
            << QString("/publishsubscribe_mt/cenrep") << QString("int")
            << QString("/cr/0xE056F50B/0x1") << QString() << QVariant(125);
    QTest::newRow("read and write string to cenrep")
            << QString("/publishsubscribe_mt/cenrep") << QString("string")
            << QString("/cr/0xE056F50B/0x5") << QString() << QVariant(QString("testString1"));
    QTest::newRow("read and write double to cenrep")
            << QString("/cr/0xE056F50B") << QString("0x2")
            << QString("/publishsubscribe_mt/cenrep/real") << QString() << QVariant(3.14);
}

void ModuletestConfigurability::readAndSetCenrep()
{
    flushEventLoop();

    QFETCH(QString, writePath);
    QFETCH(QString, writeKey);
    QFETCH(QString, readPath);
    QFETCH(QString, readKey);
    QFETCH(QVariant, value);

    QtMobility::QValueSpaceSubscriber subscriber(readPath, this);
    QtMobility::QValueSpacePublisher publisher(writePath, this);

    // using just signalspy doesn't emit connectNotify signal, so QValueSpaceSubscriber
    // doesn't start listening to change event.
    //   ==> connect the signal to dummy slot
    connect(&subscriber, SIGNAL(contentsChanged()), this, SLOT(dummy()));
    QSignalSpy spy(&subscriber, SIGNAL(contentsChanged()));

    publisher.setValue(writeKey, value);

    // wait for the change notification (timout: 5 seconds)
    QVERIFY(waitForSignal(&subscriber, SIGNAL(contentsChanged()), 5000));

    QCOMPARE(spy.count(), 1);
    QCOMPARE(subscriber.value(readKey), value);
    disconnect(&subscriber, SIGNAL(contentsChanged()), this, SLOT(dummy()));
}

// TEST CASE: basic read and and write operations to Symbian RProperty
//            through Symbian Settings Layer
void ModuletestConfigurability::readAndSetPubsub_data()
{
    QTest::addColumn<QString>("writePath");
    QTest::addColumn<QString>("writeKey");
    QTest::addColumn<QString>("readPath");
    QTest::addColumn<QString>("readKey");
    QTest::addColumn<QVariant>("value");

    QTest::newRow("numeric pubsub path, hex and decimal notation point to same key")
            << QString("/ps/0xE056F50B/0x1") << QString()
            << QString("/ps/3763795211/1") << QString() << QVariant(123);
    QTest::newRow("numeric pubsub path and qcrml path point to same key")
            << QString("/ps/0xE056F50B/0x1") << QString()
            << QString("/publishsubscribe_mt/pubsub/int") << QString() << QVariant(124);
    QTest::newRow("same as above but change the read and write keys around")
            << QString("/publishsubscribe_mt/pubsub") << QString("int")
            << QString("/ps/0xE056F50B/0x1") << QString() << QVariant(125);
    QTest::newRow("read and write string to pubusb")
            << QString("/publishsubscribe_mt/pubsub") << QString("string")
            << QString("/ps/0xE056F50B/0x3") << QString() << QVariant(QString("testString1"));
    QTest::newRow("read and write double to pubsub")
            << QString("/ps/0xE056F50B") << QString("0x3")
            << QString("/publishsubscribe_mt/pubsub/string") << QString() << QVariant(3.14);
}

void ModuletestConfigurability::readAndSetPubsub()
{
    flushEventLoop();

    QFETCH(QString, writePath);
    QFETCH(QString, writeKey);
    QFETCH(QString, readPath);
    QFETCH(QString, readKey);
    QFETCH(QVariant, value);

    QtMobility::QValueSpaceSubscriber subscriber(readPath, this);
    QtMobility::QValueSpacePublisher publisher(writePath, this);

    // it is not possible to listen to changes to an undefined property (Qt Mobility restriction)
    // so set some value here
    publisher.setValue(writeKey, (value.type()==QVariant::Int ? QVariant(0) : QVariant()));

    // using just signalspy doesn't emit connectNotify signal, so QValueSpaceSubscriber
    // doesn't start listening to change event.
    //   ==> connect the signal to dummy slot
    connect(&subscriber, SIGNAL(contentsChanged()), this, SLOT(dummy()));
    QSignalSpy spy(&subscriber, SIGNAL(contentsChanged()));

    publisher.setValue(writeKey, value);

    // wait for the change notification (timout: 5 seconds)
    QVERIFY(waitForSignal(&subscriber, SIGNAL(contentsChanged()), 5000));

    QCOMPARE(spy.count(), 1);
    QCOMPARE(subscriber.value(readKey), value);
}

// TEST CASE: verify that the numeric path parsing can handle invalid paths gracefully.
void ModuletestConfigurability::invalidPath_data()
{
    QTest::addColumn<QString>("path");
    QTest::addColumn<bool>("valid");
    QTest::newRow("cr: too many parts") << QString("/cr/0xE056F50B/0x1/0x2") << false;
    QTest::newRow("cr: too few parts") << QString("/cr/0xE056F50B") << false;
    QTest::newRow("fm: too many parts") << QString("/fm/0x1/0x2") << false;
    QTest::newRow("ps: number parsing") << QString("/ps/E056F50B/1") << false;
    QTest::newRow("cr: valid path") << QString("/cr/0xE056F50B/1") << true;
    QTest::newRow("fm: valid path") << QString("/fm/0x1") << true;
}

void ModuletestConfigurability::invalidPath()
{
    // basically this tests that the code doesn't crash with malformed paths.
    // if the path is illegal QValueSpaceSubscriber::value returns the default value.
    // (same as valid but undefined path)

    const QVariant KDefaultValue(-9999);
    QFETCH(QString, path);
    QFETCH(bool, valid);
    QtMobility::QValueSpaceSubscriber subscriber(path, this);

    QCOMPARE((subscriber.value(QString(), KDefaultValue) != KDefaultValue), valid);
}

// TEST CASE: verify that a string written to central repository through QValueSpaceSubscriber
//            can be read through native API (CRepository)
void ModuletestConfigurability::cenrepInteroperabilityQtToSymbian_data()
{
    QTest::addColumn<QString>("path");
    QTest::addColumn<QString>("key");

    QTest::newRow("numeric cenrep path") << QString("/cr/0xE056F50B/5") << QString();
    QTest::newRow("qrml cenrep path") << QString("/publishsubscribe_mt/cenrep") << QString("string");
}

void ModuletestConfigurability::cenrepInteroperabilityQtToSymbian()
{
    QFETCH(QString, path);
    QFETCH(QString, key);

    TUid uid;
    uid.iUid = ownUid;
    CRepository *repo = CRepository::NewL(uid);
    
    const QString testString("teststring");
    
    QtMobility::QValueSpacePublisher publisher(path, this);

    // write the string in a format that can be understood by Symbian code:
    QTextCodec *codec = QTextCodec::codecForName("UTF-16");
    if (codec) {
        QByteArray writeValue = codec->fromUnicode(testString);
        publisher.setValue(key, writeValue);
    }
    HBufC *res = HBufC::New(NCentralRepositoryConstants::KMaxUnicodeStringLength);
    TPtr ptr = res->Des();
    
    int err = repo->Get(5, ptr);
    QVERIFY(!err);
    
    QString result = QString::fromUtf16(ptr.Ptr(), ptr.Length());
    QCOMPARE(result, testString);
    delete res;
}

// TEST CASE: verify that string written to central repository through CRepository
//            can be read through QValueSpaceSubscriber.
void ModuletestConfigurability::cenrepInteroperabilitySymbianToQt_data()
{
    QTest::addColumn<QString>("path");

    QTest::newRow("numeric cenrep path") << QString("/cr/0xE056F50B/5?raw");
    QTest::newRow("qrml cenrep path") << QString("/publishsubscribe_mt/cenrep/string?raw");
}

void ModuletestConfigurability::cenrepInteroperabilitySymbianToQt()
{
    QFETCH(QString, path);
    
    TUid uid;
    uid.iUid = ownUid;
    CRepository *repo = CRepository::NewL(uid);
    
    int err = repo->Set(5, _L("testString"));
    QVERIFY(!err);

    QtMobility::QValueSpaceSubscriber subscriber(path, this);

    QVariant res = subscriber.value();
    QByteArray resBA = res.toByteArray();
    
    // need to know the format of the bytearray to convert it into local type.
    QTextCodec *codec = QTextCodec::codecForName("UTF-16");
    QString resString;
    if (codec)
        resString = codec->toUnicode(resBA);

    QCOMPARE(resString, QString("testString"));
}

// TEST CASE: test reading a bytearray, which can be deserialized into QVariant.
void ModuletestConfigurability::cenrepInteroparabilityDecodableBytearray_data()
{
    QTest::addColumn<QString>("path");

    QTest::newRow("numeric cenrep path") << QString("/cr/0xE056F50B/5");
    QTest::newRow("qrml cenrep path") << QString("/publishsubscribe_mt/cenrep/string");
}

void ModuletestConfigurability::cenrepInteroparabilityDecodableBytearray()
{
    QFETCH(QString, path);

    // crete a bytearray that can be deserialized to QVariant
    QVariant data(12345);
    QByteArray byteArray;
    QDataStream writeStream(&byteArray, QIODevice::WriteOnly);
    writeStream << data;

    // convert bytearray into 8-bit descriptor
    HBufC8 *dataBuf = HBufC8::NewL(byteArray.size());
    TPtr8 ptr = dataBuf->Des();
    ptr.Copy((unsigned char*)byteArray.data(), byteArray.size());

    // write descriptor to cenrep
    TUid uid;
    uid.iUid = ownUid;
    CRepository *repo = CRepository::NewL(uid);
    int err = repo->Set(5, *dataBuf);

    delete dataBuf;
    
    // now read the value through qt api. first without any parameter
    QtMobility::QValueSpaceSubscriber subscriber(path, this);
    QVariant res = subscriber.value();

    // Following condition mostly verifies that the test is functioning correctly.
    // The read value should be deserialized into int.
    QCOMPARE(res.type(), QVariant::Int);
    
    // next read the value as raw bytearray
    QtMobility::QValueSpaceSubscriber subscriber2(path + QString("?raw"), this);
    QVariant res2 = subscriber2.value();

    QCOMPARE(res2.type(), QVariant::ByteArray);
    QCOMPARE(res2.toByteArray(), byteArray);
}

// TEST CASE: test support for native float type (cenrep)
void ModuletestConfigurability::floatType()
{
    // write float to cenrep
    TUid uid;
    uid.iUid = ownUid;
    CRepository *repo = CRepository::NewL(uid);
    int err = repo->Set(2, 3.14);

    // read it through QValueSpaceSubscriber
    QtMobility::QValueSpaceSubscriber subscriber("/cr/0xE056F50B/2", this);
    QVariant res = subscriber.value();

    QCOMPARE(res.type(), QVariant::Double);
    QCOMPARE(res.toDouble(), 3.14);
    
    // clear the cenrep value
    repo->Set(2, TReal(0));

    // next try it the other way
    QtMobility::QValueSpacePublisher publisher("/cr/0xE056F50B/2", this);
    publisher.setValue(QString(), QVariant(-3.14));

    TReal result;
    err = repo->Get(2, result);
    QVERIFY(!err);
    QCOMPARE(result, -3.14);
}

// TEST CASE: test float type with RProperty (which doesn't support it natively)
void ModuletestConfigurability::floatTypeProperty()
{
    const double testDouble = 5.4321;
    // write float to RProperty through QValueSpacePublisher
    QtMobility::QValueSpacePublisher publisher("/ps/0xE056F50B/50", this);
    publisher.setValue(QString(), QVariant(testDouble));

    // verify that the property is saved as a bytearray (serialized QVariant)
    TUid uid;
    uid.iUid = ownUid;
    TBuf8<RProperty::KMaxPropertySize> result;
    int err = RProperty::Get(uid, 50, result);
    QVERIFY(!err);
    
    QByteArray nativeResult((const char *)result.Ptr(), result.Size());
    QDataStream readStream(nativeResult);
    QVariant serializedValue;
    readStream >> serializedValue;
    QVERIFY(serializedValue.isValid());
    QCOMPARE(serializedValue.type(), QVariant::Double);
    QCOMPARE(serializedValue.toDouble(), testDouble);
    
    // read the value through QValueSpaceSubscriber
    QtMobility::QValueSpaceSubscriber subscriber("/ps/0xE056F50B/50", this);
    QVariant res = subscriber.value();
    QCOMPARE(res.type(), QVariant::Double);
    QCOMPARE(res.toDouble(), testDouble);
}

// TEST CASE: test reading Symbian UTF-16 string using "?string" parameter
void ModuletestConfigurability::stringParameter1_data()
{
    QTest::addColumn<QString>("path");

    QTest::newRow("numeric cenrep path") << QString("/cr/0xE056F50B/5?string");
    QTest::newRow("qrml cenrep path") << QString("/publishsubscribe_mt/cenrep/string?string");
}

void ModuletestConfigurability::stringParameter1()
{
    QFETCH(QString, path);
    
    TUid uid;
    uid.iUid = ownUid;
    CRepository *repo = CRepository::NewL(uid);
    
    int err = repo->Set(5, _L("testString"));
    QVERIFY(!err);

    QtMobility::QValueSpaceSubscriber subscriber(path, this);

    QVariant res = subscriber.value();
    QCOMPARE(res.type(), QVariant::String);
    QCOMPARE(res.toString(), QString("testString"));
}

// TEST CASE: test writing Symbian UTF-16 string using "?string" parameter
void ModuletestConfigurability::stringParameter2_data()
{
    QTest::addColumn<QString>("path");
    QTest::addColumn<QString>("key");

    QTest::newRow("numeric cenrep path") << QString("/cr/0xE056F50B") << QString("5?string");
    QTest::newRow("qrml cenrep path") << QString("/publishsubscribe_mt/cenrep") << QString("string?string");
}

void ModuletestConfigurability::stringParameter2()
{
    QFETCH(QString, path);
    QFETCH(QString, key);

    TUid uid;
    uid.iUid = ownUid;
    CRepository *repo = CRepository::NewL(uid);
    
    const QString testString("teststring");
    
    QtMobility::QValueSpacePublisher publisher(path, this);
    publisher.setValue(key, QVariant(testString));

    HBufC *res = HBufC::New(NCentralRepositoryConstants::KMaxUnicodeStringLength);
    TPtr ptr = res->Des();
    
    int err = repo->Get(5, ptr);
    QVERIFY(!err);
    
    QString result = QString::fromUtf16(ptr.Ptr(), ptr.Length());
    QCOMPARE(result, testString);
    delete res;
}

// TEST CASE: test Symbian UTF-16 strings stored in RProperty. write
//            string through native API and read it through QValueSpaceSubscriber
void ModuletestConfigurability::stringRProperty1_data()
{
    QTest::addColumn<QString>("path");

    QTest::newRow("numeric pubsub path") << QString("/ps/0xE056F50B/3?string");
    QTest::newRow("qrml pubsub path") << QString("/publishsubscribe_mt/pubsub/string?string");
}

void ModuletestConfigurability::stringRProperty1()
{
    QFETCH(QString, path);

    // write string into RProperty through native API
    TUid uid;
    uid.iUid = ownUid;
    int err = RProperty::Define(3, RProperty::EText,
            TSecurityPolicy(TSecurityPolicy::EAlwaysPass),
            TSecurityPolicy(TSecurityPolicy::EAlwaysPass));
    QVERIFY(!err);
    err = RProperty::Set(uid, 3, _L("testString"));
    QVERIFY(!err);

    // read it through QValueSpaceSubscriber
    QtMobility::QValueSpaceSubscriber subscriber(path, this);
    QVariant res = subscriber.value();
    QCOMPARE(res.type(), QVariant::String);
    QCOMPARE(res.toString(), QString("testString"));
}

// TEST CASE: test Symbian UTF-16 strings stored in RProperty. write
//            string through QValueSpacePublisher and read it through native API
void ModuletestConfigurability::stringRProperty2_data()
{
    QTest::addColumn<QString>("path");
    QTest::addColumn<QString>("key");

    QTest::newRow("numeric pubsub path") << QString("/ps/0xE056F50B") << QString("3?string");
    QTest::newRow("qrml pubsub path") << QString("/publishsubscribe_mt/pubsub") << QString("string?string");
}

void ModuletestConfigurability::stringRProperty2()
{
    QFETCH(QString, path);
    QFETCH(QString, key);

    TUid uid;
    uid.iUid = ownUid;
    const QString testString("teststring2");
    
    QtMobility::QValueSpacePublisher publisher(path, this);
    publisher.setValue(key, QVariant(testString));

    HBufC *res = HBufC::New(NCentralRepositoryConstants::KMaxUnicodeStringLength);
    TPtr ptr = res->Des();
    
    int err = RProperty::Get(uid, 3, ptr);
    QVERIFY(!err);
    
    QString result = QString::fromUtf16(ptr.Ptr(), ptr.Length());
    QCOMPARE(result, testString);
    delete res;
}

void ModuletestConfigurability::featManagerSimpleSubscriber_data()
{
    QTest::addColumn<QString>("feature");
    QTest::addColumn<int>("expectedResult");

    QTest::newRow("valid 1.") << "/fm/0x6b8" << static_cast<int>(QVariant::Bool);
    QTest::newRow("valid 2.") << "/fm/0x6B8" << static_cast<int>(QVariant::Bool);
    QTest::newRow("valid 3.") << "fm/0x6b8" << static_cast<int>(QVariant::Bool);
    QTest::newRow("valid 4.") << "fm/0x6B8" << static_cast<int>(QVariant::Bool);
    QTest::newRow("valid 5.") << "/fm/1720" << static_cast<int>(QVariant::Bool);
    QTest::newRow("valid 6.") << "/fm//0x6b8" << static_cast<int>(QVariant::Bool);
    
    // invalid
    QTest::newRow("invalid 1.") << "/fm/0/1" << static_cast<int>(QVariant::Invalid);
    QTest::newRow("invalid 2.") << "/fm/0/2/5" << static_cast<int>(QVariant::Invalid);
    QTest::newRow("invalid 3.") << "/fm/0/1" << static_cast<int>(QVariant::Invalid);
    QTest::newRow("invalid 4.") << "/fm/a" << static_cast<int>(QVariant::Invalid);
    QTest::newRow("invalid 5.") << "/fm/-1" << static_cast<int>(QVariant::Invalid);
    QTest::newRow("invalid 6.") << "/fm/zzz" << static_cast<int>(QVariant::Invalid);
}

// TEST CASE: feature manager support
void ModuletestConfigurability::featManagerSimpleSubscriber()
{
    QFETCH(QString, feature);

    QtMobility::QValueSpaceSubscriber subscriber1("");

    subscriber1.setPath(feature);
    QVariant result1 = subscriber1.value();

    QTEST(static_cast<int>(result1.type()), "expectedResult");

    QtMobility::QValueSpaceSubscriber subscriber2(feature);
    QVariant result2 = subscriber2.value();

    QTEST(static_cast<int>(result2.type()), "expectedResult");

}

#ifdef Q_OS_SYMBIAN

void ModuletestConfigurability::featManagerAdvSubscriber_data()
{
    QTest::addColumn<QString>("feature");
    QTest::addColumn<int>("featureNum");

    QList<int> featList;
    QList<QString> formatStrings;

    // list of tested features
    featList
    << 0x6b8 // KFeatureId3DRingingTones
    << 0xfc // KFeatureIdFfAbcAgressiveUi
    << 0x8c // KFeatureIdFfAdaptiveWlanScanningSupport
    << 0xf1 // KFeatureIdFfAdditionalFonts
    << 0xac // KFeatureIdFfAiWlansnifferWidget
    << 0x13b // KFeatureIdFfAlternativeCallWaitingTone
    << 0x152 // KFeatureIdArabicHandwritingRecognitionInput
    << 0x6a7 // KFeatureIdAsynchFileSaveQueue
    << 0x3f // KFeatureIdFfAudt
    << 0x14a // KFeatureIdFfAutomotivestack
    << 0x6bb // KFeatureIdAutoRedialForVideoCall
    << 0xd3 // KFeatureIdFfAvkonEmotionIconEnabled
    << 0x16c // KFeatureIdFfBigclockScreensaver
    << 0xbd // KFeatureIdFfBirthdayImprovement
    << 0x10f // KFeatureIdFfBluetoothDataProfilesDisabled
    << 0x10e // KFeatureIdFfBluetoothDisabled
    << 0x162 // KFeatureIdFfBoxHwrInput
    << 0x169 // KFeatureIdBrowserGracefulSocketShutdown
    << 0x6bc // KFeatureIdBrowserProgressiveDownload
    << 0x132 // KFeatureIdFfCalDayView
    << 0x133 // KFeatureIdFfCalScrollablePreview
    << 0x1490 // KFeatureIdFfCalendarWidget
    << 0x128 // KFeatureIdFfCalMeetingRequestUi
    << 0x17e // KFeatureIdFfCameraLongKeyPress
    << 0xb7 // KFeatureIdFfCapacitiveDisplay
    << 0x16d // KFeatureIdFfChineseEngineImprove
    << 0x8a // KFeatureIdChineseFontsHires
    << 0xe9 // KFeatureIdFfChineseSelectionKey
    << 0xbe // KFeatureIdFfClockMultiprocolSupport
    << 0xf7 // KFeatureIdFfCmailIntegration
    << 0x43 // KFeatureIdCommsDatCreationFromXml
    << 0x17d // KFeatureIdFfConferenceBridgePinDialing
    << 0xb3 // KFeatureIdFfConnectionOverride
    << 0xec // KFeatureIdFfContactsCompanyNames
    << 0x10c // KFeatureIdFfContactsMerge
    << 0x10b // KFeatureIdffContactsMycard
    << 0x183 // KFeatureIdFfContactsNewContact
    << 0xb6 // KFeatureIdFfContactsNickname
    << 0x176 // KFeatureIdFfContactsNonEditableMycardNumber
    << 0xeb // KFeatureIdFfContactsPredictiveSearch
    << 0xea // KFeatureIdFfContactsRemoteLookup
    << 0x111 // KFeatureIdFfContactsSocial
    << 0x186 // KFeatureIdFfContactsStatusesInNamesList
    << 0x165 // KFeatureIdFfContactsVcardSyncGroup
    << 0xfe // KFeatureIdFfContextProfiles
    << 0xfa // KFeatureIdFfDeviceEncryptionFeature
    << 0x134 // KFeatureIdFfDisableOmaSuplV2
    << 0x1461 // KFeatureIdFfDisplayNetworkNameAfterCsRegistration
    << 0x115 // KFeatureIdFfDmConnmoAdapter
    << 0x16f // KFeatureIdFfDmIapOverWrite
    << 0x13c9 // KFeatureIdDmInstallSecurity
    << 0x19d // FfDolbyHeadphone
    << 0xc2 // KFeatureIdFfDualLanguageSupport
    << 0xe6 // KFeatureIdDualModem
    << 0x1a4 // KFeatureIdFfEePnnSpn
    << 0x158 // KFeatureIdFfEmailConversationView
    << 0x87 // KFeatureIdFfEmailFramework
    << 0x145 // KFeatureIdFfEmailFullscreenviewer
    << 0x89 // KFeatureIdFfEmailIpsServices
    << 0x7e // KFeatureIdEmailMceIntegration
    << 0x144 // KFeatureIdFfEmailPredictivesearch
    << 0xd7 // KFeatureIdFfEmailProtocolPlugins
    << 0x88 // KFeatureIdFfEmailUi
    << 0x143 // KFeatureIdFfEmailUiSplitscreen
    << 0x157 // KFeatureIdFfEmergencyCbs
    << 0x135 // KFeatureIdEnableIsiCommunicationInUsbChargingMode
    << 0x127 // KFeatureIdFfEnhancedCalendarEventUi
    << 0x1499 // KFeatureIdFfEnhUplinkNoiceCancel
    << 0x1469 // KFeatureIdFfEntryPointForVideoShare
    << 0xd5 // KFeatureIdFfFarsiCalendar
    << 0x161 // KFeatureIdFfFingerHwrInput
    << 0x6a9 // KFeatureIdFmtx
    << 0x170 // KFeatureIdFfFmtxAutoTune
    << 0xdc // KFeatureIdFfFmtxRdsText
    << 0x181 // FfFontTable
    << 0x146 // KFeatureIdFfFotaCheckLimitation
    << 0x16b // KFeatureIdFfFotaDecryptMemBeforeUpdate
    << 0x42 // KFeatureIdFullscrHwrInput
    << 0x175 // KFeatureIdFfFuzzyPinyinSettingAndContactsImportingForChineseInput
    << 0x101 // KFeatureIdFfGenericAccessNetwork
    << 0xbc // KFeatureIdFfGenericFontsAssets
    << 0x17f // KFeatureIdFfGstreamerIadUpdatable
    << 0x149a // KFeatureIdFfGstreamerPlaybackEnablerPlugins
    << 0xa9 // KFeatureIdFfHdmi
    << 0xa4 // KFeatureIdFfHelpTutorialMerge
    << 0xd4 // KFeatureIdFfHijriCalendar
    << 0x6ac // KFeatureIdHitchcockAppShell
    << 0x9a // KFeatureIdFfHomescreenSwitchableRoot
    << 0x13dd // KFeatureIdProductIncludesHomeScreenEasyDialing
    << 0x13ea // KFeatureIdFfHomeScreenVanityDialing
    << 0x1e // KFeatureIdIAUpdate
    << 0xb9 // KFeatureIdFfIaupdatePhase2
    << 0x180 // KFeatureIdFfIaupdateStartAfterFota
    << 0x15d // KFeatureIdFfIce
    << 0xff // KFeatureIdFfImageEditor
    << 0x146e // KFeatureIdFfImsDeregistrationInOffline
    << 0x146f // KFeatureIdFfImsDeregistrationInRfs
    << 0x1459 // KFeatureIdFfImsDeregistrationInVpn
    << 0xc0 // KFeatureIdFfInputsPerOrientation
    << 0xd8 // KFeatureIdFfIntelligentTextInput
    << 0x198 // FfIsimGbaAuthentication
    ;

    // format strings for features
    formatStrings
        << "/fm/%#x"
        << "/fm/%#X"
        << "/fm/%d"
        << "fm/%#x"
        << "fm/%#X"
        << "fm/%d"
        << "fm//%#x"
        << "fm//%#X"
        << "fm//%d"
        << "fm//%#x/"
        << "fm//%#X/"
        << "fm//%d/"
        ;

    foreach (int featureNum, featList) {
        foreach (QString formatString, formatStrings) {
            QString featureString = QString().sprintf(formatString.toLatin1(), featureNum);

            QTest::newRow(QString("%1:%2").arg(featureNum).arg(formatString).toLatin1().data())
                        << featureString
                        << featureNum;
        }
    }

}

// TEST CASE: feature manager support
void ModuletestConfigurability::featManagerAdvSubscriber()
{
    QFETCH(QString, feature);
    QFETCH(int, featureNum);

    CFeatureDiscovery *featDiscovery = 0;
    try {
        QT_TRAP_THROWING(featDiscovery = CFeatureDiscovery::NewL());
    } catch (std::exception &e) {
        delete featDiscovery;
        QFAIL(e.what());
    }

    QtMobility::QValueSpaceSubscriber subscriber("");

    TUid uid = TUid::Uid(featureNum);

    TBool isSupportedOrig = featDiscovery->IsSupported(uid);

    subscriber.setPath(feature);
    QVERIFY2(subscriber.value() == isSupportedOrig, QString("Feature manager: wrong result for feature: %1").arg(feature).toLatin1().data());

    delete featDiscovery;
}

#endif

void ModuletestConfigurability::featManagerMapperCase_data()
{
    QTest::addColumn<QString>("feature");
    QTest::addColumn<QString>("featureReference");
    QTest::addColumn<bool>("expectedResult");

    QTest::newRow("c") << "/featuremanager/c" << "0x6b8" << true;
    QTest::newRow("d") << "/featuremanager/d" << "0xfc" << true;
    QTest::newRow("e") << "/featuremanager/e" << "0x8c" << true;
    QTest::newRow("i") << "/featuremanager/invalid" << "invalid" << false;

}

// TEST CASE: feature manager support
void ModuletestConfigurability::featManagerMapperCase()
{
    QFETCH(QString, feature);
    QFETCH(QString, featureReference);

    QtMobility::QValueSpaceSubscriber subscriber(feature);
    QtMobility::QValueSpaceSubscriber referenceSubscriber("/fm/" + featureReference);

    QTEST(subscriber.value().type() == QVariant::Bool, "expectedResult");

    QCOMPARE(subscriber.value(), referenceSubscriber.value());
}




// Dummy slot for connecting QValueSpaceSubscriber::contentsChanged signal.
// Subscriber doesn't start listening to change events before it is connected
// to some slot.
void ModuletestConfigurability::dummy()
{
}

QTEST_MAIN(ModuletestConfigurability)
