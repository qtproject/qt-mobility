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
#include <QtCore/qdebug.h>
#include <QtCore/qbuffer.h>

#include <QMediaPlayer>
#include <QMediaPlayerControl>
#include <QMediaPlaylist>
#include <QMediaService>
#include <QMediaStreamsControl>

QTM_USE_NAMESPACE
class AutoConnection
{
public:
    AutoConnection(QObject *sender, const char *signal, QObject *receiver, const char *method)
            : sender(sender), signal(signal), receiver(receiver), method(method)
    {
        QObject::connect(sender, signal, receiver, method);
    }

    ~AutoConnection()
    {
        QObject::disconnect(sender, signal, receiver, method);
    }

private:
    QObject *sender;
    const char *signal;
    QObject *receiver;
    const char *method;
};

class tst_QMediaPlayer: public QObject
{
    Q_OBJECT

public slots:
    void initTestCase_data();
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void testMedia();
    void testDuration();
    void testPosition();
    void testVolume();
    void testMuted();
    void testVideoAvailable();
   // //void testBufferStatus();
    void testSeekable();
   // //void testPlaybackRate();
   // //void testError();
   // //void testErrorString();
   // //void testService();
   
    void testPlay();
    void testPause();
    void testStop();
   // //void testMediaStatus();
    void testPlaylist();
};

