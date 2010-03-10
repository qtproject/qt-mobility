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

#include <QtTest/QtTest>
#include <QDebug>
#include <QTimer>

#include <qmediaobject.h>
#include <qmediacontrol.h>
#include <qmediaservice.h>
#include <qradiotunercontrol.h>
#include <qradiotuner.h>

#include <QMessageBox>

QTM_USE_NAMESPACE

class tst_QRadioTuner: public QObject
{
    Q_OBJECT

public slots:
    void initTestCase();
    void cleanupTestCase();

private slots:
    void testBand();
    void testFrequency();
    void testMute();
    void testSearch();
    void testVolume();
    void testSignal();
    void testStereo();

private:
    QRadioTuner    *radio;
};

void tst_QRadioTuner::initTestCase()
{
    qRegisterMetaType<QRadioTuner::State>("QRadioTuner::State");
    qRegisterMetaType<QRadioTuner::Band>("QRadioTuner::Band");

    radio = new QRadioTuner(0);
    QVERIFY(radio->service() != 0);

    QSignalSpy stateSpy(radio, SIGNAL(stateChanged(QRadioTuner::State)));

    QCOMPARE(radio->state(), QRadioTuner::StoppedState); 
    radio->start();
    QCOMPARE(radio->state(), QRadioTuner::ActiveState);

    QCOMPARE(stateSpy.count(), 1);
    QCOMPARE(stateSpy.first()[0].value<QRadioTuner::State>(), QRadioTuner::ActiveState);
}

void tst_QRadioTuner::cleanupTestCase()
{
    QVERIFY(radio->error() == QRadioTuner::NoError);
    QVERIFY(radio->errorString().isEmpty());

    QSignalSpy stateSpy(radio, SIGNAL(stateChanged(QRadioTuner::State)));

    radio->stop();
    QCOMPARE(radio->state(), QRadioTuner::StoppedState);
    QCOMPARE(stateSpy.count(), 1);

#ifdef QTM_NAMESPACE
    QEXPECT_FAIL("", "QSignalSpy doesn't grab the correct value from signal because of QtMobility namespace", Continue);
#endif
    QCOMPARE(stateSpy.first()[0].value<QRadioTuner::State>(), QRadioTuner::StoppedState);

    delete radio;
}

void tst_QRadioTuner::testBand()
{
    QVERIFY(radio->isBandSupported(QRadioTuner::FM));
    QVERIFY(!radio->isBandSupported(QRadioTuner::SW));

    if(radio->isBandSupported(QRadioTuner::AM)) {
        QSignalSpy readSignal(radio, SIGNAL(bandChanged(QRadioTuner::Band)));
        radio->setBand(QRadioTuner::AM);
        QTestEventLoop::instance().enterLoop(1);
        QVERIFY(radio->band() == QRadioTuner::AM);
        QVERIFY(readSignal.count() == 1);
    }
}

void tst_QRadioTuner::testFrequency()
{
    QSignalSpy readSignal(radio, SIGNAL(frequencyChanged(int)));
    radio->setFrequency(104500000);
    QTestEventLoop::instance().enterLoop(1);
    QVERIFY(radio->frequency() == 104500000);
    QVERIFY(readSignal.count() == 1);

    // frequencyStep for FM radio is 100kHz (100000Hz)
    QVERIFY(radio->frequencyStep(QRadioTuner::FM) == 100000);
    QPair<int,int> test = radio->frequencyRange(QRadioTuner::FM);
    // frequency range for FM radio is 87,5MHz - 108MHz
    QVERIFY(test.first == 87500000);
    QVERIFY(test.second == 108000000);
}

void tst_QRadioTuner::testMute()
{
    QSignalSpy readSignal(radio, SIGNAL(mutedChanged(bool)));
    radio->setMuted(true);
    QTestEventLoop::instance().enterLoop(1);
    QVERIFY(radio->isMuted());
    QVERIFY(readSignal.count() == 1);
}

void tst_QRadioTuner::testSearch()
{
    QSignalSpy readSignal(radio, SIGNAL(searchingChanged(bool)));
    QVERIFY(!radio->isSearching());

    radio->searchForward();
    QVERIFY(radio->isSearching());
    QVERIFY(readSignal.count() == 1);

    radio->cancelSearch();
    QTestEventLoop::instance().enterLoop(1);
    QVERIFY(!radio->isSearching());
    QVERIFY(readSignal.count() == 2);

    radio->searchBackward();
    QVERIFY(radio->isSearching());
    QVERIFY(readSignal.count() == 3);

    radio->cancelSearch();
    QVERIFY(!radio->isSearching());
}

void tst_QRadioTuner::testVolume()
{
    QVERIFY(radio->volume() == 100);
    QSignalSpy readSignal(radio, SIGNAL(volumeChanged(int)));
    radio->setVolume(50);
    QTestEventLoop::instance().enterLoop(1);
    QVERIFY(radio->volume() == 50);
    QVERIFY(readSignal.count() == 1);
}

void tst_QRadioTuner::testSignal()
{
    QVERIFY(radio->signalStrength() == 0);
    // There is no set of this only a get, do nothing else.
}

void tst_QRadioTuner::testStereo()
{
    QVERIFY(radio->isStereo());
    radio->setStereoMode(QRadioTuner::ForceMono);
    QVERIFY(radio->stereoMode() == QRadioTuner::ForceMono);
}

QTEST_MAIN(tst_QRadioTuner)

#include "tst_qradiotuner_s60.moc"
