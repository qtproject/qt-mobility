/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include "moduletest_configurability.h"

#include <QtTest/QtTest>
#include <QtDebug>
#include <QVariant>
#include <QValueSpaceSubscriber>
#include <QValueSpacePublisher>

const QString ownCrFullPath("/cr/0xE056F50B/1");
const QString ownCrPath("/cr/0xE056F50B");
const QString ownCrKey("1");
const QString ownPsFullPath("/ps/0xE056F50B/1");

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


// ======== MEMBER FUNCTIONS ========

void ModuletestConfigurability::initTestCase()
{
}

void ModuletestConfigurability::init()
{
    // set own cr key to default value
    QtMobility::QValueSpacePublisher publisher(ownCrPath, this);
    publisher.setValue(ownCrKey, QVariant(0));
    
    // remove own ps key
    QtMobility::QValueSpacePublisher publisher2(ownPsFullPath, this);
    publisher2.resetValue(QString());
}

void ModuletestConfigurability::cleanup()
{
}

void ModuletestConfigurability::cleanupTestCase()
{
}

void ModuletestConfigurability::readAndSetCenrep()
{
    QtMobility::QValueSpaceSubscriber subscriber(ownCrFullPath, this);
    QtMobility::QValueSpacePublisher publisher(ownCrFullPath, this);
    
    QVariant value = subscriber.value();
    QCOMPARE(value.type(), QVariant::Int);
    QCOMPARE(value.toInt(), 0);
    
    // using just signalspy doesn't emit connectNotify signal, so QValueSpaceSubscriber
    // doesn't start listening to change event.
    //   ==> connect the signal to dummy slot
    connect(&subscriber, SIGNAL(contentsChanged()), this, SLOT(dummy()));
    QSignalSpy spy(&subscriber, SIGNAL(contentsChanged()));
    
    publisher.setValue(QString(), QVariant(1234));

    // wait for the change notification (timout: 5 seconds)
    QVERIFY(waitForSignal(&subscriber, SIGNAL(contentsChanged()), 5000));

    QCOMPARE(spy.count(), 1);
    QCOMPARE(subscriber.value().toInt(), 1234);
}

void ModuletestConfigurability::partialCenrepPath()
{
    QtMobility::QValueSpaceSubscriber subscriber(ownCrPath, this);
    QtMobility::QValueSpacePublisher publisher(ownCrPath, this);
    
    QCOMPARE(subscriber.value(ownCrKey).toInt(), 0);
    
    // It is not possible to get change notifications for partial numeric path.
    
    publisher.setValue(ownCrKey, QVariant(1234));

    QCOMPARE(subscriber.value(ownCrKey).toInt(), 1234);    
}

void ModuletestConfigurability::readAndSetPubsub()
{
    QtMobility::QValueSpaceSubscriber subscriber(ownPsFullPath, this);
    QtMobility::QValueSpacePublisher publisher(ownPsFullPath, this);
    
    QVariant value = subscriber.value();
    QCOMPARE(value.type(), QVariant::Invalid);
    
    publisher.setValue(QString(), QVariant(1));

    QCOMPARE(subscriber.value().toInt(), 1);
    
    // we can start listening to change notifications only after the pubsub
    // key has some value (Qt Mobility restriction).
    connect(&subscriber, SIGNAL(contentsChanged()), this, SLOT(dummy()));
    QSignalSpy spy(&subscriber, SIGNAL(contentsChanged()));
    
    publisher.setValue(QString(), QVariant(1234));

    // wait for the change notification (timout: 5 seconds)
    QVERIFY(waitForSignal(&subscriber, SIGNAL(contentsChanged()), 5000));

    QCOMPARE(spy.count(), 1);
    QCOMPARE(subscriber.value().toInt(), 1234);
}

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

// Dummy slot for connecting QValueSpaceSubscriber::contentsChanged signal.
// Subscriber doesn't start listening to change events before it is connected
// to some slot.
void ModuletestConfigurability::dummy()
{
}

QTEST_MAIN(ModuletestConfigurability)