void tst_QMediaPlayer::initTestCase_data()
{
    QTest::addColumn<bool>("valid");
    //QTest::addColumn<QMediaPlayer::State>("state");
    //QTest::addColumn<QMediaPlayer::MediaStatus>("status");
    QTest::addColumn<QMediaContent>("mediaContent");
    QTest::addColumn<qint64>("duration");
    QTest::addColumn<qint64>("position");
    QTest::addColumn<bool>("seekable");
    QTest::addColumn<int>("volume");
    QTest::addColumn<bool>("muted"); // Is this needed ???
    QTest::addColumn<bool>("videoAvailable"); // Is this needed ???
    //QTest::addColumn<int>("bufferStatus");
    //QTest::addColumn<qreal>("playbackRate");
    //QTest::addColumn<QMediaPlayer::Error>("error");
    //QTest::addColumn<QString>("errorString");

    QTest::newRow("TestDataNull")
	<< false // valid
	//<< QMediaPlayer::StoppedState // state
	//<< QMediaPlayer::UnknownMediaStatus // status
    << QMediaContent() // mediaContent
	<< qint64(-1) // duration
	<< qint64(0) // position
	<< false // seekable
	<< 0 // volume
	<< false // muted
	<< false; // videoAvailable
	//<< 0 // bufferStatus
	//<< qreal(0) // playbackRate
	//<< QMediaPlayer::NoError // error
	//<< QString(); // errorString

    /*
    QTest::newRow("test_3gp.3gp")
	<< true // valid
	//<< QMediaPlayer::StoppedState // state
	//<< QMediaPlayer::UnknownMediaStatus // status
	<< QMediaContent(QUrl("file:///C:/data/testfiles/test_3gp.3gp")) // mediaContent
	<< qint64(-1) // duration
	<< qint64(0) // position
	<< false // seekable
	<< 0 // volume
	<< false // muted
	<< false; // videoAvailable
	//<< 0 // bufferStatus
	//<< qreal(0) // playbackRate
	//<< QMediaPlayer::NoError // error
	//<< QString(); // errorString
    */

    QTest::newRow("test_amr.amr")
	<< true // valid
	//<< QMediaPlayer::StoppedState // state
	//<< QMediaPlayer::UnknownMediaStatus // status
	<< QMediaContent(QUrl("file:///C:/data/testfiles/test_amr.amr")) // mediaContent
	<< qint64(14400) // duration
	<< qint64(200) // position
	<< true // seekable
	<< 0 // volume
	<< false // muted
	<< false; // videoAvailable
	//<< 0 // bufferStatus
	//<< qreal(0) // playbackRate
	//<< QMediaPlayer::NoError // error
	//<< QString(); // errorString

    /*
    QTest::newRow("test_flash_video.flv")
	<< true // valid
	//<< QMediaPlayer::StoppedState // state
	//<< QMediaPlayer::UnknownMediaStatus // status
	<< QMediaContent(QUrl("file:///C:/data/testfiles/test_flash_video.flv")) // mediaContent
	<< qint64(-1) // duration
	<< qint64(0) // position
	<< true // seekable
	<< 0 // volume
	<< false // muted
	<< false; // videoAvailable
	//<< 0 // bufferStatus
	//<< qreal(0) // playbackRate
	//<< QMediaPlayer::NoError // error
	//<< QString(); // errorString
    */

    QTest::newRow("test_invalid_extension_mp4.xyz")
    << true // valid
    //<< QMediaPlayer::StoppedState // state
    //<< QMediaPlayer::UnknownMediaStatus // status
    << QMediaContent(QUrl("file:///C:/data/testfiles/test_invalid_extension_mp4.xyz")) // mediaContent
    << qint64(2701) // duration //TODO: FIXME: In this case duration could not be read
    << qint64(0) // position
    << false // seekable
    << 0 // volume
    << false // muted
    << false; // videoAvailable
    //<< 0 // bufferStatus
    //<< qreal(0) // playbackRate
    //<< QMediaPlayer::NoError // error
    //<< QString(); // errorString

    QTest::newRow("test_invalid_extension_wav.xyz")
	<< true // valid
	//<< QMediaPlayer::StoppedState // state
	//<< QMediaPlayer::UnknownMediaStatus // status
	<< QMediaContent(QUrl("file:///C:/data/testfiles/test_invalid_extension_wav.xyz")) // mediaContent
	<< qint64(12400) // duration
	<< qint64(400) // position
	<< true // seekable
	<< 0 // volume
	<< false // muted
	<< false; // videoAvailable
	//<< 0 // bufferStatus
	//<< qreal(0) // playbackRate
	//<< QMediaPlayer::NoError // error
	//<< QString(); // errorString

    QTest::newRow("test_mp3.mp3")
	<< true // valid
	//<< QMediaPlayer::StoppedState // state
	//<< QMediaPlayer::UnknownMediaStatus // status
	<< QMediaContent(QUrl("file:///C:/data/testfiles/test_mp3.mp3")) // mediaContent
	<< qint64(102044) // duration
	<< qint64(0) // position
	<< false // seekable
	<< 0 // volume
	<< false // muted
	<< false; // videoAvailable
	//<< 0 // bufferStatus
	//<< qreal(0) // playbackRate
	//<< QMediaPlayer::NoError // error
	//<< QString(); // errorString

    QTest::newRow("test_mp4.mp4")
    << true // valid
    //<< QMediaPlayer::StoppedState // state
    //<< QMediaPlayer::UnknownMediaStatus // status
    << QMediaContent(QUrl("file:///C:/data/testfiles/test_mp4.mp4")) // mediaContent
    << qint64(2701) // duration //TODO: FIXME: In this case duration could not be read
    << qint64(0) // position
    << false // seekable
    << 0 // volume
    << false // muted
    << false; // videoAvailable
    //<< 0 // bufferStatus
    //<< qreal(0) // playbackRate
    //<< QMediaPlayer::NoError // error
    //<< QString(); // errorString

    QTest::newRow("test_wav.wav")
	<< true // valid
	//<< QMediaPlayer::StoppedState // state
	//<< QMediaPlayer::UnknownMediaStatus // status
	<< QMediaContent(QUrl("file:///C:/data/testfiles/test_wav.wav")) // mediaContent
	<< qint64(12400) // duration
	<< qint64(321) // position
	<< true // seekable
	<< 0 // volume
	<< false // muted
	<< false; // videoAvailable
	//<< 0 // bufferStatus
	//<< qreal(0) // playbackRate
	//<< QMediaPlayer::NoError // error
	//<< QString(); // errorString

    QTest::newRow("test_wmv9.wmv")
	<< true // valid
	//<< QMediaPlayer::StoppedState // state
	//<< QMediaPlayer::UnknownMediaStatus // status
	<< QMediaContent(QUrl("file:///C:/data/testfiles/test_wmv9.wmv")) // mediaContent
	<< qint64(169389) // duration
	<< qint64(789) // position
	<< true // seekable
	<< 0 // volume
	<< false // muted
	<< false; // videoAvailable
	//<< 0 // bufferStatus
	//<< qreal(0) // playbackRate
	//<< QMediaPlayer::NoError // error
	//<< QString(); // errorString

    QTest::newRow("test youtube stream")
    << true // valid
    //<< QMediaPlayer::StoppedState // state
    //<< QMediaPlayer::UnknownMediaStatus // status
    << QMediaContent(QUrl("rtsp://v3.cache4.c.youtube.com/CkgLENy73wIaPwlU2rm7yu8PFhMYESARFEIJbXYtZ29vZ2xlSARSB3JlbGF0ZWRaDkNsaWNrVGh1bWJuYWlsYPi6_IXT2rvpSgw=/0/0/0/video.3gp")) // mediaContent
    << qint64(0) // duration
    << qint64(0) // position
    << true // seekable
    << 0 // volume
    << false // muted
    << false; // videoAvailable
    //<< 0 // bufferStatus
    //<< qreal(0) // playbackRate
    //<< QMediaPlayer::NoError // error
    //<< QString(); // errorString

    /*
    QTest::newRow("invalid") << false << QMediaPlayer::StoppedState << QMediaPlayer::UnknownMediaStatus <<
                                QMediaContent() << qint64(-1) << qint64(0) << false << 0 << false << false << 0 <<
                                qreal(0) << QMediaPlayer::NoError << QString();
    QTest::newRow("valid+null") << true << QMediaPlayer::StoppedState << QMediaPlayer::UnknownMediaStatus <<
                                QMediaContent() << qint64(0) << qint64(0) << false << 0 << false << false << 50 <<
                                qreal(0) << QMediaPlayer::NoError << QString();
    QTest::newRow("valid+content+stopped") << true << QMediaPlayer::StoppedState << QMediaPlayer::UnknownMediaStatus <<
                                QMediaContent(QUrl("file:///some.mp3")) << qint64(0) << qint64(0) << false << 50 << false << false << 0 <<
                                qreal(1) << QMediaPlayer::NoError << QString();
    QTest::newRow("valid+content+playing") << true << QMediaPlayer::PlayingState << QMediaPlayer::LoadedMedia <<
                                QMediaContent(QUrl("file:///some.mp3")) << qint64(10000) << qint64(10) << true << 50 << true << false << 0 <<
                                qreal(1) << QMediaPlayer::NoError << QString();
    QTest::newRow("valid+content+paused") << true << QMediaPlayer::PausedState << QMediaPlayer::LoadedMedia <<
                                QMediaContent(QUrl("file:///some.mp3")) << qint64(10000) << qint64(10) << true << 50 << true << false << 0 <<
                                qreal(1)  << QMediaPlayer::NoError << QString();
    QTest::newRow("valud+streaming") << true << QMediaPlayer::PlayingState << QMediaPlayer::LoadedMedia <<
                                QMediaContent(QUrl("http://example.com/stream")) << qint64(10000) << qint64(10000) << false << 50 << false << true << 0 <<
                                qreal(1)  << QMediaPlayer::NoError << QString();
    QTest::newRow("valid+error") << true << QMediaPlayer::StoppedState << QMediaPlayer::UnknownMediaStatus <<
                                QMediaContent(QUrl("http://example.com/stream")) << qint64(0) << qint64(0) << false << 50 << false << false << 0 <<
                                qreal(0) << QMediaPlayer::ResourceError << QString("Resource unavailable");
    */
}

