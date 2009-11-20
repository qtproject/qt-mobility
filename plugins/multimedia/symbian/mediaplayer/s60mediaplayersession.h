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

#ifndef S60MEDIAPLAYERSESSION_H
#define S60MEDIAPLAYERSESSION_H

#include <QObject>
#include <QUrl>
#include <QSize>
#include <QMediaPlayer>
#include <QPair>
class QTimer;

class S60MediaPlayerSession : public QObject
{
    Q_OBJECT

public:
    S60MediaPlayerSession(QObject *parent);
    virtual ~S60MediaPlayerSession();

    virtual QUrl url()const { return m_url; };
    virtual QMediaPlayer::State state() const { return m_state; }
    virtual QMediaPlayer::MediaStatus mediaStatus() const { return m_mediaStatus; }

    virtual qint64 duration() const = 0;
    virtual qint64 position() const = 0;
    
    int volume() const;
    bool isMuted() const;

    virtual void setVideoRenderer(QObject *renderer);
    virtual bool isVideoAvailable() const;

    bool isSeekable() const;
    virtual QPair<qint64, qint64> seekRange() const;

    qreal playbackRate() const;
    virtual void setPlaybackRate(qreal rate) = 0 ;
   
    bool isMetadataAvailable() const; 
    QVariant metaData(const QString& key) const;
        
    virtual void load(const QUrl &url) = 0;

    virtual void play() = 0;
    virtual void pause() = 0;
    virtual void stop() = 0;

    virtual void setPosition(qint64 pos) = 0;

    virtual void setVolume(int volume);
    virtual void setMuted(bool muted) = 0;

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
    void metaDataChanged();
    
protected slots:
    void tick();
    bool startTimer();
    void stopTimer();

protected:
    void setMediaStatus(QMediaPlayer::MediaStatus);

    qint64 m_totalTime;
    QUrl m_url;
    QMediaPlayer::State m_state;
    QMediaPlayer::MediaStatus m_mediaStatus;

    qreal m_playbackRate;
    bool m_muted;
    bool m_videoAvailable;

    qint64 m_lastPosition;
    qint64 m_duration;

    QMap<QString, QVariant> m_metaDataMap;
    QTimer* m_timer;
    int m_volume;
    bool m_playCalled;
};

#endif
