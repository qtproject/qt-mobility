/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
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
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QtTest>
#include <QDebug>

#include <multimedia/qmediaplayercontrol.h>
#include <multimedia/qmediaservice.h>
#include <multimedia/qmediaplayer.h>


class MockPlayerControl : public QMediaPlayerControl
{
    friend class MockPlayerService;

public:
    MockPlayerControl():QMediaPlayerControl(0) {}

    QMediaPlayer::State state() const { return _state; }
    QMediaPlayer::MediaStatus mediaStatus() const { return _mediaStatus; }

    qint64 duration() const { return _duration; }

    qint64 position() const { return _position; }

    void setPosition(qint64 position) { if (position != _position) emit positionChanged(_position = position); }

    int volume() const { return _volume; }
    void setVolume(int volume) { emit volumeChanged(_volume = volume); }

    bool isMuted() const { return _muted; }
    void setMuted(bool muted) { _muted = muted; }

    int bufferStatus() const { return _bufferStatus; }

    bool isVideoAvailable() const { return _videoAvailable; }

    bool isSeekable() const { return _isSeekable; }

    float playbackRate() const { return _playbackRate; }
    void setPlaybackRate(float rate) { _playbackRate = rate; }

    QMediaSource media() const { return _media; }
    void setMedia(const QMediaSource &source, QIODevice *stream) { Q_UNUSED(stream); _media = source; }
    QIODevice *mediaStream() const { return 0; }

    void play() { if (_isValid && !_media.isNull() && _state != QMediaPlayer::PlayingState) emit stateChanged(_state = QMediaPlayer::PlayingState); }
    void pause() { if (_isValid && !_media.isNull() && _state != QMediaPlayer::PausedState && _state != QMediaPlayer::StoppedState) emit stateChanged(_state = QMediaPlayer::PausedState); }
    void stop() { if (_state != QMediaPlayer::StoppedState) emit stateChanged(_state = QMediaPlayer::StoppedState); }

    QMediaPlayer::State _state;
    QMediaPlayer::MediaStatus _mediaStatus;
    QMediaPlayer::Error _error;
    qint64 _duration;
    qint64 _position;
    int _volume;
    bool _muted;
    int _bufferStatus;
    bool _videoAvailable;
    bool _isSeekable;
    float _playbackRate;
    QMediaSource _media;
    bool _isValid;
    QString _errorString;
};

class MockPlayerService : public QMediaService
{
    Q_OBJECT

public:
    MockPlayerService():QMediaService(0)
    {
        mockControl = new MockPlayerControl;
    }

    QMediaControl* control(const char *iid) const
    {
        if (qstrcmp(iid, QMediaPlayerControl_iid) == 0)
            return mockControl;

        return 0;
    }

    void setState(QMediaPlayer::State state) { mockControl->_state = state; }
    void setMediaStatus(QMediaPlayer::MediaStatus status) { mockControl->_mediaStatus = status; }
    void setIsValid(bool isValid) { mockControl->_isValid = isValid; }
    void setMedia(QMediaSource media) { mockControl->_media = media; }
    void setDuration(qint64 duration) { mockControl->_duration = duration; }
    void setPosition(qint64 position) { mockControl->_position = position; }
    void setSeekable(bool seekable) { mockControl->_isSeekable = seekable; }
    void setVolume(int volume) { mockControl->_volume = volume; }
    void setMuted(bool muted) { mockControl->_muted = muted; }
    void setVideoAvailable(bool videoAvailable) { mockControl->_videoAvailable = videoAvailable; }
    void setBufferStatus(int bufferStatus) { mockControl->_bufferStatus = bufferStatus; }
    void setPlaybackRate(float playbackRate) { mockControl->_playbackRate = playbackRate; }
    void setError(QMediaPlayer::Error error) { mockControl->_error = error; emit mockControl->error(mockControl->_error, mockControl->_errorString); }
    void setErrorString(QString errorString) { mockControl->_errorString = errorString; emit mockControl->error(mockControl->_error, mockControl->_errorString); }

    void reset()
    {
        mockControl->_state = QMediaPlayer::StoppedState;
        mockControl->_mediaStatus = QMediaPlayer::UnknownMediaStatus;
        mockControl->_error = QMediaPlayer::NoError;
        mockControl->_duration = 0;
        mockControl->_position = 0;
        mockControl->_volume = 0;
        mockControl->_muted = false;
        mockControl->_bufferStatus = 0;
        mockControl->_videoAvailable = false;
        mockControl->_isSeekable = false;
        mockControl->_playbackRate = 0.0;
        mockControl->_media = QMediaSource();
        mockControl->_isValid = false;
        mockControl->_errorString = QString();
    }

