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

#ifndef S60MEDIAPLAYERSESSION_H
#define S60MEDIAPLAYERSESSION_H

#include <QObject>
#include <QUrl>
#include <QSize>
#include "s60videoplayercontrol.h"
#include "s60videowidget.h"
#include <multimedia/qmediaplayer.h>
#include <VideoPlayer.h>

class S60VideoPlayerSession : public QObject, public MVideoPlayerUtilityObserver
{
Q_OBJECT

public:
    S60VideoPlayerSession(QObject *parent);
    virtual ~S60VideoPlayerSession();

    QUrl url() const;

    QMediaPlayer::State state() const { return m_state; }
    QMediaPlayer::MediaStatus mediaStatus() const { return m_mediaStatus; }

    qint64 duration() const;
    qint64 position() const;

    bool isBuffering() const;

    int bufferingProgress() const;

    int volume() const;
    bool isMuted() const;

    void setVideoRenderer(QObject *renderer);
    bool isVideoAvailable() const;

    bool isSeekable() const;

    qreal playbackRate() const;
    void setPlaybackRate(qreal rate);

    QMap<QByteArray ,QVariant> tags() const { return m_tags; }
    QMap<QString,QVariant> streamProperties(int streamNumber) const { return m_streamProperties[streamNumber]; }
    int streamCount() const { return m_streamProperties.count(); }
    
public slots:
    void load(const QUrl &url);

    void play();
    void pause();
    void stop();

    void seek(qint64 pos);

    void setVolume(int volume);
    void setMuted(bool muted);

signals:
    void durationChanged(qint64 duration);
    void positionChanged(qint64 position);
    void stateChanged(QMediaPlayer::State state);
    void mediaStatusChanged(QMediaPlayer::MediaStatus mediaStatus);
    void volumeChanged(int volume);
    void mutedStateChaned(bool muted);
    void videoAvailabilityChanged(bool videoAvailable);
    void bufferingChanged(bool buffering);
    void bufferingProgressChanged(int percentFilled);
    void playbackFinished();
    void tagsChanged();
    void seekableChanged(bool);

private: 
    void getNativeHandles();
    
private: // From MVideoPlayerUtilityObserver
    void MvpuoOpenComplete(TInt aError);
    void MvpuoPrepareComplete(TInt aError);
    void MvpuoFrameReady(CFbsBitmap &aFrame, TInt aError);
    void MvpuoPlayComplete(TInt aError);
    void MvpuoEvent(const TMMFEvent &aEvent);
    
private slots:
    void getStreamsInfo();
    void setSeekable(bool);

private:
    void setMediaStatus(QMediaPlayer::MediaStatus);

    CVideoPlayerUtility* m_player;
    S60VideoWidgetControl* m_testWidget;
    RWsSession* m_wsSession;
    CWsScreenDevice* m_screenDevice;
    RWindowBase* m_window;
    TRect m_windowRect;
    TRect m_clipRect;
    QSize m_frameSize;
    qint64 m_totalTime;    
    QUrl m_url;
    QMediaPlayer::State m_state;
    QMediaPlayer::MediaStatus m_mediaStatus;
    QMap<QByteArray, QVariant> m_tags;
    QList< QMap<QString,QVariant> > m_streamProperties;
    
    int m_volume;
    qreal m_playbackRate;
    bool m_muted;
    bool m_videoAvailable;
    bool m_seekable;

    qint64 m_lastPosition;
    qint64 m_duration;
};

#endif