void tst_QMediaPlayer::initTestCase()
{
}

void tst_QMediaPlayer::cleanupTestCase()
{
}

void tst_QMediaPlayer::init()
{
	qRegisterMetaType<QMediaPlayer::State>("QMediaPlayer::State");
	qRegisterMetaType<QMediaPlayer::Error>("QMediaPlayer::Error");
	qRegisterMetaType<QMediaPlayer::MediaStatus>("QMediaPlayer::MediaStatus");
	qRegisterMetaType<QMediaContent>("QMediaContent");
}

void tst_QMediaPlayer::cleanup()
{
}

void tst_QMediaPlayer::testMedia()
{
    QFETCH_GLOBAL(QMediaContent, mediaContent);
    
    QMediaPlayer player;

    player.setMedia(mediaContent);
    QTest::qWait(500);
    QCOMPARE(player.media(), mediaContent);

    // QBuffer stream;
    // player.setMedia(mediaContent, &stream);
    // QCOMPARE(player.media(), mediaContent);
    // QCOMPARE((QBuffer*)player.mediaStream(), &stream);
}

void tst_QMediaPlayer::testDuration()
{
    QFETCH_GLOBAL(QMediaContent, mediaContent);
    QFETCH_GLOBAL(qint64, duration);

    QMediaPlayer player;
    player.setMedia(mediaContent);
    QTest::qWait(500);

    
    QEXPECT_FAIL("", "player.duration() failed: ", Continue);
    QVERIFY(player.duration() == duration);
}