    MockPlayerControl *mockControl;
};

class MockProvider : public QMediaServiceProvider
{
public:
    MockProvider(MockPlayerService *service):mockService(service) {}
    QMediaService *requestService(const QByteArray &, const QList<QByteArray> &)
    {
        return mockService;
    }

    void releaseService(QMediaService *) {}

    MockPlayerService *mockService;
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
    void testValid();
    void testMedia();
    void testDuration();
    void testPosition();
    void testVolume();
    void testMuted();
    void testVideoAvailable();
    void testBufferStatus();
    void testSeekable();
    void testPlaybackRate();
    void testError();
    void testErrorString();
    void testService();
    void testPlay();
    void testPause();
    void testStop();

private:
    MockProvider *mockProvider;
    MockPlayerService  *mockService;
    QMediaPlayer *player;
};

void tst_QMediaPlayer::initTestCase_data()
{
    QTest::addColumn<bool>("valid");
    QTest::addColumn<QMediaPlayer::State>("state");
    QTest::addColumn<QMediaPlayer::MediaStatus>("status");
    QTest::addColumn<QMediaSource>("mediaSource");
    QTest::addColumn<qint64>("duration");
    QTest::addColumn<qint64>("position");
    QTest::addColumn<bool>("seekable");
    QTest::addColumn<int>("volume");
    QTest::addColumn<bool>("muted");
    QTest::addColumn<bool>("videoAvailable");
    QTest::addColumn<int>("bufferStatus");
    QTest::addColumn<float>("playbackRate");
    QTest::addColumn<QMediaPlayer::Error>("error");
    QTest::addColumn<QString>("errorString");

    QTest::newRow("invalid") << false << QMediaPlayer::StoppedState << QMediaPlayer::UnknownMediaStatus <<
                                QMediaSource() << qint64(0) << qint64(0) << false << 0 << false << false << 0 <<
                                float(0) << QMediaPlayer::NoError << QString();
    QTest::newRow("valid+null") << true << QMediaPlayer::StoppedState << QMediaPlayer::UnknownMediaStatus <<
                                QMediaSource() << qint64(0) << qint64(0) << false << 0 << false << false << 50 <<
                                float(0) << QMediaPlayer::NoError << QString();
    QTest::newRow("valid+content+stopped") << true << QMediaPlayer::StoppedState << QMediaPlayer::UnknownMediaStatus <<
                                QMediaSource(QUrl("file:///some.mp3")) << qint64(0) << qint64(0) << false << 50 << false << false << 0 <<
                                float(1) << QMediaPlayer::NoError << QString();
    QTest::newRow("valid+content+playing") << true << QMediaPlayer::PlayingState << QMediaPlayer::LoadedMedia <<
                                QMediaSource(QUrl("file:///some.mp3")) << qint64(10000) << qint64(10) << true << 50 << true << false << 0 <<
                                float(1) << QMediaPlayer::NoError << QString();
    QTest::newRow("valid+content+paused") << true << QMediaPlayer::PausedState << QMediaPlayer::LoadedMedia <<
                                QMediaSource(QUrl("file:///some.mp3")) << qint64(10000) << qint64(10) << true << 50 << true << false << 0 <<
                                float(1)  << QMediaPlayer::NoError << QString();
    QTest::newRow("valud+streaming") << true << QMediaPlayer::PlayingState << QMediaPlayer::LoadedMedia <<
                                QMediaSource(QUrl("http://example.com/stream")) << qint64(10000) << qint64(10000) << false << 50 << false << true << 0 <<
                                float(1)  << QMediaPlayer::NoError << QString();
    QTest::newRow("valid+error") << true << QMediaPlayer::StoppedState << QMediaPlayer::UnknownMediaStatus <<
                                QMediaSource(QUrl("http://example.com/stream")) << qint64(0) << qint64(0) << false << 50 << false << false << 0 <<
                                float(0) << QMediaPlayer::ResourceError << QString("Resource unavailable");
}

void tst_QMediaPlayer::initTestCase()
{
    mockService = new MockPlayerService;
    mockProvider = new MockProvider(mockService);
    player = new QMediaPlayer(0, mockProvider);
}

