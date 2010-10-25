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

//TESTED_COMPONENT=src/multimedia

#include <QtTest/QtTest>
#include <QtCore/qlocale.h>
#include <qaudiooutput.h>
#include <qaudiodeviceinfo.h>
#include <qaudio.h>
#include <qsoundeffect_p.h>


class tst_QSoundEffect : public QObject
{
    Q_OBJECT
public:
    tst_QSoundEffect(QObject* parent=0) : QObject(parent) {}

private slots:
    void initTestCase();
    void testSource();
    void testLooping();
    void testVolume();
    void testMuting();

    void testPlaying();
    void testStatus();

private:
    QSoundEffect* sound;
};

void tst_QSoundEffect::initTestCase()
{
    sound = new QSoundEffect(this);

    QVERIFY(sound->source().isEmpty());
    QVERIFY(sound->loopCount() == 1);
    QVERIFY(sound->volume() == 1);
    QVERIFY(sound->isMuted() == false);
}

void tst_QSoundEffect::testSource()
{
    QSignalSpy readSignal(sound, SIGNAL(sourceChanged()));

    QUrl url = QUrl::fromLocalFile(QString(QLatin1String("%1%2")).arg(QLatin1String(SRCDIR)).arg(QLatin1String("test.wav")));
    sound->setSource(url);

    QCOMPARE(sound->source(),url);
    QCOMPARE(readSignal.count(),1);

    QTestEventLoop::instance().enterLoop(1);
    sound->play();

    QTest::qWait(3000);
}

void tst_QSoundEffect::testLooping()
{
    QSignalSpy readSignal(sound, SIGNAL(loopCountChanged()));

    sound->setLoopCount(5);
    QCOMPARE(sound->loopCount(),5);

    sound->play();

    // test.wav is about 200ms, wait until it has finished playing 5 times
    QTest::qWait(3000);

}

void tst_QSoundEffect::testVolume()
{
    QSignalSpy readSignal(sound, SIGNAL(volumeChanged()));

    sound->setVolume(0.5);
    QCOMPARE(sound->volume(),0.5);

    QTest::qWait(20);
    QCOMPARE(readSignal.count(),1);
}

void tst_QSoundEffect::testMuting()
{
    QSignalSpy readSignal(sound, SIGNAL(mutedChanged()));

    sound->setMuted(true);
    QCOMPARE(sound->isMuted(),true);

    QTest::qWait(20);
    QCOMPARE(readSignal.count(),1);
}

void tst_QSoundEffect::testPlaying()
{
    sound->setLoopCount(QSoundEffect::Infinite);
    //valid source
    QUrl url = QUrl::fromLocalFile(QString(QLatin1String("%1%2")).arg(QLatin1String(SRCDIR)).arg(QLatin1String("test.wav")));
    sound->setSource(url);
    QTestEventLoop::instance().enterLoop(1);
    sound->play();
    QTestEventLoop::instance().enterLoop(1);
    QCOMPARE(sound->isPlaying(), true);
    sound->stop();

    //empty source
    url = QUrl();
    sound->setSource(url);
    QTestEventLoop::instance().enterLoop(1);
    sound->play();
    QTestEventLoop::instance().enterLoop(1);
    QCOMPARE(sound->isPlaying(), false);

    //invalid source
    url = QUrl((QLatin1String("invalid source")));
    sound->setSource(url);
    QTestEventLoop::instance().enterLoop(1);
    sound->play();
    QTestEventLoop::instance().enterLoop(1);
    QCOMPARE(sound->isPlaying(), false);

    sound->setLoopCount(1);
}

void tst_QSoundEffect::testStatus()
{
    QUrl url = QUrl();
    sound->setSource(url);
    QSignalSpy readSignal(sound, SIGNAL(statusChanged()));
    QCOMPARE(sound->status(), QSoundEffect::Null);

    int signalCount = 1;
    //valid source
    url = QUrl::fromLocalFile(QString(QLatin1String("%1%2")).arg(QLatin1String(SRCDIR)).arg(QLatin1String("test.wav")));
    sound->setSource(url);
    QCOMPARE(readSignal.count(), signalCount);

    if (sound->status() == QSoundEffect::Loading) {
        QTestEventLoop::instance().enterLoop(1);
        QCOMPARE(readSignal.count(), ++signalCount);
    }
    QCOMPARE(sound->status(), QSoundEffect::Ready);

    //empty source
    url = QUrl();
    sound->setSource(url);
    QTestEventLoop::instance().enterLoop(1);
    QCOMPARE(readSignal.count(), ++signalCount);
    QCOMPARE(sound->status(), QSoundEffect::Null);

    //invalid source
    sound->setLoopCount(QSoundEffect::Infinite);
    url = QUrl(QLatin1String("invalid source"));

    sound->setSource(url);
    QTestEventLoop::instance().enterLoop(1);
    QCOMPARE(sound->status(), QSoundEffect::Error);
}


QTEST_MAIN(tst_QSoundEffect)

#include "tst_qsoundeffect.moc"
