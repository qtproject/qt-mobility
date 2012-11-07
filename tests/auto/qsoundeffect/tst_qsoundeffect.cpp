/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
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
    QUrl url;
};

void tst_QSoundEffect::initTestCase()
{
#ifdef QT_QSOUNDEFFECT_USEAPPLICATIONPATH
    url = QUrl::fromLocalFile(QCoreApplication::applicationDirPath() + QString("/test.wav"));
#else
    url = QUrl::fromLocalFile(QString(SRCDIR "test.wav"));
#endif

    sound = new QSoundEffect(this);

    QVERIFY(sound->source().isEmpty());
    QVERIFY(sound->loopCount() == 1);
    QVERIFY(sound->volume() == 1);
    QVERIFY(sound->isMuted() == false);
}

void tst_QSoundEffect::testSource()
{
    QSignalSpy readSignal(sound, SIGNAL(sourceChanged()));

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
    sound->setSource(url);
    QTestEventLoop::instance().enterLoop(1);
    sound->play();
    QTestEventLoop::instance().enterLoop(1);
    QCOMPARE(sound->isPlaying(), true);
    sound->stop();

    //empty source
    sound->setSource(QUrl());
    QTestEventLoop::instance().enterLoop(1);
    sound->play();
    QTestEventLoop::instance().enterLoop(1);
    QCOMPARE(sound->isPlaying(), false);

    //invalid source
    sound->setSource(QUrl((QLatin1String("invalid source"))));
    QTestEventLoop::instance().enterLoop(1);
    sound->play();
    QTestEventLoop::instance().enterLoop(1);
    QCOMPARE(sound->isPlaying(), false);

    sound->setLoopCount(1);
}

void tst_QSoundEffect::testStatus()
{
    sound->setSource(QUrl());
    QCOMPARE(sound->status(), QSoundEffect::Null);

    //valid source
    sound->setSource(url);

    QTestEventLoop::instance().enterLoop(1);
    QCOMPARE(sound->status(), QSoundEffect::Ready);

    //empty source
    sound->setSource(QUrl());
    QTestEventLoop::instance().enterLoop(1);
    QCOMPARE(sound->status(), QSoundEffect::Null);

    //invalid source
    sound->setLoopCount(QSoundEffect::Infinite);

    sound->setSource(QUrl(QLatin1String("invalid source")));
    QTestEventLoop::instance().enterLoop(1);
    QCOMPARE(sound->status(), QSoundEffect::Error);
}


QTEST_MAIN(tst_QSoundEffect)

#include "tst_qsoundeffect.moc"