void tst_QMediaPlayer::cleanupTestCase()
{
    delete player;
}

void tst_QMediaPlayer::init()
{
    mockService->reset();
}

void tst_QMediaPlayer::cleanup()
{
}

void tst_QMediaPlayer::testValid()
{
    /*
    QFETCH_GLOBAL(bool, valid);

    mockService->setIsValid(valid);
    QCOMPARE(player->isValid(), valid);
    */
}

void tst_QMediaPlayer::testMedia()
{
    QFETCH_GLOBAL(QMediaSource, mediaSource);

    mockService->setMedia(mediaSource);
    QVERIFY(player->media() == mediaSource);
}

void tst_QMediaPlayer::testDuration()
{
    QFETCH_GLOBAL(qint64, duration);

    mockService->setDuration(duration);
    QVERIFY(player->duration() == duration);
}

void tst_QMediaPlayer::testPosition()
{
    QFETCH_GLOBAL(bool, valid);
    QFETCH_GLOBAL(bool, seekable);
    QFETCH_GLOBAL(qint64, position);
    QFETCH_GLOBAL(qint64, duration);

    mockService->setIsValid(valid);
    mockService->setSeekable(seekable);
    mockService->setPosition(position);
    mockService->setDuration(duration);
    QVERIFY(player->isSeekable() == seekable);
    QVERIFY(player->position() == position);
    QVERIFY(player->duration() == duration);

    if (seekable) {
        { QSignalSpy spy(player, SIGNAL(positionChanged(qint64)));
        player->setPosition(position);
        QCOMPARE(player->position(), position);
        QCOMPARE(spy.count(), 0); }

        mockService->setPosition(position);
        { QSignalSpy spy(player, SIGNAL(positionChanged(qint64)));
        player->setPosition(0);
        QCOMPARE(player->position(), qint64(0));
        QCOMPARE(spy.count(), position == 0 ? 0 : 1); }

        mockService->setPosition(position);
        { QSignalSpy spy(player, SIGNAL(positionChanged(qint64)));
        player->setPosition(duration);
        QCOMPARE(player->position(), duration);
        QCOMPARE(spy.count(), position == duration ? 0 : 1); }

        mockService->setPosition(position);
        { QSignalSpy spy(player, SIGNAL(positionChanged(qint64)));
        player->setPosition(-1);
        QCOMPARE(player->position(), qint64(0));
        QCOMPARE(spy.count(), position == 0 ? 0 : 1); }

        mockService->setPosition(position);
        { QSignalSpy spy(player, SIGNAL(positionChanged(qint64)));
        player->setPosition(duration + 1);
        QCOMPARE(player->position(), duration);
        QCOMPARE(spy.count(), position == duration ? 0 : 1); }
    }
    else {
        QSignalSpy spy(player, SIGNAL(positionChanged(qint64)));
        player->setPosition(position);

        QCOMPARE(player->position(), position);
        QCOMPARE(spy.count(), 0);
    }
}

void tst_QMediaPlayer::testVolume()
{
    QFETCH_GLOBAL(bool, valid);
    QFETCH_GLOBAL(int, volume);

    mockService->setVolume(volume);
    QVERIFY(player->volume() == volume);

    if (valid) {
        { QSignalSpy spy(player, SIGNAL(volumeChanged(int)));
        player->setVolume(10);
        QCOMPARE(player->volume(), 10);
        QCOMPARE(spy.count(), 1); }

        { QSignalSpy spy(player, SIGNAL(volumeChanged(int)));
        player->setVolume(-1000);
        QCOMPARE(player->volume(), 0);
        QCOMPARE(spy.count(), 1); }

        { QSignalSpy spy(player, SIGNAL(volumeChanged(int)));
        player->setVolume(100);
        QCOMPARE(player->volume(), 100);
        QCOMPARE(spy.count(), 1); }

        { QSignalSpy spy(player, SIGNAL(volumeChanged(int)));
        player->setVolume(1000);
        QCOMPARE(player->volume(), 100);
        QCOMPARE(spy.count(), 0); }
    }
}

void tst_QMediaPlayer::testMuted()
{
    QFETCH_GLOBAL(bool, muted);

    mockService->setMuted(muted);
    QVERIFY(player->isMuted() == muted);
}

void tst_QMediaPlayer::testVideoAvailable()
{
    QFETCH_GLOBAL(bool, videoAvailable);

    mockService->setVideoAvailable(videoAvailable);
    QVERIFY(player->isVideoAvailable() == videoAvailable);
}