void tst_QMediaPlayer::testPosition()
{
    QFETCH_GLOBAL(QMediaContent, mediaContent);
    QFETCH_GLOBAL(qint64, duration);
    QFETCH_GLOBAL(qint64, position);
    QFETCH_GLOBAL(bool, seekable);

    QMediaPlayer player;

    player.setMedia(mediaContent);
    QTest::qWait(500);
    player.setPosition(position);

    qDebug() << "Mediafile" << player.media().canonicalUrl().toString();
    QEXPECT_FAIL("", "player.isSeekable() failed: ", Continue);
    QVERIFY(player.isSeekable() == seekable);
    QEXPECT_FAIL("", "player.position() failed: ", Continue);
    QVERIFY(player.position() == position);
    QEXPECT_FAIL("", "player.duration() failed: ", Continue);
    QVERIFY(player.duration() == duration);

    if (seekable) {
        { QSignalSpy spy(&player, SIGNAL(positionChanged(qint64)));
        player.setPosition(position);
        qDebug() << player.position();
        QCOMPARE(player.position(), position);
        QCOMPARE(spy.count(), 0); }

        player.setPosition(position);
        { QSignalSpy spy(&player, SIGNAL(positionChanged(qint64)));
        player.setPosition(0);
        QCOMPARE(player.position(), qint64(0));
        QCOMPARE(spy.count(), position == 0 ? 0 : 1); }

        player.setPosition(position);
        { QSignalSpy spy(&player, SIGNAL(positionChanged(qint64)));
        player.setPosition(duration);
        QEXPECT_FAIL("", "player.duration() failed: ", Continue);
        QCOMPARE(player.position(), duration);
        QCOMPARE(spy.count(), position == duration ? 0 : 1); }

        player.setPosition(position);
        { QSignalSpy spy(&player, SIGNAL(positionChanged(qint64)));
        player.setPosition(-1);
        QCOMPARE(player.position(), qint64(0));
        QCOMPARE(spy.count(), position == 0 ? 0 : 1); }

        player.setPosition(position);
        { QSignalSpy spy(&player, SIGNAL(positionChanged(qint64)));
        player.setPosition(duration + 1);
        QCOMPARE(player.position(), duration);
        QCOMPARE(spy.count(), position == duration ? 0 : 1); }
    }
    else {
        QSignalSpy spy(&player, SIGNAL(positionChanged(qint64)));
        player.setPosition(position);

        QCOMPARE(player.position(), position);
        QCOMPARE(spy.count(), 0);
    }
}

void tst_QMediaPlayer::testVolume()
{
    QFETCH_GLOBAL(QMediaContent, mediaContent);
    QFETCH_GLOBAL(bool, valid);
    QFETCH_GLOBAL(int, volume);

    QMediaPlayer player;

    player.setMedia(mediaContent);
    QTest::qWait(500);

    player.setVolume(volume);
    QVERIFY(player.volume() == volume);

    //TODO: Remove this IF
    if (valid) {
        { QSignalSpy spy(&player, SIGNAL(volumeChanged(int)));
        player.setVolume(10);
        QCOMPARE(player.volume(), 10);
        QCOMPARE(spy.count(), 1); }

        { QSignalSpy spy(&player, SIGNAL(volumeChanged(int)));
        player.setVolume(-1000);
        QCOMPARE(player.volume(), 0);
        QCOMPARE(spy.count(), 1); }

        { QSignalSpy spy(&player, SIGNAL(volumeChanged(int)));
        player.setVolume(100);
        QCOMPARE(player.volume(), 100);
        QCOMPARE(spy.count(), 1); }

        { QSignalSpy spy(&player, SIGNAL(volumeChanged(int)));
        player.setVolume(1000);
        QCOMPARE(player.volume(), 100);
        QCOMPARE(spy.count(), 0); }
    }
}

void tst_QMediaPlayer::testMuted()
{
    QFETCH_GLOBAL(bool, valid);
    QFETCH_GLOBAL(int, volume);
    QFETCH_GLOBAL(bool, muted);
    
    // TODO Use valid
    
    QMediaPlayer player;

    if (valid) {
        player.setMuted(muted);
        player.setVolume(volume);
        QVERIFY(player.isMuted() == muted);

        QSignalSpy spy(&player, SIGNAL(mutedChanged(bool)));
        player.setMuted(!muted);
        QCOMPARE(player.isMuted(), !muted);
        QCOMPARE(player.volume(), volume);
        QCOMPARE(spy.count(), 1);
    }
}

void tst_QMediaPlayer::testVideoAvailable()
{
    QFETCH_GLOBAL(bool, videoAvailable);
    QFETCH_GLOBAL(QMediaContent, mediaContent);
    
    QMediaPlayer player;

    player.setMedia(mediaContent);
    QTest::qWait(500);
    QVERIFY(player.isVideoAvailable() == videoAvailable);
}

