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

#include <QtTest/QtTest>

#include "../qconnectivitytestcommon.h"
#include "qnearfieldmanager_emulator_p.h"

#include <qnearfieldmanager.h>
#include <qndefnfctextrecord.h>
#include <qndefnfcurirecord.h>
#include <qndefmessage.h>
#include <qndefrecord.h>

Q_DECLARE_METATYPE(QNearFieldTarget*)
Q_DECLARE_METATYPE(QNearFieldTarget::Type)
Q_DECLARE_METATYPE(QNdefFilter)

QTM_USE_NAMESPACE

class tst_QNearFieldManager : public QObject
{
    Q_OBJECT

public:
    tst_QNearFieldManager();

private slots:
    void targetDetected();

    void unregisterTargetDetectedHandler();

    void registerTargetDetectedHandler1_data();
    void registerTargetDetectedHandler1();

    void registerTargetDetectedHandler_filter_data();
    void registerTargetDetectedHandler_filter();
};

tst_QNearFieldManager::tst_QNearFieldManager()
{
    QDir::setCurrent(QLatin1String(SRCDIR));

    qRegisterMetaType<QNdefMessage>("QNdefMessage");
    qRegisterMetaType<QNearFieldTarget *>("QNearFieldTarget*");
}

void tst_QNearFieldManager::targetDetected()
{
    QNearFieldManagerPrivateImpl *emulatorBackend = new QNearFieldManagerPrivateImpl;
    QNearFieldManager manager(emulatorBackend, 0);

    QSignalSpy targetDetectedSpy(&manager, SIGNAL(targetDetected(QNearFieldTarget*)));
    QSignalSpy targetLostSpy(&manager, SIGNAL(targetLost(QNearFieldTarget*)));

    QTRY_VERIFY(!targetDetectedSpy.isEmpty());

    QNearFieldTarget *target = targetDetectedSpy.first().at(0).value<QNearFieldTarget *>();

    QSignalSpy disconnectedSpy(target, SIGNAL(disconnected()));

    QVERIFY(target);

    QVERIFY(!target->uid().isEmpty());

    QCOMPARE(target->hasNdefMessage(), !target->ndefMessages().isEmpty());

    QTRY_VERIFY(!targetLostSpy.isEmpty());

    QNearFieldTarget *lostTarget = targetLostSpy.first().at(0).value<QNearFieldTarget *>();

    QCOMPARE(target, lostTarget);

    QVERIFY(!disconnectedSpy.isEmpty());
}

void tst_QNearFieldManager::unregisterTargetDetectedHandler()
{
    QNearFieldManagerPrivateImpl *emulatorBackend = new QNearFieldManagerPrivateImpl;
    QNearFieldManager manager(emulatorBackend, 0);

    QVERIFY(!manager.unregisterTargetDetectedHandler(-1));
    QVERIFY(!manager.unregisterTargetDetectedHandler(0));
}

void tst_QNearFieldManager::registerTargetDetectedHandler1_data()
{
    QTest::addColumn<QNearFieldTarget::Type>("type");

    QTest::newRow("AnyTarget") << QNearFieldTarget::AnyTarget;
    QTest::newRow("NfcTagType1") << QNearFieldTarget::NfcTagType1;
}

class MessageListener : public QObject
{
    Q_OBJECT

signals:
    void matchedNdefMessage(const QNdefMessage &message, QNearFieldTarget *target);
};

void tst_QNearFieldManager::registerTargetDetectedHandler1()
{
    QFETCH(QNearFieldTarget::Type, type);

    QNearFieldManagerPrivateImpl *emulatorBackend = new QNearFieldManagerPrivateImpl;
    QNearFieldManager manager(emulatorBackend, 0);

    MessageListener listener;
    QSignalSpy messageSpy(&listener, SIGNAL(matchedNdefMessage(QNdefMessage,QNearFieldTarget*)));

    int id = manager.registerTargetDetectedHandler(type, &listener,
                                                   SIGNAL(matchedNdefMessage(QNdefMessage,QNearFieldTarget*)));

    QVERIFY(id != -1);

    QTRY_VERIFY(!messageSpy.isEmpty());

    const QNdefMessage message = messageSpy.first().at(0).value<QNdefMessage>();
    QNearFieldTarget *target = messageSpy.first().at(1).value<QNearFieldTarget *>();

    QVERIFY(target);

    if (type != QNearFieldTarget::AnyTarget)
        QCOMPARE(target->type(), type);

    QVERIFY(manager.unregisterTargetDetectedHandler(id));
}

void tst_QNearFieldManager::registerTargetDetectedHandler_filter_data()
{
    QTest::addColumn<QNearFieldTarget::Type>("type");
    QTest::addColumn<QNdefFilter>("filter");

    QNdefFilter filter;

    QTest::newRow("NfcTagType1") << QNearFieldTarget::NfcTagType1 << filter;

    filter.clear();
    filter.setOrderMatch(true);
    filter.appendRecord<QNdefNfcTextRecord>(2, 10);
    filter.appendRecord<QNdefNfcUriRecord>(1, 1);
    QTest::newRow("Multiple Text + URI") << QNearFieldTarget::NfcTagType1 << filter;

    filter.clear();
    filter.setOrderMatch(true);
    filter.appendRecord<QNdefNfcTextRecord>(1, 1);
    filter.appendRecord<QNdefNfcUriRecord>(1, 1);
    QTest::newRow("Text + URI") << QNearFieldTarget::AnyTarget << filter;
}

void tst_QNearFieldManager::registerTargetDetectedHandler_filter()
{
    QFETCH(QNearFieldTarget::Type, type);
    QFETCH(QNdefFilter, filter);

    QNearFieldManagerPrivateImpl *emulatorBackend = new QNearFieldManagerPrivateImpl;
    QNearFieldManager manager(emulatorBackend, 0);

    MessageListener listener;
    QSignalSpy messageSpy(&listener, SIGNAL(matchedNdefMessage(QNdefMessage,QNearFieldTarget*)));

    int id = manager.registerTargetDetectedHandler(type, filter,
                                                   &listener,
                                                   SIGNAL(matchedNdefMessage(QNdefMessage,QNearFieldTarget*)));

    QVERIFY(id != -1);

    QTRY_VERIFY(!messageSpy.isEmpty());

    const QNdefMessage message = messageSpy.first().at(0).value<QNdefMessage>();

    qDebug() << "Verify message";

    QNearFieldTarget *target = messageSpy.first().at(1).value<QNearFieldTarget *>();

    QVERIFY(target);

    if (type != QNearFieldTarget::AnyTarget)
        QCOMPARE(target->type(), type);
}

QTEST_MAIN(tst_QNearFieldManager);

#include "tst_qnearfieldmanager.moc"
