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

#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QtCore/QCoreApplication>

#include "qnfctestcommon.h"
#include "qnfctestutil.h"

#include <qnearfieldmanager.h>
#include <qnearfieldtarget.h>

QTM_USE_NAMESPACE

Q_DECLARE_METATYPE(QNearFieldTarget*)
Q_DECLARE_METATYPE(QNearFieldTarget::Type)

class tst_QNearFieldManager : public QObject
{
    Q_OBJECT

public:
    tst_QNearFieldManager();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void targetDetected();
    void targetDetected_data();
};

tst_QNearFieldManager::tst_QNearFieldManager()
{
}

void tst_QNearFieldManager::initTestCase()
{
}

void tst_QNearFieldManager::cleanupTestCase()
{
}

/**
 * Description: Unit test for NFC target detected and lost
 *
 * TestScenario: 1. Touch and remove llcp device/Type1/Type2/Type3/Type4 tag one by one
 *
 * TestExpectedResults: 1. llcp device/Type1/Type2/Type3/Type4 tag detected/lost signal can be received
 */

void tst_QNearFieldManager::targetDetected()
{
    QFETCH(QNearFieldTarget::Type, type);
    QFETCH(QString, hint);

    QNearFieldManager nfcManager;

    QSignalSpy targetDetectedSpy(&nfcManager, SIGNAL(targetDetected(QNearFieldTarget*)));
    QSignalSpy targetLostSpy(&nfcManager, SIGNAL(targetLost(QNearFieldTarget*)));

    nfcManager.startTargetDetection(type);
    QNfcTestUtil::ShowMessage(hint);
    QTRY_VERIFY(!targetDetectedSpy.isEmpty());

    QNearFieldTarget *target = targetDetectedSpy.first().at(0).value<QNearFieldTarget *>();

    QSignalSpy disconnectedSpy(target, SIGNAL(disconnected()));
    QVERIFY(target);

    QVERIFY(!target->uid().isEmpty());

    if (type != QNearFieldTarget::AnyTarget)
    {
        QCOMPARE(target->type(), type);
    }

    QNfcTestUtil::ShowMessage("please remove the target");

    QTRY_VERIFY(!targetLostSpy.isEmpty());
    
    QNearFieldTarget *lostTarget = targetLostSpy.first().at(0).value<QNearFieldTarget *>();

    QCOMPARE(target, lostTarget);

    QVERIFY(!disconnectedSpy.isEmpty());

    nfcManager.stopTargetDetection(); 
}

void tst_QNearFieldManager::targetDetected_data()
{
    QTest::addColumn<QNearFieldTarget::Type>("type");
    QTest::addColumn<QString>("hint");
    QTest::newRow("llcp device") << QNearFieldTarget::AnyTarget << "Please touch llcp device";
    QTest::newRow("NfcTagType1") << QNearFieldTarget::NfcTagType1 << "Please touch tag type1";
    QTest::newRow("NfcTagType2") << QNearFieldTarget::NfcTagType1 << "Please touch tag type2";
    QTest::newRow("NfcTagType3") << QNearFieldTarget::NfcTagType1 << "Please touch tag type3";
    QTest::newRow("NfcTagType4") << QNearFieldTarget::NfcTagType1 << "Please touch tag type4";
}

QTEST_MAIN(tst_QNearFieldManager);

#include "tst_qnearfieldmanager.moc"