void tst_QMediaPlayer::testBufferStatus()
{
    QFETCH_GLOBAL(int, bufferStatus);

    mockService->setBufferStatus(bufferStatus);
    QVERIFY(player->bufferStatus() == bufferStatus);
}

void tst_QMediaPlayer::testSeekable()
{
    QFETCH_GLOBAL(bool, seekable);

    mockService->setSeekable(seekable);
    QVERIFY(player->isSeekable() == seekable);
}

void tst_QMediaPlayer::testPlaybackRate()
{
    QFETCH_GLOBAL(float, playbackRate);

    mockService->setPlaybackRate(playbackRate);
    QVERIFY(player->playbackRate() == playbackRate);
}

void tst_QMediaPlayer::testError()
{
    QFETCH_GLOBAL(QMediaPlayer::Error, error);

    mockService->setError(error);
    QVERIFY(player->error() == error);
}

void tst_QMediaPlayer::testErrorString()
{
    QFETCH_GLOBAL(QString, errorString);

    mockService->setErrorString(errorString);
    QVERIFY(player->errorString() == errorString);
}

void tst_QMediaPlayer::testService()
{
    /*
    QFETCH_GLOBAL(bool, valid);

    mockService->setIsValid(valid);

    if (valid)
        QVERIFY(player->service() != 0);
    else
        QVERIFY(player->service() == 0);
        */
}

void tst_QMediaPlayer::testPlay()
{
    QFETCH_GLOBAL(bool, valid);
    QFETCH_GLOBAL(QMediaSource, mediaSource);
    QFETCH_GLOBAL(QMediaPlayer::State, state);

    mockService->setIsValid(valid);
    mockService->setState(state);
    mockService->setMedia(mediaSource);
    QVERIFY(player->state() == state);
    QVERIFY(player->media() == mediaSource);

    QSignalSpy spy(player, SIGNAL(stateChanged(QMediaPlayer::State)));

    player->play();

    if (!valid || mediaSource.isNull())  {
        QCOMPARE(player->state(), QMediaPlayer::StoppedState);
        QCOMPARE(spy.count(), 0);
    }
    else {
        QCOMPARE(player->state(), QMediaPlayer::PlayingState);
        QCOMPARE(spy.count(), state == QMediaPlayer::PlayingState ? 0 : 1);
    }
}

void tst_QMediaPlayer::testPause()
{
    QFETCH_GLOBAL(bool, valid);
    QFETCH_GLOBAL(QMediaSource, mediaSource);
    QFETCH_GLOBAL(QMediaPlayer::State, state);

    mockService->setIsValid(valid);
    mockService->setState(state);
    mockService->setMedia(mediaSource);
    QVERIFY(player->state() == state);
    QVERIFY(player->media() == mediaSource);

    QSignalSpy spy(player, SIGNAL(stateChanged(QMediaPlayer::State)));

    player->pause();

    if (!valid || mediaSource.isNull()) {
        QCOMPARE(player->state(), QMediaPlayer::StoppedState);
        QCOMPARE(spy.count(), 0);
    }
    else {
        if (state == QMediaPlayer::StoppedState) {
            QCOMPARE(player->state(), QMediaPlayer::StoppedState);
            QCOMPARE(spy.count(), 0);
        }
        else {
            QCOMPARE(player->state(), QMediaPlayer::PausedState);
            QCOMPARE(spy.count(), state == QMediaPlayer::PausedState ? 0 : 1);
        }
    }
}

void tst_QMediaPlayer::testStop()
{
    QFETCH_GLOBAL(QMediaSource, mediaSource);
    QFETCH_GLOBAL(QMediaPlayer::State, state);

    mockService->setState(state);
    mockService->setMedia(mediaSource);
    QVERIFY(player->state() == state);
    QVERIFY(player->media() == mediaSource);

    QSignalSpy spy(player, SIGNAL(stateChanged(QMediaPlayer::State)));

    player->stop();

    if (mediaSource.isNull() || state == QMediaPlayer::StoppedState) {
        QCOMPARE(player->state(), QMediaPlayer::StoppedState);
        QCOMPARE(spy.count(), 0);
    }
    else {
        QCOMPARE(player->state(), QMediaPlayer::StoppedState);
        QCOMPARE(spy.count(), 1);
    }
}

QTEST_MAIN(tst_QMediaPlayer)

#include "tst_qmediaplayer.moc"