/*
void tst_QMediaPlayer::testBufferStatus()
{
    QFETCH_GLOBAL(int, bufferStatus);

    QMediaPlayer player;

    mockService->setBufferStatus(bufferStatus);
    QVERIFY(player.bufferStatus() == bufferStatus);
}
*/

void tst_QMediaPlayer::testSeekable()
{
    QFETCH_GLOBAL(QMediaContent, mediaContent);
    QFETCH_GLOBAL(bool, seekable);
    
    QMediaPlayer player;

    player.setMedia(mediaContent);
    QTest::qWait(500);
    QVERIFY(player.isSeekable() == seekable);
}

/*
void tst_QMediaPlayer::testPlaybackRate()
{
    QFETCH_GLOBAL(bool, valid);
    QFETCH_GLOBAL(qreal, playbackRate);

    QMediaPlayer player;

    if (valid) {
        player.setPlaybackRate(playbackRate);
        QVERIFY(player.playbackRate() == playbackRate);

        QSignalSpy spy(&player, SIGNAL(playbackRateChanged(qreal)));
        player.setPlaybackRate(playbackRate + 0.5f);
        QCOMPARE(player.playbackRate(), playbackRate + 0.5f);
        QCOMPARE(spy.count(), 1);
    }
}
*/

/*
void tst_QMediaPlayer::testError()
{
    QFETCH_GLOBAL(QMediaPlayer::Error, error);
    

    QMediaPlayer player;

    //mockService->setError(error);
    //QVERIFY(player.error() == error);
}
*/

/*
void tst_QMediaPlayer::testErrorString()
{
    QFETCH_GLOBAL(QString, errorString);
    
    QMediaPlayer player;

    //mockService->setErrorString(errorString);
    //QVERIFY(player.errorString() == errorString);
}
*/

/*
void tst_QMediaPlayer::testService()
{
    QFETCH_GLOBAL(bool, valid);
    
    QMediaPlayer player;

    mockService->setIsValid(valid);

    if (valid)
        QVERIFY(player.service() != 0);
    else
        QVERIFY(player.service() == 0);
}
*/

void tst_QMediaPlayer::testPlay()
{
    QFETCH_GLOBAL(bool, valid);
    QFETCH_GLOBAL(QMediaContent, mediaContent);

    QMediaPlayer player;

    player.setMedia(mediaContent);
    QTest::qWait(500);
    QVERIFY(player.state() == QMediaPlayer::StoppedState);
    QVERIFY(player.media() == mediaContent);

    {
        QSignalSpy spy(&player, SIGNAL(stateChanged(QMediaPlayer::State)));

        player.play();
        QTest::qWait(500);

        if (!valid || mediaContent.isNull())  {
            QCOMPARE(player.state(), QMediaPlayer::StoppedState);
            QCOMPARE(spy.count(), 0);
        }
        else {
            QCOMPARE(player.state(), QMediaPlayer::PlayingState);
            QCOMPARE(spy.count(), 1);
        }
    }

    {
        QSignalSpy spy(&player, SIGNAL(stateChanged(QMediaPlayer::State)));

        player.play();
        QTest::qWait(500);

        if (!valid || mediaContent.isNull()) {
            QCOMPARE(player.state(), QMediaPlayer::StoppedState);
            QCOMPARE(spy.count(), 0);
        }
        else {
            QCOMPARE(player.state(), QMediaPlayer::PlayingState);
            QCOMPARE(spy.count(), 0);
        }
    }
}

void tst_QMediaPlayer::testPause()
{
    QFETCH_GLOBAL(bool, valid);
    QFETCH_GLOBAL(QMediaContent, mediaContent);

    QMediaPlayer player;

    player.setMedia(mediaContent);
    QTest::qWait(500);
    QCOMPARE(player.state(), QMediaPlayer::StoppedState);
    QVERIFY(player.media() == mediaContent);

    player.play();
    QTest::qWait(500);

    {
        QSignalSpy spy(&player, SIGNAL(stateChanged(QMediaPlayer::State)));

        player.pause();

        if (!valid || mediaContent.isNull()) {
            QCOMPARE(player.state(), QMediaPlayer::StoppedState);
            QCOMPARE(spy.count(), 0);
        }
        else {
            QCOMPARE(player.state(), QMediaPlayer::PausedState);
            QCOMPARE(spy.count(), 1);
        }
    }

    {
        QSignalSpy spy(&player, SIGNAL(stateChanged(QMediaPlayer::State)));

        player.pause();
        QTest::qWait(500);

        if (!valid || mediaContent.isNull()) {
            QCOMPARE(player.state(), QMediaPlayer::StoppedState);
            QCOMPARE(spy.count(), 0);
        }
        else {
            QCOMPARE(player.state(), QMediaPlayer::PausedState);
            QCOMPARE(spy.count(), 0);
        }
    }
}

