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

#include <qmlmediabase_p.h>

#include <QtCore/qbasictimer.h>
#include <QtDeclarative/qmlgraphicsitem.h>

class QTimerEvent;
class QVideoSurfaceFormat;

QTM_BEGIN_NAMESPACE

class QPainterVideoSurface;
class QVideoOutputControl;
class QVideoRendererControl;

class QmlGraphicsVideo : public QmlGraphicsItem, public QmlMediaBase
{
    Q_OBJECT
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(bool playing READ isPlaying WRITE setPlaying NOTIFY playingChanged)
    Q_PROPERTY(bool paused READ isPaused WRITE setPaused NOTIFY pausedChanged)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(int duration READ duration NOTIFY durationChanged)
    Q_PROPERTY(int position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(qreal volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(bool muted READ isMuted WRITE setMuted NOTIFY mutedChanged)
    Q_PROPERTY(bool videoAvailable READ isVideoAvailable NOTIFY videoAvailableChanged)
    Q_PROPERTY(int bufferStatus READ bufferStatus NOTIFY bufferStatusChanged)
    Q_PROPERTY(bool seekable READ isSeekable NOTIFY seekableChanged)
    Q_PROPERTY(qreal playbackRate READ playbackRate WRITE setPlaybackRate NOTIFY playbackRateChanged)
    Q_PROPERTY(Error error READ error NOTIFY errorChanged)
    Q_PROPERTY(QString errorString READ errorString NOTIFY errorChanged)
    Q_PROPERTY(FillMode fillMode READ fillMode WRITE setFillMode)
    Q_ENUMS(FillMode)
    Q_ENUMS(Status)
    Q_ENUMS(Error)
public:
    enum FillMode
    {
        Stretch,
        PreserveAspectFit,
        PreserveAspectCrop
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

    QmlGraphicsVideo(QmlGraphicsItem *parent = 0);
    ~QmlGraphicsVideo();

    bool isVideoAvailable() const;

    FillMode fillMode() const;
    void setFillMode(FillMode mode);

    Status status() const;
    Error error() const;

    void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);

public Q_SLOTS:
    void play();
    void pause();
    void stop();

Q_SIGNALS:
    void sourceChanged();

    void playingChanged();
    void pausedChanged();

    void statusChanged();

    void durationChanged();
    void positionChanged();

    void volumeChanged();
    void mutedChanged();
    void videoAvailableChanged();

    void bufferStatusChanged();

    void seekableChanged();
    void playbackRateChanged();

    void errorChanged();

protected:
    void geometryChanged(const QRectF &geometry, const QRectF &);

private Q_SLOTS:
    void _q_videoSurfaceFormatChanged(const QVideoSurfaceFormat &);
    void _q_frameChanged();

private:
    Q_DISABLE_COPY(QmlGraphicsVideo)

    void updateVideoRect(const QRectF &geometry, const QSize &videoSize);

    QVideoOutputControl *m_videoOutputControl;
    QVideoRendererControl *m_videoRendererControl;
    QPainterVideoSurface *m_videoSurface;

    FillMode m_fillMode;
    QRectF m_scaledRect;
    bool m_updatePaintDevice;

    Q_PRIVATE_SLOT(mediaBase(), void _q_stateChanged(QMediaPlayer::State))
    Q_PRIVATE_SLOT(mediaBase(), void _q_mediaStatusChanged(QMediaPlayer::MediaStatus))
    Q_PRIVATE_SLOT(mediaBase(), void _q_error(QMediaPlayer::Error, const QString &))
    Q_PRIVATE_SLOT(mediaBase(), void _q_metaDataChanged())

    inline QmlMediaBase *mediaBase() { return this; }
};

QTM_END_NAMESPACE

QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QmlGraphicsVideo))

#endif
