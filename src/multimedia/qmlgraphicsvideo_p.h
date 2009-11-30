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

#ifndef QMLGRAPHICSVIDEO_H
#define QMLGRAPHICSVIDEO_H

#include <qmediaplayer.h>

#include <QtCore/qbasictimer.h>
#include <QtDeclarative/qmlgraphicsitem.h>

class QTimerEvent;
class QVideoSurfaceFormat;

QTM_BEGIN_NAMESPACE

class QMediaPlayerControl;
class QMediaService;
class QMediaServiceProvider;
class QPainterVideoSurface;
class QVideoOutputControl;
class QVideoRendererControl;

class QmlGraphicsVideoPrivate;

class Q_MEDIA_EXPORT QmlGraphicsVideo : public QmlGraphicsItem
{
    Q_OBJECT
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(State state READ state WRITE setState NOTIFY stateChanged)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(qreal duration READ duration NOTIFY durationChanged)
    Q_PROPERTY(qreal position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(qreal volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(bool muted READ isMuted WRITE setMuted NOTIFY mutingChanged)
    Q_PROPERTY(bool videoAvailable READ isVideoAvailable NOTIFY videoAvailableChanged)
    Q_PROPERTY(int bufferStatus READ bufferStatus NOTIFY bufferStatusChanged)
    Q_PROPERTY(bool seekable READ isSeekable NOTIFY seekableChanged)
    Q_PROPERTY(qreal playbackRate READ playbackRate WRITE setPlaybackRate NOTIFY playbackRateChanged)
    Q_PROPERTY(Error error READ error NOTIFY errorChanged)
    Q_PROPERTY(QString errorString READ errorString NOTIFY errorStringChanged)
    Q_PROPERTY(FillMode fillMode READ fillMode WRITE setFillMode)
    Q_ENUMS(State)
    Q_ENUMS(Status)
    Q_ENUMS(Error)
    Q_ENUMS(FillMode)
public:
    enum State
    {
        Stopped = QMediaPlayer::StoppedState,
        Playing = QMediaPlayer::PlayingState,
        Paused  = QMediaPlayer::PausedState
    };

    enum Status
    {
        UnknownStatus = QMediaPlayer::UnknownMediaStatus,
        NoMedia       = QMediaPlayer::NoMedia,
        Loading       = QMediaPlayer::LoadingMedia,
        Loaded        = QMediaPlayer::LoadedMedia,
        Stalled       = QMediaPlayer::StalledMedia,
        Buffering     = QMediaPlayer::BufferingMedia,
        Buffered      = QMediaPlayer::BufferedMedia,
        EndOfMedia    = QMediaPlayer::EndOfMedia,
        InvalidMedia  = QMediaPlayer::InvalidMedia
    };

    enum Error
    {
        NoError        = QMediaPlayer::NoError,
        ResourceError  = QMediaPlayer::ResourceError,
        FormatError    = QMediaPlayer::FormatError,
        NetworkError   = QMediaPlayer::NetworkError,
        AccessDenied   = QMediaPlayer::AccessDeniedError,
        ServiceMissing = QMediaPlayer::ServiceMissingError
    };

    enum FillMode
    {
        Stretch,
        PreserveAspectFit,
        PreserveAspectCrop
    };

    QmlGraphicsVideo(QmlGraphicsItem *parent = 0);
    ~QmlGraphicsVideo();

    QUrl source() const;
    void setSource(const QUrl &url);

    State state() const;
    void setState(State state);

    Status status() const;

    qreal duration() const;

    qreal position() const;
    void setPosition(qreal position);

    qreal volume() const;
    void setVolume(qreal volume);

    bool isMuted() const;
    void setMuted(bool muted);

    bool isVideoAvailable() const;

    int bufferStatus() const;

    bool isSeekable() const;

    qreal playbackRate() const;
    void setPlaybackRate(qreal rate);

    Error error() const;
    QString errorString() const;

    FillMode fillMode() const;
    void setFillMode(FillMode mode);

    void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);

Q_SIGNALS:
    void sourceChanged(const QUrl &url);

    void stateChanged(QmlGraphicsVideo::State state);
    void statusChanged(QmlGraphicsVideo::Status status);

    void durationChanged(qreal duration);
    void positionChanged(qreal position);

    void volumeChanged(qreal volume);
    void mutingChanged(bool muted);
    void videoAvailableChanged(bool available);

    void bufferStatusChanged(int percentage);

    void seekableChanged(bool seekable);
    void playbackRateChanged(qreal rate);

    void errorChanged(QmlGraphicsVideo::Error error);
    void errorStringChanged(const QString &error);

protected:
    void timerEvent(QTimerEvent *event);
    void geometryChanged(const QRectF &geometry, const QRectF &);

private Q_SLOTS:
    void _q_stateChanged(QMediaPlayer::State state);
    void _q_mediaStatusChanged(QMediaPlayer::MediaStatus status);
    void _q_mediaChanged(const QMediaContent &media);
    void _q_durationChanged(qint64 duration);
    void _q_positionChanged(qint64 position);
    void _q_volumeChanged(int volume);
    void _q_error(QMediaPlayer::Error error, const QString &errorString);

    void _q_videoSurfaceFormatChanged(const QVideoSurfaceFormat &format);
    void _q_frameChanged();

private:
    Q_DISABLE_COPY(QmlGraphicsVideo)

    void updateVideoRect(const QRectF &geometry, const QSize &videoSize);

    QMediaServiceProvider *m_mediaProvider;
    QMediaService *m_mediaService;
    QMediaPlayerControl *m_playerControl;
    QVideoOutputControl *m_videoOutputControl;
    QVideoRendererControl *m_videoRendererControl;
    QPainterVideoSurface *m_videoSurface;
    QBasicTimer m_notifyTimer;

    State m_state;
    Status m_status;
    Error m_error;
    FillMode m_fillMode;
    QString m_errorString;
    QRectF m_scaledRect;
    bool m_updatePaintDevice;
};

QTM_END_NAMESPACE

QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QmlGraphicsVideo))

#endif