void tst_QMediaPlayer::testStop()
{
    QFETCH_GLOBAL(QMediaContent, mediaContent);

    QMediaPlayer player;

    player.setMedia(mediaContent);
    QTest::qWait(500);
    QVERIFY(player.state() == QMediaPlayer::StoppedState);
    QVERIFY(player.media() == mediaContent);

    player.play();
    QTest::qWait(500);

    {
        QSignalSpy spy(&player, SIGNAL(stateChanged(QMediaPlayer::State)));

        player.stop();
        QTest::qWait(500);

        if (mediaContent.isNull()) {
            QCOMPARE(player.state(), QMediaPlayer::StoppedState);
            QCOMPARE(spy.count(), 0);
        }
        else {
            QCOMPARE(player.state(), QMediaPlayer::StoppedState);
            QCOMPARE(spy.count(), 1);
        }
    }

    {
        QSignalSpy spy(&player, SIGNAL(stateChanged(QMediaPlayer::State)));

        if (mediaContent.isNull() || player.state() == QMediaPlayer::StoppedState) {
            QCOMPARE(player.state(), QMediaPlayer::StoppedState);
            QCOMPARE(spy.count(), 0);
        }
    }
}

/*
void tst_QMediaPlayer::testMediaStatus()
{
    QFETCH_GLOBAL(int, bufferStatus);
    
    QMediaPlayer player;
    
    int bufferSignals = 0;

    player.setNotifyInterval(10);

    //mockService->setMediaStatus(QMediaPlayer::NoMedia);
    //mockService->setBufferStatus(bufferStatus);

    AutoConnection connection(
            &player, SIGNAL(bufferStatusChanged(int)),
            &QTestEventLoop::instance(), SLOT(exitLoop()));

    QSignalSpy statusSpy(&player, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)));
    QSignalSpy bufferSpy(&player, SIGNAL(bufferStatusChanged(int)));

    QCOMPARE(player.mediaStatus(), QMediaPlayer::NoMedia);

    //mockService->setMediaStatus(QMediaPlayer::LoadingMedia);
    QCOMPARE(player.mediaStatus(), QMediaPlayer::LoadingMedia);
    QCOMPARE(statusSpy.count(), 1);
    QCOMPARE(qvariant_cast<QMediaPlayer::MediaStatus>(statusSpy.last().value(0)),
             QMediaPlayer::LoadingMedia);

    //mockService->setMediaStatus(QMediaPlayer::LoadedMedia);
    QCOMPARE(player.mediaStatus(), QMediaPlayer::LoadedMedia);
    QCOMPARE(statusSpy.count(), 2);
    QCOMPARE(qvariant_cast<QMediaPlayer::MediaStatus>(statusSpy.last().value(0)),
             QMediaPlayer::LoadedMedia);

    // Verify the bufferStatusChanged() signal isn't being emitted.
    QTestEventLoop::instance().enterLoop(1);
    QCOMPARE(bufferSpy.count(), 0);

    //mockService->setMediaStatus(QMediaPlayer::StalledMedia);
    QCOMPARE(player.mediaStatus(), QMediaPlayer::StalledMedia);
    QCOMPARE(statusSpy.count(), 3);
    QCOMPARE(qvariant_cast<QMediaPlayer::MediaStatus>(statusSpy.last().value(0)),
             QMediaPlayer::StalledMedia);

    // Verify the bufferStatusChanged() signal is being emitted.
    QTestEventLoop::instance().enterLoop(1);
    QVERIFY(bufferSpy.count() > bufferSignals);
    QCOMPARE(bufferSpy.last().value(0).toInt(), bufferStatus);
    bufferSignals = bufferSpy.count();
    
    /-*
    mockService->setMediaStatus(QMediaPlayer::BufferingMedia);
    QCOMPARE(player.mediaStatus(), QMediaPlayer::BufferingMedia);
    QCOMPARE(statusSpy.count(), 4);
    QCOMPARE(qvariant_cast<QMediaPlayer::MediaStatus>(statusSpy.last().value(0)),
             QMediaPlayer::BufferingMedia);
    *-/

    // Verify the bufferStatusChanged() signal is being emitted.
    QTestEventLoop::instance().enterLoop(1);
    QVERIFY(bufferSpy.count() > bufferSignals);
    QCOMPARE(bufferSpy.last().value(0).toInt(), bufferStatus);
    bufferSignals = bufferSpy.count();

    //mockService->setMediaStatus(QMediaPlayer::BufferedMedia);
    QCOMPARE(player.mediaStatus(), QMediaPlayer::BufferedMedia);
    QCOMPARE(statusSpy.count(), 5);
    QCOMPARE(qvariant_cast<QMediaPlayer::MediaStatus>(statusSpy.last().value(0)),
             QMediaPlayer::BufferedMedia);

    // Verify the bufferStatusChanged() signal isn't being emitted.
    QTestEventLoop::instance().enterLoop(1);
    QCOMPARE(bufferSpy.count(), bufferSignals);

    //mockService->setMediaStatus(QMediaPlayer::EndOfMedia);
    QCOMPARE(player.mediaStatus(), QMediaPlayer::EndOfMedia);
    QCOMPARE(statusSpy.count(), 6);
    QCOMPARE(qvariant_cast<QMediaPlayer::MediaStatus>(statusSpy.last().value(0)),
             QMediaPlayer::EndOfMedia);
}
*/

