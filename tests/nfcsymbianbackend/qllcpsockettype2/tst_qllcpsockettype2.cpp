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

#include <qllcpsocket.h>

#include "qnfctestcommon.h"
#include "qnfctestutil.h"

QTM_USE_NAMESPACE

class tst_qllcpsockettype2 : public QObject
{
    Q_OBJECT

public:
    tst_qllcpsockettype2();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void echo();
    void echo_data();
};

tst_qllcpsockettype2::tst_qllcpsockettype2()
{
}

void tst_qllcpsockettype2::initTestCase()
{
}

void tst_qllcpsockettype2::cleanupTestCase()
{
}

void tst_qllcpsockettype2::echo()
{
    QFETCH(QString, uri);
    QFETCH(QString, hint);
    QNearFieldManager nfcManager;
    QSignalSpy targetDetectedSpy(&nfcManager, SIGNAL(targetDetected(QNearFieldTarget*)));
    nfcManager.startTargetDetection(QNearFieldTarget::AnyTarget);

    QNfcTestUtil::ShowMessage(hint);
    QTRY_VERIFY(!targetDetectedSpy.isEmpty());

    QNearFieldTarget *target = targetDetectedSpy.first().at(0).value<QNearFieldTarget *>();
    QVERIFY(target!=NULL);
    QVERIFY(target->accessMethods() & QNearFieldTarget::LlcpAccess);

    QLlcpSocket socket(this);

}

void tst_qllcpsockettype2::echo_data()
{
    QTest::addColumn<QString>("URI");
    QTest::addColumn<QString>("hint");
    QTest::newRow("uri") << "uri" << "Please touch a NFC device with llcp client enabled: uri = ";

}

QTEST_MAIN(tst_qllcpsockettype2);

#include "tst_qllcpsockettype2.moc"
