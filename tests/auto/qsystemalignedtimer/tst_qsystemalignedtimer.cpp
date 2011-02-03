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

//TESTED_COMPONENT=src/systeminfo

#include <QtTest/QtTest>
#include "qsystemalignedtimer.h"

QTM_USE_NAMESPACE

Q_DECLARE_METATYPE(QSystemAlignedTimer::AlignedTimerError);

class tst_QSystemAlignedTimer : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();

    void tst_wokeUp();

    void tst_minimumInterval();
    void tst_maximumInterval();

    void tst_setSingleShot();
    void tst_isSingleShot() ;

    void tst_singleShot();

    void tst_lastError();

    void tst_start();
    void tst_stop();

public Q_SLOTS:
    void timeout();

private:
    QSystemAlignedTimer alignedtime2r;

};

void tst_QSystemAlignedTimer::initTestCase()
{
   qRegisterMetaType<QSystemAlignedTimer::AlignedTimerError>("QSystemAlignedTimer::AlignedTimerError");
}

void tst_QSystemAlignedTimer::tst_wokeUp()
{
    QSystemAlignedTimer alignedtimer;
    if(alignedtimer.lastError() == QSystemAlignedTimer::AlignedTimerNotSupported) {
        QSKIP("This test not supported on this platform", SkipAll);
    }
    alignedtimer.setSingleShot(true);
    alignedtimer.start(8,10);

    QVERIFY(alignedtimer.isActive());

    alignedtimer.wokeUp();
    QVERIFY(!alignedtimer.isActive());

}

void tst_QSystemAlignedTimer::tst_minimumInterval()
{
    QSystemAlignedTimer alignedtimer;
    if(alignedtimer.lastError() == QSystemAlignedTimer::AlignedTimerNotSupported) {
        QSKIP("This test not supported on this platform", SkipAll);
    }
    alignedtimer.setMinimumInterval(0);
    QVERIFY(alignedtimer.minimumInterval() == 0);
    alignedtimer.setMinimumInterval(10);
    QVERIFY(alignedtimer.minimumInterval() == 10);
}

void tst_QSystemAlignedTimer::tst_maximumInterval()
{

    QSystemAlignedTimer alignedtimer;
    if(alignedtimer.lastError() == QSystemAlignedTimer::AlignedTimerNotSupported) {
        QSKIP("This test not supported on this platform", SkipAll);
    }

    alignedtimer.setMinimumInterval(0);
    alignedtimer.setMaximumInterval(0);
    QVERIFY(alignedtimer.maximumInterval() == 0);
    QVERIFY(alignedtimer.lastError() == QSystemAlignedTimer::InvalidArgument);

    alignedtimer.setMaximumInterval(10);
    QVERIFY(alignedtimer.maximumInterval() == 10);
}

void tst_QSystemAlignedTimer::tst_setSingleShot()
{
    QSystemAlignedTimer alignedtimer;
    if(alignedtimer.lastError() == QSystemAlignedTimer::AlignedTimerNotSupported) {
        QSKIP("This test not supported on this platform", SkipAll);
    }
    alignedtimer.setSingleShot(true);
    QVERIFY(alignedtimer.isSingleShot());
    alignedtimer.setSingleShot(false);
    QVERIFY(!alignedtimer.isSingleShot());
}

void tst_QSystemAlignedTimer::tst_isSingleShot()
{
    QSystemAlignedTimer alignedtimer;
    if(alignedtimer.lastError() == QSystemAlignedTimer::AlignedTimerNotSupported) {
        QSKIP("This test not supported on this platform", SkipAll);
    }
    alignedtimer.setSingleShot(true);
    QVERIFY(alignedtimer.isSingleShot());
    alignedtimer.setSingleShot(false);
    QVERIFY(!alignedtimer.isSingleShot());
}

void tst_QSystemAlignedTimer::tst_singleShot()
{
    QSystemAlignedTimer alignedtimer;
    if(alignedtimer.lastError() == QSystemAlignedTimer::AlignedTimerNotSupported) {
        QSKIP("This test not supported on this platform", SkipAll);
    }

}

void tst_QSystemAlignedTimer::tst_lastError()
{
    QSystemAlignedTimer alignedtimer;
    if(alignedtimer.lastError() == QSystemAlignedTimer::AlignedTimerNotSupported) {
        QSKIP("This test not supported on this platform", SkipAll);
    }
}

void tst_QSystemAlignedTimer::tst_start()
{
    QSystemAlignedTimer alignedtimer;
    if(alignedtimer.lastError() == QSystemAlignedTimer::AlignedTimerNotSupported) {
        QSKIP("This test not supported on this platform", SkipAll);
    }
    alignedtimer.start(8,10);
    QVERIFY(alignedtimer.isActive());
    alignedtimer.stop();
    alignedtimer.setMinimumInterval(8);
    alignedtimer.setMaximumInterval(10);
    alignedtimer.start();
    QVERIFY(alignedtimer.isActive());

    alignedtime2r.start(8,10);
    QSignalSpy spy(&alignedtime2r,SLOT(timeout()));
    QCOMPARE(spy.count(), 1);
}

void tst_QSystemAlignedTimer::tst_stop()
{
    QSystemAlignedTimer alignedtimer;
    if(alignedtimer.lastError() == QSystemAlignedTimer::AlignedTimerNotSupported) {
        QSKIP("This test not supported on this platform", SkipAll);
    }
    alignedtimer.start(8,10);
    alignedtimer.stop();
    QVERIFY(!alignedtimer.isActive());
}

void tst_QSystemAlignedTimer::timeout()
{
    QVERIFY(!alignedtime2r.isActive());
}


QTEST_MAIN(tst_QSystemAlignedTimer)
#include "tst_qsystemalignedtimer.moc"