void tst_QMediaPlayer::testPlaylist()
{
    QMediaContent content0(QUrl(QLatin1String("file:///c:/data/test.mp3"))); //audio
    QMediaContent content1(QUrl(QLatin1String("file:///c:/data/test.mp3"))); //audio
    QMediaContent content2(QUrl(QLatin1String("file:///c:/data/test.mp4"))); //video
    QMediaContent content3(QUrl(QLatin1String("file:///c:/data/test.mp4"))); //video
    QMediaContent content4(QUrl(QLatin1String("file:///c:/data/test.mp3"))); //image

    QMediaPlayer player;

    QMediaPlaylist *playlist = new QMediaPlaylist(&player);

    QSignalSpy stateSpy(&player, SIGNAL(stateChanged(QMediaPlayer::State)));
    QSignalSpy mediaSpy(&player, SIGNAL(mediaChanged(QMediaContent)));

    // Test the player does nothing with an empty playlist attached.
    player.play();
    QCOMPARE(player.state(), QMediaPlayer::StoppedState);
    QCOMPARE(player.media(), QMediaContent());
    QCOMPARE(stateSpy.count(), 0);
    QCOMPARE(mediaSpy.count(), 0);

    playlist->addMedia(content0);
    playlist->addMedia(content1);
    playlist->addMedia(content2);
    playlist->addMedia(content3);

    // Test changing the playlist position, changes the current media, but not the playing state.
    playlist->setCurrentIndex(1);
    QCOMPARE(player.media(), content1);
    QCOMPARE(player.state(), QMediaPlayer::StoppedState);
    QCOMPARE(stateSpy.count(), 0);
    QCOMPARE(mediaSpy.count(), 1);

    // Test playing starts with the current media.
    player.play();
    QCOMPARE(player.media(), content1);
    QCOMPARE(player.state(), QMediaPlayer::PlayingState);
    QCOMPARE(stateSpy.count(), 1);
    QCOMPARE(mediaSpy.count(), 1);

    // Test pausing doesn't change the current media.
    player.pause();
    QCOMPARE(player.media(), content1);
    QCOMPARE(player.state(), QMediaPlayer::PausedState);
    QCOMPARE(stateSpy.count(), 2);
    QCOMPARE(mediaSpy.count(), 1);

    // Test stopping doesn't change the current media.
    player.stop();
    QCOMPARE(player.media(), content1);
    QCOMPARE(player.state(), QMediaPlayer::StoppedState);
    QCOMPARE(stateSpy.count(), 3);
    QCOMPARE(mediaSpy.count(), 1);

    // Test when the player service reaches the end of the current media, the player moves onto
    // the next item without stopping.
    player.play();
    QCOMPARE(player.media(), content1);
    QCOMPARE(player.state(), QMediaPlayer::PlayingState);
    QCOMPARE(stateSpy.count(), 4);
    QCOMPARE(mediaSpy.count(), 1);

    /*
    // TODO FIXME: This test case needs to be refactored after duration and setposition works
    //mockService->setState(QMediaPlayer::StoppedState, QMediaPlayer::EndOfMedia);
    QCOMPARE(player.media(), content2);
    QCOMPARE(player.state(), QMediaPlayer::PlayingState);
    QCOMPARE(stateSpy.count(), 4);
    QCOMPARE(mediaSpy.count(), 2);

    // Test skipping the current media doesn't change the state.
    playlist->next();
    QCOMPARE(player.media(), content3);
    QCOMPARE(player.state(), QMediaPlayer::PlayingState);
    QCOMPARE(stateSpy.count(), 4);
    QCOMPARE(mediaSpy.count(), 3);

    // Test changing the current media while paused doesn't change the state.
    player.pause();
    //mockService->setMediaStatus(QMediaPlayer::BufferedMedia);
    QCOMPARE(player.media(), content3);
    QCOMPARE(player.state(), QMediaPlayer::PausedState);
    QCOMPARE(stateSpy.count(), 5);
    QCOMPARE(mediaSpy.count(), 3);

    playlist->previous();
    QCOMPARE(player.media(), content2);
    QCOMPARE(player.state(), QMediaPlayer::PausedState);
    QCOMPARE(stateSpy.count(), 5);
    QCOMPARE(mediaSpy.count(), 4);

    // Test changing the current media while stopped doesn't change the state.
    player.stop();
    //mockService->setMediaStatus(QMediaPlayer::LoadedMedia);
    QCOMPARE(player.media(), content2);
    QCOMPARE(player.state(), QMediaPlayer::StoppedState);
    QCOMPARE(stateSpy.count(), 6);
    QCOMPARE(mediaSpy.count(), 4);

    playlist->next();
    QCOMPARE(player.media(), content3);
    QCOMPARE(player.state(), QMediaPlayer::StoppedState);
    QCOMPARE(stateSpy.count(), 6);
    QCOMPARE(mediaSpy.count(), 5);

    // Test the player is stopped and the current media cleared when it reaches the end of the last
    // item in the playlist.
    player.play();
    QCOMPARE(player.media(), content3);
    QCOMPARE(player.state(), QMediaPlayer::PlayingState);
    QCOMPARE(stateSpy.count(), 7);
    QCOMPARE(mediaSpy.count(), 5);

    // Double up the signals to ensure some noise doesn't destabalize things.
    //mockService->setState(QMediaPlayer::StoppedState, QMediaPlayer::EndOfMedia);
    //mockService->setState(QMediaPlayer::StoppedState, QMediaPlayer::EndOfMedia);
    QCOMPARE(player.media(), QMediaContent());
    QCOMPARE(player.state(), QMediaPlayer::StoppedState);
    QCOMPARE(stateSpy.count(), 8);
    QCOMPARE(mediaSpy.count(), 6);

    // Test starts playing from the start of the playlist if there is no current media selected.
    player.play();
    QCOMPARE(player.media(), content0);
    QCOMPARE(player.state(), QMediaPlayer::PlayingState);
    QCOMPARE(stateSpy.count(), 9);
    QCOMPARE(mediaSpy.count(), 7);
    */

    // TODO FIXME: This part of needs to be used after using refactored test cases before
    //             Here are original signal spy values.
    /*
    // Test deleting the playlist stops the player and clears the media it set.
    delete playlist;
    QCOMPARE(player.media(), QMediaContent());
    QCOMPARE(player.state(), QMediaPlayer::StoppedState);
    QCOMPARE(stateSpy.count(), 10);
    QCOMPARE(mediaSpy.count(), 8);

    // Test the player works as normal with the playlist removed.
    player.play();
    QCOMPARE(player.media(), QMediaContent());
    QCOMPARE(player.state(), QMediaPlayer::StoppedState);
    QCOMPARE(stateSpy.count(), 10);
    QCOMPARE(mediaSpy.count(), 8);

    player.setMedia(content1);
    player.play();

    QCOMPARE(player.media(), content1);
    QCOMPARE(player.state(), QMediaPlayer::PlayingState);
    QCOMPARE(stateSpy.count(), 11);
    QCOMPARE(mediaSpy.count(), 9);
    */
    
    // Test deleting the playlist stops the player and clears the media it set.
    delete playlist;
    QCOMPARE(player.media(), QMediaContent());
    QCOMPARE(player.state(), QMediaPlayer::StoppedState);
    QCOMPARE(stateSpy.count(), 4);
    QCOMPARE(mediaSpy.count(), 1);

    // Test the player works as normal with the playlist removed.
    player.play();
    QCOMPARE(player.media(), QMediaContent());
    QCOMPARE(player.state(), QMediaPlayer::StoppedState);
    QCOMPARE(stateSpy.count(), 4);
    QCOMPARE(mediaSpy.count(), 1);

    player.setMedia(content1);
    player.play();

    QCOMPARE(player.media(), content1);
    QCOMPARE(player.state(), QMediaPlayer::PlayingState);
    QCOMPARE(stateSpy.count(), 4);
    QCOMPARE(mediaSpy.count(), 2);
}

QTEST_MAIN(tst_QMediaPlayer)

#include "tst_qmediaplayer.moc"
