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
#ifndef TST_QMEDIAPLAYER_H
#define TST_QMEDIAPLAYER_H

#include <QtTest/QtTest>
#include <QtCore/qdebug.h>
#include <QtCore/qbuffer.h>

#include <qmediaplayer.h>
#include <qmediaplayercontrol.h>
#include <qmediaplaylist.h>
#include <qmediaservice.h>
#include <qmediastreamscontrol.h>

QT_USE_NAMESPACE

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
    void setMuted(bool muted) { if (muted != _muted) emit mutedChanged(_muted = muted); }

    int bufferStatus() const { return _bufferStatus; }

    bool isAudioAvailable() const { return _audioAvailable; }
    bool isVideoAvailable() const { return _videoAvailable; }

    bool isSeekable() const { return _isSeekable; }
    QMediaTimeRange availablePlaybackRanges() const { return QMediaTimeRange(_seekRange.first, _seekRange.second); }
    void setSeekRange(qint64 minimum, qint64 maximum) { _seekRange = qMakePair(minimum, maximum); }

    qreal playbackRate() const { return _playbackRate; }
    void setPlaybackRate(qreal rate) { if (rate != _playbackRate) emit playbackRateChanged(_playbackRate = rate); }

    QMediaContent media() const { return _media; }
    void setMedia(const QMediaContent &content, QIODevice *stream)
    {
        _stream = stream;
        _media = content;
        if (_state != QMediaPlayer::StoppedState) {
            _mediaStatus = _media.isNull() ? QMediaPlayer::NoMedia : QMediaPlayer::LoadingMedia;
            emit stateChanged(_state = QMediaPlayer::StoppedState);
            emit mediaStatusChanged(_mediaStatus);
        }
        emit mediaChanged(_media = content);
    }
    QIODevice *mediaStream() const { return _stream; }

    void play() { if (_isValid && !_media.isNull() && _state != QMediaPlayer::PlayingState) emit stateChanged(_state = QMediaPlayer::PlayingState); }
    void pause() { if (_isValid && !_media.isNull() && _state != QMediaPlayer::PausedState) emit stateChanged(_state = QMediaPlayer::PausedState); }
    void stop() { if (_state != QMediaPlayer::StoppedState) emit stateChanged(_state = QMediaPlayer::StoppedState); }

    QMediaPlayer::State _state;
    QMediaPlayer::MediaStatus _mediaStatus;
    QMediaPlayer::Error _error;
    qint64 _duration;
    qint64 _position;
    int _volume;
    bool _muted;
    int _bufferStatus;
    bool _audioAvailable;
    bool _videoAvailable;
    bool _isSeekable;
    QPair<qint64, qint64> _seekRange;
    qreal _playbackRate;
    QMediaContent _media;
    QIODevice *_stream;
    bool _isValid;
    QString _errorString;
};

class MockStreamsControl : public QMediaStreamsControl
{
public:
    MockStreamsControl(QObject *parent = 0) : QMediaStreamsControl(parent) {}

    int streamCount() { return _streams.count(); }
    void setStreamCount(int count) { _streams.resize(count); }

    StreamType streamType(int index) { return _streams.at(index).type; }
    void setStreamType(int index, StreamType type) { _streams[index].type = type; }

    QVariant metaData(int index, QtMultimediaKit::MetaData key) {
        return _streams.at(index).metaData.value(key); }
    void setMetaData(int index, QtMultimediaKit::MetaData key, const QVariant &value) {
        _streams[index].metaData.insert(key, value); }

    bool isActive(int index) { return _streams.at(index).active; }
    void setActive(int index, bool state) { _streams[index].active = state; }

private:
    struct Stream
    {
        Stream() : type(UnknownStream), active(false) {}
        StreamType type;
        QMap<QtMultimediaKit::MetaData, QVariant> metaData;
        bool active;
    };

    QVector<Stream> _streams;
};

class MockPlayerService : public QMediaService
{
    Q_OBJECT

public:
    MockPlayerService():QMediaService(0)
    {
        mockControl = new MockPlayerControl;
        mockStreamsControl = new MockStreamsControl;
    }

    ~MockPlayerService()
    {
        delete mockControl;
        delete mockStreamsControl;
    }

    QMediaControl* requestControl(const char *iid)
    {
        if (qstrcmp(iid, QMediaPlayerControl_iid) == 0)
            return mockControl;

        return 0;
    }

    void releaseControl(QMediaControl *)
    {
    }

    void setState(QMediaPlayer::State state) { emit mockControl->stateChanged(mockControl->_state = state); }
    void setState(QMediaPlayer::State state, QMediaPlayer::MediaStatus status) {
        mockControl->_state = state;
        mockControl->_mediaStatus = status;
        emit mockControl->mediaStatusChanged(status);
        emit mockControl->stateChanged(state);
    }
    void setMediaStatus(QMediaPlayer::MediaStatus status) { emit mockControl->mediaStatusChanged(mockControl->_mediaStatus = status); }
    void setIsValid(bool isValid) { mockControl->_isValid = isValid; }
    void setMedia(QMediaContent media) { mockControl->_media = media; }
    void setDuration(qint64 duration) { mockControl->_duration = duration; }
    void setPosition(qint64 position) { mockControl->_position = position; }
    void setSeekable(bool seekable) { mockControl->_isSeekable = seekable; }
    void setVolume(int volume) { mockControl->_volume = volume; }
    void setMuted(bool muted) { mockControl->_muted = muted; }
    void setVideoAvailable(bool videoAvailable) { mockControl->_videoAvailable = videoAvailable; }
    void setBufferStatus(int bufferStatus) { mockControl->_bufferStatus = bufferStatus; }
    void setPlaybackRate(qreal playbackRate) { mockControl->_playbackRate = playbackRate; }
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
        mockControl->_media = QMediaContent();
        mockControl->_stream = 0;
        mockControl->_isValid = false;
        mockControl->_errorString = QString();
    }

    MockPlayerControl *mockControl;
    MockStreamsControl *mockStreamsControl;
};

class MockProvider : public QMediaServiceProvider
{
public:
    MockProvider(MockPlayerService *service):mockService(service) {}
    QMediaService *requestService(const QByteArray &, const QMediaServiceProviderHint &)
    {
        return mockService;
    }

    void releaseService(QMediaService *service) { delete service; }

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
    void testNullService();
    void testValid();
    void testMedia();
    void testDuration();
    void testPosition();
    void testVolume();
    void testMuted();
    void testIsAvailable();
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
    void testMediaStatus();
    void testPlaylist();

private:
    MockProvider *mockProvider;
    MockPlayerService  *mockService;
    QMediaPlayer *player;
};

#endif //TST_QMEDIAPLAYER_H
